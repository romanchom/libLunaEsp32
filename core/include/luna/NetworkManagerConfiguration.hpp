#pragma once

#include "TlsConfiguration.hpp"

#include <string>

namespace luna
{
    struct NetworkManagerConfiguration
    {
        std::string_view name;
        std::string_view mqttAddress;
        TlsConfiguration tls;
    };
}
