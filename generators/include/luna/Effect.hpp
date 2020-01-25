#pragma once

#include <luna/Generator.hpp>
#include <luna/Configurable.hpp>

#include <string>
#include <memory>

namespace luna
{
    struct EffectEngine;

    struct Effect : Configurable
    {
        using Configurable::Configurable;
        virtual void update(float timeStep) = 0;
        virtual std::unique_ptr<Generator> generator() = 0;
    };
}
