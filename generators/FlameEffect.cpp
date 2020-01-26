#include "FlameEffect.hpp"

namespace luna
{
    FlameEffect::FlameEffect(std::string && name) :
        Effect(std::move(name)),
        mTemperatureLow("tempLow", 1500.0f),
        mTemperatureHigh("tempHigh", 3000.0f),
        mFrequency("frequency", 1.0f)
    {}

    std::unique_ptr<Generator> FlameEffect::generator(Time const & t)
    {
        return std::make_unique<FlameGenerator>(t.total, mTemperatureLow, mTemperatureHigh, mFrequency);
    };

    std::vector<AbstractProperty *> FlameEffect::properties()
    {
        return {&mTemperatureLow, &mTemperatureHigh, &mFrequency};
    }

}
