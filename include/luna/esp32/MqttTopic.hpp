#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace luna::esp32
{
    struct MqttLevel
    {
        explicit MqttLevel(std::string level) :
            mLevel(std::move(level))
        {}

        bool singleLevelWildcard() const noexcept
        {
            return mLevel[0] == '+';
        }

        bool multiLevelWildcard() const noexcept
        {
            return mLevel[0] == '#';
        }

        bool operator==(MqttLevel const & other) const noexcept
        {
            if (singleLevelWildcard() || other.singleLevelWildcard()) {
                return true;
            }
            return mLevel == other.mLevel;
        }

        bool operator<(MqttLevel const & other) const noexcept
        {
            return mLevel < other.mLevel;
        }

        std::string const & str() const noexcept
        {
            return mLevel;
        }
    private:
        std::string mLevel;
    };

    struct MqttTopic
    {
        MqttTopic(MqttTopic && other) = default;
        MqttTopic(MqttTopic const &) = delete;

        MqttTopic & operator=(MqttTopic &&) = default;
        void operator=(MqttTopic const &) = delete;

        MqttTopic(std::string const & topic)
        {
            size_t begin = 0;
            for (size_t i = 0; i < topic.size(); ++i) {
                if (topic[i] == '/') {
                    mLevels.emplace_back(topic.substr(begin, i - begin));
                    begin = i + 1;
                }
            }
            mLevels.emplace_back(topic.substr(begin, topic.size() - begin));
        }

        std::string str() const noexcept
        {
            std::string ret;
            for (auto const & lvl : mLevels) {
                ret.append(lvl.str());
                ret.append("/");
            }
            ret.pop_back();
            return ret;
        }

        MqttLevel const & operator[](size_t index) const noexcept
        {
            return mLevels[index];
        }

        bool operator<(MqttTopic const & other) const noexcept
        {
            int level = 0;
            for (;;) {
                if (mLevels.size() == level && other.mLevels.size() > level) {
                    return true;
                }
                if (other.mLevels.size() == level) {
                    return false;
                }
                auto const & lvlA = mLevels[level];
                auto const & lvlB = other.mLevels[level];
                if (lvlA.multiLevelWildcard() || lvlB.multiLevelWildcard()) {
                    return false;
                }
                if (lvlA == lvlB) {
                    ++level;
                    continue;
                }
                return lvlA < lvlB;
            }
        }

        bool operator==(MqttTopic const & other) const noexcept
        {
            return !(*this < other) && !(other < *this); // TODO optimize
        }
    private:
        std::vector<MqttLevel> mLevels;
    };
}
