#pragma once

#include <cstdint>

#include <mbedtls/pk.h>

#include "Exception.hpp"

namespace tls {

class PrivateKey {
private:
    mbedtls_pk_context mPrivateKey;
    explicit PrivateKey()
    {
        mbedtls_pk_init(&mPrivateKey);
    }

    void parse(const uint8_t * data, size_t dataLength)
    {
        int error = mbedtls_pk_parse_key(&mPrivateKey, data, dataLength, NULL, 0);
        if (0 != error) {
            throw tls::Exception(error);
        }
    }

    void parseFile(const char * fileName, const char * password)
    {
        int error = mbedtls_pk_parse_keyfile(&mPrivateKey, fileName, password);
        if (0 != error) {
            throw tls::Exception(error);
        }
    }
public:
    explicit PrivateKey(uint8_t const * buffer, size_t bufferLength) :
        PrivateKey()
    {
        parse(buffer, bufferLength);
    }

    explicit PrivateKey(char const * fileName, const char * password) :
        PrivateKey()
    {
        parseFile(fileName, password);
    }

    ~PrivateKey()
    {
        mbedtls_pk_free(&mPrivateKey);
    }

    mbedtls_pk_context * get()
    {
        return &mPrivateKey;
    }
};

}
