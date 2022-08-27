/* File: cmd2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 2) */

#include "angband.h"


/*:::�N�G�X�g�_���W��������N�G�X�g�������ŏo��ꍇ�Ɋm�F���鏈��*/
bool confirm_leave_level(bool down_stair)
{
	quest_type *q_ptr = &quest[p_ptr->inside_quest];

	//v1.1.24 �o�����B�N�G�X�g�p�̊��������ǉ�
	/* Confirm leaving from once only quest */
	if (confirm_quest && p_ptr->inside_quest &&(
		  q_ptr->type == QUEST_TYPE_RANDOM ||
	     (q_ptr->flags & QUEST_FLAG_ONCE &&	(q_ptr->status != QUEST_STATUS_COMPLETED)) ||
		 (q_ptr->flags & QUEST_FLAG_TOWER && ((q_ptr->status != QUEST_STATUS_STAGE_COMPLETED) ||
			 (down_stair && (quest[QUEST_TOWER1].status != QUEST_STATUS_COMPLETED)))) ||
		  q_ptr->type == QUEST_TYPE_FIND_EXIT
					 
						 ))
	{
#ifdef JP
		msg_print("���̊K����x����Ɠ�x�Ɩ߂��ė����܂���B");
		if (get_check("�{���ɂ��̊K������܂����H")) return TRUE;
#else
		msg_print("You can't come back here once you leave this floor.");
		if (get_check("Really leave this floor? ")) return TRUE;
#endif
	}
	else
	{
		///mod140113 �N�G�X�g�t���O��QUEST_RETRY��t������B
		/*:::����ɂ��Ē���\�ȃN�G�X�g�ɓ�x�ڈȍ~�ɓ������Ƃ��A�C�e���������Ă��Ȃ��悤�ɂ���*/
		if( p_ptr->inside_quest ) q_ptr->flags |= QUEST_FLAG_RETRY;
		if((q_ptr->status != QUEST_STATUS_STAGE_COMPLETED) && p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 40) set_deity_bias(DBIAS_REPUTATION, -1);
		return TRUE;
	}
	return FALSE;
}


/*
 * Go up one level
 */
/*:::�_���W�������ŊK�i���オ��@�r��͕ʉӏ��ŏ�������Ă���*/
void do_cmd_go_up(void)
{
	bool go_up = FALSE;

	/* Player grid */
	cave_type *c_ptr = &cave[py][px];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	int up_num = 0;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Verify stairs */
	if (!have_flag(f_ptr->flags, FF_LESS))
	{
#ifdef JP
		msg_print("�����ɂ͏��K�i����������Ȃ��B");
#else
		msg_print("I see no up staircase here.");
#endif

		return;
	}

	/* Quest up stairs */
	/*:::�N�G�X�g�t���O�����K�i�̏ꍇ?*/
	if (have_flag(f_ptr->flags, FF_QUEST))
	{
		/* Cancel the command */
		if (!confirm_leave_level(FALSE)) return;

	
		/* Success */
#ifdef JP
		///del131213 msg
		//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
		//	msg_print("�Ȃ񂾂��̊K�i�́I");
		//else
			msg_print("��̊K�ɓo�����B");
#else
		msg_print("You enter the up staircase.");
#endif

		leave_quest_check();
		/*:::���̂��߂̏����H*/
		p_ptr->inside_quest = c_ptr->special;

		/* Activate the quest */
		/*:::����N�G�X�g�͐V���ȊK�𐶐��H����p���H*/
		if (!quest[p_ptr->inside_quest].status)
		{
			if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM)
			{
				init_flags = INIT_ASSIGN;
				process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			}
			quest[p_ptr->inside_quest].status = QUEST_STATUS_TAKEN;
		}

		/* Leaving a quest */
		if (!p_ptr->inside_quest)
		{
			dun_level = 0;
		}

		/* Leaving */
		p_ptr->leaving = TRUE;

		p_ptr->oldpx = 0;
		p_ptr->oldpy = 0;
		
		/* Hack -- take a turn */
		energy_use = 100;

		/* End the command */
		return;
	}

	if (!dun_level)
	{
		go_up = TRUE;
	}
	else
	{
		go_up = confirm_leave_level(FALSE);
	}

	/* Cancel the command */
	if (!go_up) return;

	/* Hack -- take a turn */
	energy_use = 100;
	/*:::�I�v�V�����ɂ��I�[�g�Z�[�u*/
	if (autosave_l) do_cmd_save_game(TRUE);

	/* For a random quest */
	if (p_ptr->inside_quest &&
	    quest[p_ptr->inside_quest].type == QUEST_TYPE_RANDOM)
	{
		leave_quest_check();

		p_ptr->inside_quest = 0;
	}

	/* For a fixed quest */
	/*:::�����N�G�ȊO*/
	if (p_ptr->inside_quest &&
	    quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM)
	{

		leave_quest_check();

		p_ptr->inside_quest = c_ptr->special;
		dun_level = 0;
		up_num = 0;
	}

	/* For normal dungeon and random quest */
	else
	{
		/* New depth */
		if (have_flag(f_ptr->flags, FF_SHAFT))
		{
			/* Create a way back */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_SHAFT);

			up_num = 2;
		}
		else
		{
			/* Create a way back */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP);

			up_num = 1;
		}

		/* Get out from current dungeon */
		if (dun_level - up_num < d_info[dungeon_type].mindepth)
			up_num = dun_level;
	}

#ifdef JP
	if (record_stair) do_cmd_write_nikki(NIKKI_STAIR, 0-up_num, "�K�i�������");
#else
	if (record_stair) do_cmd_write_nikki(NIKKI_STAIR, 0-up_num, "climbed up the stairs to");
#endif

	/* Success */
#ifdef JP
	///del131213 msg
	//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
	//	msg_print("�Ȃ񂾂��̊K�i�́I");
	//else if (up_num == dun_level)
	if (up_num == dun_level)
		msg_print("�n��ɖ߂����B");
	else
		msg_print("�K�i������ĐV���Ȃ���{�ւƑ��𓥂ݓ��ꂽ�B");
#else
	if (up_num == dun_level)
		msg_print("You go back to the surface.");
	else
		msg_print("You enter a maze of up staircases.");
#endif

	/* Leaving */
	p_ptr->leaving = TRUE;
}



/*
* Return the number of features around (or under) the character.
* Usually look for doors and floor traps.
*/
/*:::���̎��ӂ̓���̕��̐��𐔂���B��������̂͊֐��|�C���^�ŏ�����n��*/
static int count_dt(int *y, int *x, bool(*test)(int feat), bool under)
{
	int d, count, xx, yy;

	/* Count how many matches */
	count = 0;

	/* Check around (and under) the character */
	for (d = 0; d < 9; d++)
	{
		cave_type *c_ptr;
		s16b feat;

		/* if not searching under player continue */
		if ((d == 8) && !under) continue;

		/* Extract adjacent (legal) location */
		yy = py + ddy_ddd[d];
		xx = px + ddx_ddd[d];

		/* Get the cave */
		c_ptr = &cave[yy][xx];

		/* Must have knowledge */
		if (!(c_ptr->info & (CAVE_MARK))) continue;

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Not looking for this feature */
		if (!((*test)(feat))) continue;

		/* OK */
		++count;

		/* Remember the location. Only useful if only one match */
		*y = yy;
		*x = xx;
	}

	/* All done */
	return count;
}


/*
* Determine if a grid contains a chest
*/
static s16b chest_check(int y, int x, bool trapped)
{
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;


	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Skip unknown chests XXX XXX */
		/* if (!(o_ptr->marked & OM_FOUND)) continue; */

		/* Check for non empty chest */
		if ((o_ptr->tval == TV_CHEST) &&
			(((!trapped) && (o_ptr->pval)) || /* non empty */
			((trapped) && (o_ptr->pval > 0)))) /* trapped only */
		{
			return (this_o_idx);
		}
	}

	/* No chest */
	return (0);
}

/*
* Return the number of chests around (or under) the character.
* If requested, count only trapped chests.
*/
/*:::�����ӂ̔��̂���O���b�h���𐔂���*/
/*:::trapped:�g���b�v�̂��锠�݂̂𐔂���*/
static int count_chests(int *y, int *x, bool trapped)
{
	int d, count, o_idx;

	object_type *o_ptr;

	/* Count how many matches */
	count = 0;

	/* Check around (and under) the character */
	for (d = 0; d < 9; d++)
	{
		/* Extract adjacent (legal) location */
		int yy = py + ddy_ddd[d];
		int xx = px + ddx_ddd[d];

		/* No (visible) chest is there */
		if ((o_idx = chest_check(yy, xx, FALSE)) == 0) continue;

		/* Grab the object */
		o_ptr = &o_list[o_idx];

		/* Already open */
		if (o_ptr->pval == 0) continue;

		/* No (known) traps here */
		if (trapped && (!object_is_known(o_ptr) ||
			!chest_traps[o_ptr->pval])) continue;

		/* OK */
		++count;

		/* Remember the location. Only useful if only one match */
		*y = yy;
		*x = xx;
	}

	/* All done */
	return count;
}


//v1.1.97 �킴��㩂𓥂� cmd_go_down()����̂ݎg�p
static void step_on_trap(void)
{
	int dir,x,y;
	s16b feat;
	cave_type *c_ptr;

	if (!get_check_strict("�߂���㩂��킴�Ɠ��݂܂����H", CHECK_DEFAULT_Y)) return;
	if (!get_rep_dir(&dir, TRUE)) return;

	/* Get requested location */
	y = py + ddy[dir];
	x = px + ddx[dir];

	/* Get requested grid */
	c_ptr = &cave[y][x];

	if (!is_known_trap(c_ptr))
	{
		msg_print("�����ɂ�㩂���������Ȃ��B");
		return;
	}

	//move_player()�ɐV�����t���O���������킴�ƃg���b�v����������
	move_player(dir, FALSE, FALSE,TRUE);

	//���ڎ��s����ꍇ���̓g���b�v�̏ꏊ�֓����Ȃ��̂ŗ��Ƃ��˂Ƃ��ɗ������Ȃ��B�C���[�W�I�ɂ��ړ����ׂ����B
	//activate_floor_trap(y, x, FALSE);

}

/*
 * Go down one level
 */
/*:::�K�i�������*/
//v1.1.97 㩂��킴�Ɠ��ދ@�\��ǉ�
void do_cmd_go_down(void)
{
	/* Player grid */
	cave_type *c_ptr = &cave[py][px];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	int trap_count = 0;

	bool fall_trap = FALSE;
	int down_num = 0;

	int ty, tx;

	//�����������͗אڂ������m�ς݂�㩂𐔂���
	trap_count = count_dt(&ty, &tx, is_trap, TRUE);


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Verify stairs */
	//v1.1.97 ���Ƃ��˂�step_on_trap()�ŏ�������̂ł����ŊK�i�łȂ��g���b�v�ƌ��Ȃ��悤�ɂ���
	if (!(have_flag(f_ptr->flags, FF_MORE) && !have_flag(f_ptr->flags, FF_TRAP)))
	{
#ifdef JP
		msg_print("�����ɂ͉���K�i����������Ȃ��B");
#else
		msg_print("I see no down staircase here.");
#endif
		//v1.1.97 �킴��㩂𓥂�
		if (trap_count) step_on_trap();

		return;

	}

	//v1.1.97 �������̃t���O�͗����Ȃ��͂����������̂��ʓ|�Ȃ̂ł��̂܂�
	if (have_flag(f_ptr->flags, FF_TRAP)) fall_trap = TRUE;

	/* Quest entrance */
	if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		do_cmd_quest();
	}
	/*::: Mega Hack - Extra���[�h�ŃI�x�����œ|��ɊK�i���~���ƃT�[�y���g�N�G�X�g����̂����ׂ̗̈�101�K��*/
	else if(EXTRA_MODE && dun_level == 100 && quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED)
	{
		int tmp = p_ptr->inside_quest;

		p_ptr->inside_quest = QUEST_SERPENT;
		init_flags |= INIT_ASSIGN;
		process_dungeon_file("q_info.txt", 0, 0, 0, 0);
		quest[QUEST_SERPENT].status = QUEST_STATUS_TAKEN;
		p_ptr->inside_quest = tmp;

		p_ptr->oldpx = px;
		p_ptr->oldpy = py;
		dungeon_type = DUNGEON_CHAOS;

		p_ptr->leaving = TRUE;
		prepare_change_floor_mode(CFM_FIRST_FLOOR | CFM_DOWN);
		if (autosave_l) do_cmd_save_game(TRUE);
		energy_use = 100;

		msg_print("���Ȃ��̓_���W�����̍X�Ȃ�[�w�ւƍ~��Ă������E�E");
	}

	/* Quest down stairs */
	else if (have_flag(f_ptr->flags, FF_QUEST))
	{
		/* Confirm Leaving */
		if(!confirm_leave_level(TRUE)) return;
		
#ifdef JP
		///del131213 msg
		//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
		//	msg_print("�Ȃ񂾂��̊K�i�́I");
		//else
			msg_print("���̊K�ɍ~�肽�B");
#else
			msg_print("You enter the down staircase.");
#endif

		leave_quest_check();
		leave_tower_check();

		p_ptr->inside_quest = c_ptr->special;

		/* Activate the quest */
		/*:::����p�����H*/
		if (!quest[p_ptr->inside_quest].status)
		{
			if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM)
			{
				init_flags = INIT_ASSIGN;
				process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			}
			quest[p_ptr->inside_quest].status = QUEST_STATUS_TAKEN;
		}

		/* Leaving a quest */
		if (!p_ptr->inside_quest)
		{
			dun_level = 0;
		}

		/* Leaving */
		p_ptr->leaving = TRUE;

		p_ptr->oldpx = 0;
		p_ptr->oldpy = 0;
		
		
        /* Hack -- take a turn */
        energy_use = 100;
	}

	else
	{
		int target_dungeon = 0;
		if (!dun_level)
		{
			target_dungeon = have_flag(f_ptr->flags, FF_ENTRANCE) ? c_ptr->special : DUNGEON_ANGBAND;

			/*:::�A�҂Ȃ��I�v�V�����ł͓S���ȊO�̃_���W�����̊K�i�֓���Ȃ�*/
			if (ironman_downward && (target_dungeon != DUNGEON_ANGBAND))
			{
#ifdef JP
				msg_print("�_���W�����̓����͍ǂ���Ă���I");
#else
				msg_print("The entrance of this dungeon is closed!");
#endif
				return;
			}
			/*:::���߂ē���Ƃ���max_dlv[]��0�炵���B�m�F������*/
			if (!max_dlv[target_dungeon])
			{
#ifdef JP
				msg_format("�����ɂ�%s�̓����(%d�K����)������܂�", d_name+d_info[target_dungeon].name, d_info[target_dungeon].mindepth);
				if (!get_check("�{���ɂ��̃_���W�����ɓ���܂����H")) return;
#else
				msg_format("There is the entrance of %s (Danger level: %d)", d_name+d_info[target_dungeon].name, d_info[target_dungeon].mindepth);
				if (!get_check("Do you really get in this dungeon? ")) return;
#endif
			}

			/* Save old player position */
			p_ptr->oldpx = px;
			p_ptr->oldpy = py;
			dungeon_type = (byte)target_dungeon;

			/*
			 * Clear all saved floors
			 * and create a first saved floor
			 */
			prepare_change_floor_mode(CFM_FIRST_FLOOR);
		}
		//�G�N�X�g���N�G�X�g������
		if(INSIDE_EXTRA_QUEST)
		{
			if(!confirm_leave_level(TRUE)) return;
			leave_quest_check();
			p_ptr->inside_quest = 0;
		}

		/* Hack -- take a turn */
		energy_use = 100;

		if (autosave_l) do_cmd_save_game(TRUE);

		/* Go down */
		if (have_flag(f_ptr->flags, FF_SHAFT) && !EXTRA_MODE) down_num += 2;
		else down_num += 1;

		if (!dun_level)
		{
			/* Enter the dungeon just now */
			p_ptr->enter_dungeon = TRUE;
			down_num = d_info[dungeon_type].mindepth;
		}

		if (record_stair)
		{
#ifdef JP
			if (fall_trap) do_cmd_write_nikki(NIKKI_STAIR, down_num, "���Ƃ��˂ɗ�����");
			else do_cmd_write_nikki(NIKKI_STAIR, down_num, "�K�i�����肽");
#else
			if (fall_trap) do_cmd_write_nikki(NIKKI_STAIR, down_num, "fell through a trap door");
			else do_cmd_write_nikki(NIKKI_STAIR, down_num, "climbed down the stairs to");
#endif
		}

		if (fall_trap)
		{
#ifdef JP
			msg_print("�킴�Ɨ��Ƃ��˂ɗ������B");
#else
			msg_print("You deliberately jump through the trap door.");
#endif
		}
		else
		{
			/* Success */
			if (target_dungeon)
			{
#ifdef JP
				msg_format("%s�֓������B", d_text + d_info[dungeon_type].text);
#else
				msg_format("You entered %s.", d_text + d_info[dungeon_type].text);
#endif
			}
			else
			{
#ifdef JP
				///del131213 msg
				//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				//	msg_print("�Ȃ񂾂��̊K�i�́I");
				//else
				msg_print("�K�i������ĐV���Ȃ���{�ւƑ��𓥂ݓ��ꂽ�B");
#else
				msg_print("You enter a maze of down staircases.");
#endif
			}
		}


		/* Leaving */
		p_ptr->leaving = TRUE;

		if (fall_trap)
			/*:::���Ƃ��˂ɗ������Ƃ��̓����_���ȏꏊ�Ɂ������������I�v�V����*/
		{
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
		}
		else if(EXTRA_MODE)
		{
			prepare_change_floor_mode(CFM_DOWN | CFM_NO_RETURN);
		}
		else
		{
			if (have_flag(f_ptr->flags, FF_SHAFT))
			{
				/* Create a way back */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_SHAFT);
			}
			else
			{
				/* Create a way back */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN);
			}
		}
	}
}



/*
 * Simple command to "search" for one turn
 */
/*:::�T��*/
void do_cmd_search(void)
{
	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	energy_use = 100;

	/* Search */
	search();
}



/*
 * Allocates objects upon opening a chest    -BEN-
 *
 * Disperse treasures from the given chest, centered at (x,y).
 *
 * Small chests often contain "gold", while Large chests always contain
 * items.  Wooden chests contain 2 items, Iron chests contain 4 items,
 * and Steel chests contain 6 items.  The "value" of the items in a
 * chest is based on the "power" of the chest, which is in turn based
 * on the level on which the chest is generated.
 */
/*:::�����J���A�C�e�����U��΂�Bscatter��TRUE���ƃ_���W�������ɎU��΂�*/
/*:::�J����Ɠ���A�C�e�����o�������Ȃ��Ƃ��N���锠��ǉ�����*/
static void chest_death(bool scatter, int y, int x, s16b o_idx)
{
	int number;

	int k_quantity = 0; //k_idx���w�肷�锠�Ō����w�肷��Ƃ�

	bool small;
	u32b mode = AM_GOOD;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr = &o_list[o_idx];

	if (o_ptr->tval != TV_CHEST)
	{
		msg_format("ERROR:tval��TV_CHEST�ȊO(%d)�̂Ƃ�chest_death()���Ă΂ꂽ",o_ptr->tval);
		return;
	}

	/*:::�o��A�C�e���̎��ƌ�������*/
	/* Small chests often hold "gold" */
	small = (o_ptr->sval < SV_CHEST_MIN_LARGE);

	/* Determine how much to drop (see above) */
	number = (o_ptr->sval % SV_CHEST_MIN_LARGE) * 2;
	/*:::�J���d���̐������x���͓���*/
	if (o_ptr->sval == SV_CHEST_KANDUME)
	{
		number = 5;
		small = FALSE;
		mode |= AM_GREAT;
		object_level = o_ptr->xtra3;
	}
	else
	{
		/* Determine the "value" of the items */
		//object_level = ABS(o_ptr->pval) + 10;
		//v1.1.97 ���̒��g�̃��x���������X�^�[�Ɠ����������t���A+5�ɂ���
		object_level = MIN(127,o_ptr->xtra3);
	}

	if (cheat_peek) msg_format("object_level:%d",object_level);

	/* Zero pval means empty chest */
	/*:::�󂢂���������������*/
	if (!o_ptr->pval) number = 0;

	/* Opening a chest */
	/*:::make_object()�̂Ƃ��ɔ�����������Ȃ����߂̃O���[�o���t���O*/
	opening_chest = TRUE;

	/*:::����̐ݔ���N�G�X�g�Ŏ�ɓ��锠�͒��g���Œ肳��Ă���*/
	///mod140117 ���g�w�蔠���[�`����ǉ�
	if(o_ptr->sval >= SV_CHEST_MARIPO_01 && o_ptr->sval != SV_CHEST_KANDUME)
	{
		int a_idx = 0;
		int k_idx = 0;

		///mod140323 �N���[���n����V�@�C�𗚂��Ȃ����ɂ̓V���@�W���P
		///mod141118 �L�X�������V�@�V���@��
		//�l�����C�h�ɂ̓V���@���
		if(o_ptr->sval == SV_CHEST_CLONE)
		{
			if (p_ptr->pclass == CLASS_KISUME)
			{
				object_level = 60;
				q_ptr = &forge;
				do
				{
					object_prep(q_ptr, lookup_kind(TV_ARMOR, SV_ARMOR_TUB));
					create_artifact(q_ptr, FALSE);
				}while(object_is_cursed(q_ptr));
				q_ptr->pval = 4;
				add_flag(q_ptr->art_flags, TR_SPEED);
				add_flag(q_ptr->art_flags, TR_FREE_ACT);
				if(q_ptr->to_a < 16) q_ptr->to_a = 16;
				q_ptr->art_name = quark_add("�V���@�̉��g��");
				(void)drop_near(q_ptr, -1, y, x);
			}
			else if (p_ptr->prace == RACE_NINGYO && p_ptr->pclass == CLASS_MAID)
			{
				object_level = 60;
				q_ptr = &forge;
				do
				{
					object_prep(q_ptr, lookup_kind(TV_GLOVES, SV_HAND_DRESSGLOVES));
					create_artifact(q_ptr, FALSE);
				}while(object_is_cursed(q_ptr));
				q_ptr->pval = 4;
				add_flag(q_ptr->art_flags, TR_SPEED);
				add_flag(q_ptr->art_flags, TR_FREE_ACT);
				q_ptr->art_name = quark_add("�V���@�̉��g��");
				(void)drop_near(q_ptr, -1, y, x);


			}
			else if (p_ptr->prace == RACE_NINGYO || p_ptr->pclass == CLASS_TOZIKO)
			{
				a_idx = ART_SHIVA_JACKET;
			}
			else
			{
				a_idx = ART_SHIVA_SHOES;
			}
		}


		/*:::�Â����V�@�E�Ƃ��ƂɎw�肷��*/
		else if(o_ptr->sval == SV_CHEST_OLDCASTLE)
		{
			if(p_ptr->pclass == CLASS_WARRIOR)
			{
				//���l��m�͓��ꏈ���ŏ��������Ƃ��΂₫�����Z�b�g
				if(p_ptr->prace == RACE_KOBITO)
				{
					bool ok = FALSE;
					artifact_type *a_ptr;
					a_ptr = &a_info[ART_TINYTHORN];
					if (!a_ptr->cur_num)
					{
						ok = TRUE;
						if (create_named_art(ART_TINYTHORN, y, x))
						{
							a_ptr->cur_num = 1;

							if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
						}
						else if (!preserve_mode) a_ptr->cur_num = 1;
					}
					a_ptr = &a_info[ART_QUICKTHORN];
					if (!a_ptr->cur_num)
					{
						ok = TRUE;
						if (create_named_art(ART_QUICKTHORN, y, x))
						{
							a_ptr->cur_num = 1;

							if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
						}
						else if (!preserve_mode) a_ptr->cur_num = 1;
					}
					if(!ok) k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//������
				}
				else
					a_idx = ART_GLAIVE_PAIN;
			}
			else if(p_ptr->pclass == CLASS_CAVALRY)
			{
				if(p_ptr->prace == RACE_KOBITO)
					a_idx = ART_GUNGNIR;
				else 
					a_idx = ART_GLAIVE_PAIN;
			}				

			else if(p_ptr->pclass == CLASS_ROGUE) a_idx = ART_ALHAZRED;
			else if(p_ptr->pclass == CLASS_MAGE || p_ptr->pclass == CLASS_HIGH_MAGE)
			{
				if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU)
					a_idx = ART_ODUNU;
				else
					a_idx = ART_INDRA;
			}
			else if(p_ptr->pclass == CLASS_RANGER) a_idx = ART_BARD_BOW;
			else if(p_ptr->pclass == CLASS_MAID && p_ptr->psex == SEX_MALE) a_idx = ART_WALTER;
			else if(p_ptr->pclass == CLASS_MAID && p_ptr->psex == SEX_FEMALE) a_idx = ART_ROBERTA;
			else if(p_ptr->pclass == CLASS_ARCHER) a_idx = ART_YORIMASA;
			else if(p_ptr->pclass == CLASS_CIVILIAN)
			{
				if(p_ptr->prace == RACE_KOBITO)
					a_idx = ART_POWER;
				else 
					a_idx = ART_CHAINSWORD;
			}

			else if(p_ptr->pclass == CLASS_TEACHER) a_idx = ART_JACK;
			else if(p_ptr->pclass == CLASS_PRIEST) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_OUTSIDER) a_idx = ART_AMENONUBOKO;
			//���_����̍�������
			else if(p_ptr->pclass == CLASS_TSUKUMO_MASTER)
			{
				q_ptr = &forge;
				if(p_ptr->prace == RACE_KOBITO) object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_HAYABUSA));
				else object_prep(q_ptr, lookup_kind(TV_AXE, SV_WEAPON_MASAKARI));
				apply_magic(q_ptr, 60, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
				if(p_ptr->prace == RACE_KOBITO) q_ptr->pval = 3; 
				(void)drop_near(q_ptr, -1, y, x);
			}
			else if(p_ptr->pclass == CLASS_SYUGEN) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_MAGIC_KNIGHT) a_idx = ART_CHARMED_PENDANT;
			else if(p_ptr->pclass == CLASS_LIBRARIAN) a_idx = ART_CHARMED_PENDANT;
			else if(p_ptr->pclass == CLASS_ENGINEER)
			{
				if(p_ptr->prace == RACE_KOBITO || gun_free_world)
					a_idx = ART_MARIO;
				else 
					 a_idx = ART_CRIMSON;
			}
			else if(p_ptr->pclass == CLASS_SAMURAI) a_idx = ART_AGLARANG;
			else if(p_ptr->pclass == CLASS_MINDCRAFTER) a_idx = ART_ELMI;
			else if(p_ptr->pclass == CLASS_PALADIN) a_idx = ART_SACRED_KNIGHT;
			else if(p_ptr->pclass == CLASS_SH_DEALER)
				k_idx = lookup_kind(TV_MATERIAL, SV_MATERIAL_HIHIIROKANE);
			else if(p_ptr->pclass == CLASS_JEWELER) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_NINJA) a_idx = ART_NIGHT;

			else if (p_ptr->pclass == CLASS_KASEN)
			{
				if(is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
					a_idx = ART_ECSEDI;
				else
					a_idx = ART_METALHAND;
			}
			else if(p_ptr->pclass == CLASS_SUIKA) a_idx = ART_SHUTEN_DOJI;
			else if(p_ptr->pclass == CLASS_TEWI) a_idx = ART_DAIKOKU;
			else if(p_ptr->pclass == CLASS_KOGASA) a_idx = ART_SANAGI;
			else if(p_ptr->pclass == CLASS_IKU) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_KOMACHI) a_idx = ART_KRONOS;
			else if(p_ptr->pclass == CLASS_KOISHI) a_idx = ART_CHAINSWORD;
			else if (p_ptr->pclass == CLASS_UDONGE)
			{
				if(is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
					a_idx = ART_CRIMSON;
				else
					a_idx = ART_BLOODYMOON;
			}
			else if(p_ptr->pclass == CLASS_MOMIZI) a_idx = ART_ODUNU;
			else if(p_ptr->pclass == CLASS_YOUMU) a_idx = ART_ZANTETSU;
			else if(p_ptr->pclass == CLASS_SEIGA) a_idx = ART_HEAVENLY_MAIDEN;
			else if(p_ptr->pclass == CLASS_CIRNO) a_idx = ART_EXCALIBUR;
			else if(p_ptr->pclass == CLASS_ORIN) a_idx = ART_HARNESS_HELL;
			else if(p_ptr->pclass == CLASS_SHINMYOUMARU) a_idx = ART_AMENOKAGAMI;
			else if(p_ptr->pclass == CLASS_NAZRIN) a_idx = ART_KUKAI;
			else if(p_ptr->pclass == CLASS_LETTY) a_idx = ART_GRAYSWANDIR;
			else if(p_ptr->pclass == CLASS_RUMIA) a_idx = ART_GRAYSWANDIR;

			else if(p_ptr->pclass == CLASS_YOSHIKA) a_idx = ART_HARNESS_HELL;
			else if(p_ptr->pclass == CLASS_PATCHOULI) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_AYA) a_idx = ART_ODUNU;
			else if(p_ptr->pclass == CLASS_BANKI) a_idx = ART_DEATH_CSYTHE;
			else if(p_ptr->pclass == CLASS_MYSTIA) a_idx = ART_CHICKEN_KNIFE;
			else if(p_ptr->pclass == CLASS_FLAN) a_idx = ART_TWILIGHT;
			else if(p_ptr->pclass == CLASS_SHOU) a_idx = ART_BISYAMON;
			else if(p_ptr->pclass == CLASS_MEDICINE)  a_idx = ART_METALHAND;
			else if(p_ptr->pclass == CLASS_YUYUKO)  a_idx = ART_SAIGYOU;
			else if(p_ptr->pclass == CLASS_SATORI)  a_idx = ART_CHARMED_PENDANT;
			else if(p_ptr->pclass == CLASS_KYOUKO) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_TOZIKO) a_idx = ART_YORIMASA;
			else if(p_ptr->pclass == CLASS_LILY_WHITE) a_idx = ART_HURIN;
			else if(p_ptr->pclass == CLASS_KISUME) a_idx = ART_MJOLLNIR;
			else if(p_ptr->pclass == CLASS_HATATE) a_idx = ART_ODUNU;
			else if(p_ptr->pclass == CLASS_WAKASAGI) a_idx = ART_TUOR;
			else if(p_ptr->pclass == CLASS_MIKO) a_idx = ART_AMENOHABAKIRI;
			else if(p_ptr->pclass == CLASS_KOKORO) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_WRIGGLE) a_idx = ART_NAUSICAA;
			else if(p_ptr->pclass == CLASS_YUUKA)  a_idx = ART_NARYA;
			else if(p_ptr->pclass == CLASS_CHEN) a_idx = ART_HARNESS_HELL;
			else if(p_ptr->pclass == CLASS_MURASA) a_idx = ART_SARASVATI;
			else if(p_ptr->pclass == CLASS_KEINE) a_idx = ART_AMENOHABAKIRI;
			else if(p_ptr->pclass == CLASS_YUGI) a_idx = ART_CAMBELEG;
			else if(p_ptr->pclass == CLASS_REIMU) a_idx = ART_IDUNOME;
			else if(p_ptr->pclass == CLASS_KAGEROU) a_idx = ART_LUTHIEN;
			else if(p_ptr->pclass == CLASS_SHIZUHA) a_idx = ART_COLLUIN;
			else if(p_ptr->pclass == CLASS_SANAE) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_MINORIKO) a_idx = ART_KRONOS;
			else if(p_ptr->pclass == CLASS_REMY) a_idx = ART_GUNGNIR;
			else if(p_ptr->pclass == CLASS_NITORI) a_idx = ART_HIRAISHI;
			else if(p_ptr->pclass == CLASS_BYAKUREN) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_NUE) a_idx = ART_TRAPEZOHEDRON;
			else if(p_ptr->pclass == CLASS_SEIJA) a_idx = ART_TUOR;
			else if(p_ptr->pclass == CLASS_UTSUHO) a_idx = ART_GIL_GALAD;
			else if(p_ptr->pclass == CLASS_YAMAME) a_idx = ART_SANAGI;
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			{
				if(p_ptr->prace == RACE_NINGYO) a_idx = ART_NENYA;
				else	a_idx = ART_SHIVA_JACKET;
			}
			else if(p_ptr->pclass == CLASS_MAGIC_EATER) a_idx = ART_LAPUTA;
			else if(p_ptr->pclass == CLASS_SUWAKO) a_idx = ART_KRONOS;
			else if(p_ptr->pclass == CLASS_RAN) a_idx = ART_GOKASHITIKIN;
			else if(p_ptr->pclass == CLASS_EIKI) a_idx = ART_JUDGE;
			else if(p_ptr->pclass == CLASS_TENSHI) a_idx = ART_THORIN;
			else if(p_ptr->pclass == CLASS_MEIRIN) a_idx = ART_CHINGRONG;
			else if(p_ptr->pclass == CLASS_PARSEE) a_idx = ART_TERROR;
			else if(p_ptr->pclass == CLASS_SHINMYOU_2) a_idx = ART_CHARIOT;
			else if(p_ptr->pclass == CLASS_SUMIREKO) a_idx = ART_PALANTIR;
			else if(p_ptr->pclass == CLASS_ICHIRIN) a_idx = ART_SACRED_KNIGHT;
			else if(p_ptr->pclass == CLASS_MOKOU) a_idx = ART_GURENKI;
			else if(p_ptr->pclass == CLASS_KANAKO) a_idx = ART_MAGATAMA;
			else if(p_ptr->pclass == CLASS_FUTO) a_idx = ART_SEIMEI;
			else if(p_ptr->pclass == CLASS_SUNNY) a_idx = ART_WEREWINDLE;
			else if(p_ptr->pclass == CLASS_LUNAR) a_idx = ART_ZYOGA;
			else if(p_ptr->pclass == CLASS_STAR) a_idx = ART_TRAPEZOHEDRON;
			else if(p_ptr->pclass == CLASS_ALICE) a_idx = ART_QUEEN_OF_RED;
			else if(p_ptr->pclass == CLASS_MARISA) a_idx = ART_CADUCEUS;
			else if(p_ptr->pclass == CLASS_LUNASA || p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
				a_idx = ART_HARMONIA;
			else if(p_ptr->pclass == CLASS_CLOWNPIECE || p_ptr->pclass == CLASS_VFS_CLOWNPIECE) a_idx = ART_BLOODYMOON;
			else if(p_ptr->pclass == CLASS_DOREMY) a_idx = ART_METALHAND;
			else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI) a_idx = ART_SARASVATI;
			else if (p_ptr->pclass == CLASS_SAKUYA) //���@�����_���A�[�e�B�t�@�N�g���C�h���@���f�ϐ���[���u�폜�ۏ�
			{
				object_level = 75;
				q_ptr = &forge;
				do
				{
					object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_MAID));
					create_artifact(q_ptr, FALSE);
				}while(object_is_cursed(q_ptr));
				add_flag(q_ptr->art_flags, TR_RES_ACID);
				add_flag(q_ptr->art_flags, TR_RES_ELEC);
				add_flag(q_ptr->art_flags, TR_RES_FIRE);
				add_flag(q_ptr->art_flags, TR_RES_COLD);
				remove_flag(q_ptr->art_flags, TR_NO_TELE);
				remove_flag(q_ptr->art_flags, TR_NO_MAGIC);
				q_ptr->to_a = 18 + randint0(6);

				(void)drop_near(q_ptr, -1, y, x);
			}
			else if(p_ptr->pclass == CLASS_HINA)
				a_idx = ART_HARMONIA;
			else if(p_ptr->pclass == CLASS_3_FAIRIES) a_idx = ART_THE_DAWN;
			else if(p_ptr->pclass == CLASS_RAIKO) a_idx = ART_INDRA;
			else if(p_ptr->pclass == CLASS_CHEMIST) a_idx = ART_AESCULAPIUS;
			else if(p_ptr->pclass == CLASS_MAMIZOU) a_idx = ART_GOGO;
			else if(p_ptr->pclass == CLASS_YUKARI) a_idx = ART_ZYOGA;
			else if(p_ptr->pclass == CLASS_RINGO) a_idx = ART_CRIMSON;
			else if(p_ptr->pclass == CLASS_SEIRAN) a_idx = ART_MASKET;
			else if(p_ptr->pclass == CLASS_EIRIN) a_idx = ART_AMANOMAKAKO;
			else if(p_ptr->pclass == CLASS_KAGUYA) k_idx = lookup_kind(TV_MATERIAL,SV_MATERIAL_MYSTERIUM);
			
			else if(p_ptr->pclass == CLASS_SAGUME) a_idx = ART_QUEEN_OF_RED;
			else if(p_ptr->pclass == CLASS_REISEN2) a_idx = ART_MASKET;
			else if(p_ptr->pclass == CLASS_TOYOHIME) a_idx = ART_SHIOMITSUTAMA;
			else if(p_ptr->pclass == CLASS_YORIHIME) a_idx = ART_AMENOHABAKIRI;
			else if(p_ptr->pclass == CLASS_HECATIA) a_idx = ART_CADUCEUS;
			else if(p_ptr->pclass == CLASS_JUNKO) a_idx = ART_KRONOS;
			else if (p_ptr->pclass == CLASS_SOLDIER)
			{
				if(p_ptr->prace == RACE_KOBITO)
					a_idx = ART_BILLY_THE_KID;
				else
					a_idx = ART_MASKET;
			}
			else if(p_ptr->pclass == CLASS_NEMUNO) a_idx = ART_KINTAROU;
			else if(p_ptr->pclass == CLASS_AUNN) a_idx = ART_SACRED_KNIGHT;
			else if(p_ptr->pclass == CLASS_NARUMI) a_idx = ART_KUKAI_KASA;
			else if(p_ptr->pclass == CLASS_KOSUZU) a_idx = ART_SANAGI;
			else if(p_ptr->pclass == CLASS_LARVA) a_idx = ART_AMBER;
			else if(p_ptr->pclass == CLASS_MAI) a_idx = ART_RED_SHOES;
			else if (p_ptr->pclass == CLASS_SATONO) a_idx = ART_ASTARTE;
			else if(p_ptr->pclass == CLASS_JYOON) a_idx = ART_ECSEDI;
			else if (p_ptr->pclass == CLASS_SHION) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_OKINA) a_idx = ART_TRAPEZOHEDRON;
			else if (p_ptr->pclass == CLASS_EIKA) a_idx = ART_SARASVATI;
			else if (p_ptr->pclass == CLASS_MAYUMI) a_idx = ART_AGLARANG;
			else if (p_ptr->pclass == CLASS_KUTAKA) a_idx = ART_CHICKEN_KNIFE;
			else if (p_ptr->pclass == CLASS_URUMI) a_idx = ART_TUOR;
			else if (p_ptr->pclass == CLASS_SAKI) a_idx = ART_FEANOR_BOOTS;
			else if (p_ptr->pclass == CLASS_YACHIE) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_KEIKI)
				k_idx = lookup_kind(TV_MATERIAL, SV_MATERIAL_HIHIIROKANE);
			else if (p_ptr->pclass == CLASS_RESEARCHER) a_idx = ART_PALANTIR;
			else if (p_ptr->pclass == CLASS_CARD_DEALER) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_TAKANE) a_idx = ART_THRANDUIL;
			else if (p_ptr->pclass == CLASS_MOMOYO) a_idx = ART_NAIN_PICK;
			else if (p_ptr->pclass == CLASS_SANNYO) a_idx = ART_ANDVARI;
			else if (p_ptr->pclass == CLASS_MIKE) a_idx = ART_DAIKOKU;


			else k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//������
		}
		else if(o_ptr->sval == SV_CHEST_MARIPO_01) k_idx = 549;
		else if(o_ptr->sval == SV_CHEST_MARIPO_02) k_idx = 550;
		else if(o_ptr->sval == SV_CHEST_MARIPO_03) k_idx = 548;
		else if(o_ptr->sval == SV_CHEST_MARIPO_04) k_idx = 400;
		else if(o_ptr->sval == SV_CHEST_MARIPO_05) k_idx = 397;
		else if(o_ptr->sval == SV_CHEST_MARIPO_06) k_idx = 578;
		else if(o_ptr->sval == SV_CHEST_MARIPO_07) k_idx = 556;
		else if(o_ptr->sval == SV_CHEST_MARIPO_08) k_idx = 574;

		else if(o_ptr->sval == SV_CHEST_MARIPO_09) k_idx = 347;
		else if(o_ptr->sval == SV_CHEST_MARIPO_10) k_idx = 265;
		else if(o_ptr->sval == SV_CHEST_MARIPO_11) k_idx = 422;
		else if(o_ptr->sval == SV_CHEST_MARIPO_12) k_idx = 266;
		else if(o_ptr->sval == SV_CHEST_MARIPO_13) k_idx = 428;
		else if(o_ptr->sval == SV_CHEST_MARIPO_14) k_idx = 354;
		else if(o_ptr->sval == SV_CHEST_MARIPO_15) k_idx = 601;
		else if(o_ptr->sval == SV_CHEST_MARIPO_16) k_idx = 580;

		else if(o_ptr->sval == SV_CHEST_MARIPO_17) k_idx = 356;
		//�T�C�o�[�f�[�����̐l�`
		else if(o_ptr->sval == SV_CHEST_MARIPO_18)
		{
			q_ptr = &forge;
			object_prep(q_ptr, 391);
			q_ptr->pval = MON_CYBER;
			(void)drop_near(q_ptr, -1, y, x);
		}
		//�d��(75�K������������)
		else if(o_ptr->sval == SV_CHEST_MARIPO_19)
		{
			q_ptr = &forge;
			object_prep(q_ptr, 105);//�d��
			apply_magic(q_ptr, 75, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
			(void)drop_near(q_ptr, -1, y, x);
		}
		else if(o_ptr->sval == SV_CHEST_MARIPO_20) a_idx = 129;
		else if(o_ptr->sval == SV_CHEST_MARIPO_21) a_idx = 86;
		else if(o_ptr->sval == SV_CHEST_MARIPO_22) a_idx = 22;
		else if(o_ptr->sval == SV_CHEST_MARIPO_23) a_idx = 234;
		//�����I�[���i�v
		else if(o_ptr->sval == SV_CHEST_MARIPO_24)
		{
			msg_print("�����J����ƁA�^�����ȉ��������o���Ă��Ȃ����񂾁c");
			msg_print(NULL);
			(void)gain_random_mutation(193);//�����I�[��
			p_ptr->muta3_perma |= MUT3_GOOD_LUCK;

		}
		/*:::���~���A����̏���*/
		else if (o_ptr->sval == SV_CHEST_REMY)
		{
			msg_print("�p���̗�󂾁B���������ȃ��{�����ꏏ�ɓ����Ă���B");
			q_ptr = &forge;
			object_prep(q_ptr, 160);//���{��
			apply_magic(q_ptr, 20, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
			(void)drop_near(q_ptr, -1, y, x);
		}
		/*:::�h���S���ގ���V�@�X�s�[�h�̎w��+5�`9*/
		else if (o_ptr->sval == SV_CHEST_FAIRY)
		{
			q_ptr = &forge;
			object_prep(q_ptr, 259);
			q_ptr->pval = 4 + randint1(3);
			if(EXTRA_MODE) q_ptr->pval += 3;
			if(weird_luck()) q_ptr->pval += 2;
			(void)drop_near(q_ptr, -1, y, x);
		}
		//���g�h�q�N�G��V�@�V���A�ˑR�ψفA�����A�����������Ȃ��
		//mod151227 �����ǉ�
		///mod160618 �i�Ԃ̂Ƃ��͕s�����̉H��
		else if(o_ptr->sval == SV_CHEST_EIRIN)
		{
			if(p_ptr->pclass == CLASS_EIRIN || p_ptr->pclass == CLASS_KAGUYA)
			{
				k_idx = lookup_kind(TV_SOUVENIR, SV_SOUVENIR_PHOENIX_FEATHER);
			}
			else
			{
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_NEW_LIFE));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_POLYMORPH));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_DETONATIONS));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_NANIKASUZUSHIKUNARU));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_LOVE));
				q_ptr->number = 1;
				(void)drop_near(q_ptr, -1, y, x);
			}
		}
		/*:::��n�x�����}���V�@���̋����̎w��+3�`5*/
		else if (o_ptr->sval == SV_CHEST_BYAKUREN)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_MUSCLE));
			q_ptr->pval = 3 + randint1(15)/7;
			(void)drop_near(q_ptr, -1, y, x);
		}
		else if(o_ptr->sval == SV_CHEST_SEIJA)
		{
			msg_print("���ɂ͋������Ɗ����������Ă����B");
			p_ptr->au += 1000000L;
			p_ptr->redraw |= (PR_GOLD);						
			k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//������
		}
		///mod140629 �ϒK�푈
		else if (o_ptr->sval == SV_CHEST_KORI)
		{
			//���`�[��
			if(o_ptr->xtra5 == 1 && p_ptr->pclass != CLASS_RAN) a_idx = ART_GOKASHITIKIN;
			//�K�`�[��
			else if(o_ptr->xtra5 == 2 && p_ptr->pclass != CLASS_MAMIZOU) a_idx = ART_TONBO;
			//�ɂƂ�̑S�ł̓A�~���łȂ����
			else if(p_ptr->pclass == CLASS_NITORI)
			{
				msg_print("�F�̊|�����������Ă���B���Ȃ��̑���肾�B");
				p_ptr->au += 3000000L;
				p_ptr->redraw |= (PR_GOLD);		
			}
			//�S�� ���V���K���t�����O�Œ�̔��˃A�~����
			else
			{
				object_level = 50;
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_REFLECTION));
				create_artifact(q_ptr, FALSE);
				add_flag(q_ptr->art_flags, TR_LEVITATION);
				q_ptr->art_name = quark_add("�G���W�j�A�͏�ɂƂ��");
				(void)drop_near(q_ptr, -1, y, x);
			}

		}


		//v1.1.91 ���N�U�R��1
		else if (o_ptr->sval == SV_CHEST_YAKUZA1)
		{
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_ZOKU;
				else
					a_idx = ART_TETSUGETA;
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_TARNKAPPE;
				else
					a_idx = ART_SPECTRAL_DRAGON;
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_TRAPEZOHEDRON;
				else
					a_idx = ART_MAGMA;
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_HANIWA)
			{
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q1_ACHIEVE)
					a_idx = ART_NOMINOSUKUNE;
				else
					a_idx = ART_OOONOTE;
			}
			//�S�� ����Ă��Ȃ������_���Ȏw�ւ̃����_���A�[�e�B�t�@�N�g
			else
			{
				int tmp_sv;

				while (1)
				{
					object_kind *k_ptr;
					int k_idx;
					//�����_���Ɏw�փx�[�X�A�C�e����I��
					tmp_sv = randint1(SV_RING_MAX);
					k_idx = lookup_kind(TV_RING, tmp_sv);
					if (!k_idx) continue;
					k_ptr = &k_info[k_idx];
					//�􂢊m��⁚�m��̃x�[�X�A�C�e���͏��O
					if (k_ptr->gen_flags & (TRG_CURSED)) continue;
					if (k_ptr->gen_flags & (TRG_INSTA_ART)) continue;
					break;
				}

				object_level = 60;
				while (1)
				{
					q_ptr = &forge;
					object_prep(q_ptr, lookup_kind(TV_RING, tmp_sv));
					apply_magic(q_ptr, object_level, AM_FORCE_NORMAL);
					create_artifact(q_ptr, FALSE);
					if (object_is_cursed(q_ptr)) continue;
					break;
				}
				object_level = base_level;

				(void)drop_near(q_ptr, -1, y, x);
			}

		}
		//v1.1.98 �A�����������U
		else if (o_ptr->sval == SV_CHEST_HANGOKU2)
		{
			int i = 5+randint1(15);
			q_ptr = &forge;
			for (; i > 0; i--)
			{
				int roll = randint1(100);

				if(roll == 100)
					object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_METEORICIRON));
				else if(roll > 90)
					object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_MAGIC_WATERMELON));
				else if(roll > 75)
					object_prep(q_ptr, lookup_kind(TV_MUSHROOM, SV_MUSHROOM_PUFFBALL));
				else if (roll > 55)
					object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_FLOWER));
				else
					object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_SKULL));

				apply_magic(q_ptr, object_level, 0L);
				(void)drop_near(q_ptr, -1, y, x);
			}

		}
		/*:::�T�g���E�R���t���N�g��V�@���@�E�Ȃ���̈�A�X�y�}�X�n�Ȃ�g�p�\�ȗ̈悩�烉���_���A�ǂ���ł��Ȃ��Ȃ�S�������_���Ȏl���ږ��@��*/
		else if(o_ptr->sval == SV_CHEST_SATORI) 
		{
			int book_tv;
			if(cp_ptr->realm_aptitude[0] == 1 || cp_ptr->realm_aptitude[0] == 3) book_tv = p_ptr->realm1;
			else if(p_ptr->pclass == CLASS_CHEMIST) book_tv = TV_BOOK_MEDICINE;
			else if(cp_ptr->realm_aptitude[0] == 2)
			{
				int j,cnt=0;
				for(j=1;j<=MAX_MAGIC;j++)
				{
					///mod �X�y�}�X�n�E�̈ꕔ�ŕ�V����������Ȃ������̂ŏC��
					if(cp_ptr->realm_aptitude[j] >1 && rp_ptr->realm_aptitude[j])
					{
						cnt++;
						if(one_in_(cnt)) book_tv = j;
					}
				}
		
				if(!cnt) book_tv = randint1(TV_BOOK_END-1); //�����_���Ȗ{�̍ŏI���@TV=15�͖������̗\���̈�̂���-1���Ƃ�
			}
			else book_tv = randint1(TV_BOOK_END-1);

			k_idx = lookup_kind(book_tv,3);

			/*::: Hack - �}���T�R���N���t�g�͔����J����ΒB���ɂ���*/
			if(p_ptr->inside_quest == QUEST_MARICON) check_find_art_quest_completion(o_ptr);

		}
		/*:::��l�Ȃ�ĕ|���Ȃ��@�Ԕ؊�̓V���o�[DSM �d���͉���̎w�� �ق��͑ϔj�׃A�~��*/
		else if(o_ptr->sval == SV_CHEST_GRASSROOT) 
		{
			if(p_ptr->pclass == CLASS_BANKI) k_idx = lookup_kind(TV_DRAG_ARMOR,SV_DRAGARMOR_SILVER);
			else if(p_ptr->prace == RACE_FAIRY) k_idx = lookup_kind(TV_RING,SV_RING_LORDLY);
			else k_idx =  lookup_kind(TV_AMULET,SV_AMULET_RES_HOLY);

		}
		/*:::�w�J�[�e�B�A�����@�����̊Z �������͔��F��*/
		else if(o_ptr->sval == SV_CHEST_HECATIA) 
		{

			if(p_ptr->pclass == CLASS_MARISA)
				k_idx = lookup_kind(TV_MATERIAL, SV_MATERIAL_HIHIIROKANE);
			else 
				a_idx = ART_RAZORBACK;
		}
		//���}���@�Őj�@�S�̏ꍇ��
		else if(o_ptr->sval == SV_CHEST_YAMAME) 
		{

			if(p_ptr->prace == RACE_ONI)
				k_idx = lookup_kind(TV_ALCOHOL, SV_ALCOHOL_SYUTYUU);
			else 
				k_idx = lookup_kind(TV_KNIFE, SV_WEAPON_DOKUBARI);
		}
		//�u�x�������v�Ə����ꂽ��
		else if(o_ptr->sval == SV_CHEST_SUPPLY) 
		{
			k_quantity = 10 + randint1(10);
			if(weird_luck()) k_quantity *= 2;

			k_idx = lookup_kind(TV_SWEETS, SV_SWEETS_DANGO);
		}
		///v1.1.18 �푰/�N���X����N�G�X�g
		else if (o_ptr->sval == SV_CHEST_SPECIAL_QUEST)
		{

			switch(o_ptr->xtra5)
			{
				case QUEST_TYPHOON:
				{
					q_ptr = &forge;
					object_prep(q_ptr, lookup_kind(TV_HEAD, SV_HEAD_TOKIN));//����
					apply_magic(q_ptr, 40, AM_NO_FIXED_ART | AM_GOOD | AM_GREAT);
					(void)drop_near(q_ptr, -1, y, x);
				}
				break;
				default:
				{
					msg_format("ERROR:���̔���xtra5�l����������(%d)",o_ptr->xtra5);
					k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//�l�с�

				}
			}
		}
		///v1.1.24 �}������N�G�X�g
		else if (o_ptr->sval == SV_CHEST_TORRENT)
		{

			switch(qrkdr_rec_turn)
			{
				case 1:
					msg_print("���ɂ͗D���ܕi�������Ă����B");
					k_idx =  lookup_kind(TV_MATERIAL, SV_MATERIAL_IZANAGIOBJECT);
					break;
				case 2:
					msg_print("���ɂ͏��D���ܕi�������Ă����B");
					k_idx =  lookup_kind(TV_MATERIAL, SV_MATERIAL_DRAGONNAIL);
					break;
				case 3:
					msg_print("���ɂ͎O�ʓ��܏ܕi�������Ă����B");
					k_idx =  lookup_kind(TV_POTION, SV_POTION_INC_CON);
					break;

				default:
					msg_print("���ɂ͎Q���܂������Ă����c");
					k_idx =  lookup_kind(TV_ALCOHOL, SV_ALCOHOL_KAPPA);
					break;
			}
		}
		//v1.1.36 ���鎸�H�N�G�X�g
		else if(o_ptr->sval == SV_CHEST_KOSUZU)
		{
			msg_print("���ɂ͐����̊����������Ă����B");

			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_FIRE));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_ICE));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_CHAOS));
			q_ptr->number = randint1(3);;
			(void)drop_near(q_ptr, -1, y, x);
		}
		//v1.1.42 ���́��œ|�N�G�X�g
		else if (o_ptr->sval == SV_CHEST_DREAM)
		{
			msg_print("���ɂ͌Âт����������Ă����B");
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SHILVER_KEY));
			(void)drop_near(q_ptr, -1, y, x);
		}
		//v1.1.87 10�A�K�`��
		else if (o_ptr->sval == SV_CHEST_10_GACHA)
		{
			int i;
			q_ptr = &forge;
			for (i = 0; i < 10; i++)
			{
				object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
				apply_magic(q_ptr, 1, 0);
				if (ability_card_list[q_ptr->pval].rarity_rank >= 4)
				{
					project_hack2(GF_RAINBOW, 0, 0, 777);
				}
				else if (ability_card_list[q_ptr->pval].rarity_rank >= 3)
				{
					project(0,5,py,px,77,GF_LITE_WEAK,(PROJECT_KILL|PROJECT_JUMP),-1);
				}

				(void)drop_near(q_ptr, -1, y, x);

			}

		}
		else if(o_ptr->sval == SV_CHEST_DEBUG) 
		{
			int j;
			int rank[ABILITY_CARD_LIST_LEN];
			msg_print("�e�X�g�p�A�r���e�B�J�[�h����");


			for (j = 0; j < ABILITY_CARD_LIST_LEN; j++)
			{
				rank[j] = 0;
			}
			for (j = 0; j < 1000000; j++)
			{
				int tmp;
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
				tmp = apply_magic_abilitycard(q_ptr, -1, 0, 0);
				rank[q_ptr->pval]++;
			}

			for (j = 0; j < ABILITY_CARD_LIST_LEN; j++)
			{
				msg_format("idx%d:%d", j,rank[j]);

			}



			(void)drop_near(q_ptr, -1, y, x);


		}
		else 	msg_print("ERROR:chest_death����`");



		if(a_idx > 0)
		{
			artifact_type *a_ptr = &a_info[a_idx];

			//v1.1.18 �e���o�Ȃ��I�v�V�������Ɓ��e���o�鎞�������ɂ���
			if(a_ptr->tval == TV_GUN && gun_free_world)
				k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);
			else if (!a_ptr->cur_num)
			{
				/* Create the artifact */
				if (create_named_art(a_idx, y, x))
				{
					a_ptr->cur_num = 1;

					/* Hack -- Memorize location of artifact in saved floors */
					if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
				}
				else if (!preserve_mode) a_ptr->cur_num = 1;
			}
			else k_idx =  lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);//���łɏo���ς݂̏ꍇ������
		}


		if (k_idx)
		{
			q_ptr = &forge;
			object_prep(q_ptr, k_idx);
			///mod150103 �A�C�e���������x����obj_level����plev*2�ɕύX
			apply_magic(q_ptr, (p_ptr->lev * 2), AM_NO_FIXED_ART | AM_GOOD);
			if(k_quantity) q_ptr->number = k_quantity;
			(void)drop_near(q_ptr, -1, y, x);
		}
	}//���g�m�蔠�����I��



	/* Drop some objects (non-chests) */
	else
	for (; number > 0; --number)
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Small chests often drop gold */
		if (small && (randint0(100) < 25))
		{
			/* Make some gold */
			if (!make_gold(q_ptr)) continue;
		}

		/* Otherwise drop an item */
		else
		{
			/* Make a good object */
			/*:::�A�C�e������*/
			if (!make_object(q_ptr, mode)) continue;
		}

		/* If chest scatters its contents, pick any floor square. */
		/*:::���̒��g���_���W�������ɎU��΂�㩂ɂ��������Ƃ�*/
		if (scatter)
		{
			int i;
			for (i = 0; i < 200; i++)
			{
				/* Pick a totally random spot. */
				y = randint0(MAX_HGT);
				x = randint0(MAX_WID);

				/* Must be an empty floor. */
				if (!cave_empty_bold(y, x)) continue;

				/* Place the object there. */
				drop_near(q_ptr, -1, y, x);

				/* Done. */
				break;
			}
		}
		/* Normally, drop object near the chest. */
		else drop_near(q_ptr, -1, y, x);
	}

	/* Reset the object level */
	object_level = base_level;

	/* No longer opening a chest */
	opening_chest = FALSE;

	/* Empty */
	o_ptr->pval = 0;

	/* Known */
	object_known(o_ptr);
}



//v1.1.97 ���̃g���b�v��������㩂�������Bchest_trap()������ɒu��������B
//���u�����ɂ���ā��ȊO�̃����X�^�[�Ȃǂ�㩂ɂ����邱�Ƃ��ł���悤�ɂ���B
//���̃g���b�v�̔����ɂ���Ĕ����̂��j�󂳂�邱�Ƃ�����B
//trap_player:����㩕t�������J���邩�������s����㩂ɂ�����Ƃ�TRUE�B
//FALSE�̂Ƃ�㩂ɂ���Ă͎��͂̓K���ȃv���C���[�⃂���X�^�[�������_���Ƀ^�[�Q�b�g���ă{���g�Ȃǂ𔭎˂���
void activate_chest_trap(int y, int x, s16b o_idx, bool trap_player)
{

	bool trap_monster = FALSE;
	int  i, trap_type;
	int dam,typ;
	int trap_range = MAX_RANGE;

	u32b summon_flag = (PM_ALLOW_GROUP | PM_NO_PET);

	//�{���g�Ȃǂ������猂���o�����Ƃ��̃^�[�Q�b�g�O���b�h
	int ty = 0, tx = 0;

	//������^�[�Q�b�g�܂ł̋���
	int target_dist = 0;

	object_type *o_ptr = &o_list[o_idx];//���Ȃ̂ŏ�ɏ���

	int chest_level = o_ptr->xtra3;//���̃��x��(�������x��+5)

	/* Ignore disarmed chests */
	if (o_ptr->pval <= 0) return;

	//����������Ƒ��̃g���b�v�̔����Ȃǂɂ�肱�̔����j�󂳂ꂽ��ԂŌĂ΂�邩������Ȃ��̂ŔO�̂���
	if (!o_ptr->k_idx) return;
	if (o_ptr->pval >= CHEST_TRAP_LIST_LENGTH)
	{
		msg_format("ERROR:���̔���pval���g���b�v�C���f�b�N�X�͈̔͊O(%d)",o_ptr->pval);
		o_ptr->pval = 0 - o_ptr->pval;//GF_ACTIV_TRAP�o�R�ł����ɗ�����������������Ȃ��ƃ��[�v�ɂȂ�Ǝv���̂ňꉞ
		return;
	}

	//㩂̎�ނ��擾
	trap_type = chest_new_traps[o_ptr->pval];

	//�t���������J���Ɏ��s����Ƃ悭��������
	if (p_ptr->pclass == CLASS_FLAN && one_in_(2)) trap_type = (CHEST_TRAP_EXPLODE);

	//���������ς݂ɂ���
	o_ptr->pval = (0 - o_ptr->pval);
	object_known(o_ptr);

	//v1.1.98 �g���b�v�Ȃ�(�{��)�̂Ƃ��I��
	if (trap_type == CHEST_TRAP_NOTHING) return;

	//GF_ACTIV_TRAP�o�R�ł����ɗ�����������������Ȃ��ƃ��[�v�ɂȂ�Ǝv���̂Ŏ��Ԓ�~���ł��������Ă���
	if (SAKUYA_WORLD || world_player)
	{
		msg_print("����㩂͍쓮���Ȃ������B");
		return;
	}


	//�^�[�Q�b�g�O���b�h�Ɣ�����^�[�Q�b�g�܂ł̋��������߂�B�g���b�v�̎�ނɂ���Ă͂��̏��������ʂɂȂ邪�債�����͂Ȃ����낤
	//�������ʂɔ���㩂ɂ��������Ƃ�
	if (trap_player)
	{
		tx = px;
		ty = py;
		if (!player_bold(y, x)) target_dist = 1;
	}
	//���̏�ɒN���������ԂŔ��̃g���b�v���N�����ꂽ�Ƃ�
	else if (player_bold(y, x) || cave[y][x].m_idx)
	{
		tx = x;
		ty = y;
	}
	//����ȊO�̂Ƃ��A���̋߂��̃v���C���[�⃂���X�^�[�������_���ɑI�肷��
	else
	{
		int tmp_dist = 255;
		//�܂����������l�ɂ���
		if (projectable(py, px, y, x))
		{
			tx = px;
			ty = py;
			tmp_dist = distance(py, px, y, x);
		}
		//����ȉ��̋����Ƀ����X�^�[������΂����炪�I�΂��(�����̊ȗ����̂���m_idx�����������X�^�[�D��)
		for (i = 0; i < m_max; i++)
		{
			int tmp_dist2;

			monster_type *m_ptr_tmp = &m_list[i];
			if (!m_ptr_tmp->r_idx) continue;

			tmp_dist2 = distance(y, x, m_ptr_tmp->fy, m_ptr_tmp->fx);
			if (tmp_dist2 > tmp_dist) continue; //�������Ȃ灗��胂���X�^�[�D��œ����邱�ƂɂȂ�
			if (!projectable(y, x, m_ptr_tmp->fy, m_ptr_tmp->fx)) continue;

			tmp_dist = tmp_dist2;
			tx = m_ptr_tmp->fx;
			ty = m_ptr_tmp->fy;
		}
		target_dist = tmp_dist;


	}

	//�����͈ȊO�̃����X�^�[���^�[�Q�b�g�ɂȂ��Ă���Ƃ������g���b�v�̏����t���O��ύX
	if (!trap_player && cave[ty][tx].m_idx && is_hostile(&m_list[cave[ty][tx].m_idx]))
	{
		summon_flag = (PM_ALLOW_GROUP | PM_FORCE_FRIENDLY);
	}

	//if (p_ptr->wizard) trap_type = CHEST_TRAP_FUSION; //�e�X�g�p

	switch (trap_type)
	{

	case CHEST_TRAP_LOSE_STR:
	case CHEST_TRAP_LOSE_CON:
	case CHEST_TRAP_LOSE_MAG:

		if (trap_type == CHEST_TRAP_LOSE_STR)
			typ = GF_DEC_ATK;
		else if (trap_type == CHEST_TRAP_LOSE_CON)
			typ = GF_DEC_DEF;
		else
			typ = GF_DEC_MAG;

		//�g���b�v�̎˒���ݒ肵�A���͈͓̔��Ƀ^�[�Q�b�g�����Ȃ���ΓK���Ɍ��߂�
		trap_range = 6;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 100 + randint1(chest_level);
		msg_print("�����珬���ȃ_�[�c�������o���ꂽ�B");
		//project()�̔��ˈʒu�����ϐ��Őݒ肵�Ďˏo����
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, typ, (PROJECT_REFLECTABLE | PROJECT_STOP | PROJECT_KILL | PROJECT_GRID), -1);
		break;

	case CHEST_TRAP_TELEPORTER:
	{
		trap_range = 5;
		msg_print("�������ƁI�e���|�[�^�[�I");
		//�܂����ɉe�����y�ڂ��Ȃ����Ӄ����X�^�[�e���|�[�g
		project(0, trap_range, y, x, 100, GF_AWAY_ALL, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);
		//�͈͓��́��������e���|�[�g�B�ǂ̒��ɏo�邱�Ƃ�����B
		if (distance(py, px, y, x) < trap_range) teleporter_trap();
		break;
	}

	case CHEST_TRAP_BA_POIS:
		dam = chest_level * 3;
		msg_print("������ΐF�̃K�X�������o�����I");
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_POIS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_BA_SLEEP:
		dam = 150 + chest_level * 3;
		msg_print("�����甒���K�X�������o�����I");
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_OLD_SLEEP, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_BA_CONF:
		dam = 150 + chest_level * 3;
		msg_print("�����炫��߂��K�X�������o�����I");
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_OLD_CONF, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_EXPLODE:
		dam = 150 + chest_level * 2;
		msg_print("�������ƁI���e�I");
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_SHARDS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		//���̔��A�C�e������
		floor_item_increase(o_idx, -1);
		floor_item_optimize(o_idx);
		lite_spot(y, x);

		break;

	case CHEST_TRAP_ALARM:
		dam = 150 + chest_level * 3;

#ifdef JP
		msg_print("�������܂��������苿�����I");
#else
		msg_print("An alarm sounds!");
#endif
		//�X�s�[�h�����X�^�[���Ȃ���
		aggravate_monsters(0, FALSE);
		project(PROJECT_WHO_TRAP, 2, y, x, dam, GF_STUN, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);

		break;

	case CHEST_TRAP_SUMMON:
	case CHEST_TRAP_S_BIRD:
	case CHEST_TRAP_S_ELEMENTAL:
	case CHEST_TRAP_S_DEMON: 
	case CHEST_TRAP_S_DRAGON:
	case CHEST_TRAP_S_CHIMERA:
	case CHEST_TRAP_S_VORTEX:
	case CHEST_TRAP_S_KWAI:
	{
		int summon_num;
		bool summon_done = FALSE;
		cptr summon_msg;
		int summon_lev;
		int summon_mode;

		switch (trap_type)
		{
		case CHEST_TRAP_S_KWAI:
			summon_num = 4 + randint1(4);
			summon_msg = "�Â�̒�����鳖��鲂����낼��N���o�Ă����I";
			summon_lev = chest_level;
			summon_mode = SUMMON_KWAI;
			break;

		case CHEST_TRAP_S_VORTEX:
			summon_num = 5 + randint1(5);
			summon_msg = "�Q�������̂��A�j�􂵂��I";
			summon_lev = chest_level / 2;
			summon_mode = SUMMON_VORTEX;
			break;



		case CHEST_TRAP_S_CHIMERA:
			summon_num = 3 + randint1(3);
			summon_msg = "��Ȏp�̉������P���ė����I";
			summon_lev = chest_level;
			summon_mode = SUMMON_HYBRID;
			break;

		case CHEST_TRAP_S_DRAGON:
			summon_num = 2 + randint1(2);
			summon_msg = "�ÈłɃh���S���̉e���ڂ���ƌ��ꂽ�I";
			summon_lev = chest_level;
			summon_mode = SUMMON_DRAGON;
			break;

		case CHEST_TRAP_S_DEMON:
			summon_num = 2 + randint1(2);
			summon_msg = "���Ɨ����̉_�̒��Ɉ������p���������I";
			summon_lev = chest_level;
			summon_mode = SUMMON_DEMON;
			break;

		case CHEST_TRAP_S_ELEMENTAL:
			summon_num = 3 + randint1(3);
			summon_msg = "�����邽�߂ɃG�������^�������ꂽ�I";
			summon_lev = chest_level;
			summon_mode = SUMMON_ELEMENTAL;
			break;

		case CHEST_TRAP_S_BIRD:
			summon_num = 5 + randint1(5);
			summon_msg = "�����璹�̌Q�ꂪ��яo�����I";
			summon_lev = chest_level / 2 + 1;
			summon_mode = SUMMON_BIRD;
			break;


		default:
			summon_num = 4 + randint1(4);
			summon_msg = "�����烂���X�^�[�����ꂽ�I";
			summon_lev = chest_level;
			summon_mode = 0;
			break;



		}

		for (i = 0; i < summon_num; i++)
		{
			if(summon_specific(0, y, x, summon_lev, summon_mode, summon_flag)) summon_done = TRUE;
		}
		if (summon_done)
		{
			msg_format("%s", summon_msg);

			if (summon_flag & PM_FORCE_FRIENDLY)
				msg_print("�����̃��[���̓^�[�Q�b�g����F�����悤���I");
		}
		else
		{
			msg_print("�����̃��[���͕s���������I");
		}

		break;
	}

	case CHEST_TRAP_SUIKI:
	{
		msg_print("�n�ʂ��璴�����̐����������o�����I");
		dam = chest_level * 4;

		project(PROJECT_WHO_TRAP, 2, y, x, dam, GF_WATER_FLOW, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_GRID), -1);
		project(PROJECT_WHO_TRAP, 2, y, x, dam, GF_WATER, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);


		if (trap_player)
		{
			msg_print("���Ȃ��͈�u�����𕕂���ꂽ�I");
			p_ptr->energy_need += ENERGY_NEED();
		}
		else if (cave[y][x].m_idx)
		{
			char            m_name[80];
			monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
			msg_format("%s�͋���ł���I",m_name);
			monster_delay(cave[y][x].m_idx, 100 + randint1(100));
			project(PROJECT_WHO_TRAP, 0, y, x, 25+chest_level/4, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_KILL), -1);


		}

		break;
	}


	case CHEST_TRAP_RUIN:

		if (trap_player)
		{
			/* Determine how many nasty tricks can be played. */
			int nasty_tricks_count = 4 + randint0(3);

			/* Message. */
#ifdef JP
			msg_print("���낵������������:  �u�Èł������܂�I�v");
#else
			msg_print("Hideous voices bid:  'Let the darkness have thee!'");
#endif

			/* This is gonna hurt... */
			for (; nasty_tricks_count > 0; nasty_tricks_count--)
			{
				/* ...but a high saving throw does help a little. */
				if (randint1(100 + chest_level) > p_ptr->skill_sav)
				{
#ifdef JP
					if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "�j�ł̃g���b�v�̕�", -1);
#else
					if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "a chest dispel-player trap", -1);
#endif
					else if (one_in_(5)) (void)set_cut(p_ptr->cut + 200);
					else if (one_in_(4))
					{
						if (!p_ptr->free_act)
							(void)set_paralyzed(p_ptr->paralyzed + 2 +
								randint0(6));
						else
							(void)set_stun(p_ptr->stun + 10 +
								randint0(100));
					}
					else if (one_in_(3)) apply_disenchant(0, 0);
					else if (one_in_(2))
					{
						(void)do_dec_stat(A_STR);
						(void)do_dec_stat(A_DEX);
						(void)do_dec_stat(A_CON);
						(void)do_dec_stat(A_INT);
						(void)do_dec_stat(A_WIS);
						(void)do_dec_stat(A_CHR);
					}
					else (void)fire_meteor(-1, GF_NETHER, y, x, 150, 1);
				}
			}
		}
		else if (target_dist <= 1)
		{
			msg_print("�׈��ȃ��[�������������I");
			project(0, 0, ty, tx, 100 + chest_level * 3, GF_DEC_ALL, (PROJECT_JUMP | PROJECT_KILL), -1);
			project(0, 0, ty, tx, 10 + chest_level / 2, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_KILL), -1);
			project(PROJECT_WHO_TRAP, 0, y, x, 100+chest_level*2, GF_HAND_DOOM, (PROJECT_KILL | PROJECT_HIDE | PROJECT_JUMP), -1);

		}
		else
		{
			msg_print("�׈��ȃ��[���������������s���ɏI������悤���B");

		}
		break;

	case CHEST_TRAP_SLINGSHOT:

		//�g���b�v�̎˒���ݒ肵�A���͈͓̔��Ƀ^�[�Q�b�g�����Ȃ���ΓK���Ɍ��߂�
		trap_range = 6;
		if(target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 10 + randint1(chest_level/2);
		msg_print("��������I�������o���ꂽ�I");
		//project()�̔��ˈʒu�����ϐ��Őݒ肵�Ďˏo����
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_ARROW, (PROJECT_STOP|PROJECT_KILL|PROJECT_GRID), -1);
		break;


	case CHEST_TRAP_ARROW:

		//�g���b�v�̎˒���ݒ肵�A���͈͓̔��Ƀ^�[�Q�b�g�����Ȃ���ΓK���Ɍ��߂�
		trap_range = 12;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 25 + randint1(chest_level);
		msg_print("�����������o���ꂽ�I");
		//project()�̔��ˈʒu�����ϐ��Őݒ肵�Ďˏo����
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_ARROW, (PROJECT_STOP | PROJECT_KILL | PROJECT_GRID), -1);
		break;

	case CHEST_TRAP_STEEL_ARROW:

		//�g���b�v�̎˒���ݒ肵�A���͈͓̔��Ƀ^�[�Q�b�g�����Ȃ���ΓK���Ɍ��߂�
		trap_range = 15;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = 50 + damroll(2,chest_level);
		msg_print("������|�S�̖�����o���ꂽ�I");
		//project()�̔��ˈʒu�����ϐ��Őݒ肵�Ďˏo����
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_ARROW, (PROJECT_STOP | PROJECT_KILL | PROJECT_GRID), -1);
		break;

	case CHEST_TRAP_PUNCH:
	{
		int	dice = chest_level / 10 + 4;
		int	sides = dice;
		dam = damroll(dice, sides);
		if (trap_player)
		{
			msg_print("�o�l�d�|���̃p���`���O�O���[�u�����Ȃ��̊�ʂɂ߂荞�񂾁I");
			take_hit(DAMAGE_NOESCAPE, dam, "�", -1);
			(void)set_stun(p_ptr->stun + 50 + randint0(50));
		}
		else
		{
			//�g���b�v�̎˒���ݒ肵�A���͈͓̔��Ƀ^�[�Q�b�g�����Ȃ���ΓK���Ɍ��߂�
			trap_range = 2;
			if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
			msg_print("������p���`���O�O���[�u�������o���ꂽ�I");
			//project()�̔��ˈʒu�����ϐ��Őݒ肵�Ďˏo����
			hack_project_start_x = x;
			hack_project_start_y = y;
			(void)project(PROJECT_WHO_TRAP, 0, ty, tx, dam, GF_FORCE, (PROJECT_KILL), -1);
		}
		break;
	}

	case CHEST_TRAP_BR_FIRE:
	case CHEST_TRAP_BR_ACID:
		//�g���b�v�̎˒���ݒ肵�A���͈͓̔��Ƀ^�[�Q�b�g�����Ȃ���ΓK���Ɍ��߂�
		trap_range = 8;
		if (target_dist > trap_range) scatter(&ty, &tx, y, x, trap_range, 0);
		dam = chest_level * 6;
		if (dam < 100) dam = 100;
		if (trap_type == CHEST_TRAP_BR_FIRE)
		{
			typ = GF_FIRE;
			msg_print("�Ή����˂��I");
		}
		else
		{
			typ = GF_ACID;
			msg_print("�����狭�_�������o�����I");
		}

		//project()�̔��ˈʒu�����ϐ��Őݒ肵�Ďˏo����
		hack_project_start_x = x;
		hack_project_start_y = y;
		(void)project(PROJECT_WHO_TRAP, -2, ty, tx, dam, typ, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER), -1);
		break;

	case CHEST_TRAP_BA_TIME:
		dam = chest_level * 2 + randint1(chest_level);
		msg_print("�t�ʎ蔠���I");
		project(PROJECT_WHO_TRAP, 5, y, x, dam, GF_TIME, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);

		break;


	case CHEST_TRAP_MIMIC:
		msg_print("�Ȃ�Ɣ��̓~�~�b�N�������I");
		//���̔��A�C�e������
		floor_item_increase(o_idx, -1);
		floor_item_optimize(o_idx);
		lite_spot(y, x);
		//�~�~�b�N����
		if (!summon_named_creature(0, y, x, MON_MIMIC_CHEST, 0L))
			msg_print("�������~�~�b�N�͂ǂ����֏������B");

		break;

	case CHEST_TRAP_MAGIC_DRAIN:
		trap_range = 5;
		dam = chest_level * 4;
		msg_print("���̖͂��͂����ɋz�����܂ꂽ�I");

		//���ɉe�����y�ڂ��Ȃ����@�͒ቺ�̃{�[��
		project(0, trap_range, y, x, dam, GF_DEC_MAG, (PROJECT_JUMP | PROJECT_KILL), -1);
		//�͈͓������X�^�[�ɖ��͏���
		for (i = 1; i<m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			if (!m_ptr->r_idx) continue;
			if (distance(m_ptr->fy, m_ptr->fx, y, x) > trap_range) continue;
			if (!projectable(m_ptr->fy, m_ptr->fx, y, x)) continue;

			dispel_monster_status(i);
		}

		//�����͈͓��Ȃ疂�͏�����MP�_���[�W
		if (distance(py, px, y, x) < trap_range)
		{
			dispel_player();
			p_ptr->csp -= dam;
			if (p_ptr->csp < 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				p_ptr->redraw |= (PR_MANA);
			}

		}

		break;

	case CHEST_TRAP_BA_BERSERK:
		dam = 150 + randint1(chest_level * 3);
		msg_print("������^���ԂȃK�X�������o�����I");
		project(PROJECT_WHO_TRAP, 4, y, x, dam, GF_BERSERK, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM), -1);
		break;

	case CHEST_TRAP_FUSION:
		msg_print("�������ƁI�j���[�N���A�t���[�W�����I");
		dam = chest_level * 8;
		if (dam < 300) dam = 300;

		//���̔��A�C�e������
		floor_item_increase(o_idx, -1);
		floor_item_optimize(o_idx);
		lite_spot(y, x);
		//���E���j�M�U��
		project_hack3(PROJECT_WHO_TRAP, y, x, GF_NUKE, 0, 0, dam);
		break;


	default:
		msg_format("ERROR:trap_type(%d)�̏�������`����Ă��Ȃ�",trap_type);
		break;
	}

	hack_project_start_x = 0;
	hack_project_start_y = 0;

}

/*
 * Chests have traps too.
 *
 * Exploding chest destroys contents (and traps).
 * Note that the chest itself is never destroyed.
 */
/*:::���̃g���b�v����������*/
//v1.1.97 ���̊֐���chest_trap()�ɒu��������
#if 0
static void chest_trap(int y, int x, s16b o_idx)
{
	int  i, trap;

	object_type *o_ptr = &o_list[o_idx];

	int mon_level = o_ptr->xtra3;

	/* Ignore disarmed chests */
	if (o_ptr->pval <= 0) return;

	/* Obtain the traps */
	/*:::tables.c��chest_traps[]��pval���Ƃ�㩂̎�ނ��L�q����Ă���Bint�����r�b�g�t���O�Ȃ̂�㩂��������邱�Ƃ�����*/
	trap = chest_traps[o_ptr->pval];

	//�t���������J���Ɏ��s����Ƃ悭��������
	if(p_ptr->pclass == CLASS_FLAN && one_in_(2)) trap = (CHEST_EXPLODE);

	if(SAKUYA_WORLD || world_player)
	{
		msg_print("����㩂͍쓮���Ȃ������B");
		return;
	}

	/* Lose strength */
	if (trap & (CHEST_LOSE_STR))
	{
#ifdef JP
		msg_print("�d�|�����Ă��������Ȑj�Ɏh����Ă��܂����I");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "�Őj", -1);
#else
		msg_print("A small needle has pricked you!");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "a poison needle", -1);
#endif

		(void)do_dec_stat(A_STR);
	}

	/* Lose constitution */
	if (trap & (CHEST_LOSE_CON))
	{
#ifdef JP
		msg_print("�d�|�����Ă��������Ȑj�Ɏh����Ă��܂����I");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "�Őj", -1);
#else
		msg_print("A small needle has pricked you!");
		take_hit(DAMAGE_NOESCAPE, damroll(1, 4), "a poison needle", -1);
#endif

		(void)do_dec_stat(A_CON);
	}

	/* Poison */
	if (trap & (CHEST_POISON))
	{
#ifdef JP
		msg_print("�˔@�����o�����ΐF�̃K�X�ɕ�ݍ��܂ꂽ�I");
#else
		msg_print("A puff of green gas surrounds you!");
#endif

		if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))
		{
			(void)set_poisoned(p_ptr->poisoned + 10 + randint1(20));
		}
		if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 3000);
	}

	/* Paralyze */
	if (trap & (CHEST_PARALYZE))
	{
#ifdef JP
		msg_print("�˔@�����o�������F���K�X�ɕ�ݍ��܂ꂽ�I");
#else
		msg_print("A puff of yellow gas surrounds you!");
#endif


		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(p_ptr->paralyzed + 10 + randint1(20));
		}
	}

	/* Summon monsters */
	if (trap & (CHEST_SUMMON))
	{
		int num = 2 + randint1(3);
#ifdef JP
		msg_print("�˔@�����o�������ɕ�ݍ��܂ꂽ�I");
#else
		msg_print("You are enveloped in a cloud of smoke!");
#endif


		for (i = 0; i < num; i++)
		{
			if (randint1(100)<dun_level)
				activate_hi_summon(py, px, FALSE);
			else
				(void)summon_specific(0, y, x, mon_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Elemental summon. */
	if (trap & (CHEST_E_SUMMON))
	{
#ifdef JP
		msg_print("�����邽�߂ɃG�������^�������ꂽ�I");
#else
		msg_print("Elemental beings appear to protect their treasures!");
#endif
		for (i = 0; i < randint1(3) + 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, SUMMON_ELEMENTAL, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Force clouds, then summon birds. */
	if (trap & (CHEST_BIRD_STORM))
	{
#ifdef JP
		msg_print("���̌Q�ꂪ���Ȃ�����芪�����I");
#else
		msg_print("A storm of birds swirls around you!");
#endif

		for (i = 0; i < randint1(3) + 3; i++)
			(void)fire_meteor(-1, GF_FORCE, y, x, o_ptr->pval / 5, 7);

		for (i = 0; i < randint1(5) + o_ptr->pval / 5; i++)
		{
			(void)summon_specific(0, y, x, mon_level, SUMMON_BIRD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}

	/* Various colorful summonings. */
	if (trap & (CHEST_H_SUMMON))
	{
		/* Summon demons. */
		if (one_in_(4))
		{
#ifdef JP
			msg_print("���Ɨ����̉_�̒��Ɉ������p���������I");
#else
			msg_print("Demons materialize in clouds of fire and brimstone!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)fire_meteor(-1, GF_FIRE, y, x, 10, 5);
				(void)summon_specific(0, y, x, mon_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon dragons. */
		else if (one_in_(3))
		{
#ifdef JP
			msg_print("�ÈłɃh���S���̉e���ڂ���ƌ��ꂽ�I");
#else
			msg_print("Draconic forms loom out of the darkness!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon hybrids. */
		else if (one_in_(2))
		{
#ifdef JP
			msg_print("��Ȏp�̉������P���ė����I");
#else
			msg_print("Creatures strange and twisted assault you!");
#endif

			for (i = 0; i < randint1(5) + 3; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_HYBRID, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}

		/* Summon vortices (scattered) */
		else
		{
#ifdef JP
			msg_print("�Q�������̂��A�j�􂵂��I");
#else
			msg_print("Vortices coalesce and wreak destruction!");
#endif

			for (i = 0; i < randint1(3) + 2; i++)
			{
				(void)summon_specific(0, y, x, mon_level, SUMMON_VORTEX, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
		}
	}

	/* Dispel player. */
	if ((trap & (CHEST_RUNES_OF_EVIL)) && o_ptr->k_idx)
	{
		/* Determine how many nasty tricks can be played. */
		int nasty_tricks_count = 4 + randint0(3);

		/* Message. */
#ifdef JP
		msg_print("���낵������������:  �u�Èł������܂�I�v");
#else
		msg_print("Hideous voices bid:  'Let the darkness have thee!'");
#endif

		/* This is gonna hurt... */
		for (; nasty_tricks_count > 0; nasty_tricks_count--)
		{
			/* ...but a high saving throw does help a little. */
			if (randint1(100+o_ptr->pval*2) > p_ptr->skill_sav)
			{
#ifdef JP
				if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "�j�ł̃g���b�v�̕�", -1);
#else
				if (one_in_(6)) take_hit(DAMAGE_NOESCAPE, damroll(5, 20), "a chest dispel-player trap", -1);
#endif
				else if (one_in_(5)) (void)set_cut(p_ptr->cut + 200);
				else if (one_in_(4))
				{
					if (!p_ptr->free_act) 
						(void)set_paralyzed(p_ptr->paralyzed + 2 + 
						randint0(6));
					else 
						(void)set_stun(p_ptr->stun + 10 + 
						randint0(100));
				}
				else if (one_in_(3)) apply_disenchant(0,0);
				else if (one_in_(2))
				{
					(void)do_dec_stat(A_STR);
					(void)do_dec_stat(A_DEX);
					(void)do_dec_stat(A_CON);
					(void)do_dec_stat(A_INT);
					(void)do_dec_stat(A_WIS);
					(void)do_dec_stat(A_CHR);
				}
				else (void)fire_meteor(-1, GF_NETHER, y, x, 150, 1);
			}
		}
	}

	/* Aggravate monsters. */
	if (trap & (CHEST_ALARM))
	{
#ifdef JP
		msg_print("�������܂��������苿�����I");
#else
		msg_print("An alarm sounds!");
#endif
		aggravate_monsters(0,TRUE);
	}

	/* Explode */
	/*:::�������B����pval��0�ɂȂ邱�Ƃł��̂��Ƃ�chest_death()�ŃA�C�e�����o�Ȃ�*/
	if ((trap & (CHEST_EXPLODE)) && o_ptr->k_idx)
	{
#ifdef JP
		msg_print("�ˑR�A�������������I");
		msg_print("���̒��̕��͂��ׂĕ��X�ɍӂ��U�����I");
#else
		msg_print("There is a sudden explosion!");
		msg_print("Everything inside the chest is destroyed!");
#endif

		o_ptr->pval = 0;
		sound(SOUND_EXPLODE);
#ifdef JP
		take_hit(DAMAGE_ATTACK, damroll(5, 8), "�������锠", -1);
#else
		take_hit(DAMAGE_ATTACK, damroll(5, 8), "an exploding chest", -1);
#endif

	}
	/* Scatter contents. */
	if ((trap & (CHEST_SCATTER)) && o_ptr->k_idx)
	{
#ifdef JP
		msg_print("�󔠂̒��g�̓_���W�������イ�ɎU�������I");
#else
		msg_print("The contents of the chest scatter all over the dungeon!");
#endif
		chest_death(TRUE, y, x, o_idx);
		o_ptr->pval = 0;
	}
}
#endif

/*
 * Attempt to open the given chest at the given location
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::�����J����B㩂��������甭������B*/
static bool do_cmd_open_chest(int y, int x, s16b o_idx)
{
	int i, j;

	bool flag = TRUE;

	bool more = FALSE;

	object_type *o_ptr = &o_list[o_idx];


	/* Take a turn */
	energy_use = 100;

	/* Attempt to unlock it */
	/*:::�����|�����Ă���i��㩂�����j�ꍇ����pval���v���X*/
	/*:::�����X�L����pval�ŊJ�����������v�Z�@�������������Ă�㩂͉�������Ȃ�*/
	if (o_ptr->pval > 0)
	{
		/* Assume locked, and thus not open */
		flag = FALSE;

		/* Get the "disarm" factor */
		i = p_ptr->skill_dis;

		/* Penalize some conditions */
		if (p_ptr->blind || no_lite()) i = i / 10;
		if (p_ptr->confused || p_ptr->image) i = i / 10;

		/* Extract the difficulty */
		j = i - o_ptr->pval;

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		/* Success -- May still have traps */
		if (randint0(100) < j)
		{
#ifdef JP
			msg_print("�����͂������B");
#else
			msg_print("You have picked the lock.");
#endif

			gain_exp(1);
			flag = TRUE;
		}

		/* Failure -- Keep trying */
		else
		{
			/* We may continue repeating */
			more = TRUE;
			if (flush_failure) flush();
#ifdef JP
			msg_print("�����͂����Ȃ������B");
#else
			msg_print("You failed to pick the lock.");
#endif

		}
	}

	/* Allowed to open */
	if (flag)
	{
		/* Apply chest traps, if any */
		/*:::����㩂�����Δ�������*/
		//v1.1.97 ���g���b�v�d�l�ύX
		activate_chest_trap(y, x, o_idx, TRUE);
		//chest_trap(y, x, o_idx);

		/* Let the Chest drop items */
		//v1.1.97 ����㩂ɂ���Ă��̔����j�󂳂��\��������̂ŏ������ǉ�
		if (!o_list[o_idx].k_idx)
		{
			if (cheat_peek) msg_print("(�����j�󂳂�Ă���)");
		}
		else
		{
			chest_death(FALSE, y, x, o_idx);
		}
	}

	/* Result */
	return (more);
}


#if defined(ALLOW_EASY_OPEN) || defined(ALLOW_EASY_DISARM) /* TNB */

/*
 * Return TRUE if the given feature is an open door
 */
static bool is_open(int feat)
{
	return have_flag(f_info[feat].flags, FF_CLOSE) && (feat != feat_state(feat, FF_CLOSE));
}



/*
 * Convert an adjacent location to a direction.
 */
static int coords_to_dir(int y, int x)
{
	int d[3][3] = { {7, 4, 1}, {8, 5, 2}, {9, 6, 3} };
	int dy, dx;

	dy = y - py;
	dx = x - px;

	/* Paranoia */
	if (ABS(dx) > 1 || ABS(dy) > 1) return (0);

	return d[dx + 1][dy + 1];
}

#endif /* defined(ALLOW_EASY_OPEN) || defined(ALLOW_EASY_DISARM) -- TNB */


/*
 * Perform the basic "open" command on doors
 *
 * Assume destination is a closed/locked/jammed door
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::�h�A���J����or�J������*/
static bool do_cmd_open_aux(int y, int x)
{
	int i, j;

	/* Get requested grid */
	cave_type *c_ptr = &cave[y][x];

	feature_type *f_ptr = &f_info[c_ptr->feat];

	bool more = FALSE;


	/* Take a turn */
	energy_use = 100;

	/* Seeing true feature code (ignore mimic) */

	/* Jammed door */
	if (!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		///mod140829 �t�����̓h�A��ⓚ���p�ŉ�
		if(p_ptr->pclass == CLASS_FLAN)
		{
			cave_alter_feat(y, x, FF_BASH);
			msg_format("%s�͂�������ƕ����Ă����悤���B", f_name + f_info[get_feat_mimic(c_ptr)].name);
		}
		else msg_format("%s�͂�������ƕ����Ă���悤���B", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("The %s appears to be stuck.", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif


	}

	/* Locked door */
	else if (f_ptr->power)
	{
		/* Disarm factor */
		i = p_ptr->skill_dis;

		/* Penalize some conditions */
		if (p_ptr->blind || no_lite()) i = i / 10;
		if (p_ptr->confused || p_ptr->image) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		/* Flan */
		if(p_ptr->pclass == CLASS_FLAN)
		{
			msg_format("%s�Ɍ����������Ă����悤���B", f_name + f_info[get_feat_mimic(c_ptr)].name);
			cave_alter_feat(y, x, FF_BASH);
		}
		/* Success */
		else if (randint0(100) < j)
		{
			/* Message */
#ifdef JP
			msg_print("�����͂������B");
#else
			msg_print("You have picked the lock.");
#endif

			/* Open the door */
			cave_alter_feat(y, x, FF_OPEN);

			/* Sound */
			sound(SOUND_OPENDOOR);

			/* Experience */
			gain_exp(1);
		}

		/* Failure */
		else
		{
			/* Failure */
			if (flush_failure) flush();

			/* Message */
#ifdef JP
			msg_print("�����͂����Ȃ������B");
#else
			msg_print("You failed to pick the lock.");
#endif


			/* We may keep trying */
			more = TRUE;
		}
	}

	/* Closed door */
	else
	{
		/* Open the door */
		cave_alter_feat(y, x, FF_OPEN);

		/* Sound */
		sound(SOUND_OPENDOOR);
	}

	/* Result */
	return (more);
}



/*
 * Open a closed/locked/jammed door or a closed/locked chest.
 *
 * Unlocking a locked door/chest is worth one experience point.
 */
/*:::�h�A�������J����*/
void do_cmd_open(void)
{
	int y, x, dir;

	s16b o_idx;

	bool more = FALSE;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

#ifdef ALLOW_EASY_OPEN /* TNB */

	/* Option: Pick a direction */
	/*:::easy_open�̂Ƃ��K���Ȉ������Ύ����I�Ƀ^�[�Q�b�g�Ƃ��đI������*/
	if (easy_open)
	{
		int num_doors, num_chests;

		/* Count closed doors (locked or jammed) */
		/*:::���̎��ӂ̕����h�A�̐��𐔂���*/
		num_doors = count_dt(&y, &x, is_closed_door, FALSE);

		/* Count chests (locked) */
		/*:::���̎��ӂ̔��̂���O���b�h�̐��𐔂���*/
		num_chests = count_chests(&y, &x, FALSE);

		/* See if only one target */
		if (num_doors || num_chests)
		{
			bool too_many = (num_doors && num_chests) || (num_doors > 1) ||
			    (num_chests > 1);
			if (!too_many) command_dir = coords_to_dir(y, x);
		}
	}

#endif /* ALLOW_EASY_OPEN -- TNB */

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir, TRUE))
	{
		s16b feat;
		cave_type *c_ptr;

		/* Get requested location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get requested grid */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Check for chest */
		o_idx = chest_check(y, x, FALSE);

		/* Nothing useful */
		if (!have_flag(f_info[feat].flags, FF_OPEN) && !o_idx)
		{
			/* Message */
#ifdef JP
		msg_print("�����ɂ͊J������̂���������Ȃ��B");
#else
			msg_print("You see nothing there to open.");
#endif

		}

		/* Monster in the way */
		else if (c_ptr->m_idx && p_ptr->riding != c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
		msg_print("�����X�^�[�������ӂ������Ă���I");
#else
			msg_print("There is a monster in the way!");
#endif


			/* Attack */
			py_attack(y, x, 0);
		}

		/* Handle chests */
		else if (o_idx)
		{
			/* Open the chest */
			more = do_cmd_open_chest(y, x, o_idx);
		}

		/* Handle doors */
		else
		{
			/* Open the door */
			more = do_cmd_open_aux(y, x);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(0, 0);
}



/*
 * Perform the basic "close" command
 *
 * Assume destination is an open/broken door
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::�h�A�����*/
static bool do_cmd_close_aux(int y, int x)
{
	/* Get grid and contents */
	cave_type *c_ptr = &cave[y][x];
	s16b      old_feat = c_ptr->feat;
	bool      more = FALSE;

	/* Take a turn */
	energy_use = 100;

	/* Seeing true feature code (ignore mimic) */

	/* Open door */
	if (have_flag(f_info[old_feat].flags, FF_CLOSE))
	{
		s16b closed_feat = feat_state(old_feat, FF_CLOSE);

		/* Hack -- object in the way */
		/*:::�����ǂ������B�J�����h�A�̏ꏊ�ɃA�C�e�����Ƃ����胋�[���~������ł��Ȃ��͂�����*/
		if ((c_ptr->o_idx || (c_ptr->info & CAVE_OBJECT)) &&
		    (closed_feat != old_feat) && !have_flag(f_info[closed_feat].flags, FF_DROP))
		{
			/* Message */
#ifdef JP
			msg_print("�������������ĕ܂�Ȃ��B");
#else
			msg_print("There seems stuck.");
#endif
		}
		else
		{
			/* Close the door */
			cave_alter_feat(y, x, FF_CLOSE);

			/* Broken door */
			if (old_feat == c_ptr->feat)
			{
				/* Message */
#ifdef JP
				msg_print("�h�A�͉��Ă��܂��Ă���B");
#else
				msg_print("The door appears to be broken.");
#endif
			}
			else
			{
				/* Sound */
				sound(SOUND_SHUTDOOR);
			}
		}
	}

	/* Result */
	return (more);
}


/*
 * Close an open door.
 */
/*:::�h�A��߂�*/
void do_cmd_close(void)
{
	int y, x, dir;

	bool more = FALSE;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

#ifdef ALLOW_EASY_OPEN /* TNB */

	/* Option: Pick a direction */
	if (easy_open)
	{
		/* Count open doors */
		if (count_dt(&y, &x, is_open, FALSE) == 1)
		{
			command_dir = coords_to_dir(y, x);
		}
	}

#endif /* ALLOW_EASY_OPEN -- TNB */

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		cave_type *c_ptr;
		s16b feat;

		/* Get requested location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require open/broken door */
		if (!have_flag(f_info[feat].flags, FF_CLOSE))
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͕�����̂���������Ȃ��B");
#else
			msg_print("You see nothing there to close.");
#endif
		}

		/* Monster in the way */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�����X�^�[�������ӂ������Ă���I");
#else
			msg_print("There is a monster in the way!");
#endif

			/* Attack */
			py_attack(y, x, 0);
		}

		/* Close the door */
		else
		{
			/* Close the door */
			more = do_cmd_close_aux(y, x);
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(0, 0);
}


/*
 * Determine if a given grid may be "tunneled"
 */
/*:::�����Ȃ��n�`�ƌ@��Ȃ��n�`��FALSE��Ԃ��A�@���n�`��TRUE��Ԃ�*/
static bool do_cmd_tunnel_test(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	/* Must have knowledge */
	if (!(c_ptr->info & CAVE_MARK))
	{
		/* Message */
#ifdef JP
		msg_print("�����ɂ͉�����������Ȃ��B");
#else
		msg_print("You see nothing there.");
#endif

		/* Nope */
		return (FALSE);
	}

	/* Must be a wall/door/etc */
	if (!cave_have_flag_grid(c_ptr, FF_TUNNEL))
	{
		/* Message */
#ifdef JP
		msg_print("�����ɂ͌@����̂���������Ȃ��B");
#else
		msg_print("You see nothing there to tunnel.");
#endif

		/* Nope */
		return (FALSE);
	}

	/* Okay */
	return (TRUE);
}


/*
 * Perform the basic "tunnel" command
 *
 * Assumes that no monster is blocking the destination
 *
 * Do not use twall anymore
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::�g���l�����@���Ɓ@���s�[�g���̂Ƃ�TRUE,�@��I���/�@��Ȃ��Ƃ�FALSE��Ԃ�*/
static bool do_cmd_tunnel_aux(int y, int x)
{
	cave_type *c_ptr;
	feature_type *f_ptr, *mimic_f_ptr;
	int power;
	cptr name;
	bool more = FALSE;

	/* Verify legality */
	/*:::�@���n�`���ǂ�������*/
	if (!do_cmd_tunnel_test(y, x)) return (FALSE);

	/* Take a turn */
	energy_use = 100;

	/* Get grid */
	/*:::f_info[]����power�𓾂�@mimic�łȂ�����*/
	c_ptr = &cave[y][x];
	f_ptr = &f_info[c_ptr->feat];
	power = f_ptr->power;

	//�t�����͂����ɕǂ��@���
	//�E�V�ƓV�q��
	//�ݕP��
	if(p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_TENSHI || p_ptr->pclass == CLASS_KEIKI) power = 0;

	/* Feature code (applying "mimic" field) */
	mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];
	/*:::f_info[get_feat_mimic(c_ptr)]����n�`���𓾂�*/
	name = f_name + mimic_f_ptr->name;

	/* Sound */
	sound(SOUND_DIG);
	/*:::�i�v�ǂ͌@��Ȃ�*/
	if (have_flag(f_ptr->flags, FF_PERMANENT))
	{
		/* Titanium */
		if (have_flag(mimic_f_ptr->flags, FF_PERMANENT))
		{
#ifdef JP
			msg_print("���̊�͍d�����Č@��Ȃ��悤���B");
#else
			msg_print("This seems to be permanent rock.");
#endif
		}

		/* Map border (mimiccing Permanent wall) */
		else
		{
#ifdef JP
			msg_print("�����͌@��Ȃ�!");
#else
			msg_print("You can't tunnel through that!");
#endif
		}
	}

	/* Dig or tunnel */
	/*:::�n�`�t���O��power��p_ptr->skill_dig�Ō��@�蔻��B*/
	/*:::�؂�ǂ̏ꍇpower+randint(40*power)���@��Z�\�l�������Ό��@�萬��*/
	/*:::���@��ɐ���������n�`��n�ʂɂ��AFALSE��Ԃ�*/
	/*:::�@��Ȃ�������TRUE��Ԃ�*/
	/*:::�B���h�A���@���Ă���Ƃ�1/4�Œn�`���m����*/
	///sys skill �@��Z�\�֘A
	else if (have_flag(f_ptr->flags, FF_CAN_DIG))
	{
		/* Dig */
		if (p_ptr->skill_dig > randint0(20 * power))
		{
			/* Message */
#ifdef JP
			msg_format("%s�����������B", name);
#else
			msg_format("You have removed the %s.", name);
#endif

			/* Remove the feature */
			cave_alter_feat(y, x, FF_TUNNEL);

			/* Update some things */
			p_ptr->update |= (PU_FLOW);
		}
		else
		{
			/* Message, keep digging */
#ifdef JP
			msg_format("%s���������Ă���B", name);
#else
			msg_format("You dig into the %s.", name);
#endif

			more = TRUE;
		}
	}

	else
	{
		bool tree = have_flag(mimic_f_ptr->flags, FF_TREE);

		/* Tunnel */
		if (p_ptr->skill_dig > power + randint0(40 * power))
		{
#ifdef JP
			if (tree) msg_format("%s��؂蕥�����B", name);
			else
			{

				if(difficulty == DIFFICULTY_LUNATIC && one_in_(100))
				{
					msg_print("���Ղ��I");
					earthquake(y, x, 3 + randint1(3));

				}
				else 
				{
					if(p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_YUGI)
						msg_print("��u�ŕǂɑ匊���J�����I");
					else if(p_ptr->pclass == CLASS_TENSHI || p_ptr->pclass == CLASS_KEIKI)
						msg_print("�ǂ��������񂾁B");
					else 
						msg_print("�����@��I�����B");
				}
				p_ptr->update |= (PU_FLOW);
			}
#else
			if (tree) msg_format("You have cleared away the %s.", name);
			else
			{
				msg_print("You have finished the tunnel.");
				p_ptr->update |= (PU_FLOW);
			}
#endif

			/* Sound */
			if (have_flag(f_ptr->flags, FF_GLASS)) sound(SOUND_GLASS);

			/* Remove the feature */
			cave_alter_feat(y, x, FF_TUNNEL);

			chg_virtue(V_DILIGENCE, 1);
			chg_virtue(V_NATURE, -1);
		}

		/* Keep trying */
		else
		{
			if (tree)
			{
				/* We may continue chopping */
#ifdef JP
				msg_format("%s��؂��Ă���B", name);
#else
				msg_format("You chop away at the %s.", name);
#endif
				/* Occasional Search XXX XXX */
				if (randint0(100) < 25) search();
			}
			else
			{
				/* We may continue tunelling */
#ifdef JP
				msg_format("%s�Ɍ����@���Ă���B", name);
#else
				msg_format("You tunnel into the %s.", name);
#endif
			}

			more = TRUE;
		}
	}

	if (is_hidden_door(c_ptr))
	{
		/* Occasional Search XXX XXX */
		if (randint0(100) < 25) search();
	}

	/* Result */
	return more;
}


/*
 * Tunnels through "walls" (including rubble and closed doors)
 *
 * Note that you must tunnel in order to hit invisible monsters
 * in walls, though moving into walls still takes a turn anyway.
 *
 * Digging is very difficult without a "digger" weapon, but can be
 * accomplished by strong players using heavy weapons.
 */
/*:::�g���l�����@��B��ƕ����h�A���܂܂��*/
/*:::�ǂ̒��ɂ��錩���Ȃ������X�^�[�ɂ������I�ɍU������*/
void do_cmd_tunnel(void)
{
	int			y, x, dir;

	cave_type	*c_ptr;
	s16b feat;

	bool		more = FALSE;


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Allow repeated command */
	/*:::���s�[�g������*/
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}
	//��\��o�����t���f
	stop_tsukumo_music();

	/* Get a direction to tunnel, or Abort */
	/*:::�@���������͂�����*/
	if (get_rep_dir(&dir,FALSE))
	{
		/* Get location */
		/*:::���̂���ꏊ����ddx[],ddy[]���Q�Ƃ��@��ꏊ�̃}�X���v�Z*/
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid */

		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		/*:::�Ώۃ}�X��cave[][]����n�`�f�[�^�𓾂�B*/
		/*:::(c_ptr->mimic�ɐ��l������Ȃ炻�̐��l�A�Ȃ����c_ptr->feat�̐��l��f_info[].mimic�𓾂Ă���)*/
		feat = get_feat_mimic(c_ptr);

		/* No tunnelling through doors */
/*Q019 �ic_ptr����mimic���������ē���ꂽ�l���炳���f_info[].mimic�𓾂Ă��̐��l��f_info[]���Q�Ƃ��Ă���B�Ȃ�ł���ȉ�肭�ǂ��������H�j*/
		if (have_flag(f_info[feat].flags, FF_DOOR))
		{
			/* Message */
#ifdef JP
			msg_print("�h�A�͌@��Ȃ��B");
#else
			msg_print("You cannot tunnel through doors.");
#endif
		}

		/* No tunnelling through most features */
		else if (!have_flag(f_info[feat].flags, FF_TUNNEL))
		{
#ifdef JP
			msg_print("�����͌@��Ȃ��B");
#else
			msg_print("You can't tunnel through that.");
#endif
		}

		/* A monster is in the way */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�����X�^�[�������ӂ������Ă���I");
#else
			msg_print("There is a monster in the way!");
#endif

			/* Attack */
			py_attack(y, x, 0);
		}

		/* Try digging */
		else
		{
			/* Tunnel through walls */
			/*:::���ۂɃg���l�����@����*/
			more = do_cmd_tunnel_aux(y, x);
		}
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(0, 0);
}


#ifdef ALLOW_EASY_OPEN /* TNB */

/*
 * easy_open_door --
 *
 *	If there is a jammed/closed/locked door at the given location,
 *	then attempt to unlock/open it. Return TRUE if an attempt was
 *	made (successful or not), otherwise return FALSE.
 *
 *	The code here should be nearly identical to that in
 *	do_cmd_open_test() and do_cmd_open_aux().
 */
bool easy_open_door(int y, int x)
{
	int i, j;

	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Must be a closed door */
	if (!is_closed_door(c_ptr->feat))
	{
		/* Nope */
		return (FALSE);
	}

	//v1.1.68 �V�n�`�u�X��v
	if (have_flag(f_ptr->flags, FF_ICE_WALL))
		return (FALSE);


	/* Jammed door */
	if (!have_flag(f_ptr->flags, FF_OPEN))
	{
		/* Stuck */
#ifdef JP
		if(p_ptr->pclass == CLASS_FLAN)
		{
			cave_alter_feat(y, x, FF_BASH);
			msg_format("%s�͂�������ƕ����Ă����悤���B", f_name + f_info[get_feat_mimic(c_ptr)].name);
		}
		else	msg_format("%s�͂�������ƕ����Ă���悤���B", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("The %s appears to be stuck.", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif

	}

	/* Locked door */
	else if (f_ptr->power)
	{
		/* Disarm factor */
		i = p_ptr->skill_dis;

		/* Penalize some conditions */
		if (p_ptr->blind || no_lite()) i = i / 10;
		if (p_ptr->confused || p_ptr->image) i = i / 10;

		/* Extract the lock power */
		j = f_ptr->power;

		/* Extract the difficulty XXX XXX XXX */
		j = i - (j * 4);

		/* Always have a small chance of success */
		if (j < 2) j = 2;

		if(p_ptr->pclass == CLASS_FLAN)
		{
			msg_format("%s�Ɍ����������Ă����悤���B", f_name + f_info[get_feat_mimic(c_ptr)].name);
			cave_alter_feat(y, x, FF_BASH);
		}
		/* Success */
		else if (randint0(100) < j)
		{
			/* Message */
#ifdef JP
			msg_print("�����͂������B");
#else
			msg_print("You have picked the lock.");
#endif

			/* Open the door */
			cave_alter_feat(y, x, FF_OPEN);

			/* Sound */
			sound(SOUND_OPENDOOR);

			/* Experience */
			gain_exp(1);
		}

		/* Failure */
		else
		{
			/* Failure */
			if (flush_failure) flush();

			/* Message */
#ifdef JP
			msg_print("�����͂����Ȃ������B");
#else
			msg_print("You failed to pick the lock.");
#endif

		}
	}

	/* Closed door */
	else
	{
		/* Open the door */
		cave_alter_feat(y, x, FF_OPEN);

		/* Sound */
		sound(SOUND_OPENDOOR);
	}

	/* Result */
	return (TRUE);
}

#endif /* ALLOW_EASY_OPEN -- TNB */


/*
 * Perform the basic "disarm" command
 *
 * Assume destination is a visible trap
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::���̃g���b�v����������*/
static bool do_cmd_disarm_chest(int y, int x, s16b o_idx)
{
	int i, j;

	bool more = FALSE;

	object_type *o_ptr = &o_list[o_idx];


	/* Take a turn */
	energy_use = 100;

	/* Get the "disarm" factor */
	/*:::�����Z�\�Ɣ���pval�Ő��������Z�o*/
	i = p_ptr->skill_dis;

	/* Penalize some conditions */
	if (p_ptr->blind || no_lite()) i = i / 10;
	if (p_ptr->confused || p_ptr->image) i = i / 10;

	/* Extract the difficulty */
	j = i - o_ptr->pval;

	/* Always have a small chance of success */
	if (j < 2) j = 2;

	/* Must find the trap first. */
	if (!object_is_known(o_ptr))
	{
#ifdef JP
		msg_print("�g���b�v����������Ȃ��B");
#else
		msg_print("I don't see any traps.");
#endif

	}

	/* Already disarmed/unlocked */
	else if (o_ptr->pval <= 0)
	{
#ifdef JP
		msg_print("���ɂ̓g���b�v���d�|�����Ă��Ȃ��B");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* No traps to find. */
	else if (!chest_traps[o_ptr->pval])
	{
#ifdef JP
		msg_print("���ɂ̓g���b�v���d�|�����Ă��Ȃ��B");
#else
		msg_print("The chest is not trapped.");
#endif

	}

	/* Success (get a lot of experience) */
	/*:::�ʂ�����o_ptr->pval�𕉂ɂ���(㩉����ς�)*/
	else if (randint0(100) < j)
	{
#ifdef JP
		msg_print("���Ɏd�|�����Ă����g���b�v�����������B");
#else
		msg_print("You have disarmed the chest.");
#endif

		gain_exp(o_ptr->pval);
		o_ptr->pval = (0 - o_ptr->pval);
	}

	/* Failure -- Keep trying */
	else if ((i > 5) && (randint1(i) > 5))
	{
		/* We may keep trying */
		more = TRUE;
		if (flush_failure) flush();
#ifdef JP
		msg_print("���̃g���b�v�����Ɏ��s�����B");
#else
		msg_print("You failed to disarm the chest.");
#endif

	}

	/* Failure -- Set off the trap */
	/*:::���s������A�������ƃ����_���l��㩔������肵�g���b�v�쓮*/
	else
	{
#ifdef JP
		msg_print("�g���b�v���쓮�����Ă��܂����I");
#else
		msg_print("You set off a trap!");
#endif

		sound(SOUND_FAIL);
		//v1.1.97 ���g���b�v�d�l�ύX
		activate_chest_trap(y, x, o_idx, TRUE);
		//chest_trap(y, x, o_idx);
	}

	/* Result */
	return (more);
}


/*
 * Perform the basic "disarm" command
 *
 * Assume destination is a visible trap
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::����̃g���b�v����������B���̃g���b�v�Ƃقړ����B㩂̔����͉�������ĂȂ���Ԃňړ����邱�Ƃōs����*/
#ifdef ALLOW_EASY_DISARM /* TNB */

bool do_cmd_disarm_aux(int y, int x, int dir)

#else /* ALLOW_EASY_DISARM -- TNB */

static bool do_cmd_disarm_aux(int y, int x, int dir)

#endif /* ALLOW_EASY_DISARM -- TNB */
{
	/* Get grid and contents */
	cave_type *c_ptr = &cave[y][x];

	/* Get feature */
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Access trap name */
	cptr name = (f_name + f_ptr->name);

	/* Extract trap "power" */
	int power = f_ptr->power;

	bool more = FALSE;

	/* Get the "disarm" factor */
	int i = p_ptr->skill_dis;

	int j;

	/* Take a turn */
	energy_use = 100;

	/* Penalize some conditions */
	if (p_ptr->blind || no_lite()) i = i / 10;
	if (p_ptr->confused || p_ptr->image) i = i / 10;

	/* Extract the difficulty */
	j = i - power;

	/* Always have a small chance of success */
	if (j < 2) j = 2;

	if(p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_FLAN && p_ptr->lev > 29)  j=100; //��ɐ���
	else if(p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		if(randint1(p_ptr->lev) > 10) j = 100;
		else i=0;
	}

	/* Success */
	if (randint0(100) < j)
	{
		/* Message */
#ifdef JP
		if(p_ptr->pclass == CLASS_FLAN || p_ptr->pseikaku == SEIKAKU_BERSERK)
			msg_format("%s��j�󂵂��B", name);
		else if(p_ptr->pclass == CLASS_YUGI)
			msg_format("%s�𓥂ݒׂ����B", name);
		else
			msg_format("%s�����������B", name);
#else
		msg_format("You have disarmed the %s.", name);
#endif

		/* Reward */
		gain_exp(power);

		/* Remove the trap */
		cave_alter_feat(y, x, FF_DISARM);

#ifdef ALLOW_EASY_DISARM /* TNB */

		/* Move the player onto the trap */
		move_player(dir, easy_disarm, FALSE,FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

		/* move the player onto the trap grid */
		move_player(dir, FALSE, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */
	}

	/* Failure -- Keep trying */
	else if ((i > 5) && (randint1(i) > 5))
	{
		/* Failure */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_format("%s�̉����Ɏ��s�����B", name);
#else
		msg_format("You failed to disarm the %s.", name);
#endif

		/* We may keep trying */
		more = TRUE;
	}

	/* Failure -- Set off the trap */
	else
	{
		/* Message */
#ifdef JP
		msg_format("%s���쓮�����Ă��܂����I", name);
#else
		msg_format("You set off the %s!", name);
#endif

#ifdef ALLOW_EASY_DISARM /* TNB */

		/* Move the player onto the trap */
		move_player(dir, easy_disarm, FALSE,FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

		/* Move the player onto the trap */
		move_player(dir, FALSE, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */
	}

	/* Result */
	return (more);
}


/*
 * Disarms a trap, or chest
 */
/*:::㩂���������*/
void do_cmd_disarm(void)
{
	int y, x, dir;

	s16b o_idx;

	bool more = FALSE;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

#ifdef ALLOW_EASY_DISARM /* TNB */

	/* Option: Pick a direction */
	if (easy_disarm)
	{
		int num_traps, num_chests;

		/* Count visible traps */
		num_traps = count_dt(&y, &x, is_trap, TRUE);

		/* Count chests (trapped) */
		num_chests = count_chests(&y, &x, TRUE);

		/* See if only one target */
		if (num_traps || num_chests)
		{
			bool too_many = (num_traps && num_chests) || (num_traps > 1) ||
			    (num_chests > 1);
			if (!too_many) command_dir = coords_to_dir(y, x);
		}
	}

#endif /* ALLOW_EASY_DISARM -- TNB */

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction (or abort) */
	if (get_rep_dir(&dir,TRUE))
	{
		cave_type *c_ptr;
		s16b feat;

		/* Get location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Check for chests */
		o_idx = chest_check(y, x, TRUE);

		/* Disarm a trap */
		if (!is_trap(feat) && !o_idx)
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͉���������̂���������Ȃ��B");
#else
			msg_print("You see nothing there to disarm.");
#endif

		}

		/* Monster in the way */
		else if (c_ptr->m_idx && p_ptr->riding != c_ptr->m_idx)
		{
			/* Message */
#ifdef JP
			msg_print("�����X�^�[�������ӂ������Ă���I");
#else
			msg_print("There is a monster in the way!");
#endif


			/* Attack */
			py_attack(y, x, 0);
		}

		/* Disarm chest */
		else if (o_idx)
		{
			/* Disarm the chest */
			more = do_cmd_disarm_chest(y, x, o_idx);
		}

		/* Disarm trap */
		else
		{
			/* Disarm the trap */
			more = do_cmd_disarm_aux(y, x, dir);
		}
	}

	/* Cancel repeat unless told not to */
	if (!more) disturb(0, 0);
}


/*
 * Perform the basic "bash" command
 *
 * Assume destination is a closed/locked/jammed door
 *
 * Assume there is no monster blocking the destination
 *
 * Returns TRUE if repeated commands may continue
 */
/*:::�̓����� �h�A�̒��ɓG�����Ȃ�����*/
static bool do_cmd_bash_aux(int y, int x, int dir)
{
	/* Get grid */
	cave_type	*c_ptr = &cave[y][x];

	/* Get feature */
	feature_type *f_ptr = &f_info[c_ptr->feat];

	/* Hack -- Bash power based on strength */
	/* (Ranges from 3 to 20 to 100 to 200) */
	int bash = adj_str_blow[p_ptr->stat_ind[A_STR]];

	/* Extract door power */
	int temp = f_ptr->power;

	bool		more = FALSE;

	cptr name = f_name + f_info[get_feat_mimic(c_ptr)].name;

	/* Take a turn */
	energy_use = 100;

	/* Message */
#ifdef JP
	if(p_ptr->pclass != CLASS_YUGI)
		msg_format("%s�ɑ̓�����������I", name);

#else
	msg_format("You smash into the %s!", name);
#endif

	/* Compare bash power to door power XXX XXX XXX */
	temp = (bash - (temp * 10));
	///class ����m�̓h�A��@���󂵂₷��
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) temp *= 2;

	/* Hack -- always have a chance */
	if (temp < 1) temp = 1;

	if(p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_SAKI) temp = 100; //�K����������B

	/* Hack -- attempt to bash down the door */
	if (randint0(100) < temp)
	{

		/* Sound */
		sound(have_flag(f_ptr->flags, FF_GLASS) ? SOUND_GLASS : SOUND_OPENDOOR);

		if(p_ptr->pclass == CLASS_YUGI || p_ptr->pclass == CLASS_SAKI)
		{
			msg_format("%s���R��󂵂��B", name);
			cave_alter_feat(y, x, FF_BASH);
		}

		/* Break down the door */
		else if ((randint0(100) < 50) || (feat_state(c_ptr->feat, FF_OPEN) == c_ptr->feat) || have_flag(f_ptr->flags, FF_GLASS))
		{
			msg_format("%s����ꂽ�I", name);
			cave_alter_feat(y, x, FF_BASH);
		}

		/* Open the door */
		else
		{
			msg_format("%s���J�����I", name);
			cave_alter_feat(y, x, FF_OPEN);
		}

		/* Hack -- Fall through the door */
		move_player(dir, FALSE, FALSE,FALSE);
	}

	/* Saving throw against stun */
	else if (randint0(100) < adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
		 p_ptr->lev)
	{
		/* Message */
#ifdef JP
		msg_format("����%s�͊�䂾�B", name);
#else
		msg_format("The %s holds firm.", name);
#endif


		/* Allow repeated bashing */
		more = TRUE;
	}

	/* High dexterity yields coolness */
	else
	{
		/* Message */
#ifdef JP
		///msg131213
		//msg_print("�̂̃o�����X���������Ă��܂����B");
		msg_print("�h�A�m�u���e���ɓ˂��h�������I");
#else
		msg_print("You are off-balance.");
#endif


		/* Hack -- Lose balance ala paralysis */
		(void)set_paralyzed(p_ptr->paralyzed + 2 + randint0(2));
	}

	/* Result */
	return (more);
}


/*
 * Bash open a door, success based on character strength
 *
 * For a closed door, pval is positive if locked; negative if stuck.
 *
 * For an open door, pval is positive for a broken door.
 *
 * A closed door can be opened - harder if locked. Any door might be
 * bashed open (and thereby broken). Bashing a door is (potentially)
 * faster! You move into the door way. To open a stuck door, it must
 * be bashed. A closed door can be jammed (see do_cmd_spike()).
 *
 * Creatures can also open or bash doors, see elsewhere.
 */
/*:::�����h�A��j�󂷂�*/
void do_cmd_bash(void)
{
	int			y, x, dir;

	cave_type	*c_ptr;

	bool		more = FALSE;


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		s16b feat;

		/* Bash location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Nothing useful */
		if (!have_flag(f_info[feat].flags, FF_BASH))
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͑̓����肷����̂���������Ȃ��B");
#else
			msg_print("You see nothing there to bash.");
#endif

		}

		/* Monster in the way */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�����X�^�[�������ӂ������Ă���I");
#else
			msg_print("There is a monster in the way!");
#endif


			/* Attack */
			py_attack(y, x, 0);
		}

		/* Bash a closed door */
		else
		{
			/* Bash the door */
			more = do_cmd_bash_aux(y, x, dir);
		}
	}

	/* Unless valid action taken, cancel bash */
	if (!more) disturb(0, 0);
}


/*
 * Manipulate an adjacent grid in some way
 *
 * Attack monsters, tunnel through walls, disarm traps, open doors.
 *
 * Consider confusion XXX XXX XXX
 *
 * This command must always take a turn, to prevent free detection
 * of invisible monsters.
 */
/*:::�w������ɉ�������ėp�R�}���h*/
void do_cmd_alter(void)
{
	int			y, x, dir;

	cave_type	*c_ptr;

	bool		more = FALSE;


	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Get a direction */
	if (get_rep_dir(&dir,TRUE))
	{
		s16b feat;
		feature_type *f_ptr;

		/* Get location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &f_info[feat];

		/* Take a turn */
		energy_use = 100;

		/* Attack monsters */
		if (c_ptr->m_idx)
		{
			/* Attack */
			py_attack(y, x, 0);
		}

		/* Locked doors */
		else if (have_flag(f_ptr->flags, FF_OPEN))
		{
			more = do_cmd_open_aux(y, x);
		}

		/* Bash jammed doors */
		else if (have_flag(f_ptr->flags, FF_BASH))
		{
			more = do_cmd_bash_aux(y, x, dir);
		}

		/* Tunnel through walls */
		else if (have_flag(f_ptr->flags, FF_TUNNEL))
		{
			more = do_cmd_tunnel_aux(y, x);
		}

		/* Close open doors */
		else if (have_flag(f_ptr->flags, FF_CLOSE))
		{
			more = do_cmd_close_aux(y, x);
		}

		/* Disarm traps */
		else if (have_flag(f_ptr->flags, FF_DISARM))
		{
			more = do_cmd_disarm_aux(y, x, dir);
		}

		/* Oops */
		else
		{
			/* Oops */
#ifdef JP
			msg_print("�����Ȃ��󒆂��U�������B");
#else
			msg_print("You attack the empty air.");
#endif

		}
	}

	/* Cancel repetition unless we can continue */
	if (!more) disturb(0, 0);
}


/*
 * Find the index of some "spikes", if possible.
 *
 * XXX XXX XXX Let user choose a pile of spikes, perhaps?
 */
/*:::�����т�T���Bj�R�}���h����̂݌Ă΂��*/
///del131223 ������
///sysdel item �����т�T��
#if 0
static bool get_spike(int *ip)
{
	int i;

	/* Check every item in the pack */
	for (i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Check the "tval" code */
		if (o_ptr->tval == TV_SPIKE)
		{
			/* Save the spike index */
			(*ip) = i;

			/* Success */
			return (TRUE);
		}
	}

	/* Oops */
	return (FALSE);
}
#endif

/*
 * Jam a closed door with a spike
 *
 * This command may NOT be repeated
 */
/*:::������������h�A��pval�����炵��*/
/*:::�h�A�ɂ����т�ł�*/
///sys item �h�A�ɞ���ł����@�����폜���Ă��܂����Ƃɂ���
#if 0
void do_cmd_spike(void)
{
	int dir;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		int y, x, item;
		cave_type *c_ptr;
		s16b feat;

		/* Get location */
		y = py + ddy[dir];
		x = px + ddx[dir];

		/* Get grid and contents */
		c_ptr = &cave[y][x];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);

		/* Require closed door */
		if (!have_flag(f_info[feat].flags, FF_SPIKE))
		{
			/* Message */
#ifdef JP
			msg_print("�����ɂ͂����т�łĂ���̂���������Ȃ��B");
#else
			msg_print("You see nothing there to spike.");
#endif

		}

		/* Get a spike */
		else if (!get_spike(&item))
		{
			/* Message */
#ifdef JP
			msg_print("�����т������Ă��Ȃ��I");
#else
			msg_print("You have no spikes!");
#endif
		}

		/* Is a monster in the way? */
		else if (c_ptr->m_idx)
		{
			/* Take a turn */
			energy_use = 100;

			/* Message */
#ifdef JP
			msg_print("�����X�^�[�������ӂ������Ă���I");
#else
			msg_print("There is a monster in the way!");
#endif

			/* Attack */
			py_attack(y, x, 0);
		}

		/* Go for it */
		else
		{
			/* Take a turn */
			energy_use = 100;

			/* Successful jamming */
#ifdef JP
			msg_format("%s�ɂ����т�ł����񂾁B", f_name + f_info[feat].name);
#else
			msg_format("You jam the %s with a spike.", f_name + f_info[feat].name);
#endif

			cave_alter_feat(y, x, FF_SPIKE);

			/* Use up, and describe, a single spike, from the bottom */
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
	}
}
#endif


/*
 * Support code for the "Walk" and "Jump" commands
 */
/*:::�������� pickup:�����E��*/
void do_cmd_walk(bool pickup)
{
	int dir;

	bool more = FALSE;

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}
 
	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		/* Take a turn */
		energy_use = 100;

		if ((dir != 5) && (p_ptr->special_defense & KATA_MUSOU))
		{
			set_action(ACTION_NONE);
		}

		//�˕P�����̏�ɑҋ@���Ă���Ƃ��_���l�̃��Z�b�g�t���O��������
		if(p_ptr->pclass == CLASS_YORIHIME && dir == 5 && (p_ptr->kamioroshi & KAMIOROSHI_GION))
		{
			reset_concent = FALSE;
		}

		///sys �S�̃}�b�v�ňړ������Ƃ��̃^�[������
		///mod140814 ���͑S�̃}�b�v�ړ�������
		/* Hack -- In small scale wilderness it takes MUCH more time to move */
		if (p_ptr->wild_mode) 
		{
			energy_use *= ((MAX_HGT + MAX_WID) / 2);
			if(p_ptr->pclass == CLASS_AYA || p_ptr->pclass == CLASS_MARISA || p_ptr->pclass == CLASS_SAKI) energy_use /= 2;
			if(CHECK_FAIRY_TYPE == 22) energy_use /= 2;
		}

		///sys ���삯�̃^�[������
		if (p_ptr->action == ACTION_HAYAGAKE) energy_use = energy_use * (45-(p_ptr->lev/2)) / 100;
		///mod150105 ���ѐ��������ړ�
		else if(p_ptr->pclass == CLASS_MURASA)
		{
			feature_type *f_ptr = &f_info[cave[py][px].feat];
			if(have_flag(f_ptr->flags, FF_WATER)) energy_use /= 3;
		}
		///mod131228 �����ړ��̃^�[������
		else if(p_ptr->speedster) energy_use = energy_use * (75-p_ptr->lev/2) / 100;
		///mod131228 �X���C���ω����̃^�[������
		else if(p_ptr->mimic_form == MIMIC_SLIME && !p_ptr->levitation) energy_use = energy_use * 3 / 2;
		///mod140907 �l���̃^�[������
		///mod150321 ��n���͖��֌W
		else if((prace_is_(RACE_NINGYO) || p_ptr->muta3 & MUT3_FISH_TAIL) && !p_ptr->levitation && !p_ptr->riding)
		{
			feature_type *f_ptr = &f_info[cave[py][px].feat];
			if(!have_flag(f_ptr->flags, FF_WATER)) energy_use = energy_use * 4 / 3;
		}
		/* Actually move the character */
		move_player(dir, pickup, FALSE,FALSE);

		/* Allow more walking */
		more = TRUE;
	}

	/* Hack again -- Is there a special encounter ??? */
	/*:::�r��ړ����̏P������*/
	///sys �r��P������
	///mod140814 ���͏P�����󂯂Ȃ�
	//���������ǉ�
	//���Ԓ�~�������ǉ�
	if (p_ptr->wild_mode && !cave_have_flag_bold(py, px, FF_TOWN) && (p_ptr->pclass != CLASS_AYA) && (p_ptr->pclass != CLASS_MARISA) && (p_ptr->pclass != CLASS_SAKI) && !SAKUYA_WORLD)
	{
		bool flag_encounter = FALSE;

		int tmp = 120 + p_ptr->lev*10 - wilderness[py][px].level + 5;
		if (tmp < 1) tmp = 1;

		//�����̒|�т�k���ňړ����Ă���ƏP�����󂯂₷��
		if(!p_ptr->levitation && distance(py,px,55,46) < 6)
		{
			tmp = randint1(tmp);
			//msg_print("chk:�k���P�����㏸");
		}

		if (((wilderness[py][px].level + 5) > (p_ptr->lev / 2)) && randint0(tmp) < (21 - p_ptr->skill_stl))
			flag_encounter = TRUE;

		//�O�r�̉�
		if (wilderness[py][px].level >= 40 && wilderness[py][px].terrain == TERRAIN_DEEP_WATER)
		{
			//msg_print("sans");
			//�푰���_�Ƃ��_�l�͏P���Ȃ�������ȊO�͒����m���ŏP����
			if (p_ptr->prace == RACE_DEATH
				|| p_ptr->prace == RACE_DEITY
				|| p_ptr->prace == RACE_STRAYGOD)
				flag_encounter = FALSE;
			else if(randint0(10) < (26 - p_ptr->skill_stl))
				flag_encounter = TRUE;

		}



		if(flag_encounter)
		{
			/* Inform the player of his horrible fate :=) */
#ifdef JP
			msg_print("�P�����I");
#else
			msg_print("You are ambushed !");
#endif

			/* Go into large wilderness view */
			p_ptr->oldpy = randint1(MAX_HGT-2);
			p_ptr->oldpx = randint1(MAX_WID-2);
			change_wild_mode();

			/* Give first move to monsters */
			energy_use = 100;

			/* HACk -- set the encouter flag for the wilderness generation */
			generate_encounter = TRUE;
		}
	}

	/* Cancel repeat unless we may continue */
	if (!more) disturb(0, 0);
}



/*
 * Start running.
 */
void do_cmd_run(void)
{
	int dir;

	/* Hack -- no running when confused */
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��đ���Ȃ��I");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Get a "repeated" direction */
	if (get_rep_dir(&dir,FALSE))
	{
		/* Hack -- Set the run counter */
		running = (command_arg ? command_arg : 1000);

		/* First step */
		run_step(dir);
	}
}



/*
 * Stay still.  Search.  Enter stores.
 * Pick up treasure if "pickup" is true.
 */
void do_cmd_stay(bool pickup)
{
	u32b mpe_mode = MPE_STAYING | MPE_ENERGY_USE;

	/* Allow repeated command */
	if (command_arg)
	{
		/* Set repeat count */
		command_rep = command_arg - 1;

		/* Redraw the state */
		p_ptr->redraw |= (PR_STATE);

		/* Cancel the arg */
		command_arg = 0;
	}

	/* Take a turn */
	energy_use = 100;

	break_eibon_flag = FALSE;

	if (pickup) mpe_mode |= MPE_DO_PICKUP;
	(void)move_player_effect(py, px, mpe_mode);
}



/*
 * Resting allows a player to safely restore his hp	-RAK-
 */
void do_cmd_rest(void)
{

	set_action(ACTION_NONE);
	///class ��V���l�Ǝ�p�n�C�͋x�e���~�߂�
	//if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] || p_ptr->magic_num1[1]))
///mod140817 �̊֌W�ɋ�V���l�ȊO�̐E���֏�
	if ((CHECK_MUSIC_CLASS) && (p_ptr->magic_num1[0] || p_ptr->magic_num1[1]))
	{

		stop_singing();
		stop_tsukumo_music();
		stop_raiko_music();
	}

	/* Hex */
	if (hex_spelling_any()) stop_hex_spell_all();

	/* Prompt for time if needed */
	if (command_arg <= 0)
	{
#ifdef JP
		cptr p = "�x�e (0-9999, '*' �� HP/MP�S��, '&' �ŕK�v�Ȃ���): ";
#else
		cptr p = "Rest (0-9999, '*' for HP/SP, '&' as needed): ";
#endif


		char out_val[80];

		/* Default */
		strcpy(out_val, "&");

		/* Ask for duration */
		if (!get_string(p, out_val, 4)) return;

		/* Rest until done */
		if (out_val[0] == '&')
		{
			command_arg = (-2);
		}

		/* Rest a lot */
		else if (out_val[0] == '*')
		{
			command_arg = (-1);
		}

		/* Rest some */
		else
		{
			command_arg = atoi(out_val);
			if (command_arg <= 0) return;
		}
	}


	/* Paranoia */
	if (command_arg > 9999) command_arg = 9999;

	///class �E�҂͋x�e�����B��������
	if (p_ptr->special_defense & NINJA_S_STEALTH) set_superstealth(FALSE);

	/* Take a turn XXX XXX XXX (?) */
	energy_use = 100;


	/* The sin of sloth */
	///del131209 virtue
	/*
	if (command_arg > 100)
		chg_virtue(V_DILIGENCE, -1);
	*/

	/* Why are you sleeping when there's no need?  WAKE UP!*/
/*
	if ((p_ptr->chp == p_ptr->mhp) &&
	    (p_ptr->csp == p_ptr->msp) &&
	    !p_ptr->blind && !p_ptr->confused &&
	    !p_ptr->poisoned && !p_ptr->afraid &&
	    !p_ptr->stun && !p_ptr->cut &&
	    !p_ptr->slow && !p_ptr->paralyzed &&
	    !p_ptr->image && !p_ptr->word_recall &&
	    !p_ptr->alter_reality)
			chg_virtue(V_DILIGENCE, -1);
*/

	if(p_ptr->pclass == CLASS_MEIRIN)
	{
		if ((p_ptr->chp == p_ptr->mhp) &&
	    (p_ptr->csp == p_ptr->msp) &&
	    !p_ptr->blind && !p_ptr->confused &&
	    !p_ptr->poisoned && !p_ptr->afraid &&
	    !p_ptr->stun && !p_ptr->cut &&
	    !p_ptr->slow && !p_ptr->paralyzed &&
	    !p_ptr->image )
		 msg_print("���Ȃ��͋�������n�߂��E�E");
		else msg_print("���Ȃ��͉񕜂ɏW�������E�E");
	}

	break_eibon_flag = FALSE;

	/* Save the rest code */
	resting = command_arg;
	p_ptr->action = ACTION_REST;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Refresh */
	Term_fresh();

}


/*
 * Determines the odds of an object breaking when thrown at a monster
 *
 * Note that artifacts never break, see the "drop_near()" function.
 */
/*:::��Ɠ������̔j�󔻒�@�A�[�`���[�͉��ɂ���*/
///class item TVAL ��Ɠ������̔j�󔻒�
int breakage_chance(object_type *o_ptr)
{
	int archer_bonus = (p_ptr->pclass == CLASS_ARCHER ? (p_ptr->lev-1)/7 + 4: 0);

	/* Examine the snipe type */
	if (snipe_type)
	{
		if (snipe_type == SP_KILL_WALL) return (100);
		if (snipe_type == SP_EXPLODE) return (100);
		if (snipe_type == SP_PIERCE) return (100);
		if (snipe_type == SP_FINAL) return (100);
		if (snipe_type == SP_NEEDLE) return (100);
		if (snipe_type == SP_EVILNESS) return (40);
		if (snipe_type == SP_HOLYNESS) return (40);
	}

	/* Examine the item type */
	
	switch (o_ptr->tval)
	{
		/* Always break */
		case TV_FLASK:
			if(o_ptr->sval == SV_FLASK_OIL) return (100);
			else return (10);
		case TV_POTION:
		//case TV_BOTTLE:
		case TV_FOOD:
		case TV_MUSHROOM:
		case TV_BULLET:
		//case TV_JUNK:
			return (100);

		/* Often break */
		case TV_LITE:
		case TV_SCROLL:
		//case TV_SKELETON:
			return (50);

		/* Sometimes break */
		case TV_WAND:
		//case TV_SPIKE:
			return (25);
		case TV_ARROW:
			return (20 - archer_bonus * 2);

			//������@�H���̖�͊���Ȃ�
		case TV_COMPOUND:
			if(o_ptr->sval == SV_COMPOUND_HOURAI) return 0;
			else return 100;

		/* Rarely break */
		//case TV_SHOT:
		case TV_BOLT:
			return (10 - archer_bonus);

		case TV_OTHERWEAPON:
			return 15;

		default:
			if(p_ptr->pclass == CLASS_NINJA && object_is_melee_weapon(o_ptr)) return (8 - p_ptr->lev / 7);
			else if(p_ptr->pclass == CLASS_SAKUYA && object_is_melee_weapon(o_ptr)) return (8 - p_ptr->lev / 9);
			///mod150918�@�����̌��Ђ͂��Ȃ炸����
			else if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_NIGHTMARE_FRAGMENT) return (100);
			else return (10);
	}
}

/*:::�ˌ��̃_���[�W�v�Z�@�x�[�X�_���[�W�ɃX���C�A�����A���͂��v�Z����@�Ƃ��������͂�����炵��*/
/*:::�ߐڕ���Ƃ͈���ă_�C�X�����łȂ��_���[�W�S�Ăɔ{����������炵���B*/
///sys item ego mon flg �ˌ��������̖�̃X���C�v�Z�Ȃ�
static s16b tot_dam_aux_shot(object_type *o_ptr, int tdam, monster_type *m_ptr)
{
	int mult = 10;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		///mod131223
		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			/* Slay Animal */
			if ((have_flag(flgs, TR_SLAY_ANIMAL)) &&
			    (r_ptr->flags3 & RF3_ANIMAL))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANIMAL;
				}

				if (mult < 17) mult = 17;
			}

			/* Kill Animal */
			if ((have_flag(flgs, TR_KILL_ANIMAL)) &&
			    (r_ptr->flags3 & RF3_ANIMAL))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANIMAL;
				}

				if (mult < 27) mult = 27;
			}

			/* Slay Evil */
			if ((have_flag(flgs, TR_SLAY_EVIL)) &&
			    (r_ptr->flags3 & RF3_ANG_CHAOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_CHAOS;
				}

				if (mult < 15) mult = 15;
			}

			/* Kill Evil */
			if ((have_flag(flgs, TR_KILL_EVIL)) &&
			    (r_ptr->flags3 & RF3_ANG_CHAOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_CHAOS;
				}

				if (mult < 25) mult = 25;
			}
			//mod160630 �����X���C�ǉ�
			if ((have_flag(flgs, TR_SLAY_GOOD)) &&
			    (r_ptr->flags3 & RF3_ANG_COSMOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_COSMOS;
				}

				if (mult < 15) mult = 15;
			}

			if ((have_flag(flgs, TR_KILL_GOOD)) &&
			    (r_ptr->flags3 & RF3_ANG_COSMOS))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_ANG_COSMOS;
				}

				if (mult < 25) mult = 25;
			}

			/* Slay Human */


///mod131231 �����X�^�[�t���O�ύX
			if ((have_flag(flgs, TR_SLAY_HUMAN)) &&
			    (r_ptr->flags3 & RF3_HUMAN))
//			    (r_ptr->flags2 & RF2_HUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_HUMAN;
//					r_ptr->r_flags2 |= RF2_HUMAN;
				}

				if (mult < 17) mult = 17;
			}

			/* Kill Human */
			if ((have_flag(flgs, TR_KILL_HUMAN)) &&
			    (r_ptr->flags2 & RF3_HUMAN))
//			    (r_ptr->flags2 & RF2_HUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_HUMAN;
//					r_ptr->r_flags2 |= RF2_HUMAN;
				}

				if (mult < 27) mult = 27;
			}

			/* Slay Undead */
			if ((have_flag(flgs, TR_SLAY_UNDEAD)) &&
			    (r_ptr->flags3 & RF3_UNDEAD))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_UNDEAD;
				}

				if (mult < 20) mult = 20;
			}

			/* Kill Undead */
			if ((have_flag(flgs, TR_KILL_UNDEAD)) &&
			    (r_ptr->flags3 & RF3_UNDEAD))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_UNDEAD;
				}

				if (mult < 30) mult = 30;
			}

			/* Slay Demon */
			if ((have_flag(flgs, TR_SLAY_DEMON)) &&
			    (r_ptr->flags3 & RF3_DEMON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMON;
				}

				if (mult < 20) mult = 20;
			}

			/* Kill Demon */
			if ((have_flag(flgs, TR_KILL_DEMON)) &&
			    (r_ptr->flags3 & RF3_DEMON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMON;
				}

				if (mult < 30) mult = 30;
			}

///mod131231 �����X�^�[�t���O�ύX
			if ((have_flag(flgs, TR_SLAY_KWAI)) &&
			    (r_ptr->flags3 & RF3_KWAI))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_KWAI;
				}

				if (mult < 20) mult = 20;
			}
			if ((have_flag(flgs, TR_KILL_KWAI)) &&
			    (r_ptr->flags3 & RF3_KWAI))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_KWAI;
				}

				if (mult < 30) mult = 30;
			}


			if ((have_flag(flgs, TR_SLAY_DEITY)) &&
			    (r_ptr->flags3 & RF3_DEITY))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEITY;
				}

				if (mult < 20) mult = 20;
			}

			if ((have_flag(flgs, TR_KILL_DEITY)) &&
			    (r_ptr->flags3 & RF3_DEITY))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEITY;
				}

				if (mult < 30) mult = 30;
			}

			//DEMIHUMAN�ɂ͐l�X���C�����������悤�ɂ���
			if ((have_flag(flgs, TR_SLAY_HUMAN)) &&
			    (r_ptr->flags3 & RF3_DEMIHUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMIHUMAN;
				}

				if (mult < 15) mult = 15;
			}

			if ((have_flag(flgs, TR_KILL_HUMAN)) &&
			    (r_ptr->flags3 & RF3_DEMIHUMAN))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DEMIHUMAN;
				}

				if (mult < 25) mult = 25;
			}

			/* Slay Dragon  */
			if ((have_flag(flgs, TR_SLAY_DRAGON)) &&
			    (r_ptr->flags3 & RF3_DRAGON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DRAGON;
				}

				if (mult < 20) mult = 20;
			}

			/* Execute Dragon */
			if ((have_flag(flgs, TR_KILL_DRAGON)) &&
			    (r_ptr->flags3 & RF3_DRAGON))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_DRAGON;
				}

				if (mult < 30) mult = 30;

				//�X�}�E�O�Ƀo���h��𓖂Ă�Ƒ�_���[�W�ɂȂ���ꏈ��
				if ((o_ptr->name1 == ART_BARD_ARROW) &&
				    (m_ptr->r_idx == MON_SMAUG) &&
				//    (inventory[INVEN_BOW].name1 == ART_BARD))
				    (inventory[INVEN_RARM].name1 == ART_BARD || inventory[INVEN_LARM].name1 == ART_BARD))
					mult *= 5;

			}
			/*:::�������̋|�Ŏˌ�����Ƃʂ��ɑ�_���[�W*/
			if ( (m_ptr->r_idx == MON_NUE) && (inventory[INVEN_RARM].name1 == ART_YORIMASA || inventory[INVEN_LARM].name1 == ART_YORIMASA))
			{
				char mname[256];
				monster_desc(mname, m_ptr, 0);

				//v1.1.83 ���̕s����������悤�ɂ��Ă݂�
				mult *= 3;
				if (m_ptr->mflag2 & MFLAG2_KAGE)
				{
					m_ptr->mflag2 &= ~(MFLAG2_KAGE);
					m_ptr->ap_r_idx = m_ptr->r_idx;
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
				msg_format("%s�͎�_��˂���Ĕߖ��グ���I", mname);

			}

			/* Brand (Acid) */
			if (have_flag(flgs, TR_BRAND_ACID))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 17) mult = 17;
				}
			}

			/* Brand (Elec) */
			if (have_flag(flgs, TR_BRAND_ELEC))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 17) mult = 17;
				}
			}

			/* Brand (Fire) */
			if (have_flag(flgs, TR_BRAND_FIRE))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
///mod131231 �����X�^�[�t���O�ύX �Ή���_RF3����RFR��
					if (r_ptr->flagsr & RFR_HURT_FIRE)
					{
						if (mult < 25) mult = 25;
						if (is_original_ap_and_seen(m_ptr))
						{
							r_ptr->r_flagsr |= RFR_HURT_FIRE;
						}
					}
					else if (mult < 17) mult = 17;
				}
			}

			/* Brand (Cold) */
			if (have_flag(flgs, TR_BRAND_COLD))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK);
					}
				}
				/* Otherwise, take the damage */
				else
				{
///mod131231 �����X�^�[�t���O�ύX ��C��_RF3����RFR��

					if (r_ptr->flagsr & RFR_HURT_COLD)
					{
						if (mult < 25) mult = 25;
						if (is_original_ap_and_seen(m_ptr))
						{
							r_ptr->r_flagsr |= RFR_HURT_COLD;
						}
					}
					else if (mult < 17) mult = 17;
				}
			}

			/* Brand (Poison) */
			if (have_flag(flgs, TR_BRAND_POIS))
			{
				/* Notice immunity */
				if (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK);
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 17) mult = 17;
				}
			}
			///mod141018 ��ʂ̗��͏����@�_���[�W�S�Ă�2.5�{
			//����MP��MAX��1/30����10�ŌŒ�ɏC��
			//if ((have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (p_ptr->msp / 30)))
			if ((have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp >= 10))
			{
				//p_ptr->csp -= (1+(p_ptr->msp / 30));
				p_ptr->csp -= 10;
				p_ptr->redraw |= (PR_MANA);
				mult = mult * 5 / 2;
			}
			break;
		}
	}

	/* Sniper */
	///mod141116 �������ǉ�
	//v1.1.74 �X�i�C�p�[�����폜
	//if (p_ptr->pclass == CLASS_SNIPER && snipe_type) mult = tot_dam_aux_snipe(mult, m_ptr);
	if (snipe_type) mult = tot_dam_aux_snipe(mult, m_ptr);

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Fire an object from the pack or floor.
 *
 * You may only fire items that "match" your missile launcher.
 *
 * You must use slings + pebbles/shots, bows + arrows, xbows + bolts.
 *
 * See "calc_bonuses()" for more calculations and such.
 *
 * Note that "firing" a missile is MUCH better than "throwing" it.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Objects are more likely to break if they "attempt" to hit a monster.
 *:::�ˌ�������ʂ͉��₷��
 *
 * Rangers (with Bows) and Anyone (with "Extra Shots") get extra shots.
 *
 * The "extra shot" code works by decreasing the amount of energy
 * required to make each shot, spreading the shots out over time.
 *:::�ǉ��ˌ��̌��ʂ͎g�p�s���^�[���̌����Ŏ���
 *
 * Note that when firing missiles, the launcher multiplier is applied
 * after all the bonuses are added in, making multipliers very useful.
 *:::�|�̔{���͑S�Ẵ_���[�W��{��������H
 *
 * Note that Bows of "Extra Might" get extra range and an extra bonus
 * for the damage multiplier.
 *:::���͎˂͎˒���������炵��
 *
 * Note that Bows of "Extra Shots" give an extra shot.
 */
/*:::�ˌ������@�|�Ɩ�͑I���ς�
 *:::item:�I��������ʂ̃C���x���g��/����C���f�b�N�X
 *:::j_ptr:�������̋|��object_type*/
void do_cmd_fire_aux(int item, object_type *j_ptr)
{
	int dir;
	int i, j, y, x, ny, nx, ty, tx, prev_y, prev_x;
	int tdam_base, tdis, thits, tmul;
	int bonus, chance;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	bool hit_body = FALSE;

	char o_name[MAX_NLEN];

	u16b path_g[512];	/* For calcuration of path length */

	int msec = delay_factor * delay_factor * delay_factor;

	/* STICK TO */
	bool stick_to = FALSE;

	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Sniper - Cannot shot a single arrow twice */
	/*:::�_�u���V���b�g�Ŗ��{�����Ȃ�������ʏ�ˌ��ɂȂ�*/
	if ((snipe_type == SP_DOUBLE) && (o_ptr->number < 2)) snipe_type = SP_NONE;

	/* Describe the object */
	object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

	/* Use the proper number of shots */
	/*:::���݂̎ˌ���(*100)*/
	thits = p_ptr->num_fire;

	/* Use a base distance */
	tdis = 10;

	/* Base damage from thrown object plus launcher bonus */
	tdam_base = damroll(o_ptr->dd, o_ptr->ds) + o_ptr->to_d + j_ptr->to_d;

	/* Actually "fire" the object */
	/*:::�������v�Z�@�N���X�{�E�͓�����₷��*/
	bonus = (p_ptr->to_h_b + o_ptr->to_h + j_ptr->to_h);


	///mod131227 skill �|�̖������v�Z �V����Z�\�l�K�p
	if (j_ptr->tval == TV_CROSSBOW)
		chance = (p_ptr->skill_thb + (ref_skill_exp(TV_CROSSBOW) / 400 + bonus) * BTH_PLUS_ADJ);
	else
		chance = (p_ptr->skill_thb + ((ref_skill_exp(TV_BOW) - (WEAPON_EXP_MASTER / 2)) / 200 + bonus) * BTH_PLUS_ADJ);
	/* v1.1.71 �|��N���X�{�E�̕���n���x�͂����Ōv�Z���Ă��邪�ߐڕ���̏n���x��calc_bonuses()�ŋZ�\�lp_ptr->to_h[]�ɎZ�����Ă���B
	�@ �X�b�L�����Ȃ���C��ʂɕ���Z�\�𔽉f����ז��ɂȂ�̂ł��̌v�Z��calc_bonuses()�ŋZ�\�lp_ptr->t_h_b�ɂ܂Ƃ߂悤�Ǝv�������A
	  ���ׂĂ݂�Ɠ����̖������ɂ�p_ptr->t_h_b���g���Ă����̂Œf�O�B���������ΐ��N�O�ɂ��C�ɂȂ���u���Ă��񂾂����B
	*/


	///mod140928 ���|��Ԃ��Ɩ���������
	if(p_ptr->afraid) chance = chance * 2 / 3;

	/*:::�|�̎�ނɂ���{�ˌ����x�@�܂��ŏI�l�ł͂Ȃ�*/
	energy_use = bow_energy(j_ptr->tval, j_ptr->sval);
	/*:::�|�̎�ނɂ���{�{���@������܂��ŏI�l�ł͂Ȃ�*/
	tmul = bow_tmul(j_ptr->tval, j_ptr->sval);

	/* Get extra "power" from "extra might" */
	if (p_ptr->xtra_might) tmul++;

	/*:::adj_str_td��126-148*/
	tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);

	/* Boost the damage */
	tdam_base *= tmul;
	tdam_base /= 100;

	/* Base range */
	
	tdis = 13 + tmul/80;
	///item sys �|TVAL�ύX�֘A
	///mod131223 tval
	//if ((j_ptr->sval == SV_LIGHT_XBOW) || (j_ptr->sval == SV_HEAVY_XBOW))
	if (j_ptr->tval == TV_CROSSBOW)
	{
		if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent)
			tdis -= (5 - (p_ptr->concent + 1) / 2);
		else
			tdis -= 5;
	}
	/*:::�|�̎˒��͍ō�20�}�X�@�N���X�{�E�͎˒����Z����x5�|�ŃX�i�C�p�[���W�������20�}�X�ɂȂ�*/
	project_length = tdis + 1;

	/* Get a direction (or cancel) */
	/*:::�^�[�Q�b�g��I������B�L�����Z��������s������ƃX�i�C�p�[�̓���ˌ������Z�b�g*/
	if (!get_aim_dir(&dir))
	{
		energy_use = 0;

		if (snipe_type == SP_AWAY) snipe_type = SP_NONE;

		/* need not to reset project_length (already did)*/

		return;
	}

	/* Predict the "target" location */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Check for "target request" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	//�u���̐��E�v
	if(SAKUYA_WORLD)
	{
		sakuya_time_stop(FALSE);
	}

	/* Get projection path length */
	tdis = project_path(path_g, project_length, py, px, ty, tx, PROJECT_PATH|PROJECT_THRU) - 1;

	project_length = 0; /* reset to default */

	/* Don't shoot at my feet */
	/*:::�X�i�C�p�[�̃V���[�g���A�E�F�C���������邩�Ǝv�����������ł��Ȃ������Bis_fired��ON�ɂȂ�Ȃ����߂�*/
	if (tx == px && ty == py)
	{
		energy_use = 0;

		/* project_length is already reset to 0 */

		return;
	}


	/* Take a (partial) turn */
	/*:::����G�l���M�[�ŏI��*/
	energy_use = (energy_use / thits);
	is_fired = TRUE;

	/* Sniper - Difficult to shot twice at 1 turn */
	if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_DOUBLE)  p_ptr->concent = (p_ptr->concent + 1) / 2;

	/*:::���ԏ���*/
	/* Sniper - Repeat shooting when double shots */
	for (i = 0; i < ((snipe_type == SP_DOUBLE) ? 2 : 1); i++)
	{

	/* Start at the player */
	y = py;
	x = px;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Single object */
	q_ptr->number = 1;

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
		floor_item_optimize(0 - item);
	}

	/* Sound */
	sound(SOUND_SHOOT);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* The shot does not hit yet */
	hit_body = FALSE;

	/* Travel until stopped */
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		

		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny = y;
		nx = x;
		mmove2(&ny, &nx, py, px, ty, tx);

		/* Shatter Arrow */
		if (snipe_type == SP_KILL_WALL)
		{
			cave_type *c_ptr = &cave[ny][nx];

			if (cave_have_flag_grid(c_ptr, FF_HURT_ROCK) && !c_ptr->m_idx)
			{
#ifdef JP
				if (c_ptr->info & (CAVE_MARK)) msg_print("�₪�ӂ��U�����B");
#else
				if (c_ptr->info & (CAVE_MARK)) msg_print("Wall rocks were shattered.");
#endif
				/* Forget the wall */
				c_ptr->info &= ~(CAVE_MARK);

				p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

				/* Destroy the wall */
				cave_alter_feat(ny, nx, FF_HURT_ROCK);

				hit_body = TRUE;
				break;
			}
		}

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny, nx, FF_PROJECT) && !cave[ny][nx].m_idx) break;

		/* Advance the distance */
		cur_dis++;

		/* Sniper */
		if (snipe_type == SP_LITE)
		{
			cave[ny][nx].info |= (CAVE_GLOW);

			/* Notice */
			note_spot(ny, nx);

			/* Redraw */
			lite_spot(ny, nx);
		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny, nx) && player_can_see_bold(ny, nx))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(c, a, ny, nx);
			move_cursor_relative(ny, nx);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(ny, nx);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Sniper */
		if (snipe_type == SP_KILL_TRAP)
		{
			project(0, 0, ny, nx, 0, GF_KILL_TRAP,
				(PROJECT_JUMP | PROJECT_HIDE | PROJECT_GRID | PROJECT_ITEM), -1);
		}

		/* Sniper */
		if (snipe_type == SP_EVILNESS)
		{
			cave[ny][nx].info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Notice */
			note_spot(ny, nx);

			/* Redraw */
			lite_spot(ny, nx);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx;
		y = ny;


		/* Monster here, Try to hit it */
		/*:::�����X�^�[�������瓖���鏈���A���ω��A����n���x�A��n�n���x�����Ȃ�*/
		if (cave[y][x].m_idx)
		{
			int armour;
			cave_type *c_ptr = &cave[y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Check the visibility */
			visible = m_ptr->ml;

			/* Note the collision */
			hit_body = TRUE;
///del131214 virtue
/*
			if (MON_CSLEEP(m_ptr))
			{
				if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
				if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
			}
*/
			///sys �ˌ����̕���o���l����
			///mod131227 skill tval�|�̌o���l�l�� �V����Z�\�l�K�p
			gain_skill_exp(j_ptr->tval,m_ptr);
			/*
			if ((r_ptr->level + 10) > p_ptr->lev)
			{

				int now_exp = p_ptr->weapon_exp[0][j_ptr->sval];
				if (now_exp < s_info[p_ptr->pclass].w_max[0][j_ptr->sval])
				{
					int amount = 0;
					if (now_exp < WEAPON_EXP_BEGINNER) amount = 80;
					else if (now_exp < WEAPON_EXP_SKILLED) amount = 25;
					else if ((now_exp < WEAPON_EXP_EXPERT) && (p_ptr->lev > 19)) amount = 10;
					else if (p_ptr->lev > 34) amount = 2;
					p_ptr->weapon_exp[0][j_ptr->sval] += amount;
					p_ptr->update |= (PU_BONUS);
				}
			}
			*/

			///sys �ˌ����̏�n�o���l����
			if (p_ptr->riding)
			{
				///mod131226 skill �Z�\�ƕ���Z�\�̓���
				gain_skill_exp(SKILL_RIDING,m_ptr);
#if 0				
				if ((p_ptr->skill_exp[GINOU_RIDING] < s_info[p_ptr->pclass].s_max[GINOU_RIDING])
					&& ((p_ptr->skill_exp[GINOU_RIDING] - (RIDING_EXP_BEGINNER * 2)) / 200 < r_info[m_list[p_ptr->riding].r_idx].level)
					&& one_in_(2))
				{
					p_ptr->skill_exp[GINOU_RIDING] += 1;
					p_ptr->update |= (PU_BONUS);
				}
#endif

			}
			/* Some shots have hit bonus */
			armour = r_ptr->ac;

			//v1.1.94 �����X�^�[�h��͒ቺ����AC25%�J�b�g
			if (MON_DEC_DEF(m_ptr))
			{
				armour = MONSTER_DECREASED_AC(armour);

			}

			/*
			if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent)
			{
				armour *= (10 - p_ptr->concent);
				armour /= 10;
			}
			*/

			//v1.1.74 �Z�C���g�X�^�[�A���[�𔭓����ʂƂ��ĕ���������concent���Ȃ��̂Ŗ������������␳
			if (snipe_type == SP_FINAL)
			{
				armour /= 2;
			}

			/* Did we hit it (penalize range) */
			/*:::��������*/
			if (test_hit_fire(chance - cur_dis, armour, m_ptr->ml))
			{
				bool fear = FALSE;
				int tdam = tdam_base;

				/* Get extra damage from concentration */
				if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent) tdam = boost_concentration_damage(tdam);

				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
#ifdef JP
					msg_format("%s���G��ߑ������B", o_name);
#else
					msg_format("The %s finds a mark.", o_name);
#endif

				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%s��%s�ɖ��������B", o_name, m_name);
#else
					msg_format("The %s hits %s.", o_name, m_name);
#endif

					if (m_ptr->ml)
					{
						/* Hack -- Track this monster race */
						if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

						/* Hack -- Track this monster */
						health_track(c_ptr->m_idx);
					}
				}

				if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_NEEDLE)
				{
					if ((randint1(randint1(r_ptr->level / (3 + p_ptr->concent)) + (8 - p_ptr->concent)) == 1)
						&& !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2))
					{
						char m_name[80];

						/* Get "the monster" or "it" */
						monster_desc(m_name, m_ptr, 0);

						tdam = m_ptr->hp + 1;
#ifdef JP
						msg_format("%s�̋}���ɓ˂��h�������I", m_name);
#else
						msg_format("Your shot sticked on a fatal spot of %s!", m_name);
#endif
					}
					else tdam = 1;
				}
				else
				{
					/*:::�ʏ�̖����_���[�W����*/
					/* Apply special damage XXX XXX XXX */
					tdam = tot_dam_aux_shot(q_ptr, tdam, m_ptr);
					tdam = critical_shot(q_ptr->weight, q_ptr->to_h, tdam);

					/* No negative damage */
					if (tdam < 0) tdam = 0;

					/* Modify the damage */
					tdam = mon_damage_mod(m_ptr, tdam, FALSE);
				}

				/* Complex message */
				///sys �ˌ��̃_���[�W�\��
				if (p_ptr->wizard || cheat_xtra)
				{
#ifdef JP
					msg_format("%d/%d �̃_���[�W��^�����B",
						   tdam, m_ptr->hp);
#else
					msg_format("You do %d (out of %d) damage.",
						   tdam, m_ptr->hp);
#endif

				}

				/* Sniper */
				if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_EXPLODE)
				{
					u16b flg = (PROJECT_STOP | PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID);

					sound(SOUND_EXPLODE); /* No explode sound - use breath fire instead */
					project(0, ((p_ptr->concent + 1) / 2 + 1), ny, nx, tdam, GF_MISSILE, flg, -1);
					break;
				}

				/* Sniper */
				if (snipe_type == SP_HOLYNESS)
				{
					cave[ny][nx].info |= (CAVE_GLOW);

					/* Notice */
					note_spot(ny, nx);

					/* Redraw */
					lite_spot(ny, nx);
				}

				/* Hit the monster, check for death */
				if (mon_take_hit(c_ptr->m_idx, tdam, &fear, extract_note_dies(real_r_ptr(m_ptr))))
				{
					/* Dead monster */
				}

				/* No death */
				else
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);
					/* STICK TO */
					/*:::�A�[�e�B�t�@�N�g�̖�͎h����*/
					if (object_is_fixed_artifact(q_ptr))
					{

						stick_to = TRUE;
#ifdef JP
						msg_format("%s��%s�ɓ˂��h�������I",o_name, m_name);
#else
						msg_format("%^s have stuck into %s!",o_name, m_name);
#endif
					}

					/* Message */
					message_pain(c_ptr->m_idx, tdam);

					/* Anger the monster */
					if (tdam > 0) anger_monster(m_ptr);

					///mod141018�@���ꏈ���@��ɒb��t�Z�\��|�ׂ��t���Ă����獬���̎�Ɠ�������
					{
						u32b flgs[TR_FLAG_SIZE];
						object_flags(q_ptr, flgs);
						if(have_flag(flgs,TR_CHAOTIC) && !(r_ptr->flags3 & RF3_NO_CONF) && !(r_ptr->flagsr & RFR_RES_CHAO) && (randint0(100) > r_ptr->level))
						{
							msg_format("%^s�͍��������悤���B", m_name);
							(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
						}
					}

					/* Take note */
					if (fear && m_ptr->ml)
					{

						/* Sound */
						sound(SOUND_FLEE);

						/* Get the monster name (or "it") */
						monster_desc(m_name, m_ptr, 0);

						/* Message */
#ifdef JP
						msg_format("%^s�͋��|���ē����o�����I", m_name);
#else
						msg_format("%^s flees in terror!", m_name);
#endif

					}

					set_target(m_ptr, py, px);

					/* Sniper */
					if (snipe_type == SP_RUSH)
					{
						int n = randint1(5) + 3;
						int m_idx = c_ptr->m_idx;

						for ( ; cur_dis <= tdis; )
						{
							int ox = nx;
							int oy = ny;

							if (!n) break;

							/* Calculate the new location (see "project()") */
							mmove2(&ny, &nx, py, px, ty, tx);

							/* Stopped by wilderness boundary */
							if (!in_bounds2(ny, nx)) break;

							/* Stopped by walls/doors */
							if (!player_can_enter(cave[ny][nx].feat, 0)) break;

							/* Stopped by monsters */
							if (!cave_empty_bold(ny, nx)) break;

							cave[ny][nx].m_idx = m_idx;
							cave[oy][ox].m_idx = 0;

							m_ptr->fx = nx;
							m_ptr->fy = ny;

							/* Update the monster (new location) */
							update_mon(c_ptr->m_idx, TRUE);

							lite_spot(ny, nx);
							lite_spot(oy, ox);

							Term_fresh();
							Term_xtra(TERM_XTRA_DELAY, msec);

							x = nx;
							y = ny;
							cur_dis++;
							n--;
						}
					}
				}
			}

			/* Sniper */
			/*:::�ђʏ���*/
			if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_PIERCE)
			{
				if(p_ptr->concent < 1) break;
				p_ptr->concent--;
				continue;
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	/*:::��ʂ̔j�󗦌v�Z*/
	j = (hit_body ? breakage_chance(q_ptr) : 0);

	if (stick_to)
	{
		int m_idx = cave[y][x].m_idx;
		monster_type *m_ptr = &m_list[m_idx];
		int o_idx = o_pop();

		/*:::��h�������͂������łɃt���A�̃A�C�e������t�̎��H*/
		if (!o_idx)
		{
#ifdef JP
			msg_format("%s�͂ǂ����֍s�����B", o_name);
#else
			msg_format("The %s have gone to somewhere.", o_name);
#endif
			if (object_is_fixed_artifact(q_ptr))
			{
				a_info[j_ptr->name1].cur_num = 0;
			}
			return;
		}

		o_ptr = &o_list[o_idx];
		object_copy(o_ptr, q_ptr);

		/* Forget mark */
		o_ptr->marked &= OM_TOUCHED;

		/* Forget location */
		o_ptr->iy = o_ptr->ix = 0;

		/* Memorize monster */
		o_ptr->held_m_idx = m_idx;

		/* Build a stack */
		o_ptr->next_o_idx = m_ptr->hold_o_idx;

		/* Carry object */
		m_ptr->hold_o_idx = o_idx;
	}
	else if (cave_have_flag_bold(y, x, FF_PROJECT))
	{
		/* Drop (or break) near that location */
		(void)drop_near(q_ptr, j, y, x);
	}
	else
	{
		/* Drop (or break) near that location */
		(void)drop_near(q_ptr, j, prev_y, prev_x);
	}

	/* Sniper - Repeat shooting when double shots */
	}

	/* Sniper - Loose his/her concentration after any shot */
	//if (p_ptr->concent) reset_concentration(FALSE);
	///mod141116 ���莮�ύX
	if(CLASS_USE_CONCENT)
	{
		reset_concentration(FALSE);

	}

}

///mod160508
//�e�̕K�v�[�U���Ԋ�{�l���v�Z����B�n���x�ȂǂɊ֌W�Ȃ��A�C�e�����̂̒l�Ŕ����C���f�b�N�X�ɂ��ω�����
int calc_gun_timeout(object_type *o_ptr)
{
	const activation_type* const act_ptr = find_activation_info(o_ptr);
	int timeout;
	if(o_ptr->tval != TV_GUN){msg_print("ERROR:�e�ȊO��calc_gun_timeout()���Ă΂ꂽ");return 0;}
	if(!act_ptr){msg_print("ERROR:calc_gun_timeout()�ň���ꂽ�����C���f�b�N�X����������");return 0;}

	//GUN_VARIABLE(�e�̉ϓ��ꔭ�����[�`��)�̏ꍇsval�l�ɉ������l�𓾂�
	if(act_ptr->index == ACT_GUN_VARIABLE)
	{
		timeout = gun_base_param_table[o_ptr->sval].charge_turn;
	}
	//����ȊO�̔����̏ꍇ�A�����e�[�u������^�C���A�E�g�l(�x�[�X�����̂�)�𓾂�
	else
	{
		timeout = act_ptr->timeout.constant;
	}

	return timeout;

}

//�e�ɂ��ˌ��̎��s�����@��{�I��A�R�}���h�ɂ�锭���Ɠ������������i�A���ʐ��A�����̔������e�ύX�ɑΉ����邽�ߊ�{�p�����[�^���ŏ��Ɉꊇ��������
void do_cmd_fire_gun_aux(int slot, int dir)
{
	int plev = p_ptr->lev;

	int dice=0,sides=0,base=0;
	int typ = GF_MISSILE;
	int mode = GUN_FIRE_MODE_DEFAULT;
	int rad=0;
	int dam;
	int timeout_base, timeout_max;
	char o_name[160];
	object_type *o_ptr = &inventory[slot];
	int chance;

	cptr shot_msg = NULL; //�ˌ������b�Z�[�W

	const activation_type* const act_ptr = find_activation_info(o_ptr);

	if(o_ptr->tval != TV_GUN){msg_print("ERROR:�e�ȊO��activate_gun()���Ă΂ꂽ");return;}

	if(!act_ptr->index)
	{
		msg_print("���̏e�͌��ĂȂ��悤���B");
		return;
	}





	/*::: -Mega Hack- �e�̃^�C���A�E�g�l��1000�{�ɂ��A�\�{�̕p�x�ŏ[�U���[�`�����ĂсA���̂��т�100���炷�B
	 *::: ����Ɍ��炷���l�������ˌ���e�n���x�ɉ����đ���������B*/

	//�ˌ��ꔭ���Ƃ̃^�C���A�E�g�����l
	timeout_base = calc_gun_timeout(o_ptr) * 1000;
	//�ˌ��\�ȃ^�C���A�E�g����l�@������z����ƒe�؂�
	timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
	if(o_ptr->timeout > timeout_max)
	{
		msg_print("���e���ς�ł��Ȃ��B"); //�����̏�̊֐��Ŕ���͍ς܂��Ă��������A�ق����炱�������ڌĂ΂�邩������Ȃ��̂ł����ł��`�F�b�N���Ƃ�
		return;
	}

	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW))
		energy_use = 75;
	else
		energy_use = 100;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
		msg_print("�e���쓮���Ȃ��B");
		sound(SOUND_FAIL);
		return;
	}



	chance = calc_gun_fire_chance(slot);


	//�ˌ��̊�b�p�����[�^�ƃ��b�Z�[�W��ݒ肷�� ���֘A�̓n�[�h�R�[�f�B���O���Ă��܂�
	switch(act_ptr->index)
	{
	case ACT_CRIMSON:
		rad = 2;
		base = p_ptr->lev * p_ptr->lev * 6 / 50;
		typ = GF_ROCKET;
		mode = GUN_FIRE_MODE_ROCKET;
		shot_msg = "��������������w�N�����]���x���Ԃ��ςȂ����I";
		break;

	case ACT_BFG9000:
		rad = 5;
		base = 800;
		dice = 8;
		sides = 100;
		typ = GF_DISP_ALL;
		mode = GUN_FIRE_MODE_ROCKET;
		shot_msg = "�e�g���狐��ȃG�l���M�[�e�������ꂽ�I";
		break;
	case ACT_BAROQUE:
		rad=1;
		base = 500;
		typ = GF_GENOCIDE;
		mode = GUN_FIRE_MODE_ROCKET;
		if(one_in_(6)) shot_msg = "�H�̂悤�Ȃ��̂���юU��̂��������C������...";
		else shot_msg = "�V�g�e�𔭎˂����I";
		break;
	case ACT_DER_FREISCHUTZ:
		rad = 1;
		base = plev*2;
		if(special_shooting_mode == SSM_SEIRAN5) base *= 5;
		typ = GF_SHARDS;
		mode = GUN_FIRE_MODE_MADAN;
		shot_msg = "���e��������I";

			
		break;

	case ACT_YAKUZA_GUN:
		dice = 1;
		sides = 20;
		if(special_shooting_mode == SSM_SEIRAN5 || special_shooting_mode == SSM_ELEC_BEAM) dice = 20;
		typ = GF_ARROW;
		mode = GUN_FIRE_MODE_BLAST;
		if(one_in_(10)) shot_msg = "�u�U�b�P���i�R���[�b�I�v";
		else shot_msg = "BLAMBLAMBLAMBLAMBLAM!";
		break;

	case ACT_HARKONNEN2:
		base = 1;
		mode = GUN_FIRE_MODE_VLADIMIR;
		shot_msg = "�L�旧�̐����p����ĈΞ֒e�𔭎˂����I";
		break;


	//��{�I�ȏe
	case ACT_GUN_VARIABLE:
		dice = o_ptr->dd;
		sides = o_ptr->ds;
		base = o_ptr->to_d;
		typ = o_ptr->xtra1;
		mode = o_ptr->xtra4;
		//����msg�����H
		break;

	default:
		msg_print("ERROR:���̏e�̎ˌ���b�p�����[�^����������Ă��Ȃ�");
		return;
	}


	//�_���[�W�v�Z
	dam = base;
	if(dice && sides) dam += damroll(dice,sides);
	if(dam < 1)
	{
		object_desc(o_name, o_ptr, OD_NAME_ONLY);
		msg_format("%s�͕s�����I",o_name);
		return;
	}

	//���m�u�e�e�ύX�v�̂Ƃ�
	if(special_shooting_mode >= SSM_CHANGE_AMMO_START && special_shooting_mode <= SSM_CHANGE_AMMO_END)
	{
		int bullet_idx = special_shooting_mode;
		//�n���R���l���Q�ȂǓ���ȕ���͔�ΏہB���̂܂ܕ��ʂɌ����Ă��܂��Ǝc�e�Ȃǂ�����Ă��܂��̂ł����ŏI��
		//v1.1.22c ���P���������O�B���ňꌂ3000���炢�̂��o�Ă��܂��ĊȒP�ɃJ���X�g����B
 		if(o_ptr->sval == SV_FIRE_GUN_ROCKET ||
			mode == GUN_FIRE_MODE_MADAN || mode == GUN_FIRE_MODE_VLADIMIR || mode == GUN_FIRE_MODE_BLAST
			|| act_ptr->index == ACT_BFG9000 || act_ptr->index == ACT_BAROQUE) 
		{
			if(p_ptr->pclass == CLASS_SEIRAN)
				msg_print("���̏e�ł��̓��Z���g�����Ƃ͂ł��Ȃ��B");
			else
				msg_print("���̏e�͏e�e��ύX�ł��Ȃ��B");
			return;
		}


		//�E�ƕ��m�́u�e�e�ύX�v�Ɋւ���c�e��MP�̏���
		if (p_ptr->pclass == CLASS_SOLDIER)
		{

			if (soldier_bullet_table[bullet_idx].magic_bullet_lev == SS_M_MATERIAL_BULLET && !p_ptr->magic_num1[bullet_idx])
			{
				msg_print("�e�؂ꂾ�I");
				return;
			}
			else if (soldier_bullet_table[bullet_idx].cost > p_ptr->csp)
			{
				msg_print("MP������Ȃ��B");
				return;
			}
			p_ptr->csp -= soldier_bullet_table[bullet_idx].cost;
			if (soldier_bullet_table[bullet_idx].magic_bullet_lev == SS_M_MATERIAL_BULLET)
				p_ptr->magic_num1[bullet_idx] -= 1;

		}


		//�u�ˌ��v�ȊO�̕�����e�e�ύX����ƈЗ͌���
		switch(typ)
		{
		case GF_ARROW:
			break;
		case GF_FIRE:
		case GF_COLD:
		case GF_ACID:
		case GF_ELEC:
		case GF_POIS:
		case GF_PLASMA:
		case GF_NUKE:
		case GF_POLLUTE:
		case GF_ICE:
		case GF_STEAM:
			dam -= dam / 2;
			break;
		default:
			dam -= dam / 3;
			break;
		}
	}

	//v1.1.21 ���m�����ɔ��������ύX�𐮓�
	//special_shooting_mode�ɂ�鑮���ύX�Ȃ�
	switch(special_shooting_mode)
	{
	case 0:	//����ˌ��Ȃ�
		break;

	case SSM_SEIRAN1: //����1 �e������
		if(mode == GUN_FIRE_MODE_BLAST) break;
 		if(mode == GUN_FIRE_MODE_MADAN) break;
		if(mode == GUN_FIRE_MODE_VLADIMIR) break; 
		chance = 100; //�O��Ȃ�
		if(!rad && (mode == GUN_FIRE_MODE_BALL || mode == GUN_FIRE_MODE_ROCKET || mode == GUN_FIRE_MODE_BREATH || mode == GUN_FIRE_MODE_SPARK ))
			rad = 1 + p_ptr->lev / 20;
		mode = GUN_FIRE_MODE_JUMP;
		break;

	case SSM_SEIRAN2: //����2 �A�E�F�C�ǉ�
		break;
	case SSM_SEIRAN3: //����3 ���e
		break;
	case SSM_SEIRAN4: //����4 �S�e�ˌ�
		break;

	case SSM_SEIRAN5: //����5 �����ύX
		if(mode == GUN_FIRE_MODE_VLADIMIR) break; 
 		if(mode == GUN_FIRE_MODE_ROCKET) break;
		mode = GUN_FIRE_MODE_BEAM;
		typ = GF_SEIRAN_BEAM;
		dam = dam * 3 / 2;
		shot_msg = "�K�E�̈ꌂ����Ԃ�c�߂��I";
		break;

	case SSM_BLACK_PEGASUS:
		if (mode == GUN_FIRE_MODE_VLADIMIR) break;
		if (mode == GUN_FIRE_MODE_MADAN) break;
		mode = GUN_FIRE_MODE_ROCKET;
		typ = GF_METEOR;
		dam *= 2 ;
		rad = 5;
		shot_msg = "�V�����k�킹��ꌂ��������I";
		break;


	case SSM_CONFUSE: //�����ˌ� �_���[�W0 �����t�^�@�͈͍U���n�̏e�ɂ͖���
 		if(mode == GUN_FIRE_MODE_MADAN
		||mode == GUN_FIRE_MODE_VLADIMIR 
 		||mode == GUN_FIRE_MODE_BREATH
 		||mode == GUN_FIRE_MODE_SPARK
 		||mode == GUN_FIRE_MODE_BALL
 		||mode == GUN_FIRE_MODE_ROCKET)
		{
			msg_print("���̏e�ł͌����ɂȂ�Ȃ��B");
			return;
		}

		if(one_in_(3))		shot_msg = "�G�̑�����_���Č������B";
		else if(one_in_(2))	shot_msg = "�G�̕@��𗩂߂Č������B";
		else				shot_msg = "�G�̋@��𐧂��Č������B";
		typ = GF_SPECIAL_SHOT;

	//case SSM_HEAD_SHOT: //�w�b�h�V���b�g �N���e�B�J�����蔭��..���������ʏ����ɂ���
	//	break;
	case SSM_RAPID_FIRE: //���s�b�h�t�@�C�A ����
		if(is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
			shot_msg = "�e�ۂ��󒆂ŗ����˂��W�I�֌��������I";
		break;

	case SSM_FIRE:
		typ = GF_FIRE;
		shot_msg = "�ԔM����e�ۂ�������I";
		dam *= 2;
		break;
	case SSM_COLD:
		typ = GF_COLD;
		shot_msg = "��C���܂Ƃ��e�ۂ�������I";
		dam *= 2;
		break;
	case SSM_ELEC:
		typ = GF_ELEC;
		shot_msg = "�ΉԂ��U�炷�e�ۂ�������I";
		dam *= 2;
		break;
	case SSM_ACID:
		typ = GF_ACID;
		shot_msg = "�_�̕������e�ۂ�������I";
		dam *= 2;
		break;
	case SSM_LIGHT: //�M���e
		typ = GF_SPECIAL_SHOT;
		rad = 2;
		mode = GUN_FIRE_MODE_ROCKET;
		shot_msg = "�M���e��������I";
		break;
	case SSM_VAMPIRIC:
		typ = GF_SPECIAL_SHOT;
		mode = GUN_FIRE_MODE_BOLT;
		shot_msg = "���ɋQ�����e��������I";
		break;
	case SSM_SONICWAVE:
		typ = GF_SOUND;
		mode = GUN_FIRE_MODE_BEAM;
		shot_msg = "�Ռ��g��������I";
		break;
	case SSM_GRAV:
		typ = GF_GRAVITY;
		rad = 2;
		mode = GUN_FIRE_MODE_BALL;
		shot_msg = "�͏�̒e�ۂ�������I";
		break;

	case SSM_POIS:
		typ = GF_POIS;
		dam *= 3;
		shot_msg = "�ŃK�X�e��������I";
		break;
	case SSM_HEAVY: 
		typ = GF_ARROW;
		mode = GUN_FIRE_MODE_DEFAULT;
		dam *= 2;
		shot_msg = "�����x�̒e�ۂ�������I";
		break;
	case SSM_BREATH:
		mode = GUN_FIRE_MODE_BREATH;
		dam *= 3;
		shot_msg = "�e�����猃�����u���X�������o���ꂽ�I";
		break;
	case SSM_ICE:
		typ = GF_ICE;
		mode = GUN_FIRE_MODE_ROCKET;
		rad = 1;
		dam *= 3;
		shot_msg = "���Ă��e�ۂ�������I";
		break;

	case SSM_HOLY:
		typ = GF_HOLY_FIRE;
		mode = GUN_FIRE_MODE_ROCKET;
		rad = 2;
		dam *= 3;
		shot_msg = "�_�X�����P���e�ۂ�������I";
		break;

	case SSM_RED:
		typ = GF_FIRE;
		mode = GUN_FIRE_MODE_BEAM;
		dam *= 5;
		shot_msg = "�^�g�̌�����������I";
		break;
	case SSM_DIAMOND:
		typ = GF_PSY_SPEAR;
		mode = GUN_FIRE_MODE_BEAM;
		dam *= 4;
		shot_msg = "�e������ῂ��P������ꂽ�I";
		break;
	case SSM_METEOR:
		typ = GF_ARROW;
		mode = GUN_FIRE_MODE_DEFAULT;
		dam *= 4;
		shot_msg = "�e�����痬���������ꂽ�I";
		break;
	case SSM_HIHIIROKANE:
		typ = GF_DISINTEGRATE;
		mode = GUN_FIRE_MODE_SPARK;
		dam *= 10;
		shot_msg = "����ȃG�l���M�[�������ꂽ�I";
		break;


	case SSM_SLAY_ANIMAL: 
	case SSM_SLAY_NONLIV: 
	case SSM_SLAY_EVIL: 
	case SSM_SLAY_HUMAN: 
	case SSM_SLAY_DRAGON:
	case SSM_SLAY_DEITY: 
	case SSM_SILVER: 
	case SSM_NIGHTMARE: 
	case SSM_SLAY_FLYER:
		typ = GF_SPECIAL_SHOT;
		break;

	case SSM_ELEC_BEAM: //�d���r�[��
		if (mode == GUN_FIRE_MODE_MADAN	|| mode == GUN_FIRE_MODE_VLADIMIR)
		{
			msg_print("���̏e�ł͋Z���g���Ȃ��B");
			return;
		}
		typ = GF_ELEC;
		mode = GUN_FIRE_MODE_BEAM;
		break;

	default:
		break;

	}

	//�[�U����(�����Ɏ��s���Ă��[�U�҂��ɂȂ�)
	if(o_ptr->timeout + timeout_base > 32000L)
	{
		msg_print("ERROR:���̏e�̃^�C���A�E�g�l��32000�𒴂���");
		return;
	}
	o_ptr->timeout += timeout_base;
	p_ptr->window |= PW_EQUIP;
	//�K���J�^�c�e����̂��ߒe�؂�`�F�b�N
	if(o_ptr->timeout > timeout_max)
		p_ptr->update |= PU_BONUS;

	if (cheat_xtra) msg_format("gun_chance:%d%%", chance);
	//�������s����
	if(randint1(100) > chance)
	{
		object_desc(o_name, o_ptr, OD_NAME_ONLY);
		msg_format("%s�̎ˌ��Ɏ��s�����I�e�͂���ʕ����֔��ł�����..",o_name);
		return;
	}


	//v1.1.21 �ꕔ����ˌ��̂Ƃ��̃N���e�B�J���{�[�i�X���v�Z���邩�ǂ����̃t���O�𗧂Ă�B
	//�U�e�e�Ŕ͈͂̓G�S���ɃN���e�B�J���Ƃ��o����Ȃ񂩕ςȂ̂ŁA
	//�ŏ���project_m()���Ă΂ꂽ�Ƃ��ɂ����_���[�W�����v�Z���ăt���O���N���A����B
	//�����Œ��ڔ{�����v�Z���Ȃ��̂́A�u�ߐڎˌ��v�̗אڔ����w�b�h�V���b�g�̑ΐ�������Ȃǂ������ł͓������
	if(mode != GUN_FIRE_MODE_BLAST && mode != GUN_FIRE_MODE_MADAN && mode != GUN_FIRE_MODE_VLADIMIR)
	{
		//�O��̍s����Ƀ��Z�b�g����Ă�͂������ꉞ
		hack_gun_fire_critical_flag = 0L;
		//�N���e�B�J������Ɏg���e�̖����C����ێ�
		hack_gun_fire_critical_hit_bonus = o_ptr->to_h;

		//if(special_shooting_mode == SSM_HEAD_SHOT)//�w�b�h�V���b�g
		if(p_ptr->special_attack & ATTACK_HEAD_SHOT)
			hack_gun_fire_critical_flag |= GUN_CRIT_HEAD;

		if(p_ptr->concent)//���_�W��
			hack_gun_fire_critical_flag |= GUN_CRIT_CONCENT;

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_CLOSE_RANGE_SHOT))//�ߐڎˌ�
			hack_gun_fire_critical_flag |= GUN_CRIT_CLOSE;

		if(p_ptr->pseikaku == SEIKAKU_TRIGGER)//�g���K�[�n�b�s�[
			hack_gun_fire_critical_flag |= GUN_CRIT_TRIGGERHAPPY;

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_NOCTO_VISION) && !(cave[py][px].info & CAVE_GLOW) && p_ptr->cur_lite <= 0)		
			hack_gun_fire_critical_flag |= GUN_CRIT_NIGHT;

		if(CHECK_CONCEALMENT)
			hack_gun_fire_critical_flag |= GUN_CRIT_CONCEAL;

		//���m�u�e�e�ύX�v�̂Ƃ�
		if(special_shooting_mode >= SSM_CHANGE_AMMO_START && special_shooting_mode <= SSM_CHANGE_AMMO_END)
			hack_gun_fire_critical_flag |= GUN_CRIT_SPECIALBULLET;

		//���e�̎ˎ�
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_DER_FREISCHUTZ))
			hack_gun_fire_critical_flag |= GUN_CRIT_MAGICBULLET;

		//�ˌ���{���g�ȊO�̓N���e�B�J�����ɂ���
		if(mode == GUN_FIRE_MODE_SPARK || mode == GUN_FIRE_MODE_BREATH)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_5;
		else if(mode == GUN_FIRE_MODE_BALL || mode == GUN_FIRE_MODE_ROCKET)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_3;
		else if(mode == GUN_FIRE_MODE_BEAM)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_2;
		//�A�ˌn�Z�\�̂Ƃ��ɂ̓N���e�B�J�����ɂ���
		else if(special_shooting_mode == SSM_RAPID_FIRE || special_shooting_mode == SSM_SEIRAN4)
			hack_gun_fire_critical_flag |= GUN_CRIT_DEV_2;

	}

	//msg
	if(shot_msg) msg_format("%s",shot_msg);

	//�o���l�𓾂邽�߂̃t���O
	hack_flag_gain_gun_skill_exp = TRUE;

	switch(mode)
	{
	case GUN_FIRE_MODE_DEFAULT: //�ˌ�
	case GUN_FIRE_MODE_BOLT://�{���g
		fire_bolt(typ,dir,dam);
		break;
	case GUN_FIRE_MODE_BALL:
		rad = 1 + p_ptr->lev / 20;
		fire_ball(typ, dir, dam, rad);
		break;
	case GUN_FIRE_MODE_BEAM:
		fire_beam(typ,dir,dam);
		break;
	case GUN_FIRE_MODE_ROCKET:
		if(!rad) rad = 1 + p_ptr->lev / 20;
		fire_rocket(typ, dir, dam, rad);
		break;
	case GUN_FIRE_MODE_BREATH:
		rad = -2 - p_ptr->lev / 40;
		fire_ball(typ, dir, dam, rad);
		break;
	case GUN_FIRE_MODE_SPARK:
		rad = 1;
		fire_spark(typ,dir,dam,rad);
		break;
	case GUN_FIRE_MODE_JUMP:
		fire_ball_jump(typ,dir,dam,rad,"�e�e�͋�Ԃ𒵖􂵂��I");
		break;
	case GUN_FIRE_MODE_BLAST:
		fire_blast(typ,dir,dice,sides,20,3,0L);
		break;
	case GUN_FIRE_MODE_MADAN:
		{ //v1.1.44 ���[�`��������

			if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET, SS_M_DER_FREISCHUTZ))
				hack_der_freischutz_basenum = 20 + p_ptr->concent;
			else
				hack_der_freischutz_basenum = 0;

			if (cheat_xtra) msg_format("basenum_init:%d", hack_der_freischutz_basenum);
			execute_restartable_project(0, dir, 8, 0, 0, dam, GF_SHARDS, rad);
			hack_der_freischutz_basenum = 0;

		}

		/*
		{
			int ty,tx;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_MADAN;
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}
			if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_DER_FREISCHUTZ))
				hack_der_freischutz_basenum = 20 + p_ptr->concent;
			else
				hack_der_freischutz_basenum = 0;

			if(cheat_xtra) msg_format("basenum_init:%d",hack_der_freischutz_basenum);
			project(0, rad, ty, tx, dam, GF_SHARDS, flg, -1);
		}
		*/
		break;
	//�n���R���l��2 �_�~�[�A�C�e�������Ƃ��ē��ꏈ������
	case GUN_FIRE_MODE_VLADIMIR:
		{
			object_type forge;
			object_type *tmp_o_ptr = &forge;
			int ty,tx;

			if(special_shooting_mode == SSM_SEIRAN2) special_shooting_mode = 0L; 
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
			if ((dir == 5) && target_okay())
			{
				tx = target_col;
				ty = target_row;
			}

			object_prep(tmp_o_ptr,lookup_kind(TV_COMPOUND,SV_COMPOUND_VLADIMIR));

			do_cmd_throw_aux2(py,px,ty,tx,2,tmp_o_ptr,5);

		}
		break;
	default:
		msg_print("ERROR:do_cmd_fire_gun_aux()�ɂ��̕���̐ݒ肪����Ă��Ȃ�");
		return;
	}
	hack_flag_gain_gun_skill_exp = FALSE;


	return;

}


//���m���Z�u���s�b�h�t�@�C�A�v���ǂ񂰐�p���i���Z�u���i�e�B�b�N�_�u���v�Ɏg���B���ǂ񂰐V���i�̓��Z�ɗ��p���邽�߂ɕʊ֐��ɂ����B
//�s���������Ƃ�TRUE��Ԃ�
//shoot_num:�ő�A�ː�
//mode 
//0:�ʏ�
//1:���C�g�j���O�l�C�@�d�������r�[���ɂȂ�
bool	rapid_fire(int shoot_num, int mode)
{
	int dir = 5;
	int timeout_base, timeout_max;
	int count;
	object_type *o_ptr;
	bool flag_ammo = FALSE;
	bool flag_mon = FALSE;

	if (!CHECK_USE_GUN)
	{
		msg_print("�e�����ĂȂ��B");
		return FALSE;
	}

	switch (mode)
	{
	case 1:
		special_shooting_mode = SSM_ELEC_BEAM;
		msg_print("���Ȃ��̏e�͓d���̂悤�ɑM�����I");
		break;

	default:
		special_shooting_mode = SSM_RAPID_FIRE;
		break;

	}


	for (count = 0; count < shoot_num ; count++)
	{
		bool righthand = FALSE;
		bool lefthand = FALSE;

		//�c�e�`�F�b�N
		if (inventory[INVEN_RARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_RARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
			if (o_ptr->timeout <= timeout_max) righthand = TRUE;
		}
		if (!get_random_target(0, 0)) break;
		flag_mon = TRUE;
		if (righthand)do_cmd_fire_gun_aux(INVEN_RARM, dir);

		if (inventory[INVEN_LARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_LARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1);
			if (o_ptr->timeout <= timeout_max) lefthand = TRUE;
		}
		if (!righthand && !lefthand) break;
		else flag_ammo = TRUE;

		if (!get_random_target(0, 0)) break;
		if (lefthand)do_cmd_fire_gun_aux(INVEN_LARM, dir);
	}
	special_shooting_mode = 0L;

	if (!flag_mon)
	{
		msg_print("���͂ɕW�I�����Ȃ��B");
		return FALSE;
	}
	else if (!flag_ammo)
	{
		msg_print("�������e���Ȃ������B");
		return FALSE;
	}

	return TRUE;

}


/*:::�ˌ�������*/
///sys item �ˌ����[�`��
///mod160603 ��������ˌ��̂��߂�bool��Ԃ����Ƃɂ���
bool do_cmd_fire(void)
{
	int item;
	object_type *j_ptr;
	cptr q, s;
	int tester;

	is_fired = FALSE;	/* not fired yet */

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE || p_ptr->clawextend)
	{
		msg_print("���̎p�ł͎ˌ����ł��Ȃ��B");
		return FALSE;
	}

	///item sys �|��object_type�𓾂镔��
	/* Get the "bow" (if any) */
	//j_ptr = &inventory[INVEN_BOW];
	j_ptr = &inventory[INVEN_PACK + shootable(&tester)];


	if (tester == SHOOT_HEAVY_WEAPON)
	{
#ifdef JP
			msg_print("�ˌ����킪�d�����č\�����Ȃ��B");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return FALSE;
	}

	if (tester == SHOOT_UNSUITABLE)
	{
#ifdef JP
			msg_print("���̑����ł͂��܂��ˌ����ł��Ȃ��B");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return FALSE;
	}


	/* Require a launcher */
	if (!j_ptr->tval)
	{
#ifdef JP
		msg_print("�ˌ��p�̕���������Ă��Ȃ��B");
#else
		msg_print("You have nothing to fire with.");
#endif
		flush();
		return FALSE;
	}

/*
	///mod131223
	if (j_ptr->tval ==TV_GUN && j_ptr->sval == SV_FIRE_GUN_CRIMSON
		|| j_ptr->tval ==TV_GUN && j_ptr->sval == SV_FIRE_GUN_MUSKET)
	//if (j_ptr->sval == SV_CRIMSON)
	{
#ifdef JP
		msg_print("���̕���͔������Ďg�����̂̂悤���B");
#else
		msg_print("Do activate.");
#endif
		flush();
		return;
	}
*/

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	//��\��o�����t���f
	stop_tsukumo_music();

	///mod160503 �e�������A�e������p���[�`���֔��
	if(CHECK_USE_GUN)
	{
		bool righthand = FALSE;
		bool lefthand = FALSE;
		int dir;
		int timeout_base, timeout_max;
		object_type *o_ptr;

		if(inventory[INVEN_RARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_RARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
			if(o_ptr->timeout <= timeout_max) righthand = TRUE;
		}
		if(inventory[INVEN_LARM].tval == TV_GUN)
		{
			o_ptr = &inventory[INVEN_LARM];
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
			if(o_ptr->timeout <= timeout_max) lefthand = TRUE;
		}

		if(!righthand && !lefthand)
		{
			msg_print("�܂��e�e�̑��U���ς�ł��Ȃ��B");
			return FALSE;
		}
		if (!get_aim_dir(&dir)) return FALSE;

		if(righthand)do_cmd_fire_gun_aux(INVEN_RARM,dir);
		if(dir == 5 && !target_okay()) lefthand = FALSE;
		if(lefthand)do_cmd_fire_gun_aux(INVEN_LARM,dir);

		//���������ǂ������肷�邽�߂�energy_use���g��
		if(energy_use) return TRUE;
		else return FALSE;
	}

	/* Require proper missile */
	///item �������Ă���|�ɓK���������
	item_tester_tval = p_ptr->tval_ammo;

	/* Get an item */
#ifdef JP
	q = "�ǂ�������܂���? ";
	s = "���˂����A�C�e��������܂���B";
#else
	q = "Fire which item? ";
	s = "You have nothing to fire.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
	{
		flush();
		return FALSE;
	}

	/* Fire the item */
	do_cmd_fire_aux(item, j_ptr);



	//v1.1.74 �|�̔������ʂŃZ�C���g�X�^�[�A���[��ǉ������̂ŕ���
	if (snipe_type == SP_FINAL)
	{
#ifdef JP
		msg_print("�ˌ��̔������̂��P�����B");
#else
		msg_print("A reactionary of shooting attacked you. ");
#endif
		(void)set_slow(p_ptr->slow + randint0(7) + 7, FALSE);
		(void)set_stun(p_ptr->stun + randint1(25));
	}



	return TRUE;

#if 0
	///class �X�i�C�p�[�̎ˌ�����ꏈ��
	if (!is_fired || p_ptr->pclass != CLASS_SNIPER) return;

	/*:::�X�i�C�p�[�̓���ˌ�����*/
	/* Sniper actions after some shootings */
	if ((p_ptr->pclass == CLASS_SNIPER) && snipe_type == SP_AWAY)
	{
		teleport_player(10 + (p_ptr->concent * 2), 0L);
	}
	if (snipe_type == SP_FINAL)
	{
#ifdef JP
		msg_print("�ˌ��̔������̂��P�����B");
#else
		msg_print("A reactionary of shooting attacked you. ");
#endif
		(void)set_slow(p_ptr->slow + randint0(7) + 7, FALSE);
		(void)set_stun(p_ptr->stun + randint1(25));
	}

#endif

}

///mod131224
static bool item_tester_hook_boomerang(object_type *o_ptr)
{
	//if ((o_ptr->tval==TV_DIGGING) || (o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM) || (o_ptr->tval == TV_HAFTED)) return (TRUE);
	if(object_is_melee_weapon(o_ptr)) return (TRUE);
	/* Assume not */
	return (FALSE);
}


/*
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
/*:::����*/
/*:::mult:�����p���[�@���͓����ψوȊO��1*/
/*:::boomerang:���p�Ƃ́u�u�[�������v�̂Ƃ�TRUE*/
/*:::shriken �E�҂̔����藠���̂Ƃ��C���x���g���ԍ��A����ȊO�̂Ƃ�-1�������Ă�B�F����俎q���Z�ɂ����꓊���̂Ƃ�-2*/

bool do_cmd_throw_aux(int mult, bool boomerang, int shuriken)
{
	int dir, item;
	int i, j, y, x, ty, tx, prev_y, prev_x;
	int ny[19], nx[19];
	int chance, tdam, tdis;
	int mul, div, dd, ds;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool equiped_item = FALSE;
	bool return_when_thrown = FALSE;
	bool suitable_item = FALSE;

	bool sumireko_throwing = FALSE;//俎q���꓊���@�K���߂��Ă���

	char o_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[TR_FLAG_SIZE];
	cptr q, s;
	bool come_back = FALSE;
	bool do_drop = TRUE;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		msg_print("���̎p�ł͓������ł��Ȃ��B");
		return FALSE;
	}

	if(p_ptr->pclass == CLASS_SUMIREKO && shuriken == -2 && p_ptr->lev > 34) sumireko_throwing = TRUE;

	/*:::�E�҂̔����藠���̂Ƃ��͓�����A�C�e�������܂��Ă���*/
	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	if (shuriken >= 0)
	{
		item = shuriken;
	}
	/*:::���p�Ƃ��u�[�������̋Z���g���Ƃ��͎�Ɏ����Ă镐��𓊂���@�񓁗��̎��͑I��*/
	else if (boomerang)
	{
		if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
		{
			item_tester_hook = item_tester_hook_boomerang;
#ifdef JP
			q = "�ǂ̕���𓊂��܂���? ";
			s = "�����镐�킪�Ȃ��B";
#else
			q = "Throw which item? ";
			s = "You have nothing to throw.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP)))
			{
				flush();
				return FALSE;
			}
		}
		else if (buki_motteruka(INVEN_LARM)) item = INVEN_LARM;
		else item = INVEN_RARM;
	}
	/*:::����ȊO�̎��͓�������̂�I��*/
	else
	{
		/* Get an item */
#ifdef JP
		q = "�ǂ̃A�C�e���𓊂��܂���? ";
		s = "������A�C�e�����Ȃ��B";
#else
		q = "Throw which item? ";
		s = "You have nothing to throw.";
#endif

		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR | USE_EQUIP)))
		{
			flush();
			return FALSE;
		}
	}

	/* Access the item (if in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}

	///mod140415 �O���Ȃ������𓊂��Ȃ��悤�ɂ���
	if(item >= INVEN_RARM && !wield_check(item,INVEN_PACK)) return FALSE;

	/* Item is cursed */
	/*:::�������̂��̂𓊂��悤�Ƃ��Ď���Ă���ꍇ*/
	if (object_is_cursed(o_ptr) && (item >= INVEN_RARM))
	{
		/* Oops */
#ifdef JP
		msg_print("�Ӂ[�ށA�ǂ�������Ă���悤���B");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif

		/* Nope */
		return FALSE;
	}
	/*:::�A���[�i�ł̓u�[�������ȊO�g���Ȃ�*/
	//v1.1.51 ����
#if 0
	if (p_ptr->inside_arena && !boomerang)
	{
		//if (o_ptr->tval != TV_SPIKE)
		{
#ifdef JP
			msg_print("�A���[�i�ł̓A�C�e�����g���Ȃ��I");
#else
			msg_print("You're in the arena now. This is hand-to-hand!");
#endif
			msg_print(NULL);

			/* Nope */
			return FALSE;
		}
	}
#endif

	if(sumireko_throwing && object_is_weapon(o_ptr)) boomerang = TRUE;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Extract the thrown object's flags. */
	object_flags(q_ptr, flgs);
	/*:::�����܂p�t���O����*/
	torch_flags(q_ptr, flgs);

	if(have_flag(flgs, TR_THROW)) suitable_item = TRUE;

	/* Distribute the charges of rods/wands between the stacks */
	/*:::���@�_�⃍�b�h�𓊂���Ƃ��Ɏc�������̏[�U���⎞�Ԃ����炷����*/
	distribute_charges(o_ptr, q_ptr, 1);

	/* Single object */
	/*:::������̂͏�Ɉ��*/
	q_ptr->number = 1;

	/* Description */
	object_desc(o_name, q_ptr, OD_OMIT_PREFIX);

	/*:::���͓����̎w�ւ̌���*/
	if (p_ptr->mighty_throw) mult += 2;


	/* Extract a "distance multiplier" */
	/* Changed for 'launcher' mutation */
	mul = 10 + 2 * (mult - 1);

	/* Enforce a minimum "weight" of one pound */
	/*:::�Œ�d��10�ɌŒ�@�d��10��1�|���h(0.5kg)����*/
	///mod140901 �˒��Z�������B����d�ʑS�̓I�Ɍy����������␳
	//div = ((q_ptr->weight > 10) ? q_ptr->weight : 10);
	div = (((q_ptr->weight * 2) > 10) ? (q_ptr->weight * 2) : 10);
	if ((have_flag(flgs, TR_THROW)) || boomerang) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[p_ptr->stat_ind[A_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if (tdis > mul) tdis = mul;

	if(tdis > 18) tdis = 18;
	if(tdis < 1) tdis = 1;

	if(sumireko_throwing) tdis = 18;

	/*:::�����藠���̏ꍇ���̈ʒu����}50��xy���^�[�Q�b�g�ɂȂ�*/
	if (shuriken >= 0)
	{
		ty = randint0(101)-50+py;
		tx = randint0(101)-50+px;
	}
	/*:::����ȊO�̓^�[�Q�b�g��I��*/
	else
	{
		project_length = tdis + 1;

		/* Get a direction (or cancel) */
		if (!get_aim_dir(&dir)) return FALSE;

		/* Predict the "target" location */
		/*:::�Ȃ�99�ȂǊ|����̂��낤���H�˒����ς�����Ƃ肠�����傫�ڂɂ��Ă���H
		 *:::�^�[�Q�b�g������Ƃ��͕ʏ����Ȃ̂ł���Ŏx��Ȃ��̂�*/
		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];

		/* Check for "target request" */
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}

		project_length = 0;  /* reset to default */
	}
	/*:::�g�[���n���}�[�A�C�[�W�X�t�@���O�A�u�[�������̃t���O*/
	///mod140830 �����Ă��Ԃ��Ă���A�C�e�����t���O�ɕύX
	//if ((q_ptr->name1 == ART_MJOLLNIR) ||
	//    (q_ptr->name1 == ART_AEGISFANG) || boomerang)
	if ( have_flag(flgs, TR_BOOMERANG) || boomerang)
		return_when_thrown = TRUE;

	/*:::�A�C�e�������A�u�܂����́��������Ă���v�Ȃǂ̋L�q*/
	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		if (!return_when_thrown)
			inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}
	/*:::�����i�𓊂����Ƃ�*/
	if (item >= INVEN_RARM)
	{
		equiped_item = TRUE;
		p_ptr->redraw |= (PR_EQUIPPY);
	}

	/* Take a turn */
	///mod140830 �����Z�\�������Ƒ��x���オ�邱�Ƃɂ��� �ő�O�{���x
	energy_use = 100;
	if(ref_skill_exp(SKILL_THROWING) > 1600)
	{
		int dec;

		if(suitable_item)	dec = (ref_skill_exp(SKILL_THROWING) - 1600) / 100;
		else dec = (ref_skill_exp(SKILL_THROWING) -1600) / 128;

		if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE && p_ptr->pclass == CLASS_REIMU)
			dec += osaisen_rank() * 2;

		energy_use -= dec;
	}
	if(p_ptr->pseikaku == SEIKAKU_TRIGGER) energy_use -= energy_use / 5;

	if(energy_use < 20) energy_use = 20;//paranoia


	///class�@�������x�{�[�i�X ��̑��x�{�[�i�X����ꂽ�̂ō폜����
	/*:::�����ƔE�҂͑��x�Ƀ{�[�i�X*/
	/* Rogue and Ninja gets bonus */
	//if ((p_ptr->pclass == CLASS_ROGUE) || (p_ptr->pclass == CLASS_NINJA))
	//	energy_use -= p_ptr->lev;

	/* Start at the player */
	y = py;
	x = px;


	/* Hack -- Handle stuff */
	handle_stuff();
	/*:::�����A�C�e���̌��肪�ς񂾂̂�shriken��ʂ̃t���O�Ƃ��čė��p���Ă���炵��*/
	/*:::�����đ傫�ȃ_���[�W�̕���͔E�҂�������Ƃ���ɍ��{�[�i�X*/
	///class item ���������̕���ƔE��
	///mod131223 tval
	//if ((p_ptr->pclass == CLASS_NINJA) && ((q_ptr->tval == TV_SPIKE) || ((have_flag(flgs, TR_THROW)) && (q_ptr->tval == TV_SWORD)))) shuriken = TRUE;
	///mod140830 suitable_item�Ƃ��ăt���O�𕪂��A�E�ɂ�����炸������������̏����͍s����悤�ɂ���
	//if ((p_ptr->pclass == CLASS_NINJA) && (((have_flag(flgs, TR_THROW)) && (q_ptr->tval == TV_KNIFE)))) shuriken = TRUE;
	//else shuriken = FALSE;

	/* Chance of hitting */
	/*:::���������̕���͖�������ɕ��펩�g�̖������������*/
	if (suitable_item) chance = ((p_ptr->skill_tht) +
		((p_ptr->to_h_b + q_ptr->to_h) * BTH_PLUS_ADJ));
	else chance = (p_ptr->skill_tht + (p_ptr->to_h_b * BTH_PLUS_ADJ));

	/*:::�E�҂����������̂��̂𓊂����ꍇ����������*/
	//if (shuriken) chance *= 2;
	///�E�łȂ������X�L���ő�p���邱�Ƃɂ��� �ő��{
	if(suitable_item) chance += chance * ref_skill_exp(SKILL_THROWING) / 8000;
	else chance += chance * ref_skill_exp(SKILL_THROWING) / 16000;

	///mod140928 ���|��Ԃ��Ɩ���������
	if(p_ptr->afraid) chance = chance * 2 / 3;

	//�u���̐��E�v
	if(SAKUYA_WORLD)
	{
		sakuya_time_stop(FALSE);
	}

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	/* Travel until stopped */
	/*:::���������̂���ԏ���*/
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], py, px, ty, tx);

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			/*:::�ǂɓ������āA�������̂���A�l�`�A���邢�͂���ȊO�𓊂��ĕǂ̒��Ƀ����X�^�[�����Ȃ������ꍇ���[�v�𔲂���*/
			if ((q_ptr->tval == TV_FIGURINE) || object_is_potion(q_ptr) || !cave[ny[cur_dis]][nx[cur_dis]].m_idx) break;
		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny[cur_dis], nx[cur_dis]) && player_can_see_bold(ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(c, a, ny[cur_dis], nx[cur_dis]);
			move_cursor_relative(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx[cur_dis];
		y = ny[cur_dis];

		/* Advance the distance */
		cur_dis++;

		/* Monster here, Try to hit it */
		if (cave[y][x].m_idx)
		{
			cave_type *c_ptr = &cave[y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			int mon_ac = r_ptr->ac;

			//v1.1.94 �����X�^�[�h��͒ቺ����AC25%�J�b�g
			if (MON_DEC_DEF(m_ptr))
			{
				mon_ac = MONSTER_DECREASED_AC(mon_ac);
			}


			/* Check the visibility */
			visible = m_ptr->ml;

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if (test_hit_fire(chance - cur_dis, mon_ac, m_ptr->ml))
			{
				bool fear = FALSE;

				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
#ifdef JP
					msg_format("%s���G��ߑ������B", o_name);
#else
					msg_format("The %s finds a mark.", o_name);
#endif

				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, m_ptr, 0);

					/* Message */
#ifdef JP
					msg_format("%s��%s�ɖ��������B", o_name, m_name);
#else
					msg_format("The %s hits %s.", o_name, m_name);
#endif

					if (m_ptr->ml)
					{
						/* Hack -- Track this monster race */
						if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

						/* Hack -- Track this monster */
						health_track(c_ptr->m_idx);
					}
				}
				//v1.1.27 �e���Ԃ񓊂����Ƃ��͕ʏ����ɂ���
				if(q_ptr->tval == TV_GUN)
				{
					gun_throw_effect(y,x,q_ptr);
					//�����Əe�̗����̌o���l�𓾂邱�Ƃɂ��Ă���
					gain_skill_exp(SKILL_THROWING, m_ptr);
					gain_skill_exp(TV_GUN, m_ptr);
				}
				else
				{

					/* Hack -- Base damage from thrown object */
					dd = q_ptr->dd;
					ds = q_ptr->ds;
					torch_dice(q_ptr, &dd, &ds); /* throwing a torch */

					//�ޏ��n���j�𓊂���ƃ_�C�X�{�[�i�X
					if(q_ptr->tval == TV_OTHERWEAPON && q_ptr->sval == SV_OTHERWEAPON_NEEDLE 
						&&(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_SANAE) )
					{
						dd += 3;
					}
					//v1.1.21 ���m�Z���Z�\
					else if ((q_ptr->tval == TV_KNIFE) && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_KNIFE_MASTERY))
					{
						dd += 2;
					
					}

					if(suitable_item && object_is_artifact(q_ptr)) dd *= 2; //���������̃A�[�e�B�t�@�N�g�𓊂������̃{�[�i�X�ǉ�
				
					tdam = damroll(dd, ds);
					/* Apply special damage XXX XXX XXX */
					tdam = tot_dam_aux(q_ptr, tdam, m_ptr, 0, TRUE);
					tdam = critical_shot(q_ptr->weight, q_ptr->to_h, tdam);
					if (q_ptr->to_d > 0)
						tdam += q_ptr->to_d;
					else
						tdam += -q_ptr->to_d;

					if (!sumireko_throwing && boomerang)
					{
						tdam *= (mult+p_ptr->num_blow[item - INVEN_RARM]);
						tdam += p_ptr->to_d_m;
					}
					else if (suitable_item)
					{
						tdam *= (2+mult);
						tdam += p_ptr->to_d_m;
					}
					else
					{
						tdam *= mult;
					}

					//if (shuriken)
					//{
					//	tdam += ((p_ptr->lev+30)*(p_ptr->lev+30)-900)/55;
					//}

					/* No negative damage */
					if (tdam < 0) tdam = 0;

					///mod150620 �_���[�W�ɏd�ʃ{�[�i�X�����悤�ɂ��Ă݂�
					tdam += damroll(MAX(1,(tdis / 3)), MAX(q_ptr->weight/10,1));


					/* Modify the damage */
					tdam = mon_damage_mod(m_ptr, tdam, FALSE);

					/* Complex message */
					if (p_ptr->wizard)
					{
	#ifdef JP
						msg_format("%d/%d�̃_���[�W��^�����B",
							   tdam, m_ptr->hp);
	#else
						msg_format("You do %d (out of %d) damage.",
							   tdam, m_ptr->hp);
	#endif

					}
					//�����X�L���𓾂�
					gain_skill_exp(SKILL_THROWING, m_ptr);

					/* Hit the monster, check for death */
					if (mon_take_hit(c_ptr->m_idx, tdam, &fear, extract_note_dies(real_r_ptr(m_ptr))))
					{
						/* Dead monster */
					}

					/* No death */
					else
					{
						/* Message */
						message_pain(c_ptr->m_idx, tdam);

						/* Anger the monster */
						if ((tdam > 0) && !object_is_potion(q_ptr))
							anger_monster(m_ptr);

						/* Take note */
						if (fear && m_ptr->ml)
						{
							char m_name[80];

							/* Sound */
							sound(SOUND_FLEE);

							/* Get the monster name (or "it") */
							monster_desc(m_name, m_ptr, 0);

							/* Message */
	#ifdef JP
							msg_format("%^s�͋��|���ē����o�����I", m_name);
	#else
							msg_format("%^s flees in terror!", m_name);
	#endif

						}
					}

				}
			}

			/* Stop looking */
			break;
		}
	}

	/* decrease toach's fuel */
	if (hit_body) torch_lost_fuel(q_ptr);

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(q_ptr) : 0);

	/* Figurines transform */
	if ((q_ptr->tval == TV_FIGURINE) && !(p_ptr->inside_arena))
	{
		j = 100;

		if (!(summon_named_creature(0, y, x, q_ptr->pval,
					    !(object_is_cursed(q_ptr)) ? PM_FORCE_PET : 0L)))
#ifdef JP
msg_print("�l�`�͔P���Ȃ���ӂ��U���Ă��܂����I");
#else
			msg_print("The Figurine writhes and then shatters.");
#endif

		else if (object_is_cursed(q_ptr))
#ifdef JP
msg_print("����͂��܂�ǂ��Ȃ��C������B");
#else
			msg_print("You have a bad feeling about this.");
#endif

	}

	//v1.1.43 �I�j�t�X�x�͔j�􂵂č����{�[������
	if (q_ptr->tval == TV_MUSHROOM && q_ptr->sval == SV_MUSHROOM_PUFFBALL)
	{
		if (hit_body || hit_wall)
		{
			msg_format("%s�͔j�􂵂��I", o_name);
			project(0, 3, y, x, damroll(8, 12), GF_CONFUSION, (PROJECT_JUMP | PROJECT_KILL), -1);
			do_drop = FALSE;
		}
		else j = 0;
	}


	/* Potions smash open */
	if (object_is_potion(q_ptr))
	{
		if (hit_body || hit_wall || (randint1(100) < j))
		{
			/* Message */
#ifdef JP
			msg_format("%s�͍ӂ��U�����I", o_name);
#else
			msg_format("The %s shatters!", o_name);
#endif


			if (potion_smash_effect(0, y, x, q_ptr->k_idx))
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				/* ToDo (Robert): fix the invulnerability */
				if (cave[y][x].m_idx &&
				    is_friendly(&m_list[cave[y][x].m_idx]) &&
				    !MON_INVULNER(m_ptr) &&
					m_ptr->r_idx != MON_MASTER_KAGUYA
					)
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
					msg_format("%s�͓{�����I", m_name);
#else
					msg_format("%^s gets angry!", m_name);
#endif

					set_hostile(&m_list[cave[y][x].m_idx]);
				}
			}
			do_drop = FALSE;
		}
		else
		{
			j = 0;
		}
	}

	if (return_when_thrown)
	{
		int back_chance = randint1(30)+20+((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
		char o2_name[MAX_NLEN];
		
		//�u�[�������t���O�t���́��������p�Ɠ��Z�u�u�[�������v�œ�������K���Ԃ��Ă���
		if (have_flag(flgs, TR_BOOMERANG) && object_is_artifact(q_ptr) && boomerang) come_back = TRUE;

		//俎q���꓊���͕K���Ԃ��Ă���
		if(sumireko_throwing) come_back = TRUE;

		//�����n���x�������ꍇ�K���Ԃ��Ă���
		if (ref_skill_exp(SKILL_THROWING) >= 5600) come_back = TRUE;

		if (cheat_xtra && come_back) msg_print("(come back)");

		j = -1;
		if (boomerang) back_chance += 4+randint1(5);
		object_desc(o2_name, q_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		if((back_chance > 30) && !one_in_(100) || come_back)
		{
			for (i = cur_dis - 1; i > 0; i--)
			{
				if (panel_contains(ny[i], nx[i]) && player_can_see_bold(ny[i], nx[i]))
				{
					char c = object_char(q_ptr);
					byte a = object_attr(q_ptr);

					/* Draw, Hilite, Fresh, Pause, Erase */
					print_rel(c, a, ny[i], nx[i]);
					move_cursor_relative(ny[i], nx[i]);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(ny[i], nx[i]);
					Term_fresh();
				}
				else
				{
					/* Pause anyway, for consistancy */
					Term_xtra(TERM_XTRA_DELAY, msec);
				}
			}
			//�K���߂��Ă���ꍇ���b�Z�[�W��ς���
			if (come_back)
			{
				if (item >= 0)
				{
					msg_format("%s�͋z�����܂��悤�ɂ��Ȃ��̎�̒��ɔ[�܂����B", o2_name);
				}
				else
				{
					msg_format("%s�͂��Ȃ��̑����֐Â��Ɋ��荞�񂾁B", o2_name);
					y = py;
					x = px;
				}

			}

			else if((back_chance > 37) && !p_ptr->blind && (item >= 0))
			{
#ifdef JP
				msg_format("%s���茳�ɕԂ��Ă����B", o2_name);
#else
				msg_format("%s comes back to you.", o2_name);
#endif
				come_back = TRUE;
			}
			else
			{
				if (item >= 0)
				{
#ifdef JP
					msg_format("%s���󂯑��˂��I", o2_name);
#else
					msg_format("%s backs, but you can't catch!", o2_name);
#endif
				}
				else
				{
#ifdef JP
					msg_format("%s���Ԃ��Ă����B", o2_name);
#else
					msg_format("%s comes back.", o2_name);
#endif
				}
				y = py;
				x = px;
			}
		}
		else
		{
#ifdef JP
			msg_format("%s���Ԃ��Ă��Ȃ������I", o2_name);
#else
			msg_format("%s doesn't back!", o2_name);
#endif
		}
	}

	if (come_back)
	{
		if (item == INVEN_RARM || item == INVEN_LARM)
		{
			/* Access the wield slot */
			o_ptr = &inventory[item];

			/* Wear the new stuff */
			object_copy(o_ptr, q_ptr);

			/* Increase the weight */
			p_ptr->total_weight += q_ptr->weight;

			/* Increment the equip counter by hand */
			equip_cnt++;

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Recalculate torch */
			p_ptr->update |= (PU_TORCH);

			/* Recalculate mana XXX */
			p_ptr->update |= (PU_MANA);

			/* Window stuff */
			p_ptr->window |= (PW_EQUIP);
		}
		else
		{
			inven_carry(q_ptr);
		}
		do_drop = FALSE;
	}
	else if (equiped_item)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}

	/* Drop (or break) near that location */
	if (do_drop)
	{
		if (cave_have_flag_bold(y, x, FF_PROJECT))
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, y, x);
		}
		else
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, prev_y, prev_x);
		}
	}

	//v1.1.30 �L�X����������
	if (p_ptr->pclass == CLASS_KISUME) reset_concent = FALSE;

	return TRUE;
}


/*
 * Throw an object from the pack or floor.
 */
void do_cmd_throw(void)
{
	do_cmd_throw_aux(1, FALSE, -1);
}


#ifdef TRAVEL
/*
 * Hack: travel command
 */
#define TRAVEL_UNABLE 9999

static int flow_head = 0;
static int flow_tail = 0;
static s16b temp2_x[MAX_SHORT];
static s16b temp2_y[MAX_SHORT];

/* Hack: forget the "flow" information */
//�S�O���b�h�ɐݒ肳�ꂽ�g���x���R�}���h�p�ړ��R�X�g�l�����Z�b�g����
void forget_travel_flow(void)
{
	int x, y;

	/* Check the entire dungeon */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Forget the old data */
			travel.cost[y][x] = MAX_SHORT;
		}	
	}
}

//�n�`�ɉ������ړ��R�X�g�l���v�Z����
static int travel_flow_cost(int y, int x)
{
	feature_type *f_ptr = &f_info[cave[y][x].feat];
	int cost = 1;

	/* Avoid obstacles (ex. trees) */
	if (have_flag(f_ptr->flags, FF_AVOID_RUN)) cost += 1;

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		if (have_flag(f_ptr->flags, FF_DEEP) && !p_ptr->levitation) cost += 5;
	}

	/* Lava */
	if (have_flag(f_ptr->flags, FF_LAVA))
	{
		int lava = 2;
		if (!p_ptr->resist_fire) lava *= 2;
		if (!p_ptr->levitation) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}

	//v1.1.85 �n�`�ǉ�
	if (have_flag(f_ptr->flags, FF_ACID_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_acid) lava *= 2;
		if (!p_ptr->levitation) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}
	if (have_flag(f_ptr->flags, FF_POISON_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_pois) lava *= 2;
		if (!p_ptr->levitation) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}
	if (have_flag(f_ptr->flags, FF_COLD_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_cold) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}
	if (have_flag(f_ptr->flags, FF_ELEC_PUDDLE))
	{
		int lava = 2;
		if (!p_ptr->resist_elec) lava *= 2;
		if (have_flag(f_ptr->flags, FF_DEEP)) lava *= 2;

		cost += lava;
	}






	/* Detected traps and doors */
	if (cave[y][x].info & (CAVE_MARK))
	{
		if (have_flag(f_ptr->flags, FF_DOOR)) cost += 1;
		if (have_flag(f_ptr->flags, FF_TRAP)) cost += 10;
	}

	return (cost);
}

//�^����ꂽ�R�X�g�l�ɒn�`�ɉ������R�X�g�����l�����Z����travel.cost[][]�Ɋi�[����
static void travel_flow_aux(int y, int x, int n, bool wall)
{
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int old_head = flow_head;
	int add_cost = 1;
	int base_cost = (n % TRAVEL_UNABLE);
	int from_wall = (n / TRAVEL_UNABLE);
	int cost;

	/* Ignore out of bounds */
	if (!in_bounds(y, x)) return;

	/* Ignore unknown grid except in wilderness */
	if (dun_level > 0 && !(c_ptr->info & CAVE_KNOWN)) return;

	/* Ignore "walls" and "rubble" (include "secret doors") */
	if (have_flag(f_ptr->flags, FF_WALL) ||
		have_flag(f_ptr->flags, FF_CAN_DIG) ||
		(have_flag(f_ptr->flags, FF_DOOR) && cave[y][x].mimic) ||
		(!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !p_ptr->levitation))
	{
		if (!wall || !from_wall) return;
		add_cost += TRAVEL_UNABLE;
	}
	else
	{
		add_cost = travel_flow_cost(y, x);
	}

	cost = base_cost + add_cost;

	/* Ignore lower cost entries */
	if (travel.cost[y][x] <= cost) return;

	/* Save the flow cost */
	travel.cost[y][x] = cost;

	/* Enqueue that entry */
	temp2_y[flow_head] = y;
	temp2_x[flow_head] = x;

	/* Advance the queue */
	if (++flow_head == MAX_SHORT) flow_head = 0;

	/* Hack -- notice overflow by forgetting new entry */
	if (flow_head == flow_tail) flow_head = old_head;

	return;
}

/*:::���Wy,x�ւ̓��B�R�X�g��S�O���b�h�ɑ΂��Čv�Z����*/
///mod160722 �P����Z�Ŏg������extern
void travel_flow(int ty, int tx)
{
	int x, y, d;
	bool wall = FALSE;
	feature_type *f_ptr = &f_info[cave[py][px].feat];

	/* Reset the "queue" */
	flow_head = flow_tail = 0;

	/* is player in the wall? */
	if (!have_flag(f_ptr->flags, FF_MOVE)) wall = TRUE;

	/* Start at the target grid */
	travel_flow_aux(ty, tx, 0, wall);

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		y = temp2_y[flow_tail];
		x = temp2_x[flow_tail];

		/* Forget that entry */
		if (++flow_tail == MAX_SHORT) flow_tail = 0;

		/* Ignore too far entries */
		//if (distance(ty, tx, y, x) > 100) continue;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			/* Add that child if "legal" */
			travel_flow_aux(y + ddy_ddd[d], x + ddx_ddd[d], travel.cost[y][x], wall);
		}
	}

	/* Forget the flow info */
	flow_head = flow_tail = 0;
}

/*:::�g���x���R�}���h*/
//'`'�R�}���h�B�ړI�n��I�����Atravel.cost[][]���Čv�Z���Atravel.run��255���i�[����
//���ۂ̈ړ���travel_step()�ōs��
void do_cmd_travel(void)
{
	int x, y, i;
	int dx, dy, sx, sy;
	feature_type *f_ptr;

	if (!tgt_pt(&x, &y)) return;

	if ((x == px) && (y == py))
	{
#ifdef JP
		msg_print("���łɂ����ɂ��܂��I");
#else
		msg_print("You are already there!!");
#endif
		return;
	}

	f_ptr = &f_info[cave[y][x].feat];

	if ((cave[y][x].info & CAVE_MARK) &&
		(have_flag(f_ptr->flags, FF_WALL) ||
			have_flag(f_ptr->flags, FF_CAN_DIG) ||
			(have_flag(f_ptr->flags, FF_DOOR) && cave[y][x].mimic)))
	{
#ifdef JP
		msg_print("�����ɂ͍s�����Ƃ��ł��܂���I");
#else
		msg_print("You cannot travel there!");
#endif
		return;
	}

	travel.x = x;
	travel.y = y;

	forget_travel_flow();
	travel_flow(y, x);

	/* Travel till 255 steps */
	travel.run = 255;

	/* Paranoia */
	travel.dir = 0;

	/* Decides first direction */
	dx = abs(px - x);
	dy = abs(py - y);
	sx = ((x == px) || (dx < dy)) ? 0 : ((x > px) ? 1 : -1);
	sy = ((y == py) || (dy < dx)) ? 0 : ((y > py) ? 1 : -1);

	for (i = 1; i <= 9; i++)
	{
		if ((sx == ddx[i]) && (sy == ddy[i])) travel.dir = i;
	}
}
#endif
