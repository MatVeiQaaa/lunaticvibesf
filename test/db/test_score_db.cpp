#include <gmock/gmock.h>

#include <db/db_score.h>

static constexpr auto&& IN_MEMORY_DB_PATH = ":memory:";

TEST(ScoreDb, ChartScoreUpdating)
{
    static const HashMD5 hash = md5("deadbeef");

    ScoreDB score_db { IN_MEMORY_DB_PATH };

    EXPECT_EQ(score_db.getChartScoreBMS(hash), nullptr);
    EXPECT_EQ(score_db.getStats().pgreat, 0);

    {
        ScoreBMS score;
        score.exscore = 16;
        score.lamp = ScoreBMS::Lamp::NOPLAY;
        score.pgreat = 5;
        score.great = 12;
        score.good = 32;
        score.bad = 69;
        score.kpoor = 84;
        score.miss = 58;
        score.bp = 1469;
        score.combobreak = 1385;
        score.replayFileName = "score1";
        score_db.insertChartScoreBMS(hash, score);
        const auto pb = score_db.getChartScoreBMS(hash);
        ASSERT_NE(pb, nullptr);
        EXPECT_EQ(pb->exscore, score.exscore);
        EXPECT_EQ(pb->lamp, score.lamp);
        EXPECT_EQ(pb->pgreat, score.pgreat);
        EXPECT_EQ(pb->great, score.great);
        EXPECT_EQ(pb->good, score.good);
        EXPECT_EQ(pb->bad, score.bad);
        EXPECT_EQ(pb->kpoor, score.kpoor);
        EXPECT_EQ(pb->miss, score.miss);
        EXPECT_EQ(pb->bp, score.bp);
        EXPECT_EQ(pb->combobreak, score.combobreak);
        EXPECT_EQ(pb->replayFileName, score.replayFileName);
        const auto stats = score_db.getStats();
        EXPECT_EQ(stats.pgreat, 0);
        EXPECT_EQ(stats.play_count, 0);
        EXPECT_EQ(stats.clear_count, 0);
    }

    {
        ScoreBMS score;
        score.final_combo = 5;
        score.play_time = lunaticvibes::Time{12340};
        score.exscore = 17;
        score.lamp = ScoreBMS::Lamp::FAILED;
        score.pgreat = 6;
        score.great = 11;
        score.good = 32;
        score.bad = 69;
        score.kpoor = 84;
        score.miss = 58;
        score.bp = 1469;
        score.combobreak = 1385;
        score.replayFileName = "score2";
        score_db.insertChartScoreBMS(hash, score);
        const auto pb = score_db.getChartScoreBMS(hash);
        ASSERT_NE(pb, nullptr);
        EXPECT_EQ(pb->exscore, score.exscore);
        EXPECT_EQ(pb->lamp, score.lamp);
        EXPECT_EQ(pb->pgreat, score.pgreat);
        EXPECT_EQ(pb->great, score.great);
        EXPECT_EQ(pb->good, score.good);
        EXPECT_EQ(pb->bad, score.bad);
        EXPECT_EQ(pb->kpoor, score.kpoor);
        EXPECT_EQ(pb->miss, score.miss);
        EXPECT_EQ(pb->bp, score.bp);
        EXPECT_EQ(pb->combobreak, score.combobreak);
        EXPECT_EQ(pb->replayFileName, score.replayFileName);
        const auto stats = score_db.getStats();
        EXPECT_EQ(stats.pgreat, score.pgreat);
        EXPECT_EQ(stats.great, score.great);
        EXPECT_EQ(stats.good, score.good);
        EXPECT_EQ(stats.bad, score.bad);
        EXPECT_EQ(stats.poor, pb->kpoor + pb->miss);
        EXPECT_EQ(stats.play_count, 1);
        EXPECT_EQ(stats.clear_count, 0);
        EXPECT_EQ(stats.running_combo, 5);
        EXPECT_EQ(stats.max_running_combo, 5);
        EXPECT_EQ(stats.playtime, 12340);
    }

    {
        ScoreBMS score;
        score.exscore = 1;
        score.lamp = ScoreBMS::Lamp::EASY;
        score.pgreat = 2;
        score.replayFileName = "score3";
        score_db.insertChartScoreBMS(hash, score);
        const auto pb = score_db.getChartScoreBMS(hash);
        ASSERT_NE(pb, nullptr);
        EXPECT_EQ(pb->exscore, 17);
        const auto stats = score_db.getStats();
        EXPECT_EQ(stats.pgreat, 6 + 2);
        EXPECT_EQ(stats.play_count, 2);
        EXPECT_EQ(stats.clear_count, 1);
    }

    // Cache reloading works.
    {
        score_db.preloadScore();
        const auto pb = score_db.getChartScoreBMS(hash);
        ASSERT_NE(pb, nullptr);
        EXPECT_EQ(pb->exscore, 17);
    }
}

TEST(ScoreDb, CourseScoreUpdating)
{
    static const HashMD5 hash = md5("deadbeef");

    ScoreDB score_db { IN_MEMORY_DB_PATH };
    ScoreBMS score;
    score.exscore = 1;
    score.lamp = ScoreBMS::Lamp::EASY;
    score.pgreat = 2;

    EXPECT_EQ(score_db.getCourseScoreBMS(hash), nullptr);
    EXPECT_EQ(score_db.getStats().pgreat, 0);

    score_db.updateCourseScoreBMS(hash, score);
    EXPECT_EQ(score_db.getCourseScoreBMS(hash)->exscore, 1);
    EXPECT_EQ(score_db.getStats().pgreat, 0);

    score.exscore = 2;
    score_db.updateCourseScoreBMS(hash, score);
    EXPECT_EQ(score_db.getCourseScoreBMS(hash)->exscore, 2);
    EXPECT_EQ(score_db.getStats().pgreat, 0);

    score.exscore = 1;
    score_db.updateCourseScoreBMS(hash, score);
    EXPECT_EQ(score_db.getCourseScoreBMS(hash)->exscore, 2);
    EXPECT_EQ(score_db.getStats().pgreat, 0);

    // Cache reloading works.
    score_db.preloadScore();
    ASSERT_NE(score_db.getCourseScoreBMS(hash), nullptr);
    EXPECT_EQ(score_db.getCourseScoreBMS(hash)->exscore, 2);
}

TEST(ScoreDb, ChartScoreDeleting)
{
    static const HashMD5 hash = md5("deadbeef");

    ScoreDB score_db { IN_MEMORY_DB_PATH };
    ScoreBMS score;
    score.exscore = 1;

    EXPECT_EQ(score_db.getChartScoreBMS(hash), nullptr);

    // And also test for old DB table. Same scores themselves are never supposed to be in both.
    score_db.updateLegacyChartScoreBMS(hash, score);
    score_db.insertChartScoreBMS(hash, score);
    ASSERT_NE(score_db.getChartScoreBMS(hash), nullptr);
    EXPECT_EQ(score_db.getChartScoreBMS(hash)->exscore, 1);

    score_db.deleteAllChartScoresBMS(hash);
    EXPECT_EQ(score_db.getChartScoreBMS(hash), nullptr);
    // Not just in-memory cache that was invalidated.
    EXPECT_EQ(score_db.fetchCachedPbBMS(hash), nullptr);

    // Not just cache that was invalidated.
    score_db.rebuildBmsPbCache();
    EXPECT_EQ(score_db.getChartScoreBMS(hash), nullptr);
}

TEST(ScoreDb, CourseScoreDeleting)
{
    static const HashMD5 hash = md5("deadbeef");

    ScoreDB score_db { IN_MEMORY_DB_PATH };
    ScoreBMS score;
    score.exscore = 1;

    EXPECT_EQ(score_db.getCourseScoreBMS(hash), nullptr);

    score_db.updateCourseScoreBMS(hash, score);
    EXPECT_NE(score_db.getCourseScoreBMS(hash), nullptr);
    EXPECT_EQ(score_db.getCourseScoreBMS(hash)->exscore, 1);

    score_db.deleteCourseScoreBMS(hash);
    EXPECT_EQ(score_db.getCourseScoreBMS(hash), nullptr);
    // Not just in-memory cache that was invalidated.
    EXPECT_EQ(score_db.fetchCachedPbBMS(hash), nullptr);

    // Not just cache that was invalidated.
    score_db.rebuildBmsPbCache();
    EXPECT_EQ(score_db.getChartScoreBMS(hash), nullptr);
}
