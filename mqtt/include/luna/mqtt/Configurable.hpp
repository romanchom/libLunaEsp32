#pragma once

#include "Topic.hpp"
#include "Property.hpp"

#include <functional>
#include <set>
#include <string>
#include <string_view>

namespace luna::mqtt
{
    struct Configurable
    {
        explicit Configurable(std::string_view name);
        void setProperty(std::string_view propertyName, std::string_view text);
    protected:
        void addProperty(std::string_view name, std::function<void(std::string_view)> && setter);
        ~Configurable();

        std::set<Property, PropertyNameCompare> mProperties;
        nvs_handle mHandle;
    };
}
