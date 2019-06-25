#include "luna/esp32/NetworkManager.hpp"

#include "DiscoveryResponder.hpp"

#include "mbedtls-cpp/Configuration.hpp"
#include "mbedtls-cpp/CounterDeterministicRandomGenerator.hpp"
#include "mbedtls-cpp/StandardEntropy.hpp"

#include <luna/proto/Builder.hpp>
#include <luna/proto/Command.hpp>

#include <esp_log.h>

#include <cstring>

static char const TAG[] = "Luna";

namespace luna::esp32 {

NetworkManager::NetworkManager(NetworkManagerConfiguration const & configuration, HardwareController * controller) :
    mController(controller),
    mOwnKey(configuration.ownKey, configuration.ownKeySize),
    mOwnCertificate(configuration.ownCertificate, configuration.ownCertificateSize),
    mCaCertificate(configuration.caCertificate, configuration.caCertificateSize)//,
    // mUpdater(&mOwnKey, &mOwnCertificate, &mCaCertificate),
    // mMqtt("mqtt://192.168.1.1")
{
    // mMqtt.subscribe("/on", [this](void * data, size_t length) {
    //     if (length >= 1) {
    //         mEnabled = (*static_cast<char *>(data) == '1');
    //     }
    // });

    // mMqtt.subscribe("/whiteness", [this](void * data, size_t length) {
    //     std::string text(static_cast<char *>(data), length);
    //     mWhiteness = std::stof(text);
    // });

}

NetworkManager::~NetworkManager() = default;

void NetworkManager::enable()
{
    mSocket = std::make_unique<lwip_async::DtlsInputOutput>(mOwnKey, mOwnCertificate, mCaCertificate);
    mSocket->onDataRead([this](lwip_async::DtlsInputOutput& sender, std::byte const* data, size_t size) {
        dispatchCommand(data, size);
    });

    mSocket->onConnected([this](lwip_async::DtlsInputOutput& sender, bool connected) {
        mController->enabled(connected);
    });

    startDaemon();
    // mMqtt.connect();
}

void NetworkManager::disable()
{
    stopDaemon();
    mSocket.reset();
}

void NetworkManager::dispatchCommand(std::byte const * data, size_t size)
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

        mSocket->write(builder.data(), builder.size());
    }
}

void NetworkManager::setColor(luna::proto::SetColor const& cmd)
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

void NetworkManager::startDaemon()
{
    xTaskCreate((TaskFunction_t) &NetworkManager::daemonTask, "Daemon", 1024 * 8, this, 5, &mTaskHandle);
}

void NetworkManager::stopDaemon()
{
    mIoService->stop();
    mTaskHandle = 0;
}

void NetworkManager::daemonTask()
{
    tls::StandardEntropy mEntropy;
    tls::CounterDeterministicRandomGenerator mRandom;

    mRandom.seed(&mEntropy, "asdqwe", 6);

    tls::Configuration updaterConfiguration;

    updaterConfiguration.setRandomGenerator(&mRandom);
    updaterConfiguration.setDefaults(tls::Endpoint::server, tls::Transport::stream, tls::Preset::default_);
    updaterConfiguration.setOwnCertificate(&mOwnCertificate, &mOwnKey);
    updaterConfiguration.setCertifiateAuthorityChain(&mCaCertificate);
    updaterConfiguration.setAuthenticationMode(tls::AuthenticationMode::required);

    asio::io_service ioService;
    mIoService = &ioService;
    
    luna::esp32::DiscoveryResponder discoveryResponder(ioService, mSocket->port(), "Loszek", mController->strands());
    luna::esp32::Updater updater(ioService, &updaterConfiguration);

    ioService.run();

    vTaskDelete(nullptr);
}

}
