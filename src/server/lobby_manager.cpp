#include "lobby_manager.h"

#include "backend_api_client.h"
#include "game_engine/engine.h"
#include "lobby_ipc.h"
#include "scenes_loaders.h"
#include "utils/logger.h"

#include <chrono>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <signal.h>
    #include <sys/wait.h>
    #include <unistd.h>
#endif

namespace {
constexpr int k_shutdown_wait_ms = 100;
constexpr int k_heartbeat_interval_ticks = 60;
constexpr std::uint16_t k_backend_port = 8081;
} // namespace

// GameLobby implementation
GameLobby::GameLobby(std::uint32_t lobby_id, const std::string& lobby_name, std::uint8_t max_players,
                     std::uint16_t port)
    : m_lobby_id(lobby_id), m_lobby_name(lobby_name), m_max_players(max_players), m_port(port), m_current_players(0),
      m_process_handle(
#ifdef _WIN32
          NULL
#else
          -1
#endif
      ) {
}

GameLobby::~GameLobby() {
    stop();
}

void GameLobby::start() {
    if (m_running.load()) {
        return;
    }

    m_ipc = std::make_unique<ipc::LobbyIPC>(m_lobby_id);

    m_running = true;
    fork_and_run_lobby_process();

#ifdef _WIN32
    DWORD pid = m_process_handle ? GetProcessId(m_process_handle) : 0;
#else
    pid_t pid = m_process_handle;
#endif
    LOG_INFO("Lobby '{}' (ID: {}) started on port {} with PID {}", m_lobby_name, m_lobby_id, m_port, pid);
}

void GameLobby::stop() {
    if (!m_running.exchange(false)) {
        return;
    }

#ifdef _WIN32
    bool has_valid_process = (m_process_handle != NULL);
#else
    bool has_valid_process = (m_process_handle > 0);
#endif

    if (m_ipc && has_valid_process) {
        ipc::IPCMessage msg;
        msg.type = ipc::MessageType::SHUTDOWNREQ;
        msg.lobby_id = m_lobby_id;
        m_ipc->send_to_lobby(msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(k_shutdown_wait_ms));

        if (is_process_alive()) {
            LOG_WARNING("Lobby {} process didn't exit gracefully, terminating", m_lobby_id);
#ifdef _WIN32
            TerminateProcess(m_process_handle, 1);
#else
            kill(m_process_handle, SIGTERM);
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(k_shutdown_wait_ms));

            if (is_process_alive()) {
                LOG_WARNING("Lobby {} process didn't respond, force killing", m_lobby_id);
#ifdef _WIN32
                TerminateProcess(m_process_handle, 1);
#else
                kill(m_process_handle, SIGKILL);
#endif
            }
        }

#ifdef _WIN32
        WaitForSingleObject(m_process_handle, INFINITE);
        CloseHandle(m_process_handle);
        m_process_handle = NULL;
#else
        int status = 0;
        waitpid(m_process_handle, &status, 0);
        m_process_handle = -1;
#endif
    }

    if (m_ipc) {
        ipc::LobbyIPC::cleanup_lobby_ipc(m_lobby_id);
        m_ipc.reset();
    }

    LOG_INFO("Lobby '{}' (ID: {}) stopped", m_lobby_name, m_lobby_id);
}

void GameLobby::fork_and_run_lobby_process() {
#ifdef _WIN32
    // Windows implementation using CreateProcess
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi = {};
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);

    std::string cmd_line =
        std::string(exe_path) + " -islobby -lobby-id " + std::to_string(m_lobby_id) + " -p " + std::to_string(m_port);

    if (!CreateProcessA(NULL, const_cast<char*>(cmd_line.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        LOG_ERROR("Failed to create process for lobby {}: {}", m_lobby_id, GetLastError());
        m_running = false;
        throw std::runtime_error("CreateProcess failed");
    }

    m_process_handle = pi.hProcess;
    CloseHandle(pi.hThread);
#else
    // Unix implementation using fork
    pid_t pid = fork();

    if (pid < 0) {
        LOG_ERROR("Failed to fork process for lobby {}", m_lobby_id);
        m_running = false;
        throw std::runtime_error("Fork failed");
    }

    if (pid == 0) {
        // Child process
        run_lobby_in_child_process(m_lobby_id, m_lobby_name, m_port, m_max_players);
        _exit(0);
    }

    m_process_handle = pid;
#endif
}

void GameLobby::run_lobby_in_child_process(std::uint32_t lobby_id, const std::string& lobby_name, std::uint16_t port,
                                           std::uint8_t max_players) {
    try {
        LOG_INFO("Lobby '{}' process started (PID: {})", lobby_name,
#ifdef _WIN32
                 GetCurrentProcessId()
#else
                 getpid()
#endif
        );

        ipc::LobbyIPC ipc(lobby_id);

        engn::EngineContext lobby_engine_ctx;

        // Initialize backend API client for score sync
        engn::BackendAPIClient api_client("localhost", k_backend_port);
        lobby_engine_ctx.backend_api_client = &api_client;
        lobby_engine_ctx.current_lobby_id = lobby_id;

        auto server = std::make_unique<NetworkServer>(lobby_engine_ctx, port);
        server->start();
        server->get_engine().add_scene_loader("lobby", lobby_scene_loader);
        server->get_engine().set_scene("lobby");

        LOG_INFO("Lobby '{}' game server running on port {}", lobby_name, port);

        constexpr int k_tick_ms = 16;
        int heartbeat_counter = 0;
        int last_player_count = -1;

        while (!lobby_engine_ctx.should_quit) {
            server->get_engine().delta_time = k_tick_ms / 1000.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
            ipc::IPCMessage msg;
            if (ipc.try_receive_from_main(msg, 0)) {
                if (msg.type == ipc::MessageType::SHUTDOWNREQ) {
                    LOG_INFO("Lobby {} received shutdown request", lobby_id);
                    lobby_engine_ctx.should_quit = true;

                    ipc::IPCMessage ack;
                    ack.type = ipc::MessageType::SHUTDOWNACK;
                    ack.lobby_id = lobby_id;
                    ipc.send_to_main(ack);
                }
            }

            if (++heartbeat_counter >= k_heartbeat_interval_ticks) {
                ipc::IPCMessage heartbeat;
                heartbeat.type = ipc::MessageType::HEARTBEAT;
                heartbeat.lobby_id = lobby_id;
                ipc.send_to_main(heartbeat);
                heartbeat_counter = 0;
            }

            // Update player count via IPC if it changed
            int current_player_count = static_cast<int>(server->get_engine().get_clients().size());
            if (current_player_count != last_player_count) {
                ipc::IPCMessage count_msg;
                count_msg.type = ipc::MessageType::PLAYERCOUNT;
                count_msg.lobby_id = lobby_id;
                count_msg.data = static_cast<std::uint64_t>(current_player_count);
                ipc.send_to_main(count_msg);
                last_player_count = current_player_count;
                LOG_INFO("Lobby {} player count updated to {}", lobby_id, current_player_count);
            }

            server->poll();
            server->get_engine().run_systems();
            server->get_engine().registry.process_deferred_kills();
            std::this_thread::sleep_for(std::chrono::milliseconds(k_tick_ms));
        }

        server->stop();
        LOG_INFO("Lobby '{}' process exiting gracefully", lobby_name);

    } catch (const std::exception& e) {
        LOG_ERROR("Lobby '{}' process error: {}", lobby_name, e.what());
    }
}

bool GameLobby::is_full() const {
    return m_current_players.load() >= m_max_players;
}

bool GameLobby::is_process_alive() const {
#ifdef _WIN32
    if (m_process_handle == NULL) {
        return false;
    }
    DWORD exit_code;
    if (GetExitCodeProcess(m_process_handle, &exit_code)) {
        return exit_code == STILL_ACTIVE;
    }
    return false;
#else
    if (m_process_handle <= 0) {
        return false;
    }
    return kill(m_process_handle, 0) == 0;
#endif
}

bool GameLobby::can_join() const {
    return !is_full() && m_running.load();
}

void GameLobby::process_ipc_messages() {
    if (!m_ipc) {
        return;
    }

    ipc::IPCMessage msg;
    while (m_ipc->try_receive_from_lobby(msg, 0)) {
        switch (msg.type) {
            case ipc::MessageType::HEARTBEAT:
                LOG_DEBUG("Received heartbeat from lobby {}", msg.lobby_id);
                break;
            case ipc::MessageType::PLAYERCOUNT: {
                std::uint8_t new_count = static_cast<std::uint8_t>(msg.data);
                m_current_players = new_count;
                LOG_DEBUG("Lobby {} player count updated: {}", msg.lobby_id, msg.data);
                if (new_count == 0) {
                    std::lock_guard<std::mutex> lock(m_players_mutex);
                    m_players.clear();
                }
                break;
            }
            case ipc::MessageType::SHUTDOWNACK:
                LOG_INFO("Lobby {} acknowledged shutdown", msg.lobby_id);
                break;
            default:
                LOG_WARNING("Unknown IPC message type from lobby {}", msg.lobby_id);
                break;
        }
    }
}

void GameLobby::add_player(const std::string& player_ip) {
    std::lock_guard<std::mutex> lock(m_players_mutex);
    auto it = std::find(m_players.begin(), m_players.end(), player_ip);
    if (it == m_players.end()) {
        m_players.push_back(player_ip);
        m_current_players++;
        LOG_INFO("Player {} joined lobby '{}'. Current players: {}/{}", player_ip, m_lobby_name,
                 m_current_players.load(), m_max_players);
    } else {
        LOG_INFO("Player {} reconnected to lobby '{}'", player_ip, m_lobby_name);
    }
}

void GameLobby::remove_player(const std::string& player_ip) {
    std::lock_guard<std::mutex> lock(m_players_mutex);
    auto it = std::find(m_players.begin(), m_players.end(), player_ip);
    if (it != m_players.end()) {
        m_players.erase(it);
        if (m_current_players > 0) {
            m_current_players--;
        }
        LOG_INFO("Player {} left lobby '{}'. Current players: {}/{}", player_ip, m_lobby_name, m_current_players.load(),
                 m_max_players);
    }
}

// LobbyManager implementation
LobbyManager::LobbyManager(std::uint16_t base_lobby_port) : m_base_lobby_port(base_lobby_port) {
    // Initialize HTTP API client for backend communication
    // Backend expected to run on localhost:8081 (Node.js server)
    m_api_client = std::make_unique<engn::BackendAPIClient>("localhost", k_backend_port);
}

LobbyManager::~LobbyManager() {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);
    for (auto& [id, lobby] : m_lobbies) {
        lobby->stop();
    }
    m_lobbies.clear();
}

std::uint32_t LobbyManager::create_lobby(const std::string& lobby_name, std::uint8_t max_players) {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);

    std::uint32_t lobby_id = m_next_lobby_id++;
    std::uint16_t port = allocate_port();

    auto lobby = std::make_shared<GameLobby>(lobby_id, lobby_name, max_players, port);
    m_lobbies[lobby_id] = lobby;
    m_empty_ticks[lobby_id] = 0;

    lobby->start();

    LOG_INFO("Created lobby '{}' with ID {} on port {}", lobby_name, lobby_id, port);

    // Notify backend about the new lobby (async, don't block on failure)
    if (m_api_client) {
        auto backend_lobby_id = m_api_client->create_lobby(lobby_name, max_players);
        if (backend_lobby_id.has_value()) {
            LOG_INFO("Synced lobby to backend with ID {}", backend_lobby_id.value());
        } else {
            LOG_WARNING("Failed to sync lobby to backend: {}", m_api_client->get_last_error());
        }
    }

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
        // Finalize match data before destroying lobby
        if (m_api_client) {
            if (m_api_client->finalize_match(lobby_id)) {
                LOG_INFO("Finalized match data for lobby {}", lobby_id);
            } else {
                LOG_WARNING("Failed to finalize match data for lobby {}: {}", lobby_id, m_api_client->get_last_error());
            }
        }

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
        lobby->process_ipc_messages();

        if (!lobby->is_process_alive()) {
            LOG_WARNING("Lobby {} process died unexpectedly", id);
            to_remove.push_back(id);
            continue;
        }

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
            // This path handles lobbies whose process died or stayed empty.
            // Finalize backend state here as well so lobby/session rows are cleaned.
            if (m_api_client) {
                if (m_api_client->finalize_match(id)) {
                    LOG_INFO("Finalized match data for cleaned lobby {}", id);
                } else {
                    LOG_WARNING("Failed to finalize cleaned lobby {}: {}", id, m_api_client->get_last_error());
                }
            }

            it->second->stop();
            m_lobbies.erase(it);
            m_empty_ticks.erase(id);
            LOG_INFO("Cleaned up lobby ID {}", id);
        }
    }

#ifndef _WIN32
    // Reap zombie processes on Unix
    int status = 0;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        // Zombie reaped
    }
#endif
}

void LobbyManager::sync_player_counts() {
    if (!m_api_client) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_lobbies_mutex);
    for (const auto& [id, lobby] : m_lobbies) {
        std::uint8_t player_count = lobby->get_current_players();
        m_api_client->update_lobby_player_count(id, player_count);
    }
}

std::optional<std::uint32_t> LobbyManager::add_player_session(std::uint32_t lobby_id, const std::string& player_name,
                                                              std::optional<std::uint32_t> account_id,
                                                              const std::string& ip_address) {
    if (!m_api_client) {
        return std::nullopt;
    }

    std::optional<std::string> ip_opt = std::nullopt;
    if (!ip_address.empty()) {
        ip_opt = ip_address;
    }

    return m_api_client->add_player_session(lobby_id, player_name, account_id, ip_opt);
}

std::optional<engn::BackendAPIClient::BanCheckResult> LobbyManager::check_player_ban(const std::string& player_name) {
    if (!m_api_client) {
        return std::nullopt;
    }

    return m_api_client->check_player_ban(player_name);
}

std::uint16_t LobbyManager::allocate_port() {
    return m_base_lobby_port + m_next_port_offset++;
}
