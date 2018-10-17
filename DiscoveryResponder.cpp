#include "DiscoveryResponder.hpp"

#include <lwip/udp.h>

namespace luna {
namespace esp32 {

DiscoveryResponder::DiscoveryResponder() :
    mUdp(udp_new())
{
    udp_recv(mUdp,
        [](void * arg, udp_pcb * pcb, pbuf * buffer, ip_addr_t const * addr, u16_t port) {
            udp_sendto(pcb, buffer, addr, port);
            pbuf_free(buffer);
        },
        nullptr
    );

    udp_bind(mUdp, reinterpret_cast<ip_addr_t const *>(IP4_ADDR_ANY), 9510);
}

DiscoveryResponder::~DiscoveryResponder()
{
    udp_remove(mUdp);
}

}}
