#include "RealtimePlugin.hpp"

#include "DiscoveryResponder.hpp"

#include "RealtimeService.hpp"

#include <luna/NetworkService.hpp>
#include <luna/NetworkingContext.hpp>

namespace luna
{
    struct RealtimeNetworkService : NetworkService
    {
        explicit RealtimeNetworkService(std::string name, NetworkingContext const & context, DirectService * service, Device * device) :
            mService(context, service),
            mResponder(context.ioContext, mService.port(), name, device->strands())
        {}

        ~RealtimeNetworkService() final = default;
    private:
        RealtimeService mService;
        DiscoveryResponder mResponder;
    };

    RealtimePlugin::RealtimePlugin(std::string && name, Device * device) : 
        mName(std::move(name)),
        mDevice(device)
    {}

    Service * RealtimePlugin::initializeService()
    {
        return &mService;
    }
    
    std::unique_ptr<NetworkService> RealtimePlugin::initializeNetworking(NetworkingContext const & network)
    {
        return std::make_unique<RealtimeNetworkService>(mName, network, &mService, mDevice);
    }
}