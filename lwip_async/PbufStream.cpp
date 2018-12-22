#include "PbufStream.hpp"

#include <mbedtls/ssl.h>

#include <lwip/pbuf.h>

#include <algorithm>

namespace lwip_async
{

PbufStream::PbufStream() :
    mData(nullptr),
    mReadOffset(0),
    mReadThisCycle(0)
{}

int PbufStream::read(uint8_t * data, size_t length)
{
    if (nullptr == mData) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }

    uint16_t const bytesRead = pbuf_copy_partial(mData, data, static_cast<uint16_t>(length), mReadOffset);
    mReadOffset += bytesRead;

    while (nullptr != mData && mData->len <= mReadOffset) {
        auto next = pbuf_dechain(mData);
        
        mReadOffset -= mData->len;
        pbuf_free(mData);
        
        mData = next;
    }

    return static_cast<int>(bytesRead);
}

void PbufStream::append(pbuf * buffer)
{
    if (nullptr != mData) {
        pbuf_cat(mData, buffer);
    } else {
        mData = buffer;
    }
}

uint16_t PbufStream::readCount() const
{
    return mReadThisCycle;
}

void PbufStream::resetReadCount()
{
    mReadThisCycle = 0;
}

}