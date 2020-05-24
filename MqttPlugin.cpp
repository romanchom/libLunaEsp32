#include "MqttPlugin.hpp"

#include <luna/mqtt/Controller.hpp>
#include <luna/NetworkingContext.hpp>

namespace luna
{
    MqttPlugin::MqttPlugin(std::string && name, std::string && address, Configurable * effectEngine, float floatScale) :
        mName(std::move(name)),
        mAddress(std::move(address)),
        mEffectEngine(effectEngine),
        mFloatScale(floatScale)
    {}

    MqttPlugin::~MqttPlugin() = default;

    luna::Controller * MqttPlugin::getController(LunaContext const & context) 
    {
        return nullptr;
    }

    std::unique_ptr<NetworkService> MqttPlugin::makeNetworkService(LunaContext const & context,NetworkingContext const & network)
    {
        return std::make_unique<mqtt::Controller>(context.mainLoop, mEffectEngine, mName, mAddress, network.tlsCredentials, mFloatScale);
    }
}