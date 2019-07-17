#pragma once

#include "Effect.hpp"

#include <luna/FlameGenerator.hpp>

namespace luna::mqtt
{
    struct FlameEffect : Effect
    {
        using Effect::Effect;
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
        void configure(Topic const & topic, std::string_view payload) final;
    private:
        FlameGenerator mGenerator;
    };
}
