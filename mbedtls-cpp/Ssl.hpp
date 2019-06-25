#pragma once

#include <stdexcept>
#include <iostream>
#include <string>

#include <mbedtls/ssl.h>

#include "Configuration.hpp"
#include "Timer.hpp"
#include "BasicInputOutput.hpp"
#include "Exception.hpp"


namespace tls {

class Ssl {
private:
    mbedtls_ssl_context mSsl;
public:
    Ssl()
    {
        mbedtls_ssl_init(&mSsl);
    }

    ~Ssl()
    {
        mbedtls_ssl_free(&mSsl);
    }

    void setup(Configuration * configuration)
    {
        auto error = mbedtls_ssl_setup(&mSsl, configuration->get());
        if (0 != error) {
            throw tls::Exception(error);
        }
    }

    void resetSession()
    {
        auto error = mbedtls_ssl_session_reset(&mSsl);
        if (0 != error) {
            throw tls::Exception(error);
        }
    }

    void setTimer(Timer * timer)
    {
        mbedtls_ssl_set_timer_cb(&mSsl,
            timer->getContext(),
            timer->getDelaySetter(),
            timer->getDelayGetter());
    }

    void setClientId(const unsigned char * data, size_t dataLength)
    {
        auto error = mbedtls_ssl_set_client_transport_id(&mSsl, data, dataLength);
        if (0 != error) {
            throw tls::Exception(error);
        }
    }

    void setInputOutput(BasicInputOutput * bio)
    {
        mbedtls_ssl_set_bio(&mSsl,
            bio->getContext(),
            bio->getSender(),
            bio->getReceiver(),
            bio->getReceiverTimeout());
    }

    void setHostName(const char * host_name)
    {
        int error = mbedtls_ssl_set_hostname(&mSsl, host_name);
        if (0 != error) {
            throw tls::Exception(error);
        }
    }

    bool handshake()
    {
        int error;
        do {
            error = mbedtls_ssl_handshake(&mSsl);
        } while(error == MBEDTLS_ERR_SSL_WANT_READ
                || error == MBEDTLS_ERR_SSL_WANT_WRITE);

        if (0 != error && MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED != error) {
            throw tls::Exception(error);
        }

        return 0 == error;
    }

    int handshakeStep()
    {
        return mbedtls_ssl_handshake_step(&mSsl);
    }


    int read(std::byte * data, size_t dataLength)
    {
        int ret;
        ret = mbedtls_ssl_read(&mSsl, (unsigned char *) data, dataLength);

        if (ret < 0) {
            switch (ret) {
                case MBEDTLS_ERR_SSL_WANT_READ:
                case MBEDTLS_ERR_SSL_WANT_WRITE:
                case MBEDTLS_ERR_SSL_TIMEOUT:
                    ret = 0;
                    break;
                case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                    ret = -1;
                    break;
                default:
                    throw tls::Exception(ret);
            }
        }

        return ret;
    }

    int write(std::byte const * data, size_t dataLength)
    {
        int ret;
        do {
            ret = mbedtls_ssl_write(&mSsl, (unsigned char const *)data, dataLength);
        } while (MBEDTLS_ERR_SSL_WANT_READ == ret
                || MBEDTLS_ERR_SSL_WANT_WRITE == ret);

        if (ret < 0) {
            throw tls::Exception(ret);
        }

        return ret;
    }

    void closeNotify()
    {
        int ret;
        do {
            ret = mbedtls_ssl_close_notify(&mSsl);
        } while(MBEDTLS_ERR_SSL_WANT_WRITE == ret);
    }

    bool inHandshakeOver() const
    {
        return mSsl.state == MBEDTLS_SSL_HANDSHAKE_OVER;
    }
};

}
