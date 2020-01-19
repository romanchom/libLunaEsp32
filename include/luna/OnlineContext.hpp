#pragma once

#include <asio/io_context.hpp>

#include "Configuration.hpp"
#include "DiscoveryResponder.hpp"
#include "RealtimeService.hpp"
#include "Updater.hpp"

#include <luna/mqtt/Service.hpp>

namespace luna
{
    struct TlsConfiguration;
    struct Configurable;
    struct HardwareController;
    struct DirectService;

    struct OnlineContext
    {
        explicit OnlineContext(Configuration::Network const & config, TlsConfiguration * tlsConfigutation, HardwareController * controller, Configurable * effectEngine, DirectService * directService);
        ~OnlineContext();
    private:
        asio::io_context mIoContext;
        RealtimeService mRealtimeService;
        mqtt::Service mMqtt;
        DiscoveryResponder mDiscoveryResponder;
        Updater mUpdater;

        TaskHandle_t mTaskHandle;
    };

}
