#pragma once

#include <string_view>

namespace luna
{
    struct TlsConfiguration
    {
        std::string_view ownKey;
        std::string_view ownCertificate;
        std::string_view caCertificate;
    };
}
