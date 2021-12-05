#pragma once

#include "Client.hpp"

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

        float floatScale() const { return mFloatScale; }

        Client * client() { return &mClient; }
        LunaInterface * luna() { return mLuna; }

        template<typename T>
        std::optional<T> parse(std::string_view text) const;
        template<typename T>
        std::string serialize(T const & value) const;
    private:
        void subscribeConfigurable(Configurable * configurable, std::string name);
        
        Client mClient;
        LunaInterface * mLuna;
        
        float const mFloatScale;
    };
}
