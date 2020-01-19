#include "EffectEngine.hpp"

#include "ConstantGenerator.hpp"

#include <luna/Parse.hpp>
#include <luna/HardwareController.hpp>
#include <luna/Strand.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>


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
        mTaskHandle(nullptr),
        mShouldRun(false)
    {
        mEffectMixer.switchTo(mEffects->find("light"));
        mMessageQueue = xQueueCreate(4, sizeof(Callback));
    }

    void EffectEngine::post(Callback && callback)
    {
        char buffer[sizeof(Callback)];
        auto view = new (buffer) Callback(std::move(callback));
        auto ret = xQueueSendToBack(mMessageQueue, static_cast<void *>(view), portMAX_DELAY );
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
            mController = controller;
            mController->enabled(true);
            mShouldRun = true;
            xTaskCreatePinnedToCore(&EffectEngine::tick, "Daemon", 1024 * 2, this, 5, &mTaskHandle, 0);
        }
    }

    void EffectEngine::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");

        post([this](){
            mShouldRun = false;
            mController = nullptr;
        });
    }

    void EffectEngine::tick(void * data)
    {
        auto self = static_cast<EffectEngine *>(data);

        constexpr int frequency = 50;
        constexpr int periodMs = 1000 / frequency;
        constexpr int periodTicks = periodMs / portTICK_PERIOD_MS;

        auto wakeTime = xTaskGetTickCount();
        ESP_LOGI(TAG, "Running");

        while (self->mShouldRun) {
            wakeTime += periodTicks;
            self->update();

            for (;;) {
                auto ticksToWait = int(wakeTime - xTaskGetTickCount());
                if (ticksToWait <= 0) { break; }

                char buffer[sizeof(Callback)];
                auto error = xQueueReceive(self->mMessageQueue, buffer, ticksToWait);
                if (error == pdTRUE) {
                    auto callback = reinterpret_cast<Callback *>(buffer);
                    (*callback)();
                    callback->~Callback();
                }
            }
        }
        ESP_LOGI(TAG, "Exiting");
        vTaskDelete(0);
    }

    void EffectEngine::update()
    {
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
