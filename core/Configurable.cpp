#include "Configurable.hpp"

namespace luna
{
    Configurable::Configurable(std::string && name) :
        mName(std::move(name))
    {}

    Configurable::~Configurable() = default;

}
