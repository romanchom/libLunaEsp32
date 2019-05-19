#pragma once

#include "PbufStream.hpp"
#include "Timer.hpp"

// #include <luna/server/StrandDecoder.hpp>

#include "mbedtls-cpp/BasicInputOutput.hpp"
#include "mbedtls-cpp/Configuration.hpp"
#include "mbedtls-cpp/CounterDeterministicRandomGenerator.hpp"
#include "mbedtls-cpp/Ssl.hpp"
#include "mbedtls-cpp/StandardCookie.hpp"
#include "mbedtls-cpp/StandardEntropy.hpp"
#include "mbedtls-cpp/Timer.hpp"
#include "mbedtls-cpp/PrivateKey.hpp"
#include "mbedtls-cpp/Certificate.hpp"

#include <lwip/udp.h>

#include <cstdint>
#include <vector>
#include <functional>

namespace lwip_async
{

class DtlsInputOutput : private tls::BasicInputOutput
{
public:
    using DataReadCallback = std::function<void(DtlsInputOutput&, uint8_t const*, size_t)>;
    using DisconnectedCallback = std::function<void(DtlsInputOutput&, bool)>;

    explicit DtlsInputOutput(tls::PrivateKey & ownKey, tls::Certificate & ownCertificate, tls::Certificate & caCertificate);
    ~DtlsInputOutput() override;

    uint16_t port() const;
    void onDataRead(DataReadCallback value) {
        mDataReadCallback = std::move(value);
    }

    void onConnected(DisconnectedCallback value) {
        mConnectedCallback = value;
    }

    int write(uint8_t const * data, size_t size);
private:
    using StepFunctionType = bool (DtlsInputOutput::*)(ip_addr_t const * address, u16_t port);

    mbedtls_ssl_send_t * getSender() override;
    mbedtls_ssl_recv_t * getReceiver() override;
    mbedtls_ssl_recv_timeout_t * getReceiverTimeout() override;
    void * getContext() override;

    int socketWrite(uint8_t const * data, size_t length);
    void udpReceive(pbuf * buffer, ip_addr_t const * address, u16_t port);

    bool handshakeStep(ip_addr_t const * address, u16_t port);
    bool readDataStep(ip_addr_t const * address, u16_t port);

    static void timeoutCallback(void * data);

    void startHeartbeat();
    void stopHeartbeat();
    void reset();

    udp_pcb * mUdp;
    PbufStream mBuffer;

    Timer mTimer;

    StepFunctionType mCurrentStep;

    tls::StandardEntropy mEntropy;
    tls::CounterDeterministicRandomGenerator mRandom;
    tls::StandardCookie mCookie;
    tls::Configuration mTlsConfiguration;
    tls::Ssl mSsl;

    DataReadCallback mDataReadCallback;
    DisconnectedCallback mConnectedCallback;
};

};