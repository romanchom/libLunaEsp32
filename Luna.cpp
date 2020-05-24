#include "Luna.hpp"
#include "OnlineContext.hpp"

#include <luna/Plugin.hpp>
#include <luna/EventLoop.hpp>
#include <luna/LunaContext.hpp>

#include <esp_log.h>

namespace luna
{
    Luna::Luna(LunaConfiguration const * config) :
        mConfig(config),
        mControllerMux(mConfig->device)
    {
        LunaContext context {
            &mMainLoop,
            &mControllerMux,
        };

        for (auto plugin : config->plugins) {
            auto controller = plugin->getController(context);
            if (controller) {
                mControllerMux.add(controller);
            }
        }

        mMainLoop.post([this]{
            mWiFi = std::make_unique<WiFi>(mConfig->wifiCredentials.ssid, mConfig->wifiCredentials.password);
            mWiFi->observer(this);
            mWiFi->enabled(true);
        });
    }

    Luna::~Luna() = default;

    void Luna::run()
    {
        ESP_LOGW("Luna", "Up and running.");
        mMainLoop.execute();
    }

    void Luna::connected()
    {
        mMainLoop.post([this]{
            LunaContext context {
                &mMainLoop,
                &mControllerMux,
            };
            mOnlineContext = std::make_unique<OnlineContext>(context, mConfig->tlsCredentials, mConfig->plugins);
        });
    }

    void Luna::disconnected()
    {
        mMainLoop.post([this]{
            mOnlineContext.reset();
        });
    }
}
