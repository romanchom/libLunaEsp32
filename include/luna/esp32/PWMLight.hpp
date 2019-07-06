#pragma once

#include "Strand.hpp"
#include "PWM.hpp"
#include "ElectricalLoad.hpp"

namespace luna::esp32
{

struct PWMLight : Strand, ElectricalLoad
{
    explicit PWMLight(Location const & location, int pin, PWMTimer * timer, float maximumCurrentDraw = 0.0f);
    size_t pixelCount() const noexcept override;
    proto::Format format() const noexcept override;
    prism::RGBColorSpace colorSpace() const noexcept override;
    void rawBytes(std::byte const * data, size_t size) override;
    void fill(Generator const * generator) final;
private:
    void set(float duty);
    PWM mPWM;
    float mMaximumCurrentDraw;
};

}
