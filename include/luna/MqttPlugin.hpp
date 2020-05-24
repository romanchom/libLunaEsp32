#pragma once

#include <luna/Plugin.hpp>

#include <string>

namespace luna
{
    struct EventLoop;
    struct Configurable;

    struct MqttPlugin : Plugin
    {
        explicit MqttPlugin(std::string && name, std::string && address, Configurable * effectEngine, float floatScale);
        ~MqttPlugin();

        luna::Controller * getController(LunaContext const & context) final;
        std::unique_ptr<NetworkService> makeNetworkService(LunaContext const & context,NetworkingContext const & network) final;
    private:
        std::string mName;
        std::string mAddress;
        Configurable * mEffectEngine;
        float const mFloatScale;
    };
}