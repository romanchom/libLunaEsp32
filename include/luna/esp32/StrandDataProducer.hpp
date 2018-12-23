#pragma once

#include "StrandConfiguration.hpp"
#include "RGB.hpp"

namespace luna
{
namespace esp32
{

class StrandDataProducer
{
public:
    virtual ~StrandDataProducer() = default;
    virtual void produceRGB(StrandConfiguration const & config, RGB * rgb) const = 0;
};

}
}