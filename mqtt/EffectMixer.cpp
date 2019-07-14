#include "EffectMixer.hpp"

namespace luna
{
    EffectMixer::EffectMixer() :
        mPrimary(nullptr),
        mSecondary(nullptr),
        mQueue(nullptr),
        mTransitionProgress(0.0f)
    {}

    void EffectMixer::update(float timeStep)
    {
        mPrimary->update(timeStep);
        if (mSecondary) {
            mSecondary->update(timeStep);

            mTransitionProgress += timeStep;
            if (mTransitionProgress >= 1.0f) {
                mPrimary = mSecondary;
                mSecondary = mQueue;
                mQueue = nullptr;
                mTransitionProgress -= 1.0f;

                if (!mSecondary) {
                    mTransitionProgress = 0.0f;
                    // transition ended
                }
            }
        }
    }

    Generator * EffectMixer::generator(Location const & location)
    {
        if (mSecondary) {
            mGenerator.first(mPrimary->generator(location), 1.0f - mTransitionProgress);
            mGenerator.second(mSecondary->generator(location), mTransitionProgress);
            return &mGenerator;
        } else {
            return mPrimary->generator(location);
        }
    }

    void EffectMixer::switchTo(MqttEffect * effect)
    {
        if (!mPrimary) {
            mPrimary = effect;
        } else if (!mSecondary && mPrimary != effect) {
            mSecondary = effect;
        } else if (mSecondary != effect) {
            mQueue = effect;
        }
    }
}