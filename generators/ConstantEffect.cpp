#include "ConstantEffect.hpp"

#include <cmath>
#include <esp_log.h>
namespace luna
{
    namespace
    {
        bool _converterInit = false;
        prism::RGBColorSpaceTransformation _converter;
        prism::RGBColorSpaceTransformation const & converter()
        {
            if (!_converterInit) {
                _converter = prism::RGBColorSpaceTransformation(prism::sRGB());
            }
            return _converter;
        }
        auto const TAG = "Light";
    }

    ConstantEffect::ConstantEffect() :
        mCurrentColor(prism::CieXYZ::Zero()),
        mTargetChromaticity(0.31271f, 0.32902f),
        mTargetBrightness(0.0f),
        mTargetWhiteness(0.0f),
        mBrightness(1.0f),
        mRGB(this, &ConstantEffect::getRGB, &ConstantEffect::setRGB),
        mRGBW(this, &ConstantEffect::getRGBW, &ConstantEffect::setRGBW),
        mWhiteness(this, &ConstantEffect::getWhiteness, &ConstantEffect::setWhiteness),
        mTemp(this, &ConstantEffect::getTemp, &ConstantEffect::setTemp)
    {}

    std::unique_ptr<Generator> ConstantEffect::generator(Time const & t)
    {
        auto factor = exp(-t.delta * 4);
        mCurrentColor = mCurrentColor * factor + mBrightness * targetColor() * (1.0f - factor);
        return std::make_unique<ConstantGenerator>(mCurrentColor, mTempValue);
    }

    std::vector<std::tuple<std::string, AbstractProperty *>> ConstantEffect::properties()
    {
        return {
            {"brightness", &mBrightness},
            {"rgbw", &mRGBW},
            {"rgb", &mRGB},
            {"whiteness", &mWhiteness},
            {"temp", &mTemp}
        };
    }

    prism::RGB ConstantEffect::getRGB() const
    {
        return converter().transform(targetColor()).array().cwiseMax(0.0f).cwiseMin(1.0f);
    }

    void ConstantEffect::setRGB(prism::RGB const & value)
    {
        ESP_LOGI(TAG, "R%F G%f B%f", value[0], value[1], value[2]);
        setRGBW({value[0], value[1], value[2], mTargetWhiteness});
    }

    prism::RGBW ConstantEffect::getRGBW() const
    {
        auto const rgb = getRGB();
        auto const w = mTargetWhiteness;

        return {rgb[0], rgb[1], rgb[2], w};
    }

    void ConstantEffect::setRGBW(prism::RGBW const & value)
    {
        ESP_LOGI(TAG, "R%F G%f B%f W%f", value[0], value[1], value[2], value[3]);

        if ((getRGBW() - value).cwiseAbs().maxCoeff() < 0.01f) return;

        auto cie = converter().transform(prism::RGB(value)).head<3>().eval();
        if (cie.sum() > 0) {
            mTargetBrightness = cie[1];
            mTargetChromaticity = cie.head<2>() / cie.sum();
        } else {
            mTargetBrightness = 0;
        }

        mTargetWhiteness = value[3];

        mRGB.notify(getRGB());
        mRGBW.notify(getRGBW());
        mWhiteness.notify(mTargetWhiteness);
    }

    float ConstantEffect::getWhiteness() const
    {
        return mTargetWhiteness;
    }

    void ConstantEffect::setWhiteness(float const & value)
    {
        auto rgb = getRGB();
        setRGBW({rgb[0], rgb[1], rgb[2], value});
    }

    float ConstantEffect::getTemp() const
    {
        return mTempValue;
    }

    void ConstantEffect::setTemp(float const & value)
    {
        mTempValue = value;
    }

    prism::CieXYZ ConstantEffect::targetColor() const
    {
        prism::CieXYZ ret;
        auto xyz = ret.head<3>();
        auto xy = mTargetChromaticity;
        xyz << xy.x() / xy.y(), 1, (1 - xy.x() - xy.y()) / xy.y();
        xyz *= mTargetBrightness;
        ret[3] = mTargetWhiteness;
        return ret;
    }
}
