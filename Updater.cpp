#include "luna/esp32/Updater.hpp"

#include "mbedtls-cpp/BasicInputOutput.hpp"
#include "mbedtls-cpp/Configuration.hpp"
#include "mbedtls-cpp/CounterDeterministicRandomGenerator.hpp"
#include "mbedtls-cpp/Ssl.hpp"
#include "mbedtls-cpp/StandardEntropy.hpp"
#include "mbedtls-cpp/Timer.hpp"
#include "mbedtls-cpp/PrivateKey.hpp"
#include "mbedtls-cpp/Certificate.hpp"
#include "mbedtls-cpp/SocketInputOutput.hpp"

#include <esp_ota_ops.h>
#include <esp_log.h>

static char const TAG[] = "OTA";

namespace luna::esp32
{

Updater::Updater(tls::PrivateKey * ownKey, tls::Certificate * ownCertificate, tls::Certificate * caCertificate) : 
    mOwnKey(ownKey),
    mOwnCertificate(ownCertificate),
    mCaCertificate(caCertificate),
    mTaskHandle(nullptr)
{
    mWaitHandle = xSemaphoreCreateBinary();
}

void Updater::start()
{
    if (mTaskHandle == nullptr) {
        // mTaskHandle = xTaskCreate();
        xTaskCreate(&Updater::task, "OTA", 1024 * 12, this, 5, &mTaskHandle);
    } else {
        std::terminate();
    }
}

void Updater::stop()
{
    // xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction);
    xSemaphoreTake(mWaitHandle, portMAX_DELAY);
    mTaskHandle = nullptr;
}

void Updater::task(void * context)
{
    auto updater = static_cast<Updater *>(context);
    updater->doOta();
}

void Updater::doOta()
{
    using namespace tls;

    esp_partition_t const * update_partition = esp_ota_get_next_update_partition(nullptr);

    StandardEntropy entropy;
    CounterDeterministicRandomGenerator random;
    Configuration tlsConfiguration;
    Ssl ssl;

    random.seed(&entropy, "asdqwe", 6);

    tlsConfiguration.setRandomGenerator(&random);
    tlsConfiguration.setDefaults(Endpoint::server, Transport::stream, Preset::default_);
    tlsConfiguration.setOwnCertificate(mOwnCertificate, mOwnKey);
    tlsConfiguration.setCertifiateAuthorityChain(mCaCertificate);
    tlsConfiguration.setAuthenticationMode(AuthenticationMode::required);

    SocketInputOutput server;
    server.bind(nullptr, "60000", Protocol::tcp);
    // server.blocking(false);


    for (;;) {
        ESP_LOGI(TAG, "Listening");
        try {
            SocketInputOutput connection = server.accept();
            ESP_LOGI(TAG, "Conected");

            ssl.setup(&tlsConfiguration);
            ssl.setInputOutput(&connection);

            ssl.handshake();

            ESP_LOGI(TAG, "Authenticated");

            esp_err_t err;
            esp_ota_handle_t update_handle = 0;
            err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);

            uint8_t buffer[256];
            int read;
            
            ESP_LOGI(TAG, "Beginning flash");
            while ((read = ssl.read(buffer, sizeof(buffer))) > 0) {
                // ESP_LOGE(TAG, "TLS: %d bytes", read);
                err = esp_ota_write(update_handle, buffer, read);
            }

            ESP_LOGI(TAG, "Finalizing flash");
            
            err = esp_ota_end(update_handle);
            if (err != ESP_OK) {
                std::terminate();
            }

            ESP_LOGI(TAG, "Setting boot partition");
            
            err = esp_ota_set_boot_partition(update_partition);
            ESP_LOGI(TAG, "Rebooting...");
            esp_restart();
        } catch (tls::Exception const & e) {
            ESP_LOGE(TAG, "TLS: %s", e.what());
            ssl.resetSession();
        }
    }

    xSemaphoreGive(mWaitHandle);
    vTaskDelete(0);
} 
 
}
