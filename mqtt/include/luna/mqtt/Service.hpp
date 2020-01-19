#pragma once

#include "Client.hpp"

#include <luna/Configurable.hpp>
#include <luna/EffectEngine.hpp>

#include <string_view>

namespace luna::mqtt
{
    struct Service
    {
        explicit Service(EffectEngine * effectEngine, std::string name, Client::Configuration const & configuration);

        Client * client() { return &mClient; }
        EffectEngine * effectEngine() { return mEffectEngine; }
    private:
        void subscribeConfigurable(Configurable * configurable, std::string name);

        Client mClient;
        EffectEngine * mEffectEngine;
    };
}
