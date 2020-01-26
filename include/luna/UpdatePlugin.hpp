#pragma once

#include <luna/Plugin.hpp>

namespace luna
{
    struct UpdatePlugin : Plugin
    {
        Controller * getController() final;
        std::unique_ptr<NetworkService> makeNetworkService(NetworkingContext const & network) final;
    };
}
