#pragma once

#include <asio/io_context.hpp>

namespace luna
{
    struct OnlineContext
    {
        explicit OnlineContext();
        ~OnlineContext();

        asio::io_context * ioContext()
        {
            return &mIoContext;
        }
    private:
        static void task(void * data);

        asio::io_context mIoContext;

        TaskHandle_t mTaskHandle;
    };
}
