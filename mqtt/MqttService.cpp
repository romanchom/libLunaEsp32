#include "MqttService.hpp"

#include "Parse.hpp"

#include <luna/HardwareController.hpp>
#include <luna/Strand.hpp>
#include <luna/ConstantGenerator.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>

#include <chrono>

static char const TAG[] = "MqttSvc";

namespace luna
{
    MqttService::MqttService(asio::io_context * ioContext, std::string const & address, std::string name) :
        mMqtt(address),
        mTick(*ioContext),
        mController(nullptr),
        mName(name)
    {}

    void MqttService::addEffect(std::string name, MqttEffect * effect)
    {
        mEffects.try_emplace(std::move(name), effect);
        if (mEffects.size() == 1) {
            mEffectMixer.switchTo(effect);
        }
    }

    void MqttService::switchTo(std::string_view effectName)
    {
        if (auto it = mEffects.find(effectName); it != mEffects.end()) {
            mEffectMixer.switchTo(it->second);
        }
    }

    void MqttService::start()
    {
        mMqtt.subscribe(mName + "/enabled", [this](MqttTopic const & topic, std::string_view payload) {
            if (auto value = tryParse<int>(payload)) {
                bool enabled = (*value > 0);
                serviceEnabled(enabled);
                ESP_LOGI(TAG, "%s", enabled ? "On" : "Off");
            }
        });

        mMqtt.subscribe(mName + "/effect", [this](MqttTopic const & topic, std::string_view payload) {
            switchTo(payload);
        });

        for (auto & [effectName, effect] : mEffects) {
            mMqtt.subscribe(mName + "/effects/" + effectName + "/#", [effect](MqttTopic const & topic, std::string_view payload) {
                effect->configure(topic, payload);
            });
        }

        mMqtt.connect();
    }

    void MqttService::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        mController = controller;
        mController->enabled(true);
        startTick();
    }

    void MqttService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        auto l = std::lock_guard(mMutex);
        mTick.cancel();
        mController = nullptr;
    }

    void MqttService::startTick()
    {
        mTick.expires_after(std::chrono::milliseconds(20));
        update();

        mTick.async_wait([this](asio::error_code const & error) {
            if (!error) {
                startTick();
            }
        });
    }

    void MqttService::update()
    {
        auto l = std::lock_guard(mMutex);

        if (!mController) {
            return;
        }

        mEffectMixer.update(0.02f);

        for (auto strand : mController->strands()) {
            auto gen = mEffectMixer.generator(strand->location());
            strand->fill(gen);
        }

        mController->update();
    }
}
