/* File: object2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Object code, part 2 */

#include "angband.h"

#include "kajitips.h"


/*
 * Excise a dungeon object from any stacks
 */
void excise_object_idx(int o_idx)
{
	object_type *j_ptr;

	s16b this_o_idx, next_o_idx = 0;

	s16b prev_o_idx = 0;


	/* Object */
	j_ptr = &o_list[o_idx];

	/* Monster */
	if (j_ptr->held_m_idx)
	{
		monster_type *m_ptr;

		/* Monster */
		m_ptr = &m_list[j_ptr->held_m_idx];

		/* Scan all objects in the grid */
		for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Done */
			if (this_o_idx == o_idx)
			{
				/* No previous */
				if (prev_o_idx == 0)
				{
					/* Remove from list */
					m_ptr->hold_o_idx = next_o_idx;
				}

				/* Real previous */
				else
				{
					object_type *k_ptr;

					/* Previous object */
					k_ptr = &o_list[prev_o_idx];

					/* Remove from list */
					k_ptr->next_o_idx = next_o_idx;
				}

				/* Forget next pointer */
				o_ptr->next_o_idx = 0;

				/* Done */
				break;
			}

			/* Save prev_o_idx */
			prev_o_idx = this_o_idx;
		}
	}

	/* Dungeon */
	else
	{
		cave_type *c_ptr;

		int y = j_ptr->iy;
		int x = j_ptr->ix;

		/* Grid */
		c_ptr = &cave[y][x];

		/* Scan all objects in the grid */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Done */
			if (this_o_idx == o_idx)
			{
				/* No previous */
				if (prev_o_idx == 0)
				{
					/* Remove from list */
					c_ptr->o_idx = next_o_idx;
				}

				/* Real previous */
				else
				{
					object_type *k_ptr;

					/* Previous object */
					k_ptr = &o_list[prev_o_idx];

					/* Remove from list */
					k_ptr->next_o_idx = next_o_idx;
				}

				/* Forget next pointer */
				o_ptr->next_o_idx = 0;

				/* Done */
				break;
			}

			/* Save prev_o_idx */
			prev_o_idx = this_o_idx;
		}
	}
}


/*
 * Delete a dungeon object
 *
 * Handle "stacks" of objects correctly.
 */
/*:::����A�C�e����j�󂷂�B�X�^�b�N�l���B*/
void delete_object_idx(int o_idx)
{
	object_type *j_ptr;

	/* Excise */
	excise_object_idx(o_idx);

	/* Object */
	j_ptr = &o_list[o_idx];

	/* Dungeon floor */
	if (!(j_ptr->held_m_idx))
	{
		int y, x;

		/* Location */
		y = j_ptr->iy;
		x = j_ptr->ix;

		/* Visual update */
		lite_spot(y, x);
	}

	/* Wipe the object */
	object_wipe(j_ptr);

	/* Count objects */
	o_cnt--;
}


/*
 * Deletes all objects at given location
 */
void delete_object(int y, int x)
{
	cave_type *c_ptr;

	s16b this_o_idx, next_o_idx = 0;


	/* Refuse "illegal" locations */
	if (!in_bounds(y, x)) return;


	/* Grid */
	c_ptr = &cave[y][x];

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Wipe the object */
		object_wipe(o_ptr);

		/* Count objects */
		o_cnt--;
	}

	/* Objects are gone */
	c_ptr->o_idx = 0;

	/* Visual update */
	lite_spot(y, x);
}


/*
 * Move an object from index i1 to index i2 in the object list
 */
/*:::o_list[]�̒��ŗv�f���ړ�����@�ړ���㏑��*/
/*:::cave[][].o_idx�Ȃ�o_list[]�̓Y�������g���Ă镔�����C������*/
static void compact_objects_aux(int i1, int i2)
{
	int i;

	cave_type *c_ptr;

	object_type *o_ptr;


	/* Do nothing */
	if (i1 == i2) return;

	/*:::�A�C�e�����ςݏd�Ȃ��Ă�ꍇ��next�l���C��*/
	/* Repair objects */
	for (i = 1; i < o_max; i++)
	{
		/* Acquire object */
		o_ptr = &o_list[i];

		/* Skip "dead" objects */
		if (!o_ptr->k_idx) continue;

		/* Repair "next" pointers */
		if (o_ptr->next_o_idx == i1)
		{
			/* Repair */
			o_ptr->next_o_idx = i2;
		}
	}


	/* Acquire object */
	o_ptr = &o_list[i1];

	/*:::�����X�^�[���A�C�e���������Ă���ꍇ��idx�l���C��*/
	/* Monster */
	if (o_ptr->held_m_idx)
	{
		monster_type *m_ptr;

		/* Acquire monster */
		m_ptr = &m_list[o_ptr->held_m_idx];

		/* Repair monster */
		if (m_ptr->hold_o_idx == i1)
		{
			/* Repair */
			m_ptr->hold_o_idx = i2;
		}
	}

	/* Dungeon */
	/*:::�_���W�����ɃA�C�e���������Ă���ꍇ�Acave[][].o_idx���C��*/
	else
	{
		int y, x;

		/* Acquire location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Acquire grid */
		c_ptr = &cave[y][x];

		/* Repair grid */
		if (c_ptr->o_idx == i1)
		{
			/* Repair */
			c_ptr->o_idx = i2;
		}
	}


	/* Structure copy */
	o_list[i2] = o_list[i1];

	/* Wipe the hole */
	object_wipe(o_ptr);
}


/*
 * Compact and Reorder the object list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" objects, we base the saving throw on a
 * combination of object level, distance from player, and current
 * "desperation".
 *
 * After "compacting" (if needed), we "reorder" the objects into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
/*:::�t���A�̃A�C�e�����X�g�����k����B*/
/*:::size���w�肳�ꂽ�ꍇ���̐��̃A�C�e�������ł�����B�჌�x���Ł����痣�ꂽ�A�C�e�����폜����₷���B*/
/*:::�A�[�e�B�t�@�N�g�͂�����m���ō폜�����*/
void compact_objects(int size)
{
	int i, y, x, num, cnt;
	int cur_lev, cur_dis, chance;
	object_type *o_ptr;


	/* Compact */
	if (size)
	{
		/* Message */
#ifdef JP
		msg_print("�A�C�e���������k���Ă��܂�...");
#else
		msg_print("Compacting objects...");
#endif


		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
	}


	/* Compact at least 'size' objects */
	/*:::size��1�ȏ�̂Ƃ����̐��̃A�C�e�������ł�����*/
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Examine the objects */
		for (i = 1; i < o_max; i++)
		{
			o_ptr = &o_list[i];

			/* Skip dead objects */
			/*:::�����ȃA�C�e���͑Ώۂł͂Ȃ��B���Ƃō폜���邩��*/
			if (!o_ptr->k_idx) continue;

			/* Hack -- High level objects start out "immune" */
			if (k_info[o_ptr->k_idx].level > cur_lev) continue;

			/* Monster */
			/*:::�����X�^�[�̎����Ă���A�C�e���͏��ł��ɂ���*/
			if (o_ptr->held_m_idx)
			{
				monster_type *m_ptr;

				/* Acquire monster */
				m_ptr = &m_list[o_ptr->held_m_idx];

				/* Get the location */
				y = m_ptr->fy;
				x = m_ptr->fx;

				/* Monsters protect their objects */
				if (randint0(100) < 90) continue;
			}

			/* Dungeon */
			else
			{
				/* Get the location */
				y = o_ptr->iy;
				x = o_ptr->ix;
			}

			/* Nearby objects start out "immune" */
			/*:::�߂��̃A�C�e���͏��ł��Ȃ�*/
			if ((cur_dis > 0) && (distance(py, px, y, x) < cur_dis)) continue;

			/* Saving throw */
			chance = 90;

			/* Hack -- only compact artifacts in emergencies */
			if ((object_is_fixed_artifact(o_ptr) || o_ptr->art_name) &&
			    (cnt < 1000)) chance = 100;

			/* Apply the saving throw */
			if (randint0(100) < chance) continue;

			/* Delete the object */
			delete_object_idx(i);

			/* Count it */
			num++;
		}
	}


	/* Excise dead objects (backwards!) */
	for (i = o_max - 1; i >= 1; i--)
	{
		o_ptr = &o_list[i];

		/* Skip real objects */
		if (o_ptr->k_idx) continue;

		/* Move last object into open hole */
		compact_objects_aux(o_max - 1, i);

		/* Compress "o_max" */
		o_max--;
	}
}


/*
 * Delete all the items when player leaves the level
 *
 * Note -- we do NOT visually reflect these (irrelevant) changes
 *
 * Hack -- we clear the "c_ptr->o_idx" field for every grid,
 * and the "m_ptr->next_o_idx" field for every monster, since
 * we know we are clearing every object.  Technically, we only
 * clear those fields for grids/monsters containing objects,
 * and we clear it once for every such object.
 */
void wipe_o_list(void)
{
	int i;

	/* Delete the existing objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Mega-Hack -- preserve artifacts */
		if (!character_dungeon || preserve_mode)
		{
			/* Hack -- Preserve unknown artifacts */
			if (object_is_fixed_artifact(o_ptr) && !object_is_known(o_ptr))
			{
				/* Mega-Hack -- Preserve the artifact */
				a_info[o_ptr->name1].cur_num = 0;
			}
		}

		/* Monster */
		if (o_ptr->held_m_idx)
		{
			monster_type *m_ptr;

			/* Monster */
			m_ptr = &m_list[o_ptr->held_m_idx];

			/* Hack -- see above */
			m_ptr->hold_o_idx = 0;
		}

		/* Dungeon */
		else
		{
			cave_type *c_ptr;

			/* Access location */
			int y = o_ptr->iy;
			int x = o_ptr->ix;

			/* Access grid */
			c_ptr = &cave[y][x];

			/* Hack -- see above */
			c_ptr->o_idx = 0;
		}

		/* Wipe the object */
		/*:::o_ptr�����̃������J���H*/
		object_wipe(o_ptr);
	}

	/* Reset "o_max" */
	o_max = 1;

	/* Reset "o_cnt" */
	o_cnt = 0;
}


/*
 * Acquires and returns the index of a "free" object.
 *
 * This routine should almost never fail, but in case it does,
 * we must be sure to handle "failure" of this routine.
 */
/*:::o_list[]�̒��Ŏg���Ă��Ȃ��z��ԍ���Ԃ� �A�C�e���������łɏ���Ȃ�0��Ԃ�*/
s16b o_pop(void)
{
	int i;


	/* Initial allocation */
	if (o_max < max_o_idx)
	{
		/* Get next space */
		i = o_max;

		/* Expand object array */
		o_max++;

		/* Count objects */
		o_cnt++;

		/* Use this object */
		return (i);
	}


	/* Recycle dead objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[i];

		/* Skip live objects */
		if (o_ptr->k_idx) continue;

		/* Count objects */
		o_cnt++;

		/* Use this object */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
#ifdef JP
	if (character_dungeon) msg_print("�A�C�e������������I");
#else
	if (character_dungeon) msg_print("Too many objects!");
#endif


	/* Oops */
	return (0);
}


/*
 * Apply a "object restriction function" to the "object allocation table"
 */
/*:::���̂��Ǝ��s�����get_obj_num()�Ŏg����x�[�X�A�C�e�����X�galloc_kind_table[]�ɑ΂������s��������Ă���B*/
/*:::get_obj_num_hook�Ɋ֐����ݒ肳��Ă���ꍇ���̊֐��ɃA�C�e����n���AFALSE�ɂȂ�����alloc_kind_table[].prob2��0�ɂȂ��ăA�C�e������������Ȃ��Ȃ�B*/
/*:::get_obj_num_hook���N���A���Ă��炱�̊֐����Ă񂾂琶���s���肪���Z�b�g�����*/
errr get_obj_num_prep(void)
{
	int i;

	/* Get the entry */
	alloc_entry *table = alloc_kind_table;

	/* Scan the allocation table */
	for (i = 0; i < alloc_kind_size; i++)
	{


		/* Accept objects which pass the restriction, if any */
		//else 
			if (!get_obj_num_hook || (*get_obj_num_hook)(table[i].index))
		{
			/* Accept this object */
			table[i].prob2 = table[i].prob1;
		}

		/* Do not use this object */
		else
		{
			/* Decline this object */
			table[i].prob2 = 0;
		}
	}

	/* Success */
	return (0);
}


/*
 * Choose an object kind that seems "appropriate" to the given level
 *
 * This function uses the "prob2" field of the "object allocation table",
 * and various local information, to calculate the "prob3" field of the
 * same table, which is then used to choose an "appropriate" object, in
 * a relatively efficient manner.
 *
 * It is (slightly) more likely to acquire an object of the given level
 * than one of a lower level.  This is done by choosing several objects
 * appropriate to the given level and keeping the "hardest" one.
 *
 * Note that if no objects are "appropriate", then this function will
 * fail, and return zero, but this should *almost* never happen.
 */
/*:::�������x������͂��A���������x�[�X�A�C�e���̃A�C�e����ރC���f�b�N�X(k_info[]�̓Y��)��Ԃ��B */
/*:::�O�x�܂ōĐ����������č����ق��̃��x�����Ƃ邱�Ƃ�level�ɋ߂��A�C�e�������o�₷��.1/10�̊m���Ń��x���u�[�X�g�����B */

s16b get_obj_num(int level)
{
	int             i, j, p;
	int             k_idx;
	long            value, total;
	object_kind     *k_ptr;
	alloc_entry     *table = alloc_kind_table;

	if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;

	/* Boost level */

	if ((level > 0) && !(d_info[dungeon_type].flags1 & DF1_BEGINNER))
	{
		/* Occasional "boost" */
			/*:::1/10*/
		if (one_in_(GREAT_OBJ))
		{
			/* What a bizarre calculation */
			/*:::level�����ƊȒP�ɒ��ˏオ��悤��*/
			level = 1 + (level * MAX_DEPTH / randint1(MAX_DEPTH));
		}
		else if(EXTRA_MODE && one_in_(3))//Extra�ŗǂ��A�C�e�����o�₷������
		{
			level += randint1(50 - level / 4);

		}
	}

	/*:::Hack �����˂͗ǂ��A�C�e�����o�₷��*/
	if((level > 0 && dungeon_type == DUNGEON_MUEN && weird_luck()))
	{
		int tmp = level + randint1(40);
		level = MIN(MAX_DEPTH, tmp);
	}
	/* Reset total */
	total = 0L;

	/* Process probabilities */
	for (i = 0; i < alloc_kind_size; i++)
	{
		/* Objects are sorted by depth */
		if (table[i].level > level) break;

		/* Default */
		table[i].prob3 = 0;

		/* Access the index */
		k_idx = table[i].index;

		/* Access the actual kind */
		k_ptr = &k_info[k_idx];

		/* Hack -- prevent embedded chests */
		/*:::�����J�����Ƃ������甠���o�Ȃ��Ȃ�*/
		if (opening_chest && (k_ptr->tval == TV_CHEST)) continue;

		/*:::Hack �d���{���o�����Ȃ��I�v�V����*/
		if(no_capture_book && (k_ptr->tval == TV_CAPTURE)) continue; 

		/*:::Hack �e���o�����Ȃ��I�v�V����*/
		if(gun_free_world && (k_ptr->tval == TV_GUN)) continue; 

		//Extra���[�h�ł͋A�҂�t���A���Z�b�g���o�Ȃ�
		if(EXTRA_MODE)
		{
			if(k_ptr->tval == TV_SCROLL && k_ptr->sval == SV_SCROLL_WORD_OF_RECALL) continue;
			if(k_ptr->tval == TV_SCROLL && k_ptr->sval == SV_SCROLL_RESET_RECALL) continue;
			if(k_ptr->tval == TV_ROD && k_ptr->sval == SV_ROD_RECALL) continue;
		}

		/* Accept */
		table[i].prob3 = table[i].prob2;

		//EXtra���[�h�ł͗d���{���o�₷�����Ă݂�
		if(EXTRA_MODE && (k_ptr->tval == TV_CAPTURE)) table[i].prob3 *= 2;

		/* Total */
		total += table[i].prob3;
	}

	/* No legal objects */
	if (total <= 0) return (0);


	/* Pick an object */
	value = randint0(total);

	/* Find the object */
	for (i = 0; i < alloc_kind_size; i++)
	{
		/* Found the entry */
		if (value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}


	/* Power boost */
	p = randint0(100);

	/* Try for a "better" object once (50%) or twice (10%) */
	if (p < 60)
	{
		/* Save old */
		j = i;

		/* Pick a object */
		value = randint0(total);

		/* Find the object */
		for (i = 0; i < alloc_kind_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Try for a "better" object twice (10%) */
	if (p < 10)
	{
		/* Save old */
		j = i;

		/* Pick a object */
		value = randint0(total);

		/* Find the object */
		for (i = 0; i < alloc_kind_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}


	/* Result */
	return (table[i].index);
}


/*
 * Known is true when the "attributes" of an object are "known".
 * These include tohit, todam, toac, cost, and pval (charges).
 *
 * Note that "knowing" an object gives you everything that an "awareness"
 * gives you, and much more.  In fact, the player is always "aware" of any
 * item of which he has full "knowledge".
 *
 * But having full knowledge of, say, one "wand of wonder", does not, by
 * itself, give you knowledge, or even awareness, of other "wands of wonder".
 * It happens that most "identify" routines (including "buying from a shop")
 * will make the player "aware" of the object as well as fully "know" it.
 *
 * This routine also removes any inscriptions generated by "feelings".
 */
/*:::�A�C�e�����u�Ӓ�ς݁v�ɂ��邽�߂̃t���O���āH*/
void object_known(object_type *o_ptr)
{
	/* Remove "default inscriptions" */
	o_ptr->feeling = FEEL_NONE;

	/* Clear the "Felt" info */
	o_ptr->ident &= ~(IDENT_SENSE);

	/* Clear the "Empty" info */
	o_ptr->ident &= ~(IDENT_EMPTY);

	/* Now we know about the item */
	o_ptr->ident |= (IDENT_KNOWN);
}


/*
 * The player is now aware of the effects of the given object.
 */
/*:::�A�C�e���𔻕ʏ�Ԃɂ���E�E�H*/
///item �g���΁u���ʁv�ς݂ɂȂ�A�C�e���H
///mod131224 TVAL
void object_aware(object_type *o_ptr)
{
	bool mihanmei = !object_is_aware(o_ptr);

	/* Fully aware of the effects */
	k_info[o_ptr->k_idx].aware = TRUE;

	if(mihanmei && !(k_info[o_ptr->k_idx].gen_flags & TRG_INSTA_ART) && record_ident &&  !p_ptr->is_dead 
		&& (o_ptr->tval == TV_AMULET  || o_ptr->tval == TV_RING  || o_ptr->tval == TV_STAFF  || o_ptr->tval == TV_WAND  
		|| o_ptr->tval == TV_ROD || o_ptr->tval == TV_SCROLL  || o_ptr->tval == TV_MUSHROOM  || o_ptr->tval == TV_POTION))
	{
		object_type forge;
		object_type *q_ptr;
		char o_name[MAX_NLEN];

		q_ptr = &forge;
		object_copy(q_ptr, o_ptr);

		q_ptr->number = 1;

		//v1.1.62
		//�A�N�Z�T���ނ����ŏ����肵���Ƃ�OD_NAME_ONLY���Ɓu���d���̎w�ցw�A�]�b�g�x�����ʂ����B�v�݂����ɂȂ�̂ŏC��
		if(object_is_random_artifact(q_ptr)) 
			sprintf(o_name, "%s", k_name + k_info[q_ptr->k_idx].name);
		else
			object_desc(o_name, q_ptr, OD_NAME_ONLY);
		
		do_cmd_write_nikki(NIKKI_HANMEI, 0, o_name);
	}
}


/*
 * Something has been "sampled"
 */
/*:::�A�C�e���Ɂu�g�������Ƃ�����v�t���O�t��*/
void object_tried(object_type *o_ptr)
{
	/* Mark it as tried (even if "aware") */
	k_info[o_ptr->k_idx].tried = TRUE;
}


/*
 * Return the "value" of an "unknown" item
 * Make a guess at the value of non-aware items
 */
/*:::���Ӓ莞�̃A�C�e�����i�v�Z*/
///item TVAL
///mod131223
static s32b object_value_base(object_type *o_ptr)
{
	/* Aware item -- use template cost */
	if (object_is_aware(o_ptr)) return (k_info[o_ptr->k_idx].cost);

	/* Analyze the type */
	switch (o_ptr->tval)
	{

		/* Un-aware Food */
		case TV_MUSHROOM: return (5L);

		/* Un-aware Potions */
		case TV_POTION: return (20L);

		/* Un-aware Scrolls */
		case TV_SCROLL: return (20L);

		/* Un-aware Staffs */
		case TV_STAFF: return (70L);

		/* Un-aware Wands */
		case TV_WAND: return (50L);

		/* Un-aware Rods */
		case TV_ROD: return (90L);

		/* Un-aware Rings */
		case TV_RING: return (45L);

		/* Un-aware Amulets */
		case TV_AMULET: return (45L);

		/* Figurines, relative to monster level */
		case TV_FIGURINE:
		{
			int level = r_info[o_ptr->pval].level;
			if (level < 20) return level*50L;
			else if (level < 30) return 1000+(level-20)*150L;
			else if (level < 40) return 2500+(level-30)*350L;
			else if (level < 50) return 6000+(level-40)*800L;
			else return 14000+(level-50)*2000L;
		}

		case TV_CAPTURE:
			if (!o_ptr->pval) return 1000L;
			else
			{
				int level = r_info[o_ptr->pval].level;
				if (level < 20) return 1000 + level*50L;
				else if (level < 30) return 2000+(level-20)*150L;
				else if (level < 40) return 3500+(level-30)*350L;
				else if (level < 50) return 7000+(level-40)*800L;
				else return 15000+(level-50)*2000L;
			}
	}

	/* Paranoia -- Oops */
	return (0L);
}


/* Return the value of the flags the object has... */
/*:::�A�C�e���ɂ��Ă���t���O�̉��l�𐔒l���Z�B�A�[�e�B�t�@�N�g�����A�A���h���C�h�o���l�A�l�i�v�Z�Ŏg����*/
///item res flag
///mod131228 TR�t���O����ւ��ɂ���ʑϐ��A�X���C�AESP�ύX
//v1.1.15 ���ς̏����p����t���O�ǉ�
s32b flag_cost(object_type *o_ptr, int plusses, bool flag_junko)
{
	s32b total = 0;
	u32b flgs[TR_FLAG_SIZE];
	s32b tmp_cost;
	int count;
	int i;
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	object_flags(o_ptr, flgs);

	/*
	 * Exclude fixed flags of the base item.
	 * pval bonuses of base item will be treated later.
	 */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] &= ~(k_ptr->flags[i]);

	//v1.1.15 all_flag��TRUE�̂Ƃ�����G�S�̊�{�t���O���Z�����A����̒ǉ��_�C�X���v�Z(���̂Ƃ��돃�ς̏�����p)
	if(flag_junko)
	{
		if(object_is_melee_weapon(o_ptr))
		{
			int dam,dam_base;
			dam = o_ptr->dd * o_ptr->ds;
			dam_base = k_ptr->dd * k_ptr->ds;

			if(dam > dam_base) total += (dam - dam_base) * 500;

		}
		//���ς̃t���O�𓾂�
		junko_gain_equipment_flags(o_ptr,o_ptr->number,flgs);

	}
	else
	{
		/* Exclude fixed flags of the fixed artifact. */
		if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			for (i = 0; i < TR_FLAG_SIZE; i++)
				flgs[i] &= ~(a_ptr->flags[i]);
		}

		/* Exclude fixed flags of the ego-item. */
		else if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			for (i = 0; i < TR_FLAG_SIZE; i++)
				flgs[i] &= ~(e_ptr->flags[i]);
		}
	}



	/*
	 * Calucurate values of remaining flags
	 */
	///mod131229 �����̕]������������C������̂ŏ����������Ă݂�
	if (have_flag(flgs, TR_STR)) total += (2000 * plusses);
	if (have_flag(flgs, TR_INT)) total += (2000 * plusses);
	if (have_flag(flgs, TR_WIS)) total += (1800 * plusses);
	if (have_flag(flgs, TR_DEX)) total += (1800 * plusses);
	if (have_flag(flgs, TR_CON)) total += (2000 * plusses);
	if (have_flag(flgs, TR_CHR)) total += (1500 * plusses);
	if (have_flag(flgs, TR_MAGIC_MASTERY)) total += (3000 * plusses);
	if (have_flag(flgs, TR_STEALTH)) total += (2000 * plusses);
	if (have_flag(flgs, TR_SEARCH)) total += (1000 * plusses);
	if (have_flag(flgs, TR_INFRA)) total += (300 * plusses);
	if (have_flag(flgs, TR_TUNNEL)) total += (300 * plusses);
	if ((have_flag(flgs, TR_SPEED)) && (plusses > 0))
		total += (3500 * plusses);
	if ((have_flag(flgs, TR_BLOWS)) && (plusses > 0))
		total += (8000  * plusses);

	if (have_flag(flgs, TR_DISARM)) total += (1000 * plusses);
	if (have_flag(flgs, TR_SAVING)) total += (1500 * plusses);


	tmp_cost = 0;
	count = 0;
	if (have_flag(flgs, TR_CHAOTIC)) {total += 3000;count++;}
	if (have_flag(flgs, TR_VAMPIRIC)) {total += 6500;count++;}
	if (have_flag(flgs, TR_FORCE_WEAPON)) {tmp_cost += 2500;count++;}
	if (have_flag(flgs, TR_KILL_ANIMAL)) {tmp_cost += 2000;count++;}
	else if (have_flag(flgs, TR_SLAY_ANIMAL)) {tmp_cost += 1500;count++;}
	if (have_flag(flgs, TR_KILL_EVIL)) {tmp_cost += 5000;count++;}
	else if (have_flag(flgs, TR_SLAY_EVIL)) {tmp_cost += 2000;count++;}
	if (have_flag(flgs, TR_KILL_HUMAN)) {tmp_cost += 2500;count++;}
	else if (have_flag(flgs, TR_SLAY_HUMAN)) {tmp_cost += 1500;count++;}
	if (have_flag(flgs, TR_KILL_UNDEAD)) {tmp_cost += 2000;count++;}
	else if (have_flag(flgs, TR_SLAY_UNDEAD)) {tmp_cost += 1000;count++;}
	if (have_flag(flgs, TR_KILL_DEMON)) {tmp_cost += 2000;count++;}
	else if (have_flag(flgs, TR_SLAY_DEMON)) {tmp_cost += 1000;count++;}
	if (have_flag(flgs, TR_KILL_DEITY)) {tmp_cost += 3000;count++;}
	else if (have_flag(flgs, TR_SLAY_DEITY)) {tmp_cost += 2000;count++;}
	if (have_flag(flgs, TR_KILL_KWAI)) {tmp_cost += 2000;count++;}
	else if (have_flag(flgs, TR_SLAY_KWAI)) {tmp_cost += 1000;count++;}
	if (have_flag(flgs, TR_KILL_GOOD)) {tmp_cost += 3000;count++;}
	else if (have_flag(flgs, TR_SLAY_GOOD)) {tmp_cost += 1500;count++;}
	//if (have_flag(flgs, TR_KILL_ORC)) {tmp_cost += 2500;count++;}
	//else if (have_flag(flgs, TR_SLAY_ORC)) {tmp_cost += 1500;count++;}
	//if (have_flag(flgs, TR_KILL_TROLL)) {tmp_cost += 2800;count++;}
	//else if (have_flag(flgs, TR_SLAY_TROLL)) {tmp_cost += 1800;count++;}
	//if (have_flag(flgs, TR_KILL_GIANT)) {tmp_cost += 2800;count++;}
	//else if (have_flag(flgs, TR_SLAY_GIANT)) {tmp_cost += 1800;count++;}
	if (have_flag(flgs, TR_KILL_DRAGON)) {tmp_cost += 3000;count++;}
	else if (have_flag(flgs, TR_SLAY_DRAGON)) {tmp_cost += 2000;count++;}

	if (have_flag(flgs, TR_VORPAL)) {tmp_cost += 1500;count++;}
	if (have_flag(flgs, TR_EX_VORPAL)) {tmp_cost += 4500;count++;}
	if (have_flag(flgs, TR_IMPACT)) {tmp_cost += 2000;count++;}
	if (have_flag(flgs, TR_BRAND_POIS)) {tmp_cost += 3000;count++;}
	if (have_flag(flgs, TR_BRAND_ACID)) {tmp_cost += 3000;count++;}
	if (have_flag(flgs, TR_BRAND_ELEC)) {tmp_cost += 2500;count++;}
	if (have_flag(flgs, TR_BRAND_FIRE)) {tmp_cost += 2000;count++;}
	if (have_flag(flgs, TR_BRAND_COLD)) {tmp_cost += 2000;count++;}

	if(flag_junko)
		total += (tmp_cost * (count+2)/3);
	else
		total += (tmp_cost * count);

	if (have_flag(flgs, TR_SUST_STR)) total += 1500;
	if (have_flag(flgs, TR_SUST_INT)) total += 1500;
	if (have_flag(flgs, TR_SUST_WIS)) total += 1000;
	if (have_flag(flgs, TR_SUST_DEX)) total += 1000;
	if (have_flag(flgs, TR_SUST_CON)) total += 1500;
	if (have_flag(flgs, TR_SUST_CHR)) total += 800;
	if (have_flag(flgs, TR_RIDING)) total += 1000;
	if (have_flag(flgs, TR_EASY_SPELL)) total += 7000;
	if (have_flag(flgs, TR_THROW)) total += 1000;
	if (have_flag(flgs, TR_BOOMERANG)) total += 5000;
	if (have_flag(flgs, TR_SPEEDSTER)) total += 10000;
	if (have_flag(flgs, TR_GENZI)) total += 30000;

	if (have_flag(flgs, TR_RES_FEAR)) total += 2000;
	if (have_flag(flgs, TR_FREE_ACT)) total += 3000;
	if (have_flag(flgs, TR_RES_CONF)) total += 4000;
	if (have_flag(flgs, TR_RES_BLIND)) total += 5000; 
	if (have_flag(flgs, TR_RES_INSANITY)) total += 10000;


	tmp_cost = 0;
	count = 0;
	if (have_flag(flgs, TR_IM_ACID)) {tmp_cost += 10000;count += 2;}
	if (have_flag(flgs, TR_IM_ELEC)) {tmp_cost += 10000;count += 2;}
	if (have_flag(flgs, TR_IM_FIRE)) {tmp_cost += 10000;count += 2;}
	if (have_flag(flgs, TR_IM_COLD)) {tmp_cost += 10000;count += 2;}
	if (have_flag(flgs, TR_REFLECT)) {tmp_cost += 5000;count += 2;}
	if (have_flag(flgs, TR_RES_ACID)) {tmp_cost += 800;count++;}
	if (have_flag(flgs, TR_RES_ELEC)) {tmp_cost += 800;count++;}
	if (have_flag(flgs, TR_RES_FIRE)) {tmp_cost += 800;count++;}
	if (have_flag(flgs, TR_RES_COLD)) {tmp_cost += 800;count++;}
	if (have_flag(flgs, TR_RES_POIS)) {tmp_cost += 1500;count += 2;}
	if (have_flag(flgs, TR_RES_LITE)) {tmp_cost += 1000;count += 2;}
	if (have_flag(flgs, TR_RES_DARK)) {tmp_cost += 1000;count += 2;}
	if (have_flag(flgs, TR_RES_SOUND)) {tmp_cost += 1200;count += 2;}
	if (have_flag(flgs, TR_RES_SHARDS)) {tmp_cost += 1200;count += 2;}
	if (have_flag(flgs, TR_RES_NETHER)) {tmp_cost += 1500;count += 2;}
	if (have_flag(flgs, TR_RES_WATER)) {tmp_cost += 1000;count += 2;}
	if (have_flag(flgs, TR_RES_HOLY)) {tmp_cost += 2500;count += 2;}
	if (have_flag(flgs, TR_RES_TIME)) {tmp_cost += 5000;count += 2;}
	if (have_flag(flgs, TR_RES_CHAOS)) {tmp_cost += 1500;count += 2;}
	if (have_flag(flgs, TR_RES_DISEN)) {tmp_cost += 1000;count += 2;}

	if(flag_junko)
		total += (tmp_cost * (count+3)/4);
	else
		total += (tmp_cost * count);

	if (have_flag(flgs, TR_SH_FIRE)) total += 3000;
	if (have_flag(flgs, TR_SH_ELEC)) total += 3000;
	if (have_flag(flgs, TR_SH_COLD)) total += 3000;
	if (have_flag(flgs, TR_NO_TELE)) total -= 10000;
	if (have_flag(flgs, TR_NO_MAGIC)) total += 2500;
	if (have_flag(flgs, TR_TY_CURSE)) total -= 15000;
	if (have_flag(flgs, TR_HIDE_TYPE)) total += 0;
	if (have_flag(flgs, TR_SHOW_MODS)) total += 0;
	if (have_flag(flgs, TR_LEVITATION)) total += 1250;
	if (have_flag(flgs, TR_LITE)) total += 1250;
	if (have_flag(flgs, TR_SEE_INVIS)) total += 2000;
	if (have_flag(flgs, TR_TELEPATHY)) total += 20000;
	if (have_flag(flgs, TR_ESP_ANIMAL)) total += 2000;
	if (have_flag(flgs, TR_ESP_UNDEAD)) total += 2000;
	if (have_flag(flgs, TR_ESP_DEMON)) total += 2000;
	if (have_flag(flgs, TR_ESP_DEITY)) total += 2000;
	if (have_flag(flgs, TR_ESP_KWAI)) total += 2000;
	if (have_flag(flgs, TR_ESP_DRAGON)) total += 3000;
	if (have_flag(flgs, TR_ESP_HUMAN)) total += 2000;
	if (have_flag(flgs, TR_ESP_EVIL)) total += 10000;
	if (have_flag(flgs, TR_ESP_GOOD)) total += 5000;
	if (have_flag(flgs, TR_ESP_NONLIVING)) total += 5000;
	if (have_flag(flgs, TR_ESP_UNIQUE)) total += 10000;
	if (have_flag(flgs, TR_SLOW_DIGEST)) total += 750;
	if (have_flag(flgs, TR_REGEN)) total += 2500;
	if (have_flag(flgs, TR_WARNING)) total += 2000;
	if (have_flag(flgs, TR_DEC_MANA)) total += 10000;
	if (have_flag(flgs, TR_XTRA_MIGHT)) total += 2250;
	if (have_flag(flgs, TR_XTRA_SHOTS)) total += 10000;
	if (have_flag(flgs, TR_IGNORE_ACID)) total += 100;
	if (have_flag(flgs, TR_IGNORE_ELEC)) total += 100;
	if (have_flag(flgs, TR_IGNORE_FIRE)) total += 100;
	if (have_flag(flgs, TR_IGNORE_COLD)) total += 100;
	if (have_flag(flgs, TR_ACTIVATE)) total += 100;
	if (have_flag(flgs, TR_DRAIN_EXP)) total -= 12500;
	if (have_flag(flgs, TR_TELEPORT))
	{
		if (object_is_cursed(o_ptr))
			total -= 7500;
		else
			total += 250;
	}
	if (have_flag(flgs, TR_AGGRAVATE)) total -= 10000;
	if (have_flag(flgs, TR_BLESSED)) total += 750;
	if (o_ptr->curse_flags & TR_ADD_L_CURSE) total -= 5000;
	if (o_ptr->curse_flags & TR_ADD_H_CURSE) total -= 12500;
	if (o_ptr->curse_flags & TRC_CURSED) total -= 5000;
	if (o_ptr->curse_flags & TRC_HEAVY_CURSE) total -= 12500;
	if (o_ptr->curse_flags & TRC_PERMA_CURSE) total -= 15000;

	/* Also, give some extra for activatable powers... */
	if (o_ptr->art_name && (have_flag(o_ptr->art_flags, TR_ACTIVATE)))
	{
		const activation_type* const act_ptr = find_activation_info(o_ptr);
		if (act_ptr) {
			total += act_ptr->value;
		}
	}

	return total;
}


/*
 * Return the "real" price of a "known" item, not including discounts
 *
 * Wand and staffs get cost for each charge
 *
 * Armor is worth an extra 100 gold per bonus point to armor class.
 *
 * Weapons are worth an extra 100 gold per bonus point (AC,TH,TD).
 *
 * Missiles are only worth 5 gold per bonus point, since they
 * usually appear in groups of 20, and we want the player to get
 * the same amount of cash for any "equivalent" item.  Note that
 * missiles never have any of the "pval" flags, and in fact, they
 * only have a few of the available flags, primarily of the "slay"
 * and "brand" and "ignore" variety.
 *
 * Armor with a negative armor bonus is worthless.
 * Weapons with negative hit+damage bonuses are worthless.
 *
 * Every wearable item with a "pval" bonus is worth extra (see below).
 */
/*:::�A�C�e���̉��i�𔻒肷��@�Ӓ�ς݂ł��邱��*/
///item TVAL
///mod131224
s32b object_value_real(object_type *o_ptr)
{
	s32b value;

	u32b flgs[TR_FLAG_SIZE];

	object_kind *k_ptr = &k_info[o_ptr->k_idx];


	/* Hack -- "worthless" items */
	if (!k_info[o_ptr->k_idx].cost) return (0L);

	/* Base cost */
	value = k_info[o_ptr->k_idx].cost;

	/* Extract some flags */
	object_flags(o_ptr, flgs);

	/* Artifact */
	if (object_is_fixed_artifact(o_ptr))
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		/* Hack -- "worthless" artifacts */
		if (!a_ptr->cost) return (0L);

		/* Hack -- Use the artifact cost instead */
		value = a_ptr->cost;
		value += flag_cost(o_ptr, o_ptr->pval,FALSE);

		//v1.1.25 ���i��ۑ�
		if(value < 0) 
			o_ptr->score_value = 0L;
		else 
			o_ptr->score_value = value;
		/* Don't add pval bonuses etc. */
		return (value);
	}

	/* Ego-Item */
	else if (object_is_ego(o_ptr))
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Hack -- "worthless" ego-items */
		if (!e_ptr->cost) return (0L);

		/* Hack -- Reward the ego-item with a bonus */
		value += e_ptr->cost;
		value += flag_cost(o_ptr, o_ptr->pval,FALSE);
	}

	else
	{
		int i;
		bool flag = FALSE;

		for (i = 0; i < TR_FLAG_SIZE; i++) 
			if (o_ptr->art_flags[i]) flag = TRUE;

		if (flag) value += flag_cost(o_ptr, o_ptr->pval,FALSE);
	}

	/* Analyze pval bonus for normal object */
	///item pval���t�����ꂤ��TVAL
	switch (o_ptr->tval)
	{
	case TV_BULLET:
	case TV_ARROW:
	case TV_BOLT:
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
	case TV_DRAG_ARMOR:
	case TV_LITE:
	case TV_AMULET:
	case TV_RING:
	case TV_RIBBON:
		/* No pval */
		if (!o_ptr->pval) break;

		/* Hack -- Negative "pval" is always bad */
		if (o_ptr->pval < 0) return (0L);

		/* Give credit for stat bonuses */
		if (have_flag(flgs, TR_STR)) value += (o_ptr->pval * 200L);
		if (have_flag(flgs, TR_INT)) value += (o_ptr->pval * 200L);
		if (have_flag(flgs, TR_WIS)) value += (o_ptr->pval * 200L);
		if (have_flag(flgs, TR_DEX)) value += (o_ptr->pval * 200L);
		if (have_flag(flgs, TR_CON)) value += (o_ptr->pval * 200L);
		if (have_flag(flgs, TR_CHR)) value += (o_ptr->pval * 200L);

		/* Give credit for stealth and searching */
		if (have_flag(flgs, TR_MAGIC_MASTERY)) value += (o_ptr->pval * 100);
		if (have_flag(flgs, TR_STEALTH)) value += (o_ptr->pval * 100L);
		if (have_flag(flgs, TR_SEARCH)) value += (o_ptr->pval * 100L);

		if (have_flag(flgs, TR_DISARM)) value += (o_ptr->pval * 100L);
		if (have_flag(flgs, TR_SAVING)) value += (o_ptr->pval * 100L);

		/* Give credit for infra-vision and tunneling */
		if (have_flag(flgs, TR_INFRA)) value += (o_ptr->pval * 50L);
		if (have_flag(flgs, TR_TUNNEL)) value += (o_ptr->pval * 50L);

		/* Give credit for extra attacks */
		if (have_flag(flgs, TR_BLOWS)) value += (o_ptr->pval * 5000L);

		/* Give credit for speed bonus */
		if (have_flag(flgs, TR_SPEED)) value += (o_ptr->pval * 10000L);

		break;
	}


	/* Analyze the item */
	switch (o_ptr->tval)
	{
		/* Wands/Staffs */
		case TV_WAND:
		{
			/* Pay extra for charges, depending on standard number of
			 * charges.  Handle new-style wands correctly. -LM-
			 */
			value += (value * o_ptr->pval / o_ptr->number / (k_ptr->pval * 2));

			/* Done */
			break;
		}
		case TV_STAFF:
		{
			/* Pay extra for charges, depending on standard number of
			 * charges.  -LM-
			 */
			value += (value * o_ptr->pval / (k_ptr->pval * 2));

			/* Done */
			break;
		}
		//v1.1.86
		case TV_ABILITY_CARD:
		{

			if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
			{
				msg_format("ERROR:object_value_real()�ɕs���ȃJ�[�hidx(%d)���������A�r���e�B�J�[�h���n���ꂽ", o_ptr->pval);
				break;
			}
			//TODO:������������l�Ƃ��g���čČv�Z����
			switch (ability_card_list[o_ptr->pval].rarity_rank)
			{
			case 1:
				value = 1000;
				break;
			case 2:
				value = 10000;
				break;
			case 3:
				value = 100000;
				break;
			case 4:
				value = 1000000;
				break;
			default:
				value = 100;
			}

		}
		break;

		/* Rings/Amulets */
		case TV_RING:
		case TV_AMULET:
		{
			/* Hack -- negative bonuses are bad */
			if (o_ptr->to_h + o_ptr->to_d + o_ptr->to_a < 0) return (0L);

			/* Give credit for bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d + o_ptr->to_a) * 200L);

			/* Done */
			break;
		}

		/* Armor */
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HEAD:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_CLOTHES:
	case TV_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_RIBBON:
		{
			/* Hack -- negative armor bonus */
			if (o_ptr->to_a < 0) return (0L);

			/* Give credit for bonuses */
			value += (((o_ptr->to_h - k_ptr->to_h) + (o_ptr->to_d - k_ptr->to_d)) * 200L + (o_ptr->to_a) * 100L);

			/* Done */
			break;
		}

		/* Bows/Weapons */

		//v1.1.64
		//�e�̏C���l�̉��i��flag_cost�Ɉڂ��Ă������A
		//NORMAL�i�̏C���l�����f����Ă��Ȃ��̂ł����ɖ߂���
	case TV_GUN:

		//�e�̓��ꔭ��
		if (o_ptr->xtra2 == ACT_GUN_VARIABLE)
		{
			int test_dam;

			test_dam = o_ptr->dd * (o_ptr->ds + 1) / 2 + o_ptr->to_d;

			switch (o_ptr->xtra1)
			{
			case GF_FIRE:case GF_POIS:case GF_ELEC:case GF_COLD:case GF_ACID:
				test_dam /= 2; break;
			case GF_NUKE:case GF_PLASMA:case GF_ICE:case GF_NETHER:
				test_dam -= test_dam / 3; break;
			case GF_LITE: case GF_DARK: case GF_SHARDS: case GF_DISENCHANT: case GF_CHAOS:
				test_dam -= test_dam / 6; break;

			case GF_PSY_SPEAR: case GF_WATER: case GF_HOLY_FIRE: case GF_SOUND: case GF_FORCE: case GF_HELL_FIRE:
				test_dam += test_dam / 3; break;
			}

			switch (o_ptr->xtra4)
			{
			case GUN_FIRE_MODE_BEAM: case GUN_FIRE_MODE_ROCKET:
				test_dam += test_dam / 3; break;
			case GUN_FIRE_MODE_BREATH: case GUN_FIRE_MODE_SPARK:
				test_dam += test_dam / 2; break;

			}
			//v1.1.30
			//test_dam /= (gun_base_param_table[o_ptr->sval].charge_turn / 4 + 1);
			//test_dam += test_dam * o_ptr->to_h / 50;
			//if(test_dam > 0) total += test_dam * 250;

			//�`���[�W���Ԃ̒����ɉ����ĉ��l����
			test_dam = test_dam * 3 / (gun_base_param_table[o_ptr->sval].charge_turn + 1);
			//�e�q�������Ɖ��l����
			test_dam += (test_dam * gun_base_param_table[o_ptr->sval].bullets - 1) / 7;
			test_dam += test_dam * o_ptr->to_h / 100;
			//����͉��i����
			if (object_is_nameless(o_ptr)) test_dam /= 2;
			if (test_dam > 0) value += test_dam * 200;
		}

		break;

	case TV_BOW:
	case TV_CROSSBOW:

	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_OTHERWEAPON:	
		{
			/* Hack -- negative hit/damage bonuses */
			if (o_ptr->to_h + o_ptr->to_d < 0) return (0L);

			/* Factor in the bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d + o_ptr->to_a) * 100L);

			/* Hack -- Factor in extra damage dice and sides */
			value += (o_ptr->dd - k_ptr->dd) * o_ptr->ds * 250L;
			value += (o_ptr->ds - k_ptr->ds) * o_ptr->dd * 250L;

			/* Done */
			break;
		}

		/* Ammo */
	case TV_BULLET:
	case TV_ARROW:
	case TV_BOLT:
		{
			/* Hack -- negative hit/damage bonuses */
			if (o_ptr->to_h + o_ptr->to_d < 0) return (0L);

			/* Factor in the bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d) * 5L);

			/* Hack -- Factor in extra damage dice and sides */
			value += (o_ptr->dd - k_ptr->dd) * o_ptr->ds * 5L;
			value += (o_ptr->ds - k_ptr->ds) * o_ptr->dd * 5L;

			/* Done */
			break;
		}

		/* Figurines, relative to monster level */
		case TV_FIGURINE:
		{
			int level = r_info[o_ptr->pval].level;
			if (level < 20) value = level*50L;
			else if (level < 30) value = 1000+(level-20)*150L;
			else if (level < 40) value = 2500+(level-30)*350L;
			else if (level < 50) value = 6000+(level-40)*800L;
			else value = 14000+(level-50)*2000L;
			break;
		}

		case TV_CAPTURE:
		{
			if (!o_ptr->pval) return 1000L;
			else
			{
				int level = r_info[o_ptr->pval].level;
				if (level < 20) return 1000 + level*50L;
				else if (level < 30) return 2000+(level-20)*150L;
				else if (level < 40) return 3500+(level-30)*350L;
				else if (level < 50) return 7000+(level-40)*800L;
				else return 15000+(level-50)*2000L;
			}
			break;
		}

		case TV_CHEST:
		{
			if (!o_ptr->pval) value = 0L;
			break;
		}
		//default:
		//value = 1L;
	}

	//�u�j�v�͉��i1/10
	//v2.0.16 ����1/10�̂܂܂ɂ���
	if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE ) 
	{
		value /= 10;
		if(value < k_info[o_ptr->k_idx].cost) value = k_info[o_ptr->k_idx].cost;
	}
	//v2.0.16 ��́���1/10�ɂ���
	else if (object_is_needle_arrow_bolt(o_ptr) && object_is_artifact(o_ptr))
	{
		value /= 10;
		if (value < k_info[o_ptr->k_idx].cost) value = k_info[o_ptr->k_idx].cost;
	}

	//v1.1.60 ������W�i�̓t���O�{�[�i�X��啝�ɍ�������
	if (o_ptr->tval == TV_ANTIQUE && value > 0)
	{

		value = k_info[o_ptr->k_idx].cost + (value - k_info[o_ptr->k_idx].cost) * 100 * o_ptr->sval;

		if (value > 99999999) value = 99999999;

	}

	//v1.1.25 ���i��ۑ�
	if(value < 0) 
		o_ptr->score_value = 0L;
	else 
		o_ptr->score_value = value;

	/* Worthless object */
	if (value < 0) return 0L;




	/* Return the value */
	return (value);
}


/*
 * Return the price of an item including plusses (and charges)
 *
 * This function returns the "value" of the given item (qty one)
 *
 * Never notice "unknown" bonuses or properties, including "curses",
 * since that would give the player information he did not have.
 *
 * Note that discounted items stay discounted forever, even if
 * the discount is "forgotten" by the player via memory loss.
 */
/*:::�A�C�e���̒l�i���v�Z*/
s32b object_value(object_type *o_ptr)
{
	s32b value;


	/* Unknown items -- acquire a base value */
	if (object_is_known(o_ptr))
	{
		/* Broken items -- worthless */
		if (object_is_broken(o_ptr)) return (0L);

		/* Cursed items -- worthless */
		if (object_is_cursed(o_ptr)) return (0L);

		/* Real value (see above) */
		value = object_value_real(o_ptr);
	}

	/* Known items -- acquire the actual value */
	else
	{
		/* Hack -- Felt broken items */
		if ((o_ptr->ident & (IDENT_SENSE)) && object_is_broken(o_ptr)) return (0L);

		/* Hack -- Felt cursed items */
		if ((o_ptr->ident & (IDENT_SENSE)) && object_is_cursed(o_ptr)) return (0L);

		/* Base value (see above) */
		value = object_value_base(o_ptr);
	}


	/* Apply discount (if any) */
	if (o_ptr->discount) value -= (value * o_ptr->discount / 100L);


	/* Return the final value */
	return (value);
}


/*
 * Determines whether an object can be destroyed, and makes fake inscription.
 */
/*:::�A�C�e����j��\������@���Ӓ�Ȃ�(���ʐ�)��(���낵��)�̊ȈՊӒ������*/
///item �A�C�e���j�󔻒� PDSM���󂹂Ȃ��悤�ɂ��Ă�����
bool can_player_destroy_object(object_type *o_ptr)
{
	/* Artifacts cannot be destroyed */
	if (!object_is_artifact(o_ptr)) return TRUE;

	/* If object is unidentified, makes fake inscription */
	if (!object_is_known(o_ptr))
	{
		byte feel = FEEL_SPECIAL;

		/* Hack -- Handle icky artifacts */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) feel = FEEL_TERRIBLE;

		/* Hack -- inscribe the artifact */
		o_ptr->feeling = feel;

		/* We have "felt" it (again) */
		o_ptr->ident |= (IDENT_SENSE);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return FALSE;
	}

	/* Identified artifact -- Nothing to do */
	return FALSE;
}


/*
 * Distribute charges of rods or wands.
 *
 * o_ptr = source item
 * q_ptr = target item, must be of the same type as o_ptr
 * amt   = number of items that are transfered
 */
/*:::���@�_�ƃ��b�h�𕪂����Ƃ��ɏ[�U����[�U���Ԃ������@�ڍז���*/
void distribute_charges(object_type *o_ptr, object_type *q_ptr, int amt)
{
	/*
	 * Hack -- If rods or wands are dropped, the total maximum timeout or
	 * charges need to be allocated between the two stacks.  If all the items
	 * are being dropped, it makes for a neater message to leave the original
	 * stack's pval alone. -LM-
	 */
	if ((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_ROD))
	{
		//���b�h��pval�l�͏������Ă���{�����̎g�p���`���[�W���ԍ��v�Ȃ̂Ń��b�h�𕪂���Ƃ���pval��������B
		//�Ȃ�����Ȍv�Z�̎d���ɂ��Ă���̂��͕������B

		q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
		if (amt < o_ptr->number) o_ptr->pval -= q_ptr->pval;

		/* Hack -- Rods also need to have their timeouts distributed.  The
		 * dropped stack will accept all time remaining to charge up to its
		 * maximum.
		 */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			if (q_ptr->pval > o_ptr->timeout)
				q_ptr->timeout = o_ptr->timeout;
			else
				q_ptr->timeout = q_ptr->pval;

			if (amt < o_ptr->number) o_ptr->timeout -= q_ptr->timeout;
		}
	}
	//v1.1.86 �A�r���e�B�J�[�h�̏[�U�֌W����
	if (o_ptr->tval == TV_ABILITY_CARD)
	{
		if (o_ptr->timeout)
		{
			int base_charge_time = ability_card_list[o_ptr->pval].charge_turn;

			if (base_charge_time * amt > o_ptr->timeout)
				q_ptr->timeout = o_ptr->timeout;
			else
				q_ptr->timeout = base_charge_time * amt;

			if (amt < o_ptr->number) o_ptr->timeout -= q_ptr->timeout;
		}
	}




}

void reduce_charges(object_type *o_ptr, int amt)
{
	/*
	 * Hack -- If rods or wand are destroyed, the total maximum timeout or
	 * charges of the stack needs to be reduced, unless all the items are
	 * being destroyed. -LM-
	 */
	if (((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_ROD)) &&
		(amt < o_ptr->number))
	{
		o_ptr->pval -= o_ptr->pval * amt / o_ptr->number;
	}
}


/*
 * Determine if an item can "absorb" a second item
 *
 * See "object_absorb()" for the actual "absorption" code.
 *
 * If permitted, we allow staffs (if they are known to have equal charges
 * and both are either known or confirmed empty) and wands (if both are
 * either known or confirmed empty) and rods (in all cases) to combine.
 * Staffs will unstack (if necessary) when they are used, but wands and
 * rods will only unstack if one is dropped. -LM-
 *
 * If permitted, we allow weapons/armor to stack, if fully "known".
 *
 * Missiles will combine if both stacks have the same "known" status.
 * This is done to make unidentified stacks of missiles useful.
 *
 * Food, potions, scrolls, and "easy know" items always stack.
 *
 * Chests, and activatable items, never stack (for various reasons).
 */

/*
 * A "stack" of items is limited to less than or equal to 99 items (hard-coded).
 */
#define MAX_STACK_SIZE 99


/*
 *  Determine if an item can partly absorb a second item.
 *  Return maximum number of stack.
 */
/*:::�A�C�e�����܂Ƃ߂��邩�ǂ������肷��⏕�֐�*/
//2�̃A�C�e�����܂Ƃ߂��Ȃ��Ƃ�0�A�܂Ƃ߂���Ƃ��X�^�b�N�ł���ő�l(����99,�ꕔ���b�h��timeout���I�[�o�[�t���[���Ȃ��l)��Ԃ��B
//��̃A�C�e�����X�^�b�N�ő�l�Ɏ��܂邩�ǂ����̔���͂��̊֐��̊O�ōs��
int object_similar_part(object_type *o_ptr, object_type *j_ptr)
{
	int i;

	/* Default maximum number of stack */
	int max_num = MAX_STACK_SIZE;

	/* Require identical object types */
	if (o_ptr->k_idx != j_ptr->k_idx) return 0;

	//v2.0.19 �Е����Œ聚�Ȃ�܂Ƃ߂��Ȃ��悤�ɂ���@���́����܂Ƃ߂̑ΏۂɂȂ��Ă��܂����̂ŏ������ǉ�
	if (object_is_fixed_artifact(o_ptr) || object_is_fixed_artifact(j_ptr)) return 0;

	/* Analyze the items */
	switch (o_ptr->tval)
	{
		/* Chests and Statues*/
		case TV_CHEST:
		//case TV_CARD:
		case TV_CAPTURE:
		case TV_MAGICITEM:
		case TV_MACHINE:
		case TV_SOUVENIR:
		case TV_STRANGE_OBJ:
		case TV_SPELLCARD:
		case TV_MASK:
		{
			/* Never okay */
			return 0;
		}
///del131221 ���̂Ƒ�
#if 0

		case TV_STATUE:
		{
			if ((o_ptr->sval != SV_PHOTO) || (j_ptr->sval != SV_PHOTO)) return 0;
			if (o_ptr->pval != j_ptr->pval) return 0;
			break;
		}
#endif
		/* Figurines and Corpses*/


		///mod151213�V���ǉ� pval��r_idx���i�[
		case TV_BUNBUN:
		case TV_KAKASHI:
		case TV_FIGURINE:
		case TV_ITEMCARD:
		///del131221 ���̂Ƒ�
		//case TV_CORPSE:
		{
			/* Same monster */
			if (o_ptr->pval != j_ptr->pval) return 0;

			/* Assume okay */
			break;
		}
		case TV_PHOTO:
		{
			if (o_ptr->sval != j_ptr->sval)
				return 0;
			if (o_ptr->sval == SV_PHOTO_NIGHTMARE && j_ptr->sval == SV_PHOTO_NIGHTMARE)//v1.1.51 �V�A���[�i
			{
				if (o_ptr->art_name != j_ptr->art_name) return 0;
				if (o_ptr->xtra4 != j_ptr->xtra4) return 0;
				if (o_ptr->xtra5 != j_ptr->xtra5) return 0;

			}
			else
			{
				/* Same monster */
				if (o_ptr->pval != j_ptr->pval) return 0;
			}

			/* Assume okay */
			break;
		}



		/* Food and Potions and Scrolls */
		case TV_FOOD:
		case TV_POTION:
		case TV_SCROLL:
		case TV_MATERIAL:
		case TV_MUSHROOM:
		case TV_SOFTDRINK:
		case TV_ALCOHOL:
		case TV_SWEETS:
		{
			/* Assume okay */
			break;
		}

		/* Staffs */
		case TV_STAFF:
		{
			/* Require either knowledge or known empty for both staffs. */
			if ((!(o_ptr->ident & (IDENT_EMPTY)) &&
				!object_is_known(o_ptr)) ||
				(!(j_ptr->ident & (IDENT_EMPTY)) &&
				!object_is_known(j_ptr))) return 0;

			/* Require identical charges, since staffs are bulky. */
			if (o_ptr->pval != j_ptr->pval) return 0;

			/* Assume okay */
			break;
		}

		/* Wands */
		case TV_WAND:
		{
			/* Require either knowledge or known empty for both wands. */
			if ((!(o_ptr->ident & (IDENT_EMPTY)) &&
				!object_is_known(o_ptr)) ||
				(!(j_ptr->ident & (IDENT_EMPTY)) &&
				!object_is_known(j_ptr))) return 0;

			/* Wand charges combine in O&ZAngband.  */

			/* Assume okay */
			break;
		}

		/* Staffs and Wands and Rods */
		case TV_ROD:
		{
			/* Prevent overflaw of timeout */
			max_num = MIN(max_num, MAX_SHORT / k_info[o_ptr->k_idx].pval);

			/* Assume okay */
			break;
		}
		case TV_ABILITY_CARD:
		{
			/*�A�r���e�B�J�[�h��sval�łȂ�pval�l�Ŏ�ނ𕪂��Ă���*/
			if (o_ptr->pval != j_ptr->pval) return 0;

			max_num = ABL_CARD_MAX_STACK_SIZE;
			break;

		}

		/* Weapons and Armor */
		case TV_BOW:
		case TV_CROSSBOW:
		case TV_GUN:

	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HEAD:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_CLOTHES:
	case TV_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_RIBBON:
	case TV_MAGICWEAPON:

		/* Rings, Amulets, Lites */
		case TV_RING:
		case TV_AMULET:
		case TV_LITE:
		case TV_WHISTLE:
		{
			/* Require full knowledge of both items */
			if (!object_is_known(o_ptr) || !object_is_known(j_ptr)) return 0;

			/* Fall through */
		}


		//v1.1.62 �i�C�t�ނƂ��̑�����(�j��z��)�͖��Ӓ�ł��܂Ƃ߂���悤�ɂ����B
		//�ނ�Ƃ��܂Ƃ܂��Ă��܂����ǂ��Ƃ���
		case TV_KNIFE:
		case TV_OTHERWEAPON:
		/* Missiles */
		case TV_BOLT:
		case TV_ARROW:
		case TV_BULLET:
		{
			/* Require identical knowledge of both items */
			if (object_is_known(o_ptr) != object_is_known(j_ptr)) return 0;

			if (o_ptr->feeling != j_ptr->feeling) return 0;

			/* Require identical "bonuses" */
			if (o_ptr->to_h != j_ptr->to_h) return 0;
			if (o_ptr->to_d != j_ptr->to_d) return 0;
			if (o_ptr->to_a != j_ptr->to_a) return 0;

			/* Require identical "pval" code */
			if (o_ptr->pval != j_ptr->pval) return 0;

			/* Require identical "ego-item" names */
			if (o_ptr->name2 != j_ptr->name2) return 0;

			/* Require identical added essence  */
			if (o_ptr->xtra3 != j_ptr->xtra3) return 0;
			if (o_ptr->xtra4 != j_ptr->xtra4) return 0;

			///mod160505 �e�͎ˌ�������xtra1���g���̂œ����Ȃ�OK�Ƃ���
			if(o_ptr->tval == TV_GUN)
			{
				if (o_ptr->xtra1 != j_ptr->xtra1) return 0;
			}
			else
			{
				/* Hack -- Never stack "powerful" items */
				if (o_ptr->xtra1 || j_ptr->xtra1) return 0;
			}

			/* Hack -- Never stack recharging items */
			if (o_ptr->timeout || j_ptr->timeout) return 0;

			/* Require identical "values" */
			if (o_ptr->ac != j_ptr->ac) return 0;
			if (o_ptr->dd != j_ptr->dd) return 0;
			if (o_ptr->ds != j_ptr->ds) return 0;

			//v2.0.16 ��A�{���g�A�j�́��ł��ݐω\�ɂ���
			if (object_is_artifact(o_ptr) || object_is_artifact(j_ptr))
			{
				if (!object_is_needle_arrow_bolt(o_ptr)) return 0;
				if (object_is_fixed_artifact(o_ptr) || object_is_fixed_artifact(j_ptr))	return 0;

				//�����������Ȃ瓯�ꎋ����B�A�C�e���t���O�ƎE�C�l�����Ń`�F�b�N�����̂ő����Փ˂͂Ȃ����낤
				if (o_ptr->art_name != j_ptr->art_name) return 0;
			}

			/* Probably okay */
			break;
		}

		/* Various */
		default:
		{
			/* Require knowledge */
			if (!object_is_known(o_ptr) || !object_is_known(j_ptr)) return 0;

			/* Probably okay */
			break;
		}
	}


	/* Hack -- Identical art_flags! */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		if (o_ptr->art_flags[i] != j_ptr->art_flags[i]) return 0;

	/* Hack -- Require identical "cursed" status */
	if (o_ptr->curse_flags != j_ptr->curse_flags) return 0;

	/* Hack -- Require identical "broken" status */
	if ((o_ptr->ident & (IDENT_BROKEN)) != (j_ptr->ident & (IDENT_BROKEN))) return 0;


	/* Hack -- require semi-matching "inscriptions" */
	if (o_ptr->inscription && j_ptr->inscription &&
	    (o_ptr->inscription != j_ptr->inscription))
		return 0;

	/* Hack -- normally require matching "inscriptions" */
	if (!stack_force_notes && (o_ptr->inscription != j_ptr->inscription)) return 0;

	/* Hack -- normally require matching "discounts" */
	if (!stack_force_costs && (o_ptr->discount != j_ptr->discount)) return 0;


	/* They match, so they must be similar */
	return max_num;
}

/*
 *  Determine if an item can absorb a second item.
 */
/*:::�A�C�e�����܂Ƃ߂��邩����*/
bool object_similar(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;
	int max_num;

	/* Are these objects similar? */
	max_num = object_similar_part(o_ptr, j_ptr);

	/* Return if not similar */
	if (!max_num) return FALSE;

	/* Maximal "stacking" limit */
	if (total > max_num) return (0);


	/* They match, so they must be similar */
	return (TRUE);
}



/*
 * Allow one item to "absorb" another, assuming they are similar
 */
/*:::�A�C�e�����܂Ƃ߂�B�ŏ��̃A�C�e���Ɏ��̃A�C�e�����܂Ƃ߂���*/
void object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int max_num = object_similar_part(o_ptr, j_ptr);
	int total = o_ptr->number + j_ptr->number;
	int diff = (total > max_num) ? total - max_num : 0;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > max_num) ? max_num : total;

	/* Hack -- blend "known" status */
	if (object_is_known(j_ptr)) object_known(o_ptr);

	/* Hack -- clear "storebought" if only one has it */
	if (((o_ptr->ident & IDENT_STORE) || (j_ptr->ident & IDENT_STORE)) &&
	    (!((o_ptr->ident & IDENT_STORE) && (j_ptr->ident & IDENT_STORE))))
	{
		if (j_ptr->ident & IDENT_STORE) j_ptr->ident &= 0xEF;
		if (o_ptr->ident & IDENT_STORE) o_ptr->ident &= 0xEF;
	}

	/* Hack -- blend "mental" status */
	if (j_ptr->ident & (IDENT_MENTAL)) o_ptr->ident |= (IDENT_MENTAL);

	/* Hack -- blend "inscriptions" */
	if (j_ptr->inscription) o_ptr->inscription = j_ptr->inscription;

	/* Hack -- blend "feelings" */
	if (j_ptr->feeling) o_ptr->feeling = j_ptr->feeling;

	/* Hack -- could average discounts XXX XXX XXX */
	/* Hack -- save largest discount XXX XXX XXX */
	if (o_ptr->discount < j_ptr->discount) o_ptr->discount = j_ptr->discount;

	/* Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM- */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;
		o_ptr->timeout += j_ptr->timeout * (j_ptr->number - diff) / j_ptr->number;
	}

	//v1.1.86
	if (o_ptr->tval == TV_ABILITY_CARD)
	{
		o_ptr->timeout += j_ptr->timeout * (j_ptr->number - diff) / j_ptr->number;
		o_ptr->xtra3 = MAX(o_ptr->xtra3, j_ptr->xtra3);//xtra3������ꍇ�傫�������L�^����
	}

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if (o_ptr->tval == TV_WAND)
	{
		o_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;
	}
}


/*
 * Find the index of the object_kind with the given tval and sval
 */
/*:::�A�C�e���̕��ށA�T�u���ނ���k_info[]�ɑΉ�����idx�l��Ԃ�*/
/*:::�߂�l�Fk_info.txt��N�l�@sval��SV_ANY�ɂ����tval����v����A�C�e���̒����烉���_���ȃA�C�e����IDX���A��*/
/*:::tval:�A�C�e���̕��ޔԍ� �Ⴆ�Ώd�Z�Ȃ�37 defines.h�ɒ�`����k_info.txt�ɋL�^����Ă���*/
/*:::sval:�A�C�e���̃T�u���ޔԍ� defines.h�ɑ�ʂɒ�`����Ă���B  */
s16b lookup_kind(int tval, int sval)
{
	int k;
	int num = 0;
	int bk = 0;

	/* Look for it */
	for (k = 1; k < max_k_idx; k++)
	{
		object_kind *k_ptr = &k_info[k];

		/* Require correct tval */
		if (k_ptr->tval != tval) continue;

		/* Found a match */
		if (k_ptr->sval == sval) return (k);

		/* Ignore illegal items */
		if (sval != SV_ANY) continue;

		/* Apply the randomizer */
		/*:::tval����v����sval����v����255�ł��Ȃ��Ƃ������ɗ���͂��B���̏�����bk�̎g�p�ɉ��̈Ӗ�������̂��H*/
		/*:::��sval��SV_ANY��ݒ肵���Ƃ��Y���A�C�e���̒����烉���_���ȃA�C�e����Ԃ������炵��*/
		if (!one_in_(++num)) continue;

		/* Use this value */
		bk = k;
	}

	/* Return this choice */
	if (sval == SV_ANY)
	{
		return bk;
	}

#if 0
	/* Oops */
#ifdef JP
	msg_format("�A�C�e�����Ȃ� (%d,%d)", tval, sval);
#else
	msg_format("No object (%d,%d)", tval, sval);
#endif
#endif


	/* Oops */
	return (0);
}


/*
 * Wipe an object clean.
 */
void object_wipe(object_type *o_ptr)
{
	/* Wipe the structure */
	(void)WIPE(o_ptr, object_type);
}

/*
 * Prepare an object based on an existing object
 */
//j_ptr����o_ptr�փR�s�[�����
void object_copy(object_type *o_ptr, object_type *j_ptr)
{
	/* Copy the structure */
	(void)COPY(o_ptr, j_ptr, object_type);
}


/*
 * Prepare an object based on an object kind.
 */
/*:::k_idx�Ŏw�肵����ނ̃A�C�e���̊�{����o_ptr�ɃR�s�[����B�G�S�A���̔���͂����ł͍s��Ȃ�*/
void object_prep(object_type *o_ptr, int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Clear the record */
	object_wipe(o_ptr);

	/* Save the kind index */
	o_ptr->k_idx = k_idx;

	/* Efficiency -- tval/sval */
	o_ptr->tval = k_ptr->tval;
	o_ptr->sval = k_ptr->sval;

	/* Default "pval" */
	o_ptr->pval = k_ptr->pval;

	/* Default number */
	o_ptr->number = 1;

	/* Default weight */
	o_ptr->weight = k_ptr->weight;

	/* Default magic */
	o_ptr->to_h = k_ptr->to_h;
	o_ptr->to_d = k_ptr->to_d;
	o_ptr->to_a = k_ptr->to_a;

	/* Default power */
	o_ptr->ac = k_ptr->ac;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Default activation */
	if (k_ptr->act_idx > 0) o_ptr->xtra2 = k_ptr->act_idx;

	/* Hack -- worthless items are always "broken" */
	if (k_info[o_ptr->k_idx].cost <= 0) o_ptr->ident |= (IDENT_BROKEN);

	/* Hack -- cursed items are always "cursed" */
	if (k_ptr->gen_flags & (TRG_CURSED)) o_ptr->curse_flags |= (TRC_CURSED);
	if (k_ptr->gen_flags & (TRG_HEAVY_CURSE)) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
	if (k_ptr->gen_flags & (TRG_PERMA_CURSE)) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
	if (k_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
	if (k_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
	if (k_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);

	///mod160503 �e�d�l�ύX
	if(o_ptr->tval == TV_GUN)
	{
		//xtra1��GF_*�̑������i�[
		o_ptr->xtra1 = gun_base_param_table[o_ptr->sval].gf_type;
		//xtra4��project()�̃^�C�v���i�[
		o_ptr->xtra4 = gun_base_param_table[o_ptr->sval].project_type;
	}
	//v1.1.25 ���莞���x���ƃ^�[�����L�^
	o_ptr->create_turn = turn;
	o_ptr->create_lev = dun_level;

}


/*
 * Help determine an "enchantment bonus" for an object.
 *
 * To avoid floating point but still provide a smooth distribution of bonuses,
 * we simply round the results of division in such a way as to "average" the
 * correct floating point value.
 *
 * This function has been changed.  It uses "randnor()" to choose values from
 * a normal distribution, whose mean moves from zero towards the max as the
 * level increases, and whose standard deviation is equal to 1/4 of the max,
 * and whose values are forced to lie between zero and the max, inclusive.
 *
 * Since the "level" rarely passes 100 before Morgoth is dead, it is very
 * rare to get the "full" enchantment on an object, even a deep levels.
 *
 * It is always possible (albeit unlikely) to get the "full" enchantment.
 *
 * A sample distribution of values from "m_bonus(10, N)" is shown below:
 *
 *   N       0     1     2     3     4     5     6     7     8     9    10
 * ---    ----  ----  ----  ----  ----  ----  ----  ----  ----  ----  ----
 *   0   66.37 13.01  9.73  5.47  2.89  1.31  0.72  0.26  0.12  0.09  0.03
 *   8   46.85 24.66 12.13  8.13  4.20  2.30  1.05  0.36  0.19  0.08  0.05
 *  16   30.12 27.62 18.52 10.52  6.34  3.52  1.95  0.90  0.31  0.15  0.05
 *  24   22.44 15.62 30.14 12.92  8.55  5.30  2.39  1.63  0.62  0.28  0.11
 *  32   16.23 11.43 23.01 22.31 11.19  7.18  4.46  2.13  1.20  0.45  0.41
 *  40   10.76  8.91 12.80 29.51 16.00  9.69  5.90  3.43  1.47  0.88  0.65
 *  48    7.28  6.81 10.51 18.27 27.57 11.76  7.85  4.99  2.80  1.22  0.94
 *  56    4.41  4.73  8.52 11.96 24.94 19.78 11.06  7.18  3.68  1.96  1.78
 *  64    2.81  3.07  5.65  9.17 13.01 31.57 13.70  9.30  6.04  3.04  2.64
 *  72    1.87  1.99  3.68  7.15 10.56 20.24 25.78 12.17  7.52  4.42  4.62
 *  80    1.02  1.23  2.78  4.75  8.37 12.04 27.61 18.07 10.28  6.52  7.33
 *  88    0.70  0.57  1.56  3.12  6.34 10.06 15.76 30.46 12.58  8.47 10.38
 *  96    0.27  0.60  1.25  2.28  4.30  7.60 10.77 22.52 22.51 11.37 16.53
 * 104    0.22  0.42  0.77  1.36  2.62  5.33  8.93 13.05 29.54 15.23 22.53
 * 112    0.15  0.20  0.56  0.87  2.00  3.83  6.86 10.06 17.89 27.31 30.27
 * 120    0.03  0.11  0.31  0.46  1.31  2.48  4.60  7.78 11.67 25.53 45.72
 * 128    0.02  0.01  0.13  0.33  0.83  1.41  3.24  6.17  9.57 14.22 64.07
 */
/*:::max������Ƃ�level�������قǏ���l�̏o�₷���߂�l��Ԃ�*/
s16b m_bonus(int max, int level)
{
	int bonus, stand, extra, value;


	/* Paranoia -- enforce maximal "level" */
	if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;


	/* The "bonus" moves towards the max */
	/*:::max=2/level=100�Ȃ�bonus=1*/
	bonus = ((max * level) / MAX_DEPTH);

	/* Hack -- determine fraction of error */
	/*:::max=2/level=100�Ȃ�extra=72*/
	extra = ((max * level) % MAX_DEPTH);

	/* Hack -- simulate floating point computations */
	/*:::max=2/level=100�Ȃ�72/128�̊m����bonus+1*/
	if (randint0(MAX_DEPTH) < extra) bonus++;

	/*:::������max=2/level=100�Ȃ�stand=0  2/4�̊m����1 */
	/* The "stand" is equal to one quarter of the max */
	stand = (max / 4);

	/* Hack -- determine fraction of error */
	extra = (max % 4);

	/* Hack -- simulate floating point computations */
	if (randint0(4) < extra) stand++;


	/* Choose an "interesting" value */
	value = randnor(bonus, stand);

	/* Enforce the minimum value */
	if (value < 0) return (0);

	/* Enforce the maximum value */
	if (value > max) return (max);

	/* Result */
	return (value);
}


/*
 * Cheat -- describe a created object for the user
 */
static void object_mention(object_type *o_ptr)
{
	char o_name[MAX_NLEN];

	/* Describe */
	object_desc(o_name, o_ptr, (OD_NAME_ONLY | OD_STORE));

	/* Artifact */
	if (object_is_fixed_artifact(o_ptr))
	{
		/* Silly message */
#ifdef JP
		msg_format("�`���̃A�C�e�� (%s)", o_name);
#else
		msg_format("Artifact (%s)", o_name);
#endif

	}

	/* Random Artifact */
	else if (o_ptr->art_name)
	{
#ifdef JP
		msg_print("�����_���E�A�[�e�B�t�@�N�g");
#else
		msg_print("Random artifact");
#endif

	}

	/* Ego-item */
	else if (object_is_ego(o_ptr))
	{
		/* Silly message */
#ifdef JP
		msg_format("���̂���A�C�e�� (%s)", o_name);
#else
		msg_format("Ego-item (%s)", o_name);
#endif

	}

	/* Normal item */
	else
	{
		/* Silly message */
#ifdef JP
		msg_format("�A�C�e�� (%s)", o_name);
#else
		msg_format("Object (%s)", o_name);
#endif

	}
}

/*:::21%���ʑϐ�10%�őϐ�69%��ʑϐ� 1/2�ł���ɒǉ�*/
void dragon_resist(object_type * o_ptr)
{
	do
	{
		if (one_in_(4))
			one_dragon_ele_resistance(o_ptr);
		else
			one_high_resistance(o_ptr);
	}
	while (one_in_(2));
}



/*
 * Mega-Hack -- Attempt to create one of the "Special Objects"
 *
 * We are only called from "make_object()", and we assume that
 * "apply_magic()" is called immediately after we return.
 *
 * Note -- see "make_artifact()" and "apply_magic()"
 */
/*:::�����_���A�C�e��������SpecialObjects�����ʂ����ꍇ�����ɗ���*/
/*:::Special Objects�Ƃ́A�N�����]����R���̕�΂ȂǕK���A�[�e�B�t�@�N�g�ɂȂ�x�[�X�A�C�e���̂��ƁB*/
static bool make_artifact_special(object_type *o_ptr)
{
	int i;
	int k_idx = 0;


	/* No artifacts in the town */
	if (!dun_level) return (FALSE);

	/* Themed object */
	/*:::get_obj_num_hook�ɉ����֐����ݒ肳��Ă����炱�̊֐��͉��������I������炵���B*/
	if (get_obj_num_hook) return (FALSE);

	/* Check the artifact list (just the "specials") */
	/*:::�Œ�A�[�e�B�t�@�N�g�̔��� �A�[�e�B�t�@�N�g�̃��X�g�̏ォ�珇�ɔ��肷��*/
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Cannot make an artifact twice */
		/*:::���ɐ����ς݂́��͏o�Ȃ�*/
		if (a_ptr->cur_num) continue;

		/*:::�N�G�X�g�A�C�e���t���O�́��͂����ł͏o�Ȃ�*/
		if (a_ptr->gen_flags & TRG_QUESTITEM) continue;
	
		/*:::�N�����]����ފƂȂǁu�K���A�[�e�B�t�@�N�g�ɂȂ�x�[�X�A�C�e���v�ȊO�̃A�[�e�B�t�@�N�g�͂����ł͈���Ȃ�*/
		if (!(a_ptr->gen_flags & TRG_INSTA_ART)) continue;

		//v1.1.18 �e���o�����Ȃ��I�v�V���������Y��ǉ�
		if(a_ptr->tval == TV_GUN && gun_free_world) continue;

		/* XXX XXX Enforce minimum "depth" (loosely) */
		/*:::���݊K���x���ȏ�̃A�[�e�B�t�@�N�g�� 1/((�A�[�e�B�t�@�N�g���x��-�K���x��)*2)�̃`�F�b�N��ʂ�Ȃ��Ɛ�������Ȃ�*/
		if (a_ptr->level > dun_level)
		{
			/* Acquire the "out-of-depth factor" */
			int d = (a_ptr->level - dun_level) * 2;

			/* Roll for out-of-depth creation */
			if (!one_in_(d)) continue;
		}

		/* Artifact "rarity roll" */
		/*:::1/���A���e�B��ʂ�Ȃ��Ɛ�������Ȃ�*/
		if (!one_in_(a_ptr->rarity)) continue;

		/* Find the base object */
		k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);


		/* XXX XXX Enforce minimum "object" level (loosely) */
		/*:::���݊K���x���ȏ�̃x�[�X�̃A�[�e�B�t�@�N�g�� 1/((�x�[�X���x��-�K���x��)*5)�̃`�F�b�N��ʂ�Ȃ��Ɛ�������Ȃ�*/
		if (k_info[k_idx].level > object_level)
		{
			/* Acquire the "out-of-depth factor" */
			int d = (k_info[k_idx].level - object_level) * 5;

			/* Roll for out-of-depth creation */
			if (!one_in_(d)) continue;
		}

		/* Assign the template */
		/*:::�x�[�X�A�C�e���̏����R�s�[*/
		object_prep(o_ptr, k_idx);

		/* Mega-Hack -- mark the item as an artifact */
		o_ptr->name1 = i;

		/* Hack: Some artifacts get random extra powers */
		/*:::�ǉ��ϐ�������ꍇ�K�p*/
		random_artifact_resistance(o_ptr, a_ptr);

		/*:::�A�[�e�B�t�@�N�g���̂̐��\�͂��̂���apply_magic()�ŕt�^�����*/
		/* Success */
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}


/*
 * Attempt to change an object into an artifact
 *
 * This routine should only be called by "apply_magic()"
 *
 * Note -- see "make_artifact_special()" and "apply_magic()"
 */
/*:::�w�肵���x�[�X�A�C�e�����Œ�A�[�e�B�t�@�N�g�ɂȂ锻��Ƃ��̏���*/
/*:::make_artifact_special()�Ƃ͕ʕ��B���łɃx�[�X�A�C�e�������܂��Ă���_�������Ă͓�������*/
static bool make_artifact(object_type *o_ptr)
{
	int i;

	/* No artifacts in the town */
	if (!dun_level) return (FALSE);

	/* Paranoia -- no "plural" artifacts */
	/*:::�����̃A�C�e���̓A�[�e�B�t�@�N�g�ɂȂ�Ȃ��B�����͐������ɂ������Ȃ��͂��Ȃ̂�paranoia*/
	if (o_ptr->number != 1) return (FALSE);

	/* Check the artifact list (skip the "specials") */
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];

		/* Skip "empty" items */
		if (!a_ptr->name) continue;

		/* Cannot make an artifact twice */
		if (a_ptr->cur_num) continue;

		if (a_ptr->gen_flags & TRG_QUESTITEM) continue;

		if (a_ptr->gen_flags & TRG_INSTA_ART) continue;

		/* Must have the correct fields */
		/*:::�x�[�X�A�C�e������v���Ă���K�v������*/
		if (a_ptr->tval != o_ptr->tval) continue;
		if (a_ptr->sval != o_ptr->sval) continue;

		/* XXX XXX Enforce minimum "depth" (loosely) */
		if (a_ptr->level > dun_level)
		{
			/* Acquire the "out-of-depth factor" */
			int d = (a_ptr->level - dun_level) * 2;

			/* Roll for out-of-depth creation */
			if (!one_in_(d)) continue;
		}

		/* We must make the "rarity roll" */
		if (!one_in_(a_ptr->rarity)) continue;

		/* Hack -- mark the item as an artifact */
		o_ptr->name1 = i;

		/* Hack: Some artifacts get random extra powers */
		random_artifact_resistance(o_ptr, a_ptr);

		/* Success */
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}


/*
 *  Choose random ego type
 */
/*:::����Ɩh��̃G�S�A�C�e�������Bslot�͑����ӏ�(RARM������ALARM�����j�Abool�͎���Ă��邩�ǂ���*/

/*:::�u���̓S���v�ȂǃT�u�x�[�X�ƃG�S����v���Ȃ��ꍇ�O����܂��ď��������B�w�ւƃA�~���̃G�S�͂����ŏ�������Ȃ��B*/
static byte get_random_ego(byte slot, bool good)
{
	int i, value;
	ego_item_type *e_ptr;

	long total = 0L;
	
	for (i = 1; i < max_e_idx; i++)
	{
		e_ptr = &e_info[i];

		
		/*:::�ʏ�̃G�S��rating�����Ŗ����l�G�S��rating��0�ɂȂ��Ă���*/
		if (e_ptr->slot == slot
		    && ((good && e_ptr->rating) || (!good && !e_ptr->rating)) )
		{
			if (e_ptr->rarity)
				/*:::���A���e�B0�ݒ�̃G�S�͕ʃ��[�`���ō���Ă���*/
				total += (255 / e_ptr->rarity);
		}
	}

	value = randint1(total);

	/*:::value�������_���ɋ��߂ď��ɒ萔�������Ă�����0�ɂȂ����Ƃ��C���f�b�N�X��Ԃ��B*/
	for (i = 1; i < max_e_idx; i++)
	{
		e_ptr = &e_info[i];
		
		if (e_ptr->slot == slot
		    && ((good && e_ptr->rating) || (!good && !e_ptr->rating)) )
		{
			if (e_ptr->rarity)
				value -= (255 / e_ptr->rarity);
			if (value <= 0L) break;
		}
	}
	return (byte)i;
}


/*
 * Apply magic to an item known to be a "weapon"
 *
 * Hack -- note special base damage dice boosting
 * Hack -- note special processing for weapon/digger
 */
/*:::����ɑ΂���㎿�A�����i�A�����_���A�[�e�B�t�@�N�g����*/
/*:::�����ɓ���O�Ƀ_�C�A�����h�G�b�W��pval=4�ɂȂ��Ă���B*/
///item res ego �G�S�A����������
///mod131223 �A�C�e���e�[�u���ATVAL�֘A�����
///mod140101 ����G�S�Ɋւ�����ꏈ������ʂ���������H
static void a_m_aux_1(object_type *o_ptr, int level, int power)
{
	int tohit1 = randint1(5) + m_bonus(5, level);
	int todam1 = randint1(5) + m_bonus(5, level);

	int tohit2 = m_bonus(10, level);
	int todam2 = m_bonus(10, level);

	if ((o_ptr->tval == TV_BOLT) || (o_ptr->tval == TV_ARROW) || (o_ptr->tval == TV_BULLET))
	{
		tohit2 = (tohit2+1)/2;
		todam2 = (todam2+1)/2;
	}
	else if(o_ptr->tval == TV_GUN && k_info[o_ptr->k_idx].to_d > 10)
	{
		int tmp_dam_base = (k_info[o_ptr->k_idx].to_d - 9) / 2;
		todam1 += randint0(tmp_dam_base / 2) + m_bonus(tmp_dam_base / 2,level);
		todam2 += m_bonus(tmp_dam_base ,level);
	}

	/* Good */
	/*:::�㎿��(+10,+10)�A�����i��(+20,+20)������ɎE�C�C���t��*/
	///mod150425 �}���E�S�[�V����AC�{�[�i�X�ǉ�
	if (power > 0)
	{
		/* Enchant */
		o_ptr->to_h += tohit1;
		o_ptr->to_d += todam1;
		if(o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_MAIN_GAUCHE) o_ptr->to_a += (tohit1 + todam1 ) / 3;
		/* Very good */
		if (power > 1)
		{
			/* Enchant again */
			o_ptr->to_h += tohit2;
			o_ptr->to_d += todam2;
			if(o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_MAIN_GAUCHE) o_ptr->to_a += (tohit2 + todam2 ) / 2;
		}
	}

	/* Cursed */
	/*:::�􂢂̎E�C�C���}�C�i�X����*/
	else if (power < 0)
	{
		if(o_ptr->tval == TV_GUN && one_in_(2))
		{
			o_ptr->to_h = -(o_ptr->to_h);
			o_ptr->to_d = -(o_ptr->to_d);

		}
		/* Penalize */
		o_ptr->to_h -= tohit1;
		o_ptr->to_d -= todam1;

		/* Very cursed */
		if (power < -1)
		{
			/* Penalize again */
			o_ptr->to_h -= tohit2;
			o_ptr->to_d -= todam2;
		}

		/* Cursed (if "bad") */
		if (o_ptr->to_h + o_ptr->to_d < 0) o_ptr->curse_flags |= TRC_CURSED;
	}

	/*:::�_�C�������h�G�b�W�̓G�S�ɂ��A�[�e�B�t�@�N�g�ɂ��Ȃ�Ȃ� �E�C��{�l��+10�Ȃ̂�power2�Ő��������ΏC���l��30�߂��ɂȂ邱�Ƃ�����*/
	///mod131224 �q�q�C���J�l�iD�G�b�W)�̓G�S�⁙�ɂ��Ȃ肤��悤�ɂ���@
	//if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DIAMOND_EDGE)) return;


	///mod160205 �u�j�v�̓��ꏈ��
	if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE)
	{
		if (power > 1)
		{
			if (one_in_(30) || power > 2) /* power > 2 is debug only */
			{
				create_artifact(o_ptr, FALSE);
				return;
			}
			o_ptr->name2 = get_random_ego(INVEN_EGO_AMMO, TRUE);

			while (one_in_(10L * o_ptr->dd * o_ptr->ds)) o_ptr->dd++;

			/* Hack -- restrict the damage dice */
			if (o_ptr->dd > 9) o_ptr->dd = 9;
		}

		/* Very cursed */
		else if (power < -1)
		{
			/* Roll for ego-item */
			if (randint0(MAX_DEPTH) < level)
			{
				o_ptr->name2 = get_random_ego(INVEN_EGO_AMMO, FALSE);
			}
		}
		return;
	}

	/* Analyze type */
	switch (o_ptr->tval)
	{
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_OTHERWEAPON:	

		{
			//���؂̒Z���ɒǉ��ϐ������@�����������i�ł�2/3�ŃG�S�⁙�ɂȂ炸�I��
			if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DRAGON_DAGGER)
			{
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}

			/* Very Good */
			if (power > 1)
			{
				int chance = 30 - level / 10;

				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;
				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				while (1)
				{
					//�����_���G�S����
					//v1.1.99 �x�[�X�A�C�e���u�ԁv�͐�p�̃G�S�������s��
					if(o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_FLOWER)
						o_ptr->name2 = get_random_ego(INVEN_EGO_FLOWER, TRUE);
					else
						o_ptr->name2 = get_random_ego(INVEN_RARM, TRUE);

					//���ȊO�̐n�̂��镐��ɒn��i�n�k�j�G�S�͂��Ȃ�
					if(object_has_a_blade(o_ptr))
					{
						if(o_ptr->name2 == EGO_WEAPON_QUAKE && o_ptr->tval != TV_AXE)continue;
					}
					//�n�̂Ȃ�����ɋƕ��A��ƕ��G�S�͂��Ȃ�
					else
					{
						if(o_ptr->name2 == EGO_WEAPON_SHARP	|| o_ptr->name2 == EGO_WEAPON_EXSHARP)continue;
						//�n�̂Ȃ����i�����X�Ƃ��j�Ɓu���̑�����v�ɂ͒n��G�S�͂��Ȃ�
						if((o_ptr->tval == TV_SPEAR || o_ptr->tval == TV_OTHERWEAPON) && (o_ptr->name2 == EGO_WEAPON_QUAKE)) continue;

						//v1.1.99 �R�W�G�S���n�̂Ȃ�����ɂ͂��Ȃ�
						if (o_ptr->name2 == EGO_WEAPON_YAMANBA )continue;

					}

					//v1.1.99 �_���T�[�G�S�͌y������ɂ������Ȃ�
					if (o_ptr->name2 == EGO_WEAPON_DANCER &&  o_ptr->weight > 30) continue;



					break;
				}
				//�p�Y�̃G�S�ɐ[�w�ȍ~�ǉ��U���t���̃`�����X
				if(o_ptr->name2 == EGO_WEAPON_HERO)
				{
					if (one_in_(3) && (level > 50))
						add_flag(o_ptr->art_flags, TR_BLOWS);
				}
				//���҃G�S�ɓőϐ��ƌx��
				if(o_ptr->name2 == EGO_WEAPON_DEFENDER)
				{
					if (one_in_(3))
						add_flag(o_ptr->art_flags, TR_RES_POIS);
					if (one_in_(3))
						add_flag(o_ptr->art_flags, TR_WARNING);
				}
				//���_�G�S��1/3�Ŕ����J�}�C�^�`������
				if(o_ptr->name2 == EGO_WEAPON_FUJIN && one_in_(3))
				{
						add_flag(o_ptr->art_flags, TR_ACTIVATE );
						if(object_has_a_blade(o_ptr)) o_ptr->xtra2 = ACT_WHIRLWIND;
						else o_ptr->xtra2 = ACT_TORNADO;
				}
				//�S�A��ƕ��A���_�i��m���j�G�S�ɎE�C�Ɠ����_�C�X�����@|�ł͕t���Ȃ�
				if(o_ptr->name2 == EGO_WEAPON_ONI || o_ptr->name2 == EGO_WEAPON_EXSHARP 
					|| (o_ptr->name2 == EGO_WEAPON_RYU_JIN && one_in_(5)) )
				{
					/*:::�E�C�̕���̃_�C�X����*/
					if (one_in_(3)) /* double damage */
						o_ptr->dd *= 2;
					else
					{
						do
						{
							o_ptr->dd++;
						}
						while (one_in_(o_ptr->dd));
						do
						{
							o_ptr->ds++;
						}
						while (one_in_(o_ptr->ds));
					}
				}


				//�x�[�X��|�؂̂��镐��(�m���ŕt�����ꂽ���̊܂�)���ƕ����ƕ��G�S�ɂȂ����Ƃ�|X�؂ɂȂ邱�Ƃ�����
				if(o_ptr->name2 == EGO_WEAPON_EXSHARP || o_ptr->name2 == EGO_WEAPON_SHARP)
				{
					if( (have_flag(o_ptr->art_flags, TR_VORPAL) || have_flag(k_info[o_ptr->k_idx].flags, TR_VORPAL)) && weird_luck())
						add_flag(o_ptr->art_flags, TR_EX_VORPAL);
				}
				//�X���C�G�S��/X�ɂȂ邱�Ƃ����� ��{�I�ɂ�50�K�ȍ~�ŃG�S�ƃx�[�X���d�������Ƃ�
				//���ۃx�[�X�ɂ���ȂɃX���C�����\��Ȃ����ǃR�[�h�Ƃ��Ă͂������Ƃ����ق����ʂ肪�����C������
				if(o_ptr->name2 == EGO_WEAPON_CHAOS)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_GOOD) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_GOOD))) )
						add_flag(o_ptr->art_flags, TR_KILL_GOOD);
				}
				if(o_ptr->name2 == EGO_WEAPON_GODEATER)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_DEITY) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_DEITY))) )
						add_flag(o_ptr->art_flags, TR_KILL_DEITY);
				}
				if(o_ptr->name2 == EGO_WEAPON_HUNTER)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_ANIMAL))) )
						add_flag(o_ptr->art_flags, TR_KILL_ANIMAL);
				}
				if(o_ptr->name2 == EGO_WEAPON_GHOSTBUSTER)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_UNDEAD))) )
						add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
				}
				if(o_ptr->name2 == EGO_WEAPON_YOUKAITAIZI)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_KWAI) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_KWAI))) )
						add_flag(o_ptr->art_flags, TR_KILL_KWAI);
				}
				if(o_ptr->name2 == EGO_WEAPON_DEMONSLAY)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_DEMON) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_DEMON))) )
						add_flag(o_ptr->art_flags, TR_KILL_DEMON);
				}
				if(o_ptr->name2 == EGO_WEAPON_DRAGONSLAY)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_DRAGON) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_DRAGON))) )
						add_flag(o_ptr->art_flags, TR_KILL_DRAGON);
				}
				if(o_ptr->name2 == EGO_WEAPON_MURDERER)
				{
					if((level > 50 || weird_luck()) && (weird_luck() || (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN) || have_flag(k_info[o_ptr->k_idx].flags, TR_SLAY_HUMAN))) )
						add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
				}


				//�����G�S�ɒ�m���Ŕ\�͒ǉ�
				if(o_ptr->name2 == EGO_WEAPON_SUI_RYU)
				{
					if (one_in_(5))
						add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
					if (one_in_(10))
						add_flag(o_ptr->art_flags, TR_BRAND_ACID);
					if (one_in_(7))
						one_ability(o_ptr);
				}
				//�n��G�S�ɐ[�w�Œǉ��U���t�^�̃`�����X
				//v1.1.99 ��n�G�S(�闖�G�S����ω�)�ɂ������������s��
				if(o_ptr->name2 == EGO_WEAPON_QUAKE || o_ptr->name2 == EGO_WEAPON_BRANDPOIS)
				{
					if (one_in_(3) && (level > 60))
						add_flag(o_ptr->art_flags, TR_BLOWS);

				}				
				//�z���G�S�ɒ�m����/�l���t��
				if(o_ptr->name2 == EGO_WEAPON_VAMP)
				{
					if (one_in_(5))
						add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
				}
				//����G�S�ɂ���Ɏ��d���Ȃ����Ƃ���
				if(o_ptr->name2 == EGO_WEAPON_HAKUREI)
				{
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_STR);
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_INT);
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_WIS);
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_DEX);
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_CON);
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_CHR);
				}	


				/*:::�G�S�S�ʒ�m���Ń_�C�X�����̃`�����X�@���ƃ_�C�X��(dd)��10�ȏ�ɂȂ��Ă�9�ɖ߂����*/
				if (!o_ptr->art_name)
				{
					/* Hack -- Super-charge the damage dice */
					while (one_in_(10L * o_ptr->dd * o_ptr->ds)) o_ptr->dd++;

					/* Hack -- Lower the damage dice */
					if (o_ptr->dd > 9) o_ptr->dd = 9;
				}

				//�S�̕����dd�������ɔ�Ⴕ�ďd�ʂ���������
				if(o_ptr->name2 == EGO_WEAPON_ONI)
				{
					o_ptr->weight = (k_info[o_ptr->k_idx].weight * (o_ptr->dd) / (k_info[o_ptr->k_idx].dd) );
				}
			}

			/* Very cursed */
			else if (power < -1)
			{
				/* Roll for ego-item */
				if (randint0(MAX_DEPTH) < level)
				{
					//v1.1.99 �x�[�X�A�C�e���u�ԁv�͐�p�̃G�S�������s��
					if(o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_FLOWER)
						o_ptr->name2 = get_random_ego(INVEN_EGO_FLOWER, FALSE);
					else
						o_ptr->name2 = get_random_ego(INVEN_RARM, FALSE);

					switch (o_ptr->name2)
					{
					case EGO_WEAPON_GHOST:
						if (one_in_(6)) add_flag(o_ptr->art_flags, TR_TY_CURSE);
						break;
					case EGO_WEAPON_DEMON:
						if (one_in_(6)) add_flag(o_ptr->art_flags, TR_TY_CURSE);
						if (one_in_(3)) add_flag(o_ptr->art_flags, TR_SLAY_GOOD);
						if (one_in_(3)) add_flag(o_ptr->art_flags, TR_AGGRAVATE);
						break;
					case EGO_WEAPON_DEMONLORD:
						if (!one_in_(6)) add_flag(o_ptr->art_flags, TR_TY_CURSE);
						if (one_in_(3)) add_flag(o_ptr->art_flags, TR_KILL_GOOD);
						if (one_in_(3)) add_flag(o_ptr->art_flags, TR_RES_TIME);
						break;
					}
				}
			}

			break;
		}


	case TV_BOW:
	case TV_CROSSBOW:
		{
			/* Very good */
			if (power > 1)
			{

				int chance = 20 - level / 20;
				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				//���l�̋|�Ǝ�l�̋|�ɔ{��˂��t�������`�����X�@���������o�������Ⴂ�C������
				o_ptr->name2 = get_random_ego(INVEN_EGO_BOW, TRUE);

				if(o_ptr->name2 == EGO_BOW_WARRIOR)
				{
					if (one_in_(3))
						add_flag(o_ptr->art_flags, TR_XTRA_MIGHT);
				}
				if(o_ptr->name2 == EGO_BOW_HUNTER)
				{
					if (one_in_(3))
						add_flag(o_ptr->art_flags, TR_XTRA_SHOTS);
				}


			}

			break;
		}
	case TV_GUN:
		{
			int need_turn = gun_base_param_table[o_ptr->sval].charge_turn;
			int base_dd = k_info[o_ptr->k_idx].dd;
			int base_ds = k_info[o_ptr->k_idx].ds;
			int base_to_d = k_info[o_ptr->k_idx].to_d;
			int base_to_h = k_info[o_ptr->k_idx].to_h;
			/* Very good */
			if (power > 1)
			{

				int chance = 16 - level / 16;
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;
				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_EGO_GUN, TRUE);

				//�G�S�A�C�e���̃_�C�X��_���[�W���������͓���Ȃ̂�e_info.txt��C�s�����łȂ������ōs���B
				switch(o_ptr->name2)
				{
				case EGO_GUN_WALTER: //�@�V���ςݐ���@�z�[���[�t�@�C�A����
					o_ptr->xtra1 = GF_HOLY_FIRE;
					break;
				case EGO_GUN_MOON: //���@�_�C�X�ƃ_���[�W��
					o_ptr->dd += randint0(base_dd / 2+1) + base_dd / 2;
					o_ptr->to_d += damroll(2,10) + base_to_d / 2;
					break;
				case EGO_GUN_THOUSAND: //�����I�u�T�E�U���h�@�����啝���@�_���[�W��
					o_ptr->to_h += 10 + damroll(2,5);
					o_ptr->to_d += damroll(2,5) + base_to_d / 3;
					break;
				case EGO_GUN_FLAME: //�ĈΒe���@�_�C�X�啝�����A�Ή�����
					o_ptr->xtra1 = GF_FIRE;
					o_ptr->dd += base_dd + randint1((base_dd+1) / 2);
					break;
				case EGO_GUN_PLASMA: //�v���Y�}�@�_�C�X�啝�����A�_���[�W�����A�v���Y�}����
					o_ptr->xtra1 = GF_PLASMA;
					o_ptr->dd += base_dd * 3 / 2 + randint1((base_dd+1)/2 );
					o_ptr->to_d += base_to_d / 2 + randint1(base_to_d / 2 + 1);
					break;
				case EGO_GUN_KAPPA: //�͓��@������
					o_ptr->xtra1 = GF_WATER;
					break;
				case EGO_GUN_YATAGARASU: //���@�G�@�j�M�����A�_�C�X�ƃ_���[�W�啝�����@���܂ɃX�p�[�N��
					o_ptr->xtra1 = GF_NUKE;
					o_ptr->dd +=  base_dd + randint1((base_dd+1));
					o_ptr->to_d += base_to_d + randint1(base_to_d);
					if(weird_luck()) o_ptr->xtra4 = GUN_FIRE_MODE_SPARK;
					break;
				case EGO_GUN_M_SCIENTIST: //�}�b�h�T�C�G���e�B�X�g�@�����_�������A�_�C�X�����A�����ƃ_���[�W����
					{
						int typ_list[] = {GF_ELEC,GF_POIS,GF_ACID,GF_COLD,GF_FIRE,GF_PLASMA,GF_LITE,GF_DARK,GF_SOUND,GF_CHAOS,GF_NETHER,GF_TIME,GF_GRAVITY,GF_POLLUTE,GF_NUKE,GF_TORNADO,GF_STEAM,GF_DISINTEGRATE};
						o_ptr->xtra1 = typ_list[randint0(sizeof(typ_list)/sizeof(int))];
						o_ptr->dd += randint1(base_dd);
						o_ptr->to_d += (rand_range(-20,80) * (10+base_to_d / 2))/100;
						o_ptr->to_h += rand_range(-20,10);
					}
					break;
				}
			}
			else if (power < -1)
			{
				/* Roll for ego-item */
				if (randint0(MAX_DEPTH) < level)
				{
					o_ptr->name2 = get_random_ego(INVEN_EGO_GUN, FALSE);

					switch(o_ptr->name2)
					{
					case EGO_GUN_LOWQUALITY: //�e���i�@�q�b�g�E�_�C�X����
						o_ptr->to_d -= base_to_d * (50+randint1(100)) / 100;
						o_ptr->to_h -= damroll(2,10);
						break;
					case EGO_GUN_MAGATSUHI: //��ВÓ��@�_�C�X�ƃ_���[�W�啝�㏸�A�w���t�@�C�A�����t�^
						o_ptr->dd += base_dd / 2 + randint1(base_dd / 2 + 1);
						o_ptr->to_d += base_to_d + randint1(base_to_d) + damroll(3,level/3);
						o_ptr->xtra1 = GF_HELL_FIRE;
						break;
					}

				}
			}


			//�ˌ���ARROW�łȂ��Ƃ����@���ނ��{���g�ɂ���
			if(o_ptr->xtra1 != GF_ARROW && o_ptr->xtra4 == GUN_FIRE_MODE_DEFAULT) 
				o_ptr->xtra4 = GUN_FIRE_MODE_BOLT;


			break;
		}


	case TV_BOLT:
	case TV_ARROW:
	case TV_BULLET:
		{
			/* Very good */
			if (power > 1)
			{
				int chance = 25 - level / 12;

				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if (ironman_no_fixed_art) chance = chance * 2 / 3;
				if (EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}

				o_ptr->name2 = get_random_ego(INVEN_EGO_AMMO, TRUE);

				/* Hack -- super-charge the damage dice */
				while (one_in_(10L * o_ptr->dd * o_ptr->ds)) o_ptr->dd++;

				/* Hack -- restrict the damage dice */
				if (o_ptr->dd > 9) o_ptr->dd = 9;
			}

			/* Very cursed */
			else if (power < -1)
			{
				/* Roll for ego-item */
				if (randint0(MAX_DEPTH) < level)
				{
					o_ptr->name2 = get_random_ego(INVEN_EGO_AMMO, FALSE);
				}
			}

			break;
		}
	}
}

/*:::��ESP�t���@�t�����ꂽESP��~���̂Ƃ��߂�l��TRUE*/
///mod140101 ��ESP��~���ז��ɐݒ� ���t���������߂�lTRUE�ɂȂ�悤�ɂ��Ƃ�
static bool add_esp_strong(object_type *o_ptr)
{
	bool nonliv = FALSE;

	switch (randint1(4))
	{
	case 1: add_flag(o_ptr->art_flags, TR_ESP_EVIL); break;
	case 2: add_flag(o_ptr->art_flags, TR_TELEPATHY); break;
	case 3: add_flag(o_ptr->art_flags, TR_ESP_UNIQUE);  nonliv = TRUE; break;
	case 4:	add_flag(o_ptr->art_flags, TR_ESP_NONLIVING); nonliv = TRUE; break;
	}

	return nonliv;
}

/*:::extra:ON���ƕt��������e���p�X�̐���������*/
///mod140101 ��ESP��~�������_�����l�d�ɐݒ�
static void add_esp_weak(object_type *o_ptr, bool extra)
{
	int i;
	int tmp;
	int add_count;

	u32b weak_esp_list[] = {
		TR_ESP_GOOD,
		TR_ESP_ANIMAL,
		TR_ESP_HUMAN,
		TR_ESP_UNDEAD,
		TR_ESP_DRAGON,
		TR_ESP_DEITY,
		TR_ESP_DEMON,
		TR_ESP_KWAI,
	};
	const int MAX_ESP_WEAK = sizeof(weak_esp_list) / sizeof(weak_esp_list[0]);
	//const int add_count = MIN(MAX_ESP_WEAK, (extra) ? (3 + randint1(randint1(6))) : randint1(3));
	//mod140906 add_count���܂��9�ɂȂ��ė�����̂ŏC��..�Ƃ�����MIN()�������Ɠ����ĂȂ��B�ǂ��������Ƃ��H
	//��MIN()�̒���randint1����ꂽ�̂ŏ������莞�Ƒ������randint�̒l���ς���Ă����B�C�����悤�B
	tmp = (extra) ? (3 + randint1(randint1(5))) : randint1(3);
	add_count = MIN(MAX_ESP_WEAK, tmp);

	/* Add unduplicated weak esp flags randomly */
	for (i = 0; i < add_count; ++ i)
	{
		int choice = rand_range(i, MAX_ESP_WEAK - 1);

		add_flag(o_ptr->art_flags, weak_esp_list[choice]);
		weak_esp_list[choice] = weak_esp_list[i];
	}
}


/*
 * Apply magic to an item known to be "armor"
 *
 * Hack -- note special processing for crown/helm
 * Hack -- note special processing for robe of permanence
 */
/*:::�h��ɑ΂���㎿�A�����i�A�����_���A�[�e�B�t�@�N�g����*/
///item res ego sys �G�S�A����������
///mod140101 �V�G�S��������
static void a_m_aux_2(object_type *o_ptr, int level, int power)
{
	int toac1 = randint1(5) + m_bonus(5, level);

	int toac2 = m_bonus(10, level);
	/*:::�㎿+10,�����i+20�܂�AC�����Z*/

	/* Good */
	if (power > 0)
	{
		/* Enchant */
		o_ptr->to_a += toac1;

		/* Very good */
		if (power > 1)
		{
			/* Enchant again */
			o_ptr->to_a += toac2;
		}
	}

	/* Cursed */
	else if (power < 0)
	{
		/* Penalize */
		o_ptr->to_a -= toac1;

		/* Very cursed */
		if (power < -1)
		{
			/* Penalize again */
			o_ptr->to_a -= toac2;
		}

		/* Cursed (if "bad") */
		if (o_ptr->to_a < 0) o_ptr->curse_flags |= TRC_CURSED;
	}


	/* Analyze type */
	switch (o_ptr->tval)
	{
		case TV_DRAG_ARMOR:
		{

			int chance = 35 - level / 15;
			///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
			if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

			///mod140607 �[�w�قǁ����o�₷�����Ă݂�
			///mod140101 one_in_50��30
			if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				create_artifact(o_ptr, FALSE);

			/*:::�h���S���X�P�C�����C���̓G�S�ɂȂ�Ȃ�*/
			/* Mention the item */
			if (cheat_peek) object_mention(o_ptr);

			break;
		}

		case TV_ARMOR:
		case TV_CLOTHES:
		{
			//v1.1.64 �t�@�C���Z���~�b�N�X�̊Z�@�h���S�������݂����ɒǉ��ϐ�����
			if (o_ptr->tval == TV_ARMOR && o_ptr->sval == SV_ARMOR_FINE_CERAMIC)
			{
				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				/*:::�G�S�⁙�ɂȂ�ɂ���*/
				if (!one_in_(3)) break;
			}

			/* Very good */
			if (power > 1)
			{
				///del140101 �i���Ə��Ń��[�u�����̓��ꏈ�������������폜����


				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				int chance = 20 - level / 20;
				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}

				while (1)
				{
					o_ptr->name2 = get_random_ego(INVEN_BODY, TRUE);
					//�l���A�d���G�S�͊Z�ɂ͕t���Ȃ�
					if(o_ptr->tval == TV_ARMOR && (o_ptr->name2 == EGO_BODY_NINGYO || o_ptr->name2 == EGO_BODY_FAIRY)) continue;

					//v1.1.99 �R���A�R���Y�͊Z�ɂ͂��Ȃ�
					if (o_ptr->tval == TV_ARMOR && (o_ptr->name2 == EGO_BODY_YAMAWARO || o_ptr->name2 == EGO_BODY_YAMAJORO)) continue;
					//v1.1.99 ���ւ͊Z�ɂ������Ȃ�
					if (o_ptr->tval != TV_ARMOR && o_ptr->name2 == EGO_BODY_HANIWA) continue;

					//�S�_���A���E�_�G�S�͏o�ɂ���
					if(o_ptr->name2 == EGO_BODY_KISHIN && ( !(weird_luck()) || (level < 30))) continue;
					if(o_ptr->name2 == EGO_BODY_MAKAISHIN && (!(weird_luck()) || (level < 50) || (o_ptr->tval == TV_ARMOR) )) continue;
					break;
				}
				//�͓��G�S�Ɋm����r���t��
				if (o_ptr->name2 == EGO_BODY_KAPPA && one_in_(3))
					add_flag(o_ptr->art_flags, TR_RES_WATER);
				//v1.1.32 �͓��G�S�̍�ƕ��Ɋ�p���㏸�ǉ�
				if (o_ptr->name2 == EGO_BODY_KAPPA && o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_WORKER)
					add_flag(o_ptr->art_flags, TR_DEX);

				//��ԃG�S�Ɋm���Œǉ��ϐ�
				if (o_ptr->name2 == EGO_BODY_GATEKEEPER && one_in_(3))
					one_resistance(o_ptr);
				//�d�����G�S�Ɋm���Œǉ��\��
				if (o_ptr->name2 == EGO_BODY_FAIRY && one_in_(3))
					one_ability(o_ptr);
				//�S�̊Z�͊m���őϋv�㏸�B�Z�̏ꍇ50%AC�x�[�X���㏸���邪25%�d���Ȃ�
				if (o_ptr->name2 == EGO_BODY_ONI)
				{
					if(one_in_(3)) add_flag(o_ptr->art_flags, TR_CON);
					if(o_ptr->tval == TV_ARMOR)
					{
						o_ptr->ac = k_info[o_ptr->k_idx].ac * 3 / 2 ;
						o_ptr->weight = k_info[o_ptr->k_idx].weight * 5 / 4;
					}
				}
				//�V��̊Z�͊m����r�ŕt���B�Z�̏ꍇ25%AC�x�[�X���������邪25%�y���Ȃ�
				if (o_ptr->name2 == EGO_BODY_TENGU)
				{
					if(one_in_(3)) add_flag(o_ptr->art_flags, TR_RES_POIS);
					if(o_ptr->tval == TV_ARMOR)
					{
						o_ptr->ac = k_info[o_ptr->k_idx].ac * 3 / 4 ;
						o_ptr->weight = k_info[o_ptr->k_idx].weight * 3 / 4;
					}
				}


			}
			/*:::�����l�G�S*/
			else if (power < -1)
			{
				/* Roll for ego-item */
				if (randint0(MAX_DEPTH) < level)
				{
					o_ptr->name2 = get_random_ego(INVEN_BODY, FALSE);
					//���P�G�S�͒j�������Ō��ʂ�ς��Ă݂�
					if(o_ptr->name2 == EGO_BODY_HASHIHIME)
					{
						if (p_ptr->psex == SEX_MALE)
						{
							add_flag(o_ptr->art_flags, TR_WARNING);
							add_flag(o_ptr->art_flags, TR_AGGRAVATE);
						}
						else
						{
							add_flag(o_ptr->art_flags, TR_NO_TELE);
							add_flag(o_ptr->art_flags, TR_TY_CURSE);
						}
					}
				}
			}
			break;
		}

		case TV_SHIELD:
		{

			if (o_ptr->sval == SV_DRAGON_SHIELD)
			{
				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				/*:::�h���S���V�[���h�̓G�S�⁙�ɂȂ�ɂ���*/
				if (!one_in_(3)) break;
			}

			/* Very good */
			if (power > 1)
			{

				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				int chance = 20 - level / 20;
				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_LARM, TRUE);
				//���T�V��G�S�Ɋm���Ōx���Ǝ�ESP
				if(o_ptr->name2 == EGO_SHIELD_HAKUROU)
				{
					if (one_in_(4)) add_flag(o_ptr->art_flags, TR_WARNING);
					if (one_in_(4)) one_low_esp(o_ptr);
				}
				//���҃G�S�Ɋm���œőϐ�
				if(o_ptr->name2 == EGO_SHIELD_DEFENDER)
				{
					if (one_in_(4)) add_flag(o_ptr->art_flags, TR_RES_POIS);
				}
			
			}

			else if (power < -1)
			{
				/* Roll for ego-item */
				if (randint0(MAX_DEPTH) < level)
				{
					o_ptr->name2 = get_random_ego(INVEN_LARM, FALSE);

				}
			}
			break;
		}

		case TV_GLOVES:
		{
			if (o_ptr->sval == SV_HAND_DRAGONGLOVES)
			{
				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}
			if (power > 1)
			{

				int chance = 20 - level / 20;
				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				while (1)
				{
					o_ptr->name2 = get_random_ego(INVEN_HANDS, TRUE);
					//�d���G�S�͏d������ɂ͕t���Ȃ�
					if(o_ptr->name2 == EGO_HANDS_FAIRY && k_info[o_ptr->k_idx].weight > 10) continue;
					//�S�G�S�͌y����܂ɂ͕t���Ȃ�
					if(o_ptr->name2 == EGO_HANDS_ONI && k_info[o_ptr->k_idx].weight < 10) continue;

					break;
				}
				//�͓��G�S�Ɋm���Ŗ�გm�炸�Ɛ��ϐ�
				if(o_ptr->name2 == EGO_HANDS_KAAPPA)
				{
					if (one_in_(3)) add_flag(o_ptr->art_flags, TR_FREE_ACT);
					if (one_in_(3)) add_flag(o_ptr->art_flags, TR_RES_WATER);
				}
				//�S�G�S��AC�ƃ_���[�W�C���������邪�d���Ȃ�
				if(o_ptr->name2 == EGO_HANDS_ONI)
				{
						o_ptr->to_d = 5;
						o_ptr->ac = k_info[o_ptr->k_idx].ac * 3 / 2 ;
						o_ptr->weight = k_info[o_ptr->k_idx].weight * 3 / 2;
				}
				//�������G�S�ɒ�m���ŋ��C�ϐ�
				if(o_ptr->name2 == EGO_HANDS_IMP)
				{
					if ((one_in_(2) && weird_luck())) add_flag(o_ptr->art_flags, TR_RES_INSANITY);
				}
			}
			
			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_HANDS, FALSE);
			}

			break;
		}

		case TV_BOOTS:
		{
			if (o_ptr->sval == SV_LEG_DRAGONBOOTS)
			{
				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}
			/* Very good */
			if (power > 1)
			{

				int chance = 20 - level / 20;
				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}

				while (1)
				{
					o_ptr->name2 = get_random_ego(INVEN_FEET, TRUE);
					//�d���G�S�͏d���C�ɂ͕t���Ȃ�
					if(o_ptr->name2 == EGO_BOOTS_FAIRY && k_info[o_ptr->k_idx].weight > 20) continue;
					if(o_ptr->name2 == EGO_BOOTS_D_FAIRY && k_info[o_ptr->k_idx].weight > 20) continue;

					break;
				}
				if(o_ptr->name2 == EGO_BOOTS_FAIRY)
				{
					if (one_in_(3)) add_flag(o_ptr->art_flags, TR_FREE_ACT);
					if (one_in_(3)) one_resistance(o_ptr);
				}
				if(o_ptr->name2 == EGO_BOOTS_KAPPA)
				{
					if (one_in_(3)) add_flag(o_ptr->art_flags, TR_RES_WATER);
				}
				if(o_ptr->name2 == EGO_BOOTS_TSUKUMO)
				{
					if (one_in_(4)) add_esp_weak(o_ptr,FALSE);
				}
				//��V��̌C�Ɋm���ŏr���@�H�ɉ����u�[�X�g ��w�ł͂��̂���apply_magic()��pval�������[��
				if(o_ptr->name2 == EGO_BOOTS_RAVEN)
				{
					if (one_in_(3)) add_flag(o_ptr->art_flags, TR_SPEEDSTER);
					if (weird_luck()){
						while(one_in_(2) && o_ptr->pval < 89) o_ptr->pval++;
					}
				}


			}
			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_FEET, FALSE);
			}

			break;
		}

		case TV_HEAD:
		{
			if (o_ptr->sval == SV_HEAD_DRAGONHELM)
			{
				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}

			/* Very good */
			if (power > 1)
			{

				int chance = 20 - level / 20;
				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				while (1)
				{
					o_ptr->name2 = get_random_ego(INVEN_HEAD, TRUE);
					//�����G�S�͌y���X�q�ɂ������Ȃ�
					if(o_ptr->name2 == EGO_HEAD_WITCH && k_info[o_ptr->k_idx].weight > 20) continue;
					//�V�l�A���ҁA���Ƃ�A�P�N�G�S�͏d�����ɂ͂��Ȃ�
					if(o_ptr->name2 == EGO_HEAD_TEN_NIN && k_info[o_ptr->k_idx].weight > 40) continue;
					if(o_ptr->name2 == EGO_HEAD_MAGI && k_info[o_ptr->k_idx].weight > 40) continue;
					if(o_ptr->name2 == EGO_HEAD_SATORI && k_info[o_ptr->k_idx].weight > 40) continue;
					if(o_ptr->name2 == EGO_HEAD_PRINCESS && k_info[o_ptr->k_idx].weight > 40) continue;
					//�S�G�S�͊��ɂ������Ȃ�
					if(o_ptr->name2 == EGO_HEAD_ONI && (o_ptr->sval != SV_HEAD_GOLDCROWN && o_ptr->sval != SV_HEAD_JEWELCROWN)) continue;
					//���t�G�S�Əb�l�G�S�͊��ɂ͂��Ȃ�
					if(o_ptr->name2 == EGO_HEAD_TEACHER && (o_ptr->sval == SV_HEAD_GOLDCROWN || o_ptr->sval == SV_HEAD_JEWELCROWN)) continue;
					if(o_ptr->name2 == EGO_HEAD_BEAST && (o_ptr->sval == SV_HEAD_GOLDCROWN || o_ptr->sval == SV_HEAD_JEWELCROWN)) continue;

					//�����t�̖X�q���ǉ�
					if(o_ptr->name2 == EGO_HEAD_CLOWN && 
						(o_ptr->sval != SV_HEAD_LEATHER && o_ptr->sval != SV_HEAD_MASK && o_ptr->sval != SV_HEAD_WITCH && o_ptr->sval != SV_HEAD_HAT &&o_ptr->sval != SV_HEAD_GOLDCROWN && o_ptr->sval != SV_HEAD_JEWELCROWN)) continue;

					//v1.1.77 �T��G�S�͖X�q�ɂ������Ȃ�
					if (o_ptr->name2 == EGO_HEAD_DETECTIVE && (o_ptr->sval != SV_HEAD_LEATHER && o_ptr->sval != SV_HEAD_HAT)) continue;


					//�V��G�S�͓��݂ɂ������Ȃ�
					if(o_ptr->name2 == EGO_HEAD_TENGU && o_ptr->sval != SV_HEAD_TOKIN) continue;
					break;
				}
				//�����̖X�q�Ɋm���Ŗ��@��Փx����
				if(o_ptr->name2 == EGO_HEAD_WITCH)
				{
					if (one_in_(4) && (level >= 50 || weird_luck())) add_flag(o_ptr->art_flags, TR_EASY_SPELL);
				}
				//���_�G�S��ESP�t�� ���ɂ͗ǂ��̂��t���₷��
				if(o_ptr->name2 == EGO_HEAD_REAPER)
				{
					if(one_in_(2)) add_flag(o_ptr->art_flags, TR_ESP_HUMAN);
					else  add_flag(o_ptr->art_flags, TR_ESP_UNDEAD);
					
					if (one_in_(2) && (o_ptr->sval == SV_HEAD_GOLDCROWN || o_ptr->sval == SV_HEAD_JEWELCROWN))
					{
							if (one_in_(2)) add_esp_strong(o_ptr);
							else add_esp_weak(o_ptr, FALSE);
					}
					else if(one_in_(5)){
							if (one_in_(3)) add_esp_strong(o_ptr);
							else add_esp_weak(o_ptr, FALSE);
					}
				}
				//�[�w�Ō��҃G�S�ɐF�X�t��
				if(o_ptr->name2 == EGO_HEAD_MAGI)
				{
					if (one_in_(4) && (level >= 50 || weird_luck())) 
					{
							if(one_in_(4))add_flag(o_ptr->art_flags, TR_DEC_MANA);
							else if(one_in_(3)) add_esp_strong(o_ptr);
							else add_esp_weak(o_ptr, FALSE);
					}
				}
				//���҃G�S�Ɋm���Ōx��
				if(o_ptr->name2 == EGO_HEAD_DEFENDER)
				{
					if (one_in_(4)) add_flag(o_ptr->art_flags, TR_WARNING);
				}
				//�����t�G�S�Ɋm���Œǉ��\��
				if(o_ptr->name2 == EGO_HEAD_CLOWN)
				{
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_CHR);
					if (one_in_(4)) add_flag(o_ptr->art_flags, TR_SEE_INVIS);
					if (one_in_(5)) add_flag(o_ptr->art_flags, TR_RES_CHAOS);
					if (one_in_(9)) add_flag(o_ptr->art_flags, TR_RES_INSANITY);

				}

				//v1.1.77 �T��G�S�Ɋm���Œǉ��\��
				if (o_ptr->name2 == EGO_HEAD_DETECTIVE)
				{
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_RES_BLIND);
					if (one_in_(2))
					{
						add_flag(o_ptr->art_flags, TR_ACTIVATE);
						o_ptr->xtra2 = ACT_DETECT_ALL;
					}
					if (one_in_(2)) add_flag(o_ptr->art_flags, TR_ESP_UNIQUE);
				}

				//�b�l�G�S�ɓ���������ESP�������o�[�T�[�N
				if(o_ptr->name2 == EGO_HEAD_BEAST)
				{
					if (one_in_(3)) add_flag(o_ptr->art_flags, TR_SEE_INVIS);
					else if(one_in_(2)) one_low_esp(o_ptr);
					else{
						add_flag(o_ptr->art_flags, TR_ACTIVATE );
						o_ptr->xtra2 = ACT_BERSERK;
					}
				}
				//���Ƃ�G�S��ESP�t���@~����~�ׂ����Ȃ������Ƃ��͑��߂̎�ESP���t�������
				if(o_ptr->name2 == EGO_HEAD_SATORI)
				{
					if (add_esp_strong(o_ptr)) add_esp_weak(o_ptr, TRUE);
					else add_esp_weak(o_ptr, FALSE);	
				}
				//�P�N�G�S�@e_info�̊�{�l�ƍ��킹��Ɩ���+4�`10�ɂȂ� ���̂Ƃ������t���A�m���Ŕj�בϐ��t��
				if(o_ptr->name2 == EGO_HEAD_PRINCESS)
				{
					o_ptr->pval = 3;
					if (o_ptr->sval == SV_HEAD_GOLDCROWN || o_ptr->sval == SV_HEAD_JEWELCROWN)
					{
							add_flag(o_ptr->art_flags, TR_LITE );
							if (one_in_(3)) add_flag(o_ptr->art_flags, TR_RES_HOLY );
					}
				}

				break;
			}
			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_HEAD, FALSE);
			}
			break;
		}

		case TV_CLOAK:
		{
			/* Very good */
			if (power > 1)
			{

				int chance = 20 - level / 20;
				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				if (one_in_(chance) || (power > 2)) /* power > 2 is debug only */
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_OUTER, TRUE);

			}

			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_OUTER, FALSE);
			}

			break;
		}
	}
}


/*
 * Apply magic to an item known to be a "ring" or "amulet"
 *
 * Hack -- note special "pval boost" code for ring of speed
 * Hack -- note that some items must be cursed (or blessed)
 */
/*:::�w�ւƃA�~���̃G�S�A�������@�G�S�ւ̃p�����[�^�����������ōs��*/
///item res�@�w�ւƃA�~����pval����ƃG�S�����@���Ȃ蒷��
///mod140111 �w�ւƃA�~���̃G�S����
static void a_m_aux_3(object_type *o_ptr, int level, int power)
{
	int chance;
//o_ptr->pval=2;
//return;
//#if 0
	/* Apply magic (good or bad) according to type */
	switch (o_ptr->tval)
	{
		/*:::�w�֑S��*/
		case TV_RING:
		{
			/* Analyze */
			/*:::�w�ւ�sval���ƂɃx�[�Xpval��ǉ��ϐ���􂢃t���O�����@�֌W�Ȃ��w�ւ�����*/
			switch (o_ptr->sval)
			{
				/*:::�ǉ��U���̎w��*/
				case SV_RING_ATTACKS:
				{
					/* Stat bonus */
					o_ptr->pval = m_bonus(2, level);/*:::2�ɂȂ�m����80�K��7%���x�炵��*/
					if (one_in_(15)) o_ptr->pval++;
					if (o_ptr->pval < 1) o_ptr->pval = 1;

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				case SV_RING_SHOTS:
				{
					break;
				}

				/* Strength, Constitution, Dexterity, Intelligence */
				case SV_RING_STR:
				case SV_RING_CON:
				case SV_RING_DEX:
				{
					/* Stat bonus */
					o_ptr->pval = 1 + m_bonus(5, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Ring of Speed! */
				case SV_RING_SPEED:
				{
					/* Base speed (1 to 10) */
					o_ptr->pval = randint1(5) + m_bonus(5, level);

					/* Super-charge the ring */
					while (randint0(100) < 50) o_ptr->pval++;

					/* Cursed Ring */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);

						break;
					}

					/* Mention the item */
					if (cheat_peek) object_mention(o_ptr);

					break;
				}

				case SV_RING_LORDLY:
				{
					do
					{
						 /*:::���҂̉���̎w�֐�����p�̏�ʑϐ��t�� �łƗ򉻂������Ă���*/
						one_lordly_high_resistance(o_ptr);
					}
					while (one_in_(4));

					/* Bonus to armor class */
					o_ptr->to_a = 10 + randint1(5) + m_bonus(10, level);
				}
				break;

				case SV_RING_WARNING:
				{
					if (one_in_(3)) one_low_esp(o_ptr);
					break;
				}

				/* Searching */
				case SV_RING_SEARCHING:
				{
					/* Bonus to searching */
					o_ptr->pval = 1 + m_bonus(5, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Flames, Acid, Ice */
				case SV_RING_FLAMES:
				case SV_RING_ACID:
				case SV_RING_ICE:
				case SV_RING_ELEC:
				{
					/* Bonus to armor class */
					o_ptr->to_a = 5 + randint1(5) + m_bonus(10, level);
					break;
				}

				/* Weakness, Stupidity */
				case SV_RING_WEAKNESS:
				case SV_RING_STUPIDITY:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= TRC_CURSED;

					/* Penalize */
					o_ptr->pval = 0 - (1 + m_bonus(5, level));
					if (power > 0) power = 0 - power;

					break;
				}

				/* WOE, Stupidity */
				case SV_RING_WOE:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= TRC_CURSED;

					/* Penalize */
					o_ptr->to_a = 0 - (5 + m_bonus(10, level));
					o_ptr->pval = 0 - (1 + m_bonus(5, level));
					if (power > 0) power = 0 - power;

					break;
				}

				/* Ring of damage */
				case SV_RING_DAMAGE:
				{
					/* Bonus to damage */
					o_ptr->to_d = 1 + randint1(3) + m_bonus(16, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse bonus */
						o_ptr->to_d = 0 - o_ptr->to_d;
					}

					break;
				}

				/* Ring of Accuracy */
				case SV_RING_ACCURACY:
				{
					/* Bonus to hit */
					o_ptr->to_h = 1 + randint1(5) + m_bonus(16, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse tohit */
						o_ptr->to_h = 0 - o_ptr->to_h;
					}

					break;
				}

				/* Ring of Protection */
				case SV_RING_PROTECTION:
				{
					/* Bonus to armor class */
					o_ptr->to_a = 5 + randint1(8) + m_bonus(10, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse toac */
						o_ptr->to_a = 0 - o_ptr->to_a;
					}

					break;
				}

				/* Ring of Slaying */
				case SV_RING_SLAYING:
				{
					/* Bonus to damage and to hit */
					o_ptr->to_d = randint1(5) + m_bonus(12, level);
					o_ptr->to_h = randint1(5) + m_bonus(12, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse bonuses */
						o_ptr->to_h = 0 - o_ptr->to_h;
						o_ptr->to_d = 0 - o_ptr->to_d;
					}

					break;
				}

				case SV_RING_MUSCLE:
				{
					o_ptr->pval = 1 + m_bonus(3, level);
					if (one_in_(4)) o_ptr->pval++;

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}
				case SV_RING_AGGRAVATION:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= TRC_CURSED;

					if (power > 0) power = 0 - power;
					break;
				}
			}
			/*:::�w�ւ́�����*/
			///mod140111 �w�ւ́����������������Ɗɘa����
			//if ((one_in_(400) && (power > 0) && !object_is_cursed(o_ptr) && (level > 79))
			//    || (power > 2)) /* power > 2 is debug only */


				chance = 100 - level / 5;
				///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
				if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;

				///mod140607 �[�w�قǁ����o�₷�����Ă݂�
				///mod150316 ������Ə����ɘa
			if ((one_in_(chance) && (power > 0) && !object_is_cursed(o_ptr) && (level > ((ironman_no_fixed_art) ? 29 : 59) ))
			    || (power > 2))
			{
				///mod160619 pval������create_artifact()���ōs��
				//o_ptr->pval = MIN(o_ptr->pval, 4);
				/* Randart amulet */
				create_artifact(o_ptr, FALSE);
			}
			/*:::�w�ւ̃G�S�����@�����i�t���O�������Ă�1/2�ł����G�S�ɂȂ�Ȃ�*/
			else if ((power == 2) && one_in_(2))
			{
				while(!o_ptr->name2)
				{
					int tmp = m_bonus(10, level);
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(28))
					{
					case 1: case 2:
						//o_ptr->name2 = EGO_RING_THROW;
						break;
					case 3: case 4:
						if (have_flag(k_ptr->flags, TR_REGEN)) break;
						o_ptr->name2 = EGO_RING_REGEN;
						break;
					case 5: case 6:
						if (have_flag(k_ptr->flags, TR_LITE)) break;
						o_ptr->name2 = EGO_RING_LIGHT;
						break;
					case 7: case 8:
						if (have_flag(k_ptr->flags, TR_TELEPORT)) break;
						o_ptr->name2 = EGO_RING_TELE_RANDOM;
						break;
					case 9: case 10:
						if (o_ptr->to_h) break;
						o_ptr->name2 = EGO_RING_ACCURACY;
						break;
					case 11: case 12:
						if (o_ptr->to_d) break;
						o_ptr->name2 = EGO_RING_DAMAGE;
						break;
					case 13:
						//if ((o_ptr->to_h) || (o_ptr->to_d)) break;
						//o_ptr->name2 = EGO_RING_SLAY;
						break;
					case 14:
						//if ((have_flag(k_ptr->flags, TR_STR)) || o_ptr->to_h || o_ptr->to_d) break;
						o_ptr->name2 = EGO_RING_WIZARDRY;
						break;
					case 15:
						if (have_flag(k_ptr->flags, TR_ACTIVATE)) break;
						o_ptr->name2 = EGO_RING_HERO;
						break;
					case 16:
						if (have_flag(k_ptr->flags, TR_ACTIVATE)) break;
						if (tmp > 8) o_ptr->name2 = EGO_RING_BA_MANA;
						else if (tmp > 4) o_ptr->name2 = EGO_RING_BO_MANA;
						else o_ptr->name2 = EGO_RING_BO_MISS_1;
						break;
					case 17:
						if (have_flag(k_ptr->flags, TR_ACTIVATE)) break;
						if (!(have_flag(k_ptr->flags, TR_RES_FIRE)) && (have_flag(k_ptr->flags, TR_RES_COLD) || have_flag(k_ptr->flags, TR_RES_ELEC) || have_flag(k_ptr->flags, TR_RES_ACID))) break;
						else o_ptr->name2 = EGO_RING_BA_FIRE_1;
						break;
					case 18:
						if (have_flag(k_ptr->flags, TR_ACTIVATE)) break;
						if (!(have_flag(k_ptr->flags, TR_RES_COLD)) && (have_flag(k_ptr->flags, TR_RES_FIRE) || have_flag(k_ptr->flags, TR_RES_ELEC) || have_flag(k_ptr->flags, TR_RES_ACID))) break;
						else o_ptr->name2 = EGO_RING_BA_COLD_2;
						break;
					case 19:
						if (have_flag(k_ptr->flags, TR_ACTIVATE)) break;
						if (!(have_flag(k_ptr->flags, TR_RES_ELEC)) && (have_flag(k_ptr->flags, TR_RES_COLD) || have_flag(k_ptr->flags, TR_RES_FIRE) || have_flag(k_ptr->flags, TR_RES_ACID))) break;
						else o_ptr->name2 = EGO_RING_BA_ELEC_2;
						break;
					case 20:
						if (have_flag(k_ptr->flags, TR_ACTIVATE)) break;
						if (!(have_flag(k_ptr->flags, TR_RES_ACID)) && (have_flag(k_ptr->flags, TR_RES_COLD) || have_flag(k_ptr->flags, TR_RES_ELEC) || have_flag(k_ptr->flags, TR_RES_FIRE))) break;
						else o_ptr->name2 = EGO_RING_BA_ACID_1;
						break;
					case 21: case 22: case 23: case 24: case 25: case 26:
						switch (o_ptr->sval)
						{
						case SV_RING_SPEED:
							if (!one_in_(3)) break;
							o_ptr->name2 = EGO_RING_DOUBLESPEED;
							break;
						case SV_RING_DAMAGE:
						case SV_RING_ACCURACY:
							if (one_in_(2)) break;
							o_ptr->name2 = EGO_RING_HERO;

							break;
						case SV_RING_SLAYING:
							if (!one_in_(3)) break;
							else if(one_in_(2)) 
							{
								o_ptr->name2 = EGO_RING_MUSOU;
								o_ptr->to_h += 2+randint1(8);
								o_ptr->to_d += 2+randint1(8);
							}
							else
							{
								o_ptr->name2 = EGO_RING_SYURA;
								o_ptr->to_a -= 30;
								o_ptr->to_d += 9 + randint1(11);
								o_ptr->pval = -3 - randint0(3);

							}
							break;

						case SV_RING_PROTECTION:
							o_ptr->name2 = EGO_RING_EXTRA_AC;
							o_ptr->to_a += 7 + m_bonus(5, level);
							break;
						case SV_RING_RES_FEAR:
							o_ptr->name2 = EGO_RING_HERO;
							break;
						case SV_RING_SHOTS:
							if (one_in_(2)) break;
							o_ptr->name2 = EGO_RING_HUNTER;
							break;
						case SV_RING_SEARCHING:
							o_ptr->name2 = EGO_RING_STEALTH;
							break;
						case SV_RING_TELEPORTATION:
							if(weird_luck()) o_ptr->name2 = EGO_RING_RES_TIME;
							else o_ptr->name2 = EGO_RING_TELE_OTHER;
							break;
						case SV_RING_RES_BLINDNESS:
							if (one_in_(2))
								o_ptr->name2 = EGO_RING_RESISTLIGHT;
							else
								o_ptr->name2 = EGO_RING_RESISTDARK;
							break;
						case SV_RING_LORDLY:
							if (!one_in_(20)) break;
							if (one_in_(2))
								o_ptr->name2 = EGO_RING_GALAXY;
							else
								o_ptr->name2 = EGO_RING_GALAXY2;
							break;
						case SV_RING_SUSTAIN:
							if (!one_in_(4)) break;
							o_ptr->name2 = EGO_RING_RES_TIME;
							break;

						case SV_RING_MIGHTY_THROW:
							if (!one_in_(2)) break;
							if (one_in_(3))	o_ptr->name2 = EGO_RING_PERFECTMAID;
							else o_ptr->name2 = EGO_RING_TADIKARAO;
							break;

						case SV_RING_WARNING:
							if (!one_in_(2)) break;
							o_ptr->name2 = EGO_RING_DETECTALLMON;
							break;
						default:
							break;
						}
						break;
					}
				}
				/* Uncurse it */
				o_ptr->curse_flags = 0L;
			}
			else if ((power == -2) && one_in_(2))
			{
				if (o_ptr->to_h > 0) o_ptr->to_h = 0-o_ptr->to_h;
				if (o_ptr->to_d > 0) o_ptr->to_d = 0-o_ptr->to_d;
				if (o_ptr->to_a > 0) o_ptr->to_a = 0-o_ptr->to_a;
				if (o_ptr->pval > 0) o_ptr->pval = 0-o_ptr->pval;
				o_ptr->art_flags[0] = 0;
				o_ptr->art_flags[1] = 0;
				while(!o_ptr->name2)
				{
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(5))
					{
					case 1:
						if (have_flag(k_ptr->flags, TR_DRAIN_EXP)) break;
						o_ptr->name2 = EGO_RING_DRAIN_EXP;
						break;
					case 2:
						o_ptr->name2 = EGO_RING_PACIFIST;
						break;
					case 3:
						if (have_flag(k_ptr->flags, TR_AGGRAVATE)) break;
						o_ptr->name2 = EGO_RING_AGGRAVATE;
						break;
					case 4:
						if (have_flag(k_ptr->flags, TR_TY_CURSE)) break;
						o_ptr->name2 = EGO_RING_TY_CURSE;
						break;
					case 5:
						o_ptr->name2 = EGO_RING_ALBINO;
						break;
					}
				}
				/* Broken */
				o_ptr->ident |= (IDENT_BROKEN);

				/* Cursed */
				o_ptr->curse_flags |= (TRC_CURSED | TRC_HEAVY_CURSE);
			}
			break;
		}

		case TV_AMULET:
		{
			/* Analyze */
			switch (o_ptr->sval)
			{
				/* Amulet of wisdom/charisma */
				case SV_AMULET_INTELLIGENCE:
				case SV_AMULET_WISDOM:
				case SV_AMULET_CHARISMA:
				{
					o_ptr->pval = 1 + m_bonus(5, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}

				/* Amulet of brilliance */
				case SV_AMULET_BRILLIANCE:
				{
					o_ptr->pval = 1 + m_bonus(3, level);
					if (one_in_(4)) o_ptr->pval++;

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}

				case SV_AMULET_NO_MAGIC: case SV_AMULET_NO_TELE:
				{
					if (power < 0)
					{
						o_ptr->curse_flags |= (TRC_CURSED);
					}
					break;
				}

				case SV_AMULET_RESISTANCE:
				{
					if (one_in_(3)) one_high_resistance(o_ptr);
					if (one_in_(3)) add_flag(o_ptr->art_flags, TR_RES_POIS);
				}
				break;

				/* Amulet of searching */
				case SV_AMULET_SEARCHING:
				{
					o_ptr->pval = randint1(2) + m_bonus(4, level);
					one_low_esp(o_ptr);//�T���A�~���Ɏ�ESP�t�����Ă݂�

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Amulet of the Magi -- never cursed */
				case SV_AMULET_THE_MAGI:
				{
					o_ptr->pval = randint1(5) + m_bonus(5, level);
					o_ptr->to_a = randint1(5) + m_bonus(5, level);

					/* gain one low ESP */
					add_esp_weak(o_ptr, FALSE);

					/* Mention the item */
					if (cheat_peek) object_mention(o_ptr);

					break;
				}

				/* Amulet of Doom -- always cursed */
				case SV_AMULET_DOOM:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= (TRC_CURSED);

					/* Penalize */
					o_ptr->pval = 0 - (randint1(5) + m_bonus(5, level));
					o_ptr->to_a = 0 - (randint1(5) + m_bonus(5, level));
					if (power > 0) power = 0 - power;

					break;
				}

				case SV_AMULET_MAGIC_MASTERY:
				{
					o_ptr->pval = 1 + m_bonus(4, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}
			}
			chance = 50 - level / 10;
			///mod150205 ���֎~�I�v�V�����̂Ƃ��́����o�₷�����Ă݂�
			if(ironman_no_fixed_art) chance = chance * 2 / 3;
				if(EXTRA_MODE) chance = chance * 3 / 4;
			/*:::�A�~�����b�g�́����� */
			///mod140111 �A�~�����b�g�́����� �����ɘa���Ă݂�
			///mod150316 ��������level�����Y��ĉ��K�ł��o��悤�ɂȂ��Ă��̂ŏC��
			if ((one_in_(chance) && (power > 0) && !object_is_cursed(o_ptr) && level > ((ironman_no_fixed_art) ? 29 : 59))
			    || (power > 2))

			//if ((one_in_(150) && (power > 0) && !object_is_cursed(o_ptr) && (level > 79))
			//    || (power > 2)) /* power > 2 is debug only */
			{
				///mod160619 pval������create_artifact()���ōs��
				//o_ptr->pval = MIN(o_ptr->pval, 4);
				/* Randart amulet */
				create_artifact(o_ptr, FALSE);
			}
			else if ((power == 2) && one_in_(2))
			{
				while(!o_ptr->name2)
				{
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(21))
					{

					case 1: case 2:
						//if (have_flag(k_ptr->flags, TR_SLOW_DIGEST)) break;
						o_ptr->name2 = EGO_AMU_SENNIN;
						break;
					case 3: case 4:
						//if (o_ptr->pval) break;
						//o_ptr->name2 = EGO_AMU_INFRA;
						break;
					case 5: case 6:
						//if (have_flag(k_ptr->flags, TR_SEE_INVIS)) break;
						//o_ptr->name2 = EGO_AMU_SEE_INVIS;
						break;
					//case 7: case 8:
					//	if (have_flag(k_ptr->flags, TR_HOLD_LIFE)) break;
					//	o_ptr->name2 = EGO_AMU_HOLD_LIFE;
					//	break;
					case 7: case 8: case 9:
						if (have_flag(k_ptr->flags, TR_LEVITATION)) break;
						o_ptr->name2 = EGO_AMU_LEVITATION;
						break;
					case 10: case 11: case 21:
						o_ptr->name2 = EGO_AMU_PROTECTION;
						break;
					case 12:
						if (have_flag(k_ptr->flags, TR_RES_FIRE)) break;
						if (m_bonus(10, level) > 5)
							o_ptr->name2 = EGO_AMU_RES_FIRE_;
						else
							o_ptr->name2 = EGO_AMU_RES_FIRE;
						break;
					case 13:
						if (have_flag(k_ptr->flags, TR_RES_COLD)) break;
						if (m_bonus(10, level) > 5)
							o_ptr->name2 = EGO_AMU_RES_COLD_;
						else
							o_ptr->name2 = EGO_AMU_RES_COLD;
						break;
					case 14:
						if (have_flag(k_ptr->flags, TR_RES_ELEC)) break;
						if (m_bonus(10, level) > 5)
							o_ptr->name2 = EGO_AMU_RES_ELEC_;
						else
							o_ptr->name2 = EGO_AMU_RES_ELEC;
						break;
					case 15:
						if (have_flag(k_ptr->flags, TR_RES_ACID)) break;
						if (m_bonus(10, level) > 5)
							o_ptr->name2 = EGO_AMU_RES_ACID_;
						else
							o_ptr->name2 = EGO_AMU_RES_ACID;
						break;
					case 16: case 17: case 18: case 19: case 20:
						switch (o_ptr->sval)
						{
						case SV_AMULET_TELEPORT:
							if (m_bonus(10, level) > 9) o_ptr->name2 = EGO_AMU_D_DOOR;
							else if (weird_luck()) o_ptr->name2 = EGO_AMU_RES_TIME;
							else if(one_in_(2))o_ptr->name2 = EGO_AMU_PHASE_DOOR;
							else o_ptr->name2 = EGO_AMU_NORANDOMTELE;
							break;
						case SV_AMULET_RESIST_ACID:
							if (m_bonus(10, level) > 6) o_ptr->name2 = EGO_AMU_RES_ACID_;
							break;
						case SV_AMULET_RESIST_ELEC:
							if (m_bonus(10, level) > 6) o_ptr->name2 = EGO_AMU_RES_ELEC_;
							break;
						case SV_AMULET_SEARCHING:
							o_ptr->name2 = EGO_AMU_STEALTH;
							break;
						case SV_AMULET_BRILLIANCE:
							if (!one_in_(3)) break;
							o_ptr->name2 = EGO_AMU_IDENTIFY;
							break;
						case SV_AMULET_THE_MAGI:
							if (!one_in_(3)) break;
							if(one_in_(2)) o_ptr->name2 = EGO_AMU_OFTHEMOON;
							else o_ptr->name2 = EGO_AMU_HIGH_MAGI;
							o_ptr->pval = 2 + randint1(3);
							break;
						case SV_AMULET_RESISTANCE:
							if (one_in_(2)) break;
							if ( one_in_(2))o_ptr->name2 = EGO_AMU_DEFENDER;
							else o_ptr->name2 = EGO_AMU_OPPOSE;
							break;
						case SV_AMULET_TELEPATHY:
							if (!one_in_(3)) break;
							o_ptr->name2 = EGO_AMU_DETECTION;
							break;
						}
					}
				}
				/* Uncurse it */
				o_ptr->curse_flags = 0L;
			}
			else if ((power == -2) && one_in_(2))
			{
				if (o_ptr->to_h > 0) o_ptr->to_h = 0-o_ptr->to_h;
				if (o_ptr->to_d > 0) o_ptr->to_d = 0-o_ptr->to_d;
				if (o_ptr->to_a > 0) o_ptr->to_a = 0-o_ptr->to_a;
				if (o_ptr->pval > 0) o_ptr->pval = 0-o_ptr->pval;
				o_ptr->art_flags[0] = 0;
				o_ptr->art_flags[1] = 0;
				while(!o_ptr->name2)
				{
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(4))
					{
					case 1:
						if (have_flag(k_ptr->flags, TR_DRAIN_EXP)) break;
						o_ptr->name2 = EGO_AMU_DRAIN_EXP;
						break;
					case 2:
						o_ptr->name2 = EGO_AMU_STUPIDITY;
						break;
					case 3:
						if (have_flag(k_ptr->flags, TR_AGGRAVATE)) break;
						o_ptr->name2 = EGO_AMU_AGGRAVATE;
						break;
					case 4:
						if (have_flag(k_ptr->flags, TR_TY_CURSE)) break;
						o_ptr->name2 = EGO_AMU_TY_CURSE;
						break;

					}
				}
				/* Broken */
				o_ptr->ident |= (IDENT_BROKEN);

				/* Cursed */
				o_ptr->curse_flags |= (TRC_CURSED | TRC_HEAVY_CURSE);
			}
			break;
		}
	}

//#endif
}


/*
 * Hack -- help pick an item type
 */
//�l�`�������̃����X�^�[����
bool item_monster_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* No uniques */
	if (r_ptr->flags1 & RF1_UNIQUE) return (FALSE);
	if (r_ptr->flags7 & RF7_KAGE) return (FALSE);
	if (r_ptr->flagsr & RFR_RES_ALL) return (FALSE);
	if (r_ptr->flags7 & RF7_NAZGUL) return (FALSE);
	if (r_ptr->flags1 & RF1_FORCE_DEPTH) return (FALSE);
	if (r_ptr->flags7 & RF7_UNIQUE2) return (FALSE);
	if (r_idx == MON_TEST) return (FALSE);
	if (r_idx == MON_EXTRA_FIELD) return (FALSE);

	///mod160208 ���ꃂ���X�^�[���l�`�ɂȂ�Ȃ��悤�ǉ�
	if (r_ptr->flags7 & RF7_VARIABLE) return (FALSE);
	if (r_ptr->rarity > 100) return (FALSE);



	/* Okay */
	return (TRUE);
}

//���X�B�V���Ɖԉʎq�O��̕t�ԑΏۂɂȂ邩�ǂ������肷��
static bool monster_is_target_of_paparazzi(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//v1.1.92 ���O
	if (r_idx == MON_EXTRA_FIELD) return FALSE;
	if (r_idx == MON_SUMIREKO_2) return FALSE;

	if(r_idx == MON_MASTER_KAGUYA) return FALSE; //�i�Ԑ�p���ꃂ���X�^�[�����O
	if(is_gen_unique(r_idx)) return TRUE;
	if(r_ptr->flags7 & RF7_VARIABLE) return FALSE;//�����_�����j�[�N�A�σp�����[�^���ꃂ���X�^�[(���C�}������)�͔�Ώ�
	if(r_ptr->rarity > 100) return FALSE; //�ʏ�o�����Ȃ������X�^�[�̓p�X
	if(!r_ptr->rarity) return FALSE; //Dead monster�炵��������ȃ����X�^�[����̂��H



	return TRUE;
}

//���X�B�V���Ɖԉʎq�O��̍��������߂�B���ꃂ���X�^�[���������S�����X�^�[�̒ʂ��ԍ��ƂȂ�B
//���̔ԍ���xtra5�Ɋi�[����A�C�e�����ɕ\�������B��Ώۂ̃����X�^�[��0���Ԃ萶������Ȃ��B
int numbering_newspaper(int r_idx)
{
	int cnt;
	int num = 0;

	monster_race *r_ptr = &r_info[r_idx];

	//���̃����X�^�[���V���̎�ޑΏۂłȂ��ꍇ0��Ԃ�
	if(!monster_is_target_of_paparazzi(r_idx)) return 0;

	for(cnt=1;cnt<=r_idx;cnt++)
	{
		if(!monster_is_target_of_paparazzi(cnt)) continue;

		num++;

	}

	return (num);
}



/*
 * Apply magic to an item known to be "boring"
 *
 * Hack -- note the special code for various items
 */
/*:::����h��w�փA�~���ȊO�ɑ΂���A�C�e�������֘A����*/
///item system
static void a_m_aux_4(object_type *o_ptr, int level, int power)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Unused */
	(void)level;

	/* Apply magic (good or bad) according to type */
	switch (o_ptr->tval)
	{
		case TV_WHISTLE:
		{
#if 0
			/* Cursed */
			if (power < 0)
			{
				/* Broken */
				o_ptr->ident |= (IDENT_BROKEN);

				/* Cursed */
				o_ptr->curse_flags |= (TRC_CURSED);
			}
#endif
			break;
		}
		case TV_FLASK:
		{
			if(o_ptr->sval == SV_FLASK_OIL)
			{
				o_ptr->xtra4 = o_ptr->pval;
				o_ptr->pval = 0;
			}
		}
		break;
		case TV_LITE:
		{
			int art_chance;
			int art_level;
			/* Hack -- Torches -- random fuel */
			if (o_ptr->sval == SV_LITE_TORCH)
			{
				if (o_ptr->pval > 0) o_ptr->xtra4 = randint1(o_ptr->pval);
				o_ptr->pval = 0;
			}

			/* Hack -- Lanterns -- random fuel */
			if (o_ptr->sval == SV_LITE_LANTERN)
			{
				if (o_ptr->pval > 0) o_ptr->xtra4 = randint1(o_ptr->pval);
				o_ptr->pval = 0;
			}

			///mod150205 �^�J�̃����^���Ɩ��@�̃����^���́��ɂȂ肤��悤�ɂ���
			art_chance = 30;
			art_level = 79;
			if(ironman_no_fixed_art)
			{
				art_chance = 20;
				art_level = 29;
			}
			if(EXTRA_MODE) art_chance = art_chance * 3 / 4;


			if (power > 2 || level > art_level && one_in_(art_chance) && (o_ptr->sval == SV_LITE_FEANOR || o_ptr->sval == SV_LITE_LANTERN)) /* power > 2 is debug only */
			{
				create_artifact(o_ptr, FALSE);
			}
			else if ((power == 2) || ((power == 1) && one_in_(3)))
			{
				while (!o_ptr->name2)
				{
					while (1)
					{
						bool okay_flag = TRUE;

						o_ptr->name2 = get_random_ego(INVEN_LITE, TRUE);

						switch (o_ptr->name2)
						{
						case EGO_LITE_LONG:
							if (o_ptr->sval == SV_LITE_FEANOR)
								okay_flag = FALSE;
						}
						if (okay_flag)
							break;
					}
				}
			}
			else if (power == -2)
			{
				o_ptr->name2 = get_random_ego(INVEN_LITE, FALSE);

				switch (o_ptr->name2)
				{
				case EGO_LITE_DARKNESS:
					o_ptr->xtra4 = 0;
					break;
				}
			}

			break;
		}

		case TV_WAND:
		case TV_STAFF:
		{
			/* The wand or staff gets a number of initial charges equal
			 * to between 1/2 (+1) and the full object kind's pval. -LM-
			 */
			o_ptr->pval = k_ptr->pval / 2 + randint1((k_ptr->pval + 1) / 2);
			break;
		}

		case TV_ROD:
		{
			/* Transfer the pval. -LM- */
			o_ptr->pval = k_ptr->pval;
			break;
		}

		case TV_CAPTURE:
		{
			o_ptr->pval = 0;
			object_aware(o_ptr);
			object_known(o_ptr);
			break;
		}

		case TV_FIGURINE:
		{
			int i = 1;
			int check;

			monster_race *r_ptr;

			/* Pick a random non-unique monster race */
			while (1)
			{
				i = randint1(max_r_idx - 1);

				if (!item_monster_okay(i)) continue;
				//v1.1.53 �c�`�m�R�̐l�`���o��悤�ɂ���
				//if (i == MON_TSUCHINOKO) continue;

				r_ptr = &r_info[i];

				check = (dun_level < r_ptr->level) ? (r_ptr->level - dun_level) : 0;

				/* Ignore dead monsters */
				if (!r_ptr->rarity) continue;

				/* Ignore uncommon monsters */
				if (r_ptr->rarity > 100) continue;

				/* Prefer less out-of-depth monsters */
				if (randint0(check)) continue;

				break;
			}

			o_ptr->pval = i;

			/* Some figurines are cursed */
			if (one_in_(6)) o_ptr->curse_flags |= TRC_CURSED;

			if (cheat_peek)
			{
#ifdef JP
				msg_format("%s�̐l�`, �[�� +%d%s",
#else
				msg_format("Figurine of %s, depth +%d%s",
#endif

							  r_name + r_ptr->name, check - 1,
							  !object_is_cursed(o_ptr) ? "" : " {cursed}");
			}

			break;
		}
		///mod151213 �V���ǉ��@�l�`�ƈႢ���j�[�N�Ȃǂ��ΏہB������xtra5�ɕt�Ԃ����B
		case TV_BUNBUN:
		case TV_KAKASHI:
		{
			int cnt;
			int r_idx;
			int rlev = -1;

			monster_race *r_ptr;

			for(cnt=0;cnt<3;cnt++) //�O�񐶐����ł��n�C���x���ȃ����X�^�[�Ɍ���
			{
				while (1)
				{
					int check;
					int i = randint1(max_r_idx - 1);
					if (!monster_is_target_of_paparazzi(i)) continue;
					r_ptr = &r_info[i];
					//�l�`�̐������Ƃ�����ƕς��� ���������e�X�g���Ē����v
					check = (dun_level < r_ptr->level) ? (r_ptr->level - dun_level): 0;
					check += r_ptr->rarity / 5;
					if (randint0(check)) continue;

					if(rlev < r_ptr->level)
					{
						rlev = r_ptr->level;
						r_idx = i;
					}
					break;
				}

			}

			o_ptr->pval = r_idx;
			o_ptr->xtra5 = numbering_newspaper(r_idx);

			break;
		}

		///del131221 ���̂Ƒ�
#if 0
		case TV_CORPSE:
		{
			int i = 1;
			int check;

			u32b match = 0;

			monster_race *r_ptr;

			if (o_ptr->sval == SV_SKELETON)
			{
				match = RF9_DROP_SKELETON;
			}
			else if (o_ptr->sval == SV_CORPSE)
			{
				match = RF9_DROP_CORPSE;
			}

			/* Hack -- Remove the monster restriction */
			get_mon_num_prep(item_monster_okay, NULL);

			/* Pick a random non-unique monster race */
			while (1)
			{
				i = get_mon_num(dun_level);

				r_ptr = &r_info[i];

				check = (dun_level < r_ptr->level) ? (r_ptr->level - dun_level) : 0;

				/* Ignore dead monsters */
				if (!r_ptr->rarity) continue;

				/* Ignore corpseless monsters */
				if (!(r_ptr->flags9 & match)) continue;

				/* Prefer less out-of-depth monsters */
				if (randint0(check)) continue;

				break;
			}

			o_ptr->pval = i;

			if (cheat_peek)
			{
#ifdef JP
				msg_format("%s�̎���, �[�� +%d",
#else
				msg_format("Corpse of %s, depth +%d",
#endif

							  r_name + r_ptr->name, check - 1);
			}

			object_aware(o_ptr);
			object_known(o_ptr);
			break;
		}
		///sysdel
		case TV_STATUE:
		{
			int i = 1;

			monster_race *r_ptr;

			/* Pick a random monster race */
			while (1)
			{
				i = randint1(max_r_idx - 1);

				r_ptr = &r_info[i];

				/* Ignore dead monsters */
				if (!r_ptr->rarity) continue;

				break;
			}

			o_ptr->pval = i;

			if (cheat_peek)
			{
#ifdef JP
				msg_format("%s�̑�", r_name + r_ptr->name);
#else
				msg_format("Statue of %s", r_name + r_ptr->name);
#endif

			}
			object_aware(o_ptr);
			object_known(o_ptr);

			break;
		}
#endif
		case TV_CHEST:
		{
			//���΂��ꂽ��:0 �����Ȗ�:5 �傫�Ȗ�:15 �����ȓS:25 �傫�ȓS:35 �����ȍ|�S:45 �傫�ȍ|�S:55
			byte obj_level = k_info[o_ptr->k_idx].level;

			/* Hack -- skip ruined chests */
			if (obj_level <= 0) break;

			/* Hack -- pick a "difficulty" */
			o_ptr->pval = randint1(obj_level);

			//v1.1.97 �����n�C���x���ȃg���b�v���o���₷������
			if (obj_level > 40 && o_ptr->pval < (obj_level / 2) && one_in_(2)) o_ptr->pval += obj_level / 2;


			//if (o_ptr->sval == SV_CHEST_KANDUME) o_ptr->pval = 6;
			///mod140117 ���ꔠ��㩂��d�|�����Ă��Ȃ�
			///mod140323 �{��������ĂȂ����Ƃɂ���
			if (o_ptr->sval > SV_CHEST_L_STEEL ) o_ptr->pval = -6;

			o_ptr->xtra3 = dun_level + 5;

			/* Never exceed "difficulty" of 55 to 59 */
			//����@�\���Ă���̂��H�ő�ł�55�̂͂�����
			if (o_ptr->pval > 55) o_ptr->pval = 55 + (byte)randint0(5);

			///mod140629 �ϒK�푈
			/*:::-Hack-�@�ϒK�푈�̕�V�������`�[���ɂ���ĕς��邽��xtra5�փt���O��ݒ�*/
			if(o_ptr->sval == SV_CHEST_KORI)
			{
				if(quest[QUEST_KORI].flags & QUEST_FLAG_TEAM_A) o_ptr->xtra5 = 1;
				else if(quest[QUEST_KORI].flags & QUEST_FLAG_TEAM_B) o_ptr->xtra5 = 2;
				else  o_ptr->xtra5 = 3;
			}
			///v1.1.18 �푰/�N���X����N�G�X�g
			/*::: -Hack-�@������QUEST_STATUS_REWARDED(��V������)�ɂȂ��Ă���N�G�X�g��T���ăN�G�X�gidx�𔠂�xtra5�Ɋi�[����B�����󂯂�Ƃ����̒l���Q�Ƃ��ĕ�V�𕪊򂳂���B*/
			else if(o_ptr->sval == SV_CHEST_SPECIAL_QUEST)
			{
				int quest_idx;
				for (quest_idx = 0; quest_idx < max_quests; quest_idx++)
				{
					if (quest[quest_idx].status == QUEST_STATUS_REWARDED) break;
				}
				if(quest_idx == max_quests)
				{
					msg_print("ERROR:�푰�E�N���X����N�G�X�g�̕�V�����Ɏ��s����");
					o_ptr->xtra5 = 0;
					break;
				}

				o_ptr->xtra5 = quest_idx;

			}

			break;
		}
	}
}



/*:::���{���p��apply_magic()�⏕���[�`��*/
static void a_m_aux_5(object_type *o_ptr, int level, int power)
{
	int chance;
	int chance_art;
	int random_stat[9] = {TR_STR,TR_INT,TR_WIS,TR_DEX,TR_CON,TR_CHR,TR_SEARCH,TR_STEALTH,TR_SPEED};

	if(power>1) o_ptr->to_a = 5 + m_bonus(5,level);
	else if(power>0) o_ptr->to_a = randint1(5);	
	else if(power<0) o_ptr->to_a = 0 -10 - randint1(10);

	chance_art = 30 - level / 12;
	if(ironman_no_fixed_art) chance_art = chance_art * 2 / 3;
	if(EXTRA_MODE) chance_art = chance_art * 3 / 4;

	if(power>2 || power == 2 && one_in_(chance_art))
	{
		create_artifact(o_ptr,FALSE);
	}
	else if(power == 2)
	{
		chance = m_bonus(10,level/2);
		while(weird_luck())chance++;
		/*:::�ԁ`���@�\�͈�㏸*/
		if(chance < 1)
		{
			if(one_in_(6)) 		o_ptr->name2 = EGO_RIBBON_RED;
			else if(one_in_(5)) 	o_ptr->name2 = EGO_RIBBON_PURPLE;
			else if(one_in_(4)) 	o_ptr->name2 = EGO_RIBBON_YELLOW;
			else if(one_in_(3)) 	o_ptr->name2 = EGO_RIBBON_BLUE;
			else if(one_in_(2)) 	o_ptr->name2 = EGO_RIBBON_GREEN;
			else  		 	o_ptr->name2 = EGO_RIBBON_ORANGE;
			o_ptr->pval = 1 + m_bonus(5,level);
		}
		/*:::�아�̃��{���@AC�����A�����_���\�́A�����_���ϐ�*/
		else if(chance < 2)
		{
			o_ptr->name2 = EGO_RIBBON_CHARM;
			o_ptr->to_a += m_bonus(5,level) + randint1(5);
			one_resistance(o_ptr);
			one_ability(o_ptr);

		}
		/*:::�������{���@�\�͓�㏸*/
		else if(chance < 3)
		{
			int stat1=0,stat2=0;
			o_ptr->name2 = EGO_RIBBON_WHITE;
			o_ptr->pval = 1 + m_bonus(3,level) + randint0(2);
			stat1 = randint0(8);
			do{stat2 = randint0(8);}while(stat1==stat2);
	 		add_flag(o_ptr->art_flags, random_stat[stat1]);
			add_flag(o_ptr->art_flags, random_stat[stat2]);		
		}
		/*:::���F�̃��{���@���f�ϐ��A�m���œőϐ�*/
		else if(chance < 4)
		{
			o_ptr->name2 = EGO_RIBBON_RAINBOW;
			if(one_in_(4))  add_flag(o_ptr->art_flags, TR_RES_POIS);
		}
		/*:::�������{���@��������*/
		else if(chance < 5)
		{
			o_ptr->name2 = EGO_RIBBON_BLACK;
			o_ptr->pval = randint1(4) + m_bonus(6,level);
			while(o_ptr->pval<=50 && one_in_(3)) o_ptr->pval+=1;
		}
		/*:::�P�����{���@�\�͎O�㏸*/
		else if(chance < 7)
		{
			int stat1=0,stat2=0,stat3=0;
			o_ptr->name2 = EGO_RIBBON_BRIGHT;
			o_ptr->pval = 1 + m_bonus(3,level) + randint0(2);
			stat1 = randint0(9);
			do{stat2 = randint0(9);}while(stat1==stat2);
			do{stat3 = randint0(9);}while(stat1==stat3||stat2==stat3);
	 		add_flag(o_ptr->art_flags, random_stat[stat1]);
			add_flag(o_ptr->art_flags, random_stat[stat2]);		
			add_flag(o_ptr->art_flags, random_stat[stat3]);		
		}
		/*:::��l�̃��{���@AC�����A�����_���\�́A�����_���ϐ�1-2�A�\��1-2�㏸*/
		else 
		{
			int stat1=0,stat2=0;
			o_ptr->name2 = EGO_RIBBON_PATTERN;
			o_ptr->to_a += m_bonus(5,level) + randint1(5);
			one_resistance(o_ptr);
			if(one_in_(2)) one_resistance(o_ptr);
			one_ability(o_ptr);
	 		add_flag(o_ptr->art_flags, random_stat[randint0(9)]);
	 		if(one_in_(2)) add_flag(o_ptr->art_flags, random_stat[randint0(9)]);
			o_ptr->pval = 1 + m_bonus(3,level) + randint0(2);

		}

	}
	/*:::�㎿�����ł��G�S�ɂȂ邱�Ƃ�����*/
	else if(power == 1 && one_in_(3))
	{
		if(one_in_(6)) 		o_ptr->name2 = EGO_RIBBON_RED;
		else if(one_in_(5)) 	o_ptr->name2 = EGO_RIBBON_PURPLE;
		else if(one_in_(4)) 	o_ptr->name2 = EGO_RIBBON_YELLOW;
		else if(one_in_(3)) 	o_ptr->name2 = EGO_RIBBON_BLUE;
		else if(one_in_(2)) 	o_ptr->name2 = EGO_RIBBON_GREEN;
		else  		 	o_ptr->name2 = EGO_RIBBON_ORANGE;
		o_ptr->pval = 1 + m_bonus(4,level);
	}
	/*:::�N���G�S�@*/
	else if(power == -2 && m_bonus(10,level/2) > 5)
	{
		o_ptr->name2 = EGO_RIBBON_BLOOD;
		o_ptr->to_h =  randint1(10) + m_bonus(10,level);
		o_ptr->to_d =  randint1(10) + m_bonus(10,level);
		o_ptr->to_a = 0 - damroll(7,7);
		add_flag(o_ptr->art_flags, TR_SHOW_MODS);
		if(one_in_(2)) add_flag(o_ptr->art_flags, TR_RES_NETHER);
		o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
		if(weird_luck())
		{
			o_ptr->to_h += 10 + randint1(5);
			o_ptr->to_d += 10 + randint1(5);
			o_ptr->to_a -=  damroll(7,7);
			if(!weird_luck()) add_flag(o_ptr->art_flags, TR_TY_CURSE);
			if(!weird_luck()) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
		}
	}
	else if(power < 0)
	{
		if (power < 0) o_ptr->curse_flags |= (TRC_CURSED);
		//v1.1.62 �􂢃��{����power=-2�Ő�������Ď�����ɋ����􂢂������̂Œ�����

	}

}






/*
 * Complete the "creation" of an object by applying "magic" to the item
 *
 * This includes not only rolling for random bonuses, but also putting the
 * finishing touches on ego-items and artifacts, giving charges to wands and
 * staffs, giving fuel to lites, and placing traps on chests.
 *:::�G�S�A�C�e����A�[�e�B�t�@�N�g�ւ̃p�����[�^�t�^�A������̏[�U�Ȃǂ��s��
 *
 * In particular, note that "Instant Artifacts", if "created" by an external
 * routine, must pass through this function to complete the actual creation.
 *:::��p�x�[�X�̃A�[�e�B�t�@�N�g��������ʂ�K�v������
 *
 * The base "chance" of the item being "good" increases with the "level"
 * parameter, which is usually derived from the dungeon level, being equal
 * to the level plus 10, up to a maximum of 75.  If "good" is true, then
 * the object is guaranteed to be "good".  If an object is "good", then
 * the chance that the object will be "great" (ego-item or artifact), also
 * increases with the "level", being equal to half the level, plus 5, up to
 * a maximum of 20.  If "great" is true, then the object is guaranteed to be
 * "great".  At dungeon level 65 and below, 15/100 objects are "great".
 *
 * If the object is not "good", there is a chance it will be "cursed", and
 * if it is "cursed", there is a chance it will be "broken".  These chances
 * are related to the "good" / "great" chances above.
 *
 * Otherwise "normal" rings and amulets will be "good" half the time and
 * "cursed" half the time, unless the ring/amulet is always good or cursed.
 *
 * If "okay" is true, and the object is going to be "great", then there is
 * a chance that an artifact will be created.  This is true even if both the
 * "good" and "great" arguments are false.  As a total hack, if "great" is
 * true, then the item gets 3 extra "attempts" to become an artifact.
 */
/*:::����ς݂̃x�[�X�A�C�e���ɑ΂��G�S�A�C�e�������o_ptr�ւ̃p�����[�^����U��A
/*:::�Œ�A�[�e�B�t�@�N�g�̂Ƃ����ۂ�o_ptr�փp�����[�^����U��A
/*:::������񐔂⃉���^���̔R���⋋�Ȃǂ��s���B
/*:::�Ȃ��A�����i�x�[�X��AC��pval�Ȃǂ͂����ɓ���O��object_prep()��k_info[]����o_ptr�փR�s�[����Ă���B
/*:::mode:AM_GOOD�Ȃǂ̐����t���O*/
///item seikaku mutation 
///mod131223 �A�C�e���e�[�u���֘A
void apply_magic(object_type *o_ptr, int lev, u32b mode)
{
	bool miko= FALSE;
	int i, rolls, f1, f2, power;
	/*:::	f1 �㎿���� level+10�@����_���W�����ˑ��@�S��75?
	 *:::		f2 �����i���� (level+10)* 2/3 ����_���W�����ˑ��@�S��20?
	 *:::		power: 1�㎿ 2�����i 3�K�����ʐ��i�f�o�b�O�R�}���h�̂�)
	 */
	///seikaku del
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) lev += randint0(p_ptr->lev/2+10);

	/* Maximum "level" for various things */
	if (lev > MAX_DEPTH - 1) lev = MAX_DEPTH - 1;

	/* Base chance of being "good" */
	f1 = lev + 10;


	/* Maximal chance of being "good" */
	if (f1 > d_info[dungeon_type].obj_good) f1 = d_info[dungeon_type].obj_good;

	/* Base chance of being "great" */
	f2 = f1 * 2 / 3;

	//�A�C�e���J�[�h���ꏈ�� r_idx=0
	if(o_ptr->tval == TV_ITEMCARD) 
	{
		apply_magic_itemcard(o_ptr, lev, 0);
		return;
	}
	//v1.1.86 �A�r���e�B�J�[�h�ւ̏���
	if (o_ptr->tval == TV_ABILITY_CARD)
	{
		apply_magic_abilitycard(o_ptr, -1, 0, 0);
		return;
	}

	/* Maximal chance of being "great" */
	///seikaku del
	if ((p_ptr->pseikaku != SEIKAKU_MUNCHKIN) && (f2 > d_info[dungeon_type].obj_great))
		f2 = d_info[dungeon_type].obj_great;

	if (p_ptr->lucky || (p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 6))
	{
		f1 += 10;
		f2 += 5;
	}
	else if (p_ptr->muta3 & MUT3_GOOD_LUCK || (p_ptr->pclass == CLASS_REIMU && osaisen_rank() > 4))
	{
		f1 += 5;
		f2 += 2;
	}
	else if(p_ptr->muta3 & MUT3_BAD_LUCK || p_ptr->today_mon == FORTUNETELLER_BADLUCK)
	{
		f1 -= 5;
		f2 -= 2;
	}

	if (p_ptr->today_mon == FORTUNETELLER_GOODLUCK)
	{
		f1 += 5;
		f2 += 2;
	}

	/*:::Hack �����˂͗ǂ��A�C�e�����o�₷��*/
	if (dun_level > 0 && dungeon_type == DUNGEON_MUEN)
	{
		f1 += 5;
		f2 += 5;
	}

	//EXTRA���[�h�͗ǂ������o�₷��
	if(EXTRA_MODE)
	{
		f1 *= 2;
		f2 *= 2;
		if(f1 < 30) f1 = 30;
		if(f2 < 20) f2 = 20;

	}

	//v1.1.43 EXTRA���ׂ̗̈�̏ꍇHARD���Ⴍ�Ȃ��Ă����̂ŏC�� ���łɏ���l��S�̂ɓK�p
	if (f1 > 99) f1 = 99;
	if (f2 > 75) f2 = 75;

	/* Assume normal */
	power = 0;

	/* Roll for "good" */
	/*:::magik(P)�FP��0-99�̗������傫�����True ���̃t�@�C���ł����g���ĂȂ�*/
	if ((mode & AM_GOOD) || magik(f1))
	{
		/* Assume "good" */
		power = 1;

		/* Roll for "great" */
		if ((mode & AM_GREAT) || magik(f2))
		{
			power = 2;

			/* Roll for "special" */
			/*:::�K�����ʐ��ɂȂ�t���O�H�P���Ɍ����������̓E�B�U�[�h���[�h�ł����g���ĂȂ��B*/
			if (mode & AM_SPECIAL) power = 3;
		}
	}

	/* Roll for "cursed" */
	else if (magik(f1))
	{
		/* Assume "cursed" */
		power = -1;

		/* Roll for "broken" */
		if (magik(f2)) power = -2;
	}

	//v1.1.48 �����͎��ꂽ�A�C�e�����o�₷��
	if (p_ptr->pclass == CLASS_SHION)
	{
		if (one_in_(13)) power = -2;
		else if (one_in_(6)) power = -1;

	}


	/* Apply curse */
	/*:::���̃t���O�͏�̏㎿������㏑������ �E�B�U�[�h�R�}���h���炵���g���Ă��Ȃ��͗l*/
	if (mode & AM_CURSED)
	{
		/* Assume 'cursed' */
		if (power > 0)
		{
			power = 0 - power;
		}
		/* Everything else gets more badly cursed */
		else
		{
			power--;
		}
	}


	/* Assume no rolls */
	rolls = 0;

	/* Get one roll if excellent */
	if (power >= 2) 
	{
		rolls = 1;
		/*:::Hack �����˂͗ǂ��A�C�e�����o�₷��*/
		if (dun_level > 0 && dungeon_type == DUNGEON_MUEN) rolls++;
		//v1.1.20 EXTRA�̃��[���{�[�i�X��߂�
		//if (EXTRA_MODE && one_in_(2)) rolls++;
	}
	/* Hack -- Get four rolls if forced great or special */
	if (mode & (AM_GREAT | AM_SPECIAL)) rolls = 4;

	/* Hack -- Get no rolls if not allowed */
	/*:::��ʃA�C�e���A���łɌŒ�A�[�e�B�t�@�N�g�ƌ��܂��Ă�A�C�e���͎��̃��[����ʂ�Ȃ�*/
	if ((mode & AM_NO_FIXED_ART) || o_ptr->name1) rolls = 0;

	/*:::�����I�ɕ��̕i�𐶐�����i�����Ȃǂ̏[�U�����͍s����j*/
	if (mode & AM_FORCE_NORMAL)
	{
		rolls = 0;
		power = 0;
	}

	/* Roll for artifacts if allowed */
	/*:::�Œ�A�[�e�B�t�@�N�g�̐�������*/
	for (i = 0; i < rolls; i++)
	{
		/* Roll for an artifact */
		if (make_artifact(o_ptr)) break;
		if ((p_ptr->muta3 & MUT3_GOOD_LUCK) && one_in_(77))
		{
			if (make_artifact(o_ptr)) break;
		}
	}


	/* Hack -- analyze artifacts */
	/*:::�Œ�A�[�e�B�t�@�N�g�̏ꍇ�A�p�����[�^��o_ptr�Ɋi�[���ďI��*/
	if (object_is_fixed_artifact(o_ptr))
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		/* Hack -- Mark the artifact as "created" */
		a_ptr->cur_num = 1;

		/* Hack -- Memorize location of artifact in saved floors */
		if (character_dungeon)
			a_ptr->floor_id = p_ptr->floor_id;

		/* Extract the other fields */
		o_ptr->pval = a_ptr->pval;
		o_ptr->ac = a_ptr->ac;
		o_ptr->dd = a_ptr->dd;
		o_ptr->ds = a_ptr->ds;
		o_ptr->to_a = a_ptr->to_a;
		o_ptr->to_h = a_ptr->to_h;
		o_ptr->to_d = a_ptr->to_d;
		o_ptr->weight = a_ptr->weight;
		o_ptr->xtra2 = a_ptr->act_idx;

		/* Hack -- extract the "broken" flag */
		if (!a_ptr->cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- extract the "cursed" flag */
		if (a_ptr->gen_flags & TRG_CURSED) o_ptr->curse_flags |= (TRC_CURSED);
		if (a_ptr->gen_flags & TRG_HEAVY_CURSE) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
		if (a_ptr->gen_flags & TRG_PERMA_CURSE) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
		if (a_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
		if (a_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
		if (a_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);

		///mod150908 �N���E���s�[�X�̏����̓��ꏈ�� 
		if(o_ptr->name1 == ART_CLOWNPIECE) o_ptr->xtra4 = -1;


		/* Cheat -- peek at the item */
		if (cheat_peek) object_mention(o_ptr);

		/* Done */
		return;
	}

	///mod131230 �x�[�X�A�C�e���ɑ΂����m���ŕt�������t���O�Ȃǂ̏�����ǉ����Ă݂�
	if(!(mode & AM_FORCE_NORMAL)) add_art_flags(o_ptr);

	/* Apply magic */
	/*:::�x�[�X�A�C�e�����ނɏ]���G�S�A���A���̑��p�����[�^�������s���Ă���*/
	///item 
	switch (o_ptr->tval)
	{

	case TV_BULLET:
	case TV_ARROW:
	case TV_BOLT:
	case TV_BOW:
	case TV_CROSSBOW:
	case TV_GUN:

	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_OTHERWEAPON:	
		{
			/*:::�_�C�A�����h�G�b�W��pval�͂����ɓ���O��object_prep()�œK�p����Ă���*/
			if (power) a_m_aux_1(o_ptr, lev, power);
			break;
		}

	case TV_KNIFE:
		{
			if (power && !(o_ptr->sval == SV_WEAPON_DOKUBARI)) a_m_aux_1(o_ptr, lev, power);
			else if(o_ptr->sval == SV_WEAPON_DRAGON_DAGGER)a_m_aux_1(o_ptr, lev, power);
			break;
		}

	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HEAD:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_CLOTHES:
	case TV_ARMOR:
	case TV_DRAG_ARMOR:
		{
			/* Elven Cloak and Black Clothes ... */
			/*:::�G���t�N���[�N�i�B�ꖪ�j�ƍ������ɑ΂�pval��K�p���Ă���B*/
			if (((o_ptr->tval == TV_CLOAK) && (o_ptr->sval == SV_CLOAK_STEALTH)) ||
			    ((o_ptr->tval == TV_CLOTHES) && (o_ptr->sval == SV_CLOTH_KUROSHOUZOKU)))
				o_ptr->pval = randint1(4);

#if 1
			if (power ||
			     ((o_ptr->tval == TV_HEAD) && (o_ptr->sval == SV_HEAD_DRAGONHELM)) ||
			     ((o_ptr->tval == TV_SHIELD) && (o_ptr->sval == SV_DRAGON_SHIELD)) ||
			     ((o_ptr->tval == TV_GLOVES) && (o_ptr->sval == SV_HAND_DRAGONGLOVES)) ||
			     ((o_ptr->tval == TV_BOOTS) && (o_ptr->sval == SV_LEG_DRAGONBOOTS)))
				a_m_aux_2(o_ptr, lev, power);
#else
			if (power) a_m_aux_2(o_ptr, lev, power);
#endif
			break;
		}
		case TV_RING:
		case TV_AMULET:
		{
			if (!(mode & AM_FORCE_NORMAL) && !power && (randint0(100) < 50)) power = -1;
			a_m_aux_3(o_ptr, lev, power);
			break;
		}
		case TV_RIBBON:
		{
			if(power)a_m_aux_5(o_ptr,lev,power);
			break;
		}
		//v1.1.60 ������W�i�@�K�����ɂȂ�
		case TV_ANTIQUE:
			create_artifact(o_ptr, FALSE);
			break;

		default:
		{
			a_m_aux_4(o_ptr, lev, power);
			break;
		}
	}
	///del ��Ȃ������̓��ꏈ�������������폜����
	///mod140524 �ޏ��͛ޏ����Ƀ{�[�i�X
	if ((p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_SANAE) && (p_ptr->psex == SEX_FEMALE) && 
		(o_ptr->tval == TV_CLOTHES) &&
	    (o_ptr->sval == SV_CLOTH_MIKO) 
		)
	{
		miko = TRUE;
		if(!o_ptr->pval) o_ptr->pval = 1 + m_bonus(2,lev);
		add_flag(o_ptr->art_flags, TR_STR);
		add_flag(o_ptr->art_flags, TR_INT);
		add_flag(o_ptr->art_flags, TR_WIS);
		add_flag(o_ptr->art_flags, TR_DEX);
		add_flag(o_ptr->art_flags, TR_CON);
		add_flag(o_ptr->art_flags, TR_CHR);
	}

	/*:::�J�n��(FORCE_NORMAL)��pval�t���̑����������Ďn�܂�Ƃ��Apval�������I��1�ɂ���(���̂Ƃ������ɂ͗��Ȃ�)*/
	///mod150201 �H����pval�܂�1�ɂȂ��Ă��܂����̂ŏC��
	if(object_is_weapon_armour_ammo(o_ptr) && o_ptr->pval && (mode & AM_FORCE_NORMAL)) o_ptr->pval = 1;

	/* Hack -- analyze ego-items */
	/*:::�G�S�A�C�e���t���O�����Ȃ�*/
	if (object_is_ego(o_ptr))
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Hack -- acquire "broken" flag */
		if (!e_ptr->cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- acquire "cursed" flag */
		if (e_ptr->gen_flags & TRG_CURSED) o_ptr->curse_flags |= (TRC_CURSED);
		if (e_ptr->gen_flags & TRG_HEAVY_CURSE) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
		if (e_ptr->gen_flags & TRG_PERMA_CURSE) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
		if (e_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
		if (e_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
		if (e_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);

		if (e_ptr->gen_flags & (TRG_ONE_SUSTAIN)) one_sustain(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_POWER)) one_ability(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_H_RES)) one_high_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_E_RES)) one_ele_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_D_RES)) one_dragon_ele_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_L_RES)) one_lordly_high_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_RES)) one_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_DICE))
		{
			do
			{
				o_ptr->dd++;
			}
			while (one_in_(o_ptr->dd));
			if (o_ptr->dd > 9) o_ptr->dd = 9;
		}

		/* Hack -- apply activatin index if needed */
		if (e_ptr->act_idx) o_ptr->xtra2 = e_ptr->act_idx;

		/* Hack -- apply extra penalties if needed */
		/*:::�􂢃G�S�̃p�����[�^���������@�א_�̉���̎w�ւ�����*/
		if ((object_is_cursed(o_ptr) || object_is_broken(o_ptr)) && o_ptr->name2 != EGO_RING_GALAXY2)
		{
			/* Hack -- obtain bonuses */
			if (e_ptr->max_to_h) o_ptr->to_h -= randint1(e_ptr->max_to_h);
			if (e_ptr->max_to_d) o_ptr->to_d -= randint1(e_ptr->max_to_d);
			if (e_ptr->max_to_a) o_ptr->to_a -= randint1(e_ptr->max_to_a);

			/* Hack -- obtain pval */
			///mod140102 �y�w偃G�S���B�ꖪ�ɂ����Ƃ�pval���v���X�ɂȂ�Ȃ��悤�ɂ��Ă���
			if (e_ptr->max_pval) o_ptr->pval =  -(randint1(e_ptr->max_pval));
			//if (e_ptr->max_pval) o_ptr->pval -= randint1(e_ptr->max_pval);

			///mod131225 ���̃f�����n���G�S�ɎE�C�C����������
			if ((o_ptr->tval == TV_SHIELD) && (o_ptr->name2 == EGO_SHIELD_DULLAHAN))
			{
				o_ptr->to_d = m_bonus(10, lev) + randint1(5);
			}
			///mod131225 �C�̉ΎԃG�S��pval��������
			if ((o_ptr->tval == TV_BOOTS) && (o_ptr->name2 == EGO_BOOTS_KASYA))
			{
				o_ptr->pval += 2 + randint1(3);
			}
			///mod131225 �O���[�u�̋֊��G�S�@���܂ɎE�C�C�����Ԃ���Ԃ悤��
			if ((o_ptr->tval == TV_GLOVES) && (o_ptr->name2 == EGO_HANDS_FORBIDDEN))
			{
				o_ptr->to_h = 5 - randint1(10);
				while(one_in_(2) && o_ptr->to_h > -95) o_ptr->to_h -= 5;
				//v1.1.99 ���������_���[�W���₷
				o_ptr->to_d = 10 + randint1(15);
				while(one_in_(2) && o_ptr->to_d < 45) o_ptr->to_d += 5;
				if(o_ptr->to_d > 20 && randint0(o_ptr->to_d) > 5) add_flag(o_ptr->art_flags, TR_TY_CURSE);
				if(o_ptr->to_d > 30) o_ptr->to_a -= (o_ptr->to_d - 20);
				if(o_ptr->to_d > 40) o_ptr->curse_flags |= TRC_PERMA_CURSE;
			}

		}

		/* Hack -- apply extra bonuses if needed */
		else
		{
			/* Hack -- obtain bonuses */

			if (e_ptr->max_to_h)
			{
				/*:::�啃N���[�N�ȂǂŃG�S�̒ǉ��{�[�i�X�����̂Ƃ���byte�^�Ή��̕����ϊ������炵��*/
				if (e_ptr->max_to_h > 127)
					o_ptr->to_h -= randint1(256-e_ptr->max_to_h);
				else o_ptr->to_h += randint1(e_ptr->max_to_h);
			}
			if (e_ptr->max_to_d)
			{
				if (e_ptr->max_to_d > 127)
					o_ptr->to_d -= randint1(256-e_ptr->max_to_d);
				else o_ptr->to_d += randint1(e_ptr->max_to_d);
			}
			if (e_ptr->max_to_a)
			{
				if (e_ptr->max_to_a > 127)
					o_ptr->to_a -= randint1(256-e_ptr->max_to_a);
				else o_ptr->to_a += randint1(e_ptr->max_to_a);
			}

			/* Hack -- obtain pval */
			/*:::����Ă��Ȃ��A�C�e����pval�����B�x�[�X�A�C�e����pval������A�C�e���ɒ��ӁB*/
			///mod140102 �q�q�C���J�l�E�B�ꖪ�E�������ɍ��킹����
			if (e_ptr->max_pval)
			{
				if ((o_ptr->name2 == EGO_WEAPON_HERO) && (have_flag(o_ptr->art_flags, TR_BLOWS)))
				{
					o_ptr->pval = 1;
					if ((lev > 60) && one_in_(3) && ((o_ptr->dd*(o_ptr->ds+1)) < 15)) o_ptr->pval++;					
					//o_ptr->pval++;
					//if ((lev > 60) && one_in_(3) && ((o_ptr->dd*(o_ptr->ds+1)) < 15)) o_ptr->pval++;
				}
				else if (o_ptr->name2 == EGO_WEAPON_EXATTACK)
				{
					o_ptr->pval = randint1(e_ptr->max_pval*lev/100+1);
					if (o_ptr->pval > 3) o_ptr->pval = 3;
					if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_HAYABUSA))
					{
						///mod151108 +5�U�͂������ɖő��ɏo�Ȃ��悤�ɂ��Ƃ�
						o_ptr->pval++;
						if(weird_luck()) o_ptr->pval++;
					}
				}
				else if (o_ptr->name2 == EGO_CLOAK_VAMPIRE)
				{
					o_ptr->pval = randint1(e_ptr->max_pval);
					if (o_ptr->sval == SV_CLOAK_STEALTH) o_ptr->pval += randint1(2);
				}
				//�q�q�C���J�l���G�S�ɂȂ����Ƃ��G�S�̒l��pval4�ɑ������pval��������΂Ȃ����߂̒ǉ�����
				else if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_WEAPON_HIHIIROKANE))
				{
					o_ptr->pval = 1 + randint1(e_ptr->max_pval);
				}
				//�����������̃G�S�ɏd�Ȃ���pval���オ�肷���Ȃ����߂̏���
				else if((o_ptr->tval == TV_CLOTHES) && (o_ptr->sval == SV_CLOTH_KUROSHOUZOKU))
				{
					if(o_ptr->name2 == EGO_BODY_GATEKEEPER || o_ptr->name2 == EGO_BODY_ONI || o_ptr->name2 == EGO_BODY_MAKAISHIN ||  o_ptr->name2 == EGO_BODY_KISHIN)
						o_ptr->pval = 1 + randint1(e_ptr->max_pval-1);
					else o_ptr->pval += randint1(e_ptr->max_pval-1);
				}
				//�ʏ�̃A�C�e���ɕt�����G�S��pval�t������
				else
				{
					o_ptr->pval += randint1(e_ptr->max_pval);
				}
			}
			/*:::��w�ŃX�s�[�h�̌C���o����pval�ł�����x���[�����č����l���o�ɂ������Ă���*/
			if ((o_ptr->name2 == EGO_BOOTS_RAVEN) && (lev < 50))
			{
				o_ptr->pval = randint1(o_ptr->pval);
			}
			if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_HAYABUSA) && (o_ptr->pval > 2) && (o_ptr->name2 != EGO_WEAPON_EXATTACK))
				o_ptr->pval = 2;
		}

		/* Cheat -- describe the item */
		if (cheat_peek) object_mention(o_ptr);

		//�ޏ������Ƀ{�[�i�X�������Ƃ�����G�S�ŋ�������h��o����̂�}�~
		if(miko && o_ptr->pval > 3) o_ptr->pval = 3;

		/* Done */
		return;
	}
	
	//test
	//if (o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_T_SHIRT) msg_print("chk");

	/* Examine real objects */
	if (o_ptr->k_idx)
	{
		object_kind *k_ptr = &k_info[o_ptr->k_idx];

		/* Hack -- acquire "broken" flag */
		if (!k_info[o_ptr->k_idx].cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- acquire "cursed" flag */
		if (k_ptr->gen_flags & (TRG_CURSED)) o_ptr->curse_flags |= (TRC_CURSED);
		if (k_ptr->gen_flags & (TRG_HEAVY_CURSE)) o_ptr->curse_flags |= TRC_HEAVY_CURSE;
		if (k_ptr->gen_flags & (TRG_PERMA_CURSE)) o_ptr->curse_flags |= TRC_PERMA_CURSE;
		if (k_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
		if (k_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
		if (k_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);
	}


	//v2.1.0 ����̓A�C�e���̎􂢂𖳌��� �􂢃t���O��object_flags()�Ŗ�������̂ł����ł�pval�Ȃǂ̃}�C�i�X��0�ɂ���
	if (p_ptr->pclass == CLASS_MIZUCHI && object_is_equipment(o_ptr))
	{
		if (o_ptr->to_a < 0) o_ptr->to_a = 0;
		if (o_ptr->to_d < 0) o_ptr->to_d = 0;
		if (o_ptr->to_h < 0) o_ptr->to_h = 0;
		if (o_ptr->pval < 0) o_ptr->pval = 0;
	}

}


/*
 * Hack -- determine if a template is "good"
 */
/*:::�u�㎿�ȁv�h���b�v�Ɏg���Ă����x�[�X�A�C�e���Ȃ�true*/
///item
///mod131223 TVAL�֘A
static bool kind_is_good(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	switch (k_ptr->tval)
	{
		/* Armor -- Good unless damaged */
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HEAD:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_CLOTHES:
	case TV_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_RIBBON:
		{
			if (k_ptr->to_a < 0) return (FALSE);
			return (TRUE);
		}

		/* Weapons -- Good unless damaged */

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
		{
			if (k_ptr->to_h < 0) return (FALSE);
			if (k_ptr->to_d < 0) return (FALSE);
			return (TRUE);
		}

		/* Ammo -- Arrows/Bolts are good */
	case TV_BULLET:
	case TV_ARROW:
	case TV_BOLT:
		{
			return (TRUE);
		}

		/* Books -- High level books are good (except Arcane books) */

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

/*
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_CRAFT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		case TV_HEX_BOOK:
*/
		{
			if (k_ptr->sval >= SV_BOOK_MIN_GOOD) return (TRUE);
			return (FALSE);
		}

		/* Rings -- Rings of Speed are good */
		case TV_RING:
		{
			if (k_ptr->sval == SV_RING_SPEED) return (TRUE);
			if (k_ptr->sval == SV_RING_LORDLY) return (TRUE);
			return (FALSE);
		}

		/* Amulets -- Amulets of the Magi and Resistance are good */
		case TV_AMULET:
		{
			if (k_ptr->sval == SV_AMULET_THE_MAGI) return (TRUE);
			if (k_ptr->sval == SV_AMULET_RESISTANCE) return (TRUE);
			return (FALSE);
		}
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Attempt to make an object (normal or good/great)
 *
 * This routine plays nasty games to generate the "special artifacts".
 *
 * This routine uses "object_level" for the "generation level".
 *
 * We assume that the given object has been "wiped".
 */
/*:::�����_���ȃA�C�e���𐶐�����B�������x����object_level���g�p����B*/
/*:::�A�C�e�����w�肵������΂�����ĂԑO��get_obj_num_hook�Ȃǂ�hook�֐����w�肷��*/
/*:::mode:AM_????�̃t���O*/
///system item
///mob131223 TVAL
bool make_object(object_type *j_ptr, u32b mode)
{
	int prob, base;
	byte obj_level;


	/* Chance of "special object" */
	/*:::�����m�����������炷�H*/
	prob = ((mode & AM_GOOD) ? 10 : 1000);
	//v1.1.20 7/300���班������
	if(EXTRA_MODE) prob = ((mode & AM_GOOD) ? 8 : 400);

	/* Base level for the object */
	base = ((mode & AM_GOOD) ? (object_level + 10) : object_level);


	/* Generate a special object, or a normal object */
	/*::: 1/prob���A�[�e�B�t�@�N�g�����̂ǂ��炩���ʂ�Ȃ���΂����ɗ���H�@�����ʂ�����ǂ��Ȃ�񂾂낤*/
	/*::: ���l���Ă݂��one_in_(prob)���ʂ�Ȃ��������_��make_artifact_special�ɍs���������ɓ���͂��B�܂聚�����̍ŏ��̏����Ƃ���1/1000(good��1/10)��ʂ�Ȃ���΂����Ȃ�*/
	/*::: �����ł�make_artifact_special�̓N�����]���␯�ȂǌŒ�A�[�e�B�t�@�N�g�p�x�[�X�A�C�e���̐����ɂ̂ݓK�p�����*/
	if (!one_in_(prob) || !make_artifact_special(j_ptr))
	{
		int k_idx;

		/* Good objects */
		/*::: GOOD�t���O�������ĂāA����get_obj_num_hook�ɉ��̊֐��̃A�h���X�������ĂȂ��ꍇ�H*/
		if ((mode & AM_GOOD) && !get_obj_num_hook)
		{
			/* Activate restriction (if already specified, use that) */
			/*:::Q003 kind_is_good�͊֐��ł�����������Ȃ��񂾂�����͊֐����Ă�ł���̂��H*/
			/*:::�u�֐��ւ̃|�C���^�v�Ƃ�����炵���Bget_obj_num_hook�̃A�h���X��kind_is_good()�Ɠ����ɂ����H*/
			/*:::variable.c�́ubool (*get_obj_num_hook)(int k_idx);�v�Ƃ����\���Q��*/
			get_obj_num_hook = kind_is_good;
		}

		/* Restricted objects - prepare allocation table */

		/*:::�����t���O���u�㎿�v�̂Ƃ��A�㎿�����i�A�ꕔ�̎w�ւ�A�~���A�O���ڈȏ㖂�����Ȃǈꕔ�̐�p�x�[�X�����o�Ȃ��Ȃ�*/
		if (get_obj_num_hook) get_obj_num_prep();

		/* Pick a random object */
		/*:::�x�[�X�A�C�e�������߂�*/
		k_idx = get_obj_num(base);

		/*:::get_obj_num_hook���g�����ꍇ�����ŃN���A��kind_list���Đݒ肷��*/
		/* Restricted objects */
		if (get_obj_num_hook)
		{
			/* Clear restriction */
			get_obj_num_hook = NULL;

			/* Reset allocation table to default */
			get_obj_num_prep();
		}

		/* Handle failure */
		if (!k_idx) return (FALSE);

		/* Prepare the object */
		/*:::j_ptr�ɃA�C�e���̊�{�����i�[*/
		object_prep(j_ptr, k_idx);
	}

	/* Apply magic (allow artifacts) */
	apply_magic(j_ptr, object_level, mode);

	//v2.0.16 ��Ɛj�̓����_���A�[�e�B�t�@�N�g�ł�������������邱�Ƃɂ���
	/*
	switch (j_ptr->tval)
	{
		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (!j_ptr->name1)
			{
				//v1.1.62 EXTRA���[�h�Ō�����
				if (EXTRA_MODE)
					j_ptr->number = 24 + (byte)damroll(6, 7);
				else
					j_ptr->number = (byte)damroll(6, 7);
			}
		}
	}
	///mod160205 �u�j�v�̌�����
	if (j_ptr->tval == TV_OTHERWEAPON && j_ptr->sval == SV_OTHERWEAPON_NEEDLE && !object_is_artifact(j_ptr))
	{
		//v1.1.62 EXTRA���[�h�Ō�����
		if (EXTRA_MODE)
			j_ptr->number = 16 + (byte)damroll(4, 6);
		else
			j_ptr->number = 8 + (byte)damroll(2, 6);
	}
	*/

	if (object_is_needle_arrow_bolt(j_ptr) && !object_is_fixed_artifact(j_ptr))
	{
		if (EXTRA_MODE)
			j_ptr->number = 24 + (byte)damroll(6, 7);
		else
			j_ptr->number = (byte)damroll(6, 7);
	}



	//Extra���[�h�ŏ��Օi�̈ꕔ���������������
	//v1.1.20 ���������炷
	if(EXTRA_MODE && !object_is_ego(j_ptr) && !object_is_artifact(j_ptr))
	{
		int value = object_value_real(j_ptr);

		switch (j_ptr->tval)
		{
		case TV_BOOK_ELEMENT:		case TV_BOOK_FORESEE:		case TV_BOOK_ENCHANT:
		case TV_BOOK_SUMMONS:		case TV_BOOK_MYSTIC:		case TV_BOOK_LIFE:
		case TV_BOOK_PUNISH:		case TV_BOOK_NATURE:		case TV_BOOK_TRANSFORM:
		case TV_BOOK_DARKNESS:		case TV_BOOK_NECROMANCY:		case TV_BOOK_CHAOS:
		case TV_BOOK_HISSATSU:	  	case TV_BOOK_MEDICINE:		case TV_BOOK_OCCULT:
			if(j_ptr->sval == 0 && one_in_(3)) j_ptr->number += randint1(2);
			if(j_ptr->sval == 1 && one_in_(4)) j_ptr->number += randint1(2);
			break;
		case TV_KNIFE:
			if( (j_ptr->sval == SV_WEAPON_KNIFE || j_ptr->sval == SV_WEAPON_DAGGER) && !one_in_(3))
				j_ptr->number = 4 + randint1(5+dun_level/6);
			break;
		case TV_AXE:
			if( (j_ptr->sval == SV_WEAPON_FARANCESCA) && one_in_(2))
				j_ptr->number = 2 + randint1(3+dun_level/8);
			break;
		case TV_SPEAR:
			if( (j_ptr->sval == SV_WEAPON_JAVELIN) && one_in_(2))
				j_ptr->number = 2 + randint1(3+dun_level/8);
			break;
		case TV_LITE:
			if(j_ptr->sval == SV_LITE_TORCH)
				j_ptr->number += randint0(3) + randint0(3);
			break;
		case TV_FLASK:
			j_ptr->number += randint0(3);
			if(weird_luck()) j_ptr->number += 3;
			break;
		case TV_WAND: 
			if(value < 600)
				j_ptr->number += randint1(25+dun_level/4) / 20;
			else if(value < 3000)
				j_ptr->number += randint1(15+dun_level/6) / 20;
			else if(weird_luck())
				j_ptr->number++;
			//���@�_��pval���S�{���p
			j_ptr->pval *= j_ptr->number;
			break;
		case TV_ROD:
			if(value < 2000)
				j_ptr->number += randint1(25+dun_level/4) / 20;
			else if(value < 8000)
				j_ptr->number += randint1(15+dun_level/6) / 20;
			else if(weird_luck())
				j_ptr->number++;
			break;
		case TV_SCROLL:
			if(!value)
			{
				j_ptr->number += randint1(30+dun_level/3) / 20;
				if(one_in_(13)) j_ptr->number += randint1(10);
			}
			else if(value < 50)
				j_ptr->number += randint1(100+dun_level/2) / 20;
			else if(value < 250)
				j_ptr->number += randint1(60+dun_level) / 25;
			else if(value < 15000)
				j_ptr->number += randint1(50+dun_level) / 40;
			else if(weird_luck())
				j_ptr->number++;
			break;
		case TV_MUSHROOM:
			if(value < 100)
				j_ptr->number += randint1(30+dun_level/2) / 10;
			if(value < 500)
				j_ptr->number += randint1(40+dun_level/2) / 20;
			else 
				j_ptr->number += randint1(50+dun_level/2) / 30;
			break;
		case TV_POTION:

			//���̖�͕�����������Ȃ��悤�ɂ��Ă����B��ʂɑ����Ŋ���đ��������炿����Ɨ��s�s�Ȃ���
			if(j_ptr->sval == SV_POTION_DEATH) break;

			if(!value)
			{
				j_ptr->number += randint1(30+dun_level/3) / 20;
				if(one_in_(13)) j_ptr->number += randint1(10);
			}
			else if(value < 50)
				j_ptr->number += 3 + randint1(30+dun_level/4) / 12;
			else if(value < 200)
				j_ptr->number += randint1(2) + randint1(30+dun_level/3) / 18;
			else if(value < 1000)
				j_ptr->number += randint1(50+dun_level/3) / 40;
			else if(value < 3000)
				j_ptr->number += randint1(60+dun_level/2) / 60;
			else if(value < 50000)
				j_ptr->number += randint1(80+dun_level/3) / 70;
			else if(weird_luck())
				j_ptr->number++;
			break;
		case TV_SWEETS:
		case TV_FOOD:
			if(value < 50)
				j_ptr->number += randint1(6);
			else
				j_ptr->number += randint1(100)/45;
			break;


		}
	}

	/*:::���̃_���W������荂���x���̃A�C�e�����o���ꍇ�`�[�g�I�v�V�����Ń��b�Z�[�W���o��*/
	obj_level = k_info[j_ptr->k_idx].level;
	if (object_is_fixed_artifact(j_ptr)) obj_level = a_info[j_ptr->name1].level;

	/* Notice "okay" out-of-depth objects */
	if (!object_is_cursed(j_ptr) && !object_is_broken(j_ptr) &&
	    (obj_level > dun_level))
	{
		/* Cheat -- peek at items */
		if (cheat_peek) object_mention(j_ptr);
	}

	/* Success */
	return (TRUE);
}


/*
 * Attempt to place an object (normal or good/great) at the given location.
 *
 * This routine plays nasty games to generate the "special artifacts".
 *
 * This routine uses "object_level" for the "generation level".
 *
 * This routine requires a clean floor grid destination.
 */
/*:::�w�肵�����W�Ƀ����_���ȃA�C�e����z�u����Bobject_level�ŊK�w�����肳���B�G�S�⁚���ɂ��Ȃ邪���ɂ͂Ȃ�Ȃ��H*/
/*:::���̊֐��ł̓A�C�e���͏d�Ȃ�Ȃ��B*/
void place_object(int y, int x, u32b mode)
{
	s16b o_idx;

	/* Acquire grid */
	cave_type *c_ptr = &cave[y][x];

	object_type forge;
	object_type *q_ptr;


	/* Paranoia -- check bounds */
	/*:::x,y���}�b�v�͈͓̔����ǂ����`�F�b�N*/
	if (!in_bounds(y, x)) return;

	/* Require floor space */
	/*:::x,y������n�`�ȂǃA�C�e����u���Ȃ��ꏊ����Ȃ����ǂ����`�F�b�N�H*/
	if (!cave_drop_bold(y, x)) return;

	/* Avoid stacking on other objects */
	/*:::���̊֐�����̓A�C�e�����d�˂Ĕz�u�ł��Ȃ��H*/
	if (c_ptr->o_idx) return;


	/* Get local object */
	q_ptr = &forge;

	/* Wipe the object */
	/*:::�錾���ăA�h���X�ڂ��������Ȃ̂ɂȂ�wipe���Ȃ��Ƃ����Ȃ��̂��s���B����������Ȃ��̂��H*/
	/*:::make_object��wipe���ꂽ�A�h���X��v�����Ă邹���炵���B����ł�wipe���Ȃ��Ƃǂ����e��������̂��킩���*/
	object_wipe(q_ptr);

	/* Make an object (if possible) */
	if (!make_object(q_ptr, mode)) return;


	/* Make an object */
	/*:::o_list[]�̒��Ŏg���Ă��Ȃ��z��ԍ���Ԃ� �A�C�e���������łɏ���Ȃ�0��Ԃ�*/
	o_idx = o_pop();

	/* Success */
	/*:::���������A�C�e����o_list�Ɋi�[���A���W��K�p*/
	if (o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[o_idx];

		/* Structure Copy */
		/*:::make_object����q_ptr��o_list[]��o_ptr�Ɉڂ�*/
		object_copy(o_ptr, q_ptr);

		/* Location */
		/*:::�ꏊ�w��@cave[x][y]�Ƃœ�x��ԂȋC�����邪*/
		o_ptr->iy = y;
		o_ptr->ix = x;

		/* Build a stack */
		/*:::��}�X�ɃA�C�e�����d�Ȃ��Ă�ꍇ�̂��߂̃C���f�b�N�X�H���������s�O�ł�����0�łȂ�������return���Ă����̂Ŏ���0������*/
		o_ptr->next_o_idx = c_ptr->o_idx;

		/* Place the object */
		/*:::�_���W�����\���̂̎w��̃}�X�ɃA�C�e����z�u�@*/
		c_ptr->o_idx = o_idx;

		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
	/*:::�A�C�e�������������Ĕz�u�ł��Ȃ������ꍇ �Œ�A�[�e�B�t�@�N�g�𖢏o���ɖ߂�*/
	else
	{
		/* Hack -- Preserve artifacts */
		if (object_is_fixed_artifact(q_ptr))
		{
			a_info[q_ptr->name1].cur_num = 0;
		}
	}
}


/*
 * Make a treasure object
 *
 * The location must be a legal, clean, floor grid.
 */
/*:::j_ptr�ɑ΂�����𐶐�����B*/
/*:::�O���[�o���ϐ�coin_type�����O�ɐݒ肵�Ă����Ɛ�����������̎�ނ�I�ׂ�*/
bool make_gold(object_type *j_ptr)
{
	int i;

	s32b base;


	/* Hack -- Pick a Treasure variety */
	i = ((randint1(object_level + 2) + 2) / 2) - 1;

	/* Apply "extra" magic */
	///mod140901 ���͎�ɓ����������
	if (one_in_(GREAT_OBJ) || p_ptr->pclass == CLASS_SHOU && one_in_(3) || (p_ptr->today_mon == FORTUNETELLER_GOODLUCK) && one_in_(3))
	{
		i += randint1(object_level + 1);
	}

	/* Hack -- Creeping Coins only generate "themselves" */
	if (coin_type) i = coin_type;

	/* Do not create "illegal" Treasure Types */
	if (i >= MAX_GOLD) i = MAX_GOLD - 1;

	/* Prepare a gold object */
	object_prep(j_ptr, OBJ_GOLD_LIST + i);

	/* Hack -- Base coin cost */
	base = k_info[OBJ_GOLD_LIST+i].cost;

	/* Determine how much the treasure is "worth" */
	j_ptr->pval = (base + (8L * randint1(base)) + randint1(8));

	/* Success */
	return (TRUE);
}


/*
 * Places a treasure (Gold or Gems) at given location
 *
 * The location must be a legal, clean, floor grid.
 */
void place_gold(int y, int x)
{
	s16b o_idx;

	/* Acquire grid */
	cave_type *c_ptr = &cave[y][x];


	object_type forge;
	object_type *q_ptr;


	/* Paranoia -- check bounds */
	if (!in_bounds(y, x)) return;

	/* Require floor space */
	if (!cave_drop_bold(y, x)) return;

	/* Avoid stacking on other objects */
	if (c_ptr->o_idx) return;


	/* Get local object */
	q_ptr = &forge;

	/* Wipe the object */
	object_wipe(q_ptr);

	/* Make some gold */
	if (!make_gold(q_ptr)) return;


	/* Make an object */
	o_idx = o_pop();

	/* Success */
	if (o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[o_idx];

		/* Copy the object */
		object_copy(o_ptr, q_ptr);

		/* Save location */
		o_ptr->iy = y;
		o_ptr->ix = x;

		/* Build a stack */
		o_ptr->next_o_idx = c_ptr->o_idx;

		/* Place the object */
		c_ptr->o_idx = o_idx;

		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
}


/*
 * Let an object fall to the ground at or near a location.
 *
 * The initial location is assumed to be "in_bounds()".
 *
 * This function takes a parameter "chance".  This is the percentage
 * chance that the item will "disappear" instead of drop.  If the object
 * has been thrown, then this is the chance of disappearance on contact.
 *
 * Hack -- this function uses "chance" to determine if it should produce
 * some form of "description" of the drop event (under the player).
 *
 * We check several locations to see if we can find a location at which
 * the object can combine, stack, or be placed.  Artifacts will try very
 * hard to be placed, including "teleporting" to a useful grid if needed.
 */
/*:::�w����W�̋߂��ɃA�C�e���𗎂Ƃ��B*/
/*:::chance��0�ȉ��ɂ��Ă��̊֐����g���΃A�C�e���͏��ł��Ȃ�*/
s16b drop_near(object_type *j_ptr, int chance, int y, int x)
{
	int i, k, d, s;

	int bs, bn;
	int by, bx;
	int dy, dx;
	int ty, tx = 0;

	s16b o_idx = 0;

	s16b this_o_idx, next_o_idx = 0;

	cave_type *c_ptr;

	char o_name[MAX_NLEN];

	bool flag = FALSE;
	bool done = FALSE;

#ifndef JP
	/* Extract plural */
	bool plural = (j_ptr->number != 1);
#endif

	/* Describe object */
	object_desc(o_name, j_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));


	/* Handle normal "breakage" */
	if (!object_is_artifact(j_ptr) && (randint0(100) < chance))
	{
		/* Message */
#ifdef JP
		msg_format("%s�͏������B", o_name);
#else
		msg_format("The %s disappear%s.",
			   o_name, (plural ? "" : "s"));
#endif


		/* Debug */
#ifdef JP
		if (p_ptr->wizard) msg_print("(�j��)");
#else
		if (p_ptr->wizard) msg_print("(breakage)");
#endif


		/* Failure */
		return (0);
	}


	/* Score */
	bs = -1;

	/* Picker */
	bn = 0;

	/* Default */
	by = y;
	bx = x;

	/* Scan local grids */
	for (dy = -3; dy <= 3; dy++)
	{
		/* Scan local grids */
		for (dx = -3; dx <= 3; dx++)
		{
			bool comb = FALSE;

			/* Calculate actual distance */
			d = (dy * dy) + (dx * dx);

			/* Ignore distant grids */
			if (d > 10) continue;

			/* Location */
			ty = y + dy;
			tx = x + dx;

			/* Skip illegal grids */
			if (!in_bounds(ty, tx)) continue;

			/* Require line of projection */
			if (!projectable(y, x, ty, tx)) continue;

			/* Obtain grid */
			c_ptr = &cave[ty][tx];

			/* Require floor space */
			if (!cave_drop_bold(ty, tx)) continue;

			/* No objects */
			k = 0;

			/* Scan objects in that grid */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Check for possible combination */
				if (object_similar(o_ptr, j_ptr)) comb = TRUE;

				/* Count objects */
				k++;
			}

			/* Add new object */
			if (!comb) k++;

			/* Paranoia */
			if (k > 99) continue;

			/* Calculate score */
			s = 1000 - (d + k * 5);

			/* Skip bad values */
			if (s < bs) continue;

			/* New best value */
			if (s > bs) bn = 0;

			/* Apply the randomizer to equivalent values */
			if ((++bn >= 2) && !one_in_(bn)) continue;

			/* Keep score */
			bs = s;

			/* Track it */
			by = ty;
			bx = tx;

			/* Okay */
			flag = TRUE;
		}
	}


	/* Handle lack of space */
	if (!flag && !object_is_artifact(j_ptr))
	{
		/* Message */
#ifdef JP
		msg_format("%s�͏������B", o_name);
#else
		msg_format("The %s disappear%s.",
			   o_name, (plural ? "" : "s"));
#endif


		/* Debug */
#ifdef JP
		if (p_ptr->wizard) msg_print("(���X�y�[�X���Ȃ�)");
#else
		if (p_ptr->wizard) msg_print("(no floor space)");
#endif


		/* Failure */
		return (0);
	}


	/* Find a grid */
	for (i = 0; !flag && (i < 1000); i++)
	{
		/* Bounce around */
		ty = rand_spread(by, 1);
		tx = rand_spread(bx, 1);

		/* Verify location */
		if (!in_bounds(ty, tx)) continue;

		/* Bounce to that location */
		by = ty;
		bx = tx;

		/* Require floor space */
		if (!cave_drop_bold(by, bx)) continue;

		/* Okay */
		flag = TRUE;
	}


	if (!flag)
	{
		int candidates = 0, pick;

		for (ty = 1; ty < cur_hgt - 1; ty++)
		{
			for (tx = 1; tx < cur_wid - 1; tx++)
			{
				/* A valid space found */
				if (cave_drop_bold(ty, tx)) candidates++;
			}
		}

		/* No valid place! */
		if (!candidates)
		{
			/* Message */
#ifdef JP
			msg_format("%s�͏������B", o_name);
#else
			msg_format("The %s disappear%s.", o_name, (plural ? "" : "s"));
#endif

			/* Debug */
#ifdef JP
			if (p_ptr->wizard) msg_print("(���X�y�[�X���Ȃ�)");
#else
			if (p_ptr->wizard) msg_print("(no floor space)");
#endif

			/* Mega-Hack -- preserve artifacts */
			if (preserve_mode)
			{
				/* Hack -- Preserve unknown artifacts */
				if (object_is_fixed_artifact(j_ptr) && !object_is_known(j_ptr))
				{
					/* Mega-Hack -- Preserve the artifact */
					a_info[j_ptr->name1].cur_num = 0;
				}
			}

			/* Failure */
			return 0;
		}

		/* Choose a random one */
		pick = randint1(candidates);

		for (ty = 1; ty < cur_hgt - 1; ty++)
		{
			for (tx = 1; tx < cur_wid - 1; tx++)
			{
				if (cave_drop_bold(ty, tx))
				{
					pick--;

					/* Is this a picked one? */
					if (!pick) break;
				}
			}

			if (!pick) break;
		}

		by = ty;
		bx = tx;
	}


	/* Grid */
	c_ptr = &cave[by][bx];

	/* Scan objects in that grid for combination */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Check for combination */
		if (object_similar(o_ptr, j_ptr))
		{
			/* Combine the items */
			object_absorb(o_ptr, j_ptr);

			/* Success */
			done = TRUE;

			/* Done */
			break;
		}
	}

	/* Get new object */
	if (!done) o_idx = o_pop();

	/* Failure */
	if (!done && !o_idx)
	{
		/* Message */
#ifdef JP
		msg_format("%s�͏������B", o_name);
#else
		msg_format("The %s disappear%s.",
			   o_name, (plural ? "" : "s"));
#endif


		/* Debug */
#ifdef JP
		if (p_ptr->wizard) msg_print("(�A�C�e�������߂���)");
#else
		if (p_ptr->wizard) msg_print("(too many objects)");
#endif


		/* Hack -- Preserve artifacts */
		if (object_is_fixed_artifact(j_ptr))
		{
			a_info[j_ptr->name1].cur_num = 0;
		}

		/* Failure */
		return (0);
	}

	/* Stack */
	if (!done)
	{
		/* Structure copy */
		object_copy(&o_list[o_idx], j_ptr);

		/* Access new object */
		j_ptr = &o_list[o_idx];

		/* Locate */
		j_ptr->iy = by;
		j_ptr->ix = bx;

		/* No monster */
		j_ptr->held_m_idx = 0;

		/* Build a stack */
		j_ptr->next_o_idx = c_ptr->o_idx;

		/* Place the object */
		c_ptr->o_idx = o_idx;

		/* Success */
		done = TRUE;
	}

	/* Note the spot */
	note_spot(by, bx);

	/* Draw the spot */
	lite_spot(by, bx);

	/* Sound */
	///mod141231 ���󐶐��Ȃǂŉ����o�Ȃ��悤�ɂ��Ă���
	if(!character_dungeon) sound(SOUND_DROP);

	///mod140428 ���̑����ɉ������������Ƃ������A�C�e�����X�g���X�V
	if(player_bold(by, bx)) fix_floor_item_list(py,px);

	/* Mega-Hack -- no message if "dropped" by player */
	/* Message when an object falls under the player */
	if (chance && player_bold(by, bx))
	{
#ifdef JP
		msg_print("�����������ɓ]�����Ă����B");
#else
		msg_print("You feel something roll beneath your feet.");
#endif

	}

	/* XXX XXX XXX */

	/* Result */
	return (o_idx);
}


/*
 * Scatter some "great" objects near the player
 */
/*:::�l���̊�����J�I�X�p�g������V�Ȃ�*/
void acquirement(int y1, int x1, int num, bool great, bool known)
{
	object_type *i_ptr;
	object_type object_type_body;
	u32b mode = AM_GOOD | (great ? AM_GREAT : 0L);

	/* Acquirement */
	while (num--)
	{
		/* Get local object */
		i_ptr = &object_type_body;

		/* Wipe the object */
		object_wipe(i_ptr);

		/* Make a good (or great) object (if possible) */
		if (!make_object(i_ptr, mode)) continue;

		if (known)
		{
			object_aware(i_ptr);
			object_known(i_ptr);
		}

		/* Drop the object */
		(void)drop_near(i_ptr, -1, y1, x1);
	}
}


/*
 * Scatter some "amusing" objects near the player
 */

#define AMS_NOTHING   0x00 /* No restriction */
#define AMS_NO_UNIQUE 0x01 /* Don't make the amusing object of uniques */
#define AMS_FIXED_ART 0x02 /* Make a fixed artifact based on the amusing object */
#define AMS_MULTIPLE  0x04 /* Drop 1-3 objects for one type */
#define AMS_PILE      0x08 /* Drop 1-99 pile objects for one type */

///del131216 �N���̊����𖳌���
#if 0
typedef struct
{
	int tval;
	int sval;
	int prob;
	byte flag;
} amuse_type;

amuse_type amuse_info[] =
{
	{ TV_BOTTLE, SV_ANY, 5, AMS_NOTHING },
	{ TV_JUNK, SV_ANY, 3, AMS_MULTIPLE },
	{ TV_SPIKE, SV_ANY, 10, AMS_PILE },
	{ TV_STATUE, SV_ANY, 15, AMS_NOTHING },
	{ TV_CORPSE, SV_ANY, 15, AMS_NO_UNIQUE },
	{ TV_SKELETON, SV_ANY, 10, AMS_NO_UNIQUE },
	{ TV_FIGURINE, SV_ANY, 10, AMS_NO_UNIQUE },
	{ TV_PARCHMENT, SV_ANY, 1, AMS_NOTHING },
	{ TV_POLEARM, SV_TSURIZAO, 3, AMS_NOTHING }, //Fishing Pole of Taikobo
	{ TV_SWORD, SV_BROKEN_DAGGER, 3, AMS_FIXED_ART }, //Broken Dagger of Magician
	{ TV_SWORD, SV_BROKEN_DAGGER, 10, AMS_NOTHING },
	{ TV_SWORD, SV_BROKEN_SWORD, 5, AMS_NOTHING },
	{ TV_SCROLL, SV_SCROLL_AMUSEMENT, 10, AMS_NOTHING },

	{ 0, 0, 0 }
};
#endif
///del131216 �N���̊����𖳌���
#if 0
void amusement(int y1, int x1, int num, bool known)
{
	object_type *i_ptr;
	object_type object_type_body;
	int n, t = 0;

	for (n = 0; amuse_info[n].tval != 0; n++)
	{
		t += amuse_info[n].prob;
	}

	/* Acquirement */
	while (num)
	{
		int i, k_idx, a_idx = 0;
		int r = randint0(t);
		bool insta_art, fixed_art;

		for (i = 0; ; i++)
		{
			r -= amuse_info[i].prob;
			if (r <= 0) break;
		}

		/* Get local object */
		i_ptr = &object_type_body;

		/* Wipe the object */
		object_wipe(i_ptr);

		/* Wipe the object */
		k_idx = lookup_kind(amuse_info[i].tval, amuse_info[i].sval);

		/* Paranoia - reroll if nothing */
		if (!k_idx) continue;

		/* Search an artifact index if need */
		insta_art = (k_info[k_idx].gen_flags & TRG_INSTA_ART);
		fixed_art = (amuse_info[i].flag & AMS_FIXED_ART);

		if (insta_art || fixed_art)
		{
			for (a_idx = 1; a_idx < max_a_idx; a_idx++)
			{
				if (insta_art && !(a_info[a_idx].gen_flags & TRG_INSTA_ART)) continue;
				if (a_info[a_idx].tval != k_info[k_idx].tval) continue;
				if (a_info[a_idx].sval != k_info[k_idx].sval) continue;
				if (a_info[a_idx].cur_num > 0) continue;
				break;
			}

			if (a_idx >= max_a_idx) continue;
		}

		/* Make an object (if possible) */
		object_prep(i_ptr, k_idx);
		if (a_idx) i_ptr->name1 = a_idx;
		apply_magic(i_ptr, 1, AM_NO_FIXED_ART);

		if (amuse_info[i].flag & AMS_NO_UNIQUE)
		{
			if (r_info[i_ptr->pval].flags1 & RF1_UNIQUE) continue;
		}

		if (amuse_info[i].flag & AMS_MULTIPLE) i_ptr->number = randint1(3);
		if (amuse_info[i].flag & AMS_PILE) i_ptr->number = randint1(99);

		if (known)
		{
			object_aware(i_ptr);
			object_known(i_ptr);
		}

		/* Paranoia - reroll if nothing */
		if (!(i_ptr->k_idx)) continue;

		/* Drop the object */
		(void)drop_near(i_ptr, -1, y1, x1);

		num--;
	}
}
#endif

#define MAX_NORMAL_TRAPS 18

/* See init_feat_variables() in init2.c */
static s16b normal_traps[MAX_NORMAL_TRAPS];

/*
 * Initialize arrays for normal traps
 */
/*:::�J���n���}�Q�h���͕ʏ����炵��*/
//�����̃C���f�b�N�X��TRAP_***�ƈ�v���Ă��邱��
//�r�[��㩂Ȃǒǉ���㩂�choose_random_trap()�ŕʂɏ������Ă���B

void init_normal_traps(void)
{
	int cur_trap = 0;

	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_TRAPDOOR");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_PIT");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_SPIKED_PIT");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_POISON_PIT");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_TY_CURSE");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_TELEPORT");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_FIRE");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_ACID");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_SLOW");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_LOSE_STR");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_LOSE_DEX");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_LOSE_CON");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_BLIND");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_CONFUSE");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_POISON");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_SLEEP");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_TRAPS");
	normal_traps[cur_trap++] = f_tag_to_index_in_init("TRAP_ALARM");
}

/*
 * Get random trap
 *
 * XXX XXX XXX This routine should be redone to reflect trap "level".
 * That is, it does not make sense to have spiked pits at 50 feet.
 * Actually, it is not this routine, but the "trap instantiation"
 * code, which should also check for "trap doors" on quest levels.
 */
/*:::�����N�G��ŉ��K�ɗ��Ƃ��˂�����Ă͂����Ȃ�*/
s16b choose_random_trap(void)
{
	s16b feat;


	/* Pick a trap */
	while (1)
	{

		//v1.1.24 TRAP��18-20�̓n���}�Q�Ȃǂ̓���g���b�v�ɂȂ��Ă���ʏ�㩂�ǉ����邱�Ƃ��z�肳��Ă��Ȃ�
		//normal_traps[]���Ӎ�蒼���͖̂ʓ|�Ȃ̂Ń��[�U�[�g���b�v�ǉ��ɂ����肱���ɖ��������荞��
		//�܊p�Ȃ̂ŋP��h�q�N�G�ƌ��s���ۓW�N�G�ł͑S������ɂȂ�悤�ɂ��Ă݂�
		//v2.0.11 �g���o�T�~�����������̂ł���ɖ������ǉ��B�����Ƃ܂Ƃ��ȏ����ɂ��ׂ��������܂��K�͂�㩂𑝂₷�\������̂Ƃ���Ȃ��̂ł��̂܂�
		//TODO:���ケ��ȏ�㩂𑝂₷�Ȃ珈���𐮗����ׂ�
		if(p_ptr->inside_quest == QUEST_KILL_GUYA && one_in_(2) || p_ptr->inside_quest == QUEST_MOON_VAULT && !one_in_(5)|| dun_level > 19 && one_in_(20))
			feat = f_tag_to_index_in_init("TRAP_BEAM");
		else if ( one_in_(19))
			feat = f_tag_to_index_in_init("TRAP_BEAR");
		else
			/* Hack -- pick a trap */
			feat = normal_traps[randint0(MAX_NORMAL_TRAPS)];
		//����g���b�v�𑝂₷�Ƃ���normal_traps[]�Ƀn���}�Q�h�����V�g���b�v���S������ă����_���ɑI�уn���}�Q�Ȃǂ̓���g���b�v�����̃��[�v�Œe���悤�ɂ���ق���������������Ȃ�

		//v1.1.97 ���܂菘�Ղɋ�����㩂͏o�Ȃ��悤�ɂ��Ă���
		if (feat == TRAP_TY_CURSE && dun_level < 30) continue;
		if (feat == TRAP_POISON && dun_level < 30) continue;
		if (feat == TRAP_FIRE && dun_level < 20) continue;
		if (feat == TRAP_ACID && dun_level < 20) continue;
		if (feat == TRAP_SLEEP && dun_level < 20) continue;

		//�n�`�ύX㩂͏����o�Â炭����
		if (feat == TRAP_SPIKED_PIT && one_in_(2)) continue;
		if (feat == TRAP_POISON_PIT && one_in_(2)) continue;

		/* Accept non-trapdoors */
		/*:::�g���b�v�h�A�𔻒肵�Ă�B�����ɂ�normal_trap�őI�΂��n�`�̂���FF_MORE�̃t���O�������Ă���n�`�𔻕�*/
		if (!have_flag(f_info[feat].flags, FF_MORE)) break;

		/* Hack -- no trap doors on special levels */
		if (p_ptr->inside_arena || quest_number(dun_level)) continue;

		/* Hack -- no trap doors on the deepest level */
		if (dun_level >= d_info[dungeon_type].maxdepth) continue;
	/*:::Easy�ł�50�K�܂ł����s���Ȃ�*/
	if(difficulty == DIFFICULTY_EASY && dungeon_type == DUNGEON_ANGBAND &&  dun_level == 50) continue;

		break;
	}

	//v2.0.11 �d�m�q�̃g���b�v�����͑S�ăg���o�T�~�ɂȂ�
	if (hack_flag_enoko_make_beartrap)
	{
		feat = f_tag_to_index_in_init("TRAP_BEAR");
	}

	//�g���o�T�~�̃e�X�g�p
	//if (p_ptr->wizard)feat = f_tag_to_index_in_init("TRAP_BEAR");

	return feat;
}

/*
 * Disclose an invisible trap
 */
/*:::�g���b�v�i�ƉB���h�A�H�j�𔭌�����@�g���b�v�ɂ��������Ƃ����Ă΂��*/
void disclose_grid(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	if (cave_have_flag_grid(c_ptr, FF_SECRET))
	{
		/* No longer hidden */
		cave_alter_feat(y, x, FF_SECRET);
	}
	else if (c_ptr->mimic)
	{
		/* No longer hidden */
		c_ptr->mimic = 0;

		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
}


/*
 * Places a random trap at the given location.
 *
 * The location must be a legal, naked, floor grid.
 *
 * Note that all traps start out as "invisible" and "untyped", and then
 * when they are "discovered" (by detecting them or setting them off),
 * the trap is "instantiated" as a visible, "typed", trap.
 */
/*:::�g���b�v����*/
void place_trap(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Paranoia -- verify location */
	if (!in_bounds(y, x)) return;

	/* Require empty, clean, floor grid */
	if (!cave_clean_bold(y, x)) return;

	/* Place an invisible trap */
	c_ptr->mimic = c_ptr->feat;
	c_ptr->feat = choose_random_trap();
}


/*
 * Describe the charges on an item in the inventory.
 */
void inven_item_charges(int item)
{
	object_type *o_ptr = &inventory[item];

	/* Require staff/wand */
	if ((o_ptr->tval != TV_STAFF) && (o_ptr->tval != TV_WAND)) return;

	/* Require known item */
	if (!object_is_known(o_ptr)) return;

#ifdef JP
	if (o_ptr->pval <= 0)
	{
		msg_print("�������͂��c���Ă��Ȃ��B");
	}
	else
	{
		msg_format("���� %d �񕪂̖��͂��c���Ă���B", o_ptr->pval);
	}
#else
	/* Multiple charges */
	if (o_ptr->pval != 1)
	{
		/* Print a message */
		msg_format("You have %d charges remaining.", o_ptr->pval);
	}

	/* Single charge */
	else
	{
		/* Print a message */
		msg_format("You have %d charge remaining.", o_ptr->pval);
	}
#endif

}


/*
 * Describe an item in the inventory.
 */
/*:::�A�C�e���̌������������Ƃ��ɌĂ΂��Ǝv����B�c������b�Z�[�W����*/
void inven_item_describe(int item)
{
	object_type *o_ptr = &inventory[item];
	char        o_name[MAX_NLEN];

	/* Get a description */
	object_desc(o_name, o_ptr, 0);

	/* Print a message */
#ifdef JP
	/* "no more" �̏ꍇ�͂�����ŕ\������ */
	if (o_ptr->number <= 0)
	{
		/*FIRST*//*�����͂����ʂ�Ȃ����� */
		msg_format("����%s�������Ă��Ȃ��B", o_name);
	}
	else
	{
		/* �A�C�e�������p���؂�ւ��@�\�Ή� */
		msg_format("�܂� %s�������Ă���B", o_name);
	}
#else
	msg_format("You have %s.", o_name);
#endif

}


/*
 * Increase the "number" of an item in the inventory
 */
/*:::�A�C�e���̌��𑝌�������B����̕t���O���ɂ��֌W*/
void inven_item_increase(int item, int num)
{
	object_type *o_ptr = &inventory[item];

	/* Apply */
	num += o_ptr->number;

	/* Bounds check */
	/*:::Q019 num�̒l��␳�E�E�H*/
	if (num > 255) num = 255;
	else if (num < 0) num = 0;

	/* Un-apply */
	num -= o_ptr->number;

	/* Change the number and weight */
	if (num)
	{
		/* Add the number */
		o_ptr->number += num;

		/* Add the weight */
		p_ptr->total_weight += (num * o_ptr->weight);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		p_ptr->update |= (PU_HP | PU_MANA);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Hack -- Clear temporary elemental brands if player takes off weapons */
		if (!o_ptr->number && p_ptr->ele_attack)
		{
			if ((item == INVEN_RARM) || (item == INVEN_LARM))
			{
				if (!buki_motteruka(INVEN_RARM + INVEN_LARM - item))
				{
					/* Clear all temporary elemental brands */
					set_ele_attack(0, 0);
				}
			}
		}
	}
}


/*
 * Erase an inventory slot if it has no more items
 */
/*:::�w�藓�̃A�C�e������0�Ȃ炻�̗���������*/
void inven_item_optimize(int item)
{
	object_type *o_ptr = &inventory[item];

	/* Only optimize real items */
	if (!o_ptr->k_idx) return;

	/* Only optimize empty items */
	if (o_ptr->number) return;

	/* The item is in the pack */
	if (item < INVEN_RARM)
	{
		int i;

		/* One less item */
		inven_cnt--;

		/* Slide everything down */
		for (i = item; i < INVEN_PACK; i++)
		{
			/* Structure copy */
			inventory[i] = inventory[i+1];
		}

		/* Erase the "final" slot */
		object_wipe(&inventory[i]);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* The item is being wielded */
	else
	{
		/* One less item */
		equip_cnt--;

		/* Erase the empty slot */
		object_wipe(&inventory[item]);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate torch */
		p_ptr->update |= (PU_TORCH);

		/* Recalculate mana XXX */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Window stuff */
	p_ptr->window |= (PW_SPELL);
}


/*
 * Describe the charges on an item on the floor.
 */
void floor_item_charges(int item)
{
	object_type *o_ptr = &o_list[item];

	/* Require staff/wand */
	if ((o_ptr->tval != TV_STAFF) && (o_ptr->tval != TV_WAND)) return;

	/* Require known item */
	if (!object_is_known(o_ptr)) return;

#ifdef JP
	if (o_ptr->pval <= 0)
	{
		msg_print("���̏���̃A�C�e���́A�������͂��c���Ă��Ȃ��B");
	}
	else
	{
		msg_format("���̏���̃A�C�e���́A���� %d �񕪂̖��͂��c���Ă���B", o_ptr->pval);
	}
#else
	/* Multiple charges */
	if (o_ptr->pval != 1)
	{
		/* Print a message */
		msg_format("There are %d charges remaining.", o_ptr->pval);
	}

	/* Single charge */
	else
	{
		/* Print a message */
		msg_format("There is %d charge remaining.", o_ptr->pval);
	}
#endif

}


/*
 * Describe an item in the inventory.
 */
void floor_item_describe(int item)
{
	object_type *o_ptr = &o_list[item];
	char        o_name[MAX_NLEN];

	/* Get a description */
	object_desc(o_name, o_ptr, 0);

	/* Print a message */
#ifdef JP
	/* "no more" �̏ꍇ�͂�����ŕ\���𕪂��� */
	if (o_ptr->number <= 0)
	{
		msg_format("����ɂ́A����%s�͂Ȃ��B", o_name);
	}
	else
	{
		msg_format("����ɂ́A�܂� %s������B", o_name);
	}
#else
	msg_format("You see %s.", o_name);
#endif

}


/*
 * Increase the "number" of an item on the floor
 */
void floor_item_increase(int item, int num)
{
	object_type *o_ptr = &o_list[item];

	/* Apply */
	num += o_ptr->number;

	/* Bounds check */
	if (num > 255) num = 255;
	else if (num < 0) num = 0;

	/* Un-apply */
	num -= o_ptr->number;

	/* Change the number */
	o_ptr->number += num;

	///mod140428 @�̑����̃A�C�e�������������Ƃ������A�C�e���ꗗ���X�V
	if(player_bold(o_ptr->iy,o_ptr->ix)) fix_floor_item_list(py,px);
}


/*
 * Optimize an item on the floor (destroy "empty" items)
 */
void floor_item_optimize(int item)
{
	object_type *o_ptr = &o_list[item];
	int ox,oy;
	ox = o_ptr->ix;
	oy = o_ptr->iy;

	/* Paranoia -- be sure it exists */
	if (!o_ptr->k_idx) return;

	/* Only optimize empty items */
	if (o_ptr->number) return;

	/* Delete the object */
	delete_object_idx(item);

	///mod140428 @�̑����̃A�C�e�������������Ƃ������A�C�e���ꗗ���X�V
	if(player_bold(oy,ox)) fix_floor_item_list(py,px);

}


/*
 * Check if we have space for an item in the pack without overflow
 */
/*:::�A�C�e�����U�b�N�ɓ�����邩����*/
bool inven_carry_okay(object_type *o_ptr)
{
	int j;

	/* Empty slot? */
	if (inven_cnt < INVEN_PACK) return (TRUE);

	/* Similar slot? */
	for (j = 0; j < INVEN_PACK; j++)
	{
		object_type *j_ptr = &inventory[j];

		/* Skip non-objects */
		if (!j_ptr->k_idx) continue;

		/* Check if the two items can be combined */
		if (object_similar(j_ptr, o_ptr)) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}

/*:::�A�C�e�������ԏ��Ԃ��r���� o_ptr�̂ق�����ɗ���ꍇTRUE��Ԃ�*/
///item�@TVAL �A�C�e����r 
bool object_sort_comp(object_type *o_ptr, s32b o_value, object_type *j_ptr)
{
	int o_type, j_type;

	/* Use empty slots */
	if (!j_ptr->k_idx) return TRUE;

	/* Hack -- readable books always come first */


	//v1.1.32 �p�`�����[��p���i�u���s�v�̗�O����
	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
	{
		if (o_ptr->tval <= MAX_MAGIC && o_ptr->tval == j_ptr->tval)
		{
			if (o_ptr->sval < j_ptr->sval) return TRUE;
			if (o_ptr->sval > j_ptr->sval) return FALSE;
		}
		else if (o_ptr->tval <= MAX_MAGIC)
		{
			if (j_ptr->tval > MAX_MAGIC) return TRUE;
			else if (o_ptr->tval < j_ptr->tval) return TRUE;
			else return FALSE;
		}
		else if (j_ptr->tval <= MAX_MAGIC)
		{
			if (o_ptr->tval > MAX_MAGIC) return FALSE;
			else if (o_ptr->tval < j_ptr->tval) return TRUE;
			else return FALSE;
		}
	}
	///mod140815 �X�y�}�X�n�̃A�C�e������
	else if(REALM_SPELLMASTER)
	{

		if(o_ptr->tval <= MAX_MAGIC && o_ptr->tval == j_ptr->tval)
		{
			if (o_ptr->sval < j_ptr->sval) return TRUE;
			if (o_ptr->sval > j_ptr->sval) return FALSE;
		}
		else if(o_ptr->tval <= MAX_MAGIC && rp_ptr->realm_aptitude[o_ptr->tval] && cp_ptr->realm_aptitude[o_ptr->tval] )
		{
			if(j_ptr->tval > MAX_MAGIC || rp_ptr->realm_aptitude[j_ptr->tval] == 0 || cp_ptr->realm_aptitude[j_ptr->tval] == 0 ) return TRUE;
			else if(o_ptr->tval < j_ptr->tval) return TRUE;
			else return FALSE;
		}
		else if(j_ptr->tval <= MAX_MAGIC && rp_ptr->realm_aptitude[j_ptr->tval] && cp_ptr->realm_aptitude[j_ptr->tval] )
		{
			if(o_ptr->tval > MAX_MAGIC || rp_ptr->realm_aptitude[o_ptr->tval] == 0 || cp_ptr->realm_aptitude[o_ptr->tval] == 0 ) return FALSE;
			else if(o_ptr->tval < j_ptr->tval) return TRUE;
			else return FALSE;
		}
	}
	else
	{
		if ((o_ptr->tval == p_ptr->realm1) && (j_ptr->tval != p_ptr->realm1)) return TRUE;
		if ((j_ptr->tval == p_ptr->realm1) && (o_ptr->tval != p_ptr->realm1)) return FALSE;

		if ((o_ptr->tval == p_ptr->realm2) && (j_ptr->tval != p_ptr->realm2)) return TRUE;
		if ((j_ptr->tval == p_ptr->realm2) && (o_ptr->tval != p_ptr->realm2)) return FALSE;
	}

	//v1.1.60 TV_ANTIQUE�͈�ԍŌ�ɂ���
	if (o_ptr->tval == TV_ANTIQUE && j_ptr->tval != TV_ANTIQUE) return FALSE;

	/* Objects sort by decreasing type */
	if (o_ptr->tval > j_ptr->tval) return TRUE;
	if (o_ptr->tval < j_ptr->tval) return FALSE;

	/* Non-aware (flavored) items always come last */
	/* Can happen in the home */
	if (!object_is_aware(o_ptr)) return FALSE;
	if (!object_is_aware(j_ptr)) return TRUE;

	/* Objects sort by increasing sval */
	if (o_ptr->sval < j_ptr->sval) return TRUE;
	if (o_ptr->sval > j_ptr->sval) return FALSE;

	/* Unidentified objects always come last */
	/* Objects in the home can be unknown */
	if (!object_is_known(o_ptr)) return FALSE;
	if (!object_is_known(j_ptr)) return TRUE;

	/* Fixed artifacts, random artifacts and ego items */
	if (object_is_fixed_artifact(o_ptr)) o_type = 3;
	else if (o_ptr->art_name) o_type = 2;
	else if (object_is_ego(o_ptr)) o_type = 1;
	else o_type = 0;

	if (object_is_fixed_artifact(j_ptr)) j_type = 3;
	else if (j_ptr->art_name) j_type = 2;
	else if (object_is_ego(j_ptr)) j_type = 1;
	else j_type = 0;

	if (o_type < j_type) return TRUE;
	if (o_type > j_type) return FALSE;

	switch (o_ptr->tval)
	{
	case TV_BUNBUN:
	case TV_KAKASHI:
	case TV_ITEMCARD:
	case TV_ABILITY_CARD:
		if(o_ptr->pval < j_ptr->pval) return TRUE;
		return FALSE;

	case TV_FIGURINE:
	///del131221 ����
	//case TV_STATUE:
	//case TV_CORPSE:
	case TV_CAPTURE:
		if (r_info[o_ptr->pval].level < r_info[j_ptr->pval].level) return TRUE;
		if ((r_info[o_ptr->pval].level == r_info[j_ptr->pval].level) && (o_ptr->pval < j_ptr->pval)) return TRUE;
		return FALSE;

	case TV_BULLET:
	case TV_ARROW:
	case TV_BOLT:
		/* Objects sort by increasing hit/damage bonuses */
		if (o_ptr->to_h + o_ptr->to_d < j_ptr->to_h + j_ptr->to_d) return TRUE;
		if (o_ptr->to_h + o_ptr->to_d > j_ptr->to_h + j_ptr->to_d) return FALSE;
		break;

	case TV_PHOTO:
		if (o_ptr->sval == SV_PHOTO_NIGHTMARE) //v1.1.51 �V�A���[�i�험�i�̎ʐ^�̓X�R�A���Ƀ\�[�g�����悤�ɂ���
		{
			if (o_ptr->xtra4 > j_ptr->xtra4) return TRUE;
			else return FALSE;

			if (o_ptr->xtra5 > j_ptr->xtra5) return TRUE;
			else return FALSE;

		}
		break;
	/* Hack:  otherwise identical rods sort by
	increasing recharge time --dsb */
	case TV_ROD:
		if (o_ptr->pval < j_ptr->pval) return TRUE;
		if (o_ptr->pval > j_ptr->pval) return FALSE;
		break;
	}

	/* Objects sort by decreasing value */
	return o_value > object_value(j_ptr);
}


/*
 * Add an item to the players inventory, and return the slot used.
 *
 * If the new item can combine with an existing item in the inventory,
 * it will do so, using "object_similar()" and "object_absorb()", else,
 * the item will be placed into the "proper" location in the inventory.
 *
 * This function can be used to "over-fill" the player's pack, but only
 * once, and such an action must trigger the "overflow" code immediately.
 * Note that when the pack is being "over-filled", the new item must be
 * placed into the "overflow" slot, and the "overflow" must take place
 * before the pack is reordered, but (optionally) after the pack is
 * combined.  This may be tricky.  See "dungeon.c" for info.
 *
 * Note that this code must remove any location/stack information
 * from the object once it is placed into the inventory.
 */
/*:::�A�C�e�����U�b�N�ɒǉ�
 *:::�܂Ƃ߂���A�C�e���͂܂Ƃ߁A�����łȂ��A�C�e���͐V�����Ƃ���ɔz�u
 *:::�U�b�N���̂Ƃ��͈ꎞ�̈�ɒu�����H
 *:::�A�C�e�����E������_���W�����̃A�C�e���ʒu�A�ςݏd�ˏ����X�V����K�v�����邱�Ƃɒ��ӁH
 */
s16b inven_carry(object_type *o_ptr)
{
	int i, j, k;
	int n = -1;

	object_type *j_ptr;


	/* Check for combining */
	/*:::�A�C�e�����U�b�N�̂ǂꂩ�ɂ܂Ƃ߂���ꍇ�܂Ƃ߂�*/
	for (j = 0; j < INVEN_PACK; j++)
	{
		j_ptr = &inventory[j];

		/* Skip non-objects */
		if (!j_ptr->k_idx) continue;

		/* Hack -- track last item */
		n = j;

		/* Check if the two items can be combined */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Combine the items */
			object_absorb(j_ptr, o_ptr);

			/* Increase the weight */
			p_ptr->total_weight += (o_ptr->number * o_ptr->weight);

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN);

			/* Success */
			return (j);
		}
	}


	/* Paranoia */
	/*:::�U�b�N�̃A�C�e�������\���̈��葽���Ȃ�����I���H�����ɂ͗��Ȃ��͂�*/
	if (inven_cnt > INVEN_PACK) return (-1);

	/* Find an empty slot */
	/*:::�J���Ă�X���b�g��T��*/
	for (j = 0; j <= INVEN_PACK; j++)
	{
		j_ptr = &inventory[j];

		/* Use it if found */
		if (!j_ptr->k_idx) break;
	}

	/* Use that slot */
	i = j;


	/* Reorder the pack */
	/*:::�A�C�e�����������̂��\���̈�łȂ��ꍇ�A�A�C�e���𐮗�H*/
	if (i < INVEN_PACK)
	{
		/* Get the "value" of the item */
		s32b o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_PACK; j++)
		{
			if (object_sort_comp(o_ptr, o_value, &inventory[j])) break;
		}

		/* Use that slot */
		i = j;

		/* Slide objects */
		/*:::n�̓C���x���g���̒��̍Ō�̃A�C�e��������ꏊ�@i�̏ꏊ���󂭂܂Ō��ɂ��炷*/
		for (k = n; k >= i; k--)
		{
			/* Hack -- Slide the item */
			object_copy(&inventory[k+1], &inventory[k]);
		}

		/* Wipe the empty slot */
		/*:::�J�����ꏊ�ɏE�����A�C�e��������O��WIPE*/
		object_wipe(&inventory[i]);
	}


	/* Copy the item */
	object_copy(&inventory[i], o_ptr);

	/*:::�E�����A�C�e���̃t���O�ލď���*/
	/* Access new object */
	j_ptr = &inventory[i];

	/* Forget stack */
	j_ptr->next_o_idx = 0;

	/* Forget monster */
	j_ptr->held_m_idx = 0;

	/* Forget location */
	j_ptr->iy = j_ptr->ix = 0;

	/* Player touches it, and no longer marked */
	j_ptr->marked = OM_TOUCHED;

	/* Increase the weight */
	p_ptr->total_weight += (j_ptr->number * j_ptr->weight);

	/* Count the items */
	inven_cnt++;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine and Reorder pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Return the slot */
	return (i);
}


/*
 * Take off (some of) a non-cursed equipment item
 *
 * Note that only one item at a time can be wielded per slot.
 *
 * Note that taking off an item when "full" may cause that item
 * to fall to the ground.
 *
 * Return the inventory slot into which the item is placed.
 */
/*:::�������O���B�ǂ���O�����I���ς݁@�􂢂ŊO��Ȃ��ȂǏ����ς�
 *:::item:INVEN_RARM�Ȃǂ̕��ʂɑΉ�����l 
 *:::amt:�u�����O�����v�炵���B�@�Ӗ�����̂��H
 */
s16b inven_takeoff(int item, int amt)
{
	int slot;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr act;

	char o_name[MAX_NLEN];


	/* Get the item to take off */
	o_ptr = &inventory[item];

	/* Paranoia */
	if (amt <= 0) return (-1);

	/* Verify */
	if (amt > o_ptr->number) amt = o_ptr->number;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = amt;

	/* Describe the object */
	object_desc(o_name, q_ptr, 0);

	///mod151202 �O�d������msg
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		act = "�𑕔�����͂�����";
	}

	/* Took off weapon */
	else if (((item == INVEN_RARM) || (item == INVEN_LARM)) &&
	    object_is_melee_weapon(o_ptr))
	{
#ifdef JP
		act = "�𑕔�����͂�����";
#else
		act = "You were wielding";
#endif

	}

	/* Took off bow */
	else if (item == INVEN_RIBBON)
	{
#ifdef JP
		act = "���قǂ���";
#else
		act = "You were holding";
#endif

	}

	/* Took off light */
	else if (item == INVEN_LITE)
	{
#ifdef JP
		act = "����������͂�����";
#else
		act = "You were holding";
#endif

	}

	/* Took off something */
	else
	{
#ifdef JP
		act = "�𑕔�����͂�����";
#else
		act = "You were wearing";
#endif

	}

	/* Modify, Optimize */
	inven_item_increase(item, -amt);
	inven_item_optimize(item);

	/* Carry the object */
	slot = inven_carry(q_ptr);

	/* Message */
#ifdef JP
	msg_format("%s(%c)%s�B", o_name, index_to_label(slot), act);
#else
	msg_format("%s %s (%c).", act, o_name, index_to_label(slot));
#endif


	/* Return slot */
	return (slot);
}


/*
 * Drop (some of) a non-cursed inventory/equipment item
 *
 * The object will be dropped "near" the current location
 */
/*:::�A�C�e���𗎂Ƃ��Bitem:INVEN_?? amt:��*/
void inven_drop(int item, int amt)
{
	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	char o_name[MAX_NLEN];


	/* Access original object */
	o_ptr = &inventory[item];

	/* Error check */
	if (amt <= 0) return;

	/* Not too many */
	if (amt > o_ptr->number) amt = o_ptr->number;


	/* Take off equipment */
	if (item >= INVEN_RARM)
	{
		/* Take off first */
		item = inven_takeoff(item, amt);

		/* Access original object */
		o_ptr = &inventory[item];
	}


	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Distribute charges of wands or rods */
	distribute_charges(o_ptr, q_ptr, amt);

	/* Modify quantity */
	q_ptr->number = amt;

	/* Describe local object */
	object_desc(o_name, q_ptr, 0);

	/* Message */
#ifdef JP
	msg_format("%s(%c)�𗎂Ƃ����B", o_name, index_to_label(item));
#else
	msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif


	/* Drop it near the player */
	(void)drop_near(q_ptr, 0, py, px);

	/* Modify, Describe, Optimize */
	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);
}


/*
 * Combine items in the pack
 *
 * Note special handling of the "overflow" slot
 */
/*:::�U�b�N�̒��𒲂ׁA�܂Ƃ߂�����̂�����΂܂Ƃ߂�*/
void combine_pack(void)
{
	int             i, j, k;
	object_type     *o_ptr;
	object_type     *j_ptr;
	bool            flag = FALSE, combined;

	do
	{
		combined = FALSE;

		/* Combine the pack (backwards) */
		for (i = INVEN_PACK; i > 0; i--)
		{
			/* Get the item */
			o_ptr = &inventory[i];

			/* Skip empty items */
			if (!o_ptr->k_idx) continue;

			/* Scan the items above that item */
			for (j = 0; j < i; j++)
			{
				int max_num;

				/* Get the item */
				j_ptr = &inventory[j];

				/* Skip empty items */
				if (!j_ptr->k_idx) continue;

				/*
				 * Get maximum number of the stack if these
				 * are similar, get zero otherwise.
				 */
				max_num = object_similar_part(j_ptr, o_ptr);

				/* Can we (partialy) drop "o_ptr" onto "j_ptr"? */
				if (max_num && j_ptr->number < max_num)
				{
					if (o_ptr->number + j_ptr->number <= max_num)
					{
						/* Take note */
						flag = TRUE;

						/* Add together the item counts */
						object_absorb(j_ptr, o_ptr);

						/* One object is gone */
						inven_cnt--;

						/* Slide everything down */
						for (k = i; k < INVEN_PACK; k++)
						{
							/* Structure copy */
							inventory[k] = inventory[k+1];
						}

						/* Erase the "final" slot */
						object_wipe(&inventory[k]);
					}
					else
					{
						int old_num = o_ptr->number;
						int remain = j_ptr->number + o_ptr->number - max_num;
#if 0
						o_ptr->number -= remain;
#endif
						/* Add together the item counts */
						object_absorb(j_ptr, o_ptr);

						o_ptr->number = remain;

						/* Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM- */
						if (o_ptr->tval == TV_ROD)
						{
							o_ptr->pval =  o_ptr->pval * remain / old_num;
							o_ptr->timeout = o_ptr->timeout * remain / old_num;
						}
						//v1.1.86
						if (o_ptr->tval == TV_ABILITY_CARD)
						{
							o_ptr->timeout = o_ptr->timeout * remain / old_num;

						}

						/* Hack -- if wands are stacking, combine the charges. -LM- */
						if (o_ptr->tval == TV_WAND)
						{
							o_ptr->pval = o_ptr->pval * remain / old_num;
						}
					}

					/* Window stuff */
					p_ptr->window |= (PW_INVEN);

					/* Take note */
					combined = TRUE;

					/* Done */
					break;
				}
			}
		}
	}
	while (combined);

	/* Message */
#ifdef JP
	if (flag) msg_print("�U�b�N�̒��̃A�C�e�����܂Ƃߒ������B");
#else
	if (flag) msg_print("You combine some items in your pack.");
#endif
}


/*
 * Reorder items in the pack
 *
 * Note special handling of the "overflow" slot
 */
/*:::�U�b�N�̒��̃A�C�e������ׂȂ���*/
void reorder_pack(void)
{
	int             i, j, k;
	s32b            o_value;
	object_type     forge;
	object_type     *q_ptr;
	object_type     *o_ptr;
	bool            flag = FALSE;


	/* Re-order the pack (forwards) */
	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Mega-Hack -- allow "proper" over-flow */
		if ((i == INVEN_PACK) && (inven_cnt == INVEN_PACK)) break;

		/* Get the item */
		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Get the "value" of the item */
		o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_PACK; j++)
		{
			if (object_sort_comp(o_ptr, o_value, &inventory[j])) break;
		}

		/* Never move down */
		if (j >= i) continue;

		/* Take note */
		flag = TRUE;

		/* Get local object */
		q_ptr = &forge;

		/* Save a copy of the moving item */
		object_copy(q_ptr, &inventory[i]);

		/* Slide the objects */
		for (k = i; k > j; k--)
		{
			/* Slide the item */
			object_copy(&inventory[k], &inventory[k-1]);
		}

		/* Insert the moving item */
		object_copy(&inventory[j], q_ptr);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* Message */
#ifdef JP
	if (flag) msg_print("�U�b�N�̒��̃A�C�e������ג������B");
#else
	if (flag) msg_print("You reorder some items in your pack.");
#endif

}


/*
 * Hack -- display an object kind in the current window
 *
 * Include list of usable spells for readible books
 */
/*:::�A�C�e���̏ڍׂ��T�u�E�B���h�E�ɕ\������B���̂Ƃ��떂�@�������炵��*/
///class realm ���@���̒��g�\���@�ʓ|�����疳���ɂ��悤���H
void display_koff(int k_idx)
{
	int y;

	object_type forge;
	object_type *q_ptr;
	int         sval;
	int         use_realm;

	char o_name[MAX_NLEN];


	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* No info */
	if (!k_idx) return;

	/* Get local object */
	q_ptr = &forge;

	/* Prepare the object */
	object_prep(q_ptr, k_idx);

	/* Describe */
	object_desc(o_name, q_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY | OD_STORE));

	/* Mention the object name */
	Term_putstr(0, 0, -1, TERM_WHITE, o_name);

	/* Access the item's sval */
	sval = q_ptr->sval;
	use_realm = q_ptr->tval;

	/* Warriors are illiterate */
	if (p_ptr->realm1 || p_ptr->realm2)
	{
		if ((use_realm != p_ptr->realm1) && (use_realm != p_ptr->realm2)) return;
	}
	else
	{
		if (REALM_SPELLMASTER) return;
		if (p_ptr->realm1 == TV_BOOK_HISSATSU) return;
		//if ((p_ptr->pclass == CLASS_RED_MAGE) && (use_realm != REALM_ARCANE) && (sval > 1)) return;
	}

	/* Display spells in readible books */
	{
		int     spell = -1;
		int     num = 0;
		byte    spells[64];

		/* Extract spells */
		for (spell = 0; spell < 32; spell++)
		{
			/* Check for this spell */
			if (fake_spell_flags[sval] & (1L << spell))
			{
				/* Collect this spell */
				spells[num++] = spell;
			}
		}

		/* Print spells */
		print_spells(0, spells, num, 2, 0, use_realm);
	}
}

/* Choose one of items that have warning flag */
object_type *choose_warning_item(void)
{
	int i;
	int choices[INVEN_TOTAL - INVEN_RARM];
	int number = 0;

	/* Paranoia -- Player has no warning ability */
	if (!p_ptr->warning) return NULL;

	/* Search Inventory */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		u32b flgs[TR_FLAG_SIZE];
		object_type *o_ptr = &inventory[i];
		if(check_invalidate_inventory(i))continue;

		object_flags(o_ptr, flgs);
		if (have_flag(flgs, TR_WARNING))
		{
			choices[number] = i;
			number++;
		}
	}

	/* Choice one of them */
	return number ? &inventory[choices[randint0(number)]] : NULL;
}

/* Calculate spell damages */
/*:::�G�̎����̃_���[�W���v�Z����@�x����������̂ݎg���Ă���炵���@�_���[�W�ɗ�����������͍̂ň��̒l���̗p*/
///res �����X�^�[�X�y���̃_���[�W�v�Z�i�x���p�j
///mod131229 �����_���[�W�v�Z�@�V����������ϐ��ύX�ő傫���ύX
static void spell_damcalc(monster_type *m_ptr, int typ, int dam, int limit, int *max)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int          rlev = r_ptr->level;
	bool         ignore_wraith_form = FALSE;

	if (limit) dam = (dam > limit) ? limit : dam;

	//������Ɖu
	if(CHECK_RINGO_IMMUNE(typ)) dam = 0;

	/* Vulnerability, resistance and immunity */
	switch (typ)
	{
	case GF_ELEC:
		if (p_ptr->immune_elec)
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else
		{
			///mod140302 ���f��_�A�ϐ���������
			dam = mod_elec_dam(dam);

			/*
			if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
			if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;
		//	if (prace_is_(RACE_ANDROID)) dam += dam / 3;
			if (prace_is_(RACE_KAPPA)) dam += dam / 3;
			if (p_ptr->resist_elec) dam = (dam + 2) / 3;
			if (IS_OPPOSE_ELEC())
				dam = (dam + 2) / 3;
			*/
		}
		break;

	case GF_POIS:
		if(p_ptr->pclass == CLASS_MEDICINE) dam=0;
		if(p_ptr->pclass == CLASS_EIRIN) dam=0;
		if (p_ptr->resist_pois) dam = (dam + 2) / 3;
		if (IS_OPPOSE_POIS()) dam = (dam + 2) / 3;
		break;

	case GF_ACID:
		if (p_ptr->immune_acid)
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else
		{
			///mod140302 ���f��_�A�ϐ���������
			dam = mod_acid_dam(dam);

			/*
			if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
			if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;
			if (p_ptr->resist_acid) dam = (dam + 2) / 3;
			if (IS_OPPOSE_ACID()) dam = (dam + 2) / 3;
			*/
		}
		break;

	case GF_COLD:
	case GF_ICE:
		if (p_ptr->immune_cold)
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else
		{
			///mod140302 ���f��_�A�ϐ���������
			dam = mod_cold_dam(dam);
/*
			if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
			if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;
			if (p_ptr->resist_cold) dam = (dam + 2) / 3;
			if (IS_OPPOSE_COLD()) dam = (dam + 2) / 3;
*/
		}
		break;

	case GF_FIRE:
		if (p_ptr->immune_fire)
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else
		{
			///mod140302 ���f��_�A�ϐ���������
			dam = mod_fire_dam(dam);
			/*
			if (p_ptr->muta3 & MUT3_VULN_ELEM) dam *= 2;
			if (prace_is_(RACE_ENT)) dam += dam / 3;
			if (p_ptr->special_defense & KATA_KOUKIJIN) dam += dam / 3;
			if (p_ptr->resist_fire) dam = (dam + 2) / 3;
			if (IS_OPPOSE_FIRE()) dam = (dam + 2) / 3;
			*/
		}
		break;

	case GF_PSY_SPEAR:
	case GF_GUNGNIR:
		ignore_wraith_form = TRUE;
		break;

	case GF_ARROW:
		if (!p_ptr->blind &&
		    ((!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].k_idx && (inventory[INVEN_RARM].name1 == ART_ZANTETSU)) ||
		     (!check_invalidate_inventory(INVEN_LARM) && inventory[INVEN_LARM].k_idx && (inventory[INVEN_LARM].name1 == ART_ZANTETSU))))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		break;

	case GF_LITE:

		if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME) dam = 0;
		else if (p_ptr->resist_lite)
			dam /= 2; /* Worst case of 4 / (d4 + 7) */
		dam = dam * (100 + is_hurt_lite()) / 100;

		//if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) dam *= 2;
		//else if (prace_is_(RACE_S_FAIRY)) dam = dam * 4 / 3;

		/*
		 * Cannot use "ignore_wraith_form" strictly (for "random one damage")
		 * "dam *= 2;" for later "dam /= 2"
		 */
		//if (p_ptr->wraith_form) dam *= 2;
		break;

	case GF_DARK:
		if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)  || (p_ptr->mimic_form == MIMIC_NUE)|| p_ptr->wraith_form || (p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 19))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else if (p_ptr->resist_dark) dam /= 2; /* Worst case of 4 / (d4 + 7) */
		break;

	case GF_SHARDS:
		if (p_ptr->resist_shard) dam = dam * 3 / 4; /* Worst case of 6 / (d4 + 7) */

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else if ((p_ptr->muta4 & MUT4_GHOST_HANIWA) || prace_is_(RACE_HANIWA)) dam += dam / 4;

		break;

	case GF_SOUND:
		if (p_ptr->resist_sound) dam = dam * 5 / 8; /* Worst case of 5 / (d4 + 7) */

		if ((p_ptr->muta4 & MUT4_GHOST_HANIWA) || prace_is_(RACE_HANIWA)) dam += dam / 4;

		break;

	case GF_CONFUSION:
		if (p_ptr->resist_conf) dam = dam * 5 / 8; /* Worst case of 5 / (d4 + 7) */
		break;

	case GF_CHAOS:
		if (p_ptr->resist_chaos) dam = dam * 3 / 4; /* Worst case of 6 / (d4 + 7) */
		break;

	case GF_NETHER:
		if (prace_is_(RACE_SPECTRE) || prace_is_(RACE_ANIMAL_GHOST))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else if (prace_is_(RACE_HANIWA))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		else if (p_ptr->resist_neth) dam = dam * 3 / 4; /* Worst case of 6 / (d4 + 7) */
		break;

	case GF_DISENCHANT:
		if (p_ptr->resist_disen) dam = dam * 3 / 4; /* Worst case of 6 / (d4 + 7) */
		dam = dam * (100 + is_hurt_disen()) / 100;
		break;

	case GF_POLLUTE:
		dam = dam/2 * (100 + is_hurt_disen()) / 100 + dam/2 / (p_ptr->resist_pois?3:1) / (IS_OPPOSE_POIS()?3:1);

		break;
	case GF_NEXUS:
		///mod131228 ���ʍ��������@���ʍ���������ϐ�
		//if (p_ptr->resist_nexus) dam = dam * 3 / 4; /* Worst case of 6 / (d4 + 7) */
		if (p_ptr->resist_time) dam = dam * 3 / 4; /* Worst case of 6 / (d4 + 7) */
		break;

	case GF_TIME:
	case GF_DISTORTION:
		if (p_ptr->resist_time) dam /= 2; /* Worst case of 4 / (d4 + 7) */
		break;

	case GF_GRAVITY:
		if (p_ptr->levitation) dam = (dam * 2) / 3;
		if (p_ptr->resist_time) dam = (dam * 2) / 3;
		break;

	case GF_ROCKET:
		if (p_ptr->resist_shard) dam /= 2;
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		break;

	case GF_NUKE:
		dam = ((p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)?0:mod_lite_dam(dam/2)) + mod_fire_dam(dam/2);

	//	if (p_ptr->resist_pois) dam = (2 * dam + 2) / 5;
	//	if (IS_OPPOSE_POIS()) dam = (2 * dam + 2) / 5;
		break;

	case GF_DEATH_RAY:
		if (p_ptr->mimic_form)
		{
			if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)
			{
				dam = 0;
				ignore_wraith_form = TRUE;
			}
		}
		else
		{

			if (RACE_RESIST_DEATH_RAY)
			{
				dam = 0;
				ignore_wraith_form = TRUE;
			}

		}
		break;


	case GF_HELL_FIRE:
			if (prace_is_(RACE_TENNIN) || prace_is_(RACE_LUNARIAN))
			{
				dam = dam * 4 / 3;
			}
			else if( (prace_is_(RACE_GYOKUTO) && p_ptr->pclass != CLASS_UDONGE))
			{
				dam = dam * 6 / 5;
			}

		break;

	case GF_MIND_BLAST:
	case GF_BRAIN_SMASH:
		if (100 + rlev / 2 <= MAX(5, p_ptr->skill_sav))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		break;
	case GF_COSMIC_HORROR:
		if(immune_insanity()) dam = 0;
		else if (p_ptr->resist_insanity) dam /= 2;
		break;

	case GF_CAUSE_1:
	case GF_CAUSE_2:
	case GF_CAUSE_3:
	case GF_HAND_DOOM:
		if (100 + rlev / 2 <= p_ptr->skill_sav)
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		break;


	case GF_CAUSE_4:
		if ((100 + rlev / 2 <= p_ptr->skill_sav) && (m_ptr->r_idx != MON_KENSHIROU))
		{
			dam = 0;
			ignore_wraith_form = TRUE;
		}
		break;

	case GF_PUNISH_1:
	case GF_PUNISH_2:
	case GF_PUNISH_3:
	case GF_PUNISH_4:
		if(is_hurt_holy() <= 0) dam=0;
		break;

	case GF_HOLY_FIRE:
			dam = mod_holy_dam(dam);
		break;

	case GF_METEOR:
		break;
	case GF_TORNADO:
		break;
	case GF_FORCE:
		break;

	case GF_PLASMA:
			dam = mod_fire_dam(dam/2) + mod_elec_dam(dam/2);
		break;

	case GF_WATER:
		if(p_ptr->pclass == CLASS_WAKASAGI && p_ptr->lev > 19) dam = 0;
		else if(p_ptr->pclass == CLASS_MURASA) dam = 0;
		else if (p_ptr->resist_water) dam /= 2;
		break;
	case GF_INACT:
	case GF_DISINTEGRATE:
	case GF_MISSILE:
	case GF_MANA:
		break;

	case GF_OLD_SLEEP://����̎����p
		if(!p_ptr->free_act) dam = 9999;
		else dam = 0;
		break;

	default:
		msg_format("WARNING:spell_damcalc�ɂ�(%d)��GF�l��������`����Ă��Ȃ�",typ);

	}

	if (p_ptr->wraith_form && !ignore_wraith_form)
	{
		dam /= 2;
		if (!dam) dam = 1;
	}

	if (dam > *max) *max = dam;
}

/* Calculate blow damages */
/*:::�x���̂��߂̑Ō��_���[�W���v�Z�炵��*/
///pend �x���̂��߂̑Ō��_���[�W�v�Z�@�ϐ��Ƃ��֌W����U���͒ǉ��v�Ȃ񂾂������ʓ|�Ȃ̂Ŗ������悤���H�j�ׂ������Ƃœ���Ă�����
static int blow_damcalc(monster_type *m_ptr, monster_blow *blow_ptr)
{
	int  dam = blow_ptr->d_dice * blow_ptr->d_side;
	int  dummy_max = 0;
	bool check_wraith_form = TRUE;

	if (blow_ptr->method != RBM_EXPLODE)
	{
		int ac = p_ptr->ac + p_ptr->to_a;

		switch (blow_ptr->effect)
		{
		case RBE_SUPERHURT:
		{
			int tmp_dam = dam - (dam * ((ac < 150) ? ac : 150) / 250);
			dam = MAX(dam, tmp_dam * 2);
			if(p_ptr->mimic_form == MIMIC_MIST) dam /= 3;
			break;
		}

		case RBE_HURT:
		case RBE_SHATTER:
		case RBE_BLEED:
			dam -= (dam * ((ac < 150) ? ac : 150) / 250);
			if(p_ptr->mimic_form == MIMIC_MIST) dam /= 3;
			break;

		case RBE_ACID:
			spell_damcalc(m_ptr, GF_ACID, dam, 0, &dummy_max);
			dam = dummy_max;
			check_wraith_form = FALSE;
			break;

		case RBE_ELEC:
			spell_damcalc(m_ptr, GF_ELEC, dam, 0, &dummy_max);
			dam = dummy_max;
			check_wraith_form = FALSE;
			break;

		case RBE_FIRE:
			spell_damcalc(m_ptr, GF_FIRE, dam, 0, &dummy_max);
			dam = dummy_max;
			check_wraith_form = FALSE;
			break;

		case RBE_COLD:
			spell_damcalc(m_ptr, GF_COLD, dam, 0, &dummy_max);
			dam = dummy_max;
			check_wraith_form = FALSE;
			break;

		case RBE_DR_MANA:
			dam = 0;
			check_wraith_form = FALSE;
			break;
		}

		if (check_wraith_form && p_ptr->wraith_form)
		{
			dam /= 2;
			if (!dam) dam = 1;
		}
	}
	else
	{
		dam = (dam + 1) / 2;
		spell_damcalc(m_ptr, mbe_info[blow_ptr->effect].explode_type, dam, 0, &dummy_max);
		dam = dummy_max;
	}

	return dam;
}

/* Examine the grid (xx,yy) and warn the player if there are any danger */
/*:::�����X�^�[�̎˒��͈͂�㩂Ɉړ����悤�Ƃ����Ƃ��x���������k���鏈���@�ڍז���*/
///mod140425 �V�X�y���ǉ�
///v1.1.11 �T�O����p�t���O�ǉ�
bool process_warning(int xx, int yy, bool sagume_check)
{
	int mx, my;
	cave_type *c_ptr;
	char o_name[MAX_NLEN];

#define WARNING_AWARE_RANGE 12
	int dam_max = 0;
	static int old_damage = 0;

	//���Ԓ�~���͌x���𖳎�����悤�ɂ���
	if(SAKUYA_WORLD || world_player) return TRUE;


	for (mx = xx - WARNING_AWARE_RANGE; mx < xx + WARNING_AWARE_RANGE + 1; mx++)
	{
		for (my = yy - WARNING_AWARE_RANGE; my < yy + WARNING_AWARE_RANGE + 1; my++)
		{
			int dam_max0 = 0;
			monster_type *m_ptr;
			monster_race *r_ptr;

			if (!in_bounds(my, mx) || (distance(my, mx, yy, xx) > WARNING_AWARE_RANGE)) continue;

			c_ptr = &cave[my][mx];

			if (!c_ptr->m_idx) continue;

			m_ptr = &m_list[c_ptr->m_idx];

			if (MON_CSLEEP(m_ptr)) continue;
			if (!is_hostile(m_ptr)) continue;

			r_ptr = &r_info[m_ptr->r_idx];

			/* Monster spells (only powerful ones)*/
			///mon res �X�y���t���O�g�p�ӏ�
			if (projectable(my, mx, yy, xx))
			{
				int breath_dam_div3 = m_ptr->hp / 3;
				int breath_dam_div6 = m_ptr->hp / 6;
				u32b f4 = r_ptr->flags4;
				u32b f5 = r_ptr->flags5;
				u32b f6 = r_ptr->flags6;
				u32b f9 = r_ptr->flags9;

				if (!(d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
				{
					int rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);
					int storm_dam = rlev * 4 + 150;
					bool powerful = (bool)(r_ptr->flags2 & RF2_POWERFUL);
					int storm_dam2 = powerful ? (rlev*10+200) : (rlev*7+100);

					if (f4 & RF4_BA_CHAO) spell_damcalc(m_ptr, GF_CHAOS, rlev * (powerful ? 3 : 2) + 100, 0, &dam_max0);
					if (f5 & RF5_BA_MANA) spell_damcalc(m_ptr, GF_MANA, storm_dam, 0, &dam_max0);
					if (f5 & RF5_BA_DARK) spell_damcalc(m_ptr, GF_DARK, storm_dam, 0, &dam_max0);
					if (f5 & RF5_BA_LITE) spell_damcalc(m_ptr, GF_LITE, storm_dam, 0, &dam_max0);
					if (f6 & RF6_HAND_DOOM) spell_damcalc(m_ptr, GF_HAND_DOOM, p_ptr->chp * 6 / 10, 0, &dam_max0);
					if (f6 & RF6_PSY_SPEAR) spell_damcalc(m_ptr, GF_PSY_SPEAR, powerful ? (rlev * 2 + 150) : (rlev * 3 / 2 + 100), 0, &dam_max0);

					///mod140425
					if (f6 & RF6_COSMIC_HORROR) spell_damcalc(m_ptr, GF_COSMIC_HORROR, storm_dam, 0, &dam_max0);
					if (f9 & RF9_FIRE_STORM) spell_damcalc(m_ptr, GF_FIRE, storm_dam2, 0, &dam_max0);
					if (f9 & RF9_ICE_STORM) spell_damcalc(m_ptr, GF_COLD, storm_dam2, 0, &dam_max0);
					if (f9 & RF9_THUNDER_STORM) spell_damcalc(m_ptr, GF_ELEC, storm_dam2, 0, &dam_max0);
					if (f9 & RF9_ACID_STORM) spell_damcalc(m_ptr, GF_ACID, storm_dam2, 0, &dam_max0);
					if (f9 & RF9_TOXIC_STORM) spell_damcalc(m_ptr, GF_POIS, powerful?(rlev*8 + 100):(rlev*5+100), 0, &dam_max0);
					if (f9 & RF9_BA_POLLUTE) spell_damcalc(m_ptr, GF_POLLUTE, powerful?(rlev*4 + 150):(rlev*3+100), 0, &dam_max0);
					if (f9 & RF9_BA_DISI) spell_damcalc(m_ptr, GF_DISINTEGRATE, powerful?(rlev*2 + 100):(rlev+50), 0, &dam_max0);
					if (f9 & RF9_BA_HOLY) spell_damcalc(m_ptr, GF_HOLY_FIRE, powerful?(rlev*2 + 150):(rlev+150), 0, &dam_max0);
					if (f9 & RF9_BA_METEOR) spell_damcalc(m_ptr, GF_METEOR, powerful?(rlev*3 + 300):(rlev*2+150), 0, &dam_max0);
					if (f9 & RF9_BA_DISTORTION) spell_damcalc(m_ptr, GF_DISTORTION, powerful?(rlev*3 + 150):(rlev*3/2+150), 0, &dam_max0);
					if (f9 & RF9_PUNISH_3) spell_damcalc(m_ptr, GF_PUNISH_3, 150, 0, &dam_max0);
					if (f9 & RF9_PUNISH_4) spell_damcalc(m_ptr, GF_PUNISH_4, 225, 0, &dam_max0);
					if (f5 & RF5_CAUSE_3) spell_damcalc(m_ptr, GF_CAUSE_3, 150, 0, &dam_max0);
					if (f5 & RF5_CAUSE_4) spell_damcalc(m_ptr, GF_CAUSE_4, 225, 0, &dam_max0);
					if (f5 & RF5_BO_MANA && !p_ptr->reflect) spell_damcalc(m_ptr, GF_MANA, rlev*7/2+50, 0, &dam_max0);
					if (f9 & RF9_BO_HOLY && !p_ptr->reflect) spell_damcalc(m_ptr, GF_HOLY_FIRE, powerful?(rlev*3/2+100):(rlev+100), 0, &dam_max0);
					if (f9 & RF9_GIGANTIC_LASER) spell_damcalc(m_ptr, GF_NUKE, powerful?(rlev*8+100):(rlev*4+100), 0, &dam_max0);
					if (f9 & RF9_HELLFIRE ) spell_damcalc(m_ptr, GF_HELL_FIRE, powerful?(rlev*4+200):(rlev*3+100), 0, &dam_max0);
					if (f9 & RF9_HOLYFIRE ) spell_damcalc(m_ptr, GF_HOLY_FIRE, powerful?(rlev*4+200):(rlev*3+100), 0, &dam_max0);
					if (f9 & RF9_TORNADO ) spell_damcalc(m_ptr, GF_TORNADO, powerful?(rlev*3+100):(rlev*2+50), 0, &dam_max0);



				}
				if (f4 & RF4_ROCKET) spell_damcalc(m_ptr, GF_ROCKET, m_ptr->hp / 4, 800, &dam_max0);
				if (f4 & RF4_BR_ACID) spell_damcalc(m_ptr, GF_ACID, breath_dam_div3, 1600, &dam_max0);
				if (f4 & RF4_BR_ELEC) spell_damcalc(m_ptr, GF_ELEC, breath_dam_div3, 1600, &dam_max0);
				if (f4 & RF4_BR_FIRE) spell_damcalc(m_ptr, GF_FIRE, breath_dam_div3, 1600, &dam_max0);
				if (f4 & RF4_BR_COLD) spell_damcalc(m_ptr, GF_COLD, breath_dam_div3, 1600, &dam_max0);
				if (f4 & RF4_BR_POIS) spell_damcalc(m_ptr, GF_POIS, breath_dam_div3, 800, &dam_max0);
				if (f4 & RF4_BR_NETH) spell_damcalc(m_ptr, GF_NETHER, breath_dam_div6, 550, &dam_max0);
				if (f4 & RF4_BR_LITE) spell_damcalc(m_ptr, GF_LITE, breath_dam_div6, 400, &dam_max0);
				if (f4 & RF4_BR_DARK) spell_damcalc(m_ptr, GF_DARK, breath_dam_div6, 400, &dam_max0);
//				if (f4 & RF4_BR_CONF) spell_damcalc(m_ptr, GF_CONFUSION, breath_dam_div6, 450, &dam_max0);
				if (f4 & RF4_BR_SOUN) spell_damcalc(m_ptr, GF_SOUND, breath_dam_div6, 450, &dam_max0);
				if (f4 & RF4_BR_CHAO) spell_damcalc(m_ptr, GF_CHAOS, breath_dam_div6, 600, &dam_max0);
				if (f4 & RF4_BR_DISE) spell_damcalc(m_ptr, GF_DISENCHANT, breath_dam_div6, 500, &dam_max0);
			//	if (f4 & RF4_BR_NEXU) spell_damcalc(m_ptr, GF_NEXUS, breath_dam_div3, 250, &dam_max0);
				if (f4 & RF4_BR_TIME) spell_damcalc(m_ptr, GF_TIME, breath_dam_div3, 150, &dam_max0);
				if (f4 & RF4_BR_INER) spell_damcalc(m_ptr, GF_INACT, breath_dam_div6, 200, &dam_max0);
				if (f4 & RF4_BR_GRAV) spell_damcalc(m_ptr, GF_GRAVITY, breath_dam_div3, 200, &dam_max0);
				if (f4 & RF4_BR_SHAR) spell_damcalc(m_ptr, GF_SHARDS, breath_dam_div6, 500, &dam_max0);
				//if (f4 & RF4_BR_PLAS) spell_damcalc(m_ptr, GF_PLASMA, breath_dam_div6, 150, &dam_max0);
//				if (f4 & RF4_BR_WALL) spell_damcalc(m_ptr, GF_FORCE, breath_dam_div6, 200, &dam_max0);
				if (f4 & RF4_BR_MANA) spell_damcalc(m_ptr, GF_MANA, breath_dam_div3, 250, &dam_max0);
				//if (f4 & RF4_BR_NUKE) spell_damcalc(m_ptr, GF_NUKE, breath_dam_div3, 800, &dam_max0);
				if (f4 & RF4_BR_DISI) spell_damcalc(m_ptr, GF_DISINTEGRATE, breath_dam_div6, 150, &dam_max0);

				///mod140425�ǉ�
				if (f4 & RF4_BR_HOLY) spell_damcalc(m_ptr, GF_HOLY_FIRE, breath_dam_div6, 500, &dam_max0);
				if (f4 & RF4_BR_HELL) spell_damcalc(m_ptr, GF_HELL_FIRE, breath_dam_div6, 500, &dam_max0);
				if (f4 & RF4_BR_AQUA) spell_damcalc(m_ptr, GF_WATER, m_ptr->hp / 8, 350, &dam_max0);
				if (f4 & RF4_WAVEMOTION) spell_damcalc(m_ptr, GF_DISINTEGRATE, breath_dam_div6, 555, &dam_max0);
				if (f4 & RF4_BR_NEXU) spell_damcalc(m_ptr, GF_NEXUS, m_ptr->hp / 7, 600, &dam_max0);
				if (f4 & RF4_BR_PLAS) spell_damcalc(m_ptr, GF_PLASMA, m_ptr->hp / 4, 1200, &dam_max0);
				if (f4 & RF4_BA_FORCE) spell_damcalc(m_ptr, GF_FORCE, m_ptr->hp / 4, 350, &dam_max0);//powerful����
				if (f4 & RF4_BR_NUKE) spell_damcalc(m_ptr, GF_NUKE, m_ptr->hp / 4, 1000, &dam_max0);

			}

			/* Monster melee attacks */
			if (!(r_ptr->flags1 & RF1_NEVER_BLOW) && !(d_info[dungeon_type].flags1 & DF1_NO_MELEE))
			{
				if (mx <= xx + 1 && mx >= xx - 1 && my <= yy + 1 && my >= yy - 1)
				{
					int m;
					int dam_melee = 0;
					for (m = 0; m < 4; m++)
					{
						/* Skip non-attacks */
						if (!r_ptr->blow[m].method || (r_ptr->blow[m].method == RBM_SHOOT)) continue;

						/* Extract the attack info */
						dam_melee += blow_damcalc(m_ptr, &r_ptr->blow[m]);
						if (r_ptr->blow[m].method == RBM_EXPLODE) break;
					}
					if (dam_melee > dam_max0) dam_max0 = dam_melee;
				}
			}

			/* Contribution from this monster */
			dam_max += dam_max0;
		}
	}

	if(cave[yy][xx].m_idx && r_info[m_list[cave[yy][xx].m_idx].r_idx].flags7 & RF7_DOPPELGANGER)
		dam_max = 9999;

	///v1.1.11 �T�O���u�G���̎�v��p�`�F�b�N
	if(sagume_check)
	{
		if(dam_max > p_ptr->chp / 2) return TRUE;
		else return FALSE;

	}

	/* Prevent excessive warning */
	if (dam_max > old_damage)
	{
		old_damage = dam_max * 3 / 2;

		if (dam_max > p_ptr->chp / 2)
		{
			object_type *o_ptr = choose_warning_item();

			if (o_ptr)
			{
#ifdef JP
					object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
					msg_format("%s���s���k�����I", o_name);
			}
			else
			{
				if(p_ptr->pclass == CLASS_WRIGGLE)
					msg_format("���͂��щ��峂��x���𔭂����I");
				else if(p_ptr->pclass == CLASS_ICHIRIN)
					msg_format("�_�R���x���̈ӎv�𔭂����I");
				else if (p_ptr->pclass == CLASS_KUTAKA)
					msg_format("���Ȃ��̓���̃q���R���s�������I");
				else
					msg_format("���Ȃ��̑�Z�����x���𔭂����I");
			}
				
#endif
			disturb(0, 1);
#ifdef JP
			return get_check("�{���ɂ��̂܂ܐi�ނ��H");
#else
			return get_check("Really want to go ahead? ");
#endif
		}
	}
	else old_damage = old_damage / 2;

	c_ptr = &cave[yy][xx];
	if (((!easy_disarm && is_trap(c_ptr->feat))
	    || (c_ptr->mimic && is_trap(c_ptr->feat))) && !one_in_(13))
	{
		object_type *o_ptr = choose_warning_item();
			if (o_ptr)
			{
#ifdef JP
					object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
					msg_format("%s���s���k�����I", o_name);
			}
			else
			{
				if(p_ptr->pclass == CLASS_WRIGGLE)
					msg_format("���͂��щ��峂��x���𔭂����I");
				else if(p_ptr->pclass == CLASS_ICHIRIN)
					msg_format("�_�R���x���̈ӎv�𔭂����I");
				else if (p_ptr->pclass == CLASS_KUTAKA)
					msg_format("���Ȃ��̓���̃q���R���s�������I");
				else
					msg_format("���Ȃ��̑�Z�����x���𔭂����I");
			}
#endif
		disturb(0, 1);
#ifdef JP
		return get_check("�{���ɂ��̂܂ܐi�ނ��H");
#else
		return get_check("Really want to go ahead? ");
#endif
	}

	return TRUE;
}

///mod131224
static bool item_tester_hook_melee_ammo(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_KATANA:
		case TV_HAMMER:
		case TV_STICK:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:		
		case TV_OTHERWEAPON:	
		case TV_BOLT:
		case TV_ARROW:
		case TV_BULLET:
		{
			return (TRUE);
		}
		case TV_KNIFE:
		{
			if (o_ptr->sval != SV_WEAPON_DOKUBARI) return (TRUE);
		}
	}

	return (FALSE);
}



/*:::�Ó���A����t���G�b�Z���X�𒊏o�ł���A�C�e��*/
//v1.1.74 �ݕP�ǉ�
static bool item_tester_hook_drain_essence(object_type *o_ptr)
{

	int tv = o_ptr->tval;
	int sv = o_ptr->sval;	

	if(p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_KEIKI)
	{
		if(object_is_weapon_armour_ammo(o_ptr)) return (TRUE);

		if(tv == TV_MATERIAL &&
		(sv == SV_MATERIAL_MAGNETITE || sv == SV_MATERIAL_HEMATITE || sv == SV_MATERIAL_ARSENIC ||
		 sv == SV_MATERIAL_MERCURY || sv == SV_MATERIAL_MITHRIL || sv ==  SV_MATERIAL_ADAMANTITE ||
		 sv == SV_MATERIAL_DRAGONNAIL || sv == SV_MATERIAL_DRAGONSCALE || sv ==  SV_MATERIAL_METEORICIRON ||
		 sv == SV_MATERIAL_HIHIIROKANE || sv == SV_MATERIAL_SCRAP_IRON || sv == SV_MATERIAL_ICESCALE  ||
		 sv == SV_MATERIAL_NIGHTMARE_FRAGMENT || sv == SV_MATERIAL_HOPE_FRAGMENT ||
		 sv == SV_MATERIAL_MYSTERIUM || sv == SV_MATERIAL_IZANAGIOBJECT || sv == SV_MATERIAL_RYUUZYU ||
		 sv == SV_MATERIAL_ISHIZAKURA || sv == SV_MATERIAL_BROKEN_NEEDLE || sv == SV_MATERIAL_SKULL
		 )) return (TRUE);

		if(tv == TV_SOUVENIR && sv ==  SV_SOUVENIR_PHOENIX_FEATHER ) return (TRUE);
		if(tv == TV_SOUVENIR && sv ==  SV_SOUVENIR_ILMENITE ) return (TRUE);
		if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) return (TRUE);

	}
	if(p_ptr->pclass == CLASS_JEWELER || p_ptr->pclass == CLASS_MISUMARU || p_ptr->pclass == CLASS_KEIKI)
	{
		if(tv == TV_MATERIAL) return (TRUE);
		if(tv == TV_AMULET) return (TRUE);
		if(tv == TV_RING) return (TRUE);
		if(tv == TV_RIBBON) return (TRUE);
		if(tv == TV_LITE && sv != SV_LITE_TORCH) return (TRUE);
		if(tv == TV_SOUVENIR && sv ==  SV_SOUVENIR_PHOENIX_FEATHER ) return (TRUE);
		if(tv == TV_SOUVENIR && sv ==  SV_SOUVENIR_ILMENITE ) return (TRUE);
		if(tv == TV_SOUVENIR && sv ==  SV_SOUVENIR_ASIA ) return (TRUE);
		if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) return (TRUE);//v1.1.89

	}

	return (FALSE);
}




/*
 *  Smithing type data for Weapon smith
 */
/*:::�b��t�p*/
/*:::�����TR�t���O�����o�G�b�Z���X���t�^���K�v�G�b�Z���X���̑Ή��\*/
//{add,add_name,type,essence,value}
//add:(add+1)�̒l��o_ptr->xtra3�Ɋi�[�����
//add_name(�G�b�Z���X�t�^�̂Ƃ����j���[�ɏo�閼�O),
//type(�ǂ̃y�[�W�ɏo�邩),
//essence(�ǂ̃G�b�Z���X���g�����@�����g������Ȃ��̂�-1),
//value(�G�b�Z���X�������g����)}
//v1.1.17 ���ϗp��extern����
essence_type essence_info[] = 
{
	{TR_STR, "�r��", 4, TR_STR, 20},
	{TR_INT, "�m�\", 4, TR_INT, 20},
	{TR_WIS, "����", 4, TR_WIS, 20},
	{TR_DEX, "��p��", 4, TR_DEX, 20},
	{TR_CON, "�ϋv��", 4, TR_CON, 20},
	{TR_CHR, "����", 4, TR_CHR, 20},
	{TR_MAGIC_MASTERY, "���͎x�z", 4, TR_MAGIC_MASTERY, 20},
	{TR_STEALTH, "�B��", 4, TR_STEALTH, 40},
	{TR_SEARCH, "�T��", 4, TR_SEARCH, 15},
	{TR_INFRA, "�ԊO������", 4, TR_INFRA, 15},
	{TR_TUNNEL, "�̌@", 4, TR_TUNNEL, 15},
	//v1.1.99 �����Ɩ��@�h���ǉ�
	{ TR_DISARM, "㩉���", 4, TR_DISARM, 15 },
	{ TR_SAVING, "���@�h��㏸", 4, TR_SAVING, 30 },
	{TR_SPEED, "�X�s�[�h", 4, TR_SPEED, 12},
	{TR_BLOWS, "�ǉ��U��", 4, TR_BLOWS, 20},

	{TR_CHAOTIC, "�J�I�X�U��", 1, TR_CHAOTIC, 25},
	{TR_VAMPIRIC, "�z���U��", 1, TR_VAMPIRIC, 50},
	{TR_FORCE_WEAPON, "����", 1, TR_FORCE_WEAPON, 75},
	{TR_VORPAL, "�؂ꖡ", 1, TR_VORPAL, 50},
	{TR_EX_VORPAL, "*�؂ꖡ*", 1, TR_VORPAL, 300},
	{TR_IMPACT, "�n�k����", 7, TR_IMPACT, 15},
	{TR_BRAND_POIS, "�ŎE", 1, TR_BRAND_POIS, 20},
	{TR_BRAND_ACID, "�n��", 1, TR_BRAND_ACID, 20},
	{TR_BRAND_ELEC, "�d��", 1, TR_BRAND_ELEC, 20},
	{TR_BRAND_FIRE, "�Ċ�", 1, TR_BRAND_FIRE, 20},
	{TR_BRAND_COLD, "����", 1, TR_BRAND_COLD, 20},
	{TR_SUST_STR, "�r�͈ێ�", 3, TR_SUST_STR, 15},
	{TR_SUST_INT, "�m�\�ێ�", 3, TR_SUST_STR, 15},
	{TR_SUST_WIS, "�����ێ�", 3, TR_SUST_STR, 15},
	{TR_SUST_DEX, "��p���ێ�", 3, TR_SUST_STR, 15},
	{TR_SUST_CON, "�ϋv�͈ێ�", 3, TR_SUST_STR, 15},
	{TR_SUST_CHR, "���͈ێ�", 3, TR_SUST_STR, 15},
	{TR_IM_ACID, "�_�Ɖu", 7, TR_IM_ACID, 20},
	{TR_IM_ELEC, "�d���Ɖu", 7, TR_IM_ACID, 20},
	{TR_IM_FIRE, "�Ή��Ɖu", 7, TR_IM_ACID, 20},
	{TR_IM_COLD, "��C�Ɖu", 7, TR_IM_ACID, 20},
	{TR_REFLECT, "����", 3, TR_REFLECT, 20},
	{TR_RES_ACID, "�ώ_", 2, TR_RES_ACID, 15},
	{TR_RES_ELEC, "�ϓd��", 2, TR_RES_ELEC, 15},
	{TR_RES_FIRE, "�ωΉ�", 2, TR_RES_FIRE, 15},
	{TR_RES_COLD, "�ϗ�C", 2, TR_RES_COLD, 15},
	{TR_RES_POIS, "�ϓ�", 2, TR_RES_POIS, 25},
	{TR_RES_LITE, "�ϑM��", 2, TR_RES_LITE, 20},
	{TR_RES_DARK, "�ψÍ�", 2, TR_RES_DARK, 20},
	{TR_RES_SOUND, "�ύ���", 2, TR_RES_SOUND, 20},
	{TR_RES_SHARDS, "�ϔj��", 2, TR_RES_SHARDS, 20},
	{TR_RES_NETHER, "�ϒn��", 2, TR_RES_NETHER, 20},
	{TR_RES_WATER, "�ϐ�", 2, TR_RES_WATER, 20},
	{TR_RES_HOLY, "�ϔj��", 2, TR_RES_HOLY, 30},
	{TR_RES_TIME, "�ώ���", 2, TR_RES_TIME, 40},
	{TR_RES_CHAOS, "�σJ�I�X", 2, TR_RES_CHAOS, 20},
	{TR_RES_DISEN, "�ϗ�", 2, TR_RES_DISEN, 20},
	{TR_RES_FEAR, "�ϋ��|", 2, TR_RES_FEAR, 20},
	{TR_FREE_ACT, "�ϖ��", 2, TR_FREE_ACT, 20},
	{TR_RES_BLIND, "�ϖӖ�", 2, TR_RES_BLIND, 20},
	{TR_RES_CONF, "�ύ���", 2, TR_RES_CONF, 20},
	{TR_RES_INSANITY, "�ϋ��C", 2, TR_RES_INSANITY, 30},
	{TR_SH_FIRE, "", 0, -2, 0},
	{TR_SH_ELEC, "", 0, -2, 0},
	{TR_SH_COLD, "", 0, -2, 0},

	{ TR_NO_TELE, "���e���|", 3, TR_NO_TELE, 15 },
	{TR_NO_MAGIC, "�����@", 3, TR_NO_MAGIC, 15},
	{TR_WARNING, "�x��", 3, TR_WARNING, 20},
	{TR_LEVITATION, "���V", 3, TR_LEVITATION, 20},
	{TR_LITE, "�i�v����", 3, TR_LITE, 15},
	{TR_SEE_INVIS, "������", 3, TR_SEE_INVIS, 20},
	{TR_TELEPATHY, "�e���p�V�[", 6, TR_TELEPATHY, 15},
	{TR_SLOW_DIGEST, "�x����", 3, TR_SLOW_DIGEST, 15},
	{TR_REGEN, "�}����", 3, TR_REGEN, 20},
	{TR_BLESSED, "�j��", 3, TR_BLESSED, 20},
	{TR_TELEPORT, "�e���|�[�g", 3, TR_TELEPORT, 25},

	{TR_EASY_SPELL, "���@��x�ቺ", 3, TR_EASY_SPELL, 30},
	{TR_DEC_MANA, "����͌���", 3, TR_DEC_MANA, 30},
	{TR_XTRA_MIGHT, "���͎ˌ�", 3, TR_XTRA_MIGHT, 30},
	{TR_XTRA_SHOTS, "�����ˌ�", 3, TR_XTRA_SHOTS, 30},

	{TR_GENZI, "�����̏���", 3, TR_GENZI, 30}, //�O���[�u�ȊO�ɓK�p�s��
	{TR_SPEEDSTER, "�����ړ�", 3, TR_SPEEDSTER, 30},


	{TR_SLAY_EVIL, "���׃X���C", 5, TR_SLAY_EVIL, 100},
	{TR_KILL_EVIL, "*���׃X���C*", 5, TR_SLAY_EVIL,500},
	{TR_SLAY_GOOD, "�����X���C", 5, TR_SLAY_GOOD, 100},
	{TR_KILL_GOOD, "*�����X���C*", 5, TR_SLAY_GOOD,500},
	{TR_SLAY_ANIMAL, "�����X���C", 5, TR_SLAY_ANIMAL, 25},
	{TR_KILL_ANIMAL, "*�����X���C*", 5, TR_SLAY_ANIMAL, 75},
	{TR_SLAY_HUMAN, "�l�ԃX���C", 5, TR_SLAY_HUMAN, 40},
	{TR_KILL_HUMAN, "*�l�ԃX���C*", 5, TR_SLAY_HUMAN, 100},
	{TR_SLAY_UNDEAD, "�s���X���C", 5, TR_SLAY_UNDEAD, 25},
	{TR_KILL_UNDEAD, "*�s���X���C*", 5, TR_SLAY_UNDEAD, 75},
	{TR_SLAY_DRAGON, "���X���C", 5, TR_SLAY_DRAGON, 40},
	{TR_KILL_DRAGON, "*���X���C*", 5, TR_SLAY_DRAGON, 100},
	{TR_SLAY_DEITY, "�_�i�X���C", 5, TR_SLAY_DEITY, 40},
	{TR_KILL_DEITY, "*�_�i�X���C*", 5, TR_SLAY_DEITY, 100},
	{TR_SLAY_DEMON, "�����X���C", 5, TR_SLAY_DEMON, 25},
	{TR_KILL_DEMON, "*�����X���C*", 5, TR_SLAY_DEMON, 75},
	{TR_SLAY_KWAI, "�d���X���C", 5, TR_SLAY_KWAI, 25},
	{TR_KILL_KWAI, "*�d���X���C*", 5, TR_SLAY_KWAI, 75},

	{TR_ESP_EVIL, "����ESP", 6, TR_SLAY_EVIL, 40},   
	{TR_ESP_GOOD, "����ESP", 6, TR_SLAY_GOOD, 40},   
	{TR_ESP_ANIMAL, "����ESP", 6, TR_SLAY_ANIMAL, 40},
	{TR_ESP_HUMAN, "�l��ESP", 6, TR_SLAY_HUMAN, 40},
	{TR_ESP_UNDEAD, "�s��ESP", 6, TR_SLAY_UNDEAD, 40}, 
	{TR_ESP_DRAGON, "��ESP", 6, TR_SLAY_DRAGON, 40},
	{TR_ESP_DEITY, "�_�iESP", 6, TR_SLAY_DEITY, 40},     
	{TR_ESP_DEMON, "����ESP", 6, TR_SLAY_DEMON, 40},       
	{TR_ESP_KWAI, "�d��ESP", 6, TR_SLAY_KWAI, 40},   

	{TR_BOOMERANG, "�u�[������", 1, TR_THROW, 20},   //TR_BOOMERANG(118)��magic_num1[108]�ɓ���Ȃ��̂�THROW�ɕ��荞��ł���.���̑����THROW�͎g���Ȃ�

	{ESSENCE_ATTACK, "�U��", 10, TR_ES_ATTACK, 30},
	{ESSENCE_AC, "�h��", 10, TR_ES_AC, 15},
	{ESSENCE_TMP_RES_ACID, "�_�ϐ�����", 7, TR_RES_ACID, 50},
	{ESSENCE_TMP_RES_ELEC, "�d���ϐ�����", 7, TR_RES_ELEC, 50},
	{ESSENCE_TMP_RES_FIRE, "�Ή��ϐ�����", 7, TR_RES_FIRE, 50},
	{ESSENCE_TMP_RES_COLD, "��C�ϐ�����", 7, TR_RES_COLD, 50},
	{ESSENCE_SH_FIRE, "�Ή��I�[��", 7, -1, 50},
	{ESSENCE_SH_ELEC, "�d���I�[��", 7, -1, 50},
	{ESSENCE_SH_COLD, "��C�I�[��", 7, -1, 50},
	{ESSENCE_RESISTANCE, "�S�ϐ�", 2, -1, 100},//v1.1.74 150��100
	{ESSENCE_SUSTAIN, "�����ێ�", 10, -1, 10},
	{ESSENCE_SLAY_GLOVE, "����U���C��", 1, TR_ES_ATTACK, 200},
	{ESSENCE_TMP_RES_ALL, "�S�ϐ�����", 7, -1, 150 },

	{ ESSENCE_WHIRL_WIND, "�J�}�C�^�`����", 7, -1, 25 },
	{ ESSENCE_TIM_INC_DICE, "�ꎞ�_�C�X��������", 7, -1, 50 },
	{ ESSENCE_PSY_SPEAR, "���̌�����", 7, -1, 20 },
	{ ESSENCE_L_RANGE_ATTACK, "�������U������", 7, -1, 25 },
	{ ESSENCE_HIT_AND_AWAY, "�ꌂ���E����", 7, -1, 25 },
	{ ESSENCE_HEAL_500, "�񕜔���", 7, -1, 30 },
	{ ESSENCE_INVULN, "���G������", 7, -1, 50 },
	{ ESSENCE_SS_ARROW, "�Z�C���g�X�^�[�A���[����", 7, -1, 30 },
	{ ESSENCE_SUPER_STEALTH, "���B������", 7, TR_STEALTH, 300 },
	{ ESSENCE_DIM_DOOR, "�����̔�����", 7, -1, 20 },
	{ ESSENCE_DUAL_CAST, "�A��������", 7, -1, 100 },




	{-1, NULL, 0, -1, 0}
};

/*:::�G�b�Z���X�ꗗ*/
/*:::�z��̓Y������TR_????�̒l�Ɉ�v����*/
static cptr essence_name[TR_FLAG_MAX+1] = 
{
	"�r��",
	"�m�\",
	"����",
	"��p��",
	"�ϋv��",
	"����",
	"���͎x�z",
	"�B��",
	"�T��",
	"�ԊO������",
	"�̌@",
	"�X�s�[�h",
	"�ǉ��U��",
	"�J�I�X�U��",
	"�z���U��",
	"����",
	"�؂ꖡ",
	"",
	"�n�k",
	"�ŎE",
	"�n��",
	"�d��",
	"�Ċ�",
	"����",
	"���׃X���C",
	"�����X���C",
	"�����X���C",
	"�l�ԃX���C",
	"�s���X���C",
	"���X���C",
	"�_�i�X���C",
	"�����X���C",
	"�d���X���C",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"�\�͈ێ�",
	"",
	"",
	"",
	"",
	"",
	"���@��x�ቺ",
	"����͌���",
	"���͎ˌ�",
	"�����ˌ�",
	"�u�[������", //TR_THROW�̏ꏊ���g��
	"",
	"����",
	"�Ɖu",
	"",
	"",
	"",
	"�ώ_",
	"�ϓd��",
	"�ωΉ�",
	"�ϗ�C",
	"�ϓ�",
	"�ϑM��",
	"�ψÍ�",
	"�ϔj��",
	"�ύ���",
	"�ϒn��",
	"�ϐ�",
	"�σJ�I�X",
	"�ϗ�",
	"�ϔj��",
	"�ώ���",
	"�ϖ��",
	"�ϋ��|",
	"�ϖӖ�",
	"�ύ���",
	"�ϋ��C",
	"���V",
	"������",
	"�x����",
	"�}����",
	"�i�v����",
	"�x��",
	"�j��",
	"�e���|�[�g",
	"�񓁓K��",
	"�����ړ�",
	"�e���p�V�[",//100
	"",
	"",
	"",
	"���e���|�[�g",
	"�����@",
	"�U��",
	"�h��",
	"",
	"",
	"",//110
	"",
	"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",//120
		"",
		"",
		"",
		"",
		"",
		"",
		"㩉���",
		"���@�h��㏸",
	NULL
};


/*:::�b��t�̃G�b�Z���X�ꗗ��\������*/
static void display_essence(void)
{
	int i, num = 0;

	screen_save();
	for (i = 1; i < 22; i++)
	{
		prt("",i,0);
	}
#ifdef JP
	prt("�G�b�Z���X   ��     �G�b�Z���X   ��     �G�b�Z���X   ��", 1, 8);
#else
	prt("Essence      Num      Essence      Num      Essence      Num ", 1, 8);
#endif
	for (i = 0; essence_name[i]; i++)
	{
		if (!essence_name[i][0]) continue;
		prt(format("%-12s %5d", essence_name[i], p_ptr->magic_num1[i]), 2+num%20, 4+num/20*22);
		num++;
	}
#ifdef JP
	prt("���ݏ������Ă���G�b�Z���X", 0, 0);
#else
	prt("List of all essences you have.", 0, 0);
#endif
	(void)inkey();
	screen_load();
	return;
}


#if 0
//�b��t�����[�`��
/*:::�I�������A�C�e������G�b�Z���X�𒊏o����*/
static void drain_essence(void)
{
	int drain_value[sizeof(p_ptr->magic_num1) / sizeof(s32b)];
	int i, item;
	int dec = 4;
	bool observe = FALSE;
	int old_ds, old_dd, old_to_h, old_to_d, old_ac, old_to_a, old_pval, old_name2, old_timeout;
	u32b old_flgs[TR_FLAG_SIZE], new_flgs[TR_FLAG_SIZE];
	object_type *o_ptr;
	cptr            q, s;
	byte iy, ix, marked, number;
	s16b next_o_idx, weight;

	for (i = 0; i < sizeof(drain_value) / sizeof(int); i++)
		drain_value[i] = 0;

	item_tester_hook = object_is_weapon_armour_ammo;
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�����璊�o���܂����H";
	s = "���o�ł���A�C�e��������܂���B";
#else
	q = "Extract from which item? ";
	s = "You have nothing you can extract from.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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

	if (object_is_known(o_ptr) && !object_is_nameless(o_ptr))
	{
		char o_name[MAX_NLEN];
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
		if (!get_check(format("�{����%s���璊�o���Ă�낵���ł����H", o_name))) return;
#else
		if (!get_check(format("Really extract from %s? ", o_name))) return;
#endif
	}

	energy_use = 100;

	object_flags(o_ptr, old_flgs);
	/*:::�G�b�Z���X���o�p�Ƀt���O�������Ă�炵��*/
	if (have_flag(old_flgs, TR_KILL_DRAGON)) add_flag(old_flgs, TR_SLAY_DRAGON);
	if (have_flag(old_flgs, TR_KILL_ANIMAL)) add_flag(old_flgs, TR_SLAY_ANIMAL);
	if (have_flag(old_flgs, TR_KILL_EVIL)) add_flag(old_flgs, TR_SLAY_EVIL);
	if (have_flag(old_flgs, TR_KILL_UNDEAD)) add_flag(old_flgs, TR_SLAY_UNDEAD);
	if (have_flag(old_flgs, TR_KILL_DEMON)) add_flag(old_flgs, TR_SLAY_DEMON);
	//if (have_flag(old_flgs, TR_KILL_ORC)) add_flag(old_flgs, TR_SLAY_ORC);
	//if (have_flag(old_flgs, TR_KILL_TROLL)) add_flag(old_flgs, TR_SLAY_TROLL);
	//if (have_flag(old_flgs, TR_KILL_GIANT)) add_flag(old_flgs, TR_SLAY_GIANT);
	if (have_flag(old_flgs, TR_KILL_HUMAN)) add_flag(old_flgs, TR_SLAY_HUMAN);

	old_to_a = o_ptr->to_a;
	old_ac = o_ptr->ac;
	old_to_h = o_ptr->to_h;
	old_to_d = o_ptr->to_d;
	old_ds = o_ptr->ds;
	old_dd = o_ptr->dd;
	old_pval = o_ptr->pval;
	old_name2 = o_ptr->name2;
	old_timeout = o_ptr->timeout;
	/*:::�􂢂Ȃǃ}�C�i�X���ʂ�dec�����Z���A1���邲�ƂɌ�̏����ŃG�b�Z���X��1/4����*/
	if (o_ptr->curse_flags & (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE)) dec--;
	if (have_flag(old_flgs, TR_ADD_L_CURSE)) dec--;
	if (have_flag(old_flgs, TR_ADD_H_CURSE)) dec--;
	if (have_flag(old_flgs, TR_AGGRAVATE)) dec--;
	if (have_flag(old_flgs, TR_NO_TELE)) dec--;
	if (have_flag(old_flgs, TR_DRAIN_EXP)) dec--;
	if (have_flag(old_flgs, TR_TY_CURSE)) dec--;

	iy = o_ptr->iy;
	ix = o_ptr->ix;
	next_o_idx = o_ptr->next_o_idx;
	marked = o_ptr->marked;
	weight = o_ptr->weight;
	number = o_ptr->number;

	/*:::���̃A�C�e���̃G�S�Ȃǂ��������Ċ�{���݂̂ɂ��Ă���H*/
	object_prep(o_ptr, o_ptr->k_idx);

	o_ptr->iy=iy;
	o_ptr->ix=ix;
	o_ptr->next_o_idx=next_o_idx;
	o_ptr->marked=marked;
	o_ptr->number = number;
	if (o_ptr->tval == TV_DRAG_ARMOR) o_ptr->timeout = old_timeout;
	if (item >= 0) p_ptr->total_weight += (o_ptr->weight*o_ptr->number - weight*number);
	o_ptr->ident |= (IDENT_MENTAL);
	object_aware(o_ptr);
	object_known(o_ptr);

	object_flags(o_ptr, new_flgs);

	/*:::�t���O���G�b�Z���X�ɕϊ����Ă���*/
	///class ����t�����̂��߂ɂ͂����Ŗ��@��Փx�����A���͂Ȃǂ̃G�b�Z���X������������
	for (i = 0; essence_info[i].add_name; i++)
	{
		essence_type *es_ptr = &essence_info[i];
		int pval = 0;

		/*:::�G���t�̃N���[�N�Ȃǌ��Xpval�l�����A�C�e���̏ꍇ������G�b�Z���X����A�C�e����{�l�������Ă���炵��*/
		if (es_ptr->add < TR_FLAG_MAX && is_pval_flag(es_ptr->add) && old_pval)
			pval = (have_flag(new_flgs, es_ptr->add)) ? old_pval - o_ptr->pval : old_pval;

		/*:::essence_info[i]�ɊY������G�b�Z���X�������Ă����ꍇ�i�����ꂪ�x�[�X�A�C�e���̎����Ă�����̂łȂ��ꍇ�j*/
		if (es_ptr->add < TR_FLAG_MAX &&
		    (!have_flag(new_flgs, es_ptr->add) || pval) &&
		    have_flag(old_flgs, es_ptr->add))
		{
			if (pval)
			{
				drain_value[es_ptr->essence] += 10 * pval;
			}
			/*:::-2�ɂȂ�͉̂Η�d�̃I�[���@��̏����ɃX�L�b�v*/
			else if (es_ptr->essence != -2)
			{
				drain_value[es_ptr->essence] += 10;
			}
			else if (es_ptr->add == TR_SH_FIRE)
			{
				drain_value[TR_BRAND_FIRE] += 10;
				drain_value[TR_RES_FIRE] += 10;
			}
			else if (es_ptr->add == TR_SH_ELEC)
			{
				drain_value[TR_BRAND_ELEC] += 10;
				drain_value[TR_RES_ELEC] += 10;
			}
			else if (es_ptr->add == TR_SH_COLD)
			{
				drain_value[TR_BRAND_COLD] += 10;
				drain_value[TR_RES_COLD] += 10;
			}
		}
	}
	/*:::essence_info[]�ɗ��͂̍��ڂ͂Ȃ�*/
	///��������̂��߂ɂ����M��K�v������
	if ((have_flag(old_flgs, TR_FORCE_WEAPON)) && !(have_flag(new_flgs, TR_FORCE_WEAPON)))
	{
		drain_value[TR_INT] += 5;
		drain_value[TR_WIS] += 5;
	}
	/*:::�؂ꖡ*/
	if ((have_flag(old_flgs, TR_VORPAL)) && !(have_flag(new_flgs, TR_VORPAL)))
	{
		drain_value[TR_BRAND_POIS] += 5;
		drain_value[TR_BRAND_ACID] += 5;
		drain_value[TR_BRAND_ELEC] += 5;
		drain_value[TR_BRAND_FIRE] += 5;
		drain_value[TR_BRAND_COLD] += 5;
	}
	if ((have_flag(old_flgs, TR_DEC_MANA)) && !(have_flag(new_flgs, TR_DEC_MANA)))
	{
		drain_value[TR_INT] += 10;
	}
	if ((have_flag(old_flgs, TR_XTRA_MIGHT)) && !(have_flag(new_flgs, TR_XTRA_MIGHT)))
	{
		drain_value[TR_STR] += 10;
	}
	if ((have_flag(old_flgs, TR_XTRA_SHOTS)) && !(have_flag(new_flgs, TR_XTRA_SHOTS)))
	{
		drain_value[TR_DEX] += 10;
	}
	if (old_name2 == EGO_HANDS_MUSOU)
	{
		drain_value[TR_DEX] += 20;
	}
	if (object_is_weapon_ammo(o_ptr))
	{
		if (old_ds > o_ptr->ds) drain_value[TR_ES_ATTACK] += (old_ds-o_ptr->ds)*10;

		if (old_dd > o_ptr->dd) drain_value[TR_ES_ATTACK] += (old_dd-o_ptr->dd)*10;
	}
	if (old_to_h > o_ptr->to_h) drain_value[TR_ES_ATTACK] += (old_to_h-o_ptr->to_h)*10;
	if (old_to_d > o_ptr->to_d) drain_value[TR_ES_ATTACK] += (old_to_d-o_ptr->to_d)*10;
	if (old_ac > o_ptr->ac) drain_value[TR_ES_AC] += (old_ac-o_ptr->ac)*10;
	if (old_to_a > o_ptr->to_a) drain_value[TR_ES_AC] += (old_to_a-o_ptr->to_a)*10;

	/*:::����ꂽ�G�b�Z���X�Ɍ����|���ă}�C�i�X���ʂ̌��Z������*/
	for (i = 0; i < sizeof(drain_value) / sizeof(int); i++)
	{
		drain_value[i] *= number;
		drain_value[i] = drain_value[i] * dec / 4;
		drain_value[i] = MAX(drain_value[i], 0);
		if ((o_ptr->tval >= TV_BULLET) && (o_ptr->tval <= TV_BOLT)) drain_value[i] /= 10;
		if (drain_value[i])
		{
			observe = TRUE;
		}
	}
	if (!observe)
	{
		msg_print(_("�G�b�Z���X�͒��o�ł��܂���ł����B", "You were not able to extract any essence."));
	}
	else
	{
		/*:::�G�b�Z���X��\����magic_num1[]�։��Z*/
		msg_print(_("���o�����G�b�Z���X:", "Extracted essences:"));

		for (i = 0; essence_name[i]; i++)
		{
			if (!essence_name[i][0]) continue;
			if (!drain_value[i]) continue;

			p_ptr->magic_num1[i] += drain_value[i];
			p_ptr->magic_num1[i] = MIN(20000, p_ptr->magic_num1[i]);
			msg_print(NULL);
			msg_format("%s...%d%s", essence_name[i], drain_value[i], _("�B", ". "));
		}
	}

	/* Apply autodestroy/inscription to the drained item */
	autopick_alter_item(item, TRUE);

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);
}
#endif


/*:::�I�������A�C�e������G�b�Z���X�𒊏o����*/
///mod141010 �Ó���ƕ���t�̂��߂ɑ啝�ύX �S�ẴG�b�Z���X���z�������A�C�e���͏�����
static void drain_essence(void)
{
	int drain_value[sizeof(p_ptr->magic_num1) / sizeof(s32b)];
	int i, item;
	int dec = 4;
	bool observe = FALSE;
	u32b old_flgs[TR_FLAG_SIZE];
	object_type *o_ptr;
	cptr            q, s;
	s16b next_o_idx, weight;

	for (i = 0; i < sizeof(drain_value) / sizeof(int); i++) drain_value[i] = 0;

	item_tester_hook = item_tester_hook_drain_essence;
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�����璊�o���܂����H";
	s = "���o�ł���A�C�e��������܂���B";
#else
	q = "Extract from which item? ";
	s = "You have nothing you can extract from.";
#endif

	//v2.0.19 ���ォ����G�b�Z���X���o�ł���悤�ɂ���@�����ݕP��p
	if (!get_item(&item, q, s, (USE_INVEN|USE_FLOOR))) return;

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

	if (!object_is_weapon_armour_ammo(o_ptr) || object_is_known(o_ptr) && !object_is_nameless(o_ptr))
	{
		char o_name[MAX_NLEN];
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
		if (!get_check(format("%s��%s�����Ȃ�܂��B��낵���ł����H", o_name,(o_ptr->number>1?"�S��":"")))) return;
#else
		if (!get_check(format("Really extract from %s? ", o_name))) return;
#endif
	}

	//energy_use = 100;
	

	if(o_ptr->tval == TV_SOUVENIR || o_ptr->tval == TV_MATERIAL) //�f�ރA�C�e���ȂǂɈꎞ�I��flgs��pval��t�^
	{
		int tv = o_ptr->tval;
		int sv = o_ptr->sval;

		for(i=0;i<TR_FLAG_SIZE;i++) old_flgs[i] = 0L;

		if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIKARIGOKE)  add_flag(old_flgs, TR_LITE); //�q�J���S�P�@����
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_STONE) ; //�����Ȃ�
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_HEMATITE)
		{
			int rand_flg = (TR_RES_ACID + randint0(15));
			o_ptr->to_d = 10 + randint1(10); //�w�}�^�C�g�@�U���A�����_���ϐ�
			add_flag(old_flgs, rand_flg);

		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_MAGNETITE)
		{
			int rand_flg = (TR_SLAY_EVIL + randint0(9));
			add_flag(old_flgs, rand_flg);
		 	o_ptr->to_a = 10 + randint1(10); //�}�O�l�^�C�g�@�h��A�����_���X���C
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_ARSENIC) //��f�z�΁@�ŎE�A�ϓ�
		{
		 	add_flag(old_flgs, TR_BRAND_POIS);
		 	add_flag(old_flgs, TR_RES_POIS);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_MERCURY) //����z�΁@�m�\�A����
		{
			 add_flag(old_flgs, TR_INT);
			 add_flag(old_flgs, TR_WIS);
			o_ptr->pval = randint1(3);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_MITHRIL) //�~�X�����@��p�A���́A�B���A�׃X���C
		{
			 add_flag(old_flgs, TR_DEX);
			 add_flag(old_flgs, TR_CHR);
			 add_flag(old_flgs, TR_STEALTH);
			o_ptr->pval = randint1(3);
			add_flag(old_flgs, TR_SLAY_EVIL);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_ADAMANTITE) //�A�_�}���^�C�g�@�r�́A�ϋv�A�h��
		{
			 add_flag(old_flgs, TR_STR);
			 add_flag(old_flgs, TR_CON);
			o_ptr->pval = randint1(3);
			o_ptr->to_a = 30;
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONNAIL) //���̒܁@�U���A�S�����A�ǉ��U��
		{
			add_flag(old_flgs, TR_BRAND_FIRE);
			add_flag(old_flgs, TR_BRAND_COLD);
			add_flag(old_flgs, TR_BRAND_ELEC);
			add_flag(old_flgs, TR_BRAND_ACID);
			add_flag(old_flgs, TR_BRAND_POIS);
			add_flag(old_flgs, TR_BLOWS);
			o_ptr->pval = 2 + randint0(2);
			o_ptr->to_d = 100;
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONSCALE) //���̗؁@�h��A��ʑϐ�����
		{
			for(i=0;i<3;i++)
			{
				int rand_flg = (TR_RES_POIS + randint0(11));
				add_flag(old_flgs, rand_flg);
			}
			o_ptr->to_a = 10;
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_NIGHTMARE_FRAGMENT) //�����̂�����@�U���A�l�ԃX���C�A�z��
		{
			o_ptr->to_d = 10 + randint1(10);
			add_flag(old_flgs, TR_SLAY_HUMAN);
			add_flag(old_flgs, TR_VAMPIRIC);
		}
		else if (tv == TV_MATERIAL && sv == SV_MATERIAL_SKULL) //���W���@�n���ϐ�
		{
			add_flag(old_flgs, TR_RES_NETHER);
		}

		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_ISHIZAKURA) //�΍��@�n���ϐ��A�J�I�X�ϐ�
		{
			add_flag(old_flgs, TR_RES_NETHER);
			add_flag(old_flgs, TR_RES_CHAOS);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_BROKEN_NEEDLE) //�܂ꂽ�j�@�����A�A���f�b�h�A�d���A�_�i�X���C
		{
			add_flag(old_flgs, TR_SLAY_UNDEAD);
			add_flag(old_flgs, TR_SLAY_KWAI);
			add_flag(old_flgs, TR_SLAY_DEMON);
			add_flag(old_flgs, TR_SLAY_DEITY);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_HOPE_FRAGMENT) //��]�̌����@���́A�׃X���C(�O�{)
		{
			add_flag(old_flgs, TR_SLAY_EVIL);
			add_flag(old_flgs, TR_FORCE_WEAPON);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) //�ɜQ�������@/�_(��{) +�R���ǉ�
		{
			add_flag(old_flgs, TR_SLAY_DEITY);
			add_flag(old_flgs, TR_SAVING);

		}
		else if (tv == TV_MATERIAL && sv == SV_MATERIAL_RYUUZYU) //����@/��(��{)
		{
			add_flag(old_flgs, TR_SLAY_DRAGON);
		}

		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_ICESCALE) //�X�̗؁@�h��A���ϐ��A��C�ϐ��A�X�U��
		{
			add_flag(old_flgs, TR_BRAND_COLD);
			add_flag(old_flgs, TR_RES_COLD);
			add_flag(old_flgs, TR_RES_WATER);
			o_ptr->to_a = 10;
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_GELSEMIUM) //�Q���Z�~�E���E�G���K���X�@�J�I�X�U���A�ŎE�A�ϓ�
		{
		 	add_flag(old_flgs, TR_BRAND_POIS);
		 	add_flag(old_flgs, TR_RES_POIS);
		 	add_flag(old_flgs, TR_CHAOTIC);
		}

		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_METEORICIRON) //覓S�@�S����U���A�S�X���C
		{
			add_flag(old_flgs, TR_CHAOTIC);
			add_flag(old_flgs, TR_VAMPIRIC);
			add_flag(old_flgs, TR_FORCE_WEAPON);
			add_flag(old_flgs, TR_VORPAL);
			add_flag(old_flgs, TR_IMPACT);
			add_flag(old_flgs, TR_SLAY_EVIL);
			add_flag(old_flgs, TR_SLAY_GOOD);
			add_flag(old_flgs, TR_SLAY_ANIMAL);
			add_flag(old_flgs, TR_SLAY_HUMAN);
			add_flag(old_flgs, TR_SLAY_UNDEAD);
			add_flag(old_flgs, TR_SLAY_DRAGON);
			add_flag(old_flgs, TR_SLAY_DEITY);
			add_flag(old_flgs, TR_SLAY_DEMON);
			add_flag(old_flgs, TR_SLAY_KWAI);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIHIIROKANE) //�q�q�C���J�l *�S��*
		{
			for(i=0;i<106;i++) add_flag(old_flgs, i);
			add_flag(old_flgs, TR_BOOMERANG); //v1.1.77b
			add_flag(old_flgs, TR_SAVING); //v1.1.77b
			add_flag(old_flgs, TR_DISARM); //v1.1.77b

			o_ptr->pval = 2 + randint0(3);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_SCRAP_IRON) //�S�N�Y �����_��
		{
			o_ptr->pval = randint1(3);
			do
			{
				int flag = randint0(106);

				//v1.1.99 �V�t���O��K���ɒǉ�
				if (one_in_(100)) add_flag(old_flgs, TR_SAVING);
				else if (one_in_(100)) add_flag(old_flgs, TR_DISARM);
				else add_flag(old_flgs, flag);

			}while(!one_in_(3));
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_MYSTERIUM) //�~�X�e���E�� �����_�����
		{
			int chance = 5+randint1(5);
			while(!one_in_(7)) chance += randint1(4);
			o_ptr->pval = 2+randint1(3);
			for(;chance>0;chance--)
			{
				int flag = randint0(106);

				//v1.1.99 �V�t���O��K���ɒǉ�
				if (one_in_(100)) add_flag(old_flgs, TR_SAVING);
				else if (one_in_(100)) add_flag(old_flgs, TR_DISARM);

				else add_flag(old_flgs, flag);
			}
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_COAL) //�Y�@�ΈÍ�
		{
		 	add_flag(old_flgs, TR_RES_DARK);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_TAKAKUSAGORI) //�����S�̌���|�@�}�񕜁A�Í��n���J�I�X�ϐ�
		{
		 	add_flag(old_flgs, TR_REGEN);
		 	add_flag(old_flgs, TR_RES_DARK);
		 	add_flag(old_flgs, TR_RES_NETHER);
		 	add_flag(old_flgs, TR_RES_CHAOS);
		}

		else if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_PHOENIX_FEATHER ) //�s�����̉H���@�Ɖu*2
		{
			add_flag(old_flgs, TR_IM_FIRE);
			add_flag(old_flgs, TR_IM_COLD);
		}

		else if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_ILMENITE ) //���̃C�����i�C�g�@���C�A����ϐ�
		{
			add_flag(old_flgs, TR_RES_INSANITY);
			add_flag(old_flgs, TR_RES_TIME);
		}
		else if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) //�Â����̂̐����@���@��x�ቺ�A����MP�ቺ
		{
			add_flag(old_flgs, TR_DEC_MANA);
			add_flag(old_flgs, TR_EASY_SPELL);
		}


		else if(tv == TV_MATERIAL && (sv == SV_MATERIAL_GARNET || sv == SV_MATERIAL_TOPAZ) ) //�K�[�l�b�g�A�g�p�[�Y �Α���
		{
			add_flag(old_flgs, TR_BRAND_FIRE);
		}
		else if(tv == TV_MATERIAL && (sv == SV_MATERIAL_AMETHYST || sv == SV_MATERIAL_LAPISLAZULI  )) //�A���W�X�g�A���s�X���Y�� �d������
		{
			add_flag(old_flgs, TR_BRAND_ELEC);
		}
		else if(tv == TV_MATERIAL && (sv == SV_MATERIAL_AQUAMARINE ||  sv == SV_MATERIAL_OPAL )) //�A�N�A�}�����A�I�p�[���@ ��C����
		{
			add_flag(old_flgs, TR_BRAND_COLD);
		}
		else if(tv == TV_MATERIAL && (sv == SV_MATERIAL_PERIDOT|| sv == SV_MATERIAL_MOONSTONE)) //�y���h�b�g�A���[���X�g�[���@ �_����
		{
			add_flag(old_flgs, TR_BRAND_ACID);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_DIAMOND ) //�_�C�������h �؂ꖡ(���ꏈ����5�{����)
		{
			add_flag(old_flgs, TR_VORPAL);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_EMERALD  ) //�G�������h �@�S�X���C
		{
			add_flag(old_flgs, TR_SLAY_EVIL);
			add_flag(old_flgs, TR_SLAY_GOOD);
			add_flag(old_flgs, TR_SLAY_ANIMAL);
			add_flag(old_flgs, TR_SLAY_HUMAN);
			add_flag(old_flgs, TR_SLAY_UNDEAD);
			add_flag(old_flgs, TR_SLAY_DRAGON);
			add_flag(old_flgs, TR_SLAY_DEITY);
			add_flag(old_flgs, TR_SLAY_DEMON);
			add_flag(old_flgs, TR_SLAY_KWAI);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_RUBY  ) //���r�[�@ �z��(���ꏈ����3�{����)
		{
			add_flag(old_flgs, TR_VAMPIRIC);
		}
		else if(tv == TV_MATERIAL && sv == SV_MATERIAL_SAPPHIRE ) //�T�t�@�C�A�@ ����(���ꏈ����3�{����)
		{
			add_flag(old_flgs, TR_FORCE_WEAPON);
		}
		else if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_ASIA ) //�G�C�W���̐Ԑ΁@ /�׎��� 30�{
		{
			add_flag(old_flgs, TR_SLAY_EVIL);
			add_flag(old_flgs, TR_SLAY_UNDEAD);
			add_flag(old_flgs, TR_SLAY_DEMON);
		}
		else
		{
			msg_print("ERROR:���̃A�C�e���̃G�b�Z���X���o��������`����Ă��Ȃ�");
			return;
		}
	}

	else
	{
		
		object_flags(o_ptr, old_flgs);
		/*:::�􂢂Ȃǃ}�C�i�X���ʂ�dec�����Z���A1���邲�ƂɌ�̏����ŃG�b�Z���X��1/4����*/
		if (o_ptr->curse_flags & (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE)) dec--;
		if (have_flag(old_flgs, TR_ADD_L_CURSE)) dec--;
		if (have_flag(old_flgs, TR_ADD_H_CURSE)) dec--;
		if (have_flag(old_flgs, TR_AGGRAVATE)) dec--;
		//if (have_flag(old_flgs, TR_NO_TELE)) dec--; //v1.1.74 ���e���|�̃G�b�Z���X�̈����ύX
		if (have_flag(old_flgs, TR_DRAIN_EXP)) dec--;
		if (have_flag(old_flgs, TR_TY_CURSE)) dec--;
	}
	/*:::�G�b�Z���X���o�p�Ƀt���O�������Ă�炵��*/
	if (have_flag(old_flgs, TR_KILL_EVIL)) add_flag(old_flgs, TR_SLAY_EVIL);
	if (have_flag(old_flgs, TR_KILL_GOOD)) add_flag(old_flgs, TR_SLAY_GOOD);
	if (have_flag(old_flgs, TR_KILL_ANIMAL)) add_flag(old_flgs, TR_SLAY_ANIMAL);
	if (have_flag(old_flgs, TR_KILL_HUMAN)) add_flag(old_flgs, TR_SLAY_HUMAN);
	if (have_flag(old_flgs, TR_KILL_UNDEAD)) add_flag(old_flgs, TR_SLAY_UNDEAD);
	if (have_flag(old_flgs, TR_KILL_DRAGON)) add_flag(old_flgs, TR_SLAY_DRAGON);
	if (have_flag(old_flgs, TR_KILL_DEITY)) add_flag(old_flgs, TR_SLAY_DEITY);
	if (have_flag(old_flgs, TR_KILL_DEMON)) add_flag(old_flgs, TR_SLAY_DEMON);
	if (have_flag(old_flgs, TR_KILL_KWAI)) add_flag(old_flgs, TR_SLAY_KWAI);
	if (have_flag(old_flgs, TR_EX_VORPAL)) add_flag(old_flgs, TR_VORPAL);





#if 0
	old_to_a = o_ptr->to_a;
	old_ac = o_ptr->ac;
	old_to_h = o_ptr->to_h;
	old_to_d = o_ptr->to_d;
	old_ds = o_ptr->ds;
	old_dd = o_ptr->dd;
	old_pval = o_ptr->pval;
	old_name2 = o_ptr->name2;
	old_timeout = o_ptr->timeout;
	iy = o_ptr->iy;
	ix = o_ptr->ix;
	next_o_idx = o_ptr->next_o_idx;
	marked = o_ptr->marked;
	weight = o_ptr->weight;
	number = o_ptr->number;
	/*:::���̃A�C�e���̃G�S�Ȃǂ��������Ċ�{���݂̂ɂ��Ă���H*/
	//object_prep(o_ptr, o_ptr->k_idx);
	o_ptr->iy=iy;
	o_ptr->ix=ix;
	o_ptr->next_o_idx=next_o_idx;
	o_ptr->marked=marked;
	o_ptr->number = number;
	if (o_ptr->tval == TV_DRAG_ARMOR) o_ptr->timeout = old_timeout;
	if (item >= 0) p_ptr->total_weight += (o_ptr->weight*o_ptr->number - weight*number);
	o_ptr->ident |= (IDENT_MENTAL);
	object_aware(o_ptr);
	object_known(o_ptr);

	object_flags(o_ptr, new_flgs);
#endif

	/*:::�t���O���G�b�Z���X�ɕϊ����Ă���*/
	for (i = 0; essence_info[i].add_name; i++)
	{
		essence_type *es_ptr = &essence_info[i];
		int pval = 0;

		/*:::�G���t�̃N���[�N�Ȃǌ��Xpval�l�����A�C�e���̏ꍇ������G�b�Z���X����A�C�e����{�l�������Ă���炵��*/
		//if (es_ptr->add < TR_FLAG_MAX && is_pval_flag(es_ptr->add) && old_pval)
		//	pval = (have_flag(new_flgs, es_ptr->add)) ? old_pval - o_ptr->pval : old_pval;
		pval = o_ptr->pval;

		/*:::essence_info[i]�ɊY������G�b�Z���X�������Ă����ꍇ�i�����ꂪ�x�[�X�A�C�e���̎����Ă�����̂łȂ��ꍇ�j*/
		if (es_ptr->add < TR_FLAG_MAX &&
		   /* (!have_flag(new_flgs, es_ptr->add) || pval) && */
		    have_flag(old_flgs, es_ptr->add))
		{
			if (is_pval_flag(es_ptr->add))
			{
				drain_value[es_ptr->essence] += 10 * pval;
			}
			/*:::-2�ɂȂ�͉̂Η�d�̃I�[���B��̏����ɃX�L�b�v���A����ȊO��drain_value�ɒl������*/
			else if (es_ptr->essence != -2)
			{
				drain_value[es_ptr->essence] += 10;
			}
			else if (es_ptr->add == TR_SH_FIRE)
			{
				drain_value[TR_BRAND_FIRE] += 10;
				drain_value[TR_RES_FIRE] += 10;
			}
			else if (es_ptr->add == TR_SH_ELEC)
			{
				drain_value[TR_BRAND_ELEC] += 10;
				drain_value[TR_RES_ELEC] += 10;
			}
			else if (es_ptr->add == TR_SH_COLD)
			{
				drain_value[TR_BRAND_COLD] += 10;
				drain_value[TR_RES_COLD] += 10;
			}
		}
	}

	/* �_�C�X�����������@������
	if (object_is_weapon_ammo(o_ptr))
	{
		if (old_ds > o_ptr->ds) drain_value[TR_ES_ATTACK] += (old_ds-o_ptr->ds)*10;

		if (old_dd > o_ptr->dd) drain_value[TR_ES_ATTACK] += (old_dd-o_ptr->dd)*10;
	}
	*/
	if (o_ptr->to_h) drain_value[TR_ES_ATTACK] += (o_ptr->to_h)*10;
	if (o_ptr->to_d) drain_value[TR_ES_ATTACK] += (o_ptr->to_d)*10;
	if (o_ptr->to_a) drain_value[TR_ES_AC] += (o_ptr->to_a)*10;

	/*:::����ꂽ�G�b�Z���X�Ɍ����|���ă}�C�i�X���ʂ̌��Z������*/
	for (i = 0; i < sizeof(drain_value) / sizeof(int); i++)
	{
		drain_value[i] *= o_ptr->number;
		drain_value[i] = drain_value[i] * dec / 4;
		drain_value[i] = MAX(drain_value[i], 0);
//		if ((o_ptr->tval >= TV_BULLET) && (o_ptr->tval <= TV_BOLT)) drain_value[i] /= 10;
		if(object_is_needle_arrow_bolt(o_ptr)) drain_value[i] /= 10;
		if (drain_value[i])
		{
			observe = TRUE;
		}
	}
	if (!observe)
	{
		msg_print(_("�G�b�Z���X�͒��o�ł��܂���ł����B", "You were not able to extract any essence."));
	}
	else
	{
		/*:::�G�b�Z���X��\����magic_num1[]�։��Z*/
		msg_print(_("���o�����G�b�Z���X:", "Extracted essences:"));

		for (i = 0; essence_name[i]; i++)
		{
			if (!essence_name[i][0]) continue;
			if (!drain_value[i]) continue;

			//�ꕔ�̕�΂��璊�o����ƃG�b�Z���X������
			if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_DIAMOND) drain_value[i] *= 10;
			if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_RUBY) drain_value[i] *= 5;
			if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_SAPPHIRE) drain_value[i] *= 3;
			if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_HOPE_FRAGMENT) drain_value[i] *= 3;
			if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_IZANAGIOBJECT) drain_value[i] *= 2;
			if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_RYUUZYU) drain_value[i] *= 2;
			if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_ASIA) drain_value[i] *= 30;

			//v1.1.74 �q�q�C���J�l�̃G�b�Z���X��3�{��
			if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_HIHIIROKANE) drain_value[i] *= 3;

			p_ptr->magic_num1[i] += drain_value[i];
			//v1.1.74 �ő�l20000��99999
			p_ptr->magic_num1[i] = MIN(99999, p_ptr->magic_num1[i]);
			msg_print(NULL);
			msg_format("%s...%d%s", essence_name[i], drain_value[i], _("�B", ". "));
		}
	}

	/* Apply autodestroy/inscription to the drained item */
	//autopick_alter_item(item, TRUE);

	//�f�ނɈꎞ�I�ɕt�^�����p�����[�^���폜
	if(!object_is_weapon_armour_ammo(o_ptr))
	{
		o_ptr->to_d = 0;
		o_ptr->to_a = 0;
		o_ptr->to_h = 0;
		o_ptr->pval = 0;
	}

	if(item >= 0)
	{
		inven_item_increase(item, -o_ptr->number);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		//v2.1.0 �������1�ɂȂ��Ă����̂ŏC��
		floor_item_increase(0 - item, -o_ptr->number);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}	
	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);
}





/*:::�b��t�̃G�b�Z���X�t���p���j���[��\�����ǂ̈ꗗ���g�����I���@�ꗗ�̒��̑I����add_essence()�ōs��*/
static int choose_essence(void)
{
	int mode = 0;
	char choice;
	int menu_line = (use_menu ? 1 : 0);

#ifdef JP
	cptr menu_name[] = {
		"���푮��", 
		"�ϐ�",
		"�\��",
		"���l",
		"�X���C",
		"ESP",
		"���̑�"
	};
#else
	cptr menu_name[] = {
		"Brand weapon",
		"Resistance",
		"Ability",
		"Magic number", 
		"Slay",
		"ESP",
		"Others"
	};
#endif
	const int mode_max = 7;

#ifdef ALLOW_REPEAT
	if (repeat_pull(&mode) && 1 <= mode && mode <= mode_max)
		return mode;
	mode = 0;
#endif /* ALLOW_REPEAT */

	if (use_menu)
	{
		screen_save();

		while(!mode)
		{
			int i;
			for (i = 0; i < mode_max; i++)
#ifdef JP
				prt(format(" %s %s", (menu_line == 1+i) ? "�t" : "  ", menu_name[i]), 2 + i, 14);
			prt("�ǂ̎�ނ̃G�b�Z���X�t�����s���܂����H", 0, 0);
#else
				prt(format(" %s %s", (menu_line == 1+i) ? "> " : "  ", menu_name[i]), 2 + i, 14);
			prt("Choose from menu.", 0, 0);
#endif

			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return 0;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line += mode_max - 1;
				break;
			case '\r':
			case '\n':
			case 'x':
			case 'X':
				mode = menu_line;
				break;
			}
			if (menu_line > mode_max) menu_line -= mode_max;
		}
		screen_load();
	}
	else
	{
		screen_save();
		while (!mode)
		{
			int i;

			for (i = 0; i < mode_max; i++)
				prt(format("  %c) %s", 'a' + i, menu_name[i]), 2 + i, 14);

#ifdef JP
			if (!get_com("����t�����܂���:", &choice, TRUE))
#else
			if (!get_com("Command :", &choice, TRUE))
#endif
			{
				screen_load();
				return 0;
			}

			if (isupper(choice)) choice = tolower(choice);

			if ('a' <= choice && choice <= 'a' + (char)mode_max - 1)
				mode = (int)choice - 'a' + 1;
		}
		screen_load();
	}

#ifdef ALLOW_REPEAT
	repeat_push(mode);
#endif /* ALLOW_REPEAT */
	return mode;
}

#if 0
�b��t�����[�`��

/*:::�b��t�@�G�b�Z���X�t��*/
/*:::mode10:����h���*/
static void add_essence(int mode)
{
	int item, max_num = 0;
	int i;
	bool flag,redraw;
	char choice;
	cptr            q, s;
	object_type *o_ptr;
	int ask = TRUE;
	char out_val[160];
	int num[22];
	char o_name[MAX_NLEN];
	int use_essence;
	essence_type *es_ptr;

	int menu_line = (use_menu ? 1 : 0);

	for (i = 0; essence_info[i].add_name; i++)
	{
		es_ptr = &essence_info[i];

		if (es_ptr->type != mode) continue;
		num[max_num++] = i;
	}

#ifdef ALLOW_REPEAT
	if (!repeat_pull(&i) || i<0 || i>=max_num)
	{
#endif /* ALLOW_REPEAT */


	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt */
#ifdef JP
	(void) strnfmt(out_val, 78, "('*'�ňꗗ, ESC�Œ��f) �ǂ̔\�͂�t�����܂����H");
#else
	(void)strnfmt(out_val, 78, "(*=List, ESC=exit) Add which ability? ");
#endif
	if (use_menu) screen_save();

	/* Get a spell from the user */

	choice = (always_show_list || use_menu) ? ESCAPE:1;
	while (!flag)
	{
		bool able[22];
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break; 

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					return;
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (max_num-1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case '4':
				case 'h':
				case 'H':
				{
					menu_line = 1;
					break;
				}
				case '6':
				case 'l':
				case 'L':
				{
					menu_line = max_num;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				case '\n':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if (menu_line > max_num) menu_line -= max_num;
		}
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				byte y, x = 10;
				int ctr;
				char dummy[80], dummy2[80];
				byte col;

				strcpy(dummy, "");

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				for (y = 1; y < 24; y++)
					prt("", y, x);

				/* Print header(s) */
#ifdef JP
				prt(format("   %-43s %6s/%s", "�\��(�K�v�G�b�Z���X)", "�K�v��", "������"), 1, x);

#else
				prt(format("   %-43s %6s/%s", "Ability (needed essence)", "Needs", "Possess"), 1, x);
#endif
				/* Print list */
				for (ctr = 0; ctr < max_num; ctr++)
				{
					es_ptr = &essence_info[num[ctr]];

					if (use_menu)
					{
						if (ctr == (menu_line-1))
#ifdef JP
							strcpy(dummy, "�t ");
#else
							strcpy(dummy, ">  ");
#endif
						else strcpy(dummy, "   ");
						
					}
					/* letter/number for power selection */
					else
					{
						sprintf(dummy, "%c) ",I2A(ctr));
					}

					strcat(dummy, es_ptr->add_name);

					col = TERM_WHITE;
					able[ctr] = TRUE;

					if (es_ptr->essence != -1)
					{
						strcat(dummy, format("(%s)", essence_name[es_ptr->essence]));
						if (p_ptr->magic_num1[es_ptr->essence] < es_ptr->value) able[ctr] = FALSE;
					}
					else
					{
						switch(es_ptr->add)
						{
						case ESSENCE_SH_FIRE:
#ifdef JP
							strcat(dummy, "(�Ċ�+�ωΉ�)");
#else
							strcat(dummy, "(brand fire + res.fire)");
#endif
							if (p_ptr->magic_num1[TR_BRAND_FIRE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_SH_ELEC:
#ifdef JP
							strcat(dummy, "(�d��+�ϓd��)");
#else
							strcat(dummy, "(brand elec. + res. elec.)");
#endif
							if (p_ptr->magic_num1[TR_BRAND_ELEC] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_SH_COLD:
#ifdef JP
							strcat(dummy, "(����+�ϗ�C)");
#else
							strcat(dummy, "(brand cold + res. cold)");
#endif
							if (p_ptr->magic_num1[TR_BRAND_COLD] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_RESISTANCE:
#ifdef JP
							strcat(dummy, "(�ωΉ�+�ϗ�C+�ϓd��+�ώ_)");
#else
							strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
							if (p_ptr->magic_num1[TR_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ACID] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_SUSTAIN:
#ifdef JP
							strcat(dummy, "(�ωΉ�+�ϗ�C+�ϓd��+�ώ_)");
#else
							strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
							if (p_ptr->magic_num1[TR_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ACID] < es_ptr->value) able[ctr] = FALSE;
							break;
						}
					}

					if (!able[ctr]) col = TERM_RED;

					if (es_ptr->essence != -1)
					{
						sprintf(dummy2, "%-49s %3d/%d", dummy, es_ptr->value, (int)p_ptr->magic_num1[es_ptr->essence]);
					}
					else
					{
						sprintf(dummy2, "%-49s %3d/(\?\?)", dummy, es_ptr->value);
					}

					c_prt(col, dummy2, ctr+2, x);
				}
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= max_num) || !able[i])
		{
			bell();
			continue;
		}

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
			(void) strnfmt(tmp_val, 78, "%s��t�����܂����H ", essence_info[num[i]].add_name);
#else
			(void) strnfmt(tmp_val, 78, "Add the abilitiy of %s? ", essence_info[num[i]].add_name);
#endif

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	if (!flag) return;

#ifdef ALLOW_REPEAT
	repeat_push(i);
	}
#endif /* ALLOW_REPEAT */

	es_ptr = &essence_info[num[i]];

	if (es_ptr->add == ESSENCE_SLAY_GLOVE)
		item_tester_tval = TV_GLOVES;
	else if (mode == 1 || mode == 5)
		item_tester_hook = item_tester_hook_melee_ammo;
	else if (es_ptr->add == ESSENCE_ATTACK)
		item_tester_hook = object_allow_enchant_weapon;
	else if (es_ptr->add == ESSENCE_AC)
		item_tester_hook = object_is_armour;
	else
		item_tester_hook = object_is_weapon_armour_ammo;
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�������ǂ��܂����H";
	s = "���ǂł���A�C�e��������܂���B";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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

	if ((mode != 10) && (object_is_artifact(o_ptr) || object_is_smith(o_ptr)))
	{
#ifdef JP
		msg_print("���̃A�C�e���͂���ȏ���ǂł��Ȃ��B");
#else
		msg_print("This item is no more able to be improved.");
#endif
		return;
	}

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	use_essence = es_ptr->value;
	if ((o_ptr->tval >= TV_BULLET) && (o_ptr->tval <= TV_BOLT)) use_essence = (use_essence+9)/10;
	if (o_ptr->number > 1)
	{
		use_essence *= o_ptr->number;
#ifdef JP
		msg_format("%d����̂ŃG�b�Z���X��%d�K�v�ł��B", o_ptr->number, use_essence);
#else
		msg_format("It will take %d essences.",use_essence);
#endif

	}

	if (es_ptr->essence != -1)
	{
		if (p_ptr->magic_num1[es_ptr->essence] < use_essence)
		{
#ifdef JP
			msg_print("�G�b�Z���X������Ȃ��B");
#else
			msg_print("You don't have enough essences.");
#endif
			return;
		}
		if (is_pval_flag(es_ptr->add))
		{
			if (o_ptr->pval < 0)
			{
#ifdef JP
				msg_print("���̃A�C�e���̔\�͏C�����������邱�Ƃ͂ł��Ȃ��B");
#else
				msg_print("You cannot increase magic number of this item.");
#endif
				return;
			}
			else if (es_ptr->add == TR_BLOWS)
			{
				if (o_ptr->pval > 1)
				{
#ifdef JP
					if (!get_check("�C���l��1�ɂȂ�܂��B��낵���ł����H")) return;
#else
					if (!get_check("The magic number of this weapon will become 1. Are you sure? ")) return;
#endif
				}

				o_ptr->pval = 1;
#ifdef JP
				msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
				msg_format("It will take %d essences.", use_essence);
#endif
			}
			else if (o_ptr->pval > 0)
			{
				use_essence *= o_ptr->pval;
#ifdef JP
				msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
				msg_format("It will take %d essences.", use_essence);
#endif
			}
			else
			{
				char tmp[80];
				char tmp_val[160];
				int pval;
				int limit = MIN(5, p_ptr->magic_num1[es_ptr->essence]/es_ptr->value);

#ifdef JP
				sprintf(tmp, "�����t�����܂����H (1-%d): ", limit);
#else
				sprintf(tmp, "Enchant how many? (1-%d): ", limit);
#endif
				strcpy(tmp_val, "1");

				if (!get_string(tmp, tmp_val, 1)) return;
				pval = atoi(tmp_val);
				if (pval > limit) pval = limit;
				else if (pval < 1) pval = 1;
				o_ptr->pval += pval;
				use_essence *= pval;
#ifdef JP
				msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
				msg_format("It will take %d essences.", use_essence);
#endif
			}

			if (p_ptr->magic_num1[es_ptr->essence] < use_essence)
			{
#ifdef JP
				msg_print("�G�b�Z���X������Ȃ��B");
#else
				msg_print("You don't have enough essences.");
#endif
				return;
			}
		}
		else if (es_ptr->add == ESSENCE_SLAY_GLOVE)
		{
			char tmp_val[160];
			int val;
			int get_to_h, get_to_d;

			strcpy(tmp_val, "1");
#ifdef JP
			if (!get_string(format("�����t�����܂����H (1-%d):", p_ptr->lev/7+3), tmp_val, 2)) return;
#else
			if (!get_string(format("Enchant how many? (1-%d):", p_ptr->lev/7+3), tmp_val, 2)) return;
#endif
			val = atoi(tmp_val);
			if (val > p_ptr->lev/7+3) val = p_ptr->lev/7+3;
			else if (val < 1) val = 1;
			use_essence *= val;
#ifdef JP
			msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
			msg_format("It will take %d essences.", use_essence);
#endif
			if (p_ptr->magic_num1[es_ptr->essence] < use_essence)
			{
#ifdef JP
				msg_print("�G�b�Z���X������Ȃ��B");
#else
				msg_print("You don't have enough essences.");
#endif
				return;
			}
			get_to_h = ((val+1)/2+randint0(val/2+1));
			get_to_d = ((val+1)/2+randint0(val/2+1));
			o_ptr->xtra4 = (get_to_h<<8)+get_to_d;
			o_ptr->to_h += get_to_h;
			o_ptr->to_d += get_to_d;
		}
		p_ptr->magic_num1[es_ptr->essence] -= use_essence;
		if (es_ptr->add == ESSENCE_ATTACK)
		{
			if ((o_ptr->to_h >= p_ptr->lev/5+5) && (o_ptr->to_d >= p_ptr->lev/5+5))
			{
#ifdef JP
				msg_print("���ǂɎ��s�����B");
#else
				msg_print("You failed to enchant.");
#endif
				energy_use = 100;
				return;
			}
			else
			{
				if (o_ptr->to_h < p_ptr->lev/5+5) o_ptr->to_h++;
				if (o_ptr->to_d < p_ptr->lev/5+5) o_ptr->to_d++;
			}
		}
		else if (es_ptr->add == ESSENCE_AC)
		{
			if (o_ptr->to_a >= p_ptr->lev/5+5)
			{
#ifdef JP
				msg_print("���ǂɎ��s�����B");
#else
				msg_print("You failed to enchant.");
#endif
				energy_use = 100;
				return;
			}
			else
			{
				if (o_ptr->to_a < p_ptr->lev/5+5) o_ptr->to_a++;
			}
		}
		else
		{
			o_ptr->xtra3 = es_ptr->add + 1;
		}
	}
	else
	{
		bool success = TRUE;

		switch(es_ptr->add)
		{
		case ESSENCE_SH_FIRE:
			if ((p_ptr->magic_num1[TR_BRAND_FIRE] < use_essence) || (p_ptr->magic_num1[TR_RES_FIRE] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_BRAND_FIRE] -= use_essence;
			p_ptr->magic_num1[TR_RES_FIRE] -= use_essence;
			break;
		case ESSENCE_SH_ELEC:
			if ((p_ptr->magic_num1[TR_BRAND_ELEC] < use_essence) || (p_ptr->magic_num1[TR_RES_ELEC] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_BRAND_ELEC] -= use_essence;
			p_ptr->magic_num1[TR_RES_ELEC] -= use_essence;
			break;
		case ESSENCE_SH_COLD:
			if ((p_ptr->magic_num1[TR_BRAND_COLD] < use_essence) || (p_ptr->magic_num1[TR_RES_COLD] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_BRAND_COLD] -= use_essence;
			p_ptr->magic_num1[TR_RES_COLD] -= use_essence;
			break;
		case ESSENCE_RESISTANCE:
		case ESSENCE_SUSTAIN:
			if ((p_ptr->magic_num1[TR_RES_ACID] < use_essence) || (p_ptr->magic_num1[TR_RES_ELEC] < use_essence) || (p_ptr->magic_num1[TR_RES_FIRE] < use_essence) || (p_ptr->magic_num1[TR_RES_COLD] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_RES_ACID] -= use_essence;
			p_ptr->magic_num1[TR_RES_ELEC] -= use_essence;
			p_ptr->magic_num1[TR_RES_FIRE] -= use_essence;
			p_ptr->magic_num1[TR_RES_COLD] -= use_essence;
			break;
		}
		if (!success)
		{
#ifdef JP
			msg_print("�G�b�Z���X������Ȃ��B");
#else
			msg_print("You don't have enough essences.");
#endif
			return;
		}
		if (es_ptr->add == ESSENCE_SUSTAIN)
		{
			add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
			add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
			add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
			add_flag(o_ptr->art_flags, TR_IGNORE_COLD);
		}
		else
		{
			o_ptr->xtra3 = es_ptr->add + 1;
		}
	}

	energy_use = 100;

#ifdef JP
	msg_format("%s��%s�̔\�͂�t�����܂����B", o_name, es_ptr->add_name);
#else
	msg_format("You have added ability of %s to %s.", es_ptr->add_name, o_name);
#endif

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);
}
#endif


//��Ύt��AC�t���\�ȕi
bool object_ribbon_helm(object_type *o_ptr)
{
	if (o_ptr->tval == TV_RIBBON || o_ptr->tval == TV_HEAD) return TRUE;

	return FALSE;
}

bool object_ribbon_and_armour(object_type *o_ptr)
{
	if (o_ptr->tval == TV_RIBBON) return TRUE;
	if (object_is_armour(o_ptr)) return TRUE;

	return FALSE;
}


//��Ύt�̒ǉ��U���t���\�ȕi
bool object_ring_weapon(object_type *o_ptr)
{
	if (o_ptr->tval == TV_RING ) return TRUE;
	if(object_is_melee_weapon(o_ptr)) return TRUE;

	return FALSE;
}

//��Ύt�̃G�b�Z���X�t���\�ȕi
bool item_tester_hook_jeweler(object_type *o_ptr)
{
	if (object_is_melee_weapon(o_ptr)) return TRUE;
	if(o_ptr->tval == TV_RING) return TRUE;
	if(o_ptr->tval == TV_AMULET) return TRUE;
	if(o_ptr->tval == TV_RIBBON) return TRUE;
	if(o_ptr->tval == TV_HEAD) return TRUE;
	if(o_ptr->tval == TV_LITE && o_ptr->sval != SV_LITE_TORCH) return TRUE;

	return FALSE;
}

//v1.1.74 �ݕP�̃G�b�Z���X�t���\�ȕi
bool item_tester_hook_keiki(object_type *o_ptr)
{
	if (object_is_weapon_armour_ammo(o_ptr)) return TRUE;
	if (o_ptr->tval == TV_RING) return TRUE;
	if (o_ptr->tval == TV_AMULET) return TRUE;
	if (o_ptr->tval == TV_RIBBON) return TRUE;
	if (o_ptr->tval == TV_LITE && o_ptr->sval != SV_LITE_TORCH) return TRUE;


	return FALSE;
}


/*:::�b��t�@�G�b�Z���X�t��*/
/*:::mode10:����h���*/
/*:::mode�I�������蒼�������Ƃ�FALSE��Ԃ��悤�ɂ���*/
static bool add_essence(int mode)
{
	int item, max_num = 0;
	int i;
	bool flag,redraw;
	char choice;
	cptr            q, s;
	object_type *o_ptr;
	int ask = TRUE;
	char out_val[160];
	int num[26]; //�s�����₷�Ƃ��͂����Ɖ���able[]���g��
	char o_name[MAX_NLEN];
	int use_essence;
	essence_type *es_ptr;

	int max_add_ac_hit_dam;

	int menu_line = (use_menu ? 1 : 0);

	max_add_ac_hit_dam = p_ptr->lev / 5 + 5;
	if (p_ptr->pclass == CLASS_KEIKI) max_add_ac_hit_dam += 5;

	for (i = 0; essence_info[i].add_name; i++)
	{
		es_ptr = &essence_info[i];

		if (es_ptr->type != mode) continue;
		num[max_num++] = i;
	}

#ifdef ALLOW_REPEAT
	if (!repeat_pull(&i) || i<0 || i>=max_num)
	{
#endif /* ALLOW_REPEAT */


	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt */
#ifdef JP
	(void) strnfmt(out_val, 78, "('*'�ňꗗ, ESC�Œ��f) �ǂ̔\�͂�t�����܂����H");
#else
	(void)strnfmt(out_val, 78, "(*=List, ESC=exit) Add which ability? ");
#endif
	if (use_menu) screen_save();

	/* Get a spell from the user */

	choice = (always_show_list || use_menu) ? ESCAPE:1;
	while (!flag)
	{
		bool able[26];
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break; 

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					return FALSE;
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (max_num-1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case '4':
				case 'h':
				case 'H':
				{
					menu_line = 1;
					break;
				}
				case '6':
				case 'l':
				case 'L':
				{
					menu_line = max_num;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				case '\n':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if (menu_line > max_num) menu_line -= max_num;
		}
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				byte y, x = 10;
				int ctr;
				char dummy[256], dummy2[1024];
				byte col;

				strcpy(dummy, "");

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				for (y = 1; y < 24; y++)
					prt("", y, x);

				/* Print header(s) */
#ifdef JP
				prt(format("   %-43s %6s/%s", "�\��(�K�v�G�b�Z���X)", "�K�v��", "������"), 1, x);

#else
				prt(format("   %-43s %6s/%s", "Ability (needed essence)", "Needs", "Possess"), 1, x);
#endif
				/* Print list */
				for (ctr = 0; ctr < max_num; ctr++)
				{
					es_ptr = &essence_info[num[ctr]];

					if (use_menu)
					{
						if (ctr == (menu_line-1))
#ifdef JP
							strcpy(dummy, "�t ");
#else
							strcpy(dummy, ">  ");
#endif
						else strcpy(dummy, "   ");
						
					}
					/* letter/number for power selection */
					else
					{
						sprintf(dummy, "%c) ",I2A(ctr));
					}

					strcat(dummy, es_ptr->add_name);

					col = TERM_WHITE;
					able[ctr] = TRUE;

					if (es_ptr->essence != -1)
					{
						strcat(dummy, format("(%s)", essence_name[es_ptr->essence]));
						if (p_ptr->magic_num1[es_ptr->essence] < es_ptr->value) able[ctr] = FALSE;
					}
					else
					{
						switch(es_ptr->add)
						{
						case ESSENCE_SH_FIRE:
#ifdef JP
							strcat(dummy, "(�Ċ�+�ωΉ�)");
#else
							strcat(dummy, "(brand fire + res.fire)");
#endif
							if (p_ptr->magic_num1[TR_BRAND_FIRE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_SH_ELEC:
#ifdef JP
							strcat(dummy, "(�d��+�ϓd��)");
#else
							strcat(dummy, "(brand elec. + res. elec.)");
#endif
							if (p_ptr->magic_num1[TR_BRAND_ELEC] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_SH_COLD:
#ifdef JP
							strcat(dummy, "(����+�ϗ�C)");
#else
							strcat(dummy, "(brand cold + res. cold)");
#endif
							if (p_ptr->magic_num1[TR_BRAND_COLD] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_RESISTANCE:
#ifdef JP
							strcat(dummy, "(�ωΉ�+�ϗ�C+�ϓd��+�ώ_)");
#else
							strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
							if (p_ptr->magic_num1[TR_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ACID] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_SUSTAIN:
#ifdef JP
							strcat(dummy, "(�ωΉ�+�ϗ�C+�ϓd��+�ώ_)");
#else
							strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
							if (p_ptr->magic_num1[TR_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ACID] < es_ptr->value) able[ctr] = FALSE;
							break;

						case ESSENCE_TMP_RES_ALL:
							strcat(dummy, "(�ωΉ�+�ϗ�C+�ϓd��+�ώ_+�ϓ�)");
							if (p_ptr->magic_num1[TR_RES_FIRE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_COLD] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ELEC] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_ACID] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_RES_POIS] < es_ptr->value) able[ctr] = FALSE;
							break;

						case ESSENCE_WHIRL_WIND:
							strcat(dummy, "(�؂ꖡ+�ǉ��U��)");
							if (p_ptr->magic_num1[TR_VORPAL] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_BLOWS] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_PSY_SPEAR:
							strcat(dummy, "(����+�i�v����+�u�[������)");
							if (p_ptr->magic_num1[TR_FORCE_WEAPON] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_LITE] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_THROW] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_L_RANGE_ATTACK:
							strcat(dummy, "(���͎ˌ�+�ǉ��U��)");
							if (p_ptr->magic_num1[TR_XTRA_MIGHT] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_BLOWS] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_HIT_AND_AWAY:
							strcat(dummy, "(�����ړ�+�e���|�[�g)");
							if (p_ptr->magic_num1[TR_SPEEDSTER] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_TELEPORT] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_HEAL_500:
							strcat(dummy, "(�}����+�j��)");
							if (p_ptr->magic_num1[TR_REGEN] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_BLESSED] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_INVULN:
							strcat(dummy, "(����+�Ɖu+�j��)");
							if (p_ptr->magic_num1[TR_REFLECT] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_IM_ACID] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_BLESSED] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_SS_ARROW:
							strcat(dummy, "(���͎ˌ�+�����ˌ�)");
							if (p_ptr->magic_num1[TR_XTRA_MIGHT] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_XTRA_SHOTS] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_DIM_DOOR:
							strcat(dummy, "(�e���|�[�g+���e���|�[�g)");
							if (p_ptr->magic_num1[TR_TELEPORT] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_NO_TELE] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_DUAL_CAST:
							strcat(dummy, "(���͎x�z+���@��x�ቺ+�X�s�[�h)");
							if (p_ptr->magic_num1[TR_MAGIC_MASTERY] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_EASY_SPELL] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_SPEED] < es_ptr->value) able[ctr] = FALSE;
							break;
						case ESSENCE_TIM_INC_DICE:
							strcat(dummy, "(����+�؂ꖡ+�n�k)");
							if (p_ptr->magic_num1[TR_FORCE_WEAPON] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_VORPAL] < es_ptr->value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[TR_IMPACT] < es_ptr->value) able[ctr] = FALSE;
							break;

						}
					}

					if (!able[ctr]) col = TERM_RED;

					if (es_ptr->essence != -1)
					{
						sprintf(dummy2, "%-49s %3d/%d", dummy, es_ptr->value, (int)p_ptr->magic_num1[es_ptr->essence]);
					}
					else
					{
						sprintf(dummy2, "%-49s %3d/(\?\?)", dummy, es_ptr->value);
					}

					c_prt(col, dummy2, ctr+2, x);
				}
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= max_num) || !able[i])
		{
			bell();
			continue;
		}

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
			(void) strnfmt(tmp_val, 78, "%s��t�����܂����H ", essence_info[num[i]].add_name);
#else
			(void) strnfmt(tmp_val, 78, "Add the abilitiy of %s? ", essence_info[num[i]].add_name);
#endif

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	if (!flag) return FALSE;

#ifdef ALLOW_REPEAT
	repeat_push(i);
	}
#endif /* ALLOW_REPEAT */

	es_ptr = &essence_info[num[i]];

	if (es_ptr->add == ESSENCE_SLAY_GLOVE ||  es_ptr->add == TR_GENZI)
	{
		item_tester_tval = TV_GLOVES;
	}
	else if(es_ptr->add == TR_XTRA_MIGHT || es_ptr->add == TR_XTRA_SHOTS )
	{
		item_tester_hook = object_is_shooting_weapon;
	}
	else if(es_ptr->add == TR_BLOWS )
	{
		if(p_ptr->pclass == CLASS_SH_DEALER) item_tester_hook = object_is_melee_weapon;
		else item_tester_hook = object_ring_weapon;//����+�w��
	}
	else if (mode == 1 || mode == 5)
	{
		if(p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_KEIKI) item_tester_hook = item_tester_hook_melee_ammo;
		else item_tester_hook = object_is_melee_weapon;
	}
	else if (es_ptr->add == ESSENCE_ATTACK)
	{
		item_tester_hook = object_allow_enchant_weapon;
	}
	else if (es_ptr->add == ESSENCE_AC)
	{
		if(p_ptr->pclass == CLASS_SH_DEALER) item_tester_hook = object_is_armour;
		else if (p_ptr->pclass == CLASS_KEIKI) item_tester_hook = object_ribbon_and_armour;
		else item_tester_hook = object_ribbon_helm;
	}
	else
	{
		if(p_ptr->pclass == CLASS_SH_DEALER) item_tester_hook = object_is_weapon_armour_ammo;
		else if (p_ptr->pclass == CLASS_KEIKI) item_tester_hook = item_tester_hook_keiki;
		else item_tester_hook = item_tester_hook_jeweler;

	}


	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	if(p_ptr->pclass == CLASS_SH_DEALER)
	{
		q = "�ǂ̃A�C�e�������ǂ��܂����H";
		s = "���ǂł���A�C�e��������܂���B";
	}
	else if (p_ptr->pclass == CLASS_KEIKI)
	{
		q = "�ǂ̃A�C�e���ɑ��`���s���܂����H";
		s = "���ǂł���A�C�e��������܂���B";

	}
	else
	{
		q = "�ǂ̃A�C�e���֑������{���܂����H";
		s = "�����ɓK�����A�C�e��������܂���B";
	}
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return TRUE;

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

	//if ((mode != 10) && (object_is_artifact(o_ptr) || object_is_smith(o_ptr)))
	if ((mode != 10) && (object_is_artifact(o_ptr) || o_ptr->xtra3 && o_ptr->xtra3 != SPECIAL_ESSENCE_ONI))
	{
#ifdef JP
		msg_print("���̃A�C�e���͂���ȏ���ǂł��Ȃ��B");
#else
		msg_print("This item is no more able to be improved.");
#endif
		return TRUE;
	}

///mod141009
	if(mode != 10 && o_ptr->tval != TV_GLOVES && es_ptr->add == TR_GENZI)
	{
		msg_print("���̉��ǂ̓O���[�u�ނɑ΂��Ă����s���Ȃ��B");
		return TRUE;
	}
	if(mode != 10 && !object_has_a_blade(o_ptr) && (es_ptr->add == TR_VORPAL || es_ptr->add == TR_EX_VORPAL ))
	{
		msg_print("���̉��ǂ͐n�̂��镐��ɑ΂��Ă����s���Ȃ��B");
		return TRUE;
	}
	if(mode != 10 && object_has_a_blade(o_ptr) && es_ptr->add == TR_IMPACT)
	{
		msg_print("���̉��ǂ͐n�̂Ȃ�����ɑ΂��Ă����s���Ȃ��B");
		return TRUE;
	}
	if(mode != 10 && !object_is_melee_weapon(o_ptr) && (es_ptr->add == TR_VAMPIRIC || es_ptr->add == TR_IMPACT ))
	{
		msg_print("���̉��ǂ͋ߐڕ���ɑ΂��Ă����s���Ȃ��B");
		return TRUE;
	}

	if (mode != 10 && !object_is_melee_weapon(o_ptr) && (es_ptr->add == ESSENCE_WHIRL_WIND || es_ptr->add == ESSENCE_L_RANGE_ATTACK || es_ptr->add == ESSENCE_TIM_INC_DICE))
	{
		msg_print("���̉��ǂ͋ߐڕ���ɑ΂��Ă����s���Ȃ��B");
		return TRUE;
	}

	if (mode != 10 && o_ptr->tval != TV_BOW && o_ptr->tval != TV_CROSSBOW && es_ptr->add == ESSENCE_SS_ARROW)
	{
		msg_print("���̉��ǂ͋|���N���X�{�E�ɑ΂��Ă����s���Ȃ��B");
		return TRUE;
	}


	if((es_ptr->add == ESSENCE_TMP_RES_ACID || es_ptr->add == ESSENCE_TMP_RES_ELEC || es_ptr->add == ESSENCE_TMP_RES_FIRE || es_ptr->add == ESSENCE_TMP_RES_COLD 
		|| es_ptr->add >= ESSENCE_TMP_RES_ALL && es_ptr->add <= ESSENCE_DUAL_CAST) //�ʓ|���������ď��������ȗ������B��������G�b�Z���X�t�^���ʒǉ������Ƃ�����
		&& o_ptr->tval == TV_GUN)
	{
		msg_print("�e�̔������ʂ�ς��邱�Ƃ͂ł��Ȃ��B");
		return TRUE;
	}


	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	use_essence = es_ptr->value;
	if ((o_ptr->tval >= TV_BULLET) && (o_ptr->tval <= TV_BOLT)) use_essence = (use_essence+9)/10;
	if (o_ptr->number > 1)
	{
		use_essence *= o_ptr->number;
#ifdef JP
		msg_format("%d����̂ŃG�b�Z���X��%d�K�v�ł��B", o_ptr->number, use_essence);
#else
		msg_format("It will take %d essences.",use_essence);
#endif

	}

	if (es_ptr->essence != -1)
	{
		if (p_ptr->magic_num1[es_ptr->essence] < use_essence)
		{
#ifdef JP
			msg_print("�G�b�Z���X������Ȃ��B");
#else
			msg_print("You don't have enough essences.");
#endif
			return TRUE;
		}
		if (is_pval_flag(es_ptr->add))
		{
			if (o_ptr->pval < 0)
			{
#ifdef JP
				msg_print("���̃A�C�e���̔\�͏C�����������邱�Ƃ͂ł��Ȃ��B");
#else
				msg_print("You cannot increase magic number of this item.");
#endif
				return TRUE;
			}
			else if (es_ptr->add == TR_BLOWS)
			{
				if (o_ptr->pval > 1)
				{
#ifdef JP
					if (!get_check("�C���l��1�ɂȂ�܂��B��낵���ł����H")) return TRUE;
#else
					if (!get_check("The magic number of this weapon will become 1. Are you sure? ")) return;
#endif
				}

				o_ptr->pval = 1;
#ifdef JP
				msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
				msg_format("It will take %d essences.", use_essence);
#endif
			}
			///mod141009
			else if (o_ptr->pval > 5)
			{
				int pval_new;
				if (!get_check("5�𒴂����C���l�͌������܂��B��낵���ł����H")) return TRUE;

				pval_new = 5 + ( o_ptr->pval - 5 + randint0(3)) / 3;
				use_essence *= pval_new;
				if (p_ptr->magic_num1[es_ptr->essence] >= use_essence) o_ptr->pval = pval_new; //�G�b�Z���X������Ȃ�����pval��M��Ȃ�

#ifdef JP
				msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
				msg_format("It will take %d essences.", use_essence);
#endif
			}
			else if (o_ptr->pval > 0)
			{
				use_essence *= o_ptr->pval;
#ifdef JP
				msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
				msg_format("It will take %d essences.", use_essence);
#endif
			}
			else
			{
				char tmp[80];
				char tmp_val[160];
				int pval;
				int limit = MIN(5, p_ptr->magic_num1[es_ptr->essence]/es_ptr->value);

#ifdef JP
				sprintf(tmp, "�����t�����܂����H (1-%d): ", limit);
#else
				sprintf(tmp, "Enchant how many? (1-%d): ", limit);
#endif
				strcpy(tmp_val, "1");

				if (!get_string(tmp, tmp_val, 1)) return TRUE;
				pval = atoi(tmp_val);
				if (pval > limit) pval = limit;
				else if (pval < 1) pval = 1;
				o_ptr->pval += pval;
				use_essence *= pval;
#ifdef JP
				msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
				msg_format("It will take %d essences.", use_essence);
#endif
			}

			if (p_ptr->magic_num1[es_ptr->essence] < use_essence)
			{
#ifdef JP
				msg_print("�G�b�Z���X������Ȃ��B");
#else
				msg_print("You don't have enough essences.");
#endif
				return TRUE;
			}
		}
		else if (es_ptr->add == ESSENCE_SLAY_GLOVE)
		{
			char tmp_val[160];
			int val;
			int get_to_h, get_to_d;

			strcpy(tmp_val, "1");
#ifdef JP
			if (!get_string(format("�����t�����܂����H (1-%d):", p_ptr->lev/7+3), tmp_val, 2)) return TRUE;
#else
			if (!get_string(format("Enchant how many? (1-%d):", p_ptr->lev/7+3), tmp_val, 2)) return;
#endif
			val = atoi(tmp_val);
			if (val > p_ptr->lev/7+3) val = p_ptr->lev/7+3;
			else if (val < 1) val = 1;
			use_essence *= val;
#ifdef JP
			msg_format("�G�b�Z���X��%d�g�p���܂��B", use_essence);
#else
			msg_format("It will take %d essences.", use_essence);
#endif
			if (p_ptr->magic_num1[es_ptr->essence] < use_essence)
			{
#ifdef JP
				msg_print("�G�b�Z���X������Ȃ��B");
#else
				msg_print("You don't have enough essences.");
#endif
				return TRUE;
			}
			get_to_h = ((val+1)/2+randint0(val/2+1));
			get_to_d = ((val+1)/2+randint0(val/2+1));
			o_ptr->xtra4 = (get_to_h<<8)+get_to_d;
			o_ptr->to_h += get_to_h;
			o_ptr->to_d += get_to_d;
		}
		p_ptr->magic_num1[es_ptr->essence] -= use_essence;
		if (es_ptr->add == ESSENCE_ATTACK)
		{
			if ((o_ptr->to_h >= max_add_ac_hit_dam) && (o_ptr->to_d >= max_add_ac_hit_dam))
			{
#ifdef JP
				msg_print("���ǂɎ��s�����B");
#else
				msg_print("You failed to enchant.");
#endif
				//energy_use = 100;
				return TRUE;
			}
			else
			{
				if (o_ptr->to_h < max_add_ac_hit_dam) o_ptr->to_h++;
				if (o_ptr->to_d < max_add_ac_hit_dam) o_ptr->to_d++;
			}
		}
		else if (es_ptr->add == ESSENCE_AC)
		{
			if (o_ptr->to_a >= max_add_ac_hit_dam)
			{
#ifdef JP
				msg_print("���ǂɎ��s�����B");
#else
				msg_print("You failed to enchant.");
#endif
				//energy_use = 100;
				return TRUE;
			}
			else
			{
				if (o_ptr->to_a < max_add_ac_hit_dam) o_ptr->to_a++;
			}
		}
		else
		{
			o_ptr->xtra3 = es_ptr->add + 1;
		}
	}
	else
	{
		bool success = TRUE;

		switch(es_ptr->add)
		{
		case ESSENCE_SH_FIRE:
			if ((p_ptr->magic_num1[TR_BRAND_FIRE] < use_essence) || (p_ptr->magic_num1[TR_RES_FIRE] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_BRAND_FIRE] -= use_essence;
			p_ptr->magic_num1[TR_RES_FIRE] -= use_essence;
			break;
		case ESSENCE_SH_ELEC:
			if ((p_ptr->magic_num1[TR_BRAND_ELEC] < use_essence) || (p_ptr->magic_num1[TR_RES_ELEC] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_BRAND_ELEC] -= use_essence;
			p_ptr->magic_num1[TR_RES_ELEC] -= use_essence;
			break;
		case ESSENCE_SH_COLD:
			if ((p_ptr->magic_num1[TR_BRAND_COLD] < use_essence) || (p_ptr->magic_num1[TR_RES_COLD] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_BRAND_COLD] -= use_essence;
			p_ptr->magic_num1[TR_RES_COLD] -= use_essence;
			break;
		case ESSENCE_RESISTANCE:
		case ESSENCE_SUSTAIN:
			if ((p_ptr->magic_num1[TR_RES_ACID] < use_essence) || (p_ptr->magic_num1[TR_RES_ELEC] < use_essence) || (p_ptr->magic_num1[TR_RES_FIRE] < use_essence) || (p_ptr->magic_num1[TR_RES_COLD] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_RES_ACID] -= use_essence;
			p_ptr->magic_num1[TR_RES_ELEC] -= use_essence;
			p_ptr->magic_num1[TR_RES_FIRE] -= use_essence;
			p_ptr->magic_num1[TR_RES_COLD] -= use_essence;
			break;

		case ESSENCE_TMP_RES_ALL:
			if ((p_ptr->magic_num1[TR_RES_ACID] < use_essence)
				|| (p_ptr->magic_num1[TR_RES_ELEC] < use_essence)
				|| (p_ptr->magic_num1[TR_RES_FIRE] < use_essence)
				|| (p_ptr->magic_num1[TR_RES_COLD] < use_essence)
				|| (p_ptr->magic_num1[TR_RES_POIS] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_RES_ACID] -= use_essence;
			p_ptr->magic_num1[TR_RES_ELEC] -= use_essence;
			p_ptr->magic_num1[TR_RES_FIRE] -= use_essence;
			p_ptr->magic_num1[TR_RES_COLD] -= use_essence;
			p_ptr->magic_num1[TR_RES_POIS] -= use_essence;
			break;

		case ESSENCE_WHIRL_WIND:
			if ((p_ptr->magic_num1[TR_VORPAL] < use_essence)
				|| (p_ptr->magic_num1[TR_BLOWS] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_VORPAL] -= use_essence;
			p_ptr->magic_num1[TR_BLOWS] -= use_essence;
			break;
		case ESSENCE_PSY_SPEAR:
			if ((p_ptr->magic_num1[TR_FORCE_WEAPON] < use_essence)
				|| (p_ptr->magic_num1[TR_LITE] < use_essence)
				|| (p_ptr->magic_num1[TR_THROW] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_FORCE_WEAPON] -= use_essence;
			p_ptr->magic_num1[TR_LITE] -= use_essence;
			p_ptr->magic_num1[TR_THROW] -= use_essence;
			break;
		case ESSENCE_L_RANGE_ATTACK:
			if ((p_ptr->magic_num1[TR_XTRA_MIGHT] < use_essence)
				|| (p_ptr->magic_num1[TR_BLOWS] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_XTRA_MIGHT] -= use_essence;
			p_ptr->magic_num1[TR_BLOWS] -= use_essence;
			break;
		case ESSENCE_HIT_AND_AWAY:
			if ((p_ptr->magic_num1[TR_SPEEDSTER] < use_essence)
				|| (p_ptr->magic_num1[TR_TELEPORT] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_SPEEDSTER] -= use_essence;
			p_ptr->magic_num1[TR_TELEPORT] -= use_essence;
			break;
		case ESSENCE_HEAL_500:
			if ((p_ptr->magic_num1[TR_REGEN] < use_essence)
				|| (p_ptr->magic_num1[TR_BLESSED] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_REGEN] -= use_essence;
			p_ptr->magic_num1[TR_BLESSED] -= use_essence;
			break;
		case ESSENCE_INVULN:
			if ((p_ptr->magic_num1[TR_REFLECT] < use_essence)
				|| (p_ptr->magic_num1[TR_IM_ACID] < use_essence)
				|| (p_ptr->magic_num1[TR_BLESSED] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_REFLECT] -= use_essence;
			p_ptr->magic_num1[TR_IM_ACID] -= use_essence;
			p_ptr->magic_num1[TR_BLESSED] -= use_essence;
			break;
		case ESSENCE_SS_ARROW:
			if ((p_ptr->magic_num1[TR_XTRA_MIGHT] < use_essence)
				|| (p_ptr->magic_num1[TR_XTRA_SHOTS] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_XTRA_MIGHT] -= use_essence;
			p_ptr->magic_num1[TR_XTRA_SHOTS] -= use_essence;
			break;
		case ESSENCE_DIM_DOOR:
			if ((p_ptr->magic_num1[TR_TELEPORT] < use_essence)
				|| (p_ptr->magic_num1[TR_NO_TELE] < use_essence)
)
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_TELEPORT] -= use_essence;
			p_ptr->magic_num1[TR_NO_TELE] -= use_essence;
			break;
		case ESSENCE_DUAL_CAST:
			if ((p_ptr->magic_num1[TR_MAGIC_MASTERY] < use_essence)
				|| (p_ptr->magic_num1[TR_EASY_SPELL] < use_essence)
				|| (p_ptr->magic_num1[TR_SPEED] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_MAGIC_MASTERY] -= use_essence;
			p_ptr->magic_num1[TR_EASY_SPELL] -= use_essence;
			p_ptr->magic_num1[TR_SPEED] -= use_essence;
			break;
		case ESSENCE_TIM_INC_DICE:
			if ((p_ptr->magic_num1[TR_FORCE_WEAPON] < use_essence)
				|| (p_ptr->magic_num1[TR_VORPAL] < use_essence)
				|| (p_ptr->magic_num1[TR_IMPACT] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[TR_FORCE_WEAPON] -= use_essence;
			p_ptr->magic_num1[TR_VORPAL] -= use_essence;
			p_ptr->magic_num1[TR_IMPACT] -= use_essence;
			break;



		}
		if (!success)
		{
#ifdef JP
			msg_print("�G�b�Z���X������Ȃ��B");
#else
			msg_print("You don't have enough essences.");
#endif
			return TRUE;
		}
		if (es_ptr->add == ESSENCE_SUSTAIN)
		{
			add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
			add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
			add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
			add_flag(o_ptr->art_flags, TR_IGNORE_COLD);
		}
		else
		{
			o_ptr->xtra3 = es_ptr->add + 1;
		}
	}

//	energy_use = 100;

#ifdef JP
	msg_format("%s��%s�̔\�͂�t�����܂����B", o_name, es_ptr->add_name);
#else
	msg_format("You have added ability of %s to %s.", es_ptr->add_name, o_name);
#endif

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	return TRUE;
}


/*:::�G�b�Z���X����*/
static void erase_essence(void)
{
	int item;
	cptr q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	u32b flgs[TR_FLAG_SIZE];

	//�b��A�C�e��
	item_tester_hook = object_is_smith;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���̃G�b�Z���X���������܂����H";
	s = "�G�b�Z���X��t�������A�C�e��������܂���B";
#else
	q = "Remove from which item? ";
	s = "You have nothing to remove essence.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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

	if(o_ptr->xtra3 == SPECIAL_ESSENCE_ONI) 
	{
		msg_print("���̕��킩��G�b�Z���X���������邱�Ƃ͂ł��Ȃ��B");
		return;
	}

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
	if (!get_check(format("��낵���ł����H [%s]", o_name))) return;
#else
	if (!get_check(format("Are you sure? [%s]", o_name))) return;
#endif

	//energy_use = 100;

	if (o_ptr->xtra3 == 1+ESSENCE_SLAY_GLOVE)
	{
		o_ptr->to_h -= (o_ptr->xtra4>>8);
		o_ptr->to_d -= (o_ptr->xtra4 & 0x000f);
		o_ptr->xtra4 = 0;
		if (o_ptr->to_h < 0) o_ptr->to_h = 0;
		if (o_ptr->to_d < 0) o_ptr->to_d = 0;
	}

	//v1.1.74 �����@�\�̒ǉ������Ă����ꍇ�[�U���Z�b�g
	//���������w��̎d���͍���̒ǉ��̂Ƃ����������Y��₷���ėǂ��Ȃ��̂�������ȏ㔭���ǉ��Ƃ����Ȃ����낤����
	if ((o_ptr->xtra3 - 1) >= ESSENCE_TMP_RES_ALL && (o_ptr->xtra3 - 1) <= ESSENCE_TIM_INC_DICE)
		o_ptr->timeout = 0;


	o_ptr->xtra3 = 0;


	///mod150311 -Hack- �ߋ��ɕ���C�����ꂽ����̂Ƃ��A�����������Ă����t���O��߂�
	///mod160511 �e�̂Ƃ�xtra1�͕ʂ̂��ƂɎg���Ă邪�C���͂���Ȃ��̂ł���ɗ�O�����ǉ��@���񂾂�X�p�Q�b�e�B�[�ɂȂ��Ă��Ă���
	if(o_ptr->xtra1 && o_ptr->tval != TV_GUN) o_ptr->xtra3 = SPECIAL_ESSENCE_ONI;

	//����C����pval�������Ƃ�������ƍ���̂ł��̏����폜
	//object_flags(o_ptr, flgs);
	/*:::���Xpval�̂Ȃ��A�C�e���������ꍇpval��0�ɂ��Ă���H*/
	//if (!(have_pval_flags(flgs))) o_ptr->pval = 0;
#ifdef JP
	msg_print("�G�b�Z���X����苎�����B");
#else
	msg_print("You removed all essence you have added.");
#endif

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);
}

/*:::�b��t�̋Z�\*/
void do_cmd_kaji(bool only_browse)
{
	int mode = 0;
	char choice;
	int menu_line = (use_menu ? 1 : 0);

	if(p_ptr->pclass != CLASS_SH_DEALER && p_ptr->pclass != CLASS_JEWELER && p_ptr->pclass != CLASS_KEIKI && p_ptr->pclass != CLASS_MISUMARU)
	{
			msg_print("���Ȃ��ɂ͍H�[�łł��邱�Ƃ͂Ȃ��B");
			return;
	}

	if (!only_browse)
	{
		if (p_ptr->confused)
		{
#ifdef JP
			msg_print("�������Ă��č�Ƃł��Ȃ��I");
#else
			msg_print("You are too confused!");
#endif

			return;
		}
		if (p_ptr->blind)
		{
#ifdef JP
			msg_print("�ڂ������Ȃ��č�Ƃł��Ȃ��I");
#else
			msg_print("You are blind!");
#endif

			return;
		}
		if (p_ptr->image)
		{
#ifdef JP
			msg_print("���܂������Ȃ��č�Ƃł��Ȃ��I");
#else
			msg_print("You are hallucinating!");
#endif

			return;
		}
	}

#ifdef ALLOW_REPEAT
	if (!(repeat_pull(&mode) && 1 <= mode && mode <= 5))
	{
#endif /* ALLOW_REPEAT */

	if (only_browse) screen_save();
	do {
	if (!only_browse) screen_save();
	if (use_menu)
	{
		/*:::mode�͏����l0�A���ꂼ��̍�Ƃ�1-5������U����*/
		while(!mode)
		{
#ifdef JP
			prt(format(" %s �G�b�Z���X�ꗗ", (menu_line == 1) ? "�t" : "  "), 2, 14);
			prt(format(" %s �G�b�Z���X���o", (menu_line == 2) ? "�t" : "  "), 3, 14);
			prt(format(" %s �G�b�Z���X����", (menu_line == 3) ? "�t" : "  "), 4, 14);
			prt(format(" %s �G�b�Z���X�t��", (menu_line == 4) ? "�t" : "  "), 5, 14);
			prt(format(" %s ����/�h���", (menu_line == 5) ? "�t" : "  "), 6, 14);
			prt(format("�ǂ̎�ނ̋Z�p��%s�܂����H", only_browse ? "����" : "�g��"), 0, 0);
#else
			prt(format(" %s List essences", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s Extract essence", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s Remove essence", (menu_line == 3) ? "> " : "  "), 4, 14);
			prt(format(" %s Add essence", (menu_line == 4) ? "> " : "  "), 5, 14);
			prt(format(" %s Enchant weapon/armor", (menu_line == 5) ? "> " : "  "), 6, 14);
			prt(format("Choose command from menu."), 0, 0);
#endif
			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 4;
				break;
			case '\r':
			case '\n':
			case 'x':
			case 'X':
				mode = menu_line;
				break;
			}
			if (menu_line > 5) menu_line -= 5;
		}
	}

	else
	{
		while (!mode)
		{
#ifdef JP
			prt("  a) �G�b�Z���X�ꗗ", 2, 14);
			prt("  b) �G�b�Z���X���o", 3, 14);
			prt("  c) �G�b�Z���X����", 4, 14);
			prt("  d) �G�b�Z���X�t��", 5, 14);
			prt("  e) ����/�h���", 6, 14);
			if (!get_com(format("�ǂ̔\�͂�%s�܂���:", only_browse ? "����" : "�g��"), &choice, TRUE))
#else
			prt("  a) List essences", 2, 14);
			prt("  b) Extract essence", 3, 14);
			prt("  c) Remove essence", 4, 14);
			prt("  d) Add essence", 5, 14);
			prt("  e) Enchant weapon/armor", 6, 14);
			if (!get_com("Command :", &choice, TRUE))
#endif
			{
				/*:::�I����ESC�Ƃ��������炱���ŏo��*/
				screen_load();
				return;
			}
			switch (choice)
			{
			case 'A':
			case 'a':
				mode = 1;
				break;
			case 'B':
			case 'b':
				mode = 2;
				break;
			case 'C':
			case 'c':
				mode = 3;
				break;
			case 'D':
			case 'd':
				mode = 4;
				break;
			case 'E':
			case 'e':
				mode = 5;
				break;
			}
		}
	}
	/*:::b�R�}���h�̂Ƃ��������\��*/
	if (only_browse)
	{
		char temp[62*5];
		int line, j;

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(14, 21, 255);
		Term_erase(14, 20, 255);
		Term_erase(14, 19, 255);
		Term_erase(14, 18, 255);
		Term_erase(14, 17, 255);
		Term_erase(14, 16, 255);

		if(p_ptr->pclass == CLASS_SH_DEALER) roff_to_buf(kaji_tips_sh[mode-1], 62, temp, sizeof(temp));
		else roff_to_buf(kaji_tips_jewel[mode-1], 62, temp, sizeof(temp));

		for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
		mode = 0;
	}
	if (!only_browse) screen_load();
	} while (only_browse);
#ifdef ALLOW_REPEAT
	repeat_push(mode);
	}
#endif /* ALLOW_REPEAT */

	switch(mode)
	{
		case 1: display_essence();break;
		case 2: drain_essence();break;
		case 3: erase_essence();break;
		case 4:
			while(1) //�G�b�Z���X�I���ڍ׉�ʂ�ESC��������G�b�Z���X�I�𕪗މ�ʂ܂Ŗ߂�悤�ɂ���
			{
				mode = choose_essence();
				if (mode == 0)	break;
				if(add_essence(mode)) break;
			}
			break;
		case 5: add_essence(10);break;
	}
}

/*
 * Torches have special abilities when they are flaming.
 */
/*:::�����܂𓊂����Ƃ��̂��߂̍U���t���O*/
void torch_flags(object_type *o_ptr, u32b *flgs)
{
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
	{
		if (o_ptr->xtra4 > 0)
		{
			add_flag(flgs, TR_BRAND_FIRE);
			add_flag(flgs, TR_KILL_UNDEAD);
			add_flag(flgs, TR_THROW);
		}
		else if(object_is_artifact(o_ptr))
		{

			add_flag(flgs, TR_BRAND_FIRE);
			add_flag(flgs, TR_SLAY_GOOD);
			add_flag(flgs, TR_KILL_HUMAN);
			add_flag(flgs, TR_THROW);

		}
	}
}
/*:::�����܂𓊂����Ƃ��̃_�C�X�ݒ�*/
void torch_dice(object_type *o_ptr, int *dd, int *ds)
{
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
	{
		if (o_ptr->xtra4 > 0)
		{
			(*dd) = 1;
			(*ds) = 6;
		}
	}
}

void torch_lost_fuel(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH) && !object_is_artifact(o_ptr))
	{
		o_ptr->xtra4 -= (FUEL_TORCH / 25);
		if (o_ptr->xtra4 < 0) o_ptr->xtra4 = 0;
	}
}

/*:::Mega Hack - ���P�̖{�̂̎P�̃p�����[�^��ݒ肷��*/
void apply_kogasa_magic(object_type *o_ptr, int lev, bool birth)
{

	if(o_ptr->tval != TV_STICK || o_ptr->sval != SV_WEAPON_KOGASA)
	{
		msg_format("ERROR:���P�̗�����ɎP�ȊO����������Ă�H");
		return;
	}

	o_ptr->number = 1;
	o_ptr->ac = 0;
	o_ptr->to_a = p_ptr->lev / 3 + 4;
	o_ptr->dd = p_ptr->lev / 12 + 1;
	o_ptr->ds = p_ptr->lev / 8 + 1;
	o_ptr->to_h = p_ptr->lev / 3 + 4;
	o_ptr->to_d = p_ptr->lev / 3 + 4;
	o_ptr->weight = 30;
	o_ptr->pval = lev / 10;
	add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(o_ptr->art_flags, TR_IGNORE_COLD);
	o_ptr->ident |= (IDENT_MENTAL);
	
	if(birth)
	{
		o_ptr->art_name = quark_add("���X�Ǐ�");
		return;
	}

	if(lev > 9) add_flag(o_ptr->art_flags, TR_CHR);
	if(lev > 19) add_flag(o_ptr->art_flags, TR_ESP_HUMAN);
	if(lev > 29) add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
	if(lev > 29) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
	if(lev > 39) add_flag(o_ptr->art_flags, TR_RES_DISEN);

	switch(p_ptr->pseikaku)
	{
	case SEIKAKU_CHIKARA:
		if(lev > 9) add_flag(o_ptr->art_flags, TR_STR);
		if(lev >14) add_flag(o_ptr->art_flags, TR_FREE_ACT);
		if(lev >24) add_flag(o_ptr->art_flags, TR_CON);
		if(lev >34) add_flag(o_ptr->art_flags, TR_SUST_STR);
		if(lev >34) add_flag(o_ptr->art_flags, TR_SUST_CON);
		o_ptr->dd+=1;
		o_ptr->to_d += p_ptr->lev / 5;
		break;
	case SEIKAKU_KIREMONO:
		if(lev > 9) add_flag(o_ptr->art_flags, TR_INT);
		if(lev >19) add_flag(o_ptr->art_flags, TR_RES_CONF);
		if(lev >29) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		if(lev >34) add_flag(o_ptr->art_flags, TR_TELEPATHY);
		if(lev >39) add_flag(o_ptr->art_flags, TR_WIS);
		if(lev >44) add_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
		o_ptr->to_h += p_ptr->lev / 10;
		break;
	case SEIKAKU_SHIAWASE:
		if(lev > 9) add_flag(o_ptr->art_flags, TR_WIS);
		if(lev >14) add_flag(o_ptr->art_flags, TR_BLESSED);
		if(lev >19) add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
		if(lev >24) add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		if(lev >29) add_flag(o_ptr->art_flags, TR_CON);
		if(lev >39) add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		if(lev >44) add_flag(o_ptr->art_flags, TR_RES_HOLY);
		break;
	case SEIKAKU_SUBASI:
		if(lev > 9) add_flag(o_ptr->art_flags, TR_DEX);
		if(lev >14) add_flag(o_ptr->art_flags, TR_FREE_ACT);
		if(lev >24) add_flag(o_ptr->art_flags, TR_SPEEDSTER);
		if(lev >29) add_flag(o_ptr->art_flags, TR_BLOWS);
		if(lev >34) add_flag(o_ptr->art_flags, TR_SPEED);
		if(lev >39) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		if(lev > 9) o_ptr->pval = 1 + lev / 30;
		o_ptr->to_h += p_ptr->lev / 10;
		break;
	case SEIKAKU_INOCHI:
		if(lev > 4) add_flag(o_ptr->art_flags, TR_RES_FEAR);
		if(lev > 9) add_flag(o_ptr->art_flags, TR_BLOWS);
		if(lev >14) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		if(lev >19) add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
		if(lev >24) add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
		if(lev >29) add_flag(o_ptr->art_flags, TR_RES_ACID);
		if(lev >29) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		if(lev >34) add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		if(lev >39) add_flag(o_ptr->art_flags, TR_SLAY_GOOD);
		if(lev >44) add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		if(lev > 9) o_ptr->pval = 1 + lev / 20;
		o_ptr->to_d += p_ptr->lev / 5;
		o_ptr->to_h -= p_ptr->lev / 5;
		o_ptr->to_a = 4 - lev+1;
		break;
	case SEIKAKU_TRIGGER:
		if(lev > 1) add_flag(o_ptr->art_flags, TR_ACTIVATE);
		if(lev > 1) o_ptr->xtra2 = ACT_BO_MISS_1;
		if(lev > 9) o_ptr->xtra2 = ACT_KOGASA_LASER;
		if(lev > 19) o_ptr->xtra2 = ACT_KOGASA_ROCKET;
		o_ptr->to_a = 4 + lev / 30;
		o_ptr->timeout = 0;

		break;
	case SEIKAKU_SEXY:
		if(lev > 9) add_flag(o_ptr->art_flags, TR_SPEED);
		if(lev >14) add_flag(o_ptr->art_flags, TR_INT);
		if(lev >19) add_flag(o_ptr->art_flags, TR_DEX);
		if(lev >24) add_flag(o_ptr->art_flags, TR_WIS);
		if(lev >29) add_flag(o_ptr->art_flags, TR_STR);
		if(lev >34) add_flag(o_ptr->art_flags, TR_CON);
		if(lev >39) add_flag(o_ptr->art_flags, TR_RES_ACID);
		if(lev >39) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		if(lev >44) add_flag(o_ptr->art_flags, TR_SPEEDSTER);
		o_ptr->to_d += p_ptr->lev / 10;
		o_ptr->to_h += p_ptr->lev / 10;
		break;
	case SEIKAKU_LUCKY:
		if(lev > 9) add_flag(o_ptr->art_flags, TR_STEALTH);
		if(lev >14) add_flag(o_ptr->art_flags, TR_WARNING);
		if(lev >19) add_flag(o_ptr->art_flags, TR_SEARCH);
		if(lev >24) add_flag(o_ptr->art_flags, TR_VAMPIRIC);
		if(lev >34) add_flag(o_ptr->art_flags, TR_ESP_UNIQUE);
		if(lev >44) add_flag(o_ptr->art_flags, TR_ACTIVATE);
		if(lev >44) o_ptr->xtra2 = ACT_INVULN;
		o_ptr->timeout = 0;
		break;
	case SEIKAKU_GAMAN:
		if(lev > 4) add_flag(o_ptr->art_flags, TR_FREE_ACT);
		if(lev > 9) add_flag(o_ptr->art_flags, TR_CON);
		if(lev >14) add_flag(o_ptr->art_flags, TR_RES_COLD);
		if(lev >19) add_flag(o_ptr->art_flags, TR_WIS);
		if(lev >24) add_flag(o_ptr->art_flags, TR_RES_ELEC);
		if(lev >29) add_flag(o_ptr->art_flags, TR_RES_ACID);
		if(lev >34) add_flag(o_ptr->art_flags, TR_RES_FIRE);
		if(lev >39) add_flag(o_ptr->art_flags, TR_ACTIVATE);
		if(lev >39) o_ptr->xtra2 = ACT_RESIST_ALL;
		o_ptr->timeout = 0;
		o_ptr->to_a += lev / 5;
		break;

	case SEIKAKU_ELEGANT:
		add_flag(o_ptr->art_flags, TR_SUST_CHR);
		if(lev > 4) add_flag(o_ptr->art_flags, TR_CHR);
		if(lev > 4) o_ptr->pval = 1 + lev / 15;
		if(lev > 9) add_flag(o_ptr->art_flags, TR_RES_COLD);
		if(lev >14) add_flag(o_ptr->art_flags, TR_DEX);
		if(lev >19) add_flag(o_ptr->art_flags, TR_FREE_ACT);
		if(lev >24) add_flag(o_ptr->art_flags, TR_BRAND_COLD);
		if(lev >29) add_flag(o_ptr->art_flags, TR_RES_FEAR);
		if(lev >34) add_flag(o_ptr->art_flags, TR_VORPAL);
		if(lev >39) add_flag(o_ptr->art_flags, TR_SPEED);
		if(lev >44) add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		o_ptr->to_h += p_ptr->lev / 10;
		break;


	case SEIKAKU_BERSERK:
		if(lev > 4) o_ptr->pval = 1 + lev / 20;
		if(lev > 4) add_flag(o_ptr->art_flags, TR_TUNNEL);
		if(lev > 4) add_flag(o_ptr->art_flags, TR_STR);
		if(lev > 4) add_flag(o_ptr->art_flags, TR_DEX);
		if(lev > 4) add_flag(o_ptr->art_flags, TR_SPEED);
		if(lev > 9) add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		if(lev >14) add_flag(o_ptr->art_flags, TR_BLOWS);
		if(lev >19) add_flag(o_ptr->art_flags, TR_CON);
		if(lev >19) add_flag(o_ptr->art_flags, TR_IMPACT);
		if(lev >24) add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
		if(lev >24) add_flag(o_ptr->art_flags, TR_CHAOTIC);
		if(lev >24) add_flag(o_ptr->art_flags, TR_RES_CHAOS);
		if(lev >29) add_flag(o_ptr->art_flags, TR_RES_ACID);
		if(lev >29) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		if(lev >34) add_flag(o_ptr->art_flags, TR_REFLECT);
		if(lev >34) add_flag(o_ptr->art_flags, TR_SLAY_GOOD);
		if(lev >39) add_flag(o_ptr->art_flags, TR_AGGRAVATE);
		if(lev >39) add_flag(o_ptr->art_flags, TR_VAMPIRIC);
		if(lev >44) add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		if(lev >44) add_flag(o_ptr->art_flags, TR_TY_CURSE);
		if(lev >49) add_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
		o_ptr->dd+= lev / 24;
		o_ptr->to_d += p_ptr->lev / 5;
		break;


		break;

	default:
		if(lev > 9) add_flag(o_ptr->art_flags, TR_FREE_ACT);
		if(lev >19) add_flag(o_ptr->art_flags, TR_DEX);
		if(lev >29) add_flag(o_ptr->art_flags, TR_CON);
		if(lev >34) add_flag(o_ptr->art_flags, TR_RES_ACID);
		if(lev >44) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		break;
	}
	return;
}


//v1.1.57
//�B��ޓ��Z�u�����̌��v�@�ꎞ�I�Ȍ��𐶐����ė����r�Ɏ���
//���s�����Ƃ���FALSE��Ԃ�
//���̏��ŏ�����process_world()���̓��ꏈ���ōs��
//v2.0.18 mode�l��ǉ��B0���B��ނ�1���t����
bool generate_seven_star_sword(int mode)
{
	int plev = p_ptr->lev;
	object_type forge;
	object_type *o_ptr = &forge;

	if (p_ptr->pclass == CLASS_ALICE)
	{
		msg_print("���Ȃ��͎��畐���������͂Ȃ��B");
		return FALSE;
	}
	if (p_ptr->pclass == CLASS_ENOKO)
	{
		msg_print("�g���o�T�~���ז��ŕ�������ĂȂ��B");
		return FALSE;
	}

	if ((check_invalidate_inventory(INVEN_RARM) || check_invalidate_inventory(INVEN_LARM))
		|| (inventory[INVEN_RARM].k_idx && inventory[INVEN_LARM].k_idx)
		|| object_is_cursed(&inventory[INVEN_RARM]))
	{
		msg_print("���͐V���ȕ�������ĂȂ��B");
		return FALSE;
	}
	//�A�C�e������
	if (mode == 0)
	{
		msg_print("���X�̗͂����Ȃ��̎�̒��Ō��̌`��������I");
		object_prep(o_ptr, lookup_kind(TV_SWORD, SV_WEAPON_SEVEN_STAR));
		//�p�����[�^�ݒ�
		o_ptr->dd = 7;
		o_ptr->ds = 7;
		o_ptr->to_d = plev / 3;
		o_ptr->to_h = plev / 3;
		//�X���C�t�^
		add_flag(o_ptr->art_flags, TR_KILL_DEMON);
		add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
		add_flag(o_ptr->art_flags, TR_KILL_KWAI);
		add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		add_flag(o_ptr->art_flags, TR_LITE);
		//�����X�^�[�o�[�X�g
		add_flag(o_ptr->art_flags, TR_ACTIVATE);
		o_ptr->xtra2 = ACT_BA_LITE;

	}
	else
	{
		msg_print("���Ȃ��͖��͂��Ïk���Č����`������I");
		object_prep(o_ptr, lookup_kind(TV_SWORD, SV_WEAPON_MAGIC_SWORD));
		//�p�����[�^�ݒ�
		o_ptr->dd = 1 + plev / 7;
		o_ptr->ds = 7 + plev / 50;
		o_ptr->to_d = plev / 2;
		o_ptr->to_h = 0;

	}


	//�b��A�C�e�������ɂ���
	o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER; 

	//*�Ӓ�*�ςɂ���
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	//�󂢂Ă����ɑ�������
	if (!inventory[INVEN_RARM].k_idx)
	{
		object_copy(&inventory[INVEN_RARM], o_ptr);
	}
	else
	{
		object_copy(&inventory[INVEN_LARM], o_ptr);

		//�����E��Ɏ����Ă����̂��e��N���X�{�E�Ȃ獶�E����ւ���K�v������B�{���Ȃ炱�̕ӂ̏������܂Ƃ߂Đ؂�o����do_cmd_wield()�Ƃ܂Ƃ߂�ׂ��Ȃ񂾂낤
		if (object_is_shooting_weapon(&inventory[INVEN_RARM]) && !object_is_cursed(&inventory[INVEN_RARM]))
		{
			object_type object_tmp;
			object_type *otmp_ptr = &object_tmp;

			object_copy(otmp_ptr, &inventory[INVEN_RARM]);
			object_copy(&inventory[INVEN_RARM], &inventory[INVEN_LARM]);
			object_copy(&inventory[INVEN_LARM], otmp_ptr);

		}

	}
	p_ptr->total_weight += o_ptr->weight;


	p_ptr->update |= (PU_BONUS | PU_TORCH);
	p_ptr->redraw |= (PR_EQUIPPY | PR_HP | PR_MANA);
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
	handle_stuff();

	return TRUE;

}


