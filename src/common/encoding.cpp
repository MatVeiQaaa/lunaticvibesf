#include "encoding.h"

#include <fstream>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "common/assert.h"
#include "common/log.h"
#include "common/sysutil.h"

static bool is_ascii(const std::string_view str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        if (c > 0x7f)
            return false;
    }
    return true;
}

static bool is_shiftjis(const std::string_view str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;

        // ascii
        if (c <= 0x7f)
            continue;

        // hankaku gana
        if ((c >= 0xa1 && c <= 0xdf))
            continue;

        // JIS X 0208
        else if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xef))
        {
            if (++it == str.end())
                return false;
            uint8_t cc = *it;
            if ((cc >= 0x40 && cc <= 0x7e) || (cc >= 0x80 && cc <= 0xfc))
                continue;
        }

        // user defined
        else if (c >= 0xf0 && c <= 0xfc)
        {
            if (++it == str.end())
                return false;
            uint8_t cc = *it;
            if ((cc >= 0x40 && cc <= 0x7e) || (cc >= 0x80 && cc <= 0xfc))
                continue;
        }

        else
            return false;
    }

    return true;
}

static bool is_euckr(const std::string_view str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;

        // ascii
        if (c <= 0x7f)
            continue;

        // euc-jp
        if (c == 0x8e || c == 0x8f)
            return false;

        // shared range
        else if (c >= 0xa1 && c <= 0xfe)
        {
            if (++it == str.end())
                return false;
            uint8_t cc = *it;
            if (cc >= 0xa1 && cc <= 0xfe)
                continue;
        }

        else
            return false;
    }

    return true;
}

static bool is_utf8(const std::string_view str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        int bytes = 0;

        // invalid
        if ((c & 0b1100'0000) == 0b1000'0000 || (c & 0b1111'1110) == 0b1111'1110)
            return false;

        // 1 byte (ascii)
        else if ((c & 0b1000'0000) == 0)
            continue;

        // 2~6 bytes
        else if ((c & 0b1110'0000) == 0b1100'0000)
            bytes = 2;
        else if ((c & 0b1111'0000) == 0b1110'0000)
            bytes = 3;
        else if ((c & 0b1111'1000) == 0b1111'0000)
            bytes = 4;
        else if ((c & 0b1111'1100) == 0b1111'1000)
            bytes = 5;
        else if ((c & 0b1111'1110) == 0b1111'1100)
            bytes = 6;
        else
            return false;

        while (--bytes)
        {
            if (++it == str.end())
                return false;
            uint8_t cc = *it;
            if ((cc & 0b1100'0000) != 0b10000000)
                return false;
        }
    }

    return true;
}

eFileEncoding getFileEncoding(const Path& path)
{
    std::ifstream fs(path);
    if (fs.fail())
    {
        return eFileEncoding::LATIN1;
    }
    return getFileEncoding(fs);
}

eFileEncoding getFileEncoding(std::istream& is)
{
    std::streampos oldPos = is.tellg();

    is.clear();
    is.seekg(0);

    std::string buf;
    eFileEncoding enc = eFileEncoding::LATIN1;
    for (std::string buf; std::getline(is, buf);)
    {
        if (is_ascii(buf))
            continue;

        if (is_utf8(buf))
        {
            enc = eFileEncoding::UTF8;
            break;
        }
        if (is_euckr(buf))
        {
            enc = eFileEncoding::EUC_KR;
            break;
        }
        if (is_shiftjis(buf))
        {
            enc = eFileEncoding::SHIFT_JIS;
            break;
        }
    }

    is.clear();
    is.seekg(oldPos);

    if (enc == eFileEncoding::EUC_KR)
    {
        LOG_WARNING << "beep, boop, detected EUC-KR encoding (rare occurrence)";
    }

    return enc;
}

const char* getFileEncodingName(eFileEncoding enc)
{
    switch (enc)
    {
    case eFileEncoding::EUC_KR: return "EUC-KR";
    case eFileEncoding::LATIN1: return "Latin 1";
    case eFileEncoding::SHIFT_JIS: return "Shift JIS";
    case eFileEncoding::UTF8: return "UTF-8";
    default: return "Unknown";
    }
}

std::string to_utf8(const std::string& input, eFileEncoding fromEncoding)
{
    std::string out;
    lunaticvibes::to_utf8(input, fromEncoding, out);
    return out;
}

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void lunaticvibes::to_utf8(const std::string& input, eFileEncoding fromEncoding, std::string& out)
{
    int cp = CP_UTF8;
    switch (fromEncoding)
    {
    case eFileEncoding::SHIFT_JIS: cp = 932; break;
    case eFileEncoding::EUC_KR: cp = 949; break;
    case eFileEncoding::LATIN1: cp = CP_ACP; break;
    default: cp = CP_UTF8; break;
    }
    if (cp == CP_UTF8)
    {
        out = input;
        return;
    }

    DWORD dwNum;

    dwNum = MultiByteToWideChar(cp, 0, input.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[dwNum];
    MultiByteToWideChar(cp, 0, input.c_str(), -1, wstr, dwNum);

    dwNum = WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, NULL, 0, NULL, FALSE);
    char* ustr = new char[dwNum];
    WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, ustr, dwNum, NULL, FALSE);

    out = ustr;

    delete[] wstr;
    delete[] ustr;
}

std::string from_utf8(const std::string& input, eFileEncoding toEncoding)
{
    int cp = CP_UTF8;
    switch (toEncoding)
    {
    case eFileEncoding::SHIFT_JIS: cp = 932; break;
    case eFileEncoding::EUC_KR: cp = 949; break;
    case eFileEncoding::LATIN1: cp = CP_ACP; break;
    default: cp = CP_UTF8; break;
    }
    if (cp == CP_UTF8)
        return input;

    DWORD dwNum;

    dwNum = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[dwNum];
    MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, wstr, dwNum);

    dwNum = WideCharToMultiByte(cp, NULL, wstr, -1, NULL, 0, NULL, FALSE);
    char* lstr = new char[dwNum];
    WideCharToMultiByte(cp, NULL, wstr, -1, lstr, dwNum, NULL, FALSE);

    std::string ret(lstr);

    delete[] wstr;
    delete[] lstr;
    return ret;
}

#else

#include <cerrno>
#include <cstring>
#include <memory>
#include <type_traits>

#include <iconv.h>

static const char* get_iconv_encoding_name(eFileEncoding encoding)
{
    switch (encoding)
    {
    case eFileEncoding::LATIN1: return "ISO-8859-1";
    case eFileEncoding::SHIFT_JIS: return "CP932";
    case eFileEncoding::EUC_KR: return "CP949";
    case eFileEncoding::UTF8: return "UTF-8";
    }
    lunaticvibes::assert_failed("Incorrect eFileEncoding");
}

struct IcdDeleter
{
    void operator()(iconv_t icd)
    {
        int ret = iconv_close(icd);
        if (ret == -1)
        {
            const int error = errno;
            LOG_ERROR << "iconv_close() error: " << safe_strerror(error) << " (" << error << ")";
        }
    }
};
using IcdPtr = std::unique_ptr<std::remove_pointer_t<iconv_t>, IcdDeleter>;

static void convert(std::string_view input, eFileEncoding from, eFileEncoding to, std::string& out)
{
    thread_local std::map<std::pair<eFileEncoding, eFileEncoding>, IcdPtr> icds;

    auto icd_it = icds.find({from, to});
    if (icd_it == icds.end())
    {
        const auto* source_encoding_name = get_iconv_encoding_name(from);
        const auto* target_encoding_name = get_iconv_encoding_name(to);
        auto icd = IcdPtr(iconv_open(target_encoding_name, source_encoding_name));
        if (reinterpret_cast<size_t>(icd.get()) == static_cast<size_t>(-1))
        {
            const int error = errno;
            LOG_ERROR << "iconv_open() error: " << safe_strerror(error) << " (" << error << ")";
            out = "(conversion descriptor opening error)";
            return;
        }
        icd_it = icds.insert_or_assign({from, to}, std::move(icd)).first;
        LVF_DEBUG_ASSERT(icd_it != icds.end());
    }
    auto icd = icd_it->second.get();

    static constexpr size_t BUF_SIZE = 1024l * 32l;
    // I wanted to avoid manually allocating here so that we don't have
    // to clean up manually in all return paths.
    char out_buf[BUF_SIZE] = {0};

    // BRUH-cast.
    char* buf_ptr = const_cast<char*>(input.data());
    std::size_t buf_len = input.length();
    char* out_ptr = static_cast<char*>(out_buf);
    std::size_t out_len = sizeof(out_buf);

    std::size_t iconv_ret = iconv(icd, &buf_ptr, &buf_len, &out_ptr, &out_len);
    if (iconv_ret == static_cast<size_t>(-1))
    {
        const int error = errno;
        LOG_ERROR << "iconv() error: " << safe_strerror(error) << " (" << error << ")";
        out = "(conversion error)";
        return;
    }

    out = static_cast<char*>(out_buf);
}

void lunaticvibes::to_utf8(const std::string& input, eFileEncoding fromEncoding, std::string& buf)
{
    convert(input, fromEncoding, eFileEncoding::UTF8, buf);
}

std::string from_utf8(const std::string& input, eFileEncoding toEncoding)
{
    std::string buf;
    convert(input, eFileEncoding::UTF8, toEncoding, buf);
    return buf;
}

#endif // _WIN32

void lunaticvibes::utf8_to_utf32(const std::string& str, std::u32string& out)
{
    static const auto locale = std::locale("");
    static const auto& facet_u32_u8 = std::use_facet<std::codecvt<char32_t, char, std::mbstate_t>>(locale);
    out.resize(str.size() * facet_u32_u8.max_length(), '\0');

    std::mbstate_t s;
    const char* from_next = &str[0];
    char32_t* to_next = &out[0];

    std::codecvt_base::result res;
    do
    {
        res = facet_u32_u8.in(s, from_next, &str[str.size()], from_next, to_next, &out[out.size()], to_next);

        // skip unconvertiable chars (which is impossible though)
        if (res == std::codecvt_base::error)
            from_next++;

    } while (res == std::codecvt_base::error);

    out.resize(to_next - &out[0]);
}

std::string utf32_to_utf8(const std::u32string& str)
{
    static const auto locale = std::locale("");
    static const auto& facet_u32_u8 = std::use_facet<std::codecvt<char32_t, char, std::mbstate_t>>(locale);
    std::string u8Text(str.size() * 4, '\0');

    std::mbstate_t s;
    const char32_t* from_next = &str[0];
    char* to_next = &u8Text[0];

    std::codecvt_base::result res;
    do
    {
        res = facet_u32_u8.out(s, from_next, &str[str.size()], from_next, to_next, &u8Text[u8Text.size()], to_next);

        // skip unconvertiable chars (which is impossible though)
        if (res == std::codecvt_base::error)
            from_next++;

    } while (res == std::codecvt_base::error);

    u8Text.resize(to_next - &u8Text[0]);
    return u8Text;
}
