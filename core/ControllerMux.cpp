#include "ControllerMux.hpp"
#include "Controller.hpp"
#include "Device.hpp"
#include "Strand.hpp"
#include "Plugin.hpp"

#include <algorithm>

namespace luna
{
    ControllerMux::ControllerMux(EventLoop * mainLoop, Device * device, std::vector<Plugin *> plugins) :
        mDevice(device),
        mActive(nullptr)
    {
        mControllers.emplace_back(&mIdleController);

        for (auto plugin : plugins) {
            if (auto controller = plugin->getController()) {
                controller->setManager(this);
                mControllers.emplace_back(controller);
            }
        }

        findActive();
    }

    void ControllerMux::enabled(Controller * controller, bool enabled)
    {
        auto it = std::find(mControllers.begin(), mControllers.end(), controller);

        if (it == mControllers.end()) {
            return;
        }

        findActive();
    }

    void ControllerMux::findActive()
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

    Controller * ControllerMux::maxEnabled()
    {
        auto it = std::find_if(mControllers.rbegin(), mControllers.rend(), [](auto controller){
            return controller->enabled();
        });

        if (it != mControllers.rend()) {
            return *it;
        } else {
            return nullptr;
        }
    }
}
