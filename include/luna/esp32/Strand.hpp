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
    virtual ~StrandBase() = default;
    virtual size_t pixelCount() const noexcept = 0;
    virtual proto::Format format() const noexcept = 0;
    virtual ColorSpace colorSpace() const noexcept = 0;
    virtual void render() {};
    virtual Location location() const noexcept { return {}; };
};

template<typename T>
struct Strand : StrandBase
{
    using StrandBase::StrandBase;
    virtual void setLight(T const * data, size_t size, size_t offset) = 0;
};

}
