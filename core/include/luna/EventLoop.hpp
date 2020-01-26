#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <functional>

namespace luna
{
    struct EventLoop
    {
        using Event = std::function<void()>;

        explicit EventLoop();
        ~EventLoop();
        void execute();
        void exit();
        void post(Event && event);
        TaskHandle_t taskHandle() { return mTaskHandle; }
    private:
        TaskHandle_t mTaskHandle;
        QueueHandle_t mMessageQueue;
        bool mShouldRun;
    };
}
