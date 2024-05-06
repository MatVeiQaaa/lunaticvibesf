#include "entry_random_song.h"

#include <sstream>
#include <string>

#include "common/entry/entry.h"
#include "common/hash.h"

lunaticvibes::EntryRandomChart::EntryRandomChart(std::string name, std::string name2, Filter filter) : _filter(filter)
{
    _type = eEntryType::RANDOM_CHART;
    _name = std::move(name);
    _name2 = std::move(name2);

    std::stringstream ss;
    ss << static_cast<int>(filter);
    ss << name;
    ss << name2;
    this->md5 = ::md5(ss.str());
};
