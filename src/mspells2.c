/* File: mspells2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Monster spells (attack monster) */

#include "angband.h"


/*
 * Monster casts a breath (or ball) attack at another monster.
 * Pass over any monsters that may be in the way
 * Affect grids, objects, monsters, and the player
 */
///sys mon �j�׌n�̎􂢂ȂǓ��ˈ����̍U������������Ƃ��ύX�v
static void monst_breath_monst(int m_idx, int y, int x, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Determine the radius of the blast */
	if (rad < 1 && breath) rad = (r_ptr->flags2 & RF2_POWERFUL) ? 3 : 2;

	/* Handle breath attacks */
	if (breath) rad = 0 - rad;

	switch (typ)
	{
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
		flg |= (PROJECT_HIDE | PROJECT_AIMED);

	case GF_METEOR:
	case GF_TORNADO:
		flg |= PROJECT_JUMP;
		break;

		break;
	}

	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*
 * Monster casts a bolt at another monster
 * Stop if we hit a monster
 * Affect monsters and the player
 */
static void monst_bolt_monst(int m_idx, int y, int x, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	if (typ != GF_ARROW) flg |= PROJECT_REFLECTABLE;
	(void)project(m_idx, 0, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

static void monst_beam_monst(int m_idx, int y, int x, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU;

	(void)project(m_idx, 0, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

/*
 * Determine if a beam spell will hit the target.
 */
static bool direct_beam(int y1, int x1, int y2, int x2, monster_type *m_ptr)
{
	bool hit2 = FALSE;
	int i, y, x;

	int grid_n = 0;
	u16b grid_g[2048];

	bool friend_ = is_pet(m_ptr);

	if(m_ptr->r_idx == MON_MASTER_KAGUYA) friend_ = TRUE;

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, PROJECT_THRU);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if (y == y2 && x == x2)
			hit2 = TRUE;
		else if (friend_ && cave[y][x].m_idx > 0 &&
			 !are_enemies(m_ptr, &m_list[cave[y][x].m_idx]))
		{
			/* Friends don't shoot friends */
			return FALSE;
		}

		if (friend_ && player_bold(y, x))
			return FALSE;
	}
	if (!hit2)
		return FALSE;
	return TRUE;
}

/*:::�w��O���b�h���u���X�̌��ʔ͈͂ɓ��邩���肷��炵��*/
static bool breath_direct(int y1, int x1, int y2, int x2, int rad, int typ, bool friend_)
{
	/* Must be the same as projectable() */

	int i;

	/* Initial grid */
	int y = y1;
	int x = x1;

	int grid_n = 0;
	u16b grid_g[2048];

	int grids = 0;
	byte gx[2048], gy[2048];
	byte gm[32];
	int gm_rad = rad;

	bool hit2 = FALSE;
	bool hityou = FALSE;

	int flg;

	switch (typ)
	{
	case GF_LITE:
	case GF_LITE_WEAK:
		flg = PROJECT_LOS;
		break;
	case GF_DISINTEGRATE:
	case GF_BLAZINGSTAR:
		flg = PROJECT_DISI;
		break;
	default:
		flg = 0;
		break;
	}

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, flg);

	/* Project along the path */
	for (i = 0; i < grid_n; ++i)
	{
		int ny = GRID_Y(grid_g[i]);
		int nx = GRID_X(grid_g[i]);

		if (flg & PROJECT_DISI)
		{
			/* Hack -- Balls explode before reaching walls */
			if (cave_stop_disintegration(ny, nx)) break;
		}
		else if (flg & PROJECT_LOS)
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_los_bold(ny, nx)) break;
		}
		else
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_have_flag_bold(ny, nx, FF_PROJECT)) break;
		}

		/* Save the "blast epicenter" */
		y = ny;
		x = nx;
	}

	grid_n = i;

	if (!grid_n)
	{
		if (flg & PROJECT_DISI)
		{
			if (in_disintegration_range(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (in_disintegration_range(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
		else if (flg & PROJECT_LOS)
		{
			if (los(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (los(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
		else
		{
			if (projectable(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (projectable(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
	}
	else
	{
		breath_shape(grid_g, grid_n, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, y, x, typ, FALSE);

		for (i = 0; i < grids; i++)
		{
			/* Extract the location */
			y = gy[i];
			x = gx[i];

			if ((y == y2) && (x == x2)) hit2 = TRUE;
			if (player_bold(y, x)) hityou = TRUE;
		}
	}

	if (!hit2) return FALSE;
	if (friend_ && hityou) return FALSE;

	return TRUE;
}

/*
 * Get the actual center point of ball spells (rad > 1) (originally from TOband)
 */
void get_project_point(int sy, int sx, int *ty, int *tx, int flg)
{
	u16b path_g[128];
	int  path_n, i;

	path_n = project_path(path_g, MAX_RANGE, sy, sx, *ty, *tx, flg);

	*ty = sy;
	*tx = sx;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		sy = GRID_Y(path_g[i]);
		sx = GRID_X(path_g[i]);

		/* Hack -- Balls explode before reaching walls */
		if (!cave_have_flag_bold(sy, sx, FF_PROJECT)) break;

		*ty = sy;
		*tx = sx;
	}
}

/*
 * Check should monster cast dispel spell at other monster.
 */
static bool dispel_check_monster(int m_idx, int t_idx)
{
	monster_type *t_ptr = &m_list[t_idx];

	/* Invulnabilty */
	if (MON_INVULNER(t_ptr)) return TRUE;

	/* Speed */
	if (t_ptr->mspeed < 135)
	{
		if (MON_FAST(t_ptr)) return TRUE;
	}

	/* Riding monster */
	if (t_idx == p_ptr->riding)
	{
		if (dispel_check(m_idx)) return TRUE;
	}

	/* No need to cast dispel spell */
	return FALSE;
}

/*
 * Monster tries to 'cast a spell' (or breath, etc)
 * at another monster.
 *
 * The player is only disturbed if able to be affected by the spell.
 */
/*:::�����X�^�[�������X�^�[�̖��@�U��*/
///sys mon �����X�^�[�Ԃ̍U�����@�@���ւ̂Ɠ��l��f9�t���O��������
///mod140424 �V�t���O�Ή�
bool monst_spell_monst(int m_idx)
{
	int y = 0, x = 0;
	int i, k, t_idx = 0;
	int thrown_spell=0, count = 0;
	int rlev;
	int dam = 0;
	int start;
	int plus = 1;
	u32b u_mode = 0L;
	int s_num_6 = (easy_band ? 2 : 6);
	int s_num_4 = (easy_band ? 1 : 4);
	int rad = 0; //For elemental balls

	byte spell[128], num = 0;

	char m_name[160];
	char t_name[160];

#ifndef JP
	char m_poss[160];
#endif

	monster_type *m_ptr = &m_list[m_idx];
	monster_type *t_ptr = NULL;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_race *tr_ptr = NULL;

	u32b f4, f5, f6, f9;

	bool wake_up = FALSE;
	bool fear = FALSE;

	bool blind = (p_ptr->blind ? TRUE : FALSE);

	bool see_m = is_seen(m_ptr);
	bool maneable = player_has_los_bold(m_ptr->fy, m_ptr->fx);
	bool learnable = (m_ptr->ml && maneable && !world_monster);

	int caster_dist = m_ptr->cdis;	//�����X�^�[�����@���g�����u�Ԃ́�����̋���

	bool see_t;
	bool see_either;
	bool known;
	bool suisidebomb = FALSE; //�����@�T�C�o�C�}���p

	bool pet = is_pet(m_ptr);

	bool in_no_magic_dungeon = (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && dun_level
		&& (!p_ptr->inside_quest || is_fixed_quest_idx(p_ptr->inside_quest));

	bool can_use_lite_area = FALSE;

	bool can_remember;

	bool resists_tele = FALSE;

	/* Prepare flags for summoning */
	if (!pet) u_mode |= PM_ALLOW_UNIQUE;

	//v1.1.95 ����m����Ԃ��Ɩ��@�����s�����g���Ȃ�
	if (MON_BERSERK(m_ptr))	return (FALSE);

	/* Cannot cast spells when confused */
	if (MON_CONFUSED(m_ptr)) return (FALSE);

	//�}�~�]�E��b��Ƃ̍ق�
	if(p_ptr->pclass == CLASS_MAMIZOU && p_ptr->magic_num1[0] == m_idx)
		return (FALSE);

	/* Extract the racial spell flags */
	f4 = r_ptr->flags4;
	f5 = r_ptr->flags5;
	f6 = r_ptr->flags6;
	f9 = r_ptr->flags9;

	/* Target is given for pet? */
	if (pet_t_m_idx && pet)
	{
		t_idx = pet_t_m_idx;
		t_ptr = &m_list[t_idx];

		/* Cancel if not projectable (for now) */
		if ((m_idx == t_idx) || !projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
		{
			t_idx = 0;
		}
	}

							
	/* Is there counter attack target? */
	if (!t_idx && m_ptr->target_y)
	{
		t_idx = cave[m_ptr->target_y][m_ptr->target_x].m_idx;

		if (t_idx)
		{
			t_ptr = &m_list[t_idx];

			/* Cancel if neither enemy nor a given target */
			if ((m_idx == t_idx) ||
			    ((t_idx != pet_t_m_idx) && !are_enemies(m_ptr, t_ptr)))
			{
				t_idx = 0;
			}

			/* Allow only summoning etc.. if not projectable */
			else if (!projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
			{
				f4 &= (RF4_INDIRECT_MASK);
				f5 &= (RF5_INDIRECT_MASK);
				f6 &= (RF6_INDIRECT_MASK);
				f9 &= (RF9_INDIRECT_MASK);

			}
		}
	}

	if(m_ptr->r_idx == MON_HASSYAKUSAMA && hassyakusama_target_idx)
	{
		if(m_list[hassyakusama_target_idx].r_idx)
		{
			t_idx = hassyakusama_target_idx;
			t_ptr = &m_list[t_idx];
			if (!projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
			{
				f4 &= (RF4_INDIRECT_MASK);
				f5 &= (RF5_INDIRECT_MASK);
				f6 &= (RF6_INDIRECT_MASK);
				f9 &= (RF9_INDIRECT_MASK);
			}
		}
		else
			hassyakusama_target_idx = 0;
	}


	/* Look for enemies normally */
	if (!t_idx)
	{
		bool success = FALSE;

		if (p_ptr->inside_battle)
		{
			start = randint1(m_max-1) + m_max;
			if (randint0(2)) plus = -1;
		}
		else start = m_max + 1;

		/* Scan thru all monsters */
		for (i = start; ((i < start + m_max) && (i > start - m_max)); i += plus)
		{
			int dummy = (i % m_max);
			if (!dummy) continue;

			t_idx = dummy;
			t_ptr = &m_list[t_idx];

			/* Skip dead monsters */
			if (!t_ptr->r_idx) continue;

			/* Monster must be 'an enemy' */
			if ((m_idx == t_idx) || !are_enemies(m_ptr, t_ptr)) continue;

			/* Monster must be projectable */
			if (!projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx)) continue;

			/* Get it */
			success = TRUE;
			break;
		}

		/* No enemy found */
		if (!success) return FALSE;
	}


	/* OK -- we've got a target */
	y = t_ptr->fy;
	x = t_ptr->fx;
	tr_ptr = &r_info[t_ptr->r_idx];

	/* Forget old counter attack target */
	reset_target(m_ptr);

	/* Extract the monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

	//v1.1.94 �����X�^�[���@�͒ቺ���̓��x������l25%�J�b�g
	if (MON_DEC_MAG(m_ptr))
	{
		rlev = (rlev * 3 + 1) / 4;
	}


	/* Remove unimplemented spells */
	f6 &= ~(RF6_WORLD | RF6_TRAPS | RF6_FORGET);

	///mod140818 ���m�͈͊O�̓G�͏������@���g���ɂ����悤�ɂ���
	if(m_ptr->cdis > r_ptr->aaf && !one_in_(13))
	{
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);

	}

	if (f4 & RF4_BR_LITE)
	{
		if (!los(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
			f4 &= ~(RF4_BR_LITE);
	}

	//if ((f9 & RF9_MAELSTROM) && distance(y,x,m_ptr->fy,m_ptr->fx) < 8 && !(r_ptr->flags2 & RF2_STUPID))
	///mod150604 ���Z��ł̂ݎg���悤�ɂ���
	///mod141010 ���C���V���g�����̓^�[�Q�b�g�Ƃ��y�b�g�������݂Ƃ��ʓ|�Ȃ̂œG�͎g��Ȃ����Ƃɂ���
	//v1.1.91 ����ς�z���ȊO�̓G�͎g���悤�ɂ����@
	//TODO:������������������Ɛ������ā��̔z���̂Ƃ��ɂ��Ȃɂ��Ɏg����悤�ɂ�����
	if (is_pet(m_ptr) || distance(y,x,m_ptr->fy,m_ptr->fx) > 5)
	{
		f9 &= ~(RF9_MAELSTROM);
	}
	//�z����*�j��*�⎞�Ԓ�~���g��Ȃ�
	if(is_pet(m_ptr))
	{
		f9 &= ~(RF9_DESTRUCTION);
		f6 &= ~(RF6_WORLD);

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
	//�w�J�[�e�B�A�́����^�[�Q�b�g�̂Ƃ��������ʍs�������Ȃ�
	//v1.1.49 �������B��ނ̓��ʍs��2��
	if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3 || m_ptr->r_idx == MON_OKINA)
	{
			f4 &= ~(RF4_SPECIAL2);
	}
	//�T�C�o�C�}���̓^�[�Q�b�g���߂��ɂ��đ̗͂��Ⴍ�Ȃ��Ɠ��ʍs�������Ȃ�
	if(m_ptr->r_idx == MON_SAIBAIMAN)
	{
		if(m_ptr->hp > m_ptr->maxhp / 3 || distance(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx) > 1 || pet && m_ptr->cdis < 3)
			f4 &= ~(RF4_SPECIAL2);
	}


	//v1.1.91 �ޖ���HP�������ĂĎ��͂ɐΖ��n�`������Ƃ��ɂ������ʍs�������Ȃ��B���Ɠ��Z���A���[�i�ł͓��ʍs�������Ȃ��B
	if (m_ptr->r_idx == MON_YUMA)
	{
		if (p_ptr->inside_arena || p_ptr->inside_battle || (m_ptr->hp > m_ptr->maxhp * 3 / 4) || yuma_vacuum_oil(m_ptr->fy, m_ptr->fx, TRUE, 0) < 1)
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


	/* Remove unimplemented special moves */
	//�����X�^�[�������X�^�[�̓��ʍs���͎w�肵�����̂������}�~
	//�Ȃ��A�������̕��ɂ����ЂƂ����悤�ȗ}�~���[�`��������
	if (f6 & RF6_SPECIAL)
	{
		if ((m_ptr->r_idx != MON_ROLENTO)
			&& (m_ptr->r_idx != MON_MOKOU) 
			&& (m_ptr->r_idx != MON_SHION_2)
			&& (m_ptr->r_idx != MON_MICHAEL)
			&& (m_ptr->r_idx != MON_REIMU) 
			&& (m_ptr->r_idx != MON_SUMIREKO)
			&& (m_ptr->r_idx != MON_YUKARI)
			&& (m_ptr->r_idx != MON_SEIJA)
			&& (m_ptr->r_idx != MON_TSUCHINOKO)
			&& (m_ptr->r_idx != MON_CHIMATA)
			&& (m_ptr->r_idx != MON_TAISAI)
			&& (r_ptr->d_char != 'B'))
			f6 &= ~(RF6_SPECIAL);

		//v1.1.32 ���Ɨ��T�̓��ʍs�����}�~���Ă����B���ĂȂ��Ƃ���œG�������N�A�b�v���Ă��炳�����ɗ��s�s�Ȃ̂ŁB
	}

	if (f6 & RF6_DARKNESS)
	{
		bool vs_ninja = (p_ptr->pclass == CLASS_NINJA) && !is_hostile(t_ptr);
///mod131231 �����X�^�[�t���O�ύX�@�M����_��RF3����RFR��
		if (vs_ninja &&
		    !(r_ptr->flags3 & RF3_UNDEAD) &&
		    !(r_ptr->flagsr & RFR_HURT_LITE) &&
		    !(r_ptr->flags7 & RF7_DARK_MASK))
			can_use_lite_area = TRUE;

		if (!(r_ptr->flags2 & RF2_STUPID))
		{
			if (d_info[dungeon_type].flags1 & DF1_DARKNESS) f6 &= ~(RF6_DARKNESS);
			else if (vs_ninja && !can_use_lite_area) f6 &= ~(RF6_DARKNESS);
		}
	}

	///mod160708 �^�[�Q�b�g�ɗאڂ����G�͗אڃe���|���g��Ȃ��悤�ɂ���
	if ((f9 & (RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH)) && distance(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx) < 2)
	{
		f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
	}

	/*:::�����@���A�ł͖��@���g��Ȃ��iSTUPID�͎g���j*/
	///mod140423 �u�G��ق点��v���ʒ������l�ɏ���
	///mod150604 �f�P�����ǉ�
	//if (in_no_magic_dungeon && !(r_ptr->flags2 & RF2_STUPID))
	if ((in_no_magic_dungeon || p_ptr->silent_floor || (m_ptr->mflag & MFLAG_NO_SPELL))&& !(r_ptr->flags2 & RF2_STUPID))
	{
		f4 &= (RF4_NOMAGIC_MASK);
		f5 &= (RF5_NOMAGIC_MASK);
		f6 &= (RF6_NOMAGIC_MASK);
		f9 &= (RF9_NOMAGIC_MASK);
		if(m_ptr->r_idx == MON_REIMU || m_ptr->r_idx == MON_MICHAEL) f6 &= ~(RF6_SPECIAL);

	}

	//�A���[�i�Ɠ��Z��ł͏������g��Ȃ��B
	//v1.1.43 �����ŋ��C��ԂɂȂ�Ə������g��Ȃ����Ƃɂ��Ă���
	if (p_ptr->inside_arena || p_ptr->inside_battle
		|| m_ptr->mflag & MFLAG_LUNATIC_TORCH)
	{
		f4 &= ~(RF4_SUMMON_MASK | RF4_SHRIEK);
		f5 &= ~(RF5_SUMMON_MASK);
		f6 &= ~(RF6_SUMMON_MASK | RF6_TRAPS | RF6_TELE_LEVEL | RF6_WORLD | RF6_DARKNESS);
		f9 &= ~(RF9_SUMMON_MASK | RF9_DESTRUCTION | RF9_ALARM);
		if (m_ptr->r_idx == MON_SATONO || m_ptr->r_idx == MON_MAI) f6 &= ~(RF6_SPECIAL); //v1.1.32

		if (m_ptr->r_idx == MON_ROLENTO) f6 &= ~(RF6_SPECIAL);
	}

	// 2/3�ŗ}�~��1/2�ŗ}�~
	if (p_ptr->inside_battle && !one_in_(2))
	{
		f6 &= ~(RF6_HEAL);
	}

	//��n���̃����X�^�[�̓e���|�[�g��x��Ȃǂ��g��Ȃ�
	//�����R���̍r�n��炵�Ń��l���h�[���ɏ���Ă���Ɠ��ʍs�����g���Ă����Ƃ����񍐂�����B(v1.1.61)
	if (m_idx == p_ptr->riding)
	{
		f4 &= ~(RF4_RIDING_MASK);
		f5 &= ~(RF5_RIDING_MASK);
		f6 &= ~(RF6_RIDING_MASK);
		f9 &= ~(RF9_RIDING_MASK);
	}

	//v1.1.36 �G�^�j�e�B�����o�͋��|���ɂ������L�_���g��Ȃ��悤�ɂ���
	if(m_ptr->r_idx == MON_LARVA)
	{
		if (!MON_MONFEAR(m_ptr))
		{
			f5 &= ~(RF5_BA_POIS);
		}
	}

	if (pet)
	{
		f4 &= ~(RF4_SHRIEK);
		f6 &= ~(RF6_DARKNESS | RF6_TRAPS);
		f9 &= ~( RF9_DESTRUCTION | RF9_ALARM);

		if (!(p_ptr->pet_extra_flags & PF_TELEPORT))
		{
			f6 &= ~(RF6_BLINK | RF6_TPORT | RF6_TELE_TO | RF6_TELE_AWAY | RF6_TELE_LEVEL);
			f9 &= ~(RF9_TELE_APPROACH | RF9_TELE_HI_APPROACH);
		}

		if (!(p_ptr->pet_extra_flags & PF_ATTACK_SPELL))
		{
			f4 &= ~(RF4_ATTACK_MASK);
			f5 &= ~(RF5_ATTACK_MASK);
			f6 &= ~(RF6_ATTACK_MASK);
			f9 &= ~(RF9_ATTACK_MASK);
		}

		if (!(p_ptr->pet_extra_flags & PF_SUMMON_SPELL))
		{
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}

		/* Prevent collateral damage */
		/*:::�t�����̕��g�͍U���Ƀv���C���[����������*/
		if ((m_ptr->r_idx != MON_FLAN_4) && !(p_ptr->pet_extra_flags & PF_BALL_SPELL) && (m_idx != p_ptr->riding))
		{
			if ((f4 & (RF4_BALL_MASK & ~(RF4_ROCKET))) ||
			    (f5 & RF5_BALL_MASK) ||
			    (f6 & RF6_BALL_MASK) ||
				(f9 & RF9_BALL_MASK) )
			{
				int real_y = y;
				int real_x = x;

				get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, 0L);

				if (projectable(real_y, real_x, py, px))
				{
					int dist = distance(real_y, real_x, py, px);

					if (dist <= 2)
					{
						f4 &= ~(RF4_BALL_MASK & ~(RF4_ROCKET));
						f5 &= ~(RF5_BALL_MASK);
						f6 &= ~(RF6_BALL_MASK);
						f9 &= ~(RF9_BALL_MASK);
					}
					else if (dist <= 4)
					{
						f4 &= ~(RF4_BIG_BALL_MASK);
						f5 &= ~(RF5_BIG_BALL_MASK);
						f6 &= ~(RF6_BIG_BALL_MASK);
						f9 &= ~(RF9_BIG_BALL_MASK);
					}
					else if (dist <= 5)
					{
						f9 &= ~(RF9_BA_DISI | RF9_BA_METEOR | RF9_HELLFIRE | RF9_HOLYFIRE | RF9_TORNADO);


					}
				}
				else if (f5 & RF5_BA_LITE)
				{
					if ((distance(real_y, real_x, py, px) <= 4) && los(real_y, real_x, py, px))
						f5 &= ~(RF5_BA_LITE);
				}
			}

			/*:::���僌�[�U�[�͈͔̔���͂�����Ɠ���̂Œ[�܂���*/
			f9 &= ~(RF9_GIGANTIC_LASER | RF9_FINALSPARK);
			f4 &= ~(RF4_WAVEMOTION);


			if (f4 & RF4_ROCKET)
			{
				int real_y = y;
				int real_x = x;

				get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, PROJECT_STOP);
				if (projectable(real_y, real_x, py, px) && (distance(real_y, real_x, py, px) <= 2))
					f4 &= ~(RF4_ROCKET);
			}

			if (((f4 & RF4_BEAM_MASK) ||
			     (f5 & RF5_BEAM_MASK) ||
			     (f9 & RF9_BEAM_MASK) ||
			     (f6 & RF6_BEAM_MASK)) &&
			    !direct_beam(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, m_ptr))
			{
				f4 &= ~(RF4_BEAM_MASK);
				f5 &= ~(RF5_BEAM_MASK);
				f6 &= ~(RF6_BEAM_MASK);
				f9 &= ~(RF9_BEAM_MASK);
			}

			if ((f4 & RF4_BREATH_MASK) ||
			    (f5 & RF5_BREATH_MASK) ||
			    (f9 & RF9_BREATH_MASK) ||
			    (f6 & RF6_BREATH_MASK))
			{
				/* Expected breath radius */
				int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 3 : 2;

				if (!breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, 0, TRUE))
				{
					f4 &= ~(RF4_BREATH_MASK);
					f5 &= ~(RF5_BREATH_MASK);
					f6 &= ~(RF6_BREATH_MASK);
					f9 &= ~(RF9_BREATH_MASK);
				}
				else if ((f4 & RF4_BR_LITE) &&
					 !breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, GF_LITE, TRUE))
				{
					f4 &= ~(RF4_BR_LITE);
				}
				else if ((f4 & RF4_BR_DISI) &&
					 !breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, GF_DISINTEGRATE, TRUE))
				{
					f4 &= ~(RF4_BR_DISI);
				}
			}
		}

		/* Special moves restriction */

		if (f6 & RF6_SPECIAL)
		{
			if (m_ptr->r_idx == MON_ROLENTO)
			{
				if ((p_ptr->pet_extra_flags & (PF_ATTACK_SPELL | PF_SUMMON_SPELL)) != (PF_ATTACK_SPELL | PF_SUMMON_SPELL))
					f6 &= ~(RF6_SPECIAL);
			}
			else if (r_ptr->d_char == 'B')
			{
				if ((p_ptr->pet_extra_flags & (PF_ATTACK_SPELL | PF_TELEPORT)) != (PF_ATTACK_SPELL | PF_TELEPORT))
					f6 &= ~(RF6_SPECIAL);
			}
			else if(m_ptr->r_idx == MON_MICHAEL);

			///mod140626 �얲�͕Ǎۂł͖��z������g���Ȃ�
			else if(m_ptr->r_idx == MON_REIMU)
			{
				if (!in_bounds2(m_ptr->fy + 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy + 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy - 1,m_ptr->fx) || !cave_have_flag_grid( &cave[m_ptr->fy - 1][m_ptr->fx],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx + 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx + 1],FF_PROJECT)
				||  !in_bounds2(m_ptr->fy ,m_ptr->fx - 1) || !cave_have_flag_grid( &cave[m_ptr->fy ][m_ptr->fx - 1],FF_PROJECT))
				f6 &= ~(RF6_SPECIAL);
			}
			else if(m_ptr->r_idx == MON_TSUCHINOKO);
			else if (m_ptr->r_idx == MON_CHIMATA);
			else if (m_ptr->r_idx == MON_TAISAI);

			else f6 &= ~(RF6_SPECIAL);
		}
	}

	//�P��(��)�̍s���}������
	if(m_ptr->r_idx == MON_MASTER_KAGUYA)
	{
		int real_y = y;
		int real_x = x;

		if(!clean_shot(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, TRUE))
		{
			f4 &= ~(RF4_BOLT_MASK);
			f5 &= ~(RF5_BOLT_MASK);
			f9 &= ~(RF9_BOLT_MASK);
			f6 &= ~(RF6_BOLT_MASK);
		}

		if (!direct_beam(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, m_ptr))
		{
			f4 &= ~(RF4_BEAM_MASK);
			f5 &= ~(RF5_BEAM_MASK);
			f6 &= ~(RF6_BEAM_MASK);
			f9 &= ~(RF9_BEAM_MASK);
		}

		get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, 0L);

		if (projectable(real_y, real_x, py, px))
		{
			int dist = distance(real_y, real_x, py, px);

			if (dist <= 2)
			{
				f4 &= ~(RF4_BALL_MASK & ~(RF4_ROCKET));
				f5 &= ~(RF5_BALL_MASK);
				f6 &= ~(RF6_BALL_MASK);
				f9 &= ~(RF9_BALL_MASK);
			}
			else if (dist <= 4)
			{
				f4 &= ~(RF4_BIG_BALL_MASK);
				f5 &= ~(RF5_BIG_BALL_MASK);
				f6 &= ~(RF6_BIG_BALL_MASK);
				f9 &= ~(RF9_BIG_BALL_MASK);
			}
			else if (dist <= 5)
			{
				f9 &= ~(RF9_BA_DISI | RF9_BA_METEOR | RF9_HELLFIRE | RF9_HOLYFIRE | RF9_TORNADO);


			}
		}
		else if (f5 & RF5_BA_LITE)
		{
			if ((distance(real_y, real_x, py, px) <= 4) && los(real_y, real_x, py, px))
				f5 &= ~(RF5_BA_LITE);
		}

	}

	/* Remove some spells if necessary */

	if (!(r_ptr->flags2 & RF2_STUPID))
	{
		/* Check for a clean bolt shot */
		if (((f4 & RF4_BOLT_MASK) ||
		     (f5 & RF5_BOLT_MASK) ||
		     (f9 & RF9_BOLT_MASK) ||
		     (f6 & RF6_BOLT_MASK)) &&
		    !clean_shot(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, pet))
		{
			f4 &= ~(RF4_BOLT_MASK);
			f5 &= ~(RF5_BOLT_MASK);
			f9 &= ~(RF9_BOLT_MASK);
			f6 &= ~(RF6_BOLT_MASK);
		}

		/* Check for a possible summon */
		if (((f4 & RF4_SUMMON_MASK) ||
		     (f5 & RF5_SUMMON_MASK) ||
		     (f9 & RF9_SUMMON_MASK) ||
		     (f6 & RF6_SUMMON_MASK)) &&
		    !(summon_possible(t_ptr->fy, t_ptr->fx)))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
		}

		//v1.1.13 �����X�^�[��SMART�ȂƂ��u���]������㩁v���L�����ƂقƂ�Ǐ��������݂Ȃ�
		//v1.1.54 �l���m���Z�ǉ�
		/* Check for a possible summon */

		if ((r_ptr->flags2 & RF2_SMART)
			&& ((CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO)) || (p_ptr->special_flags & SPF_ORBITAL_TRAP))
			&& ((f4 & RF4_SUMMON_MASK) || (f5 & RF5_SUMMON_MASK) || (f6 & RF6_SUMMON_MASK) || (f9 & RF9_SUMMON_MASK))
			&& !one_in_(4))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
			f9 &= ~(RF9_SUMMON_MASK);
		}



		/* Dispel magic */
		if ((f4 & RF4_DISPEL) && !dispel_check_monster(m_idx, t_idx))
		{
			/* Remove dispel spell */
			f4 &= ~(RF4_DISPEL);
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
//			f6 &= ~(RF6_RAISE_DEAD);

		/* Special moves restriction */
		if (f6 & RF6_SPECIAL)
		{
			if ((m_ptr->r_idx == MON_ROLENTO) && !summon_possible(t_ptr->fy, t_ptr->fx))
			{
				f6 &= ~(RF6_SPECIAL);
			}
		}
	}

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
		//RF4_DISPEL
		if(f4 & RF4_DISPEL && one_in_(2)) thrown_spell = 96 + 2;

		/* Hack -- decline "teleport level" in some case */
		if ((f6 & RF6_TELE_LEVEL) && TELE_LEVEL_IS_INEFF((t_idx == p_ptr->riding) ? 0 : t_idx))
		{
			f6 &= ~(RF6_TELE_LEVEL);
		}
		///�j��g�p���菈��
		if ( !pet && (f9 & RF9_DESTRUCTION))
		{
			int j;
			int mcount = 0;
			for (j = 1; j < m_max; j++)
			{
				monster_race *r_ptr_tmp = &r_info[m_list[j].r_idx]; 
				if(distance( m_ptr->fy, m_ptr->fx,m_list[j].fy,m_list[j].fx) < 10 && r_ptr_tmp->level > r_ptr->level / 2 && are_enemies(m_ptr,&m_list[j])) mcount++;
			}
			//	msg_format("CHECK2:cnt=%d",mcount);
			if(mcount > randint0(10) )
			{
				thrown_spell = 192+24; /* RF9_DESTRUCTION */

			}

		}





	}

	/* No spells left */
	if (!f4 && !f5 && !f6 && !f9) return FALSE;

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
	for (k = 0; k < 32; k++)
	{
		if (f9 & (1L << k)) spell[num++] = k + 32 * 6;
	}

	/* No spells left */
	if (!num) return (FALSE);

	/* Stop if player is dead or gone */
	if (!p_ptr->playing || p_ptr->is_dead) return (FALSE);

	/* Handle "leaving" */
	if (p_ptr->leaving) return (FALSE);

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0x00);

#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif

	/* Get the target's name (or "it") */
	monster_desc(t_name, t_ptr, 0x00);

	/* Choose a spell to cast */
	/*:::�܂��X�y�������܂��Ă��Ȃ��Ƃ������_���Ɍ��߂�悤�ɂ���*/
	if(!thrown_spell) thrown_spell = spell[randint0(num)];

	see_t = is_seen(t_ptr);
	see_either = (see_m || see_t);

	/* Can the player be aware of this attack? */
	known = (m_ptr->cdis <= MAX_SIGHT) || (t_ptr->cdis <= MAX_SIGHT);

	if (p_ptr->riding && (m_idx == p_ptr->riding)) disturb(1, 1);

	/* Check for spell failure (inate attacks never fail) */
	//v1.1.94 ���@�͒ቺ����1/4�Ŏ��s���邱�Ƃɂ���
	//v1.1.96 ���|�̂Ƃ��ɂ��������ቺ�@�����ɂ�mspells1�Ƃ�����Ɗm�����Ⴄ���܂�����
	if (!spell_is_inate(thrown_spell) && (in_no_magic_dungeon || (MON_STUNNED(m_ptr) && one_in_(2)) || MON_DEC_MAG(m_ptr) && one_in_(4) || MON_MONFEAR(m_ptr) && one_in_(4) ||  (m_ptr->mflag & MFLAG_NO_SPELL)))
	{
		disturb(1, 1);
		/* Message */
#ifdef JP
		if (see_m) msg_format("%^s�͎����������悤�Ƃ��������s�����B", m_name);
#else
		if (see_m) msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif

		return (TRUE);
	}

	/* Hex: Anti Magic Barrier */
	if (!spell_is_inate(thrown_spell) && magic_barrier(m_idx))
	{
#ifdef JP
		if (see_m) msg_format("�����@�o���A��%^s�̎����������������B", m_name);
#else
		if (see_m) msg_format("Anti magic barrier cancels the spell which %^s casts.");
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

	if (is_hostile(m_ptr) && spell_is_summon(thrown_spell) && CHECK_USING_SD_UNIQUE_CLASS_POWER(CLASS_NEMUNO) && randint1(rlev) < p_ptr->lev)
	{
		msg_format("%^s���������������悤�Ƃ������A���Ȃ��͕s�^�ȗ��K�҂����₵���B", m_name);
		return (TRUE);
	}

	if(is_hostile(m_ptr) && spell_is_summon(thrown_spell) && p_ptr->deportation && los(y,x,py,px))
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
			else if(monspell_list2[monspell_num].level && distance(py,px, m_ptr->fy, m_ptr->fx) < MAX_RANGE) //�_�~�[����ʍs���͔�Ώ�
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
	//���������E�n�b�s�[���C�u
	if(p_ptr->pclass == CLASS_MERLIN  && !spell_is_inate(thrown_spell) && music_singing(MUSIC_NEW_MERLIN_SOLO) && distance(py,px,m_ptr->fy,m_ptr->fx) < MAX_RANGE)
	{

		//v1.1.65 �O�����Z�q�̗D�揇�ʂ��ԈႦ�Ă�tmp�l��1��2�ɂȂ��Ă��܂��Ă����̂ŏC��
		int tmp = rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);

		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND)) tmp *= 2;


		if(randint1(tmp) < p_ptr->lev + adj_general[p_ptr->stat_ind[A_CHR]])
		{
			if(one_in_(3)) msg_format("%^s�͎����������悤�Ƃ��Ă���ʂ��Ƃ����������B", m_name);
			else if(one_in_(2))msg_format("%^s�̓V�A���Z���B", m_name);
			else msg_format("%^s�͏����A���ȗl�q���B", m_name);
			return (TRUE);
		}
	}

	can_remember = is_original_ap_and_seen(m_ptr);

	switch (thrown_spell)
	{
	/* RF4_SHRIEK */
	//�����X�^�[���m�̋��т͉��������Ȃ��炵��
	case 96+0:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ċ��񂾁B", m_name, t_name);
#else
				msg_format("%^s shrieks at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		wake_up = TRUE;

		break;

		/* RF4_DANMAKU */
	case 96+1:

		if (known)
		{
			if (see_either)
			{
				if (blind)
				{
#ifdef JP
					msg_format("%^s��������������B", m_name);
#else
					msg_format("%^s makes a strange noise.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ɍ����Ēe����������B", m_name, t_name);
#else
					msg_format("%^s fires an arrow at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_SHOOT);
		}
		dam = rlev;
		monst_bolt_monst(m_idx, y, x, GF_ARROW, dam, MS_SHOOT, learnable);

		break;
	/* RF4_DISPEL */
	case 96+2:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s��%s�ɑ΂��Ė��͏����̎�����O�����B", m_name, t_name);
#else
				msg_format("%^s invokes a dispel magic at %s.", m_name, t_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (t_idx == p_ptr->riding) dispel_player();
		dispel_monster_status(t_idx);

		break;

	/* RF4_ROCKET */
	case 96+3:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s���������˂����B", m_name);
#else
					msg_format("%^s shoots something.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ƀ��P�b�g�𔭎˂����B", m_name, t_name);
#else
					msg_format("%^s fires a rocket at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = ((m_ptr->hp / 4) > 800 ? 800 : (m_ptr->hp / 4));
		monst_breath_monst(m_idx, y, x, GF_ROCKET,
				   dam, 2, FALSE, MS_ROCKET, learnable);

		break;

	/* RF4_SHOOT */
	case 96+4:
		{
			int r_idx = m_ptr->r_idx;
			dam = damroll(r_ptr->blow[0].d_dice, r_ptr->blow[0].d_side);
			if(r_idx == MON_SAKUYA || r_idx == MON_MURASA || r_idx == MON_NINJA_SLAYER) dam *= 2;
			else  if(r_idx == MON_COLOSSUS || r_idx == MON_SUIKA) dam = dam * 5 / 2;

			if (known && see_either)
			{
				if (blind)
				{
					msg_format("%^s���������ǂ����֕������悤���B", m_name);
				}
				else
				{
					if(r_idx == MON_NODEPPOU || r_idx == MON_DROLEM)
						msg_format("%^s��%s�։�����������B", m_name, t_name);
					else if(r_idx == MON_SUMIREKO || r_idx == MON_SUMIREKO_2 || r_idx == MON_CHARGE_MAN || r_idx == MON_JURAL || r_idx == MON_NITORI || r_idx == MON_GYOKUTO || r_idx == MON_SEIRAN || r_idx == MON_EAGLE_RABBIT)
						msg_format("%^s��%s�֏e���������B", m_name,t_name);
					else if(r_idx == MON_HAMMERBROTHER)
						msg_format("%^s��%s�փn���}�[�𓊂����B", m_name,t_name);
					else if(r_idx == MON_YAMAWARO)
						msg_format("%^s��%s�֎藠���𓊂����B", m_name,t_name);
					else if(r_idx == MON_KISUME)
						msg_format("%^s��%s�֍��𓊂����B", m_name,t_name);
					else if(r_idx == MON_SAKUYA)
						msg_format("%^s��%s�֑�ʂ̃i�C�t�𓊝������B", m_name,t_name);
					else if(r_idx == MON_MURASA)
						msg_format("%^s��%s�Ɍ������ĕd�𓊂������I", m_name,t_name);
					else if(r_idx == MON_HALFLING_S || r_idx == MON_CULVERIN || r_idx == MON_EIKA)
						msg_format("%^s��%s�֐΂ԂĂ�������B", m_name,t_name);
					else if(r_idx == MON_COLOSSUS || r_idx == MON_SUIKA)
						msg_format("%^s��%s�̂ق��֑��𓊂������I", m_name,t_name);
					else if(r_idx == MON_NINJA_SLAYER)
						msg_format("%^s��%s�Ƀc���C�E�X���P����������I", m_name,t_name);
					else msg_format("%^s��%s�֖��������B", m_name,t_name);
				}
			}

			if (!see_either) mon_fight = TRUE;

			sound(SOUND_SHOOT);

			monst_bolt_monst(m_idx, y, x, GF_ARROW, dam, MS_SHOOT, learnable);
		}
		break;

	/* RF4_br_holy */
	case 96+5:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɐ��Ȃ�u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes acid at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_HOLY_FIRE,
				   dam,0, TRUE, MS_BR_HOLY, learnable);

		break;


	/* RF4_BRHELL */
	case 96+6:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɒn���̋Ɖ΂̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes acid at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_HELL_FIRE,
				   dam,0, TRUE, MS_BR_HELL, learnable);

		break;





		/* RF4_�A�N�A�u���X */
	case 96+7:
	if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɃA�N�A�u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes acid at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

			dam = ((m_ptr->hp / 8) > 350 ? 350 : (m_ptr->hp / 8));
		monst_breath_monst(m_idx, y, x, GF_WATER,
				   dam,0, TRUE, MS_BR_AQUA, learnable);

		break;


	/* RF4_BR_ACID */
	case 96+8:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ɏ_�̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes acid at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_ACID,
				   dam,0, TRUE, MS_BR_ACID, learnable);

		break;

	/* RF4_BR_ELEC */
	case 96+9:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ɉ�Ȃ̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes lightning at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_ELEC,
				   dam,0, TRUE, MS_BR_ELEC, learnable);

		break;

	/* RF4_BR_FIRE */
	case 96+10:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɉΉ��̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes fire at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_FIRE,
				   dam,0, TRUE, MS_BR_FIRE, learnable);

		break;

	/* RF4_BR_COLD */
	case 96+11:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɗ�C�̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes frost at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_COLD,
				   dam,0, TRUE, MS_BR_COLD, learnable);
		break;

	/* RF4_BR_POIS */
	case 96+12:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɃK�X�̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes gas at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_POIS,
				   dam,0, TRUE, MS_BR_POIS, learnable);

		break;

	/* RF4_BR_NETH */
	case 96+13:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɒn���̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes nether at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 550 ? 550 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_NETHER,
				   dam,0, TRUE, MS_BR_NETHER, learnable);

		break;

	/* RF4_BR_LITE */
	case 96+14:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɑM���̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes light at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_LITE,
				   dam,0, TRUE, MS_BR_LITE, learnable);

		break;

	/* RF4_BR_DARK */
	case 96+15:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɈÍ��̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes darkness at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_DARK,
				   dam,0, TRUE, MS_BR_DARK, learnable);

		break;

	/* RF4_WAVEMOTION */
	case 96+16:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s���������͂ȍU���������B", m_name);
#else
					msg_format("%^s mumbles.", m_name);
#endif

				}
				else if(m_ptr->r_idx == MON_TOYOHIME)
				{
					msg_format("%^s�͐�̂悤�Ȃ��̂����o����%s�Ɍ����đ傫���U�����I", m_name,t_name);
				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɔg���C��������I", m_name, t_name);
#else
					msg_format("%^s casts a ball of radiation at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = ((m_ptr->hp / 6) > 555 ? 555 : (m_ptr->hp / 6));
		masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, MS_WAVEMOTION, FALSE,2);
		/*:::Hack - �g���C���ˌ�́����s������܂Ŗ��@���g��Ȃ�*/
		m_ptr->mflag |= (MFLAG_NICE);
		repair_monsters = TRUE;
		break;



	/* RF4_BR_SOUN */
	case 96+17:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (m_ptr->r_idx == MON_JAIAN)
#ifdef JP
					msg_format("�u�{�H�G�`�`�`�`�`�`�v");
#else
				msg_format("'Booooeeeeee'");
#endif

				else if (m_ptr->r_idx == MON_KUTAKA)
					msg_format("�u�R�@�P�@�R�@�b�@�R�@�[�@�I�v");

				else if (blind)
				{
#ifdef JP
					msg_format("%^s���Ռ��g��������B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ɍ����ďՌ��g��������B", m_name, t_name);
#else
					msg_format("%^s breathes sound at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_SOUND,
				   dam,0, TRUE, MS_BR_SOUND, learnable);

		break;

	/* RF4_BR_CHAO */
	case 96+18:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɃJ�I�X�̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes chaos at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 600 ? 600 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_CHAOS,
				   dam,0, TRUE, MS_BR_CHAOS, learnable);

		break;

	/* RF4_BR_DISE */
	case 96+19:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɗ򉻂̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes disenchantment at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_DISENCHANT,
				   dam,0, TRUE, MS_BR_DISEN, learnable);

		break;

	/* RF4_BR_NEXU */
	case 96+20:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ɉ��ʍ����̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes nexus at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 7) > 600 ? 600 : (m_ptr->hp / 7));
		monst_breath_monst(m_idx, y, x, GF_NEXUS,
				   dam,0, TRUE, MS_BR_NEXUS, learnable);

		break;

	/* RF4_BR_TIME */
	case 96+21:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ɏ��ԋt�]�̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes time at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 150 ? 150 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_TIME,
				   dam,0, TRUE, MS_BR_TIME, learnable);

		break;

	/* RF4_BR_INER */
	case 96+22:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɒx�݂̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes inertia at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_INACT,
				   dam,0, TRUE, MS_BR_INERTIA, learnable);

		break;

	/* RF4_BR_GRAV */
	case 96+23:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɏd�͂̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes gravity at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 200 ? 200 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_GRAVITY,
				   dam,0, TRUE, MS_BR_GRAVITY, learnable);

		break;

	/* RF4_BR_SHAR */
	case 96+24:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (m_ptr->r_idx == MON_BOTEI)
#ifdef JP
					msg_format("�u�{��r���J�b�^�[�I�I�I�v");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
				else if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɔj�Ђ̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes shards at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_SHARDS,
				   dam,0, TRUE, MS_BR_SHARDS, learnable);

		break;

	/* RF4_BR_PLAS */
	case 96+25:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ƀv���Y�}�̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes plasma at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 4) > 1200 ? 1200 : (m_ptr->hp / 4));
		monst_breath_monst(m_idx, y, x, GF_PLASMA,
				   dam,0, TRUE, MS_BR_PLASMA, learnable);

		break;

	/* RF4_BR_WALL ��BA_FORCE*/
	case 96+26:
	
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s��������������B", m_name);
#else
					msg_format("%^s mumbles.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɋC�e��������B", m_name, t_name);
#else
					msg_format("%^s casts a ball of radiation at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

			if(r_ptr->flags2 & RF2_POWERFUL)
				dam = ((m_ptr->hp / 4) > 350 ? 350 : (m_ptr->hp / 4));
			else
				dam = ((m_ptr->hp / 6) > 250 ? 250 : (m_ptr->hp / 6));
			
		monst_breath_monst(m_idx, y, x, GF_FORCE,
				   dam, 2, FALSE, MS_BA_FORCE, learnable);

		break;



	/* RF4_BR_MANA */
	case 96+27:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɖ��͂̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes mana at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
		monst_breath_monst(m_idx, y, x, GF_MANA,
				   dam,0, TRUE, MS_BR_MANA, learnable);

		break;

	/* RF4_SPECIAL2 */
	case 96+28:
		{
			disturb(1, 1);
		

			if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D)
			{

				/*:::���חאڎ��@���ƍU��*/
				if(distance(y, x, m_ptr->fy, m_ptr->fx)<2)
				{
					if (see_m)	msg_format("%^s��%s�����F�̏��ƂłԂ񉣂����I", m_name,t_name);
					dam = 100 + randint0(100);
					monst_breath_monst(m_idx, y, x, GF_MISSILE, dam, 0, FALSE, 0, FALSE);
					earthquake_aux(y, x, 4, m_idx);
				}
				/*:::���ה�אڎ��@�ʒu����*/
				else
				{
					if( tr_ptr->flagsr & RFR_RES_TELE || cave[y][x].info & CAVE_ICKY || cave[m_ptr->fy][m_ptr->fx].info & CAVE_ICKY || p_ptr->riding == t_idx)
					{
						if (see_m) msg_format("%^s��%s�ɉ��������悤�Ƃ��������s�����悤���B",m_name,t_name);
					}
					else
					{
						int x1=x,y1=y,x2 = m_ptr->fx,y2=m_ptr->fy;
						if (see_m)msg_format("%^s��%s�Əꏊ�����������B",m_name,t_name);
						/*:::�����X�^�[���m�̈ʒu��������*/
						cave[y2][x2].m_idx = t_idx;
						cave[y1][x1].m_idx = m_idx;
						t_ptr->fy = y2;
						t_ptr->fx = x2;
						m_ptr->fy = y1;
						m_ptr->fx = x1;
						update_mon(m_idx,TRUE);
						update_mon(t_idx,TRUE);
						lite_spot(y1,x1);
						lite_spot(y2,x2);
						verify_panel();
					}
				}
			}
			else if(m_ptr->r_idx == MON_FLAN)
			{
				if (!blind)
				{ 
					msg_format("%^s��%s�����߂Č����������B", m_name,t_name);
				}
				monst_breath_monst(m_idx, y, x, GF_KYUTTOSHITEDOKA_N, 300, 0, FALSE, 0, FALSE);


			}
			//�A�U�g�[�g����U���@�������僌�[�U�[�������{�[����A������
			else if(m_ptr->r_idx == MON_AZATHOTH)
			{
				int atkcount;
				if (!blind)
				{ 
					msg_format("%^s�̋���ȋU�����_���W�������c���Ɋт����I", m_name);
					msg_print(NULL);
				}
				dam = 100 + randint1(100);
				for(atkcount = 6;atkcount>0;atkcount--)
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
				msg_format("ERROR: monst_spell_monst()�ɂ�����%^s�̓��ʂȍs��2�͒�`����Ă��Ȃ��B", m_name);
			}

			break;
		}
	/* RF4_BR_NUKE */
	case 96+29:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�Ɋj�M�̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes toxic waste at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}
		dam = ((m_ptr->hp / 4) > 1000 ? 1000 : (m_ptr->hp / 4));
		monst_breath_monst(m_idx, y, x, GF_NUKE,
				   dam,0, TRUE, MS_BR_NUKE, learnable);
		break;

	/* RF4_BA_CHAO */
	case 96+30:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�����낵���ɂԂ₢���B", m_name);
#else
					msg_format("%^s mumbles frighteningly.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɏ����O���X��������B", m_name, t_name);
#else
					msg_format("%^s invokes raw Logrus upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 2) + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_CHAOS,
				   dam, 4, FALSE, MS_BALL_CHAOS, learnable);

		break;

	/* RF4_BR_DISI */
	case 96+31:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s�������̃u���X��f�����B", m_name);
#else
					msg_format("%^s breathes.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɕ����̃u���X��f�����B", m_name, t_name);
#else
					msg_format("%^s breathes disintegration at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}

			sound(SOUND_BREATH);
		}

		dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
		monst_breath_monst(m_idx, y, x, GF_DISINTEGRATE,
				   dam,0, TRUE, MS_BR_DISI, learnable);
		break;

	/* RF5_BA_ACID */
	case 128+0:
		if (known)
		{
			if (see_either)
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
					msg_format("%^s��%s�Ɍ������ăA�V�b�h�E�{�[���̎������������B", m_name, t_name);
#else
					msg_format("%^s casts an acid ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

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
		monst_breath_monst(m_idx, y, x, GF_ACID, dam, rad, FALSE, MS_BALL_ACID, learnable);
		break;

	/* RF5_BA_ELEC */
	case 128+1:
		if (known)
		{
			if (see_either)
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
					msg_format("%^s��%s�Ɍ������ăT���_�[�E�{�[���̎������������B", m_name, t_name);
#else
					msg_format("%^s casts a lightning ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

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
		monst_breath_monst(m_idx, y, x, GF_ELEC, dam, rad, FALSE, MS_BALL_ELEC, learnable);
		break;

	/* RF5_BA_FIRE */
	case 128+2:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (m_ptr->r_idx == MON_ROLENTO)
				{
#ifdef JP
					if (blind)
						msg_format("%^s�������𓊂����B", m_name);
					else
						msg_format("%^s��%^s�Ɍ������Ď�֒e�𓊂����B", m_name, t_name);
#else
					if (blind)
						msg_format("%^s throws something.", m_name);
					else
						msg_format("%^s throws a hand grenade.", m_name);
#endif
				}
				else
				{
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
						msg_format("%^s��%s�Ɍ������ăt�@�C�A�E�{�[���̎������������B", m_name, t_name);
#else
						msg_format("%^s casts a fire ball at %s.", m_name, t_name);
#endif

					}
				}
			}
			else
			{
				mon_fight = TRUE;
			}
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
		monst_breath_monst(m_idx, y, x, GF_FIRE, dam, rad, FALSE, MS_BALL_FIRE, learnable);
		break;

	/* RF5_BA_COLD */
	case 128+3:
		if (known)
		{
			if (see_either)
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
					msg_format("%^s��%s�Ɍ������ăA�C�X�E�{�[���̎������������B", m_name, t_name);
#else
					msg_format("%^s casts a frost ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

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
		monst_breath_monst(m_idx, y, x, GF_COLD, dam, rad, FALSE, MS_BALL_COLD, learnable);
		break;

	/* RF5_BA_POIS */
	case 128+4:
		if (known)
		{
			if (see_either)
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
					msg_format("%^s��%s�Ɍ������Ĉ��L�_�̎������������B", m_name, t_name);
#else
					msg_format("%^s casts a stinking cloud at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(12, 2) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_breath_monst(m_idx, y, x, GF_POIS, dam, 2, FALSE, MS_BALL_POIS, learnable);

		break;

	/* RF5_BA_NETH */
	case 128+5:
		if (known)
		{
			if (see_either)
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
					msg_format("%^s��%s�Ɍ������Ēn�����̎������������B", m_name, t_name);
#else
					msg_format("%^s casts a nether ball at %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
		monst_breath_monst(m_idx, y, x, GF_NETHER, dam, 2, FALSE, MS_BALL_NETHER, learnable);

		break;

	/* RF5_BA_WATE */
	case 128+6:
		if (known)
		{
			if (see_either)
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
					msg_format("%^s��%s�ɑ΂��ė����悤�Ȑg�U��������B", m_name, t_name);
#else
					msg_format("%^s gestures fluidly at %s.", m_name, t_name);
#endif

#ifdef JP
					msg_format("%^s�͉Q���Ɉ��ݍ��܂ꂽ�B", t_name);
#else
					msg_format("%^s is engulfed in a whirlpool.", t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = ((r_ptr->flags2 & RF2_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
		monst_breath_monst(m_idx, y, x, GF_WATER, dam, 4, FALSE, MS_BALL_WATER, learnable);

		break;

	/* RF5_BA_MANA */
	case 128+7:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s��������͋����Ԃ₢���B", m_name);
#else
					msg_format("%^s mumbles powerfully.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɑ΂��Ė��̗͂��̎�����O�����B", m_name, t_name);
#else
					msg_format("%^s invokes a mana storm upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 4) + 50 + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_MANA, dam, 4, FALSE, MS_BALL_MANA, learnable);

		break;

	/* RF5_BA_DARK */
	case 128+8:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s��������͋����Ԃ₢���B", m_name);
#else
					msg_format("%^s mumbles powerfully.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɑ΂��ĈÍ��̗��̎�����O�����B", m_name, t_name);
#else
					msg_format("%^s invokes a darkness storm upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 4) + 50 + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_DARK, dam, 4, FALSE, MS_BALL_DARK, learnable);

		break;

	/* RF5_DRAIN_MANA */
	case 128+9:
		if (see_m)
		{
			/* Basic message */
#ifdef JP
			msg_format("%^s�͐��_�G�l���M�[��%s����z���Ƃ����B", m_name, t_name);
#else
			msg_format("%^s draws psychic energy from %s.", m_name, t_name);
#endif

		}

		dam = ((randint1(rlev) / 2) + 1);
		monst_breath_monst(m_idx, y, x, GF_DRAIN_MANA, dam, 0, FALSE, MS_DRAIN_MANA, learnable);

		break;

	/* RF5_MIND_BLAST */
	case 128+10:
		if (see_m)
		{
#ifdef JP
			msg_format("%^s��%s���������ɂ񂾁B", m_name, t_name);
#else
			msg_format("%^s gazes intently at %s.", m_name, t_name);
#endif

		}

		dam = damroll(7, 7);
		monst_breath_monst(m_idx, y, x, GF_MIND_BLAST, dam, 0, FALSE, MS_MIND_BLAST, learnable);

		break;

	/* RF5_BRAIN_SMASH */
	case 128+11:
		if (see_m)
		{
#ifdef JP
			msg_format("%^s��%s���������ɂ񂾁B", m_name, t_name);
#else
			msg_format("%^s gazes intently at %s.", m_name, t_name);
#endif

		}

		dam = damroll(12, 12);
		monst_breath_monst(m_idx, y, x, GF_BRAIN_SMASH, dam, 0, FALSE, MS_BRAIN_SMASH, learnable);

		break;

	/* RF5_CAUSE_1 */
	case 128+12:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s��%s���w�����Ď􂢂��������B", m_name, t_name);
#else
				msg_format("%^s points at %s and curses.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(3, 8);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_1, dam, 0, FALSE, MS_CAUSE_1, learnable);

		break;

	/* RF5_CAUSE_2 */
	case 128+13:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s��%s���w�����ċ��낵���Ɏ􂢂��������B", m_name, t_name);
#else
				msg_format("%^s points at %s and curses horribly.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(8, 8);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_2, dam, 0, FALSE, MS_CAUSE_2, learnable);

		break;

	/* RF5_CAUSE_3 */
	case 128+14:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s��%s���w�����A���낵���Ɏ������������I", m_name, t_name);
#else
				msg_format("%^s points at %s, incanting terribly!", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(10, 15);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_3, dam, 0, FALSE, MS_CAUSE_3, learnable);

		break;

	/* RF5_CAUSE_4 */
	case 128+15:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				///msg131221
				msg_format("%^s��%s�ɑ΂��Ď��̌����������B", m_name, t_name);
#else
				msg_format("%^s points at %s, screaming the word, 'DIE!'", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(15, 15);
		monst_breath_monst(m_idx, y, x, GF_CAUSE_4, dam, 0, FALSE, MS_CAUSE_4, learnable);

		break;

	/* RF5_BO_ACID */
	case 128+16:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%s��%s�Ɍ������ăA�V�b�h�E�{���g�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts an acid bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(7, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_ACID,
				 dam, MS_BOLT_ACID, learnable);

		break;

	/* RF5_BO_ELEC */
	case 128+17:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ăT���_�[�E�{���g�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a lightning bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(4, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_ELEC,
				 dam, MS_BOLT_ELEC, learnable);

		break;

	/* RF5_BO_FIRE */
	case 128+18:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ăt�@�C�A�E�{���g�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a fire bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(9, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_FIRE,
				 dam, MS_BOLT_FIRE, learnable);

		break;

	/* RF5_BO_COLD */
	case 128+19:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ăA�C�X�E�{���g�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a frost bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (damroll(6, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
		monst_bolt_monst(m_idx, y, x, GF_COLD,
				 dam, MS_BOLT_COLD, learnable);

		break;

	/* RF5_BA_LITE */
	case 128+20:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				if (blind)
				{
#ifdef JP
					msg_format("%^s��������͋����Ԃ₢���B", m_name);
#else
					msg_format("%^s mumbles powerfully.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s��%s�ɑ΂��đM���̗��̎�����O�����B", m_name, t_name);
#else
					msg_format("%^s invokes a starburst upon %s.", m_name, t_name);
#endif

				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (rlev * 4) + 50 + damroll(10, 10);
		monst_breath_monst(m_idx, y, x, GF_LITE, dam, 4, FALSE, MS_STARBURST, learnable);

		break;

	/* RF5_BO_NETH */
	case 128+21:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������Ēn���̖�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a nether bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 30 + damroll(5, 5) + (rlev * 4) / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3);
		monst_bolt_monst(m_idx, y, x, GF_NETHER,
				 dam, MS_BOLT_NETHER, learnable);

		break;

	/* RF5_BO_WATE */
	case 128+22:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ăE�H�[�^�[�E�{���g�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a water bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_WATER,
				 dam, MS_BOLT_WATER, learnable);

		break;

	/* RF5_BO_MANA */
	case 128+23:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������Ė��̖͂�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a mana bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = randint1(rlev * 7 / 2) + 50;
		monst_bolt_monst(m_idx, y, x, GF_MANA,
				 dam, MS_BOLT_MANA, learnable);

		break;

	/* RF5_BO_PLAS */
	case 128+24:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ăv���Y�}�E�{���g�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a plasma bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 10 + damroll(8, 7) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_PLASMA,
				 dam, MS_BOLT_PLASMA, learnable);

		break;

	/* RF5_BO_ICEE */
	case 128+25:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ċɊ��̖�̎������������B", m_name, t_name);
#else
				msg_format("%^s casts an ice bolt at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(6, 6) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_ICE,
				 dam, MS_BOLT_ICE, learnable);

		break;

	/* RF5_MISSILE */
	case 128+26:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�Ɍ������ă}�W�b�N�E�~�T�C���̎������������B", m_name, t_name);
#else
				msg_format("%^s casts a magic missile at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = damroll(2, 6) + (rlev / 3);
		monst_bolt_monst(m_idx, y, x, GF_MISSILE,
				 dam, MS_MAGIC_MISSILE, learnable);

		break;

	/* RF5_SCARE */
	case 128+27:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s�����낵���Ȍ��o�����o�����B", m_name, t_name);
#else
				msg_format("%^s casts a fearful illusion in front of %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flags3 & RF3_NO_FEAR)
		{
#ifdef JP
			if (see_t) msg_format("%^s�͋��|�������Ȃ��B", t_name);
#else
			if (see_t) msg_format("%^s refuses to be frightened.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^s�͋��|�������Ȃ��B", t_name);
#else
			if (see_t) msg_format("%^s refuses to be frightened.", t_name);
#endif

		}
		else
		{
			if (set_monster_monfear(t_idx, MON_MONFEAR(t_ptr) + randint0(4) + 4)) fear = TRUE;
		}

		wake_up = TRUE;

		break;

	/* RF5_BLIND */
	case 128+28:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%s�͎�����������%s�̖ڂ��Ă��t�������B", m_name, t_name);
#else
				msg_format("%^s casts a spell, burning %s%s eyes.", m_name, t_name,
					   (streq(t_name, "it") ? "s" : "'s"));
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Simulate blindness with confusion */
		if (tr_ptr->flags3 & RF3_NO_CONF)
		{
#ifdef JP
			if (see_t) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else
		{
#ifdef JP
			if (see_t) msg_format("%^s�͖ڂ������Ȃ��Ȃ����I ", t_name);
#else
			if (see_t) msg_format("%^s is blinded!", t_name);
#endif

			(void)set_monster_confused(t_idx, MON_CONFUSED(t_ptr) + 12 + randint0(4));
		}

		wake_up = TRUE;

		break;

	/* RF5_CONF */
	case 128+29:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�̑O�Ɍ��f�I�Ȍ�������o�����B", m_name, t_name);
#else
				msg_format("%^s casts a mesmerizing illusion in front of %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flags3 & RF3_NO_CONF)
		{
#ifdef JP
			if (see_t) msg_format("%^s�͘f�킳��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s disbelieves the feeble spell.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^s�͘f�킳��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s disbelieves the feeble spell.", t_name);
#endif

		}
		else
		{
#ifdef JP
			if (see_t) msg_format("%^s�͍��������悤���B", t_name);
#else
			if (see_t) msg_format("%^s seems confused.", t_name);
#endif

			(void)set_monster_confused(t_idx, MON_CONFUSED(t_ptr) + 12 + randint0(4));
		}

		wake_up = TRUE;

		break;

	/* RF5_SLOW */
	case 128+30:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%s��%s�̋ؓ�����͂��z���Ƃ����B", m_name, t_name);
#else
				msg_format("%^s drains power from %s%s muscles.", m_name, t_name,
					   (streq(t_name, "it") ? "s" : "'s"));
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flags1 & RF1_UNIQUE)
		{
#ifdef JP
			if (see_t) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else
		{

			//			if (set_monster_slow(t_idx, MON_SLOW(t_ptr) + 50))
			//v1.1.90 �t�^�ʒ���
			if (set_monster_slow(t_idx, MON_SLOW(t_ptr) + 8+randint1(8)))
			{
#ifdef JP
				if (see_t) msg_format("%s�̓������x���Ȃ����B", t_name);
#else
				if (see_t) msg_format("%^s starts moving slower.", t_name);
#endif
			}
		}

		wake_up = TRUE;

		break;

	/* RF5_HOLD */
	case 128+31:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�������ƌ��߂��B", m_name, t_name);
#else
				msg_format("%^s stares intently at %s.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if ((tr_ptr->flags1 & RF1_UNIQUE) ||
		    (tr_ptr->flags3 & RF3_NO_STUN))
		{
#ifdef JP
			if (see_t) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
		{
#ifdef JP
			if (see_t) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

		}
		else
		{
#ifdef JP
			if (see_t) msg_format("%^s�͖�Ⴢ����I", t_name);
#else
			if (see_t) msg_format("%^s is paralyzed!", t_name);
#endif

			(void)set_monster_stunned(t_idx, MON_STUNNED(t_ptr) + randint1(4) + 4);
		}

		wake_up = TRUE;

		break;


	/* RF6_HASTE */
	case 160+0:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s�������̑̂ɔO�𑗂����B", m_name);
#else
				msg_format("%^s concentrates on %s body.", m_name, m_poss);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Allow quick speed increases to base+10 */
		if (set_monster_fast(m_idx, MON_FAST(m_ptr) + 100))
		{
#ifdef JP
			if (see_m) msg_format("%^s�̓����������Ȃ����B", m_name);
#else
			if (see_m) msg_format("%^s starts moving faster.", m_name);
#endif
		}
		break;

	/* RF6_HAND_DOOM */
	case 160+1:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s��%s��<�j�ł̎�>��������I", m_name, t_name);
#else
				msg_format("%^s invokes the Hand of Doom upon %s!", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = 20; /* Dummy power */
		monst_breath_monst(m_idx, y, x, GF_HAND_DOOM, dam, 0, FALSE, MS_HAND_DOOM, learnable);

		break;

	/* RF6_HEAL */
	case 160+2:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s�͎����̏��ɔO���W�������B", m_name);
#else
				msg_format("%^s concentrates on %s wounds.", m_name, m_poss);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Heal some */
		m_ptr->hp += (rlev * 6);

		/* Fully healed */
		if (m_ptr->hp >= m_ptr->maxhp)
		{
			/* Fully healed */
			m_ptr->hp = m_ptr->maxhp;

			if (known)
			{
				if (see_m)
				{
#ifdef JP
					msg_format("%^s�͊��S�Ɏ������I", m_name);
#else
					msg_format("%^s looks completely healed!", m_name);
#endif

				}
				else
				{
					mon_fight = TRUE;
				}
			}
		}

		/* Partially healed */
		else if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s�̗͑͂��񕜂����悤���B", m_name);
#else
				msg_format("%^s looks healthier.", m_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		/* Redraw (later) if needed */
		if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
		if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

		/* Cancel fear */
		if (MON_MONFEAR(m_ptr))
		{
			/* Cancel fear */
			(void)set_monster_monfear(m_idx, 0);

			/* Message */
#ifdef JP
			if (see_m) msg_format("%^s�͗E�C�����߂����B", m_name);
#else
			if (see_m) msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif
		}

		break;

	/* RF6_INVULNER */
	case 160+3:
		if (known)
		{
			if (see_m)
			{
				disturb(1, 1);
#ifdef JP
				if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format("%^s�̑̂����̂������Â������ʂ����E�E", m_name);
				else msg_format("%s�͖����̋��̎������������B", m_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", m_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (!MON_INVULNER(m_ptr)) (void)set_monster_invulner(m_idx, randint1(4) + 4, FALSE);
		break;

	/* RF6_BLINK */
	case 160+4:

		if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(r_ptr->flags1 & RF1_QUESTOR))
		{
			if (see_m)
				msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", m_name);

			delete_monster_idx(m_idx);
			return TRUE;

		}
		else if (teleport_barrier(m_idx))
		{
			if (see_m)
			{
#ifdef JP
			msg_format("%^s�̃e���|�[�g�͖W�Q���ꂽ�B", m_name);
			//msg_format("���@�̃o���A��%^s�̃e���|�[�g���ז������B", m_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", m_name);
#endif
			}
		}
		else
		{
			if (see_m)
			{
#ifdef JP
				if(m_ptr->r_idx != MON_KOISHI || p_ptr->pclass == CLASS_KOISHI)
					msg_format("%^s���u���ɏ������B", m_name);
#else
				msg_format("%^s blinks away.", m_name);
#endif
			}
			teleport_away(m_idx, 10, 0L);
		}
		break;

	/* RF6_TPORT */
	case 160+5:
		if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(r_ptr->flags1 & RF1_QUESTOR))
		{
			if (see_m)
				msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", m_name);

			delete_monster_idx(m_idx);
			return TRUE;

		}
		else if (teleport_barrier(m_idx))
		{
			if (see_m)
			{
#ifdef JP
				
				msg_format("%^s�̃e���|�[�g�͖W�Q���ꂽ�B", m_name);
			//msg_format("���@�̃o���A��%^s�̃e���|�[�g���ז������B", m_name);
#else
				msg_format("Magic barrier obstructs teleporting of %^s.", m_name);
#endif
			}
		}
		else
		{
			if (see_m)
			{
#ifdef JP
				if(m_ptr->r_idx != MON_KOISHI || p_ptr->pclass == CLASS_KOISHI)
					msg_format("%^s���e���|�[�g�����B", m_name);
#else
				msg_format("%^s teleports away.", m_name);
#endif
			}
			teleport_away_followable(m_idx);
		}
		break;

	/* RF6_WORLD */
	case 160+6:
#if 0
		int who = 0;
		if(m_ptr->r_idx = MON_DIO) who == 1;
		else if(m_ptr->r_idx = MON_WONG) who == 3;
		dam = who;
		if(!process_the_world(randint1(2)+2, who, player_has_los_bold(m_ptr->fy, m_ptr->fx))) return (FALSE);
#endif
		return FALSE;

	/* RF6_SPECIAL */
	case 160+7:
		switch (m_ptr->r_idx)
		{
		case MON_OHMU:
			/* Moved to process_monster(), like multiplication */
			return FALSE;
		case MON_REIMU:
				if (see_m)
					msg_format("%^s�͖��z�����������I", m_name);
				cast_musou_hu_in(m_idx);
			break;
		case MON_ROLENTO:
			if (known)
			{
				if (see_either)
				{
					disturb(1, 1);

#ifdef JP
					msg_format("%^s�͎�֒e���΂�܂����B", m_name);
#else
					msg_format("%^s throws some hand grenades.", m_name);
#endif
				}
				else
				{
					mon_fight = TRUE;
				}
			}

			{
				int num = 1 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, MON_SHURYUUDAN, 0);
				}
			}

			if (known && !see_t && count)
			{
				mon_fight = TRUE;
			}
			break;
		case MON_MICHAEL:
#ifdef JP
				if (see_m)
				{
					disturb(1, 1);
					msg_format("%^s�u�R���s����I�X�s�L���[���I�v", m_name);
				}
#else
				if (blind) msg_format("%^s spreads something.", m_name);
				else msg_format("%^s throws some hand grenades.", m_name);
#endif
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1600);
				if (see_m)
				{
					if(one_in_(3))msg_format("%^s�u���̉��l�͋������낤���I�v", m_name);
					else if(one_in_(2))msg_format("%^s�u���������������I�M�����������������I�v", m_name);
					else msg_format("%^s�u���܂ł������߂Ă���I�v", m_name);
				}

				break;
			case MON_MOKOU:
				if (see_m) msg_format("%^s�̓t�W���}���H���P�C�m��������I", m_name);
				project_hack3(m_idx,m_ptr->fy,m_ptr->fx, GF_FIRE, 0,0,1000);
				break;

			case MON_SHION_2:
				msg_format("%^s�͕s�^�ƕs�K���܂��U�炵���I", m_name);
				project_hack3(m_idx, m_ptr->fy, m_ptr->fx, GF_DISENCHANT, 0, 0, 400);

				if (p_ptr->au > 0 && projectable(py,px,m_ptr->fy,m_ptr->fx) && !weird_luck())
				{
					p_ptr->au /= 2;
					msg_print("���z���������y���Ȃ����C������I");
					p_ptr->redraw |= PR_GOLD;
				}

				break;

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
						{
							if (see_m)msg_format("%^s�͉��������悤�Ƃ��������s�����悤���B",m_name);
						}
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
						count += summon_named_creature(m_idx, old_y, old_x, MON_SEIJA_D, 0L);
					}
				}
				break;


			case MON_YUKARI:
				{
					msg_format("%^s���󒆂Ɍ����J����ƁA���̒�����d�Ԃ��ːi���Ă����I", m_name);
					monst_beam_monst(m_idx, y, x, GF_TRAIN, 100+randint1(50), 0, FALSE);

				}
				break;
			case MON_SUMIREKO:
				{
					msg_format("%^s�͓S����|�����I", m_name);
					(void)project(m_idx, 0, y, x, GF_TRAIN, 100+randint1(50), (PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);
				}
				break;

				//v1.1.53�@�X�ѐ���
			case MON_TSUCHINOKO:
				if(see_m)	msg_format("�ˑR�ӂ������΂���%^s�𕢂����B", m_name);
				project(m_idx, 1, m_ptr->fy, m_ptr->fx, 1, GF_MAKE_TREE, (PROJECT_GRID), 0);

				break;

			case MON_CHIMATA:
				if (see_m)	msg_format("%^s�͓��F�̗��������N�������I", m_name);

				dam = (rlev * 4) + 50 + damroll(10, 10);
				monst_breath_monst(m_idx, y, x, GF_RAINBOW, dam, 4, FALSE, 0, FALSE);

				//(void)project(m_idx, 5, y, x, GF_RAINBOW, dam, (PROJECT_KILL | PROJECT_PLAYER | PROJECT_JUMP), -1);

				break;
				//���ΐ��N����s���@�L�͈͒n�k�A���@�̗͂������Ă���قǔ͈͂Ɖ񐔂�����
			case MON_TAISAI:
			{
				int tmp_rad = 5 + (m_ptr->maxhp - m_ptr->hp) / 1000;
				int tmp_num = 2 + (m_ptr->maxhp - m_ptr->hp) / 3000;


				if (see_m)	msg_format("%^s�͑�n�k���N�������I", m_name);
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
				//v1.1.62 ���l���h�[���ɍr�n��炵�ŋR�悵�Ă���Ƃ��ɂ����ɗ��ċR���Ԃ��ςɂȂ����Ƃ����񍐂���B
				//p_ptr->riding��0�̂܂܋R�悵�Ă���Ƃ������ƂɂȂ邪�A�Ƃ肠�����}�~���Ă���
				if (one_in_(3) && !player_bold(m_ptr->fy,m_ptr->fx))
				{
					if (see_m)
					{
#ifdef JP
						msg_format("%^s�͓ˑR�}�㏸���Ď��E���������!", m_name);
#else
						msg_format("%^s suddenly go out of your sight!", m_name);
#endif
					}
					teleport_away(m_idx, 10, TELEPORT_NONMAGICAL);
					p_ptr->update |= (PU_MONSTERS);
				}
				else
				{
					if (known)
					{
						if (see_either)
						{
#ifdef JP
							msg_format("%^s��%s��͂�ŋ󒆂��瓊�����Ƃ����B", m_name, t_name);
#else
							msg_format("%^s holds %s, and drops from the sky.", m_name, t_name);
#endif

						}
						else
						{
							mon_fight = TRUE;
						}
					}

					dam = damroll(4, 8);

					if (t_idx == p_ptr->riding) teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
					else teleport_monster_to(t_idx, m_ptr->fy, m_ptr->fx, 100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);

					sound(SOUND_FALL);

					if (tr_ptr->flags7 & RF7_CAN_FLY)
					{
#ifdef JP
						if (see_t) msg_format("%^s�͐Â��ɒ��n�����B", t_name);
#else
						if (see_t) msg_format("%^s floats gently down to the ground.", t_name);
#endif
					}
					else
					{
#ifdef JP
						if (see_t) msg_format("%^s�͒n�ʂɒ@������ꂽ�B", t_name);
#else
						if (see_t) msg_format("%^s crashed into the ground.", t_name);
#endif
						dam += damroll(6, 8);
					}

					if (p_ptr->riding == t_idx)
					{
						int get_damage = 0;

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
					}

					mon_take_hit_mon(t_idx, dam, &fear, extract_note_dies(real_r_ptr(t_ptr)), m_idx);
				}
				break;
			}

			/* Something is wrong */
			else return FALSE;
		}

		/* done */
		break;

	/* RF6_TELE_TO */
	case 160+8:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�������߂����B", m_name, t_name);
#else
				msg_format("%^s commands %s to return.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flagsr & RFR_RES_TELE)
		{
			if ((tr_ptr->flags1 & RF1_UNIQUE) || (tr_ptr->flagsr & RFR_RES_ALL))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
					msg_format("%^s is unaffected!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
			else if (tr_ptr->level > randint1(100))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^s�͑ϐ��������Ă���I", t_name);
#else
					msg_format("%^s resists!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
		}

		if (!resists_tele)
		{
			if (t_idx == p_ptr->riding) teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			else teleport_monster_to(t_idx, m_ptr->fy, m_ptr->fx, 100, TELEPORT_PASSIVE);
		}

		wake_up = TRUE;
		break;

	/* RF6_TELE_AWAY */
	case 160+9:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s���e���|�[�g�������B", m_name, t_name);
#else
				msg_format("%^s teleports %s away.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (tr_ptr->flagsr & RFR_RES_TELE || teleport_barrier(t_idx))
		{
			if ((tr_ptr->flags1 & RF1_UNIQUE) || (tr_ptr->flagsr & RFR_RES_ALL))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
					msg_format("%^s is unaffected!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
			else if (tr_ptr->level > randint1(100))
			{
				if (is_original_ap_and_seen(t_ptr)) tr_ptr->r_flagsr |= RFR_RES_TELE;
				if (see_t)
				{
#ifdef JP
					msg_format("%^s�͑ϐ��������Ă���I", t_name);
#else
					msg_format("%^s resists!", t_name);
#endif
				}

				resists_tele = TRUE;
			}
		}

		if (!resists_tele)
		{
			if (t_idx == p_ptr->riding) teleport_player_away(m_idx, MAX_SIGHT * 2 + 5);

			else if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(tr_ptr->flags1 & RF1_QUESTOR))
			{
				if (see_either)
					msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", t_name);

				delete_monster_idx(t_idx);
				return TRUE;

			}

			else teleport_away(t_idx, MAX_SIGHT * 2 + 5, TELEPORT_PASSIVE);
		}

		wake_up = TRUE;
		break;

	/* RF6_TELE_LEVEL */
	case 160+10:
		if (known)
		{
			if (see_either)
			{
#ifdef JP
				msg_format("%^s��%s�̑����w�������B", m_name, t_name);
#else
				msg_format("%^s gestures at %s's feet.", m_name, t_name);
#endif
			}
			else
			{
				mon_fight = TRUE;
			}
		}


		if (tr_ptr->flagsr & (RFR_EFF_RES_NEXU_MASK | RFR_RES_TELE))
		{
#ifdef JP
			if (see_t) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", t_name);
#else
			if (see_t) msg_format("%^s is unaffected!", t_name);
#endif
		}


		else if ((tr_ptr->flags1 & RF1_QUESTOR) ||
			    (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10))
		{
#ifdef JP
			if (see_t) msg_format("%^s�͌��͂𒵂˕Ԃ����I", t_name);
#else
			if (see_t) msg_format("%^s resist the effects!", t_name);
#endif
		}
		else teleport_level((t_idx == p_ptr->riding) ? 0 : t_idx);

		wake_up = TRUE;
		break;

	/* RF6_PSY_SPEAR */
	case 160+11:
		if (known)
		{
			if (see_either)
			{
				if(m_ptr->r_idx == MON_REMY) msg_format("%^s��%s�Ɍ������ăO���O�j���𓊂������B", m_name, t_name);

				else msg_format("%^s��%s�Ɍ������Č��̌���������B", m_name, t_name);

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		dam = (r_ptr->flags2 & RF2_POWERFUL) ? (randint1(rlev * 2) + 180) : (randint1(rlev * 3 / 2) + 120);
		if(m_ptr->r_idx == MON_REMY) monst_beam_monst(m_idx, y, x, GF_GUNGNIR, dam, MS_PSY_SPEAR, learnable);
		else  monst_beam_monst(m_idx, y, x,GF_PSY_SPEAR, dam, MS_PSY_SPEAR, learnable);
		break;

	/* RF6_DARKNESS */
	case 160+12:
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				if (can_use_lite_area) msg_format("%^s���ӂ�𖾂邭�Ƃ炵���B", m_name);
				else msg_format("%^s���Èł̒��Ŏ��U�����B", m_name);
#else
				if (can_use_lite_area) msg_format("%^s cast a spell to light up.", m_name);
				else msg_format("%^s gestures in shadow.", m_name);
#endif

				if (see_t)
				{
#ifdef JP
					if (can_use_lite_area) msg_format("%^s�͔������ɕ�܂ꂽ�B", t_name);
					else msg_format("%^s�͈Èłɕ�܂ꂽ�B", t_name);
#else
					if (can_use_lite_area) msg_format("%^s is surrounded by a white light.", t_name);
					else msg_format("%^s is surrounded by darkness.", t_name);
#endif
				}
			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (can_use_lite_area)
		{
			(void)project(m_idx, 3, y, x, 0, GF_LITE_WEAK, PROJECT_GRID | PROJECT_KILL, -1);
			lite_room(y, x);
		}
		else
		{
			(void)project(m_idx, 3, y, x, 0, GF_DARK_WEAK, PROJECT_GRID | PROJECT_KILL, MS_DARKNESS);
			unlite_room(y, x);
		}

		break;

	/* RF6_TRAPS */
	case 160+13:
#if 0
		if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s�������������Ď׈��ɔ��΂񂾁B", m_name);
#else
				msg_format("%^s casts a spell and cackles evilly.", m_name);
#endif
			}
			else
			{
#ifdef JP
				msg_format("%^s���������Ԃ₢���B", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif
			}
		}

		trap_creation(y, x);

		break;
#else
		/* Not implemented */
		return FALSE;
#endif

	/* RF6_FORGET */
	case 160+14:
		/* Not implemented */
		return FALSE;

	/* RF6_RAISE_DEAD */
	case 160+15:
			if (known)
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%^s��%s�̐��_�𖼏󂵂������F���I���|�ւƊׂꂽ�B", m_name, t_name);
#else
				msg_format("%^s points at %s and curses.", m_name, t_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}
		dam = (rlev * 4) + 50 + damroll(10, 10);

		monst_breath_monst(m_idx, y, x, GF_COSMIC_HORROR, dam, 0, FALSE, MS_COSMIC_HORROR, learnable);

		break;

	/* RF6_S_KIN */
	case 160+16:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

				//if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
				if (m_ptr->r_idx == MON_SERPENT)
				{
#ifdef JP
					msg_format("%^s�����ׂ̋��͂ȑ��݂����������B", m_name);
#else
					msg_format("%^s magically summons guardians of dungeons.", m_name);
#endif
				}
				else if(m_ptr->r_idx == MON_ALICE)
				{
					msg_format("%^s���l�`�B���Ăяo�����B", m_name);
				}
				else if(m_ptr->r_idx == MON_SUIKA)
				{
					msg_format("%^s�������ȕ��g���Ăяo�����B", m_name);
				}
				else if(m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
				{
					msg_format("%^s�͎艺�ƒ��Ԃ��Ă񂾁I", m_name);
				}
				else if(m_ptr->r_idx == MON_YUKARI)
				{
					msg_format("%^s�������Ăяo�����B", m_name);
				}
				else if(m_ptr->r_idx == MON_NAPPA)
				{
					msg_format("%^s�͒n�ʂɉ�����A�����E�E", m_name);
				}
				else if (m_ptr->r_idx == MON_MAYUMI || m_ptr->r_idx == MON_KEIKI)
				{
					msg_format("%^s�͔z���̏��֒B���Ăяo�����B", m_name);
				}
				else if (m_ptr->r_idx == MON_LUNASA || m_ptr->r_idx == MON_MERLIN || m_ptr->r_idx == MON_LYRICA)
				{
					msg_format("%^s�͎o�����Ăяo�����B", m_name);
				}

				else
				{
#ifdef JP
					msg_format("%s�����@��%s�����������B", m_name,
						   ((r_ptr->flags1 & RF1_UNIQUE) ? "�艺" : "����"));
#else
					msg_format("%^s magically summons %s %s.", m_name, m_poss,
						   ((r_ptr->flags1 & RF1_UNIQUE) ? "minions" : "kin"));
#endif
				}

			}
			else
			{
				mon_fight = TRUE;
			}
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

		case MON_BULLGATES:
			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, MON_IE, 0);
				}
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


		case MON_SERPENT:
	//	case MON_ZOMBI_SERPENT:
			/*
			if (r_info[MON_JORMUNGAND].cur_num < r_info[MON_JORMUNGAND].max_num && one_in_(6))
			{
				if (known && see_t)
				{
#ifdef JP
					msg_print("�n�ʂ��琅�������o�����I");
#else
					msg_print("Water blew off from the ground!");
#endif
				}
				project(t_idx, 8, y, x, 3, GF_WATER_FLOW, PROJECT_GRID | PROJECT_HIDE, -1);
			}

			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			*/
			count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

			break;

		case MON_CALDARM:
			{
				int num = randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, MON_LOCKE_CLONE, 0);
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

		case MON_ORIN:
			{
				if(one_in_(2))
				{
					num = 1 + randint1(2);
					for (k = 0; k < num; k++)
						count += summon_named_creature(m_idx, y, x, MON_ZOMBIE_FAIRY, 0);
				}
				else
				{
					count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					if(one_in_(2))count += summon_named_creature(m_idx, y, x, MON_G_ONRYOU, 0);
					count += summon_named_creature(m_idx, y, x, MON_ONRYOU, 0);

				}

			}
			break;
		case MON_LOUSY:
			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_LOUSE, (PM_ALLOW_GROUP));
				}
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
					count += summon_named_creature(m_idx, y, x, MON_CHAOS_BLADE, 0);
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
					count += summon_named_creature(m_idx, m_ptr->fy, m_ptr->fx, MON_SAIBAIMAN, 0);
				}
			}
			break;
		case MON_MAYUMI:
		case MON_KEIKI:
		{
			int num = 4 + randint1(3);
			u32b s_mode = (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);

			if (is_hostile(m_ptr)) s_mode |= PM_FORCE_ENEMY;

			for (k = 0; k < num; k++)
			{
				count += summon_specific(m_idx, m_ptr->fy, m_ptr->fx, rlev, SUMMON_HANIWA, s_mode);
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
			summon_kin_type = r_ptr->d_char;

			for (k = 0; k < 4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_KIN, (PM_ALLOW_GROUP));
			}
			break;
		}

		if(see_either && !count) msg_print("��������������Ȃ������悤���B");

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}

		break;

	/* RF6_S_CYBER */
	case 160+17:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s���T�C�o�[�f�[���������������I", m_name);
#else
				msg_format("%^s magically summons Cyberdemons!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		if (is_friendly(m_ptr))
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_CYBER, (PM_ALLOW_GROUP));
		}
		else
		{
			count += summon_cyber(m_idx, y, x);
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_MONSTER */
	case 160+18:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�Œ��Ԃ����������I", m_name);
#else
				msg_format("%^s magically summons help!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		count += summon_specific(m_idx, y, x, rlev, 0, (u_mode));

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_MONSTERS */
	case 160+19:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�Ń����X�^�[�����������I", m_name);
#else
				msg_format("%^s magically summons monsters!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | u_mode));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_ANT */
	case 160+20:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�ŃA�������������B", m_name);
#else
				msg_format("%^s magically summons ants.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_ANT, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_SPIDER */
	case 160+21:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�Œ������������B", m_name);
#else
				msg_format("%^s magically summons spiders.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_SPIDER, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_HOUND */
	case 160+22:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�Ńn�E���h�����������B", m_name);
#else
				msg_format("%^s magically summons hounds.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HOUND, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_HYDRA */
	case 160+23:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�Ńq�h�������������B", m_name);
#else
				msg_format("%^s magically summons hydras.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HYDRA, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_ANGEL */
	case 160+24:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�œV�g�����������I", m_name);
#else
				msg_format("%^s magically summons an angel!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		{
			int num = 1;

			if ((r_ptr->flags1 & RF1_UNIQUE) && !easy_band)
			{
				num += rlev/40;
			}

			for (k = 0; k < num; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANGEL, (PM_ALLOW_GROUP));
			}
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_DEMON */
	case 160+25:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				///msg131221
				msg_format("%^s�����@�Ńf�[���������������I", m_name);
#else
				msg_format("%^s magically summons a demon from the Courts of Chaos!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < 1; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_DEMON, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_UNDEAD */
	case 160+26:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%s�����@�ŃA���f�b�h�����������B", m_name);
#else
				msg_format("%^s magically summons undead.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < 1; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_UNDEAD, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_DRAGON */
	case 160+27:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�Ńh���S�������������I", m_name);
#else
				msg_format("%^s magically summons a dragon!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < 1; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_DRAGON, (PM_ALLOW_GROUP));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_HI_UNDEAD */
	case 160+28:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%s�����@�ŃA���f�b�h�����������B", m_name);
#else
				msg_format("%^s magically summons undead.", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_6; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | u_mode));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_HI_DRAGON */
	case 160+29:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�ŌÑ�h���S�������������I", m_name);
#else
				msg_format("%^s magically summons ancient dragons!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | u_mode));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_AMBERITES */
	case 160+30:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s���A���o�[�̉��������������I", m_name);
#else
				msg_format("%^s magically summons Lords of Amber!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
		}

		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_UNIQUE */
	case 160+31:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);

#ifdef JP
				msg_format("%^s�����@�œ��ʂȋ��G�����������I", m_name);
#else
				msg_format("%^s magically summons special opponents!", m_name);
#endif

			}
			else
			{
				mon_fight = TRUE;
			}
		}

		for (k = 0; k < s_num_4; k++)
		{
			count += summon_specific(m_idx, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
		}

		{
			int non_unique_type = SUMMON_HI_UNDEAD;

			if ((m_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
				non_unique_type = 0;
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD)
				non_unique_type = SUMMON_ANGEL;

			for (k = count; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, non_unique_type, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
		}
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		if (known && !see_t && count)
		{
			mon_fight = TRUE;
		}
		break;


		//��������V�X�y��


		/* RF9_FIRE_STORM */
		case 192+0:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ĉ��̗��̎������������B", m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_FIRE, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_ICE_STORM */
		case 192+1:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ĕX�̗��̎������������B", m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_COLD, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_THUNDER_STORM */
		case 192+2:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ė��̗��̎������������B", m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_ELEC, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_ACID_STORM */
		case 192+3:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������Ď_�̗��̎������������B", m_name,t_name);
		}
		rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 10) + 100 + randint1(100);
			else	dam = (rlev * 7) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_ACID, dam, rad, FALSE, 0, FALSE);
			break;
		
		/* RF9_TOXIC_STORM */
		case 192+4:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ēőf�̗��̎������������B", m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 8) + 50 + randint1(50);
			else	dam = (rlev * 5) + 25 + randint1(25);
			monst_breath_monst(m_idx, y, x, GF_POIS, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_BA_POLLUTE */
		case 192+5:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s����������ȉ��𗧂Ă��B", m_name);
			else msg_format("%^s��%s�Ɍ������ĉ����̋��̎������������B", m_name,t_name);
		}
			rad = 4;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 50 + damroll(10, 10);
			else	dam = (rlev * 3) + 50 + randint1(50);
			monst_breath_monst(m_idx, y, x, GF_POLLUTE, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_BA_DISI */
		case 192+6:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) msg_format("%^s�͂ǂ����炩����Ȕ��e�����o�����I", m_name);
			else if (blind) msg_format("%^s�������������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������Č��q�����̎������������B", m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 2) + 50 + randint1(50);
			else	dam = rlev + 25 + randint1(25);
			monst_breath_monst(m_idx, y, x, GF_DISINTEGRATE, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_BA_HOLY */
		case 192+7:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������r�������B", m_name);
			else msg_format("%^s��%s�Ɍ������Ĕj�ׂ̌����̎������������B", m_name,t_name);
		}
			rad = 2;
			dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			monst_breath_monst(m_idx, y, x, GF_HOLY_FIRE, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_METEOR */
		case 192+8:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ă��e�I�X�g���C�N�̎������������I", m_name,t_name);
		}
			if (r_ptr->flags2 & RF2_POWERFUL)
			{
				rad = 5;
				//v2.0 ������̉��@�ǔ����Ōy���ł��Ȃ�����w���t�@�C�A�Ƃ������Ғl���Ⴍ�Ă�����
				//dam = (rlev * 4) + damroll(1, 300);
				dam = (rlev * 3) + damroll(1,250);			}
			else
			{
				rad = 4;
				dam = (rlev * 2) + damroll(1,150);
			}
			monst_breath_monst(m_idx, y, x, GF_METEOR, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_BA_DISTORTION */
		case 192+9:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s���������G�Ȏ������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ċ�Ԙc�Ȃ̎������������I", m_name,t_name);
		}
			rad = 4;
			dam = 50 + damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 1 : 2));
			monst_breath_monst(m_idx, y, x, GF_DISTORTION, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_1 */
		case 192+10:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format("%^s��%s�Ɍ������Ė������̂܂��Ȃ����������B", m_name,t_name);
			rad = 0;
		}
		dam = damroll(3, 8);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_1, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_2 */
		case 192+11:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format("%^s��%s�Ɍ������Đ��Ȃ錾�t���������B", m_name,t_name);
		}
			rad = 0;
			dam = damroll(8, 8);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_2, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_3 */
		case 192+12:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format("%^s��%s�Ɍ������đޖ��̎������������B", m_name,t_name);
			rad = 0;
		}		
			dam = damroll(10, 15);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_3, dam, rad, FALSE, 0, FALSE);
			break;
		/* RF9_PUNISH_4 */
		case 192+13:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else msg_format("%^s��%s�Ɍ������Ĕj�ׂ̈��������I", m_name,t_name);
		}
			rad = 0;
			dam = damroll(15, 15);
			monst_breath_monst(m_idx, y, x, GF_PUNISH_4, dam, rad, FALSE, 0, FALSE);
			break;
	/* RF9_BO_HOLY */
	case 192+14:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format("%^s��%s�Ɍ������Đ��Ȃ��̎������������B", m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
		monst_bolt_monst(m_idx, y, x, GF_HOLY_FIRE, dam, 0, FALSE);

		break;
	/* RF9_GIGANTIC_LASER */
	case 192+15:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				if(m_ptr->r_idx == MON_MARISA)
					msg_format("%^s��%s�Ɍ������ă}�X�^�[�X�p�[�N��������I", m_name, t_name);
				else
					msg_format("%^s��%s�Ɍ������ċ���ȃ��[�U�[��������I", m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		dam = damroll(10, 10) +  rlev * (((r_ptr->flags2 & RF2_POWERFUL) ? 8 : 4));
		masterspark(y, x, m_idx, GF_NUKE, dam, 0, FALSE,((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));			

		break;
	/* RF9_BO_SOUND */
	case 192+16:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format("%^s��%s�Ɍ������ĉ����̒e��������B", m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		if (r_ptr->flags2 & RF2_POWERFUL)	dam = damroll(10, 10) + (rlev * 3 / 2);
		else	dam = damroll(3, 10) + rlev;
		monst_bolt_monst(m_idx, y, x, GF_SOUND, dam, 0, FALSE);
		break;

	/* RF6_S_ANIMAL */
	case 192+17:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format("%^s�����������������B", m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_ANIMAL, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;
	/* RF9_BEAM_LITE */
	case 192+18:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format("%^s��%s�Ɍ������ă��[�U�[��������B", m_name, t_name);
			}
			else mon_fight = TRUE;
		}

		if(m_ptr->r_idx == MON_BANKI_HEAD_2)
		{
			monst_beam_monst(m_idx, y, x, GF_BANKI_BEAM, rlev, 0, FALSE);
		}
		else
		{
			dam = (r_ptr->flags2 & RF2_POWERFUL) ? (rlev * 4) : (rlev * 2);
			monst_beam_monst(m_idx, y, x, GF_LITE, dam, 0, FALSE);
		}
		break;
	/* RF9_HELLFIRE */
	case 192+20:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s�������ЁX�����������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ăw���E�t�@�C�A�̎������������B", m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);

			monst_breath_monst(m_idx, y, x, GF_HELL_FIRE, dam, rad, FALSE, 0, FALSE);
			break;

	/* RF9_HOLYFIRE */
	case 192+21:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("%^s�������_�X�����������������B", m_name);
			else msg_format("%^s��%s�Ɍ������ăz�[���[�E�t�@�C�A�̎������������B", m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 4) + 100 + damroll(10,10);
			else	dam = (rlev * 3) + 50 + damroll(10,5);

			monst_breath_monst(m_idx, y, x, GF_HOLY_FIRE, dam, rad, FALSE, 0, FALSE);
			break;

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
else msg_format("%^s��%s�֌����ăt�@�C�i���X�p�[�N��������I", m_name, t_name);
#endif

			dam = 200 + randint1(rlev) +  rlev * 2;
			masterspark(y, x, m_idx, GF_DISINTEGRATE, dam, 0, FALSE,2);	
			/*:::Hack - �t�@�C�i���X�p�[�N���ˌ�́����s������܂Ŗ��@���g��Ȃ�*/
			m_ptr->mflag |= (MFLAG_NICE);
			repair_monsters = TRUE;
			break;
		}


	/* RF9_TORNADO */
	case 192+23:
		if (known)
		{
			disturb(1, 1);
			if(!see_either) mon_fight = TRUE;
			else if (blind) msg_format("�ˑR�˕��������r�ꂽ�B", m_name);
			else msg_format("%^s��%s�̂���Ƃ���ɗ������N�������B", m_name,t_name);
		}
			rad = 5;
			if (r_ptr->flags2 & RF2_POWERFUL)	dam = (rlev * 3) + 50 + randint1(50);
			else	dam = (rlev * 2) + 25 + randint1(25);

			monst_breath_monst(m_idx, y, x, GF_TORNADO, dam, rad, FALSE, 0, FALSE);
			break;

		/* RF9_DESTRUCTION */
	case 192+24:
		{

			disturb(1, 1);
			if(!known) msg_format("����Ȕ������������A��u�_���W�������h�ꂽ�B");
			else if (blind) msg_format("%^s��������������ƒn�����������c", m_name);
			else msg_format("%^s���j��̌��t���r�������I", m_name);
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
					//v1.1.86 NOESCAPE��ATTACK��
					if(p_ptr->pclass != CLASS_CLOWNPIECE) 
						take_hit(DAMAGE_ATTACK, dam, m_name, -1);
					if(p_ptr->pseikaku != SEIKAKU_BERSERK || one_in_(7))
					{
						msg_format("���Ȃ��͐�����΂��ꂽ�I", m_name);
						teleport_player_away(m_idx, 100);
					}
				}
			}

			break;
		}

	/* RF6_S_DEITY */
	case 192+25:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format("%^s���_�i�����������B", m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_DEITY, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_HI_DEMON */
	case 192+26:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format("%^s����ʈ��������������I", m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DEMON, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

	/* RF6_S_KWAI */
	case 192+27:
		if (known)
		{
			if (see_either)
			{
				disturb(1, 1);
				msg_format("%^s���d�������������I", m_name);
			}
			else	mon_fight = TRUE;
		}

		for (k = 0; k < s_num_4; k++)
			count += summon_specific(m_idx, y, x, rlev, SUMMON_KWAI, (PM_ALLOW_GROUP));
		if (known && !see_t && count)	mon_fight = TRUE;
		if(see_either && !count) msg_print("��������������Ȃ������悤���B");
		break;

		///pend �Ƃ肠�������󃂃��X�^�[���m�̎��E�O�אڃe���|�͎��E���ł�����΂Ȃ�
		/* RF9_TELE_APPROACH */
	case 192+28:
		/* RF9_TELE_HI_APPROACH */
	case 192+29:
		{
			if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && !(r_ptr->flags1 & RF1_QUESTOR))
			{
				if (see_m)
					msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", m_name);

				delete_monster_idx(m_idx);
				return TRUE;
			}

			if (teleport_barrier(m_idx)) break;
			disturb(1, 1);
			teleport_monster_to(m_idx, y, x, 100, 0L);	
			if (see_either)
				msg_format("%s���u����%s�̋߂��ɏo�������B", m_name,t_name);
			break;
		}
	//RF9_MAELSTROM ���Z��ȊO�Ŏg��Ȃ�
	case 192+30:
		{
			int rad;
			disturb(1, 1);
			if (see_either)
			{
				if(m_ptr->r_idx == MON_YUMA)
					msg_format("%^s���Ζ��̑�Q�������N�������I", m_name);
				else
					msg_format("%^s�����C���V���g�����̎������r�������B", m_name);

			}

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


			break;
		}


	/* RF9_ALARM */
	case 192+31:
		if (known)
		{

			if (see_m)
			{
				if(m_ptr->r_idx == MON_CLOWNPIECE)
					msg_format("%^s�̎����������X�ƋP�����I", m_name);
				else
					msg_format("%^s�͋���Ȍx�񉹂𔭂����I", m_name);
			}
			else
			{
				msg_format("�b�����x�񉹂����������I", m_name);
			}
		}

		wake_up = TRUE;

		break;



	default:
		msg_format("ERROR:monst_spell_monst()�ɂăX�y��%d�̏��������L�q�ł���",thrown_spell);


	} //�����X�^�[���@������switch()���I��

	if (wake_up) (void)set_monster_csleep(t_idx, 0);

	if (fear && see_t)
	{
#ifdef JP
		msg_format("%^s�͋��|���ē����o�����I", t_name);
#else
		msg_format("%^s flees in terror!", t_name);
#endif
	}


	//v1.1.82 �V�E�Ɓu�e�������Ɓv�̓��Z�K������
	if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		//���̎��_�œ��Z�g�p�����X�^�[�����Ȃ��ꍇ�A�������ʓ|�ɂȂ�̂ŏK���s��
		if (m_ptr->r_idx)
		{
			learn_monspell_new(thrown_spell, m_ptr, FALSE, learnable,caster_dist);

		}

	}
	if (m_ptr->ml && maneable && !world_monster && !p_ptr->blind && (p_ptr->pclass == CLASS_IMITATOR))
	{
		if (thrown_spell != 167) /* Not RF6_SPECIAL */
		{
			if (p_ptr->mane_num == MAX_MANE)
			{
				p_ptr->mane_num--;
				for (i = 0; i < p_ptr->mane_num - 1; i++)
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

	if(maneable) kyouko_echo(FALSE,thrown_spell);


	/* Remember what the monster did, if we saw it */
	///sys f9�t���O�ǉ�
	if (can_remember)
	{
		/* Inate spell */
		if (thrown_spell < 32*4)
		{
			r_ptr->r_flags4 |= (1L << (thrown_spell - 32*3));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Bolt or Ball */
		else if (thrown_spell < 32*5)
		{
			r_ptr->r_flags5 |= (1L << (thrown_spell - 32*4));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Special spell */
		else if (thrown_spell < 32*6)
		{
			r_ptr->r_flags6 |= (1L << (thrown_spell - 32*5));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
		else if (thrown_spell < 32 * 7)
		{
			r_ptr->r_flags9 |= (1L << (thrown_spell - 32 * 6));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
	}

	/* Always take note of monsters that kill you */
	if (p_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !p_ptr->inside_arena)
	{
		r_ptr->r_deaths++; /* Ignore appearance difference */
	}

	if(suisidebomb && !p_ptr->is_dead) delete_monster_idx(m_idx);

	/* A spell was cast */
	return TRUE;
}
