#pragma once

#include "luna/esp32/Strand.hpp"

#include <cstdint>
#include <string>
#include <vector>

struct udp_pcb;

namespace luna {
namespace esp32 {

class DiscoveryResponder
{
public:
    explicit DiscoveryResponder(uint16_t port, std::string const & name, std::vector<StrandBase *> const & strands);
    ~DiscoveryResponder();

private:
    udp_pcb * mUdp;

    std::vector<uint8_t> mResponse;
};

}
}
