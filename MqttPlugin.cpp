#include "MqttPlugin.hpp"

#include <luna/mqtt/Service.hpp>
#include <luna/NetworkingContext.hpp>

namespace luna
{
    MqttPlugin::MqttPlugin(std::string && name, std::string && address, EventLoop * mainLoop, Configurable * effectEngine) :
        mName(std::move(name)),
        mAddress(std::move(address)),
        mMainLoop(mainLoop),
        mEffectEngine(effectEngine)
    {}

    MqttPlugin::~MqttPlugin() = default;

    luna::Service * MqttPlugin::initializeService() 
    {
        return nullptr;
    }

    std::unique_ptr<NetworkService> MqttPlugin::initializeNetworking(NetworkingContext const & network)
    {
        return std::make_unique<mqtt::Service>(mMainLoop, mEffectEngine, mName, mAddress, network.tlsCredentials);
    }
}