#pragma once

#include <cstdint>
#include <cstddef>

struct pbuf;

namespace lwip_async
{

class PbufStream
{
public:
    explicit PbufStream();

    int read(uint8_t * data, size_t length);
    void append(pbuf * buffer);

    uint16_t readCount() const;
    void resetReadCount();
private:
    pbuf * mData;

    uint16_t mReadOffset;
    uint16_t mReadThisCycle;
};

}