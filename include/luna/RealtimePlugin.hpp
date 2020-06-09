#pragma once

#include <luna/Device.hpp>
#include <luna/Plugin.hpp>

namespace luna
{
    struct RealtimePlugin : Plugin
    {
        explicit RealtimePlugin(std::string && name);

        std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) final;
    private:
        std::string mName;
    };
}
