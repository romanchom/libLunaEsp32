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

    void Service::enabled(bool enabled)
    {
        mEnabled = enabled;
        if (mManager) {
            mManager->enabled(this, enabled);
        }
    }

    bool Service::enabled() const noexcept
    {
        return mEnabled;
    }
}
