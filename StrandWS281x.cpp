#include "luna/esp32/StrandWS281x.hpp"

#include <algorithm>
#include <cstring>
#include <cassert>

namespace luna {
namespace esp32 {

StrandWS281x::StrandWS281x(Location location, size_t pixelCount, int gpioPin) :
    Strand<RGB<uint8_t>>(location),
    mDriver(gpioPin, pixelCount),
    mDirty(true)
{}

void StrandWS281x::setLight(RGB<uint8_t> const * data, size_t size, size_t offset) 
{

    assert(offset < pixelCount());
    assert(offset + size <= pixelCount());

    memcpy(mDriver.data() + offset * 3, data, size * 3);
    mDirty = true;
}

size_t StrandWS281x::pixelCount() const noexcept
{
    return mDriver.size() / 3;
}

void StrandWS281x::render()
{
    if (mDirty) {
        mDriver.send();
        mDirty = false;
    }
}

ColorSpace StrandWS2811::colorSpace() const noexcept
{
    return {
        { 0.28623f, 0.27455f },
        { 0.13450f, 0.04598f },
        { 0.68934f, 0.31051f },
        { 0.13173f, 0.77457f },
    };
}

ColorSpace StrandWS2812::colorSpace() const noexcept
{
    return {
        { 0.28623f, 0.27455f },
        { 0.13173f, 0.77457f },
        { 0.68934f, 0.31051f },
        { 0.13450f, 0.04598f },
    };
}

}}
