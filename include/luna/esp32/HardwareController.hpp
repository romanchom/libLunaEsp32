#pragma once

#include "StrandDataProducer.hpp"
#include "Strand.hpp"

#include <vector>
#include <memory>
#include <functional>

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

    void enabled(bool value);

    void onEnabled(std::function<void(bool)> value);
private:
    std::vector<std::unique_ptr<Strand>> mStrands;
    std::function<void(bool)> mOnEnabled;
};

}
}
