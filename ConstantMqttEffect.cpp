#include "luna/esp32/ConstantMqttEffect.hpp"

#include "luna/esp32/Parse.hpp"

namespace luna::esp32
{
    ConstantMqttEffect::ConstantMqttEffect() :
        mCurrentColor(prism::CieXYZ::Zero()),
        mTargetColor(prism::CieXYZ::Zero()),
        mConverter(prism::rgbToXyzTransformationMatrix(prism::rec2020()))
    {}

    void ConstantMqttEffect::update(float timeStep)
    {
        mCurrentColor = mCurrentColor * 0.98f + mTargetColor * 0.02f;
        mGenerator.color(mCurrentColor);
    }

    Generator * ConstantMqttEffect::generator()
    {
        return &mGenerator;
    };

    void ConstantMqttEffect::configure(MqttTopic const & topic, std::string_view payload)
    {
        auto & property = topic[3].str();
        if (property == "color") {
            if (payload.size() >= 7) {
                prism::RGB color;
                for (int i = 0; i < 3; ++i) {
                    if (auto value = tryParse<int>(payload.substr(1 + 2 * i, 2), 16)) {
                        color[i] = *value / 255.0f;
                    }
                }
                color = prism::linearizeSRGB(color);
                mTargetColor.head<3>() = mConverter * color.head<3>();
            }
        } else if (property == "whiteness") {
            if (auto value = tryParse<float>(payload)) {
                mTargetColor[3] = std::clamp<float>(*value, 0.0f, 1.0f);;
            }
        }
    }
}