#pragma once

#include "Generator.hpp"

namespace luna::esp32
{
    struct ConstantGenerator : Generator
    {
        explicit ConstantGenerator();
        prism::CieXYZ generate(float ratio, Location const & location) const noexcept override;

        void color(prism::CieXYZ const & value);
    private:
        prism::CieXYZ mColor;
    };
}
