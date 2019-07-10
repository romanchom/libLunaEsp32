#pragma once

#include "MqttEffect.hpp"

#include <luna/FlameGenerator.hpp>

namespace luna
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
