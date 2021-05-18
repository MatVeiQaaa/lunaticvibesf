#pragma once
#include "buffered_global.h"

enum class eText: unsigned
{
    INVALID = 0,           // should be initialized with ""

    TARGET_NAME = 1,
    PLAYER_NAME,

    PLAY_TITLE = 10,
    PLAY_SUBTITLE,
    PLAY_FULLTITLE,
    PLAY_GENRE,
    PLAY_ARTIST,
    PLAY_SUBARTIST,
    PLAY_TAG,
    PLAY_PLAYLEVEL,
    PLAY_DIFFICULTY,
    
    EDIT_TITLE = 20,
    EDIT_SUBTITLE,
    EDIT_FULLTITLE,
    EDIT_GENRE,
    EDIT_ARTIST,
    EDIT_SUBARTIST,
    EDIT_TAG,
    EDIT_PLAYLEVEL,
    EDIT_DIFFICULTY,
    EDIT_INSANE_LEVEL,
    EDIT_JUKEBOX_NAME,

    KEYCONFIG_SLOT1 = 40,
    KEYCONFIG_SLOT2,
    KEYCONFIG_SLOT3,
    KEYCONFIG_SLOT4,
    KEYCONFIG_SLOT5,
    KEYCONFIG_SLOT6,
    KEYCONFIG_SLOT7,
    KEYCONFIG_SLOT8,        // not defined
    KEYCONFIG_SLOT9,        // not defined

    SKIN_NAME = 50,
    SKIN_MAKER_NAME,

    PLAY_MODE = 60,         // #MODE, ALL KEYS, SINGLE, 7KEYS, 5KEYS, DOUBLE, 14KEYS, 10KEYS, 9KEYS, 
    SORT_MODE,              // #SORT, OFF, TITLE, LEVEL, FOLDER, CLEAR, RANK, 
    DIFFICULTY,             // #DIFFICULTY, ALL, EASY, STANDARD, HARD, EXPERT, ULTIMATE,

    RANDOM_1P,
    RANDOM_2P,
    GAUGE_1P,
    GAUGE_2P,
    ASSIST_1P,
    ASSIST_2P,
    BATTLE,
    FLIP,
    SCORE_GRAPH,
    GHOST,
    SHUTTER,
    SCROLL_TYPE,
    BGA_SIZE,
    BGA,
    COLOR_DEPTH,
    VSYNC,
    WINDOWMODE,             // Fullscreen, Windowed
    JUDGE_AUTO,
    REPLAY_SAVE_TYPE,
    TRIAL1,
    TRIAL2,
    EFFECT_1P,
    EFFECT_2P,

    �����󥫥����ޥ������ƥ�����1��Ŀ = 100,
    �����󥫥����ޥ������ƥ�����2��Ŀ = 101,
    �����󥫥����ޥ������ƥ�����3��Ŀ = 102,
    �����󥫥����ޥ������ƥ�����4��Ŀ = 103,
    �����󥫥����ޥ������ƥ�����5��Ŀ = 104,
    �����󥫥����ޥ������ƥ�����6��Ŀ = 105,
    �����󥫥����ޥ������ƥ�����7��Ŀ = 106,
    �����󥫥����ޥ������ƥ�����8��Ŀ = 107,
    �����󥫥����ޥ������ƥ�����9��Ŀ = 108,
    �����󥫥����ޥ������ƥ�����10��Ŀ = 109,

    �����󥫥����ޥ����Ŀ��1��Ŀ = 110,
    �����󥫥����ޥ����Ŀ��2��Ŀ = 111,
    �����󥫥����ޥ����Ŀ��3��Ŀ = 112,
    �����󥫥����ޥ����Ŀ��4��Ŀ = 113,
    �����󥫥����ޥ����Ŀ��5��Ŀ = 114,
    �����󥫥����ޥ����Ŀ��6��Ŀ = 115,
    �����󥫥����ޥ����Ŀ��7��Ŀ = 116,
    �����󥫥����ޥ����Ŀ��8��Ŀ = 117,
    �����󥫥����ޥ����Ŀ��9��Ŀ = 118,
    �����󥫥����ޥ����Ŀ��10��Ŀ = 119,

    ��󥭥󥰱�ץ쥤��`��1��Ŀ = 120,
    ��󥭥󥰱�ץ쥤��`��2��Ŀ = 121,
    ��󥭥󥰱�ץ쥤��`��3��Ŀ = 122,
    ��󥭥󥰱�ץ쥤��`��4��Ŀ = 123,
    ��󥭥󥰱�ץ쥤��`��5��Ŀ = 124,
    ��󥭥󥰱�ץ쥤��`��6��Ŀ = 125,
    ��󥭥󥰱�ץ쥤��`��7��Ŀ = 126,
    ��󥭥󥰱�ץ쥤��`��8��Ŀ = 127,
    ��󥭥󥰱�ץ쥤��`��9��Ŀ = 128,
    ��󥭥󥰱�ץ쥤��`��10��Ŀ = 129,

    ���ꥢ�����У�����1 = 130,
    ���ꥢ�����У�����2 = 131,

    ���`���������ȥ�1stage = 150,
    ���`���������ȥ�2stage = 151,
    ���`���������ȥ�3stage = 152,
    ���`���������ȥ�4stage = 153,
    ���`���������ȥ�5stage = 154,
    ���`���������ȥ�6stage = 155,
    ���`���������ȥ�7stage = 156,
    ���`���������ȥ�8stage = 157,
    ���`���������ȥ�9stage = 158,
    ���`���������ȥ�10stage = 159,

    ���`�������֥����ȥ�1stage = 160,
    ���`�������֥����ȥ�2stage = 161,
    ���`�������֥����ȥ�3stage = 162,
    ���`�������֥����ȥ�4stage = 163,
    ���`�������֥����ȥ�5stage = 164,
    ���`�������֥����ȥ�6stage = 165,
    ���`�������֥����ȥ�7stage = 166,
    ���`�������֥����ȥ�8stage = 167,
    ���`�������֥����ȥ�9stage = 168,
    ���`�������֥����ȥ�10stage = 169,

    ���`�����ץ���󿎤���`��stage1��2 = 171,
    ���`�����ץ���󿎤���`��stage2��3 = 172,
    ���`�����ץ���󿎤���`��stage3��4 = 173,
    ���`�����ץ���󿎤���`��stage4��5 = 174,
    ���`�����ץ���󿎤���`��stage5��6 = 175,
    ���`�����ץ���󿎤���`��stage6��7 = 176,
    ���`�����ץ���󿎤���`��stage7��8 = 177,
    ���`�����ץ���󿎤���`��stage8��9 = 178,
    ���`�����ץ���󿎤���`��stage9��10 = 179,



    ���`�����ץ���󥽥ե�� = 180,
    ���`�����ץ���󥲩`�������� = 181,
    ���`�����ץ���󥪥ץ�����ֹ = 182,
    ���`�����ץ����IR = 183,


    �����ॳ�`�����ץ�������m��٥� = 190,

    �����ॳ�`�����ץ�����٥����� = 191,

    �����ॳ�`�����ץ�����٥����� = 192,

    �����ॳ�`�����ץ����bpm��ӷ� = 193,

    �����ॳ�`�����ץ����bpm���� = 194,

    �����ॳ�`�����ץ����bpm���� = 195,

    �����ॳ�`�����ץ���󥹥Ʃ`���� = 196,

    ȫ�女�`�����ץ����ǥե���ȤĤʤ������� = 198,

    ȫ�女�`�����ץ����ǥե���ȥ��`�� = 199,

	// inner texts
    _SELECT_BAR_TITLE_FULL_0 = 200,
    _SELECT_BAR_TITLE_FULL_MAX = 231,

	_TEST1 = 250,
	_TEST2,


    TEXT_COUNT
};

enum TextAlign
{
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

inline buffered_global<eText, std::string, (size_t)eText::TEXT_COUNT> gTexts;