#include "luna/esp32/StrandWS281x.hpp"

#include "luna/esp32/StrandDataProducer.hpp"

#include <esp_log.h>

#include <algorithm>
#include <cstring>

static char const TAG[] = "WS2811";

namespace luna {
namespace esp32 {

StrandWS281x::StrandWS281x(WS2812Configuration const & configuration) :
    Strand(configuration),
    mDriver(configuration.gpioPin, configuration.pixelCount),
    mDirty(true)
{
    mConfiguration.bitDepth = luna::esp32::BitDepth::integer8;
    mConfiguration.colorChannels = luna::esp32::ColorChannels::RedGreenBlue;
}

void StrandWS281x::takeData(StrandDataProducer const * producer)
{
    producer->produceRGB(mConfiguration, reinterpret_cast<RGB *>(mDriver.data()));
    mDirty = true;
}

void StrandWS281x::render()
{
    mDriver.send();
    mDirty = false;
}

StrandWS2811::StrandWS2811(WS2812Configuration const & configuration) :
    StrandWS281x(configuration)
{
    mConfiguration.colorSpace = {
        { 0.28623f, 0.27455f },
        { 0.13450f, 0.04598f },
        { 0.68934f, 0.31051f },
        { 0.13173f, 0.77457f },
    };
}

StrandWS2812::StrandWS2812(WS2812Configuration const & configuration) :
    StrandWS281x(configuration)
{
    mConfiguration.colorSpace = {
        { 0.28623f, 0.27455f },
        { 0.13173f, 0.77457f },
        { 0.68934f, 0.31051f },
        { 0.13450f, 0.04598f },
    };
}

}}
