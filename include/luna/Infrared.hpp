#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/rmt.h>
#include <esp_log.h>

namespace luna
{
    struct EventLoop;

    struct InfraredDemux
    {
        virtual void demultiplex(uint8_t address, uint8_t command) = 0;
    protected:
        ~InfraredDemux() = default;
    };

    struct Infrared
    {
        explicit Infrared(EventLoop * mainLoop, int pin, InfraredDemux * demultiplexer);
    private:
        void receive();

        EventLoop * mMainLoop;
        TaskHandle_t mTaskHandle;
        RingbufHandle_t mBuffer;
        InfraredDemux * mDemultiplexer;
    };
};
