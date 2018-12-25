#include "DiscoveryResponder.hpp"

#include <luna/proto/Builder.hpp>
#include <luna/proto/Discovery.hpp>

#include <lwip/udp.h>

#include <cstring>


namespace luna {
namespace esp32 {

static luna::proto::Point toProto(Point const & point) {
    auto ret = luna::proto::Point();
    ret.x = point.x;
    ret.y = point.y;
    ret.z = point.z;
    return ret;
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

DiscoveryResponder::DiscoveryResponder(uint16_t port, std::string const & name, HardwareController const & hardware) :
    mUdp(udp_new())
{
    using namespace luna::proto;
    
    uint8_t buffer[512];
    auto builder = Builder(buffer);
    auto discovery = builder.allocate<Discovery>();

    discovery->port = port;
    auto nameBuf = builder.allocate<char>(name.size() + 1);
    memcpy(nameBuf, name.c_str(), name.size() + 1);
    discovery->name = nameBuf;

    auto strandCount = hardware.strands().size();
    auto strandProperties = builder.allocate<StrandProperties>(strandCount);

    for (size_t i = 0; i < strandCount; ++i) {
        auto & strand = hardware.strands()[i]->configuration();
        auto & property = strandProperties[i];
        property.id = i;
        property.channels = (luna::proto::ColorChannels) strand.colorChannels;
        property.pixelCount = strand.pixelCount;
        property.begin = toProto(strand.begin);
        property.end = toProto(strand.end);
        property.colorSpace = toProto(strand.colorSpace);
    }

    discovery->strands.set(strandProperties, strandCount);

    mResponse = std::vector<uint8_t>(builder.data(), builder.data() + builder.size());

    udp_recv(mUdp,
        [](void * arg, udp_pcb * pcb, pbuf * buffer, ip_addr_t const * addr, u16_t port) {
            auto & responseData = *static_cast<std::vector<uint8_t>  *>(arg);

            auto response = pbuf_alloc(PBUF_TRANSPORT, responseData.size(), PBUF_REF);
            response->payload = responseData.data();
            response->len = static_cast<uint16_t>(responseData.size());
            response->tot_len = static_cast<uint16_t>(responseData.size());
            
            udp_sendto(pcb, response, addr, port);
            
            pbuf_free(response);

            pbuf_free(buffer);
        },
        &mResponse
    );

    udp_bind(mUdp, reinterpret_cast<ip_addr_t const *>(IP4_ADDR_ANY), 9510);
}

DiscoveryResponder::~DiscoveryResponder()
{
    udp_remove(mUdp);
}

}}
