#pragma once

#include "luna/esp32/Strand.hpp"

#include <memory>
#include <cassert>
#include <iostream>

namespace luna::esp32 {

template<typename T>
struct StrandView : Strand<T>
{
    explicit StrandView(Location location, std::shared_ptr<Strand<T>> child, size_t offset, size_t pixelCount);
    size_t pixelCount() const noexcept final;
    proto::Format format() const noexcept final;
    ColorSpace colorSpace() const noexcept final;
    void render() final;
    void setLight(T const * data, size_t size, size_t offset) final;
protected:
    std::shared_ptr<Strand<T>> mChild;
    size_t mOffset;
    size_t mPixelCount;
};

template<typename T>
StrandView<T>::StrandView(Location location, std::shared_ptr<Strand<T>> child, size_t offset, size_t pixelCount) :
    Strand<T>(location),
    mChild(std::move(child)),
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
    // std::cout << "Strand: " << mPixelCount << " + " << mOffset << std::endl;
    // std::cout << "In: " << size << " + " << offset << std::endl;
    assert(offset < mPixelCount);
    assert(offset + size <= mPixelCount);

    mChild->setLight(data, size, mOffset + offset);
}

}
