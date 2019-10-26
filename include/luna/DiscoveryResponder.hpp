#pragma once

#include <luna/Strand.hpp>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <array>

struct udp_pcb;

namespace luna {

class DiscoveryResponder
{
public:
    explicit DiscoveryResponder(asio::io_context * ioContext, uint16_t port, std::string_view name, std::vector<Strand *> const & strands);
    ~DiscoveryResponder();

private:
    void startRespond();

    asio::ip::udp::socket mSocket;
    asio::ip::udp::endpoint mRemote;

    std::vector<std::byte> mResponse;
    std::array<std::byte, 1> mReadBuffer;
};

}
