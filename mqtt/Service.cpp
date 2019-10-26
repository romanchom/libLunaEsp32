#include "Service.hpp"

#include <esp_log.h>

static char const TAG[] = "MqttSvc";

namespace luna::mqtt
{
    Service::Service(EffectEngine * effectEngine, NetworkManagerConfiguration const & configuration) :
        mClient(configuration),
        mEffectEngine(effectEngine)
    {
        std::string name(configuration.name);

        mClient.subscribe(name + "/+", [this](Topic const & topic, std::string_view payload) {
            ESP_LOGI(TAG, "%s", topic.str().c_str());
            mEffectEngine->setProperty(topic[1].str(), payload);
        });

        mClient.subscribe(name + "/effects/#", [this](Topic const & topic, std::string_view payload) {
            if (topic.size() != 4) {
                return;
            }
            auto effectName = topic[2].str();
            auto & effects = mEffectEngine->effects();
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
    }

    void Service::start()
    {
        mClient.connect();
    }
}
