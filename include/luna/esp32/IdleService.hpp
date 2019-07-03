#pragma once

#include "Service.hpp"

namespace luna::esp32
{
    struct IdleService : Service
    {
        virtual void takeOwnership(HardwareController * controller) override;
        virtual void releaseOwnership() override;
    };
}
