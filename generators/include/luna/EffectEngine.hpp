#pragma once

#include "ConstantEffect.hpp"
#include "Effect.hpp"
#include "EffectMixer.hpp"

#include <luna/Service.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <map>

namespace luna
{
    struct HardwareController;

    struct EffectEngine : Service, Configurable, private EffectMixer::Observer
    {
        explicit EffectEngine(asio::io_context * ioContext);

        void addEffect(std::string_view name, Effect * effect);
        
        std::map<std::string, Effect *, std::less<>> const & effects() const noexcept { return mEffects; }
    private:
        void switchTo(std::string_view effectName);

        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        void startTick();
        void update();

        void enabledChanged(bool enabled) override;

        asio::steady_timer mTick;
        HardwareController * mController;

        std::map<std::string, Effect *, std::less<>> mEffects;
        EffectMixer mEffectMixer;
    };
}
