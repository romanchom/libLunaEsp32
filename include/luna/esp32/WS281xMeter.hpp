#pragma once

#include "ElectricalLoad.hpp"

#include <WS281xDriver.hpp>

namespace luna::esp32
{
    struct WS281xMeter : ElectricalLoad
    {
        explicit WS281xMeter(WS281xDriver * driver, float currentPerDiode);
        void measure();
    private:
        WS281xDriver * mDriver;
        float mCurrentPerDiode;
    };
}
