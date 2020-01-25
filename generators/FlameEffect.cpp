#include "FlameEffect.hpp"

#include <luna/Parse.hpp>

namespace luna
{
    FlameEffect::FlameEffect(std::string && name) :
        Effect(std::move(name)),
        mTime(0.0f)
    {
        // addProperty("temperatureLow", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mGenerator.temperatureLow(*value);
        //     }
        // });
        // addProperty("temperatureHigh", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mGenerator.temperatureHigh(*value);
        //     }
        // });
        // addProperty("brightness", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mGenerator.brightness(*value);
        //     }
        // });
        // addProperty("frequency", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mGenerator.frequency(*value);
        //     }
        // });
    }

    void FlameEffect::update(float timeStep)
    {
        mTime += timeStep;
    }

    std::unique_ptr<Generator> FlameEffect::generator()
    {
        return std::make_unique<FlameGenerator>(mTime, 1500.0f, 3000.0f, 1.0f);
    };
}
