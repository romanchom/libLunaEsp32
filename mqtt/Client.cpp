#include "Client.hpp"

#include <algorithm>
#include <esp_log.h>

namespace luna::mqtt
{
    Client::Client(Configuration const & configuration)
    {
        esp_mqtt_client_config_t mqtt_cfg = {
            .uri = configuration.mqttAddress.data(),
            .cert_pem = configuration.caCertificate.data(),
            .client_cert_pem = configuration.ownCertificate.data(),
            .client_key_pem = configuration.ownKey.data(),
        };

        mHandle = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(mHandle, (esp_mqtt_event_id_t) ESP_EVENT_ANY_ID, &Client::handler, this);
    }

    void Client::subscribe(std::unique_ptr<Subscription> subscription)
    {
        mSubscriptions.emplace_back(std::move(subscription));
    }

    void Client::publish(Topic topic, std::string const & text)
    {
        esp_mqtt_client_publish(mHandle, topic.str().c_str(), text.c_str(), text.size(), 0, 1);
    }

    void Client::connect()
    {
        esp_mqtt_client_start(mHandle);
    }

    void Client::handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData)
    {
        static_cast<Client *>(context)->handle(static_cast<esp_mqtt_event_handle_t>(eventData));
    }

    void Client::handle(esp_mqtt_event_handle_t event)
    {
        switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            for (auto & subscription : mSubscriptions) {
                esp_mqtt_client_subscribe(mHandle, subscription->topic().str().c_str(), 0);
            }
            break;
        case MQTT_EVENT_DATA:
            {
                std::string topic(static_cast<char const *>(event->topic), event->topic_len);
                Topic mqttTopic(std::move(topic));

                // ESP_LOGI("MQTT" , "%.*s", event->topic_len, event->topic);
                auto subscription = std::find_if(mSubscriptions.begin(), mSubscriptions.end(), [mqttTopic](auto const & subscription) {
                    // ESP_LOGI("MQTT" , "%s", subscription->topic().str().c_str());
                    return subscription->topic().matches(mqttTopic);
                });
                // ESP_LOGI("MQTT" , "%d", subscription!=mSubscriptions.end());
                if (subscription != mSubscriptions.end()) {
                    std::string_view text(static_cast<char const *>(event->data), event->data_len);
                    (*subscription)->deliver(text);
                }
            }
            break;
        default:
            break;
        }
    }
}
