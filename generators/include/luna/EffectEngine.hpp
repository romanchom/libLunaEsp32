#pragma once

#include "ConstantEffect.hpp"
#include "Effect.hpp"
#include "EffectMixer.hpp"
#include "EffectSet.hpp"

#include <luna/Controller.hpp>
#include <luna/Plugin.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <set>
#include <memory>

namespace luna
{
    struct EventLoop;
    struct ControllerMux;

    struct EffectEngine : Plugin, Configurable, private Controller, private EffectMixer::Observer
    {
        explicit EffectEngine(std::initializer_list<Effect *> effects);

        Property<std::string> & activeEffect() { return mActiveEffect; }
        Property<bool> & enabled() { return mEnabled; }

        Controller * getController(LunaContext const & context) final;
        std::unique_ptr<NetworkService> makeNetworkService(LunaContext const & context,NetworkingContext const & network) final;
        
        std::vector<AbstractProperty *> properties() override;
        std::vector<Configurable *> children() override;

    private:
        void takeOwnership(Device * device) final;
        void releaseOwnership() final;

        void enabledChanged(bool value) override;

        static void tick(void * data);
        void loop();

        std::string getActiveEffect() const;
        void setActiveEffect(std::string const & value);

        bool getEnabled() const;
        void setEnabled(bool const & value);

        EffectSet mEffects;

        MemberProperty<EffectEngine, std::string> mActiveEffect;
        MemberProperty<EffectEngine, bool> mEnabled;

        std::string mLastEffect;
        Device * mDevice;
        EffectMixer mEffectMixer;
        TaskHandle_t mTaskHandle;
        std::unique_ptr<Generator> mGenerator;
        EventLoop * mMainLoop;
        ControllerMux * mMultiplexer;
    };
}
