#pragma once

#include "WiFi.hpp"
#include "WifiCredentials.hpp"

#include <luna/ControllerMux.hpp>
#include <luna/EventLoop.hpp>
#include <luna/TlsCredentials.hpp>

#include <memory>
#include <vector>

namespace luna
{
    struct Plugin;
    struct Device;
    struct OnlineContext;

    struct LunaConfiguration
    {
        std::vector<Plugin *> plugins;
        Device * device;
        WifiCredentials wifiCredentials;
        TlsCredentials tlsCredentials;
    };

    struct Luna : private WiFi::Observer
    {
        explicit Luna(LunaConfiguration const * plugins);
        ~Luna();

        void run();
    private:
        void connected() final;
        void disconnected() final;

        EventLoop mMainLoop;
        LunaConfiguration const * const mConfig;

        ControllerMux mControllerMux;
        std::unique_ptr<WiFi> mWiFi;
        std::unique_ptr<OnlineContext> mOnlineContext;
    };
}