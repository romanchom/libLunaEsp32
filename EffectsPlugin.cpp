#include "EffectsPlugin.hpp"

#include <luna/NetworkService.hpp>

namespace luna
{
    EffectPlugin::EffectPlugin(EventLoop * mainLoop, std::initializer_list<Effect *> effects) :
        mEffects(effects),
        mEffectEngine(&mEffects, mainLoop)
    {}

    EffectPlugin::~EffectPlugin() = default;

    Service * EffectPlugin::initializeService()
    {
        return &mEffectEngine;
    }

    std::unique_ptr<NetworkService> EffectPlugin::initializeNetworking(NetworkingContext const & network)
    {
        return nullptr;
    }
}
