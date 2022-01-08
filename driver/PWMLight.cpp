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
    namespace
    {
        float unlerp(float a, float b, float x) {
            return (x - a) / (b - a);
        }
    }

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
        set(rgbw, 500);
    }

    void PWMLight::fill(Generator * generator)
    {
        auto const cie = generator->generate(0.5f);
        auto const rgbw = mTransformation.transform(cie);
        set(rgbw, generator->whiteTemperature());
    }

    void PWMLight::set(prism::RGB rgbw, float temperature)
    {
        auto norm = rgbw.head<3>().maxCoeff();
        if (norm > 1.0f) {
            rgbw.head<3>() /= norm;
        }

        float totalCurrentDraw = 0.0f;
        auto tempT = std::clamp(unlerp(153, 500, temperature), 0.0f, 1.0f);
        for (auto & output : mOutputs) {
            float duty;
            if (output.color == CoolWhite) {
                duty = rgbw[3] * (1 - tempT);
            } else if (output.color == WarmWhite) {
                duty = rgbw[3] * tempT;
            } else {
                auto const index = static_cast<int>(output.color);
                duty = rgbw[index];
            }
            totalCurrentDraw += output.maximumCurrentDraw * duty;
            output.pwm->duty(duty);
        }
        notify(totalCurrentDraw);
    }
}
