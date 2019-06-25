#include "DiscoveryResponder.hpp"

#include <luna/proto/Builder.hpp>
#include <luna/proto/Discovery.hpp>

#include <asio/buffer.hpp>
#include <esp_log.h>

#include <cstring>

namespace luna::esp32
{

static luna::proto::Point toProto(Point const & point) {
    auto ret = luna::proto::Point();
    ret.x = point.x;
    ret.y = point.y;
    ret.z = point.z;
    return ret;
}

static luna::proto::Location toProto(Location const & location) {
    return {
        toProto(location.begin),
        toProto(location.end)
    };
}

static luna::proto::UV toProto(CieColorCoordinates const & uv) {
    auto ret = luna::proto::UV();
    ret.u = uv.u;
    ret.v = uv.v;
    return ret;
}   

static luna::proto::ColorSpace toProto(ColorSpace const & colorSpace) {
    auto ret = luna::proto::ColorSpace();
    ret.white = toProto(colorSpace.white);
    ret.red = toProto(colorSpace.red);
    ret.green = toProto(colorSpace.green);
    ret.blue = toProto(colorSpace.blue);
    return ret;
}

DiscoveryResponder::DiscoveryResponder(asio::io_service & ioService, uint16_t port, std::string const & name, std::vector<StrandBase *> const & strands) :
    mSocket(ioService, asio::ip::udp::endpoint(asio::ip::udp::v4(), 9510))
{
    using namespace luna::proto;
    
    std::byte buffer[512];
    auto builder = Builder(buffer);
    auto discovery = builder.allocate<Discovery>();

    discovery->port = port;
    auto nameBuf = builder.allocate<char>(name.size() + 1);
    memcpy(nameBuf, name.c_str(), name.size() + 1);
    discovery->name = nameBuf;

    auto strandCount = strands.size();
    auto strandProperties = builder.allocate<StrandProperties>(strandCount);

    for (size_t i = 0; i < strandCount; ++i) {
        auto strand = strands[i];
        auto & property = strandProperties[i];
        property.id = i;
        property.format = strand->format();
        property.pixelCount = strand->pixelCount();
        property.location = toProto(strand->location());
        property.colorSpace = toProto(strand->colorSpace());
    }

    discovery->strands.set(strandProperties, strandCount);

    mResponse = std::vector<std::byte>(builder.data(), builder.data() + builder.size());

    startRespond();
}

DiscoveryResponder::~DiscoveryResponder() = default;

void DiscoveryResponder::startRespond()
{
    mSocket.async_receive_from(asio::buffer(mReadBuffer), mRemote,
        [this](asio::error_code const & error, std::size_t bytesTransferred) {
            ESP_LOGI("DiRe", "Responding");
            if (!error || error == asio::error::message_size) {
                mSocket.send_to(asio::buffer(mResponse), mRemote);
            }
            startRespond();
        }
    );
}

}
