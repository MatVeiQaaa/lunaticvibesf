#pragma once
#include <string>
#include <array>
#include <vector>
#include <map>
#include "common/types.h"
namespace i18nText
{
	enum i18nTextIndex
	{
		LANGUAGE_NAME,
		OFF,
		ON,
		OK,
		CANCEL,
		APPLY,
		ADD_MORE,
		MAIN_KEYCONFIG,
		MAIN_SETTINGS,
		MAIN_ABOUT,
		MAIN_EXIT,
		TODO,
		SETTINGS_GENERAL,
		SETTINGS_JUKEBOX,
		SETTINGS_VIDEO,
		SETTINGS_AUDIO,
		SETTINGS_PLAY,
		SETTINGS_SELECT,
		PROFILE,
		PLAYER_NAME,
		LANGUAGE,
		JUKEBOX_REFRESH_HINT,
		JUKEBOX_FOLDER,
		JUKEBOX_DELETE_SELECTED,
		JUKEBOX_BROWSE_SELECTED,
		JUKEBOX_TABLES,
		VIDEO_RESOLUTION,
		VIDEO_SS_LEVEL,
		VIDEO_SS_LEVEL_HINT,
		VIDEO_SCREEN_MODE,
		VIDEO_WINDOWED,
		VIDEO_FULLSCREEN,
		VIDEO_BORDERLESS,
		VIDEO_VSYNC,
		VIDEO_ADAPTIVE,
		VIDEO_MAXFPS,
		AUDIO_DEVICE,
		AUDIO_REFRESH_DEVICE_LIST,
		AUDIO_BUFFER_COUNT,
		AUDIO_BUFFER_LENGTH,
		MISS_BGA_TIME,
		MIN_INPUT_INTERVAL,
		MIN_INPUT_INTERVAL_HINT,
		SCROLL_SPEED,
		SCROLL_SPEED_HINT,
		NEW_SONG_DURATION,
		NEW_SONG_DURATION_HINT,
		NEW_PROFILE,
		NEW_PROFILE_NAME,
		NEW_PROFILE_NAME_HINT,
		NEW_PROFILE_COPY_FROM_CURRENT,
		NEW_PROFILE_EMPTY,
		NEW_PROFILE_DUPLICATE,
		INITIALIZING,
		CHECKING_FOLDERS,
		LOADING_CHARTS,
		CHECKING_TABLES,
		LOADING_TABLE,
		DOWNLOADING_TABLE,
		LOADING_COURSES,
		COURSE_TITLE,
		COURSE_SUBTITLE,
		CLASS_TITLE,
		CLASS_SUBTITLE,
		BMS_NOT_FOUND,
		BMS_NOT_FOUND_HINT,
		PLEASE_WAIT,
		LOADING_SKIN_OPTIONS,
		LOADING_SKIN_OPTIONS_FINISHED,
		REFRESH_FOLDER,
		REFRESH_FOLDER_DETAIL,
		SEARCH_SONG,
		SEARCH_FAILED,
		SEARCH_RESULT,
		CHART_NOT_FOUND,
		CHART_NOT_FOUND_MD5,
		CUSTOM_FOLDER_DESCRIPTION,
		COURSE_FOLDER_DESCRIPTION,
		PREVIEW_DEDICATED,
		PREVIEW_DEDICATED_HINT,
		PREVIEW_DIRECT,
		PREVIEW_DIRECT_HINT,
		SELECT_KEYBINDINGS,
		SELECT_KEYBINDINGS_HINT,
		ENABLE_NEW_RANDOM_OPTIONS,
		ENABLE_NEW_RANDOM_OPTIONS_HINT,
		ENABLE_NEW_GAUGE_OPTIONS,
		ENABLE_NEW_GAUGE_OPTIONS_HINT,
		ENABLE_NEW_LANE_OPTIONS,
		ENABLE_NEW_LANE_OPTIONS_HINT,
		DEFAULT_TARGET,
		JUDGE_TIMING,
		JUDGE_TIMING_HINT,
		LOCK_GREENNUMBER,
		GREENNUMBER,
		GREENNUMBER_HINT,
		HISPEED,
		ONLY_DISPLAY_MAIN_TITLE_ON_BARS,
		DISABLE_PLAYMODE_ALL,
		DISABLE_DIFFICULTY_ALL,
			DISABLE_PLAYMODE_SINGLE,
			DISABLE_PLAYMODE_DOUBLE,
			IGNORE_DP_CHARTS,
			IGNORE_9K_CHARTS,
			IGNORE_5K_IF_7K_EXISTS,
			KEYCONFIG_HINT_KEY,
			KEYCONFIG_HINT_BIND,
			KEYCONFIG_HINT_DEADZONE,
			KEYCONFIG_HINT_F1,
			KEYCONFIG_HINT_F2,
			KEYCONFIG_HINT_DEL,
			KEYCONFIG_HINT_SCRATCH_ABS,
			INPUT_POLLING_RATE,
			JUKEBOX_FOLDER_PATH_HINT,
			JUKEBOX_TABLE_URL_HINT,
			INPUT_POLLING_RATE_WARNING_WINDOWS,
			PLAY_SKIN_ADJUST_JUDGE_POS_LIFT,
			PLAY_SKIN_ADJUST_JUDGE_POS_1P_X,
			PLAY_SKIN_ADJUST_JUDGE_POS_1P_Y,
			PLAY_SKIN_ADJUST_JUDGE_POS_2P_X,
			PLAY_SKIN_ADJUST_JUDGE_POS_2P_Y,
			PLAY_SKIN_ADJUST_NOTE_1P_X,
			PLAY_SKIN_ADJUST_NOTE_1P_Y,
			PLAY_SKIN_ADJUST_NOTE_1P_W,
			PLAY_SKIN_ADJUST_NOTE_1P_H,
			PLAY_SKIN_ADJUST_NOTE_2P_X,
			PLAY_SKIN_ADJUST_NOTE_2P_Y,
			PLAY_SKIN_ADJUST_NOTE_2P_W,
			PLAY_SKIN_ADJUST_NOTE_2P_H,
			PLAY_SKIN_ADJUST_RESET,

		TEXT_COUNT
	};
}
constexpr size_t i18n_TEXT_COUNT = (size_t)i18nText::TEXT_COUNT;

class i18n
{
private:
	i18n(const Path& translationFile);
public:
	~i18n() = default;

private:
	Languages type = Languages::EN;
	std::array<std::string, i18n_TEXT_COUNT> text;

private:
	static std::vector<i18n> languages;
	static size_t currentLanguage;

public:
	static void init();

	static std::vector<std::string> getLanguageList();

	static void setLanguage(size_t index);	// get index from getLanguageList()
	static void setLanguage(const std::string& name);

	static const std::string& s(size_t index);	// i18nText::i18nTextIndex
	static const char* c(size_t index);	// i18nText::i18nTextIndex

	static Languages getCurrentLanguage();
};