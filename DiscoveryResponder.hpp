#pragma once

struct udp_pcb;

namespace luna {
namespace esp32 {

class DiscoveryResponder
{
public:
    explicit DiscoveryResponder();
    ~DiscoveryResponder();
private:
    udp_pcb * mUdp;
};

}
}
