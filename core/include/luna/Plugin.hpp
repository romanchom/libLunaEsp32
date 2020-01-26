#pragma once

#include <memory>

namespace luna
{
    struct NetworkingContext;
    struct Controller;
    struct NetworkService;

    struct Plugin
    {
        virtual Controller * getController() = 0;
        virtual std::unique_ptr<NetworkService> makeNetworkService(NetworkingContext const & network) = 0;
    };
}
