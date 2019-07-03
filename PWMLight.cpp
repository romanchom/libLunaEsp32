#include "luna/esp32/PWMLight.hpp"

#include <luna/proto/Scalar.hpp>

#include <limits>

namespace luna::esp32
{

PWMLight::PWMLight(Location const & location, int pin, PWMTimer * timer) :
    Strand(location),
    mPWM(timer, pin)
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
    mPWM.duty(duty);
}

}
