#include "Property.hpp"

namespace luna
{
    Property::Property(nvs_handle handle, std::string_view name, std::function<void(std::string_view)> && setter) :
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

    void Property::set(std::string_view value) const
    {
        mSetter(value);
        std::string text(value);
        nvs_set_str(mHandle, mName.c_str(), text.c_str());
    }
}
