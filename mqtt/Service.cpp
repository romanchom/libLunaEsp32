#include "Service.hpp"

#include "Parse.hpp"

#include <luna/HardwareController.hpp>
#include <luna/Strand.hpp>
#include <luna/ConstantGenerator.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>

#include <chrono>

static char const TAG[] = "MqttSvc";

namespace luna::mqtt
{
    Service::Service(asio::io_context * ioContext, NetworkManagerConfiguration const & configuration) :
        Configurable("mqtt"),
        mClient(configuration),
        mTick(*ioContext),
        mController(nullptr),
        mName(configuration.name),
        mEffectMixer(this)
    {
        addProperty("enabled", [this](std::string_view text) {
            if (auto value = tryParse<int>(text)) {
                bool enabled = (*value > 0);
                mEffectMixer.enabled(enabled);
                ESP_LOGI(TAG, "%s", enabled ? "On" : "Off");
            }
        });
        addProperty("effect", [this](std::string_view text) {
            switchTo(text);
        });
    }

    void Service::addEffect(std::string_view name, Effect * effect)
    {
        mEffects.try_emplace(std::string(name), effect);
        if (mEffects.size() == 1) {
            mEffectMixer.switchTo(effect);
        }
    }

    void Service::switchTo(std::string_view effectName)
    {
        if (auto it = mEffects.find(effectName); it != mEffects.end()) {
            mEffectMixer.switchTo(it->second);
        }
    }

    void Service::start()
    {
        mClient.subscribe(mName + "/+", [this](Topic const & topic, std::string_view payload) {
            ESP_LOGI(TAG, "%s", topic.str().c_str());
            setProperty(topic[1].str(), payload);
        });

        for (auto & [effectName, effect] : mEffects) {
            mClient.subscribe(mName + "/effects/" + effectName + "/+", [effect](Topic const & topic, std::string_view payload) {
                effect->setProperty(topic[3].str(), payload);
            });
        }

        mClient.subscribe(mName + "/config/+", [this](Topic const & topic, std::string_view payload) {
            mEffectMixer.setProperty(topic[2].str(), payload);
        });

        mClient.connect();
    }

    void Service::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        mController = controller;
        mController->enabled(true);
        startTick();
    }

    void Service::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        mTick.cancel();
        mController = nullptr;
    }

    void Service::startTick()
    {
        mTick.expires_after(std::chrono::milliseconds(20));

        update();

        mTick.async_wait([this](asio::error_code const & error) {
            if (!error) {
                startTick();
            }
        });
    }

    void Service::update()
    {
        mEffectMixer.update(0.02f);

        if (!mController) {
            return;
        }

        for (auto strand : mController->strands()) {
            auto gen = mEffectMixer.generator(strand->location());
            strand->fill(gen);
        }

        mController->update();
    }

    void Service::enabledChanged(bool enabled)
    {
        serviceEnabled(enabled);
    }
}
