#include "ServiceManager.hpp"
#include "Service.hpp"
#include "Device.hpp"
#include "Strand.hpp"
#include "Plugin.hpp"

#include <algorithm>

namespace luna
{
    ServiceManager::ServiceManager(EventLoop * mainLoop, Device * device, std::vector<Plugin *> plugins) :
        mDevice(device),
        mActive(nullptr)
    {
        mServices.emplace_back(&mIdleService);

        for (auto plugin : plugins) {
            if (auto service = plugin->initializeService()) {
                service->setManager(this);
                mServices.emplace_back(service);
            }
        }

        findActive();
    }

    void ServiceManager::enabled(Service * service, bool enabled)
    {
        auto it = std::find(mServices.begin(), mServices.end(), service);

        if (it == mServices.end()) {
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
                newActive->takeOwnership(mDevice);
            }
            mActive = newActive;
        }
    }

    Service * ServiceManager::maxEnabled()
    {
        auto it = std::find_if(mServices.rbegin(), mServices.rend(), [](auto service){
            return service->enabled();
        });

        if (it != mServices.rend()) {
            return *it;
        } else {
            return nullptr;
        }
    }
}
