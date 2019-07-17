#pragma once

#include "Topic.hpp"

#include <luna/NetworkManagerConfiguration.hpp>

#include <mqtt_client.h>

#include <map>
#include <string>
#include <functional>

namespace luna::mqtt
{
    struct Client
    {
        using Callback = std::function<void(Topic const & topic, std::string_view)>;
        explicit Client(NetworkManagerConfiguration const & configuration);
        void subscribe(std::string const & topic, Callback callback);
        void connect();
    private:
        static void handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData);
        void handle(esp_mqtt_event_handle_t event);

        esp_mqtt_client_handle_t mHandle;
        std::map<Topic, Callback> mSubscriptions;
    };
}
