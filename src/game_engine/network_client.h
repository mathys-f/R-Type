#pragma once

#include "ecs/entity.h"
#include "networking/rtp/networking.h"

#include <asio.hpp>
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

namespace engn {

class NetworkClient {
  public:
    using OnPacketCallback = std::function<void(const net::Packet&)>;
    using OnLoginCallback = std::function<void(bool success, uint32_t player_id)>;
    using OnLogoutCallback = std::function<void()>;

    NetworkClient() = default;
    ~NetworkClient();

    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;
    NetworkClient(NetworkClient&&) = delete;
    NetworkClient& operator=(NetworkClient&&) = delete;

    void connect(const std::string& host, std::uint16_t port, const std::string& username);
    void set_on_login(OnLoginCallback callback);
    void set_on_unreliable(OnPacketCallback callback);
    void set_on_logout(OnLogoutCallback callback);
    void set_on_reliable(OnPacketCallback callback);
    void poll();
    std::uint32_t send_reliable(const net::Packet& packet);
    void send_unreliable(const net::Packet& packet);
    void send_input_mask(std::uint8_t mask, std::uint32_t tick);
    net::DeliveryStatus is_message_acknowledged(std::uint32_t id) const;
    void disconnect();
    bool is_connected() const {
        return m_connected.load();
    }
    uint32_t player_id() const {
        return m_player_id;
    }

  private:
    void send_heartbeat(); // Send keepalive ping to server

    asio::io_context m_io;
    std::shared_ptr<net::Session> m_session;
    std::thread m_io_thread;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_connected{false};
    uint32_t m_player_id{0};
    asio::ip::udp::endpoint m_server_endpoint{};
    OnLoginCallback m_on_login;
    OnLogoutCallback m_on_logout;
    OnPacketCallback m_on_reliable;
    OnPacketCallback m_on_unreliable;
    std::chrono::steady_clock::time_point m_last_heartbeat{};
};

} // namespace engn
