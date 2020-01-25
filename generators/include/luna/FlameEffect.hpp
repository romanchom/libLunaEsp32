#pragma once

#include "Effect.hpp"
#include "FlameGenerator.hpp"

namespace luna
{
    struct FlameEffect : Effect
    {
        explicit FlameEffect(std::string && name);
        void update(float timeStep) final;
        std::unique_ptr<Generator> generator() final;
    private:
        float mTime;
        float mTemperatureLow;
        float mTemperatureHigh;
        float mFrequency;
    };
}
