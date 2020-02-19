#pragma once

#include "Effect.hpp"
#include "ConstantGenerator.hpp"

#include <luna/MemberProperty.hpp>

#include <prism/Prism.hpp>

namespace luna
{
    struct ConstantEffect : Effect
    {
        explicit ConstantEffect(std::string && name);
        std::unique_ptr<Generator> generator(Time const & t) final;

        Property<prism::RGB> & rgb() { return mRGB; }
        Property<prism::CieXY> & cieXY() { return mCieXY; }
        Property<float> & brightness() { return mBrightness; }
        Property<float> & whiteness() { return mWhiteness; }

        std::vector<AbstractProperty *> properties() override;
    private:
        prism::RGB getRGB() const;
        void setRGB(prism::RGB const & value);

        prism::CieXY getCieXY() const;
        void setCieXY(prism::CieXY const & value);

        float getBrightness() const;
        void setBrightness(float const & value);

        float getWhiteness() const;
        void setWhiteness(float const & value);

        prism::CieXYZ targetColor() const;

        prism::CieXYZ mCurrentColor;

        prism::CieXY mTargetChromaticity;
        float mTargetBrightness;
        float mTargetWhiteness;

        MemberProperty<ConstantEffect, prism::RGB> mRGB;
        MemberProperty<ConstantEffect, prism::CieXY> mCieXY;
        MemberProperty<ConstantEffect, float> mBrightness;
        MemberProperty<ConstantEffect, float> mWhiteness;
    };
}
