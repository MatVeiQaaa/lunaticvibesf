#include <gmock/gmock.h>

#include <memory>

#include "common/chartformat/chartformat_bms.h"
#include "game/replay/replay_chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms_replay.h"

TEST(RulesetBmsReplay, SkipToEndWorks)
{
    auto r = std::make_shared<ReplayChart>();
    r->loadXml(
        R"(<?xml version="1.0" encoding="utf-8"?><cereal><value0><cereal_class_version>2</cereal_class_version><value0>101516808</value0><value1><value0>179</value0><value1>190</value1><value2>233</value2><value3>15</value3><value4>211</value4><value5>94</value5><value6>97</value6><value7>64</value7><value8>244</value8><value9>193</value9><value10>6</value10><value11>193</value11><value12>133</value12><value13>99</value13><value14>171</value14><value15>163</value15></value1><value2>2238515068681995405</value2><value3>3</value3><value4>0</value4><value5>0</value5><value6>2</value6><value7>0</value7><value8>0</value8><value9>0</value9><value10>2</value10><value11>false</value11><value12>false</value12><value13>3.2000000000000002</value13><value14>10</value14><value15>0</value15><value16>true</value16><value17 size="dynamic"><value0><value0>322</value0><value1>8</value1><value2>0</value2></value0><value1><value0>648</value0><value1>34</value1><value2>0</value2></value1><value2><value0>982</value0><value1>8</value1><value2>0</value2></value2><value3><value0>1141</value0><value1>34</value1><value2>0</value2></value3><value4><value0>1630</value0><value1>74</value1><value2>0</value2></value4><value5><value0>1632</value0><value1>6</value1><value2>0</value2></value5><value6><value0>1807</value0><value1>32</value1><value2>0</value2></value6><value7><value0>1990</value0><value1>63</value1><value2>0</value2></value7><value8><value0>1992</value0><value1>7</value1><value2>0</value2></value8><value9><value0>2166</value0><value1>33</value1><value2>0</value2></value9><value10><value0>2390</value0><value1>63</value1><value2>0</value2></value10><value11><value0>2392</value0><value1>8</value1><value2>0</value2></value11><value12><value0>2573</value0><value1>34</value1><value2>0</value2></value12><value13><value0>3192</value0><value1>6</value1><value2>0</value2></value13><value14><value0>3200</value0><value1>73</value1><value2>0</value2></value14><value15><value0>3202</value0><value1>10</value1><value2>0</value2></value15><value16><value0>3377</value0><value1>36</value1><value2>0</value2></value16><value17><value0>3400</value0><value1>77</value1><value2>0</value2></value17><value18><value0>3443</value0><value1>32</value1><value2>0</value2></value18><value19><value0>3759</value0><value1>76</value1><value2>0</value2></value19><value20><value0>3759</value0><value1>68</value1><value2>0</value2></value20><value21><value0>3761</value0><value1>4</value1><value2>0</value2></value21><value22><value0>4002</value0><value1>30</value1><value2>0</value2></value22><value23><value0>4119</value0><value1>76</value1><value2>0</value2></value23><value24><value0>4119</value0><value1>68</value1><value2>0</value2></value24><value25><value0>4121</value0><value1>4</value1><value2>0</value2></value25><value26><value0>4312</value0><value1>30</value1><value2>0</value2></value26><value27><value0>4420</value0><value1>74</value1><value2>0</value2></value27><value28><value0>4422</value0><value1>4</value1><value2>0</value2></value28><value29><value0>4593</value0><value1>30</value1><value2>0</value2></value29><value30><value0>6631</value0><value1>15</value1><value2>0</value2></value30><value31><value0>6857</value0><value1>41</value1><value2>0</value2></value31></value17></value0></cereal>)");
    auto bms = std::make_shared<ChartFormatBMS>("bms/5k.bms", r->randomSeed);
    ASSERT_EQ(bms->fileHash, HashMD5{"b3bee90fd35e6140f4c106c18563aba3"});
    auto obj = ChartObjectBase::createFromChartFormat(PLAYER_SLOT_TARGET, bms);
    RulesetBMSReplay rr(bms, obj, r, r->getMods(), bms->gamemode, bms->rank.value_or(RulesetBMS::LR2_DEFAULT_RANK), 20.,
                        RulesetBMS::PlaySide::AUTO, 0, 1.0);
    rr.update(LLONG_MAX);
    EXPECT_EQ(rr.getJudgeCount(RulesetBMS::JudgeType::PERFECT), 3);
    EXPECT_EQ(rr.getJudgeCount(RulesetBMS::JudgeType::GREAT), 2);
    EXPECT_EQ(rr.getJudgeCount(RulesetBMS::JudgeType::GOOD), 0);
    EXPECT_EQ(rr.getJudgeCount(RulesetBMS::JudgeType::BAD), 2);
    EXPECT_EQ(rr.getJudgeCount(RulesetBMS::JudgeType::KPOOR), 2);
    EXPECT_EQ(rr.getJudgeCount(RulesetBMS::JudgeType::MISS), 1);
    EXPECT_EQ(rr.getJudgeCountEx(RulesetBMS::JUDGE_EARLY), 2);
    EXPECT_EQ(rr.getJudgeCountEx(RulesetBMS::JUDGE_LATE), 5);
    const RulesetBase::BasicData data = rr.getData();
    EXPECT_EQ(data.play_time, (lunaticvibes::Time{6857, false})); // Scuffed number. The chart itself is a bit shorter.
    EXPECT_DOUBLE_EQ(data.health, 1.0);
    // FIXME: why not 50%? I would expect this to be the same as total_acc by the chart's end.
    EXPECT_DOUBLE_EQ(data.acc, 57.142857142857146);
    EXPECT_DOUBLE_EQ(data.total_acc, 50.);
    EXPECT_EQ(data.combo, 1);
    EXPECT_EQ(data.maxCombo, 4);
    EXPECT_EQ(data.firstMaxCombo, 4);
    EXPECT_EQ(data.comboDisplay, 0);
    EXPECT_EQ(data.maxComboDisplay, 4);
}
