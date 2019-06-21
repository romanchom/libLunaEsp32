#pragma once

#include "Strand.hpp"
#include "Located.hpp"

#include <cassert>
#include <iostream>

namespace luna::esp32 {

template<typename T>
struct StrandView : Located<Strand<T>>
{
    explicit StrandView(Location location, Strand<T> * child, size_t offset, size_t pixelCount);
    size_t pixelCount() const noexcept override;
    proto::Format format() const noexcept override;
    ColorSpace colorSpace() const noexcept override;
    void render() override;
    void setLight(T const * data, size_t size, size_t offset) override;
protected:
    Strand<T> * mChild;
    size_t mOffset;
    size_t mPixelCount;
};

template<typename T>
StrandView<T>::StrandView(Location location, Strand<T> * child, size_t offset, size_t pixelCount) :
    Located<Strand<T>>(location),
    mChild(child),
    mOffset(offset),
    mPixelCount(pixelCount)
{}

template<typename T>
size_t StrandView<T>::pixelCount() const noexcept
{
    return mPixelCount;
}

template<typename T>
proto::Format StrandView<T>::format() const noexcept 
{
    return mChild->format();
}

template<typename T>
ColorSpace StrandView<T>::colorSpace() const noexcept
{
    return mChild->colorSpace();
}

template<typename T>
void StrandView<T>::render()
{
    mChild->render();
}

template<typename T>
void StrandView<T>::setLight(T const * data, size_t size, size_t offset)
{
    assert(offset < mPixelCount);
    assert(offset + size <= mPixelCount);

    mChild->setLight(data, size, mOffset + offset);
}

}
