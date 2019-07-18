#pragma once

#include "Exception.hpp"

#include <mbedtls/x509_crt.h>
#include <mbedtls/x509.h>
#include <mbedtls/certs.h>

#include <cstdint>
#include <string_view>

namespace tls {
    class Certificate {
    private:
        mbedtls_x509_crt mCertificate;
    protected:
        explicit Certificate()
        {
            mbedtls_x509_crt_init(&mCertificate);
        }

    public:
        struct Der;
        struct Pem;

        ~Certificate()
        {
            mbedtls_x509_crt_free(&mCertificate);
        }

        Certificate(Certificate &) = delete;

        mbedtls_x509_crt * get()
        {
            return &mCertificate;
        }

        Certificate * next()
        {
            return reinterpret_cast<Certificate *>(mCertificate.next);
        }
    };

    struct Certificate::Der : Certificate
    {
        explicit Der(std::string_view certificate) :
            Certificate()
        {
            int error = mbedtls_x509_crt_parse_der(&mCertificate, reinterpret_cast<unsigned char const *>(certificate.data()), certificate.size());
            if (0 != error) {
                throw tls::Exception(error);
            }
        }
    };

    struct Certificate::Pem : Certificate
    {
        explicit Pem(std::string_view certificate) :
            Certificate()
        {
            int error = mbedtls_x509_crt_parse(&mCertificate, reinterpret_cast<unsigned char const *>(certificate.data()), certificate.size());
            if (0 != error) {
                throw tls::Exception(error);
            }
        }
    };
}
