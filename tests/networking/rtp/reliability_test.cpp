#include <gtest/gtest.h>
#include "rtp/networking.h"
#include <thread>

using namespace net;

TEST(ReliabilityTest, SequenceGeneration) {
    ReliableSendQueue queue;
    EXPECT_EQ(queue.next_sequence(), 1);
    EXPECT_EQ(queue.next_sequence(), 2);
}

TEST(ReliabilityTest, QueueAndAck) {
    ReliableSendQueue queue;
    Packet p1{};
    p1.header.m_sequence = queue.next_sequence();

    Packet p2{};
    p2.header.m_sequence = queue.next_sequence();

    auto now = std::chrono::steady_clock::now();
    asio::ip::udp::endpoint ep1(asio::ip::address::from_string("127.0.0.1"), 1000);

    queue.track(p1, now, ep1);
    queue.track(p2, now, ep1);

    EXPECT_FALSE(queue.is_acknowledged(1, ep1));
    EXPECT_FALSE(queue.is_acknowledged(2, ep1));

    queue.acknowledge(1, ep1);
    EXPECT_TRUE(queue.is_acknowledged(1, ep1));
    EXPECT_FALSE(queue.is_acknowledged(2, ep1));

    queue.acknowledge(2, ep1);
    EXPECT_TRUE(queue.is_acknowledged(2, ep1));
}

TEST(ReliabilityTest, TimeoutAndRetransmission) {
    ReliabilityConfig config;
    config.initial_rto = std::chrono::milliseconds(50);
    config.max_retransmissions = 3;

    ReliableSendQueue queue(config);
    Packet p1{};
    p1.header.m_sequence = queue.next_sequence();

    auto now = std::chrono::steady_clock::now();
    asio::ip::udp::endpoint ep1;

    queue.track(p1, now, ep1);

    auto timeouts = queue.collect_timeouts(now);
    EXPECT_TRUE(timeouts.empty());

    timeouts = queue.collect_timeouts(now + std::chrono::milliseconds(40));
    EXPECT_TRUE(timeouts.empty());

    timeouts = queue.collect_timeouts(now + std::chrono::milliseconds(60));
    ASSERT_EQ(timeouts.size(), 1);
    EXPECT_EQ(timeouts[0].header.m_sequence, p1.header.m_sequence);

    auto failures = queue.take_failures();
    EXPECT_TRUE(failures.empty());
}

TEST(ReliabilityTest, MaxRetransmissions) {
    ReliabilityConfig config;
    config.initial_rto = std::chrono::milliseconds(10);
    config.max_retransmissions = 2;

    ReliableSendQueue queue(config);
    Packet p1{};
    p1.header.m_sequence = queue.next_sequence();

    auto start = std::chrono::steady_clock::now();
    asio::ip::udp::endpoint ep1;

    queue.track(p1, start, ep1);

    auto now = start + std::chrono::milliseconds(20);
    auto timeouts = queue.collect_timeouts(now);
    EXPECT_EQ(timeouts.size(), 1);

    now += std::chrono::milliseconds(40);
    timeouts = queue.collect_timeouts(now);
    EXPECT_TRUE(timeouts.empty());

    auto failures = queue.take_failures();
    ASSERT_EQ(failures.size(), 1);
    EXPECT_EQ(failures[0], p1.header.m_sequence);
}

TEST(ReliabilityTest, ReceiveWindow) {
    ReliableReceiveWindow window;

    EXPECT_EQ(window.ack(), 0);

    window.observe(1);
    EXPECT_EQ(window.ack(), 1);

    window.observe(2);
    EXPECT_EQ(window.ack(), 2);

    window.observe(5);
    EXPECT_EQ(window.ack(), 5);

    window.observe(3);
    EXPECT_EQ(window.ack(), 5);
}
