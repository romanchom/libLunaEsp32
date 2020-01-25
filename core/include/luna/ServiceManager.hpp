#pragma once

#include "IdleService.hpp"

#include <vector>

namespace luna
{
    struct Service;
    struct HardwareController;
    struct Plugin;
    struct EventLoop;

    struct ServiceManager
    {
        explicit ServiceManager(EventLoop * mainLoop, HardwareController * controller, std::vector<Plugin *> plugins);

        void serviceEnabled(Service * service, bool enabled);
    private:
        void findActive();

        Service * maxEnabled();

        HardwareController * mController;
        IdleService mIdleService;
        std::vector<Service *> mServices;
        Service * mActive;
    };
}
