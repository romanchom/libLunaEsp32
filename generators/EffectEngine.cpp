#include "EffectEngine.hpp"

#include "ConstantGenerator.hpp"

#include <luna/Parse.hpp>
#include <luna/HardwareController.hpp>
#include <luna/Strand.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>

#include <chrono>
#include <mutex>

static char const TAG[] = "Effects";

namespace luna
{
    EffectEngine::EffectEngine(std::initializer_list<Effect *> effects) :
        Configurable("effects"),
        mController(nullptr),
        mEffects(effects),
        mEffectMixer(this)
    {
        addProperty("effect", [this](std::string_view text) {
            switchTo(text);
        });
        addProperty("enabled", [this](std::string_view text) {
            if (auto value = tryParse<int>(text)) {
                bool enabled = (*value > 0);
                mEffectMixer.enabled(enabled);
                ESP_LOGI(TAG, "%s", enabled ? "On" : "Off");
            }
        });
    }

    void EffectEngine::switchTo(std::string_view effectName)
    {
        if (auto it = mEffects.find(effectName); it != mEffects.end()) {
            mEffectMixer.switchTo(*it);
        }
    }

    void EffectEngine::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        std::unique_lock l(mMutex);
        mController = controller;
        mController->enabled(true);
        xTaskCreatePinnedToCore(&EffectEngine::tick, "Daemon", 1024 * 2, this, 5, &mTaskHandle, 0);
    }

    void EffectEngine::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        // std::unique_lock l(mMutex);
        xTaskNotify(mTaskHandle, 1, eSetValueWithOverwrite);
        mTaskHandle = 0;
        mController = nullptr;
    }

    void EffectEngine::tick(void * data)
    {
        auto self = static_cast<EffectEngine *>(data);

        auto lastWakeTime = xTaskGetTickCount();
        ESP_LOGI(TAG, "Running");

        for (;;) {
            self->update();

            vTaskDelayUntil(&lastWakeTime, 20 / portTICK_PERIOD_MS);
            if (xTaskNotifyWait(0, 0, nullptr, 0)) {
                break;
            }
        }
        ESP_LOGI(TAG, "Exiting");
        vTaskDelete(0);
    }

    void EffectEngine::update()
    {
        std::unique_lock l(mMutex);
        
        mEffectMixer.update(0.02f);

        if (!mController) {
            return;
        }

        for (auto strand : mController->strands()) {
            auto gen = mEffectMixer.generator(strand->location());
            strand->fill(gen);
        }

        mController->update();
    }

    void EffectEngine::enabledChanged(bool enabled)
    {
        serviceEnabled(enabled);
    }
}
