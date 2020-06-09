#include "EffectSet.hpp"

#include "Effect.hpp"

namespace luna
{
    EffectSet::EffectSet(std::vector<Effect *> && effects) :
        Configurable("effects"),
        mEffects(std::move(effects))
    {}

    Effect * EffectSet::find(std::string const & name)
    {
        for (auto effect : mEffects) {
            if (effect->name() == name) {
                return effect;
            }
        }
        return nullptr;
    }

    std::vector<Configurable *> EffectSet::children()
    {
        return {mEffects.begin(), mEffects.end()};
    }
}
