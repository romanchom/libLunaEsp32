#pragma once

#include "DirectService.hpp"

#include <luna/Plugin.hpp>
#include <luna/Device.hpp>

namespace luna
{
    struct RealtimePlugin : Plugin
    {
        explicit RealtimePlugin(std::string && name, Device * device);

        Service * initializeService() final;
        std::unique_ptr<NetworkService> initializeNetworking(NetworkingContext const & network) final;
    private:
        std::string mName;
        Device * mDevice;
        DirectService mService;
    };
}
