#pragma once

namespace luna
{
    struct EventLoop;
    struct ControllerMux;

    struct LunaContext
    {
        EventLoop * mainLoop;
        ControllerMux * multiplexer;
    };
}