#pragma once

#include <luna/NetworkService.hpp>

#include <esp_ota_ops.h>

#include <memory>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace luna
{
    struct UpdaterSocket : NetworkService
    {
        explicit UpdaterSocket();
        ~UpdaterSocket() final;
    private:
        static void task(void * data);
        void waitForOta();

        esp_partition_t const * mOtaPartition;
        TaskHandle_t mTask;
    };

}
