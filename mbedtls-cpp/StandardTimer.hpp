#pragma once

#include "Timer.hpp"

#include <mbedtls/ssl.h>
#include <mbedtls/timing.h>


namespace tls {

class StandardTimer : public Timer {
private:
    mbedtls_timing_delay_context mTimer;
public:
    mbedtls_ssl_set_timer_t * getDelaySetter() override 
    {
        return &mbedtls_timing_set_delay;
    }

    mbedtls_ssl_get_timer_t * getDelayGetter() override 
    {
        return &mbedtls_timing_get_delay;
    }

    void * getContext() override
    {
        return reinterpret_cast<void *>(&mTimer);
    }
};

}