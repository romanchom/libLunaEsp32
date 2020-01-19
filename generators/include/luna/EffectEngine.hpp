#pragma once

#include "Effect.hpp"
#include "EffectMixer.hpp"
#include "EffectSet.hpp"

#include <luna/Service.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <set>
#include <functional>

namespace luna
{
    struct HardwareController;

    struct EffectEngine : Service, Configurable, private EffectMixer::Observer
    {
        using Callback = std::function<void()>;

        explicit EffectEngine(EffectSet * effects);

        Property<std::string> & activeEffect() { return mActiveEffect; }
        Property<bool> & enabled() { return mEnabled; }

        void post(Callback && callback);

        std::vector<AbstractProperty *> properties() override;
        std::vector<Configurable *> children() override;

    private:
        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        static void tick(void * data);
        void update();

        void enabledChanged(bool enabled) override;

        std::string getActiveEffect() const;
        void setActiveEffect(std::string const & value);

        bool getEnabled() const;
        void setEnabled(bool const & value);

        MemberProperty<EffectEngine, std::string> mActiveEffect;
        MemberProperty<EffectEngine, bool> mEnabled;

        EffectSet * const mEffects;
        std::string mLastEffect;
        HardwareController * mController;
        EffectMixer mEffectMixer;
        TaskHandle_t mTaskHandle;
        bool mShouldRun;
        QueueHandle_t mMessageQueue;
    };
}
