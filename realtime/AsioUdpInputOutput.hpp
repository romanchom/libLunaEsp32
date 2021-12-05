#pragma once

#include <mbedtls-cpp/BasicInputOutput.hpp>

#include <asio/ip/udp.hpp>

#include <array>
#include <cstddef>

namespace luna
{
    struct AsioUdpInputOutput : tls::BasicInputOutput
    {
    public:
        explicit AsioUdpInputOutput(asio::ip::udp::socket * socket);
        ~AsioUdpInputOutput() override;

        mbedtls_ssl_send_t * getSender() override;
        mbedtls_ssl_recv_t * getReceiver() override;
        mbedtls_ssl_recv_timeout_t * getReceiverTimeout() override;
        void * getContext() override;

        asio::ip::udp::socket * mSocket;

    private:
        int read(unsigned char * data, size_t length);
        int write(unsigned char const * data, size_t length);
    };
}
