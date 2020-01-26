#include "WS281xMeter.hpp"

#include <cstdint>

namespace luna
{
    WS281xMeter::WS281xMeter(WS281xDriver * driver, float currentPerDiode) :
        mDriver(driver),
        mCurrentPerDiode(currentPerDiode / 255.0f)
    {}

    void WS281xMeter::measure()
    {
        uint32_t total = 0;
        auto const end = mDriver->data() + mDriver->size();
        for (auto it = mDriver->data(); it != end; ++it) {
            total += uint32_t(*it);
        }

        notify(total * mCurrentPerDiode);
    }
}
