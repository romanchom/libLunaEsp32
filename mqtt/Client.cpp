#include "Client.hpp"

#include <luna/TlsCredentials.hpp>

#include <algorithm>
#include <esp_log.h>

namespace luna::mqtt
{
    Client::Client(std::string_view address, TlsCredentials const & credentials) :
        mConnected(false)
    {
        esp_mqtt_client_config_t mqtt_cfg = {
            .uri = address.data(),
        };

        if (address.find_first_of("mqtts") != address.npos) {
            mqtt_cfg.cert_pem = credentials.caCertificate.data();
            mqtt_cfg.client_cert_pem = credentials.ownCertificate.data();
            mqtt_cfg.client_key_pem = credentials.ownKey.data();
        }

        mHandle = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(mHandle, (esp_mqtt_event_id_t) ESP_EVENT_ANY_ID, &Client::handler, this);
    }

    void Client::subscribe(std::unique_ptr<Subscription> subscription)
    {
        mSubscriptions.emplace_back(std::move(subscription));
    }

    void Client::publish(Topic topic, std::string const & text)
    {
        if (mConnected) {
            esp_mqtt_client_publish(mHandle, topic.str().c_str(), text.c_str(), text.size(), 0, 1);
        }
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
            mConnected = true;
            for (auto & subscription : mSubscriptions) {
                esp_mqtt_client_subscribe(mHandle, subscription->topic().str().c_str(), 0);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            mConnected = false;
            break;
        case MQTT_EVENT_DATA:
            {
                std::string topic(static_cast<char const *>(event->topic), event->topic_len);
                Topic mqttTopic(std::move(topic));

                auto subscription = std::find_if(mSubscriptions.begin(), mSubscriptions.end(), [mqttTopic](auto const & subscription) {
                    return subscription->topic().matches(mqttTopic);
                });
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
