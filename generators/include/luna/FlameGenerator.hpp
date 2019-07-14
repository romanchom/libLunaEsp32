#pragma once

#include "Noise.hpp"

#include <luna/Generator.hpp>

#include <Eigen/Dense>

namespace luna
{
    struct FlameGenerator : Generator
    {
        explicit FlameGenerator();

        prism::CieXYZ generate(float ratio) const noexcept final;

        void location(Location const & location);
        void time(float t) { mTime = t; }
        float time() const noexcept { return mTime; }
        void temperatureLow(float value) noexcept { mTemperatureLow = value; }
        void temperatureHigh(float value) noexcept { mTemperatureHigh = value; }
        void brightness(float value) noexcept { mBrightness = value; }
        void frequency(float value) noexcept { mFrequency = value; }

    private:
        float mTime;
        float mTemperatureLow;
        float mTemperatureHigh;
        float mBrightness;
        float mFrequency;

        Noise mTemperatureNoise;
        Eigen::Matrix<float, 3, 1> mBegin, mDiff;
        float mTemperatureDifference;
        bool mIsHorizontal;
    };
}
