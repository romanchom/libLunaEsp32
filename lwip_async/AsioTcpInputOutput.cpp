#include "AsioTcpInputOutput.hpp"

#include <algorithm>
#include <cstring>

#include <esp_log.h>
#include <mbedtls/net_sockets.h>

#include <lwip/timeouts.h>

static auto const TAG = "ASIO_IO";

namespace lwip_async
{
    AsioTcpInputOutput::AsioTcpInputOutput(asio::ip::tcp::socket * socket) :
        mSocket(socket)
    {}

    AsioTcpInputOutput::~AsioTcpInputOutput() = default;

    mbedtls_ssl_send_t * AsioTcpInputOutput::getSender()
    {
        return [](void * context, const unsigned char * data, size_t length) {
            return static_cast<AsioTcpInputOutput *>(context)->write(data, length);
        };
    }

    mbedtls_ssl_recv_t * AsioTcpInputOutput::getReceiver()
    {
        return [](void * context, unsigned char * data, size_t length) -> int {
            return static_cast<AsioTcpInputOutput *>(context)->read(data, length);
        };
    }

    mbedtls_ssl_recv_timeout_t * AsioTcpInputOutput::getReceiverTimeout()
    {
        return nullptr;
    }

    void * AsioTcpInputOutput::getContext()
    {
        return static_cast<void *>(this);
    }

    int AsioTcpInputOutput::write(unsigned char const * data, size_t length)
    {
        asio::error_code error;
        auto size = mSocket->write_some(asio::const_buffer(data, length), error);

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

    int AsioTcpInputOutput::read(unsigned char * data, size_t length)
    {
        asio::error_code error;
        auto size = mSocket->read_some(asio::buffer(data, length), error);

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
