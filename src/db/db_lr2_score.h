#pragma once

#include <common/u8.h>
#include <db/db_conn.h>

#include <functional>

namespace lunaticvibes
{

struct Lr2Score
{
    std::string ghost;
    std::string hash;
    std::string scorehash;
    int bad;
    int clear;
    int clear_db;
    int clear_ex;
    int clear_sd;
    int clearcount;
    int complete;
    int failcount;
    int good;
    int great;
    int maxcombo;
    int minbp;
    int op_best;
    int op_history;
    int perfect;
    int playcount;
    int poor;
    int rank;
    int rate;
    int rseed;
    int totalnotes;
};

class Lr2ScoreDb : SQLite
{
public:
    explicit Lr2ScoreDb(const char* path);
    explicit Lr2ScoreDb(const Path& path) : Lr2ScoreDb(lunaticvibes::cs(path.u8string().c_str())) {}
    void proc(const std::function<void(const Lr2Score& score)>& cb);

    struct InMemoryRwTag
    {
    };
    explicit Lr2ScoreDb(InMemoryRwTag);
};

} // namespace lunaticvibes
