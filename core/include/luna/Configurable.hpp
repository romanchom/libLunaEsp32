#pragma once

#include "Property.hpp"

#include <nvs.h>

#include <functional>
#include <set>
#include <string>
#include <string_view>

namespace luna
{
    struct PropertyNameCompare
    {
        using is_transparent = void;

        bool operator()(Property const & left, Property const & right) const
        {
            return left.name() < right.name();
        }

        template<typename T>
        bool operator()(Property const & left, T && right) const
        {
            return left.name() < right;
        }

        template<typename T>
        bool operator()(T && left, Property const & right) const
        {
            return left < right.name();
        }
    };

    struct Configurable
    {
        explicit Configurable(std::string_view name);
        void setProperty(std::string_view propertyName, std::string_view text);
        std::string const & name() const noexcept { return mName; }
    protected:
        void addProperty(std::string_view name, std::function<void(std::string_view)> && setter);
        ~Configurable();

        std::string mName;
        std::set<Property, PropertyNameCompare> mProperties;
        nvs_handle mHandle;
    };
}
