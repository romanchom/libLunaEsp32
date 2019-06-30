#include "luna/esp32/ServiceManager.hpp"
#include "luna/esp32/Service.hpp"

#include <esp_log.h>

#include <algorithm>

static const char TAG[] = "SvcMgr";

namespace luna::esp32
{
    ServiceManager::ServiceManager(HardwareController * controller) :
        mController(controller)
    {}

    void ServiceManager::manage(Service * service, int priority)
    {
        mRecords.push_back({service, priority, false});
        service->setManager(this);
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
            ESP_LOGW(TAG, "Attempting to enable non registered service");
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
