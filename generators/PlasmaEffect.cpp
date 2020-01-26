#include "PlasmaEffect.hpp"

#include "PlasmaGenerator.hpp"

namespace luna
{
    PlasmaEffect::PlasmaEffect(std::string && name) :
        Effect(std::move(name)),
        mSaturation("saturation", 1.0f)
    {}

    std::unique_ptr<Generator> PlasmaEffect::generator(Time const & t)
    {
        return std::make_unique<PlasmaGenerator>(t.total, 0.5f, 1.0f);
    };

    std::vector<AbstractProperty *> PlasmaEffect::properties()
    {
        return {&mSaturation};
    }
}
