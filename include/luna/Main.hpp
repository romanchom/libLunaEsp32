#pragma once

#include "NetworkManager.hpp"
#include "WiFi.hpp"
#include "Configuration.hpp"

#include <luna/HardwareController.hpp>

namespace luna
{
    struct Main : private WiFi::Observer
    {
        explicit Main(Configuration const & config, HardwareController * controller);

    private:
        void connected() final;
        void disconnected() final;

        NetworkManager mNetworkManager;
        WiFi mWiFi;
    };
}
