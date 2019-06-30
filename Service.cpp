#include "luna/esp32/Service.hpp"
#include "luna/esp32/ServiceManager.hpp"

namespace luna::esp32
{
    Service::Service() :
        mManager(nullptr)
    {}

    Service::~Service() = default;

    void Service::setManager(ServiceManager * manager)
    {
        mManager = manager;
    }
    
    void Service::serviceEnabled(bool enabled)
    {
        if (mManager) {
            mManager->serviceEnabled(this, enabled);
        }
    }
}
