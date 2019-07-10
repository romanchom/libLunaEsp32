#include "IdleService.hpp"

#include <luna/ConstantGenerator.hpp>

#include <esp_log.h>

static char const TAG[] = "Idle";

namespace luna
{
    void IdleService::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "On");
        ConstantGenerator generator;
        generator.color({0, 0, 0, 0});
        for (auto strand : controller->strands()) {
            strand->fill(&generator);
        }
        controller->update();
        controller->enabled(false);
    }

    void IdleService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Off");
    }
}
