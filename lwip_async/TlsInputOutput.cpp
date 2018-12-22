// #include "TlsInputOutput.hpp"

// #include <algorithm>
// #include <esp_log.h>

// static char const TAG[] = "TLS_IO";

// namespace lwip_async
// {

// TlsInputOutput::TlsInputOutput(tcp_pcb * tcp, tls::Configuration * configuration, luna::StreamSink * receiver, StreamObserver * observer) :
//     mStreamSink(receiver),
//     mObserver(observer),
//     mCurrentStep(&TlsInputOutput::handshakeStep),
//     mTcp(tcp)
// {
//     ESP_LOGI(TAG, "Ctor");

//     mSsl.setup(configuration);
//     mSsl.setInputOutput(this);

//     tcp_arg(mTcp, reinterpret_cast<void *>(this));
//     tcp_recv(mTcp, [](void * arg, tcp_pcb * tpcb, pbuf * buffer, err_t err) -> err_t {
//         return reinterpret_cast<TlsInputOutput *>(arg)->receive(buffer);
//     });

//     tcp_err(mTcp, [](void * arg, err_t err) {
//         reinterpret_cast<TlsInputOutput *>(arg)->aborted(err);
//     });
// }

// TlsInputOutput::~TlsInputOutput()
// {
//     ESP_LOGI(TAG, "Dtor");

//     if (nullptr != mTcp) {
//         tcp_recv(mTcp, nullptr);
//         tcp_close(mTcp);
//     }
// }

// void TlsInputOutput::receiveStream(uint8_t const * data, size_t size)
// {
//     int const sent = mSsl.write(data, size);

//     if(static_cast<size_t>(sent) != size) {
//         throw 5;// TODO
//     }
// }

// mbedtls_ssl_send_t * TlsInputOutput::getSender()
// {
//     return [](void * context, const unsigned char * data, size_t length) -> int {
//         auto self = reinterpret_cast<TlsInputOutput *>(context);

//         if (ERR_OK == tcp_write(self->mTcp, (void *) data, static_cast<u16_t>(length), TCP_WRITE_FLAG_COPY)) {
//             return length;
//         } else {
//             return 0;
//         }
//     };
// }

// mbedtls_ssl_recv_t * TlsInputOutput::getReceiver()
// {
//     return [](void * context, unsigned char * data, size_t length) -> int {
//         return reinterpret_cast<TlsInputOutput *>(context)->mBuffer.read(data, length);
//     };
// }

// mbedtls_ssl_recv_timeout_t * TlsInputOutput::getReceiverTimeout()
// {
//     return nullptr;
// }

// void * TlsInputOutput::getContext()
// {
//     return reinterpret_cast<void *>(this);
// }

// bool TlsInputOutput::handshakeStep()
// {
//     auto const ret = mSsl.handshakeStep();
//     if (ret != 0) {
//         if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
//             return false;
//         } else {
//             throw tls::Exception(ret);
//         }
//     }

//     if (mSsl.inHandshakeOver()) {
//         mCurrentStep = &TlsInputOutput::decodeDataStep;
//     }
//     return true;
// }

// bool TlsInputOutput::decodeDataStep()
// {
//     uint8_t buffer[128];
//     while (true) {
//         auto const size = mSsl.read(buffer, sizeof(buffer));

//         if (size <= 0) break;

//         ESP_LOGI(TAG, "Read %d bytes", size);
//         mStreamSink->receiveStream(buffer, size);
//     }

//     return false;
// }

// err_t TlsInputOutput::receive(pbuf * buffer)
// {
//     if (nullptr == buffer) {
//         ESP_LOGI(TAG, "FIN received");
//         // receiving connection already closed
//         // now close only transmitting
//         tcp_err(mTcp, nullptr);
//         tcp_shutdown(mTcp, 0, 1);
//         mTcp = nullptr;

//         mObserver->streamClosed();
//     } else {
//         mBuffer.append(buffer);

//         try {
//             while ((this->*mCurrentStep)());
//         } catch (tls::Exception const & exception) {
//             ESP_LOGI(TAG, "Abort! FIN sent.");
//             tcp_abort(mTcp);
//             mTcp = nullptr;
//             mObserver->streamClosed();
//             return ERR_ABRT;
//         }

//         tcp_recved(mTcp, mBuffer.readCount());
//         mBuffer.resetReadCount();
//     }

//     return ERR_OK;
// }

// void TlsInputOutput::aborted(err_t reason)
// {
//     ESP_LOGI(TAG, "TCP error %d", reason);
//     mTcp = nullptr;
//     mObserver->streamClosed();
// }

// }
