#include "PlasmaEffect.hpp"

#include "Parse.hpp"

namespace luna::mqtt
{
    PlasmaEffect::PlasmaEffect(Service * owner, std::string_view name) :
        Effect(owner, name)
    {
        addProperty("saturation", [this](std::string_view text) {
            if (auto value = tryParse<float>(text)) {
                mGenerator.saturation(*value);
            }
        });
        addProperty ("brightness", [this](std::string_view text) {
            if (auto value = tryParse<float>(text)) {
                mGenerator.brightness(*value);
            }
        });
    }

    void PlasmaEffect::update(float timeStep)
    {
        mGenerator.time(mGenerator.time() + timeStep);
    }

    Generator * PlasmaEffect::generator(Location const & location)
    {
        mGenerator.location(location);
        return &mGenerator;
    };
}
