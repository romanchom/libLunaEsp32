#pragma once

#include "Location.hpp"

namespace luna::esp32
{

template<typename T>
struct Located : T {
    template<typename... U>
    explicit Located(Location location, U... rest) :
        T(std::forward<U>(rest)...),
        mLocation(location)
    {}

    Location location() const noexcept override
    {
        return mLocation;
    }
private:
    Location mLocation;
};

}
