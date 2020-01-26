#include "RealtimePlugin.hpp"

#include <luna/DiscoveryResponder.hpp>
#include <luna/NetworkService.hpp>
#include <luna/NetworkingContext.hpp>
#include <luna/RealtimeStreamer.hpp>

namespace luna
{
    struct RealtimeNetworkService : NetworkService
    {
        explicit RealtimeNetworkService(std::string name, NetworkingContext const & context, DirectController * controller, Device * device) :
            mStreamer(context, controller),
            mResponder(context.ioContext, mStreamer.port(), name, device->strands())
        {}

        ~RealtimeNetworkService() final = default;
    private:
        RealtimeStreamer mStreamer;
        DiscoveryResponder mResponder;
    };

    RealtimePlugin::RealtimePlugin(std::string && name, Device * device) : 
        mName(std::move(name)),
        mDevice(device)
    {}

    Controller * RealtimePlugin::getController()
    {
        return &mController;
    }
    
    std::unique_ptr<NetworkService> RealtimePlugin::makeNetworkService(NetworkingContext const & network)
    {
        return std::make_unique<RealtimeNetworkService>(mName, network, &mController, mDevice);
    }
}