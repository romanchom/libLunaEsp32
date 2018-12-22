#pragma once

#include <mbedtls/ssl.h>

namespace tls {

class Timer {
public:
    virtual ~Timer() = default;
    virtual mbedtls_ssl_set_timer_t * getDelaySetter() = 0;
    virtual mbedtls_ssl_get_timer_t * getDelayGetter() = 0;
    virtual void * getContext() = 0;
};

}
