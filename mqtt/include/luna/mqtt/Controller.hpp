#pragma once

#include "Client.hpp"

#include <luna/Configurable.hpp>
#include <luna/EventLoop.hpp>
#include <luna/TlsCredentials.hpp>
#include <luna/NetworkService.hpp>

#include <string_view>

namespace luna::mqtt
{
    struct Controller : NetworkService
    {
        explicit Controller(EventLoop * eventLoop, Configurable * effectEngine, std::string const & name, std::string const & address, TlsCredentials const & credentials);
        ~Controller() final;

        Client * client() { return &mClient; }
        EventLoop * eventLoop() { return mEventLoop; }
    private:
        void subscribeConfigurable(Configurable * configurable, std::string name);

        Client mClient;
        EventLoop * mEventLoop;
    };
}
