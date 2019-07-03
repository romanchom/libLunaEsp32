#pragma once

#include "Service.hpp"
#include "MqttClient.hpp"

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace luna::esp32
{
    struct HardwareController;

    struct MqttService : Service
    {
        explicit MqttService(asio::io_context * ioContext, std::string const & address);

        void start();
    private:
        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        void startTick();
        void update();

        MqttClient mMqtt;
        asio::steady_timer mTick;
        HardwareController * mController;

        SemaphoreHandle_t mMutex;
        float mWhiteness;
        float mSmoothWhiteness;
    };
}
