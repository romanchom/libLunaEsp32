#pragma once

#include "RGB.hpp"

#include <cstdint>

namespace luna::esp32
{

struct Generator
{
    virtual ~Generator() = default;
    virtual RGB<uint8_t> generateRGB() = 0;
};

}
