#pragma once

#include "../rtp/networking.h"

#include <optional>
#include <string>

namespace net::handshake
{
constexpr std::uint32_t k_protocol_version = 1;
constexpr std::size_t k_max_username_len = 32;

struct ReqLogin
{
    std::string m_username;
    std::uint32_t m_version = k_protocol_version;
    std::uint16_t m_preferred_fragment_size = static_cast<std::uint16_t>(k_max_payload_size);
};

struct ResLogin
{
    bool m_success = false;
    std::uint32_t m_player_id = 0;
    std::uint16_t m_effective_fragment_size = static_cast<std::uint16_t>(k_max_payload_size);
};

// Build a REQ_LOGIN packet containing username, protocol version and preferred fragment size.
Packet make_req_login(const ReqLogin& req);

// Build a RES_LOGIN packet containing success, player id and the negotiated fragment size.
Packet make_res_login(const ResLogin& res);

// Parse a REQ_LOGIN packet returning all fields when valid.
std::optional<ReqLogin> parse_req_login(const Packet& packet);

// Parse a RES_LOGIN packet returning all fields when valid.
std::optional<ResLogin> parse_res_login(const Packet& packet);

// Server-side convenience handler: if `packet` is a REQ_LOGIN this function
// will send a RES_LOGIN reply (currently accepts any username) and return true.
// The caller should invoke this from the reliable packet callback.
bool handle_server_handshake(const Packet& packet, const std::shared_ptr<Session>& session,
    const boost::asio::ip::udp::endpoint& endpoint);

} // namespace net::handshake
