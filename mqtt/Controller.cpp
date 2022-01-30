#include "Controller.hpp"

#include <luna/Property.hpp>

#include <prism/Prism.hpp>

#include <esp_log.h>

#include <string>
#include <cstdint>
#include <optional>

static char const TAG[] = "MqttSvc";

namespace luna::mqtt
{
    struct Delivery
    {
        template<typename T>
        void operator()(Property<T> * property)
        {
            auto parsed = codec->parse<T>(data);
            if (parsed) {
                property->set(*parsed);
            }
        }

        Codec * codec;
        std::string_view data;
    };

    template<typename T>
    struct StatusProperty : Property<T>
    {
        explicit StatusProperty(Client * client, Codec * codec, std::string topic) :
            mClient(client),
            mCodec(codec),
            mTopic(std::move(topic))
        {}

    private:
        void set(T const & value) override
        {
            mClient->publish(mTopic, mCodec->serialize(value));
        }

        T getValue() const override {
            return T{};
        }

        Client * mClient;
        Codec * mCodec;
        std::string mTopic;
    };

    struct StatusPropertyFactory
    {
        template<typename T>
        void operator()(Property<T> * property) {
            auto prop = std::make_unique<StatusProperty<T>>(client, codec, std::move(topic));
            prop->bindTo(property);
            properties.emplace_back(std::move(prop));
        }

        Client * client;
        Codec * codec;
        std::string topic;
        std::vector<std::unique_ptr<AbstractProperty>> & properties;
    };

    bool tryDeliver(std::string_view topic, std::string const & name, AbstractProperty * prop, Delivery * delivery)
    {
        auto nextSeparator = topic.find('/');
        if (nextSeparator == std::string_view::npos
            || topic.substr(0, nextSeparator) != name
            || topic.substr(nextSeparator + 1) != "command")
        {
            return false;
        }

        TemplatedVisitor visitor(*delivery);
        prop->acceptVisitor(&visitor);
        return true;
    }

    bool tryDeliver(std::string_view topic, std::string const & name, Configurable * configurable, Delivery * delivery)
    {
        auto nextSeparator = topic.find('/');

        if (nextSeparator == std::string_view::npos || topic.substr(0, nextSeparator) != name) {
            return false;
        }

        auto nextTopic = topic.substr(nextSeparator + 1);

        for (auto & [name, prop] : configurable->properties())
        {
            if (tryDeliver(nextTopic, name, prop, delivery))
            {
                return true;
            }
        }

        for (auto & [name, child] : configurable->children()) {
            if (tryDeliver(nextTopic, name, child, delivery))
            {
                return true;
            }
        }
        return false;
    }


    Controller::Controller(LunaInterface * luna, Configurable * configurable, std::string const & address, TlsCredentials const & credentials, float floatScale) :
        mClient(address, credentials, this),
        mLuna(luna),
        mConfigurable(configurable),
        mPrefix(luna->name() + "/"),
        mCodec(floatScale)
    {
        mClient.connect();
        publishConfigurable(mPrefix, mConfigurable);
    }

    Controller::~Controller() = default;

    void Controller::deliver(std::string_view topic, std::string_view data)
    {
        Delivery delivery{&mCodec, data};
        tryDeliver(topic, mPrefix.substr(0, mPrefix.size() - 1), mConfigurable, &delivery);
    }

    void Controller::subscribe()
    {
        subscribeConfigurable(mPrefix , mConfigurable);
    }

    void Controller::subscribeConfigurable(std::string prefix, Configurable * configurable)
    {
        for (auto & [name, prop] : configurable->properties()) {
            std::string statusPropertyTopic = prefix;
            statusPropertyTopic += name;
            statusPropertyTopic += "/command";
            mClient.subscribe(statusPropertyTopic);
        }

        for (auto & [name, child] : configurable->children())
        {
            std::string childPrefix = prefix;
            childPrefix += name;
            childPrefix += "/";
            subscribeConfigurable(childPrefix, child);
        }
    }

    void Controller::publishConfigurable(std::string prefix, Configurable * configurable)
    {
        for (auto & [name, prop] : configurable->properties()) {
            std::string statusPropertyTopic = prefix;
            statusPropertyTopic += name;
            statusPropertyTopic += "/state";

            StatusPropertyFactory factory{
                &mClient,
                &mCodec,
                statusPropertyTopic,
                mProperties
            };
            TemplatedVisitor visitor(factory);
            prop->acceptVisitor(&visitor);
        }

        for (auto & [name, child] : configurable->children())
        {
            std::string childPrefix = prefix;
            childPrefix += name;
            childPrefix += "/";
            publishConfigurable(childPrefix, child);
        }
    }
}
