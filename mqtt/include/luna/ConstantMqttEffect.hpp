#pragma once

#include "MqttEffect.hpp"

#include <luna/ConstantGenerator.hpp>

#include <prism/Prism.hpp>

namespace luna
{
    struct ConstantMqttEffect : MqttEffect
    {
        explicit ConstantMqttEffect(MqttService * owner, std::string const & name);
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
        void configure(MqttTopic const & topic, std::string_view payload) final;

    private:
        ConstantGenerator mGenerator;
        prism::CieXYZ mCurrentColor;
        prism::CieXYZ mTargetColor;
        Eigen::Matrix<prism::ColorScalar, 3, 3> mConverter;
    };
}
