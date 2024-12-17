#pragma once

// TODO(C++20): [[unlikely]] and std::source_location

namespace lunaticvibes
{
[[noreturn]] void assert_failed(const char* file, int line, const char* msg);
void verify_failed(const char* file, int line, const char* msg);
} // namespace lunaticvibes

#define LVF_ASSERT(cond)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
            lunaticvibes::assert_failed(__FILE__, __LINE__, #cond);                                                    \
    } while (0)

#define LVF_VERIFY(cond)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
            lunaticvibes::verify_failed(__FILE__, __LINE__, #cond);                                                    \
    } while (0)

#define LVF_ASSERT_FALSE(msg) lunaticvibes::assert_failed(__FILE__, __LINE__, msg)

#ifndef NDEBUG
#define LVF_DEBUG_ASSERT(cond) LVF_ASSERT(cond)
#else // NDEBUG
#define LVF_DEBUG_ASSERT(cond)
#endif // NDEBUG
