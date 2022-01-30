#include "WiFi.hpp"
#include "Nvs.hpp"

#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_netif.h>

#include <cstring>

namespace luna
{
    static auto const TAG = "WiFi";

    WiFi::WiFi(std::string const & name, std::string_view ssid, std::string_view password) :
        mObserver(nullptr)
    {
        Nvs::init();
#if ESP32
        ESP_ERROR_CHECK(esp_netif_init());
        esp_netif_create_default_wifi_sta();
#elif ESP8266
        tcpip_adapter_init();
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, name.c_str());
#endif
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));


#if ESP32
        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFi::eventHandler, this, &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFi::eventHandler, this, &instance_got_ip));
#elif ESP8266
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFi::eventHandler, this));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFi::eventHandler, this));
#endif

        static wifi_config_t wifi_config = {};
        memset(&wifi_config, 0, sizeof(wifi_config_t));
        memcpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.data(), ssid.size());
        memcpy(reinterpret_cast<char *>(wifi_config.sta.password), password.data(), password.size());

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
#if ESP32
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
#elif ESP8266
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
#endif
        // ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    }

    void WiFi::observer(Observer * value)
    {
        mObserver = value;
    }

    void WiFi::enabled(bool value)
    {
        ESP_LOGI(TAG, "Enabled %d", (int) value);
        if (value) {
            ESP_ERROR_CHECK(esp_wifi_start());
        } else {
            ESP_ERROR_CHECK(esp_wifi_stop());
        }
    }

    void WiFi::eventHandler(void * context, esp_event_base_t eventBase, int32_t eventId, void * eventData)
    {
        return static_cast<WiFi *>(context)->handleEvent(eventBase, eventId, eventData);
    }

    void WiFi::handleEvent(esp_event_base_t eventBase, int32_t eventId, void * eventData)
    {
        if (eventBase == WIFI_EVENT) {
            switch (eventId) {
            case WIFI_EVENT_STA_DISCONNECTED:
                if (mObserver) {
                    mObserver->disconnected();
                }
                // fallthrough
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            }
        } else if (eventBase == IP_EVENT) {
            if (eventId == IP_EVENT_STA_GOT_IP) {
                ESP_LOGI(TAG, "Got IP");
                if (mObserver) {
                    mObserver->connected();
                }
            }
        }
    }
}
