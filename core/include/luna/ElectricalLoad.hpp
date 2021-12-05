#pragma once

namespace luna
{
    struct ElectricalLoad;

    struct LoadObserver
    {
        virtual ~LoadObserver() = default;
        virtual void currentDrawChanged(ElectricalLoad * emitter, float current) = 0;
    };

    struct ElectricalLoad
    {
        explicit ElectricalLoad() :
            mObserver(nullptr)
        {}

        void observer(LoadObserver * value)
        {
            mObserver = value;
        }
    protected:
        void notify(float totalCurrent)
        {
            if (mObserver) {
                mObserver->currentDrawChanged(this, totalCurrent);
            }
        }

        LoadObserver * mObserver;
    };
}
