#pragma once

#include "WiFi.hpp"
#include "WifiCredentials.hpp"

#include <luna/ServiceManager.hpp>
#include <luna/TlsCredentials.hpp>

#include <memory>
#include <vector>

namespace luna
{
    struct Plugin;
    struct Device;
    struct EventLoop;
    struct OnlineContext;

    struct LunaConfiguration
    {
        std::vector<Plugin *> plugins;
        Device * hardware;
        WifiCredentials wifiCredentials;
        TlsCredentials tlsCredentials;
    };

    struct Luna : private WiFi::Observer
    {
        explicit Luna(EventLoop * mainLoop, LunaConfiguration * plugins);
        ~Luna();
    private:
        void connected() final;
        void disconnected() final;

        EventLoop * mMainLoop;
        LunaConfiguration * mConfig;

        ServiceManager mServiceManager;
        WiFi mWiFi;
        std::unique_ptr<OnlineContext> mOnlineContext;
    };
}