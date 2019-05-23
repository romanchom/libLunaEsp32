#include "luna/esp32/Strand.hpp"
#include "luna/esp32/RGB.hpp"

namespace luna::esp32
{

template<>
proto::Format Strand<RGB<uint8_t>>::format() const noexcept
{
    return proto::Format::RGB8;
}

}
