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
        mClient(configuration),
        mTick(*ioContext),
        mController(nullptr),
        mName(configuration.name)
    {}

    void Service::addEffect(std::string name, Effect * effect)
    {
        mEffects.try_emplace(std::move(name), effect);
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
        mClient.subscribe(mName + "/enabled", [this](Topic const & topic, std::string_view payload) {
            if (auto value = tryParse<int>(payload)) {
                bool enabled = (*value > 0);
                serviceEnabled(enabled);
                ESP_LOGI(TAG, "%s", enabled ? "On" : "Off");
            }
        });

        mClient.subscribe(mName + "/effect", [this](Topic const & topic, std::string_view payload) {
            switchTo(payload);
        });

        for (auto & [effectName, effect] : mEffects) {
            mClient.subscribe(mName + "/effects/" + effectName + "/#", [effect](Topic const & topic, std::string_view payload) {
                effect->configure(topic, payload);
            });
        }

        mClient.subscribe(mName + "/config/#", [this](Topic const & topic, std::string_view payload) {
            mEffectMixer.configure(topic, payload);
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
        auto l = std::lock_guard(mMutex);
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
