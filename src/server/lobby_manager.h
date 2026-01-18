#pragma once

#include "../networking/lobby/lobby_messages.h"
#include "network_server.h"
#include "lobby_ipc.h"
#include "backend_api_client.h"

#include <asio.hpp>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    using process_handle_t = HANDLE;
#else
    #include <sys/types.h>
    using process_handle_t = pid_t;
#endif

namespace engn {
class EngineContext;
}

// Lobby constants
namespace lobby_constants {
inline constexpr std::uint16_t k_default_lobby_base_port = 9000;
}

// Represents a single game lobby with its own thread and server
class GameLobby {
  public:
    GameLobby(std::uint32_t lobby_id, const std::string& lobby_name, std::uint8_t max_players, std::uint16_t port);
    ~GameLobby();

    GameLobby(const GameLobby&) = delete;
    GameLobby& operator=(const GameLobby&) = delete;
    GameLobby(GameLobby&&) = delete;
    GameLobby& operator=(GameLobby&&) = delete;

    void start();
    void stop();
    
    // Check if process is still alive
    bool is_process_alive() const;
    
    // Process IPC messages from lobby
    void process_ipc_messages();

    bool is_full() const;
    bool can_join() const;
    void add_player(const asio::ip::udp::endpoint& endpoint);
    void remove_player(const asio::ip::udp::endpoint& endpoint);

    std::uint32_t get_id() const {
        return m_lobby_id;
    }
    std::string get_name() const {
        return m_lobby_name;
    }
    std::uint8_t get_current_players() const {
        return m_current_players.load();
    }
    std::uint8_t get_max_players() const {
        return m_max_players;
    }
    std::uint16_t get_port() const {
        return m_port;
    }
    bool is_running() const {
        return m_running.load();
    }
    
    process_handle_t get_process_handle() const {
        return m_process_handle;
    }
    
    // Static function to run lobby in child process (public for Windows lobby mode)
    static void run_lobby_in_child_process(std::uint32_t lobby_id, const std::string& lobby_name, 
        std::uint16_t port, std::uint8_t max_players);

  private:
    void fork_and_run_lobby_process();

    std::uint32_t m_lobby_id;
    std::string m_lobby_name;
    std::uint8_t m_max_players;
    std::atomic<std::uint8_t> m_current_players{0};
    std::uint16_t m_port;

    process_handle_t m_process_handle;
    std::atomic<bool> m_running{false};
    std::unique_ptr<ipc::LobbyIPC> m_ipc;

    mutable std::mutex m_players_mutex;
    std::vector<asio::ip::udp::endpoint> m_players;
};

// Manages all lobbies and handles lobby-related requests
class LobbyManager {
  public:
    explicit LobbyManager(std::uint16_t base_lobby_port = lobby_constants::k_default_lobby_base_port);
    ~LobbyManager();

    LobbyManager(const LobbyManager&) = delete;
    LobbyManager& operator=(const LobbyManager&) = delete;
    LobbyManager(LobbyManager&&) = delete;
    LobbyManager& operator=(LobbyManager&&) = delete;

    // Create a new lobby
    std::uint32_t create_lobby(const std::string& lobby_name, std::uint8_t max_players);

    // Get lobby by ID
    std::shared_ptr<GameLobby> get_lobby(std::uint32_t lobby_id);

    // Remove a lobby (when empty and inactive)
    void remove_lobby(std::uint32_t lobby_id);

    // Get list of all active lobbies
    std::vector<net::lobby::LobbyInfo> get_lobby_list() const;

    // Clean up empty lobbies
    void cleanup_empty_lobbies();
    
    // Sync player counts to backend database
    void sync_player_counts();
    
    // Backend API integration helpers
    std::optional<std::uint32_t> add_player_session(
        std::uint32_t lobby_id,
        const std::string& player_name,
        std::optional<std::uint32_t> account_id = std::nullopt,
        const std::string& ip_address = ""
    );

  private:
    std::uint16_t allocate_port();

    mutable std::mutex m_lobbies_mutex;
    std::unordered_map<std::uint32_t, std::shared_ptr<GameLobby>> m_lobbies;
    std::unordered_map<std::uint32_t, std::size_t> m_empty_ticks;
    std::uint32_t m_next_lobby_id{1};
    std::uint16_t m_base_lobby_port;
    std::uint16_t m_next_port_offset{0};
    std::unique_ptr<engn::BackendAPIClient> m_api_client;
};
