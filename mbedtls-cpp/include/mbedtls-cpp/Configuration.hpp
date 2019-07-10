#pragma once

#include <stdexcept>
#include <cstdint>

#include <mbedtls/ssl.h>
#include <mbedtls/debug.h>

#include "Certificate.hpp"
#include "PrivateKey.hpp"
#include "Cookie.hpp"
#include "RandomGenerator.hpp"
#include "Exception.hpp"

namespace tls {
    enum class Endpoint : int {
        client = MBEDTLS_SSL_IS_CLIENT,
        server = MBEDTLS_SSL_IS_SERVER,
    };

    enum class Transport : int {
        stream = MBEDTLS_SSL_TRANSPORT_STREAM,
        datagram  = MBEDTLS_SSL_TRANSPORT_DATAGRAM,
    };

    enum class Preset : int {
        default_ = MBEDTLS_SSL_PRESET_DEFAULT,
        suiteb = MBEDTLS_SSL_PRESET_SUITEB,
    };

    enum class AuthenticationMode : int {
        none = MBEDTLS_SSL_VERIFY_NONE,
        optional = MBEDTLS_SSL_VERIFY_OPTIONAL,
        required = MBEDTLS_SSL_VERIFY_REQUIRED,
    };

    class Configuration {
    public:
        using DebugCallback = void (void *, int, const char *, int, const char *);
        using CertificateVerificationCallback = int (void *, mbedtls_x509_crt *, int, uint32_t *);
    private:
        mbedtls_ssl_config mConfiguration;
    public:
        explicit Configuration()
        {
            mbedtls_ssl_config_init(&mConfiguration);
        }

        ~Configuration()
        {
            mbedtls_ssl_config_free(&mConfiguration);
        }

        mbedtls_ssl_config * get()
        {
            return &mConfiguration;
        }

        void setDefaults(Endpoint ep, Transport tp, Preset p)
        {
            auto error = mbedtls_ssl_config_defaults(
                &mConfiguration,
                static_cast<int>(ep),
                static_cast<int>(tp),
                static_cast<int>(p));

            if (0 != error) {
                throw tls::Exception(error);
            }
        }

        void setRandomGenerator(RandomGenerator * rg)
        {
            mbedtls_ssl_conf_rng(&mConfiguration, rg->getCallback(), rg->getContext());
        }

        void setCertifiateAuthorityChain(Certificate * cert)
        {
            mbedtls_ssl_conf_ca_chain(&mConfiguration, cert->get(), nullptr);
        }

        void setOwnCertificate(Certificate * cert, PrivateKey * key)
        {
            auto error = mbedtls_ssl_conf_own_cert(&mConfiguration, cert->get(), key->get());
            if (0 != error) {
                throw tls::Exception(error);
            }
        }

        void setDtlsCookies(Cookie * cookie)
        {
            mbedtls_ssl_conf_dtls_cookies(&mConfiguration, cookie->getWriter(), cookie->getChecker(), cookie->getContext());
        }

        void setAuthenticationMode(AuthenticationMode mode)
        {
            mbedtls_ssl_conf_authmode(&mConfiguration,
                static_cast<int>(mode));
        }

        void enableDebug(DebugCallback * callback, int debugThreshold)
        {
            mbedtls_ssl_conf_dbg(&mConfiguration, callback, NULL);
            mbedtls_debug_set_threshold(debugThreshold);
        }

        void setSharedKey(uint8_t const * sharedKey, size_t sharedKeyLength, uint8_t const * identity, size_t identityLength)
        {
            int error = mbedtls_ssl_conf_psk(&mConfiguration, sharedKey, sharedKeyLength, identity, identityLength);
            if (0 != error) {
                throw tls::Exception(error);
            }
        }

        void setCipherSuites(int const * cipher_suites)
        {
            mbedtls_ssl_conf_ciphersuites(&mConfiguration, cipher_suites);
        }

        void setCertificateVerificationCallback(CertificateVerificationCallback * callback)
        {
            mbedtls_ssl_conf_verify(&mConfiguration, callback, nullptr);
        }
    };
}
