#include "luna/esp32/NetworkManager.hpp"

#include "DiscoveryResponder.hpp"
#include "luna/esp32/Updater.hpp"
#include "luna/esp32/ServiceManager.hpp"
#include "luna/esp32/RealtimeService.hpp"
#include "luna/esp32/MqttService.hpp"

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
    xTaskCreatePinnedToCore(&NetworkManager::daemonTask, "Daemon", 1024 * 8, this, 5, &mTaskHandle, 1);
}

void NetworkManager::stopDaemon()
{
    mIoService->stop();
    mTaskHandle = 0;
}

void NetworkManager::daemonTask(void * context) {
    static_cast<NetworkManager *>(context)->run();
    vTaskDelete(nullptr);
}

void NetworkManager::run()
{
    for (;;) {
        try {
            asio::io_context ioContext;
            mIoService = &ioContext;
            
            Updater updater(ioContext, &mUpdaterConfiguration);
            RealtimeService realtime(&ioContext, &mRealtimeConfiguration);
            DiscoveryResponder discoveryResponder(ioContext, realtime.port(), "Loszek", mController->strands());
            MqttService mqtt(&ioContext);

            ServiceManager serviceManager(mController);
            serviceManager.manage(&realtime, 10);
            serviceManager.manage(&mqtt, 1);

            mqtt.start();
            ioContext.run();
            break;
        } catch (std::exception const & exception) {
            std::cout << "Exception in NetworkManager: " << exception.what() << std::endl;
        }
    }
}

}
