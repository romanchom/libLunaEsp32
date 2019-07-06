#pragma once

#include "Strand.hpp"

#include <prism/Prism.hpp>

#include <WS281xDriver.hpp>

namespace luna::esp32 {
    struct StrandWS281x : Strand
    {
        size_t pixelCount() const noexcept final;
        proto::Format format() const noexcept final;
        void rawBytes(std::byte const * data, size_t size) final;
        void fill(Generator * generator) final;
    protected:
        explicit StrandWS281x(Location const & location, WS281xDriver * driver, size_t size, size_t offset, prism::RGBColorSpace const & colorSpace);

        WS281xDriver * mDriver;
        size_t mSize;
        size_t mOffset;
        Eigen::Matrix<prism::ColorScalar, 3, 3> mCieToRgbMatrix;
        Eigen::Matrix<prism::ColorScalar, 3, 1> mError;
    };

    struct StrandWS2811 : StrandWS281x
    {
        explicit StrandWS2811(Location const & location, WS281xDriver * driver, size_t size, size_t offset = 0);
        prism::RGBColorSpace colorSpace() const noexcept final;
    };

    struct StrandWS2812 : StrandWS281x
    {
        explicit StrandWS2812(Location const & location, WS281xDriver * driver, size_t size, size_t offset = 0);
        prism::RGBColorSpace colorSpace() const noexcept final;
    };
}
