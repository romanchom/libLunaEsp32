#include "PlasmaGenerator.hpp"

using namespace prism;

namespace luna
{
    PlasmaGenerator::PlasmaGenerator(float time, float frequency, float saturation) :
        mTime(time),
        mFrequency(frequency),
        mHueChangeRate(0.05f),
        mSaturation(saturation),
        mColorTransformation(rgbToXyzTransformationMatrix(rec2020()))
    {}

    prism::CieXYZ PlasmaGenerator::generate(float ratio) const noexcept
    {
        auto position = mBegin + mDiff * ratio;
        float hue = mNoise.at(position) * 2.0f;
        prism::HSV hsv{hue + mTime * mHueChangeRate, mSaturation, 1.0f, 0.0f};
        prism::CieXYZ ret;
        ret.head<3>() = mColorTransformation * prism::toRgb(hsv).head<3>();
        ret[3] = 0.0f;
        return ret;
    }

    void PlasmaGenerator::location(Location const & location)
    {
        mDiff = (location.end - location.begin) * mFrequency;
        mBegin = location.begin;
        mBegin.z() += mTime * 0.1f;
    }
}
