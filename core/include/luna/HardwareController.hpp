#pragma once

#include "Strand.hpp"

#include <vector>

namespace luna {
    struct HardwareController
    {
        virtual std::vector<Strand *> strands() = 0;
        virtual void enabled(bool value) = 0;
        virtual void update() = 0;
    protected:
        ~HardwareController() = default;
    };
}
