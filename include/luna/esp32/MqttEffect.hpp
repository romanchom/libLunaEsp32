#pragma once

#include "Generator.hpp"
#include "MqttTopic.hpp"

namespace luna::esp32
{
    struct MqttEffect
    {
        virtual void update(float timeStep) = 0;
        virtual Generator const * generator() const = 0;
        virtual void configure(MqttTopic const & topic, void * data, size_t size) = 0;
    protected:
        ~MqttEffect() = default;
    };
}
