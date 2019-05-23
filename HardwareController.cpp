#include "luna/esp32/HardwareController.hpp"

namespace luna {
namespace esp32 {

HardwareController::HardwareController(std::vector<std::unique_ptr<StrandBase>> strands) :
    mStrands(std::move(strands))
{}

HardwareController::~HardwareController() = default;

std::vector<StrandBase *> HardwareController::strands() const
{
    std::vector<StrandBase *> ret;
    ret.reserve(mStrands.size());
    
    for (auto & strand : mStrands) {
        ret.emplace_back(strand.get());
    }

    return ret;
}

void HardwareController::enabled(bool value)
{
    if (mOnEnabled) {
        mOnEnabled(value);
    }
}

void HardwareController::onEnabled(std::function<void(bool)> value)
{
    mOnEnabled = value;
}

}
}
