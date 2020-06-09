#pragma once

#include <luna/Configurable.hpp>

#include <vector>
#include <string>

namespace luna
{
    struct Effect;

    struct EffectSet : Configurable
    {
        explicit EffectSet(std::vector<Effect *> && effects);
        Effect * find(std::string const & name);
        std::vector<Configurable *> children() override;

    private:
        std::vector<Effect *> mEffects;
    };
}
