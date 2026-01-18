#pragma once

#include "lobby_manager.h"
#include <httplib.h>
#include <memory>
#include <thread>
#include <atomic>

// HTTP Status Codes
namespace http {
    constexpr int k_status_ok = 200;
    constexpr int k_status_created = 201;
    constexpr int k_status_bad_request = 400;
    constexpr int k_status_not_found = 404;
    constexpr int k_status_internal_error = 500;
}

/**
 * Simple HTTP server for admin commands
 * Allows Node.js backend to control C++ game server (stop lobbies, etc.)
 */
class AdminHTTPServer {
  public:
    explicit AdminHTTPServer(LobbyManager* lobby_manager, std::uint16_t port = 8082);
    ~AdminHTTPServer();

    AdminHTTPServer(const AdminHTTPServer&) = delete;
    AdminHTTPServer& operator=(const AdminHTTPServer&) = delete;
    AdminHTTPServer(AdminHTTPServer&&) = delete;
    AdminHTTPServer& operator=(AdminHTTPServer&&) = delete;

    void start();
    void stop();
    bool is_running() const { return m_running; }

  private:
    void run_server();
    void setup_routes();

    LobbyManager* m_lobby_manager;
    std::uint16_t m_port;
    std::unique_ptr<httplib::Server> m_server;
    std::unique_ptr<std::thread> m_server_thread;
    std::atomic<bool> m_running{false};
};
