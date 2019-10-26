#pragma once

#include "Topic.hpp"

#include <luna/NetworkManagerConfiguration.hpp>

#include <mqtt_client.h>

#include <vector>
#include <string_view>
#include <functional>

namespace luna::mqtt
{
    struct Client
    {
        using Callback = std::function<void(Topic const & topic, std::string_view)>;
        explicit Client(NetworkManagerConfiguration const & configuration);
        void subscribe(Topic topic, Callback callback);
        void connect();
    private:
        static void handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData);
        void handle(esp_mqtt_event_handle_t event);

        struct Record {
            Topic topic;
            Callback callback;
        };

        esp_mqtt_client_handle_t mHandle;
        std::vector<Record> mSubscriptions;
    };
}
