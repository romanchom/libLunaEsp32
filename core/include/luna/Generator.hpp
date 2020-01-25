#pragma once

#include "Location.hpp"

#include <prism/Prism.hpp>

namespace luna
{
    struct Generator
    {
        virtual ~Generator() = default;
        virtual void location(Location const & location) = 0;
        virtual prism::CieXYZ generate(float ratio) const noexcept = 0;
    };
}
