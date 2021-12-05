#pragma once

#include <luna/Plugin.hpp>

namespace luna
{
    struct UpdatePlugin : Plugin
    {
        std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) final;
    };
}
