#include "ConstantEffect.hpp"

#include <luna/Parse.hpp>

namespace luna
{
    ConstantEffect::ConstantEffect(EffectEngine * owner, std::string_view name) :
        Effect(owner, name),
        mCurrentColor(prism::CieXYZ::Zero()),
        mTargetColor(prism::CieXYZ::Zero()),
        mConverter(prism::rgbToXyzTransformationMatrix(prism::rec2020()))
    {
        addProperty("color", [this](std::string_view text) {
            if (text.size() >= 7) {
                prism::RGB color;
                for (int i = 0; i < 3; ++i) {
                    if (auto value = tryParse<int>(text.substr(1 + 2 * i, 2), 16)) {
                        color[i] = *value / 255.0f;
                    }
                }
                color = prism::linearizeSRGB(color);
                mTargetColor.head<3>() = mConverter * color.head<3>();
            }
        });
        addProperty("whiteness", [this](std::string_view text) {
            if (auto value = tryParse<float>(text)) {
                mTargetColor[3] = std::clamp<float>(*value, 0.0f, 1.0f);
            }
        });
    }

    void ConstantEffect::update(float timeStep)
    {
        mCurrentColor = mCurrentColor * 0.98f + mTargetColor * 0.02f;
        mGenerator.color(mCurrentColor);
    }

    Generator * ConstantEffect::generator(Location const & location)
    {
        return &mGenerator;
    };
}
