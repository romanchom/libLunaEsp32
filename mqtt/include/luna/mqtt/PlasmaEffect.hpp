#pragma once

#include "Effect.hpp"

#include <luna/PlasmaGenerator.hpp>

namespace luna::mqtt
{
    struct PlasmaEffect : Effect
    {
        using Effect::Effect;
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
        void configure(Topic const & topic, std::string_view payload) final;
    private:
        PlasmaGenerator mGenerator;
    };
}
