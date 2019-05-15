#pragma once

#include "luna/esp32/Strand.hpp"

#include <WS281xDriver.hpp>

namespace luna {
namespace esp32 {

struct WS2812Configuration : StrandConfiguration
{
    int gpioPin;
};

class StrandWS281x : public Strand
{
public:
    explicit StrandWS281x(WS2812Configuration const & configuration);
    void takeData(StrandDataProducer const * producer) override;
    void render() override;
protected:
    WS281xDriver mDriver;
    bool mDirty;
};

class StrandWS2811 : public StrandWS281x
{
public:
    explicit StrandWS2811(WS2812Configuration const & configuration);
};

class StrandWS2812 : public StrandWS281x
{
public:
    explicit StrandWS2812(WS2812Configuration const & configuration);
};

}}
