#pragma once

#include "ecs/entity.h"
#include "networking/rtp/networking.h"

#include <asio.hpp>
#include <atomic>
#include <memory>
#include <thread>

namespace engn {
class EngineContext;
}

class NetworkServer {
  public:
    NetworkServer(engn::EngineContext& engine_ctx, std::uint16_t port);
    ~NetworkServer();
    NetworkServer(const NetworkServer&) = delete;
    NetworkServer& operator=(const NetworkServer&) = delete;
    NetworkServer(NetworkServer&&) = delete;
    NetworkServer& operator=(NetworkServer&&) = delete;
    void start();
    void poll();
    void stop();

  private:
    engn::EngineContext& m_engine_ctx;
    std::uint16_t m_port;
    asio::io_context m_io;
    std::shared_ptr<net::Session> m_session;
    std::thread m_io_thread;
    std::atomic<bool> m_running{false};
};
