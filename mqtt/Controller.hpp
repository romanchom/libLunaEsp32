#pragma once

#include "Client.hpp"
#include "Codec.hpp"

#include <luna/Configurable.hpp>
#include <luna/LunaInterface.hpp>
#include <luna/TlsCredentials.hpp>
#include <luna/NetworkService.hpp>

#include <string_view>

namespace luna::mqtt
{
    struct Controller : NetworkService
    {
        explicit Controller(LunaInterface * luna, Configurable * configurable, std::string const & name, std::string const & address, TlsCredentials const & credentials, float floatScale);
        ~Controller() final;

        void deliver(std::string_view topic, std::string_view data);
        void subscribe();

        Client * client() { return &mClient; }
        LunaInterface * luna() { return mLuna; }
    private:
        void subscribeConfigurable(std::string prefix, Configurable * configurable);
        void publishConfigurable(std::string prefix, Configurable * configurable);

        Client mClient;
        LunaInterface * mLuna;
        Configurable * mConfigurable;
        std::string mPrefix;
        std::vector<std::unique_ptr<AbstractProperty>> mProperties;
        Codec mCodec;
    };
}
