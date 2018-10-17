#pragma once

#include <memory>

#include <lwip/tcp.h>

#include <luna/Defragmenter.hpp>
#include <luna/server/Listener.hpp>
#include <luna/server/Decoder.hpp>
#include <luna/server/Encoder.hpp>

#include "mbedtls-cpp/PrivateKey.hpp"
#include "mbedtls-cpp/Certificate.hpp"
#include "mbedtls-cpp/StandardEntropy.hpp"
#include "mbedtls-cpp/CounterDeterministicRandomGenerator.hpp"
#include "mbedtls-cpp/Configuration.hpp"

#include <luna/esp32/ControlChannelConfig.hpp>

#include "lwip_async/TlsInputOutput.hpp"
#include "lwip_async/StreamObserver.hpp"

namespace luna {
namespace esp32 {

class ControlChannel : private lwip_async::StreamObserver
{
public:
    explicit ControlChannel(ControlChannelConfig const & configuration, luna::server::Listener * commandListener);

    luna::server::Encoder commandEncoder();
private:
    err_t acceptConnection(tcp_pcb * newConnection, err_t error);

    void streamClosed() override;

    uint16_t mListeningPort;
    tcp_pcb * mTcpListeningConnection;

    tls::PrivateKey mPrivateKey;
    tls::Certificate mCertificate;
    tls::Certificate mCaCertificate;

    tls::StandardEntropy mEntropy;
    tls::CounterDeterministicRandomGenerator mRandom;

    tls::Configuration mConfiguration;

    luna::server::Listener * mCommandListener;
    luna::server::Decoder mDecoder;
    luna::Defragmenter mDefragmenter;

    std::unique_ptr<lwip_async::TlsInputOutput> mCommandConnection;
};

}}
