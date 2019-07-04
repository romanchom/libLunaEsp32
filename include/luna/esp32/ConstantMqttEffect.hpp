#pragma once

#include "MqttEffect.hpp"
#include "ConstantGenerator.hpp"

#include <prism/Prism.hpp>

#include <charconv>

namespace luna::esp32
{
    struct ConstantMqttEffect : MqttEffect
    {
        ConstantMqttEffect() :
            mCurrentColor(prism::CieXYZ::Zero()),
            mTargetColor(prism::CieXYZ::Zero()),
            mConverter(prism::rgbToXyzTransformationMatrix(prism::rec2020()))
        {}

        void update(float timeStep) final
        {
            mCurrentColor = mCurrentColor * 0.97f + mTargetColor * 0.03f;
            mGenerator.color(mCurrentColor);
        }

        Generator const * generator() const
        {
            return &mGenerator;
        };

        void configure(MqttTopic const & topic, void * data, size_t size)
        {
            std::string_view text(static_cast<char *>(data), size);
            if (topic[3].str() == "color") {
                if (size >= 7) {
                    int value;
                    char const * p = text.begin() + 1;
                    prism::RGB color;
                    for (int i = 0; i < 3; ++i) {
                        auto [np, error] = std::from_chars(p, p + 2, value, 16);
                        if (error != std::errc()) {
                            return;
                        }
                        color[i] = value / 255.0f;
                        p = np;
                    }
                    color = prism::linearizeSRGB(color);
                    mTargetColor.head<3>() = mConverter * color.head<3>();
                }
            } else if (topic[3].str() == "whiteness") {
                float value = atof(text.data());
                value = std::clamp<float>(value, 0.0f, 1.0f);
                mTargetColor[3] = value;
            }
        }
    private:
        ConstantGenerator mGenerator;
        prism::CieXYZ mCurrentColor;
        prism::CieXYZ mTargetColor;
        Eigen::Matrix<prism::ColorScalar, 3, 3> mConverter;
    };
}
