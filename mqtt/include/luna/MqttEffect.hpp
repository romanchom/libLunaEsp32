#pragma once

#include "MqttTopic.hpp"

#include <luna/Generator.hpp>

namespace luna
{
    struct MqttEffect
    {
        virtual void update(float timeStep) = 0;
        virtual Generator * generator() = 0;
        virtual void configure(MqttTopic const & topic, std::string_view payload) = 0;
    protected:
        ~MqttEffect() = default;
    };
}
