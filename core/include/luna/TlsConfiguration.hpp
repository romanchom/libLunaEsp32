#pragma once

#include "TlsCredentials.hpp"

#include <mbedtls-cpp/PrivateKey.hpp>
#include <mbedtls-cpp/Certificate.hpp>
#include <mbedtls-cpp/Configuration.hpp>
#include <mbedtls-cpp/CounterDeterministicRandomGenerator.hpp>
#include <mbedtls-cpp/StandardEntropy.hpp>
#include <mbedtls-cpp/StandardCookie.hpp>

#include <memory>

namespace luna
{
    struct TlsConfiguration
    {
        explicit TlsConfiguration(TlsCredentials const & credentials);

        std::unique_ptr<tls::Configuration> makeTlsConfiguration();
        std::unique_ptr<tls::Configuration> makeDtlsConfiguration();
    private:
        std::unique_ptr<tls::Configuration> makeDefault();
        
        tls::PrivateKey mOwnKey;
        tls::Certificate::Pem mOwnCertificate;
        tls::Certificate::Pem mCaCertificate;
        tls::StandardEntropy mEntropy;
        tls::CounterDeterministicRandomGenerator mRandom;
        tls::StandardCookie mCookie;

        tls::Configuration mUpdaterConfiguration;
        tls::Configuration mRealtimeConfiguration;
    };
}
