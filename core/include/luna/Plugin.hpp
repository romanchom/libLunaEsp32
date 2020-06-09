#pragma once

#include "LunaInterface.hpp"

#include <memory>

namespace asio
{
    class io_context;
}

namespace luna
{
    struct PluginInstance
    {
        virtual ~PluginInstance() = default;
        virtual void onNetworkAvaliable(LunaNetworkInterface * luna) = 0;
    };

    struct Plugin
    {
        virtual std::unique_ptr<PluginInstance> instantiate(LunaInterface * luna) = 0;
    protected:
        ~Plugin() = default;
    };
}
