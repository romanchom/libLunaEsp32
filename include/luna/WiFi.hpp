#pragma once

#include <esp_event.h>

#include <string_view>
#include <string>

namespace luna
{
    struct WiFi
    {
        struct Observer
        {
            virtual void connected() = 0;
            virtual void disconnected() = 0;
        protected:
            ~Observer() = default;
        };

        explicit WiFi(std::string const & name, std::string_view ssid, std::string_view password);
        void observer(Observer * value);
        void enabled(bool value);

    private:
        static void eventHandler(void * context, esp_event_base_t eventBase, int32_t eventId, void * eventData);
        void handleEvent(esp_event_base_t event_base, int32_t event_id, void* event_data);

        Observer * mObserver;
    };
}
