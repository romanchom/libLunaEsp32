#pragma once

#include "luna/esp32/Strand.hpp"

#include <asio/io_service.hpp>
#include <asio/ip/udp.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <array>

struct udp_pcb;

namespace luna::esp32 {

class DiscoveryResponder
{
public:
    explicit DiscoveryResponder(asio::io_service & ioService, uint16_t port, std::string const & name, std::vector<StrandBase *> const & strands);
    ~DiscoveryResponder();

private:
    void startRespond();
    
    asio::ip::udp::socket mSocket;
    asio::ip::udp::endpoint mRemote;

    std::vector<std::byte> mResponse;
    std::array<std::byte, 1> mReadBuffer;
};

}
