#include "table_bms.h"

#include <fstream>
#include <string>

#include <boost/format.hpp>
#include <curl/curl.h>
#include <re2/re2.h>
#include <tao/json.hpp>

#include <common/encoding.h>
#include <common/entry/entry_song.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/meta.h>
#include <common/types.h>
#include <common/utils.h>

enum class GetResult : uint8_t
{
    OK,
    ERR_UNKNOWN,
    ERR_SYSTEM,
    ERR_RESOLVE,
    ERR_CONNECT,
    ERR_WRITE,
    ERR_READ,
    ERR_HTTP,
    ERR_TIMEOUT
};

static size_t writeCb(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    std::string& ss = *static_cast<std::string*>(userp);
    ss.append(static_cast<const char*>(contents), realsize);
    return realsize;
}

GetResult GET(const std::string& url, std::string& result)
{
    std::string https, host, port, target;

    static LazyRE2 regexURL{R"(http(s?)\:\/\/(.+?)(?:\:([\d]{1,5}))?(\/(?:.*)*))"};
    if (!RE2::FullMatch(url, *regexURL, &https, &host, &port, &target))
        return GetResult::ERR_RESOLVE;

    if (port.empty())
        port = https.empty() ? "80" : "443";
    int iport = toInt(port);

    CURLcode code;

    struct CurlDeleter
    {
        void operator()(CURL* conn) { curl_easy_cleanup(conn); }
    };
    auto conn_real = std::unique_ptr<CURL, CurlDeleter>(curl_easy_init());
    CURL* conn = conn_real.get();

    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_FOLLOWLOCATION " << code;
        return GetResult::ERR_SYSTEM;
    }

    code = curl_easy_setopt(conn, CURLOPT_TIMEOUT, 10);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_TIMEOUT " << code;
        return GetResult::ERR_SYSTEM;
    }

    code = curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, 10);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_CONNECTTIMEOUT " << code;
        return GetResult::ERR_SYSTEM;
    }

    code = curl_easy_setopt(conn, CURLOPT_SSL_VERIFYHOST, 0);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_SSL_VERIFYHOST " << code;
        return GetResult::ERR_SYSTEM;
    }

    code = curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_SSL_VERIFYPEER " << code;
        return GetResult::ERR_SYSTEM;
    }

    code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_URL " << code;
        return GetResult::ERR_SYSTEM;
    }

    code = curl_easy_setopt(conn, CURLOPT_PORT, iport);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_PORT " << code;
        return GetResult::ERR_SYSTEM;
    }

    static curl_version_info_data* curlversion = curl_version_info(CURLVERSION_NOW);
    static std::string ua = (boost::format("curl/%d.%d.%d") % (curlversion->version_num >> 16 & 0xFF) %
                             (curlversion->version_num >> 8 & 0xFF) % (curlversion->version_num & 0xFF))
                                .str();
    code = curl_easy_setopt(conn, CURLOPT_USERAGENT, ua.c_str());
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_USERAGENT " << code;
        return GetResult::ERR_SYSTEM;
    }

    std::string bodyStream;
    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writeCb);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_WRITEFUNCTION " << code;
        return GetResult::ERR_SYSTEM;
    }
    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &bodyStream);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLOPT_WRITEDATA " << code;
        return GetResult::ERR_SYSTEM;
    }

    code = curl_easy_perform(conn);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] curl_easy_perform " << code;
        switch (code)
        {
        case CURLE_OUT_OF_MEMORY: return GetResult::ERR_SYSTEM;
        case CURLE_COULDNT_RESOLVE_HOST: return GetResult::ERR_RESOLVE;
        case CURLE_COULDNT_CONNECT:
        case CURLE_REMOTE_ACCESS_DENIED:
        case CURLE_SSL_CONNECT_ERROR: return GetResult::ERR_CONNECT;
        case CURLE_WRITE_ERROR: return GetResult::ERR_WRITE;
        case CURLE_READ_ERROR: return GetResult::ERR_READ;
        case CURLE_OPERATION_TIMEDOUT: return GetResult::ERR_TIMEOUT;
        default: return GetResult::ERR_UNKNOWN;
        }
    }

    long response_code = 0;
    code = curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &response_code);
    if (code != CURLE_OK)
    {
        LOG_ERROR << "[TableBMS] CURLINFO_RESPONSE_CODE " << code;
        return GetResult::ERR_SYSTEM;
    }

    if (response_code / 100 != 2)
    {
        LOG_ERROR << "[TableBMS] HTTP " << response_code;
        return GetResult::ERR_HTTP;
    }

    result = bodyStream;
    return GetResult::OK;
}

void DifficultyTableBMS::updateFromUrl(std::function<void(DifficultyTable::UpdateResult)> finishedCallback)
{
    if (!lunaticvibes::iequals(url.substr(0, 7), "http://") && !lunaticvibes::iequals(url.substr(0, 8), "https://"))
    {
        LOG_ERROR << "[TableBMS] URL error: " << url;
        finishedCallback(UpdateResult::WEB_PATH_ERROR);
        return;
    }
    std::string remotePath = url.substr(0, url.find_last_of('/') + 1);

    std::string webUrl, headerUrl, dataUrl;

    // parse HTML
    webUrl = url;
    if (!lunaticvibes::iequals(webUrl.substr(webUrl.length() - 5), ".json"))
    {
        std::string headerFileName;
        std::string body;

        LOG_INFO << "[TableBMS] GET: " << webUrl;
        auto result = GET(webUrl, body);
        if (result == GetResult::OK)
        {
            // <meta name="bmstable" content="./header.json">
            std::string content;
            static const LazyRE2 re{R"(meta +name=['"]bmstable['"] *content=['"](.+)['"])"};
            if (RE2::PartialMatch(body, *re, &content))
            {
                headerFileName = content;
                LOG_DEBUG << "[TableBMS] bmstable: " << content;
            }
        }
        else
        {
            switch (result)
            {
            case GetResult::ERR_RESOLVE: finishedCallback(DifficultyTable::UpdateResult::WEB_PATH_ERROR); break;
            case GetResult::ERR_CONNECT:
            case GetResult::ERR_WRITE:
            case GetResult::ERR_READ:
            case GetResult::ERR_HTTP: finishedCallback(DifficultyTable::UpdateResult::WEB_CONNECT_ERR); break;
            case GetResult::ERR_TIMEOUT: finishedCallback(DifficultyTable::UpdateResult::WEB_TIMEOUT); break;
            default: finishedCallback(DifficultyTable::UpdateResult::INTERNAL_ERROR); break;
            }
        }

        if (!headerFileName.empty())
        {
            if (lunaticvibes::iequals(headerFileName.substr(0, 7), "http://") ||
                lunaticvibes::iequals(headerFileName.substr(0, 8), "https://"))
                headerUrl = headerFileName;
            else
                headerUrl = remotePath + headerFileName;
            LOG_INFO << "[TableBMS] Header URL: " << headerUrl;
        }
        else
        {
            finishedCallback(UpdateResult::WEB_PARSE_FAILED);
            return;
        }
    }
    else
    {
        headerUrl = webUrl;
    }

    // parse Header
    if (!headerUrl.empty())
    {
        std::string body;

        LOG_INFO << "[TableBMS] GET header: " << headerUrl;
        auto result = GET(headerUrl, body);
        if (result == GetResult::OK)
        {
            if (!body.empty())
            {
                parseHeader(body);

                auto tablePath = getFolderPath();
                if (!fs::exists(tablePath))
                    fs::create_directories(tablePath);
                std::ofstream ofs(tablePath / "header.json", std::ios_base::binary);
                ofs << body;
                ofs.close();
            }
        }
        else
        {
            switch (result)
            {
            case GetResult::ERR_RESOLVE: finishedCallback(DifficultyTable::UpdateResult::WEB_PATH_ERROR); break;
            case GetResult::ERR_CONNECT:
            case GetResult::ERR_WRITE:
            case GetResult::ERR_READ:
            case GetResult::ERR_HTTP: finishedCallback(DifficultyTable::UpdateResult::WEB_CONNECT_ERR); break;
            case GetResult::ERR_TIMEOUT: finishedCallback(DifficultyTable::UpdateResult::WEB_TIMEOUT); break;
            default: finishedCallback(DifficultyTable::UpdateResult::INTERNAL_ERROR); break;
            }
        }

        if (!data_url.empty() && !name.empty())
        {
            if (lunaticvibes::iequals(data_url.substr(0, 7), "http://") ||
                lunaticvibes::iequals(data_url.substr(0, 8), "https://"))
                dataUrl = data_url;
            else
                dataUrl = remotePath + data_url;
            LOG_INFO << "[TableBMS] Data URL: " << dataUrl;
        }
        else
        {
            finishedCallback(UpdateResult::HEADER_PARSE_FAILED);
            return;
        }
    }

    // parse Data
    if (!dataUrl.empty())
    {
        std::string body;

        LOG_INFO << "[TableBMS] GET body: " << dataUrl;
        switch (GET(dataUrl, body))
        {
        case GetResult::OK: parseBody(body); break;
        case GetResult::ERR_RESOLVE: finishedCallback(DifficultyTable::UpdateResult::WEB_PATH_ERROR); return;
        case GetResult::ERR_CONNECT:
        case GetResult::ERR_WRITE:
        case GetResult::ERR_READ:
        case GetResult::ERR_HTTP: finishedCallback(DifficultyTable::UpdateResult::WEB_CONNECT_ERR); return;
        case GetResult::ERR_TIMEOUT: finishedCallback(DifficultyTable::UpdateResult::WEB_TIMEOUT); return;
        case GetResult::ERR_UNKNOWN:
        case GetResult::ERR_SYSTEM: finishedCallback(DifficultyTable::UpdateResult::INTERNAL_ERROR); return;
        }

        if (entries.empty())
        {
            finishedCallback(UpdateResult::DATA_PARSE_FAILED);
            return;
        }
    }

    finishedCallback(UpdateResult::OK);
}

bool DifficultyTableBMS::loadFromFile()
{
    auto tablePath = getFolderPath();
    entries.clear();

    if (!fs::exists(tablePath))
        return false;

    Path headerPath = tablePath / "header.json";
    Path dataPath = tablePath / "data.json";
    if (!fs::exists(headerPath))
    {
        LOG_WARNING << "[TableBMS] header.json not found!";
        return false;
    }
    if (!fs::exists(dataPath))
    {
        LOG_WARNING << "[TableBMS] data.json not found!";
        return false;
    }

    // parse Header
    std::ifstream headerifs(headerPath);
    if (headerifs.fail())
    {
        LOG_WARNING << "[TableBMS] Open header.json failed!";
        return false;
    }
    std::stringstream headerFile;
    headerFile << headerifs.rdbuf();
    headerifs.sync();
    headerifs.close();
    parseHeader(headerFile.str());

    // parse Data
    std::ifstream dataifs(dataPath);
    if (dataifs.fail())
    {
        LOG_WARNING << "[TableBMS] Open data.json failed!";
        return false;
    }
    std::stringstream dataFile;
    dataFile << dataifs.rdbuf();
    dataifs.sync();
    dataifs.close();
    parseBody(dataFile.str());

    return true;
}

Path DifficultyTableBMS::getFolderPath() const
{
    Path tablePath(GAMEDATA_PATH);
    tablePath /= "table";
    tablePath /= md5(url).hexdigest();
    return tablePath;
}

std::string DifficultyTableBMS::getSymbol() const
{
    return symbol;
}

void DifficultyTableBMS::parseHeader(const std::string& content)
{
    try
    {
        std::string_view bodyview = content;
        // Some tables start with a BOM, which needs to be removed
        if (bodyview.starts_with("\xef\xbb\xbf"))
            bodyview = bodyview.substr(3);

        tao::json::value header = tao::json::from_string(bodyview);
        try
        {
            name = header["name"].get_string();
        }
        catch (std::bad_variant_access& e)
        {
        }
        try
        {
            symbol = header["symbol"].get_string();
        }
        catch (std::bad_variant_access& e)
        {
        }
        try
        {
            data_url = header["data_url"].get_string();
        }
        catch (std::bad_variant_access& e)
        {
        }
        try
        {
            const auto& levelOrder = header["level_order"].get_array();
            int levelIndex = 0;
            for (const auto& level : levelOrder)
            {
                try
                {
                    _levelOrder[level.get_string()] = levelIndex;
                }
                catch (const std::bad_variant_access& e)
                {
                }
                levelIndex += 1;
            }
        }
        catch (const std::bad_variant_access& e)
        {
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR << "[TableBMS] Header JSON Error: " << to_utf8(e.what(), eFileEncoding::LATIN1);
    }
}

void DifficultyTableBMS::parseBody(const std::string& content)
{
    try
    {
        if (!content.empty())
        {
            std::string_view bodyview = content;
            if (bodyview.substr(0, 3) == "\xef\xbb\xbf")
            {
                // Some tables start with a BOM, which needs to be removed
                bodyview = bodyview.substr(3);
            }

            std::vector<tao::json::value> header = tao::json::from_string(bodyview).get_array();
            for (auto& entry : header)
            {
                std::string level, md5, name;
                switch (entry["level"].type())
                {
                case tao::json::type::UNSIGNED: level = std::to_string(entry["level"].get_unsigned()); break;
                case tao::json::type::SIGNED: level = std::to_string(entry["level"].get_signed()); break;
                case tao::json::type::STRING: level = entry["level"].get_string(); break;
                case tao::json::type::STRING_VIEW: level = entry["level"].get_string_view(); break;
                default: level = "0"; break;
                }

                try
                {
                    md5 = entry["md5"].get_string();
                }
                catch (std::bad_variant_access& e)
                {
                }

                try
                {
                    name = entry["title"].get_string();
                }
                catch (std::bad_variant_access& e)
                {
                }

                if (!level.empty() && !md5.empty())
                {
                    auto pEntry = std::make_shared<EntryChart>();
                    pEntry->md5 = HashMD5{md5};
                    pEntry->_name = name;
                    entries[level].push_back(pEntry);
                }
            }

            auto tablePath = getFolderPath();
            if (!fs::exists(tablePath))
                fs::create_directories(tablePath);
            std::ofstream ofs(tablePath / "data.json", std::ios_base::binary);
            ofs << content;
            ofs.close();
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR << "[TableBMS] Data JSON Error: " << to_utf8(e.what(), eFileEncoding::LATIN1);
    }
}
