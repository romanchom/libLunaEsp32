#include "luna/esp32/StrandWS281x.hpp"
#include "luna/esp32/Generator.hpp"

#include <algorithm>
#include <cstring>
#include <cassert>

namespace luna::esp32 {
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

    void StrandWS281x::fill(Generator const * generator)
    {
        auto destination = reinterpret_cast<Eigen::Matrix<uint8_t, 3, 1> *>(mDriver->data() + mOffset * 3);

        using RGB = Eigen::Matrix<prism::ColorScalar, 3, 1>;

        float const step = 1.0f / float(pixelCount() - 1);
        auto const loc = location();

        for (size_t i = 0; i < pixelCount(); ++i) {
            auto color = generator->generate(i * step, loc);

            RGB corrected = mCieToRgbMatrix * color.head<3>() + mError;
            RGB rounded = corrected.array().round().cwiseMax(0).cwiseMin(255);

            mError = corrected - rounded;

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
