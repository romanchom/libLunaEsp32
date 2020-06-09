#include "MqttPlugin.hpp"

#include <luna/LunaInterface.hpp>
#include <luna/mqtt/Controller.hpp>

namespace luna
{
    struct MqttPlugin::Instance : PluginInstance
    {
        Instance(MqttPlugin const * parent) :
            mParent(parent)
        {}

        void onNetworkAvaliable(LunaNetworkInterface * luna) final
        {
            luna->addNetworkService(std::make_unique<mqtt::Controller>(
                luna,
                mParent->mEffectEngine,
                mParent->mName,
                mParent->mAddress,
                luna->tlsCredentials(),
                mParent->mFloatScale
            ));
        }

    private:
        MqttPlugin const * mParent;
    };

    MqttPlugin::MqttPlugin(std::string && name, std::string && address, Configurable * effectEngine, float floatScale) :
        mName(std::move(name)),
        mAddress(std::move(address)),
        mEffectEngine(effectEngine),
        mFloatScale(floatScale)
    {}

    MqttPlugin::~MqttPlugin() = default;

    std::unique_ptr<PluginInstance> MqttPlugin::instantiate(LunaInterface * luna) 
    {
        return std::make_unique<Instance>(this);
    }
}