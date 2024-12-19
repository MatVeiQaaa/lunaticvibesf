#pragma once

#include <functional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

namespace lunaticvibes
{

void assign(std::span<char> to, std::string_view from);

template <class Proj = std::identity>
[[nodiscard]] inline std::string join(char sep, std::ranges::input_range auto v, Proj proj = std::identity{})
{
    using std::begin, std::end;
    auto it = begin(v);
    auto e = end(v);
    if (it == e)
        return {};
    std::string s;
    s += *it++;
    for (; it != e; ++it)
    {
        s += sep;
        s += proj(*it);
    }
    return s;
}

} // namespace lunaticvibes
