#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace luna
{
    struct AbstractProperty;

    struct Configurable
    {
        virtual std::vector<std::tuple<std::string, AbstractProperty *>> properties() { return {}; }
        virtual std::vector<std::tuple<std::string, Configurable *>> children() { return {}; }

    protected:
        ~Configurable();
    };
}
