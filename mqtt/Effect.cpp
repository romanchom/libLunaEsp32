#include "Effect.hpp"

#include "Service.hpp"

namespace luna::mqtt
{
    Effect::Effect(Service * owner, std::string_view name) :
        Configurable(name)
    {
        owner->addEffect(name, this);
    }
}
