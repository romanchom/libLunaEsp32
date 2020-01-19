#pragma once

#include "Topic.hpp"
#include "Subscription.hpp"

#include <mqtt_client.h>

#include <vector>
#include <string_view>
#include <memory>

namespace luna::mqtt
{
    struct Client
    {
        struct Configuration
        {
            std::string_view mqttAddress;
            std::string_view ownKey;
            std::string_view ownCertificate;
            std::string_view caCertificate;
        };

        explicit Client(Configuration const & configuration);
        void subscribe(std::unique_ptr<Subscription> subscription);
        void publish(Topic topic, std::string const & text);
        void connect();
    private:
        static void handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData);
        void handle(esp_mqtt_event_handle_t event);

        esp_mqtt_client_handle_t mHandle;
        std::vector<std::unique_ptr<Subscription>> mSubscriptions;
    };
}
