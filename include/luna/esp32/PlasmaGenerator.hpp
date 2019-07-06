#pragma once

#include "Generator.hpp"
#include "Noise.hpp"

#include <Eigen/Dense>

namespace luna::esp32
{
    struct PlasmaGenerator : Generator
    {
        explicit PlasmaGenerator();
        prism::CieXYZ generate(float ratio) const noexcept final;
        void setup(Location const & location) final;
        
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
        