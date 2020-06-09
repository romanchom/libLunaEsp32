#pragma once

#include "Effect.hpp"
#include "InterpolatingGenerator.hpp"

#include <luna/ValueProperty.hpp>

#include <deque>

namespace luna
{
    struct EnabledObserver
    {
        virtual void enabledChanged(bool enabled) = 0;
    protected:
        ~EnabledObserver() = default;
    };

    struct EffectMixer : Effect
    {
        explicit EffectMixer();

        std::unique_ptr<Generator> generator(Time const & t) override;
        void switchTo(Effect * effect);

        void enabled(bool state);
        bool enabled() const;

        void setObserver(EnabledObserver * observer);

        std::vector<AbstractProperty *> properties() final;
    private:
        EnabledObserver * mObserver;

        std::deque<Effect *> mEffects;

        bool mEnabled;
        float mEnabledPercentage;
        float mFadeProgress;

        ValueProperty<float, ValidPositive> mBrightness;
        ValueProperty<float, ValidPositive> mFadeDuration;
    };
}
