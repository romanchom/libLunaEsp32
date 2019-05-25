#pragma once

#include "Strand.hpp"
#include "PWM.hpp"

namespace luna::esp32
{

struct PWMLight : Strand<uint16_t>
{
    explicit PWMLight(Location const & location, int pin, PWMTimer * timer);
    size_t pixelCount() const noexcept final;
    proto::Format format() const noexcept final;
    ColorSpace colorSpace() const noexcept final;
    void render() final;
    void setLight(uint16_t const * data, size_t size, size_t offset) final;
private:
    PWM mPWM;
};

}
