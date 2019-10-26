#pragma once

#include "Client.hpp"

#include <luna/NetworkManagerConfiguration.hpp>
#include <luna/EffectEngine.hpp>

namespace luna::mqtt
{
    struct Service
    {
        explicit Service(EffectEngine * effectEngine, NetworkManagerConfiguration const & configuration);

        void start();
    private:
        Client mClient;
        EffectEngine * mEffectEngine;
    };
}
