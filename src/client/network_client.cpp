#include "network_client.h"

#include "game_engine/engine.h"
#include "networking/handshake/handshake.h"

#include <iostream>

using namespace engn;

NetworkClient::NetworkClient(engn::EngineContext& engine_ctx) : m_engine_ctx(engine_ctx) {}

NetworkClient::~NetworkClient() {
    disconnect();
}

void NetworkClient::set_on_reliable(OnPacketCallback callback) {
    m_on_reliable = std::move(callback);
}

void NetworkClient::set_on_unreliable(OnPacketCallback callback) {
    m_on_unreliable = std::move(callback);
}

void NetworkClient::connect(const std::string& host, std::uint16_t port, const std::string& username) {
    if (m_running.load()) {
        std::cerr << "Already connected or connecting" << std::endl;
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
                        std::cout << "Login successful! Player ID: " << res->m_player_id << std::endl;

                        // Set fragment size
                        if (res->m_effective_fragment_size > 0) {
                            session->set_fragment_payload_size(res->m_effective_fragment_size);
                        }
                    } else {
                        std::cerr << "Login failed" << std::endl;
                    }

                    // Notify callback
                    if (m_on_login) {
                        m_on_login(res->m_success, res->m_player_id);
                    }
                    return;
                }

                // Handle other reliable messages from server
                if (!m_connected.load()) {
                    std::cerr << "Received packet before login" << std::endl;
                    return;
                }

                if (m_on_reliable) {
                    m_on_reliable(pkt);
                }
            },
            // onUnreliable
            [this](const net::Packet& pkt, const asio::ip::udp::endpoint&) {
                if (!m_connected.load())
                    return;

                if (m_on_unreliable) {
                    m_on_unreliable(pkt);
                }
            });

        // Start IO thread
        m_running = true;
        m_io_thread = std::thread([this]() {
            std::cout << "Network IO thread started" << std::endl;
            m_io.run();
            std::cout << "Network IO thread stopped" << std::endl;
        });

        // Send login request
        net::handshake::ReqLogin req{
            .m_username = username,
            .m_version = net::handshake::k_protocol_version,
            .m_preferred_fragment_size = 0 // Use default
        };
        m_session->send(net::handshake::make_req_login(req), true);

        std::cout << "Connecting to " << host << ":" << port << " as '" << username << "'..." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Failed to connect: " << e.what() << std::endl;
        disconnect();
        throw;
    }
}

void NetworkClient::set_on_login(OnLoginCallback callback) {
    m_on_login = callback;
}

void NetworkClient::poll() {
    if (m_session) {
        m_session->poll();
    }
}

std::uint32_t NetworkClient::send_reliable(const net::Packet& packet) {
    if (!m_connected.load()) {
        std::cerr << "Cannot send: not connected" << std::endl;
        return 0;
    }
    return m_session->send(packet, true);
}

void NetworkClient::send_unreliable(const net::Packet& packet) {
    if (!m_connected.load()) {
        std::cerr << "Cannot send: not connected" << std::endl;
        return;
    }
    m_session->send(packet, false);
}

bool NetworkClient::is_message_acknowledged(std::uint32_t id) const {
    if (!m_connected.load() || !m_session) {
        return false;
    }
    return m_session->is_message_acknowledged(id);
}

void NetworkClient::disconnect() {
    if (m_running.exchange(false)) {
        m_connected = false;
        m_io.stop();
        if (m_io_thread.joinable()) {
            m_io_thread.join();
        }
        std::cout << "Disconnected from server" << std::endl;
    }
}
