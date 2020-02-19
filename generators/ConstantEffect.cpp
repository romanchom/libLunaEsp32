#include "ConstantEffect.hpp"

#include <cmath>
#include <esp_log.h>
namespace luna
{
    namespace
    {
        auto const converter = prism::RGBColorSpaceTransformation(prism::sRGB());
        auto const TAG = "Light";
    }

    ConstantEffect::ConstantEffect(std::string && name) :
        Effect(std::move(name)),
        mCurrentColor(prism::CieXYZ::Ones()),
        mTargetChromaticity(0.31271f, 0.32902),
        mRGB("rgb", this, &ConstantEffect::getRGB, &ConstantEffect::setRGB),
        mCieXY("xy", this, &ConstantEffect::getCieXY, &ConstantEffect::setCieXY),
        mBrightness("brightness", this, &ConstantEffect::getBrightness, &ConstantEffect::setBrightness),
        mWhiteness("whiteness", this, &ConstantEffect::getWhiteness, &ConstantEffect::setWhiteness)
    {}

    std::unique_ptr<Generator> ConstantEffect::generator(Time const & t)
    {
        auto factor = exp(-t.delta * 4);
        mCurrentColor = mCurrentColor * factor + targetColor() * (1.0f - factor);
        return std::make_unique<ConstantGenerator>(mCurrentColor);
    }

    std::vector<AbstractProperty *> ConstantEffect::properties()
    {
        return {&mRGB, &mCieXY, &mBrightness, &mWhiteness};
    }

    prism::RGB ConstantEffect::getRGB() const
    {
        return converter.transform(targetColor()).array().cwiseMax(0.0f).cwiseMin(1.0f);
    }

    void ConstantEffect::setRGB(prism::RGB const & value)
    {
        if ((getRGB() - value).head<3>().norm() < 0.02f) { return; }

        auto cie = converter.transform(value).head<3>().eval();
        if (cie.sum() > 0) {
            mTargetBrightness = value.maxCoeff();
            mTargetChromaticity = cie.head<2>() / cie.sum();
        } else {
            mTargetBrightness = 0;
        }

        mRGB.notify(value);
        mBrightness.notify(mTargetBrightness);
        mCieXY.notify(mTargetChromaticity);
    }

    prism::CieXY ConstantEffect::getCieXY() const
    {
        return mTargetChromaticity;
    }

    void ConstantEffect::setCieXY(prism::CieXY const & value)
    {
        ESP_LOGI(TAG, "X %f Y %f", value.x(), value.y());
        if ((mTargetChromaticity - value).norm() < 0.001f) { return; }

        mTargetChromaticity = value;

        mCieXY.notify(value);
        mRGB.notify(getRGB());
    }

    float ConstantEffect::getBrightness() const
    {
        return mTargetBrightness;
    }

    void ConstantEffect::setBrightness(float const & value)
    {
        if (mTargetBrightness == value) { return; }

        mTargetBrightness = value;
        mBrightness.notify(value);
    }

    float ConstantEffect::getWhiteness() const
    {
        return mTargetWhiteness;
    }

    void ConstantEffect::setWhiteness(float const & value)
    {
        if (mTargetWhiteness == value) { return; }
        mTargetWhiteness = value;
        mWhiteness.notify(value);
    }

    prism::CieXYZ ConstantEffect::targetColor() const
    {
        prism::CieXYZ ret;
        auto xyz = ret.head<3>();
        xyz << mTargetChromaticity, 1 - mTargetChromaticity.sum();
        xyz *= (mTargetBrightness) / xyz.maxCoeff();
        ret[3] = mTargetWhiteness;
        return ret;
    }
}
