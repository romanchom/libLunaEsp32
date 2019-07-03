#include "luna/esp32/WS281xMeter.hpp"

namespace luna::esp32
{

    explicit WS281xMeter::WS281xMeter(WS281xDriver * driver, float currentPerDiode) :
        mDriver(driver),
        mCurrentPerDiode(currentPerDiode / 255.0f)
    {}

    void WS281xMeter::measure()
    {
        uint32_t total = 0;
        auto const end = mDriver->data() + mDriver->size();
        for (auto it = mDriver->data(); it != end; ++it) {
            total = uint32_t(*it);
        }

        notify(total * currentPerDiode);
    }
}
