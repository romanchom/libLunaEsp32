#pragma once

#include <driver/ledc.h>
#include <cstdint>

namespace luna::esp32
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

struct PWM
{
    explicit PWM(PWMTimer * timer, uint32_t pin);

    void duty(float duty);

private:
    uint32_t calculateDuty(float duty) const;

    PWMTimer * const mTimer;
    ledc_channel_t const mChannel;
};

}

