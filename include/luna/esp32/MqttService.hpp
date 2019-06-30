#pragma once

#include "Service.hpp"
#include "MqttClient.hpp"

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

namespace luna::esp32
{
    struct HardwareController;

    struct MqttService : Service
    {
        explicit MqttService(asio::io_context * ioContext);

        void start();
    private:
        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        void startTick();
        void update();

        MqttClient mMqtt;
        asio::steady_timer mTick;
        HardwareController * mController;

        bool mEnabled;
        float mWhiteness;
        float mSmoothWhiteness;
    };
}
