#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace luna::mqtt
{
    struct Level
    {
        explicit Level(std::string_view level) :
            mLevel(level)
        {}

        bool singleLevelWildcard() const noexcept;
        bool multiLevelWildcard() const noexcept;
        bool matches(Level const & other) const noexcept;

        std::string_view str() const noexcept
        {
            return mLevel;
        }
    private:
        std::string_view mLevel;
    };

    struct Topic
    {
        Topic(std::string topic);

        std::string const & str() const noexcept
        {
            return mText;
        }

        size_t size() const noexcept
        {
            return mLevels.size();
        }

        Level operator[](size_t index) const noexcept
        {
            auto range = mLevels[index];
            return Level(std::string_view(mText).substr(range.begin, range.size));
        }

        bool matches(Topic const & other) const noexcept;
    private:
        struct Range
        {
            size_t begin;
            size_t size;
        };

        std::string mText;
        std::vector<Range> mLevels;
    };
}
