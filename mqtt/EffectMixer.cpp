#include "EffectMixer.hpp"

#include "Parse.hpp"

#include <algorithm>

namespace luna::mqtt
{
    EffectMixer::EffectMixer(EffectMixer::Observer * observer) :
        mObserver(observer),
        mEnabled(false),
        mBrightness(1.0f),
        mEnabledPercentage(0.0),
        mTransitionDuration(1.0f),
        mTransitionProgress(0.0f)
    {}

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

    Generator * EffectMixer::generator(Location const & location)
    {
        auto const brightness = mBrightness * mEnabledPercentage * mEnabledPercentage;
        auto const t = mTransitionProgress / mTransitionDuration;

        mGenerator.first(mEffects[0]->generator(location), brightness * (1.0f - t));
        
        if (mEffects.size() >= 2) {
            mGenerator.second(mEffects[1]->generator(location), brightness * t);
        } else {
            mGenerator.second(nullptr, 0.0f);
        }
        return &mGenerator;
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
        mEnabled = state;
        if (mEnabled && mEnabledPercentage == 0.0f) {
            mObserver->enabledChanged(true);
        }
    }

    void EffectMixer::configure(Topic const & topic, std::string_view payload)
    {
        auto property = topic[2].str();
        if (property == "brightness") {
            if (auto value = tryParse<float>(payload)) {
                mBrightness = std::clamp(*value, 0.0f, 1.0f);
            }
        } else if (property == "duration") {
            if (auto value = tryParse<float>(payload)) {
                mTransitionDuration = std::max(0.0f, *value);
            }
        }
    }
}
