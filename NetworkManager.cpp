#include "luna/esp32/NetworkManager.hpp"

#include "luna/esp32/RealtimeController.hpp"
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

    mCookie.setup(&mRandom);

    mUpdaterConfiguration.setRandomGenerator(&mRandom);
    mUpdaterConfiguration.setDefaults(tls::Endpoint::server, tls::Transport::stream, tls::Preset::default_);
    mUpdaterConfiguration.setOwnCertificate(&mOwnCertificate, &mOwnKey);
    mUpdaterConfiguration.setCertifiateAuthorityChain(&mCaCertificate);
    mUpdaterConfiguration.setAuthenticationMode(tls::AuthenticationMode::required);

    mRealtimeConfiguration.setRandomGenerator(&mRandom);
    mRealtimeConfiguration.setDefaults(tls::Endpoint::server, tls::Transport::datagram, tls::Preset::default_);
    mRealtimeConfiguration.setOwnCertificate(&mOwnCertificate, &mOwnKey);
    mRealtimeConfiguration.setCertifiateAuthorityChain(&mCaCertificate);
    mRealtimeConfiguration.setAuthenticationMode(tls::AuthenticationMode::required);
    mRealtimeConfiguration.setDtlsCookies(&mCookie);
}

NetworkManager::~NetworkManager() = default;

void NetworkManager::enable()
{
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
    asio::io_context ioContext;
    mIoService = &ioContext;
    
    luna::esp32::Updater updater(ioContext, &mUpdaterConfiguration);
    luna::esp32::RealtimeController realtime(&ioContext, &mRealtimeConfiguration, mController);
    luna::esp32::DiscoveryResponder discoveryResponder(ioContext, realtime.port(), "Loszek", mController->strands());

    ioContext.run();

    vTaskDelete(nullptr);
}

}
