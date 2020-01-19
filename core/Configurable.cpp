#include "Configurable.hpp"

// #include <nvs.h>

namespace luna
{
    Configurable::Configurable(std::string && name) :
        mName(std::move(name))
    {}

    Configurable::~Configurable() = default;

}
