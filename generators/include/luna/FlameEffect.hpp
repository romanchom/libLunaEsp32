#pragma once

#include "Effect.hpp"
#include "FlameGenerator.hpp"

#include <luna/ValueProperty.hpp>

namespace luna
{
    struct FlameEffect : Effect
    {
        explicit FlameEffect(std::string && name);
        std::unique_ptr<Generator> generator(Time const & t) final;
        std::vector<AbstractProperty *> properties() final;

    private:
        ValueProperty<float, ValidPositive> mTemperatureLow;
        ValueProperty<float, ValidPositive> mTemperatureHigh;
        ValueProperty<float, ValidPositive> mFrequency;
    };
}
