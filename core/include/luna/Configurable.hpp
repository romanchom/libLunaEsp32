#pragma once

#include "Property.hpp"

#include <string>

namespace luna
{
    struct Configurable
    {
        explicit Configurable(std::string && name);
        std::string const & name() const noexcept { return mName; }

        virtual std::vector<AbstractProperty *> properties() { return {}; }
        virtual std::vector<Configurable *> children() { return {}; }

    protected:
        ~Configurable();

    private:
        std::string const mName;
    };
}
