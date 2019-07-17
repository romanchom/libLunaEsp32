#pragma once

#include "Effect.hpp"
#include "Configurable.hpp"

#include <luna/InterpolatingGenerator.hpp>

#include <deque>

namespace luna::mqtt
{
    struct EffectMixer : Configurable
    {
        explicit EffectMixer();

        void configure(Topic const & topic, std::string_view payload) final;

        void update(float timeStep);
        Generator * generator(Location const & location);
        void switchTo(Effect * effect);

    private:
        std::deque<Effect *> mEffects;

        float mTransitionProgress;

        InterpolatingGenerator mGenerator;
    };
}
