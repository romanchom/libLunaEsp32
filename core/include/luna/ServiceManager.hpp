#pragma once

#include <vector>

namespace luna
{
    struct Service;
    struct HardwareController;

    struct ServiceManager
    {
        explicit ServiceManager(HardwareController * controller, std::initializer_list<Service *> services);

        void serviceEnabled(Service * service, bool enabled);
    private:
        void findActive();

        Service * maxEnabled();

        HardwareController * mController;
        std::vector<Service *> mRecords;
        Service * mActive;
    };
}
