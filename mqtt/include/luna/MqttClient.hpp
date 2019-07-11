#pragma once

#include "MqttTopic.hpp"

#include <mqtt_client.h>

#include <map>
#include <string>
#include <functional>

namespace luna
{
    struct MqttClient
    {
        using Callback = std::function<void(MqttTopic const & topic, std::string_view)>;
        explicit MqttClient(std::string const & broker);
        void subscribe(std::string const & topic, Callback callback);
        void connect();
    private:
        static void handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData);
        void handle(esp_mqtt_event_handle_t event);

        esp_mqtt_client_handle_t mMqttHandle;
        std::map<MqttTopic, Callback> mSubscriptions;
    };
}
