#pragma once

#include "MqttEffect.hpp"
#include "ConstantGenerator.hpp"

#include <prism/Prism.hpp>

namespace luna::esp32
{
    struct ConstantMqttEffect : MqttEffect
    {
        explicit ConstantMqttEffect();
        void update(float timeStep) final;
        Generator * generator() final;
        void configure(MqttTopic const & topic, std::string_view payload) final;

    private:
        ConstantGenerator mGenerator;
        prism::CieXYZ mCurrentColor;
        prism::CieXYZ mTargetColor;
        Eigen::Matrix<prism::ColorScalar, 3, 3> mConverter;
    };
}
