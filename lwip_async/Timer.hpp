#pragma once

#include <cstdint>
#include <functional>
#include "../mbedtls-cpp/Timer.hpp"

namespace lwip_async
{

class Timer : public tls::Timer
{
public:
    explicit Timer();
    ~Timer() override;

    mbedtls_ssl_set_timer_t * getDelaySetter() override;
    mbedtls_ssl_get_timer_t * getDelayGetter() override;
    void * getContext() override;

    void setOnTimedOut(std::function<void()> callback) {
        mOnTimedOutCallback = std::move(callback);
    }
private:
    enum TimerState : int8_t {
        canceled = -1,
        started = 0,
        intermediate_passed = 1,
        final_passed = 2,
    };

    static void intermediateTimeoutHandler(void * arg);
    static void finalTimeoutHandler(void * arg);

    void stopTimers();
    void startTimers(uint32_t intermediateDelayMilliseconds, uint32_t finalDelayMilliseconds);

    std::function<void()> mOnTimedOutCallback;
    TimerState mTimerState;
};

}
