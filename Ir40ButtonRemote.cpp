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
        case colorOn:
            mEffectEngine->enabled() = true;
            break;
            
        // case white:
        //     setColor(prism::CieXY{0.968785f, 1.121183f});
        //     break;
        // case warmWhite0:
        //     setColor(prism::CieXY{});
        //     break;
        // case warmWhite1:
        //     setColor(prism::CieXY{});
        //     break;
        // case coolWhite0:
        //     setColor(prism::CieXY{});
        //     break;
        // case coolWhite1:
        //     setColor(prism::CieXY{});
        //     break;

        
        // case red:
        //     setColor(prism::CieXY{});
        //     break;
        // case redOrange:
        //     setColor(prism::CieXY{});
        //     break;
        // case orange:
        //     setColor(prism::CieXY{});
        //     break;
        // case yellow:
        //     setColor(prism::CieXY{});
        //     break;
        // case yellowOrange:
        //     setColor(prism::CieXY{});
        //     break;
        // case yellowGreen:
        //     setColor(prism::CieXY{});
        //     break;
        // case green:
        //     setColor(prism::CieXY{});
        //     break;
        // case turqoise:
        //     setColor(prism::CieXY{});
        //     break;
        // case cyan:
        //     setColor(prism::CieXY{});
        //     break;
        // case someBlue:
        //     setColor(prism::CieXY{});
        //     break;
        // case blue:
        //     setColor(prism::CieXY{});
        //     break;
        // case skyBlue:
        //     setColor(prism::CieXY{});
        //     break;
        // case purple:
        //     setColor(prism::CieXY{});
        //     break;
        // case magenta:
        //     setColor(prism::CieXY{});
        //     break;
        // case pink:
        //     setColor(prism::CieXY{});
        //     break;

        }
    }

    void Ir40ButtonRemote::setColor(prism::CieXY value)
    {
        mLight->color() = prism::CieXYZ(value.x(), 1.0f, value.y(), 0.0f);
    }
}