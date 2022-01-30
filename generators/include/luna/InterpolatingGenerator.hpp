#pragma once

#include <luna/Generator.hpp>

#include <memory>

namespace luna
{
    struct InterpolatingGenerator : Generator
    {
        explicit InterpolatingGenerator(std::unique_ptr<Generator> first, float firstRatio, std::unique_ptr<Generator> second, float secondRatio);
        void location(Location const & value) final;
        prism::CieXYZ generate(float ratio) const noexcept final;
        float whiteTemperature() const final;

    protected:
        std::unique_ptr<Generator> mFirst;
        std::unique_ptr<Generator> mSecond;
        float mFirstRatio;
        float mSecondRatio;
    };
}
