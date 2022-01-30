#include "InterpolatingGenerator.hpp"

namespace luna
{
    InterpolatingGenerator::InterpolatingGenerator(std::unique_ptr<Generator> first, float firstRatio, std::unique_ptr<Generator> second, float secondRatio) :
        mFirst(std::move(first)),
        mSecond(std::move(second)),
        mFirstRatio(firstRatio),
        mSecondRatio(secondRatio)
    {}

    void InterpolatingGenerator::location(Location const & value)
    {
        if (mFirst) {
            mFirst->location(value);
        }
        if (mSecond) {
            mSecond->location(value);
        }
    }


    prism::CieXYZ InterpolatingGenerator::generate(float ratio) const noexcept
    {
        prism::CieXYZ ret = mFirst->generate(ratio) * mFirstRatio;
        if (mSecond) {
            ret += mSecond->generate(ratio) * mSecondRatio;
        }
        return ret;
    }

    float InterpolatingGenerator::whiteTemperature() const
    {
        return mFirst->whiteTemperature();
    }

}
