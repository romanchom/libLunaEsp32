#include "UpdaterSocket.hpp"

#include <esp_log.h>
#include <esp_system.h>
#include <lwip/sockets.h>

#include <vector>

static char const TAG[] = "OTA";

namespace luna
{
    UpdaterSocket::UpdaterSocket()
    {
        mOtaPartition = esp_ota_get_next_update_partition(nullptr);
        if (mOtaPartition == nullptr) {
            ESP_LOGE(TAG, "No partition");
            std::terminate();
        }

        auto result = xTaskCreate(&task, "OTA", 2048, this, 0, &mTask);

        if (result != pdPASS) {
            ESP_LOGE(TAG, "Could not create task");
            std::terminate();
        }
    }

    UpdaterSocket::~UpdaterSocket() = default;

    void UpdaterSocket::task(void * data)
    {
        auto self = (UpdaterSocket *) data;
        self->waitForOta();
    }

    void UpdaterSocket::waitForOta()
    {
        ESP_LOGI(TAG, "Started");

        int err;
        int listen_sock;
        {
            sockaddr_in destAddr;
            destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            destAddr.sin_family = AF_INET;
            destAddr.sin_port = htons(60000);
            listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
            if (listen_sock < 0) {
                ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
                std::terminate();
            }
            err = bind(listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
            if (err != 0) {
                ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
                std::terminate();
            }
            ESP_LOGI(TAG, "Socket created");
        }

        err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
            std::terminate();
        }
        ESP_LOGI(TAG, "Socket listening");

        while (true) {
            int sock = accept(listen_sock,  nullptr, 0);
            if (sock < 0) {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                std::terminate();
            }
            ESP_LOGI(TAG, "Socket accepted");

            esp_ota_handle_t otaHandle;
            ESP_LOGI(TAG, "OTA begin");
            esp_ota_begin(mOtaPartition, OTA_SIZE_UNKNOWN, &otaHandle);

            while(true) {
                std::vector<std::byte> buffer(256);
                int read = recv(sock, buffer.data(), buffer.size(), 0);


                if (read <= 0) {
                    shutdown(sock, 0);
                    close(sock);

                    ESP_LOGI(TAG, "Finalizing flash");
                    auto err = esp_ota_end(otaHandle);

                    if (err == ESP_OK) {
                        ESP_LOGI(TAG, "Image valid");
                        esp_ota_set_boot_partition(mOtaPartition);
                        ESP_LOGI(TAG, "Rebooting...");
                        esp_restart();
                    } else {
                        ESP_LOGI(TAG, "Image invalid");
                        break;
                    }
                }

                esp_ota_write(otaHandle, buffer.data(), read);
            }
        }
    }
}
