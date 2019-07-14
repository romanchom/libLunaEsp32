#pragma once

#include "Strand.hpp"

#include <prism/Prism.hpp>

namespace luna
{
    struct Generator
    {
        virtual prism::CieXYZ generate(float ratio) const noexcept = 0;
    protected:
        ~Generator() = default;
    };
}
