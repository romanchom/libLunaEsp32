#include "luna/esp32/PWMLight.hpp"

#include <limits>

namespace luna::esp32
{

PWMLight::PWMLight(Location const & location, int pin, PWMTimer * timer) :
    Strand<proto::Scalar<uint16_t>>(location),
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

ColorSpace PWMLight::colorSpace() const noexcept
{
    return {};
}

void PWMLight::render()
{}

void PWMLight::setLight(proto::Scalar<uint16_t> const * data, size_t size, size_t offset)
{
    assert(size == 1);
    assert(offset == 0);
    float value = float(*data) / float(std::numeric_limits<uint16_t>::max());
    mPWM.duty(value);
}

}
