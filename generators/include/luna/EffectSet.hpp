#pragma once

#include <luna/Configurable.hpp>

#include <vector>
#include <string>

namespace luna
{
    struct Effect;

    struct EffectSet : Configurable
    {
        explicit EffectSet(std::vector<std::tuple<std::string, Effect *>> && effects);
        Effect * find(std::string const & name);
        std::vector<std::tuple<std::string, Configurable *>> children() override;

    private:
        std::vector<std::tuple<std::string, Effect *>> mEffects;
    };
}
