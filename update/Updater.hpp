#pragma once

#include "AsioTcpInputOutput.hpp"

#include <luna/NetworkService.hpp>

#include <mbedtls-cpp/Configuration.hpp>
#include <mbedtls-cpp/Ssl.hpp>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include <esp_ota_ops.h>

#include <memory>

namespace luna
{
    struct Updater : NetworkService
    {
        explicit Updater(asio::io_context * ioContext, std::unique_ptr<tls::Configuration> && tlsConfiguration);
        ~Updater() final;
    private:
        void acceptConnection();
        void doHandshake();
        void doUpdate();
        void reset();

        std::unique_ptr<tls::Configuration> mTlsConfiguration;
        tls::Ssl mSsl;

        asio::ip::tcp::acceptor mListeningSocket;
        asio::ip::tcp::socket mStreamSocket;
        lwip_async::AsioTcpInputOutput mIo;

        esp_partition_t const * mOtaPartition;
        esp_ota_handle_t mOtaHandle;
    };

}
