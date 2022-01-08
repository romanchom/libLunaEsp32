#include "Client.hpp"

#include "Controller.hpp"

#include <luna/TlsCredentials.hpp>

#include <algorithm>
#include <esp_log.h>

namespace luna::mqtt
{
    Client::Client(std::string_view address, TlsCredentials const & credentials, Controller * controller) :
        mConnected(false),
        mController(controller)
    {
        esp_mqtt_client_config_t mqtt_cfg = {
            .uri = address.data(),
        };

        if (address.rfind("mqtts", 0) == 0) {
            mqtt_cfg.cert_pem = credentials.caCertificate.data();
            mqtt_cfg.client_cert_pem = credentials.ownCertificate.data();
            mqtt_cfg.client_key_pem = credentials.ownKey.data();
        }

        mHandle = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(mHandle, (esp_mqtt_event_id_t) ESP_EVENT_ANY_ID, &Client::handler, this);
    }

    void Client::publish(std::string const & topic, std::string_view text)
    {
        if (mConnected) {
            esp_mqtt_client_publish(mHandle, topic.c_str(), text.data(), text.size(), 0, 1);
        }
    }

    void Client::subscribe(std::string const & topic)
    {
        esp_mqtt_client_subscribe(mHandle, topic.c_str(), 0);
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
            mController->subscribe();
            break;
        case MQTT_EVENT_DISCONNECTED:
            mConnected = false;
            break;
        case MQTT_EVENT_DATA:
            {
                std::string_view topic(event->topic, event->topic_len);
                std::string_view data(event->data, event->data_len);

                mController->deliver(topic, data);
            }
            break;
        default:
            break;
        }
    }
}
