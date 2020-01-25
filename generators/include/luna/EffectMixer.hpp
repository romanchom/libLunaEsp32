#pragma once

#include "Effect.hpp"
#include "InterpolatingGenerator.hpp"

#include "Effect.hpp"

#include <deque>

namespace luna
{
    struct EffectMixer : Effect
    {
        struct Observer
        {
            virtual void enabledChanged(bool enabled) = 0;
        protected:
            ~Observer() = default;
        };

        explicit EffectMixer(Observer * observer);

        void update(float timeStep) override;
        std::unique_ptr<Generator> generator() override;
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
    };
}
