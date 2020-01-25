#pragma once

#include "Effect.hpp"

#include "PlasmaGenerator.hpp"

namespace luna
{
    struct PlasmaEffect : Effect
    {
        explicit PlasmaEffect(std::string && name);
        void update(float timeStep) final;
        std::unique_ptr<Generator> generator() final;
    private:
        float mTime;
    };
}
