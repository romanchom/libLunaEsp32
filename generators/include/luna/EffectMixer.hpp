#pragma once

#include "Effect.hpp"
#include "InterpolatingGenerator.hpp"

#include <luna/ValueProperty.hpp>

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

        std::unique_ptr<Generator> generator(Time const & t) override;
        void switchTo(Effect * effect);

        void enabled(bool state);
        bool enabled() const;

        std::vector<AbstractProperty *> properties() final;
    private:

        Observer * mObserver;

        std::deque<Effect *> mEffects;

        bool mEnabled;
        float mEnabledPercentage;
        float mFadeProgress;

        ValueProperty<float, ValidPositive> mBrightness;
        ValueProperty<float, ValidPositive> mFadeDuration;
    };
}
