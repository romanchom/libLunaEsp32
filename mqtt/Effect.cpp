#include "Effect.hpp"

#include "Service.hpp"

namespace luna::mqtt
{
    Effect::Effect(Service * owner, std::string const & name)
    {
        owner->addEffect(name, this);
    }
}
