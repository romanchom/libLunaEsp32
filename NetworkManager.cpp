#include "luna/esp32/NetworkManager.hpp"

#include "DiscoveryResponder.hpp"
#include "luna/esp32/Updater.hpp"

#include <esp_log.h>

#include <cstring>

static char const TAG[] = "Luna";

namespace luna::esp32 {

NetworkManager::NetworkManager(NetworkManagerConfiguration const & configuration, HardwareController * controller) :
    mController(controller),
    mOwnKey(configuration.ownKey, configuration.ownKeySize),
    mOwnCertificate(configuration.ownCertificate, configuration.ownCertificateSize),
    mCaCertificate(configuration.caCertificate, configuration.caCertificateSize)
{
    mRandom.seed(&mEntropy, "asdqwe", 6);

    mUpdaterConfiguration.setRandomGenerator(&mRandom);
    mUpdaterConfiguration.setDefaults(tls::Endpoint::server, tls::Transport::stream, tls::Preset::default_);
    mUpdaterConfiguration.setOwnCertificate(&mOwnCertificate, &mOwnKey);
    mUpdaterConfiguration.setCertifiateAuthorityChain(&mCaCertificate);
    mUpdaterConfiguration.setAuthenticationMode(tls::AuthenticationMode::required);
}

NetworkManager::~NetworkManager() = default;

void NetworkManager::enable()
{
    mRealtime = std::make_unique<RealtimeController>(mController, &mOwnKey, &mOwnCertificate, &mCaCertificate);

    startDaemon();
}

void NetworkManager::disable()
{
    stopDaemon();
}

void NetworkManager::startDaemon()
{
    xTaskCreate((TaskFunction_t) &NetworkManager::daemonTask, "Daemon", 1024 * 8, this, 5, &mTaskHandle);
}

void NetworkManager::stopDaemon()
{
    mIoService->stop();
    mTaskHandle = 0;
}

void NetworkManager::daemonTask()
{
    asio::io_service ioService;
    mIoService = &ioService;
    
    luna::esp32::DiscoveryResponder discoveryResponder(ioService, mRealtime->port(), "Loszek", mController->strands());
    luna::esp32::Updater updater(ioService, &mUpdaterConfiguration);

    ioService.run();

    vTaskDelete(nullptr);
}

}
