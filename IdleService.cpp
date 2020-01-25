#include "IdleService.hpp"

#include <luna/ConstantGenerator.hpp>
#include <luna/HardwareController.hpp>

#include <esp_log.h>

static char const TAG[] = "Idle";

namespace luna
{
    IdleService::IdleService()
    {
        serviceEnabled(true);
    }

    void IdleService::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "On");
        ConstantGenerator generator({0, 0, 0, 0});
        for (auto strand : controller->strands()) {
            strand->acceptGenerator(&generator);
        }
        controller->update();
        controller->enabled(false);
    }

    void IdleService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Off");
    }
}
