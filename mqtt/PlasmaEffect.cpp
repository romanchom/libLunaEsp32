#include "PlasmaEffect.hpp"

#include "Parse.hpp"

namespace luna::mqtt
{
    void PlasmaEffect::update(float timeStep)
    {
        mGenerator.time(mGenerator.time() + timeStep);
    }

    Generator * PlasmaEffect::generator(Location const & location)
    {
        mGenerator.location(location);
        return &mGenerator;
    };

    void PlasmaEffect::configure(Topic const & topic, std::string_view payload)
    {
        auto property = topic[3].str();
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
