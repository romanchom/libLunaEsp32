#pragma once

#include "Controller.hpp"

namespace luna
{
    struct IdleController : Controller
    {
        virtual void takeOwnership(Device * device) override;
        virtual void releaseOwnership() override;
    };
}
