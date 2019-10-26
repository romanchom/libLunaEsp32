#include "Main.hpp"


#include "WiFi.hpp"
#include "Configuration.hpp"
#include "TlsConfiguration.hpp"
#include "IdleService.hpp"
#include "Updater.hpp"
#include "DiscoveryResponder.hpp"
#include "RealtimeService.hpp"

#include <luna/HardwareController.hpp>
#include <luna/EffectEngine.hpp>
#include <luna/FlameEffect.hpp>
#include <luna/ConstantEffect.hpp>
#include <luna/PlasmaEffect.hpp>
#include <luna/ServiceManager.hpp>
#include <luna/mqtt/Service.hpp>

#include <asio/io_context.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

static char const TAG[] = "Luna";

namespace
{
    void task(void * data)
    {
        ESP_LOGI(TAG, "Up and running.");
        static_cast<asio::io_context *>(data)->run();
        std::terminate();
    }
}

namespace luna
{
    struct Main::Impl : WiFi::Observer
    {
        explicit Impl(Configuration const & config, HardwareController * controller);

        void connected() override;
        void disconnected() override;
    private:
        TlsConfiguration mTlsConfiguration;
        asio::io_context mIoContext;

        IdleService mIdleSerice;

        ConstantEffect mLightEffect;
        FlameEffect mFlameEffect;
        PlasmaEffect mPlasmaEffect;
        EffectEngine mEffectEngine;

        RealtimeService mRealtimeService;

        ServiceManager mServiceManager;

        mqtt::Service mMqtt;
        DiscoveryResponder mDiscoveryResponder;
        Updater mUpdater;
        WiFi mWiFi;

        TaskHandle_t mTaskHandle;
    };

    Main::Impl::Impl(Configuration const & config, HardwareController * controller) :
        mTlsConfiguration(
            config.network.ownKey,
            config.network.ownCertificate,
            config.network.caCertificate
        ),
        mLightEffect("light"),
        mFlameEffect("flame"),
        mPlasmaEffect("plasma"),
        mEffectEngine(&mIoContext, {&mLightEffect, &mFlameEffect, &mPlasmaEffect}),
        mRealtimeService(&mIoContext, mTlsConfiguration.realtimeConfiguration()),
        mServiceManager(controller, {&mIdleSerice, &mEffectEngine, &mRealtimeService}),
        mMqtt(
            &mEffectEngine,
            config.network.name,
            {
                config.network.mqttAddress,
                config.network.ownKey,
                config.network.ownCertificate,
                config.network.caCertificate
            }
        ),
        mDiscoveryResponder(&mIoContext, mRealtimeService.port(), config.network.name, controller->strands()),
        mUpdater(&mIoContext, mTlsConfiguration.updaterConfiguration()),
        mWiFi(config.wifi.ssid, config.wifi.password),
        mTaskHandle(0)
    {
        mServiceManager.serviceEnabled(&mIdleSerice, true);

        xTaskCreatePinnedToCore(&task, "Daemon", 1024 * 8, &mIoContext, 5, &mTaskHandle, 1);

        mWiFi.observer(this);
        mWiFi.enabled(true);
    }

    void Main::Impl::connected()
    {
        mMqtt.enabled(true);
        mRealtimeService.enabled(true);
        mDiscoveryResponder.enabled(true);
        mUpdater.enabled(true);

        ESP_LOGI(TAG, "Online.");
    }

    void Main::Impl::disconnected()
    {

    }

    Main::Main(Configuration const & config, HardwareController * controller) :
        mImpl(std::make_unique<Impl>(config, controller))
    {}

    Main::~Main() = default;
}
