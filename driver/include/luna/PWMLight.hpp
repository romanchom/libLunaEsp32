#pragma once

#include "PWM.hpp"

#include <luna/ElectricalLoad.hpp>
#include <luna/Strand.hpp>

namespace luna
{

struct PWMLight : Strand, ElectricalLoad
{
    enum ColorChannel
    {
        Red,
        Green,
        Blue,
        White,
    };

    struct OutputChannel
    {
        PWM * pwm;
        ColorChannel color;
        float maximumCurrentDraw = 0.0f;
    };

    explicit PWMLight(Location const & location, prism::RGBColorSpace const & colorSpace, std::initializer_list<OutputChannel> channels);
    size_t pixelCount() const noexcept override;
    proto::Format format() const noexcept override;
    prism::RGBColorSpace colorSpace() const noexcept override;
    void rawBytes(std::byte const * data, size_t size) override;
    void fill(Generator * generator) final;
private:
    void set(prism::RGB rgbw);
    std::vector<OutputChannel> mOutputs;
    prism::RGBColorSpace mColorSpace;
    prism::RGBColorSpaceTransformation mTransformation;
};

}
