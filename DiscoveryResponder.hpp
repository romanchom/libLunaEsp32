#pragma once

#include "luna/proto/Discovery_generated.h"

#include <cstdint>
#include <string>
#include <vector>


struct udp_pcb;

namespace luna {
namespace esp32 {

class DiscoveryResponder
{
public:
    explicit DiscoveryResponder(uint16_t port, std::string const & name, std::vector<luna::proto::Strand> const & strands);
    ~DiscoveryResponder();

private:
    udp_pcb * mUdp;

    std::vector<uint8_t> mResponse;
};

}
}
