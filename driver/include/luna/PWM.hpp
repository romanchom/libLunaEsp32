#pragma once

namespace luna
{
    struct PWM
    {
        virtual void duty(float duty) = 0;
    protected:
        ~PWM() = default;
    };
}
