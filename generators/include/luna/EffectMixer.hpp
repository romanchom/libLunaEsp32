#pragma once

#include "Effect.hpp"
#include "InterpolatingGenerator.hpp"

#include <luna/Configurable.hpp>

#include <deque>

namespace luna
{
    struct EffectMixer// TODO : Configurable
    {
        struct Observer
        {
            virtual void enabledChanged(bool enabled) = 0;
        protected:
            ~Observer() = default;
        };

        explicit EffectMixer(Observer * observer);

        void update(float timeStep);
        Generator * generator(Location const & location);
        void switchTo(Effect * effect);

        void enabled(bool state);
        bool enabled() const;

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
