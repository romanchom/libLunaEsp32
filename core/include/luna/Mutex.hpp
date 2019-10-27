#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>


namespace luna
{
    struct Mutex
    {
        explicit Mutex() :
            mHandle(xSemaphoreCreateMutex())
        {}

        void lock()
        {
            xSemaphoreTake(mHandle, portMAX_DELAY);
        }

        void unlock()
        {
            xSemaphoreGive(mHandle);
        }
    private:
        SemaphoreHandle_t mHandle;
    };

    struct RecursiveMutex
    {
        explicit RecursiveMutex() :
            mHandle(xSemaphoreCreateRecursiveMutex())
        {}

        void lock()
        {
            xSemaphoreTakeRecursive(mHandle, portMAX_DELAY);
        }

        void unlock()
        {
            xSemaphoreGiveRecursive(mHandle);
        }
    private:
        SemaphoreHandle_t mHandle;
    };
}