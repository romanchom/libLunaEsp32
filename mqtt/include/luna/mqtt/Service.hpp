#pragma once

#include "Client.hpp"

#include <luna/Configurable.hpp>
#include <luna/EventLoop.hpp>
#include <luna/EffectEngine.hpp>

#include <string_view>

namespace luna::mqtt
{
    struct Service
    {
        explicit Service(EventLoop * eventLoop, EffectEngine * effectEngine, std::string name, Client::Configuration const & configuration);

        Client * client() { return &mClient; }
        EventLoop * eventLoop() { return mEventLoop; }
    private:
        void subscribeConfigurable(Configurable * configurable, std::string name);

        Client mClient;
        EffectEngine * mEffectEngine;
        EventLoop * mEventLoop;
    };
}
