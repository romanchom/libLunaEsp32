#pragma once

#include "Client.hpp"

#include <luna/EffectEngine.hpp>

#include <string_view>

namespace luna::mqtt
{
    struct Service
    {
        explicit Service(EffectEngine * effectEngine, std::string_view name, Client::Configuration const & configuration);

        void enabled(bool on);
    private:
        Client mClient;
    };
}
