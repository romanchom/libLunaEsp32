#pragma once

#include "Noise.hpp"

#include <luna/Generator.hpp>

#include <Eigen/Dense>

namespace luna
{
    struct PlasmaGenerator : Generator
    {
        explicit PlasmaGenerator();
        prism::CieXYZ generate(float ratio) const noexcept final;

        void location(Location const & location);
        void time(float t) { mTime = t; }
        float time() const noexcept { return mTime; }
        void saturation(float value) { mSaturation = value; }
        void brightness(float value) { mBrightness = value; }

    private:
        Eigen::Matrix<prism::ColorScalar, 3, 3> mColorTransformation;
        float mTime;
        float mFrequency;
        float mHueChangeRate;
        float mSaturation;
        float mBrightness;

        Noise mNoise;
        Eigen::Matrix<float, 3, 1> mBegin, mDiff;
    };
}
