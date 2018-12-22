#pragma once

#include <cstdint>
#include <cstddef>

namespace luna {
namespace esp32 {

struct ControlChannelConfig
{
    uint8_t const * privateKey;
    size_t privateKeySize;
    uint8_t const * certificate;
    size_t certificateSize;
    uint8_t const * caCertificate;
    size_t caCertificateSize;
    uint16_t listenPort;
};

}}
