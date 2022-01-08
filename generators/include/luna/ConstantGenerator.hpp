#pragma once

#include <luna/Generator.hpp>

namespace luna
{
    struct ConstantGenerator : Generator
    {
        explicit ConstantGenerator(prism::CieXYZ const & color, float temperature);
        void location(Location const & location) final;
        prism::CieXYZ generate(float ratio) const noexcept final;
        float whiteTemperature() const final;

    private:
        prism::CieXYZ mColor;
        float mTemperature;
    };
}
