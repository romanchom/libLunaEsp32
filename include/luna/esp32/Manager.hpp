#pragma once

#include "ControlChannelConfig.hpp"

#include <luna/server/StrandDecoder.hpp>

#include <memory>

namespace luna {
namespace esp32 {

class Controller;

class Manager
{
public:
    explicit Manager();
    ~Manager();
private:
    std::unique_ptr<Controller> mController;
};

}
}