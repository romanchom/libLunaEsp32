#include "EffectSet.hpp"

#include "Effect.hpp"

namespace luna
{
    EffectSet::EffectSet(std::vector<std::tuple<std::string, Effect *>> && effects) :
        mEffects(std::move(effects))
    {}

    Effect * EffectSet::find(std::string const & name)
    {
        for (auto & [effectName, effect] : mEffects) {
            if (effectName == name) {
                return effect;
            }
        }
        return nullptr;
    }

    std::vector<std::tuple<std::string, Configurable *>> EffectSet::children()
    {
        return {mEffects.begin(), mEffects.end()};
    }
}
