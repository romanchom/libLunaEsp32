#pragma once

#include "Cookie.hpp"

#include <mbedtls/ssl_cookie.h>

#include "RandomGenerator.hpp"

namespace tls {

class StandardCookie : public Cookie {
private:
    mbedtls_ssl_cookie_ctx  mCookie;
public:
    StandardCookie() 
    {
        mbedtls_ssl_cookie_init(&mCookie);
    }

    ~StandardCookie() override 
    {
        mbedtls_ssl_cookie_free(&mCookie);
    }

    mbedtls_ssl_cookie_write_t * getWriter() override 
    {
        return &mbedtls_ssl_cookie_write;
    }

    mbedtls_ssl_cookie_check_t * getChecker() 
    {
        return &mbedtls_ssl_cookie_check;
    }

    void * getContext() override 
    {
        return reinterpret_cast<void *>(&mCookie);   
    }
    
    void setup(RandomGenerator * rg)
    {
        mbedtls_ssl_cookie_setup(&mCookie, rg->getCallback(), rg->getContext());
    }
};

}
