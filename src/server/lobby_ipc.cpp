#include "lobby_ipc.h"
#include "utils/logger.h"

#include "boost/interprocess/ipc/message_queue.hpp"
#include <chrono>
#include <thread>

namespace ipc {

LobbyIPC::LobbyIPC(std::uint32_t lobby_id) : m_lobby_id(lobby_id) {
    namespace bip = boost::interprocess;

    std::string main_to_lobby_name = get_main_to_lobby_queue_name();
    std::string lobby_to_main_name = get_lobby_to_main_queue_name();

    bip::message_queue::remove(main_to_lobby_name.c_str());
    bip::message_queue::remove(lobby_to_main_name.c_str());

    try {
        // NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions)
        m_main_to_lobby_queue = std::make_unique<bip::message_queue>(
            bip::create_only,
            main_to_lobby_name.c_str(),
            k_max_messages,
            k_message_size
        );
    } catch (const bip::interprocess_exception& e) {
        LOG_ERROR("Failed to create main_to_lobby queue for lobby {}: {}", m_lobby_id, e.what());
        bip::message_queue::remove(main_to_lobby_name.c_str());
        throw;
    }

    try {
        // NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions)
        m_lobby_to_main_queue = std::make_unique<bip::message_queue>(
            bip::create_only,
            lobby_to_main_name.c_str(),
            k_max_messages,
            k_message_size
        );
    } catch (const bip::interprocess_exception& e) {
        LOG_ERROR("Failed to create lobby_to_main queue for lobby {}: {}", m_lobby_id, e.what());
        m_main_to_lobby_queue.reset();
        bip::message_queue::remove(main_to_lobby_name.c_str());
        bip::message_queue::remove(lobby_to_main_name.c_str());
        throw;
    }

    LOG_INFO("Created IPC message queues for lobby {}", m_lobby_id);
}

LobbyIPC::~LobbyIPC() {
}

bool LobbyIPC::try_receive_from_lobby(IPCMessage& msg, unsigned int timeout_ms) {
    namespace bip = boost::interprocess;

    try {
        std::size_t recvd_size = 0;
        unsigned int priority = 0;

        if (timeout_ms == 0) {
            return m_lobby_to_main_queue->try_receive(&msg, k_message_size, recvd_size, priority);
        } else {
            auto start = std::chrono::steady_clock::now();
            while (true) {
                if (m_lobby_to_main_queue->try_receive(&msg, k_message_size, recvd_size, priority)) {
                    return true;
                }
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start).count();
                if (elapsed >= timeout_ms) {
                    return false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    } catch (const bip::interprocess_exception& e) {
        LOG_ERROR("Error receiving from lobby {}: {}", m_lobby_id, e.what());
        return false;
    }
}

bool LobbyIPC::send_to_lobby(const IPCMessage& msg) {
    namespace bip = boost::interprocess;

    try {
        m_main_to_lobby_queue->send(&msg, k_message_size, 0);
        return true;
    } catch (const bip::interprocess_exception& e) {
        LOG_ERROR("Error sending to lobby {}: {}", m_lobby_id, e.what());
        return false;
    }
}

bool LobbyIPC::try_receive_from_main(IPCMessage& msg, unsigned int timeout_ms) {
    namespace bip = boost::interprocess;

    try {
        std::size_t recvd_size = 0;
        unsigned int priority = 0;

        if (timeout_ms == 0) {
            return m_main_to_lobby_queue->try_receive(&msg, k_message_size, recvd_size, priority);
        } else {
            auto start = std::chrono::steady_clock::now();
            while (true) {
                if (m_main_to_lobby_queue->try_receive(&msg, k_message_size, recvd_size, priority)) {
                    return true;
                }
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start).count();
                if (elapsed >= timeout_ms) {
                    return false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    } catch (const bip::interprocess_exception& e) {
        LOG_ERROR("Error receiving from main server: {}", e.what());
        return false;
    }
}

bool LobbyIPC::send_to_main(const IPCMessage& msg) {
    namespace bip = boost::interprocess;

    try {
        m_lobby_to_main_queue->send(&msg, k_message_size, 0);
        return true;
    } catch (const bip::interprocess_exception& e) {
        LOG_ERROR("Error sending to main server: {}", e.what());
        return false;
    }
}

void LobbyIPC::cleanup_lobby_ipc(std::uint32_t lobby_id) {
    namespace bip = boost::interprocess;

    std::string main_to_lobby = "/rtype_main_to_lobby_" + std::to_string(lobby_id);
    std::string lobby_to_main = "/rtype_lobby_to_main_" + std::to_string(lobby_id);

    bip::message_queue::remove(main_to_lobby.c_str());
    bip::message_queue::remove(lobby_to_main.c_str());

    LOG_DEBUG("Cleaned up IPC queues for lobby {}", lobby_id);
}

std::string LobbyIPC::get_main_to_lobby_queue_name() const {
    return "/rtype_main_to_lobby_" + std::to_string(m_lobby_id);
}

std::string LobbyIPC::get_lobby_to_main_queue_name() const {
    return "/rtype_lobby_to_main_" + std::to_string(m_lobby_id);
}

} // namespace ipc
