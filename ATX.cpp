#include "luna/esp32/ATX.hpp"

#include <algorithm>

namespace luna::esp32
{
    ATX::ATX(PWMTimer * timer, int powerEnablePin, int dummyLoadPin, float dummyResistance, float ratio12RailTo5Rail) :
        mPowerEnable(powerEnablePin),
        mDummyPWM(timer, dummyLoadPin),
        mMaxDummyCurrent(12.0f / dummyResistance),
        mRatio12RailTo5Rail(ratio12RailTo5Rail)
    {}

    void ATX::observe5V(std::initializer_list<ElectricalLoad *> loads)
    {
        for (auto load : loads) {
            mLoadProperties[load] = { Rail::volts5, 0.0f };
            load->powerObserver(this);
        }
    }

    void ATX::observe12V(std::initializer_list<ElectricalLoad *> loads)
    {
        for (auto load : loads) {
            mLoadProperties[load] = { Rail::volts12, 0.0f };
            load->powerObserver(this);
        }
    }

    void ATX::currentDrawChanged(ElectricalLoad * emitter, float current)
    {
        mLoadProperties[emitter].currentDraw = current;
    }

    void ATX::balanceLoad()
    {
        float currentDraws[Rail::size] = {};

        for (auto [device, properties] : mLoadProperties) {
            currentDraws[properties.rail] += properties.currentDraw;
        }

        float required12VLoad = currentDraws[Rail::volts5] * mRatio12RailTo5Rail;

        float dummyLoad = required12VLoad - currentDraws[Rail::volts12];

        float dutyCycle = dummyLoad / mMaxDummyCurrent;
        dutyCycle = std::clamp(dutyCycle, 0.0f, 1.0f);

        mDummyPWM.duty(dutyCycle);
    }

    void ATX::enabled(bool value)
    {
        mPowerEnable.out(value);
    }
}
