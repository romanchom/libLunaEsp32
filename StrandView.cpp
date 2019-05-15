#include "luna/esp32/StrandView.hpp"

#include "luna/esp32/StrandDataProducer.hpp"

namespace luna::esp32 {

namespace {

struct MaskedProducer : StrandDataProducer {
    MaskedProducer(StrandDataProducer const * producer, Strand const * strand, size_t offset) :
        mProducer(producer),
        mStrand(strand),
        mOffset(offset)
    {}
    
    void produceRGB(StrandConfiguration const & config, RGB * destination) const override
    {
        destination += mOffset;
        mProducer->produceRGB(mStrand->configuration(), destination);
    }
private:
    StrandDataProducer const * mProducer;
    Strand const * mStrand;
    size_t mOffset;
};

}


StrandView::StrandView(std::shared_ptr<Strand> child, StrandConfiguration configuration, size_t begin) :
    Strand(configuration),
    mChild(std::move(child)),
    mBegin(begin)
{
    auto const & childConf = mChild->configuration();
    mConfiguration.colorChannels = childConf.colorChannels;
    mConfiguration.bitDepth = childConf.bitDepth;
    mConfiguration.colorSpace = childConf.colorSpace;
}

void StrandView::takeData(StrandDataProducer const * producer) 
{
    auto masked = MaskedProducer(producer, this, mBegin);
    mChild->takeData(&masked);
}

void StrandView::render() 
{
    mChild->render();
}

}
