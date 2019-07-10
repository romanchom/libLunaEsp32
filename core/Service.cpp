#include "Service.hpp"
#include "ServiceManager.hpp"

namespace luna
{
    Service::Service() :
        mManager(nullptr)
    {}

    void Service::setManager(ServiceManager * manager)
    {
        mManager = manager;
    }

    void Service::serviceEnabled(bool enabled)
    {
        if (mManager) {
            mManager->serviceEnabled(this, enabled);
        }
    }
}
