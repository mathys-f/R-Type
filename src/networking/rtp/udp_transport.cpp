#include "networking.h"

#include <memory>

namespace net
{
UdpTransport::UdpTransport(boost::asio::io_context& context, std::uint16_t localPort)
    : m_socket(context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), localPort))
{
}

void UdpTransport::set_default_remote(const boost::asio::ip::udp::endpoint& endpoint)
{
    m_default_remote = endpoint;
}

bool UdpTransport::has_default_remote() const noexcept
{
    return m_default_remote.has_value();
}

const boost::asio::ip::udp::endpoint& UdpTransport::default_remote() const
{
    if (!m_default_remote.has_value())
    {
        throw std::logic_error("default remote endpoint is not set");
    }
    return *m_default_remote;
}

void UdpTransport::start(PacketHandler handler)
{
    m_handler = std::move(handler);
    m_running = true;
    do_receive();
}

void UdpTransport::async_send(const Packet& packet)
{
    if (!m_default_remote.has_value())
    {
        throw std::logic_error("default remote endpoint is not set");
    }
    async_send(packet, *m_default_remote);
}

void UdpTransport::async_send(const Packet& packet, const boost::asio::ip::udp::endpoint& endpoint,
    SendHandler handler)
{
    auto self = shared_from_this();
    auto buffer = std::make_shared<std::vector<std::uint8_t>>(packet.to_buffer());
    m_socket.async_send_to(
        boost::asio::buffer(*buffer), endpoint,
        [self, buffer, handler, packet](const boost::system::error_code& ec, std::size_t) mutable
        -> void {
            if (handler)
            {
                handler(ec, packet);
            }
        });
}

void UdpTransport::close()
{
    m_running = false;
    boost::system::error_code ec;
    m_socket.close(ec);
}

void UdpTransport::do_receive()
{
    if (!m_running)
    {
        return;
    }

    auto self = shared_from_this();
    m_socket.async_receive_from(
        boost::asio::buffer(m_buffer), m_sender,
        [self](const boost::system::error_code& error_code, std::size_t bytesTransferred)
        -> void {
            if (!self->m_running)
            {
                return;
            }

            if (error_code)
            {
                if (self->m_handler)
                {
                    self->m_handler(error_code, Packet{}, self->m_sender);
                }
            }
            else
            {
                try
                {
                    Packet packet =
                        Packet::from_buffer(std::span(self->m_buffer.data(), bytesTransferred));
                    if (self->m_handler)
                    {
                        self->m_handler(error_code, std::move(packet), self->m_sender);
                    }
                }
                catch (const std::exception&)
                {
                    if (self->m_handler)
                    {
                        boost::system::error_code decode_error =
                            boost::system::errc::make_error_code(
                                boost::system::errc::illegal_byte_sequence);
                        self->m_handler(decode_error, Packet{}, self->m_sender);
                    }
                }
            }

            self->do_receive();
        });
}
}  // namespace net
