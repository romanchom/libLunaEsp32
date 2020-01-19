#pragma once

#include "Client.hpp"

#include <luna/Configurable.hpp>

#include <string_view>

namespace luna::mqtt
{
    struct Service
    {
        explicit Service(Configurable * configurable, std::string name, Client::Configuration const & configuration);
    private:
        void subscribeConfigurable(Configurable * configurable, std::string name);

        Client mClient;
    };
}
