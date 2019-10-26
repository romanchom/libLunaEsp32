#pragma once

#include <esp_event.h>

#include <string_view>

namespace luna
{
    struct WiFi
    {
        struct Observer
        {
            virtual ~Observer() = default;
            virtual void connected() = 0;
            virtual void disconnected() = 0;
        };

        explicit WiFi(std::string_view ssid, std::string_view password);
        void observer(Observer * value);
        void enabled(bool value);
    private:
        Observer * mObserver;
        static void eventHandler(void * context, esp_event_base_t eventBase, int32_t eventId, void * eventData);
        void handleEvent(esp_event_base_t event_base, int32_t event_id, void* event_data);
    };
}
