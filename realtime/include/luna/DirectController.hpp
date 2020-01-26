#pragma once

#include <luna/Controller.hpp>
#include <luna/proto/SetColor.hpp>

namespace luna
{
    struct Device;

    struct DirectController : Controller
    {
        explicit DirectController();
        void takeOwnership(Device * device) final;
        void releaseOwnership() final;
        void setColor(luna::proto::SetColor const& cmd);
    private:
        friend struct RealtimeStreamer;

        Device * mDevice;
    };

}
