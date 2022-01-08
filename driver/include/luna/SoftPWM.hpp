#pragma once

#include "PWM.hpp"

#include <driver/pwm.h>

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace luna
{

struct SoftPWM
{
    SoftPWM(std::initializer_list<uint32_t> gpioPins);

    PWM * getChannel(int index);
    void commit();
private:
    struct Channel : PWM
    {
        explicit Channel(int index);
        void duty(float duty) override;
    private:
        uint32_t mIndex;
    };

    std::vector<Channel> mChannels;
};


}
