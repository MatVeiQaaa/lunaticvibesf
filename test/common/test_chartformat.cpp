#include <gmock/gmock.h>

#include <common/chartformat/chartformat.h>
#include <common/types.h>

TEST(chartformat, FileFormat)
{
    EXPECT_EQ(analyzeChartType("bms/fileテスト.bms"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType("bms/file.bms"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType("bms/file.bme"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType("bms/file.bml"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType("bms/file.pms"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType("bms/file.bmson"_p), eChartFormat::BMSON);
    EXPECT_EQ(analyzeChartType("bms/readme_utf8.crlf.txt"_p), eChartFormat::UNKNOWN);
    EXPECT_EQ(analyzeChartType("bms/file.テスト"_p), eChartFormat::UNKNOWN);
    EXPECT_EQ(analyzeChartType("bms/.bms"_p), eChartFormat::UNKNOWN);
}
