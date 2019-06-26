#include "luna/esp32/RealtimeController.hpp"

#include <luna/proto/Builder.hpp>
#include <luna/proto/Command.hpp>

namespace luna::esp32
{
    RealtimeController::RealtimeController(HardwareController * controller, tls::PrivateKey * ownKey, tls::Certificate * ownCertificate, tls::Certificate * caCertificate) :
        mController(controller),
        mIo(ownKey, ownCertificate, caCertificate)
    {
        mIo.onDataRead([this](lwip_async::DtlsInputOutput& sender, std::byte const* data, size_t size) {
            dispatchCommand(data, size);
        });

        mIo.onConnected([this](lwip_async::DtlsInputOutput& sender, bool connected) {
            mController->enabled(connected);
        });
    }

    void RealtimeController::dispatchCommand(std::byte const * data, size_t size)
    {
        using namespace luna::proto;
        auto packet = reinterpret_cast<Command const*>(data);
        auto & command = packet->command;

        if (auto cmd = command.as<SetColor>()) {
            setColor(*cmd);
        }

        if (packet->id != 0) {
            std::byte buffer[32];
            auto builder = Builder(buffer);

            auto response = builder.allocate<Command>();
            response->ack = packet->id;

            mIo.write(builder.data(), builder.size());
        }
    }

    void RealtimeController::setColor(luna::proto::SetColor const& cmd)
    {
        using namespace luna::proto;

        auto & strandDatas = cmd.strands;

        auto strands = mController->strands();

        for (auto & data : strandDatas) {
            auto index = data.id;
            if (index >= strands.size()) continue;
            auto strand = strands[index];

            if (auto array = data.data.as<Array<RGB>>()) {
                static_cast<Strand<RGB> *>(strand)->setLight(array->data(), array->size(), 0);
            } else if (auto array = data.data.as<Array<Scalar<uint16_t>>>()) {
                static_cast<Strand<Scalar<uint16_t>> *>(strand)->setLight(array->data(), array->size(), 0);
            }
        }

        for (auto & strand : strands) {
            strand->render();
        }

        mController->update();
    }

}
