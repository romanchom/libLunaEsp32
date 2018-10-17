#pragma once

#include "BasicInputOutput.hpp"

#include <string>
#include <sstream>
#include <iomanip>

#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>

#include "exception.hpp"

namespace tls {

enum class Protocol : int {
    tcp = MBEDTLS_NET_PROTO_TCP,
    udp = MBEDTLS_NET_PROTO_UDP,
};

struct Address {
    char data[16];
    size_t size;
    std::string toString() const {
        std::stringstream ss;
        if (4 == size) {
            int i = 0;
            while (true) {
                ss << static_cast<int>(data[i]);
                ++i;
                if (i >= 4) { break; }
                ss << '.';
            }
        } else if (16 == size) {
            ss << std::uppercase << std::setfill('0') << std::setw(4) << std::hex;
            int i = 0;
            while (true) {
                ss << static_cast<int>(data[i]);
                ++i;
                if (i >= 16) { break; }
                ss << ':';
            }
        } else {
            throw std::runtime_error("Unrepresentable ip address.");
        }
        return ss.str();
    }
};

class SocketInputOutput : public BasicInputOutput {
private:
    mbedtls_net_context mNet;
public:
    SocketInputOutput()
    {
        mbedtls_net_init(&mNet);
    }

    ~SocketInputOutput() override
    {
        close();
    }

    void close()
    {
        mbedtls_net_free(&mNet);
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

    void bind(const char * bind_ip, const char *port, protocol proto)
    {
        auto error = mbedtls_net_bind(&mNet, bind_ip, port, static_cast<int>(proto));
        if (0 != error) {
            throw tls::exception(error);
        }
    }

    void accept(SocketInputOutput * client, address * address)
    {
        auto error = mbedtls_net_accept(&mNet, &client->mNet,
            reinterpret_cast<void *>(&address->data),
            sizeof(address->data),
            &address->size);
        if (0 != error) {
            throw tls::exception(error);
        }
    }

    void connect(const char * ip, const char *port, protocol proto)
    {
        int error = mbedtls_net_connect(&mNet, ip, port, static_cast<int>(proto));
        if (0 != error) {
            throw tls::exception(error);
        }
    }
};

}
