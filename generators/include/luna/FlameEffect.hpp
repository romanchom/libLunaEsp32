#pragma once

#include "Effect.hpp"
#include "FlameGenerator.hpp"

namespace luna
{
    struct FlameEffect : Effect
    {
        explicit FlameEffect(EffectEngine * owner, std::string_view name);
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
    private:
        FlameGenerator mGenerator;
    };
}
