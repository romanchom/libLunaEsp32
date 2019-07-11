#pragma once

#include <string>

namespace luna
{
    struct NetworkManagerConfiguration
    {
        std::string name;
        std::string mqttAddress;
        uint8_t const * ownKey;
        size_t ownKeySize;
        uint8_t const * ownCertificate;
        size_t ownCertificateSize;
        uint8_t const * caCertificate;
        size_t caCertificateSize;
    };
}
