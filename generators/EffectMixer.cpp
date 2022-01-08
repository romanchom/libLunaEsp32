#include "EffectMixer.hpp"

#include <algorithm>
#include <cmath>

namespace luna
{
    EffectMixer::EffectMixer() :
        mObserver(nullptr),
        mEnabled(false),
        mEnabledPercentage(0.0),
        mFadeProgress(0.0f),
        mBrightness(1.0f),
        mFadeDuration(1.0f)
    {}

    std::unique_ptr<Generator> EffectMixer::generator(Time const & t)
    {
        if (mEnabled) {
            if (mEnabledPercentage < 1.0f) {
                mEnabledPercentage = std::min(mEnabledPercentage + t.delta, 1.0f);
            }
        } else {
            if (mEnabledPercentage > 0.0f) {
                mEnabledPercentage -= t.delta;
                if (mObserver && mEnabledPercentage <= 0.0f) {
                    mEnabledPercentage = 0.0f;
                    mObserver->enabledChanged(false);
                }
            }
        }

        if (mEffects.size() >= 2) {
            mFadeProgress += t.delta;
            if (mFadeProgress >= mFadeDuration.get()) {
                mEffects.pop_front();
                mFadeProgress -= mFadeDuration.get();

                if (mEffects.size() == 1) {
                    mFadeProgress = 0.0f;
                }
            }
        }

        auto const brightness = std::clamp(mBrightness.get(), 0.0f, 1.0f) * mEnabledPercentage * mEnabledPercentage;
        auto const x = (mFadeDuration.get() > 0.0f) ? (mFadeProgress / mFadeDuration.get()) : 1.0f;

        return std::make_unique<InterpolatingGenerator>(
            mEffects[0]->generator(t),
            brightness * (1.0f - x),
            (mEffects.size() >= 2) ? mEffects[1]->generator(t) : nullptr,
            brightness * x
        );
    }

    void EffectMixer::switchTo(Effect * effect)
    {
        if (mEffects.size() >= 3) {
            mEffects.back() = effect;
        } else if ((mEffects.size() >= 1 && mEffects.back() != effect) || mEffects.size() == 0) {
            mEffects.emplace_back(effect);
        }
    }

    void EffectMixer::enabled(bool state)
    {
        if (mEnabled == state) {
            return;
        }

        mEnabled = state;
        if (mObserver && mEnabled && mEnabledPercentage == 0.0f) {
            mObserver->enabledChanged(true);
        }
    }

    bool EffectMixer::enabled() const
    {
        return mEnabled;
    }

    void EffectMixer::setObserver(EnabledObserver * observer)
    {
        mObserver = observer;
        if (mObserver) {
            mObserver->enabledChanged(mEnabled);
        }
    }

    std::vector<std::tuple<std::string, AbstractProperty *>> EffectMixer::properties()
    {
        return {
            {"brightness", &mBrightness},
            {"fadeDuration", &mFadeDuration}
        };
    }
}
