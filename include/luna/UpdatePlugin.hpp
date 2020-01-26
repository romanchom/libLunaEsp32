#pragma once

#include <luna/Plugin.hpp>

namespace luna
{
    struct UpdatePlugin : Plugin
    {
        Service * initializeService() final;
        std::unique_ptr<NetworkService> initializeNetworking(NetworkingContext const & network) final;
    };
}
