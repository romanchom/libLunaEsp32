#include "luna/esp32/AsioTimer.hpp"

#include <chrono>

namespace luna::esp32
{
    AsioTimer::AsioTimer(asio::io_context * ioContext) :
        mTimer(*ioContext)
    {}

    AsioTimer::~AsioTimer() = default;


    void AsioTimer::onTimeout(std::function<void()> && callback)
    {
        mCallback = callback;
    }

    void AsioTimer::startTimers(uint32_t intermediateDelayMilliseconds, uint32_t finalDelayMilliseconds)
    {
        if (intermediateDelayMilliseconds > 0) {
            mTimer.expires_after(std::chrono::milliseconds(intermediateDelayMilliseconds));
            mTimer.async_wait([=](asio::error_code const & error) {
                if (!error) {
                    mTimerState = TimerState::intermediatePassed;
                    startFinal(finalDelayMilliseconds - intermediateDelayMilliseconds);
                } else {
                    mTimerState = TimerState::canceled;
                }
            });

            mTimerState = TimerState::started;
        } else if (finalDelayMilliseconds > 0) {
            startFinal(finalDelayMilliseconds);
            mTimerState = TimerState::started;
        } else {
            mTimer.cancel();
            mTimerState = TimerState::canceled;
        }
    }

    void AsioTimer::startFinal(uint32_t delay) {
        mTimer.expires_after(std::chrono::milliseconds(delay));
        mTimer.async_wait([=](asio::error_code const & error){
            if (!error) {
                mTimerState = TimerState::finalPassed;
                mCallback();
            } else {
                mTimerState = TimerState::canceled;
            }
        });
    }

    mbedtls_ssl_set_timer_t * AsioTimer::getDelaySetter()
    {
        return [](void * context, uint32_t intermediateDelayMilliseconds, uint32_t final_delay_milliseconds) {
            auto self = static_cast<AsioTimer *>(context);
            self->startTimers(intermediateDelayMilliseconds, final_delay_milliseconds);
        };
    }

    mbedtls_ssl_get_timer_t * AsioTimer::getDelayGetter()
    {
        return [](void * context) {
            auto const self = static_cast<AsioTimer *>(context);
            return static_cast<int>(self->mTimerState);
        };
    }

    void * AsioTimer::getContext()
    {
        return reinterpret_cast<void *>(this);
    }
}
