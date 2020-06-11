#include "OnlineContext.hpp"

#include <luna/Plugin.hpp>
#include <luna/NetworkService.hpp>

namespace luna
{
    OnlineContext::OnlineContext() :
        mTaskHandle(nullptr)
    {}

    void OnlineContext::task(void * data)
    {
        static_cast<OnlineContext *>(data)->mIoContext.run();
        TaskHandle_t taskToNotify;
        if (xTaskNotifyWait(~0, 0, (uint32_t*)&taskToNotify, portMAX_DELAY)) {
            xTaskNotifyGive(taskToNotify);
        }
    }

    void OnlineContext::start()
    {
        xTaskCreatePinnedToCore(&task, "Asio", 1024 * 8, this, 5, &mTaskHandle, 1);
    }

    OnlineContext::~OnlineContext()
    {
        if (!mTaskHandle) return;
        xTaskNotify(mTaskHandle, (uint32_t)xTaskGetCurrentTaskHandle(), eSetValueWithOverwrite);
        mIoContext.stop();
        ulTaskNotifyTake(0, portMAX_DELAY);
    }
}
