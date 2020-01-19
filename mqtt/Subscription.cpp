#include "Subscription.hpp"
#include "Client.hpp"

namespace luna::mqtt
{
    void Subscription::publish(std::string const & text)
    {
        mClient->publish(mTopic, text);
    }

    Subscription::~Subscription() = default;
}
