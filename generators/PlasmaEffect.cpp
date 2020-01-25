#include "PlasmaEffect.hpp"

#include <luna/Parse.hpp>

namespace luna
{
    PlasmaEffect::PlasmaEffect(std::string && name) :
        Effect(std::move(name)),
        mTime(0.0f)
    {
        // addProperty("saturation", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mGenerator.saturation(*value);
        //     }
        // });
        // addProperty ("brightness", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mGenerator.brightness(*value);
        //     }
        // });
    }

    void PlasmaEffect::update(float timeStep)
    {
        mTime += timeStep;
    }

    std::unique_ptr<Generator> PlasmaEffect::generator()
    {
        return std::make_unique<PlasmaGenerator>(mTime, 0.5f, 1.0f);
    };
}
