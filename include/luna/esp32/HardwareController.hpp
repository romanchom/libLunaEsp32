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
    virtual ~HardwareController() = default;

    virtual std::vector<Strand *> strands() = 0;
    virtual void enabled(bool value) = 0;
    virtual void update() = 0;
};

}
}
