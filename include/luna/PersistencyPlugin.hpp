#pragma once

#include <luna/Configurable.hpp>
#include <luna/Property.hpp>
#include <luna/Plugin.hpp>

#include <nvs_flash.h>

#include <vector>
#include <memory>

namespace luna
{
    struct NvsNamespace
    {
        explicit NvsNamespace(std::string const & name);
        explicit NvsNamespace(NvsNamespace && other);
        ~NvsNamespace();
        nvs_handle_t handle() const { return mHandle; }
    private:
        nvs_handle_t mHandle;
    };

    struct PersistencyPlugin : Plugin
    {
        explicit PersistencyPlugin(Configurable * configurable);
        ~PersistencyPlugin();

        std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) final;
    private:
        void recurse(Configurable * configurable, std::string const & name);

        friend struct NvsVisitor;

        Configurable * mConfigurable;
        std::vector<NvsNamespace> mNamespaces;
        std::vector<std::unique_ptr<AbstractProperty>> mProperties;
    };
}