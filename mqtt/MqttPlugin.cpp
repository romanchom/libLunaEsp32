#include "MqttPlugin.hpp"

#include "Controller.hpp"
#include <luna/LunaInterface.hpp>

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
                mParent->mConfigurable,
                mParent->mAddress,
                luna->tlsCredentials(),
                mParent->mFloatScale
            ));
        }

    private:
        MqttPlugin const * mParent;
    };

    MqttPlugin::MqttPlugin(std::string && address, Configurable * configurable, float floatScale) :
        mAddress(std::move(address)),
        mConfigurable(configurable),
        mFloatScale(floatScale)
    {}

    MqttPlugin::~MqttPlugin() = default;

    std::unique_ptr<PluginInstance> MqttPlugin::instantiate(LunaInterface * luna)
    {
        return std::make_unique<Instance>(this);
    }
}