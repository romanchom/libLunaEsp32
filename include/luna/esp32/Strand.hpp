#pragma once

#include "StrandConfiguration.hpp"

#include <cstdint>
#include <cstddef>

namespace luna
{
namespace esp32
{

class StrandDataProducer;

class Strand
{
public:
    virtual ~Strand() = default;
    
    StrandConfiguration const & configuration() const noexcept
    {
        return mConfiguration;
    }
    
    virtual void takeData(StrandDataProducer const * producer) = 0;

protected:
    explicit Strand(StrandConfiguration const & configuration) :
        mConfiguration(configuration)
    {}

protected:
    StrandConfiguration mConfiguration;
};

}
}
