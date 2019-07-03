#pragma once

#include "Strand.hpp"
#include "PWM.hpp"

namespace luna::esp32
{

struct PWMLight : Strand
{
    explicit PWMLight(Location const & location, int pin, PWMTimer * timer);
    size_t pixelCount() const noexcept override;
    proto::Format format() const noexcept override;
    prism::RGBColorSpace colorSpace() const noexcept override;
    void rawBytes(std::byte const * data, size_t size) override;
private:
    PWM mPWM;
};

}
