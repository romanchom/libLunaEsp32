#pragma once

#include <luna/Plugin.hpp>

#include <string>

namespace luna
{
    struct EventLoop;
    struct Configurable;

    struct MqttPlugin : Plugin
    {
        explicit MqttPlugin(std::string && name, std::string && address, EventLoop * mainLoop, Configurable * effectEngine);
        ~MqttPlugin();

        luna::Service * initializeService() final;
        std::unique_ptr<NetworkService> initializeNetworking(NetworkingContext const & network) final;
    private:
        std::string mName;
        std::string mAddress;
        EventLoop * mMainLoop;
        Configurable * mEffectEngine;
    };
}