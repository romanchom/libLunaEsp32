#include "ServiceManager.hpp"

#include "Service.hpp"

#include <algorithm>

namespace luna
{
    ServiceManager::ServiceManager(HardwareController * controller, std::initializer_list<Service *> services) :
        mController(controller)
    {
        for (int i = 0; i < services.size(); ++i) {
            mRecords.push_back({services.begin()[i], i, false});
        }
    }

    void ServiceManager::serviceEnabled(Service * service, bool enabled)
    {
        auto currentActive = maxEnabled();

        auto record = std::find_if(mRecords.begin(), mRecords.end(), [service](Record & r){
            return r.service == service;
        });

        if (record != mRecords.end()) {
            record->enabled = enabled;
        } else {
            return;
        }

        auto newActive = maxEnabled();

        if (currentActive != newActive) {
            if (currentActive) {
                currentActive->service->releaseOwnership();
            }
            if (newActive) {
                newActive->service->takeOwnership(mController);
            }
        }
    }

    ServiceManager::Record * ServiceManager::maxEnabled()
    {
        auto record = std::max_element(mRecords.begin(), mRecords.end(), [](auto const & l, auto const & r) {
            if (l.enabled ^ r.enabled) {
                return r.enabled;
            } else {
                return l.priority < r.priority;
            }
        });

        if (record != mRecords.end() && record->enabled) {
            return &*record;
        } else {
            return nullptr;
        }
    }
}
