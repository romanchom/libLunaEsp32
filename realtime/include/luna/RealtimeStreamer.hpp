#pragma once

#include "AsioUdpInputOutput.hpp"
#include "AsioTimer.hpp"

#include <luna/LunaInterface.hpp>

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
        explicit RealtimeStreamer(LunaInterface * luna, DirectController * controller, ControllerHandle * handle, std::unique_ptr<tls::Configuration> dtlsConfig, asio::io_context * ioContext);
        ~RealtimeStreamer();

        uint16_t port();

    private:
        void reset();
        void startHandshake();
        void doHandshake();
        void startRead();

        void activate();

        void dispatchCommand(std::byte const * data, size_t size);

        LunaInterface * mLuna;
        DirectController * mController;
        ControllerHandle * mHandle;
        std::unique_ptr<tls::Configuration> mTlsConfiguration;

        tls::Ssl mSsl;

        asio::ip::udp::socket mSocket;
        asio::steady_timer mHeartbeat;
        AsioTimer mTimer;
        AsioUdpInputOutput mIo;
    };
}
