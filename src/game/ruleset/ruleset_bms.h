#pragma once

#include "common//chartformat/chartformat_bms.h"
#include "game/scene/scene_context.h"
#include "ruleset.h"

#include <memory>

using lunaticvibes::parser_bms::JudgeDifficulty;

class RulesetBMS : virtual public RulesetBase
{
public:
    enum JudgeIndex
    {
        JUDGE_PERFECT,
        JUDGE_GREAT,
        JUDGE_GOOD,
        JUDGE_BAD,
        JUDGE_POOR,
        JUDGE_KPOOR,
        JUDGE_MISS,
        JUDGE_BP,
        JUDGE_CB,
        JUDGE_EARLY,
        JUDGE_LATE,

        JUDGE_EARLY_POOR,
        JUDGE_EARLY_BAD,
        JUDGE_EARLY_GOOD,
        JUDGE_EARLY_GREAT,
        JUDGE_EARLY_PERFECT,
        JUDGE_EXACT_PERFECT,
        JUDGE_LATE_PERFECT,
        JUDGE_LATE_GREAT,
        JUDGE_LATE_GOOD,
        JUDGE_LATE_BAD,
        JUDGE_LATE_POOR,
        JUDGE_MINE_POOR,
    };

    static constexpr auto LR2_DEFAULT_RANK{JudgeDifficulty::NORMAL};

    enum class JudgeType
    {
        PERFECT = 0, // Option::JUDGE_0
        GREAT,
        GOOD,
        BAD,
        KPOOR,
        MISS,
    };
    inline static const std::map<JudgeType, Option::e_judge_type> JudgeTypeOptMap = {
        {JudgeType::PERFECT, Option::JUDGE_0}, {JudgeType::GREAT, Option::JUDGE_1}, {JudgeType::GOOD, Option::JUDGE_2},
        {JudgeType::BAD, Option::JUDGE_3},     {JudgeType::KPOOR, Option::JUDGE_4}, {JudgeType::MISS, Option::JUDGE_5},
    };

    enum class GaugeType
    {
        GROOVE,
        EASY,
        ASSIST,
        HARD,
        EXHARD,
        DEATH,
        P_ATK,
        G_ATK,
        GRADE,
        EXGRADE,
    };

    // Judge Time definitions.
    // Values are one-way judge times in ms, representing
    // PERFECT, GREAT, GOOD, BAD, ПеPOOR respectively.
    struct JudgeTime
    {
        lunaticvibes::Time PERFECT;
        lunaticvibes::Time GREAT;
        lunaticvibes::Time GOOD;
        lunaticvibes::Time BAD;
        lunaticvibes::Time KPOOR;
    };
    // https://github.com/wcko87/lr2oraja/blob/readme/README.md
    // https://iidx.org/misc/iidx_lr2_beatoraja_diff#timing-window
    inline static const JudgeTime judgeTime[] = {
        {8, 24, 40, 200, 1000},   // VERY HARD
        {15, 30, 60, 200, 1000},  // HARD
        {18, 40, 100, 200, 1000}, // NORMAL
        {21, 60, 120, 200, 1000}, // EASY
        // TODO: VERY EASY? beatoraja uses EASY*1.25 windows, lr2oraja uses NORMAL.
    };

    // Judge area definitions.
    // e.g. EARLY_PERFECT: Perfect early half part
    enum class JudgeArea
    {
        NOTHING = 0,
        EARLY_KPOOR,
        EARLY_BAD,
        EARLY_GOOD,
        EARLY_GREAT,
        EARLY_PERFECT,
        EXACT_PERFECT,
        LATE_PERFECT,
        LATE_GREAT,
        LATE_GOOD,
        LATE_BAD,
        MISS,
        LATE_KPOOR,
        MINE_KPOOR,
    };
    inline static const std::map<JudgeArea, JudgeType> JudgeAreaTypeMap = {
        {JudgeArea::NOTHING, JudgeType::MISS},          {JudgeArea::EARLY_KPOOR, JudgeType::KPOOR},
        {JudgeArea::EARLY_BAD, JudgeType::BAD},         {JudgeArea::EARLY_GOOD, JudgeType::GOOD},
        {JudgeArea::EARLY_GREAT, JudgeType::GREAT},     {JudgeArea::EARLY_PERFECT, JudgeType::PERFECT},
        {JudgeArea::EXACT_PERFECT, JudgeType::PERFECT}, {JudgeArea::LATE_PERFECT, JudgeType::PERFECT},
        {JudgeArea::LATE_GREAT, JudgeType::GREAT},      {JudgeArea::LATE_GOOD, JudgeType::GOOD},
        {JudgeArea::LATE_BAD, JudgeType::BAD},          {JudgeArea::MISS, JudgeType::MISS},
        {JudgeArea::LATE_KPOOR, JudgeType::KPOOR},      {JudgeArea::MINE_KPOOR, JudgeType::KPOOR},
    };
    inline static const std::map<JudgeArea, std::vector<JudgeIndex>> JudgeAreaIndexAccMap = {
        {JudgeArea::NOTHING, {}},
        {JudgeArea::EARLY_KPOOR, {JUDGE_KPOOR, JUDGE_POOR, JUDGE_BP, JUDGE_EARLY}},
        {JudgeArea::EARLY_BAD, {JUDGE_BAD, JUDGE_EARLY_BAD, JUDGE_BP, JUDGE_CB, JUDGE_EARLY}},
        {JudgeArea::EARLY_GOOD, {JUDGE_GOOD, JUDGE_EARLY_GOOD, JUDGE_EARLY}},
        {JudgeArea::EARLY_GREAT, {JUDGE_GREAT, JUDGE_EARLY_GREAT, JUDGE_EARLY}},
        {JudgeArea::EARLY_PERFECT, {JUDGE_PERFECT, JUDGE_EARLY_PERFECT}},
        {JudgeArea::EXACT_PERFECT, {JUDGE_PERFECT, JUDGE_EXACT_PERFECT}},
        {JudgeArea::LATE_PERFECT, {JUDGE_PERFECT, JUDGE_LATE_PERFECT}},
        {JudgeArea::LATE_GREAT, {JUDGE_GREAT, JUDGE_LATE_GREAT, JUDGE_LATE}},
        {JudgeArea::LATE_GOOD, {JUDGE_GOOD, JUDGE_LATE_GOOD, JUDGE_LATE}},
        {JudgeArea::LATE_BAD, {JUDGE_BAD, JUDGE_LATE_BAD, JUDGE_BP, JUDGE_CB, JUDGE_LATE}},
        {JudgeArea::MISS, {JUDGE_MISS, JUDGE_POOR, JUDGE_BP, JUDGE_CB, JUDGE_LATE}},
        {JudgeArea::LATE_KPOOR, {JUDGE_KPOOR, JUDGE_POOR, JUDGE_BP, JUDGE_LATE}},
        {JudgeArea::MINE_KPOOR, {JUDGE_KPOOR, JUDGE_POOR, JUDGE_BP}},
    };

    /// /////////////////////////////////////////////////////////////////////

    typedef std::map<chart::NoteLaneIndex, IndexTimer> NoteLaneTimerMap;

    enum class PlaySide
    {
        SINGLE,
        DOUBLE,
        BATTLE_1P,
        BATTLE_2P,
        AUTO,
        AUTO_2P,
        AUTO_DOUBLE,
        RIVAL,
        MYBEST,
        NETWORK,
    };

    struct JudgeRes
    {
        JudgeArea area{JudgeArea::NOTHING};
        lunaticvibes::Time time{0};
    };

protected:
    // members set on construction
    PlaySide _side = PlaySide::SINGLE;
    bool _k1P = false, _k2P = false;
    JudgeDifficulty _judgeDifficulty = LR2_DEFAULT_RANK;
    GaugeType _gauge = GaugeType::GROOVE;
    std::shared_ptr<PlayContextParams::MutexReplayChart> _replayNew;

    std::map<JudgeType, double> _healthGain;

    bool _judgeScratch = true;

    bool showJudge = true;
    const NoteLaneTimerMap* _bombTimerMap = nullptr;
    const NoteLaneTimerMap* _bombLNTimerMap = nullptr;

    int total = -1;
    unsigned noteCount = 0;

    std::string modifierText, modifierTextShort;
    Option::e_lamp_type saveLampMax;

protected:
    // members change in game
    std::array<JudgeArea, chart::NOTELANEINDEX_COUNT> _lnJudge{JudgeArea::NOTHING};
    std::array<JudgeRes, 2> _lastNoteJudge{};

    std::map<chart::NoteLane, ChartObjectBase::NoteIterator> _noteListIterators;

    std::array<AxisDir, 2> playerScratchDirection = {0, 0};
    std::array<lunaticvibes::Time, 2> playerScratchLastUpdate = {TIMER_NEVER, TIMER_NEVER};
    std::array<double, 2> playerScratchAccumulator = {0, 0};

protected:
    // members essential for score calculation
    double moneyScore = 0.0;
    double maxMoneyScore = 200000.0;
    unsigned exScore = 0;

public:
    // fiveKeyMapIndex - if not 5k, set to -1.
    RulesetBMS(std::shared_ptr<ChartFormatBase> format, std::shared_ptr<ChartObjectBase> chart, PlayModifiers mods,
               GameModeKeys keys, JudgeDifficulty difficulty, double health, PlaySide side, int fiveKeyMapIndex,
               std::shared_ptr<PlayContextParams::MutexReplayChart> replayNew);

    void initGaugeParams(PlayModifierGaugeType gauge);

protected:
    JudgeRes _calcJudgeByTimes(const Note& note, const lunaticvibes::Time& time) const;

private:
    void _judgePress(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, const JudgeRes& judge,
                     const lunaticvibes::Time& t, int slot);
    void _judgeHold(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, const JudgeRes& judge,
                    const lunaticvibes::Time& t, int slot);
    void _judgeRelease(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, const JudgeRes& judge,
                       const lunaticvibes::Time& t, int slot);
    void judgeNotePress(Input::Pad k, const lunaticvibes::Time& t, const lunaticvibes::Time& rt, int slot);
    void judgeNoteHold(Input::Pad k, const lunaticvibes::Time& t, const lunaticvibes::Time& rt, int slot);
    void judgeNoteRelease(Input::Pad k, const lunaticvibes::Time& t, const lunaticvibes::Time& rt, int slot);
    void _updateHp(double diff);
    void _updateHp(JudgeArea judge);

public:
    // Register to InputWrapper
    void updatePress(InputMask& pg, const lunaticvibes::Time& t) override;
    // Register to InputWrapper
    void updateHold(InputMask& hg, const lunaticvibes::Time& t) override;
    // Register to InputWrapper
    void updateRelease(InputMask& rg, const lunaticvibes::Time& t) override;
    // Register to InputWrapper
    void updateAxis(double s1, double s2, const lunaticvibes::Time& t) override;
    // Called by ScenePlay
    void update(const lunaticvibes::Time& t) override;

public:
    void updateJudge(const lunaticvibes::Time& t, chart::NoteLaneIndex ch, JudgeArea judge, int slot);

public:
    GaugeType getGaugeType() const { return _gauge; }

    double getScore() const;
    double getMaxMoneyScore() const;
    unsigned getExScore() const;
    unsigned getJudgeCount(JudgeType idx) const;
    unsigned getJudgeCountEx(JudgeIndex idx) const;
    std::string getModifierText() const;
    std::string getModifierTextShort() const;

    bool isNoScore() const override { return moneyScore == 0.0; }
    bool isCleared() const override { return !isFailed() && isFinished() && _basic.health >= getClearHealth(); }
    bool isFailed() const override { return _isFailed; }

    unsigned getCurrentMaxScore() const override { return notesReached * 2; }
    unsigned getMaxScore() const override { return getNoteCount() * 2; }

    unsigned getNoteCount() const override;
    unsigned getMaxCombo() const override;

    void fail() override;

    void updateGlobals() override;
};