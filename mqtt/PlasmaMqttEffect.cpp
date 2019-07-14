#include "PlasmaMqttEffect.hpp"

#include "Parse.hpp"

namespace luna
{
    void PlasmaMqttEffect::update(float timeStep)
    {
        mGenerator.time(mGenerator.time() + timeStep);
    }

    Generator * PlasmaMqttEffect::generator(Location const & location)
    {
        mGenerator.location(location);
        return &mGenerator;
    };

    void PlasmaMqttEffect::configure(MqttTopic const & topic, std::string_view payload)
    {
        auto & property = topic[3].str();
        if (property == "saturation") {
            if (auto value = tryParse<float>(payload)) {
                mGenerator.saturation(*value);
            }
        } else if (property == "brightness") {
            if (auto value = tryParse<float>(payload)) {
                mGenerator.brightness(*value);
            }
        }
    }
}
