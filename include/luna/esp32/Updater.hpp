#pragma once

#include "mbedtls-cpp/PrivateKey.hpp"
#include "mbedtls-cpp/Certificate.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

namespace luna::esp32
{

struct Updater
{
    explicit Updater(tls::PrivateKey * ownKey, tls::Certificate * ownCertificate, tls::Certificate * caCertificate);
    void start();
    void stop();
private:
    static void task(void * context);
    void doOta();

    tls::PrivateKey * mOwnKey;
    tls::Certificate * mOwnCertificate;
    tls::Certificate * mCaCertificate;
    
    TaskHandle_t mTaskHandle;
    SemaphoreHandle_t mWaitHandle;
};

}
