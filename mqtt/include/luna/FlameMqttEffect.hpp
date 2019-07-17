#pragma once

#include "MqttEffect.hpp"

#include <luna/FlameGenerator.hpp>

namespace luna
{
    struct FlameMqttEffect : MqttEffect
    {
        using MqttEffect::MqttEffect;
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
        void configure(MqttTopic const & topic, std::string_view payload) final;
    private:
        FlameGenerator mGenerator;
    };
}
