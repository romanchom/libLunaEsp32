#include "luna/esp32/MqttService.hpp"

#include "luna/esp32/HardwareController.hpp"
#include "luna/esp32/Strand.hpp"
#include "luna/esp32/ConstantGenerator.hpp"
#include "luna/proto/Scalar.hpp"

#include <esp_log.h>

#include <algorithm>
#include <chrono>

static char const TAG[] = "MqttSvc";

namespace {
    struct Lock
    {
        explicit Lock(SemaphoreHandle_t mutex) :
            mMutex(mutex)
        {
            xSemaphoreTake(mMutex, portMAX_DELAY);
        }

        ~Lock()
        {
            xSemaphoreGive(mMutex);
        }
    private:
        SemaphoreHandle_t const mMutex;
    };
}
#include <iostream>
namespace luna::esp32
{
    MqttService::MqttService(asio::io_context * ioContext, std::string const & address) :
        mMqtt(address),
        mTick(*ioContext),
        mController(nullptr),
        mMutex(xSemaphoreCreateMutex()),
        mWhiteness(0.0f),
        mSmoothWhiteness(0.0f)
    {
        mMqtt.subscribe("led/#", [this](MqttTopic const & topic, void * data, size_t size) {
            if (topic[1].str() == "on") {
                int on = atoi((char *) data);
                bool enabled = (on > 0);
                serviceEnabled(enabled);
                ESP_LOGI(TAG, "%s", enabled ? "On" : "Off");
            } else if (topic[1].str() == "whiteness") {
                Lock l(mMutex);

                ESP_LOGI(TAG, "whiteness");
                float value = atof((char *) data);
                mWhiteness = std::clamp<float>(value, 0.0f, 1.0f);
            }
        });
    }

    void MqttService::start()
    {
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
        Lock l(mMutex);
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
        Lock l(mMutex);

        if (!mController) {
            return;
        }

        mSmoothWhiteness = mSmoothWhiteness * 0.97f + mWhiteness * 0.03f;

        ConstantGenerator generator;
        generator.color({mSmoothWhiteness, mSmoothWhiteness, mSmoothWhiteness, mSmoothWhiteness});

        for (auto strand : mController->strands()) {
            strand->fill(&generator);
        }
        mController->update();
    }
}
