#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace lunaticvibes
{

[[nodiscard]] inline std::string u8str(const std::filesystem::path& p)
{
    const auto s = p.u8string();
    return {reinterpret_cast<const char*>(s.data()), s.size()};
};

[[nodiscard]] inline std::string_view s(const std::u8string_view s)
{
    // Don't try making this take fs::path, pray on lifetime extension.
    return {reinterpret_cast<const char*>(s.data()), s.size()};
};

[[nodiscard]] inline const char* cs(const std::u8string& s)
{
    // Don't try making this take fs::path, pray on lifetime extension.
    return reinterpret_cast<const char*>(s.c_str());
};

} // namespace lunaticvibes
