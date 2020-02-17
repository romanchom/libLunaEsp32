#include "EventLoop.hpp"

#include <esp_log.h>

namespace
{
    auto const TAG = "EV";
}

namespace luna
{
    EventLoop::EventLoop() :
        mMessageQueue(xQueueCreate(8, sizeof(Event)))
    {}

    EventLoop::~EventLoop()
    {
        vQueueDelete(mMessageQueue);
    }

    void EventLoop::execute()
    {
        mTaskHandle = xTaskGetCurrentTaskHandle();
        mShouldRun = true;
        while (mShouldRun) {
            char buffer[sizeof(Event)];
            auto const error = xQueueReceive(mMessageQueue, buffer, portMAX_DELAY);
            if (error == pdTRUE) {
                auto event = reinterpret_cast<Event *>(buffer);
                (*event)();
                event->~Event();
            }
        }
    }

    void EventLoop::exit()
    {
        post([this]{
            mShouldRun = false;
        });
    }

    void EventLoop::post(Event && event)
    {
        char buffer[sizeof(Event)];
        auto const view = new (buffer) Event(std::move(event));
        auto const ret = xQueueSendToBack(mMessageQueue, static_cast<void *>(view), 100);
        if (ret != pdTRUE) {
            ESP_LOGE(TAG, "Cannot post");
        }
    }
}
