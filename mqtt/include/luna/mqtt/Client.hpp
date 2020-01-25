#pragma once

#include "Topic.hpp"
#include "Subscription.hpp"

#include <mqtt_client.h>

#include <vector>
#include <string_view>
#include <memory>

namespace luna {
    struct TlsCredentials;
}

namespace luna::mqtt
{
    struct Client
    {
        explicit Client(std::string_view address, TlsCredentials const & credentials);
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
