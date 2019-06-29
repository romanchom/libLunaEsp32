#include "luna/esp32/RealtimeController.hpp"

#include <luna/proto/Builder.hpp>
#include <luna/proto/Command.hpp>

#include <iostream>
#include <chrono>

char* if_indextoname(unsigned int , char* ) { return nullptr; }

namespace luna::esp32
{
    RealtimeController::RealtimeController(asio::io_context * ioContext, tls::Configuration * tlsConfiguration, HardwareController * controller) :
        mController(controller),
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
    }

    uint16_t RealtimeController::port()
    {
        return mSocket.local_endpoint().port();
    }

    void RealtimeController::reset() 
    {
        mController->enabled(false);
        mSsl.resetSession();
        mSsl.setInputOutput(&mIo);
        mSsl.setTimer(&mTimer);
        sockaddr unspecified = { AF_UNSPEC };
        connect(mSocket.native_handle(), &unspecified, sizeof(unspecified)); // workaround asio connect being unable to disassociate UDP sockets
        startHandshake();
    }

    void RealtimeController::startHandshake() 
    {
        mSocket.async_wait(mSocket.wait_read, [this](asio::error_code const & error) {
            if (!error) {
                doHandshake();
            } else {
                startHandshake();
            }
        });
    }

    void RealtimeController::doHandshake()
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
                reset();
                break;
            } else if (mSsl.inHandshakeOver()) {
                activate();
                break;
            }
        }
    }

    void RealtimeController::startRead()
    {
        mSocket.async_wait(mSocket.wait_read, [this](asio::error_code const & error) {
            if (!error) {
                std::byte buffer[1024];
                auto read = mSsl.read(buffer, sizeof(buffer));
                if (read > 0) {
                    dispatchCommand(buffer, read);
                    startRead();
                } else {
                    mSsl.closeNotify();
                    reset();
                }
            } else {
                reset();
            }
        });

        mHeartbeat.expires_after(std::chrono::milliseconds(1000));
        mHeartbeat.async_wait([this](asio::error_code const & error) {
            if (!error) {
                mSocket.cancel();
                reset();
            }
        });
    }

    void RealtimeController::activate()
    {
        mController->enabled(true);
        startRead();
    }

    void RealtimeController::dispatchCommand(std::byte const * data, size_t size)
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

    void RealtimeController::setColor(luna::proto::SetColor const& cmd)
    {
        using namespace luna::proto;

        auto & strandDatas = cmd.strands;

        auto strands = mController->strands();

        for (auto & data : strandDatas) {
            auto index = data.id;
            if (index >= strands.size()) continue;
            auto strand = strands[index];

            if (auto array = data.data.as<Array<RGB>>()) {
                static_cast<Strand<RGB> *>(strand)->setLight(array->data(), array->size(), 0);
            } else if (auto array = data.data.as<Array<Scalar<uint16_t>>>()) {
                static_cast<Strand<Scalar<uint16_t>> *>(strand)->setLight(array->data(), array->size(), 0);
            }
        }

        for (auto & strand : strands) {
            strand->render();
        }

        mController->update();
    }
}
