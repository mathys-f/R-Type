#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include "boost/interprocess/ipc/message_queue.hpp"

namespace ipc {

// IPC message types
enum class MessageType : std::uint8_t {
    HEARTBEAT = 1,
    PLAYERCOUNT = 2,
    SHUTDOWNREQ = 3,
    SHUTDOWNACK = 4,
};

// IPC message structure (fixed size for message queue)
struct IPCMessage {
    MessageType type;
    std::uint32_t lobby_id;
    std::uint32_t data;      // Generic data field (e.g., player count)
    char extra[64];          // Extra string data if needed

    IPCMessage() : type(MessageType::HEARTBEAT), lobby_id(0), data(0), extra{0} {}
};

// LobbyIPC manages bidirectional communication between main server and lobby processes
class LobbyIPC {
  public:
    // Create IPC channels for a specific lobby
    explicit LobbyIPC(std::uint32_t lobby_id);
    ~LobbyIPC();

    LobbyIPC(const LobbyIPC&) = delete;
    LobbyIPC& operator=(const LobbyIPC&) = delete;
    LobbyIPC(LobbyIPC&&) = delete;
    LobbyIPC& operator=(LobbyIPC&&) = delete;

    // Main server side methods
    bool try_receive_from_lobby(IPCMessage& msg, unsigned int timeout_ms = 0);
    bool send_to_lobby(const IPCMessage& msg);

    // Lobby process side methods
    bool try_receive_from_main(IPCMessage& msg, unsigned int timeout_ms = 0);
    bool send_to_main(const IPCMessage& msg);

    // Cleanup IPC resources (call from main server after process ends)
    static void cleanup_lobby_ipc(std::uint32_t lobby_id);

    std::uint32_t get_lobby_id() const { return m_lobby_id; }

  private:
    std::string get_main_to_lobby_queue_name() const;
    std::string get_lobby_to_main_queue_name() const;

    std::uint32_t m_lobby_id;

    // Message queues (boost::interprocess)
    std::unique_ptr<boost::interprocess::message_queue> m_main_to_lobby_queue;
    std::unique_ptr<boost::interprocess::message_queue> m_lobby_to_main_queue;

    static constexpr std::size_t k_max_messages = 1000;  // Increased from 10 to handle high-frequency multiplayer updates
    static constexpr std::size_t k_message_size = sizeof(IPCMessage);
};

} // namespace ipc
