#pragma once

#include <mbedtls/ssl.h>

namespace tls {
    class BasicInputOutput {
    public:
        virtual ~BasicInputOutput() = default;
        virtual mbedtls_ssl_send_t * getSender() = 0;
        virtual mbedtls_ssl_recv_t * getReceiver() = 0;
        virtual mbedtls_ssl_recv_timeout_t * getReceiverTimeout() = 0;
        virtual void * getContext() = 0;
    };
}