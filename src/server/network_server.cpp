#include "network_server.h"

#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "lobby_manager.h"
#include "networking/handshake/handshake.h"
#include "networking/lobby/lobby_messages.h"
#include "utils/logger.h"

#include <iostream>

using namespace engn;

NetworkServer::NetworkServer(engn::EngineContext& engine_ctx, std::uint16_t port, LobbyManager* lobby_manager)
    : m_engine_ctx(engine_ctx), m_port(port), m_lobby_manager(lobby_manager) {
    m_session = std::make_shared<net::Session>(m_io, asio::ip::udp::endpoint{}, net::ReliabilityConfig{}, m_port);
}

NetworkServer::~NetworkServer() {
    stop();
}

void NetworkServer::start() {
    m_session->start(
        [this](const net::Packet& pkt, const asio::ip::udp::endpoint& from) {
            if (net::handshake::handle_server_handshake(pkt, m_session, from)) {
                LOG_INFO("Client connected from {}:{}", from.address().to_string(), from.port());
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
                lobby->add_player(from);
                res.m_success = true;
                res.m_port = lobby->get_port();
                std::cout << "Player joined lobby ID " << req->m_lobby_id << "\n";
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
            lobby->remove_player(from);
            std::cout << "Player left lobby ID " << req->m_lobby_id << "\n";
        }
        return;
    }
}
