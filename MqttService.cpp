#include "luna/esp32/MqttService.hpp"

#include "luna/esp32/HardwareController.hpp"
#include "luna/esp32/Strand.hpp"
#include "luna/esp32/ConstantGenerator.hpp"
#include "luna/esp32/InterpolatingGenerator.hpp"
#include "luna/proto/Scalar.hpp"

#include <esp_log.h>

#include <algorithm>
#include <chrono>

static char const TAG[] = "MqttSvc";

namespace {
    struct Lock
    {
        explicit Lock(SemaphoreHandle_t mutex) :
            mMutex(mutex)
        {
            xSemaphoreTake(mMutex, portMAX_DELAY);
        }

        ~Lock()
        {
            xSemaphoreGive(mMutex);
        }
    private:
        SemaphoreHandle_t const mMutex;
    };
}

namespace luna::esp32
{
    MqttService::MqttService(asio::io_context * ioContext, std::string const & address, std::string name) :
        mMqtt(address),
        mTick(*ioContext),
        mController(nullptr),
        mMutex(xSemaphoreCreateMutex()),
        mName(name),
        mActiveEffect(nullptr)
    {
    }

    void MqttService::addEffect(std::string name, MqttEffect * effect)
    {
        mEffects.try_emplace(std::move(name), effect);
        if (mEffects.size() == 1) {
            mActiveEffect = effect;
        }
    }

    void MqttService::switchTo(std::string const & effectName)
    {
        if (auto it = mEffects.find(effectName); it != mEffects.end()) {
            mActiveEffect = it->second;
        }
    }

    void MqttService::start()
    {
        mMqtt.subscribe(mName + "/enabled", [this](MqttTopic const & topic, void * data, size_t size) {
            int on = atoi((char *) data);
            bool enabled = (on > 0);
            serviceEnabled(enabled);
            ESP_LOGI(TAG, "%s", enabled ? "On" : "Off");
        });

        mMqtt.subscribe(mName + "/effect", [this](MqttTopic const & topic, void * data, size_t size) {
            std::string name((char *) data, size);
            switchTo(name);
        });

        for (auto & [effectName, effect] : mEffects) {
            mMqtt.subscribe(mName + "/effects/" + effectName + "/#", [effect](MqttTopic const & topic, void * data, size_t size) {
                effect->configure(topic, data, size);
            });
        }

        mMqtt.connect();
    }

    void MqttService::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        mController = controller;
        mController->enabled(true);
        startTick();
    }

    void MqttService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        Lock l(mMutex);
        mTick.cancel();
        mController = nullptr;
    }

    void MqttService::startTick()
    {
        mTick.expires_after(std::chrono::milliseconds(20));
        update();

        mTick.async_wait([this](asio::error_code const & error) {
            if (!error) {
                startTick();
            }
        });
    }

    void MqttService::update()
    {
        Lock l(mMutex);

        if (!mController) {
            return;
        }

        // bool const isTransitionInProgress = (mActiveEffects[0] != mActiveEffects[1]);
        // bool const isTransitionInPending = (mActiveEffects[1] != mActiveEffects[2]);

        // if (isTransitionInProgress) {
        //     mEffectTransitionRatio += 0.02f;
        // }

        // if (mEffectTransitionRatio >= 1.0f || (isTransitionInPending && !isTransitionInProgress)) {
        //     mEffectTransitionRatio = 0.0f;
        //     mActiveEffects[0] = mActiveEffects[1];
        //     mActiveEffects[1] = mActiveEffects[2];

        //     serviceEnabled(mActiveEffects[0] != &mOffEffect);
        // }

        // if (mEffectTransitionRatio > 0.0f) {
        //     InterpolatingGenerator gen(mActiveEffects[0]->generator(), mActiveEffects[1]->generator(), mEffectTransitionRatio);

        //     for (auto strand : mController->strands()) {
        //         strand->fill(&gen);
        //     }
        // } else {
        mActiveEffect->update(0.02f);
        auto gen = mActiveEffect->generator();

        for (auto strand : mController->strands()) {
            strand->fill(gen);
        }
        // }

        mController->update();
    }
}
