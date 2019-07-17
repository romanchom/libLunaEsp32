#pragma once

#include "Effect.hpp"

#include <luna/ConstantGenerator.hpp>

#include <prism/Prism.hpp>

namespace luna::mqtt
{
    struct ConstantEffect : Effect
    {
        explicit ConstantEffect(Service * owner, std::string const & name);
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
        void configure(Topic const & topic, std::string_view payload) final;

    private:
        ConstantGenerator mGenerator;
        prism::CieXYZ mCurrentColor;
        prism::CieXYZ mTargetColor;
        Eigen::Matrix<prism::ColorScalar, 3, 3> mConverter;
    };
}
