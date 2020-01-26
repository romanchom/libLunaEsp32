#include "Luna.hpp"
#include "OnlineContext.hpp"

#include <luna/Plugin.hpp>
#include <luna/EventLoop.hpp>

#include <esp_log.h>

namespace luna
{
    Luna::Luna(EventLoop * mainLoop, LunaConfiguration * config) :
        mMainLoop(mainLoop),
        mConfig(config),
        mControllerMux(mMainLoop, config->hardware, mConfig->plugins),
        mWiFi(mConfig->wifiCredentials.ssid, mConfig->wifiCredentials.password)
    {
        mWiFi.observer(this);
        mMainLoop->post([this]{
            ESP_LOGW("Luna", "Up and running.");
            mWiFi.enabled(true);
        });
    }

    Luna::~Luna() = default;

    void Luna::connected()
    {
        mMainLoop->post([this]{
            mOnlineContext = std::make_unique<OnlineContext>(mMainLoop, mConfig->tlsCredentials, mConfig->plugins);
        });
    }

    void Luna::disconnected()
    {
        mMainLoop->post([this]{
            mOnlineContext.reset();
        });
    }
}
