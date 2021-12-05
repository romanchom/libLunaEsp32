#pragma once

#include "Exception.hpp"

#include <mbedtls/pk.h>

#include <cstdint>
#include <string_view>

namespace tls {

class PrivateKey {
private:
    mbedtls_pk_context mPrivateKey;
    explicit PrivateKey()
    {
        mbedtls_pk_init(&mPrivateKey);
    }

    void parse(std::string_view key)
    {
        int error = mbedtls_pk_parse_key(&mPrivateKey, reinterpret_cast<unsigned char const *>(key.data()), key.size(), NULL, 0);
        if (0 != error) {
            std::terminate();
            // throw tls::Exception(error);
        }
    }

    void parseFile(const char * fileName, const char * password)
    {
        int error = mbedtls_pk_parse_keyfile(&mPrivateKey, fileName, password);
        if (0 != error) {
            std::terminate();
            // throw tls::Exception(error);
        }
    }
public:
    explicit PrivateKey(std::string_view key) :
        PrivateKey()
    {
        parse(key);
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
