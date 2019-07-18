#pragma once

#include "WiFi.hpp"
#include <luna/NetworkManagerConfiguration.hpp>

namespace luna
{
    struct Configuration
    {
        WiFi::Config wifi;
        NetworkManagerConfiguration network;
    };
}
