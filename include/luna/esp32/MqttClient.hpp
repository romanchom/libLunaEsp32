#pragma once

#include <mqtt_client.h>

#include <map>
#include <string>
#include <functional>

namespace luna::esp32
{
    struct MqttClient
    {
        using Callback = std::function<void(void *, size_t)>;
        MqttClient(std::string const & broker);
        void subscribe(std::string const & topic, Callback callback);
        void connect();
    private:
        static void handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData);
        void handle(esp_mqtt_event_handle_t event);

        esp_mqtt_client_handle_t mMqttHandle;
        std::map<std::string, Callback, std::less<>> mSubscriptions;
    };
}