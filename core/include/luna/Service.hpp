#pragma once

namespace luna
{
    struct ServiceManager;
    struct Device;

    struct Service
    {
        explicit Service();

        void serviceEnabled(bool enabled);
        bool serviceEnabled() const noexcept;
    protected:
        ~Service() = default;

    private:
        friend ServiceManager;

        virtual void takeOwnership(Device * device) = 0;
        virtual void releaseOwnership() = 0;
        void setManager(ServiceManager * manager);

        ServiceManager * mManager;
        bool mEnabled;
    };
}
