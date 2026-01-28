#include <gtest/gtest.h>
#include "rtp/networking.h"
#include "handshake/handshake.h"
#include <thread>
#include <atomic>

using namespace net;

class IntegrationTest : public ::testing::Test {
protected:
    asio::io_context m_ctx;
    std::shared_ptr<asio::io_context::work> m_work;
    std::thread m_thread;

    void SetUp() override {
        m_work = std::make_shared<asio::io_context::work>(m_ctx);
        m_thread = std::thread([this]() { m_ctx.run(); });
    }

    void TearDown() override {
        m_work.reset();
        m_ctx.stop();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
};

TEST_F(IntegrationTest, SessionSendReceive) {
    auto server = std::make_shared<Session>(m_ctx, asio::ip::udp::endpoint{}, ReliabilityConfig{}, 0);
    auto client = std::make_shared<Session>(m_ctx, asio::ip::udp::endpoint{}, ReliabilityConfig{}, 0);

    std::atomic<bool> received = false;
    Packet received_packet{};

    server->start(
        [&](const Packet& p, const asio::ip::udp::endpoint&) {
            received_packet = p;
            received = true;
        },
        [](const Packet&, const asio::ip::udp::endpoint&) {}
    );

    client->start(
        [](const Packet&, const asio::ip::udp::endpoint&) {},
        [](const Packet&, const asio::ip::udp::endpoint&) {}
    );

    Packet p{};
    p.header.m_command = 10;
    p.payload = {std::byte{0xAA}, std::byte{0xBB}};

    asio::ip::udp::endpoint server_ep(asio::ip::address_v4::loopback(), server->local_endpoint().port());

    client->send(p, server_ep, true);

    int retries = 0;
    while (!received && retries < 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        server->poll();
        client->poll();
        retries++;
    }

    ASSERT_TRUE(received);
    EXPECT_EQ(received_packet.header.m_command, 10);
    EXPECT_EQ(received_packet.payload.size(), 2);
    EXPECT_EQ(received_packet.payload[0], std::byte{0xAA});
}

TEST_F(IntegrationTest, HandshakeFlow) {
    auto server = std::make_shared<Session>(m_ctx, asio::ip::udp::endpoint{}, ReliabilityConfig{}, 0);
    auto client = std::make_shared<Session>(m_ctx, asio::ip::udp::endpoint{}, ReliabilityConfig{}, 0);

    server->start(
        [&](const Packet& p, const asio::ip::udp::endpoint& ep) {
            if (p.header.m_command == static_cast<uint8_t>(CommandId::KReqLogin)) {
                net::handshake::handle_server_handshake(p, server, ep);
            }
        },
        [](const Packet&, const asio::ip::udp::endpoint&) {}
    );

    std::atomic<bool> login_success = false;

    client->start(
        [&](const Packet& p, const asio::ip::udp::endpoint&) {
            if (p.header.m_command == static_cast<uint8_t>(CommandId::KResLogin)) {
                auto res = net::handshake::parse_res_login(p);
                if (res && res->m_success) {
                    login_success = true;
                }
            }
        },
        [](const Packet&, const asio::ip::udp::endpoint&) {}
    );

    net::handshake::ReqLogin req{};
    req.m_username = "Tester";
    auto p = net::handshake::make_req_login(req);

    asio::ip::udp::endpoint server_ep(asio::ip::address_v4::loopback(), server->local_endpoint().port());
    client->send(p, server_ep, true);

    int retries = 0;
    while (!login_success && retries < 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        server->poll();
        client->poll();
        retries++;
    }

    ASSERT_TRUE(login_success);
}

TEST_F(IntegrationTest, Fragmentation) {
    auto server = std::make_shared<Session>(m_ctx, asio::ip::udp::endpoint{}, ReliabilityConfig{}, 0);
    auto client = std::make_shared<Session>(m_ctx, asio::ip::udp::endpoint{}, ReliabilityConfig{}, 0);

    client->set_fragment_payload_size(10);

    std::atomic<bool> received = false;
    Packet received_packet{};

    server->start(
        [&](const Packet& p, const asio::ip::udp::endpoint&) {
            received_packet = p;
            received = true;
        },
        [](const Packet&, const asio::ip::udp::endpoint&) {}
    );

    client->start(
        [](const Packet&, const asio::ip::udp::endpoint&) {},
        [](const Packet&, const asio::ip::udp::endpoint&) {}
    );

    Packet p{};
    p.header.m_command = 20;
    p.payload.resize(25);
    std::fill(p.payload.begin(), p.payload.end(), std::byte{0xEE});

    asio::ip::udp::endpoint server_ep(asio::ip::address_v4::loopback(), server->local_endpoint().port());
    client->send(p, server_ep, true);

    int retries = 0;
    while (!received && retries < 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        server->poll();
        client->poll();
        retries++;
    }

    ASSERT_TRUE(received);
    EXPECT_EQ(received_packet.payload.size(), 25);
    EXPECT_EQ(received_packet.payload[0], std::byte{0xEE});
    EXPECT_EQ(received_packet.payload[24], std::byte{0xEE});
}

TEST_F(IntegrationTest, SessionErrors) {
    auto session = std::make_shared<Session>(m_ctx, asio::ip::udp::endpoint{}, ReliabilityConfig{}, 0);
    Packet p{};

    EXPECT_THROW(session->send(p, asio::ip::udp::endpoint{}, true), std::logic_error);

    EXPECT_THROW(session->send(p, true), std::logic_error);
}
