#include "Topic.hpp"

namespace luna::mqtt
{
    Topic::Topic(std::string topic) :
        mText(std::move(topic))
    {
        size_t begin = 0;
        for (size_t i = 0; i < mText.size(); ++i) {
            if (mText[i] == '/') {
                mLevels.emplace_back(Range{begin, i - begin});
                begin = i + 1;
            }
        }
        mLevels.emplace_back(Range{begin, mText.size() - begin});
    }

    int Topic::compare(Topic const & other) const noexcept
    {
        for (int level = 0;; ++level) {
            if (size() == level || other.size() == level) {
                if (size() == other.size()) {
                    return 0;
                } else if (size() > level) {
                    return 1;
                } else {
                    return -1;
                }
            }

            auto lvlA = (*this)[level];
            auto lvlB = other[level];
            if (lvlA.multiLevelWildcard() || lvlB.multiLevelWildcard()) {
                return 0;
            }

            auto const comparison = lvlA.compare(lvlB);
            if (comparison == 0) {
                continue;
            } else {
                return comparison;
            }
        }
    }
}