#pragma once

#include "../rtp/networking.h"

#include <optional>
#include <string>
#include <vector>

namespace net::lobby {

constexpr std::size_t k_max_lobby_name_len = 64;
constexpr std::size_t k_max_lobbies_per_list = 32;
constexpr std::uint8_t k_max_players_per_lobby = 4;

// Lobby information structure
struct LobbyInfo {
    std::uint32_t m_lobby_id = 0;
    std::string m_lobby_name;
    std::uint8_t m_current_players = 0;
    std::uint8_t m_max_players = k_max_players_per_lobby;
    std::uint16_t m_port = 0; // Port where this lobby's game server is running
};

// Request to get list of available lobbies
struct ReqLobbyList {
    // Empty for now, can add filters later
};

// Response with list of lobbies
struct ResLobbyList {
    std::vector<LobbyInfo> m_lobbies;
};

// Request to create a new lobby
struct ReqCreateLobby {
    std::string m_lobby_name;
    std::uint8_t m_max_players = k_max_players_per_lobby;
};

// Response after creating a lobby
struct ResCreateLobby {
    bool m_success = false;
    std::uint32_t m_lobby_id = 0;
    std::uint16_t m_port = 0; // Port to connect to for this lobby
    std::string m_error_message;
};

// Request to join an existing lobby
struct ReqJoinLobby {
    std::uint32_t m_lobby_id = 0;
};

// Response after joining a lobby
struct ResJoinLobby {
    bool m_success = false;
    std::uint16_t m_port = 0; // Port to connect to for this lobby
    std::string m_error_message;
};

// Request to leave a lobby
struct ReqLeaveLobby {
    std::uint32_t m_lobby_id = 0;
};

// Notification when lobby state changes (someone joins/leaves)
struct LobbyStateUpdate {
    std::uint32_t m_lobby_id = 0;
    std::uint8_t m_current_players = 0;
};

// Build a REQ_LOBBY_LIST packet
Packet make_req_lobby_list(const ReqLobbyList& req);

// Build a RES_LOBBY_LIST packet
Packet make_res_lobby_list(const ResLobbyList& res);

// Build a REQ_CREATE_LOBBY packet
Packet make_req_create_lobby(const ReqCreateLobby& req);

// Build a RES_CREATE_LOBBY packet
Packet make_res_create_lobby(const ResCreateLobby& res);

// Build a REQ_JOIN_LOBBY packet
Packet make_req_join_lobby(const ReqJoinLobby& req);

// Build a RES_JOIN_LOBBY packet
Packet make_res_join_lobby(const ResJoinLobby& res);

// Build a REQ_LEAVE_LOBBY packet
Packet make_req_leave_lobby(const ReqLeaveLobby& req);

// Build a LOBBY_STATE_UPDATE packet
Packet make_lobby_state_update(const LobbyStateUpdate& update);

// Parse a REQ_LOBBY_LIST packet
std::optional<ReqLobbyList> parse_req_lobby_list(const Packet& packet);

// Parse a RES_LOBBY_LIST packet
std::optional<ResLobbyList> parse_res_lobby_list(const Packet& packet);

// Parse a REQ_CREATE_LOBBY packet
std::optional<ReqCreateLobby> parse_req_create_lobby(const Packet& packet);

// Parse a RES_CREATE_LOBBY packet
std::optional<ResCreateLobby> parse_res_create_lobby(const Packet& packet);

// Parse a REQ_JOIN_LOBBY packet
std::optional<ReqJoinLobby> parse_req_join_lobby(const Packet& packet);

// Parse a RES_JOIN_LOBBY packet
std::optional<ResJoinLobby> parse_res_join_lobby(const Packet& packet);

// Parse a REQ_LEAVE_LOBBY packet
std::optional<ReqLeaveLobby> parse_req_leave_lobby(const Packet& packet);

// Parse a LOBBY_STATE_UPDATE packet
std::optional<LobbyStateUpdate> parse_lobby_state_update(const Packet& packet);

} // namespace net::lobby
