/* Purpose: Artifact code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/* Chance of using syllables to form the name instead of the "template" files */
#define SINDARIN_NAME   10
#define TABLE_NAME      20
#define A_CURSED        13
///mod131229 WEIRD_LUCK���֐��ɕύX�@���b�L�[�}���̂Ƃ��ǂ��l���o�₷���悤�ɂ���
//#define WEIRD_LUCK      12
#define BIAS_LUCK       20
#define IM_LUCK         7

/*
 * Bias luck needs to be higher than weird luck,
 * since it is usually tested several times...
 */
#define ACTIVATION_CHANCE 3


/*
 * Use for biased artifact creation
 */
static int artifact_bias;

///mod131229 �����I�[�������̂Ƃ��l�X�Ȃ��Ƃ��L��
///mod141231 �얲�肢�ǉ�
bool weird_luck(void)
{

	if(p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 8)  return (one_in_(6));
	else if(p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 6)  return (one_in_(7));
	else if(p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 4)  return (one_in_(9));

	else if(p_ptr->lucky) return (one_in_(7));
	else if(p_ptr->today_mon == FORTUNETELLER_GOODLUCK)
	{
		if(p_ptr->muta3 & MUT3_GOOD_LUCK) return (one_in_(7));
		else return (one_in_(9));
	}
	else if(p_ptr->muta3 & MUT3_GOOD_LUCK) return (one_in_(9));
	else return (one_in_(12));
}


/*
 * Choose one random sustain
 */
/*:::�����_���\�͈ێ��t��*/
void one_sustain(object_type *o_ptr)
{
	switch (randint0(6))
	{
		case 0: add_flag(o_ptr->art_flags, TR_SUST_STR); break;
		case 1: add_flag(o_ptr->art_flags, TR_SUST_INT); break;
		case 2: add_flag(o_ptr->art_flags, TR_SUST_WIS); break;
		case 3: add_flag(o_ptr->art_flags, TR_SUST_DEX); break;
		case 4: add_flag(o_ptr->art_flags, TR_SUST_CON); break;
		case 5: add_flag(o_ptr->art_flags, TR_SUST_CHR); break;
	}
}


/*
 * Choose one random high resistance
 */
/*:::�����_����ʑϐ��t�^*/
///res �����_����ʑϐ��t�^
///mod131228 ��ʑϐ��ύX ����ϐ��Ɛ_���ϐ��͏����o�ɂ����悤�ɂ���
void one_high_resistance(object_type *o_ptr)
{

	if(one_in_(32)){
		add_flag(o_ptr->art_flags, TR_RES_TIME);  
		return;
	}
	if(one_in_(16)){
		add_flag(o_ptr->art_flags, TR_RES_HOLY);  
		return;
	}

	switch (randint0(9))
	{
		case  0: add_flag(o_ptr->art_flags, TR_RES_POIS);   break;
		case  1: add_flag(o_ptr->art_flags, TR_RES_LITE);   break;
		case  2: add_flag(o_ptr->art_flags, TR_RES_DARK);   break;
		case  3: add_flag(o_ptr->art_flags, TR_RES_SHARDS); break;
		case  4: add_flag(o_ptr->art_flags, TR_RES_WATER);  break;
		case  5: add_flag(o_ptr->art_flags, TR_RES_SOUND);  break;
		case  6: add_flag(o_ptr->art_flags, TR_RES_NETHER); break;
		case  7: add_flag(o_ptr->art_flags, TR_RES_CHAOS);  break;
		case  8: add_flag(o_ptr->art_flags, TR_RES_DISEN);  break;
	}
}


/*
 * Choose one random high resistance ( except poison and disenchantment )
 */
 /*:::���҂̉���̎w�֐�����p�̏�ʑϐ��t���@�łƗ򉻂������Ă���*/
/// res�@���҂̉���̎w�ւ̑ϐ��t��
///mod131228 ��ʑϐ��ύX
void one_lordly_high_resistance(object_type *o_ptr)
{
	switch (randint0(9))
	{
		case 0: add_flag(o_ptr->art_flags, TR_RES_LITE);   break;
		case 1: add_flag(o_ptr->art_flags, TR_RES_DARK);   break;
		case 2: add_flag(o_ptr->art_flags, TR_RES_SHARDS); break;
		case 3: add_flag(o_ptr->art_flags, TR_RES_WATER);  break;
		case 4: add_flag(o_ptr->art_flags, TR_RES_HOLY);   break;
		case 5: add_flag(o_ptr->art_flags, TR_RES_SOUND);  break;
		case 6: add_flag(o_ptr->art_flags, TR_RES_NETHER); break;
		case 7: add_flag(o_ptr->art_flags, TR_RES_TIME);   break;
		case 8: add_flag(o_ptr->art_flags, TR_RES_CHAOS);  break;
	}
}


/*
 * Choose one random element resistance
 */
/*:::�����_�����f�ϐ��t�^�@�ŏ���*/
void one_ele_resistance(object_type *o_ptr)
{
	switch (randint0(4))
	{
		case  0: add_flag(o_ptr->art_flags, TR_RES_ACID); break;
		case  1: add_flag(o_ptr->art_flags, TR_RES_ELEC); break;
		case  2: add_flag(o_ptr->art_flags, TR_RES_COLD); break;
		case  3: add_flag(o_ptr->art_flags, TR_RES_FIRE); break;
	}
}


/*
 * Choose one random element or poison resistance
 */
/*:::�����_�����f�ϐ��t�^�@�������ł��t�����Ƃ�����*/
void one_dragon_ele_resistance(object_type *o_ptr)
{
	if (one_in_(7))
	{
		add_flag(o_ptr->art_flags, TR_RES_POIS);
	}
	else
	{
		one_ele_resistance(o_ptr);
	}
}


/*
 * Choose one lower rank esp
 */
/*:::�A�C�e���������A�アESP��t�^����*/
///item mon �アESP�t�^
///mod131229 �アESP�͓��������l���_�d�Ƃ���
void one_low_esp(object_type *o_ptr)
{
	switch (randint1(8))
	{
	case 1:  add_flag(o_ptr->art_flags, TR_ESP_ANIMAL);   break;
	case 2:  add_flag(o_ptr->art_flags, TR_ESP_UNDEAD);   break;
	case 3:  add_flag(o_ptr->art_flags, TR_ESP_DEMON);   break;
	case 4:  add_flag(o_ptr->art_flags, TR_ESP_KWAI);   break;
	case 5:  add_flag(o_ptr->art_flags, TR_ESP_DEITY);   break;
	case 6:  add_flag(o_ptr->art_flags, TR_ESP_GOOD);   break;
	case 7:  add_flag(o_ptr->art_flags, TR_ESP_DRAGON);   break;
	case 8:  add_flag(o_ptr->art_flags, TR_ESP_HUMAN);   break;
	}
}



/*
 * Choose one random resistance
 */
/*:::�����_���ϐ�*/
void one_resistance(object_type *o_ptr)
{
	if (one_in_(3))
	{
		one_ele_resistance(o_ptr);
	}
	else
	{
		one_high_resistance(o_ptr);
	}
}


/*
 * Choose one random ability
 */
/*:::�����i�������̒ǉ��\�͕t�^*/
///item res �\�͕t�^�@�����ϐ��Ȃǂ�������Ɉڂ���
void one_ability(object_type *o_ptr)
{

	if(one_in_(32)){
		add_flag(o_ptr->art_flags, TR_RES_INSANITY);
		return;
	}
	if(one_in_(32)){
		add_flag(o_ptr->art_flags, TR_SPEEDSTER);
		return;
	}

	switch (randint0(15))
	{
	case 0: add_flag(o_ptr->art_flags, TR_LEVITATION);     break;
	case 1: add_flag(o_ptr->art_flags, TR_LITE);        break;
	case 2: add_flag(o_ptr->art_flags, TR_SEE_INVIS);   break;
	case 3: add_flag(o_ptr->art_flags, TR_WARNING);     break;
	case 4: add_flag(o_ptr->art_flags, TR_SLOW_DIGEST); break;
	case 5: add_flag(o_ptr->art_flags, TR_REGEN);       break;
	case 6:
	case 7: 
			add_flag(o_ptr->art_flags, TR_FREE_ACT);    break;
	case 8:
	case 9: 
			add_flag(o_ptr->art_flags, TR_RES_CONF);    break;
	case 10:
	case 11:
			add_flag(o_ptr->art_flags, TR_RES_FEAR);    break;
	case 12:
			add_flag(o_ptr->art_flags, TR_RES_BLIND);    break;
	case 13:
	case 14:
			one_low_esp(o_ptr);	break;
	}
}

/*
 * Choose one random activation
 */
void one_activation(object_type *o_ptr)
{
	int type = 0;
	int chance = 0;

	while (randint1(100) >= chance)
	{
		type = randint1(255);
		switch (type)
		{
			case ACT_SUNLIGHT:
			case ACT_BO_MISS_1:
			case ACT_BA_POIS_1:
			case ACT_BO_ELEC_1:
			case ACT_BO_ACID_1:
			case ACT_BO_COLD_1:
			case ACT_BO_FIRE_1:
			case ACT_CONFUSE:
			case ACT_SLEEP:
			case ACT_QUAKE:
			case ACT_CURE_LW:
			case ACT_CURE_MW:
			case ACT_CURE_POISON:
			case ACT_BERSERK:
			case ACT_LIGHT:
			case ACT_MAP_LIGHT:
			case ACT_DEST_DOOR:
			case ACT_STONE_MUD:
			case ACT_TELEPORT:
				chance = 101;
				break;
			case ACT_BA_COLD_1:
			case ACT_BA_FIRE_1:
			case ACT_DRAIN_1:
			case ACT_TELE_AWAY:
			case ACT_ESP:
			case ACT_RESIST_ALL:
			case ACT_DETECT_ALL:
			case ACT_RECALL:
			case ACT_SATIATE:
			case ACT_RECHARGE:
				chance = 85;
				break;
			case ACT_TERROR:
			case ACT_PROT_EVIL:
			case ACT_ID_PLAIN:
				chance = 75;
				break;
			case ACT_DRAIN_2:
			case ACT_VAMPIRE_1:
			case ACT_BO_MISS_2:
			case ACT_BA_FIRE_2:
			case ACT_REST_LIFE:
				chance = 66;
				break;
			case ACT_BA_FIRE_3:
			case ACT_BA_COLD_3:
			case ACT_BA_ELEC_3:
			case ACT_WHIRLWIND:
			case ACT_TORNADO:
			case ACT_VAMPIRE_2:
			case ACT_CHARM_ANIMAL:
				chance = 50;
				break;
			case ACT_SUMMON_ANIMAL:
				chance = 40;
				break;
			case ACT_DISP_EVIL:
			case ACT_BA_MISS_3:
			case ACT_DISP_GOOD:
			case ACT_BANISH_EVIL:
			case ACT_GENOCIDE:
			case ACT_MASS_GENO:
			case ACT_CHARM_UNDEAD:
			case ACT_CHARM_OTHER:
			case ACT_SUMMON_PHANTOM:
			case ACT_REST_ALL:
			case ACT_RUNE_EXPLO:
			case ACT_DAZZLE:
			case ACT_CURE_500:
				chance = 33;
				break;
			case ACT_CALL_CHAOS:
			case ACT_ROCKET:
			case ACT_CHARM_ANIMALS:
			case ACT_CHARM_OTHERS:
			case ACT_SUMMON_ELEMENTAL:
			case ACT_CURE_700:
			case ACT_SPEED:
			case ACT_ID_FULL:
			case ACT_RUNE_PROT:
				chance = 25;
				break;
			case ACT_CURE_1000:
			case ACT_XTRA_SPEED:
			case ACT_DETECT_XTRA:
			case ACT_DIM_DOOR:
			case ACT_DETECT_ARTIFACT:
				chance = 10;
				break;
			case ACT_SUMMON_UNDEAD:
			case ACT_SUMMON_DEMON:
			case ACT_WRAITH:
			case ACT_INVULN:
			case ACT_ALCHEMY:
				chance = 5;
				break;

			case ACT_SWORD_DANCE:
				if (o_ptr->tval >= TV_KNIFE && o_ptr->tval <= TV_KATANA)
					chance = 30;
				else
					chance = 0;
				break;

			//v1.1.74
			case ACT_TIM_ADD_DICES:
			case ACT_PSY_SPEAR:
			case ACT_LONG_RANGE_ATTACK:
				if (object_is_melee_weapon(o_ptr))
					chance = 50;
				else 
					chance = 0;
				break;

			case ACT_HIT_AND_AWAY:
				if (object_is_melee_weapon(o_ptr) || o_ptr->tval == TV_BOOTS)
					chance = 75;
				else
					chance = 0;
				break;
			case ACT_SAINT_STAR_ARROW:
				if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW)
					chance = 50;
				else
					chance = 0;
				break;
			case ACT_TIM_SUPER_STEALTH:
				if (o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_BOOTS || o_ptr->tval == TV_LITE)
					chance = 30;
				else
					chance = 10;
				break;
			case ACT_DUAL_CAST:
				if (o_ptr->tval == TV_HEAD || o_ptr->tval == TV_AMULET || o_ptr->tval == TV_RING)
					chance = 50;
				else if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_WIZSTAFF)
					chance = 50;
				else if (object_is_melee_weapon(o_ptr))
					chance = 30;
				else
					chance = 5;
				break;


			default:
				chance = 0;
		}
	}

	/* A type was chosen... */
	o_ptr->xtra2 = type;
	add_flag(o_ptr->art_flags, TR_ACTIVATE);
	o_ptr->timeout = 0;
}

/*:::��������Ă���Ƃ��̃t���O�ݒ�*/
///class
static void curse_artifact(object_type * o_ptr)
{
	if (o_ptr->pval > 0) o_ptr->pval = 0 - (o_ptr->pval + randint1(4));
	if (o_ptr->to_a > 0) o_ptr->to_a = 0 - (o_ptr->to_a + randint1(4));
	if (o_ptr->to_h > 0) o_ptr->to_h = 0 - (o_ptr->to_h + randint1(4));
	if (o_ptr->to_d > 0) o_ptr->to_d = 0 - (o_ptr->to_d + randint1(4));

	o_ptr->curse_flags |= (TRC_HEAVY_CURSE | TRC_CURSED);
	remove_flag(o_ptr->art_flags, TR_BLESSED);

	if (one_in_(4)) o_ptr->curse_flags |= TRC_PERMA_CURSE;
	if (one_in_(3)) add_flag(o_ptr->art_flags, TR_TY_CURSE);
	if (one_in_(2)) add_flag(o_ptr->art_flags, TR_AGGRAVATE);
	if (one_in_(3)) add_flag(o_ptr->art_flags, TR_DRAIN_EXP);
	if (one_in_(6)) add_flag(o_ptr->art_flags, TR_ADD_L_CURSE);
	if (one_in_(9)) add_flag(o_ptr->art_flags, TR_ADD_H_CURSE);
	if (one_in_(2)) add_flag(o_ptr->art_flags, TR_TELEPORT);
	else if (one_in_(3)) add_flag(o_ptr->art_flags, TR_NO_TELE);

	if ((p_ptr->pclass != CLASS_WARRIOR) && (p_ptr->pclass != CLASS_ARCHER) && (p_ptr->pclass != CLASS_CAVALRY) && one_in_(3))
		add_flag(o_ptr->art_flags, TR_NO_MAGIC);
}

/*:::�A�[�e�B�t�@�N�g�������̔\�͒l�A�b�v�@�o�C�A�X�l�Ɉˑ�*/
///class?
static void random_plus(object_type * o_ptr)
{
	//v1.1.99 �g��
	//int this_type = (object_is_weapon_ammo(o_ptr) ? 25 : 21);
	int this_type = (object_is_weapon_ammo(o_ptr) ? 29 : 25);

	switch (artifact_bias)
	{
	case BIAS_WARRIOR:
		if (!(have_flag(o_ptr->art_flags, TR_STR)))
		{
			add_flag(o_ptr->art_flags, TR_STR);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_CON)))
		{
			add_flag(o_ptr->art_flags, TR_CON);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_DEX);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_MAGE:
		if (!(have_flag(o_ptr->art_flags, TR_INT)))
		{
			add_flag(o_ptr->art_flags, TR_INT);
			if (one_in_(2)) return;
		}
		if ((o_ptr->tval == TV_GLOVES) && !(have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY)))
		{
			add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_PRIESTLY:
		if (!(have_flag(o_ptr->art_flags, TR_WIS)))
		{
			add_flag(o_ptr->art_flags, TR_WIS);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SAVING)))
		{
			add_flag(o_ptr->art_flags, TR_SAVING);
			if (one_in_(2)) return;
		}

		break;

	case BIAS_RANGER:
		if ((o_ptr->tval == TV_GLOVES) && !(have_flag(o_ptr->art_flags, TR_DISARM)))
		{
			add_flag(o_ptr->art_flags, TR_DISARM);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_DEX);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_CON)))
		{
			add_flag(o_ptr->art_flags, TR_CON);
			if (one_in_(2)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_STR)))
		{
			add_flag(o_ptr->art_flags, TR_STR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ROGUE:
		if ((o_ptr->tval == TV_GLOVES) && !(have_flag(o_ptr->art_flags, TR_DISARM)))
		{
			add_flag(o_ptr->art_flags, TR_DISARM);
			if (one_in_(4)) return;
		}

		if (!(have_flag(o_ptr->art_flags, TR_STEALTH)))
		{
			add_flag(o_ptr->art_flags, TR_STEALTH);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SEARCH)))
		{
			add_flag(o_ptr->art_flags, TR_SEARCH);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_STR:
		if (!(have_flag(o_ptr->art_flags, TR_STR)))
		{
			add_flag(o_ptr->art_flags, TR_STR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_WIS:
		if (!(have_flag(o_ptr->art_flags, TR_WIS)))
		{
			add_flag(o_ptr->art_flags, TR_WIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_INT:
		if (!(have_flag(o_ptr->art_flags, TR_INT)))
		{
			add_flag(o_ptr->art_flags, TR_INT);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_DEX:
		if (!(have_flag(o_ptr->art_flags, TR_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_DEX);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CON:
		if (!(have_flag(o_ptr->art_flags, TR_CON)))
		{
			add_flag(o_ptr->art_flags, TR_CON);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHR:
		if (!(have_flag(o_ptr->art_flags, TR_CHR)))
		{
			add_flag(o_ptr->art_flags, TR_CHR);
			if (one_in_(2)) return;
		}
		break;
	}

	if ((artifact_bias == BIAS_MAGE || artifact_bias == BIAS_PRIESTLY) && (o_ptr->tval == TV_CLOTHES) && (o_ptr->sval == SV_CLOTH_ROBE))
	{
		if (!(have_flag(o_ptr->art_flags, TR_DEC_MANA)) && one_in_(3))
		{
			add_flag(o_ptr->art_flags, TR_DEC_MANA);
			if (one_in_(2)) return;
		}
	}
	///mod131229 �A�~���Ǝ�́��ɂ͖�����x�z���t���₷������
	if((o_ptr->tval == TV_GLOVES || o_ptr->tval == TV_AMULET || o_ptr->tval == TV_RIBBON) && one_in_(100) && !(have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY))){
		add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
		return;
	}

	///mod150205 ������1/2�̊m���ŕʂ̃��[�`���ɂ���
	if(o_ptr->tval == TV_LITE && one_in_(2))
	{
		switch(randint1(10))
		{
		case 1: case 6: case 10:
			add_flag(o_ptr->art_flags, TR_SEARCH);
			break;
		case 2: case 7:
			add_flag(o_ptr->art_flags, TR_INFRA);
			break;
		case 3: 
			add_flag(o_ptr->art_flags, TR_INT);
			break;
		case 4: case 8:
			add_flag(o_ptr->art_flags, TR_WIS);
			break;
		case 5: case 9:
			add_flag(o_ptr->art_flags, TR_CHR);
			break;
		}
		return;
	}




	switch (randint1(this_type))
	{
	case 1: case 2:
		add_flag(o_ptr->art_flags, TR_STR);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_STR;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_WARRIOR;
		break;
	case 3: case 4:
		add_flag(o_ptr->art_flags, TR_INT);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_INT;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_MAGE;
		break;
	case 5: case 6:
		add_flag(o_ptr->art_flags, TR_WIS);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_WIS;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_PRIESTLY;
		break;
	case 7: case 8:
		add_flag(o_ptr->art_flags, TR_DEX);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_DEX;
		else if (!artifact_bias && one_in_(7))
			artifact_bias = BIAS_ROGUE;
		break;
	case 9: case 10:
		add_flag(o_ptr->art_flags, TR_CON);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_CON;
		else if (!artifact_bias && one_in_(9))
			artifact_bias = BIAS_RANGER;
		break;
	case 11: case 12:
		add_flag(o_ptr->art_flags, TR_CHR);
		if (!artifact_bias && !one_in_(13))
			artifact_bias = BIAS_CHR;
		break;
	case 13: case 14:
		add_flag(o_ptr->art_flags, TR_STEALTH);
		if (!artifact_bias && one_in_(3))
			artifact_bias = BIAS_ROGUE;
		break;
	case 15: case 16:
		add_flag(o_ptr->art_flags, TR_SEARCH);
		if (!artifact_bias && one_in_(9))
			artifact_bias = BIAS_RANGER;
		break;
	case 17: case 18:
		add_flag(o_ptr->art_flags, TR_INFRA);
		break;
	case 19:
		add_flag(o_ptr->art_flags, TR_SPEED);
		if (!artifact_bias && one_in_(11))
			artifact_bias = BIAS_ROGUE;
		break;

	case 20:case 21:
		add_flag(o_ptr->art_flags, TR_DISARM);
		if (!artifact_bias && one_in_(11))
			artifact_bias = BIAS_ROGUE;
		break;
	case 22:case 23:
		add_flag(o_ptr->art_flags, TR_SAVING);
		if (!artifact_bias && one_in_(11))
			artifact_bias = BIAS_PRIESTLY;
		break;




	///mod131230 TVAL���猩�Ă�����ۂ�PVAL���t���₷���Ȃ�悤�ɂ��Ă݂�
	case 24: case 25:
		switch(o_ptr->tval)
		{
		case TV_KNIFE:
			if(one_in_(3))		add_flag(o_ptr->art_flags, TR_BLOWS);
			else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_STEALTH);
			else 				add_flag(o_ptr->art_flags, TR_DEX);
			break;
		case TV_SWORD:
			if(one_in_(3))		add_flag(o_ptr->art_flags, TR_BLOWS);
			else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_CON);
			else 				add_flag(o_ptr->art_flags, TR_STR);
			break;
		case TV_KATANA:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_BLOWS);
			else				add_flag(o_ptr->art_flags, TR_DEX);
			break;
		case TV_HAMMER: case TV_AXE: case TV_POLEARM:
			add_flag(o_ptr->art_flags, TR_TUNNEL);
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
			else				add_flag(o_ptr->art_flags, TR_CON);
			break;
		case TV_STICK:
			if(o_ptr->sval == SV_WEAPON_WIZSTAFF || o_ptr->sval == SV_WEAPON_QUARTERSTAFF  || o_ptr->sval == SV_WEAPON_SYAKUJYOU) {
				if(one_in_(3))		add_flag(o_ptr->art_flags, TR_INT);
				else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_WIS);
				else				add_flag(o_ptr->art_flags, TR_MAGIC_MASTERY);
			}
			else{
				if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
				else				add_flag(o_ptr->art_flags, TR_BLOWS);
			}
			break;
		case TV_SPEAR:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_DEX);
			else				add_flag(o_ptr->art_flags, TR_SPEED);
			break;			
		case TV_BOW: case TV_CROSSBOW:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_DEX);
			else				add_flag(o_ptr->art_flags, TR_STEALTH);
			break;	
		case TV_SHIELD: case TV_ARMOR: case TV_DRAG_ARMOR:
			if(one_in_(3))		add_flag(o_ptr->art_flags, TR_CHR);

			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
			else				add_flag(o_ptr->art_flags, TR_CON);
			break;
		case TV_CLOAK:
			add_flag(o_ptr->art_flags, TR_STEALTH);
			if(one_in_(2)) break;

			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_CHR);
			else				add_flag(o_ptr->art_flags, TR_SEARCH);
			break;

		case TV_CLOTHES:
			add_flag(o_ptr->art_flags, TR_SAVING);
			if (one_in_(2))		add_flag(o_ptr->art_flags, TR_CHR);

			break;
		case TV_HEAD:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_SEARCH);
			else				add_flag(o_ptr->art_flags, TR_CHR);			
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_INT);
			else				add_flag(o_ptr->art_flags, TR_WIS);
			break;
		case TV_GLOVES:
			if (one_in_(3))		add_flag(o_ptr->art_flags, TR_DISARM);
			else if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STR);
			else				add_flag(o_ptr->art_flags, TR_DEX);
			break;
		case TV_BOOTS:
			if(one_in_(2))		add_flag(o_ptr->art_flags, TR_STEALTH);
			else				add_flag(o_ptr->art_flags, TR_SPEED);
			break;

		case TV_GUN:
			if(one_in_(2)) add_flag(o_ptr->art_flags, TR_DEX);
			else add_flag(o_ptr->art_flags, TR_STEALTH);
			break;


		default:
			random_plus(o_ptr);
			break;
		}
		break;
	case 26: case 27:
		add_flag(o_ptr->art_flags, TR_TUNNEL);
		break;
	case 28: case 29:
		if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW || o_ptr->tval == TV_GUN) 
			random_plus(o_ptr);
		else
		{
			add_flag(o_ptr->art_flags, TR_BLOWS);
			if (!artifact_bias && one_in_(11))
				artifact_bias = BIAS_WARRIOR;
		}
		break;
	}
}

/*:::�A�[�e�B�t�@�N�g�������̃����_���ϐ��@�o�C�A�X�Ɉˑ�*/
///res item ���������̑ϐ��t�^
///mod131228 TR_??�t���O�ύX�@���ʍ������폜�����A�_���A����ϐ��ǉ�
///pend �A�[�e�B�t�@�N�g�o�C�A�X�̒���
static void random_resistance(object_type * o_ptr)
{
	switch (artifact_bias)
	{
	case BIAS_ACID:
		if (!(have_flag(o_ptr->art_flags, TR_RES_ACID)))
		{
			add_flag(o_ptr->art_flags, TR_RES_ACID);
			if (one_in_(2)) return;
		}
		if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_ACID)))
		{
			add_flag(o_ptr->art_flags, TR_IM_ACID);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ELEC);
				remove_flag(o_ptr->art_flags, TR_IM_COLD);
				remove_flag(o_ptr->art_flags, TR_IM_FIRE);
			}
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ELEC:
		///mod131228 tval

		if (!(have_flag(o_ptr->art_flags, TR_RES_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_RES_ELEC);
			if (one_in_(2)) return;
		}

		if ((o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_CLOTHES ) && !(have_flag(o_ptr->art_flags, TR_SH_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_SH_ELEC);
			if (one_in_(2)) return;
		}

		if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_IM_ELEC);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ACID);
				remove_flag(o_ptr->art_flags, TR_IM_COLD);
				remove_flag(o_ptr->art_flags, TR_IM_FIRE);
			}

			if (one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if (!(have_flag(o_ptr->art_flags, TR_RES_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_RES_FIRE);
			if (one_in_(2)) return;
		}
		///mod131228 tval
		if ((o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_CLOTHES ) &&
		    !(have_flag(o_ptr->art_flags, TR_SH_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_SH_FIRE);
			if (one_in_(2)) return;
		}

		if (one_in_(BIAS_LUCK) &&
		    !(have_flag(o_ptr->art_flags, TR_IM_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_IM_FIRE);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ELEC);
				remove_flag(o_ptr->art_flags, TR_IM_COLD);
				remove_flag(o_ptr->art_flags, TR_IM_ACID);
			}
			if (one_in_(2)) return;
		}
		break;

	case BIAS_COLD:
		if (!(have_flag(o_ptr->art_flags, TR_RES_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_RES_COLD);
			if (one_in_(2)) return;
		}
		///mod131228 tval 
		
		if ((o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_CLOTHES ) &&
		    !(have_flag(o_ptr->art_flags, TR_SH_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_SH_COLD);
			if (one_in_(2)) return;
		}
		
		if (one_in_(BIAS_LUCK) && !(have_flag(o_ptr->art_flags, TR_IM_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_IM_COLD);
			if (!one_in_(IM_LUCK))
			{
				remove_flag(o_ptr->art_flags, TR_IM_ELEC);
				remove_flag(o_ptr->art_flags, TR_IM_ACID);
				remove_flag(o_ptr->art_flags, TR_IM_FIRE);
			}
			if (one_in_(2)) return;
		}
		break;

	case BIAS_POIS:
		if (!(have_flag(o_ptr->art_flags, TR_RES_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_RES_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_WARRIOR:
		if (!one_in_(3) && (!(have_flag(o_ptr->art_flags, TR_RES_FEAR))))
		{
			add_flag(o_ptr->art_flags, TR_RES_FEAR);
			if (one_in_(2)) return;
		}
		if (one_in_(3) && (!(have_flag(o_ptr->art_flags, TR_NO_MAGIC))))
		{
			add_flag(o_ptr->art_flags, TR_NO_MAGIC);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_NECROMANTIC:
		if (!(have_flag(o_ptr->art_flags, TR_RES_NETHER)))
		{
			add_flag(o_ptr->art_flags, TR_RES_NETHER);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_RES_POIS);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_DARK)))
		{
			add_flag(o_ptr->art_flags, TR_RES_DARK);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHAOS:
		if (!(have_flag(o_ptr->art_flags, TR_RES_CHAOS)))
		{
			add_flag(o_ptr->art_flags, TR_RES_CHAOS);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_CONF)))
		{
			add_flag(o_ptr->art_flags, TR_RES_CONF);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_RES_DISEN)))
		{
			add_flag(o_ptr->art_flags, TR_RES_DISEN);
			if (one_in_(2)) return;
		}
		break;
	}

	///mod150205 ������1/2�̊m���ŕʂ̃��[�`���ɂ���
	if(o_ptr->tval == TV_LITE && one_in_(2))
	{
		int cnt;
		switch(randint1(28))
		{
			//v1.1.30 ������ƕύX
		case 1: case 13:
			add_flag(o_ptr->art_flags, TR_ESP_EVIL);
			break;
		case 2: case 14:
			add_flag(o_ptr->art_flags, TR_ESP_NONLIVING);
			break;
		case 3: case 15:
			add_flag(o_ptr->art_flags, TR_ESP_UNIQUE);
			break;
		case 4: case 16:
			add_flag(o_ptr->art_flags, TR_TELEPATHY);
			break;
		case 5: case 6: case 17: case 24:
			for(cnt=randint1(3);cnt>0;cnt--) one_low_esp(o_ptr);
			break;
		case 7: case 18: case 25:
			add_flag(o_ptr->art_flags, TR_WARNING);
			break;
		case 8:
			add_flag(o_ptr->art_flags, TR_EASY_SPELL);
			break;
		case 9: case 19: case 26:
			add_flag(o_ptr->art_flags, TR_RES_LITE);
			break;
		case 10: case 20: case 23:
			add_flag(o_ptr->art_flags, TR_RES_DARK);
			break;
		case 11: case 21: case 27:
			add_flag(o_ptr->art_flags, TR_RES_BLIND);
			break;
		case 12: case 22: case 28:
			add_flag(o_ptr->art_flags, TR_SEE_INVIS);
			break;

		}
		return;
	}

	///res
	switch (randint1(41))
	{
		case 1:
			/*:::�Ɖu�͕t���ɂ���*/
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_ACID);
				if (!artifact_bias)
					artifact_bias = BIAS_ACID;
			}
			break;
		case 2:
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_ELEC);
				if (!artifact_bias)
					artifact_bias = BIAS_ELEC;
			}
			break;
		case 3:
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_COLD);
				if (!artifact_bias)
					artifact_bias = BIAS_COLD;
			}
			break;
		case 4:
			if (!weird_luck())
				random_resistance(o_ptr);
			else
			{
				add_flag(o_ptr->art_flags, TR_IM_FIRE);
				if (!artifact_bias)
					artifact_bias = BIAS_FIRE;
			}
			break;
		case 5:
		case 6:
		case 13:
			add_flag(o_ptr->art_flags, TR_RES_ACID);
			if (!artifact_bias)
				artifact_bias = BIAS_ACID;
			break;
		case 7:
		case 8:
		case 14:
			add_flag(o_ptr->art_flags, TR_RES_ELEC);
			if (!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 9:
		case 10:
		case 15:
			add_flag(o_ptr->art_flags, TR_RES_FIRE);
			if (!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 11:
		case 12:
		case 16:
			add_flag(o_ptr->art_flags, TR_RES_COLD);
			if (!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;
		case 17:
		case 18:
			add_flag(o_ptr->art_flags, TR_RES_POIS);
			if (!artifact_bias && !one_in_(4))
				artifact_bias = BIAS_POIS;
			else if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_NECROMANTIC;
			else if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_ROGUE;
			break;
		case 19:
		case 20:
			add_flag(o_ptr->art_flags, TR_RES_LITE);
			break;
		case 21:
		case 22:
			add_flag(o_ptr->art_flags, TR_RES_DARK);
			break;

		case 23:
		case 24:
			add_flag(o_ptr->art_flags, TR_RES_SOUND);
			break;
		case 25:
		case 26:
			add_flag(o_ptr->art_flags, TR_RES_SHARDS);
			break;
		case 27:
		case 28:
			add_flag(o_ptr->art_flags, TR_RES_NETHER);
			if (!artifact_bias && one_in_(3))
				artifact_bias = BIAS_NECROMANTIC;
			break;
		case 29:
		case 30:
			add_flag(o_ptr->art_flags, TR_RES_WATER);
			break;
		case 31:
		case 32:
			add_flag(o_ptr->art_flags, TR_RES_CHAOS);
			if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_CHAOS;
			break;
		case 33:
		case 34:
			add_flag(o_ptr->art_flags, TR_RES_DISEN);
			break;
		case 35:
			///mod131228 TVAL
			if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES)
				add_flag(o_ptr->art_flags, TR_SH_ELEC);
			else
			
				random_resistance(o_ptr);
			if (!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 36:
			///mod131228 TVAL
			if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES)
				add_flag(o_ptr->art_flags, TR_SH_FIRE);
			else
				random_resistance(o_ptr);
			if (!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 37:
			///mod131228 TVAL
			if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES)
				add_flag(o_ptr->art_flags, TR_SH_COLD);
			else
				random_resistance(o_ptr);
			if (!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;

		case 38:
			///mod31228 ���˂��t���m�������炵��
			if ( (o_ptr->tval == TV_SHIELD || o_ptr->tval == TV_CLOAK || o_ptr->tval == TV_ARMOR) && one_in_(2))
				add_flag(o_ptr->art_flags, TR_REFLECT);
			else
				random_resistance(o_ptr);
			break;
		case 39:
			add_flag(o_ptr->art_flags, TR_RES_HOLY);
			break;
		case 40:
			add_flag(o_ptr->art_flags, TR_RES_TIME);
			break;
///mod131229 ���E���E�N���[�N�ɂ͑S�ϐ����t���\�������߂Ă݂�
		case 41:
		if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES ||  o_ptr->tval == TV_SHIELD){
			add_flag(o_ptr->art_flags, TR_RES_ACID);
			add_flag(o_ptr->art_flags, TR_RES_ELEC);
			add_flag(o_ptr->art_flags, TR_RES_FIRE);
			add_flag(o_ptr->art_flags, TR_RES_COLD);
		}
		else
				random_resistance(o_ptr);

		break;
	}
}


/*:::���������̔\�͕t��*/
//��w�Ŗ�გm�炸���o�₷�����˂��o�ɂ����悤�ȏ����������ɓ��ꂽ��
///mod131229 �������\�͒ǉ��@�A�C�e���t���O����ւ��Ȃǂő啝�ύX
static void random_misc(object_type * o_ptr)
{
	switch (artifact_bias)
	{
	case BIAS_RANGER:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_CON)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_CON);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_STR:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_STR)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_STR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_WIS:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_WIS)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_WIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_INT:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_INT)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_INT);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_DEX:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_DEX)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_DEX);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CON:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_CON)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_CON);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHR:
		if (!(have_flag(o_ptr->art_flags, TR_SUST_CHR)))
		{
			add_flag(o_ptr->art_flags, TR_SUST_CHR);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_CHAOS:
		if (!(have_flag(o_ptr->art_flags, TR_TELEPORT)))
		{
			add_flag(o_ptr->art_flags, TR_TELEPORT);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if (!(have_flag(o_ptr->art_flags, TR_LITE)))
		{
			add_flag(o_ptr->art_flags, TR_LITE); /* Freebie */
		}
		break;
	}

	switch (randint1(40))
	{
		case 1:
			add_flag(o_ptr->art_flags, TR_SUST_STR);
			if (!artifact_bias)
				artifact_bias = BIAS_STR;
			break;
		case 2:
			add_flag(o_ptr->art_flags, TR_SUST_INT);
			if (!artifact_bias)
				artifact_bias = BIAS_INT;
			break;
		case 3:
			add_flag(o_ptr->art_flags, TR_SUST_WIS);
			if (!artifact_bias)
				artifact_bias = BIAS_WIS;
			break;
		case 4:
			add_flag(o_ptr->art_flags, TR_SUST_DEX);
			if (!artifact_bias)
				artifact_bias = BIAS_DEX;
			break;
		case 5:
			add_flag(o_ptr->art_flags, TR_SUST_CON);
			if (!artifact_bias)
				artifact_bias = BIAS_CON;
			break;
		case 6:
			add_flag(o_ptr->art_flags, TR_SUST_CHR);
			if (!artifact_bias)
				artifact_bias = BIAS_CHR;
			break;
		case 7:
		case 8:
		case 9:
			add_flag(o_ptr->art_flags, TR_FREE_ACT);
			break;
		case 10:
		case 11:
			add_flag(o_ptr->art_flags, TR_RES_CONF);
			break;
		case 12:
		case 13:
			add_flag(o_ptr->art_flags, TR_RES_BLIND);
			break;
		case 14:
		case 15:
			add_flag(o_ptr->art_flags, TR_RES_FEAR);
			break;
		case 16:
		case 17:
			add_flag(o_ptr->art_flags, TR_LITE);
			break;
		case 18:
		case 19:
			add_flag(o_ptr->art_flags, TR_LEVITATION);
			break;
		case 20:
		case 21:
		case 22:
			add_flag(o_ptr->art_flags, TR_SEE_INVIS);
			break;
		case 23:
		case 24:
			add_flag(o_ptr->art_flags, TR_SLOW_DIGEST);
			break;
		case 25:
		case 26:
			add_flag(o_ptr->art_flags, TR_REGEN);
			break;
		case 27:
			add_flag(o_ptr->art_flags, TR_TELEPORT);
			break;
		case 28:
		case 29:
			if (object_is_armour(o_ptr))
				random_misc(o_ptr);
			else
			{
				o_ptr->to_a = 4 + randint1(11);
			}
			break;
		case 30:
		case 31:
		case 32:
		{
			int bonus_h, bonus_d;
			add_flag(o_ptr->art_flags, TR_SHOW_MODS);
			bonus_h = 4 + (randint1(11));
			bonus_d = 4 + (randint1(11));

			/*:::Hack - �e�ւ̃_���[�W�{�[�i�X�𑝉��@���̑����l��k_info�x�[�X�A�C�e���l�ɏ�����*/
			if(o_ptr->tval == TV_GUN)
			{
				int mult_h = 100 + k_info[o_ptr->k_idx].to_h * 5;
				int mult_d = 100 + k_info[o_ptr->k_idx].to_d * 5;

				bonus_h = bonus_h * mult_h / 100 + randint1(bonus_h);
				bonus_d = bonus_d * mult_h / 100 + randint1(bonus_d);
			}
			else if ((o_ptr->tval != TV_RING) && (o_ptr->tval != TV_GLOVES) && !(object_is_melee_weapon(o_ptr)))
			{
				bonus_h /= 2;
				bonus_d /= 2;
			}

			o_ptr->to_h += bonus_h;
			o_ptr->to_d += bonus_d;
			break;
		}
		case 33:
			add_flag(o_ptr->art_flags, TR_NO_MAGIC);
			break;
		case 34:
			add_flag(o_ptr->art_flags, TR_NO_TELE);
			break;
		case 35:
			add_flag(o_ptr->art_flags, TR_WARNING);
			break;

		case 36:
			switch (randint1(4))
			{
			case 1:
				add_flag(o_ptr->art_flags, TR_ESP_EVIL);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_LAW;
				break;
			case 2:
				add_flag(o_ptr->art_flags, TR_ESP_NONLIVING);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_MAGE;
				break;
			case 3:
				add_flag(o_ptr->art_flags, TR_TELEPATHY);
				if (!artifact_bias && one_in_(9))
					artifact_bias = BIAS_MAGE;
				break;
			case 4:
				add_flag(o_ptr->art_flags, TR_ESP_UNIQUE);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_MAGE;
				break;

			}
			break;

		case 37:
		{
			int idx[3];
			int n = randint1(3);
			int i;

			/* v1.1.30 ���ƂȂ����������琦���C���������R�[�h�Ȃ̂ŕύX�@�ǂ���1~3�̃����_�����Ȃ̂Ŕ���Ă��C�ɂ��Ȃ�
			idx[0] = randint1(8);

			idx[1] = randint1(7);
			if (idx[1] >= idx[0]) idx[1]++;

			idx[2] = randint1(6);
			if (idx[2] >= idx[0]) idx[2]++;
			if (idx[2] >= idx[1]) idx[2]++;
			*/
			for(i=0;i<n;i++)
				idx[i] = randint1(8);

			for (i = 0; i < n; i++) switch (idx[i])
			{
			case 1:
				add_flag(o_ptr->art_flags, TR_ESP_ANIMAL);
				if (!artifact_bias && one_in_(4))
					artifact_bias = BIAS_RANGER;
				break;
			case 2:
				add_flag(o_ptr->art_flags, TR_ESP_UNDEAD);
				if (!artifact_bias && one_in_(3))
					artifact_bias = BIAS_PRIESTLY;
				else if (!artifact_bias && one_in_(6))
					artifact_bias = BIAS_NECROMANTIC;
				break;
			case 3:
				add_flag(o_ptr->art_flags, TR_ESP_DEMON);
				break;
			case 4:
				add_flag(o_ptr->art_flags, TR_ESP_DEITY);
				break;
			case 5:
				add_flag(o_ptr->art_flags, TR_ESP_KWAI);
				break;
			case 6:
				add_flag(o_ptr->art_flags, TR_ESP_GOOD);
				break;
			case 7:
				add_flag(o_ptr->art_flags, TR_ESP_HUMAN);
				if (!artifact_bias && one_in_(6))
					artifact_bias = BIAS_ROGUE;
				break;
			case 8:
				add_flag(o_ptr->art_flags, TR_ESP_DRAGON);
				break;
			}
			break;
		}
		case 38:
		case 39:
		{
			if (object_is_melee_weapon(o_ptr))
			{
				switch (randint1(7))
				{
				case 1:	case 2:
				case 3:	case 4:
					{
						u32b flgs[TR_FLAG_SIZE];
						object_flags(o_ptr, flgs);
						if(have_flag(flgs, TR_THROW)) add_flag(o_ptr->art_flags, TR_BOOMERANG);
						else add_flag(o_ptr->art_flags, TR_THROW);
					}
					break;
				case 5:
					add_flag(o_ptr->art_flags, TR_RIDING);
					break;
				case 6:
					add_flag(o_ptr->art_flags, TR_EASY_SPELL);
					break;
				case 7:
					add_flag(o_ptr->art_flags, TR_RES_INSANITY);
					break;
				}
			}
			else if (object_is_ammo(o_ptr))
			{
				o_ptr->to_h += 5 + randint1(15);
				o_ptr->to_d += 5 + randint1(15);
			}
			else if (o_ptr->tval == TV_GUN )
			{
			 	add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
			}
			else if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW  )
			{
				if(one_in_(2))	add_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
				else 	add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
			}
			else if (o_ptr->tval == TV_CLOAK ||  o_ptr->tval == TV_ARMOR || o_ptr->tval == TV_CLOTHES || o_ptr->tval == TV_DRAG_ARMOR)
			{
				switch (randint1(4))
				{
				case 1:
					add_flag(o_ptr->art_flags, TR_SUST_CHR);
					add_flag(o_ptr->art_flags, TR_SUST_INT);
					add_flag(o_ptr->art_flags, TR_SUST_WIS);
					break;
				case 2:
					add_flag(o_ptr->art_flags, TR_SUST_STR);
					add_flag(o_ptr->art_flags, TR_SUST_CON);
					add_flag(o_ptr->art_flags, TR_SUST_DEX);
					break;
				case 3:
					add_flag(o_ptr->art_flags, TR_DEC_MANA);
					break;
				case 4:
					add_flag(o_ptr->art_flags, TR_RES_INSANITY);
					break;
				}
			}
			else if (o_ptr->tval == TV_HEAD )
			{
				add_flag(o_ptr->art_flags, TR_RES_INSANITY);
			}
			else if (o_ptr->tval == TV_GLOVES )
			{
				if(one_in_(3))	add_flag(o_ptr->art_flags, TR_GENZI);
				else{
					o_ptr->to_h += 3 + randint1(7);
					o_ptr->to_d += 3 + randint1(7);
				}
			}
			else if (o_ptr->tval == TV_BOOTS )
			{
				add_flag(o_ptr->art_flags, TR_SPEEDSTER);
			}
			else if (o_ptr->tval == TV_RING )
			{
				if(one_in_(2))	add_flag(o_ptr->art_flags, TR_DEC_MANA);
				else			add_flag(o_ptr->art_flags, TR_EASY_SPELL);
			}
			else	add_flag(o_ptr->art_flags, TR_RES_INSANITY);

			break;
		}
		//���C�ϐ������΂ɏo�ɂ�������C������̂Œǉ�
		case 40:
			add_flag(o_ptr->art_flags, TR_RES_INSANITY);
			break;
	}
}

/*:::�A�[�e�B�t�@�N�g�������̃X���C�═��\�͕t��*/
///item mon
///mod131229 �������X���C�ȂǕt���@�A�C�e���t���O����ւ��Ȃǂő啝�ύX
static void random_slay(object_type *o_ptr)
{
	///mod160504 �e�����������Ƃ��̓_���[�W������ˌ���ޕω����N����
	if (o_ptr->tval == TV_GUN)
	{
		int change_mode_chance;

		//�����ύX�̋N����m���ݒ�
		switch(o_ptr->sval)
		{
		case SV_FIRE_GUN_CRIMSON:
			change_mode_chance = 70;
			break;
		case SV_FIRE_GUN_LASER:
		case SV_FIRE_GUN_MUSKET:
			change_mode_chance = 50;
			break;
		case SV_FIRE_GUN_LUNATIC:
		case SV_FIRE_GUN_SHOTGUN:
		case SV_FIRE_GUN_ROCKET:
			change_mode_chance = 33;
			break;
		default:
			change_mode_chance = 10;
			break;
		}
		//�e�̑�����ύX����
		if(randint0(100) < change_mode_chance)
		{
			int new_type = 0;
			int old_type = o_ptr->xtra1;
			if(!artifact_bias)  artifact_bias = randint1(ARTIFACT_BIAS_MAX);

			switch(artifact_bias)
			{
			case BIAS_ELEC:
				if(old_type == GF_PLASMA || one_in_(3)) new_type = GF_PLASMA;
				else new_type = GF_ELEC;
				break;
			case BIAS_POIS:
				if(old_type == GF_POLLUTE || one_in_(5)) new_type = GF_POLLUTE;
				else new_type = GF_POIS;
				break;
			case BIAS_FIRE:
				if(old_type == GF_NUKE || one_in_(4))new_type = GF_NUKE;
				else new_type = GF_FIRE;
				break;
			case BIAS_ACID:
				if(old_type == GF_WATER || one_in_(6)) new_type = GF_WATER;
				else new_type = GF_ACID;
				break;
			case BIAS_COLD:
				if(old_type == GF_ICE || one_in_(4)) new_type = GF_ICE;
				else new_type = GF_COLD;
				break;
			case BIAS_CHAOS:
				if(old_type == GF_DISENCHANT || one_in_(4)) new_type = GF_DISENCHANT;
				else new_type = GF_CHAOS;
				break;
			case BIAS_PRIESTLY:
				if(old_type == GF_PSY_SPEAR || one_in_(7)) new_type = GF_PSY_SPEAR;
				else new_type = GF_LITE;
				break;
			case BIAS_NECROMANTIC:
				if(old_type == GF_DARK || one_in_(5)) new_type = GF_DARK;
				else new_type = GF_NETHER;
				break;
			case BIAS_LAW:
				if(old_type == GF_HOLY_FIRE || one_in_(10)) new_type = GF_HOLY_FIRE;
				else new_type = GF_LITE;
				break;
			case BIAS_ROGUE:
				if(old_type == GF_SHARDS || one_in_(6))new_type = GF_SHARDS;
				else new_type = GF_MISSILE;
				break;
			case BIAS_MAGE:
				if(old_type == GF_METEOR || one_in_(4)) new_type = GF_METEOR;
				else if(old_type == GF_MANA || one_in_(3)) new_type = GF_MANA;
				else new_type = GF_MISSILE;
				break;
			case BIAS_WARRIOR:
				if(old_type == GF_FORCE || one_in_(7)) new_type = GF_FORCE;
				else new_type = GF_ARROW;
				break;
			case BIAS_RANGER:
				if(old_type == GF_DISINTEGRATE || one_in_(9)) new_type = GF_DISINTEGRATE;
				else if(old_type == GF_TORNADO || one_in_(3)) new_type = GF_TORNADO;
				else new_type = GF_ARROW;
				break;
			default:
				//�������Ȃ�
				break;
			}

			if(!new_type && object_is_cursed(o_ptr) && weird_luck()) new_type = GF_HELL_FIRE;

			if(new_type)
			{
				o_ptr->xtra1 = new_type;
				//�Ăє��肵�A�ʂ�����ˌ����@�ύX
				if(randint0(100) < change_mode_chance)
				{
					int new_mode = 0;
					switch(o_ptr->xtra4)
					{
					case GUN_FIRE_MODE_DEFAULT:
					case GUN_FIRE_MODE_BOLT:
						if(one_in_(4)) new_mode = GUN_FIRE_MODE_BEAM;
						else new_mode = GUN_FIRE_MODE_BALL;
						break;
					case GUN_FIRE_MODE_BALL:
						if(one_in_(3)) new_mode = GUN_FIRE_MODE_ROCKET;
						else new_mode = GUN_FIRE_MODE_BREATH;
						break;
					case GUN_FIRE_MODE_BEAM:
						if(one_in_(3)) new_mode = GUN_FIRE_MODE_SPARK;
						else new_mode = GUN_FIRE_MODE_BREATH;
						break;
					default:
						new_mode = GUN_FIRE_MODE_SPARK;
						break;
					}
					o_ptr->xtra4 = new_mode;
				}
				//new_type���w�肳�ꂽ�炱���ŏI���B���肪�ʂ�Ȃ�������֌W�Ȃ��o�C�A�X��new_type���w�肳��Ȃ��ꍇ�I��炸����
				return;
			}
		}

		if(one_in_(3)) //�_�C�X����
		{
			do
			{
				int new_dd = o_ptr->dd + randint1(o_ptr->dd / 2 + 1) + m_bonus(5,object_level);
				if(new_dd > 99) new_dd = 99;
				o_ptr->dd = new_dd;
			}while(!randint0(o_ptr->dd));
		}
		else if(one_in_(2)) //�_�C�X�ʑ����@���܂葝���Ȃ�
		{
			int new_ds = o_ptr->ds + randint1((o_ptr->ds + 3)/4 + 1);
			if(new_ds > 99) new_ds = 99;
			o_ptr->ds = new_ds;
		}
		else //�C���l���� �x�[�X�A�C�e���C���l�ɏ�����
		{
			int bonus_h, bonus_d;
			int mult_d = 100 + k_info[o_ptr->k_idx].to_d * 10;

			if(mult_d > 500) mult_d = 500;
			bonus_h = m_bonus(7,object_level) + (randint1(7));
			bonus_d = m_bonus(7,object_level) + (randint1(7));

			bonus_d = bonus_d * mult_d / 100 + randint1(bonus_d);

			o_ptr->to_h += bonus_h;
			o_ptr->to_d += bonus_d;

		}


		return;
	}
	else if (o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW)
	///tvsv131223
	//if (o_ptr->tval == TV_BOW)
	{
		switch (randint1(7))
		{
			///mod131229 �X���C�ɗ���m���𑝂₵���班���{�˂��o�₷������C������̂Œ���
			case 1:
				random_resistance(o_ptr);
				break;
			case 2:
				random_misc(o_ptr);
				break;
			case 3:
				random_plus(o_ptr);
				///mod �|�́���pval=0�ɂȂ邱�Ƃ�����̂Œǉ�
				if(!o_ptr->pval) o_ptr->pval = randint1(3);
				break;
			case 4:case 5:
				add_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
				if (!one_in_(7)) remove_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
				if (!artifact_bias && one_in_(9))
					artifact_bias = BIAS_RANGER;
				break;
			default:
				add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
				if (!one_in_(7)) remove_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
				if (!artifact_bias && one_in_(9))
					artifact_bias = BIAS_RANGER;
			break;
		}

		return;
	}

	switch (artifact_bias)
	{
	case BIAS_CHAOS:
		if (!(have_flag(o_ptr->art_flags, TR_CHAOTIC)))
		{
			add_flag(o_ptr->art_flags, TR_CHAOTIC);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_PRIESTLY:
		//v2.0.16 �����ʂ̐������Ȃ�����
		//if((o_ptr->tval == TV_SWORD || o_ptr->tval == TV_POLEARM || o_ptr->tval == TV_KATANA|| o_ptr->tval == TV_AXE || o_ptr->tval == TV_SPEAR ) &&
		 if(  !(have_flag(o_ptr->art_flags, TR_BLESSED)))
		{
			/* A free power for "priestly" random artifacts */
			add_flag(o_ptr->art_flags, TR_BLESSED);
		}
		break;

	case BIAS_NECROMANTIC:
		if (!(have_flag(o_ptr->art_flags, TR_VAMPIRIC)))
		{
			add_flag(o_ptr->art_flags, TR_VAMPIRIC);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)) && one_in_(2))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_RANGER:
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ROGUE:
		if ( object_is_melee_weapon(o_ptr) && !(have_flag(o_ptr->art_flags, TR_THROW)))
		{
			/* Free power for rogues... */
			add_flag(o_ptr->art_flags, TR_THROW);
		}
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_POIS:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_POIS)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_FIRE:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_FIRE)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_COLD:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_COLD)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_COLD);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ELEC:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_ELEC)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_ACID:
		if (!(have_flag(o_ptr->art_flags, TR_BRAND_ACID)))
		{
			add_flag(o_ptr->art_flags, TR_BRAND_ACID);
			if (one_in_(2)) return;
		}
		break;

	case BIAS_LAW:
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_EVIL)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
			if (one_in_(2)) return;
		}
		if (!(have_flag(o_ptr->art_flags, TR_SLAY_DEMON)))
		{
			add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
			if (one_in_(2)) return;
		}
		break;
	}

	switch (randint1(40))
	{
		case 1:
		case 2:
			if(have_flag(o_ptr->art_flags, TR_SLAY_EVIL))
				add_flag(o_ptr->art_flags, TR_KILL_EVIL);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_EVIL);

			if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_LAW;
			else if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_PRIESTLY;
			break;		
		case 3:
		case 4:
			if(have_flag(o_ptr->art_flags, TR_SLAY_GOOD))
				add_flag(o_ptr->art_flags, TR_KILL_GOOD);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_GOOD);

			if (!artifact_bias && one_in_(2))
				artifact_bias = BIAS_CHAOS;
			else if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_NECROMANTIC;
			break;
		case 5:
		case 6:
		case 7:
			if(have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL))
				add_flag(o_ptr->art_flags, TR_KILL_ANIMAL);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
			break;
		case 8:
		case 9:
		case 10:
			if(have_flag(o_ptr->art_flags, TR_SLAY_HUMAN))
				add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
			break;
		case 11:
		case 12:
		case 13:
			if(have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD))
				add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
			if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_PRIESTLY;
			break;
		case 14:
		case 15:
		case 16:
			if(have_flag(o_ptr->art_flags, TR_SLAY_DRAGON))
				add_flag(o_ptr->art_flags, TR_KILL_DRAGON);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
			break;
		case 17:
		case 18:
		case 19:
			if(have_flag(o_ptr->art_flags, TR_SLAY_DEITY))
				add_flag(o_ptr->art_flags, TR_KILL_DEITY);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
			break;
		case 20:
		case 21:
		case 22:
			if(have_flag(o_ptr->art_flags, TR_SLAY_DEMON))
				add_flag(o_ptr->art_flags, TR_KILL_DEMON);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
			if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_PRIESTLY;
			break;
		case 23:
		case 24:
		case 25:
			if(have_flag(o_ptr->art_flags, TR_SLAY_KWAI))
				add_flag(o_ptr->art_flags, TR_KILL_KWAI);
			else
				add_flag(o_ptr->art_flags, TR_SLAY_KWAI);
			break;
		case 26:
		case 27:
			if (object_has_a_blade(o_ptr))
			{
				if(have_flag(o_ptr->art_flags, TR_VORPAL))
					add_flag(o_ptr->art_flags, TR_EX_VORPAL);
				else
					add_flag(o_ptr->art_flags, TR_VORPAL);
			}
			else if( o_ptr->tval == TV_SPEAR )
			{
				o_ptr->dd++;
			}
			else
			{
				if(have_flag(o_ptr->art_flags, TR_IMPACT))
					o_ptr->to_d += damroll(2,6);
				else
				add_flag(o_ptr->art_flags, TR_IMPACT);
			}

			if (!artifact_bias && one_in_(9))
				artifact_bias = BIAS_WARRIOR;
			break;
		case 28:
		case 29:
			add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
			if (!artifact_bias)
				artifact_bias = BIAS_FIRE;
			break;
		case 30:
		case 31:
			add_flag(o_ptr->art_flags, TR_BRAND_COLD);
			if (!artifact_bias)
				artifact_bias = BIAS_COLD;
			break;
		case 32:
		case 33:
			add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
			if (!artifact_bias)
				artifact_bias = BIAS_ELEC;
			break;
		case 34:
		case 35:
			add_flag(o_ptr->art_flags, TR_BRAND_ACID);
			if (!artifact_bias)
				artifact_bias = BIAS_ACID;
			break;
		case 36:
		case 37:
			add_flag(o_ptr->art_flags, TR_BRAND_POIS);
			if (!artifact_bias && !one_in_(3))
				artifact_bias = BIAS_POIS;
			else if (!artifact_bias && one_in_(6))
				artifact_bias = BIAS_NECROMANTIC;
			else if (!artifact_bias)
				artifact_bias = BIAS_ROGUE;
			break;
		case 38:
			add_flag(o_ptr->art_flags, TR_VAMPIRIC);
			if (!artifact_bias)
				artifact_bias = BIAS_NECROMANTIC;
			break;
		case 39:
			add_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
			if (!artifact_bias)
				artifact_bias = (one_in_(2) ? BIAS_MAGE : BIAS_PRIESTLY);
			break;
		default:
			add_flag(o_ptr->art_flags, TR_CHAOTIC);
			if (!artifact_bias)
				artifact_bias = BIAS_CHAOS;
			break;
	}
}

/*:::���ւ̃����_�������t���@�o�C�A�X�e��*/
static void give_activation_power(object_type *o_ptr)
{
	int type = 0, chance = 0;

	switch (artifact_bias)
	{
		case BIAS_ELEC:
			if (!one_in_(3))
			{
				type = ACT_BO_ELEC_1;
			}
			else if (!one_in_(5))
			{
				type = ACT_BA_ELEC_2;
			}
			else
			{
				type = ACT_BA_ELEC_3;
			}
			chance = 101;
			break;

		case BIAS_POIS:
			type = ACT_BA_POIS_1;
			chance = 101;
			break;

		case BIAS_FIRE:
			if (!one_in_(3))
			{
				type = ACT_BO_FIRE_1;
			}
			else if (!one_in_(5))
			{
				type = ACT_BA_FIRE_1;
			}
			else
			{
				type = ACT_BA_FIRE_2;
			}
			chance = 101;
			break;

		case BIAS_COLD:
			chance = 101;
			if (!one_in_(3))
				type = ACT_BO_COLD_1;
			else if (!one_in_(3))
				type = ACT_BA_COLD_1;
			else if (!one_in_(3))
				type = ACT_BA_COLD_2;
			else
				type = ACT_BA_COLD_3;
			break;

		case BIAS_CHAOS:
			chance = 50;
			if (one_in_(6))
				type = ACT_SUMMON_DEMON;
			else
				type = ACT_CALL_CHAOS;
			break;

		case BIAS_PRIESTLY:
			chance = 101;

			if (one_in_(13))
				type = ACT_CHARM_UNDEAD;
			else if (one_in_(12))
				type = ACT_BANISH_EVIL;
			else if (one_in_(11))
				type = ACT_DISP_EVIL;
			else if (one_in_(10))
				type = ACT_PROT_EVIL;
			else if (one_in_(9))
				type = ACT_CURE_1000;
			else if (one_in_(8))
				type = ACT_CURE_700;
			else if (one_in_(7))
				type = ACT_REST_ALL;
			else if (one_in_(6))
				type = ACT_REST_LIFE;
			else if (one_in_(5))
				type = ACT_CURE_500;
			else
				type = ACT_CURE_MW;
			break;

		case BIAS_NECROMANTIC:
			chance = 101;
			if (one_in_(66))
				type = ACT_WRAITH;
			else if (one_in_(13))
				type = ACT_DISP_GOOD;
			else if (one_in_(9))
				type = ACT_MASS_GENO;
			else if (one_in_(8))
				type = ACT_GENOCIDE;
			else if (one_in_(13))
				type = ACT_SUMMON_UNDEAD;
			else if (one_in_(9))
				type = ACT_VAMPIRE_2;
			else if (one_in_(6))
				type = ACT_CHARM_UNDEAD;
			else
				type = ACT_VAMPIRE_1;
			break;

		case BIAS_LAW:
			chance = 101;
			if (one_in_(8))
				type = ACT_BANISH_EVIL;
			else if (one_in_(4))
				type = ACT_DISP_EVIL;
			else
				type = ACT_PROT_EVIL;
			break;

		case BIAS_ROGUE:
			chance = 101;
			if (one_in_(50))
				type = ACT_SPEED;
			else if (one_in_(4))
				type = ACT_SLEEP;
			else if (one_in_(3))
				type = ACT_DETECT_ALL;
			else if (one_in_(8))
				type = ACT_ID_FULL;
			else
				type = ACT_ID_PLAIN;
			break;

		case BIAS_MAGE:
			chance = 66;
			if (one_in_(20))
				type = ACT_SUMMON_ELEMENTAL;
			else if (one_in_(10))
				type = ACT_SUMMON_PHANTOM;
			else if (one_in_(5))
				type = ACT_RUNE_EXPLO;
			else
				type = ACT_ESP;
			break;

		case BIAS_WARRIOR:
			chance = 80;
			if (one_in_(100))
				type = ACT_INVULN;
			else
				type = ACT_BERSERK;
			break;

		case BIAS_RANGER:
			chance = 101;
			if (one_in_(20))
				type = ACT_CHARM_ANIMALS;
			else if (one_in_(7))
				type = ACT_SUMMON_ANIMAL;
			else if (one_in_(6))
				type = ACT_CHARM_ANIMAL;
			else if (one_in_(4))
				type = ACT_RESIST_ALL;
			else if (one_in_(3))
				type = ACT_SATIATE;
			else
				type = ACT_CURE_POISON;
			break;
	}

	///mod160213 ��m�����������Ńt�@�C�i���X�g���C�N�ǉ����Ă݂�
	if(object_is_melee_weapon(o_ptr) && one_in_(128))
	{
		type = ACT_FINAL_STRIKE;
		chance = 101;
	}

	if (!type || (randint1(100) >= chance))
	{
		one_activation(o_ptr);
		return;
	}

	/* A type was chosen... */
	o_ptr->xtra2 = type;
	add_flag(o_ptr->art_flags, TR_ACTIVATE);
	o_ptr->timeout = 0;
}

/*:::�����_���A�[�e�B�t�@�N�g�̖���*/
///mod160504 ����armor���폜�@o_ptr��n���ē�������
static void get_random_name(char *return_name, object_type *o_ptr, int power)
{
	int prob = randint1(100);

	if (prob <= SINDARIN_NAME)
	{
		get_table_sindarin(return_name, TRUE);
	}
	else if (prob <= TABLE_NAME)
	{
		get_table_name(return_name);
	}
	else
	{
		cptr filename;

		int name_type = 0;

		//v1.1.60 ������W�A�C�e��������h��e�̂ǂ̃����_���������悤�ɂ���
		if (o_ptr->tval == TV_GUN)
			name_type = 0;
		else if (object_is_armour(o_ptr) || o_ptr->tval == TV_RIBBON || o_ptr->tval == TV_LITE)
			name_type = 1;
		else 
			name_type = 2;

		if (o_ptr->tval == TV_ANTIQUE)
			name_type = randint0(3);

		switch (name_type)
		{
		case 0:
			//�ˌ����햽��
			{
				switch (power)
				{
				case 0:
					filename = "f_cursed_j.txt";
					break;
				case 1:
					filename = "f_low_j.txt";
					break;
				case 2:
					filename = "f_med_j.txt";
					break;
				default:
					filename = "f_high_j.txt";
				}
			}
			break;
		case 1:
			//�h��ޖ���
			{
				switch (power)
				{
				case 0:
					filename = "a_cursed_j.txt";
					break;
				case 1:
					filename = "a_low_j.txt";
					break;
				case 2:
					filename = "a_med_j.txt";
					break;
				default:
					filename = "a_high_j.txt";
				}
			}
			break;
		default:
			{
				switch (power)
				{
				case 0:
					filename = "w_cursed_j.txt";
					break;
				case 1:
					filename = "w_low_j.txt";
					break;
				case 2:
					filename = "w_med_j.txt";
					break;
				default:
					filename = "w_high_j.txt";
				}
			}
			break;
		}
		/*:::artifact_bias���l������Őݒ肵���t�@�C�����烉���_���Ɉ�s����B*/
		/*:::�����łȂ������o�C�A�X�����K�p����Ė��O���t���Ă��邪�Arandom_plus()��random_resistance()�ȂǂŊm����artifact_bias���ݒ肳��Ă��邱�Ƃɂ��B*/

		(void)get_rnd_line(filename, artifact_bias, return_name);
#ifdef JP
		 if (return_name[0] == 0) get_table_name(return_name);
#endif
	}
}

/*:::�A�[�e�B�t�@�N�g���� (scroll:�����g�p)*/
//�w�ւ�A�~����apply_magic()��ʂ����ɂ����ɔ�΂����ꍇpval��0�̂܂܂ɂȂ��Ă���B

bool create_artifact(object_type *o_ptr, bool a_scroll)
{
	char    new_name[1024];
	int     has_pval = 0;/*:::bool�ɂ��Ȃ����R���킩��Ȃ�*/
	int     powers = randint1(5) + 1;
	int     max_type = (object_is_weapon_ammo(o_ptr) ? 8 : 6);
	int     power_level;
	s32b    total_flags;
	bool    a_cursed = FALSE;
	int     warrior_artifact_bias = 0;
	int i;

	if(o_ptr->tval == TV_LITE) max_type = 5;

	/* Reset artifact bias */
	artifact_bias = 0;

	/* Nuke enchantments */
	o_ptr->name1 = 0;
	o_ptr->name2 = 0;

	/*:::�x�[�X�A�C�e���̃t���O��o_ptr��art_flags�Ƃ��ď������ݒ����Ă���*/
	for (i = 0; i < TR_FLAG_SIZE; i++)
		o_ptr->art_flags[i] |= k_info[o_ptr->k_idx].flags[i];


	if (o_ptr->pval) has_pval = TRUE;

	/*:::�����g�p���ɂ�1/4�̊m���ł��̐E�ƌ����̐��\�ɂȂ�炵�� �ʓ|�Ȃ̂ŏ����Ă��܂������H*/
	/*:::�ł���m�����▂�p�t�����̃o�C�A�X���Ă͖̂ʔ����̂ŊȈՂɏ����������@class_info[p_ptr->pclass]����Z�o����悤�ɂ���΃N���X�ǉ��̂��тɏ��������Ȃ��Ă���*/
	/// sysdel class
	///pend�@�A�[�e�B�t�@�N�g�o�C�A�X�@�ꎞ������
/*
	if (a_scroll && one_in_(4))
	{
		switch (p_ptr->pclass)
		{
			case CLASS_WARRIOR:
			case CLASS_BERSERKER:
			case CLASS_ARCHER:
			case CLASS_SAMURAI:
			case CLASS_CAVALRY:
			case CLASS_SMITH:
				artifact_bias = BIAS_WARRIOR;
				break;
			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			case CLASS_SORCERER:
			case CLASS_MAGIC_EATER:
			case CLASS_BLUE_MAGE:
				artifact_bias = BIAS_MAGE;
				break;
			case CLASS_PRIEST:
				artifact_bias = BIAS_PRIESTLY;
				break;
			case CLASS_ROGUE:
			case CLASS_NINJA:
				artifact_bias = BIAS_ROGUE;
				warrior_artifact_bias = 25;
				break;
			case CLASS_RANGER:
			case CLASS_SNIPER:
				artifact_bias = BIAS_RANGER;
				warrior_artifact_bias = 30;
				break;
			case CLASS_PALADIN:
				artifact_bias = BIAS_PRIESTLY;
				warrior_artifact_bias = 40;
				break;
			case CLASS_WARRIOR_MAGE:
			case CLASS_RED_MAGE:
				artifact_bias = BIAS_MAGE;
				warrior_artifact_bias = 40;
				break;
			case CLASS_CHAOS_WARRIOR:
			//case CLASS_MAID:
				artifact_bias = BIAS_CHAOS;
				warrior_artifact_bias = 40;
				break;
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
				artifact_bias = BIAS_PRIESTLY;
				break;
			case CLASS_MINDCRAFTER:
			case CLASS_BARD:
				if (randint1(5) > 2) artifact_bias = BIAS_PRIESTLY;
				break;
			case CLASS_TOURIST:
				if (randint1(5) > 2) artifact_bias = BIAS_WARRIOR;
				break;
			case CLASS_IMITATOR:
				if (randint1(2) > 1) artifact_bias = BIAS_RANGER;
				break;
			case CLASS_BEASTMASTER:
				artifact_bias = BIAS_CHR;
				warrior_artifact_bias = 50;
				break;
			case CLASS_MIRROR_MASTER:
				if (randint1(4) > 1) 
				{
				    artifact_bias = BIAS_MAGE;
				}
				else
				{
				    artifact_bias = BIAS_ROGUE;
				}
				break;
		}
	}
	if (a_scroll && (randint1(100) <= warrior_artifact_bias))
		artifact_bias = BIAS_WARRIOR;
*/

	strcpy(new_name, "");

	if (!a_scroll && one_in_(A_CURSED))/*:::1/13*/
		a_cursed = TRUE;
	if (((o_ptr->tval == TV_AMULET) || (o_ptr->tval == TV_RING)) && object_is_cursed(o_ptr))
		a_cursed = TRUE;

	while (one_in_(powers) || one_in_(7) || weird_luck())
		powers++;

	///mod131229 �[�w�قǗǂ������o�₷�����Ă݂�
	powers += m_bonus(4,object_level);
	
	if (!a_cursed && weird_luck())
		powers *= 2;

	//v1.1.60 
	if (o_ptr->tval == TV_ANTIQUE) powers += randint1(5) + randint0(o_ptr->sval);


	if (a_cursed) powers /= 2;

	if(powers > 100) powers = 100;

	/*:::powers�͏���2-6,��m����10�𒴂��邭�炢��*/
	/*:::�e�X�g���Ă݂��畽��5 95%��10������20�z����10000��ɐ��x*/
	/*:::�F�X�������������ʒ����́��������o�₷���Ȃ����B*/
	/* Main loop */
	while (powers--)
	{
		switch (randint1(max_type))
		{
			/*:::pval�\�̓A�b�v�t���O�@�B���▂���������*/
			case 1: case 2:
				if (object_is_ammo(o_ptr))
				{
					; //��������case3,4��
				}
				else
				{
					random_plus(o_ptr);
					has_pval = TRUE;
					break;
				}
			/*:::�ߐڕ���̂Ƃ��_�C�X�u�[�X�g�̃`�����X�@�����łȂ��Ƃ��ϐ��𓾂�*/
			//v2.0.16 ��Ȃǂ��_�C�X�u�[�X�g����
			case 3: case 4:

				if (one_in_(2) && object_is_melee_weapon(o_ptr) || object_is_ammo(o_ptr))
				{
					if (a_cursed && !one_in_(13)) break;
					if (one_in_(13))
					{
						if (one_in_(o_ptr->ds+4)) o_ptr->ds++;
					}
					else
					{
						if (one_in_(o_ptr->dd+1)) o_ptr->dd++;
					}
				}
				else
					random_resistance(o_ptr);
				break;
			/*:::�\�͕t���@�ێ���e���p�X�Ȃ�*/
			case 5:
				//��ʂ͒�m���Ń_���[�W�u�[�X�g
				//random_misc()�ł̃u�[�X�g�����݂̂��G�ɔ��f��������
				if (object_is_ammo(o_ptr))
				{
					if (one_in_(7))
					{
						o_ptr->to_h += 4 + randint1(11);
						o_ptr->to_d += 4 + randint1(11);
					}
				}
				else
				{
					random_misc(o_ptr);
				}
				break;
				///mod131229 ����ɃX���C���t���₷���A�h��ɔ\�͂��t���₷������
			case 6:
				if(object_is_weapon_ammo(o_ptr)) random_slay(o_ptr);
				else random_misc(o_ptr);
				break;
			case 7: case 8:
			/*:::�X���C�@�ǉ��ˌ���z���U���Ȃǂ�����*/
				random_slay(o_ptr);
				break;
			default:
				if (p_ptr->wizard) msg_print("Switch error in create_artifact!");
				powers++;
		}
	};

	//�e�p�����[�^�␳
	if(o_ptr->tval == TV_GUN)
	{
		int tmp_dd;
		int need_turn = gun_base_param_table[o_ptr->sval].charge_turn;
		int base_dd = k_info[o_ptr->k_idx].dd;
		int base_ds = k_info[o_ptr->k_idx].ds;
		int base_to_d = k_info[o_ptr->k_idx].to_d;
		int base_to_h = k_info[o_ptr->k_idx].to_h;

		//���܂�З͂��オ�肷���Ȃ��悤������x������������K�v������B
		if(o_ptr->dd > 99) o_ptr->dd = 99;
		if(o_ptr->dd > (need_turn * 3 + 24) && !weird_luck()) o_ptr->dd = (need_turn * 3 + 24);
		if(o_ptr->dd > (base_dd * 2 + 8) && !weird_luck()) o_ptr->dd = base_dd * 2 + 8 + randint0(o_ptr->dd - base_dd * 2 - 8);

		if(o_ptr->ds > 99) o_ptr->ds = 99;
		if(o_ptr->ds > base_ds * 2 && !weird_luck()) o_ptr->ds = base_ds * 2;

		if(o_ptr->to_d > 999) o_ptr->to_d = 999;
		if(o_ptr->to_d > 30 && o_ptr->to_d > base_to_d * 3 && !weird_luck()) o_ptr->to_d = base_to_d * 3 + randint0(o_ptr->to_d - base_to_d * 3);

		if(o_ptr->to_h > 50) o_ptr->to_h = 50;
		if(o_ptr->to_h > 30 && o_ptr->to_h > base_to_h * 3 && !weird_luck()) o_ptr->to_h = base_to_h * 3 + randint0(o_ptr->to_h - base_to_h * 3);

		//�����ɂ���ă_�C�X�ɕ␳
		tmp_dd = o_ptr->dd;
		switch(o_ptr->xtra1)
		{
		//���f�n�͋����Ȃ�
		case GF_FIRE: case GF_ELEC: case GF_ACID:case GF_COLD:case GF_POIS:
		case GF_NUKE: case GF_PLASMA:
			tmp_dd += (tmp_dd) + randint1(tmp_dd);
			break;
		case GF_METEOR: case GF_CHAOS: case GF_NETHER: case GF_POLLUTE:
			tmp_dd += (tmp_dd+2)/3 + randint0(tmp_dd/3);
			break;
		//��ʑ���
		case GF_LITE: case GF_DARK: case GF_SHARDS: case GF_MANA: case GF_DISENCHANT:
		case GF_TORNADO:
			tmp_dd += (tmp_dd+3)/4 + randint0(tmp_dd/4);
			break;
		//���⍌���ȂǗL�p�Ȃ��͕̂ω��Ȃ�
		default:
			break;
		}
		if(tmp_dd > 255) tmp_dd = 255;
		o_ptr->dd = tmp_dd;

		//�ˌ���ARROW�łȂ��Ƃ����@���ނ��{���g�ɂ���
		if(o_ptr->xtra1 != GF_ARROW && o_ptr->xtra4 == GUN_FIRE_MODE_DEFAULT) 
			o_ptr->xtra4 = GUN_FIRE_MODE_BOLT;

		//�ˌ����{���g�łȂ��Ȃ����Ƃ�������MISSILE�ɂ���
		if(o_ptr->xtra1 == GF_ARROW && o_ptr->xtra4 != GUN_FIRE_MODE_DEFAULT) 
			o_ptr->xtra1 = GF_MISSILE;

	}


	if (has_pval)
	{
#if 0
		add_flag(o_ptr->art_flags, TR_SHOW_MODS);

		/* This one commented out by gw's request... */
		if (!a_scroll)
			add_flag(o_ptr->art_flags, TR_HIDE_TYPE);
#endif
		/*:::�ǉ��U���̂Ƃ���pval�����炳���*/
		if (have_flag(o_ptr->art_flags, TR_BLOWS))
		{
			if(o_ptr->pval > 2) o_ptr->pval = 2;
			else if(o_ptr->pval > 1 && one_in_(2)) o_ptr->pval = 2;
			else o_ptr->pval = randint1(2);
			///mod131223
			//if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_HAYABUSA))
			if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_HAYABUSA))
				o_ptr->pval++;
		}
		else
		{
			bool flag_no_limit = FALSE;
			//�X�s�w�́����������ς����B��pval�̃X�s�w��pval���ێ���������ɑ���pval�t���O��������A��������pval4�ɂȂ�
			if (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_SPEED)
			{
				if(have_flag(o_ptr->art_flags, TR_STR) || have_flag(o_ptr->art_flags, TR_INT) ||
				   have_flag(o_ptr->art_flags, TR_WIS) || have_flag(o_ptr->art_flags, TR_DEX) ||
				   have_flag(o_ptr->art_flags, TR_CON) || have_flag(o_ptr->art_flags, TR_CHR) ||
				   have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY) || have_flag(o_ptr->art_flags, TR_STEALTH))
				{
					if(randint1(o_ptr->pval) > 4)
					{
						remove_flag(o_ptr->art_flags,TR_STR);
						remove_flag(o_ptr->art_flags,TR_INT);
						remove_flag(o_ptr->art_flags,TR_WIS);
						remove_flag(o_ptr->art_flags,TR_DEX);
						remove_flag(o_ptr->art_flags,TR_CON);
						remove_flag(o_ptr->art_flags,TR_CHR);
						remove_flag(o_ptr->art_flags,TR_MAGIC_MASTERY);
						remove_flag(o_ptr->art_flags,TR_STEALTH);
						flag_no_limit = TRUE;
					}
					else
					{
						if(o_ptr->pval > 4) o_ptr->pval = 4;
					}
				}
				else
				{
						flag_no_limit = TRUE;
				}
			}
			//���҃A�~���̐����K�����ύX�@�T����ԊO���ȊO��pval�����Ă��pval��4��
			else if(o_ptr->tval == TV_AMULET && o_ptr->sval == SV_AMULET_THE_MAGI)
			{
				if(have_flag(o_ptr->art_flags, TR_STR) || have_flag(o_ptr->art_flags, TR_INT) ||
				   have_flag(o_ptr->art_flags, TR_WIS) || have_flag(o_ptr->art_flags, TR_DEX) ||
				   have_flag(o_ptr->art_flags, TR_CON) || have_flag(o_ptr->art_flags, TR_CHR) ||
				   have_flag(o_ptr->art_flags, TR_MAGIC_MASTERY) || have_flag(o_ptr->art_flags, TR_STEALTH) ||
				   have_flag(o_ptr->art_flags, TR_SPEED)  )
				{
					if(o_ptr->pval > 4) o_ptr->pval = 4;
				}
				else
				{
					flag_no_limit = TRUE;
				}
			}
			//���̑����i��pval��4�𒴂��Ă���Ƃ���4�ɂ���B�r�͂̎w�ւȂ�
			else
			{
				if(o_ptr->pval > 4) o_ptr->pval = 4;
			}

			do
			{
				o_ptr->pval++;
			}
			while (o_ptr->pval < randint1(5) || one_in_(o_ptr->pval));

			if ((o_ptr->pval > 4) && !(weird_luck()) && !flag_no_limit) o_ptr->pval = 4;
		}

	}

	/* give it some plusses... */
	/*:::20�ȉ��̏C���l��20�܂ŏオ��`�����X������*/
	if (object_is_armour(o_ptr))
		o_ptr->to_a += randint1(o_ptr->to_a > 19 ? 1 : 20 - o_ptr->to_a);
	else if (object_is_weapon_ammo(o_ptr))
	{
		o_ptr->to_h += randint1(o_ptr->to_h > 19 ? 1 : 20 - o_ptr->to_h);
		o_ptr->to_d += randint1(o_ptr->to_d > 19 ? 1 : 20 - o_ptr->to_d);
		if ((have_flag(o_ptr->art_flags, TR_WIS)) && (o_ptr->pval > 0)) add_flag(o_ptr->art_flags, TR_BLESSED);
	}

	/* Just to be sure */
	/*:::���f����ی�*/
	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
	add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(o_ptr->art_flags, TR_IGNORE_COLD);

	/*:::����ꂽ�t���O�̉��l�𐔒l��*/
	total_flags = flag_cost(o_ptr, o_ptr->pval,FALSE);
	if (cheat_peek) msg_format("%ld", total_flags);

	if (a_cursed) curse_artifact(o_ptr);

	
	/*:::�����\�͕t���@����1/3,�h��1/6�@��ɂ����̂��H*/
	///mod160504 �e�ɂ͂��Ȃ��悤�ɂ���
	//v2.0.16 ��ɂ����Ȃ��悤�ɂ���
	if (!a_cursed && o_ptr->tval != TV_GUN && !object_is_ammo(o_ptr) &&
	    one_in_(object_is_armour(o_ptr) ? ACTIVATION_CHANCE * 2 : ACTIVATION_CHANCE))
	{
		o_ptr->xtra2 = 0;
		give_activation_power(o_ptr);
	}

	/*:::�h��ւ̎E�C�C���t���E�E�E�H�s�������t�̂悤�ȋC�����邪*/
	/*:::�Ǝv���Ă悭������t���������E�C�C�������������鏈���������B*/
	if (object_is_armour(o_ptr))
	{
		while ((o_ptr->to_d+o_ptr->to_h) > 20)
		{
			if (one_in_(o_ptr->to_d) && one_in_(o_ptr->to_h)) break;
			o_ptr->to_d -= (s16b)randint0(3);
			o_ptr->to_h -= (s16b)randint0(3);
		}
		while ((o_ptr->to_d+o_ptr->to_h) > 10)
		{
			if (one_in_(o_ptr->to_d) || one_in_(o_ptr->to_h)) break;
			o_ptr->to_d -= (s16b)randint0(3);
			o_ptr->to_h -= (s16b)randint0(3);
		}
	}

	if (((artifact_bias == BIAS_MAGE) || (artifact_bias == BIAS_INT)) && (o_ptr->tval == TV_GLOVES)) add_flag(o_ptr->art_flags, TR_FREE_ACT);

	/*:::�Őj�̑����̃t���O������*/
	///mod131223 dokubari
	//if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI))
	if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI))

	{
		o_ptr->to_h = 0;
		o_ptr->to_d = 0;
		remove_flag(o_ptr->art_flags, TR_BLOWS);
		remove_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
		remove_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
		remove_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		remove_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
		remove_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		//remove_flag(o_ptr->art_flags, TR_SLAY_ORC);
		//remove_flag(o_ptr->art_flags, TR_SLAY_TROLL);
		//remove_flag(o_ptr->art_flags, TR_SLAY_GIANT);
		remove_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
		remove_flag(o_ptr->art_flags, TR_KILL_DRAGON);
		remove_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		remove_flag(o_ptr->art_flags, TR_VORPAL);
		remove_flag(o_ptr->art_flags, TR_BRAND_POIS);
		remove_flag(o_ptr->art_flags, TR_BRAND_ACID);
		remove_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		remove_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		remove_flag(o_ptr->art_flags, TR_BRAND_COLD);
	}

	//v2.1.0 ����̓A�C�e���̎􂢂𖳌��� �􂢃t���O��object_flags()�Ŗ�������̂ł����ł�pval�Ȃǂ̃}�C�i�X��0�ɂ���
	if (p_ptr->pclass == CLASS_MIZUCHI && object_is_equipment(o_ptr))
	{
		if (o_ptr->to_a < 0) o_ptr->to_a = 0;
		if (o_ptr->to_d < 0) o_ptr->to_d = 0;
		if (o_ptr->to_h < 0) o_ptr->to_h = 0;
		if (o_ptr->pval < 0) o_ptr->pval = 0;
	}



	/*:::power_level�̓����_�������̃����N�Ɏg����*/
	if (!object_is_weapon_ammo(o_ptr))
	{
		/* For armors */
		if (a_cursed) power_level = 0;
		else if (total_flags < 15000) power_level = 1;
		else if (total_flags < 35000) power_level = 2;
		else power_level = 3;
	}
	else if (o_ptr->tval == TV_GUN)
	{
		/* For armors */
		if (a_cursed) power_level = 0;
		else if (total_flags < 25000) power_level = 1;
		else if (total_flags < 60000) power_level = 2;
		else power_level = 3;
	}
	else
	{
		/* For weapons */
		if (a_cursed) power_level = 0;
		else if (total_flags < 20000) power_level = 1;
		else if (total_flags < 45000) power_level = 2;
		else power_level = 3;
	}

	if (a_scroll)
	{
		char dummy_name[80] = "";
#ifdef JP
		cptr ask_msg = "���̃A�[�e�B�t�@�N�g�����Ɩ��t���܂����H";
#else
		cptr ask_msg = "What do you want to call the artifact? ";
#endif

		/* Identify it fully */
		/*:::��������*�Ӓ�*�ς݁@*/
		object_aware(o_ptr);
		object_known(o_ptr);

		/* Mark the item as fully known */
		o_ptr->ident |= (IDENT_MENTAL);

		/* For being treated as random artifact in screen_object() */
		o_ptr->art_name = quark_add("");

		(void)screen_object(o_ptr, 0L);

		if (!get_string(ask_msg, dummy_name, sizeof dummy_name)
		    || !dummy_name[0])
		{
			/* Cancelled */
			if (one_in_(2))
			{
				get_table_sindarin_aux(dummy_name);
			}
			else
			{
				get_table_name_aux(dummy_name);
			}
		}

#ifdef JP
		sprintf(new_name, "�s%s�t", dummy_name);
#else
		sprintf(new_name, "'%s'", dummy_name);
#endif
		/*:::��������������ꕔ�̓��������@�폜�\��*/
		///del131223 virtue
		//chg_virtue(V_INDIVIDUALISM, 2);
		//chg_virtue(V_ENCHANT, 5);
	}
	else
	{
		/*:::�V���_�����A�e�[�u���ȊO�̃����_���A�[�e�B�t�@�N�g��������*/
		get_random_name(new_name, o_ptr, power_level);
	}

	if (cheat_xtra)
	{
#ifdef JP
		if (artifact_bias) msg_format("�^�̕΂����A�[�e�B�t�@�N�g: %d�B", artifact_bias);
		else msg_print("�A�[�e�B�t�@�N�g�ɉ^�̕΂�Ȃ��B");
#else
		if (artifact_bias) msg_format("Biased artifact: %d.", artifact_bias);
		else msg_print("No bias in artifact.");
#endif
	}

	/* Save the inscription */
	o_ptr->art_name = quark_add(new_name);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	//v1.1.25 ���莞���x���ƃ^�[�����L�^
	o_ptr->create_turn = turn;
	o_ptr->create_lev = dun_level;


	return TRUE;
}


int activation_index(object_type *o_ptr)
/*:::�A�C�e���̔����C���f�b�N�X(activation_info[]�̓Y����)���擾����*/
/*:::���@�̓J�ƃ����X�^�[�{�[���͂����ŃC���f�b�N�X���Ԃ�Ȃ�*/
{
	/* Give priority to weaponsmith's essential activations */
	/*:::�b��t�p�ǉ�����������Ƃ�*/
	if (object_is_smith(o_ptr))
	{
		switch (o_ptr->xtra3-1)
		{
		case ESSENCE_TMP_RES_ACID: return ACT_RESIST_ACID;
		case ESSENCE_TMP_RES_ELEC: return ACT_RESIST_ELEC;
		case ESSENCE_TMP_RES_FIRE: return ACT_RESIST_FIRE;
		case ESSENCE_TMP_RES_COLD: return ACT_RESIST_COLD;
		case TR_IMPACT: return ACT_QUAKE;

			//v1.1.74
		case ESSENCE_TMP_RES_ALL: return ACT_RESIST_ALL;
		case ESSENCE_WHIRL_WIND: return ACT_WHIRLWIND;
		case ESSENCE_PSY_SPEAR: return ACT_PSY_SPEAR;
		case ESSENCE_L_RANGE_ATTACK: return ACT_LONG_RANGE_ATTACK;
		case ESSENCE_HIT_AND_AWAY: return ACT_HIT_AND_AWAY;
		case ESSENCE_HEAL_500: return ACT_CURE_500;
		case ESSENCE_INVULN: return ACT_INVULN;
		case ESSENCE_SS_ARROW: return ACT_SAINT_STAR_ARROW;
		case ESSENCE_SUPER_STEALTH: return ACT_TIM_SUPER_STEALTH;
		case ESSENCE_DIM_DOOR: return ACT_DIM_DOOR;
		case ESSENCE_DUAL_CAST: return ACT_DUAL_CAST;
		case ESSENCE_TIM_INC_DICE: return ACT_TIM_ADD_DICES;

		}
	}

	/*:::�Œ�A�[�e�B�t�@�N�g��a_info�ɔ����C���f�b�N�X���i�[�ς�*/
	if (object_is_fixed_artifact(o_ptr))
	{
		if (have_flag(a_info[o_ptr->name1].flags, TR_ACTIVATE))
		{
			return a_info[o_ptr->name1].act_idx;
		}
	}
	if (object_is_ego(o_ptr))
	{
		if (have_flag(e_info[o_ptr->name2].flags, TR_ACTIVATE))
		{
			return e_info[o_ptr->name2].act_idx;
		}
	}
	/*:::�Ή��̎w�ւȂǒʏ�A�C�e���H*/
	if (!object_is_random_artifact(o_ptr))
	{
		if (have_flag(k_info[o_ptr->k_idx].flags, TR_ACTIVATE))
		{
			return k_info[o_ptr->k_idx].act_idx;
		}
	}
	/*:::�����_���A�[�e�B�t�@�N�g*/
	return o_ptr->xtra2;
}
/*:::�����̖��O�A��Փx�Ȃǂ̏��𓾂�Bactivation_info[]�̒������v������̂���Ԃ��B*/
const activation_type* find_activation_info(object_type *o_ptr)
{
	/*:::k_info,a_info�Ȃǂɏ����ꂽ�����^�O SUNLIGHT�Ȃ�*/
	const int index = activation_index(o_ptr);

	const activation_type* p;

	for (p = activation_info; p->flag != NULL; ++ p) {
		if (p->index == index)
		{
			return p;
		}
	}

	return NULL;
}


/* Dragon breath activation */
/*:::�h���S���Z�̔�������*/
static bool activate_dragon_breath(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE]; /* for resistance flags */
	int type[20];
	cptr name[20];
	int i, dir, t, n = 0;

	if (!get_aim_dir(&dir)) return FALSE;

	object_flags(o_ptr, flgs);

	for (i = 0; dragonbreath_info[i].flag != 0; i++)
	{
		if (have_flag(flgs, dragonbreath_info[i].flag))
		{
			type[n] = dragonbreath_info[i].type;
			name[n] = dragonbreath_info[i].name;
			n++;
		}
	}

	/* Paranoia */
	if (n == 0) return FALSE;

	/* Stop speaking */
	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any()) stop_hex_spell_all();

	t = randint0(n);
	msg_format(_("���Ȃ���%s�̃u���X��f�����B", "You breathe %s."), name[t]);
	fire_ball(type[t], dir, 250, -4);

	return TRUE;
}


//v1.1.85 �A�[�e�B�t�@�N�g�����ɂ�郂���X�^�[�ߊl
//do_cmd_activate_aux()�ɋL�q���ꂽTV_CAPTURE�̔����Ə����͓��������p�����[�^���ۑ������ꏊ���Ⴄ
//xtra6:�����X�^�[idx
//xtra7:�����X�^�[�����l
//xtra4:�����X�^�[HP
//xtra5:�����X�^�[MHP
static bool activate_artifact_capture(object_type *o_ptr)
{

	//r_idx���ۊǂ���Ă��Ȃ��Ƃ��ߊl����
	if(!o_ptr->xtra6)
	{
		int dir;
		bool old_target_pet = target_pet;
		target_pet = TRUE;
		if (!get_aim_dir(&dir))
		{
			target_pet = old_target_pet;
			return TRUE;
		}
		target_pet = old_target_pet;

		if (fire_ball(GF_CAPTURE, dir, 0, 0))
		{
			/*:::�����X�^�[�̃p�����[�^�������X�^�[�{�[���Ɋi�[�@��p�O���[�o���ϐ��g�p*/
			o_ptr->xtra6 = cap_mon;
			o_ptr->xtra7 = cap_mspeed;
			o_ptr->xtra4 = cap_hp;
			o_ptr->xtra5 = cap_maxhp;
			if (cap_nickname)
			{
				cptr t;
				char *s;
				char buf[160] = "";

				if (o_ptr->inscription)
					strcpy(buf, quark_str(o_ptr->inscription));
				s = buf;
				for (s = buf; *s && (*s != '#'); s++)
				{
					if (iskanji(*s)) s++;
				}
				*s = '#';
				s++;

				t = quark_str(cap_nickname);
				while (*t)
				{
					*s = *t;
					s++;
					t++;
				}

				*s = '\0';
				o_ptr->inscription = quark_add(buf);
			}
		}


	}
	else
	{
		int dir;
		bool release_success = FALSE;
		//v1.1.16 �d���{�p�����[�^�s���ɑΉ�
		if (o_ptr->xtra6 < 0 || o_ptr->xtra6 >= max_r_idx)
		{
			msg_format("ERROR:�����ߊl�A�C�e����pval�l����������(%d)", o_ptr->xtra6);
			return FALSE;
		}

		if (!get_rep_dir2(&dir)) return TRUE;

		if (monster_can_enter(py + ddy[dir], px + ddx[dir], &r_info[o_ptr->xtra6], 0))
		{
			if (place_monster_aux(0, py + ddy[dir], px + ddx[dir], o_ptr->xtra6, (PM_FORCE_PET | PM_NO_KAGE)))
			{
				if (o_ptr->xtra7) m_list[hack_m_idx_ii].mspeed = o_ptr->xtra7;
				if (o_ptr->xtra5) m_list[hack_m_idx_ii].max_maxhp = o_ptr->xtra5;
				if (o_ptr->xtra4) m_list[hack_m_idx_ii].hp = o_ptr->xtra4;
				m_list[hack_m_idx_ii].maxhp = m_list[hack_m_idx_ii].max_maxhp;
				if (o_ptr->inscription)
				{
					char buf[160];
					cptr t;

					t = quark_str(o_ptr->inscription);
					for (t = quark_str(o_ptr->inscription); *t && (*t != '#'); t++)
					{
						if (iskanji(*t)) t++;
					}
					if (*t)
					{
						char *s = buf;
						t++;

						while (*t)
						{
							*s = *t;
							t++;
							s++;
						}
						*s = '\0';
						m_list[hack_m_idx_ii].nickname = quark_add(buf);
						t = quark_str(o_ptr->inscription);
						s = buf;
						while (*t && (*t != '#'))
						{
							*s = *t;
							t++;
							s++;
						}
						*s = '\0';
						o_ptr->inscription = quark_add(buf);
					}
				}
				o_ptr->xtra6 = 0;
				o_ptr->xtra7 = 0;
				o_ptr->xtra4 = 0;
				o_ptr->xtra5 = 0;
				release_success = TRUE;
			}
		}
		if (!release_success)
			msg_print("�����ƁA����Ɏ��s�����B");
	}


	return TRUE;


}



//v1.1.85 �A�[�e�B�t�@�N�g�u�v���e�E�X�����O�v�ł̃����_���ϐg�p
static bool mon_hook_random_metamorphose(int r_idx)
{

	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->level > p_ptr->lev + 10 || r_ptr->level < p_ptr->lev - 10) return FALSE;

	if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2 || r_ptr->flags7 & RF7_VARIABLE) return FALSE;

	if (r_ptr->rarity >= 50) return FALSE;

	return TRUE;


}



/*:::�A�C�e���������ʁ@�ʂɃ����_���A�[�e�B�t�@�N�g��p�ł͂Ȃ��炵��*/
///item class
///mod160213 �t�@�C�i���X�g���C�N�p��item���擾
bool activate_random_artifact(object_type *o_ptr, int item)
{
	int plev = p_ptr->lev;
	int k, dir, dummy = 0;
	///mod160122 k_name�̃x�[�X�����Ǝw�ւȂǂ̔����Łu�����̎w�ցv�ƕ\������Ȃ��̂ŏC��
	//cptr name = k_name + k_info[o_ptr->k_idx].name;

	char name[80];
	const activation_type* const act_ptr = find_activation_info(o_ptr);

	/* Paranoia */
	if (!act_ptr) {
		/* Maybe forgot adding information to activation_info table ? */
		msg_print("Activation information is not found.");
		return FALSE;
	}

	object_desc(name,o_ptr,OD_NAME_ONLY|OD_NO_PLURAL);

	/* Activate for attack */
	switch (act_ptr->index)
	{
		case ACT_SUNLIGHT:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���z�����������ꂽ�B");
#else
			msg_print("A line of sunlight appears.");
#endif
			(void)lite_line(dir, damroll(6, 8));
			break;
		}

		case ACT_BO_MISS_1:
		{
#ifdef JP
			msg_print("�����ῂ������炢�ɖ��邭�P���Ă���...");
#else
			msg_print("It glows extremely brightly...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_MISSILE, dir, damroll(2, 6));
			break;
		}

		case ACT_BA_POIS_1:
		{
#ifdef JP
			msg_print("����͔Z�ΐF�ɖ������Ă���...");
#else
			msg_print("It throbs deep green...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_POIS, dir, 12, 3);
			break;
		}

		case ACT_BO_ELEC_1:
		{
#ifdef JP
			msg_print("����͉ΉԂɕ���ꂽ...");
#else
			msg_print("It is covered in sparks...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ELEC, dir, damroll(4, 8));
			break;
		}

		case ACT_BO_ACID_1:
		{
#ifdef JP
			msg_print("����͎_�ɕ���ꂽ...");
#else
			msg_print("It is covered in acid...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ACID, dir, damroll(5, 8));
			break;
		}

		case ACT_BO_COLD_1:
		{
#ifdef JP
			msg_print("����͑��ɕ���ꂽ...");
#else
			msg_print("It is covered in frost...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_COLD, dir, damroll(6, 8));
			break;
		}

		case ACT_BO_FIRE_1:
		{
#ifdef JP
			msg_print("����͉��ɕ���ꂽ...");
#else
			msg_print("It is covered in fire...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_FIRE, dir, damroll(9, 8));
			break;
		}

		case ACT_BA_COLD_1:
		{
#ifdef JP
			msg_print("����͑��ɕ���ꂽ...");
#else
			msg_print("It is covered in frost...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 48, 2);
			break;
		}

		case ACT_BA_FIRE_1:
		{
#ifdef JP
			msg_print("����͐Ԃ��������P����...");
#else
			msg_print("It glows an intense red...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 72, 2);
			break;
		}

		case ACT_DRAIN_1:
		{
#ifdef JP
			///msg131214
			//msg_format("���Ȃ���%s�ɓG����ߎE���悤�������B", name);
			msg_format("���Ȃ���%s�ɓG����ߕt����悤�������B", name);

#else
			msg_format("You order the %s to strangle your opponent.", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			if (drain_life(dir, 100))
			break;
		}

		case ACT_BA_COLD_2:
		{
#ifdef JP
			msg_print("����͐��������P����...");
#else
			msg_print("It glows an intense blue...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 100, 2);
			break;
		}

		case ACT_BA_ELEC_2:
		{
#ifdef JP
			msg_print("�d�C���p�`�p�`���𗧂Ă�...");
#else
			msg_print("It crackles with electricity...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_ELEC, dir, 100, 3);
			break;
		}

		case ACT_BA_FIRE_2:
		{
#ifdef JP
			msg_format("%s���牊�������o����...", name);
#else
			msg_format("The %s rages in fire...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 120, 3);
			break;
		}

		case ACT_DRAIN_2:
		{
#ifdef JP
			msg_print("�����P���Ă���...");
#else
			msg_print("It glows black...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			drain_life(dir, 120);
			break;
		}

		case ACT_VAMPIRE_1:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if (drain_life(dir, 50))
				hp_player(50);
			}
			break;
		}

		case ACT_BO_MISS_2:
		{
#ifdef JP
			msg_print("���@�̃g�Q�����ꂽ...");
#else
			msg_print("It grows magical spikes...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ARROW, dir, 150);
			break;
		}

		case ACT_BA_FIRE_3:
		{
#ifdef JP
			msg_print("�[�ԐF�ɋP���Ă���...");
#else
			msg_print("It glows deep red...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 300, 3);
			break;
		}

		case ACT_BA_COLD_3:
		{
#ifdef JP
			msg_print("���邭���F�ɋP���Ă���...");
#else
			msg_print("It glows bright white...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 400, 3);
			break;
		}

		case ACT_BA_ELEC_3:
		{
#ifdef JP
			msg_print("�[�F�ɋP���Ă���...");
#else
			msg_print("It glows deep blue...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_ELEC, dir, 500, 3);
			break;
		}

		case ACT_WHIRLWIND:
		{
			//v1.1.33 �ʊ֐��ɕ���
			whirlwind_attack(0);
			break;
		}

		case ACT_VAMPIRE_2:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			for (dummy = 0; dummy < 3; dummy++)
			{
				if (drain_life(dir, 100))
				hp_player(100);
			}
			break;
		}


		case ACT_CALL_CHAOS:
		{
#ifdef JP
			msg_print("�l�X�ȐF�̉ΉԂ𔭂��Ă���...");
#else
			msg_print("It glows in scintillating colours...");
#endif
			call_chaos();
			break;
		}

		case ACT_ROCKET:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���P�b�g�𔭎˂����I");
#else
			msg_print("You launch a rocket!");
#endif
			fire_ball(GF_ROCKET, dir, 250 + plev*3, 2);
			break;
		}

		case ACT_DISP_EVIL:
		{
#ifdef JP
			msg_print("�_���ȕ��͋C���[������...");
#else
			msg_print("It floods the area with goodness...");
#endif
			dispel_evil(p_ptr->lev * 5);
			break;
		}

		case ACT_BA_MISS_3:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���Ȃ��̓G�������g�̃u���X��f�����B");
#else
			msg_print("You breathe the elements.");
#endif
			fire_ball(GF_MISSILE, dir, 300, -4);
			break;
		}

		case ACT_DISP_GOOD:
		{
#ifdef JP
			msg_print("�׈��ȕ��͋C���[������...");
#else
			msg_print("It floods the area with evil...");
#endif
			dispel_good(p_ptr->lev * 5);
			break;
		}

		case ACT_BO_MANA:
		{
#ifdef JP
			msg_format("%s�ɖ��@�̃g�Q�����ꂽ...", name);
#else
			msg_format("The %s grows magical spikes...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_bolt(GF_ARROW, dir, 150);
			break;
		}

		case ACT_BA_WATER:
		{
#ifdef JP
			msg_format("%s���[���F�Ɍۓ����Ă���...", name);
#else
			msg_format("The %s throbs deep blue...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_WATER, dir, 200, 3);
			break;
		}

		case ACT_BA_DARK:
		{
#ifdef JP
			msg_format("%s���[���łɕ���ꂽ...", name);
#else
			msg_format("The %s is coverd in pitch-darkness...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_DARK, dir, 250, 4);
			break;
		}

		case ACT_BA_MANA:
		{
#ifdef JP
			msg_format("%s�������������D�D�D", name);
#else
			msg_format("The %s glows pale...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_MANA, dir, 250, 4);
			break;
		}

		case ACT_PESTICIDE:
		{
#ifdef JP
			msg_print("���Ȃ��͊Q������|�����B");
#else
			msg_print("You exterminate small life.");
#endif
			(void)dispel_monsters(4);
			break;
		}

		case ACT_BLINDING_LIGHT:
		{
#ifdef JP
			msg_format("%s��ῂ������ŋP����...", name);
#else
			msg_format("The %s gleams with blinding light...", name);
#endif
			fire_ball(GF_LITE, 0, 300, 6);
			confuse_monsters(3 * p_ptr->lev / 2);
			break;
		}

		case ACT_BIZARRE:
		{
#ifdef JP
			msg_format("%s�͎����ɋP����...", name);
#else
			msg_format("The %s glows intensely black...", name);
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			ring_of_power(dir);
			break;
		}

		case ACT_CAST_BA_STAR:
		{
			int num = damroll(5, 3);
			int y, x;
			int attempts;
#ifdef JP
			msg_format("%s����Ȃŕ���ꂽ...", name);
#else
			msg_format("The %s is surrounded by lightning...", name);
#endif
			for (k = 0; k < num; k++)
			{
				attempts = 1000;

				while (attempts--)
				{
					scatter(&y, &x, py, px, 4, 0);

					if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

					if (!player_bold(y, x)) break;
				}

				project(0, 3, y, x, 150, GF_ELEC,
							(PROJECT_THRU | PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
			}

			break;
		}

		case ACT_BLADETURNER:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���Ȃ��̓G�������g�̃u���X��f�����B");
#else
			msg_print("You breathe the elements.");
#endif
			fire_ball(GF_MISSILE, dir, 300, -4);
#ifdef JP
			msg_print("�Z���l�X�ȐF�ɋP����...");
#else
			msg_print("Your armor glows many colours...");
#endif
			(void)set_afraid(0);
			(void)set_hero(randint1(50) + 50, FALSE);
			(void)hp_player(10);
			(void)set_blessed(randint1(50) + 50, FALSE);
			(void)set_oppose_acid(randint1(50) + 50, FALSE);
			(void)set_oppose_elec(randint1(50) + 50, FALSE);
			(void)set_oppose_fire(randint1(50) + 50, FALSE);
			(void)set_oppose_cold(randint1(50) + 50, FALSE);
			(void)set_oppose_pois(randint1(50) + 50, FALSE);
			break;
		}
		case ACT_BA_ACID_1:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_ACID, dir, 100, 2);
			break;
		}

		case ACT_BR_FIRE:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_FIRE, dir, 200, -2);
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
			{
				(void)set_oppose_fire(randint1(20) + 20, FALSE);
			}
			break;
		}
		case ACT_BR_COLD:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_ball(GF_COLD, dir, 200, -2);
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
			{
				(void)set_oppose_cold(randint1(20) + 20, FALSE);
			}
			break;
		}
		case ACT_BR_DRAGON:
		{
			if (!activate_dragon_breath(o_ptr)) return FALSE;
			break;
		}

		/* Activate for other offensive action */

		case ACT_CONFUSE:
		{
#ifdef JP
			msg_print("�l�X�ȐF�̉ΉԂ𔭂��Ă���...");
#else
			msg_print("It glows in scintillating colours...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			confuse_monster(dir, 20);
			break;
		}

		case ACT_SLEEP:
		{
#ifdef JP
			msg_print("�[�F�ɋP���Ă���...");
#else
			msg_print("It glows deep blue...");
#endif
			sleep_monsters_touch();
			break;
		}

		case ACT_QUAKE:
		{
			earthquake(py, px, 5);
			break;
		}

		case ACT_TERROR:
		{
			turn_monsters(40 + p_ptr->lev);
			break;
		}

		case ACT_TELE_AWAY:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)fire_beam(GF_AWAY_ALL, dir, plev);
			break;
		}

		case ACT_BANISH_EVIL:
		{
			if (banish_evil(100))
			{
#ifdef JP
				msg_print("�A�[�e�B�t�@�N�g�̗͂��׈���ł��������I");
#else
				msg_print("The power of the artifact banishes evil!");
#endif
			}
			break;
		}

		case ACT_GENOCIDE:
		{
#ifdef JP
			msg_print("�[�F�ɋP���Ă���...");
#else
			msg_print("It glows deep blue...");
#endif
			(void)symbol_genocide(200, TRUE,0);
			break;
		}

		case ACT_MASS_GENO:
		{
#ifdef JP
			msg_print("�Ђǂ��s����������o��...");
#else
			msg_print("It lets out a long, shrill note...");
#endif
			(void)mass_genocide(200, TRUE);
			break;
		}

		case ACT_SCARE_AREA:
		{
			if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
			if (hex_spelling_any()) stop_hex_spell_all();
#ifdef JP
			msg_print("���Ȃ��͗͋����˕��𐁂��炵���B���͂̓G���k������Ă���!");
#else
			msg_print("You wind a mighty blast; your enemies tremble!");
#endif
			(void)turn_monsters((3 * p_ptr->lev / 2) + 10);
			break;
		}

		case ACT_AGGRAVATE:
		{
			if (o_ptr->name1 == ART_HYOUSIGI)
			{
#ifdef JP
				msg_print("���q�؂�ł����B");
#else
				msg_print("You beat Your wooden clappers.");
#endif
			}
			else
			{
#ifdef JP
				msg_format("%s�͕s���ȕ����𗧂Ă��B", name);
#else
				msg_format("The %s sounds an unpleasant noise.", name);
#endif
			}
			aggravate_monsters(0,FALSE);
			break;
		}

		/* Activate for summoning / charming */

		case ACT_CHARM_ANIMAL:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)charm_animal(dir, plev * 2);
			break;
		}

		case ACT_CHARM_UNDEAD:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)control_one_undead(dir, plev * 2);
			break;
		}

		case ACT_CHARM_OTHER:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			(void)charm_monster(dir, plev * 2);
			break;
		}

		case ACT_CHARM_ANIMALS:
		{
			(void)charm_animals(plev * 2);
			break;
		}

		case ACT_CHARM_OTHERS:
		{
			charm_monsters(plev * 2);
			break;
		}

		case ACT_SUMMON_ANIMAL:
		{
			(void)summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET));
			break;
		}

		case ACT_SUMMON_PHANTOM:
		{
#ifdef JP
			msg_print("��������������B");
#else
			msg_print("You summon a phantasmal servant.");
#endif
			(void)summon_specific(-1, py, px, dun_level, SUMMON_PHANTOM, (PM_ALLOW_GROUP | PM_FORCE_PET));
			break;
		}

		case ACT_SUMMON_ELEMENTAL:
		{
			bool pet = one_in_(3);
			u32b mode = 0L;

			if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;
			if (pet) mode |= PM_FORCE_PET;
			else mode |= PM_NO_PET;

			if (summon_specific((pet ? -1 : 0), py, px, ((plev * 3) / 2), SUMMON_ELEMENTAL, mode))
			{
#ifdef JP
				msg_print("�G�������^�������ꂽ...");
#else
				msg_print("An elemental materializes...");
#endif
				if (pet)
#ifdef JP
					msg_print("���Ȃ��ɕ��]���Ă���悤���B");
#else
					msg_print("It seems obedient to you.");
#endif
				else
#ifdef JP
					msg_print("������R���g���[���ł��Ȃ������I");
#else
					msg_print("You fail to control it!");
#endif
			}

			break;
		}

		case ACT_SUMMON_DEMON:
		{
			bool pet = one_in_(3);
			u32b mode = 0L;

			if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;
			if (pet) mode |= PM_FORCE_PET;
			else mode |= PM_NO_PET;

			if (summon_specific((pet ? -1 : 0), py, px, ((plev * 3) / 2), SUMMON_DEMON, mode))
			{
#ifdef JP
				msg_print("�����̈��L���[�������B");
#else
				msg_print("The area fills with a stench of sulphur and brimstone.");
#endif
				if (pet)
#ifdef JP
					msg_print("�u���p�ł������܂����A����l�l�v");
#else
					msg_print("'What is thy bidding... Master?'");
#endif
				else
#ifdef JP
					msg_print("�uNON SERVIAM! Wretch! ���O�̍��𒸂����I�v");
#else
					msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
			}

			break;
		}

		case ACT_SUMMON_UNDEAD:
		{
			bool pet = one_in_(3);
			int type;
			u32b mode = 0L;

			type = (plev > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);

			if (!pet || ((plev > 24) && one_in_(3))) mode |= PM_ALLOW_GROUP;
			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px, ((plev * 3) / 2), type, mode))
			{
#ifdef JP
				msg_print("�₽���������Ȃ��̎���ɐ����n�߂��B����͕��s�L���^��ł���...");
#else
				msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif
				if (pet)
#ifdef JP
					msg_print("�Â��̎�����ҋ������Ȃ��Ɏd���邽�ߓy�����S�����I");
#else
					msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
				else
#ifdef JP
					msg_print("���҂��S�����B�����W���邠�Ȃ��𔱂��邽�߂ɁI");
#else
					msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
			}

			break;
		}

		case ACT_SUMMON_HOUND:
		{
			u32b mode = PM_ALLOW_GROUP;
			bool pet = !one_in_(5);
			if (pet) mode |= PM_FORCE_PET;
			else mode |= PM_NO_PET;

			if (summon_specific((pet ? -1 : 0), py, px, ((p_ptr->lev * 3) / 2), SUMMON_HOUND, mode))
			{

				if (pet)
#ifdef JP
					msg_print("�n�E���h�����Ȃ��̉��l�Ƃ��ďo�������B");
#else
				msg_print("A group of hounds appear as your servant.");
#endif

				else
#ifdef JP
					msg_print("�n�E���h�͂��Ȃ��ɉ�������Ă���I");
#else
					msg_print("A group of hounds appear as your enemy!");
#endif

			}

			break;
		}

		case ACT_SUMMON_DAWN:
		{
#ifdef JP
			msg_print("�ł̎t�c�����������B");
#else
			msg_print("You summon the Legion of the Dawn.");
#endif
			(void)summon_specific(-1, py, px, dun_level, SUMMON_DAWN, (PM_ALLOW_GROUP | PM_FORCE_PET));
			break;
		}

		case ACT_SUMMON_OCTOPUS:
		{
			u32b mode = PM_ALLOW_GROUP;
			bool pet = !one_in_(5);
			if (pet) mode |= PM_FORCE_PET;

			if (summon_named_creature(0, py, px, MON_JIZOTAKO, mode))
			{
				if (pet)
#ifdef JP
					msg_print("�������Ȃ��̉��l�Ƃ��ďo�������B");
#else
					msg_print("A group of octopuses appear as your servant.");
#endif
				else
#ifdef JP
					msg_print("���͂��Ȃ����ɂ�ł���I");
#else
					msg_print("A group of octopuses appear as your enemy!");
#endif
			}

			break;
		}

		/* Activate for healing */

		case ACT_CHOIR_SINGS:
		{
#ifdef JP
			msg_print("�V���̉̂���������...");
#else
			msg_print("A heavenly choir sings...");
#endif
			(void)set_poisoned(0);
			(void)set_cut(0);
			(void)set_stun(0);
			(void)set_confused(0);
			(void)set_blind(0);
			(void)set_afraid(0);
			(void)set_hero(randint1(25) + 25, FALSE);
			(void)hp_player(777);
			break;
		}

		case ACT_CURE_LW:
		{
			(void)set_afraid(0);
			(void)hp_player(30);
			break;
		}

		case ACT_CURE_MW:
		{
#ifdef JP
			msg_print("�[���F�̌��𔭂��Ă���...");
#else
			msg_print("It radiates deep purple...");
#endif
			hp_player(damroll(4, 8));
			(void)set_cut((p_ptr->cut / 2) - 50);
			break;
		}

		case ACT_CURE_POISON:
		{
#ifdef JP
			msg_print("�[�F�ɋP���Ă���...");
#else
			msg_print("It glows deep blue...");
#endif
			(void)set_afraid(0);
			(void)set_poisoned(0);
			break;
		}

		case ACT_REST_LIFE:
		{
#ifdef JP
			msg_print("�[�g�ɋP���Ă���...");
#else
			msg_print("It glows a deep red...");
#endif
			restore_level();
			break;
		}

		case ACT_REST_ALL:
		{
#ifdef JP
			msg_print("�Z�ΐF�ɋP���Ă���...");
#else
			msg_print("It glows a deep green...");
#endif
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_CHR);
			(void)restore_level();
			break;
		}

		case ACT_CURE_700:
		{
#ifdef JP
			msg_print("�[�F�ɋP���Ă���...");
			msg_print("�̓��ɒg�����ۓ�����������...");
#else
			msg_print("It glows deep blue...");
			msg_print("You feel a warm tingling inside...");
#endif
			(void)hp_player(700);
			(void)set_cut(0);
			break;
		}
		//v1.1.74
		case ACT_CURE_500:
		{
			msg_print("�F�ɋP���Ă���...");
			(void)hp_player(500);
			(void)set_cut(0);
			break;
		}
		case ACT_CURE_1000:
		{
#ifdef JP
			msg_print("�������邭�P���Ă���...");
			msg_print("�Ђ��傤�ɋC�����悢...");
#else
			msg_print("It glows a bright white...");
			msg_print("You feel much better...");
#endif
			(void)hp_player(1000);
			(void)set_cut(0);
			break;
		}

		case ACT_CURING:
		{
#ifdef JP
			msg_format("%s�̗D�����ɖ������...", name);
#else
			msg_format("the %s cures you affectionately ...", name);
#endif
			(void)set_poisoned(0);
			(void)set_confused(0);
			(void)set_blind(0);
			(void)set_stun(0);
			(void)set_cut(0);
			(void)set_image(0);

			break;
		}

		case ACT_CURE_MANA_FULL:
		{
#ifdef JP
			msg_format("%s�������������D�D�D", name);
#else
			msg_format("The %s glows pale...", name);
#endif
			///class ������p�m��MP��
			if (p_ptr->pclass == CLASS_MAGIC_EATER)
			{
				int i;
				for (i = 0; i < EATER_EXT*2; i++)
				{
					p_ptr->magic_num1[i] += (p_ptr->magic_num2[i] < 10) ? EATER_CHARGE * 3 : p_ptr->magic_num2[i]*EATER_CHARGE/3;
					if (p_ptr->magic_num1[i] > p_ptr->magic_num2[i]*EATER_CHARGE) p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
				}
				for (; i < EATER_EXT*3; i++)
				{
					int k_idx = lookup_kind(TV_ROD, i-EATER_EXT*2);
					p_ptr->magic_num1[i] -= ((p_ptr->magic_num2[i] < 10) ? EATER_ROD_CHARGE*3 : p_ptr->magic_num2[i]*EATER_ROD_CHARGE/3)*k_info[k_idx].pval;
					if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;
				}
#ifdef JP
				msg_print("�����n�b�L���Ƃ����B");
#else
				msg_print("You feel your head clear.");
#endif
				p_ptr->window |= (PW_PLAYER);
			}
			else if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
#ifdef JP
				msg_print("�����n�b�L���Ƃ����B");
#else
				msg_print("You feel your head clear.");
#endif
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);
			}
			break;
		}

		/* Activate for timed effect */

		case ACT_ESP:
		{
			(void)set_tim_esp(randint1(30) + 25, FALSE);
			break;
		}

		case ACT_BERSERK:
		{
			if(o_ptr->name2 == EGO_HEAD_BEAST)
				msg_format("���I�[�[�[�[�[���I", name);

			(void)set_afraid(0);
			(void)set_shero(randint1(25) + 25, FALSE);
			/* (void)set_afraid(0);
			(void)set_hero(randint1(50) + 50, FALSE);
			(void)set_blessed(randint1(50) + 50, FALSE);
			o_ptr->timeout = 100 + randint1(100); */
			break;
		}

		case ACT_PROT_EVIL:
		{
#ifdef JP
			msg_format("%s����s����������o��...", name);
#else
			msg_format("The %s lets out a shrill wail...", name);
#endif
			k = 3 * p_ptr->lev;
			(void)set_protevil(randint1(25) + k, FALSE);
			break;
		}

		case ACT_RESIST_ALL:
		{
#ifdef JP

			if (o_ptr->name1 == ART_IKKYU)
				msg_print("�S�Ă��󂯓���鋫�n�Ɏ������C������...");
			else
				msg_print("�l�X�ȐF�ɋP���Ă���...");

#else
			msg_print("It glows many colours...");
#endif
			(void)set_oppose_acid(randint1(40) + 40, FALSE);
			(void)set_oppose_elec(randint1(40) + 40, FALSE);
			(void)set_oppose_fire(randint1(40) + 40, FALSE);
			(void)set_oppose_cold(randint1(40) + 40, FALSE);
			(void)set_oppose_pois(randint1(40) + 40, FALSE);
			break;
		}

		case ACT_SPEED:
		{
#ifdef JP
			msg_print("���邭�ΐF�ɋP���Ă���...");
#else
			msg_print("It glows bright green...");
#endif
			(void)set_fast(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_XTRA_SPEED:
		{
#ifdef JP
			msg_print("���邭�P���Ă���...");
#else
			msg_print("It glows brightly...");
#endif
			(void)set_fast(randint1(75) + 75, FALSE);
			break;
		}

		case ACT_WRAITH:
		{
			set_wraith_form(randint1(plev / 2) + (plev / 2), FALSE);
			break;
		}

		case ACT_INVULN:
		{
			(void)set_invuln(randint1(8) + 8, FALSE);
			break;
		}

		case ACT_HELO:
		{
			(void)set_afraid(0);
			set_hero(randint1(25)+25, FALSE);
			hp_player(10);
			break;
		}

		case ACT_HELO_SPEED:
		{
			(void)set_fast(randint1(50) + 50, FALSE);
			hp_player(10);
			set_afraid(0);
			set_hero(randint1(50) + 50, FALSE);
			break;
		}
		///item �Ή��̎w�ւȂǂ̓��ꔭ��
		case ACT_RESIST_ACID:
		{
#ifdef JP
			msg_format("%s�������P����...", name);
#else
			msg_format("The %s grows black.", name);
#endif
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ACID))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_ACID, dir, 100, 2);
			}
			(void)set_oppose_acid(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_FIRE:
		{
			//v1.1.31 ���J�ǉ�
			if(o_ptr->name1 == ART_MURASAME)
				msg_format("%s���琅�������o�����I", name);
			else
				msg_format("%s���Ԃ��P����...", name);

			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_FIRE, dir, 100, 2);
			}

			(void)set_oppose_fire(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_COLD:
		{
#ifdef JP
			msg_format("%s�������P����...", name);
#else
			msg_format("The %s grows white.", name);
#endif
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_COLD, dir, 100, 2);
			}
			(void)set_oppose_cold(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_ELEC:
		{
#ifdef JP
			msg_format("%s�����P����...", name);
#else
			msg_format("The %s grows blue.", name);
#endif
			if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ELEC))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball(GF_ELEC, dir, 100, 2);
			}
			(void)set_oppose_elec(randint1(20) + 20, FALSE);
			break;
		}

		case ACT_RESIST_POIS:
		{
#ifdef JP
			msg_format("%s���΂ɋP����...", name);
#else
			msg_format("The %s grows green.", name);
#endif
			(void)set_oppose_pois(randint1(20) + 20, FALSE);
			break;
		}

		/* Activate for general purpose effect (detection etc.) */

		case ACT_LIGHT:
		{
#ifdef JP
			msg_format("%s���琟�񂾌������ӂ�o��...", name);
#else
			msg_format("The %s wells with clear light...", name);
#endif
			lite_area(damroll(2, 15), 3);
			break;
		}

		case ACT_MAP_LIGHT:
		{
#ifdef JP
			msg_print("ῂ����P����...");
#else
			msg_print("It shines brightly...");
#endif
			map_area(DETECT_RAD_MAP);
			lite_area(damroll(2, 15), 3);
			break;
		}

		case ACT_DETECT_ALL:
		{
#ifdef JP
			msg_print("�������邭�P���Ă���...");
			msg_print("�S�ɃC���[�W��������ł���...");
#else
			msg_print("It glows bright white...");
			msg_print("An image forms in your mind...");
#endif
			detect_all(DETECT_RAD_DEFAULT);
			break;
		}

		case ACT_DETECT_XTRA:
		{
#ifdef JP
			msg_print("���邭�P���Ă���...");
#else
			msg_print("It glows brightly...");
#endif
			detect_all(DETECT_RAD_DEFAULT);
			probing();
			identify_fully(FALSE);
			break;
		}

		case ACT_ID_FULL:
		{
#ifdef JP
			msg_print("���F���P���Ă���...");
#else
			msg_print("It glows yellow...");
#endif
			identify_fully(FALSE);
			break;
		}

		case ACT_ID_PLAIN:
		{
			if (!ident_spell(FALSE)) return FALSE;
			break;
		}

		case ACT_RUNE_EXPLO:
		{
			if(p_ptr->csp < 20)
			{
				msg_format("㩂��d�|����͂��c���Ă��Ȃ��E�E");
				break;
			}
			msg_format("���Ȃ��̑����Ƀ��[�����P�����B");
			explosive_rune();
			p_ptr->csp -= 20;
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();
			break;
		}

		case ACT_RUNE_PROT:
		{
#ifdef JP
			msg_print("�u���[�ɖ��邭�P���Ă���...");
#else
			msg_print("It glows light blue...");
#endif
			warding_glyph();
			break;
		}

		case ACT_SATIATE:
		{
			(void)set_food(PY_FOOD_MAX - 1);
			break;
		}

		case ACT_DEST_DOOR:
		{
#ifdef JP
			msg_print("���邢�ԐF�ɋP���Ă���...");
#else
			msg_print("It glows bright red...");
#endif
			destroy_doors_touch();
			break;
		}

		case ACT_STONE_MUD:
		{
#ifdef JP
			msg_print("�ۓ����Ă���...");
#else
			msg_print("It pulsates...");
#endif
			if (!get_aim_dir(&dir)) return FALSE;
			wall_to_mud(dir, 20 + randint1(30));
			break;
		}

		case ACT_RECHARGE:
		{
			recharge(130);
			break;
		}

		case ACT_ALCHEMY:
		{
#ifdef JP
			msg_print("���邢���F�ɋP���Ă���...");
#else
			msg_print("It glows bright yellow...");
#endif
			(void)alchemy(0);
			break;
		}

		case ACT_DIM_DOOR:
		{
#ifdef JP
			msg_print("�����̔����J�����B�ړI�n��I��ŉ������B");
#else
			msg_print("You open a dimensional gate. Choose a destination.");
#endif
			if (!dimension_door(D_DOOR_NORMAL)) return FALSE;
			break;
		}


		case ACT_TELEPORT:
		{
#ifdef JP
			msg_print("����̋�Ԃ��c��ł���...");
#else
			msg_print("It twists space around you...");
#endif
			teleport_player(100, 0L);
			break;
		}

		case ACT_RECALL:
		{
#ifdef JP
			msg_print("���炩�Ȕ��F�ɋP���Ă���...");
#else
			msg_print("It glows soft white...");
#endif
			if (!word_of_recall()) return FALSE;
			break;
		}

		case ACT_JUDGE:
		{
#ifdef JP
			msg_format("%s�͐Ԃ����邭�������I", name);
#else
			msg_format("The %s flashes bright red!", name);
#endif
		///del131214 virtue		
			//chg_virtue(V_KNOWLEDGE, 1);
			//chg_virtue(V_ENLIGHTEN, 1);
			wiz_lite(FALSE);
#ifdef JP
			msg_format("%s�͂��Ȃ��̗̑͂�D����...", name);
			take_hit(DAMAGE_LOSELIFE, damroll(3,8), "�R���̕��", -1);
#else
			msg_format("The %s drains your vitality...", name);
			take_hit(DAMAGE_LOSELIFE, damroll(3, 8), "the Jewel of Judgement", -1);
#endif
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
#ifdef JP
			if (get_check("�A�҂̗͂��g���܂����H"))
#else
			if (get_check("Activate recall? "))
#endif
			{
				(void)word_of_recall();
			}

			break;
		}

		case ACT_TELEKINESIS:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_format("%s��L�΂����B", name);
#else
			msg_format("You stretched your %s.", name);
#endif
			fetch(dir, 500, TRUE);
			break;
		}

		case ACT_DETECT_UNIQUE:
		{
			int i;
			monster_type *m_ptr;
			monster_race *r_ptr;
#ifdef JP
			msg_print("��ȏꏊ�����̒��ɕ����񂾁D�D�D");
#else
			msg_print("Some strange places show up in your mind. And you see ...");
#endif
			/* Process the monsters (backwards) */
			for (i = m_max - 1; i >= 1; i--)
			{
				/* Access the monster */
				m_ptr = &m_list[i];

				/* Ignore "dead" monsters */
				if (!m_ptr->r_idx) continue;

				r_ptr = &r_info[m_ptr->r_idx];

				if(r_ptr->flags1 & RF1_UNIQUE)
				{
#ifdef JP
					msg_format("%s�D ",r_name + r_ptr->name);
#else
					msg_format("%s. ",r_name + r_ptr->name);
#endif
				}
			}
			break;
		}

		case ACT_ESCAPE:
		{
			switch (randint1(13))
			{
			case 1: case 2: case 3: case 4: case 5:
				teleport_player(10, 0L);
				break;
			case 6: case 7: case 8: case 9: case 10:
				teleport_player(222, 0L);
				break;
			case 11: case 12:
				(void)stair_creation();
				break;
			default:
#ifdef JP
				if (get_check("���̊K������܂����H"))
#else
				if (get_check("Leave this level? "))
#endif
				{
					if (autosave_l) do_cmd_save_game(TRUE);

					//�����t���A�č\���B�N�G�X�g�_���W�����܂ōč\�������Ȃǂ��Ȃ��ȋ���������B
					/* Leaving */
					p_ptr->leaving = TRUE;
				}
			}
			break;
		}

		case ACT_DISP_CURSE_XTRA:
		{
#ifdef JP
			msg_format("%s���^�����Ƃ炵�o��...", name);
#else
			msg_format("The %s exhibits the truth...", name);
#endif
			if (remove_all_curse())
			{
#ifdef JP
				msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
				msg_print("You feel as if someone is watching over you.");
#endif
			}
			(void)probing();
			break;
		}

		case ACT_BRAND_FIRE_BOLTS:
		{
#ifdef JP
			msg_format("%s���[�g�ɋP����...", name);
#else
			msg_format("Your %s glows deep red...", name);
#endif
			(void)brand_bolts();
			break;
		}

		case ACT_RECHARGE_XTRA:
		{
#ifdef JP
			msg_format("%s�������P�����D�D�D", name);
#else
			msg_format("The %s gleams with blinding light...", name);
#endif
			if (!recharge(1000)) return FALSE;
			break;
		}

		case ACT_LORE:
		{
#ifdef JP
			msg_print("�΂��B���ꂽ�閧���ʂ��o�����D�D�D");
#else
			msg_print("The stone reveals hidden mysteries...");
#endif
			if (!ident_spell(FALSE)) return FALSE;

			//if (mp_ptr->spell_book)
			//if(p_ptr->msp > 20)
			{
				/* Sufficient mana */
				if (20 <= p_ptr->csp)
				{
					/* Use some mana */
					p_ptr->csp -= 20;
				}

				/* Over-exert the player */
				else
				{
					int oops = 20 - p_ptr->csp;

					/* No mana left */
					p_ptr->csp = 0;
					p_ptr->csp_frac = 0;

					//v1.1.56 ��ჂłȂ�sanity_blast�ōς܂��邱�Ƃɂ���
					if (randint1(100) < oops)
					{
						msg_print("�΂𐧌�ł��Ȃ��I");
						sanity_blast(0, TRUE);
					}
				}

				/* Redraw mana */
				p_ptr->redraw |= (PR_MANA);
			}
#ifdef JP
			take_hit(DAMAGE_LOSELIFE, damroll(1, 12), "�댯�Ȕ閧", -1);
#else
			take_hit(DAMAGE_LOSELIFE, damroll(1, 12), "perilous secrets", -1);
#endif
			/* Confusing. */
			if (one_in_(5)) (void)set_confused(p_ptr->confused +
				randint1(10));

			/* Exercise a little care... */
			if (one_in_(20))
#ifdef JP
				take_hit(DAMAGE_LOSELIFE, damroll(4, 10), "�댯�Ȕ閧", -1);
#else
				take_hit(DAMAGE_LOSELIFE, damroll(4, 10), "perilous secrets", -1);
#endif
			break;
		}

///del131223
#if 0
		case ACT_SHIKOFUMI:
		{
#ifdef JP
			msg_print("�͋����l�҂𓥂񂾁B");
#else
			msg_print("You stamp. (as if you are in a ring.)");
#endif
			(void)set_afraid(0);
			(void)set_hero(randint1(20) + 20, FALSE);
			dispel_evil(p_ptr->lev * 3);
			break;
		}
#endif
		case ACT_PHASE_DOOR:
		{
			teleport_player(10, 0L);
			break;
		}

		case ACT_DETECT_ALL_MONS:
		{
			(void)detect_monsters_invis(255);
			(void)detect_monsters_normal(255);
			break;
		}

		case ACT_ULTIMATE_RESIST:
		{
			int v = randint1(25)+25;
			(void)set_afraid(0);
			(void)set_hero(v, FALSE);
			(void)hp_player(10);
			(void)set_blessed(v, FALSE);
			(void)set_oppose_acid(v, FALSE);
			(void)set_oppose_elec(v, FALSE);
			(void)set_oppose_fire(v, FALSE);
			(void)set_oppose_cold(v, FALSE);
			(void)set_oppose_pois(v, FALSE);
			(void)set_ultimate_res(v, FALSE);
			break;
		}
///del131214 �u�����Y�N���X����
#if 0
		/* Unique activation */
		///class ���C�p
		case ACT_CAST_OFF:
		{
			int inv, o_idx, t;
			char o_name[MAX_NLEN];
			object_type forge;

			/* Cast off activated item */
			for (inv = INVEN_RARM; inv <= INVEN_FEET; inv++)
			{
				if (o_ptr == &inventory[inv]) break;
			}

			/* Paranoia */
			if (inv > INVEN_FEET) return FALSE;

			object_copy(&forge, o_ptr);
			inven_item_increase(inv, (0 - o_ptr->number));
			inven_item_optimize(inv);
			o_idx = drop_near(&forge, 0, py, px);
			o_ptr = &o_list[o_idx];

			object_desc(o_name, o_ptr, OD_NAME_ONLY);
			msg_format(_("%s��E���̂Ă��B", "You cast off %s."), o_name);

			/* Get effects */
			msg_print(_("�u�R���オ�ꉴ�̏��F���I�v", "You say, 'Burn up my cosmo!"));
			t = 20 + randint1(20);
			(void)set_blind(p_ptr->blind + t);
			(void)set_afraid(0);
			(void)set_tim_esp(p_ptr->tim_esp + t, FALSE);
			(void)set_tim_regen(p_ptr->tim_regen + t, FALSE);
			(void)set_hero(p_ptr->hero + t, FALSE);
			(void)set_blessed(p_ptr->blessed + t, FALSE);
			(void)set_fast(p_ptr->fast + t, FALSE);
			(void)set_shero(p_ptr->shero + t, FALSE);
			if (p_ptr->pclass == CLASS_FORCETRAINER)
			{
				p_ptr->magic_num1[0] = plev * 5 + 190;
				msg_print(_("�C���������O�ɂȂ����B", "Your force are immediatly before explosion."));
			}

			break;
		}
#endif
		case ACT_FISHING:
		{
			int x, y;

			if (!get_rep_dir2(&dir)) return FALSE;
			y = py+ddy[dir];
			x = px+ddx[dir];
			tsuri_dir = dir;
			if (!cave_have_flag_bold(y, x, FF_WATER))
			{
#ifdef JP
				msg_print("�����͐��ӂł͂Ȃ��B");
#else
				msg_print("There is no fishing place.");
#endif
				return FALSE;
			}
			else if (cave[y][x].m_idx)
			{
				char m_name[80];
				monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
				msg_format("%s���ז����I", m_name);
#else
				msg_format("%^s is stand in your way.", m_name);
#endif
				energy_use = 0;
				return FALSE;
			}
			set_action(ACTION_FISH);
			p_ptr->redraw |= (PR_STATE);
			break;
		}

///del131224 ���Ĕ���
#if 0
		case ACT_INROU:
		{
			int count = 0, i;
			monster_type *m_ptr;
#ifndef JP
			cptr kakusan = "";
#endif
			if (summon_named_creature(0, py, px, MON_SUKE, PM_FORCE_PET))
			{
#ifdef JP
				msg_print("�w������x�����ꂽ�B");
#else
				msg_print("Suke-san apperars.");
				kakusan = "Suke-san";
#endif
				count++;
			}
			if (summon_named_creature(0, py, px, MON_KAKU, PM_FORCE_PET))
			{
#ifdef JP
				msg_print("�w�i����x�����ꂽ�B");
#else
				msg_print("Kaku-san appears.");
				kakusan = "Kaku-san";
#endif
				count++;
			}
			if (!count)
			{
				for (i = m_max - 1; i > 0; i--)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					if (!((m_ptr->r_idx == MON_SUKE) || (m_ptr->r_idx == MON_KAKU))) continue;
					if (!los(m_ptr->fy, m_ptr->fx, py, px)) continue;
					if (!projectable(m_ptr->fy, m_ptr->fx, py, px)) continue;
					count++;
					break;
				}
			}

			if (count)
			{
#ifdef JP
				msg_print("�u�҂ǂ��A�Ђ������낤�I�I�I���̂������ǂȂ��Ƃ����낦��B�v");
#else
				msg_format("%^s says 'WHO do you think this person is! Bow your head, down your knees!'", kakusan);
#endif
				sukekaku = TRUE;
				stun_monsters(120);
				confuse_monsters(120);
				turn_monsters(120);
				stasis_monsters(120);
				sukekaku = FALSE;
			}
			else
			{
#ifdef JP
				msg_print("�������A�����N���Ȃ������B");
#else
				msg_print("Nothing happen.");
#endif
			}
			break;
		}
#endif
		///item ���������@�����Ƃق��̌��ʂɕς��悤���H
		case ACT_MURAMASA:
		{
			/* Only for Muramasa */
			if (o_ptr->name1 != ART_MURAMASA) return FALSE;
#ifdef JP
			if (get_check("�{���Ɏg���܂����H"))
#else
			if (get_check("Are you sure?!"))
#endif
			{
#ifdef JP
				msg_print("�������k�����D�D�D");
#else
				msg_print("The Muramasa pulsates...");
#endif
				do_inc_stat(A_STR);
				if (one_in_(2))
				{
#ifdef JP
					msg_print("�����͉�ꂽ�I");
#else
					msg_print("The Muramasa is destroyed!");
#endif
					curse_weapon_object(TRUE, o_ptr);
				}
			}
			break;
		}

		case ACT_BLOODY_MOON:
		{
			/* Only for Bloody Moon */
			if (o_ptr->name1 != ART_BLOOD) return FALSE;
#ifdef JP
			msg_print("�������邭�P����...");
#else
			msg_print("Your scythe glows brightly!");
#endif
			get_bloody_moon_flags(o_ptr);
			if (p_ptr->prace == RACE_ANDROID) calc_android_exp();
			p_ptr->update |= (PU_BONUS | PU_HP);
			break;
		}
		case ACT_CRIMSON:
		{
			int num = 1;
			int i;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int tx, ty;

			/* Only for Crimson */
			if (o_ptr->name1 != ART_CRIMSON) return FALSE;

#ifdef JP
				msg_print("��������������w�N�����]���x���Ԃ��ςȂ����I");
#else
			msg_print("I'll fire CRIMSON! SEKKAKUDAKARA!");
#endif

			if (!get_aim_dir(&dir)) return FALSE;

			/* Use the given direction */
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];

			/* Hack -- Use an actual "target" */
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			///class �A�[�`���[�̃N�����]������
			if (p_ptr->pclass == CLASS_ARCHER || p_ptr->pclass == CLASS_KOGASA)
			{
				/* Extra shot at level 10 */
				if (p_ptr->lev >= 10) num++;

				/* Extra shot at level 30 */
				if (p_ptr->lev >= 30) num++;

				/* Extra shot at level 45 */
				if (p_ptr->lev >= 45) num++;
			}

			for (i = 0; i < num; i++)
				project(0, p_ptr->lev/20+1, ty, tx, p_ptr->lev*p_ptr->lev*6/50, GF_ROCKET, flg, -1);
			break;
		}
		case ACT_NERUNERUNERUNE:
			msg_print(NULL);
			msg_print("����Η���قǁc�F���ς���āc������!!!");
			msg_print(NULL);
			if(p_ptr->pclass == CLASS_KOGASA || p_ptr->pclass == CLASS_DOREMY || p_ptr->pclass == CLASS_HINA)
				msg_print("�c���������Ȃ��̕��͖c��Ȃ������B");
			else 
				(void)set_food(PY_FOOD_MAX - 1);
			if (!p_ptr->resist_chaos && one_in_(3))	(void)set_image(p_ptr->image + randint0(50) + 50);
			break;

		case ACT_ALTER_REALITY:
			msg_print("�����v���ʐ��E�̌i�F���f���o���c");
			alter_reality();
			break;


		case ACT_KOGASA_LASER:
		{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�P���烌�[�U�[�������ꂽ�I");
#endif
			fire_beam(GF_LITE, dir, p_ptr->lev + damroll(p_ptr->lev / 3, 10));
			break;
		}
		/*:::���P���g���K�[�n�b�s�[�̂Ƃ��̂ݔ����@�A�[�`���[�N�����]���Ɠ����d�l*/
		case ACT_KOGASA_ROCKET:
		{
			int num = 1;
			int i;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int tx, ty;

			if (o_ptr->tval != TV_STICK || o_ptr->sval != SV_WEAPON_KOGASA) return FALSE;
			if (!get_aim_dir(&dir)) return FALSE;

			/* Use the given direction */
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];

			/* Hack -- Use an actual "target" */
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}

			if (p_ptr->lev >= 10) num++;
			if (p_ptr->lev >= 30) num++;
			if (p_ptr->lev >= 45) num++;
			msg_format("�P����%d���̃��P�b�g�e�������ꂽ�I",num);

			for (i = 0; i < num; i++)
				project(0, p_ptr->lev/20+1, ty, tx, p_ptr->lev*p_ptr->lev*6/50, GF_ROCKET, flg, -1);
			break;
		}

		case ACT_HYPER_BERSERK:
		{
			int dam = randint1(200);
			if (dam <= p_ptr->csp)
			{
			p_ptr->csp -= dam;
			}
			else
			{
				msg_format("���Ȃ��̑̂͋���m���̔����ɔߖ��グ���I", name);
				dam -= p_ptr->csp;
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				take_hit(DAMAGE_LOSELIFE,dam,"����m���̔���",-1);
			}
			msg_format("�����b�̂悤�Ȍ�����Ȃ��̈ӎ���h��ׂ����E�E", name);
			(void)set_afraid(0);
			(void)set_shero(randint1(125) + 125, FALSE);
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();
			break;
		}


		case ACT_DRINK:
		{
			//v1.1.49 �����Y��Ă��̂Œǉ�
			if (p_ptr->pclass == CLASS_BYAKUREN)
			{
				msg_print("���Ȃ��͉����ɂ��������߂Ȃ��B");
				break;
			}


			///mod151108 �x���ǉ�
			if(!(p_ptr->muta2 & MUT2_ALCOHOL) 
				&& (p_ptr->alcohol + calc_alcohol_mod(6000,TRUE)) >= DRANK_4 
				&& !process_warning2() ) return FALSE;

			msg_format("���Ȃ��͕Z�\����������񂾁B");
			set_alcohol(p_ptr->alcohol + calc_alcohol_mod(6000,FALSE)); 
			set_food(p_ptr->food + 200);
			break;
		}
		case ACT_ROBERTA:
		{
			msg_format("�u�ł͊F�l�A��@���悤�B�v");

			project_hack2(GF_SHARDS,0,0,200);
			teleport_player(50,TELEPORT_NONMAGICAL);
			break;
		}
		case ACT_WALTER:
		{
			project_hack2(GF_MISSILE,0,0,200);
			msg_format("�u���h!!�v");
			break;
		}
		case ACT_DETECT_ITEM:
		{
			msg_format("���Ȃ��̓_�E�W���O���n�߂��E�E");
			detect_objects_normal(DETECT_RAD_DEFAULT);
			break;
		}

		case ACT_SANAGI:
		{
			msg_format("���Ȃ��͓S�����y�����ɃK���K���Ɩ炵���I");
			(void)set_afraid(0);
			(void)set_hero(randint1(25) + 25, FALSE);
			aggravate_monsters(0,FALSE);
			break;
		}
		case ACT_DER_FREISCHUTZ:
		{
			//�ʃ��[�`����ʂ邱�Ƃɂ����̂ł����ɂ͗��Ȃ��͂�
			msg_format("ERROR:���e�̎ˎ肪activate_random_artifact�o�R�ŌĂ΂ꂽ");
			return FALSE;

		}
		break;

		case ACT_GOKASHITIKIN:
		{
			if(p_ptr->csp < 40)
			{
				msg_format("���������͂��c���Ă��Ȃ��E�E");
				break;
			}
			msg_format("���Ȃ��͐��傫���U��A�Ռ��g��������I");
			project_hack2(GF_SOUND,0,0,150);
			p_ptr->csp -= 40;
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();

			break;
		}
		case ACT_SUMMON_FOUNTAIN:
		{
			msg_format("�n�ʂɏ�����ƁA�򂪗N���o�����I");
			project(0,2,py,px,3,GF_WATER_FLOW,PROJECT_GRID,-1);
			break;
		}
		case ACT_TORNADO:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			if(!fire_ball_jump(GF_TORNADO, dir, 200, 3,"�������ĂыN�������I")) return FALSE;

			break;
		}
		case ACT_TRAPEZOHEDRON:
		{
			wiz_lite(TRUE);

			if(one_in_(6))
			{
				u32b mode = PM_ALLOW_UNIQUE;
				msg_print("�������ׂɘȂ�ł���B");
				if(!summon_named_creature(0,py,px,MON_NYARLATHOTEP,mode))
				{
					int num = randint1(3);
					bool flag = FALSE;
					for(;num>0;num--)
					{
						if(summon_named_creature(0,py,px,MON_LESSER_O_G,mode)) flag = TRUE;
					}
					if(!flag) msg_print("�E�E�悤�ȋC���������A�C�̂����������B"); 
				}
			}

			break;
		}
		case ACT_DETECT_ARTIFACT:
		{
			msg_format("%s���������I",name);
			search_specific_object(2);
			break;
		}
		case ACT_LUNATIC_TIME:
		{
			int time = randint1(20) + 20;
			msg_format("�n���̏��������X�ƋP�����I");
			(void)set_afraid(0);
			(void)set_fast(time,FALSE);

			if (p_ptr->pclass == CLASS_LARVA)
			{
				msg_format("���Ȃ��͏����ɍ��߂�ꂽ�����͂�\���������I");
				project_hack(GF_OLD_POLY, p_ptr->lev * 2);
				project(0, 3 + randint1(3), py, px, 10, GF_MAKE_FLOWER, PROJECT_JUMP | PROJECT_GRID, -1);
				//���΂炭�g���Ȃ�����
				msg_print("�����������Ă��܂���...");
				p_ptr->update |= PU_TORCH;
				o_ptr->timeout += 20 + randint1(20);
				return TRUE;
			}
			else
			{
				aggravate_monsters(-1, TRUE);
				if (one_in_(3))sanity_blast(0, TRUE);
				else (void)set_shero(time, FALSE);
			}

			break;
		}
		case ACT_SUMMON_GHOSTS:
			{
				int num = 1 + plev / 20;
				bool flag = FALSE;

				for(;num>0;num--)
				{
					if(summon_specific(0,py,px,(dun_level/2+plev/2),SUMMON_GHOST,(PM_ALLOW_GROUP | PM_FORCE_PET))) flag = TRUE;
				}
				if(flag)
					msg_print("�s���ɗH�삽��������Ă����B");
				else
					msg_print("���ɉ����N����Ȃ������B");

			}
			break;
		case ACT_TRANSPORT_ITEM:
			{
				msg_print("���P���J���ƒ��������̕����Ɍq�����Ă����B");
				item_recall(0);
			}
			break;
		case ACT_TOYOHIME:
			{
				int tx,ty;
				int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
				if (!get_aim_dir(&dir)) return FALSE;
				msg_format("���Ȃ��͐���J���A�傫���ł��U�����I");
				fire_spark(GF_DISINTEGRATE,dir,999,2);

			break;
			}
		case ACT_MIGHTY_THROW:
			{
				msg_print("���Ȃ��͓�������̂�T���n�߂��E�E");
				if (!do_cmd_throw_aux(4, FALSE, -1)) return FALSE;

			}
			break;

		case ACT_FINAL_STRIKE:
			{
				if( !get_check_strict("����͔j�󂳂�܂��B��낵���ł���? ", CHECK_NO_HISTORY)) return FALSE;
				final_strike(item, TRUE);
			}
			break;

		case ACT_SHIOMITSUTAMA:
			{
				msg_format("�˔@���������ĉQ���������I");
				project(0, 7, py, px, 7, GF_WATER_FLOW, PROJECT_GRID, -1);
				project(0, 7, py, px, (100+randint1(100)), GF_WATER, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

			}
			break;

		case ACT_DAZZLE:
			{
				int power = 100 + plev * 4;
				if(o_ptr->name1 == ART_BEPPIN)
					msg_format("�u�L���X�e�E�S�[����...�v");
				else
					msg_print("���Ȃ��̎���ɗl�X�Ȍ����x����...");
				confuse_monsters(power);
				stun_monsters(power);

			}
			break;
			//v1.1.31 �����L�΂��@�ˌ������r�[������
		case ACT_MID_RANGE_ATTACK:
			{
				int dam = 100 + randint1(100);
				project_length = 5;

				if(p_ptr->csp < 30)
				{
					msg_print("������MP������Ȃ��I");
					return FALSE;
				}

				if (!get_aim_dir(&dir)) return FALSE;

				msg_print("�u���b�N���b�h���L�т��I");
				p_ptr->csp -= 30;
				p_ptr->redraw |= PR_MANA;
				fire_beam(GF_ARROW,dir,dam);

			}
			break;
		case ACT_BO_INACT:
			{

				if (!get_aim_dir(&dir)) return FALSE;
				msg_print("��̐n����񂾁I");
				fire_bolt(GF_INACT, dir, damroll(4, 6));
			}
			break;

			//v1.1.32
		case ACT_MATARA:
			{
				int		v = 25 + randint1(25);
				msg_print("���C���ǂ��ۂ̉��������n����...");
				set_blessed(v,FALSE);
				set_resist_magic(v,FALSE);
				set_protevil(v,FALSE);
			}
			break;
			//v1.1.36
		case ACT_HYAKKIYAKOU:
			{
				bool flag = FALSE;
				int num = 6+randint1(6);
				for(;num>0;num--)
				{
					if(summon_specific(0,py,px,(dun_level/2+plev/2),SUMMON_TSUKUMO,(PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))) flag = TRUE;
				}
				if(flag)
					msg_print("�ǂ�����Ƃ��Ȃ��t�r�_���������ꂽ�B");
				else
					msg_print("���ɉ����N����Ȃ������B");
			}
			break;
		case ACT_SWORD_DANCE:
			{
				int num;
				cave_type       *c_ptr;
				monster_type    *m_ptr;

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
				{
					msg_print("����������Ă��Ȃ��Ƃ��̔����͎g���Ȃ��B");
					return FALSE;
				}

				msg_print("���Ȃ��͌��̕�����x�����I");
				//�אڃO���b�h�̃����_���ȃ����X�^�[��4��U������
				for (num = 0; num < 4; num++)
				{
					int cnt = 0;
					int tx, ty;
					for (dir = 0; dir < 9; dir++)
					{
						int y = py + ddy_ddd[dir];
						int x = px + ddx_ddd[dir];
						c_ptr = &cave[y][x];
						m_ptr = &m_list[c_ptr->m_idx];
						if (c_ptr->m_idx && m_ptr->ml && !is_pet(m_ptr) && !is_friendly(m_ptr))
						{
							cnt++;
							if (one_in_(cnt))
							{
								ty = y;
								tx = x;
							}
						}
					}
					if (cnt)
						py_attack(ty, tx, 0);
					else
						break;

				}

			}
			break;

		case ACT_BA_LITE://v1.1.57 �A�C�e���u�������v�����ŃX�^�[�o�[�X�g���g����悤�ɂ��Ă݂�
			{
				if (!get_aim_dir(&dir)) return FALSE;
				msg_format("%s���M����������I", name);
				fire_ball(GF_LITE, dir, (50 + plev * 4 + damroll(10,10)), 4);
				break;
			}
			//v1.1.62 �V�A�C�e���u���@�̖�v�̔���
		case ACT_ACCESS_HOME_ONLY_ARROW:
			{

				hack_flag_access_home_only_arrow = TRUE;
				do_cmd_store();
				hack_flag_access_home_only_arrow = FALSE;


			}

			break;

		case ACT_SUMMON_HANIWA:
		{
			if(summon_specific(-1, py, px, dun_level, SUMMON_HANIWA, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				msg_print("���s�̕��c�����������B");
			else
				msg_print("��������Ȃ�����...");

			break;
		}

		//v1.1.74
		case ACT_TIM_ADD_DICES:
		{
			int base = 4 + p_ptr->lev / 8;
			int sides = 4 + p_ptr->lev / 8;

			set_ele_attack(ATTACK_INC_DICES, base + randint1(sides));
			break;
		}

		case ACT_PSY_SPEAR:
		{
			msg_print("ῂ��P���n�߂�...");
			if (!get_aim_dir(&dir)) return FALSE;
			fire_beam(GF_PSY_SPEAR, dir, randint1(150) + 150);
			break;
		}

		case ACT_LONG_RANGE_ATTACK:
		{
			monster_type *m_ptr;
			int x, y;

			project_length = 3 + plev / 12;
			if (!get_aim_dir(&dir)) return FALSE;
			if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
			{
				msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
				return FALSE;
			}
			y = target_row;
			x = target_col;

			m_ptr = &m_list[cave[y][x].m_idx];

			if (!m_ptr->r_idx || !m_ptr->ml)
			{
				msg_print("�����ɂ͉������Ȃ��B");
				return FALSE;
			}
			else
			{
				py_attack(m_ptr->fy, m_ptr->fx, HISSATSU_NO_AURA);
			}
			break;

		}
		case ACT_HIT_AND_AWAY:
		{
			msg_print("���Ȃ��͑f�����������B");
			if (!hit_and_away()) return FALSE;

		}
		break;
		case ACT_SAINT_STAR_ARROW:
		{
			msg_print("�|�͗͋����P���Ă���...");
			if (p_ptr->shoot_penalty < SHOOT_HIGH_PENALTY)
			{
				msg_print("���������̑����ł͎ˌ����ł��Ȃ��B");
				return FALSE;
			}

			snipe_type = SP_FINAL;
			do_cmd_fire();
			snipe_type = 0;

			break;
		}
		case ACT_TIM_SUPER_STEALTH:
		{
			int base = 2 + p_ptr->lev / 12;
			int dice = 2 + p_ptr->lev / 12;

			msg_print("���Ȃ��̋C�z���������B");

			set_tim_superstealth(base + randint1(dice), FALSE, SUPERSTEALTH_TYPE_NORMAL);
			break;
		}

		case ACT_DUAL_CAST:
		{

			if (!can_do_cmd_cast()) return FALSE;
			do_cmd_cast();
			handle_stuff();
			if (can_do_cmd_cast())
				do_cmd_cast();

			break;
		}
		case ACT_ART_CAPTURE:
		{
			if (!activate_artifact_capture(o_ptr)) return FALSE;


			break;
		}
		case ACT_TRANSFORM:
		{
			int r_idx = 0;

			get_mon_num_prep(mon_hook_random_metamorphose, NULL);
			r_idx = get_mon_num(p_ptr->lev);
			get_mon_num_prep(NULL, NULL);

			metamorphose_to_monster(r_idx, 100);
			break;

		}

		case ACT_MAKE_MAGMA:
		{

			msg_format("%s���^���ԂɋP���n�߂�...",name);

			if (!get_aim_dir(&dir)) return FALSE;
			if (!fire_ball_jump(GF_LAVA_FLOW, dir, 3 + randint1(2), 4, NULL)) return FALSE;
			fire_ball_jump(GF_FIRE, dir, 300, 5, NULL);
			break;
		}

		case ACT_MAKE_BLIZZARD:
		{

			msg_format("%s�������P���n�߂�...", name);

			if (!get_aim_dir(&dir)) return FALSE;
			if (!fire_ball_jump(GF_MAKE_BLIZZARD, dir, 3 + randint1(2), 4, NULL)) return FALSE;
			fire_ball_jump(GF_COLD, dir, 300, 5, NULL);
			break;
		}

		case ACT_DIG_OIL:
		{
			msg_print("���Ȃ��͐Ζ����@��n�߂�...");
			//v1.1.94 �����X�^�[�ւ̃_���[�W�͂Ȃ����A�����Z�Ŏ𑮐��ɕς��̂Ńp���[�������グ�Ă���
			project(0, 4, py, px, plev*10, GF_DIG_OIL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);
			break;
		}

		break;

		case ACT_BLAST_MISSILES:
		{
			int dice = 4;
			int sides = 4+plev/6;
			int num = 5 + plev / 12;

			if (!get_aim_dir(&dir)) return FALSE;
			msg_format("���Ȃ��͖��@�̖�𗐎˂����I");
			fire_blast(GF_MISSILE, dir, dice, sides, 10, 3, 0);
			break;
		}

		case ACT_TRANSPORTATION_TRAP:
		{
			int v = 10 + randint1(10);

			set_tim_transportation_trap(v, FALSE);

		}
		break;

		case ACT_MUGENSYUKU:
		{
			int	dam = p_ptr->lev * 6;

			//�ޏ��n�E�Ƃ͈З�2�{
			if (p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_SANAE || (CHECK_MIZUCHI_RESURRECT))
				dam *= 2;
			msg_print("��Ԃ�����ȗ�̉��ɕ����ꂽ�B");
			project_hack2(GF_HOLY_FIRE, 0, 0, dam);
			project_hack(GF_NO_MOVE, dam / 10);

		}
		break;

		default:
		{
#ifdef JP
			msg_format("Unknown activation effect: %d.", act_ptr->index);
#else
			msg_format("Unknown activation effect: %d.", act_ptr->index);
#endif
			return FALSE;
		}
	}

	/* Set activation timeout */
	if (act_ptr->timeout.constant >= 0) 
	{
		o_ptr->timeout = act_ptr->timeout.constant;
		if (act_ptr->timeout.dice > 0) {
			o_ptr->timeout += randint1(act_ptr->timeout.dice);
		}
	} 
	else 
	{
		/* Activations that have special timeout */
		switch (act_ptr->index) {
		case ACT_BR_FIRE:
			o_ptr->timeout = ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES)) ? 200 : 250;
			break;
		case ACT_BR_COLD:
			o_ptr->timeout = ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE)) ? 200 : 250;
			break;
		case ACT_TERROR:
			o_ptr->timeout = 3 * (p_ptr->lev + 10);
			break;
		case ACT_MURAMASA:
			/* Nothing to do */
			break;
		default:
			msg_format("Special timeout is not implemented: %d.", act_ptr->index);
			return FALSE;
		}
	}

	return TRUE;
}

#define BLOODY_MOON_TYPE_NUM 18
const int bloody_moon_type[BLOODY_MOON_TYPE_NUM][2] =
{
	{TR_CHAOTIC,-1},
	{TR_VAMPIRIC,-1},
	{TR_VORPAL,TR_EX_VORPAL},
	{TR_IMPACT,-1},
	{TR_BRAND_POIS,-1},
	{TR_BRAND_ACID,-1},
	{TR_BRAND_ELEC,-1},
	{TR_BRAND_FIRE,-1},
	{TR_BRAND_COLD,-1},
	{TR_SLAY_EVIL,-1},
	{TR_SLAY_GOOD,-1},
	{TR_SLAY_ANIMAL,TR_KILL_ANIMAL},
	{TR_SLAY_HUMAN,TR_KILL_HUMAN},
	{TR_SLAY_UNDEAD,TR_KILL_UNDEAD},
	{TR_SLAY_DRAGON,TR_KILL_DRAGON},
	{TR_SLAY_DEITY,TR_KILL_DEITY},
	{TR_SLAY_DEMON,TR_KILL_DEMON},
	{TR_SLAY_KWAI,TR_KILL_KWAI},
};

/*:::�u���b�f�B���[���̑����ύX*/
///item �u���b�f�B���[���@�X���C�ƃp�����[�^���A�A�C�e���t���O�Ɋւ���n�[�h�R�[�f�B���O�L
///mod150411 �V�����A�C�e���t���O�l�ɍ��킹�ďC���B���̍��܂ŖY��Ă��B
void get_bloody_moon_flags(object_type *o_ptr)
{
	int dummy, i;

	for (i = 0; i < TR_FLAG_SIZE; i++)
		o_ptr->art_flags[i] = a_info[ART_BLOOD].flags[i];

	dummy = randint1(2) + randint1(3);//�{�ƂƈႢKILL_??����x�ł��Ȃ��Ȃ��������𑝂₵��
	for (i = 0; i < dummy; i++)
	{
		int type = randint0(BLOODY_MOON_TYPE_NUM);

		add_flag(o_ptr->art_flags, bloody_moon_type[type][0]);
		if(bloody_moon_type[type][1] >= 0 && one_in_(4))
		{
			add_flag(o_ptr->art_flags, bloody_moon_type[type][1]);
		}
	}

	dummy = randint1(2);
	for (i = 0; i < dummy; i++) one_resistance(o_ptr);

	for (i = 0; i < 2; i++)
	{
		int tmp = randint0(11);
		if (tmp < 6) add_flag(o_ptr->art_flags, TR_STR + tmp);
		else add_flag(o_ptr->art_flags, TR_STEALTH + tmp - 6);
	}
}

/*:::�Œ�A�[�e�B�t�@�N�g�ɑ΂��ǉ��ϐ���\�͂�t���B�E�ƈˑ��̓��ꏈ���������ōs��*/
///item class race �E�ƁA���ʈˑ��Ȃǂ̃A�C�e���������ꏈ��
void random_artifact_resistance(object_type * o_ptr, artifact_type *a_ptr)
{
	bool give_resistance = FALSE, give_power = FALSE;

	//v2.1.0 ����̓A�C�e���̎􂢂𖳌��� �􂢃t���O��object_flags()�Ŗ�������̂ł����ł�pval�Ȃǂ̃}�C�i�X��0�ɂ���
	if (p_ptr->pclass == CLASS_MIZUCHI && object_is_equipment(o_ptr))
	{
		if (o_ptr->to_a < 0) o_ptr->to_a = 0;
		if (o_ptr->to_d < 0) o_ptr->to_d = 0;
		if (o_ptr->to_h < 0) o_ptr->to_h = 0;
		if (o_ptr->pval < 0) o_ptr->pval = 0;
	}



	if (o_ptr->name1 == ART_TERROR) /* Terror Mask is for warriors... */
	{
		if (p_ptr->pclass == CLASS_PARSEE || p_ptr->pclass == CLASS_WARRIOR || p_ptr->pclass == CLASS_ARCHER || p_ptr->pclass == CLASS_CAVALRY || p_ptr->pseikaku == SEIKAKU_BERSERK || p_ptr->pclass == CLASS_CLOWNPIECE)
		{
			give_power = TRUE;
			give_resistance = TRUE;
		}
		else
		{
			add_flag(o_ptr->art_flags, TR_AGGRAVATE);
			add_flag(o_ptr->art_flags, TR_TY_CURSE);
			o_ptr->curse_flags |=
			    (TRC_CURSED | TRC_HEAVY_CURSE);
			o_ptr->curse_flags |= get_curse(2, o_ptr);
			return;
		}
	}
	/*:::�_�X�̉����̓t�����̂Ƃ����Ẩ��O���t���Ȃ����Ƃɂ���*/
	else if (o_ptr->name1 == ART_TWILIGHT) 
	{
		if (p_ptr->pclass == CLASS_FLAN)
		{
				o_ptr->to_d = 20;
				o_ptr->to_h = 20;
		}
		else
		{
			add_flag(o_ptr->art_flags, TR_TY_CURSE);
		}
		return;
	}
	//v2.0.20 �O�ό��͗d���v���C���ȊO�ɂ̓_���[�W�����EAC�啝���������悤�ɂ���
	else if (o_ptr->name1 == ART_ROUKANKEN)
	{
		if (p_ptr->pclass != CLASS_YOUMU)
		{
			o_ptr->to_d = 25;
			o_ptr->to_a = -25;
			add_flag(o_ptr->art_flags, TR_AGGRAVATE);
		}
		return;

	}
	//v2.1.0 ����̂Ƃ��n���̎�ւɉi���̎�
	else if (o_ptr->name1 == ART_HARNESS_HELL)
	{
		if (p_ptr->pclass == CLASS_MIZUCHI)
		{
			o_ptr->curse_flags |= TRC_PERMA_CURSE;
		}
		return;

	}

	else if (o_ptr->name1 == ART_MURAMASA)
	{
		if (p_ptr->pclass != CLASS_SAMURAI && p_ptr->pclass != CLASS_YOUMU)
		{
			add_flag(o_ptr->art_flags, TR_NO_MAGIC);
			o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
		}
	}
	else if (o_ptr->name1 == ART_XIAOLONG)
	{
		if (p_ptr->pclass == CLASS_MONK)
			add_flag(o_ptr->art_flags, TR_BLOWS);
	}
	else if (o_ptr->name1 == ART_BLOOD)
	{
		get_bloody_moon_flags(o_ptr);
	}
	else if (o_ptr->name1 == ART_HEAVENLY_MAIDEN)
	{
		if (p_ptr->psex != SEX_FEMALE)
		{
			add_flag(o_ptr->art_flags, TR_AGGRAVATE);
		}
	}
	//�ꕔ�̏e�ɓ���p�����[�^�t��
	else if(o_ptr->name1 == ART_FREEZER)
	{
		o_ptr->xtra1 = GF_COLD;
		o_ptr->xtra4 = GUN_FIRE_MODE_BEAM;
	}
	else if(o_ptr->name1 == ART_RAT)
	{
		o_ptr->xtra1 = GF_ACID;
		o_ptr->xtra4 = GUN_FIRE_MODE_BOLT;

	}


	if (a_ptr->gen_flags & (TRG_XTRA_POWER)) give_power = TRUE;
	if (a_ptr->gen_flags & (TRG_XTRA_H_RES)) give_resistance = TRUE;
	if (a_ptr->gen_flags & (TRG_XTRA_RES_OR_POWER))
	{
		/* Give a resistance OR a power */
		if (one_in_(2)) give_resistance = TRUE;
		else give_power = TRUE;
	}

	if (give_power)
	{
		one_ability(o_ptr);
	}

	if (give_resistance)
	{
		one_high_resistance(o_ptr);
	}


}


/*
 * Create the artifact of the specified number
 */
/*:::�w�肵��id�́����w��ʒu�ɐ���*/
/*:::���łɏo���ς݂��ǂ����͔���ς݂ł��邱��*/
bool create_named_art(int a_idx, int y, int x)
{
	object_type forge;
	object_type *q_ptr;
	int i;

	artifact_type *a_ptr = &a_info[a_idx];

	/* Get local object */
	q_ptr = &forge;

	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return FALSE;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	/*:::a_info�ɂ��邪���݂��Ȃ��x�[�X�A�C�e���̂Ƃ�*/
	if (!i) return FALSE;

	/* Create the artifact */
	object_prep(q_ptr, i);

	/* Save the name */
	q_ptr->name1 = a_idx;

	/* Extract the fields */
	q_ptr->pval = a_ptr->pval;
	q_ptr->ac = a_ptr->ac;
	q_ptr->dd = a_ptr->dd;
	q_ptr->ds = a_ptr->ds;
	q_ptr->to_a = a_ptr->to_a;
	q_ptr->to_h = a_ptr->to_h;
	q_ptr->to_d = a_ptr->to_d;
	q_ptr->weight = a_ptr->weight;

	///mod150908 �N���E���s�[�X�̏����̓��ꏈ��
	if(q_ptr->name1 == ART_CLOWNPIECE) q_ptr->xtra4 = -1;


	/* Hack -- extract the "cursed" flag */
	if (a_ptr->gen_flags & TRG_CURSED) q_ptr->curse_flags |= (TRC_CURSED);
	if (a_ptr->gen_flags & TRG_HEAVY_CURSE) q_ptr->curse_flags |= (TRC_HEAVY_CURSE);
	if (a_ptr->gen_flags & TRG_PERMA_CURSE) q_ptr->curse_flags |= (TRC_PERMA_CURSE);
	if (a_ptr->gen_flags & (TRG_RANDOM_CURSE0)) q_ptr->curse_flags |= get_curse(0, q_ptr);
	if (a_ptr->gen_flags & (TRG_RANDOM_CURSE1)) q_ptr->curse_flags |= get_curse(1, q_ptr);
	if (a_ptr->gen_flags & (TRG_RANDOM_CURSE2)) q_ptr->curse_flags |= get_curse(2, q_ptr);

	random_artifact_resistance(q_ptr, a_ptr);

	/*
	 * drop_near()���ŕ��ʂ̌Œ�A�[�e�B�t�@�N�g���d�Ȃ�Ȃ������Ɉˑ�����.
	 * ����2�ȏ㑶�݉\�������i�ȊO�̌Œ�A�[�e�B�t�@�N�g���쐬������
	 * ���̊֐��̕Ԃ�l�͐M�p�ł��Ȃ��Ȃ�.
	 */

	//���s���ۓW�̓���A�C�e��
	if(p_ptr->inside_quest == QUEST_MOON_VAULT && a_idx == ART_HAGOROMO_MOON)
		q_ptr->marked |= OM_SPECIAL_FLAG;

	/* Drop the artifact from heaven */
	return drop_near(q_ptr, -1, y, x) ? TRUE : FALSE;
}

/*:::�A�C�e���̎�ނŕ��򂵃t���O��t������B*/
/*:::�x�[�X�A�C�e���Ɉ��m���ŕt�������t���O���������邽�߂ɍ���Ă݂��B*/
/*:::apply_magic()�̒��́�����̌�ōs���B�i�O�ɍs���Ă���������邾���̂͂������j*/
/*:::�u|�؂��d�Ȃ�����|X�؂ɂȂ�v�݂����ȏ����́������ł͋@�\���邪�A�G�S�����ł͋@�\���Ȃ��B�ʏ������K�v�B*/
void add_art_flags(object_type * o_ptr)
{

	if(o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_EVIL_KATANA && one_in_(2))
	{
		add_flag(o_ptr->art_flags, TR_VORPAL);
		//if(one_in_(4)) 	add_flag(o_ptr->art_flags, TR_AGGRAVATE);
		if(one_in_(4))
		{
			o_ptr->to_a -= damroll(3,10);
			o_ptr->to_d += 5;
		}
	}
	else if(o_ptr->tval == TV_SWORD && o_ptr->sval == SV_WEAPON_CLAYMORE && one_in_(4))
	{
		add_flag(o_ptr->art_flags, TR_VORPAL);
	}
	else if(o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_KATANA && one_in_(2))
	{
		add_flag(o_ptr->art_flags, TR_VORPAL);
	}
	else if(o_ptr->tval == TV_SPEAR && o_ptr->sval == SV_WEAPON_TRIFURCATE_SPEAR)
	{
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_THROW);

		if(one_in_(3)) add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		else add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
	}
	else if(o_ptr->tval == TV_POLEARM && o_ptr->sval == SV_WEAPON_SCYTHE_OF_SLICING)
	{
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		if(one_in_(2)) add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
	}

	else if(o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_MIKO && one_in_(2))
	{
		add_flag(o_ptr->art_flags, TR_RES_HOLY);
	}
	else if(o_ptr->tval == TV_CLOAK && o_ptr->sval == SV_CLOAK_FUR && one_in_(3))
	{
		add_flag(o_ptr->art_flags, TR_RES_COLD);
	}
	else if(o_ptr->tval == TV_HEAD && o_ptr->sval == SV_HEAD_MASK && one_in_(3))
	{
		add_flag(o_ptr->art_flags, TR_RES_BLIND);
	}
	else if(o_ptr->tval == TV_HEAD && o_ptr->sval == SV_HEAD_KASA && one_in_(5))
	{
		add_flag(o_ptr->art_flags, TR_RES_BLIND);
	}
}
