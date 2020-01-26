#pragma once

#include "Service.hpp"

namespace luna
{
    struct IdleService : Service
    {
        explicit IdleService();
        virtual void takeOwnership(Device * device) override;
        virtual void releaseOwnership() override;
    };
}
