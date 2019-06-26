#pragma once

#include "HardwareController.hpp"

#include "lwip_async/DtlsInputOutput.hpp"

#include <luna/proto/SetColor.hpp>

namespace luna::esp32
{
    struct RealtimeController
    {
        RealtimeController(HardwareController * controller, tls::PrivateKey * ownKey, tls::Certificate * ownCertificate, tls::Certificate * caCertificate);
        uint16_t port() { return mIo.port(); }
    private:
        void dispatchCommand(std::byte const * data, size_t size);

        void setColor(luna::proto::SetColor const& cmd);
        
        HardwareController * mController;
        lwip_async::DtlsInputOutput mIo;
    };
}
