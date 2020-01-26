#include "UpdatePlugin.hpp"
#include "Updater.hpp"

#include <luna/NetworkingContext.hpp>
#include <luna/TlsConfiguration.hpp>

namespace luna
{
    Service * UpdatePlugin::initializeService() { return nullptr; }

    std::unique_ptr<NetworkService> UpdatePlugin::initializeNetworking(NetworkingContext const & network)
    {
        return std::make_unique<Updater>(network.ioContext, network.tlsConfiguration->makeTlsConfiguration());
    }
}