#pragma once

#include <string_view>

namespace luna
{
    struct Configuration
    {
        struct Wifi
        {
            std::string_view ssid;
            std::string_view password;
        };

        struct Network
        {
            std::string_view name;
            std::string_view mqttAddress;
            std::string_view ownKey;
            std::string_view ownCertificate;
            std::string_view caCertificate;
        };

        Wifi wifi;
        Network network;
    };
}
