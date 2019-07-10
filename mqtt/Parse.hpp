#pragma once

#include <string_view>
#include <charconv>
#include <optional>

namespace luna
{
    template<typename T, typename... Args>
    std::optional<T> tryParse(std::string_view text, Args... args)
    {
        T value;
        auto [_, error] = std::from_chars(text.data(), text.data() + text.size(), value, std::forward<Args>(args)...);
        if (error != std::errc()) {
            return std::nullopt;
        } else {
            return value;
        }
    }

    template<>
    inline std::optional<float> tryParse<float>(std::string_view text)
    {
        return strtof(text.data(), nullptr);
    }
}
