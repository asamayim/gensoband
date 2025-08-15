/* File: mutation.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Mutation effects (and racial powers) */

#include "angband.h"




/*:::�ˑR�ψق��s���R�ȏd�������Ȃ��悤�ɂ��邽�߂̔z��B�z��̓Y������MUT????�́u���ԁv�ɑΉ����z��̒l�̓O���[�v�ԍ�(0�̓O���[�v����)�Alose_mutation�p����*/
/*:::�����ψق𓾂�Ƃ����̔z�񂪎Q�Ƃ���A�\�̌��܂œ����ň�̌����Ⴄ�O���[�v�ԍ��̕ψق����ɂ����lose_mutation()�ŏ������B*/
/*:::�O���[�v���ɉi���ψق�����Ɣr���ψق͓����Ȃ��B*/

int exclusive_muta_lis[128][2]=
{
	{0,1},//MUT1_BR_ACID
	{0,5},//MUT1_BR_FIRE
	{1,8},//MUT1_HYPN_GAZE
	{0,10},//MUT1_TELEKINES;
	{0,12},//MUT1_VTELEPORT;
	{0,15},//MUT1_MIND_BLST;
	{0,17},//MUT1_BR_ELEC;
	{141,19},//MUT1_VAMPIRISM;
	{143,21},//MUT1_ACCELERATION;
	{0,24},//MUT1_BEAST_EAR;
	{0,28},//MUT1_BLINK;
	{142,31},//MUT1_EAT_ROCK;
	{0,33},//MUT1_SWAP_POS;
	{52,35},//MUT1_BR_SOUND;
	{0,38},//MUT1_BR_COLD;
	{0,41},//MUT1_DET_ITEM;
	{0,43},//MUT1_BERSERK;
	{0,46},//MUT1_BR_POIS;
	{0,47},//MUT1_MIDAS_TCH;
	{151,49},//MUT1_GROW_MOLD;
	{0,50},//MUT1_RESIST;
	{0,53},//MUT1_EARTHQUAKE;
	{0,56},//MUT1_EAT_MAGIC;
	{152,57},//MUT1_RAISE_WOOD;
	{0,59},//MUT1_STERILITY;
	{0,60},//MUT1_PANIC_HIT;
	{1,62},//MUT1_DAZZLE;
	{1,65},//MUT1_LASER_EYE;
	{0,68},//MUT1_RECALL;
	{0,70},//MUT1_ID_MUSHROOM;
	{0,71},//MUT1_DANMAKU;
	{0,0}, //dummy

	{0,75},//MUT2_BERS_RAGE;
	{102,76},//MUT2_COWARDICE;
	{0,77},//MUT2_RTELEPORT;
	{0,78},//MUT2_ALCOHOL;
	{0,79},//MUT2_HALLU;
	{0,80},//MUT2_YAKU;
	{21,81},//MUT2_HARDHEAD;
	{22,83},//MUT2_HORNS;
	{23,85},//MUT2_BIGHORN;
	{0,87},//MUT2_ATT_DEMON;
	{0,89},//MUT2_PROD_MANA;
	{0,90},//MUT2_SPEED_FLUX;
	{0,92},//MUT2_BANISH_ALL;
	{0,94},//MUT2_EAT_LIGHT;
	{31,95},//MUT2_TAIL;
	{0,97},//MUT2_ATT_ANIMAL;
	{32,98},//MUT2_BIGTAIL;
	{0,99},//MUT2_RAW_CHAOS;
	{0,100},//MUT2_NORMALITY;
	{0,103},//MUT2_WRAITH;
	{0,104},//MUT2_ATT_GHOST;
	{0,105},//MUT2_ATT_KWAI;
	{0,106},//MUT2_ATT_DRAGON;
	{41,107},//MUT2_XOM;
	{11,109},//MUT2_BIRDBRAIN;
	{42,110},//MUT2_CHAOS_GIFT;
	{0,112},//MUT2_WALK_SHAD;
	{0,113},//MUT2_WARNING;
	{0,115},//MUT2_INVULN;
	{51,116},//MUT2_ASTHMA;
	{0,118},//MUT2_THE_WORLD;
	{132,119},//MUT2_DISARM;

	{61,120},//MUT3_HYPER_STR;
	{62,123},//MUT3_PUNY;
	{12,126},//MUT3_HYPER_INT;
	{11,129},//MUT3_MORONIC;
	{71,132},//MUT3_RESILIENT;
	{0,134},//MUT3_WARNING;
	{72,136},//MUT3_ALBINO;
	{61,138},//MUT3_SPEEDSTER;
	{0,141},//MUT3_RES_WATER;
	{2,143},//MUT3_BLANK_FAC;
	{0,145},//MUT3_HURT_LITE;
	{1,146},//MUT3_XTRA_EYES;
	{0,149},//MUT3_MAGIC_RES;
	{0,151},//MUT3_XTRA_NOIS;
	{0,154},//MUT3_BYDO;
	{33,157},//MUT3_FISH_TAIL;
	{23,159},//MUT3_PEGASUS;
	{91,161},//MUT3_ACID_SCALES;
	{92,163},//MUT3_ELEC_SCALES;
	{93,165},//MUT3_FIRE_SCALES;
	{94,168},//MUT3_COLD_SCALES;
	{95,171},//MUT3_POIS_SCALES;
	{0,173},//MUT3_WINGS;
	{101,175},//MUT3_FEARLESS;
	{0,178},//MUT3_FEVER;
	{0,180},//MUT3_ESP;
	{111,182},//MUT3_LIMBER;
	{112,185},//MUT3_ARTHRITIS;
	{121,188},//MUT3_BAD_LUCK;
	{0,189},//MUT3_VULN_ELEM;
	{131,190},//MUT3_MOTION;
	{122,193},//MUT3_GOOD_LUCK;

	{ 161,194}, //������߈�(�I�I�J�~)
	{ 162,200 }, //�J���E�\
	{ 163,206 }, //�I�I���V
	{ 164,212 }, //�N���Q
	{ 165,213 }, //�E�V
	{ 166,214 }, //�q���R
	{ 167,215 }, //�J��
	{ 168,216 }, //�n�j��
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������
	{ 0,0 }, //������

};


/*:::�����_���ȓˑR�ψق𓾂�B����m�̓E�B�U�[�h���[�h�ł̎w��������Ĕ����^�ψق𓾂��Ȃ�*/
///sys race mutation class �ˑR�ψُ���
bool gain_random_mutation(int choose_mut)
{
	int     attempts_left = 20;
	cptr    muta_desc = "";
	bool    muta_chosen = FALSE;
	u32b    muta_which = 0;
	u32b    *muta_class = NULL;

	bool	no_wield_change = FALSE;

	//�O�����ƕϐg���͑����i�ɉe�����o��ψق��󂯂Ȃ�
	if(p_ptr->mimic_form || p_ptr->pclass == CLASS_3_FAIRIES) no_wield_change = TRUE;

	//v1.1.32 �������ł͑����i�ɉe���̏o��ψق��󂯂Ȃ��悤�ɂ��Ă���
	if (character_icky) no_wield_change = TRUE;

	//v2.1.0 �����i�����������n�̕ψق�2�ȏ���Ȃ����Ƃɂ���B�ˑR�ψق̖�ŘA���ł��Ƒ����O��̃A�C�e����ꗓ���o�O��\��������
	if ((p_ptr->muta3 & MUT3_PEGASUS) || (p_ptr->muta3 & MUT3_FISH_TAIL) || (p_ptr->muta2 & MUT2_BIGHORN)) no_wield_change = TRUE;


	//�H���l�͕ψٖ��� ���ɂ��I�[������
	//v1.1.98 ����̜߈˂��󂯂�
	if(p_ptr->prace == RACE_HOURAI && choose_mut != 193 && choose_mut != 217)
	{
		if(one_in_(3))msg_print("��u�����t�������C�������������N����Ȃ������B");
		else if(one_in_(2))msg_print("��u��؂����Y���Y�����C�������������N����Ȃ������B");
		else msg_print("��u�����ڂ����Ƃ����������N����Ȃ������B");
		return TRUE;
	}

	if (choose_mut) attempts_left = 1;

	set_deity_bias(DBIAS_COSMOS, -1);

	while (attempts_left--)
	{
		//���i���C�������^�ψق𓾂���悤�ɂ���B�܂�������
		//switch (choose_mut ? choose_mut : (p_ptr->pseikaku == SEIKAKU_BERSERK ? 74+randint1(119) : randint1(193)))
		switch (choose_mut ? choose_mut : randint1(218))
		{
		case 1: case 2: case 3: 
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ACID;
#ifdef JP
muta_desc = "�_��f���\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to spit acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "�΂�f���\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "�Ö���̔\�͂𓾂��B";
#else
			muta_desc = "Your eyes look mesmerizing...";
#endif

			break;
		case 10: case 11:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "���̂�O���͂œ������\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "�����̈ӎv�Ńe���|�[�g����\�͂𓾂��B";
#else
			muta_desc = "You gain the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "���_�U���̔\�͂𓾂��B";
#else
			muta_desc = "You gain the power of Mind Blast.";
#endif

			break;
		case 17: case 18:case 4:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ELEC;
#ifdef JP
muta_desc = "����f���\�͂𓾂��B";
#else
			muta_desc = "You start emitting hard radiation.";
#endif

			break;
		case 19: case 20:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "�l�Ԃ̌���T�肽���Ȃ����c";
#else
			muta_desc = "You become vampiric.";
#endif

			break;
		case 21: case 22: 
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ACCELERATION;
#ifdef JP
muta_desc = "�����ɉ������u�����ڂ��ꂽ�I";
#else
			muta_desc = "You smell a metallic odor.";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BEAST_EAR;
#ifdef JP
muta_desc = "�b���������Ă����B";
#else
			muta_desc = "You smell filthy monsters.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "�ߋ����e���|�[�g�̔\�͂𓾂��B";
#else
			muta_desc = "You gain the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "�ǂ������������Ɍ�����B";
#else
			muta_desc = "The walls look delicious.";
#endif

			break;
		case 33: case 34:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "���l�̌C�ň�}�C�������悤�ȋC��������B";
#else
			muta_desc = "You feel like walking a mile in someone else's shoes.";
#endif

			break;
		case 35: case 36: 
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_SOUND;
#ifdef JP
muta_desc = "������ӂ��吺���o����悤�ɂȂ����I";
#else
			muta_desc = "Your vocal cords get much tougher.";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_COLD;
#ifdef JP
muta_desc = "�����f���\�͂𓾂��B";
#else
			muta_desc = "You can light up rooms with your presence.";
#endif

			break;
		case 41: case 42:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DET_ITEM;
#ifdef JP
muta_desc = "�ˑR�_�E�W���O�̃R�c��͂񂾋C������B";
#else
			muta_desc = "You can feel evil magics.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "����\�Ȍ����������B";
#else
			muta_desc = "You feel a controlled rage.";
#endif

			break;
		case 46:case 37:case 23:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_POIS;
#ifdef JP
muta_desc = "�ŃK�X��f���\�͂𓾂��B";
#else
			muta_desc = "Your body seems mutable.";
#endif

			break;
		case 47: case 48:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "�u�~�_�X���̎�v�̔\�͂𓾂��B";/*�g�D�[�����C�_�[�X�ɂ���܂����ˁB */
#else
			muta_desc = "You gain the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "�ˑR�J�r�ɐe���݂��o�����B";
#else
			muta_desc = "You feel a sudden affinity for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "���Ȃ��͌��f�̍U�����玩�����g������C������B";
#else
			muta_desc = "You feel like you can protect yourself.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "�_���W�����Œn�k���N�����\�͂𓾂��B";
#else
			muta_desc = "You gain the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "���@�̃A�C�e�������������Ɍ�����B";
#else
			muta_desc = "Your magic items look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RAISE_WOOD;
#ifdef JP
muta_desc = "���Ȃ��͐A���ƗF�B�ɂȂ����C������B";
#else
			muta_desc = "You feel you can better understand the magic around you.";
#endif

			break;
		case 59:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "���i�[���Ȃ����C������B";
#else
			muta_desc = "You can give everything around you a headache.";
#endif

			break;
		case 60: case 61:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "�ˑR�ꌂ���E�̃R�c��͂񂾋C������B";
#else
			muta_desc = "You suddenly understand how thieves feel.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "���C�������炷����𓾂��B";
#else
			muta_desc = "You gain the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "�Ⴉ��r�[�������Ă�悤�ɂȂ����I";
#else
			muta_desc = "Your eyes burn for a moment.";
#endif

			break;
		case 68: case 69:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "�_���W�����ƒn��̊Ԃ��e���|�[�g�ł���悤�ɂȂ����B";
#else
			muta_desc = "You feel briefly homesick, but it passes.";
#endif

			break;
		case 70:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ID_MUSHROOM;
#ifdef JP
muta_desc = "�L�m�R�̎�ނ�k�������邱�Ƃ��ł���悤�ɂȂ����B";
#else
			muta_desc = "You feel a holy wrath fill you.";
#endif

			break;
		case 71: case 72:
		case 73: case 74:

			if (is_special_seikaku(SEIKAKU_SPECIAL_CIRNO) || is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			{
				;//�`���m�Ɛ����̓��ꐫ�i�͂��łɔw���ɔ����J���Ă�ݒ�Ȃ̂ŉ������Ȃ�
			}
			else
			{
				muta_class = &(p_ptr->muta1);
				muta_which = MUT1_DANMAKU;
#ifdef JP
				//v1.1.39 �ύX
				muta_desc = "�w���̕ӂ肩��͂����ӂ�Ă���C������I";
#else
				muta_desc = "Your hands get very cold.";
#endif
			}
			break;
			/*:::�A�C�e�������ψق��폜�B�푰���C�V��������������蕥�����̂ł����g���Ȃ��͂�*/
/*
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "���Ȃ��̕��𓊂����͂��Ȃ苭���Ȃ����C������B";
#else
			muta_desc = "Your throwing arm feels much stronger.";
#endif

			break;
*/
		case 75:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "����s�\�Ȍ����������I";
#else
			muta_desc = "You become subject to fits of berserk rage!";
#endif

			break;
		case 76:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "���Ȃ��͕����|�ǂɂȂ����I";
#else
			muta_desc = "You become an incredible coward!";
#endif

			break;
		case 77:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "�����ӎ����B���ɂȂ����B";
#else
			muta_desc = "Your position seems very uncertain...";
#endif

			break;
		case 78:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "���Ȃ��̓A���R�[�����łɂȂ����B";
#else
			muta_desc = "Your body starts producing alcohol!";
#endif

			break;
		case 79:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "���Ȃ��͌��o�������N�������_�����ɖ`���ꂽ�B";
#else
			muta_desc = "You are afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_YAKU;
#ifdef JP
muta_desc = "���Ȃ��̎���ɖ�W�܂�悤�ɂȂ����c";
#else
			muta_desc = "You become subject to uncontrollable flatulence.";
#endif

			break;
		case 81: case 82:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HARDHEAD;
#ifdef JP
muta_desc = "��ςȐΓ��ɂȂ����I";
#else
			muta_desc = "You grow a scorpion tail!";
#endif

			break;
		case 83: case 84:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�z�Ɋp���������I";
#else
			muta_desc = "Horns pop forth into your forehead!";
#endif

			break;
		case 85: case 86:
			/*::: -Hack- �ϐg���͑�����������Ԃ悤�ȕψق��󂯂Ȃ��悤�ɂ��Ă���*/
			if(!no_wield_change)
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_BIGHORN;
				muta_desc = "�S�̂悤�ȗ��h�Ȋp�������Ă����I";
			}
			break;
		case 87: case 88:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "�����������t����悤�ɂȂ����B";
#else
			muta_desc = "You start attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "���Ȃ��͐���s�\�Ȗ��@�̃G�l���M�[�𔭐�����悤�ɂȂ����B";
#else
			muta_desc = "You start producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "���Ȃ����N�T���ɂȂ����B";
#else
			muta_desc = "You become manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "���낵���͂����Ȃ��̔w��ɐ���ł���C������B";
#else
			muta_desc = "You feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "���Ȃ��̓E���S���A���g�Ɋ�Ȑe���݂��o����悤�ɂȂ����B";
#else
			muta_desc = "You feel a strange kinship with Ungoliant.";
#endif

			break;
		case 95: case 96:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TAIL;
#ifdef JP
muta_desc = "���Ȃ₩�ȐK���������Ă����B";
#else
			muta_desc = "Your nose grows into an elephant-like trunk.";
#endif

			break;
		case 97:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "�����������t����悤�ɂȂ����B";
#else
			muta_desc = "You start attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIGTAIL;
#ifdef JP
muta_desc = "���̂悤�ȑ傫�ȐK���������Ă����B";
#else
			muta_desc = "Evil-looking tentacles sprout from your sides.";
#endif

			break;
		case 99:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "���͂̋�Ԃ��s����ɂȂ����C������B";
#else
			muta_desc = "You feel the universe is less stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "���Ȃ��͒ʏ�̑̂ɋ߂Â��Ă����C������B";
#else
			muta_desc = "You feel strangely normal.";
#endif

			break;
		case 103:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "���Ȃ��͗H�̉���������̉������肷��悤�ɂȂ����B";
#else
			muta_desc = "You start to fade in and out of the physical world.";
#endif

			break;
		case 104:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_GHOST;
#ifdef JP
muta_desc = "�H�삽�������Ȃ��̎���Ś��������̂��������B";
#else
			muta_desc = "You feel forces of chaos entering your old scars.";
#endif

			break;
		case 105:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_KWAI;
#ifdef JP
muta_desc = "�ǂ�����Ƃ��Ȃ��d���B�̎������������B";
#else
			muta_desc = "You suddenly contract a horrible wasting disease.";
#endif

			break;
		case 106:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "���Ȃ��̓h���S������������悤�ɂȂ����B";
#else
			muta_desc = "You start attracting dragons.";
#endif

			break;
		case 107: case 108:
			if (p_ptr->pclass == CLASS_MAID)
				msg_format("�]�������Ȃ��ɋ��������������傪�ǂ��������B");
			else if (p_ptr->prace == RACE_STRAYGOD || p_ptr->prace == RACE_DEITY)
				msg_format("�]���͂��Ȃ��������������������������悤���B");
			else
			{


			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_XOM;
#ifdef JP
muta_desc = "�]���͍������B�u���O���V�����I���`�����I�v";
#else
			muta_desc = "Your thoughts suddenly take off in strange directions.";
#endif
			}
			break;
		case 109:
			///mod150121 �d���͒����ɂȂ�Ȃ�
			if(p_ptr->pclass != CLASS_KEINE)
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_BIRDBRAIN;
				muta_desc = "���Y�ꂪ�Ђǂ��Ȃ����C������B";
			}
			break;
		case 110: case 111:
			/* Chaos warriors already have a chaos deity */
			//if (p_ptr->pclass != CLASS_CHAOS_WARRIOR)
			if (p_ptr->pclass == CLASS_MAID)
				msg_format("�J�I�X�̎�숫�������Ȃ��ɋ��������������傪�ǂ��������B");
			else if (p_ptr->prace == RACE_STRAYGOD || p_ptr->prace == RACE_DEITY)
				msg_format("�J�I�X�̎�숫���͂��Ȃ��ɋ����������Ȃ������B");
			else
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "���Ȃ��̓J�I�X�̎�숫���̒��ӂ��䂭�悤�ɂȂ����B";
#else
				muta_desc = "You attract the notice of a chaos deity!";
#endif

			}
			break;
		case 112:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "���Ȃ��͌��������̂悤�ɔ����Ɗ�����悤�ɂȂ����B";
#else
			muta_desc = "You feel like reality is as thin as paper.";
#endif

			break;
		case 113: case 114:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "���Ȃ��͓ˑR�p���m�C�A�ɂȂ����C������B";
#else
			muta_desc = "You suddenly feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "���Ȃ��͏j������A���G��ԂɂȂ锭����N�����悤�ɂȂ����B";
#else
			muta_desc = "You are blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			//�N�^�J�͚b���ɂȂ�Ȃ����Ƃɂ���B�����ɂ͚b���͍A�̕a�C�ł͂Ȃ����ǃj���^���_�̌䗘�v�͊P�~�߂������Ԃ�M���M�����\�͈͓̔����낤
			if (p_ptr->pclass == CLASS_KUTAKA) break;

			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ASTHMA;
#ifdef JP
muta_desc = "�b�������ɂȂ����B";
#else
			muta_desc = "You are subject to fits of magical healing.";
#endif

			break;
		case 118:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_THE_WORLD;
#ifdef JP
muta_desc = "�~�܂������̐��E�ɖ������ނ悤�ɂȂ����B";
#else
			muta_desc = "You are subject to fits of painful clarity.";
#endif

			break;
		case 119:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "���Ȃ��͂Ȃ����悭�]�Ԃ悤�ɂȂ����B";
#else
			muta_desc = "Your feet grow to four times their former size.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "���l�I�ɋ����Ȃ����I";
#else
			muta_desc = "You turn into a superhuman he-man!";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "�ؓ�������Ă��܂���...";
#else
			muta_desc = "Your muscles wither away...";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "���Ȃ��̔]�͐��̃R���s���[�^�ɐi�������I";
#else
			muta_desc = "Your brain evolves into a living computer!";
#endif

			break;
		case 129: case 130: case 131:


			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "�]�݂���������ƕϐF�����I";
#else
			muta_desc = "Your brain withers away...";
#endif

			break;
		case 132: case 133:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "���O��ă^�t�ɂȂ����B";
#else
			muta_desc = "You become extraordinarily resilient.";
#endif

			break;
		case 134: case 135:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WARNING;
#ifdef JP
muta_desc = "���܂��������s���Ȃ����I";
#else
			muta_desc = "You become sickeningly fat!";
#endif

			break;
		case 136: case 137:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "����̎��ɂȂ����C������...";
#else
			muta_desc = "You turn into an albino! You feel frail...";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SPEEDSTER;
#ifdef JP
muta_desc = "���Ȃ��̋r�͕͂��O��ċ����Ȃ����I";
#else
			muta_desc = "Your flesh is afflicted by a rotting disease!";
#endif

			break;
		case 141: case 142:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RES_WATER;
#ifdef JP
muta_desc = "�ۈ���ł������~�߂Ă�����C������I";
#else
			muta_desc = "Your voice turns into a ridiculous squeak!";
#endif

			break;
		case 143: case 144:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "�̂��؂�ڂ��ɂȂ����I";
#else
			muta_desc = "Your face becomes completely featureless!";
#endif

			break;
		case 145:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HURT_LITE;
#ifdef JP
muta_desc = "���邢�������ɂȂ����C������c";
#else
			muta_desc = "You start projecting a reassuring image.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
			if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				muta_desc = "�z�ɑ�l�̖ڂ��J�Ⴕ���I";
			else
				muta_desc = "�z�ɑ�O�̖ڂ��J�Ⴕ���I";
#else
			muta_desc = "You grow an extra pair of eyes!";
#endif

			break;
		case 149: case 150:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "���@�ւ̑ϐ��������B";
#else
			muta_desc = "You become resistant to magic.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "���Ȃ��͊�ȉ��𗧂Ďn�߂��I";
#else
			muta_desc = "You start making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BYDO;
#ifdef JP
muta_desc = "�o�C�h�זE�ɐZ�H���ꂽ�I";
#else
			muta_desc = "Your infravision is improved.";
#endif
			//v1.1.58
			flag_update_floor_music = TRUE;
			break;
		case 157: case 158:
			/*::: -Hack- �ϐg���͑�����������Ԃ悤�ȕψق��󂯂Ȃ��悤�ɂ��Ă���*/
			if(!no_wield_change)
			{
				muta_class = &(p_ptr->muta3);
				muta_which = MUT3_FISH_TAIL;
				muta_desc = "�����g���l���̐K���ɂȂ����I";
			}
			break;
		case 159: case 160:
			/*::: -Hack- �ϐg���͑�����������Ԃ悤�ȕψق��󂯂Ȃ��悤�ɂ��Ă���*/
			if(!no_wield_change)
			{

				muta_class = &(p_ptr->muta3);
				muta_which = MUT3_PEGASUS;
				muta_desc = "���^�����V�y�K�T�XMIX����ɂȂ����I";
			}
			break;
		case 161: case 162:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ACID_SCALES;
#ifdef JP
muta_desc = "���������؂ɕ���ꂽ�I";
#else
			muta_desc = "Electricity starts running through you!";
#endif

			break;
		case 163: case 164:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ELEC_SCALES;
#ifdef JP
muta_desc = "�������؂ɕ���ꂽ�I";
#else
			muta_desc = "Your body is enveloped in flames!";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FIRE_SCALES;
#ifdef JP
muta_desc = "�����Ԃ��؂ɕ���ꂽ�I";
#else
			muta_desc = "Disgusting warts appear everywhere on you!";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_COLD_SCALES;
#ifdef JP
muta_desc = "���������؂ɕ���ꂽ�I";
#else
			muta_desc = "Your skin turns into black scales!";
#endif

			break;
		case 171: case 172:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_POIS_SCALES;
#ifdef JP
muta_desc = "�����΂̗؂ɕ���ꂽ�I";
#else
			muta_desc = "Your skin turns to steel!";
#endif

			break;
		case 173: case 174:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "�w���ɉH���������B";
#else
			muta_desc = "You grow a pair of wings.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "���S�ɕ|��m�炸�ɂȂ����B";
#else
			muta_desc = "You become completely fearless.";
#endif

			break;
		case 178: case 179:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEVER;
#ifdef JP
muta_desc = "�����s���̔M�a�ɖ`���ꂽ�B";
#else
			muta_desc = "You start regenerating.";
#endif

			break;
		case 180: case 181:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "�e���p�V�[�̔\�͂𓾂��I";
#else
			muta_desc = "You develop a telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "�ؓ������Ȃ₩�ɂȂ����B";
#else
			muta_desc = "Your muscles become limber.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "�֐߂��ˑR�ɂݏo�����B";
#else
			muta_desc = "Your joints suddenly hurt.";
#endif

			break;
		case 188:
			if (p_ptr->pseikaku == SEIKAKU_LUCKY) break;
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "���ӂɖ����������I�[�������Ȃ����Ƃ�܂���...";
#else
			muta_desc = "There is a malignant black aura surrounding you...";
#endif

			break;
		case 189:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "���ɖ��h���ɂȂ����C������B";
#else
			muta_desc = "You feel strangely exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "�̂̓��삪��萳�m�ɂȂ����B";
#else
			muta_desc = "You move with new assurance.";
#endif

			break;
		case 193:
			///���J������������ɗ���̂ŕύX������
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "���ߐ[�������I�[�������Ȃ����Ƃ�܂���...";
#else
			muta_desc = "There is a benevolent white aura surrounding you...";
#endif

			break;

		case 194:	case 195:	case 196:
		case 197:	case 198:	case 199:
			//�푰�_�i�Ǝ푰�H��ƐE�ƃv���Y�����o�[�͜߈˂���Ȃ����Ƃɂ���B
			//�ݒ�I�ȕs���R��������邽�߂����邪�A�v���Y�����o�[�ƃw�J�[�e�B�A�̓ˑR�ψٕێ������ǉ����ʓ|�Ȃ̂���Ԃ̗��R�B
			//����muta4��ǉ������Ƃ��ǂ����悤�B
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))
			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}

			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_WOLF;
			muta_desc = "�I�I�J�~�̓�����ɜ߈˂��ꂽ�I";
			break;

		case 200:	case 201:	case 202:
		case 203:	case 204:	case 205:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_OTTER;
			muta_desc = "�J���E�\�̓�����ɜ߈˂��ꂽ�I";
			break;

		case 206:	case 207:	case 208:
		case 209:	case 210:	case 211:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_EAGLE;
			muta_desc = "�I�I���V�̓�����ɜ߈˂��ꂽ�I";
			break;

		case 212:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_JELLYFISH;
			muta_desc = "�N���Q�̓�����ɜ߈˂��ꂽ�I";
			break;

		case 213:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_COW;
			muta_desc = "�E�V�̓�����ɜ߈˂��ꂽ�I";
			break;

		case 214:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_CHICKEN;
			muta_desc = "�q���R�̓�����ɜ߈˂��ꂽ�I";
			break;

		case 215:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_TORTOISE;
			muta_desc = "�J���̓�����ɜ߈˂��ꂽ�I";
			break;

		case 216:
			if (p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_SPECTRE || p_ptr->prace == RACE_ANIMAL_GHOST || (CLASS_IS_PRISM_SISTERS))

			{
				msg_print("�����삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			//���łɓ��萨�͂ɎQ�����Ă���ʁX�ƎQ���������ɂȂ��l
			if (p_ptr->pclass == CLASS_BITEN || p_ptr->pclass == CLASS_ENOKO || p_ptr->pclass == CLASS_CHIYARI || p_ptr->pclass == CLASS_ZANMU) break;

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANIWA;
			muta_desc = "�n�j���́c�c�����H�c�c��H�ɜ߈˂��ꂽ�I";
			break;

		case 217://v1.1.84 �������Ɏ��߂����
			 //���̕ψق͕ψَw��ł��̊֐��ɗ����Ƃ����������Ȃ�
			if (!choose_mut)
			{
				break;
			}
			//�w�J�ƃv���Y�����o�[�͒e���Bmut4�������Ȃ�����
			if (p_ptr->pclass == CLASS_HECATIA || p_ptr->pclass == CLASS_ZANMU)
			{
				msg_print("���͂ȉ��삪����Ă��������Ȃ������ē����Ă������B");
				break;
			}
			if(CLASS_IS_PRISM_SISTERS)
			{
				msg_print("���͂ȉ��삪����Ă������o���������撣���Ēǂ��������B");
				break;
			}
			//�����삪�i�v�ψقƂ��Ĝ߂��Ă�ꍇ�͎��߂���Ȃ�(���̏�LUNATIC�ŋH�ɋN���邾��)
			if (p_ptr->muta4_perma & (MUT4_GHOST_WOLF | MUT4_GHOST_OTTER | MUT4_GHOST_EAGLE | MUT4_GHOST_JELLYFISH | MUT4_GHOST_COW | MUT4_GHOST_CHICKEN | MUT4_GHOST_TORTOISE | MUT4_GHOST_HANIWA))
			{
				break;
			}
			//�����삪�ʏ�ψقƂ��Ĝ߂��Ă�ꍇ�͏�����
			p_ptr->muta4 &= ~(MUT4_GHOST_WOLF | MUT4_GHOST_OTTER | MUT4_GHOST_EAGLE | MUT4_GHOST_JELLYFISH | MUT4_GHOST_COW | MUT4_GHOST_CHICKEN | MUT4_GHOST_TORTOISE | MUT4_GHOST_HANIWA);


			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANGOKUOH;
			muta_desc = "���Ȃ��͑剅��ɑ̂��������Ă��܂����I";
			break;

		case 218://v2.1.0 ���삽���̉���
			//�w�J�ƃv���Y�����o�[�͒e���Bmut4�������Ȃ�����
			if (p_ptr->pclass == CLASS_HECATIA || p_ptr->pclass == CLASS_ZANMU || p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_EIKI)
			{
				msg_print("���삽��������Ă��������Ȃ������ē����Ă������B");
				break;
			}
			if (CLASS_IS_PRISM_SISTERS)
			{
				msg_print("���삽��������Ă������o���������撣���Ēǂ��������B");
				break;
			}
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_CHEERS;
			muta_desc = "���삽���̉��݂̗͂����Ȃ��ɗ��ꍞ��ł����I";
			break;



		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (!(*muta_class & muta_which))
			{
				muta_chosen = TRUE;
			}
		}

		///mod140319 ��������ψق̏d�Ȃ�̗}�~�A�i���ψق̕ی�Ȃǂ̃��[�`����ǉ�
		if(muta_chosen)
		{
			int mut_num,i;
			/*:::�ψٌ���0�`127�̒l�ɕϊ�*/
			if(muta_class == &p_ptr->muta1)      mut_num = 0;
			else if(muta_class == &p_ptr->muta2) mut_num = 32;
			else if(muta_class == &p_ptr->muta3) mut_num = 64;
			else								 mut_num = 96;
	
			for(i=0;i<32;i++) if((muta_which>>i) & 1L) mut_num += i;

			/*:::�ψٌ��ɉ����r���I�ȑ��̕ψق�����Ƃ�*/
			if(exclusive_muta_lis[mut_num][0] != 0)
			{
				for(i=0;i<128;i++)
				{
					/*:::�_�~�[�s�͔�΂�*/
					if(exclusive_muta_lis[i][1] == 0)continue;
					if(exclusive_muta_lis[i][0] == 0)continue;


					/*:::�ψٌ��ɁA�O���[�vid��10�̌��ȏオ��v��1�̌�����v���Ȃ��ԍ�������ꍇ�r������*/
					if((exclusive_muta_lis[i][0]/10) == (exclusive_muta_lis[mut_num][0]/10) && exclusive_muta_lis[i][0] != exclusive_muta_lis[mut_num][0])
					{
						/*:::�r�������̑�������i���ψق̏ꍇ���̕ψٌ������Z�b�g*/
						if((i < 32        && p_ptr->muta1_perma >> (i%32) &1L) 
						 ||(i>=32 && i<64 && p_ptr->muta2_perma >> (i%32) &1L)
						 ||(i>=64 && i<96 && p_ptr->muta3_perma >> (i % 32) & 1L)
							||(i>=96         && p_ptr->muta4_perma >> (i%32) &1L))
						{
							if(cheat_xtra) msg_format("TESTMSG:�ψ�%d���r���K���ɂ��擾�s��",mut_num);
							muta_chosen = FALSE;
							break;
						}
					}
				}

				/*:::�i���ψقȂǂ��Ȃ��Ɗm�F������r������ψق̍폜*/
				if(muta_chosen) for(i=0;i<128;i++)
				{
					/*:::�_�~�[�s�͔�΂�*/
					if(exclusive_muta_lis[i][1] == 0)continue;
					if(exclusive_muta_lis[i][0] == 0)continue;
					/*:::�ψٌ��ɁA�O���[�vid��10�̌��ȏオ��v��1�̌�����v���Ȃ��ԍ�������ꍇ�r������*/
					if((exclusive_muta_lis[i][0]/10) == (exclusive_muta_lis[mut_num][0]/10) && exclusive_muta_lis[i][0] != exclusive_muta_lis[mut_num][0])
					{
						/*:::�r�������̑�����̕ψق�����*/
						if((i < 32        && p_ptr->muta1 >> (i%32) &1L) 
						||(i>=32 && i<64 && p_ptr->muta2 >> (i%32) &1L)
						||(i >= 64 && i<96 && p_ptr->muta3 >> (i % 32) & 1L)
							||(i>=96         && p_ptr->muta4 >> (i%32) &1L))
						{
							lose_mutation(exclusive_muta_lis[i][1]);
						}
					}
				}
			}
		}



		if (muta_chosen == TRUE) break;

		//muta_chosen��FALSE�̂Ƃ���while�̍Ē��I���[�v�ɖ߂�
	}

	if (!muta_chosen)
	{
#ifdef JP
//msg_print("���ʂɂȂ����C������B");
if(one_in_(4))msg_print("��u�����ڂ����Ƃ����������N����Ȃ������B");
else if(one_in_(3))msg_print("��u�w�������Y���Y�����������N����Ȃ������B");
else if(one_in_(2))msg_print("��u�̒����y���Ȃ����������N����Ȃ������B");
else msg_print("��u�����t�������������N����Ȃ������B");
#else
		msg_print("You feel normal.");
#endif

		return FALSE;
	}
	else
	{
		//chg_virtue(V_CHANCE, 1);

		/*
		  some races are apt to gain specified mutations
		  This should be allowed only if "choose_mut" is 0.
							--- henkma
		*/
			///pend ����푰�œ���̓ˑR�ψق𓾂₷�������@�ꎞ�I�ɖ�����
		if(!choose_mut)
		{
/*
			if (p_ptr->prace == RACE_VAMPIRE &&
			  !(p_ptr->muta1 & MUT1_HYPN_GAZE) &&
			   (randint1(10) < 7))
			{
				muta_class = &(p_ptr->muta1);
				muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "�Ⴊ���f�I�ɂȂ���...";
#else
				muta_desc = "Your eyes look mesmerizing...";
#endif

			}
*/
/*
			else if (p_ptr->prace == RACE_IMP &&
				 !(p_ptr->muta2 & MUT2_HORNS) &&
				 (randint1(10) < 7))
			  {
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�p���z���琶���Ă����I";
#else
				muta_desc = "Horns pop forth into your forehead!";
#endif

			}
*/
/*
			else if (p_ptr->prace == RACE_MIND_FLAYER &&
				!(p_ptr->muta2 & MUT2_BIGTAIL) &&
				(randint1(10) < 7))
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_BIGTAIL;
#ifdef JP
muta_desc = "�傫�ȐK���������Ă����B";
#else
				muta_desc = "Evil-looking tentacles sprout from your mouth.";
#endif

			}
*/
			
		}

#ifdef JP
msg_print("�ˑR�ψق����I");
#else
		msg_print("You mutate!");
#endif

		msg_print(muta_desc);
		*muta_class |= muta_which;

		///mod141124 ��ՓxLUNATIC�̂Ƃ��ψق��i�����i�V���ȂǂŎ���)
		if(difficulty == DIFFICULTY_LUNATIC  && one_in_(13) )
		{
			if (muta_class == &(p_ptr->muta3)) p_ptr->muta3_perma |= muta_which;
			else if (muta_class == &(p_ptr->muta2)) p_ptr->muta2_perma |= muta_which;
			else if (muta_class == &(p_ptr->muta1)) p_ptr->muta1_perma |= muta_which;
			else if (muta_class == &(p_ptr->muta4)) p_ptr->muta4_perma |= muta_which;
			else msg_print("ERROR:LUNATIC�̉i���ψُ�������������");

		}


		///mod140319 ����̕ψق𓾂��Ƃ��������O���B���Ƃ��̂̓R�[�h�ʓ|����������ƍ������ƂɂȂ邩������Ȃ��̂ł�߂Ă����B�U�b�N����t�Ȃ�ǂ��������邪�B
		//v2.1.0 �����̕ψق̈�����łɎ����Ă���ꍇ���̕ψق͂��Ȃ����Ƃɂ���
		if (muta_class == &(p_ptr->muta3) && muta_which == MUT3_FISH_TAIL)
		{
			if(inventory[INVEN_FEET].k_idx)
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_FEET], 0);

				msg_format("%s���E���Ă��܂����I",obj_name);
				inven_takeoff(INVEN_FEET,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}

		}
		if (muta_class == &(p_ptr->muta3) && muta_which == MUT3_PEGASUS)
		{
			if(inventory[INVEN_HEAD].k_idx)
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_HEAD], 0);

				msg_format("%s���E���Ă��܂����I",obj_name);
				inven_takeoff(INVEN_HEAD,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}

		}		
		if (muta_class == &(p_ptr->muta2) && muta_which == MUT2_BIGHORN)
		{
			if(inventory[INVEN_HEAD].k_idx && 
			(	inventory[INVEN_HEAD].sval == SV_HEAD_METALCAP 
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STARHELM 
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STEELHELM 
			||  inventory[INVEN_HEAD].sval == SV_HEAD_DRAGONHELM  ))
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_HEAD], 0);

				msg_format("%s���E���Ă��܂����I",obj_name);
				inven_takeoff(INVEN_HEAD,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}

		}


		/*:::��������ψق���x�Ɏ����Ȃ����鏈���@�ʏ����Ŏ��������̂Ŗ�����*/
#if 0

		if (muta_class == &(p_ptr->muta3))
		{
			if (muta_which == MUT3_PUNY)
			{
				if (p_ptr->muta3 & MUT3_HYPER_STR)
				{
#ifdef JP
msg_print("���Ȃ��͂������l�I�ɋ����͂Ȃ��I");
#else
					msg_print("You no longer feel super-strong!");
#endif

					p_ptr->muta3 &= ~(MUT3_HYPER_STR);
				}
			}
			else if (muta_which == MUT3_HYPER_STR)
			{
				if (p_ptr->muta3 & MUT3_PUNY)
				{
#ifdef JP
msg_print("���Ȃ��͂�������ł͂Ȃ��I");
#else
					msg_print("You no longer feel puny!");
#endif

					p_ptr->muta3 &= ~(MUT3_PUNY);
				}
			}
			else if (muta_which == MUT3_MORONIC)
			{
				if (p_ptr->muta3 & MUT3_HYPER_INT)
				{
#ifdef JP
msg_print("���Ȃ��̔]�͂������̃R���s���[�^�ł͂Ȃ��B");
#else
					msg_print("Your brain is no longer a living computer.");
#endif

					p_ptr->muta3 &= ~(MUT3_HYPER_INT);
				}
			}
			else if (muta_which == MUT3_HYPER_INT)
			{
				if (p_ptr->muta3 & MUT3_MORONIC)
				{
#ifdef JP
msg_print("���Ȃ��͂������_����ł͂Ȃ��B");
#else
					msg_print("You are no longer moronic.");
#endif

					p_ptr->muta3 &= ~(MUT3_MORONIC);
				}
			}
			else if (muta_which == MUT3_POIS_SCALES)
			{
				if (p_ptr->muta3 & MUT3_COLD_SCALES)
				{
#ifdef JP
msg_print("�؂��Ȃ��Ȃ����B");
#else
					msg_print("You lose your scales.");
#endif

					p_ptr->muta3 &= ~(MUT3_COLD_SCALES);
				}
				if (p_ptr->muta3 & MUT3_SPEEDSTER)
				{
#ifdef JP
msg_print("�r�͂����ɖ߂����B");
#else
					msg_print("Your flesh rots no longer.");
#endif

					p_ptr->muta3 &= ~(MUT3_SPEEDSTER);
				}
				if (p_ptr->muta3 & MUT3_FIRE_SCALES)
				{
#ifdef JP
msg_print("�������ɖ߂����B");
#else
					msg_print("You lose your warts.");
#endif

					p_ptr->muta3 &= ~(MUT3_FIRE_SCALES);
				}
			}
			else if (muta_which == MUT3_COLD_SCALES)
			{
				if (p_ptr->muta3 & MUT3_POIS_SCALES)
				{
#ifdef JP
msg_print("�؂��������B");
#else
					msg_print("Your skin is no longer made of steel.");
#endif

					p_ptr->muta3 &= ~(MUT3_POIS_SCALES);
				}
			}
			else if (muta_which == MUT3_FEARLESS)
			{
				if (p_ptr->muta2 & MUT2_COWARDICE)
				{
#ifdef JP
msg_print("�����|�ǂ��������B");
#else
					msg_print("You are no longer cowardly.");
#endif

					p_ptr->muta2 &= ~(MUT2_COWARDICE);
				}
			}

			else if (muta_which == MUT3_LIMBER)
			{
				if (p_ptr->muta3 & MUT3_ARTHRITIS)
				{
#ifdef JP
msg_print("�֐߂��ɂ��Ȃ��Ȃ����B");
#else
					msg_print("Your joints stop hurting.");
#endif

					p_ptr->muta3 &= ~(MUT3_ARTHRITIS);
				}
			}
			else if (muta_which == MUT3_ARTHRITIS)
			{
				if (p_ptr->muta3 & MUT3_LIMBER)
				{
#ifdef JP
msg_print("���Ȃ��͂��Ȃ₩�łȂ��Ȃ����B");
#else
					msg_print("You no longer feel limber.");
#endif

					p_ptr->muta3 &= ~(MUT3_LIMBER);
				}
			}
		}
		else if (muta_class == &(p_ptr->muta2))
		{
			if (muta_which == MUT2_COWARDICE)
			{
				if (p_ptr->muta3 & MUT3_FEARLESS)
				{
#ifdef JP
msg_print("����m�炸�łȂ��Ȃ����B");
#else
					msg_print("You no longer feel fearless.");
#endif

					p_ptr->muta3 &= ~(MUT3_FEARLESS);
				}
			}
			if (muta_which == MUT2_BIGHORN)
			{
				if (p_ptr->muta2 & MUT2_TAIL)
				{
#ifdef JP
msg_print("�K�����������B");
#else
					msg_print("Your nose is no longer elephantine.");
#endif

					p_ptr->muta2 &= ~(MUT2_TAIL);
				}
			}
			if (muta_which == MUT2_TAIL)
			{
				if (p_ptr->muta2 & MUT2_BIGHORN)
				{
#ifdef JP
msg_print("�p���������B");
#else
					msg_print("You no longer have a hard beak.");
#endif

					p_ptr->muta2 &= ~(MUT2_BIGHORN);
				}
			}
		}
#endif
		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


/*:::�ˑR�ψق������_���Ɏ���*/
/*:::20�񎎍s���������Ă���ψق��I�΂��΂���������ďI����TRUE��Ԃ��B��x���I�΂�Ȃ����FALSE��Ԃ��B*/
/*:::choose_mut:�����ˑR�ψق��w��(ID�ł͂Ȃ��I�j�����ꂾ��������*/
bool lose_mutation(int choose_mut)
{
	int attempts_left = 20;
	cptr muta_desc = "";
	bool muta_chosen = FALSE;
	u32b muta_which = 0;
	u32b *muta_class = NULL;

	if (choose_mut) attempts_left = 1;

	/*:::��{20�񎎍s�@�I�΂ꂽ�ψق������Ă����break*/
	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : randint1(217))
		{
		case 1: case 2: case 3: case 4:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ACID;
#ifdef JP
muta_desc = "�_�̃u���X��f���\�͂��������B";
#else
			muta_desc = "You lose the ability to spit acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "���̃u���X��f���\�͂��������B";
#else
			muta_desc = "You lose the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "�Ö���̔\�͂��������B";
#else
			muta_desc = "Your eyes look uninteresting.";
#endif

			break;
		case 10: case 11:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "�O���͂ŕ��𓮂����\�͂��������B";
#else
			muta_desc = "You lose the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "�����̈ӎv�Ńe���|�[�g����\�͂��������B";
#else
			muta_desc = "You lose the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "���_�U���̔\�͂��������B";
#else
			muta_desc = "You lose the power of Mind Blast.";
#endif

			break;
		case 17: case 18:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_ELEC;
#ifdef JP
muta_desc = "���̃u���X��f���\�͂��������B";
#else
			muta_desc = "You stop emitting hard radiation.";
#endif

			break;
		case 19: case 20:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "�z���̔\�͂��������B";
#else
			muta_desc = "You are no longer vampiric.";
#endif

			break;
		case 21: case 22: case 23:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ACCELERATION;
#ifdef JP
muta_desc = "�����̉������u���@�\���������C������B";
#else
			muta_desc = "You no longer smell a metallic odor.";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BEAST_EAR;
#ifdef JP
muta_desc = "�b�����������񂾁B";
#else
			muta_desc = "You no longer smell filthy monsters.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "�ߋ����e���|�[�g�̔\�͂��������B";
#else
			muta_desc = "You lose the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "�ǂ͔����������Ɍ����Ȃ��Ȃ����B";
#else
			muta_desc = "The walls look unappetizing.";
#endif

			break;
		case 33: case 34:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "���Ȃ��͎����̌C�ɗ��܂銴��������B";
#else
			muta_desc = "You feel like staying in your own shoes.";
#endif

			break;
		case 35: case 36: case 37:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_SOUND;
#ifdef JP
muta_desc = "���Ȃ��̐��͎キ�Ȃ����B";
#else
			muta_desc = "Your vocal cords get much weaker.";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_COLD;
#ifdef JP
muta_desc = "�����f���\�͂��������B";
#else
			muta_desc = "You can no longer light up rooms with your presence.";
#endif

			break;
		case 41: case 42:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DET_ITEM;
#ifdef JP
muta_desc = "�_�E�W���O�̃R�c��Y�ꂽ�B";
#else
			muta_desc = "You can no longer feel evil magics.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "����ł��錃��������Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel a controlled rage.";
#endif

			break;
		case 46:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_POIS;
#ifdef JP
muta_desc = "�ŃK�X��f���\�͂��������B";
#else
			muta_desc = "Your body seems stable.";
#endif

			break;
		case 47: case 48:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "�~�_�X�̎�̔\�͂��������B";
#else
			muta_desc = "You lose the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "�ˑR�J�r�������ɂȂ����B";
#else
			muta_desc = "You feel a sudden dislike for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "���f����g�������@��Y�ꂽ�B";
#else
			muta_desc = "You feel like you might be vulnerable.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "�_���W�������󂷔\�͂��������B";
#else
			muta_desc = "You lose the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "���@�̃A�C�e���͂��������������Ɍ����Ȃ��Ȃ����B";
#else
			muta_desc = "Your magic items no longer look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RAISE_WOOD;
#ifdef JP
muta_desc = "�A���Ɍ���ꂽ�C������B";
#else
			muta_desc = "You no longer sense magic.";
#endif

			break;
		case 59:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "��������̈��g�̓f�������������B";
#else
			muta_desc = "You hear a massed sigh of relief.";
#endif

			break;
		case 60: case 61:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "���������֒��ׂ�C�����Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel jumpy.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "���Ȃ��̊Ⴉ�狶�C���������B";
#else
			muta_desc = "You lose the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "�Ⴉ��r�[�������ĂȂ��Ȃ����B";
#else
			muta_desc = "Your eyes burn for a moment, then feel soothed.";
#endif

			break;
		case 68: case 69:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "�_���W�����ƒn��̊Ԃ��e���|�[�g������@��Y�ꂽ�B";
#else
			muta_desc = "You feel briefly homesick.";
#endif

			break;
		case 70:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ID_MUSHROOM;
#ifdef JP
muta_desc = "�L�m�R�̓����̋�ʂ����Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel a holy wrath.";
#endif

			break;
		case 71: case 72:
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DANMAKU;
#ifdef JP
muta_desc = "�w������̗͂������Ȃ��Ȃ����B";
#else
			muta_desc = "Your hands warm up.";
#endif

			break;
/*
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "���𓊂���肪�キ�Ȃ����C������B";
#else
			muta_desc = "Your throwing arm feels much weaker.";
#endif

			break;
*/
		case 75:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "�������������������B";
#else
			muta_desc = "You are no longer subject to fits of berserk rage!";
#endif

			break;
		case 76:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "�����|�ǂ��������I";
#else
			muta_desc = "You are no longer an incredible coward!";
#endif

			break;
		case 77:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "���Ȃ��̈ӎ��͖��Ă������߂����B";
#else
			muta_desc = "Your position seems more certain.";
#endif

			break;
		case 78:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "�A���R�[�����ł��������I";
#else
			muta_desc = "Your body stops producing alcohol!";
#endif

			break;
		case 79:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "���o���Ђ��N�������_��Q���N�����Ȃ��Ȃ����I";
#else
			muta_desc = "You are no longer afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_YAKU;
#ifdef JP
muta_desc = "��������ꂽ�C������B";
#else
			muta_desc = "You are no longer subject to uncontrollable flatulence.";
#endif

			break;
		case 81: case 82:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HARDHEAD;
#ifdef JP
muta_desc = "�����_�炩���Ȃ����I";
#else
			muta_desc = "You lose your scorpion tail!";
#endif

			break;
		case 83: case 84:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�z����p���������I";
#else
			muta_desc = "Your horns vanish from your forehead!";
#endif

			break;
		case 85: case 86:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIGHORN;
#ifdef JP
muta_desc = "�p�������������B";
#else
			muta_desc = "Your mouth reverts to normal!";
#endif

			break;
		case 87: case 88:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "�f�[�����������񂹂Ȃ��Ȃ����B";
#else
			muta_desc = "You stop attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "����s�\�Ȗ��@�̃G�l���M�[�𔭐����Ȃ��Ȃ����B";
#else
			muta_desc = "You stop producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "�N�T���łȂ��Ȃ����B";
#else
			muta_desc = "You are no longer manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "�w��ɋ��낵���͂������Ȃ��Ȃ����B";
#else
			muta_desc = "You no longer feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "���E�����邢�Ɗ�����B";
#else
			muta_desc = "You feel the world's a brighter place.";
#endif

			break;
		case 95: case 96:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TAIL;
#ifdef JP
muta_desc = "�K�����������B";
#else
			muta_desc = "Your nose returns to a normal length.";
#endif

			break;
		case 97:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "�����������񂹂Ȃ��Ȃ����B";
#else
			muta_desc = "You stop attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIGTAIL;
#ifdef JP
muta_desc = "�K�����������B";
#else
			muta_desc = "Your tentacles vanish from your sides.";
#endif

			break;
		case 99:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "���͂̋�Ԃ����肵���C������B";
#else
			muta_desc = "You feel the universe is more stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "�ʏ�̑̂ɖ߂��Ă����������Ȃ��Ȃ����B";
#else
			muta_desc = "You feel normally strange.";
#endif

			break;
		case 103:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "���Ȃ��͕������E�ɂ������葶�݂��Ă���B";
#else
			muta_desc = "You are firmly in the physical world.";
#endif

			break;
		case 104:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_GHOST;
#ifdef JP
muta_desc = "�H��̋������䂩�Ȃ��Ȃ����C������B";
#else
			muta_desc = "You feel forces of chaos departing your old scars.";
#endif

			break;
		case 105:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_KWAI;
#ifdef JP
muta_desc = "�d���̋������䂩�Ȃ��Ȃ����C������B";
#else
			muta_desc = "You are cured of the horrible wasting disease!";
#endif

			break;
		case 106:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "�h���S���������񂹂Ȃ��Ȃ����B";
#else
			muta_desc = "You stop attracting dragons.";
#endif

			break;
		case 107: case 108:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_XOM;
#ifdef JP
muta_desc = "�]���͂��Ȃ��ɋ������������悤���B";
#else
			muta_desc = "Your thoughts return to boring paths.";
#endif

			break;
		case 109:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BIRDBRAIN;
#ifdef JP
muta_desc = "�������������B";
#else
			muta_desc = "Your stomach stops roiling.";
#endif

			break;
		case 110: case 111:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "���ׂ̐_�X�̋������䂩�Ȃ��Ȃ����B";
#else
			muta_desc = "You lose the attention of the chaos deities.";
#endif

			break;
		case 112:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "�������E�ɕ߂���Ă���C������B";
#else
			muta_desc = "You feel like you're trapped in reality.";
#endif

			break;
		case 113: case 114:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "�p���m�C�A�łȂ��Ȃ����B";
#else
			muta_desc = "You no longer feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "���G��Ԃ̔�����N�����Ȃ��Ȃ����B";
#else
			muta_desc = "You are no longer blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ASTHMA;
#ifdef JP
muta_desc = "�b�����������B";
#else
			muta_desc = "You are no longer subject to fits of magical healing.";
#endif

			break;
		case 118:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_THE_WORLD;
#ifdef JP
muta_desc = "�~�܂������̐��E�ɖ������܂Ȃ��Ȃ����B";
#else
			muta_desc = "You are no longer subject to fits of painful clarity.";
#endif

			break;
		case 119:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "�]�т₷���Ȃ��Ȃ����C������B";
#else
			muta_desc = "Your feet shrink to their former size.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "�ؓ������ʂɖ߂����B";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "�ؓ������ʂɖ߂����B";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "�]�����ʂɖ߂����B";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 129: case 130: case 131:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "�]�����ʂɖ߂����B";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 132: case 133:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "���ʂ̏�v���ɖ߂����B";
#else
			muta_desc = "You become ordinarily resilient again.";
#endif

			break;
		case 134: case 135:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WARNING;
#ifdef JP
muta_desc = "���������Ȃ����C������c";
#else
			muta_desc = "You benefit from a miracle diet!";
#endif

			break;
		case 136: case 137:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "����̎����������I";
#else
			muta_desc = "You are no longer an albino!";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SPEEDSTER;
#ifdef JP
muta_desc = "�r�͂����ɖ߂����B";
#else
			muta_desc = "Your flesh is no longer afflicted by a rotting disease!";
#endif

			break;
		case 141: case 142:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RES_WATER;
#ifdef JP
muta_desc = "�x���ʂ����ʂɖ߂����B";
#else
			muta_desc = "Your voice returns to normal.";
#endif

			break;
		case 143: case 144:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "��ɖڕ@���߂����B";
#else
			muta_desc = "Your facial features return.";
#endif

			break;
		case 145:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HURT_LITE;
#ifdef JP
muta_desc = "�������ł͂Ȃ��Ȃ����B";
#else
			muta_desc = "You stop projecting a reassuring image.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
muta_desc = "�z�̖ڂ������Ă��܂����I";
#else
			muta_desc = "Your extra eyes vanish!";
#endif

			break;
		case 149: case 150:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "���@�Ɏキ�Ȃ����B";
#else
			muta_desc = "You become susceptible to magic again.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "��ȉ��𗧂ĂȂ��Ȃ����I";
#else
			muta_desc = "You stop making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BYDO;
#ifdef JP
muta_desc = "�o�C�h�זE�̐N�H���~�܂����B";
#else
			muta_desc = "Your infravision is degraded.";
#endif

			break;
		case 157: case 158:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FISH_TAIL;
#ifdef JP
muta_desc = "�����g����Ԃɖ߂����B";
#else
			muta_desc = "Your extra legs disappear!";
#endif

			break;
		case 159: case 160:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PEGASUS;
#ifdef JP
muta_desc = "���^�����ʂɖ߂����B";
#else
			muta_desc = "Your legs lengthen to normal.";
#endif

			break;
		case 161: case 162:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ACID_SCALES;
#ifdef JP
muta_desc = "�������ɖ߂����B";
#else
			muta_desc = "Electricity stops running through you.";
#endif

			break;
		case 163: case 164:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ELEC_SCALES;
#ifdef JP
muta_desc = "�������ɖ߂����B";
#else
			muta_desc = "Your body is no longer enveloped in flames.";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FIRE_SCALES;
#ifdef JP
muta_desc = "�������ɖ߂����B";
#else
			muta_desc = "Your warts disappear!";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_COLD_SCALES;
#ifdef JP
muta_desc = "�������ɖ߂����B";
#else
			muta_desc = "Your scales vanish!";
#endif

			break;
		case 171: case 172:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_POIS_SCALES;
#ifdef JP
muta_desc = "�������ɖ߂����B";
#else
			muta_desc = "Your skin reverts to flesh!";
#endif

			break;
		case 173: case 174:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "�w���̉H������ꗎ�����B";
#else
			muta_desc = "Your wings fall off.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "�Ăы��|��������悤�ɂȂ����B";
#else
			muta_desc = "You begin to feel fear again.";
#endif

			break;
		case 178: case 179:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEVER;
#ifdef JP
muta_desc = "�M�a���������B";
#else
			muta_desc = "You stop regenerating.";
#endif

			break;
		case 180: case 181:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "�e���p�V�[�̔\�͂��������I";
#else
			muta_desc = "You lose your telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "�ؓ����d���Ȃ����B";
#else
			muta_desc = "Your muscles stiffen.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "�֐߂��ɂ��Ȃ��Ȃ����B";
#else
			muta_desc = "Your joints stop hurting.";
#endif

			break;
		case 188:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "�����I�[���͉Q�����ď������B";
#else
			muta_desc = "Your black aura swirls and fades.";
#endif

			break;
		case 189:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "���h���Ȋ����͂Ȃ��Ȃ����B";
#else
			muta_desc = "You feel less exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "����̐��m�����Ȃ��Ȃ����B";
#else
			muta_desc = "You move with less assurance.";
#endif

			break;
		case 193:
			if (p_ptr->pseikaku == SEIKAKU_LUCKY) break;
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "�����I�[���͋P���ď������B";
#else
			muta_desc = "Your white aura shimmers and fades.";
#endif

			break;


		case 194:	case 195:	case 196:
		case 197:	case 198:	case 199:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_WOLF;
			muta_desc = "�I�I�J�~�̓�����͏o�Ă������B";
			break;

		case 200:	case 201:	case 202:
		case 203:	case 204:	case 205:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_OTTER;
			muta_desc = "�J���E�\�̓�����͏o�Ă������B";
			break;

		case 206:	case 207:	case 208:
		case 209:	case 210:	case 211:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_EAGLE;
			muta_desc = "�I�I���V�̓�����͏o�Ă������B";
			break;

		case 212:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_JELLYFISH;
			muta_desc = "�N���Q�̓�����͏o�Ă������B";
			break;

		case 213:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_COW;
			muta_desc = "�E�V�̓�����͏o�Ă������B";
			break;

		case 214:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_CHICKEN;
			muta_desc = "�q���R�̓�����͏o�Ă������B";
			break;

		case 215:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_TORTOISE;
			muta_desc = "�J���̓�����͏o�Ă������B";
			break;

		case 216:
			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANIWA;
			muta_desc = "�n�j���́c�c�����H�c�c��H�͏o�Ă������B";
			break;

		case 217:

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_HANGOKUOH;
			muta_desc = "�����͂ǂ����H�L�������ł���C������c";
			break;

		case 218:

			muta_class = &(p_ptr->muta4);
			muta_which = MUT4_GHOST_CHEERS;
			muta_desc = "���삽���̗͂������������I";
			break;



		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (*(muta_class) & muta_which)
			{
				muta_chosen = TRUE;
			}

			///mod140318 �i���ψفA�����ψق͓ˑR�ψَ��Â̑ΏۂɂȂ�Ȃ�
			if(muta_class == &(p_ptr->muta1) && (muta_which & p_ptr->muta1_perma))	muta_chosen = FALSE;
			if(muta_class == &(p_ptr->muta2) && (muta_which & p_ptr->muta2_perma))	muta_chosen = FALSE;
			if(muta_class == &(p_ptr->muta3) && (muta_which & p_ptr->muta3_perma))	muta_chosen = FALSE;
			if(muta_class == &(p_ptr->muta4) && (muta_which & p_ptr->muta4_perma))	muta_chosen = FALSE;

		}

		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
		return FALSE;
	}
	/*:::�ˑR�ψق������B�t���O�𑀍삵�Đ����x���Čv�Z�B*/
	else
	{
		msg_print(muta_desc);
		*(muta_class) &= ~(muta_which);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		return TRUE;
	}
}



///mutation �ˑR�ψوꗗ��\������
void dump_mutations(FILE *OutFile)
{
	if (!OutFile) return;

	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_BR_ACID)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎_�̃u���X��f�����Ƃ��ł���B(�_���[�W ���x��X2)\n");
#else
			fprintf(OutFile, " You can spit acid (dam lvl).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͉��̃u���X��f�����Ƃ��ł���B(�_���[�W ���x��X2)\n");
#else
			fprintf(OutFile, " You can breathe fire (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ����ɂ݂͖������ʂ����B\n");
#else
			fprintf(OutFile, " Your gaze is hypnotic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͔O���͂������Ă���B\n");
#else
			fprintf(OutFile, " You are telekinetic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����̈ӎv�Ńe���|�[�g�ł���B\n");
#else
			fprintf(OutFile, " You can teleport at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͓G�𐸐_�U���ł���B\n");
#else
			fprintf(OutFile, " You can Mind Blast your enemies.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_ELEC)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͗��̃u���X��f�����Ƃ��ł���B(�_���[�W ���x��X2)\n");
#else
			fprintf(OutFile, " You can emit hard radiation at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋z���S�̂悤�ɓG���琶���͂��z�����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can drain life from a foe like a vampire.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_ACCELERATION)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͈ꎞ�I�ɉ������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can smell nearby precious metal.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BEAST_EAR)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͉s�q�Ȓ��o�ŋ߂��̃����X�^�[�����m�ł���B\n");
#else
			fprintf(OutFile, " You can smell nearby monsters.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͒Z���������e���|�[�g�ł���B\n");
#else
			fprintf(OutFile, " You can teleport yourself short distances.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͍d�����H�ׂ邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can consume solid rock.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͑��̎҂Əꏊ�����ւ�邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can switch locations with another being.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_SOUND)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͑吺�œG���U���ł���B(�_���[�W �ő�Ń��x��X2)\n");
#else
			fprintf(OutFile, " You can emit a horrible shriek.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_COLD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐����f�����Ƃ��ł���B(�_���[�W ���x��X2)\n");
#else
			fprintf(OutFile, " You can emit bright light.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DET_ITEM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋߂��ɗ����Ă���A�C�e�������m���邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can feel the danger of evil magic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����̈ӎv�ŋ����퓬��ԂɂȂ邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can drive yourself into a berserk frenzy.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_POIS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓K�X�̃u���X��f�����Ƃ��ł���B(�_���[�W ���x��X2)\n");
#else
			fprintf(OutFile, " You can polymorph yourself at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͒ʏ�A�C�e�������ɕς��邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can turn ordinary items to gold.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��͂ɃL�m�R�𐶂₷���Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can cause mold to grow near you.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͌��f�̍U���ɑ΂��Đg���d�����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can harden yourself to the ravages of the elements.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��͂̃_���W��������󂳂��邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can bring down the dungeon around your ears.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͖��@�̃G�l���M�[�������̕��Ƃ��Ďg�p�ł���B\n");
#else
			fprintf(OutFile, " You can consume magic energy for your own use.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RAISE_WOOD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎����̂��鏊�ɐX�т𐶐����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can feel the strength of the magics affecting you.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͍�����t���A�Ń����X�^�[�̑��B��j�~���邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can cause mass impotence.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͍U��������g����邽�ߓ����邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can run for your life after hitting something.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̊�͋��C�������炷�B \n");
#else
			fprintf(OutFile, " You can emit confusing, blinding radiation.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͖ڂ���r�[���������Ƃ��ł���B\n");
#else
			fprintf(OutFile, " Your eyes can fire laser beams.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͊X�ƃ_���W�����̊Ԃ��s�������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can travel between town and the depths.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_ID_MUSHROOM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓L�m�R�̊Ӓ肪�ł���B\n");
#else
			fprintf(OutFile, " You can send evil creatures directly to Hell.\n");
#endif

		}
		//v1.1.39
		if (p_ptr->muta1 & MUT1_DANMAKU)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋G�߂̗͂����͂Ȓe���Ƃ��ĕ����Ƃ��ł���B(����x5)\n");
#else
			fprintf(OutFile, " You can freeze things with a touch.\n");
#endif

		}
/*
		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓A�C�e����͋��������邱�Ƃ��ł���B\n");
#else
			fprintf(OutFile, " You can hurl objects with great force.\n");
#endif

		}
*/
	}

	if (p_ptr->muta2)
	{
		if (p_ptr->muta2 & MUT2_BERS_RAGE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͋���m���̔�����N�����B\n");
#else
			fprintf(OutFile, " You are subject to berserker fits.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_COWARDICE)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X���Q�ɏP����B\n");
#else
			fprintf(OutFile, " You are subject to cowardice.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X�ӎ����B���ɂȂ�B\n");
#else
			fprintf(OutFile, " You are teleporting randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ALCOHOL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓A���R�[�����ł��B(���݉߂��Ă��|��Ȃ��������؂��ƑS�\��-3 )\n");
#else
			fprintf(OutFile, " Your body produces alcohol.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HALLU)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͌��o�������N�������_�����ɐN����Ă���B\n");
#else
			fprintf(OutFile, " You have a hallucinatory insanity.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_YAKU)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̎���ɂ͖�W�܂��Ă���B\n");
#else
			fprintf(OutFile, " You are subject to uncontrollable flatulence.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_PROD_MANA)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐���s�\�Ȗ��@�̃G�l���M�[�𔭂��Ă���B\n");
#else
			fprintf(OutFile, " You are producing magical energy uncontrollably.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DEMON)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓f�[��������������B\n");
#else
			fprintf(OutFile, " You attract demons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HARDHEAD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͐Γ����B(�m�\-1�A����+1�AAC+5)\n");
#else
			fprintf(OutFile, " You have a scorpion tail (poison, 3d7).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HORNS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͊p�������Ă���B\n");
#else
			fprintf(OutFile, " You have horns (dam. 2d6).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BIGHORN)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͗��h�Ȋp�������Ă���B\n");
#else
			fprintf(OutFile, " You have a beak (dam. 2d4).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SPEED_FLUX)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓����_���ɑ�����������x���������肷��B\n");
#else
			fprintf(OutFile, " You move faster or slower randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BANISH_ALL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X�߂��̃����X�^�[�����ł�����B\n");
#else
			fprintf(OutFile, " You sometimes cause nearby creatures to vanish.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_EAT_LIGHT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X���͂̌����z�����ĉh�{�ɂ���B\n");
#else
			fprintf(OutFile, " You sometimes feed off of the light around you.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_TAIL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂��Ȃ₩�ȐK���������Ă���B(��p+4)\n");
#else
			fprintf(OutFile, " You have an elephantine trunk (dam 1d4).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_ANIMAL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͓�������������B\n");
#else
			fprintf(OutFile, " You attract animals.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BIGTAIL)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͗��̂悤�ȑ傫�ȐK���������Ă���B\n");
#else
			fprintf(OutFile, " You have evil looking tentacles (dam 2d5).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RAW_CHAOS)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂��΂��Ώ��J�I�X�ɕ�܂��B\n");
#else
			fprintf(OutFile, " You occasionally are surrounded with raw chaos.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_NORMALITY)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂Ƃ��ǂ��ψق���񕜂���B\n");
#else
			fprintf(OutFile, " You may be mutated, but you're recovering.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WRAITH)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓��̂͗H�̉���������̉������肷��B\n");
#else
			fprintf(OutFile, " You fade in and out of physical reality.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_GHOST)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͗H�����������B\n");
#else
			fprintf(OutFile, " Your health is subject to chaotic forces.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_KWAI)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͗d������������B\n");
#else
			fprintf(OutFile, " You have a horrible wasting disease.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DRAGON)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓h���S������������B\n");
#else
			fprintf(OutFile, " You attract dragons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_XOM)
		{
#ifdef JP
if(p_ptr->lev > 24) fprintf(OutFile, " ���Ȃ��̓]���̂��C�ɓ���̊ߋ�B\n");
else fprintf(OutFile, " ���Ȃ��̓]���̊ߋ�B\n");
#else
			fprintf(OutFile, " Your mind randomly expands and contracts.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BIRDBRAIN)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂����ɕ�����Y���B\n");
#else
			fprintf(OutFile, " You have a seriously upset stomach.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̓J�I�X�̎�숫������J���������Ƃ�B\n");
#else
			fprintf(OutFile, " Chaos deities give you gifts.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WALK_SHAD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂��΂��Α��́u�e�v�ɖ������ށB\n");
#else
			fprintf(OutFile, " You occasionally stumble into other shadows.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WARNING)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͓G�Ɋւ���x����������B\n");
#else
			fprintf(OutFile, " You receive warnings about your foes.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_INVULN)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͎��X�����m�炸�ȋC���ɂȂ�B\n");
#else
			fprintf(OutFile, " You occasionally feel invincible.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ASTHMA)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͚b���������B\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your muscles.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_THE_WORLD)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��̎���Ŏ��X���Ԃ���~����B\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your head.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_DISARM)
		{
#ifdef JP
fprintf(OutFile, " ���Ȃ��͂悭�܂Â��ĕ��𗎂Ƃ��B\n");
#else
			fprintf(OutFile, " You occasionally stumble and drop things.\n");
#endif

		}
	}

	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͒��l�I�ɋ����B(�r��+4)\n");
#else
			fprintf(OutFile, " You are superhumanly strong (+4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_PUNY)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͋��ゾ�B(�r��-4)\n");
#else
			fprintf(OutFile, " You are puny (-4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̔]�͐��̃R���s���[�^���B(�m�\������+4)\n");
#else
			fprintf(OutFile, " Your brain is a living computer (+4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MORONIC)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͐��_���ゾ�B(�m�\������-4)\n");
#else
			fprintf(OutFile, " You are moronic (-4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̑̂͒e�͐��ɕx��ł���B(�ϋv+4)\n");
#else
			fprintf(OutFile, " You are very resilient (+4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WARNING)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͊����s���g�ɔ������댯���@�m�ł���B\n");
#else
			fprintf(OutFile, " You are extremely fat (+2 CON, -2 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ALBINO)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͋���̎����B(�ϋv-4)\n");
#else
			fprintf(OutFile, " You are albino (-4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SPEEDSTER)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͋��x�ȋr�͂������Ă���B\n");
#else
			fprintf(OutFile, " Your flesh is rotting (-2 CON, -1 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_RES_WATER)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͐��U���ɑϐ������B\n");
#else
			fprintf(OutFile, " Your voice is a silly squeak (-4 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͂̂��؂�ڂ����B(����-3)\n");
#else
			fprintf(OutFile, " Your face is featureless (-1 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_HURT_LITE)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͖��邢������肾�B\n");
#else
			fprintf(OutFile, " Your appearance is masked with illusion.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
#ifdef JP
			if (p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
				fprintf(OutFile, " ���Ȃ��͊z�ɑ�l�̖ڂ������Ă���B\n");
			else
				fprintf(OutFile, " ���Ȃ��͊z�ɑ�O�̖ڂ������Ă���B\n");
#else
			fprintf(OutFile, " You have an extra pair of eyes (+15 search).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͖��@�ւ̑ϐ��������Ă���B\n");
#else
			fprintf(OutFile, " You are resistant to magic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͕ςȉ��𔭂��Ă���B(�B��-3)\n");
#else
			fprintf(OutFile, " You make a lot of strange noise (-3 stealth).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BYDO)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̓o�C�h�זE�ɐZ�H����Ă���B(�S�\�͏㏸�A���͑啝�����A�����@�O���ȓ��Ɏ��Â��Ȃ��ƁE�E�H)\n");
#else
			fprintf(OutFile, " You have remarkable infravision (+3).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FISH_TAIL)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̉����g�͐l���̐K�����B\n");
#else
			fprintf(OutFile, " You have an extra pair of legs (+3 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_PEGASUS)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̓��͏��V�y�K�T�XMIX���肾�B(AC+5 ���h��𑕔��ł��Ȃ�)\n");
#else
			fprintf(OutFile, " Your legs are short stubs (-3 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ACID_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̔��͍����؂ɕ����Ă���B(AC+5 �_�ϐ� ����-2)\n");
#else
			fprintf(OutFile, " Electricity is running through your veins.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ELEC_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̔��͐��؂ɕ����Ă���B(AC+5 �d�ϐ� ����-2)\n");
#else
			fprintf(OutFile, " Your body is enveloped in flames.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FIRE_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̔��͐Ԃ��؂ɕ����Ă���B(AC+5 �Αϐ� ����-2)\n");
#else
			fprintf(OutFile, " Your skin is covered with warts (-2 CHR, +5 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_COLD_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̔��͔����؂ɕ����Ă���B(AC+5 ��ϐ� ����-2)\n");
#else
			fprintf(OutFile, " Your skin has turned into scales (-1 CHR, +10 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_POIS_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̔��͗΂̗؂ɕ����Ă���B(AC+5 �őϐ� ����-2)\n");
#else
			fprintf(OutFile, " Your skin is made of steel (-1 DEX, +25 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WINGS)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͉H�������Ă���B\n");
#else
			fprintf(OutFile, " You have wings.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͑S�����|�������Ȃ��B\n");
#else
			fprintf(OutFile, " You are completely fearless.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FEVER)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͌����s���̔M�a�ɖ`����Ă���B(�m�\��p-2)\n");
#else
			fprintf(OutFile, " You are regenerating.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ESP)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̓e���p�V�[�������Ă���B\n");
#else
			fprintf(OutFile, " You are telepathic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_LIMBER)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͔̑̂��ɂ��Ȃ₩���B(��p+3)\n");
#else
			fprintf(OutFile, " Your body is very limber (+3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͂����֐߂ɒɂ݂������Ă���B(��p-3)\n");
#else
			fprintf(OutFile, " Your joints ache constantly (-3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_VULN_ELEM)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͌��f�̍U���Ɏア�B\n");
#else
			fprintf(OutFile, " You are susceptible to damage from the elements.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MOTION)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��̓���͐��m�ŗ͋����B(�B��+1)\n");
#else
			fprintf(OutFile, " Your movements are precise and forceful (+1 STL).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_GOOD_LUCK)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͔����I�[���ɂ܂�Ă���B\n");
#else
			fprintf(OutFile, " There is a white aura surrounding you.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BAD_LUCK)
		{
#ifdef JP
			fprintf(OutFile, " ���Ȃ��͍����I�[���ɂ܂�Ă���B\n");
#else
			fprintf(OutFile, " There is a black aura surrounding you.\n");
#endif
		}

	}
	if(p_ptr->muta4)
	{
		//v1.1.64
		if (p_ptr->muta4 & MUT4_GHOST_WOLF)
		{
			fprintf(OutFile, " ���Ȃ��̓I�I�J�~�̓�����ɜ߈˂���Ă���B(���̋���/�m�\�����ቺ)\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_OTTER)
		{
			fprintf(OutFile, " ���Ȃ��̓J���E�\�̓�����ɜ߈˂���Ă���B(�Z�\�㏸/�S�\�͂��ቺ)\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_EAGLE)
		{
			fprintf(OutFile, " ���Ȃ��̓I�I���V�̓�����ɜ߈˂���Ă���B(�m�\�����T���㏸/�r�͑ϋv�ቺ\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_JELLYFISH)
		{
			fprintf(OutFile, " ���Ȃ��̓N���Q�̓�����ɜ߈˂���Ă���B(�Ŗ�ა��ϐ�/�r�͑啝�ቺ\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_COW)
		{
			fprintf(OutFile, " ���Ȃ��̓E�V�̓�����ɜ߈˂���Ă���B(�r�͑ϋv�啝�㏸/�B���啝�ቺ)\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_CHICKEN)
		{
			fprintf(OutFile, " ���Ȃ��̓q���R�̓�����ɜ߈˂���Ă���B(���͑啝�㏸/AC�ቺ)\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_TORTOISE)
		{
			fprintf(OutFile, " ���Ȃ��̓J���̓�����ɜ߈˂���Ă���B(AC�啝�㏸/�����ቺ)\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
		{
			fprintf(OutFile, " ���Ȃ��̓n�j���̓����c�H��c�H�ɜ߈˂���Ă���B(���@�h��啝�㏸/�����j�Ў�_25%%)\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
		{
			fprintf(OutFile, " ���Ȃ��͑剅��ɏ������Ă���...(�n���ϐ�/�j�׎�_/�����G�i�W�[�h���C��)\n");
		}
		if (p_ptr->muta4 & MUT4_GHOST_CHEERS)
		{
			fprintf(OutFile, " ���Ȃ��͉��삽���ɉ�������Ă���B(�ߐڔ\�͏㏸/�ꕔ���Z�̑����ύX/�j�ב������󂯂�Ɖ���)\n");
		}



	}
}


/*
 * List mutations we have...
 */
void do_cmd_knowledge_mutations(void)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if (fff) dump_mutations(fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "�ˑR�ψ�", 0, 0);
#else
	show_file(TRUE, file_name, "Mutations", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


int count_bits(u32b x)
{
	int n = 0;

	if (x) do
	{
		n++;
	}
	while (0 != (x = x&(x-1)));

	return (n);
}


int count_mutations(void)
{
	return (count_bits(p_ptr->muta1) +
		count_bits(p_ptr->muta2) +
		count_bits(p_ptr->muta3) +
		count_bits(p_ptr->muta4));
}


/*
 * Return the modifier to the regeneration rate
 * (in percent)
 */
/*:::�ˑR�ψقɂ��Đ����x�ω����v�Z�@�ő�l��100�Ƃ��Ă���*/
int calc_mutant_regenerate_mod(void)
{
	int regen;
	int mod = 10;
	int count = count_mutations();

	int count_perma;

	count_perma = (count_bits(p_ptr->muta1_perma) + count_bits(p_ptr->muta2_perma) + count_bits(p_ptr->muta3_perma) + count_bits(p_ptr->muta4_perma));

	///mod140318 �Đ����x�v�Z�ɉi���ψق␶���^�ψق��܂܂Ȃ�
	count -= count_perma;

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */
	
	if (p_ptr->pseikaku == SEIKAKU_LUCKY) count--;
	/*:::�b�l�̓y�i���e�B�����Ȃ��@10�ڈȍ~���y�i���e�B����*/
	/*
	if (p_ptr->prace == RACE_BEASTMAN)
	{
		count -= 10;
		mod = 5;
	}
	*/
	//v1.1.39 �����o�͓ˑR�ψقɉe������Ȃ�
	if (p_ptr->pclass == CLASS_LARVA) count = 0;

	/* No negative modifier */
	if (count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if (regen < 10) regen = 10;

	return (regen);
}

/*:::�����g�������̃R�}���h�ł�������s�@�̂Ɖr���𒆒f����*/
void mutation_stop_mouth()
{
	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any()) stop_hex_spell_all();
}

/*:::�����^�ψق̎��s*/
//����:MUT1�ȊO�̔����ψق�cmd_racial_power_aux()�֍s��
bool mutation_power_aux(u32b power)
{
	int     dir = 0;
	int     lvl = p_ptr->lev;

	if (hack_flag_powerup_mutation) lvl *= 2;

	switch (power)
	{
		case MUT1_BR_ACID:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("���Ȃ��͎_�̃u���X��f����...");
#else
			msg_print("You spit acid...");
#endif
			//v1.1.39 �u���X�`���
			fire_ball(GF_ACID, dir, lvl * 2, -2);
			break;

		case MUT1_BR_FIRE:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("���Ȃ��͉Ή��̃u���X��f����...");
#else
			msg_print("You breathe fire...");
#endif

			//v1.1.39 �u���X�`���
			fire_ball(GF_FIRE, dir, lvl * 2, -2);
			break;

		case MUT1_HYPN_GAZE:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���Ȃ��͌��f�I�Ȋ���ɂ݂���...");
#else
			msg_print("Your eyes look mesmerizing...");
#endif

			(void)charm_monster(dir, lvl);
			break;

		case MUT1_TELEKINES:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�W�����Ă���...");
#else
			msg_print("You concentrate...");
#endif

			fetch(dir, lvl * 10, TRUE);
			break;

		case MUT1_VTELEPORT:
#ifdef JP
			msg_print("�W�����Ă���...");
#else
			msg_print("You concentrate...");
#endif

			teleport_player(10 + 4 * lvl, 0L);
			break;

		case MUT1_MIND_BLST:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�W�����Ă���...");
#else
			msg_print("You concentrate...");
#endif

			fire_bolt(GF_PSI, dir, damroll(3 + ((lvl - 1) / 5), 3));
			break;

		case MUT1_BR_ELEC:
			if (!get_aim_dir(&dir)) return FALSE;

#ifdef JP
			msg_print("���Ȃ��͗��̃u���X��f����...");
#else
			msg_print("Radiation flows from your body!");
#endif
			mutation_stop_mouth();

			//v1.1.39 �u���X�`���
			fire_ball(GF_ELEC, dir, lvl * 2, -2);
			break;

		case MUT1_VAMPIRISM:
			{
				int x, y, dummy;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir2(&dir)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				mutation_stop_mouth();

				if (!(c_ptr->m_idx))
				{
#ifdef JP
					msg_print("�����Ȃ��ꏊ�Ɋ��݂����I");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���Ȃ��̓j�����Ƃ��ĉ���ނ���...");
#else
				msg_print("You grin and bare your fangs...");
#endif


				dummy = lvl * 2;

				if (drain_life(dir, dummy))
				{
					if (p_ptr->food < PY_FOOD_FULL || p_ptr->pclass == CLASS_YUMA)
						/* No heal if we are "full" */
						(void)hp_player(dummy);
					else
#ifdef JP
						msg_print("���Ȃ��͋󕠂ł͂���܂���B");
#else
						msg_print("You were not hungry.");
#endif

					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = p_ptr->food + MIN(5000, 100 * dummy);
					if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX-1 : dummy);
				}
				else
#ifdef JP
					msg_print("�����I�Ђǂ������B");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;

		case MUT1_ACCELERATION:
			mutation_stop_mouth();
		//	(void)detect_treasure(DETECT_RAD_DEFAULT);
			set_fast(30, FALSE);

			break;

		case MUT1_BEAST_EAR:
			msg_print("���Ȃ��͂����Ǝ��𐟂܂���...");
			mutation_stop_mouth();
			(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
			(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
			break;

		case MUT1_BLINK:
			teleport_player(10, 0L);
			break;
			/*:::�ǐH��*/
		case MUT1_EAT_ROCK:
			if(!eat_rock()) return FALSE;

			break;

		case MUT1_SWAP_POS:
			project_length = -1;
			if (!get_aim_dir(&dir))
			{
				project_length = 0;
				return FALSE;
			}
			(void)teleport_swap(dir);
			project_length = 0;
			break;

		case MUT1_BR_SOUND:
			mutation_stop_mouth();

			if(one_in_(128))msg_format("�u�@�@�ق��@�@�v");
			else if(one_in_(128))msg_format("�u�Ȃ�ł��[�Ȃ�́I�v");
			else if(one_in_(2))	msg_format("�u�΁@���@���@�́@�|�@�I�v");
			else 	msg_format("�uYah��o !�v");


			(void)fire_ball(GF_SOUND, 0, lvl * 4, 8);

			
			//	(void)fire_ball(GF_SOUND, 0, 2 * lvl, 8);
			(void)aggravate_monsters(0,FALSE);
			break;

		case MUT1_BR_COLD:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("���Ȃ��͗�C�̃u���X��f����...");
#else
			msg_print("You breathe fire...");
#endif

			fire_ball(GF_COLD, dir, lvl * 2, -2);
			break;
		case MUT1_DET_ITEM:
			{
			msg_print("���Ȃ��̓_�E�W���O���n�߂�...");
			detect_objects_normal(DETECT_RAD_DEFAULT);
				/*
				int i;
				for (i = 0; i < INVEN_TOTAL; i++)
				{
					object_type *o_ptr = &inventory[i];

					if (!o_ptr->k_idx) continue;
					if (!object_is_cursed(o_ptr)) continue;

					o_ptr->feeling = FEEL_CURSED;
				}
				*/
			}
			break;

		case MUT1_BERSERK:
			(void)set_shero(randint1(25) + 25, FALSE);
			(void)hp_player(30);
			(void)set_afraid(0);
			break;

		case MUT1_BR_POIS:
			if (!get_aim_dir(&dir)) return FALSE;
			mutation_stop_mouth();
#ifdef JP
			msg_print("���Ȃ��̓K�X�̃u���X��f����...");
#else
			msg_print("You breathe fire...");
#endif

			fire_ball(GF_POIS, dir, lvl * 2, -2);
			break;
			
			
/*
#ifdef JP
			if (!get_check("�ϐg���܂��B��낵���ł����H")) return FALSE;
#else
			if (!get_check("You will polymorph your self. Are you sure? ")) return FALSE;
#endif
			do_poly_self();
			break;
*/
		case MUT1_MIDAS_TCH:
			if (!alchemy(0)) return FALSE;
			break;

		/* Summon pet molds around the player */
		case MUT1_GROW_MOLD:
			{
				int i;
				for (i = 0; i < 8; i++)
				{
					summon_specific(-1, py, px, lvl, SUMMON_BIZARRE1, PM_FORCE_PET);
				}
			}
			break;

		case MUT1_RESIST:
			{
				int num = lvl / 10;
				int dur = randint1(20) + 20;

				if (num > 5) num = 5;

				if (randint0(5) < num)
				{
					(void)set_oppose_acid(dur, FALSE);
					num--;
				}
				if (randint0(4) < num)
				{
					(void)set_oppose_elec(dur, FALSE);
					num--;
				}
				if (randint0(3) < num)
				{
					(void)set_oppose_fire(dur, FALSE);
					num--;
				}
				if (randint0(2) < num)
				{
					(void)set_oppose_cold(dur, FALSE);
					num--;
				}
				if (num)
				{
					(void)set_oppose_pois(dur, FALSE);
					num--;
				}
			}
			break;

		case MUT1_EARTHQUAKE:
			(void)earthquake(py, px, 10);
			break;

		case MUT1_EAT_MAGIC:
			if (!eat_magic(p_ptr->lev * 2)) return FALSE;
			break;

		case MUT1_RAISE_WOOD:
			{

			if (cave_clean_bold(py, px)){
				msg_print("���Ȃ��̑�������؂������Ă����I");
				cave_set_feat(py, px, feat_tree);
			}
			else
				msg_print("�����ł͐A�����炽�Ȃ��I");

		//	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
		//	project(0, 1, py, px, 0, GF_MAKE_TREE, flg, -1);

			//report_magics();
			break;
			}
			/*:::�W�c�I���B�s�Ǖψ�*/
		case MUT1_STERILITY:
			/* Fake a population explosion. */
#ifdef JP
			msg_print("���Ȃ��̉��l�̐����_���W�������ɋ����n�����E�E�E");
			take_hit(DAMAGE_LOSELIFE, randint1(17) + 17, "�֗~����������J", -1);
#else
			msg_print("You suddenly have a headache!");
			take_hit(DAMAGE_LOSELIFE, randint1(17) + 17, "the strain of forcing abstinence", -1);
#endif

			num_repro += MAX_REPRO;
			break;
			/*:::H&A�ψ�*/
		case MUT1_PANIC_HIT:
			{
				int x, y;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if (cave[y][x].m_idx)
				{
					py_attack(y, x, 0);
					if (randint0(p_ptr->skill_dis) < 7)
#ifdef JP
						msg_print("���܂��������Ȃ������B");
#else
						msg_print("You failed to teleport.");
#endif
					else teleport_player(30, 0L);
				}
				else
				{
#ifdef JP
					msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
					msg_print("You don't see any monster in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;

		case MUT1_DAZZLE:
			stun_monsters(lvl * 4);
			confuse_monsters(lvl * 4);
			turn_monsters(lvl * 4);
			break;

		case MUT1_LASER_EYE:
			if (!get_aim_dir(&dir)) return FALSE;
			fire_beam(GF_LITE, dir, 3 * lvl);
			break;

		case MUT1_RECALL:
			if (!word_of_recall()) return FALSE;
			break;

			/*:::�׈����ŕψ�*/
		case MUT1_ID_MUSHROOM:
			{

				if (!ident_spell_2(1)) return FALSE;

#if 0
				int x, y;
				cave_type *c_ptr;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_rep_dir2(&dir)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("�׈��ȑ��݂������Ƃ�܂���I");
#else
					msg_print("You sense no evil there!");
#endif

					break;
				}

				m_ptr = &m_list[c_ptr->m_idx];
				r_ptr = &r_info[m_ptr->r_idx];

				if ((r_ptr->flags3 & RF3_ANG_CHAOS) &&
				    !(r_ptr->flags1 & RF1_QUESTOR) &&
				    !(r_ptr->flags1 & RF1_UNIQUE) &&
				    !p_ptr->inside_arena && !p_ptr->inside_quest &&
					(r_ptr->level < randint1(p_ptr->lev+50)) &&
					!(m_ptr->mflag2 & MFLAG2_NOGENO))
				{
					if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
					{
						char m_name[80];

						monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
						do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
					}

					/* Delete the monster, rather than killing it. */
					delete_monster_idx(c_ptr->m_idx);
#ifdef JP
					msg_print("���̎׈��ȃ����X�^�[�͗����L�����ƂƂ��ɏ����������I");
#else
					msg_print("The evil creature vanishes in a puff of sulfurous smoke!");
#endif

				}
				else
				{
#ifdef JP
					msg_print("�F��͌��ʂ��Ȃ������I");
#else
					msg_print("Your invocation is ineffectual!");
#endif

					if (one_in_(13)) m_ptr->mflag2 |= MFLAG2_NOGENO;
				}
#endif
			}
			break;
		//v1.1.39
		case MUT1_DANMAKU:
			{
				int dam_tmp;
				if (!get_aim_dir(&dir)) return FALSE;
				msg_print("���Ȃ��͋��͂Ȓe����������I");
				dam_tmp = (p_ptr->stat_ind[A_CHR] + 3) * 5;

				if (hack_flag_powerup_mutation) dam_tmp *= 2;

				fire_ball(GF_DISP_ALL, dir, dam_tmp, 3);

			}
			break;
#if 0
		/* XXX_XXX_XXX Hack!  MUT1_LAUNCHER is negative, see above */

		case 3: /* MUT1_LAUNCHER */
			/* Gives a multiplier of 2 at first, up to 3 at 40th */
			if (!do_cmd_throw_aux(2 + lvl / 40, FALSE, -1)) return FALSE;
			break;
#endif
		default:
			energy_use = 0;
#ifdef JP
			msg_format("�\�� %s �͎�������Ă��܂���B", power);
#else
			msg_format("Power %s not implemented. Oops.", power);
#endif

	}

	return TRUE;
}

//���̓I�ȃ����_���ˑR�ψق��l������B�ϗe���@�́u���̕ϗe�v���g�����Ƃ��Ɩ��@�̐X�̓�̓���H�ׂ��Ƃ��Ă΂��
void gain_physical_mutation(void)
{
	int attempt = 10;

	//���̓I�ȓˑR�ψق̃��X�g
	//gain_random_mutation()�ɓn���p�����[�^
	int muta_lis[] =
	{
		1,5,8,17,19,24,31,35,38,46,62,65,81,83,85,95,98,100,109,120,123,126,129,136,138,141,143,146,154,157,161,163,165,168,171,173,182,185
	};

	while ((attempt-- > 0) && !gain_random_mutation(muta_lis[randint0(sizeof(muta_lis) / sizeof(int)) - 1]));


}