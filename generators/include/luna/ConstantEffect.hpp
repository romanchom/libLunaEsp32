#pragma once

#include "Effect.hpp"
#include "ConstantGenerator.hpp"

#include <luna/MemberProperty.hpp>
#include <luna/ValueProperty.hpp>

#include <prism/Prism.hpp>

namespace luna
{
    struct ConstantEffect : Effect
    {
        explicit ConstantEffect();
        std::unique_ptr<Generator> generator(Time const & t) final;

        Property<float> & brightness() { return mBrightness; }
        Property<prism::RGB> & rgb() { return mRGB; }
        Property<prism::RGBW> & rgbw() { return mRGBW; }
        Property<float> & whiteness() { return mWhiteness; }
        Property<int> & temp() { return mTemp; }

        std::vector<std::tuple<std::string, AbstractProperty *>> properties() override;
    private:
        prism::RGB getRGB() const;
        void setRGB(prism::RGB const & value);

        prism::RGBW getRGBW() const;
        void setRGBW(prism::RGBW const & value);

        float getWhiteness() const;
        void setWhiteness(float const & value);

        prism::CieXYZ targetColor() const;

        prism::CieXYZ mCurrentColor;

        prism::CieXY mTargetChromaticity;
        float mTargetBrightness;
        float mTargetWhiteness;

        ValueProperty<float, ValidZeroOne> mBrightness;
        MemberProperty<ConstantEffect, prism::RGB> mRGB;
        MemberProperty<ConstantEffect, prism::RGBW> mRGBW;
        MemberProperty<ConstantEffect, float> mWhiteness;
        ValueProperty<int, ValidInRange<int, 153, 500>> mTemp;
    };
}
