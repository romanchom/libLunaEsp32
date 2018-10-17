#pragma once

namespace lwip_async
{

class StreamObserver
{
public:
    virtual ~StreamObserver() = default;
    virtual void streamClosed() = 0;
};

};