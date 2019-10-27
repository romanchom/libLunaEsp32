#pragma once

#include <luna/Service.hpp>

namespace luna
{
    struct IdleService : Service
    {
        explicit IdleService();
        virtual void takeOwnership(HardwareController * controller) override;
        virtual void releaseOwnership() override;
    };
}
