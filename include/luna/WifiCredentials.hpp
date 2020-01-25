#pragma once

#include <string_view>

namespace luna
{
    struct WifiCredentials
    {
        std::string_view ssid;
        std::string_view password;
    };
}