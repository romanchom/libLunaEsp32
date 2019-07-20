#include "EffectMixer.hpp"

#include "Parse.hpp"

#include <algorithm>

namespace luna::mqtt
{
    EffectMixer::EffectMixer() :
        mBrightness(1.0f),
        mTransitionDuration(1.0f),
        mTransitionProgress(0.0f)
    {}

    void EffectMixer::update(float timeStep)
    {
        mEffects[0]->update(timeStep);
        if (mEffects.size() >= 2) {
            mEffects[1]->update(timeStep);

            mTransitionProgress += timeStep;
            if (mTransitionProgress >= mTransitionDuration) {
                mEffects.pop_front();
                mTransitionProgress -= mTransitionDuration;

                if (mEffects.size() == 1) {
                    mTransitionProgress = 0.0f;
                    // transition ended
                }
            }
        }
    }

    Generator * EffectMixer::generator(Location const & location)
    {
        if (mEffects.size() >= 2) {
            auto t = mTransitionProgress / mTransitionDuration;
            mGenerator.first(mEffects[0]->generator(location), (1.0f - t));
            mGenerator.second(mEffects[1]->generator(location), t);
            return &mGenerator;
        } else {
            return mEffects.front()->generator(location);
        }
    }

    void EffectMixer::switchTo(Effect * effect)
    {
        if (mEffects.size() >= 3) {
            mEffects.back() = effect;
        } else if ((mEffects.size() >= 1 && mEffects.back() != effect) || mEffects.size() == 0) {
            mEffects.emplace_back(effect);
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
