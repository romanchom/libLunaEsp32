#include "ServiceManager.hpp"

#include "Service.hpp"

#include <algorithm>

namespace luna
{
    ServiceManager::ServiceManager(HardwareController * controller, std::initializer_list<Service *> services) :
        mController(controller),
        mActive(nullptr)
    {
        for (int i = 0; i < services.size(); ++i) {
            auto service = services.begin()[i];
            service->setManager(this);
            mRecords.push_back(service);
        }

        findActive();
    }

    void ServiceManager::serviceEnabled(Service * service, bool enabled)
    {
        auto it = std::find(mRecords.begin(), mRecords.end(), service);

        if (it == mRecords.end()) {
            return;
        }

        findActive();
    }

    void ServiceManager::findActive()
    {
        auto newActive = maxEnabled();

        if (mActive != newActive) {
            if (mActive) {
                mActive->releaseOwnership();
            }
            if (newActive) {
                newActive->takeOwnership(mController);
            }
            mActive = newActive;
        }
    }

    Service * ServiceManager::maxEnabled()
    {
        for (int i = mRecords.size() - 1; i >= 0; --i) {
            auto service = mRecords[i];
            if (service->serviceEnabled()) {
                return service;
            }
        }
        return nullptr;
    }
}
