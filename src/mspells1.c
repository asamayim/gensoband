/* File: mspells1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Monster spells (attack player) */

#include "angband.h"


/*
 * And now for Intelligent monster attacks (including spells).
 *
 * Original idea and code by "DRS" (David Reeves Sward).
 * Major modifications by "BEN" (Ben Harrison).
 *
 * Give monsters more intelligent attack/spell selection based on
 * observations of previous attacks on the player, and/or by allowing
 * the monster to "cheat" and know the player status.
 *
 * Maintain an idea of the player status, and use that information
 * to occasionally eliminate "ineffective" spell attacks.  We could
 * also eliminate ineffective normal attacks, but there is no reason
 * for the monster to do this, since he gains no benefit.
 * Note that MINDLESS monsters are not allowed to use this code.
 * And non-INTELLIGENT monsters only use it partially effectively.
 *
 * Actually learn what the player resists, and use that information
 * to remove attacks or spells before using them.  This will require
 * much less space, if I am not mistaken.  Thus, each monster gets a
 * set of 32 bit flags, "smart", build from the various "SM_*" flags.
 *
 * This has the added advantage that attacks and spells are related.
 * The "smart_learn" option means that the monster "learns" the flags
 * that should be set, and "smart_cheat" means that he "knows" them.
 * So "smart_cheat" means that the "smart" field is always up to date,
 * while "smart_learn" means that the "smart" field is slowly learned.
 * Both of them have the same effect on the "choose spell" routine.
 */



/*
 * Internal probability routine
 */
static bool int_outof(monster_race *r_ptr, int prob)
{
	/* Non-Smart monsters are half as "smart" */
	if (!(r_ptr->flags2 & RF2_SMART)) prob = prob / 2;

	/* Roll the dice */
	return (randint0(100) < prob);
}


//v1.1.32 �w�胂���X�^�[�ׂ̗Ɂu�����N�A�b�v�\�ȃ����X�^�[�v������΂���m_idx��Ԃ��B
//���Ɨ��T�̓��ʍs���p
static int can_rankup_monster(monster_type *m_ptr)
{
	int i,x,y;
	int m_idx = 0;

	for(i=0;i<10;i++)
	{
		monster_type *tm_ptr;
		monster_race *tr_ptr;

		y = m_ptr->fy+ddy[i];
		x = m_ptr->fx+ddx[i];

		if(!in_bounds(y,x)) continue;
		if(!cave[y][x].m_idx) continue;
		tm_ptr = &m_list[cave[y][x].m_idx];
		tr_ptr = &r_info[tm_ptr->r_idx];
		if (tm_ptr->mflag2 & MFLAG2_CHAMELEON)continue;
		if (tr_ptr->flags7 & RF7_TANUKI) continue;
		if(!tr_ptr->next_exp) continue;
		if(are_enemies(m_ptr,tm_ptr)) continue;

		m_idx = cave[y][x].m_idx;
		break;
	}
	return m_idx;

}

/*
 * Remove the "bad" spells from a spell list
 */
/*:::���X�y�����X�g�̒�������ʂ̂Ȃ����̂����O����@STUPID��smart�I�v�V����OFF�̂Ƃ��͉������Ȃ�*/
///res system�@mon f9flag �w�K�ɂ��G�����ʂ̂Ȃ����@���g��Ȃ��Ȃ�
///mod140406 �����w�K������V�t���O�E�X�y���ɍ��킹�ύX
static void remove_bad_spells(int m_idx, u32b *f4p, u32b *f5p, u32b *f6p, u32b *f9p)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	u32b f4 = (*f4p);
	u32b f5 = (*f5p);
	u32b f6 = (*f6p);
	u32b f9 = (*f9p);

	u32b smart = 0L;

	/* Too stupid to know anything */
	if (r_ptr->flags2 & RF2_STUPID) return;

	/* Must be cheating or learning */
	if (!smart_cheat && !smart_learn) return;

	/* Update acquired knowledge */
	if (smart_learn)
	{
		/* Hack -- Occasionally forget player status */
		/* Only save SM_FRIENDLY, SM_PET or SM_CLONED */
		if (m_ptr->smart && (randint0(100) < 1)) m_ptr->smart &= (SM_FRIENDLY | SM_PET | SM_CLONED);

		/* Use the memorized flags */
		smart = m_ptr->smart;
	}


	/* Cheat if requested */
	//if (smart_cheat)
	///mod140406 SMART_EX�t���O�̓G�͍ŏ�����S�Ēm���Ă���ݒ�ɂ���
	if (smart_cheat || r_ptr->flags2 & RF2_SMART_EX)
	{
		/* Know basic info */
		if (p_ptr->resist_acid) smart |= (SM_RES_ACID);
		if (IS_OPPOSE_ACID()) smart |= (SM_OPP_ACID);
		if (p_ptr->immune_acid) smart |= (SM_IMM_ACID);
		if (p_ptr->resist_elec) smart |= (SM_RES_ELEC);
		if (IS_OPPOSE_ELEC()) smart |= (SM_OPP_ELEC);
		if (p_ptr->immune_elec) smart |= (SM_IMM_ELEC);
		if (p_ptr->resist_fire) smart |= (SM_RES_FIRE);
		if (IS_OPPOSE_FIRE()) smart |= (SM_OPP_FIRE);
		if (p_ptr->immune_fire) smart |= (SM_IMM_FIRE);
		if (p_ptr->resist_cold) smart |= (SM_RES_COLD);
		if (IS_OPPOSE_COLD()) smart |= (SM_OPP_COLD);
		if (p_ptr->immune_cold) smart |= (SM_IMM_COLD);

		/* Know poison info */
		if (p_ptr->resist_pois) smart |= (SM_RES_POIS);
		if (IS_OPPOSE_POIS()) smart |= (SM_OPP_POIS);

		/* Know special resistances */
		if (p_ptr->resist_neth) smart |= (SM_RES_NETH);
		if (p_ptr->resist_lite) smart |= (SM_RES_LITE);
		if (p_ptr->resist_dark) smart |= (SM_RES_DARK);
		if (p_ptr->resist_fear && !(IS_VULN_FEAR)) smart |= (SM_RES_FEAR);
		if (p_ptr->resist_conf) smart |= (SM_RES_CONF);
		if (p_ptr->resist_chaos) smart |= (SM_RES_CHAOS);
		if (p_ptr->resist_disen) smart |= (SM_RES_DISEN);
		if (p_ptr->resist_blind) smart |= (SM_RES_BLIND);
		///del131228 res_nexus
		//if (p_ptr->resist_nexus) smart |= (SM_RES_NEXUS);
		if (p_ptr->resist_sound) smart |= (SM_RES_SOUND);
		if (p_ptr->resist_shard) smart |= (SM_RES_SHARD);
		if (p_ptr->reflect) smart |= (SM_IMM_REFLECT);

		/* Know bizarre "resistances" */
		if (p_ptr->free_act) smart |= (SM_IMM_FREE);
		//if (!p_ptr->msp) smart |= (SM_IMM_MANA);
		/*:::�ǉ��t���O*/
		if(p_ptr->resist_time)  smart |= (SM_RES_TIME);
		if(p_ptr->resist_holy)  smart |= (SM_RES_HOLY);
		if(p_ptr->resist_water)  smart |= (SM_RES_WATER);

	}


	/* Nothing known */
	if (!smart) return;


	if (smart & SM_IMM_ACID)
	{
		f4 &= ~(RF4_BR_ACID);
		f5 &= ~(RF5_BA_ACID);
		f5 &= ~(RF5_BO_ACID);
		f9 &= ~(RF9_ACID_STORM);
	}
	else if ((smart & (SM_OPP_ACID)) && (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_ACID_STORM);
	}
	else if ((smart & (SM_OPP_ACID)) || (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_ACID_STORM);
	}


	if (smart & (SM_IMM_ELEC))
	{
		f4 &= ~(RF4_BR_ELEC);
		f5 &= ~(RF5_BA_ELEC);
		f5 &= ~(RF5_BO_ELEC);
		f9 &= ~(RF9_THUNDER_STORM);
	}
	else if ((smart & (SM_OPP_ELEC)) && (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_THUNDER_STORM);
	}
	else if ((smart & (SM_OPP_ELEC)) || (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_THUNDER_STORM);
	}


	if (smart & (SM_IMM_FIRE))
	{
		f4 &= ~(RF4_BR_FIRE);
		f5 &= ~(RF5_BA_FIRE);
		f5 &= ~(RF5_BO_FIRE);
		f9 &= ~(RF9_FIRE_STORM);
	}
	else if ((smart & (SM_OPP_FIRE)) && (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_FIRE_STORM);
	}
	else if ((smart & (SM_OPP_FIRE)) || (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_FIRE_STORM);
	}


	if (smart & (SM_IMM_COLD))
	{
		f4 &= ~(RF4_BR_COLD);
		f5 &= ~(RF5_BA_COLD);
		f5 &= ~(RF5_BO_COLD);
		f5 &= ~(RF5_BO_ICEE);
		f9 &= ~(RF9_ICE_STORM);
	}
	else if ((smart & (SM_OPP_COLD)) && (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 80)) f9 &= ~(RF9_ICE_STORM);
	}
	else if ((smart & (SM_OPP_COLD)) || (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 20)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_ICE_STORM);
	}

	//�j�M�����g�p���~����
	if (smart & (SM_IMM_FIRE | SM_OPP_FIRE) && smart & (SM_RES_LITE) )
	{
		if (int_outof(r_ptr, 70)) f9 &= ~(RF9_GIGANTIC_LASER);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_NUKE);
	}
	//�v���Y�}����
	if (smart & (SM_IMM_FIRE | SM_OPP_FIRE) &&  smart & (SM_IMM_ELEC | SM_OPP_ELEC) )
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_PLAS);
		if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_PLAS);
	}

	if ((smart & (SM_OPP_POIS)) && (smart & (SM_RES_POIS)))
	{
		if(p_ptr->pclass == CLASS_MEDICINE || p_ptr->pclass == CLASS_EIRIN)
		{
			f4 &= ~(RF4_BR_POIS);
			f5 &= ~(RF5_BA_POIS);
			f9 &= ~(RF9_TOXIC_STORM);
			if (int_outof(r_ptr, 60)) f9 &= ~(RF9_BA_POLLUTE);
		}
		else
		{
			if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_POIS);
			if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_POIS);
			if (int_outof(r_ptr, 80)) f9 &= ~(RF9_TOXIC_STORM);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BA_POLLUTE);
		}
	}
	else if ((smart & (SM_OPP_POIS)) || (smart & (SM_RES_POIS)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_POIS);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_POIS);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_TOXIC_STORM);
	}

	if (smart & (SM_RES_NETH))
	{
		if (prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST) || p_ptr->pclass == CLASS_HECATIA)
		{
			f4 &= ~(RF4_BR_NETH);
			f5 &= ~(RF5_BA_NETH);
			f5 &= ~(RF5_BO_NETH);
		}
		else
		{
			if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_NETH);
		}
	}

	if (smart & (SM_RES_LITE))
	{

		if(is_hurt_lite() < 0)
		{
			f4 &= ~(RF4_BR_LITE);
			f5 &= ~(RF5_BA_LITE);
			f9 &= ~(RF9_BEAM_LITE);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_LITE);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_LITE);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BEAM_LITE);
		}

	}

	//�􂢍U���@�Í��A�n���ϐ��Ŕ���ǉ�
	if (smart & (SM_RES_DARK) || smart & (SM_RES_NETH) )
	{
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_1);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_2);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_3);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_CAUSE_4);
	}
	if (smart & (SM_RES_DARK))
	{
		if (prace_is_(RACE_VAMPIRE) || p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 19)
		{
			f4 &= ~(RF4_BR_DARK);
			f5 &= ~(RF5_BA_DARK);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_DARK);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_DARK);
		}
	}

	if (smart & (SM_RES_FEAR))
	{
		f5 &= ~(RF5_SCARE);
	}

	if (smart & (SM_RES_CONF))
	{
		f5 &= ~(RF5_CONF);
//		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_CONF);
	}

	if (smart & (SM_RES_CHAOS))
	{
		if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_CHAO);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BA_CHAO);
	}

	if (smart & (SM_RES_DISEN))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_DISE);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_BA_POLLUTE);
	}

	if (smart & (SM_RES_BLIND))
	{
		f5 &= ~(RF5_BLIND);
	}

	//if (smart & (SM_RES_NEXUS))
	//{
	//	if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_NEXU);
	//	f6 &= ~(RF6_TELE_LEVEL);
	//}

	if (smart & (SM_RES_SOUND))
	{

		if (p_ptr->pclass == CLASS_KYOUKO && p_ptr->lev > 29)
		{
			f4 &= ~(RF4_BR_SOUN);
			f9 &= ~(RF9_BO_SOUND);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_SOUN);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BO_SOUND);
		}
	}

	if (smart & (SM_RES_SHARD))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_SHAR);

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
		{
			f4 &= ~(RF4_BR_SHAR | RF4_ROCKET);
		}

	}

	if (smart & (SM_IMM_REFLECT))
	{
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_NETH);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_WATE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_MANA);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_PLAS);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_MISSILE);
		if (int_outof(r_ptr, 150)) f4 &= ~(RF4_SHOOT);
		f4 &= ~(RF4_DANMAKU);
		if (int_outof(r_ptr, 150)) f9 &= ~(RF9_BO_HOLY);
	}

	if (smart & (SM_IMM_FREE))
	{
		f5 &= ~(RF5_HOLD);
		f5 &= ~(RF5_SLOW);
	}
/*
	if (smart & (SM_IMM_MANA))
	{
		f5 &= ~(RF5_DRAIN_MANA);
	}
*/
	/*:::�ǉ�����*/
	if (smart & (SM_RES_HOLY))
	{
		f9 &= ~(RF9_PUNISH_1);
		f9 &= ~(RF9_PUNISH_2);
		f9 &= ~(RF9_PUNISH_3);
		f9 &= ~(RF9_PUNISH_4);
		if(is_hurt_holy() < 0)
		{
			f4 &= ~(RF4_BR_HOLY);
			f9 &= ~(RF9_BA_HOLY);
			f9 &= ~(RF9_BO_HOLY);
			f9 &= ~(RF9_HOLYFIRE);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_HOLY);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_BA_HOLY);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_HOLYFIRE);
			if (int_outof(r_ptr, 70)) f9 &= ~(RF9_BO_HOLY);
		}

	}
	if (smart & (SM_RES_TIME))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_NEXU);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_TIME);
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_GRAV);
		if (int_outof(r_ptr, 30)) f9 &= ~(RF9_BA_DISTORTION);
		if (int_outof(r_ptr, 70)) f6 &= ~(RF6_TELE_AWAY);
		f6 &= ~(RF6_TELE_LEVEL);

	}
	if (smart & (SM_RES_WATER))
	{

		if (m_ptr->r_idx == MON_YUMA)
		{
			; //�ޖ��̃��C���V���g�����͉񕜍s�������˂Ă���̂Ł��ɐ��ϐ��������Ă��~�߂Ȃ�
		}
		else if(is_hurt_water() < 0)
		{
 			f4 &= ~(RF4_BR_AQUA);
			f5 &= ~(RF5_BO_WATE);
			f5 &= ~(RF5_BA_WATE);
			f9 &= ~(RF9_MAELSTROM);

		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_AQUA);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_WATE);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_WATE);
			if (int_outof(r_ptr, 50)) f9 &= ~(RF9_MAELSTROM);
		}

	}


	/* XXX XXX XXX No spells left? */
	/* if (!f4 && !f5 && !f6) ... */

	(*f4p) = f4;
	(*f5p) = f5;
	(*f6p) = f6;
	(*f9p) = f9;
}


/*
 * Determine if there is a space near the player in which
 * a summoned creature can appear
 */
/*:::���̋߂��Ƀ����X�^�[����������X�y�[�X�����邩�ǂ�������*/
bool summon_possible(int y1, int x1)
{
	int y, x;

	/* Start at the player's location, and check 2 grids in each dir */
	for (y = y1 - 2; y <= y1 + 2; y++)
	{
		for (x = x1 - 2; x <= x1 + 2; x++)
		{
			/* Ignore illegal locations */
			if (!in_bounds(y, x)) continue;

			/* Only check a circular area */
			if (distance(y1, x1, y, x)>2) continue;

			/* ...nor on the Pattern */
			if (pattern_tile(y, x)) continue;

			/* Require empty floor grid in line of projection */
			if (cave_empty_bold(y, x) && projectable(y1, x1, y, x) && projectable(y, x, y1, x1)) return (TRUE);
		}
	}

	return FALSE;
}

/*:::�G�����l�Ԃ��̖��@���g���邩����*/
///del131221 ���̊֘A�폜
#if 0
bool raise_possible(monster_type *m_ptr)
{
	int xx, yy;
	int y = m_ptr->fy;
	int x = m_ptr->fx;
	s16b this_o_idx, next_o_idx = 0;
	cave_type *c_ptr;

	for (xx = x - 5; xx <= x + 5; xx++)
	{
		for (yy = y - 5; yy <= y + 5; yy++)
		{
			if (distance(y, x, yy, xx) > 5) continue;
			if (!los(y, x, yy, xx)) continue;
			if (!projectable(y, x, yy, xx)) continue;

			c_ptr = &cave[yy][xx];
			/* Scan the pile of objects */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire object */
				object_type *o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Known to be worthless? */
				if (o_ptr->tval == TV_CORPSE)
				{
					if (!monster_has_hostile_align(m_ptr, 0, 0, &r_info[o_ptr->pval])) return TRUE;
				}
			}
		}
	}
	return FALSE;
}
#endif

/*
 * Originally, it was possible for a friendly to shoot another friendly.
 * Change it so a "clean shot" means no equally friendly monster is
 * between the attacker and target.
 */
/*
 * Determine if a bolt spell will hit the player.
 *
 * This is exactly like "projectable", but it will
 * return FALSE if a monster is in the way.
 * no equally friendly monster is
 * between the attacker and target.
 */
/*:::�����͂̃����X�^�[�Ƀ{���g�������邩�ǂ�������*/
bool clean_shot(int y1, int x1, int y2, int x2, bool _friend)
{
	/* Must be the same as projectable() */

	int i, y, x;

	int grid_n = 0;
	u16b grid_g[512];

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, 0);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	/* Final grid */
	y = GRID_Y(grid_g[grid_n-1]);
	x = GRID_X(grid_g[grid_n-1]);

	/* May not end in an unrequested grid */
	if ((y != y2) || (x != x2)) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if ((cave[y][x].m_idx > 0) && !((y == y2) && (x == x2)))
		{
			monster_type *m_ptr = &m_list[cave[y][x].m_idx];
			if (_friend == is_pet(m_ptr))
			{
				return (FALSE);
			}
		}
		/* Pets may not shoot through the character - TNB */
		if (player_bold(y, x))
		{
			if (_friend) return (FALSE);
		}
	}

	return (TRUE);
}

/*
 * Cast a bolt at the player
 * Stop if we hit a monster
 * Affect monsters and the player
 */
static void bolt(int m_idx, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_PLAYER;
	if (typ != GF_ARROW) flg  |= PROJECT_REFLECTABLE;

	/* Target the player with a bolt attack */
	(void)project(m_idx, 0, py, px, dam_hp, typ, flg, (learnable ? monspell : -1));
}

static void beam(int m_idx, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

	/* Target the player with a bolt attack */
	(void)project(m_idx, 0, py, px, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*
 * Cast a breath (or ball) attack at the player
 * Pass over any monsters that may be in the way
 * Affect grids, objects, monsters, and the player
 */
///sys mon �j�׌n�̎􂢂Ȃǃ����X�^�[�̓��ˌn�X�L���������ύX�v�H
static void breath(int y, int x, int m_idx, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Determine the radius of the blast */
	if ((rad < 1) && breath) rad = (r_ptr->flags2 & (RF2_POWERFUL)) ? 3 : 2;

	/* Handle breath attacks */
	if (breath) rad = 0 - rad;

	switch (typ)
	{
	///spell �O�Ղ̂Ȃ����@�͂����Œ�`����K�v������
	case GF_ROCKET:
		flg |= PROJECT_STOP;
		break;
	case GF_DRAIN_MANA:
	case GF_MIND_BLAST:
	case GF_BRAIN_SMASH:
	case GF_CAUSE_1:
	case GF_CAUSE_2:
	case GF_CAUSE_3:
	case GF_CAUSE_4:
	case GF_PUNISH_1:
	case GF_PUNISH_2:
	case GF_PUNISH_3:
	case GF_PUNISH_4:
	case GF_COSMIC_HORROR:
	case GF_HAND_DOOM:
	case GF_KYUTTOSHITEDOKA_N:
		flg |= (PROJECT_HIDE | PROJECT_AIMED);
		break;
	case GF_METEOR:
	case GF_TORNADO:
		flg |= PROJECT_JUMP;
		break;

	}


	/* Target the player with a ball attack */
	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

///mod140328 ��^���[�U�[
void masterspark(int y, int x, int m_idx, int typ, int dam_hp, int monspell, bool learnable, int rad)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_THRU | PROJECT_FAST | PROJECT_MASTERSPARK;
	if(typ == GF_DISINTEGRATE) flg |= PROJECT_DISI;

	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*:::�����̎􂢂����߂�*/
u32b get_curse(int power, object_type *o_ptr)
{
	u32b new_curse;

	while(1)
	{
		new_curse = (1 << (randint0(MAX_CURSE)+4));
		if (power == 2)
		{
			if (!(new_curse & TRC_HEAVY_MASK)) continue;
		}
		else if (power == 1)
		{
			if (new_curse & TRC_SPECIAL_MASK) continue;
		}
		else if (power == 0)
		{
			if (new_curse & TRC_HEAVY_MASK) continue;
		}
		if (new_curse == TRC_LOW_MELEE && !object_is_weapon(o_ptr)) continue;
		if (new_curse == TRC_LOW_AC && !object_is_armour(o_ptr)) continue;
		break;
	}
	return new_curse;
}

/*:::�A�C�e�����􂤁Bchance:�����m�� heavy_chance:���ꂽ�ꍇ���ꂪ�����􂢂ɂȂ�m��*/
void curse_equipment(int chance, int heavy_chance)
{
	bool        changed = FALSE;
	int         curse_power = 0;
	int slot;
	u32b        new_curse;
	u32b oflgs[TR_FLAG_SIZE];
	///item �A�C�e���􂢁@����������E�̂Ƃ�����
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_format("�����i�����ꂩ�������A���Ȃ����􂢂��z��������B");
		hina_gain_yaku(chance * 2 + heavy_chance * 5 +  randint1(30));
		return;
	}
	if (p_ptr->pclass == CLASS_SHION)
	{
		msg_format("�����i�����ꂩ�������A���Ȃ�������ɕs�K��w�������B");
		p_ptr->magic_num1[1] += (chance * 10 + heavy_chance * 50 + randint1(1000));
		return;
	}


	slot = INVEN_RARM + randint0(12);

	if(check_invalidate_inventory(slot)) return;
	if (randint1(100) > chance) return;
	o_ptr = &inventory[slot];

	if (!o_ptr->k_idx) return;

	object_flags(o_ptr, oflgs);

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	//v1.1.15 ���ς̏����ςݑ����i�͎���Ȃ�
	if(p_ptr->pclass == CLASS_JUNKO && o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER)
	{
		msg_format("%s�͎􂢂𒵂˕Ԃ����I", o_name);
		return;
	}

	/* Extra, biased saving throw for blessed items */
	if (have_flag(oflgs, TR_BLESSED) && (randint1(888) > chance))
	{
#ifdef JP
msg_format("%s�͎􂢂𒵂˕Ԃ����I", o_name,
#else
		msg_format("Your %s resist%s cursing!", o_name,
#endif

			((o_ptr->number > 1) ? "" : "s"));
		/* Hmmm -- can we wear multiple items? If not, this is unnecessary */
		return;
	}

	if ((randint1(100) <= heavy_chance) &&
	    (object_is_artifact(o_ptr) || object_is_ego(o_ptr)))
	{
		if (!(o_ptr->curse_flags & TRC_HEAVY_CURSE))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_HEAVY_CURSE;
		o_ptr->curse_flags |= TRC_CURSED;
		curse_power++;
	}
	else
	{
		if (!object_is_cursed(o_ptr))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_CURSED;
	}
	if (heavy_chance >= 50) curse_power++;

	new_curse = get_curse(curse_power, o_ptr);
	if (!(o_ptr->curse_flags & new_curse))
	{
		changed = TRUE;
		o_ptr->curse_flags |= new_curse;
	}

	if (changed)
	{
#ifdef JP
msg_format("���ӂɖ����������I�[����%s���Ƃ�܂���...", o_name);
#else
		msg_format("There is a malignant black aura surrounding %s...", o_name);
#endif

		if(p_ptr->pclass == CLASS_KOGASA && o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && o_ptr->weight == 30)
			msg_format("�������̒��������C������...");

		o_ptr->feeling = FEEL_NONE;
	}
	p_ptr->update |= (PU_BONUS);
}

///sys mon �������牺�A���΂炭�����X�^�[�X�y���̕��ޗp�֐��@�S�ăX�y���ԍ��̃n�[�h�R�[�f�B���O
/*
 * Return TRUE if a spell is good for hurting the player (directly).
 */
static bool spell_attack(byte spell)
{
	/* All RF4 spells hurt (except for shriek and dispel) */
	if (spell < 128 && spell > 96 && spell != 98) return (TRUE);

	/* Various "ball" spells */
	if (spell >= 128 && spell <= 128 + 8) return (TRUE);

	/* "Cause wounds" and "bolt" spells */
	if (spell >= 128 + 12 && spell < 128 + 27) return (TRUE);

	/* Hand of Doom */
	if (spell == 160 + 1) return (TRUE);

	/* Psycho-Spear */
	if (spell == 160 + 11) return (TRUE);

	/* Cosmic Horror */
	if (spell == 160 + 15) return (TRUE);

	if(spell >= 192 && spell <= 208) return (TRUE);
	if(spell >= 210 && spell <= 215) return (TRUE);
	if(spell == 222) return (TRUE);

	if(spell == 175) return (TRUE);//�R�Y�~�b�N�z���[


	/* Doesn't hurt */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for escaping.
 */
static bool spell_escape(byte spell)
{
	/* Blink or Teleport */
	if (spell == 160 + 4 || spell == 160 + 5) return (TRUE);

	/* Teleport the player away */
	if (spell == 160 + 9 || spell == 160 + 10) return (TRUE);

	/* �j�� */
	if (spell == 216) return (TRUE);


	/* Isn't good for escaping */
	return (FALSE);
}

/*
 * Return TRUE if a spell is good for annoying the player.
 */
static bool spell_annoy(byte spell)
{
	/* Shriek */
	if (spell == 96 + 0) return (TRUE);

	/* Brain smash, et al (added curses) */
	if (spell >= 128 + 9 && spell <= 128 + 14) return (TRUE);

	/* Scare, confuse, blind, slow, paralyze */
	if (spell >= 128 + 27 && spell <= 128 + 31) return (TRUE);

	/* Teleport to */
	if (spell == 160 + 8) return (TRUE);

	/* Teleport level */
	if (spell == 160 + 10) return (TRUE);

	/* Darkness, make traps, cause amnesia */
	if (spell >= 160 + 12 && spell <= 160 + 14) return (TRUE);
	/* �j�� */
	if (spell == 216) return (TRUE);

	/* �A���[�� */
	if (spell == 223) return (TRUE);

	/* Doesn't annoy */
	return (FALSE);
}

/*
 * Return TRUE if a spell summons help.
 */
static bool spell_summon(byte spell)
{
	/* All summon spells */
	//�������ǉ� 192����
	if (spell >= 160 + 16 && spell < 192) return (TRUE);

	if (spell == 209) return (TRUE);
	if (spell == 217) return (TRUE);
	if (spell == 218) return (TRUE);
	if (spell == 219) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}

#if 0
/*
 * Return TRUE if a spell raise-dead.
 */

static bool spell_raise(byte spell)
{
	/* All raise-dead spells */
	if (spell == 160 + 15) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}
#endif

/*
 * Return TRUE if a spell is good in a tactical situation.
 */
static bool spell_tactic(byte spell)
{
	/* Blink */
	if (spell == 160 + 4) return (TRUE);

	//�אڃe���|
	if (spell == 220) return (TRUE);
	if (spell == 221) return (TRUE);

	/* Not good */
	return (FALSE);
}

/*
 * Return TRUE if a spell makes invulnerable.
 */
static bool spell_invulner(byte spell)
{
	/* Invulnerability */
	if (spell == 160 + 3) return (TRUE);

	/* No invulnerability */
	return (FALSE);
}

/*
 * Return TRUE if a spell hastes.
 */
static bool spell_haste(byte spell)
{
	/* Haste self */
	if (spell == 160 + 0) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell world.
 */
static bool spell_world(byte spell)
{
	/* world */
	if (spell == 160 + 6) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell special.
 */
static bool spell_special(byte spell)
{
//	if (p_ptr->inside_battle) return FALSE;

	/* RF6_SPECIAL */
	if (spell == 160 + 7) return (TRUE);
	//RF4_SPECIAL2
	if( spell ==  96+28) return TRUE;

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell psycho-spear.
 */
static bool spell_psy_spe(byte spell)
{
	/* world */
	if (spell == 160 + 11) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for healing.
 */
static bool spell_heal(byte spell)
{
	/* Heal */
	if (spell == 160 + 2) return (TRUE);

	/* No healing */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for dispel.
 */
static bool spell_dispel(byte spell)
{
	/* Dispel */
	if (spell == 96 + 2) return (TRUE);

	/* No dispel */
	return (FALSE);
}


/*
 * Check should monster cast dispel spell.
 */
/*:::���ɖ��͏��������邩���肷��@�����̏����Ɉ���������ꍇSMART�̓G��1/2�Ŗ��͏����������Ă���*/
///spell mon realm
bool dispel_check(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int i;

	/* Invulnabilty (including the song) */
	if (IS_INVULN() && !(p_ptr->special_defense & MUSOU_TENSEI) && !SUPER_SHION && (YUMA_ULTIMATE_MODE)) return (TRUE);

	//v1.1.44 �C�r���A���W�����[�V����
	if (p_ptr->special_defense & EVIL_UNDULATION_MASK) return (TRUE);

	if (p_ptr->special_defense & SD_LIFE_EXPLODE) return (TRUE);

	if(p_ptr->tim_superstealth) return (TRUE);
	if(p_ptr->tim_sh_death) return (TRUE);
	if(p_ptr->deportation) return (TRUE);

	for(i=0;i<6;i++) if(p_ptr->tim_addstat_num[i]) 
	{
		return (TRUE);
	}

	//���@���̋���
	if(p_ptr->pclass == CLASS_BYAKUREN && p_ptr->tim_general[0]) return (TRUE);

	//�T�O���_�X�̒e��
	if(p_ptr->pclass == CLASS_SAGUME && p_ptr->tim_general[0]) return (TRUE);

	//�����̋���
	if(p_ptr->pclass == CLASS_RINGO && p_ptr->tim_general[0] > 2) return (TRUE);

	//v1.1.17 ���ϓƎ��ꎞ���ʁ@�S��1/2
	if(USE_NAMELESS_ARTS)
	{
		for(i=0;i<TIM_GENERAL_MAX;i++) if(p_ptr->tim_general[0] && one_in_(2)) return (TRUE);
	}

	//v1.1.56 �X�y���J�[�h�@�S��1/2
	if (p_ptr->tim_spellcard_count && one_in_(2)) return (TRUE);


	/* Wraith form */
	if (p_ptr->wraith_form) return (TRUE);

	/* Shield */
	if (p_ptr->shield) return (TRUE);

	/* Magic defence */
	if (p_ptr->magicdef) return (TRUE);

	/* Multi Shadow */
	if (p_ptr->multishadow) return (TRUE);

	/* Robe of dust */
	if (p_ptr->dustrobe) return (TRUE);

	/* Berserk Strength */
	if (p_ptr->shero && (p_ptr->pseikaku != SEIKAKU_BERSERK)) return (TRUE);

	/* Demon Lord */
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD) return (TRUE);
	else if(p_ptr->mimic_form == MIMIC_GIGANTIC) return (TRUE);
	else if(p_ptr->mimic_form == MIMIC_DRAGON) return (TRUE);
	else if (IS_METAMORPHOSIS)//v1.1.48 �����t���O����
	{
		//�h���~�[�Ǝ����̃����X�^�[�ϐg�͖��͏�������ɂ�����Ȃ�
		if((p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME));

		else if (one_in_(2)) return TRUE;

	}
	else if(p_ptr->mimic_form && one_in_(2)) return (TRUE);

	if(p_ptr->kamioroshi && !one_in_(3)) return (TRUE);

	/* Elemental resistances */
	if (r_ptr->flags4 & RF4_BR_ACID || r_ptr->flags9 & RF9_ACID_STORM)
	{

		if (!p_ptr->immune_acid && (p_ptr->oppose_acid || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
		if (p_ptr->special_defense & DEFENSE_ACID) return (TRUE);
	}

	if (r_ptr->flags4 & RF4_BR_FIRE || r_ptr->flags9 & RF9_FIRE_STORM)
	{
		if ((p_ptr->pclass == CLASS_ORIN || p_ptr->pclass == CLASS_FLAN) && (p_ptr->lev > 34));//��Ώ�
		else if ((p_ptr->pclass == CLASS_MOKOU) && (p_ptr->lev > 29));//��Ώ�
		else if ((p_ptr->pclass == CLASS_FUTO) && (p_ptr->lev > 39));//��Ώ�
		else if ((p_ptr->pclass == CLASS_MAYUMI) && (p_ptr->lev > 29));//��Ώ�
		else if ((p_ptr->pclass == CLASS_KEIKI) && (p_ptr->lev > 29));//��Ώ�
		else if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && (p_ptr->lev > 29));
		else
		{
			if (!p_ptr->immune_fire && (p_ptr->oppose_fire || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_FIRE) return (TRUE);
		}
	}

	if (r_ptr->flags4 & RF4_BR_ELEC || r_ptr->flags9 & RF9_THUNDER_STORM)
	{
		if ((p_ptr->pclass == CLASS_IKU ||p_ptr->pclass == CLASS_TOZIKO ||p_ptr->pclass == CLASS_RAIKO) && (p_ptr->lev > 29)); //��Ώ�
		else
		{
			if (!p_ptr->immune_elec && (p_ptr->oppose_elec || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_ELEC) return (TRUE);
		}
	}

	if (r_ptr->flags4 & RF4_BR_COLD || r_ptr->flags9 & RF9_ICE_STORM)
	{
		if ((p_ptr->pclass == CLASS_LETTY) && (p_ptr->lev > 19)) ;//��Ώ�
		else if ((p_ptr->pclass == CLASS_NARUMI) && (p_ptr->lev > 29)) ;//��Ώ�
		else
		{
			if (!p_ptr->immune_cold && (p_ptr->oppose_cold || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST))) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_COLD) return (TRUE);
		}
	}

	if (r_ptr->flags4 & (RF4_BR_POIS) || r_ptr->flags9 & RF9_TOXIC_STORM)
	{
		if ((p_ptr->pclass == CLASS_NINJA) && p_ptr->lev > 44);//��Ώ�
		else if ((p_ptr->pclass == CLASS_MOMOYO) && p_ptr->lev > 19);
		else if ((p_ptr->pclass == CLASS_YUMA) && p_ptr->lev > 19);
		else
		{
			if (p_ptr->oppose_pois || music_singing(MUSIC_NEW_LYRICA_SOLO) || music_singing(MUSIC_RESIST)) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_POIS) return (TRUE);
		}
	}

	/* Ultimate resistance */
	if (p_ptr->ult_res) return (TRUE);

	/* Potion of Neo Tsuyosi special */
	if (p_ptr->tsuyoshi) return (TRUE);

	/* Elemental Brands */
	if ((p_ptr->special_attack & ATTACK_ACID) && !(r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_FIRE) && !(r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_ELEC) && !(r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_COLD) && !(r_ptr->flagsr & RFR_EFF_IM_COLD_MASK)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_POIS) && !(r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)) return (TRUE);

	if (p_ptr->special_attack & ATTACK_DARK) return (TRUE);
	if (p_ptr->special_attack & ATTACK_FORCE) return (TRUE);

	/* Speed */
	if (p_ptr->pspeed < 145)
	{
		if (IS_FAST()) return (TRUE);
	}

	/* Light speed */
	if (p_ptr->lightspeed ) return (TRUE);

	if (p_ptr->riding && (m_list[p_ptr->riding].mspeed < 135))
	{
		if (MON_FAST(&m_list[p_ptr->riding])) return (TRUE);
	}

	/* No need to cast dispel spell */
	return (FALSE);
}

//v1.1.46 �������Z�u�v���b�N�s�W�����v�ɂ��㏸���閂�@���s���̌v�Z
static int pluck_pigeon_magic_fail(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int tmp;

	//�����u�v���b�N�s�W�����v�������̂ݗL��
	if (p_ptr->pclass != CLASS_JYOON) return 0;
	if (!p_ptr->tim_general[0]) return 0;

	//�����X�^�[�����E�����ǂ����̔���͂��Ȃ��B
	//���̃��[�`���̓����X�^�[�����ɖ��@���g���Ƃ��̔���ɂ����e�����Ȃ����߁B

	//�A�C�e����������ł������Ă����+10%�A���ʐ��h���b�v�͂����+10%�A�����1�����莸�s��2%�㏸������B
	//���j�[�N�����X�^�[�͌��ʔ����B
	tmp = 0;
	if (r_ptr->flags1 & RF1_DROP_60) tmp += 120;
	if (r_ptr->flags1 & RF1_DROP_90) tmp += 180;
	if (r_ptr->flags1 & RF1_DROP_1D2) tmp += 300;
	if (r_ptr->flags1 & RF1_DROP_2D2) tmp += 600;
	if (r_ptr->flags1 & RF1_DROP_3D2) tmp += 900;
	if (r_ptr->flags1 & RF1_DROP_4D2) tmp += 1200;
	if (r_ptr->flags1 & RF1_DROP_GREAT) tmp += 1000;
	if (tmp) tmp += 1000;

	if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) tmp /= 2;


	return tmp / 100;
}

/*
 * Have a monster choose a spell from a list of "useful" spells.
 *
 * Note that this list does NOT include spells that will just hit
 * other monsters, and the list is restricted when the monster is
 * "desperate".  Should that be the job of this function instead?
 *
 * Stupid monsters will just pick a spell randomly.  Smart monsters
 * will choose more "intelligently".
 *
 * Use the helper functions above to put spells into categories.
 *
 * This function may well be an efficiency bottleneck.
 */
/*:::���ʂȂǂ��낢�딻��ς݂�spells[]����ǂ̖��@���g�����I��*/
///mon
static int choose_attack_spell(int m_idx, byte spells[], byte num)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ���flags9�ǉ������̂ł����̔z��T�C�Y���ꉞ96��128�Ɋg�����Ă���
	byte escape[128], escape_num = 0;
	byte attack[128], attack_num = 0;
	byte summon[128], summon_num = 0;
	byte tactic[128], tactic_num = 0;
	byte annoy[128], annoy_num = 0;
	byte invul[128], invul_num = 0;
	byte haste[128], haste_num = 0;
	byte world[128], world_num = 0;
	byte special[128], special_num = 0;
	byte psy_spe[128], psy_spe_num = 0;
//	byte raise[128], raise_num = 0;
	byte heal[128], heal_num = 0;
	byte dispel[128], dispel_num = 0;

	int i;

	/* Stupid monsters choose randomly */
	if (r_ptr->flags2 & (RF2_STUPID))
	{
		/* Pick at random */
		return (spells[randint0(num)]);
	}

	///sys mon f9�����ƃ����X�^�[�X�y���ύX�ɔ��������̉��̊֐����Ȃ�ύX�v
	/* Categorize spells */
	for (i = 0; i < num; i++)
	{
		/* Escape spell? */
		if (spell_escape(spells[i])) escape[escape_num++] = spells[i];

		/* Attack spell? */
		if (spell_attack(spells[i])) attack[attack_num++] = spells[i];

		/* Summon spell? */
		if (spell_summon(spells[i])) summon[summon_num++] = spells[i];

		/* Tactical spell? */
		if (spell_tactic(spells[i])) tactic[tactic_num++] = spells[i];

		/* Annoyance spell? */
		if (spell_annoy(spells[i])) annoy[annoy_num++] = spells[i];

		/* Invulnerability spell? */
		if (spell_invulner(spells[i])) invul[invul_num++] = spells[i];

		/* Haste spell? */
		if (spell_haste(spells[i])) haste[haste_num++] = spells[i];

		/* World spell? */
		if (spell_world(spells[i])) world[world_num++] = spells[i];

		/* Special spell? */
		if (spell_special(spells[i])) special[special_num++] = spells[i];

		/* Psycho-spear spell? */
		if (spell_psy_spe(spells[i])) psy_spe[psy_spe_num++] = spells[i];

		/* Raise-dead spell? */
		//if (spell_raise(spells[i])) raise[raise_num++] = spells[i];

		/* Heal spell? */
		if (spell_heal(spells[i])) heal[heal_num++] = spells[i];

		/* Dispel spell? */
		if (spell_dispel(spells[i])) dispel[dispel_num++] = spells[i];
	}

	/*** Try to pick an appropriate spell type ***/

	/* world */
	if (world_num && (randint0(100) < 15) && !world_monster)
	{
		/* Choose haste spell */
		return (world[randint0(world_num)]);
	}

	/* special */
	if (special_num)
	{
		bool success = FALSE;
		switch(m_ptr->r_idx)
		{
			case MON_BANOR:
			case MON_LUPART:
				if ((m_ptr->hp < m_ptr->maxhp / 2) && r_info[MON_BANOR].max_num && r_info[MON_LUPART].max_num) success = TRUE;
				break;

				//v1.1.36 �����HP���������獂�m���ŕt�r�_���z������
			case MON_KOSUZU:
				if (randint1(m_ptr->hp) < m_ptr->maxhp / 2) success = TRUE;
				break;


			default: break;

		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Still hurt badly, couldn't flee, attempt to heal */
	if (m_ptr->hp < m_ptr->maxhp / 3 && one_in_(2))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Hurt badly or afraid, attempt to flee */
	if (((m_ptr->hp < m_ptr->maxhp / 3) || MON_MONFEAR(m_ptr)) && one_in_(2))
	{
		/* Choose escape spell if possible */
		if (escape_num) return (escape[randint0(escape_num)]);
	}

	/* special */
	///sys mon ���ʂȍs���̐�������
	if (special_num)
	{
		bool success = FALSE;
		switch (m_ptr->r_idx)
		{
			case MON_OHMU:
			case MON_BANOR:
			case MON_LUPART:
				break;
			case MON_MAI:
			case MON_SATONO:
			case MON_BANORLUPART:
				if (randint0(100) < 70) success = TRUE;
				break;
			case MON_ROLENTO:
			case MON_SEIJA:
			case MON_SEIJA_D:
				if (randint0(100) < 40) success = TRUE;
				break;
			default:
				///mod150216 ���ʍs���̔䗦����������̂�50%��33%�Ɍ��炷
				if (randint0(100) < 33) success = TRUE;
				break;
		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Player is close and we have attack spells, blink away */
	if ((distance(py, px, m_ptr->fy, m_ptr->fx) < 4) && (attack_num || (r_ptr->flags6 & RF6_TRAPS)) && (randint0(100) < 75) && !world_monster)
	{
		/* Choose tactical spell */
		if (tactic_num) return (tactic[randint0(tactic_num)]);
	}

	/* Summon if possible (sometimes) */
	if (summon_num && (randint0(100) < 40))
	{
		/* Choose summon spell */
		return (summon[randint0(summon_num)]);
	}

	/* dispel */
	if (dispel_num && one_in_(2))
	{
		/* Choose dispel spell if possible */
		if (dispel_check(m_idx))
		{
			return (dispel[randint0(dispel_num)]);
		}
	}
	///sysdel ���l�Ԃ�
	/* Raise-dead if possible (sometimes) */
#if 0	
	if (raise_num && (randint0(100) < 40))
	{
		/* Choose raise-dead spell */
		return (raise[randint0(raise_num)]);
	}
#endif

	/* Attack spell (most of the time) */
	if (IS_INVULN())
	{
		if (psy_spe_num && (randint0(100) < 50))
		{
			/* Choose attack spell */
			return (psy_spe[randint0(psy_spe_num)]);
		}
		else if (attack_num && (randint0(100) < 40))
		{
			/* Choose attack spell */
			return (attack[randint0(attack_num)]);
		}
	}
	else if (attack_num && (randint0(100) < 85))
	{
		/* Choose attack spell */
		return (attack[randint0(attack_num)]);
	}

	/* Try another tactical spell (sometimes) */
	if (tactic_num && (randint0(100) < 50) && !world_monster)
	{
		/* Choose tactic spell */
		return (tactic[randint0(tactic_num)]);
	}

	/* Cast globe of invulnerability if not already in effect */
	if (invul_num && !m_ptr->mtimed[MTIMED_INVULNER] && (randint0(100) < 50))
	{
		/* Choose Globe of Invulnerability */
		return (invul[randint0(invul_num)]);
	}

	/* We're hurt (not badly), try to heal */
	if ((m_ptr->hp < m_ptr->maxhp * 3 / 4) && (randint0(100) < 25))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Haste self if we aren't already somewhat hasted (rarely) */
	if (haste_num && (randint0(100) < 20) && !MON_FAST(m_ptr))
	{
		/* Choose haste spell */
		return (haste[randint0(haste_num)]);
	}

	/* Annoy player (most of the time) */
	if (annoy_num && (randint0(100) < 80))
	{
		/* Choose annoyance spell */
		return (annoy[randint0(annoy_num)]);
	}

	/* Choose no spell */
	return (0);
}

/*
 * �X�y���ԍ�����񖂖@�X�y���𔻒肷��B
 */
bool spell_is_inate(u16b spell)
{
	if (spell < 32 * 4) /* Set RF4 */
	{
		if ((1L << (spell - 32 * 3)) & RF4_NOMAGIC_MASK) return TRUE;
	}
	else if (spell < 32 * 5) /* Set RF5 */
	{
		if ((1L << (spell - 32 * 4)) & RF5_NOMAGIC_MASK) return TRUE;
	}
	else if (spell < 32 * 6) /* Set RF6 */
	{
		if ((1L << (spell - 32 * 5)) & RF6_NOMAGIC_MASK) return TRUE;
	}
	else if (spell < 32 * 7) /* Set RF9 */
	{
		if ((1L << (spell - 32 * 6)) & RF9_NOMAGIC_MASK) return TRUE;
	}

	/* This spell is not "inate" */
	return FALSE;
}
/*
 * Return TRUE if a spell is inate spell.
 */
bool spell_is_summon(u16b spell)
{
	if (spell < 32 * 4) /* Set RF4 */
	{
		if ((1L << (spell - 32 * 3)) & RF4_SUMMON_MASK) return TRUE;
	}
	else if (spell < 32 * 5) /* Set RF5 */
	{
		if ((1L << (spell - 32 * 4)) & RF5_SUMMON_MASK) return TRUE;
	}
	else if (spell < 32 * 6) /* Set RF6 */
	{
		if ((1L << (spell - 32 * 5)) & RF6_SUMMON_MASK) return TRUE;
	}
	else if (spell < 32 * 7) /* Set RF6 */
	{
		if ((1L << (spell - 32 * 6)) & RF9_SUMMON_MASK) return TRUE;
	}

	/* This spell is not "inate" */
	return FALSE;
}

//���ɒ��ڎː����ʂ�Ȃ��Ă��ׂ̃O���b�h�Ɏː����ʂ邩�ǂ����`�F�b�N���Ă���炵��
static bool adjacent_grid_check(monster_type *m_ptr, int *yp, int *xp,
	int f_flag, bool (*path_check)(int, int, int, int))
{
	int i;
	int tonari;
	static int tonari_y[4][8] = {{-1, -1, -1,  0,  0,  1,  1,  1},
			                     {-1, -1, -1,  0,  0,  1,  1,  1},
			                     { 1,  1,  1,  0,  0, -1, -1, -1},
			                     { 1,  1,  1,  0,  0, -1, -1, -1}};
	static int tonari_x[4][8] = {{-1,  0,  1, -1,  1, -1,  0,  1},
			                     { 1,  0, -1,  1, -1,  1,  0, -1},
			                     {-1,  0,  1, -1,  1, -1,  0,  1},
			                     { 1,  0, -1,  1, -1,  1,  0, -1}};

	if (m_ptr->fy < py && m_ptr->fx < px) tonari = 0;
	else if (m_ptr->fy < py) tonari = 1;
	else if (m_ptr->fx < px) tonari = 2;
	else tonari = 3;

	for (i = 0; i < 8; i++)
	{
		int next_x = *xp + tonari_x[tonari][i];
		int next_y = *yp + tonari_y[tonari][i];
		cave_type *c_ptr;

		/* Access the next grid */
		c_ptr = &cave[next_y][next_x];

		/* Skip this feature */
		if (!cave_have_flag_grid(c_ptr, f_flag)) continue;

		if (path_check(m_ptr->fy, m_ptr->fx, next_y, next_x))
		{
			*yp = next_y;
			*xp = next_x;
			return TRUE;
		}
	}

	return FALSE;
}

#define DO_SPELL_NONE    0
#define DO_SPELL_BR_LITE 1
#define DO_SPELL_BR_DISI 2
#define DO_SPELL_BA_LITE 3
#define DO_SPELL_BA_DISI 4
#define DO_SPELL_TELE_HI_APPROACH 5
#define DO_SPELL_SPECIAL_AZATHOTH 6
#define DO_SPELL_WAVEMOTION 7
#define DO_SPELL_DESTRUCTION 8
#define DO_SPELL_FINALSPARK 9


/*
 * Creatures can cast spells, shoot missiles, and breathe.
 *
 * Returns "TRUE" if a spell (or whatever) was (successfully) cast.
 *
 * XXX XXX XXX This function could use some work, but remember to
 * keep it as optimized as possible, while retaining generic code.
 *
 * Verify the various "blind-ness" checks in the code.
 *
 * XXX XXX XXX Note that several effects should really not be "seen"
 * if the player is blind.  See also "effects.c" for other "mistakes".
 *
 * Perhaps monsters should breathe at locations *near* the player,
 * since this would allow them to inflict "partial" damage.
 *
 * Perhaps smart monsters should decline to use "bolt" spells if
 * there is a monster in the way, unless they wish to kill it.
 *
 * Note that, to allow the use of the "track_target" option at some
 * later time, certain non-optimal things are done in the code below,
 * including explicit checks against the "direct" variable, which is
 * currently always true by the time it is checked, but which should
 * really be set according to an explicit "projectable()" test, and
 * the use of generic "x,y" locations instead of the player location,
 * with those values being initialized with the player location.
 *
 * It will not be possible to "correctly" handle the case in which a
 * monster attempts to attack a location which is thought to contain
 * the player, but which in fact is nowhere near the player, since this
 * might induce all sorts of messages about the attack itself, and about
 * the effects of the attack, which the player might or might not be in
 * a position to observe.  Thus, for simplicity, it is probably best to
 * only allow "faulty" attacks by a monster if one of the important grids
 * (probably the initial or final grid) is in fact in view of the player.
 * It may be necessary to actually prevent spell attacks except when the
 * monster actually has line of sight to the player.  Note that a monster
 * could be left in a bizarre situation after the player ducked behind a
 * pillar and then teleported away, for example.
 *
 * Note that certain spell attacks do not use the "project()" function
 * but "simulate" it via the "direct" variable, which is always at least
 * as restrictive as the "project()" function.  This is necessary to
 * prevent "blindness" attacks and such from bending around walls, etc,
 * and to allow the use of the "track_target" option in the future.
 *
 * Note that this function attempts to optimize the use of spells for the
 * cases in which the monster has no spells, or has spells but cannot use
 * them, or has spells but they will have no "useful" effect.  Note that
 * this function has been an efficiency bottleneck in the past.
 *
 * Note the special "MFLAG_NICE" flag, which prevents a monster from using
 * any spell attacks until the player has had a single chance to move.
 */

/*:::�����X�^�[�����ɑ΂��Ė��@���g������*/
/*:::���@�A�u���X�A�ˌ��Ȃǂ����s������TRUE���Ԃ�(���s�ł�TRUE�@�������Ȃ�������FALSE) */
/*:::���̖Ӗڎ��ɂ̓��b�Z�[�W���ς��*/
/*:::�����X�^�[�́��ׂ̗̃}�X�ɖ��@�������Ƃ�����*/
/*:::SMART�ȃ����X�^�[�͖����̃����X�^�[���Ԃɂ���Ƃ����Ƀ{���g�������Ȃ�*/
/*:::�ڍוs���@���W�`�F�b�N�Ɋւ��ĉ��������Ă�*/
/*:::���𒆐S�łȂ��͈͓��ɑ�����U���ɂ��Ă͊��S�ɂ͎����ł��Ă��Ȃ�*/
/*:::�������̍U��������project()���g�킸�O���[�o���ϐ��ɒ��ڃA�N�Z�X���s���̂Œ���*/
/*:::�����X�^�[���L�p�ȃX�y���������Ȃ��Ƃ��̓���͍œK��ڎw���Ē������Ă��邪�̂͂������{�g���l�b�N�������H*/
///sys mon �����X�^�[�U�����@�t���Of9��ǉ�����K�v�L��
///mod160618 ����special_flag��ǉ��B�P��ɕK���e���|�[�g���g�킹�邽�߂̃t���O
bool make_attack_spell(int m_idx, int special_flag)
{
	int             k, thrown_spell = 0, rlev, failrate;

	///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ���flags9�ǉ�
	byte            spell[128], num = 0;
	u32b            f4, f5, f6, f9;
	//byte            spell[96], num = 0;
	//u32b            f4, f5, f6;
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];
#ifndef JP
	char            m_poss[80];
#endif
	bool            no_inate = FALSE;
	bool            do_spell = DO_SPELL_NONE;
	int             dam = 0;
	u32b mode = 0L;
	int s_num_6 = (easy_band ? 2 : 6);
	int s_num_4 = (easy_band ? 1 : 4);
	int rad = 0; //For elemental spells

	int hecatia_change_idx = 0; //�w�J�[�e�B�A���ꏈ���p
	int rankup_monster_idx = 0; //�������T���ꏈ���p

	/* Target location */
	int x = px;
	int y = py;

	/* Target location for lite breath */
	int x_br_lite = 0;
	int y_br_lite = 0;

	int caster_dist=m_ptr->cdis;//�����X�^�[�����@���g�����u�Ԃ́�����̋���

	/* Summon count */
	int count = 0;

	/* Extract the blind-ness */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Extract the "see-able-ness" */
	bool seen = (!blind && m_ptr->ml);

	bool maneable = player_has_los_bold(m_ptr->fy, m_ptr->fx);
	bool learnable = (seen && maneable && !world_monster);

	///mod151228 Hack - �T�C�o�C�}����p�����@��������Ƃ��ꂪTRUE�ɂȂ�A�֐��I�����O��delete_monster_idx(m_idx)����B
	bool suisidebomb = FALSE;

	/* Check "projectable" */
	bool direct;
	/*:::�����@���A�t���O*/
	bool in_no_magic_dungeon = (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && dun_level
		&& (!p_ptr->inside_quest || is_fixed_quest_idx(p_ptr->inside_quest));

	bool can_use_lite_area = FALSE;

	bool can_remember;

	//�E�Ƃʂ��̂Ƃ��G�����̕s�������j��m���̌W��(%) ���ɂȂ�ƕK�����j����
	int nue_check_mult = 100;

	//v1.1.95 ����m����Ԃ��Ɩ��@�����s�����g���Ȃ�
	if (MON_BERSERK(m_ptr))
	{
		return (FALSE);
	}

	/* Cannot cast spells when confused */
	if (MON_CONFUSED(m_ptr))
	{
		reset_target(m_ptr);
		return (FALSE);
	}

	//�}�~�]�E��b��Ƃ̍ق�
	if(p_ptr->pclass == CLASS_MAMIZOU && p_ptr->magic_num1[0] == m_idx)
		return (FALSE);

	/* Cannot cast spells when nice */
	if (m_ptr->mflag & MFLAG_NICE) return (FALSE);
	if (!is_hostile(m_ptr) && !special_flag) return (FALSE);

	///mod141005 �H�X�q�́u���o�̗U�铔�v
	//v1.1.76 �L�q�ύX ���E���G�΃����X�^�[�Ɋm���Ŗ��@�g�p�֎~����
	//if (p_ptr->pclass == CLASS_YUYUKO && m_ptr->mflag & MFLAG_SPECIAL) return (FALSE);
	if (p_ptr->pclass == CLASS_YUYUKO && p_ptr->tim_general[0]	&& !is_pet(m_ptr) && los(py, px, m_ptr->fy, m_ptr->fx))
	{

		int chance = p_ptr->lev * 3 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) chance /= 2;
		if (r_ptr->level < randint1(chance))
		{
			monster_desc(m_name, m_ptr, 0);
			msg_format("%s�͌��ɗU���Ă���...", m_name);
			return (FALSE);
		}
	}


	/*:::innate(������) �u���X�A���тȂǓ��̓I�Ȗ��@�̐����@���@�g�p���̒Ⴂ�����X�^�[�͖��@�g�p���Ƀu���X�Ȃǂ��g��Ȃ����Ƃ������炵������|���悭�����*/
	/* Sometimes forbid inate attacks (breaths) */
	if (randint0(100) >= (r_ptr->freq_spell * 2)) no_inate = TRUE;

	/* XXX XXX XXX Handle "track_target" option (?) */


	/* Extract the racial spell flags */
	/*:::�����X�^�[�̎����Ă���t���O*/
	f4 = r_ptr->flags4;
	f5 = r_ptr->flags5;
	f6 = r_ptr->flags6;
	f9 = r_ptr->flags9;

	/*** require projectable player ***/

	/* Check range */
	/*:::MAX_RANGE:18 ���������ɂ��đ��Ƀ^�[�Q�b�g�����Ȃ���Ή������Ȃ�*/

	///mod140103 ���E�O�e���|�[�g�����G�́��������Ă��܂��I���Ȃ��悤�ɂ����B�o�O�̂��ƂɂȂ肻���ȋC������B
	if (!(f9 & RF9_TELE_HI_APPROACH) && (m_ptr->cdis > MAX_RANGE) && !m_ptr->target_y) return (FALSE);
	//if ((m_ptr->cdis > MAX_RANGE) && !m_ptr->target_y) return (FALSE);


	/* Extract the monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

	//v1.1.94 �����X�^�[���@�͒ቺ���̓��x������l25%�J�b�g
	if (MON_DEC_MAG(m_ptr))
	{
		rlev = (rlev * 3 + 1) / 4;
	}

	/*:::�M���A�����u���X�̓���D�揈��*/
	///system �M���u���X�̗D��g�p�����@�M���������[�U�[�Ƃ�����������ǉ����ׂ����낤
	/* Check path for lite breath */
	if (f4 & RF4_BR_LITE)
	{
		y_br_lite = y;
		x_br_lite = x;

		if (los(m_ptr->fy, m_ptr->fx, y_br_lite, x_br_lite))
		{
			feature_type *f_ptr = &f_info[cave[y_br_lite][x_br_lite].feat];

			if (!have_flag(f_ptr->flags, FF_LOS))
			{
				if (have_flag(f_ptr->flags, FF_PROJECT) && one_in_(2)) f4 &= ~(RF4_BR_LITE);
			}
		}

		/* Check path to next grid */
		else if (!adjacent_grid_check(m_ptr, &y_br_lite, &x_br_lite, FF_LOS, los)) f4 &= ~(RF4_BR_LITE);

		/* Don't breath lite to the wall if impossible */
		if (!(f4 & RF4_BR_LITE))
		{
			y_br_lite = 0;
			x_br_lite = 0;
		}
	}

	///mod140613 ���|���Ă���G�́��ւ̗אڃe���|���g��Ȃ�
	if (MON_MONFEAR(m_ptr))
	{
		f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
	}

	//v1.2.00 ���т́��̋߂��ɐ����Ȃ��Ǝ��E�O�אڃe���|���g��Ȃ�
	if (m_ptr->r_idx == MON_MURASA)
	{
		bool find_water = FALSE;
		int j;

		for (j = 1; j <= 9; j++)
		{
			int tmp_x, tmp_y;

			tmp_x = px + ddx[j];
			tmp_y = py + ddy[j];

			if (!in_bounds(tmp_y, tmp_x)) continue;
			if (cave_have_flag_bold(tmp_y, tmp_x, FF_WATER))
			{
				find_water = TRUE;
				break;
			}
		}

		if (!find_water)
		{
			f9 &= ~(RF9_TELE_HI_APPROACH);
		}
	}

	//�z����*�j��*�⎞�Ԓ�~���g��Ȃ�
	if(is_pet(m_ptr))
	{
		f9 &= ~(RF9_DESTRUCTION);
		f6 &= ~(RF6_WORLD);
	}

	///mod140115 ���ɗאڂ����G�͗אڃe���|���g��Ȃ��悤�ɂ���
	if ((f9 & (RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH)) && m_ptr->cdis < 2)
	{
		f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
	}
	///mod140714 �����痣�ꂽ�G�̓��C���V���g�������g��Ȃ��悤�ɂ��� v1.1.64 7��4
	if ((f9 & RF9_MAELSTROM) && m_ptr->cdis > 4 && !(r_ptr->flags2 & RF2_STUPID) )
	{
		f9 &= ~(RF9_MAELSTROM);
	}
	///����vault���ɋ���Ƃ��͎��E�O�אڃe���|�Ŕ��ł��Ȃ��悤�ɂ���
	if (f9 & RF9_TELE_HI_APPROACH)
	{
		cave_type    *c_ptr = &cave[y][x];
		if(c_ptr->info & CAVE_ICKY)	f9 &= ~(RF9_TELE_HI_APPROACH);
	}
	//���i�`���Î�
	if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] && m_ptr->cdis <= p_ptr->lev / 5 || p_ptr->tim_general[1]))
	{
		f4 &= (RF4_NOMAGIC_MASK);
		f4 &= ~(RF4_SHRIEK | RF4_BR_SOUN);
		f5 &= (RF5_NOMAGIC_MASK);
		f6 &= (RF6_NOMAGIC_MASK);
		f9 &= (RF9_NOMAGIC_MASK);
		if(m_ptr->r_idx == MON_REIMU || m_ptr->r_idx == MON_MICHAEL) f6 &= ~(RF6_SPECIAL);
	}
	//�w�J�[�e�B�A���ꏈ���@���̑̂��HP�������|����ĂȂ��̂�I�� ���Ȃ���Γ��ʍs�����Ȃ�
	if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
	{
		int i;
		int current_hp = m_ptr->hp;
		for(i=0;i<3;i++)
		{
			if(m_ptr->r_idx ==(MON_HECATIA1+i)) continue;
			if(r_info[MON_HECATIA1+i].r_akills) continue;
			if((int)hecatia_hp[i] < current_hp) continue;
			current_hp = hecatia_hp[i];
			hecatia_change_idx = MON_HECATIA1+i;

		}
		if(!hecatia_change_idx) f4 &= ~(RF4_SPECIAL2);
		//�s���`�̂Ƃ����m���Ō�シ��
		else if(m_ptr->hp < 2000 && one_in_(2))
		{
			f4 = RF4_SPECIAL2;
			f5 = 0L;
			f6 = 0L;
			f9 = 0L;
		}
	}
	//�T�C�o�C�}���̓^�[�Q�b�g���߂��ɂ��đ̗͂��Ⴍ�Ȃ��Ɠ��ʍs�������Ȃ�
	if(m_ptr->r_idx == MON_SAIBAIMAN)
	{
		if(m_ptr->hp > m_ptr->maxhp / 3 || m_ptr->cdis > 1)
			f4 &= ~(RF4_SPECIAL2);
	}

	//���Ƀ����_�����j�[�N2���t���A�ɂ���ꍇ�B��ނ͓��ʍs��2�����Ȃ�
	if (m_ptr->r_idx == MON_OKINA && r_info[MON_RANDOM_UNIQUE_2].cur_num)
	{
		if (cheat_hear) msg_print("msg:���łɃ����_�����j�[�N���t���A�ɂ���̂ŉB��ނ̓��ʍs�����}�~���ꂽ");
		f4 &= ~(RF4_SPECIAL2);
	}

	//v1.1.91 �ޖ���HP�������ĂĎ��͂ɐΖ��n�`������Ƃ��ɂ������ʍs�������Ȃ��B���Ɠ��Z���A���[�i�ł͓��ʍs�������Ȃ��B
	if (m_ptr->r_idx == MON_YUMA)
	{
		if (p_ptr->inside_arena || p_ptr->inside_battle || (m_ptr->hp > m_ptr->maxhp * 3 / 4) || yuma_vacuum_oil(m_ptr->fy,m_ptr->fx,TRUE,0) < 1)
		{
			f4 &= ~(RF4_SPECIAL2);
		}
	}

	//v1.1.36 ����͕t�r�_���t���A�ɂ��邩�_���[�W���󂯂Ă��Ȃ��Ɠ��ʍs�������Ȃ�
	if(m_ptr->r_idx == MON_KOSUZU)
	{
		int j;
		bool no_use = FALSE;

		if(m_ptr->hp == m_ptr->maxhp) 
			no_use = TRUE;
		else
		{
			for (j = 1; j < m_max; j++)
			{
				monster_type *tmp_m_ptr = &m_list[j];
				if (tmp_m_ptr->r_idx != MON_TSUKUMO_1 && tmp_m_ptr->r_idx != MON_TSUKUMO_2 ) continue;
				break;
			}
			if(j == m_max) no_use = TRUE;
		}

		if(no_use)
			f4 &= ~(RF4_SPECIAL2);
	}


	/* Check path */

	//���ꃂ�[�h�@�K���אڃe���|
	if(special_flag == 1)
	{
			do_spell = DO_SPELL_TELE_HI_APPROACH;
			
	}
	//���ƃ����X�^�[�̊ԂɎː����ʂ��Ă���Ƃ�
	else if (projectable(m_ptr->fy, m_ptr->fx, y, x))
	{
		feature_type *f_ptr = &f_info[cave[y][x].feat];

		//�������E���̕ǂ�K���X�̕ǂ̒��ɂ���Ƃ������Ȃǌ������Ȃ��U����1/2�őI������炵��
		if (!have_flag(f_ptr->flags, FF_PROJECT))
		{
			/* Breath disintegration to the wall if possible */
			if ((f4 & RF4_BR_DISI) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(2)) do_spell = DO_SPELL_BR_DISI;
			///mod140103 ���q������ǂ̒��́��Ɍ����Ďg���₷������
			if ((f9 & RF9_BA_DISI) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(2)) do_spell = DO_SPELL_BA_DISI;

			if ((f9 & RF9_FINALSPARK) && have_flag(f_ptr->flags, FF_HURT_DISI) && one_in_(3)) do_spell = DO_SPELL_FINALSPARK;

			/* Breath lite to the transparent wall if possible */
			else if ((f4 & RF4_BR_LITE) && have_flag(f_ptr->flags, FF_LOS) && one_in_(2)) do_spell = DO_SPELL_BR_LITE;
		}
	}
	//���ƃ����X�^�[�̊ԂɎː����ʂ��Ă��Ȃ��Ƃ�
	/* Check path to next grid */
	else
	{
		bool success = FALSE;

		//���E�O�אڃe���|�p�̒萔
		int dev;
		if(difficulty == DIFFICULTY_EASY) dev = 10;
		else if(difficulty == DIFFICULTY_NORMAL) dev = 5;
		else if(difficulty == DIFFICULTY_LUNATIC) dev = 1;
		else dev = 3;


		/*:::���E�O���番���u���X�Ƃ��f���̂͂����炵��*/
		///system ������j��Ƃ������{�[���Ƃ����������炱���ɒǉ�
		///���E�O��20-5���炢�̋����Ŏ���������⋩�т��g���悤�ɂ��Ă������BSMART��30%�ȉ���1/5�ASTUPID�ȊO��15%�ȉ���1/10�̊m���ōs�����x��������
		if ((f4 & RF4_BR_DISI) && (m_ptr->cdis < MAX_RANGE/2) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(10) || (projectable(y, x, m_ptr->fy, m_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_BR_DISI;
			success = TRUE;
		}

		if ((f4 & RF4_WAVEMOTION) && (m_ptr->cdis < MAX_RANGE/2) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(10)))
		{
			do_spell = DO_SPELL_WAVEMOTION;
			success = TRUE;
		}
		else if ((f9 & RF9_FINALSPARK) && (m_ptr->cdis < MAX_RANGE/2) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(10) || (projectable(y, x, m_ptr->fy, m_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_FINALSPARK;
			success = TRUE;
		}
		///mod140103 ���q�����̎��E�O�U������������
		else if ((f9 & RF9_BA_DISI) && (m_ptr->cdis < 5) &&
		    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
		    (one_in_(7) || (projectable(y, x, m_ptr->fy, m_ptr->fx) && one_in_(2))))
		{
			do_spell = DO_SPELL_BA_DISI;
			success = TRUE;
		}
		///���E�O�אڃe���|�[�g���������Ă݂��@���x��*2�̋�������Ȃ���ł���i���܂藣�ꂽ�G�͍s�����肪�Ȃ����������邪�j
		///mod160326 ������Ǝ˒������Z������
		//else if ((f9 & RF9_TELE_HI_APPROACH) && (m_ptr->cdis < rlev * 2) && one_in_(10))
		else if ((f9 & RF9_TELE_HI_APPROACH) && (m_ptr->cdis < (r_ptr->aaf / dev)) && one_in_(10))
		{
			do_spell = DO_SPELL_TELE_HI_APPROACH;
			success = TRUE;
		}
		///�A�U�g�[�g�̓���U��
		else if(m_ptr->r_idx == MON_AZATHOTH && m_ptr->cdis < 10 && one_in_(6))
		{
			do_spell = DO_SPELL_SPECIAL_AZATHOTH;
			success = TRUE;
		}



		else if ((f4 & RF4_BR_LITE) && (m_ptr->cdis < MAX_RANGE/2) &&
		    los(m_ptr->fy, m_ptr->fx, y, x) && one_in_(5))
		{
			do_spell = DO_SPELL_BR_LITE;
			success = TRUE;
		}
		else if ((f5 & RF5_BA_LITE) && (m_ptr->cdis <= MAX_RANGE))
		{
			int by = y, bx = x;
			get_project_point(m_ptr->fy, m_ptr->fx, &by, &bx, 0L);
			if ((distance(by, bx, y, x) <= 3) && los(by, bx, y, x) && one_in_(5))
			{
				do_spell = DO_SPELL_BA_LITE;
				success = TRUE;
			}
		}

		//���ƃ����X�^�[�Ƃ̊ԂɎ��E���ʂ��Ă��炸�A�����ȂǑ��Ɏg������Z���Ȃ��Ƃ��A���ׂ̗̃O���b�h�Ɏː����ʂ��Ă��邩�`�F�b�N���ʂ��Ă��x,y��ύX
		//if (!success && p_ptr->wizard) msg_print("no success");
		if (!success) success = adjacent_grid_check(m_ptr, &y, &x, FF_PROJECT, projectable);

		//����ł��ː����ʂ��ĂȂ���Ώ�����e���|�Ȃǈꕔ�̓��Z�̂ݎg���@�����锽������������
		if (!success)
		{

			if (m_ptr->target_y && m_ptr->target_x)
			{
				y = m_ptr->target_y;
				x = m_ptr->target_x;
				f4 &= (RF4_INDIRECT_MASK);
				f5 &= (RF5_INDIRECT_MASK);
				f6 &= (RF6_INDIRECT_MASK);
				f9 &= (RF9_INDIRECT_MASK);
				success = TRUE;
			}

			if (y_br_lite && x_br_lite && (m_ptr->cdis < MAX_RANGE/2) && one_in_(5))
			{
				if (!success)
				{
					y = y_br_lite;
					x = x_br_lite;
					do_spell = DO_SPELL_BR_LITE;
					success = TRUE;
				}
				else f4 |= (RF4_BR_LITE);
			}
		}

		/* No spells */
		if (!success) return FALSE;
	}


	/*::: Hack- �j��g�p���菈�� */
	if (r_ptr->flags2 & RF2_SMART && (f9 & RF9_DESTRUCTION))
	{
		int j;
		int mcount = 0;
		for (j = 1; j < m_max; j++)
		{
			monster_race *r_ptr_tmp = &r_info[m_list[j].r_idx]; 
			if(distance( m_ptr->fy, m_ptr->fx,m_list[j].fy,m_list[j].fx) < 10 && r_ptr_tmp->level > r_ptr->level / 2 && are_enemies(m_ptr,&m_list[j])) mcount++;
		}
		//msg_format("CHECK:cnt=%d ",mcount);
		if(mcount > randint0(10) )
		{
			do_spell = DO_SPELL_DESTRUCTION;
		}
	}

	reset_target(m_ptr);

	/* Forbid inate attacks sometimes */
	/*:::�u���X�A���сA�ˌ��A���P�b�g�A���ʂȍs�����Y������Bno_inate�̂Ƃ��������g��Ȃ�*/
	if (no_inate)
	{
		f4 &= ~(RF4_NOMAGIC_MASK);
		f5 &= ~(RF5_NOMAGIC_MASK);
		f6 &= ~(RF6_NOMAGIC_MASK);
		f9 &= ~(RF9_NOMAGIC_MASK);
	}
	///class �E�ғ��ꏈ��
	if (f6 & RF6_DARKNESS)
	{
		if ((p_ptr->pclass == CLASS_NINJA || p_ptr->pclass == CLASS_NUE) &&
		    !(r_ptr->flags3 & RF3_UNDEAD) &&
///mod131231 �����X�^�[�t���O�ύX �M����_RF3����RFR��
			!(r_ptr->flagsr & RFR_HURT_LITE) &&
		    !(r_ptr->flags7 & RF7_DARK_MASK))
			can_use_lite_area = TRUE;

		/*:::�����E�҂̂Ƃ��A�Èł̖��@���g���G������Ƀ��C�g�G���A���g�����Ƃ�����炵��*/
		if (!(r_ptr->flags2 & RF2_STUPID))
		{
			if (d_info[dungeon_type].flags1 & DF1_DARKNESS) f6 &= ~(RF6_DARKNESS);
			else if ((p_ptr->pclass == CLASS_NINJA || p_ptr->pclass == CLASS_NUE) && !can_use_lite_area) f6 &= ~(RF6_DARKNESS);
		}
	}
	/*:::�����@���A�ł͖��@���g��Ȃ��iSTUPID�͎g���j*/
	///mod140423 �u�G��ق点��v���ʒ������l�ɏ���
	///mod150604 �f�P����𕕂����G���ǉ�
	if ((in_no_magic_dungeon || p_ptr->silent_floor || (m_ptr->mflag & MFLAG_NO_SPELL)) && !(r_ptr->flags2 & RF2_STUPID))
	{
		f4 &= (RF4_NOMAGIC_MASK);
		f5 &= (RF5_NOMAGIC_MASK);
		f6 &= (RF6_NOMAGIC_MASK);
		f9 &= (RF9_NOMAGIC_MASK);
		if(m_ptr->r_idx == MON_REIMU || m_ptr->r_idx == MON_MICHAEL) f6 &= ~(RF6_SPECIAL);
	}

	//v1.1.36 �G�^�j�e�B�����o�͋��|���ɂ������L�_���g��Ȃ��悤�ɂ���
	if(m_ptr->r_idx == MON_LARVA)
	{
		if (!MON_MONFEAR(m_ptr))
		{
			f5 &= ~(RF5_BA_POIS);
		}
	}

	/*:::SMART�ȓG�̓s���`�̂Ƃ��U�����@���e���|�[�g�⎡���⏢����o�X�e�𑽗p����悤�ɂȂ�*/
	///sys SMART�ȓG���s���`�̂Ƃ��̍s���I��@�����ƍׂ��������悤
	if (r_ptr->flags2 & RF2_SMART)
	{
		/* Hack -- allow "desperate" spells */
		if ((m_ptr->hp < m_ptr->maxhp / 10) &&
			(randint0(100) < 50))
		{
			/* Require intelligent spells */
			f4 &= (RF4_INT_MASK);
			f5 &= (RF5_INT_MASK);
			f6 &= (RF6_INT_MASK);
			f9 &= (RF9_INT_MASK);

			//v1.1.91 �ޖ��̓��ʍs���ƃ��C���V���g�����͉񕜂����˂Ă���̂ŏȂ��Ȃ�
			if (m_ptr->r_idx == MON_YUMA)
			{
				f4 |= RF4_SPECIAL2;
				f9 |= RF9_MAELSTROM;
			}

		}

		/* Hack -- decline "teleport level" in some case */
		if ((f6 & RF6_TELE_LEVEL) && TELE_LEVEL_IS_INEFF(0))
		{
			f6 &= ~(RF6_TELE_LEVEL);
		}
	}





	/* No spells left */
	if (!f4 && !f5 && !f6 && !f9) return (FALSE);


	/* Remove the "ineffective" spells */
	/*:::�����w�K�ɔ������ʂ̒Ⴂ������e������*/
	remove_bad_spells(m_idx, &f4, &f5, &f6, &f9);

	//�A���[�i�Ɠ��Z��ł͏������g��Ȃ��B
	//v1.1.43 �����ŋ��C��ԂɂȂ�Ə������g��Ȃ����Ƃɂ��Ă���
	if (p_ptr->inside_arena || p_ptr->inside_battle 
		|| m_ptr->mflag & MFLAG_LUNATIC_TORCH)
	{
		f4 &= ~(RF4_SUMMON_MASK);
		f5 &= ~(RF5_SUMMON_MASK);
		f6 &= ~(RF6_SUMMON_MASK | RF6_TELE_LEVEL);
		f9 &= ~(RF9_SUMMON_MASK);

		if (m_ptr->r_idx == MON_FLAN) f6 &= ~(RF6_SPECIAL);
		if (m_ptr->r_idx == MON_ROLENTO) f6 &= ~(RF6_SPECIAL);
		if (m_ptr->r_idx == MON_OKINA) f4 &= ~(RF4_SPECIAL2);
		if (m_ptr->r_idx == MON_SATONO || m_ptr->r_idx == MON_MAI) f6 &= ~(RF6_SPECIAL); //v1.1.32
	}
	//�z�K�q�������͑����̍U�����@���g���Ȃ�
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
	{
		f4 &= (RF4_SHRIEK | RF4_WAVEMOTION | RF4_BR_DISI);
		f5 = 0L;
		f6 &= (RF6_HASTE | RF6_HEAL | RF6_BLINK | RF6_TPORT | RF6_PSY_SPEAR | RF6_DARKNESS | RF6_TRAPS | RF6_SUMMON_MASK);
		f9 &= (RF9_DESTRUCTION | RF9_BA_DISI | RF9_SUMMON_MASK | RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH | RF9_ALARM);

	}

	/* No spells left */
	if (!f4 && !f5 && !f6 && !f9) return (FALSE);

	/*:::STUPID�łȂ��Ƃ��A���ɖ��͂��Ȃ��Ƃ����͋z�����g��Ȃ��A�����ɓ�����{���g�������Ȃ��A���ʂȏ��������Ȃ�*/
	if (!(r_ptr->flags2 & RF2_STUPID))
	{
		if (!p_ptr->csp) f5 &= ~(RF5_DRAIN_MANA);

		/* Check for a clean bolt shot */
		if (((f4 & RF4_BOLT_MASK) ||
		     (f5 & RF5_BOLT_MASK) ||
		     (f6 & RF6_BOLT_MASK) ||
		     (f9 & RF9_BOLT_MASK) ) &&
		    !clean_shot(m_ptr->fy, m_ptr->fx, py, px, FALSE))
		{
			/* Remove spells that will only hurt friends */
			f4 &= ~(RF4_BOLT_MASK);
			f5 &= ~(RF5_BOLT_MASK);
			f6 &= ~(RF6_BOLT_MASK);
			f9 &= ~(RF9_BOLT_MASK);
		}

		/* Check for a possible summon */
		if (((f4 & RF4_SUMMON_MASK) ||
		     (f5 & RF5_SUMMON_MASK) ||
		     (f6 & RF6_SUMMON_MASK) ||
		     (f9 & RF9_SUMMON_MASK)) &&
		    !(summon_possible(y, x)))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}

		//v1.1.13 �����X�^�[��SMART�ȂƂ��u���]������㩁v���L�����ƂقƂ�Ǐ��������݂Ȃ�
		//v1.1.54 �l���m���Z�ǉ�
		/* Check for a possible summon */
		if ((r_ptr->flags2 & RF2_SMART) 
			&&( (CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO))|| (p_ptr->special_flags & SPF_ORBITAL_TRAP)) 
			&&((f4 & RF4_SUMMON_MASK) ||(f5 & RF5_SUMMON_MASK) ||(f6 & RF6_SUMMON_MASK) ||(f9 & RF9_SUMMON_MASK))
			&& !one_in_(4))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}

		///del131221 ���l�Ԃ�
		/* Check for a possible raise dead */
#if 0
		if ((f6 & RF6_RAISE_DEAD) && !raise_possible(m_ptr))
		{
			/* Remove raise dead spell */
			f6 &= ~(RF6_RAISE_DEAD);
		}
#endif
//		f6 &= ~(RF6_RAISE_DEAD);
		/* Special moves restriction */
		///sys RF6�̓���s���̎g�p����
		if (f6 & RF6_SPECIAL)
		{
			if ((m_ptr->r_idx == MON_ROLENTO) && !summon_possible(y, x))
			{
				f6 &= ~(RF6_SPECIAL);
			}
			/*:::�t�������ꏈ���@���g�����ɋ���Ƃ��̓t�H�[�I�u�A�J�C���h���g��Ȃ�*/
			if(m_ptr->r_idx == MON_FLAN)
			{
				int j;
				for (j = 1; j < m_max; j++) if(m_list[j].r_idx == MON_FLAN_4) f6 &= ~(RF6_SPECIAL);
			}
			///mod140626 �얲�͕Ǎۂł͖��z������g���Ȃ�
			if(m_ptr->r_idx == MON_REIMU)
			{
				if (!in_bounds2(m_ptr->fy + 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy + 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy - 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy - 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx + 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx + 1],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx - 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx - 1],FF_PROJECT))
				f6 &= ~(RF6_SPECIAL);
			}

			//v1.1.32 �������T�̓��ʍs���@�����̎��͂Ƀ����N�A�b�v�\�ȃ����X�^�[�����邩�ǂ������肵�A���Ȃ���Γ��ʍs������
			if (m_ptr->r_idx == MON_SATONO || m_ptr->r_idx == MON_MAI)
			{
				rankup_monster_idx = can_rankup_monster(m_ptr);

				if(!rankup_monster_idx)
					f6 &= ~(RF6_SPECIAL);
			}

			if(m_ptr->r_idx == MON_OKINA)
			{
				if(p_ptr->word_recall || p_ptr->inside_arena)
					f6 &= ~(RF6_SPECIAL);

			}

			//v1.1.78 �����́��ɗאڂ��Ă��Ȃ��Ɠ��ʍs�������Ȃ�
			if (m_ptr->r_idx == MON_MIYOI)
			{
				if (m_ptr->cdis > 1) f6 &= ~(RF6_SPECIAL);

			}

		}

		/* No spells left */
		if (!f4 && !f5 && !f6 && !f9) return (FALSE);
	}


	/*:::����܂Ő��ڂ����t���O��spell[]�ɓW�J*/
	/* Extract the "inate" spells */
	for (k = 0; k < 32; k++)
	{
		if (f4 & (1L << k)) spell[num++] = k + 32 * 3;
	}

	/* Extract the "normal" spells */
	for (k = 0; k < 32; k++)
	{
		if (f5 & (1L << k)) spell[num++] = k + 32 * 4;
	}

	/* Extract the "bizarre" spells */
	for (k = 0; k < 32; k++)
	{
		if (f6 & (1L << k)) spell[num++] = k + 32 * 5;
	}
	/*:::�V�����ǉ����������X�^�[�X�y�� */
	for (k = 0; k < 32; k++)
	{
		if (f9 & (1L << k)) spell[num++] = k + 32 * 6;
	}

	/*:::������f9flag����������ɂ�32*6�ł����̂��H���Ȃ��Ǝv�����Ȃ�ŕ��ʂ�0����n�߂Ȃ��񂾂낤*/
	/*:::flags1��0�Ƃ����l�ɑ����Ă���炵���B�Ђ���Ƃ��Ăǂ����ŉ����t���O�S�̂ɂ�����鏈���Ɏg���Ă���̂��H*/



	/* No spells left */
	if (!num) return (FALSE);

	/* Stop if player is dead or gone */
	if (!p_ptr->playing || p_ptr->is_dead) return (FALSE);

	/* Stop if player is leaving */
	if (p_ptr->leaving) return (FALSE);

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0x00);

	/* Projectable? */
	direct = player_bold(y, x);


#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif
/*
	//v1.1.85 �ː����ʂ��Ă��邪���E���Ղ��Ă���(�V�n�`�u�[�����@����v�Ȃ�)�Ƃ��ASMART�łȂ������X�^�[�͖��@�������Ă��Ȃ����Ƃɂ���H
	//�אڂ��Ă�ƌ��B���łɕ����Ȃǌ����@�����܂��Ă�Ƃ������B
	//���̏����̏ꍇ�A�K���X�̕ǂƐ[��ᏋC�ɋ��܂�Ă���ƃK���X�̕ǂ̌���������M���u���X�������ꂽ�肷��B���P�v�B
	if (!(r_ptr->flags2 & RF2_SMART) && !do_spell )
	{
	(�ǋL)
	����2�̂Ƃ��G�����@���������茂���Ȃ������肷��B
	�ŏ�����ː����ʂ��Ă�Ƃ����Ō����Ȃ��Ȃ�̂����A�ː�����O��Ă�Ɓu���ׂ̗̃O���b�h�Ƀ^�[�Q�b�g�����炷�v��������ɗ��ă^�[�Q�b�g�ƃ����X�^�[���אڂ��邹���B
	�^�[�Q�b�g�ƃ����X�^�[���אڂ��Ă��LOS�t���O���Ȃ��Ă������Ă��鈵���ɂȂ�炵���B
	�ǂ�����Ζ����̂Ȃ������ɂȂ�H�ׂ̃O���b�h�Ƀ^�[�Q�b�g�����炷�������̂ɓ����悤�ȏ���������ׂ����B

	�ʂɂ���ȏ����������E���Ղ��ĂĂ��ː����ʂ��Ă�΃o�J�X�J�����Ă��Ă������񂾂��A�X�ѐ����Ƃ̃C���[�W�̐������Y�܂����B
	�X�ѐ����͎��E���Ղ��Ė��@��h���Ǝv���Ă邪�A�����I�ɂ́u�����Ȃ����猂���Ȃ��v����Ȃ��āu�����Ă��؂ɓ������ē͂��Ȃ����猂���Ȃ��v�Ɣ��f���Ă���̂Ŗ��̌��������猂���Ă��Ă��ʂɖ����͂Ȃ��񂾂��B
	�����Ă��v������̂Ō�񂵂ɂ���B������������Ď���������Amonst-monst�̊֐��������悤�ɏ������邱��

	(�ǋL2)
	���ۂɎ��E���Ղ���������Ă݂����A���A�⒆�w�S���ӂ�ł��������̗v���ɂȂ肻���B
	�V���{����~����#�ɂ������ʂ����邳���������ɏ����E���v�f��ǉ�����̂����Ȃ̂Ō��ǎ��E���Ղ鏈�����̂𓖖ʂ�߂Ƃ����Ƃɂ����B
	������u�����[���đO�������Ȃ��v�݂����ȏꏊ�������ŏo����V�_���W�����Ƃ��Ŏ������悤�B


		if (direct && !los(m_ptr->fy, m_ptr->fx, y, x) || !direct && cave_have_flag_bold(y,x,FF_LOS))
		{
			if(p_ptr->wizard)msg_format("���E���Ղ��Ă��邽�ߖ��@�������Ȃ�(����%d)",m_ptr->cdis);
			return (FALSE);
		}

	}
	*/

	/*:::�ŏ��̂ق��ŕ����u���X��M���u���X���g���ƌ��܂����炻�̃C���f�b�N�X���A�����łȂ����spell[]����I��*/
	switch (do_spell)
	{
	case DO_SPELL_NONE:
		{
			int attempt = 10;
			while (attempt--)
			{
				thrown_spell = choose_attack_spell(m_idx, spell, num);
				if (thrown_spell) break;
			}
		}
		break;

		///sys �����u���X�Ȃǂ̓��ꏈ�������ɂ�
	case DO_SPELL_BR_LITE:
		thrown_spell = 96+14; /* RF4_BR_LITE */
		break;

	case DO_SPELL_BR_DISI:
		thrown_spell = 96+31; /* RF4_BR_DISI */
		break;

	case DO_SPELL_BA_LITE:
		thrown_spell = 128+20; /* RF5_BA_LITE */
		break;

	///mod140626 �t�@�C�i���X�p�[�N���ꏈ������
	case DO_SPELL_FINALSPARK:
		thrown_spell = 192+22; /* RF9_FINALSPARK */
		break;
	///mod140103 ���q������ꏈ������
	case DO_SPELL_BA_DISI:
		thrown_spell = 192+6; /* RF9_BA_DISI */
		break;
	///mod140103 ���E�O�אڃe���|�[�g���ꏈ������
	case DO_SPELL_TELE_HI_APPROACH:
		thrown_spell = 192+29; /* RF9_BA_DISI */
		break;
	case DO_SPELL_SPECIAL_AZATHOTH:
		thrown_spell = 96+28; /* RF4_SPECIAL2 */
		break;
	case DO_SPELL_WAVEMOTION:
		thrown_spell = 96+16; /* RF4_WAVEMOTION */
		break;
	case DO_SPELL_DESTRUCTION:
		thrown_spell = 192+24; /* RF9_DESTRUCTION */
		break;


	default:
		return FALSE; /* Paranoia */
	}

	/* Abort if no spell was chosen */
	if (!thrown_spell) return (FALSE);

	/* Calculate spell failure rate */
	/*:::���@���s���v�Z�@STUPID�͎��s���Ȃ�*/
	///system ���s���v�Z�͏����ς��Ă���������
	failrate = 25 - (rlev + 3) / 4;

	/* Hack -- Stupid monsters will never fail (for jellies and such) */
	if (r_ptr->flags2 & RF2_STUPID) failrate = 0;

	//v1.1.94 ���@�͒ቺ���͂���Ɏ��s��25%�㏸
	if (MON_DEC_MAG(m_ptr)) failrate += 25;

	//v1.1.96 ���|���͂���Ɏ��s��25%�㏸
	if (MON_MONFEAR(m_ptr)) failrate += 25;

	//v1.1.46 �����u�v���b�N�s�W�����v�ɂ�閂�@���s���㏸
	failrate += pluck_pigeon_magic_fail(m_ptr);

	//v1.1.48
	if (SUPER_SHION && failrate < 50) failrate = 50;

	if (cheat_xtra) msg_format("failrate:%d%%",failrate);

	/* Check for spell failure (inate attacks never fail) */
	/*:::���s���`�F�b�N�@�N�O�Ƃ��Ă��1/2�Ŏ��s*/
	///mod150604 �f�P����𕕂����G�̏����ǉ�
	if (!spell_is_inate(thrown_spell) && !special_flag
	    && (in_no_magic_dungeon || (MON_STUNNED(m_ptr) && one_in_(2)) || (randint0(100) < failrate) || (m_ptr->mflag & MFLAG_NO_SPELL)))
	{
		disturb(1, 1);
		/* Message */
#ifdef JP
		msg_format("%^s�͎����������悤�Ƃ��������s�����B", m_name);
#else
		msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif

		return (TRUE);
	}
	//���������E�n�b�s�[���C�u
	if(p_ptr->pclass == CLASS_MERLIN && !spell_is_inate(thrown_spell) &&  music_singing(MUSIC_NEW_MERLIN_SOLO) && distance(py,px,m_ptr->fy,m_ptr->fx) < MAX_RANGE)
	{
		//v1.1.65 �O�����Z�q�̗D�揇�ʂ��ԈႦ�Ă�tmp�l��1��2�ɂȂ��Ă��܂��Ă����̂ŏC��
		int tmp = rlev * ((r_ptr->flags2 & RF2_POWERFUL)?2:1);

		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND)) tmp *= 2;

		if(randint1(tmp) < (p_ptr->lev + adj_general[p_ptr->stat_ind[A_CHR]]))
		{
			if(one_in_(3)) msg_format("%^s�͎����������悤�Ƃ��Ă���ʂ��Ƃ����������B", m_name);
			else if(one_in_(2))msg_format("%^s�̓V�A���Z���B", m_name);
			else msg_format("%^s�͏����A���ȗl�q���B", m_name);
			return (TRUE);
		}
	}


	/* Hex: Anti Magic Barrier */
	if (!spell_is_inate(thrown_spell) && magic_barrier(m_idx))
	{
#ifdef JP
		msg_format("�����@�o���A��%^s�̎����������������B", m_name);
#else
		msg_format("Anti magic barrier cancels the spell which %^s casts.");
#endif
		return (TRUE);
	}
	//���_�̈�ɂ�鏢���W�Q����
	if(spell_is_summon(thrown_spell) && (r_ptr->flags3 & RF3_ANG_CHAOS))
	{
		int xx,yy;	

		for(xx=x-10;xx<x+10;xx++) for(yy=y-10;yy<y+10;yy++)
		{
			if(!in_bounds(yy,xx)) continue;
			if(!is_elder_sign(&cave[yy][xx])) continue;
			msg_format("���_�̈�%^s�̏������@�𖳌��������I", m_name);
			return (TRUE);
		}
	}
	if(spell_is_summon(thrown_spell) && (p_ptr->special_flags & SPF_ORBITAL_TRAP))
	{
		msg_format("%^s���������������悤�Ƃ��������]�����̃g���b�v�ɑj�Q���ꂽ�B", m_name);
		return (TRUE);
	}

	if (spell_is_summon(thrown_spell) && CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO) && randint1(rlev) < p_ptr->lev)
	{
		msg_format("%^s���������������悤�Ƃ������A���Ȃ��͕s�^�ȗ��K�҂����₵���B", m_name);
		return (TRUE);
	}

	if (spell_is_summon(thrown_spell) && SUPER_SHION && randint1(rlev) < p_ptr->lev)
	{
		msg_format("%^s�������X�^�[�������������A�����X�^�[�͉��E���ċA���Ă������B", m_name);
		return (TRUE);
	}

	//���ҏp�ɂ�鏢���W�Q
	if(spell_is_summon(thrown_spell) && p_ptr->deportation )
	{
		int power = p_ptr->lev;

		if (p_ptr->pclass == CLASS_OKINA) power *= 2;

		if (randint0(rlev) < power)
		{
			msg_format("%^s���������������悤�Ƃ��������Ȃ��͖W�Q�����B", m_name);
			return (TRUE);

		}

	}
	if ((p_ptr->pclass == CLASS_MARISA && p_ptr->tim_general[0] || check_activated_nameless_arts(JKF1_ABSORB_MAGIC))
		&& !spell_is_inate(thrown_spell) )
	{
		int power = rlev;
		int chance = p_ptr->lev;

		if(p_ptr->pclass == CLASS_JUNKO) chance *= 2;
		if(r_ptr->flags2 & RF2_POWERFUL) power = power * 3 / 2;
		if(randint1(power) < chance)
		{
			int monspell_num = thrown_spell - 95;

			if(monspell_num < 1 || monspell_num > MAX_MONSPELLS2)
			{
				msg_format("ERROR:�}�W�b�N�A�u�\�[�o�[�̓G���@��������������");
			}
			else if(monspell_list2[monspell_num].level) //�_�~�[����ʍs���͔�Ώ�
			{
				msg_format("���Ȃ���%^s�������Ă������@���z�������I", m_name);
				p_ptr->csp += monspell_list2[monspell_num].smana;
				if(p_ptr->csp > p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
				}
				p_ptr->redraw |= PR_MANA;
				redraw_stuff();
				return (TRUE);
			}
		}
		else
		{
				msg_format("%^s�̖��@�̖W�Q�Ɏ��s�����I", m_name);
		}
	}


	//v1.1.73 ����d�̃����X�^�[�����t���O����
	if (spell_is_summon(thrown_spell) && p_ptr->pclass == CLASS_YACHIE && p_ptr->magic_num1[0] && randint1(rlev) < p_ptr->lev)
	{
		flag_bribe_summon_monsters = TRUE;
	}



	can_remember = is_original_ap_and_seen(m_ptr);


	/* Cast the spell. */
	/*:::���������Aspell[]�̎g�p�ԍ��ŃX�C�b�`�@�Ȃ���switch���͒萔�錾�łȂ��n�[�h�R�[�f�B���O*/
	///sys mon res ��������惂���X�^�[�X�y���̎��s���� �������͑ϐ��������L�q
	switch (thrown_spell)
	{
		/* RF4_SHRIEK */
		case 96+0:
		{
			disturb(1, 1);
			if (m_ptr->r_idx == MON_KUTAKA)
			{
				msg_format("�u�R�P�[�[�I�v�u�s���I�v");
			}
			else
			{
				msg_format("%^s�����񍂂����ѐ����������B", m_name);

			}

			aggravate_monsters(m_idx,FALSE);
			nue_check_mult = 50;
			break;
		}

		/* RF4_DANMAKU */
		///�e���U���@���x�����_���[�W
		case 96+1:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������˂����B", m_name);
#else
			if (blind) msg_format("%^s shoots something.", m_name);
#endif
			else msg_format("%^s���e����������B", m_name);
			dam = rlev;

			bolt(m_idx, GF_MISSILE, dam, MS_DANMAKU, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);

			break;
		}

		/* RF4_DISPEL */
		case 96+2:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s��������͋����Ԃ₢���B", m_name);
			else msg_format("%^s�����͏����̎�����O�����B", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
			else msg_format("%^s invokes a dispel magic.", m_name);
#endif
			dispel_player();
			if (p_ptr->riding) dispel_monster_status(p_ptr->riding);

#ifdef JP
			///msg131221
			//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//	msg_print("���₪�����ȁI");
#endif
			learn_spell(MS_DISPEL);
			nue_check_mult = -1;
			break;
		}

		/* RF4_ROCKET */
		case 96+3:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������˂����B", m_name);
#else
			if (blind) msg_format("%^s shoots something.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����P�b�g�𔭎˂����B", m_name);
#else
			else msg_format("%^s fires a rocket.", m_name);
#endif

			dam = ((m_ptr->hp / 4) > 800 ? 800 : (m_ptr->hp / 4));
			breath(y, x, m_idx, GF_ROCKET,
				dam, 2, FALSE, MS_ROCKET, learnable);
			update_smart_learn(m_idx, DRS_SHARD);
			break;
		}

		/* RF4_SHOOT */
		case 96+4:
		{
			int r_idx = m_ptr->r_idx;
			if (!direct) return (FALSE);
			disturb(1, 1);
			dam = damroll(r_ptr->blow[0].d_dice, r_ptr->blow[0].d_side);

			if(r_idx == MON_NODEPPOU || r_idx == MON_DROLEM)
			{
				msg_format("%^s��������������B", m_name);
			}
			else if(r_idx == MON_SUMIREKO || r_idx == MON_SUMIREKO_2 || r_idx == MON_CHARGE_MAN || r_idx == MON_JURAL || r_idx == MON_NITORI || r_idx == MON_GYOKUTO || r_idx == MON_SEIRAN || r_idx == MON_EAGLE_RABBIT)
			{
				msg_format("%^s���e���������B", m_name);
			}
			else if(r_idx == MON_HAMMERBROTHER)
			{
				msg_format("%^s���n���}�[�𓊂����B", m_name);
			}
			else if(r_idx == MON_YAMAWARO)
			{
				msg_format("%^s���藠���𓊂����B", m_name);
			}
			else if(r_idx == MON_KISUME)
			{
				msg_format("%^s�����𓊂����B", m_name);
			}
			else if(r_idx == MON_SAKUYA)
			{
				msg_format("%^s����ʂ̃i�C�t�𓊝������B", m_name);
				dam *= 2;
			}
			else if(r_idx == MON_MURASA)
			{
				msg_format("%^s�����Ȃ��Ɍ������ĕd�𓊂������I", m_name);
				dam *= 2;
			}
			else if(r_idx == MON_HALFLING_S || r_idx == MON_CULVERIN || r_idx == MON_EIKA)
			{
				msg_format("%^s���΂ԂĂ�������B", m_name);
			}
			else if(r_idx == MON_COLOSSUS || r_idx == MON_SUIKA)
			{
				msg_format("%^s�����Ȃ��Ɍ������đ��𓊂������I", m_name);
				dam = dam * 5 / 2;
			}
			else if(r_idx == MON_NINJA_SLAYER)
			{
				msg_format("%^s�̓c���C�E�X���P����������I", m_name);
				dam = dam * 2;
			}

			else msg_format("%^s�����������B", m_name);

			bolt(m_idx, GF_ARROW, dam, MS_SHOOT, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF4_BR_HOLY */
		case 96+5:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ�u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes nether.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_HOLY_FIRE, dam,0, TRUE, MS_BR_HOLY, learnable);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 500;
			break;
		}

		/* RF4_BR_HELL */
		case 96+6:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���n���̋Ɖ΂̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes nether.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_HELL_FIRE, dam,0, TRUE, MS_BR_HELL, learnable);
			//update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF4_�A�N�A�u���X */
		case 96+7:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���A�N�A�u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes darkness.", m_name);
#endif

			dam = ((m_ptr->hp / 8) > 350 ? 350 : (m_ptr->hp / 8));
			breath(y, x, m_idx, GF_WATER, dam,0, TRUE, MS_BR_AQUA, learnable);
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF4_BR_ACID */
		case 96+8:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���_�̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes acid.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_ACID, dam, 0, TRUE, MS_BR_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF4_BR_ELEC */
		case 96+9:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s����Ȃ̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes lightning.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_ELEC, dam,0, TRUE, MS_BR_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF4_BR_FIRE */
		case 96+10:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Ή��̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes fire.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_FIRE, dam,0, TRUE, MS_BR_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}

		/* RF4_BR_COLD */
		case 96+11:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s����C�̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes frost.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_COLD, dam,0, TRUE, MS_BR_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}

		/* RF4_BR_POIS */
		case 96+12:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���K�X�̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes gas.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_POIS, dam, 0, TRUE, MS_BR_POIS, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}


		/* RF4_BR_NETH */
		case 96+13:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���n���̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes nether.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 550 ? 550 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_NETHER, dam,0, TRUE, MS_BR_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF4_BR_LITE */
		case 96+14:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���M���̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes light.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
			breath(y_br_lite, x_br_lite, m_idx, GF_LITE, dam,0, TRUE, MS_BR_LITE, learnable);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 400;
			break;
		}

		/* RF4_BR_DARK */
		case 96+15:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Í��̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes darkness.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DARK, dam,0, TRUE, MS_BR_DARK, learnable);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF4_WAVEMOTION */
		case 96+16:
		{
			disturb(1, 1);
			if (blind) 
				msg_format("%^s���������͂ȍU�����s�����B", m_name);
			else if(m_ptr->r_idx == MON_TOYOHIME)
				msg_format("%^s�͐�̂悤�Ȃ��̂����o�����Ȃ��Ɍ����đ傫���U�����I", m_name);
			else 
			msg_format("%^s���g���C��������I", m_name);
			dam = ((m_ptr->hp / 6) > 555 ? 555 : (m_ptr->hp / 6));
			//breath(y, x, m_idx, GF_DISINTEGRATE, dam, 5, FALSE, MS_WAVEMOTION, learnable);
			masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, MS_WAVEMOTION, learnable,2);
			//update_smart_learn(m_idx, DRS_CONF);

			/*:::Hack - �g���C���ˌ�́����s������܂Ŗ��@���g��Ȃ�*/
			m_ptr->mflag |= (MFLAG_NICE);
			repair_monsters = TRUE;

			//v1.1.58 ���̍U���ŏ��߂Ď��E���ʂ�����������Ȃ��̂�BGM���ă`�F�b�N
			check_music_mon_r_idx(m_idx);

			break;
		}

		/* RF4_BR_SOUN */
		case 96+17:
		{
			disturb(1, 1);
			if (m_ptr->r_idx == MON_JAIAN)
				msg_format("�u�{�H�G�`�`�`�`�`�`�v");
			else if(m_ptr->r_idx == MON_KUTAKA)
				msg_format("�u�R�@�P�@�R�@�b�@�R�@�[�@�I�v");

#ifdef JP
else if (blind) msg_format("%^s�����Ȃ��Ɍ����ďՌ��g��������I", m_name);
#else
			else if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ��Ɍ����ďՌ��g��������I", m_name);
#else
			else msg_format("%^s breathes sound.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_SOUND, dam,0, TRUE, MS_BR_SOUND, learnable);
			update_smart_learn(m_idx, DRS_SOUND);
			break;
		}

		/* RF4_BR_CHAO */
		case 96+18:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���J�I�X�̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes chaos.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 600 ? 600 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_CHAOS, dam,0, TRUE, MS_BR_CHAOS, learnable);
			update_smart_learn(m_idx, DRS_CHAOS);
			nue_check_mult = 10;
			break;
		}

		/* RF4_BR_DISE */
		case 96+19:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���򉻂̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes disenchantment.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DISENCHANT, dam,0, TRUE, MS_BR_DISEN, learnable);
			update_smart_learn(m_idx, DRS_DISEN);
			nue_check_mult = 200;
			break;
		}

		/* RF4_BR_NEXU */
		case 96+20:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ʍ����̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes nexus.", m_name);
#endif

			dam = ((m_ptr->hp / 7) > 600 ? 600 : (m_ptr->hp / 7));
			breath(y, x, m_idx, GF_NEXUS, dam,0, TRUE, MS_BR_NEXUS, learnable);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF4_BR_TIME */
		case 96+21:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ԋt�]�̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes time.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 150 ? 150 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_TIME, dam,0, TRUE, MS_BR_TIME, learnable);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF4_BR_INER */
		case 96+22:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���x�݂̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes inertia.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_INACT, dam,0, TRUE, MS_BR_INERTIA, learnable);
			break;
		}

		/* RF4_BR_GRAV */
		case 96+23:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���d�͂̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes gravity.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 200 ? 200 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_GRAVITY, dam,0, TRUE, MS_BR_GRAVITY, learnable);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF4_BR_SHAR */
		case 96+24:
		{
			disturb(1, 1);
			if (m_ptr->r_idx == MON_BOTEI)
#ifdef JP
				msg_format("�u�{��r���J�b�^�[�I�I�I�v");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
#ifdef JP
else if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			else if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���j�Ђ̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes shards.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_SHARDS, dam,0, TRUE, MS_BR_SHARDS, learnable);
			update_smart_learn(m_idx, DRS_SHARD);
			break;
		}

		/* RF4_BR_PLAS */
		case 96+25:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���v���Y�}�̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes plasma.", m_name);
#endif

			dam = ((m_ptr->hp / 4) > 1200 ? 1200 : (m_ptr->hp / 4));
			breath(y, x, m_idx, GF_PLASMA, dam,0, TRUE, MS_BR_PLASMA, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_ELEC);

			break;
		}

		/* RF4_BA_FORCE */
		case 96+26:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������������B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���C�e��������B", m_name);
#else
			else msg_format("%^s breathes force.", m_name);
#endif
			if(r_ptr->flags2 & RF2_POWERFUL)
				dam = ((m_ptr->hp / 4) > 350 ? 350 : (m_ptr->hp / 4));
			else
				dam = ((m_ptr->hp / 6) > 250 ? 250 : (m_ptr->hp / 6));
				
			breath(y, x, m_idx, GF_FORCE, dam, 2, FALSE, MS_BA_FORCE, learnable);
			break;
		}

		/* RF4_BR_MANA */
		case 96+27:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����͂̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes mana.", m_name);
#endif
			dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_MANA, dam,0, TRUE, MS_BR_MANA, learnable);
			break;
		}

		/* RF4_SPECIAL2 */
		case 96+28:
		{
			disturb(1, 1);
			
			//�t��������U���@�K���j�ł̎�+�����_���؂菝+�o���l����+�S�\�͒ቺ�@�אڂ��ĂȂ����HP700�ł܂������͂��Ȃ��͂�
			if(m_ptr->r_idx == MON_FLAN)
			{
				if (!direct) return (FALSE);
				if (!blind)
				{ 
					//v1.1.91 ���b�Z�[�W�ύX
					msg_format("%^s�͂��Ȃ������ăj�����Ə΂��ƌ������肵�߂��B", m_name);
					msg_print(NULL);
				}
				dam = (((s32b) ((40 + randint1(20)) * (p_ptr->chp))) / 100);
				breath(y, x, m_idx, GF_KYUTTOSHITEDOKA_N, dam, 0, FALSE, -1, FALSE);


			}
			//v1.1.49 �B��ޓ���s���@�����_�����j�[�N����
			else if (m_ptr->r_idx == MON_OKINA)
			{
				if (summon_named_creature(0, m_ptr->fy, m_ptr->fx, MON_RANDOM_UNIQUE_2, PM_ALLOW_SP_UNIQUE))
					msg_format("%^s�͐V���ȃ����X�^�[��n��o�����I", m_name);

			}

			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D)
			{
				/*:::���חאڎ��@���ƍU��*/
				if(distance(y, x, m_ptr->fy, m_ptr->fx)<2)
				{
					if (!blind)	msg_format("���F�̏��ƂłԂ񉣂�ꂽ�I");
					else msg_format("����������ɐU�艺�낳�ꂽ�I");
					//v1.2.00 50�|�C���g��̉�
					dam = 50 + randint0(100);
					breath(y, x, m_idx, GF_MISSILE, dam, 0, FALSE, -1, FALSE);
					earthquake_aux(y, x, 4, m_idx);
				}
				/*:::���ה�אڎ��@�ʒu����*/
				else
				{
					if( p_ptr->anti_tele || p_ptr->resist_time ) msg_format("%^s�̐�ł������������B",m_name);
					else if(cave[y][x].info & CAVE_ICKY || cave[m_ptr->fy][m_ptr->fx].info & CAVE_ICKY)  msg_format("%^s�͉��������悤�Ƃ��������s�����悤���B",m_name);
					else
					{
						msg_format("%^s�͂��Ȃ��Əꏊ�����������I",m_name);
						(void)move_player_effect(m_ptr->fy, m_ptr->fx, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
					}
				}
			}

			//�A�U�g�[�g����U���@�ߊ��ƕ������僌�[�U�[�������{�[����A������
			else if(m_ptr->r_idx == MON_AZATHOTH)
			{
				int atk_num;
				if (!blind)
				{ 
					msg_format("%^s�̋���ȋU�����_���W�������c���Ɋт����I", m_name);
					msg_print(NULL);
				}
				dam = 100 + randint1(100);
				for(atk_num = 6;atk_num>0;atk_num--)
				{
					int tx, ty;
					if(one_in_(6)) 	masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, 0, FALSE,(2));

					else 
					{
						while(TRUE)
						{
							tx = rand_spread(x,7);
							ty = rand_spread(y,7);
							if (!in_bounds(ty, tx)) continue;
							if (cave_have_flag_grid(&cave[ty][tx], FF_PERMANENT)) continue;
							break;
						}
						(void)project(m_idx, 1 + randint1(4), ty, tx, dam, GF_DISINTEGRATE, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
					}
				}

			}
			//�w�J�[�e�B�A����s��
			else if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
			{
				int dummy_x = m_ptr->fx;
				int dummy_y = m_ptr->fy;
				if(hecatia_change_idx < MON_HECATIA1 || hecatia_change_idx > MON_HECATIA3)
				{
					msg_format("ERROR:hecatia_change_idx����������(%d)",hecatia_change_idx);
					return FALSE;
				}
				if (p_ptr->inside_arena || p_ptr->inside_battle || !summon_possible(m_ptr->fy, m_ptr->fx)) return FALSE;
				hecatia_hp[m_ptr->r_idx - MON_HECATIA1] = m_ptr->hp;
				delete_monster_idx(cave[m_ptr->fy][m_ptr->fx].m_idx);

				//���̒���HP���Đݒ肷��
				if(summon_named_creature(0, dummy_y, dummy_x, hecatia_change_idx, 0L))
					msg_format("%^s�͑̂���サ���I", m_name);

			}
			else if(m_ptr->r_idx == MON_SAIBAIMAN)
			{
				if (!blind)
				{ 
					msg_format("%^s�͎��������I", m_name);
				}
				project(m_idx,2,m_ptr->fy,m_ptr->fx,damroll(10,80),GF_SHARDS,(PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
				suisidebomb = TRUE;
			}
			else if(m_ptr->r_idx == MON_KOSUZU)
			{
					msg_format("%^s�͎��͂̕t�r�_���z�������I", m_name);
					absorb_tsukumo(m_idx);
			}
			else if (m_ptr->r_idx == MON_YUMA)
			{
				int oil_field_num;
				int tmp_hp;
				msg_format("%^s�͎��͂̐Ζ����z������ŋz�������I", m_name);
				oil_field_num = yuma_vacuum_oil(m_ptr->fy, m_ptr->fx, FALSE, 0);

				//�Ȃ��Ǝv�����I�[�o�[�t���[�h�~�̂��߂Ɉꎞ�ϐ����o�R
				tmp_hp = m_ptr->hp + oil_field_num * 100;
				if (tmp_hp > m_ptr->maxhp) tmp_hp = m_ptr->maxhp;
				m_ptr->hp = tmp_hp;
				if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			}

			else
			{
				msg_format("ERROR: %^s�̓��ʂȍs��2�͒�`����Ă��Ȃ��B", m_name);
			}

			break;
		}

		/* RF4_BR_NUKE */
		case 96+29:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���j�M�̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes toxic waste.", m_name);
#endif

			dam = ((m_ptr->hp / 4) > 1000 ? 1000 : (m_ptr->hp / 4));
			breath(y, x, m_idx, GF_NUKE, dam,0, TRUE, MS_BR_NUKE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 150;
			break;
		}

		/* RF4_BA_CHAO */
		case 96+30:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�����낵���ɂԂ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles frighteningly.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������O���X��������B", m_name);/*nuke me*/
#else
			else msg_format("%^s invokes a raw Logrus.", m_name);
#endif

			dam = ((r_ptr->flags2 & RF2_POWERFUL) ? (rlev * 3) : (rlev * 2))+ damroll(10, 10);
			breath(y, x, m_idx, GF_CHAOS, dam, 4, FALSE, MS_BALL_CHAOS, learnable);
			update_smart_learn(m_idx, DRS_CHAOS);
			nue_check_mult = 10;
			break;
		}

		/* RF4_BR_DISI */
		case 96+31:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������̃u���X��f�����B", m_name);
#else
			else msg_format("%^s breathes disintegration.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DISINTEGRATE, dam,0, TRUE, MS_BR_DISI, learnable);

			//v1.1.58 ���̍U���ŏ��߂Ď��E���ʂ�����������Ȃ��̂�BGM���ă`�F�b�N
			check_music_mon_r_idx(m_idx);

			break;
		}



		/* RF5_BA_ACID */
		case 128+0:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
			else msg_format("%^s���A�V�b�h�E�{�[���̎������������B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
			else msg_format("%^s casts an acid ball.", m_name);
#endif
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 3) + 15);
			}
			breath(y, x, m_idx, GF_ACID, dam, rad, FALSE, MS_BALL_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF5_BA_ELEC */
		case 128+1:
		{
			int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 4 : 2;
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
			else msg_format("%^s���T���_�[�E�{�[���̎������������B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
			else msg_format("%^s casts a lightning ball.", m_name);
#endif
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 3 / 2) + 8);
			}
			breath(y, x, m_idx, GF_ELEC, dam, rad, FALSE, MS_BALL_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF5_BA_FIRE */
		case 128+2:
		{
			int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 4 : 2;
			disturb(1, 1);

			if (m_ptr->r_idx == MON_ROLENTO)
			{
#ifdef JP
				if (blind)
					msg_format("%s�������𓊂����B", m_name);
				else 
					msg_format("%s�͎�֒e�𓊂����B", m_name);
#else
				if (blind)
					msg_format("%^s throws something.", m_name);
				else
					msg_format("%^s throws a hand grenade.", m_name);
#endif
			}
			else if(m_ptr->r_idx == MON_ALICE)
			{
				if (blind)
					msg_format("%s�������𓊂����B", m_name);
				else
					msg_format("%s�͔���̋l�܂����l�`�𓊂����B", m_name);
			}

			else
			{
#ifdef JP
				if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
				else msg_format("%^s���t�@�C�A�E�{�[���̎������������B", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
				else msg_format("%^s casts a fire ball.", m_name);
#endif
			}

			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 7 / 2) + 10);
			}
			breath(y, x, m_idx, GF_FIRE, dam, rad, FALSE, MS_BALL_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}

		/* RF5_BA_COLD */
		case 128+3:
		{
			int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 4 : 2;
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
			else msg_format("%^s���A�C�X�E�{�[���̎������������B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
			else msg_format("%^s casts a frost ball.", m_name);
#endif
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 4;
				dam = (rlev * 4) + 50 + damroll(10, 10);
			}
			else
			{
				rad = 2;
				dam = (randint1(rlev * 3 / 2) + 10);
			}
			breath(y, x, m_idx, GF_COLD, dam, rad, FALSE, MS_BALL_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}

		/* RF5_BA_POIS */
		case 128+4:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����L�_�̎������������B", m_name);
#else
			else msg_format("%^s casts a stinking cloud.", m_name);
#endif

			dam = damroll(12, 2) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_POIS, dam, 2, FALSE, MS_BALL_POIS, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF5_BA_NETH */
		case 128+5:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���n�����̎������������B", m_name);
#else
			else msg_format("%^s casts a nether ball.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			breath(y, x, m_idx, GF_NETHER, dam, 2, FALSE, MS_BALL_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF5_BA_WATE */
		case 128+6:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������悤�Ȑg�U��������B", m_name);
#else
			else msg_format("%^s gestures fluidly.", m_name);
#endif

#ifdef JP
msg_print("���Ȃ��͉Q�����Ɉ��ݍ��܂ꂽ�B");
#else
			msg_print("You are engulfed in a whirlpool.");
#endif

			dam = ((r_ptr->flags2 & RF2_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
			breath(y, x, m_idx, GF_WATER, dam, 4, FALSE, MS_BALL_WATER, learnable);
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF5_BA_MANA */
		case 128+7:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������͋����Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����̗͂��̎�����O�����B", m_name);
#else
			else msg_format("%^s invokes a mana storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_MANA, dam, 4, FALSE, MS_BALL_MANA, learnable);
			break;
		}

		/* RF5_BA_DARK */
		case 128+8:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������͋����Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Í��̗��̎�����O�����B", m_name);
#else
			else msg_format("%^s invokes a darkness storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_DARK, dam, 4, FALSE, MS_BALL_DARK, learnable);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_DRAIN_MANA */
		case 128+9:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

			dam = (randint1(rlev) / 2) + 1;
			breath(y, x, m_idx, GF_DRAIN_MANA, dam, 0, FALSE, MS_DRAIN_MANA, learnable);
			update_smart_learn(m_idx, DRS_MANA);
			break;
		}

		/* RF5_MIND_BLAST */
		case 128+10:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
			if (!seen)
			{
#ifdef JP
msg_print("���������Ȃ��̐��_�ɔO������Ă���悤���B");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s�����Ȃ��̓��������Ƃɂ��ł���B", m_name);
#else
				msg_format("%^s gazes deep into your eyes.", m_name);
#endif

			}

			dam = damroll(7, 7);
			breath(y, x, m_idx, GF_MIND_BLAST, dam, 0, FALSE, MS_MIND_BLAST, learnable);
			nue_check_mult = 200;
			break;
		}

		/* RF5_BRAIN_SMASH */
		case 128+11:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
			if (!seen)
			{
#ifdef JP
msg_print("���������Ȃ��̐��_�ɔO������Ă���悤���B");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s�����Ȃ��̓��������ƌ��Ă���B", m_name);
#else
				msg_format("%^s looks deep into your eyes.", m_name);
#endif

			}

			dam = damroll(12, 12);
			breath(y, x, m_idx, GF_BRAIN_SMASH, dam, 0, FALSE, MS_BRAIN_SMASH, learnable);
			nue_check_mult = 200;
			break;
		}

		/* RF5_CAUSE_1 */
		case 128+12:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ����w�����Ď�����B", m_name);
#else
			else msg_format("%^s points at you and curses.", m_name);
#endif

			dam = damroll(3, 8);
			breath(y, x, m_idx, GF_CAUSE_1, dam, 0, FALSE, MS_CAUSE_1, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);


			break;
		}

		/* RF5_CAUSE_2 */
		case 128+13:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ����w�����ċ��낵���Ɏ�����B", m_name);
#else
			else msg_format("%^s points at you and curses horribly.", m_name);
#endif

			dam = damroll(8, 8);
			breath(y, x, m_idx, GF_CAUSE_2, dam, 0, FALSE, MS_CAUSE_2, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_CAUSE_3 */
		case 128+14:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������吺�ŋ��񂾁B", m_name);
#else
			if (blind) msg_format("%^s mumbles loudly.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ����w�����ċ��낵���Ɏ������������I", m_name);
#else
			else msg_format("%^s points at you, incanting terribly!", m_name);
#endif

			dam = damroll(10, 15);
			breath(y, x, m_idx, GF_CAUSE_3, dam, 0, FALSE, MS_CAUSE_3, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_CAUSE_4 */
		case 128+15:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
			///msg131221
			if (blind) msg_format("%^s�������ЁX�������t�����񂾁B", m_name);
#else
			if (blind) msg_format("%^s screams the word 'DIE!'", m_name);
#endif

#ifdef JP
			///msg131221
			else msg_format("%^s�����Ȃ��Ɍ����Ď��̌����������I", m_name);
#else
			else msg_format("%^s points at you, screaming the word DIE!", m_name);
#endif

			dam = damroll(15, 15);
			breath(y, x, m_idx, GF_CAUSE_4, dam, 0, FALSE, MS_CAUSE_4, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_BO_ACID */
		case 128+16:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���A�V�b�h�E�{���g�̎������������B", m_name);
#else
			else msg_format("%^s casts a acid bolt.", m_name);
#endif

			dam = (damroll(7, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_ACID, dam, MS_BOLT_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_ELEC */
		case 128+17:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���T���_�[�E�{���g�̎������������B", m_name);
#else
			else msg_format("%^s casts a lightning bolt.", m_name);
#endif

			dam = (damroll(4, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_ELEC, dam, MS_BOLT_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_FIRE */
		case 128+18:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���t�@�C�A�E�{���g�̎������������B", m_name);
#else
			else msg_format("%^s casts a fire bolt.", m_name);
#endif

			dam = (damroll(9, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_FIRE, dam, MS_BOLT_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_COLD */
		case 128+19:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���A�C�X�E�{���g�̎������������B", m_name);
#else
			else msg_format("%^s casts a frost bolt.", m_name);
#endif

			dam = (damroll(6, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_COLD, dam, MS_BOLT_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BA_LITE */
		case 128+20:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������͋����Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
			///msg131221
else msg_format("%^s���M���̗��̎�����O�����B", m_name);
#else
			else msg_format("%^s invokes a starburst.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_LITE, dam, 4, FALSE, MS_STARBURST, learnable);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 500;
			break;
		}

		/* RF5_BO_NETH */
		case 128+21:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���n���̖�̎������������B", m_name);
#else
			else msg_format("%^s casts a nether bolt.", m_name);
#endif

			dam = 30 + damroll(5, 5) + (rlev * 4) / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3);
			bolt(m_idx, GF_NETHER, dam, MS_BOLT_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_WATE */
		case 128+22:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���E�H�[�^�[�E�{���g�̎������������B", m_name);
#else
			else msg_format("%^s casts a water bolt.", m_name);
#endif

			dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_WATER, dam, MS_BOLT_WATER, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF5_BO_MANA */
		case 128+23:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����̖͂�̎������������B", m_name);
#else
			else msg_format("%^s casts a mana bolt.", m_name);
#endif

			dam = randint1(rlev * 7 / 2) + 50;
			bolt(m_idx, GF_MANA, dam, MS_BOLT_MANA, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_PLAS */
		case 128+24:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���v���Y�}�E�{���g�̎������������B", m_name);
#else
			else msg_format("%^s casts a plasma bolt.", m_name);
#endif

			dam = 100 + damroll(1, 200) + (rlev * 3 * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			bolt(m_idx, GF_PLASMA, dam, MS_BOLT_PLASMA, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF5_BO_ICEE */
		case 128+25:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Ɋ��̖�̎������������B", m_name);
#else
			else msg_format("%^s casts an ice bolt.", m_name);
#endif

			dam = damroll(6, 6) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_ICE, dam, MS_BOLT_ICE, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_MISSILE */
		case 128+26:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���}�W�b�N�E�~�T�C���̎������������B", m_name);
#else
			else msg_format("%^s casts a magic missile.", m_name);
#endif

			dam = damroll(2, 6) + (rlev / 3);
			bolt(m_idx, GF_MISSILE, dam, MS_MAGIC_MISSILE, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_SCARE */
		case 128+27:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₭�ƁA���낵���ȉ������������B", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear scary noises.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����낵���Ȍ��o�����o�����B", m_name);
#else
			else msg_format("%^s casts a fearful illusion.", m_name);
#endif


			//v1.1.37 �����O����
			if(IS_VULN_FEAR)
			{
				int chance = 100 + rlev;
				if (p_ptr->resist_fear) chance /= 2;

				if(randint0(chance) > p_ptr->skill_sav)
				{
					(void)set_afraid(p_ptr->afraid + randint0(6) + 6);

					if(!p_ptr->resist_fear && randint0(rlev) > p_ptr->lev)
					{
						msg_print("���Ȃ��͋��|�̂��܂�ɋC�₵�Ă��܂����I");
						(void)set_paralyzed(p_ptr->paralyzed + randint0(3) + 3);
					}

				}
				else
				{
					msg_print("���������|�ɐN����Ȃ������B");
				}

			}
			else if (p_ptr->resist_fear)
			{
#ifdef JP
msg_print("���������|�ɐN����Ȃ������B");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������|�ɐN����Ȃ������B");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else
			{
				(void)set_afraid(p_ptr->afraid + randint0(4) + 4);
			}
			learn_spell(MS_SCARE);
			update_smart_learn(m_idx, DRS_FEAR);
			nue_check_mult = 50;
			break;
		}

		/* RF5_BLIND */
		case 128+28:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������������Ă��Ȃ��̖ڂ�����܂����I", m_name);
#else
			else msg_format("%^s casts a spell, burning your eyes!", m_name);
#endif

			if (p_ptr->resist_blind)
			{
#ifdef JP
msg_print("���������ʂ��Ȃ������I");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_blind(12 + randint0(4));
			}
			learn_spell(MS_BLIND);
			update_smart_learn(m_idx, DRS_BLIND);
			nue_check_mult = 50;
			break;
		}

		/* RF5_CONF */
		case 128+29:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₭�ƁA����Y�܂����������B", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear puzzling noises.", m_name);
#endif

#ifdef JP
else msg_format("%^s���U�f�I�Ȍ��o�����o�����B", m_name);
#else
			else msg_format("%^s creates a mesmerising illusion.", m_name);
#endif

			if (p_ptr->resist_conf)
			{
#ifdef JP
msg_print("���������o�ɂ͂��܂���Ȃ������B");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������o�ɂ͂��܂���Ȃ������B");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else
			{
				(void)set_confused(p_ptr->confused + randint0(4) + 4);
			}
			learn_spell(MS_CONF);
			update_smart_learn(m_idx, DRS_CONF);
			nue_check_mult = 50;
			break;
		}

		/* RF5_SLOW */
		case 128+30:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
msg_format("%^s�����Ȃ��̋ؗ͂��z����낤�Ƃ����I", m_name);
#else
			msg_format("%^s drains power from your muscles!", m_name);
#endif

			if (p_ptr->free_act)
			{
#ifdef JP
msg_print("���������ʂ��Ȃ������I");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);
			}
			learn_spell(MS_SLOW);
			update_smart_learn(m_idx, DRS_FREE);
			nue_check_mult = 50;
			break;
		}

		/* RF5_HOLD */
		case 128+31:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ��̖ڂ������ƌ��߂��I", m_name);
#else
			else msg_format("%^s stares deep into your eyes!", m_name);
#endif

			if (p_ptr->free_act)
			{
#ifdef JP
msg_print("���������ʂ��Ȃ������I");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_format("���������͂𒵂˕Ԃ����I");
#else
				msg_format("You resist the effects!");
#endif

			}
			else
			{
				(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
			}
			learn_spell(MS_SLEEP);
			update_smart_learn(m_idx, DRS_FREE);
			break;
		}

		/* RF6_HASTE */
		case 160+0:
		{
			disturb(1, 1);
			if (blind)
			{
#ifdef JP
msg_format("%^s���������Ԃ₢���B", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s�������̑̂ɔO�𑗂����B", m_name);
#else
				msg_format("%^s concentrates on %s body.", m_name, m_poss);
#endif

			}

			/* Allow quick speed increases to base+10 */
			if (set_monster_fast(m_idx, MON_FAST(m_ptr) + 100))
			{
#ifdef JP
				msg_format("%^s�̓����������Ȃ����B", m_name);
#else
				msg_format("%^s starts moving faster.", m_name);
#endif
			}
			nue_check_mult = 20;
			break;
		}

		/* RF6_HAND_DOOM */
		case 160+1:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
msg_format("%^s��<�j�ł̎�>��������I", m_name);
#else
			msg_format("%^s invokes the Hand of Doom!", m_name);
#endif
			dam = (((s32b) ((40 + randint1(20)) * (p_ptr->chp))) / 100);
			breath(y, x, m_idx, GF_HAND_DOOM, dam, 0, FALSE, MS_HAND_DOOM, learnable);
			break;
		}

		/* RF6_HEAL */
		case 160+2:
		{
			disturb(1, 1);

			/* Message */
			if (blind)
			{
#ifdef JP
msg_format("%^s���������Ԃ₢���B", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s�������̏��ɏW�������B", m_name);
#else
				msg_format("%^s concentrates on %s wounds.", m_name, m_poss);
#endif

			}

			/* Heal some */
			m_ptr->hp += (rlev * 6);

			/* Fully healed */
			if (m_ptr->hp >= m_ptr->maxhp)
			{
				/* Fully healed */
				m_ptr->hp = m_ptr->maxhp;

				/* Message */
				if (seen)
				{
#ifdef JP
msg_format("%^s�͊��S�Ɏ������I", m_name);
#else
					msg_format("%^s looks completely healed!", m_name);
#endif

				}
				else
				{
#ifdef JP
msg_format("%^s�͊��S�Ɏ������悤���I", m_name);
#else
					msg_format("%^s sounds completely healed!", m_name);
#endif

				}
			}

			/* Partially healed */
			else
			{
				/* Message */
				if (seen)
				{
#ifdef JP
msg_format("%^s�̗͑͂��񕜂����悤���B", m_name);
#else
					msg_format("%^s looks healthier.", m_name);
#endif

				}
				else
				{
#ifdef JP
msg_format("%^s�̗͑͂��񕜂����悤���B", m_name);
#else
					msg_format("%^s sounds healthier.", m_name);
#endif

				}
			}

			/* Redraw (later) if needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);
			nue_check_mult = 10;

			/* Cancel fear */
			if (MON_MONFEAR(m_ptr))
			{
				/* Cancel fear */
				(void)set_monster_monfear(m_idx, 0);

				/* Message */
#ifdef JP
				msg_format("%^s�͗E�C�����߂����B", m_name);
#else
				msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif
			}
			break;
		}

		/* RF6_INVULNER */
		case 160+3:
		{
			disturb(1, 1);

			/* Message */
			if (!seen)
			{
#ifdef JP
msg_format("%^s��������͋����Ԃ₢���B", m_name);
#else
				msg_format("%^s mumbles powerfully.", m_name);
#endif

			}
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format("%^s�̑̂����̂������Â������ʂ����E�E", m_name);

			else
			{
#ifdef JP
msg_format("%s�͖����̋��̎������������B", m_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", m_name);
#endif

			}

			if (!MON_INVULNER(m_ptr)) (void)set_monster_invulner(m_idx, randint1(4) + 4, FALSE);
			nue_check_mult = 20;
			break;
		}

		/* RF6_BLINK */
		case 160+4:
		{
			if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && is_hostile(m_ptr) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (seen)
					msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", m_name);

				delete_monster_idx(m_idx);
				return TRUE;

			}

			disturb(1, 1);

			if (teleport_barrier(m_idx))
			{
#ifdef JP
				msg_format("%^s�̃e���|�[�g�͖W�Q���ꂽ�B", m_name);
			//	msg_format("���@�̃o���A��%^s�̃e���|�[�g���ז������B", m_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", m_name);
#endif
			}
			else
			{
#ifdef JP
			if(m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) ; //�������̃V���[�g�e���|�[�g�̓��b�Z�[�W���o�Ȃ�
			else 	msg_format("%^s���u���ɏ������B", m_name);
#else
				msg_format("%^s blinks away.", m_name);
#endif
				teleport_away(m_idx, 10, 0L);
				p_ptr->update |= (PU_MONSTERS);
			}
			nue_check_mult = 20;
			break;
		}

		/* RF6_TPORT */
		case 160+5:
		{

			if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && is_hostile(m_ptr) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (seen)
					msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", m_name);

				delete_monster_idx(m_idx);
				return TRUE;

			}

			disturb(1, 1);


			if (teleport_barrier(m_idx))
			{
#ifdef JP
				msg_format("%^s�̃e���|�[�g�͖W�Q���ꂽ�B", m_name);
			//	msg_format("���@�̃o���A��%^s�̃e���|�[�g���ז������B", m_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", m_name);
#endif
			}
			else
			{
#ifdef JP
			if(m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI) ; //�������̃e���|�[�g�̓��b�Z�[�W���o�Ȃ�
			else	msg_format("%^s���e���|�[�g�����B", m_name);
#else
				msg_format("%^s teleports away.", m_name);
#endif
				teleport_away_followable(m_idx);
			}
			nue_check_mult = 20;
			break;
		}

		/* RF6_WORLD */
		case 160+6:
		{
			///mod140107 ���Ԓ�~�ɍ���ǉ�
			int who = 0;
			disturb(1, 1);
			if(m_ptr->r_idx == MON_DIO) who = 1;
			else if(m_ptr->r_idx == MON_SAKUYA) who = 2;
			else if(m_ptr->r_idx == MON_WONG) who = 3;
			dam = who;
			if (!process_the_world(randint1(2)+2, who, TRUE)) return (FALSE);
			break;
		}

		/* RF6_SPECIAL */
		///mon ���ʍs���̎��s����
		case 160+7:
		{
			int k;

			disturb(1, 1);
			switch (m_ptr->r_idx)
			{
			case MON_OHMU:
				/* Moved to process_monster(), like multiplication */
				return FALSE;
			case MON_NINJA_SLAYER:
				if (blind) msg_format("%^s�͉������ʂɕ������I", m_name);
				else msg_format("�i���T���I%s���������͔̂�l������}�L�r�V���I", m_name);

				breath(y, x, m_idx, GF_INACT, 100, 3, FALSE, 0, FALSE);
				break;

			case MON_HISAMI:
				if (blind) msg_format("�ˑR�̂������ɒ��ߕt����ꂽ�I", m_name);
				else msg_format("�n�ʂ��疠�̂悤�Ȃ��̂������Ă��Ă��Ȃ����S�������I", m_name);

				breath(y, x, m_idx, GF_NO_MOVE, 10+randint1(10), 3, FALSE, 0, FALSE);
				break;

			case MON_CHIMATA:
				if (blind) msg_format("%^s����ٗl�ȃG�l���M�[�����˂��ꂽ�I", m_name);
				else msg_format("%^s�͓��F�̗��������N�������I", m_name);
				dam = (rlev * 4) + 50 + damroll(10, 10);
				breath(y, x, m_idx, GF_RAINBOW, dam, 5, FALSE, 0, FALSE);

				break;

			case MON_REIMU:
				if (blind) msg_format("%^s�͉������ʂɕ������I", m_name);
				else msg_format("%^s�͖��z�����������I", m_name);
				cast_musou_hu_in(m_idx);
				break;
			case MON_BANORLUPART:
				{
					int dummy_hp = (m_ptr->hp + 1) / 2;
					int dummy_maxhp = m_ptr->maxhp/2;
					int dummy_y = m_ptr->fy;
					int dummy_x = m_ptr->fx;

					if (p_ptr->inside_arena || p_ptr->inside_battle || !summon_possible(m_ptr->fy, m_ptr->fx)) return FALSE;
					delete_monster_idx(cave[m_ptr->fy][m_ptr->fx].m_idx);
					summon_named_creature(0, dummy_y, dummy_x, MON_BANOR, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;
					summon_named_creature(0, dummy_y, dummy_x, MON_LUPART, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;

#ifdef JP
					msg_print("�w�o�[�m�[���E���p�[�g�x�����􂵂��I");
#else
					msg_print("Banor=Rupart splits in two person!");
#endif

					break;
				}

			case MON_BANOR:
			case MON_LUPART:
				{
					int dummy_hp = 0;
					int dummy_maxhp = 0;
					int dummy_y = m_ptr->fy;
					int dummy_x = m_ptr->fx;

					if (!r_info[MON_BANOR].cur_num || !r_info[MON_LUPART].cur_num) return (FALSE);
					for (k = 1; k < m_max; k++)
					{
						if (m_list[k].r_idx == MON_BANOR || m_list[k].r_idx == MON_LUPART)
						{
							dummy_hp += m_list[k].hp;
							dummy_maxhp += m_list[k].maxhp;
							if (m_list[k].r_idx != m_ptr->r_idx)
							{
								dummy_y = m_list[k].fy;
								dummy_x = m_list[k].fx;
							}
							delete_monster_idx(k);
						}
					}
					summon_named_creature(0, dummy_y, dummy_x, MON_BANORLUPART, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;

#ifdef JP
					msg_print("�w�o�[�m�[���x�Ɓw���p�[�g�x�����̂����I");
#else
					msg_print("Banor and Rupart combine into one!");
#endif

					break;
				}

			case MON_ROLENTO:
#ifdef JP
				if (blind) msg_format("%^s��������ʂɓ������B", m_name);
				else msg_format("%^s�͎�֒e���΂�܂����B", m_name);
#else
				if (blind) msg_format("%^s spreads something.", m_name);
				else msg_format("%^s throws some hand grenades.", m_name);
#endif

				{
					int num = 1 + randint1(3);

					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_SHURYUUDAN, mode);
					}
				}
#ifdef JP
				if (blind && count) msg_print("�����̂��̂��ԋ߂ɂ΂�܂���鉹������B");
#else
				if (blind && count) msg_print("You hear many things are scattered nearby.");
#endif
				break;

			case MON_MICHAEL:
#ifdef JP
				msg_format("%^s�u�R���s����I�X�s�L���[���I�v", m_name);
#else
				if (blind) msg_format("%^s spreads something.", m_name);
				else msg_format("%^s throws some hand grenades.", m_name);
#endif
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1600);				
				if(one_in_(3))msg_format("%^s�u���̉��l�͋������낤���I�v", m_name);
				else if(one_in_(2))msg_format("%^s�u���������������I�M�����������������I�v", m_name);
				else msg_format("%^s�u���܂ł������߂Ă���I�v", m_name);

				break;
			case MON_MOKOU:
				msg_format("%^s�̓t�W���}���H���P�C�m��������I", m_name);
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1000);
				break;

			case MON_SHION_2:
				msg_format("%^s�͕s�^�ƕs�K���܂��U�炵���I", m_name);
				project_hack3(m_idx, m_ptr->fy, m_ptr->fx, GF_DISENCHANT, 0, 0, 400);

				if (p_ptr->au > 0 && direct && !weird_luck() && !p_ptr->inside_arena)
				{
					p_ptr->au /= 2;
					msg_print("���z���������y���Ȃ����C������I");
					p_ptr->redraw |= PR_GOLD;
				}


				break;

			case MON_FLAN:
				{
					for (k = 0; k < 3; k++)
						count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_FLAN_4, mode);
				}
				break;
				//���ד���s��
			case MON_SEIJA:
				{
					int decoy_m_idx = 0;
					int j;
					for (j = 1; j < m_max; j++) if(m_list[j].r_idx == MON_SEIJA_D) 
					{
						decoy_m_idx = j;
						break;
					}
					/*:::�f�R�C�l�`�����łɂ���ꍇ���b�Z�[�W�����Ńf�R�C�Ƃ̈ʒu����*/
					if(decoy_m_idx)
					{
						monster_type *dm_ptr = &m_list[decoy_m_idx];
						int x1=dm_ptr->fx,y1=dm_ptr->fy  ,x2 = m_ptr->fx,y2=m_ptr->fy;
					
						if( cave[y1][x1].info & CAVE_ICKY || cave[y2][x2].info & CAVE_ICKY || teleport_barrier(m_idx) || teleport_barrier(decoy_m_idx))
							 msg_format("%^s�͉��������悤�Ƃ��������s�����悤���B",m_name);
						else
						{
							cave[y2][x2].m_idx = decoy_m_idx;
							cave[y1][x1].m_idx = m_idx;
							dm_ptr->fy = y2;
							dm_ptr->fx = x2;
							m_ptr->fy = y1;
							m_ptr->fx = x1;
							update_mon(m_idx,TRUE);
							update_mon(decoy_m_idx,TRUE);
							lite_spot(y1,x1);
							lite_spot(y2,x2);
							verify_panel();
						}

					}
					/*:::�f�R�C�l�`�����Ȃ��ꍇ���b�Z�[�W�����Ńe���|�[�g���A���̏ꏊ�Ƀf�R�C������*/
					else
					{

						int old_y = m_ptr->fy;
						int old_x = m_ptr->fx;
						teleport_away(m_idx, 30, 0L);
						count += summon_named_creature(m_idx, old_y, old_x, MON_SEIJA_D, mode);
					}
				}
				break;
			case MON_YUKARI:
				{
					if (!direct) return (FALSE);
					msg_format("%^s���󒆂Ɍ����J����ƁA���̒�����d�Ԃ��ːi���Ă����I", m_name);
					beam(m_idx, GF_TRAIN, 100+randint1(50), 0, FALSE);
				}
				break;
			case MON_SUMIREKO:
				{
					if (!direct) return (FALSE);
					msg_format("�˔@�A�ǂ����炩���Ȃ��̕��ɓS�����|��Ă����I");
					(void)project(m_idx, 0, py, px, 100+randint1(50), GF_TRAIN, (PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
				}
				break;
				//v1.1.32
			case MON_SATONO:
			case MON_MAI:
				{
					char tmp_m_name[80];
					monster_desc(tmp_m_name,&m_list[rankup_monster_idx],0);
					msg_format("%^s��%s�̔w��ŗx�����I",m_name,tmp_m_name);
					rankup_monster(rankup_monster_idx,1);
				}
				break;
			case MON_OKINA:
				{
					msg_format("%^s�͂��Ȃ��̔w��̔����J�����I",m_name);
					if(p_ptr->resist_time || p_ptr->anti_tele)
					{
						msg_print("���������Ȃ��͒�R�����I");
					}
					else
					{
						//1�̂Ƃ�process_world_aux_movement()�ɍs���΋A�Ҕ�������
						recall_player(randint1(2));
					}
				}
				break;

				//v1.1.53�@�X�ѐ���
			case MON_TSUCHINOKO:
				msg_format("�ˑR�ӂ������΂���%^s�𕢂����B", m_name);
				project(m_idx, 1, m_ptr->fy, m_ptr->fx, 1, GF_MAKE_TREE, (PROJECT_GRID), 0);

				break;

			case MON_MIYOI: //v1.1.78

				msg_format("%^s�͂��Ȃ��ɂ��������߂Ă����B", m_name);

				if (p_ptr->paralyzed || p_ptr->confused || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
				{
					msg_print("���������Ȃ��͕Ԏ����ł��Ȃ��B");

				}
				else
				{
					set_alcohol(p_ptr->alcohol + calc_alcohol_mod(2000 + randint1(2000), FALSE));
					if (p_ptr->alcohol >= DRANK_2 && !(EXTRA_MODE))
					{

						if (summon_named_creature(m_idx, py, px, MON_SYOUZYOU, mode))
							msg_print("�������n�����𗧂ĂĂ��Ȃ��̔w��Ɍ��ꂽ�I");
					}

				}
				break;

				//v1.1.98 ����@���ɜ߈˂��ď���
			case MON_MIZUCHI:

				msg_format("%^s�͋���Ȏ�������Ȃ��̎�Ɍ����Ĕ�΂����I", m_name);

				if (p_ptr->pclass == CLASS_BANKI)
				{
					msg_print("���������Ȃ��ɂ͎񂪂Ȃ������B");
					break;
				}
				else if (p_ptr->prace == RACE_HANIWA)
				{
					msg_print("���Ȃ��ɂ͊��S�ȑϐ�������I");
					break;
				}
				//���Ẩ��O�̒������Ɠ�������ɂ��Ă�����
				else if (randint1(125) < p_ptr->skill_sav)
				{
					msg_print("���Ȃ��͜߈˂ɒ�R�����I");
					break;
				}
				else
				{
					msg_print("���Ȃ��̎�Ɏ�������������I");
					gain_random_mutation(217);
					delete_monster_idx(m_idx);
					aggravate_monsters(0, TRUE);

					return TRUE;

				}

				break;

			//���ΐ��N����s���@�L�͈͒n�k�A���@�̗͂������Ă���قǔ͈͂Ɖ񐔂�����
			case MON_TAISAI:
			{
				int tmp_rad = 7 + (m_ptr->maxhp - m_ptr->hp) / 1000;
				int tmp_num = 2 + (m_ptr->maxhp - m_ptr->hp) / 5000;

				msg_format("%^s�͑�n�k���N�������I", m_name);
				for (k = 0; k < tmp_num; k++)
				{
					earthquake_aux(m_ptr->fy, m_ptr->fx, tmp_rad, m_idx);
					tmp_rad += randint1(2);

				}
			}
			break;

			default:
				if (r_ptr->d_char == 'B')
				{
					disturb(1, 1);
					if (one_in_(3) || !direct)
					{
#ifdef JP
						msg_format("%^s�͓ˑR���E���������!", m_name);
#else
						msg_format("%^s suddenly go out of your sight!", m_name);
#endif
						teleport_away(m_idx, 10, TELEPORT_NONMAGICAL);
						p_ptr->update |= (PU_MONSTERS);
					}
					else
					{
						int get_damage = 0;
						bool fear; /* dummy */

#ifdef JP
						msg_format("%^s�����Ȃ���͂�ŋ󒆂��瓊�����Ƃ����B", m_name);
#else
						msg_format("%^s holds you, and drops from the sky.", m_name);
#endif
						dam = damroll(4, 8);
						teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

						sound(SOUND_FALL);

						if (p_ptr->levitation)
						{
#ifdef JP
							msg_print("���Ȃ��͐Â��ɒ��n�����B");
#else
							msg_print("You float gently down to the ground.");
#endif
						}
						else
						{
#ifdef JP
							msg_print("���Ȃ��͒n�ʂɒ@������ꂽ�B");
#else
							msg_print("You crashed into the ground.");
#endif
							dam += damroll(6, 8);
						}

						/* Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.
						   -- henkma
						 */
						get_damage = take_hit(DAMAGE_NOESCAPE, dam, m_name, -1);
						if (p_ptr->tim_eyeeye && get_damage > 0 && !p_ptr->is_dead)
						{
#ifdef JP
							msg_format("�U����%s���g���������I", m_name);
#else
							char m_name_self[80];

							/* hisself */
							monster_desc(m_name_self, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

							msg_format("The attack of %s has wounded %s!", m_name, m_name_self);
#endif
							project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);

							if(p_ptr->pclass == CLASS_HINA)
							{
								hina_gain_yaku(-get_damage/5);
								if(!p_ptr->magic_num1[0]) set_tim_eyeeye(0,TRUE);
							}

							else set_tim_eyeeye(p_ptr->tim_eyeeye-5, TRUE);
						}

						if (p_ptr->riding) mon_take_hit_mon(p_ptr->riding, dam, &fear, extract_note_dies(real_r_ptr(&m_list[p_ptr->riding])), m_idx);
					}
					break;
				}

				/* Something is wrong */
				else return FALSE;
			}
			break;
		}

		/* RF6_TELE_TO */
		case 160+8:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);

			if(p_ptr->pclass == CLASS_KOMACHI && p_ptr->lev > 29)
			{
				msg_format("%^s�����Ȃ��������߁E�E�����Ƃ��������Ȃ��͌��̈ʒu�ɖ߂����B", m_name);
				break;
			}
			else if(CHECK_FAIRY_TYPE == 44)
			{
				msg_format("%^s�����Ȃ��������߁E�E�����Ƃ��������Ȃ��͂��蔲�����B", m_name);
				break;
			}
			if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags2 & RFR_RES_TELE)
			{
				msg_format("%^s�̃e���|�[�g�E�o�b�N�͂��Ȃ��ɂ͌����Ȃ������B", m_name);
				break;
			}

#ifdef JP
			if(m_ptr->r_idx == MON_NINJA_SLAYER) 
				msg_format("%^s�����Ȃ��������߂����B�h�E�O�Ђ̃t�b�N���[�v�́A�}�b�|�[�̐��Ɏ�������E�l�̋Z���p�����Â��Ă���̂��B", m_name);
			else
				msg_format("%^s�����Ȃ��������߂����B", m_name);

#else
			msg_format("%^s commands you to return.", m_name);
#endif

			teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			learn_spell(MS_TELE_TO);
			break;
		}

		/* RF6_TELE_AWAY */
		case 160+9:
		{
			if (!direct) return (FALSE);
			///mod131228 ����ϐ��ɂ��e���|�A�E�F�C��R���������Ă݂��B
			if(p_ptr->pclass == CLASS_KOMACHI && p_ptr->lev > 29)
			{
				msg_format("%^s�Ƀe���|�[�g�������E�E�����������Ȃ��͌��̈ʒu�ɖ߂����B", m_name);
				break;
			}
			if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags2 & RFR_RES_TELE)
			{
				msg_format("%^s�̃e���|�[�g�E�A�E�F�C�͂��Ȃ��ɂ͌����Ȃ������B", m_name);
				break;
			}

			if(p_ptr->resist_time && p_ptr->lev > randint1(rlev))
			{
				msg_format("%^s�̃e���|�[�g�E�A�E�F�C�͂��Ȃ��ɂ͌����Ȃ������B", m_name);
				break;
			}

			disturb(1, 1);
#ifdef JP
msg_format("%^s�Ƀe���|�[�g������ꂽ�B", m_name);
					///msg131221			
					//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
					//msg_print("�������`");
#else
			msg_format("%^s teleports you away.", m_name);
#endif

			learn_spell(MS_TELE_AWAY);
			teleport_player_away(m_idx, 100);
			update_smart_learn(m_idx, DRS_TIME);
			nue_check_mult = 50;
			break;
		}

		/* RF6_TELE_LEVEL */
		case 160+10:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������Ȍ��t���Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles strangely.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ��̑����w�������B", m_name);
#else
			else msg_format("%^s gestures at your feet.", m_name);
#endif
///mod131228 ���҃e���|���x���@���ʍ����ϐ�������ϐ��@�����폜���邩��
			//if (p_ptr->resist_nexus)
			if (p_ptr->resist_time)
			{
#ifdef JP
msg_print("���������ʂ��Ȃ������I");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				teleport_level(0);
			}
			learn_spell(MS_TELE_LEVEL);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF6_PSY_SPEAR */
		case 160+11:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
			if (blind) msg_format("%^s�������Ïk���ꂽ�G�l���M�[��������B", m_name);
			else if(m_ptr->r_idx == MON_REMY) msg_format("%^s���O���O�j���𓊂������B", m_name);
			else msg_format("%^s�����̌���������B", m_name);

			dam = (r_ptr->flags2 & RF2_POWERFUL) ? (randint1(rlev * 2) + 150) : (randint1(rlev * 3 / 2) + 100);
			if(m_ptr->r_idx == MON_REMY) beam(m_idx, GF_GUNGNIR, dam, MS_PSY_SPEAR, learnable);
			else beam(m_idx, GF_PSY_SPEAR, dam, MS_PSY_SPEAR, learnable);
			nue_check_mult = 300;
			break;
		}

		/* RF6_DARKNESS */
		case 160+12:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else if (can_use_lite_area) msg_format("%^s���ӂ�𖾂邭�Ƃ炵���B", m_name);
			else msg_format("%^s���Èł̒��Ŏ��U�����B", m_name);
#else
			else if (can_use_lite_area) msg_format("%^s cast a spell to light up.", m_name);
			else msg_format("%^s gestures in shadow.", m_name);
#endif

			if (can_use_lite_area) 
			{
				(void)lite_area(0, 3);
				nue_check_mult = 500;
			}
			else
			{
				learn_spell(MS_DARKNESS);
				(void)unlite_area(0, 3);
			}

			break;
		}

		/* RF6_TRAPS */
		case 160+13:
		{
			disturb(1, 1);

			if (m_ptr->r_idx == MON_ENOKO)
			{
				msg_format("%^s�͂��Ȃ��̎���ɉ������o���T�����I", m_name);
				hack_flag_enoko_make_beartrap = TRUE;
			}

#ifdef JP
			else if (blind) msg_format("%^s���������Ԃ₢���B����̒n�ʂɉ��������ꂽ�C������B", m_name);
#else
			else if (blind) msg_format("%^s mumbles, and then cackles evilly.", m_name);
#endif

#ifdef JP
			else msg_format("%^s���������������B����̒n�ʂɉ��������ꂽ�C������B", m_name);
#else
			else msg_format("%^s casts a spell and cackles evilly.", m_name);
#endif

			learn_spell(MS_MAKE_TRAP);
			nue_check_mult = 30;
			(void)trap_creation(y, x);

			hack_flag_enoko_make_beartrap = FALSE;

			break;
		}

		/* RF6_FORGET */
		case 160+14:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
msg_format("%^s�����Ȃ��̋L�����������悤�Ƃ��Ă���B", m_name);
#else
			msg_format("%^s tries to blank your mind.", m_name);
#endif

			if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif

			}
			else if (lose_all_info())
			{
#ifdef JP
msg_print("�L��������Ă��܂����B");
#else
				msg_print("Your memories fade away.");
#endif

			}
			learn_spell(MS_FORGET);
			nue_check_mult = 50;
			break;
		}

		/* RF6_COSMIC_HORROR */
		///mod �R�Y�~�b�N�E�z���[�ǉ�
		case 160+15:
		{
			disturb(1, 1);
			{
#ifdef JP
				if( m_ptr->r_idx == MON_KOISHI) msg_format("%^s�����Ȃ��̖ڂ�[���`�����񂾁c", m_name);
				else if (m_ptr->r_idx == MON_ZANMU) msg_format("%^s�ƑΛ����Ă���Ɠr�����Ȃ��������ɏP��ꂽ�I", m_name);
				else	msg_format("%^s�̑傢�Ȃ鈫�ӂ����Ȃ��̐��_�𗍂ߎ�����c", m_name);
#endif

			}

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_COSMIC_HORROR, dam, 0, FALSE, MS_COSMIC_HORROR, learnable);
			//update_smart_learn(m_idx, DRS_INSANITY);
			break;
		}

		/* RF6_S_KIN */
		///sys mon �~�������H�����X�^�[���Ƃɂ��낢����ꏈ��
		case 160+16:
		{
			disturb(1, 1);
			//if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
			if (m_ptr->r_idx == MON_SERPENT)
			{
#ifdef JP
				if (blind)
					msg_format("%^s���������Ԃ₢���B", m_name);
				else
					msg_format("%^s�����ׂ̋��͂ȑ��݂����������B", m_name);
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons guardians of dungeons.", m_name);
#endif
			}
			else if(m_ptr->r_idx == MON_ALICE)
			{
				if (blind)	msg_format("%^s���������Ԃ₢���B", m_name);
				else	msg_format("%^s���l�`�B���Ăяo�����B", m_name);
			}

			else if(m_ptr->r_idx == MON_YUKARI)
			{
				if (blind)	msg_format("%^s�͎��̖����Ă񂾁B", m_name);
				else	msg_format("%^s�������Ăяo�����B", m_name);
			}
			else if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
			{
				if (blind)	msg_format("%^s�͉��҂��֐����������B", m_name);
				else	msg_format("%^s���艺�ƒ��Ԃ��Ă񂾁I", m_name);
			}

			else if(m_ptr->r_idx == MON_SUIKA)
			{
				if (blind)	msg_format("�}�ɕӂ肪���L���Ȃ����B");
				else	msg_format("%^s�͂��Ȃ��ɔ��̖т𐁂��t�����E�E", m_name);
			}
			else if(m_ptr->r_idx == MON_STORMBRINGER)
			{
				if (blind)	msg_format("%^s�����������ԂƁA�ӂ�Ɉٗl�ȋC�z���������B", m_name);
				else	msg_format("%^s�͍��̌��̕��g���Ăяo�����I", m_name);
			}
			else if(m_ptr->r_idx == MON_NAPPA)
			{
				if (blind)	msg_format("�n�ʂ��@��悤�ȉ������������B");
				else	msg_format("%^s�͒n�ʂɉ�����A�����E�E", m_name);
			}
			else if (m_ptr->r_idx == MON_MAYUMI || m_ptr->r_idx == MON_KEIKI)
			{
				if (blind)	msg_format("%^s�����������ԂƁA�ӂ�Ɉٗl�ȋC�z���������B", m_name);
				else	msg_format("%^s�͔z���̏��֒B���Ăяo�����B", m_name);
			}
			else if (m_ptr->r_idx == MON_LUNASA || m_ptr->r_idx == MON_MERLIN || m_ptr->r_idx == MON_LYRICA)
			{
				if (blind)	msg_format("�ˑR�A�b�v�e���|�Ȃ���j�q���Ȏ����ς̂��鉹�y�ɕ�܂ꂽ�B");
				else	msg_format("%^s�͎o�����Ăяo�����B", m_name);
			}

			else
			{
#ifdef JP
				if (blind)
					msg_format("%^s���������Ԃ₢���B", m_name);
				else
					msg_format("%^s�͖��@��%s�����������B",
					m_name,
					((r_ptr->flags1) & RF1_UNIQUE ?
					"�艺" : "����"));
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons %s %s.",
					m_name, m_poss,
					((r_ptr->flags1) & RF1_UNIQUE ?
					"minions" : "kin"));
#endif
			}

			switch (m_ptr->r_idx)
			{
			case MON_MENELDOR:
			case MON_GWAIHIR:
			case MON_THORONDOR:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_EAGLES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
				}
				break;
			case MON_MAMIZOU:
				{
					int num = randint1(5);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_TANUKI, 0);
					}
					if(!one_in_(3))	count += summon_named_creature(m_idx, y, x, MON_O_TANUKI, 0);

				}
				break;
			case MON_HECATIA1:
			case MON_HECATIA2:
			case MON_HECATIA3:
				{
					count += summon_named_creature(m_idx, y, x, MON_CLOWNPIECE, 0);
					count += summon_named_creature(m_idx, y, x, MON_JUNKO, 0);
				}
			break;

			case MON_ALICE:
				{
					int num = 1;
					if(dun_level > 39) num++;
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_DOLLS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
				}
				break;
			case MON_ORIN:
			{
				if(one_in_(2))
				{
					num = 5 + randint1(10);
					for (k = 0; k < num; k++)
						count += summon_named_creature(m_idx, y, x, MON_ZOMBIE_FAIRY, 0);
				}
				else
				{
					num = 5 + randint1(10);
					count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					if(one_in_(2))count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					for (k = 0; k < num; k++)
					count += summon_named_creature(m_idx, y, x, MON_ONRYOU, 0);

				}
				break;

			}
			case MON_YUKARI:
				{
					int j, idx=0;
					for (j = 1; j < m_max; j++) if(m_list[j].r_idx == MON_RAN) idx = j;

					if(idx) teleport_monster_to(idx, y, x, 100, TELEPORT_PASSIVE);
					else count +=  summon_named_creature(m_idx, y, x, MON_RAN, 0);
				}
				break;
			case MON_PATCHOULI:
			case MON_MARISA:
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_LIB_GOLEM, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

				}
				break;
			case MON_SUIKA:
				{
					int k;
					int num = 5 + randint0(6);
					for (k = 0; k < num; k++)
					{
						count +=  summon_named_creature(m_idx, y, x, MON_MINI_SUIKA, 0);
					}
				}
				break;
			case MON_EIRIN:
			case MON_TEWI:
				{
					int k;
					int num = 5 + randint0(6);
					for (k = 0; k < num; k++)
					{
						count +=  summon_named_creature(m_idx, y, x, MON_YOUKAI_RABBIT2, 0);
					}
				}
				break;
			case MON_TENSHI:
				{
					int k;
					int num = 3 + randint1(5);
					for (k = 0; k < num; k++)
					{
						count +=  summon_named_creature(m_idx, y, x, MON_KANAME, 0);
					}
				}
				break;

			case MON_GREATER_DEMON:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_GREATER_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}

				}
				break;
			case MON_KOSUZU:
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_TSUKUMO, 0);
					}

				}
				break;

			case MON_BULLGATES:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_IE, mode);
					}
				}
				break;

			case MON_SERPENT:
			//case MON_ZOMBI_SERPENT:
				{
	/*
					int num = 2 + randint1(3);

					if (r_info[MON_JORMUNGAND].cur_num < r_info[MON_JORMUNGAND].max_num && one_in_(6))
					{
#ifdef JP
						msg_print("�n�ʂ��琅�������o�����I");
#else
						msg_print("Water blew off from the ground!");
#endif
						fire_ball_hide(GF_WATER_FLOW, 0, 3, 8);
					}

					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					}
	*/
					count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
					count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
				break;

			case MON_CALDARM:
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_LOCKE_CLONE, mode);
					}
				}
				break;

			case MON_LOUSY:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_LOUSE, PM_ALLOW_GROUP);
					}
				}
				break;

			case MON_AZATHOTH:
				{
					int num = randint1(5);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_INSANITY3, PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);
					}
				}
				break;
			case MON_MORGOTH:
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_MORGOTH_MINION, PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);
					}
				}
				break;
			//v1.1.32
			case MON_NARUMI:
				{
					count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_BULLET_GOLEM, 0);
				}
				break;
			case MON_STORMBRINGER:
				{
					int num = 1 + randint1(4);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_CHAOS_BLADE, mode);
					}
				}
				break;
			case MON_NAZRIN:
			case MON_HAKUSEN:
				{
					int num = 1 + randint1(5);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_RAT, PM_ALLOW_GROUP);
					}
				}
				break;
			case MON_NAPPA:
				{
					int num = randint1(6);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_SAIBAIMAN, mode);
					}
				}
				break;
			case MON_MAYUMI:
			case MON_KEIKI:
			{
				int num = 4 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, m_ptr->fy, m_ptr->fx, rlev, SUMMON_HANIWA, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_FORCE_ENEMY));
				}
			}
			break;

			case MON_LUNASA:
			case MON_MERLIN:
			case MON_LYRICA:
			{

				count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_LUNASA, 0);
				count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_MERLIN, 0);
				count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_LYRICA, 0);
			}
			break;

			default:
				summon_kin_type = r_ptr->d_char; /* Big hack */

				for (k = 0; k < 4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_KIN, PM_ALLOW_GROUP);
				}
				break;
			}
#ifdef JP
			if (blind && count) msg_print("�����̂��̂��ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}

		/* RF6_S_CYBER */
		case 160+17:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���T�C�o�[�f�[���������������I", m_name);
#else
			else msg_format("%^s magically summons Cyberdemons!", m_name);
#endif

			count += summon_cyber(m_idx, y, x);

#ifdef JP
if (blind && count) msg_print("�d���ȑ������߂��ŕ�������B");
#else
			if (blind && count) msg_print("You hear heavy steps nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_MONSTER */
		case 160+18:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�Œ��Ԃ����������I", m_name);
#else
			else msg_format("%^s magically summons help!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
if (blind && count) msg_print("�������ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_MONSTERS */
		case 160+19:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�Ń����X�^�[�����������I", m_name);
#else
			else msg_format("%^s magically summons monsters!", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
if (blind && count) msg_print("�����̂��̂��ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_ANT */
		case 160+20:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�ŃA�������������B", m_name);
#else
			else msg_format("%^s magically summons ants.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANT, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�����̂��̂��ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_SPIDER */
		case 160+21:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�Œ������������B", m_name);
#else
			else msg_format("%^s magically summons spiders.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_SPIDER, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�����̂��̂��ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_HOUND */
		case 160+22:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�Ńn�E���h�����������B", m_name);
#else
			else msg_format("%^s magically summons hounds.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HOUND, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�����̂��̂��ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_HYDRA */
		case 160+23:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�Ńq�h�������������B", m_name);
#else
			else msg_format("%^s magically summons hydras.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HYDRA, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�����̂��̂��ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_ANGEL */
		case 160+24:
		{
			int num = 1;

			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�œV�g�����������I", m_name);
#else
			else msg_format("%^s magically summons an angel!", m_name);
#endif

			if ((r_ptr->flags1 & RF1_UNIQUE) && !easy_band)
			{
				num += rlev/40;
			}

			for (k = 0; k < num; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANGEL, PM_ALLOW_GROUP);
			}

			if (count < 2)
			{
#ifdef JP
if (blind && count) msg_print("�������ԋ߂Ɍ��ꂽ��������B");
#else
				if (blind && count) msg_print("You hear something appear nearby.");
#endif
			}
			else
			{
#ifdef JP
if (blind) msg_print("�����̂��̂��ԋ߂Ɍ��ꂽ��������B");
#else
				if (blind) msg_print("You hear many things appear nearby.");
#endif
			}
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_DEMON */
		case 160+25:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			///msg131221
			else msg_format("%^s�͈��������������I", m_name);
#else
			else msg_format("%^s magically summons a demon from the Courts of Chaos!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DEMON, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�������ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_UNDEAD */
		case 160+26:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�ŃA���f�b�h�̋��G�����������I", m_name);
#else
			else msg_format("%^s magically summons an undead adversary!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_UNDEAD, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�������ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_DRAGON */
		case 160+27:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�Ńh���S�������������I", m_name);
#else
			else msg_format("%^s magically summons a dragon!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DRAGON, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�������ԋ߂Ɍ��ꂽ��������B");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif
			if(!blind && !count) msg_print("��������������Ȃ������B");

			break;
		}

		/* RF6_S_HI_UNDEAD */
		case 160+28:
		{
			disturb(1, 1);

			///mod140726 �����S�X�폜�@�����S�X�͋~�������Ńi�Y�O���Ƃ��o�����O�Ƃ��S���Ă�
			if (((m_ptr->r_idx == MON_SAURON) || (m_ptr->r_idx == MON_ANGMAR)) && ((r_info[MON_NAZGUL].cur_num+2) < r_info[MON_NAZGUL].max_num))
			{
				int cy = y;
				int cx = x;

#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�ŗH�S��������������I", m_name);
#else
				else msg_format("%^s magically summons rangers of Nazgul!", m_name);
#endif
				msg_print(NULL);

				for (k = 0; k < 30; k++)
				{
					if (!summon_possible(cy, cx) || !cave_empty_bold(cy, cx))
					{
						int j;
						for (j = 100; j > 0; j--)
						{
							scatter(&cy, &cx, y, x, 2, 0);
							if (cave_empty_bold(cy, cx)) break;
						}
						if (!j) break;
					}
					if (!cave_empty_bold(cy, cx)) continue;

					if (summon_named_creature(m_idx, cy, cx, MON_NAZGUL, mode))
					{
						y = cy;
						x = cx;
						count++;
						if (count == 1)
#ifdef JP
msg_format("�u�H�S���%d���A�i�Y�O���E�u���b�N�I�v", count);
#else
							msg_format("A Nazgul says 'Nazgul-Rangers Number %d, Nazgul-Black!'",count);
#endif
						else
#ifdef JP
msg_format("�u������%d���A�i�Y�O���E�u���b�N�I�v", count);
#else
							msg_format("Another one says 'Number %d, Nazgul-Black!'",count);
#endif
						msg_print(NULL);
					}
				}
#ifdef JP
msg_format("�u%d�l������āA�����O�����W���[�I�v", count);
#else
msg_format("They say 'The %d meets! We are the Ring-Ranger!'.", count);
#endif
				msg_print(NULL);
			}
			else
			{
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�ŋ��͂ȃA���f�b�h�����������I", m_name);
#else
				else msg_format("%^s magically summons greater undead!", m_name);
#endif

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			if (blind && count)
			{
#ifdef JP
msg_print("�ԋ߂ŉ��������̂��̂�������鉹����������B");
#else
				msg_print("You hear many creepy things appear nearby.");
#endif

			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}

		/* RF6_S_HI_DRAGON */
		case 160+29:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�ŌÑ�h���S�������������I", m_name);
#else
			else msg_format("%^s magically summons ancient dragons!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("�����̗͋������̂��ԋ߂Ɍ��ꂽ������������B");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}

		/* RF6_S_AMBERITES */
		case 160+30:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���A���o�[�̉��������������I", m_name);
#else
			else msg_format("%^s magically summons Lords of Amber!", m_name);
#endif



			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("�͋����������߂��Ɍ����̂����������B");
#else
				msg_print("You hear immortal beings appear nearby.");
#endif

			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}

		/* RF6_S_UNIQUE */
		case 160+31:
		{
			bool uniques_are_summoned = FALSE;
			int non_unique_type = SUMMON_HI_UNDEAD;

			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����@�œ��ʂȋ��G�����������I", m_name);
#else
			else msg_format("%^s magically summons special opponents!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}

			if (count) uniques_are_summoned = TRUE;
			///sys ���j�[�N�����̐��͔���
			if ((m_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
				non_unique_type = 0;
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD)
				non_unique_type = SUMMON_ANGEL;

			for (k = count; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, non_unique_type, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}

			if (blind && count)
			{
#ifdef JP
				msg_format("������%s���ԋ߂Ɍ��ꂽ������������B", uniques_are_summoned ? "�͋�������" : "����");
#else
				msg_format("You hear many %s appear nearby.", uniques_are_summoned ? "powerful things" : "things");
#endif
			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}
///mod ���������V�X�y���ǉ���
///�����X�^�[�̐V�X�y���̓��[�j���O�s��

		/* RF9_FIRE_STORM */
		case 192+0:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s�����̗��̎������������B", m_name);
#endif
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_FIRE, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}
		/* RF9_ICE_STORM */
		case 192+1:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s���X�̗��̎������������B", m_name);
#endif
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_COLD, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}
		/* RF9_THUNDER_STORM */
		case 192+2:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s�����̗��̎������������B", m_name);
#endif
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_ELEC, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}
		/* RF9_ACID_STORM */
		case 192+3:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s���_�̗��̎������������B", m_name);
#endif
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			breath(y, x, m_idx, GF_ACID, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF9_TOXIC_STORM */
		case 192+4:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s���őf�̗��̎������������B", m_name);
#endif
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 8) + 50 + randint1(50);
			else	dam = (rlev * 5) + 25 + randint1(25);
			breath(y, x, m_idx, GF_POIS, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF9_BA_POLLUTE */
		case 192+5:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s����������ȉ��𗧂Ă��B", m_name);
			else msg_format("%^s�������̋��̎������������B", m_name);
#endif
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 50 + damroll(10, 10);
			else	dam = (rlev * 3) + 50 + randint1(50);
			breath(y, x, m_idx, GF_POLLUTE, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_POIS);
			update_smart_learn(m_idx, DRS_DISEN);
			break;
		}


		/* RF9_BA_DISI */
		case 192+6:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������G�Ȏ������������B�ˑR���ӈ�т̕������������C������I", m_name);
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format("%^s�͂ǂ����炩����Ȕ��e�����o�����I", m_name);
			else msg_format("%^s�����q�����̎������������B", m_name);
#endif
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 2) + 50 + randint1(50);
			else	dam = rlev + 25 + randint1(25);
			breath(y, x, m_idx, GF_DISINTEGRATE, dam, rad, FALSE, 0, FALSE);
			//update_smart_learn(m_idx, DRS_POIS);

			//v1.1.58 ���̍U���ŏ��߂Ď��E���ʂ�����������Ȃ��̂�BGM���ă`�F�b�N
			check_music_mon_r_idx(m_idx);

			break;
		}

		/*:::RF9_BA_HOLY*/
		case 192+7:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������r�������B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���j�ׂ̌����̎������������B", m_name);
#else
			else msg_format("%^s casts a nether ball.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			breath(y, x, m_idx, GF_HOLY_FIRE, dam, 2, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 500;
			break;
		}

		/* RF9_METEOR */
		case 192+8:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������G�Ȏ������������B���������ォ��~���Ă����I", m_name);
			else msg_format("%^s�����e�I�X�g���C�N�̎������������I", m_name);
#endif


			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 5;
				//v2.0 ������̉��@�ǔ����Ōy���ł��Ȃ�����w���t�@�C�A�Ƃ������Ғl���Ⴍ�Ă�����
				//dam = (rlev * 4) + damroll(1, 300);
				dam = (rlev * 3) + damroll(1,250);
			}
			else
			{
				rad = 4;
				dam = (rlev * 2) + damroll(1,150);
			}
			breath(y, x, m_idx, GF_METEOR, dam, rad, FALSE, 0, FALSE);
			break;
		}

		/*:::RF9_BA_DISTORTION*/
		case 192+9:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����Ԙc�Ȃ̎������������B", m_name);
#else
			else msg_format("%^s casts a nether ball.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 1 : 2));
			breath(y, x, m_idx, GF_DISTORTION, dam, 4, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_TIME);
			break;
		}

		/* RF9_PUNISH_1 */
		case 192+10:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ��ɖ������̂܂��Ȃ����������B", m_name);
#endif

			dam = damroll(3, 8);
			breath(y, x, m_idx, GF_PUNISH_1, dam, 0, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 300;
			break;
		}

		/* RF9_PUNISH_2 */
		case 192+11:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ��Ɍ������Đ��Ȃ錾�t���������B", m_name);
#endif

			dam = damroll(8, 8);
			breath(y, x, m_idx, GF_PUNISH_2, dam, 0, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 300;

			break;
		}

		/* RF9_PUNISH_3 */
		case 192+12:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������吺�ŋ��񂾁B", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ����w�����đޖ��̎������������B", m_name);
#endif

			dam = damroll(10, 15);
			breath(y, x, m_idx, GF_PUNISH_3, dam, 0, FALSE, 0, FALSE);
			nue_check_mult = 400;
			update_smart_learn(m_idx, DRS_HOLY);
			break;
		}

		/* RF9_CAUSE_4 */
		case 192+13:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
			///msg131221
			if (blind) msg_format("%^s�������_�X�������t�����񂾁B", m_name);
#endif

#ifdef JP
			///msg131221
			else msg_format("%^s�����Ȃ��Ɍ����Ĕj�ׂ̈��������I", m_name);
#endif

			dam = damroll(15, 15);
			breath(y, x, m_idx, GF_PUNISH_4, dam, 0, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 400;
			break;
		}

		/* RF9_BO_HOLY */
		case 192+14:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ȃ��̎������������B", m_name);
#endif

			dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_HOLY_FIRE, dam, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			update_smart_learn(m_idx, DRS_REFLECT);
			nue_check_mult = 300;
			break;
		}
		/* RF9_GIGANTIC_LASER*/
		case 192+15:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s����������˂����I", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
			else if(m_ptr->r_idx == MON_MARISA)msg_format("%^s���}�X�^�[�X�p�[�N��������I", m_name);
			else msg_format("%^s������ȃ��[�U�[��������I", m_name);
#endif

			dam = damroll(10, 10) +  rlev * (((r_ptr->flags2 & RF2_POWERFUL) ? 8 : 4));
			masterspark(y, x, m_idx, GF_NUKE, dam, 0, FALSE,((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));	
			update_smart_learn(m_idx, DRS_LITE);
			update_smart_learn(m_idx, DRS_FIRE);
			nue_check_mult = 300;
			break;
		}


		/* RF9_BO_SOUND */
		case 192+16:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���傫�ȉ���������B", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������̒e��������B", m_name);
#endif
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = damroll(10, 10) + (rlev * 3 / 2);
			else	dam = damroll(3, 10) + rlev;

			bolt(m_idx, GF_SOUND, dam, 0, FALSE);
			update_smart_learn(m_idx, DRS_REFLECT);
			update_smart_learn(m_idx, DRS_SOUND);
			break;
		}


		/* RF9_S_ANIMAL */
		///mod140419 ���������@
		case 192+17:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#endif

#ifdef JP
else msg_format("%^s�����������������I", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANIMAL, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("���Ȃ����֖҂ȚX�萺�ɕ�܂ꂽ�B");
#endif
			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}


		/* RF9_BEAM_LITE */
		case 192+18:
		{
			if (!direct) return (FALSE);
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s��������������B", m_name);
else msg_format("%^s�����[�U�[��������B", m_name);
#endif
			dam = (r_ptr->flags2 & RF2_POWERFUL) ? (rlev * 4) : (rlev * 2);
			beam(m_idx, GF_LITE, dam, 0, FALSE);
			update_smart_learn(m_idx, DRS_LITE);
			nue_check_mult = 300;
			break;
		}


		/* RF9_HELLFIRE */
		case 192+20:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s�������ЁX�������G�Ȏ������r�������B", m_name);
			else msg_format("%^s���w���E�t�@�C�A�̎������r�������B", m_name);
#endif
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);
			breath(y, x, m_idx, GF_HELL_FIRE, dam, rad, FALSE, 0, FALSE);
			//update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF9_HOLYFIRE */
		case 192+21:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s�������_�X�������G�Ȏ������r�������B", m_name);
			else msg_format("%^s���z�[���[�E�t�@�C�A�̎������r�������B", m_name);
#endif
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);
			breath(y, x, m_idx, GF_HOLY_FIRE, dam, rad, FALSE, 0, FALSE);
			update_smart_learn(m_idx, DRS_HOLY);
			nue_check_mult = 800;
			break;
		}
		/* RF9_FINALSPARK*/
		case 192+22:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s������ȃG�l���M�[����˂����I", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���t�@�C�i���X�p�[�N��������I", m_name);
#endif

			dam = 200 + randint1(rlev) +  rlev * 2;
			masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, 0, FALSE,2);	
			nue_check_mult = 300;

			/*:::Hack - �t�@�C�i���X�p�[�N������́����s������܂Ŗ��@���g��Ȃ�*/
			m_ptr->mflag |= (MFLAG_NICE);
			repair_monsters = TRUE;

			//v1.1.58 ���̍U���ŏ��߂Ď��E���ʂ�����������Ȃ��̂�BGM���ă`�F�b�N
			check_music_mon_r_idx(m_idx);

			break;
		}


		/* RF9_TORNADO */
		case 192+23:
		{
			disturb(1, 1);
#ifdef JP
			if (blind) msg_format("%^s���������N�������B", m_name);
			else msg_format("%^s���������N�������B", m_name);
#endif
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 3) + 50 + randint1(50);
			else	dam = (rlev * 2) + 25 + randint1(25);
			breath(y, x, m_idx, GF_TORNADO, dam, rad, FALSE, 0, FALSE);
			break;
		}


		///pend �����X�^�[��*�j��*���@�@�ڍז����� ���󁗂̑����̒n�`���j�󂳂�Ȃ����ߕʏ������v�邩��

		/* RF9_DESTRUCTION */
		case 192+24:
		{

			disturb(1, 1);
#ifdef JP
			if(m_ptr->cdis > MAX_SIGHT) msg_format("����Ȕ������������A��u�_���W�������h�ꂽ�B");
			else if (blind) msg_format("%^s��������������ƒn�����������c", m_name);
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format("%^s�͂ǂ����炩����Ȕ��e�����o�����I", m_name);
			else msg_format("%^s���j��̌��t���r�������I", m_name);
#endif
			if(destroy_area(m_ptr->fy, m_ptr->fx,10,FALSE,FALSE,FALSE))
			{
				if(distance(py, px, m_ptr->fy, m_ptr->fx)<10)
				{
					if(check_activated_nameless_arts(JKF1_DESTRUCT_DEF))
					{
						msg_format("���Ȃ��͔j��̗͂𕨂Ƃ����Ȃ������I");
						break;
					}
					dam = p_ptr->chp / 2;
					if(p_ptr->pclass != CLASS_CLOWNPIECE) 
					{
						//v1.1.86 NOESCAPE��ATTACK��
						take_hit(DAMAGE_ATTACK, dam, m_name, -1);
					}
					if(p_ptr->pseikaku != SEIKAKU_BERSERK || one_in_(7))
					{
						msg_format("���Ȃ��͐�����΂��ꂽ�I");
						teleport_player_away(m_idx, 100);
					}
					nue_check_mult = 10;
				}
			}

			break;
		}
		/* RF9_S_DEITY */
		///mod140228 �_�i�����@
		case 192+25:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#endif

#ifdef JP
else msg_format("%^s���_�i�����������I", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DEITY, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("���Ȃ��̎��͈͂ٗl�ȋ�C�ɖ������ꂽ�B");
#endif
			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}

		/* RF9_S_HI_DEMON */
		///mod140228 ��ʈ��������@
		case 192+26:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#endif

#ifdef JP
else msg_format("%^s����ʈ��������������I", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DEMON, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("���Ȃ��̎��͎͂׈��Ȃ���߂��ɖ������ꂽ�B");
#endif
			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}

		/* RF9_S_KWAI */
		///mod140118 �d�������@��{�I�Ƀ��j�[�N�͏o�Ȃ����Ƃɂ��Ă���
		case 192+27:
		{
			disturb(1, 1);
#ifdef JP
if (blind) msg_format("%^s���������Ԃ₢���B", m_name);
#endif

#ifdef JP
else msg_format("%^s���d�������������I", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_KWAI, (PM_ALLOW_GROUP));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("���Ȃ��̎��͖͂ʗd�Ȃ���߂��ɖ������ꂽ�B");
#endif
			}
			if(!blind && !count) msg_print("��������������Ȃ������B");
			break;
		}



		/* RF9_TELE_APPROACH */
		case 192+28:
		{
			if (!direct) return (FALSE);
			if (teleport_barrier(m_idx)) break;

			disturb(1, 1);
//			teleport_monster_to(m_idx, y, x, 100, 0L);	
			teleport_monster_to(m_idx, y, x, 100, TELEPORT_FORCE_NEXT);	

#ifdef JP
msg_format("%s���u���Ɋԍ������l�߂Ă����B", m_name);
#endif
			break;

		}
		/* RF9_TELE_HI_APPROACH */
		case 192+29:
		{

			if ((p_ptr->special_flags & SPF_ORBITAL_TRAP) && is_hostile(m_ptr) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (seen)
					msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", m_name);

				delete_monster_idx(m_idx);
				return TRUE;

			}

			if (teleport_barrier(m_idx)) break;

			disturb(1, 1);
			teleport_monster_to(m_idx, y, x, 100, 0L);
			///mod150926 ���E�O�e���|����̘A���s�����@�̉\�������炵��
			if (!one_in_(6) && !ironman_nightmare)
			{
				m_ptr->mflag |= (MFLAG_NICE);
				repair_monsters = TRUE;
			}

			if (m_ptr->r_idx == MON_KOISHI && p_ptr->pclass != CLASS_KOISHI)
			{
				msg_format("�������c�C�̂������H");
			}
			else if (m_ptr->r_idx == MON_MURASA)
			{
#ifdef JP
				msg_format("%s�����̒����猻�ꂽ�I", m_name);
#endif
			}
			else if (distance(y, x, m_ptr->fy, m_ptr->fx) < 3)
			{
#ifdef JP
				msg_format("%s���ˑR���Ȃ��̋߂��Ɍ��ꂽ�B", m_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%s���u���Ɉړ������B", m_name);
#endif
			}

			//v1.1.58 ���̍U���ŏ��߂Ď��E���ʂ�����������Ȃ��̂�BGM���ă`�F�b�N
			check_music_mon_r_idx(m_idx);

			break;
		}
		/* RF9_MAELSTROM*/
		case 192+30:
		{
			int rad;
	//		if (!direct) return (FALSE);
			disturb(1, 1);

			//v1.1.91 ���ꏈ���@�ޖ��̃��C���V���g�����͐Ζ��n�`�����

			if (blind)
				msg_format("�ˑR��Q�ɓۂݍ��܂ꂽ�I", m_name);
			else if (m_ptr->r_idx == MON_YUMA)
				msg_format("%^s���Ζ��̑�Q�������N�������I",m_name);
			else 
				msg_format("%^s�����C���V���g�����̎������r�������B", m_name);

			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				dam = (rlev * 8) + randint1(rlev * 6);
				rad = 8;
			}
			else
			{
				dam = (rlev * 5) + randint1(rlev * 3);
				rad = 6;
			}
			if (m_ptr->r_idx == MON_YUMA)
			{
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, dam, GF_WATER, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP | PROJECT_HIDE), FALSE);
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, dam, GF_DIG_OIL, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP), FALSE);
			}
			else
			{
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, dam, GF_WATER, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), FALSE);
				(void)project(m_idx, rad, m_ptr->fy, m_ptr->fx, rad, GF_WATER_FLOW, (PROJECT_GRID | PROJECT_ITEM | PROJECT_JUMP | PROJECT_HIDE), FALSE);
			}
			update_smart_learn(m_idx, DRS_WATER);
			break;
		}

		/* RF9_ALARM*/
		case 192+31:
		{
			disturb(1, 1);
#ifdef JP
			if(m_ptr->r_idx == MON_CLOWNPIECE)
				msg_format("%^s�̎����������X�ƋP�����I", m_name);
			else
				msg_format("%^s�͋���Ȍx�񉹂𔭂����I", m_name);

#else
			msg_format("%^s makes a high pitched shriek.", m_name);
#endif

			aggravate_monsters(m_idx,TRUE);
			break;
		}

//v1.1.58 �ςȊ��ʂ��������̂ō폜�@������Ԉ���Ă����H
//		}




	}
	/*:::���@�g�p�I���@���@���g�p���������X�^�[�����̎��_�Ŏ���ł邱�Ƃ�����̂Œ���*/

	//v1.1.73 ����d�̃����X�^�[�����t���O����
	flag_bribe_summon_monsters = FALSE;

///class ���̂܂˂Ɛ��̓��ꏈ���炵��
	if ((p_ptr->action == ACTION_LEARN) && thrown_spell > 175)
	{
		learn_spell(thrown_spell - 96);
	}

	//v1.1.82 �V�E�Ɓu�e�������Ɓv�̓��Z�K������
	if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		//���̎��_�œ��Z�g�p�����X�^�[�����Ȃ��ꍇ�A�������ʓ|�ɂȂ�̂ŏK���s��
		if (m_ptr->r_idx)
		{
			learn_monspell_new(thrown_spell, m_ptr, TRUE, learnable,caster_dist);

		}

	}

	if (seen && maneable && !world_monster && (p_ptr->pclass == CLASS_IMITATOR))
	{
		if (thrown_spell != 167) /* Not RF6_SPECIAL */
		{
			if (p_ptr->mane_num == MAX_MANE)
			{
				int i;
				p_ptr->mane_num--;
				for (i = 0;i < p_ptr->mane_num;i++)
				{
					p_ptr->mane_spell[i] = p_ptr->mane_spell[i+1];
					p_ptr->mane_dam[i] = p_ptr->mane_dam[i+1];
				}
			}
			p_ptr->mane_spell[p_ptr->mane_num] = thrown_spell - 96;
			p_ptr->mane_dam[p_ptr->mane_num] = dam;
			p_ptr->mane_num++;
			new_mane = TRUE;

			p_ptr->redraw |= (PR_IMITATION);
		}
	}


	//�ʂ����ꏈ���@�s����̃����X�^�[�����̂��Ŕj�� ���̃����X�^�[����������ł�Ƃ��͖���
	if (p_ptr->pclass == CLASS_NUE && m_ptr->r_idx)
	{
		check_nue_revealed(m_idx, nue_check_mult);

	}

	/* Remember what the monster did to us */
	/*:::���̎��E���Ń����X�^�[���g�����������L�����镔��*/
	if (can_remember)
	{
		/* Inate spell */
		if (thrown_spell < 32 * 4)
		{
			r_ptr->r_flags4 |= (1L << (thrown_spell - 32 * 3));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Bolt or Ball */
		else if (thrown_spell < 32 * 5)
		{
			r_ptr->r_flags5 |= (1L << (thrown_spell - 32 * 4));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Special spell */
		else if (thrown_spell < 32 * 6)
		{
			r_ptr->r_flags6 |= (1L << (thrown_spell - 32 * 5));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
		else if (thrown_spell < 32 * 7)
		{
			r_ptr->r_flags9 |= (1L << (thrown_spell - 32 * 6));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/*:::�u�͋��������X�^�[�v�̊w�K�@�Ƃ肠�������ł��ڂ̑O�Ŗ��@�g������킩�邱�Ƃɂ��Ă���*/
		if(r_ptr->flags2 & RF2_POWERFUL) r_ptr->r_flags2 |= RF2_POWERFUL;


	}

	///mod140721 ���x�ł̓����X�^�[�͖��@��A���g�p���Ȃ�
	if(difficulty <= DIFFICULTY_NORMAL && m_ptr->r_idx)
	{
		m_ptr->mflag |= (MFLAG_NICE);
		repair_monsters = TRUE;
	}

	kyouko_echo(FALSE,thrown_spell);

	/* Always take note of monsters that kill you */
	if (p_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !p_ptr->inside_arena)
	{
		r_ptr->r_deaths++; /* Ignore appearance difference */
	}
	//�������������X�^�[������(@�����񂾂�_���v�ɖ��O�o�����߂ɏ������)
	if(suisidebomb && !p_ptr->is_dead ) delete_monster_idx(m_idx);

	/* A spell was cast */
	return (TRUE);
}


