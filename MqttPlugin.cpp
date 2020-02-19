#include "MqttPlugin.hpp"

#include <luna/mqtt/Controller.hpp>
#include <luna/NetworkingContext.hpp>

namespace luna
{
    MqttPlugin::MqttPlugin(std::string && name, std::string && address, EventLoop * mainLoop, Configurable * effectEngine, float floatScale) :
        mName(std::move(name)),
        mAddress(std::move(address)),
        mMainLoop(mainLoop),
        mEffectEngine(effectEngine),
        mFloatScale(floatScale)
    {}

    MqttPlugin::~MqttPlugin() = default;

    luna::Controller * MqttPlugin::getController() 
    {
        return nullptr;
    }

    std::unique_ptr<NetworkService> MqttPlugin::makeNetworkService(NetworkingContext const & network)
    {
        return std::make_unique<mqtt::Controller>(mMainLoop, mEffectEngine, mName, mAddress, network.tlsCredentials, mFloatScale);
    }
}