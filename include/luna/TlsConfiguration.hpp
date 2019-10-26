#pragma once

#include <string_view>

#include <mbedtls-cpp/PrivateKey.hpp>
#include <mbedtls-cpp/Certificate.hpp>
#include <mbedtls-cpp/Configuration.hpp>
#include <mbedtls-cpp/CounterDeterministicRandomGenerator.hpp>
#include <mbedtls-cpp/StandardEntropy.hpp>
#include <mbedtls-cpp/StandardCookie.hpp>

namespace luna
{
    struct TlsConfiguration
    {
        explicit TlsConfiguration(std::string_view ownKey, std::string_view ownCertificate, std::string_view caCertificate);

        tls::Configuration * updaterConfiguration() noexcept { return &mUpdaterConfiguration; }
        tls::Configuration * realtimeConfiguration() noexcept { return &mRealtimeConfiguration; }
    private:
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
