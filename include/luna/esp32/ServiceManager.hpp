#pragma once

#include <vector>

namespace luna::esp32
{
    struct Service;
    struct HardwareController;

    struct ServiceManager
    {
        explicit ServiceManager(HardwareController * controller);

        void manage(Service * service, int priority, bool enabled = false);
    private:
        friend Service;

        struct Record
        {
            Service * service;
            int priority;
            bool enabled;
        };

        void serviceEnabled(Service * service, bool enabled);
        Record * maxEnabled();

        HardwareController * mController;
        std::vector<Record> mRecords;
    };
}
