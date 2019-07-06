#pragma once

#include "MqttEffect.hpp"
#include "PlasmaGenerator.hpp"

namespace luna::esp32
{
    struct PlasmaMqttEffect : MqttEffect
    {
        void update(float timeStep) final;
        Generator * generator() final;
        void configure(MqttTopic const & topic, std::string_view payload) final;
    private:
        PlasmaGenerator mGenerator;
    };
}
