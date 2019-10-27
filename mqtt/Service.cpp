#include "Service.hpp"

#include <esp_log.h>

#include <string>

static char const TAG[] = "MqttSvc";

namespace luna::mqtt
{
    Service::Service(EffectEngine * effectEngine, std::string_view name, Client::Configuration const & configuration) :
        mClient(configuration)
    {
        std::string nameString(name);

        mClient.subscribe(nameString + "/+", [effectEngine](Topic const & topic, std::string_view payload) {
            ESP_LOGI(TAG, "%s", topic.str().c_str());
            effectEngine->setProperty(topic[1].str(), payload);
        });

        mClient.subscribe(nameString + "/effects/#", [effectEngine](Topic const & topic, std::string_view payload) {
            if (topic.size() != 4) {
                return;
            }
            auto effectName = topic[2].str();
            auto & effects = effectEngine->effects();
            auto effect = effects.find(effectName);
            if (effect == effects.end()) {
                return;
            }
            auto propertyName = topic[3].str();
            (*effect)->setProperty(propertyName, payload);
        });
        // mClient.subscribe(mName + "/config/+", [this](Topic const & topic, std::string_view payload) {
        //     mEffectMixer.setProperty(topic[2].str(), payload);
        // });
        mClient.connect();
    }
}
