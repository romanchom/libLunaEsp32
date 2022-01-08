#pragma once

#include <mqtt_client.h>

#include <vector>
#include <string_view>
#include <memory>

namespace luna {
    struct TlsCredentials;
}

namespace luna::mqtt
{
    struct Controller;

    struct Client
    {
        explicit Client(std::string_view address, TlsCredentials const & credentials, Controller * controller);
        void publish(std::string const & topic, std::string_view text);
        void subscribe(std::string const& topic);
        void connect();
    private:
        static void handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData);
        void handle(esp_mqtt_event_handle_t event);

        esp_mqtt_client_handle_t mHandle;
        bool mConnected;
        Controller * mController;
    };
}
