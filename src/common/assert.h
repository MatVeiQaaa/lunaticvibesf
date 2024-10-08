#pragma once

namespace lunaticvibes
{
[[noreturn]] void die_on_assertion_fail(const char* file, int line, const char* msg);
} // namespace lunaticvibes

#ifndef NDEBUG

// TODO(C++20): [[unlikely]] and std::source_location
#define LVF_DEBUG_ASSERT(cond)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
            lunaticvibes::die_on_assertion_fail(__FILE__, __LINE__, #cond);                                            \
    } while (0)

#else // NDEBUG

#define LVF_DEBUG_ASSERT(cond)

#endif // NDEBUG
