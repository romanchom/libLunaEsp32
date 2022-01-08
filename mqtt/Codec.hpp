#pragma once

#include <optional>
#include <string>

namespace luna::mqtt {
    struct Codec
    {
        explicit Codec(float scalarScale) :
            mScalarScale(scalarScale)
        {}

        template<typename T>
        std::optional<T> parse(std::string_view text) const;
        template<typename T>
        std::string serialize(T const & value) const;

    private:
        float const mScalarScale;
    };
}