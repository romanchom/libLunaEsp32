#include "Timer.hpp"

#include <lwip/timeouts.h>

namespace lwip_async
{

Timer::Timer() :
    mTimerState(TimerState::canceled)
{}

Timer::~Timer()
{
    stopTimers();
}

void Timer::stopTimers()
{
    switch (mTimerState) {
    case TimerState::started:
        sys_untimeout(intermediateTimeoutHandler, this);
        // fallthrough
    case TimerState::intermediate_passed:
        sys_untimeout(finalTimeoutHandler, this);
        break;
    default:
        break;
    }
}

void Timer::startTimers(uint32_t intermediateDelayMilliseconds, uint32_t finalDelayMilliseconds)
{
    if (finalDelayMilliseconds > 0) {
        sys_timeout(finalDelayMilliseconds, finalTimeoutHandler, this);
        if (intermediateDelayMilliseconds > 0) {
            sys_timeout(intermediateDelayMilliseconds, intermediateTimeoutHandler, this);
            mTimerState = TimerState::started;
        } else {
            mTimerState = TimerState::intermediate_passed;
        }
    } else {
        mTimerState = TimerState::canceled;
    }
}

mbedtls_ssl_set_timer_t * Timer::getDelaySetter()
{
    return [](void * context, uint32_t intermediateDelayMilliseconds, uint32_t final_delay_milliseconds) {
        auto self = reinterpret_cast<Timer *>(context);
        self->stopTimers();
        self->startTimers(intermediateDelayMilliseconds, final_delay_milliseconds);
    };
}

mbedtls_ssl_get_timer_t * Timer::getDelayGetter()
{
    return [](void * context) {
        auto const self = reinterpret_cast<Timer *>(context);
        return static_cast<int>(self->mTimerState);
    };
}

void * Timer::getContext()
{
    return reinterpret_cast<void *>(this);
}

void Timer::intermediateTimeoutHandler(void * argument)
{
    auto const self = reinterpret_cast<Timer *>(argument);
    self->mTimerState = TimerState::intermediate_passed;
}

void Timer::finalTimeoutHandler(void * argument)
{
    auto const self = reinterpret_cast<Timer *>(argument);
    self->mTimerState = TimerState::final_passed;
}

}
