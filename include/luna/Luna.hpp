#pragma once

#include "WiFi.hpp"
#include "WifiCredentials.hpp"

#include <luna/ControllerMux.hpp>
#include <luna/EventLoop.hpp>
#include <luna/LunaInterface.hpp>
#include <luna/NetworkService.hpp>
#include <luna/TlsConfiguration.hpp>
#include <luna/TlsCredentials.hpp>
#if ESP32
#   include <luna/OnlineContext.hpp>
#endif

#include <memory>
#include <optional>
#include <vector>

namespace luna
{
    struct Plugin;
    struct PluginInstance;
    struct Device;

    struct LunaConfiguration
    {
        std::string name;
        std::vector<Plugin *> plugins;
        Device * device;
        WifiCredentials wifiCredentials;
        TlsCredentials tlsCredentials;
    };

    struct Luna : private WiFi::Observer, private LunaNetworkInterface
    {
        explicit Luna(LunaConfiguration const * plugins);
        ~Luna();

        void run();
    private:
        void connected() final;
        void disconnected() final;

        std::string const & name() final;
        void post(std::function<void()> && task) final;
        std::unique_ptr<ControllerHandle> addController(Controller * controller) final;
        Device * device() final;

        void addNetworkService(std::unique_ptr<NetworkService> service) final;
        TlsCredentials const & tlsCredentials() final;
        TlsConfiguration * tlsConfiguration() final;
        asio::io_context * ioContext() final;

        EventLoop mMainLoop;
        LunaConfiguration const * const mConfig;

        ControllerMux mControllerMux;
        std::vector<std::unique_ptr<PluginInstance>> mPluginInstances;
        std::optional<WiFi> mWiFi;
#if ESP32
        std::optional<OnlineContext> mOnlineContext;
#endif
        std::optional<TlsConfiguration> mTlsConfiguration;
        std::vector<std::unique_ptr<NetworkService>> mNetworkServices;
    };
}