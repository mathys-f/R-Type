---
title: RFC
id: rfc
slug: /networking/
---

# Network RFC for R-Type Protocol

This document describes the R-Type Protocol (RTP), a custom binary application-layer protocol designed for real-time multiplayer gaming. RTP provides a hybrid reliability model over UDP, supporting both unreliable high-frequency state updates and reliable control messages through a custom acknowledgment system. This document defines the packet structure, serialization rules, reliability mechanisms, and command definitions required for interoperability between Client and Server implementation.

## Introduction

The R-Type project requires a high-performance networking architecture capable of handling fast-paced arcade gameplay. Standard TCP implementations introduce Head-of-Line (HOL) blocking and Nagle’s algorithm delays, which are unsuitable for real-time entity synchronization.

The R-Type Protocol (RTP) is designed to run exclusively over UDP (User Datagram Protocol). To mitigate the inherent unreliability of UDP, RTP implements a lightweight application-layer reliability mechanism (RUDP) for critical data, while allowing transient game state data to remain unreliable for maximum throughput.

### Terminology

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "NOT RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in BCP 14 (RFC2119, RFC8174) when, and only when, they appear in all capitals, as shown here.

## Data Representation

### Byte Ordering

All multi-byte numeric fields (uint16, uint32, float) MUST be serialized in **Little Endian** byte order. This decision aligns with the native architecture of modern gaming platforms (x86_64 and ARM64), facilitating "Zero-Copy" deserialization strategies.

### Structure Alignment

All data structures transmitted over the network MUST be packed with 1-byte alignment (`#pragma pack(1)`) to prevent compiler-inserted padding bytes from altering the packet size.

## Protocol Header Structure

Every UDP datagram transmitted by either Client or Server MUST begin with the following 20-byte fixed header.

```
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |             Magic Number      |   Command ID  |     Flags     |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                          Sequence ID                          |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                            Ack ID                             |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |          Fragment ID          |   Frag Index  |  Total Frags  |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |          Payload Size         |            Checksum           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

### Field Definitions

* **Magic Number (16 bits):** MUST be `0xD1CE`. Serves as a protocol identifier; invalid packets MUST be dropped.
* **Command ID (8 bits):** Type of payload (Login, Move, Shoot, etc.).
* **Flags (8 bits):** Bitmask defining packet behavior:
  * `0x01` (RELIABLE): Receiver MUST send acknowledgment.
  * `0x02` (IS_FRAGMENT): Payload is part of a larger message.
  * `0x04` (IS_ACK): Explicit acknowledgment; payload SHOULD be empty.
  * `0x08` (IS_ERROR): Indicates an error; payload contains error code and message.
* **Sequence ID (32 bits):** Monotonic counter for ordering and loss detection.
* **Ack ID (32 bits):** `Sequence ID` of the last valid reliable packet received.
* **Fragment ID (16 bits):** Unique identifier for fragmented chunks.
* **Frag Index (8 bits):** Current chunk index (0 to N).
* **Total Frags (8 bits):** Total number of chunks.
* **Payload Size (16 bits):** Size of payload in bytes.
* **Checksum (16 bits):** Covers header + payload; implementations SHOULD use CRC-16-CCITT.

## Reliability Mechanism

### Unreliable Transmission
Packets with the `RELIABLE` flag unset are "Fire-and-Forget", used for high-frequency updates. Lost packets are ignored.

### Reliable Transmission
Packets with the `RELIABLE` flag MUST be acknowledged:

1. **Sender:** Store packet in "Sent Queue" with timestamp and track retransmissions.
2. **Receiver:** Should piggyback `Ack ID` or send explicit ACK packet (`IS_ACK` flag, `CommandID = 0xFF`).
3. **Ack semantics:** `Ack ID` acknowledges all reliable packets with `Sequence ID <= Ack ID`. Selective acknowledgment optional.
4. **Retransmission / RTO:** Measure RTT; apply exponential backoff; cap attempts (default 5). If no ACK, consider peer unreachable.
5. **Congestion & pacing:** Avoid aggressive retransmissions; implement send-pacing or rate limiter.

## Fragmentation

Maximum Transmission Unit (MTU) safe limit: **1400 bytes**. Payloads exceeding this MUST be fragmented.

* Payload Size = this packet's payload only.
* Fragment payload size = `effective_fragment_size` (default 1004 bytes if not negotiated).
* Each fragment sets `IS_FRAGMENT` flag and populates Fragment fields.
* All fragments MUST be **RELIABLE**.
* Reassembly: validate indices, enforce limits, discard on timeout or memory cap.
* Fragment ID reuse: avoid until old reassembly complete.
* Buffer fragments until all received; validate with checksum.

Default fragment sizing: recommended 1004 bytes; allows 20-byte RTP header. Negotiation optional during login.

## Command Definitions

### Management (Reliable)
* **0x01 (REQ_LOGIN):** Client requests connection.
* **0x02 (RES_LOGIN):** Server response.
* **0x03 (REQ_JOIN_ROOM):** Client joins a lobby.
* **0x04 (RES_ROOM_STATE):** Room info.

### Gameplay (Unreliable)
* **0x10 (C_INPUT):** Client input state.
* **0x11 (S_ENTITY_STATE):** World update.

### Gameplay (Reliable)
* **0x20 (S_PLAYER_DEATH):** Notification of entity destruction.
* **0x21 (S_SCORE_UPDATE):** Global score change.

## Security Considerations

1. **Magic Number Validation:** Must check immediately.
2. **Buffer Bounds Checking:** Ensure payload size matches header.
3. **Max Payload Enforcement:** Drop packets > 1400 bytes.
4. **Sequence Validation & Replay Protection:** Compare sequence numbers safely.
5. **Rate Limiting & Resource Caps:** Limit per-client memory and fragment reassembly.
6. **Fragmentation DoS Mitigations:** Limit simultaneous incomplete fragment reassemblies.
7. **Retransmission & Backoff:** Cap attempts, exponential backoff.
8. **Handshake & Authentication:** Require login handshake; use short-lived session tokens.
9. **Integrity / Authentication:** CRC detects accidental corruption.
10. **NAT Traversal & Keepalive:** Send periodic keepalive; drop idle sessions after 60s.

## IANA Considerations

No IANA actions. Protocol runs on user-defined UDP port (default 8080).

## Acknowledgements

Developed by the Epitech R-Type Serveur Robuste.
