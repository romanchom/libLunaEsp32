#pragma once

#include "StrandConfiguration.hpp"

#include <cstdint>
#include <cstddef>

namespace luna
{
namespace esp32
{

class Strand
{
protected:
    explicit Strand(StrandConfiguration const & configuration) :
        mConfiguration(configuration)
    {}

public:
    virtual ~Strand() = default;
    StrandConfiguration const & configuration() const noexcept { return mConfiguration; }

protected:
    StrandConfiguration mConfiguration;
};

}
}
