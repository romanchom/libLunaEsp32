#pragma once

#include <luna/Generator.hpp>

namespace luna
{
    struct InterpolatingGenerator : Generator
    {
        explicit InterpolatingGenerator();

        void first(Generator * generator, float ratio);
        void second(Generator * generator, float ratio);

        prism::CieXYZ generate(float ratio) const noexcept final;

    protected:
        Generator * mFirst;
        float mFirstRatio;
        Generator * mSecond;
        float mSecondRatio;
    };
}
