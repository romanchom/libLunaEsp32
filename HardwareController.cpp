#include "luna/esp32/HardwareController.hpp"

namespace luna {
namespace esp32 {

HardwareController::HardwareController() = default;

HardwareController::~HardwareController() = default;

void HardwareController::setAll(StrandDataProducer const * producer)
{
    for (auto & strand : mStrands) {
        strand->takeData(producer);
    }
}

}
}
