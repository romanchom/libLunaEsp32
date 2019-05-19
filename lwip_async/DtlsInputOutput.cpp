#include "DtlsInputOutput.hpp"

#include <algorithm>
#include <cstring>

#include <esp_log.h>

#include <lwip/timeouts.h>

static auto const TAG = "DTLS_IO";

namespace lwip_async
{

DtlsInputOutput::DtlsInputOutput(tls::PrivateKey & ownKey, tls::Certificate & ownCertificate, tls::Certificate & caCertificate) :
    mUdp(udp_new()),
    mCurrentStep(&DtlsInputOutput::handshakeStep)
{
    ESP_LOGI(TAG, "Ctor");

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
    mTlsConfiguration.setDtlsCookies(&mCookie);
    mTlsConfiguration.setOwnCertificate(&ownCertificate, &ownKey);
    mTlsConfiguration.setCertifiateAuthorityChain(&caCertificate);
    mTlsConfiguration.setAuthenticationMode(tls::AuthenticationMode::required);

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


int DtlsInputOutput::write(uint8_t const * data, size_t size)
{
    return mSsl.write(data, size);
}

mbedtls_ssl_send_t * DtlsInputOutput::getSender()
{
    return [](void * context, const unsigned char * data, size_t length) {
        return reinterpret_cast<DtlsInputOutput *>(context)->socketWrite(data, length);
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

int DtlsInputOutput::socketWrite(unsigned char const * data, size_t length)
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
    stopHeartbeat();
    startHeartbeat();

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
        if (mConnectedCallback) {
            mConnectedCallback(*this, true);
        }
        mCurrentStep = &DtlsInputOutput::readDataStep;
    }
    return true;
}

bool DtlsInputOutput::readDataStep(ip_addr_t const * address, u16_t port)
{
    uint8_t buffer[1024];
    auto const bytesRead = mSsl.read(buffer, sizeof(buffer));

    if (bytesRead > 0) {
        if (mDataReadCallback) {
            mDataReadCallback(*this, buffer, bytesRead);
        }
        return false;
    } else {
        ESP_LOGI(TAG, "Read error %x.", -bytesRead);

        switch(bytesRead) {
        case -1:
            reset();
            return true;
        case MBEDTLS_ERR_SSL_CLIENT_RECONNECT:
            ESP_LOGI(TAG, "Reconnect.");
            mCurrentStep = &DtlsInputOutput::handshakeStep;
            return true;
        default:
            return false;
        }
    }
}

void DtlsInputOutput::timeoutCallback(void * data)
{
    static_cast<DtlsInputOutput *>(data)->reset();
}

void DtlsInputOutput::startHeartbeat()
{
    sys_timeout(10000, &DtlsInputOutput::timeoutCallback, this);
}

void DtlsInputOutput::stopHeartbeat()
{
    sys_untimeout(&DtlsInputOutput::timeoutCallback, this);
}

void DtlsInputOutput::reset()
{
    ESP_LOGI(TAG, "Reset.");
    stopHeartbeat();
    mSsl.resetSession();
    mSsl.setInputOutput(this);
    udp_disconnect(mUdp);
    mCurrentStep = &DtlsInputOutput::handshakeStep;
    if (mConnectedCallback) {
        mConnectedCallback(*this, false);
    }
}

}
