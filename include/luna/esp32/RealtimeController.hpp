#pragma once

#include "HardwareController.hpp"

#include "luna/esp32/AsioUdpInputOutput.hpp"
#include "luna/esp32/AsioTimer.hpp"
#include "mbedtls-cpp/Configuration.hpp"
#include "mbedtls-cpp/Ssl.hpp"

#include <luna/proto/SetColor.hpp>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/steady_timer.hpp>

namespace luna::esp32
{
    struct RealtimeController
    {
        RealtimeController(asio::io_context * ioContext, tls::Configuration * tlsConfiguration, HardwareController * controller);
        uint16_t port();
    private:
        void reset();
        void startHandshake();
        void doHandshake();
        void startRead();

        void activate();

        void dispatchCommand(std::byte const * data, size_t size);

        void setColor(luna::proto::SetColor const& cmd);
        
        HardwareController * mController;

        tls::Ssl mSsl;

        asio::ip::udp::socket mSocket;
        asio::steady_timer mHeartbeat;
        AsioTimer mTimer;
        AsioUdpInputOutput mIo;
    };
}