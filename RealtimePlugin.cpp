#include "RealtimePlugin.hpp"

#include <luna/DirectController.hpp>
#include <luna/DiscoveryResponder.hpp>
#include <luna/NetworkService.hpp>
#include <luna/RealtimeStreamer.hpp>
#include <luna/LunaInterface.hpp>
#include <luna/TlsConfiguration.hpp>

namespace luna
{
    struct RealtimeNetworkService : NetworkService
    {
        explicit RealtimeNetworkService(LunaNetworkInterface * luna, std::string const & name, DirectController * controller, ControllerHandle * handle) :
            mStreamer(
                luna,
                controller,
                handle,
                std::move(luna->tlsConfiguration()->makeDtlsConfiguration()),
                luna->ioContext()
            ),
            mResponder(
                luna->ioContext(),
                mStreamer.port(),
                name,
                luna->device()->strands()
            )
        {}

        ~RealtimeNetworkService() final = default;
    private:
        RealtimeStreamer mStreamer;
        DiscoveryResponder mResponder;
    };

    struct RealtimePluginInstance : PluginInstance
    {
        RealtimePluginInstance(LunaInterface * luna, std::string const & name) :
            mName(name),
            mHandle(luna->addController(&mController))
        {}

        void onNetworkAvaliable(LunaNetworkInterface * luna) override
        {
            luna->addNetworkService(std::make_unique<RealtimeNetworkService>(luna, mName, &mController, mHandle.get()));
        }
        
    private:
        std::string const & mName;
        DirectController mController;
        std::unique_ptr<ControllerHandle> mHandle;
    };

    RealtimePlugin::RealtimePlugin(std::string && name) : 
        mName(std::move(name))
    {}

    std::unique_ptr<PluginInstance> RealtimePlugin::instantiate(LunaInterface * luna)
    {
        return std::make_unique<RealtimePluginInstance>(luna, mName);
    }
}