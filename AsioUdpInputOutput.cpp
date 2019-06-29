#include "luna/esp32/AsioUdpInputOutput.hpp"

#include <mbedtls/net_sockets.h>

namespace luna::esp32
{
    AsioUdpInputOutput::AsioUdpInputOutput(asio::ip::udp::socket * socket) :
        mSocket(socket)
    {}

    AsioUdpInputOutput::~AsioUdpInputOutput() = default;

    mbedtls_ssl_send_t * AsioUdpInputOutput::getSender()
    {
        return [](void * context, const unsigned char * data, size_t length) {
            return static_cast<AsioUdpInputOutput *>(context)->write(data, length);
        };
    }

    mbedtls_ssl_recv_t * AsioUdpInputOutput::getReceiver()
    {
        return [](void * context, unsigned char * data, size_t length) -> int {
            return static_cast<AsioUdpInputOutput *>(context)->read(data, length);
        };
    }

    mbedtls_ssl_recv_timeout_t * AsioUdpInputOutput::getReceiverTimeout()
    {
        return nullptr;
    }

    void * AsioUdpInputOutput::getContext()
    {
        return static_cast<void *>(this);
    }

    int AsioUdpInputOutput::write(unsigned char const * data, size_t length)
    {
        asio::error_code error;
        auto size = mSocket->send(asio::const_buffer(data, length), 0, error);

        if (!error) {
            return size;
        } else if (error == asio::error::connection_reset || error == asio::error::broken_pipe) {
            return MBEDTLS_ERR_NET_CONN_RESET;
        } else if (error == asio::error::interrupted) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        } else {
            return MBEDTLS_ERR_NET_SEND_FAILED; 
        }
    }

    int AsioUdpInputOutput::read(unsigned char * data, size_t length)
    {
        asio::error_code error;
        auto size = mSocket->receive(asio::buffer(data, length));

        if (!error) {
            return size;
        } else if (error == asio::error::connection_reset || error == asio::error::broken_pipe) {
            return MBEDTLS_ERR_NET_CONN_RESET;
        } else if (error == asio::error::interrupted || error == asio::error::would_block) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        } else {
            return MBEDTLS_ERR_NET_RECV_FAILED; 
        }
    }
}
