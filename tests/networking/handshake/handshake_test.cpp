#include <gtest/gtest.h>
#include "handshake/handshake.h"

using namespace net::handshake;

TEST(HandshakeTest, ReqLoginRoundTrip) {
    ReqLogin original{};
    original.m_username = "Player1";
    original.m_version = 123;
    original.m_preferred_fragment_size = 1000;

    auto packet = make_req_login(original);
    auto parsed = parse_req_login(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->m_username, "Player1");
    EXPECT_EQ(parsed->m_version, 123);
    EXPECT_EQ(parsed->m_preferred_fragment_size, 1000);
}

TEST(HandshakeTest, ReqLoginTruncation) {
    ReqLogin original{};
    original.m_username = std::string(100, 'A');
    original.m_version = 1;
    original.m_preferred_fragment_size = 500;

    auto packet = make_req_login(original);
    auto parsed = parse_req_login(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->m_username.size(), k_max_username_len);
    EXPECT_EQ(parsed->m_username, std::string(32, 'A'));
}

TEST(HandshakeTest, ResLoginRoundTrip) {
    ResLogin original{};
    original.m_success = true;
    original.m_player_id = 999;
    original.m_effective_fragment_size = 1024;

    auto packet = make_res_login(original);
    auto parsed = parse_res_login(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_TRUE(parsed->m_success);
    EXPECT_EQ(parsed->m_player_id, 999);
    EXPECT_EQ(parsed->m_effective_fragment_size, 1004);
}

TEST(HandshakeTest, ResLoginFailRoundTrip) {
    ResLogin original{};
    original.m_success = false;
    original.m_player_id = 0;
    original.m_effective_fragment_size = 512;

    auto packet = make_res_login(original);
    auto parsed = parse_res_login(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_FALSE(parsed->m_success);
    EXPECT_EQ(parsed->m_player_id, 0);
    EXPECT_EQ(parsed->m_effective_fragment_size, 512);
}

TEST(HandshakeTest, ReqLogoutRoundTrip) {
    ReqLogout original{};
    original.m_player_id = 12345;

    auto packet = make_req_logout(original);
    auto parsed = parse_req_logout(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->m_player_id, 12345);
}

TEST(HandshakeTest, InvalidPacketCommand) {
    net::Packet packet{};
    packet.header.m_command = 255;

    auto parsed = parse_req_login(packet);
    EXPECT_FALSE(parsed.has_value());
}
