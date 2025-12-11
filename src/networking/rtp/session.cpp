#include "networking.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <numeric>

namespace net
{
Session::Session(boost::asio::io_context& context, const boost::asio::ip::udp::endpoint& remote,
    ReliabilityConfig config, std::uint16_t localPort)
    : m_transport(std::make_shared<UdpTransport>(context, localPort)), m_config(config),
      m_send_queue(config), m_retransmit_timer(context)
{
    m_transport->set_default_remote(remote);
}

void Session::start(PacketCallback onReliable, PacketCallback onUnreliable)
{
    m_reliable_callback = std::move(onReliable);
    m_unreliable_callback = std::move(onUnreliable);
    m_started = true;
    auto self = shared_from_this();
    m_transport->start([self](const boost::system::error_code& ec, Packet packet,
        const boost::asio::ip::udp::endpoint& endpoint)
        { self->handle_packet(ec, std::move(packet), endpoint); });
    schedule_retransmission();
}

void Session::send(Packet packet, bool reliable)
{
    if (!m_transport)
    {
        throw std::logic_error("transport is not initialized");
    }
    if (!m_transport->has_default_remote())
    {
        throw std::logic_error("no default remote endpoint configured");
    }
    const auto k_endpoint = m_transport->default_remote();
    send(std::move(packet), k_endpoint, reliable);
}

void Session::send(Packet packet, const boost::asio::ip::udp::endpoint& endpoint, bool reliable)
{
    if (!m_started)
    {
        throw std::logic_error("session not started");
    }

    if (packet.payload.size() > m_fragment_payload_size)
    {
        fragment_and_send(std::move(packet), endpoint, reliable);
    }
    else
    {
        send_single_packet(std::move(packet), endpoint, reliable);
    }

    schedule_retransmission();
}

void Session::poll()
{
    if (!m_started)
    {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    auto due = m_send_queue.collect_timeouts(now);
    for (Packet& packet : due)
    {
        packet.header.m_ack = m_receive_window.ack();
        m_transport->async_send(packet);
    }
    m_failed_cache = m_send_queue.take_failures();
    schedule_retransmission();
}

const std::vector<std::uint32_t>& Session::failed_sequences() const noexcept
{
    return m_failed_cache;
}

void Session::handle_packet(const boost::system::error_code& ec, Packet packet,
    const boost::asio::ip::udp::endpoint& endpoint)
{
    if (ec)
    {
        return;
    }

    m_send_queue.acknowledge(packet.header.m_ack);

    if (packet.payload.size() > m_fragment_payload_size)
    {
        return;
    }

    bool is_reliable = has_flag(packet.header.m_flags, PacketFlag::k_reliable);
    const bool k_is_ack_packet = has_flag(packet.header.m_flags, PacketFlag::k_ack);
    const bool k_is_fragment = has_flag(packet.header.m_flags, PacketFlag::k_fragment);

    if (is_reliable)
    {
        m_receive_window.observe(packet.header.m_sequence);
        Packet ack_packet{};
        ack_packet.header.m_command = static_cast<std::uint8_t>(CommandId::k_ack);
        ack_packet.header.m_flags = static_cast<std::uint8_t>(PacketFlag::k_ack);
        ack_packet.header.m_ack = m_receive_window.ack();
        m_transport->async_send(ack_packet, endpoint);
    }

    if (k_is_ack_packet && !is_reliable)
    {
        return;
    }

    if (k_is_fragment)
    {
        auto assembled = ingest_fragment(std::move(packet));
        if (!assembled.has_value())
        {
            schedule_retransmission();
            return;
        }
        packet = std::move(assembled.value());
        is_reliable = has_flag(packet.header.m_flags, PacketFlag::k_reliable);
    }

    if (is_reliable)
    {
        if (m_reliable_callback)
        {
            m_reliable_callback(packet, endpoint);
        }
    }
    else
    {
        if (m_unreliable_callback)
        {
            m_unreliable_callback(packet, endpoint);
        }
    }

    schedule_retransmission();
}

void Session::set_fragment_payload_size(std::size_t fragmentPayloadSize)
{
    if (fragmentPayloadSize == 0 || fragmentPayloadSize > k_max_payload_size)
    {
        m_fragment_payload_size = k_max_payload_size;
        return;
    }
    m_fragment_payload_size = fragmentPayloadSize;
}

std::size_t Session::fragment_payload_size() const noexcept
{
    return m_fragment_payload_size;
}

void Session::send_single_packet(Packet packet, const boost::asio::ip::udp::endpoint& endpoint,
    bool reliable)
{
    auto now = std::chrono::steady_clock::now();

    if (reliable || has_flag(packet.header.m_flags, PacketFlag::k_reliable))
    {
        packet.header.m_flags = set_flag(packet.header.m_flags, PacketFlag::k_reliable);
        packet.header.m_sequence = m_send_queue.next_sequence();
        packet.header.m_ack = m_receive_window.ack();
        m_send_queue.track(packet, now);
    }
    else
    {
        packet.header.m_flags = clear_flag(packet.header.m_flags, PacketFlag::k_reliable);
        packet.header.m_sequence = 0;
        packet.header.m_ack = m_receive_window.ack();
    }

    m_transport->async_send(packet, endpoint);
    m_failed_cache.clear();
}

void Session::fragment_and_send(Packet packet, const boost::asio::ip::udp::endpoint& endpoint,
    bool reliable)
{
    reliable = true; // RFC: fragmented messages must be reliable

    const std::size_t k_total_size = packet.payload.size();
    const std::size_t k_fragment_size = m_fragment_payload_size;
    const std::size_t k_fragment_count =
        (k_total_size + k_fragment_size - 1) / k_fragment_size;

    if (k_fragment_count == 0 || k_fragment_count > k_max_fragments ||
        k_fragment_count > std::numeric_limits<std::uint8_t>::max())
    {
        throw std::runtime_error("payload too large to fragment");
    }

    const std::uint16_t k_fragment_id = m_next_fragment_id++;
    std::size_t offset = 0;
    for (std::size_t i = 0; i < k_fragment_count; ++i)
    {
        const std::size_t k_chunk_size =
            std::min(k_fragment_size, k_total_size - offset);

        Packet fragment{};
        fragment.header = packet.header;
        fragment.header.m_flags = set_flag(fragment.header.m_flags, PacketFlag::k_fragment);
        fragment.header.m_fragmentId = k_fragment_id;
        fragment.header.m_fragmentIndex = static_cast<std::uint8_t>(i);
        fragment.header.m_fragmentCount = static_cast<std::uint8_t>(k_fragment_count);
        fragment.payload.assign(packet.payload.begin() + static_cast<std::ptrdiff_t>(offset),
            packet.payload.begin() + static_cast<std::ptrdiff_t>(offset + k_chunk_size));

        send_single_packet(std::move(fragment), endpoint, reliable);
        offset += k_chunk_size;
    }
}

std::optional<Packet> Session::ingest_fragment(Packet packet)
{
    cleanup_fragment_buffers();

    if (packet.header.m_fragmentCount == 0 ||
        packet.header.m_fragmentIndex >= packet.header.m_fragmentCount ||
        static_cast<std::size_t>(packet.header.m_fragmentCount) > k_max_fragments)
    {
        return std::nullopt;
    }

    if (packet.payload.size() > m_fragment_payload_size)
    {
        return std::nullopt;
    }

    if (packet.header.m_fragmentCount == 0 ||
        packet.header.m_fragmentIndex >= packet.header.m_fragmentCount)
    {
        return std::nullopt;
    }

    auto& buffer = m_fragment_buffers[packet.header.m_fragmentId];
    const bool k_new_buffer = buffer.m_parts.empty();
    if (k_new_buffer || buffer.m_parts.size() != packet.header.m_fragmentCount)
    {
        if (k_new_buffer && m_fragment_buffers.size() >= k_max_inflight_reassemblies)
        {
            auto oldest = std::ranges::min_element(m_fragment_buffers, [](const auto& lhs, const auto& rhs) {
                return lhs.second.m_created_at < rhs.second.m_created_at;
            });
            if (oldest != m_fragment_buffers.end())
            {
                m_fragment_buffers.erase(oldest);
            }
        }
        buffer = FragmentBuffer{};
        buffer.m_header = packet.header;
        buffer.m_parts.resize(packet.header.m_fragmentCount);
        buffer.m_reliable = has_flag(packet.header.m_flags, PacketFlag::k_reliable);
        buffer.m_created_at = std::chrono::steady_clock::now();
        buffer.m_total_bytes = 0;
    }

    const std::size_t k_index = packet.header.m_fragmentIndex;
    if (buffer.m_parts[k_index].empty())
    {
        buffer.m_parts[k_index] = std::move(packet.payload);
        buffer.m_total_bytes += buffer.m_parts[k_index].size();
        ++buffer.m_received;
    }

    if (buffer.m_total_bytes > k_max_reassembly_bytes)
    {
        m_fragment_buffers.erase(packet.header.m_fragmentId);
        return std::nullopt;
    }

    if (buffer.m_received != buffer.m_parts.size())
    {
        return std::nullopt;
    }

    Packet assembled = rebuild_packet(buffer);
    m_fragment_buffers.erase(packet.header.m_fragmentId);
    return assembled;
}

void Session::cleanup_fragment_buffers()
{
    auto now = std::chrono::steady_clock::now();
    for (auto it = m_fragment_buffers.begin(); it != m_fragment_buffers.end();)
    {
        bool erase = false;
        if (it->second.m_created_at != std::chrono::steady_clock::time_point{} &&
            now - it->second.m_created_at >= k_fragment_reassembly_timeout)
        {
            erase = true;
        }
        if (it->second.m_total_bytes > k_max_reassembly_bytes)
        {
            erase = true;
        }

        if (erase)
        {
            it = m_fragment_buffers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

Packet Session::rebuild_packet(FragmentBuffer& buffer)
{
    Packet assembled{};
    assembled.header = buffer.m_header;
    assembled.header.m_flags = clear_flag(assembled.header.m_flags, PacketFlag::k_fragment);
    assembled.header.m_fragmentId = 0;
    assembled.header.m_fragmentIndex = 0;
    assembled.header.m_fragmentCount = 0;

    const std::size_t k_total_size =
        std::accumulate(buffer.m_parts.begin(), buffer.m_parts.end(), std::size_t{0},
            [](std::size_t sum, const std::vector<std::byte>& part) { return sum + part.size(); });
    assembled.payload.reserve(k_total_size);
    for (const auto& part : buffer.m_parts)
    {
        assembled.payload.insert(assembled.payload.end(), part.begin(), part.end());
    }

    return assembled;
}

void Session::schedule_retransmission()
{
    if (!m_started)
    {
        return;
    }

    auto next_delay = m_send_queue.time_until_next_timeout(std::chrono::steady_clock::now());
    if (!next_delay.has_value())
    {
        m_retransmit_timer.cancel();
        return;
    }

    m_retransmit_timer.expires_after(next_delay.value());
    auto self = shared_from_this();
    m_retransmit_timer.async_wait(
        [self](const boost::system::error_code& ec)
        -> void {
            if (!ec)
            {
                self->poll();
            }
        });
}
}  // namespace net
