#pragma once

#include <luna/EffectEngine.hpp>
#include <luna/EffectSet.hpp>
#include <luna/Plugin.hpp>

namespace luna
{
    struct EventLoop;

    struct EffectPlugin : Plugin
    {
        explicit EffectPlugin(EventLoop * mainLoop, std::initializer_list<Effect *> effects);
        ~EffectPlugin();
        Service * initializeService() final;
        std::unique_ptr<NetworkService> initializeNetworking(NetworkingContext const & network) final;

        EffectEngine & effectEngine() { return mEffectEngine; }
    private:
        EffectSet mEffects;
        EffectEngine mEffectEngine;
    };
}