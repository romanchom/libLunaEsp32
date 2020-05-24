#pragma once

#include "LunaContext.hpp"
#include "NetworkService.hpp"

#include <memory>

namespace luna
{
    struct Controller;
    struct EventLoop;
    struct ControllerMux;
    struct NetworkingContext;

    struct Plugin
    {
        virtual Controller * getController(LunaContext const & context) = 0;
        virtual std::unique_ptr<NetworkService> makeNetworkService(LunaContext const & context, NetworkingContext const & network) = 0;
    };
}
