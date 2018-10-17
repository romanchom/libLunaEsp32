#pragma once

#include <functional>
#include <cstdint>

#include <lwip/tcp.h>

#include <luna/StreamSink.hpp>

#include "../mbedtls-cpp/BasicInputOutput.hpp"
#include "../mbedtls-cpp/Ssl.hpp"

#include "StreamObserver.hpp"
#include "PbufStream.hpp"

namespace lwip_async
{

class TlsInputOutput : public luna::StreamSink, private tls::BasicInputOutput
{
public:
    TlsInputOutput(tcp_pcb * tcp, tls::Configuration * configuration, luna::StreamSink * receiver, StreamObserver * observer);
    ~TlsInputOutput() override;

    void receiveStream(uint8_t const * data, size_t size) override;
private:
    using StepFunction = bool (TlsInputOutput::*)();

    mbedtls_ssl_send_t * getSender() override;
    mbedtls_ssl_recv_t * getReceiver() override;
    mbedtls_ssl_recv_timeout_t * getReceiverTimeout() override;
    void * getContext() override;

    bool handshakeStep();
    bool decodeDataStep();

    err_t receive(pbuf * buffer);
    void aborted(err_t reason);

    luna::StreamSink * mStreamSink;
    StreamObserver * mObserver;
    StepFunction mCurrentStep;

    tcp_pcb * mTcp;
    tls::Ssl mSsl;
    PbufStream mBuffer;
};

}
