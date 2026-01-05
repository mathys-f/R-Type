#include "networking.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace net {
// Create a namespace-scope anonymous to keep helper functions private to this C++ file.
namespace {
constexpr std::uint8_t k_byte_mask = 0xFFU;
constexpr std::uint8_t k_shift8 = 8U;
constexpr std::uint8_t k_shift16 = 16U;
constexpr std::uint8_t k_shift24 = 24U;
constexpr std::uint16_t k_crc_poly = 0x1021U;
constexpr std::uint16_t k_crc_init = 0xFFFFU;
constexpr std::uint16_t k_crc_high_bit = 0x8000U;
constexpr std::size_t k_bits_per_byte = 8;

constexpr std::size_t k_magic_number_offset = 0;
constexpr std::size_t k_command_offset = 2;
constexpr std::size_t k_flags_offset = 3;
constexpr std::size_t k_sequence_offset = 4;
constexpr std::size_t k_ack_offset = 8;
constexpr std::size_t k_fragment_id_offset = 12;
constexpr std::size_t k_fragment_index_offset = 14;
constexpr std::size_t k_fragment_count_offset = 15;
constexpr std::size_t k_payload_size_offset = 16;
constexpr std::size_t k_checksum_offset = 18;

// Computes a CRC-16-CCITT checksum for the provided buffer.
std::uint16_t crc16_ccitt(std::span<const std::uint8_t> buffer) noexcept {
    std::uint16_t crc = k_crc_init;
    for (std::uint8_t byte : buffer) {
        crc ^= static_cast<std::uint16_t>(byte) << k_shift8;
        for (std::size_t i = 0; i < k_bits_per_byte; ++i) {
            if ((crc & k_crc_high_bit) != 0U) {
                crc = static_cast<std::uint16_t>((crc << 1U) ^ k_crc_poly);
            } else {
                crc = static_cast<std::uint16_t>(crc << 1U);
            }
        }
    }
    return crc;
}

// Encodes a 16-bit integer into two little-endian bytes at the requested offset.
void write_u16(std::uint16_t value, std::span<std::uint8_t> target, std::size_t offset) noexcept {
    target[offset] = static_cast<std::uint8_t>(value & k_byte_mask);
    target[offset + 1] = static_cast<std::uint8_t>((value >> k_shift8) & k_byte_mask);
}

// Encodes a 32-bit integer into four little-endian bytes at the requested offset.
void write_u32(std::uint32_t value, std::span<std::uint8_t> target, std::size_t offset) noexcept {
    target[offset] = static_cast<std::uint8_t>(value & k_byte_mask);
    target[offset + 1] = static_cast<std::uint8_t>((value >> k_shift8) & k_byte_mask);
    target[offset + 2] = static_cast<std::uint8_t>((value >> k_shift16) & k_byte_mask);
    target[offset + 3] = static_cast<std::uint8_t>((value >> k_shift24) & k_byte_mask);
}

// Decodes a 16-bit integer from two little-endian bytes.
std::uint16_t read_u16(std::span<const std::uint8_t> source, std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(source[offset]) | static_cast<std::uint16_t>(source[offset + 1] << k_shift8);
}

// Decodes a 32-bit integer from four little-endian bytes.
std::uint32_t read_u32(std::span<const std::uint8_t> source, std::size_t offset) noexcept {
    return static_cast<std::uint32_t>(source[offset]) | (static_cast<std::uint32_t>(source[offset + 1]) << k_shift8) |
           (static_cast<std::uint32_t>(source[offset + 2]) << k_shift16) |
           (static_cast<std::uint32_t>(source[offset + 3]) << k_shift24);
}
} // namespace

std::array<std::uint8_t, k_header_size> PacketHeader::serialize() const {
    std::array<std::uint8_t, k_header_size> bytes{};
    auto view = std::span<std::uint8_t>(bytes.data(), bytes.size());
    write_u16(m_magic, view, k_magic_number_offset);
    bytes[k_command_offset] = m_command;
    bytes[k_flags_offset] = m_flags;
    write_u32(m_sequence, view, k_sequence_offset);
    write_u32(m_ack, view, k_ack_offset);
    write_u16(m_fragment_id, view, k_fragment_id_offset);
    bytes[k_fragment_index_offset] = m_fragment_index;
    bytes[k_fragment_count_offset] = m_fragment_count;
    write_u16(m_payload_size, view, k_payload_size_offset);
    write_u16(m_checksum, view, k_checksum_offset);
    return bytes;
}

PacketHeader PacketHeader::deserialize(std::span<const std::uint8_t, k_header_size> buffer) {
    PacketHeader header{};
    auto view = std::span<const std::uint8_t>(buffer.data(), buffer.size());
    header.m_magic = read_u16(view, k_magic_number_offset);
    header.m_command = buffer[k_command_offset];
    header.m_flags = buffer[k_flags_offset];
    header.m_sequence = read_u32(view, k_sequence_offset);
    header.m_ack = read_u32(view, k_ack_offset);
    header.m_fragment_id = read_u16(view, k_fragment_id_offset);
    header.m_fragment_index = buffer[k_fragment_index_offset];
    header.m_fragment_count = buffer[k_fragment_count_offset];
    header.m_payload_size = read_u16(view, k_payload_size_offset);
    header.m_checksum = read_u16(view, k_checksum_offset);
    if (header.m_magic != k_magic_number) {
        throw std::runtime_error("invalid magic number");
    }
    if (header.m_payload_size > k_max_payload_size) {
        throw std::runtime_error("payload too large");
    }

    return header;
}

std::vector<std::uint8_t> Packet::to_buffer() const {
    if (payload.size() > k_max_payload_size) {
        throw std::runtime_error("payload exceeds maximum size");
    }

    PacketHeader header_copy = header;
    header_copy.m_payload_size = static_cast<std::uint16_t>(payload.size());
    header_copy.m_checksum = 0;

    std::vector<std::uint8_t> buffer(k_header_size + payload.size());
    auto view = std::span<std::uint8_t>(buffer.data(), buffer.size());
    const auto k_header_bytes = header_copy.serialize();
    std::ranges::copy(k_header_bytes, buffer.begin());
    std::ranges::transform(payload, buffer.begin() + k_header_size, [](std::byte b) { return net::byte_to_u8(b); });
    const auto k_checksum = crc16_ccitt(buffer);
    write_u16(k_checksum, view, k_checksum_offset);

    return buffer;
}

Packet Packet::from_buffer(std::span<const std::uint8_t> buffer) {
    if (buffer.size() < k_header_size) {
        throw std::runtime_error("buffer too small");
    }

    std::array<std::uint8_t, k_header_size> header_bytes{};
    std::memcpy(header_bytes.data(), buffer.data(), k_header_size);
    PacketHeader header = PacketHeader::deserialize(header_bytes);

    const std::size_t k_expected_size = k_header_size + header.m_payload_size;
    if (buffer.size() != k_expected_size) {
        throw std::runtime_error("payload size mismatch");
    }

    std::vector<std::uint8_t> verification(buffer.begin(), buffer.end());
    verification[k_checksum_offset] = 0;
    verification[k_checksum_offset + 1] = 0;

    const std::uint16_t k_expected_checksum = crc16_ccitt(verification);
    if (k_expected_checksum != header.m_checksum) {
        throw std::runtime_error("checksum mismatch");
    }

    Packet packet{};
    packet.header = header;
    const auto k_payload_begin = buffer.begin() + k_header_size;
    const auto k_payload_end = buffer.end();
    packet.payload.resize(std::distance(k_payload_begin, k_payload_end));
    std::ranges::transform(k_payload_begin, k_payload_end, packet.payload.begin(),
                           [](std::uint8_t b) { return static_cast<std::byte>(b); });
    return packet;
}
} // namespace net
