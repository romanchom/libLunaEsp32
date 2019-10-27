#pragma once

#include <luna/Service.hpp>
#include <luna/proto/SetColor.hpp>

namespace luna
{
    struct HardwareController;

    struct DirectService : Service
    {
        explicit DirectService();
        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;
        void setColor(luna::proto::SetColor const& cmd);
    private:
        HardwareController * mController;
    };

}
