#include "DirectService.hpp"

#include <luna/Device.hpp>

#include <esp_log.h>

static char const TAG[] = "DirectSvc";

namespace luna
{
    DirectService::DirectService() :
        mDevice(nullptr)
    {}

    void DirectService::setColor(luna::proto::SetColor const& cmd)
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

    void DirectService::takeOwnership(Device * device)
    {
        ESP_LOGI(TAG, "Enabled");
        mDevice = device;
        mDevice->enabled(true);
    }

    void DirectService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        mDevice = nullptr;
    }

}
