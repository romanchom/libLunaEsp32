#pragma once

#include "MqttEffect.hpp"
#include "MqttConfigurable.hpp"

#include <luna/InterpolatingGenerator.hpp>

#include <deque>

namespace luna
{
    struct EffectMixer : MqttConfigurable
    {
        explicit EffectMixer();

        void configure(MqttTopic const & topic, std::string_view payload) final;

        void update(float timeStep);
        Generator * generator(Location const & location);
        void switchTo(MqttEffect * effect);

    private:
        std::deque<MqttEffect *> mEffects;

        float mTransitionProgress;

        InterpolatingGenerator mGenerator;
    };
}
