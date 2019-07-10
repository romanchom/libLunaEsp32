#include "FlameMqttEffect.hpp"

#include "Parse.hpp"

namespace luna
{
    void FlameMqttEffect::update(float timeStep)
    {
        mGenerator.time(mGenerator.time() + timeStep);
    }

    Generator * FlameMqttEffect::generator()
    {
        return &mGenerator;
    };

    void FlameMqttEffect::configure(MqttTopic const & topic, std::string_view payload)
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
