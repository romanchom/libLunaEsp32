#pragma once

#include "lwip_async/DtlsInputOutput.hpp"

#include "mbedtls-cpp/PrivateKey.hpp"
#include "mbedtls-cpp/Certificate.hpp"

#include "Strand.hpp"
#include "HardwareController.hpp"
#include "Updater.hpp"

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
    void dispatchCommand(uint8_t const * data, size_t size);

    void setColor(luna::proto::SetColor const& cmd);

    HardwareController * mController;

    tls::PrivateKey mOwnKey;
    tls::Certificate::Pem mOwnCertificate;
    tls::Certificate::Pem mCaCertificate;

    std::unique_ptr<lwip_async::DtlsInputOutput> mSocket;
    std::unique_ptr<luna::esp32::DiscoveryResponder> mDiscoveryResponder;
    Updater mUpdater;
};

}
}
