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

Every UDP datagram transmitted by either Client or Server MUST begin with the following 16-byte fixed header.

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
* **Flags (8 bits):** A bitmask defining packet behavior.
    * `0x01` (RELIABLE): The receiver MUST send an acknowledgment.
    * `0x02` (IS_FRAGMENT): The payload is part of a larger message.
    * `0x03` (IS_ACK): The packet is an acknowledgment; payload is empty.
    * `0x04` (IS_ERROR): The packet indicates an error condition. The payload contains an error code and message.
* **Sequence ID (32 bits):** A monotonic counter incremented by the sender for every new packet. Used for ordering and loss detection.
* **Ack ID (32 bits):** The `Sequence ID` of the last valid reliable packet received by the sender.
* **Fragment ID (16 bits):** A unique identifier grouping fragmented chunks.
* **Frag Index (8 bits):** The current chunk index (0 to N).
* **Total Frags (8 bits):** The total number of chunks.
* **Payload Size (16 bits):** The size of the data following the header, in bytes.
* **Checksum (16 bits):** A simple checksum (e.g., CRC16) covering the entire packet (header + payload) for data integrity verification. (the checksum field is set to 0 during checksum calculation).

# Reliability Mechanism

The protocol implements a selective reliability layer.

## Unreliable Transmission
Packets with the `RELIABLE` flag unset (0) are "Fire-and-Forget". These are used for high-frequency data such as Entity Position Updates (60Hz). If such a packet is lost, no action is taken.

## Reliable Transmission
Packets with the `RELIABLE` flag set (1) MUST be acknowledged.
1.  **Sender:** MUST store the packet in a "Sent Queue" with a timestamp.
2.  **Receiver:** Upon receiving a Reliable packet, MUST send a response packet with `CommandID = ACK (0xFF)` and the `Ack ID` set to the received packet's Sequence ID.
3.  **Retransmission:** If the sender does not receive an ACK within **200ms**, it MUST resend the packet.

# Fragmentation

The Maximum Transmission Unit (MTU) safe limit is defined as **1400 bytes**. Payloads exceeding this size MUST be fragmented.

1.  The payload is split into chunks of 1004 bytes.
2.  Each chunk is sent as a separate packet with the `IS_FRAGMENT` flag set.
3.  All fragments MUST be sent as **RELIABLE**.
4.  The receiver MUST buffer fragments until all `Total Frags` are received before reassembling and processing the message.

# Command Definitions

The following Command IDs are reserved.

## Management (Reliable)
* **0x01 (REQ_LOGIN):** Client requests connection. Payload: `{ char username[32]; uint32_t version; }`
* **0x02 (RES_LOGIN):** Server response. Payload: `{ uint8_t success; uint32_t playerId; }`
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

Implementations MUST adhere to the following security guidelines to prevent Denial of Service (DoS) and execution flaws:

1.  **Buffer Bounds Checking:** The receiver MUST verify that `bytes_transferred == sizeof(Header) + Header.payloadSize`. Mismatched sizes MUST result in packet rejection.
2.  **Max Payload Enforcement:** Any packet indicating a `payloadSize` > 1400 bytes MUST be dropped immediately to prevent buffer overflow attacks.
3.  **Magic Number Validation:** The `Magic Number` check MUST occur before any memory allocation or logic processing.
4.  **Sequence Validation:** The receiver SHOULD discard Reliable packets with `Sequence ID` significantly lower than the last processed ID to prevent Replay Attacks.

# IANA Considerations {#IANA}

This document has no IANA actions. The protocol operates on a user-defined UDP port (Default: 4242).

--- back

# Acknowledgements {#Acknowledgements}
{: numbered="false"}

Developed by the Epitech R-Type Serveur Robuste.

<!--
To transform this file into a proper RFC, the following steps are needed:
1. Install the kramdown-rfc2629 tool (gem install kramdown-rfc2629)
2. Install the xml2rfc tool (pip install xml2rfc)
3. Execute kramdown-rfc2629 to convert this markdown into XML:
   kramdown-rfc2629 docs/network-rfc.md > docs/network-rfc.xml
4. Validate and generate the final RFC text formats using xml2rfc:
   xml2rfc docs/network-rfc.xml
-->
