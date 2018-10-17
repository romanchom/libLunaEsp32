#pragma once

#include <cstdint>
#include <stdexcept>

#include <mbedtls/x509_crt.h>
#include <mbedtls/x509.h>
#include <mbedtls/certs.h>

#include "Exception.hpp"

namespace tls {
    class Certificate {
    private:
        mbedtls_x509_crt mCertificate;
        explicit Certificate()
        {
            mbedtls_x509_crt_init(&mCertificate);
        }

        void parse(uint8_t const * buffer, size_t bufferLength)
        {
            int error = mbedtls_x509_crt_parse(&mCertificate, buffer, bufferLength);
            if (0 != error) {
                throw tls::Exception(error);
            }
        }

        void parseFile(char const * fileName)
        {
            int error = mbedtls_x509_crt_parse_file(&mCertificate, fileName);
            if (0 != error) {
                throw tls::Exception(error);
            }
        }
    public:
        explicit Certificate(uint8_t const * buffer, size_t bufferLength) :
            Certificate()
        {
            parse(buffer, bufferLength);
        }

        explicit Certificate(char const * fileName) :
            Certificate()
        {
            parseFile(fileName);
        }

        ~Certificate()
        {
            mbedtls_x509_crt_free(&mCertificate);
        }

        mbedtls_x509_crt * get()
        {
            return &mCertificate;
        }

        Certificate * next()
        {
            return reinterpret_cast<Certificate *>(mCertificate.next);
        }
    };
}