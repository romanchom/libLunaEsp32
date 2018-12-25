#include "luna/esp32/NetworkManager.hpp"

#include "DiscoveryResponder.hpp"
#include "luna/esp32/StrandDataProducer.hpp"

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
    mCaCertificate(configuration.caCertificate, configuration.caCertificateSize)
{}

NetworkManager::~NetworkManager() = default;

void NetworkManager::enable()
{
    mSocket = std::make_unique<lwip_async::DtlsInputOutput>(mOwnKey, mOwnCertificate, mCaCertificate);
    mSocket->onDataRead([this](lwip_async::DtlsInputOutput& sender, uint8_t const* data, size_t size) {
        dispatchCommand(data, size);
    });

    mSocket->onDisconnected([this](lwip_async::DtlsInputOutput& sender){
        turnOff();
    });


    mDiscoveryResponder = std::make_unique<luna::esp32::DiscoveryResponder>(mSocket->port(), "Loszek", *mController);
}

void NetworkManager::disable()
{
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

class ProtoDataProducer : public StrandDataProducer
{
public:
    explicit ProtoDataProducer(luna::proto::StrandData const& data) :
        mData(data)
    {}

    void produceRGB(StrandConfiguration const & config, RGB * rgb) const 
    {
        auto rgbData = mData.data.as<luna::proto::Array<luna::proto::RGB>>();
        memcpy(rgb, rgbData->data(), rgbData->size() * 3);
    }
private:
    luna::proto::StrandData const& mData;
};

void NetworkManager::setColor(luna::proto::SetColor const& cmd)
{
    auto & strandDatas = cmd.strands;

    auto & strands = mController->strands();

    for (auto & strandData : strandDatas) {
        auto index = strandData.id;
        if (index >= strands.size()) continue;
        
        auto & strand = strands[index];
        auto producer = ProtoDataProducer(strandData);
        strand->takeData(&producer);
    }
}

class BlackDataProducer : public StrandDataProducer
{
public:
    void produceRGB(StrandConfiguration const & config, RGB * rgb) const 
    {
        memset(rgb, 0, config.pixelCount * 3);
    }
};

void NetworkManager::turnOff()
{
    auto producer = BlackDataProducer();
    mController->setAll(&producer);
}

}
}