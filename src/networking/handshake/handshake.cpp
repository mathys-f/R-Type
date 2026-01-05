#include "handshake.h"

#include <algorithm>

namespace net::handshake {
namespace {
constexpr std::uint8_t k_byte_mask = 0xFFU;
constexpr std::uint8_t k_shift8 = 8U;
constexpr std::uint8_t k_shift16 = 16U;
constexpr std::uint8_t k_shift24 = 24U;

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
} // namespace

Packet make_req_login(const ReqLogin& req) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KReqLogin);
    packet.header.m_flags = 0;

    std::string trimmed = req.m_username.substr(0, k_max_username_len);
    const auto k_name_len = static_cast<std::uint8_t>(trimmed.size());
    const std::uint16_t k_pref =
        (req.m_preferred_fragment_size == 0 || req.m_preferred_fragment_size > k_max_payload_size)
            ? static_cast<std::uint16_t>(k_max_payload_size)
            : req.m_preferred_fragment_size;

    packet.payload.reserve(1 + k_name_len + sizeof(req.m_version) + sizeof(k_pref));
    packet.payload.push_back(static_cast<std::byte>(k_name_len));
    std::ranges::transform(trimmed, std::back_inserter(packet.payload),
                           [](char c) { return static_cast<std::byte>(static_cast<unsigned char>(c)); });
    append_u32_le(packet.payload, req.m_version);
    append_u16_le(packet.payload, k_pref);
    return packet;
}

Packet make_res_login(const ResLogin& res) {
    Packet packet{};
    packet.header.m_command = static_cast<std::uint8_t>(CommandId::KResLogin);
    packet.header.m_flags = 0;

    const std::uint16_t k_effective =
        (res.m_effective_fragment_size == 0 || res.m_effective_fragment_size > k_max_payload_size)
            ? static_cast<std::uint16_t>(k_max_payload_size)
            : res.m_effective_fragment_size;

    packet.payload.reserve(1 + sizeof(res.m_player_id) + sizeof(k_effective));
    packet.payload.push_back(static_cast<std::byte>(res.m_success ? 1 : 0));
    append_u32_le(packet.payload, res.m_player_id);
    append_u16_le(packet.payload, k_effective);
    return packet;
}

std::optional<ReqLogin> parse_req_login(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KReqLogin)) {
        return std::nullopt;
    }
    const auto& buf = packet.payload;
    if (buf.size() < 1 + sizeof(std::uint32_t) + sizeof(std::uint16_t)) {
        return std::nullopt;
    }

    const auto k_name_len = static_cast<std::uint8_t>(buf[0]);
    const std::size_t k_required = 1 + k_name_len + sizeof(std::uint32_t) + sizeof(std::uint16_t);
    if (k_name_len > k_max_username_len || buf.size() < k_required) {
        return std::nullopt;
    }

    ReqLogin result{};
    result.m_username.reserve(k_name_len);
    for (std::size_t i = 0; i < k_name_len; ++i) {
        result.m_username.push_back(static_cast<char>(static_cast<unsigned char>(buf[1 + i])));
    }
    const std::size_t k_version_offset = 1 + k_name_len;
    const std::size_t k_pref_offset = k_version_offset + sizeof(std::uint32_t);
    result.m_version = read_u32_le(buf, k_version_offset);
    result.m_preferred_fragment_size = read_u16_le(buf, k_pref_offset);
    return result;
}

std::optional<ResLogin> parse_res_login(const Packet& packet) {
    if (packet.header.m_command != static_cast<std::uint8_t>(CommandId::KResLogin)) {
        return std::nullopt;
    }
    const auto& buf = packet.payload;
    if (buf.size() < 1 + sizeof(std::uint32_t) + sizeof(std::uint16_t)) {
        return std::nullopt;
    }

    ResLogin result{};
    result.m_success = static_cast<std::uint8_t>(buf[0]) != 0;
    result.m_player_id = read_u32_le(buf, 1);
    result.m_effective_fragment_size = read_u16_le(buf, 1 + sizeof(std::uint32_t));
    return result;
}

bool handle_server_handshake(const Packet& packet, const std::shared_ptr<Session>& session,
                             const asio::ip::udp::endpoint& endpoint) {
    const auto k_req = parse_req_login(packet);
    if (!k_req.has_value()) {
        return false;
    }

    const std::uint16_t k_requested = k_req->m_preferred_fragment_size;
    const std::uint16_t k_effective =
        (k_requested == 0) ? static_cast<std::uint16_t>(k_max_payload_size)
                           : static_cast<std::uint16_t>(std::min<std::size_t>(k_requested, k_max_payload_size));

    session->set_fragment_payload_size(k_effective);
    ResLogin resp_payload{.m_success = true, .m_player_id = 0, .m_effective_fragment_size = k_effective};
    Packet resp = make_res_login(resp_payload);
    session->send(resp, endpoint, true);
    return true;
}

} // namespace net::handshake
