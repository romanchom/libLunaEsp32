#include "DirectService.hpp"

#include <esp_log.h>

static char const TAG[] = "DirectSvc";

namespace luna
{
    DirectService::DirectService() :
        mController(nullptr)
    {}

    void DirectService::setColor(luna::proto::SetColor const& cmd)
    {
        if (!mController) {
            return;
        }

        auto strands = mController->strands();

        for (auto & strandData : cmd.strands) {
            size_t index = strandData.id;
            if (index >= strands.size()) continue;
            auto strand = strands[index];

            strand->rawBytes(strandData.rawBytes.data(), strandData.rawBytes.size());
        }

        mController->update();
    }

    void DirectService::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        mController = controller;
        mController->enabled(true);
    }

    void DirectService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        mController = nullptr;
    }

}
