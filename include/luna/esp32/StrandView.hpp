#pragma once

#include "luna/esp32/Strand.hpp"

#include <memory>

namespace luna::esp32 {

struct StrandView : Strand
{
    explicit StrandView(std::shared_ptr<Strand> child, StrandConfiguration configuration, size_t begin);
    void takeData(StrandDataProducer const * producer) override;
    void render() override;
protected:
    std::shared_ptr<Strand> mChild;
    size_t mBegin;
    size_t mSize;
};

}
