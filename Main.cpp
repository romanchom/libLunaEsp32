#include "Main.hpp"


#include "Nvs.hpp"
#include "WiFi.hpp"
#include "Configuration.hpp"
#include "TlsConfiguration.hpp"
#include "IdleService.hpp"
#include "DirectService.hpp"
#include "OnlineContext.hpp"
#include "Infrared.hpp"

#include <luna/EventLoop.hpp>
#include <luna/EffectEngine.hpp>
#include <luna/ConstantEffect.hpp>
#include <luna/FlameEffect.hpp>
#include <luna/PlasmaEffect.hpp>
#include <luna/ServiceManager.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace luna
{
    struct Main::Impl : private WiFi::Observer
    {
        explicit Impl(Configuration const & config, HardwareController * controller);
        void execute()
        {
            mWiFi.enabled(true);
            mMainLoop.execute();
            mWiFi.enabled(false);
        }
    private:
        void connected() override;
        void disconnected() override;

        HardwareController * mController;
        Configuration::Network mNetworkConfiguration;
        TlsConfiguration mTlsConfiguration;

        EventLoop mMainLoop;

        IdleService mIdleSerice;

        ConstantEffect mLightEffect;
        FlameEffect mFlameEffect;
        PlasmaEffect mPlasmaEffect;
        EffectSet mEffects;
        EffectEngine mEffectEngine;

        DirectService mDirectService;

        ServiceManager mServiceManager;

        Nvs mNvs;
        Infrared mIR;
        WiFi mWiFi;

        std::unique_ptr<OnlineContext> mOnlineContext;
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
        mEffects({&mLightEffect, &mFlameEffect, &mPlasmaEffect}),
        mEffectEngine(&mEffects, &mMainLoop),
        mServiceManager(controller, {&mIdleSerice, &mEffectEngine, &mDirectService}),
        mIR(27),
        mWiFi(config.wifi.ssid, config.wifi.password)
    {
        mWiFi.observer(this);
    }

    void Main::Impl::connected()
    {
        mMainLoop.post([this]{
            mOnlineContext = std::make_unique<OnlineContext>(mNetworkConfiguration, &mTlsConfiguration, mController, &mEffectEngine, &mDirectService, &mMainLoop);
        });
    }

    void Main::Impl::disconnected()
    {
        mMainLoop.post([this]{
           mOnlineContext.reset();
        });
    }

    Main::Main(Configuration const & config, HardwareController * controller) :
        mImpl(std::make_unique<Impl>(config, controller))
    {}

    void Main::execute()
    {
        mImpl->execute();
    }

    Main::~Main() = default;
}
