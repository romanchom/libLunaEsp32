#pragma once

#include "luna/esp32/Strand.hpp"
#include "luna/proto/SetColor.hpp"

#include <WS281xDriver.hpp>

namespace luna {
namespace esp32 {

struct StrandWS281x : Strand<proto::RGB>
{
    explicit StrandWS281x(Location location, size_t pixelCount, int gpioPin);
    size_t pixelCount() const noexcept final;
    void setLight(proto::RGB const * data, size_t size, size_t offset) final;
    void render() final;
    proto::Format format() const noexcept final;
protected:
    WS281xDriver mDriver;
    bool mDirty;
};

struct StrandWS2811 : StrandWS281x
{
    using StrandWS281x::StrandWS281x;
    ColorSpace colorSpace() const noexcept final;
};

struct StrandWS2812 : StrandWS281x
{
    using StrandWS281x::StrandWS281x;
    ColorSpace colorSpace() const noexcept final;
};

}}
