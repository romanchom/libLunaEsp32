#pragma once

#include "Effect.hpp"

#include "PlasmaGenerator.hpp"

namespace luna
{
    struct PlasmaEffect : Effect
    {
        explicit PlasmaEffect(EffectEngine * owner, std::string_view name);
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
    private:
        PlasmaGenerator mGenerator;
    };
}
