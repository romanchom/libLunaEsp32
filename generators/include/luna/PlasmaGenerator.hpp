#pragma once

#include "Noise.hpp"

#include <luna/Generator.hpp>

#include <Eigen/Dense>

namespace luna
{
    struct PlasmaGenerator : Generator
    {
        explicit PlasmaGenerator(float time, float frequency, float saturation);
        prism::CieXYZ generate(float ratio) const noexcept final;
        void location(Location const & location) final;

    private:
        float mTime;
        float mFrequency;
        float mHueChangeRate;
        float mSaturation;
        float mBrightness;

        Noise mNoise;
        Eigen::Matrix<float, 3, 1> mBegin, mDiff;
        Eigen::Matrix<prism::ColorScalar, 3, 3> mColorTransformation;
    };
}
