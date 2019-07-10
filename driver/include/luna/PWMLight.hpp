#pragma once

#include "PWM.hpp"

#include <luna/ElectricalLoad.hpp>
#include <luna/Strand.hpp>

namespace luna
{

struct PWMLight : Strand, ElectricalLoad
{
    explicit PWMLight(Location const & location, int pin, PWMTimer * timer, float maximumCurrentDraw = 0.0f);
    size_t pixelCount() const noexcept override;
    proto::Format format() const noexcept override;
    prism::RGBColorSpace colorSpace() const noexcept override;
    void rawBytes(std::byte const * data, size_t size) override;
    void fill(Generator * generator) final;
private:
    void set(float duty);
    PWM mPWM;
    float mMaximumCurrentDraw;
};

}
