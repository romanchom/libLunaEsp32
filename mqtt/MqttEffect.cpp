#include "MqttEffect.hpp"

#include "MqttService.hpp"

namespace luna
{
    MqttEffect::MqttEffect(MqttService * owner, std::string const & name)
    {
        owner->addEffect(name, this);
    }
}
