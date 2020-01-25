#pragma once

namespace luna
{
    struct ServiceManager;
    struct HardwareController;

    struct Service
    {
        explicit Service();

        void serviceEnabled(bool enabled);
        bool serviceEnabled() const noexcept;
    protected:
        ~Service() = default;

    private:
        friend ServiceManager;

        virtual void takeOwnership(HardwareController * controller) = 0;
        virtual void releaseOwnership() = 0;
        void setManager(ServiceManager * manager);

        ServiceManager * mManager;
        bool mEnabled;
    };
}
