#include "EffectMixer.hpp"

#include <algorithm>

namespace luna
{
    EffectMixer::EffectMixer(EffectMixer::Observer * observer) :
        Effect("mixer"),
        mObserver(observer),
        mEnabled(false),
        mBrightness(1.0f),
        mEnabledPercentage(0.0),
        mTransitionDuration(1.0f),
        mTransitionProgress(0.0f)
    {
        // addProperty("brightness", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mBrightness = std::clamp(*value, 0.0f, 1.0f);
        //     }
        // });
        // addProperty("duration", [this](std::string_view text) {
        //     if (auto value = tryParse<float>(text)) {
        //         mTransitionDuration = std::max(0.0f, *value);
        //     }
        // });
    }

    void EffectMixer::update(float timeStep)
    {
        if (mEnabled) {
            if (mEnabledPercentage < 1.0f) {
                mEnabledPercentage = std::min(mEnabledPercentage + timeStep, 1.0f);
            }
        } else {
            if (mEnabledPercentage > 0.0f) {
                mEnabledPercentage -= timeStep;
                if (mEnabledPercentage <= 0.0f) {
                    mEnabledPercentage = 0.0f;
                    mObserver->enabledChanged(false);
                }
            }
        }

        mEffects[0]->update(timeStep);
        if (mEffects.size() >= 2) {
            mEffects[1]->update(timeStep);

            mTransitionProgress += timeStep;
            if (mTransitionProgress >= mTransitionDuration) {
                mEffects.pop_front();
                mTransitionProgress -= mTransitionDuration;

                if (mEffects.size() == 1) {
                    mTransitionProgress = 0.0f;
                }
            }
        }
    }

    std::unique_ptr<Generator> EffectMixer::generator()
    {
        auto const brightness = mBrightness * mEnabledPercentage * mEnabledPercentage;
        auto const t = mTransitionProgress / mTransitionDuration;

        return std::make_unique<InterpolatingGenerator>(
            mEffects[0]->generator(),
            brightness * (1.0f - t),
            (mEffects.size() >= 2) ? mEffects[1]->generator() : nullptr,
            brightness * t
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
        if (mEnabled && mEnabledPercentage == 0.0f) {
            mObserver->enabledChanged(true);
        }
    }

    bool EffectMixer::enabled() const
    {
        return mEnabled;
    }
}
