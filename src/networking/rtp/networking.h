#pragma once

#include <array>
#include <asio.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

namespace net {
constexpr std::uint16_t k_magic_number = 0xD1CE; // Magic number defined in the RFC
constexpr std::size_t k_header_size = 20;        // Size of the packet (see RFC for details)
constexpr std::size_t k_max_payload_size = 1004; // Maximum payload size (1024 - header size)
constexpr std::size_t k_max_packet_size = k_header_size + k_max_payload_size; // 1024
constexpr std::size_t k_max_fragments = 256;
constexpr std::size_t k_max_inflight_reassemblies = 8;
constexpr std::size_t k_max_reassembly_bytes = 1 << 20; // 1 MiB
constexpr std::chrono::seconds k_fragment_reassembly_timeout{5};
constexpr std::size_t k_default_max_retransmissions = 5;
constexpr std::chrono::milliseconds k_default_initial_rto{200};
constexpr std::chrono::milliseconds k_default_max_rto{2000};
constexpr std::size_t k_default_window_size = 256;
constexpr std::uint32_t k_byte_mask = 0xFFU;

// Command identifiers for different packet types.
// TODO: Add commands for the game
enum class CommandId : std::uint8_t {
    KReqLogin = 0x01,
    KResLogin = 0x02,
    KReqJoinRoom = 0x03,
    KResRoomState = 0x04,
    KClientInput = 0x10,
    KServerEntityState = 0x11,
    KServerPlayerDeath = 0x20,
    KServerScoreUpdate = 0x21,
    KAck = 0xFF
};

// Flags used in the packet header to indicate special properties.
enum class PacketFlag : std::uint8_t { KReliable = 0x01, KFragment = 0x02, KAck = 0x04, KError = 0x08 };

// Bitwise OR operator use for combining flags.
constexpr PacketFlag operator|(PacketFlag lhs, PacketFlag rhs) {
    return static_cast<PacketFlag>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

// Bitwise AND operator use for checking flags.
constexpr PacketFlag operator&(PacketFlag lhs, PacketFlag rhs) {
    return static_cast<PacketFlag>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

// Bitwise OR assignment operator for PacketFlag.
constexpr PacketFlag& operator|=(PacketFlag& lhs, PacketFlag rhs) {
    lhs = lhs | rhs;
    return lhs;
}

// Checks if a specific flag is set in the mask.
constexpr bool has_flag(std::uint8_t mask, PacketFlag flag) {
    return (mask & static_cast<std::uint8_t>(flag)) != 0;
}

// Sets a specific flag in the mask.
constexpr std::uint8_t set_flag(std::uint8_t mask, PacketFlag flag) {
    return mask | static_cast<std::uint8_t>(flag);
}

// Clears a specific flag in the mask.
constexpr std::uint8_t clear_flag(std::uint8_t mask, PacketFlag flag) {
    return mask & ~static_cast<std::uint8_t>(flag);
}

// Helpers for converting to/from std::byte for payload manipulation.
constexpr std::byte to_byte(std::uint32_t v) noexcept {
    return static_cast<std::byte>(static_cast<unsigned char>(v & k_byte_mask));
}

constexpr std::uint8_t byte_to_u8(std::byte b) noexcept {
    return static_cast<std::uint8_t>(b);
}

// Structure that represents the header of a network packet.
struct PacketHeader {
    std::uint16_t m_magic = k_magic_number;
    std::uint8_t m_command = 0;
    std::uint8_t m_flags = 0;
    std::uint32_t m_sequence = 0;
    std::uint32_t m_ack = 0;
    std::uint16_t m_fragment_id = 0;
    std::uint8_t m_fragment_index = 0;
    std::uint8_t m_fragment_count = 0;
    std::uint16_t m_payload_size = 0;
    std::uint16_t m_checksum = 0;

    /**
     * Serializes the header into a packed little-endian byte array.
     */
    [[nodiscard]] std::array<std::uint8_t, k_header_size> serialize() const;
    /**
     * Rebuilds a header from a serialized fixed-size buffer.
     */
    static PacketHeader deserialize(std::span<const std::uint8_t, k_header_size> buffer);
};

// Structure that represents a complete network packet, including header and payload.
struct Packet {
    PacketHeader header{};
    std::vector<std::byte> payload{};

    /**
     * Serializes a packet into a contiguous buffer while computing its checksum.
     */
    [[nodiscard]] std::vector<std::uint8_t> to_buffer() const;
    /**
     * Parses a packet from a contiguous buffer, validating size and checksum.
     */
    static Packet from_buffer(std::span<const std::uint8_t> buffer);
};

// Configuration parameters for reliability mechanisms.
// RTO - Retransmission Timeout
// Window Size - Number of packets that can be sent without acknowledgment
struct ReliabilityConfig {
    std::size_t max_retransmissions = k_default_max_retransmissions;
    std::chrono::milliseconds initial_rto{k_default_initial_rto};
    std::chrono::milliseconds max_rto{k_default_max_rto};
    std::size_t window_size = k_default_window_size;
};

// Queue that manages reliable packet sending with retransmission logic.
class ReliableSendQueue {
  public:
    /**
     * Builds a send queue with the requested reliability configuration.
     */
    explicit ReliableSendQueue(ReliabilityConfig config = {});

    // Returns the next sequence identifier and increments the internal counter.
    std::uint32_t next_sequence();

    /**
     * Tracks a reliable packet that has just been transmitted.
     */
    void track(const Packet& packet, std::chrono::steady_clock::time_point now);
    /**
     * Removes packets covered by the latest acknowledgement identifier.
     */
    void acknowledge(std::uint32_t ackId);
    /**
     * Returns packets that have exceeded their retransmission timeout.
     */
    std::vector<Packet> collect_timeouts(std::chrono::steady_clock::time_point now);
    /**
     * Reports the amount of time remaining until the next retransmission event.
     */
    [[nodiscard]] std::optional<std::chrono::milliseconds>
    time_until_next_timeout(std::chrono::steady_clock::time_point now) const;
    /**
     * Moves the list of sequence numbers that exhausted retransmission attempts.
     */
    std::vector<std::uint32_t> take_failures();

  private:
    struct Pending {
        Packet m_packet;
        std::chrono::steady_clock::time_point m_last_sent;
        std::size_t m_attempts = 0;
        std::chrono::milliseconds m_rto{};
    };

    std::deque<Pending> m_queue{};
    ReliabilityConfig m_config{};
    std::uint32_t m_next_sequence = 1;
    std::vector<std::uint32_t> m_failed{};
};

// Window that manages the reception of reliable packets and tracks acknowledgments.
class ReliableReceiveWindow {
  public:
    /**
     * Updates the receive window with a newly observed sequence number.
     */
    void observe(std::uint32_t sequence);
    /**
     * Returns the highest contiguous acknowledgement identifier.
     */
    [[nodiscard]] std::uint32_t ack() const noexcept;

  private:
    std::uint32_t m_highest_contiguous = 0;
};

// Low level UDP transport abstraction.
class UdpTransport : public std::enable_shared_from_this<UdpTransport> {
  public:
    using PacketHandler = std::function<void(const asio::error_code&, Packet, const asio::ip::udp::endpoint&)>;
    using SendHandler = std::function<void(const asio::error_code&, const Packet&)>;

    /**
     * Constructs a UDP transport bound to the specified local port.
     */
    explicit UdpTransport(asio::io_context& context, std::uint16_t localPort = 0);

    /**
     * Stores the default remote endpoint used when no explicit destination is provided.
     */
    void set_default_remote(const asio::ip::udp::endpoint& endpoint);
    /**
     * Returns true when a default remote endpoint has been configured.
     */
    [[nodiscard]] bool has_default_remote() const noexcept;
    /**
     * Retrieves the configured default remote endpoint, throwing if none is set.
     */
    [[nodiscard]] const asio::ip::udp::endpoint& default_remote() const;
    /**
     * Starts the asynchronous receive loop with the provided callback.
     */
    void start(PacketHandler handler);
    /**
     * Sends a packet to the stored default remote endpoint.
     */
    void async_send(const Packet& packet);
    /**
     * Sends a packet to the specified endpoint.
     */
    void async_send(const Packet& packet, const asio::ip::udp::endpoint& endpoint, SendHandler handler = {});
    /**
     * Closes the underlying socket and stops the receive loop.
     */
    void close();

  private:
    void do_receive();

    asio::ip::udp::socket m_socket;
    std::optional<asio::ip::udp::endpoint> m_default_remote{};
    asio::ip::udp::endpoint m_sender{};
    std::array<std::uint8_t, k_max_packet_size> m_buffer{};
    PacketHandler m_handler{};
    bool m_running = false;
};

// High-level session that manages reliable and unreliable packet sending/receiving.
class Session : public std::enable_shared_from_this<Session> {
  public:
    using PacketCallback = std::function<void(const Packet&, const asio::ip::udp::endpoint&)>;

    /**
     * Constructs a session with its own transport instance and reliability bookkeeping.
     */
    Session(asio::io_context& context, const asio::ip::udp::endpoint& remote, ReliabilityConfig config = {},
            std::uint16_t localPort = 0);

    /**
     * Starts the session by registering callbacks and enabling retransmission timers.
     */
    void start(PacketCallback onReliable, PacketCallback onUnreliable);
    /**
     * Sends a packet to the default remote endpoint.
     */
    void send(Packet packet, bool reliable = false);
    /**
     * Sends a packet to the specified endpoint, optionally tracking it for reliability.
     */
    void send(Packet packet, const asio::ip::udp::endpoint& endpoint, bool reliable = false);
    /**
     * Updates the fragment payload size to a negotiated value (bounded by k_max_payload_size).
     */
    void set_fragment_payload_size(std::size_t fragmentPayloadSize);
    [[nodiscard]] std::size_t fragment_payload_size() const noexcept;
    /**
     * Polls the retransmission queue and reschedules the timer.
     */
    void poll();
    /**
     * Returns the list of sequence identifiers that exceeded retry limits.
     */
    [[nodiscard]] const std::vector<std::uint32_t>& failed_sequences() const noexcept;

  private:
    struct FragmentBuffer {
        PacketHeader m_header{};
        std::vector<std::vector<std::byte>> m_parts{};
        std::size_t m_received = 0;
        bool m_reliable = false;
        std::chrono::steady_clock::time_point m_created_at{};
        std::size_t m_total_bytes = 0;
    };

    /**
     * Sends a single packet, optionally tracking it for reliability.
     */
    void send_single_packet(Packet packet, const asio::ip::udp::endpoint& endpoint, bool reliable);
    /**
     * Fragments a large packet and sends the fragments reliably or unreliably.
     */
    void fragment_and_send(Packet packet, const asio::ip::udp::endpoint& endpoint, bool reliable);
    /**
     * Ingests a fragment and attempts to reassemble the full packet.
     */
    std::optional<Packet> ingest_fragment(Packet packet);
    [[nodiscard]] static Packet rebuild_packet(FragmentBuffer& buffer);
    void cleanup_fragment_buffers();

    /**
     * Handles an incoming packet, updating reliability state and dispatching callbacks.
     */
    void handle_packet(const asio::error_code& ec, Packet packet, const asio::ip::udp::endpoint& endpoint);

    /**
     * Schedules the next retransmission timer tick.
     */
    void schedule_retransmission();

    std::shared_ptr<UdpTransport> m_transport;
    ReliabilityConfig m_config{};
    ReliableSendQueue m_send_queue{};
    ReliableReceiveWindow m_receive_window{};
    PacketCallback m_reliable_callback{};
    PacketCallback m_unreliable_callback{};
    asio::steady_timer m_retransmit_timer;
    std::vector<std::uint32_t> m_failed_cache{};
    std::uint16_t m_next_fragment_id = 1;
    std::unordered_map<std::uint16_t, FragmentBuffer> m_fragment_buffers{};
    std::size_t m_fragment_payload_size = k_max_payload_size;
    bool m_started = false;
};
} // namespace net