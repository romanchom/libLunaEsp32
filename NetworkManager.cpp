#include "NetworkManager.hpp"

#include "DiscoveryResponder.hpp"
#include "Updater.hpp"
#include "IdleService.hpp"
#include "RealtimeService.hpp"

#include <luna/ServiceManager.hpp>
#include <luna/MqttService.hpp>
#include <luna/FlameMqttEffect.hpp>
#include <luna/ConstantMqttEffect.hpp>
#include <luna/PlasmaMqttEffect.hpp>

#include <esp_log.h>

#include <cstring>

static char const TAG[] = "Luna";

namespace luna
{
    NetworkManager::NetworkManager(NetworkManagerConfiguration const & configuration, HardwareController * controller) :
        mController(controller),
        mConfiguration(configuration),
        mOwnKey(configuration.ownKey, configuration.ownKeySize),
        mOwnCertificate(configuration.ownCertificate, configuration.ownCertificateSize),
        mCaCertificate(configuration.caCertificate, configuration.caCertificateSize),
        mTaskHandle(0),
        mIoService(nullptr)
    {
        mRandom.seed(&mEntropy, mConfiguration.name.data(), mConfiguration.name.size());

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
        xTaskCreatePinnedToCore(&NetworkManager::daemonTask, "Daemon", 1024 * 8, this, 5, &mTaskHandle, 1);
    }

    void NetworkManager::disable()
    {
        if (mIoService) {
            mIoService->stop();
            mTaskHandle = 0;
        }
    }

    void NetworkManager::daemonTask(void * context)
    {
        static_cast<NetworkManager *>(context)->run();
        vTaskDelete(nullptr);
    }

    void NetworkManager::run()
    {
        for (;;) {
            try {
                asio::io_context ioContext;
                mIoService = &ioContext;

                Updater updater(&ioContext, &mUpdaterConfiguration);
                RealtimeService realtime(&ioContext, &mRealtimeConfiguration);
                DiscoveryResponder discoveryResponder(&ioContext, realtime.port(), mConfiguration.name, mController->strands());

                IdleService idle;
                MqttService mqtt(&ioContext, mConfiguration);
                ConstantMqttEffect lightEffect(&mqtt, "light");
                FlameMqttEffect flameEffect(&mqtt, "flame");
                PlasmaMqttEffect plasmaEffect(&mqtt, "plasma");

                ServiceManager serviceManager(mController);
                serviceManager.manage(&realtime, 10);
                serviceManager.manage(&mqtt, 1);
                serviceManager.manage(&idle, 0, true);

                mqtt.start();
                ioContext.run();
                break;
            } catch (std::exception const & exception) {
                ESP_LOGE(TAG, "Exception in NetworkManager: %s", exception.what());
            }
        }
    }
}
