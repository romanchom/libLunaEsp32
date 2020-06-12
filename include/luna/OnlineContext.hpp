#pragma once

#include <asio/io_context.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace luna
{
    struct OnlineContext
    {
        explicit OnlineContext();
        ~OnlineContext();

        void start();

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
