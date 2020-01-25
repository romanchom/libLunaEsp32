#pragma once

#include "Noise.hpp"

#include <luna/Generator.hpp>

#include <Eigen/Dense>

namespace luna
{
    struct FlameGenerator : Generator
    {
        explicit FlameGenerator(float time, float temperatureLow, float temperatureHigh, float frequency);
        void location(Location const & location) final;
        prism::CieXYZ generate(float ratio) const noexcept final;

    private:
        float mTime;
        float mTemperatureLow;
        float mTemperatureDifference;
        float mFrequency;

        Noise mTemperatureNoise;
        Eigen::Matrix<float, 3, 1> mBegin, mDiff;
        bool mIsHorizontal;
    };
}
