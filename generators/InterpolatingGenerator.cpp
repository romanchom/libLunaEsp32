#include "InterpolatingGenerator.hpp"

namespace luna
{
    InterpolatingGenerator::InterpolatingGenerator() :
        mFirst(nullptr),
        mFirstRatio(0.0f),
        mSecond(nullptr),
        mSecondRatio(0.0f)
    {}
    
    void InterpolatingGenerator::first(Generator * generator, float ratio)
    {
        mFirst = generator;
        mFirstRatio = ratio;
    }

    void InterpolatingGenerator::second(Generator * generator, float ratio)
    {
        mSecond = generator;
        mSecondRatio = ratio;
    }

    prism::CieXYZ InterpolatingGenerator::generate(float ratio) const noexcept
    {
        return mFirst->generate(ratio) * mFirstRatio + mSecond->generate(ratio) * mSecondRatio;
    }
}
