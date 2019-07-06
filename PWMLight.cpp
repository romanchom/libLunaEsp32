#include "luna/esp32/PWMLight.hpp"
#include "luna/esp32/Generator.hpp"

#include <luna/proto/Scalar.hpp>

#include <limits>
#include <algorithm>

namespace luna::esp32
{
    PWMLight::PWMLight(Location const & location, int pin, PWMTimer * timer, float maximumCurrentDraw) :
        Strand(location),
        mPWM(timer, pin),
        mMaximumCurrentDraw(maximumCurrentDraw)
    {}

    size_t PWMLight::pixelCount() const noexcept
    {
        return 1;
    }

    proto::Format PWMLight::format() const noexcept
    {
        return proto::Format::White16;
    }

    prism::RGBColorSpace PWMLight::colorSpace() const noexcept
    {
        return {};
    }

    void PWMLight::rawBytes(std::byte const * data, size_t size)
    {
        assert(size == 2);
        auto value = reinterpret_cast<proto::Scalar<uint16_t> const *>(data);

        float duty = float(*value) / float(std::numeric_limits<uint16_t>::max());
        set(duty);
    }

    void PWMLight::fill(Generator const * generator)
    {
        auto cie = generator->generate(0.5f, location());
        set(cie[3]);
    }

    void PWMLight::set(float duty)
    {
        duty = std::clamp(duty, 0.0f, 1.0f);
        mPWM.duty(duty);
        notify(duty * mMaximumCurrentDraw);
    }
}
