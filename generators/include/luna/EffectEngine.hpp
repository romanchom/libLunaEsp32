#pragma once

#include "ConstantEffect.hpp"
#include "Effect.hpp"
#include "EffectMixer.hpp"
#include "EffectSet.hpp"

#include <luna/Mutex.hpp>
#include <luna/Service.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <set>
#include <memory>

namespace luna
{
    struct EventLoop;

    struct EffectEngine : Service, Configurable, private EffectMixer::Observer
    {
        explicit EffectEngine(EffectSet * effects, EventLoop * mainLoop);

        Property<std::string> & activeEffect() { return mActiveEffect; }
        Property<bool> & enabled() { return mEnabled; }

        std::vector<AbstractProperty *> properties() override;
        std::vector<Configurable *> children() override;

    private:
        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        static void tick(void * data);
        void loop();

        void enabledChanged(bool enabled) override;

        std::string getActiveEffect() const;
        void setActiveEffect(std::string const & value);

        bool getEnabled() const;
        void setEnabled(bool const & value);

        EffectSet * const mEffects;
        EventLoop * const mMainLoop;

        MemberProperty<EffectEngine, std::string> mActiveEffect;
        MemberProperty<EffectEngine, bool> mEnabled;

        std::string mLastEffect;
        HardwareController * mController;
        EffectMixer mEffectMixer;
        TaskHandle_t mTaskHandle;
        std::unique_ptr<Generator> mGenerator;
    };
}
