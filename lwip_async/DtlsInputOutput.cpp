#include "DtlsInputOutput.hpp"

#include <algorithm>
#include <cstring>

#include <esp_log.h>

static auto const TAG = "DTLS_IO";

static int const cipherSuites[] = {
    MBEDTLS_TLS_PSK_WITH_AES_128_CCM,
    MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA,
    0,
};


namespace lwip_async
{

DtlsInputOutput::DtlsInputOutput(luna::server::StrandDecoder * strandDecoder) :
    mStrandDecoder(strandDecoder),
    mUdp(udp_new()),
    mCurrentStep(&DtlsInputOutput::handshakeStep)
{
    ESP_LOGI(TAG, "Handshake successful");

    udp_recv(mUdp,
        [](void * arg, udp_pcb * pcb, pbuf * buffer, ip_addr_t const * addr, u16_t port) {
            reinterpret_cast<DtlsInputOutput *>(arg)->udpReceive(buffer, addr, port);
        },
        reinterpret_cast<void *>(this)
    );
    udp_bind(mUdp, reinterpret_cast<ip_addr_t const *>(IP4_ADDR_ANY), 0);

    mRandom.seed(&mEntropy, "asdqwe", 6);

    mCookie.setup(&mRandom);

    mTlsConfiguration.setRandomGenerator(&mRandom);
    mTlsConfiguration.setDefaults(tls::Endpoint::server, tls::Transport::datagram, tls::Preset::default_);
    mTlsConfiguration.setCipherSuites(cipherSuites);
    mTlsConfiguration.setDtlsCookies(&mCookie);

    char const identity[] = "Luna";

    mSharedKey.resize(16);
    mRandom.generate(mSharedKey.data(), mSharedKey.size());

    mTlsConfiguration.setSharedKey(
        mSharedKey.data(),
        mSharedKey.size(),
        reinterpret_cast<uint8_t const*>(identity),
        4
    );

    //mTlsConfiguration.enableDebug(&my_debug, 2);

    mSsl.setup(&mTlsConfiguration);

    mSsl.setInputOutput(this);
    mSsl.setTimer(&mTimer);
}

DtlsInputOutput::~DtlsInputOutput()
{
    ESP_LOGI(TAG, "Dtor");

    udp_remove(mUdp);
}

uint16_t DtlsInputOutput::port() const
{
    return mUdp->local_port;
}

std::vector<uint8_t> const & DtlsInputOutput::sharedKey() const
{
    return mSharedKey;
}

mbedtls_ssl_send_t * DtlsInputOutput::getSender()
{
    return [](void * context, const unsigned char * data, size_t length) {
        return reinterpret_cast<DtlsInputOutput *>(context)->write(data, length);
    };
}

mbedtls_ssl_recv_t * DtlsInputOutput::getReceiver()
{
    return [](void * context, unsigned char * data, size_t length) -> int {
        return reinterpret_cast<DtlsInputOutput *>(context)->mBuffer.read(data, length);
    };
}

mbedtls_ssl_recv_timeout_t * DtlsInputOutput::getReceiverTimeout()
{
    return nullptr;
}

void * DtlsInputOutput::getContext()
{
    return reinterpret_cast<void *>(this);
}

int DtlsInputOutput::write(unsigned char const * data, size_t length)
{
    auto buffer = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);

    memcpy(buffer->payload, data, length);
    buffer->len = buffer->tot_len = static_cast<u16_t>(length);

    udp_send(mUdp, buffer);

    pbuf_free(buffer);
    return length;
}

void DtlsInputOutput::udpReceive(pbuf * buffer, ip_addr_t const * address, u16_t port)
{
    if (nullptr == buffer) {
        return;
    }

    mBuffer.append(buffer);

    udp_connect(mUdp, address, port);

    while ((this->*mCurrentStep)(address, port));
}

bool DtlsInputOutput::handshakeStep(ip_addr_t const * address, u16_t port)
{
    mSsl.setClientId(reinterpret_cast<uint8_t const *>(address), sizeof(ip_addr_t));

    auto const ret = mSsl.handshakeStep();

    if (ret != ERR_OK) {
        ESP_LOGI(TAG, "Handshake step, %x", -ret);
        if (!(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
            ESP_LOGI(TAG, "Reset");
            mSsl.resetSession();
            mSsl.setInputOutput(this);

            return true;
        } else {
            return false;
        }
    }

    if (mSsl.inHandshakeOver()) {
        ESP_LOGI(TAG, "Handshake successful");
        mCurrentStep = &DtlsInputOutput::readDataStep;
    }
    return true;
}

bool DtlsInputOutput::readDataStep(ip_addr_t const * address, u16_t port)
{
    uint8_t buffer[1024];
    auto const bytesRead = mSsl.read(buffer, sizeof(buffer));
    if(bytesRead <= 0) return false;

    mStrandDecoder->decode(buffer, bytesRead);

    return false;
}


}