#include "OnlineContext.hpp"

#include "TlsConfiguration.hpp"

#include <luna/HardwareController.hpp>

namespace luna
{
    OnlineContext::OnlineContext(Configuration::Network const & config, TlsConfiguration * tlsConfigutation, HardwareController * controller, EffectEngine * effectEngine, DirectService * directService, EventLoop * mainLoop) :
        mRealtimeService(&mIoContext, tlsConfigutation->realtimeConfiguration(), directService),
        mMqtt(
            mainLoop,
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
        xTaskCreatePinnedToCore(&task, "Daemon", 1024 * 8, this, 5, &mTaskHandle, 1);
    }

    void OnlineContext::task(void * data)
    {
        static_cast<OnlineContext *>(data)->mIoContext.run();
        TaskHandle_t taskToNotify;
        if (xTaskNotifyWait(~0, 0, (uint32_t*)&taskToNotify, portMAX_DELAY)) {
            xTaskNotifyGive(taskToNotify);
        }
    }


    OnlineContext::~OnlineContext()
    {
        xTaskNotify(mTaskHandle, (uint32_t)xTaskGetCurrentTaskHandle(), eSetValueWithOverwrite);
        mIoContext.stop();
        ulTaskNotifyTake(0, portMAX_DELAY);
    }
}
