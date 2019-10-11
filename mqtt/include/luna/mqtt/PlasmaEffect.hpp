#pragma once

#include "Effect.hpp"

#include <luna/PlasmaGenerator.hpp>

namespace luna::mqtt
{
    struct PlasmaEffect : Effect
    {
        explicit PlasmaEffect(Service * owner, std::string_view name);
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
    private:
        PlasmaGenerator mGenerator;
    };
}
