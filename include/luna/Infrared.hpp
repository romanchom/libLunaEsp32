#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/rmt.h>
#include <esp_log.h>

#include <bitset>
#include <functional>

namespace luna
{
    struct Infrared
    {
        using Callback = std::function<void(uint8_t address, uint8_t command)>;
        explicit Infrared(int pin);
        void setOnMessageReceived(Callback callback);
    private:
        void receive();

        TaskHandle_t mTaskHandle;
        RingbufHandle_t mBuffer;
        Callback mOnMessageReceived;
    };
};
