#pragma once

#include "Strand.hpp"
#include "PWM.hpp"
#include "luna/proto/Scalar.hpp"

namespace luna::esp32
{

struct PWMLight : Strand<proto::Scalar<uint16_t>>
{
    explicit PWMLight(int pin, PWMTimer * timer);
    size_t pixelCount() const noexcept override;
    proto::Format format() const noexcept override;
    ColorSpace colorSpace() const noexcept override;
    void render() override;
    void setLight(proto::Scalar<uint16_t> const * data, size_t size, size_t offset) override;
private:
    PWM mPWM;
};

}
