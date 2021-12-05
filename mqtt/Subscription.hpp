#pragma once

#include "Topic.hpp"

namespace luna::mqtt
{
    class Client;

    struct Subscription
    {
        explicit Subscription(Topic && topic) :
            mTopic(std::move(topic))
        {}
        virtual ~Subscription();

        Topic const & topic() const { return mTopic; }
        virtual void deliver(std::string_view text) = 0;

    private:
        Topic mTopic;
    };
}
