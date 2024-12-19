#pragma once

#include <any>
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <stdint.h>

#include "common/hash.h"
#include "common/types.h"
#include "common/u8.h"
#include "db/db_conn.h"

class ScoreBase;
class ScoreBMS;

namespace lunaticvibes
{

class Lr2ScoreDb;

struct OverallStats
{
    int64_t play_count;
    int64_t clear_count;
    int64_t pgreat;
    int64_t great;
    int64_t good;
    int64_t bad;
    int64_t poor;
    int64_t running_combo; // Accumulates between plays.
    int64_t max_running_combo;
    int64_t playtime; // In milliseconds (LR2 used seconds).
};

} // namespace lunaticvibes

class ScoreDB : public SQLite
{
protected:
    mutable std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<ScoreBMS>>> cache;

public:
    ScoreDB() = delete;
    ScoreDB(const char* path);
    ScoreDB(const Path& path) : ScoreDB(lunaticvibes::cs(path.u8string())) {}
    ~ScoreDB() override = default;
    ScoreDB(ScoreDB&) = delete;
    ScoreDB& operator=(ScoreDB&) = delete;

protected:
    void deleteLegacyScoreBMS(const char* tableName, const HashMD5& hash);
    void updateLegacyScoreBMS(const char* tableName, const HashMD5& hash, const ScoreBMS& score);

    [[nodiscard]] std::shared_ptr<ScoreBMS> getScoreBMS(const char* tableName, const HashMD5& hash) const;

    void saveChartScoreBmsToHistory(const HashMD5& hash, const ScoreBMS& score);
    void updateCachedChartPbBms(const HashMD5& hash, const ScoreBMS& score);

public:
    [[nodiscard]] std::shared_ptr<ScoreBMS> fetchCachedPbBMS(const HashMD5& hash) const;
    [[nodiscard]] std::shared_ptr<ScoreBMS> getChartScoreBMS(const HashMD5& hash) const;
    void deleteAllChartScoresBMS(const HashMD5& hash);
    void insertChartScoreBMS(const HashMD5& hash, const ScoreBMS& score);

    void deleteCourseScoreBMS(const HashMD5& hash);
    [[nodiscard]] std::shared_ptr<ScoreBMS> getCourseScoreBMS(const HashMD5& hash) const;
    void updateCourseScoreBMS(const HashMD5& hash, const ScoreBMS& score);

    void importScores(lunaticvibes::Lr2ScoreDb& lr2_db);

    bool isBmsPbCacheEmpty();
    void rebuildBmsPbCache();
    void preloadScore();

    [[nodiscard]] lunaticvibes::OverallStats getStats();

    // Test things, don't normally use:

    void updateLegacyChartScoreBMS(const HashMD5& hash, const ScoreBMS& score);

private:
    void updateStats(const ScoreBMS& score);
    void initTables();
    [[nodiscard]] std::vector<std::pair<HashMD5, ScoreBMS>> fetchCachedPbBMSImpl(
        const std::string& sql_where, std::initializer_list<std::any> params) const;
};
