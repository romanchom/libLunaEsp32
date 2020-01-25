#include "Nvs.hpp"

#include <nvs_flash.h>

namespace luna
{
    void Nvs::init() {
        static Nvs instance;
    }
    
    Nvs::Nvs()
    {
        esp_err_t ret = nvs_flash_init();

        if (ESP_ERR_NVS_NO_FREE_PAGES == ret) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }

        ESP_ERROR_CHECK(ret);
    }
}
        // auto error = nvs_open(mName.c_str(), NVS_READWRITE, &mHandle);
        // if (error != ESP_OK) {
        //     throw std::runtime_error("Unable to open NVS.");
        // }
