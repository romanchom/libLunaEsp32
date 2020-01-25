#pragma once

#include <memory>

namespace luna
{
    struct NetworkingContext;
    struct Service;
    struct NetworkService;

    struct Plugin
    {
        virtual Service * initializeService();
        virtual std::unique_ptr<NetworkService> initializeNetworking(NetworkingContext const & network) = 0;
    };
}
