#pragma once

#include "Generator.hpp"

namespace luna::esp32
{
    struct InterpolatingGenerator : Generator
    {
        InterpolatingGenerator(Generator const * first, Generator const * second, float ratio) :
            mFirst(first),
            mSecond(second),
            mRatio(ratio)
        {}

        prism::CieXYZ generate(float ratio, Location const & location) const noexcept final
        {
            return mFirst->generate(ratio, location) * (1 - mRatio) + mSecond->generate(ratio, location) * mRatio;
        }
    private:
        Generator const * mFirst;
        Generator const * mSecond;
        float mRatio;
    };
}
