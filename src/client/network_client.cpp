#include "network_client.h"

#include <cstdio>
#include "game_engine/engine.h"
#include "networking/handshake/handshake.h"

using namespace engn;

NetworkClient::NetworkClient(engn::EngineContext &engine_ctx)
    : m_engine_ctx(engine_ctx)
{
}

NetworkClient::~NetworkClient()
{
    disconnect();
}

void NetworkClient::connect(const std::string& host, std::uint16_t port, const std::string& username)
{
    if (m_running.load()) {
        std::fprintf(stderr, "Already connected or connecting\n");
        return;
    }

    try {
        // Resolve server address
        asio::ip::udp::resolver resolver(m_io);
        auto endpoints = resolver.resolve(asio::ip::udp::v4(), host, std::to_string(port));
        asio::ip::udp::endpoint server_endpoint = *endpoints.begin();

        // Create session pointing to server
        m_session = std::make_shared<net::Session>(m_io, server_endpoint);

        // Start listening (capture session in lambdas)
        m_session->start(
            // onReliable
            [this, session = m_session](const net::Packet& pkt, const asio::ip::udp::endpoint&) {
                // Parse login response
                if (auto res = net::handshake::parse_res_login(pkt)) {
                    m_player_id = res->m_player_id;
                    m_connected = res->m_success;

                    if (res->m_success) {
                        std::printf("Login successful! Player ID: %u\n", res->m_player_id);

                        // Set fragment size
                        if (res->m_effective_fragment_size > 0) {
                            session->set_fragment_payload_size(res->m_effective_fragment_size);
                        }
                    } else {
                        std::fprintf(stderr, "Login failed\n");
                    }

                    // Notify callback
                    if (m_on_login) {
                        m_on_login(res->m_success, res->m_player_id);
                    }
                    return;
                }

                // Handle other reliable messages from server
                // e.g., spawn enemy, player died, level complete
                if (!m_connected.load()) {
                    std::fprintf(stderr, "Received packet before login\n");
                    return;
                }
            },
            // onUnreliable
            [this](const net::Packet& pkt, const asio::ip::udp::endpoint&) {
                if (!m_connected.load()) return;
            }
        );

        // Start IO thread
        m_running = true;
        m_io_thread = std::thread([this]() {
            std::printf("Network IO thread started\n");
            m_io.run();
            std::printf("Network IO thread stopped\n");
        });

        // Send login request
        net::handshake::ReqLogin req{
            .m_username = username,
            .m_version = net::handshake::k_protocol_version,
            .m_preferred_fragment_size = 0  // Use default
        };
        m_session->send(net::handshake::make_req_login(req), true);

        std::printf("Connecting to %s:%d as '%s'...\n", host.c_str(), port, username.c_str());

    } catch (const std::exception& e) {
        std::fprintf(stderr, "Failed to connect: %s\n", e.what());
        disconnect();
        throw;
    }
}

void NetworkClient::set_on_login(OnLoginCallback callback)
{
    m_on_login = callback;
}

void NetworkClient::poll()
{
    if (m_session) {
        m_session->poll();
    }
}

void NetworkClient::send_reliable(const net::Packet& packet)
{
    if (!m_connected.load()) {
        std::fprintf(stderr, "Cannot send: not connected\n");
        return;
    }
    m_session->send(packet, true);
}

void NetworkClient::send_unreliable(const net::Packet& packet)
{
    if (!m_connected.load()) {
        std::fprintf(stderr, "Cannot send: not connected\n");
        return;
    }
    m_session->send(packet, false);
}

void NetworkClient::disconnect()
{
    if (m_running.exchange(false)) {
        m_connected = false;
        m_io.stop();
        if (m_io_thread.joinable()) {
            m_io_thread.join();
        }
        std::printf("Disconnected from server\n");
    }
}
