#include "luna/esp32/ConstantGenerator.hpp"

namespace luna::esp32
{
    ConstantGenerator::ConstantGenerator() :
        mColor(0, 0, 0, 0)
    {}

    prism::CieXYZ ConstantGenerator::generate(float ratio, Location const & location) const noexcept
    {
        return mColor;
    }

    void ConstantGenerator::color(prism::CieXYZ const & value)
    {
        mColor = value;
    }
}
