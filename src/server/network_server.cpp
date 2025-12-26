#include "network_server.h"

#include <cstdio>

#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "networking/handshake/handshake.h"

using namespace engn;

NetworkServer::NetworkServer(engn::EngineContext &engine_ctx, std::uint16_t port)
    : m_engine_ctx(engine_ctx), m_port(port)
{
    m_session = std::make_shared<net::Session>(m_io, asio::ip::udp::endpoint{}, net::ReliabilityConfig{}, m_port);
}

NetworkServer::~NetworkServer()
{
    stop();
}

void NetworkServer::start()
{
    m_session->start(
        [this](const net::Packet& pkt, const asio::ip::udp::endpoint& from) {
            if (net::handshake::handle_server_handshake(pkt, m_session, from)) {
                std::printf("Client connected from %s:%d\n",
                    from.address().to_string().c_str(), from.port());
                return;
            }
            // Handle other reliable packets here
        },
        // onUnreliable
        [this](const net::Packet& pkt, const asio::ip::udp::endpoint& from) {
            // Handle unreliable packets here (player input, etc.)
        }
    );
    m_running = true;
    m_io_thread = std::thread([this]() {
        std::printf("Network IO thread started\n");
        m_io.run();
        std::printf("Network IO thread stopped\n");
    });
    std::printf("Server started on port %d\n", m_port);
}

void NetworkServer::poll()
{
    m_session->poll();
}

void NetworkServer::stop()
{
    if (m_running.exchange(false)) {
        m_io.stop();
        if (m_io_thread.joinable()) {
            m_io_thread.join();
        }
    }
}
