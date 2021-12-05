#include "EffectPlugin.hpp"

#include <luna/Device.hpp>
#include <luna/Strand.hpp>
#include <luna/Controller.hpp>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <chrono>

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
    struct EffectPlugin::Instance : PluginInstance, private Controller, private EnabledObserver
    {
        explicit Instance(EffectPlugin * parent, LunaInterface * luna) :
            mParent(parent),
            mLuna(luna),
            mControllerHandle(mLuna->addController(this)),
            mTaskHandle(nullptr),
            mDevice(nullptr),
            mMainTaskHandle(nullptr)
        {
            mParent->mEffectMixer.setObserver(this);
        }

        ~Instance()
        {
            mParent->mEffectMixer.setObserver(nullptr);
        }

        void onNetworkAvaliable(LunaNetworkInterface * luna) override
        {}

    private:
        void takeOwnership(Device * device) final
        {
            ESP_LOGI(TAG, "Enabled");
            mDevice = device;
            mDevice->enabled(true);
            xTaskCreatePinnedToCore(&Instance::task, "effects", 1024 * 4, this, 5, &mTaskHandle, 0);
        }

        void releaseOwnership() final
        {
            ESP_LOGI(TAG, "Disabled");
            mMainTaskHandle = xTaskGetCurrentTaskHandle();
            xTaskNotify(mTaskHandle, Notification::Exit, eSetBits);
            ulTaskNotifyTake(false, portMAX_DELAY);
            mDevice = nullptr;
        }

        static void task(void * data)
        {
            ESP_LOGI(TAG, "Running");
            static_cast<Instance *>(data)->loop();
            ESP_LOGI(TAG, "Exiting");
            vTaskDelete(0);
        }

        void loop()
        {
            auto lastWakeTime = xTaskGetTickCount();
            auto const startTime = lastWakeTime;

            for (;;) {
                Time t{
                    .total = (lastWakeTime - startTime) * portTICK_PERIOD_MS / 1000.0f,
                    .delta = 0.02f,
                };

                mLuna->post([this, t]{
                    mGenerator = mParent->mEffectMixer.generator(t);
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
            xTaskNotifyGive(mMainTaskHandle);
        }

        void enabledChanged(bool enabled) override
        {
            mLuna->post([this, enabled]{
                mControllerHandle->enabled(enabled);
            });
        }

        EffectPlugin * mParent;
        LunaInterface * mLuna;
        std::unique_ptr<ControllerHandle> mControllerHandle;
        TaskHandle_t mTaskHandle;
        Device * mDevice;
        std::unique_ptr<Generator> mGenerator;
        TaskHandle_t mMainTaskHandle;
    };

    EffectPlugin::EffectPlugin(std::vector<Effect *> && effects) :
        Configurable("effects"),
        mEffects(std::move(effects)),
        mActiveEffect("effect", this, &EffectPlugin::getActiveEffect, &EffectPlugin::setActiveEffect),
        mEnabled("enabled", this, &EffectPlugin::getEnabled, &EffectPlugin::setEnabled),
        mEffectMixer()
    {
        mEffectMixer.switchTo(mEffects.find("light"));
    }

    std::unique_ptr<PluginInstance> EffectPlugin::instantiate(LunaInterface * luna)
    {
        return std::make_unique<Instance>(this, luna);
    }

    std::vector<AbstractProperty *> EffectPlugin::properties()
    {
        return {&mActiveEffect, &mEnabled};
    }

    std::vector<Configurable *> EffectPlugin::children()
    {
        return {&mEffects, &mEffectMixer};
    }

    std::string EffectPlugin::getActiveEffect() const
    {
        return mLastEffect;
    }

    void EffectPlugin::setActiveEffect(std::string const & value)
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

    bool EffectPlugin::getEnabled() const
    {
        return mEffectMixer.enabled();
    }

    void EffectPlugin::setEnabled(bool const & value)
    {
        ESP_LOGI(TAG, "%d", value);
        mEffectMixer.enabled(value);
        mEnabled.notify(value);
    }
}
