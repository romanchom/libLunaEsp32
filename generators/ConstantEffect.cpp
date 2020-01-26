#include "ConstantEffect.hpp"

namespace luna
{
    ConstantEffect::ConstantEffect(std::string && name) :
        Effect(std::move(name)),
        mCurrentColor(prism::CieXYZ::Zero()),
        mTargetColor(prism::CieXYZ::Zero()),
        mColor("color", this, &ConstantEffect::getColor, &ConstantEffect::setColor),
        mWhiteness("whiteness", this, &ConstantEffect::getWhiteness, &ConstantEffect::setWhiteness)
    {}

    void ConstantEffect::update(float timeStep)
    {
        mCurrentColor = mCurrentColor * 0.98f + mTargetColor * 0.02f;
    }

    std::unique_ptr<Generator> ConstantEffect::generator()
    {
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
        mTargetColor.head<3>() = value.head<3>();
    }

    float ConstantEffect::getWhiteness() const
    {
        return mTargetColor[3];
    }

    void ConstantEffect::setWhiteness(float const & value)
    {
        mTargetColor[3] = value;
    }
}
