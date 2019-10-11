#pragma once

#include <nvs.h>

#include <functional>
#include <string>

struct Property
{
    explicit Property(nvs_handle handle, std::string_view name, std::function<void(std::string_view)> && setter) :
        mHandle(handle),
        mName(name),
        mSetter(std::move(setter))
    {
        size_t bufferSize = 16;
        char buffer[bufferSize];
        esp_err_t error = nvs_get_str(mHandle, mName.c_str(), buffer, &bufferSize);
        if (error == ESP_OK) {
            mSetter(buffer);
        }
    }

    std::string const & name() const { return mName; }

    void set(std::string_view value) const
    {
        mSetter(value);
        std::string text(value);
        nvs_set_str(mHandle, mName.c_str(), text.c_str());
    }

private:
    nvs_handle mHandle;
    std::string const mName;
    std::function<void(std::string_view)> mSetter;
};

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
