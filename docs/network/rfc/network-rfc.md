---
title: Network RFC for R-Type Protocol
abbrev: R-Type
docname: draft-pintard-rtype-00
lang: en
date: 2025-11-25
category: info
ipr: trust200902
area: Internet
workgroup: R-Type Working Group
keyword: [R-Type, Network Protocol, RFC, UDP]

author:
  - ins: L. Pintard
    name: Léopold Pintard
    organization: Epitech
    email: leopold.pintard@epitech.eu

---

--- abstract

This document describes the R-Type Protocol (RTP), a custom binary application-layer protocol designed for real-time multiplayer gaming. RTP provides a hybrid reliability model over UDP, supporting both unreliable high-frequency state updates and reliable control messages through a custom acknowledgment system. This document defines the packet structure, serialization rules, reliability mechanisms, and command definitions required for interoperability between Client and Server implementation.

--- middle

# Introduction

The R-Type project requires a high-performance networking architecture capable of handling fast-paced arcade gameplay. Standard TCP implementations introduce Head-of-Line (HOL) blocking and Nagle’s algorithm delays, which are unsuitable for real-time entity synchronization.

The R-Type Protocol (RTP) is designed to run exclusively over UDP (User Datagram Protocol). To mitigate the inherent unreliability of UDP, RTP implements a lightweight application-layer reliability mechanism (RUDP) for critical data, while allowing transient game state data to remain unreliable for maximum throughput.

## Terminology

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "NOT RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in BCP 14 {{!RFC2119}} {{!RFC8174}} when, and only when, they appear in all capitals, as shown here.

# Data Representation

## Byte Ordering

All multi-byte numeric fields (uint16, uint32, float) MUST be serialized in **Little Endian** byte order. This decision aligns with the native architecture of modern gaming platforms (x86_64 and ARM64), facilitating "Zero-Copy" deserialization strategies.

## Structure Alignment

All data structures transmitted over the network MUST be packed with 1-byte alignment (`#pragma pack(1)`) to prevent compiler-inserted padding bytes from altering the packet size.

# Protocol Header Structure

Every UDP datagram transmitted by either Client or Server MUST begin with the following 20-byte fixed header.

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

## Field Definitions

* **Magic Number (16 bits):** MUST be `0xD1CE`. This serves as a protocol identifier and a sanity check against random network noise. Packets with invalid magic numbers MUST be dropped immediately.
* **Command ID (8 bits):** Identifies the type of payload (e.g., Login, Move, Shoot).
* **Flags (8 bits):** A bitmask defining packet behavior. Each flag occupies a distinct bit; multiple flags MAY be combined using bitwise OR.
    * `0x01` (RELIABLE): The receiver MUST send an acknowledgment (see Reliability Mechanism).
    * `0x02` (IS_FRAGMENT): The payload is part of a larger message and carries fragment fields.
    * `0x04` (IS_ACK): The packet is an explicit acknowledgment; payload SHOULD be empty.
    * `0x08` (IS_ERROR): The packet indicates an error condition. The payload contains an error code and human-readable message.
* **Sequence ID (32 bits):** A monotonic counter incremented by the sender for every new packet. Used for ordering and loss detection.
* **Ack ID (32 bits):** The `Sequence ID` of the last valid reliable packet received by the sender.
* **Fragment ID (16 bits):** A unique identifier grouping fragmented chunks.
* **Frag Index (8 bits):** The current chunk index (0 to N).
* **Total Frags (8 bits):** The total number of chunks.
* **Payload Size (16 bits):** The size of the data following the header, in bytes.
* **Checksum (16 bits):** A checksum covering the entire packet (header + payload) for data integrity verification. Implementations SHOULD use CRC-16-CCITT (poly=0x1021, init=0xFFFF, reflected=false) and MUST set the checksum field to `0` while computing it.

# Reliability Mechanism

The protocol implements a selective reliability layer.

## Unreliable Transmission
Packets with the `RELIABLE` flag unset (0) are "Fire-and-Forget". These are used for high-frequency data such as Entity Position Updates (60Hz). If such a packet is lost, no action is taken.

## Reliable Transmission
Packets with the `RELIABLE` flag set (bit `0x01`) MUST be acknowledged.

1. **Sender:** MUST store the packet in a "Sent Queue" with a timestamp and track retransmission attempts.
2. **Receiver:** Upon receiving a Reliable packet, the receiver SHOULD either:
    - piggyback the `Ack ID` (highest contiguous Sequence ID received) in the header of any outgoing packet to that peer, or
    - send an explicit ACK packet (set `IS_ACK` flag and `CommandID = ACK (0xFF)`) containing the `Ack ID` of the received packet.
    Piggybacking is preferred for efficiency; explicit ACKs are allowed when no outgoing packet can carry the `Ack ID` promptly.
3. **Ack semantics:** `Ack ID` in the header is interpreted as a cumulative ACK: it acknowledges all reliable packets with `Sequence ID <= Ack ID`. Implementations MAY include selective-acknowledgment (SACK) extensions in a control payload for finer-grained loss reporting.
4. **Retransmission / RTO:** Implementations SHOULD measure RTT from reliable packet/ACK exchanges and compute a retransmission timeout (RTO) using a standard algorithm (e.g., TCP's SRTT/RTO calculation). An implementation MAY start with a conservative initial RTO of 200ms, but MUST apply exponential backoff on repeated retransmissions (RTO *= 2) and SHOULD jitter timers to avoid synchronization. Implementations MUST cap retransmission attempts (RECOMMENDED default: 5 attempts). If no ACK is received after the maximum retries, the sender SHOULD consider the peer unreachable and close the session.
5. **Congestion & pacing:** Because RTP runs over UDP, implementations MUST avoid aggressive retransmission that could exacerbate congestion. A simple send-pacing or token-bucket rate limiter for retransmissions is RECOMMENDED.

# Fragmentation

The Maximum Transmission Unit (MTU) safe limit is defined as **1400 bytes**. Payloads exceeding this size MUST be fragmented.

1. The `Payload Size` field is the size of this packet's payload only (not the total message length).
2. The payload SHOULD be split into fragment payloads of size equal to the session's negotiated `effective_fragment_size` (see `REQ_LOGIN`/`RES_LOGIN`). If no negotiation occurs, implementations SHOULD use the default application fragment payload of **1004 bytes**. Implementations MUST ensure the chosen fragment payload does not exceed the path-MTU-derived limit (`MTU - header_size`) to avoid IP fragmentation; using the document's conservative MTU assumption (1400) yields an upper bound of `1400 - 20 = 1380` bytes per fragment payload. Implementations MAY choose a slightly smaller fragment payload to allow for additional lower-layer headers.
3. Each fragment packet MUST set the `IS_FRAGMENT` flag and populate `Fragment ID`, `Frag Index` and `Total Frags`.
4. All fragments of a logical message MUST be sent as **RELIABLE**.
5. **Reassembly limits & timeouts:** The receiver MUST validate `Frag Index < Total Frags` and SHOULD impose limits to mitigate resource exhaustion: a recommended `MAX_FRAGS_PER_MESSAGE = 256`, per-message reassembly timeout `FRAGMENT_REASSEMBLY_TIMEOUT = 5s`, and per-sender reassembly memory cap (RECOMMENDED default: 1 MiB). Partially received fragment sets MUST be discarded after timeout or when resource caps are exceeded.
6. **Fragment ID lifetime:** `Fragment ID` is a 16-bit identifier scoped to the sender; implementations MUST avoid reusing a given `Fragment ID` for new fragmented messages while an older reassembly for that ID could still be in progress (recommendation: reuse only after `FRAGMENT_REUSE_DELAY = 60s` or after wrap-around detection).
7. The receiver MUST buffer fragments until all `Total Frags` are received before reassembling and processing the message. Reassembly MUST include validation using the per-packet checksum and, optionally, a message-level checksum supplied in the first fragment's payload for large messages.

Default fragment sizing and negotiation

 - **Default (recommended):** For broad compatibility and efficient buffer pooling, RTP RECOMMENDS a default application fragment payload of **1004 bytes**. This corresponds to a UDP target payload of **1024 bytes** (2^10) and leaves room for the 20-byte RTP header (app_fragment = UDP_target_payload - header_size => 1024 - 20 = 1004). Choosing 1024 as the UDP target payload produces power-of-two friendly buffers while staying well below common MTU limits to avoid IP fragmentation on most Internet paths.
 - **Alternative safe values:** Implementations may prefer a larger UDP target payload (e.g., 1200 → app_fragment 1180, or 1400 → app_fragment 1380) for higher throughput in controlled networks; however, these are not power-of-two and may risk fragmentation on narrow-path MTUs.
 - **Negotiation:** To allow endpoints to select an appropriate fragment payload for their deployment, RTP supports optional fragment-size negotiation during login (see `REQ_LOGIN`/`RES_LOGIN` below). Servers MUST select an `effective_fragment_size` that does not exceed their configured per-packet limit and SHOULD choose a conservative value when interacting with unknown networks.

Formula: `app_fragment = UDP_target_payload - header_size` (header_size = 20).

# Command Definitions

The following Command IDs are reserved.

## Management (Reliable)
* **0x01 (REQ_LOGIN):** Client requests connection. Payload: `{ uint8_t username_len; char username[variable]; uint32_t version; uint16_t preferred_fragment_size; }` where `username_len <= 32` and `username` is UTF-8 bytes (not NUL-terminated). `preferred_fragment_size` is optional (set to `0` if not used) and expresses the client's preferred application-fragment payload in bytes (e.g., `1004`). Use of length-prefixed strings avoids ambiguity and buffer overrun risks.
* **0x02 (RES_LOGIN):** Server response. Payload: `{ uint8_t success; uint32_t playerId; uint16_t effective_fragment_size; }` where `effective_fragment_size` is the per-packet application-fragment payload the server agrees to use for this session (e.g., `1004`). If `success == 0`, the `effective_fragment_size` MAY be set to `0`.
* **0x03 (REQ_JOIN_ROOM):** Client joins a lobby. Payload: `{ uint32_t roomId; }`
* **0x04 (RES_ROOM_STATE):** Room info.

## Gameplay (Unreliable)
* **0x10 (C_INPUT):** Client input state. Payload: `{ uint8_t inputMask; }` (Bitmask: Up, Down, Left, Right, Shoot).
* **0x11 (S_ENTITY_STATE):** World update. Uses coordinate quantization.
    * Payload: `{ uint32_t entityId; uint16_t x; uint16_t y; uint8_t type; }`

## Gameplay (Reliable)
* **0x20 (S_PLAYER_DEATH):** Notification of entity destruction.
* **0x21 (S_SCORE_UPDATE):** Global score change.

# Security Considerations {#Security}

Implementations MUST adhere to the following security and operational guidelines to prevent Denial of Service (DoS), replay, and resource exhaustion attacks:

1. **Magic Number Validation:** The `Magic Number` check MUST occur immediately upon packet receipt and before any memory allocation or further processing. Packets with invalid magic numbers MUST be dropped.
2. **Buffer Bounds Checking:** The receiver MUST verify that `bytes_transferred == sizeof(Header) + Header.payloadSize`. Mismatched sizes MUST result in immediate packet rejection with no further processing.
3. **Max Payload Enforcement:** Any packet indicating a `payloadSize` > 1400 bytes MUST be dropped immediately to prevent buffer overflow attacks. For fragmented messages, the per-packet `payloadSize` MUST still respect this limit.
4. **Sequence Validation & Replay Protection:** The receiver SHOULD discard reliable packets with `Sequence ID` significantly lower than the last processed ID. Implementations MUST define a clear sequence-number comparison (see Recommendations): for 32-bit sequence numbers, treat a sequence `s1` as newer than `s2` if `(s1 - s2) mod 2^32 < 2^31`.
5. **Rate Limiting & Resource Caps:** Servers MUST apply per-client rate limits for incoming packets and a per-client resource cap for buffered/reassembled fragments (recommendation: 1 MiB). Excessive or malformed traffic SHOULD be dropped and, when abusive, result in temporary throttling or disconnection.
6. **Fragmentation DoS Mitigations:** Limit the number of simultaneous incomplete fragment reassemblies per client (RECOMMENDED default: 8) and enforce `FRAGMENT_REASSEMBLY_TIMEOUT`. Reject new fragmented streams when resource quotas are exhausted.
7. **Retransmission & Backoff:** As noted in the Reliability section, implementations MUST cap retransmission attempts and apply exponential backoff to avoid retransmit storms that amplify congestion.
8. **Handshake & Authentication:** Implementations SHOULD perform a handshake (e.g., `REQ_LOGIN`/`RES_LOGIN`) before allocating significant state for a client. Servers SHOULD issue a short-lived session token on successful login and require that token in subsequent sensitive control messages. For stronger security, use authenticated cryptographic tokens and negotiate an HMAC key for packet authentication.
9. **Integrity / Authentication:** CRC checks detect accidental corruption but do not prevent malicious tampering.
10. **NAT Traversal & Keepalive:** Because RTP runs over UDP, clients SHOULD send periodic keepalive/ping packets (RECOMMENDED interval: 15s) to maintain NAT mappings. Servers SHOULD drop idle sessions after a configurable idle timeout (RECOMMENDED default: 60s) unless keepalives are observed.

# IANA Considerations {#IANA}

This document has no IANA actions. The protocol operates on a user-defined UDP port (Default: 8080).

--- back

# Acknowledgements {#Acknowledgements}
{: numbered="false"}

Developed by the Epitech R-Type Serveur Robuste.

<!--
To transform this file into a proper RFC, the following steps are needed:
1. Install the kramdown-rfc2629 tool (gem install kramdown-rfc2629)
2. Install the xml2rfc tool (pip install xml2rfc)
3. Execute kramdown-rfc2629 to convert this markdown into XML:
   kramdown-rfc2629 docs/network/rfc/network-rfc.md > docs/network/rfc/network-rfc.xml
4. Validate and generate the final RFC text formats using xml2rfc:
   xml2rfc docs/network/rfc/network-rfc.xml
5. Optionally, remove the intermediate XML file:
   rm -f docs/network/rfc/network-rfc.xml

Full command line ->
    kramdown-rfc2629 docs/network/rfc/network-rfc.md | xml2rfc docs/network/rfc/network-rfc.xml | rm -f docs/network/rfc/network-rfc.xml
-->
