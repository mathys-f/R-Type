#pragma once

#include <asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <functional>

#include "networking/rtp/networking.h"
#include "ecs/entity.h"

namespace engn { class EngineContext; }

class NetworkClient {
public:
    using OnLoginCallback = std::function<void(bool success, uint32_t player_id)>;

    NetworkClient(engn::EngineContext &engine_ctx);
    ~NetworkClient();

    void connect(const std::string& host, std::uint16_t port, const std::string& username);
    void set_on_login(OnLoginCallback callback);
    void poll();
    void send_reliable(const net::Packet& packet);
    void send_unreliable(const net::Packet& packet);
    void disconnect();
    bool is_connected() const { return m_connected.load(); }
    uint32_t player_id() const { return m_player_id; }

private:
    engn::EngineContext &m_engine_ctx;
    asio::io_context m_io;
    std::shared_ptr<net::Session> m_session;
    std::thread m_io_thread;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_connected{false};
    uint32_t m_player_id{0};
    OnLoginCallback m_on_login;
};
