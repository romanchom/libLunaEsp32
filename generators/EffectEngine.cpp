#include "EffectEngine.hpp"

#include "ConstantGenerator.hpp"

#include <luna/Parse.hpp>
#include <luna/HardwareController.hpp>
#include <luna/Strand.hpp>
#include <luna/proto/Scalar.hpp>

#include <esp_log.h>

#include <chrono>

static char const TAG[] = "Effects";

namespace luna
{
    EffectEngine::EffectEngine(asio::io_context * ioContext) :
        Configurable("effects"),
        mTick(*ioContext),
        mController(nullptr),
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

    void EffectEngine::addEffect(std::string_view name, Effect * effect)
    {
        mEffects.try_emplace(std::string(name), effect);
        if (mEffects.size() == 1) {
            mEffectMixer.switchTo(effect);
        }
    }

    void EffectEngine::switchTo(std::string_view effectName)
    {
        if (auto it = mEffects.find(effectName); it != mEffects.end()) {
            mEffectMixer.switchTo(it->second);
        }
    }

    void EffectEngine::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        mController = controller;
        mController->enabled(true);
        startTick();
    }

    void EffectEngine::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        mTick.cancel();
        mController = nullptr;
    }

    void EffectEngine::startTick()
    {
        mTick.expires_after(std::chrono::milliseconds(20));

        update();

        mTick.async_wait([this](asio::error_code const & error) {
            if (!error) {
                startTick();
            }
        });
    }

    void EffectEngine::update()
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

    void EffectEngine::enabledChanged(bool enabled)
    {
        serviceEnabled(enabled);
    }
}
