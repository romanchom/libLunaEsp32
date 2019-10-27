#include "Main.hpp"


#include "Nvs.hpp"
#include "WiFi.hpp"
#include "Configuration.hpp"
#include "TlsConfiguration.hpp"
#include "IdleService.hpp"
#include "DirectService.hpp"
#include "OnlineContext.hpp"

#include <luna/EffectEngine.hpp>
#include <luna/FlameEffect.hpp>
#include <luna/ConstantEffect.hpp>
#include <luna/PlasmaEffect.hpp>
#include <luna/ServiceManager.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace luna
{
    struct Main::Impl : private WiFi::Observer
    {
        explicit Impl(Configuration const & config, HardwareController * controller);
    private:
        void connected() override;
        void disconnected() override;

        HardwareController * mController;
        Configuration::Network mNetworkConfiguration;

        Nvs mNvs;
        TlsConfiguration mTlsConfiguration;

        IdleService mIdleSerice;

        ConstantEffect mLightEffect;
        FlameEffect mFlameEffect;
        PlasmaEffect mPlasmaEffect;
        EffectEngine mEffectEngine;

        DirectService mDirectService;

        ServiceManager mServiceManager;

        std::unique_ptr<OnlineContext> mOnlineContext;
        WiFi mWiFi;
    };

    Main::Impl::Impl(Configuration const & config, HardwareController * controller) :
        mController(controller),
        mNetworkConfiguration(config.network),
        mTlsConfiguration(
            config.network.ownKey,
            config.network.ownCertificate,
            config.network.caCertificate
        ),
        mLightEffect("light"),
        mFlameEffect("flame"),
        mPlasmaEffect("plasma"),
        mEffectEngine({&mLightEffect, &mFlameEffect, &mPlasmaEffect}),
        mServiceManager(controller, {&mIdleSerice, &mEffectEngine, &mDirectService}),
        mWiFi(config.wifi.ssid, config.wifi.password)
    {
        mServiceManager.serviceEnabled(&mIdleSerice, true);

        mWiFi.observer(this);
        mWiFi.enabled(true);
    }

    void Main::Impl::connected()
    {
        mOnlineContext = std::make_unique<OnlineContext>(mNetworkConfiguration, &mTlsConfiguration, mController, &mEffectEngine, &mDirectService);
    }

    void Main::Impl::disconnected()
    {
        mOnlineContext.reset();
    }

    Main::Main(Configuration const & config, HardwareController * controller) :
        mImpl(std::make_unique<Impl>(config, controller))
    {}

    Main::~Main() = default;
}
