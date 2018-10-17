#pragma once

#include <mbedtls/ssl_cookie.h>

namespace tls {

class Cookie {
public:
    virtual ~Cookie() = default;
    
    virtual mbedtls_ssl_cookie_write_t * getWriter() = 0;
    virtual mbedtls_ssl_cookie_check_t * getChecker() = 0;
    virtual void * getContext() = 0;
};

}
