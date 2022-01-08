#include "UpdatePlugin.hpp"

#if ESP32
#   include "UpdaterAsio.hpp"
#else
#   include "UpdaterSocket.hpp"
#endif

#include <luna/LunaInterface.hpp>
#include <luna/TlsConfiguration.hpp>

namespace luna
{
    namespace
    {
        struct Instance : PluginInstance
        {
            void onNetworkAvaliable(LunaNetworkInterface * luna) final
            {
#if ESP32
                auto updater = std::make_unique<UpdaterAsio>(luna->ioContext(), luna->tlsConfiguration()->makeTlsConfiguration());
#else
                auto updater = std::make_unique<UpdaterSocket>();
#endif
                luna->addNetworkService(std::move(updater));
            }
        };
    }

    std::unique_ptr<PluginInstance> UpdatePlugin::instantiate(LunaInterface * luna)
    {
        return std::make_unique<Instance>();
    }
}
