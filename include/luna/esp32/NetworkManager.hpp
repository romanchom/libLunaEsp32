#pragma once

#include "mbedtls-cpp/PrivateKey.hpp"
#include "mbedtls-cpp/Certificate.hpp"
#include "mbedtls-cpp/Configuration.hpp"
#include "mbedtls-cpp/CounterDeterministicRandomGenerator.hpp"
#include "mbedtls-cpp/StandardEntropy.hpp"

#include "HardwareController.hpp"
#include "RealtimeController.hpp"

#include <asio/io_service.hpp>

#include <memory>
#include <vector>

namespace luna {
namespace proto {
    struct SetColor;
}

namespace esp32 {

class DiscoveryResponder;

struct NetworkManagerConfiguration
{
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

    void daemonTask();

    HardwareController * mController;

    tls::PrivateKey mOwnKey;
    tls::Certificate::Pem mOwnCertificate;
    tls::Certificate::Pem mCaCertificate;
    tls::StandardEntropy mEntropy;
    tls::CounterDeterministicRandomGenerator mRandom;
    tls::Configuration mUpdaterConfiguration;

    std::unique_ptr<RealtimeController> mRealtime;

    TaskHandle_t mTaskHandle;
    asio::io_service * mIoService;
};

}
}
