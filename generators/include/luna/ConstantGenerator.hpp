#pragma once

#include <luna/Generator.hpp>

namespace luna
{
    struct ConstantGenerator : Generator
    {
        explicit ConstantGenerator();
        prism::CieXYZ generate(float ratio) const noexcept final;

        void color(prism::CieXYZ const & value);

    private:
        prism::CieXYZ mColor;
    };
}
