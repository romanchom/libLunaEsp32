#include "Service.hpp"
#include "ServiceManager.hpp"

namespace luna
{
    Service::Service() :
        mManager(nullptr),
        mEnabled(false)
    {}

    void Service::setManager(ServiceManager * manager)
    {
        mManager = manager;
    }

    void Service::serviceEnabled(bool enabled)
    {
        mEnabled = true;
        if (mManager) {
            mManager->serviceEnabled(this, enabled);
        }
    }

    bool Service::serviceEnabled() const noexcept
    {
        return mEnabled;
    }
}
