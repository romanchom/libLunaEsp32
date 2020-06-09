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

        std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) final;
    private:
        struct Instance;

        std::string mName;
        std::string mAddress;
        Configurable * mEffectEngine;
        float const mFloatScale;
    };
}