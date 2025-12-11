---
title: Implementation Details
id: implementation-details
---

# Networking API Reference (concise)

This page provides a compact API reference for the networking module. Focused on the public types, functions, and methods you will call from client/server code.

Source: `src/networking/rtp/networking.h`, `src/networking/rtp/*.cpp`, `src/networking/handshake/*`.

Packet & header
- `struct PacketHeader`
	- Fields: `m_magic`, `m_command`, `m_flags`, `m_sequence`, `m_ack`, `m_fragmentId`, `m_fragmentIndex`, `m_fragmentCount`, `m_payloadSize`, `m_checksum`.
	- `std::array<uint8_t, k_header_size> serialize() const` — produce header bytes (little-endian).
	- `static PacketHeader deserialize(std::span<const uint8_t, k_header_size>)` — parse header and validate magic/payload size.

- `struct Packet`
	- Fields: `PacketHeader header`, `std::vector<std::byte> payload` (raw payload bytes stored as `std::byte`).
	- `std::vector<uint8_t> to_buffer() const` — serialize header+payload into the on-wire `uint8_t` buffer and compute CRC.
	- `static Packet from_buffer(std::span<const uint8_t>)` — parse full packet from an on-wire `uint8_t` buffer and validate checksum.

Enums & flags
- `enum class CommandId : uint8_t` — command identifiers (e.g. `k_req_login`, `k_res_login`, `k_client_input`, `k_server_entity_state`, `k_ack`).
- `enum class PacketFlag : uint8_t` — `k_reliable`, `k_fragment`, `k_ack`, `k_error`.
- Helpers: `has_flag(uint8_t mask, PacketFlag)`, `set_flag(uint8_t mask, PacketFlag)`, `clear_flag(...)` and bitwise operators.

Reliability primitives
- `struct ReliabilityConfig` — parameters: `maxRetransmissions`, `initialRto`, `maxRto`, `windowSize`.

- `class ReliableSendQueue`
	- Constructor: `ReliableSendQueue(ReliabilityConfig config = {})`
	- `uint32_t next_sequence()` — returns next sequence id (increments counter).
	- `void track(const Packet& packet, time_point now)` — start tracking a sent reliable packet.
	- `void acknowledge(uint32_t ackId)` — remove packets up to `ackId` from the queue.
	- `std::vector<Packet> collect_timeouts(time_point now)` — return packets that need retransmission.
	- `optional<milliseconds> time_until_next_timeout(time_point now) const` — time until next retransmit.
	- `std::vector<uint32_t> take_failures()` — retrieve sequences that exhausted retries.

- `class ReliableReceiveWindow`
	- `void observe(uint32_t sequence)` — update window with observed sequence.
	- `uint32_t ack() const noexcept` — get highest contiguous ack id.

Transport
- `class UdpTransport : enable_shared_from_this<UdpTransport>`
	- Constructor: `UdpTransport(io_context&, uint16_t localPort = 0)` — bind UDP socket.
	- `void set_default_remote(const udp::endpoint&)` — store default remote endpoint.
	- `bool has_default_remote() const noexcept` — check default endpoint.
	- `const udp::endpoint& default_remote() const` — get default endpoint (throws if not set).
	- `void start(PacketHandler handler)` — start async receive loop; `PacketHandler` is `function<void(const error_code&, Packet, const udp::endpoint&)>`.
	- `void async_send(const Packet& packet)` — send to default remote (throws if none).
	- `void async_send(const Packet& packet, const udp::endpoint& endpoint, SendHandler handler = {})` — send to endpoint; `SendHandler` is `function<void(const error_code&, const Packet&)>`.
	- `void close()` — close socket and stop receives.

Session (high-level)
- `class Session : enable_shared_from_this<Session>`
	- Constructor: `Session(io_context&, const udp::endpoint& remote, ReliabilityConfig config = {}, uint16_t localPort = 0)` — creates `UdpTransport`, configures remote.
	- `using PacketCallback = function<void(const Packet&, const udp::endpoint&)>` — callback type.
	- `void start(PacketCallback onReliable, PacketCallback onUnreliable)` — begin receiving and enable retransmit timers.
	- `void send(Packet packet, bool reliable = false)` — send to configured default remote.
	- `void send(Packet packet, const udp::endpoint& endpoint, bool reliable = false)` — send to explicit endpoint.
	- `void set_fragment_payload_size(size_t)` / `size_t fragment_payload_size() const` — set/get negotiated fragment payload size.
	- `void poll()` — check retransmissions and send due packets (also invoked by internal timer).
	- `const std::vector<uint32_t>& failed_sequences() const noexcept` — sequences that exhausted retries.

Handshake helpers (namespace `net::handshake`)
- `struct ReqLogin { string m_username; uint32_t m_version; uint16_t m_preferred_fragment_size; }`
- `struct ResLogin { bool m_success; uint32_t m_player_id; uint16_t m_effective_fragment_size; }`
- `Packet make_req_login(const ReqLogin&)` — build REQ_LOGIN packet payload.
- `Packet make_res_login(const ResLogin&)` — build RES_LOGIN packet payload.
- `optional<ReqLogin> parse_req_login(const Packet&)` — parse and validate REQ_LOGIN.
- `optional<ResLogin> parse_res_login(const Packet&)` — parse and validate RES_LOGIN.
- `bool handle_server_handshake(const Packet& packet, const shared_ptr<Session>& session, const udp::endpoint& endpoint)` — convenience: parse incoming REQ_LOGIN, set fragment size, send RES_LOGIN (returns true when handled).

Quick usage notes
- To receive reliable messages: call `session->start(onReliableCallback, onUnreliableCallback)`.
- To send reliably: set `packet.header.m_flags` to include `k_reliable` or call `session->send(packet, true)`.
- For large payloads: `Session` will fragment automatically (fragments are always sent reliably).

Where to look in code
- `src/networking/rtp/networking.h` — primary API types and method signatures.
- `src/networking/rtp/packet.cpp` — serialization and checksum behavior.
- `src/networking/rtp/reliability.cpp` — retransmission and window logic.
- `src/networking/rtp/session.cpp` — session behavior, fragmentation, and callbacks.
- `src/networking/handshake/*` — login handshake helpers.
