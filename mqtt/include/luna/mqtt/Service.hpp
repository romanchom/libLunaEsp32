#pragma once

#include "Client.hpp"
#include "ConstantEffect.hpp"
#include "Effect.hpp"
#include "EffectMixer.hpp"

#include <luna/Service.hpp>
#include <luna/NetworkManagerConfiguration.hpp>

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <mutex>
#include <map>

namespace luna
{
    struct HardwareController;
}

namespace luna::mqtt
{
    struct Service : ::luna::Service, private EffectMixer::Observer, private Configurable
    {
        explicit Service(asio::io_context * ioContext, NetworkManagerConfiguration const & configuration);

        void addEffect(std::string_view name, Effect * effect);

        void start();
    private:
        void switchTo(std::string_view effectName);

        void takeOwnership(HardwareController * controller) final;
        void releaseOwnership() final;

        void startTick();
        void update();

        void enabledChanged(bool enabled) override;

        Client mClient;
        asio::steady_timer mTick;
        HardwareController * mController;

        std::string mName;
        std::map<std::string, Effect *, std::less<>> mEffects;
        EffectMixer mEffectMixer;
    };
}
