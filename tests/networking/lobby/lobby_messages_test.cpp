#include <gtest/gtest.h>
#include "lobby/lobby_messages.h"

using namespace net::lobby;

TEST(LobbyMessagesTest, ReqLobbyListRoundTrip) {
    ReqLobbyList req{};
    auto packet = make_req_lobby_list(req);
    auto parsed = parse_req_lobby_list(packet);

    ASSERT_TRUE(parsed.has_value());
}

TEST(LobbyMessagesTest, ResLobbyListRoundTrip) {
    ResLobbyList original{};
    original.m_lobbies.push_back({1, "Lobby1", 2, 4, 8080});
    original.m_lobbies.push_back({2, "Lobby2", 1, 4, 8081});

    auto packet = make_res_lobby_list(original);
    auto parsed = parse_res_lobby_list(packet);

    ASSERT_TRUE(parsed.has_value());
    ASSERT_EQ(parsed->m_lobbies.size(), 2);

    EXPECT_EQ(parsed->m_lobbies[0].m_lobby_id, 1);
    EXPECT_EQ(parsed->m_lobbies[0].m_lobby_name, "Lobby1");
    EXPECT_EQ(parsed->m_lobbies[0].m_current_players, 2);
    EXPECT_EQ(parsed->m_lobbies[0].m_max_players, 4);
    EXPECT_EQ(parsed->m_lobbies[0].m_port, 8080);

    EXPECT_EQ(parsed->m_lobbies[1].m_lobby_id, 2);
    EXPECT_EQ(parsed->m_lobbies[1].m_lobby_name, "Lobby2");
}

TEST(LobbyMessagesTest, ReqCreateLobbyRoundTrip) {
    ReqCreateLobby original{};
    original.m_lobby_name = "MyLobby";
    original.m_max_players = 3;

    auto packet = make_req_create_lobby(original);
    auto parsed = parse_req_create_lobby(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->m_lobby_name, "MyLobby");
    EXPECT_EQ(parsed->m_max_players, 3);
}

TEST(LobbyMessagesTest, ResCreateLobbyRoundTrip) {
    ResCreateLobby original{};
    original.m_success = true;
    original.m_lobby_id = 100;
    original.m_port = 9000;
    original.m_error_message = "";

    auto packet = make_res_create_lobby(original);
    auto parsed = parse_res_create_lobby(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_TRUE(parsed->m_success);
    EXPECT_EQ(parsed->m_lobby_id, 100);
    EXPECT_EQ(parsed->m_port, 9000);
    EXPECT_TRUE(parsed->m_error_message.empty());
}

TEST(LobbyMessagesTest, ResCreateLobbyFailRoundTrip) {
    ResCreateLobby original{};
    original.m_success = false;
    original.m_lobby_id = 0;
    original.m_port = 0;
    original.m_error_message = "Name taken";

    auto packet = make_res_create_lobby(original);
    auto parsed = parse_res_create_lobby(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_FALSE(parsed->m_success);
    EXPECT_EQ(parsed->m_error_message, "Name taken");
}

TEST(LobbyMessagesTest, ReqJoinLobbyRoundTrip) {
    ReqJoinLobby original{};
    original.m_lobby_id = 55;

    auto packet = make_req_join_lobby(original);
    auto parsed = parse_req_join_lobby(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->m_lobby_id, 55);
}

TEST(LobbyMessagesTest, ResJoinLobbyRoundTrip) {
    ResJoinLobby original{};
    original.m_success = true;
    original.m_port = 1234;
    original.m_error_message = "";

    auto packet = make_res_join_lobby(original);
    auto parsed = parse_res_join_lobby(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_TRUE(parsed->m_success);
    EXPECT_EQ(parsed->m_port, 1234);
}

TEST(LobbyMessagesTest, ReqLeaveLobbyRoundTrip) {
    ReqLeaveLobby original{};
    original.m_lobby_id = 77;

    auto packet = make_req_leave_lobby(original);
    auto parsed = parse_req_leave_lobby(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->m_lobby_id, 77);
}

TEST(LobbyMessagesTest, LobbyStateUpdateRoundTrip) {
    LobbyStateUpdate original{};
    original.m_lobby_id = 101;
    original.m_current_players = 3;

    auto packet = make_lobby_state_update(original);
    auto parsed = parse_lobby_state_update(packet);

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->m_lobby_id, 101);
    EXPECT_EQ(parsed->m_current_players, 3);
}
