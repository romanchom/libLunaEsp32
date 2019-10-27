#include "Topic.hpp"

namespace luna::mqtt
{

    bool Level::singleLevelWildcard() const noexcept
    {
        return mLevel[0] == '+';
    }

    bool Level::multiLevelWildcard() const noexcept
    {
        return mLevel[0] == '#';
    }

    bool Level::matches(Level const & other) const noexcept
    {
        if (singleLevelWildcard() || other.singleLevelWildcard()) {
            return true;
        } else {
            return mLevel == other.mLevel;
        }
    }

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

    bool Topic::matches(Topic const & other) const noexcept
    {
        for (int level = 0;; ++level) {
            if (size() == level || other.size() == level) {
                return size() == other.size();
            }

            auto lvlA = (*this)[level];
            auto lvlB = other[level];
            if (lvlA.multiLevelWildcard() || lvlB.multiLevelWildcard()) {
                return true;
            }

            if (!lvlA.matches(lvlB)) {
                return false;
            }
        }
    }
}