#include "luna/esp32/PlasmaGenerator.hpp"

using namespace prism;

namespace luna::esp32
{
    PlasmaGenerator::PlasmaGenerator() :
        mColorTransformation(rgbToXyzTransformationMatrix(rec2020())),
        mFrequency(0.5f),
        mHueChangeRate(0.05f),
        mSaturation(1.0f),
        mBrightness(1.0f)
    {}

    prism::CieXYZ PlasmaGenerator::generate(float ratio) const noexcept 
    {
        auto position = mBegin + mDiff * ratio;
        float hue = mNoise.at(position) * 2.0f;
        prism::HSV hsv{hue + mTime * mHueChangeRate, mSaturation, mBrightness, 0.0f};
        prism::CieXYZ ret;
        ret.head<3>() = mColorTransformation * prism::toRgb(hsv).head<3>();
        ret[3] = 0.0f;
        return ret;
    }

    void PlasmaGenerator::setup(Location const & location)
    {
        mDiff = (location.end - location.begin) * mFrequency;
        mBegin = location.begin;
        mBegin.z() += mTime * 0.1f;
    }
}