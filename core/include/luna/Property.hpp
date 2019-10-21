#pragma once

#include <nvs.h>

#include <functional>
#include <string>

namespace luna
{
    struct Property
    {
        explicit Property(nvs_handle handle, std::string_view name, std::function<void(std::string_view)> && setter);
        std::string const & name() const { return mName; }
        void set(std::string_view value) const;

    private:
        nvs_handle mHandle;
        std::string const mName;
        std::function<void(std::string_view)> mSetter;
    };
}
