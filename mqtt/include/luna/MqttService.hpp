#pragma once

#include "MqttClient.hpp"
#include "MqttEffect.hpp"

#include <luna/Service.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <map>

namespace luna
{
    struct HardwareController;

    struct MqttService : Service
    {
        explicit MqttService(asio::io_context * ioContext, std::string const & address, std::string name);

        void addEffect(std::string name, MqttEffect * effect);

        void start();
    private:
        void switchTo(std::string_view effectName);

        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        void startTick();
        void update();

        MqttClient mMqtt;
        asio::steady_timer mTick;
        HardwareController * mController;
        SemaphoreHandle_t mMutex;

        std::string mName;
        std::map<std::string, MqttEffect *, std::less<>> mEffects;
        MqttEffect * mActiveEffect;
    };
}
