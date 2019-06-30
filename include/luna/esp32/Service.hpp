#pragma once

namespace luna::esp32
{
    struct ServiceManager;
    struct HardwareController;

    struct Service
    {
        explicit Service();
        virtual ~Service();

    protected:
        virtual void takeOwnership(HardwareController * controller) = 0;
        virtual void releaseOwnership() = 0;

        void serviceEnabled(bool enabled);

    private:
        friend ServiceManager;
        
        void setManager(ServiceManager * manager);

        ServiceManager * mManager;
    };
}
