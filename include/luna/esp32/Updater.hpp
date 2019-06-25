#pragma once

#include "mbedtls-cpp/Configuration.hpp"
#include "mbedtls-cpp/Ssl.hpp"

#include <lwip_async/AsioTcpInputOutput.hpp>

#include <esp_ota_ops.h>
#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>

namespace luna::esp32
{

    struct Updater
    {
        explicit Updater(asio::io_service & ioService, tls::Configuration * tlsConfiguration);
    private:
        void acceptConnection();
        void doHandshake();
        void doUpdate();
        void reset();
        
        tls::Ssl mSsl;

        asio::ip::tcp::acceptor mListeningSocket;
        asio::ip::tcp::socket mStreamSocket;
        lwip_async::AsioTcpInputOutput mIo;

        esp_partition_t const * mOtaPartition;
        esp_ota_handle_t mOtaHandle;
    };

}
