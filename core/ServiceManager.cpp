#include "ServiceManager.hpp"
#include "Service.hpp"
#include "HardwareController.hpp"
#include "Strand.hpp"
#include "Plugin.hpp"

#include <algorithm>

namespace luna
{
    ServiceManager::ServiceManager(EventLoop * mainLoop, HardwareController * controller, std::vector<Plugin *> plugins) :
        mController(controller),
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

    void ServiceManager::serviceEnabled(Service * service, bool enabled)
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
                newActive->takeOwnership(mController);
            }
            mActive = newActive;
        }
    }

    Service * ServiceManager::maxEnabled()
    {
        auto it = std::find_if(mServices.rbegin(), mServices.rend(), [](auto service){
            return service->serviceEnabled();
        });

        if (it != mServices.rend()) {
            return *it;
        } else {
            return nullptr;
        }
    }
}
