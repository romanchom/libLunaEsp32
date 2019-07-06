#include "luna/esp32/PlasmaMqttEffect.hpp"

#include "luna/esp32/Parse.hpp"

namespace luna::esp32
{
    void PlasmaMqttEffect::update(float timeStep)
    {
        mGenerator.time(mGenerator.time() + timeStep);
    }

    Generator * PlasmaMqttEffect::generator()
    {
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