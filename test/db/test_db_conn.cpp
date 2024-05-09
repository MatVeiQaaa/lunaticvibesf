#include <gtest/gtest.h>

#include <db/db_conn.h>

static constexpr auto&& IN_MEMORY_DB_PATH = ":memory:";

TEST(DbConn, MigrationSystemWorks)
{
    struct TestSQLite : public SQLite
    {
        TestSQLite() : SQLite(IN_MEMORY_DB_PATH, "MigrationSystemWorks"){};
        void runTest()
        {
            auto createDb = [this]() { return exec("CREATE TABLE some_data(id INTEGER);", {}) == SQLITE_OK; };
            EXPECT_TRUE(applyMigration("20240509T000000", createDb));
            EXPECT_TRUE(applyMigration("20240509T000000", createDb));
            auto addStuff = [this]() { return exec("INSERT INTO some_data(id) VALUES (1337);", {}) == SQLITE_OK; };
            EXPECT_TRUE(applyMigration("20240509T010000", addStuff));
            EXPECT_TRUE(applyMigration("20240509T010000", addStuff));
            auto badAddStuff = [this]() {
                if (exec("INSERT INTO some_data(id) VALUES (420);", {}) != SQLITE_OK)
                {
                    abort();
                }
                return false;
            };
            EXPECT_FALSE(applyMigration("20240509T020000", badAddStuff));
            EXPECT_FALSE(applyMigration("20240509T020000", badAddStuff));

            auto res = query("SELECT id FROM some_data;");
            ASSERT_EQ(res.size(), 1);
            ASSERT_EQ(res[0].size(), 1);
            EXPECT_EQ(ANY_INT(res[0][0]), 1337);
        }
    };
    TestSQLite{}.runTest();
}
