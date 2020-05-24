#include "OnlineContext.hpp"

#include <luna/NetworkingContext.hpp>
#include <luna/Plugin.hpp>
#include <luna/NetworkService.hpp>

namespace luna
{
    OnlineContext::OnlineContext(LunaContext const & context, TlsCredentials const & credentials, std::vector<Plugin *> plugins) :
        mTlsConfiguration(credentials)
    {
        NetworkingContext network{
            .tlsCredentials = credentials,
            .tlsConfiguration = &mTlsConfiguration,
            .ioContext = &mIoContext,
        };

        for (auto plugin : plugins) {
            if (auto service = plugin->makeNetworkService(context, network)) {
                mServices.emplace_back(std::move(service));
            }
        }

        xTaskCreatePinnedToCore(&task, "Asio", 1024 * 8, this, 5, &mTaskHandle, 1);
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
