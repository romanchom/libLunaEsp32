#include "luna/esp32/RealtimeService.hpp"

#include <luna/proto/Builder.hpp>
#include <luna/proto/Command.hpp>

#include <esp_log.h>
#include <chrono>

char* if_indextoname(unsigned int , char* ) { return nullptr; }

static char const TAG[] = "RT";

namespace luna::esp32
{
    RealtimeService::RealtimeService(asio::io_context * ioContext, tls::Configuration * tlsConfiguration) :
        mController(nullptr),
        mSocket(*ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
        mHeartbeat(*ioContext),
        mTimer(ioContext),
        mIo(&mSocket)
    {
        mTimer.onTimeout([=](){
            doHandshake();
        });

        mSsl.setup(tlsConfiguration);
        mSsl.setInputOutput(&mIo);
        mSsl.setTimer(&mTimer);

        startHandshake();
        ESP_LOGI(TAG, "Up on port %d", int(port()));
    }

    RealtimeService::~RealtimeService() = default;

    uint16_t RealtimeService::port()
    {
        return mSocket.local_endpoint().port();
    }

    void RealtimeService::reset()
    {
        ESP_LOGI(TAG, "Reset");
        serviceEnabled(false);
        mSsl.resetSession();
        mSsl.setInputOutput(&mIo);
        mSsl.setTimer(&mTimer);
        sockaddr unspecified = { AF_UNSPEC };
        connect(mSocket.native_handle(), &unspecified, sizeof(unspecified)); // workaround asio connect being unable to disassociate UDP sockets
        startHandshake();
    }

    void RealtimeService::startHandshake()
    {
        mSocket.async_wait(mSocket.wait_read, [this](asio::error_code const & error) {
            if (!error) {
                doHandshake();
            } else {
                startHandshake();
            }
        });
    }

    void RealtimeService::doHandshake()
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

    void RealtimeService::startRead()
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

    void RealtimeService::takeOwnership(HardwareController * controller)
    {
        ESP_LOGI(TAG, "Enabled");
        mController = controller;
        mController->enabled(true);
    }

    void RealtimeService::releaseOwnership()
    {
        ESP_LOGI(TAG, "Disabled");
        mController = nullptr;
    }

    void RealtimeService::activate()
    {
        ESP_LOGI(TAG, "Peer connected and authenticated");
        serviceEnabled(true);
        startRead();
    }

    void RealtimeService::dispatchCommand(std::byte const * data, size_t size)
    {
        using namespace luna::proto;
        auto packet = reinterpret_cast<Command const*>(data);
        auto & command = packet->command;

        if (auto cmd = command.as<SetColor>()) {
            setColor(*cmd);
        }

        if (packet->id != 0) {
            std::byte buffer[32];
            auto builder = Builder(buffer);

            auto response = builder.allocate<Command>();
            response->ack = packet->id;

            mSsl.write(builder.data(), builder.size());
        }
    }

    void RealtimeService::setColor(luna::proto::SetColor const& cmd)
    {
        if (!mController) {
            return;
        }

        auto strands = mController->strands();

        for (auto & strandData : cmd.strands) {
            size_t index = strandData.id;
            if (index >= strands.size()) continue;
            auto strand = strands[index];

            strand->rawBytes(strandData.rawBytes.data(), strandData.rawBytes.size());
        }

        mController->update();
    }
}
