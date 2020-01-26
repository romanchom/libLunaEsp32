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
        mControllerMux(mMainLoop, config->hardware, mConfig->plugins)
    {
        mMainLoop->post([this]{
            mWiFi = std::make_unique<WiFi>(mConfig->wifiCredentials.ssid, mConfig->wifiCredentials.password);
            mWiFi->observer(this);
            mWiFi->enabled(true);
        });
        ESP_LOGW("Luna", "Up and running.");
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
