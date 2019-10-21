#pragma once

#include "Client.hpp"

#include <luna/Service.hpp>
#include <luna/NetworkManagerConfiguration.hpp>
#include <luna/EffectEngine.hpp>

namespace luna::mqtt
{
    struct Service
    {
        explicit Service(asio::io_context * ioContext, EffectEngine * effectEngine, NetworkManagerConfiguration const & configuration);

        void start();
    private:
        Client mClient;
        EffectEngine * mEffectEngine;
    };
}
