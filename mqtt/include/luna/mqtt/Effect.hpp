#pragma once

#include "Configurable.hpp"

#include <luna/Generator.hpp>

#include <string>

namespace luna::mqtt
{
    struct Service;

    struct Effect : Configurable
    {
        explicit Effect(Service * owner, std::string_view name);
        virtual void update(float timeStep) = 0;
        virtual Generator * generator(Location const & location) = 0;
    protected:
        ~Effect() = default;
    };
}
