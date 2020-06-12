#pragma once

#include <luna/Generator.hpp>
#include <luna/Configurable.hpp>

#include <string>
#include <memory>

namespace luna
{
    struct EffectPlugin;

    struct Time
    {
        float total;
        float delta;
    };

    struct Effect : Configurable
    {
        using Configurable::Configurable;
        virtual std::unique_ptr<Generator> generator(Time const & t) = 0;
    };
}
