#pragma once

#include "Effect.hpp"

#include <luna/FlameGenerator.hpp>

namespace luna::mqtt
{
    struct FlameEffect : Effect
    {
        explicit FlameEffect(Service * owner, std::string_view name);
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
    private:
        FlameGenerator mGenerator;
    };
}
