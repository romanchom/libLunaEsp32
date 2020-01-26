#pragma once

#include "IdleService.hpp"

#include <vector>

namespace luna
{
    struct Service;
    struct Device;
    struct Plugin;
    struct EventLoop;

    struct ServiceManager
    {
        explicit ServiceManager(EventLoop * mainLoop, Device * device, std::vector<Plugin *> plugins);

        void enabled(Service * service, bool enabled);
    private:
        void findActive();

        Service * maxEnabled();

        Device * mDevice;
        IdleService mIdleService;
        std::vector<Service *> mServices;
        Service * mActive;
    };
}
