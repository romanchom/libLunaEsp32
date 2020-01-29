#include "Ir40ButtonRemote.hpp"

#include <luna/ConstantEffect.hpp>
#include <luna/EffectEngine.hpp>

namespace luna
{
    namespace
    {
        enum Code
        {
            fade7               = 0x8,
            flash               = 0x9,
            auto_               = 0x0a,
            slow                = 0xb,
            jump3               = 0x0c,
            fade3               = 0x0d,
            jump7               = 0x0e,
            quick               = 0xf,

            white25             = 0x10,
            white50             = 0x11,
            white75             = 0x12,
            white100            = 0x13,
            whiteBrightnessUp   = 0x14,
            whitePrightnessDown = 0x15,
            whiteOff            = 0x16,
            whiteOn             = 0x17,

            yellow              = 0x18,
            someBlue            = 0x19,
            pink                = 0x1a,
            coolWhite1          = 0x1b,
            yellowOrange        = 0x1c,
            cyan                = 0x1d,
            magenta             = 0x1e,
            coolWhite0          = 0x1f,

            colorOn             = 0x40,
            colorOff            = 0x41,

            white               = 0x44,
            blue                = 0x45,
            warmWhite0          = 0x48,
            skyBlue             = 0x49,
            warmWhite1          = 0x4c,
            purple              = 0x4d,
            orange              = 0x50,
            turqoise            = 0x51,
            redOrange           = 0x54,
            yellowGreen         = 0x55,
            red                 = 0x58,
            green               = 0x59,

            colorBrightnessUp   = 0x5c,
            colorBrightnessDown = 0x5d,

        };
    }

    Ir40ButtonRemote::Ir40ButtonRemote(EffectEngine * effectEngine, ConstantEffect * light) :
        mEffectEngine(effectEngine),
        mLight(light)
    {}

    void Ir40ButtonRemote::demultiplex(uint8_t address, uint8_t command)
    {
        if (address != 0x00) { return; }
        switch (command)
        {
        case colorOff:
            mEffectEngine->enabled() = false;
            break;
        case whiteOn:
        case colorOn:
            mEffectEngine->enabled() = true;
            break;
            
        case warmWhite0:
            mLight->color() = prism::temperature(3500.0f);
            break;
        case warmWhite1:
            mLight->color() = prism::temperature(5000.0f);
            break;
        case white:
            mLight->color() = prism::temperature(6500.0f);
            break;
        case coolWhite0:
            mLight->color() = prism::temperature(8000.0f);
            break;
        case coolWhite1:
            mLight->color() = prism::temperature(10000.0f);
            break;

        
        case red:
            setColor({0.7264257f, 0.2629310f});
            break;
        case redOrange:
            setColor({0.6363855f, 0.3563218f});
            break;
        case orange:
            setColor({0.6032129f,	0.3970307f});
            break;
        case yellowOrange:
            setColor({0.5629317f,	0.4281609f});
            break;
        case yellow:
            setColor({0.4586747f,	0.5359195f});
            break;
        case green:
            setColor({0.1790763f, 0.7897510f});
            break;
        case yellowGreen:
            setColor({0.3591566f, 0.6508621f});
            break;
        case turqoise:
            setColor({0.0155823f, 0.7586207f});
            break;
        case cyan:
            setColor({0.0061044f, 0.5598659f});
            break;
        case someBlue:
            setColor({0.0440161f, 0.3012452f});
            break;
        case blue:
            setColor({0.1293173f, 0.0450192f});
            break;
        case skyBlue:
            setColor({0.2098795f, 0.2294061f});
            break;
        case purple:
            setColor({0.1648594f, 0.0090996f});
            break;
        case magenta:
            setColor({0.3781124f, 0.1072797f});
            break;
        case pink:
            setColor({0.3710040f, 0.2246169f});
            break;


        case whiteOff:
            mLight->whiteness() = 0.0f;
            break;
        case white25:
            mLight->whiteness() = exp(-3.0f);
            break;
        case white50:
            mLight->whiteness() = exp(-2.0f);
            break;
        case white75:
            mLight->whiteness() = exp(-1.0f);
            break;
        case white100:
            mLight->whiteness() = 1.0f;
            break;
        case whiteBrightnessUp:
            mLight->whiteness() = std::clamp<float>(mLight->whiteness() * exp(0.1f), 0.0f, 1.0f);
            break;
        case whitePrightnessDown:
            mLight->whiteness() = std::clamp<float>(mLight->whiteness() * exp(-0.1f), 0.0f, 1.0f);
            break;
        }
    }

    void Ir40ButtonRemote::setColor(prism::CieXY value)
    {
        auto const Y = 1;
        auto const X = Y * value.x() / value.y();
        auto const Z = Y * (1 - value.x() - value.y()) / value.y();
        mLight->color() = prism::CieXYZ(X, Y, Z, 0.0f);
    }
}