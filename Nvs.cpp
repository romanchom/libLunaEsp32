#include "Nvs.hpp"

#include <nvs_flash.h>

namespace luna
{
    static bool initialized = false;

    void Nvs::init() {
        if (initialized) return;
        initialized = true;

        esp_err_t ret = nvs_flash_init();

        if (ESP_ERR_NVS_NO_FREE_PAGES == ret) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }

        ESP_ERROR_CHECK(ret);
    }
}
