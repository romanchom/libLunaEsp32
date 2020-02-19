#pragma once

#include "Infrared.hpp"

#include <prism/Prism.hpp>

namespace luna
{
    struct EffectEngine;
    struct ConstantEffect;

    struct Ir40ButtonRemote : InfraredDemux
    {
        explicit Ir40ButtonRemote(EffectEngine * effectEngine, ConstantEffect * light, float increment);

        void demultiplex(uint8_t address, uint8_t command) final;
    private:
        void setChroma(prism::CieXY value);

        EffectEngine * mEffectEngine;
        ConstantEffect * mLight;
        float mIncrement;
        float mLastBrightness;
        float mLastWhiteness;
    };

}