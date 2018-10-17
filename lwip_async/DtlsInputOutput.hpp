#pragma once

#include "PbufStream.hpp"
#include "Timer.hpp"

#include <luna/server/StrandDecoder.hpp>

#include "../mbedtls-cpp/BasicInputOutput.hpp"
#include "../mbedtls-cpp/Configuration.hpp"
#include "../mbedtls-cpp/CounterDeterministicRandomGenerator.hpp"
#include "../mbedtls-cpp/Ssl.hpp"
#include "../mbedtls-cpp/StandardCookie.hpp"
#include "../mbedtls-cpp/StandardEntropy.hpp"
#include "../mbedtls-cpp/Timer.hpp"

#include <lwip/udp.h>

#include <cstdint>
#include <vector>

namespace lwip_async
{

class DtlsInputOutput : private tls::BasicInputOutput
{
public:
    explicit DtlsInputOutput(luna::server::StrandDecoder * strandDecoder);
    ~DtlsInputOutput() override;

    uint16_t port() const;
    std::vector<uint8_t> const & sharedKey() const;
private:
    using StepFunctionType = bool (DtlsInputOutput::*)(ip_addr_t const * address, u16_t port);

    mbedtls_ssl_send_t * getSender() override;
    mbedtls_ssl_recv_t * getReceiver() override;
    mbedtls_ssl_recv_timeout_t * getReceiverTimeout() override;
    void * getContext() override;

    int write(uint8_t const * data, size_t length);
    void udpReceive(pbuf * buffer, ip_addr_t const * address, u16_t port);

    bool handshakeStep(ip_addr_t const * address, u16_t port);
    bool readDataStep(ip_addr_t const * address, u16_t port);

    luna::server::StrandDecoder * mStrandDecoder;

    udp_pcb * mUdp;
    PbufStream mBuffer;

    Timer mTimer;

    StepFunctionType mCurrentStep;

    tls::StandardEntropy mEntropy;
    tls::CounterDeterministicRandomGenerator mRandom;
    tls::StandardCookie mCookie;
    tls::Configuration mTlsConfiguration;
    tls::Ssl mSsl;

    std::vector<uint8_t> mSharedKey;
};

};