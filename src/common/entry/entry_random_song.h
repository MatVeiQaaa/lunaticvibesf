#pragma once

#include "entry.h"

namespace lunaticvibes
{

class EntryRandomChart final : public EntryBase
{
public:
    enum class Filter
    {
        Any,
        Failed,
        Unplayed,
    };

    explicit EntryRandomChart(std::string name, std::string name2, Filter filter);

    Filter filter() const { return _filter; };

private:
    Filter _filter;
};

} // namespace lunaticvibes
