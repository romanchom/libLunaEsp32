#include "luna/esp32/MqttService.hpp"

#include "luna/esp32/HardwareController.hpp"
#include "luna/esp32/Strand.hpp"
#include "luna/proto/Scalar.hpp"

#include <esp_log.h>

#include <algorithm>
#include <chrono>

static char const TAG[] = "MqttSvc";

namespace luna::esp32
{
    MqttService::MqttService(asio::io_context * ioContext) :
        mMqtt("mqtt://192.168.1.1:1883"),
        mTick(*ioContext),
        mController(nullptr),
        mWhiteness(0.0f),
        mSmoothWhiteness(0.0f)
    {        
        mMqtt.subscribe("/on", [this](void * data, size_t size) {
            int on = atoi((char *) data);
            mEnabled = (on > 0);
            ESP_LOGI(TAG, "%s", mEnabled ? "On" : "Off");
        });

        mMqtt.subscribe("/whiteness", [this](void * data, size_t size) {
            ESP_LOGI(TAG, "whiteness");
            float value = atof((char *) data);
            mWhiteness = std::clamp<float>(value, 0.0f, 1.0f);
        });

    }

    void MqttService::start()
    {
        serviceEnabled(true);
        mMqtt.connect();
    }

    void MqttService::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        mController = controller;
        startTick();
    }
    
    void MqttService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        mTick.cancel();
        mController->enabled(false);
        mController = nullptr;
    }
    
    void MqttService::startTick()
    {
        mTick.expires_after(std::chrono::milliseconds(20));
        mTick.async_wait([this](asio::error_code const & error) {
            if (!error) {
                update();
                startTick();
            }
        });
    }

    void MqttService::update()
    {
        mSmoothWhiteness = mSmoothWhiteness * 0.97f + mWhiteness * 0.03f;

        if (mController) {
            mController->enabled(mEnabled);
            if (mEnabled) {
                
                for (auto strand : mController->strands()) {
                    if (strand->format() == proto::Format::White16) {
                        auto whiteStrand = static_cast<Strand<proto::Scalar<uint16_t>> *>(strand);
                        proto::Scalar<uint16_t> asd = uint16_t(mSmoothWhiteness * ((1 << 16) - 1));
                        whiteStrand->setLight(&asd, 1, 0);
                    }
                }
            }
        }
    }
}
