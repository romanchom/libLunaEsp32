#include "DiscoveryResponder.hpp"

#include <lwip/udp.h>


#include <esp_log.h>
using namespace luna::proto;


namespace luna {
namespace esp32 {

DiscoveryResponder::DiscoveryResponder(uint16_t port, std::string const & name, std::vector<luna::proto::Strand> const & strands) :
    mUdp(udp_new())
{
    flatbuffers::FlatBufferBuilder builder(512);

    using namespace luna::proto;

    auto flatStrands = builder.CreateVectorOfStructs(strands); 
    auto flatName = builder.CreateString(name);
    
    auto discoveryBuilder = DiscoveryBuilder(builder);
    discoveryBuilder.add_port(port);
    discoveryBuilder.add_name(flatName);
    discoveryBuilder.add_strands(flatStrands);

    auto response = discoveryBuilder.Finish();
    builder.Finish(response);

    auto data = builder.GetBufferPointer();
    auto size = builder.GetSize();

    mResponse = std::vector<uint8_t>(data, data + size);

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
