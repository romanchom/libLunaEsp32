#include "UpdatePlugin.hpp"
#include "Updater.hpp"

#include <luna/LunaInterface.hpp>
#include <luna/TlsConfiguration.hpp>

namespace luna
{
    struct UpdateInstance : PluginInstance
    {
        void onNetworkAvaliable(LunaNetworkInterface * luna) final
        {
            luna->addNetworkService(std::make_unique<Updater>(luna->ioContext(), luna->tlsConfiguration()->makeTlsConfiguration()));
        }
    };

    std::unique_ptr<PluginInstance> UpdatePlugin::instantiate(LunaInterface * luna)
    {
        return std::make_unique<UpdateInstance>();
    }
}