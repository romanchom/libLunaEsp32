#pragma once

#include "MqttEffect.hpp"
#include "FlameGenerator.hpp"

namespace luna::esp32
{
    struct FlameMqttEffect : MqttEffect
    {
        void update(float timeStep) final;
        Generator * generator() final;
        void configure(MqttTopic const & topic, std::string_view payload) final;
    private:
        FlameGenerator mGenerator;
    };
}
