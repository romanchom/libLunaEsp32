#include <luna/esp32/StrandWS281x.hpp>

#include <esp_log.h>

#include <algorithm>
#include <cstring>

static char const TAG[] = "WS2811";


namespace luna {
namespace esp32 {

StrandWS281x::StrandWS281x(WS2812Configuration const & configuration) :
    Strand(configuration),
    mDriver(configuration.gpioPin, configuration.pixelCount)
{
    mConfiguration.bitDepth = luna::BitDepth::integer8;
    mConfiguration.colorChannels = luna::ColorChannels::RedGreenBlue;

    enabled(false);
}

void StrandWS281x::enabled(bool value)
{
    if (!value) {
        auto & data = mDriver.data();
        std::fill(data.begin(), data.end(), 0);
        mDriver.send();
    }
}

StrandWS2811::StrandWS2811(WS2812Configuration const & configuration) :
    StrandWS281x(configuration)
{}

void StrandWS2811::display(uint8_t const * data, size_t length)
{
    auto bytes = mDriver.data().data();
    for (size_t i = 0; i < length; i += 3) {
        bytes[i + 1] = data[i + 0];
        bytes[i + 2] = data[i + 1];
        bytes[i + 0] = data[i + 2];
    }

    mDriver.send();
}

StrandWS2812::StrandWS2812(WS2812Configuration const & configuration) :
    StrandWS281x(configuration)
{
    mConfiguration.colorSpace = {
        { 0.28623f, 0.27455f },
        { 0.68934f, 0.31051f },
        { 0.13173f, 0.77457f },
        { 0.13450f, 0.04598f },
    };
}

void StrandWS2812::display(uint8_t const * data, size_t length)
{
    auto bytes = mDriver.data().data();
    for (size_t i = 0; i < length; i += 3) {
        bytes[i + 1] = data[i + 0];
        bytes[i + 0] = data[i + 1];
        bytes[i + 2] = data[i + 2];
    }

    mDriver.send();
}

}}
