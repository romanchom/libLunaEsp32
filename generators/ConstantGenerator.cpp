#include "ConstantGenerator.hpp"

namespace luna
{
    ConstantGenerator::ConstantGenerator() :
        mColor(0, 0, 0, 0)
    {}

    prism::CieXYZ ConstantGenerator::generate(float ratio) const noexcept
    {
        return mColor;
    }

    void ConstantGenerator::color(prism::CieXYZ const & value)
    {
        mColor = value;
        auto maximum = mColor.maxCoeff();
        if (maximum > 1.0f) {
            mColor /= maximum;
        }
    }

    void ConstantGenerator::setup(Location const & location)
    {}
}
