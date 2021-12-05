#pragma once

#include "Infrared.hpp"

#include <prism/Prism.hpp>

namespace luna
{
    struct EffectPlugin;
    struct ConstantEffect;

    struct Ir40ButtonRemote : InfraredDemux
    {
        explicit Ir40ButtonRemote(EffectPlugin * effectPlugin, ConstantEffect * light, float increment);

        void demultiplex(uint8_t address, uint8_t command) final;
    private:
        void setChroma(prism::CieXY value);

        EffectPlugin * mEffectPlugin;
        ConstantEffect * mLight;
        float mIncrement;
    };

}