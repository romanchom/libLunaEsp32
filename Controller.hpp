#pragma once

#include <memory>

#include "ControlChannel.hpp"
#include "DiscoveryResponder.hpp"
#include "lwip_async/DtlsInputOutput.hpp"

#include <luna/server/Listener.hpp>
#include <luna/server/StrandDecoder.hpp>

namespace luna {
namespace esp32 {

class Controller : private luna::server::Listener
{
public:
    explicit Controller(ControlChannelConfig const & config, luna::server::StrandDecoder * strandDecoder);
    ~Controller() override;

private:
    void strandConfigurationRequested() override;
    void dataChannelRequested() override;
    void dataChannelClosed() override;
    void disconnected() override;

    void reset();

    luna::server::StrandDecoder * mStrandDecoder;
    ControlChannel mControlChannel;
    std::unique_ptr<lwip_async::DtlsInputOutput> mDtls;
    DiscoveryResponder mDiscoveryResponder;
};

}}
