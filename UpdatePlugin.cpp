#include "UpdatePlugin.hpp"
#include "Updater.hpp"

#include <luna/NetworkingContext.hpp>
#include <luna/TlsConfiguration.hpp>

namespace luna
{
    Controller * UpdatePlugin::getController() { return nullptr; }

    std::unique_ptr<NetworkService> UpdatePlugin::makeNetworkService(NetworkingContext const & network)
    {
        return std::make_unique<Updater>(network.ioContext, network.tlsConfiguration->makeTlsConfiguration());
    }
}