#pragma once

#include "ColorSpace.hpp"
#include "Location.hpp"

#include <luna/proto/Format.hpp>

#include <cstddef>
#include <cstdint>

namespace luna::esp32
{

struct StrandBase
{
    explicit StrandBase(Location location);
    virtual ~StrandBase() = default;
    virtual size_t pixelCount() const noexcept = 0;
    virtual proto::Format format() const noexcept = 0;
    virtual ColorSpace colorSpace() const noexcept = 0;
    virtual void render() = 0;
    Location location() const noexcept;

private:
    Location mLocation;
};

inline StrandBase::StrandBase(Location location) :
    mLocation(location)
{}

inline Location StrandBase::location() const noexcept
{
    return mLocation;
}

template<typename T>
struct Strand : StrandBase
{
    using StrandBase::StrandBase;
    virtual void setLight(T const * data, size_t size, size_t offset) = 0;
    proto::Format format() const noexcept final;
};

}
