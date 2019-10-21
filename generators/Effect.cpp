#include "Effect.hpp"

#include "EffectEngine.hpp"

namespace luna
{
    Effect::Effect(EffectEngine * owner, std::string_view name) :
        Configurable(name)
    {
        owner->addEffect(name, this);
    }
}
