#pragma once

#include <mbedtls-cpp/Timer.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <functional>

namespace luna
{
    struct AsioTimer : tls::Timer
    {
        explicit AsioTimer(asio::io_context * ioContext);
        ~AsioTimer() final;

        void onTimeout(std::function<void()> && callback);

        mbedtls_ssl_set_timer_t * getDelaySetter() final;
        mbedtls_ssl_get_timer_t * getDelayGetter() final;
        void * getContext() final;

    private:
        void startTimers(uint32_t intermediateDelayMilliseconds, uint32_t finalDelayMilliseconds);
        void startFinal(uint32_t delay);

        TimerState mTimerState;
        asio::steady_timer mTimer;
        std::function<void()> mCallback;
    };
}
