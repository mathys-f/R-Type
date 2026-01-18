#include "network_server.h"

#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "game_engine/events/events.h"
#include "lobby_manager.h"
#include "networking/handshake/handshake.h"
#include "networking/lobby/lobby_messages.h"
#include "utils/logger.h"

#include <iostream>

using namespace engn;

NetworkServer::NetworkServer(engn::EngineContext& engine_ctx, std::uint16_t port, LobbyManager* lobby_manager)
    : m_engine_ctx(engine_ctx), m_port(port), m_lobby_manager(lobby_manager) {
    m_session = std::make_shared<net::Session>(m_io, asio::ip::udp::endpoint{}, net::ReliabilityConfig{}, m_port);
    m_engine_ctx.network_session = m_session;
}

NetworkServer::~NetworkServer() {
    stop();
}

EngineContext &NetworkServer::get_engine() {
    return m_engine_ctx;
}

void NetworkServer::start() {
    // Set up connection callbacks
    m_session->on_client_connect = [this](const asio::ip::udp::endpoint& endpoint) {
        m_io.post([this, endpoint]() { handle_client_connect(endpoint); });
    };

    m_session->on_client_disconnect = [this](const asio::ip::udp::endpoint& endpoint) {
        m_io.post([this, endpoint]() { handle_client_disconnect(endpoint); });
    };

    m_session->start(
        [this](const net::Packet& pkt, const asio::ip::udp::endpoint& from) {
            if (net::handshake::handle_server_handshake(pkt, m_session, from)) {  // NOLINT
                LOG_INFO("Client connected from {}:{}", from.address().to_string(), from.port());  // NOLINT
                return;
            }

            // Handle lobby requests if lobby manager is available
            if (m_lobby_manager) {
                handle_lobby_requests(pkt, from);
            }
            // Handle other reliable packets here
        },
        // onUnreliable
        [this](const net::Packet& pkt, const asio::ip::udp::endpoint& from) {
            // Handle unreliable packets here (player input, etc.)
            if (pkt.header.m_command == static_cast<std::uint8_t>(net::CommandId::KClientInput)) {
                handle_client_input(pkt, from);
            }
        });
    m_running = true;
    m_io_thread = std::thread([this]() {
        LOG_DEBUG("Network IO thread started\n");
        m_io.run();
        LOG_DEBUG("Network IO thread stopped\n");
    });
    LOG_INFO("Server started on port {}", m_port);
}

void NetworkServer::poll() {
    m_session->poll();
}

void NetworkServer::stop() {
    if (m_running.exchange(false)) {
        m_io.stop();
        if (m_io_thread.joinable()) {
            m_io_thread.join();
        }
    }
}

void NetworkServer::handle_lobby_requests(const net::Packet& pkt, const asio::ip::udp::endpoint& from) {
    // Handle REQ_LOBBY_LIST
    if (auto req = net::lobby::parse_req_lobby_list(pkt)) {
        auto lobby_list = m_lobby_manager->get_lobby_list();
        net::lobby::ResLobbyList res;
        res.m_lobbies = lobby_list;
        m_session->send(net::lobby::make_res_lobby_list(res), from, true);
        std::cout << "Sent lobby list to " << from.address().to_string() << ":" << from.port() << "\n";
        return;
    }

    // Handle REQ_CREATE_LOBBY
    if (auto req = net::lobby::parse_req_create_lobby(pkt)) {
        net::lobby::ResCreateLobby res;
        try {
            std::uint32_t lobby_id = m_lobby_manager->create_lobby(req->m_lobby_name, req->m_max_players);
            auto lobby = m_lobby_manager->get_lobby(lobby_id);

            if (lobby) {
                res.m_success = true;
                res.m_lobby_id = lobby_id;
                res.m_port = lobby->get_port();
                std::cout << "Created lobby '" << req->m_lobby_name << "' (ID: " << lobby_id << ") on port "
                          << res.m_port << "\n";
            } else {
                res.m_success = false;
                res.m_error_message = "Failed to create lobby";
            }
        } catch (const std::exception& e) {
            res.m_success = false;
            res.m_error_message = e.what();
            std::cerr << "Error creating lobby: " << e.what() << "\n";
        }

        m_session->send(net::lobby::make_res_create_lobby(res), from, true);
        return;
    }

    // Handle REQ_JOIN_LOBBY
    if (auto req = net::lobby::parse_req_join_lobby(pkt)) {
        net::lobby::ResJoinLobby res;
        auto lobby = m_lobby_manager->get_lobby(req->m_lobby_id);

        if (lobby) {
            if (lobby->can_join()) {
                std::string player_ip = from.address().to_string();
                lobby->add_player(player_ip);
                res.m_success = true;
                res.m_port = lobby->get_port();
                std::cout << "Player " << player_ip << " joined lobby ID " << req->m_lobby_id << "\n";
            } else if (lobby->is_full()) {
                res.m_success = false;
                res.m_error_message = "Lobby is full";
            } else {
                res.m_success = false;
                res.m_error_message = "Lobby is not available";
            }
        } else {
            res.m_success = false;
            res.m_error_message = "Lobby not found";
        }

        m_session->send(net::lobby::make_res_join_lobby(res), from, true);
        return;
    }

    // Handle REQ_LEAVE_LOBBY
    if (auto req = net::lobby::parse_req_leave_lobby(pkt)) {
        auto lobby = m_lobby_manager->get_lobby(req->m_lobby_id);
        if (lobby) {
            std::string player_ip = from.address().to_string();
            lobby->remove_player(player_ip);
            std::cout << "Player " << player_ip << " left lobby ID " << req->m_lobby_id << "\n";
        }
        return;
    }
}

void NetworkServer::handle_client_connect(const asio::ip::udp::endpoint& endpoint) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (m_connected_clients.insert(endpoint).second) {
        LOG_INFO("Client connected: {}:{}", endpoint.address().to_string(), endpoint.port());
        m_engine_ctx.add_client(endpoint);
    }
}

void NetworkServer::handle_client_disconnect(const asio::ip::udp::endpoint& endpoint) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (m_connected_clients.erase(endpoint) > 0) {
        LOG_INFO("Client disconnected: {}:{}", endpoint.address().to_string(), endpoint.port());
        m_engine_ctx.remove_client(endpoint);
    }
}

void NetworkServer::handle_client_input(const net::Packet& pkt, const asio::ip::udp::endpoint& from) {
    constexpr std::size_t k_input_packet_size = 5;
    constexpr std::uint8_t k_shift_8 = 8;
    constexpr std::uint8_t k_shift_16 = 16;
    constexpr std::uint8_t k_shift_24 = 24;
    constexpr std::uint8_t k_input_up = 0x01;
    constexpr std::uint8_t k_input_down = 0x02;
    constexpr std::uint8_t k_input_left = 0x04;
    constexpr std::uint8_t k_input_right = 0x08;
    constexpr std::uint8_t k_input_shoot = 0x10;

    if (pkt.payload.size() < k_input_packet_size) {
        LOG_WARNING("Invalid client input packet size: {}", pkt.payload.size());
        return;
    }

    // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
    std::uint32_t tick = static_cast<std::uint32_t>(pkt.payload[0]) |
                         (static_cast<std::uint32_t>(pkt.payload[1]) << k_shift_8) |
                         (static_cast<std::uint32_t>(pkt.payload[2]) << k_shift_16) |
                         (static_cast<std::uint32_t>(pkt.payload[3]) << k_shift_24);

    std::uint8_t input_mask = static_cast<std::uint8_t>(pkt.payload[4]);

    bool move_up = (input_mask & k_input_up) != 0;
    bool move_down = (input_mask & k_input_down) != 0;
    bool move_left = (input_mask & k_input_left) != 0;
    bool move_right = (input_mask & k_input_right) != 0;
    bool shoot = (input_mask & k_input_shoot) != 0;

    std::string player_ip = from.address().to_string();

    auto& player_queue = m_engine_ctx.player_input_queues[player_ip];

    const auto& controls = m_engine_ctx.controls;

    if (move_up) {
        player_queue.push(engn::evts::KeyHold{controls.move_up.primary});
    }
    if (move_down) {
        player_queue.push(engn::evts::KeyHold{controls.move_down.primary});
    }
    if (move_left) {
        player_queue.push(engn::evts::KeyHold{controls.move_left.primary});
    }
    if (move_right) {
        player_queue.push(engn::evts::KeyHold{controls.move_right.primary});
    }
    if (shoot) {
        player_queue.push(engn::evts::KeyHold{controls.shoot.primary});
    }
}
