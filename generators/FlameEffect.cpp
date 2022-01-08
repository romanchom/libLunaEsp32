#include "FlameEffect.hpp"

namespace luna
{
    FlameEffect::FlameEffect() :
        mTemperatureLow(1500.0f),
        mTemperatureHigh(3000.0f),
        mFrequency(1.0f)
    {}

    std::unique_ptr<Generator> FlameEffect::generator(Time const & t)
    {
        return std::make_unique<FlameGenerator>(t.total, mTemperatureLow, mTemperatureHigh, mFrequency);
    };

    std::vector<std::tuple<std::string, AbstractProperty *>> FlameEffect::properties()
    {
        return {
            {"tempLow", &mTemperatureLow},
            {"tempHigh", &mTemperatureHigh},
            {"frequency", &mFrequency}
        };
    }

}
