#pragma once

#include "Generator.hpp"

namespace luna::esp32
{
    struct ConstantGenerator : Generator
    {
        explicit ConstantGenerator();
        prism::CieXYZ generate(float ratio) const noexcept final;

        void color(prism::CieXYZ const & value);
        void setup(Location const & location) final;

    private:
        prism::CieXYZ mColor;
    };
}
