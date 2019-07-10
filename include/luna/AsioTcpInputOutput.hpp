#pragma once

#include <mbedtls-cpp/BasicInputOutput.hpp>

#include <asio/ip/tcp.hpp>

#include <array>
#include <cstddef>

namespace lwip_async
{
    struct AsioTcpInputOutput : tls::BasicInputOutput
    {
    public:
        explicit AsioTcpInputOutput(asio::ip::tcp::socket * socket);
        ~AsioTcpInputOutput() override;

        mbedtls_ssl_send_t * getSender() override;
        mbedtls_ssl_recv_t * getReceiver() override;
        mbedtls_ssl_recv_timeout_t * getReceiverTimeout() override;
        void * getContext() override;

        asio::ip::tcp::socket * mSocket;

    private:
        int read(unsigned char * data, size_t length);
        int write(unsigned char const * data, size_t length);
    };
}
