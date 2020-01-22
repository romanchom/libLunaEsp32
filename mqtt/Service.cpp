#include "Service.hpp"
#include "Subscription.hpp"

#include <esp_log.h>

#include <charconv>
#include <string>
#include <optional>

static char const TAG[] = "MqttSvc";

namespace luna::mqtt
{
    namespace
    {
        template<typename T>
        std::optional<T> parse(std::string_view text);

        template<>
        std::optional<bool> parse<bool>(std::string_view text)
        {
            if (text == "0" || text == "off") { return false; }
            if (text == "1" || text == "on") { return true; }
            return std::nullopt;
        }

        template<>
        std::optional<int> parse<int>(std::string_view text)
        {
            int value;
            auto [_, error] = std::from_chars(text.data(), text.data() + text.size(), value);
            if (error == std::errc()) {
                return value;
            }
            return std::nullopt;
        }

        template<>
        std::optional<float> parse<float>(std::string_view text)
        {
            // TODO fix overflow
            return strtof(text.data(), nullptr);
        }

        template<>
        std::optional<std::string> parse<std::string>(std::string_view text)
        {
            return std::string(text);
        }

        template<>
        std::optional<prism::CieXYZ> parse<prism::CieXYZ>(std::string_view text)
        {
            if (text.size() == 7 && text[0] == '#') {
                prism::RGB color;
                static auto converter = prism::rgbToXyzTransformationMatrix(prism::rec2020());
                for (int i = 0; i < 3; ++i) {
                    auto channel = text.substr(1 + 2 * i, 2);
                    int value;
                    auto [_, error] = std::from_chars(channel.data(), channel.data() + channel.size(), value, 16);
                    if (error == std::errc()) {
                        color[i] = value / 255.0f;
                    }
                }
                color = prism::linearizeSRGB(color);
                prism::CieXYZ ret;
                ret[3] = 0;
                ret.head<3>() = converter * color.head<3>();
                return ret;
            }
            return std::nullopt;
        }

        template<typename T>
        std::string serialize(T const & value);

        template<>
        std::string serialize<bool>(bool const & value)
        {
            return value ? "1" : "0";
        }

        template<>
        std::string serialize<int>(int const & value)
        {
            return std::to_string(value);
        }

        template<>
        std::string serialize<float>(float const & value)
        {
            return std::to_string(value);
        }

        template<>
        std::string serialize<std::string>(std::string const & value)
        {
            return value;
        }

        template<>
        std::string serialize<prism::CieXYZ>(prism::CieXYZ const & value)
        {
            static auto converter = prism::rgbToXyzTransformationMatrix(prism::rec2020()).inverse().eval();
            auto rgb = (converter * value.head<3>() * 255).array().cwiseMax(0).cwiseMin(255).cast<uint8_t>().eval();
            char buf[8];
            sprintf(buf, "#%02x%02x%02x", rgb[0], rgb[1], rgb[2]);
            return buf;
        }

        template<typename T>
        struct MqttProperty : Subscription, Property<T>
        {
            explicit MqttProperty(Service * service, std::string const & topic) :
                Subscription(topic),
                Property<T>(std::string{}),
                mService(service)
            {}

            T getValue() const override
            {
                return mValue;
            }

            void setValue(T const & value) override
            {
                if (mValue == value) { return; }
                mValue = value;
                auto text = serialize<T>(value);
                mService->client()->publish(topic(), text);
            }
        private:
            void deliver(std::string_view text) override
            {
                ESP_LOGI(TAG, "%s: %.*s", topic().str().c_str(), text.size(), text.data());
                if (auto value = parse<T>(text)) {
                    mService->effectEngine()->post([this, v = *value](){
                        mValue = v;
                        this->notify(mValue);
                    });
                }
            }

            T mValue = T{};
            Service * const mService;
        };

        struct MqttVisitor : Visitor
        {
            explicit MqttVisitor(Service * service, std::string && topic) :
                mService(service),
                mTopic(std::move(topic))
            {}

            template<typename T>
            void makeSubscription(Property<T> * property)
            {
                auto mqttProperty = std::make_unique<MqttProperty<T>>(mService, mTopic);
                mqttProperty->bindTo(property);
                property->bindTo(mqttProperty.get());
                mService->client()->subscribe(std::move(mqttProperty));
            }

            void visit(Property<bool> * property) final { makeSubscription<bool>(property); }
            void visit(Property<int> * property) final { makeSubscription<int>(property); }
            void visit(Property<float> * property) final { makeSubscription<float>(property); }
            void visit(Property<prism::CieXYZ> * property) final { makeSubscription<prism::CieXYZ>(property); }
            void visit(Property<std::string> * property) final { makeSubscription<std::string>(property); }

        private:
            Service * mService;
            std::string const mTopic;
        };
    }

    Service::Service(EffectEngine * effectEngine, std::string name, Client::Configuration const & configuration) :
        mClient(configuration),
        mEffectEngine(effectEngine)
    {
        subscribeConfigurable(effectEngine, name + "/");
        mClient.connect();
    }

    void Service::subscribeConfigurable(Configurable * configurable, std::string name)
    {
        ESP_LOGI(TAG, "%s", name.c_str());
        for (auto property : configurable->properties()) {
            MqttVisitor visitor(this, name + property->name());
            property->acceptVisitor(&visitor);
        }

        for (auto child : configurable->children()) {
            subscribeConfigurable(child, name + child->name() + "/");
        }
    }
}
