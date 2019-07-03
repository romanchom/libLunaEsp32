#pragma once

#include "ElectricalLoad.hpp"
#include "GPIO.hpp"
#include "PWM.hpp"

#include <map>

namespace luna::esp32
{
    struct ATX : private LoadObserver
    {
        explicit ATX(PWMTimer * timer, int powerEnablePin, int dummyLoadPin, float dummyResistance, float ratio12RailTo5Rail);

        void observe5V(std::initializer_list<ElectricalLoad *> loads);
        void observe12V(std::initializer_list<ElectricalLoad *> loads);
        void balanceLoad();
        void enabled(bool value);
    private:
        void currentDrawChanged(ElectricalLoad * emitter, float current) final;

        enum Rail {
            volts5,
            volts12,
            size,
        };

        struct LoadProperties
        {
            Rail rail;
            float currentDraw;
        };

        luna::esp32::GPIO mPowerEnable;
        PWM mDummyPWM;

        std::map<ElectricalLoad *, LoadProperties> mLoadProperties;

        float mMaxDummyCurrent;
        float mRatio12RailTo5Rail;
    };
}
