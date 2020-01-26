#pragma once

#include "Effect.hpp"

#include <luna/ValueProperty.hpp>

namespace luna
{
    struct PlasmaEffect : Effect
    {
        explicit PlasmaEffect(std::string && name);
        std::unique_ptr<Generator> generator(Time const & t) final;
        std::vector<AbstractProperty *> properties() final;
        
    private:
        ValueProperty<float, ValidPositive> mSaturation;
    };
}
