#include <gtest/gtest.h>
#include "rtp/networking.h"
#include <vector>
#include <cstring>

using namespace net;

TEST(PacketTest, HeaderSerializeDeserialize) {
    PacketHeader original{};
    original.m_magic = k_magic_number;
    original.m_command = static_cast<std::uint8_t>(CommandId::KReqLogin);
    original.m_flags = static_cast<std::uint8_t>(PacketFlag::KReliable);
    original.m_sequence = 123456789;
    original.m_ack = 987654321;
    original.m_fragment_id = 55;
    original.m_fragment_index = 2;
    original.m_fragment_count = 10;
    original.m_payload_size = 100;
    original.m_checksum = 0;

    auto bytes = original.serialize();
    auto deserialized = PacketHeader::deserialize(bytes);

    EXPECT_EQ(deserialized.m_magic, k_magic_number);
    EXPECT_EQ(deserialized.m_command, original.m_command);
    EXPECT_EQ(deserialized.m_flags, original.m_flags);
    EXPECT_EQ(deserialized.m_sequence, original.m_sequence);
    EXPECT_EQ(deserialized.m_ack, original.m_ack);
    EXPECT_EQ(deserialized.m_fragment_id, original.m_fragment_id);
    EXPECT_EQ(deserialized.m_fragment_index, original.m_fragment_index);
    EXPECT_EQ(deserialized.m_fragment_count, original.m_fragment_count);
    EXPECT_EQ(deserialized.m_payload_size, original.m_payload_size);
}

TEST(PacketTest, PacketToBufferFromBuffer) {
    Packet original{};
    original.header.m_command = 10;
    original.header.m_sequence = 1;
    original.payload.push_back(std::byte{0x01});
    original.payload.push_back(std::byte{0x02});
    original.payload.push_back(std::byte{0x03});

    auto buffer = original.to_buffer();

    ASSERT_GE(buffer.size(), k_header_size);

    auto parsed = Packet::from_buffer(buffer);

    EXPECT_EQ(parsed.header.m_command, 10);
    EXPECT_EQ(parsed.header.m_sequence, 1);
    EXPECT_EQ(parsed.payload.size(), 3);
    EXPECT_EQ(parsed.payload[0], std::byte{0x01});
    EXPECT_EQ(parsed.payload[1], std::byte{0x02});
    EXPECT_EQ(parsed.payload[2], std::byte{0x03});
}

TEST(PacketTest, InvalidMagic) {
    std::vector<std::uint8_t> buffer(k_header_size, 0);

    EXPECT_THROW(PacketHeader::deserialize(std::span<const std::uint8_t, k_header_size>(buffer.data(), k_header_size)), std::runtime_error);
}

TEST(PacketTest, ChecksumMismatch) {
    Packet original{};
    original.header.m_command = 10;
    original.payload.push_back(std::byte{0x01});

    auto buffer = original.to_buffer();

    buffer[k_header_size] = 0xFF;

    EXPECT_THROW(Packet::from_buffer(buffer), std::runtime_error);
}

TEST(PacketTest, PayloadTooLarge) {
    Packet original{};
    original.payload.resize(k_max_payload_size + 1);

    EXPECT_THROW((void)original.to_buffer(), std::runtime_error);
}
