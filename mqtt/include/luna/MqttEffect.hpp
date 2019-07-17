#pragma once

#include "MqttConfigurable.hpp"

#include <luna/Generator.hpp>

#include <string>

namespace luna
{
    struct MqttService;

    struct MqttEffect : MqttConfigurable
    {
        explicit MqttEffect(MqttService * owner, std::string const & name);
        virtual void update(float timeStep) = 0;
        virtual Generator * generator(Location const & location) = 0;
    protected:
        ~MqttEffect() = default;
    };
}
