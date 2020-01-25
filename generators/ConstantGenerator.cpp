#include "ConstantGenerator.hpp"

namespace luna
{
    ConstantGenerator::ConstantGenerator(prism::CieXYZ const & color) :
        mColor(color)
    {
        auto maximum = mColor.maxCoeff();
        if (maximum > 1.0f) {
            mColor /= maximum;
        }
    }

    void ConstantGenerator::location(Location const & location) {}

    prism::CieXYZ ConstantGenerator::generate(float ratio) const noexcept
    {
        return mColor;
    }
}
