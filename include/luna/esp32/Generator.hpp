#pragma once

#include "Strand.hpp"

#include <prism/Prism.hpp>

namespace luna::esp32
{
    struct Generator
    {
        template<typename F>
        void prepare(Location const & location, F && callback)
        {
            setup(location);
            callback(this);
        }

        virtual prism::CieXYZ generate(float ratio) const noexcept = 0;
        virtual void setup(Location const & location) = 0;
    protected:
        ~Generator() = default;
    };
}
