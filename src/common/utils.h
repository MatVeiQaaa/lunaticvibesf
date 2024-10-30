#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "types.h"

namespace fs = std::filesystem;

inline Path executablePath;

// path may include wildcard (*, ?)
// Very expensive, use fs::directory_iterator or fs::recursive_directory_iterator when possible.
[[nodiscard]] std::vector<Path> findFiles(Path path, bool recursive = false);

bool isParentPath(Path parent, Path dir);

// string to int
[[nodiscard]] int toInt(std::string_view str, int defVal = 0) noexcept;
// string to double
[[nodiscard]] double toDouble(std::string_view str, double defVal = 0.0) noexcept;

namespace lunaticvibes
{
// strcasecmp
[[nodiscard]] bool iequals(std::string_view lhs, std::string_view rhs) noexcept;
} // namespace lunaticvibes

[[nodiscard]] constexpr unsigned base36(char c)
{
    return (c > '9') ? (c >= 'a' ? 10 + c - 'a' : 10 + c - 'A') : (c - '0');
}

[[nodiscard]] constexpr unsigned base36(char first, char second)
{
    return 36 * base36(first) + base36(second);
}

[[nodiscard]] constexpr unsigned base16(char c)
{
    return (c > '9') ? (c >= 'a' ? 10 + c - 'a' : 10 + c - 'A') : (c - '0');
}

[[nodiscard]] constexpr unsigned base16(char first, char second)
{
    return 16 * base16(first) + base16(second);
}

[[nodiscard]] std::string bin2hex(const void* bin, size_t size);
void hex2bin(std::string_view hex, unsigned char* buf);
[[nodiscard]] std::string hex2bin(std::string_view hex);

[[nodiscard]] std::string toLower(std::string_view s);
[[nodiscard]] std::string toUpper(std::string_view s);

[[nodiscard]] std::string convertLR2Path(const std::string& lr2path, const Path& relative_path);
[[nodiscard]] std::string convertLR2Path(const std::string& lr2path, const std::string& relative_path_utf8);
[[nodiscard]] std::string convertLR2Path(const std::string& lr2path, const char* relative_path_utf8);
[[nodiscard]] std::string convertLR2Path(const std::string& lr2path, std::string_view relative_path_utf8);

[[nodiscard]] Path PathFromUTF8(std::string_view s);

void preciseSleep(long long nanoseconds);

double normalizeLinearGrowth(double prev, double curr);

namespace lunaticvibes
{

// Trim leading and trailing symbols 'markers' from string 's'.
[[nodiscard]] constexpr std::string_view trim(const std::string_view s, const std::string_view markers)
{
    auto l = s.find_first_not_of(markers);
    if (l == std::string_view::npos)
        return {};
    const auto r = s.find_last_not_of(markers);
    return s.substr(l, r - l + 1);
}
// Trim spaces from string 's'.
[[nodiscard]] inline constexpr std::string_view trim(const std::string_view s)
{
    constexpr std::string_view WHITESPACE_MARKERS = " \t\n\v\f\r";
    return trim(s, WHITESPACE_MARKERS);
}

void trim_in_place(std::string& s);

[[nodiscard]] inline std::string trim(std::string&& s)
{
    trim_in_place(s);
    return s;
}

// Fix path separators and resolve case-insensitive paths on case-sensitive file systems.
// This does IO.
// Preserves non-existing paths. Does not trim redundant `./`.
// Length out of the returned string is the same as of the input string.
//
// This function assumes paths without drive letter assignments and
// operates on strings directly.
// In addition, directories must be separated by forward slashes.
//
// Example:
// On your system there is a `/tmp/AFileWithUpperCaseInIt`.
// `resolvePathCaseInsensitively("/tmp/afilewithuppercaseinit")` =
// `"/tmp/AFileWithUpperCaseInIt"`
std::string resolve_windows_path(std::string);

} // namespace lunaticvibes
