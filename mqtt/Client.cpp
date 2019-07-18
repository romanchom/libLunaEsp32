#include "Client.hpp"

namespace luna::mqtt
{
    Client::Client(NetworkManagerConfiguration const & configuration)
    {
        esp_mqtt_client_config_t mqtt_cfg = {
            .uri = configuration.mqttAddress.data(),
            .cert_pem = configuration.tls.caCertificate.data(),
            .client_cert_pem = configuration.tls.ownCertificate.data(),
            .client_key_pem = configuration.tls.ownKey.data(),
        };

        mHandle = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(mHandle, (esp_mqtt_event_id_t) ESP_EVENT_ANY_ID, &Client::handler, this);
    }

    void Client::subscribe(std::string const & topic, Callback callback)
    {
        mSubscriptions[Topic(topic)] = std::move(callback);
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
            for (auto & [topic, callback] : mSubscriptions) {
                esp_mqtt_client_subscribe(mHandle, topic.str().c_str(), 0);
            }
            break;
        case MQTT_EVENT_DATA:
            {
                std::string topic(static_cast<char const *>(event->topic), event->topic_len);
                Topic mqttTopic(topic);

                if (auto subscription = mSubscriptions.find(mqttTopic); subscription != mSubscriptions.end()) {
                    std::string_view text(static_cast<char const *>(event->data), event->data_len);
                    subscription->second(mqttTopic, text);
                }
            }
            break;
        default:
            break;
        }
    }
}
