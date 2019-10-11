#pragma once

#include "Effect.hpp"

#include <luna/ConstantGenerator.hpp>

#include <prism/Prism.hpp>

namespace luna::mqtt
{
    struct ConstantEffect : Effect
    {
        explicit ConstantEffect(Service * owner, std::string_view name);
        void update(float timeStep) final;
        Generator * generator(Location const & location) final;

    private:
        ConstantGenerator mGenerator;
        prism::CieXYZ mCurrentColor;
        prism::CieXYZ mTargetColor;
        Eigen::Matrix<prism::ColorScalar, 3, 3> mConverter;
    };
}
