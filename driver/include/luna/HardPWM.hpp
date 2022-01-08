#pragma once

#include "PWM.hpp"

#include <driver/ledc.h>

#include <cstdint>

namespace luna
{
    struct PWMTimer
    {
        explicit PWMTimer(uint32_t timerId, uint32_t frequency, uint32_t resolutionBits);
        ledc_channel_t nextChannel();
        ledc_timer_t id() const;
        uint32_t steps() const;
        ledc_mode_t mode() const;
        float frequency() const;
    private:
        ledc_timer_t const mTimerId;
        int mCurrentChannel;
        uint32_t mSteps;
        float mFrequency;
    };

    struct HardPWM : PWM
    {
        explicit HardPWM(PWMTimer * timer, uint32_t pin);

        void duty(float duty) override;

    private:
        uint32_t calculateDuty(float duty) const;

        PWMTimer * const mTimer;
        ledc_channel_t const mChannel;
    };
}
