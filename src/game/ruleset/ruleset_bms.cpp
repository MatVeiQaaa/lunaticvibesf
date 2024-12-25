#include "ruleset_bms.h"

#include <iterator>
#include <utility>

#include "game/arena/arena_data.h"
#include "game/chart/chart_bms.h"
#include "game/runtime/state.h"
#include "game/scene/scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include <common/assert.h>
#include <common/sysutil.h>
#include <config/config_mgr.h>

using namespace chart;

void setJudgeInternalTimer1P(RulesetBMS::JudgeType judge, long long t)
{
    State::set(IndexTimer::_JUDGE_1P_0, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_1P_1, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_1P_2, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_1P_3, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_1P_4, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_1P_5, TIMER_NEVER);
    switch (judge)
    {
    case RulesetBMS::JudgeType::KPOOR: State::set(IndexTimer::_JUDGE_1P_0, t); break;
    case RulesetBMS::JudgeType::MISS: State::set(IndexTimer::_JUDGE_1P_1, t); break;
    case RulesetBMS::JudgeType::BAD: State::set(IndexTimer::_JUDGE_1P_2, t); break;
    case RulesetBMS::JudgeType::GOOD: State::set(IndexTimer::_JUDGE_1P_3, t); break;
    case RulesetBMS::JudgeType::GREAT: State::set(IndexTimer::_JUDGE_1P_4, t); break;
    case RulesetBMS::JudgeType::PERFECT: State::set(IndexTimer::_JUDGE_1P_5, t); break;
    default: break;
    }
}

void setJudgeInternalTimer2P(RulesetBMS::JudgeType judge, long long t)
{
    State::set(IndexTimer::_JUDGE_2P_0, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_2P_1, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_2P_2, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_2P_3, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_2P_4, TIMER_NEVER);
    State::set(IndexTimer::_JUDGE_2P_5, TIMER_NEVER);
    switch (judge)
    {
    case RulesetBMS::JudgeType::KPOOR: State::set(IndexTimer::_JUDGE_2P_0, t); break;
    case RulesetBMS::JudgeType::MISS: State::set(IndexTimer::_JUDGE_2P_1, t); break;
    case RulesetBMS::JudgeType::BAD: State::set(IndexTimer::_JUDGE_2P_2, t); break;
    case RulesetBMS::JudgeType::GOOD: State::set(IndexTimer::_JUDGE_2P_3, t); break;
    case RulesetBMS::JudgeType::GREAT: State::set(IndexTimer::_JUDGE_2P_4, t); break;
    case RulesetBMS::JudgeType::PERFECT: State::set(IndexTimer::_JUDGE_2P_5, t); break;
    default: break;
    }
}

RulesetBMS::RulesetBMS(std::shared_ptr<ChartFormatBase> format, std::shared_ptr<ChartObjectBase> chart,
                       PlayModifiers mods, GameModeKeys keys, JudgeDifficulty difficulty, double health,
                       RulesetBMS::PlaySide side, const int fiveKeyMapIndex,
                       std::shared_ptr<PlayContextParams::MutexReplayChart> replayNew)
    : RulesetBase(std::move(format), std::move(chart)), _judgeDifficulty(difficulty), _replayNew(std::move(replayNew))
{
    if (_replayNew)
    {
        LVF_DEBUG_ASSERT(_replayNew->replay != nullptr);
    }

    static const NoteLaneTimerMap bombTimer5k[] = {
        {{
            {chart::Sc1, IndexTimer::S1_BOMB},
            {chart::N11, IndexTimer::K11_BOMB},
            {chart::N12, IndexTimer::K12_BOMB},
            {chart::N13, IndexTimer::K13_BOMB},
            {chart::N14, IndexTimer::K14_BOMB},
            {chart::N15, IndexTimer::K15_BOMB},
            {chart::N21, IndexTimer::K21_BOMB},
            {chart::N22, IndexTimer::K22_BOMB},
            {chart::N23, IndexTimer::K23_BOMB},
            {chart::N24, IndexTimer::K24_BOMB},
            {chart::N25, IndexTimer::K25_BOMB},
            {chart::Sc2, IndexTimer::S2_BOMB},
        }},
        {{
            {chart::Sc1, IndexTimer::S1_BOMB},
            {chart::N11, IndexTimer::K11_BOMB},
            {chart::N12, IndexTimer::K12_BOMB},
            {chart::N13, IndexTimer::K13_BOMB},
            {chart::N14, IndexTimer::K14_BOMB},
            {chart::N15, IndexTimer::K15_BOMB},
            {chart::N21, IndexTimer::K23_BOMB},
            {chart::N22, IndexTimer::K24_BOMB},
            {chart::N23, IndexTimer::K25_BOMB},
            {chart::N24, IndexTimer::K26_BOMB},
            {chart::N25, IndexTimer::K27_BOMB},
            {chart::Sc2, IndexTimer::S2_BOMB},
        }},
        {{
            {chart::Sc1, IndexTimer::S1_BOMB},
            {chart::N11, IndexTimer::K13_BOMB},
            {chart::N12, IndexTimer::K14_BOMB},
            {chart::N13, IndexTimer::K15_BOMB},
            {chart::N14, IndexTimer::K16_BOMB},
            {chart::N15, IndexTimer::K17_BOMB},
            {chart::N21, IndexTimer::K21_BOMB},
            {chart::N22, IndexTimer::K22_BOMB},
            {chart::N23, IndexTimer::K23_BOMB},
            {chart::N24, IndexTimer::K24_BOMB},
            {chart::N25, IndexTimer::K25_BOMB},
            {chart::Sc2, IndexTimer::S2_BOMB},
        }},
        {{
            {chart::Sc1, IndexTimer::S1_BOMB},
            {chart::N11, IndexTimer::K13_BOMB},
            {chart::N12, IndexTimer::K14_BOMB},
            {chart::N13, IndexTimer::K15_BOMB},
            {chart::N14, IndexTimer::K16_BOMB},
            {chart::N15, IndexTimer::K17_BOMB},
            {chart::N21, IndexTimer::K23_BOMB},
            {chart::N22, IndexTimer::K24_BOMB},
            {chart::N23, IndexTimer::K25_BOMB},
            {chart::N24, IndexTimer::K26_BOMB},
            {chart::N25, IndexTimer::K27_BOMB},
            {chart::Sc2, IndexTimer::S2_BOMB},
        }},
    };

    static const NoteLaneTimerMap bombTimer5kLN[] = {
        {{
            {chart::Sc1, IndexTimer::S1_LN_BOMB},
            {chart::N11, IndexTimer::K11_LN_BOMB},
            {chart::N12, IndexTimer::K12_LN_BOMB},
            {chart::N13, IndexTimer::K13_LN_BOMB},
            {chart::N14, IndexTimer::K14_LN_BOMB},
            {chart::N15, IndexTimer::K15_LN_BOMB},
            {chart::N21, IndexTimer::K21_LN_BOMB},
            {chart::N22, IndexTimer::K22_LN_BOMB},
            {chart::N23, IndexTimer::K23_LN_BOMB},
            {chart::N24, IndexTimer::K24_LN_BOMB},
            {chart::N25, IndexTimer::K25_LN_BOMB},
            {chart::Sc2, IndexTimer::S2_LN_BOMB},
        }},
        {{
            {chart::Sc1, IndexTimer::S1_LN_BOMB},
            {chart::N11, IndexTimer::K11_LN_BOMB},
            {chart::N12, IndexTimer::K12_LN_BOMB},
            {chart::N13, IndexTimer::K13_LN_BOMB},
            {chart::N14, IndexTimer::K14_LN_BOMB},
            {chart::N15, IndexTimer::K15_LN_BOMB},
            {chart::N21, IndexTimer::K23_LN_BOMB},
            {chart::N22, IndexTimer::K24_LN_BOMB},
            {chart::N23, IndexTimer::K25_LN_BOMB},
            {chart::N24, IndexTimer::K26_LN_BOMB},
            {chart::N25, IndexTimer::K27_LN_BOMB},
            {chart::Sc2, IndexTimer::S2_LN_BOMB},
        }},
        {{
            {chart::Sc1, IndexTimer::S1_LN_BOMB},
            {chart::N11, IndexTimer::K13_LN_BOMB},
            {chart::N12, IndexTimer::K14_LN_BOMB},
            {chart::N13, IndexTimer::K15_LN_BOMB},
            {chart::N14, IndexTimer::K16_LN_BOMB},
            {chart::N15, IndexTimer::K17_LN_BOMB},
            {chart::N21, IndexTimer::K21_LN_BOMB},
            {chart::N22, IndexTimer::K22_LN_BOMB},
            {chart::N23, IndexTimer::K23_LN_BOMB},
            {chart::N24, IndexTimer::K24_LN_BOMB},
            {chart::N25, IndexTimer::K25_LN_BOMB},
            {chart::Sc2, IndexTimer::S2_LN_BOMB},
        }},
        {{
            {chart::Sc1, IndexTimer::S1_LN_BOMB},
            {chart::N11, IndexTimer::K13_LN_BOMB},
            {chart::N12, IndexTimer::K14_LN_BOMB},
            {chart::N13, IndexTimer::K15_LN_BOMB},
            {chart::N14, IndexTimer::K16_LN_BOMB},
            {chart::N15, IndexTimer::K17_LN_BOMB},
            {chart::N21, IndexTimer::K23_LN_BOMB},
            {chart::N22, IndexTimer::K24_LN_BOMB},
            {chart::N23, IndexTimer::K25_LN_BOMB},
            {chart::N24, IndexTimer::K26_LN_BOMB},
            {chart::N25, IndexTimer::K27_LN_BOMB},
            {chart::Sc2, IndexTimer::S2_LN_BOMB},
        }},
    };

    static const NoteLaneTimerMap bombTimer7k = {{
        {chart::Sc1, IndexTimer::S1_BOMB},
        {chart::N11, IndexTimer::K11_BOMB},
        {chart::N12, IndexTimer::K12_BOMB},
        {chart::N13, IndexTimer::K13_BOMB},
        {chart::N14, IndexTimer::K14_BOMB},
        {chart::N15, IndexTimer::K15_BOMB},
        {chart::N16, IndexTimer::K16_BOMB},
        {chart::N17, IndexTimer::K17_BOMB},
        {chart::N21, IndexTimer::K21_BOMB},
        {chart::N22, IndexTimer::K22_BOMB},
        {chart::N23, IndexTimer::K23_BOMB},
        {chart::N24, IndexTimer::K24_BOMB},
        {chart::N25, IndexTimer::K25_BOMB},
        {chart::N26, IndexTimer::K26_BOMB},
        {chart::N27, IndexTimer::K27_BOMB},
        {chart::Sc2, IndexTimer::S2_BOMB},
    }};

    static const NoteLaneTimerMap bombTimer7kLN = {{
        {chart::Sc1, IndexTimer::S1_LN_BOMB},
        {chart::N11, IndexTimer::K11_LN_BOMB},
        {chart::N12, IndexTimer::K12_LN_BOMB},
        {chart::N13, IndexTimer::K13_LN_BOMB},
        {chart::N14, IndexTimer::K14_LN_BOMB},
        {chart::N15, IndexTimer::K15_LN_BOMB},
        {chart::N16, IndexTimer::K16_LN_BOMB},
        {chart::N17, IndexTimer::K17_LN_BOMB},
        {chart::N21, IndexTimer::K21_LN_BOMB},
        {chart::N22, IndexTimer::K22_LN_BOMB},
        {chart::N23, IndexTimer::K23_LN_BOMB},
        {chart::N24, IndexTimer::K24_LN_BOMB},
        {chart::N25, IndexTimer::K25_LN_BOMB},
        {chart::N26, IndexTimer::K26_LN_BOMB},
        {chart::N27, IndexTimer::K27_LN_BOMB},
        {chart::Sc2, IndexTimer::S2_LN_BOMB},
    }};

    static const NoteLaneTimerMap bombTimer9k = {{
        {chart::N11, IndexTimer::K11_BOMB},
        {chart::N12, IndexTimer::K12_BOMB},
        {chart::N13, IndexTimer::K13_BOMB},
        {chart::N14, IndexTimer::K14_BOMB},
        {chart::N15, IndexTimer::K15_BOMB},
        {chart::N16, IndexTimer::K16_BOMB},
        {chart::N17, IndexTimer::K17_BOMB},
        {chart::N18, IndexTimer::K18_BOMB},
        {chart::N19, IndexTimer::K19_BOMB},
    }};

    static const NoteLaneTimerMap bombTimer9kLN = {{
        {chart::N11, IndexTimer::K11_LN_BOMB},
        {chart::N12, IndexTimer::K12_LN_BOMB},
        {chart::N13, IndexTimer::K13_LN_BOMB},
        {chart::N14, IndexTimer::K14_LN_BOMB},
        {chart::N15, IndexTimer::K15_LN_BOMB},
        {chart::N16, IndexTimer::K16_LN_BOMB},
        {chart::N17, IndexTimer::K17_LN_BOMB},
        {chart::N18, IndexTimer::K18_LN_BOMB},
        {chart::N19, IndexTimer::K19_LN_BOMB},
    }};

    switch (keys)
    {
    case 5:
    case 10: {
        LVF_DEBUG_ASSERT(fiveKeyMapIndex >= 0);
        LVF_DEBUG_ASSERT(fiveKeyMapIndex < static_cast<int>(std::size(bombTimer5k)));
        LVF_DEBUG_ASSERT(fiveKeyMapIndex < static_cast<int>(std::size(bombTimer5kLN)));
        _bombTimerMap = &bombTimer5k[fiveKeyMapIndex];
        _bombLNTimerMap = &bombTimer5kLN[fiveKeyMapIndex];
        break;
    }
    case 7:
    case 14:
        _bombTimerMap = &bombTimer7k;
        _bombLNTimerMap = &bombTimer7kLN;
        break;
    case 9:
        _bombTimerMap = &bombTimer9k;
        _bombLNTimerMap = &bombTimer9kLN;
        break;
    default: break;
    }

    switch (keys)
    {
    case 7:
    case 14: maxMoneyScore = 200000.0; break;
    case 5:
    case 10:
    case 9: maxMoneyScore = 100000.0; break;
    default: break;
    }

    if (_chart)
    {
        noteCount = _chart->getNoteRegularCount() + _chart->getNoteLnCount();
    }

    using namespace std::string_literals;

    _basic.health = health;
    initGaugeParams(mods.gauge);

    _side = side;
    _judgeScratch = !(mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
    switch (side)
    {
    case RulesetBMS::PlaySide::SINGLE:
        _k1P = true;
        _k2P = false;
        break;
    case RulesetBMS::PlaySide::DOUBLE:
        _k1P = true;
        _k2P = true;
        break;
    case RulesetBMS::PlaySide::BATTLE_1P:
        _k1P = true;
        _k2P = false;
        break;
    case RulesetBMS::PlaySide::BATTLE_2P:
        _k1P = false;
        _k2P = true;
        break;
    default:
        _k1P = true;
        _k2P = true;
        break;
    }

    std::stringstream ssMod;
    std::stringstream ssModShort;
    switch (side)
    {
    case RulesetBMS::PlaySide::SINGLE:
    case RulesetBMS::PlaySide::BATTLE_1P:
    case RulesetBMS::PlaySide::AUTO:
        ssMod << (State::get(IndexOption::PLAY_RANDOM_TYPE_1P) == Option::RAN_NORMAL
                      ? ""
                      : Option::s_random_type[State::get(IndexOption::PLAY_RANDOM_TYPE_1P)]);
        ssModShort << (State::get(IndexOption::PLAY_RANDOM_TYPE_1P) == Option::RAN_NORMAL
                           ? ""
                           : Option::s_random_type_short[State::get(IndexOption::PLAY_RANDOM_TYPE_1P)]);
        if (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P))
        {
            if (!ssMod.str().empty())
                ssMod << ", ";
            if (!ssModShort.str().empty())
                ssModShort << ",";
            ssMod << "" << Option::s_assist_type[Option::ASSIST_AUTOSCR];
            ssModShort << "" << Option::s_assist_type_short[Option::ASSIST_AUTOSCR];
        }
        break;

    case RulesetBMS::PlaySide::RIVAL:
    case RulesetBMS::PlaySide::BATTLE_2P:
    case RulesetBMS::PlaySide::AUTO_2P:
        ssMod << (State::get(IndexOption::PLAY_RANDOM_TYPE_2P) == Option::RAN_NORMAL
                      ? ""
                      : Option::s_random_type[State::get(IndexOption::PLAY_RANDOM_TYPE_2P)]);
        ssModShort << (State::get(IndexOption::PLAY_RANDOM_TYPE_2P) == Option::RAN_NORMAL
                           ? ""
                           : Option::s_random_type_short[State::get(IndexOption::PLAY_RANDOM_TYPE_2P)]);
        if (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_2P))
        {
            if (!ssMod.str().empty())
                ssMod << ", ";
            if (!ssModShort.str().empty())
                ssModShort << ",";
            ssMod << "" << Option::s_assist_type[Option::ASSIST_AUTOSCR];
            ssModShort << "" << Option::s_assist_type_short[Option::ASSIST_AUTOSCR];
        }
        break;

    case RulesetBMS::PlaySide::DOUBLE:
    case RulesetBMS::PlaySide::AUTO_DOUBLE:
        if (!(State::get(IndexOption::PLAY_RANDOM_TYPE_1P) == Option::RAN_NORMAL &&
              State::get(IndexOption::PLAY_RANDOM_TYPE_2P) == Option::RAN_NORMAL))
        {
            ssMod << (State::get(IndexOption::PLAY_RANDOM_TYPE_1P) == Option::RAN_NORMAL
                          ? "-"
                          : Option::s_random_type_short[State::get(IndexOption::PLAY_RANDOM_TYPE_1P)])
                  << "/"
                  << (State::get(IndexOption::PLAY_RANDOM_TYPE_2P) == Option::RAN_NORMAL
                          ? "-"
                          : Option::s_random_type_short[State::get(IndexOption::PLAY_RANDOM_TYPE_2P)]);
            ssModShort << (State::get(IndexOption::PLAY_RANDOM_TYPE_1P) == Option::RAN_NORMAL
                               ? "-"
                               : Option::s_random_type_short[State::get(IndexOption::PLAY_RANDOM_TYPE_1P)])
                       << "/"
                       << (State::get(IndexOption::PLAY_RANDOM_TYPE_2P) == Option::RAN_NORMAL
                               ? "-"
                               : Option::s_random_type_short[State::get(IndexOption::PLAY_RANDOM_TYPE_2P)]);
        }
        if (State::get(IndexSwitch::PLAY_OPTION_DP_FLIP))
        {
            if (!ssMod.str().empty())
                ssMod << ", ";
            if (!ssModShort.str().empty())
                ssModShort << ",";
            ssMod << "FLIP";
            ssModShort << "F";
        }
        if (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P))
        {
            if (!ssMod.str().empty())
                ssMod << ", ";
            if (!ssModShort.str().empty())
                ssModShort << ",";
            ssMod << "" << Option::s_assist_type[Option::ASSIST_AUTOSCR];
            ssModShort << "" << Option::s_assist_type_short[Option::ASSIST_AUTOSCR];
        }
        break;

    case RulesetBMS::PlaySide::MYBEST:
    case RulesetBMS::PlaySide::NETWORK: break;
    }
    modifierText = ssMod.str();
    if (modifierText.empty())
    {
        modifierText = "NONE";
    }
    modifierTextShort = ssMod.str();

    saveLampMax = getSaveScoreType(false).second;

    _lnJudge.fill(JudgeArea::NOTHING);

    if (_chart)
    {
        for (size_t k = Input::S1L; k <= Input::K2SPDDN; ++k)
        {
            NoteLaneIndex idx;
            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
                _noteListIterators[{NoteLaneCategory::Note, idx}] = _chart->firstNote(NoteLaneCategory::Note, idx);
            idx = _chart->getLaneFromKey(NoteLaneCategory::LN, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
                _noteListIterators[{NoteLaneCategory::LN, idx}] = _chart->firstNote(NoteLaneCategory::LN, idx);
            idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
                _noteListIterators[{NoteLaneCategory::Mine, idx}] = _chart->firstNote(NoteLaneCategory::Mine, idx);
            idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
                _noteListIterators[{NoteLaneCategory::Invs, idx}] = _chart->firstNote(NoteLaneCategory::Invs, idx);
        }
    }
}

static RulesetBMS::GaugeType get_gauge(PlayModifierGaugeType gauge)
{
    using P = PlayModifierGaugeType;
    using G = RulesetBMS::GaugeType;
    switch (gauge)
    {
    case P::NORMAL: return G::GROOVE;
    case P::HARD: return G::HARD;
    case P::DEATH: return G::DEATH;
    case P::EASY: return G::EASY;
    // TODO: check these.
    case P::PATTACK:
    case P::GATTACK: return G::GROOVE;
    // case P::PATTACK: return G::P_ATK;
    // case P::GATTACK: return G::G_ATK;
    case P::EXHARD: return G::EXHARD;
    case P::ASSISTEASY: return G::ASSIST;
    case P::GRADE_NORMAL: return G::GRADE;
    case P::GRADE_DEATH:
    case P::GRADE_HARD: return G::EXGRADE;
    }
    lunaticvibes::assert_failed("invalid PlayModifierGaugeType");
}

void RulesetBMS::initGaugeParams(PlayModifierGaugeType gauge)
{
    // Reference: https://github.com/aeventyr/LR2GAS_pub
    // Note that regarding gauge lr2oraja is wrong.
    // TODO: also check by disassembled LR2.

    _gauge = get_gauge(gauge);

    if (_format)
    {
        switch (_format->type())
        {
        case eChartFormat::BMS: {
            auto bms = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(_format);
            total = bms->total;
            break;
        }
        case eChartFormat::UNKNOWN:
        case eChartFormat::BMSON: break;
        }
    }
    // NOTE: LR2 handles #TOTAL 0 as if total was not set.
    if (total <= 0)
    {
        switch (_gauge)
        {
        case RulesetBMS::GaugeType::HARD:
        case RulesetBMS::GaugeType::EXHARD:
        case RulesetBMS::GaugeType::DEATH:
        case RulesetBMS::GaugeType::GRADE:
        case RulesetBMS::GaugeType::EXGRADE: total = 300; break;
        case RulesetBMS::GaugeType::GROOVE:
        case RulesetBMS::GaugeType::EASY:
        case RulesetBMS::GaugeType::ASSIST:
        default: total = 160; break;
        }
    }

    switch (_gauge)
    {
    case GaugeType::HARD:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 0.001;
        _healthGain[JudgeType::GREAT] = 0.001;
        _healthGain[JudgeType::GOOD] = 0.001 / 2;
        _healthGain[JudgeType::BAD] = -0.06;
        _healthGain[JudgeType::MISS] = -0.1;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::EXHARD:
        //_basic.health             = 1.0;
        // TODO: match lr2oraja?
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD] = -0.12;
        _healthGain[JudgeType::MISS] = -0.2;
        _healthGain[JudgeType::KPOOR] = -0.1;
        break;

    case GaugeType::DEATH:
        //_basic.health               = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::GOOD] = 0.0;
        _healthGain[JudgeType::BAD] = -1.0;
        _healthGain[JudgeType::MISS] = -1.0;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::P_ATK:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 0.001;
        _healthGain[JudgeType::GREAT] = -0.01;
        _healthGain[JudgeType::GOOD] = -1.0;
        _healthGain[JudgeType::BAD] = -1.0;
        _healthGain[JudgeType::MISS] = -1.0;
        _healthGain[JudgeType::KPOOR] = -1.0;
        break;

    case GaugeType::G_ATK:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = -0.1;
        _healthGain[JudgeType::GREAT] = -0.01;
        _healthGain[JudgeType::GOOD] = 0.001;
        _healthGain[JudgeType::BAD] = -0.06;
        _healthGain[JudgeType::MISS] = -0.1;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::GROOVE:
        //_basic.health             = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.8;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / noteCount;
        _healthGain[JudgeType::GREAT] = 0.01 * total / noteCount;
        _healthGain[JudgeType::GOOD] = 0.01 * total / noteCount / 2;
        _healthGain[JudgeType::BAD] = -0.04;
        _healthGain[JudgeType::MISS] = -0.06;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::EASY:
        //_basic.health             = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.8;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GREAT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GOOD] = 0.01 * total / noteCount / 2 * 1.2;
        _healthGain[JudgeType::BAD] = -0.032;
        _healthGain[JudgeType::MISS] = -0.048;
        _healthGain[JudgeType::KPOOR] = -0.016;
        break;

    case GaugeType::ASSIST:
        //_basic.health             = 0.2;
        // TODO: match lr2oraja?
        _minHealth = 0.02;
        _clearHealth = 0.6;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GREAT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GOOD] = 0.01 * total / noteCount / 2 * 1.2;
        _healthGain[JudgeType::BAD] = -0.032;
        _healthGain[JudgeType::MISS] = -0.048;
        _healthGain[JudgeType::KPOOR] = -0.016;
        break;

    case GaugeType::GRADE:
        //_basic.health             = 1.0;
        // TODO: check numbers.
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD] = -0.02;
        _healthGain[JudgeType::MISS] = -0.03;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::EXGRADE:
        //_basic.health             = 1.0;
        // TODO: check numbers.
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD] = -0.12;
        _healthGain[JudgeType::MISS] = -0.1;
        _healthGain[JudgeType::KPOOR] = -0.1;
        break;

    default: break;
    }
}

RulesetBMS::JudgeRes RulesetBMS::_calcJudgeByTimes(const Note& note, const lunaticvibes::Time& time) const
{
    // spot judge area
    JudgeArea a = JudgeArea::NOTHING;
    lunaticvibes::Time error = time - note.time;
    if (error > -judgeTime[(size_t)_judgeDifficulty].KPOOR)
    {
        if (error < -judgeTime[(size_t)_judgeDifficulty].BAD)
            a = JudgeArea::EARLY_KPOOR;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].GOOD)
            a = JudgeArea::EARLY_BAD;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].GREAT)
            a = JudgeArea::EARLY_GOOD;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].PERFECT)
            a = JudgeArea::EARLY_GREAT;
        else if (error < 0)
            a = JudgeArea::EARLY_PERFECT;
        else if (error == 0)
            a = JudgeArea::EXACT_PERFECT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].PERFECT)
            a = JudgeArea::LATE_PERFECT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].GREAT)
            a = JudgeArea::LATE_GREAT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].GOOD)
            a = JudgeArea::LATE_GOOD;
        else if (error < judgeTime[(size_t)_judgeDifficulty].BAD)
            a = JudgeArea::LATE_BAD;
    }

    // log
    /*
    switch (a)
    {
    case JudgeArea::EARLY_KPOOR:   LOG_DEBUG << "EARLY  KPOOR   " << error; break;
    case JudgeArea::EARLY_BAD:     LOG_DEBUG << "EARLY  BAD     " << error; break;
    case JudgeArea::EARLY_GOOD:    LOG_DEBUG << "EARLY  GOOD    " << error; break;
    case JudgeArea::EARLY_GREAT:   LOG_DEBUG << "EARLY  GREAT   " << error; break;
    case JudgeArea::EARLY_PERFECT: LOG_DEBUG << "EARLY  PERFECT " << error; break;
    case JudgeArea::LATE_PERFECT:  LOG_DEBUG << "LATE   PERFECT " << error; break;
    case JudgeArea::LATE_GREAT:    LOG_DEBUG << "LATE   GREAT   " << error; break;
    case JudgeArea::LATE_GOOD:     LOG_DEBUG << "LATE   GOOD    " << error; break;
    case JudgeArea::LATE_BAD:      LOG_DEBUG << "LATE   BAD     " << error; break;
    case JudgeArea::NOTHING:
    case JudgeArea::EXACT_PERFECT:
    case JudgeArea::MISS:
    case JudgeArea::LATE_KPOOR:
    case JudgeArea::MINE_KPOOR: break;
    }
    */

    return {a, error};
}

// TODO: nuke this and JUDGE_* replay commands.
static const std::map<RulesetBMS::JudgeArea, ReplayChart::Commands::Type> judgeAreaReplayCommandType[] = {
    {
        {RulesetBMS::JudgeArea::EXACT_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_EXACT_0},
        {RulesetBMS::JudgeArea::EARLY_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_0},
        {RulesetBMS::JudgeArea::EARLY_GREAT, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_1},
        {RulesetBMS::JudgeArea::EARLY_GOOD, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_2},
        {RulesetBMS::JudgeArea::EARLY_BAD, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_3},
        {RulesetBMS::JudgeArea::EARLY_KPOOR, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_5},
        {RulesetBMS::JudgeArea::LATE_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_0},
        {RulesetBMS::JudgeArea::LATE_GREAT, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_1},
        {RulesetBMS::JudgeArea::LATE_GOOD, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_2},
        {RulesetBMS::JudgeArea::LATE_BAD, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3},
        {RulesetBMS::JudgeArea::MISS, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4},
    },
    {
        {RulesetBMS::JudgeArea::EXACT_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_EXACT_0},
        {RulesetBMS::JudgeArea::EARLY_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_0},
        {RulesetBMS::JudgeArea::EARLY_GREAT, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_1},
        {RulesetBMS::JudgeArea::EARLY_GOOD, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_2},
        {RulesetBMS::JudgeArea::EARLY_BAD, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_3},
        {RulesetBMS::JudgeArea::EARLY_KPOOR, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_5},
        {RulesetBMS::JudgeArea::LATE_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_0},
        {RulesetBMS::JudgeArea::LATE_GREAT, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_1},
        {RulesetBMS::JudgeArea::LATE_GOOD, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_2},
        {RulesetBMS::JudgeArea::LATE_BAD, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3},
        {RulesetBMS::JudgeArea::MISS, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4},
    }};

void RulesetBMS::_judgePress(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, const JudgeRes& judge,
                             const lunaticvibes::Time& t, int slot)
{
    if (cat == NoteLaneCategory::LN && (note.flags & Note::LN_TAIL) &&
        (idx == NoteLaneIndex::Sc1 || idx == NoteLaneIndex::Sc2) && _lnJudge[idx] != JudgeArea::NOTHING)
    {
        // Handle scratch direction change as miss
        _judgeRelease(cat, idx, note, judge, t, slot);
        if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
            State::set(_bombLNTimerMap->at(idx), TIMER_NEVER);
    }

    bool pushReplayCommand = false;
    switch (cat)
    {
    case NoteLaneCategory::Note:
        switch (judge.area)
        {
        case JudgeArea::EARLY_PERFECT:
        case JudgeArea::EXACT_PERFECT:
        case JudgeArea::LATE_PERFECT:
        case JudgeArea::EARLY_GREAT:
        case JudgeArea::LATE_GREAT:
        case JudgeArea::EARLY_GOOD:
        case JudgeArea::LATE_GOOD:
            pushReplayCommand = true;
            note.hit = true;
            note.expired = true;
            notesExpired++;
            updateJudge(t, idx, judge.area, slot);
            break;

        case JudgeArea::EARLY_BAD:
        case JudgeArea::LATE_BAD:
            pushReplayCommand = true;
            note.expired = true;
            notesExpired++;
            updateJudge(t, idx, judge.area, slot);
            break;

        case JudgeArea::EARLY_KPOOR:
            pushReplayCommand = true;
            updateJudge(t, idx, judge.area, slot);
            break;

        case JudgeArea::NOTHING:
        case JudgeArea::MISS:
        case JudgeArea::LATE_KPOOR:
        case JudgeArea::MINE_KPOOR: break;
        }
        break;

    case NoteLaneCategory::Invs: break;

    case NoteLaneCategory::LN:
        if (!(note.flags & Note::LN_TAIL))
        {
            switch (judge.area)
            {
            case JudgeArea::EARLY_PERFECT:
            case JudgeArea::EXACT_PERFECT:
            case JudgeArea::LATE_PERFECT:
            case JudgeArea::EARLY_GREAT:
            case JudgeArea::LATE_GREAT:
            case JudgeArea::EARLY_GOOD:
            case JudgeArea::LATE_GOOD:
                _lnJudge[idx] = judge.area;
                note.hit = true;
                note.expired = true;
                if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    State::set(_bombLNTimerMap->at(idx), t.norm());
                break;

            case JudgeArea::EARLY_BAD:
            case JudgeArea::LATE_BAD:
                note.expired = true;
                notesExpired++;
                pushReplayCommand = true;
                updateJudge(t, idx, judge.area, slot);
                break;

            case JudgeArea::EARLY_KPOOR:
                pushReplayCommand = true;
                updateJudge(t, idx, judge.area, slot);
                break;

            case JudgeArea::NOTHING:
            case JudgeArea::MISS:
            case JudgeArea::LATE_KPOOR:
            case JudgeArea::MINE_KPOOR: break;
            }
            break;
        }
        break;

    case NoteLaneCategory::_:
    case NoteLaneCategory::Mine:
    case NoteLaneCategory::EXTRA:
    case NoteLaneCategory::NOTECATEGORY_COUNT: break;
    }

    if (note.expired || judge.area == JudgeArea::EARLY_KPOOR || judge.area == JudgeArea::MINE_KPOOR)
    {
        _lastNoteJudge[slot] = judge;
    }

    // push replay command
    if (pushReplayCommand && _hasStartTime && _replayNew)
    {
        std::unique_lock rl{_replayNew->mutex};
        if (judgeAreaReplayCommandType[slot].find(judge.area) != judgeAreaReplayCommandType[slot].end())
        {
            long long ms = t.norm() - _startTime.norm();
            ReplayChart::Commands cmd;
            cmd.ms = ms;
            cmd.type = judgeAreaReplayCommandType[slot].at(judge.area);
            _replayNew->replay->commands.push_back(cmd);
        }
    }
}

// Judges LNs in the case of overhold.
void RulesetBMS::_judgeHold(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, const JudgeRes& judge,
                            const lunaticvibes::Time& t, int slot)
{
    switch (cat)
    {
    case NoteLaneCategory::Mine: {
        if (judge.area == JudgeArea::EXACT_PERFECT || (judge.area == JudgeArea::EARLY_PERFECT && judge.time < -2) ||
            (judge.area == JudgeArea::LATE_PERFECT && judge.time < 2))
        {
            note.hit = true;
            note.expired = true;
            _updateHp(-0.01 * note.dvalue / 2);

            // kpoor + 1
            for (auto& i : JudgeAreaIndexAccMap.at(JudgeArea::MINE_KPOOR))
            {
                ++_basic.judge[i];
            }
            if (showJudge)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    State::set(IndexTimer::PLAY_JUDGE_1P, t.norm());
                    setJudgeInternalTimer1P(JudgeType::KPOOR, t.norm());
                    SoundMgr::playSysSample(SoundChannelType::KEY_LEFT, eSoundSample::SOUND_LANDMINE);
                }
                else if (slot == PLAYER_SLOT_TARGET)
                {
                    State::set(IndexTimer::PLAY_JUDGE_2P, t.norm());
                    setJudgeInternalTimer2P(JudgeType::KPOOR, t.norm());
                    SoundMgr::playSysSample(SoundChannelType::KEY_RIGHT, eSoundSample::SOUND_LANDMINE);
                }
            }

            _lastNoteJudge = {{JudgeArea::MINE_KPOOR, t.norm()}};

            // push replay command
            if (_hasStartTime && _replayNew)
            {
                std::unique_lock rl{_replayNew->mutex};
                long long ms = t.norm() - _startTime.norm();
                ReplayChart::Commands cmd;
                cmd.ms = ms;
                cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LANDMINE
                                                      : ReplayChart::Commands::Type::JUDGE_RIGHT_LANDMINE;
                _replayNew->replay->commands.push_back(cmd);
            }
        }
        break;
    }
    case NoteLaneCategory::LN:
        if ((note.flags & Note::LN_TAIL) && _lnJudge[idx] != RulesetBMS::JudgeArea::NOTHING &&
            _lnJudge[idx] != RulesetBMS::JudgeArea::EARLY_BAD && _lnJudge[idx] != RulesetBMS::JudgeArea::LATE_BAD)
        {
            if (judge.area == JudgeArea::EXACT_PERFECT || (judge.area == JudgeArea::EARLY_PERFECT && judge.time < -2) ||
                (judge.area == JudgeArea::LATE_PERFECT && judge.time < 2))
            {
                note.hit = true;
                note.expired = true;
                notesExpired++;
                updateJudge(t, idx, _lnJudge[idx], slot);

                if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    State::set(_bombLNTimerMap->at(idx), TIMER_NEVER);

                _lastNoteJudge[slot].area = _lnJudge[idx];
                _lastNoteJudge[slot].time = 0;

                // push replay command
                if (_hasStartTime && _replayNew)
                {
                    std::unique_lock rl{_replayNew->mutex};
                    if (judgeAreaReplayCommandType[slot].find(_lnJudge[idx]) != judgeAreaReplayCommandType[slot].end())
                    {
                        long long ms = t.norm() - _startTime.norm();
                        ReplayChart::Commands cmd;
                        cmd.ms = ms;
                        cmd.type = judgeAreaReplayCommandType[slot].at(_lnJudge[idx]);
                        _replayNew->replay->commands.push_back(cmd);
                    }
                }

                _lnJudge[idx] = RulesetBMS::JudgeArea::NOTHING;
            }
        }
        break;

    default: break;
    }
}
void RulesetBMS::_judgeRelease(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, const JudgeRes& judge,
                               const lunaticvibes::Time& t, int slot)
{
    bool pushReplayCommand = false;
    switch (cat)
    {
    case NoteLaneCategory::LN:
        if ((note.flags & Note::LN_TAIL) && _lnJudge[idx] != RulesetBMS::JudgeArea::NOTHING)
        {
            bool hit = true;
            JudgeArea lnJudge = judge.area;
            switch (judge.area)
            {
            case JudgeArea::NOTHING:
            case JudgeArea::EARLY_KPOOR:
            case JudgeArea::EARLY_BAD:
                lnJudge = JudgeArea::EARLY_BAD;
                hit = false;
                break;

            default:
                switch (_lnJudge[idx])
                {
                case JudgeArea::EARLY_PERFECT:
                case JudgeArea::LATE_PERFECT:
                    if (judge.area == JudgeArea::EARLY_PERFECT)
                        lnJudge = JudgeArea::EARLY_PERFECT;
                    break;

                case JudgeArea::EARLY_GREAT:
                case JudgeArea::LATE_GREAT:
                    if (judge.area == JudgeArea::EARLY_PERFECT || judge.area == JudgeArea::EARLY_GREAT)
                        lnJudge = JudgeArea::EARLY_GREAT;
                    break;

                case JudgeArea::EARLY_GOOD:
                case JudgeArea::LATE_GOOD:
                    if (judge.area == JudgeArea::EARLY_PERFECT || judge.area == JudgeArea::EARLY_GREAT ||
                        judge.area == JudgeArea::EARLY_GOOD)
                        lnJudge = JudgeArea::EARLY_GOOD;
                    break;

                default: lnJudge = JudgeArea::EARLY_BAD; break;
                }
                break;
            }

            note.hit = hit;
            note.expired = true;
            notesExpired++;
            updateJudge(t, idx, lnJudge, slot);
            _lnJudge[idx] = RulesetBMS::JudgeArea::NOTHING;
            _lastNoteJudge[slot] = judge;
            pushReplayCommand = true;

            if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                State::set(_bombLNTimerMap->at(idx), TIMER_NEVER);

            break;
        }
        break;

    default: break;
    }

    // push replay command
    if (pushReplayCommand && _hasStartTime && _replayNew)
    {
        std::unique_lock rl{_replayNew->mutex};
        if (judgeAreaReplayCommandType[slot].find(judge.area) != judgeAreaReplayCommandType[slot].end())
        {
            long long ms = t.norm() - _startTime.norm();
            ReplayChart::Commands cmd;
            cmd.ms = ms;
            cmd.type = judgeAreaReplayCommandType[slot].at(judge.area);
            _replayNew->replay->commands.push_back(cmd);
        }
    }
}

void RulesetBMS::_updateHp(double diff)
{
    // TOTAL補正, totalnotes補正
    // ref: https://web.archive.org/web/20150226213104/http://2nd.geocities.jp/yoshi_65c816/bms/LR2.html
    // TODO: instead match https://github.com/aeventyr/LR2GAS_pub/blob/main/src/gas.cpp
    if ((_gauge == RulesetBMS::GaugeType::HARD || _gauge == RulesetBMS::GaugeType::EXHARD) && diff < 0)
    {
        double pTotal = 1.0;
        if (total >= 240)
            ;
        else if (total >= 230)
            pTotal = 10.0 / 9;
        else if (total >= 210)
            pTotal = 1.25;
        else if (total >= 200)
            pTotal = 1.5;
        else if (total >= 180)
            pTotal = 5.0 / 3;
        else if (total >= 160)
            pTotal = 2.0;
        else if (total >= 150)
            pTotal = 2.5;
        else if (total >= 130)
            pTotal = 10.0 / 3;
        else if (total >= 120)
            pTotal = 5.0;
        else
            pTotal = 10.0;

        double pNotes = 1.0;
        unsigned notes = getNoteCount();
        if (notes >= 1000)
            ;
        else if (notes >= 500)
            pNotes = (notes - 500) * 0.002;
        else if (notes >= 250)
            pNotes = 1.0 + (notes - 250) * 0.004;
        else if (notes >= 125)
            pNotes = 2.0 + (notes - 125) * 0.008;
        else if (notes >= 62)
            pNotes = 3.0 + (notes - 62) * (1.0 / 62);
        else if (notes >= 31)
            pNotes = 4.0 + (notes - 31) * (1.0 / 31);
        else if (notes >= 16)
            pNotes = 5.0 + (notes - 16) * 0.0625;
        else if (notes >= 8)
            pNotes = 6.0 + (notes - 8) * 0.125;
        else if (notes >= 4)
            pNotes = 7.0 + (notes - 4) * 0.25;
        else if (notes >= 2)
            pNotes = 8.0 + (notes - 2) * 0.50;
        else if (notes == 1)
            pNotes = 9.0;
        else
            pNotes = 10.0;

        diff *= 1.0 * std::max(pTotal, pNotes);
    }

    double tmp = _basic.health;

    // 30% buff
    if ((_gauge == RulesetBMS::GaugeType::HARD || _gauge == RulesetBMS::GaugeType::GRADE) && tmp < 0.32 && diff < 0.0)
    {
        tmp += diff * 0.6;
    }
    else
    {
        tmp += diff;
    }

    _basic.health = std::max(_minHealth, std::min(1.0, tmp));

    if (failWhenNoHealth() && _basic.health <= _minHealth)
    {
        fail();
    }
}
void RulesetBMS::_updateHp(JudgeArea judge)
{
    _updateHp(_healthGain.at(JudgeAreaTypeMap.at(judge)));
}

void RulesetBMS::updateJudge(const lunaticvibes::Time& t, const NoteLaneIndex ch, const RulesetBMS::JudgeArea judge,
                             const int slot)
{
    if (isFailed())
        return;

    for (auto& i : JudgeAreaIndexAccMap.at(judge))
    {
        ++_basic.judge[i];
    }

    switch (judge)
    {
    case JudgeArea::EARLY_PERFECT:
    case JudgeArea::EXACT_PERFECT:
    case JudgeArea::LATE_PERFECT:
        // moneyScore += 150000.0 / getNoteCount() +
        //     std::min(int(_basic.combo) - 1, 10) * 50000.0 / (10 * getNoteCount() - 55);
        moneyScore += 1.0 * maxMoneyScore / getNoteCount();
        exScore += 2;
        ++_basic.combo;
        break;

    case JudgeArea::EARLY_GREAT:
    case JudgeArea::LATE_GREAT:
        // moneyScore += 100000.0 / getNoteCount() +
        //     std::min(int(_basic.combo) - 1, 10) * 50000.0 / (10 * getNoteCount() - 55);
        moneyScore += 0.5 * maxMoneyScore / getNoteCount();
        exScore += 1;
        ++_basic.combo;
        break;

    case JudgeArea::EARLY_GOOD:
    case JudgeArea::LATE_GOOD:
        // moneyScore += 20000.0 / getNoteCount() +
        //     std::min(int(_basic.combo) - 1, 10) * 50000.0 / (10 * getNoteCount() - 55);
        moneyScore += 0.25 * maxMoneyScore / getNoteCount();
        ++_basic.combo;
        break;

    case JudgeArea::EARLY_BAD:
    case JudgeArea::LATE_BAD:
    case JudgeArea::MISS:
        _basic.combo = 0;
        _basic.comboDisplay = 0;
        break;

    case JudgeArea::NOTHING:
    case JudgeArea::EARLY_KPOOR:
    case JudgeArea::LATE_KPOOR:
    case JudgeArea::MINE_KPOOR: break;
    }

    _updateHp(judge);
    if (_basic.combo > _basic.maxCombo)
        _basic.maxCombo = _basic.combo;
    if (_basic.combo + _basic.comboDisplay > _basic.maxComboDisplay)
        _basic.maxComboDisplay = _basic.combo + _basic.comboDisplay;
    if (_basic.judge[JUDGE_CB] == 0)
        _basic.firstMaxCombo = _basic.combo;

    unsigned max = getNoteCount() * 2;
    _basic.total_acc = 100.0 * exScore / max;
    _basic.acc = notesExpired ? (100.0 * exScore / notesExpired / 2) : 0;

    const JudgeType judgeType = JudgeAreaTypeMap.at(judge);
    if (showJudge)
    {
        const bool should_show_bomb = judgeType == JudgeType::PERFECT || judgeType == JudgeType::GREAT;
        if (should_show_bomb && _bombTimerMap != nullptr)
        {
            if (auto it = _bombTimerMap->find(ch); it != _bombTimerMap->end())
            {
                State::set(it->second, t.norm());
            }
        }

        if (slot == PLAYER_SLOT_PLAYER)
        {
            State::set(IndexTimer::PLAY_JUDGE_1P, t.norm());
            setJudgeInternalTimer1P(judgeType, t.norm());
            State::set(IndexNumber::_DISP_NOWCOMBO_1P, _basic.combo + _basic.comboDisplay);
            State::set(IndexOption::PLAY_LAST_JUDGE_1P, JudgeTypeOptMap.at(judgeType));
        }
        else if (slot == PLAYER_SLOT_TARGET)
        {
            State::set(IndexTimer::PLAY_JUDGE_2P, t.norm());
            setJudgeInternalTimer2P(judgeType, t.norm());
            State::set(IndexNumber::_DISP_NOWCOMBO_2P, _basic.combo + _basic.comboDisplay);
            State::set(IndexOption::PLAY_LAST_JUDGE_2P, JudgeTypeOptMap.at(judgeType));
        }
    }
}

static bool isMainUserSide(RulesetBMS::PlaySide side)
{
    switch (side)
    {
    case RulesetBMS::PlaySide::SINGLE:
    case RulesetBMS::PlaySide::DOUBLE:
    case RulesetBMS::PlaySide::BATTLE_1P: return true;
    case RulesetBMS::PlaySide::BATTLE_2P:
    case RulesetBMS::PlaySide::AUTO:
    case RulesetBMS::PlaySide::AUTO_2P:
    case RulesetBMS::PlaySide::AUTO_DOUBLE:
    case RulesetBMS::PlaySide::RIVAL:
    case RulesetBMS::PlaySide::MYBEST:
    case RulesetBMS::PlaySide::NETWORK: return false;
    }
    lunaticvibes::assert_failed("isMainUserSide");
}

static bool isBadOrBetter(const RulesetBMS::JudgeArea area)
{
    switch (area)
    {
    case RulesetBMS::JudgeArea::EARLY_BAD:
    case RulesetBMS::JudgeArea::EARLY_GOOD:
    case RulesetBMS::JudgeArea::EARLY_GREAT:
    case RulesetBMS::JudgeArea::EARLY_PERFECT:
    case RulesetBMS::JudgeArea::EXACT_PERFECT:
    case RulesetBMS::JudgeArea::LATE_PERFECT:
    case RulesetBMS::JudgeArea::LATE_GREAT:
    case RulesetBMS::JudgeArea::LATE_GOOD:
    case RulesetBMS::JudgeArea::LATE_BAD: return true;
    case RulesetBMS::JudgeArea::NOTHING:
    case RulesetBMS::JudgeArea::EARLY_KPOOR:
    case RulesetBMS::JudgeArea::MISS:
    case RulesetBMS::JudgeArea::LATE_KPOOR:
    case RulesetBMS::JudgeArea::MINE_KPOOR: return false;
    }
    lunaticvibes::assert_failed("isBadOrBetter");
}

void RulesetBMS::updateAutoadjust(const JudgeRes& j, const lunaticvibes::Time& rt)
{
    if (!isMainUserSide(_side))
        return;
    if (State::get(IndexOption::PLAY_AUTOADJUST) == 0)
        return;
    const bool isOffsetPositive = j.time >= 0;
    if (isBadOrBetter(j.area))
    {
        _notesSinceLastAutoadjust++;
        if (_notesSinceLastAutoadjust > 9)
        {
            if (j.time.norm() != 0)
            {
                // NOTE: compared to adjust buttons this doesn't need to clamp. Some people do in fact abuse this.
                const int newAdjust = State::get(IndexNumber::TIMING_ADJUST_VISUAL) + (isOffsetPositive ? 1 : -1);
                State::set(IndexNumber::TIMING_ADJUST_VISUAL, newAdjust);
            }
            _notesSinceLastAutoadjust = 0;
        }
    }
}

static std::pair<NoteLaneIndex, HitableNote*> getClosestNote(ChartObjectBase& chart, const Input::Pad k,
                                                             const NoteLaneCategory cat)
{
    NoteLaneIndex idx = chart.getLaneFromKey(cat, k);
    if (idx != _ && !chart.isLastNote(cat, idx))
    {
        auto itNote = chart.incomingNote(cat, idx);
        while (!chart.isLastNote(cat, idx, itNote) && itNote->expired)
            ++itNote;
        if (!chart.isLastNote(cat, idx, itNote))
            return {idx, &*itNote};
    }
    return {};
}

void RulesetBMS::judgeNotePress(const Input::Pad k, const lunaticvibes::Time& t, const lunaticvibes::Time& rt,
                                const int slot)
{
    auto [idx, note] = getClosestNote(*_chart, k, NoteLaneCategory::Note);
    auto category = NoteLaneCategory::Note;
    {
        auto [idx2, note2] = getClosestNote(*_chart, k, NoteLaneCategory::LN);
        if (note2 && (note == nullptr || note2->time < note->time))
        {
            idx = idx2;
            note = note2;
            category = NoteLaneCategory::LN;
        }
    }

    if (note && !note->expired)
    {
        const JudgeRes j = _calcJudgeByTimes(*note, rt);
        updateAutoadjust(j, rt);
        _judgePress(category, idx, *note, j, t, slot);
    }
}
void RulesetBMS::judgeNoteHold(Input::Pad k, const lunaticvibes::Time& t, const lunaticvibes::Time& rt, int slot)
{
    NoteLaneIndex idx;

    idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, k);
    if (idx != _ && !_chart->isLastNote(NoteLaneCategory::Mine, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::Mine, idx);
        auto j = _calcJudgeByTimes(note, rt);
        _judgeHold(NoteLaneCategory::Mine, idx, note, j, t, slot);
    }

    idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    if (idx != _ && !_chart->isLastNote(NoteLaneCategory::LN, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::LN, idx);
        auto j = _calcJudgeByTimes(note, rt);
        _judgeHold(NoteLaneCategory::LN, idx, note, j, t, slot);
    }
}
void RulesetBMS::judgeNoteRelease(Input::Pad k, const lunaticvibes::Time& t, const lunaticvibes::Time& rt, int slot)
{
    NoteLaneIndex idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    if (idx != _)
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx);
        while (!_chart->isLastNote(NoteLaneCategory::LN, idx, itNote))
        {
            if (!itNote->expired)
            {
                auto j = _calcJudgeByTimes(*itNote, rt);
                _judgeRelease(NoteLaneCategory::LN, idx, *itNote, j, t, slot);
                break;
            }
            ++itNote;
        }
    }
}

void RulesetBMS::updatePress(InputMask& pg, const lunaticvibes::Time& t, const lunaticvibes::InputMaskTimes& tt)
{
    if (t.norm() - _startTime.norm() < 0)
        return;
    if (gPlayContext.isAuto)
        return;
    auto updatePressRange = [&](Input::Pad begin, Input::Pad end, int slot) {
        for (size_t k = begin; k <= static_cast<size_t>(end); ++k)
        {
            if (!pg[k])
                continue;
            judgeNotePress((Input::Pad)k, t, tt[k] - _startTime, slot);
        }
    };
    if (_k1P)
        updatePressRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P)
        updatePressRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P)
        {
            if (pg[Input::S1L])
                playerScratchDirection[PLAYER_SLOT_PLAYER] = AxisDir::AXIS_UP;
            if (pg[Input::S1R])
                playerScratchDirection[PLAYER_SLOT_PLAYER] = AxisDir::AXIS_DOWN;
            updatePressRange(Input::S1L, Input::S1R, PLAYER_SLOT_PLAYER);
        }
        if (_k2P)
        {
            if (pg[Input::S2L])
                playerScratchDirection[PLAYER_SLOT_TARGET] = AxisDir::AXIS_UP;
            if (pg[Input::S2R])
                playerScratchDirection[PLAYER_SLOT_TARGET] = AxisDir::AXIS_DOWN;
            updatePressRange(Input::S2L, Input::S2R, PLAYER_SLOT_TARGET);
        }
    }
}
void RulesetBMS::updateHold(InputMask& hg, const lunaticvibes::Time& t)
{
    lunaticvibes::Time rt = t - _startTime.norm();
    if (rt < 0)
        return;
    if (gPlayContext.isAuto)
        return;

    auto updateHoldRange = [&](Input::Pad begin, Input::Pad end, int slot) {
        for (size_t k = begin; k <= static_cast<size_t>(end); ++k)
        {
            if (!hg[k])
                continue;
            judgeNoteHold((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P)
        updateHoldRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P)
        updateHoldRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P)
            updateHoldRange(Input::S1L, Input::S1R, PLAYER_SLOT_PLAYER);
        if (_k2P)
            updateHoldRange(Input::S2L, Input::S2R, PLAYER_SLOT_TARGET);
    }
}
void RulesetBMS::updateRelease(InputMask& rg, const lunaticvibes::Time& t)
{
    lunaticvibes::Time rt = t - _startTime.norm();
    if (rt < 0)
        return;
    if (gPlayContext.isAuto)
        return;

    auto updateReleaseRange = [&](Input::Pad begin, Input::Pad end, int slot) {
        for (size_t k = begin; k <= static_cast<size_t>(end); ++k)
        {
            if (!rg[k])
                continue;
            judgeNoteRelease((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P)
        updateReleaseRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P)
        updateReleaseRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P)
        {
            if (playerScratchDirection[PLAYER_SLOT_PLAYER] == AxisDir::AXIS_UP && rg[Input::S1L])
                updateReleaseRange(Input::S1L, Input::S1L, PLAYER_SLOT_PLAYER);
            if (playerScratchDirection[PLAYER_SLOT_PLAYER] == AxisDir::AXIS_DOWN && rg[Input::S1R])
                updateReleaseRange(Input::S1R, Input::S1R, PLAYER_SLOT_PLAYER);
        }
        if (_k2P)
        {
            if (playerScratchDirection[PLAYER_SLOT_TARGET] == AxisDir::AXIS_UP && rg[Input::S2L])
                updateReleaseRange(Input::S2L, Input::S2L, PLAYER_SLOT_TARGET);
            if (playerScratchDirection[PLAYER_SLOT_TARGET] == AxisDir::AXIS_DOWN && rg[Input::S2R])
                updateReleaseRange(Input::S2R, Input::S2R, PLAYER_SLOT_TARGET);
        }
    }
}
void RulesetBMS::updateAxis(double s1, double s2, const lunaticvibes::Time& t)
{
    lunaticvibes::Time rt = t - _startTime.norm();
    if (rt.norm() < 0)
        return;

    using namespace Input;

    if (!gPlayContext.isAuto && (!gPlayContext.isReplay || !_hasStartTime))
    {
        playerScratchAccumulator[PLAYER_SLOT_PLAYER] += s1;
        playerScratchAccumulator[PLAYER_SLOT_TARGET] += s2;
    }
}

void RulesetBMS::update(const lunaticvibes::Time& t)
{
    if (!_hasStartTime)
        setStartTime(t);

    auto rt = t - _startTime.norm();
    _basic.play_time = rt;

    for (auto& [c, n] : _noteListIterators)
    {
        auto [cat, idx] = c;
        while (!_chart->isLastNote(cat, idx, n) && rt >= n->time)
        {
            switch (cat)
            {
            case NoteLaneCategory::Note: notesReached++; break;

            case NoteLaneCategory::LN:
                if (n->flags & Note::LN_TAIL)
                    notesReached++;
                break;

            case NoteLaneCategory::_:
            case NoteLaneCategory::Mine:
            case NoteLaneCategory::Invs:
            case NoteLaneCategory::EXTRA:
            case NoteLaneCategory::NOTECATEGORY_COUNT: break;
            }

            n++;
        }
    }

    auto updateRange = [&](Input::Pad begin, Input::Pad end, int slot) {
        for (size_t k = begin; k <= static_cast<size_t>(end); ++k)
        {
            auto is_scratch_input = [](size_t k) {
                switch (k)
                {
                case Input::S1L:
                case Input::S1R:
                case Input::S2L:
                case Input::S2R: return true;
                default: return false;
                }
            };
            const bool scratch = is_scratch_input(k);

            NoteLaneIndex idx;

            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Note, idx, itNote) && !itNote->expired)
                {
                    const lunaticvibes::Time latePoorWindow =
                        (!scratch || _judgeScratch) ? judgeTime[(size_t)_judgeDifficulty].BAD : 0;
                    if (rt - itNote->time >= latePoorWindow)
                    {
                        itNote->expired = true;

                        if (!scratch || _judgeScratch)
                        {
                            updateJudge(t, idx, JudgeArea::MISS, slot);
                            _lastNoteJudge[slot].area = JudgeArea::MISS;
                            _lastNoteJudge[slot].time = latePoorWindow;

                            // push replay command
                            if (_hasStartTime && _replayNew)
                            {
                                std::unique_lock rl{_replayNew->mutex};
                                long long ms = t.norm() - _startTime.norm();
                                ReplayChart::Commands cmd;
                                cmd.ms = ms;
                                cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4
                                                                      : ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4;
                                _replayNew->replay->commands.push_back(cmd);
                            }
                        }

                        notesExpired++;
                        // LOG_DEBUG << "LATE   POOR    "; break;
                    }
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::LN, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx);
                while (!_chart->isLastNote(NoteLaneCategory::LN, idx, itNote) && !itNote->expired)
                {
                    if (!(itNote->flags & Note::LN_TAIL))
                    {
                        if (rt >= itNote->time)
                        {
                            lunaticvibes::Time hitTime = itNote->time + judgeTime[(size_t)_judgeDifficulty].BAD;
                            auto itTail = itNote;
                            itTail++;
                            if (!_chart->isLastNote(NoteLaneCategory::LN, idx, itTail) &&
                                (itTail->flags & Note::LN_TAIL) && hitTime > itTail->time)
                            {
                                hitTime = itTail->time;
                            }
                            if (rt >= hitTime)
                            {
                                itNote->expired = true;

                                if (!scratch || _judgeScratch)
                                {
                                    updateJudge(t, idx, JudgeArea::MISS, slot);
                                    _lastNoteJudge[slot].area = JudgeArea::MISS;
                                    _lastNoteJudge[slot].time = hitTime;

                                    // push replay command
                                    if (_hasStartTime && _replayNew)
                                    {
                                        std::unique_lock rl{_replayNew->mutex};
                                        long long ms = t.norm() - _startTime.norm();
                                        ReplayChart::Commands cmd;
                                        cmd.ms = ms;
                                        cmd.type = slot == PLAYER_SLOT_PLAYER
                                                       ? ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3
                                                       : ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3;
                                        _replayNew->replay->commands.push_back(cmd);
                                    }
                                }

                                // LOG_DEBUG << "LATE   POOR    "; break;
                            }
                        }
                    }
                    else
                    {
                        auto itHead = itNote;
                        itHead--;
                        if (rt >= itNote->time)
                        {
                            if (!scratch || _judgeScratch)
                            {
                                if (!itHead->hit)
                                {
                                    itNote->expired = true;
                                    notesExpired++;
                                }
                            }
                        }
                    }
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                const lunaticvibes::Time& hitTime = -judgeTime[(size_t)_judgeDifficulty].BAD;
                auto itNote = _chart->incomingNote(NoteLaneCategory::Invs, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Invs, idx, itNote) && !itNote->expired &&
                       rt - itNote->time >= hitTime)
                {
                    itNote->expired = true;
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Mine, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Mine, idx, itNote) && !itNote->expired &&
                       rt >= itNote->time)
                {
                    itNote->expired = true;
                    itNote++;
                }
            }
        }
    };
    if (_k1P)
        updateRange(Input::S1L, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P)
        updateRange(Input::S2L, Input::K29, PLAYER_SLOT_TARGET);

    if (_judgeScratch)
    {
        auto updateScratch = [&](const lunaticvibes::Time& t, Input::Pad up, Input::Pad dn, double& val, int slot) {
            double scratchThreshold = 0.001;
            double scratchRewind = 0.0001;
            if (val > scratchThreshold)
            {
                // scratch down
                val -= scratchThreshold;

                switch (playerScratchDirection[slot])
                {
                case AxisDir::AXIS_DOWN: judgeNoteHold(dn, t, rt, slot); break;
                case AxisDir::AXIS_UP:
                    judgeNoteRelease(up, t, rt, slot);
                    judgeNotePress(dn, t, rt, slot);
                    break;
                case AxisDir::AXIS_NONE:
                    judgeNoteRelease(up, t, rt, slot);
                    judgeNotePress(dn, t, rt, slot);
                    break;
                }

                playerScratchLastUpdate[slot] = t;
                playerScratchDirection[slot] = AxisDir::AXIS_DOWN;
            }
            else if (val < -scratchThreshold)
            {
                // scratch up
                val += scratchThreshold;

                switch (playerScratchDirection[slot])
                {
                case AxisDir::AXIS_UP: judgeNoteHold(up, t, rt, slot); break;
                case AxisDir::AXIS_DOWN:
                    judgeNoteRelease(dn, t, rt, slot);
                    judgeNotePress(up, t, rt, slot);
                    break;
                case AxisDir::AXIS_NONE:
                    judgeNoteRelease(dn, t, rt, slot);
                    judgeNotePress(up, t, rt, slot);
                    break;
                }

                playerScratchLastUpdate[slot] = t;
                playerScratchDirection[slot] = AxisDir::AXIS_UP;
            }

            if (val > scratchRewind)
                val -= scratchRewind;
            else if (val < -scratchRewind)
                val += scratchRewind;
            else
                val = 0.;

            if ((t - playerScratchLastUpdate[slot]).norm() > 133)
            {
                // release
                switch (playerScratchDirection[slot])
                {
                case AxisDir::AXIS_UP: judgeNoteRelease(up, t, rt, slot); break;
                case AxisDir::AXIS_DOWN: judgeNoteRelease(dn, t, rt, slot); break;
                }

                playerScratchDirection[slot] = AxisDir::AXIS_NONE;
                playerScratchLastUpdate[slot] = TIMER_NEVER;
            }
        };
        updateScratch(t, Input::S1L, Input::S1R, playerScratchAccumulator[PLAYER_SLOT_PLAYER], PLAYER_SLOT_PLAYER);
        updateScratch(t, Input::S2L, Input::S2R, playerScratchAccumulator[PLAYER_SLOT_TARGET], PLAYER_SLOT_TARGET);
    }

    _isCleared = isCleared();

    if (isFinished())
        _isFailed |= _basic.health < getClearHealth();

    updateGlobals();
}

double RulesetBMS::getScore() const
{
    return moneyScore;
}

double RulesetBMS::getMaxMoneyScore() const
{
    return maxMoneyScore;
}

unsigned RulesetBMS::getExScore() const
{
    return exScore;
}

unsigned RulesetBMS::getJudgeCount(JudgeType idx) const
{
    switch (idx)
    {
    case JudgeType::PERFECT: return _basic.judge[JUDGE_PERFECT];
    case JudgeType::GREAT: return _basic.judge[JUDGE_GREAT];
    case JudgeType::GOOD: return _basic.judge[JUDGE_GOOD];
    case JudgeType::BAD: return _basic.judge[JUDGE_BAD];
    case JudgeType::KPOOR: return _basic.judge[JUDGE_KPOOR];
    case JudgeType::MISS: return _basic.judge[JUDGE_MISS];
    }
    return 0;
}

unsigned RulesetBMS::getJudgeCountEx(JudgeIndex idx) const
{
    return _basic.judge[idx];
}

std::string RulesetBMS::getModifierText() const
{
    return modifierText;
}
std::string RulesetBMS::getModifierTextShort() const
{
    return modifierTextShort;
}

unsigned RulesetBMS::getNoteCount() const
{
    return noteCount;
}

unsigned RulesetBMS::getMaxCombo() const
{
    if (_judgeScratch)
    {
        return getNoteCount();
    }
    else
    {
        unsigned count = getNoteCount();
        auto pChart = std::dynamic_pointer_cast<ChartObjectBMS>(_chart);
        if (pChart != nullptr)
        {
            count -= pChart->getScratchCount();
        }
        return count;
    }
}

void RulesetBMS::fail()
{
    _isFailed = true;

    _basic.health = _minHealth;
    _basic.combo = 0;

    int notesRemain = getNoteCount() - notesExpired;
    _basic.judge[JUDGE_BP] += notesRemain;
    _basic.judge[JUDGE_CB] += notesRemain;
    notesExpired = notesReached = getNoteCount();

    //_basic.acc = _basic.total_acc;
}

void RulesetBMS::updateGlobals()
{
    if (_side == PlaySide::SINGLE || _side == PlaySide::DOUBLE || _side == PlaySide::BATTLE_1P ||
        _side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE) // includes DP
    {
        State::set(IndexNumber::PLAY_1P_SCORE, int(std::round(moneyScore)));
        State::set(IndexNumber::PLAY_1P_EXSCORE, exScore);
        State::set(IndexNumber::PLAY_1P_NOWCOMBO, _basic.combo + _basic.comboDisplay);
        State::set(IndexNumber::PLAY_1P_MAXCOMBO, _basic.maxComboDisplay);
        State::set(IndexNumber::PLAY_1P_RATE, int(std::floor(_basic.acc)));
        State::set(IndexNumber::PLAY_1P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        State::set(IndexNumber::PLAY_1P_TOTALNOTES, getNoteCount());
        State::set(IndexNumber::PLAY_1P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        State::set(IndexNumber::PLAY_1P_TOTAL_RATE_DECIMAL2,
                   int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        State::set(IndexNumber::PLAY_1P_PERFECT, _basic.judge[JUDGE_PERFECT]);
        State::set(IndexNumber::PLAY_1P_GREAT, _basic.judge[JUDGE_GREAT]);
        State::set(IndexNumber::PLAY_1P_GOOD, _basic.judge[JUDGE_GOOD]);
        State::set(IndexNumber::PLAY_1P_BAD, _basic.judge[JUDGE_BAD]);
        State::set(IndexNumber::PLAY_1P_POOR, _basic.judge[JUDGE_POOR]);
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE, int(_basic.health * 100));
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE_AFTER_DOT, int(_basic.health * 100'00) % 100);

        State::set(IndexNumber::PLAY_1P_MISS, _basic.judge[JUDGE_MISS]);
        State::set(IndexNumber::PLAY_1P_FAST_COUNT, _basic.judge[JUDGE_EARLY]);
        State::set(IndexNumber::PLAY_1P_SLOW_COUNT, _basic.judge[JUDGE_LATE]);
        State::set(IndexNumber::PLAY_1P_COMBOBREAK, _basic.judge[JUDGE_CB]);
        State::set(IndexNumber::PLAY_1P_BPOOR, _basic.judge[JUDGE_KPOOR]);
        State::set(IndexNumber::PLAY_1P_BP, _basic.judge[JUDGE_BP]);
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_COUNT, _basic.judge[JUDGE_EARLY]);
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_SLOW_COUNT, _basic.judge[JUDGE_LATE]);
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_COMBOBREAK, _basic.judge[JUDGE_CB]);

        if (showJudge)
        {
            // 1:fast 2:slow
            auto get_fastslow = [](JudgeArea area) {
                switch (area)
                {
                case JudgeArea::EARLY_GREAT:
                case JudgeArea::EARLY_GOOD:
                case JudgeArea::EARLY_BAD:
                case JudgeArea::EARLY_KPOOR: return 1;
                case JudgeArea::LATE_GREAT:
                case JudgeArea::LATE_GOOD:
                case JudgeArea::LATE_BAD:
                case JudgeArea::MISS:
                case JudgeArea::LATE_KPOOR: return 2;
                case JudgeArea::NOTHING:
                case JudgeArea::EARLY_PERFECT:
                case JudgeArea::EXACT_PERFECT:
                case JudgeArea::LATE_PERFECT:
                case JudgeArea::MINE_KPOOR: return 0;
                }
                lunaticvibes::assert_failed("get_fastslow");
            };

            const int fs_of_player = get_fastslow(_lastNoteJudge[PLAYER_SLOT_PLAYER].area);
            State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_SLOW, fs_of_player);
            State::set(IndexOption::PLAY_LAST_JUDGE_FASTSLOW_1P, fs_of_player);
            State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_JUDGE_TIME_ERROR_MS,
                       _lastNoteJudge[PLAYER_SLOT_PLAYER].time.norm());
            State::set(IndexNumber::PLAY_1P_JUDGE_TIME_ERROR_MS, _lastNoteJudge[PLAYER_SLOT_PLAYER].time.norm());

            if (_side == PlaySide::DOUBLE || _side == PlaySide::AUTO_DOUBLE)
            {
                const int fs_of_target = get_fastslow(_lastNoteJudge[PLAYER_SLOT_TARGET].area);
                State::set(IndexNumber::LR2IR_REPLACE_PLAY_2P_FAST_SLOW, fs_of_target);
                State::set(IndexOption::PLAY_LAST_JUDGE_FASTSLOW_2P, fs_of_target);
                State::set(IndexNumber::LR2IR_REPLACE_PLAY_2P_JUDGE_TIME_ERROR_MS,
                           _lastNoteJudge[PLAYER_SLOT_TARGET].time.norm());
                State::set(IndexNumber::PLAY_2P_JUDGE_TIME_ERROR_MS, _lastNoteJudge[PLAYER_SLOT_TARGET].time.norm());
            }
        }

        State::set(IndexOption::PLAY_RANK_ESTIMATED_1P, Option::getRankType(_basic.acc));
        State::set(IndexOption::PLAY_RANK_BORDER_1P, Option::getRankType(_basic.total_acc));
        State::set(IndexOption::RESULT_RANK_1P, Option::getRankType(_basic.total_acc));
        State::set(IndexOption::PLAY_HEALTH_1P, Option::getHealthType(_basic.health));

        int maxScore = getMaxScore();
        // if      (dp.total_acc >= 94.44) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * 1.000 -
        // dp.score2));    // MAX-
        if (_basic.total_acc >= 100.0 * 8.0 / 9)
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, exScore - maxScore); // MAX-
        else if (_basic.total_acc >= 100.0 * 7.0 / 9)
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 8.0 / 9)); // AAA-
        else if (_basic.total_acc >= 100.0 * 6.0 / 9)
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 7.0 / 9)); // AA-
        else if (_basic.total_acc >= 100.0 * 5.0 / 9)
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 6.0 / 9)); // A-
        else if (_basic.total_acc >= 100.0 * 4.0 / 9)
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 5.0 / 9)); // B-
        else if (_basic.total_acc >= 100.0 * 3.0 / 9)
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 4.0 / 9)); // C-
        else if (_basic.total_acc >= 100.0 * 2.0 / 9)
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 3.0 / 9)); // D-
        else
            State::set(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 2.0 / 9)); // E-
        State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, State::get(IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF));

        State::set(IndexNumber::LR2IR_REPLACE_PLAY_RUNNING_NOTES, notesExpired);
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_REMAIN_NOTES, getNoteCount() - notesExpired);

        Option::e_lamp_type lamp = Option::LAMP_NOPLAY;
        if (isNoScore() && _basic.judge[JUDGE_BP] == 0)
        {
            lamp = Option::LAMP_NOPLAY;
        }
        else if (_basic.judge[JUDGE_CB] == 0)
        {
            if (_basic.acc >= 100.0)
                lamp = Option::LAMP_MAX;
            else if (_basic.judge[JUDGE_GOOD] == 0)
                lamp = Option::LAMP_PERFECT;
            else
                lamp = Option::LAMP_FULLCOMBO;
        }
        else if (!isFailed())
        {
            switch (_gauge)
            {
            case GaugeType::HARD: lamp = Option::LAMP_HARD; break;
            case GaugeType::EXHARD: lamp = Option::LAMP_EXHARD; break;
            case GaugeType::DEATH: lamp = Option::LAMP_FULLCOMBO; break;
            // case GaugeType::P_ATK:      lamp = Option::LAMP_FULLCOMBO; break;
            // case GaugeType::G_ATK:      lamp = Option::LAMP_FULLCOMBO; break;
            case GaugeType::GROOVE: lamp = Option::LAMP_NORMAL; break;
            case GaugeType::EASY: lamp = Option::LAMP_EASY; break;
            case GaugeType::ASSIST: lamp = Option::LAMP_ASSIST; break;
            case GaugeType::GRADE: lamp = Option::LAMP_NOPLAY; break;
            case GaugeType::EXGRADE: lamp = Option::LAMP_NOPLAY; break;
            default: break;
            }
        }
        else
        {
            lamp = Option::LAMP_FAILED;
        }
        State::set(IndexOption::RESULT_CLEAR_TYPE_1P, std::min(lamp, saveLampMax));
    }
    else if (_side == PlaySide::BATTLE_2P || _side == PlaySide::AUTO_2P || _side == PlaySide::RIVAL) // excludes DP
    {
        State::set(IndexNumber::PLAY_2P_SCORE, int(std::round(moneyScore)));
        if (_side == PlaySide::RIVAL)
        {
            // target exscore is affected by target type. Handle in ScenePlay
        }
        else
        {
            State::set(IndexNumber::PLAY_2P_EXSCORE, exScore);
        }
        State::set(IndexNumber::PLAY_2P_NOWCOMBO, _basic.combo + _basic.comboDisplay);
        State::set(IndexNumber::PLAY_2P_MAXCOMBO, _basic.maxComboDisplay);
        State::set(IndexNumber::PLAY_2P_RATE, int(std::floor(_basic.acc)));
        State::set(IndexNumber::PLAY_2P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        State::set(IndexNumber::PLAY_2P_TOTALNOTES, getNoteCount());
        State::set(IndexNumber::PLAY_2P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        State::set(IndexNumber::PLAY_2P_TOTAL_RATE_DECIMAL2,
                   int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        State::set(IndexNumber::PLAY_2P_PERFECT, _basic.judge[JUDGE_PERFECT]);
        State::set(IndexNumber::PLAY_2P_GREAT, _basic.judge[JUDGE_GREAT]);
        State::set(IndexNumber::PLAY_2P_GOOD, _basic.judge[JUDGE_GOOD]);
        State::set(IndexNumber::PLAY_2P_BAD, _basic.judge[JUDGE_BAD]);
        State::set(IndexNumber::PLAY_2P_POOR, _basic.judge[JUDGE_POOR]);
        State::set(IndexNumber::PLAY_2P_GROOVEGAUGE, int(_basic.health * 100));

        State::set(IndexNumber::PLAY_2P_MISS, _basic.judge[JUDGE_MISS]);
        State::set(IndexNumber::PLAY_2P_FAST_COUNT, _basic.judge[JUDGE_EARLY]);
        State::set(IndexNumber::PLAY_2P_SLOW_COUNT, _basic.judge[JUDGE_LATE]);
        State::set(IndexNumber::PLAY_2P_COMBOBREAK, _basic.judge[JUDGE_CB]);
        State::set(IndexNumber::PLAY_2P_BPOOR, _basic.judge[JUDGE_KPOOR]);
        State::set(IndexNumber::PLAY_2P_BP, _basic.judge[JUDGE_BP]);

        if (showJudge)
        {
            int fastslow = 0; // 1:fast 2:slow
            switch (_lastNoteJudge[PLAYER_SLOT_TARGET].area)
            {
            case JudgeArea::EARLY_GREAT:
            case JudgeArea::EARLY_GOOD:
            case JudgeArea::EARLY_BAD:
            case JudgeArea::EARLY_KPOOR: fastslow = 1; break;

            case JudgeArea::LATE_GREAT:
            case JudgeArea::LATE_GOOD:
            case JudgeArea::LATE_BAD:
            case JudgeArea::MISS:
            case JudgeArea::LATE_KPOOR: fastslow = 2; break;
            case JudgeArea::NOTHING:
            case JudgeArea::EARLY_PERFECT:
            case JudgeArea::EXACT_PERFECT:
            case JudgeArea::LATE_PERFECT:
            case JudgeArea::MINE_KPOOR: break;
            }
            State::set(IndexNumber::LR2IR_REPLACE_PLAY_2P_FAST_SLOW, fastslow);
            State::set(IndexOption::PLAY_LAST_JUDGE_FASTSLOW_2P, fastslow);
            State::set(IndexNumber::LR2IR_REPLACE_PLAY_2P_JUDGE_TIME_ERROR_MS,
                       _lastNoteJudge[PLAYER_SLOT_TARGET].time.norm());
            State::set(IndexNumber::PLAY_2P_JUDGE_TIME_ERROR_MS, _lastNoteJudge[PLAYER_SLOT_TARGET].time.norm());
        }

        State::set(IndexOption::PLAY_RANK_ESTIMATED_2P, Option::getRankType(_basic.acc));
        State::set(IndexOption::PLAY_RANK_BORDER_2P, Option::getRankType(_basic.total_acc));
        State::set(IndexOption::RESULT_RANK_2P, Option::getRankType(_basic.total_acc));
        State::set(IndexOption::PLAY_HEALTH_2P, Option::getHealthType(_basic.health));

        int maxScore = getMaxScore();
        // if      (dp.total_acc >= 94.44) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * 1.000 -
        // dp.score2));    // MAX-
        if (_basic.total_acc >= 100.0 * 8.0 / 9)
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, exScore - maxScore); // MAX-
        else if (_basic.total_acc >= 100.0 * 7.0 / 9)
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 8.0 / 9)); // AAA-
        else if (_basic.total_acc >= 100.0 * 6.0 / 9)
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 7.0 / 9)); // AA-
        else if (_basic.total_acc >= 100.0 * 5.0 / 9)
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 6.0 / 9)); // A-
        else if (_basic.total_acc >= 100.0 * 4.0 / 9)
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 5.0 / 9)); // B-
        else if (_basic.total_acc >= 100.0 * 3.0 / 9)
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 4.0 / 9)); // C-
        else if (_basic.total_acc >= 100.0 * 2.0 / 9)
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 3.0 / 9)); // D-
        else
            State::set(IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF, int(exScore - maxScore * 2.0 / 9)); // E-

        Option::e_lamp_type lamp = Option::LAMP_NOPLAY;
        if (isNoScore() && _basic.judge[JUDGE_BP] == 0)
        {
            lamp = Option::LAMP_NOPLAY;
        }
        else if (_basic.judge[JUDGE_CB] == 0)
        {
            if (_basic.acc >= 100.0)
                lamp = Option::LAMP_MAX;
            else if (_basic.judge[JUDGE_GOOD] == 0)
                lamp = Option::LAMP_PERFECT;
            else
                lamp = Option::LAMP_FULLCOMBO;
        }
        else if (!isFailed())
        {
            switch (_gauge)
            {
            case GaugeType::HARD: lamp = Option::LAMP_HARD; break;
            case GaugeType::EXHARD: lamp = Option::LAMP_EXHARD; break;
            case GaugeType::DEATH:
                lamp = Option::LAMP_FULLCOMBO;
                break;
                // case GaugeType::P_ATK:      lamp = Option::LAMP_FULLCOMBO; break;
                // case GaugeType::G_ATK:      lamp = Option::LAMP_FULLCOMBO; break;
            case GaugeType::GROOVE: lamp = Option::LAMP_NORMAL; break;
            case GaugeType::EASY: lamp = Option::LAMP_EASY; break;
            case GaugeType::ASSIST: lamp = Option::LAMP_ASSIST; break;
            case GaugeType::GRADE: lamp = Option::LAMP_NOPLAY; break;
            case GaugeType::EXGRADE: lamp = Option::LAMP_NOPLAY; break;
            default: break;
            }
        }
        else
        {
            lamp = Option::LAMP_FAILED;
        }
        State::set(IndexOption::RESULT_CLEAR_TYPE_2P, std::min(lamp, saveLampMax));
    }
}
