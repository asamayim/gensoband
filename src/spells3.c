/* File: spells3.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spell code (part 3) */

#include "angband.h"

/* Maximum number of tries for teleporting */
#define MAX_TRIES 100

/* 1/x chance of reducing stats (for elemental attacks) */
#define HURT_CHANCE 16

/*:::�w�肵�������X�^�[���w�肵���O���b�h�Ƀe���|�[�g���Ă���邩���肷��*/
static bool cave_monster_teleportable_bold(int m_idx, int y, int x, u32b mode)
{
	monster_type *m_ptr = &m_list[m_idx];
	cave_type    *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Require "teleportable" space */
	if (!have_flag(f_ptr->flags, FF_TELEPORTABLE)) return FALSE;

	if (c_ptr->m_idx && (c_ptr->m_idx != m_idx)) return FALSE;
	if (player_bold(y, x)) return FALSE;

	/* Hack -- no teleport onto glyph of warding */
	if (is_glyph_grid(c_ptr)) return FALSE;
	if (is_explosive_rune_grid(c_ptr)) return FALSE;

	if (!(mode & TELEPORT_PASSIVE))
	{
		if (!monster_can_cross_terrain(c_ptr->feat, &r_info[m_ptr->r_idx], 0)) return FALSE;
	}

	return TRUE;
}


/*
 * Teleport a monster, normally up to "dis" grids away.
 *
 * Attempt to move the monster at least "dis/2" grids away.
 *
 * But allow variation to prevent infinite loops.
 */
bool teleport_away(int m_idx, int dis, u32b mode)
{
	int oy, ox, d, i, min;
	int tries = 0;
	int ny = 0, nx = 0;

	bool look = TRUE;

	monster_type *m_ptr = &m_list[m_idx];

	/* Paranoia */
	if (!m_ptr->r_idx) return (FALSE);

	/* Save the old location */
	oy = m_ptr->fy;
	ox = m_ptr->fx;

	//v1.1.63�ǉ�
	/* Verify max distance */
	if (dis > 200) dis = 200;

	 /* Minimum distance */
	min = dis / 2;

	if ((mode & TELEPORT_DEC_VALOUR) &&
	    (((p_ptr->chp * 10) / p_ptr->mhp) > 5) &&
		(4+randint1(5) < ((p_ptr->chp * 10) / p_ptr->mhp)))
	{
		chg_virtue(V_VALOUR, -1);
	}

	/* Look until done */
	while (look)
	{
		tries++;


		/* Verify max distance */
		//if (dis > 200) dis = 200; v1.1.63 ���[�v�̊O�ɏo�����Bdis������Ȓl����min��dis�𒴂��ă��[�v���邽��

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				ny = rand_spread(oy, dis);
				nx = rand_spread(ox, dis);
				d = distance(oy, ox, ny, nx);
				if(QRKDR ) break; //�}�����蒆�̓A�E�F�C�����Œ�l�𖳎�����
				if ((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if (!in_bounds(ny, nx)) continue;

			if (!cave_monster_teleportable_bold(m_idx, ny, nx, mode)) continue;

			//v1.1.24 �}�����蒆�͍��m���ŉ����ɂ����s���Ȃ�
			if(QRKDR && (nx < ox) && !one_in_(10)) continue;

			/* No teleporting into vaults and such */
			if (!(p_ptr->inside_quest || p_ptr->inside_arena))
				if (cave[ny][nx].info & CAVE_ICKY) continue;

			/* This grid looks good */
			look = FALSE;

			/* Stop looking */
			break;
		}

		/* Increase the maximum distance */
		dis = dis * 2;
		if (dis > 200) dis = 200;//v1.1.63�ǉ�


		/* Decrease the minimum distance */
		min = min / 2;

		/* Stop after MAX_TRIES tries */
		if (tries > MAX_TRIES) return (FALSE);
	}

	/* Sound */
	sound(SOUND_TPOTHER);

	/* Update the old location */
	cave[oy][ox].m_idx = 0;

	/* Update the new location */
	cave[ny][nx].m_idx = m_idx;

	/* Move the monster */
	m_ptr->fy = ny;
	m_ptr->fx = nx;

	/* Forget the counter target */
	reset_target(m_ptr);

	/* Update the monster (new location) */
	update_mon(m_idx, TRUE);

	/* Redraw the old grid */
	lite_spot(oy, ox);

	/* Redraw the new grid */
	lite_spot(ny, nx);

	if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
		p_ptr->update |= (PU_MON_LITE);

	return (TRUE);
}



/*
 * Teleport monster next to a grid near the given location
 */
/*:::�����X�^�[���w��ʒu�i�����̋߂��j�Ƀe���|�[�g������*/
/*:::�����X�^�[�̒ǔ��e���|�[�g�A�e���|�[�g�o�b�N�A���@�̓J�Ȃ�*/
void teleport_monster_to(int m_idx, int ty, int tx, int power, u32b mode)
{
	int ny, nx, oy, ox, d, i, min;
	int attempts = 500;
	int dis = 2;
	bool look = TRUE;
	monster_type *m_ptr = &m_list[m_idx];

	/* Paranoia */
	if (!m_ptr->r_idx) return;

	/* "Skill" test */
	/*:::�ǔ��e���|�̏ꍇpower�ɂ̓����X�^�[���x��������B����ȊO�͍��̂Ƃ���S��100�ŌĂ΂��炵��*/
	if (randint1(100) > power) return;


	/* Initialize */
	ny = m_ptr->fy;
	nx = m_ptr->fx;

	/* Save the old location */
	oy = m_ptr->fy;
	ox = m_ptr->fx;

	///mod140222 �����񂹍U���̂��߂ɒǉ�
	if(mode & TELEPORT_FORCE_NEXT) dis = 1;

	/* Minimum distance */
	min = 1;


	/*:::�ړ���I��*/
	/* Look until done */
	while (look && --attempts)
	{
		/* Verify max distance */
		if (dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				ny = rand_spread(ty, dis);
				nx = rand_spread(tx, dis);
				d = distance(ty, tx, ny, nx);
				if ((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if (!in_bounds(ny, nx)) continue;

			if (!cave_monster_teleportable_bold(m_idx, ny, nx, mode)) continue;

			/* No teleporting into vaults and such */
			/* if (cave[ny][nx].info & (CAVE_ICKY)) continue; */

			/* This grid looks good */
			look = FALSE;

			/* Stop looking */
			break;
		}

		/* Increase the maximum distance */
		dis = dis * 2;

		/* Decrease the minimum distance */
		min = min / 2;
	}

	if (attempts < 1) return;

	/* Sound */
	sound(SOUND_TPOTHER);

	/*:::�ړ�����*/
	/* Update the old location */
	cave[oy][ox].m_idx = 0;

	/* Update the new location */
	cave[ny][nx].m_idx = m_idx;

	/* Move the monster */
	m_ptr->fy = ny;
	m_ptr->fx = nx;

	/* Update the monster (new location) */
	update_mon(m_idx, TRUE);

	/* Redraw the old grid */
	lite_spot(oy, ox);

	/* Redraw the new grid */
	lite_spot(ny, nx);

	if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
		p_ptr->update |= (PU_MON_LITE);
}

/*:::�w��O���b�h�������e���|�[�g���Ă��邱�Ƃ��ł���ꏊ�����肷��*/
bool cave_player_teleportable_bold(int y, int x, u32b mode)
{
	cave_type    *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Require "teleportable" space */
	if (!have_flag(f_ptr->flags, FF_TELEPORTABLE)) return FALSE;

	/* No magical teleporting into vaults and such */
	if (!(mode & TELEPORT_NONMAGICAL) && (c_ptr->info & CAVE_ICKY)) return FALSE;

	///mod150711 �����X�^�[�𖳎����郂�[�h��ǉ�
	if (!(mode & TELEPORT_IGNORE_MON) && c_ptr->m_idx && (c_ptr->m_idx != p_ptr->riding)) return FALSE;

	/* don't teleport on a trap. */
	if (have_flag(f_ptr->flags, FF_HIT_TRAP)) return FALSE;

	if (!(mode & TELEPORT_PASSIVE))
	{
		if (!player_can_enter(c_ptr->feat, 0)) return FALSE;

		if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP))
		{
			if (!p_ptr->levitation && !p_ptr->can_swim) return FALSE;
		}

		//v1.1.85 �n�`�ǉ��@�Ɖu�����������O���鏈���͖ʓ|�Ȃ̂ł�߂�
		if (have_flag(f_ptr->flags, FF_LAVA) && !p_ptr->immune_fire && !IS_INVULN())
		{
			/* Always forbid deep lava */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Forbid shallow lava when the player don't have levitation */
			if (!p_ptr->levitation) return FALSE;
		}
		if (have_flag(f_ptr->flags, FF_ACID_PUDDLE) && !p_ptr->immune_acid && !IS_INVULN())
		{
			/* Always forbid deep lava */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Forbid shallow lava when the player don't have levitation */
			if (!p_ptr->levitation) return FALSE;
		}
		if (have_flag(f_ptr->flags, FF_POISON_PUDDLE) && !IS_INVULN())
		{
			/* Always forbid deep lava */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Forbid shallow lava when the player don't have levitation */
			if (!p_ptr->levitation) return FALSE;
		}
		if (have_flag(f_ptr->flags, FF_COLD_PUDDLE) && !p_ptr->immune_cold && !IS_INVULN())
		{
			return FALSE;
		}
		if (have_flag(f_ptr->flags, FF_ELEC_PUDDLE) && !p_ptr->immune_elec && !IS_INVULN())
		{
			return FALSE;
		}




	}

	return TRUE;
}


/*
 * Teleport the player to a location up to "dis" grids away.
 *
 * If no such spaces are readily available, the distance may increase.
 * Try very hard to move the player at least a quarter that distance.
 *
 * There was a nasty tendency for a long time; which was causing the
 * player to "bounce" between two or three different spots because
 * these are the only spots that are "far enough" way to satisfy the
 * algorithm.
 *
 * But this tendency is now removed; in the new algorithm, a list of
 * candidates is selected first, which includes at least 50% of all
 * floor grids within the distance, and any single grid in this list
 * of candidates has equal possibility to be choosen as a destination.
 */
/*:::�����e���|�[�g������B�K�؂Ȓn�_��������Ȃ��Ƃ������𑝉�������@�ڍז���*/
/*:::���݂̃A���S���Y���ł͘A���e���|�Ō��܂����n�_���������Ă��܂��͉̂��P���ꂽ�炵���@�����͌����Ȃ���*/
#define MAX_TELEPORT_DISTANCE 200

/*:::mode:TELEPORT_NONMAGICAL ���@�ȊO�̃e���|�[�g�@�E�B�U�[�h���[�h�␌����������g���h�Ȃ�*/
bool teleport_player_aux(int dis, u32b mode)
{
	int candidates_at[MAX_TELEPORT_DISTANCE + 1];
	int total_candidates, cur_candidates;
	int y = 0, x = 0, min, pick, i;

	int left = MAX(1, px - dis);
	int right = MIN(cur_wid - 2, px + dis);
	int top = MAX(1, py - dis);
	int bottom = MIN(cur_hgt - 2, py + dis);

	if (p_ptr->wild_mode) return FALSE;

	if (p_ptr->anti_tele && !(mode & TELEPORT_NONMAGICAL))
	{
#ifdef JP
		msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return FALSE;
	}

	/* Initialize counters */
	total_candidates = 0;
	for (i = 0; i <= MAX_TELEPORT_DISTANCE; i++)
		candidates_at[i] = 0;

	/* Limit the distance */
	if (dis > MAX_TELEPORT_DISTANCE) dis = MAX_TELEPORT_DISTANCE;

	/* Search valid locations */
	for (y = top; y <= bottom; y++)
	{
		for (x = left; x <= right; x++)
		{
			int d;

			/* Skip illegal locations */
			if (!cave_player_teleportable_bold(y, x, mode)) continue;

			/* Calculate distance */
			d = distance(py, px, y, x);

			/* Skip too far locations */
			if (d > dis) continue;

			/* Count the total number of candidates */
			total_candidates++;

			/* Count the number of candidates in this circumference */
			candidates_at[d]++;
		}
	}

	/* No valid location! */
	if (0 == total_candidates) return FALSE;

	/* Fix the minimum distance */
	for (cur_candidates = 0, min = dis; min >= 0; min--)
	{
		cur_candidates += candidates_at[min];

		/* 50% of all candidates will have an equal chance to be choosen. */
		if (cur_candidates && (cur_candidates >= total_candidates / 2)) break;
	}

	/* Pick up a single location randomly */
	pick = randint1(cur_candidates);

	/* Search again the choosen location */
	for (y = top; y <= bottom; y++)
	{
		for (x = left; x <= right; x++)
		{
			int d;

			/* Skip illegal locations */
			if (!cave_player_teleportable_bold(y, x, mode)) continue;

			/* Calculate distance */
			d = distance(py, px, y, x);

			/* Skip too far locations */
			if (d > dis) continue;

			/* Skip too close locations */
			if (d < min) continue;

			/* This grid was picked up? */
			pick--;
			if (!pick) break;
		}

		/* Exit the loop */
		if (!pick) break;
	}

	if (player_bold(y, x)) return FALSE;

	/* Sound */
	sound(SOUND_TELEPORT);
	///sysdel
#ifdef JP
	//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
	//	msg_format("�w�����������A%s�x", player_name);
#endif

	/* Move the player */
	(void)move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);

	return TRUE;
}

/*:::�����_���e���|�[�g*/
///other �O��̈ʒu���L�����Ă��ĉ����ė������Â炭�Ȃ�悤�ɂł��Ȃ���
void teleport_player(int dis, u32b mode)
{
	int yy, xx;

	/* Save the old location */
	int oy = py;
	int ox = px;
	char m_name[80];
	int chance;

	if (!teleport_player_aux(dis, mode)) return;

	/* Monsters with teleport ability may follow the player */
	for (xx = -1; xx < 2; xx++)
	{
		for (yy = -1; yy < 2; yy++)
		{
			int tmp_m_idx = cave[oy+yy][ox+xx].m_idx;

			/* A monster except your mount may follow */
			if (tmp_m_idx && (p_ptr->riding != tmp_m_idx))
			{
				monster_type *m_ptr = &m_list[tmp_m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0x00);
				chance = 50 * r_ptr->level / 128 + 40;
				///mod140103 �ǔ��e���|�[�g���t���O�����Ɏ��������B����ɐ�������͂����ɓ���ă��b�Z�[�W�ǉ��B�������������ς����B
				//���łɃp�b�V�u�e���|�ɂ͂��Ă��Ȃ��悤�ɂ��Ă������H�Ǝv���������Ă����ق����ʔ�����
				if ((r_ptr->flags2 & RF2_TELE_AFTER) && (randint1(100) < chance) && !MON_CSLEEP(m_ptr))
				{

					if((p_ptr->special_flags & SPF_ORBITAL_TRAP) && is_hostile(m_ptr) && !(r_ptr->flags1 & RF1_QUESTOR))
					{
						msg_format("%^s�͌��]�����̃g���b�v�ɂ������Ă��̃t���A����������B", m_name);

						delete_monster_idx(tmp_m_idx);
						continue;
					}
					msg_format("%s�����Ȃ��̃e���|�[�g��ǂ������Ă����I", m_name);
					teleport_monster_to(tmp_m_idx, py, px, 100, 0L);
				}
			}
		}
	}
}


void teleport_player_away(int m_idx, int dis)
{
	int yy, xx;

	/* Save the old location */
	int oy = py;
	int ox = px;

	if (!teleport_player_aux(dis, TELEPORT_PASSIVE)) return;

	/* Monsters with teleport ability may follow the player */
	for (xx = -1; xx < 2; xx++)
	{
		for (yy = -1; yy < 2; yy++)
		{
			int tmp_m_idx = cave[oy+yy][ox+xx].m_idx;

			/* A monster except your mount or caster may follow */
			if (tmp_m_idx && (p_ptr->riding != tmp_m_idx) && (m_idx != tmp_m_idx))
			{
				monster_type *m_ptr = &m_list[tmp_m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				/*
				 * The latter limitation is to avoid
				 * totally unkillable suckers...
				 */
				if ((r_ptr->flags6 & RF6_TPORT) &&
				    !(r_ptr->flagsr & RFR_RES_TELE))
				{
					if (!MON_CSLEEP(m_ptr)) teleport_monster_to(tmp_m_idx, py, px, r_ptr->level, 0L);
				}
			}
		}
	}
}


/*
 * Teleport player to a grid near the given location
 *
 * This function is slightly obsessive about correctness.
 * This function allows teleporting into vaults (!)
 */
void teleport_player_to(int ny, int nx, u32b mode)
{
	int y, x, dis = 0, ctr = 0;

	if (p_ptr->anti_tele && !(mode & TELEPORT_NONMAGICAL))
	{
#ifdef JP
		msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return;
	}

	/* Find a usable location */
	while (1)
	{
		/* Pick a nearby legal location */
		while (1)
		{
			y = rand_spread(ny, dis);
			x = rand_spread(nx, dis);
			if (in_bounds(y, x)) break;
		}

		if(mode & TELEPORT_ANYPLACE && (!cave[y][x].m_idx || (cave[y][x].m_idx == p_ptr->riding))) break;
		/* Accept any grid when wizard mode */
		if (p_ptr->wizard && !(mode & TELEPORT_PASSIVE) && (!cave[y][x].m_idx || (cave[y][x].m_idx == p_ptr->riding))) break;

		/* Accept teleportable floor grids */
		if (cave_player_teleportable_bold(y, x, mode)) break;

		/* Occasionally advance the distance */
		if (++ctr > (4 * dis * dis + 4 * dis + 1))
		{
			ctr = 0;
			dis++;
		}
	}

	/* Sound */
	sound(SOUND_TELEPORT);

	/* Move the player */
	(void)move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
}


void teleport_away_followable(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	int          oldfy = m_ptr->fy;
	int          oldfx = m_ptr->fx;
	bool         old_ml = m_ptr->ml;
	int          old_cdis = m_ptr->cdis;

	teleport_away(m_idx, MAX_SIGHT * 2 + 5, 0L);

	if (old_ml && (old_cdis <= MAX_SIGHT) && !world_monster && !p_ptr->inside_battle && los(py, px, oldfy, oldfx))
	{
		bool follow = FALSE;

		if ((p_ptr->muta1 & MUT1_VTELEPORT) || (p_ptr->pclass == CLASS_IMITATOR) || (p_ptr->pclass == CLASS_KOMACHI && p_ptr->lev > 24)) follow = TRUE;
		else if (p_ptr->pclass == CLASS_RESEARCHER && p_ptr->lev > 24) follow = TRUE;
		else if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags2 & RF2_TELE_AFTER) follow = TRUE;
		else
		{
			u32b flgs[TR_FLAG_SIZE];
			object_type *o_ptr;
			int i;

			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			{
				if(check_invalidate_inventory(i)) continue;
				o_ptr = &inventory[i];
				if (o_ptr->k_idx && !object_is_cursed(o_ptr))
				{
					object_flags(o_ptr, flgs);
					if (have_flag(flgs, TR_TELEPORT))
					{
						follow = TRUE;
						break;
					}
				}
			}
		}

		if(p_ptr->confused || p_ptr->stun || p_ptr->paralyzed) follow = FALSE;

		if (follow)
		{
#ifdef JP
			if (get_check_strict("���Ă����܂����H", CHECK_OKAY_CANCEL))
#else
			if (get_check_strict("Do you follow it? ", CHECK_OKAY_CANCEL))
#endif
			{
				if (one_in_(3))
				{
					teleport_player(200, TELEPORT_PASSIVE);
#ifdef JP
					msg_print("���s�I");
#else
					msg_print("Failed!");
#endif
				}
				else teleport_player_to(m_ptr->fy, m_ptr->fx, 0L);
				p_ptr->energy_need += ENERGY_NEED();
			}
		}
	}
}


/*
 * Teleport the player one level up or down (random when legal)
 * Note: If m_idx <= 0, target is player.
 */
void teleport_level(int m_idx)
{
	bool         go_up;
	char         m_name[160];
	bool         see_m = TRUE;

	if (m_idx <= 0) /* To player */
	{
#ifdef JP
		strcpy(m_name, "���Ȃ�");
#else
		strcpy(m_name, "you");
#endif
	}
	else /* To monster */
	{
		monster_type *m_ptr = &m_list[m_idx];

		/* Get the monster name (or "it") */
		monster_desc(m_name, m_ptr, 0);

		see_m = is_seen(m_ptr);
	}

	/* No effect in some case */
	if (TELE_LEVEL_IS_INEFF(m_idx))
	{
#ifdef JP
		if (see_m) msg_print("���ʂ��Ȃ������B");
#else
		if (see_m) msg_print("There is no effect.");
#endif

		return;
	}

	if ((m_idx <= 0) && p_ptr->anti_tele) /* To player */
	{
#ifdef JP
		msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif
		return;
	}

	/* Choose up or down */
	if (randint0(100) < 50) go_up = TRUE;
	else go_up = FALSE;

	if ((m_idx <= 0) && p_ptr->wizard)
	{
		if (get_check("Force to go up? ")) go_up = TRUE;
		else if (get_check("Force to go down? ")) go_up = FALSE;
	}

	/* Down only */ 
	if ((ironman_downward && (m_idx <= 0)) || (dun_level <= d_info[dungeon_type].mindepth))
	{
#ifdef JP
		if (see_m) msg_format("%^s�͏���˂��j���Ē���ł����B", m_name);
#else
		if (see_m) msg_format("%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s");
#endif
		if (m_idx <= 0) /* To player */
		{
			if (!dun_level)
			{
				dungeon_type = p_ptr->recall_dungeon;
				p_ptr->oldpy = py;
				p_ptr->oldpx = px;
			}

			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, 1, NULL);

			if (autosave_l) do_cmd_save_game(TRUE);

			if (!dun_level)
			{
				dun_level = d_info[dungeon_type].mindepth;
				prepare_change_floor_mode(CFM_RAND_PLACE);
			}
			else if(EXTRA_MODE)
			{
				prepare_change_floor_mode(CFM_DOWN | CFM_RAND_PLACE | CFM_NO_RETURN);
			}
			else
			{
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
			}

			if(INSIDE_EXTRA_QUEST)
			{
				leave_quest_check();
				p_ptr->inside_quest = 0;
			}

			/* Leaving */
			p_ptr->leaving = TRUE;
		}
	}

	/* Up only */
	else if (quest_number(dun_level) || (dun_level >= d_info[dungeon_type].maxdepth) || (difficulty == DIFFICULTY_EASY && dungeon_type == DUNGEON_ANGBAND &&  dun_level >= 49))
	{
#ifdef JP
		if (see_m) msg_format("%^s�͓V���˂��j���Ē��֕����Ă����B", m_name);
#else
		if (see_m) msg_format("%^s rise%s up through the ceiling.", m_name, (m_idx <= 0) ? "" : "s");
#endif


		if (m_idx <= 0) /* To player */
		{
			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, -1, NULL);

			if (autosave_l) do_cmd_save_game(TRUE);

			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			leave_quest_check();

			/* Leaving */
			p_ptr->inside_quest = 0;
			p_ptr->leaving = TRUE;
		}
	}
	else if (go_up)
	{
#ifdef JP
		if (see_m) msg_format("%^s�͓V���˂��j���Ē��֕����Ă����B", m_name);
#else
		if (see_m) msg_format("%^s rise%s up through the ceiling.", m_name, (m_idx <= 0) ? "" : "s");
#endif


		if (m_idx <= 0) /* To player */
		{
			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, -1, NULL);

			if (autosave_l) do_cmd_save_game(TRUE);

			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			/* Leaving */
			p_ptr->leaving = TRUE;
		}
	}
	else
	{
#ifdef JP
		if (see_m) msg_format("%^s�͏���˂��j���Ē���ł����B", m_name);
#else
		if (see_m) msg_format("%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s");
#endif

		if (m_idx <= 0) /* To player */
		{
			/* Never reach this code on the surface */
			/* if (!dun_level) dungeon_type = p_ptr->recall_dungeon; */

			if (record_stair) do_cmd_write_nikki(NIKKI_TELE_LEV, 1, NULL);

			if (autosave_l) do_cmd_save_game(TRUE);

			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			/* Leaving */
			p_ptr->leaving = TRUE;
		}
	}

	/* Monster level teleportation is simple deleting now */
	if (m_idx > 0)
	{
		monster_type *m_ptr = &m_list[m_idx];

		/* Check for quest completion */
		check_quest_completion(m_ptr);

		if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
		{
			char m2_name[80];

			monster_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_TELE_LEVEL, m2_name);
		}

		delete_monster_idx(m_idx);
	}
	else
	{
		set_deity_bias(DBIAS_WARLIKE, -1);
		if(one_in_(2)) set_deity_bias(DBIAS_REPUTATION, -1);
	}

	/* Sound */
	sound(SOUND_TPLEVEL);
}



/*:::�_���W������I�ԁB�A�ҁA�K�w��e���|�[�g�A�t���A���Z�b�g�H*/
int choose_dungeon(cptr note, int y, int x)
{
	int select_dungeon;
	int i, num = 0;
	s16b *dun;

	/* Hack -- No need to choose dungeon in some case */
	if (lite_town || vanilla_town || ironman_downward)
	{
		if (max_dlv[DUNGEON_ANGBAND]) return DUNGEON_ANGBAND;
		else
		{
#ifdef JP
			msg_format("�܂�%s�ɓ��������Ƃ͂Ȃ��B", d_name + d_info[DUNGEON_ANGBAND].name);
#else
			msg_format("You haven't entered %s yet.", d_name + d_info[DUNGEON_ANGBAND].name);
#endif
			msg_print(NULL);
			return 0;
		}
	}

	/* Allocate the "dun" array */
	C_MAKE(dun, max_d_idx, s16b);

	screen_save();
	for(i = 1; i < max_d_idx; i++)
	{
		char buf[80];
		bool seiha = FALSE;

		if (!d_info[i].maxdepth) continue;
		if (!max_dlv[i]) continue;

		if(flag_dungeon_complete[i]) seiha = TRUE;
#if 0
		if (d_info[i].final_guardian)
		{
			if (!r_info[d_info[i].final_guardian].max_num) seiha = TRUE;
		}
		else if (max_dlv[i] == d_info[i].maxdepth) seiha = TRUE;
#endif
#ifdef JP
		sprintf(buf,"      %c) %c%-12s : �ő� %d �K", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#else
		sprintf(buf,"      %c) %c%-16s : Max level %d", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#endif
		prt(buf, y + num, x);
		dun[num++] = i;
	}

	if (!num)
	{
#ifdef JP
		prt("      �I�ׂ�_���W�������Ȃ��B", y, x);
#else
		prt("      No dungeon is available.", y, x);
#endif
	}

#ifdef JP
	prt(format("�ǂ̃_���W����%s���܂���:", note), 0, 0);
#else
	prt(format("Which dungeon do you %s?: ", note), 0, 0);
#endif
	while(1)
	{
		i = inkey();
		if ((i == ESCAPE) || !num)
		{
			/* Free the "dun" array */
			C_KILL(dun, max_d_idx, s16b);

			screen_load();
			return 0;
		}
		if (i >= 'a' && i <('a'+num))
		{
			select_dungeon = dun[i-'a'];
			break;
		}
		else bell();
	}
	screen_load();

	/* Free the "dun" array */
	C_KILL(dun, max_d_idx, s16b);

	return select_dungeon;
}


/*
 * Recall the player to town or dungeon
 */
/*:::�A�� turns:�����^�[�����@�����A�����A��*/
/*:::�����ł̓_���W������I�����ăJ�E���^��ݒ肷��̂݁B���ۂ̏����͕ʂ̏ꏊ�ōs��*/
bool recall_player(int turns)
{
	/*
	 * TODO: Recall the player to the last
	 * visited town when in the wilderness
	 */

	/* Ironman option */
	if (p_ptr->inside_arena || ironman_downward)
	{
#ifdef JP
msg_print("�����N����Ȃ������B");
#else
		msg_print("Nothing happens.");
#endif

		return TRUE;
	}

	if (dun_level && (max_dlv[dungeon_type] > dun_level) && !p_ptr->inside_quest && !p_ptr->word_recall)
	{
#ifdef JP
if (get_check("�����͍Ő[���B�K���󂢊K�ł��B���̊K�ɖ߂��ė��܂����H "))
#else
		if (get_check("Reset recall depth? "))
#endif
		{
			max_dlv[dungeon_type] = dun_level;
			if (record_maxdepth)
#ifdef JP
				do_cmd_write_nikki(NIKKI_TRUMP, dungeon_type, "�A�҂̂Ƃ���");
#else
				do_cmd_write_nikki(NIKKI_TRUMP, dungeon_type, "when recall from dungeon");
#endif
		}

	}
	/*:::�A�ґ҂����łȂ��Ƃ�*/
	if (!p_ptr->word_recall)
	{
		/*:::�n�ォ��Ȃ�A�҂���_���W������I������*/
		if (!dun_level)
		{
			int select_dungeon;
#ifdef JP
			select_dungeon = choose_dungeon("�ɋA��", 2, 14);
#else
			select_dungeon = choose_dungeon("recall", 2, 14);
#endif
			if (!select_dungeon) return FALSE;
			p_ptr->recall_dungeon = select_dungeon;
		}
		p_ptr->word_recall = turns;
#ifdef JP
msg_print("���̑�C������߂Ă���...");
#else
		msg_print("The air about you becomes charged...");
#endif

		p_ptr->redraw |= (PR_STATUS);
	}
	else
	{
		p_ptr->word_recall = 0;
#ifdef JP
msg_print("����߂���C�����ꋎ����...");
#else
		msg_print("A tension leaves the air around you...");
#endif

		p_ptr->redraw |= (PR_STATUS);
	}
	return TRUE;
}


bool word_of_recall(void)
{
	return(recall_player(randint0(21) + 15));
}


bool reset_recall(void)
{
	int select_dungeon, dummy = 0;
	char ppp[80];
	char tmp_val[160];

#ifdef JP
	select_dungeon = choose_dungeon("���Z�b�g", 2, 14);
#else
	select_dungeon = choose_dungeon("reset", 2, 14);
#endif

	/* Ironman option */
	if (ironman_downward)
	{
#ifdef JP
		msg_print("�����N����Ȃ������B");
#else
		msg_print("Nothing happens.");
#endif

		return TRUE;
	}

	if (!select_dungeon) return FALSE;
	/* Prompt */
#ifdef JP
sprintf(ppp, "���K�ɃZ�b�g���܂��� (%d-%d):", d_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#else
	sprintf(ppp, "Reset to which level (%d-%d): ", d_info[select_dungeon].mindepth, max_dlv[select_dungeon]);
#endif


	/* Default */
	sprintf(tmp_val, "%d", MAX(dun_level, 1));

	/* Ask for a level */
	if (get_string(ppp, tmp_val, 10))
	{
		/* Extract request */
		dummy = atoi(tmp_val);

		/* Paranoia */
		if (dummy < 1) dummy = 1;

		/* Paranoia */
		if (dummy > max_dlv[select_dungeon]) dummy = max_dlv[select_dungeon];
		if (dummy < d_info[select_dungeon].mindepth) dummy = d_info[select_dungeon].mindepth;

		max_dlv[select_dungeon] = dummy;

		if (record_maxdepth)
#ifdef JP
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "�t���A�E���Z�b�g��");
#else
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "using a scroll of reset recall");
#endif
					/* Accept request */
#ifdef JP
msg_format("%s�̋A�҃��x���� %d �K�ɃZ�b�g�B", d_name+d_info[select_dungeon].name, dummy, dummy * 50);
#else
		msg_format("Recall depth set to level %d (%d').", dummy, dummy * 50);
#endif

	}
	else
	{
		return FALSE;
	}
	return TRUE;
}


/*
 * Apply disenchantment to the player's stuff
 *
 * XXX XXX XXX This function is also called from the "melee" code
 *
 * Return "TRUE" if the player notices anything
 */
/*:::�򉻍U���Ȃǂɂ�鑕���̗�*/
/*:::slot�ŗ򉻑Ώۂ�I�ׂ�悤�ɂ���*/
bool apply_disenchant(int mode, int slot)
{
	int             t = 0;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	int to_h, to_d, to_a, pval;

	/* Pick a random slot */
	switch (randint1(8))
	{
		case 1: t = INVEN_RARM; break;
		case 2: t = INVEN_LARM; break;
		case 3: t = INVEN_RIBBON; break;
		case 4: t = INVEN_BODY; break;
		case 5: t = INVEN_OUTER; break;
		case 6: t = INVEN_HEAD; break;
		case 7: t = INVEN_HANDS; break;
		case 8: t = INVEN_FEET; break;
	}

	///mod151205 �O�����͂ǂ��̃X���b�g�ɂǂꂪ�����Ă邩�킩��Ȃ��̂œK���ɑI��B
	//�����⑕���i�̏ꍇ�e�����̂ŗ򉻊m�������邪�܂�������
	if(p_ptr->pclass == CLASS_3_FAIRIES) t = INVEN_RARM + randint0(12);

	if(slot >= INVEN_RARM && slot <= INVEN_FEET) t = slot;

	/* Get the item */
	o_ptr = &inventory[t];

	/* No item, nothing happens */
	if (!o_ptr->k_idx) return (FALSE);
	if(check_invalidate_inventory(t)) return FALSE;

	/* Disenchant equipments only -- No disenchant on monster ball */
	if (!object_is_weapon_armour_ammo(o_ptr))
		return FALSE;

	/* Nothing to disenchant */
	if ((o_ptr->to_h <= 0) && (o_ptr->to_d <= 0) && (o_ptr->to_a <= 0) && (o_ptr->pval <= 1))
	{
		/* Nothing to notice */
		return (FALSE);
	}


	/* Describe the object */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));


	/* Artifacts have 71% chance to resist */
	if (object_is_artifact(o_ptr) && (randint0(100) < 71))
	{
		/* Message */
#ifdef JP
msg_format("%s(%c)�͗򉻂𒵂˕Ԃ����I",o_name, index_to_label(t) );
#else
		msg_format("Your %s (%c) resist%s disenchantment!",
			   o_name, index_to_label(t),
			   ((o_ptr->number != 1) ? "" : "s"));
#endif


		/* Notice */
		return (TRUE);
	}


	/* Memorize old value */
	to_h = o_ptr->to_h;
	to_d = o_ptr->to_d;
	to_a = o_ptr->to_a;
	pval = o_ptr->pval;

	/* Disenchant tohit */
	if (o_ptr->to_h > 0) o_ptr->to_h--;
	if ((o_ptr->to_h > 5) && (randint0(100) < 20)) o_ptr->to_h--;

	/* Disenchant todam */
	if (o_ptr->to_d > 0) o_ptr->to_d--;
	if ((o_ptr->to_d > 5) && (randint0(100) < 20)) o_ptr->to_d--;

	/* Disenchant toac */
	if (o_ptr->to_a > 0) o_ptr->to_a--;
	if ((o_ptr->to_a > 5) && (randint0(100) < 20)) o_ptr->to_a--;

	/* Disenchant pval (occasionally) */
	/* Unless called from wild_magic() */
	if ((o_ptr->pval > 1) && one_in_(13) && !(mode & 0x01)) o_ptr->pval--;

	if ((to_h != o_ptr->to_h) || (to_d != o_ptr->to_d) ||
	    (to_a != o_ptr->to_a) || (pval != o_ptr->pval))
	{
		/* Message */
#ifdef JP
		msg_format("%s(%c)�͗򉻂��Ă��܂����I",
			   o_name, index_to_label(t) );
#else
		msg_format("Your %s (%c) %s disenchanted!",
			   o_name, index_to_label(t),
			   ((o_ptr->number != 1) ? "were" : "was"));
#endif

		//chg_virtue(V_HARMONY, 1);
		//chg_virtue(V_ENCHANT, -2);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP | PW_PLAYER);

		calc_android_exp();
	}

	/* Notice */
	return (TRUE);
}

/*:::�X�e�[�^�X�V���b�t��*/
void mutate_player(void)
{
	int max1, cur1, max2, cur2, ii, jj, i;

	/* Pick a pair of stats */
	ii = randint0(6);
	for (jj = ii; jj == ii; jj = randint0(6)) /* loop */;

	max1 = p_ptr->stat_max[ii];
	cur1 = p_ptr->stat_cur[ii];
	max2 = p_ptr->stat_max[jj];
	cur2 = p_ptr->stat_cur[jj];

	p_ptr->stat_max[ii] = max2;
	p_ptr->stat_cur[ii] = cur2;
	p_ptr->stat_max[jj] = max1;
	p_ptr->stat_cur[jj] = cur1;

	for (i=0;i<6;i++)
	{
		if(p_ptr->stat_max[i] > p_ptr->stat_max_max[i]) p_ptr->stat_max[i] = p_ptr->stat_max_max[i];
		if(p_ptr->stat_cur[i] > p_ptr->stat_max_max[i]) p_ptr->stat_cur[i] = p_ptr->stat_max_max[i];
	}

	p_ptr->update |= (PU_BONUS);
}


/*
 * Apply Nexus
 */
///sys nexus ���ʍ������󂯂��Ƃ��̌��ʁ@�ύX�\��
void apply_nexus(monster_type *m_ptr)
{
	switch (randint1(7))
	{
		case 1: case 2: case 3:
		{
			teleport_player(200, TELEPORT_PASSIVE);
			break;
		}

		case 4: case 5:
		{
			teleport_player_to(m_ptr->fy, m_ptr->fx, TELEPORT_PASSIVE);
			break;
		}

		case 6:
		{
			///mod140927 �e���|���x�����ˑR�ψ�
			msg_print("�̂̋�����������I");
			(void)gain_random_mutation(0);
/*
			if (randint0(100) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif

				break;
			}
*/
			/* Teleport Level */
			//teleport_level(0);

			break;
		}

		case 7:
		{
			if (randint0(100) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif

				break;
			}

#ifdef JP
msg_print("�̂��˂���n�߂�...");
#else
			msg_print("Your body starts to scramble...");
#endif

			mutate_player();
			break;
		}
	}
}


/*
 * Charge a lite (torch or latern)
 */
///del131214 item �R�f�̖��@
#if 0
void phlogiston(void)
{
	int max_flog = 0;
	object_type * o_ptr = &inventory[INVEN_LITE];

	/* It's a lamp */
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_LANTERN))
	{
		max_flog = FUEL_LAMP;
	}

	/* It's a torch */
	else if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
	{
		max_flog = FUEL_TORCH;
	}

	/* No torch to refill */
	else
	{
#ifdef JP
msg_print("�R�f�������A�C�e���𑕔����Ă��܂���B");
#else
		msg_print("You are not wielding anything which uses phlogiston.");
#endif

		return;
	}

	if (o_ptr->xtra4 >= max_flog)
	{
#ifdef JP
msg_print("���̃A�C�e���ɂ͂���ȏ�R�f���[�ł��܂���B");
#else
		msg_print("No more phlogiston can be put in this item.");
#endif

		return;
	}

	/* Refuel */
	o_ptr->xtra4 += (max_flog / 2);

	/* Message */
#ifdef JP
msg_print("�Ɩ��p�A�C�e���ɔR�f���[�����B");
#else
	msg_print("You add phlogiston to your light item.");
#endif


	/* Comment */
	if (o_ptr->xtra4 >= max_flog)
	{
		o_ptr->xtra4 = max_flog;
#ifdef JP
msg_print("�Ɩ��p�A�C�e���͖��^���ɂȂ����B");
#else
		msg_print("Your light item is full.");
#endif

	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}
#endif

/*
 * Brand the current weapon
 */
///item realm TVAL/EGO ���푮���t�^
///mod140419 �V�G�S�ɑΉ��@�ő�l17��15
void brand_weapon(int brand_type)
{
	int         item;
	object_type *o_ptr;
	cptr        q, s;


	/* Assume enchant weapon */
	item_tester_hook = object_allow_enchant_melee_weapon;
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
q = "�ǂ̕�����������܂���? ";
s = "�����ł��镐�킪�Ȃ��B";
#else
	q = "Enchant which weapon? ";
	s = "You have nothing to enchant.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return;
		}

	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* you can never modify artifacts / ego-items */
	/* you can never modify cursed items */
	/* TY: You _can_ modify broken items (if you're silly enough) */
	if (o_ptr->k_idx && !object_is_artifact(o_ptr) && !object_is_ego(o_ptr) &&
	    !object_is_cursed(o_ptr) &&
	    !((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)))
	{
		cptr act = NULL;

		/* Let's get the name before it is changed... */
		char o_name[MAX_NLEN];
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		switch (brand_type)
		{
		case 15:
			if (object_has_a_blade(o_ptr))
			{
#ifdef JP
act = "�̐n�͉s���𑝂����I";
#else
				act = "becomes very sharp!";
#endif

				o_ptr->name2 = EGO_WEAPON_SHARP;
			}
			else
			{
#ifdef JP
act = "�͏d�X�����U�������I";
#else
				act = "seems very powerful.";
#endif

				o_ptr->name2 = EGO_WEAPON_QUAKE;
				o_ptr->pval = 1 + m_bonus(2, dun_level);
			}
			break;

		case 14:
#ifdef JP
act = "�͐l�Ԃ̌������߂Ă���I";
#else
			act = "coated with acid!";
#endif

			o_ptr->name2 = EGO_WEAPON_MURDERER;
			o_ptr->pval = 1 + m_bonus(2, dun_level);
			break;
		case 13:
#ifdef JP
act = "�͗��̌������߂Ă���I";
#else
			act = "seems to be looking for evil monsters!";
#endif

			o_ptr->name2 = EGO_WEAPON_DRAGONSLAY;
			o_ptr->pval = 1 + m_bonus(2, dun_level);
			break;
		case 12:
#ifdef JP
act = "�͈����̌������߂Ă���I";
#else
			act = "seems to be looking for demons!";
#endif

			o_ptr->pval = 1 + m_bonus(2, dun_level);
			o_ptr->name2 = EGO_WEAPON_DEMONSLAY;
			break;
		case 11:
#ifdef JP
act = "�͗d���̌������߂Ă���I";
#else
			act = "seems to be looking for undead!";
#endif

			o_ptr->name2 = EGO_WEAPON_YOUKAITAIZI;
			o_ptr->pval = 1 + m_bonus(2, dun_level);
			break;
		case 10:
#ifdef JP
act = "�͕s���̂��̂̌������߂Ă���I";
#else
			act = "seems to be looking for animals!";
#endif

			o_ptr->name2 = EGO_WEAPON_GHOSTBUSTER;
			o_ptr->pval = 1 + m_bonus(2, dun_level);
			break;
		case 9:
#ifdef JP
act = "�͏b�̌������߂Ă���I";
#else
			act = "seems to be looking for dragons!";
#endif

			o_ptr->name2 = EGO_WEAPON_HUNTER;
			o_ptr->pval = 1 + m_bonus(2, dun_level);
			break;
		case 8:
#ifdef JP
act = "�͐_�̌������߂Ă���I";
#else
			act = "seems to be looking for troll!s";
#endif

			o_ptr->name2 = EGO_WEAPON_GODEATER;
			o_ptr->pval = 1 + m_bonus(2, dun_level);
			break;
		case 7:
#ifdef JP
act = "�͉ЁX�����P���ꂽ�B";
#else
			act = "seems to be looking for orcs!";
#endif

			o_ptr->name2 = EGO_WEAPON_CHAOS;
			break;
		case 6:
#ifdef JP
act = "�ɐ����̗͂��h�����悤���E�E";
#else
			act = "seems to be looking for giants!";
#endif

			o_ptr->name2 = EGO_WEAPON_SUI_RYU;
			o_ptr->pval = randint1(2);
			break;
		case 5:
#ifdef JP
act = "�͖�������悤�ɐk�����B";
#else
			act = "seems very unstable now.";
#endif

			o_ptr->name2 = EGO_WEAPON_VAMP;
			break;
		case 4:
#ifdef JP
act = "����ŉt���H�����B";
#else
			act = "thirsts for blood!";
#endif

			o_ptr->name2 = EGO_WEAPON_BRANDPOIS;
			break;
		case 3:
#ifdef JP
act = "�͗�C�ɕ���ꂽ�B";
#else
			act = "is coated with poison.";
#endif

			o_ptr->name2 = EGO_WEAPON_BRANDCOLD;
			break;
		case 2:
#ifdef JP
act = "�͈�u�ԔM�����B";
#else
			act = "is engulfed in raw Logrus!";
#endif

			o_ptr->name2 = EGO_WEAPON_BRANDFIRE;
			break;
		case 1:
#ifdef JP
act = "����ΉԂ��U�����I";
#else
			act = "is covered in a fiery shield!";
#endif

			o_ptr->name2 = EGO_WEAPON_BRANDELEC;
			break;
		default:
#ifdef JP
act = "�͔��������グ�n�߂��E�E";
#else
			act = "glows deep, icy blue!";
#endif

			o_ptr->name2 = EGO_WEAPON_BRANDACID;
			break;
		}

#ifdef JP
msg_format("���Ȃ���%s%s", o_name, act);
#else
		msg_format("Your %s %s", o_name, act);
#endif


		enchant(o_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		o_ptr->discount = 99;
		///del131214 virtue
		//chg_virtue(V_ENCHANT, 2);
	}
	else
	{
		if (flush_failure) flush();

#ifdef JP
msg_print("�����t���Ɏ��s�����B");
#else
		msg_print("The Branding failed.");
#endif
			///del131214 virtue
		//chg_virtue(V_ENCHANT, -2);
	}
	calc_android_exp();
}


/*
 * Vanish all walls in this floor
 */
/*:::�_���W�����̑S�Ă̕ǂƃh�A�ƃ��[�������������X�^�[���N����@���������̂Ƃ�1/666�Ŕ���*/
///sys �S�Ă̕ǂ���������ʁ@�{�X�̓���U���œ���悤
bool vanish_dungeon(void)
{
	int          y, x;
	cave_type    *c_ptr;
	feature_type *f_ptr;
	monster_type *m_ptr;
	char         m_name[80];

	/* Prevent vasishing of quest levels and town */
	if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
	{
		return FALSE;
	}

	/* Scan all normal grids */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			c_ptr = &cave[y][x];

			/* Seeing true feature code (ignore mimic) */
			f_ptr = &f_info[c_ptr->feat];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

			m_ptr = &m_list[c_ptr->m_idx];

			/* Awake monster */
			if (c_ptr->m_idx && MON_CSLEEP(m_ptr))
			{
				/* Reset sleep counter */
				(void)set_monster_csleep(c_ptr->m_idx, 0);

				/* Notice the "waking up" */
				if (m_ptr->ml)
				{
					/* Acquire the monster name */
					monster_desc(m_name, m_ptr, 0);

					/* Dump a message */
#ifdef JP
					msg_format("%^s���ڂ��o�܂����B", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
				}
			}

			/* Process all walls, doors and patterns */
			if (have_flag(f_ptr->flags, FF_HURT_DISI)) cave_alter_feat(y, x, FF_HURT_DISI);
		}
	}

	/* Special boundary walls -- Top and bottom */
	for (x = 0; x < cur_wid; x++)
	{
		c_ptr = &cave[0][x];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}

		c_ptr = &cave[cur_hgt - 1][x];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}
	}

	/* Special boundary walls -- Left and right */
	for (y = 1; y < (cur_hgt - 1); y++)
	{
		c_ptr = &cave[y][0];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}

		c_ptr = &cave[y][cur_wid - 1];
		f_ptr = &f_info[c_ptr->mimic];

		/* Lose room and vault */
		c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

		/* Set boundary mimic if needed */
		if (c_ptr->mimic && have_flag(f_ptr->flags, FF_HURT_DISI))
		{
			c_ptr->mimic = feat_state(c_ptr->mimic, FF_HURT_DISI);

			/* Check for change to boring grid */
			if (!have_flag(f_info[c_ptr->mimic].flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);
		}
	}

	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	return TRUE;
}

/*:::��������*/
void call_the_(void)
{
	int i;
	cave_type *c_ptr;
	bool do_call = TRUE;

	for (i = 0; i < 9; i++)
	{
		c_ptr = &cave[py + ddy_ddd[i]][px + ddx_ddd[i]];

		if (!cave_have_flag_grid(c_ptr, FF_PROJECT))
		{
			if (!c_ptr->mimic || !have_flag(f_info[c_ptr->mimic].flags, FF_PROJECT) ||
			    !permanent_wall(&f_info[c_ptr->feat]))
			{
				do_call = FALSE;
				break;
			}
		}
	}

	if (do_call)
	{
		for (i = 1; i < 10; i++)
		{
			if (i - 5) fire_ball(GF_ROCKET, i, 175, 2);
		}

		for (i = 1; i < 10; i++)
		{
			if (i - 5) fire_ball(GF_MANA, i, 175, 3);
		}

		for (i = 1; i < 10; i++)
		{
			if (i - 5) fire_ball(GF_NUKE, i, 175, 4);
		}
	}

	/* Prevent destruction of quest levels and town */
	else if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
	{
#ifdef JP
		msg_print("�n�ʂ��h�ꂽ�B");
#else
		msg_print("The ground trembles.");
#endif
	}

	else
	{
#ifdef JP
		msg_format("���Ȃ���%s��ǂɋ߂�����ꏊ�ŏ����Ă��܂����I",
//			((mp_ptr->spell_book == TV_LIFE_BOOK) ? "�F��" : "����"));
			("����"));
		msg_print("�傫�Ȕ��������������I");
#else
		msg_format("You %s the %s too close to a wall!",
			((mp_ptr->spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
			((mp_ptr->spell_book == TV_LIFE_BOOK) ? "prayer" : "spell"));
		msg_print("There is a loud explosion!");
#endif

		if (one_in_(666))
		{
#ifdef JP
			if (!vanish_dungeon()) msg_print("�_���W�����͈�u�Â܂�Ԃ����B");
#else
			if (!vanish_dungeon()) msg_print("The dungeon silences a moment.");
#endif
		}
		else
		{
			if (destroy_area(py, px, 15 + p_ptr->lev + randint0(11), FALSE,FALSE,FALSE))
#ifdef JP
				msg_print("�_���W���������󂵂�...");
#else
				msg_print("The dungeon collapses...");
#endif

			else
#ifdef JP
				msg_print("�_���W�����͑傫���h�ꂽ�B");
#else
				msg_print("The dungeon trembles.");
#endif
		}

#ifdef JP
		take_hit(DAMAGE_NOESCAPE, 100 + randint1(150), "���E�I�ȋ�������", -1);
#else
		take_hit(DAMAGE_NOESCAPE, 100 + randint1(150), "a suicidal Call the Void", -1);
#endif
	}
}


/*
 * Fetch an item (teleport it right underneath the caster)
 */
/*:::�����ɃA�C�e���������񂹂�B����A�g���b�v�⃋�[���������ɂ����Ă����̂܂܈����񂹂Ă��܂�*/
void fetch(int dir, int wgt, bool require_los)
{
	int             ty, tx, i;
	cave_type       *c_ptr;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];

	/* Check to see if an object is already there */
	if (cave[py][px].o_idx)
	{
#ifdef JP
msg_print("�����̑��̉��ɂ��镨�͎��܂���B");
#else
		msg_print("You can't fetch when you're already standing on something.");
#endif

		return;
	}

	/* Use a target */
	if (dir == 5 && target_okay())
	{
		tx = target_col;
		ty = target_row;

		if (distance(py, px, ty, tx) > MAX_RANGE)
		{
#ifdef JP
msg_print("����Ȃɉ����ɂ��镨�͎��܂���I");
#else
			msg_print("You can't fetch something that far away!");
#endif

			return;
		}

		c_ptr = &cave[ty][tx];

		/* We need an item to fetch */
		if (!c_ptr->o_idx)
		{
#ifdef JP
msg_print("�����ɂ͉�������܂���B");
#else
			msg_print("There is no object at this place.");
#endif

			return;
		}

		/* No fetching from vault */
		if (c_ptr->info & CAVE_ICKY)
		{
#ifdef JP
msg_print("�A�C�e�����R���g���[�����O��ė������B");
#else
			msg_print("The item slips from your control.");
#endif

			return;
		}

		/* We need to see the item */
		if (require_los)
		{
			if (!player_has_los_bold(ty, tx))
			{
#ifdef JP
				msg_print("�����͂��Ȃ��̎��E�ɓ����Ă��܂���B");
#else
				msg_print("You have no direct line of sight to that location.");
#endif

				return;
			}
			else if (!projectable(py, px, ty, tx))
			{
#ifdef JP
				msg_print("�����͕ǂ̌������ł��B");
#else
				msg_print("You have no direct line of sight to that location.");
#endif

				return;
			}
		}
	}
	else
	{
		/* Use a direction */
		ty = py; /* Where to drop the item */
		tx = px;

		do
		{
			ty += ddy[dir];
			tx += ddx[dir];
			c_ptr = &cave[ty][tx];

			if ((distance(py, px, ty, tx) > MAX_RANGE) ||
				!cave_have_flag_bold(ty, tx, FF_PROJECT)) return;
		}
		while (!c_ptr->o_idx);
	}

	o_ptr = &o_list[c_ptr->o_idx];

	if (o_ptr->weight > wgt)
	{
		/* Too heavy to 'fetch' */
#ifdef JP
msg_print("���̃A�C�e���͏d�߂��܂��B");
#else
		msg_print("The object is too heavy.");
#endif

		return;
	}

	i = c_ptr->o_idx;
	c_ptr->o_idx = o_ptr->next_o_idx;
	cave[py][px].o_idx = i; /* 'move' it */
	o_ptr->next_o_idx = 0;
	o_ptr->iy = (byte)py;
	o_ptr->ix = (byte)px;

	object_desc(o_name, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("%^s�����Ȃ��̑����ɔ��ł����B", o_name);
#else
	msg_format("%^s flies through the air to your feet.", o_name);
#endif


	note_spot(py, px);
	p_ptr->redraw |= PR_MAP;
}

/*:::�����ϗe*/
///sys �����ϗe�@����������������悤�ɂ���Ȃ�1�ɂ��Ă�������
void alter_reality(void)
{
	/* Ironman option */
	if (p_ptr->inside_arena || ironman_downward)
	{
#ifdef JP
		msg_print("�����N����Ȃ������B");
#else
		msg_print("Nothing happens.");
#endif
		return;
	}

	if (!p_ptr->alter_reality)
	{
		int turns = randint0(21) + 15;

		p_ptr->alter_reality = turns;
#ifdef JP
		msg_print("���̌i�F���ς��n�߂�...");
#else
		msg_print("The view around you begins to change...");
#endif

		p_ptr->redraw |= (PR_STATUS);
	}
	else
	{
		p_ptr->alter_reality = 0;
#ifdef JP
		msg_print("�i�F�����ɖ߂���...");
#else
		msg_print("The view around you got back...");
#endif

		p_ptr->redraw |= (PR_STATUS);
	}
	return;
}


/*
 * Leave a "glyph of warding" which prevents monster movement
 */
bool warding_glyph(void)
{
	/* XXX XXX XXX */
	if (!cave_clean_bold(py, px))
	{
#ifdef JP
msg_print("����̃A�C�e���������𒵂˕Ԃ����B");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_print("���̃��[��������ɏo�������c�Ǝv�����炠�Ȃ��̖���z���ĉ�ꂽ�I");
		hina_gain_yaku(-500);

		//v1.1.53 �Y��Ă��̂Œǉ�
		return FALSE;
	}

	/* Create a glyph */
	cave[py][px].info |= CAVE_OBJECT;
	cave[py][px].mimic = feat_glyph;

	/* Notice */
	note_spot(py, px);

	/* Redraw */
	lite_spot(py, px);

	return TRUE;
}

bool place_mirror(void)
{
	/* XXX XXX XXX */
	if (!cave_clean_bold(py, px))
	{
#ifdef JP
msg_print("����̃A�C�e���������𒵂˕Ԃ����B");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	/* Create a mirror */
	cave[py][px].info |= CAVE_OBJECT;
	cave[py][px].mimic = feat_mirror;

	/* Turn on the light */
	cave[py][px].info |= CAVE_GLOW;

	/* Notice */
	note_spot(py, px);

	/* Redraw */
	lite_spot(py, px);

	update_local_illumination(py, px);

	return TRUE;
}


/*
 * Leave an "explosive rune" which prevents monster movement
 */
bool explosive_rune(void)
{
	/* XXX XXX XXX */
	if (!cave_clean_bold(py, px))
	{
#ifdef JP
msg_print("����̃A�C�e���������𒵂˕Ԃ����B");
#else
		msg_print("The object resists the spell.");
#endif

		return FALSE;
	}

	/* Create a glyph */
	cave[py][px].info |= CAVE_OBJECT;
	cave[py][px].mimic = feat_explosive_rune;

	/* Notice */
	note_spot(py, px);
	
	/* Redraw */
	lite_spot(py, px);

	return TRUE;
}


/*
 * Identify everything being carried.
 * Done by a potion of "self knowledge".
 */
/*:::�C���x���g���̒��g��S�ĊӒ肷��B�����M���h��*�[��*�̖�H*/
void identify_pack(void)
{
	int i;

	/* Simply identify and know every item */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Identify it */
		identify_item(o_ptr);

		/* Auto-inscription */
		autopick_alter_item(i, FALSE);
	}
}


/*
 * Used by the "enchant" function (chance of failure)
 * (modified for Zangband, we need better stuff there...) -- TY
 */
static int enchant_table[16] =
{
	0, 10,  50, 100, 200,
	300, 400, 500, 650, 800,
	950, 987, 993, 995, 998,
	1000
};


/*
 * Removes curses from items in inventory
 *
 * Note that Items which are "Perma-Cursed" (The One Ring,
 * The Crown of Morgoth) can NEVER be uncursed.
 *
 * Note that if "all" is FALSE, then Items which are
 * "Heavy-Cursed" (Mormegil, Calris, and Weapons of Morgul)
 * will not be uncursed.
 */
/*:::����@*����*�̂Ƃ�all=TRUE �������������ꂽ��TRUE���Ԃ�*/
///sys ����@�����􂢂�i���̎􂢂��������s����Ƃ����b�Z�[�W�o���Ă�����
static int remove_curse_aux(int all)
{
	int i, cnt = 0;

	/* Attempt to uncurse items being worn */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Uncursed already */
		if (!object_is_cursed(o_ptr)) continue;

		/* Heavily Cursed Items need a special spell */
		if (!all && (o_ptr->curse_flags & TRC_HEAVY_CURSE)) continue;

		/* Perma-Cursed Items can NEVER be uncursed */
		if (o_ptr->curse_flags & TRC_PERMA_CURSE)
		{
			/* Uncurse it */
			o_ptr->curse_flags &= (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE);
			continue;
		}

		/* Uncurse it */
		o_ptr->curse_flags = 0L;

		/* Hack -- Assume felt */
		o_ptr->ident |= (IDENT_SENSE);

		/* Take note */
		o_ptr->feeling = FEEL_NONE;

		/* Recalculate the bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);

		/* Count the uncursings */
		cnt++;
	}

	/* Return "something uncursed" */
	return (cnt);
}


/*
 * Remove most curses
 */
bool remove_curse(void)
{

	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0])
	{
		if(p_ptr->magic_num1[0] > 10000)
			msg_print("���͂₠�Ȃ��̖�͂��̒��x�̉���ł͏��������ɂȂ��B");
		else
		{
			msg_print("������������B");
			hina_gain_yaku(-(randint1(25)));
		}
	}

	return (remove_curse_aux(FALSE));
}

/*
 * Remove all curses
 */
bool remove_all_curse(void)
{

	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0])
	{
		msg_print("����Ȃ�ɖ�������B");
		hina_gain_yaku(-(100 + randint1(100)));
	}
	return (remove_curse_aux(TRUE));
}


/*
 * Turns an object into gold, gain some of its value in a shop
 */
/*:::�B���p�A�~�_�X�̎�*/
///mod140824 ���99999�ɕύX
bool alchemy(void)
{
	int item, amt = 1;
	int old_number;
	long price;
	bool force = FALSE;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	char out_val[MAX_NLEN+40];

	cptr q, s;

	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e�������ɕς��܂����H";
s = "���ɕς����镨������܂���B";
#else
	q = "Turn which item to gold? ";
	s = "You have nothing to turn to gold.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	//v1.1.86 ���������J�[�h���̂����������Ƃ�₱�����Ȃ�̂ŋ֎~
	if (o_ptr->tval == TV_ABILITY_CARD)
	{
		msg_print("���̃J�[�h�͏���̎�i�łȂ��Ɗ����ł��Ȃ��悤���B");
		return FALSE;
	}


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return FALSE;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, 0);
	o_ptr->number = old_number;

	/* Verify unless quantity given */
	if (!force)
	{
		if (confirm_destroy || (object_value(o_ptr) > 0))
		{
			/* Make a verification */
#ifdef JP
sprintf(out_val, "�{����%s�����ɕς��܂����H", o_name);
#else
			sprintf(out_val, "Really turn %s to gold? ", o_name);
#endif

			if (!get_check(out_val)) return FALSE;
		}
	}

	/* Artifacts cannot be destroyed */
	if (!can_player_destroy_object(o_ptr))
	{
		/* Message */
#ifdef JP
		msg_format("%s�����ɕς��邱�ƂɎ��s�����B", o_name);
#else
		msg_format("You fail to turn %s to gold!", o_name);
#endif

		/* Done */
		return FALSE;
	}

	price = object_value_real(o_ptr);

	if (price <= 0)
	{
		/* Message */
#ifdef JP
msg_format("%s���j�Z�̋��ɕς����B", o_name);
#else
		msg_format("You turn %s to fool's gold.", o_name);
#endif

	}
	else
	{
		price /= 3;



		//v1.1.89 discount�l�����f����Ă��Ȃ������̂Œǉ�
		if (o_ptr->discount) price -= (price * o_ptr->discount / 100L);
		if (price > 99999) price = 99999;
		if (amt > 1) price *= amt;

		msg_format("%s����%d �̋��ɕς����B", o_name, price);

		p_ptr->au += price;

		/* Redraw gold */
		p_ptr->redraw |= (PR_GOLD);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

	}

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	return TRUE;
}


/*
 * Break the curse of an item
 */
/*:::�A�C�e���̎􂢉����@����h��X�⊪���Ȃǂŋ������ꂽ�Ƃ���p����*/
static void break_curse(object_type *o_ptr)
{
	if (object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_PERMA_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) && (randint0(100) < 25))
	{
#ifdef JP
msg_print("�������Ă����􂢂��ł��j��ꂽ�I");
#else
		msg_print("The curse is broken!");
#endif

		o_ptr->curse_flags = 0L;

		o_ptr->ident |= (IDENT_SENSE);

		o_ptr->feeling = FEEL_NONE;
	}
}


/*
 * Enchants a plus onto an item. -RAK-
 *
 * Revamped!  Now takes item pointer, number of times to try enchanting,
 * and a flag of what to try enchanting.  Artifacts resist enchantment
 * some of the time, and successful enchantment to at least +0 might
 * break a curse on the item. -CFT-
 *
 * Note that an item can technically be enchanted all the way to +15 if
 * you wait a very, very, long time.  Going from +9 to +10 only works
 * about 5% of the time, and from +10 to +11 only about 1% of the time.
 *
 * Note that this function can now be used on "piles" of items, and
 * the larger the pile, the lower the chance of success.
 */

/*:::�����h��̏C���l���㏸������*/
/*:::�A�[�e�B�t�@�N�g�͋����ɒ�R���邱�Ƃ�����B+0�ȏ�ւ̋����͎􂢂������B*/
/*:::�����̃A�C�e���ɂ��g���邪�A�C�e����������قǐ�����������*/
/*:::n:�������s��*/
/*:::eflag:�����C���l�A�K���������邩�ǂ����Ȃǂ̃t���O*/
///mod131224 TVAL
bool enchant(object_type *o_ptr, int n, int eflag)
{
	int     i, chance, prob;
	bool    res = FALSE;
	bool    a = object_is_artifact(o_ptr);
	bool    force = (eflag & ENCH_FORCE);


	/* Large piles resist enchantment */
	/*:::���s����l��ݒ�@���ɔ��@��͒Ⴂ*/
	prob = o_ptr->number * 100;

	/* Missiles are easy to enchant */
	if ((o_ptr->tval == TV_BOLT) ||
	    (o_ptr->tval == TV_ARROW) ||
	    (o_ptr->tval == TV_BULLET))
	{
		prob = prob / 20;
	}

	/* Try "n" times */
	for (i = 0; i < n; i++)
	{
		/* Hack -- Roll for pile resistance */
		if (!force && randint0(prob) >= 100) continue;

		/* Enchant to hit */
		/*:::FORCE�Ȃ�K�������A���̏C���l��0�����Ȃ�K�������A����ȊO��enchant_table[]�ɏ�����A�A�[�e�B�t�@�N�g�͐�����1/2*/
		if (eflag & ENCH_TOHIT)
		{
			if (o_ptr->to_h < 0) chance = 0;
			else if (o_ptr->to_h > 15) chance = 1000;
			else if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 77 ) chance = 0; //�b��_�͕K������ 
			else chance = enchant_table[o_ptr->to_h];

			if (force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				o_ptr->to_h++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				/*:::�C���l��0�ȏ�ɂȂ������􂢂������邱�Ƃ�����*/
				if (o_ptr->to_h >= 0)
					break_curse(o_ptr);
			}
		}

		/* Enchant to damage */
		if (eflag & ENCH_TODAM)
		{
			if (o_ptr->to_d < 0) chance = 0;
			else if (o_ptr->to_d > 15) chance = 1000;
			else if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 77 ) chance = 0; //�b��_�͕K������ 
			else chance = enchant_table[o_ptr->to_d];

			if (force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				o_ptr->to_d++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if (o_ptr->to_d >= 0)
					break_curse(o_ptr);
			}
		}

		/* Enchant to armor class */
		if (eflag & ENCH_TOAC)
		{
			if (o_ptr->to_a < 0) chance = 0;
			else if (o_ptr->to_a > 15) chance = 1000;
			else if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 77 ) chance = 0; //�b��_�͕K������ 
			else chance = enchant_table[o_ptr->to_a];

			if (force || ((randint1(1000) > chance) && (!a || (randint0(100) < 50))))
			{
				o_ptr->to_a++;
				res = TRUE;

				/* only when you get it above -1 -CFT */
				if (o_ptr->to_a >= 0)
					break_curse(o_ptr);
			}
		}
	}

	/* Failure */
	if (!res) return (FALSE);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp();

	/* Success */
	return (TRUE);
}



/*
 * Enchant an item (in the inventory or on the floor)
 * Note that "num_ac" requires armour, else weapon
 * Returns TRUE if attempted, FALSE if cancelled
 */
bool enchant_spell(int num_hit, int num_dam, int num_ac)
{
	int         item;
	bool        okay = FALSE;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	cptr        q, s;


	/* Assume enchant weapon */
	item_tester_hook = object_allow_enchant_weapon;
	item_tester_no_ryoute = TRUE;

	/* Enchant armor if requested */
	if (num_ac) item_tester_hook = object_is_armour;

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e�����������܂���? ";
s = "�����ł���A�C�e�����Ȃ��B";
#else
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Describe */
#ifdef JP
msg_format("%s �͖��邭�P�����I",
    o_name);
#else
	msg_format("%s %s glow%s brightly!",
		   ((item >= 0) ? "Your" : "The"), o_name,
		   ((o_ptr->number > 1) ? "" : "s"));
#endif


	/* Enchant */
	if (enchant(o_ptr, num_hit, ENCH_TOHIT)) okay = TRUE;
	if (enchant(o_ptr, num_dam, ENCH_TODAM)) okay = TRUE;
	if (enchant(o_ptr, num_ac, ENCH_TOAC)) okay = TRUE;

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
msg_print("�����Ɏ��s�����B");
#else
		msg_print("The enchantment failed.");
#endif
		///del131215 virtue
		//if (one_in_(3)) chg_virtue(V_ENCHANT, -1);
	}
	//else
		//chg_virtue(V_ENCHANT, 1);

	calc_android_exp();

	/* Something happened */
	return (TRUE);
}


/*
 * Check if an object is nameless weapon or armour
 */
/*:::�������Ώێw��֐�*/
///mod140607 �w�ւ�A�~�����b�g�⃊�{���ɂ���������悤�ɂ���
static bool item_tester_hook_nameless_weapon_armour(object_type *o_ptr)
{
	/* Require weapon or armour */
	//if (!object_is_weapon_armour_ammo(o_ptr)) return FALSE;
	if (!object_is_weapon_armour_ammo(o_ptr)
		&& o_ptr->tval != TV_RIBBON
		&& o_ptr->tval != TV_RING
		&& o_ptr->tval != TV_AMULET) return FALSE;
	
	/* Require nameless object if the object is well known */
	if (object_is_known(o_ptr) && !object_is_nameless(o_ptr))
		return FALSE;

	return TRUE;
}

///mod140607 �w�ւ�A�~�����b�g�⃊�{���ɂ���������悤�ɂ���
bool artifact_scroll(void)
{
	int             item;
	bool            okay = FALSE;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	cptr            q, s;


	item_tester_no_ryoute = TRUE;

	/* Enchant weapon/armour */
	item_tester_hook = item_tester_hook_nameless_weapon_armour;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�����������܂���? ";
	s = "�����ł���A�C�e�����Ȃ��B";
#else
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Describe */
#ifdef JP
	msg_format("%s ��ῂ����𔭂����I",o_name);
#else
	msg_format("%s %s radiate%s a blinding light!",
		  ((item >= 0) ? "Your" : "The"), o_name,
		  ((o_ptr->number > 1) ? "" : "s"));
#endif

	if (object_is_artifact(o_ptr))
	{
#ifdef JP
		msg_format("%s�͊��ɓ`���̃A�C�e���ł��I", o_name  );
#else
		msg_format("The %s %s already %s!",
		    o_name, ((o_ptr->number > 1) ? "are" : "is"),
		    ((o_ptr->number > 1) ? "artifacts" : "an artifact"));
#endif

		okay = FALSE;
	}

	else if (object_is_ego(o_ptr))
	{
#ifdef JP
		msg_format("%s�͊��ɖ��̂���A�C�e���ł��I", o_name );
#else
		msg_format("The %s %s already %s!",
		    o_name, ((o_ptr->number > 1) ? "are" : "is"),
		    ((o_ptr->number > 1) ? "ego items" : "an ego item"));
#endif

		okay = FALSE;
	}

	else if (o_ptr->xtra3)
	{
#ifdef JP
		msg_format("%s�͊��ɋ�������Ă��܂��I", o_name );
#else
		msg_format("The %s %s already %s!",
		    o_name, ((o_ptr->number > 1) ? "are" : "is"),
		    ((o_ptr->number > 1) ? "customized items" : "a customized item"));
#endif
	}

	else
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
			msg_print("�����̃A�C�e���ɖ��@�������邾���̃G�l���M�[�͂���܂���I");
			msg_format("%d ��%s����ꂽ�I",(o_ptr->number)-1, o_name);
#else
			msg_print("Not enough enough energy to enchant more than one object!");
			msg_format("%d of your %s %s destroyed!",(o_ptr->number)-1, o_name, (o_ptr->number>2?"were":"was"));
#endif

			if (item >= 0)
			{
				inven_item_increase(item, 1-(o_ptr->number));
			}
			else
			{
				floor_item_increase(0-item, 1-(o_ptr->number));
			}
		}
		okay = create_artifact(o_ptr, TRUE);
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_print("�����Ɏ��s�����B");
#else
		msg_print("The enchantment failed.");
#endif
		///del131216 virtue
		//if (one_in_(3)) chg_virtue(V_ENCHANT, -1);
	}
	///del131216 virtue
	//else
	//	chg_virtue(V_ENCHANT, 1);

	calc_android_exp();

	/* Something happened */
	return (TRUE);
}


/*
 * Identify an object
 */
/*:::�A�C�e�����Ӓ肷�� *�Ӓ�*�ł͂Ȃ��@*/
bool identify_item(object_type *o_ptr)
{
	bool old_known = FALSE;
	char o_name[MAX_NLEN];

	/* Description */
	object_desc(o_name, o_ptr, 0);

	if (o_ptr->ident & IDENT_KNOWN)
		old_known = TRUE;
	/*
	if (!(o_ptr->ident & (IDENT_MENTAL)))
	{
		if (object_is_artifact(o_ptr) || one_in_(5))
			///sysdel virtue
			chg_virtue(V_KNOWLEDGE, 1);
	}
	*/
	/* Identify it fully */
	/*:::���ʂƊӒ�̃t���O����*/
	object_aware(o_ptr);
	object_known(o_ptr);

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	strcpy(record_o_name, o_name);
	record_turn = turn;

	/* Description */
	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if(record_fix_art && !old_known && object_is_fixed_artifact(o_ptr))
		do_cmd_write_nikki(NIKKI_ART, 0, o_name);
	if(record_rand_art && !old_known && o_ptr->art_name)
		do_cmd_write_nikki(NIKKI_ART, 0, o_name);

	return old_known;
}


static bool item_tester_hook_identify(object_type *o_ptr)
{
	return (bool)!object_is_known(o_ptr);
}

//�T�O�����Ӓ�̊�����ǂ񂾎���p
static bool item_tester_hook_identify_sagume(object_type *o_ptr)
{
	if(k_info[o_ptr->k_idx].easy_know) return FALSE;
	if(o_ptr->ident & IDENT_SENSE) return TRUE;
	return (bool)object_is_known(o_ptr);
}


static bool item_tester_hook_identify_weapon_armour(object_type *o_ptr)
{
	if (object_is_known(o_ptr))
		return FALSE;
	return object_is_weapon_armour_ammo(o_ptr);
}

//�L�m�R�Ӓ�p
static bool item_tester_hook_identify_kinoko(object_type *o_ptr)
{
	if (object_is_known(o_ptr))
		return FALSE;
	if(o_ptr->tval != TV_MUSHROOM)
		return FALSE;

	return TRUE;
}
//�L�m�R�ƃ|�[�V�����Ӓ�p
static bool item_tester_hook_identify_kinoko_potion(object_type *o_ptr)
{
	if (object_is_known(o_ptr))
		return FALSE;
	if(o_ptr->tval == TV_MUSHROOM || o_ptr->tval == TV_POTION)
		return TRUE;

	return FALSE;
}

//�����Ӓ�p
static bool item_tester_hook_identify_scroll(object_type *o_ptr)
{
	if (object_is_known(o_ptr))
		return FALSE;
	if(o_ptr->tval != TV_SCROLL)
		return FALSE;

	return TRUE;
}

/*
 * Identify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was identified, else FALSE.
 */
/*:::����������Ӓ肷��@�Ӓ�̖��@��A�C�e���Ƒ����̉�*/
bool ident_spell(bool only_equip)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	cptr            q, s;
	bool old_known;

	item_tester_no_ryoute = TRUE;

	if (only_equip)
		item_tester_hook = item_tester_hook_identify_weapon_armour;
	else
		item_tester_hook = item_tester_hook_identify;

	if (can_get_item())
	{
#ifdef JP
		q = "�ǂ̃A�C�e�����Ӓ肵�܂���? ";
#else
		q = "Identify which item? ";
#endif
	}
	else
	{
		if (only_equip)
			item_tester_hook = object_is_weapon_armour_ammo;
		else
			item_tester_hook = NULL;

#ifdef JP
		q = "���ׂĊӒ�ς݂ł��B ";
#else
		q = "All items are identified. ";
#endif
	}

	/* Get an item */
#ifdef JP
	s = "�Ӓ肷��ׂ��A�C�e�����Ȃ��B";
#else
	s = "You have nothing to identify.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}

	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Identify it */
	old_known = identify_item(o_ptr);

	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
	if (item >= INVEN_RARM)
	{
#ifdef JP
		msg_format("%^s: %s(%c)�B", describe_use(item), o_name, index_to_label(item));
#else
		msg_format("%^s: %s (%c).", describe_use(item), o_name, index_to_label(item));
#endif
	}
	else if (item >= 0)
	{
#ifdef JP
		msg_format("�U�b�N��: %s(%c)�B", o_name, index_to_label(item));
#else
		msg_format("In your pack: %s (%c).", o_name, index_to_label(item));
#endif
	}
	else
	{
#ifdef JP
		msg_format("����: %s�B", o_name);
#else
		msg_format("On the ground: %s.", o_name);
#endif
	///mod140428 @�̑����̃A�C�e�����Ӓ肳�ꂽ�Ƃ������A�C�e���ꗗ���X�V
	fix_floor_item_list(py,px);
	}

	/* Auto-inscription/destroy */
	autopick_alter_item(item, (bool)(destroy_identify && !old_known));

	/* Something happened */
	return (TRUE);
}


/*
 * Mundanify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was mundanified, else FALSE.
 */
/*:::�}�f��*/
///sys �}�f���@���聗�̎��S�����v
bool mundane_spell(bool only_equip)
{
	int             item;
	object_type     *o_ptr;
	cptr            q, s;

	if (only_equip) item_tester_hook = object_is_weapon_armour_ammo;
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
q = "�ǂ��}�f�����܂����H";
s = "�g������̂�����܂���B";
#else
	q = "Use which item? ";
	s = "You have nothing you can use.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	if(p_ptr->pclass == CLASS_EIKI && o_ptr->name1 == ART_EIKI)
	{
		msg_print("�~�߂Ă������B�x���i�͑�؂ɂ���ׂ����B");
		return FALSE;
	}


	//���P��������}�f��������Q�[���I�[�o�[
	///mod151101 ���̓��P�ƃx�[�X�A�C�e�����L������s��ł��̂Ŗ�D��
	if(p_ptr->pclass == CLASS_KOGASA && o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && item == INVEN_RARM)
	{
		if(!process_warning2()) return FALSE;
		msg_print("�܂΂䂢�M�����������I");
		take_hit(DAMAGE_LOSELIFE,9999,"�}�f�̊���",-1);
		return TRUE;
	}
	else if(p_ptr->pclass == CLASS_IKU && o_ptr->name1 == ART_HEAVENLY_MAIDEN)
	{
		if(!process_warning2()) return FALSE;
		msg_print("�܂΂䂢�M�����������I");
		take_hit(DAMAGE_LOSELIFE,9999,"�x���i��ʖڂɂ��ċސT����",-1);
		return TRUE;
	}
	else
	{
		msg_print("�܂΂䂢�M�����������I");
	}


	{
		byte iy = o_ptr->iy;                 /* Y-position on map, or zero */
		byte ix = o_ptr->ix;                 /* X-position on map, or zero */
		s16b next_o_idx = o_ptr->next_o_idx; /* Next object in stack (if any) */
		byte marked = o_ptr->marked;         /* Object is marked */
		s16b weight = o_ptr->number * o_ptr->weight;
		u16b inscription = o_ptr->inscription;

		/* Wipe it clean */
		object_prep(o_ptr, o_ptr->k_idx);

		o_ptr->iy = iy;
		o_ptr->ix = ix;
		o_ptr->next_o_idx = next_o_idx;
		o_ptr->marked = marked;
		o_ptr->inscription = inscription;
		if (item >= 0) p_ptr->total_weight += (o_ptr->weight - weight);
	}
	calc_android_exp();

	///mod140218 �}�f���̂Ƃ��E�B���h�E�ĕ`�悪����Ȃ��悤�Ȃ̂Œǉ����Ƃ���
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
	///mod140218 �����i�֌W�̏������ǉ�
	if(item > INVEN_PACK) p_ptr->update |= PU_BONUS;

	window_stuff();


	/* Something happened */
	return TRUE;
}


/*:::�܂�*�Ӓ�*����Ă��Ȃ��A�C�e����Ԃ�hook*/
static bool item_tester_hook_identify_fully(object_type *o_ptr)
{
	return (bool)(!object_is_known(o_ptr) || !(o_ptr->ident & IDENT_MENTAL));
}
/*:::�܂�*�Ӓ�*����Ă��Ȃ������Ԃ�hook*/
static bool item_tester_hook_identify_fully_weapon_armour(object_type *o_ptr)
{
	if (!item_tester_hook_identify_fully(o_ptr))
		return FALSE;
	return object_is_weapon_armour_ammo(o_ptr);
}

/*
 * Fully "identify" an object in the inventory  -BEN-
 * This routine returns TRUE if an item was identified.
 */
/*:: *�Ӓ�* �X���������������ڗ����@�ڍז��� */
bool identify_fully(bool only_equip)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	cptr            q, s;
	bool old_known;

	item_tester_no_ryoute = TRUE;
	if (only_equip)
		item_tester_hook = item_tester_hook_identify_fully_weapon_armour;
	else
		item_tester_hook = item_tester_hook_identify_fully;

	if (can_get_item())
	{
#ifdef JP
		q = "�ǂ̃A�C�e����*�Ӓ�*���܂���? ";
#else
		q = "*Identify* which item? ";
#endif
	}
	else
	{
		if (only_equip)
			item_tester_hook = object_is_weapon_armour_ammo;
		else
			item_tester_hook = NULL;

#ifdef JP
		q = "���ׂ�*�Ӓ�*�ς݂ł��B ";
#else
		q = "All items are *identified*. ";
#endif
	}

	/* Get an item */
#ifdef JP
	s = "*�Ӓ�*����ׂ��A�C�e�����Ȃ��B";
#else
	s = "You have nothing to *identify*.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Identify it */
	/*:::�Ӓ�*/
	old_known = identify_item(o_ptr);

	/*:::*�Ӓ�*�t���O�𗧂Ă�*/
	/* Mark the item as fully known */
	o_ptr->ident |= (IDENT_MENTAL);

	/* Handle stuff */
	handle_stuff();

	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
	if (item >= INVEN_RARM)
	{
#ifdef JP
		msg_format("%^s: %s(%c)�B", describe_use(item), o_name, index_to_label(item));
#else
		msg_format("%^s: %s (%c).", describe_use(item), o_name, index_to_label(item));
#endif


	}
	else if (item >= 0)
	{
#ifdef JP
		msg_format("�U�b�N��: %s(%c)�B", o_name, index_to_label(item));
#else
		msg_format("In your pack: %s (%c).", o_name, index_to_label(item));
#endif
	}
	else
	{
#ifdef JP
		msg_format("����: %s�B", o_name);
#else
		msg_format("On the ground: %s.", o_name);
#endif
	}

	/* Describe it fully */
	(void)screen_object(o_ptr, 0L);

	/* Auto-inscription/destroy */
	autopick_alter_item(item, (bool)(destroy_identify && !old_known));

	/* Success */
	return (TRUE);
}


//v1.1.86 ��ƃ��b�h�Ɩ��@�_
//���Ƃ���item_tester_hook_recharge()�ł��̔�����s���Ă������A
//�V�����̃A�r���e�B�J�[�h�͖��͏[�U�̑ΏۂɂȂ邪���͐H���▂����p�t���Z�̑ΏۂɂȂ�Ȃ����߁A
//���̓�𔻒肷��֐��𕪂����B
bool item_tester_hook_staff_rod_wand(object_type *o_ptr)
{
	/* Recharge staffs */
	if (o_ptr->tval == TV_STAFF) return (TRUE);

	/* Recharge wands */
	if (o_ptr->tval == TV_WAND) return (TRUE);

	/* Hack -- Recharge rods */
	if (o_ptr->tval == TV_ROD) return (TRUE);

	/* Nope */
	return (FALSE);


}


//v1.1.86 ���e���������Bitem_tester_hook_staff_rod_wand()�Q��
bool item_tester_hook_recharge(object_type *o_ptr)
{
	if (item_tester_hook_staff_rod_wand(o_ptr)) return TRUE;

	if (o_ptr->tval == TV_ABILITY_CARD && ability_card_list[o_ptr->pval].activate) return TRUE;

	/* Nope */
	return (FALSE);
}


/*
 * Recharge a wand/staff/rod from the pack or on the floor.
 * This function has been rewritten in Oangband and ZAngband.
 *
 * Sorcery/Arcane -- Recharge  --> recharge(plev * 4)
 * Chaos -- Arcane Binding     --> recharge(90)
 *
 * Scroll of recharging        --> recharge(130)
 * Artifact activation/Thingol --> recharge(130)
 *
 * It is harder to recharge high level, and highly charged wands,
 * staffs, and rods.  The more wands in a stack, the more easily and
 * strongly they recharge.  Staffs, however, each get fewer charges if
 * stacked.
 *
 * XXX XXX XXX Beware of "sliding index errors".
 */
/*:::���͏[�U*/
//v1.1.86 �A�r���e�B�J�[�h�̏����ǉ��@���b�h�Ɠ��������
//...�Ǝv��������͂�A�r���e�B�J�[�h�[�U�֎~�B
//�F�X�����������[�U�L��ɂ���ƃ��C�W�E�����ʍU���J�[�h2��+���͏[�U(200)������J�|���Ă��܂������B
bool recharge(int power)
{
	int item, lev;
	int recharge_strength, recharge_amount;

	object_type *o_ptr;
	object_kind *k_ptr;

	bool fail = FALSE;
	byte fail_type = 1;

	cptr q, s;
	char o_name[MAX_NLEN];

	/* Only accept legal items */
	//item_tester_hook = item_tester_hook_recharge;//�A�r���e�B�J�[�h�̏[�U�͂�߂�
	item_tester_hook = item_tester_hook_staff_rod_wand;

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e���ɖ��͂��[�U���܂���? ";
s = "���͂��[�U���ׂ��A�C�e�����Ȃ��B";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Get the object kind. */
	k_ptr = &k_info[o_ptr->k_idx];


	if (o_ptr->tval == TV_ABILITY_CARD)
	{
		lev = ability_card_list[o_ptr->pval].difficulty;
	}
	else
	{
		/* Extract the object "level" */
		lev = k_info[o_ptr->k_idx].level;
	}

	/* Recharge a rod */
	if (o_ptr->tval == TV_ROD || o_ptr->tval == TV_ABILITY_CARD)
	{
		if (o_ptr->tval == TV_ABILITY_CARD)
		{
			//v1.1.86 �A�r���e�B�J�[�h�͏[�U�̓�Փx���������Ă���
			recharge_strength = ((power > lev) ? (power - lev) : 0) / 10;
		}
		else
		{
			/* Extract a recharge strength by comparing object level to power. */
			recharge_strength = ((power > lev / 2) ? (power - lev / 2) : 0) / 5;
		}


		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}

		/* Recharge */
		else
		{
			/* Recharge amount */
			recharge_amount = (power * damroll(3, 2));

			/* Recharge by that amount */
			if (o_ptr->timeout > recharge_amount)
				o_ptr->timeout -= recharge_amount;
			else
				o_ptr->timeout = 0;
		}
	}


	/* Recharge wand/staff */
	else
	{
		/* Extract a recharge strength by comparing object level to power.
		 * Divide up a stack of wands' charges to calculate charge penalty.
		 */
		if ((o_ptr->tval == TV_WAND) && (o_ptr->number > 1))
			recharge_strength = (100 + power - lev -
			(8 * o_ptr->pval / o_ptr->number)) / 15;

		/* All staffs, unstacked wands. */
		else recharge_strength = (100 + power - lev -
			(8 * o_ptr->pval)) / 15;

		/* Paranoia */
		if (recharge_strength < 0) recharge_strength = 0;

		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}

		/* If the spell didn't backfire, recharge the wand or staff. */
		else
		{
			/* Recharge based on the standard number of charges. */
			recharge_amount = randint1(1 + k_ptr->pval / 2);

			/* Multiple wands in a stack increase recharging somewhat. */
			if ((o_ptr->tval == TV_WAND) && (o_ptr->number > 1))
			{
				recharge_amount +=
					(randint1(recharge_amount * (o_ptr->number - 1))) / 2;
				if (recharge_amount < 1) recharge_amount = 1;
				if (recharge_amount > 12) recharge_amount = 12;
			}

			/* But each staff in a stack gets fewer additional charges,
			 * although always at least one.
			 */
			if ((o_ptr->tval == TV_STAFF) && (o_ptr->number > 1))
			{
				recharge_amount /= o_ptr->number;
				if (recharge_amount < 1) recharge_amount = 1;
			}

			/* Recharge the wand or staff. */
			o_ptr->pval += recharge_amount;


			/* Hack -- we no longer "know" the item */
			o_ptr->ident &= ~(IDENT_KNOWN);

			/* Hack -- we no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
		}
	}


	/* Inflict the penalties for failing a recharge. */
	if (fail)
	{
		/* Artifacts are never destroyed. */
		if (object_is_fixed_artifact(o_ptr))
		{
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("���͂��t�������I%s�͊��S�ɖ��͂��������B", o_name);
#else
			msg_format("The recharging backfires - %s is completely drained!", o_name);
#endif


			/* Artifact rods. */
			if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout < 10000))
				o_ptr->timeout = (o_ptr->timeout + 100) * 2;

			/* Artifact wands and staffs. */
			else if ((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_STAFF))
				o_ptr->pval = 0;
		}
		else
		{
			/* Get the object description */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/*** Determine Seriousness of Failure ***/

			/* Mages recharge objects more safely. */
			///class ���C�W�n�͖��͏[�U���s�̃y�i���e�B���Ⴂ
			if (cp_ptr->magicmaster)
			{
				/* 10% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD || o_ptr->tval == TV_ABILITY_CARD)
				{
					if (one_in_(10)) fail_type = 2;
					else fail_type = 1;
				}
				/* 75% chance to blow up one wand, otherwise draining. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (!one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 50% chance to blow up one staff, otherwise no effect. */
				else if (o_ptr->tval == TV_STAFF)
				{
					if (one_in_(2)) fail_type = 2;
					else fail_type = 0;
				}
			}

			/* All other classes get no special favors. */
			else
			{
				/* 33% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD || o_ptr->tval == TV_ABILITY_CARD)
				{
					if (one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 20% chance of the entire stack, else destroy one wand. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (one_in_(5)) fail_type = 3;
					else fail_type = 2;
				}
				/* Blow up one staff. */
				else if (o_ptr->tval == TV_STAFF)
				{
					fail_type = 2;
				}
			}

			/*** Apply draining and destruction. ***/

			/* Drain object or stack of objects. */
			if (fail_type == 1)
			{
				if (o_ptr->tval == TV_ROD || o_ptr->tval == TV_ABILITY_CARD)
				{
					int tmp;

					if (o_ptr->tval == TV_ROD)
					{
						msg_format("���͂��t���˂��āA���b�h���炳��ɖ��͂��z������Ă��܂����I");
						tmp = (o_ptr->timeout + 100) * 2;
					}
					else
					{
						msg_format("���͂��t���˂����I�J�[�h�̒��q�������Ȃ����C������...");
						tmp = o_ptr->timeout + 1000;

					}

					if (tmp > 32000) tmp = 32000;//�^�C���A�E�g�l�̃I�[�o�[�t���[�h�~

					o_ptr->timeout = tmp;
				}
				else if (o_ptr->tval == TV_WAND)
				{
#ifdef JP
msg_format("%s�͔j����Ƃꂽ���A���͂��S�Ď���ꂽ�B", o_name);
#else
					msg_format("You save your %s from destruction, but all charges are lost.", o_name);
#endif

					o_ptr->pval = 0;
				}
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if (fail_type == 2)
			{
				if (o_ptr->number > 1)
					msg_format("���\�Ȗ��@�̂��߂�%s��%s��ꂽ�I", o_name,(o_ptr->tval == TV_ABILITY_CARD)?"�ꖇ":"��{");
				else
					msg_format("���\�Ȗ��@�̂��߂�%s����ꂽ�I", o_name);


				/* Reduce rod stack maximum timeout, drain wands. */
				if (o_ptr->tval == TV_ROD) o_ptr->timeout = (o_ptr->number - 1) * k_ptr->pval;
				if (o_ptr->tval == TV_WAND) o_ptr->pval = 0;

				/* Reduce and describe inventory */
				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}

			/* Destroy all members of a stack of objects. */
			if (fail_type == 3)
			{
				if (o_ptr->number > 1)
#ifdef JP
msg_format("���\�Ȗ��@�̂��߂�%s���S�ĉ�ꂽ�I", o_name);
#else
					msg_format("Wild magic consumes all your %s!", o_name);
#endif

				else
#ifdef JP
msg_format("���\�Ȗ��@�̂��߂�%s����ꂽ�I", o_name);
#else
					msg_format("Wild magic consumes your %s!", o_name);
#endif



				/* Reduce and describe inventory */
				if (item >= 0)
				{
					inven_item_increase(item, -999);
					inven_item_describe(item);
					inven_item_optimize(item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -999);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}
		}
	}

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Something was done */
	return (TRUE);
}


/*
 * Bless a weapon
 */
/*:::�v���[�X�g�̕���j��*/
bool bless_weapon(void)
{
	int             item;
	object_type     *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	char            o_name[MAX_NLEN];
	cptr            q, s;

	item_tester_no_ryoute = TRUE;

	/* Bless only weapons */
	item_tester_hook = object_is_weapon;

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e�����j�����܂����H";
s = "�j���ł��镐�킪����܂���B";
#else
	q = "Bless which weapon? ";
	s = "You have weapon to bless.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR)))
		return FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}

	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	if (object_is_cursed(o_ptr))
	{
		if (((o_ptr->curse_flags & TRC_HEAVY_CURSE) && (randint1(100) < 33)) ||
			have_flag(flgs, TR_ADD_L_CURSE) ||
			have_flag(flgs, TR_ADD_H_CURSE) ||
		    (o_ptr->curse_flags & TRC_PERMA_CURSE))
		{
#ifdef JP
msg_format("%s�𕢂������I�[���͏j���𒵂˕Ԃ����I",
    o_name);
#else
			msg_format("The black aura on %s %s disrupts the blessing!",
			    ((item >= 0) ? "your" : "the"), o_name);
#endif

			return TRUE;
		}

#ifdef JP
msg_format("%s ����׈��ȃI�[�����������B",
    o_name);
#else
		msg_format("A malignant aura leaves %s %s.",
		    ((item >= 0) ? "your" : "the"), o_name);
#endif


		/* Uncurse it */
		o_ptr->curse_flags = 0L;

		/* Hack -- Assume felt */
		o_ptr->ident |= (IDENT_SENSE);

		/* Take note */
		o_ptr->feeling = FEEL_NONE;

		/* Recalculate the bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/*
	 * Next, we try to bless it. Artifacts have a 1/3 chance of
	 * being blessed, otherwise, the operation simply disenchants
	 * them, godly power negating the magic. Ok, the explanation
	 * is silly, but otherwise priests would always bless every
	 * artifact weapon they find. Ego weapons and normal weapons
	 * can be blessed automatically.
	 */
	if (have_flag(flgs, TR_BLESSED))
	{
#ifdef JP
msg_format("%s �͊��ɏj������Ă���B",
    o_name    );
#else
		msg_format("%s %s %s blessed already.",
		    ((item >= 0) ? "Your" : "The"), o_name,
		    ((o_ptr->number > 1) ? "were" : "was"));
#endif

		return TRUE;
	}

	if (!(object_is_artifact(o_ptr) || object_is_ego(o_ptr)) || one_in_(3))
	{
		/* Describe */
#ifdef JP
msg_format("%s�͋P�����I",
     o_name);
#else
		msg_format("%s %s shine%s!",
		    ((item >= 0) ? "Your" : "The"), o_name,
		    ((o_ptr->number > 1) ? "" : "s"));
#endif

		add_flag(o_ptr->art_flags, TR_BLESSED);
		o_ptr->discount = 99;
	}
	else
	{
		bool dis_happened = FALSE;

#ifdef JP
msg_print("���̕���͏j���������Ă���I");
#else
		msg_print("The weapon resists your blessing!");
#endif


		/* Disenchant tohit */
		if (o_ptr->to_h > 0)
		{
			o_ptr->to_h--;
			dis_happened = TRUE;
		}

		if ((o_ptr->to_h > 5) && (randint0(100) < 33)) o_ptr->to_h--;

		/* Disenchant todam */
		if (o_ptr->to_d > 0)
		{
			o_ptr->to_d--;
			dis_happened = TRUE;
		}

		if ((o_ptr->to_d > 5) && (randint0(100) < 33)) o_ptr->to_d--;

		/* Disenchant toac */
		if (o_ptr->to_a > 0)
		{
			o_ptr->to_a--;
			dis_happened = TRUE;
		}

		if ((o_ptr->to_a > 5) && (randint0(100) < 33)) o_ptr->to_a--;

		if (dis_happened)
		{
#ifdef JP
msg_print("���͂��}�f�ȕ��͋C�Ŗ�����...");
#else
			msg_print("There is a static feeling in the air...");
#endif

#ifdef JP
msg_format("%s �͗򉻂����I",
     o_name    );
#else
			msg_format("%s %s %s disenchanted!",
			    ((item >= 0) ? "Your" : "The"), o_name,
			    ((o_ptr->number > 1) ? "were" : "was"));
#endif

		}
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Window stuff */
	p_ptr->window |= (PW_EQUIP | PW_PLAYER);

	calc_android_exp();

	return TRUE;
}


/*
 * pulish shield
 */
/*:::������*/
#if 0
bool pulish_shield(void)
{
	int             item;
	object_type     *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	char            o_name[MAX_NLEN];
	cptr            q, s;

	item_tester_no_ryoute = TRUE;
	/* Assume enchant weapon */
	item_tester_tval = TV_SHIELD;

	/* Get an item */
#ifdef JP
q = "�ǂ̏��𖁂��܂����H";
s = "������������܂���B";
#else
	q = "Pulish which weapon? ";
	s = "You have weapon to pulish.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR)))
		return FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	if (o_ptr->k_idx && !object_is_artifact(o_ptr) && !object_is_ego(o_ptr) &&
	    !object_is_cursed(o_ptr) && (o_ptr->sval != SV_MIRROR_SHIELD))
	{
#ifdef JP
msg_format("%s�͋P�����I", o_name);
#else
		msg_format("%s %s shine%s!",
		    ((item >= 0) ? "Your" : "The"), o_name,
		    ((o_ptr->number > 1) ? "" : "s"));
#endif
		o_ptr->name2 = EGO_REFLECTION;
		enchant(o_ptr, randint0(3) + 4, ENCH_TOAC);

		o_ptr->discount = 99;
		///del131214 virtue
		//chg_virtue(V_ENCHANT, 2);

		return TRUE;
	}
	else
	{
		if (flush_failure) flush();

#ifdef JP
msg_print("���s�����B");
#else
		msg_print("Failed.");
#endif

		chg_virtue(V_ENCHANT, -2);
	}
	calc_android_exp();

	return FALSE;
}
#endif

//������(TV_COMPOUND)�����ꂽ���̃{�[�������Ȃ�
//(y,x)�̒n�_�ɗF�D�I�ȃ����X�^�[������Ƃ��A���̃����X�^�[��{�点��Ȃ�TRUE��Ԃ�
bool potion_smash_effect2(int who, int y, int x, int sval)
{
	int rad;
	bool angry = FALSE;
	int i;
	int flg = (PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL);

	switch (sval)
	{
	case SV_COMPOUND_MOLOTOV_COCKTAIL://�Ή��r
		(void)project(who, 2, y, x, damroll(8,8), GF_FIRE,flg, -1);
		angry = TRUE;
		break;
	case SV_COMPOUND_STRONG_ACID://���_
		(void)project(who, 3, y, x, damroll(16,16), GF_ACID,flg, -1);
		angry = TRUE;
		break;
	case SV_COMPOUND_BREATH_OF_FIRE://�ΐ�����
		(void)project(who, 4, y, x, 200, GF_FIRE,flg, -1);
		angry = TRUE;
		break;
	case SV_COMPOUND_GELSEMIUM_TEA://�Q���Z�~�E���E�G���K���X
		(void)project(who, 3, y, x, 800, GF_POIS,flg, -1);
		angry = TRUE;
		break;
	case SV_COMPOUND_NIGHTMARE: //�Ӓ����ۃi�C�g���A
		(void)project(who, 2, y, x, 400, GF_NIGHTMARE,flg, -1);
		break;
	case SV_COMPOUND_STAR_FEEDING: //*�엿* ���a1�̃����X�^�[���ő�܂Ń����N�A�b�v������
		(void)project(who, 1, y, x, 0, GF_MAKE_TREE,flg, -1);

		if(cave[y][x].m_idx) rankup_monster(cave[y][x].m_idx,0);
		for (i = 0; i < 8; i++)
		{
			int ty = y + ddy_ddd[i];
			int tx = x + ddx_ddd[i];
			if(cave[ty][tx].m_idx) rankup_monster(cave[ty][tx].m_idx,0);
		}
		break;

	case SV_COMPOUND_SAIBAIMAN: //�T�C�o�C�}���̕r�@�T�C�o�C�}������������
		{
			int num = randint1(3);
			for(i=0;i<num;i++) summon_named_creature(0, y, x, MON_SAIBAIMAN, PM_FORCE_PET);

		}
		break;
	case SV_COMPOUND_YOKOSHIMA: //������œ����t
		(void)project(who, 2, y, x, 1600, GF_TIME,flg, -1);
		angry = TRUE;
		break;

	case SV_COMPOUND_STAR_DETORNATION: //*����*�̖�
		msg_print("��r���唚�������I");
		project_hack3(0,y,x,GF_NUKE,1,800,800);
		break;

	case SV_COMPOUND_VLADIMIR: //�E���f�B�[�~��(�n���R���l��2�����p�_�~�[�A�C�e��)
		{
			int typ_temp = GF_NUKE;
			project_hack3(0,y,x,typ_temp,0,0,1600);
			break;
		}
	case SV_COMPOUND_ICHIKORORI: //�C�`�R����
		(void)project(who, 3, y, x, damroll(50,50), GF_DISP_ALL,flg, -1);
		angry = TRUE;
		break;
	case SV_COMPOUND_ANTIMATTER: //������
		(void)project(who, 7, y, x, damroll(10,500), GF_DISINTEGRATE,flg, -1);
		angry = TRUE;
		break;
	case SV_COMPOUND_SUPER_SPEED: //�V����
		(void)project(who, 7, y, x, 0, GF_OLD_SPEED,flg, -1);
		break;

	case SV_COMPOUND_MICRO_OCSIGEN: //�~�N���I�L�V�Q��
		(void)project(who, 6, y, x, 1600, GF_ACID,flg, -1);
		angry = TRUE;
		break;
	case SV_COMPOUND_MAGICAL_ECO_BOMB: //�Y�p
		(void)project(who, 5, y, x, 777, GF_CHAOS,flg, -1);
		break;
	case SV_COMPOUND_HAPPY:
		if(cave[y][x].m_idx)
		{
			bool flag = TRUE;
			char m_name[80];	
			monster_type *m_ptr = &m_list[cave[y][x].m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			if((r_ptr->flagsr & RFR_RES_ALL)) break;
			if(!(r_ptr->flags3 & RF3_NO_CONF) && randint0(150) > r_ptr->level)
			{
				flag = TRUE;
				(void)set_monster_confused(cave[y][x].m_idx, MON_CONFUSED(m_ptr) + 10 + randint1(10));
			}
			if(!(r_ptr->flags3 & RF3_NO_STUN) && randint0(150) > r_ptr->level)
			{
				flag = TRUE;
				(void)set_monster_stunned(cave[y][x].m_idx, MON_STUNNED(m_ptr) + 10 + randint1(10));
			}
			monster_desc(m_name, m_ptr, 0);
			if(flag) msg_format("%s�͍K����̓��^�ɂ��K���ɂȂ����悤���B",m_name);
			else msg_format("%s�ɂ͌��ʂ��Ȃ������I",m_name);
		}
		break;

	//v1.1.95 �W�L�����m�̖�@���͂ȋ���m��
	case SV_COMPOUND_JEKYLL:
		(void)project(who, 3, y, x, 150+randint1(150), GF_BERSERK, flg, -1);
		angry = TRUE;
		break;

	default:
		;//�������Ȃ�
	}

	return angry;
}

/*
 * Potions "smash open" and cause an area effect when
 * (1) they are shattered while in the player's inventory,
 * due to cold (etc) attacks;
 * (2) they are thrown at a monster, or obstacle;
 * (3) they are shattered by a "cold ball" or other such spell
 * while lying on the floor.
 *
 * Arguments:
 *    who   ---  who caused the potion to shatter (0=player)
 *          potions that smash on the floor are assumed to
 *          be caused by no-one (who = 1), as are those that
 *          shatter inside the player inventory.
 *          (Not anymore -- I changed this; TY)
 *    y, x  --- coordinates of the potion (or player if
 *          the potion was in her inventory);
 *    o_ptr --- pointer to the potion object.
 */
/*:::�򂪔j�󂳂ꂽ�Ƃ��̏��� k_idx��k_info[]�̓Y����*/
/*:::��̎�ނɂ���đ����ƃ_���[�W�����肵�Aprojcet()�Ō��ʔ���*/
///mod151228 �����򏈗��ǉ�
bool potion_smash_effect(int who, int y, int x, int k_idx)
{
	int     radius = 2; /*:::���ꂽ�Ƃ��̌��ʔ��a*/
	int     dt = 0; /*:::���ꂽ�Ƃ��̑���*/
	int     dam = 0; /*:::���ꂽ�Ƃ��̃_���[�W*/
	bool    angry = FALSE; /*:::�����X�^�[��{�点�邩*/

	object_kind *k_ptr = &k_info[k_idx];

	if(k_ptr->tval == TV_COMPOUND)
	{
		return potion_smash_effect2(who,y,x,k_ptr->sval);
	}

	switch (k_ptr->sval)
	{
		//case SV_POTION_SALT_WATER:
		//case SV_POTION_SLIME_MOLD:
		case SV_POTION_LOSE_MEMORIES:
		//case SV_POTION_WATER:   /* perhaps a 'water' attack? */
		//case SV_POTION_APPLE_JUICE:
			return TRUE;

		case SV_POTION_INFRAVISION:
		case SV_POTION_DETECT_INVIS:
		case SV_POTION_SLOW_POISON:
		case SV_POTION_CURE_POISON:
		case SV_POTION_BOLDNESS:
		case SV_POTION_RESIST_HEAT:
		case SV_POTION_RESIST_COLD:
		case SV_POTION_HEROISM:
		case SV_POTION_RES_STR:
		case SV_POTION_RES_INT:
		case SV_POTION_RES_WIS:
		case SV_POTION_RES_DEX:
		case SV_POTION_RES_CON:
		case SV_POTION_RES_CHR:
		case SV_POTION_INC_STR:
		case SV_POTION_INC_INT:
		case SV_POTION_INC_WIS:
		case SV_POTION_INC_DEX:
		case SV_POTION_INC_CON:
		case SV_POTION_INC_CHR:
		case SV_POTION_AUGMENTATION:
		case SV_POTION_ENLIGHTENMENT:
		case SV_POTION_STAR_ENLIGHTENMENT:
		case SV_POTION_SELF_KNOWLEDGE:
		case SV_POTION_EXPERIENCE:
		case SV_POTION_RESISTANCE:
		case SV_POTION_INVULNERABILITY:
		case SV_POTION_NEW_LIFE:
			/* All of the above potions have no effect when shattered */
			return FALSE;


			//v1.1.94
			//�\�͌����n�̖���U���ቺ�����Ȃǂɂ���
		case SV_POTION_DEC_STR:
		case SV_POTION_DEC_DEX:
			dt = GF_DEC_ATK;
			dam = 100;
			break;

		case SV_POTION_DEC_INT:
		case SV_POTION_DEC_WIS:
			dt = GF_DEC_MAG;
			dam = 100;
			break;
		case SV_POTION_DEC_CON:
		case SV_POTION_DEC_CHR:
			dt = GF_DEC_DEF;
			dam = 100;
			break;
		case SV_POTION_RUINATION:
			dt = GF_DEC_ALL;
			dam = 500;
			break;

		case SV_POTION_BESERK_STRENGTH:
			dt = GF_BERSERK;
			dam = 50+randint1(50);
			break;



		case SV_POTION_SLOWNESS:
			dt = GF_OLD_SLOW;
			dam = 25+randint1(25);
			angry = TRUE;
			break;
		case SV_POTION_POISON:
			dt = GF_POIS;
			dam = 3;
			angry = TRUE;
			break;
			///mod140907 �ғł̖�ǉ�
		case SV_POTION_POISON2:
			dt = GF_POIS;
			dam = damroll(20,20);
			angry = TRUE;
			break;
		case SV_POTION_BLINDNESS:
			dt = GF_DARK;
			angry = TRUE;
			break;
//		case SV_POTION_CONFUSION: /* Booze */
//			dt = GF_OLD_CONF;
//			angry = TRUE;
//			break;
		case SV_POTION_SLEEP:
			dt = GF_OLD_SLEEP;
			angry = TRUE;
			break;
		case SV_POTION_DETONATIONS:
			dt = GF_SHARDS;
			dam = damroll(25, 25);
			angry = TRUE;
			break;
		case SV_POTION_DEATH:
			dt = GF_DEATH_RAY;    /* !! */
			dam = k_ptr->level * 10;
			angry = TRUE;
			radius = 1;
			break;
		case SV_POTION_SPEED:
			dt = GF_OLD_SPEED;
			break;
		case SV_POTION_CURE_LIGHT:
			dt = GF_OLD_HEAL;
			dam = damroll(2, 3);
			break;
		case SV_POTION_CURE_SERIOUS:
			dt = GF_OLD_HEAL;
			dam = damroll(4, 3);
			break;
		case SV_POTION_CURE_CRITICAL:
		case SV_POTION_CURING:
			dt = GF_OLD_HEAL;
			dam = damroll(6, 3);
			break;
		case SV_POTION_HEALING:
			dt = GF_OLD_HEAL;
			dam = damroll(10, 10);
			break;
		case SV_POTION_RESTORE_EXP:
			dt = GF_STAR_HEAL;
			dam = 0;
			radius = 1;
			break;
		/*:::�����̖�*/
		case SV_POTION_LIFE:
			dt = GF_STAR_HEAL;
			dam = damroll(50, 50);
			radius = 1;
			break;
		/*:::*�̗͉�*�̖�*/
		case SV_POTION_STAR_HEALING:
			dt = GF_OLD_HEAL;
			dam = damroll(50, 50);
			radius = 1;
			break;
		case SV_POTION_RESTORE_MANA:   /* MANA */
			dt = GF_MANA;
			dam = damroll(10, 10);
			radius = 1;
			break;
		case SV_POTION_NANIKASUZUSHIKUNARU:
			dt = GF_COLD;
			dam = 1600;
			radius = 5;
			break;
		default:
			//typ=0�̃{�[������������B�����I�Ȍ��ʂ͂Ȃ����o�I���ʂ̂��߂Ǝv����B
			/* Do nothing */  ;
	}

	(void)project(who, radius, y, x, dam, dt,
	    (PROJECT_JUMP | PROJECT_ITEM | PROJECT_KILL), -1);

	//-Hack- �u�Ȃɂ��������Ȃ��v�𓊂�����ǉ��Ő���n�`����
	if (k_ptr->sval == SV_POTION_NANIKASUZUSHIKUNARU)
	{
		project(who, 5, y, x, 5, GF_MAKE_BLIZZARD, (PROJECT_JUMP | PROJECT_GRID),-1);
	}

	/* XXX  those potions that explode need to become "known" */
	return angry;
}


/*
 * Hack -- Display all known spells in a window
 *
 * XXX XXX XXX Need to analyze size of the window.
 *
 * XXX XXX XXX Need more color coding.
 */
/*:::�X�y�����X�g��\��*/
///class realm�@�X�y�����X�g�@���\�͎҂Ȃǂ��܂�
void display_spell_list(void)
{
	int             i, j;
	int             y, x;
	int             m[9];
	magic_type forge, *s_ptr = &forge;

	char            name[80];
	char            out_val[160];


	/* Erase window */
	clear_from(0);

	/* They have too many spells to list */
/*:::�X�y�}�X�ƐԖ��͖���*/
	if (p_ptr->pclass == CLASS_SORCERER) return;
	if (p_ptr->pclass == CLASS_RED_MAGE) return;

	/* Snipers */
	if (p_ptr->pclass == CLASS_SNIPER)
	{
		display_snipe_list();
		return;
	}

	/* mind.c type classes */
/*:::���\�͎ҁE�B�C�p�t�E���g���E�o�[�T�[�J�[�E�E�҂̏���*/
	if ((p_ptr->pclass == CLASS_MINDCRAFTER) ||
	    (p_ptr->pclass == CLASS_SUMIREKO) ||
	   // (p_ptr->pclass == CLASS_BERSERKER) ||
	   // (p_ptr->pclass == CLASS_NINJA) ||
	    (p_ptr->pclass == CLASS_MIRROR_MASTER) ||
	    (p_ptr->pclass == CLASS_FORCETRAINER))
	{
		int             i;
		int             y = 1;
		int             x = 1;
		int             minfail = 0;
		int             plev = p_ptr->lev;
		int             chance = 0;
		mind_type       spell;
		char            comment[80];
		char            psi_desc[80];
		int             use_mind;
		bool use_hp = FALSE;

		/* Display a list of spells */
		prt("", y, x);
#ifdef JP
put_str("���O", y, x + 5);
put_str("Lv   MP ���� ����", y, x + 35);
#else
		put_str("Name", y, x + 5);
		put_str("Lv Mana Fail Info", y, x + 35);
#endif

		switch(p_ptr->pclass)
		{
		case CLASS_MINDCRAFTER:
		case CLASS_SUMIREKO:
			use_mind = MIND_MINDCRAFTER;break;
		case CLASS_FORCETRAINER:          use_mind = MIND_KI;break;
		//case CLASS_BERSERKER: use_mind = MIND_BERSERKER; use_hp = TRUE; break;
		case CLASS_MIRROR_MASTER: use_mind = MIND_MIRROR_MASTER; break;
		case CLASS_NINJA: use_mind = MIND_NINJUTSU; use_hp = TRUE; break;
		default:                use_mind = 0;break;
		}

		/* Dump the spells */
		for (i = 0; i < MAX_MIND_POWERS; i++)
		{
			byte a = TERM_WHITE;

			/* Access the available spell */
			spell = mind_powers[use_mind].info[i];
			if (spell.min_lev > plev) break;

			/* Get the failure rate */
			chance = spell.fail;

			/* Reduce failure rate by "effective" level adjustment */
			chance -= 3 * (p_ptr->lev - spell.min_lev);
			
			///sys ���\�͎҂Ȃǂ̓��Z�̎��s���v�Z

			/* Reduce failure rate by INT/WIS adjustment */
			/*:::���s�����p�����[�^�̍����Ō�����*/
			chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);

			if (!use_hp)
			{
				/* Not enough mana to cast */
				if (spell.mana_cost > p_ptr->csp)
				{
					chance += 5 * (spell.mana_cost - p_ptr->csp);
					a = TERM_ORANGE;
				}
			}
			else
			{
				/* Not enough hp to cast */
				if (spell.mana_cost > p_ptr->chp)
				{
					chance += 100;
					a = TERM_RED;
				}
			}

			/* Extract the minimum failure rate */
			minfail = adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]];

			/* Minimum failure rate */
			if (chance < minfail) chance = minfail;

			/* Stunning makes spells harder */
			if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
			else if (p_ptr->stun) chance += 15;

			/* Always a 5 percent chance of working */
			if (chance > 95) chance = 95;

			/* Get info */
			/*:::���Z�́u���ʁv���̃R�����g�𓾂�@����m�͌��ʃR�����g���Ȃ�*/
			mindcraft_info(comment, use_mind, i);

			/* Dump the spell */
			sprintf(psi_desc, "  %c) %-30s%2d %4d %3d%%%s",
			    I2A(i), spell.name,
			    spell.min_lev, spell.mana_cost, chance, comment);

			Term_putstr(x, y + i + 1, -1, a, psi_desc);
		}
		return;
	}


/*:::��������͖��@�����@���Ƃ��͂ǂ��Ȃ�̂��H*/
	/* Cannot read spellbooks */
	if (REALM_NONE == p_ptr->realm1) return;

	/* Normal spellcaster with books */

	/* Scan books */
	/*:::���̈�A����Α��̈�*/
	for (j = 0; j < ((p_ptr->realm2 > REALM_NONE) ? 2 : 1); j++)
	{
		int n = 0;

		/* Reset vertical */
		m[j] = 0;

		/* Vertical location */
		y = (j < 3) ? 0 : (m[j - 3] + 2);

		/* Horizontal location */
		x = 27 * (j % 3);

		/* Scan spells */
		for (i = 0; i < 32; i++)
		{
			byte a = TERM_WHITE;

			/* Access the spell */
			/*:::realm1(2)�ɓ����Ă���͖̂��@�̈� !is_magic()�͎�p�n�C����V�����p�Ȃ̂Ŏ���2�̈�ڂ͂Ȃ�*/
			//if (!is_magic((j < 1) ? p_ptr->realm1 : p_ptr->realm2))
			//{
			//	s_ptr = &technic_info[((j < 1) ? p_ptr->realm1 : p_ptr->realm2) - MIN_TECHNIC][i % 32];
			//}
			if(p_ptr->realm1 == TV_BOOK_HISSATSU) 
				s_ptr = &hissatsu_info[i];
			else
			{
				//s_ptr = &mp_ptr->info[((j < 1) ? p_ptr->realm1 : p_ptr->realm2) - 1][i % 32];
				calc_spell_info(s_ptr,((j < 1) ? p_ptr->realm1 : p_ptr->realm2),i % 32);
			}
			/*:::�w��̈�A�w��ԍ��̃X�y���̖��O�𓾂�*/
			strcpy(name, do_spell((j < 1) ? p_ptr->realm1 : p_ptr->realm2, i % 32, SPELL_NAME));

			/* Illegible */
			if (s_ptr->slevel >= 99)
			{
				/* Illegible */
#ifdef JP
strcpy(name, "(���Ǖs�\)");
#else
				strcpy(name, "(illegible)");
#endif


				/* Unusable */
				a = TERM_L_DARK;
			}

			/* Forgotten */
			else if ((j < 1) ?
				((p_ptr->spell_forgotten1 & (1L << i))) :
				((p_ptr->spell_forgotten2 & (1L << (i % 32)))))
			{
				/* Forgotten */
				a = TERM_ORANGE;
			}

			/* Unknown */
			else if (!((j < 1) ?
				(p_ptr->spell_learned1 & (1L << i)) :
				(p_ptr->spell_learned2 & (1L << (i % 32)))))
			{
				/* Unknown */
				a = TERM_RED;
			}

			/* Untried */
			else if (!((j < 1) ?
				(p_ptr->spell_worked1 & (1L << i)) :
				(p_ptr->spell_worked2 & (1L << (i % 32)))))
			{
				/* Untried */
				a = TERM_YELLOW;
			}

			/* Dump the spell --(-- */
			sprintf(out_val, "%c/%c) %-20.20s",
				I2A(n / 8), I2A(n % 8), name);

			/* Track maximum */
			m[j] = y + n;

			/* Dump onto the window */
			Term_putstr(x, m[j], -1, a, out_val);

			/* Next */
			n++;
		}
	}
}


/*
 * Returns experience of a spell
 */
/*:::�w�肵�����@�̌��݂̌o���l��Ԃ�*/
///mod140207 ���@�ύX
s16b experience_of_spell(int spell, int use_realm)
{
	if(flag_spell_consume_book) 
		return 0;

	//v1.1.32 �p�`�����[��p���i�u���s�v�̗�O����
	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
	{
		return SPELL_EXP_EXPERT;
	}
	else if(REALM_SPELLMASTER)
	{			
		if( cp_ptr->realm_aptitude[use_realm] == 1)  return SPELL_EXP_SKILLED;
		else  return SPELL_EXP_EXPERT;
	}
	else if (use_realm == p_ptr->realm1) return p_ptr->spell_exp[spell];
	else if (use_realm == p_ptr->realm2) return p_ptr->spell_exp[spell + 32];
	else return 0;
}


/*
 * Modify mana consumption rate using spell exp and p_ptr->dec_mana
 */
/*:::�w�肵�����@�̎g�pMP���v�Z����*/
///mod140207 ���@�ύX
int mod_need_mana(int need_mana, int spell, int realm)
{
#define MANA_CONST   2400
#define MANA_DIV        4
#define DEC_MANA_DIV    3

	if(flag_spell_consume_book) return 0;

	/* Realm magic */
	//if ((realm > REALM_NONE) && (realm <= MAX_REALM))
	if (realm <= TV_BOOK_CHAOS || realm == TV_BOOK_OCCULT)
	{
		/*
		 * need_mana defaults if spell exp equals SPELL_EXP_EXPERT and !p_ptr->dec_mana.
		 * MANA_CONST is used to calculate need_mana effected from spell proficiency.
		 */
///sys item ����̈�̂ݎ������s����������悤�ȑ�����Z�\�͂����Ɏ�������̂��ǂ���
		need_mana = need_mana * (MANA_CONST + SPELL_EXP_EXPERT - experience_of_spell(spell, realm)) + (MANA_CONST - 1);
		need_mana *= p_ptr->dec_mana ? DEC_MANA_DIV : MANA_DIV;
		need_mana /= MANA_CONST * MANA_DIV;
		if (need_mana < 1) need_mana = 1;
	}

	/* Non-realm magic */
	else
	{
		if (p_ptr->dec_mana) need_mana = (need_mana + 1) * DEC_MANA_DIV / MANA_DIV;
	}

#undef DEC_MANA_DIV
#undef MANA_DIV
#undef MANA_CONST

	return need_mana;
}


/*
 * Modify spell fail rate
 * Using p_ptr->to_m_chance, p_ptr->dec_mana, p_ptr->easy_spell and p_ptr->heavy_spell
 */
int mod_spell_chance_1(int chance)
{
	chance += p_ptr->to_m_chance;

	if (p_ptr->heavy_spell) chance += 20;

	if (p_ptr->dec_mana && p_ptr->easy_spell) chance -= 4;
	else if (p_ptr->easy_spell) chance -= 3;
	else if (p_ptr->dec_mana) chance -= 2;

	return chance;
}


/*
 * Modify spell fail rate (as "suffix" process)
 * Using p_ptr->dec_mana, p_ptr->easy_spell and p_ptr->heavy_spell
 * Note: variable "chance" cannot be negative.
 */
/*:::�������s���v�Z�̕␳�@���@�̓�Փx�ɉe�����鑕��*/
int mod_spell_chance_2(int chance)
{
	if (p_ptr->dec_mana) chance--;

	if (p_ptr->heavy_spell) chance += 5;
	if(p_ptr->pclass == CLASS_FLAN && chance < 20 ) chance = 20;//�t�������@���s��

	//���i�u���C�v�y�i���e�B
	if(p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		int minfail;
		if (cp_ptr->magicmaster) minfail = 20;
		else minfail = 30;

		if(chance < minfail) chance = minfail;
	}

	return MAX(chance, 0);
}


/*
 * Returns spell chance of failure for spell -RAK-
 */
/*:::�������s���v�Z �ڍז��ǂ����N���X�Ƃ��A���C�����g�Ƃ�����̂Œ����\��*/
///class realm
s16b spell_chance(int spell, int use_realm)
{
	int             chance, minfail;
	magic_type forge, *s_ptr = &forge;

	int             need_mana;
	//int penalty = (mp_ptr->spell_stat == A_WIS) ? 10 : 4;
	int penalty = 4;

	if(flag_spell_consume_book) return 0;


	//v1.1.32 �p�`�����[��p���i�u���s�v�̗�O����
	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && use_realm <= MAX_MAGIC);
	/* Paranoia -- must be literate */
	//if (!mp_ptr->spell_book) return (100);
	else if (cp_ptr->realm_aptitude[0] == 0) return (100);

	if (use_realm == TV_BOOK_HISSATSU) return 0;

	/* Access the spell */

	/*
	if (!is_magic(use_realm))
	{
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	}
	*/
	if(p_ptr->realm1 == TV_BOOK_HISSATSU) 
		s_ptr = &hissatsu_info[spell];
	else
	{
		//s_ptr = &mp_ptr->info[use_realm - 1][spell];
		calc_spell_info(s_ptr,use_realm,spell);
	}

	/* Extract the base spell failure rate */
	chance = s_ptr->sfail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (p_ptr->lev - s_ptr->slevel);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);

	///mod140813 �b���ɂ�������x�㏸
	if(p_ptr->asthma >= ASTHMA_1) chance += p_ptr->asthma / 200;

	if (p_ptr->riding && !CLASS_RIDING_BACKDANCE)
		///mod131226 skill �Z�\�ƕ���Z�\�̓���
		//chance += (MAX(r_info[m_list[p_ptr->riding].r_idx].level - p_ptr->skill_exp[GINOU_RIDING] / 100 - 10, 0));
		chance += (MAX(r_info[m_list[p_ptr->riding].r_idx].level - ref_skill_exp(SKILL_RIDING) / 100 - 10, 0));


	/* Extract mana consumption rate */
	need_mana = mod_need_mana(s_ptr->smana, spell, use_realm);

	/* Not enough mana to cast */
	if (need_mana > p_ptr->csp)
	{
		chance += 5 * (need_mana - p_ptr->csp);
	}

	/*:::���̈�̓�x�㏸��calc_spell_info()�Ɉڂ�*/
	//if ((use_realm != p_ptr->realm1) && ((p_ptr->pclass == CLASS_MAGE) || (p_ptr->pclass == CLASS_PRIEST))) chance += 5;
	//if(use_realm == p_ptr->realm2) chance += 5;

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]];

	/*
	 * Non mage/priest characters never get too good
	 * (added high mage, mindcrafter)
	 */
	//if (mp_ptr->spell_xtra & MAGIC_FAIL_5PERCENT)
	if (!cp_ptr->magicmaster)
	{
		if (minfail < 5) minfail = 5;
	}


	/* Hack -- Priest prayer penalty for "edged" weapons  -DGK */
	if (((p_ptr->pclass == CLASS_PRIEST) || (p_ptr->pclass == CLASS_SORCERER)) && p_ptr->icky_wield[0]) chance += 25;
	if (((p_ptr->pclass == CLASS_PRIEST) || (p_ptr->pclass == CLASS_SORCERER)) && p_ptr->icky_wield[1]) chance += 25;

	chance = mod_spell_chance_1(chance);

	/* Goodness or evilness gives a penalty to failure rate */
	/*
	switch (use_realm)
	{
	case REALM_NATURE:
		if ((p_ptr->align > 50) || (p_ptr->align < -50)) chance += penalty;
		break;
	case REALM_LIFE: case REALM_CRUSADE:
		if (p_ptr->align < -20) chance += penalty;
		break;
	case REALM_DEATH: case REALM_DAEMON: case REALM_HEX:
		if (p_ptr->align > 20) chance += penalty;
		break;
	}
	*/
	/* Minimum failure rate */
	if (chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
	else if (p_ptr->stun) chance += 15;

	///mod140402 ���|��Ԃ��Ǝ��s���オ��悤�ɂ��Ă݂�
	if(p_ptr->afraid) chance += 20;


	/* Always a 5 percent chance of working */
	if (chance > 95) chance = 95;

	if ((use_realm == p_ptr->realm1) || (use_realm == p_ptr->realm2)
//	    || (p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
		|| (REALM_SPELLMASTER))
	{
		s16b exp = experience_of_spell(spell, use_realm);
		if (exp >= SPELL_EXP_EXPERT) chance--;
		if (exp >= SPELL_EXP_MASTER) chance--;
	}

	/* Return the chance */
	return mod_spell_chance_2(chance);
}



/*
 * Determine if a spell is "okay" for the player to cast or study
 * The spell must be legible, not forgotten, and also, to cast,
 * it must be known, and to study, it must not be known.
 */
/*:::���������̖��@���g��/�w�Ԃ��Ƃ��ł��邩���肷��*/
/*:::spell:���@�E���Z�ԍ� 0-31?*/
/*:::learned:���Ɋw�K�ς݂Ȃ�TRUE*/
/*:::study_pray:�����E�̊w�K�̂Ƃ�TRUE?*/
/*:::use_realm:�g�p�̈������ REALM_???�̒萔*/
bool spell_okay(int spell, bool learned, bool study_pray, int use_realm)
{
	magic_type forge, *s_ptr = &forge;


	/* Access the spell */

	/*:::magic_type�^�̌��ݑI�����Ă��閂�@�A�Z�̏��𓾂�*/
#if 0
	if (!is_magic(use_realm))
	{
		/*:::�́A��p�A���p*/
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	}
#endif
	//���͕ʃ��[�g�Ŗ��@���g��
	if(flag_spell_consume_book)
	{
		return TRUE;
	}

	if(p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU) s_ptr = &hissatsu_info[spell];
	else
	{
		//s_ptr = &mp_ptr->info[use_realm - 1][spell];
		calc_spell_info(s_ptr,use_realm,spell);
	}

	/* Spell is illegal */
	/*:::���x��������Ȃ��i�������͏K���s�\�j�Ȃ�I��*/
	if (s_ptr->slevel > p_ptr->lev) return (FALSE);

	/* Spell is forgotten */
	/*:::�Y�ꂽ���@�Ȃ�I���@�w�K�̑Ώۂɂ��Ȃ�Ȃ�*/
	if ((use_realm == p_ptr->realm2) ?
	    (p_ptr->spell_forgotten2 & (1L << spell)) :
	    (p_ptr->spell_forgotten1 & (1L << spell)))
	{
		/* Never okay */
		return (FALSE);
	}

	/*:::�X�y�}�X�ƐԖ��͏��TRUE �Ԗ��̎O���ڈȍ~��Ԗ��A�X�y�}�X�̉̂�K�E���͖{�I���̂Ƃ��ɒe����Ă���͂�*/
	///class
	//if (p_ptr->pclass == CLASS_SORCERER) return (TRUE);
	//if (p_ptr->pclass == CLASS_RED_MAGE) return (TRUE);
		/*:::���@���ŏ�����K�����Ă�E�@�g�p�\�ȗ̈�͑S��TRUE*/
		if( (REALM_SPELLMASTER) && cp_ptr->realm_aptitude[use_realm] !=0 ) return (TRUE);

		//v1.1.32 �p�`�����[��p���i�u���s�v�̗�O����
		if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && use_realm <= MAX_MAGIC)
			return TRUE;

	/* Spell is learned */
	if ((use_realm == p_ptr->realm2) ?
	    (p_ptr->spell_learned2 & (1L << spell)) :
	    (p_ptr->spell_learned1 & (1L << spell)))
	{
		/* Always true */
		return (!study_pray);
	}

	/* Okay to study, not to cast */
	return (!learned);
}


/*
 * Print a list of spells (for browsing or casting or viewing)
 */
/*:::�I�������̈�A�����ڂ��疂�@�ꗗ��\������@�Y�p�┻�Ǖs�\���l��*/
/*:::target_spell �ڍוs���B���j���[�\���̂Ƃ��g���錻�݃J�[�\���̂�������H*/
/*:::spells byte�^�z��@�I�����ꂽ�{�ɑΉ����Ă���spell�ԍ��������Ă��� */
/*:::num ���@�����ł͎���8�H*/
/*:::y,x �\���J�n�ʒu�H�@�T�u�E�B���h�E�ɖ��@�ꗗ��\������Ƃ��g���̂�*/

///class realm
void print_spells(int target_spell, byte *spells, int num, int y, int x, int use_realm)
{
	int             i, spell, exp_level, increment = 64;
	magic_type forge, *s_ptr = &forge;
	cptr            comment;
	char            info[80];
	char            out_val[160];
	byte            line_attr;
	int             need_mana;
	char            ryakuji[5];
	char            buf[256];
	bool max = FALSE;

	if (((use_realm <= 0) || (use_realm > TV_BOOK_END))) msg_format("ERROR:print_spell(realm:%d)",use_realm);
/*
	if (((use_realm <= REALM_NONE) || (use_realm > MAX_REALM)) && p_ptr->wizard)
#ifdef JP
msg_print("�x���I print_spell ���̈�Ȃ��ɌĂ΂ꂽ");
#else
		msg_print("Warning! print_spells called with null realm");
#endif
*/

	/* Title the list */
	/*:::���p�ƂƂ���ȊO�͕\�����@���Ⴄ*/
	prt("", y, x);
	if (use_realm == TV_BOOK_HISSATSU)
#ifdef JP
		strcpy(buf,"  Lv   MP");
#else
		strcpy(buf,"  Lv   SP");
#endif
	else
#ifdef JP
		strcpy(buf,"�n���x Lv   MP ���� ����");
#else
		strcpy(buf,"Profic Lv   SP Fail Effect");
#endif

#ifdef JP
put_str("���O", y, x + 5);
put_str(buf, y, x + 29);
#else
	put_str("Name", y, x + 5);
	put_str(buf, y, x + 29);
#endif

	//if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE)) increment = 0;
	if(REALM_SPELLMASTER) increment = 0;
	else if (use_realm == p_ptr->realm1) increment = 0;
	else if (use_realm == p_ptr->realm2) increment = 32;

	/* Dump the spells */
	for (i = 0; i < num; i++)
	{
		/* Access the spell */
		spell = spells[i];

		/* Access the spell */
		/*
		if (!is_magic(use_realm))
		{
			s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
		}
		*/
		if (use_realm == TV_BOOK_HISSATSU)s_ptr = &hissatsu_info[spell];
		else
		{
			//s_ptr = &mp_ptr->info[use_realm - 1][spell];
			calc_spell_info(s_ptr,use_realm,spell);
		}

		if (use_realm == TV_BOOK_HISSATSU)
			need_mana = s_ptr->smana;
		else
		{
			/*:::�w�肵�����@�̌��݂̌o���l��Ԃ�*/
			s16b exp = experience_of_spell(spell, use_realm);

			/* Extract mana consumption rate */
			/*:::�w�肵�����@�̎g�pMP���v�Z����*/
			need_mana = mod_need_mana(s_ptr->smana, spell, use_realm);

			/*:::increment��64�Ȃ̂͂ǂ�ȂƂ��H*/
			if (flag_spell_consume_book || (increment == 64) || (s_ptr->slevel >= 99)) exp_level = EXP_LEVEL_UNSKILLED;
			/*:::���@�̋Z�\�l���n�����x���p�̒l�ɒ����Ă���炵���@SPELL_EXP_SKILLED��EXP_LEVEL_SKILLED�Ȃ�*/
			else exp_level = spell_exp_level(exp);

			max = FALSE;
			/*:::�̈��N���X�ȂǍl��������ȏ�o���l���オ��Ȃ��ꍇ�t���O���Ă�*/
			if(flag_spell_consume_book) max = TRUE;
			if (!increment && (exp_level == EXP_LEVEL_MASTER)) max = TRUE;
			else if ((increment == 32) && (exp_level >= EXP_LEVEL_EXPERT)) max = TRUE;
			else if (s_ptr->slevel >= 99) max = TRUE;
			//else if ((p_ptr->pclass == CLASS_RED_MAGE) && (exp_level >= EXP_LEVEL_SKILLED)) max = TRUE;
			else if(REALM_SPELLMASTER) max = TRUE;

			strncpy(ryakuji, exp_level_str[exp_level], 4);
			ryakuji[3] = ']';
			ryakuji[4] = '\0';
		}

		if (use_menu && target_spell)
		{
			if (i == (target_spell-1))
#ifdef JP
				strcpy(out_val, "  �t ");
#else
				strcpy(out_val, "  >  ");
#endif
			else
				strcpy(out_val, "     ");
		}
		else sprintf(out_val, "  %c) ", I2A(i));
		/* Skip illegible spells */
		if (s_ptr->slevel >= 99)
		{
#ifdef JP
strcat(out_val, format("%-30s", "(���Ǖs�\)"));
#else
				strcat(out_val, format("%-30s", "(illegible)"));
#endif

				c_prt(TERM_L_DARK, out_val, y + i + 1, x);
				continue;
		}

		/* XXX XXX Could label spells above the players level */

		/* Get extra info */
		strcpy(info, do_spell(use_realm, spell, SPELL_INFO));

		/* Use that info */
		comment = info;

		/* Assume spell is known and tried */
		line_attr = TERM_WHITE;

		/* Analyze the spell */
		//if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
		if(flag_spell_consume_book)
		{
			line_attr = TERM_VIOLET;
		}
		else if(REALM_SPELLMASTER) 
		{
			if (s_ptr->slevel > p_ptr->max_plv)
			{
#ifdef JP
comment = "���m";
#else
				comment = "unknown";
#endif

				line_attr = TERM_L_BLUE;
			}
			else if (s_ptr->slevel > p_ptr->lev)
			{
#ifdef JP
comment = "�Y�p";
#else
				comment = "forgotten";
#endif

				line_attr = TERM_YELLOW;
			}
		}
		else if ((use_realm != p_ptr->realm1) && (use_realm != p_ptr->realm2))
		{
#ifdef JP
comment = "���m";
#else
			comment = "unknown";
#endif

			line_attr = TERM_L_BLUE;
		}
		else if ((use_realm == p_ptr->realm1) ?
		    ((p_ptr->spell_forgotten1 & (1L << spell))) :
		    ((p_ptr->spell_forgotten2 & (1L << spell))))
		{
#ifdef JP
comment = "�Y�p";
#else
			comment = "forgotten";
#endif

			line_attr = TERM_YELLOW;
		}
		else if (!((use_realm == p_ptr->realm1) ?
		    (p_ptr->spell_learned1 & (1L << spell)) :
		    (p_ptr->spell_learned2 & (1L << spell))))
		{
#ifdef JP
comment = "���m";
#else
			comment = "unknown";
#endif

			line_attr = TERM_L_BLUE;
		}
		else if (!((use_realm == p_ptr->realm1) ?
		    (p_ptr->spell_worked1 & (1L << spell)) :
		    (p_ptr->spell_worked2 & (1L << spell))))
		{
#ifdef JP
comment = "���o��";
#else
			comment = "untried";
#endif

			line_attr = TERM_L_GREEN;
		}

		/* Dump the spell --(-- */
		if (use_realm == TV_BOOK_HISSATSU)
		{
			strcat(out_val, format("%-25s %2d %4d",
			    do_spell(use_realm, spell, SPELL_NAME), /* realm, spell */
			    s_ptr->slevel, need_mana));
		}
		else
		{
			strcat(out_val, format("%-25s%c%-4s %2d %4d %3d%% %s",
			    do_spell(use_realm, spell, SPELL_NAME), /* realm, spell */
			    (max ? '!' : ' '), ryakuji,
			    s_ptr->slevel, need_mana, spell_chance(spell, use_realm), comment));
		}
		c_prt(line_attr, out_val, y + i + 1, x);
	}

	/* Clear the bottom line */
	prt("", y + i + 1, x);
}


/*
 * Note that amulets, rods, and high-level spell books are immune
 * to "inventory damage" of any kind.  Also sling ammo and shovels.
 */


/*
 * Does a given class of objects (usually) hate acid?
 * Note that acid can either melt or corrode something.
 */
/*:::�A�C�e�����_�ŉ��邩����B���f�ی삪�t�����ꂽ�A�C�e���͂��̊֐��̊O�ŏ����B*/
///item TVAL���Ƃ̌��f�j��۔���
bool hates_acid(object_type *o_ptr)
{
	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Wearable items */
		case TV_ARROW:
		case TV_BOLT:
		case TV_BULLET:
		case TV_BOW:
		case TV_CROSSBOW:
		case TV_GUN:

	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_OTHERWEAPON:	
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HEAD:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_CLOTHES:
	case TV_ARMOR:
	case TV_RIBBON:
		case TV_BUNBUN:
		case TV_KAKASHI:
		{
			return (TRUE);
		}

		/* Staffs/Scrolls are wood/paper */
		case TV_STAFF:
		case TV_SCROLL:
		{
			return (TRUE);
		}

		/* Ouch */
		case TV_CHEST:
		{
			return (TRUE);
		}


	}

	return (FALSE);
}


/*
 * Does a given object (usually) hate electricity?
 */
/*:::�A�C�e�����d�C�ŉ��邩����B���f�ی삪�t�����ꂽ�A�C�e���͂��̊֐��̊O�ŏ����B*/
///item TVAL���Ƃ̌��f�j��۔���
bool hates_elec(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_RING:
		case TV_WAND:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Does a given object (usually) hate fire?
 * Hafted/Polearm weapons have wooden shafts.
 * Arrows/Bows are mostly wooden.
 */
/*:::�A�C�e�����΂ŉ��邩����B���f�ی삪�t�����ꂽ�A�C�e���͂��̊֐��̊O�ŏ����B*/
///item TVAL���Ƃ̌��f�j��۔���
bool hates_fire(object_type *o_ptr)
{
	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Wearable */
		case TV_LITE:
		case TV_ARROW:
		case TV_BOW:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CLOTHES:
		case TV_BUNBUN:
		case TV_KAKASHI:

		{
			return (TRUE);
		}

		/* Books */
		case TV_BOOK_ELEMENT:
		case TV_BOOK_FORESEE:
		case TV_BOOK_ENCHANT:
		case TV_BOOK_SUMMONS:
		case TV_BOOK_MYSTIC:
		case TV_BOOK_LIFE:
		case TV_BOOK_PUNISH:
		case TV_BOOK_NATURE:
		case TV_BOOK_TRANSFORM:
		case TV_BOOK_DARKNESS:
		case TV_BOOK_NECROMANCY:
		case TV_BOOK_CHAOS:
  		case TV_BOOK_HISSATSU:
  		case TV_BOOK_MEDICINE:	
		case TV_BOOK_OCCULT:
			
		{
			return (TRUE);
		}

		/* Chests */
		case TV_CHEST:
		{
			return (TRUE);
		}

		/* Staffs/Scrolls burn */
		case TV_STAFF:
		case TV_SCROLL:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Does a given object (usually) hate cold?
 */
/*:::�A�C�e������C�ŉ��邩����B���f�ی삪�t�����ꂽ�A�C�e���͂��̊֐��̊O�ŏ����B*/
///item TVAL���Ƃ̌��f�j��۔���
///mod140819 TV_FLASK�ɑ��̃A�C�e�����₵���̂Ŗ��₾���Ώۂɂ���悤�C��
bool hates_cold(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_POTION:
			return (TRUE);
		case TV_FLASK:
			if(o_ptr->sval == SV_FLASK_OIL) return TRUE;
	}

	return (FALSE);
}


/*
 * Melt something
 */
/*:::�A�C�e����TVAL�����̑����Ɏキ�A�����̃A�C�e���ʂɑϐ����t���Ă��Ȃ��Ƃ�TRUE*/
int set_acid_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_acid(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_ACID)) return (FALSE);
	return (TRUE);
}


/*
 * Electrical damage
 */
/*:::�A�C�e����TVAL�����̑����Ɏキ�A�����̃A�C�e���ʂɑϐ����t���Ă��Ȃ��Ƃ�TRUE*/

int set_elec_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_elec(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_ELEC)) return (FALSE);
	return (TRUE);
}


/*
 * Burn something
 */
/*:::�A�C�e����TVAL�����̑����Ɏキ�A�����̃A�C�e���ʂɑϐ����t���Ă��Ȃ��Ƃ�TRUE*/

int set_fire_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_fire(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_FIRE)) return (FALSE);
	return (TRUE);
}


/*
 * Freeze things
 */
/*:::�A�C�e����TVAL�����̑����Ɏキ�A�����̃A�C�e���ʂɑϐ����t���Ă��Ȃ��Ƃ�TRUE*/

int set_cold_destroy(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if (!hates_cold(o_ptr)) return (FALSE);
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_IGNORE_COLD)) return (FALSE);
	return (TRUE);
}


/*
 * Destroys a type of item on a given percent chance
 * Note that missiles are no longer necessarily all destroyed
 * Destruction taken from "melee.c" code for "stealing".
 * New-style wands and rods handled correctly. -LM-
 * Returns number of items destroyed.
 */
/*:::�����i�ւ̔j�󏈗�*/
/*:::typ:�j��ۂ����߂邽�߂̊֐��ւ̃|�C���^�@���̑������ŕς���*/
/*:::perc:�j��p���[�@�_���[�W�Ō��܂�*/
int inven_damage(inven_func typ, int perc)
{
	int         i, j, k, amt;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];

	int inaba_card_num;
	bool inaba_protect = FALSE;

	if (CHECK_MULTISHADOW()) return 0;

	//v1.1.51 �V�A���[�i�ł̓A�C�e���j����󂯂�
//	if (p_ptr->inside_arena) return 0;

	/* Count the casualties */
	k = 0;

	//v1.1.86 �K�^�������̑��J�[�h�ɂ��j��ی�
	inaba_card_num = count_ability_card(ABL_CARD_INABA_LEG);
	if (inaba_card_num)
	{
		int prob = 25;

		if (inaba_card_num > 1) prob += (inaba_card_num - 1) * 8;

		if (inaba_card_num >= 9 || randint1(100)<prob) 
			inaba_protect = TRUE;

	}

	/* Scan through the slots backwards */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Hack -- for now, skip artifacts */
		if (object_is_artifact(o_ptr)) continue;

		//�����x���̖�t�͖�������Ȃ�
		if(p_ptr->pclass == CLASS_CHEMIST && p_ptr->lev > 39 && 
			(o_ptr->tval == TV_POTION || o_ptr->tval == TV_COMPOUND))
			continue;

		/* Give this item slot a shot at death */
		if ((*typ)(o_ptr))
		{
			/* Count the casualties */
			for (amt = j = 0; j < o_ptr->number; ++j)
			{
				if (randint0(100) < perc) amt++;
			}
			//v1.1.86 
			if (amt && inaba_protect)
			{
				msg_print("�K�^�̗͂��A�C�e����j�󂩂������I");
				return k;
			}

			/* Some casualities */
			if (amt)
			{
				/* Get a description */
				object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

				/* Message */
#ifdef JP
msg_format("%s(%c)��%s���Ă��܂����I",
#else
				msg_format("%sour %s (%c) %s destroyed!",
#endif

#ifdef JP
o_name, index_to_label(i),
    ((o_ptr->number > 1) ?
    ((amt == o_ptr->number) ? "�S��" :
    (amt > 1 ? "����" : "���")) : "")    );
#else
				    ((o_ptr->number > 1) ?
				    ((amt == o_ptr->number) ? "All of y" :
				    (amt > 1 ? "Some of y" : "One of y")) : "Y"),
				    o_name, index_to_label(i),
				    ((amt > 1) ? "were" : "was"));
#endif
///del131208
/*
#ifdef JP
				if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_print("���₪�����ȁI");
#endif
*/
				/* Potions smash open */
				/*:::��Ȃ犄���*/
				if (object_is_potion(o_ptr))
				{
					(void)potion_smash_effect(0, py, px, o_ptr->k_idx);
				}

				/* Reduce the charges of rods/wands */
				/*:::���@�_�Ȃ�񐔂�����*/
				reduce_charges(o_ptr, amt);

				/* Destroy "amt" items */
				/*:::�A�C�e�����̌�������*/
				inven_item_increase(i, -amt);
				inven_item_optimize(i);

				/* Count the casualties */
				k += amt;
			}
		}
	}

	/* Return the casualty count */
	return (k);
}


/*
 * Acid has hit the player, attempt to affect some armor.
 *
 * Note that the "base armor" of an object never changes.
 *
 * If any armor is damaged (or resists), the player takes less damage.
 */
/*:::�_�ɂ��h��N�H�@�h��Ɏ_��������΁i�����Ȃ��Ă��j�O���Ł��ւ̃_���[�W������*/
static int minus_ac(void)
{
	object_type *o_ptr = NULL;
	u32b flgs[TR_FLAG_SIZE];
	char        o_name[MAX_NLEN];

	int slot = randint1(7);
	if(check_invalidate_inventory(slot)) return FALSE;

	/* Pick a (possibly empty) inventory slot */
	switch (slot)
	{
		case 1: o_ptr = &inventory[INVEN_RARM]; break;
		case 2: o_ptr = &inventory[INVEN_LARM]; break;
		case 3: o_ptr = &inventory[INVEN_BODY]; break;
		case 4: o_ptr = &inventory[INVEN_OUTER]; break;
		case 5: o_ptr = &inventory[INVEN_HANDS]; break;
		case 6: o_ptr = &inventory[INVEN_HEAD]; break;
		case 7: o_ptr = &inventory[INVEN_FEET]; break;
	}

	/* Nothing to damage */
	/*:::���̃X���b�g�ɉ����������Ă��Ȃ���ΏI���@���փ_���[�W�f�ʂ�*/
	if (!o_ptr->k_idx) return (FALSE);


	/*:::���⃂���X�^�[�{�[���͏I�����փ_���[�W�f�ʂ�*/
	if (!object_is_armour(o_ptr)) return (FALSE);

	/* No damage left to be done */
	/*:::�h��̍��vAC������0�Ȃ�I���@�_���[�W�f�ʂ�*/
	if (o_ptr->ac + o_ptr->to_a <= 0) return (FALSE);


	/* Describe */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Object resists */
	/*:::�_�ϐ��h���A�[�e�B�t�@�N�g�͏����Ȃ��@���ւ̃_���[�W����*/
	if (have_flag(flgs, TR_IGNORE_ACID))
	{
#ifdef JP
msg_format("������%s�ɂ͌��ʂ��Ȃ������I", o_name);
#else
		msg_format("Your %s is unaffected!", o_name);
#endif


		return (TRUE);
	}

	/* Message */
	/*:::�h��Ƀ_���[�W�@ac�����������{�[�i�X�ƃA���h���C�h�o���l���Čv�Z*/
#ifdef JP
msg_format("%s���_���[�W���󂯂��I", o_name);
#else
	msg_format("Your %s is damaged!", o_name);
#endif


	/* Damage the item */
	o_ptr->to_a--;

	/* Calculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Window stuff */
	p_ptr->window |= (PW_EQUIP | PW_PLAYER);

	calc_android_exp();

	/* Item was damaged */
	return (TRUE);
}


/*
 * Hurt the player with Acid
 */
/*:::�_�_���[�W����*/
int acid_dam(int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_ACID();

	/* Total Immunity */
	if (p_ptr->immune_acid || (dam <= 0))
	{
		learn_spell(monspell);
		return 0;
	}

	///mod140302 ���f��_�A�ϐ���������
	dam = mod_acid_dam(dam);

	/* Vulnerability (Ouch!) */
	//if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
	//if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;

	/* Resist the damage */
	//if (p_ptr->resist_acid) dam = (dam + 2) / 3;
	//if (double_resist) dam = (dam + 2) / 3;

	if (!CHECK_MULTISHADOW())
	{
		if ((!(double_resist || p_ptr->resist_acid)) &&
		    one_in_(HURT_CHANCE))
			(void)do_dec_stat(A_CHR);

		/* If any armor gets hit, defend the player */
		if (minus_ac()) dam = (dam + 1) / 2;
	}

	/* Take damage */
	get_damage = take_hit(DAMAGE_ATTACK, dam, kb_str, monspell);

	/* Inventory damage */
	if (!(double_resist && p_ptr->resist_acid))
		inven_damage(set_acid_destroy, inv);
	return get_damage;
}


/*
 * Hurt the player with electricity
 */
/*:::�d���_���[�W*/
int elec_dam(int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_ELEC();

	/* Total immunity */
	if (p_ptr->immune_elec || (dam <= 0))
	{
		learn_spell(monspell);
		return 0;
	}
	///mod140302 ���f��_�A�ϐ���������
	dam = mod_elec_dam(dam);
	/*:::�e�T���ꏈ��*/
	if(p_ptr->pclass == CLASS_KAGEROU && !p_ptr->resist_fear && !p_ptr->mimic_form && dam > randint1(100))
	{
		if(!p_ptr->afraid) msg_print("�u�Ё[�A���̃L���[�e�B�N�����[�I�v");
		set_afraid(p_ptr->afraid + randint0(10) + 10);
	}

	/* Vulnerability (Ouch!) */
	//if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
	//if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;
	//if (prace_is_(RACE_ANDROID)) dam += dam / 3;

	//if (prace_is_(RACE_KAPPA))
	//{
	//	if(!(p_ptr->resist_elec && double_resist))msg_format("�d���͋�肾�I");
	//	dam += dam / 3;
	//}
	/* Resist the damage */
	//if (p_ptr->resist_elec) dam = (dam + 2) / 3;
	//if (double_resist) dam = (dam + 2) / 3;

	if ((!(double_resist || p_ptr->resist_elec)) &&
	    one_in_(HURT_CHANCE) && !CHECK_MULTISHADOW())
		(void)do_dec_stat(A_DEX);

	/* Take damage */
	get_damage = take_hit(DAMAGE_ATTACK, dam, kb_str, monspell);

	///mod140821 �G���W�j�A�̋@�B���̏Ⴗ�锻��
	engineer_malfunction(GF_ELEC, dam);

	/* Inventory damage */
	if (!(double_resist && p_ptr->resist_elec))
		inven_damage(set_elec_destroy, inv);

	return get_damage;
}


/*
 * Hurt the player with Fire
 */
/*:::�Ή��_���[�W����*/
int fire_dam(int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_FIRE();

	/* Totally immune */
	if (p_ptr->immune_fire || (dam <= 0))
	{
		learn_spell(monspell);
		return 0;
	}

	/* Vulnerability (Ouch!) */
	//if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
	//if (prace_is_(RACE_ENT)) dam += dam / 3;
	//if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;

	/* Resist the damage */
	//if (p_ptr->resist_fire) dam = (dam + 2) / 3;
	//if (double_resist) dam = (dam + 2) / 3;

	///mod140302 ���f��_�A�ϐ���������
	dam = mod_fire_dam(dam);


	/*:::�e�T���ꏈ��*/
	if(p_ptr->pclass == CLASS_KAGEROU && !p_ptr->resist_fear && !p_ptr->mimic_form && dam > randint1(100))
	{
		if(!p_ptr->afraid) msg_print("�u�Ё[�A���̃L���[�e�B�N�����[�I�v");
		set_afraid(p_ptr->afraid + randint0(10) + 10);
	}

	if ((!(double_resist || p_ptr->resist_fire)) &&
	    one_in_(HURT_CHANCE) && !CHECK_MULTISHADOW())
		(void)do_dec_stat(A_STR);

	/* Take damage */
	get_damage = take_hit(DAMAGE_ATTACK, dam, kb_str, monspell);

	/* Inventory damage */
	if (!(double_resist && p_ptr->resist_fire))
		inven_damage(set_fire_destroy, inv);

	return get_damage;
}


/*
 * Hurt the player with Cold
 */
int cold_dam(int dam, cptr kb_str, int monspell)
{
	int get_damage;  
	int inv = (dam < 30) ? 1 : (dam < 60) ? 2 : 3;
	bool double_resist = IS_OPPOSE_COLD();

	/* Total immunity */
	if (p_ptr->immune_cold || (dam <= 0))
	{
		learn_spell(monspell);
		return 0;
	}
	///mod140302 ���f��_�A�ϐ���������
	dam = mod_cold_dam(dam);

	/* Vulnerability (Ouch!) */
	//if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
	//if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;

	/* Resist the damage */
	//if (p_ptr->resist_cold) dam = (dam + 2) / 3;
	//if (double_resist) dam = (dam + 2) / 3;

	if ((!(double_resist || p_ptr->resist_cold)) &&
	    one_in_(HURT_CHANCE) && !CHECK_MULTISHADOW())
		(void)do_dec_stat(A_STR);

	/* Take damage */
	get_damage = take_hit(DAMAGE_ATTACK, dam, kb_str, monspell);

	/* Inventory damage */
	if (!(double_resist && p_ptr->resist_cold))
		inven_damage(set_cold_destroy, inv);

	return get_damage;
}

/*:::���H�j�~�̖��@*/
bool rustproof(void)
{
	int         item;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	cptr        q, s;

	item_tester_no_ryoute = TRUE;
	/* Select a piece of armour */
	item_tester_hook = object_is_armour;

	/* Get an item */
#ifdef JP
q = "�ǂ̖h��ɎK�~�߂����܂����H";
s = "�K�~�߂ł�����̂�����܂���B";
#else
	q = "Rustproof which piece of armour? ";
	s = "You have nothing to rustproof.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}

	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);

	if ((o_ptr->to_a < 0) && !object_is_cursed(o_ptr))
	{
#ifdef JP
msg_format("%s�͐V�i���l�ɂȂ����I",o_name);
#else
		msg_format("%s %s look%s as good as new!",
			((item >= 0) ? "Your" : "The"), o_name,
			((o_ptr->number > 1) ? "" : "s"));
#endif

		o_ptr->to_a = 0;
	}

#ifdef JP
msg_format("%s�͕��H���Ȃ��Ȃ����B", o_name);
#else
	msg_format("%s %s %s now protected against corrosion.",
		((item >= 0) ? "Your" : "The"), o_name,
		((o_ptr->number > 1) ? "are" : "is"));
#endif


	calc_android_exp();

	return TRUE;
}


/*
 * Curse the players armor
 */
/*:::�h����� �Z�̂�*/
bool curse_armor(void)
{
	int i;
	object_type *o_ptr;

	char o_name[MAX_NLEN];

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_format("���Ȃ��͋��|�̈Í��I�[�����z��������B");
		hina_gain_yaku(50 + randint1(50));
		return FALSE;
	}

	if (p_ptr->pclass == CLASS_SHION)
	{
		msg_format("���Ȃ��͋��|�̈Í��I�[�����z��������B");
		p_ptr->magic_num1[1] += 10000;
		return FALSE;
	}

	///mod151205 �O�����͓K���ɊZ��I��
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		int cnt = 0;
		int slot = 0;

		//v1.1.68 ��p���i��INVEN_RIBBON�ɂ��Z�𒅂Ă��邱�Ƃ�����
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			i = INVEN_RIBBON;
		else
			i = INVEN_RIGHT;

		for(;i<=INVEN_FEET;i++)
		{
			o_ptr = &inventory[i];
			if(!o_ptr->k_idx) continue;
			if(wield_slot(o_ptr) != INVEN_BODY) continue;
			cnt++;
			if(one_in_(cnt)) slot=i;
		}

		if(!slot) return FALSE;
		else o_ptr = &inventory[slot];

	}
	else
	{
		/* Curse the body armor */
		o_ptr = &inventory[INVEN_BODY];
		/* Nothing to curse */
		if (!o_ptr->k_idx) return (FALSE);
		if(check_invalidate_inventory(INVEN_BODY)) return FALSE;
	}

	/* Describe */
	object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

	/* Attempt a saving throw for artifacts */
	if (object_is_artifact(o_ptr) && (randint0(100) < 50))
	{
		/* Cool */
#ifdef JP
msg_format("%s��%s���ݍ������Ƃ������A%s�͂���𒵂˕Ԃ����I",
"���|�̈Í��I�[��", "�h��", o_name);
#else
		msg_format("A %s tries to %s, but your %s resists the effects!",
			   "terrible black aura", "surround your armor", o_name);
#endif

	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
#ifdef JP
msg_format("���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I", o_name);
#else
		msg_format("A terrible black aura blasts your %s!", o_name);
#endif
		///del131215 virtue
		//chg_virtue(V_ENCHANT, -5);

		/* Blast the armor */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_BLASTED;
		o_ptr->to_a = 0 - randint1(5) - randint1(5);
		o_ptr->to_h = 0;
		o_ptr->to_d = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		for (i = 0; i < TR_FLAG_SIZE; i++)
			o_ptr->art_flags[i] = 0;

		/* Curse it */
		o_ptr->curse_flags = TRC_CURSED;

		/* Break it */
		o_ptr->ident |= (IDENT_BROKEN);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	return (TRUE);
}


/*
 * Curse the players weapon
 */
/*:::�������*/
///sys ��������@�ꕔ�L�����̎��S���������悤
bool curse_weapon_object(bool force, object_type *o_ptr)
{
	int i;
	char o_name[MAX_NLEN];

	//v1.1.34 ���v�̂͂������ꉞ�ǉ�
	if(!object_is_melee_weapon(o_ptr)) 
		return FALSE;

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_format("���Ȃ��͋��|�̈Í��I�[�����z��������B");
		hina_gain_yaku(50 + randint1(50));
		return FALSE;
	}
	if (p_ptr->pclass == CLASS_SHION)
	{
		msg_format("���Ȃ��͋��|�̈Í��I�[�����z��������B");
		p_ptr->magic_num1[1] += 10000;
		return FALSE;
	}

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);

	/* Describe */
	object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

	/* Attempt a saving throw */
	if (object_is_artifact(o_ptr) && (randint0(100) < 50) && !force)
	{
		/* Cool */
#ifdef JP
		msg_format("%s��%s���ݍ������Ƃ������A%s�͂���𒵂˕Ԃ����I",
				"���|�̈Í��I�[��", "����", o_name);
#else
		msg_format("A %s tries to %s, but your %s resists the effects!",
				"terrible black aura", "surround your weapon", o_name);
#endif
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
#ifdef JP
		if (!force) msg_format("���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I", o_name);
#else
		if (!force) msg_format("A terrible black aura blasts your %s!", o_name);
#endif
		///del131215 virtue
		//chg_virtue(V_ENCHANT, -5);
		//���P�̖{�̂̎P�Ɣ��_���̎P�̃x�[�X�A�C�e���𓯂��ɂ���������weight�ŗ�O���肵�Ȃ��Ƃ����Ȃ��Ȃ���
		if(p_ptr->pclass == CLASS_KOGASA && o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && o_ptr->weight == 30)
		{
			take_hit(DAMAGE_LOSELIFE,9999,"���|�̈Í��I�[��",-1);
			return TRUE;
		}

		/* Shatter the weapon */
		o_ptr->name1 = 0;
		o_ptr->name2 = EGO_SHATTERED;
		o_ptr->to_h = 0 - randint1(5) - randint1(5);
		o_ptr->to_d = 0 - randint1(5) - randint1(5);
		o_ptr->to_a = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		for (i = 0; i < TR_FLAG_SIZE; i++)
			o_ptr->art_flags[i] = 0;

		/* Curse it */
		o_ptr->curse_flags = TRC_CURSED;

		/* Break it */
		o_ptr->ident |= (IDENT_BROKEN);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	}

	/* Notice */
	return (TRUE);
}

bool curse_weapon(bool force, int slot)
{

	if(check_invalidate_inventory(slot)) return FALSE;

	/* Curse the weapon */
	return curse_weapon_object(force, &inventory[slot]);
}


/*
 * Enchant some bolts
 */
/*:::�n��̃t�@�C�A�{���g*/
///item �N���X�{�E�̖���Ή��G�S�ɂ���
bool brand_bolts(void)
{
	int i;

	/* Use the first acceptable bolts */
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-bolts */
		if (o_ptr->tval != TV_BOLT) continue;

		/* Skip artifacts and ego-items */
		if (object_is_artifact(o_ptr) || object_is_ego(o_ptr))
			continue;

		/* Skip cursed/broken items */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) continue;

		/* Randomize */
		if (randint0(100) < 75) continue;

		/* Message */
#ifdef JP
msg_print("�N���X�{�E�̖���̃I�[���ɕ�܂ꂽ�I");
#else
		msg_print("Your bolts are covered in a fiery aura!");
#endif


		/* Ego-item */
		o_ptr->name2 = EGO_ARROW_FIRE;

		/* Enchant */
		enchant(o_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		/* Notice */
		return (TRUE);
	}

	/* Flush */
	if (flush_failure) flush();

	/* Fail */
#ifdef JP
msg_print("���ŋ�������̂Ɏ��s�����B");
#else
	msg_print("The fiery enchantment failed.");
#endif


	/* Notice */
	return (TRUE);
}


/*
 * Helper function -- return a "nearby" race for polymorphing
 *
 * Note that this function is one of the more "dangerous" ones...
 */
static s16b poly_r_idx(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	int i, r, lev1, lev2;

	/* Hack -- Uniques/Questors never polymorph */
	if ((r_ptr->flags1 & RF1_UNIQUE) ||
	    (r_ptr->flags1 & RF1_QUESTOR))
		return (r_idx);


	/* Allowable range of "levels" for resulting monster */
	lev1 = r_ptr->level - ((randint1(20) / randint1(9)) + 1);
	lev2 = r_ptr->level + ((randint1(20) / randint1(9)) + 1);

	/* Pick a (possibly new) non-unique race */
	for (i = 0; i < 1000; i++)
	{
		/* Pick a new race, using a level calculation */
		r = get_mon_num((dun_level + r_ptr->level) / 2 + 5);

		/* Handle failure */
		if (!r) break;

		/* Obtain race */
		r_ptr = &r_info[r];

		/* Ignore unique monsters */
		if (r_ptr->flags1 & RF1_UNIQUE) continue;

		//�σp�����[�^�����X�^�[�ɂ͕ω����Ȃ��悤�ɂ��Ă���
		if(r_ptr->flags7 & RF7_VARIABLE) continue;

		///mod160316 EASY�ȂǂŐ����֎~�̃����X�^�[�ɂ͕ω����Ȃ�
		if(!check_rest_f50(r)) continue;

		/* Ignore monsters with incompatible levels */
		if ((r_ptr->level < lev1) || (r_ptr->level > lev2)) continue;

		/* Use that index */
		r_idx = r;

		/* Done */
		break;
	}

	/* Result */
	return (r_idx);
}

/*:::�`�F���W�E�����X�^�[�@�ϐg��̃����X�^�[�͑O�̃����X�^�[���E���Ă����A�C�e���������p��*/
bool polymorph_monster(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];
	monster_type *m_ptr = &m_list[c_ptr->m_idx];
	bool polymorphed = FALSE;
	int new_r_idx;
	int old_r_idx = m_ptr->r_idx;
	bool targeted = (target_who == c_ptr->m_idx) ? TRUE : FALSE;
	bool health_tracked = (p_ptr->health_who == c_ptr->m_idx) ? TRUE : FALSE;
	monster_type back_m;

	if (p_ptr->inside_arena || p_ptr->inside_battle) return (FALSE);

	/*:::�σp�����[�^�����X�^�[�͕ω����Ȃ��悤�ɂ��Ă���*/
	if(r_info[m_ptr->r_idx].flags7 & RF7_VARIABLE) return (FALSE);

	//v1.1.59 EASY/NORMAL�ł͕ϐg���Ȃ����Ƃɂ���
	if (difficulty == DIFFICULTY_EASY || difficulty == DIFFICULTY_NORMAL) return FALSE;


	if ((p_ptr->riding == c_ptr->m_idx) || (m_ptr->mflag2 & MFLAG2_KAGE)) return (FALSE);

	/* Memorize the monster before polymorphing */
	back_m = *m_ptr;

	/* Pick a "new" monster race */
	new_r_idx = poly_r_idx(old_r_idx);

	/* Handle polymorph */
	if (new_r_idx != old_r_idx)
	{
		u32b mode = 0L;
		bool preserve_hold_objects = back_m.hold_o_idx ? TRUE : FALSE;
		s16b this_o_idx, next_o_idx = 0;

		/* Get the monsters attitude */
		if (is_friendly(m_ptr)) mode |= PM_FORCE_FRIENDLY;
		if (is_pet(m_ptr)) mode |= PM_FORCE_PET;
		if (m_ptr->mflag2 & MFLAG2_NOPET) mode |= PM_NO_PET;

		/* Mega-hack -- ignore held objects */
		m_ptr->hold_o_idx = 0;

		/* "Kill" the "old" monster */
		delete_monster_idx(c_ptr->m_idx);

		/* Create a new monster (no groups) */
		if (place_monster_aux(0, y, x, new_r_idx, mode))
		{
			m_list[hack_m_idx_ii].nickname = back_m.nickname;
			m_list[hack_m_idx_ii].parent_m_idx = back_m.parent_m_idx;
			m_list[hack_m_idx_ii].hold_o_idx = back_m.hold_o_idx;

			/* Success */
			polymorphed = TRUE;
			set_deity_bias(DBIAS_COSMOS, -1);

		}
		else
		{
			/* Placing the new monster failed */
			if (place_monster_aux(0, y, x, old_r_idx, (mode | PM_NO_KAGE | PM_IGNORE_TERRAIN)))
			{
				m_list[hack_m_idx_ii] = back_m;

				/* Re-initialize monster process */
				mproc_init();
			}
			else preserve_hold_objects = FALSE;
		}

		/* Mega-hack -- preserve held objects */
		if (preserve_hold_objects)
		{
			for (this_o_idx = back_m.hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire object */
				object_type *o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Held by new monster */
				o_ptr->held_m_idx = hack_m_idx_ii;
			}
		}
		else if (back_m.hold_o_idx) /* Failed (paranoia) */
		{
			/* Delete objects */
			for (this_o_idx = back_m.hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire next object */
				next_o_idx = o_list[this_o_idx].next_o_idx;

				/* Delete the object */
				delete_object_idx(this_o_idx);
			}
		}

		if (targeted) target_who = hack_m_idx_ii;
		if (health_tracked) health_track(hack_m_idx_ii);
	}

	return polymorphed;
}






/*
 * Dimension Door
 */
///mod140527 ���샂�[�h��ݒ肷��悤�ɂ���
//����������TRUE,���s(�ςȏ��ɔ�񂾂̂��܂�)��FALSE��Ԃ�
static bool dimension_door_aux(int x, int y, int mode)
{
	int	plev = p_ptr->lev;

	//v1.1.13 if/else�������������̂�switch�ɏ���������
	switch(mode)
	{
	case D_DOOR_SHADOW:
		if(!cave_player_teleportable_bold(y, x, 0L))
		{
			msg_print("�����ɂ����ďo���Ȃ������B");
			return TRUE;
		}
		else if(distance(y, x, py, px) > plev )
		{
			msg_print("���܂��q����Ȃ������B��������悤���B");
			return TRUE;
		}
		else
		{
			teleport_player_to(y, x, 0L);
			return TRUE;
		}
		break;

	case D_DOOR_BACKDOOR:
		{
			int i,tx,ty;
			int cnt = 0;
			monster_type *m_ptr;
			char m_name[80];

			if (!cave[y][x].m_idx)
			{
				msg_print("�����ɂ͔����Ȃ��B");
				return FALSE;
			}

			m_ptr = &m_list[cave[y][x].m_idx];
			monster_desc(m_name, m_ptr, 0L);

			for (i = 0; i < 8; i++)
			{
				int yy = y + ddy_ddd[i];
				int xx = x + ddx_ddd[i];

				if (cave_player_teleportable_bold(yy, xx, 0L))
				{
					cnt++;
					if (one_in_(cnt))
					{
						tx = xx;
						ty = yy;

					}
				}
			}

			if (cnt)
			{
				msg_format("���Ȃ���%s�̔w���̔�����o���B",m_name);
				teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
				return TRUE;
			}
			else
			{
				msg_print("������o���Ȃ������B");
				return FALSE;
			}
		}
		break;


	case D_DOOR_MINI:
		if(!cave_player_teleportable_bold(y, x, 0L))
		{
			msg_print("�����ɂ͏o���Ȃ��B");
			return TRUE;
		}

		else
		{
			teleport_player_to(y, x, 0L);
			return TRUE;
		}
		break;
	case D_DOOR_FUTO:
		msg_print("���Ȃ��͐����ƂƂ��ɎM�̏�ɕ����~�肽�I");
		project(0,plev/16,y,x,plev*4,GF_TORNADO,(PROJECT_KILL | PROJECT_JUMP),-1);
		if(cave[y][x].m_idx)
			msg_print("�����������ړ��Ɏ��s�����B");
		teleport_player_to(y, x, TELEPORT_ANYPLACE);
		futo_break_plate(y,x);
		return TRUE;
		break;
	case D_DOOR_MURASA:
		{
			cave_type    *c_ptr = &cave[y][x];
			feature_type *f_ptr = &f_info[c_ptr->feat];

			if (c_ptr->m_idx && (c_ptr->m_idx != p_ptr->riding))
			{
				msg_print("�����ɂ͂��łɉ���������B");
				return TRUE;
			}
			else if(!have_flag(f_ptr->flags, FF_TELEPORTABLE) /* || (c_ptr->info & CAVE_ICKY)*/ || have_flag(f_ptr->flags, FF_HIT_TRAP))
			{
				msg_print("�����Ɏז�����ĐN���ł��Ȃ������B");
				return TRUE;
			}
			else
			{
				//�쐶���ō��ꂽ���n�`�ɓ����悤��ANYPLACE�ɂ��Ă��܂�
				teleport_player_to(y, x, TELEPORT_ANYPLACE);
				msg_print("���Ȃ��͐��̒�����o�������B");
				return TRUE;
			}

		}
		break;
	case D_DOOR_MIST:
		msg_print("���Ȃ��͖̑̂��ɂȂ�A����ɗn���Ă������E�E");
		p_ptr->energy_need += (s16b)ENERGY_NEED();
		if(!cave_player_teleportable_bold(y, x, 0L))
		{
			msg_print("���܂��_�����ꏊ�ɐN���ł��Ȃ������B");
			teleport_player((plev + 2) * 2, TELEPORT_PASSIVE);
			return FALSE;
		}

		else
		{
			msg_print("���Ȃ��͏������ꂽ�ꏊ�Ɏ��̉������B");
			teleport_player_to(y, x, 0L);
			return TRUE;
		}
		break;
	case D_DOOR_MACHINE:
		{
			//v1.1.97 �e���|�[�^�[�̎��s������ʊ֐��ɓ����B���ƒZ�����Ȃ班���\�����ɂ�������
			if(!cave_player_teleportable_bold(y, x, 0L) || randint1(100) + MAX_RANGE < distance(y,x,py,px)) 
			{
				msg_print("�e���|�[�^�[����쓮�����I");
				teleporter_trap();
			}
			else
			{
				if(one_in_(100))
				{
					msg_print("�e���|�[�^�[�u����H���̈�`�q���E�E");
					msg_print(NULL);
					msg_print("�����A�C�̂����ł��B������肠��܂���B�����E�E�v");
				}
				else msg_print("���Ȃ��͎w�肵�����W�Ƀe���|�[�g�����B");
				teleport_player_to(y, x, 0L);
			}
			return TRUE;

		}
		break;
	case D_DOOR_MOKOU:

		msg_print("���Ȃ��͓��̂��̂č������ɂȂ��Ĕ�񂾁E�E");
		if (!player_can_enter(cave[y][x].feat, 0) || !cave_player_teleportable_bold(y, x, TELEPORT_IGNORE_MON))
		{
			 msg_print("�_�����ꏊ�ɂ��܂��ړ��ł��Ȃ������B");
		}
		else if(cave[y][x].m_idx)
		{
			int dam = plev * 3 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			char m_name[80];	
			monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
			msg_format("%s�͓ˑR�������R���オ�����I",m_name);
			flag_mokou_possess = TRUE; 
			project(0,0,y,x,dam,GF_NUKE,(PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID),-1);
			flag_mokou_possess = FALSE; 
		}
		teleport_player_to(y, x, 0L);
		mokou_resurrection();
		return TRUE;
		break;
	case D_DOOR_DREAM:
		msg_print("���Ȃ��͖��̐��E���ЂƂ���тɔ�񂾁I");
		if (!player_can_enter(cave[y][x].feat, 0) || !cave_player_teleportable_bold(y, x, 0L))
		{
			 msg_print("�_�����ꏊ�ɂ��܂��ړ��ł��Ȃ������B");
		}
		teleport_player_to(y, x, 0L);
		return TRUE;
	case D_DOOR_YUKARI:
		msg_print("���Ȃ��͉������ꂽ�ꏊ�ւƃX�L�}���J�����B");
		if (!player_can_enter(cave[y][x].feat, 0) || !cave_player_teleportable_bold(y, x, 0L))
		{
			 msg_print("�_�����ꏊ�ɂ��܂��q����Ȃ������B");
		}
		teleport_player_to(y, x, 0L);
		return TRUE;
		break;

	case D_DOOR_PATTERN:
		msg_print("���Ȃ��͉������ꂽ�ꏊ�ւƓ]�ڂ����B");
		if (!player_can_enter(cave[y][x].feat, 0) || !cave_player_teleportable_bold(y, x, 0L))
		{
			 msg_print("�_�����ꏊ�ɂ��܂��q����Ȃ������B");
		}
		teleport_player_to(y, x, 0L);
		return TRUE;
		break;
	case D_DOOR_TOYOHIME:
	case D_DOOR_CHOSEN_DIST:
		{
			int dist = distance(y,x,py,px);
			int range = p_ptr->lev;

			if(mode == D_DOOR_CHOSEN_DIST) range = dimension_door_distance;

			if(randint1(dist) > range)
			{
				teleport_player(100, TELEPORT_PASSIVE);
				return FALSE;
			}
			else
			{
				teleport_player_to(y, x, 0L);
				return TRUE;
			}
		}
		break;

	case D_DOOR_NORMAL:
	case D_DOOR_DRAGON://v1.1.68 ����Y��Ă��̂Œǉ�

		if(mode == D_DOOR_NORMAL)
			p_ptr->energy_need += (s16b)((s32b)(60 - plev) * ENERGY_NEED() / 100L);

		if (!cave_player_teleportable_bold(y, x, 0L) ||
	    (distance(y, x, py, px) > plev / 2 + 10) ||
	    (!randint0(plev / 10 + 10)))
		{
			if (mode == D_DOOR_NORMAL)
				p_ptr->energy_need += (s16b)((s32b)(60 - plev) * ENERGY_NEED() / 100L);
			teleport_player((plev + 2) * 2, TELEPORT_PASSIVE);
			return FALSE;
		}
		else
		{
			teleport_player_to(y, x, 0L);
			return TRUE;
		}
		break;

	default:
		msg_format("ERROR:dimension_door_aux()�̓��샂�[�h[%d]���K�肳��Ă��Ȃ�",mode);
		return FALSE;
	}

	msg_print("WARNING:dimension_door_aux�̃R���g���[���p�X����`����Ă��Ȃ�");
	return TRUE;

}


///mod140527 ���샂�[�h��ݒ肷��悤�ɂ���
/*:::�����̔��@�s���������Ƃ�TRUE��Ԃ�*/
/*
 * Dimension Door
 */
bool dimension_door( int mode)
{
	int x = 0, y = 0;

	if(QRKDR)
	{
		msg_print("�����ł��������͔̂������B");
		return FALSE;
	}


	/* Rerutn FALSE if cancelled */
	if (!tgt_pt(&x, &y)) return FALSE;

	if(mode == D_DOOR_MINI && distance(y,x,py,px) > 3)
	{
		msg_print("���Ȃ��̔\�͂ł͂����͉�������B");
		return FALSE;
	}
	if(mode == D_DOOR_FUTO)
	{
		if(!is_plate_grid(&cave[y][x]))
		{
			msg_print("�����ɂ͎M���Ȃ��B");
			return FALSE;
		}

		if(distance(y,x,py,px) > p_ptr->lev)
		{
			msg_print("�����͉������Ă��܂���ׂȂ��B");
			return FALSE;
		}
	}

	if( (mode == D_DOOR_SHADOW) && cave[y][x].info & CAVE_GLOW)
	{
		msg_print("���܂��Q�[�g���q����Ȃ������B�����͖��邷����悤���B");
		return TRUE;
	}
	if( (mode == D_DOOR_MIST) && !player_has_los_bold(y, x))
	{
		msg_print("�����Ȃ��ꏊ�ɂ͈ړ��ł��Ȃ��B");
		return FALSE;
	}
	if( (mode == D_DOOR_MURASA) && !cave_have_flag_bold((y), (x), FF_WATER))
	{
		msg_print("�����ɂ͐����Ȃ��悤���B");
		return FALSE;
	}
	if (dimension_door_aux(x, y, mode)) return TRUE;

	if(mode == D_DOOR_NORMAL) msg_print("����E���畨���E�ɖ߂鎞���܂������Ȃ������I");
	else if(mode == D_DOOR_TOYOHIME) msg_print("�����q���̂Ɏ��s�����I");
	else if(mode == D_DOOR_CHOSEN_DIST) msg_print("���s�I");
	else if(mode == D_DOOR_DRAGON) msg_print("�e���|�[�g�Ɏ��s�����B");
	else if(mode == D_DOOR_MIST) msg_print("�ʂ̏ꏊ�Ɏ��̉������B");
	else if(mode == D_DOOR_MACHINE) msg_print("�@�B����쓮�����悤���B");

	return TRUE;
}


/*
 * Mirror Master's Dimension Door
 */
bool mirror_tunnel(void)
{
	int x = 0, y = 0;

	/* Rerutn FALSE if cancelled */
	if (!tgt_pt(&x, &y)) return FALSE;

	if (dimension_door_aux(x, y,FALSE)) return TRUE;

#ifdef JP
	msg_print("���̐��E�����܂��ʂ�Ȃ������I");
#else
	msg_print("You fail to pass the mirror plane correctly!");
#endif

	return TRUE;
}

/*:::���C�W�̖��͐H��*/
bool eat_magic(int power)
{
	object_type * o_ptr;
	object_kind *k_ptr;
	int lev, item;
	int recharge_strength = 0;

	bool fail = FALSE;
	byte fail_type = 1;

	cptr q, s;
	char o_name[MAX_NLEN];

	item_tester_hook = item_tester_hook_staff_rod_wand;

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e�����疂�͂��z�����܂����H";
s = "���͂��z���ł���A�C�e��������܂���B";
#else
	q = "Drain which item? ";
	s = "You have nothing to drain.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}

	k_ptr = &k_info[o_ptr->k_idx];
	lev = k_info[o_ptr->k_idx].level;

	if (o_ptr->tval == TV_ROD)
	{
		recharge_strength = ((power > lev/2) ? (power - lev/2) : 0) / 5;

		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}
		else
		{
			if (o_ptr->timeout > (o_ptr->number - 1) * k_ptr->pval)
			{
#ifdef JP
msg_print("�[�U���̃��b�h���疂�͂��z�����邱�Ƃ͂ł��܂���B");
#else
				msg_print("You can't absorb energy from a discharged rod.");
#endif

			}
			else
			{
				p_ptr->csp += lev;
				o_ptr->timeout += k_ptr->pval;
			}
		}
	}
	else
	{
		/* All staffs, wands. */
		recharge_strength = (100 + power - lev) / 15;

		/* Paranoia */
		if (recharge_strength < 0) recharge_strength = 0;

		/* Back-fire */
		if (one_in_(recharge_strength))
		{
			/* Activate the failure code. */
			fail = TRUE;
		}
		else
		{
			if (o_ptr->pval > 0)
			{
				p_ptr->csp += lev / 2;
				o_ptr->pval --;

				/* XXX Hack -- unstack if necessary */
				if ((o_ptr->tval == TV_STAFF) && (item >= 0) && (o_ptr->number > 1))
				{
					object_type forge;
					object_type *q_ptr;

					/* Get local object */
					q_ptr = &forge;

					/* Obtain a local object */
					object_copy(q_ptr, o_ptr);

					/* Modify quantity */
					q_ptr->number = 1;

					/* Restore the charges */
					o_ptr->pval++;

					/* Unstack the used item */
					o_ptr->number--;
					p_ptr->total_weight -= q_ptr->weight;
					item = inven_carry(q_ptr);

					/* Message */
#ifdef JP
					msg_print("����܂Ƃ߂Ȃ������B");
#else
					msg_print("You unstack your staff.");
#endif

				}
			}
			else
			{
#ifdef JP
msg_print("�z���ł��閂�͂�����܂���I");
#else
				msg_print("There's no energy there to absorb!");
#endif

			}
			if (!o_ptr->pval) o_ptr->ident |= IDENT_EMPTY;
		}
	}

	/* Inflict the penalties for failing a recharge. */
	if (fail)
	{
		/* Artifacts are never destroyed. */
		if (object_is_fixed_artifact(o_ptr))
		{
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("���͂��t�������I%s�͊��S�ɖ��͂��������B", o_name);
#else
			msg_format("The recharging backfires - %s is completely drained!", o_name);
#endif


			/* Artifact rods. */
			if (o_ptr->tval == TV_ROD)
				o_ptr->timeout = k_ptr->pval * o_ptr->number;

			/* Artifact wands and staffs. */
			else if ((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_STAFF))
				o_ptr->pval = 0;
		}
		else
		{
			/* Get the object description */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/*** Determine Seriousness of Failure ***/

			/* Mages recharge objects more safely. */
			if (cp_ptr->magicmaster)
			{
				/* 10% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD)
				{
					if (one_in_(10)) fail_type = 2;
					else fail_type = 1;
				}
				/* 75% chance to blow up one wand, otherwise draining. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (!one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 50% chance to blow up one staff, otherwise no effect. */
				else if (o_ptr->tval == TV_STAFF)
				{
					if (one_in_(2)) fail_type = 2;
					else fail_type = 0;
				}
			}

			/* All other classes get no special favors. */
			else
			{
				/* 33% chance to blow up one rod, otherwise draining. */
				if (o_ptr->tval == TV_ROD)
				{
					if (one_in_(3)) fail_type = 2;
					else fail_type = 1;
				}
				/* 20% chance of the entire stack, else destroy one wand. */
				else if (o_ptr->tval == TV_WAND)
				{
					if (one_in_(5)) fail_type = 3;
					else fail_type = 2;
				}
				/* Blow up one staff. */
				else if (o_ptr->tval == TV_STAFF)
				{
					fail_type = 2;
				}
			}

			/*** Apply draining and destruction. ***/

			/* Drain object or stack of objects. */
			if (fail_type == 1)
			{
				if (o_ptr->tval == TV_ROD)
				{
#ifdef JP
msg_print("���b�h�͔j����Ƃꂽ���A���͂͑S�Ď��Ȃ�ꂽ�B");
#else
					msg_format("You save your rod from destruction, but all charges are lost.", o_name);
#endif

					o_ptr->timeout = k_ptr->pval * o_ptr->number;
				}
				else if (o_ptr->tval == TV_WAND)
				{
#ifdef JP
msg_format("%s�͔j����Ƃꂽ���A���͂��S�Ď���ꂽ�B", o_name);
#else
					msg_format("You save your %s from destruction, but all charges are lost.", o_name);
#endif

					o_ptr->pval = 0;
				}
				/* Staffs aren't drained. */
			}

			/* Destroy an object or one in a stack of objects. */
			if (fail_type == 2)
			{
				if (o_ptr->number > 1)
				{
#ifdef JP
msg_format("���\�Ȗ��@�̂��߂�%s����{��ꂽ�I", o_name);
#else
					msg_format("Wild magic consumes one of your %s!", o_name);
#endif

					/* Reduce rod stack maximum timeout, drain wands. */
					if (o_ptr->tval == TV_ROD) o_ptr->timeout = MIN(o_ptr->timeout, k_ptr->pval * (o_ptr->number - 1));
					else if (o_ptr->tval == TV_WAND) o_ptr->pval = o_ptr->pval * (o_ptr->number - 1) / o_ptr->number;

				}
				else
#ifdef JP
msg_format("���\�Ȗ��@�̂��߂�%s�����{����ꂽ�I", o_name);
#else
					msg_format("Wild magic consumes your %s!", o_name);
#endif

				/* Reduce and describe inventory */
				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}

			/* Destroy all members of a stack of objects. */
			if (fail_type == 3)
			{
				if (o_ptr->number > 1)
#ifdef JP
msg_format("���\�Ȗ��@�̂��߂�%s���S�ĉ�ꂽ�I", o_name);
#else
					msg_format("Wild magic consumes all your %s!", o_name);
#endif

				else
#ifdef JP
msg_format("���\�Ȗ��@�̂��߂�%s����ꂽ�I", o_name);
#else
					msg_format("Wild magic consumes your %s!", o_name);
#endif



				/* Reduce and describe inventory */
				if (item >= 0)
				{
					inven_item_increase(item, -999);
					inven_item_describe(item);
					inven_item_optimize(item);
				}

				/* Reduce and describe floor item */
				else
				{
					floor_item_increase(0 - item, -999);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);
				}
			}
		}
	}

	if (p_ptr->csp > p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
	}

	/* Redraw mana and hp */
	p_ptr->redraw |= (PR_MANA);

	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	p_ptr->window |= (PW_INVEN);

	return TRUE;
}

///race ���R�����@�V���{���w��v
///mod141011 �F�X�ύX
bool summon_kin_player(int level, int y, int x, u32b mode)
{
	bool pet = (bool)(mode & PM_FORCE_PET);
	if (!pet) mode |= PM_NO_PET;

	//�ꕔ���ꏈ��
	if(prace_is_(RACE_DEATH)) return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_DEATH, mode);
	if(prace_is_(RACE_DAIYOUKAI)) return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_KWAI, mode);
	if(prace_is_(RACE_WARBEAST)) return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_ANIMAL, mode);
	if(prace_is_(RACE_BAKEDANUKI)) return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_TANUKI, mode);
	if (prace_is_(RACE_HANIWA)) return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_HANIWA, mode);
	if(p_ptr->mimic_form == MIMIC_MARISA || p_ptr->pclass == CLASS_PATCHOULI) return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_LIB_GOLEM, mode);


	if(prace_is_(RACE_GYOKUTO)) return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_GYOKUTO, mode);


	switch (p_ptr->mimic_form)
	{
	case MIMIC_NONE:

		//v1.1.65 �~�������ŏo�Ă���V���{���́u���̐��E�́��v�����_�����j�[�N�̃V���{���Ɠ����ɂ���
		summon_kin_type = rp_ptr->random_unique_symbol;

		break;
	case MIMIC_DEMON:
		if (one_in_(13)) summon_kin_type = 'U';
		else summon_kin_type = 'u';
		break;
	case MIMIC_DEMON_LORD:
		summon_kin_type = 'U';
		break;
	case MIMIC_VAMPIRE:
		summon_kin_type = 'V';
		break;
	case MIMIC_NUE:
		summon_kin_type = 'H';
		break;

	case MIMIC_METAMORPHOSE_NORMAL:
	case MIMIC_METAMORPHOSE_MONST:
	case MIMIC_METAMORPHOSE_GIGANTIC:
		summon_kin_type = r_info[MON_EXTRA_FIELD].d_char;
		break;


	default:
		summon_kin_type = 'p';
		break;
	}	

	return summon_specific((pet ? -1 : 0), y, x, level, SUMMON_KIN, mode);
}

///mod140626
/*:::�얲�i�G�j�ɂ�閲�z����@���E���̓G�ΓI���݁i���܂ށj�ɑ΂������_����DISP_ALL�����̃{�[���𐔔�����*/
/*:::EX�_���W�����ȂǂŎ��̎��̌����ɂȂ肩�˂Ȃ��̂ŗ얲�̏㉺���E�ǂꂩ�ɕǂ�����Ǝg��Ȃ����Ƃɂ���B*/
void cast_musou_hu_in(int reimu_idx)
{
	monster_type *reimu_m_ptr = &m_list[reimu_idx];
	int num = MAX(1,dun_level / 10);
	int dice = 10;
	int sides = dun_level / 8;
	int j;
	int ry = reimu_m_ptr->fy;
	int rx = reimu_m_ptr->fx;

	if(p_ptr->inside_battle) num = battle_mon_base_level / 10;

	for(;num > 0;num--)
	{
		int cnt = 0;
		int tx,ty;

		if(projectable(py,px,ry,rx) && is_hostile(reimu_m_ptr))
		{
			cnt++;
			tx = px;
			ty = py;
		}

		for(j = 1; j < m_max; j++)
		{
			monster_type *tmp_m_ptr = &m_list[j];
			if (!tmp_m_ptr->r_idx) continue;
			if (tmp_m_ptr->r_idx == MON_REIMU) continue;
			if(!are_enemies(reimu_m_ptr,tmp_m_ptr)) continue;
			if(!projectable(ry, rx, tmp_m_ptr->fy,tmp_m_ptr->fx))continue;
			cnt++;
			if(!one_in_(cnt))continue;
			tx = tmp_m_ptr->fx;
			ty = tmp_m_ptr->fy;
		}
		if(!cnt) break;
		project(reimu_idx,1,ty,tx,damroll(dice,sides),GF_MISSILE, (PROJECT_KILL | PROJECT_PLAYER),-1);
	}

}

/*:::ident_spell���قڃR�s�[�@����ȊӒ�*/
/*:::mode1:�L�m�R*/
/*:::mode2:�L�m�R�Ɩ�*/
/*:::mode3:�T�O���̊Ӓ�̊����@�A�C�e�������Ӓ�ɖ߂�*/
//mode4:����
bool ident_spell_2(int mode)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	cptr            q, s;
	bool old_known;

	item_tester_no_ryoute = TRUE;

	if (mode == 1)
		item_tester_hook = item_tester_hook_identify_kinoko;
	else if(mode == 2)
		item_tester_hook = item_tester_hook_identify_kinoko_potion;
	else if(mode == 3)
		item_tester_hook = item_tester_hook_identify_sagume;
	else if(mode == 4)
		item_tester_hook = item_tester_hook_identify_scroll;

	else
		item_tester_hook = item_tester_hook_identify;

	if (can_get_item())
	{
		if(mode == 1) q = "�ǂ̃L�m�R���Ӓ肵�܂���? ";
		else if(mode == 2) q = "�ǂ̃A�C�e���𒲂ׂ܂���? ";
		else if(mode == 4) q = "�ǂ̊����𒲂ׂ܂���? ";
		else q = "�ǂ̃A�C�e�����Ӓ肵�܂���? ";
	}
	else
	{
		//item_tester_hook = NULL;
		q = "���ׂĊӒ�ς݂ł��B ";
	}

	/* Get an item */
	s = "�Ӓ�ł���A�C�e�����Ȃ��B";

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_print("������ɂ͐G��Ȃ��B");
			return FALSE;
		}
	}
	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	//�T�O����p�@�A�C�e���𖢊Ӓ�ɖ߂�
	if(mode == 3)
	{
		o_ptr->ident &= ~(IDENT_KNOWN | IDENT_MENTAL | IDENT_SENSE);

		o_ptr->feeling = FEEL_NONE;
		o_ptr->marked |= OM_TOUCHED;
		p_ptr->update |= (PU_BONUS);
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
		return TRUE;
	}

	/* Identify it */
	old_known = identify_item(o_ptr);

	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
	if(mode == 1)
	{
		msg_format("�ނށA���̃L�m�R�́c%s���I", o_name);
		if(item < 0) fix_floor_item_list(py,px);
	}
	else
	{
		if (item >= INVEN_RARM)
		{
			msg_format("%^s: %s(%c)�B", describe_use(item), o_name, index_to_label(item));
		}
		else if (item >= 0)
		{
			msg_format("�U�b�N��: %s(%c)�B", o_name, index_to_label(item));
		}
		else
		{
			msg_format("����: %s�B", o_name);
			///mod140428 @�̑����̃A�C�e�����Ӓ肳�ꂽ�Ƃ������A�C�e���ꗗ���X�V
			fix_floor_item_list(py,px);
		}
	}

	/* Auto-inscription/destroy */
	autopick_alter_item(item, (bool)(destroy_identify && !old_known));

	/* Something happened */
	return (TRUE);
}

//�i�ԃ��C�W���O�Q�[���Ə��ς̏�����p�@�t���A�̂��ׂĂ̐����𑝐B������
//�T�O�������E�̊�����ǂ�ł��N���邱�Ƃɂ���
void raising_game(void)
{
	monster_type *m_ptr;
	monster_race *r_ptr;
	char m_name[80];
	int old_m_max;
	int i;

	compact_monsters(0);

	if(m_cnt + 64 >= max_m_idx)
	{
		msg_print("���łɂ��̃t���A�ɂ͑����̐����̑������Ђ��߂��Ă���B");
		return;
	}

	old_m_max = m_max;

	for (i = 1; i < old_m_max; i++)
	{
		bool seen = FALSE;
		u32b mode = 0L;

		m_ptr = &m_list[i];
		if (!m_ptr->r_idx) continue;

		r_ptr = &r_info[m_ptr->r_idx];
		if(!monster_living(r_ptr)) continue;
		if(r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & (RF7_UNIQUE2 | RF7_VARIABLE)) continue;

		if(is_pet(m_ptr)) mode = PM_FORCE_PET;
		else if(is_friendly(m_ptr)) mode = PM_FORCE_FRIENDLY;

		if(is_seen(m_ptr)) 
		{
			seen = TRUE;
			monster_desc(m_name, m_ptr, 0L);
		}
		if (multiply_monster(i,TRUE,mode))
		{
			if(seen)  msg_format("%^s�����B�����I", m_name);
	
		}
		if(m_cnt + 64 >= max_m_idx) return;
	}


}

//����(�Ɏ��߂����׋S)���t���A�̕t�r�_���z������B�G�ł����ł��g����悤�ɂ��Ă���
void absorb_tsukumo(int m_idx)
{
	int j;
	monster_type *m_ptr;
	int cnt = 0;
	char m_name[80];

	for (j = 1; j < m_max; j++)
	{
		m_ptr = &m_list[j];
		if (m_ptr->r_idx != MON_TSUKUMO_1 && m_ptr->r_idx != MON_TSUKUMO_2 && m_ptr->r_idx != MON_TSUKUMO_3 ) continue;
		if(!m_idx && !projectable(py,px,m_ptr->fy,m_ptr->fx)) continue;
		cnt++;
		delete_monster_idx(j);
	}
	if(!cnt)
	{
		msg_print("�����������N����Ȃ������B");
	}
	else if(m_idx)
	{

		m_ptr = &m_list[m_idx];
		m_ptr->hp += cnt * 200;
		if (m_ptr->hp >= m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
		if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
		if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

	}
	else if(!m_idx)
	{
		int sides = 200;
		msg_format("���͂̕t�r�_%s���z�������I",cnt==1?"":"����");
		hp_player(damroll(cnt,sides));
	}
	else
		msg_print("ERROR:absorb_tsukumo()�ɕs����m_idx���n���ꂽ");

}


//v1.1.43 �N���E���s�[�X�́u���C�̏����v�����B�����X�^�[��̂ɓ���t���O�t�^����B����������TRUE��Ԃ��B
bool lunatic_torch(int m_idx, int power)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr;
	char m_name[80];

	if (!m_ptr->r_idx) { msg_format("ERROR:lunatic_torch()�ɕs����m_idx(%d)���n���ꂽ", m_idx); return FALSE; }

	r_ptr = &r_info[m_ptr->r_idx];
	monster_desc(m_name, m_ptr, 0);

	//�܂��N����
	set_monster_csleep(m_idx, 0);

	if(m_ptr->mflag & MFLAG_LUNATIC_TORCH)
	{
		msg_format("%s�ɂ͂��łɌ����Ă���B", m_name);
		return FALSE;
	}

	//���j�[�N�ɂ͌����Â炢
	if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flags7 & (RF7_UNIQUE2)))
		power /= 2;

	//���C�A���S�A�N�G�X�g�����ΏہA�͂����^�A����l�l�A�z���ɂ͌����Ȃ�
	if ((r_ptr->flags2 & (RF2_ELDRITCH_HORROR | RF2_EMPTY_MIND | RF2_WEIRD_MIND))
		|| (r_ptr->flags1 & RF1_QUESTOR)
		|| (r_ptr->flagsr & RFR_RES_ALL)
		|| (m_ptr->r_idx >= MON_HECATIA1 && m_ptr->r_idx <= MON_HECATIA3)
		|| is_pet(m_ptr)
		|| r_ptr->level >= power)
	{
		msg_format("%s�ɂ͌��ʂ��Ȃ������I", m_name);
		return FALSE;
	}


	//����ʂ������p�t���O�t�^�B�F�D�����X�^�[�͓G�΂ɂ���B
	if (randint1(power) > r_ptr->level)
	{
		if (one_in_(2))
			msg_format("*�s�L���[���I*");
		else if (one_in_(2))
			msg_format("%s�͋��C�ɖ`���ꂽ�I", m_name);
		else
			msg_format("%s�͓ˑR�\��n�߂��I", m_name);

		m_ptr->mflag |= MFLAG_LUNATIC_TORCH;
		set_hostile(m_ptr);
		return TRUE;

	}
	else
	{
		msg_format("%s�͋��C�ɖ`����Ȃ������B", m_name);
		return FALSE;
	}

}





//v1.1.57 �B��ށu��_�̈Ö��e���v����g���B�p�^�[���Ɋ����ē��萨�͂̐������Ƀ_���[�W�𕉂킹��B
//PATTERN_ATTACK_TYPE_***��p_ptr->magic_num2[0]�ɋL�^���A�����X�^�[��������HP��������(�w�蕉���̌F�x�݂�����)���s���B
//�s�������Ƃ�TRUE��Ԃ��B
bool pattern_attack(void)
{

	int new_attack_type;
	int cur_attack_type = p_ptr->magic_num2[0];
	int i;
	char c;
	int menu_line;

	if (p_ptr->pclass != CLASS_OKINA) { msg_print("ERROR:�z�肳��Ă��Ȃ��N���X��pattern_attack()���Ă΂ꂽ"); return FALSE; }
	if (cur_attack_type < 0 || cur_attack_type > PATTERN_ATTACK_TYPE_MAX) { msg_format("ERROR:magic_num2[0]�̒l����������(%d)", cur_attack_type); return FALSE; }

	if (!cur_attack_type)
		msg_format("���݂͂ǂ��̐��͂ɑ΂��Ă��U�����d�|���Ă��Ȃ��B");
	else
		msg_format("���݁u%s�v�ɑ΂���U�������s�����B", pattern_attack_table[cur_attack_type].desc);


	//�p�^�[���̒��S(�p�^�[���e���|�[�g�\�ȃO���b�h)�łȂ��Ǝg���Ȃ�
	if (!pattern_tile(py, px) || f_info[cave[py][px].feat].subtype != PATTERN_TILE_TELEPORT)
	{
		msg_format("�u�p�^�[���v�̒��S�ɍs���Ȃ��ƍU���̑Ώۂ�I�Ԃ��Ƃ��ł��Ȃ��B");
		if(!p_ptr->wizard) return FALSE;
	}

	//�U���ΏۑI�����j���[��`��
	screen_save();
	menu_line = 1;
	Term_erase(17, menu_line++, 255);
	Term_erase(17, menu_line, 255);
	prt("�ǂ̐��͂ɍU�����d�|���܂����H(ESC:�L�����Z��)", menu_line++, 20);
	for (i = 0;; i++)
	{
		if (i > PATTERN_ATTACK_TYPE_MAX || pattern_attack_table[i].lev > p_ptr->lev) break;
		Term_erase(17, menu_line, 255);
		prt(format("%c) %s", ('a' + i), pattern_attack_table[i].desc), menu_line++, 25);
	}
	Term_erase(17, menu_line, 255);

	//���͎�t���ƑI���\����
	while (1)
	{
		c = inkey();
		new_attack_type = c - 'a';
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}

		if (new_attack_type >= 0 && new_attack_type <= PATTERN_ATTACK_TYPE_MAX && pattern_attack_table[new_attack_type].lev <= p_ptr->lev)
		{
			break;
		}
	}
	screen_load();

	if (!new_attack_type)
		msg_print("���Ȃ��́q�p�^�[���r�ɑ΂��銱���~�߂��B");
	else
		msg_format("���Ȃ��́q�p�^�[���r�Ɋ���%s�֍U�����J�n�����I", pattern_attack_table[new_attack_type].desc);

	p_ptr->magic_num2[0] = new_attack_type;

	return TRUE;

}

//v1.1.59
//�������w�肵�A������̎w�苗���n�_�փe���|�[�g����B�e���|�[�g�Ɏ��s��������ӂ̃e���|�[�g�\�n�_��T���B
//�s�����������TRUE��Ԃ��B
//俎q�ƈ���俎q�ŏd������̂ň�{�������B
bool teleport_to_specific_dir(int dist, int retry_chance, int mode)
{
	int y, x;
	int i;
	int dir;

	if (p_ptr->anti_tele)
	{
		msg_format("���̓e���|�[�g�ł��Ȃ��B");
		return FALSE;
	}

	if (!get_rep_dir2(&dir)) return FALSE;
	if (dir == 5) return FALSE;

	y = py + ddy[dir] * dist;
	x = px + ddx[dir] * dist;

	if (!in_bounds(y, x))
	{
		msg_print("�e���|�[�g�Ɏ��s�����I");
	}
	else if (cave_player_teleportable_bold(y, x, 0L))
	{
		msg_print("���Ȃ��͏u���ɏ������B");
		teleport_player_to(y, x, 0L);
	}
	else
	{
		int xx, yy;
		for (i = retry_chance; i>0; i--)
		{
			xx = x + randint1(dist / 2);
			yy = y + randint1(dist / 2);

			if (!in_bounds(yy, xx)) continue;
			if (cave_player_teleportable_bold(yy, xx, 0L)) break;
		}
		if (i)
		{
			msg_print("�e���|�[�g�ɂ�����Ǝ��s�����B");
			teleport_player_to(yy, xx, 0L);
		}
		else
		{
			msg_print("�e���|�[�g�Ɏ��s�����I");
		}
	}
	return TRUE;
}


//v1.1.59 �T�j�[���ǂꂭ�炢������~�ς��Ă��邩�̃����N�����߂�@�p�����[�^�t�^����ȂǂɎg��
int check_sunny_sunlight_state(int charge)
{

	if (p_ptr->pclass != CLASS_3_FAIRIES && p_ptr->pclass != CLASS_SUNNY) return 0;

	if (!charge)
		return 0;
	if (charge < SUNNY_CHARGE_SUNLIGHT_1)
		return 1;
	if (charge < SUNNY_CHARGE_SUNLIGHT_2)
		return 2;
	if (charge < SUNNY_CHARGE_SUNLIGHT_3)
		return 3;
	if (charge < SUNNY_CHARGE_SUNLIGHT_MAX)
		return 4;

	return 5;
}

//v1.1.59
//�T�j�[�~���N��������~��/�����Bamt�͒~�ϗ�/�����
//���j�[�N�N���X�u�T�j�[�~���N�v�u���̎O�d���v�̂Ƃ��Ă΂��
//�~�ς��������|�C���g(1�Q�[���^�[����1�~��)��p_ptr->magic_num1[0]�ɋL�^����
void sunny_charge_sunlight(int amt)
{

	int cur_state, new_state, cur_charge, new_charge;
	if (p_ptr->pclass != CLASS_3_FAIRIES && p_ptr->pclass != CLASS_SUNNY) return;
	if (!amt) return;

	cur_charge = p_ptr->magic_num1[0];

	if (!cur_charge && amt < 0 || cur_charge == SUNNY_CHARGE_SUNLIGHT_MAX && amt > 0) return;


	//���݂ǂ̂��炢���������܂��Ă��邩�̃����N
	cur_state = check_sunny_sunlight_state(cur_charge);

	//�l�����Ə���������Ēl���X�V
	new_charge = cur_charge + amt;
	if (new_charge < 0) new_charge = 0;
	if (new_charge > SUNNY_CHARGE_SUNLIGHT_MAX) new_charge = SUNNY_CHARGE_SUNLIGHT_MAX;
	p_ptr->magic_num1[0] = new_charge;

	new_state = check_sunny_sunlight_state(new_charge);

		//�����N���܂����ő��������Ƃ����b�Z�[�W�ƃp�����[�^�Čv�Z�t���O����	
		if (cur_state != new_state)
		{
			cptr myself;

			if (p_ptr->pclass == CLASS_SUNNY)
				myself = "���Ȃ�";
			else
				myself = "�T�j�[";

			if (amt > 0)
			{
				switch (new_state)
				{
				case 2:
					msg_format("%s�͏��������𗁂т��B", myself); break;
				case 3:
					msg_format("%s�͓����𗁂тđ̂ɒ~�ς����B", myself); break;
				case 4:
					msg_format("%s�͑����ɓ����������Č��C��t���I", myself); break;
				case 5:
					msg_format("%s�͌��E�܂œ����𒙂ߍ��񂾁I", myself); break;
				}
			}
			else
			{
				switch (new_state)
				{
				case 0:
					msg_format("%s�͑̂ɒ~�ς����������g���؂����B", myself); break;
				case 1:
					msg_format("%s�̑̂̌��͏��������ɂȂ��Ă���B", myself); break;
				case 2:
					msg_format("%s�̓����̒~�ς������Ă����悤���B", myself); break;
				case 3:
					msg_format("%s�̃e���V���������������Ă����B", myself); break;
				}

			}
			disturb(0, 1);
			p_ptr->update |= (PU_VIEW | PU_LITE | PU_TORCH | PU_BONUS);
		}

}

//v1.1.84 �G�i�W�[�h���C���@�אڂ����G����HP/MP���z��
//�푰���b�`�̃��C�V�������������������Ɏ��߂��ꂽ��Ԃ̂Ƃ��ɂ��g�����Ƃɂ����̂ŕʊ֐��ɕ���
//�s���������Ƃ�TRUE��Ԃ�
bool energy_drain(void)
{

	int y, x, dir;
	cave_type *c_ptr;

	/* Only works on adjacent monsters */
	if (!get_rep_dir(&dir, FALSE)) return FALSE;   /* was get_aim_dir */
	y = py + ddy[dir];
	x = px + ddx[dir];
	c_ptr = &cave[y][x];
	if (!c_ptr->m_idx)
	{
		msg_print("���Ȃ��̐L�΂�����͋��؂����B");
	}
	else
	{
		monster_type    *m_ptr = &m_list[c_ptr->m_idx];
		monster_race    *r_ptr = &r_info[m_ptr->r_idx];
		char            m_name[120];
		int fail = r_ptr->level;
		int dam = p_ptr->lev * 2;

		monster_desc(m_name, m_ptr, 0);
		if (!monster_living(r_ptr))
		{
			msg_format("%s�͐����͂������Ȃ��悤���B", m_name);
		}
		if (r_ptr->flags2 & RF2_POWERFUL) fail = fail * 3 / 2;
		if (r_ptr->flags1 & RF1_UNIQUE) fail *= 2;
		if (r_ptr->flags7 & RF7_CAN_FLY) fail = fail * 3 / 2;
		fail -= p_ptr->lev;
		if (fail < 0) fail = 0;
		if (randint0(100) < fail)
		{
			msg_format("%s�ɐG�낤�Ƃ��������킳�ꂽ�B", m_name);
		}
		else
		{
			if (m_ptr->hp + 1 < dam) dam = m_ptr->hp + 1;
			msg_format("%s����G�l���M�[���z��������I", m_name);
			player_gain_mana(dam);
			hp_player(dam);
			project(0, 0, y, x, dam, GF_OLD_DRAIN, PROJECT_HIDE | PROJECT_KILL, -1);
			touch_zap_player(m_ptr);
		}
	}

	return TRUE;
}


//v1.1.86 �ǔ����e���|
//�����Z�����łȂ��A�r���e�B�J�[�h�ł��ł���悤�ɂ��邽�߂ɕʊ֐��ɕ���
//��p�̌��ԏ��͂�����Ɠ��삪�Ⴄ�B���̂����������ׂ�����
//mode:�e���|�[�g���[�h�@�����ɔ����Ēl��������Ƃ�
//�s��������Ȃ��Ƃ�FALSE
bool wall_through_telepo(int mode)
{
	int x, y,dir;
	int attempt = 500;

	//8������������w��
	if (!get_rep_dir2(&dir)) return FALSE;
	if (dir == 5) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];

	if (cave_have_flag_bold(y, x, FF_MOVE) || cave_have_flag_bold(y, x, FF_CAN_FLY))
	{
		msg_format("�����ɂ͕ǂ��Ȃ��B");
		return FALSE;
	}
	//�w�肵�������𒲂ׁA�o���Ȃ��ꏊ�Ȃ炳��ɗׁA�}�b�v�[�ɒ������甽�Α��ɏo��B�o����ꏊ���Ȃ������烉���_���e���|
	while (1)
	{
		attempt--;
		y += ddy[dir];
		x += ddx[dir];

		if (y >= cur_hgt) y = 1;
		if (y < 1) y = cur_hgt - 1;
		if (x >= cur_wid) x = 1;
		if (x < 1) x = cur_wid - 1;

		if (attempt<0)
		{
			msg_print("���s�����I�K���ȏꏊ�ɋً}�����B");
			teleport_player(200, 0L);
			return TRUE;
		}
		if (!cave_player_teleportable_bold(y, x, 0L)) continue;
		msg_format("���Ȃ��͕ǂ�˂��������I");
		teleport_player_to(y, x, 0L);
		break;
	}

	return TRUE;


}


//v1.1.91
//�b�W�ޖ������͂̐Ζ��n�`�����݊����Ēn�`��ύX����B�����X�^�[���ʍs���Ɓ��̓��Z�����Ŏg���B
//�n�`�ύX��������Ԃ��B
//x,y:���S�n rad:�L�����a
//check:TRUE���Ǝ��͂̐Ζ��n�`�𐔂��邾���Œn�`��ύX���Ȃ��B���Z�g�p�\����ɁB
//rad:�L���͈� 0���ƂƂ肠����4��
int yuma_vacuum_oil(int y, int x, bool check, int rad)
{
	int oil_field_num = 0;

	int xx, yy;

	//rad��0�̂Ƃ�4�ɂ���B�����X�^�[�̖ޖ��֌W�͌ĂԂƂ�0�ɂ��Ă����Ő��l�ݒ肵�悤)
	if (rad < 1) rad = 3;

	//���͂̐Ζ��n�`�̐��𐔂���
	for (yy = y - 10; yy <= y + 10; yy++)for (xx = x - 10; xx <= x + 10; xx++)
	{
		if (!in_bounds(yy, xx)) continue;
		if (!cave_have_flag_bold(yy, xx, FF_OIL_FIELD)) continue;
		if (distance(yy, xx, y, x) > rad) continue;
		if (!projectable(yy, xx, y, x)) continue;
		oil_field_num++;

		if (check) continue;

		//check�łȂ��Ƃ����ۂɒn�`�ύX���s��
		cave_set_feat(yy, xx, (one_in_(2)?feat_dirt:feat_floor));

	}
	if (!check && cheat_xtra) msg_format("oil_field_num:%d",oil_field_num);
	return oil_field_num;

}




//v1.1.97 �����e���|�[�^�[㩂Ɉ������������Ƃ��̃e���|�[�g�����B�ǂ̒��ɏo�邱�Ƃ�����B
//���̃e���|�[�^�[㩂ƃG���W�j�A�@�B�̃e���|�[�^�[�̎��s�����Ŏg��
void teleporter_trap(void)
{
	cave_type    *c_ptr;
	feature_type *f_ptr;
	int i, x2, y2;

	int x = px, y = py;

	if (p_ptr->anti_tele)
	{
		msg_print("�������s�v�c�ȗ͂��e���|�[�g��h�����B");
		return;
	}

	///mod131228 ����ϐ��ɂ��e���|�A�E�F�C��R���������Ă݂��B
	if (	p_ptr->pclass == CLASS_KOMACHI && p_ptr->lev > 29
		||	IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags2 & RFR_RES_TELE
		||	p_ptr->resist_time && p_ptr->lev > randint1(100))
	{
		msg_print("���Ȃ��̓e���|�[�g�ɒ�R�����I");
		return;
	}

	for (i = 0; i<100; i++)
	{
		y2 = y + randint1(y) - randint1(y);
		x2 = x + randint1(x) - randint1(x);
		if (!in_bounds(y2, x2)) continue;
		if (cave[y2][x2].m_idx) continue;
		teleport_player_to(y2, x2, TELEPORT_ANYPLACE);
		break;
	}
	if (i>99) teleport_player((p_ptr->lev + 2) * 2, TELEPORT_PASSIVE);

	c_ptr = &cave[py][px];
	f_ptr = &f_info[c_ptr->feat];
	if (have_flag(f_ptr->flags, FF_LAVA)) msg_print("�n��̏�ɏo�Ă��܂����I");
	else if (have_flag(f_ptr->flags, FF_WALL)) msg_print("*�����̂Ȃ��ɂ���*");
	else if (have_flag(f_ptr->flags, FF_DOOR) && have_flag(f_ptr->flags, FF_CLOSE))msg_print("*�h�A�̂Ȃ��ɂ���*");

}



