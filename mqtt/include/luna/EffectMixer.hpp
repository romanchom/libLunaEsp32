#pragma once

#include "MqttEffect.hpp"

#include <luna/InterpolatingGenerator.hpp>

#include <deque>

namespace luna
{
    struct EffectMixer : MqttEffect
    {
        explicit EffectMixer();

        void update(float timeStep) final;
        Generator * generator(Location const & location) final;
        void configure(MqttTopic const & topic, std::string_view payload) final {};

        void switchTo(MqttEffect * effect);

    private:
        std::deque<MqttEffect *> mEffects;

        float mTransitionProgress;

        InterpolatingGenerator mGenerator;
    };
}
