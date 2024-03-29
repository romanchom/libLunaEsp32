#include "UpdaterAsio.hpp"

#include <esp_log.h>

static char const TAG[] = "OTA";

namespace luna
{
    UpdaterAsio::UpdaterAsio(asio::io_context * ioContext, std::unique_ptr<tls::Configuration> && tlsConfiguration) :
        mTlsConfiguration(std::move(tlsConfiguration)),
        mListeningSocket(*ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 60000)),
        mStreamSocket(*ioContext),
        mIo(&mStreamSocket)
    {
        mSsl.setup(mTlsConfiguration.get());
        mSsl.setInputOutput(&mIo);

        mOtaPartition = esp_ota_get_next_update_partition(nullptr);

        acceptConnection();
    }

    UpdaterAsio::~UpdaterAsio() = default;

    void UpdaterAsio::acceptConnection()
    {
        mListeningSocket.async_accept(mStreamSocket, [this](asio::error_code const & error){
            if (!error) {
                ESP_LOGI(TAG, "Connected");
                mStreamSocket.non_blocking(true);
                doHandshake();
            } else {
                acceptConnection();
            }
        });
        ESP_LOGI(TAG, "Awaiting connection");
    }

    void UpdaterAsio::doHandshake()
    {
        mStreamSocket.async_wait(mStreamSocket.wait_read, [this](asio::error_code const & error) {
            for (;;) {
                auto const ret = mSsl.handshakeStep();

                if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                    doHandshake();
                    break;
                } else if (ret != ERR_OK) {
                    reset();
                    break;
                } else if (mSsl.inHandshakeOver()) {
                    ESP_LOGI(TAG, "Peer authenticated");
                    esp_ota_begin(mOtaPartition, OTA_SIZE_UNKNOWN, &mOtaHandle);
                    doUpdate();
                    ESP_LOGI(TAG, "Flashing");
                    break;
                }
            }
        });
    }

    void UpdaterAsio::doUpdate()
    {
        mStreamSocket.async_wait(mStreamSocket.wait_read, [this](asio::error_code const & error) {
            if (error) {
                reset();
            }

            std::byte buffer[256];
            int read = mSsl.read(buffer, sizeof(buffer));

            if (read >= 0) {
                esp_ota_write(mOtaHandle, buffer, read);
                doUpdate();
            } else {
                mSsl.closeNotify();

                ESP_LOGI(TAG, "Finalizing flash");
                auto err = esp_ota_end(mOtaHandle);

                if (err == ESP_OK) {
                    ESP_LOGI(TAG, "Image valid");
                    esp_ota_set_boot_partition(mOtaPartition);
                    ESP_LOGI(TAG, "Rebooting...");
                    esp_restart();
                } else {
                    ESP_LOGI(TAG, "Image invalid");
                    reset();
                }
            }
        });
    }

    void UpdaterAsio::reset()
    {
        mSsl.resetSession();
        mSsl.setInputOutput(&mIo);
        asio::error_code err;
        mStreamSocket.close(err);
        acceptConnection();
    }
}
