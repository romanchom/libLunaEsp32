#pragma once

#include <luna/Generator.hpp>

namespace luna
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
