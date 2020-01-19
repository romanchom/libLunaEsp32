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
    EffectEngine::EffectEngine(EffectSet * effects) :
        Configurable("effects"),
        mActiveEffect("effect", this, &EffectEngine::getActiveEffect, &EffectEngine::setActiveEffect),
        mEnabled("enabled", this, &EffectEngine::getEnabled, &EffectEngine::setEnabled),
        mEffects(effects),
        mController(nullptr),
        mEffectMixer(this),
        mTaskHandle(nullptr)
    {
        mEffectMixer.switchTo(mEffects->find("light"));
    }

    std::vector<AbstractProperty *> EffectEngine::properties()
    {
        return {&mActiveEffect, &mEnabled};
    }

    std::vector<Configurable *> EffectEngine::children()
    {
        return {mEffects};
        // return {&mEffectMixer};
    }

    void EffectEngine::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        {
            std::unique_lock l(mMutex);
            mController = controller;
            mController->enabled(true);
            xTaskCreatePinnedToCore(&EffectEngine::tick, "Daemon", 1024 * 2, this, 5, &mTaskHandle, 0);
        }
    }

    void EffectEngine::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        xTaskNotify(mTaskHandle, 1, eSetValueWithOverwrite);
        mTaskHandle = 0;
        {
            std::unique_lock l(mMutex);
            mController = nullptr;
        }
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
        mEnabled.notify(enabled);
    }

    std::string EffectEngine::getActiveEffect() const
    {
        return mLastEffect;
    }

    void EffectEngine::setActiveEffect(std::string const & value)
    {
        if (mLastEffect == value) {
            return;
        }

        if (auto effect = mEffects->find(value)) {
            mLastEffect = value;
            mEffectMixer.switchTo(effect);
            mActiveEffect.notify(value);
        }
    }

    bool EffectEngine::getEnabled() const
    {
        return mEffectMixer.enabled();
    }

    void EffectEngine::setEnabled(bool const & value)
    {
        mEffectMixer.enabled(value);
    }
}
