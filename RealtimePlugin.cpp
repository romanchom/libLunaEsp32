#include "RealtimePlugin.hpp"

#include <luna/DiscoveryResponder.hpp>
#include <luna/NetworkService.hpp>
#include <luna/NetworkingContext.hpp>
#include <luna/RealtimeStreamer.hpp>

namespace luna
{
    struct RealtimeNetworkService : NetworkService
    {
        explicit RealtimeNetworkService(LunaContext const & context, NetworkingContext const & network, std::string name, DirectController * controller, Device * device) :
            mStreamer(context, network, controller),
            mResponder(network.ioContext, mStreamer.port(), name, device->strands())
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

    Controller * RealtimePlugin::getController(LunaContext const & context)
    {
        return &mController;
    }
    
    std::unique_ptr<NetworkService> RealtimePlugin::makeNetworkService(LunaContext const & context, NetworkingContext const & network)
    {
        return std::make_unique<RealtimeNetworkService>(context, network, mName, &mController, mDevice);
    }
}