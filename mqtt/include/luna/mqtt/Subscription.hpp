#pragma once

#include "Topic.hpp"

namespace luna::mqtt
{
    class Client;

    struct Subscription
    {
        explicit Subscription(Client * client, Topic && topic) :
            mClient(client),
            mTopic(std::move(topic))
        {}
        virtual ~Subscription();

        Topic const & topic() const { return mTopic; }
        virtual void deliver(std::string_view text) = 0;
    protected:
        void publish(std::string const & text);

    private:
        Client * mClient;
        Topic mTopic;
    };
}
