#include "luna/esp32/NetworkManager.hpp"

#include "DiscoveryResponder.hpp"
#include "luna/esp32/RGB.hpp"

#include <luna/proto/Builder.hpp>
#include <luna/proto/Command.hpp>

#include <esp_log.h>

#include <cstring>

static char const TAG[] = "Luna";

namespace luna {
namespace esp32 {

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
        if (connected) {
            mController->enabled(true);
        } else {
            turnOff();
        }
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
    auto command = reinterpret_cast<luna::proto::Command const*>(data);
    command->command.call(this, &NetworkManager::setColor);

    if (command->id != 0) {
        using namespace luna::proto;

        uint8_t buffer[32];
        auto builder = Builder(buffer);

        auto response = builder.allocate<Command>();
        response->ack = command->id;

        mSocket->write(builder.data(), builder.size());
    }
}

void NetworkManager::setColor(luna::proto::SetColor const& cmd)
{
    auto & strandDatas = cmd.strands;

    auto strands = mController->strands();

    for (auto & data : strandDatas) {
        auto index = data.id;
        if (index >= strands.size()) continue;
        auto strand = strands[index];

        if (auto rgb = data.data.as<proto::Array<proto::RGB>>()) {
            static_cast<Strand<RGB<uint8_t>> *>(strand)->setLight(reinterpret_cast<RGB<uint8_t> const *>(rgb->data()), rgb->size(), 0);
        }

    }

    for (auto & strand : strands) {
        strand->render();
    }
}


void NetworkManager::turnOff()
{
    mController->enabled(false);

    auto strands = mController->strands();
    for (auto & strand : strands) {
        strand->render();
    }
}

}
}