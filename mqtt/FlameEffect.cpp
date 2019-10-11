#include "FlameEffect.hpp"

#include "Parse.hpp"

namespace luna::mqtt
{
    FlameEffect::FlameEffect(Service * owner, std::string_view name) :
        Effect(owner, name)
    {
        addProperty("temperatureLow", [this](std::string_view text) {
            if (auto value = tryParse<float>(text)) {
                mGenerator.temperatureLow(*value);
            }
        });
        addProperty("temperatureHigh", [this](std::string_view text) {
            if (auto value = tryParse<float>(text)) {
                mGenerator.temperatureHigh(*value);
            }
        });
        addProperty("brightness", [this](std::string_view text) {
            if (auto value = tryParse<float>(text)) {
                mGenerator.brightness(*value);
            }
        });
        addProperty("frequency", [this](std::string_view text) {
            if (auto value = tryParse<float>(text)) {
                mGenerator.frequency(*value);
            }
        });
    }

    void FlameEffect::update(float timeStep)
    {
        mGenerator.time(mGenerator.time() + timeStep);
    }

    Generator * FlameEffect::generator(Location const & location)
    {
        mGenerator.location(location);
        return &mGenerator;
    };
}
