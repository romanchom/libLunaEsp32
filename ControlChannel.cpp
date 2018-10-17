#include "ControlChannel.hpp"

#include <esp_log.h>

using namespace tls;
using namespace lwip_async;

static char const TAG[] = "ControlChannel";


static int certificate_verification_callback(void *, mbedtls_x509_crt *, int, uint32_t * flags) {
    constexpr uint32_t mask =  ~(MBEDTLS_X509_BADCERT_FUTURE
        | MBEDTLS_X509_BADCERT_EXPIRED
        | MBEDTLS_X509_BADCERT_CN_MISMATCH);

    *flags &= mask;
    return 0;
}


namespace luna {
namespace esp32 {

ControlChannel::ControlChannel(ControlChannelConfig const & configuration, luna::server::Listener * commandListener) :
    mListeningPort(configuration.listenPort),
    mTcpListeningConnection(nullptr),
    mPrivateKey(configuration.privateKey, configuration.privateKeySize),
    mCertificate(configuration.certificate, configuration.certificateSize),
    mCaCertificate(configuration.caCertificate, configuration.caCertificateSize),
    mRandom(&mEntropy, "Stuff", 6),
    mCommandListener(commandListener),
    mDecoder(commandListener),
    mDefragmenter(&mDecoder),
    mCommandConnection(nullptr)
{
    mConfiguration.setDefaults(Endpoint::server, Transport::stream, Preset::default_);
    mConfiguration.setRandomGenerator(&mRandom);
    mConfiguration.setCertifiateAuthorityChain(&mCaCertificate);
    mConfiguration.setOwnCertificate(&mCertificate, &mPrivateKey);
    mConfiguration.setCertificateVerificationCallback(certificate_verification_callback);
    //mConfiguration.enableDebug()

    ESP_LOGI(TAG, "Init");

    mTcpListeningConnection = tcp_new();
    auto error = tcp_bind(mTcpListeningConnection, IP_ADDR_ANY, mListeningPort);
    mTcpListeningConnection = tcp_listen(mTcpListeningConnection);

    tcp_arg(mTcpListeningConnection, reinterpret_cast<void *>(this));

    tcp_accept(mTcpListeningConnection, [](void * argument, tcp_pcb * newConnection, err_t error) -> err_t {
        auto self = reinterpret_cast<ControlChannel *>(argument);
        return self->acceptConnection(newConnection, error);
    });
}

err_t ControlChannel::acceptConnection(tcp_pcb * newConnection, err_t error)
{
    if (nullptr == mCommandConnection) {
        ESP_LOGI(TAG, "Accept");

        mCommandConnection.reset(new TlsInputOutput(
            newConnection,
            &mConfiguration,
            &mDefragmenter,
            this
        ));
    } else {
        ESP_LOGI(TAG, "Refuse connection");
        tcp_close(newConnection);
    }

    return ERR_OK;
}

luna::server::Encoder ControlChannel::commandEncoder()
{
    return luna::server::Encoder(mCommandConnection.get());
}

void ControlChannel::streamClosed()
{
    ESP_LOGI(TAG, "Closing");

    mCommandConnection.reset();
    mDefragmenter.reset();

    mCommandListener->disconnected();
}

}}
