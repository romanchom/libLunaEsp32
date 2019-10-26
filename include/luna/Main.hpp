#pragma once

#include <memory>

namespace luna
{
    struct Configuration;
    struct HardwareController;

    struct Main
    {
        explicit Main(Configuration const & config, HardwareController * controller);
        ~Main();
    private:
        struct Impl;
        std::unique_ptr<Impl> mImpl;
    };
}
