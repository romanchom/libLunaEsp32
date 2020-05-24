#pragma once

#include "TlsCredentials.hpp"

namespace asio
{
    class io_context;
}

namespace luna {
    struct TlsConfiguration;

    struct NetworkingContext
    {
        TlsCredentials tlsCredentials;
        TlsConfiguration * tlsConfiguration;
        asio::io_context * ioContext;
    };
}
