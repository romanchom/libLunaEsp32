#include "PWMLight.hpp"

#include <luna/proto/Scalar.hpp>
#include <luna/Generator.hpp>

#include <esp_log.h>

#include <limits>
#include <algorithm>

namespace
{
    auto const TAG = "PWMLight";
}

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

    void PWMLight::rawBytes(std::byte const * data, size_t const size)
    {
        auto const expectedSize = sizeof(uint16_t) * 4;
        if (size != expectedSize) {
            ESP_LOGW(TAG, "Size mismatch %d %d", size, expectedSize);
            return;
        }

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
        auto norm = rgbw.head<3>().maxCoeff();
        if (norm > 1.0f) {
            rgbw.head<3>() /= norm;
        }

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
