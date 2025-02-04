#pragma once
#include "common/chartformat/chartformat.h"
#include "entry_folder.h"
#include <memory>
#include <utility>

// entry for individual song, e.g. jukebox/bms/L9
class EntryFolderSong : public EntryFolderBase
{
public:
    EntryFolderSong() = delete;
    EntryFolderSong(const HashMD5& md5, const Path& path, StringContentView name = "", StringContentView name2 = "")
        : EntryFolderBase(md5, name, name2), _path(path)
    {
        _type = eEntryType::SONG;
    }
    EntryFolderSong(std::shared_ptr<ChartFormatBase> pChart)
        : EntryFolderBase(pChart->fileHash, pChart->title, pChart->title2), _path(pChart->fileName)
    {
        pushChart(std::move(pChart));
        _type = eEntryType::SONG;
    }

protected:
    Path _path;
    std::vector<std::shared_ptr<ChartFormatBase>> charts;
    size_t idx = 0;

private:
    std::shared_ptr<EntryBase> getEntry(size_t idx) override { return nullptr; }
    void pushEntry(std::shared_ptr<EntryBase> f) override {}

public:
    Path getPath() override { return _path; }
    size_t getContentsCount() override { return charts.size(); }
    bool empty() override { return charts.empty(); }

    std::shared_ptr<ChartFormatBase> getChart(size_t idx);
    std::shared_ptr<ChartFormatBase> getCurrentChart();
    size_t incCurrentChart();
    void pushChart(std::shared_ptr<ChartFormatBase> c);

protected:
    std::map<int, std::map<unsigned, std::vector<std::shared_ptr<ChartFormatBase>>>> chartMap;

public:
    const std::vector<std::shared_ptr<ChartFormatBase>>& getDifficultyList(int gamemode, unsigned difficulty) const;
};

// entry for each charts of song, e.g. jukebox/bms/L9/kuso9_7.bme
class EntryChart : public EntryBase
{
public:
    std::shared_ptr<ChartFormatBase> _file = nullptr;

    // extend info
    std::shared_ptr<EntryFolderSong> _song = nullptr;

    Path getPath() override { return _file->absolutePath; }

public:
    EntryChart() = default;
    EntryChart(std::shared_ptr<ChartFormatBase> p, std::shared_ptr<EntryFolderSong> ps = nullptr)
        : _file(std::move(p)), _song(std::move(ps))
    {
        _type = eEntryType::CHART;
        md5 = _file->fileHash;
        _name = _file->title;
        _name2 = _file->title2;

        _addTime = _file->addTime;
    }
    EntryChart(ChartFormatBase& f) : EntryChart(std::make_shared<ChartFormatBase>(f)) {}

    std::shared_ptr<EntryFolderSong> getSongEntry() const { return _song; }
};

class ChartFormatBase;
class EntryFolderRegular : public EntryFolderBase
{
public:
    EntryFolderRegular() = delete;
    EntryFolderRegular(const HashMD5& md5, const Path& path, StringContentView name = "", StringContentView name2 = "")
        : EntryFolderBase(md5, name, name2), _path(path)
    {
        _type = eEntryType::FOLDER;
    }

protected:
    Path _path;

public:
    Path getPath() override { return _path; }
};

// entry for individual song, e.g. jukebox/bms/L9
class EntryFolderNewSong : public EntryFolderRegular
{
public:
    EntryFolderNewSong() = delete;
    EntryFolderNewSong(StringContentView name, StringContentView name2 = "")
        : EntryFolderRegular(HashMD5{}, "", name, name2)
    {
        _type = eEntryType::NEW_SONG_FOLDER;
    }
};
