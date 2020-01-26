#pragma once

#include "IdleController.hpp"

#include <vector>

namespace luna
{
    struct Controller;
    struct Device;
    struct Plugin;
    struct EventLoop;

    struct ControllerMux
    {
        explicit ControllerMux(EventLoop * mainLoop, Device * device, std::vector<Plugin *> plugins);

        void enabled(Controller * controller, bool enabled);
    private:
        void findActive();

        Controller * maxEnabled();

        Device * mDevice;
        IdleController mIdleController;
        std::vector<Controller *> mControllers;
        Controller * mActive;
    };
}
