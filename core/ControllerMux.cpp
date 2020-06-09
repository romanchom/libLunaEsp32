#include "ControllerMux.hpp"
#include "Controller.hpp"

#include <algorithm>

namespace luna
{
    ControllerMux::ControllerMux(Device * device) :
        mDevice(device),
        mActive(nullptr)
    {
        mControllers.emplace_back(State{true, &mIdleController});
        findActive();
    }

    void ControllerMux::add(Controller * controller)
    {
        mControllers.emplace_back(State{false, controller});
    }

    void ControllerMux::remove(Controller * controller)
    {
        setEnabled(controller, false);
        auto it = std::find_if(mControllers.begin(), mControllers.end(), [controller](const auto& state) {
            return state.controller == controller;
        });

        mControllers.erase(it);
    }

    void ControllerMux::setEnabled(Controller * controller, bool enabled)
    {
        auto it = std::find_if(mControllers.begin(), mControllers.end(), [controller](const auto& state) {
            return state.controller == controller;
        });

        if (it == mControllers.end()) return;
        if (it->enabled == enabled) return;
        it->enabled = enabled;
        findActive();
    }

    void ControllerMux::findActive()
    {
        Controller * newActive = nullptr;
        
        auto it = std::find_if(mControllers.rbegin(), mControllers.rend(), [](auto controller){
            return controller.enabled;
        });

        if (it != mControllers.rend()) {
            newActive = it->controller;
        }

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
}
