#pragma once

namespace luna
{
    struct Device;

    struct Controller
    {
        virtual void takeOwnership(Device * device) = 0;
        virtual void releaseOwnership() = 0;

    protected:
        ~Controller() = default;
    };
}
