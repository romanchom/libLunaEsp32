#pragma once

#include "Strand.hpp"

#include <luna/proto/SetColor.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>
#include <limits>
#include <utility>

namespace luna::esp32
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
        float mCurrentPerPixel;
    };
}
