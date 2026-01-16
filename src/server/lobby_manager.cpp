#include "lobby_manager.h"

#include "game_engine/engine.h"
#include "utils/logger.h"
#include "lobby_ipc.h"

#include <chrono>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <signal.h>
#endif

namespace {
    constexpr int k_shutdown_wait_ms = 100;
    constexpr int k_heartbeat_interval_ticks = 60;
}

// GameLobby implementation
GameLobby::GameLobby(std::uint32_t lobby_id, const std::string& lobby_name, std::uint8_t max_players,
                     std::uint16_t port)
    : m_lobby_id(lobby_id), m_lobby_name(lobby_name), m_max_players(max_players), m_port(port),
      m_current_players(0), m_process_handle(
#ifdef _WIN32
          NULL
#else
          -1
#endif
      ) {}

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

    std::string cmd_line = std::string(exe_path) + " -islobby -lobby-id " +
                           std::to_string(m_lobby_id) + " -p " + std::to_string(m_port);

    if (!CreateProcessA(NULL, const_cast<char*>(cmd_line.c_str()), NULL, NULL, FALSE,
                        0, NULL, NULL, &si, &pi)) {
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

void GameLobby::run_lobby_in_child_process(std::uint32_t lobby_id, const std::string& lobby_name,
                                           std::uint16_t port, std::uint8_t max_players) {
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

        auto server = std::make_unique<NetworkServer>(lobby_engine_ctx, port);
        server->start();

        LOG_INFO("Lobby '{}' game server running on port {}", lobby_name, port);

        constexpr int k_tick_ms = 16;
        bool running = true;
        int heartbeat_counter = 0;

        while (running) {
            ipc::IPCMessage msg;
            if (ipc.try_receive_from_main(msg, 0)) {
                if (msg.type == ipc::MessageType::SHUTDOWNREQ) {
                    LOG_INFO("Lobby {} received shutdown request", lobby_id);
                    running = false;

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
            server->poll();
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
            case ipc::MessageType::PLAYERCOUNT:
                m_current_players = static_cast<std::uint8_t>(msg.data);
                LOG_DEBUG("Lobby {} player count updated: {}", msg.lobby_id, msg.data);
                break;
            case ipc::MessageType::SHUTDOWNACK:
                LOG_INFO("Lobby {} acknowledged shutdown", msg.lobby_id);
                break;
            default:
                LOG_WARNING("Unknown IPC message type from lobby {}", msg.lobby_id);
                break;
        }
    }
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

std::uint32_t LobbyManager::create_lobby(const std::string& lobby_name, std::uint8_t max_players) {
    std::lock_guard<std::mutex> lock(m_lobbies_mutex);

    std::uint32_t lobby_id = m_next_lobby_id++;
    std::uint16_t port = allocate_port();

    auto lobby = std::make_shared<GameLobby>(lobby_id, lobby_name, max_players, port);
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

std::uint16_t LobbyManager::allocate_port() {
    return m_base_lobby_port + m_next_port_offset++;
}
