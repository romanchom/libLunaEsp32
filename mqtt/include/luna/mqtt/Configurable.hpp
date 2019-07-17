#pragma once

#include "Topic.hpp"

#include <string_view>

namespace luna::mqtt
{
    struct Configurable
    {
        virtual void configure(Topic const & topic, std::string_view payload) = 0;
    protected:
        ~Configurable() = default;
    };
}
