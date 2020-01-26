#include "DirectController.hpp"

#include <luna/Device.hpp>
#include <luna/Strand.hpp>

#include <esp_log.h>

static char const TAG[] = "DirectSvc";

namespace luna
{
    DirectController::DirectController() :
        mDevice(nullptr)
    {}

    void DirectController::setColor(luna::proto::SetColor const& cmd)
    {
        if (!mDevice) {
            return;
        }

        auto strands = mDevice->strands();

        for (auto & strandData : cmd.strands) {
            size_t index = strandData.id;
            if (index >= strands.size()) continue;
            auto strand = strands[index];

            strand->rawBytes(strandData.rawBytes.data(), strandData.rawBytes.size());
        }

        mDevice->update();
    }

    void DirectController::takeOwnership(Device * device)
    {
        ESP_LOGI(TAG, "Enabled");
        mDevice = device;
        mDevice->enabled(true);
    }

    void DirectController::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        mDevice = nullptr;
    }

}
