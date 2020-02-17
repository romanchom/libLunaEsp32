#include "Controller.hpp"
#include "Subscription.hpp"

#include <luna/EventLoop.hpp>
#include <luna/Property.hpp>

#include <prism/Prism.hpp>

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
            std::string cstr(text);
            return strtof(cstr.c_str(), nullptr);
        }

        template<>
        std::optional<std::string> parse<std::string>(std::string_view text)
        {
            return std::string(text);
        }

        template<>
        std::optional<prism::RGB> parse<prism::RGB>(std::string_view text)
        {
            if (text.size() == 7 && text[0] == '#') {
                prism::RGB color;
                for (int i = 0; i < 3; ++i) {
                    auto channel = text.substr(1 + 2 * i, 2);
                    int value;
                    auto [_, error] = std::from_chars(channel.data(), channel.data() + channel.size(), value, 16);
                    if (error == std::errc()) {
                        color[i] = value / 255.0f;
                    }
                }
                color = prism::linearizeSRGB(color);
                prism::RGB ret;
                ret[3] = 0;
                ret.head<3>() = color.head<3>();
                return ret;
            }
            return std::nullopt;
        }

        template<>
        std::optional<prism::CieXY> parse<prism::CieXY>(std::string_view text)
        {
            std::string cstr(text);
            prism::CieXY ret;
            if (2 == sscanf(cstr.c_str(), "%f,%f", &ret[0], &ret[1])) {
                return ret;
            } else {
                return std::nullopt;
            }
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
        std::string serialize<prism::RGB>(prism::RGB const & value)
        {
            auto rgb = (prism::compressSRGB(value).head<3>() * 255).array().cwiseMax(0).cwiseMin(255).cast<uint8_t>().eval();
            char buf[8];
            sprintf(buf, "#%02x%02x%02x", rgb[0], rgb[1], rgb[2]);
            return buf;
        }

        template<>
        std::string serialize<prism::CieXY>(prism::CieXY const & value)
        {
            char buf[64];
            sprintf(buf, "%.4f,%.4f", value[0], value[1]);
            return buf;
        }

        template<typename T>
        struct MqttProperty : Subscription, Property<T>
        {
            explicit MqttProperty(Controller * controller, std::string const & topic) :
                Subscription(topic),
                Property<T>(std::string{}),
                mController(controller)
            {}

            T getValue() const override
            {
                return mValue;
            }

            void set(T const & value) override
            {
                if (mValue == value) { return; }
                mValue = value;
                auto text = serialize<T>(value);
                ESP_LOGI(TAG, "Sending: %s %s", topic().str().c_str(), text.c_str());
                mController->client()->publish(topic(), text);
            }
        private:
            void deliver(std::string_view text) override
            {
                ESP_LOGI(TAG, "Got %s: %.*s", topic().str().c_str(), text.size(), text.data());
                if (auto value = parse<T>(text)) {
                    mController->eventLoop()->post([this, v = *value](){
                        mValue = v;
                        this->notify(mValue);
                    });
                }
            }

            T mValue = T{};
            Controller * const mController;
        };

        struct MqttVisitor : Visitor
        {
            explicit MqttVisitor(Controller * controller, std::string && topic) :
                mController(controller),
                mTopic(std::move(topic))
            {
                ESP_LOGI(TAG, "Sub: %s", mTopic.c_str());
            }

            template<typename T>
            void makeSubscription(Property<T> * property)
            {
                auto mqttProperty = std::make_unique<MqttProperty<T>>(mController, mTopic);
                mqttProperty->bindTo(property);
                property->bindTo(mqttProperty.get());
                mController->client()->subscribe(std::move(mqttProperty));
            }

            void visit(Property<bool> * property) final { makeSubscription<bool>(property); }
            void visit(Property<int> * property) final { makeSubscription<int>(property); }
            void visit(Property<float> * property) final { makeSubscription<float>(property); }
            void visit(Property<prism::RGB> * property) final { makeSubscription<prism::RGB>(property); }
            void visit(Property<prism::CieXY> * property) final { makeSubscription<prism::CieXY>(property); }
            void visit(Property<std::string> * property) final { makeSubscription<std::string>(property); }

        private:
            Controller * mController;
            std::string const mTopic;
        };
    }

    Controller::Controller(EventLoop * eventLoop, Configurable * effectEngine, std::string const & name, std::string const & address, TlsCredentials const & credentials) :
        mClient(address, credentials),
        mEventLoop(eventLoop)
    {
        subscribeConfigurable(effectEngine, name + "/");
        mClient.connect();
    }

    Controller::~Controller() = default;

    void Controller::subscribeConfigurable(Configurable * configurable, std::string name)
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
