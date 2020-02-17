#pragma once

#include "Infrared.hpp"

#include <prism/Prism.hpp>

namespace luna
{
    struct EffectEngine;
    struct ConstantEffect;

    struct Ir40ButtonRemote : InfraredDemux
    {
        explicit Ir40ButtonRemote(EffectEngine * effectEngine, ConstantEffect * light);

        void demultiplex(uint8_t address, uint8_t command) final;
    private:
        void setChroma(prism::CieXY value);
        void multiplyLuma(float factor);

        EffectEngine * mEffectEngine;
        ConstantEffect * mLight;
        float mLastBrightness;
        float mLastWhiteness;
    };

}