#include "StrandWS281x.hpp"

#include <luna/Generator.hpp>

#include <algorithm>
#include <cstring>
#include <cassert>

namespace luna {
    StrandWS281x::StrandWS281x(Location const & location, WS281xDriver * driver, size_t size, size_t offset, prism::RGBColorSpace const & colorSpace) :
        Strand(location),
        mDriver(driver),
        mSize(size),
        mOffset(offset),
        mCieToRgbMatrix(prism::rgbToXyzTransformationMatrix(colorSpace).inverse() * 255.0f),
        mError(0, 0, 0)
    {
        assert(mOffset + mSize <= mDriver->size() / 3);
    }

    void StrandWS281x::rawBytes(std::byte const * data, size_t size)
    {
        assert(mOffset * 3 + size <= mDriver->size());

        memcpy(mDriver->data() + mOffset * 3, data, size);
    }

    proto::Format StrandWS281x::format() const noexcept
    {
        return proto::Format::RGB8;
    }

    size_t StrandWS281x::pixelCount() const noexcept
    {
        return mSize;
    }

    void StrandWS281x::fill(Generator * generator)
    {
        using RGB = Eigen::Matrix<prism::ColorScalar, 3, 1>;

        auto destination = reinterpret_cast<Eigen::Matrix<uint8_t, 3, 1> *>(mDriver->data() + mOffset * 3);
        float const step = 1.0f / float(pixelCount() - 1);
        float constexpr maxError = 255.0f * 3;

        for (size_t i = 0; i < pixelCount(); ++i) {
            auto color = generator->generate(i * step);

            RGB corrected = mCieToRgbMatrix * color.head<3>() + mError;
            RGB rounded = corrected.array().round().max(0).min(255);

            mError = (corrected - rounded).cwiseMin(maxError).cwiseMax(-maxError);

            destination[i] = rounded.cast<uint8_t>();
        }
    }

    StrandWS2811::StrandWS2811(Location const & location, WS281xDriver * driver, size_t size, size_t offset) :
        StrandWS281x(location, driver, size, offset, colorSpace())
    {}

    prism::RGBColorSpace StrandWS2811::colorSpace() const noexcept
    {
        return {
            { 0.28623f, 0.27455f },
            { 0.13450f, 0.04598f },
            { 0.68934f, 0.31051f },
            { 0.13173f, 0.77457f },
        };
    }

    StrandWS2812::StrandWS2812(Location const & location, WS281xDriver * driver, size_t size, size_t offset) :
        StrandWS281x(location, driver, size, offset, colorSpace())
    {}

    prism::RGBColorSpace StrandWS2812::colorSpace() const noexcept
    {
        return {
            { 0.28623f, 0.27455f },
            { 0.13173f, 0.77457f },
            { 0.68934f, 0.31051f },
            { 0.13450f, 0.04598f },
        };
    }
}
