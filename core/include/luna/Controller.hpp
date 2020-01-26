#pragma once

namespace luna
{
    struct ControllerMux;
    struct Device;

    struct Controller
    {
        explicit Controller();

    protected:
        void enabled(bool enabled);
        bool enabled() const noexcept;
        ~Controller() = default;

    private:
        friend ControllerMux;

        virtual void takeOwnership(Device * device) = 0;
        virtual void releaseOwnership() = 0;
        void setManager(ControllerMux * manager);

        ControllerMux * mManager;
        bool mEnabled;
    };
}
