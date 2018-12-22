#pragma once

#include <luna/Strand.hpp>

#include <WS281xDriver.hpp>

#include <cstdint>
#include <cstddef>

namespace luna {
namespace esp32 {

struct WS2812Configuration : luna::StrandConfiguration
{
    int gpioPin;
};

class StrandWS281x : public luna::Strand
{
public:
    explicit StrandWS281x(WS2812Configuration const & configuration);
    void enabled(bool value) override;

protected:
    WS281xDriver mDriver;
};

class StrandWS2811 : public StrandWS281x
{
public:
    explicit StrandWS2811(WS2812Configuration const & configuration);
    void display(uint8_t const * data, size_t length) override;
};

class StrandWS2812 : public StrandWS281x
{
public:
    explicit StrandWS2812(WS2812Configuration const & configuration);
    void display(uint8_t const * data, size_t length) override;
};

}}
