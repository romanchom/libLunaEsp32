#pragma once

#include <luna/Configurable.hpp>

#include <initializer_list>
#include <string>

namespace luna
{
    struct Effect;

    struct EffectSet : Configurable
    {
        explicit EffectSet(std::initializer_list<Effect *> effects);
        Effect * find(std::string const & name);
        std::vector<Configurable *> children() override;

    private:
        std::vector<Effect *> mEffects;
    };
}
