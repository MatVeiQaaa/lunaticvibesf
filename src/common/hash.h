#pragma once

#include <cstring>
#include <functional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

namespace lunaticvibes
{
[[nodiscard]] std::string bin2hex(std::span<const unsigned char> bin);
[[nodiscard]] inline std::string bin2hex(std::span<const char> bin)
{
    return bin2hex({reinterpret_cast<const unsigned char*>(bin.data()), bin.size()});
};
void hex2bin(std::string_view hex, std::span<unsigned char> buf);
[[nodiscard]] std::string hex2bin(std::string_view hex);

} // namespace lunaticvibes

template <size_t Len> class Hash
{
private:
    unsigned char data[Len] = {0};
    bool set = false;

public:
    constexpr Hash() = default;
    explicit Hash(const std::string_view hex)
    {
        if (hex.size() != Len * 2)
            throw std::runtime_error("invalid 'hex' length");
        set = true;
        lunaticvibes::hex2bin(hex, data);
    }
    template <size_t HexLen> explicit Hash(const char (&hex)[HexLen])
    {
        static_assert(HexLen - 1 == Len * 2);
        set = true;
        lunaticvibes::hex2bin(hex, data);
    }

    constexpr size_t length() const { return Len; }
    // TODO: remove this and use std::optional<Hash> where needed.
    constexpr bool empty() const { return !set; }
    std::string hexdigest() const { return lunaticvibes::bin2hex({data, Len}); }
    constexpr const unsigned char* hex() const { return data; }
    void reset()
    {
        set = false;
        memset(data, 0, Len);
    }

    bool operator<(const Hash<Len>& rhs) const { return memcmp(data, rhs.data, Len) < 0; }
    bool operator>(const Hash<Len>& rhs) const { return memcmp(data, rhs.data, Len) > 0; }
    bool operator<=(const Hash<Len>& rhs) const { return !(*this > rhs); }
    bool operator>=(const Hash<Len>& rhs) const { return !(*this > rhs); }
    bool operator==(const Hash<Len>& rhs) const { return memcmp(data, rhs.data, Len) == 0; }
    bool operator!=(const Hash<Len>& rhs) const { return memcmp(data, rhs.data, Len) != 0; }

    friend struct std::hash<Hash<Len>>;
};

template <size_t Len> struct std::hash<Hash<Len>>
{
    size_t operator()(const Hash<Len>& obj) const
    {
        return std::hash<std::string_view>()({reinterpret_cast<const char*>(obj.data), Len});
    }
};

using HashMD5 = Hash<16>;

HashMD5 md5(std::string_view str);
HashMD5 md5file(const std::filesystem::path& filePath);
