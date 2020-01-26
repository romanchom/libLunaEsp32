#include "IdleService.hpp"

#include "Device.hpp"
#include "Strand.hpp"
#include "Generator.hpp"

#include <esp_log.h>


namespace luna
{
    namespace
    {
        static char const TAG[] = "Idle";

        struct NullGenerator : Generator
        {
            void location(Location const & location) final {}

            prism::CieXYZ generate(float ratio) const noexcept final
            {
                return {0.0f, 0.0f, 0.0f, 0.0f};
            }
        };
    }

    IdleService::IdleService()
    {
        serviceEnabled(true);
    }

    void IdleService::takeOwnership(Device * device)
    {
        ESP_LOGI(TAG, "On");
        NullGenerator generator;
        for (auto strand : device->strands()) {
            strand->acceptGenerator(&generator);
        }
        device->update();
        device->enabled(false);
    }

    void IdleService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Off");
    }
}
