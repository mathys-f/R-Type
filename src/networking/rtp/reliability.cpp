#include "networking.h"

#include <algorithm>

namespace net {
namespace {
// Returns true when lhs is considered newer than rhs under 32-bit wrap-around semantics.
constexpr bool is_seq_newer(std::uint32_t lhs, std::uint32_t rhs) noexcept {
    return static_cast<std::int32_t>(lhs - rhs) > 0;
}

// Returns true when lhs is older-than-or-equal to rhs under wrap-around semantics.
constexpr bool is_seq_later_than_or_equal(std::uint32_t lhs, std::uint32_t rhs) noexcept {
    return !is_seq_newer(lhs, rhs);
}
} // namespace

ReliableSendQueue::ReliableSendQueue(ReliabilityConfig config) : m_config(config) {}

std::uint32_t ReliableSendQueue::next_sequence() {
    const std::uint32_t k_current = m_next_sequence;
    ++m_next_sequence;
    return k_current;
}

void ReliableSendQueue::track(const Packet& packet, std::chrono::steady_clock::time_point now, const asio::ip::udp::endpoint& endpoint) {
    m_queue.push_back(Pending{.m_packet = packet, .m_last_sent = now, .m_attempts = 1, .m_rto = m_config.initial_rto, .m_endpoint = endpoint});
}

void ReliableSendQueue::acknowledge(std::uint32_t ackId, const asio::ip::udp::endpoint& endpoint) {
    if (ackId == 0) {
        return;
    }
    for (auto it = m_queue.begin(); it != m_queue.end();) {
        Pending& pending = *it;
        if (pending.m_endpoint == endpoint && !is_seq_newer(pending.m_packet.header.m_sequence, ackId)) {
            it = m_queue.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<Packet> ReliableSendQueue::collect_timeouts(std::chrono::steady_clock::time_point now) {
    std::vector<Packet> due{};
    for (auto it = m_queue.begin(); it != m_queue.end();) {
        Pending& pending = *it;

        if (pending.m_attempts >= m_config.max_retransmissions) {
            m_failed.push_back(pending.m_packet.header.m_sequence);
            it = m_queue.erase(it);
            continue;
        }

        const auto k_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pending.m_last_sent);
        if (k_elapsed >= pending.m_rto) {
            pending.m_last_sent = now;
            ++pending.m_attempts;
            pending.m_rto = std::min(pending.m_rto * 2, m_config.max_rto);
            due.push_back(pending.m_packet);
        }
        ++it;
    }
    return due;
}

std::optional<std::chrono::milliseconds>
ReliableSendQueue::time_until_next_timeout(std::chrono::steady_clock::time_point now) const {
    std::optional<std::chrono::milliseconds> result;
    for (const Pending& pending : m_queue) {
        if (pending.m_attempts >= m_config.max_retransmissions) {
            continue;
        }
        const auto k_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pending.m_last_sent);
        if (k_elapsed >= pending.m_rto) {
            return std::chrono::milliseconds::zero();
        }
        const auto k_remaining = pending.m_rto - k_elapsed;
        if (!result.has_value() || k_remaining < result.value()) {
            result = k_remaining;
        }
    }
    return result;
}

std::vector<std::uint32_t> ReliableSendQueue::take_failures() {
    auto failures = std::move(m_failed);
    m_failed.clear();
    return failures;
}

DeliveryStatus ReliableSendQueue::is_acknowledged(std::uint32_t sequence, const asio::ip::udp::endpoint& endpoint) const {
    if (sequence == 0) {
        return DeliveryStatus::Failed;
    }

    if (!is_seq_newer(m_next_sequence, sequence)) {
        return DeliveryStatus::Pending;
    }

    for (const Pending& pending : m_queue) {
        if (pending.m_packet.header.m_sequence == sequence && pending.m_endpoint == endpoint) {
            const auto k_now = std::chrono::steady_clock::now();
            const auto k_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(k_now - pending.m_last_sent);
            if (k_elapsed >= pending.m_rto) {
                return DeliveryStatus::TimedOut;
            }
            return DeliveryStatus::Pending;
        }
    }

    for (std::uint32_t failed_seq : m_failed) {
        if (failed_seq == sequence) {
            return DeliveryStatus::Failed;
        }
    }

    return DeliveryStatus::Acknowledged;
}


void ReliableReceiveWindow::observe(std::uint32_t sequence) {
    if (sequence == 0) {
        return;
    }

    if (is_seq_newer(sequence, m_highest_contiguous)) {
        m_highest_contiguous = sequence;
    }
}

std::uint32_t ReliableReceiveWindow::ack() const noexcept {
    return m_highest_contiguous;
}
} // namespace net
