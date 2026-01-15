#include "lobby_manager.h"

#include "game_engine/engine.h"
#include "utils/logger.h"

#include <chrono>
#include <iostream>

// GameLobby implementation
GameLobby::GameLobby(std::uint32_t lobby_id, const std::string& lobby_name, std::uint8_t max_players,
                     std::uint16_t port, engn::EngineContext& engine_ctx)
    : m_lobby_id(lobby_id), m_lobby_name(lobby_name), m_max_players(max_players), m_port(port),
      m_engine_ctx(engine_ctx), m_current_players(0) {}

GameLobby::~GameLobby() {
    stop();
}

void GameLobby::start() {
    if (m_running.load()) {
        return;
    }

    m_running = true;
    m_lobby_thread = std::thread(&GameLobby::lobby_thread_func, this);
    LOG_INFO("Lobby '{}' (ID: {}) started on port {}", m_lobby_name, m_lobby_id, m_port);
}

void GameLobby::stop() {
    if (!m_running.exchange(false)) {
        return;
    }

    if (m_server) {
        m_server->stop();
    }

    if (m_lobby_thread.joinable()) {
        m_lobby_thread.join();
    }

    LOG_INFO("Lobby '{}' (ID: {}) stopped", m_lobby_name, m_lobby_id);
}

void GameLobby::lobby_thread_func() {
    try {
        // Create a new engine context for this lobby's game
        engn::EngineContext lobby_engine_ctx;

        // Create the network server for this lobby
        m_server = std::make_unique<NetworkServer>(lobby_engine_ctx, m_port);
        m_server->start();
        m_server->get_engine().add_scene_loader("lobby", lobby_scene_loader);
        m_server->get_engine().set_scene("lobby");

        LOG_INFO("Lobby '{}' game server running on port {}", m_lobby_name, m_port);

        // Main lobby loop (runs the game)
        constexpr int k_tick_ms = 16; // ~60 FPS
        while (m_running.load()) {
            m_server->poll();
            m_server->get_engine().run_systems();
            std::this_thread::sleep_for(std::chrono::milliseconds(k_tick_ms));
        }

        m_server->stop();
    } catch (const std::exception& e) {
        LOG_ERROR("Lobby '{}' thread error: {}", m_lobby_name, e.what());
    }
}

bool GameLobby::is_full() const {
    return m_current_players.load() >= m_max_players;
}

bool GameLobby::can_join() const {
    return !is_full() && m_running.load();
}

void GameLobby::add_player(const asio::ip::udp::endpoint& endpoint) {
    std::lock_guard<std::mutex> lock(m_players_mutex);
    m_players.push_back(endpoint);
    m_current_players = static_cast<std::uint8_t>(m_players.size());
    LOG_INFO("Player joined lobby '{}'. Current players: {}/{}", m_lobby_name, m_current_players.load(), m_max_players);
}

void GameLobby::remove_player(const asio::ip::udp::endpoint& endpoint) {
    std::lock_guard<std::mutex> lock(m_players_mutex);
    auto it = std::find(m_players.begin(), m_players.end(), endpoint);
    if (it != m_players.end()) {
        m_players.erase(it);
        m_current_players = static_cast<std::uint8_t>(m_players.size());
        LOG_INFO("Player left lobby '{}'. Current players: {}/{}", m_lobby_name, m_current_players.load(),
                 m_max_players);
    }
}

// LobbyManager implementation
LobbyManager::LobbyManager(std::uint16_t base_lobby_port) : m_base_lobby_port(base_lobby_port) {}

LobbyManager::~LobbyManager() {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);
    for (auto& [id, lobby] : m_lobbies) {
        lobby->stop();
    }
    m_lobbies.clear();
}

std::uint32_t LobbyManager::create_lobby(const std::string& lobby_name, std::uint8_t max_players,
                                         engn::EngineContext& engine_ctx) {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);

    std::uint32_t lobby_id = m_next_lobby_id++;
    std::uint16_t port = allocate_port();

    auto lobby = std::make_shared<GameLobby>(lobby_id, lobby_name, max_players, port, engine_ctx);
    m_lobbies[lobby_id] = lobby;
    m_empty_ticks[lobby_id] = 0;

    lobby->start();

    LOG_INFO("Created lobby '{}' with ID {} on port {}", lobby_name, lobby_id, port);
    return lobby_id;
}

std::shared_ptr<GameLobby> LobbyManager::get_lobby(std::uint32_t lobby_id) {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);
    auto it = m_lobbies.find(lobby_id);
    if (it != m_lobbies.end()) {
        return it->second;
    }
    return nullptr;
}

void LobbyManager::remove_lobby(std::uint32_t lobby_id) {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);
    auto it = m_lobbies.find(lobby_id);
    if (it != m_lobbies.end()) {
        it->second->stop();
        m_lobbies.erase(it);
        LOG_INFO("Removed lobby ID {}", lobby_id);
    }
}

std::vector<net::lobby::LobbyInfo> LobbyManager::get_lobby_list() const {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);
    std::vector<net::lobby::LobbyInfo> result;
    result.reserve(m_lobbies.size());

    for (const auto& [id, lobby] : m_lobbies) {
        if (lobby->is_running()) {
            net::lobby::LobbyInfo info{};
            info.m_lobby_id = lobby->get_id();
            info.m_lobby_name = lobby->get_name();
            info.m_current_players = lobby->get_current_players();
            info.m_max_players = lobby->get_max_players();
            info.m_port = lobby->get_port();
            result.push_back(info);
        }
    }

    return result;
}

void LobbyManager::cleanup_empty_lobbies() {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);
    constexpr std::size_t k_empty_threshold_ticks = 300; // ~5 seconds at 60Hz

    std::vector<std::uint32_t> to_remove;

    for (const auto& [id, lobby] : m_lobbies) {
        if (lobby->get_current_players() == 0) {
            m_empty_ticks[id]++;
            if (m_empty_ticks[id] > k_empty_threshold_ticks) {
                to_remove.push_back(id);
            }
        } else {
            m_empty_ticks[id] = 0;
        }
    }

    for (std::uint32_t id : to_remove) {
        auto it = m_lobbies.find(id);
        if (it != m_lobbies.end()) {
            it->second->stop();
            m_lobbies.erase(it);
            m_empty_ticks.erase(id);
            LOG_INFO("Cleaned up empty lobby ID {}", id);
        }
    }
}

std::uint16_t LobbyManager::allocate_port() {
    return m_base_lobby_port + m_next_port_offset++;
}
