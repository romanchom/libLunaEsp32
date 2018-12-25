#pragma once

#include "StrandDataProducer.hpp"
#include "Strand.hpp"

#include <vector>
#include <memory>

namespace luna {
namespace esp32 {

class HardwareController
{
public:
    explicit HardwareController();
    ~HardwareController();
    std::vector<std::unique_ptr<Strand>> & strands()
    {
        return mStrands;
    }

    std::vector<std::unique_ptr<Strand>> const& strands() const
    {
        return mStrands;
    }

    void setAll(StrandDataProducer const * producer);
private:
    std::vector<std::unique_ptr<Strand>> mStrands;
};

}
}
