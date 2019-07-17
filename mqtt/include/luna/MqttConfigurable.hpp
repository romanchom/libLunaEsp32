#pragma once

#include "MqttTopic.hpp"

#include <string_view>

namespace luna
{
    struct MqttConfigurable
    {
        virtual void configure(MqttTopic const & topic, std::string_view payload) = 0;
    protected:
        ~MqttConfigurable() = default;
    };
}
