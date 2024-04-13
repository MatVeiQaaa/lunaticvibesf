#include "chartformat.h"

#include <fstream>
#include <utility>

#include "chartformat_bms.h"
#include "common/encoding.h"
#include "common/log.h"

eChartFormat analyzeChartType(const Path& p)
{
    if (!p.has_extension())
        return eChartFormat::UNKNOWN;

    eChartFormat fmt = eChartFormat::UNKNOWN;

    auto extension = p.extension().u8string();
    if (extension.length() == 4)
    {
        if (lunaticvibes::iequals(extension, ".bms") ||
            lunaticvibes::iequals(extension, ".bme") ||
            lunaticvibes::iequals(extension, ".bml") ||
            lunaticvibes::iequals(extension, ".pms"))
            fmt = eChartFormat::BMS;
    }
    else if (extension.length() == 6)
    {
        if (lunaticvibes::iequals(extension, ".bmson"))
            fmt = eChartFormat::BMSON;
    }

    if (!fs::is_regular_file(p))
        return eChartFormat::UNKNOWN;
    else
        return fmt;
}

std::shared_ptr<ChartFormatBase> ChartFormatBase::createFromFile(const Path& path, uint64_t randomSeed)
{
    Path filePath = fs::absolute(path);
    std::ifstream fs(filePath.c_str());
    if (fs.fail())
    {
        LOG_WARNING << "[Chart] File invalid: " << filePath;
        return nullptr;
    }

    // dispatch Chart object upon filename extension
    switch (analyzeChartType(path))
    {
    case eChartFormat::BMS:
        return std::static_pointer_cast<ChartFormatBase>(std::make_shared<ChartFormatBMS>(filePath, randomSeed));

    case eChartFormat::UNKNOWN:
        LOG_WARNING << "[Chart] File type unknown: " << filePath;
        return nullptr;

    default:
        LOG_WARNING << "[Chart] File type unsupported: " << filePath;
        return nullptr;
    }

}

std::string ChartFormatBase::formatReadmeText(const std::vector<std::pair<std::string, std::string>>& files)
{
    std::string out;
    for (const auto& [file_name, text] : files)
    {
        out += file_name;
        out += ":\n";
        out += text;
        out += '\n';
    }
    return out;
}

bool ChartFormatBase::checkHasReadme() const
{
    return !findFiles(getDirectory() / "*.txt").empty();
}

std::vector<std::pair<std::string, std::string>> ChartFormatBase::getReadmeFiles() const
{
    using Pair = std::pair<std::string, std::string>;
    std::vector<Pair> out;
    auto files = findFiles(getDirectory() / "*.txt");
    out.reserve(files.size());
    for (const auto& file : files)
    {
        std::ifstream ifs{file};
        if (ifs.fail())
        {
            LOG_ERROR << "[Chart] Failed to open file " << file;
            continue;
        }
        auto encoding = getFileEncoding(ifs);
        std::string utf8_text;
        for (std::string line_; std::getline(ifs, line_);)
        {
            utf8_text += to_utf8(line_, encoding);
            utf8_text += '\n';
        }
        out.emplace_back(file.filename().u8string(), std::move(utf8_text));
    }
    std::sort(out.begin(), out.end(), [](const Pair& lhs, const Pair& rhs) { return lhs.first < rhs.first; });
    return out;
}

Path ChartFormatBase::getDirectory() const
{
    return absolutePath.parent_path();
}