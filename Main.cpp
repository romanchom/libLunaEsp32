#include "Main.hpp"

namespace luna
{
    Main::Main(Configuration const & config, HardwareController * controller) :
        mNetworkManager(config.network, controller),
        mWiFi(config.wifi)
    {
        mWiFi.observer(this);
        mWiFi.enabled(true);
    }

    void Main::connected()
    {
        mNetworkManager.enable();
    }

    void Main::disconnected()
    {
        mNetworkManager.disable();
    }
}
