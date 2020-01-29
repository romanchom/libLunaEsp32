#include "ConstantEffect.hpp"

#include <cmath>

namespace luna
{
    ConstantEffect::ConstantEffect(std::string && name) :
        Effect(std::move(name)),
        mCurrentColor(prism::CieXYZ::Zero()),
        mTargetColor(prism::CieXYZ::Zero()),
        mColor("color", this, &ConstantEffect::getColor, &ConstantEffect::setColor),
        mWhiteness("whiteness", this, &ConstantEffect::getWhiteness, &ConstantEffect::setWhiteness)
    {}

    std::unique_ptr<Generator> ConstantEffect::generator(Time const & t)
    {
        auto factor = exp(-t.delta * 4);
        mCurrentColor = mCurrentColor * factor + mTargetColor * (1.0f - factor);
        return std::make_unique<ConstantGenerator>(mCurrentColor);
    }

    std::vector<AbstractProperty *> ConstantEffect::properties()
    {
        return {&mColor, &mWhiteness};
    }

    prism::CieXYZ ConstantEffect::getColor() const
    {
        return mTargetColor;
    }

    void ConstantEffect::setColor(prism::CieXYZ const & value)
    {
        if ((mCurrentColor.head<3>() - value.head<3>()).norm() < 0.01f) { return; }
        mTargetColor.head<3>() = value.head<3>();
        mColor.notify(value);
    }

    float ConstantEffect::getWhiteness() const
    {
        return mTargetColor[3];
    }

    void ConstantEffect::setWhiteness(float const & value)
    {
        if (mTargetColor[3] == value) { return; }
        mTargetColor[3] = value;
        mWhiteness.notify(value);
    }
}
