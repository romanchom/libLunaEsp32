#pragma once

#include "Effect.hpp"
#include "Configurable.hpp"

#include <luna/InterpolatingGenerator.hpp>

#include <deque>

namespace luna::mqtt
{
    struct EffectMixer : Configurable
    {
        struct Observer
        {
            virtual void enabledChanged(bool enabled) = 0;
        protected:
            ~Observer() = default;
        };

        explicit EffectMixer(Observer * observer);

        void configure(Topic const & topic, std::string_view payload) final;

        void update(float timeStep);
        Generator * generator(Location const & location);
        void switchTo(Effect * effect);

        void enabled(bool state);

    private:
        Observer * mObserver;

        std::deque<Effect *> mEffects;

        bool mEnabled;
        float mBrightness;
        float mEnabledPercentage;
        float mTransitionDuration;
        float mTransitionProgress;

        InterpolatingGenerator mGenerator;
    };
}
