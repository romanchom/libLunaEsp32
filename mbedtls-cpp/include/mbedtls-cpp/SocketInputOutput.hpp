#pragma once

#include "BasicInputOutput.hpp"

#include <string>

#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>

#include "Exception.hpp"

namespace tls {

enum class Protocol : int {
    tcp = MBEDTLS_NET_PROTO_TCP,
    udp = MBEDTLS_NET_PROTO_UDP,
};

struct Address {
    char data[16];
    size_t size;
};

class SocketInputOutput : public BasicInputOutput {
private:
    mbedtls_net_context mNet;
    bool mInitialized;
public:
    SocketInputOutput()
    {
        mbedtls_net_init(&mNet);
    }

    ~SocketInputOutput() override
    {
        mbedtls_net_free(&mNet);
    }

    SocketInputOutput(SocketInputOutput && orig) :
        mNet(orig.mNet)
    {
        mbedtls_net_init(&orig.mNet);
    }

    void operator=(SocketInputOutput && orig)
    {
        mbedtls_net_free(&mNet);
        mNet = orig.mNet;
        mbedtls_net_init(&orig.mNet);
    }

    mbedtls_ssl_send_t * getSender() override
    {
        return &mbedtls_net_send;
    }

    mbedtls_ssl_recv_t * getReceiver()  override
    {
        return &mbedtls_net_recv;
    }

    mbedtls_ssl_recv_timeout_t * getReceiverTimeout() override
    {
        return &mbedtls_net_recv_timeout;
    }

    void * getContext() override
    {
        return reinterpret_cast<void *>(&mNet);
    }

    void bind(const char * bind_ip, const char *port, Protocol proto)
    {
        auto error = mbedtls_net_bind(&mNet, bind_ip, port, static_cast<int>(proto));
        if (0 != error) {
            throw tls::Exception(error);
        }
    }

    SocketInputOutput accept()
    {
        SocketInputOutput ret;

        auto error = mbedtls_net_accept(&mNet, &ret.mNet,
            nullptr, //reinterpret_cast<void *>(&address->data),
            0, //sizeof(address->data),
            nullptr);//&address->size);

        if (0 != error) {
            throw tls::Exception(error);
        }

        return ret;
    }

    void connect(const char * ip, const char *port, Protocol proto)
    {
        int error = mbedtls_net_connect(&mNet, ip, port, static_cast<int>(proto));
        if (0 != error) {
            throw tls::Exception(error);
        }
    }

    void blocking(bool value)
    {
        int error;
        if (value) {
            error = mbedtls_net_set_block(&mNet);
        } else {
            error = mbedtls_net_set_nonblock(&mNet);
        }
        if (0 != error) {
            throw tls::Exception(error);
        }
    }
};

}
