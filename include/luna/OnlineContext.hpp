#pragma once

#include <luna/TlsConfiguration.hpp>
#include <luna/TlsCredentials.hpp>

#include <asio/io_context.hpp>
#include <vector>

namespace luna
{
    struct EventLoop;
    struct Plugin;
    struct TlsConfiguration;
    struct NetworkService;

    struct OnlineContext
    {
        explicit OnlineContext(EventLoop * mainLoop, TlsCredentials const & credentials, std::vector<Plugin *> plugins);
        ~OnlineContext();

    private:
        static void task(void * data);

        TlsConfiguration mTlsConfiguration;
        asio::io_context mIoContext;

        std::vector<std::unique_ptr<NetworkService>> mServices;

        TaskHandle_t mTaskHandle;
    };
}
