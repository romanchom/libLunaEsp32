#pragma once

#include "MqttEffect.hpp"

#include <luna/InterpolatingGenerator.hpp>

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
        MqttEffect * mPrimary;
        MqttEffect * mSecondary;
        MqttEffect * mQueue;

        float mTransitionProgress;

        InterpolatingGenerator mGenerator;
    };
}