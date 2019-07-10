#pragma once

#include "HardwareController.hpp"

namespace luna
{
    struct ServiceManager;

    struct Service
    {
        explicit Service();

    protected:
        ~Service() = default;

        virtual void takeOwnership(HardwareController * controller) = 0;
        virtual void releaseOwnership() = 0;

        void serviceEnabled(bool enabled);

    private:
        friend ServiceManager;

        void setManager(ServiceManager * manager);

        ServiceManager * mManager;
    };
}
