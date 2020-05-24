#pragma once

#include <luna/Plugin.hpp>

namespace luna
{
    struct UpdatePlugin : Plugin
    {
        Controller * getController(LunaContext const & context) final;
        std::unique_ptr<NetworkService> makeNetworkService(LunaContext const & context,NetworkingContext const & network) final;
    };
}
