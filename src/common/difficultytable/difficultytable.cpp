#include "difficultytable.h"

#include <algorithm>
#include <climits>
#include <functional>
#include <unordered_map>

std::vector<std::string> DifficultyTable::getLevelList() const
{
    std::vector<std::string> levelList;
    levelList.reserve(entries.size());
    for (const auto& entry : entries)
        levelList.push_back(entry.first);
    std::ranges::sort(levelList, std::bind_front(&DifficultyTable::compareByLevelOrder, this));
    return levelList;
}

std::vector<std::shared_ptr<EntryBase>> DifficultyTable::getEntryList(const std::string& level)
{
    if (auto it = entries.find(level); it != entries.end())
        return it->second;
    return {};
}

bool DifficultyTable::compareByLevelOrder(const std::string& first, const std::string& second) const
{
    auto getPosition = [](const std::unordered_map<std::string, int>& map, const std::string& key) -> int {
        if (const auto keyIt = map.find(key); keyIt != map.cend())
        {
            return keyIt->second;
        }

        // If it's a numeric key, return it's value.
        try
        {
            return std::stoi(key);
        }
        catch (...)
        {
            // Non-numeric key, ignore.
        }

        return INT_MAX;
    };

    const int firstPos = getPosition(_levelOrder, first);
    const int secondPos = getPosition(_levelOrder, second);

    return firstPos < secondPos;
}
