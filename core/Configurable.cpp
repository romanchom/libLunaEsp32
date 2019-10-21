#include "Configurable.hpp"

#include <nvs.h>

namespace luna
{
    Configurable::Configurable(std::string_view name)
    {
        std::string nameString(name);
        auto error = nvs_open(nameString.c_str(), NVS_READWRITE, &mHandle);
        if (error != ESP_OK) {
            throw std::runtime_error("Unable to open NVS.");
        }
    }

    Configurable::~Configurable()
    {
        nvs_close(mHandle);
    }

    void Configurable::setProperty(std::string_view propertyName, std::string_view text)
    {
        auto property = mProperties.find(propertyName);
        if (property != mProperties.end()) {
            property->set(text);
        }
    }

    void Configurable::addProperty(std::string_view name, std::function<void(std::string_view)> && setter)
    {
        mProperties.emplace(mHandle, std::move(name), std::move(setter));
    }
}
