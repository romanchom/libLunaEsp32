#include "RealtimeStreamer.hpp"

#include "DirectController.hpp"

#include <luna/TlsConfiguration.hpp>

#include <luna/proto/Builder.hpp>
#include <luna/proto/Command.hpp>

#include <esp_log.h>
#include <chrono>

static char const TAG[] = "RT";

namespace luna
{
    RealtimeStreamer::RealtimeStreamer(NetworkingContext const & context, DirectController * controller) :
        mTlsConfiguration(context.tlsConfiguration->makeDtlsConfiguration()),
        mController(controller),
        mSocket(*context.ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
        mHeartbeat(*context.ioContext),
        mTimer(context.ioContext),
        mIo(&mSocket)
    {
        mTimer.onTimeout([=](){
            doHandshake();
        });

        mSsl.setup(mTlsConfiguration.get());
        mSsl.setInputOutput(&mIo);
        mSsl.setTimer(&mTimer);

        startHandshake();
        ESP_LOGI(TAG, "Up on port %d", int(port()));
    }

    RealtimeStreamer::~RealtimeStreamer() = default;

    uint16_t RealtimeStreamer::port()
    {
        return mSocket.local_endpoint().port();
    }

    void RealtimeStreamer::reset()
    {
        ESP_LOGI(TAG, "Reset");
        mController->enabled(false);
        mSsl.resetSession();
        mSsl.setInputOutput(&mIo);
        mSsl.setTimer(&mTimer);
        sockaddr unspecified = { AF_UNSPEC };
        connect(mSocket.native_handle(), &unspecified, sizeof(unspecified)); // workaround asio connect being unable to disassociate UDP sockets
        startHandshake();
    }

    void RealtimeStreamer::startHandshake()
    {
        mSocket.async_wait(mSocket.wait_read, [this](asio::error_code const & error) {
            if (!error) {
                doHandshake();
            } else {
                startHandshake();
            }
        });
    }

    void RealtimeStreamer::doHandshake()
    {
        for (;;) {
            // workaround lwip recvmsg not returning sender address
            char asd[1];
            asio::ip::udp::endpoint sender;
            socklen_t sendsize = sender.capacity();
            int  bytes = recvfrom(mSocket.native_handle(), asd, 1, MSG_PEEK, (sockaddr*) sender.data(), &sendsize);

            if (bytes >= 0) {
                sender.resize(sendsize);
                mSocket.connect(sender);
                mSsl.setClientId(sender.data(), sender.size());
            }

            auto const ret = mSsl.handshakeStep();

            if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                startHandshake();
                break;
            } else if (ret != ERR_OK) {
                ESP_LOGI(TAG, "Handshake -%04x", -ret);
                reset();
                break;
            } else if (mSsl.inHandshakeOver()) {
                activate();
                break;
            }
        }
    }

    void RealtimeStreamer::startRead()
    {
        mSocket.async_wait(mSocket.wait_read, [this](asio::error_code const & error) {
            if (!error) {
                std::byte buffer[1024];
                try {
                    auto read = mSsl.read(buffer, sizeof(buffer));
                    if (read > 0) {
                        dispatchCommand(buffer, read);
                        startRead();
                        return;
                    } else {
                        mSsl.closeNotify();
                    }
                } catch (tls::Exception const & e) {
                    ESP_LOGW(TAG, "Exception int startRead() %s", e.what());
                }
            }
            reset();
        });

        mHeartbeat.expires_after(std::chrono::milliseconds(1000));
        mHeartbeat.async_wait([this](asio::error_code const & error) {
            if (!error) {
                ESP_LOGI(TAG, "Peer died");
                mSocket.cancel();
            }
        });
    }

    void RealtimeStreamer::activate()
    {
        ESP_LOGI(TAG, "Peer connected and authenticated");
        mController->enabled(true);
        startRead();
    }

    void RealtimeStreamer::dispatchCommand(std::byte const * data, size_t size)
    {
        using namespace luna::proto;
        auto packet = reinterpret_cast<Command const*>(data);
        auto & command = packet->command;

        if (auto cmd = command.as<SetColor>()) {
            mController->setColor(*cmd);
        }

        if (packet->id != 0) {
            std::byte buffer[32];
            auto builder = Builder(buffer);

            auto response = builder.allocate<Command>();
            response->ack = packet->id;

            mSsl.write(builder.data(), builder.size());
        }
    }
}
