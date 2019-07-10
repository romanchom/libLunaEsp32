#include "FlameGenerator.hpp"

#include <Eigen/Dense>

namespace luna
{
    FlameGenerator::FlameGenerator() :
        mTime(0.0f),
        mTemperatureLow(1500.0f),
        mTemperatureHigh(3000.0f),
        mBrightness(2.0f),
        mFrequency(1.5f)
    {}

    prism::CieXYZ FlameGenerator::generate(float ratio) const noexcept
    {
        Eigen::Matrix<float, 3, 1> position = mBegin  + mDiff * ratio;

        auto baseTemperature = mTemperatureNoise.at(position) + 0.5f;

        auto smoothTemp = mTemperatureLow + mTemperatureDifference * baseTemperature;

        auto const fadeFactor = mIsHorizontal ?  0.0f : ratio * ratio * ratio;

        auto color = prism::temperature(smoothTemp);
        color *= mBrightness * std::max(0.0f, baseTemperature - fadeFactor);
        return color;
    }

    void FlameGenerator::setup(Location const & location)
    {
        mDiff = (location.end - location.begin) * mFrequency;
        mIsHorizontal = (std::abs(mDiff.y()) < 0.5f);

        mBegin = location.begin;
        if (mIsHorizontal) {
            mBegin.y() -= mTime;
        } else {
            mBegin.y() -= mTime * 2.3743324f;
            mBegin.z() += mTime;
        }

        mTemperatureDifference = mTemperatureHigh - mTemperatureLow;
    }
}
