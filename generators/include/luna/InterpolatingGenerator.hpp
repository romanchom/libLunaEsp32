#pragma once

#include <luna/Generator.hpp>

namespace luna
{
    struct InterpolatingGenerator : Generator
    {
        InterpolatingGenerator(Generator * first, Generator * second, float ratio) :
            mFirst(first),
            mSecond(second),
            mRatio(ratio)
        {}

        prism::CieXYZ generate(float ratio) const noexcept final
        {
            return mFirst->generate(ratio) * (1 - mRatio) + mSecond->generate(ratio) * mRatio;
        }

        void setup(Location const & location) final
        {
            mFirst->setup(location);
            mSecond->setup(location);
        }

    protected:
        Generator * mFirst;
        Generator * mSecond;
        float mRatio;
    };
}
