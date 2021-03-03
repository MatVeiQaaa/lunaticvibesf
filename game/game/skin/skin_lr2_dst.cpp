#include "skin_lr2.h"
#include "game/data/number.h"

std::bitset<900> _op;
std::bitset<100> _customOp;
inline void set(int idx, bool val = true) { _op.set(idx, val); }
inline void set(std::initializer_list<int> idx, bool val = true)
{
	for (auto& i : idx)
		_op.set(i, val);
}
inline bool get(int idx) { return _op[idx]; }

constexpr bool dst(eOption option_entry, std::initializer_list<unsigned> entries)
{
    auto op = gOptions.get(option_entry);
    for (auto e : entries)
        if (op == e) return true;
    return false;
}
constexpr bool dst(eOption option_entry, unsigned entry)
{
	return gOptions.get(option_entry) == entry;
}

constexpr bool sw(std::initializer_list<eSwitch> entries)
{
    for (auto e : entries)
        if (gSwitches.get(e)) return true;
    return false;
}
constexpr bool sw(eSwitch entry)
{
	return gSwitches.get(entry);
}

bool getDstOpt(int d)
{
	bool result = false;
	dst_option op = (dst_option)std::abs(d);

	if (d == DST_TRUE)
		result = true;
	else if (d == DST_FALSE)
		result = false;
	else if (op < 900)
		result = _op[op];
	else if (op >= 900) 
	{
		if (op > 999)
			result = false;
		else
			result = _customOp[op - 900];
	}
	return (d >= 0) ? result : !result;
}

void setCustomDstOpt(unsigned base, size_t offset, bool val)
{
    if (base + offset < 900 || base + offset > 999) return;
    _customOp[base + offset - 900] = val;
}

void clearCustomDstOpt()
{
	_customOp.reset();
}

void updateDstOpt()
{
	_op.reset();

	// 0 ����true
	set(0);
	// 1 �x�k�ХЩ`���ե����
	// 2 �x�k�ХЩ`����
	// 3 �x�k�ХЩ`�����`��
	// 4 �x�k�ХЩ`����Ҏ���`������
	// 5 �x�k�ХЩ`���ץ쥤����(�������`���Ȥʤ�true
	{
		switch (gOptions.get(eOption::SELECT_ENTRY_TYPE))
		{
		using namespace Option;
		case ENTRY_FOLDER: set({ 1 }); break;
		case ENTRY_SONG: set({ 2, 5 }); break;
		case ENTRY_COURSE: set({ 3, 5 }); break;
		case ENTRY_NEW_COURSE: set(4); break;
		}
	}

	// 10 ���֥�or���֥�Хȥ�ʤ�true
	// 11 �Хȥ�ʤ�true
	// 12 ���֥�or�Хȥ�or���֥�Хȥ�ʤ�true
	// 13 ���`���ȥХȥ�or�Хȥ�ʤ�true
	{
		auto m = gOptions.get(eOption::PLAY_MODE);
		enum { SP, DP, SB, DB, GB } mode = decltype(mode)(m);
		if (m == PLAY_BATTLE) mode = SB;
		switch (mode)
		{
		case SP: break;
		case DP: set({ 10, 12 }); break;
		case SB: set({ 11, 12, 13 }); break;
		case DB: set({ 10, 12 }); break;
		case GB: set({ 13 }); break;
		}
	}

	// 20 �ѥͥ����Ӥ��Ƥ��ʤ�
	// 21 �ѥͥ�1���ӕr
	{
		set(20, sw({
			eSwitch::SELECT_PANEL1,
			eSwitch::SELECT_PANEL2,
			eSwitch::SELECT_PANEL3,
			eSwitch::SELECT_PANEL4,
			eSwitch::SELECT_PANEL5,
			eSwitch::SELECT_PANEL6,
			eSwitch::SELECT_PANEL7,
			eSwitch::SELECT_PANEL8,
			eSwitch::SELECT_PANEL9,
			}));
		for (unsigned i = 21; i <= 29; ++i)
			set(i, sw((eSwitch)(i - 21 + (unsigned)eSwitch::SELECT_PANEL1)));
	}

	// 30 BGA normal
	// 31 BGA extend
	// 40 BGA off
	// 41 BGA on
	switch (gOptions.get(eOption::PLAY_BGA_TYPE))
	{
	using namespace Option;
	case BGA_OFF: set(40); break;
	case BGA_NORMAL: set({ 30, 41 }); break;
	case BGA_EXTEND: set({ 31, 41 }); break;
	}

	// 32 autoplay off
	// 33 autoplay on
	set(32, true);
	set(33, false);

	// 34 ghost off
	// 35 ghost typeA
	// 36 ghost typeB
	// 37 ghost typeC
	switch (gOptions.get(eOption::PLAY_GHOST_TYPE_1P))
	{
	using namespace Option;
	case GHOST_OFF: set(34); break;
	case GHOST_TOP: set(35); break;
	case GHOST_SIDE: set(36); break;
	case GHOST_SIDE_BOTTOM: set(37); break;
	}

	// 38 scoregraph off
	// 39 scoregraph on
	set(38, !sw(eSwitch::SYSTEM_SCOREGRAPH));
	set(39, sw(eSwitch::SYSTEM_SCOREGRAPH));

	// 42 1P�Ȥ��Ω`�ޥ륲�`��
	// 43 1P�Ȥ��ॲ�`��
	// 44 2P�Ȥ��Ω`�ޥ륲�`��
	// 45 2P�Ȥ��ॲ�`��
	{
		using namespace Option;
		set(42, dst(eOption::PLAY_GAUGE_TYPE_1P, { GAUGE_ASSIST, GAUGE_EASY, GAUGE_NORMAL }));
		set(43, dst(eOption::PLAY_GAUGE_TYPE_1P, { GAUGE_HARD, GAUGE_EXHARD, GAUGE_DEATH }));
		set(44, dst(eOption::PLAY_GAUGE_TYPE_2P, { GAUGE_ASSIST, GAUGE_EASY, GAUGE_NORMAL }));
		set(45, dst(eOption::PLAY_GAUGE_TYPE_2P, { GAUGE_HARD, GAUGE_EXHARD, GAUGE_DEATH }));
	}

	// 46 �y�׶ȥե��륿���Є�
	// 47 �y�׶ȥե��륿���o��
	{
		using namespace Option;
		set(46);
	}

	// 50 ���ե饤��
	// 51 ����饤��
	set(50, !sw(eSwitch::NETWORK));
	set(51, sw(eSwitch::NETWORK));

	// 52 EXTRA MODE OFF
	// 53 EXTRA MODE ON
	set(52, !sw(eSwitch::PLAY_OPTION_EXTRA));
	set(53, sw(eSwitch::PLAY_OPTION_EXTRA));

	// 54 AUTOSCRATCH 1P OFF
	// 55 AUTOSCRATCH 1P ON
	// 56 AUTOSCRATCH 2P OFF
	// 57 AUTOSCRATCH 2P ON
	set(54, !sw(eSwitch::PLAY_OPTION_AUTOSCR_1P));
	set(55, sw(eSwitch::PLAY_OPTION_AUTOSCR_1P));
	set(56, !sw(eSwitch::PLAY_OPTION_AUTOSCR_2P));
	set(57, sw(eSwitch::PLAY_OPTION_AUTOSCR_2P));

	// 60 ���������`�ֲ�����
	// 61 ���������`�ֿ���
	set(60, !sw(eSwitch::CHART_CAN_SAVE_SCORE));
	set(61, sw(eSwitch::CHART_CAN_SAVE_SCORE));

	// 62 ���ꥢ���`�ֲ�����
	// 63 EASY���`�� ���˘����Ǥϡ����`���`�ǥ��`�֡���
	// 64 NORMAL���`�� ���˘����Ǥϡ��Ω`�ޥ�ǥ��`�֡���
	// 65 HARD/G-ATTACK���`�� ���˘����Ǥϡ��ϩ`�ɤǥ��`�֡���
	// 66 DEATH/P-ATTACK���`�� ���˘����Ǥϡ��ե륳��Τߡ���
	{
		using namespace Option;
		set(62, dst(eOption::CHART_CAN_SAVE_LAMP, { LAMP_NOPLAY, LAMP_FAILED }));
		set(63, dst(eOption::CHART_CAN_SAVE_LAMP, { LAMP_ASSIST, LAMP_EASY }));
		set(64, dst(eOption::CHART_CAN_SAVE_LAMP, LAMP_NORMAL));
		set(65, dst(eOption::CHART_CAN_SAVE_LAMP, { LAMP_HARD, LAMP_EXHARD }));
		set(66, dst(eOption::CHART_CAN_SAVE_LAMP, { LAMP_FULLCOMBO, LAMP_PERFECT, LAMP_MAX }));
	}

	// 70 ͬ�ե����beginner�Υ�٥뤬Ҏ������Խ���Ƥ��ʤ�(5/10keys��LV9��7/14keys��LV12��9keys��LV42����)
	// 75 ͬ�ե����beginner�Υ�٥뤬Ҏ������Խ���Ƥ���
	{
		int ceiling = 12;
		switch (gOptions.get(eOption::CHART_PLAY_KEYS))
		{
			using namespace Option;
		case KEYS_7:
		case KEYS_14:
			ceiling = 12; break;
		case KEYS_5:
		case KEYS_10:
			ceiling = 9; break;
		case KEYS_9:
			ceiling = 42; break;
		case KEYS_24:
		case KEYS_48:
			ceiling = 90; break;
		}
		set(70, gNumbers.get(eNumber::MUSIC_BEGINNER_LEVEL) <= ceiling);
		set(71, gNumbers.get(eNumber::MUSIC_NORMAL_LEVEL) <= ceiling);
		set(72, gNumbers.get(eNumber::MUSIC_HYPER_LEVEL) <= ceiling);
		set(73, gNumbers.get(eNumber::MUSIC_ANOTHER_LEVEL) <= ceiling);
		set(74, gNumbers.get(eNumber::MUSIC_INSANE_LEVEL) <= ceiling);
		set(75, gNumbers.get(eNumber::MUSIC_BEGINNER_LEVEL) > ceiling);
		set(76, gNumbers.get(eNumber::MUSIC_NORMAL_LEVEL) > ceiling);
		set(77, gNumbers.get(eNumber::MUSIC_HYPER_LEVEL) > ceiling);
		set(78, gNumbers.get(eNumber::MUSIC_ANOTHER_LEVEL) > ceiling);
		set(79, gNumbers.get(eNumber::MUSIC_INSANE_LEVEL) > ceiling);
	}


	// 80 ��`��δ����
	// 81 ��`������
	{
		using namespace Option;
		set(80, dst(eOption::PLAY_SCENE_STAT, { SPLAY_PREPARE, SPLAY_LOADING }));
		set(81, !_op[80]);
	}

	// 82 ��ץ쥤����
	// 83 ��ץ쥤�h����
	// 84 ��ץ쥤������

	// 90 �ꥶ ���ꥢ
	// 91 �ꥶ �ߥ�
	set(90, !sw(eSwitch::RESULT_CLEAR));
	set(91, sw(eSwitch::RESULT_CLEAR));


	// /////////////////////////////////
	// //�x���ꥹ����
	// 100 NOT PLAYED
	// 101 FAILED
	// 102 EASY CLEARED
	// 103 NORMAL CLEARED
	// 104 HARD CLEARED
	// 105 FULL COMBO
	{
		using namespace Option;
		switch (gOptions.get(eOption::SELECT_ENTRY_LAMP))
		{
		case LAMP_NOT_APPLICIABLE: break;
		case LAMP_NOPLAY: set(100); break;
		case LAMP_FAILED: set(101); break;
		case LAMP_ASSIST:
		case LAMP_EASY: set(102); break;
		case LAMP_NORMAL: set(103); break;
		case LAMP_HARD:
		case LAMP_EXHARD: set(104); break;
		case LAMP_FULLCOMBO:
		case LAMP_PERFECT:
		case LAMP_MAX: set(106); break;
		}
	}

	// 110 AAA 8/9
	{
		using namespace Option;
		switch (gOptions.get(eOption::SELECT_ENTRY_RANK))
		{
		case RANK_0:
		case RANK_1: set(110); break;
		case RANK_2: set(111); break;
		case RANK_3: set(112); break;
		case RANK_4: set(113); break;
		case RANK_5: set(114); break;
		case RANK_6: set(115); break;
		case RANK_7: set(116); break;
		case RANK_8: set(117); break;
		}
	}

	// 624 �Է֤����֤Υ���������^����״�r�ǤϤʤ� (�F״�Ǥϡ���󥭥󥰱�ʾ�Фȥ饤�Х�ե����)
	// 625 �Է֤����֤Υ���������^����٤�״�r�Ǥ���
	set(624);


	// rival
	// 640 NOT PLAYED
	// 641 FAILED
	// 642 EASY CLEARED
	// 643 NORMAL CLEARED
	// 644 HARD CLEARED
	// 645 FULL COMBO

	// rival
	// 650 AAA 8/9
	// 651 AA 7/9
	// 652 A 6/9
	// 653 B 5/9
	// 654 C 4/9
	// 655 D 3/9
	// 656 E 2/9
	// 657 F 1/9



	// //���ꥢ�g�ߥ��ץ����ե饰(���`��)
	// 118 GROOVE
	// 119 SURVIVAL
	// 120 SUDDEN DEATH
	// 121 EASY
	// 122 PERFECT ATTACK
	// 123 GOOD ATTACK
	// set(118);

	// //���ꥢ�g�ߥ��ץ����ե饰(������)
	// 126 ��Ҏ
	// 127 MIRROR
	// 128 RANDOM
	// 129 S-RANDOM
	// 130 SCATTER
	// 131 CONVERGE
	// set(126);

	// //���ꥢ�g�ߥ��ץ����ե饰(���ե�����)
	// 134 �o��
	// 135 HIDDEN
	// 136 SUDDEN
	// 137 HID+SUD
	// set(134);

	// //���������ץ����ե饰
	// 142 AUTO SCRATCH (�Ԅ���i���ǥ��ꥢ����������ޤ�)
	// 143 EXTRA MODE
	// 144 DOUBLE BATTLE
	// 145 SP TO DP (�⤷�����������DP TO SP�� 9 TO 7�ȹ����Ŀ�ˤʤ뤫�⡣

	// 150 difficulty0 (δ�O��)
	if (get(5))
	{
		switch (gOptions.get(eOption::CHART_DIFFICULTY))
		{
			using namespace Option;
		case DIFF_ANY: set(150); break;
		case DIFF_BEGINNER: set(151); break;
		case DIFF_NORMAL: set(152); break;
		case DIFF_HYPER: set(153); break;
		case DIFF_ANOTHER: set(154); break;
		case DIFF_INSANE: set(155); break;
		}
	}

	if (get(5))	// is playable
	{

		// //Ԫ�ǩ`��
		// 160 7keys
		// 161 5keys
		// 162 14keys
		// 163 10keys
		// 164 9keys
		{
			using namespace Option;
			switch (gOptions.get(eOption::CHART_PLAY_KEYS))
			{
			case KEYS_NOT_PLAYABLE: break;
			case KEYS_7: set(160); break;
			case KEYS_5: set(161); break;
			case KEYS_14: set(162); break;
			case KEYS_10: set(163); break;
			case KEYS_9: set(164); break;
			}
		}

		// //���ץ����ȫ�m�������K�Ĥ��I�P��
		// //165 7keys
		// //166 5keys
		// //167 14keys
		// //168 10keys
		// //169 9keys


		// 170 BGA�o��
		// 171 BGA�Ф�
		set(170, !sw(eSwitch::CHART_HAVE_BGA));
		set(171, sw(eSwitch::CHART_HAVE_BGA));

		// 172 ��󥰥Ω`�ȟo��
		// 173 ��󥰥Ω`���Ф�
		set(172, !sw(eSwitch::CHART_HAVE_LN));
		set(173, sw(eSwitch::CHART_HAVE_LN));

		// 174 �����ƥ����ȟo��
		// 175 �����ƥ������Ф�
		set(174, !sw(eSwitch::CHART_HAVE_README));
		set(175, sw(eSwitch::CHART_HAVE_README));

		// 176 BPM�仯�o��
		// 177 BPM�仯�Ф�
		set(176, !sw(eSwitch::CHART_HAVE_BPMCHANGE));
		set(177, sw(eSwitch::CHART_HAVE_BPMCHANGE));

		// 178 ����������o��
		// 179 �����������Ф�
		set(178, !sw(eSwitch::CHART_HAVE_RANDOM));
		set(179, sw(eSwitch::CHART_HAVE_RANDOM));

		// 180 �ж�veryhard
		// 181 �ж�hard
		// 182 �ж�normal
		// 183 �ж�easy
		switch (gOptions.get(eOption::CHART_JUDGE_TYPE))
		{
			using namespace Option;
		case JUDGE_VHARD: set(180); break;
		case JUDGE_HARD: set(181); break;
		case JUDGE_NORMAL: set(182); break;
		case JUDGE_EASY: set(183); break;
		}

		// 185 ��٥뤬Ҏ�����ڤˤ���(5/10keys��LV9��7/14keys��LV12��9keys��LV42����)
		// 186 ��٥뤬Ҏ������Խ���Ƥ���
		switch (gOptions.get(eOption::CHART_DIFFICULTY))
		{
			using namespace Option;
			//case DIFF_ANY: set(185); break;
		case DIFF_BEGINNER: set(185, _op[70]); set(186, _op[75]);  break;
		case DIFF_NORMAL:  set(185, _op[71]); set(186, _op[76]); break;
		case DIFF_HYPER:  set(185, _op[72]); set(186, _op[77]); break;
		case DIFF_ANOTHER:  set(185, _op[73]); set(186, _op[78]); break;
		case DIFF_INSANE:  set(185, _op[74]); set(186, _op[79]); break;
		}

		// 190 STAGEFILE�o��
		// 191 STAGEFILE�Ф�
		set(190, !sw(eSwitch::CHART_HAVE_STAGEFILE));
		set(191, sw(eSwitch::CHART_HAVE_STAGEFILE));

		// 192 BANNER�o��
		// 193 BANNER�Ф�
		set(192, !sw(eSwitch::CHART_HAVE_BANNER));
		set(193, sw(eSwitch::CHART_HAVE_BANNER));

		// 194 BACKBMP�o��
		// 195 BACKBMP�Ф�
		set(194, !sw(eSwitch::CHART_HAVE_BACKBMP));
		set(195, sw(eSwitch::CHART_HAVE_BACKBMP));

		// 196 ��ץ쥤�o��
		// 197 ��ץ쥤�Ф�
		set(196);
	}

	// /////////////////////////////////
	// //�ץ쥤��
	// 200 1P AAA
	{
		using namespace Option;
		switch (gOptions.get(eOption::PLAY_RANK_ESTIMATED_1P))
		{
		case RANK_0:
		case RANK_1: set(200); break;
		case RANK_2: set(201); break;
		case RANK_3: set(202); break;
		case RANK_4: set(203); break;
		case RANK_5: set(204); break;
		case RANK_6: set(205); break;
		case RANK_7: set(206); break;
		case RANK_8: set(207); break;
		case RANK_NONE: break;
		}
	}

	// 210 2P AAA
	{
		using namespace Option;
		switch (gOptions.get(eOption::PLAY_RANK_ESTIMATED_2P))
		{
		case RANK_0:
		case RANK_1: set(210); break;
		case RANK_2: set(211); break;
		case RANK_3: set(212); break;
		case RANK_4: set(213); break;
		case RANK_5: set(214); break;
		case RANK_6: set(215); break;
		case RANK_7: set(216); break;
		case RANK_8: set(217); break;
		case RANK_NONE: break;
		}
	}


	// 220 AAA�_��
	{
		using namespace Option;
		switch (gOptions.get(eOption::PLAY_RANK_BORDER_1P))
		{
		case RANK_0:
		case RANK_1: set(220); break;
		case RANK_2: set(221); break;
		case RANK_3: set(222); break;
		case RANK_4: set(223); break;
		case RANK_5: set(224); break;
		case RANK_6: set(225); break;
		case RANK_7: set(226); break;
		case RANK_8: set(227); break;
		case RANK_NONE: break;
		}
	}

	// 230 1P 0-10%
	{
		using namespace Option;
		switch (gOptions.get(eOption::PLAY_ACCURACY_1P))
		{
		case ACC_0:  set(230); break;
		case ACC_10: set(231); break;
		case ACC_20: set(232); break;
		case ACC_30: set(233); break;
		case ACC_40: set(234); break;
		case ACC_50: set(235); break;
		case ACC_60: set(236); break;
		case ACC_70: set(237); break;
		case ACC_80: set(238); break;
		case ACC_90: set(239); break;
		case ACC_100: set(240); break;
		}
	}

	// 241 1P PERFECT
	// 242 1P GREAT
	// 243 1P GOOD
	// 244 1P BAD
	// 245 1P POOR
	// 246 1P ��POOR
	{
		using namespace Option;
		switch (gOptions.get(eOption::PLAY_LAST_JUDGE_1P))
		{
		case JUDGE_NONE: break;
		case JUDGE_0: set(241); break;
		case JUDGE_1: set(242); break;
		case JUDGE_2: set(243); break;
		case JUDGE_3: set(244); break;
		case JUDGE_4: set(245); break;
		case JUDGE_5: set(246); break;
		}
	}

	// //��ʽ�ϩ`�ե���������ҤΥͥ����äǤ� 2P�Ȥ�
	// 247 1P POORBGA��ʾ�r�g��
	// 248 1P POORBGA��ʾ�r�g��
	set(247);

	// 250 2P 0-10%
	{
		using namespace Option;
		switch (gOptions.get(eOption::PLAY_ACCURACY_2P))
		{
		case ACC_0:  set(250); break;
		case ACC_10: set(251); break;
		case ACC_20: set(252); break;
		case ACC_30: set(253); break;
		case ACC_40: set(254); break;
		case ACC_50: set(255); break;
		case ACC_60: set(256); break;
		case ACC_70: set(257); break;
		case ACC_80: set(258); break;
		case ACC_90: set(259); break;
		case ACC_100: set(260); break;
		}
	}

	// 261 2P PERFECT
	// 262 2P GREAT
	// 263 2P GOOD
	// 264 2P BAD
	// 265 2P POOR
	// 266 2P ��POOR
	{
		using namespace Option;
		switch (gOptions.get(eOption::PLAY_LAST_JUDGE_2P))
		{
		case JUDGE_0: set(261); break;
		case JUDGE_1: set(262); break;
		case JUDGE_2: set(263); break;
		case JUDGE_3: set(264); break;
		case JUDGE_4: set(265); break;
		case JUDGE_5: set(266); break;
		}
	}

	// 267 2P POORBGA��ʾ�r�g��
	// 268 2P POORBGA��ʾ�r�g��
	set(267);

	// 270 1P SUD+�����
	// 271 2P SUD+�����
	set(270, sw(eSwitch::P1_SETTING_SPEED));
	set(271, sw(eSwitch::P2_SETTING_SPEED));

	// 280 ���`�����Ʃ`��1
	// 281 ���`�����Ʃ`��2
	// 282 ���`�����Ʃ`��3
	// 283 ���`�����Ʃ`��4
	// 289 ���`�����Ʃ`��FINAL
	// (ע�� ������STAGE3����K���Ʃ`���Έ��ϡ����Ʃ`��FINAL�����Ȥ��졢283����282���դȤʤ�ޤ���)
	// (�F�ڤόgװ���Ƥ��ޤ��󤬡�����Β����˂䤨��284-288�ˤ�����STAGE5-9�λ���⤢�餫�������äƤ����������������⤷��ޤ���
	{
		switch (gOptions.get(eOption::PLAY_COURSE_STAGE))
		{
			using namespace Option;
		case STAGE_NOT_COURSE: break;
		case STAGE_1: set(280); break;
		case STAGE_2: set(281); break;
		case STAGE_3: set(282); break;
		case STAGE_4: set(283); break;
		case STAGE_FINAL: set(289); break;
		}
	}

	// 290 ���`��
	// 291 �Υ󥹥ȥå�
	// 292 �������ѩ`��
	// 293 ��λ�J��

	// //////////////////////////////////
	// //�ꥶ

	// 300 1P AAA
	{
		using namespace Option;
		switch (gOptions.get(eOption::RESULT_RANK_1P))
		{
		case RANK_0:
		case RANK_1: set(300); break;
		case RANK_2: set(301); break;
		case RANK_3: set(302); break;
		case RANK_4: set(303); break;
		case RANK_5: set(304); break;
		case RANK_6: set(305); break;
		case RANK_7: set(306); break;
		case RANK_8: set(307); break;
		case RANK_NONE: set(308); break;
		}
	}

	// 310 2P AAA
	{
		using namespace Option;
		switch (gOptions.get(eOption::RESULT_RANK_2P))
		{
		case RANK_0:
		case RANK_1: set(310); break;
		case RANK_2: set(311); break;
		case RANK_3: set(312); break;
		case RANK_4: set(313); break;
		case RANK_5: set(314); break;
		case RANK_6: set(315); break;
		case RANK_7: set(316); break;
		case RANK_8: set(317); break;
		case RANK_NONE: set(318); break;
		}
	}

	// 320 ����ǰ AAA
	{
		using namespace Option;
		switch (gOptions.get(eOption::RESULT_MYBEST_RANK))
		{
		case RANK_0:
		case RANK_1: set(320); break;
		case RANK_2: set(321); break;
		case RANK_3: set(322); break;
		case RANK_4: set(323); break;
		case RANK_5: set(324); break;
		case RANK_6: set(325); break;
		case RANK_7: set(326); break;
		case RANK_8: set(327); break;
		case RANK_NONE: break;
		}
	}

	// 330 �����������¤��줿
	set(330, sw(eSwitch::RESULT_UPDATED_SCORE));
	// 331 MAXCOMBO�����¤��줿
	set(331, sw(eSwitch::RESULT_UPDATED_MAXCOMBO));
	// 332 ��СB+P�����¤��줿
	set(332, sw(eSwitch::RESULT_UPDATED_BP));
	// 333 �ȥ饤���뤬���¤��줿
	//set(333, sw(eSwitch::RESULT_UPDATED_TRIAL));
	set(333, false);
	// 334 IR���λ�����¤��줿
	//set(334, sw(eSwitch::RESULT_UPDATED_IRRANK));
	set(334, false);
	// 335 ��������󥯤����¤��줿
	set(335, sw(eSwitch::RESULT_UPDATED_RANK));

	// 340 ������ AAA
	{
		using namespace Option;
		switch (gOptions.get(eOption::RESULT_UPDATED_RANK))
		{
		case RANK_0:
		case RANK_1: set(340); break;
		case RANK_2: set(341); break;
		case RANK_3: set(342); break;
		case RANK_4: set(343); break;
		case RANK_5: set(344); break;
		case RANK_6: set(345); break;
		case RANK_7: set(346); break;
		case RANK_8: set(347); break;
		}
	}


	// 350 �ꥶ��ȥե�åןo��(�ץ쥤�������#FLIPRESULT����o�����⤷����#DISABLEFLIP�����Խ�
	// 351 �ꥶ��ȥե�å��Є�(�ץ쥤�������#FLIPRESULT�����Ф�
	set(350);

	// 352 1PWIN 2PLOSE
	// 353 1PLOSE 2PWIN
	// 354 DRAW


	// ///////////////////////////////////
	// //���`����ե���

	// 400 7/14KEYS
	// 401 9KEYS
	// 402 5/10KEYS


	// ///////////////////////////////////
	// //������
	// 500 ͬ���ե������beginner�V�椬���ڤ��ʤ�
	// 501 ͬ���ե������normal�V�椬���ڤ��ʤ�
	// 502 ͬ���ե������hyper�V�椬���ڤ��ʤ�
	// 503 ͬ���ե������another�V�椬���ڤ��ʤ�
	// 504 ͬ���ե������insane�V�椬���ڤ��ʤ�

	// 505 ͬ���ե������beginner�V�椬���ڤ���
	// 506 ͬ���ե������normal�V�椬���ڤ���
	// 507 ͬ���ե������hyper�V�椬���ڤ���
	// 508 ͬ���ե������another�V�椬���ڤ���
	// 509 ͬ���ե������insane�V�椬���ڤ���

	// 510 ͬ���ե������һ����beginner�V�椬���ڤ���
	// 511 ͬ���ե������һ����normal�V�椬���ڤ���
	// 512 ͬ���ե������һ����hyper�V�椬���ڤ���
	// 513 ͬ���ե������һ����another�V�椬���ڤ���
	// 514 ͬ���ե������һ����nsane�V�椬���ڤ���

	// 515 ͬ���ե�������}����beginner�V�椬���ڤ���
	// 516 ͬ���ե�������}����normal�V�椬���ڤ���
	// 517 ͬ���ե�������}����hyper�V�椬���ڤ���
	// 518 ͬ���ե�������}����another�V�椬���ڤ���
	// 519 ͬ���ե�������}����nsane�V�椬���ڤ���
	if (get(5))
	{
		set({ 500, 501, 502, 503, 504 });

		switch (gOptions.get(eOption::CHART_DIFFICULTY))
		{
			using namespace Option;
		case DIFF_BEGINNER:
			set(500, false); 
			set({ 505, 510 }); 
			break;
		case DIFF_NORMAL: 
			set(501, false); 
			set({ 506, 511 });  
			break;
		case DIFF_HYPER: 
			set(502, false);
			set({ 507, 512 });  
			break;
		case DIFF_ANOTHER: 
			set(503, false); 
			set({ 508, 513 }); 
			break;
		case DIFF_INSANE: 
			set(504, false);
			set({ 509, 514 });
			break;
		default: 
			break;
		}
	}

	// 520 ��٥�Щ` beginner no play
	// 521 ��٥�Щ` beginner failed
	// 522 ��٥�Щ` beginner easy
	// 523 ��٥�Щ` beginner clear
	// 524 ��٥�Щ` beginner hardclear
	// 525 ��٥�Щ` beginner fullcombo

	// 530 ��٥�Щ` normal no play
	// 531 ��٥�Щ` normal failed
	// 532 ��٥�Щ` normal easy
	// 533 ��٥�Щ` normal clear
	// 534 ��٥�Щ` normal hardclear
	// 535 ��٥�Щ` normal fullcombo

	// 540 ��٥�Щ` hyper no play
	// 541 ��٥�Щ` hyper failed
	// 542 ��٥�Щ` hyper easy
	// 543 ��٥�Щ` hyper clear
	// 544 ��٥�Щ` hyper hardclear
	// 545 ��٥�Щ` hyper fullcombo

	// 550 ��٥�Щ` another no play
	// 551 ��٥�Щ` another failed
	// 552 ��٥�Щ` another easy
	// 553 ��٥�Щ` another clear
	// 554 ��٥�Щ` another hardclear
	// 555 ��٥�Щ` another fullcombo

	// 560 ��٥�Щ` insane no play
	// 561 ��٥�Щ` insane failed
	// 562 ��٥�Щ` insane easy
	// 563 ��٥�Щ` insane clear
	// 564 ��٥�Щ` insane hardclear
	// 565 ��٥�Щ` insane fullcombo


	// /////////////////////////////////////
	// //�����`�����쥯���v�B

	// 580 ���`��stage��1����
	// 581 ���`��stage��2����
	// 582 ���`��stage��3����
	// 583 ���`��stage��4����
	// 584 ���`��stage��5����
	// 585 ���`��stage��6����
	// 586 ���`��stage��7����
	// 587 ���`��stage��8����
	// 588 ���`��stage��9����
	// 589 ���`��stage��10����

	// 590 ���`�����쥯�� stage1�x�k��
	// 591 ���`�����쥯�� stage2�x�k��
	// 592 ���`�����쥯�� stage3�x�k��
	// 593 ���`�����쥯�� stage4�x�k��
	// 594 ���`�����쥯�� stage5�x�k��
	// 595 ���`�����쥯�� stage6�x�k��
	// 596 ���`�����쥯�� stage7�x�k��
	// 597 ���`�����쥯�� stage8�x�k��
	// 598 ���`�����쥯�� stage9�x�k��
	// 599 ���`�����쥯�� stage10�x�k��

	// 571 ���`�����쥯���ФǤ���
	// 572 ���`�����쥯���ФǤϟo��
	set(572);

	// //���`��stage1
	// 700 ���`��stage1 difficultyδ���x
	// 701 ���`��stage1 difficulty1
	// 702 ���`��stage1 difficulty2
	// 703 ���`��stage1 difficulty3
	// 704 ���`��stage1 difficulty4
	// 705 ���`��stage1 difficulty5

	// //���`��stage2
	// 710 ���`��stage2 difficultyδ���x
	// 711 ���`��stage2 difficulty1
	// 712 ���`��stage2 difficulty2
	// 713 ���`��stage2 difficulty3
	// 714 ���`��stage2 difficulty4
	// 715 ���`��stage2 difficulty5

	// //���`��stage3
	// 720 ���`��stage3 difficultyδ���x
	// 721 ���`��stage3 difficulty1
	// 722 ���`��stage3 difficulty2
	// 723 ���`��stage3 difficulty3
	// 724 ���`��stage3 difficulty4
	// 725 ���`��stage3 difficulty5

	// //���`��stage4
	// 730 ���`��stage4 difficultyδ���x
	// 731 ���`��stage4 difficulty1
	// 732 ���`��stage4 difficulty2
	// 733 ���`��stage4 difficulty3
	// 734 ���`��stage4 difficulty4
	// 735 ���`��stage4 difficulty5

	// //���`��stage5
	// 740 ���`��stage5 difficultyδ���x
	// 741 ���`��stage5 difficulty1
	// 742 ���`��stage5 difficulty2
	// 743 ���`��stage5 difficulty3
	// 744 ���`��stage5 difficulty4
	// 745 ���`��stage5 difficulty5


	// ///////////////////////////////////
	// //LR2IR�v�B
	// 600 IR����ǤϤʤ�
	// 601 IR�i���z����
	// 602 IR�i���z������
	// 603 IR�ץ쥤��`�o��
	// 604 IR�ӾAʧ��
	// 605 BAN��
	// 606 IR���´���
	// 607 IR����������
	// 608 IR�ӥ��`
	set(600);


	// 620 ��󥭥󥰱�ʾ�ФǤϤʤ�
	// 621 ��󥭥󥰱�ʾ��
	set(620);

	// 622 ���`���ȥХȥ�ǤϤʤ�
	// 623 ���`���ȥХȥ�k����(�Q���ݳ����ꥶ��Ȥ��g�Τ�)
	set(622);
}