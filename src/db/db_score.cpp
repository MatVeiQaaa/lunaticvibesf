#include "db_score.h"

#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

#include "common/log.h"
#include "common/types.h"
#include "common/hash.h"

static constexpr auto&& CREATE_SCORE_BMS_TABLE_STR =
"CREATE TABLE IF NOT EXISTS score_bms( "
"md5 TEXT PRIMARY KEY UNIQUE NOT NULL, "
"notes INTEGER NOT NULL, "
"score INTEGER NOT NULL, "
"rate REAL NOT NULL, "
"fast INTEGER NOT NULL, "
"slow INTEGER NOT NULL, "
"maxcombo INTEGER NOT NULL DEFAULT 0, "
"addtime INTEGER NOT NULL DEFAULT 0, "
"pc INTEGER NOT NULL DEFAULT 0, "
"clearcount INTEGER NOT NULL DEFAULT 0, "
"exscore INTEGER NOT NULL, "
"lamp INTEGER NOT NULL, "
"pgreat INTEGER NOT NULL, "
"great INTEGER NOT NULL, "
"good INTEGER NOT NULL, "
"bad INTEGER NOT NULL, "
"bpoor INTEGER NOT NULL, "
"miss INTEGER NOT NULL, "
"bp INTEGER NOT NULL, "
"cb INTEGER NOT NULL, "
"replay TEXT "
")";
static constexpr size_t SCORE_BMS_PARAM_COUNT = 21;
struct score_bms_all_params
{
    std::string md5;
    long long notes = 0;
    long long score = 0;
    double    rate = 0;
    long long fast = 0;
    long long slow = 0;
    long long maxcombo = 0;
    long long addtime = 0;
    long long pc = 0;
    long long clearcount = 0;
    long long exscore = 0;
    long long lamp = 0;
    long long pgreat = 0;
    long long great = 0;
    long long good = 0;
    long long bad = 0;
    long long kpoor = 0;
    long long miss = 0;
    long long bp = 0;
    long long cb = 0;
    std::string replay;

    score_bms_all_params(const std::vector<std::any>& queryResult)
    {
        try
        {
            md5 = ANY_STR(queryResult.at(0));
            notes = ANY_INT(queryResult.at(1));
            score = ANY_INT(queryResult.at(2));
            rate = ANY_REAL(queryResult.at(3));
            fast = ANY_INT(queryResult.at(4));
            slow = ANY_INT(queryResult.at(5));
            maxcombo = ANY_INT(queryResult.at(6));
            addtime = ANY_INT(queryResult.at(7));
            pc = ANY_INT(queryResult.at(8));
            clearcount = ANY_INT(queryResult.at(9));
            exscore = ANY_INT(queryResult.at(10));
            lamp = ANY_INT(queryResult.at(11));
            pgreat = ANY_INT(queryResult.at(12));
            great = ANY_INT(queryResult.at(13));
            good = ANY_INT(queryResult.at(14));
            bad = ANY_INT(queryResult.at(15));
            kpoor = ANY_INT(queryResult.at(16));
            miss = ANY_INT(queryResult.at(17));
            bp = ANY_INT(queryResult.at(18));
            cb = ANY_INT(queryResult.at(19));
            replay = ANY_STR(queryResult.at(20));
        }
        catch (std::out_of_range&)
        {
        }
    }
};
bool convert_score_bms(ScoreBMS& out, const std::vector<std::any>& in)
{
    if (in.size() < SCORE_BMS_PARAM_COUNT) return false;

    score_bms_all_params params(in);

    out.notes = params.notes;
    out.score = params.score;
    out.rate = params.rate;
    out.fast = params.fast;
    out.slow = params.slow;
    out.maxcombo = params.maxcombo;
    out.addtime = params.addtime;
    out.playcount = params.pc;
    out.clearcount = params.clearcount;
    out.exscore = params.exscore;
    out.lamp = (ScoreBMS::Lamp)params.lamp;
    out.pgreat = params.pgreat;
    out.great = params.great;
    out.good = params.good;
    out.bad = params.bad;
    out.kpoor = params.kpoor;
    out.miss = params.miss;
    out.bp = params.bp;
    out.combobreak = params.cb;
    out.replayFileName = params.replay;
    return true;
}

static constexpr auto&& CREATE_SCORE_COURSE_BMS_TABLE_STR =
"CREATE TABLE IF NOT EXISTS score_course_bms( "
"md5 TEXT PRIMARY KEY UNIQUE NOT NULL, "
"notes INTEGER NOT NULL, "
"score INTEGER NOT NULL, "
"rate REAL NOT NULL, "
"fast INTEGER NOT NULL, "
"slow INTEGER NOT NULL, "
"maxcombo INTEGER NOT NULL DEFAULT 0, "
"addtime INTEGER NOT NULL DEFAULT 0, "
"pc INTEGER NOT NULL DEFAULT 0, "
"clearcount INTEGER NOT NULL DEFAULT 0, "
"exscore INTEGER NOT NULL, "
"lamp INTEGER NOT NULL, "
"pgreat INTEGER NOT NULL, "
"great INTEGER NOT NULL, "
"good INTEGER NOT NULL, "
"bad INTEGER NOT NULL, "
"bpoor INTEGER NOT NULL, "
"miss INTEGER NOT NULL, "
"bp INTEGER NOT NULL, "
"cb INTEGER NOT NULL, "
"replay TEXT "
")";

ScoreDB::ScoreDB(const char* path): SQLite(path, "SCORE")
{
    if (exec(CREATE_SCORE_BMS_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[ScoreDB] Create table score_bms ERROR! " << errmsg();
        abort();
    }
    if (exec(CREATE_SCORE_COURSE_BMS_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[ScoreDB] Create table score_course_bms ERROR! " << errmsg();
        abort();
    }
}

void ScoreDB::deleteScoreBMS(const char* tableName, const HashMD5& hash)
{
    const auto hashStr = hash.hexdigest();

    char sqlbuf[128] = { 0 };
    snprintf(static_cast<char*>(sqlbuf), sizeof(sqlbuf) - 1, "DELETE FROM %s WHERE md5=?", tableName);
    exec(static_cast<char*>(sqlbuf), {hashStr});

    cache[tableName].erase(hashStr);
}

std::shared_ptr<ScoreBMS> ScoreDB::getScoreBMS(const char* tableName, const HashMD5& hash) const
{
    return cache[tableName][hash.hexdigest()];
}

void ScoreDB::updateScoreBMS(const char* tableName, const HashMD5& hash, const ScoreBMS& score)
{
    std::string hashStr = hash.hexdigest();

    auto pRecord = getScoreBMS(tableName, hash);
    if (pRecord)
    {
        auto record = *pRecord;

        if (score.exscore > record.exscore)
        {
            record.rate = score.rate;
            record.fast = score.fast;
            record.slow = score.slow;
            record.exscore = score.exscore;
            record.pgreat = score.pgreat;
            record.great = score.great;
            record.good = score.good;
            record.bad = score.bad;
            record.kpoor = score.kpoor;
            record.miss = score.miss;
            record.combobreak = score.combobreak;
            record.replayFileName = score.replayFileName;
        }
        else if (score.exscore == record.exscore)
        {
            if (score.maxcombo > record.maxcombo || score.bp < record.bp || (int)score.lamp >(int)record.lamp)
                record.replayFileName = score.replayFileName;
        }

        record.bp = std::min(record.bp, score.bp);
        record.clearcount = std::max(record.clearcount, score.clearcount);
        record.maxcombo = std::max(record.maxcombo, score.maxcombo);
        record.notes = score.notes;
        record.playcount = std::max(record.playcount, score.playcount);
        record.score = std::max(record.score, score.score);

        if ((int)score.lamp > (int)record.lamp)
        {
            record.lamp = score.lamp;
        }

        char sqlbuf[224] = { 0 };
        sprintf(sqlbuf, "UPDATE %s SET notes=?,score=?,rate=?,fast=?,slow=?,maxcombo=?,addtime=?,pc=?,clearcount=?,exscore=?,lamp=?,"
            "pgreat=?,great=?,good=?,bad=?,bpoor=?,miss=?,bp=?,cb=?,replay=? WHERE md5=?", tableName);
        exec(sqlbuf,
            { record.notes, record.score, record.rate, record.fast, record.slow,
            record.maxcombo, (long long)std::time(nullptr), record.playcount, record.clearcount, record.exscore, (int)record.lamp,
            record.pgreat, record.great, record.good, record.bad, record.kpoor, record.miss, record.bp, record.combobreak, record.replayFileName,
            hashStr });
        cache[tableName].erase(hashStr);
    }
    else
    {
        char sqlbuf[224] = { 0 };
        sprintf(sqlbuf, "INSERT INTO %s(md5,notes,score,rate,fast,slow,maxcombo,addtime,pc,clearcount,exscore,lamp,"
            "pgreat,great,good,bad,bpoor,miss,bp,cb,replay) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", tableName);
        exec(sqlbuf,
            { hashStr,
            score.notes, score.score, score.rate, score.fast, score.slow,
            score.maxcombo, (long long)std::time(nullptr), score.playcount, score.clearcount, score.exscore, (int)score.lamp,
            score.pgreat, score.great, score.good, score.bad, score.kpoor, score.miss, score.bp, score.combobreak, score.replayFileName });
    }

    char sqlbuf[96] = { 0 };
    sprintf(sqlbuf, "SELECT * FROM %s WHERE md5=?", tableName);
    auto result = query(sqlbuf, { hashStr });
    if (!result.empty())
    {
        const auto& r = result[0];
        auto ret = std::make_shared<ScoreBMS>();
        convert_score_bms(*ret, r);
        cache[tableName][hashStr] = ret;
    }
}

void ScoreDB::deleteChartScoreBMS(const HashMD5& hash)
{
    return deleteScoreBMS("score_bms", hash);
}

std::shared_ptr<ScoreBMS> ScoreDB::getChartScoreBMS(const HashMD5& hash) const
{
    return getScoreBMS("score_bms", hash);
}

void ScoreDB::updateChartScoreBMS(const HashMD5& hash, const ScoreBMS& score)
{
    return updateScoreBMS("score_bms", hash, score);
}

void ScoreDB::deleteCourseScoreBMS(const HashMD5& hash)
{
    return deleteScoreBMS("score_course_bms", hash);
}

std::shared_ptr<ScoreBMS> ScoreDB::getCourseScoreBMS(const HashMD5& hash) const
{
    return getScoreBMS("score_course_bms", hash);
}

void ScoreDB::updateCourseScoreBMS(const HashMD5& hash, const ScoreBMS& score)
{
    return updateScoreBMS("score_course_bms", hash, score);
}

void ScoreDB::preloadScore()
{
    cache.clear();
    for (auto& r : query("SELECT * FROM score_course_bms"))
    {
        auto ret = std::make_shared<ScoreBMS>();
        convert_score_bms(*ret, r);
        cache["score_course_bms"].insert_or_assign(ANY_STR(r[0]), std::move(ret));
    }
    for (auto& r : query("SELECT * FROM score_bms"))
    {
        auto ret = std::make_shared<ScoreBMS>();
        convert_score_bms(*ret, r);
        cache["score_bms"].insert_or_assign(ANY_STR(r[0]), std::move(ret));
    }
}