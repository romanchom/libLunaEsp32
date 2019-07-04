#pragma once

#include "Service.hpp"
#include "MqttClient.hpp"
#include "MqttEffect.hpp"
#include "ConstantMqttEffect.hpp"

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <map>

namespace luna::esp32
{
    struct HardwareController;

    struct MqttService : Service
    {
        explicit MqttService(asio::io_context * ioContext, std::string const & address, std::string name);

        void addEffect(std::string name, MqttEffect * effect);

        void start();
    private:
        void switchTo(std::string const & effectName);

        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        void startTick();
        void update();

        MqttClient mMqtt;
        asio::steady_timer mTick;
        HardwareController * mController;
        SemaphoreHandle_t mMutex;

        std::string mName;
        // std::string mLastActiveName;
        // ConstantMqttEffect mOffEffect;
        std::map<std::string, MqttEffect *> mEffects;
        MqttEffect * mActiveEffect;
        // MqttEffect * mActiveEffects[3];
        // float mEffectTransitionRatio;
    };
}
