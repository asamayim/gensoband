/* File: cave.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: low level dungeon routines -BEN- */


#include "angband.h"


/*
 * Support for Adam Bolt's tileset, lighting and transparency effects
 * by Robert Ruehlmann (rr9@angband.org)
 */

static byte display_autopick;
static int match_autopick;
static object_type *autopick_obj;
static int feat_priority;

/*
 * Distance between two points via Newton-Raphson technique
 */
int distance (int y1, int x1, int y2, int x2)
{
	int dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
	int dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);

	/* Squared distance */
	int target = (dy * dy) + (dx * dx);

	/* Approximate distance: hypot(dy,dx) = max(dy,dx) + min(dy,dx) / 2 */
	int d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

	int err;

	/* Simple case */
	if (!dy || !dx) return d;

	while (1)
	{
		/* Approximate error */
		err = (target - d * d) / (2 * d);

		/* No error - we are done */
		if (!err) break;

		/* Adjust distance */
		d += err;
	}

	return d;
}


/*
 * Return TRUE if the given feature is a trap
 */
bool is_trap(int feat)
{
	return have_flag(f_info[feat].flags, FF_TRAP);
}


/*
 * Return TRUE if the given grid is a known trap
 */
bool is_known_trap(cave_type *c_ptr)
{
	if (!c_ptr->mimic && !cave_have_flag_grid(c_ptr, FF_SECRET) &&
	    is_trap(c_ptr->feat)) return TRUE;
	else
		return FALSE;
}


/*
 * Return TRUE if the given grid is a closed door
 */
bool is_closed_door(int feat)
{
	feature_type *f_ptr = &f_info[feat];

	return (have_flag(f_ptr->flags, FF_OPEN) || have_flag(f_ptr->flags, FF_BASH)) &&
	       !have_flag(f_ptr->flags, FF_MOVE);
}


/*
 * Return TRUE if the given grid is a hidden closed door
 */
/*:::�n�`���B���h�A���ǂ�������*/
bool is_hidden_door(cave_type *c_ptr)
{
	//v1.1.68  �V�n�`�u�X��v BASH�t���O�����邪�J�����Ȃ��̂ŋ���FALSE�ɂ���
	//v1.1.69 FF_OPEN�̂܂܂ɂ��ĂĉB���h�A��������Ȃ��Ȃ��Ă����̂ŏC��
	if (cave_have_flag_grid(c_ptr, FF_ICE_WALL))
		return FALSE;

	if ((c_ptr->mimic || cave_have_flag_grid(c_ptr, FF_SECRET)) &&
	    is_closed_door(c_ptr->feat))
		return TRUE;
	else
		return FALSE;
}


/*
 * A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
 * 4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu.
 *
 * Returns TRUE if a line of sight can be traced from (x1,y1) to (x2,y2).
 *
 * The LOS begins at the center of the tile (x1,y1) and ends at the center of
 * the tile (x2,y2).  If los() is to return TRUE, all of the tiles this line
 * passes through must be floor tiles, except for (x1,y1) and (x2,y2).
 *
 * We assume that the "mathematical corner" of a non-floor tile does not
 * block line of sight.
 *
 * Because this function uses (short) ints for all calculations, overflow may
 * occur if dx and dy exceed 90.
 *
 * Once all the degenerate cases are eliminated, the values "qx", "qy", and
 * "m" are multiplied by a scale factor "f1 = abs(dx * dy * 2)", so that
 * we can use integer arithmetic.
 *
 * We travel from start to finish along the longer axis, starting at the border
 * between the first and second tiles, where the y offset = .5 * slope, taking
 * into account the scale factor.  See below.
 *
 * Also note that this function and the "move towards target" code do NOT
 * share the same properties.  Thus, you can see someone, target them, and
 * then fire a bolt at them, but the bolt may hit a wall, not them.  However,
 * by clever choice of target locations, you can sometimes throw a "curve".
 *
 * Note that "line of sight" is not "reflexive" in all cases.
 *
 * Use the "projectable()" routine to test "spell/missile line of sight".
 *
 * Use the "update_view()" function to determine player line-of-sight.
 */
bool los(int y1, int x1, int y2, int x2)
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
	if ((ax < 2) && (ay < 2)) return TRUE;


	/* Paranoia -- require "safe" origin */
	/* if (!in_bounds(y1, x1)) return FALSE; */
	/* if (!in_bounds(y2, x2)) return FALSE; */


	/* Directly South/North */
	if (!dx)
	{
		/* South -- check for walls */
		if (dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				if (!cave_los_bold(ty, x1)) return FALSE;
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				if (!cave_los_bold(ty, x1)) return FALSE;
			}
		}

		/* Assume los */
		return TRUE;
	}

	/* Directly East/West */
	if (!dy)
	{
		/* East -- check for walls */
		if (dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				if (!cave_los_bold(y1, tx)) return FALSE;
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				if (!cave_los_bold(y1, tx)) return FALSE;
			}
		}

		/* Assume los */
		return TRUE;
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if (ax == 1)
	{
		if (ay == 2)
		{
			if (cave_los_bold(y1 + sy, x1)) return TRUE;
		}
	}

	/* Horizontal "knights" */
	else if (ay == 1)
	{
		if (ax == 2)
		{
			if (cave_los_bold(y1, x1 + sx)) return TRUE;
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
			if (!cave_los_bold(ty, tx)) return FALSE;

			qy += m;

			if (qy < f2)
			{
				tx += sx;
			}
			else if (qy > f2)
			{
				ty += sy;
				if (!cave_los_bold(ty, tx)) return FALSE;
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
			if (!cave_los_bold(ty, tx)) return FALSE;

			qx += m;

			if (qx < f2)
			{
				ty += sy;
			}
			else if (qx > f2)
			{
				tx += sx;
				if (!cave_los_bold(ty, tx)) return FALSE;
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
	return TRUE;
}






#define COMPLEX_WALL_ILLUMINATION

/*
 * Check for "local" illumination
 */
static bool check_local_illumination(int y, int x)
{
	/* Hack -- move towards player */
	/*:::x,y���灗�̂ق���1�}�X������ʒu*/
	int yy = (y < py) ? (y + 1) : (y > py) ? (y - 1) : y;
	int xx = (x < px) ? (x + 1) : (x > px) ? (x - 1) : x;

	/* Check for "local" illumination */

#ifdef COMPLEX_WALL_ILLUMINATION /* COMPLEX_WALL_ILLUMINATION */
	/*:::�����ɏƂ炳��Ă锻��H�@�����⃉�C�g�G���A��mimic�t�B�[���h��CAVE_GLOW�����̂��H*/

	/* Check for "complex" illumination */
	if ((feat_supports_los(get_feat_mimic(&cave[yy][xx])) &&
	     (cave[yy][xx].info & CAVE_GLOW)) ||
	    (feat_supports_los(get_feat_mimic(&cave[y][xx])) &&
	     (cave[y][xx].info & CAVE_GLOW)) ||
	    (feat_supports_los(get_feat_mimic(&cave[yy][x])) &&
	     (cave[yy][x].info & CAVE_GLOW)))
	{
		return TRUE;
	}
	else return FALSE;

#else /* COMPLEX_WALL_ILLUMINATION */

	/* Check for "simple" illumination */
	return (cave[yy][xx].info & CAVE_GLOW) ? TRUE : FALSE;

#endif /* COMPLEX_WALL_ILLUMINATION */
}


#define update_local_illumination_aux(Y, X) \
{ \
	if (player_has_los_bold((Y), (X))) \
	{ \
		/* Update the monster */ \
		if (cave[(Y)][(X)].m_idx) update_mon(cave[(Y)][(X)].m_idx, FALSE); \
\
		/* Notice and redraw */ \
		note_spot((Y), (X)); \
		lite_spot((Y), (X)); \
	} \
}


/*
 * Update "local" illumination
 */
void update_local_illumination(int y, int x)
{
	int i, yy, xx;

	if (!in_bounds(y, x)) return;

#ifdef COMPLEX_WALL_ILLUMINATION /* COMPLEX_WALL_ILLUMINATION */

	if ((y != py) && (x != px))
	{
		yy = (y < py) ? (y - 1) : (y + 1);
		xx = (x < px) ? (x - 1) : (x + 1);
		update_local_illumination_aux(yy, xx);
		update_local_illumination_aux(y, xx);
		update_local_illumination_aux(yy, x);
	}
	else if (x != px) /* y == py */
	{
		xx = (x < px) ? (x - 1) : (x + 1);
		for (i = -1; i <= 1; i++)
		{
			yy = y + i;
			update_local_illumination_aux(yy, xx);
		}
		yy = y - 1;
		update_local_illumination_aux(yy, x);
		yy = y + 1;
		update_local_illumination_aux(yy, x);
	}
	else if (y != py) /* x == px */
	{
		yy = (y < py) ? (y - 1) : (y + 1);
		for (i = -1; i <= 1; i++)
		{
			xx = x + i;
			update_local_illumination_aux(yy, xx);
		}
		xx = x - 1;
		update_local_illumination_aux(y, xx);
		xx = x + 1;
		update_local_illumination_aux(y, xx);
	}
	else /* Player's grid */
	{
		for (i = 0; i < 8; i++)
		{
			yy = y + ddy_cdd[i];
			xx = x + ddx_cdd[i];
			update_local_illumination_aux(yy, xx);
		}
	}

#else /* COMPLEX_WALL_ILLUMINATION */

	if ((y != py) && (x != px))
	{
		yy = (y < py) ? (y - 1) : (y + 1);
		xx = (x < px) ? (x - 1) : (x + 1);
		update_local_illumination_aux(yy, xx);
	}
	else if (x != px) /* y == py */
	{
		xx = (x < px) ? (x - 1) : (x + 1);
		for (i = -1; i <= 1; i++)
		{
			yy = y + i;
			update_local_illumination_aux(yy, xx);
		}
	}
	else if (y != py) /* x == px */
	{
		yy = (y < py) ? (y - 1) : (y + 1);
		for (i = -1; i <= 1; i++)
		{
			xx = x + i;
			update_local_illumination_aux(yy, xx);
		}
	}
	else /* Player's grid */
	{
		for (i = 0; i < 8; i++)
		{
			yy = y + ddy_cdd[i];
			xx = x + ddx_cdd[i];
			update_local_illumination_aux(yy, xx);
		}
	}

#endif /* COMPLEX_WALL_ILLUMINATION */
}


/*
 * Can the player "see" the given grid in detail?
 *
 * He must have vision, illumination, and line of sight.
 *
 *:::�������̃}�X�������邩�ǂ������肷��B
 *:::�ӖڂłȂ����̃}�X���Ƃ炳��Ă��莋�����ʂ��Ă���K�v������B
 *
 * Note -- "CAVE_LITE" is only set if the "torch" has "los()".
 * So, given "CAVE_LITE", we know that the grid is "fully visible".
 *:::CAVE_LITE�͎����̒ʂ����}�X�ɂ������Ȃ��B
 *
 * Note that "CAVE_GLOW" makes little sense for a wall, since it would mean
 * that a wall is visible from any direction.  That would be odd.  Except
 * under wizard light, which might make sense.  Thus, for walls, we require
 * not only that they be "CAVE_GLOW", but also, that they be adjacent to a
 * grid which is not only "CAVE_GLOW", but which is a non-wall, and which is
 * in line of sight of the player.
 *:::CAVE_GLOW�����ǂ͂��̂܂܏�������ƑS�������猩���ĕςȂ̂ŁA
 *:::CAVE_GLOW�������Ɨאڂ��ĂĂ��̏��Ɂ��̎������ʂ��Ă���Ƃ�������������悤�ɂ���H
 *
 * This extra check is expensive, but it provides a more "correct" semantics.
 *
 * Note that we should not run this check on walls which are "outer walls" of
 * the dungeon, or we will induce a memory fault, but actually verifying all
 * of the locations would be extremely expensive.
 *:::���̃`�F�b�N�͏����\�͂�H���B�_���W�����̍ŊO���̕ǂɂ��̃`�F�b�N�����ƃo�O��H
 *
 *
 * Thus, to speed up the function, we assume that all "perma-walls" which are
 * "CAVE_GLOW" are "illuminated" from all sides.  This is correct for all cases
 * except "vaults" and the "buildings" in town.  But the town is a hack anyway,
 * and the player has more important things on his mind when he is attacking a
 * monster vault.  It is annoying, but an extremely important optimization.
 *:::�o�O����ƌ������̂��߂ɁA�u�S�Ă�CAVE_GLOW�̉i�v�ǂ͑S��������Ƃ炳��Ă�v���̂Ƃ��Ĉ����B
 *vault�ƊX�ł͂��̏����͕s���R�����A�X�͕ʏ������邵vault�ł͂���Ȃ��ƋC�ɂ��Ă�ꍇ����Ȃ��B
 *
 *
 * Note that "glowing walls" are only considered to be "illuminated" if the
 * grid which is next to the wall in the direction of the player is also a
 * "glowing" grid.  This prevents the player from being able to "see" the
 * walls of illuminated rooms from a corridor outside the room.
 *:::CAVE_GLOW�̕ǂ͏Ƃ炳�ꂽ���ɗאڂ��ĂȂ��Ə�������Ȃ��̂ŁA�����̊O����ǂ�����������͂��Ȃ��B
 */
bool player_can_see_bold(int y, int x)
{
	cave_type *c_ptr;

	/* Blind players see nothing */
	if (p_ptr->blind) return FALSE;

	/* Access the cave grid */
	c_ptr = &cave[y][x];

	/* Note that "torch-lite" yields "illumination" */
	/*:::los����̑O�ɂ��������肳��Ă���Ƃ��������ƁA�����̃t���O�������������тɍX�V����Ă���̂��H*/
	if (c_ptr->info & (CAVE_LITE | CAVE_MNLT)) return TRUE;

	/* Require line of sight to the grid */
	/*:::�������ʂ��ĂȂ���false*/
	if (!player_has_los_bold(y, x)) return FALSE;

	/* Noctovision of Ninja */
	/*:::�j���W���͏�Ɍ�����@�z���S�̏������������H*/
	if (p_ptr->see_nocto) return TRUE;

	/* Require "perma-lite" of the grid */

	/*:::CAVE_GLOW��CAVE_MNDK�̃t���O�𒊏o���A���ꂪCAVE_GLOW�݂̂ƈ�v���Ȃ���FALSE��Ԃ��B
	 *:::CAVE_GLOW����ON�Ȃ炻�̂܂ܐ��
	 *:::MNDK����ON�Ȃ�FALSE��Ԃ��ďI��
	 *:::����ON�Ȃ�FALSE��Ԃ��ďI��
	 *:::����OFF�Ȃ�FALSE��Ԃ��ďI��
	 *:::�����Ƃ��ƌ����Ă鏰�A����mon���Â����Ă��Ȃ����łȂ��Ɓ��̌����⃉�C�g�G���A�𖳎����ĕs�������E�E�H
	 *:::Q008���ꂶ��ʏ�̏��Ō����������Ȃ��BCAVE_GLOW��CAVE_LITE�Ɋւ��Č�����Ă邩���̏�����ǂ݊ԈႦ�Ă���炵���B
	 *:::���ǂ������C�g�G���A����������ŏƂ炳�ꂽ����CAVE_GLOW�����ɂȂ��Ă���H�܂��ڍוs��*/
	if ((c_ptr->info & (CAVE_GLOW | CAVE_MNDK)) != CAVE_GLOW) return FALSE;

	/* Feature code (applying "mimic" field) */
	/* Floors are simple */
	/*:::cave_type.mimic�������mimic�̒l��f_info[].mimic�A�Ȃ����cave_type.feat��f_info[].mimic��ΏۂɁA*/
	/*:::f_info[mimic].flags��FF_LOS�������Ă邩����B�����Ă���TRUE*/
	/*:::Q009���̋@�\�̈Ӗ����悭����Ȃ�*/
	if (feat_supports_los(get_feat_mimic(c_ptr))) return TRUE;

	/* Check for "local" illumination */
	return check_local_illumination(y, x);
}



/*
 * Returns true if the player's grid is dark
 */
bool no_lite(void)
{
	return (!player_can_see_bold(py, px));
}




/*
 * Determine if a given location may be "destroyed"
 *
 * Used by destruction spells, and for placing stairs, etc.
 */
bool cave_valid_bold(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;


	/* Forbid perma-grids */
	if (cave_perma_grid(c_ptr)) return (FALSE);

	/* Check objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Forbid artifact grids */
		if (object_is_artifact(o_ptr)) return (FALSE);
	}

	/* Accept */
	return (TRUE);
}




/*
 * Hack -- Legal monster codes
 */
static char image_monster_hack[] = \
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/*
 * Hack -- Legal object codes
 */
static char image_object_hack[] = "?/|\\\"!$()_-=[]{},~";

/*
 * Mega-Hack -- Hallucinatory monster
 */
static void image_monster(byte *ap, char *cp)
{
	/* Random symbol from set above */
	if (use_graphics)
	{
		monster_race *r_ptr = &r_info[randint1(max_r_idx - 1)];

		*cp = r_ptr->x_char;
		*ap = r_ptr->x_attr;
	}
	else
	/* Text mode */
	{
		*cp = (one_in_(25) ?
		       image_object_hack[randint0(sizeof(image_object_hack) - 1)] :
		       image_monster_hack[randint0(sizeof(image_monster_hack) - 1)]);

		/* Random color */
		*ap = randint1(15);
	}
}


/*
 * Mega-Hack -- Hallucinatory object
 */
static void image_object(byte *ap, char *cp)
{
	if (use_graphics)
	{
		object_kind *k_ptr = &k_info[randint1(max_k_idx-1)];

		*cp = k_ptr->x_char;
		*ap = k_ptr->x_attr;
	}
	else
	{
		int n = sizeof(image_object_hack) - 1;

		*cp = image_object_hack[randint0(n)];

		/* Random color */
		*ap = randint1(15);
	}
}



/*
 * Hack -- Random hallucination
 */
static void image_random(byte *ap, char *cp)
{
	/* Normally, assume monsters */
	if (randint0(100) < 75)
	{
		image_monster(ap, cp);
	}

	/* Otherwise, assume objects */
	else
	{
		image_object(ap, cp);
	}
}

/*
 * This array lists the effects of "brightness" on various "base" colours.
 *
 * This is used to do dynamic lighting effects in ascii :-)
 * At the moment, only the various "floor" tiles are affected.
 *
 * The layout of the array is [x][0] = light and [x][1] = dark.
 */

static byte lighting_colours[16][2] =
{
	/* TERM_DARK */
	{TERM_L_DARK, TERM_DARK},

	/* TERM_WHITE */
	{TERM_YELLOW, TERM_SLATE},

	/* TERM_SLATE */
	{TERM_WHITE, TERM_L_DARK},

	/* TERM_ORANGE */
	{TERM_L_UMBER, TERM_UMBER},

	/* TERM_RED */
	{TERM_RED, TERM_RED},

	/* TERM_GREEN */
	{TERM_L_GREEN, TERM_GREEN},

	/* TERM_BLUE */
	{TERM_BLUE, TERM_BLUE},

	/* TERM_UMBER */
	{TERM_L_UMBER, TERM_RED},

	/* TERM_L_DARK */
	{TERM_SLATE, TERM_L_DARK},

	/* TERM_L_WHITE */
	{TERM_WHITE, TERM_SLATE},

	/* TERM_VIOLET */
	{TERM_L_RED, TERM_BLUE},

	/* TERM_YELLOW */
	{TERM_YELLOW, TERM_ORANGE},

	/* TERM_L_RED */
	{TERM_L_RED, TERM_L_RED},

	/* TERM_L_GREEN */
	{TERM_L_GREEN, TERM_GREEN},

	/* TERM_L_BLUE */
	{TERM_L_BLUE, TERM_L_BLUE},

	/* TERM_L_UMBER */
	{TERM_L_UMBER, TERM_UMBER}
};


/*
 * Apply "default" feature lighting effects
 */
void apply_default_feat_lighting(byte f_attr[F_LIT_MAX], byte f_char[F_LIT_MAX])
{
	byte s_attr = f_attr[F_LIT_STANDARD];
	byte s_char = f_char[F_LIT_STANDARD];
	int i;

	if (is_ascii_graphics(s_attr)) /* For ASCII */
	{
		f_attr[F_LIT_LITE] = lighting_colours[s_attr & 0x0f][0];
		f_attr[F_LIT_DARK] = lighting_colours[s_attr & 0x0f][1];
		for (i = F_LIT_NS_BEGIN; i < F_LIT_MAX; i++) f_char[i] = s_char;
	}
	else /* For tile graphics */
	{
		for (i = F_LIT_NS_BEGIN; i < F_LIT_MAX; i++) f_attr[i] = s_attr;
		f_char[F_LIT_LITE] = s_char + 2;
		f_char[F_LIT_DARK] = s_char + 1;
	}
}


/* Is this grid "darkened" by monster? */
#define darkened_grid(C) \
	((((C)->info & (CAVE_VIEW | CAVE_LITE | CAVE_MNLT | CAVE_MNDK)) == (CAVE_VIEW | CAVE_MNDK)) && \
	!p_ptr->see_nocto)


/*
 * Extract the attr/char to display at the given (legal) map location
 *:::�}�b�v��}�X��}�X�̕��������肷��B
 *
 * Basically, we "paint" the chosen attr/char in several passes, starting
 * with any known "terrain features" (defaulting to darkness), then adding
 * any known "objects", and finally, adding any known "monsters".  This
 * is not the fastest method but since most of the calls to this function
 * are made for grids with no monsters or objects, it is fast enough.
 *:::�n�`�E�A�C�e���E�����X�^�[�̏��ɏ����B���������������������ʂ̒n�`�Ȃ̂Ŗ��Ȃ�
 *
 * Note that this function, if used on the grid containing the "player",
 * will return the attr/char of the grid underneath the player, and not
 * the actual player attr/char itself, allowing a lot of optimization
 * in various "display" functions.
 *::::���̏����͂����ł͍s���Ȃ��H
 *
 * Note that the "zero" entry in the feature/object/monster arrays are
 * used to provide "special" attr/char codes, with "monster zero" being
 * used for the player attr/char, "object zero" being used for the "stack"
 * attr/char, and "feature zero" being used for the "nothing" attr/char,
 * though this function makes use of only "feature zero".
 *:::�����X�^�[�A�A�C�e���A�n�`�̋󎚂͓��ʂȖ����E�E�H
 *
 * Note that monsters can have some "special" flags, including "ATTR_MULTI",
 * which means their color changes, and "ATTR_CLEAR", which means they take
 * the color of whatever is under them, and "CHAR_CLEAR", which means that
 * they take the symbol of whatever is under them.  Technically, the flag
 * "CHAR_MULTI" is supposed to indicate that a monster looks strange when
 * examined, but this flag is currently ignored.
 *:::���F�A���������X�^�[�̏����������ł��邪�}���`�V���{���̏����͂��Ȃ��E�E�H
 *
 * Currently, we do nothing with multi-hued objects, because there are
 * not any.  If there were, they would have to set "shimmer_objects"
 * when they were created, and then new "shimmer" code in "dungeon.c"
 * would have to be created handle the "shimmer" effect, and the code
 * in "cave.c" would have to be updated to create the shimmer effect.
 *:::���F�A�C�e���͑��݂��Ȃ��̂ŏ������Ȃ��B���Ȃ�dungeon.c��M��K�v������B
 *
 * Note the effects of hallucination.  Objects always appear as random
 * "objects", monsters as random "monsters", and normal grids occasionally
 * appear as random "monsters" or "objects", but note that these random
 * "monsters" and "objects" are really just "colored ascii symbols".
 *:::���o����������
 *
 * Note that "floors" and "invisible traps" (and "zero" features) are
 * drawn as "floors" using a special check for optimization purposes,
 * and these are the only features which get drawn using the special
 * lighting effects activated by "view_special_lite".
 *:::�������g���b�v�͏��Ƃ��ĕ`��Bspecial_lite�̒n�`�͌���E�E�H
 *
 * Note the use of the "mimic" field in the "terrain feature" processing,
 * which allows any feature to "pretend" to be another feature.  This is
 * used to "hide" secret doors, and to make all "doors" appear the same,
 * and all "walls" appear the same, and "hidden" treasure stay hidden.
 * It is possible to use this field to make a feature "look" like a floor,
 * but the "special lighting effects" for floors will not be used.
 *:::mimic�t�B�[���h������ƕʂ̒n�`�Ƃ��ĕ`�悳���B�B���h�A�Ƃ�
 *
 * Note the use of the new "terrain feature" information.  Note that the
 * assumption that all interesting "objects" and "terrain features" are
 * memorized allows extremely optimized processing below.  Note the use
 * of separate flags on objects to mark them as memorized allows a grid
 * to have memorized "terrain" without granting knowledge of any object
 * which may appear in that grid.
 *:::�A�C�e���Ɠ���n�`�͋L�������
 *
 * Note the efficient code used to determine if a "floor" grid is
 * "memorized" or "viewable" by the player, where the test for the
 * grid being "viewable" is based on the facts that (1) the grid
 * must be "lit" (torch-lit or perma-lit), (2) the grid must be in
 * line of sight, and (3) the player must not be blind, and uses the
 * assumption that all torch-lit grids are in line of sight.
 *:::�O���b�h�������邽�߂ɂ́A�Ƃ炳��Ă��莋�����ʂ��Ă��聗���ӖڂłȂ���
 *
 * Note that floors (and invisible traps) are the only grids which are
 * not memorized when seen, so only these grids need to check to see if
 * the grid is "viewable" to the player (if it is not memorized).  Since
 * most non-memorized grids are in fact walls, this induces *massive*
 * efficiency, at the cost of *forcing* the memorization of non-floor
 * grids when they are first seen.  Note that "invisible traps" are
 * always treated exactly like "floors", which prevents "cheating".
 *:::�����̃t���A�͌��Ă��L������Ȃ�
 *
 * Note the "special lighting effects" which can be activated for floor
 * grids using the "view_special_lite" option (for "white" floor grids),
 * causing certain grids to be displayed using special colors.  If the
 * player is "blind", we will use "dark gray", else if the grid is lit
 * by the torch, and the "view_yellow_lite" option is set, we will use
 * "yellow", else if the grid is "dark", we will use "dark gray", else
 * if the grid is not "viewable", and the "view_bright_lite" option is
 * set, and the we will use "slate" (gray).  We will use "white" for all
 * other cases, in particular, for illuminated viewable floor grids.
 *:::�悭����񂪖Ӗڎ��G�ꂽ�ǂ̋L�������H
 *
 * Note the "special lighting effects" which can be activated for wall
 * grids using the "view_granite_lite" option (for "white" wall grids),
 * causing certain grids to be displayed using special colors.  If the
 * player is "blind", we will use "dark gray", else if the grid is lit
 * by the torch, and the "view_yellow_lite" option is set, we will use
 * "yellow", else if the "view_bright_lite" option is set, and the grid
 * is not "viewable", or is "dark", or is glowing, but not when viewed
 * from the player's current location, we will use "slate" (gray).  We
 * will use "white" for all other cases, in particular, for correctly
 * illuminated viewable wall grids.
 *
 * Note that, when "view_granite_lite" is set, we use an inline version
 * of the "player_can_see_bold()" function to check the "viewability" of
 * grids when the "view_bright_lite" option is set, and we do NOT use
 * any special colors for "dark" wall grids, since this would allow the
 * player to notice the walls of illuminated rooms from a hallway that
 * happened to run beside the room.  The alternative, by the way, would
 * be to prevent the generation of hallways next to rooms, but this
 * would still allow problems when digging towards a room.
 *
 * Note that bizarre things must be done when the "attr" and/or "char"
 * codes have the "high-bit" set, since these values are used to encode
 * various "special" pictures in some versions, and certain situations,
 * such as "multi-hued" or "clear" monsters, cause the attr/char codes
 * to be "scrambled" in various ways.
 *
 * Note that eventually we may use the "&" symbol for embedded treasure,
 * and use the "*" symbol to indicate multiple objects, though this will
 * have to wait for Angband 2.8.0 or later.  Note that currently, this
 * is not important, since only one object or terrain feature is allowed
 * in each grid.  If needed, "k_info[0]" will hold the "stack" attr/char.
 *
 * Note the assumption that doing "x_ptr = &x_info[x]" plus a few of
 * "x_ptr->xxx", is quicker than "x_info[x].xxx", if this is incorrect
 * then a whole lot of code should be changed...  XXX XXX
 */

/*:::���C����ʂ̎w��ʒu�ɂǂ̕����L����\������̂��v�Z�@�����X�^�[�E�A�C�e���E�n�`�̏��ɗD��@�ڍז���*/
/*:::���o����������*/
void map_info(int y, int x, byte *ap, char *cp, byte *tap, char *tcp)
{
	/* Get the cave */
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;

	/* Feature code (applying "mimic" field) */
	s16b feat = get_feat_mimic(c_ptr);

	/* Access floor */
	feature_type *f_ptr = &f_info[feat];

	byte a;
	byte c;

	/* Boring grids (floors, etc) */
	if (!have_flag(f_ptr->flags, FF_REMEMBER))
	{
		/*
		 * Handle Memorized or visible floor
		 *
		 * No visual when blinded.
		 *   (to prevent strange effects on darkness breath)
		 * otherwise,
		 * - Can see grids with CAVE_MARK.
		 * - Can see grids with CAVE_LITE or CAVE_MNLT.
		 *   (Such grids also have CAVE_VIEW)
		 * - Can see grids with CAVE_VIEW unless darkened by monsters.
		 */
		if (!p_ptr->blind &&
		    ((c_ptr->info & (CAVE_MARK | CAVE_LITE | CAVE_MNLT)) ||
		     ((c_ptr->info & CAVE_VIEW) && (((c_ptr->info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW) || p_ptr->see_nocto))))
		{
			/* Normal attr/char */
			a = f_ptr->x_attr[F_LIT_STANDARD];
			c = f_ptr->x_char[F_LIT_STANDARD];

			if (p_ptr->wild_mode)
			{
				/* Special lighting effects */
				/* Handle "night" */
				if (view_special_lite && !is_daytime())
				{
					/* Use a darkened colour/tile */
					a = f_ptr->x_attr[F_LIT_DARK];
					c = f_ptr->x_char[F_LIT_DARK];
				}
			}
			///mod150818 ���s���ۓW�N�G�X�g���ꏈ��
			else if(p_ptr->inside_quest == QUEST_MOON_VAULT && quest[QUEST_MOON_VAULT].flags & QUEST_FLAG_SPECIAL)
			{
				a = TERM_BLUE;
			}

			/* Mega-Hack -- Handle "in-sight" and "darkened" grids */
			else if (darkened_grid(c_ptr))
			{
				/* Unsafe cave grid -- idea borrowed from Unangband */
				feat = (view_unsafe_grids && (c_ptr->info & CAVE_UNSAFE)) ? feat_undetected : feat_none;

				/* Access darkness */
				f_ptr = &f_info[feat];

				/* Char and attr of darkness */
				a = f_ptr->x_attr[F_LIT_STANDARD];
				c = f_ptr->x_char[F_LIT_STANDARD];
			}
			//v1.1.35
			else if(c_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)
			{
				a = TERM_RED;
			}

			/* Special lighting effects */
			else if (view_special_lite)
			{
				/* Handle "torch-lit" grids */
				if (c_ptr->info & (CAVE_LITE | CAVE_MNLT))
				{
					/* Torch lite */
					if (view_yellow_lite)
					{
						/* Use a brightly lit colour/tile */
						a = f_ptr->x_attr[F_LIT_LITE];
						c = f_ptr->x_char[F_LIT_LITE];
					}
				}

				/* Handle "dark" grids */
				else if ((c_ptr->info & (CAVE_GLOW | CAVE_MNDK)) != CAVE_GLOW)
				{
					/* Use a darkened colour/tile */
					a = f_ptr->x_attr[F_LIT_DARK];
					c = f_ptr->x_char[F_LIT_DARK];
				}

				/* Handle "out-of-sight" grids */
				else if (!(c_ptr->info & CAVE_VIEW))
				{
					/* Special flag */
					if (view_bright_lite)
					{
						/* Use a darkened colour/tile */
						a = f_ptr->x_attr[F_LIT_DARK];
						c = f_ptr->x_char[F_LIT_DARK];
					}
				}
			}
		}

		/* Unknown */
		else
		{
			/* Unsafe cave grid -- idea borrowed from Unangband */
			feat = (view_unsafe_grids && (c_ptr->info & CAVE_UNSAFE)) ? feat_undetected : feat_none;

			/* Access darkness */
			f_ptr = &f_info[feat];

			/* Normal attr/char */
			a = f_ptr->x_attr[F_LIT_STANDARD];
			c = f_ptr->x_char[F_LIT_STANDARD];
		}
	}

	/* Interesting grids (non-floors) */
	else
	{
		/* Memorized grids */
		if (c_ptr->info & CAVE_MARK)
		{
			/* Normal attr/char */
			a = f_ptr->x_attr[F_LIT_STANDARD];
			c = f_ptr->x_char[F_LIT_STANDARD];

			if (p_ptr->wild_mode)
			{
				/* Special lighting effects */
				/* Handle "blind" or "night" */
				if (view_granite_lite && (p_ptr->blind || !is_daytime()))
				{
					/* Use a darkened colour/tile */
					a = f_ptr->x_attr[F_LIT_DARK];
					c = f_ptr->x_char[F_LIT_DARK];
				}
			}

			/* Mega-Hack -- Handle "in-sight" and "darkened" grids */
			else if (darkened_grid(c_ptr) && !p_ptr->blind)
			{
				if (have_flag(f_ptr->flags, FF_LOS) && have_flag(f_ptr->flags, FF_PROJECT))
				{
					/* Unsafe cave grid -- idea borrowed from Unangband */
					feat = (view_unsafe_grids && (c_ptr->info & CAVE_UNSAFE)) ? feat_undetected : feat_none;

					/* Access darkness */
					f_ptr = &f_info[feat];

					/* Char and attr of darkness */
					a = f_ptr->x_attr[F_LIT_STANDARD];
					c = f_ptr->x_char[F_LIT_STANDARD];
				}
				else if (view_granite_lite && view_bright_lite)
				{
					/* Use a darkened colour/tile */
					a = f_ptr->x_attr[F_LIT_DARK];
					c = f_ptr->x_char[F_LIT_DARK];
				}
			}

			/* Special lighting effects */
			else if (view_granite_lite)
			{
				/* Handle "blind" */
				if (p_ptr->blind)
				{
					/* Use a darkened colour/tile */
					a = f_ptr->x_attr[F_LIT_DARK];
					c = f_ptr->x_char[F_LIT_DARK];
				}

				/* Handle "torch-lit" grids */
				else if (c_ptr->info & (CAVE_LITE | CAVE_MNLT))
				{
					/* Torch lite */
					if (view_yellow_lite)
					{
						/* Use a brightly lit colour/tile */
						a = f_ptr->x_attr[F_LIT_LITE];
						c = f_ptr->x_char[F_LIT_LITE];
					}
				}

				/* Handle "view_bright_lite" */
				else if (view_bright_lite)
				{
					/* Not viewable */
					if (!(c_ptr->info & CAVE_VIEW))
					{
						/* Use a darkened colour/tile */
						a = f_ptr->x_attr[F_LIT_DARK];
						c = f_ptr->x_char[F_LIT_DARK];
					}

					/* Not glowing */
					else if ((c_ptr->info & (CAVE_GLOW | CAVE_MNDK)) != CAVE_GLOW)
					{
						/* Use a darkened colour/tile */
						a = f_ptr->x_attr[F_LIT_DARK];
						c = f_ptr->x_char[F_LIT_DARK];
					}

					/* Not glowing correctly */
					else if (!have_flag(f_ptr->flags, FF_LOS) && !check_local_illumination(y, x))
					{
						/* Use a darkened colour/tile */
						a = f_ptr->x_attr[F_LIT_DARK];
						c = f_ptr->x_char[F_LIT_DARK];
					}
				}
			}
		}

		/* Unknown */
		else
		{
			/* Unsafe cave grid -- idea borrowed from Unangband */
			feat = (view_unsafe_grids && (c_ptr->info & CAVE_UNSAFE)) ? feat_undetected : feat_none;

			/* Access feature */
			f_ptr = &f_info[feat];

			/* Normal attr/char */
			a = f_ptr->x_attr[F_LIT_STANDARD];
			c = f_ptr->x_char[F_LIT_STANDARD];
		}

		///mod150818 ���s���ۓW�N�G�X�g���ꏈ��
		if(p_ptr->inside_quest == QUEST_MOON_VAULT && quest[QUEST_MOON_VAULT].flags & QUEST_FLAG_SPECIAL)
		{
			a = TERM_BLUE;
		}
		//v1.1.48 �����̃X�[�p�[�n�R�_���[�h
		if (SUPER_SHION)
		{
			a = TERM_BLUE;
		}
	}

	if (feat_priority == -1) feat_priority = f_ptr->priority;

	/* Save the terrain info for the transparency effects */
	(*tap) = a;
	(*tcp) = c;

	/* Save the info */
	(*ap) = a;
	(*cp) = c;

	/* Hack -- rare random hallucination, except on outer dungeon walls */
	if (p_ptr->image)
	{
		if (one_in_(256))
		{
			/* Hallucinate */
			image_random(ap, cp);
		}
	}

	/* Objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized objects */
		if (o_ptr->marked & OM_FOUND)
		{
			if (display_autopick)
			{
				byte act;

				match_autopick = is_autopick(o_ptr);
				if(match_autopick == -1)
					continue;

				act = autopick_list[match_autopick].action;

				if ((act & DO_DISPLAY) && (act & display_autopick))
				{
					autopick_obj = o_ptr;
				}
				else
				{
					match_autopick = -1;
					continue;
				}
			}
			/* Normal char */
			(*cp) = object_char(o_ptr);

			/* Normal attr */
			(*ap) = object_attr(o_ptr);

			feat_priority = 20;

			/* Hack -- hallucination */
			if (p_ptr->image) image_object(ap, cp);

			/* Done */
			break;
		}
	}


	/* Handle monsters */
	if (c_ptr->m_idx && display_autopick == 0 )
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];

		/* Visible monster */
		if (m_ptr->ml)
		{
			monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];

			feat_priority = 30;

			/* Hallucination */
			///mod150328 �ʂ��̐��̕s���̎폈���ǉ�
			if (p_ptr->image ||  (m_ptr->mflag & MFLAG_NUE_UNKNOWN))
			{
				/*
				 * Monsters with both CHAR_CLEAR and ATTR_CLEAR
				 * flags are always unseen.
				 */
				if ((r_ptr->flags1 & (RF1_CHAR_CLEAR | RF1_ATTR_CLEAR)) == (RF1_CHAR_CLEAR | RF1_ATTR_CLEAR))
				{
					/* Do nothing */
				}
				else
				{
					/* Hallucinatory monster */
					image_monster(ap, cp);
				}
			}
			else
			{
				/* Monster attr/char */
				a = r_ptr->x_attr;
				c = r_ptr->x_char;

				//�}�~�]�E���Z�Ŏ�̉�
				if(p_ptr->pclass == CLASS_MAMIZOU && p_ptr->magic_num1[0] == c_ptr->m_idx)
					c = 'x';

				/* Normal monsters */
				if (!(r_ptr->flags1 & (RF1_CHAR_CLEAR | RF1_SHAPECHANGER | RF1_ATTR_CLEAR
					                   | RF1_ATTR_MULTI | RF1_ATTR_SEMIRAND)))
				{
					/* Desired monster attr/char */
					*ap = a;
					*cp = c;
				}

				/*
				 * Monsters with both CHAR_CLEAR and ATTR_CLEAR
				 * flags are always unseen.
				 */
				else if ((r_ptr->flags1 & (RF1_CHAR_CLEAR | RF1_ATTR_CLEAR)) == (RF1_CHAR_CLEAR | RF1_ATTR_CLEAR))
				{
					/* Do nothing */
				}

				else
				{
					/***  Monster's attr  ***/
					if ((r_ptr->flags1 & RF1_ATTR_CLEAR) && (*ap != TERM_DARK) && !use_graphics)
					{
						/* Clear-attr */
						/* Do nothing */
					}
					else if ((r_ptr->flags1 & RF1_ATTR_MULTI) && !use_graphics)
					{
						/* Multi-hued attr */
						if (r_ptr->flags2 & RF2_ATTR_ANY) *ap = randint1(15);
						else switch (randint1(7))
						{
						case 1: *ap = TERM_RED;     break;
						case 2: *ap = TERM_L_RED;   break;
						case 3: *ap = TERM_WHITE;   break;
						case 4: *ap = TERM_L_GREEN; break;
						case 5: *ap = TERM_BLUE;    break;
						case 6: *ap = TERM_L_DARK;  break;
						case 7: *ap = TERM_GREEN;   break;
						}
					}
					else if ((r_ptr->flags1 & RF1_ATTR_SEMIRAND) && !use_graphics)
					{
						/* Use semi-random attr (usually mimics' colors vary) */
						*ap = c_ptr->m_idx % 15 + 1;
					}
					else
					{
						/* Normal case */
						*ap = a;
					}



					/***  Monster's char  ***/
					if ((r_ptr->flags1 & RF1_CHAR_CLEAR) && (*cp != ' ') && !use_graphics)
					{
						/* Clear-char */
						/* Do nothing */
					}
					else if (r_ptr->flags1 & RF1_SHAPECHANGER)
					{
						if (use_graphics)
						{
							monster_race *tmp_r_ptr = &r_info[randint1(max_r_idx - 1)];
							*cp = tmp_r_ptr->x_char;
							*ap = tmp_r_ptr->x_attr;
						}
						else
						{
							*cp = (one_in_(25) ?
							       image_object_hack[randint0(sizeof(image_object_hack) - 1)] :
							       image_monster_hack[randint0(sizeof(image_monster_hack) - 1)]);
						}
					}
					else
					{
						/* Normal case */
						*cp = c;
					}
				}

				if (MON_BERSERK(m_ptr) && one_in_(2)) *ap = TERM_RED;

			}
		}
	}

	/* Handle "player" */
	if (player_bold(y, x))
	{
		monster_race *r_ptr = &r_info[0];

		/* Get the "player" attr */
		*ap = r_ptr->x_attr;

		/* Get the "player" char */
		*cp = r_ptr->x_char;

		feat_priority = 31;
	}
}


/*
 * Calculate panel colum of a location in the map
 */
static int panel_col_of(int col)
{
	col -= panel_col_min;
	if (use_bigtile) col *= 2;
///mod140126 ��ʍ������L�����鏈��
	//return col + 13; 
	return col + 17; 

}


/*
 * Moves the cursor to a given MAP (y,x) location
 */
void move_cursor_relative(int row, int col)
{
	/* Real co-ords convert to screen positions */
	row -= panel_row_prt;

	/* Go there */
	Term_gotoxy(panel_col_of(col), row);
}



/*
 * Place an attr/char pair at the given map coordinate, if legal.
 */
/*:::�w��ʒu�Ɏw��F�̕�����1�����`��*/
void print_rel(char c, byte a, int y, int x)
{
	/* Only do "legal" locations */
	if (panel_contains(y, x))
	{
		/* Hack -- fake monochrome */
		if (!use_graphics)
		{
			if (world_monster) a = TERM_DARK;
			else if (SUPER_SHION) a = TERM_L_BLUE;
			else if (SAKUYA_WORLD)a = TERM_L_DARK;
			else if (IS_INVULN() || world_player) a = TERM_WHITE;
			else if (p_ptr->wraith_form) a = TERM_L_DARK;
		}

		/* Draw the char using the attr */
		Term_queue_bigchar(panel_col_of(x), y-panel_row_prt, a, c, 0, 0);
	}
}





/*
 * Memorize interesting viewable object/features in the given grid
 *
 * This function should only be called on "legal" grids.
 *
 * This function will memorize the object and/or feature in the given
 * grid, if they are (1) viewable and (2) interesting.  Note that all
 * objects are interesting, all terrain features except floors (and
 * invisible traps) are interesting, and floors (and invisible traps)
 * are interesting sometimes (depending on various options involving
 * the illumination of floor grids).
 *
 * The automatic memorization of all objects and non-floor terrain
 * features as soon as they are displayed allows incredible amounts
 * of optimization in various places, especially "map_info()".
 *
 * Note that the memorization of objects is completely separate from
 * the memorization of terrain features, preventing annoying floor
 * memorization when a detected object is picked up from a dark floor,
 * and object memorization when an object is dropped into a floor grid
 * which is memorized but out-of-sight.
 *
 * This function should be called every time the "memorization" of
 * a grid (or the object in a grid) is called into question, such
 * as when an object is created in a grid, when a terrain feature
 * "changes" from "floor" to "non-floor", when any grid becomes
 * "illuminated" or "viewable", and when a "floor" grid becomes
 * "torch-lit".
 *
 * Note the relatively efficient use of this function by the various
 * "update_view()" and "update_lite()" calls, to allow objects and
 * terrain features to be memorized (and drawn) whenever they become
 * viewable or illuminated in any way, but not when they "maintain"
 * or "lose" their previous viewability or illumination.
 *
 * Note the butchered "internal" version of "player_can_see_bold()",
 * optimized primarily for the most common cases, that is, for the
 * non-marked floor grids.
 */
/*:::�t���A�̕ǂ�A�C�e���E�g���b�v���L�����ď����L�����Ȃ��Ȃǂ̏���*/
/*:::cave[][].info��CAVE_MARK�t���O�𗧂Ă邱�ƂŋL������*/
void note_spot(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;


	/* Blind players see nothing */
	if (p_ptr->blind) return;

	/*:::�Ƃ炳��ĂȂ������̒ʂ��ĂȂ��}�X�A���E�������Â��ăm�N�g�r�W�����̂Ȃ����͉������Ȃ��H*/
	/* Analyze non-torch-lit grids */

	if (!(c_ptr->info & (CAVE_LITE | CAVE_MNLT)))
	{
		/* Require line of sight to the grid */
		if (!(c_ptr->info & (CAVE_VIEW))) return;

		/* Require "perma-lite" of the grid */
		/*:::�uGLOW��TRUE�A����MNDK��FALSE�v�ȊO�͂����ɓ���*/
		if ((c_ptr->info & (CAVE_GLOW | CAVE_MNDK)) != CAVE_GLOW)
		{
			/* Not Ninja */
			if (!p_ptr->see_nocto) return;
		}
	}


	/* Hack -- memorize objects */
	/*:::�Ώۃ}�X�ɃA�C�e���������Ă����ꍇo_list[].marked��OM_FOUND�t���O��ǉ�*/
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorize objects */
		o_ptr->marked |= OM_FOUND;
	}


	/* Hack -- memorize grids */
	if (!(c_ptr->info & (CAVE_MARK)))
	{
		/* Feature code (applying "mimic" field) */
		feature_type *f_ptr = &f_info[get_feat_mimic(c_ptr)];

		/* Memorize some "boring" grids */
		if (!have_flag(f_ptr->flags, FF_REMEMBER))
		{
			/* Option -- memorize all torch-lit floors */
			if (view_torch_grids &&
			    ((c_ptr->info & (CAVE_LITE | CAVE_MNLT)) || p_ptr->see_nocto))
			{
				/* Memorize */
				c_ptr->info |= (CAVE_MARK);
			}

			/* Option -- memorize all perma-lit floors */
			else if (view_perma_grids && ((c_ptr->info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW))
			{
				/* Memorize */
				c_ptr->info |= (CAVE_MARK);
			}
		}

		/* Memorize normal grids */
		else if (have_flag(f_ptr->flags, FF_LOS))
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}

		/* Memorize torch-lit walls */
		else if (c_ptr->info & (CAVE_LITE | CAVE_MNLT))
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}

		/* Memorize walls seen by noctovision of Ninja */
		else if (p_ptr->see_nocto)
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}

		/* Memorize certain non-torch-lit wall grids */
		else if (check_local_illumination(y, x))
		{
			/* Memorize */
			c_ptr->info |= (CAVE_MARK);
		}
	}

	/* Memorize terrain of the grid */
	c_ptr->info |= (CAVE_KNOWN);
}

/*:::�T�u�E�B���h�E�Ɏ��͂̒n�`��\������I�v�V�����p*/
void display_dungeon(void)
{
	int x, y;
	byte a;
	char c;

	byte ta;
	char tc;

	for (x = px - Term->wid / 2 + 1; x <= px + Term->wid / 2; x++)
	{
		for (y = py - Term->hgt / 2 + 1; y <= py + Term->hgt / 2; y++)
		{
			if (in_bounds2(y, x))
			{

				/* Examine the grid */
				map_info(y, x, &a, &c, &ta, &tc);

				/* Hack -- fake monochrome */
				if (!use_graphics)
				{
					if (world_monster) a = TERM_DARK;
					else if (SUPER_SHION) a = TERM_L_BLUE;
					else if (SAKUYA_WORLD)a = TERM_L_DARK;
					else if (IS_INVULN()  || world_player) a = TERM_WHITE;
					else if (p_ptr->wraith_form) a = TERM_L_DARK;
				}

				/* Hack -- Queue it */
				Term_queue_char(x - px + Term->wid / 2 - 1, y - py + Term->hgt / 2 - 1, a, c, ta, tc);
			}
			else
			{
				/* Clear out-of-bound tiles */

				/* Access darkness */
				feature_type *f_ptr = &f_info[feat_none];

				/* Normal attr */
				a = f_ptr->x_attr[F_LIT_STANDARD];

				/* Normal char */
				c = f_ptr->x_char[F_LIT_STANDARD];

				/* Hack -- Queue it */
				Term_queue_char(x - px + Term->wid / 2 - 1, y - py + Term->hgt / 2 - 1, a, c, ta, tc);
			}
		}
	}
}


/*
 * Redraw (on the screen) a given MAP location
 *
 * This function should only be called on "legal" grids
 */
/*:::�w��O���b�h���ĕ`��*/
void lite_spot(int y, int x)
{
	/* Redraw if on screen */
	if (panel_contains(y, x) && in_bounds2(y, x))
	{
		byte a;
		char c;

		byte ta;
		char tc;

		/* Examine the grid */
		map_info(y, x, &a, &c, &ta, &tc);

		/* Hack -- fake monochrome */
		if (!use_graphics)
		{
			if (world_monster) a = TERM_DARK;
			else if (SUPER_SHION) a = TERM_L_BLUE;

			else if (SAKUYA_WORLD)a = TERM_L_DARK;
			else if (IS_INVULN() || world_player) a = TERM_WHITE;
			else if (p_ptr->wraith_form) a = TERM_L_DARK;
		}

		/* Hack -- Queue it */
		Term_queue_bigchar(panel_col_of(x), y-panel_row_prt, a, c, ta, tc);

		/* Update sub-windows */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
	}
}


/*
 * Prints the map of the dungeon
 *
 * Note that, for efficiency, we contain an "optimized" version
 * of both "lite_spot()" and "print_rel()", and that we use the
 * "lite_spot()" function to display the player grid, if needed.
 */
void prt_map(void)
{
	int     x, y;
	int     v;

	/* map bounds */
	s16b xmin, xmax, ymin, ymax;

	int wid, hgt;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Remove map offset */
	wid -= COL_MAP + 2;
	hgt -= ROW_MAP + 2;

	/* Access the cursor state */
	(void)Term_get_cursor(&v);

	/* Hide the cursor */
	(void)Term_set_cursor(0);

	/* Get bounds */
	xmin = (0 < panel_col_min) ? panel_col_min : 0;
	xmax = (cur_wid - 1 > panel_col_max) ? panel_col_max : cur_wid - 1;
	ymin = (0 < panel_row_min) ? panel_row_min : 0;
	ymax = (cur_hgt - 1 > panel_row_max) ? panel_row_max : cur_hgt - 1;

	/* Bottom section of screen */
	for (y = 1; y <= ymin - panel_row_prt; y++)
	{
		/* Erase the section */
		Term_erase(COL_MAP, y, wid);
	}

	/* Top section of screen */
	/*:::���C����ʂ̃}�b�v�������N���A*/
	for (y = ymax - panel_row_prt; y <= hgt; y++)
	{
		/* Erase the section */
		Term_erase(COL_MAP, y, wid);
	}

	/* Dump the map */
	for (y = ymin; y <= ymax; y++)
	{
		/* Scan the columns of row "y" */
		for (x = xmin; x <= xmax; x++)
		{
			byte a;
			char c;

			byte ta;
			char tc;

			/* Determine what is there */
			map_info(y, x, &a, &c, &ta, &tc);

			/* Hack -- fake monochrome */
			if (!use_graphics)
			{
				if (world_monster) a = TERM_DARK;
				else if (SUPER_SHION) a = TERM_L_BLUE;

				else if (SAKUYA_WORLD)a = TERM_L_DARK;
				else if (IS_INVULN() || world_player) a = TERM_WHITE;
				else if (p_ptr->wraith_form) a = TERM_L_DARK;
			}

			/* Efficiency -- Redraw that grid of the map */
			Term_queue_bigchar(panel_col_of(x), y-panel_row_prt, a, c, ta, tc);
		}
	}

	/* Display player */
	lite_spot(py, px);

	/* Restore the cursor */
	(void)Term_set_cursor(v);
}



/*
 * print project path
 */
void prt_path(int y, int x)
{
	int i;
	int path_n;
	u16b path_g[512];
	int default_color = TERM_SLATE;

	if (!display_path) return;
	if (-1 == project_length)
		return;

	/* Get projection path */
	path_n = project_path(path_g, (project_length ? project_length : MAX_RANGE), py, px, y, x, PROJECT_PATH|PROJECT_THRU);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Redraw stuff */
	redraw_stuff();

	/* Draw path */
	for (i = 0; i < path_n; i++)
	{
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);
		cave_type *c_ptr = &cave[ny][nx];

		if (panel_contains(ny, nx))
		{
			byte a = default_color;
			char c;

			byte ta;
			char tc;

			if (c_ptr->m_idx && m_list[c_ptr->m_idx].ml)
			{
				/* Determine what is there */
				map_info(ny, nx, &a, &c, &ta, &tc);

				if (!is_ascii_graphics(a))
					a = default_color;
				else if (c == '.' && (a == TERM_WHITE || a == TERM_L_WHITE))
					a = default_color;
				else if (a == default_color)
					a = TERM_WHITE;
			}

			if (!use_graphics)
			{
				if (world_monster) a = TERM_DARK;
				else if (SUPER_SHION) a = TERM_L_BLUE;

				else if (SAKUYA_WORLD)a = TERM_L_DARK;
				else if (IS_INVULN() || world_player) a = TERM_WHITE;
				else if (p_ptr->wraith_form) a = TERM_L_DARK;
			}

			c = '*';

			/* Hack -- Queue it */
			Term_queue_bigchar(panel_col_of(nx), ny-panel_row_prt, a, c, ta, tc);
		}

		/* Known Wall */
		if ((c_ptr->info & CAVE_MARK) && !cave_have_flag_grid(c_ptr, FF_PROJECT)) break;

		/* Change color */
		if (nx == x && ny == y) default_color = TERM_L_DARK;
	}
}


static cptr simplify_list[][2] =
{
#ifdef JP
	{"�̖��@��", ""},
	{NULL, NULL}
#else
	{"^Ring of ",   "="},
	{"^Amulet of ", "\""},
	{"^Scroll of ", "?"},
	{"^Scroll titled ", "?"},
	{"^Wand of "  , "-"},
	{"^Rod of "   , "-"},
	{"^Staff of " , "_"},
	{"^Potion of ", "!"},
	{" Spellbook ",""},
	{"^Book of ",   ""},
	{" Magic [",   "["},
	{" Book [",    "["},
	{" Arts [",    "["},
	{"^Set of ",    ""},
	{"^Pair of ",   ""},
	{NULL, NULL}
#endif
};

/*:::�~�j�}�b�v�̍����ɃA�C�e������\��*/
static void display_shortened_item_name(object_type *o_ptr, int y)
{
	char buf[MAX_NLEN];
	char *c = buf;
	int len = 0;
	byte attr;

	object_desc(buf, o_ptr, (OD_NO_FLAVOR | OD_OMIT_PREFIX | OD_NAME_ONLY));
	attr = tval_to_attr[o_ptr->tval % 128];

	if (p_ptr->image)
	{
		attr = TERM_WHITE;

#ifdef JP
		strcpy(buf, "������ȕ�");
#else
		strcpy(buf, "something strange");
#endif
	}

	for (c = buf; *c; c++)
	{
		int i;
		for (i = 0; simplify_list[i][1]; i++)
		{
			cptr org_w = simplify_list[i][0];

			if (*org_w == '^')
			{
				if (c == buf)
					org_w++;
				else
					continue;
			}

			if (!strncmp(c, org_w, strlen(org_w)))
			{
				char *s = c;
				cptr tmp = simplify_list[i][1];
				while (*tmp)
					*s++ = *tmp++;
				tmp = c + strlen(org_w);
				while (*tmp)
					*s++ = *tmp++;
				*s = '\0';
			}
		}
	}

	c = buf;
	len = 0;
	/* ���p 12 �������Ő؂� */
	///mod140426 ��18�����܂Ŋg������
	while(*c)
	{
#ifdef JP
		if(iskanji(*c))
		{
			if(len + 2 > 18) break;
			c+=2;
			len+=2;
		}
		else
#endif
		{
			if(len + 1 > 18) break;
			c++;
			len++;
		}
	}
	*c='\0';
	Term_putstr(0, y, 18, attr, buf);
}

/*
 * Display a "small-scale" map of the dungeon in the active Term
 */
/*:::�~�j�S�̃}�b�v��\������BM�R�}���h�̂ق��T�u�E�B���h�E�̃}�b�v�\���Ŏg��*/
/*:::cy,cx�̓~�j�}�b�v���Ł��̈ʒu��Ԃ��̂Ɏg����BM�R�}���h�̃J�[�\������Ɏg���炵��*/
void display_map(int *cy, int *cx)
{
	int i, j, x, y;

	byte ta;
	char tc;

	byte tp;

	byte **bigma;
	char **bigmc;
	byte **bigmp;

	byte **ma;
	char **mc;
	byte **mp;

	/* Save lighting effects */
	bool old_view_special_lite = view_special_lite;
	bool old_view_granite_lite = view_granite_lite;

	int hgt, wid, yrat, xrat;

	int **match_autopick_yx;
	object_type ***object_autopick_yx;

	/* Get size */
	Term_get_size(&wid, &hgt);
	hgt -= 2;
	wid -= 14;
	if (use_bigtile) wid /= 2;

	yrat = (cur_hgt + hgt - 1) / hgt;
	xrat = (cur_wid + wid - 1) / wid;

	/* Disable lighting effects */
	view_special_lite = FALSE;
	view_granite_lite = FALSE;

	/* Allocate the maps */
	C_MAKE(ma, (hgt + 2), byte_ptr);
	C_MAKE(mc, (hgt + 2), char_ptr);
	C_MAKE(mp, (hgt + 2), byte_ptr);
	C_MAKE(match_autopick_yx, (hgt + 2), sint_ptr);
	C_MAKE(object_autopick_yx, (hgt + 2), object_type **);

	/* Allocate and wipe each line map */
	for (y = 0; y < (hgt + 2); y++)
	{
		/* Allocate one row each array */
		C_MAKE(ma[y], (wid + 2), byte);
		C_MAKE(mc[y], (wid + 2), char);
		C_MAKE(mp[y], (wid + 2), byte);
		C_MAKE(match_autopick_yx[y], (wid + 2), int);
		C_MAKE(object_autopick_yx[y], (wid + 2), object_type *);

		for (x = 0; x < wid + 2; ++x)
		{
			match_autopick_yx[y][x] = -1;
			object_autopick_yx[y][x] = NULL;

			/* Nothing here */
			ma[y][x] = TERM_WHITE;
			mc[y][x] = ' ';

			/* No priority */
			mp[y][x] = 0;
		}
	}

	/* Allocate the maps */
	C_MAKE(bigma, (cur_hgt + 2), byte_ptr);
	C_MAKE(bigmc, (cur_hgt + 2), char_ptr);
	C_MAKE(bigmp, (cur_hgt + 2), byte_ptr);

	/* Allocate and wipe each line map */
	for (y = 0; y < (cur_hgt + 2); y++)
	{
		/* Allocate one row each array */
		C_MAKE(bigma[y], (cur_wid + 2), byte);
		C_MAKE(bigmc[y], (cur_wid + 2), char);
		C_MAKE(bigmp[y], (cur_wid + 2), byte);

		for (x = 0; x < cur_wid + 2; ++x)
		{
			/* Nothing here */
			bigma[y][x] = TERM_WHITE;
			bigmc[y][x] = ' ';

			/* No priority */
			bigmp[y][x] = 0;
		}
	}

	/* Fill in the map */
	for (i = 0; i < cur_wid; ++i)
	{
		for (j = 0; j < cur_hgt; ++j)
		{
			/* Location */
			x = i / xrat + 1;
			y = j / yrat + 1;

			match_autopick=-1;
			autopick_obj=NULL;
			feat_priority = -1;

			/* Extract the current attr/char at that map location */
			map_info(j, i, &ta, &tc, &ta, &tc);

			/* Extract the priority */
			tp = feat_priority;

			if(match_autopick!=-1
			   && (match_autopick_yx[y][x] == -1
			       || match_autopick_yx[y][x] > match_autopick))
			{
				match_autopick_yx[y][x] = match_autopick;
				object_autopick_yx[y][x] = autopick_obj;
				tp = 0x7f;
			}

			/* Save the char, attr and priority */
			bigmc[j+1][i+1] = tc;
			bigma[j+1][i+1] = ta;
			bigmp[j+1][i+1] = tp;
		}
	}

	for (j = 0; j < cur_hgt; ++j)
	{
		for (i = 0; i < cur_wid; ++i)
		{
			/* Location */
			x = i / xrat + 1;
			y = j / yrat + 1;

			tc = bigmc[j+1][i+1];
			ta = bigma[j+1][i+1];
			tp = bigmp[j+1][i+1];

			/* rare feature has more priority */
			if (mp[y][x] == tp)
			{
				int t;
				int cnt = 0;

				for (t = 0; t < 8; t++)
				{
					if (tc == bigmc[j+1+ddy_cdd[t]][i+1+ddx_cdd[t]] &&
					    ta == bigma[j+1+ddy_cdd[t]][i+1+ddx_cdd[t]])
						cnt++;
				}
				if (cnt <= 4)
					tp++;
			}

			/* Save "best" */
			if (mp[y][x] < tp)
			{
				/* Save the char, attr and priority */
				mc[y][x] = tc;
				ma[y][x] = ta;
				mp[y][x] = tp;
			}
		}
	}


	/* Corners */
	x = wid + 1;
	y = hgt + 1;

	/* Draw the corners */
	mc[0][0] = mc[0][x] = mc[y][0] = mc[y][x] = '+';

	/* Draw the horizontal edges */
	for (x = 1; x <= wid; x++) mc[0][x] = mc[y][x] = '-';

	/* Draw the vertical edges */
	for (y = 1; y <= hgt; y++) mc[y][0] = mc[y][x] = '|';


	/* Display each map line in order */
	for (y = 0; y < hgt + 2; ++y)
	{
		/* Start a new line */
		Term_gotoxy(COL_MAP, y);

		/* Display the line */
		for (x = 0; x < wid + 2; ++x)
		{
			ta = ma[y][x];
			tc = mc[y][x];

			/* Hack -- fake monochrome */
			if (!use_graphics)
			{
				if (world_monster) ta = TERM_DARK;
				else if (SUPER_SHION) ta = TERM_L_BLUE;

				else if (SAKUYA_WORLD) ta = TERM_L_DARK;
				else if (IS_INVULN() || world_player) ta = TERM_WHITE;
				else if (p_ptr->wraith_form) ta = TERM_L_DARK;
			}

			/* Add the character */
			Term_add_bigch(ta, tc);
		}
	}


	for (y = 1; y < hgt + 1; ++y)
	{
	  match_autopick = -1;
	  for (x = 1; x <= wid; x++){
	    if (match_autopick_yx[y][x] != -1 &&
		(match_autopick > match_autopick_yx[y][x] ||
		 match_autopick == -1)){
	      match_autopick = match_autopick_yx[y][x];
	      autopick_obj = object_autopick_yx[y][x];
	    }
	  }

	  /* Clear old display */
	//  Term_putstr(0, y, 12, 0, "                  ");
	  Term_putstr(0, y, 18, 0, "                  ");

	  if (match_autopick != -1)
#if 1
		  display_shortened_item_name(autopick_obj, y);
#else
	  {
		  char buf[13] = "\0";
		  strncpy(buf,autopick_list[match_autopick].name,12);
		  buf[12] = '\0';
		  put_str(buf,y,0); 
	  }
#endif

	}

	/* Player location */
		(*cy) = py / yrat + 1 + ROW_MAP;
	if (!use_bigtile)
		(*cx) = px / xrat + 1 + COL_MAP;
	else
		(*cx) = (px / xrat + 1) * 2 + COL_MAP;

	/* Restore lighting effects */
	view_special_lite = old_view_special_lite;
	view_granite_lite = old_view_granite_lite;

	/* Free each line map */
	for (y = 0; y < (hgt + 2); y++)
	{
		/* Free one row each array */
		C_KILL(ma[y], (wid + 2), byte);
		C_KILL(mc[y], (wid + 2), char);
		C_KILL(mp[y], (wid + 2), byte);
		C_KILL(match_autopick_yx[y], (wid + 2), int);
		C_KILL(object_autopick_yx[y], (wid + 2), object_type *);
	}

	/* Free each line map */
	C_KILL(ma, (hgt + 2), byte_ptr);
	C_KILL(mc, (hgt + 2), char_ptr);
	C_KILL(mp, (hgt + 2), byte_ptr);
	C_KILL(match_autopick_yx, (hgt + 2), sint_ptr);
	C_KILL(object_autopick_yx, (hgt + 2), object_type **);

	/* Free each line map */
	for (y = 0; y < (cur_hgt + 2); y++)
	{
		/* Free one row each array */
		C_KILL(bigma[y], (cur_wid + 2), byte);
		C_KILL(bigmc[y], (cur_wid + 2), char);
		C_KILL(bigmp[y], (cur_wid + 2), byte);
	}

	/* Free each line map */
	C_KILL(bigma, (cur_hgt + 2), byte_ptr);
	C_KILL(bigmc, (cur_hgt + 2), char_ptr);
	C_KILL(bigmp, (cur_hgt + 2), byte_ptr);
}


/*
 * Display a "small-scale" map of the dungeon for the player
 *
 * Currently, the "player" is displayed on the map.  XXX XXX XXX
 */
/*:::�}�b�v������*/
void do_cmd_view_map(void)
{
	int cy, cx;


	/* Save the screen */
	screen_save();

	/* Note */
#ifdef JP
prt("���҂�������...", 0, 0);
#else
	prt("Please wait...", 0, 0);
#endif

	/* Flush */
	Term_fresh();

	/* Clear the screen */
	Term_clear();

	display_autopick = 0;

	/* Display the map */
	display_map(&cy, &cx);

	/* Wait for it */
	if(max_autopick && !p_ptr->wild_mode)
	{
		display_autopick = ITEM_DISPLAY;

		while (1)
		{
			int i;
			byte flag;

			int wid, hgt, row_message;

			Term_get_size(&wid, &hgt);
			row_message = hgt - 1;

#ifdef JP
			put_str("�����L�[�������Ă�������('M':�E�� 'N':���u 'D':M+N 'K':�󂷃A�C�e����\��)", row_message, 1);
#else
			put_str(" Hit M, N(for ~), K(for !), or D(same as M+N) to display auto-picker items.", row_message, 1);
#endif

			/* Hilite the player */
			move_cursor(cy, cx);

			i = inkey();

			if ('M' == i)
				flag = (DO_AUTOPICK | DO_QUERY_AUTOPICK);
			else if ('N' == i)
				flag = DONT_AUTOPICK;
			else if ('K' == i)
				flag = DO_AUTODESTROY;
			else if ('D' == i)
				flag = (DO_AUTOPICK | DO_QUERY_AUTOPICK | DONT_AUTOPICK);
			else
				break;

			Term_fresh();
			
			if (~display_autopick & flag)
				display_autopick |= flag;
			else
				display_autopick &= ~flag;
			/* Display the map */
			display_map(&cy, &cx);
		}
		
		display_autopick = 0;

	}
	else
	{
#ifdef JP
		put_str("�����L�[�������ƃQ�[���ɖ߂�܂�", 23, 30);
#else
		put_str("Hit any key to continue", 23, 30);
#endif		/* Hilite the player */
		move_cursor(cy, cx);
		/* Get any key */
		inkey();
	}

	/* Restore the screen */
	screen_load();
}




/*:::Q022 ���̂����ǂ�ł�����*/
/*
 * Some comments on the cave grid flags.  -BEN-
 *
 *
 * One of the major bottlenecks in previous versions of Angband was in
 * the calculation of "line of sight" from the player to various grids,
 * such as monsters.  This was such a nasty bottleneck that a lot of
 * silly things were done to reduce the dependancy on "line of sight",
 * for example, you could not "see" any grids in a lit room until you
 * actually entered the room, and there were all kinds of bizarre grid
 * flags to enable this behavior.  This is also why the "call light"
 * spells always lit an entire room.
 *
 * The code below provides functions to calculate the "field of view"
 * for the player, which, once calculated, provides extremely fast
 * calculation of "line of sight from the player", and to calculate
 * the "field of torch lite", which, again, once calculated, provides
 * extremely fast calculation of "which grids are lit by the player's
 * lite source".  In addition to marking grids as "GRID_VIEW" and/or
 * "GRID_LITE", as appropriate, these functions maintain an array for
 * each of these two flags, each array containing the locations of all
 * of the grids marked with the appropriate flag, which can be used to
 * very quickly scan through all of the grids in a given set.
 *
 * To allow more "semantically valid" field of view semantics, whenever
 * the field of view (or the set of torch lit grids) changes, all of the
 * grids in the field of view (or the set of torch lit grids) are "drawn"
 * so that changes in the world will become apparent as soon as possible.
 * This has been optimized so that only grids which actually "change" are
 * redrawn, using the "temp" array and the "GRID_TEMP" flag to keep track
 * of the grids which are entering or leaving the relevent set of grids.
 *
 * These new methods are so efficient that the old nasty code was removed.
 *
 * Note that there is no reason to "update" the "viewable space" unless
 * the player "moves", or walls/doors are created/destroyed, and there
 * is no reason to "update" the "torch lit grids" unless the field of
 * view changes, or the "light radius" changes.  This means that when
 * the player is resting, or digging, or doing anything that does not
 * involve movement or changing the state of the dungeon, there is no
 * need to update the "view" or the "lite" regions, which is nice.
 *
 * Note that the calls to the nasty "los()" function have been reduced
 * to a bare minimum by the use of the new "field of view" calculations.
 *
 * I wouldn't be surprised if slight modifications to the "update_view()"
 * function would allow us to determine "reverse line-of-sight" as well
 * as "normal line-of-sight", which would allow monsters to use a more
 * "correct" calculation to determine if they can "see" the player.  For
 * now, monsters simply "cheat" somewhat and assume that if the player
 * has "line of sight" to the monster, then the monster can "pretend"
 * that it has "line of sight" to the player.
 *
 *
 * The "update_lite()" function maintains the "CAVE_LITE" flag for each
 * grid and maintains an array of all "CAVE_LITE" grids.
 *
 * This set of grids is the complete set of all grids which are lit by
 * the players light source, which allows the "player_can_see_bold()"
 * function to work very quickly.
 *
 * Note that every "CAVE_LITE" grid is also a "CAVE_VIEW" grid, and in
 * fact, the player (unless blind) can always "see" all grids which are
 * marked as "CAVE_LITE", unless they are "off screen".
 *
 *
 * The "update_view()" function maintains the "CAVE_VIEW" flag for each
 * grid and maintains an array of all "CAVE_VIEW" grids.
 *
 * This set of grids is the complete set of all grids within line of sight
 * of the player, allowing the "player_has_los_bold()" macro to work very
 * quickly.
 *
 *
 * The current "update_view()" algorithm uses the "CAVE_XTRA" flag as a
 * temporary internal flag to mark those grids which are not only in view,
 * but which are also "easily" in line of sight of the player.  This flag
 * is always cleared when we are done.
 *
 *
 * The current "update_lite()" and "update_view()" algorithms use the
 * "CAVE_TEMP" flag, and the array of grids which are marked as "CAVE_TEMP",
 * to keep track of which grids were previously marked as "CAVE_LITE" or
 * "CAVE_VIEW", which allows us to optimize the "screen updates".
 *
 * The "CAVE_TEMP" flag, and the array of "CAVE_TEMP" grids, is also used
 * for various other purposes, such as spreading lite or darkness during
 * "lite_room()" / "unlite_room()", and for calculating monster flow.
 *
 *
 * Any grid can be marked as "CAVE_GLOW" which means that the grid itself is
 * in some way permanently lit.  However, for the player to "see" anything
 * in the grid, as determined by "player_can_see()", the player must not be
 * blind, the grid must be marked as "CAVE_VIEW", and, in addition, "wall"
 * grids, even if marked as "perma lit", are only illuminated if they touch
 * a grid which is not a wall and is marked both "CAVE_GLOW" and "CAVE_VIEW".
 *
 *
 * To simplify various things, a grid may be marked as "CAVE_MARK", meaning
 * that even if the player cannot "see" the grid, he "knows" the terrain in
 * that grid.  This is used to "remember" walls/doors/stairs/floors when they
 * are "seen" or "detected", and also to "memorize" floors, after "wiz_lite()",
 * or when one of the "memorize floor grids" options induces memorization.
 *
 * Objects are "memorized" in a different way, using a special "marked" flag
 * on the object itself, which is set when an object is observed or detected.
 *
 *
 * A grid may be marked as "CAVE_ROOM" which means that it is part of a "room",
 * and should be illuminated by "lite room" and "darkness" spells.
 *
 *
 * A grid may be marked as "CAVE_ICKY" which means it is part of a "vault",
 * and should be unavailable for "teleportation" destinations.
 *
 *
 * The "view_perma_grids" allows the player to "memorize" every perma-lit grid
 * which is observed, and the "view_torch_grids" allows the player to memorize
 * every torch-lit grid.  The player will always memorize important walls,
 * doors, stairs, and other terrain features, as well as any "detected" grids.
 *
 * Note that the new "update_view()" method allows, among other things, a room
 * to be "partially" seen as the player approaches it, with a growing cone of
 * floor appearing as the player gets closer to the door.  Also, by not turning
 * on the "memorize perma-lit grids" option, the player will only "see" those
 * floor grids which are actually in line of sight.
 *
 * And my favorite "plus" is that you can now use a special option to draw the
 * "floors" in the "viewable region" brightly (actually, to draw the *other*
 * grids dimly), providing a "pretty" effect as the player runs around, and
 * to efficiently display the "torch lite" in a special color.
 *
 *
 * Some comments on the "update_view()" algorithm...
 *
 * The algorithm is very fast, since it spreads "obvious" grids very quickly,
 * and only has to call "los()" on the borderline cases.  The major axes/diags
 * even terminate early when they hit walls.  I need to find a quick way
 * to "terminate" the other scans.
 *
 * Note that in the worst case (a big empty area with say 5% scattered walls),
 * each of the 1500 or so nearby grids is checked once, most of them getting
 * an "instant" rating, and only a small portion requiring a call to "los()".
 *
 * The only time that the algorithm appears to be "noticeably" too slow is
 * when running, and this is usually only important in town, since the town
 * provides about the worst scenario possible, with large open regions and
 * a few scattered obstructions.  There is a special "efficiency" option to
 * allow the player to reduce his field of view in town, if needed.
 *
 * In the "best" case (say, a normal stretch of corridor), the algorithm
 * makes one check for each viewable grid, and makes no calls to "los()".
 * So running in corridors is very fast, and if a lot of monsters are
 * nearby, it is much faster than the old methods.
 *
 * Note that resting, most normal commands, and several forms of running,
 * plus all commands executed near large groups of monsters, are strictly
 * more efficient with "update_view()" that with the old "compute los() on
 * demand" method, primarily because once the "field of view" has been
 * calculated, it does not have to be recalculated until the player moves
 * (or a wall or door is created or destroyed).
 *
 * Note that we no longer have to do as many "los()" checks, since once the
 * "view" region has been built, very few things cause it to be "changed"
 * (player movement, and the opening/closing of doors, changes in wall status).
 * Note that door/wall changes are only relevant when the door/wall itself is
 * in the "view" region.
 *
 * The algorithm seems to only call "los()" from zero to ten times, usually
 * only when coming down a corridor into a room, or standing in a room, just
 * misaligned with a corridor.  So if, say, there are five "nearby" monsters,
 * we will be reducing the calls to "los()".
 *
 * I am thinking in terms of an algorithm that "walks" from the central point
 * out to the maximal "distance", at each point, determining the "view" code
 * (above).  For each grid not on a major axis or diagonal, the "view" code
 * depends on the "cave_los_bold()" and "view" of exactly two other grids
 * (the one along the nearest diagonal, and the one next to that one, see
 * "update_view_aux()"...).
 *
 * We "memorize" the viewable space array, so that at the cost of under 3000
 * bytes, we reduce the time taken by "forget_view()" to one assignment for
 * each grid actually in the "viewable space".  And for another 3000 bytes,
 * we prevent "erase + redraw" ineffiencies via the "seen" set.  These bytes
 * are also used by other routines, thus reducing the cost to almost nothing.
 *
 * A similar thing is done for "forget_lite()" in which case the savings are
 * much less, but save us from doing bizarre maintenance checking.
 *
 * In the worst "normal" case (in the middle of the town), the reachable space
 * actually reaches to more than half of the largest possible "circle" of view,
 * or about 800 grids, and in the worse case (in the middle of a dungeon level
 * where all the walls have been removed), the reachable space actually reaches
 * the theoretical maximum size of just under 1500 grids.
 *
 * Each grid G examines the "state" of two (?) other (adjacent) grids, G1 & G2.
 * If G1 is lite, G is lite.  Else if G2 is lite, G is half.  Else if G1 and G2
 * are both half, G is half.  Else G is dark.  It only takes 2 (or 4) bits to
 * "name" a grid, so (for MAX_RAD of 20) we could use 1600 bytes, and scan the
 * entire possible space (including initialization) in one step per grid.  If
 * we do the "clearing" as a separate step (and use an array of "view" grids),
 * then the clearing will take as many steps as grids that were viewed, and the
 * algorithm will be able to "stop" scanning at various points.
 * Oh, and outside of the "torch radius", only "lite" grids need to be scanned.
 */








/*
 * Actually erase the entire "lite" array, redrawing every grid
 */
void forget_lite(void)
{
	int i, x, y;

	/* None to forget */
	if (!lite_n) return;

	/* Clear them all */
	for (i = 0; i < lite_n; i++)
	{
		y = lite_y[i];
		x = lite_x[i];

		/* Forget "LITE" flag */
		cave[y][x].info &= ~(CAVE_LITE);

		/* Redraw */
		/* lite_spot(y, x); Perhaps don't need? */
	}

	/* None left */
	lite_n = 0;
}


/*
 * For delayed visual update
 */
#define cave_note_and_redraw_later(C,Y,X) \
{\
	(C)->info |= CAVE_NOTE; \
	cave_redraw_later((C), (Y), (X)); \
}


/*
 * For delayed visual update
 */
#define cave_redraw_later(C,Y,X) \
{\
	if (!((C)->info & CAVE_REDRAW)) \
	{ \
		(C)->info |= CAVE_REDRAW; \
		redraw_y[redraw_n] = (Y); \
		redraw_x[redraw_n++] = (X); \
	} \
}


/*
 * XXX XXX XXX
 *
 * This macro allows us to efficiently add a grid to the "lite" array,
 * note that we are never called for illegal grids, or for grids which
 * have already been placed into the "lite" array, and we are never
 * called when the "lite" array is full.
 */
#define cave_lite_hack(Y,X) \
{\
	if (!(cave[Y][X].info & (CAVE_LITE))) \
	{ \
		cave[Y][X].info |= (CAVE_LITE); \
		lite_y[lite_n] = (Y); \
		lite_x[lite_n++] = (X); \
	} \
}


/*
 * Update the set of grids "illuminated" by the player's lite.
 *
 * This routine needs to use the results of "update_view()"
 *
 * Note that "blindness" does NOT affect "torch lite".  Be careful!
 *
 * We optimize most lites (all non-artifact lites) by using "obvious"
 * facts about the results of "small" lite radius, and we attempt to
 * list the "nearby" grids before the more "distant" ones in the
 * array of torch-lit grids.
 *
 * We assume that "radius zero" lite is in fact no lite at all.
 *
 *     Torch     Lantern     Artifacts
 *     (etc)
 *                              ***
 *                 ***         *****
 *      ***       *****       *******
 *      *@*       **@**       ***@***
 *      ***       *****       *******
 *                 ***         *****
 *                              ***
 */
/*:::�����ɂ��u�Ƃ炳��Ă���v�t���O����*/
void update_lite(void)
{
	int i, x, y, min_x, max_x, min_y, max_y;
	int p = p_ptr->cur_lite;
	cave_type *c_ptr;

	/*** Special case ***/

#if 0
	/* Hack -- Player has no lite */
	if (p <= 0)
	{
		/* Forget the old lite */
		/* forget_lite(); Perhaps don't need? */

		/* Add it to later visual update */
		cave_redraw_later(&cave[py][px], py, px);
	}
#endif

	//v1.1.66 ��������̃O���b�h���J�E���g����temp_n�����Z�b�g���Ă���
	temp_n = 0;

	/*** Save the old "lite" grids for later ***/
	/* Clear them all */
	for (i = 0; i < lite_n; i++)
	{
		y = lite_y[i];
		x = lite_x[i];

		/* Mark the grid as not "lite" */
		cave[y][x].info &= ~(CAVE_LITE);

		/* Mark the grid as "seen" */
		cave[y][x].info |= (CAVE_TEMP);

		/* Add it to the "seen" set */
		temp_y[temp_n] = y;
		temp_x[temp_n] = x;
		temp_n++;
	}

	/* None left */
	lite_n = 0;
//testmsg(format("py:%d px:%d",py,px));
//testmsg(format("wy:%d wx:%d",p_ptr->wilderness_y,p_ptr->wilderness_x));

	/*** Collect the new "lite" grids ***/

	/* Radius 1 -- torch radius */
	if (p >= 1)
	{
		/* Player grid */
		cave_lite_hack(py, px);

		/* Adjacent grid */
		cave_lite_hack(py+1, px);
		cave_lite_hack(py-1, px);
		cave_lite_hack(py, px+1);
		cave_lite_hack(py, px-1);

		/* Diagonal grids */
		cave_lite_hack(py+1, px+1);
		cave_lite_hack(py+1, px-1);
		cave_lite_hack(py-1, px+1);
		cave_lite_hack(py-1, px-1);
	}
	/* Radius 2 -- lantern radius */
	if (p >= 2)
	{
		/* South of the player */
		if (cave_los_bold(py + 1, px))
		{
			cave_lite_hack(py+2, px);
			cave_lite_hack(py+2, px+1);
			cave_lite_hack(py+2, px-1);
		}

		/* North of the player */
		if (cave_los_bold(py - 1, px))
		{
			cave_lite_hack(py-2, px);
			cave_lite_hack(py-2, px+1);
			cave_lite_hack(py-2, px-1);
		}

		/* East of the player */
		if (cave_los_bold(py, px + 1))
		{
			cave_lite_hack(py, px+2);
			cave_lite_hack(py+1, px+2);
			cave_lite_hack(py-1, px+2);
		}

		/* West of the player */
		if (cave_los_bold(py, px - 1))
		{
			cave_lite_hack(py, px-2);
			cave_lite_hack(py+1, px-2);
			cave_lite_hack(py-1, px-2);
		}
	}

	/* Radius 3+ -- artifact radius */
	if (p >= 3)
	{
		int d;

		/* Paranoia -- see "LITE_MAX" */
		if (p > 14) p = 14;

		/* South-East of the player */
		if (cave_los_bold(py + 1, px + 1))
		{
			cave_lite_hack(py+2, px+2);
		}

		/* South-West of the player */
		if (cave_los_bold(py + 1, px - 1))
		{
			cave_lite_hack(py+2, px-2);
		}

		/* North-East of the player */
		if (cave_los_bold(py - 1, px + 1))
		{
			cave_lite_hack(py-2, px+2);
		}

		/* North-West of the player */
		if (cave_los_bold(py - 1, px - 1))
		{
			cave_lite_hack(py-2, px-2);
		}

		/* Maximal north */
		min_y = py - p;
		if (min_y < 0) min_y = 0;

		/* Maximal south */
		max_y = py + p;
		if (max_y > cur_hgt-1) max_y = cur_hgt-1;

		/* Maximal west */
		min_x = px - p;
		if (min_x < 0) min_x = 0;

		/* Maximal east */
		max_x = px + p;
		if (max_x > cur_wid-1) max_x = cur_wid-1;

		/* Scan the maximal box */
		for (y = min_y; y <= max_y; y++)
		{
			for (x = min_x; x <= max_x; x++)
			{
				int dy = (py > y) ? (py - y) : (y - py);
				int dx = (px > x) ? (px - x) : (x - px);

				/* Skip the "central" grids (above) */
				if ((dy <= 2) && (dx <= 2)) continue;

				/* Hack -- approximate the distance */
				d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

				/* Skip distant grids */
				if (d > p) continue;

				/* Viewable, nearby, grids get "torch lit" */
				if (cave[y][x].info & CAVE_VIEW)
				{
					/* This grid is "torch lit" */
					cave_lite_hack(y, x);
				}
			}
		}
	}


	/*** Complete the algorithm ***/

	/* Draw the new grids */
	for (i = 0; i < lite_n; i++)
	{
		y = lite_y[i];
		x = lite_x[i];

		c_ptr = &cave[y][x];

		/* Update fresh grids */
		if (c_ptr->info & (CAVE_TEMP)) continue;

		/* Add it to later visual update */
		cave_note_and_redraw_later(c_ptr, y, x);
	}

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		y = temp_y[i];
		x = temp_x[i];

		c_ptr = &cave[y][x];

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Update stale grids */
		if (c_ptr->info & (CAVE_LITE)) continue;

		/* Add it to later visual update */
		cave_redraw_later(c_ptr, y, x);
	}

	/* None left */
	temp_n = 0;

	/* Mega-Hack -- Visual update later */
	p_ptr->update |= (PU_DELAY_VIS);
}


static bool mon_invis;
static s16b mon_fy, mon_fx;

/*
 * Add a square to the changes array
 */
static void mon_lite_hack(int y, int x)
{
	cave_type *c_ptr;
	int       midpoint, dpf, d;

	/* We trust this grid is in bounds */
	/* if (!in_bounds2(y, x)) return; */

	c_ptr = &cave[y][x];

	/* Want a unlit square in view of the player */
	if ((c_ptr->info & (CAVE_MNLT | CAVE_VIEW)) != CAVE_VIEW) return;

	if (!cave_los_grid(c_ptr))
	{
		/* Hack -- Prevent monster lite leakage in walls */

		/* Horizontal walls between player and a monster */
		if (((y < py) && (y > mon_fy)) || ((y > py) && (y < mon_fy)))
		{
			dpf = py - mon_fy;
			d = y - mon_fy;
			midpoint = mon_fx + ((px - mon_fx) * ABS(d)) / ABS(dpf);

			/* Only first wall viewed from mid-x is lit */
			if (x < midpoint)
			{
				if (!cave_los_bold(y, x + 1)) return;
			}
			else if (x > midpoint)
			{
				if (!cave_los_bold(y, x - 1)) return;
			}

			/* Hack XXX XXX - Is it a wall and monster not in LOS? */
			else if (mon_invis) return;
		}

		/* Vertical walls between player and a monster */
		if (((x < px) && (x > mon_fx)) || ((x > px) && (x < mon_fx)))
		{
			dpf = px - mon_fx;
			d = x - mon_fx;
			midpoint = mon_fy + ((py - mon_fy) * ABS(d)) / ABS(dpf);

			/* Only first wall viewed from mid-y is lit */
			if (y < midpoint)
			{
				if (!cave_los_bold(y + 1, x)) return;
			}
			else if (y > midpoint)
			{
				if (!cave_los_bold(y - 1, x)) return;
			}

			/* Hack XXX XXX - Is it a wall and monster not in LOS? */
			else if (mon_invis) return;
		}
	}

	/* We trust temp_n does not exceed TEMP_MAX */

	/* New grid */
	if (!(c_ptr->info & CAVE_MNDK))
	{
		/* Save this square */
		temp_x[temp_n] = x;
		temp_y[temp_n] = y;
		temp_n++;
	}

	/* Darkened grid */
	else
	{
		/* No longer dark */
		c_ptr->info &= ~(CAVE_MNDK);
	}

	/* Light it */
	c_ptr->info |= CAVE_MNLT;
}


/*
 * Add a square to the changes array
 */
static void mon_dark_hack(int y, int x)
{
	cave_type *c_ptr;
	int       midpoint, dpf, d;

	/* We trust this grid is in bounds */
	/* if (!in_bounds2(y, x)) return; */

	c_ptr = &cave[y][x];

	/* Want a unlit and undarkened square in view of the player */
	if ((c_ptr->info & (CAVE_LITE | CAVE_MNLT | CAVE_MNDK | CAVE_VIEW)) != CAVE_VIEW) return;

	if (!cave_los_grid(c_ptr) && !cave_have_flag_grid(c_ptr, FF_PROJECT))
	{
		/* Hack -- Prevent monster dark lite leakage in walls */

		/* Horizontal walls between player and a monster */
		if (((y < py) && (y > mon_fy)) || ((y > py) && (y < mon_fy)))
		{
			dpf = py - mon_fy;
			d = y - mon_fy;
			midpoint = mon_fx + ((px - mon_fx) * ABS(d)) / ABS(dpf);

			/* Only first wall viewed from mid-x is lit */
			if (x < midpoint)
			{
				if (!cave_los_bold(y, x + 1) && !cave_have_flag_bold(y, x + 1, FF_PROJECT)) return;
			}
			else if (x > midpoint)
			{
				if (!cave_los_bold(y, x - 1) && !cave_have_flag_bold(y, x - 1, FF_PROJECT)) return;
			}

			/* Hack XXX XXX - Is it a wall and monster not in LOS? */
			else if (mon_invis) return;
		}

		/* Vertical walls between player and a monster */
		if (((x < px) && (x > mon_fx)) || ((x > px) && (x < mon_fx)))
		{
			dpf = px - mon_fx;
			d = x - mon_fx;
			midpoint = mon_fy + ((py - mon_fy) * ABS(d)) / ABS(dpf);

			/* Only first wall viewed from mid-y is lit */
			if (y < midpoint)
			{
				if (!cave_los_bold(y + 1, x) && !cave_have_flag_bold(y + 1, x, FF_PROJECT)) return;
			}
			else if (y > midpoint)
			{
				if (!cave_los_bold(y - 1, x) && !cave_have_flag_bold(y - 1, x, FF_PROJECT)) return;
			}

			/* Hack XXX XXX - Is it a wall and monster not in LOS? */
			else if (mon_invis) return;
		}
	}

	/* We trust temp_n does not exceed TEMP_MAX */

	/* Save this square */
	temp_x[temp_n] = x;
	temp_y[temp_n] = y;
	temp_n++;

	/* Darken it */
	c_ptr->info |= CAVE_MNDK;
}


/*
 * Update squares illuminated or darkened by monsters.
 *
 * Hack - use the CAVE_ROOM flag (renamed to be CAVE_MNLT) to
 * denote squares illuminated by monsters.
 *
 * The CAVE_TEMP and CAVE_XTRA flag are used to store the state during the
 * updating.  Only squares in view of the player, whos state
 * changes are drawn via lite_spot().
 */
/*:::�����X�^�[�ɂ������������v�Z*/
void update_mon_lite(void)
{
	int i, rad;
	cave_type *c_ptr;

	s16b fx, fy;
	/*:::mon_lite_hack()��mon_dark_hack()�ɂȂ�*/
	void (*add_mon_lite)(int, int);
	int f_flag;

	s16b end_temp;

	/* Non-Ninja player in the darkness */
	int dis_lim = ((d_info[dungeon_type].flags1 & DF1_DARKNESS) && !p_ptr->see_nocto) ?
		(MAX_SIGHT / 2 + 1) : (MAX_SIGHT + 3);

	/* Clear all monster lit squares */
	for (i = 0; i < mon_lite_n; i++)
	{
		/* Point to grid */
		c_ptr = &cave[mon_lite_y[i]][mon_lite_x[i]];

		/* Set temp or xtra flag */
		c_ptr->info |= (c_ptr->info & CAVE_MNLT) ? CAVE_TEMP : CAVE_XTRA;

		/* Clear monster illumination flag */
		c_ptr->info &= ~(CAVE_MNLT | CAVE_MNDK);
	}

	/* Empty temp list of new squares to lite up */
	temp_n = 0;

	/* If a monster stops time, don't process */
	if (!world_monster)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;

		/* Loop through monsters, adding newly lit squares to changes list */
		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];
			r_ptr = &r_info[m_ptr->r_idx];

			/* Skip dead monsters */
			if (!m_ptr->r_idx) continue;

			/* Is it too far away? */
			if (m_ptr->cdis > dis_lim) continue;

			/* Get lite radius */
			rad = 0;

			/* Note the radii are cumulative */
			if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_SELF_LITE_1)) rad++;
			if (r_ptr->flags7 & (RF7_HAS_LITE_2 | RF7_SELF_LITE_2)) rad += 2;
			if (r_ptr->flags7 & (RF7_HAS_DARK_1 | RF7_SELF_DARK_1)) rad--;
			if (r_ptr->flags7 & (RF7_HAS_DARK_2 | RF7_SELF_DARK_2)) rad -= 2;

			/* Exit if has no light */
			if (!rad) continue;
			else if (rad > 0)
			{
				if (!(r_ptr->flags7 & (RF7_SELF_LITE_1 | RF7_SELF_LITE_2)) && (MON_CSLEEP(m_ptr) || (!dun_level && is_daytime()) || p_ptr->inside_battle)) continue;
				if (d_info[dungeon_type].flags1 & DF1_DARKNESS) rad = 1;
				add_mon_lite = mon_lite_hack;
				f_flag = FF_LOS;
			}
			else
			{
				if (!(r_ptr->flags7 & (RF7_SELF_DARK_1 | RF7_SELF_DARK_2)) && (MON_CSLEEP(m_ptr) || (!dun_level && !is_daytime()))) continue;
				add_mon_lite = mon_dark_hack;
				f_flag = FF_PROJECT;
				rad = -rad; /* Use absolute value */
			}

			/* Access the location */
			mon_fx = m_ptr->fx;
			mon_fy = m_ptr->fy;

			/* Is the monster visible? */
			mon_invis = !(cave[mon_fy][mon_fx].info & CAVE_VIEW);

			/* The square it is on */
			add_mon_lite(mon_fy, mon_fx);

			/* Adjacent squares */
			add_mon_lite(mon_fy + 1, mon_fx);
			add_mon_lite(mon_fy - 1, mon_fx);
			add_mon_lite(mon_fy, mon_fx + 1);
			add_mon_lite(mon_fy, mon_fx - 1);
			add_mon_lite(mon_fy + 1, mon_fx + 1);
			add_mon_lite(mon_fy + 1, mon_fx - 1);
			add_mon_lite(mon_fy - 1, mon_fx + 1);
			add_mon_lite(mon_fy - 1, mon_fx - 1);

			/* Radius 2 */
			if (rad >= 2)
			{
				/* South of the monster */
				if (cave_have_flag_bold(mon_fy + 1, mon_fx, f_flag))
				{
					add_mon_lite(mon_fy + 2, mon_fx + 1);
					add_mon_lite(mon_fy + 2, mon_fx);
					add_mon_lite(mon_fy + 2, mon_fx - 1);

					c_ptr = &cave[mon_fy + 2][mon_fx];

					/* Radius 3 */
					if ((rad == 3) && cave_have_flag_grid(c_ptr, f_flag))
					{
						add_mon_lite(mon_fy + 3, mon_fx + 1);
						add_mon_lite(mon_fy + 3, mon_fx);
						add_mon_lite(mon_fy + 3, mon_fx - 1);
					}
				}

				/* North of the monster */
				if (cave_have_flag_bold(mon_fy - 1, mon_fx, f_flag))
				{
					add_mon_lite(mon_fy - 2, mon_fx + 1);
					add_mon_lite(mon_fy - 2, mon_fx);
					add_mon_lite(mon_fy - 2, mon_fx - 1);

					c_ptr = &cave[mon_fy - 2][mon_fx];

					/* Radius 3 */
					if ((rad == 3) && cave_have_flag_grid(c_ptr, f_flag))
					{
						add_mon_lite(mon_fy - 3, mon_fx + 1);
						add_mon_lite(mon_fy - 3, mon_fx);
						add_mon_lite(mon_fy - 3, mon_fx - 1);
					}
				}

				/* East of the monster */
				if (cave_have_flag_bold(mon_fy, mon_fx + 1, f_flag))
				{
					add_mon_lite(mon_fy + 1, mon_fx + 2);
					add_mon_lite(mon_fy, mon_fx + 2);
					add_mon_lite(mon_fy - 1, mon_fx + 2);

					c_ptr = &cave[mon_fy][mon_fx + 2];

					/* Radius 3 */
					if ((rad == 3) && cave_have_flag_grid(c_ptr, f_flag))
					{
						add_mon_lite(mon_fy + 1, mon_fx + 3);
						add_mon_lite(mon_fy, mon_fx + 3);
						add_mon_lite(mon_fy - 1, mon_fx + 3);
					}
				}

				/* West of the monster */
				if (cave_have_flag_bold(mon_fy, mon_fx - 1, f_flag))
				{
					add_mon_lite(mon_fy + 1, mon_fx - 2);
					add_mon_lite(mon_fy, mon_fx - 2);
					add_mon_lite(mon_fy - 1, mon_fx - 2);

					c_ptr = &cave[mon_fy][mon_fx - 2];

					/* Radius 3 */
					if ((rad == 3) && cave_have_flag_grid(c_ptr, f_flag))
					{
						add_mon_lite(mon_fy + 1, mon_fx - 3);
						add_mon_lite(mon_fy, mon_fx - 3);
						add_mon_lite(mon_fy - 1, mon_fx - 3);
					}
				}
			}

			/* Radius 3 */
			if (rad == 3)
			{
				/* South-East of the monster */
				if (cave_have_flag_bold(mon_fy + 1, mon_fx + 1, f_flag))
				{
					add_mon_lite(mon_fy + 2, mon_fx + 2);
				}

				/* South-West of the monster */
				if (cave_have_flag_bold(mon_fy + 1, mon_fx - 1, f_flag))
				{
					add_mon_lite(mon_fy + 2, mon_fx - 2);
				}

				/* North-East of the monster */
				if (cave_have_flag_bold(mon_fy - 1, mon_fx + 1, f_flag))
				{
					add_mon_lite(mon_fy - 2, mon_fx + 2);
				}

				/* North-West of the monster */
				if (cave_have_flag_bold(mon_fy - 1, mon_fx - 1, f_flag))
				{
					add_mon_lite(mon_fy - 2, mon_fx - 2);
				}
			}
		}
	}

	/* Save end of list of new squares */
	end_temp = temp_n;

	/*
	 * Look at old set flags to see if there are any changes.
	 */
	for (i = 0; i < mon_lite_n; i++)
	{
		fx = mon_lite_x[i];
		fy = mon_lite_y[i];

		/* We trust this grid is in bounds */

		/* Point to grid */
		c_ptr = &cave[fy][fx];

		if (c_ptr->info & CAVE_TEMP) /* Pervious lit */
		{
			/* It it no longer lit? */
			if ((c_ptr->info & (CAVE_VIEW | CAVE_MNLT)) == CAVE_VIEW)
			{
				/* It is now unlit */
				/* Add it to later visual update */
				cave_note_and_redraw_later(c_ptr, fy, fx);
			}
		}
		else /* Pervious darkened */
		{
			/* It it no longer darken? */
			if ((c_ptr->info & (CAVE_VIEW | CAVE_MNDK)) == CAVE_VIEW)
			{
				/* It is now undarken */
				/* Add it to later visual update */
				cave_note_and_redraw_later(c_ptr, fy, fx);
			}
		}

		/* Add to end of temp array */
		temp_x[temp_n] = (byte)fx;
		temp_y[temp_n] = (byte)fy;
		temp_n++;
	}

	/* Clear the lite array */
	mon_lite_n = 0;

	/* Copy the temp array into the lit array lighting the new squares. */
	for (i = 0; i < end_temp; i++)
	{
		fx = temp_x[i];
		fy = temp_y[i];

		/* We trust this grid is in bounds */

		/* Point to grid */
		c_ptr = &cave[fy][fx];

		if (c_ptr->info & CAVE_MNLT) /* Lit */
		{
			/* The is the square newly lit and visible? */
			if ((c_ptr->info & (CAVE_VIEW | CAVE_TEMP)) == CAVE_VIEW)
			{
				/* It is now lit */
				/* Add it to later visual update */
				cave_note_and_redraw_later(c_ptr, fy, fx);
			}
		}
		else /* Darkened */
		{
			/* The is the square newly darkened and visible? */
			if ((c_ptr->info & (CAVE_VIEW | CAVE_XTRA)) == CAVE_VIEW)
			{
				/* It is now darkened */
				/* Add it to later visual update */
				cave_note_and_redraw_later(c_ptr, fy, fx);
			}
		}

		/* Save in the monster lit or darkened array */
		mon_lite_x[mon_lite_n] = fx;
		mon_lite_y[mon_lite_n] = fy;
		mon_lite_n++;
	}

	/* Clear the temp flag for the old lit or darken grids */
	for (i = end_temp; i < temp_n; i++)
	{
		/* We trust this grid is in bounds */

		cave[temp_y[i]][temp_x[i]].info &= ~(CAVE_TEMP | CAVE_XTRA);
	}

	/* Finished with temp_n */
	temp_n = 0;

	/* Mega-Hack -- Visual update later */
	p_ptr->update |= (PU_DELAY_VIS);

	p_ptr->monlite = (cave[py][px].info & CAVE_MNLT) ? TRUE : FALSE;

	///class �����X�^�[�����ƔE�҃X�e���X�̃��b�Z�[�W
	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		if (p_ptr->old_monlite != p_ptr->monlite)
		{
			if (p_ptr->monlite)
			{
#ifdef JP
				msg_print("�e�̕��������ꂽ�C������B");
#else
				msg_print("Your mantle of shadow become thin.");
#endif
			}
			else
			{
#ifdef JP
				msg_print("�e�̕������Z���Ȃ����I");
#else
				msg_print("Your mantle of shadow restored its original darkness.");
#endif
			}
		}
	}
	p_ptr->old_monlite = p_ptr->monlite;
}

void clear_mon_lite(void)
{
	int i;
	cave_type *c_ptr;

	/* Clear all monster lit squares */
	for (i = 0; i < mon_lite_n; i++)
	{
		/* Point to grid */
		c_ptr = &cave[mon_lite_y[i]][mon_lite_x[i]];

		/* Clear monster illumination flag */
		c_ptr->info &= ~(CAVE_MNLT | CAVE_MNDK);
	}

	/* Empty the array */
	mon_lite_n = 0;
}



/*
 * Clear the viewable space
 */
void forget_view(void)
{
	int i;

	cave_type *c_ptr;

	/* None to forget */
	if (!view_n) return;

	/* Clear them all */
	for (i = 0; i < view_n; i++)
	{
		int y = view_y[i];
		int x = view_x[i];

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* Forget that the grid is viewable */
		c_ptr->info &= ~(CAVE_VIEW);

		/* if (!panel_contains(y, x)) continue; */

		/* Update the screen */
		/* lite_spot(y, x); Perhaps don't need? */
	}

	/* None left */
	view_n = 0;
}



/*
 * This macro allows us to efficiently add a grid to the "view" array,
 * note that we are never called for illegal grids, or for grids which
 * have already been placed into the "view" array, and we are never
 * called when the "view" array is full.
 */
#define cave_view_hack(C,Y,X) \
{\
    if (!((C)->info & (CAVE_VIEW))){\
    (C)->info |= (CAVE_VIEW); \
    view_y[view_n] = (Y); \
    view_x[view_n] = (X); \
    view_n++;}\
}



/*
 * Helper function for "update_view()" below
 *
 * We are checking the "viewability" of grid (y,x) by the player.
 *
 * This function assumes that (y,x) is legal (i.e. on the map).
 *
 * Grid (y1,x1) is on the "diagonal" between (py,px) and (y,x)
 * Grid (y2,x2) is "adjacent", also between (py,px) and (y,x).
 *
 * Note that we are using the "CAVE_XTRA" field for marking grids as
 * "easily viewable".  This bit is cleared at the end of "update_view()".
 *
 * This function adds (y,x) to the "viewable set" if necessary.
 *
 * This function now returns "TRUE" if vision is "blocked" by grid (y,x).
 */
static bool update_view_aux(int y, int x, int y1, int x1, int y2, int x2)
{
	bool f1, f2, v1, v2, z1, z2, wall;

	cave_type *c_ptr;

	cave_type *g1_c_ptr;
	cave_type *g2_c_ptr;

	/* Access the grids */
	g1_c_ptr = &cave[y1][x1];
	g2_c_ptr = &cave[y2][x2];


	/* Check for walls */
	f1 = (cave_los_grid(g1_c_ptr));
	f2 = (cave_los_grid(g2_c_ptr));

	/* Totally blocked by physical walls */
	if (!f1 && !f2) return (TRUE);


	/* Check for visibility */
	v1 = (f1 && (g1_c_ptr->info & (CAVE_VIEW)));
	v2 = (f2 && (g2_c_ptr->info & (CAVE_VIEW)));

	/* Totally blocked by "unviewable neighbors" */
	if (!v1 && !v2) return (TRUE);


	/* Access the grid */
	c_ptr = &cave[y][x];


	/* Check for walls */
	wall = (!cave_los_grid(c_ptr));


	/* Check the "ease" of visibility */
	z1 = (v1 && (g1_c_ptr->info & (CAVE_XTRA)));
	z2 = (v2 && (g2_c_ptr->info & (CAVE_XTRA)));

	/* Hack -- "easy" plus "easy" yields "easy" */
	if (z1 && z2)
	{
		c_ptr->info |= (CAVE_XTRA);

		cave_view_hack(c_ptr, y, x);

		return (wall);
	}

	/* Hack -- primary "easy" yields "viewed" */
	if (z1)
	{
		cave_view_hack(c_ptr, y, x);

		return (wall);
	}

	/* Hack -- "view" plus "view" yields "view" */
	if (v1 && v2)
	{
		/* c_ptr->info |= (CAVE_XTRA); */

		cave_view_hack(c_ptr, y, x);

		return (wall);
	}


	/* Mega-Hack -- the "los()" function works poorly on walls */
	if (wall)
	{
		cave_view_hack(c_ptr, y, x);

		return (wall);
	}


	/* Hack -- check line of sight */
	if (los(py, px, y, x))
	{
		cave_view_hack(c_ptr, y, x);

		return (wall);
	}


	/* Assume no line of sight. */
	return (TRUE);
}



/*
 * Calculate the viewable space
 *
 *  1: Process the player
 *  1a: The player is always (easily) viewable
 *  2: Process the diagonals
 *  2a: The diagonals are (easily) viewable up to the first wall
 *  2b: But never go more than 2/3 of the "full" distance
 *  3: Process the main axes
 *  3a: The main axes are (easily) viewable up to the first wall
 *  3b: But never go more than the "full" distance
 *  4: Process sequential "strips" in each of the eight octants
 *  4a: Each strip runs along the previous strip
 *  4b: The main axes are "previous" to the first strip
 *  4c: Process both "sides" of each "direction" of each strip
 *  4c1: Each side aborts as soon as possible
 *  4c2: Each side tells the next strip how far it has to check
 *
 * Note that the octant processing involves some pretty interesting
 * observations involving when a grid might possibly be viewable from
 * a given grid, and on the order in which the strips are processed.
 *
 * Note the use of the mathematical facts shown below, which derive
 * from the fact that (1 < sqrt(2) < 1.5), and that the length of the
 * hypotenuse of a right triangle is primarily determined by the length
 * of the longest side, when one side is small, and is strictly less
 * than one-and-a-half times as long as the longest side when both of
 * the sides are large.
 *
 *   if (manhatten(dy,dx) < R) then (hypot(dy,dx) < R)
 *   if (manhatten(dy,dx) > R*3/2) then (hypot(dy,dx) > R)
 *
 *   hypot(dy,dx) is approximated by (dx+dy+MAX(dx,dy)) / 2
 *
 * These observations are important because the calculation of the actual
 * value of "hypot(dx,dy)" is extremely expensive, involving square roots,
 * while for small values (up to about 20 or so), the approximations above
 * are correct to within an error of at most one grid or so.
 *
 * Observe the use of "full" and "over" in the code below, and the use of
 * the specialized calculation involving "limit", all of which derive from
 * the observations given above.  Basically, we note that the "circle" of
 * view is completely contained in an "octagon" whose bounds are easy to
 * determine, and that only a few steps are needed to derive the actual
 * bounds of the circle given the bounds of the octagon.
 *
 * Note that by skipping all the grids in the corners of the octagon, we
 * place an upper limit on the number of grids in the field of view, given
 * that "full" is never more than 20.  Of the 1681 grids in the "square" of
 * view, only about 1475 of these are in the "octagon" of view, and even
 * fewer are in the "circle" of view, so 1500 or 1536 is more than enough
 * entries to completely contain the actual field of view.
 *
 * Note also the care taken to prevent "running off the map".  The use of
 * explicit checks on the "validity" of the "diagonal", and the fact that
 * the loops are never allowed to "leave" the map, lets "update_view_aux()"
 * use the optimized "cave_los_bold()" macro, and to avoid the overhead
 * of multiple checks on the validity of grids.
 *
 * Note the "optimizations" involving the "se","sw","ne","nw","es","en",
 * "ws","wn" variables.  They work like this: While travelling down the
 * south-bound strip just to the east of the main south axis, as soon as
 * we get to a grid which does not "transmit" viewing, if all of the strips
 * preceding us (in this case, just the main axis) had terminated at or before
 * the same point, then we can stop, and reset the "max distance" to ourself.
 * So, each strip (named by major axis plus offset, thus "se" in this case)
 * maintains a "blockage" variable, initialized during the main axis step,
 * and checks it whenever a blockage is observed.  After processing each
 * strip as far as the previous strip told us to process, the next strip is
 * told not to go farther than the current strip's farthest viewable grid,
 * unless open space is still available.  This uses the "k" variable.
 *
 * Note the use of "inline" macros for efficiency.  The "cave_los_grid()"
 * macro is a replacement for "cave_los_bold()" which takes a pointer to
 * a cave grid instead of its location.  The "cave_view_hack()" macro is a
 * chunk of code which adds the given location to the "view" array if it
 * is not already there, using both the actual location and a pointer to
 * the cave grid.  See above.
 *
 * By the way, the purpose of this code is to reduce the dependancy on the
 * "los()" function which is slow, and, in some cases, not very accurate.
 *
 * It is very possible that I am the only person who fully understands this
 * function, and for that I am truly sorry, but efficiency was very important
 * and the "simple" version of this function was just not fast enough.  I am
 * more than willing to replace this function with a simpler one, if it is
 * equally efficient, and especially willing if the new function happens to
 * derive "reverse-line-of-sight" at the same time, since currently monsters
 * just use an optimized hack of "you see me, so I see you", and then use the
 * actual "projectable()" function to check spell attacks.
 */
void update_view(void)
{
	int n, m, d, k, y, x, z;

	int se, sw, ne, nw, es, en, ws, wn;

	int full, over;

	int y_max = cur_hgt - 1;
	int x_max = cur_wid - 1;

	cave_type *c_ptr;

	//v1.1.66 ��������̃O���b�h���J�E���g����temp_n,temp_x[],temp_y[]�����Z�b�g���Ă���
	temp_n = 0;


	/*** Initialize ***/
	/* Optimize */
	if (view_reduce_view && !dun_level)
	{
		/* Full radius (10) */
		full = MAX_SIGHT / 2;

		/* Octagon factor (15) */
		over = MAX_SIGHT * 3 / 4;
	}

	/* Normal */
	else
	{
		/* Full radius (20) */
		full = MAX_SIGHT;

		/* Octagon factor (30) */
		over = MAX_SIGHT * 3 / 2;
	}

	/*** Step 0 -- Begin ***/
	/* Save the old "view" grids for later */
	for (n = 0; n < view_n; n++)
	{
		y = view_y[n];
		x = view_x[n];

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* Mark the grid as not in "view" */
		c_ptr->info &= ~(CAVE_VIEW);

		/* Mark the grid as "seen" */
		c_ptr->info |= (CAVE_TEMP);

		/* Add it to the "seen" set */
		temp_y[temp_n] = y;
		temp_x[temp_n] = x;
		temp_n++;
	}

	/* Start over with the "view" array */
	view_n = 0;

	/*** Step 1 -- adjacent grids ***/

	/* Now start on the player */
	y = py;
	x = px;

	/* Access the grid */
	c_ptr = &cave[y][x];

	/* Assume the player grid is easily viewable */
	c_ptr->info |= (CAVE_XTRA);

	/* Assume the player grid is viewable */
	cave_view_hack(c_ptr, y, x);

	/*** Step 2 -- Major Diagonals ***/

	/* Hack -- Limit */
	z = full * 2 / 3;

	/* Scan south-east */
	for (d = 1; d <= z; d++)
	{
		c_ptr = &cave[y+d][x+d];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y+d, x+d);
		if (!cave_los_grid(c_ptr)) break;
	}

	/* Scan south-west */
	for (d = 1; d <= z; d++)
	{
		c_ptr = &cave[y+d][x-d];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y+d, x-d);
		if (!cave_los_grid(c_ptr)) break;
	}

	/* Scan north-east */
	for (d = 1; d <= z; d++)
	{
		c_ptr = &cave[y-d][x+d];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y-d, x+d);
		if (!cave_los_grid(c_ptr)) break;
	}

	/* Scan north-west */
	for (d = 1; d <= z; d++)
	{
		c_ptr = &cave[y-d][x-d];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y-d, x-d);
		if (!cave_los_grid(c_ptr)) break;
	}

	/*** Step 3 -- major axes ***/

	/* Scan south */
	for (d = 1; d <= full; d++)
	{
		c_ptr = &cave[y+d][x];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y+d, x);
		if (!cave_los_grid(c_ptr)) break;
	}

	/* Initialize the "south strips" */
	se = sw = d;

	/* Scan north */
	for (d = 1; d <= full; d++)
	{
		c_ptr = &cave[y-d][x];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y-d, x);
		if (!cave_los_grid(c_ptr)) break;
	}

	/* Initialize the "north strips" */
	ne = nw = d;

	/* Scan east */
	for (d = 1; d <= full; d++)
	{
		c_ptr = &cave[y][x+d];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y, x+d);
		if (!cave_los_grid(c_ptr)) break;
	}

	/* Initialize the "east strips" */
	es = en = d;

	/* Scan west */
	for (d = 1; d <= full; d++)
	{
		c_ptr = &cave[y][x-d];
		c_ptr->info |= (CAVE_XTRA);
		cave_view_hack(c_ptr, y, x-d);
		if (!cave_los_grid(c_ptr)) break;
	}

	/* Initialize the "west strips" */
	ws = wn = d;


	/*** Step 4 -- Divide each "octant" into "strips" ***/

	/* Now check each "diagonal" (in parallel) */
	for (n = 1; n <= over / 2; n++)
	{
		int ypn, ymn, xpn, xmn;


		/* Acquire the "bounds" of the maximal circle */
		z = over - n - n;
		if (z > full - n) z = full - n;
		while ((z + n + (n>>1)) > full) z--;


		/* Access the four diagonal grids */
		ypn = y + n;
		ymn = y - n;
		xpn = x + n;
		xmn = x - n;


		/* South strip */
		if (ypn < y_max)
		{
			/* Maximum distance */
			m = MIN(z, y_max - ypn);

			/* East side */
			if ((xpn <= x_max) && (n < se))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypn+d, xpn, ypn+d-1, xpn-1, ypn+d-1, xpn))
					{
						if (n + d >= se) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				se = k + 1;
			}

			/* West side */
			if ((xmn >= 0) && (n < sw))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypn+d, xmn, ypn+d-1, xmn+1, ypn+d-1, xmn))
					{
						if (n + d >= sw) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				sw = k + 1;
			}
		}


		/* North strip */
		if (ymn > 0)
		{
			/* Maximum distance */
			m = MIN(z, ymn);

			/* East side */
			if ((xpn <= x_max) && (n < ne))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymn-d, xpn, ymn-d+1, xpn-1, ymn-d+1, xpn))
					{
						if (n + d >= ne) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				ne = k + 1;
			}

			/* West side */
			if ((xmn >= 0) && (n < nw))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymn-d, xmn, ymn-d+1, xmn+1, ymn-d+1, xmn))
					{
						if (n + d >= nw) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				nw = k + 1;
			}
		}


		/* East strip */
		if (xpn < x_max)
		{
			/* Maximum distance */
			m = MIN(z, x_max - xpn);

			/* South side */
			if ((ypn <= x_max) && (n < es))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypn, xpn+d, ypn-1, xpn+d-1, ypn, xpn+d-1))
					{
						if (n + d >= es) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				es = k + 1;
			}

			/* North side */
			if ((ymn >= 0) && (n < en))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymn, xpn+d, ymn+1, xpn+d-1, ymn, xpn+d-1))
					{
						if (n + d >= en) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				en = k + 1;
			}
		}


		/* West strip */
		if (xmn > 0)
		{
			/* Maximum distance */
			m = MIN(z, xmn);

			/* South side */
			if ((ypn <= y_max) && (n < ws))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypn, xmn-d, ypn-1, xmn-d+1, ypn, xmn-d+1))
					{
						if (n + d >= ws) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				ws = k + 1;
			}

			/* North side */
			if ((ymn >= 0) && (n < wn))
			{
				/* Scan */
				for (k = n, d = 1; d <= m; d++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymn, xmn-d, ymn+1, xmn-d+1, ymn, xmn-d+1))
					{
						if (n + d >= wn) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				wn = k + 1;
			}
		}
	}


	/*** Step 5 -- Complete the algorithm ***/

	/* Update all the new grids */
	for (n = 0; n < view_n; n++)
	{
		y = view_y[n];
		x = view_x[n];

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* Clear the "CAVE_XTRA" flag */
		c_ptr->info &= ~(CAVE_XTRA);

		//v1.1.84 ���Ƃ��㩂������u�Ԕ����ł��邱�Ƃɂ���
		if (!character_xtra && p_ptr->pclass == CLASS_SATORI && p_ptr->lev > 14 && (PLAYER_CAN_USE_EYES) && c_ptr->mimic && cave_have_flag_bold((y), (x), FF_TRAP))
		{
			disclose_grid(y, x);
			msg_print("�g���b�v�𔭌������B");
			disturb(0, 1);
		}

		/* Update only newly viewed grids */
		if (c_ptr->info & (CAVE_TEMP)) continue;

		/* Add it to later visual update */
		cave_note_and_redraw_later(c_ptr, y, x);
	}

	/* Wipe the old grids, update as needed */
	for (n = 0; n < temp_n; n++)
	{
		y = temp_y[n];
		x = temp_x[n];

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Update only non-viewable grids */
		if (c_ptr->info & (CAVE_VIEW)) continue;

		/* Add it to later visual update */
		cave_redraw_later(c_ptr, y, x);
	}

	/* None left */
	temp_n = 0;

	/* Mega-Hack -- Visual update later */
	p_ptr->update |= (PU_DELAY_VIS);
}


/*
 * Mega-Hack -- Delayed visual update
 * Only used if update_view(), update_lite() or update_mon_lite() was called
 */
/*:::mon�̌����ړ��Ɋ�����`��H*/
void delayed_visual_update(void)
{
	int       i, y, x;
	cave_type *c_ptr;

	/* Update needed grids */
	for (i = 0; i < redraw_n; i++)
	{
		y = redraw_y[i];
		x = redraw_x[i];

		/* Access the grid */
		c_ptr = &cave[y][x];

		/* Update only needed grids (prevent multiple updating) */
		if (!(c_ptr->info & CAVE_REDRAW)) continue;

		/* If required, note */
		if (c_ptr->info & CAVE_NOTE) note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);

		/* Hack -- Visual update of monster on this grid */
		if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_NOTE | CAVE_REDRAW);
	}

	/* None left */
	redraw_n = 0;
}


/*
 * Hack -- forget the "flow" information
 */
void forget_flow(void)
{
	int x, y;

	/* Check the entire dungeon */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Forget the old data */
			cave[y][x].dist = 0;
			cave[y][x].cost = 0;
			cave[y][x].when = 0;
			//v1.1.24
			cave[y][x].flying_dist = 0;
		}
	}
}


/*
 * Hack - speed up the update_flow algorithm by only doing
 * it everytime the player moves out of LOS of the last
 * "way-point".
 */
static u16b flow_x = 0;
static u16b flow_y = 0;



//v1.1.24 update_flow()������
/*:::cave[][].flying_dist���v�Z����B
 * �e�O���b�h�ɂ��ā��̂���n�_����̕��V��Ԃł̈ړ����������߂�B��s�����X�^�[������ǐՂ���̂Ɏg���B
 * �]���A�����X�^�[�́u������ː����ʂ��Ă��炸�����ʂ������Ƃ̂Ȃ�(when�l�̂Ȃ�)�Â����o�R�̒ʘH���z���ā���ǂ����Ƃ͂Ȃ��v�d�l�B
 * �{�Ƃł͗��A�̂��Ȃ���肳�ꂽ�n�`�ƃ����X�^�[�z�u�łȂ��ƈ��p�ł��Ȃ��̂ő債�����ɂ͂Ȃ�Ȃ������悤�����A
 * ����łł͖��@�ňÂ����n�`�𐶐��ł���̂ő΍􂷂邱�Ƃɂ����B
 * �����������Ԃ������邪��قǌÂ��}�V���łȂ�������Ȃ����낤�B
 */
void update_flying_dist(void)
{
	int x, y, d;
	int flow_head = 1;
	int flow_tail = 0;

	/* Paranoia -- make sure the array is empty */
	//if (temp_n) return;

	//�_�b�V�����͍Čv�Z���Ȃ��H
	/* The last way-point is on the map */
	if (running && in_bounds(flow_y, flow_x))
	{
		/* The way point is in sight - do not update.  (Speedup) */
		if (cave[flow_y][flow_x].info & CAVE_VIEW) return;
	}

	/* Erase all of the current flow information */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave[y][x].flying_dist = 0;
		}
	}

	/* Save player position */
	flow_y = py;
	flow_x = px;

	/* Add the player's grid to the queue */
	temp_y[0] = py;
	temp_x[0] = px;

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		int ty, tx;

		/* Extract the next entry */
		ty = temp_y[flow_tail];
		tx = temp_x[flow_tail];

		/* Forget that entry */
		if (++flow_tail == TEMP_MAX) flow_tail = 0;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			int old_head = flow_head;
			int n = cave[ty][tx].flying_dist + 1;
			cave_type *c_ptr;

			/* Child location */
			y = ty + ddy_ddd[d];
			x = tx + ddx_ddd[d];

			/* Ignore player's grid */
			if (player_bold(y, x)) continue;

			c_ptr = &cave[y][x];

			/* Ignore "pre-stamped" entries */
			if (c_ptr->flying_dist != 0 && c_ptr->flying_dist <= n) continue;


			//v1.1.43 �T�j�[���Z�p����t���O�����B�����h�A��J�[�e����ʂ�Ȃ��B�K���X�͒ʂ�B
			if (flag_sunny_refraction)
			{
				if(!cave_have_flag_grid(c_ptr, FF_LOS))		continue;
			}
			else
			{

				/*::: �ǂ��Ȃǈړ��s�n�`�������B��������s�N���\�ȁu�Â����v�u�R��(�n���)�v�n�`��OK�Ƃ���B*/
				if (!cave_have_flag_grid(c_ptr, FF_MOVE) && !is_closed_door(c_ptr->feat) && !cave_have_flag_grid(c_ptr, FF_CAN_FLY)) continue;

			}


			if (c_ptr->flying_dist == 0 || c_ptr->flying_dist > n) c_ptr->flying_dist = n;

			/* Hack -- limit flow depth */
			if (n == MONSTER_FLOW_DEPTH) continue;

			/* Enqueue that entry */
			temp_y[flow_head] = y;
			temp_x[flow_head] = x;

			/* Advance the queue */
			if (++flow_head == TEMP_MAX) flow_head = 0;

			/* Hack -- notice overflow by forgetting new entry */
			if (flow_head == flow_tail) flow_head = old_head;
		}
	}
}

/*
 * Hack -- fill in the "cost" field of every grid that the player
 * can "reach" with the number of steps needed to reach that grid.
 * This also yields the "distance" of the player from every grid.
 *
 * In addition, mark the "when" of the grids that can reach
 * the player with the incremented value of "flow_n".
 *
 * Hack -- use the "seen" array as a "circular queue".
 *
 * We do not need a priority queue because the cost from grid
 * to grid is always "one" and we process them in order.
 */
/*:::cave[][].cost���v�Z����B�������ꂼ��̃}�X�ɓ��B����̂Ɋ|�����Ԃ̊T�Z�l�E�E�H*/
void update_flow(void)
{
	int x, y, d;
	int flow_head = 1;
	int flow_tail = 0;

	/* Paranoia -- make sure the array is empty */
	if (temp_n) return;

	/* The last way-point is on the map */
	if (running && in_bounds(flow_y, flow_x))
	{
		/* The way point is in sight - do not update.  (Speedup) */
		if (cave[flow_y][flow_x].info & CAVE_VIEW) return;
	}

	/* Erase all of the current flow information */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave[y][x].cost = 0;
			cave[y][x].dist = 0;
		}
	}

	/* Save player position */
	flow_y = py;
	flow_x = px;

	/* Add the player's grid to the queue */
	temp_y[0] = py;
	temp_x[0] = px;

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		int ty, tx;

		/* Extract the next entry */
		ty = temp_y[flow_tail];
		tx = temp_x[flow_tail];

		/* Forget that entry */
		if (++flow_tail == TEMP_MAX) flow_tail = 0;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			int old_head = flow_head;
			int m = cave[ty][tx].cost + 1;
			int n = cave[ty][tx].dist + 1;
			cave_type *c_ptr;

			/* Child location */
			y = ty + ddy_ddd[d];
			x = tx + ddx_ddd[d];

			/* Ignore player's grid */
			if (player_bold(y, x)) continue;

			c_ptr = &cave[y][x];

			if (is_closed_door(c_ptr->feat)) m += 3;

			/* Ignore "pre-stamped" entries */
			if (c_ptr->dist != 0 && c_ptr->dist <= n && c_ptr->cost <= m) continue;

			/* Ignore "walls" and "rubble" */
			if (!cave_have_flag_grid(c_ptr, FF_MOVE) && !is_closed_door(c_ptr->feat)) continue;

			/* Save the flow cost */
			if (c_ptr->cost == 0 || c_ptr->cost > m) c_ptr->cost = m;
			if (c_ptr->dist == 0 || c_ptr->dist > n) c_ptr->dist = n;

			/* Hack -- limit flow depth */
			if (n == MONSTER_FLOW_DEPTH) continue;

			/* Enqueue that entry */
			temp_y[flow_head] = y;
			temp_x[flow_head] = x;

			/* Advance the queue */
			if (++flow_head == TEMP_MAX) flow_head = 0;

			/* Hack -- notice overflow by forgetting new entry */
			if (flow_head == flow_tail) flow_head = old_head;
		}
	}
	//v1.1.24 �ЂƂ�̊֐��Q��
	update_flying_dist();

}



static int scent_when = 0;

/*
 * Characters leave scent trails for perceptive monsters to track.
 *
 * Smell is rather more limited than sound.  Many creatures cannot use 
 * it at all, it doesn't extend very far outwards from the character's 
 * current position, and monsters can use it to home in the character, 
 * but not to run away from him.
 *
 * Smell is valued according to age.  When a character takes his turn, 
 * scent is aged by one, and new scent of the current age is laid down.  
 * Speedy characters leave more scent, true, but it also ages faster, 
 * which makes it harder to hunt them down.
 *
 * Whenever the age count loops, most of the scent trail is erased and 
 * the age of the remainder is recalculated.
 */
void update_smell(void)
{
	int i, j;
	int y, x;

	/* Create a table that controls the spread of scent */
	const int scent_adjust[5][5] = 
	{
		{ -1, 0, 0, 0,-1 },
		{  0, 1, 1, 1, 0 },
		{  0, 1, 2, 1, 0 },
		{  0, 1, 1, 1, 0 },
		{ -1, 0, 0, 0,-1 },
	};

	/* Loop the age and adjust scent values when necessary */
	if (++scent_when == 254)
	{
		/* Scan the entire dungeon */
		for (y = 0; y < cur_hgt; y++)
		{
			for (x = 0; x < cur_wid; x++)
			{
				int w = cave[y][x].when;
				cave[y][x].when = (w > 128) ? (w - 128) : 0;
			}
		}

		/* Restart */
		scent_when = 126;
	}


	/* Lay down new scent */
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			cave_type *c_ptr;

			/* Translate table to map grids */
			y = i + py - 2;
			x = j + px - 2;

			/* Check Bounds */
			if (!in_bounds(y, x)) continue;

			c_ptr = &cave[y][x];

			/* Walls, water, and lava cannot hold scent. */
			if (!cave_have_flag_grid(c_ptr, FF_MOVE) && !is_closed_door(c_ptr->feat)) continue;

			/* Grid must not be blocked by walls from the character */
			if (!player_has_los_bold(y, x)) continue;

			/* Note grids that are too far away */
			if (scent_adjust[i][j] == -1) continue;

			/* Mark the grid with new scent */
			c_ptr->when = scent_when + scent_adjust[i][j];
		}
	}
}


/*
 * Hack -- map the current panel (plus some) ala "magic mapping"
 */
/*:::���ӊ��m �ڍז���*/
void map_area(int range)
{
	int             i, x, y;
	cave_type       *c_ptr;
	s16b            feat;
	feature_type    *f_ptr;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan that area */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if (distance(py, px, y, x) > range) continue;

			c_ptr = &cave[y][x];

			/* Feature code (applying "mimic" field) */
			feat = get_feat_mimic(c_ptr);
			f_ptr = &f_info[feat];

			///mod150116 ���т̎������n�`���m
			if(murasa_detect_water && !have_flag(f_ptr->flags, FF_WATER)) continue;

			/* Memorize terrain of the grid */
			///mod150116�������Ɉړ�
			c_ptr->info |= (CAVE_KNOWN);


			/* All non-walls are "checked" */
			if (!have_flag(f_ptr->flags, FF_WALL))
			{
				/* Memorize normal features */
				if (have_flag(f_ptr->flags, FF_REMEMBER))
				{
					/* Memorize the object */
					c_ptr->info |= (CAVE_MARK);
				}

				/* Memorize known walls */
				for (i = 0; i < 8; i++)
				{
					if(murasa_detect_water) break;

					c_ptr = &cave[y + ddy_ddd[i]][x + ddx_ddd[i]];

					/* Feature code (applying "mimic" field) */
					feat = get_feat_mimic(c_ptr);
					f_ptr = &f_info[feat];

					/* Memorize walls (etc) */
					if (have_flag(f_ptr->flags, FF_REMEMBER))
					{
						/* Memorize the walls */
						c_ptr->info |= (CAVE_MARK);
					}
				}
			}
		}
	}

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}



/*
 * Light up the dungeon using "clairvoyance"
 *
 * This function "illuminates" every grid in the dungeon, memorizes all
 * "objects", memorizes all grids as with magic mapping, and, under the
 * standard option settings (view_perma_grids but not view_torch_grids)
 * memorizes all floor grids too.
 *
 * Note that if "view_perma_grids" is not set, we do not memorize floor
 * grids, since this would defeat the purpose of "view_perma_grids", not
 * that anyone seems to play without this option.
 *
 * Note that if "view_torch_grids" is set, we do not memorize floor grids,
 * since this would prevent the use of "view_torch_grids" as a method to
 * keep track of what grids have been observed directly.
 */
/*:::�痢��@�ڍז���*/
void wiz_lite(bool ninja)
{
	int i, y, x;
	s16b feat;
	feature_type *f_ptr;

	/* Memorize objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		//v1.1.43 �T�j�[���Z��p�t���O �ʂ̏ꏊ�Ōv�Z����flying_dist�l�̂���Ƃ��낾���[�ւ���
		if (flag_sunny_refraction)
		{
			if (!cave[o_ptr->iy][o_ptr->ix].flying_dist) continue;
		}

		/* Memorize */
		o_ptr->marked |= OM_FOUND;
	}

	/* Scan all normal grids */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		/* Scan all normal grids */
		for (x = 1; x < cur_wid - 1; x++)
		{
			cave_type *c_ptr = &cave[y][x];


			//v1.1.43 �T�j�[���Z��p�t���O �ʂ̏ꏊ�Ōv�Z����flying_dist�l�̂���Ƃ��낾���[�ւ���
			if (flag_sunny_refraction)
			{
				if (!cave[y][x].flying_dist) continue;
			}

			/* Memorize terrain of the grid */
			c_ptr->info |= (CAVE_KNOWN);

			/* Feature code (applying "mimic" field) */
			feat = get_feat_mimic(c_ptr);
			f_ptr = &f_info[feat];

			/* Process all non-walls */
			if (!have_flag(f_ptr->flags, FF_WALL))
			{
				/* Scan all neighbors */
				for (i = 0; i < 9; i++)
				{
					int yy = y + ddy_ddd[i];
					int xx = x + ddx_ddd[i];

					/* Get the grid */
					c_ptr = &cave[yy][xx];

					/* Feature code (applying "mimic" field) */
					f_ptr = &f_info[get_feat_mimic(c_ptr)];

					/* Perma-lite the grid */
					if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS) && !ninja)
					{
						c_ptr->info |= (CAVE_GLOW);
					}

					/* Memorize normal features */
					if (have_flag(f_ptr->flags, FF_REMEMBER))
					{
						/* Memorize the grid */
						c_ptr->info |= (CAVE_MARK);
					}

					/* Perma-lit grids (newly and previously) */
					else if (c_ptr->info & CAVE_GLOW)
					{
						/* Normally, memorize floors (see above) */
						if (view_perma_grids && !view_torch_grids)
						{
							/* Memorize the grid */
							c_ptr->info |= (CAVE_MARK);
						}
					}
				}
			}
		}
	}

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	set_deity_bias(DBIAS_COSMOS, 1);
	if(one_in_(2))set_deity_bias(DBIAS_WARLIKE, -1);


	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		if (cave[py][px].info & CAVE_GLOW) set_superstealth(FALSE);
	}
}


/*
 * Forget the dungeon map (ala "Thinking of Maud...").
 */
/*:::�_���W�����̒n�`��Y��鏈���H*/
void wiz_dark(void)
{
	int i, y, x;


	/* Forget every grid */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			/* Process the grid */
			c_ptr->info &= ~(CAVE_MARK | CAVE_IN_DETECT | CAVE_KNOWN);
			c_ptr->info |= (CAVE_UNSAFE);
		}
	}

	/* Forget every grid on horizontal edge */
	for (x = 0; x < cur_wid; x++)
	{
		cave[0][x].info &= ~(CAVE_MARK);
		cave[cur_hgt - 1][x].info &= ~(CAVE_MARK);
	}

	/* Forget every grid on vertical edge */
	for (y = 1; y < (cur_hgt - 1); y++)
	{
		cave[y][0].info &= ~(CAVE_MARK);
		cave[y][cur_wid - 1].info &= ~(CAVE_MARK);
	}

	/* Forget all objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Forget the object */
		o_ptr->marked &= OM_TOUCHED;
	}

	/* Forget travel route when we have forgotten map */
	forget_travel_flow();

	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update the view and lite */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}





/*
 * Change the "feat" flag for a grid, and notice/redraw the grid
 */
/*:::�n�`�ύX�B��ɏ��������ʂ̂��̂ɂ���Ƃ�*/
/*:::������[���͏�����*/
/*:::������_���W�����ɑ΂���ύX�̏ꍇ�A�����X�^�[������֌W�̏����X�V����*/
void cave_set_feat(int y, int x, int feat)
{
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[feat];
	bool old_los, old_mirror;

	/*:::�_���W�����������̂Ƃ�character_dungeon=FALSE*/
	if (!character_dungeon)
	{
		/* Clear mimic type */
		c_ptr->mimic = 0;

		/* Change the feature */
		c_ptr->feat = feat;

		/* Hack -- glow the GLOW terrain */
		if (have_flag(f_ptr->flags, FF_GLOW) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS))
		{
			int i, yy, xx;

			for (i = 0; i < 9; i++)
			{
				yy = y + ddy_ddd[i];
				xx = x + ddx_ddd[i];
				if (!in_bounds2(yy, xx)) continue;
				cave[yy][xx].info |= CAVE_GLOW;
			}
		}

		return;
	}
	/*:::���������͐����ς݃_���W�����ɑ΂��鉽�炩�̍s���ɂ��ύX*/
	old_los = cave_have_flag_bold(y, x, FF_LOS);
	old_mirror = is_mirror_grid(c_ptr);

	/* Clear mimic type */
	c_ptr->mimic = 0;

	/* Change the feature */
	c_ptr->feat = feat;

	/* Remove flag for mirror/glyph */
	c_ptr->info &= ~(CAVE_OBJECT);

	if (old_mirror && (d_info[dungeon_type].flags1 & DF1_DARKNESS))
	{
		c_ptr->info &= ~(CAVE_GLOW);
		if (!view_torch_grids) c_ptr->info &= ~(CAVE_MARK);

		update_local_illumination(y, x);
	}

	/* Check for change to boring grid */
	if (!have_flag(f_ptr->flags, FF_REMEMBER)) c_ptr->info &= ~(CAVE_MARK);

	/* Update the monster */
	if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);

	/* Notice */
	note_spot(y, x);

	/* Redraw */
	lite_spot(y, x);

	/* Check if los has changed */
	if (old_los ^ have_flag(f_ptr->flags, FF_LOS))
	{

#ifdef COMPLEX_WALL_ILLUMINATION /* COMPLEX_WALL_ILLUMINATION */

		update_local_illumination(y, x);

#endif /* COMPLEX_WALL_ILLUMINATION */

		/* Update the visuals */
		p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE | PU_MONSTERS);
	}

	/* Hack -- glow the GLOW terrain */
	if (have_flag(f_ptr->flags, FF_GLOW) && !(d_info[dungeon_type].flags1 & DF1_DARKNESS))
	{
		int i, yy, xx;
		cave_type *cc_ptr;

		for (i = 0; i < 9; i++)
		{
			yy = y + ddy_ddd[i];
			xx = x + ddx_ddd[i];
			if (!in_bounds2(yy, xx)) continue;
			cc_ptr = &cave[yy][xx];
			cc_ptr->info |= CAVE_GLOW;

			if (player_has_los_grid(cc_ptr))
			{
				/* Update the monster */
				if (cc_ptr->m_idx) update_mon(cc_ptr->m_idx, FALSE);

				/* Notice */
				note_spot(yy, xx);

				/* Redraw */
				lite_spot(yy, xx);
			}

			update_local_illumination(yy, xx);
		}

		if (p_ptr->special_defense & NINJA_S_STEALTH)
		{
			if (cave[py][px].info & CAVE_GLOW) set_superstealth(FALSE);
		}
	}
}

/*:::c_ptr->feat = conv_dungeon_feat(letter[idx].feature);  init1.c���*/

/*:::newfeat:f_info�̒n�`�C���f�b�N�X�H�����ĂȂ��ꍇ�͂ǂ��Ȃ�H*/
int conv_dungeon_feat(int newfeat)
{
	feature_type *f_ptr = &f_info[newfeat];

	/*:::defines.h��FF_????�Ƃ��ăt���O����`����Ă���*/
	if (have_flag(f_ptr->flags, FF_CONVERT))
	{
		switch (f_ptr->subtype)
		{
		case CONVERT_TYPE_FLOOR:
			/*:::set_floor_and_wall()�Ń_���W�����ʂɕǂ̎�ނ��ݒ肳��Ă���͂�*/
			return floor_type[randint0(100)];
		case CONVERT_TYPE_WALL:
			return fill_type[randint0(100)];
		case CONVERT_TYPE_INNER:
			return feat_wall_inner;
		case CONVERT_TYPE_OUTER:
			return feat_wall_outer;
		case CONVERT_TYPE_SOLID:
			return feat_wall_solid;
		case CONVERT_TYPE_STREAM1:
			return d_info[dungeon_type].stream1;
		case CONVERT_TYPE_STREAM2:
			return d_info[dungeon_type].stream2;
		default:
			return newfeat;
		}
	}
	else return newfeat;
}


/*
 * Take a feature, determine what that feature becomes
 * through applying the given action.
 */
/*:::�n�`�ɑ΂��Ď��������̍s���ɂ�肻�̒n�`���ǂ��ς�邩������B�ڍז���*/
int feat_state(int feat, int action)
{
	feature_type *f_ptr = &f_info[feat];
	int i;

	/* Get the new feature */
	for (i = 0; i < MAX_FEAT_STATES; i++)
	{
		if (f_ptr->state[i].action == action) return conv_dungeon_feat(f_ptr->state[i].result);
	}

	if (have_flag(f_ptr->flags, FF_PERMANENT)) return feat;

	return (feature_action_flags[action] & FAF_DESTROY) ? conv_dungeon_feat(f_ptr->destroyed) : feat;
}

/*
 * Takes a location and action and changes the feature at that 
 * location through applying the given action.
 */
/*:::�n�`�ύX�B��ɉ������󂵂ď��ɂ���Ƃ��B*/
/*:::�V�����n�`�����肵�Acave_set_feat()�ŕύX����*/
/*:::����A�C�e�����@��o������K���X�n�`���󂷏������s���Ă���*/
void cave_alter_feat(int y, int x, int action)
{
	/* Set old feature */
	int oldfeat = cave[y][x].feat;

	/* Get the new feat */
	int newfeat = feat_state(oldfeat, action);

	/* No change */
	if (newfeat == oldfeat) return;

	/* Set the new feature */
	cave_set_feat(y, x, newfeat);

	if (!(feature_action_flags[action] & FAF_NO_DROP))
	{
		feature_type *old_f_ptr = &f_info[oldfeat];
		feature_type *f_ptr = &f_info[newfeat];
		bool found = FALSE;

		/* Handle gold */
		if (have_flag(old_f_ptr->flags, FF_HAS_GOLD) && !have_flag(f_ptr->flags, FF_HAS_GOLD))
		{
			/* Place some gold */
			place_gold(y, x);
			found = TRUE;
		}
		///mod140405 ��΂��������₪�]�����Ă��邱�Ƃɂ���
		/* Handle item */
		if (have_flag(old_f_ptr->flags, FF_HAS_ITEM) && !have_flag(f_ptr->flags, FF_HAS_ITEM) && (one_in_(2)))
		{
			int i;
			object_type forge;
			object_type *q_ptr = &forge;
			object_prep(q_ptr, 605); //�Ή�
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
			/* Place object */
			//place_object(y, x, 0L);
			found = TRUE;
		}

		if (found && character_dungeon && player_can_see_bold(y, x))
		{
#ifdef JP
		//	msg_print("�����𔭌������I");
#else
			msg_print("You have found something!");
#endif
		}
	}

	if (feature_action_flags[action] & FAF_CRASH_GLASS)
	{
		feature_type *old_f_ptr = &f_info[oldfeat];

		///mod160118 ��鎞�Ԓ�~���̓K���X���ӂ��Ă��j�Ђ���΂Ȃ�
		if (have_flag(old_f_ptr->flags, FF_GLASS) && character_dungeon && !SAKUYA_WORLD)
		{
			project(PROJECT_WHO_GLASS_SHARDS, 1, y, x, MIN(dun_level, 100) / 4, GF_SHARDS,
			        (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_HIDE | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
		}
	}
}


/* Remove a mirror */
/*:::�������ł�����B���[���Ȃǂ������炵��*/
void remove_mirror(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Remove the mirror */
	c_ptr->info &= ~(CAVE_OBJECT);
	c_ptr->mimic = 0;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS)
	{
		c_ptr->info &= ~(CAVE_GLOW);
		if (!view_torch_grids) c_ptr->info &= ~(CAVE_MARK);

		/* Update the monster */
		if (c_ptr->m_idx) update_mon(c_ptr->m_idx, FALSE);

		update_local_illumination(y, x);
	}

	/* Notice */
	note_spot(y, x);

	/* Redraw */
	lite_spot(y, x);
}


/*
 *  Return TRUE if there is a mirror on the grid.
 */
bool is_mirror_grid(cave_type *c_ptr)
{
	if ((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_MIRROR))
		return TRUE;
	else
		return FALSE;
}


/*
 *  Return TRUE if there is a mirror on the grid.
 */
bool is_glyph_grid(cave_type *c_ptr)
{
	if ((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_GLYPH))
		return TRUE;
	else
		return FALSE;
}

//�u���_�̈�v�̔���
bool is_elder_sign(cave_type *c_ptr)
{
	if ((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_ELDER_SIGN))
		return TRUE;
	else
		return FALSE;
}


/*
 *  Return TRUE if there is a mirror on the grid.
 */
bool is_explosive_rune_grid(cave_type *c_ptr)
{
	if ((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_MINOR_GLYPH))
		return TRUE;
	else
		return FALSE;
}

///mod150719 �z�s �M��u�����n�`
bool is_plate_grid(cave_type *c_ptr)
{
	if ((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_PLATE))
		return TRUE;
	else
		return FALSE;
}


/*
 * Calculate "incremental motion". Used by project() and shoot().
 * Assumes that (*y,*x) lies on the path from (y1,x1) to (y2,x2).
 */
void mmove2(int *y, int *x, int y1, int x1, int y2, int x2)
{
	int dy, dx, dist, shift;

	/* Extract the distance travelled */
	dy = (*y < y1) ? y1 - *y : *y - y1;
	dx = (*x < x1) ? x1 - *x : *x - x1;

	/* Number of steps */
	dist = (dy > dx) ? dy : dx;

	/* We are calculating the next location */
	dist++;


	/* Calculate the total distance along each axis */
	dy = (y2 < y1) ? (y1 - y2) : (y2 - y1);
	dx = (x2 < x1) ? (x1 - x2) : (x2 - x1);

	/* Paranoia -- Hack -- no motion */
	if (!dy && !dx) return;


	/* Move mostly vertically */
	if (dy > dx)
	{
		/* Extract a shift factor */
		shift = (dist * dx + (dy - 1) / 2) / dy;

		/* Sometimes move along the minor axis */
		(*x) = (x2 < x1) ? (x1 - shift) : (x1 + shift);

		/* Always move along major axis */
		(*y) = (y2 < y1) ? (y1 - dist) : (y1 + dist);
	}

	/* Move mostly horizontally */
	else
	{
		/* Extract a shift factor */
		shift = (dist * dy + (dx - 1) / 2) / dx;

		/* Sometimes move along the minor axis */
		(*y) = (y2 < y1) ? (y1 - shift) : (y1 + shift);

		/* Always move along major axis */
		(*x) = (x2 < x1) ? (x1 - dist) : (x1 + dist);
	}
}



/*
 * Determine if a bolt spell cast from (y1,x1) to (y2,x2) will arrive
 * at the final destination, assuming no monster gets in the way.
 *
 * This is slightly (but significantly) different from "los(y1,x1,y2,x2)".
 */
/*:::�ǂɂ�������ꂸ�������ʂ��Ă��邩���肷��H*/
bool projectable(int y1, int x1, int y2, int x2)
{
	int y, x;

	int grid_n = 0;
	u16b grid_g[512];

	/* Check the projection path */
	/*:::Q011 �����܂��ڂ������ĂȂ�*/
	grid_n = project_path(grid_g, (project_length ? project_length : MAX_RANGE), y1, x1, y2, x2, 0);

	/* Identical grid */
	if (!grid_n) return TRUE;

	/* Final grid */
	y = GRID_Y(grid_g[grid_n - 1]);
	x = GRID_X(grid_g[grid_n - 1]);

	/* May not end in an unrequested grid */
	if ((y != y2) || (x != x2)) return (FALSE);

	/* Assume okay */
	return (TRUE);
}


/*
 * Standard "find me a location" function
 *
 * Obtains a legal location within the given distance of the initial
 * location, and with "los()" from the source to destination location.
 *
 * This function is often called from inside a loop which searches for
 * locations while increasing the "d" distance.
 *
 * Currently the "m" parameter is unused.
 */
//�w��|�C���g����ː��̒ʂ�������d�ȓ��̃����_���Ȓn�_��Ԃ�
//�����ǂ̒��Ŏg������w��|�C���g�Ɠ������W���Ԃ�͂�
void scatter(int *yp, int *xp, int y, int x, int d, int m)
{
	int nx, ny;

	/* Unused */
	m = m;

	//v1.1.92 �O�̈גǉ�
	if (!in_bounds(y, x))
	{
		msg_print("ERROR:scatter()�ɕs����x,y���n���ꂽ");
		return;
	}

	/* Pick a location */
	while (TRUE)
	{
		/* Pick a new location */
		ny = rand_spread(y, d);
		nx = rand_spread(x, d);

		/* Ignore annoying locations */
		if (!in_bounds(ny, nx)) continue;

		/* Ignore "excessively distant" locations */
		if ((d > 1) && (distance(y, x, ny, nx) > d)) continue;

		/* Require "line of projection" */
		if (projectable(y, x, ny, nx)) break;
	}

	/* Save the location */
	(*yp) = ny;
	(*xp) = nx;
}




/*
 * Track a new monster
 */
void health_track(int m_idx)
{
	/* Mount monster is already tracked */
	if (m_idx && m_idx == p_ptr->riding) return;

	/* Track a new guy */
	p_ptr->health_who = m_idx;

	/* Redraw (later) */
	p_ptr->redraw |= (PR_HEALTH);
}



/*
 * Hack -- track the given monster race
 */
/*:::�����X�^�[�̎v���o�E�B���h�E�ɕ\������t���O�𗧂Ă�*/
void monster_race_track(int r_idx)
{
	/* Save this monster ID */
	p_ptr->monster_race_idx = r_idx;

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER);
}



/*
 * Hack -- track the given object kind
 */
void object_kind_track(int k_idx)
{
	/* Save this monster ID */
	p_ptr->object_kind_idx = k_idx;

	/* Window stuff */
	p_ptr->window |= (PW_OBJECT);
}



/*
 * Something has happened to disturb the player.
 *
 * The first arg indicates a major disturbance, which affects search.
 *
 * The second arg is currently unused, but could induce output flush.
 *
 * All disturbance cancels repeated commands, resting, and running.
 */
void disturb(int stop_search, int stop_travel)
{
#ifndef TRAVEL
	/* Unused */
	stop_travel = stop_travel;
#endif

	/* Cancel auto-commands */
	/* command_new = 0; */

	/* Cancel repeated commands */
	if (command_rep)
	{
		/* Cancel */
		command_rep = 0;

		/* Redraw the state (later) */
		p_ptr->redraw |= (PR_STATE);
	}

	/* Cancel Resting */
	if ((p_ptr->action == ACTION_REST) || (p_ptr->action == ACTION_FISH) || (stop_search && (p_ptr->action == ACTION_SEARCH)))
	{
		/* Cancel */
		set_action(ACTION_NONE);

	}

	/* Cancel running */
	if (running)
	{
		/* Cancel */
		running = 0;

		/* Check for new panel if appropriate */
		if (center_player && !center_running) verify_panel();

		/* Calculate torch radius */
		p_ptr->update |= (PU_TORCH);

		/* Update monster flow */
		p_ptr->update |= (PU_FLOW);
	}

#ifdef TRAVEL
	if (stop_travel)
	{
		/* Cancel */
		travel.run = 0;

		/* Check for new panel if appropriate */
		if (center_player && !center_running) verify_panel();

		/* Calculate torch radius */
		p_ptr->update |= (PU_TORCH);
	}
#endif

	/* Flush the input if requested */
	if (flush_disturb) flush();
}


/*
 * Glow deep lava and building entrances in the floor
 */
/*:::�n��ƌ�������������点��*/
void glow_deep_lava_and_bldg(void)
{
	int y, x, i, yy, xx;
	cave_type *c_ptr;

	/* Not in the darkness dungeon */
	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) return;

	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			c_ptr = &cave[y][x];

			/* Feature code (applying "mimic" field) */

			if (have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_GLOW))
			{
				for (i = 0; i < 9; i++)
				{
					yy = y + ddy_ddd[i];
					xx = x + ddx_ddd[i];
					if (!in_bounds2(yy, xx)) continue;
					cave[yy][xx].info |= CAVE_GLOW;
				}
			}
		}
	}

	/* Update the view and lite */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);
}
