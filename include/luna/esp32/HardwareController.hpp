#pragma once

#include "Strand.hpp"

#include <vector>
#include <memory>
#include <functional>

namespace luna {
namespace esp32 {

class HardwareController
{
public:
    explicit HardwareController(std::vector<std::unique_ptr<StrandBase>> strands);
    ~HardwareController();

    std::vector<StrandBase *> strands() const;

    void enabled(bool value);

    void onEnabled(std::function<void(bool)> value);
private:
    std::vector<std::unique_ptr<StrandBase>> mStrands;
    std::function<void(bool)> mOnEnabled;
};

}
}
