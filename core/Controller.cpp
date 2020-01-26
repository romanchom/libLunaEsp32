#include "Controller.hpp"
#include "ControllerMux.hpp"

namespace luna
{
    Controller::Controller() :
        mManager(nullptr),
        mEnabled(false)
    {}

    void Controller::setManager(ControllerMux * manager)
    {
        mManager = manager;
    }

    void Controller::enabled(bool enabled)
    {
        mEnabled = enabled;
        if (mManager) {
            mManager->enabled(this, enabled);
        }
    }

    bool Controller::enabled() const noexcept
    {
        return mEnabled;
    }
}
