#include "luna/esp32/NetworkManager.hpp"

#include "DiscoveryResponder.hpp"

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
    mCaCertificate(configuration.caCertificate, configuration.caCertificateSize),
    mUpdater(&mOwnKey, &mOwnCertificate, &mCaCertificate)
{}

NetworkManager::~NetworkManager() = default;

void NetworkManager::enable()
{
    mSocket = std::make_unique<lwip_async::DtlsInputOutput>(mOwnKey, mOwnCertificate, mCaCertificate);
    mSocket->onDataRead([this](lwip_async::DtlsInputOutput& sender, uint8_t const* data, size_t size) {
        dispatchCommand(data, size);
    });

    mSocket->onConnected([this](lwip_async::DtlsInputOutput& sender, bool connected){
        mController->enabled(connected);
    });

    mDiscoveryResponder = std::make_unique<luna::esp32::DiscoveryResponder>(mSocket->port(), "Loszek", mController->strands());

    mUpdater.start();
}

void NetworkManager::disable()
{
    mUpdater.stop();
    mDiscoveryResponder.reset();
    mSocket.reset();
}

void NetworkManager::dispatchCommand(uint8_t const * data, size_t size)
{

    using namespace luna::proto;
    auto packet = reinterpret_cast<Command const*>(data);
    auto & command = packet->command;

    if (auto cmd = command.as<SetColor>()) {
        setColor(*cmd);
    }

    if (packet->id != 0) {

        uint8_t buffer[32];
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

}
