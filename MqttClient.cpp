#include "luna/esp32/MqttClient.hpp"

namespace luna::esp32
{
    MqttClient::MqttClient(std::string const & broker)
    {
        esp_mqtt_client_config_t mqtt_cfg = {
            .uri = broker.c_str(),
        };

        mMqttHandle = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(mMqttHandle, (esp_mqtt_event_id_t) ESP_EVENT_ANY_ID, &MqttClient::handler, this);
    }

    void MqttClient::subscribe(std::string const & topic, Callback callback)
    {
        mSubscriptions[topic] = std::move(callback);
    }

    void MqttClient::connect()
    {
        esp_mqtt_client_start(mMqttHandle);
    }

    void MqttClient::handler(void * context, esp_event_base_t base, int32_t eventId, void * eventData)
    {
        static_cast<MqttClient *>(context)->handle(static_cast<esp_mqtt_event_handle_t>(eventData));
    }

    void MqttClient::handle(esp_mqtt_event_handle_t event)
    {
        switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            for (auto & [topic, callback] : mSubscriptions) {
                esp_mqtt_client_subscribe(mMqttHandle, topic.c_str(), 0);
            }
            break;
        case MQTT_EVENT_DATA:
            {
                std::string_view topic(static_cast<char const *>(event->topic), event->topic_len);
                if (auto subscription = mSubscriptions.find(topic); subscription != mSubscriptions.end()) {
                    subscription->second(event->data, event->data_len);
                }
            }
            break;
        default:
            break;
        }
    }
}