#pragma once

#include <luna/Generator.hpp>

namespace luna
{
    struct ConstantGenerator : Generator
    {
        explicit ConstantGenerator(prism::CieXYZ const & color);
        void location(Location const & location) final;
        prism::CieXYZ generate(float ratio) const noexcept final;

    private:
        prism::CieXYZ mColor;
    };
}
