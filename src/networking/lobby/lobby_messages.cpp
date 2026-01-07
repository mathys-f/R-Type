#include "lobby_messages.h"

#include <algorithm>
#include <cstring>

namespace net::lobby {
namespace {
constexpr std::uint8_t k_byte_mask = 0xFFU;
constexpr std::uint8_t k_shift8 = 8U;
constexpr std::uint8_t k_shift16 = 16U;
constexpr std::uint8_t k_shift24 = 24U;
constexpr std::size_t k_res_create_lobby_min_size = 7;   // 1 + 4 + 2 minimum
constexpr std::size_t k_lobby_state_update_min_size = 5; // 4 + 1 minimum

void append_u32_le(std::vector<std::byte>& out, std::uint32_t value) {
    out.push_back(net::to_byte(value & k_byte_mask));
    out.push_back(net::to_byte((value >> k_shift8) & k_byte_mask));
    out.push_back(net::to_byte((value >> k_shift16) & k_byte_mask));
    out.push_back(net::to_byte((value >> k_shift24) & k_byte_mask));
}

void append_u16_le(std::vector<std::byte>& out, std::uint16_t value) {
    out.push_back(net::to_byte(value & k_byte_mask));
    out.push_back(net::to_byte((value >> k_shift8) & k_byte_mask));
}

void append_u8(std::vector<std::byte>& out, std::uint8_t value) {
    out.push_back(static_cast<std::byte>(value));
}

void append_string(std::vector<std::byte>& out, const std::string& str, std::size_t max_len) {
    std::string trimmed = str.substr(0, max_len);
    append_u8(out, static_cast<std::uint8_t>(trimmed.size()));
    std::ranges::transform(trimmed, std::back_inserter(out),
                           [](char c) { return static_cast<std::byte>(static_cast<unsigned char>(c)); });
}

std::uint32_t read_u32_le(const std::vector<std::byte>& buffer, std::size_t offset) {
    return static_cast<std::uint32_t>(net::byte_to_u8(buffer[offset])) |
           (static_cast<std::uint32_t>(net::byte_to_u8(buffer[offset + 1])) << k_shift8) |
           (static_cast<std::uint32_t>(net::byte_to_u8(buffer[offset + 2])) << k_shift16) |
           (static_cast<std::uint32_t>(net::byte_to_u8(buffer[offset + 3])) << k_shift24);
}

std::uint16_t read_u16_le(const std::vector<std::byte>& buffer, std::size_t offset) {
    return static_cast<std::uint16_t>(net::byte_to_u8(buffer[offset])) |
           (static_cast<std::uint16_t>(net::byte_to_u8(buffer[offset + 1])) << k_shift8);
}

std::uint8_t read_u8(const std::vector<std::byte>& buffer, std::size_t offset) {
    return net::byte_to_u8(buffer[offset]);
}

std::string read_string(const std::vector<std::byte>& buffer, std::size_t& offset) {
    std::uint8_t len = read_u8(buffer, offset);
    offset++;
    std::string result;
    result.reserve(len);
    for (std::size_t i = 0; i < len; ++i) {
        result.push_back(static_cast<char>(static_cast<unsigned char>(buffer[offset + i])));
    }
    offset += len;
    return result;
}
} // namespace

// REQ_LOBBY_LIST
Packet make_req_lobby_list(const ReqLobbyList& /*req*/) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KReqLobbyList);
    packet.header.m_flags = 0;
    // No payload needed
    return packet;
}

std::optional<ReqLobbyList> parse_req_lobby_list(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KReqLobbyList)) {
        return std::nullopt;
    }
    return ReqLobbyList{};
}

// RES_LOBBY_LIST
Packet make_res_lobby_list(const ResLobbyList& res) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KResLobbyList);
    packet.header.m_flags = 0;

    auto count = static_cast<std::uint8_t>(std::min(res.m_lobbies.size(), k_max_lobbies_per_list));
    append_u8(packet.payload, count);

    for (std::uint8_t i = 0; i < count; ++i) {
        const auto& lobby = res.m_lobbies[i];
        append_u32_le(packet.payload, lobby.m_lobby_id);
        append_string(packet.payload, lobby.m_lobby_name, k_max_lobby_name_len);
        append_u8(packet.payload, lobby.m_current_players);
        append_u8(packet.payload, lobby.m_max_players);
        append_u16_le(packet.payload, lobby.m_port);
    }

    return packet;
}

std::optional<ResLobbyList> parse_res_lobby_list(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KResLobbyList)) {
        return std::nullopt;
    }

    const auto& buf = packet.payload;
    if (buf.empty()) {
        return std::nullopt;
    }

    std::size_t offset = 0;
    std::uint8_t count = read_u8(buf, offset);
    offset++;

    ResLobbyList result{};
    result.m_lobbies.reserve(count);

    for (std::uint8_t i = 0; i < count; ++i) {
        if (offset + 4 >= buf.size())
            break; // Need at least lobby_id

        LobbyInfo lobby{};
        lobby.m_lobby_id = read_u32_le(buf, offset);
        offset += 4;

        lobby.m_lobby_name = read_string(buf, offset);

        if (offset + 4 > buf.size())
            break;
        lobby.m_current_players = read_u8(buf, offset);
        offset++;
        lobby.m_max_players = read_u8(buf, offset);
        offset++;
        lobby.m_port = read_u16_le(buf, offset);
        offset += 2;

        result.m_lobbies.push_back(lobby);
    }

    return result;
}

// REQ_CREATE_LOBBY
Packet make_req_create_lobby(const ReqCreateLobby& req) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KReqCreateLobby);
    packet.header.m_flags = 0;

    append_string(packet.payload, req.m_lobby_name, k_max_lobby_name_len);
    append_u8(packet.payload, req.m_max_players);

    return packet;
}

std::optional<ReqCreateLobby> parse_req_create_lobby(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KReqCreateLobby)) {
        return std::nullopt;
    }

    const auto& buf = packet.payload;
    if (buf.empty()) {
        return std::nullopt;
    }

    std::size_t offset = 0;
    ReqCreateLobby result{};
    result.m_lobby_name = read_string(buf, offset);

    if (offset >= buf.size()) {
        return std::nullopt;
    }
    result.m_max_players = read_u8(buf, offset);

    return result;
}

// RES_CREATE_LOBBY
Packet make_res_create_lobby(const ResCreateLobby& res) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KResCreateLobby);
    packet.header.m_flags = 0;

    append_u8(packet.payload, res.m_success ? 1 : 0);
    append_u32_le(packet.payload, res.m_lobby_id);
    append_u16_le(packet.payload, res.m_port);
    append_string(packet.payload, res.m_error_message, k_max_lobby_name_len);

    return packet;
}

std::optional<ResCreateLobby> parse_res_create_lobby(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KResCreateLobby)) {
        return std::nullopt;
    }

    const auto& buf = packet.payload;
    if (buf.size() < k_res_create_lobby_min_size) { // 1 + 4 + 2 minimum
        return std::nullopt;
    }

    std::size_t offset = 0;
    ResCreateLobby result{};
    result.m_success = read_u8(buf, offset) != 0;
    offset++;
    result.m_lobby_id = read_u32_le(buf, offset);
    offset += 4;
    result.m_port = read_u16_le(buf, offset);
    offset += 2;

    if (offset < buf.size()) {
        result.m_error_message = read_string(buf, offset);
    }

    return result;
}

// REQ_JOIN_LOBBY
Packet make_req_join_lobby(const ReqJoinLobby& req) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KReqJoinLobby);
    packet.header.m_flags = 0;

    append_u32_le(packet.payload, req.m_lobby_id);

    return packet;
}

std::optional<ReqJoinLobby> parse_req_join_lobby(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KReqJoinLobby)) {
        return std::nullopt;
    }

    const auto& buf = packet.payload;
    if (buf.size() < 4) {
        return std::nullopt;
    }

    ReqJoinLobby result{};
    result.m_lobby_id = read_u32_le(buf, 0);

    return result;
}

// RES_JOIN_LOBBY
Packet make_res_join_lobby(const ResJoinLobby& res) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KResJoinLobby);
    packet.header.m_flags = 0;

    append_u8(packet.payload, res.m_success ? 1 : 0);
    append_u16_le(packet.payload, res.m_port);
    append_string(packet.payload, res.m_error_message, k_max_lobby_name_len);

    return packet;
}

std::optional<ResJoinLobby> parse_res_join_lobby(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KResJoinLobby)) {
        return std::nullopt;
    }

    const auto& buf = packet.payload;
    if (buf.size() < 3) { // 1 + 2 minimum
        return std::nullopt;
    }

    std::size_t offset = 0;
    ResJoinLobby result{};
    result.m_success = read_u8(buf, offset) != 0;
    offset++;
    result.m_port = read_u16_le(buf, offset);
    offset += 2;

    if (offset < buf.size()) {
        result.m_error_message = read_string(buf, offset);
    }

    return result;
}

// REQ_LEAVE_LOBBY
Packet make_req_leave_lobby(const ReqLeaveLobby& req) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KReqLeaveLobby);
    packet.header.m_flags = 0;

    append_u32_le(packet.payload, req.m_lobby_id);

    return packet;
}

std::optional<ReqLeaveLobby> parse_req_leave_lobby(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KReqLeaveLobby)) {
        return std::nullopt;
    }

    const auto& buf = packet.payload;
    if (buf.size() < 4) {
        return std::nullopt;
    }

    ReqLeaveLobby result{};
    result.m_lobby_id = read_u32_le(buf, 0);

    return result;
}

// LOBBY_STATE_UPDATE
Packet make_lobby_state_update(const LobbyStateUpdate& update) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KLobbyStateUpdate);
    packet.header.m_flags = 0;

    append_u32_le(packet.payload, update.m_lobby_id);
    append_u8(packet.payload, update.m_current_players);

    return packet;
}

std::optional<LobbyStateUpdate> parse_lobby_state_update(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KLobbyStateUpdate)) {
        return std::nullopt;
    }

    const auto& buf = packet.payload;
    if (buf.size() < k_lobby_state_update_min_size) {
        return std::nullopt;
    }

    LobbyStateUpdate result{};
    result.m_lobby_id = read_u32_le(buf, 0);
    result.m_current_players = read_u8(buf, 4);

    return result;
}

} // namespace net::lobby
