#pragma once

#include "MqttEffect.hpp"

#include <luna/PlasmaGenerator.hpp>

namespace luna
{
    struct PlasmaMqttEffect : MqttEffect
    {
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
        void configure(MqttTopic const & topic, std::string_view payload) final;
    private:
        PlasmaGenerator mGenerator;
    };
}
