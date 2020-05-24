#pragma once

#include "AsioUdpInputOutput.hpp"
#include "AsioTimer.hpp"

#include <luna/NetworkingContext.hpp>
#include <luna/LunaContext.hpp>

#include <mbedtls-cpp/Configuration.hpp>
#include <mbedtls-cpp/Ssl.hpp>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/steady_timer.hpp>

#include <memory>

namespace luna
{
    struct ControllerMux;
    struct DirectController;

    struct RealtimeStreamer
    {
        explicit RealtimeStreamer(LunaContext const & context, NetworkingContext const & network, DirectController * controller);
        ~RealtimeStreamer();

        uint16_t port();

    private:
        void reset();
        void startHandshake();
        void doHandshake();
        void startRead();

        void activate();

        void dispatchCommand(std::byte const * data, size_t size);

        EventLoop * const mMainLoop;
        ControllerMux * const mMultiplexer;
        std::unique_ptr<tls::Configuration> mTlsConfiguration;
        DirectController * const mController;

        tls::Ssl mSsl;

        asio::ip::udp::socket mSocket;
        asio::steady_timer mHeartbeat;
        AsioTimer mTimer;
        AsioUdpInputOutput mIo;
    };
}
