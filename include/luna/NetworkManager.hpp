#pragma once

#include <mbedtls-cpp/PrivateKey.hpp>
#include <mbedtls-cpp/Certificate.hpp>
#include <mbedtls-cpp/Configuration.hpp>
#include <mbedtls-cpp/CounterDeterministicRandomGenerator.hpp>
#include <mbedtls-cpp/StandardEntropy.hpp>
#include <mbedtls-cpp/StandardCookie.hpp>

#include <luna/HardwareController.hpp>
#include <luna/NetworkManagerConfiguration.hpp>

#include <asio/io_context.hpp>

namespace luna
{
    struct DiscoveryResponder;

    struct NetworkManager
    {
    public:
        explicit NetworkManager(NetworkManagerConfiguration const & configuration, HardwareController * controller);
        ~NetworkManager();

        void enable();
        void disable();
    private:
        static void daemonTask(void * context);
        void run();

        HardwareController * mController;

        NetworkManagerConfiguration mConfiguration;

        tls::PrivateKey mOwnKey;
        tls::Certificate::Pem mOwnCertificate;
        tls::Certificate::Pem mCaCertificate;
        tls::StandardEntropy mEntropy;
        tls::CounterDeterministicRandomGenerator mRandom;

        tls::Configuration mUpdaterConfiguration;

        tls::StandardCookie mCookie;
        tls::Configuration mRealtimeConfiguration;

        TaskHandle_t mTaskHandle;
        asio::io_context * mIoService;
    };
}
