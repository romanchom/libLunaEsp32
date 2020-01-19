#include "OnlineContext.hpp"

#include "TlsConfiguration.hpp"

#include <luna/HardwareController.hpp>

namespace
{
    void task(void * data)
    {
        static_cast<asio::io_context *>(data)->run();
        std::terminate();
    }
}

namespace luna
{
    OnlineContext::OnlineContext(Configuration::Network const & config, TlsConfiguration * tlsConfigutation, HardwareController * controller, EffectEngine * effectEngine, DirectService * directService) :
        mRealtimeService(&mIoContext, tlsConfigutation->realtimeConfiguration(), directService),
        mMqtt(
            effectEngine,
            std::string(config.name),
            {
                config.mqttAddress,
                config.ownKey,
                config.ownCertificate,
                config.caCertificate
            }
        ),
        mDiscoveryResponder(&mIoContext, mRealtimeService.port(), config.name, controller->strands()),
        mUpdater(&mIoContext, tlsConfigutation->updaterConfiguration())
    {
        xTaskCreatePinnedToCore(&task, "Daemon", 1024 * 8, &mIoContext, 5, &mTaskHandle, 1);
    }

    OnlineContext::~OnlineContext() = default;
}
