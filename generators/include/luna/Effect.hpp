#pragma once

#include <luna/Generator.hpp>
#include <luna/Configurable.hpp>

#include <string>

namespace luna
{
    struct EffectEngine;

    struct Effect : Configurable
    {
        using Configurable::Configurable;
        virtual void update(float timeStep) = 0;
        virtual Generator * generator(Location const & location) = 0;
    };
}
