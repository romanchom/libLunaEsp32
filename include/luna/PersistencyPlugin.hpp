#pragma once

#include <luna/Plugin.hpp>
#include <luna/Configurable.hpp>

namespace luna
{
    struct PersistencyPlugin : Plugin
    {
        explicit PersistencyPlugin(Configurable * configurable);
        ~PersistencyPlugin();

        std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) final;
    private:
        Configurable * mConfigurable;
    };
}