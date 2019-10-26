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

        struct Record
        {
            Service * service;
            int priority;
            bool enabled;
        };

        Record * maxEnabled();

        HardwareController * mController;
        std::vector<Record> mRecords;
    };
}
