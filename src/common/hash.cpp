#include "hash.h"

#include <span>
#include <string>
#include <string_view>

std::string lunaticvibes::bin2hex(std::span<const unsigned char> buf)
{
    std::string res;
    res.reserve(buf.size() * 2 + 1);
    constexpr char rgbDigits[] = "0123456789abcdef";
    for (const unsigned char c : buf)
    {
        res += rgbDigits[(c >> 4) & 0xf];
        res += rgbDigits[(c >> 0) & 0xf];
    }
    return res;
}

void lunaticvibes::hex2bin(std::string_view hex, std::span<unsigned char> buf)
{
    // LANDMINE. TODO: add size check;
    for (size_t i = 0, j = 0; i < hex.length() - 1; i += 2, j++)
    {
        unsigned char& c = buf[j];
        unsigned char c1 = tolower(hex[i]);
        unsigned char c2 = tolower(hex[i + 1]);
        c += (c1 + ((c1 >= 'a') ? (10 - 'a') : (-'0'))) << 4;
        c += (c2 + ((c2 >= 'a') ? (10 - 'a') : (-'0')));
    }
}
std::string lunaticvibes::hex2bin(std::string_view hex)
{
    std::string res;
    res.resize(hex.length() / 2);
    lunaticvibes::hex2bin(hex, {reinterpret_cast<unsigned char*>(res.data()), res.size()});
    return res;
}
