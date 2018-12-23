#include "luna/esp32/NetworkManager.hpp"

#include "DiscoveryResponder.hpp"
#include "luna/esp32/StrandDataProducer.hpp"

#include <luna/proto/Commands_generated.h>
#include <luna/proto/Discovery_generated.h>

#include <esp_log.h>

static char const TAG[] = "Luna";

namespace luna {
namespace esp32 {

static luna::proto::Point toProto(Point const & point) {
    return luna::proto::Point(point.x, point.y, point.z);
}

static luna::proto::UV toProto(CieColorCoordinates const & uv) {
    return luna::proto::UV(uv.u, uv.v);
}   

static luna::proto::ColorSpace toProto(ColorSpace const & colorSpace) {
    return luna::proto::ColorSpace(
        toProto(colorSpace.white),
        toProto(colorSpace.red),
        toProto(colorSpace.green),
        toProto(colorSpace.blue)
    );
}

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

    auto & strands = mController->strands();

    std::vector<luna::proto::Strand> strandConfigs;
    strandConfigs.reserve(strands.size());

    for (int i = 0; i < strands.size(); ++i) {
        auto const & config = strands[i]->configuration();
        strandConfigs.emplace_back(
            i,
            (luna::proto::ColorChannels)config.colorChannels,
            config.pixelCount,
            toProto(config.begin),
            toProto(config.end),
            toProto(config.colorSpace)
        );
    }

    mDiscoveryResponder = std::make_unique<luna::esp32::DiscoveryResponder>(mSocket->port(), "Loszek", strandConfigs);
}

void NetworkManager::disable()
{
    mDiscoveryResponder.reset();
    mSocket.reset();
}

void NetworkManager::dispatchCommand(uint8_t const * data, size_t size)
{
    auto command = luna::proto::GetCommand(data);
    switch (command->command_type()) {
    case luna::proto::AnyCommand_SetColor:
        setColor(command->command_as_SetColor());
        break;
    default:
        ESP_LOGI(TAG, "Unrecognized command.");
        break;
    }
}

class ProtoDataProducer : public StrandDataProducer
{
public:
    explicit ProtoDataProducer(luna::proto::StrandData const * data) :
        mData(data)
    {}

    void produceRGB(StrandConfiguration const & config, RGB * rgb) const 
    {
        auto rgbData = mData->data_as_RGBData()->data();
        memcpy(rgb, rgbData->data(), rgbData->size() * 3);
    }
private:
    luna::proto::StrandData const * mData;
};

void NetworkManager::setColor(luna::proto::SetColor const * cmd)
{
    auto const strandDatas = cmd->strands();
    if (strandDatas == nullptr) return;

    auto & strands = mController->strands();

    for (auto strandData : *strandDatas) {
        auto index = strandData->id();
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