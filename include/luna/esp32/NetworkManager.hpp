#pragma once

#include "mbedtls-cpp/PrivateKey.hpp"
#include "mbedtls-cpp/Certificate.hpp"
#include "mbedtls-cpp/Configuration.hpp"
#include "mbedtls-cpp/CounterDeterministicRandomGenerator.hpp"
#include "mbedtls-cpp/StandardEntropy.hpp"
#include "mbedtls-cpp/StandardCookie.hpp"

#include "HardwareController.hpp"

#include <asio/io_context.hpp>

#include <memory>
#include <vector>

namespace luna::esp32 {

class DiscoveryResponder;

struct NetworkManagerConfiguration
{
    std::string name;
    std::string mqttAddress;
    uint8_t const * ownKey;
    size_t ownKeySize;
    uint8_t const * ownCertificate;
    size_t ownCertificateSize;
    uint8_t const * caCertificate;
    size_t caCertificateSize;
};

class NetworkManager
{
public:
    explicit NetworkManager(NetworkManagerConfiguration const & configuration, HardwareController * controller);
    ~NetworkManager();

    void enable();
    void disable();
private:

    void startDaemon();
    void stopDaemon();

    static void daemonTask(void * context);
    void run();

    HardwareController * mController;

    std::string mName;
    std::string mMqttAddress;

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
