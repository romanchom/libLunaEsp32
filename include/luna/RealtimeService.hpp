#pragma once

#include "AsioUdpInputOutput.hpp"
#include "AsioTimer.hpp"

#include <mbedtls-cpp/Configuration.hpp>
#include <mbedtls-cpp/Ssl.hpp>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/steady_timer.hpp>

#include <memory>

namespace luna
{
    struct DirectService;

    struct RealtimeService
    {
        explicit RealtimeService(asio::io_context * ioContext, std::unique_ptr<tls::Configuration> && tlsConfiguration, DirectService * service);
        ~RealtimeService();

        uint16_t port();

    private:
        void reset();
        void startHandshake();
        void doHandshake();
        void startRead();

        void activate();

        void dispatchCommand(std::byte const * data, size_t size);

        std::unique_ptr<tls::Configuration> mTlsConfiguration;
        DirectService * mService;

        tls::Ssl mSsl;

        asio::ip::udp::socket mSocket;
        asio::steady_timer mHeartbeat;
        AsioTimer mTimer;
        AsioUdpInputOutput mIo;
    };
}
