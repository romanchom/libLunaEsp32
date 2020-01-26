#pragma once

#include <luna/Device.hpp>
#include <luna/DirectController.hpp>
#include <luna/Plugin.hpp>

namespace luna
{
    struct RealtimePlugin : Plugin
    {
        explicit RealtimePlugin(std::string && name, Device * device);

        Controller * getController() final;
        std::unique_ptr<NetworkService> makeNetworkService(NetworkingContext const & network) final;
    private:
        std::string mName;
        Device * mDevice;
        DirectController mController;
    };
}
