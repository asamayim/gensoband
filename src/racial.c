/* File: racial.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Racial powers (and mutations) */

#include "angband.h"

/*
 * Hook to determine if an object is contertible in an arrow/bolt
 */
/*:::��ʐ����̂��߂�hook*/
///del131221 ����
#if 0
static bool item_tester_hook_convertible(object_type *o_ptr)
{
	if((o_ptr->tval==TV_JUNK) || (o_ptr->tval==TV_SKELETON)) return TRUE;

	if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON)) return TRUE;
	/* Assume not */
	return (FALSE);
}
#endif

/*
 * do_cmd_cast calls this function if the player's class
 * is 'archer'.
 */
/*:::�A�[�`���[�̖�ʐ���*/
///sysdel class �A�[�`���[��ʐ���

#if 0
static bool do_cmd_archer(void)
{
	int ext=0;
	char ch;

	object_type	forge;
	object_type     *q_ptr;

	char com[80];
	char o_name[MAX_NLEN];

	q_ptr = &forge;

	if(p_ptr->lev >= 20)
#ifdef JP
		sprintf(com, "[S]�e, [A]��, [B]�N���X�{�E�̖� :");
#else
		sprintf(com, "Create [S]hots, Create [A]rrow or Create [B]olt ?");
#endif
	else if(p_ptr->lev >= 10)
#ifdef JP
		sprintf(com, "[S]�e, [A]��:");
#else
		sprintf(com, "Create [S]hots or Create [A]rrow ?");
#endif
	else
#ifdef JP
		sprintf(com, "[S]�e:");
#else
		sprintf(com, "Create [S]hots ?");
#endif

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă�I");
#else
		msg_print("You are too confused!");
#endif
		return FALSE;
	}

	if (p_ptr->blind)
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��I");
#else
		msg_print("You are blind!");
#endif
		return FALSE;
	}

	while (TRUE)
	{
		if (!get_com(com, &ch, TRUE))
		{
			return FALSE;
		}
		if (ch == 'S' || ch == 's')
		{
			ext = 1;
			break;
		}
		if ((ch == 'A' || ch == 'a')&&(p_ptr->lev >= 10))
		{
			ext = 2;
			break;
		}
		if ((ch == 'B' || ch == 'b')&&(p_ptr->lev >= 20))
		{
			ext = 3;
			break;
		}
	}

	/**********Create shots*********/
	if (ext == 1)
	{
		int x,y, dir;
		cave_type *c_ptr;

		if (!get_rep_dir(&dir, FALSE)) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];
		c_ptr = &cave[y][x];

		if (!have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_CAN_DIG))
		{
#ifdef JP
			msg_print("�����ɂ͊�΂��Ȃ��B");
#else
			msg_print("You need pile of rubble.");
#endif
			return FALSE;
		}
		else if (!cave_have_flag_grid(c_ptr, FF_CAN_DIG) || !cave_have_flag_grid(c_ptr, FF_HURT_ROCK))
		{
#ifdef JP
			msg_print("�d�����ĕ����Ȃ������B");
#else
			msg_print("You failed to make ammo.");
#endif
		}
		else
		{
			s16b slot;

			/* Get local object */
			q_ptr = &forge;

			/* Hack -- Give the player some small firestones */
			object_prep(q_ptr, lookup_kind(TV_SHOT, m_bonus(1, p_ptr->lev) + 1));
			q_ptr->number = (byte)rand_range(15,30);
			object_aware(q_ptr);
			object_known(q_ptr);
			apply_magic(q_ptr, p_ptr->lev, AM_NO_FIXED_ART);
			q_ptr->discount = 99;

			slot = inven_carry(q_ptr);

			object_desc(o_name, q_ptr, 0);
#ifdef JP
			msg_format("%s��������B", o_name);
#else
			msg_print("You make some ammo.");
#endif

			/* Auto-inscription */
			if (slot >= 0) autopick_alter_item(slot, FALSE);

			/* Destroy the wall */
			cave_alter_feat(y, x, FF_HURT_ROCK);

			p_ptr->update |= (PU_FLOW);
		}
	}
	/**********Create arrows*********/
	else if (ext == 2)
	{
		int item;
		cptr q, s;
		s16b slot;
		///del131221 ���̔p�~�Ƌ��ɖ�ʐ���������
		msg_format("���͔̂p�~���ꂽ�B", o_name);
		return FALSE;
		//item_tester_hook = item_tester_hook_convertible;

		/* Get an item */
#ifdef JP
		q = "�ǂ̃A�C�e��������܂����H ";
		s = "�ޗ��������Ă��Ȃ��B";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			q_ptr = &inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			q_ptr = &o_list[0 - item];
		}

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(q_ptr, lookup_kind(TV_ARROW, m_bonus(1, p_ptr->lev)+ 1));
		q_ptr->number = (byte)rand_range(5, 10);
		object_aware(q_ptr);
		object_known(q_ptr);
		apply_magic(q_ptr, p_ptr->lev, AM_NO_FIXED_ART);

		q_ptr->discount = 99;

		object_desc(o_name, q_ptr, 0);
#ifdef JP
		msg_format("%s��������B", o_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}

		slot = inven_carry(q_ptr);

		/* Auto-inscription */
		if (slot >= 0) autopick_alter_item(slot, FALSE);
	}
	/**********Create bolts*********/
	else if (ext == 3)
	{
		int item;
		cptr q, s;
		s16b slot;
		///del131221 ���̔p�~�Ƌ��ɖ�ʐ���������
		msg_format("���͔̂p�~���ꂽ�B", o_name);
		return FALSE;

		//item_tester_hook = item_tester_hook_convertible;

		/* Get an item */
#ifdef JP
		q = "�ǂ̃A�C�e��������܂����H ";
		s = "�ޗ��������Ă��Ȃ��B";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			q_ptr = &inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			q_ptr = &o_list[0 - item];
		}

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(q_ptr, lookup_kind(TV_BOLT, m_bonus(1, p_ptr->lev)+1));
		q_ptr->number = (byte)rand_range(4, 8);
		object_aware(q_ptr);
		object_known(q_ptr);
		apply_magic(q_ptr, p_ptr->lev, AM_NO_FIXED_ART);

		q_ptr->discount = 99;

		object_desc(o_name, q_ptr, 0);
#ifdef JP
		msg_format("%s��������B", o_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}

		slot = inven_carry(q_ptr);

		/* Auto-inscription */
		if (slot >= 0) autopick_alter_item(slot, FALSE);
	}
	return TRUE;
}
#endif



/*:::������p�t�̖�����z��*/
///class
///system item ��▂�@�_�A���b�h�̎�ނ𑝌�����Ƃ�����
///mod150330 ���חp�ɏ����ǋL��ύX
bool gain_magic(void)
{
	int item;
	int pval;
	int ext = 0;
	cptr q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	/* Only accept legal items */
	/*:::��A���b�h�A���@�_*/
	item_tester_hook = item_tester_hook_staff_rod_wand;

	/* Get an item */
#ifdef JP
	if(p_ptr->pclass == CLASS_SEIJA)
	{
		q = "�ǂ̖�������B�������܂����H ";
		s = "������������Ă��Ȃ��B";
	}
	else
	{
		q = "�ǂ̃A�C�e���̖��͂���荞�݂܂���? ";
		s = "���͂���荞�߂�A�C�e�����Ȃ��B";
	}
#else
	q = "Gain power of which item? ";
	s = "You have nothing to gain power.";
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
	/*:::�A���f�b�h�̏����H���̏�*/
	if (o_ptr->tval == TV_STAFF && o_ptr->sval == SV_STAFF_NOTHING)
	{
#ifdef JP
		msg_print("���̏�ɂ͔����ׂ̈̔\�͉͂���������Ă��Ȃ��悤���B");
#else
		msg_print("This staff doesn't have any magical ability.");
#endif
		return FALSE;
	}


	if (!object_is_known(o_ptr))
	{
#ifdef JP
	if(p_ptr->pclass == CLASS_SEIJA)
		msg_print("�Ӓ肳��Ă��Ȃ��ƈ����Ȃ��B");
	else
		msg_print("�Ӓ肳��Ă��Ȃ��Ǝ�荞�߂Ȃ��B");
#else
		msg_print("You need to identify before absorbing.");
#endif
		return FALSE;
	}

	if (o_ptr->timeout)
	{
#ifdef JP
	if(p_ptr->pclass == CLASS_SEIJA)
		msg_print("�[�U���̃A�C�e���͈����Ȃ��B");
	else
		msg_print("�[�U���̃A�C�e���͎�荞�߂Ȃ��B");
#else
		msg_print("This item is still charging.");
#endif
		return FALSE;
	}

	pval = o_ptr->pval;
	if (o_ptr->tval == TV_ROD)
		ext = EATER_EXT*2;
	else if (o_ptr->tval == TV_WAND)
		ext = EATER_EXT;

	if (o_ptr->tval == TV_ROD)
	{
		p_ptr->magic_num2[o_ptr->sval + ext] += o_ptr->number;
		if (p_ptr->magic_num2[o_ptr->sval + ext] > 99) p_ptr->magic_num2[o_ptr->sval + ext] = 99;
	}
	else
	{
		int num;
		for (num = o_ptr->number; num; num--)
		{
			int gain_num = pval;
			if (o_ptr->tval == TV_WAND) gain_num = (pval + num - 1) / num;
			if (p_ptr->magic_num2[o_ptr->sval + ext])
			{
				gain_num *= 256;
				gain_num = (gain_num/3 + randint0(gain_num/3)) / 256;
				if (gain_num < 1) gain_num = 1;
			}
			/*:::magic_num2[]�͍ő�񐔁Amagic_num1[]�͏[�U�󋵂炵��*/
			p_ptr->magic_num2[o_ptr->sval + ext] += gain_num;
			if (p_ptr->magic_num2[o_ptr->sval + ext] > 99) p_ptr->magic_num2[o_ptr->sval + ext] = 99;
			p_ptr->magic_num1[o_ptr->sval + ext] += pval * 0x10000;
			if (p_ptr->magic_num1[o_ptr->sval + ext] > 99 * 0x10000) p_ptr->magic_num1[o_ptr->sval + ext] = 99 * 0x10000;
			if (p_ptr->magic_num1[o_ptr->sval + ext] > p_ptr->magic_num2[o_ptr->sval + ext] * 0x10000) p_ptr->magic_num1[o_ptr->sval + ext] = p_ptr->magic_num2[o_ptr->sval + ext] * 0x10000;
			if (o_ptr->tval == TV_WAND) pval -= (pval + num - 1) / num;
		}
	}

	object_desc(o_name, o_ptr, 0);
	/* Message */
#ifdef JP
	if(p_ptr->pclass == CLASS_SEIJA)
	{
		msg_format("%s����������d�������񂾁B", o_name);
	}
	else
		msg_format("%s�̖��͂���荞�񂾁B", o_name);

#else
	msg_format("You absorb magic of %s.", o_name);
#endif

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -999);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -999);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
	energy_use = 100;
	return TRUE;
}




//v1.1.79 �����̊֐��Ŏg���]����I��phook

bool item_tester_hook_moving(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	object_flags(o_ptr, flgs);

	if (have_flag(flgs, TR_HOUSE)) return (TRUE);
	else return (FALSE);
}

//v1.1.79 �V�푰�u���~��炵�v�̈ړ����~�ύX����
//�ړ����~�A�C�e���͓���C���x���g��inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE]�Ɋi�[�����
//�����������s��������v�̂Ƃ�TRUE
bool zashiki_moving(void)
{
	int item;
	int pval;
	int ext = 0;
	cptr q, s;
	object_type forge;
	object_type *o_ptr,*q_ptr;
	char o_name[MAX_NLEN];


	if (p_ptr->prace != RACE_ZASHIKIWARASHI)
	{
		msg_print("ERROR:���~��炵�ȊO�̎푰����zashiki_moving()���Ă΂ꂽ");
		return FALSE;
	}

	q_ptr = &forge;
	//�t���OTR_HOUSE�����A�C�e�����w��
	item_tester_hook = item_tester_hook_moving;

	q = "�ǂ�Ɏ��߂��܂����H";
	s = "�]����ɓK�����A�C�e���������Ă��Ȃ��B";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);

	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];

	if (object_is_cursed(o_ptr))
	{
		msg_print("���̉��~�͎הO�ɖ����Ă���B��߂Ă������B");
		return FALSE;
	}

	//v1.1.85 *�Ӓ�*�ςɂ���
	identify_item(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	//�Â��A�C�e�����p�C���x���g������ꎞ�̈�փR�s�[
	object_copy(q_ptr, &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE]);
	
	//�V�����A�C�e�����p�C���x���g���փR�s�[(�ړ����~�͒��i���A�[�e�B�t�@�N�g�Ȃ̂Ō��͕K��1�̂͂������ꉞ������)
	object_copy(&inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE],o_ptr);
	inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE].number = 1;


	//�V�������~�ɓ��������̃R�����g�Ƃ��H
	object_desc(o_name, o_ptr, OD_NAME_AND_ENCHANT);
	msg_format("���Ȃ���%s�̒��ɔ�э��񂾁B",o_name);



	if (object_is_artifact(o_ptr))
	{
		if (o_ptr->name1 == ART_IBUKI)
			msg_print("���̕Z�\�̒��͘V�܂̎𑠂��I");

	}
	else if (o_ptr->tval == TV_SOUVENIR)
	{
		if(o_ptr->sval == SV_SOUVENIR_DOUJU)
			msg_print("���̚�̒��͘V�܂̖��≮���I");

		if (o_ptr->sval == SV_SOUVENIR_MORINJI)
			msg_print("������Ə��������B");

		if (o_ptr->sval == SV_SOUVENIR_BOB)
			msg_print("���͉��~�ɂȂ��Ă���B���ɐl�������������������Z��ł���...");

		if (o_ptr->sval == SV_SOUVENIR_MIRROR_OF_RUFFNOR)
			msg_print("���͍L��Ō��łȋ�Ԃ��B�O���珝�����i�͂����Ȃ����낤�B");

		if (o_ptr->sval == SV_SOUVENIR_TRUMPS_OF_DOOM)
			msg_print("�J�[�h�̒��ɂ������̐��E���������Ă���̂�������c�c");

		if (o_ptr->sval == SV_SOUVENIR_SKIDBLADNIR)
			msg_print("���̑D�����グ���̂͐_���������E�l�ɈႢ�Ȃ��I�Z��ł��鎩���܂ŗ͂��݂Ȃ���悤���I");

		if (o_ptr->sval == SV_SOUVENIR_MOON_COMPUTER)
			msg_print("�����ɂ͐l�ނ̗��j�����Â�����̉b�q���l�܂��Ă���I");

		if (o_ptr->sval == SV_SOUVENIR_NODENS_CHARIOT)
			msg_print("�����E�̑���������������ꂽ�C������...");

		if (o_ptr->sval == SV_SOUVENIR_ELDER_THINGS_CRYSTAL)
			msg_print("���̐����̒��ɂ͖��͂������Ă���I");

		if (o_ptr->sval == SV_SOUVENIR_EMPTY_BOTTLE)
			msg_print("�Ђǂ����L������...");


	}
	else
	{

	}

	//�A�C�e����������
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		//inven_item_describe(item); �u���������������Ă��Ȃ��v�Ƃ����\���͕s���R�Ȃ̂ō폜
		inven_item_optimize(item);
	}
	else
	{
		floor_item_increase(0 - item, -1);
		//floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	//�Â��A�C�e���̉��
	inven_carry(q_ptr);

	//�p�����[�^�Ƒϐ��̍Čv�Z�t���O
	p_ptr->update |= (PU_BONUS|PU_MANA);

	return TRUE;
}




/*:::�����@���g�����Ԃ��ǂ������肷��*/
//v1.1.74 �ǋL����static�O��
bool can_do_cmd_cast(void)
{

	//v1.1.74

	if (p_ptr->is_dead) return FALSE;
	if (p_ptr->confused) return FALSE;
	if (p_ptr->paralyzed) return FALSE;
	if (p_ptr->stun >= 100) return FALSE;
	if (p_ptr->alcohol >= DRANK_4) return FALSE;



	if (dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
#ifdef JP
		msg_print("�_���W���������@���z�������I");
#else
		msg_print("The dungeon absorbs all attempted magic!");
#endif
		msg_print(NULL);
		return FALSE;
	}
	else if (p_ptr->anti_magic)
	{
#ifdef JP
		msg_print("�����@�o���A�����@���ז������I");
#else
		msg_print("An anti-magic shell disrupts your magic!");
#endif
		return FALSE;
	}
	else if (p_ptr->shero)
	{
#ifdef JP
		msg_format("����m�����Ă��ē������Ȃ��I");
#else
		msg_format("You cannot think directly!");
#endif
		return FALSE;
	}
	else
		return TRUE;
}

///sys class �C�s�m�̍\��
static bool choose_kamae(void)
{
	char choice;
	int new_kamae = 0;
	int i;
	char buf[80];

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��č\�����Ȃ��I");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) �\�����Ƃ�", 2, 20);
#else
	prt(" a) No form", 2, 20);
#endif

	for (i = 0; i < MAX_KAMAE; i++)
	{
		if (p_ptr->lev >= kamae_shurui[i].min_level)
		{
			sprintf(buf," %c) %-12s  %s",I2A(i+1), kamae_shurui[i].desc, kamae_shurui[i].info);
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        �ǂ̍\�����Ƃ�܂����H", 1, 14);
#else
	prt("        Choose Form: ", 1, 14);
#endif

	while(1)
	{
		choice = inkey();

		if (choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((choice == 'a') || (choice == 'A'))
		{
			if (p_ptr->action == ACTION_KAMAE)
			{
				set_action(ACTION_NONE);
			}
			else
#ifdef JP
				msg_print("���Ƃ��ƍ\���Ă��Ȃ��B");
#else
				msg_print("You are not assuming a posture.");
#endif
			screen_load();
			return TRUE;
		}
		else if ((choice == 'b') || (choice == 'B'))
		{
			new_kamae = 0;
			break;
		}
		else if (((choice == 'c') || (choice == 'C')) && (p_ptr->lev > 29))
		{
			new_kamae = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (p_ptr->lev > 34))
		{
			new_kamae = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (p_ptr->lev > 39))
		{
			new_kamae = 3;
			break;
		}
	}
	set_action(ACTION_KAMAE);

	if (p_ptr->special_defense & (KAMAE_GENBU << new_kamae))
	{
#ifdef JP
		msg_print("�\���������B");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		p_ptr->special_defense &= ~(KAMAE_MASK);
		p_ptr->update |= (PU_BONUS);
		p_ptr->redraw |= (PR_STATE);
#ifdef JP
		msg_format("%s�̍\�����Ƃ����B",kamae_shurui[new_kamae].desc);
#else
		msg_format("You assume a posture of %s form.",kamae_shurui[new_kamae].desc);
#endif
		p_ptr->special_defense |= (KAMAE_GENBU << new_kamae);
	}
	p_ptr->redraw |= PR_STATE;
	screen_load();
	return TRUE;
}

/*:::���p�Ƃ̌^�I��*/
bool choose_samurai_kata(void)
{
	char choice;
	int new_kata = 0;
	int i;
	char buf[80];

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��č\�����Ȃ��I");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	if (p_ptr->stun)
	{
#ifdef JP
		msg_print("�ӎ����͂�����Ƃ��Ȃ��B");
#else
		msg_print("You are not clear headed");
#endif
		return FALSE;
	}

	if (p_ptr->afraid)
	{
#ifdef JP
		msg_print("�̂��k���č\�����Ȃ��I");
#else
		msg_print("You are trembling with fear!");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) �^�����", 2, 20);
#else
	prt(" a) No Form", 2, 20);
#endif

	for (i = 0; i < MAX_KATA; i++)
	{
		if (p_ptr->lev >= kata_shurui[i].min_level)
		{
#ifdef JP
			sprintf(buf," %c) %s�̌^    %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#else
			sprintf(buf," %c) Form of %-12s  %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#endif
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        �ǂ̌^�ō\���܂����H", 1, 14);
#else
	prt("        Choose Form: ", 1, 14);
#endif

	while(1)
	{
		choice = inkey();

		if (choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((choice == 'a') || (choice == 'A'))
		{
			if (p_ptr->action == ACTION_KATA)
			{
				set_action(ACTION_NONE);
			}
			else
#ifdef JP
				msg_print("���Ƃ��ƍ\���Ă��Ȃ��B");
#else
				msg_print("You are not assuming posture.");
#endif
			screen_load();
			return TRUE;
		}
		else if ((choice == 'b') || (choice == 'B'))
		{
			new_kata = 0;
			break;
		}
		else if (((choice == 'c') || (choice == 'C')) && (p_ptr->lev > 29))
		{
			new_kata = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (p_ptr->lev > 34))
		{
			new_kata = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (p_ptr->lev > 39))
		{
			new_kata = 3;
			break;
		}
	}
	set_action(ACTION_KATA);

	if (p_ptr->special_defense & (KATA_IAI << new_kata))
	{
#ifdef JP
		msg_print("�\���������B");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		p_ptr->special_defense &= ~(KATA_MASK);
		p_ptr->update |= (PU_BONUS);
		p_ptr->update |= (PU_MONSTERS);
#ifdef JP
		msg_format("%s�̌^�ō\�����B",kata_shurui[new_kata].desc);
#else
		msg_format("You assume a posture of %s form.",kata_shurui[new_kata].desc);
#endif
		p_ptr->special_defense |= (KATA_IAI << new_kata);
	}
	p_ptr->redraw |= (PR_STATE);
	p_ptr->redraw |= (PR_STATUS);
	screen_load();
	return TRUE;
}


/*
 * Returns the chance to activate a racial power/mutation
 */
static int racial_chance(power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  fail_rate = pd_ptr->fail;

	int i;
	int val;
	int sum = 0;
	int stat = p_ptr->stat_cur[pd_ptr->stat];

	///mod140126 �p�����[�^�ύX��stat_cur��18/***�����l���~�߂��̂ł����ōČv�Z
	if(p_ptr->stat_cur[pd_ptr->stat] > 18) stat = 18 + (p_ptr->stat_cur[pd_ptr->stat] - 18) * 10;

	/* No chance for success */
	if ((p_ptr->lev < min_level) || p_ptr->confused)
	{
		return (0);
	}

	if (fail_rate == 0) return 100;

	/* Calculate difficulty */
	if (p_ptr->stun)
	{
		fail_rate += p_ptr->stun;
	}
	else if (p_ptr->lev > min_level)
	{
		int lev_adj = ((p_ptr->lev - min_level) / 3);
		if (lev_adj > 10) lev_adj = 10;
		fail_rate -= lev_adj;
	}

	if (fail_rate < 5) fail_rate = 5;

	/* We only need halfs of the difficulty */
	///pend ���C�V�����̐������v�Z�@�p�����[�^��18/***�\�L����ύX�������ߑ傫���ς���K�v�����邩������Ȃ����������Ă��Ȃ�
	fail_rate = fail_rate / 2;

	for (i = 1; i <= stat; i++)
	{
		val = i - fail_rate;
		if (val > 0)
			sum += (val <= fail_rate) ? val : fail_rate;
	}

	if (fail_rate == 0)
		return (100);
	else
		return (((sum * 100) / fail_rate) / stat);
}


static int  racial_cost;

/*
 * Note: return value indicates that we have succesfully used the power
 * 1: Succeeded, 0: Cancelled, -1: Failed
 */
/*:::���C�V�����p���[�̐�������*/
static int racial_aux(power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  fail_rate = pd_ptr->fail;
	int  use_hp = 0;
	int stat = p_ptr->stat_cur[pd_ptr->stat];

	racial_cost = pd_ptr->cost;


	///mod140126 �p�����[�^�ύX��stat_cur��18/***�����l���~�߂��̂ł����ōČv�Z
	if(p_ptr->stat_cur[pd_ptr->stat] > 18) stat = 18 + (p_ptr->stat_cur[pd_ptr->stat] - 18) * 10;

	/* Not enough mana - use hp */
	if (p_ptr->csp < racial_cost) use_hp = racial_cost - p_ptr->csp;

	/* Power is not available yet */
	if (p_ptr->lev < min_level)
	{
#ifdef JP
		msg_format("���̔\�͂��g�p����ɂ̓��x�� %d �ɒB���Ă��Ȃ���΂Ȃ�܂���B", min_level);
#else
		msg_format("You need to attain level %d to use this power.", min_level);
#endif

		energy_use = 0;
		return 0;
	}

	/* Too confused */
	else if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��Ă��̔\�͎͂g���Ȃ��B");
#else
		msg_print("You are too confused to use this power.");
#endif

		energy_use = 0;
		return 0;
	}

	/* Risk death? */
	else if (p_ptr->chp < use_hp)
	{
#ifdef JP
		if (!get_check("�{���ɍ��̐��サ����Ԃł��̔\�͂��g���܂����H"))
#else
		if (!get_check("Really use the power in your weakened state? "))
#endif
		{
			energy_use = 0;
			return 0;
		}
	}

	/* Else attempt to do it! */

	if (fail_rate)
	{
		if (p_ptr->stun)
		{
			fail_rate += p_ptr->stun;
		}
		else if (p_ptr->lev > min_level)
		{
			int lev_adj = ((p_ptr->lev - min_level) / 3);
			if (lev_adj > 10) lev_adj = 10;
			fail_rate -= lev_adj;
		}

		if (fail_rate < 5) fail_rate = 5;
	}

	/* take time and pay the price */
	energy_use = 100;



	/* Success? */
	if (randint1(stat) >=
	    ((fail_rate / 2) + randint1(fail_rate / 2)))
	{
		return 1;
	}

	if (flush_failure) flush();
#ifdef JP
	msg_print("�[���ɏW���ł��Ȃ������B");
#else
	msg_print("You've failed to concentrate hard enough.");
#endif

	return -1;
}


/*:::�����r��������̂��Ă�Ƃ��I�����鏈��*/
void ratial_stop_mouth()
{
	if (music_singing_any()) stop_singing();
	if (hex_spelling_any()) stop_hex_spell_all();
}


/*:::�E�ƁA�푰���Ƃ̃��C�V�������ʔ����@���ꂼ����ނ��������ĂȂ�*/
//�����͂����ł��Ȃ�
static bool cmd_racial_power_aux(s32b command)
{
	s16b        plev = p_ptr->lev;
	int         dir = 0;

///mod140202 �E�ƃ��C�V������S�Ĕp�~
#if 0
	if (command <= -3)
	{
		switch (p_ptr->pclass)
		{
		case CLASS_WARRIOR:
		{
			int y = 0, x = 0, i;
			cave_type       *c_ptr;

			for (i = 0; i < 6; i++)
			{
				dir = randint0(8);
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				/* Hack -- attack monsters */
				if (c_ptr->m_idx)
					py_attack(y, x, 0);
				else
				{
#ifdef JP
					msg_print("�U��������������B");
#else
					msg_print("You attack the empty air.");
#endif
				}
			}
			break;
		}
		/*:::��p�ȊO�̃n�C���C�W�͂��̂܂܉��ɍs���Ė��͐H������*/
		case CLASS_HIGH_MAGE:
		if (p_ptr->realm1 == REALM_HEX)
		{
			bool retval = stop_hex_spell();
			if (retval) energy_use = 10;
			return (retval);
		}
		case CLASS_MAGE:
		/* case CLASS_HIGH_MAGE: */
		case CLASS_SORCERER:
		{
			if (!eat_magic(p_ptr->lev * 2)) return FALSE;
			break;
		}
		case CLASS_PRIEST:
		{
			if (is_good_realm(p_ptr->realm1))
			{
				if (!bless_weapon()) return FALSE;
			}
			else
			{

				(void)dispel_monsters(plev * 4);
				turn_monsters(plev * 4);
				banish_monsters(plev * 4);
			}
			break;
		}
		///sys �q�b�g���A�E�F�C
		case CLASS_ROGUE:
		{
			int x, y;

			if (!get_rep_dir(&dir, FALSE)) return FALSE;
			y = py + ddy[dir];
			x = px + ddx[dir];
			if (cave[y][x].m_idx)
			{
				py_attack(y, x, 0);
				if (randint0(p_ptr->skill_dis) < 7)
#ifdef JP
					msg_print("���܂��������Ȃ������B");
#else
					msg_print("You are failed to run away.");
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
			break;
		}
		case CLASS_RANGER:
		case CLASS_SNIPER:
		{
#ifdef JP
			msg_print("�G�𒲍�����...");
#else
			msg_print("You examine your foes...");
#endif

			probing();
			break;
		}
		case CLASS_PALADIN:
		{
			if (!get_aim_dir(&dir)) return FALSE;
			fire_beam(is_good_realm(p_ptr->realm1) ? GF_HOLY_FIRE : GF_HELL_FIRE,
			          dir, plev * 3);
			break;
		}
		case CLASS_WARRIOR_MAGE:
		{
			if (command == -3)
			{
#ifdef JP
				int gain_sp = take_hit(DAMAGE_USELIFE, p_ptr->lev, "�g�o����l�o�ւ̖��d�ȕϊ�", -1) / 5;
#else
				int gain_sp = take_hit(DAMAGE_USELIFE, p_ptr->lev, "thoughtless convertion from HP to SP", -1) / 5;
#endif
				if (gain_sp)
				{
					p_ptr->csp += gain_sp;
					if (p_ptr->csp > p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
				}
				else
#ifdef JP
					msg_print("�ϊ��Ɏ��s�����B");
#else
					msg_print("You failed to convert.");
#endif
			}
			else if (command == -4)
			{
				if (p_ptr->csp >= p_ptr->lev / 5)
				{
					p_ptr->csp -= p_ptr->lev / 5;
					hp_player(p_ptr->lev);
				}
				else
#ifdef JP
					msg_print("�ϊ��Ɏ��s�����B");
#else
					msg_print("You failed to convert.");
#endif
			}

			/* Redraw mana and hp */
			p_ptr->redraw |= (PR_HP | PR_MANA);

			break;
		}

		case CLASS_CHAOS_WARRIOR:
		{
#ifdef JP
			msg_print("�ӂ���ɂ�...");
#else
			msg_print("You glare nearby monsters...");
#endif
			slow_monsters(p_ptr->lev);
			stun_monsters(p_ptr->lev * 4);
			confuse_monsters(p_ptr->lev * 4);
			turn_monsters(p_ptr->lev * 4);
			stasis_monsters(p_ptr->lev * 4);
			break;
		}
		case CLASS_MONK:
		{
			if (!(empty_hands(TRUE) & EMPTY_HAND_RARM))
			{
#ifdef JP
				msg_print("�f�肶��Ȃ��Ƃł��܂���B");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}
			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("��n���͂ł��܂���B");
#else
				msg_print("You need to get off a pet.");
#endif
				return FALSE;
			}

			if (command == -3)
			{
				if (!choose_kamae()) return FALSE;
				p_ptr->update |= (PU_BONUS);
			}
			else if (command == -4)
			{
				int x, y;

				if (!get_rep_dir(&dir, FALSE)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if (cave[y][x].m_idx)
				{
#ifdef JP
					if (one_in_(2)) msg_print("���[���������������������������������������������I�I�I");
					else msg_print("�I���I���I���I���I���I���I���I���I���I���I���I���I�I�I");
#else
					if (one_in_(2)) msg_print("Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!");
					else msg_print("Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!");
#endif
					/*:::��x�U�����A����Ń����X�^�[������łȂ���΂�����x�U���@�ǂ���ɂ��Ă�energy��{����*/
					py_attack(y, x, 0);
					if (cave[y][x].m_idx)
					{
						handle_stuff();
						py_attack(y, x, 0);
					}
					p_ptr->energy_need += ENERGY_NEED();
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
		}
		/*:::�����~��*/
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
		{
			if (total_friends)
			{
#ifdef JP
				///msg131217
				msg_print("���͔z���̂��ƂɏW�����Ă��Ȃ��ƁB");
				//msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
#ifdef JP
			msg_print("���������n�b�L�������B");
#else
			msg_print("You feel your head clear a little.");
#endif

			p_ptr->csp += (3 + p_ptr->lev/20);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}

			/* Redraw mana */
			p_ptr->redraw |= (PR_MANA);
			break;
		}
		case CLASS_TOURIST:
		{
			if (command == -3)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				project_length = 1;
				fire_beam(GF_PHOTO, dir, 1);
			}
			else if (command == -4)
			{
				if (!identify_fully(FALSE)) return FALSE;
			}
			break;
		}
		case CLASS_IMITATOR:
		{
			handle_stuff();
			if (!do_cmd_mane(TRUE)) return FALSE;
			break;
		}
		case CLASS_BEASTMASTER:
		{
			if (command == -3)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				(void)fire_ball_hide(GF_CONTROL_LIVING, dir, p_ptr->lev, 0);
			}
			else if (command == -4)
			{
				project_hack(GF_CONTROL_LIVING, p_ptr->lev);
			}
			break;
		}
		case CLASS_ARCHER:
		{
		msg_print("�A�[�`���[�̖�ʐ����͖��������B");
		return FALSE;
		//if (!do_cmd_archer()) return FALSE;
			break;
		}
		case CLASS_MAGIC_EATER:
		{
			if (command == -3) {
				if (!gain_magic()) return FALSE;
			} else if (command == -4) {
				if (!can_do_cmd_cast()) return FALSE;
				if (!do_cmd_magic_eater(FALSE, TRUE)) return FALSE;
			}
			break;
		}
		case CLASS_BARD:
		{
			/* Singing is already stopped */
			if (!p_ptr->magic_num1[0] && !p_ptr->magic_num1[1]) return FALSE;

			stop_singing();
			energy_use = 10;
			break;
		}
		case CLASS_RED_MAGE:
		{
			if (!can_do_cmd_cast()) return FALSE;
			handle_stuff();
			do_cmd_cast();
			handle_stuff();
			if (!p_ptr->paralyzed && can_do_cmd_cast())
				do_cmd_cast();
			break;
		}
		case CLASS_SAMURAI:
		{
			if (command == -3)
			{
				int max_csp = MAX(p_ptr->msp*4, p_ptr->lev*5+5);

				if (total_friends)
				{
#ifdef JP
					///msg131217
					//msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
					msg_print("���͔z���̂��ƂɏW�����Ă��Ȃ��ƁB");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (p_ptr->special_defense & KATA_MASK)
				{
#ifdef JP
					msg_print("���͍\���ɏW�����Ă���B");
#else
					msg_print("You need concentration on your form.");
#endif
					return FALSE;
				}
#ifdef JP
				msg_print("���_���W�����ċC�����𗭂߂��B");
#else
				msg_print("You concentrate to charge your power.");
#endif

				p_ptr->csp += p_ptr->msp / 2;
				if (p_ptr->csp >= max_csp)
				{
					p_ptr->csp = max_csp;
					p_ptr->csp_frac = 0;
				}

				/* Redraw mana */
				p_ptr->redraw |= (PR_MANA);
			}
			else if (command == -4)
			{
				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
				{
#ifdef JP
					msg_print("����������Ȃ��Ƃ����܂���B");
#else
					msg_print("You need to wield a weapon.");
#endif
					return FALSE;
				}
				if (!choose_kata()) return FALSE;
				p_ptr->update |= (PU_BONUS);
			}
			break;
		}
		case CLASS_BLUE_MAGE:
		{
			if (p_ptr->action == ACTION_LEARN)
			{
				set_action(ACTION_NONE);
			}
			else
			{
				set_action(ACTION_LEARN);
			}
			energy_use = 0;
			break;
		}
		case CLASS_CAVALRY:
		{
			char m_name[80];
			monster_type *m_ptr;
			monster_race *r_ptr;
			int rlev;

			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("���͏�n�����B");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if (!do_riding(TRUE)) return TRUE;
			m_ptr = &m_list[p_ptr->riding];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%s�ɏ�����B",m_name);
#else
			msg_format("You ride on %s.",m_name);
#endif
			///sys �r�n��炵�@��n�Z�\
			if (is_pet(m_ptr)) break;
			rlev = r_ptr->level;
			if (r_ptr->flags1 & RF1_UNIQUE) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			///mod131226 skill �Z�\�ƕ���Z�\�̓���
		//	if ((randint1(p_ptr->skill_exp[GINOU_RIDING] / 120 + p_ptr->lev * 2 / 3) > rlev)
			if ((randint1(ref_skill_exp(SKILL_RIDING) / 120 + p_ptr->lev * 2 / 3) > rlev)
				&& one_in_(2) && !p_ptr->inside_arena && !p_ptr->inside_battle
			    && !(r_ptr->flags7 & (RF7_GUARDIAN)) && !(r_ptr->flags1 & (RF1_QUESTOR))
			    && (rlev < p_ptr->lev * 3 / 2 + randint0(p_ptr->lev / 5)))
			{
#ifdef JP
				msg_format("%s����Ȃ������B",m_name);
#else
				msg_format("You tame %s.",m_name);
#endif
				set_pet(m_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%s�ɐU�藎�Ƃ��ꂽ�I",m_name);
#else
				msg_format("You have thrown off by %s.",m_name);
#endif
				rakuba(1,TRUE);

				/* Paranoia */
				/* ���n�����Ɏ��s���Ă��Ƃɂ�����n���� */
				p_ptr->riding = 0;
			}
			break;
		}
		case CLASS_BERSERKER:
		{
			if (!word_of_recall()) return FALSE;
			break;
		}
		case CLASS_SMITH:
		{
			if (p_ptr->lev > 29)
			{
				if (!identify_fully(TRUE)) return FALSE;
			}
			else
			{
				if (!ident_spell(TRUE)) return FALSE;
			}
			break;
		}
		case CLASS_MIRROR_MASTER:
		{
			if (command == -3)
			{
				/* Explode all mirrors */
				remove_all_mirrors(TRUE);
			}
			else if (command == -4)
			{
				if (total_friends)
				{
#ifdef JP
					msg_print("���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (is_mirror_grid(&cave[py][px]))
				{
#ifdef JP
					msg_print("���������n�b�L�������B");
#else
					msg_print("You feel your head clear a little.");
#endif

					p_ptr->csp += (5 + p_ptr->lev * p_ptr->lev / 100);
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}

					/* Redraw mana */
					p_ptr->redraw |= (PR_MANA);
				}
				else
				{
#ifdef JP
					msg_print("���̏�łȂ��ƏW���ł��Ȃ��I");
#else
					msg_print("Here are not any mirrors!");
#endif
				}
			}
			break;
		}
		case CLASS_NINJA:
		{
			if (p_ptr->action == ACTION_HAYAGAKE)
			{
				set_action(ACTION_NONE);
			}
			else
			{
				cave_type *c_ptr = &cave[py][px];
				feature_type *f_ptr = &f_info[c_ptr->feat];

				if (!have_flag(f_ptr->flags, FF_PROJECT) ||
				    (!p_ptr->levitation && have_flag(f_ptr->flags, FF_DEEP)))
				{
#ifdef JP
					msg_print("�����ł͑f���������Ȃ��B");
#else
					msg_print("You cannot run in here.");
#endif
				}
				else
				{
					set_action(ACTION_HAYAGAKE);
				}
			}


			energy_use = 0;
			break;
		}

		}
	}
	else 
#endif


	//����
	if (command == -99)
	{
		if (!energy_drain()) return FALSE;

	}
	//����@�܋����g�p���̃��C�V�����n���܎E�@
	else if(p_ptr->clawextend && command == -100)
	{

		cave_type       *c_ptr;
		monster_type    *m_ptr;
		monster_race	*r_ptr;
		int x,y;

		for (dir = 0; dir < 8; dir++)
		{
			y = py + ddy_ddd[dir];
			x = px + ddx_ddd[dir];
			c_ptr = &cave[y][x];

			m_ptr = &m_list[c_ptr->m_idx];
			/*:::�s���̃����X�^�[���U�����邪�A�ǂ̒��ɂ���s���̃����X�^�[�͍U�����Ȃ�*/
			if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
			{

				py_attack(y, x, HISSATSU_EXCLAW);

			}
		}

	}
	//command-101�`-104��EXTRA���[�h�̃A�C�e���J�[�h�֌W�R�}���h�ɂ���
	else if(command == -101) //�J�[�h���Z�b�g
	{
		if(!set_itemcard()) return FALSE;
	}
	else if(command == -102) //�J�[�h���g�p
	{
		if(!activate_itemcard()) return FALSE;
	}
	else if(command == -103) //�J�[�h���O��
	{
		if(!remove_itemcard()) return FALSE;
	}
	else if(command == -104) //�J�[�h��ǂ�
	{
		read_itemcard();
		return FALSE;
	}
	else if(command == -105) //���i���C�̋A��
	{
		if (!word_of_recall()) return FALSE;
	}

	else if (p_ptr->mimic_form)
	{
		switch (p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		{
			int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
			if (!get_aim_dir(&dir)) return FALSE;
			ratial_stop_mouth();
#ifdef JP
			msg_format("���Ȃ���%s�̃u���X��f�����B",((type == GF_NETHER) ? "�n��" : "�Ή�"));
#else
			msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

			fire_ball(type, dir, plev * 3, -(plev / 15) - 1);
			break;
		}
		case MIMIC_VAMPIRE:
			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return FALSE;
			}
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir(&dir, FALSE)) return FALSE;   /* was get_aim_dir */
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				ratial_stop_mouth();

				if (!c_ptr->m_idx)
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

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   /* Dmg */
				if (drain_life(dir, dummy))
				{
					if (p_ptr->food < PY_FOOD_FULL)
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
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
				}
				else
#ifdef JP
					msg_print("�����B�Ђǂ������B");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;
		case MIMIC_SHINMYOU:
		case MIMIC_GIGANTIC:
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;

				set_mimic(0,0,TRUE);
			}
			break;

		case MIMIC_BEAST:
			if(command == -1)
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;

				set_mimic(0,0,TRUE);
			}
			else if(command == -2)
			{
				msg_print("���낵�����т��_���W�����ɋ������I");
				(void)set_afraid(0);
				(void)set_shero(10 + randint1(plev), FALSE);
				confuse_monsters(p_ptr->lev * 2);
				turn_monsters(p_ptr->lev * 4);
				(void)aggravate_monsters(0,FALSE);
			}
			/*:::�ːi�@ruch_attack()������*/
			else
			{
				int dir;
				int tx, ty;
				u16b path_g[32];
				int path_n, i;
				bool moved = FALSE;

				project_length = 5;
				if (!get_aim_dir(&dir)) return FALSE;
				tx = px + project_length * ddx[dir];
				ty = py + project_length * ddy[dir];
				if ((dir == 5) && target_okay())
				{
					tx = target_col;
					ty = target_row;
				}

				if (in_bounds(ty, tx)) msg_format("���Ȃ��͖җ�ȓːi���������I");

				path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_THRU | PROJECT_KILL);
				project_length = 0;
				if (!path_n) return FALSE;
				ty = py;
				tx = px;

				/* Project along the path */
				for (i = 0; i < path_n; i++)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;
					char m_name[80];
					bool stop = FALSE;

					int ny = GRID_Y(path_g[i]);
					int nx = GRID_X(path_g[i]);
	
					if (cave[ny][nx].m_idx)
					{
						m_ptr = &m_list[cave[ny][nx].m_idx];
						r_ptr = &r_info[m_ptr->r_idx];
						monster_desc(m_name, m_ptr, 0);

						if(d_info[dungeon_type].flags1 & DF1_NO_MELEE)
						{
							msg_format("�Ȃ���%s�̒��O�ő����~�܂����B",m_name);
							break;
						}						
						else if(r_ptr->flags2 & RF2_GIGANTIC)
						{			
							msg_format("%s�ɓːi���~�߂�ꂽ�I",m_name);
							(void)project(0, 0, ny, nx, p_ptr->lev * 3, GF_MISSILE, PROJECT_HIDE|PROJECT_KILL, -1);
							break;
						}
						else 
						{
							msg_format("%s��e����΂����I",m_name);
							(void)project(0, 0, ny, nx, p_ptr->lev * 3, GF_MISSILE, PROJECT_HIDE|PROJECT_KILL, -1);
							if(cave[ny][nx].m_idx) teleport_away(cave[ny][nx].m_idx, 5,TELEPORT_PASSIVE);
						}

					}
					if (cave_empty_bold(ny, nx) && player_can_enter(cave[ny][nx].feat, 0))
					{
						ty = ny;
						tx = nx;
					}
					else 
					{
						msg_print("�����ɂ͓���Ȃ��B");
						break;
					}

				}

				if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);

			}
			break;
		case MIMIC_SLIME:
			if(command == -1)
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;

				set_mimic(0,0,TRUE);
			}
			else if(command == -2)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͎_�̃u���X��f�����B");
				//v1.1.31 MIN��MAX���ԈႦ�Ă����̂ŏC��
				fire_ball(GF_ACID, dir, MAX(p_ptr->chp/3,1), -(plev / 15) - 1);
			}
			break;


		case MIMIC_MARISA:
			if(command == -1)
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;

				set_mimic(0,0,TRUE);
			}
			else if(command == -2)
			{
				int dir;
				int damage = p_ptr->lev * 3;
				if (!get_aim_dir(&dir)) return FALSE;
				fire_beam(GF_NUKE, dir, damage);
			}
			else if(command == -3)
			{
				int len=p_ptr->lev / 7;
				int damage =  p_ptr->lev * 2;
				if (!rush_attack2(len,GF_MISSILE,damage,0)) return FALSE;
			}
			else if(command == -4)
			{
				int dir;
				int damage = p_ptr->lev * 7;
				if (!get_aim_dir(&dir)) return FALSE;
				fire_spark(GF_DISINTEGRATE,dir,damage,1);
			}

			break;
		case MIMIC_MIST:
			if(command == -1)
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;

				set_mimic(0,0,TRUE);
			}
			else if(command == -2)
			{
				msg_print("�ǂ��ֈړ����܂����H");
				if (!dimension_door(D_DOOR_MIST)) return FALSE;
			}
			break;


		case MIMIC_DRAGON:
			if(command == -1)
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;
				set_mimic(0,0,TRUE);
			}
			else if(command == -2)
			{
				int dam = MIN(1600,p_ptr->chp/3);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͉Ή��̃u���X��f�����B");
				fire_ball(GF_FIRE, dir, dam, -4);
			}
			else if(command == -3)
			{
				int dam = MIN(1200,p_ptr->chp/4);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��̓v���Y�}�̃u���X��f�����B");
				fire_ball(GF_PLASMA, dir, dam, -4);
			}
			else if(command == -4)
			{
				int dam = MIN(1000,p_ptr->chp/4);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͊j�M�̃u���X��f�����B");
				fire_ball(GF_NUKE, dir, dam, -4);
			}
			else if(command == -5)
			{
				int dam = MIN(450,p_ptr->chp/6);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͏Ռ��g�̃u���X��f�����B");
				fire_ball(GF_SOUND, dir, dam, -4);
			}
			else if(command == -6)
			{
				int dam = MIN(600,p_ptr->chp/6);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��̓J�I�X�̃u���X��f�����B");
				fire_ball(GF_CHAOS, dir, dam, -4);
			}
			else if(command == -7)
			{
				int dam = MIN(600,p_ptr->chp/7);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͈��ʍ����̃u���X��f�����B");
				fire_ball(GF_NEXUS, dir, dam, -4);
			}
			else if(command == -8)
			{
				int dam = MIN(555,p_ptr->chp/6);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͍����k�^�L�I�����q�̃u���X��f�����I");
				fire_spark(GF_DISINTEGRATE,dir,dam,2);
			}

			else if(command == -9)
			{
				int dam = MIN(1600,p_ptr->chp/3);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͓d���̃u���X��f�����B");
				fire_ball(GF_ELEC, dir, dam, -4);
			}
			else if(command == -10)
			{
				int dam = MIN(1600,p_ptr->chp/3);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͗�C�̃u���X��f�����B");
				fire_ball(GF_COLD, dir, dam, -4);
			}
			else if(command == -11)
			{
				int dam = MIN(1600,p_ptr->chp/3);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͎_�̃u���X��f�����B");
				fire_ball(GF_ACID, dir, dam, -4);
			}

			else if(command == -12)
			{
				msg_print("�ǂ��ɔ�т܂����H");
				if (!dimension_door(D_DOOR_DRAGON)) return FALSE;
			}
			break;
		case MIMIC_CAT:
			if(command == -1)
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;

				set_mimic(0,0,TRUE);
			}
			break;
		case MIMIC_METAMORPHOSE_NORMAL:
		case MIMIC_METAMORPHOSE_MONST:
		case MIMIC_METAMORPHOSE_GIGANTIC:
			if(command == -1)
			{
				screen_save();
				screen_roff(MON_EXTRA_FIELD, 0x01);//���ׂĂ̏��𓾂�
				inkey();
				screen_load();
				return FALSE;

			}
			else if(command == -2)
			{
				if (!get_check_strict("���̎p�ɖ߂�܂����H", CHECK_DEFAULT_Y)) return FALSE;

				p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
				p_ptr->special_flags &= ~(SPF_IGNORE_METAMOR_TIME);

				set_mimic(0,0,TRUE);
			}
			else if(command == -3)
			{
				if(!cast_monspell_new()) return FALSE;
			}
			break;
		case MIMIC_NUE:
			if(command == -1)
			{
				msg_print("���Ȃ��͏u���ɏ������B");
				teleport_player(10, 0L);
			}
			else if(command == -2)
			{
				msg_print("�s�g�Ȗ������ӂ�ɋ������E�E");
				confuse_monsters(plev * 3);
				turn_monsters(50 + plev * 3);
			}
			else if(command == -3)
			{
				int dam = MIN(300,p_ptr->chp/5);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ���ᏋC��f�����B");
				fire_ball(GF_POIS, dir, dam, -3);
			}
			else if(command == -4)
			{
				if (!get_aim_dir(&dir)) return FALSE;		
				if(!fire_ball_jump(GF_ELEC,dir,plev*5,3,"�����������I")) return FALSE;
			}
			else if(command == -5)
			{
				int dam = MIN(400,p_ptr->chp/4);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͈Í���f�����B");
				fire_ball(GF_DARK, dir, dam, -3);
			}
			else if(command == -6)
			{
				int dam = MIN(600,p_ptr->chp/3);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
				msg_format("���Ȃ��͍��ׂ�f�����B");
				fire_ball(GF_CHAOS, dir, dam, -3);
			}
			break;






		}



	}

	else 
	{
/*:::�푰���ƃ��C�V�����@��ϐg��*/
	switch (p_ptr->prace)
	{
		case RACE_SENNIN:
		case RACE_TENNIN:

			if (command == -1)
			{

				//�����~��
				if (total_friends)
				{
#ifdef JP
					msg_print("���͔z���̂��ƂɏW�����Ă��Ȃ��ƁB");
#endif
					return FALSE;
				}

				if(player_gain_mana(3 + p_ptr->lev / 20))
					msg_print("���������n�b�L�������B");


			}
			else //�V�l�̓����Z�a�@������l�ɓ�ڂ̃��C�V�����ǉ������璍��
			{

				int power = plev * 3;

				charm_animals(power);

			}
			break;
		case RACE_KAPPA:
			//���S�C
			if(command == -1)
			{
				int dice = 3 + (plev) / 5;
				int sides = 4;
				int range = plev / 6 + 2;

				project_length = range;
				if (!get_aim_dir(&dir)) return FALSE;
				fire_bolt(GF_WATER, dir, damroll(dice, sides));
			}
			//�ʔw
			else if(command == -2)
			{
				project_length = p_ptr->lev / 12;
				if (!get_aim_dir(&dir)) return FALSE;
				project_hook(GF_ATTACK, dir, HISSATSU_KAPPA, PROJECT_STOP | PROJECT_KILL);
			}
			//�������荞��
			else if(command == -3)
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;
				feature_type *f_ptr = &f_info[cave[py][px].feat];
				/* Only works on adjacent monsters */
				if (!have_flag(f_ptr->flags, FF_WATER))
				{
					msg_print("�����ɂ͐����Ȃ��B");
					return FALSE;
				}
				if (!get_rep_dir(&dir, FALSE)) return FALSE;   /* was get_aim_dir */
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];
				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("�����ɂ͉������Ȃ��B");
#endif
					break;
				}
				else
				{
					monster_type    *m_ptr = &m_list[c_ptr->m_idx];
					monster_race    *r_ptr = &r_info[m_ptr->r_idx];
					char            m_name[80];
					int fail = r_ptr->level;
					if(r_ptr->flags1 & RF1_UNIQUE) fail*=2;
					if(r_ptr->flags2 & RF2_POWERFUL) fail*=2;
					if(r_ptr->flags7 & RF7_CAN_SWIM) fail*=2;
					if(r_ptr->flags7 & RF7_CAN_FLY) fail*=3;
					if(r_ptr->flags2 & RF2_GIGANTIC) fail*=5;
					if(r_ptr->flagsr & RFR_RES_WATE) fail*=5;
					if(r_ptr->flags7 & RF7_AQUATIC) fail*=10;
					fail -= p_ptr->lev + adj_general[p_ptr->stat_ind[A_STR]] * 2;
					if(have_flag(f_ptr->flags, FF_DEEP)) fail /= 2;
					monster_desc(m_name, m_ptr, 0);
					if(fail<0) fail = 0;
					if(randint0(100) < fail)
					{
						msg_format("%s���������荞�����Ƃ��������s�����B",m_name);
						break;
					}
					else
					{
						int dam;
						if(!(r_ptr->flags1 & RF1_UNIQUE) && randint1(randint1(r_ptr->level/7)+2) == 1)
						{
							msg_format("%s�͐���ւƒ���ł������E�E",m_name);
							dam =  m_ptr->hp + 1;
						}
						else 
						{
							msg_format("%s�𐅒��ֈ������荞�񂾁I",m_name);
							dam = plev * 3 + randint1(adj_general[p_ptr->stat_ind[A_STR]] * 4);
						}
						project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_WATER,PROJECT_KILL,-1);
						touch_zap_player(m_ptr);
					}
				}
			}
			break;

			//���͒D�� monster_type.mflag��SPECIAL2�t���O���g��
		case RACE_MAGICIAN:
		{
			int y, x, dummy = 0;
			cave_type *c_ptr;
			/* Only works on adjacent monsters */
			if (!get_rep_dir(&dir, FALSE)) return FALSE;   /* was get_aim_dir */
			y = py + ddy[dir];
			x = px + ddx[dir];
			c_ptr = &cave[y][x];
			if (!c_ptr->m_idx)
			{
					msg_print("�����ɂ͉������Ȃ��B");
					break;
			}
			else
			{
				//v1.1.27 ���C�V�����̐��\�ύX
				monster_type    *m_ptr = &m_list[c_ptr->m_idx];
				monster_race    *r_ptr = &r_info[m_ptr->r_idx];
				char            m_name[80];
				int fail = r_ptr->level;
				if(r_ptr->flags2 & RF2_SMART) fail += r_ptr->level / 2;
				if(r_ptr->flags2 & RF2_POWERFUL) fail += r_ptr->level / 2;
				if(MON_STUNNED(m_ptr)) fail /= 2;
				if(MON_CONFUSED(m_ptr)) fail /= 2;
				if(MON_CSLEEP(m_ptr)) fail = 0;

				monster_desc(m_name, m_ptr, 0);

				if(m_ptr->mflag & MFLAG_SPECIAL2)
				{
					msg_format("%s����͂������͂�D���Ȃ��悤���B",m_name);
				}
				else if(randint1(fail) > plev)
				{
					msg_format("%s���疂�͂�D���̂Ɏ��s�����B",m_name);
				}
				else
				{
					msg_format("%s���疂�͂��z��������I",m_name);
					p_ptr->csp += r_ptr->level;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					(void)set_monster_csleep(c_ptr->m_idx, 0);

					if(randint0(100) > r_ptr->level ) m_ptr->mflag |= MFLAG_SPECIAL2;

					p_ptr->redraw |= (PR_MANA);
				}

#if 0 
				monster_type    *m_ptr = &m_list[c_ptr->m_idx];
				monster_race    *r_ptr = &r_info[m_ptr->r_idx];
				char            m_name[80];
				int fail = r_ptr->level + r_ptr->ac / 2;
				if(r_ptr->flags2 & RF2_SMART) fail*=2;
				if(r_ptr->flags1 & RF1_UNIQUE) fail*=2;
				if(r_ptr->flags2 & RF2_STUPID) fail/=2;
				if(r_ptr->flags7 & RF7_CAN_FLY) fail=fail * 3 / 2;
				if(MON_STUNNED(m_ptr)) fail /= 3;
				if(MON_CONFUSED(m_ptr)) fail /= 3;
				if(MON_CSLEEP(m_ptr)) fail = 0;

				fail -= p_ptr->lev + adj_general[p_ptr->stat_ind[A_DEX]] + ref_skill_exp(SKILL_MARTIALARTS) / 200;
				monster_desc(m_name, m_ptr, 0);
				if(fail<0) fail = 0;

				if(m_ptr->mflag & MFLAG_SPECIAL2)
				{
					msg_format("%s����͂������͂��z���Ȃ��悤���B",m_name);
				}
				else if(randint0(100) < fail)
				{
					msg_format("%s�ɐG�낤�Ƃ��������킳�ꂽ�B",m_name);
					break;
				}
				else
				{
					msg_format("%s���疂�͂��z��������I",m_name);
					p_ptr->csp += r_ptr->level;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					(void)set_monster_csleep(c_ptr->m_idx, 0);

					if(randint0(100) > r_ptr->level ) m_ptr->mflag |= MFLAG_SPECIAL2;

					touch_zap_player(m_ptr);
					p_ptr->redraw |= (PR_MANA);
				}
#endif
			}
			break;
		}
		case RACE_KARASU_TENGU:
		{
			//�ʐ^�B�e
			if(command == -1)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				project_length = 1;
				fire_beam(GF_PHOTO, dir, 1);
				break;
			}
			//�}����
			if(command == -2)
			{
				int len=p_ptr->lev / 5;
				if (!rush_attack2(len,0,0,0)) return FALSE;
				break;
			}
		}
		case RACE_HAKUROU_TENGU:
		{
			//�����X�^�[���m
			if(command == -1)
			{
				msg_print("���Ȃ��͂����Ǝ��𐟂܂���...");
				(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
				(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
			}
			//�H������
			else if(command == -2)
			{
				object_type *q_ptr;
				object_type forge;
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
#ifdef JP
				msg_print("�H�Ƃ𒲒B�����B");
#endif
				(void)drop_near(q_ptr, -1, py, px);
			}
			//����
			else
			{
				probing();
			}
			break;
		}
		case RACE_ONI:
		{
			//����m��
			if(command == -1)
			{
#ifdef JP
				msg_print("���Ȃ��͌���ɐg��C�����E�E");
#endif
				(void)set_afraid(0);
				(void)set_shero(10 + randint1(plev), FALSE);
				(void)hp_player(30);
			}
			//���K
			else
			{
				msg_print("���Ȃ��͑剹����������I");

				(void)fire_ball(GF_SOUND, 0, p_ptr->lev * 4, 8);
				(void)aggravate_monsters(0,FALSE);
			}
			break;
		}
		case RACE_WARBEAST:
		{
			//�����X�^�[���m
			if(command == -1)
			{
				msg_print("���Ȃ��͂����Ǝ��𐟂܂���...");
				(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
				(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
			}
			//����m��
			else if(command == -2)
			{
#ifdef JP
				msg_print("���Ȃ��͏b�̂悤�ɖi�����I");
#endif
				(void)set_afraid(0);
				(void)set_shero(10 + randint1(plev), FALSE);
				(void)hp_player(30);
			}

			break;
		}
		case RACE_GYOKUTO:
		{
			//�\�𕷂�
			if(command == -1)
			{
				display_rumor_new(0);

			}
			//�����X�^�[���m
			else if(command == -2)
			{
				msg_print("���Ȃ��͂����Ǝ��𐟂܂���...");
				(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
				(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
			}
			//�V���[�g�E�e���|�[�g
			else
			{
#ifdef JP
				msg_print("���Ȃ��͏u���ɏ������B");
#endif
				teleport_player(10, 0L);
			}
			break;
		}
		case RACE_YAMAWARO:

			//�ʔw
			project_length = p_ptr->lev / 12;
			if (!get_aim_dir(&dir)) return FALSE;
			project_hook(GF_ATTACK, dir, HISSATSU_KAPPA, PROJECT_STOP | PROJECT_KILL);
			break;
		case RACE_BAKEDANUKI:
		{
			//�����X�^�[���m
			if(command == -1)
			{
				msg_print("���Ȃ��͂����Ǝ��𐟂܂���...");
				(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
				(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
			}
			//�e���|�[�g
			else if(command == -2)
			{
				msg_print("���Ȃ��͉��Ƌ��ɏ������B");
				teleport_player(p_ptr->lev + 25, 0L);
			}
			else
			{
				if (!(p_ptr->special_defense & NINJA_KAWARIMI))
				{
					msg_print("���Ȃ��͂�������Ǝd���݂��ς܂����E�E");

					p_ptr->special_defense |= NINJA_KAWARIMI;
					p_ptr->redraw |= (PR_STATUS);
				}
			}
			break;

		}

		case RACE_YOUKO:
		{
			//�����X�^�[���m
			if(command == -1)
			{
				msg_print("���Ȃ��͂����Ǝ��𐟂܂���...");
				(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
				(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
			}
			//���_�U��
			else if(command == -2)
			{
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���Ȃ��͐��_�g��������B");
#else
			msg_print("You concentrate and your eyes glow red...");
#endif

			fire_bolt(GF_PSI, dir, plev * 2);
			}

			//���f
			else
			{
				msg_print("���Ȃ��̖ڂ��Ԃ��P����...");
				stun_monsters(plev * 4);
				confuse_monsters(plev * 4);
				turn_monsters(plev * 4);
			}
			break;
		case RACE_HOFGOBLIN:

			{
				object_type *q_ptr;
				object_type forge;
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
#ifdef JP
				msg_print("�H�Ƃ𒲒B�����B");
#endif
				(void)drop_near(q_ptr, -1, py, px);
			}
			break;

		case RACE_NYUDOU:
		{
			//�V���[�g�e���|
			if(command == -1)
			{
#ifdef JP
				msg_print("���Ȃ��͏u���ɏ������B");
#endif
				teleport_player(10, 0L);
			}
			//�e���|�o�b�N
			else
			{
				int idx;
				if(!teleport_back(&idx, TELEPORT_FORCE_NEXT)) return FALSE;
			}


			break;
		}
		case RACE_STRAYGOD:
			if(!deity_racial_power_aux(command)) return FALSE;
			break;

		case RACE_FAIRY:
			if(!fairy_racial_power_aux(command)) return FALSE;
			break;

		case RACE_NINJA:
			//�X���P��
			if(command == -1)
			{
				int dice = 2 + (plev) / 6;
				int sides = 5;
				int i;
				int count = (plev) / 5;
				if (!get_aim_dir(&dir)) return FALSE;
				if(count==1) msg_format("�X���P����������I");
				else if(count<4) msg_format("%d�{�̃X���P����������I",count);
				else if(count<6) msg_format("%d�{�̃X���P���A�������I���U�}�G�I",count);
				else if(count<9) msg_format("%d�{�̃X���P���A�������I�^�c�W���I",count);
				else msg_format("�S�E�����K�I%d�{�̃X���P���A�������I",count);
				for (i = 0; i < count; i++)
				{
					fire_bolt(GF_WINDCUTTER, dir, damroll(dice, sides));
				}
			}
			//�`���h�[
			else
			{
				bool heal = FALSE;
				msg_print("�u�X�E�[�b�c�c�n�A�[�b�c�c�v");
				if(p_ptr->chp < p_ptr->mhp || p_ptr->csp < p_ptr->msp)
					msg_print("�g�̂Ɋ��͂��߂��Ă���̂�������B");
				hp_player(randint1(10 + p_ptr->lev/5));
				p_ptr->csp += randint1(5+p_ptr->lev/10);
				if(p_ptr->csp > p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
				}
				p_ptr->redraw |= (PR_MANA);
				set_poisoned(p_ptr->poisoned - 10);
				set_stun(p_ptr->stun - 10);
				set_cut(p_ptr->cut - 10);
				set_image(p_ptr->image - 10);

			}
			break;

		case RACE_GOLEM:
			if (command == -1)
			{
				if(!self_repair()) return FALSE;

			}
			else
			{
				if (!eat_rock()) return FALSE;
			}
			break;

		case RACE_LICH:
		
			if(command == -1)
			{
				if (!energy_drain()) return FALSE;
			}
			else
			{
				msg_format("�׈��ȃI�[�����ӂ�ɏ[�������B");
				dispel_living(plev * 2);
				charm_undead(plev * 2);
			}
			break;

		}
		case RACE_LUNARIAN:

			if(command == -1)//�A��
			{
				if (!word_of_recall()) return FALSE;
			}
			else//�z������
			{
				bool flag = FALSE;
				u32b mode = PM_FORCE_PET;
				int r_idx;

				//summon_specific���g���ƍ��ׂ̗̈����̋ʓe�Ɖq�m���o�Ȃ��̂�r_idx�w��
				if(randint1(p_ptr->lev) > 44)
				{
					r_idx = MON_MOON_SENTINEL;
					mode |= PM_ALLOW_GROUP;
				}
				else if(randint1(p_ptr->lev) > 39)
				{
					r_idx = MON_GYOKUTO;
					mode |= PM_ALLOW_GROUP;
				}
				else if(randint1(p_ptr->lev) > 34)
					r_idx = MON_MOON_SENTINEL;
				else if(randint1(p_ptr->lev) > 24)
					r_idx = MON_GYOKUTO;
				else 
				{
					if(p_ptr->lev > 34) mode |= PM_ALLOW_GROUP;
					r_idx = MON_EAGLE_RABBIT;
				}

				if (summon_named_creature(0,py,px,r_idx,mode)) flag = TRUE;

				if(flag) msg_print("���Ȃ��͔z�����Ăяo�����B");
				else msg_print("�N�����Ȃ������B");
			}
		break;
		//v1.1.66
		case RACE_HANIWA:
		case RACE_ANDROID:
			{
			if (!self_repair()) return FALSE;

			}
		break;

		//v1.1.70 ������@�������͂ɂ���ă��C�V�������Ⴄ
		case RACE_ANIMAL_GHOST:
		{
			switch (CHECK_ANIMAL_GHOST_STRIFE)
			{
			case ANIMAL_GHOST_STRIFE_KEIGA:
				if (command == -1)
				{
					msg_print("���Ȃ��͌��ɋQ�������т�������I");
					(void)set_afraid(0);
					(void)set_shero(10 + randint1(plev), FALSE);
					(void)hp_player(30);
				}
				else
				{
					set_fast(15 + randint1(15), FALSE);

				}
				break;

			case ANIMAL_GHOST_STRIFE_KIKETSU:
				if (command == -1)
				{
					msg_print("���Ȃ��͏u���ɏ������B");
					teleport_player(10, 0L);
				}
				else
				{
					int len = p_ptr->lev / 10;
					if (!rush_attack(NULL, len, 0)) return FALSE;
					break;
				}

				break;

			case ANIMAL_GHOST_STRIFE_GOUYOKU:
				if (command == -1)
				{
					msg_print("���Ȃ��͓G�̋C�z��T����...");
					(void)detect_monsters_normal(p_ptr->lev / 2 + 10);
					(void)detect_monsters_invis(p_ptr->lev / 2 + 10);
				}
				else
				{
					msg_print("���͂̒n�`�𒲍������B");
					map_area(p_ptr->lev / 2 + 10);
				}
				break;
			}
		}
		break;
		//v1.1.79
		case RACE_ZASHIKIWARASHI:
		{
			if (command == -1)
			{
				if (!zashiki_moving()) return FALSE;

			}
			else if (command == -2)
			{
				object_type *o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

				//�����̚�:�Ή��̃u���X
				if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU)
				{

					int dam = MIN(1600, p_ptr->chp / 3);
					if (dam < 1) dam = 1;
					if (!get_aim_dir(&dir)) return FALSE;
					ratial_stop_mouth();
					msg_format("���Ȃ��͉Ή��̃u���X��f�����B");
					fire_ball(GF_FIRE, dir, dam, -3);

				}
				//�Ηю��̒���:���C�̃u���X
				else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_MORINJI)
				{
					int dam = MIN(500, p_ptr->chp / 4);
					if (dam < 1) dam = 1;
					if (!get_aim_dir(&dir)) return FALSE;
					ratial_stop_mouth();
					msg_format("���Ȃ��͏��C�̃u���X��f�����B");
					fire_ball(GF_STEAM, dir, dam, -3);
				}
				//�{�u�@�����X�^�[��������
				else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BOB)
				{
					int num = 3 + p_ptr->lev / 16;
					bool flag_success = FALSE;
					u32b summon_mode = PM_FORCE_PET | PM_ALLOW_GROUP;

					for (; num > 0; num--)
					{

						if (summon_specific(-1, py, px, p_ptr->lev, 0, summon_mode))
						{
							flag_success = TRUE;
						}
					}
					if(flag_success)
						msg_print("�ق��牽�҂�����яo���Ă����I");

				}
				else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_TRUMPS_OF_DOOM)
				{
					msg_print("�ǂ��ֈړ����܂����H");
					if (!dimension_door(D_DOOR_NORMAL)) return FALSE;

				}
				else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_EMPTY_BOTTLE)
				{
					if (!summon_specific(0, py, px, plev, SUMMON_ONE_ORC, PM_FORCE_PET | PM_ALLOW_UNIQUE | PM_ALLOW_GROUP))
						msg_print("��������Ȃ������B");

				}

				else
				{
					msg_print("ERROR:���̃��C�V�����p���[�̎��s���̏������L�q����Ă��Ȃ�");

				}




			}

		}
		break;


		/*
		case RACE_DWARF:
#ifdef JP
			msg_print("���͂𒲂ׂ��B");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			break;

		*/			
	#if 0
		case RACE_HOBBIT:
			{
				object_type *q_ptr;
				object_type forge;

				/* Get local object */
				q_ptr = &forge;

				/* Create the food ration */
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

				/* Drop the object from heaven */
				(void)drop_near(q_ptr, -1, py, px);
#ifdef JP
				msg_print("�H���𗿗����č�����B");
#else
				msg_print("You cook some food.");
#endif

			}
			break;
#endif
			/*
		case RACE_GNOME:
#ifdef JP
			msg_print("�p�b�I");
#else
			msg_print("Blink!");
#endif

			teleport_player(10, 0L);
			break;
			*/
			/*
		case RACE_HALF_ORC:
#ifdef JP
			msg_print("�E�C���o�����B");
#else
			msg_print("You play tough.");
#endif

			(void)set_afraid(0);
			break;
			*/
			/*
		case RACE_HALF_TROLL:
#ifdef JP
			msg_print("���������I");
#else
			msg_print("RAAAGH!");
#endif

			(void)set_afraid(0);
			(void)set_shero(10 + randint1(plev), FALSE);
			(void)hp_player(30);
			break;
			*/
			/*
		case RACE_AMBERITE:
			if (command == -1)
			{
#ifdef JP
				msg_print("���Ȃ��͕�������n�߂��B");
#else
				msg_print("You start walking around. ");
#endif
				alter_reality();
			}
			else if (command == -2)
			{
#ifdef JP
				msg_print("���Ȃ��́u�p�^�[���v��S�ɕ`���Ă��̏�������...");
#else
				msg_print("You picture the Pattern in your mind and walk it...");
#endif

				(void)set_poisoned(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);
				(void)set_blind(0);
				(void)set_afraid(0);
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_INT);
				(void)do_res_stat(A_WIS);
				(void)do_res_stat(A_DEX);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_CHR);
				(void)restore_level();
			}
			break;
		*/
			/*
		case RACE_BARBARIAN:
#ifdef JP
			msg_print("�����������I");
#else
			msg_print("Raaagh!");
#endif

			(void)set_afraid(0);
			(void)set_shero(10 + randint1(plev), FALSE);
			(void)hp_player(30);
			break;
		*/
			/*
		case RACE_HALF_OGRE:
#ifdef JP
			msg_print("�����̃��[����T�d�Ɏd�|����...");
#else
			msg_print("You carefully set an explosive rune...");
#endif

			explosive_rune();
			break;

		case RACE_HALF_GIANT:
			if (!get_aim_dir(&dir)) return FALSE;
			(void)wall_to_mud(dir, 20 + randint1(30));
			break;
			*/
/*
		case RACE_HALF_TITAN:
#ifdef JP
			msg_print("�G�𒲍�����...");
#else
			msg_print("You examine your foes...");
#endif

			probing();
			break;

		case RACE_CYCLOPS:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("����Ȋ�𓊂����B");
#else
			msg_print("You throw a huge boulder.");
#endif

			fire_bolt(GF_MISSILE, dir, (3 * plev) / 2);
			break;
*/
/*
		case RACE_YEEK:
			if (!get_aim_dir(&dir)) return FALSE;
			ratial_stop_mouth();
#ifdef JP
			msg_print("�g�̖т��悾���ѐ����グ���I");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_monster(dir, plev);
			break;
*/
/*
		case RACE_KLACKON:
			if (!get_aim_dir(&dir)) return FALSE;
			ratial_stop_mouth();
#ifdef JP
			msg_print("�_��f�����B");
#else
			msg_print("You spit acid.");
#endif

			if (plev < 25) fire_bolt(GF_ACID, dir, plev);
			else fire_ball(GF_ACID, dir, plev, 2);
			break;

		case RACE_KOBOLD:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�ł̃_�[�c�𓊂����B");
#else
			msg_print("You throw a dart of poison.");
#endif

			fire_bolt(GF_POIS, dir, plev);
			break;

		case RACE_NIBELUNG:
#ifdef JP
			msg_print("���͂𒲍������B");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			break;

		case RACE_DARK_ELF:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�}�W�b�N�E�~�T�C����������B");
#else
			msg_print("You cast a magic missile.");
#endif

			fire_bolt_or_beam(10, GF_MISSILE, dir,
			    damroll(3 + ((plev - 1) / 5), 4));
			break;
*/
/*
		case RACE_DRACONIAN:
			{
				int  Type = (one_in_(3) ? GF_COLD : GF_FIRE);
#ifdef JP
				cptr Type_desc = ((Type == GF_COLD) ? "��C" : "��");
#else
				cptr Type_desc = ((Type == GF_COLD) ? "cold" : "fire");
#endif

				if (!get_aim_dir(&dir)) return FALSE;

				if (randint1(100) < plev)
				{
					switch (p_ptr->pclass)
					{
						case CLASS_WARRIOR:
						case CLASS_BERSERKER:
						case CLASS_RANGER:
						case CLASS_TOURIST:
						case CLASS_IMITATOR:
						case CLASS_ARCHER:
						case CLASS_SMITH:
							if (one_in_(3))
							{
								Type = GF_MISSILE;
#ifdef JP
								Type_desc = "�G�������g";
#else
								Type_desc = "the elements";
#endif
							}
							else
							{
								Type = GF_SHARDS;
#ifdef JP
								Type_desc = "�j��";
#else
								Type_desc = "shards";
#endif
							}
							break;
						case CLASS_MAGE:
						case CLASS_WARRIOR_MAGE:
						case CLASS_HIGH_MAGE:
						case CLASS_SORCERER:
						case CLASS_MAGIC_EATER:
						case CLASS_RED_MAGE:
						case CLASS_BLUE_MAGE:
						case CLASS_MIRROR_MASTER:
							if (one_in_(3))
							{
								Type = GF_MANA;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "mana";
#endif
							}
							else
							{
								Type = GF_DISENCHANT;
#ifdef JP
								Type_desc = "��";
#else
								Type_desc = "disenchantment";
#endif
							}
							break;
						case CLASS_CHAOS_WARRIOR:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_CHAOS;
#ifdef JP
								Type_desc = "�J�I�X";
#else
								Type_desc = "chaos";
#endif
							}
							break;
						case CLASS_MONK:
						case CLASS_SAMURAI:
						case CLASS_FORCETRAINER:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "sound";
#endif
							}
							break;
						case CLASS_MINDCRAFTER:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_PSI;
#ifdef JP
								Type_desc = "���_�G�l���M�[";
#else
								Type_desc = "mental energy";
#endif
							}
							break;
						case CLASS_PRIEST:
						case CLASS_PALADIN:
							if (one_in_(3))
							{
								Type = GF_HELL_FIRE;
#ifdef JP
								Type_desc = "�n���̍���";
#else
								Type_desc = "hellfire";
#endif
							}
							else
							{
								Type = GF_HOLY_FIRE;
#ifdef JP
								Type_desc = "���Ȃ鉊";
#else
								Type_desc = "holy fire";
#endif
							}
							break;
						case CLASS_ROGUE:
						case CLASS_NINJA:
							if (one_in_(3))
							{
								Type = GF_DARK;
#ifdef JP
								Type_desc = "�Í�";
#else
								Type_desc = "darkness";
#endif
							}
							else
							{
								Type = GF_POIS;
#ifdef JP
								Type_desc = "��";
#else
								Type_desc = "poison";
#endif
							}
							break;
						case CLASS_BARD:
							if (!one_in_(3))
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "sound";
#endif
							}
							else
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							break;
					}
				}

				ratial_stop_mouth();

#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B", Type_desc);
#else
				msg_format("You breathe %s.", Type_desc);
#endif

				fire_ball(Type, dir, plev * 2,
				    -(plev / 15) - 1);
			}
			break;

		case RACE_MIND_FLAYER:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���Ȃ��͏W�����A�ڂ��Ԃ��P����...");
#else
			msg_print("You concentrate and your eyes glow red...");
#endif

			fire_bolt(GF_PSI, dir, plev);
			break;

		case RACE_IMP:
			if (!get_aim_dir(&dir)) return FALSE;
			if (plev >= 30)
			{
#ifdef JP
				msg_print("�t�@�C�A�E�{�[����������B");
#else
				msg_print("You cast a ball of fire.");
#endif

				fire_ball(GF_FIRE, dir, plev, 2);
			}
			else
			{
#ifdef JP
				msg_print("�t�@�C�A�E�{���g��������B");
#else
				msg_print("You cast a bolt of fire.");
#endif

				fire_bolt(GF_FIRE, dir, plev);
			}
			break;

		case RACE_GOLEM:
			(void)set_shield(randint1(20) + 30, FALSE);
			break;
*/
/*
		case RACE_SKELETON:
		case RACE_ZOMBIE:
#ifdef JP
			msg_print("���Ȃ��͎������G�l���M�[�����߂����Ǝ��݂��B");
#else
			msg_print("You attempt to restore your lost energies.");
#endif

			(void)restore_level();
			break;
*/

		case RACE_VAMPIRE:
			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return FALSE;
			}
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir(&dir,FALSE)) return FALSE;   /* was get_aim_dir */
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				ratial_stop_mouth();

				if (!c_ptr->m_idx)
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

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   /* Dmg */
				if (drain_life(dir, dummy))
				{
					if (p_ptr->food < PY_FOOD_FULL)
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
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
				}
				else
#ifdef JP
					msg_print("�����B�Ђǂ������B");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;
/*
		case RACE_SPECTRE:
			if (!get_aim_dir(&dir)) return FALSE;
			ratial_stop_mouth();
#ifdef JP
			msg_print("���Ȃ��͂��ǂ남�ǂ낵�����ѐ����������I");
#else
			msg_print("You emit an eldritch howl!");
#endif

			(void)fear_monster(dir, plev);
			break;
*/
		/*
		case RACE_SPRITE:
#ifdef JP
			msg_print("���Ȃ��͖��@�̕��𓊂�����...");
#else
			msg_print("You throw some magic dust...");
#endif

			if (plev < 25) sleep_monsters_touch();
			else (void)sleep_monsters(plev);
			break;
*/
			/*
		case RACE_DEMON:
			{
				int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
				if (!get_aim_dir(&dir)) return FALSE;
				ratial_stop_mouth();
#ifdef JP
				msg_format("���Ȃ���%s�̃u���X��f�����B",((type == GF_NETHER) ? "�n��" : "�Ή�"));
#else
				msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

				fire_ball(type, dir, plev * 3, -(plev / 15) - 1);
			}
			break;
		case RACE_KUTAR:
			(void)set_tsubureru(randint1(20) + 30, FALSE);
			break;
		case RACE_ANDROID:
			if (!get_aim_dir(&dir)) return FALSE;
			if (plev < 10)
			{
#ifdef JP
				msg_print("���C�K���𔭎˂����B");
#else
				msg_print("You fire your ray gun.");
#endif
				fire_bolt(GF_MISSILE, dir, (plev+1) / 2);
			}
			else if (plev < 25)
			{
#ifdef JP
				msg_print("�u���X�^�[�𔭎˂����B");
#else
				msg_print("You fire your blaster.");
#endif
				fire_bolt(GF_MISSILE, dir, plev);
			}
			else if (plev < 35)
			{
#ifdef JP
				msg_print("�o�Y�[�J�𔭎˂����B");
#else
				msg_print("You fire your bazooka.");
#endif
				fire_ball(GF_MISSILE, dir, plev * 2, 2);
			}
			else if (plev < 45)
			{
#ifdef JP
				msg_print("�r�[���L���m���𔭎˂����B");
#else
				msg_print("You fire a beam cannon.");
#endif
				fire_beam(GF_MISSILE, dir, plev * 2);
			}
			else
			{
#ifdef JP
				msg_print("���P�b�g�𔭎˂����B");
#else
				msg_print("You fire a rocket.");
#endif
				fire_rocket(GF_ROCKET, dir, plev * 5, 2);
			}
			break;
*/
		default:
#ifdef JP
			msg_print("���̎푰�͓���Ȕ\�͂������Ă��܂���B");
#else
			msg_print("This race has no bonus power.");
#endif

			energy_use = 0;
	}
	}







	return TRUE;
}


/*
 * Allow user to choose a power (racial / mutation) to activate
 */
/*:::���C�V�����p���[���g�p*/
///mod140226 info�\����ǉ�
void do_cmd_racial_power(bool only_info)
{
	power_desc_type power_desc[36];
	int             num, i = 0;
	int             ask = TRUE;
	int             lvl = p_ptr->lev;
	bool            flag, redraw, cast = FALSE;
	///class ��m�A����m�̓��ꃌ�C�V��������
	bool            warrior = ((p_ptr->pclass == CLASS_WARRIOR/* || p_ptr->pclass == CLASS_BERSERKER*/) ? TRUE : FALSE);
	char            choice;
	char            out_val[160];
	int menu_line = (use_menu ? 1 : 0);

	if(SAKUYA_WORLD)
	{
		msg_print("���͂��̔\�͂��g���Ȃ��B");
		return;
	}

	for (num = 0; num < 36; num++)
	{
		strcpy(power_desc[num].name, "");
		power_desc[num].number = 0;
	}

	num = 0;

	if (p_ptr->confused)
	{
#ifdef JP
msg_print("�������Ă��ē���\�͂��g���܂���I");
#else
		msg_print("You are too confused to use any powers!");
#endif

		energy_use = 0;
		return;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	//���i���C�@�A��
	if(p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		strcpy(power_desc[num].name, "�A��");

		power_desc[num].level = 10;
		power_desc[num].cost = 10;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 12;
		power_desc[num].info = "�n��ƃ_���W�������s��������B";
		power_desc[num++].number = -105;
	}
	//Extra���[�h�̃A�C�e���J�[�h�i���i���C�ł͎g���Ȃ��j
	else if(EXTRA_MODE && !(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM)))
	{
			strcpy(power_desc[num].name, "�A�C�e���J�[�h���Z�b�g");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 0;
			power_desc[num].info = "�u�A�C�e���J�[�h�v���J�[�h�X���b�g��8��ނ܂Ŋi�[����B";
			power_desc[num++].number = -101;

			strcpy(power_desc[num].name, "�A�C�e���J�[�h���g��");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 0;
			power_desc[num].info = "�Z�b�g�����u�A�C�e���J�[�h�v���g�p����B�g������ꖇ����B";
			power_desc[num++].number = -102;

			strcpy(power_desc[num].name, "�A�C�e���J�[�h����");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 0;
			power_desc[num].info = "�Z�b�g�����u�A�C�e���J�[�h�v���J�[�h�X���b�g������o���B";
			power_desc[num++].number = -103;

			strcpy(power_desc[num].name, "�A�C�e���J�[�h��ǂ�");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 0;
			power_desc[num].info = "�Z�b�g�����u�A�C�e���J�[�h�v�ɏ����ꂽ��������ǂށB";
			power_desc[num++].number = -104;
	}


	///class �E�Ƃ��ƃ��C�V�����p���[����
	///del140202 �E�ƃ��C�V�����S�Ĕp�~ J�R�}���h�Ɉڍs
#if 0
	switch (p_ptr->pclass)
	{
	case CLASS_WARRIOR:
	{
#ifdef JP
strcpy(power_desc[num].name, "���̕���");
#else
		strcpy(power_desc[num].name, "Sword Dancing");
#endif

		power_desc[num].level = 40;
		power_desc[num].cost = 75;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 35;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_HIGH_MAGE:
	if (p_ptr->realm1 == REALM_HEX)
	{
#ifdef JP
		strcpy(power_desc[num].name, "�r������߂�");
#else
		strcpy(power_desc[num].name, "Stop spelling");
#endif
		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_MAGE:
	/* case CLASS_HIGH_MAGE: */
	case CLASS_SORCERER:
	{
#ifdef JP
strcpy(power_desc[num].name, "���͐H��");
#else
		strcpy(power_desc[num].name, "Eat Magic");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 1;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 25;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_PRIEST:
	{
		if (is_good_realm(p_ptr->realm1))
		{
#ifdef JP
strcpy(power_desc[num].name, "����j��");
#else
			strcpy(power_desc[num].name, "Bless Weapon");
#endif

			power_desc[num].level = 35;
			power_desc[num].cost = 70;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 50;
			power_desc[num++].number = -3;
		}
		else
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Evocation");
#endif

			power_desc[num].level = 42;
			power_desc[num].cost = 40;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 35;
			power_desc[num++].number = -3;
		}
		break;
	}
	case CLASS_ROGUE:
	{
#ifdef JP
strcpy(power_desc[num].name, "�q�b�g���A�E�F�C");
#else
		strcpy(power_desc[num].name, "Hit and Away");
#endif

		power_desc[num].level = 8;
		power_desc[num].cost = 12;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 14;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_RANGER:
	case CLASS_SNIPER:
	{
#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[����");
#else
		strcpy(power_desc[num].name, "Probe Monster");
#endif

		power_desc[num].level = 15;
		power_desc[num].cost = 20;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 12;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_PALADIN:
	{
		if (is_good_realm(p_ptr->realm1))
		{
#ifdef JP
strcpy(power_desc[num].name, "�z�[���[�E�����X");
#else
			strcpy(power_desc[num].name, "Holy Lance");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 30;
			power_desc[num++].number = -3;
		}
		else
		{
#ifdef JP
strcpy(power_desc[num].name, "�w���E�����X");
#else
			strcpy(power_desc[num].name, "Hell Lance");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 30;
			power_desc[num++].number = -3;
		}
		break;
	}
	case CLASS_WARRIOR_MAGE:
	{
#ifdef JP
strcpy(power_desc[num].name, "�ϊ�: �g�o���l�o");
#else
		strcpy(power_desc[num].name, "Convert HP to SP");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�ϊ�: �l�o���g�o");
#else
		strcpy(power_desc[num].name, "Convert SP to HP");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 10;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_CHAOS_WARRIOR:
	{
#ifdef JP
strcpy(power_desc[num].name, "���f�̌�");
#else
		strcpy(power_desc[num].name, "Confusing Light");
#endif

		power_desc[num].level = 40;
		power_desc[num].cost = 50;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 25;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_MONK:
	{
#ifdef JP
strcpy(power_desc[num].name, "�\����");
#else
		strcpy(power_desc[num].name, "Assume a Posture");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�S��");
#else
		strcpy(power_desc[num].name, "Double Attack");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = 30;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_MINDCRAFTER:
	case CLASS_FORCETRAINER:
	{
#ifdef JP
strcpy(power_desc[num].name, "�����~��");
#else
		strcpy(power_desc[num].name, "Clear Mind");
#endif

		power_desc[num].level = 15;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_TOURIST:
	{
#ifdef JP
strcpy(power_desc[num].name, "�ʐ^�B�e");
#else
		strcpy(power_desc[num].name, "Take a Photograph");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�^�E�Ӓ�");
#else
		strcpy(power_desc[num].name, "Identify True");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 20;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_IMITATOR:
	{
#ifdef JP
strcpy(power_desc[num].name, "�{�Ԃ�");
#else
		strcpy(power_desc[num].name, "Double Revenge");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = 100;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 30;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_BEASTMASTER:
	{
#ifdef JP
strcpy(power_desc[num].name, "�����x�z");
#else
		strcpy(power_desc[num].name, "Dominate a Living Thing");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = (p_ptr->lev+3)/4;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�^�E�����x�z");
#else
		strcpy(power_desc[num].name, "Dominate Living Things");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = (p_ptr->lev+20)/2;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_ARCHER:
	{
#ifdef JP
strcpy(power_desc[num].name, "�e/��̐���");
#else
		strcpy(power_desc[num].name, "Create Ammo");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_MAGIC_EATER:
	{
#ifdef JP
strcpy(power_desc[num].name, "���͂̎�荞��");
#else
		strcpy(power_desc[num].name, "Absorb Magic");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;

		strcpy(power_desc[num].name, _("���͔���", "Powerful Activation"));
		power_desc[num].level = 10;
		power_desc[num].cost = 10 + (lvl - 10) / 2;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_BARD:
	{
#ifdef JP
strcpy(power_desc[num].name, "�̂��~�߂�");
#else
		strcpy(power_desc[num].name, "Stop Singing");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_RED_MAGE:
	{
#ifdef JP
strcpy(power_desc[num].name, "�A����");
#else
		strcpy(power_desc[num].name, "Double Magic");
#endif

		power_desc[num].level = 48;
		power_desc[num].cost = 20;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_SAMURAI:
	{
#ifdef JP
strcpy(power_desc[num].name, "�C��������");
#else
		strcpy(power_desc[num].name, "Concentration");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�^");
#else
		strcpy(power_desc[num].name, "Assume a Posture");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_BLUE_MAGE:
	{
#ifdef JP
strcpy(power_desc[num].name, "���[�j���O");
#else
		strcpy(power_desc[num].name, "Learning");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_CAVALRY:
	{
#ifdef JP
strcpy(power_desc[num].name, "�r�n�Ȃ炵");
#else
		strcpy(power_desc[num].name, "Rodeo");
#endif

		power_desc[num].level = 10;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
		break;
	}
/*
	case CLASS_BERSERKER:
	{
#ifdef JP
strcpy(power_desc[num].name, "�A��");
#else
		strcpy(power_desc[num].name, "Recall");
#endif

		power_desc[num].level = 10;
		power_desc[num].cost = 10;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 20;
		power_desc[num++].number = -3;
		break;
	}
*/
	case CLASS_MIRROR_MASTER:
	{
#ifdef JP
strcpy(power_desc[num].name, "������");
#else
		strcpy(power_desc[num].name, "Break Mirrors");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�Ð�");
#else
		strcpy(power_desc[num].name, "Mirror Concentration");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_SMITH:
	{
#ifdef JP
strcpy(power_desc[num].name, "�ڗ���");
#else
		strcpy(power_desc[num].name, "Judgment");
#endif

		power_desc[num].level = 5;
		power_desc[num].cost = 15;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_NINJA:
	{
#ifdef JP
strcpy(power_desc[num].name, "���삯");
#else
		strcpy(power_desc[num].name, "Quick Walk");
#endif

		power_desc[num].level = 20;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	default:
#ifdef JP
strcpy(power_desc[0].name, "(�Ȃ�)");
#else
		strcpy(power_desc[0].name, "(none)");
#endif

	}
#endif
	///magic �ϐg���̃��C�V����
	if (p_ptr->mimic_form)
	{
		switch (p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
#ifdef JP
sprintf(power_desc[num].name, "�n��/�Ή��̃u���X (�_���[�W %d)", lvl * 3);
#else
			sprintf(power_desc[num].name, "Nether or Fire Breath (dam %d)", lvl * 3);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10+lvl/3;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 20;
			power_desc[num].info = "�Ή����n���̂ǂ��炩�̑����̃u���X��f���B";
			power_desc[num++].number = -1;
			break;
		case MIMIC_VAMPIRE:
#ifdef JP
strcpy(power_desc[num].name, "�z���̉�");
#else
			strcpy(power_desc[num].name, "Drain Life");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = 1 + (lvl / 3);
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 9;
			power_desc[num].info = "�G�Ɋ��ݕt���Č����z���A�̗͂��z�����Ė����x���񕜂�����B";
			power_desc[num++].number = -1;
			break;
		case MIMIC_GIGANTIC:
		case MIMIC_SHINMYOU:
#ifdef JP
strcpy(power_desc[num].name, "���̑傫���ɖ߂�");
#else
			strcpy(power_desc[num].name, "Drain Life");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 0;
			power_desc[num].info = "���̑傫���ɖ߂�B";
			power_desc[num++].number = -1;
			break;

		case MIMIC_BEAST:
			strcpy(power_desc[num].name, "���̎p�ɖ߂�");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 0;
			power_desc[num].info = "���̎p�ɖ߂�B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "���K");
			power_desc[num].level = 20;
			power_desc[num].cost = 20;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 5;
			power_desc[num].info = "���̏�Ŗi����B���E���̓G�����|�A���������Ď����͋���m������B���͂̐Q�Ă���G���N����B";
			power_desc[num++].number = -2;

			strcpy(power_desc[num].name, "�ːi");
			power_desc[num].level = 30;
			power_desc[num].cost = 50;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 30;
			power_desc[num].info = "�w��ʒu�܂ōő�5�O���b�h�i�݁A���̓r���ɂ���G�Ƀ_���[�W(���x��*4)��^��������΂��B�N���s�\�n�`�⋐��ȓG�ɓ�����Ǝ~�܂�B";
			power_desc[num++].number = -3;

			break;
		case MIMIC_SLIME:
			strcpy(power_desc[num].name, "���̎p�ɖ߂�");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 0;
			power_desc[num].info = "���̎p�ɖ߂�B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "�_�̃u���X");
			power_desc[num].level = 40;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 30;
			power_desc[num].info = "���ݑ̗͂�1/3�_���[�W�̎_�̃u���X��f���B";
			power_desc[num++].number = -2;

			break;


		case MIMIC_MARISA:
			strcpy(power_desc[num].name, "���̎p�ɖ߂�");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 0;
			power_desc[num].info = "���̎p�ɖ߂�B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "�}�X�^�[�X�p�[�N(�U)");
			power_desc[num].level = 24;
			power_desc[num].cost = 36;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 30;
			power_desc[num].info = "���x��*3�̈З͂̊j�M�����̃��[�U�[����B";
			power_desc[num++].number = -2;

			strcpy(power_desc[num].name, "�E�B�b�`���C���C��(�U)");
			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 20;
			power_desc[num].info = "ⴂɌׂ��ēˌ�����B�G�ɂԂ���ƃ��x��*2�̖������̃_���[�W��^����B";
			power_desc[num++].number = -3;

			strcpy(power_desc[num].name, "�t�@�C�i���X�p�[�N(�U)");
			power_desc[num].level = 40;
			power_desc[num].cost = 60;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 50;
			power_desc[num].info = "���x��*7�̈З͂̕��𑮐��̑�^���[�U�[����B";
			power_desc[num++].number = -4;
			break;

		case MIMIC_MIST:
			strcpy(power_desc[num].name, "���̎p�ɖ߂�");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 0;
			power_desc[num].info = "���̎p�ɖ߂�B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "���ړ�");
			power_desc[num].level = 30;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 20;
			power_desc[num].info = "���ɂȂ��Ď��E���̔C�ӂ̏ꏊ�Ɉړ�����B��R�X�g�����s���͂��{�����B";
			power_desc[num++].number = -2;
			break;

		case MIMIC_DRAGON:
		{
			int rank = p_ptr->mimic_dragon_rank;
			strcpy(power_desc[num].name, "���̎p�ɖ߂�");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 0;
			power_desc[num].info = "���̎p�ɖ߂�B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "�Ή��̃u���X");
			power_desc[num].level = 30;
			power_desc[num].cost = 50;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 25;
			power_desc[num].info = "����HP��1/3(�ő�1600)�̈З͂̉Ή��̃u���X��f���B";
			power_desc[num++].number = -2;

			if(rank > 119)
			{
				strcpy(power_desc[num].name, "�v���Y�}�̃u���X");
				power_desc[num].level = 35;
				power_desc[num].cost = 70;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 30;
				power_desc[num].info = "����HP��1/4(�ő�1200)�̈З͂̃v���Y�}�̃u���X��f���B";
				power_desc[num++].number = -3;
			}

			if(rank > 124)
			{
				strcpy(power_desc[num].name, "�j�M�̃u���X");
				power_desc[num].level = 40;
				power_desc[num].cost = 80;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 35;
				power_desc[num].info = "����HP��1/4(�ő�1000)�̈З͂̊j�M�̃u���X��f���B";
				power_desc[num++].number = -4;
			}
			if(rank > 129)
			{
				strcpy(power_desc[num].name, "�Ռ��g�̃u���X");
				power_desc[num].level = 45;
				power_desc[num].cost = 100;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 35;
				power_desc[num].info = "����HP��1/6(�ő�450)�̈З͂̏Ռ��g�̃u���X��f���B";
				power_desc[num++].number = -5;
			}
			if(rank > 134)
			{
				strcpy(power_desc[num].name, "�J�I�X�̃u���X");
				power_desc[num].level = 45;
				power_desc[num].cost = 130;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 35;
				power_desc[num].info = "����HP��1/6(�ő�600)�̈З͂̃J�I�X�̃u���X��f���B";
				power_desc[num++].number = -6;
			}
			if(rank > 139)
			{
				strcpy(power_desc[num].name, "���ʍ����̃u���X");
				power_desc[num].level = 48;
				power_desc[num].cost = 160;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 40;
				power_desc[num].info = "����HP��1/7(�ő�600)�̈З͂̈��ʍ��������̃u���X��f���B����ϐ��Œ�R�����B";
				power_desc[num++].number = -7;
			}
			if(rank > 149)
			{
				strcpy(power_desc[num].name, "�g���C");
				power_desc[num].level = 50;
				power_desc[num].cost = 200;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 45;
				power_desc[num].info = "����HP��1/6(�ő�555)�̈З͂̕��𑮐��̋���ȃ��[�U�[����B";
				power_desc[num++].number = -8;
			}
			if(rank > 154)
			{
				strcpy(power_desc[num].name, "�d���̃u���X");
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 25;
				power_desc[num].info = "����HP��1/3(�ő�1600)�̈З͂̓d���̃u���X��f���B";
				power_desc[num++].number = -9;
			}
			if(rank > 159)
			{
				strcpy(power_desc[num].name, "��C�̃u���X");
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 25;
				power_desc[num].info = "����HP��1/3(�ő�1600)�̈З̗͂�C�̃u���X��f���B";
				power_desc[num++].number = -10;
			}
			if(rank > 164)
			{
				strcpy(power_desc[num].name, "�_�̃u���X");
				power_desc[num].level = 30;
				power_desc[num].cost = 50;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 25;
				power_desc[num].info = "����HP��1/3(�ő�1600)�̈З͂̎_�̃u���X��f���B";
				power_desc[num++].number = -11;
			}

			if(rank > 169)
			{
				strcpy(power_desc[num].name, "�����̔�");
				power_desc[num].level = 50;
				power_desc[num].cost = 100;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 50;
				power_desc[num].info = "�ꏊ���w�肵�ăe���|�[�g����B";
				power_desc[num++].number = -12;
			}


			break;
		}
		case MIMIC_CAT:
			strcpy(power_desc[num].name, "�l�^�ɂȂ�");
			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 0;
			power_desc[num].info = "���̎p�ɖ߂�B";
			power_desc[num++].number = -1;
			break;


		case MIMIC_NUE:
		{
			strcpy(power_desc[num].name, "�V���[�g�E�e���|�[�g");
			power_desc[num].level = 1;
			power_desc[num].cost = 3;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 5;
			power_desc[num].info = "����10�̃e���|�[�g������B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "�K�̖���");
			power_desc[num].level = 7;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 5;
			power_desc[num].info = "���E���̓G�����|�A����������B";
			power_desc[num++].number = -2;

			strcpy(power_desc[num].name, "�K�X�̃u���X");
			power_desc[num].level = 15;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 15;
			power_desc[num].info = "����HP��1/5(�ő�300)�̈З͂̓ł̃u���X��f���B";
			power_desc[num++].number = -3;

			strcpy(power_desc[num].name, "����");
			power_desc[num].level = 24;
			power_desc[num].cost = 16;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 20;
			power_desc[num].info = "�w�肵���ꏊ�ɈЗ�(���x��*5)�̗��𗎂Ƃ��B";
			power_desc[num++].number = -4;

			strcpy(power_desc[num].name, "�Í��̃u���X");
			power_desc[num].level = 32;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 30;
			power_desc[num].info = "����HP��1/4(�ő�400)�̈З͂̈Í��̃u���X��f���B";
			power_desc[num++].number = -5;

			strcpy(power_desc[num].name, "�J�I�X�̃u���X");
			power_desc[num].level = 40;
			power_desc[num].cost = 50;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 40;
			power_desc[num].info = "����HP��1/3(�ő�600)�̈З͂̃J�I�X�̃u���X��f���B";
			power_desc[num++].number = -6;

			break;
			}

		//�h���~�[�ϐg�Ȃ�
		case MIMIC_METAMORPHOSE_NORMAL:
		case MIMIC_METAMORPHOSE_MONST:
		case MIMIC_METAMORPHOSE_GIGANTIC:
			{

				strcpy(power_desc[num].name, "���̎p�ɂ��Ē��ׂ�");
				power_desc[num].level = 1;
				power_desc[num].cost = 0;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 0;
				power_desc[num].info = "���ϐg���̃����X�^�[�̏��𓾂�B";
				power_desc[num++].number = -1;

				strcpy(power_desc[num].name, "���̎p�ɖ߂�");
				power_desc[num].level = 1;
				power_desc[num].cost = 0;
				power_desc[num].stat = A_CON;
				power_desc[num].fail = 0;
				power_desc[num].info = "���̎p�ɖ߂�B";
				power_desc[num++].number = -2;

				strcpy(power_desc[num].name, "���Z");
				power_desc[num].level = 1;
				power_desc[num].cost = 0;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 0;
				power_desc[num].info = "�ϐg���̃����X�^�[�̓��Z���g���B���Z�̎�ނɂ���ď���MP�␬�������ω�����B";
				power_desc[num++].number = -3;


			}
			break;

		}

	}
	else
	{
	///race �푰���ƃ��C�V����
	switch (p_ptr->prace)
	{

		case RACE_SENNIN:
#ifdef JP
strcpy(power_desc[num].name, "�����~��");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 1;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 20;
			power_desc[num].info = "���_���������܂��AMP���킸���ɉ񕜂�����B";
			power_desc[num++].number = -1;
			break;
		case RACE_TENNIN:
#ifdef JP
strcpy(power_desc[num].name, "�����~��");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 1;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 18;
			power_desc[num].info = "���_���������܂��AMP���킸���ɉ񕜂�����B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "�����Z�a");
			power_desc[num].level = 35;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 20;
			power_desc[num].info = "���͂̓����n�����X�^�[��z���ɂ��悤�Ǝ��݂�B";
			power_desc[num++].number = -2;

			break;

		case RACE_KAPPA:
#ifdef JP
strcpy(power_desc[num].name, "���S�C");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 7;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 10;
			power_desc[num].info = "(3+���x��/5)d4�̈З͂̐������{���g����B";
			power_desc[num++].number = -1;

#ifdef JP
strcpy(power_desc[num].name, "�ʔw");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 10;
			power_desc[num].info = "���ꂽ�Ƃ���ɂ��郂���X�^�[���U������B�����ɉ����čU���񐔂�����B";
			power_desc[num++].number = -2;
#ifdef JP
strcpy(power_desc[num].name, "�������荞��");
#endif

			power_desc[num].level = 35;
			power_desc[num].cost = 24;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 30;
			power_desc[num].info = "�אڂ����G�𐅒��Ɉ������荞�ށB����������ɂ��Ȃ��Ǝg���Ȃ��B����ȓG����ł���G�ɂ͌����ɂ����B";
			power_desc[num++].number = -3;


			break;
		case RACE_MAGICIAN:
#ifdef JP
strcpy(power_desc[num].name, "���͒D��");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 1;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 30;
			power_desc[num].info = "�אڂ��������X�^�[��̂��疂�͂�D���B�����x���ȓG��͋����G�ɂ͌����ɂ����B";
			power_desc[num++].number = -1;
			break;

		case RACE_KARASU_TENGU:
#ifdef JP
strcpy(power_desc[num].name, "�ʐ^�B�e");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = 0;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 0;
			power_desc[num].info = "�אڂ����^�[�Q�b�g�̎ʐ^���B�e����B";
			power_desc[num++].number = -1;
#ifdef JP
strcpy(power_desc[num].name, "�}����");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 16;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 36;
			power_desc[num].info = "����(���x��/5)�̎��E���̎w�肵���ʒu�ֈ�u�ňړ�����B�G�E�X�E�ǂȂǂɎՂ���Ƃ����Ŏ~�܂�B";
			power_desc[num++].number = -2;
			break;



		case RACE_HAKUROU_TENGU:
#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[���m");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 3;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num].info = "���a(���x��/2+10)�܂ł̃����X�^�[�̋C�z���@�m����B";
			power_desc[num++].number = -1;

#ifdef JP
strcpy(power_desc[num].name, "�H�Ɛ���");
#endif
			power_desc[num].level = 15;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num].info = "���͂̐H�ׂ�����̂�T���Čg�ѐH���Ɏd�グ��B";
			power_desc[num++].number = -2;
#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[����");
#else
			strcpy(power_desc[num].name, "Probing");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 20;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 20;
			power_desc[num].info = "���E���̃����X�^�[�̏ڍׂȏ��𓾂�B";
			power_desc[num++].number = -3;
			break;

		case RACE_ONI:

#ifdef JP
			strcpy(power_desc[num].name, "����m��");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 10;
			power_desc[num].info = "�S�̌�����J�����A����m��ԂɂȂ�B";
			power_desc[num++].number = -1;

#ifdef JP
			strcpy(power_desc[num].name, "���K");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 20;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 20;
			power_desc[num].info = "������ӂ��剹������������ōU������B���͂̓G���N����B";
			power_desc[num++].number = -2;


			break;
		case RACE_WARBEAST:
			if(p_ptr->pclass != CLASS_KEINE ) //�d���̓����X�^�[���m���g���Ȃ�
			{

			strcpy(power_desc[num].name, "�����X�^�[���m");
			power_desc[num].level = 3;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num].info = "���a(���x��/2+10)�܂ł̃����X�^�[�̋C�z���@�m����B";
			power_desc[num++].number = -1;
			}
			if(p_ptr->pclass != CLASS_NAZRIN && p_ptr->pclass != CLASS_KEINE ) //�i�Y�[�����͋���m�����g���Ȃ�
			{

			strcpy(power_desc[num].name, "����m��");
			power_desc[num].level = 25;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = (cp_ptr->magicmaster ? 20:10);
			power_desc[num].info = "�ꎞ�I�ɋ���m������B";
			power_desc[num++].number = -2;
			}
			break;
		case RACE_GYOKUTO:

#ifdef JP
strcpy(power_desc[num].name, "�\�𕷂�");
#endif
			power_desc[num].level = 1;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 0;
			power_desc[num].info = "�ʓe�����̃l�b�g���[�N����\�𕷂��B";
			power_desc[num++].number = -1;

#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[���m");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 4;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num].info = "���a(���x��/2+10)�܂ł̃����X�^�[�̋C�z���@�m����B";
			power_desc[num++].number = -2;

#ifdef JP
strcpy(power_desc[num].name, "�V���[�g�E�e���|�[�g");
#else
			strcpy(power_desc[num].name, "Probing");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 8;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num].info = "�Z�����̃e���|�[�g������B";
			power_desc[num++].number = -3;
			break;
		case RACE_YAMAWARO:

#ifdef JP
strcpy(power_desc[num].name, "�ʔw");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 12;
			power_desc[num].info = "���ꂽ�Ƃ���ɂ��郂���X�^�[���U������B�����ɉ����čU���񐔂�����B";
			power_desc[num++].number = -1;
			break;

		case RACE_BAKEDANUKI:
			strcpy(power_desc[num].name, "�����X�^�[���m");
			power_desc[num].level = 5;
			power_desc[num].cost = 4;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num].info = "���a(���x��/2+10)�܂ł̃����X�^�[�̋C�z���@�m����B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "�e���|�[�g");
			power_desc[num].level = 12;
			power_desc[num].cost = 14;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 24;
			power_desc[num].info = "�������̃e���|�[�g������B";
			power_desc[num++].number = -2;

			sprintf(power_desc[num].name, "�ς��g");
			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 50;
			power_desc[num].info = "�U�����󂯂����_���[�W���󂯂��Ƀe���|�[�g����悤�ɂȂ�B���s���邱�Ƃ�����B";
			power_desc[num++].number = -3;
			break;


		case RACE_YOUKO:
#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[���m");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 4;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num].info = "���a(���x��/2+10)�܂ł̃����X�^�[�̋C�z���@�m����B";
			power_desc[num++].number = -1;
#ifdef JP
sprintf(power_desc[num].name, "���_�U�� (�_���[�W %d)", lvl*2);
#else
			sprintf(power_desc[num].name, "Mind Blast (dam %d)", lvl);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 14;
			power_desc[num].info = "�^�[�Q�b�g�ɑ΂��A���x��*2�̃_���[�W�̐��_�U�����s���B";
			power_desc[num++].number = -2;
#ifdef JP
strcpy(power_desc[num].name, "���f");
#else
			strcpy(power_desc[num].name, "Dazzle");
#endif

			power_desc[num].level =25;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 20;
			power_desc[num].info = "���E���̃����X�^�[��N�O�A�����A���|������B";
			power_desc[num++].number = -3;
			break;
	case RACE_HOFGOBLIN:

#ifdef JP
strcpy(power_desc[num].name, "�H�Ɛ���");
#endif
			power_desc[num].level = 5;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num].info = "���͂̐H�ׂ�����̂�T���Čg�ѐH���Ɏd�グ��B";
			power_desc[num++].number = -1;
			break;
		case RACE_NYUDOU:
#ifdef JP
strcpy(power_desc[num].name, "�V���[�g�E�e���|�[�g");
#else
			strcpy(power_desc[num].name, "Probing");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 14;
			power_desc[num].info = "�Z�����̃e���|�[�g������B";
			power_desc[num++].number = -1;

#ifdef JP
strcpy(power_desc[num].name, "�e���|�[�g�E�o�b�N");
#else
			strcpy(power_desc[num].name, "Probing");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 20;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 25;
			power_desc[num].info = "���ꂽ�ꏊ�ɂ���G�������̋߂��ֈ����񂹂�B";
			power_desc[num++].number = -2;
			break;

		case RACE_STRAYGOD:
			deity_racial_power(power_desc, &num);

			break;
		case RACE_FAIRY:
			fairy_racial_power(power_desc, &num);
			break;

		case RACE_NINJA:
#ifdef JP
strcpy(power_desc[num].name, "�X���P������");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 10;
			power_desc[num].info = "�^�[�Q�b�g�֖������̃{���g�𕡐����B�h��͂̍�������ɂ͓�����Ȃ����Ƃ�����B���x�����㏸����Ɛ���������B";
			power_desc[num++].number = -1;

#ifdef JP
strcpy(power_desc[num].name, "�`���h�[�̌ċz");
#endif

			power_desc[num].level = 35;
			power_desc[num].cost = 1;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 60;
			power_desc[num].info = "����Ȍċz�@�Ńj���W���񕜗͂���������B";
			power_desc[num++].number = -2;

			break;
		case RACE_GOLEM:

			strcpy(power_desc[num].name, "���ȏC��");
			power_desc[num].level = 20;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 30;
			power_desc[num].info = "�������[�̏���j�������Â���B";
			power_desc[num++].number = -1;
//			break;


#ifdef JP
strcpy(power_desc[num].name, "�ǐH��");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 20;
			power_desc[num].info = "�_���W�����̕ǂ�H�ׂĉh�{�ɂ���B";
			power_desc[num++].number = -2;
			break;



		case RACE_LICH:
#ifdef JP
strcpy(power_desc[num].name, "�G�i�W�[�h���C��");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 10;
			power_desc[num].info = "�G�ɐG����HP��MP��D���B�������邱�Ƃ�����B�����������Ȃ��G�ɂ͖����B";
			power_desc[num++].number = -1;

#ifdef JP
strcpy(power_desc[num].name, "�n���̔g��");
#endif
			power_desc[num].level = 40;
			power_desc[num].cost = 43;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 20;
			power_desc[num].info = "���E���̐����Ƀ_���[�W��^���A�A���f�b�h��ꑮ������B";
			power_desc[num++].number = -2;
			break;

		case RACE_LUNARIAN:

			strcpy(power_desc[num].name, "�A��");
			power_desc[num].level = 5;
			power_desc[num].cost = 20;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 10;
			power_desc[num].info = "�n��ƃ_���W�������s��������B";
			power_desc[num++].number = -1;

			strcpy(power_desc[num].name, "�z������");
			power_desc[num].level = 25;
			power_desc[num].cost = 32;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 40;
			power_desc[num].info = "�z���̋ʓe��q�����Ăяo���B";
			power_desc[num++].number = -2;
			break;
			//v1.1.66
		case RACE_HANIWA:
		case RACE_ANDROID:
			strcpy(power_desc[num].name, "���ȏC��");
			power_desc[num].level = 20;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 30;
			power_desc[num].info = "�������[�̏���j�������Â���B";
			power_desc[num++].number = -1;
			break;

			//v1.1.70 ������@�������͂ɂ���ă��C�V�������Ⴄ
		case RACE_ANIMAL_GHOST:
			switch (CHECK_ANIMAL_GHOST_STRIFE)
			{
			case ANIMAL_GHOST_STRIFE_KEIGA:

				strcpy(power_desc[num].name, "����m��");
				power_desc[num].level = 20;
				power_desc[num].cost = 15;
				power_desc[num].stat = A_STR;
				power_desc[num].fail = (cp_ptr->magicmaster ? 20 : 10);
				power_desc[num].info = "�ꎞ�I�ɋ���m������B";
				power_desc[num++].number = -1;

				strcpy(power_desc[num].name, "�X�s�[�h");
				power_desc[num].level = 35;
				power_desc[num].cost = 25;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 25;
				power_desc[num].info = "�ꎞ�I�ɉ�������B";
				power_desc[num++].number = -2;
				break;


			case ANIMAL_GHOST_STRIFE_KIKETSU:

				strcpy(power_desc[num].name, "�V���[�g�E�e���|�[�g");
				power_desc[num].level = 5;
				power_desc[num].cost = 10;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 15;
				power_desc[num].info = "�Z�����̃e���|�[�g������B";
				power_desc[num++].number = -1;

				strcpy(power_desc[num].name, "��P");
				power_desc[num].level = 35;
				power_desc[num].cost = 30;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 30;
				power_desc[num].info = "���O���b�h���ꂽ�^�[�Q�b�g�̏ꏊ�܂ňړ����Ă��̂܂܍U������B";
				power_desc[num++].number = -2;
				break;

			case ANIMAL_GHOST_STRIFE_GOUYOKU:
				strcpy(power_desc[num].name, "�����X�^�[���m");
				power_desc[num].level = 5;
				power_desc[num].cost = 4;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 12;
				power_desc[num].info = "���a(���x��/2+10)�܂ł̃����X�^�[�̋C�z���@�m����B";
				power_desc[num++].number = -1;

				strcpy(power_desc[num].name, "���ӊ��m");
				power_desc[num].level = 25;
				power_desc[num].cost = 18;
				power_desc[num].stat = A_INT;
				power_desc[num].fail = 20;
				power_desc[num].info = "���͂̒n�`�����m����B";
				power_desc[num++].number = -2;
				break;

			}

			break;

		case RACE_ZASHIKIWARASHI:
			{

				object_type *o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

				strcpy(power_desc[num].name, "�]��");
				power_desc[num].level = 1;
				power_desc[num].cost = 20;
				power_desc[num].stat = A_DEX;
				power_desc[num].fail = 30;
				power_desc[num].info = "���Z��ł��镨�ƕʂ̈ړ����~�ɏZ�ݒ����B";
				power_desc[num++].number = -1;

				//�����̚�:�Ή��̃u���X
				if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU)
				{
					strcpy(power_desc[num].name, "�Ή��̃u���X");
					power_desc[num].level = 15;
					power_desc[num].cost = 30;
					power_desc[num].stat = A_CON;
					power_desc[num].fail = 30;
					power_desc[num].info = "HP/3�̈З͂̉Ή��̃u���X��f���B";
					power_desc[num++].number = -2;
					break;
				}
				//�Ηю��̒���:���C�̃u���X
				if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_MORINJI)
				{
					strcpy(power_desc[num].name, "���C�̃u���X");
					power_desc[num].level = 20;
					power_desc[num].cost = 30;
					power_desc[num].stat = A_CON;
					power_desc[num].fail = 30;
					power_desc[num].info = "HP/4�̈З͂̏��C�̃u���X��f���B";
					power_desc[num++].number = -2;
					break;
				}
				//�}�C�N�����t�g�E�{�u:�����X�^�[��������
				if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BOB)
				{
					strcpy(power_desc[num].name, "�����X�^�[��������");
					power_desc[num].level = 20;
					power_desc[num].cost = 50;
					power_desc[num].stat = A_CHR;
					power_desc[num].fail = 30;
					power_desc[num].info = "�����X�^�[��z���Ƃ��ĕ�����������B";
					power_desc[num++].number = -2;
					break;
				}
				//�^���̐؎D�@�����̔�
				if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_TRUMPS_OF_DOOM)
				{
					strcpy(power_desc[num].name, "�����̔�");
					power_desc[num].level = 30;
					power_desc[num].cost = 50;
					power_desc[num].stat = A_INT;
					power_desc[num].fail = 60;
					power_desc[num].info = "�C�ӂ̒n�_�Ƀe���|�[�g����B�����������Ǝ��s���₷���B";
					power_desc[num++].number = -2;
					break;
				}

				//�󂫕r�@�I�[�N����
				if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_EMPTY_BOTTLE)
				{
					strcpy(power_desc[num].name, "�I�[�N�̏���");
					power_desc[num].level = 10;
					power_desc[num].cost = 5;
					power_desc[num].stat = A_CHR;
					power_desc[num].fail = 30;
					power_desc[num].info = "��̂̃I�[�N��S�u��������������B";
					power_desc[num++].number = -2;
					break;
				}


			}
			break;


		/*
		case RACE_DWARF:
#ifdef JP
strcpy(power_desc[num].name, "�h�A��� ���m");
#else
			strcpy(power_desc[num].name, "Detect Doors+Traps");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		*/
		/*
		case RACE_NIBELUNG:
#ifdef JP
strcpy(power_desc[num].name, "�h�A��� ���m");
#else
			strcpy(power_desc[num].name, "Detect Doors+Traps");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
			break;
			*/
			/*
		case RACE_HOBBIT:
#ifdef JP
strcpy(power_desc[num].name, "�H�Ɛ���");
#else
			strcpy(power_desc[num].name, "Create Food");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
			break;
*/
/*
			case RACE_GNOME:
#ifdef JP
sprintf(power_desc[num].name, "�V���[�g�E�e���|�[�g");
#else
			sprintf(power_desc[num].name, "Blink");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
*/
			/*
			case RACE_HALF_ORC:
#ifdef JP
strcpy(power_desc[num].name, "���|����");
#else
			strcpy(power_desc[num].name, "Remove Fear");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = warrior ? 5 : 10;
			power_desc[num++].number = -1;
			break;
			*/
			/*
		case RACE_HALF_TROLL:
#ifdef JP
strcpy(power_desc[num].name, "����m��");
#else
			strcpy(power_desc[num].name, "Berserk");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = warrior ? 6 : 12;
			power_desc[num++].number = -1;
			break;
			*/
			/*
		case RACE_BARBARIAN:
#ifdef JP
strcpy(power_desc[num].name, "����m��");
#else
			strcpy(power_desc[num].name, "Berserk");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = warrior ? 6 : 12;
			power_desc[num++].number = -1;
			break;
			*/
			/*
		case RACE_AMBERITE:
#ifdef JP
strcpy(power_desc[num].name, "�V���h�E�E�V�t�g");
#else
			strcpy(power_desc[num].name, "Shadow Shifting");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 50;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 50;
			power_desc[num++].number = -1;
#ifdef JP
strcpy(power_desc[num].name, "�p�^�[���E�E�H�[�N");
#else
			strcpy(power_desc[num].name, "Pattern Mindwalking");
#endif

			power_desc[num].level = 40;
			power_desc[num].cost = 75;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 50;
			power_desc[num++].number = -2;
			break;
			*/
/*
		case RACE_HALF_OGRE:
#ifdef JP
strcpy(power_desc[num].name, "�����̃��[��");
#else
			strcpy(power_desc[num].name, "Explosive Rune");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 35;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
			break;
		case RACE_HALF_GIANT:
#ifdef JP
strcpy(power_desc[num].name, "��Ηn��");
#else
			strcpy(power_desc[num].name, "Stone to Mud");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
*/
/*
			case RACE_HALF_TITAN:
#ifdef JP
strcpy(power_desc[num].name, "�X�L�����E�����X�^�[");
#else
			strcpy(power_desc[num].name, "Probing");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_CYCLOPS:
#ifdef JP
sprintf(power_desc[num].name, "��Γ����i�_���[�W %d�j", (3 * lvl) / 2);
#else
			sprintf(power_desc[num].name, "Throw Boulder (dam %d)", (3 * lvl) / 2);
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
			*/
/*
			case RACE_YEEK:
#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[���Q");
#else
			strcpy(power_desc[num].name, "Scare Monster");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
			break;
*/
/*
		case RACE_SPECTRE:
#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[���Q");
#else
			strcpy(power_desc[num].name, "Scare Monster");
#endif

			power_desc[num].level = 4;
			power_desc[num].cost = 6;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 3;
			power_desc[num++].number = -1;
			break;
		case RACE_KLACKON:
#ifdef JP
sprintf(power_desc[num].name, "�_�̑� (�_���[�W %d)", lvl);
#else
			sprintf(power_desc[num].name, "Spit Acid (dam %d)", lvl);
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 9;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 14;
			power_desc[num++].number = -1;
			break;
		case RACE_KOBOLD:
#ifdef JP
sprintf(power_desc[num].name, "�ł̃_�[�c (�_���[�W %d)", lvl);
#else
			sprintf(power_desc[num].name, "Poison Dart (dam %d)", lvl);
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 8;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 14;
			power_desc[num++].number = -1;
			break;
		case RACE_DARK_ELF:
#ifdef JP
sprintf(power_desc[num].name, "�}�W�b�N�E�~�T�C�� (�_���[�W %dd%d)", 3 + ((lvl - 1) / 5), 4);
#else
			sprintf(power_desc[num].name, "Magic Missile (dm %dd%d)", 3 + ((lvl - 1) / 5), 4);
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 2;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 9;
			power_desc[num++].number = -1;
			break;
*/
/*
			case RACE_DRACONIAN:
#ifdef JP
sprintf(power_desc[num].name, "�u���X (�_���[�W %d)", lvl * 2);
#else
			sprintf(power_desc[num].name, "Breath Weapon (dam %d)", lvl * 2);
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = lvl;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_MIND_FLAYER:
#ifdef JP
sprintf(power_desc[num].name, "���_�U�� (�_���[�W %d)", lvl);
#else
			sprintf(power_desc[num].name, "Mind Blast (dam %d)", lvl);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 14;
			power_desc[num++].number = -1;
			break;
		case RACE_IMP:
#ifdef JP
sprintf(power_desc[num].name, "�t�@�C�A�E�{���g/�{�[�� (�_���[�W %d)", lvl);
#else
			sprintf(power_desc[num].name, "Fire Bolt/Ball (dam %d)", lvl);
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
			break;
		case RACE_GOLEM:
#ifdef JP
strcpy(power_desc[num].name, "���Ή� (���� 1d20+30)");
#else
			strcpy(power_desc[num].name, "Stone Skin (dur 1d20+30)");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 8;
			power_desc[num++].number = -1;
			break;
		*/
/*
		case RACE_SKELETON:
		case RACE_ZOMBIE:
#ifdef JP
strcpy(power_desc[num].name, "�o���l����");
#else
			strcpy(power_desc[num].name, "Restore Life");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 18;
			power_desc[num++].number = -1;
			break;
*/
			case RACE_VAMPIRE:
#ifdef JP
strcpy(power_desc[num].name, "�����͋z��");
#else
			strcpy(power_desc[num].name, "Drain Life");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = 1 + (lvl / 3);
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 9;
			power_desc[num].info = "�G�Ɋ��ݕt���Č����z���A�󕠂𖞂����B";
			power_desc[num++].number = -1;
			break;
			/*
		case RACE_SPRITE:
#ifdef JP
strcpy(power_desc[num].name, "���蕲");
#else
			strcpy(power_desc[num].name, "Sleeping Dust");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
			break;
			*/
/*
		case RACE_DEMON:
#ifdef JP
sprintf(power_desc[num].name, "�n��/�Ή��̃u���X (�_���[�W %d)", lvl * 3);
#else
			sprintf(power_desc[num].name, "Nether or Fire Breath (dam %d)", lvl * 3);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10+lvl/3;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 20;
			power_desc[num++].number = -1;
			break;
			*/
			/*
		case RACE_KUTAR:
#ifdef JP
strcpy(power_desc[num].name, "���ɐL�т�");
#else
			strcpy(power_desc[num].name, "Expand Horizontally (dur 30+1d20)");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 8;
			power_desc[num++].number = -1;
			break;
			*/
			/*
		case RACE_ANDROID:
			if (p_ptr->lev < 10)
			{
#ifdef JP
strcpy(power_desc[num].name, "���C�K��");
#else
				strcpy(power_desc[num].name, "Ray Gun");
#endif

				power_desc[num].level = 1;
				power_desc[num].cost = 7;
				power_desc[num].fail = 8;
			}
			else if (p_ptr->lev < 25)
			{
#ifdef JP
strcpy(power_desc[num].name, "�u���X�^�[");
#else
				strcpy(power_desc[num].name, "Blaster");
#endif

				power_desc[num].level = 10;
				power_desc[num].cost = 13;
				power_desc[num].fail = 10;
			}
			else if (p_ptr->lev < 35)
			{
#ifdef JP
strcpy(power_desc[num].name, "�o�Y�[�J");
#else
				strcpy(power_desc[num].name, "Bazooka");
#endif

				power_desc[num].level = 25;
				power_desc[num].cost = 26;
				power_desc[num].fail = 12;
			}
			else if (p_ptr->lev < 45)
			{
#ifdef JP
strcpy(power_desc[num].name, "�r�[���L���m��");
#else
				strcpy(power_desc[num].name, "Beam Cannon");
#endif

				power_desc[num].level = 35;
				power_desc[num].cost = 40;
				power_desc[num].fail = 15;
			}
			else
			{
#ifdef JP
strcpy(power_desc[num].name, "���P�b�g");
#else
				strcpy(power_desc[num].name, "Rocket");
#endif

				power_desc[num].level = 45;
				power_desc[num].cost = 60;
				power_desc[num].fail = 18;
			}
			power_desc[num].stat = A_STR;
			power_desc[num++].number = -1;
			break;
			*/
		default:
		{
			break;
		}
	}
	}
	///mutation �ˑR�ψقɂ�郌�C�V����
	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_BR_ACID)
		{
#ifdef JP
strcpy(power_desc[num].name, "�_�̃u���X");
#else
			strcpy(power_desc[num].name, "Spit Acid");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = (lvl+1)/2;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num].info = "�_�̃u���X��f���B";
			power_desc[num++].number = MUT1_BR_ACID;
		}

		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
strcpy(power_desc[num].name, "���̃u���X");
#else
			strcpy(power_desc[num].name, "Fire Breath");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = (lvl + 1) / 2;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num].info = "�Ή��̃u���X��f���B";
			power_desc[num++].number = MUT1_BR_FIRE;
		}

		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Hypnotic Gaze");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 18;
			power_desc[num].info = "�G��̂𖣗����A�z���ɂ���B";
			power_desc[num++].number = MUT1_HYPN_GAZE;
		}

		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
strcpy(power_desc[num].name, "�O����");
#else
			strcpy(power_desc[num].name, "Telekinesis");
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 9;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 14;
			power_desc[num].info = "���ꂽ�Ƃ���ɂ���A�C�e���𑫌��Ɉ����񂹂�B��������A�C�e���̏d�ʂ�(���x��*0.5)Kg�܂ŁB";
			power_desc[num++].number = MUT1_TELEKINES;
		}

		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
strcpy(power_desc[num].name, "�e���|�[�g");
#else
			strcpy(power_desc[num].name, "Teleport");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 7;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num].info = "����(���x��*4+10)�̃e���|�[�g������B";
			power_desc[num++].number = MUT1_VTELEPORT;
		}

		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
strcpy(power_desc[num].name, "���_�U��");
#else
			strcpy(power_desc[num].name, "Mind Blast");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 3;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num].info = "�G��̂ɑ΂����_�U�������̃{���g����B";
			power_desc[num++].number = MUT1_MIND_BLST;
		}

		if (p_ptr->muta1 & MUT1_BR_ELEC)
		{
#ifdef JP
strcpy(power_desc[num].name, "���̃u���X");
#else
			strcpy(power_desc[num].name, "Emit Radiation");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = (lvl + 1) / 2;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num].info = "���̃u���X��f���B";
			power_desc[num++].number = MUT1_BR_ELEC;
		}

		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
strcpy(power_desc[num].name, "�z���̉�");
#else
			strcpy(power_desc[num].name, "Vampiric Drain");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = (1 + (lvl / 3));
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 9;
			power_desc[num].info = "�G�Ɋ��ݕt���Č����z���A�̗͂��z�����Ė����x���񕜂�����B";
			power_desc[num++].number = MUT1_VAMPIRISM;
		}

		if (p_ptr->muta1 & MUT1_ACCELERATION)
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Smell Metal");
#endif
			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].fail = 25;
			power_desc[num].stat = A_DEX;
			power_desc[num].info = "�ꎞ�I�ɉ�������B";
			power_desc[num++].number = MUT1_ACCELERATION;
		}

		if (p_ptr->muta1 & MUT1_BEAST_EAR)
		{
#ifdef JP
strcpy(power_desc[num].name, "�����X�^�[���m");
#else
			strcpy(power_desc[num].name, "Smell Monsters");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 4;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num].info = "���a(���x��/2+10)�܂ł̃����X�^�[�̋C�z���@�m����B";
			power_desc[num++].number = MUT1_BEAST_EAR;
		}

		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
strcpy(power_desc[num].name, "�V���[�g�E�e���|�[�g");
#else
			strcpy(power_desc[num].name, "Blink");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 3;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 12;
			power_desc[num].info = "����10�̃e���|�[�g������B";
			power_desc[num++].number = MUT1_BLINK;
		}

		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
strcpy(power_desc[num].name, "��H��");
#else
			strcpy(power_desc[num].name, "Eat Rock");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num].info = "�ǂ�H�ׂċ󕠂𖞂����B";
			power_desc[num++].number = MUT1_EAT_ROCK;
		}

		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
strcpy(power_desc[num].name, "�ʒu����");
#else
			strcpy(power_desc[num].name, "Swap Position");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 16;
			power_desc[num].info = "�w��^�[�Q�b�g�ƈʒu����������B";
			power_desc[num++].number = MUT1_SWAP_POS;
		}

		if (p_ptr->muta1 & MUT1_BR_SOUND)
		{
#ifdef JP
strcpy(power_desc[num].name, "�����U��");
#else
			strcpy(power_desc[num].name, "Shriek");
#endif

			power_desc[num].level = 23;
			power_desc[num].cost = (lvl + 1) / 2;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num].info = "�剹��������A���͂̓G�ɍU������B";
			power_desc[num++].number = MUT1_BR_SOUND;
		}

		if (p_ptr->muta1 & MUT1_BR_COLD)
		{
#ifdef JP
strcpy(power_desc[num].name, "��C�̃u���X");
#else
			strcpy(power_desc[num].name, "Illuminate");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = (lvl + 1) / 2;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num].info = "��C�̃u���X��f���B";
			power_desc[num++].number = MUT1_BR_COLD;
		}

		if (p_ptr->muta1 & MUT1_DET_ITEM)
		{
#ifdef JP
strcpy(power_desc[num].name, "�A�C�e�����m");
#else
			strcpy(power_desc[num].name, "Detect Curses");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 15;
			power_desc[num].info = "�߂��ɗ����Ă���A�C�e����T�肾���B";
			power_desc[num++].number = MUT1_DET_ITEM;
		}

		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
strcpy(power_desc[num].name, "����m��");
#else
			strcpy(power_desc[num].name, "Berserk");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 8;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 14;
			power_desc[num].info = "������ĂыN�����A�ꎞ�I�ɋ���m������B";
			power_desc[num++].number = MUT1_BERSERK;
		}

		if (p_ptr->muta1 & MUT1_BR_POIS)
		{
#ifdef JP
strcpy(power_desc[num].name, "�K�X�̃u���X");
#else
			strcpy(power_desc[num].name, "Polymorph");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = (lvl + 1) / 2;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num].info = "�ł̃u���X��f���B";
			power_desc[num++].number = MUT1_BR_POIS;
		}

		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
strcpy(power_desc[num].name, "�~�_�X�̎�");
#else
			strcpy(power_desc[num].name, "Midas Touch");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num].info = "�w�肵���A�C�e����G���ċ��ɕς���B���z�̓A�C�e���̉��l��1/3�ƂȂ�B";
			power_desc[num++].number = MUT1_MIDAS_TCH;
		}

		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
strcpy(power_desc[num].name, "�J�r����");
#else
			strcpy(power_desc[num].name, "Grow Mold");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = 6;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 14;
			power_desc[num].info = "�����̎��͂ɃJ�r�̃����X�^�[����������B";
			power_desc[num++].number = MUT1_GROW_MOLD;
		}

		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
strcpy(power_desc[num].name, "�G�������g�ϐ�");
#else
			strcpy(power_desc[num].name, "Resist Elements");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 12;
			power_desc[num].info = "�ꎞ�I�Ɏ_�A�d���A�Ή��A��C�A�ł̂����������̑ϐ��𓾂�B";
			power_desc[num++].number = MUT1_RESIST;
		}

		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
strcpy(power_desc[num].name, "�n�k");
#else
			strcpy(power_desc[num].name, "Earthquake");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 16;
			power_desc[num].info = "�n�k���N�����A�_���W�����̏���ǂ������_���ɓ���ւ���B";
			power_desc[num++].number = MUT1_EARTHQUAKE;
		}

		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
strcpy(power_desc[num].name, "���͐H��");
#else
			strcpy(power_desc[num].name, "Eat Magic");
#endif

			power_desc[num].level = 17;
			power_desc[num].cost = 1;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num].info = "������̖��͂��z������MP���񕜂�����B��������邱�Ƃ�����B";
			power_desc[num++].number = MUT1_EAT_MAGIC;
		}

		if (p_ptr->muta1 & MUT1_RAISE_WOOD)
		{
#ifdef JP
strcpy(power_desc[num].name, "���ؐ���");
#else
			strcpy(power_desc[num].name, "Weigh Magic");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 20;
			power_desc[num].info = "�����̂���ꏊ��X�тɂ���B";
			power_desc[num++].number = MUT1_RAISE_WOOD;
		}

		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
strcpy(power_desc[num].name, "���B�j�~");
#else
			strcpy(power_desc[num].name, "Sterilize");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 23;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 15;
			power_desc[num].info = "�t���A�̑��B���郂���X�^�[������ȏ㑝�B�ł��Ȃ��Ȃ�B";
			power_desc[num++].number = MUT1_STERILITY;
		}

		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
strcpy(power_desc[num].name, "�q�b�g���A�E�F�C");
#else
			strcpy(power_desc[num].name, "Panic Hit");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 14;
			power_desc[num].info = "�U�����d�|���A���̌��u�ł��̏ꂩ�痣�E����B���s���邱�Ƃ�����B";
			power_desc[num++].number = MUT1_PANIC_HIT;
		}

		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
strcpy(power_desc[num].name, "���C�̖���");
#else
			strcpy(power_desc[num].name, "Dazzle");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 8;
			power_desc[num].info = "���E���̃����X�^�[�𖰂点�A���������A���|������B";
			power_desc[num++].number = MUT1_DAZZLE;
		}

		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
strcpy(power_desc[num].name, "�ڂ���r�[��");
#else
			strcpy(power_desc[num].name, "Laser Eye");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num].info = "�M�������̃r�[������B";
			power_desc[num++].number = MUT1_LASER_EYE;
		}

		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
strcpy(power_desc[num].name, "�A��");
#else
			strcpy(power_desc[num].name, "Recall");
#endif

			power_desc[num].level = 17;
			power_desc[num].cost = 50;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 16;
			power_desc[num].info = "�n��ƃ_���W�������s��������B���ʂ̔����܂łɃ^�C�����O������B";
			power_desc[num++].number = MUT1_RECALL;
		}

		if (p_ptr->muta1 & MUT1_ID_MUSHROOM)
		{
#ifdef JP
strcpy(power_desc[num].name, "�L�m�R�Ӓ�");
#else
			strcpy(power_desc[num].name, "Banish Evil");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 3;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 8;
			power_desc[num].info = "�������Ă���L�m�R��S�ĊӒ肷��B";
			power_desc[num++].number = MUT1_ID_MUSHROOM;
		}
		//v1.1.39
		if (p_ptr->muta1 & MUT1_DANMAKU)
		{
			strcpy(power_desc[num].name, "�G�߂̗�");

			power_desc[num].level = 10;
			power_desc[num].cost = 40;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 25;
			power_desc[num].info = "(����*5)�̈З͂̋���ȃ{�[������B";
			power_desc[num++].number = MUT1_DANMAKU;
		}

#if 0
		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
strcpy(power_desc[num].name, "�A�C�e������");
#else
			strcpy(power_desc[num].name, "Throw Object");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = lvl;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 6;
			/* XXX_XXX_XXX Hack! MUT1_LAUNCHER counts as negative... */
			power_desc[num++].number = 3;
		}
#endif
	}


	//power_desc.number�����̒l��muta1�Ɏg���Ă���Amut4�ɐV�����ǉ����������ψق�����ꏊ���Ȃ��̂ŁA�s�{�ӂȂ���-99�Ɍ��ߑł�����B
	if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
	{
		strcpy(power_desc[num].name, "�G�i�W�[�h���C��");
		power_desc[num].level = 30;
		power_desc[num].cost = 10;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 10;
		power_desc[num].info = "�G�ɐG����HP��MP��D���B�������邱�Ƃ�����B�����������Ȃ��G�ɂ͖����B";
		power_desc[num++].number = -99;

	}



	if(p_ptr->clawextend)
	{
			strcpy(power_desc[num].name, "�n���܎E�@");
			power_desc[num].level = 20;
			power_desc[num].cost = 25;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 15;
			power_desc[num].info = "�אڂ����G�S�ĂɍU������B�őϐ��̂Ȃ��G�ɑ΂��č����_���[�W��^����B";
			power_desc[num++].number = -100;
	}


	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt */
#ifdef JP

	if(!num)
	{
		msg_print("���Ȃ��͈ӎ����Ďg���悤�ȓ���\�͂������Ă��Ȃ��B");
		return;
	}
if(only_info)
	(void) strnfmt(out_val, 78, "(����\�� %c-%c, *'�ňꗗ, ESC�Œ��f) �ǂ̓���\�͂̐��������܂����H",	I2A(0), (num <= 26) ? I2A(num - 1) : '0' + num - 27);
else
	(void) strnfmt(out_val, 78, "(����\�� %c-%c, *'�ňꗗ, ESC�Œ��f) �ǂ̓���\�͂��g���܂����H",	I2A(0), (num <= 26) ? I2A(num - 1) : '0' + num - 27);

#else
	(void)strnfmt(out_val, 78, "(Powers %c-%c, *=List, ESC=exit) Use which power? ",
		I2A(0), (num <= 26) ? I2A(num - 1) : '0' + num - 27);
#endif


#ifdef ALLOW_REPEAT
if (!repeat_pull(&i) || i<0 || i>=num) {
#endif /* ALLOW_REPEAT */
	if (use_menu) screen_save();
	 /* Get a spell from the user */

	choice = (always_show_list || use_menu) ? ESCAPE:1;
	while (!flag)
	{
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break; 

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					energy_use = 0;
					return;
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (num - 1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case '6':
				case 'l':
				case 'L':
				case '4':
				case 'h':
				case 'H':
				{
					if (menu_line > 18)
						menu_line -= 18;
					else if (menu_line+18 <= num)
						menu_line += 18;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if (menu_line > num) menu_line -= num;
		}
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				byte y = 1, x = 0;
				int ctr = 0;
				char dummy[80];
				char letter;
				int x1, y1;

				strcpy(dummy, "");

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				/* Print header(s) */
				if (num < 18)
#ifdef JP
prt("                            Lv   MP ����", y++, x);
#else
					prt("                            Lv Cost Fail", y++, x);
#endif

				else
#ifdef JP
prt("                            Lv   MP ����                            Lv   MP ����", y++, x);
#else
					prt("                            Lv Cost Fail                            Lv Cost Fail", y++, x);
#endif


				/* Print list */
				while (ctr < num)
				{
					x1 = ((ctr < 18) ? x : x + 40);
					y1 = ((ctr < 18) ? y + ctr : y + ctr - 18);

					if (use_menu)
					{
#ifdef JP
						if (ctr == (menu_line-1)) strcpy(dummy, " �t ");
#else
						if (ctr == (menu_line-1)) strcpy(dummy, " >  ");
#endif
						else strcpy(dummy, "    ");
					}
					else
					{
						/* letter/number for power selection */
						if (ctr < 26)
							letter = I2A(ctr);
						else
							letter = '0' + ctr - 26;
						sprintf(dummy, " %c) ",letter);
					}
					strcat(dummy, format("%-23.23s %2d %4d %3d%%",
						power_desc[ctr].name, power_desc[ctr].level, power_desc[ctr].cost,
						100 - racial_chance(&power_desc[ctr])));
					prt(dummy, y1, x1);
					ctr++;
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
			if (choice == '\r' && num == 1)
			{
				choice = 'a';
			}

			if (isalpha(choice))
			{
				/* Note verify */
				ask = (isupper(choice));

				/* Lowercase */
				if (ask) choice = tolower(choice);

				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
			}
			else
			{
				ask = FALSE; /* Can't uppercase digits */

				i = choice - '0' + 26;
			}
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Verify it */
		if (ask && !only_info)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
(void) strnfmt(tmp_val, 78, "%s���g���܂����H ", power_desc[i].name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", power_desc[i].name);
#endif


			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	/* Abort if needed */
	if (!flag)
	{
		energy_use = 0;
		return;
	}
#ifdef ALLOW_REPEAT
	if(!only_info) repeat_push(i);
	} /*if (!repeat_pull(&i) || ...)*/
#endif /* ALLOW_REPEAT */

	if(only_info)
	{
		int j, line;
		char temp[62*5];
		screen_save();
		Term_erase(12, 21, 255);
		Term_erase(12, 20, 255);
		Term_erase(12, 19, 255);
		Term_erase(12, 18, 255);
		Term_erase(12, 17, 255);
		Term_erase(12, 16, 255);
		roff_to_buf(power_desc[i].info, 62, temp, sizeof(temp));
		for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
		prt("�����L�[�������ĉ������B",0,0);
		(void)inkey();
		screen_load();


		energy_use = 0;
		return;
	}



	/*:::��������*/
	switch (racial_aux(&power_desc[i]))
	{
	case 1:
		if (power_desc[i].number < 0)
			cast = cmd_racial_power_aux(power_desc[i].number);
		else
			cast = mutation_power_aux(power_desc[i].number);
		break;
	case 0:
		cast = FALSE;
		break;
	case -1:
		cast = TRUE;
		break;
	}

	if (cast)
	{
		if (racial_cost)
		{
			int actual_racial_cost = racial_cost / 2 + randint1(racial_cost / 2);

			/* If mana is not enough, player consumes hit point! */
			if (p_ptr->csp < actual_racial_cost)
			{
				actual_racial_cost -= p_ptr->csp;
				p_ptr->csp = 0;
#ifdef JP
				take_hit(DAMAGE_USELIFE, actual_racial_cost, "�ߓx�̏W��", -1);
#else
				take_hit(DAMAGE_USELIFE, actual_racial_cost, "concentrating too hard", -1);
#endif
			}
			else p_ptr->csp -= actual_racial_cost;

			/* Redraw mana and hp */
			p_ptr->redraw |= (PR_HP | PR_MANA);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER | PW_SPELL);
		}
	}
	else energy_use = 0;

	/* Success */
	return;
}
