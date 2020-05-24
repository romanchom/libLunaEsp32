#include "EffectEngine.hpp"

#include "ConstantGenerator.hpp"

#include <luna/EventLoop.hpp>
#include <luna/Device.hpp>
#include <luna/Strand.hpp>
#include <luna/ControllerMux.hpp>
#include <luna/NetworkService.hpp>

#include <esp_log.h>

#include <chrono>
#include <mutex>

static char const TAG[] = "Effects";

namespace
{
    enum Notification : uint32_t
    {
        Exit = 0x1,
        Update = 0x2,
    };
}

namespace luna
{
    EffectEngine::EffectEngine(std::initializer_list<Effect *> effects) :
        Configurable("effects"),
        mEffects(effects),
        mActiveEffect("effect", this, &EffectEngine::getActiveEffect, &EffectEngine::setActiveEffect),
        mEnabled("enabled", this, &EffectEngine::getEnabled, &EffectEngine::setEnabled),
        mDevice(nullptr),
        mEffectMixer(this),
        mTaskHandle(nullptr),
        mMainLoop(nullptr)
    {
        mEffectMixer.switchTo(mEffects.find("light"));
    }

    Controller * EffectEngine::getController(LunaContext const & context)
    {
        mMainLoop = context.mainLoop;
        mMultiplexer = context.multiplexer;
        return this;
    }

    std::unique_ptr<NetworkService> EffectEngine::makeNetworkService(LunaContext const & context,NetworkingContext const & network)
    {
        return nullptr;
    }

    std::vector<AbstractProperty *> EffectEngine::properties()
    {
        return {&mActiveEffect, &mEnabled};
    }

    std::vector<Configurable *> EffectEngine::children()
    {
        return {&mEffects, &mEffectMixer};
    }

    void EffectEngine::takeOwnership(Device * device)
    {
        ESP_LOGI(TAG, "Enabled");
        mDevice = device;
        mDevice->enabled(true);
        xTaskCreatePinnedToCore(&EffectEngine::tick, "effects", 1024 * 2, this, 5, &mTaskHandle, 0);
    }

    void EffectEngine::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        xTaskNotify(mTaskHandle, Notification::Exit, eSetBits);
        ulTaskNotifyTake(false, portMAX_DELAY);
        mDevice = nullptr;
    }

    void EffectEngine::tick(void * data)
    {
        ESP_LOGI(TAG, "Running");
        static_cast<EffectEngine *>(data)->loop();
        ESP_LOGI(TAG, "Exiting");
        vTaskDelete(0);
    }

    void EffectEngine::loop()
    {
        auto lastWakeTime = xTaskGetTickCount();
        auto const startTime = lastWakeTime;

        for (;;) {
            Time t{
                .total = (lastWakeTime - startTime) * portTICK_PERIOD_MS / 1000.0f,
                .delta = 0.02f,
            };

            mMainLoop->post([this, t]{
                mGenerator = mEffectMixer.generator(t);
                if (mTaskHandle != 0) {
                    xTaskNotify(mTaskHandle, Notification::Update, eSetBits);
                }   
            });

            uint32_t notification = 0;
            xTaskNotifyWait(0, ~0, &notification, portMAX_DELAY);

            if (notification & Notification::Exit) {
                break;
            }

            if (notification & Notification::Update) {
                for (auto strand : mDevice->strands()) {
                    strand->acceptGenerator(mGenerator.get());
                }

                mDevice->update();
            }

            vTaskDelayUntil(&lastWakeTime, 20 / portTICK_PERIOD_MS);
        }

        mTaskHandle = 0;
        xTaskNotifyGive(mMainLoop->taskHandle());
    }

    void EffectEngine::enabledChanged(bool value)
    {
        mMainLoop->post([this, value]{
            mMultiplexer->setEnabled(this, value);
        });
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

        if (auto effect = mEffects.find(value)) {
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
        ESP_LOGI(TAG, "%d", value);
        mEffectMixer.enabled(value);
        mEnabled.notify(value);
    }
}
