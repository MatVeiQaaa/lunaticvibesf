#include <gmock/gmock.h>

#include <common/chartformat/chartformat.h>
#include <common/types.h>

TEST(chartformat, FileFormat)
{
    EXPECT_EQ(analyzeChartType(u8"bms/fileテスト.bms"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType(u8"bms/file.bms"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType(u8"bms/file.bme"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType(u8"bms/file.bml"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType(u8"bms/file.pms"_p), eChartFormat::BMS);
    EXPECT_EQ(analyzeChartType(u8"bms/file.bmson"_p), eChartFormat::BMSON);
    EXPECT_EQ(analyzeChartType(u8"bms/readme_utf8.crlf.txt"_p), eChartFormat::UNKNOWN);
    EXPECT_EQ(analyzeChartType(u8"bms/file.テスト"_p), eChartFormat::UNKNOWN);
    EXPECT_EQ(analyzeChartType(u8"bms/.bms"_p), eChartFormat::UNKNOWN);
}
