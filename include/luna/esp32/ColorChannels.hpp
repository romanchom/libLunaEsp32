#pragma once

#include <cstdint>

namespace luna
{
namespace esp32
{

enum ColorChannels : uint32_t
{
    Red = 1 << 0,
    Green = 1 << 1,
    Blue = 1 << 2,
    White = 1 << 3,
    RedGreenBlue = Red | Green | Blue,
};

}
}
