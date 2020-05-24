#pragma once

#include "IdleController.hpp"

#include <vector>

namespace luna
{
    struct Controller;
    struct Device;

    struct ControllerMux
    {
        explicit ControllerMux(Device * device);

        void add(Controller * controller);
        void setEnabled(Controller * controller, bool enabled);
    private:
        friend class Controller;

        void findActive();

        Device * mDevice;
        IdleController mIdleController;
        struct State
        {
            bool enabled;
            Controller * controller;
        };
        std::vector<State> mControllers;
        Controller * mActive;
    };
}
