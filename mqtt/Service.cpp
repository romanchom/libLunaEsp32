#include "Service.hpp"

#include <luna/Parse.hpp>
#include <luna/HardwareController.hpp>
#include <luna/Strand.hpp>
#include <luna/ConstantGenerator.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>

#include <chrono>

static char const TAG[] = "MqttSvc";

namespace luna::mqtt
{
    Service::Service(asio::io_context * ioContext, EffectEngine * effectEngine, NetworkManagerConfiguration const & configuration) :
        mClient(configuration),
        mEffectEngine(effectEngine)
    {
        std::string name(configuration.name);

        mClient.subscribe(name + "/+", [this](Topic const & topic, std::string_view payload) {
            ESP_LOGI(TAG, "%s", topic.str().c_str());
            mEffectEngine->setProperty(topic[1].str(), payload);
        });

        for (auto & [effectName, effect] : mEffectEngine->effects()) {
            mClient.subscribe(name + "/effects/" + effectName + "/+", [effect](Topic const & topic, std::string_view payload) {
                effect->setProperty(topic[3].str(), payload);
            });
        }
        
        // mClient.subscribe(mName + "/config/+", [this](Topic const & topic, std::string_view payload) {
        //     mEffectMixer.setProperty(topic[2].str(), payload);
        // });
    }

    void Service::start()
    {
        mClient.connect();
    }
}
