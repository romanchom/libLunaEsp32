#include "FlameGenerator.hpp"

#include <Eigen/Dense>

namespace luna
{
    FlameGenerator::FlameGenerator(float time, float temperatureLow, float temperatureHigh, float frequency) :
        mTime(time),
        mTemperatureLow(temperatureLow),
        mTemperatureDifference(temperatureHigh - temperatureLow),
        mFrequency(frequency)
    {}

    prism::CieXYZ FlameGenerator::generate(float ratio) const noexcept
    {
        Eigen::Matrix<float, 3, 1> position = mBegin  + mDiff * ratio;

        auto const baseTemperature = mTemperatureNoise.at(position) + 0.5f;

        auto const smoothTemp = mTemperatureLow + mTemperatureDifference * baseTemperature;

        auto const fadeFactor = mIsHorizontal ?  0.0f : ratio * ratio * ratio;

        auto const chromaticity = prism::temperature(smoothTemp);
        prism::CieXYZ color;
        color << chromaticity, 1 - chromaticity.sum(), 0;
        auto xyz = color.head<3>();
        xyz /= xyz.maxCoeff();
        color *= std::max(0.0f, baseTemperature - fadeFactor);
        return color;
    }

    void FlameGenerator::location(Location const & location)
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
    }
}
