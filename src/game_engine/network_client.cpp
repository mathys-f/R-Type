#include "network_client.h"

#include "networking/handshake/handshake.h"

#include <iostream>

using namespace engn;

constexpr std::chrono::seconds k_heartbeat_interval{5};
constexpr std::chrono::milliseconds k_leave_msg_wait_time{250};

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
        m_server_endpoint = server_endpoint;

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

void NetworkClient::set_on_logout(OnLogoutCallback callback) {
    m_on_logout = callback;
}

void NetworkClient::poll() {
    if (m_session) {
        m_session->poll();
    }

    if (m_connected.load()) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_last_heartbeat);
        if (elapsed >= k_heartbeat_interval) {
            send_heartbeat();
            m_last_heartbeat = now;
        }
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
    return m_session->is_message_acknowledged(id, m_server_endpoint);
}

void NetworkClient::send_heartbeat() {
    if (!m_connected.load() || !m_session) {
        return;
    }
    
    // Create a simple heartbeat packet with no payload
    net::Packet heartbeat{};
    heartbeat.header.m_command = static_cast<std::uint8_t>(net::CommandId::KHeartbeat);
    
    // Send unreliably - it's just a keepalive, if it's lost the next one will arrive
    try {
        m_session->send(heartbeat, false);
    } catch (const std::exception& e) {
        std::cerr << "Failed to send heartbeat: " << e.what() << std::endl;
    }
}

void NetworkClient::disconnect() {
    if (m_running.exchange(false)) {
        // Send logout message to server before disconnecting
        if (m_connected.load() && m_session) {
            net::handshake::ReqLogout logout_req{.m_player_id = m_player_id};
            try {
                m_session->send(net::handshake::make_req_logout(logout_req), true);
                // Give a brief moment for the message to be sent
                std::this_thread::sleep_for(k_leave_msg_wait_time);
            } catch (const std::exception& e) {
                std::cerr << "Failed to send logout message: " << e.what() << std::endl;
            }
        }

        m_connected = false;
        m_io.stop();
        if (m_io_thread.joinable()) {
            m_io_thread.join();
        }
        if (m_on_logout)
            m_on_logout();
        std::cout << "Disconnected from server" << std::endl;
    }
}
