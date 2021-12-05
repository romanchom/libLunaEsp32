#pragma once

#include <luna/Plugin.hpp>
#include <luna/Configurable.hpp>

#include <string>

namespace luna
{
    struct MqttPlugin : Plugin
    {
        explicit MqttPlugin(std::string && name, std::string && address, Configurable * configurable, float floatScale);
        ~MqttPlugin();

        std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) final;
    private:
        struct Instance;

        std::string mName;
        std::string mAddress;
        Configurable * mConfigurable;
        float const mFloatScale;
    };
}