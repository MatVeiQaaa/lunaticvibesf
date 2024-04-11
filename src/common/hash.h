#pragma once

#include <cstring>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "utils.h"

template <size_t _Len>
class Hash
{
private:
    unsigned char data[_Len] = { 0 };
    bool set = false;
public:
    constexpr Hash() = default;
    explicit Hash(const std::string_view hex)
    {
        if (hex.size() != _Len * 2)
            throw std::runtime_error("invalid 'hex' length");
        set = true;
        hex2bin(hex, data);
    }
    template<size_t HexLen>
    explicit Hash(const char(&hex)[HexLen])
    {
        static_assert(HexLen - 1 == _Len * 2);
        set = true;
        hex2bin(hex, data);
    }

    constexpr size_t length() const { return _Len; }
    // TODO: remove this and use std::optional<Hash> where needed.
    constexpr bool empty() const { return !set; }
    std::string hexdigest() const { return bin2hex(data, _Len); }
    constexpr const unsigned char* hex() const { return data; }
    void reset() { set = false; memset(data, 0, _Len); }

    bool operator<(const Hash<_Len>& rhs) const { return memcmp(data, rhs.data, _Len) < 0; }
    bool operator>(const Hash<_Len>& rhs) const { return memcmp(data, rhs.data, _Len) > 0; }
    bool operator<=(const Hash<_Len>& rhs) const { return !(*this > rhs); }
    bool operator>=(const Hash<_Len>& rhs) const { return !(*this > rhs); }
    bool operator==(const Hash<_Len>& rhs) const { return memcmp(data, rhs.data, _Len) == 0; }
    bool operator!=(const Hash<_Len>& rhs) const { return memcmp(data, rhs.data, _Len) != 0; }

    friend struct std::hash<Hash<_Len>>;
};

template<size_t _Len>
struct std::hash<Hash<_Len>>
{
    size_t operator()(const Hash<_Len>& obj) const
    {
        return std::hash<std::string_view>()({reinterpret_cast<const char*>(obj.data), _Len});
    }
};

using HashMD5 = Hash<16>;

HashMD5 md5(std::string_view str);
HashMD5 md5file(const Path& filePath);
