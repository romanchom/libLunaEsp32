#pragma once

#include "DirectService.hpp"

#include <luna/Plugin.hpp>
#include <luna/HardwareController.hpp>

namespace luna
{
    struct RealtimePlugin : Plugin
    {
        explicit RealtimePlugin(std::string && name, HardwareController * hardware);

        Service * initializeService() final;
        std::unique_ptr<NetworkService> initializeNetworking(NetworkingContext const & network) final;
    private:
        std::string mName;
        HardwareController * mHardware;
        DirectService mService;
    };
}
