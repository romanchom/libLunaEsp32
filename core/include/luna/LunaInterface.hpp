#pragma once

#include <functional>
#include <memory>

namespace asio
{
    class io_context;
}

namespace luna
{
    struct Controller;
    struct Device;
    struct NetworkService;
    struct TlsConfiguration;
    struct TlsCredentials;

    struct ControllerHandle
    {
        virtual ~ControllerHandle() = default;
        virtual void enabled(bool value) = 0;
    };

    struct LunaInterface
    {
        virtual void post(std::function<void()> && task) = 0;
        virtual std::unique_ptr<ControllerHandle> addController(Controller * controller) = 0;
        virtual Device * device() = 0;
    protected:
        ~LunaInterface() = default;
    };

    struct LunaNetworkInterface : LunaInterface
    {
        virtual void addNetworkService(std::unique_ptr<NetworkService> service) = 0;
        virtual TlsCredentials const & tlsCredentials() = 0;
        virtual TlsConfiguration * tlsConfiguration() = 0;
        virtual asio::io_context * ioContext() = 0;
    };
}