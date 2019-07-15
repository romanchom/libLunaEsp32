#include "EffectMixer.hpp"
#include <iostream>
namespace luna
{
    EffectMixer::EffectMixer() :
        mTransitionProgress(0.0f)
    {}

    void EffectMixer::update(float timeStep)
    {
        mEffects[0]->update(timeStep);
        if (mEffects.size() >= 2) {
            mEffects[1]->update(timeStep);

            mTransitionProgress += timeStep;
            if (mTransitionProgress >= 1.0f) {
                mEffects.pop_front();
                mTransitionProgress -= 1.0f;

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
            mGenerator.first(mEffects[0]->generator(location), 1.0f - mTransitionProgress);
            mGenerator.second(mEffects[1]->generator(location), mTransitionProgress);
            return &mGenerator;
        } else {
            return mEffects.front()->generator(location);
        }
    }

    void EffectMixer::switchTo(MqttEffect * effect)
    {
        if (mEffects.size() >= 3) {
            mEffects.back() = effect;
        } else if ((mEffects.size() >= 1 && mEffects.back() != effect) || mEffects.size() == 0) {
            mEffects.emplace_back(effect);
        }
    }
}
