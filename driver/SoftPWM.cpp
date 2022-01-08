#include "SoftPWM.hpp"

namespace luna
{
    namespace {
        uint32_t const period = 1000;
        uint32_t zeros[8] = {0};
    }

    SoftPWM::SoftPWM(std::initializer_list<uint32_t> gpioPins)
    {
        pwm_init(period, zeros, gpioPins.size(), gpioPins.begin());
        float phases[8] = {0,};
        pwm_set_phases(phases);

        mChannels.reserve(gpioPins.size());
        int index = 0;
        for (auto pin : gpioPins)
        {
            mChannels.emplace_back(index++);
        }
    }

    PWM * SoftPWM::getChannel(int index)
    {
        return &mChannels[index];
    }

    void SoftPWM::commit()
    {
        pwm_start();
    }

    SoftPWM::Channel::Channel(int index) :
        mIndex(index)
    {}

    void SoftPWM::Channel::duty(float duty)
    {
        pwm_set_duty(mIndex, uint32_t(duty * period));
    }
}
