#pragma once

#include "Location.hpp"
#include "Generator.hpp"

#include <prism/Prism.hpp>
#include <luna/proto/Format.hpp>

#include <cstddef>

namespace luna
{
    struct Strand
    {
        explicit Strand(Location const & location) :
            mLocation(location)
        {}
        virtual size_t pixelCount() const noexcept = 0;
        virtual proto::Format format() const noexcept = 0;
        virtual prism::RGBColorSpace colorSpace() const noexcept = 0;
        Location location() const noexcept { return mLocation; }
        virtual void rawBytes(std::byte const * data, size_t size) = 0;
        void acceptGenerator(Generator * generator)
        {
            generator->location(mLocation);
            fill(generator);
        }
    protected:
        ~Strand() = default;
    private:
        virtual void fill(Generator * generator) = 0;

        Location mLocation;
    };

}
