#pragma once

#include <luna/Generator.hpp>
#include <luna/Configurable.hpp>

#include <string>

namespace luna
{
    struct EffectEngine;

    struct Effect : Configurable
    {
        explicit Effect(EffectEngine * owner, std::string_view name);
        virtual void update(float timeStep) = 0;
        virtual Generator * generator(Location const & location) = 0;
    protected:
        ~Effect() = default;
    };
}
