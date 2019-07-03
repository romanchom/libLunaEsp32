#pragma once

#include "Strand.hpp"

#include <prism/Prism.hpp>

namespace luna::esp32
{
    struct Generator
    {
        virtual ~Generator() = default;
        virtual prism::CieXYZ generate(float ratio, Location const & location) const noexcept = 0;
    };
}
