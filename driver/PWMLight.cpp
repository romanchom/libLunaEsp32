#include "PWMLight.hpp"

#include <luna/proto/Scalar.hpp>
#include <luna/Generator.hpp>

#include <limits>
#include <algorithm>

namespace luna
{
    PWMLight::PWMLight(Location const & location, prism::RGBColorSpace const & colorSpace, std::initializer_list<OutputChannel> channels) :
        Strand(location),
        mOutputs(channels.begin(), channels.end()),
        mColorSpace(colorSpace),
        mTransformation(colorSpace)
    {}

    size_t PWMLight::pixelCount() const noexcept
    {
        return 1;
    }

    proto::Format PWMLight::format() const noexcept
    {
        return proto::Format::RGBW16;
    }

    prism::RGBColorSpace PWMLight::colorSpace() const noexcept
    {
        return mColorSpace;
    }

    void PWMLight::rawBytes(std::byte const * data, size_t size)
    {
        assert(size == 4 * 2);
        auto const value = reinterpret_cast<proto::Scalar<uint16_t> const *>(data);

        prism::RGB rgbw;
        for (int i = 0; i < 4; ++i) {
            rgbw[i] = float(value[i]) / float(std::numeric_limits<uint16_t>::max());
        }
        set(rgbw);
    }

    void PWMLight::fill(Generator * generator)
    {
        auto const cie = generator->generate(0.5f);
        auto const rgbw = mTransformation.transform(cie);
        set(rgbw);
    }

    void PWMLight::set(prism::RGB rgbw)
    {
        rgbw = rgbw.cwiseMax(0.0f).cwiseMin(1.0f);
        float totalCurrentDraw = 0.0f;
        for (auto & output : mOutputs) {
            auto const index = static_cast<int>(output.color);
            auto const duty = rgbw[index];
            totalCurrentDraw += output.maximumCurrentDraw * duty;
            output.pwm->duty(duty);
        }
        notify(totalCurrentDraw);
    }
}
