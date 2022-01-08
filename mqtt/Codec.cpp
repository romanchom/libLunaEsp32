#include "Codec.hpp"

#include <charconv>
#include <prism/Prism.hpp>
#include <esp_log.h>

namespace luna::mqtt
{
    template<>
    std::optional<bool> Codec::parse<bool>(std::string_view text) const
    {
        if (text == "0" || text == "off") { return false; }
        if (text == "1" || text == "on") { return true; }
        return std::nullopt;
    }

    template<>
    std::optional<int> Codec::parse<int>(std::string_view text) const
    {
        int value;
        auto [_, error] = std::from_chars(text.data(), text.data() + text.size(), value);
        if (error == std::errc()) {
            return value;
        }
        return std::nullopt;
    }

    template<>
    std::optional<float> Codec::parse<float>(std::string_view text) const
    {
        std::string cstr(text);
        return strtof(cstr.c_str(), nullptr) / mScalarScale;
    }

    template<>
    std::optional<std::string> Codec::parse<std::string>(std::string_view text) const
    {
        return std::string(text);
    }

    template<>
    std::optional<prism::RGB> Codec::parse<prism::RGB>(std::string_view text) const
    {
        if (text.size() == 7 && text[0] == '#') {
            prism::RGB color;
            for (int i = 0; i < 3; ++i) {
                auto channel = text.substr(1 + 2 * i, 2);
                int value;
                auto [_, error] = std::from_chars(channel.data(), channel.data() + channel.size(), value, 16);
                if (error == std::errc()) {
                    color[i] = value / 255.0f;
                }
            }
            prism::RGB ret;
            ret[3] = 0;
            ret.head<3>() = color.head<3>();
            return ret;
        }
        return std::nullopt;
    }

    template<>
    std::optional<prism::RGBW> Codec::parse<prism::RGBW>(std::string_view text) const
    {
        prism::RGBW color;
        auto begin = text.begin();
        for (int i = 0; i < 4; ++i) {
            auto comma = std::find(begin, text.end(), ',');
            uint8_t number;
            auto [_, error] = std::from_chars(begin, comma, number, 10);
            if (error != std::errc()) {
                return std::nullopt;
            }
            color[i] = number / 255.0f;
            begin = comma + 1;
        }
        return color;
    }

    template<>
    std::optional<prism::CieXY> Codec::parse<prism::CieXY>(std::string_view text) const
    {
        std::string cstr(text);
        prism::CieXY ret;
        if (2 == sscanf(cstr.c_str(), "%f,%f", &ret[0], &ret[1])) {
            return ret;
        } else {
            return std::nullopt;
        }
    }

    template<>
    std::string Codec::serialize<bool>(bool const & value) const
    {
        return value ? "1" : "0";
    }

    template<>
    std::string Codec::serialize<int>(int const & value) const
    {
        return std::to_string(value);
    }

    template<>
    std::string Codec::serialize<float>(float const & value) const
    {
        return std::to_string(value * mScalarScale);
    }

    template<>
    std::string Codec::serialize<std::string>(std::string const & value) const
    {
        return value;
    }

    template<>
    std::string Codec::serialize<prism::RGB>(prism::RGB const & value) const
    {
        auto rgb = (value.head<3>() * 255).array().cwiseMax(0).cwiseMin(255).round().cast<uint8_t>().eval();
        char buf[8];
        sprintf(buf, "#%02x%02x%02x", rgb[0], rgb[1], rgb[2]);
        return buf;
    }

    template<>
    std::string Codec::serialize<prism::RGBW>(prism::RGBW const & value) const
    {
        auto rgb = (value * 255).array().cwiseMax(0).cwiseMin(255).round().cast<uint8_t>().eval();
        char buf[16];
        sprintf(buf, "%d,%d,%d,%d", rgb[0], rgb[1], rgb[2], rgb[3]);
        return buf;
    }

    template<>
    std::string Codec::serialize<prism::CieXY>(prism::CieXY const & value) const
    {
        char buf[64];
        sprintf(buf, "%.4f,%.4f", value[0], value[1]);
        return buf;
    }
}