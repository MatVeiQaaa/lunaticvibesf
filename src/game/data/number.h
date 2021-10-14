#pragma once
//#include <array>
#include "buffered_global.h"

enum class eNumber: unsigned
{
    ZERO = 0,           // should be initialized with 0

	HS_1P = 10,
	HS_2P,

	TIMING_ADJUST_VISUAL = 12,
	DEFAULT_TARGET_RATE,

	LANECOVER_1P = 14,
	LANECOVER_2P,

    FPS = 20,
	DATE_YEAR,
	DATE_MON,
	DATE_DAY,
	DATE_HOUR,
	DATE_MIN,
	DATE_SEC,

	PROFILE_PLAY_COUNT = 30,
	PROFILE_CLEAR_COUNT,
	PROFILE_FAIL_COUNT,
	PROFILE_PERFECT,
	PROFILE_GREAT,
	PROFILE_GOOD,
	PROFILE_BAD,
	PROFILE_POOR,
	PROFILE_RUNNING_COMBO,
	PROFILE_RUNNING_COMBO_MAX,
	PROFILE_TRIAL_LEVEL,
	PROFILE_TRIAL_LEVEL_PREV,

	MUSIC_BEGINNER_LEVEL = 45,
	MUSIC_NORMAL_LEVEL,
	MUSIC_HYPER_LEVEL,
	MUSIC_ANOTHER_LEVEL,
	MUSIC_INSANE_LEVEL,

	EQ0 = 50,
	EQ1,
	EQ2,
	EQ3,
	EQ4,
	EQ5,
	EQ6,
	VOLUME_MASTER,
	VOLUME_KEY,
	VOLUME_BGM,

	FX0_P1 = 60,
	FX0_P2,
	FX1_P1,
	FX1_P2,
	FX2_P1,
	FX2_P2,
	PITCH,

	INFO_SCORE = 70,
	INFO_EXSCORE,
	INFO_EXSCORE_MAX,
	INFO_RATE,
	INFO_TOTALNOTE,
	INFO_MAXCOMBO,
	INFO_BP,
	INFO_PLAYCOUNT,
	INFO_CLEAR,
	INFO_FAIL,
	INFO_PERFECT_COUNT,
	INFO_GREAT_COUNT,
	INFO_GOOD_COUNT,
	INFO_BAD_COUNT,
	INFO_POOR_COUNT,
	INFO_PERFECT_RATE,
	INFO_GREAT_RATE,
	INFO_GOOD_RATE,
	INFO_BAD_RATE,
	INFO_POOR_RATE,
	INFO_BPM_MAX,
	INFO_BPM_MIN,
	INFO_LR2IR_RANK,
	INFO_LR2IR_TOTALPLAYER,
	INFO_LR2IR_CLEARRATE,
	INFO_LR2IR_SCORE_TO_RIVAL,

	PLAY_1P_SCORE = 100,
	PLAY_1P_EXSCORE,
	PLAY_1P_RATE,
	PLAY_1P_RATEDECIMAL,
	PLAY_1P_NOWCOMBO,
	PLAY_1P_MAXCOMBO,
    PLAY_1P_TOTALNOTES,
    PLAY_1P_GROOVEGAUGE,
    PLAY_1P_EXSCORE_DELTA,

    PLAY_1P_PERFECT = 110,
    PLAY_1P_GREAT,
    PLAY_1P_GOOD,
    PLAY_1P_BAD,
    PLAY_1P_POOR,
    PLAY_1P_TOTAL_RATE,
    PLAY_1P_TOTAL_RATE_DECIMAL2,

	PLAY_2P_SCORE = 120,
	PLAY_2P_EXSCORE,
	PLAY_2P_RATE,
	PLAY_2P_RATEDECIMAL,
	PLAY_2P_NOWCOMBO,
	PLAY_2P_MAXCOMBO,
    PLAY_2P_TOTALNOTES,
    PLAY_2P_GROOVEGAUGE,
    PLAY_2P_EXSCORE_DELTA,

    PLAY_2P_PERFECT = 130,
    PLAY_2P_GREAT,
    PLAY_2P_GOOD,
    PLAY_2P_BAD,
    PLAY_2P_POOR,
    PLAY_2P_TOTAL_RATE,
    PLAY_2P_TOTAL_RATE_DECIMAL2,

    PLAY_BPM = 160,
    PLAY_MIN,
    PLAY_SEC,
    PLAY_REMAIN_MIN,
    PLAY_REMAIN_SEC,
    PLAY_LOAD_PROGRESS_PERCENT,

    PLAY_LOAD_PROGRESS_SYS = 166,
    PLAY_LOAD_PROGRESS_WAV,
    PLAY_LOAD_PROGRESS_BGA,

    RESULT_MYBEST_EX_BEFORE = 170,
    RESULT_MYBEST_EX_NOW,
    RESULT_MYBEST_EX_DIFF,
    RESULT_MYBEST_MAXCOMBO_BEFORE,
    RESULT_MYBEST_MAXCOMBO_NOW,
    RESULT_MYBEST_MAXCOMBO_DIFF,
    RESULT_MYBEST_BP_BEFORE,
    RESULT_MYBEST_BP_NOW,
    RESULT_MYBEST_BP_DIFF,
    RESULT_MYBEST_IRRANK_BEFORE,
    RESULT_MYBEST_IRRANK_NOW,
    RESULT_MYBEST_IRRANK_DIFF,
    RESULT_IR_RANK,
    RESULT_IR_TOTALPLAYER,
    RESULT_IR_CLEARRATE,
    RESULT_IR_RANK_BEFORE,
    RESULT_MYBEST_RATE,
    RESULT_MYBEST_RATE_DECIMAL2,

    BPM_MAX = 290,
    BPM_MIN,

    RANDOM,

	// Inner numbers
	_TEST1 = 300,
	_TEST2,
	_TEST3,
	_TEST4,
	_TEST5,
	_TEST6,
	_TEST7,
	_TEST8,
	_TEST9,

	_PPS1 = 311,
	_PPS2,
	_PPS3,
	_PPS4,
	_PPS5,
	_PPS6,
	_PPS7,
	_PPS8,
	_PPS9,

	_DISP_NOWCOMBO_1P,
	_DISP_NOWCOMBO_2P,
    _ANGLE_TT_1P,
    _ANGLE_TT_2P,

    _SELECT_BAR_LEVEL_0 = 400,
	_SELECT_BAR_LEVEL_MAX = 431,

	SCENE_UPDATE_FPS = 1000,
	INPUT_DETECT_FPS,
	NEW_ENTRY_SECONDS,

    NUMBER_COUNT
};

inline buffered_global<eNumber, int, (size_t)eNumber::NUMBER_COUNT> gNumbers;
/*
class gNumbers
{
protected:
    constexpr gNumbers() : _data{ 0 } {}
private:
    static gNumbers _inst;
    std::array<int, (size_t)eNumber::NUMBER_COUNT> _data;
public:
    static constexpr int get(eNumber n) { return _inst._data[(size_t)n]; }
    static constexpr void set(eNumber n, int value) { _inst._data[(size_t)n] = value; }
};
*/
