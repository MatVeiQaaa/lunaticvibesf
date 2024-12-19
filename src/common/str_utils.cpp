#include "str_utils.h"

#include <cstring>
#include <span>
#include <string_view>

void lunaticvibes::assign(std::span<char> to, std::string_view from)
{
    size_t arrn = to.size();
    if (arrn == 0)
        arrn = 1; // Saturating subtraction.
    const size_t bytes = std::min(arrn - 1, from.size());
    strncpy(to.data(), from.data(), bytes);
    to[bytes] = '\0';
}
