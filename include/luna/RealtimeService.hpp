#pragma once

#include "AsioUdpInputOutput.hpp"
#include "AsioTimer.hpp"

#include <mbedtls-cpp/Configuration.hpp>
#include <mbedtls-cpp/Ssl.hpp>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/steady_timer.hpp>

namespace luna
{
    struct DirectService;

    struct RealtimeService
    {
        RealtimeService(asio::io_context * ioContext, tls::Configuration * tlsConfiguration, DirectService * service);
        ~RealtimeService();

        uint16_t port();

    private:
        void reset();
        void startHandshake();
        void doHandshake();
        void startRead();

        void activate();

        void dispatchCommand(std::byte const * data, size_t size);

        DirectService * mService;

        tls::Ssl mSsl;

        asio::ip::udp::socket mSocket;
        asio::steady_timer mHeartbeat;
        AsioTimer mTimer;
        AsioUdpInputOutput mIo;
    };
}
