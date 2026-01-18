#pragma once

#include "ecs/entity.h"
#include "networking/rtp/networking.h"

#include <asio.hpp>
#include <atomic>
#include <memory>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace engn {
class EngineContext;
}

class LobbyManager;

class NetworkServer {
  public:
    NetworkServer(engn::EngineContext& engine_ctx, std::uint16_t port, LobbyManager* lobby_manager = nullptr);
    ~NetworkServer();
    NetworkServer(const NetworkServer&) = delete;
    NetworkServer& operator=(const NetworkServer&) = delete;
    NetworkServer(NetworkServer&&) = delete;
    NetworkServer& operator=(NetworkServer&&) = delete;

    engn::EngineContext &get_engine();

    void start();
    void poll();
    void stop();
    void check_client_timeouts(); // Check for inactive clients

  private:
    void handle_lobby_requests(const net::Packet& pkt, const asio::ip::udp::endpoint& from);
    void handle_client_connect(const asio::ip::udp::endpoint& endpoint);
    void handle_client_disconnect(const asio::ip::udp::endpoint& endpoint);
    void update_client_activity(const asio::ip::udp::endpoint& endpoint); // Update last activity time

    engn::EngineContext& m_engine_ctx;
    std::uint16_t m_port;
    asio::io_context m_io;
    std::shared_ptr<net::Session> m_session;
    std::thread m_io_thread;
    std::atomic<bool> m_running{false};
    LobbyManager* m_lobby_manager;
    std::unordered_set<asio::ip::udp::endpoint, net::EndpointHash> m_connected_clients;
    std::unordered_map<asio::ip::udp::endpoint, std::chrono::steady_clock::time_point, net::EndpointHash> m_client_last_activity;
    std::mutex clients_mutex;
};
