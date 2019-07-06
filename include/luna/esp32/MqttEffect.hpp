#pragma once

#include "Generator.hpp"
#include "MqttTopic.hpp"

namespace luna::esp32
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
