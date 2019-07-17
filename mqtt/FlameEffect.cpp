#include "FlameEffect.hpp"

#include "Parse.hpp"

namespace luna::mqtt
{
    void FlameEffect::update(float timeStep)
    {
        mGenerator.time(mGenerator.time() + timeStep);
    }

    Generator * FlameEffect::generator(Location const & location)
    {
        mGenerator.location(location);
        return &mGenerator;
    };

    void FlameEffect::configure(Topic const & topic, std::string_view payload)
    {
        auto & property = topic[3].str();
        if (property == "temperatureLow") {
            if (auto value = tryParse<float>(payload)) {
                mGenerator.temperatureLow(*value);
            }
        } else if (property == "temperatureHigh") {
            if (auto value = tryParse<float>(payload)) {
                mGenerator.temperatureHigh(*value);
            }
        } else if (property == "brightness") {
            if (auto value = tryParse<float>(payload)) {
                mGenerator.brightness(*value);
            }
        } else if (property == "frequency") {
            if (auto value = tryParse<float>(payload)) {
                mGenerator.frequency(*value);
            }
        }
    }
}
