/* File: spells1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spell projection */

#include "angband.h"

/* ToDo: Make this global */
/* 1/x chance of reducing stats (for elemental attacks) */
#define HURT_CHANCE 16


static int rakubadam_m;
static int rakubadam_p;

/*:::project()�̎˒��������f�t�H���g�l�ȊO�ɂ������ꍇ�͌ĂԑO�ɂ����ɒl�����Ă���*/
/*:::-1�ɂ�����˒��������Ȃ��Ȃ�炵��*/
int project_length = 0;

/*
 * Get another mirror. for SEEKER 
 */
static void next_mirror( int* next_y , int* next_x , int cury, int curx)
{
	int mirror_x[10],mirror_y[10]; /* ���͂����Ə��Ȃ� */
	int mirror_num=0;              /* ���̐� */
	int x,y;
	int num;

	for( x=0 ; x < cur_wid ; x++ )
	{
		for( y=0 ; y < cur_hgt ; y++ )
		{
			if( is_mirror_grid(&cave[y][x])){
				mirror_y[mirror_num]=y;
				mirror_x[mirror_num]=x;
				mirror_num++;
			}
		}
	}
	if( mirror_num )
	{
		num=randint0(mirror_num);
		*next_y=mirror_y[num];
		*next_x=mirror_x[num];
		return;
	}
	*next_y=cury+randint0(5)-2;
	*next_x=curx+randint0(5)-2;
	return;
}
		
/*
 * Get a legal "multi-hued" color for drawing "spells"
 */
static byte mh_attr(int max)
{
	switch (randint1(max))
	{
		case  1: return (TERM_RED);
		case  2: return (TERM_GREEN);
		case  3: return (TERM_BLUE);
		case  4: return (TERM_YELLOW);
		case  5: return (TERM_ORANGE);
		case  6: return (TERM_VIOLET);
		case  7: return (TERM_L_RED);
		case  8: return (TERM_L_GREEN);
		case  9: return (TERM_L_BLUE);
		case 10: return (TERM_UMBER);
		case 11: return (TERM_L_UMBER);
		case 12: return (TERM_SLATE);
		case 13: return (TERM_WHITE);
		case 14: return (TERM_L_WHITE);
		case 15: return (TERM_L_DARK);
	}

	return (TERM_WHITE);
}


/*
 * Return a color to use for the bolt/ball spells
 */
/*:::�����ɂ��F�����߂�@�߂�l��misc_to_attr[]��misc_to_char[]�ւ̃V�t�g��*/
byte spell_color(int type)
{
	/* Check if A.B.'s new graphics should be used (rr9) */
	if (streq(ANGBAND_GRAF, "new"))
	{
		/* Analyze */
		switch (type)
		{
			case GF_PSY_SPEAR:      return (0x06);
			case GF_MISSILE:        return (0x0F);
			case GF_ACID:           return (0x04);
			case GF_ELEC:           return (0x02);
			case GF_FIRE:           return (0x00);
			case GF_COLD:           return (0x01);
			case GF_DISP_LIVING:
			case GF_POIS:           return (0x03);
			case GF_HOLY_FIRE:      return (0x02);
			case GF_HELL_FIRE:      return (0x00);
			case GF_MANA:           return (0x0E);
			  /* by henkma */
			case GF_SEEKER:         return (0x0E);
			case GF_SUPER_RAY:      return (0x0E);

			case GF_ARROW:          return (0x0F);
			case GF_WATER:
			case GF_HOLY_WATER:
			case GF_MOSES:
			case GF_SPECIAL_SHOT:
				return (0x04);
			case GF_NETHER:         return (0x07);
			case GF_CHAOS:          return (mh_attr(15));
			case GF_DISENCHANT:     return (0x0F);
			case GF_NEXUS:          return (0x0C);
			case GF_CONFUSION:      return (mh_attr(4));
			case GF_SOUND:          return (0x09);
			case GF_SHARDS:         return (0x08);
			case GF_TIMED_SHARD:         return (0x08);

			case GF_FORCE:          return (0x09);
			case GF_INACT:        return (0x09);
			case GF_GRAVITY:        return (0x09);
			case GF_TIME:           return (0x09);
			case GF_LITE_WEAK:      return (0x06);
			case GF_LITE:           return (0x06);
			case GF_DARK_WEAK:      return (0x07);
			case GF_DARK:           return (0x07);
			case GF_PLASMA:         return (0x0B);
			case GF_METEOR:         return (0x00);
			case GF_KANAMEISHI:         return (0x00);
			case GF_ICE:            return (0x01);
			case GF_ROCKET:         return (0x0F);
			case GF_DEATH_RAY:      return (0x07);
			case GF_NUKE:           return (mh_attr(2));
			case GF_DISINTEGRATE:   return (0x05);
			case GF_PSI:
			case GF_PSI_DRAIN:
			case GF_TELEKINESIS:
			case GF_DOMINATION:
			case GF_DRAIN_MANA:
			case GF_MIND_BLAST:
			case GF_BRAIN_SMASH:
						return (0x09);
			case GF_CAUSE_1:
			case GF_CAUSE_2:
			case GF_CAUSE_3:
			case GF_CAUSE_4:        return (0x0E);
			case GF_HAND_DOOM:
			case GF_KYUTTOSHITEDOKA_N:
				return (0x07);
			case GF_CAPTURE  :      return (0x0E);
			case GF_IDENTIFY:       return (0x01);
			case GF_ATTACK:        return (0x0F);
			case GF_BANKI_BEAM:
			case GF_BANKI_BEAM2:
			case GF_PHOTO   :      return (0x06);
				///mod131229 �{���g�̋O�Ղ̐F�@�Ƃ肠�����K����
			case GF_WINDCUTTER:
			case GF_TRAIN:
				return (0x0D);
			case GF_REDEYE:
			case GF_SOULSCULPTURE:
			case GF_YOUMU:
			case GF_SATORI:
			case GF_POSSESSION:
			case GF_REDMAGIC:
			case GF_GUNGNIR:
			case GF_BLAZINGSTAR:
			case GF_PURIFY:
				return (0x00);
			case GF_RYUURI:     
			case GF_SEIRAN_BEAM:
				return (0x02);
			case GF_OLD_SLEEP:		
			case GF_NORTHERN:
				return (0x02);
			case GF_HOUTOU:
				return (0x09);
			case GF_YAKU:
				return (0x04);
			case GF_STEAM:
				return (0x01);
			default:return (0x01);

		}
	}
	/* Normal tiles or ASCII */
	else
	{
		byte a;
		char c;

		/* Lookup the default colors for this type */
		cptr s = quark_str(gf_color[type]);

		/* Oops */
		if (!s) return (TERM_WHITE);

		/* Pick a random color */
		c = s[randint0(strlen(s))];

		/* Lookup this color */
		a = my_strchr(color_char, c) - color_char;

		/* Invalid color (note check for < 0 removed, gave a silly
		 * warning because bytes are always >= 0 -- RG) */
		if (a > 15) return (TERM_WHITE);

		/* Use this color */
		return (a);
	}

	/* Standard "color" */
	return (TERM_WHITE);
}


/*
 * Find the attr/char pair to use for a spell effect
 *
 * It is moving (or has moved) from (x,y) to (nx,ny).
 *
 * If the distance is not "one", we (may) return "*".
 */
/*:::�{���g�����ł����Ƃ��̃G�t�F�N�g�Ɏg�������ƐF���Z�o
 *:::�Ԃ�l��16bit�ŁA���8bit���F�A����8bit������
 */
u16b bolt_pict(int y, int x, int ny, int nx, int typ)
{
	int base;

	byte k;

	byte a;
	char c;

	/* No motion (*) */
	if ((ny == y) && (nx == x)) base = 0x30;

	/* Vertical (|) */
	else if (nx == x) base = 0x40;

	/* Horizontal (-) */
	else if (ny == y) base = 0x50;

	/* Diagonal (/) */
	else if ((ny - y) == (x - nx)) base = 0x60;

	/* Diagonal (\) */
	else if ((ny - y) == (nx - x)) base = 0x70;

	/* Weird (*) */
	/*:::�r�[���Ȃ炱��*/
	else base = 0x30;

	/* Basic spell color */
	k = spell_color(typ);

	/* Obtain attr/char */
	a = misc_to_attr[base + k];
	c = misc_to_char[base + k];

	/* Create pict */
	return (PICT(a, c));
}


/*
 * Determine the path taken by a projection.
 *:::�{�[����r�[���Ȃǂ̌o�H����肷��B
 *
 * The projection will always start from the grid (y1,x1), and will travel
 * towards the grid (y2,x2), touching one grid per unit of distance along
 * the major axis, and stopping when it enters the destination grid or a
 * wall grid, or has travelled the maximum legal distance of "range".
 *:::(y1,x1)����(y2,x2)�܂ł̒��������߁A����ɉ����ăO���b�h��I�o����B�ǂ��˒��͈͂Ŏ~�܂�B
 *
 * Note that "distance" in this function (as in the "update_view()" code)
 * is defined as "MAX(dy,dx) + MIN(dy,dx)/2", which means that the player
 * actually has an "octagon of projection" not a "circle of projection".
 *:::�˒������͏�L�B���ʁA�˒��͈͎͂g�p�҂��甪�p�`�ɂȂ�B
 *
 * The path grids are saved into the grid array pointed to by "gp", and
 * there should be room for at least "range" grids in "gp".  Note that
 * due to the way in which distance is calculated, this function normally
 * uses fewer than "range" grids for the projection path, so the result
 * of this function should never be compared directly to "range".  Note
 * that the initial grid (y1,x1) is never saved into the grid array, not
 * even if the initial grid is also the final grid.  XXX XXX XXX
 *:::�O���b�h��gp[]�ɕۑ������Bgp[]�͎˒��͈͂ɏ\���Ȓ��������邱�ƁB
 *:::�o�H�͂��΂��Γr���Ŏ~�܂�̂�gp[]��range�܂Œ����Ȃ�Ȃ����ɒ��Ӂi�H�j�B
 *:::gp[]�ɊJ�n�O���b�h(x1,y1)�͓���Ȃ��B
 *
 * The "flg" flags can be used to modify the behavior of this function.
 *
 * In particular, the "PROJECT_STOP" and "PROJECT_THRU" flags have the same
 * semantics as they do for the "project" function, namely, that the path
 * will stop as soon as it hits a monster, or that the path will continue
 * through the destination grid, respectively.
 *:::STOP�̓����X�^�[�ɓ�����Ǝ~�܂�ATHRU�̓����X�^�[�ɓ������Ă�����
 *
 * The "PROJECT_JUMP" flag, which for the "project()" function means to
 * start at a special grid (which makes no sense in this function), means
 * that the path should be "angled" slightly if needed to avoid any wall
 * grids, allowing the player to "target" any grid which is in "view".
 * This flag is non-trivial and has not yet been implemented, but could
 * perhaps make use of the "vinfo" array (above).  XXX XXX XXX
 *:::�������Ă邩�������B
 *
 * This function returns the number of grids (if any) in the path.  This
 * function will return zero if and only if (y1,x1) and (y2,x2) are equal.
 *:::�^�[�Q�b�g�܂ł̃O���b�h����Ԃ��B�n�_�ƏI�_�������Ƃ�0��Ԃ��B
 *
 * This algorithm is similar to, but slightly different from, the one used
 * by "update_view_los()", and very different from the one used by "los()".

 *:::gp[]�ɂ́Ax,y�����̃O���b�h���W������炵���B���8bit��y�A����8bit��x
 *:::�ڂ����v�Z�͖���
 */
sint project_path(u16b *gp, int range, int y1, int x1, int y2, int x2, int flg)
{
	int y, x;

	int n = 0;
	int k = 0;

	/* Absolute */
	int ay, ax;

	/* Offsets */
	int sy, sx;

	/* Fractions */
	int frac;

	/* Scale factors */
	int full, half;

	/* Slope */
	int m;

	/* No path necessary (or allowed) */
	if ((x1 == x2) && (y1 == y2)) return (0);


	/* Analyze "dy" */
	if (y2 < y1)
	{
		ay = (y1 - y2);
		sy = -1;
	}
	else
	{
		ay = (y2 - y1);
		sy = 1;
	}

	/* Analyze "dx" */
	if (x2 < x1)
	{
		ax = (x1 - x2);
		sx = -1;
	}
	else
	{
		ax = (x2 - x1);
		sx = 1;
	}


	/* Number of "units" in one "half" grid */
	half = (ay * ax);

	/* Number of "units" in one "full" grid */
	full = half << 1;

	/* Vertical */
	if (ay > ax)
	{
		/* Let m = ((dx/dy) * full) = (dx * dx * 2) */
		m = ax * ax * 2;

		/* Start */
		y = y1 + sy;
		x = x1;

		frac = m;

		if (frac > half)
		{
			/* Advance (X) part 2 */
			x += sx;

			/* Advance (X) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while (1)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if ((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) &&
				    (player_bold(y, x) || cave[y][x].m_idx != 0))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Slant */
			if (m)
			{
				/* Advance (X) part 1 */
				frac += m;

				/* Horizontal change */
				if (frac > half)
				{
					/* Advance (X) part 2 */
					x += sx;

					/* Advance (X) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (Y) */
			y += sy;
		}
	}

	/* Horizontal */
	else if (ax > ay)
	{
		/* Let m = ((dy/dx) * full) = (dy * dy * 2) */
		m = ay * ay * 2;

		/* Start */
		y = y1;
		x = x1 + sx;

		frac = m;

		/* Vertical change */
		if (frac > half)
		{
			/* Advance (Y) part 2 */
			y += sy;

			/* Advance (Y) part 3 */
			frac -= full;

			/* Track distance */
			k++;
		}

		/* Create the projection path */
		while (1)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if ((n + (k >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) &&
				    (player_bold(y, x) || cave[y][x].m_idx != 0))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Slant */
			if (m)
			{
				/* Advance (Y) part 1 */
				frac += m;

				/* Vertical change */
				if (frac > half)
				{
					/* Advance (Y) part 2 */
					y += sy;

					/* Advance (Y) part 3 */
					frac -= full;

					/* Track distance */
					k++;
				}
			}

			/* Advance (X) */
			x += sx;
		}
	}

	/* Diagonal */
	else
	{
		/* Start */
		y = y1 + sy;
		x = x1 + sx;

		/* Create the projection path */
		while (1)
		{
			/* Save grid */
			gp[n++] = GRID(y, x);

			/* Hack -- Check maximum range */
			if ((n + (n >> 1)) >= range) break;

			/* Sometimes stop at destination grid */
			if (!(flg & (PROJECT_THRU)))
			{
				if ((x == x2) && (y == y2)) break;
			}

			if (flg & (PROJECT_DISI))
			{
				if ((n > 0) && cave_stop_disintegration(y, x)) break;
			}
			else if (flg & (PROJECT_LOS))
			{
				if ((n > 0) && !cave_los_bold(y, x)) break;
			}
			else if (!(flg & (PROJECT_PATH)))
			{
				/* Always stop at non-initial wall grids */
				if ((n > 0) && !cave_have_flag_bold(y, x, FF_PROJECT)) break;
			}

			/* Sometimes stop at non-initial monsters/players */
			if (flg & (PROJECT_STOP))
			{
				if ((n > 0) &&
				    (player_bold(y, x) || cave[y][x].m_idx != 0))
					break;
			}

			if (!in_bounds(y, x)) break;

			/* Advance (Y) */
			y += sy;

			/* Advance (X) */
			x += sx;
		}
	}

	/* Length */
	return (n);
}



/*
 * Mega-Hack -- track "affected" monsters (see "project()" comments)
 */
static int project_m_n;
static int project_m_x;
static int project_m_y;
/* Mega-Hack -- monsters target */
static s16b monster_target_x;
static s16b monster_target_y;


/*
 * We are called from "project()" to "damage" terrain features
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 *
 * XXX XXX XXX We also "see" grids which are "memorized", probably a hack
 *
 * XXX XXX XXX Perhaps we should affect doors?
 */
/*:::���@�Ȃǂ̒n�`�ɑ΂���e��*/
/*:::���Ɍ�����Ƃ���ŉ����N��������TRUE��Ԃ�*/
/*:::who:�g�p�� r:�^�[�Q�b�g���W�̔��S�n����̋��� y,x �^�[�Q�b�g���W dam:�_���[�W typ:����*/
///sys magic �����@�U�����@�Ȃǂɂ��n�`�ɑ΂���e���@�؂̔j���n��ω��Ȃ�
bool project_f(int who, int r, int y, int x, int dam, int typ)
{
	cave_type       *c_ptr = &cave[y][x];
	feature_type    *f_ptr = &f_info[c_ptr->feat];

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);


	/* XXX XXX XXX */
	/*:::���̒l��FALSE�Ƃ݂Ȃ����ꍇ�̂��߂̏����E�E�E�E�H*/
	who = who ? who : 0;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	//�M���e�̔��S�n���烉�C�g�G���A
	if(typ == GF_SPECIAL_SHOT && special_shooting_mode == SSM_LIGHT && !r)
		lite_room(y,x);

	///mod150721
	/*:::�M�̔j�󏈗�*/
	if (is_plate_grid(c_ptr))
	{
		cptr message;
		switch (typ)
		{
		case GF_TIME:
		case GF_DISINTEGRATE:
			message = "������";break;
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_ROCKET:
		case GF_SHARDS:
		case GF_SOUND:
		case GF_FORCE:
		case GF_DISTORTION:
		case GF_TORNADO:
		case GF_TRAIN:
		case GF_DISENCHANT:

			message = "���ꂽ";break;
		default:
			message = NULL;break;
		}
		if (message && known)
		{
			msg_format("�M��%s�B", message);
			futo_break_plate(y,x);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}
	//���e�̗U������
	else if(((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_BOMB)))
	{
		int tmp_dam = 0;
		switch (typ)
		{
		case GF_TIME:
		case GF_DISINTEGRATE:
		case GF_PIT_FALL:
			//���e���ŏ���
			msg_format("���e�͏������B");
			c_ptr->info &= ~(CAVE_MARK);
			c_ptr->info &= ~(CAVE_OBJECT);
			c_ptr->mimic = 0;
			c_ptr->special = 0;
			note_spot(y, x);
			lite_spot(y, x);
			break;
		case GF_FIRE:
		case GF_ELEC:
		case GF_PLASMA:
		case GF_SHARDS:
			tmp_dam = dam / 10 + randint0(3);
			bomb_count(y,x,-tmp_dam);
			break;
		case GF_NUKE:
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_ROCKET:
			tmp_dam = randint1(dam/3);
			bomb_count(y,x,-tmp_dam);
			break;

		case GF_WATER:
		case GF_INACT:
			tmp_dam = 0-(dam/5);
			bomb_count(y,x,-tmp_dam);
			break;
		case GF_CHAOS:
			tmp_dam = 30 - randint1(60);
			bomb_count(y,x,-tmp_dam);
			break;

		default:
			break;

		}
	}
	//v1.1.68 �V�n�`�u�X��v
	else if (have_flag(f_ptr->flags, FF_ICE_WALL))
	{

		if (typ == GF_FIRE || typ == GF_NUKE || typ == GF_STEAM)
		{
			msg_format("�X���n�����B");
			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
			cave_set_feat(y, x, feat_floor);

		}
		else if (typ == GF_LITE)
		{
			msg_format("�X�̒������������˂��Ă���...");
			
			if (randint1(100) < dam)
			{
				if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
				cave_set_feat(y, x, feat_floor);
				msg_format("�X���唚������!");
				project(PROJECT_WHO_EXPLODE_ICE, 5, y, x, dam * 4, GF_ROCKET, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
			}

		}

	}
	///mod150402
	/*:::�w偂̑��̔j�󏈗�*/
	else if (have_flag(f_ptr->flags, FF_SPIDER_NEST_1))
	{
		cptr message;
		switch (typ)
		{
		case GF_TIME:
		case GF_WATER:
		case GF_HOLY_WATER:
			message = "������";break;
		case GF_COLD:
		case GF_NORTHERN:
		case GF_ICE:
			message = "�ӂ���";break;
		case GF_FIRE:
		case GF_ACID:
		case GF_ELEC:
		case GF_PLASMA:
		case GF_NUKE:
		case GF_STEAM:
			message = "�n����";break;
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		case GF_ROCKET:
		case GF_SOUND:
		case GF_DISENCHANT:
		case GF_FORCE:
		case GF_DISTORTION:
		case GF_TORNADO:
		case GF_WINDCUTTER:
		case GF_TRAIN:
		case GF_MOSES:
		case GF_REDMAGIC:
		case GF_GUNGNIR:

			message = "�������";break;
		default:
			message = NULL;break;
		}
		if (message && known)
		{
			msg_format("����%s�B", message);
			cave_set_feat(y, x, feat_floor);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}
	//v1.1.21 �|�[�^���̔j�󏈗��@(�󂵂Ƃ��Ȃ��Ɣ��e�ƃ|�[�^���ƃo���P�[�h�Ńn���E���ł������Ȃ̂�)
	else if (have_flag(f_ptr->flags, FF_PORTAL))
	{
		cptr message;
		switch (typ)
		{
		case GF_TIME:
			message = "������";break;
		case GF_NUKE:
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_ROCKET:
		case GF_SHARDS:
		case GF_DISENCHANT:
		case GF_FORCE:
		case GF_DISTORTION:
		case GF_TRAIN:
			message = "�j�󂳂ꂽ";break;
		default:
			message = NULL;break;
		}
		if (message)
		{
			msg_format("�|�[�^����%s�I", message);
			cave_set_feat(y, x, feat_floor);
			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}
	}
	/*:::�؂̔j�󏈗�*/
	else if (have_flag(f_ptr->flags, FF_TREE))
	{
		cptr message;
		switch (typ)
		{
		case GF_POIS:
		case GF_POLLUTE:
		case GF_DEATH_RAY:
		case GF_YAKU:
#ifdef JP
			message = "�͂ꂽ";break;
#else
			message = "was blasted.";break;
#endif
		case GF_TIME:
#ifdef JP
			message = "�k��";break;
#else
			message = "shrank.";break;
#endif
		case GF_ACID:
#ifdef JP
			message = "�n����";break;
#else
			message = "melted.";break;
#endif
		case GF_COLD:
		case GF_NORTHERN:
		case GF_ICE:
#ifdef JP
			message = "����A�ӂ��U����";break;
#else
			message = "was frozen and smashed.";break;
#endif
		case GF_FIRE:
		case GF_ELEC:
		case GF_PLASMA:
		case GF_NUKE:
#ifdef JP
			message = "�R����";break;
#else
			message = "burns up!";break;
#endif
		case GF_METEOR:
		case GF_KANAMEISHI:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		case GF_SHARDS:
		case GF_ROCKET:
		case GF_SOUND:
		case GF_DISENCHANT:
		case GF_FORCE:
		case GF_GRAVITY:
		case GF_DISTORTION:
		case GF_TORNADO:
		case GF_WINDCUTTER:
		case GF_TRAIN:
		case GF_MOSES:
		case GF_REDMAGIC:
		case GF_GUNGNIR:
		case GF_BLAZINGSTAR:

#ifdef JP
			message = "���ӂ��ꂽ";break;
#else
			message = "was crushed.";break;
#endif
		case GF_SOULSCULPTURE:
		case GF_YOUMU:
			message = "�a��|���ꂽ";break;

		default:
			message = NULL;break;
		}

		if(message) cave_set_feat(y, x, one_in_(3) ? feat_brake : feat_grass);

		if (message && known)
		{
#ifdef JP
			msg_format("�؂�%s�B", message);
#else
			msg_format("A tree %s", message);
#endif

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;
		}

	}



	/* Analyze the type */
	switch (typ)
	{
		/* Ignore most effects */
		//�����ɂ�����Ə����Ȃ��Ă���������Ȃ���
		case GF_CAPTURE:
		case GF_REDMAGIC:
		case GF_GUNGNIR:
		case GF_HAND_DOOM:
		case GF_CAUSE_1:
		case GF_CAUSE_2:
		case GF_CAUSE_3:
		case GF_CAUSE_4:
		case GF_MIND_BLAST:
		case GF_BRAIN_SMASH:
		case GF_DRAIN_MANA:
		case GF_PSY_SPEAR:
		case GF_FORCE:
		case GF_HOLY_FIRE:
		case GF_HELL_FIRE:
		case GF_PSI:
		case GF_PSI_DRAIN:
		case GF_TELEKINESIS:
		case GF_DOMINATION:
		case GF_IDENTIFY:
		case GF_ATTACK:
		case GF_ACID:
		case GF_ELEC:
		case GF_COLD:
		case GF_ICE:
		case GF_FIRE:
		case GF_PLASMA:
		case GF_METEOR:
		case GF_CHAOS:
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		case GF_COSMIC_HORROR:
		case GF_DISTORTION:
		case GF_PUNISH_1:
		case GF_PUNISH_2:
		case GF_PUNISH_3:
		case GF_PUNISH_4:
		case GF_KYUTTOSHITEDOKA_N:
		case GF_WINDCUTTER:
		case GF_REDEYE:
		case GF_RYUURI:
		case GF_DISP_LIVING:
		case GF_POSSESSION:
		case GF_SOULSTEAL:
		case GF_NIGHTMARE:
		case GF_N_E_P:
		case GF_SATORI:
		case GF_KOKORO:
		case GF_SOULSCULPTURE:
		case GF_STEAM:
		case GF_BRAIN_FINGER_PRINT:
		case GF_RAINBOW:
		{
			break;
		}

		/* Destroy Traps (and Locks) */
		/*:::�g���b�v����*/
		//v1.1.17 �������g���b�v��������悤�ɂ��Ă݂�
		case GF_KILL_TRAP:
		case GF_PURIFY:
		{
			/* Reveal secret doors */
			if (is_hidden_door(c_ptr))
			{
				/* Pick a door */
				disclose_grid(y, x);

				/* Check line of sight */
				if (known)
				{
					obvious = TRUE;
				}
			}

			/* Destroy traps */
			if (is_trap(c_ptr->feat))
			{
				/* Check line of sight */
				if (known && typ == GF_KILL_TRAP)
				{
#ifdef JP
					msg_print("�܂΂䂢�M�����������I");
#else
					msg_print("There is a bright flash of light!");
#endif

					obvious = TRUE;
				}

				/* Destroy the trap */
				cave_alter_feat(y, x, FF_DISARM);
			}

			/* Locked doors are unlocked */
			if (is_closed_door(c_ptr->feat) && f_ptr->power && have_flag(f_ptr->flags, FF_OPEN))
			{
				s16b old_feat = c_ptr->feat;

				/* Unlock the door */
				cave_alter_feat(y, x, FF_DISARM);

				/* Check line of sound */
				if (known && (old_feat != c_ptr->feat))
				{
#ifdef JP
					msg_print("�J�`�b�Ɖ��������I");
#else
					msg_print("Click!");
#endif

					obvious = TRUE;
				}
			}

			/* Remove "unsafe" flag if player is not blind */
			if (!p_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(y, x);

				obvious = TRUE;
			}

			break;
		}

		/* Destroy Doors (and traps) */
		/*:::�g���b�v�E�h�A�j��*/
		case GF_KILL_DOOR:
		{
			/* Destroy all doors and traps */
			if (is_trap(c_ptr->feat) || have_flag(f_ptr->flags, FF_DOOR))
			{
				/* Check line of sight */
				if (known)
				{
					/* Message */
#ifdef JP
					msg_print("�܂΂䂢�M�����������I");
#else
					msg_print("There is a bright flash of light!");
#endif

					obvious = TRUE;
				}

				/* Destroy the feature */
				cave_alter_feat(y, x, FF_TUNNEL);
			}

			/* Remove "unsafe" flag if player is not blind */
			if (!p_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(y, x);

				obvious = TRUE;
			}

			break;
		}

		//v1.1.96 �g���b�v����
		case GF_ACTIV_TRAP:
		{
			/* Destroy traps */
			if (is_trap(c_ptr->feat))
			{
				/* Check line of sight */
				if (known && typ == GF_KILL_TRAP)
				{
#ifdef JP
					msg_print("�g���b�v�����������I");
#else
					msg_print("There is a bright flash of light!");
#endif
					obvious = TRUE;
				}

				activate_floor_trap(y, x, 0L);

			}


		}
		break;


		/*:::�h�A�Œ�i���@�̎{���H�j*/
		/*:::�Œ�\�ȕ����h�A�ɗL���@�J�����h�A�ɂ͖������Ȃ�*/
		case GF_JAM_DOOR: /* Jams a door (as if with a spike) */
		{
			if (have_flag(f_ptr->flags, FF_SPIKE))
			{
				s16b old_mimic = c_ptr->mimic;
				feature_type *mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];

				cave_alter_feat(y, x, FF_SPIKE);

				c_ptr->mimic = old_mimic;

				/* Notice */
				note_spot(y, x);

				/* Redraw */
				lite_spot(y, x);

				/* Check line of sight */
				if (known && have_flag(mimic_f_ptr->flags, FF_OPEN))
				{
					/* Message */
#ifdef JP
					msg_format("%s�ɉ������������ĊJ���Ȃ��Ȃ����B", f_name + mimic_f_ptr->name);
#else
					msg_format("The %s seems stuck.", f_name + mimic_f_ptr->name);
#endif

					obvious = TRUE;
				}
			}
			break;
		}

		/* Destroy walls (and doors) */
		/*:::��Ηn��*/
		case GF_KILL_WALL:
		{
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
#ifdef JP

					if (have_flag(f_ptr->flags, FF_ICE_WALL))
						msg_format("%s���n�����I", f_name + f_info[get_feat_mimic(c_ptr)].name);
					else
						msg_format("%s���n���ēD�ɂȂ����I", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s turns into mud!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif

					obvious = TRUE;
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}

			break;
		}
		case GF_WALL_TO_DOOR:
		{
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_format("%s���h�A�ɂȂ����I", f_name + f_info[get_feat_mimic(c_ptr)].name);
					obvious = TRUE;
				}

				cave_set_feat(y, x, feat_door[DOOR_DOOR].closed);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}

			break;
		}

		/*:::�󓃃��[�U�[�@���ɍ���𗎂Ƃ� */
		case GF_HOUTOU:
		{
			if (have_flag(f_ptr->flags, FF_HURT_ROCK))
			{
				object_type forge;
				object_type *q_ptr = &forge;
				object_wipe(q_ptr);
				/* Message */
				if (known && (c_ptr->info & (CAVE_MARK)))
				{
					msg_format("%s����΂ɕς�����I", f_name + f_info[get_feat_mimic(c_ptr)].name);
					obvious = TRUE;
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				if (!cave_naked_bold(y, x)) break;
				if (player_bold(y, x)) break;
				if (!make_gold(q_ptr)) break;

				(void)drop_near(q_ptr, -1, y, x);
				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}

			break;
		}

		/* Make doors */
		case GF_MAKE_DOOR:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_door[DOOR_DOOR].closed);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}
		//v2.0.1 �v�Α����@���S�n����n�`�ɕς���
		case GF_KANAMEISHI:
		{
			//���S�n�̂ݑΏ�
			if (r) break;

			//�i�v�n�`�ɂ͖���
			if (cave_have_flag_bold(y, x, FF_PERMANENT)) break;

			//�����X�^�[��A�C�e���̂���Ƃ���ɂ͖���
			if ((cave[y][x].info & CAVE_OBJECT) || (cave[y][x].o_idx) || (cave[y][x].m_idx)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_rubble);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}

		/* Make doors */
		case GF_MAKE_FLOWER:
		{
			/* Require a "naked" floor grid */
			if (!cave_clean_bold(y, x) || cave[y][x].m_idx) break;

			/* Not on the player */
			//if (player_bold(y, x)) break;

			if(one_in_(4)) cave_set_feat(y, x, feat_tree);
			else cave_set_feat(y, x, feat_flower);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}
		/* Make traps */
		/*:::�g���b�v����*/
		case GF_MAKE_TRAP:
		{
			/* Place a trap */
			place_trap(y, x);

			break;
		}

		/* Make doors */
		/*:::�X�ѐ���*/
		case GF_MAKE_TREE:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			/* Create a closed door */
			cave_set_feat(y, x, feat_tree);

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;


			break;
		}


		case GF_MAKE_SPIDER_NEST:
		{
			if (!cave_have_flag_bold(y, x, FF_FLOOR)) break;
			if ((cave[y][x].info & CAVE_OBJECT)) break;
			cave_set_feat(y, x, f_tag_to_index_in_init("SPIDER_NEST"));
			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}
		case GF_THROW_PLATE:
		{
			if(!futo_can_place_plate(y,x)) break;

			cave[y][x].info |= CAVE_OBJECT;
			cave[y][x].mimic = feat_plate;

			/* Observe */
			if (c_ptr->info & (CAVE_MARK)) obvious = TRUE;

			break;
		}


		case GF_MAKE_GLYPH:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Create a glyph */
			c_ptr->info |= CAVE_OBJECT;
			c_ptr->mimic = feat_glyph;

			/* Notice */
			note_spot(y, x);

			/* Redraw */
			lite_spot(y, x);

			break;
		}

		case GF_STONE_WALL:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Not on the player */
			if (player_bold(y, x)) break;

			//v1.1.89 �S�X�����Z�p�ɕǂ����ɂ���B���ʂɐΕǐ��������Ƃ�����ɂȂ��Ă��܂����܂����p��̖��͂Ȃ����낤
			if(p_ptr->pclass == CLASS_MOMOYO)
				cave_set_feat(y, x, feat_rubble);
			else
				cave_set_feat(y, x, feat_granite);

			break;
		}


		case GF_LAVA_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Lava */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow lava */
				cave_set_feat(y, x, feat_shallow_lava);
			}
			/* Deep Lava */
			else if (dam)
			{
				/* Place a deep lava */
				cave_set_feat(y, x, feat_deep_lava);
			}

			//v1.1.85 ���̂���ʒu���n�`�ω����ĉ��炩�̃p�����[�^���ς�邩�������̂Œǉ�
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}

		case GF_WATER_FLOW:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Shallow Water */
			if (dam == 1)
			{
				/* Ignore grid without enough space */
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

				/* Place a shallow water */
				cave_set_feat(y, x, feat_shallow_water);
			}
			/* Deep Water */
			else if (dam)
			{
				/* Place a deep water */
				cave_set_feat(y, x, feat_deep_water);
			}
			//v1.1.85 ���̂���ʒu���n�`�ω����ĉ��炩�̃p�����[�^���ς�邩�������̂Œǉ�
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		//v1.1.91
		case GF_DIG_OIL:
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			/* Ignore grid without enough space */
			//�g���b�v�n����㩒n�`��~���l�߂���Ζ��n�`�ω���h�����肷��
			if (!have_flag(f_ptr->flags, FF_FLOOR)) break;

			//v1.1.92 ������p���i�ł͖��n�`�ɕω��������O���b�h�����L�^���邱�Ƃɂ��Ă݂�
			if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON ) && !who)
				jyoon_record_money_waste(1);

			cave_set_feat(y, x, feat_oil_field);

			//���̂���ʒu���n�`�ω����ĉ��炩�̃p�����[�^���ς�邩�������̂Œǉ�
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}


		case GF_MAKE_BLIZZARD: //v1.1.85 ���@����
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_cold_zone);
			}
			else if (dam)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;//���@���ᐶ���͗n��␅�ƈႢ�ǂ����Ȃ�
				cave_set_feat(y, x, feat_heavy_cold_zone);
			}
			//v1.1.85 ���̂���ʒu���n�`�ω����ĉ��炩�̃p�����[�^���ς�邩�������̂Œǉ�
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		case GF_MAKE_STORM: //v1.1.85 ���@�̗��_
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_electrical_zone);
			}
			else if (dam)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;//���_�����͗n��␅�ƈႢ�ǂ����Ȃ�
				cave_set_feat(y, x, feat_heavy_electrical_zone);
			}
			//v1.1.85 ���̂���ʒu���n�`�ω����ĉ��炩�̃p�����[�^���ς�邩�������̂Œǉ�
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		case GF_MAKE_POISON_PUDDLE: //v1.1.85 �ł̏�
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_poisonous_puddle);
			}
			else if (dam)
			{
				cave_set_feat(y, x, feat_deep_poisonous_puddle);
			}
			//v1.1.85 ���̂���ʒu���n�`�ω����ĉ��炩�̃p�����[�^���ς�邩�������̂Œǉ�
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;

			break;
		}
		case GF_MAKE_ACID_PUDDLE: //v1.1.85 �_�̏�
		{
			/* Ignore permanent grid */
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;

			if (dam == 1)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;
				cave_set_feat(y, x, feat_acid_puddle);
			}
			else if (dam)
			{
				if (!have_flag(f_ptr->flags, FF_FLOOR)) break;//���@���ᐶ���͗n��␅�ƈႢ�ǂ����Ȃ�
				cave_set_feat(y, x, feat_deep_acid_puddle);
			}
			//v1.1.85 ���̂���ʒu���n�`�ω����ĉ��炩�̃p�����[�^���ς�邩�������̂Œǉ�
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= PR_STATE | PR_STATS | PR_STATUS;
			break;
		}


		case GF_MOSES:
		{
			if (have_flag(f_ptr->flags, FF_PERMANENT)) break;
			//v1.1.85 �_�̏��Ɠł̏����ǉ�
			if (!have_flag(f_ptr->flags, FF_WATER) && !have_flag(f_ptr->flags, FF_LAVA) && !have_flag(f_ptr->flags, FF_ACID_PUDDLE && !have_flag(f_ptr->flags, FF_POISON_PUDDLE))) break;
			cave_set_feat(y, x,  feat_floor);
			break;
		}
		/* Lite up the grid */
		case GF_LITE_WEAK:
		case GF_LITE:
		{
			/* Turn on the light */
			if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
			{
				c_ptr->info |= (CAVE_GLOW);

				/* Notice */
				note_spot(y, x);

				/* Redraw */
				lite_spot(y, x);

				update_local_illumination(y, x);

				/* Observe */
				if (player_can_see_bold(y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the monster in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);

				if (p_ptr->special_defense & NINJA_S_STEALTH)
				{
					if (player_bold(y, x)) set_superstealth(FALSE);
				}
			}

			break;
		}

		/* Darken the grid */
		case GF_DARK_WEAK:
		case GF_DARK:
		{
			bool do_dark = !p_ptr->inside_battle && !is_mirror_grid(c_ptr);
			int j;

			/* Turn off the light. */
			if (do_dark)
			{
				if (dun_level || !is_daytime())
				{
					for (j = 0; j < 9; j++)
					{
						int by = y + ddy_ddd[j];
						int bx = x + ddx_ddd[j];

						if (in_bounds2(by, bx))
						{
							cave_type *cc_ptr = &cave[by][bx];

							if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
							{
								do_dark = FALSE;
								break;
							}
						}
					}

					if (!do_dark) break;
				}

				c_ptr->info &= ~(CAVE_GLOW);

				/* Hack -- Forget "boring" grids */
				if (!have_flag(f_ptr->flags, FF_REMEMBER))
				{
					/* Forget */
					c_ptr->info &= ~(CAVE_MARK);

					/* Notice */
					note_spot(y, x);
				}

				/* Redraw */
				lite_spot(y, x);

				update_local_illumination(y, x);

				/* Notice */
				if (player_can_see_bold(y, x)) obvious = TRUE;

				/* Mega-Hack -- Update the monster in the affected grid */
				/* This allows "spear of light" (etc) to work "correctly" */
				if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);
			}

			/* All done */
			break;
		}

		case GF_SHARDS:
		case GF_ROCKET:
		{
			if (is_mirror_grid(c_ptr))
			{
#ifdef JP
				msg_print("�������ꂽ�I");
#else
				msg_print("The mirror was crashed!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(y, x);
				project(0, 2, y, x, p_ptr->lev / 2 + 5, GF_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if (have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 50))
			{
				/* Message */
				if (known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%s�����ꂽ�I", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was crashed!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}
			break;
		}

		case GF_SOUND:
		{
			if (is_mirror_grid(c_ptr) && p_ptr->lev < 40)
			{
#ifdef JP
				msg_print("�������ꂽ�I");
#else
				msg_print("The mirror was crashed!");
#endif
				sound(SOUND_GLASS);
				remove_mirror(y, x);
				project(0, 2, y, x, p_ptr->lev / 2 + 5, GF_SHARDS, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}

			if (have_flag(f_ptr->flags, FF_GLASS) && !have_flag(f_ptr->flags, FF_PERMANENT) && (dam >= 200))
			{
				/* Message */
				if (known && (c_ptr->info & CAVE_MARK))
				{
#ifdef JP
					msg_format("%s�����ꂽ�I", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
					msg_format("The %s was crashed!", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif
					sound(SOUND_GLASS);
				}

				/* Destroy the wall */
				cave_alter_feat(y, x, FF_HURT_ROCK);

				/* Update some things */
				p_ptr->update |= (PU_FLOW);
			}
			break;
		}

		case GF_DISINTEGRATE:
		case GF_BLAZINGSTAR:
		{
			/* Destroy mirror/glyph */
			if (is_mirror_grid(c_ptr) || is_glyph_grid(c_ptr) || is_explosive_rune_grid(c_ptr))
				remove_mirror(y, x);

			/* Permanent features don't get effect */
			/* But not protect monsters and other objects */
			if (have_flag(f_ptr->flags, FF_HURT_DISI) && !have_flag(f_ptr->flags, FF_PERMANENT))
			{
				cave_alter_feat(y, x, FF_HURT_DISI);

				/* Update some things -- similar to GF_KILL_WALL */
				p_ptr->update |= (PU_FLOW);
			}
			break;
		}
	}

	lite_spot(y, x);
	/* Return "Anything seen?" */
	return (obvious);
}



/*
 * We are called from "project()" to "damage" objects
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * Perhaps we should only SOMETIMES damage things on the ground.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * XXX XXX XXX We also "see" grids which are "memorized", probably a hack
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 */
/*:::�w��O���b�h�A�w�葮���ɂ�鏰��A�C�e���ɑ΂���j�󏈗��@*/
///item spell �����ɂ��A�C�e���j�󏈗�
bool project_o(int who, int r, int y, int x, int dam, int typ)
{
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;

	bool obvious = FALSE;
	bool known = player_has_los_bold(y, x);

	u32b flgs[TR_FLAG_SIZE];

	char o_name[MAX_NLEN];

	int k_idx = 0;
	bool is_potion = FALSE;

	bool flag_need_restart_loop;


	/* XXX XXX XXX */
	who = who ? who : 0;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);
	
	do
	{
		flag_need_restart_loop = FALSE;

		/* Scan all objects in the grid */
		/*:::����A�C�e�����[�v*/
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			/* Acquire object */
			object_type *o_ptr = &o_list[this_o_idx];

			bool is_art = FALSE;
			bool ignore = FALSE;
			bool do_kill = FALSE;

			cptr note_kill = NULL;

#ifndef JP
			/* Get the "plural"-ness */
			bool plural = (o_ptr->number > 1);
#endif

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Extract the flags */
			object_flags(o_ptr, flgs);

			/* Check for artifact */
			if (object_is_artifact(o_ptr)) is_art = TRUE;

			///mod151227 �H���̖�͔j�󂳂�Ȃ�
			if (o_ptr->tval == TV_COMPOUND && o_ptr->sval == SV_COMPOUND_HOURAI) ignore = TRUE;

			//v1.1.79 ���t�m�[���̋����j�󂳂�Ȃ�
			if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_MIRROR_OF_RUFFNOR) ignore = TRUE;

			/* Analyze the type */
			/*:::�x�[�X�A�C�e�����w�葮���Ɏア�ꍇ�j��t���O��ON*/
			/*:::�ʂɑ����ی삳�ꂽ�A�C�e���͖����t���O��ON*/
			switch (typ)
			{
				/* Acid -- Lots of things */
			case GF_ACID:
			{
				if (hates_acid(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
					note_kill = "�Z���Ă��܂����I";
#else
					note_kill = (plural ? " melt!" : " melts!");
#endif

					if (have_flag(flgs, TR_IGNORE_ACID)) ignore = TRUE;
				}
				break;
			}

			/* Elec -- Rings and Wands */
			case GF_ELEC:
			{
				if (hates_elec(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
					note_kill = "���Ă��܂����I";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

					if (have_flag(flgs, TR_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire -- Flammable objects */
			case GF_FIRE:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
					note_kill = "�R���Ă��܂����I";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				break;
			}

			/* Cold -- potions and flasks */
			case GF_COLD:
			{
				if (hates_cold(o_ptr))
				{
#ifdef JP
					note_kill = "�ӂ��U���Ă��܂����I";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					do_kill = TRUE;
					if (have_flag(flgs, TR_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Elec */
			case GF_PLASMA:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
					note_kill = "�R���Ă��܂����I";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_elec(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
					note_kill = "���Ă��܂����I";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

					if (have_flag(flgs, TR_IGNORE_ELEC)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Cold */
			case GF_METEOR:
			case GF_KANAMEISHI:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
					note_kill = "�R���Ă��܂����I";
#else
					note_kill = (plural ? " burn up!" : " burns up!");
#endif

					if (have_flag(flgs, TR_IGNORE_FIRE)) ignore = TRUE;
				}
				if (hates_cold(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
#ifdef JP
					note_kill = "�ӂ��U���Ă��܂����I";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					if (have_flag(flgs, TR_IGNORE_COLD)) ignore = TRUE;
				}
				break;
			}

			/* Hack -- break potions and such */
			case GF_ICE:
			case GF_NORTHERN:
			case GF_SHARDS:
			case GF_FORCE:
			case GF_SOUND:
			case GF_TRAIN:
			case GF_TORNADO:
			{
				if (hates_cold(o_ptr))
				{
#ifdef JP
					note_kill = "�ӂ��U���Ă��܂����I";
#else
					note_kill = (plural ? " shatter!" : " shatters!");
#endif

					do_kill = TRUE;
				}
				break;
			}

			/* Mana and Chaos -- destroy everything */
			case GF_MANA:
			case GF_SEEKER:
			case GF_SUPER_RAY:
			{
				do_kill = TRUE;
#ifdef JP
				note_kill = "���Ă��܂����I";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				break;
			}

			case GF_DISINTEGRATE:
			case GF_BLAZINGSTAR:
			{
				do_kill = TRUE;
#ifdef JP
				note_kill = "�������Ă��܂����I";
#else
				note_kill = (plural ? " evaporate!" : " evaporates!");
#endif

				break;
			}

			case GF_CHAOS:
			{
				do_kill = TRUE;
#ifdef JP
				note_kill = "���Ă��܂����I";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				if (have_flag(flgs, TR_RES_CHAOS)) ignore = TRUE;
				else if ((o_ptr->tval == TV_SCROLL) && (o_ptr->sval == SV_SCROLL_CHAOS)) ignore = TRUE;
				break;
			}
			///mod140211�򉻑������A�C�e�����ׂĉ󂷂悤�ɂ���
			case GF_DISENCHANT:
			{
				do_kill = TRUE;
#ifdef JP
				note_kill = "����Ă��܂����I";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				if (have_flag(flgs, TR_RES_DISEN)) ignore = TRUE;
				break;
			}
			///��Ԙc�ȑ����Ɖ������� 1/3�ŃA�C�e����j�󂷂�
			case GF_DISTORTION:
			case GF_POLLUTE:
			{
				if (!one_in_(3)) break;

				do_kill = TRUE;
#ifdef JP
				note_kill = "���Ă��܂����I";
#else
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				if (have_flag(flgs, TR_RES_TIME)) ignore = TRUE;
				break;
			}


			/* Holy Fire and Hell Fire -- destroys cursed non-artifacts */
			case GF_HOLY_FIRE:
			case GF_HELL_FIRE:
			{
				if (object_is_cursed(o_ptr))
				{
					do_kill = TRUE;
#ifdef JP
					note_kill = "���Ă��܂����I";
#else
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
#endif

				}
				break;
			}

			case GF_IDENTIFY:
			{
				identify_item(o_ptr);

				/* Auto-inscription */
				autopick_alter_item((-this_o_idx), FALSE);
				break;
			}

			//v1.1.97 �g���b�v����
			case GF_ACTIV_TRAP:
			{
				if (o_ptr->tval == TV_CHEST)
				{
					if (o_ptr->pval > 0)
					{
						activate_chest_trap(y, x, this_o_idx, FALSE);
						obvious = TRUE;
						
						// -Hack- ���ȊO�ւ̃g���b�v���ʂ̎����ɂ��A
						//�������������蔠�̃g���b�v�ō���('E')��|�����肷��Ȃǂ��Ă��̃O���b�h�̑��̃A�C�e�������[�v���ɔj�󂳂�邱�Ƃ�����B
						//���̂܂܃��[�v�𑱂����next_o_idx������ď��������������Ȃ�̂ł��̃O���b�h�ɑ΂���A�C�e���������[�v����蒼�����Ƃɂ����B
						flag_need_restart_loop = TRUE;
					}
				}
			}
			break;

			/* Unlock chests */
			case GF_KILL_TRAP:
			case GF_KILL_DOOR:
			{
				/* Chests are noticed only if trapped or locked */
				if (o_ptr->tval == TV_CHEST)
				{
					/* Disarm/Unlock traps */
					if (o_ptr->pval > 0)
					{
						/* Disarm or Unlock */
						o_ptr->pval = (0 - o_ptr->pval);

						/* Identify */
						object_known(o_ptr);

						/* Notice */
						if (known && (o_ptr->marked & OM_FOUND))
						{
#ifdef JP
							msg_print("�J�`�b�Ɖ��������I");
#else
							msg_print("Click!");
#endif

							obvious = TRUE;
						}
					}
				}

				break;
			}
			///sysdel ���ҕ���
			//case GF_ANIM_DEAD:
			{
				///del131214  ���ҕ���
#if 0
				if (o_ptr->tval == TV_CORPSE)
				{
					int i;
					u32b mode = 0L;

					if (!who || is_pet(&m_list[who]))
						mode |= PM_FORCE_PET;

					for (i = 0; i < o_ptr->number; i++)
					{
						if (((o_ptr->sval == SV_CORPSE) && (randint1(100) > 80)) ||
							((o_ptr->sval == SV_SKELETON) && (randint1(100) > 60)))
						{
							if (!note_kill)
							{
#ifdef JP
								note_kill = "�D�ɂȂ����B";
#else
								note_kill = (plural ? " become dust." : " becomes dust.");
#endif
							}
							continue;
						}
						else if (summon_named_creature(who, y, x, o_ptr->pval, mode))
						{
#ifdef JP
							note_kill = "�����Ԃ����B";
#else
							note_kill = " revived.";
#endif
						}
						else if (!note_kill)
						{
#ifdef JP
							note_kill = "�D�ɂȂ����B";
#else
							note_kill = (plural ? " become dust." : " becomes dust.");
#endif
						}
					}
					do_kill = TRUE;
					obvious = TRUE;
				}
#endif
				break;
			}
			}


			/* Attempt to destroy the object */
			if (do_kill)
			{
				/* Effect "observed" */
				if (known && (o_ptr->marked & OM_FOUND))
				{
					obvious = TRUE;
					object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
				}

				/* Artifacts, and other objects, get to resist */
				/*:::�A�[�e�B�t�@�N�g�ƌ��f�ی�A�C�e���͉e�����󂯂Ȃ�*/
				if (is_art || ignore)
				{
					/* Observe the resist */
					if (known && (o_ptr->marked & OM_FOUND))
					{
#ifdef JP
						msg_format("%s�͉e�����󂯂Ȃ��I",
							o_name);
#else
						msg_format("The %s %s unaffected!",
							o_name, (plural ? "are" : "is"));
#endif

					}
				}

				/* Kill it */
				/*:::�A�C�e���j�󏈗��@*/
				else
				{
					/* Describe if needed */
					if (known && (o_ptr->marked & OM_FOUND) && note_kill)
					{
#ifdef JP
						msg_format("%s��%s", o_name, note_kill);
#else
						msg_format("The %s%s", o_name, note_kill);
#endif

					}

					k_idx = o_ptr->k_idx;
					is_potion = object_is_potion(o_ptr);


					/* Delete the object */
					delete_object_idx(this_o_idx);

					/* Potions produce effects when 'shattered' */
					if (is_potion)
					{
						(void)potion_smash_effect(who, y, x, k_idx);
					}

					/* Redraw */
					lite_spot(y, x);
				}
			}


			if (flag_need_restart_loop)
			{
				if (cheat_xtra) msg_print("RESTART LOOP");
				break;
			}

		} //next_o_idx���[�v

	}while (flag_need_restart_loop);

	/* Return "Anything seen?" */
	return (obvious);
}





//v1.1.90
//�����X�^�[�ɑ΂���Z�[�r���O�X���[����
//�����X�^�[����R�ɐ���������TRUE
//if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
//���̂悤�ȏ����Ɠ���ւ���
bool	mon_saving_throw(monster_type *m_ptr, int power)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int chance = power;
	int rlev = r_ptr->level;

	//v1.1.95 �h��ቺ��ԂȂ��R�͂������邱�Ƃɂ���
	if (MON_DEC_DEF(m_ptr))
	{
		rlev -= MAX(25, rlev / 4);
		if (rlev < 0) rlev = 0;
	}

	//���j�[�N�ɂ͒�R����₷��
	if ((r_ptr->flags1 & RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) chance = chance * 2 / 3;

	//�_�i�ɂ͒�R����₷��
	if (r_ptr->flags3 & RF3_DEITY) chance = chance * 3 / 4;

	//�͋��������X�^�[�ɂ͒�R����₷��
	if (r_ptr->flags2 & RF2_POWERFUL) chance = chance * 3 / 4;

	//���̔��莮��10�Ƃ����}�W�b�N�i���o�[���Ȃ񂩋C�ɓ���񂩂灗�̃��x���ɂ����B�܂��������̈Ⴂ���Ȃ����낤�B
	if (chance < p_ptr->lev) chance = p_ptr->lev;

	if (cheat_xtra) msg_format("saving throw: power%d-lev%d",chance,r_ptr->level);

	if (r_ptr->level >= randint1(chance)) return TRUE;
	else return FALSE;
	
}



/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to a monster.
 *
 * This routine takes a "source monster" (by index) which is mostly used to
 * determine if the player is causing the damage, and a "radius" (see below),
 * which is used to decrease the power of explosions with distance, and a
 * location, via integers which are modified by certain types of attacks
 * (polymorph and teleport being the obvious ones), a default damage, which
 * is modified as needed based on various properties, and finally a "damage
 * type" (see below).
 *:::��Ɂ��ɂ�郂���X�^�[�ւ̃_���[�W�Ɏg���Bradius�i�����j�Ō����B
 *:::rad��dam�̓_���[�W��^���Ȃ��ꕔ�̍U���̃p�����[�^�Ƃ��Ďg���邱�Ƃ�����B
 *
 * Note that this routine can handle "no damage" attacks (like teleport) by
 * taking a "zero" damage, and can even take "parameters" to attacks (like
 * confuse) by accepting a "damage", using it to calculate the effect, and
 * then setting the damage to zero.  Note that the "damage" parameter is
 * divided by the radius, so monsters not at the "epicenter" will not take
 * as much damage (or whatever)...
 *
 * Note that "polymorph" is dangerous, since a failure in "place_monster()"'
 * may result in a dereference of an invalid pointer.  XXX XXX XXX
 *:::�`�F���W�����X�^�[�͊댯�Bplace_monster()�̎��s�̓|�C���^�G���[�̌����ɂȂ�H
 *
 * Various messages are produced, and damage is applied.
 *
 * Just "casting" a substance (i.e. plasma) does not make you immune, you must
 * actually be "made" of that substance, or "breathe" big balls of it.
 *
 * We assume that "Plasma" monsters, and "Plasma" breathers, are immune
 * to plasma.
 *
 * We assume "Nether" is an evil, necromantic force, so it doesn't hurt undead,
 * and hurts evil less.  If can breath nether, then it resists it as well.
 *:::���̃p���[�̓A���f�b�h�ɂ͖����A�׈��ȓG�ɑϐ��B
 *
 * Damage reductions use the following formulas:
 *   Note that "dam = dam * 6 / (randint1(6) + 6);"
 *     gives avg damage of .655, ranging from .858 to .500
 *   Note that "dam = dam * 5 / (randint1(6) + 6);"
 *     gives avg damage of .544, ranging from .714 to .417
 *   Note that "dam = dam * 4 / (randint1(6) + 6);"
 *     gives avg damage of .444, ranging from .556 to .333
 *   Note that "dam = dam * 3 / (randint1(6) + 6);"
 *     gives avg damage of .327, ranging from .427 to .250
 *   Note that "dam = dam * 2 / (randint1(6) + 6);"
 *     gives something simple.
 *
 * In this function, "result" messages are postponed until the end, where
 * the "note" string is appended to the monster name, if not NULL.  So,
 * to make a spell have "no effect" just set "note" to NULL.  You should
 * also set "notice" to FALSE, or the player will learn what the spell does.
 *
 * We attempt to return "TRUE" if the player saw anything "useful" happen.
 */
/* "flg" was added. */
/*:::�����X�^�[�ւ̃_���[�W*/
///mon res magic �����X�^�[�̑����ϐ��A��_�Ȃ�
///sys �_���[�W��\�L�Ƃ�����Ƃ��ύX�v
bool project_m(int who, int r, int y, int x, int dam, int typ, int flg, bool see_s_msg)
{
	int tmp;

	cave_type *c_ptr = &cave[y][x];

	monster_type *m_ptr = &m_list[c_ptr->m_idx];
	monster_type *caster_ptr = (who > 0) ? &m_list[who] : NULL;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char killer[80];

	/* Is the monster "seen"? */
	bool seen = m_ptr->ml;
	bool seen_msg = is_seen(m_ptr);

	bool slept = (bool)MON_CSLEEP(m_ptr);

	/* Were the effects "obvious" (if seen)? */
	bool obvious = FALSE;

	/* Can the player know about this effect? */
	bool known = ((m_ptr->cdis <= MAX_SIGHT) || p_ptr->inside_battle);

	/* Were the effects "irrelevant"? */
	bool skipped = FALSE;

	/* Gets the monster angry at the source of the effect? */
	bool get_angry = FALSE;

	/* Polymorph setting (true or false) */
	bool do_poly = FALSE;

	/* Teleport setting (max distance) */
	int do_dist = 0;

	/* Confusion setting (amount to confuse) */
	int do_conf = 0;

	/* Stunning setting (amount to stun) */
	int do_stun = 0;

	/* Sleep amount (amount to sleep) */
	int do_sleep = 0;

	/* Fear amount (amount to fear) */
	int do_fear = 0;

	/* Time amount (amount to time) */
	int do_time = 0;

	//v1.1.94 �X�e�[�^�X�ُ�ǉ�
	int do_dec_atk = 0;
	int do_dec_def = 0;
	int do_dec_mag = 0;

	//v1.1.95 �ǉ�
	int do_drunk = 0;//��������������
	int do_berserk = 0;
	int do_not_move = 0;



	bool heal_leper = FALSE;

	/* Hold the monster name */
	char m_name[80];

#ifndef JP
	char m_poss[10];
#endif

	int photo = 0;

	/* Assume no note */
	cptr note = NULL;

	/* Assume a default death */
	cptr note_dies = extract_note_dies(real_r_ptr(m_ptr));

	int ty = m_ptr->fy;
	int tx = m_ptr->fx;

	/*:::���̍U���̎g�p�҂̃��x���@���Ȃ��{�ɂȂ�炵��*/
	int caster_lev = (who > 0) ? r_info[caster_ptr->r_idx].level : (p_ptr->lev * 2);

	//v1.1.41 �����X�^�[�������X�^�[��|�����Ƃ��v���C���[�ƃ����X�^�[�̗������o���l�𓾂���t���O�B���Ɨ��T��p
	bool flag_monster_and_player_gain_exp = FALSE;


	/* Nobody here */
	/*:::���̃O���b�h�ɉ������Ȃ���ΏI��*/
	if (!c_ptr->m_idx) return (FALSE);

	/* Never affect projector */
	/*:::�����̎g�p�҂ɂ͌��ʂ��y�ڂ��Ȃ��B��n���̂Ƃ��ɂ��񕜂�����������Č��ʂ��y�ڂ��Ȃ��B*/
	///sys magic ������n�����X�^�[��g�p�҂Ɍ��ʂ��y�ڂ��{�[���Ƃ����Ȃ炱���ɋL�q����
	if (who && (c_ptr->m_idx == who)) return (FALSE);
	if ((c_ptr->m_idx == p_ptr->riding) && !who && !(typ == GF_OLD_HEAL) && !(typ == GF_OLD_SPEED) && !(typ == GF_STAR_HEAL)) return (FALSE);
	///sysdel mon ������i����@�폜�\��
	//if (sukekaku && ((m_ptr->r_idx == MON_SUKE) || (m_ptr->r_idx == MON_KAKU))) return FALSE;

	/* Don't affect already death monsters */
	/* Prevents problems with chain reactions of exploding monsters */
	/*:::���񂾃����X�^�[�ւ͌��ʂ��y�ڂ��Ȃ�*/
	if (m_ptr->hp < 0) return (FALSE);


	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* Get the monster name (BEFORE polymorphing) */
	monster_desc(m_name, m_ptr, 0);

#ifndef JP
	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
#endif

	if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) disturb(1, 1);

	//v1.1.21 ���m������̎ˌ����s�����Ƃ��u�N���e�B�J���v�Ƃ��ĈЗ͂���������锻��
	//�ˌ����s����project_m���ŏ��ɌĂ΂ꂽ�����X�^�[�ɂ̂ݓK�p�����B
	if(hack_gun_fire_critical_flag)
	{
		int chance;
		int tmp2=0;
		tmp = 0;

		if(cheat_xtra) msg_format("flag:%x",hack_gun_fire_critical_flag);
		//�ߐڎˌ�
		if(hack_gun_fire_critical_flag & GUN_CRIT_CLOSE)
		{
			if( m_ptr->cdis == 1)
				tmp += 100;
			else if( m_ptr->cdis == 2)
				tmp += 50;
		}
		//�w�b�h�V���b�g
		if((hack_gun_fire_critical_flag & GUN_CRIT_HEAD))
		{
			if(monster_living(r_ptr)) tmp += 200;
			else tmp += 50;
		}
		//�g���K�[�n�b�s�[
		if((hack_gun_fire_critical_flag & GUN_CRIT_TRIGGERHAPPY))
		{
			tmp += 100;
		}
		//���_�W��
		if((hack_gun_fire_critical_flag & GUN_CRIT_CONCENT))
		{
				tmp += p_ptr->concent * 10;
		}
		//�m�N�g+�ÈŎˌ�
		if((hack_gun_fire_critical_flag & GUN_CRIT_NIGHT))
		{
			if(check_mon_blind(c_ptr->m_idx)) tmp += 100;
		}

		//�����Z�\
		if((hack_gun_fire_critical_flag & GUN_CRIT_CONCEAL))
		{
			tmp += 100;
		}
		//���e�̎ˎ�
		if((hack_gun_fire_critical_flag & GUN_CRIT_MAGICBULLET))
		{
			tmp += 200;
		}
		//�e�e�ύX
		if((hack_gun_fire_critical_flag & GUN_CRIT_SPECIALBULLET))
		{
			switch(special_shooting_mode)
			{

			case SSM_SLAY_ANIMAL:
				if(r_ptr->flags3 & RF3_ANIMAL) tmp += 200; break;
			case SSM_SLAY_NONLIV:
				if(r_ptr->flags3 & RF3_NONLIVING) tmp += 200; break;
			case SSM_SLAY_EVIL:
				if(r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON)) tmp += 200; break;
			case SSM_SLAY_HUMAN:
				if(r_ptr->flags3 & RF3_HUMAN) tmp += 400; break;
			case SSM_SLAY_FLYER:
				if(r_ptr->flags7 & RF7_CAN_FLY) tmp += 200; break;
			case SSM_SILVER:
				if(r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON)) tmp += 400;
				else if(r_ptr->flags3 & RF3_ANG_CHAOS) tmp += 200; 
				break;
			case SSM_HEAVY:
				tmp += 200; break;
			case SSM_SLAY_DRAGON:
				if(r_ptr->flags3 & RF3_DRAGON) tmp += 400; break;
			case SSM_SLAY_DEITY:
				if(r_ptr->flags3 & RF3_DEITY) tmp += 400; break;
			case SSM_NIGHTMARE:
				if(r_ptr->flagsr & RFR_HURT_HELL) tmp += 200; break;
			case SSM_METEOR:
				tmp += 600; break;

			default:
				break;
			}
		}

		if(hack_gun_fire_critical_flag & GUN_CRIT_DEV_5)
			tmp /= 5;
		else if(hack_gun_fire_critical_flag & GUN_CRIT_DEV_3)
			tmp /= 3;
		else if(hack_gun_fire_critical_flag & GUN_CRIT_DEV_2)
			tmp /= 2;

		chance = (tmp + p_ptr->lev * 2 + (p_ptr->to_h_b + hack_gun_fire_critical_hit_bonus) * BTH_PLUS_ADJ);

		if (tmp > 0 && randint1(1000) <= chance)
		{
			tmp2 = randint1(tmp / 3);

			if(tmp2 < tmp / 8) tmp2 = tmp / 8;
			if(tmp2 < 50) tmp2 = 50;

			if(tmp2 < 100)
				msg_print("�育�������������I");
			else if(tmp2 < 200)
				msg_print("���Ȃ�̎育�������������I");
			else if(tmp2 < 300)
				msg_print("��S�̈ꌂ���I");
			else if(tmp2 < 400)
				msg_print("�ō��̉�S�̈ꌂ���I");
			else 
				msg_print("��ނȂ��ō��̉�S�̈ꌂ���I");

			dam += dam * tmp2 / 100;

		}
		if(tmp && cheat_xtra) msg_format("tmp:%d chance:%d tmp2:%d dam:%d",tmp,chance,tmp2,dam);

		hack_gun_fire_critical_flag = 0L;
	}

	//v1.1.21
	//�e�̋Z�\�o���l�𓾂�B
	if(	hack_flag_gain_gun_skill_exp)
	{
		gain_skill_exp(TV_GUN,m_ptr);
		hack_flag_gain_gun_skill_exp = FALSE;
	}


	/* Analyze the damage type */
	switch (typ)
	{
		/* Magic Missile -- pure damage */
		case GF_MISSILE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/* Acid */
		case GF_ACID:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_ACID)
			{
#ifdef JP
note = "�ɂ͂��Ȃ�ϐ�������I";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_ACID);
			}
			break;
		}

		/* Electricity */
		case GF_ELEC:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_IM_ELEC)
			{
#ifdef JP
note = "�ɂ͂��Ȃ�ϐ�������I";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_ELEC);
			}
			else if (r_ptr->flagsr & (RFR_HURT_ELEC))
			{
#ifdef JP
note = "�͂Ђǂ��Ɏ���������B";
#else
				note = " is hit hard.";
#endif

				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ELEC);
			}
			break;
		}

		/* Fire damage */
		case GF_FIRE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.91 �Ζ��n�`�Ή��_���[�W
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);

			if (r_ptr->flagsr & RFR_IM_FIRE)
			{
#ifdef JP
note = "�ɂ͂��Ȃ�ϐ�������I";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);
			}
///mod131231 �����X�^�[�t���O�ύX �Ή���_RF3����RFR��
			else if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
#ifdef JP
note = "�͂Ђǂ��Ɏ���������B";
#else
				note = " is hit hard.";
#endif

				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}
			break;
		}

		/* Cold */
		case GF_COLD:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_COLD && !(WINTER_LETTY2)) //v1.1.85 ���e�B�̑ϐ��ђʏ���
			{


				note = "�ɂ͂��Ȃ�ϐ�������I";
				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_COLD);
			}
///mod131231 �����X�^�[�t���O�ύX ��C��_RF3����RFR��

			else if (r_ptr->flagsr & (RFR_HURT_COLD))
			{
#ifdef JP
note = "�͂Ђǂ��Ɏ���������B";
#else
				note = " is hit hard.";
#endif
				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_COLD);
			}
			break;
		}

		/* Poison */
		case GF_POIS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_POIS)
			{
#ifdef JP
note = "�ɂ͂��Ȃ�ϐ�������I";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_POIS);
			}
			break;
		}
		/*:::���������@��+�� ���ꂼ���1/3�ɂ��Ă���*/
		case GF_POLLUTE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_POIS || r_ptr->flagsr & RFR_RES_DISE )
			{
#ifdef JP
note = "�ɂ͂��Ȃ�ϐ�������I";
#endif
				if(r_ptr->flagsr & RFR_IM_POIS)
				{
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_POIS);
				}
				if(r_ptr->flagsr & RFR_RES_DISE)
				{
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DISE);
				}
			}
			else
			{
				//v1.1.94 �ǂ���̑ϐ����Ȃ���Ζh��͒ቺ����
				if (!mon_saving_throw(m_ptr, dam))
				{
					do_dec_def = 8 + randint1(8);

				}
			}
			break;
		}
		/* Nuclear waste */
		case GF_NUKE:
		{
			int dam_lite = dam / 2 + 1;
			int dam_fire = dam / 2 + 1;
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.91 �Ζ��n�`�Ή��_���[�W
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam_fire = oil_field_damage_mod(dam_fire, y, x);

			if (r_ptr->flagsr & RFR_IM_FIRE || r_ptr->flagsr & RFR_RES_LITE)
			{
				note = "�ɂ͑ϐ�������B";

				if(r_ptr->flagsr & RFR_IM_FIRE)
				{
					dam_fire /= 9;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);
				}
				if(r_ptr->flagsr & RFR_RES_LITE)
				{
					dam_lite = dam_lite * 2 / (randint1(6)+6);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
				}
			}
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				note = "�͂Ђǂ��Ɏ���������B";
				dam_lite *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = "�͂Ђǂ��Ɏ���������B";
				dam_fire *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}

			dam = dam_lite + dam_fire;

			break;
		}
		///mod160110 ���C����
		case GF_STEAM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_IM_FIRE || r_ptr->flagsr & RFR_RES_WATE)
			{
				if (r_ptr->flagsr & RFR_IM_FIRE && r_ptr->flagsr & RFR_RES_WATE)
					note = "�ɂ͂��Ȃ�ϐ�������I";
				else
					note = "�ɂ͑ϐ�������B";

				if(r_ptr->flagsr & RFR_IM_FIRE){
					dam -= dam/3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);
				}
				if(r_ptr->flagsr & RFR_RES_WATE){
					dam -= dam/3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
				}
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = "�͂Ђǂ��Ɏ���������B";

				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}
			if (r_ptr->flagsr & (RFR_HURT_WATER))
			{
				note = "�͂Ђǂ��Ɏ���������B";

				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
			}

			break;
		}
		/* Hellfire -- hurts Evil */
		case GF_REDMAGIC:
		case GF_HELL_FIRE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
///mod131231 �����X�^�[�t���O�ύX �w���t�@�C�A�̓t���O�ɂ���_������
			if (r_ptr->flagsr & RFR_HURT_HELL)
			{
				dam *= 2;
				if(typ == GF_REDMAGIC) note = "�͍̑̂g�����͂ɐN�H���ꂽ�I";
				else note = "�̑̂͒n���̋Ɖ΂ɂ��Ђǂ��Ɏ���󂯂��B";

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HELL);
			}
			break;
		}

///mod131231 �����X�^�[�t���O�ύX �j�׍U���̓t���O�ɂ��ϐ��E��_������
		/* Holy Fire -- hurts Evil, Good are immune, others _resist_ */
		case GF_HOLY_FIRE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_HOLY && r_ptr->flagsr & RFR_HURT_HELL)
			{
				dam = 0;
#ifdef JP
note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_HOLY;
			}
			else if (r_ptr->flagsr & RFR_RES_HOLY)
			{
				dam /= 4;
#ifdef JP
note = "�ɂ͑ϐ�������B";
#else
				note = " is immune.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_HOLY;
			}
			else if (r_ptr->flagsr & RFR_HURT_HOLY)
			{
				dam *= 2;
#ifdef JP
note = "�̑̂͐���ȗ͂ɂ��Ђǂ��Ɏ���󂯂��B";
#else
				note = " is hit hard.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_HURT_HOLY;
			}

			else if(r_ptr->flags3 & RF3_ANG_CHAOS)
			{
#ifdef JP
note = "�ɂ͏������ʂ������悤���B";
#else
				note = " resists.";
#endif

				dam *= 4; dam /= 3;
			}
			break;
		}

		/* Arrow -- XXX no defense */
		/*:::�ˌ��H*/
		case GF_ARROW:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/* Plasma -- XXX perhaps check ELEC or FIRE */
		case GF_PLASMA:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.91 �Ζ��n�`�Ή��_���[�W
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);



			if ((r_ptr->flagsr & RFR_IM_FIRE) || (r_ptr->flagsr & RFR_IM_ELEC))
			{
#ifdef JP
note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif
				if(r_ptr->flagsr & RFR_IM_FIRE){
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);

				}
				if(r_ptr->flagsr & RFR_IM_ELEC){
					dam /= 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_ELEC);
				}
			}
			if (r_ptr->flagsr & (RFR_HURT_ELEC))
			{
				note = "�͂Ђǂ��Ɏ���������B";
				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ELEC);
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = "�͂Ђǂ��Ɏ���������B";
				dam += dam / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}


			break;
		}

		/* Nether -- see above */
		case GF_NETHER:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140222 ���א��͂̓f�t�H�ł͒n���ϐ����������A����Ɉ����������Ƃɂ���
			if (r_ptr->flagsr & RFR_RES_NETH)
			{
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
#ifdef JP
					note = "�ɂ͊��S�ȑϐ�������B";
#else
					note = " is immune.";
#endif

					dam = 0;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
				}
				else
				{
#ifdef JP
					note = "�ɂ͑ϐ�������B";
#else
					note = " resists.";
#endif

					dam *= 3; dam /= randint1(3) + 6;
				}
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_NETH);
			}
			else if (r_ptr->flags3 & RF3_UNDEAD)
			{
				dam /= 4;
#ifdef JP
				note = "�ɂ͂��Ȃ�ϐ�������B";
#else
				note = " resists somewhat.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
			}
			else if (r_ptr->flags3 & RF3_DEMON)
			{
				dam /= 2;
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists somewhat.";
#endif

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
			}
			break;
		}

		//v2.0.2 ���������@���Ɣj�ׂ̕�������
		//���̂Ƃ���v�̉̂́u���̕�����v��p
		case GF_HOLY_WATER:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flagsr & RFR_HURT_HOLY && r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = "�͂Ђǂ��Ɏ���󂯂��I";
				dam *= 3;

				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HOLY);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);

			}
			else if (r_ptr->flagsr & RFR_HURT_HOLY)
			{
				note = "�͐��Ȃ鐅�𗁂тĐg�������߂��B";
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HOLY);
				dam *= 2;
			}
			else if (r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = "�͐������Ȃ悤���B";
				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
			}

			else if (r_ptr->flagsr & (RFR_RES_WATE | RFR_RES_HOLY))
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (r_ptr->flagsr & (RFR_RES_WATE) && is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
				if (r_ptr->flagsr & (RFR_RES_HOLY) && is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_HOLY);

			}

			if (dam > randint1(r_ptr->level * 3) && !(r_ptr->flagsr & (RFR_RES_WATE|RFR_RES_HOLY)) && !(r_ptr->flags3 & RF3_NO_STUN))
				do_stun = (randint1(4) + randint0(dam / 10));

		}
		break;

		/* Water (acid) damage -- Water spirits/elementals are immune */
		case GF_WATER:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mon �������ւ̈ꕔ�����X�^�[���ꏈ��
			if (r_ptr->flagsr & RFR_RES_WATE)
			{
				if ((m_ptr->r_idx == MON_WATER_ELEM) || (m_ptr->r_idx == MON_UNMAKER) || (m_ptr->r_idx == MON_MURASA))
				{
#ifdef JP
					note = "�ɂ͊��S�ȑϐ�������B";
#else
					note = " is immune.";
#endif

					dam = 0;
				}
				else
				{
#ifdef JP
					note = "�ɂ͑ϐ�������B";
#else
					note = " resists.";
#endif

					dam *= 3; dam /= randint1(6) + 6;
				}
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
			}
			else if(r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = "�͐������Ȃ悤���I";
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
				dam *= 2;
			}
			if(dam > randint1(r_ptr->level*3) && !(r_ptr->flagsr & RFR_RES_WATE) && !(r_ptr->flags3 & RF3_NO_STUN))
				do_stun = (randint1(4) + randint0(dam / 10));

			break;
		}
		//�C����������p�@��{�����������n�ɑ�_���[�W
		case GF_MOSES:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mon �������ւ̈ꕔ�����X�^�[���ꏈ��
			else if (r_ptr->flags7 & RF7_AQUATIC)
			{
					note = "�͒n�ʂ̏�ł������Ă���I";
					dam *= 2;
			}
			else if (r_ptr->flagsr & RFR_RES_WATE)
			{
				note = "�ɂ͑ϐ�������B";
				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WATE);
			}
			else if(r_ptr->flagsr & RFR_HURT_WATER)
			{
				note = "�͐������Ȃ悤���I";
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
				dam *= 2;
			}

			if(dam > randint1(r_ptr->level*3) && !(r_ptr->flagsr & RFR_RES_WATE) && !(r_ptr->flags3 & RF3_NO_STUN))
				do_stun = (randint1(4) + randint0(dam / 10));

			break;
		}


		/* Chaos -- Chaos breathers resist */
		case GF_CHAOS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_CHAO)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_CHAO);
			}
			else if ((r_ptr->flags3 & RF3_DEMON) && one_in_(3))
			{
#ifdef JP
				note = "�͂����炩�ϐ����������B";
#else
				note = " resists somewhat.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
			}
			else
			{
				do_poly = TRUE;
				do_conf = (5 + randint1(11) + r) / (r + 1);
			}
			break;
		}


		case GF_TIMED_SHARD:

		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SHAR)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SHAR);
				break;

			}
			else
			{
#ifdef JP
				note = "�Ɋǌϒe��H��킹���B";
#else
				note = " resists.";
#endif
			}
			//10�s���̊ԃ_���[�W��^����
			m_ptr->timed_shard_count = 10;
			get_angry = TRUE;

			dam = 0;

			break;
		}


		/* Shards -- Shard breathers resist */
		case GF_SHARDS:
		case GF_THROW_PLATE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SHAR)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SHAR);
				break;

			}
			//v1.1.64 �n�j���͍����j�Ў�_�ɂ��Ă݂�
			else if (r_ptr->flags3 & RF3_HANIWA)
			{
				note = "�͐g�k�������B";
				dam = dam * 2;

			}

			//v1.1.94 �j�Бϐ����Ȃ��Ƃ��ǉ����ʂōU���͒ቺ(�_���[�W�l�𔻒�p���[�Ƃ��ăZ�[�r���O�X���[)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(dam/10);
			}

			break;
		}

		/* Rocket: Shard resistance helps */
		case GF_ROCKET:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SHAR)
			{
#ifdef JP
				note = "�͂����炩�ϐ����������B";
#else
				note = " resists somewhat.";
#endif

				dam /= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SHAR);
				break;
			}
			//v1.1.64 �n�j���͍����j�Ў�_�ɂ��Ă݂�
			else if (r_ptr->flags3 & RF3_HANIWA)
			{
				note = "�͐g�k�������B";
				dam = dam * 2;

			}

			//v1.1.94 �j�Бϐ����Ȃ��Ƃ��ǉ����ʂōU���͒ቺ(�_���[�W�l�𔻒�p���[�Ƃ��ăZ�[�r���O�X���[)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(dam / 10);
			}

			break;
		}


		/* Sound -- Sound breathers resist */
		case GF_SOUND:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_SOUN)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_SOUN);
			}
			//v1.1.64 �n�j���͍�����_�ɂ��Ă݂�
			else if (r_ptr->flags3 & RF3_HANIWA)
			{
				note = "�͐g�k�������B";
				dam *= 2;
				do_stun = (20 + randint1(30) + r) / (r + 1);
			}
			else 
				do_stun = (10 + randint1(15) + r) / (r + 1);

			break;
		}

		/* Confusion */
		case GF_CONFUSION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags3 & RF3_NO_CONF)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
			}
			else
			{
				do_conf = (10 + randint1(15) + r) / (r + 1);
				do_dec_mag = 8 + randint1(dam/10);
			}
			break;
		}

		/* Disenchantment -- Breathers and Disenchanters resist */
		case GF_DISENCHANT:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_DISE)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DISE);
				break;
			}

			//v1.1.94 �򉻑ϐ����Ȃ��Ƃ��ǉ����ʂŖh��͒ቺ(�_���[�W�l�𔻒�p���[�Ƃ��ăZ�[�r���O�X���[)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_def = 8 + randint1(dam/10);
			}



			break;
		}

		/* Nexus -- Breathers and Existers resist */
		case GF_NEXUS:
		case GF_SEIRAN_BEAM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140914 �����X�^�[�̈��ʍ����ϐ��@�����ύX
			//�ϐ����Ȃ���HP�ő�l������������ł�����e���|����悤�ύX
			if (r_ptr->flagsr & RFR_RES_TIME)
			{
				note = "�͂����炩�ϐ����������B";

				dam *= 4; dam /= randint1(3) + 5;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_TIME);
				break;
			}
			else if(typ == GF_NEXUS)
			{
				do_time = (dam + 1) / 2;
				//���E�����@�_���[�W����⃁�b�Z�[�W����̂���player_cast��FALSE�ɂ��ČĂ�
				if ((dam <= m_ptr->hp) && (r_ptr->level < randint1(200)) && genocide_aux(c_ptr->m_idx, 200, FALSE, (r_ptr->level + 1) / 2, "�����X�^�[����"))
				{
					if (seen_msg) msg_format("%s�͂��̃t���A�ɂ��Ȃ��������ƂɂȂ����B", m_name);
					return TRUE;
				}
				else if((dam <= m_ptr->hp) && !(r_ptr->flagsr & RFR_RES_TELE))
				{
					do_dist = randint1(200);
					if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
				}

				//v1.1.94 ���ʍ����̒ǉ����ʂŔ���Ȃ��̑S�\�͒ቺ
				do_dec_atk = 16 + randint1(dam / 10);
				do_dec_def = 16 + randint1(dam / 10);
				do_dec_mag = 16 + randint1(dam / 10);

			}
			break;

		}

		/* Force */
		case GF_FORCE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod131231 �t�H�[�X�ϐ��폜�@NO_STUN������ΞN�O�Ƃ��Ȃ����Ƃɂ���
/*
			if (r_ptr->flagsr & RFR_RES_WALL)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_WALL);
			}
*/
			if (!(r_ptr->flags3 & RF3_NO_STUN)) do_stun = (randint1(15) + r) / (r + 1);
			break;
		}

		/* Inertia -- breathers resist */
		/*:::�x�ݑ���*/
		case GF_INACT:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_INER)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_INER);
			}
			else
			{
				/* Powerful monsters can resist */
				//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				//v1.1.90 ����ύX
				if (mon_saving_throw(m_ptr, dam))
				{
					obvious = FALSE;
				}
				/* Normal monsters slow down */
				else
				{
					if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8)))
					{
#ifdef JP
						note = "�̓������x���Ȃ����B";
#else
						note = " starts moving slower.";
#endif
					}
				}
			}
			break;
		}

		/* Time -- breathers resist */
		case GF_TIME:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_TIME)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_TIME);
			}
			else
			{
				do_time = (dam + 1) / 2;

				//v1.1.94 ���ԋt�]�̒ǉ����ʂőS�\�͒ቺ(�v�Z�[�r���O�X���[)
				if (!mon_saving_throw(m_ptr, dam))
				{
					do_dec_atk = 8 + randint1(dam / 10);
					do_dec_def = 8 + randint1(dam / 10);
					do_dec_mag = 8 + randint1(dam / 10);
				}

			}
			break;
		}
		///mod ��Ԙc�ȑ��� �ϐ��Ȃ��ƃe���|
		case GF_DISTORTION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flagsr & RFR_RES_TIME)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_TIME);
			}
			else if(!(r_ptr->flagsr & RFR_RES_TELE))
			{
				//v1.1.55 ���Z��ŋ�Ԙc�Ȃ�����Ƃ������X�^�[��������H�̂��m��񂪔O�̈גl���������Ƃ�
				if (p_ptr->inside_battle) 
					do_dist = 10;
				else
					do_dist = randint1(100);

				if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
			}
			break;
		}

		/* Gravity -- breathers resist */
		//�����X�^�[�ւ̏d�͑���
		case GF_GRAVITY:
		{
			bool resist_tele = FALSE;

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_TELE)
			{
				if (r_ptr->flags1 & (RF1_UNIQUE))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "�͓�������Ȃ������B";
#else
					note = " is unaffected!";
#endif

					resist_tele = TRUE;
				}
				else if (r_ptr->level > randint1(100))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "�ɂ͑ϐ�������I";
#else
					note = " resists!";
#endif

					resist_tele = TRUE;
				}
			}

			if (!resist_tele) do_dist = 10;
			else do_dist = 0;
			if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;

			if (r_ptr->flagsr & RFR_RES_TIME || r_ptr->flags7 & RF7_CAN_FLY)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif

				dam *= 3; dam /= randint1(6) + 6;
				do_dist = 0;
				//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_GRAV);
			}
			else
			{
				/*:::����ϐ�����s���Ȃ�����ȃ����X�^�[�ɂ͏d�͂��悭�������Ƃɂ��Ă݂�*/
				if(r_ptr->flags2 & (RF2_GIGANTIC)){
					msg_format("%^s�̋���ȑ̂��������n�ʂɒ@������ꂽ�I", m_name);			
					dam *= 2;
				}

				/* 1. slowness */
				/* Powerful monsters can resist */
				//v1.1.90 ����ύX
				//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))

				if (mon_saving_throw(m_ptr, dam))
				{
					obvious = FALSE;
				}
				/* Normal monsters slow down */
				else
				{
					if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4)))
					{
#ifdef JP
						note = "�̓������x���Ȃ����B";
#else
						note = " starts moving slower.";
#endif
					}
				}

				/* 2. stun */
				do_stun = damroll((caster_lev / 20) + 3 , (dam)) + 1;

				/* Attempt a saving throw */
				//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				if (mon_saving_throw(m_ptr, dam))
				{
					/* Resist */
					do_stun = 0;
					/* No obvious effect */
#ifdef JP
					note = "�ɂ͌��ʂ��Ȃ������B";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}



			}
			break;
		}

		///mod140103 ���������@����ȃ����X�^�[�ɂ͌����Ȃ��@���V�Ń_���[�W�y��
		//�e���|�ϐ��͊֌W�Ȃ��ɂ���B�������ł��������Ă邱�Ƃ����B
		case GF_TORNADO:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_GIGANTIC)
			{
#ifdef JP
				note = "�̋���ȑ̂ɂ͌��ʂ������悤���B";
#endif
				dam /= 8;
				break;
			}
				
			do_dist = 10;
			if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
			
			if (r_ptr->flags7 & RF7_CAN_FLY)
			{
				note = "�͐�����΂��ꂽ���󒆂Ńo�����X��������B";
				dam  /= (randint1(3) + 1);
			}
			else note = "�͐�����΂��ꂽ�I";

			break;
		}

		///mod14502 �d�ԑ����@�G���e���|�ϐ��������Đ�����΂�������ȓG�͔�΂Ȃ�
		case GF_TRAIN:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_GIGANTIC)
			{
#ifdef JP
				if(p_ptr->pclass == CLASS_SHINMYOU_2)
					note = "�͏d���Ēނ�グ���Ȃ������B";
				else
					note = "�̋���ȑ̂͐�����΂Ȃ������B";
#endif
				break;
			}
			else if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;				
			else
			{
				if(p_ptr->pclass == CLASS_SHINMYOU_2)
					note = "��ނ�グ���I";
				else
					note = "�͂ǂ����֒��˔�΂��ꂽ�I";

				do_dist = 10 + randint1(20);
			}
			break;
		}

		/* Pure damage */
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/*:::���e�B�̃m�[�U���E�B�i�[*/
		case GF_NORTHERN:
		{
			int msleep = MON_CSLEEP(m_ptr);
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			else if(r_ptr->flagsr & RFR_IM_COLD && !WINTER_LETTY2)//v1.1.85 ���e�B�ϐ��ђʏ���
			{
				note = "�͂����炩�ϐ����������B";
				dam = dam * 3 / (5 + randint1(4));
			}

			if (msleep)
			{
				dam *= 2;
			}


			if(msleep && m_ptr->hp >= dam && 
				( ((r_ptr->flagsr & RFR_IM_COLD) || !monster_living(r_ptr) || (r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) ) &&  (r_ptr->level > randint1(msleep / 3 + p_ptr->lev * 2))  || (r_ptr->level > randint1(msleep + p_ptr->lev * 5))  ) )
			{
				note = "���N�����Ă��܂����I";
			}  
			
			else if(msleep)
			{
				if(msleep < 2) msleep = 2;
				note = "�͂܂������Ă���E�E";
				do_sleep = msleep / 2 + randint1(msleep / 2);
			}
			note_dies = "�͓�����A�����Ȃ��Ȃ����B";

			break;
		}


		/* Pure damage */
		case GF_DISINTEGRATE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod131231 �����X�^�[�t���O�ύX ��Ηd����_RF3����RFR��
			if (r_ptr->flagsr & RFR_HURT_ROCK)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ROCK);
#ifdef JP
note = "�̔畆�������ꂽ�I";
note_dies = "�͏��������I";
#else
				note = " loses some skin!";
				note_dies = " evaporates!";
#endif

				dam *= 2;
			}
			break;
		}
		/*:::���_�U��*/
		case GF_PSI:
		{
			if (seen) obvious = TRUE;

			/* PSI only works if the monster can see you! -- RG */
			if (!(los(m_ptr->fy, m_ptr->fx, py, px)))
			{
#ifdef JP
				if (seen_msg) msg_format("%s�͂��Ȃ��������Ȃ��̂ŉe������Ȃ��I", m_name);
#else
				if (seen_msg) msg_format("%^s can't see you, and isn't affected!", m_name);
#endif
				skipped = TRUE;
				break;
			}

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				dam = 0;
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune!";
#endif
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;

			}
			else if ((r_ptr->flags2 & (RF2_STUPID | RF2_WEIRD_MIND)) ||
			         (r_ptr->flags3 & RF3_ANIMAL) ||
			         (r_ptr->level > randint1(3 * dam)))
			{
				dam /= 3;
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif


				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if ((r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) &&
				    (r_ptr->level > p_ptr->lev / 2) &&
				    one_in_(2))
				{
					note = NULL;
#ifdef JP
					msg_format("%^s�̑��������_�͍U���𒵂˕Ԃ����I", m_name);
#else
					msg_format("%^s%s corrupted mind backlashes your attack!",
					    m_name, (seen ? "'s" : "s"));
#endif

					/* Saving throw */
					if ((randint0(100 + r_ptr->level / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
					{
#ifdef JP
						msg_print("���������͂𒵂˕Ԃ����I");
#else
						msg_print("You resist the effects!");
#endif

					}
					else
					{
						/* Injure +/- confusion */
						monster_desc(killer, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
						take_hit(DAMAGE_ATTACK, dam, killer, -1);  /* has already been /3 */
						if (one_in_(4) && !CHECK_MULTISHADOW())
						{
							switch (randint1(4))
							{
								case 1:
									set_confused(p_ptr->confused + 3 + randint1(dam));
									break;
								case 2:
									set_stun(p_ptr->stun + randint1(dam));
									break;
								case 3:
								{
									if (r_ptr->flags3 & RF3_NO_FEAR)
#ifdef JP
										note = "�ɂ͌��ʂ��Ȃ������B";
#else
										note = " is unaffected.";
#endif

									else
										set_afraid(p_ptr->afraid + 3 + randint1(dam));
									break;
								}
								default:
									if (!p_ptr->free_act)
										(void)set_paralyzed(p_ptr->paralyzed + randint1(dam));
									break;
							}
						}
					}
					dam = 0;
				}
				break;
			}

			//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
			do_dec_mag = 8 + randint1(dam / 10);

			if ((dam > 0) && one_in_(4))
			{
				switch (randint1(4))
				{
					case 1:
						do_conf = 3 + randint1(dam);
						break;
					case 2:
						do_stun = 3 + randint1(dam);
						break;
					case 3:
						do_fear = 3 + randint1(dam);
						break;
					default:
#ifdef JP
						note = "�͖��荞��ł��܂����I";
#else
						note = " falls asleep!";
#endif

						do_sleep = 3 + randint1(dam);
						break;
				}
			}

#ifdef JP
			///msg dead
			//note_dies = "�̐��_�͕��󂵁A���͔̂����k�ƂȂ����B";
			note_dies = "�͓|�ꂽ�B";
#else
			note_dies = " collapses, a mindless husk.";
#endif

			break;
		}

		case GF_PSI_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				dam = 0;
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune!";
#endif
				break;
			}
			/*
			* Powerful demons & undead can turn a mindcrafter's
			* attacks back on them
			*/
			else if ((r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) &&
				(r_ptr->level > p_ptr->lev / 2) && (one_in_(2)))
			{
				note = NULL;
				msg_format("%^s�̑��������_�͍U���𒵂˕Ԃ����I", m_name);

				/* Saving throw */
				if ((randint0(100 + r_ptr->level / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
				{
					msg_print("���Ȃ��͌��͂𒵂˕Ԃ����I");
				}
				else
				{
					/* Injure + mana drain */
					monster_desc(killer, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
					if (!CHECK_MULTISHADOW())
					{
						msg_print("�t�ɖ��͂��z���Ƃ��Ă��܂����I");

						p_ptr->csp -= dam;
						if (p_ptr->csp < 0) p_ptr->csp = 0;
						p_ptr->redraw |= PR_MANA;
						p_ptr->window |= (PW_SPELL);
					}
					take_hit(DAMAGE_ATTACK, dam / 3, killer, -1); 
				}
				dam = 0;
				break;
			}
			else if (r_ptr->flags2 & (RF2_STUPID | RF2_WEIRD_MIND))
			{
				dam /= 3;
				note = "�ɂ͑ϐ�������B";
				break;
			}
			//���@��S�������Ȃ������X�^�[��MP�������Ȃ��Ɖ��߂��AMP���z���ł��Ȃ��B
			else if (!HAS_ANY_MAGIC(r_ptr))
			{
				note = "�͖��͂������Ȃ��悤���B";
			}
			else if(r_ptr->level > randint1(2 * dam))
			{
				dam /= 2;
				note = "�͍U���ɒ�R�����I";
				break;
			}
			else if (dam > 0)
			{
				int b = (dam + 1) / 2 + randint1(dam + 1) / 2;
				msg_format("%s���疂�͂��z��������I", m_name);

				player_gain_mana(b);

			}
			note_dies = "�͓|�ꂽ�B";

			//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
			do_dec_mag = 8 + randint1(dam / 10);


		}
		break;
		case GF_TELEKINESIS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (one_in_(4))
			{
				if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_dist = 0;
				else do_dist = 7;
			}

			/* 1. stun */
			do_stun = damroll((caster_lev / 20) + 3 , dam) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->level > 5 + randint1(dam)))
			{
				/* Resist */
				do_stun = 0;
				/* No obvious effect */
				obvious = FALSE;
			}
			break;
		}

		case GF_BLAZINGSTAR:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			do_dist = 100;
			break;
		}

		/* Psycho-spear -- powerful magic missile */
		///mod150305 �O���O�j���ǉ�
		case GF_GUNGNIR:
		case GF_PSY_SPEAR:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		/* Meteor -- powerful magic missile */
		case GF_METEOR:
		case GF_KANAMEISHI:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}

		case GF_DOMINATION:
		{
			if (!is_hostile(m_ptr)) break;

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) ||
			    (r_ptr->flags3 & RF3_NO_CONF) ||
			    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/*
				 * Powerful demons & undead can turn a mindcrafter's
				 * attacks back on them
				 */
				if ((r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) &&
				    (r_ptr->level > p_ptr->lev / 2) &&
				    (one_in_(2)))
				{
					note = NULL;
#ifdef JP
					msg_format("%^s�̑��������_�͍U���𒵂˕Ԃ����I", m_name);
#else
					msg_format("%^s%s corrupted mind backlashes your attack!",
					    m_name, (seen ? "'s" : "s"));
#endif

					/* Saving throw */
					if (randint0(100 + r_ptr->level/2) < p_ptr->skill_sav)
					{
#ifdef JP
						msg_print("���������͂𒵂˕Ԃ����I");
#else
						msg_print("You resist the effects!");
#endif

					}
					else
					{
						/* Confuse, stun, terrify */
						switch (randint1(4))
						{
							case 1:
								set_stun(p_ptr->stun + dam / 2);
								break;
							case 2:
								set_confused(p_ptr->confused + dam / 2);
								break;
							default:
							{
								if (r_ptr->flags3 & RF3_NO_FEAR)
#ifdef JP
									note = "�ɂ͌��ʂ��Ȃ������B";
#else
									note = " is unaffected.";
#endif

								else
									set_afraid(p_ptr->afraid + dam);
							}
						}
					}
				}
				else
				{
					/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
			}
			else
			{
				if ((dam > 29) && (randint1(100) < dam))
				{
#ifdef JP
					///sys�@�ꑮ���]����
note = "�͂��Ȃ��ɏ]�����B";
#else
					note = " is in your thrall!";
#endif
					if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -2);
					set_pet(m_ptr);
				}
				else
				{
					switch (randint1(4))
					{
						case 1:
							do_stun = dam / 2;
							break;
						case 2:
							do_conf = dam / 2;
							break;
						default:
							do_fear = dam;
					}
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}



		/* Ice -- Cold + Cuts + Stun */
		case GF_ICE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			do_stun = (randint1(15) + 1) / (r + 1);
			if (r_ptr->flagsr & RFR_IM_COLD && !(WINTER_LETTY2))//v1.1.85 ���e�B�ϐ��ђʏ���
			{
#ifdef JP
				note = "�ɂ͂��Ȃ�ϐ�������B";
#else
				note = " resists a lot.";
#endif

				dam /= 9;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_COLD);
			}
///mod131231 �����X�^�[�t���O�ύX ��C��_RF3����RFR��

			else if (r_ptr->flagsr & (RFR_HURT_COLD))
			{
#ifdef JP
				note = "�͂Ђǂ��Ɏ���������B";
#else
				note = " is hit hard.";
#endif

				dam *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_COLD);
			}
			break;
		}


		/* Drain Life */
		case GF_OLD_DRAIN:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (!monster_living(r_ptr))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}

#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				dam = 0;
			}
			else do_time = (dam+7)/8;

			break;
		}

		/* Death Ray */
		case GF_DEATH_RAY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (!monster_living(r_ptr))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}

#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������B";
#else
				note = " is immune.";
#endif

				obvious = FALSE;
				dam = 0;
			}
			else if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2))
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
			}
			//v1.1.60�Ԕ؊�̓��Ɏ��̌����ϐ��t�^
			else if (m_ptr->r_idx == MON_BANKI_HEAD_1 || (m_ptr->r_idx == MON_BANKI_HEAD_2))
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
			}

/*
			else if (((r_ptr->flags1 & RF1_UNIQUE) &&
				 (randint1(888) != 666)) ||
				 (((r_ptr->level + randint1(20)) > randint1((caster_lev / 2) + randint1(10))) &&
				 randint1(100) != 66))
 */
			else if (randint0(randint1(r_ptr->level/7)+5))
			{
				note = "�͎��̌����ɒ�R�����I";

				obvious = FALSE;
				dam = 0;
			}

			break;
		}

		/* Polymorph monster (Use "dam" as "power") */
		///mod140211 �`�F���W�����X�^�[�̓J�I�X�ϐ������ɂ͌����Ȃ�����
		case GF_OLD_POLY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt to polymorph (see below) */
			do_poly = TRUE;

			/* Powerful monsters can resist */
			if ((r_ptr->flags1 & RF1_UNIQUE) ||
			    (r_ptr->flags1 & RF1_QUESTOR) ||
			    (r_ptr->flagsr & RFR_RES_CHAO) ||
			    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������B";
#else
				note = " is unaffected!";
#endif

				do_poly = FALSE;
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Clone monsters (Ignore "dam") */
		case GF_OLD_CLONE:
		{
			if (seen) obvious = TRUE;

			if ((p_ptr->inside_arena) || is_pet(m_ptr) || (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) || (r_ptr->flags7 & (RF7_NAZGUL | RF7_UNIQUE2)))
			{
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������B";
#else
 note = " is unaffected!";
#endif
			}
			else
			{
				/* Heal fully */
				m_ptr->hp = m_ptr->maxhp;

				/* Attempt to clone. */
				if (multiply_monster(c_ptr->m_idx, TRUE, 0L))
				{
#ifdef JP
					///sys �N���[�������X�^�[�@���b�Z�[�W�ς���H
note = "�����􂵂��I";
#else
					note = " spawns!";
#endif

				}
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Heal Monster (use "dam" as amount of healing) */
		case GF_STAR_HEAL:
		{
			if (seen) obvious = TRUE;

			/* Wake up */
			(void)set_monster_csleep(c_ptr->m_idx, 0);

			if (m_ptr->maxhp < m_ptr->max_maxhp)
			{
#ifdef JP
				if (seen_msg) msg_format("%^s�̋������߂����B", m_name);
#else
				if (seen_msg) msg_format("%^s recovers %s vitality.", m_name, m_poss);
#endif
				m_ptr->maxhp = m_ptr->max_maxhp;
			}

			if (!dam)
			{
				/* Redraw (later) if needed */
				if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);
				if (p_ptr->riding == c_ptr->m_idx) p_ptr->redraw |= (PR_UHEALTH);
				break;
			}

			/* Fall through */
		}
		case GF_OLD_HEAL:
		{
			if (seen) obvious = TRUE;

			/* Wake up */
			(void)set_monster_csleep(c_ptr->m_idx, 0);
			if (MON_STUNNED(m_ptr))
			{
#ifdef JP
				if (seen_msg) msg_format("%^s�͞N�O��Ԃ��痧���������B", m_name);
#else
				if (seen_msg) msg_format("%^s is no longer stunned.", m_name);
#endif
				(void)set_monster_stunned(c_ptr->m_idx, 0);
			}
			if (MON_CONFUSED(m_ptr))
			{
#ifdef JP
				if (seen_msg) msg_format("%^s�͍������痧���������B", m_name);
#else
				if (seen_msg) msg_format("%^s is no longer confused.", m_name);
#endif
				(void)set_monster_confused(c_ptr->m_idx, 0);
			}
			if (MON_MONFEAR(m_ptr))
			{
#ifdef JP
				if (seen_msg) msg_format("%^s�͗E�C�����߂����B", m_name);
#else
				if (seen_msg) msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif
				(void)set_monster_monfear(c_ptr->m_idx, 0);
			}

			/* Heal */
			///sys monHP30000�����H
			if (m_ptr->hp < 30000) m_ptr->hp += dam;

			/* No overflow */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
///del131231 virtue
#if 0
			///sysdel mon virtue
			if (!who)
			{
				chg_virtue(V_VITALITY, 1);

				if (r_ptr->flags1 & RF1_UNIQUE)
					chg_virtue(V_INDIVIDUALISM, 1);

				if (is_friendly(m_ptr))
					chg_virtue(V_HONOUR, 1);
				else if (!(r_ptr->flags3 & RF3_EVIL))
				{
					if (r_ptr->flags3 & RF3_GOOD)
						chg_virtue(V_COMPASSION, 2);
					else
						chg_virtue(V_COMPASSION, 1);
				}

				if (r_ptr->flags3 & RF3_ANIMAL)
					chg_virtue(V_NATURE, 1);
			}

			if (m_ptr->r_idx == MON_LEPER)
			{
				heal_leper = TRUE;
				if (!who) chg_virtue(V_COMPASSION, 5);
			}
#endif
			/* Redraw (later) if needed */
			if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == c_ptr->m_idx) p_ptr->redraw |= (PR_UHEALTH);

			/* Message */
#ifdef JP
			note = "�̗͑͂��񕜂����悤���B";
#else
			note = " looks healthier.";
#endif

			//��ǐ_�l�@�z����F�D�����X�^�[��HP�����ȉ�����񕜂�����Ɩ����Ȃǂ𓾂�
			if(p_ptr->prace == RACE_STRAYGOD && !(m_ptr->mflag & MFLAG_SPECIAL2) && (m_ptr->hp < m_ptr->max_maxhp / 2))
			{
				m_ptr->mflag |= MFLAG_SPECIAL2;
				if(is_pet(m_ptr))
				{
					set_deity_bias(DBIAS_WARLIKE, -1);
				}
				else if(is_friendly(m_ptr))
				{
					set_deity_bias(DBIAS_REPUTATION, 2);
				}
			}


			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Speed Monster (Ignore "dam") */
		case GF_OLD_SPEED:
		{
			if (seen) obvious = TRUE;

			/* Speed up */
			if (set_monster_fast(c_ptr->m_idx, MON_FAST(m_ptr) + 100))
			{
#ifdef JP
				note = "�̓����������Ȃ����B";
#else
				note = " starts moving faster.";
#endif
			}

			if (!who)
			{
				if (r_ptr->flags1 & RF1_UNIQUE)
					chg_virtue(V_INDIVIDUALISM, 1);
				if (is_friendly(m_ptr))
					chg_virtue(V_HONOUR, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Slow Monster (Use "dam" as "power") */
		case GF_OLD_SLOW:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			//v1.1.90 ����ύX
			/* Powerful monsters can resist */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))

			if (r_ptr->flagsr & (RFR_RES_INER) || mon_saving_throw(m_ptr, dam))
			{
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}

			/* Normal monsters slow down */
			else
			{
				if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8)))
				{
#ifdef JP
					note = "�̓������x���Ȃ����B";
#else
					note = " starts moving slower.";
#endif
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}
		
		/*:::�����߂Ȃǂ̃X���E�����X�^�[�@�n�`�⃂���X�^�[�̎�ނɂ�薳������*/
		case GF_SLOW_TWINE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if ((r_ptr->flags2 & RF2_PASS_WALL) || (r_ptr->flags7 & RF7_CAN_FLY) )
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				break;
			}

			if(r_ptr->flags1 & RF1_UNIQUE) dam /= 2;
			if(r_ptr->flags2 & RF2_KILL_WALL) dam /= 2;
			if(r_ptr->flags8 & RF8_WILD_WOOD) dam/= 2;
			if(r_ptr->flags2 & RF2_POWERFUL) dam/= 2;
			if(r_ptr->flags2 & RF2_GIGANTIC)dam/=2;

			if(r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
			{
#ifdef JP
				note = "�͞g��U��������I";
#endif
				obvious = FALSE;
			}
			else
			{
				if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8)))
				{
#ifdef JP
					note = "�̓������x���Ȃ����B";
#endif
				}
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Sleep (Use "dam" as "power") */
		case GF_OLD_SLEEP:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    (r_ptr->flags3 & RF3_NO_SLEEP) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 ����ύX
			if (r_ptr->flags3 & (RF3_NO_SLEEP) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_SLEEP)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
				}

				/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
#ifdef JP
note = "�͖��荞��ł��܂����I";
#else
				note = " falls asleep!";
#endif

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Sleep (Use "dam" as "power") */
		/*:::�����i�j�ח̈�j*/
		case GF_STASIS_EVIL:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    !(r_ptr->flags3 & RF3_ANG_CHAOS) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 ����ύX
			if (!(r_ptr->flags3 & RF3_ANG_CHAOS) || mon_saving_throw(m_ptr, dam))
			{
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
#ifdef JP
note = "�͓����Ȃ��Ȃ����I";
#else
				note = " is suspended!";
#endif

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Sleep (Use "dam" as "power") */
		/*:::�_�ЁA���f�A���ĂȂǂ̃����X�^�[��~����*/
		case GF_STASIS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 ����ύX
			if (mon_saving_throw(m_ptr, dam))
			{
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */

				note = "�͓����Ȃ��Ȃ����I";

				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/*:::�h���S���q��S*/
		case GF_STASIS_DRAGON:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (!(r_ptr->flags3 & RF3_DRAGON))
			{
				obvious = FALSE;
			}
			else if (r_ptr->flags1 & RF1_UNIQUE)
			{
				note = "�͖���Ȃ������I";
				obvious = FALSE;
			}
			else
			{
				note = "�͂��̏�ɂ������܂�A����n�߂��B";
				do_sleep = 100;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		//v1.1.34 ������������@�d���ɖ������Ō���
		case GF_STASIS_LIVING:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (!monster_living(r_ptr))
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
			}
			else if (r_ptr->flags3 & RF3_FAIRY)
			{
				note = "�̓������~�܂����B";
				do_sleep = 500;
				(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8));
			}
			else
			{
				//v1.1.90 ����ύX
				if (mon_saving_throw(m_ptr, dam))
				{
					note = "�ɂ͌��ʂ��Ȃ������I";
				}
				else
				{
					if(!(r_ptr->flagsr & RFR_RES_INER))
					{
						set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 8 + randint1(8));
					}
					do_sleep = 100+randint1(dam);
					note = "�͓����Ȃ��Ȃ����I";

				}

			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Charm monster */
		case GF_CHARM:
		case GF_CHARM_PALADIN:
		{
			//int vir;
			dam += (adj_chr_chm[p_ptr->stat_ind[A_CHR]] - 1);
			//vir = virtue_number(V_HARMONY);
			//if (vir)
			//{
			//	dam += p_ptr->virtues[vir-1]/10;
			//}

			//vir = virtue_number(V_INDIVIDUALISM);
			//if (vir)
			//{
			//	dam -= p_ptr->virtues[vir-1]/20;
			//}

			///mod140817 �z���ɂ̓`���[���������Ȃ����Ƃɂ���
			if(is_pet(m_ptr)) return (FALSE);

			if (seen) obvious = TRUE;

			if(typ == GF_CHARM_PALADIN && (r_ptr->flags3 & RF3_ANG_CHAOS))
			{
				note = "�͐����Ɏ���݂��Ȃ��B";
				dam = 0;
				break;
			}
			if(typ == GF_CHARM_PALADIN && (m_ptr->mflag2 & MFLAG2_NOPET))
			{
				note = "�͐����ɉ����Ȃ��B";
				dam = 0;
				break;
			}
			else if(typ == GF_CHARM_PALADIN && (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND)))
			{
				note = "�͖��������B";
				dam = 0;
				break;
			}

			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}


			//v2.0.2 �������𑼂̃o�X�e�n�ɑ������BGF_CHARM�������ϐ��̂���G�Ɍ����Ȃ��̂͏]�����l
			//if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
			//	dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_QUESTOR) ||
			    (r_ptr->flags3 & RF3_NO_CONF) && typ == GF_CHARM ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				mon_saving_throw(m_ptr, dam)   )
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
#ifdef JP
				if(typ == GF_CHARM_PALADIN)
				note = "�̐����Ɏ��s�����B";
				else note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "�͂��Ȃ��ɓG�ӂ�����Ă���I";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
				if(typ == GF_CHARM_PALADIN)
				note = "�̐����ɐ��������I";

				else note = "�͓ˑR�F�D�I�ɂȂ����悤���I";
#else
				note = " suddenly seems friendly!";
#endif

				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
				set_pet(m_ptr);

				//chg_virtue(V_INDIVIDUALISM, -1);
				//if (r_ptr->flags3 & RF3_ANIMAL)
				//	chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Control undead */
		case GF_CONTROL_UNDEAD:
		{
			int vir;
			if (seen) obvious = TRUE;
			/*
			vir = virtue_number(V_UNLIFE);
			if (vir)
			{
				dam += p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_QUESTOR) ||
			  (!(r_ptr->flags3 & RF3_UNDEAD)) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "�͂��Ȃ��ɓG�ӂ�����Ă���I";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
				///sys �ꑮ���b�Z�[�W�ύX�\��
note = "�͂��Ȃ��ɕ��]�����B";
#else
				note = " is in your thrall!";
#endif

				if(r_ptr->level+10 > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
				set_pet(m_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Control demon */
		case GF_CONTROL_DEMON:
		{
			int vir;
			if (seen) obvious = TRUE;
			/*
			vir = virtue_number(V_UNLIFE);
			if (vir)
			{
				dam += p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & RF1_QUESTOR) ||
			  (!(r_ptr->flags3 & RF3_DEMON)) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "�͂��Ȃ��ɓG�ӂ�����Ă���I";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
			///sys �ꑮ���b�Z�[�W�ύX�\��
note = "�͂��Ȃ��ɕ��]�����B";
#else
				note = " is in your thrall!";
#endif

				if(r_ptr->level+10 > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
				set_pet(m_ptr);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		case GF_CHARM_C:
		case GF_CHARM_J:
		case GF_CHARM_B:
		case GF_CONTROL_FISH:
		{

			if (seen) obvious = TRUE;
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) 
				|| (m_ptr->mflag2 & MFLAG2_NOPET)

				|| typ == GF_CONTROL_FISH && !(r_ptr->flags7 & RF7_AQUATIC)

				|| typ == GF_CHARM_C && r_ptr->d_char != 'C'
				|| typ == GF_CHARM_B && r_ptr->d_char != 'B'
				|| typ == GF_CHARM_J && r_ptr->d_char != 'J')
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (r_ptr->level > randint1(dam))
			{
				note = "�ɂ͌��ʂ��Ȃ������I";

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
				note = "�͂��Ȃ��ɓG�ӂ�����Ă���I";

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
				note = "�͂Ȃ����B";
				set_pet(m_ptr);
				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}
		case GF_CONTROL_ANIMAL:
		{
			int vir;

			if (seen) obvious = TRUE;
			/*
			vir = virtue_number(V_NATURE);
			if (vir)
			{
				dam += p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_QUESTOR)) ||
			  (!(r_ptr->flags3 & (RF3_ANIMAL))) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 (r_ptr->flags3 & (RF3_NO_CONF)) ||
				 (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "�͂��Ȃ��ɓG�ӂ�����Ă���I";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
note = "�͂Ȃ����B";
#else
				note = " is tamed!";
#endif

				set_pet(m_ptr);
				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);

				//if (r_ptr->flags3 & RF3_ANIMAL)
				//	chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Tame animal */
		case GF_CONTROL_LIVING:
		{
			int vir;

			//vir = virtue_number(V_UNLIFE);
			if (seen) obvious = TRUE;

			dam += (adj_chr_chm[p_ptr->stat_ind[A_CHR]]);
			/*
			vir = virtue_number(V_UNLIFE);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/10;
			}

			vir = virtue_number(V_INDIVIDUALISM);
			if (vir)
			{
				dam -= p_ptr->virtues[vir-1]/20;
			}
			*/
			if (r_ptr->flags3 & (RF3_NO_CONF)) dam -= 30;
			if (dam < 1) dam = 1;
#ifdef JP
msg_format("%s�����߂��B",m_name);
#else
			msg_format("You stare into %s.", m_name);
#endif
			if ((r_ptr->flagsr & RFR_RES_ALL) || p_ptr->inside_arena)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_NAZGUL))
				dam = dam * 2 / 3;

			/* Attempt a saving throw */
			if ((r_ptr->flags1 & (RF1_QUESTOR)) ||
			    (m_ptr->mflag2 & MFLAG2_NOPET) ||
				 !monster_living(r_ptr) ||
				 ((r_ptr->level+10) > randint1(dam)))
			{
				/* Resist */
				/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else if (p_ptr->cursed & TRC_AGGRAVATE)
			{
#ifdef JP
note = "�͂��Ȃ��ɓG�ӂ�����Ă���I";
#else
				note = " hates you too much!";
#endif

				if (one_in_(4)) m_ptr->mflag2 |= MFLAG2_NOPET;
			}
			else
			{
#ifdef JP
		///sys �ꑮ���b�Z�[�W�ύX�\��
note = "�͂��Ȃ��ɕ��]�����B";
#else
				note = " is tamed!";
#endif

				set_pet(m_ptr);
				if(r_ptr->level > randint1(p_ptr->lev)) set_deity_bias(DBIAS_WARLIKE, -1);

				//if (r_ptr->flags3 & RF3_ANIMAL)
				//	chg_virtue(V_NATURE, 1);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Confusion (Use "dam" as "power") */
		case GF_OLD_CONF:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Get confused later */
			do_conf = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
			//    (r_ptr->flags3 & (RF3_NO_CONF)) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 ����ύX
			if (r_ptr->flags3 & (RF3_NO_CONF) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}

				/* Resist */
				do_conf = 0;

				/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}
			else
			{
				//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
				do_dec_mag = 8 + randint1(8);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		case GF_STUN:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			do_stun = damroll((caster_lev / 20) + 3 , (dam)) + 1;

			/* Attempt a saving throw */
			//v1.1.90 ����ύX
			//if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			if(r_ptr->flags3 & (RF3_NO_STUN) || mon_saving_throw(m_ptr,dam))
			{
				/* Resist */
				do_stun = 0;

				/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}




		/* Lite, but only hurts susceptible creatures */
		case GF_LITE_WEAK:
		{
			if (!dam)
			{
				skipped = TRUE;
				break;
			}
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				dam = 0;
				break;
			}
			/* Hurt by light */
///mod131231 �����X�^�[�t���O�ύX �M����_RF3����RFR��
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);

				/* Special effect */
#ifdef JP
		///sysdel dead
note = "�͌��ɐg�������߂��I";
note_dies = "�͌����󂯂ē|�ꂽ�I";
#else
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
#endif

			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}



		/* Lite -- opposite of Dark */
		case GF_LITE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_LITE)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������I";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= (randint1(6)+6);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
			}
			///mod131231 �����X�^�[�t���O�ύX �M����_RF3����RFR��
			else if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
#ifdef JP
						///sysdel dead
				note = "�͌��ɐg�������߂��I";
				note_dies = "�͌����󂯂ē|�ꂽ�I";
#else
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
#endif

				dam *= 2;
			}
			break;
		}


		/* Dark -- opposite of Lite */
		/*:::�f�[�����͈Í��U���ɑϐ������悤�ɂ��Ă���*/
		case GF_DARK:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flagsr & RFR_RES_DARK)
			{
#ifdef JP
				note = "�ɂ͑ϐ�������I";
#else
				note = " resists.";
#endif

				dam *= 2; dam /= (randint1(6)+6);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DARK);
			}
			if (r_ptr->flags3 & RF3_DEMON)
			{
#ifdef JP
				note = "�͂����炩�ϐ����������B";
#else
				note = " resists.";
#endif

				dam /= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
			}


			break;
		}


		/* Stone to Mud */
		case GF_KILL_WALL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				dam = 0;
				break;
			}
			/* Hurt by rock remover */
			///mod131231 �����X�^�[�t���O�ύX ��Ηd����_RF3����RFR��
			if (r_ptr->flagsr & (RFR_HURT_ROCK))
			{
				/* Notice effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ROCK);

				/* Cute little message */
#ifdef JP
			///sysdel dead
note = "�̔畆�������ꂽ�I";
note_dies = "�͗n�����I";
#else
				note = " loses some skin!";
				note_dies = " dissolves!";
#endif

			}

			/* Usually, ignore the effects */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Teleport undead (Use "dam" as "power") */
		case GF_AWAY_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				bool resists_tele = FALSE;

				if (r_ptr->flagsr & RFR_RES_TELE)
				{
					if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
						note = " is unaffected!";
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "�ɂ͑ϐ�������I";
#else
						note = " resists!";
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport evil (Use "dam" as "power") */
		case GF_AWAY_EVIL:
		{
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_ANG_CHAOS))
			{
				bool resists_tele = FALSE;

				if (r_ptr->flagsr & RFR_RES_TELE)
				{
					if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
						note = " is unaffected!";
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
note = "�ɂ͑ϐ�������I";
#else
						note = " resists!";
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (seen) obvious = TRUE;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_CHAOS);
					do_dist = dam;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport monster (Use "dam" as "power") */
		case GF_AWAY_ALL:
		{
			bool resists_tele = FALSE;
			if (r_ptr->flagsr & RFR_RES_TELE)
			{
				if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
					note = "�ɂ͌��ʂ��Ȃ������I";
#else
					note = " is unaffected!";
#endif

					resists_tele = TRUE;
				}
				else if (r_ptr->level > randint1(100))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
					note = "�ɂ͑ϐ�������I";
#else
					note = " resists!";
#endif

					resists_tele = TRUE;
				}
			}

			if (!resists_tele)
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Prepare to teleport */
				do_dist = dam;

				if(!who && r_ptr->level > randint1(500)) set_deity_bias(DBIAS_WARLIKE, -1);

			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn undead (Use "dam" as "power") */
		///magic �A���f�b�h�ގU�@�����𑦎����ʂɕύX���悤���H
		case GF_TURN_UNDEAD:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				break;
			}
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				//if (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				//v1.1.90 ����ύX
				if (r_ptr->flags3 & (RF3_NO_FEAR) || mon_saving_throw(m_ptr, dam))
				{
					/* No obvious effect */
#ifdef JP
					note = "�ɂ͌��ʂ��Ȃ������I";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
					do_fear = 0;
				}
				else
				{
					//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
					do_dec_mag = 8 + randint1(8);

				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn evil (Use "dam" as "power") */
		case GF_TURN_EVIL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				break;
			}
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_ANG_CHAOS))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_CHAOS);

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				//if (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				//v1.1.90 ����ύX
				if (r_ptr->flags3 & (RF3_NO_FEAR) || mon_saving_throw(m_ptr, dam))
				{
					/* No obvious effect */
#ifdef JP
					note = "�ɂ͌��ʂ��Ȃ������I";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
					do_fear = 0;
				}
				else
				{
					//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
					do_dec_mag = 8 + randint1(8);

				}

			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn monster (Use "dam" as "power") */
		case GF_TURN_ALL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				break;
			}
			/* Obvious */
			if (seen) obvious = TRUE;

			/*:::Hack �����̈Ј������@���Q�����X�^�[�̖��@�_�ȂǑS�����͂ɂȂ��Ă��܂����܂�������*/
			if(p_ptr->pclass == CLASS_KOMACHI && (r_ptr->flags3 & RF3_UNDEAD || monster_living(r_ptr))) dam *= 2;

			/* Apply some fear */
			do_fear = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
			//    (r_ptr->flags3 & (RF3_NO_FEAR)) ||
			//    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
			//v1.1.90 ����ύX
			if (r_ptr->flags3 & (RF3_NO_FEAR) || mon_saving_throw(m_ptr, dam))
			{
				/* No obvious effect */
#ifdef JP
note = "�͋��|���Ȃ������B";
#else
				note = " is unaffected!";
#endif

				obvious = FALSE;
				do_fear = 0;
			}
			else
			{
				//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
				do_dec_mag = 8 + randint1(8);

			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Dispel undead */
		case GF_DISP_UNDEAD:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect undead */
			if (r_ptr->flags3 & (RF3_UNDEAD))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_UNDEAD);
				if(randint1(r_ptr->level) < caster_lev) do_stun = randint0(4) + 4;

				/* Message */
#ifdef JP
			///sysdel dead
				note = "�͐g�k�������B";
				note_dies = "�͓|�ꂽ�B";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}


		/* Dispel evil */
		///mod131231 ���א��͂�Ώۂɂ��Ă���
		case GF_DISP_EVIL:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect evil */
			if (r_ptr->flags3 & (RF3_ANG_CHAOS))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_CHAOS);

				/* Message */
#ifdef JP
			///sysdel dead

				note = "�͐g�k�������B";
				note_dies = "�͓|�ꂽ�B";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel good */
		///mod131231 �������͂�Ώۂɂ��Ă���
		case GF_DISP_GOOD:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect good */
			if (r_ptr->flags3 & (RF3_ANG_COSMOS))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANG_COSMOS);

				/* Message */
#ifdef JP
						///sysdel dead
				note = "�͐g�k�������B";
				note_dies = "�͓|�ꂽ�B";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel living */
		case GF_DISP_LIVING:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect non-undead */
			if (monster_living(r_ptr))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
#ifdef JP
				///sysdel dead
note = "�͐g�k�������B";
note_dies = "�͓|�ꂽ�B";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif

			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel demons */
		case GF_DISP_DEMON:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect demons */
			if (r_ptr->flags3 & (RF3_DEMON))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMON);
				if(randint1(r_ptr->level) < caster_lev) do_stun = randint0(4) + 4;


				/* Message */
#ifdef JP
					///sysdel dead
				note = "�͐g�k�������B";
				note_dies = "�͓|�ꂽ�B";
#else
				note = " shudders.";
				note_dies = " dissolves!";
#endif
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}
		/* Dispel demons */
		case GF_DISP_KWAI:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Only affect demons */
			if (r_ptr->flags3 & (RF3_KWAI))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Learn about type */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_KWAI);
				if(randint1(r_ptr->level) < caster_lev) do_stun = randint0(4) + 4;
				note = "�͐g�k�������B";
				note_dies = "�͓|�ꂽ�B";
			}

			else
			{
				skipped = TRUE;
				dam = 0;
			}

			break;
		}

		/* Dispel monster */
		/*:::�͂̏�Ȃ�*/
		case GF_DISP_ALL:
		case GF_HOUTOU:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				skipped = TRUE;
				dam = 0;
				break;
			}
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Message */
#ifdef JP
			///sysdel dead
note = "�͐g�k�������B";
note_dies = "�͓|�ꂽ�B";
#else
			note = " shudders.";
			note_dies = " dissolves!";
#endif


			break;
		}

		/* Drain mana */
		case GF_DRAIN_MANA:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///sys monspell RF4-6flag ���@���g�������X�^�[�����͋z�����g���Ƃ��̔���H
			if (HAS_ANY_MAGIC(r_ptr))
			{
				if (who > 0)
				{
					/* Heal the monster */
					if (caster_ptr->hp < caster_ptr->maxhp)
					{
						/* Heal */
						caster_ptr->hp += 6 * dam;
						if (caster_ptr->hp > caster_ptr->maxhp) caster_ptr->hp = caster_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == who) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == who) p_ptr->redraw |= (PR_UHEALTH);

						/* Special message */
						if (see_s_msg)
						{
							/* Get the monster name */
							monster_desc(killer, caster_ptr, 0);
#ifdef JP
							msg_format("%^s�͋C�����ǂ��������B", killer);
#else
							msg_format("%^s appears healthier.", killer);
#endif
						}
					}
				}
				else
				{
					/* Message */
#ifdef JP
					msg_format("%s���疂�͂��z���Ƃ����B", m_name);
					//msg_format("%s���琸�_�G�l���M�[���z���Ƃ����B", m_name);
#else
					msg_format("You draw psychic energy from %s.", m_name);
#endif
					if(p_ptr->csp < p_ptr->msp) msg_print("���������n�b�L�������B");
					p_ptr->csp += dam;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= (PR_MANA);
				}
			}
			else
			{
#ifdef JP
			//	if (see_s_msg) msg_format("%s�ɂ͌��ʂ��Ȃ������B", m_name);
				if (see_s_msg) msg_format("%s�͖��͂������Ȃ��悤���B", m_name);
#else
				if (see_s_msg) msg_format("%s is unaffected.", m_name);
#endif
			}
			dam = 0;
			break;
		}

		/* Mind blast */
		//v1.1.90 BRAIN_SMASH�Ɠ�������
		case GF_MIND_BLAST:
		case GF_BRAIN_SMASH:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP

			//if (!who) msg_format("%s���������ɂ񂾁B", m_name);
#else
			if (!who) msg_format("You gaze intently at %s.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}


			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune!";
#endif
				dam = 0;
			}
			/* Attempt a saving throw */
			//if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//	 (r_ptr->flags3 & RF3_NO_CONF) ||
			//	 (r_ptr->level > randint1((caster_lev - 10) < 1 ? 1 : (caster_lev - 10)) + 10))
			//v1.1.90 ����ύX
			else if (r_ptr->flags3 & (RF3_NO_CONF) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}
#ifdef JP
				note = "�ɒ�R���ꂽ�I";
#else
				note = "is unaffected!";
#endif
				dam = 0;
				break;
			}
			else if (r_ptr->flags2 & RF2_WEIRD_MIND)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_WEIRD_MIND);
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif
				dam /= 3;
				break;
			}
			else
			{
#ifdef JP
			///sysdel dead
				note = "�͐��_�U����H������B";
				note_dies = "�͓|�ꂽ�B";
#else
				note = " is blasted by psionic energy.";
				note_dies = " collapses, a mindless husk.";
#endif

				if (typ == GF_BRAIN_SMASH)
				{
					do_conf = randint0(8) + 8;
					do_stun = randint0(8) + 8;
					(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4));
				}
				else
				{
					do_conf = randint0(4) + 4;

				}

				//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
				do_dec_mag = 8 + randint1(dam / 10);

			}
			break;
		}

		/* CAUSE_1 */
		case GF_CAUSE_1:
		{
			if (seen) obvious = TRUE;

			if(!who && p_ptr->pclass == CLASS_BENBEN)
				msg_format("%s�ɉ��삪�P�����������I", m_name);

			/* Message */
#ifdef JP
			//if (!who) msg_format("%s���w�����Ď􂢂��������B", m_name);
#else
			if (!who) msg_format("You point at %s and curse.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 �􂢌n�͖������ɂ͌����Ȃ�����
			else if(!monster_living(r_ptr))
			{
				note = "�ɂ͌��ʂ��Ȃ������B";
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "�͎􂢂ɒ�R�����B";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}

		/* CAUSE_2 */
		case GF_CAUSE_2:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			//if (!who) msg_format("%s���w�����ċ��낵���Ɏ􂢂��������B", m_name);
#else
			if (!who) msg_format("You point at %s and curse horribly.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 �􂢌n�͖������ɂ͌����Ȃ�����
			else if(!monster_living(r_ptr))
			{
				note = "�ɂ͌��ʂ��Ȃ������B";
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "�͎􂢂ɒ�R�����B";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}

		/* CAUSE_3 */
		case GF_CAUSE_3:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			//if (!who) msg_format("%s���w�����A���낵���Ɏ������������I", m_name);
#else
			if (!who) msg_format("You point at %s, incanting terribly!", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 �􂢌n�͖������ɂ͌����Ȃ�����
			else if(!monster_living(r_ptr))
			{
				note = "�ɂ͌��ʂ��Ȃ������B";
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "�͎􂢂ɒ�R�����B";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}

		/* CAUSE_4 */
		case GF_CAUSE_4:
		{
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			///sys magic ��E���b�Z�[�W�ύX
			//if (!who) msg_format("%s�Ɍ����Ď��̌����������B", m_name);
#else
			if (!who) msg_format("You point at %s, screaming the word, 'DIE!'.", m_name);
#endif

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			///mod140321 �􂢌n�͖������ɂ͌����Ȃ�����
			else if(!monster_living(r_ptr))
			{
				note = "�ɂ͌��ʂ��Ȃ������B";
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}
			/* Attempt a saving throw */
			///mon �P���V���E���ꏈ��
			if ((randint0(100 + (caster_lev / 2)) < (r_ptr->level + 35)) && ((who <= 0) || (caster_ptr->r_idx != MON_KENSHIROU)))
			{
#ifdef JP
				note = "�͎􂢂ɒ�R�����B";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}

		///mod140107 �t��������U���̓G�ւ̈З́@�ꉞ�j�ł̎�Ɠ����ɂ��Ă���
		///mod150609 ���j�[�N���낤�ƕK���ɂ��Ă݂�
		case GF_KYUTTOSHITEDOKA_N:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else
			{
				dam = ((40 + randint1(20)) * m_ptr->hp) / 100;

				if (m_ptr->hp < dam) dam = m_ptr->hp - 1;
			}
			break;
		}
		/* HAND_DOOM */
		case GF_HAND_DOOM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags3 & RF3_HANIWA)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			else
			{
				if ((who > 0) ? ((caster_lev + randint1(dam)) > (r_ptr->level + 10 + randint1(20))) :
	//			   (((caster_lev / 2) + randint1(dam)) > (r_ptr->level + randint1(200))))
				   (((caster_lev ) + randint1(dam)) > (r_ptr->level  + randint1(120))))
				{
					dam = ((40 + randint1(20)) * m_ptr->hp) / 100;

					if (m_ptr->hp < dam) dam = m_ptr->hp - 1;
				}
				else
				{
#ifdef JP
					note = "�͔j�łɑ΂���R�����I";
#else
					note = "resists!";
#endif
					dam = 0;
				}
			}
			break;
		}
		
		case GF_COSMIC_HORROR:
		{
			int chance;
			if (seen) obvious = TRUE;
			chance = randint1(100 + (caster_lev * 2));

			if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) chance /= 2;
			if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) ) chance = chance * 2 / 3;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/* Attempt a saving throw */
			else if (r_ptr->flags2 & (RF2_ELDRITCH_HORROR | RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				note = "�ɂ͌��ʂ��Ȃ������B";
				dam = 0;
			}
			else if (chance < r_ptr->level)
			{
				note = "�͍U���ɒ�R�����B";
				dam = 0;
			}
			else
			{
				do_stun = dam / 8 + randint1(dam / 4);
				do_conf = dam / 8 + randint1(dam / 4);
				do_fear = dam / 8 + randint1(dam / 4);

				//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
				do_dec_mag = 16 + randint1(dam / 8);
			}

			break;
		}



		/* Capture monster */
		case GF_CAPTURE:
		{
			int nokori_hp;
			if ((p_ptr->inside_quest && (quest[p_ptr->inside_quest].type == QUEST_TYPE_KILL_ALL) && !is_pet(m_ptr)) ||
			    (r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flags7 & (RF7_NAZGUL)) || (r_ptr->flags7 & (RF7_VARIABLE)) 
				|| (r_ptr->flags7 & (RF7_UNIQUE2)) || (r_ptr->flags1 & RF1_QUESTOR) || (m_ptr->mflag & MFLAG_EPHEMERA) 
				|| m_ptr->parent_m_idx || (r_ptr->flags7 & (RF7_FORCE_GO_STRAIGHT)))
			{
#ifdef JP
				msg_format("%s�ɂ͌��ʂ��Ȃ������B",m_name);
#else
				msg_format("%^s is unaffected.", m_name);
#endif
				skipped = TRUE;
				break;
			}

			if (is_pet(m_ptr)) nokori_hp = m_ptr->maxhp * 4L;
			///class ���b�g���̓����X�^�[�{�[��������
			else if ((p_ptr->pclass == CLASS_BEASTMASTER) && monster_living(r_ptr))
				nokori_hp = m_ptr->maxhp * 3 / 10;
			else
				nokori_hp = m_ptr->maxhp * 3 / 20;

			if (m_ptr->hp >= nokori_hp)
			{
#ifdef JP
				msg_format("�����Ǝ�点�Ȃ��ƁB");
#else
				msg_format("You need to weaken %s more.", m_name);
#endif
				skipped = TRUE;
			}
			else if (m_ptr->hp < randint0(nokori_hp))
			{
				if (m_ptr->mflag2 & MFLAG2_CHAMELEON) choose_new_monster(c_ptr->m_idx, FALSE, MON_CHAMELEON);
#ifdef JP
				msg_format("%s��߂����I",m_name);
#else
				msg_format("You capture %^s!", m_name);
#endif
				cap_mon = m_ptr->r_idx;
				cap_mspeed = m_ptr->mspeed;
				cap_hp = m_ptr->hp;
				cap_maxhp = m_ptr->max_maxhp;
				cap_nickname = m_ptr->nickname; /* Quark transfer */
				/*:::��n���Ƀ����X�^�[�{�[���𔽎˂��ꂽ�痎�n����炵��*/
				if (c_ptr->m_idx == p_ptr->riding)
				{
					if (rakuba(-1, FALSE))
					{
#ifdef JP
						msg_print("�n�ʂɗ��Ƃ��ꂽ�B");
#else
						msg_format("You have fallen from %s.", m_name);
#endif
					}
				}

				delete_monster_idx(c_ptr->m_idx);

				return (TRUE);
			}
			else
			{
#ifdef JP
msg_format("���܂��߂܂����Ȃ������B");
#else
				msg_format("You failed to capture %s.", m_name);
#endif
				skipped = TRUE;
			}
			break;
		}

		/* Attack (Use "dam" as attack type) */
		/*:::�ʏ�U������ �K�E���Ȃǁ@dam�ɍU���̎�ނ�����*/
		case GF_ATTACK:
		{
			/* Return this monster's death */
			return py_attack(y, x, dam);
		}
		case GF_SOULSCULPTURE:
		{
			/* Return this monster's death */
			return py_attack(y, x, HISSATSU_SOULSCULPTURE);
		}
		case GF_YOUMU:
		{
			/* Return this monster's death */
			return py_attack(y, x, HISSATSU_NO_AURA);
		}


		/* Sleep (Use "dam" as "power") */
		case GF_ENGETSU:
		{
			int effect = 0;
			bool done = TRUE;

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags2 & RF2_EMPTY_MIND)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune!";
#endif
				dam = 0;
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;
			}
			if (MON_CSLEEP(m_ptr))
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune!";
#endif
				dam = 0;
				skipped = TRUE;
				break;
			}

			if (one_in_(5)) effect = 1;
			else if (one_in_(4)) effect = 2;
			else if (one_in_(3)) effect = 3;
			else done = FALSE;

			if (effect == 1)
			{
				/* Powerful monsters can resist */
				if ((r_ptr->flags1 & RF1_UNIQUE) ||
				    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
#ifdef JP
					note = "�ɂ͌��ʂ��Ȃ������I";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}

				/* Normal monsters slow down */
				else
				{
					if (set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4)))
					{
#ifdef JP
						note = "�̓������x���Ȃ����B";
#else
						note = " starts moving slower.";
#endif
					}
				}
			}

			else if (effect == 2)
			{
				do_stun = damroll((p_ptr->lev / 10) + 3 , (dam)) + 1;

				/* Attempt a saving throw */
				if ((r_ptr->flags1 & (RF1_UNIQUE)) ||
				    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					/* Resist */
					do_stun = 0;

					/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
			}

			else if (effect == 3)
			{
				/* Attempt a saving throw */
				if ((r_ptr->flags1 & RF1_UNIQUE) ||
				    (r_ptr->flags3 & RF3_NO_SLEEP) ||
				    (r_ptr->level > randint1((dam - 10) < 1 ? 1 : (dam - 10)) + 10))
				{
					/* Memorize a flag */
					if (r_ptr->flags3 & RF3_NO_SLEEP)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_SLEEP);
					}

					/* No obvious effect */
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
					note = " is unaffected!";
#endif

					obvious = FALSE;
				}
				else
				{
					/* Go to sleep (much) later */
#ifdef JP
note = "�͖��荞��ł��܂����I";
#else
					note = " falls asleep!";
#endif

					do_sleep = 500;
				}
			}

			if (!done)
			{
#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune!";
#endif
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* GENOCIDE */
		/*:::�����X�^�[����*/
		case GF_GENOCIDE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

#ifdef JP
			if (genocide_aux(c_ptr->m_idx, dam, !who, (r_ptr->level + 1) / 2, "�����X�^�[����"))
#else
			if (genocide_aux(c_ptr->m_idx, dam, !who, (r_ptr->level + 1) / 2, "Genocide One"))
#endif
			{
#ifdef JP
				///sysdel dead
				if (seen_msg) msg_format("%s�͂ǂ����֏������B", m_name);
#else
				if (seen_msg) msg_format("%^s disappered!", m_name);
#endif
				//chg_virtue(V_VITALITY, -1);
				return TRUE;
			}

			skipped = TRUE;
			break;
		}
		/*:::�ʐ^�B�e*/
		case GF_PHOTO:
		{
#ifdef JP
			if (!who) msg_format("%s���ʐ^�ɎB�����B", m_name);
#else
			if (!who) msg_format("You take a photograph of %s.", m_name);
#endif
			/* Hurt by light */
			///mod131231 �����X�^�[�t���O�ύX �M����_RF3����RFR��
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);

				/* Special effect */
#ifdef JP
				///sysdel dead
				note = "�͌��ɐg�������߂��I";
				note_dies = "�͌����󂯂ē|�ꂽ�I";
#else
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
#endif
			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			photo = m_ptr->r_idx;

			break;
		}


		/* blood curse */
		case GF_BLOOD_CURSE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//v1.1.94 �ǉ����ʂŔ\�͒ቺ(�_���[�W�l�𔻒�p���[�Ƃ��ăZ�[�r���O�X���[)
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(8);
				do_dec_def = 8 + randint1(8);
				do_dec_mag = 8 + randint1(8);
			}

			break;
		}

		//v1.1.66 ���|�u�s�s�̖��s���c�v�p�ɏ�������
		case GF_CRUSADE:
		{
			if (seen) obvious = TRUE;

			if ((r_ptr->flags3 & (RF3_HANIWA)) && is_pet(m_ptr))
			{
				note = "�̓����������Ȃ����B";

				(void)set_monster_fast(c_ptr->m_idx, MON_FAST(m_ptr) + 100);

			}

			else 
			{
				if (!(r_ptr->flags3 & RF3_NO_FEAR))
				{
					do_fear = randint1(90)+10;
				}
				else if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_FEAR);
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/*:::���ɂ��y���̎􂢂Ȃ�*/
		case GF_WOUNDS:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			/* Attempt a saving throw */
			if (randint0(100 + dam) < (r_ptr->level + 50))
			{

#ifdef JP
note = "�ɂ͌��ʂ��Ȃ������B";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			break;
		}


		/* PUNISH_1/2/3/4 �j�׎�_�̓G�ɂ��������Ȃ��B��������G�ɞN�O�A���|�t�^�����݂�B*/
		case GF_PUNISH_1:
		case GF_PUNISH_2:
		case GF_PUNISH_3:
		case GF_PUNISH_4:
		{
			int power;
			if (seen) obvious = TRUE;
			/* Message */
#ifdef JP
			//if (!who) msg_format("%s���w�����Ė��������������B", m_name);
#endif
			if(typ == GF_PUNISH_1) power = 100;
			else if(typ == GF_PUNISH_2) power = 120;
			else if(typ == GF_PUNISH_3) power = 140;
			else power = 160;


			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			/*:::�j�׎�_�̓G�ɂ��������Ȃ�*/
			else if (!(r_ptr->flagsr & RFR_HURT_HOLY))
			{
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������B";
#endif
				dam = 0;
			}

			/* Attempt a saving throw */
			else if (randint0(power + (caster_lev / 2)) < (r_ptr->level + 35))
			{
#ifdef JP
				note = "�͎����ɒ�R�����B";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}

			else
			{
				dam *= 2;

				if (!(r_ptr->flags3 & RF3_NO_FEAR)) do_fear = damroll(3, (dam / 2)) + 1;
				if (!(r_ptr->flags3 & RF3_NO_STUN) && (typ == GF_PUNISH_3 || typ == GF_PUNISH_4) ) do_stun = damroll(3, (dam / 2)) + 1;

				//v1.1.94 �ǉ����ʂŔ\�͒ቺ
				if (one_in_(3))		
					do_dec_atk = 8 + randint1(8);
				else if (one_in_(2))
					do_dec_def = 8 + randint1(8);
				else
					do_dec_mag = 8 + randint1(8);
			}

			break;
		}

		/* ����̈�̉���߈˂Ȃ� */
		//v1.1.95 ����m�����ʂɂ���
		/*
		case GF_POSSESSION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			if (r_ptr->flags3 & RF3_HANIWA)//v1.1.64
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				break;
			}


			if(r_ptr->flagsr & RFR_RES_NETH) dam /= 3;
			if(r_ptr->flags1 & RF1_UNIQUE) dam /= 2;
			if(r_ptr->flags3 & RF3_UNDEAD) dam /= 3;
			if(r_ptr->flags3 & RF3_DEMON) dam /= 3;
			if(r_ptr->flags3 & RF3_NONLIVING) dam /= 2;
			if(r_ptr->flags2 & RF2_POWERFUL) dam /= 2;
			if(r_ptr->flags3 & RF3_KWAI) dam *= 2;

			if(randint1(dam/2) + dam/2 < r_ptr->level)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
			}
			else
			{
				msg_format("%s�ɉ��삪���߂����I", m_name);
				do_stun = dam / 8 + randint1(dam / 4);
				do_conf = dam / 8 + randint1(dam / 4);
				do_fear = dam / 8 + randint1(dam / 4);

				(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 16 + randint1(16));

				dam = 0;
			}


			break;
		}
		*/

		/* ����̈�̃\�E���X�e�B�[�� */
		case GF_SOULSTEAL:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if(!monster_living(r_ptr))
			{
				note = "�ɂ͌��ʂ��Ȃ������B";
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}

			else if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) ||(r_ptr->flags1 & RF1_QUESTOR))
			{
				note = "�ɂ͌����Ă��Ȃ��悤���B";
				dam = 0;
				break;
			}
			//v1.1.65�@�ϐ��͏d�����Ȃ��悤�ɂ���
			if (r_ptr->flagsr & RFR_RES_NETH) dam /= 5;
			else if(r_ptr->flags2 & RF2_POWERFUL) dam /= 2;
			else if(r_ptr->flags2 & RF2_SMART) dam /= 2;

			if (randint0(dam) < (r_ptr->level))
			{
#ifdef JP
				note = "�͎����ɒ�R�����I";
#else
				note = "is unaffected!";
#endif
				dam = 0;
			}
			else
			{
				note_dies = "�̐����͂�D��������I";
				dam = m_ptr->hp + 1;
				hp_player(dam);
			}





			break;
		}



		/*:::�E�B���h�J�b�^�[��p�@�GAC�ɏ������m���ŉ�������*/
		case GF_WINDCUTTER:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if(randint1(r_ptr->ac ) > (p_ptr->lev + 25)) //v1.1.96
			{
				note = "�ɂ͓�����Ȃ������B";
				dam = 0;
			}

			break;
		}


		/*::: ���C�����@�E�h���Q�Ȃǁ@���j�[�N�Ɍ������˂���Ȃ�������̓G�Ɍ��ʂ������������̐��_�U�� */
		case GF_REDEYE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags2 & RF2_ELDRITCH_HORROR)
			{
				//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
#ifdef JP
				note = "�ɂ͂��Ȃ�ϐ�������I";
#else
				note = " is immune!";
#endif
				dam /= 9;
				break;
			}
			else if (r_ptr->flags2 & RF2_WEIRD_MIND)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_WEIRD_MIND);
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif
				dam /= 3;
				break;
			}
			//else if ((r_ptr->flags1 & RF1_UNIQUE) ||
			//	 (r_ptr->flags3 & RF3_NO_CONF) ||
			//	 (r_ptr->level > randint1((caster_lev - 10) < 1 ? 1 : (caster_lev - 10)) + 80))
			//v1.1.90 ����ύX
			else if (r_ptr->flags3 & (RF3_NO_CONF) || r_ptr->flags3 & (RF3_NONLIVING) || mon_saving_throw(m_ptr, dam))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CONF))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}
				note = "�͍U���ɒ�R�����B";
				dam /= 2;
				break;
			}
			else
			{
#ifdef JP
				//note = "�͐��_�U����H������B";
				note_dies = "�͓|�ꂽ�B";
#else
				note = " is blasted by psionic energy.";
				note_dies = " collapses, a mindless husk.";
#endif

				do_conf = randint0(8) + 8;
				if (caster_lev > 29)
				{
					do_stun = randint0(8) + 8;
					(void)set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4));
				}

				//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
				do_dec_mag = 8 + randint1(dam / 10);
			}
			break;
		}


		/*::: ���Ƃ��p�@�A���f�b�h�Ɍ����₷�����_�U�� */
		case GF_SATORI:
		{
			bool ok = TRUE;
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				skipped = TRUE;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & RF2_EMPTY_MIND || r_ptr->flags2 & RF2_WEIRD_MIND)
			{
				//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				note = "�͖��������B";
				dam = 0;
				ok = FALSE;
				break;
			}
			else if (r_ptr->flags2 & RF2_ELDRITCH_HORROR)
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_ELDRITCH_HORROR);
				note = "�ɂ͑ϐ�������B";
				dam /= 9;
				ok = FALSE;
			}
			else if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags2 & RF2_POWERFUL)
			{
				note = "�͐��_�U���ɂ�����x��R�����B";
				dam /= 2;
				ok = FALSE;
			}

			if(r_ptr->flags3 & RF3_UNDEAD)
			{
				note = "�͌������ꂵ�݂������I";
				dam *= 2;
			}

			note_dies = "�͓|�ꂽ�B";

			if (ok && !(r_ptr->flags3 & RF3_NO_FEAR) && ((r_ptr->flags3 & RF3_UNDEAD) || randint1(r_ptr->level) < caster_lev)) 
				do_fear = damroll(3, (dam / 2)) + 1;

			if (ok && !(r_ptr->flags3 & RF3_NO_CONF) && ((r_ptr->flags3 & RF3_UNDEAD) || randint1(r_ptr->level) < caster_lev) )
				do_conf = randint0(8) + 8;

			if (ok && !mon_saving_throw(m_ptr, dam))
			{
				do_dec_atk = 8 + randint1(dam / 8);
			}

			break;
		}
		//v1.1.43 �N���E���s�[�X�u���C�̏����v����
		case GF_LUNATIC_TORCH:
		{
			//����t���O�t�^����Ə���
			lunatic_torch(c_ptr->m_idx, dam);

			dam = 0;
			break;
		}
		//���̖�U���@��R����Ȃ������Ƃ������
		case GF_YAKU:
		{
			int sav;

			if(p_ptr->pclass != CLASS_HINA)
			{
				msg_print("ERROR:���ȊO��GF_YAKU�����U�����g��ꂽ");
				dam=0;
				break;
			}

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			sav = r_ptr->level + randint1(30);

			if (r_ptr->flagsr & RFR_RES_NETH)
			{
				sav *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_NETH);
			}
			if (r_ptr->flagsr & RFR_RES_DARK)
			{
				sav *= 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_DARK);
			}
			if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2)
				sav *= 2;
			if(r_ptr->flags2 & RF2_POWERFUL)
				sav *= 2;

			if(randint1(sav) > p_ptr->lev)
			{
				note = "�͖�ɒ�R�����I";
				dam=0;
				break;
			}

			note = "�͖���󂯂��I";
			hina_gain_yaku(-dam/10);

			//v1.1.94 ���_�U���n�̒ǉ����ʂŖ��@�ቺ
			do_dec_mag = 8 + randint1(8);

			break;
		}
		/*:::�����̐��������̊���p*/
		case GF_RYUURI:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			//v1.1.47 �������ɂ������悤�ɂ����B�������ʂ�HP�z�����ʂ͂Ȃ��B
			/*
			if(!monster_living(r_ptr))
			{
				note = "�ɂ͌��ʂ��Ȃ������B";
				if (is_original_ap_and_seen(m_ptr))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_DEITY) r_ptr->r_flags3 |= (RF3_DEITY);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
				}
				dam = 0;
				break;
			}
			*/
			if (monster_living(r_ptr))
			{
				if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (r_ptr->flags1 & RF1_QUESTOR)
					|| (randint1(r_ptr->level * 4) > p_ptr->lev))
				{
					do_time = (dam + 7) / 8;
					hp_player(MIN(dam / 3, m_ptr->hp));
				}
				else
				{
					note_dies = "�̋}���ɐ[�X�Ɛn���˂��h�������I";
					if (dam < m_ptr->hp) dam = m_ptr->hp + 1;
					hp_player(dam / 3);
				}
			}

			break;
		}
		case GF_KOKORO:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				note = "�͊���������Ȃ��悤���B";
				dam = 0;
				break;
			}

			break;
		}

		//v1.1.42 �A���f�b�h�A�f�[�����ASMART�̓G�ɂ������悤�ɂ������ɏ����N���₷������
		case GF_NIGHTMARE:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune.";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			//else if (r_ptr->flags2 & (RF2_EMPTY_MIND) || (r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_NONLIVING)))
			else if (r_ptr->flags2 & (RF2_EMPTY_MIND))
			{
				dam = 0;
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#else
				note = " is immune!";
#endif
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				break;

			}
			else if ((r_ptr->flags2 & (RF2_WEIRD_MIND)) ||
					(r_ptr->flags3 & (RF3_NONLIVING)) ||
			         (r_ptr->flags1 & RF1_UNIQUE) )
			{
				dam /= 2;
#ifdef JP
				note = "�ɂ͑ϐ�������B";
#else
				note = " resists.";
#endif
				break;
			}

			
			else if (!MON_CSLEEP(m_ptr))
			{
				note = "�͐��_�U����H�����!";
			}
//			else if (randint1(r_ptr->level) > (p_ptr->lev * 3 / 2) || one_in_(10))
			else if (randint1(r_ptr->level) > (p_ptr->lev) || one_in_(10))
			{
				dam *= 2;
				note = "�͈��������Ĕ�ыN����!";
			}
			else 
			{
				dam *= 2;
				note = "�͈����ɂ��Ȃ���Ă���E�E";
				do_sleep = randint1(p_ptr->lev);
			}

			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_mag = 8 + randint1(dam / 10);
			}

#ifdef JP
			///msg dead
			//note_dies = "�̐��_�͕��󂵁A���͔̂����k�ƂȂ����B";
			note_dies = "�͓|�ꂽ�B";
#else
			note_dies = " collapses, a mindless husk.";
#endif

			break;
		}

		case GF_BANKI_BEAM:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}
		case GF_BANKI_BEAM2:
		{
			//�w���Y���C�͓��̕��g�ɖ���
			if(m_ptr->r_idx == MON_BANKI_HEAD_1 ||m_ptr->r_idx == MON_BANKI_HEAD_2) return (TRUE);
			if(is_pet(m_ptr)) return(TRUE);

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
#ifdef JP
				note = "�ɂ͊��S�ȑϐ�������I";
#endif
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			break;
		}
		case GF_N_E_P:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if(r_ptr->flags3 & (RF3_GEN_MASK))
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				break;
			}
			else if(r_ptr->flagsr & RFR_RES_TIME)
			{
				note = "�͈��ʗ��̏C���ɒ�R�����I";
				dam = dam * 6 / (12 + randint0(7));
				break;
			}
			note_dies = "�͏��ł����B";

			break;
		}

		//v1.1.61 �u���C���t�B���K�[�v�����g
		case GF_BRAIN_FINGER_PRINT:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}
			else if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				break;
			}
			else
			{
				//���݂܂łɎ󂯂��_���[�W�ʂŃ_���[�W��������B�{�X�i���㔼�u�E������ł��Ȃ��悤�����␳����B
				int mon_damaged = (m_ptr->maxhp - m_ptr->hp);

				if (mon_damaged < 3000) dam = mon_damaged / 2;
				else if (mon_damaged < 7000) dam = 1500 + (mon_damaged - 3000) / 4;
				else dam = 2500 + (mon_damaged - 7000) / 6;

				dam += adj_general[p_ptr->stat_ind[A_CHR]] * 8;

				if (r_ptr->flags3 & RF3_UNDEAD) dam *= 2;

				if (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) dam /= 2;
				if (r_ptr->flags2 & RF2_POWERFUL) dam = dam * 2 / 3;

				if (dam < 1) dam = 1;

				note = "�̋�ɂ̋L��������Ȃ��ɂ�^�����I";
				break;
			}

			//v1.1.94 ���_�n�U���Ŗ��@�\�͒ቺ
			if (!mon_saving_throw(m_ptr, dam))
			{
				do_dec_mag = 8 + randint1(dam / 10);
			}

			note_dies = "�͓|�ꂽ�B";

			break;
		}

		//v1.1.63 �������@�M������őM���E�����ϐ��̂Ȃ������X�^�[���m���ɍ���������
		//v1.1.86 �_���[�W��M���Ɛ��̔��X�ŏ�������B�M���Ɛ��̗����Ɏア�����X�^�[�ɂ̓_���[�W3�{�B
		case GF_RAINBOW:
		{
			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if ((r_ptr->flagsr & RFR_HURT_LITE) && (r_ptr->flagsr & RFR_HURT_WATER))
			{
				note = "�͑��z�ƉJ�̗͂��󂯂Ĕߖ��グ���I";
				dam *= 3;
			}
			else
			{
				int lite_dam, water_dam;
				lite_dam = (dam + 1) / 2;
				water_dam = (dam + 1) / 2;

				if (r_ptr->flagsr & RFR_RES_LITE)
				{
					note = "�͂����炩�̑ϐ����������B";

					lite_dam = lite_dam * 2 / (randint1(6) + 6);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
				}
				else if (r_ptr->flagsr & (RFR_HURT_LITE))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
					///sysdel dead
					note = "�͌��ɐg�������߂��I";
					note_dies = "�͌����󂯂ē|�ꂽ�I";
					lite_dam *= 2;
				}

				if (r_ptr->flagsr & RFR_RES_WATE || r_ptr->flags7 & RF7_AQUATIC)
				{
					note = "�͂����炩�̑ϐ����������B";

					water_dam = water_dam * 3 / (randint1(6) + 6);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
				}
				else if (r_ptr->flagsr & (RFR_HURT_WATER))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
					///sysdel dead
					note = "�͐������Ȃ悤���I";
					note_dies = "�͐����󂯂ē|�ꂽ�I";
					water_dam *= 2;
				}

				dam = lite_dam + water_dam;

			}

			if (r_ptr->flagsr & RFR_RES_LITE)
			{
				note = "�ɂ͑ϐ�������I";

				dam *= 2; dam /= (randint1(6) + 6);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_LITE);
			}
			///mod131231 �����X�^�[�t���O�ύX �M����_RF3����RFR��

			if (!(r_ptr->flagsr & RFR_RES_LITE) && !(r_ptr->flags3 & RF3_NO_CONF))
			{
				do_conf = 7 + randint1(7);

			}
			break;
		}

		//v1.1.17 ���ς́u�����v�����@�S��_�Ɏh����
		case GF_PURIFY:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flagsr & (RFR_HURT_HOLY))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HOLY);
			}
			if (r_ptr->flagsr & (RFR_HURT_HELL))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_HELL);
			}
			if (r_ptr->flagsr & (RFR_HURT_WATER))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_WATER);
			}
			if (r_ptr->flagsr & (RFR_HURT_ELEC))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ELEC);
			}
			if (r_ptr->flagsr & (RFR_HURT_COLD))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_COLD);
			}
			if (r_ptr->flagsr & (RFR_HURT_FIRE))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_FIRE);
			}
			if (r_ptr->flagsr & (RFR_HURT_ROCK))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_ROCK);
			}
			if (r_ptr->flagsr & (RFR_HURT_LITE))
			{
				note = "�͏������󂯂Đg�������߂��B";
				dam = dam * 3 / 2;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
			}

			break;
		}



		/*:::�e�ɂ�����ˌ��@special_shooting_mode�ŕ���*/
		case GF_SPECIAL_SHOT:
		{
			int power;
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͊��S�ȑϐ�������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			switch(special_shooting_mode)
			{

			case SSM_CONFUSE:	//�����ˌ�
				power = dam;
				dam = 0;
				if (r_ptr->flags3 & (RF3_NO_FEAR) )
				{
					note = "�ɂ͌��ʂ��Ȃ������I";
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_FEAR);
					break;
				}
				if((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) power /= 2;
				if(randint1(r_ptr->level) < randint1(power+caster_lev))
				{
					if (!(r_ptr->flagsr & RFR_RES_INER) && set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 4 + randint1(4)))
					{
						note = "�̓������x���Ȃ����B";
					}
					
					if(p_ptr->lev > 19 && !(r_ptr->flags3 & (RF3_NO_CONF))) do_conf = randint1(4) + randint0(power/10);
				}
				else
				{
					note = "�͘f�킳��Ȃ������I";

				}
				break;

			case SSM_LIGHT:	//�M���e
				
				if (r_ptr->flagsr & (RFR_HURT_LITE))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_HURT_LITE);
					note = "�ɑM���e�����������I";
					dam *= 3;
				}
				break;
			case SSM_VAMPIRIC: //�z���e
				if (!monster_living(r_ptr))
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
						if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
						if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
					}
					note = "�ɂ͌��ʂ��Ȃ������I";
					dam = 0;
				}
				else
				{
					note = "���琶���͂��z�������I";
					do_time = (dam+7)/8;
					hp_player(dam);
				}
				break;

			case SSM_SLAY_ANIMAL: 
				if (r_ptr->flags3 & (RF3_ANIMAL))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_ANIMAL);
					note = "�ɏb�E���̒e�ۂ��H�����񂾁I";
					dam *= 3;
				}
				break;

			case SSM_SLAY_NONLIV: 
				if (r_ptr->flags3 & (RF3_NONLIVING))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NONLIVING);
					note = "�͒e�ۂ��󂯂ē���������Ă���B";
					dam *= 3;
				}
				break;
			case SSM_SLAY_EVIL: 
				if (r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
					note = "�̑̂�e�ۂ��ł������I";
					dam *= 2;
				}
				break;
			case SSM_SLAY_HUMAN: 
				if (r_ptr->flags3 & (RF3_HUMAN))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_HUMAN);
					if (p_ptr->pclass == CLASS_SEIRAN)
					{
						note = "�̐��_�ɑŌ���^�����I";
						dam *= 2;
					}
					else
					{
						note = "�͉���Ɏ�芪���ꂽ�I";
						dam *= 3;
					}

				}
				else if (r_ptr->flags3 & (RF3_DEMIHUMAN))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEMIHUMAN);
					if (p_ptr->pclass == CLASS_SEIRAN)
						note = "�̐��_�ɑŌ���^�����I";
					else
						note = "�͉���Ɏ�芪���ꂽ�I";

					dam *= 2;
				}
				break;

			case SSM_SLAY_DRAGON:
				if (r_ptr->flags3 & (RF3_DRAGON))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DRAGON);
					note = "�̍d���؂𗳎E���̒e�ۂ������������I";
					dam *= 3;
				}
				break;
			case SSM_SLAY_DEITY: 
				if (r_ptr->flags3 & (RF3_DEITY))
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_DEITY);
					note = "�̑̂�_�E���̒e�ۂ������������I";
					dam *= 3;
				}
				break;			
			case SSM_SILVER: 
				if (r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON))
				{
					if (r_ptr->flags3 & RF3_DEMON) r_ptr->r_flags3 |= (RF3_DEMON);
					if (r_ptr->flags3 & RF3_UNDEAD) r_ptr->r_flags3 |= (RF3_UNDEAD);
					if (r_ptr->flags3 & RF3_NONLIVING) r_ptr->r_flags3 |= (RF3_NONLIVING);
					note = "�̑̂���̒e�ۂ��т����I";
					dam *= 3;
				}
				else if(r_ptr->flags3 & RF3_ANG_CHAOS)
				{
					r_ptr->r_flags3 |= (RF3_ANG_CHAOS);
					note = "�̑̂���̒e�ۂ��ł������I";
					dam = dam * 3 / 2;

				}
				break;
			case SSM_NIGHTMARE: 
				if(r_ptr->flagsr & RFR_HURT_HELL)
				{
					note = "�̐���ȑ̂������̒e�ۂ������������I";
					dam *= 3;
				}
				break;

			case SSM_SLAY_FLYER:
				if (r_ptr->flags7 & (RF7_CAN_FLY))
				{
					note = "���������Ƃ����I";
					dam *= 2;
				}
				break;			


			default:
				msg_format("ERROR:special_shooting_mode(%d)�ł�GF_SPECIAL_SHOT�������L�q����Ă��Ȃ�",special_shooting_mode);
				break;

			}


			break;
		}


		//v1.1.94
		case GF_DEC_ATK:
		case GF_DEC_DEF:
		case GF_DEC_MAG:
		case GF_DEC_ALL:
		case GF_DIG_OIL:
		case GF_SUPER_EGO:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//�X�[�p�[�G�S�@�����_�ɂ͖���
			if(typ == GF_SUPER_EGO && (r_ptr->flags2 & (RF2_WEIRD_MIND | RF2_EMPTY_MIND)))
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				obvious = FALSE;
			}

			if (mon_saving_throw(m_ptr, dam))
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				obvious = FALSE;
			}

			switch (typ)
			{
			case GF_DEC_ATK:
			case GF_DIG_OIL:
				do_dec_atk = 8 + randint1(8);
				break;
			case GF_DEC_DEF:
				do_dec_def = 8 + randint1(8);
				break;
			case GF_DEC_MAG:
				do_dec_mag = 8 + randint1(8);
				break;
			case GF_DEC_ALL:
				do_dec_atk = 8 + randint1(8);
				do_dec_def = 8 + randint1(8);
				do_dec_mag = 8 + randint1(8);
				break;

			case GF_SUPER_EGO:
				do_dec_atk = 8 + randint1(8);
				do_dec_def = 8 + randint1(8);
				do_dec_mag = 8 + randint1(8);
				do_not_move = 16 + randint1(16);
				break;
			}

			//�j�ł̖��dam��500���x�ɂ��Ă�����J�ɂ�1/2���炢�Ō����B25d25�ł������Ȃ��͂Ȃ��B
			//���̖��100���炢�ł������낤

			/* No "real" damage */
			dam = 0;
			break;
		}

		//v1.1.95 ����m��
		case GF_BERSERK:
		case GF_POSSESSION:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//�ʏ�̐��_�������Ȃ��G�A�N�G�X�g�œ|�Ώۂɂ͌����Ȃ�
			if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) || r_ptr->flags1 & RF1_QUESTOR)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				break;
			}

			//����߈˂͗d���Ɍ����₷��
			if (typ == GF_POSSESSION && r_ptr->flags3 & RF3_KWAI) dam = dam * 3 / 2;

			//�z���͒�R�����ɋ���m������
			if (is_pet(m_ptr))
			{
				do_berserk = 16 + randint1(dam / 4);
			}
			else
			{
				//�������Ȃ��G�ɂ͌����Â炢
				if (r_ptr->flags3 & RF3_NO_CONF )
				{
					dam = dam * 2 / 3;
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_NO_CONF);
				}
				//��R����Ɏ��s�����狶��m��
				if (mon_saving_throw(m_ptr, dam))
				{
					note = "�͌��ʂɒ�R�����I";
					dam = 0;
					break;
				}

				do_berserk = 8 + randint1(dam / 8);

			}



			/* No "real" damage */
			dam = 0;
			break;
		}


		case GF_NO_MOVE: //�ړ��֎~����
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}


			//�͂��ꃁ�^���ȊO�m��Ő�������B�������E�o����₷���B
			//process_monsters_mtimed_aux(),process_monster()�Q��
			do_not_move = dam;

			/* No "real" damage */
			dam = 0;
			break;
		}

		//v1.1.97 ���Ƃ������� ���g���b�v�̔����Ɓu�[���������v�̖��@
		//TODO:�Ă�̗��Ƃ����������������������F�X�������Ⴄ�̂Ō��
		case GF_PIT_FALL:
		{
			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			if (r_ptr->flags7 & RF7_CAN_FLY)
			{
				note = "�͗I�X�ƌ��̏����ł���...";
				dam = 0;
				break;
			}

			if (r_ptr->flags2 & RF2_GIGANTIC)
			{
				if (seen_msg) msg_format("%s�̋��̂��n�����𗧂ĂČ��ɗ������I",m_name);
				dam *= 2;
			}
			else
			{
				if (seen_msg) msg_format("%s�����ɗ������I", m_name);
			}

			do_stun = 8 + randint1(dam/10);

		}
		break;

		//v2.0.9 ����ۂ܂��鑮��
		case GF_ALCOHOL:
		{

			if (seen) obvious = TRUE;

			if (r_ptr->flagsr & RFR_RES_ALL)
			{
				note = "�ɂ͌��ʂ��Ȃ������I";
				dam = 0;
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
				break;
			}

			//�A���R�[���ϐ� 
			//�������͐���Ȃ�
			if (r_ptr->flags3 & RF3_NONLIVING)
			{
				dam = 0;
			}
			else if (dam < 0)//�ꌂ�œ|�����ꏈ��
			{
				dam = 30000;

			}
			//�S��E���o�~�͔��Ɏ��ɋ���
			else if (m_ptr->r_idx == MON_SUIKA || m_ptr->r_idx == MON_YUGI || m_ptr->r_idx == MON_KASEN || m_ptr->r_idx == MON_YUMA || m_ptr->r_idx == MON_UWABAMI)
			{
				dam /= 16;
				note = "�ɂ͂��Ȃ�ϐ�������I";
			}

			//�V��͎��ɋ���
			else if (r_ptr->d_char == 't')
			{
				dam /= 4;
				note = "�ɂ͑ϐ�������B";
			}
			/*
			//�őϐ��Œ�R�����悤���Ǝv��������߂�
			else if (r_ptr->flagsr & RFR_IM_POIS)
			{
				dam /= 3;
				note = "�ɂ͑ϐ�������B";

			}
			*/
			//����ȓG�Ɨ͋����G�͐����ɂ���
			else if (r_ptr->flags2 & (RF2_GIGANTIC | RF2_POWERFUL))
			{
				dam /= 2;
				note = "�͂����炩�ϐ����������B";
			}

			//�l�Ԃɂ͌���3�{�A�l�Ԍ^�����ɂ͌���2�{
			if (r_ptr->flags3 & RF3_HUMAN)
			{
				dam *= 3;

			}
			else if (r_ptr->flags3 & RF3_DEMIHUMAN)
			{
				dam *= 2;
			}

			if (!dam)
			{
				note = "�͑S�������l�q���Ȃ��I";
				break;
			}


			//�D���x�㏸
			set_monster_timed_status_add(MTIMED2_DRUNK, c_ptr->m_idx, MON_DRUNK(m_ptr) + dam);

			//�D���x�������X�^�[�̍ő�HP�𒴂����瑦���_���[�W
			if (MON_DRUNK(m_ptr) > m_ptr->maxhp)
			{
				dam = m_ptr->maxhp + 1;
			}
			else
			{
				dam = 0;
			}
			break;
		}


		/* Default */
		default:
		{
			/* Irrelevant */
			skipped = TRUE;

			/* No damage */
			dam = 0;

			break;
		}



	}//switch(typ)�I��

	/* Absolutely no effect */
	if (skipped) return (FALSE);

	/*:::Mega Hack - �t�r�_�̍U���ɂ��o���l�ƃA�C�e���́��̍U�������ɂ���*/
	///mod140709 �t�r�_�g��
	if(p_ptr->pclass == CLASS_TSUKUMO_MASTER && who > 0 && (caster_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && caster_ptr->r_idx <= MON_TSUKUMO_WEAPON5)) 
		who = 0;
	//mod140716 �d���������ɂ���
	if(p_ptr->pclass == CLASS_YOUMU && who > 0 && caster_ptr->r_idx == MON_YOUMU && (caster_ptr->mflag & MFLAG_EPHEMERA)) 
		who = 0;
	//mod140816 �Ԕ؊�̓��i�����U���E���[�U�[���Ɂj�������ɂ���
	if(p_ptr->pclass == CLASS_BANKI && who > 0 && 
		(caster_ptr->r_idx == MON_BANKI_HEAD_1 || caster_ptr->r_idx == MON_BANKI_HEAD_2) 
		&& (caster_ptr->mflag & MFLAG_EPHEMERA)) 
		who = 0;
	//�Ԕ؊�r�[�����A���X�p�ɍė��p
	if(typ == GF_BANKI_BEAM2) who = 0;
	//v1.1.41 ���Ɨ��T���x���ċ������Ă��郂���X�^�[�������ɂ���B���̂Ƃ��v���C���[�Ɣz���̗������o���l�𓾂���悤�t���O�𗧂Ă�
	if (CLASS_RIDING_BACKDANCE && p_ptr->riding == who)
	{
		who = 0;

		if(p_ptr->pclass != CLASS_TSUKASA) flag_monster_and_player_gain_exp = TRUE;
	}

	//���c�̂��݂����͎����������X�^�[���_���[�W���󂯂�悤�ɂ�����who��0�ɂ���
	//���g�̎����A�T�j�[�̔��e��
	if(who == PROJECT_WHO_OMIKUJI_HELL || who == PROJECT_WHO_SUICIDAL_FIRE || who == PROJECT_WHO_EXPLODE_BOMB || who == PROJECT_WHO_DEEPECO_BOMB)
		who = 0;

	/* "Unique" monsters cannot be polymorphed */
	if (r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & (RF7_VARIABLE)) do_poly = FALSE;

	/* Quest monsters cannot be polymorphed */
	if (r_ptr->flags1 & RF1_QUESTOR) do_poly = FALSE;

	if (p_ptr->riding && (c_ptr->m_idx == p_ptr->riding)) do_poly = FALSE;

	/* "Unique" and "quest" monsters can only be "killed" by the player. */
	///sys ���j�[�N�����̍U���ȊO�ň��ȉ���HP����Ȃ��Ȃ�悤�ȏ����͂����ɏ���
	if (((r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) || ((r_ptr->flags7 & RF7_VARIABLE ) && (r_ptr->flags7 & RF7_UNIQUE2)) || (r_ptr->flags7 & RF7_NAZGUL)) && (m_ptr->r_idx != MON_MASTER_KAGUYA) &&!p_ptr->inside_battle && !(m_ptr->mflag & MFLAG_EPHEMERA))
	{
		if (who && (dam > m_ptr->hp)) dam = m_ptr->hp;
	}

	//v1.1.22 �ő�_���[�W��9999�ɂ���
	//v2.0.9 �A���R�[���̂Ƃ������͂��̐�������蕥��
	if(typ != GF_ALCOHOL && dam > 9999) dam = 9999;

	///sysdel virtue
	/*
	if (!who && slept)
	{
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
	}
	*/

	/* Modify the damage */
	tmp = dam;
	dam = mon_damage_mod(m_ptr, dam, (bool)(typ == GF_BLAZINGSTAR || typ == GF_PSY_SPEAR || typ == GF_GUNGNIR));
#ifdef JP
	if ((tmp > 0) && (dam == 0) && typ != GF_PHOTO) note = "�̓_���[�W���󂯂Ă��Ȃ��B";
#else
	if ((tmp > 0) && (dam == 0)) note = " is unharmed.";
#endif

	//��������ˌ�
	if(special_shooting_mode == SSM_SEIRAN2 && !(r_ptr->flagsr & (RFR_RES_TELE | RFR_RES_TIME)) && dam)
	{
		do_dist = 10 + randint1(15) + randint1(p_ptr->lev);
	}

	/* Check for death */
	if (dam > m_ptr->hp)
	{
		/* Extract method of death */
		note = note_dies;
		//������ʍ쐬
		if(typ == GF_KOKORO) make_mask(m_ptr);

		//v1.1.49 �V���Ă��ƐH���ɂȂ�悤�ɂ��Ă݂�
		//�~�X�e�B�A��monster_death()����l�V���{���œ|�����̂Ƃ���ŏ�������̂ł����ł͏��O
		if (m_ptr->r_idx == MON_EEL && p_ptr->pclass != CLASS_MYSTIA)
		{
			switch (typ)
			{
			case GF_FIRE:
			case GF_ELEC:
			case GF_PLASMA:
			case GF_NUKE:
			case GF_STEAM:
				{
					object_type forge;
					object_type *tmp_o_ptr = &forge;

					object_prep(tmp_o_ptr, (int)lookup_kind(TV_FOOD, SV_FOOD_EEL));
					(void)drop_near(tmp_o_ptr, -1, m_ptr->fy, m_ptr->fx);


				}


			default:
				break;
			}
		}
	}
	else //�����X�^�[�����̍U���Ŏ��ȂȂ��Ƃ��A�X�e�[�^�X�ُ�t�^����Ȃ�
	{
		/* Sound and Impact resisters never stun */
		//v1.1.90 �����ϐ��ŞN�O���Ȃ��Ȃ�d�l���Ȃ�����
		if (do_stun && !(r_ptr->flags3 & RF3_NO_STUN))
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Get stunned */
			if (MON_STUNNED(m_ptr))
			{
#ifdef JP
				note = "�͂Ђǂ������낤�Ƃ����B";
#else
				note = " is more dazed.";
#endif

				tmp = MON_STUNNED(m_ptr) + (do_stun / 2);
			}
			else
			{
#ifdef JP
				note = "�͂����낤�Ƃ����B";
#else
				note = " is dazed.";
#endif

				tmp = do_stun;
			}

			/* Apply stun */
			(void)set_monster_stunned(c_ptr->m_idx, tmp);

			/* Get angry */
			get_angry = TRUE;
		}

		/* Confusion and Chaos resisters (and sleepers) never confuse */
		//v1.1.90 �J�I�X�ϐ��ō������Ȃ��Ȃ�d�l���Ȃ�����
		if (do_conf && !(r_ptr->flags3 & RF3_NO_CONF) )
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Already partially confused */
			if (MON_CONFUSED(m_ptr))
			{
#ifdef JP
				note = "�͂���ɍ��������悤���B";
#else
				note = " looks more confused.";
#endif

				tmp = MON_CONFUSED(m_ptr) + (do_conf / 2);
			}

			/* Was not confused */
			else
			{
#ifdef JP
				note = "�͍��������悤���B";
#else
				note = " looks confused.";
#endif

				tmp = do_conf;
			}

			/* Apply confusion */
			(void)set_monster_confused(c_ptr->m_idx, tmp);

			/* Get angry */
			get_angry = TRUE;
		}

		if (do_dec_atk)
		{
			if (seen) obvious = TRUE;

			if (MON_DEC_ATK(m_ptr))
			{
				tmp = MON_DEC_ATK(m_ptr) + do_dec_atk / 2;
			}
			else
			{
				note = "�͍U���͂����������悤���B";
				tmp = do_dec_atk;
			}
			(void)set_monster_timed_status_add(MTIMED2_DEC_ATK, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		if (do_dec_def)
		{
			if (seen) obvious = TRUE;

			if (MON_DEC_DEF(m_ptr))
			{
				tmp = MON_DEC_DEF(m_ptr) + do_dec_def / 2;
			}
			else
			{
				note = "�͖h��͂����������悤���B";
				tmp = do_dec_def;
			}
			(void)set_monster_timed_status_add(MTIMED2_DEC_DEF, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		//���@�ቺ�@���@�������Ȃ������X�^�[�ɂ͌����Ȃ�
		if (do_dec_mag && HAS_ANY_MAGIC(r_ptr))
		{
			if (seen) obvious = TRUE;

			if (MON_DEC_MAG(m_ptr))
			{
				tmp = MON_DEC_MAG(m_ptr) + do_dec_mag / 2;
			}
			else
			{
				note = "�͖��@�͂����������悤���B";
				tmp = do_dec_mag;
			}
			(void)set_monster_timed_status_add(MTIMED2_DEC_MAG, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		//�j�ł̖򓊝��Ȃǂŕ����̔\�͂����������Ƃ����b�Z�[�W��ς��Ă���
		if (do_dec_atk && do_dec_def || do_dec_atk && do_dec_mag || do_dec_def && do_dec_mag)
		{
			note = "�͕����̔\�͂����������悤���B";
		}



		if (do_berserk)
		{
			if (seen) obvious = TRUE;

			if (MON_BERSERK(m_ptr))
			{
				tmp = MON_BERSERK(m_ptr) + do_berserk / 2;
			}
			else
			{
				note = "�͌������\�ꂾ�����I";
				tmp = do_berserk;
			}
			(void)set_monster_timed_status_add(MTIMED2_BERSERK, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}

		if (do_not_move)
		{
			if (seen) obvious = TRUE;

			if (MON_NO_MOVE(m_ptr))
			{
				tmp = MON_NO_MOVE(m_ptr) + do_not_move / 2;
			}
			else
			{
				note = "�͈ړ��ł��Ȃ��Ȃ����I";
				tmp = do_not_move;
			}
			(void)set_monster_timed_status_add(MTIMED2_NO_MOVE, c_ptr->m_idx, tmp);
			get_angry = TRUE;
		}



		if (do_time)
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			if (do_time >= m_ptr->maxhp) do_time = m_ptr->maxhp - 1;

			if (do_time)
			{
#ifdef JP
				note = "�͎キ�Ȃ����悤���B";
#else
				note = " seems weakened.";
#endif
				m_ptr->maxhp -= do_time;
				if ((m_ptr->hp - dam) > m_ptr->maxhp) dam = m_ptr->hp - m_ptr->maxhp;
			}
			get_angry = TRUE;
		}

		/* Mega-Hack -- Handle "polymorph" -- monsters get a saving throw */
		if (do_poly && (randint1(90) > r_ptr->level))
		{
			if (polymorph_monster(y, x))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Monster polymorphs */
#ifdef JP
				note = "���ϐg�����I";
#else
				note = " changes!";
#endif

				/* Turn off the damage */
				dam = 0;
			}
			else
			{
				/* No polymorph */
#ifdef JP
				note = "�ɂ͌��ʂ��Ȃ������I";
#else
				note = " is unaffected!";
#endif
			}

			/* Hack -- Get new monster */
			m_ptr = &m_list[c_ptr->m_idx];

			/* Hack -- Get new race */
			r_ptr = &r_info[m_ptr->r_idx];
		}

		/* Handle "teleport" */
		//�d�ԂƗ����͕ʃ��b�Z�[�W
		if (do_dist && !(p_ptr->riding && c_ptr->m_idx == p_ptr->riding) )
		{
			
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Message */
#ifdef JP
			if(typ == GF_BLAZINGSTAR) note = "�͐�����΂��ꂽ�I";
			else if(typ != GF_TRAIN && typ != GF_TORNADO) note = "�������������I";
#else
			note = " disappears!";
#endif

		//	if (!who) chg_virtue(V_VALOUR, -1);

			/* Teleport */
			teleport_away(c_ptr->m_idx, do_dist,
						(!who ? TELEPORT_DEC_VALOUR : 0L) | TELEPORT_PASSIVE);

			/* Hack -- get new location */
			y = m_ptr->fy;
			x = m_ptr->fx;

			/* Hack -- get new grid */
			c_ptr = &cave[y][x];
		}

		/* Fear */
		//v1.1.90 ���|���Ȃ������X�^�[�̏����ǉ��@
		if (do_fear && !(r_ptr->flags3 & RF3_NO_FEAR))
		{
			/* Set fear */
			(void)set_monster_monfear(c_ptr->m_idx, MON_MONFEAR(m_ptr) + do_fear);

			/* Get angry */
			get_angry = TRUE;
		}
	}

	if (typ == GF_DRAIN_MANA)
	{
		/* Drain mana does nothing */
	}

	/* If another monster did the damage, hurt the monster by hand */
	else if (who)
	{
		/* Redraw (later) if needed */
		if (p_ptr->health_who == c_ptr->m_idx) p_ptr->redraw |= (PR_HEALTH);
		if (p_ptr->riding == c_ptr->m_idx) p_ptr->redraw |= (PR_UHEALTH);

		/* Wake the monster up */
		(void)set_monster_csleep(c_ptr->m_idx, 0);



		/* Hurt the monster */
		m_ptr->hp -= dam;

		//�P���풆���b�Z�[�W �Ȃ��Ǝv�����[�����Z�����̏��������Ƃ�
		if(m_ptr->r_idx == MON_MASTER_KAGUYA && !m_ptr->ml && m_ptr->max_maxhp)
		{
			kaguya_fight = (byte) (m_ptr->hp * 100 / m_ptr->max_maxhp);
		}

		/* Dead monster */
		if (m_ptr->hp < 0)
		{
			bool sad = FALSE;
			bool parsee = FALSE;
			bool mine = FALSE;

			if (is_pet(m_ptr) && !(m_ptr->ml))
				sad = TRUE;

			//�p���X�B�̌��e�̓����X�^�[�ɓ|�����Ɣ�������
			if(m_ptr->r_idx == MON_PARSEE && (m_ptr->mflag & MFLAG_EPHEMERA))
				parsee = TRUE;

			if(m_ptr->r_idx == MON_SAGUME_MINE) mine = TRUE;
			//v1.1.24
			else if(m_ptr->r_idx == MON_YOUR_BOAT || m_ptr->r_idx == MON_ANCIENT_SHIP)
				note = "�͒��񂾁B";

			/* Give detailed messages if destroyed */
			if (known && note)
			{
				monster_desc(m_name, m_ptr, MD_TRUE_NAME);
				if (see_s_msg)
				{
					msg_format("%^s%s", m_name, note);
				}
				else
				{
					mon_fight = TRUE;
				}
			}

			if (who > 0) monster_gain_exp(who, m_ptr->r_idx);

			//v1.1.48 �����̓t���A�Ń����X�^�[���|�ꂽ�Ƃ����̌o���l�����̒l���u�s�^�p���[�v�Ƃ��Ē~�ς���
			if (p_ptr->pclass == CLASS_SHION)
			{
				p_ptr->magic_num1[1] += r_info[m_ptr->r_idx].mexp;

			}

			//�P�邪�|���ꂽ�Ƃ��̓G���L�^
			if(p_ptr->pclass == CLASS_EIRIN && m_ptr->r_idx == MON_MASTER_KAGUYA)
			{
				if(who > 0) p_ptr->magic_num1[0] = m_list[who].r_idx;
				else p_ptr->magic_num1[0] = who;
			}

			/* Generate treasure, etc */
			monster_death(c_ptr->m_idx, FALSE);

			/* Delete the monster */
			delete_monster_idx(c_ptr->m_idx);

			if(parsee)
			{
				msg_print("���Ȃ��̌��e�͔��������I");
				project(0, 5, y,x, 100 + p_ptr->lev * 10, GF_DISP_ALL, PROJECT_KILL | PROJECT_GRID, -1);


			}
			else if (sad)
			{
				if(mine)
					msg_print("�@����������⑫�����悤���B");
				else
					msg_print("�����߂����C���������B");
			}
		}

		/* Damaged monster */
		else
		{
			/* Give detailed messages if visible or destroyed */
			if (note && seen_msg) msg_format("%^s%s", m_name, note);

			/* Hack -- Pain message */
			/*:::�_���[�W�ɑ΂��郂���X�^�[�̃��A�N�V�������L�q*/
			else if (see_s_msg)
			{
				message_pain(c_ptr->m_idx, dam);
			}
			else
			{
				mon_fight = TRUE;
			}

			/* Hack -- handle sleep */
			//�ق��̃X�e�[�^�X�ُ�ƈ����RF3_NO_SLEEP�̔�������Ă��Ȃ��̂�GF_STASIS�n�̏����Ƌ��p���邽�߂�
			if (do_sleep) (void)set_monster_csleep(c_ptr->m_idx, do_sleep);
		}
	}

	///sysdel �a�l������
	else if (heal_leper)
	{
#ifdef JP
		if (seen_msg) msg_print("�s���ȕa�l�͕a�C���������I");
#else
		if (seen_msg) msg_print("The Mangy looking leper is healed!");
#endif

		if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
		{
			char m2_name[80];

			monster_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_HEAL_LEPER, m2_name);
		}

		delete_monster_idx(c_ptr->m_idx);
	}

	/* If the player did it, give him experience, check fear */
	/*:::���ɂ��U��*/
	else
	{
		bool fear = FALSE;

		int tmp_r_idx = m_ptr->r_idx;

		/* Hurt the monster, check for fear and death */
		if (mon_take_hit(c_ptr->m_idx, dam, &fear, note_dies))
		{
			/* Dead monster */

			if (flag_monster_and_player_gain_exp) monster_gain_exp(p_ptr->riding, tmp_r_idx);
		}

		/* Damaged monster */
		else
		{
			/* HACK - anger the monster before showing the sleep message */
			if (do_sleep) anger_monster(m_ptr);

			if((m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_MOKOU) && m_ptr->hp >= 0) 
			{
				//�������Ȃ��@���ׂ▅�g�����������Ƃ��u�|�����v�ƃ��b�Z�[�W���o��̂�}�~����
			}
			/* Give detailed messages if visible or destroyed */
			else if (note && seen_msg)
#ifdef JP
				msg_format("%s%s", m_name, note);
#else
				msg_format("%^s%s", m_name, note);
#endif


			/* Hack -- Pain message */
			//v1.1.43 do_fear�̏�������������
			//else if (known && (dam || !do_fear))
			else if (known && dam)
			{
				message_pain(c_ptr->m_idx, dam);
			}

			/* Anger monsters */
			if (((dam > 0) || get_angry) && !do_sleep)
				anger_monster(m_ptr);


			/* Hack -- handle sleep */
			//�ق��̃X�e�[�^�X�ُ�ƈ����RF3_NO_SLEEP�̔�������Ă��Ȃ��̂�GF_STASIS�n�̏����Ƌ��p���邽�߂�
			if (do_sleep) (void)set_monster_csleep(c_ptr->m_idx, do_sleep);
			/* Take note */
			else if ((fear || do_fear) && seen)
			{
				sound(SOUND_FLEE);
				msg_format("%^s�͋��|���ē����o�����I", m_name);
			}
		}
	}

	if ((typ == GF_BLOOD_CURSE) && one_in_(4))
	{
		int curse_flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP);
		int count = 0;
		do
		{
			switch (randint1(28))
			{
			case 1: case 2:
				if (!count)
				{
#ifdef JP
msg_print("�n�ʂ��h�ꂽ...");
#else
					msg_print("The ground trembles...");
#endif

					earthquake(ty, tx, 4 + randint0(4));
					if (!one_in_(6)) break;
				}
			case 3: case 4: case 5: case 6:
				if (!count)
				{
					int dam = damroll(10, 10);
#ifdef JP
msg_print("�����Ȗ��͂̎����ւ̔����J�����I");
#else
					msg_print("A portal opens to a plane of raw mana!");
#endif

					project(0, 8, ty,tx, dam, GF_MANA, curse_flg, -1);
					if (!one_in_(6)) break;
				}
			case 7: case 8:
				if (!count)
				{
#ifdef JP
msg_print("��Ԃ��c�񂾁I");
#else
					msg_print("Space warps about you!");
#endif

					if (m_ptr->r_idx) teleport_away(c_ptr->m_idx, damroll(10, 10), TELEPORT_PASSIVE);
					if (one_in_(13)) count += activate_hi_summon(ty, tx, TRUE);
					if (!one_in_(6)) break;
				}
			case 9: case 10: case 11:
#ifdef JP
msg_print("�G�l���M�[�̂��˂���������I");
#else
				msg_print("You feel a surge of energy!");
#endif

				project(0, 7, ty, tx, 50, GF_DISINTEGRATE, curse_flg, -1);
				if (!one_in_(6)) break;
			case 12: case 13: case 14: case 15: case 16:
				aggravate_monsters(0,TRUE);
				if (!one_in_(6)) break;
			case 17: case 18:
				count += activate_hi_summon(ty, tx, TRUE);
				if (!one_in_(6)) break;
			case 19: case 20: case 21: case 22:
			{
				bool pet = !one_in_(3);
				u32b mode = PM_ALLOW_GROUP;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_NO_PET | PM_FORCE_FRIENDLY);

				count += summon_specific((pet ? -1 : 0), py, px, (pet ? p_ptr->lev*2/3+randint1(p_ptr->lev/2) : dun_level), 0, mode);
				if (!one_in_(6)) break;
			}
			case 23: case 24: case 25:
				///mod131228 �����͈ێ��폜
				//if (p_ptr->hold_life && (randint0(100) < 75)) break;
				if (p_ptr->resist_neth && (randint0(100) < 75)) break;

#ifdef JP
msg_print("�����͂��̂���z�����ꂽ�C������I");
#else
				msg_print("You feel your life draining away...");
#endif
			
				//if (p_ptr->hold_life) lose_exp(p_ptr->exp / 160);
				if (p_ptr->resist_neth) lose_exp(p_ptr->exp / 160);

				else lose_exp(p_ptr->exp / 16);
				if (!one_in_(6)) break;
			case 26: case 27: case 28:
			{
				int i = 0;
				if (one_in_(13))
				{
					while (i < 6)
					{
						do
						{
							(void)do_dec_stat(i);
						}
						while (one_in_(2));

						i++;
					}
				}
				else
				{
					(void)do_dec_stat(randint0(6));
				}
				break;
			}
			}
		}
		while (one_in_(5));
	}

	if (p_ptr->inside_battle)
	{
		p_ptr->health_who = c_ptr->m_idx;
		p_ptr->redraw |= (PR_HEALTH);
		redraw_stuff();
	}

	/* XXX XXX XXX Verify this code */

	/* Update the monster */
	if (m_ptr->r_idx) update_mon(c_ptr->m_idx, FALSE);

	/* Redraw the monster grid */
	lite_spot(y, x);


	/* Update monster recall window */
	if ((p_ptr->monster_race_idx == m_ptr->r_idx) && (seen || !m_ptr->r_idx))
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}

	if ((dam > 0) && !is_pet(m_ptr) && !is_friendly(m_ptr))
	{
		if (!who)
		{
			if (!(flg & PROJECT_NO_HANGEKI))
			{
				set_target(m_ptr, monster_target_y, monster_target_x);
			}
		}
		else if ((who > 0) && is_pet(caster_ptr) && !player_bold(m_ptr->target_y, m_ptr->target_x))
		{
			set_target(m_ptr, caster_ptr->fy, caster_ptr->fx);
		}
	}

	if (p_ptr->riding && (p_ptr->riding == c_ptr->m_idx) && (dam > 0))
	{
		if (m_ptr->hp > m_ptr->maxhp/3) dam = (dam + 1) / 2;
		rakubadam_m = (dam > 200) ? 200 : dam;
	}

	if (photo)
	{

		if(IS_RANDOM_UNIQUE_IDX(photo))
		{
			if(!who) msg_print("�Ȃ������܂��ʂ�Ȃ������B");
		}
		else
		{

			object_type *q_ptr;
			object_type forge;

			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_PHOTO, 0));
			q_ptr->pval = photo;
			q_ptr->ident |= (IDENT_MENTAL);
			(void)drop_near(q_ptr, -1, py, px);
		}
	}
	/* Track it */
	project_m_n++;
	project_m_x = x;
	project_m_y = y;

	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to the player.
 *
 * This routine takes a "source monster" (by index), a "distance", a default
 * "damage", and a "damage type".  See "project_m()" above.
 *
 * If "rad" is non-zero, then the blast was centered elsewhere, and the damage
 * is reduced (see "project_m()" above).  This can happen if a monster breathes
 * at the player and hits a wall instead.
 *
 * NOTE (Zangband): 'Bolt' attacks can be reflected back, so we need
 * to know if this is actually a ball or a bolt spell
 *
 *
 * We return "TRUE" if any "obvious" effects were observed.  XXX XXX Actually,
 * we just assume that the effects were obvious, for historical reasons.
 */
/*:::���ɑ΂���{���g�E�{�[���E�r�[���E�u���X�̃_���[�W����*/
/*:::monspell:�����X�^�[�̍U�����@ID�H���@���[�j���O�p�H*/
///sys class res race ���ɑ΂���_���[�W�����@�d�v
///mod131229 ���A�j�ׁA�R�Y�~�b�N�z���[�Ȃǎb��I�ɒǉ��@�ϐ��K�p��ύX
bool project_p(int who, cptr who_name, int r, int y, int x, int dam, int typ, int flg, int monspell)
{
	int k = 0;
	int rlev = 0;

	/* Hack -- assume obvious */
	bool obvious = TRUE;

	/* Player blind-ness */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Player needs a "description" (he is blind) */
	bool fuzzy = FALSE;

	/* Source monster */
	monster_type *m_ptr = NULL;

	/* Monster name (for attacks) */
	char m_name[80];

	/* Monster name (for damage) */
	char killer[80];

	/* Hack -- messages */
	cptr act = NULL;

	int get_damage = 0;


	/* Player is not here */
	/*:::�ς��g���Ȃ�m���ŉ���������I��*/
	if (!player_bold(y, x)) return (FALSE);
	
	if (p_ptr->pclass == CLASS_MAMIZOU && (p_ptr->special_defense & NINJA_KAWARIMI) && (randint0(55) < (p_ptr->lev+20))&& who && (who != p_ptr->riding))
	{
		if (kawarimi(TRUE)) return TRUE;
	}
	else if (p_ptr->pclass != CLASS_MAMIZOU && (p_ptr->special_defense & NINJA_KAWARIMI) && dam && (randint0(55) < (p_ptr->lev*3/5+20)) && who && (who != p_ptr->riding))
	{
		if (kawarimi(TRUE)) return FALSE;
	}

	//v1.1.77 ����(��p���i)�̒ǐՁ@
	if (orin_escape(who)) return TRUE;


	//�z�K�q�~�������G
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0]) 
	{
		if( typ != GF_PSY_SPEAR && typ != GF_DISINTEGRATE)
		{
			msg_print("���������Ȃ��ɂ͓͂��Ȃ������B");
			return FALSE;
		}
		else
		{
			msg_print("�U���͒n���ɂ܂œ͂����I");
			set_tim_general(0,TRUE,0,1);
		}
	}

	/* Player cannot hurt himself */
	if (!who) return (FALSE);
	if (who == p_ptr->riding) return (FALSE);

	//v1.1.17 ���ψꎞ�X�L��
	//v1.1.52 俎q�V���i�ǉ�
	//v2.0.4b�X�y�J����ʂ������������̂ŏC��
	if(check_activated_nameless_arts(JKF1_VANISH_BEAM) && typ != GF_PSY_SPEAR && 
		(flg & (PROJECT_BEAM | PROJECT_MASTERSPARK | PROJECT_REFLECTABLE)))
		{
			msg_print("���Ȃ��͍U���𖳌��������I");
			return FALSE;
		}


	/*:::���ˏ���*/
	///���̔��ˏ����@��V��̃{���g�������������ɋL�q��
	if ((p_ptr->reflect || ((p_ptr->special_defense & KATA_FUUJIN) && !p_ptr->blind)) && (flg & PROJECT_REFLECTABLE) && !one_in_(10))
	{
		byte t_y, t_x;
		int max_attempts = 10;

#ifdef JP
		if (blind) msg_print("���������˕Ԃ����I");
		else if (p_ptr->special_defense & KATA_FUUJIN) msg_print("���̔@�������U����Ēe���Ԃ����I");
		else msg_print("�U�������˕Ԃ����I");
#else
		if (blind) msg_print("Something bounces!");
		else msg_print("The attack bounces!");
#endif


		/* Choose 'new' target */
		if (who > 0)
		{
			do
			{
				t_y = m_list[who].fy - 1 + randint1(3);
				t_x = m_list[who].fx - 1 + randint1(3);
				max_attempts--;
			}
			while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(py, px, t_y, t_x));

			if (max_attempts < 1)
			{
				t_y = m_list[who].fy;
				t_x = m_list[who].fx;
			}
		}
		else
		{
			t_y = py - 1 + randint1(3);
			t_x = px - 1 + randint1(3);
		}

		project(0, 0, t_y, t_x, dam, typ, (PROJECT_STOP|PROJECT_KILL|PROJECT_REFLECTABLE), monspell);

		disturb(1, 1);
		return TRUE;
	}
	///mod140316 ��V��{���g����
	///mod140608 �����x���̃`���m�̓{���g��Ⓚ
	//v1.1.52 俎q���ꐫ�i���X�}�z�������Ă���Ƃ��Ɍ���{���g����
	/*:::��V��̓{���g���������邱�Ƃ����� �j���W���̓{���g���������*/
	if (((p_ptr->pclass == CLASS_CIRNO && p_ptr->lev > 39) 
		|| p_ptr->prace == RACE_KARASU_TENGU 
		|| p_ptr->prace == RACE_NINJA 
		|| is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO) && check_have_specific_item(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE) >= 0
		|| p_ptr->mimic_form == MIMIC_MIST) 
		&& !p_ptr->blind && (flg & PROJECT_REFLECTABLE) )
	{
		m_ptr = &m_list[who];
		rlev = (((&r_info[m_ptr->r_idx])->level >= 1) ? (&r_info[m_ptr->r_idx])->level : 1);
		if(!one_in_(10) && randint1(rlev*3/2) < p_ptr->lev + adj_general[p_ptr->stat_ind[A_DEX]]*2)
		{
			if(p_ptr->prace == RACE_NINJA)
				msg_print("�u�C���[�b�I�v���Ȃ��̓u���b�W�ł��������I");
			else if(p_ptr->mimic_form == MIMIC_MIST)
				msg_print("�G�e�͂��Ȃ��̑̂����蔲�����I");
			else if(p_ptr->pclass == CLASS_CIRNO)
				msg_print("���Ȃ��͓G�e�𓀂点���I");
			else 
				msg_print("���Ȃ��͓G�̒e���B�e���ď������I");

			return (FALSE);
		}
	}
	///mod141116 �j���ېV�@�\�@�{���g�������ɕϊ�
	else if ((p_ptr->pclass == CLASS_SHINMYOUMARU) && !p_ptr->blind && (flg & PROJECT_REFLECTABLE) && !p_ptr->tim_general[0] && !p_ptr->magic_num1[0])
	{
		m_ptr = &m_list[who];
		rlev = (((&r_info[m_ptr->r_idx])->level >= 1) ? (&r_info[m_ptr->r_idx])->level : 1);
		if(!one_in_(10) && randint1(rlev*3/2) < p_ptr->lev + adj_general[p_ptr->stat_ind[A_DEX]]*2)
		{
			msg_print("���Ƃ�U��ƓG�e�͏����ւƕω������I");
			p_ptr->au += dam;
			p_ptr->redraw |= (PR_GOLD);
			return (FALSE);
		}
	}
	//v1.1.30
	else if ((p_ptr->pclass == CLASS_YOUMU) && !p_ptr->blind && (flg & PROJECT_REFLECTABLE))
	{
		int chance = 0;
		if(inventory[INVEN_RARM].tval == TV_KATANA) chance += 20 + p_ptr->lev;
		if(inventory[INVEN_LARM].tval == TV_KATANA) chance += 20 + p_ptr->lev;
		if(randint0(100) < chance)
		{
			msg_print("���Ȃ��͓G�e��؂蕥�����I");
			return (FALSE);
		}
	}
	else if(p_ptr->pclass == CLASS_SEIJA && p_ptr->magic_num1[SEIJA_ITEM_CAMERA] &&  !p_ptr->blind && !p_ptr->confused && !p_ptr->paralyzed && (flg & PROJECT_REFLECTABLE) && !one_in_(10) )
	{
		msg_print("���Ȃ��͓G�e���B�e���ď������I");
		p_ptr->magic_num1[SEIJA_ITEM_CAMERA] -= 1;
		return (FALSE);

	}
	///mod150315 �d�����Z
	else if(CHECK_FAIRY_TYPE == 2 && (flg & PROJECT_REFLECTABLE) && one_in_(3))
	{
		msg_print("���Ȃ��͓G�e���O���C�Y�����I");
		return (FALSE);
	}




	if(CHECK_RINGO_IMMUNE(typ))
	{
		msg_print("���Ȃ��͊��S�ȖƉu���������I");
		return (FALSE);
	}

	/* XXX XXX XXX */
	/* Limit maximum damage */
	if (dam > 1600) dam = 1600;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);


	/* If the player is blind, be more descriptive */
	if (blind) fuzzy = TRUE;


	if (who > 0)
	{
		/* Get the source monster */
		m_ptr = &m_list[who];
		/* Extract the monster level */
		rlev = (((&r_info[m_ptr->r_idx])->level >= 1) ? (&r_info[m_ptr->r_idx])->level : 1);

		/* Get the monster name */
		monster_desc(m_name, m_ptr, 0);

		/* Get the monster's real name (gotten before polymorph!) */
		strcpy(killer, who_name);
	}
	else
	{
		/*:::�ꕔ�̎����p�ɕ��̒l�Œ�`����Ă���*/
		switch (who)
		{
		case PROJECT_WHO_UNCTRL_POWER:
#ifdef JP
			strcpy(killer, "����ł��Ȃ��͂̔×�");
#else
			strcpy(killer, "uncontrollable power storm");
#endif
			break;

		case PROJECT_WHO_GLASS_SHARDS:
#ifdef JP
			strcpy(killer, "�K���X�̔j��");
#else
			strcpy(killer, "shards of glass");
#endif
			break;
		case PROJECT_WHO_OMIKUJI_HELL:
			strcpy(killer, "�勥�̂��݂���");
			break;
		case PROJECT_WHO_SUICIDAL_FIRE:
			strcpy(killer, "�����̋N��������");
			break;
		case PROJECT_WHO_EXPLODE_BOMB:
			strcpy(killer, "�����̎d�|�������e");
			break;
		case PROJECT_WHO_DEEPECO_BOMB:
			strcpy(killer, "�����̓��������e");
			break;
		case PROJECT_WHO_CONTACT_YOG:
			strcpy(killer, "�w���O���\�g�[�g�x�Ƃ̐ڐG");
			rlev = 90;
			break;
		case PROJECT_WHO_TRAP:
			strcpy(killer, "�");
			break;
		case PROJECT_WHO_EXPLODE_ICE:
			strcpy(killer, "�X�̔j��");
			break;


		default:
#ifdef JP
			strcpy(killer, "(���̑�)");
#else
			strcpy(killer, "a trap");
#endif
			break;
		}

		/* Paranoia */
		strcpy(m_name, killer);
	}

	/* Analyze the damage */
	///sys res �����ƃA�C�e���j��@����ԋ��Ƃ����ɗU���Ƃ��ǉ��v
	switch (typ)
	{
		/* Standard damage -- hurts inventory too */
		case GF_ACID:
		{
#ifdef JP
			if (fuzzy) msg_print("�_�ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by acid!");
#endif
			
			get_damage = acid_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_FIRE:
		{
#ifdef JP
			if (fuzzy) msg_print("�Ή��ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by fire!");
#endif

			//v1.1.91 �Ζ��n�`�Ή��_���[�W
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);


			get_damage = fire_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_COLD:
		{
#ifdef JP
			if (fuzzy) msg_print("��C�ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by cold!");
#endif

			get_damage = cold_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_ELEC:
		{
#ifdef JP
			if (fuzzy) msg_print("�d���ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by lightning!");
#endif

			get_damage = elec_dam(dam, killer, monspell);
			break;
		}

		/* Standard damage -- also poisons player */
		case GF_POIS:
		{
			bool double_resist = IS_OPPOSE_POIS();
#ifdef JP
			if (fuzzy) msg_print("�łōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by poison!");
#endif

			if(p_ptr->pclass == CLASS_MEDICINE)
			{
				msg_print("�̂��łŖ��������B");
				hp_player(dam / 3);
				set_food(p_ptr->food + dam*10);
				break;
			}
			if(p_ptr->pclass == CLASS_EIRIN)
			{
				msg_print("���Ȃ��ɂ͊��S�ȖƉu������I");
				dam=0;
				break;
			}

			if (p_ptr->resist_pois) dam = (dam + 2) / 3;
			if (double_resist) dam = (dam + 2) / 3;

			if ((!(double_resist || p_ptr->resist_pois)) &&
			     one_in_(HURT_CHANCE) && !CHECK_MULTISHADOW())
			{
				do_dec_stat(A_CON);
			}
			///mod140813 �b������
			if ((p_ptr->muta2 & MUT2_ASTHMA) && !CHECK_MULTISHADOW())
			{
				set_asthma(p_ptr->asthma + dam * 20);
				if (!(double_resist || p_ptr->resist_pois)) set_asthma(p_ptr->asthma + 500 + randint0(1000) );
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			if (!(double_resist || p_ptr->resist_pois) && !CHECK_MULTISHADOW())
			{
				set_poisoned(p_ptr->poisoned + randint0(dam) + 10);
			}
			break;
		}

		/* �����_���[�W�@��+�򉻁@�ǂ���̑ϐ����Ȃ��ƑS�\�͌��� */
		case GF_POLLUTE:
		{
			int dam_pois, dam_disen;
			bool double_resist = IS_OPPOSE_POIS();
#ifdef JP
			if (fuzzy) msg_print("�����ɂ߂ėL�Q�ȕ����ōU�����ꂽ�I");
#endif
			dam_pois = dam / 2 + 1;
			dam_disen = dam / 2 + 1;

			if (p_ptr->resist_pois) dam_pois = (dam_pois + 2) / 3;
			if (double_resist) dam_pois = (dam_pois + 2) / 3;

			if(p_ptr->pclass == CLASS_EIRIN) 
				dam_pois = 0;
			else
				get_damage = take_hit(DAMAGE_ATTACK, dam_pois, killer, monspell);

			if (!(double_resist || p_ptr->resist_pois) && !CHECK_MULTISHADOW())
				set_poisoned(p_ptr->poisoned + randint0(dam) + 10);

			if(p_ptr->is_dead) break;

			dam_disen = mod_disen_dam(dam_disen);

			if ( !p_ptr->resist_disen && !CHECK_MULTISHADOW())
			{
				(void)apply_disenchant(0,0);
			}
			get_damage += take_hit(DAMAGE_ATTACK, dam_disen, killer, monspell);

			if ((!(double_resist || p_ptr->resist_pois || p_ptr->resist_disen)) && !CHECK_MULTISHADOW())
			{
				do_dec_stat(A_STR);
				do_dec_stat(A_INT);
				do_dec_stat(A_WIS);
				do_dec_stat(A_DEX);
				do_dec_stat(A_CON);
				do_dec_stat(A_CHR);
			}

			break;
		}

		/* Standard damage -- also poisons / mutates player */
		/*:::���˔\�F�ϐ����Ȃ����16%�Ŏ��ȕϗe�A4%�œˑR�ψفA�����17%�ő����N�H*/
		///sys ���˔\�����͑M���E�Ή��̕������ɕύX
		case GF_NUKE:
		{
			int dam_fire, dam_lite;
			bool double_resist = IS_OPPOSE_FIRE();
#ifdef JP
			if (fuzzy) msg_print("���͂ȔM���ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by radiation!");
#endif

			dam_fire = dam / 2;
			dam_lite = dam / 2;

			//v1.1.91 �Ζ��n�`�Ή��_���[�W
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam_fire = oil_field_damage_mod(dam_fire, y, x);


			//v1.1.59�T�j�[�~���N��������荞��
			sunny_charge_sunlight(dam_lite);


			if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
				dam_lite = 0;
			else if(p_ptr->pclass == CLASS_HECATIA && p_ptr->tim_general[0])
				dam_lite = 0;
			else dam_lite = mod_lite_dam(dam_lite);

			if (!blind && !p_ptr->resist_blind && !p_ptr->resist_lite && !CHECK_MULTISHADOW() && dam_lite)
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam_lite, killer, monspell);

			if (dam_lite && p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
				msg_print("�M���̂��ߔ񕨎��I�ȉe�̑��݂ł����Ȃ��Ȃ����B");

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}


			if(!p_ptr->is_dead)get_damage += fire_dam(dam_fire, killer, 0);

			break;
		}

		/* Standard damage */
		case GF_MISSILE:
		{
#ifdef JP
			if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Holy Orb -- Player only takes partial damage */
		///sys res �_���U��
		case GF_HOLY_FIRE:
		{
#ifdef JP
			if (fuzzy) msg_print("�������炩�Ȃ��̂ōU�����ꂽ�I");
			//if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif
			///mod140103 �j�בϐ��E��_�l���p�֐������
			dam = mod_holy_dam(dam);

			if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > 0)
			{
				hina_gain_yaku(-(dam * 4));
				if(!p_ptr->magic_num1[0])
					msg_print("�j�׍U���Ŗ�S�ď����Ă��܂����I");
				else
					msg_print("�j�׍U���Ŗ���������Ă��܂����I");
			}

			/*
			if (p_ptr->align > 10)
				dam /= 2;
			else if (p_ptr->align < -10)
				dam *= 2;
			*/
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			if (p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
#ifdef JP
				msg_print("�j�ׂ̌����󂯂Ĕ񕨎��I�ȉe�̑��݂ł����Ȃ��Ȃ����B");
#else
				msg_print("The light forces you out of your incorporeal shadow form.");
#endif

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}


			break;
		}

		case GF_HELL_FIRE:
		///sys res �w���t�@�C�A�U��
		{
#ifdef JP
			if (fuzzy) msg_print("��������킵�����̂ōU�����ꂽ�I");
			//if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif


			if(p_ptr->special_defense & SD_HELLFIRE_BARRIER)
			{
				msg_print("�傢�Ȃ鎜�߂����Ȃ���n���̋Ɖ΂��������I");
				dam = 0;
				p_ptr->special_defense &= ~(SD_HELLFIRE_BARRIER);
				p_ptr->redraw |= (PR_STATUS);
				break;
			}

			if (prace_is_(RACE_TENNIN) || prace_is_(RACE_LUNARIAN))
			{
				msg_print("�n���̋Ɖ΂����Ȃ��̐���ȓ��̂�[���������I");
				dam = dam * 4 / 3;
			}
			else if ((prace_is_(RACE_GYOKUTO)&&p_ptr->pclass != CLASS_UDONGE))
			{
				msg_print("�n���̋Ɖ΂����Ȃ��̐���ȓ��̂��������I");
				dam = dam * 6 / 5;
			}
			else if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_HELL)
			{
				msg_print("�n���̋Ɖ΂����Ȃ��̐���ȓ��̂�[���������I");
				dam = dam * 3 / 2;
			}

			if(p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME)
			{
				dam = dam * 6 / (randint1(5) + 7);
			}
			if(p_ptr->pclass == CLASS_HECATIA)
			{
				msg_print("���Ȃ��ɂ͑ϐ�������B");
				dam /= 2;
			}

			/*
			if (p_ptr->align > 10)
				dam *= 2;
			*/
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Arrow -- XXX no dodging */
		case GF_ARROW:
		{
#ifdef JP
			if (fuzzy) msg_print("�����s�����̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something sharp!");
#endif

			else if ((!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_ZANTETSU) || (!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_LARM].name1 == ART_ZANTETSU))
			{
#ifdef JP
				msg_print("����a��̂Ă��I");
#else
				msg_print("You cut down the arrow!");
#endif
				break;
			}

			if(p_ptr->kamioroshi & KAMIOROSHI_KANAYAMAHIKO) 
			{
				msg_print("���������R�F��������𕪉������I");
				break;
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Plasma -- XXX No resist */
	///sys res �v���Y�}
		///mod140103 �v���Y�}�ϐ��@�Γd�Ŕ����������@�����N�O�͏����y��
		case GF_PLASMA:
		{

#ifdef JP
			if (fuzzy) msg_print("�����ƂĂ��M�����̂ōU�����ꂽ�I");
#endif

			//v1.1.91 �Ζ��n�`�Ή��_���[�W
			if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) dam = oil_field_damage_mod(dam, y, x);


			get_damage = fire_dam(dam/2, killer, monspell);
			if(!p_ptr->is_dead)get_damage +=  elec_dam(dam/2, killer, monspell);

			if (!p_ptr->resist_sound && !CHECK_MULTISHADOW())
			{
				int k = (randint1((dam > 50) ? 25 : (dam / 2 + 1)));
				(void)set_stun(p_ptr->stun + k);
			}
			break;
		}

		/* Nether -- drain experience */
		/*:::�n�������@�H��̋z��������*/
		case GF_NETHER:
		{
#ifdef JP
			if (fuzzy) msg_print("�n���̗͂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by nether forces!");
#endif
			if(p_ptr->pclass == CLASS_HINA && dam > 0)
			{
				int yaku;
				if(p_ptr->magic_num1[0] > 9999)
				{
					yaku = dam;
					msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				}
				else
				{
					yaku = randint1(dam);
					msg_print("���Ȃ��͍U���̈ꕔ���Ƃ��ċz�������B");
				}
				hina_gain_yaku(yaku);
				dam -= yaku;
			}
			if (prace_is_(RACE_LUNARIAN))
			{
				msg_print("���Ȃ��̓��̂��q�ꂽ�͂ɏ�����ꂽ�I");
				dam = dam * 4 / 3;
			}

			//���҂̐_�̒n���Ɖu
			if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 61)
			{
				dam = 0;
				msg_print("���Ȃ��ɂ͊��S�ȑϐ�������I");
				learn_spell(monspell);
				break;
			}
			//v1.1.15
			else if(p_ptr->pclass == CLASS_HECATIA)
			{
				dam = 0;
				msg_print("���Ȃ��ɂ͊��S�ȑϐ�������I");
				learn_spell(monspell);
				break;
			}
			else if (prace_is_(RACE_HANIWA))
			{
				dam = 0;
				msg_print("���Ȃ��ɂ͊��S�ȑϐ�������I");
				learn_spell(monspell);
				break;
			}
			else if (p_ptr->resist_neth)
			{
				if (!(prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST) || p_ptr->pclass == CLASS_YUMA))
					dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW()) drain_exp(200 + (p_ptr->exp / 100), 200 + (p_ptr->exp / 1000), 75);

			if ((p_ptr->pclass == CLASS_YOSHIKA || prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST) || p_ptr->pclass == CLASS_YUMA) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("�n���̗͂��z�������I");
#else
				msg_print("You feel invigorated!");
#endif

				hp_player(dam / 4);
				learn_spell(monspell);
			}
			else
			{
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}

			break;
		}

		/* Water -- stun/confuse */
		///mod131229 sys res ���U��
		case GF_WATER:
		case GF_HOLY_WATER: //�ꉞ�ǉ����Ă����������󂯂�\��͂Ȃ��B���̑����̍U����G������Ȃ�j�׎�_������ǉ����邱��
		{
#ifdef JP
			if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something wet!");
#endif
			dam = mod_water_dam(dam);

			if( dam == 0 ) break;

			//if (p_ptr->resist_water)
			//{
			//	dam /= 2;
			//}
			//if (!CHECK_MULTISHADOW())

			if (p_ptr->chp >= dam && p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0] && !CHECK_MULTISHADOW())
			{
				msg_print("�������Ď���������Ă��܂����I");
				p_ptr->magic_num1[0] = 1;
				p_ptr->update |= (PU_BONUS);
			}
			else if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > 0)
			{
				hina_gain_yaku(-(dam * 2));
				if(!p_ptr->magic_num1[0])
					msg_print("���Ŗ�S�ė���Ă��܂����I");
				else
					msg_print("���Ŗ��������o�Ă��܂����I");
			}

			if (!CHECK_MULTISHADOW() && !p_ptr->resist_water)
			{
				///mod131229 ���U���̞N�O�ƃA�C�e���j��͐��ϐ��Ŕ��肷�邱�Ƃɂ���
				//if (!p_ptr->resist_sound)
				{
					set_stun(p_ptr->stun + randint1(30));
				}
				if (!p_ptr->resist_conf)
				{
					set_confused(p_ptr->confused + randint1(5) + 5);
				}

				if (one_in_(5))
				{
					inven_damage(set_cold_destroy, 3);
				}
			}
			///mod140821 �G���W�j�A�̋@�B���̏Ⴗ�鏈��
			engineer_malfunction(GF_WATER, dam);

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}


		//v1.1.86 �G���g���Ă��邱�ƂɂȂ����̂Œǉ�
		case GF_RAINBOW:
		{
			int water_dam, lite_dam;
			water_dam = (dam + 1) / 2;
			lite_dam = (dam + 1) / 2;

			if (fuzzy) msg_print("�����ōU�����ꂽ�I");

			//v1.1.59�T�j�[�~���N��������荞��
			sunny_charge_sunlight(lite_dam);

			if (CHECK_FAIRY_TYPE == 37)
			{
				msg_print("���Ȃ��͌��̈ꕔ���h�{�ɂ����I");
				set_food(MIN(p_ptr->food + lite_dam, PY_FOOD_MAX - 1));
				lite_dam = lite_dam * 4 / 5;
			}

			if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
			{
				lite_dam = 0;
			}
			else if (p_ptr->pclass == CLASS_HECATIA && p_ptr->tim_general[0])
			{

				lite_dam = 0;
			}
			else
			{
				lite_dam = mod_lite_dam(lite_dam);

				if (!CHECK_MULTISHADOW() && !p_ptr->resist_conf && p_ptr->resist_lite)
				{
					set_confused(p_ptr->confused + randint1(5) + 5);
				}
			}

			//���_���[�W���󂯂邪�A�A�C�e���j��A�G���W�j�A�̋@�B�s���A��̎�������͋N����Ȃ����Ƃɂ���
			water_dam = mod_water_dam(water_dam);

			dam = lite_dam + water_dam;
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);


			if (p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
				msg_print("�M���̂��ߔ񕨎��I�ȉe�̑��݂ł����Ȃ��Ȃ����B");

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}

			break;
		}


		/* Chaos -- many effects */
		case GF_CHAOS:
		{
#ifdef JP
			if (fuzzy) msg_print("�������̔g���ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by a wave of anarchy!");
#endif

			if (p_ptr->resist_chaos)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}

			if (!CHECK_MULTISHADOW())
			{
				if (!p_ptr->resist_conf)
				{
					(void)set_confused(p_ptr->confused + randint0(20) + 10);
				}
				if (!p_ptr->resist_chaos)
				{
					(void)set_image(p_ptr->image + randint1(10));
					if (one_in_(3))
					{
#ifdef JP
						msg_print("���Ȃ��̐g�̂̓J�I�X�̗͂ŔP���Ȃ���ꂽ�I");
#else
						msg_print("Your body is twisted by chaos!");
#endif

						(void)gain_random_mutation(0);
					}
				}
				if (!p_ptr->resist_neth && !p_ptr->resist_chaos)
				{
					drain_exp(5000 + (p_ptr->exp / 100), 500 + (p_ptr->exp / 1000), 75);
				}

				if (!p_ptr->resist_chaos || one_in_(9))
				{
					inven_damage(set_elec_destroy, 2);
					inven_damage(set_fire_destroy, 2);
				}
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Shards -- mostly cutting */
		case GF_SHARDS:
		{
#ifdef JP
			if (fuzzy) msg_print("�����s�����̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something sharp!");
#endif
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
			{
				msg_print("���Ȃ��͑S���̖������B");
				dam = 0; 
				break;
			}

			if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				dam += dam / 4;
			else if (prace_is_(RACE_HANIWA))
			{
				msg_print("�����̔j�Ђ����Ȃ����������I");
				dam += dam / 4;
			}


			if (p_ptr->resist_shard)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				(void)set_cut(p_ptr->cut + dam);
			}

			if (!p_ptr->resist_shard || one_in_(13))
			{
				inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Sound -- mostly stunning */
		case GF_SOUND:
		{
#ifdef JP
			if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by a loud noise!");
#endif
			///mod141106 ���q�����Ɖu
			if(p_ptr->pclass == CLASS_KYOUKO && p_ptr->lev > 29) break;


			if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				dam += dam / 4;
			else if (prace_is_(RACE_HANIWA))
			{
				msg_print("�Ռ������Ȃ����������I");
				dam += dam / 4;
			}


			if (p_ptr->resist_sound)
			{
				dam *= 5; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
				(void)set_stun(p_ptr->stun + k);
			}

			if (!p_ptr->resist_sound || one_in_(13))
			{
				inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Pure confusion */
		case GF_CONFUSION:
		{
#ifdef JP
			if (fuzzy) msg_print("��������������̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something puzzling!");
#endif

			if (p_ptr->resist_conf)
			{
				dam *= 5; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				(void)set_confused(p_ptr->confused + randint1(20) + 10);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}





		/* Disenchantment -- see above */
		case GF_DISENCHANT:
		{
#ifdef JP
			if (fuzzy) msg_print("���������Ȃ����̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something static!");
#endif

			dam = mod_disen_dam(dam);



			if (!p_ptr->resist_disen && !CHECK_MULTISHADOW())
			{
				(void)apply_disenchant(0,0);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Nexus -- see above */
		case GF_NEXUS:
		{
#ifdef JP
			if (fuzzy) msg_print("����������͂ȍU�����󂯂��I");
			//if (fuzzy) msg_print("������Ȃ��̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something strange!");
#endif
			if (p_ptr->resist_time)
			//if (p_ptr->resist_nexus)
			{
				dam *= 6; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				apply_nexus(m_ptr);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Force -- mostly stun */
		case GF_FORCE:
		{
#ifdef JP
			if (fuzzy) msg_print("�C�̃p���[�ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by kinetic force!");
#endif

			if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			{
				dam -= p_ptr->lev * 3;
				if(dam < 1)
				{
					msg_print("���Ȃ��͕����ꂽ�C��e�����I");
					dam = 0;
					break;
				}

			}
			else if (prace_is_(RACE_LUNARIAN))
			{
				msg_print("���Ȃ��̓��̂��q�ꂽ�͂ɏ�����ꂽ�I");
				dam = dam * 4 / 3;
			}


			if (!p_ptr->resist_sound && !CHECK_MULTISHADOW())
			{
				(void)set_stun(p_ptr->stun + randint1(20));
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}


		/* Rocket -- stun, cut */
		case GF_ROCKET:
		{
#ifdef JP
			if (fuzzy) msg_print("�������������I");
#else
			if (fuzzy) msg_print("There is an explosion!");
#endif
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
			{
				msg_print("���Ȃ��͔����̒����疳���Ō��ꂽ�B");
				dam = 0; 
				break;
			}

			if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				dam += dam / 4;
			else if (prace_is_(RACE_HANIWA))
			{
				msg_print("����Ȕ��������Ȃ����������I");
				dam += dam / 4;
			}

			if (!p_ptr->resist_sound && !CHECK_MULTISHADOW())
			{
				(void)set_stun(p_ptr->stun + randint1(20));
			}

			if (p_ptr->resist_shard)
			{
				dam /= 2;
			}
			else if (!CHECK_MULTISHADOW())
			{
				(void)set_cut(p_ptr->cut + (dam / 2));
			}

			if (!p_ptr->resist_shard || one_in_(12))
			{
				inven_damage(set_cold_destroy, 3);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Inertia -- slowness */
		case GF_INACT:
		{
#ifdef JP
			if (fuzzy) msg_print("�����x�����̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something slow!");
#endif
			if((IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_RES_INER) || p_ptr->pclass == CLASS_SAKI)
			{
				msg_print("���Ȃ��ɂ͑ϐ�������B");
				dam /= 2;
			}

			else if (!CHECK_MULTISHADOW()) (void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Lite -- blinding */
		case GF_LITE:
		{
#ifdef JP
			if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif
			///mod140122 �M���_���[�W�v�Z���@�ύX
			/*

			if (p_ptr->resist_lite)
			{
				dam *= 4; dam /= (randint1(4) + 7);
			}
			else if (!blind && !p_ptr->resist_blind && !CHECK_MULTISHADOW())
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}

			if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
			{
#ifdef JP
				if (!CHECK_MULTISHADOW()) msg_print("���œ��̂��ł����ꂽ�I");
#else
				if (!CHECK_MULTISHADOW()) msg_print("The light scorches your flesh!");
#endif

				dam *= 2;
			}

			else if (prace_is_(RACE_S_FAIRY))
			{
				dam = dam * 4 / 3;
			}
			if (p_ptr->wraith_form) dam *= 2;
			*/

			//v1.1.59�T�j�[�~���N��������荞��
			sunny_charge_sunlight(dam);

			if(CHECK_FAIRY_TYPE == 37)
			{
				msg_print("���Ȃ��͌��̈ꕔ���h�{�ɂ����I");
				set_food(MIN(p_ptr->food + dam,PY_FOOD_MAX-1));
				dam = dam * 4 / 5;
				if(!dam) break;
			}

			if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
			{
				dam = 0;
				break;
			}
			else if(p_ptr->pclass == CLASS_HECATIA && p_ptr->tim_general[0])
			{
				msg_print("���ˋ���������e�����I");

				dam = 0;
				break;
			}
			else dam = mod_lite_dam(dam);

			if(!dam) break;

			if (!p_ptr->resist_lite && !blind && !p_ptr->resist_blind && !CHECK_MULTISHADOW())
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			if (p_ptr->wraith_form && !CHECK_MULTISHADOW())
			{
				p_ptr->wraith_form = 0;
				msg_print("�M���̂��ߔ񕨎��I�ȉe�̑��݂ł����Ȃ��Ȃ����B");

				p_ptr->redraw |= PR_MAP;
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS | PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				/* Redraw status bar */
				p_ptr->redraw |= (PR_STATUS);

			}

			break;
		}

		/* Dark -- blinding */
		case GF_DARK:
		{
#ifdef JP
			if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			if(p_ptr->pclass == CLASS_HINA && dam > 0)
			{
				int yaku;
				if(p_ptr->magic_num1[0] > 9999)
				{
					yaku = dam;
					msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				}
				else
				{
					yaku = randint1(dam);
					msg_print("���Ȃ��͍U���̈ꕔ���Ƃ��ċz�������B");
				}
				hina_gain_yaku(yaku);
				dam -= yaku;
			}

			if (prace_is_(RACE_LUNARIAN))
			{
				msg_print("���Ȃ��̓��̂��q�ꂽ�͂ɏ�����ꂽ�I");
				dam = dam * 4 / 3;
			}

			if (p_ptr->resist_dark)
			{
				if (p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 39)
				{
#ifdef JP
					msg_print("���Ȃ��͈ł̗͂��z�������B");
#endif
					hp_player(dam / 4);
				}

				dam *= 4; dam /= (randint1(4) + 7);
				///sys res �z���S�Í������������@�c���H�����H
				if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE) || (p_ptr->mimic_form == MIMIC_NUE) || p_ptr->wraith_form || (p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 19) || p_ptr->tim_unite_darkness) dam = 0;
			}
			else if (!blind && !p_ptr->resist_blind && !CHECK_MULTISHADOW())
			{
				(void)set_blind(p_ptr->blind + randint1(5) + 2);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}
		case GF_DISTORTION:
		{
#ifdef JP
			if (fuzzy) msg_print("������Ȃ��̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			if (p_ptr->resist_time)
			{
				dam *= 4; dam /= (randint1(4) + 7);
			}
			else if (!CHECK_MULTISHADOW())
			{
				int tmp;
				msg_print("���Ȃ��͈���Ԃɓۂݍ��܂ꂽ�I");
				teleport_player(50+randint1(50), TELEPORT_PASSIVE);
				for(tmp=0;tmp<6;tmp++)if(one_in_(2)) do_dec_stat(tmp);
			}
			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Time -- bolt fewer effects XXX */
		case GF_TIME:
		{
#ifdef JP
			if (fuzzy) msg_print("�ߋ�����̏Ռ��ɍU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by a blast from the past!");
#endif

			if (p_ptr->resist_time)
			{
				dam *= 4;
				dam /= (randint1(4) + 7);
#ifdef JP
				msg_print("���Ԃ��ʂ�߂��Ă����C������B");
#else
				msg_print("You feel as if time is passing you by.");
#endif
			}
			else if (!CHECK_MULTISHADOW())
			{
				switch (randint1(10))
				{
					case 1: case 2: case 3: case 4: case 5:
					{
						if (p_ptr->prace == RACE_ANDROID) break;
#ifdef JP
						msg_print("�l�����t�߂肵���C������B");
#else
						msg_print("You feel life has clocked back.");
#endif

						lose_exp(100 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
						break;
					}

					case 6: case 7: case 8: case 9:
					{
						switch (randint1(6))
						{
#ifdef JP
							case 1: k = A_STR; act = "����"; break;
							case 2: k = A_INT; act = "������"; break;
							case 3: k = A_WIS; act = "������"; break;
							case 4: k = A_DEX; act = "��p��"; break;
							case 5: k = A_CON; act = "���N��"; break;
							case 6: k = A_CHR; act = "������"; break;
#else
							case 1: k = A_STR; act = "strong"; break;
							case 2: k = A_INT; act = "bright"; break;
							case 3: k = A_WIS; act = "wise"; break;
							case 4: k = A_DEX; act = "agile"; break;
							case 5: k = A_CON; act = "hale"; break;
							case 6: k = A_CHR; act = "beautiful"; break;
#endif
						}

#ifdef JP
						msg_format("���Ȃ��͈ȑO�ق�%s�Ȃ��Ȃ��Ă��܂���...�B", act);
#else
						msg_format("You're not as %s as you used to be...", act);
#endif

						p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 3) / 4;
						if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
						p_ptr->update |= (PU_BONUS);
						break;
					}

					case 10:
					{
#ifdef JP
						msg_print("���Ȃ��͈ȑO�قǗ͋����Ȃ��Ȃ��Ă��܂���...�B");
#else
						msg_print("You're not as powerful as you used to be...");
#endif

						for (k = 0; k < 6; k++)
						{
							p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 7) / 8;
							if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
						}
						p_ptr->update |= (PU_BONUS);
						break;
					}
				}
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Gravity -- stun plus slowness plus teleport */
		case GF_GRAVITY:
		{

			if(CHECK_FAIRY_TYPE == 40)
			{
				msg_print("���Ȃ��ɂ͊��S�ȑϐ�������I");
				dam = 0;
				break;
			}

			if (fuzzy) msg_print("�����d�����̂ōU�����ꂽ�I");
			msg_print("���ӂ̏d�͂��䂪�񂾁B");


			if (!CHECK_MULTISHADOW())
			{
				if (!p_ptr->resist_time) teleport_player(5, TELEPORT_PASSIVE);
				if (!p_ptr->levitation && !p_ptr->resist_time)
					(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);
				if (!(p_ptr->resist_sound || p_ptr->levitation || p_ptr->resist_time))
				{
					int k = (randint1((dam > 90) ? 35 : (dam / 3 + 5)));
					(void)set_stun(p_ptr->stun + k);
				}
			}
			if (p_ptr->levitation)
			{
				dam = (dam * 2) / 3;
			}
			if (p_ptr->resist_time)
			{
				dam = (dam * 2) / 3;
			}
			if (!p_ptr->resist_time &&  (!p_ptr->levitation || one_in_(13)))
			{
				inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}





		/*::: �������� */
		case GF_TORNADO:
		{
			int wgt = (p_ptr->total_weight / 20);
#ifdef JP
			if (fuzzy) msg_print("�\���ōU�����ꂽ�I");
#endif

			if( (p_ptr->pclass == CLASS_AYA || p_ptr->pclass == CLASS_SANAE || p_ptr->pclass == CLASS_FUTO) && p_ptr->lev > 29)
			{
				msg_print("���Ȃ��͗�����ł��������I");
				dam =0;
				break;
			}
			if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
			{
				msg_print("���Ȃ��̋���ȑ͔̂�΂���Ȃ������B");
				dam /= 4;
			}
			/*:::�d�ʂ��d���ƒ�R�@100Kg��Lev100�̓G�ɑς�����*/
			else if( wgt > rlev)
			{
				msg_print("�ו����d���Ĕ�΂���Ȃ������B");
				dam /= 3;
			}
			else if (p_ptr->levitation)
			{
				msg_print("���Ȃ��͐�����΂��ꂽ���󒆂ő̐��𗧂Ē������B");
				dam /= 2;
				teleport_player(5+randint1(5), TELEPORT_PASSIVE);
			}

			else if (!CHECK_MULTISHADOW())
			{
				msg_print("���Ȃ��͐�����΂��ꂽ�I");
				teleport_player(10+randint0(20), TELEPORT_PASSIVE);
				if(one_in_(3))inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/*::: �d�ԑ��� */
		case GF_TRAIN:
		{
			if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
			{
				msg_print("���Ȃ��̋���ȑ̂͒��˔�΂���Ȃ������B");
			}
			else if (!CHECK_MULTISHADOW())
			{
				if (fuzzy) msg_print("�Ȃɂ�����Ȃ��̂ɒ��˔�΂��ꂽ�I");
				else msg_print("���Ȃ��͒��˔�΂��ꂽ�I");
				teleport_player(10+randint0(20), TELEPORT_PASSIVE);
				if(one_in_(3))inven_damage(set_cold_destroy, 2);
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Standard damage */
		case GF_DISINTEGRATE:
		{
#ifdef JP
			if (fuzzy) msg_print("�����ȃG�l���M�[�ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by pure energy!");
#endif
			if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_ROCK)
			{
				msg_print("���Ȃ��̍d���Ȑg�̂��������Ă����I");
				dam = dam * 3 / 2;
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		case GF_OLD_HEAL:
		{
#ifdef JP
			if (fuzzy) msg_print("���炩�̍U���ɂ���ċC�����悭�Ȃ����B");
#else
			if (fuzzy) msg_print("You are hit by something invigorating!");
#endif

			(void)hp_player(dam);
			dam = 0;
			break;
		}

		case GF_OLD_SPEED:
		{
#ifdef JP
			if (fuzzy) msg_print("�����ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something!");
#endif

			(void)set_fast(p_ptr->fast + randint1(5), FALSE);
			dam = 0;
			break;
		}

		//v1.1.97 �Ȃ������������̂Œǉ�
		case GF_OLD_CONF:
		{
			if (fuzzy) msg_print("�����ōU�����ꂽ�I");

			if (!p_ptr->resist_conf && !CHECK_MULTISHADOW())
			{
				(void)set_confused(p_ptr->confused + randint1(10) + 10);
			}
			dam = 0;
			break;
		}

		case GF_OLD_SLOW:
		{
#ifdef JP
			if (fuzzy) msg_print("�������̂ɗ��݂����C������I");
#else
			if (fuzzy) msg_print("You are hit by something slow!");
#endif

			(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);
			break;
		}

		case GF_OLD_SLEEP:
		{
			if (p_ptr->free_act)  break;
#ifdef JP
			/*if (fuzzy) */ if(!p_ptr->paralyzed) msg_print("���Ȃ��͖����Ă��܂����I");
#else
			if (fuzzy) msg_print("You fall asleep!");
#endif

			if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print("�����X�b�L�������I�ł������Y��Ă���悤�ȁB");

			if (ironman_nightmare)
			{
#ifdef JP
				msg_print("���낵�����i�����ɕ�����ł����B");
#else
				msg_print("A horrible vision enters your mind.");
#endif

				/* Pick a nightmare */
				get_mon_num_prep(get_nightmare, NULL);

				/* Have some nightmares */
				have_nightmare(get_mon_num(MAX_DEPTH));

				/* Remove the monster restriction */
				get_mon_num_prep(NULL, NULL);
			}

			set_paralyzed(p_ptr->paralyzed + dam);
			dam = 0;
			break;
		}

		/* Pure damage */
		case GF_MANA:
		case GF_SEEKER:
		case GF_SUPER_RAY:
		{
#ifdef JP
			if (fuzzy) msg_print("���@�̃I�[���ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by an aura of magic!");
#endif

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			break;
		}

		/* Pure damage */
		case GF_PSY_SPEAR:
		case GF_GUNGNIR:
		{
#ifdef JP
			if (fuzzy) msg_print("���͂ȃG�l���M�[�̉�ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by an energy!");
#endif

			get_damage = take_hit(DAMAGE_FORCE, dam, killer, monspell);
			break;
		}

		/* Pure damage */
		case GF_METEOR:
		{
#ifdef JP
			if (fuzzy) msg_print("�������󂩂炠�Ȃ��̓���ɗ����Ă����I");
#else
			if (fuzzy) msg_print("Something falls from the sky on you!");
#endif

			if(CHECK_FAIRY_TYPE == 54)
			{
				msg_print("������覐΂̔j�Ђ͂��Ȃ��̓��̏�������߂Ă������I");
				dam = 0;
				break;
			}
			if(p_ptr->pclass == CLASS_STAR)
			{
				msg_print("���Ȃ��͊�炩�̑ϐ����������B");
				dam = dam * 2 / 3;
			}

			get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			if (!p_ptr->resist_shard || one_in_(13))
			{
				if (!p_ptr->immune_fire) inven_damage(set_fire_destroy, 2);
				inven_damage(set_cold_destroy, 2);
			}

			break;
		}

		/* Ice -- cold plus stun plus cuts */
		case GF_ICE:
		{
#ifdef JP
			if (fuzzy) msg_print("�����s���₽�����̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something sharp and cold!");
#endif

			get_damage = cold_dam(dam, killer, monspell);
			if (!CHECK_MULTISHADOW())
			{
				if (!p_ptr->resist_shard)
				{
					(void)set_cut(p_ptr->cut + damroll(5, 8));
				}
				if (!p_ptr->resist_sound)
				{
					(void)set_stun(p_ptr->stun + randint1(15));
				}

				if ((!(p_ptr->resist_cold || IS_OPPOSE_COLD())) || one_in_(12))
				{
					if (!p_ptr->immune_cold) inven_damage(set_cold_destroy, 3);
				}
			}

			break;
		}

		/* Death Ray */
		case GF_DEATH_RAY:
		{
#ifdef JP
			if (fuzzy) msg_print("�������ɗ₽�����̂ōU�����ꂽ�I");
#else
			if (fuzzy) msg_print("You are hit by something extremely cold!");
#endif

			if(p_ptr->pclass == CLASS_HINA && dam > 0)
			{
				int yaku;
				if(p_ptr->magic_num1[0] > 9999)
				{
					yaku = dam;
					msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				}
				else
				{
					yaku = randint1(dam);
					msg_print("���Ȃ��͍U���̈ꕔ���Ƃ��ċz�������B");
				}
				hina_gain_yaku(yaku);
				dam -= yaku;
			}
			else if (p_ptr->mimic_form)
			{
				if (!(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING))
					get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			else
			{

				if (RACE_RESIST_DEATH_RAY)
				{
					dam = 0;
					break;
				}
			}

			break;
		}

		/* Drain mana */
		case GF_DRAIN_MANA:
		{
			if(diehardmind())
			{
				msg_print("���Ȃ��͐��_�U���ɖ��������B");
				break;
			}
			else 			if (CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("�U���͌��e�ɖ������A���Ȃ��ɂ͓͂��Ȃ������B");
#else
				msg_print("The attack hits Shadow, you are unharmed!");
#endif
			}
			else if (p_ptr->csp)
			{
				/* Basic message */
#ifdef JP
				if (who > 0) msg_format("%^s�ɐ��_�G�l���M�[���z������Ă��܂����I", m_name);
				else msg_print("���_�G�l���M�[���z������Ă��܂����I");
#else
				if (who > 0) msg_format("%^s draws psychic energy from you!", m_name);
				else msg_print("Your psychic energy is drawn!");
#endif

				/* Full drain */
				if (dam >= p_ptr->csp)
				{
					dam = p_ptr->csp;
					p_ptr->csp = 0;
					p_ptr->csp_frac = 0;
				}

				/* Partial drain */
				else
				{
					p_ptr->csp -= dam;
				}

				learn_spell(monspell);

				/* Redraw mana */
				p_ptr->redraw |= (PR_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);

				if (who > 0)
				{
					/* Heal the monster */
					if (m_ptr->hp < m_ptr->maxhp)
					{
						/* Heal */
						m_ptr->hp += (6 * dam);
						if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == who) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == who) p_ptr->redraw |= (PR_UHEALTH);

						/* Special message */
						if (m_ptr->ml)
						{
#ifdef JP
							msg_format("%^s�͋C�����ǂ��������B", m_name);
#else
							msg_format("%^s appears healthier.", m_name);
#endif
						}
					}
				}
			}

			dam = 0;
			break;
		}

		/* Mind blast */
		case GF_MIND_BLAST:
		case GF_REDEYE:
		{
			if(diehardmind())
			{
				msg_print("���Ȃ��͖��������B");
				break;

			}
			else if ((randint0(100 + rlev / 2) < MAX(5, p_ptr->skill_sav)) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				if (!CHECK_MULTISHADOW())
				{
#ifdef JP
					msg_print("��I�G�l���M�[�Ő��_���U�����ꂽ�B");
#else
					msg_print("Your mind is blasted by psyonic energy.");
#endif

					if (!p_ptr->resist_conf)
					{
						(void)set_confused(p_ptr->confused + randint0(4) + 4);
					}

					if (!p_ptr->resist_chaos && one_in_(3))
					{
						(void)set_image(p_ptr->image + randint0(250) + 150);
					}

					p_ptr->csp -= 50;
					if (p_ptr->csp < 0)
					{
						p_ptr->csp = 0;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= PR_MANA;
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* Brain smash */
		case GF_BRAIN_SMASH:
		{
			if(diehardmind())
			{
				msg_print("���Ȃ��͖��������B");
				break;

			}
			else 			if ((randint0(100 + rlev / 2) < MAX(5, p_ptr->skill_sav)) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				if (!CHECK_MULTISHADOW())
				{
#ifdef JP
					msg_print("��I�G�l���M�[�Ő��_���U�����ꂽ�B");
#else
					msg_print("Your mind is blasted by psionic energy.");
#endif

					p_ptr->csp -= 100;
					if (p_ptr->csp < 0)
					{
						p_ptr->csp = 0;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= PR_MANA;
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				if (!CHECK_MULTISHADOW())
				{
					if (!p_ptr->resist_blind)
					{
						(void)set_blind(p_ptr->blind + 8 + randint0(8));
					}
					if (!p_ptr->resist_conf)
					{
						(void)set_confused(p_ptr->confused + randint0(4) + 4);
					}
					if (!p_ptr->free_act)
					{
						(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
					}
					(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);

					while (randint0(100 + rlev / 2) > (MAX(5, p_ptr->skill_sav)))
						(void)do_dec_stat(A_INT);
					while (randint0(100 + rlev / 2) > (MAX(5, p_ptr->skill_sav)))
						(void)do_dec_stat(A_WIS);

					if (!p_ptr->resist_chaos)
					{
						(void)set_image(p_ptr->image + randint0(250) + 150);
					}
				}
			}
			break;
		}

		/* �R�Y�~�b�N�z���[ ���g�����ɂ��Ă�����*/
		case GF_COSMIC_HORROR:
		{
			if(diehardmind())
			{
				msg_print("���Ȃ��͖��������B");
				break;
			}
			//skill_sav��Lev50�Ő��X100���x�@�����@�A�~����130���炢
			else if (immune_insanity() || p_ptr->resist_insanity && p_ptr->skill_sav > (30+randint1(rlev)))
			{
#ifdef JP
				msg_print("���������Ȃ��̐��_�͉e�����󂯂Ȃ������I");
#else
				msg_print("You resist the effects!");
#endif
			}
			else
			{
				int blast;
#ifdef JP
				msg_print("���Ȃ��̐��_�͉F���̐[���ւƓۂݍ��܂ꂽ�B");
#else
				msg_print("Your mind is blasted by psionic energy.");
#endif
				if (p_ptr->resist_insanity) dam/=2;
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

				if(!p_ptr->resist_insanity)
				{
					int div;

					blast = randint0(rlev / (p_ptr->resist_fear?2:1));
					
					if(IS_VULN_FEAR) blast += 12 + randint1(12);

					if(blast < 10)
					{
						if(!p_ptr->resist_conf) set_confused(100+randint0(100));
					}
					else if(blast < 20)
					{
						if(!p_ptr->resist_chaos) set_image(100+randint0(100));
					}
					else if(blast < 30)
					{
						set_stun(p_ptr->stun + 20 + randint0(31));
					}
					else if(blast < 40)
					{
						int cnt;
						for(cnt=0;cnt<6;cnt++) do_dec_stat(cnt);
					}
					else if(blast < 60)
					{
						msg_print("�������ڂ̑O�ɘȂ�ł���B");
						summon_specific(0, py, px, dun_level, SUMMON_INSANITY, (PM_NO_PET));
					}
					else if(blast < 70)
					{
						p_ptr->csp = 0;
						p_ptr->csp_frac = 0;
						p_ptr->redraw = PR_MANA;
						redraw_stuff();
						msg_print("���������߂��̂ɑS�Ă̐��_�͂��g���ʂ������E�E");
					}
					else if(blast < 80)
					{
						int muta[5] = {75,76,77,79,130};
						msg_print("�֒f�̒m���ɔ]���ς��؂�Ȃ��I");
						gain_random_mutation(muta[randint0(5)]);
					}
					else if(blast < 90)
					{
						int cnt;
						msg_print("�F���̐[�����眛�܂������݂����Ȃ���ǐՂ��Ă����I");
						for(cnt = randint1(3);cnt>0;cnt--)summon_specific(0, py, px, dun_level, SUMMON_INSANITY2, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
					}
					else
					{
						msg_print("���Ȃ��̓��̂͊������~�����B");
						if (p_ptr->free_act)
							set_paralyzed(p_ptr->paralyzed + randint1(3));
						else
							set_paralyzed(p_ptr->paralyzed + randint1(13));

					}
				}

			}
			break;
		}

		/* cause 1 */
		case GF_CAUSE_1:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				hina_gain_yaku(dam);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͍U����s�^�p���[�Ƃ��ċz�������B");
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 �􂢂͈Í��ϐ����n���ϐ��Ń_���[�W�������邱�Ƃɂ���
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				if (!CHECK_MULTISHADOW()) curse_equipment(15, 0);
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* cause 2 */
		case GF_CAUSE_2:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				hina_gain_yaku(dam);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͍U����s�^�p���[�Ƃ��ċz�������B");
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 �􂢂͈Í��ϐ����n���ϐ��Ń_���[�W�������邱�Ƃɂ���
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				if (!CHECK_MULTISHADOW()) curse_equipment(25, MIN(rlev / 2 - 15, 5));
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* cause 3 */
		case GF_CAUSE_3:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				hina_gain_yaku(dam);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͍U����s�^�p���[�Ƃ��ċz�������B");
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 �􂢂͈Í��ϐ����n���ϐ��Ń_���[�W�������邱�Ƃɂ���
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				if (!CHECK_MULTISHADOW()) curse_equipment(33, MIN(rlev / 2 - 15, 15));
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
			}
			break;
		}

		/* cause 4 */
		case GF_CAUSE_4:
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				hina_gain_yaku(dam);
				dam = 0;
			}	
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͍U����s�^�p���[�Ƃ��ċz�������B");
				p_ptr->magic_num1[1] += dam * 10;
				dam = 0;
			}
			else if ((randint0(100 + rlev / 2) < p_ptr->skill_sav) && !(m_ptr->r_idx == MON_KENSHIROU) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_print("���������͂𒵂˕Ԃ����I");
#else
				msg_print("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				///mod140321 �􂢂͈Í��ϐ����n���ϐ��Ń_���[�W�������邱�Ƃɂ���
				if(p_ptr->resist_dark || p_ptr->resist_neth) dam /= 2;
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				if (!CHECK_MULTISHADOW()) (void)set_cut(p_ptr->cut + damroll(10, 10));
			}
			break;
		}

		/* Hand of Doom */
		case GF_HAND_DOOM:
		{
			if(p_ptr->pclass == CLASS_CLOWNPIECE)
			{
				msg_format("���������͂𒵂˕Ԃ����I");
				break;
			}
			else if(p_ptr->pclass == CLASS_HINA)
			{
				msg_print("���Ȃ��͍U�����Ƃ��ċz�������B");
				hina_gain_yaku(500);
				dam = 0;
			}
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͍U����s�^�p���[�Ƃ��ċz�������B");
				p_ptr->magic_num1[1] += 5000;
				dam = 0;
			}
			else if ((randint0(100 + rlev/2) < p_ptr->skill_sav) && !CHECK_MULTISHADOW())
			{
#ifdef JP
				msg_format("���������͂𒵂˕Ԃ����I");
#else
				msg_format("You resist the effects!");
#endif
				learn_spell(monspell);
			}
			else
			{
				if (!CHECK_MULTISHADOW())
				{
#ifdef JP
					msg_print("���Ȃ��͖������܂��Ă����悤�Ɋ������I");
#else
					msg_print("You feel your life fade away!");
#endif
					curse_equipment(40, 20);
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, m_name, monspell);

				if (p_ptr->chp < 1) p_ptr->chp = 1; /* Paranoia */
			}
			break;
		}
		/* �t��������U���@�K���j�ł̎�+�؂菝+�o���l����+�S�\�͒ቺ */
		case GF_KYUTTOSHITEDOKA_N:
		{
			if(p_ptr->pclass == CLASS_CLOWNPIECE) dam /=10;

			msg_print("�̂��琶���͂��e����񂾋C������I");
			get_damage = take_hit(DAMAGE_ATTACK, dam, m_name, monspell);
			(void)set_cut(p_ptr->cut + damroll(1, 256));
			if (p_ptr->chp < 1) p_ptr->chp = 1; /* Paranoia */
			lose_exp(p_ptr->exp / 10);
			do_dec_stat(A_STR);
			do_dec_stat(A_INT);
			do_dec_stat(A_WIS);
			do_dec_stat(A_DEX);
			do_dec_stat(A_CON);
			do_dec_stat(A_CHR);

			break;
		}
		case GF_PUNISH_1:
		case GF_PUNISH_2:
		case GF_PUNISH_3:
		case GF_PUNISH_4:
		{

			if(  is_hurt_holy() <= 0 )
			{
#ifdef JP
				msg_print("���������Ȃ��ɂ͌��ʂ��Ȃ������B");
#endif
			}

			else if ( CHECK_MULTISHADOW() || (randint0(100 + rlev / 2) < p_ptr->skill_sav) )
			{
#ifdef JP
				msg_print("���������͂𒵂˕Ԃ����I");
#endif
				//learn_spell(monspell);
			}
			else
			{

				dam = mod_holy_dam(dam);

				if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > 0)
				{
					hina_gain_yaku(-(dam * 3));
					if(!p_ptr->magic_num1[0])
						msg_print("�������Ŗ�����Ă��܂����I");
					else
						msg_print("�������Ŗ���������Ă��܂����I");
				}

				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				if (!CHECK_MULTISHADOW()) (void)set_stun(p_ptr->stun + dam/5);
			}
			break;
		}

		case GF_WINDCUTTER:
		{

			if(randint1(p_ptr->ac + p_ptr->to_a) > 50)
			{
				msg_print("���̐n�͂��Ȃ��ɓ�����Ȃ������B");
			}
			else get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);

			break;
		}
		case GF_BANKI_BEAM:
		{
#ifdef JP
			if (fuzzy) msg_print("���g�̕������r�[���������ɓ��������悤���I");
#endif
			get_damage = take_hit(DAMAGE_ATTACK, dam, "�����̃r�[��", monspell);
			break;
		}

		//v1.1.97
		case GF_DEC_ATK:
		{
			if(!CHECK_MULTISHADOW())
				do_dec_stat(A_STR);
			dam = 0;
			break;
		}

		case GF_DEC_DEF:
		{
			if (!CHECK_MULTISHADOW())
				do_dec_stat(A_CON);
			dam = 0;
			break;
		}

		case GF_DEC_MAG:
		{
			if (!CHECK_MULTISHADOW())
				do_dec_stat(A_INT);
			dam = 0;
			break;
		}
		case GF_DEC_ALL:
		{

			if (!CHECK_MULTISHADOW())
			{
				do_dec_stat(A_STR);
				do_dec_stat(A_INT);
				do_dec_stat(A_WIS);
				do_dec_stat(A_DEX);
				do_dec_stat(A_CON);
				do_dec_stat(A_CHR);

			}
			dam = 0;
		}
		break;
		case GF_BERSERK:
		{
			if (!CHECK_MULTISHADOW()) set_shero(p_ptr->shero + 25 + randint1(25), FALSE);


			dam = 0;
		}
		break;

		case GF_PIT_FALL:
		{

			if (p_ptr->levitation)
			{
				msg_print("���Ȃ��͔��Ń_���[�W����������B");
				break;
			}
			else
			{
				msg_print("���ɗ������I");
				get_damage = take_hit(DAMAGE_ATTACK, dam, killer, monspell);
				set_stun(p_ptr->stun + 8 + randint1(8));

			}

		}

		break;



		/* Default */
		default:
		{
			/* No damage */
			dam = 0;

			break;
		}
	}
	//���̃_���S�C���t�����[�G���X
	if(p_ptr->pclass == CLASS_RINGO && get_damage)
	{
		p_ptr->magic_num1[2] = typ;
	}

	/* Hex - revenge damage stored */
	///race ��p�n�C
	revenge_store(get_damage);

	if ((p_ptr->tim_eyeeye || hex_spelling(HEX_EYE_FOR_EYE))
		&& (get_damage > 0) && !p_ptr->is_dead && (who > 0))
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
		else if (p_ptr->tim_eyeeye) set_tim_eyeeye(p_ptr->tim_eyeeye-5, TRUE);
	}

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_JINKIN)
		&& m_ptr->r_idx && get_damage > 0 && !p_ptr->is_dead && (who > 0))
	{
		msg_format("%s�͋Ղ̉��ɕ�܂ꂽ�E�E", m_name);
		project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
	}


	if (p_ptr->riding && dam > 0)
	{
		rakubadam_p = (dam > 200) ? 200 : dam;
	}


	/* Disturb */
	disturb(1, 1);


	//�Ȃ�����ȂƂ���ɕς��g���[�`��������̂��낤�H
	if ((p_ptr->special_defense & NINJA_KAWARIMI) && dam && who && (who != p_ptr->riding))
	{
		(void)kawarimi(FALSE);
	}


	/* Return "Anything seen?" */
	return (obvious);
}


/*
 * Find the distance from (x, y) to a line.
 */
/*:::������̋����H�u���X�͈̔͊֌W�Ŏg���炵��*/
int dist_to_line(int y, int x, int y1, int x1, int y2, int x2)
{
	/* Vector from (x, y) to (x1, y1) */
	int py = y1 - y;
	int px = x1 - x;

	/* Normal vector */
	int ny = x2 - x1;
	int nx = y1 - y2;

   /* Length of N */
	int pd = distance(y1, x1, y, x);
	int nd = distance(y1, x1, y2, x2);

	if (pd > nd) return distance(y, x, y2, x2);

	/* Component of P on N */
	nd = ((nd) ? ((py * ny + px * nx) / nd) : 0);

   /* Absolute value */
   return((nd >= 0) ? nd : 0 - nd);
}



/*
 * XXX XXX XXX
 * Modified version of los() for calculation of disintegration balls.
 * Disintegration effects are stopped by permanent walls.
 */
bool in_disintegration_range(int y1, int x1, int y2, int x2)
{
	/* Delta */
	int dx, dy;

	/* Absolute */
	int ax, ay;

	/* Signs */
	int sx, sy;

	/* Fractions */
	int qx, qy;

	/* Scanners */
	int tx, ty;

	/* Scale factors */
	int f1, f2;

	/* Slope, or 1/Slope, of LOS */
	int m;


	/* Extract the offset */
	dy = y2 - y1;
	dx = x2 - x1;

	/* Extract the absolute offset */
	ay = ABS(dy);
	ax = ABS(dx);


	/* Handle adjacent (or identical) grids */
	if ((ax < 2) && (ay < 2)) return (TRUE);


	/* Paranoia -- require "safe" origin */
	/* if (!in_bounds(y1, x1)) return (FALSE); */


	/* Directly South/North */
	if (!dx)
	{
		/* South -- check for walls */
		if (dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				if (cave_stop_disintegration(ty, x1)) return (FALSE);
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				if (cave_stop_disintegration(ty, x1)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}

	/* Directly East/West */
	if (!dy)
	{
		/* East -- check for walls */
		if (dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				if (cave_stop_disintegration(y1, tx)) return (FALSE);
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				if (cave_stop_disintegration(y1, tx)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if (ax == 1)
	{
		if (ay == 2)
		{
			if (!cave_stop_disintegration(y1 + sy, x1)) return (TRUE);
		}
	}

	/* Horizontal "knights" */
	else if (ay == 1)
	{
		if (ax == 2)
		{
			if (!cave_stop_disintegration(y1, x1 + sx)) return (TRUE);
		}
	}


	/* Calculate scale factor div 2 */
	f2 = (ax * ay);

	/* Calculate scale factor */
	f1 = f2 << 1;


	/* Travel horizontally */
	if (ax >= ay)
	{
		/* Let m = dy / dx * 2 * (dy * dx) = 2 * dy * dy */
		qy = ay * ay;
		m = qy << 1;

		tx = x1 + sx;

		/* Consider the special case where slope == 1. */
		if (qy == f2)
		{
			ty = y1 + sy;
			qy -= f1;
		}
		else
		{
			ty = y1;
		}

		/* Note (below) the case (qy == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (x2 - tx)
		{
			if (cave_stop_disintegration(ty, tx)) return (FALSE);

			qy += m;

			if (qy < f2)
			{
				tx += sx;
			}
			else if (qy > f2)
			{
				ty += sy;
				if (cave_stop_disintegration(ty, tx)) return (FALSE);
				qy -= f1;
				tx += sx;
			}
			else
			{
				ty += sy;
				qy -= f1;
				tx += sx;
			}
		}
	}

	/* Travel vertically */
	else
	{
		/* Let m = dx / dy * 2 * (dx * dy) = 2 * dx * dx */
		qx = ax * ax;
		m = qx << 1;

		ty = y1 + sy;

		if (qx == f2)
		{
			tx = x1 + sx;
			qx -= f1;
		}
		else
		{
			tx = x1;
		}

		/* Note (below) the case (qx == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (y2 - ty)
		{
			if (cave_stop_disintegration(ty, tx)) return (FALSE);

			qx += m;

			if (qx < f2)
			{
				ty += sy;
			}
			else if (qx > f2)
			{
				tx += sx;
				if (cave_stop_disintegration(ty, tx)) return (FALSE);
				qx -= f1;
				ty += sy;
			}
			else
			{
				tx += sx;
				qx -= f1;
				ty += sy;
			}
		}
	}

	/* Assume los */
	return (TRUE);
}


/*
 * breath shape
 */
/*:::�u���X�̌��ʂ��y�ԑS�O���b�h���v�Z �ڍז���*/
///mod140328 ��^���[�U�[�p�t���O�ǉ�
void breath_shape(u16b *path_g, int dist, int *pgrids, byte *gx, byte *gy, byte *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, int typ, bool masterspark)
{
	int by = y1;
	int bx = x1;
	int brad = 0;
	int brev = rad * rad / dist;
	int bdis = 0;
	int cdis;
	int path_n = 0;
	int mdis = distance(y1, x1, y2, x2) + rad;

	while (bdis <= mdis)
	{
		int x, y;

		if ((0 < dist) && (path_n < dist))
		{
			int ny = GRID_Y(path_g[path_n]);
			int nx = GRID_X(path_g[path_n]);
			int nd = distance(ny, nx, y1, x1);

			/* Get next base point */
			if (bdis >= nd)
			{
				by = ny;
				bx = nx;
				path_n++;
			}
		}

		/* Travel from center outward */
		for (cdis = 0; cdis <= brad; cdis++)
		{
			/* Scan the maximal blast area of radius "cdis" */
			for (y = by - cdis; y <= by + cdis; y++)
			{
				for (x = bx - cdis; x <= bx + cdis; x++)
				{
					/* Ignore "illegal" locations */
					if (!in_bounds(y, x)) continue;

					/* Enforce a circular "ripple" */
					if (distance(y1, x1, y, x) != bdis) continue;

					/* Enforce an arc */
					if (distance(by, bx, y, x) != cdis) continue;

					switch (typ)
					{
					case GF_LITE:
					case GF_LITE_WEAK:
						/* Lights are stopped by opaque terrains */
						if (!los(by, bx, y, x)) continue;
						break;
					case GF_DISINTEGRATE:
					case GF_BLAZINGSTAR:
						/* Disintegration are stopped only by perma-walls */
						if (!in_disintegration_range(by, bx, y, x)) continue;
						break;
					default:
						/* Ball explosions are stopped by walls */
						if (!projectable(by, bx, y, x)) continue;
						break;
					}

					/* Save this grid */
					gy[*pgrids] = y;
					gx[*pgrids] = x;
					(*pgrids)++;
				}
			}
		}

		/* Encode some more "radius" info */
		gm[bdis + 1] = *pgrids;

		/* Increase the size */
		if(masterspark)
		{	
			brad = (path_n+1) / 2 ;
			if(brad < 0 ) brad = 0;
			if(brad > rad) brad = rad;
		}
		else 
			brad = rad * (path_n + brev) / (dist + brev);

		/* Find the next ripple */
		bdis++;
	}

	/* Store the effect size */
	*pgm_rad = bdis;
}


/*
 * Generic "beam"/"bolt"/"ball" projection routine.
 *:::���ˍU���p��{���[�`��
 *
 * Input:
 *   who: Index of "source" monster (zero for "player")
 *:::who:�����Ȃǂ̎g�p�� ���Ȃ�0 �����X�^�[�Ȃ�m_list[]�̃C���f�b�N�X
 *:::    ���̂Ƃ���
 *   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 *:::rad: �{�[���̔��a�@�u���X�̂Ƃ����ŌĂ΂��炵��
 *   y,x: Target location (or location to travel "towards")
 *   dam: Base damage roll to apply to affected monsters (or player)
 *:::dam:�x�[�X�_���[�W
 *   typ: Type of damage to apply to monsters (and objects)
 *:::typ:����
 *   flg: Extra bit flags (see PROJECT_xxxx in "defines.h")
 *:::flg:�U���̎�ށA�e����^����͈͂Ȃǂ̃t���O
 *:::monspell:�H
 * Return:
 *   TRUE if any "effects" of the projection were observed, else FALSE
 *
 * Allows a monster (or player) to project a beam/bolt/ball of a given kind
 * towards a given location (optionally passing over the heads of interposing
 * monsters), and have it do a given amount of damage to the monsters (and
 * optionally objects) within the given radius of the final location.
 *:::�r�[���E�{���g�E�{�[�����w�肵���ꏊ�Ɍ����Ďw�肵�������E��ނŕ����A�_���[�W��^����
 *
 * A "bolt" travels from source to target and affects only the target grid.
 * A "beam" travels from source to target, affecting all grids passed through.
 * A "ball" travels from source to the target, exploding at the target, and
 *   affecting everything within the given radius of the target location.
 *
 * Traditionally, a "bolt" does not affect anything on the ground, and does
 * not pass over the heads of interposing monsters, much like a traditional
 * missile, and will "stop" abruptly at the "target" even if no monster is
 * positioned there, while a "ball", on the other hand, passes over the heads
 * of monsters between the source and target, and affects everything except
 * the source monster which lies within the final radius, while a "beam"
 * affects every monster between the source and target, except for the casting
 * monster (or player), and rarely affects things on the ground.
 *:::�{���g�͒n�ʂ̃A�C�e���ɉe����^�����^�[�Q�b�g�̊ԂɃ����X�^�[�������炻��ɓ�����B
 *:::�{�[���̓^�[�Q�b�g�̊ԂɃ����X�^�[�����Ă��ʉ߂��^�[�Q�b�g�Ŕ����A�g�p�҈ȊO�̃L�����ƒn�ʂ̃A�C�e���ɉe��
 *:::�r�[���͎g�p�҈ȊO�̃^�[�Q�b�g�܂ł̂��̂�S�Ċђʂ��邪�n�ʂ̂��̂ɂ́i�ő��Ɂj�e����^���Ȃ��B
 *
 * Two special flags allow us to use this function in special ways, the
 * "PROJECT_HIDE" flag allows us to perform "invisible" projections, while
 * the "PROJECT_JUMP" flag allows us to affect a specific grid, without
 * actually projecting from the source monster (or player).
 *:::PROJECT_HIDE���L���ɂȂ��Ă���Ƃ��G�t�F�N�g���\������Ȃ��H
 *:::PROJECT_JUMP���L���ɂȂ��Ă���Ƃ����ʂ��^�[�Q�b�g�ɓˑR�����H(�g�p�҂Ɋւ��鏈�������Ȃ��H�j
 *
 * The player will only get "experience" for monsters killed by himself
 * Unique monsters can only be destroyed by attacks from the player
 *:::���͒��ړ|���������X�^�[�̌o���l���������Ȃ��B���j�[�N�́��̍U���łȂ��Ǝ��ȂȂ�
 *
 * Only 256 grids can be affected per projection, limiting the effective
 * "radius" of standard ball attacks to nine units (diameter nineteen).
 *:::���̊֐��ɂ����ʂ̍ő�O���b�h����256�O���b�h
 *
 * One can project in a given "direction" by combining PROJECT_THRU with small
 * offsets to the initial location (see "line_spell()"), or by calculating
 * "virtual targets" far away from the player.
 *:::LOS�g���b�N�H
 *
 * One can also use PROJECT_THRU to send a beam/bolt along an angled path,
 * continuing until it actually hits somethings (useful for "stone to mud").
 *
 * Bolts and Beams explode INSIDE walls, so that they can destroy doors.
 *:::�{���g�ƃr�[���͕ǂɓ�����ƕǂ̒��Ŕ������鏈�������Ă���H
 *
 * Balls must explode BEFORE hitting walls, or they would affect monsters
 * on both sides of a wall.  Some bug reports indicate that this is still
 * happening in 2.7.8 for Windows, though it appears to be impossible.
 *:::�{�[���͕ǂ̎�O�Ŕ�������B�łȂ��ƕǂ̌������ɔ������͂��B�i�Â��o�[�W�����̃E�B���h�E�Y�ł͂܂��N����H�j
 *
 * We "pre-calculate" the blast area only in part for efficiency.
 * More importantly, this lets us do "explosions" from the "inside" out.
 * This results in a more logical distribution of "blast" treasure.
 * It also produces a better (in my opinion) animation of the explosion.
 * It could be (but is not) used to have the treasure dropped by monsters
 * in the middle of the explosion fall "outwards", and then be damaged by
 * the blast as it spreads outwards towards the treasure drop location.
 *:::�U�����͂��G���A�͎��O�Ɍv�Z�����B�����͓�����O�̏��ɏ��������B��������ƌ��h���������B
 *:::�{�[���U���Ń����X�^�[������ŃA�C�e���𗎂Ƃ��A�O���ɗ������A�C�e���������{�[���Ŕj�󂳂�邱�Ƃ��Ȃ��悤�ɏC�������H
 *
 * Walls and doors are included in the blast area, so that they can be
 * "burned" or "melted" in later versions.
 *:::�ǂ�h�A�͉e���͈͂Ɋ܂܂��B
 *
 * This algorithm is intended to maximize simplicity, not necessarily
 * efficiency, since this function is not a bottleneck in the code.
 *:::���̊֐��̓V���v�����D��ł��܂�����I�ł͂Ȃ����A�債���{�g���l�b�N�ɂ͂Ȃ�Ȃ��B
 *
 * We apply the blast effect from ground zero outwards, in several passes,
 * first affecting features, then objects, then monsters, then the player.
 * This allows walls to be removed before checking the object or monster
 * in the wall, and protects objects which are dropped by monsters killed
 * in the blast, and allows the player to see all affects before he is
 * killed or teleported away.  The semantics of this method are open to
 * various interpretations, but they seem to work well in practice.
 *:::�����̉e���́A������O�ցA����ɒn�`�E����A�C�e���E�����X�^�[�E���̏��ɏ�������B
 *:::�ǂ̗n���͂��̕ǂ̒��ɉ������Ă����̑O�ɍs����B
 *:::�����X�^�[������ŗ��Ƃ����A�C�e���͂��̍U������ی삳���B
 *:::�������ʂ��e���|�A�E�F�C�����O�Ɂ��͑S�Ă̌��ʂ�������B�i�ʂɕ��ʂ��Ǝv�����p��ǂ݊ԈႦ�Ă�H�j
 *:::�����̏������ǂ����߂��邩�͂��Ă�������ɂ͖��Ȃ��i�H�j
 *
 * We process the blast area from ground-zero outwards to allow for better
 * distribution of treasure dropped by monsters, and because it provides a
 * pleasing visual effect at low cost.
 *:::���S�n����O�֏������L�q����ق����L�q��r�W���A���ʂŌ��h���������E�E�H
 *
 * Note that the damage done by "ball" explosions decreases with distance.
 * This decrease is rapid, grids at radius "dist" take "1/dist" damage.
 *:::�����̃_���[�W�͋����ŋ}���Ɍ�������B1/�����ɂȂ�B
 *
 * Notice the "napalm" effect of "beam" weapons.  First they "project" to
 * the target, and then the damage "flows" along this beam of destruction.
 * The damage at every grid is the same as at the "center" of a "ball"
 * explosion, since the "beam" grids are treated as if they ARE at the
 * center of a "ball" explosion.
 *:::�r�[���͂܂��^�[�Q�b�g����������A���̌�^�[�Q�b�g�܂ł̊e�O���b�h�����������E�E�H
 *:::�r�[���̓͂��S�Ă͈̔͂̓{�[���̒��S�Ɠ������������Ȃ��_���[�W���K�p�����
 *
 * Currently, specifying "beam" plus "ball" means that locations which are
 * covered by the initial "beam", and also covered by the final "ball", except
 * for the final grid (the epicenter of the ball), will be "hit twice", once
 * by the initial beam, and once by the exploding ball.  For the grid right
 * next to the epicenter, this results in 150% damage being done.  The center
 * does not have this problem, for the same reason the final grid in a "beam"
 * plus "bolt" does not -- it is explicitly removed.  Simply removing "beam"
 * grids which are covered by the "ball" will NOT work, as then they will
 * receive LESS damage than they should.  Do not combine "beam" with "ball".
 *:::�悭�������B�r�[���E�{�[���E�{���g�̃t���O�𕡐��w�肷��Ɖ��x���_���[�W��^���邱�Ƃ�����̂ł�߂���Ă��Ƃ��H
 *
 * The array "gy[],gx[]" with current size "grids" is used to hold the
 * collected locations of all grids in the "blast area" plus "beam path".
 *:::gx[]gy[]�͉e�����󂯂�O���b�h���i�[�����
 *
 * Note the rather complex usage of the "gm[]" array.  First, gm[0] is always
 * zero.  Second, for N>1, gm[N] is always the index (in gy[],gx[]) of the
 * first blast grid (see above) with radius "N" from the blast center.  Note
 * that only the first gm[1] grids in the blast area thus take full damage.
 * Also, note that gm[rad+1] is always equal to "grids", which is the total
 * number of blast grids.
 *:::gm[]�͈З͂��Ƃ̃O���b�h������
 *:::gm[0]�͏��0�B
 *:::gm[1]�͔������S�������̓r�[���őS�_���[�W������O���b�h�̐��H
 *:::gm[N(2�ȏ�)]�ȍ~�́Agy[]gx[]�ɔ������S����̋������ɃO���b�h�񂪓����Ă钆�̋���N�̍ŏ��̃C���f�b�N�X�������Ă���H

 *
 * Note that once the projection is complete, (y2,x2) holds the final location
 * of bolts/beams, and the "epicenter" of balls.
 *:::�����I�����Ax2,y2�ɂ̓{�[���̒��S�ƃr�[���E�{���g�́i�Ō�́j�^�[�Q�b�g�������Ă���H
 *
 * Note also that "rad" specifies the "inclusive" radius of projection blast,
 * so that a "rad" of "one" actually covers 5 or 9 grids, depending on the
 * implementation of the "distance" function.  Also, a bolt can be properly
 * viewed as a "ball" with a "rad" of "zero".
 *:::���a1�̋��̌��ʔ͈͂�5�O���b�h�Ȃ̂�9�O���b�h�Ȃ̂��͋����Z�o���[�`���ɂ��
 *
 * Note that if no "target" is reached before the beam/bolt/ball travels the
 * maximum distance allowed (MAX_RANGE), no "blast" will be induced.  This
 * may be relevant even for bolts, since they have a "1x1" mini-blast.
 *:::���ɂ������炸�˒��͈͂𒴂����甚�����Ȃ�
 *
 * Note that for consistency, we "pretend" that the bolt actually takes "time"
 * to move from point A to point B, even if the player cannot see part of the
 * projection path.  Note that in general, the player will *always* see part
 * of the path, since it either starts at the player or ends on the player.
 *:::�{���g�́��Ɍ����ĂȂ��Ă����B�܂Ŏ��Ԃ��|����悤�Ɍ����鏈���ɂ����E�E�H
 *
 * Hack -- we assume that every "projection" is "self-illuminating".
 *:::�S�Ă̌��ʂ͈Â��Ă�������
 *
 * Hack -- when only a single monster is affected, we automatically track
 * (and recall) that monster, unless "PROJECT_JUMP" is used.
 *:::�����X�^�[��̂����ɓ��������ꍇ�APROJCET_JUMP���L���łȂ����肻�̃����X�^�[�̏���\������
 *
 * Note that all projections now "explode" at their final destination, even
 * if they were being projected at a more distant destination.  This means
 * that "ball" spells will *always* explode.
 *:::�S�Ẵ{�[���E�{���g�E�r�[���͓��B�n�_�Ŕ�������E�E�H�˒��͈͂Ƃ̌��ˍ����́H
 *
 * Note that we must call "handle_stuff()" after affecting terrain features
 * in the blast radius, in case the "illumination" of the grid was changed,
 * and "update_view()" and "update_monsters()" need to be called.
 *:::�����I�����ɂ�handle_stuff()���ĂԂ���

 *:::�˒�������MAX�ȊO�Ɏw�肵�����ꍇ�͂��̊֐����ĂԑO�ɃO���[�o���ϐ���project_length�ɓ���Ă����炵��
 
 */

bool project(int who, int rad, int y, int x, int dam, int typ, int flg, int monspell)
{
	int i, t, dist;

	int y1, x1;
	int y2, x2;
	int by, bx;

	int dist_hack = 0;

	int y_saver, x_saver; /* For reflecting monsters */

	int msec = delay_factor * delay_factor * delay_factor;

	/* Assume the player sees nothing */
	bool notice = FALSE;

	/* Assume the player has seen nothing */
	bool visual = FALSE;

	/* Assume the player has seen no blast grids */
	bool drawn = FALSE;

	/* Assume to be a normal ball spell */
	/*:::�u���X�U��(rad<0)�̂Ƃ�TRUE*/
	bool breath = FALSE;

	/* Is the player blind? */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	bool old_hide = FALSE;

	//v1.1.44 �폜
	//�z�s�̘Z�p�_�΂ƋM���̖�M�ɓ���������Ĕ������邽�߂̓���t���O�@
	//bool futo_broken_plate = FALSE;

	/* Number of grids in the "path" */
	int path_n = 0;

	/* Actual grids in the "path" */
	u16b path_g[1024];

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Coordinates of the affected grids */
	byte gx[2048], gy[2048];

	/* Encoded "radius" info (see above) */
	byte gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	bool jump = FALSE;

	/* Attacker's name (prepared before polymorph)*/
	char who_name[80];

	/* Can the player see the source of this effect? */
	bool see_s_msg = TRUE;

	int range;

	// -Hack- ���e�n��p�ϐ�
	//���̍U��(who=0)�͔��˓_�����̈ʒu�ɂȂ�悤�ݒ肳���B
	//���������e�n�̋Z�̍Ĕ��������͑O��̒��e�_���甭�˂���Ȃ��Ƃ����Ȃ��B
	//�Ȃ̂Ŗ��e�n�̋Z���Ĕ�������Ƃ��͂��̕ϐ��ɔ��˓_���i�[���Ă���who=-1��project()���ĂсA
	//project()�ċA�̒��ŉ��߂Ĕ��˓_�ɂ��̍��W���i�[����who��0�ɖ߂��B
	//��v1.1.44�ł�PROJECT_OPT�ł����g���ĂȂ��͂��B�������������
	static int mdn_tmp_x, mdn_tmp_y;

	/* Initialize by null string */
	who_name[0] = '\0';

	rakubadam_p = 0;
	rakubadam_m = 0;

	/* Default target of monsterspell is player */
	monster_target_y=py;
	monster_target_x=px;

	//v1.1.30 �o�O�E���o���̂��߂̌x�����b�Z�[�W
	//v1.1.31 ���Q�Ȗ�̓����Ȃǂ�typ=0�ŌĂ΂�邱�Ƃ�����悤�Ȃ̂Œʏ�\������Ȃ��悤�ɂ���
	if(cheat_xtra && !typ)
	{
		msg_print("WARNING:project()��typ�l���ݒ肳��Ă��Ȃ�");
	}

	///mod141214
	/*:::�ˌ��A�Ōn�̖��@��Z�ŁA�G�̔����⌌�̎􂢂Ł����|�ꂽ���Ȃǂɕs��ɂȂ邩������Ȃ��̂Œ��f��������Ƃ�*/
	if(who < 1 && (p_ptr->is_dead || p_ptr->paralyzed || p_ptr->chp < 0)) return FALSE;

	///��������ˌ� ���ˌn�A�{�[���A��^���[�U�[�A���e�̎ˎ�ȂǓ���e�łȂ��Ƃ���x�����Ĕ�������t���O��t�^����
	if(special_shooting_mode == SSM_SEIRAN3 && !rad && !(flg & (PROJECT_FAST | PROJECT_DISI | PROJECT_MASTERSPARK))) 
	{
		flg |= PROJECT_OPT;

	}

	
	//���̋��E���� GF�l���ς��@�󗓂���ꑮ��������̂Ōʏ���
	if(p_ptr->pclass == CLASS_YUKARI && !who && p_ptr->tim_general[1])
	{
		msg_print("�U���̑������ώ������I");

		if(typ == GF_FIRE)		typ = GF_PSY_SPEAR;
		//else if(typ == GF_PLASMA) typ = GF_WATER; v1.1.85 typ:13�𐁐ᐶ���Ŗ��߂�
		//else if(typ == GF_DARK_WEAK) typ = GF_SHARDS; v1.1.85 typ:19�𗋉_�����Ŗ��߂�
		//else if(typ == GF_INACT) typ = GF_MANA; v1.1.85 typ:25���_�̏������Ŗ��߂�
		//else if(typ == GF_ICE) typ = GF_CHAOS; v1.1.85 typ:29��ł̏������Ŗ��߂�
		//else if(typ == GF_GRAVITY) typ = GF_KILL_WALL; typ:36-39�𖄂߂�
		//else if(typ == GF_BERSERK) typ = GF_MAKE_DOOR; 45�܂Ŗ��܂���
		//else if(typ == GF_DIG_OIL) typ = GF_OLD_CLONE; //v1.1.92 MAKE_TREE��OLD_CLONE�̊Ԃ�DIG_OIL��ǉ����� v1.1.94 �����ЂƂ��߂�
		//else if(typ == GF_OLD_DRAIN) typ = GF_AWAY_UNDEAD; //v1.1.96 ���Ƃ��������ƃg���b�v���������Ŗ��߂�
		else if(typ == GF_HAND_DOOM) typ = GF_CONTROL_LIVING;//�����X�^�[�{�[���Ǝ��ҕ����͔�΂�
		else if(typ == GF_MAKE_FLOWER) typ = GF_MOSES;//GF_KOKORO�͂������p�̓��ꑮ���Ȃ̂Ŕ�΂�
		else if(typ == GF_STEAM) typ = GF_PURIFY;//GF_SEIRAN_BEAM���΂�
		else if (typ == GF_PURIFY) typ = GF_LUNATIC_TORCH;//v1.1.63 ����ˌ��p�������΂�
		else if (typ == GF_HOLY_WATER) typ = GF_ELEC;//v1.1.63�@�����͍ŏ��ɖ߂�@����I�ɏC�����悤 (MAX_GF���g���Ď����I�ɐݒ肷��Ɠ��ꑮ����ǉ������Ƃ��ɂ��������΂��Y��邩�������̂Ŏ蓮�ŕҏW����)
		else typ++;

		if (typ >= MAX_GF) typ = GF_ELEC;//�O�̂���
	}

	//v1.1.86 �A�r���e�B�J�[�h�u�͂�����Ԋ^�v����
	//���̌������{���g�����P�b�g�ɂȂ�З͕ω�
	if (!rad && !who && dam && (flg & PROJECT_STOP))
	{
		int card_num = count_ability_card(ABL_CARD_AKAGAERU);

		if (card_num)
		{
			int mult = 75;
			if (card_num > 1) mult += (card_num - 1) * 7;
			if (card_num >= 9) mult = 150;

			rad = 1;
			if (card_num >= 9) rad++;

			gm_rad = rad;

			dam = dam * mult / 100;
			if (!dam) dam = 1;

		}

	}


	/* Hack -- Jump to target */
	if (flg & (PROJECT_JUMP))
	{
		x1 = x;
		y1 = y;

		///mod151229 ���̈�s��ǉ����Ă����Ȃ���PROJECT_JUMP�œ|���ꂽ���_���v�ɏo�Ȃ��炵��
		if(who>0) monster_desc(who_name, &m_list[who], MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

		/* Clear the flag */
		flg &= ~(PROJECT_JUMP);

		jump = TRUE;
	}

	else if (flg & (/*PROJECT_MADAN | */ PROJECT_OPT))
	{
		if(!who) //�ŏ��Ɍ������Ƃ�
		{
			x1 = px;
			y1 = py;
		}
		else //���ˌ� who=-1��project()���Ă΂��
		{
			special_shooting_mode = 0L;//��������ˌ��t���Ooff
			who=0;
			flg &= ~(PROJECT_OPT);
			x1 = mdn_tmp_x;
			y1 = mdn_tmp_y;
		}
	}
	/* v1.1.44 ���[�`���������ɂ��폜
	else if (flg & (PROJECT_FUTO) )//�z�s���ꏈ��
	{
		if(!who) //�ŏ��Ɍ������Ƃ�
		{
			x1 = px;
			y1 = py;
		}
		else //project()������ċA��������Ƃ���who=-1��project()���Ă�
		{
			who=0;
			x1 = mdn_tmp_x;
			y1 = mdn_tmp_y;
		}
	}
	*/
	//v1.1.24 �X�^�[�g�ʒu���ʐݒ肷��
	else if(hack_project_start_x && hack_project_start_y)
	{
		x1 = hack_project_start_x;
		y1 = hack_project_start_y;
		hack_project_start_x = 0;
		hack_project_start_y = 0;

	}

	/* Start at player */
	else if (who <= 0)
	{
		x1 = px;
		y1 = py;
	}

	/* Start at monster */
	else if (who > 0)
	{
		x1 = m_list[who].fx;
		y1 = m_list[who].fy;
		monster_desc(who_name, &m_list[who], MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
	}

	/* Oops */
	else
	{
		x1 = x;
		y1 = y;
	}
	/*:::���˗p�H*/
	y_saver = y1;
	x_saver = x1;


	/* Default "destination" */
	y2 = y;
	x2 = x;

	///mod140907 ��^���[�U�[�n���g�p�҂Ɠ����ꏊ�Ɍ��ƃt���[�Y����̂ŉ���@�����̃{�[���ɂȂ�͂�
	if((flg & PROJECT_MASTERSPARK) && x1 == x2 && y1 == y2) flg &= ~(PROJECT_MASTERSPARK);

	/* Hack -- verify stuff */
	if (flg & (PROJECT_THRU))
	{
		if ((x1 == x2) && (y1 == y2))
		{
			flg &= ~(PROJECT_THRU);
		}
	}

	/* Handle a breath attack */
	if (rad < 0)
	{
		rad = 0 - rad;
		if(rad > 3) rad = 3;

		breath = TRUE;
		if (flg & PROJECT_HIDE) old_hide = TRUE;
		flg |= PROJECT_HIDE;
	}


	/* Hack -- Assume there will be no blast (max radius 32) */
	for (dist = 0; dist < 32; dist++) gm[dist] = 0;


	/* Initial grid */
	y = y1;
	x = x1;
	dist = 0;

	/* Collect beam grids */
	if (flg & (PROJECT_BEAM))
	{
		gy[grids] = y;
		gx[grids] = x;
		grids++;
	}
	/*:::�M���E�����̏���*/
	switch (typ)
	{
	case GF_LITE:
	case GF_LITE_WEAK:
		if (breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_LOS);
		break;
	case GF_DISINTEGRATE:
	case GF_BLAZINGSTAR:
		flg |= (PROJECT_GRID);
		if (breath || (flg & PROJECT_BEAM)) flg |= (PROJECT_DISI);
		break;
	case GF_SEIRAN_BEAM:
		if ((flg & PROJECT_BEAM)) flg |= (PROJECT_DISI);
		break;


	}

	range = project_length ? project_length : MAX_RANGE;

	if(flg & PROJECT_LONG_RANGE) range = 255;



	/* Calculate the projection path */
	/*:::�n�_����^�[�Q�b�g�i�������͏�Q���j�܂ł̌o�H���Z�o�B*/
	/*:::path_g[512]�̊e�v�f��x,y�����̍��W������B���8bit��y�A����8bit��x*/
	/*:::projcet_length�̓O���[�o���ł��̊֐��J�n����g���ĂȂ��B�˒���MAX���k�߂����Ƃ����O�Ɏw�肵�Ă����炵���B*/
	path_n = project_path(path_g, (range), y1, x1, y2, x2, flg);
	/*:::�ǂɗאڂ��ĕǂɌ����đ�^���[�U�[���ƃt���[�Y����̂ő΍�*/
	if((flg & PROJECT_MASTERSPARK) && path_n < 2) flg &= ~(PROJECT_MASTERSPARK);

	/* Hack -- Handle stuff */
	handle_stuff();

	///sysdel class ���g���̃V�[�J�[���C�ƃX�[�p�[���C����
	/* Giga-Hack SEEKER & SUPER_RAY */
	/*:::�V�[�J�[���C�ƃX�[�p�[���C�̔��˂Ɋւ��鏈���炵���B�悭�������̂Ŕ�΂�*/
	if( typ == GF_SEEKER )
	{
		int j;
		int last_i=0;

		/* Mega-Hack */
		/*:::static�̃O���[�o���ϐ�*/
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		for (i = 0; i < path_n; ++i)
		{
			int oy = y;
			int ox = x;

			int ny = GRID_Y(path_g[i]);
			int nx = GRID_X(path_g[i]);

			/* Advance */
			y = ny;
			x = nx;

			gy[grids] = y;
			gx[grids] = x;
			grids++;


			/* Only do visuals if requested */
			if (!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					/* Obtain the bolt pict */
					p = bolt_pict(oy, ox, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
					move_cursor_relative(y, x);
					/*if (fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					/*if (fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if (flg & (PROJECT_BEAM))
					{
						/* Obtain the explosion pict */
						p = bolt_pict(y, x, y, x, typ);

						/* Extract attr/char */
						a = PICT_A(p);
						c = PICT_C(p);

						/* Visual effects */
						print_rel(c, a, y, x);
					}

					/* Hack -- Activate delay */
					visual = TRUE;
				}

				/* Hack -- delay anyway for consistency */
				else if (visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_o(0,0,y,x,dam,GF_SEEKER))notice=TRUE;
			if( is_mirror_grid(&cave[y][x]))
			{
			  /* The target of monsterspell becomes tha mirror(broken) */
				monster_target_y=(s16b)y;
				monster_target_x=(s16b)x;

				remove_mirror(y,x);
				next_mirror( &oy,&ox,y,x );

				path_n = i+project_path(&(path_g[i+1]), (project_length ? project_length : MAX_RANGE), y, x, oy, ox, flg);
				for( j = last_i; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					if(project_m(0,0,y,x,dam,GF_SEEKER,flg,TRUE))notice=TRUE;
					if(!who && (project_m_n==1) && !jump ){
					  if(cave[project_m_y][project_m_x].m_idx >0 ){
					    monster_type *m_ptr = &m_list[cave[project_m_y][project_m_x].m_idx];

					    if (m_ptr->ml)
					    {
					      /* Hack -- auto-recall */
					      if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

					      /* Hack - auto-track */
					      health_track(cave[project_m_y][project_m_x].m_idx);
					    }
					  }
					}
					(void)project_f(0,0,y,x,dam,GF_SEEKER);
				}
				last_i = i;
			}
		}
		for( i = last_i ; i < path_n ; i++ )
		{
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			if(project_m(0,0,y,x,dam,GF_SEEKER,flg,TRUE))
			  notice=TRUE;
			if(!who && (project_m_n==1) && !jump ){
			  if(cave[project_m_y][project_m_x].m_idx >0 ){
			    monster_type *m_ptr = &m_list[cave[project_m_y][project_m_x].m_idx];

			    if (m_ptr->ml)
			    {
			      /* Hack -- auto-recall */
			      if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

			      /* Hack - auto-track */
			      health_track(cave[project_m_y][project_m_x].m_idx);
			    }
			  }
			}
			(void)project_f(0,0,y,x,dam,GF_SEEKER);
		}
		return notice;
	}
	else if(typ == GF_SUPER_RAY){
		int j;
		int second_step = 0;

		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		for (i = 0; i < path_n; ++i)
		{
			int oy = y;
			int ox = x;

			int ny = GRID_Y(path_g[i]);
			int nx = GRID_X(path_g[i]);

			/* Advance */
			y = ny;
			x = nx;

			gy[grids] = y;
			gx[grids] = x;
			grids++;


			/* Only do visuals if requested */
			if (!blind && !(flg & (PROJECT_HIDE)))
			{
				/* Only do visuals if the player can "see" the bolt */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					/* Obtain the bolt pict */
					p = bolt_pict(oy, ox, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
					move_cursor_relative(y, x);
					/*if (fresh_before)*/ Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					/*if (fresh_before)*/ Term_fresh();

					/* Display "beam" grids */
					if (flg & (PROJECT_BEAM))
					{
						/* Obtain the explosion pict */
						p = bolt_pict(y, x, y, x, typ);

						/* Extract attr/char */
						a = PICT_A(p);
						c = PICT_C(p);

						/* Visual effects */
						print_rel(c, a, y, x);
					}

					/* Hack -- Activate delay */
					visual = TRUE;
				}

				/* Hack -- delay anyway for consistency */
				else if (visual)
				{
					/* Delay for consistency */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			if(project_o(0,0,y,x,dam,GF_SUPER_RAY) )notice=TRUE;
			if (!cave_have_flag_bold(y, x, FF_PROJECT))
			{
				if( second_step )continue;
				break;
			}
			if( is_mirror_grid(&cave[y][x]) && !second_step )
			{
			  /* The target of monsterspell becomes tha mirror(broken) */
				monster_target_y=(s16b)y;
				monster_target_x=(s16b)x;

				remove_mirror(y,x);
				for( j = 0; j <=i ; j++ )
				{
					y = GRID_Y(path_g[j]);
					x = GRID_X(path_g[j]);
					(void)project_f(0,0,y,x,dam,GF_SUPER_RAY);
				}
				path_n = i;
				second_step =i+1;
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y-1, x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y  , x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y  , x+1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x-1, flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x  , flg);
				path_n += project_path(&(path_g[path_n+1]), (project_length ? project_length : MAX_RANGE), y, x, y+1, x+1, flg);
			}
		}
		for( i = 0; i < path_n ; i++ )
		{
			int x,y;
			y = GRID_Y(path_g[i]);
			x = GRID_X(path_g[i]);
			(void)project_m(0,0,y,x,dam,GF_SUPER_RAY,flg,TRUE);
			if(!who && (project_m_n==1) && !jump ){
			  if(cave[project_m_y][project_m_x].m_idx >0 ){
			    monster_type *m_ptr = &m_list[cave[project_m_y][project_m_x].m_idx];

			    if (m_ptr->ml)
			    {
			      /* Hack -- auto-recall */
			      if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

			      /* Hack - auto-track */
			      health_track(cave[project_m_y][project_m_x].m_idx);
			    }
			  }
			}
			(void)project_f(0,0,y,x,dam,GF_SUPER_RAY);
		}
		return notice;
	}
	/*:::�X�[�p�[���C�ƃV�[�J�[���C�̏����I��*/

	//�z�s���M�̏�ŘZ�p�_�΂Ƃ��g�����Ƃ��̂��߂̗�O����
	if(flg & (PROJECT_FUTO) && !path_n && is_plate_grid(&cave[y1][x1]))
	{
		//futo_break_plate(y1,x1);
		flag_futo_broken_plate = TRUE;
	}

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		int oy = y;
		int ox = x;

		/*:::path_g[]������W�����o��*/
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		if (flg & PROJECT_DISI)
		{
			/* Hack -- Balls explode before reaching walls */
			if (cave_stop_disintegration(ny, nx) && (rad > 0)) break;
		}
		//�ǂ��璵�˕Ԃ�ˌ����ǂ�ʂ蔲���Ȃ��悤�ɂ���
		else if(flg & PROJECT_OPT)
		{
			if (!cave_los_bold(ny, nx) ) break;
		}

		else if (flg & PROJECT_LOS)
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_los_bold(ny, nx) && (rad > 0)) break;
		}
		else
		{
			//v1.1.21 �u�C���^�[�Z�v�^�[�v�Z�\�����̕��m���G�̍U�����������Ƃ�����
			if(who > 0 && rad > 0 && !breath && !(flg & (PROJECT_JUMP | PROJECT_DISI | PROJECT_MASTERSPARK))  
				&& HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_INTERCEPTOR))
			{
				if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_RARM].sval != SV_FIRE_GUN_ROCKET
					|| inventory[INVEN_LARM].tval == TV_GUN && inventory[INVEN_LARM].sval != SV_FIRE_GUN_ROCKET)
				{
					if(projectable(ny,nx,py,px) && is_hostile(&m_list[who]) && weird_luck())
					{
						msg_print("���Ȃ��͍U�����������Ƃ����I");
						break;
					}
				}
			}


			/* Hack -- Balls explode before reaching walls */
			if (!cave_have_flag_bold(ny, nx, FF_PROJECT) && (rad > 0)) break;
			//�M�����͔��a0�����ǂ̑O�Ŕ���
			if (!cave_have_flag_bold(ny, nx, FF_PROJECT) && typ == GF_THROW_PLATE) break;
		}

		/* Advance */
		y = ny;
		x = nx;

		/* Collect beam grids */
		if (flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
			grids++;
		}

		/* Only do visuals if requested */
		/*:::�{���g�̋O�Օ`�揈���@�r�[���ł������ɓ��邪���Ƃ��珑�������Ă�*/
		if (!blind && !(flg & (PROJECT_HIDE | PROJECT_FAST)))
		{
			/* Only do visuals if the player can "see" the bolt */
			if (panel_contains(y, x) && player_has_los_bold(y, x))
			{
				u16b p;

				byte a;
				char c;

				/* Obtain the bolt pict */
				/*:::�{���g��\������Ƃ��̕����ƐF�����߂�*/
				p = bolt_pict(oy, ox, y, x, typ);

				/* Extract attr/char */
				a = PICT_A(p);
				c = PICT_C(p);

				/*:::Hack ����߈˂̋O�Ղ�'G'�ŕ\��*/
				if(typ == GF_POSSESSION) c = 'G';

				/* Visual effects */
				/*:::�w��ʒu�Ɉꕶ���`��*/
				print_rel(c, a, y, x);
				move_cursor_relative(y, x);
				/*if (fresh_before)*/ Term_fresh();
				/*:::��{�E�F�C�g�ʕ��̃f�B���C�����Ǝv����*/
				Term_xtra(TERM_XTRA_DELAY, msec);
				lite_spot(y, x);
				/*if (fresh_before)*/ Term_fresh();

				/* Display "beam" grids */
				/*:::�r�[���Ȃ炱���ɓ���A�`���'*'�ɏ��������Ă���*/
				if (flg & (PROJECT_BEAM))
				{
					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
				}

				/* Hack -- Activate delay */
				visual = TRUE;
			}

			/* Hack -- delay anyway for consistency */
			else if (visual)
			{
				/* Delay for consistency */
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		//�z�s���Z�@�M�̏ꏊ���邢�͏���̃^�[�Q�b�g�Ŏ~�܂�r�[��
		if(flg & (PROJECT_FUTO))
		{
			if(is_plate_grid(&cave[ny][nx]))
			{
				//futo_break_plate(ny,nx);
				flag_futo_broken_plate = TRUE;
				break;
			}

			if(ny == y2 && nx == x2) break;
		}

	}
	/*:::�����ŕǂɓ����������O���b�h�������炵�Ă���*/
	path_n = i;
	/* Save the "blast epicenter" */
	by = y;
	bx = x;

	//v1.1.44 ���e�_���O���[�o���ϐ��ɋL�^����
	hack_project_end_y = y;
	hack_project_end_x = x;

	if (breath && !path_n)
	{
		breath = FALSE;
		gm_rad = rad;
		if (!old_hide)
		{
			flg &= ~(PROJECT_HIDE);
		}
	}


	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	dist = path_n;
	dist_hack = dist;

	project_length = 0;

	/* If we found a "target", explode there */
	if (dist <= MAX_RANGE || flg & PROJECT_LONG_RANGE)
	{
		/* Mega-Hack -- remove the final "beam" grid */
		//if ((flg & (PROJECT_BEAM)) && !(flg & (PROJECT_FUTO)) && (grids > 0)) grids--;
		if ((flg & (PROJECT_BEAM)) && (grids > 0)) grids--;

		/*:::
		 *
		 * Create a columnar master spark
		 *
		 *    **************
		 *   ***************
		 * p********@*******
		 *   ***************
		 *    **************
		 */
		if(flg & (PROJECT_MASTERSPARK)) breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,TRUE);
		/*
		 * Create a conical breath attack
		 *
		 *         ***
		 *     ********
		 * D********@**
		 *     ********
		 *         ***
		 */
		else if (breath)
		{
			flg &= ~(PROJECT_HIDE);
			/*:::�u���X�̌��ʂ��y�ԑS�O���b�h���v�Z����*/
			breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,FALSE);
		}
		/*:::�{�[���̔����̉e�����y�ԑS�O���b�h���v�Z����*/
		else
		{
			/* Determine the blast area, work from the inside out */
			for (dist = 0; dist <= rad; dist++)
			{
				/* Scan the maximal blast area of radius "dist" */
				/*:::�����`�ɃX�L������dist����O�ꂽ��continue*/
				for (y = by - dist; y <= by + dist; y++)
				{
					for (x = bx - dist; x <= bx + dist; x++)
					{
						/* Ignore "illegal" locations */
						if (!in_bounds2(y, x)) continue;

						///mod150308 �e�ꕶ���R���v���b�T�[��p�@�㉺�ɂ����������Ȃ�
						if(flg & (PROJECT_COMPRESS) && x !=bx) continue;

						///mod150720 �z�s�̋M���̖�̓��ꏈ���B�M�ɓ����������������������A�����͎΂ߎl�����ɂ̂ݔ�ԁB
						//�Z�p�_�΂�rad��0�Ȃ̂ł����ɗ��Ȃ��B
						if(flg & (PROJECT_FUTO))
						{
							if(flag_futo_broken_plate)
							{
								if(((x-bx != y-by) && (x-bx != by-y) )) continue;
							}
							else
							{
								if(x != bx || y != by) continue;
							}
						}

						/* Enforce a "circular" explosion */
						if (distance(by, bx, y, x) != dist) continue;

						switch (typ)
						{
						case GF_LITE:
						case GF_LITE_WEAK:
							/* Lights are stopped by opaque terrains */
							if (!los(by, bx, y, x)) continue;
							break;
						case GF_DISINTEGRATE:
							/* Disintegration are stopped only by perma-walls */
							if (!in_disintegration_range(by, bx, y, x)) continue;
							break;
						default:
							/* Ball explosions are stopped by walls */
							if (!projectable(by, bx, y, x)) continue;
							break;
						}

						/* Save this grid */
						gy[grids] = y;
						gx[grids] = x;
						grids++;
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = grids;
			}
		}
	}
	/*:::�e���͈͂̎Z�o�I��*/

	/* Speed -- ignore "non-explosions" */
	/*:::�{�[����{���g���˒��͈͊O�܂Ŕ�񂾂Ƃ��H*/
	if (!grids) return (FALSE);

	/* Display the "blast area" if requested */
	if (!blind && !(flg & (PROJECT_HIDE)))
	{
		/* Then do the "blast", from inside out */
		for (t = 0; t <= gm_rad; t++)
		{
			/* Dump everything with this radius */
			for (i = gm[t]; i < gm[t+1]; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Only do visuals if the player can "see" the blast */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					drawn = TRUE;

					/* Obtain the explosion pict */
					/*:::�������`�敶����*�ɂȂ�*/
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					//v1.1.63 �������̐F��y���W�ɂ���ĕς��Ă݂�
					if (typ == GF_RAINBOW)
					{
						switch (y % 7)
						{
						case 0:a = TERM_VIOLET; break;
						case 1:a = TERM_BLUE; break;
						case 2:a = TERM_L_BLUE; break;
						case 3:a = TERM_L_GREEN; break;
						case 4:a = TERM_YELLOW; break;
						case 5:a = TERM_ORANGE; break;
						case 6:a = TERM_RED; break;

						}

					}

					/* Visual effects -- Display */
					print_rel(c, a, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush each "radius" seperately */
			/*if (fresh_before)*/ Term_fresh();

			/* Delay (efficiently) */
			if (visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		/*:::�`��v�Z���������灗�Ɍ����Ă镔�������`�悵�Ă���H*/
		/* Flush the erasing */
		if (drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Hack -- Erase if needed */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					lite_spot(y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush the explosion */
			/*if (fresh_before)*/ Term_fresh();
		}
	}


	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/*:::�����X�^�[�Ƀ_���[�W��^������ʂ̏ꍇ�A�����猩����Ȃ烁�b�Z�[�W���o���H*/
	/*:::�ǂ���who�����ŌĂ΂�邱�Ƃ�����炵���B�ǂ�Ȏ����H���K���X�����ꂽ���Ȃ�*/
	if (flg & PROJECT_KILL)
	{
		see_s_msg = (who > 0) ? is_seen(&m_list[who]) :
			(!who ? TRUE : (player_can_see_bold(y1, x1) && projectable(py, px, y1, x1)));
	}


	/* Check features */
	/*:::����Ɍ��ʂ��y�ڂ��ꍇ*/
	if (flg & (PROJECT_GRID))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			/*:::gm[]�ɋ������Ƃ�gx,gy[]�J�n�ʒu�������Ă邱�Ƃ𗘗p*/
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the grid */
				if (project_f(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the grid */
				if (project_f(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}

	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/* Check objects */
	/*:::����̃A�C�e���Ɍ��ʂ��y�ڂ��ꍇ*/
	if (flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the object in the grid */
				if (project_o(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the object in the grid */
				if (project_o(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}

	}


	/* Check monsters */
	/*:::�����X�^�[�Ɍ��ʂ��y�ڂ��ꍇ*/
	if (flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;
		/* Scan for monsters */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist + 1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			//��������ˌ��p�t���O�����@�{���g���G�ɓ�����Ɣ��˃t���O��������
			if(cave[y][x].m_idx && !(flg & PROJECT_BEAM) && (flg & PROJECT_OPT))
				flg &= ~(PROJECT_OPT);


			/* A single bolt may be reflected */
			///sys ���ˏ����@�{���g������e������������ŏ������悤
			if (grids <= 1)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				monster_race *ref_ptr = &r_info[m_ptr->r_idx];
				/*:::���ˉ\��x,y�Ƀ����X�^�[�����Ă��̃����X�^�[�����ˎ����Ł��̋R�惂���X�^�[�łȂ����ւ̍U���ł��Ȃ��Ƃ�9/10�Ŕ���*/
				if ((flg & PROJECT_REFLECTABLE) && cave[y][x].m_idx && (ref_ptr->flags2 & RF2_REFLECTING) &&
				    ((cave[y][x].m_idx != p_ptr->riding) || !(flg & PROJECT_PLAYER)) &&
				    (!who || dist_hack > 1) && !one_in_(10))
				{
					byte t_y, t_x;
					int max_attempts = 10;

					/* Choose 'new' target */
					/*:::���ː������*/
					do
					{
						t_y = y_saver - 1 + randint1(3);
						t_x = x_saver - 1 + randint1(3);
						max_attempts--;
					}
					while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(y, x, t_y, t_x));

					if (max_attempts < 1)
					{
						t_y = y_saver;
						t_x = x_saver;
					}

					if (is_seen(m_ptr))
					{
#ifdef JP
						if ((m_ptr->r_idx == MON_KENSHIROU) || (m_ptr->r_idx == MON_RAOU))
							msg_print("�u�k�l�_�����`�E��w�^��c�I�v");
						else if (m_ptr->r_idx == MON_DIO) msg_print("�f�B�I�E�u�����h�[�͎w��{�ōU����e���Ԃ����I");
						else msg_print("�U���͒��˕Ԃ����I");
#else
						msg_print("The attack bounces!");
#endif
					}
					if (is_original_ap_and_seen(m_ptr)) ref_ptr->r_flags2 |= RF2_REFLECTING;

					/* Reflected bolts randomly target either one */
					if (player_bold(y, x) || one_in_(2)) flg &= ~(PROJECT_PLAYER);
					else flg |= PROJECT_PLAYER;

					/* The bolt is reflected */
					/*:::�ċA�����Ŕ���*/
					project(cave[y][x].m_idx, 0, t_y, t_x, dam, typ, flg, monspell);

					/* Don't affect the monster any longer */
					continue;
				}

				else if ((flg & PROJECT_REFLECTABLE) && !(MON_CSLEEP(m_ptr)) &&
					((m_ptr->r_idx == MON_NINJA_SLAYER) || (m_ptr->r_idx == MON_AYA) || (m_ptr->r_idx == MON_HATATE) || (m_ptr->r_idx == MON_KARASU_TENGU) || (m_ptr->r_idx == MON_SEIJA)|| (m_ptr->r_idx == MON_SEIJA_D)  ))
				{
					if((m_ptr->r_idx == MON_AYA) && !one_in_(10)
					|| (m_ptr->r_idx == MON_HATATE) && !one_in_(10)
					|| (m_ptr->r_idx == MON_SEIJA) && !one_in_(7)
					|| (m_ptr->r_idx == MON_SEIJA_D) && !one_in_(7)
					|| (m_ptr->r_idx == MON_KARASU_TENGU) && !one_in_(5))
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format("%^s�͒e���B�e���ď������I", m_name);
						return (notice);
					}
					else if (m_ptr->r_idx == MON_NINJA_SLAYER)
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format("�u�C���[�b�I�v%^s�̓u���b�W�ł��������I", m_name);
						return (notice);

					}

	
				}


			}


			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}


			/* There is the riding player on this monster */
			if (p_ptr->riding && player_bold(y, x))
			{
				//v1.1.84 ���◢�T�͒ʏ�̋R��łȂ����ŗx���Ă���̂Ń{���g��������ɂ���
				bool flag_shield = (who && CLASS_RIDING_BACKDANCE && !one_in_(4));

				if (flag_shield && (flg & PROJECT_REFLECTABLE)) flg &= ~(PROJECT_PLAYER);

				/* Aimed on the player */
				/*:::����_�����U���̏ꍇ*/
				if (flg & PROJECT_PLAYER)
				{


					/*:::���������Ώۂ̂Ƃ��R�撆�̃����X�^�[�ɉe���͂Ȃ��i���ւ̏����͂��ƂŁj*/
					//
					if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
					{
						/*
						 * A beam or bolt is well aimed
						 * at the PLAYER!
						 * So don't affects the mount.
						 */
						continue;
					}
					/*:::�{�[����u���X�͋R�惂���X�^�[�ɂ��e����^���邪�З͂͌�������H*/
					else
					{
						/*
						 * The spell is not well aimed, 
						 * So partly affect the mount too.
						 */
						effective_dist++;
					}
				}

				/*
				 * This grid is the original target.
				 * Or aimed on your horse.
				 */
				/*:::PROJCET_PLAYER�łȂ��ꍇ�����X�^�[���_��ꂽ�U���ł���@���Ƃ͖�̔j��Ƃ��H*/
				else if (((y == y2) && (x == x2)) || (flg & PROJECT_AIMED))
				{
					/* Hit the mount with full damage */
				}

				/*
				 * Otherwise this grid is not the
				 * original target, it means that line
				 * of fire is obstructed by this
				 * monster.
				 */
				/*
				 * A beam or bolt will hit either
				 * player or mount.  Choose randomly.
				 */
				/*:::�r�[���́��ƃ����X�^�[�̂ǂ��炩�ɂ���������Ȃ�*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE))
				{
					if (one_in_(2) || flag_shield)
					{
						/* Hit the mount with full damage */
					}
					else
					{
						/* Hit the player later */
						flg |= PROJECT_PLAYER;

						/* Don't affect the mount */
						continue;
					}
				}

				/*
				 * The spell is not well aimed, so
				 * partly affect both player and
				 * mount.
				 */
				/*:::�{�[����u���X�����ꂽ�Ƃ���Ŕ��������ꍇ�H�З͂�������������C�����邪*/
				else
				{
					effective_dist++;
				}
			}

			/* Affect the monster in the grid */
			if (project_m(who, effective_dist, y, x, dam, typ, flg, see_s_msg)) notice = TRUE;
		}


		/* Player affected one monster (without "jumping") */
		/*:::���̍U���Ń����X�^�[��̂������_���[�W���󂯂��Ƃ��v���o�Ȃǂ��X�V*/
		/*:::project_m�̒��Ń����X�^�[���_���[�W���󂯂��Ƃ�projcet_m_n��++�����*/
		if (!who && (project_m_n == 1) && !jump)
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if (cave[y][x].m_idx > 0)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				if (m_ptr->ml)
				{
					/* Hack -- auto-recall */
					if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

					/* Hack - auto-track */
					if (m_ptr->ml) health_track(cave[y][x].m_idx);
				}
			}
		}

	}


	/* Check player */
	/*:::���ւ̏���*/
	if (flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the player? */
			if (!player_bold(y, x)) continue;

			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}

			/* Target may be your horse */
			if (p_ptr->riding)
			{
				/* Aimed on the player */
				if (flg & PROJECT_PLAYER)
				{
					/* Hit the player with full damage */
				}

				/*
				 * Hack -- When this grid was not the
				 * original target, a beam or bolt
				 * would hit either player or mount,
				 * and should be choosen randomly.
				 *
				 * But already choosen to hit the
				 * mount at this point.
				 *
				 * Or aimed on your horse.
				 */
				/*:::�r�[�����{���g�̓����X�^�[����O�ꂽ�Ƃ�����PROJECT_PLAYER�����Ȃ��̂ŁAPROJECT_PLAYER���Ȃ��Ȃ炷�łɋR�惂���X�^�[�ɓ������Ă���*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
				{
					/*
					 * A beam or bolt is well aimed
					 * at the mount!
					 * So don't affects the player.
					 */
					continue;
				}
				else
				{
					/*
					 * The spell is not well aimed, 
					 * So partly affect the player too.
					 */
					effective_dist++;
				}
			}
			/*:::�G�̍U������炷���ꏈ��*/
			if(who > 0 && (PLAYER_CAN_USE_ARTS))
			{
				//��������炷�d��
				if(CHECK_FAIRY_TYPE == 53 && flg & PROJECT_BEAM && typ == GF_LITE )
				{
					msg_print("���Ȃ��͌�������炵���I");
					break;
				}
				//�i���Ƃ̉􂵎�
				if((p_ptr->pclass == CLASS_MARTIAL_ARTIST) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)) )
				{
					int caster_lev = r_info[m_list[who].r_idx].level;
					int power = p_ptr->lev;
					if( r_info[m_list[who].r_idx].flags2 & RF2_POWERFUL) caster_lev = caster_lev * 3 / 2;

					if(p_ptr->csp > 100) power = power * p_ptr->csp / 100;
					if(p_ptr->tim_general[0]) power *= 2;
					if(heavy_armor()) power /= 5;

					if(randint1(caster_lev) < randint1(power))
					{
						int check = damroll(3,p_ptr->lev);
						if(check < 30)
							msg_print("���Ȃ��͙�l�ɔ�ёނ��čU������������I");
						else if(check < 80)
							msg_print("���Ȃ��͉􂵎󂯂ōU������������I");
						else if(check < 120)
							msg_print("���Ȃ��͍d�C���ōU������������I");
						else
							msg_print("���Ȃ��̓t�F�j�b�N�X�E�B���O�ōU������������I");

						break;
					}

				}
				//�j���ۏ��^�����
				if(p_ptr->mimic_form == MIMIC_SHINMYOU && one_in_(2) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)))
				{
					msg_print("���Ȃ��͊�A�ɉB��čU�������߂������I");
					break;
				}

				//v2.0.1 �A�r���e�B�J�[�h�u�C�܂܂Ȗ��Ғœ����v�{���g�m������
				if (flg & (PROJECT_STOP) && !rad)
				{
					int card_num = count_ability_card(ABL_CARD_JELLYFISH);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_JELLYFISH, card_num);
						if (randint1(100) <= prob)
						{
							msg_print("�N���Q���U����H�ׂ��I");
							break;
						}
					}
				}


				//v1.1.86 �A�r���e�B�J�[�h�u�w���̔��v�{���g�ƃr�[���m������(���̌�����)
				if (typ != GF_PSY_SPEAR && (flg & (PROJECT_BEAM | PROJECT_STOP)) && !rad)
				{
					int card_num = count_ability_card(ABL_CARD_BACKDOOR);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_BACKDOOR, card_num);
						if (randint1(100) <= prob)
						{
							msg_print("�U���͔w���̔��ɋz�����܂ꂽ�I");
							break;
						}
					}
				}
				//v1.1.86 �A�r���e�B�J�[�h�u�e���̖S��v�{�[���m������
				if (!(flg & (PROJECT_THRU | PROJECT_STOP | PROJECT_MASTERSPARK)) && rad>0 && !breath)
				{
					int card_num = count_ability_card(ABL_CARD_DANMAKUGHOST);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_DANMAKUGHOST, card_num);
						if (randint1(100) <= prob)
						{
							msg_print("�e���̖S�삪���@�������������I");
							break;
						}
					}
				}
				//v1.1.86 �A�r���e�B�J�[�h�u�d�ቹ�o�X�h�����v���P�b�g�m������
				//v1.1.88 �C�e�����������Ă��̂Œǉ�
				if (((flg & PROJECT_STOP) && rad && !breath))
				{
					int card_num = count_ability_card(ABL_CARD_BASS_DRUM);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_BASS_DRUM, card_num);
						if (randint1(100) <= prob)
						{
							msg_print("�d�ቹ�o�X�h�����������Ɣj�Ђ𑊎E�����I");
							break;
						}
					}
				}
				//v1.1.86 �A�r���e�B�J�[�h�u�T�C�R�L�l�V�X�v�u���X�m������
				if (breath)
				{
					int card_num = count_ability_card(ABL_CARD_PSYCHOKINESIS);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_PSYCHOKINESIS, card_num);
						if (randint1(100) <= prob)
						{
							msg_print("�O���͂̕ǂ��u���X��h�����I");
							break;
						}
					}
				}
				//v1.1.86 �A�r���e�B�J�[�h�u�T������UFO�v�@���̌��Ǝ􂢌n�U���ȊO�S�Ă��m���ŃJ�b�g
				if (typ != GF_PSY_SPEAR && !(flg & PROJECT_AIMED))
				{
					int card_num = count_ability_card(ABL_CARD_UFO);
					if (card_num)
					{
						int prob = calc_ability_card_prob(ABL_CARD_UFO, card_num);
						if (randint1(100) <= prob)
						{
							msg_print("�U�����T������UFO�ɎՂ�ꂽ�I");
							break;
						}
					}



				}



			}

			if(who > 0 && p_ptr->pclass == CLASS_ICHIRIN &&  unzan_guard())
			{
				break;
			}
			else if(check_activated_nameless_arts(JKF1_EXPLOSION_DEF) && rad > 0 && !breath)
			{
				msg_print("���Ȃ��͍U���ɑ΂������炩�̑ϐ����������B");
				effective_dist++;
			}
			else if(check_activated_nameless_arts(JKF1_BREATH_DEF) && breath)
			{
				msg_print("���Ȃ��͍U���ɑ΂������炩�̑ϐ����������B");
				effective_dist++;
			}


			/* Affect the player */
			if (project_p(who, who_name, effective_dist, y, x, dam, typ, flg, monspell)) notice = TRUE;
		}
	}

	//v1.1.44 ���e�n�_���烉���_���ȓG�Ɍ����Ĉ�x�����Ĕ���������Z�̏���
	if (flg & (PROJECT_OPT))
	{
		monster_type *m_ptr_tmp;
		int tx, ty, j;
		int cnt = 0;

		for (j = 1; j < m_max; j++)
		{
			m_ptr_tmp = &m_list[j];
			if (!m_ptr_tmp->r_idx) continue;
			if (is_pet(m_ptr_tmp)) continue;
			if (!projectable(by, bx, m_ptr_tmp->fy, m_ptr_tmp->fx)) continue;
			cnt++;
			if (!one_in_(cnt)) continue;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}

		if (cnt)
		{
			mdn_tmp_x = bx;
			mdn_tmp_y = by;
			project(-1, 0, ty, tx, dam, typ, flg, -1);
			//who��-1�ɂ��čċA�Ăяo������B���˒n�_�Ƃ���mdn_tmp_x/y���L�^����B
			//hack_project_start_x/y�Ƌ@�\�����̂ł����ꐮ�ڂ�����
		}
	}
	//���e�̎ˎ�p���ꏈ�� v1.1.44�ō폜
	/*
	if((flg & (PROJECT_OPT)) || hack_der_freischutz_basenum > 0 ||
		(flg & (PROJECT_MADAN)) && !one_in_((p_ptr->pclass == CLASS_REIMU) ? (osaisen_rank()) : 8))
	{
		monster_type *m_ptr_tmp;
		int tx,ty,j;
		int cnt=0;


		for (j = 1; j < m_max; j++)
		{
			m_ptr_tmp = &m_list[j];
			if (!m_ptr_tmp->r_idx) continue;
			if (is_pet(m_ptr_tmp)) continue;
		//	if(!projectable(y,x, m_ptr_tmp->fy,m_ptr_tmp->fx) && (y != m_ptr_tmp->fy || x != m_ptr_tmp->fx)) continue;
			if(!projectable(by,bx, m_ptr_tmp->fy,m_ptr_tmp->fx)) continue;
			cnt++;
			if(!one_in_(cnt)) continue;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}
		if(cheat_xtra) msg_format("���eCnt:%d",cnt);

		if(hack_der_freischutz_basenum > 0 && cnt) 
		{
			if(cheat_xtra) msg_format("basenum:%d",hack_der_freischutz_basenum);
			hack_der_freischutz_basenum -= MAX(1,(7 / cnt)); //�G�����Ȃ��Ƃ���������₷���悤�ɂ��Ƃ��B�˒��O���猂�Ă�̂ł��܂��̑���ɘA���œ���悤�ɂ���Ƌ�������B
			if(hack_der_freischutz_basenum<0) hack_der_freischutz_basenum=0;
		}

		if(cnt)
		{
			mdn_tmp_x = bx;
			mdn_tmp_y = by;
			if(oonusa_dam)
			{
				if(p_ptr->pclass == CLASS_UTSUHO) project(-1,3,ty,tx,oonusa_dam,GF_LITE,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_REIMU) project(-1,(osaisen_rank() / 4),ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_SUWAKO) project(-1,0,ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_YUKARI) project(-1,0,ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else msg_print("ERROR:oonusa_dam�֘A�̐ݒ肪�s��");
			}
			else if(flg & (PROJECT_MADAN)) project(-1,1,ty,tx,p_ptr->lev * 2,GF_SHARDS,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
			else if(flg & (PROJECT_OPT)) project(-1,0,ty,tx,dam,typ,flg,-1);
		}
	}
	*/
	//�Z�p�_�΂ƋM���̖�̍Ĕ�������
	//v1.1.44 �폜
	/*
	else if((flg & PROJECT_FUTO) && futo_broken_plate)
	{
		int next_x, next_y;
		mdn_tmp_x = bx;
		mdn_tmp_y = by;
		if(futo_determine_target(by,bx,&next_y,&next_x))
		{
			if(typ == GF_FIRE)
				return project(-1,0,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
			else
				return project(-1,5,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
		}
	}
	*/

	if(flg & PROJECT_MOVE) //�u���C�W���O�X�^�[
	{
		teleport_player_to(by,bx,TELEPORT_NONMAGICAL);
	}

	//v1.1.53 �N���E���s�[�X(VFS)�͉΂�����Ɗy�����Ȃ�
	//(�v���C���[�Ɍ�����ʒu�ŉΉ��A�j�M�̃{�[�����邢�̓u���X���y�􂷂�ƒZ���ԃq�[���[������)
	if (typ == GF_FIRE || typ == GF_NUKE )
	{
		if (rad && player_can_see_bold(hack_project_end_y, hack_project_end_x) )
		{
			clownpiece_likes_fire(typ);
		}

	}

	if (p_ptr->riding)
	{
		char m_name[80];

		monster_desc(m_name, &m_list[p_ptr->riding], 0);

		if (rakubadam_m > 0)
		{
			if (rakuba(rakubadam_m, FALSE))
			{
#ifdef JP
msg_format("%^s�ɐU�藎�Ƃ��ꂽ�I", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
			}
		}
		if (p_ptr->riding && rakubadam_p > 0)
		{
			if(rakuba(rakubadam_p, FALSE))
			{
#ifdef JP
msg_format("%^s���痎���Ă��܂����I", m_name);
#else
				msg_format("You have fallen from %s.", m_name);
#endif
			}
		}
	}



	/* Return "something was noticed" */
	return (notice);
}

bool binding_field( int dam )
{
	int mirror_x[10],mirror_y[10]; /* ���͂����Ə��Ȃ� */
	int mirror_num=0;              /* ���̐� */
	int x,y;
	int centersign;
	int x1,x2,y1,y2;
	u16b p;
	int msec= delay_factor*delay_factor*delay_factor;

	/* �O�p�`�̒��_ */
	int point_x[3];
	int point_y[3];

	/* Default target of monsterspell is player */
	monster_target_y=py;
	monster_target_x=px;

	for( x=0 ; x < cur_wid ; x++ )
	{
		for( y=0 ; y < cur_hgt ; y++ )
		{
			if( is_mirror_grid(&cave[y][x]) &&
			    distance(py,px,y,x) <= MAX_RANGE &&
			    distance(py,px,y,x) != 0 &&
			    player_has_los_bold(y,x) &&
			    projectable(py, px, y, x)
			    ){
				mirror_y[mirror_num]=y;
				mirror_x[mirror_num]=x;
				mirror_num++;
			}
		}
	}

	if( mirror_num < 2 )return FALSE;

	point_x[0] = randint0( mirror_num );
	do {
	  point_x[1] = randint0( mirror_num );
	}
	while( point_x[0] == point_x[1] );

	point_y[0]=mirror_y[point_x[0]];
	point_x[0]=mirror_x[point_x[0]];
	point_y[1]=mirror_y[point_x[1]];
	point_x[1]=mirror_x[point_x[1]];
	point_y[2]=py;
	point_x[2]=px;

	x=point_x[0]+point_x[1]+point_x[2];
	y=point_y[0]+point_y[1]+point_y[2];

	centersign = (point_x[0]*3-x)*(point_y[1]*3-y)
		- (point_y[0]*3-y)*(point_x[1]*3-x);
	if( centersign == 0 )return FALSE;
			    
	x1 = point_x[0] < point_x[1] ? point_x[0] : point_x[1];
	x1 = x1 < point_x[2] ? x1 : point_x[2];
	y1 = point_y[0] < point_y[1] ? point_y[0] : point_y[1];
	y1 = y1 < point_y[2] ? y1 : point_y[2];

	x2 = point_x[0] > point_x[1] ? point_x[0] : point_x[1];
	x2 = x2 > point_x[2] ? x2 : point_x[2];
	y2 = point_y[0] > point_y[1] ? point_y[0] : point_y[1];
	y2 = y2 > point_y[2] ? y2 : point_y[2];

	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					/* Visual effects */
					if(!(p_ptr->blind)
					   && panel_contains(y,x)){
					  p = bolt_pict(y,x,y,x, GF_MANA );
					  print_rel(PICT_C(p), PICT_A(p),y,x);
					  move_cursor_relative(y, x);
					  /*if (fresh_before)*/ Term_fresh();
					  Term_xtra(TERM_XTRA_DELAY, msec);
					}
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					(void)project_f(0,0,y,x,dam,GF_MANA); 
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					(void)project_o(0,0,y,x,dam,GF_MANA); 
				}
			}
		}
	}
	for( y=y1 ; y <=y2 ; y++ ){
		for( x=x1 ; x <=x2 ; x++ ){
			if( centersign*( (point_x[0]-x)*(point_y[1]-y)
					 -(point_y[0]-y)*(point_x[1]-x)) >=0 &&
			    centersign*( (point_x[1]-x)*(point_y[2]-y)
					 -(point_y[1]-y)*(point_x[2]-x)) >=0 &&
			    centersign*( (point_x[2]-x)*(point_y[0]-y)
					 -(point_y[2]-y)*(point_x[0]-x)) >=0 )
			{
				if (player_has_los_bold(y, x) && projectable(py, px, y, x)) {
					(void)project_m(0,0,y,x,dam,GF_MANA,
					  (PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP),TRUE);
				}
			}
		}
	}
	if( one_in_(7) ){
#ifdef JP
		msg_print("�������E�ɑς����ꂸ�A���Ă��܂����B");
#else
		msg_print("The field broke a mirror");
#endif	
		remove_mirror(point_y[0],point_x[0]);
	}

	return TRUE;
}

void seal_of_mirror( int dam )
{
	int x,y;

	for( x = 0 ; x < cur_wid ; x++ )
	{
		for( y = 0 ; y < cur_hgt ; y++ )
		{
			if( is_mirror_grid(&cave[y][x]))
			{
				if(project_m(0,0,y,x,dam,GF_GENOCIDE,
							 (PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP),TRUE))
				{
					if( !cave[y][x].m_idx )
					{
						remove_mirror(y,x);
					}
				}
			}
		}
	}
	return;
}


#if 0
/*:::�������u���C�W���O�X�^�[��p���[�`���@project()������*/
bool blazing_star(int who, int rad, int dir, int *y_end, int x_end, int dam)
{
	int i, t;

	int y1, x1;
	int y2, x2;
	int by, bx;

	int msec = delay_factor * delay_factor * delay_factor;
	int path_n = 0;

	/* Is the player blind? */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Actual grids in the "path" */
	u16b path_g[512];

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Coordinates of the affected grids */
	byte gx[1024], gy[1024];

	/* Encoded "radius" info (see above) */
	byte gm[32];

	/* Actual radius encoded in gm[] */
	int gm_rad = rad;

	bool jump = FALSE;

	if(dir == 5)
	{
		msg_print("ERROR:blazing_star()�ɓn����dir��5");
	}

	x1 = px;
	y1 = py;

	/* Default "destination" */
	y2 = py + ddy[dir] * 256;
	x2 = px + ddx[dir] * 256;

	i=0;

	//	path_n = project_path(path_g, (range), y1, x1, y2, x2, flg);
	while(1)
	{
		int tx,ty;
		tx = px + ddx[dir] * i;
		ty = py + ddy[dir] * i;
		if(!in_bounds(ty,tx)) break;
		if(cave_have_flag_bold(ty,tx,FF_PERMANENT)) break;
		path_g[path_n] = GRID(ty, tx);
		path_n++;

	}
	if(!path_n)
	{
		msg_print("������ɂ͌��ĂȂ��B");
		return FALSE;
	}

	/* Hack -- Handle stuff */
	handle_stuff();

#if 0
	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{

		int oy = py;
		int ox = px;

		/*:::path_g[]������W�����o��*/
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		y = ny;
		x = nx;

		/* Collect beam grids */
		if (flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
			grids++;
		}

		/* Only do visuals if requested */
		/*:::�{���g�̋O�Օ`�揈���@�r�[���ł������ɓ��邪���Ƃ��珑�������Ă�*/
		if (!blind && !(flg & (PROJECT_HIDE | PROJECT_FAST)))
		{
			/* Only do visuals if the player can "see" the bolt */
			if (panel_contains(y, x) && player_has_los_bold(y, x))
			{
				u16b p;

				byte a;
				char c;

				/* Obtain the bolt pict */
				/*:::�{���g��\������Ƃ��̕����ƐF�����߂�*/
				p = bolt_pict(oy, ox, y, x, typ);

				/* Extract attr/char */
				a = PICT_A(p);
				c = PICT_C(p);

				/*:::Hack ����߈˂̋O�Ղ�'G'�ŕ\��*/
				if(typ == GF_POSSESSION) c = 'G';

				/* Visual effects */
				/*:::�w��ʒu�Ɉꕶ���`��*/
				print_rel(c, a, y, x);
				move_cursor_relative(y, x);
				/*if (fresh_before)*/ Term_fresh();
				/*:::��{�E�F�C�g�ʕ��̃f�B���C�����Ǝv����*/
				Term_xtra(TERM_XTRA_DELAY, msec);
				lite_spot(y, x);
				/*if (fresh_before)*/ Term_fresh();

				/* Display "beam" grids */
				/*:::�r�[���Ȃ炱���ɓ���A�`���'*'�ɏ��������Ă���*/
				if (flg & (PROJECT_BEAM))
				{
					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
				}

				/* Hack -- Activate delay */
				visual = TRUE;
			}

			/* Hack -- delay anyway for consistency */
			else if (visual)
			{
				/* Delay for consistency */
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

	}
#endif

	/*:::�����ŕǂɓ����������O���b�h�������炵�Ă���*/
	//path_n = i;

	/* Save the "blast epicenter" */
	by = y;
	bx = x;


	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	dist = path_n;
	dist_hack = dist;

	project_length = 0;

	/* If we found a "target", explode there */
	if (dist <= MAX_RANGE)
	{
		/* Mega-Hack -- remove the final "beam" grid */
		//if ((flg & (PROJECT_BEAM)) && !(flg & (PROJECT_FUTO)) && (grids > 0)) grids--;
		if ((flg & (PROJECT_BEAM)) && (grids > 0)) grids--;

		/*:::
		 *
		 * Create a columnar master spark
		 *
		 *    **************
		 *   ***************
		 * p********@*******
		 *   ***************
		 *    **************
		 */
		if(flg & (PROJECT_MASTERSPARK)) breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,TRUE);
		/*
		 * Create a conical breath attack
		 *
		 *         ***
		 *     ********
		 * D********@**
		 *     ********
		 *         ***
		 */
		else if (breath)
		{
			flg &= ~(PROJECT_HIDE);
			/*:::�u���X�̌��ʂ��y�ԑS�O���b�h���v�Z����*/
			breath_shape(path_g, dist, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, by, bx, typ,FALSE);
		}
		/*:::�{�[���̔����̉e�����y�ԑS�O���b�h���v�Z����*/
		else
		{
			/* Determine the blast area, work from the inside out */
			for (dist = 0; dist <= rad; dist++)
			{
				/* Scan the maximal blast area of radius "dist" */
				/*:::�����`�ɃX�L������dist����O�ꂽ��continue*/
				for (y = by - dist; y <= by + dist; y++)
				{
					for (x = bx - dist; x <= bx + dist; x++)
					{
						/* Ignore "illegal" locations */
						if (!in_bounds2(y, x)) continue;

						///mod150308 �e�ꕶ���R���v���b�T�[��p�@�㉺�ɂ����������Ȃ�
						if(flg & (PROJECT_COMPRESS) && x !=bx) continue;
						///mod150720 �M���̖�͓G�ɓ��������Ƃ��͔��������A�M�ɓ������������΂ߕ����ɂ����������Ȃ�
						if(flg & (PROJECT_FUTO))
						{
							if(futo_broken_plate)
							{
								if(((x-bx != y-by) && (x-bx != by-y) )) continue;
							}
							else
							{
								if(x != bx || y != by) continue;
							}
						}

						/* Enforce a "circular" explosion */
						if (distance(by, bx, y, x) != dist) continue;

						switch (typ)
						{
						case GF_LITE:
						case GF_LITE_WEAK:
							/* Lights are stopped by opaque terrains */
							if (!los(by, bx, y, x)) continue;
							break;
						case GF_DISINTEGRATE:
							/* Disintegration are stopped only by perma-walls */
							if (!in_disintegration_range(by, bx, y, x)) continue;
							break;
						default:
							/* Ball explosions are stopped by walls */
							if (!projectable(by, bx, y, x)) continue;
							break;
						}

						/* Save this grid */
						gy[grids] = y;
						gx[grids] = x;
						grids++;
					}
				}

				/* Encode some more "radius" info */
				gm[dist+1] = grids;
			}
		}
	}
	/*:::�e���͈͂̎Z�o�I��*/

	/* Speed -- ignore "non-explosions" */
	/*:::�{�[����{���g���˒��͈͊O�܂Ŕ�񂾂Ƃ��H*/
	if (!grids) return (FALSE);


	/* Display the "blast area" if requested */
	if (!blind && !(flg & (PROJECT_HIDE)))
	{
		/* Then do the "blast", from inside out */
		for (t = 0; t <= gm_rad; t++)
		{
			/* Dump everything with this radius */
			for (i = gm[t]; i < gm[t+1]; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Only do visuals if the player can "see" the blast */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					drawn = TRUE;

					/* Obtain the explosion pict */
					/*:::�������`�敶����*�ɂȂ�*/
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects -- Display */
					print_rel(c, a, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush each "radius" seperately */
			/*if (fresh_before)*/ Term_fresh();

			/* Delay (efficiently) */
			if (visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}
		/*:::�`��v�Z���������灗�Ɍ����Ă镔�������`�悵�Ă���H*/
		/* Flush the erasing */
		if (drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Hack -- Erase if needed */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					lite_spot(y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(by, bx);

			/* Flush the explosion */
			/*if (fresh_before)*/ Term_fresh();
		}
	}


	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/*:::�����X�^�[�Ƀ_���[�W��^������ʂ̏ꍇ�A�����猩����Ȃ烁�b�Z�[�W���o���H*/
	/*:::�ǂ���who�����ŌĂ΂�邱�Ƃ�����炵���B�ǂ�Ȏ����H���K���X�����ꂽ���Ȃ�*/
	if (flg & PROJECT_KILL)
	{
		see_s_msg = (who > 0) ? is_seen(&m_list[who]) :
			(!who ? TRUE : (player_can_see_bold(y1, x1) && projectable(py, px, y1, x1)));
	}


	/* Check features */
	/*:::����Ɍ��ʂ��y�ڂ��ꍇ*/
	if (flg & (PROJECT_GRID))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			/*:::gm[]�ɋ������Ƃ�gx,gy[]�J�n�ʒu�������Ă邱�Ƃ𗘗p*/
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the grid */
				if (project_f(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the grid */
				if (project_f(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}
	}

	/* Update stuff if needed */
	if (p_ptr->update) update_stuff();

	/* Check objects */
	/*:::����̃A�C�e���Ɍ��ʂ��y�ڂ��ꍇ*/
	if (flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Find the closest point in the blast */
			if (breath)
			{
				int d = dist_to_line(y, x, y1, x1, by, bx);

				/* Affect the object in the grid */
				if (project_o(who, d, y, x, dam, typ)) notice = TRUE;
			}
			else
			{
				/* Affect the object in the grid */
				if (project_o(who, dist, y, x, dam, typ)) notice = TRUE;
			}
		}

	}


	/* Check monsters */
	/*:::�����X�^�[�Ɍ��ʂ��y�ڂ��ꍇ*/
	if (flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for monsters */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist + 1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* A single bolt may be reflected */
			///sys ���ˏ����@�{���g������e������������ŏ������悤
			if (grids <= 1)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];
				monster_race *ref_ptr = &r_info[m_ptr->r_idx];
				/*:::���ˉ\��x,y�Ƀ����X�^�[�����Ă��̃����X�^�[�����ˎ����Ł��̋R�惂���X�^�[�łȂ����ւ̍U���ł��Ȃ��Ƃ�9/10�Ŕ���*/
				if ((flg & PROJECT_REFLECTABLE) && cave[y][x].m_idx && (ref_ptr->flags2 & RF2_REFLECTING) &&
				    ((cave[y][x].m_idx != p_ptr->riding) || !(flg & PROJECT_PLAYER)) &&
				    (!who || dist_hack > 1) && !one_in_(10))
				{
					byte t_y, t_x;
					int max_attempts = 10;

					/* Choose 'new' target */
					/*:::���ː������*/
					do
					{
						t_y = y_saver - 1 + randint1(3);
						t_x = x_saver - 1 + randint1(3);
						max_attempts--;
					}
					while (max_attempts && in_bounds2u(t_y, t_x) && !projectable(y, x, t_y, t_x));

					if (max_attempts < 1)
					{
						t_y = y_saver;
						t_x = x_saver;
					}

					if (is_seen(m_ptr))
					{
#ifdef JP
						if ((m_ptr->r_idx == MON_KENSHIROU) || (m_ptr->r_idx == MON_RAOU))
							msg_print("�u�k�l�_�����`�E��w�^��c�I�v");
						else if (m_ptr->r_idx == MON_DIO) msg_print("�f�B�I�E�u�����h�[�͎w��{�ōU����e���Ԃ����I");
						else msg_print("�U���͒��˕Ԃ����I");
#else
						msg_print("The attack bounces!");
#endif
					}
					if (is_original_ap_and_seen(m_ptr)) ref_ptr->r_flags2 |= RF2_REFLECTING;

					/* Reflected bolts randomly target either one */
					if (player_bold(y, x) || one_in_(2)) flg &= ~(PROJECT_PLAYER);
					else flg |= PROJECT_PLAYER;

					/* The bolt is reflected */
					/*:::�ċA�����Ŕ���*/
					project(cave[y][x].m_idx, 0, t_y, t_x, dam, typ, flg, monspell);

					/* Don't affect the monster any longer */
					continue;
				}

				else if ((flg & PROJECT_REFLECTABLE) && !(MON_CSLEEP(m_ptr)) &&
					((m_ptr->r_idx == MON_NINJA_SLAYER) || (m_ptr->r_idx == MON_AYA) || (m_ptr->r_idx == MON_HATATE) || (m_ptr->r_idx == MON_KARASU_TENGU) || (m_ptr->r_idx == MON_SEIJA)|| (m_ptr->r_idx == MON_SEIJA_D)  ))
				{
					if((m_ptr->r_idx == MON_AYA) && !one_in_(10)
					|| (m_ptr->r_idx == MON_HATATE) && !one_in_(10)
					|| (m_ptr->r_idx == MON_SEIJA) && !one_in_(7)
					|| (m_ptr->r_idx == MON_SEIJA_D) && !one_in_(7)
					|| (m_ptr->r_idx == MON_KARASU_TENGU) && !one_in_(5))
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format("%^s�͒e���B�e���ď������I", m_name);
						return (notice);
					}
					else if (m_ptr->r_idx == MON_NINJA_SLAYER)
					{
						char m_name[80];
						monster_desc(m_name, m_ptr, 0);
						if (is_seen(m_ptr))msg_format("�u�C���[�b�I�v%^s�̓u���b�W�ł��������I", m_name);
						return (notice);

					}

	
				}


			}


			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}


			/* There is the riding player on this monster */
			if (p_ptr->riding && player_bold(y, x))
			{
				/* Aimed on the player */
				/*:::����_�����U���̏ꍇ*/
				if (flg & PROJECT_PLAYER)
				{
					/*:::���������Ώۂ̂Ƃ��R�撆�̃����X�^�[�ɉe���͂Ȃ��i���ւ̏����͂��ƂŁj*/
					if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
					{
						/*
						 * A beam or bolt is well aimed
						 * at the PLAYER!
						 * So don't affects the mount.
						 */
						continue;
					}
					/*:::�{�[����u���X�͋R�惂���X�^�[�ɂ��e����^���邪�З͂͌�������H*/
					else
					{
						/*
						 * The spell is not well aimed, 
						 * So partly affect the mount too.
						 */
						effective_dist++;
					}
				}

				/*
				 * This grid is the original target.
				 * Or aimed on your horse.
				 */
				/*:::PROJCET_PLAYER�łȂ��ꍇ�����X�^�[���_��ꂽ�U���ł���@���Ƃ͖�̔j��Ƃ��H*/
				else if (((y == y2) && (x == x2)) || (flg & PROJECT_AIMED))
				{
					/* Hit the mount with full damage */
				}

				/*
				 * Otherwise this grid is not the
				 * original target, it means that line
				 * of fire is obstructed by this
				 * monster.
				 */
				/*
				 * A beam or bolt will hit either
				 * player or mount.  Choose randomly.
				 */
				/*:::�r�[���́��ƃ����X�^�[�̂ǂ��炩�ɂ���������Ȃ�*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE))
				{
					if (one_in_(2))
					{
						/* Hit the mount with full damage */
					}
					else
					{
						/* Hit the player later */
						flg |= PROJECT_PLAYER;

						/* Don't affect the mount */
						continue;
					}
				}

				/*
				 * The spell is not well aimed, so
				 * partly affect both player and
				 * mount.
				 */
				/*:::�{�[����u���X�����ꂽ�Ƃ���Ŕ��������ꍇ�H�З͂�������������C�����邪*/
				else
				{
					effective_dist++;
				}
			}

			/* Affect the monster in the grid */
			if (project_m(who, effective_dist, y, x, dam, typ, flg, see_s_msg)) notice = TRUE;
		}


		/* Player affected one monster (without "jumping") */
		/*:::���̍U���Ń����X�^�[��̂������_���[�W���󂯂��Ƃ��v���o�Ȃǂ��X�V*/
		/*:::project_m�̒��Ń����X�^�[���_���[�W���󂯂��Ƃ�projcet_m_n��++�����*/
		if (!who && (project_m_n == 1) && !jump)
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if (cave[y][x].m_idx > 0)
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				if (m_ptr->ml)
				{
					/* Hack -- auto-recall */
					if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

					/* Hack - auto-track */
					if (m_ptr->ml) health_track(cave[y][x].m_idx);
				}
			}
		}

	}


	/* Check player */
	/*:::���ւ̏���*/
	if (flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			int effective_dist;

			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the player? */
			if (!player_bold(y, x)) continue;

			/* Find the closest point in the blast */
			if (breath || flg & PROJECT_MASTERSPARK)
			{
				effective_dist = dist_to_line(y, x, y1, x1, by, bx);
			}
			else
			{
				effective_dist = dist;
			}

			/* Target may be your horse */
			if (p_ptr->riding)
			{
				/* Aimed on the player */
				if (flg & PROJECT_PLAYER)
				{
					/* Hit the player with full damage */
				}

				/*
				 * Hack -- When this grid was not the
				 * original target, a beam or bolt
				 * would hit either player or mount,
				 * and should be choosen randomly.
				 *
				 * But already choosen to hit the
				 * mount at this point.
				 *
				 * Or aimed on your horse.
				 */
				/*:::�r�[�����{���g�̓����X�^�[����O�ꂽ�Ƃ�����PROJECT_PLAYER�����Ȃ��̂ŁAPROJECT_PLAYER���Ȃ��Ȃ炷�łɋR�惂���X�^�[�ɓ������Ă���*/
				else if (flg & (PROJECT_BEAM | PROJECT_REFLECTABLE | PROJECT_AIMED))
				{
					/*
					 * A beam or bolt is well aimed
					 * at the mount!
					 * So don't affects the player.
					 */
					continue;
				}
				else
				{
					/*
					 * The spell is not well aimed, 
					 * So partly affect the player too.
					 */
					effective_dist++;
				}
			}
			/*:::�G�̍U������炷���ꏈ��*/
			if(who > 0 && !p_ptr->blind && !p_ptr->paralyzed && !p_ptr->confused)
			{
				if(CHECK_FAIRY_TYPE == 53 && flg & PROJECT_BEAM && typ == GF_LITE )
				{
					msg_print("���Ȃ��͌�������炵���I");
					break;
				}
				//�i���Ƃ̉􂵎�
				if((p_ptr->pclass == CLASS_MARTIAL_ARTIST) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)) )
				{
					int caster_lev = r_info[m_list[who].r_idx].level;
					int power = p_ptr->lev;
					if( r_info[m_list[who].r_idx].flags2 & RF2_POWERFUL) caster_lev = caster_lev * 3 / 2;

					if(p_ptr->csp > 100) power = power * p_ptr->csp / 100;
					if(p_ptr->tim_general[0]) power *= 2;
					if(heavy_armor()) power /= 5;

					if(randint1(caster_lev) < randint1(power))
					{
						int check = damroll(3,p_ptr->lev);
						if(check < 30)
							msg_print("���Ȃ��͙�l�ɔ�ёނ��čU������������I");
						else if(check < 80)
							msg_print("���Ȃ��͉􂵎󂯂ōU������������I");
						else if(check < 120)
							msg_print("���Ȃ��͍d�C���ōU������������I");
						else
							msg_print("���Ȃ��̓t�F�j�b�N�X�E�B���O�ōU������������I");

						break;
					}

				}
				if(p_ptr->mimic_form == MIMIC_SHINMYOU && one_in_(2) && typ != GF_PSY_SPEAR && (rad || flg & (PROJECT_BEAM | PROJECT_STOP)))
				{
					msg_print("���Ȃ��͊�A�ɉB��čU�������߂������I");
					break;
				}
			}
			else if(who > 0 && p_ptr->pclass == CLASS_ICHIRIN &&  unzan_guard())
			{
				break;
			}


			/* Affect the player */
			if (project_p(who, who_name, effective_dist, y, x, dam, typ, flg, monspell)) notice = TRUE;
		}
	}
	//���e�̎ˎ�p���ꏈ��
	if((flg & (PROJECT_OPT)) || (flg & (PROJECT_MADAN)) && !one_in_((p_ptr->pclass == CLASS_REIMU) ? (osaisen_rank()) : 8))
	{
		monster_type *m_ptr_tmp;
		int tx,ty,j;
		int cnt=0;
		for (j = 1; j < m_max; j++)
		{
			m_ptr_tmp = &m_list[j];
			if (!m_ptr_tmp->r_idx) continue;
			if (is_pet(m_ptr_tmp)) continue;
		//	if(!projectable(y,x, m_ptr_tmp->fy,m_ptr_tmp->fx) && (y != m_ptr_tmp->fy || x != m_ptr_tmp->fx)) continue;
			if(!projectable(by,bx, m_ptr_tmp->fy,m_ptr_tmp->fx)) continue;
			cnt++;
			if(!one_in_(cnt)) continue;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}
		if(cheat_xtra) msg_format("���eCnt:%d",cnt);
		if(cnt)
		{
			mdn_tmp_x = bx;
			mdn_tmp_y = by;
			if(oonusa_dam)
			{
				if(p_ptr->pclass == CLASS_UTSUHO) project(-1,3,ty,tx,oonusa_dam,GF_LITE,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_REIMU) project(-1,(osaisen_rank() / 4),ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else if(p_ptr->pclass == CLASS_SUWAKO) project(-1,0,ty,tx,oonusa_dam,GF_ARROW,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
				else msg_print("ERROR:oonusa_dam�֘A�̐ݒ肪�s��");
			}
			else if(flg & (PROJECT_MADAN)) project(-1,1,ty,tx,p_ptr->lev * 2,GF_SHARDS,(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN),-1);
			else if(flg & (PROJECT_OPT)) project(-1,0,ty,tx,dam,typ,flg,-1);
		}
	}
	//�Z�p�_�΂ƋM���̖�̍Ĕ�������
	else if((flg & PROJECT_FUTO) && futo_broken_plate)
	{
		int next_x, next_y;
		mdn_tmp_x = bx;
		mdn_tmp_y = by;
		if(futo_determine_target(by,bx,&next_y,&next_x))
		{
			if(typ == GF_FIRE)
				return project(-1,0,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
			else
				return project(-1,5,next_y,next_x,dam,typ,( PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FUTO),-1);
		}
	}



	if (p_ptr->riding)
	{
		char m_name[80];

		monster_desc(m_name, &m_list[p_ptr->riding], 0);

		if (rakubadam_m > 0)
		{
			if (rakuba(rakubadam_m, FALSE))
			{
#ifdef JP
msg_format("%^s�ɐU�藎�Ƃ��ꂽ�I", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
			}
		}
		if (p_ptr->riding && rakubadam_p > 0)
		{
			if(rakuba(rakubadam_p, FALSE))
			{
#ifdef JP
msg_format("%^s���痎���Ă��܂����I", m_name);
#else
				msg_format("You have fallen from %s.", m_name);
#endif
			}
		}
	}

	/* Return "something was noticed" */
	return (notice);
}

     
#endif


