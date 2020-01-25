#pragma once

#include "Effect.hpp"
#include "ConstantGenerator.hpp"

#include <prism/Prism.hpp>

namespace luna
{
    struct ConstantEffect : Effect
    {
        explicit ConstantEffect(std::string && name);
        void update(float timeStep) final;
        std::unique_ptr<Generator> generator() final;

        Property<prism::CieXYZ> & color() { return mColor; }
        Property<float> & whiteness() { return mWhiteness; }

        std::vector<AbstractProperty *> properties() override;
    private:
        prism::CieXYZ getColor() const;
        void setColor(prism::CieXYZ const & value);

        float getWhiteness() const;
        void setWhiteness(float const & value);

        prism::CieXYZ mCurrentColor;
        prism::CieXYZ mTargetColor;

        MemberProperty<ConstantEffect, prism::CieXYZ> mColor;
        MemberProperty<ConstantEffect, float> mWhiteness;
    };
}
