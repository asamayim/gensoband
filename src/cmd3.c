/* File: cmd3.c */

/* Purpose: Inventory commands */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"



/*
 * Display inventory
 */
/*:::�U�b�N�̒���\��*/
void do_cmd_inven(void)
{
	char out_val[160];


	/* Note that we are in "inventory" mode */
	command_wrk = FALSE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "inventory" mode */
	if (easy_floor) command_wrk = (USE_INVEN);

#endif /* ALLOW_EASY_FLOOR */

	/* Save screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the inventory */
	(void)show_inven(0);

	/* Hack -- hide empty slots */
	item_tester_full = FALSE;

#ifdef JP
	sprintf(out_val, "�������F ���v %3d.%1d kg (���E��%ld%%) �R�}���h: ",
	    (int)lbtokg1(p_ptr->total_weight) , (int)lbtokg2(p_ptr->total_weight) ,
	    (long int)((p_ptr->total_weight * 100) / weight_limit()));
#else
	sprintf(out_val, "Inventory: carrying %d.%d pounds (%ld%% of capacity). Command: ",
	    (int)(p_ptr->total_weight / 10), (int)(p_ptr->total_weight % 10),
	    (p_ptr->total_weight * 100) / weight_limit());
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Load screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Hack -- Use "display" mode */
		command_see = TRUE;
	}
}


/*
 * Display equipment
 */
/*:::�����i�ꗗ��\�����R�}���h��҂�*/
void do_cmd_equip(void)
{
	char out_val[160];


	/* Note that we are in "equipment" mode */
	command_wrk = TRUE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "equipment" mode */
	if (easy_floor) command_wrk = (USE_EQUIP);

#endif /* ALLOW_EASY_FLOOR  */

	/* Save the screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the equipment */
	(void)show_equip(0);

	/* Hack -- undo the hack above */
	item_tester_full = FALSE;

	/* Build a prompt */
#ifdef JP
	sprintf(out_val, "�����F ���v %3d.%1d kg (���E��%ld%%) �R�}���h: ",
	    (int)lbtokg1(p_ptr->total_weight) , (int)lbtokg2(p_ptr->total_weight) ,
	    (long int)((p_ptr->total_weight * 100) / weight_limit()));
#else
	sprintf(out_val, "Equipment: carrying %d.%d pounds (%ld%% of capacity). Command: ",
	    (int)(p_ptr->total_weight / 10), (int)(p_ptr->total_weight % 10),
	    (long int)((p_ptr->total_weight * 100) / weight_limit()));
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Restore the screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Enter "display" mode */
		command_see = TRUE;
	}
}


/*
 * The "wearable" tester
 */
/*:::do_cmd_wield()����g����B�����\�ȃA�C�e����I�ʂ���B*/
///item �������ꏈ����TVAL
///sys ���������E��푰�͂����ɏ�������悤���H
///mod150403 static�łȂ�����
bool item_tester_hook_wear(object_type *o_ptr)
{
	///del131223 �����폜
	//if ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ABUNAI_MIZUGI))
	//	if (p_ptr->psex == SEX_MALE) return FALSE;

	/* Check for a usable slot */
	/*:::�����i�łȂ����̂�-1��Ԃ��֐�*/
	if (wield_slot(o_ptr) >= INVEN_RARM) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}

///item TVAL �����ύX�@��Ɏ��\����������̂����ׂĎw��
///mod140309 �ˌ�������Ώۂɂ���
///mod151214 �o�O�C���@MAGICWEAPON�ǉ�
static bool item_tester_hook_mochikae(object_type *o_ptr)
{
	///mod151202 �O�d���p���ꏈ�� ��������Ȃ�
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		if (((o_ptr->tval >= TV_KNIFE) && (o_ptr->tval <= TV_POLEARM)) ||
		    (o_ptr->tval == TV_CAPTURE) ||
		    (o_ptr->tval == TV_MAGICWEAPON) ||
		    (o_ptr->tval == TV_MAGICITEM) ||
		    (o_ptr->tval == TV_OTHERWEAPON)) return (TRUE);
		if(object_is_shooting_weapon(o_ptr)) return TRUE;
		return (FALSE);
	}

	/* Check for a usable slot */
	if (((o_ptr->tval >= TV_KNIFE) && (o_ptr->tval <= TV_POLEARM)) ||
	    (o_ptr->tval == TV_SHIELD) || (o_ptr->tval == TV_CAPTURE) ||
	    (o_ptr->tval == TV_MAGICITEM) ||
	    (o_ptr->tval == TV_MAGICWEAPON) ||
	    (o_ptr->tval == TV_OTHERWEAPON)) return (TRUE);
	if(object_is_shooting_weapon(o_ptr)) return TRUE;
	/* Assume not wearable */
	return (FALSE);
}

#if 0
v1.1.93 ����킵�������������̊֐������łɂ���̂ō폜
static bool item_tester_hook_melee_weapon(object_type *o_ptr)
{
	/* Check for a usable slot */
	if ((o_ptr->tval >= TV_KNIFE) && (o_ptr->tval <= TV_POLEARM)
		|| (o_ptr->tval == TV_OTHERWEAPON))return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}
#endif


//���������Ȃ��قǗD�悵��INVEN_RARM�ɓ���B�N���X�{�E�Əe���u�ʏ�E��Ɏ����ߐڕ���ƈꏏ�Ɏ������獶��ɂȂ�v�Ȃ�ĕςȎd�l�ɂ��������Ŕ��肪�ʓ|�ɂȂ��č��ɂȂ��č����
int check_priority_inven_rarm(object_type *o_ptr)
{
	switch(o_ptr->tval)
	{
	case TV_BOW:
		return 4;
	case TV_CAPTURE:
	case TV_MAGICITEM:
	case TV_SHIELD:
		return 3;
	case TV_CROSSBOW:
	case TV_GUN:
		return 2;
	default:
		return 1;
	}

}


bool select_ring_slot = FALSE;

/*
 * Wield or wear a single item from the pack or floor
 */
/*:::�����ύX*/
///item TVAL �����ύX
///mod131223 tval
//mod160515 mochikae�I�v�V�����ǉ��@���葕���i�̂�
void do_cmd_wield(bool mochikae)
{
	int item, slot;
	bool asia = FALSE;

	object_type forge,forge2;
	object_type *q_ptr;

	object_type *o_ptr;
	bool moon_vault_trap = FALSE;

	bool flag_quickdraw = FALSE;

	cptr act;

	char o_name[MAX_NLEN];

	cptr q, s;

	/*:::�E�肪���ō��肪����ȂǁA�����ւ�������Ƃ��̃t���O*/
	int need_switch_wielding = 0;

	/*:::�E��ɃN���X�{�E���������肪�|�ŗ����͎����Ȃ����ȂǁA����X���b�g�̑������O���Ƃ��̃t���O��ǉ�*/
	int need_takeoff = 0;
	object_type *takeoff_o_ptr;
	takeoff_o_ptr = &forge2;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD || p_ptr->clawextend)
	{
		msg_print("���̎p�ł͑�����ύX�ł��Ȃ��B");
		return ;
	}

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Restrict the choices */
	if(mochikae)
		item_tester_hook = item_tester_hook_mochikae;
	else
		item_tester_hook = item_tester_hook_wear;

	/* Get an item */
#ifdef JP
	q = "�ǂ�𑕔����܂���? ";
	s = "�����\�ȃA�C�e�����Ȃ��B";
#else
	q = "Wear/Wield which item? ";
	s = "You have nothing you can wear or wield.";
#endif
	/*:::��������A�C�e����I�ԁB�ꗗ�ւ̕\����item_tester_hook�Ńt�B���^�����*/
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


	/* Check the slot */
	slot = wield_slot(o_ptr);

	if(o_ptr->tval == TV_GUN && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW)) 
		flag_quickdraw = TRUE;

	///mod160515 �u�E��ƍ���ɕʁX�̕������Ƃ���Ɋm�F����v�I�v�V������ǉ�
	if((always_check_arms) && (slot == INVEN_RARM || slot == INVEN_LARM))
	{
		int old_slot = slot;
		if(!inventory[INVEN_RARM].k_idx && !inventory[INVEN_LARM].k_idx)
		{	//���肪��̎������������̂܂ܑ���
			;
		}
		else if(inventory[INVEN_RARM].k_idx && inventory[INVEN_LARM].k_idx)
		{	//���łɗ���ɕ��������Ă���Ƃ��A��ɓ���ւ��m�F
			item_tester_hook = item_tester_hook_mochikae;
			q = "�ǂ���̑����i�Ǝ��ւ��܂���?";
			s = "ERROR:do_cmd_wield()�̏�������������(1)";
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;

			//���ւ������ʁA���ʍ���Ɏ������E��Ɏ����ĂĉE��Ɏ����̂�����Ɏ���ԂɂȂ����獶�E�̑��������ւ���
			if(slot == INVEN_RARM && check_priority_inven_rarm(o_ptr) > check_priority_inven_rarm(&inventory[INVEN_LARM]))
				need_switch_wielding = INVEN_LARM;
			else if(slot == INVEN_LARM && check_priority_inven_rarm(o_ptr) < check_priority_inven_rarm(&inventory[INVEN_RARM]))
				need_switch_wielding = INVEN_RARM;
		}
		else
		{	//���E�ǂ��炩�̃X���b�g���ӂ������Ă���Ƃ��A������O�����ǂ����m�F����
			if (get_check("����Ɏ����Ă��镨�ƈꏏ�ɑ������܂����H"))
			{	
//					if(inventory[INVEN_LARM].k_idx && check_priority_inven_rarm(o_ptr) > check_priority_inven_rarm(&inventory[INVEN_LARM]))
//						need_switch_wielding = INVEN_LARM;
//					else if(inventory[INVEN_RARM].k_idx && check_priority_inven_rarm(o_ptr) < check_priority_inven_rarm(&inventory[INVEN_RARM]))
//						need_switch_wielding = INVEN_RARM;

				//v1.1.15b ���������ɋ|�����Ƃ��Ƃ����������O���Ă��܂��̂ŏC��
				if(inventory[slot].k_idx)
				{
					if(slot == INVEN_RARM && check_priority_inven_rarm(o_ptr) < check_priority_inven_rarm(&inventory[INVEN_RARM]))
					{
						slot = INVEN_LARM;
						need_switch_wielding = INVEN_RARM;
					}
					if(slot == INVEN_LARM && check_priority_inven_rarm(o_ptr) > check_priority_inven_rarm(&inventory[INVEN_LARM]))
					{
						slot = INVEN_RARM;
						need_switch_wielding = INVEN_LARM;
					}

				}
			}
			else
			{	//�O������
				if(inventory[INVEN_RARM].k_idx)
				{
					need_takeoff = INVEN_RARM;
					if(check_priority_inven_rarm(o_ptr) < 3) slot = INVEN_RARM;
				}
				else if(inventory[INVEN_LARM].k_idx)
				{
					need_takeoff = INVEN_LARM;
					if(check_priority_inven_rarm(o_ptr) > 2) slot = INVEN_LARM;
				}
			}
		}
	}
	else
	/*:::����E���E�����O�͂ǂ��ɑ������邩���܂�Ȃ��̂őI�������A�C�e���������ɑ������邩���߂�slot�Ɋi�[*/
	switch (o_ptr->tval)
	{
	/* Shields and some misc. items */
	/*:::���A�����X�^�[�{�[��*/
		///item TVAL
	case TV_CAPTURE:
	case TV_MAGICITEM:
	case TV_SHIELD:
	//case TV_CARD:
		/* Dual wielding */
		/*:::�񓁗��̏ꍇ�ǂ���̕���Ǝ��ւ��邩�I��*/
		if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
		{
			/* Restrict the choices */
			/*:::�I���ꗗ��(�Őj�ȊO��)���킵���o���Ȃ����߂�hook*/
			/*:::�E�E�Ȃ񂾂��A�Őj�񓁗��ɂ��ĂĂ������Ɨ������ɏo��B�ǂ��������Ƃ��B*/
			//��cmd3.c��bldg.c�ɓ����̊֐�������A�����͓̂Őj���I��Ώ�

			//v1.1.93 �����Ǝ���melee_weapon����֐����폜���ĕ��ʂ̂ɕς����BTV_MAGICWEAPON���V���ɑΏۂɂȂ�
			//item_tester_hook = item_tester_hook_melee_weapon;
			item_tester_hook = object_is_melee_weapon;

			item_tester_no_ryoute = TRUE;

			/* Choose a weapon from the equipment only */
#ifdef JP
			q = "�ǂ���̕���Ǝ��ւ��܂���?";
			s = "�����ƁB";
#else
			q = "Replace which weapon? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM) need_switch_wielding = INVEN_LARM;
		}
		/*:::����ɕ���������Ă���ꍇ�E��ɏ��Ƃ������H���ۂɂ͂��ĂȂ����B�����ɂ͗���̂��H*/
		else if (buki_motteruka(INVEN_LARM)) slot = INVEN_RARM;

		///mod140309 �|�g�p�~�֘A
		/*:::���E�ǂ��炩�̎�Ɏˌ�����������Ă��ĐV���ɏ��Ȃǂ𑕔�����ꍇ�̏�����ǉ�*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::�E��Ɏˌ�����������A����ɉ��������ĂȂ��Ƃ��i�N���X�{�E���e�𗼎莝���A�������̓N���X�{�E���e�Ǝ�j�j�́A���̂܂܏��Ȃǂ�����ɑ���*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				;
			}
			/*:::����Ɏˌ�����������A�E��ɉ��������ĂȂ��Ƃ��i�|�𗼎莝���A�������͋|�Ǝ�j�j�́A���Ȃǂ��E��ɑ���*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				slot = INVEN_RARM;
			}
			/*:::����Ɏˌ�������܂މ����������Ă���Ƃ��@�܂��͂ǂ�����O�����I��*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
				q = "�ǂ���̑����i�Ǝ��ւ��܂���?";
				s = "�����ƁB";
#else
				q = "Equip which hand? ";
				s = "Oops.";
#endif
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::�E��̑����Ǝ��ւ��A����Ɏc�����̂��ߐڕ���E�e�E�N���X�{�E�Ȃ玝���ւ���*/
					if(object_is_melee_weapon(&inventory[INVEN_LARM]) || inventory[INVEN_LARM].tval == TV_CROSSBOW || inventory[INVEN_LARM].tval == TV_GUN)
					{
						need_switch_wielding = INVEN_LARM;
					}
					/*:::����ɂ���̂��|�A���ȂǂȂ炻�̂܂ܑ�������*/
				}
				else
				{
					/*:::����̑����Ǝ��ւ��A�E��Ɏc�����̂��|�Ȃ玝���ւ���*/
					if(inventory[INVEN_RARM].tval == TV_BOW)
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		/* Both arms are already used by non-weapon */
		/*:::���Ȃǂ𗼎�Ɏ����Ă���ꍇ�ǂ��炩�Ɠ���ւ���*/
		else if (inventory[INVEN_RARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_RARM]) &&
		         inventory[INVEN_LARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_LARM]))
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "�ǂ���̎�ɑ������܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
		}
		break;

	/* Melee weapons */
	///item TVAL
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_OTHERWEAPON:
	case TV_MAGICWEAPON:


		///mod140309 �|�g�p�~�֘A
		/*:::���E�ǂ��炩�̎�Ɏˌ�����������Ă��ĐV���ɋߐڕ���𑕔�����ꍇ�̏�����ǉ�*/
		if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::�E��Ɏˌ�����������A����ɉ��������ĂȂ��Ƃ��i�N���X�{�E���e�𗼎莝���A�������̓N���X�{�E���e�Ǝ�j�j�́A�ˌ����������Ɏ����ւ�*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				slot = INVEN_LARM;
				need_switch_wielding = INVEN_RARM;
			}
			/*:::����Ɏˌ�����������A�E��ɉ��������ĂȂ��Ƃ��i�|�𗼎莝���A�������͋|�Ǝ�j�j�́A�V���ȋߐڕ�������̂܂܉E��ɑ���*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				;
			}
			/*:::����Ɏˌ�������܂މ����������Ă���Ƃ��@�܂��͂ǂ�����O�����I��*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
				q = "�ǂ���̑����i�Ǝ��ւ��܂���?";
				s = "�����ƁB";
#else
				q = "Equip which hand? ";
				s = "Oops.";
#endif
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::�E��̑����Ǝ��ւ����ꍇ�A����Ɏ����Ă���̂����ł��낤�Ƃ��̂܂�*/
					;
				}
				else
				{
					/*:::����̑����Ǝ��ւ����ꍇ�A�E��Ɏ����Ă���̂��ߐڕ���ȊO�Ȃ玝���ւ���*/
					if(!object_is_melee_weapon(&inventory[INVEN_RARM]))
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		/* Asking for dual wielding */
		/*:::�ߐڕ���𑕔����悤�Ƃ���slot��LARM�ɂȂ�͉̂E��ɂ̂݉����𑕔����Ă���Ƃ��@*/
		/*:::����L���v�`���[�����������Ă���Ƃ��͍���Ɏ����Ă���̂ŉE��ɂ̂݉��������Ă�Ȃ炻��͏�ɋߐڕ���*/
		/*:::���|�g�p�~�ł����Ƃ�����Ȃ��Ȃ��������̏����͏�ōς܂���*/
		else if (slot == INVEN_LARM)
		{
#ifdef JP
			if (!get_check("�񓁗��Ő킢�܂����H")) slot = INVEN_RARM;
#else
			if (!get_check("Dual wielding? ")) slot = INVEN_RARM;
#endif
		}

		else if (!inventory[INVEN_RARM].k_idx && buki_motteruka(INVEN_LARM))
		{
#ifdef JP
			if (!get_check("�񓁗��Ő킢�܂����H")) slot = INVEN_LARM;
#else
			if (!get_check("Dual wielding? ")) slot = INVEN_LARM;
#endif
		}

		/* Both arms are already used */
		else if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "�ǂ���̎�ɑ������܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if ((slot == INVEN_LARM) && !buki_motteruka(INVEN_RARM))
				need_switch_wielding = INVEN_RARM;
		}
		break;


///mod140309 �|�g�p�~�����֘A
	/*:::�|��V���ɑ����@slot�͏��INVEN_LARM*/
	case TV_BOW:
		/*:::����ɂ��ꂼ�ꉽ���������Ă�ꍇ�A�ǂ�����O�����I��Ŏc�������͏�ɉE��Ŏ���*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "�ǂ���̑����i�Ǝ����ւ��܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM)	need_switch_wielding = INVEN_LARM;
		}
		/*:::�ˌ�����̂ݎ����Ă���Ƃ��A����Ɏ����m�F��NO�Ȃ�V�������������@�t���O�ǉ�*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (!get_check("����ɂ��ꂼ��ˌ�����������܂����H")) need_takeoff = INVEN_RARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (get_check("����ɂ��ꂼ��ˌ�����������܂����H")) slot = INVEN_RARM;
		}
		/*:::�ߐڕ���̂ݎ����Ă���Ƃ��i�K���E��̂͂��j�A�ǉ������Ȃ��@�|�͍���Ŏ���*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			;
		}
		/*:::����d���{�̂ݎ����Ă���Ƃ��i�K������̂͂��j�A�|������Ɏ����Ă����͉E���*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			slot = INVEN_RARM;
			need_switch_wielding = INVEN_LARM;
		}
		/*:::�����������Ă��Ȃ��Ƃ��A�|�͍����*/
		else
		{
			;
		}
		break;

	/*:::�e��N���X�{�E��V���ɑ����@slot�͉E��ɉ��������Ă���INVEN_LARM*/
	case TV_CROSSBOW:
	case TV_GUN:
		/*:::����ɂ��ꂼ�ꉽ���������Ă�ꍇ�A�܂��ǂ�����O�����I��*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "�ǂ���̑����i�Ǝ����ւ��܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			/*:::�c�����̂��ߐڕ���Ȃ炻������E��Ɏ����e���N���X�{�E�͍����*/
			if (slot == INVEN_RARM && object_is_melee_weapon(&inventory[INVEN_LARM]))
			{
				need_switch_wielding = INVEN_LARM;
			}
			//�c�����̂��e�̏ꍇ�����ւ��Ȃ�
			else if(slot == INVEN_LARM && inventory[INVEN_RARM].tval == TV_GUN ||
				slot == INVEN_RARM && inventory[INVEN_LARM].tval == TV_GUN )
			{
				; //�������Ȃ�
			}
			/*:::�c�����̂�����d���{�Ȃ炻�������Ɏ���*/
			else if (slot == INVEN_LARM && !object_is_melee_weapon(&inventory[INVEN_RARM]))
			{
				need_switch_wielding = INVEN_RARM;
			}
		}
		/*:::�ˌ�����̂ݎ����Ă���Ƃ��A����Ɏ����m�F��NO�Ȃ�V�������������@�t���O�ǉ�*/
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (!get_check("����ɂ��ꂼ��ˌ�����������܂����H")) need_takeoff = INVEN_LARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (get_check("����ɂ��ꂼ��ˌ�����������܂����H")) slot = INVEN_LARM;
			else slot = INVEN_RARM;
		}
		/*:::�ߐڕ���̂ݎ����Ă���Ƃ��i�K���E��̂͂��j�A�e�ƃN���X�{�E�͍���Ŏ���*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			slot = INVEN_LARM;
		}
		/*:::����d���{�̂ݎ����Ă���Ƃ��i�K������̂͂��j�A�ǉ������Ȃ��@�e�ƃN���X�{�E�͉E���*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			;
		}
		/*:::�����������Ă��Ȃ��Ƃ��A�e�ƃN���X�{�E�͉E���*/
		else
		{
			;
		}
		break;

	/* Rings */
	case TV_RING:
		/* Choose a ring slot */
		if (inventory[INVEN_LEFT].k_idx && inventory[INVEN_RIGHT].k_idx)
		{
#ifdef JP
			q = "�ǂ���̎w�ւƎ��ւ��܂���?";
#else
			q = "Replace which ring? ";
#endif
		}
		else
		{
#ifdef JP
			q = "�ǂ���̎�ɑ������܂���?";
#else
			q = "Equip which hand? ";
#endif
		}

#ifdef JP
		s = "�����ƁB";
#else
		s = "Oops.";
#endif

		/* Restrict the choices */
		select_ring_slot = TRUE;
		item_tester_no_ryoute = TRUE;

		if (!get_item(&slot, q, s, (USE_EQUIP)))
		{
			select_ring_slot = FALSE;
			return;
		}
		select_ring_slot = FALSE;
		break;
	}
	/*:::����E���E�����O�̃X���b�g���菈���I��*/

	///mod140122 �������������ǉ�
	if(!(wield_check(slot,item))) return;

	//v1.1.43 need_takeoff�̑����O�����������������Ă����̂Œǉ�
	if (need_takeoff && !(wield_check(need_takeoff, INVEN_PACK))) return;

	/* Prevent wielding into a cursed slot */
	if (object_is_cursed(&inventory[slot]))
	{
		/* Describe it */
		object_desc(o_name, &inventory[slot], (OD_OMIT_PREFIX | OD_NAME_ONLY));

		/* Message */
#ifdef JP
		msg_format("%s%s�͎���Ă���悤���B",
			   describe_use(slot) , o_name );
#else
		msg_format("The %s you are %s appears to be cursed.",
			   o_name, describe_use(slot));
#endif

		/* Cancel the command */
		return;
	}

	///mod160807 need_takeoff���g���Ƃ��̎􂢏���������Ȃ������̂Œǉ�
	if (need_takeoff && object_is_cursed(&inventory[need_takeoff]))
	{
		object_desc(o_name, &inventory[need_takeoff], (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format("%s%s�͎���Ă���悤���B",  describe_use(need_takeoff) , o_name );
		return;
	}



	if (confirm_wear &&
		((object_is_cursed(o_ptr) && object_is_known(o_ptr)) ||
		((o_ptr->ident & IDENT_SENSE) &&
			(FEEL_BROKEN <= o_ptr->feeling) && (o_ptr->feeling <= FEEL_CURSED))))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "�{����%s{����Ă���}���g���܂����H", o_name);
#else
		sprintf(dummy, "Really use the %s {cursed}? ", o_name);
#endif

		if (!get_check(dummy)) return;
	}
	///item system �Ή��ʂ̎푰�ϗe �A���h���C�h�i�d���l�`�j���ψق���悤�ɂ��悤�Ǝv�������悭�l������o���l�v�Z����ςȂ��ƂɂȂ�
	if ((o_ptr->name1 == ART_STONEMASK) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		char dummy[MAX_NLEN+80];

		int j;

		for (j = 0; j < INVEN_PACK; j++) if(inventory[j].tval == TV_SOUVENIR && inventory[j].sval == SV_SOUVENIR_ASIA) asia = TRUE;

		if(asia)
		{
			msg_print("���ʂ̊z�Ɂu�G�C�W���̐Ԑ΁v���҂�������܂��Ă���B");
			msg_print(NULL);
			sprintf(dummy, "���̂܂ܑ������Ă݂܂����H");
			if (!get_check(dummy)) return;
		}
		else if(p_ptr->prace != RACE_VAMPIRE)
		{
			/* Describe it */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			sprintf(dummy, "%s�𑕔�����Ƌz���S�ɂȂ�܂��B��낵���ł����H", o_name);
			if (!get_check(dummy)) return;
		}
	}
	///mod140115 �����|�̎푰�ϗe �d���l�`�͕ϐg���Ȃ�
	if ((o_ptr->name1 == ART_MENPO) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "%s�𑕔�����ƃj���W���ɂȂ�܂��B��낵���ł����H", o_name);
#endif

		if (!get_check(dummy)) return;
	}

	//��\��o�����t���f
	stop_tsukumo_music();

	//v1.1.95 �ߋ���Z���f
	if (p_ptr->pclass == CLASS_IKU && p_ptr->tim_general[0]) set_tim_general(0, TRUE, 0, 0);

	if (need_switch_wielding && !object_is_cursed(&inventory[need_switch_wielding]))
	{
		object_type *slot_o_ptr = &inventory[slot];
		object_type *switch_o_ptr = &inventory[need_switch_wielding];
		object_type object_tmp;
		object_type *otmp_ptr = &object_tmp;
		char switch_name[MAX_NLEN];

		object_desc(switch_name, switch_o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		object_copy(otmp_ptr, switch_o_ptr);
		object_copy(switch_o_ptr, slot_o_ptr);
		object_copy(slot_o_ptr, otmp_ptr);
#ifdef JP
		msg_format("%s��%s�ɍ\���Ȃ������B", switch_name, (slot == INVEN_RARM) ? (left_hander ? "����" : "�E��") : (left_hander ? "�E��" : "����"));
#else
		msg_format("You wield %s at %s hand.", switch_name, (slot == INVEN_RARM) ? (left_hander ? "left" : "right") : (left_hander ? "right" : "left"));
#endif

		slot = need_switch_wielding;
	}



	/*:::������^�N�G�X�g�ŊY���A�C�e�����E�������`�F�b�N*/
	/*:::�󕨌ɂ̃^�[�Q�b�g�������璼�ڑ��������Ƃ��̂��߂̏����Ǝv����*/
	check_find_art_quest_completion(o_ptr);


	//���s���ۓW�p����A�C�e��
	if(	p_ptr->inside_quest == QUEST_MOON_VAULT && o_ptr->marked & OM_SPECIAL_FLAG)
	{
		activate_moon_vault_trap();
	}



	///seikaku
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		identify_item(o_ptr);

		/* Auto-inscription */
		autopick_alter_item(item, FALSE);
	}

	/* Take a turn */
	if(flag_quickdraw)
	{
		msg_print("���Ȃ��͑f�����e���\�����B");
		energy_use = 50;
	}
	else
		energy_use = 100;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &inventory[slot];

	/* Take off existing item */
	if (o_ptr->k_idx)
	{
		/* Take off existing item */
		(void)inven_takeoff(slot, 255);
	}
	else if(need_takeoff)
	{
		(void)inven_takeoff(need_takeoff, 255);
	}

	/* Wear the new stuff */
	object_copy(o_ptr, q_ptr);

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Increase the weight */
	p_ptr->total_weight += q_ptr->weight;

	/* Increment the equip counter by hand */
	equip_cnt++;

#ifdef JP
#define STR_WIELD_RARM "%s(%c)���E��ɑ��������B"
#define STR_WIELD_LARM "%s(%c)������ɑ��������B"
#define STR_WIELD_ARMS "%s(%c)�𗼎�ō\�����B"
#else
#define STR_WIELD_RARM "You are wielding %s (%c) in your right hand."
#define STR_WIELD_LARM "You are wielding %s (%c) in your left hand."
#define STR_WIELD_ARMS "You are wielding %s (%c) with both hands."
#endif

	/* Where is the item now */
	switch (slot)
	{
	case INVEN_RARM:
		if (object_allow_two_hands_wielding(o_ptr) && (empty_hands(FALSE) == EMPTY_HAND_LARM) && CAN_TWO_HANDS_WIELDING())
			act = STR_WIELD_ARMS;
		else
			act = (left_hander ? STR_WIELD_LARM : STR_WIELD_RARM);
		break;

	case INVEN_LARM:
		if (object_allow_two_hands_wielding(o_ptr) && (empty_hands(FALSE) == EMPTY_HAND_RARM) && CAN_TWO_HANDS_WIELDING())
			act = STR_WIELD_ARMS;
		else
			act = (left_hander ? STR_WIELD_RARM : STR_WIELD_LARM);
		break;

	case INVEN_RIBBON:
#ifdef JP
		act = "%s(%c)�����񂾁B";
#else
		act = "You are shooting with %s (%c).";
#endif
		break;

	case INVEN_LITE:
#ifdef JP
		act = "%s(%c)�������ɂ����B";
#else
		act = "Your light source is %s (%c).";
#endif
		break;

	default:
#ifdef JP
		act = "%s(%c)�𑕔������B";
#else
		act = "You are wearing %s (%c).";
#endif
		break;
	}

	/* Describe the result */
	object_desc(o_name, o_ptr, 0);

	/* Message */
	msg_format(act, o_name, index_to_label(slot));


	/* Cursed! */
	if (object_is_cursed(o_ptr))
	{
		/* Warn the player */
#ifdef JP
		msg_print("����I �����܂����₽���I");
#else
		msg_print("Oops! It feels deathly cold!");
#endif


		//chg_virtue(V_HARMONY, -1);

		/* Note the curse */
		o_ptr->ident |= (IDENT_SENSE);
	}

	///item race �Ή��ʂ̎푰�ϗe
	/* The Stone Mask make the player turn into a vampire! */
	if ((o_ptr->name1 == ART_STONEMASK) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		/* Turn into a vampire */
		if(asia) change_race(RACE_ULTIMATE, "");
		else if(p_ptr->prace != RACE_VAMPIRE) change_race(RACE_VAMPIRE, "");
	}
	///mod140115 �����|�̎푰�ϗe�@�d���l�`�͕ϐg���Ȃ�
	if (o_ptr->name1 == ART_MENPO && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		/*:::���������������������*/
		sprintf(p_ptr->history[0], "���Ȃ���%s�Ƃ��ĕ����ɐ����Ă����B",rp_ptr->title);
		sprintf(p_ptr->history[1], "��������������Ȃ��ɕ�������̃j���W���E�\�E�����߈˂����I");
		if(one_in_(6))
			sprintf(p_ptr->history[2], "���Ȃ��̖ړI�͗B��A���ΐ��N�ւ̕��Q���I");
		else if(one_in_(5))
			sprintf(p_ptr->history[2], "���Ȃ��̖ړI�͗B��A���̏��_�b�M�ւ̕��Q���I");
		else if(one_in_(4))
			sprintf(p_ptr->history[2], "���Ȃ��̖ړI�͗B��A�w���ׂ̃T�[�y���g�x�ւ̕��Q���I");
		else if(one_in_(3))
			sprintf(p_ptr->history[2], "���Ȃ��̖ړI�͗B��A�S�Ẵj���W���̖��E���I");
		else if(one_in_(2))
			sprintf(p_ptr->history[2], "���Ȃ��̖ړI�͗B��A������s�����ҒB�ւ̕��Q���I");
		else
			sprintf(p_ptr->history[2], "���Ȃ��̖ړI�͗B��A�S�A���o�[�̉����ւ̕��Q���I");
		sprintf(p_ptr->history[3], "����A%s�A����I",player_name);
		/* Turn into a NINJA */
		change_race(RACE_NINJA, "");
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA | PU_HP);

	p_ptr->redraw |= (PR_EQUIPPY | PR_HP | PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp();
}


/*:::���̎O�d����p�̑����ύX*/
void do_cmd_wield_3_fairies(void)
{
	int item, slot;
	bool asia = FALSE;

	object_type forge,forge2;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr act;

	char o_name[MAX_NLEN];

	cptr q, s;

	/*:::�E�肪���ō��肪����ȂǁA�����ւ�������Ƃ��̃t���O*/
	int need_switch_wielding = 0;

	/*:::�E��ɃN���X�{�E���������肪�|�ŗ����͎����Ȃ����ȂǁA����X���b�g�̑������O���Ƃ��̃t���O��ǉ�*/
	int need_takeoff = 0;
	object_type *takeoff_o_ptr;
	takeoff_o_ptr = &forge2;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD || p_ptr->clawextend)
	{
		msg_print("���̎p�ł͑�����ύX�ł��Ȃ��B");
		return ;
	}

	/* Restrict the choices */
	item_tester_hook = item_tester_hook_wear;

	/* Get an item */
	q = "�ǂ�𑕔����܂���? ";
	s = "�����\�ȃA�C�e�����Ȃ��B";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;
	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];

	/* Check the slot */
	slot = wield_slot_3_fairies(o_ptr);

	//�ǂ̃X���b�g�ɑ������邩�I�ԕK�v������Ƃ�
	switch (o_ptr->tval)
	{
	//�d���{(���͕ʕ���)
	case TV_CAPTURE:
	case TV_MAGICITEM:
		/*:::�񓁗��̏ꍇ�ǂ���̕���Ǝ��ւ��邩�I��*/
		if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
		{
			/* Restrict the choices */

			//v1.1.93 �����Ǝ���melee_weapon����֐����폜���ĕ��ʂ̂ɕς����BTV_MAGICWEAPON���V���ɑΏۂɂȂ�
			//item_tester_hook = item_tester_hook_melee_weapon;
			item_tester_hook = object_is_melee_weapon;

			item_tester_no_ryoute = TRUE;
			q = "�ǂ���̕���Ǝ��ւ��܂���?";
			s = "�����ƁB";

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM) need_switch_wielding = INVEN_LARM;
		}
		else if (buki_motteruka(INVEN_LARM)) slot = INVEN_RARM;

		/*:::���E�ǂ��炩�̎�Ɏˌ�����������Ă��ĐV���ɏ��Ȃǂ𑕔�����ꍇ�̏�����ǉ�*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::�E��Ɏˌ�����������A����ɉ��������ĂȂ��Ƃ��i�N���X�{�E���e�𗼎莝���A�������̓N���X�{�E���e�Ǝ�j�j�́A���̂܂܏��Ȃǂ�����ɑ���*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				;
			}
			/*:::����Ɏˌ�����������A�E��ɉ��������ĂȂ��Ƃ��i�|�𗼎莝���A�������͋|�Ǝ�j�j�́A���Ȃǂ��E��ɑ���*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				slot = INVEN_RARM;
			}
			/*:::����Ɏˌ�������܂މ����������Ă���Ƃ��@�܂��͂ǂ�����O�����I��*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
				q = "�ǂ���̑����i�Ǝ��ւ��܂���?";
				s = "�����ƁB";
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::�E��̑����Ǝ��ւ��A����Ɏc�����̂��ߐڕ���E�e�E�N���X�{�E�Ȃ玝���ւ���*/
					if(object_is_melee_weapon(&inventory[INVEN_LARM]) || inventory[INVEN_LARM].tval == TV_CROSSBOW || inventory[INVEN_LARM].tval == TV_GUN)
					{
						need_switch_wielding = INVEN_LARM;
					}
					/*:::����ɂ���̂��|�A���ȂǂȂ炻�̂܂ܑ�������*/
				}
				else
				{
					/*:::����̑����Ǝ��ւ��A�E��Ɏc�����̂��|�Ȃ玝���ւ���*/
					if(inventory[INVEN_RARM].tval == TV_BOW)
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		/* Both arms are already used by non-weapon */
		/*:::���Ȃǂ𗼎�Ɏ����Ă���ꍇ�ǂ��炩�Ɠ���ւ���*/
		else if (inventory[INVEN_RARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_RARM]) &&
		         inventory[INVEN_LARM].k_idx && !object_is_melee_weapon(&inventory[INVEN_LARM]))
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "�ǂ���̎�ɑ������܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
		}
		break;

	/* Melee weapons */
	///item TVAL
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_OTHERWEAPON:
	case TV_MAGICWEAPON:


		///mod140309 �|�g�p�~�֘A
		/*:::���E�ǂ��炩�̎�Ɏˌ�����������Ă��ĐV���ɋߐڕ���𑕔�����ꍇ�̏�����ǉ�*/
		if(object_is_shooting_weapon(&inventory[INVEN_RARM]) || object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			/*:::�E��Ɏˌ�����������A����ɉ��������ĂȂ��Ƃ��i�N���X�{�E���e�𗼎莝���A�������̓N���X�{�E���e�Ǝ�j�j�́A�ˌ����������Ɏ����ւ�*/
			if(object_is_shooting_weapon(&inventory[INVEN_RARM]) && !inventory[INVEN_LARM].k_idx)
			{
				slot = INVEN_LARM;
				need_switch_wielding = INVEN_RARM;
			}
			/*:::����Ɏˌ�����������A�E��ɉ��������ĂȂ��Ƃ��i�|�𗼎莝���A�������͋|�Ǝ�j�j�́A�V���ȋߐڕ�������̂܂܉E��ɑ���*/
			else if(object_is_shooting_weapon(&inventory[INVEN_LARM]) && !inventory[INVEN_RARM].k_idx)
			{
				;
			}
			/*:::����Ɏˌ�������܂މ����������Ă���Ƃ��@�܂��͂ǂ�����O�����I��*/
			else
			{
				item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
				q = "�ǂ���̑����i�Ǝ��ւ��܂���?";
				s = "�����ƁB";
#else
				q = "Equip which hand? ";
				s = "Oops.";
#endif
				if (!get_item(&slot, q, s, (USE_EQUIP))) return;

				if(slot == INVEN_RARM)
				{
					/*:::�E��̑����Ǝ��ւ����ꍇ�A����Ɏ����Ă���̂����ł��낤�Ƃ��̂܂�*/
					;
				}
				else
				{
					/*:::����̑����Ǝ��ւ����ꍇ�A�E��Ɏ����Ă���̂��ߐڕ���ȊO�Ȃ玝���ւ���*/
					if(!object_is_melee_weapon(&inventory[INVEN_RARM]))
					{
						need_switch_wielding = INVEN_RARM;
					}
				}

			}
		}

		//�񓁗��m�F�������폜

		/* Both arms are already used */
		else if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			q = "�ǂ���̎�ɑ������܂���?";
			s = "�����ƁB";

			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if ((slot == INVEN_LARM) && !buki_motteruka(INVEN_RARM))
				need_switch_wielding = INVEN_RARM;
		}
		break;


///mod140309 �|�g�p�~�����֘A
	/*:::�|��V���ɑ����@slot�͏��INVEN_LARM*/
	case TV_BOW:
		/*:::����ɂ��ꂼ�ꉽ���������Ă�ꍇ�A�ǂ�����O�����I��Ŏc�������͏�ɉE��Ŏ���*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "�ǂ���̑����i�Ǝ����ւ��܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_RARM)	need_switch_wielding = INVEN_LARM;
		}
		/*:::�ˌ�����̂ݎ����Ă���Ƃ��A����Ɏ����m�F��NO�Ȃ�V�������������@�t���O�ǉ�*/
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (!get_check("����ɂ��ꂼ��ˌ�����������܂����H")) need_takeoff = INVEN_RARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (get_check("����ɂ��ꂼ��ˌ�����������܂����H")) slot = INVEN_RARM;
		}
		/*:::�ߐڕ���̂ݎ����Ă���Ƃ��i�K���E��̂͂��j�A�ǉ������Ȃ��@�|�͍���Ŏ���*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			;
		}
		/*:::����d���{�̂ݎ����Ă���Ƃ��i�K������̂͂��j�A�|������Ɏ����Ă����͉E���*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			slot = INVEN_RARM;
			need_switch_wielding = INVEN_LARM;
		}
		/*:::�����������Ă��Ȃ��Ƃ��A�|�͍����*/
		else
		{
			;
		}
		break;

	/*:::�e��N���X�{�E��V���ɑ����@slot�͉E��ɉ��������Ă���INVEN_LARM*/
	case TV_CROSSBOW:
	case TV_GUN:
		/*:::����ɂ��ꂼ�ꉽ���������Ă�ꍇ�A�܂��ǂ�����O�����I��*/
		if (inventory[INVEN_LARM].k_idx && inventory[INVEN_RARM].k_idx)
		{
			item_tester_hook = item_tester_hook_mochikae;
#ifdef JP
			q = "�ǂ���̑����i�Ǝ����ւ��܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif
			if (!get_item(&slot, q, s, (USE_EQUIP))) return;
			/*:::�c�����̂��ߐڕ���Ȃ炻������E��Ɏ����e���N���X�{�E�͍����*/
			if (slot == INVEN_RARM && object_is_melee_weapon(&inventory[INVEN_LARM]))
			{
				need_switch_wielding = INVEN_LARM;
			}
			/*:::�c�����̂�����d���{�Ȃ炻�������Ɏ���*/
			if (slot == INVEN_LARM && !object_is_melee_weapon(&inventory[INVEN_RARM]))
			{
				need_switch_wielding = INVEN_RARM;
			}
		}
		/*:::�ˌ�����̂ݎ����Ă���Ƃ��A����Ɏ����m�F��NO�Ȃ�V�������������@�t���O�ǉ�*/
		else if(object_is_shooting_weapon(&inventory[INVEN_LARM]))
		{
			if (!get_check("����ɂ��ꂼ��ˌ�����������܂����H")) need_takeoff = INVEN_LARM;
		}
		else if(object_is_shooting_weapon(&inventory[INVEN_RARM]))
		{
			if (get_check("����ɂ��ꂼ��ˌ�����������܂����H")) slot = INVEN_LARM;
			else slot = INVEN_RARM;
		}
		/*:::�ߐڕ���̂ݎ����Ă���Ƃ��i�K���E��̂͂��j�A�e�ƃN���X�{�E�͍���Ŏ���*/
		else if(object_is_melee_weapon(&inventory[INVEN_RARM]))
		{
			slot = INVEN_LARM;
		}
		/*:::����d���{�̂ݎ����Ă���Ƃ��i�K������̂͂��j�A�ǉ������Ȃ��@�e�ƃN���X�{�E�͉E���*/
		else if(inventory[INVEN_LARM].k_idx)
		{
			;
		}
		/*:::�����������Ă��Ȃ��Ƃ��A�e�ƃN���X�{�E�͉E���*/
		else
		{
			;
		}
		break;

		case TV_RIBBON:		
		case TV_RING:
		case TV_AMULET:
		case TV_WHISTLE:
		case TV_DRAG_ARMOR:
		case TV_ARMOR:
		case TV_CLOTHES:
		case TV_CLOAK:
		case TV_HEAD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_SHIELD:

		//v1.1.68 �O�����̒ʏ퐫�i�ł͏��͂����ł̏����s�v
		if (!is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES) && o_ptr->tval == TV_SHIELD)
			break;

		//���ɑS�X���b�g���܂��Ă���Ƃ������ɓ���͂�
		if (slot == INVEN_FEET && inventory[INVEN_FEET].k_idx)
		{
			q = "�ǂ̑����i�Ǝ��ւ��܂���?";
			s = "�����ƁB";


		}

		/*::: hack - select_ring_slot�̖������㏑�����ĕ���A���A�����ȊO�̑S�X���b�g��Ώۂɂ��Ă��܂�*/
		select_ring_slot = TRUE;
		item_tester_no_ryoute = TRUE;

		if (!get_item(&slot, q, s, (USE_EQUIP)))
		{
			select_ring_slot = FALSE;
			return;
		}
		select_ring_slot = FALSE;
		break;
	}
	/*:::����E���E�����O�̃X���b�g���菈���I��*/

	if(!(wield_check(slot,item))) return;


	/* Prevent wielding into a cursed slot */
	if (object_is_cursed(&inventory[slot]))
	{
		/* Describe it */
		object_desc(o_name, &inventory[slot], (OD_OMIT_PREFIX | OD_NAME_ONLY));

		/* Message */
#ifdef JP
		msg_format("%s%s�͎���Ă���悤���B",
			   describe_use(slot) , o_name );
#else
		msg_format("The %s you are %s appears to be cursed.",
			   o_name, describe_use(slot));
#endif

		/* Cancel the command */
		return;
	}

	if (confirm_wear &&
		((object_is_cursed(o_ptr) && object_is_known(o_ptr)) ||
		((o_ptr->ident & IDENT_SENSE) &&
			(FEEL_BROKEN <= o_ptr->feeling) && (o_ptr->feeling <= FEEL_CURSED))))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "�{����%s{����Ă���}���g���܂����H", o_name);
#else
		sprintf(dummy, "Really use the %s {cursed}? ", o_name);
#endif

		if (!get_check(dummy)) return;
	}
	///item system �Ή��ʂ̎푰�ϗe �A���h���C�h�i�d���l�`�j���ψق���悤�ɂ��悤�Ǝv�������悭�l������o���l�v�Z����ςȂ��ƂɂȂ�
	if ((o_ptr->name1 == ART_STONEMASK) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		char dummy[MAX_NLEN+80];

		int j;

		for (j = 0; j < INVEN_PACK; j++) if(inventory[j].tval == TV_SOUVENIR && inventory[j].sval == SV_SOUVENIR_ASIA) asia = TRUE;

		if(asia)
		{
			msg_print("���ʂ̊z�Ɂu�G�C�W���̐Ԑ΁v���҂�������܂��Ă���B");
			msg_print(NULL);
			sprintf(dummy, "���̂܂ܑ������Ă݂܂����H");
			if (!get_check(dummy)) return;
		}
		else if(p_ptr->prace != RACE_VAMPIRE)
		{
			/* Describe it */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
			sprintf(dummy, "%s�𑕔�����Ƌz���S�ɂȂ�܂��B��낵���ł����H", o_name);
			if (!get_check(dummy)) return;
		}
	}
	///mod140115 �����|�̎푰�ϗe �d���l�`�͕ϐg���Ȃ�
	if ((o_ptr->name1 == ART_MENPO) && object_is_known(o_ptr) && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "%s�𑕔�����ƃj���W���ɂȂ�܂��B��낵���ł����H", o_name);
#endif

		if (!get_check(dummy)) return;
	}


	if (need_switch_wielding && !object_is_cursed(&inventory[need_switch_wielding]))
	{
		object_type *slot_o_ptr = &inventory[slot];
		object_type *switch_o_ptr = &inventory[need_switch_wielding];
		object_type object_tmp;
		object_type *otmp_ptr = &object_tmp;
		char switch_name[MAX_NLEN];

		object_desc(switch_name, switch_o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		object_copy(otmp_ptr, switch_o_ptr);
		object_copy(switch_o_ptr, slot_o_ptr);
		object_copy(slot_o_ptr, otmp_ptr);

		msg_format("%s�������ւ����B", switch_name);

#ifdef JP
	//	msg_format("%s��%s�ɍ\���Ȃ������B", switch_name, (slot == INVEN_RARM) ? (left_hander ? "����" : "�E��") : (left_hander ? "�E��" : "����"));
#else
		msg_format("You wield %s at %s hand.", switch_name, (slot == INVEN_RARM) ? (left_hander ? "left" : "right") : (left_hander ? "right" : "left"));
#endif

		slot = need_switch_wielding;
	}

	/*:::������^�N�G�X�g�ŊY���A�C�e�����E�������`�F�b�N*/
	/*:::�󕨌ɂ̃^�[�Q�b�g�������璼�ڑ��������Ƃ��̂��߂̏����Ǝv����*/
	check_find_art_quest_completion(o_ptr);


	///seikaku
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		identify_item(o_ptr);

		/* Auto-inscription */
		autopick_alter_item(item, FALSE);
	}

	/* Take a turn */
	energy_use = 100;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &inventory[slot];

	/* Take off existing item */
	if (o_ptr->k_idx)
	{
		/* Take off existing item */
		(void)inven_takeoff(slot, 255);
	}
	else if(need_takeoff)
	{
		(void)inven_takeoff(need_takeoff, 255);
	}

	/* Wear the new stuff */
	object_copy(o_ptr, q_ptr);

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Increase the weight */
	p_ptr->total_weight += q_ptr->weight;

	/* Increment the equip counter by hand */
	equip_cnt++;
/*
#ifdef JP
#define STR_WIELD_RARM "%s(%c)���E��ɑ��������B"
#define STR_WIELD_LARM "%s(%c)������ɑ��������B"
#define STR_WIELD_ARMS "%s(%c)�𗼎�ō\�����B"
#else
#define STR_WIELD_RARM "You are wielding %s (%c) in your right hand."
#define STR_WIELD_LARM "You are wielding %s (%c) in your left hand."
#define STR_WIELD_ARMS "You are wielding %s (%c) with both hands."
#endif
*/
	/* Where is the item now */
	switch (slot)
	{
	case INVEN_RARM:
	case INVEN_LARM:
#ifdef JP
		act = "%s(%c)���\�����B";
#else
		act = "You are shooting with %s (%c).";
#endif
		break;

	case INVEN_LITE:
#ifdef JP
		act = "%s(%c)�������ɂ����B";
#else
		act = "Your light source is %s (%c).";
#endif
		break;

	default:
#ifdef JP
		act = "%s(%c)�𑕔������B";
#else
		act = "You are wearing %s (%c).";
#endif
		break;
	}

	/* Describe the result */
	object_desc(o_name, o_ptr, 0);

	/* Message */
	msg_format(act, o_name, index_to_label(slot));


	/* Cursed! */
	if (object_is_cursed(o_ptr))
	{
		/* Warn the player */
#ifdef JP
		msg_print("����I �����܂����₽���I");
#else
		msg_print("Oops! It feels deathly cold!");
#endif


		//chg_virtue(V_HARMONY, -1);

		/* Note the curse */
		o_ptr->ident |= (IDENT_SENSE);
	}

	///item race �Ή��ʂ̎푰�ϗe
	/* The Stone Mask make the player turn into a vampire! */
	if ((o_ptr->name1 == ART_STONEMASK) && !(RACE_NEVER_CHANGE) && (p_ptr->prace != RACE_ULTIMATE))
	{
		/* Turn into a vampire */
		if(asia) change_race(RACE_ULTIMATE, "");
		else if(p_ptr->prace != RACE_VAMPIRE) change_race(RACE_VAMPIRE, "");
	}
	///mod140115 �����|�̎푰�ϗe�@�d���l�`�͕ϐg���Ȃ�
	if (o_ptr->name1 == ART_MENPO && !(RACE_NEVER_CHANGE) && p_ptr->prace != RACE_NINJA)
	{
		/*:::���������������������*/
		sprintf(p_ptr->history[0], "���Ȃ�������%s�Ƃ��ĕ����ɐ����Ă����B",rp_ptr->title);
		sprintf(p_ptr->history[1], "�������������������̃j���W���E�\�E�����߈˂����I");
		if(one_in_(6))
			sprintf(p_ptr->history[2], "���Ȃ������̖ړI�͗B��A���ΐ��N�ւ̕��Q���I");
		else if(one_in_(5))
			sprintf(p_ptr->history[2], "���Ȃ������̖ړI�͗B��A���̏��_�b�M�ւ̕��Q���I");
		else if(one_in_(4))
			sprintf(p_ptr->history[2], "���Ȃ������̖ړI�͗B��A�w���ׂ̃T�[�y���g�x�ւ̕��Q���I");
		else if(one_in_(3))
			sprintf(p_ptr->history[2], "���Ȃ������̖ړI�͗B��A�S�Ẵj���W���̖��E���I");
		else if(one_in_(2))
			sprintf(p_ptr->history[2], "���Ȃ������̖ړI�͗B��A������s�����ҒB�ւ̕��Q���I");
		else
			sprintf(p_ptr->history[2], "���Ȃ������̖ړI�͗B��A�S�A���o�[�̉����ւ̕��Q���I");
		sprintf(p_ptr->history[3], "����A%s�A����I",player_name);
		/* Turn into a NINJA */
		change_race(RACE_NINJA, "");
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA);

	p_ptr->redraw |= (PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp();
}


/*:::�����i�𓊂����藎�Ƃ�����O�����蔄�����肵���Ƃ��A�c���������i��K�؂ɑ���������*/
/*:::item:�O���������i�̃X���b�g*/
///mod140309 �|�g�p�~�֘A�����ǉ�
void kamaenaoshi(int item)
{
	object_type *o_ptr, *new_o_ptr;
	char o_name[MAX_NLEN];

	if (item == INVEN_RARM)
	{
		/*:::�E�葕�����O���č���ɏe��N���X�{�E�������Ă������A�E��Ɏ����ւ���*/
		if( inventory[INVEN_LARM].tval == TV_GUN || inventory[INVEN_LARM].tval == TV_CROSSBOW)
		{
			o_ptr = &inventory[INVEN_LARM];
			object_desc(o_name, o_ptr, 0);
			if (!object_is_cursed(o_ptr))
			{
				new_o_ptr = &inventory[INVEN_RARM];
				object_copy(new_o_ptr, o_ptr);
				p_ptr->total_weight += o_ptr->weight;
				inven_item_increase(INVEN_LARM, -((int)o_ptr->number));
				inven_item_optimize(INVEN_LARM);
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif

				else if(p_ptr->pclass == CLASS_3_FAIRIES)
					msg_format("%s���\���������B", o_name);
				else
#ifdef JP
					msg_format("%s��%s�ō\�����B", o_name, (left_hander ? "����" : "�E��"));
#else
					msg_format("You are wielding %s in your %s hand.", o_name, (left_hander ? "left":"right"));
#endif
			}
			else
			{
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
			}
		}
		else if (buki_motteruka(INVEN_LARM))
		{
			o_ptr = &inventory[INVEN_LARM];
			object_desc(o_name, o_ptr, 0);

			if (!object_is_cursed(o_ptr))
			{
				new_o_ptr = &inventory[INVEN_RARM];
				object_copy(new_o_ptr, o_ptr);
				p_ptr->total_weight += o_ptr->weight;
				inven_item_increase(INVEN_LARM, -((int)o_ptr->number));
				inven_item_optimize(INVEN_LARM);
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
				else if(p_ptr->pclass == CLASS_3_FAIRIES)
					msg_format("%s���\���������B", o_name);
				else
#ifdef JP
					msg_format("%s��%s�ō\�����B", o_name, (left_hander ? "����" : "�E��"));
#else
					msg_format("You are wielding %s in your %s hand.", o_name, (left_hander ? "left":"right"));
#endif
			}
			else
			{
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
			}
		}
		else if(inventory[INVEN_LARM].tval == TV_BOW)
		{
			o_ptr = &inventory[INVEN_LARM];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			if(p_ptr->pclass != CLASS_3_FAIRIES)
				msg_format("%s�𗼎�ō\�����B", o_name);
#else
			msg_format("You are wielding %s with both hands.", o_name);
#endif

		}

	}
	else if (item == INVEN_LARM)
	{
		o_ptr = &inventory[INVEN_RARM];
		if (o_ptr->k_idx) object_desc(o_name, o_ptr, 0);

		/*:::���葕�����O���ĉE��ɋ|�������Ă������A����Ɏ����ւ���*/
		if( inventory[INVEN_RARM].tval == TV_BOW)
		{
			o_ptr = &inventory[INVEN_RARM];
			object_desc(o_name, o_ptr, 0);
			if (!object_is_cursed(o_ptr))
			{
				new_o_ptr = &inventory[INVEN_LARM];
				object_copy(new_o_ptr, o_ptr);
				p_ptr->total_weight += o_ptr->weight;
				inven_item_increase(INVEN_RARM, -((int)o_ptr->number));
				inven_item_optimize(INVEN_RARM);
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
				else if(p_ptr->pclass == CLASS_3_FAIRIES)
					msg_format("%s���\���������B", o_name);
				else
#ifdef JP
					msg_format("%s��%s�ō\�����B", o_name, (left_hander ? "����" : "�E��"));
#else
					msg_format("You are wielding %s in your %s hand.", o_name, (left_hander ? "left":"right"));
#endif
			}
			else
			{
				if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
			}
		}

		else if (buki_motteruka(INVEN_RARM) || o_ptr->tval == TV_CROSSBOW || o_ptr->tval == TV_GUN)
		{
			if (object_allow_two_hands_wielding(o_ptr) && CAN_TWO_HANDS_WIELDING())
#ifdef JP
				msg_format("%s�𗼎�ō\�����B", o_name);
#else
				msg_format("You are wielding %s with both hands.", o_name);
#endif
		}
		else if (!(empty_hands(FALSE) & EMPTY_HAND_RARM) && !object_is_cursed(o_ptr))
		{
			new_o_ptr = &inventory[INVEN_LARM];
			object_copy(new_o_ptr, o_ptr);
			p_ptr->total_weight += o_ptr->weight;
			inven_item_increase(INVEN_RARM, -((int)o_ptr->number));
			inven_item_optimize(INVEN_RARM);
#ifdef JP
			msg_format("%s�������ւ����B", o_name);
#else
			msg_format("You switched hand of %s.", o_name);
#endif
		}
	}
}


/*
 * Take off an item
 */
/*:::�������O��*/
///item TVAL
void do_cmd_takeoff(void)
{
	int item;
	bool flag_quickdraw = FALSE;

	object_type *o_ptr;

	cptr q, s;
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD || p_ptr->clawextend)
	{
		msg_print("���̎p�ł͑�����ύX�ł��Ȃ��B");
		return ;
	}
	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ�𑕔�����͂����܂���? ";
	s = "�͂����鑕�����Ȃ��B";
#else
	q = "Take off which item? ";
	s = "You are not wearing anything to take off.";
#endif
	/*:::�O��������I���B�������ꗗ�ŏ\���Ȃ̂�hook�̎w��͕s�v*/
	if (!get_item(&item, q, s, (USE_EQUIP))) return;

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

	if(o_ptr->tval == TV_GUN && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_QUICK_DRAW)) 
		flag_quickdraw = TRUE;

	///mod140122 ��������
	if(!(wield_check(item,INVEN_PACK))) return;

	///sys ��������E�A�푰�����L�q�v �i���������j
	//v1.1.28 ������ƋL�q�Ǝd�l�ύX
	/* Item is cursed */
	if (object_is_cursed(o_ptr) )
	{
		bool force_remove1 = FALSE; //�������O���t���O
		bool force_remove2 = FALSE; //�G�߂ĊO���t���O

		if(o_ptr->curse_flags & TRC_PERMA_CURSE)
		{
			msg_print("�������O���Ȃ��B���̑����i�̎􂢂͂��܂�ɂ������I");
			return;
		}

		if(p_ptr->pclass == CLASS_FLAN) force_remove1 = TRUE;
		if(p_ptr->pclass == CLASS_YUGI) force_remove1 = TRUE;
		if(p_ptr->pseikaku == SEIKAKU_BERSERK) force_remove1 = TRUE;

		if(p_ptr->pclass == CLASS_TSUKUMO_MASTER) force_remove2 = TRUE;
		if(p_ptr->prace == RACE_TSUKUMO) force_remove2 = TRUE;

		if(force_remove1 || force_remove2)
		{
			bool success = FALSE;

			if(o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				if(one_in_(7))
				{
					if(force_remove2)
						msg_print("���͂Ɏ��ꂽ�i���Ȃ��߂ĉ��Ƃ��O��Ă�������B");
					else 
						msg_print("���͂Ȏ􂢂�͂Â��Ŕ��������I");
					success = TRUE;
				}
				else
				{
					if(force_remove2)
					{
						msg_print("���͂Ɏ��ꂽ�i���Ȃ��߂ĊO�����Ƃ��������s�����B");
					}
					else
					{
						msg_print("���͂Ȏ􂢂�͂Â��Ŕ��������Ƃ�������R���ꂽ�I");
						if(one_in_(2)) 
						{
							msg_print("�_���[�W���󂯂��I");
							take_hit(DAMAGE_NOESCAPE,randint1(50),"���͂Ȏ􂢂�͂����Ŕ��������Ƃ����ɂ�",-1);
						}
					}
				}
			}
			else
			{
				if(one_in_(4))
				{
					if(force_remove2)
						msg_print("���ꂽ�i���Ȃ��߂ĉ��Ƃ��O��Ă�������B");
					else 
						msg_print("���ꂽ������͂Â��Ŕ��������I");
					success = TRUE;
				}
				else
				{
					if(force_remove2)
						msg_print("���ꂽ�i���Ȃ��߂悤�Ƃ��������s�����B");				
					else
					{
						msg_print("���ꂽ������͂Â��Ŕ��������Ƃ�������R���ꂽ�I");
						if(one_in_(4)) 
						{
							msg_print("�_���[�W���󂯂��I");
							take_hit(DAMAGE_NOESCAPE,randint1(20),"�􂢂�͂����Ŕ��������Ƃ����ɂ�",-1);
						}
					}
				}
			}

			if(success)
			{
				o_ptr->ident |= (IDENT_SENSE);

				if(force_remove1)
				{
					o_ptr->curse_flags = 0L;
					/* Take note */
					o_ptr->feeling = FEEL_NONE;
					msg_print("�􂢂�ł��j�����B");
				}
				/* Recalculate the bonuses */
				p_ptr->update |= (PU_BONUS);
				/* Window stuff */
				p_ptr->window |= (PW_EQUIP);
			}
			else
			{
				return;
			}
		}
		else
		{
			msg_print("�������O���Ȃ��B�ǂ�������Ă���悤���B");
			return;
		}

#if 0
		///class ����m�����ꂽ�����𔍂������ꏈ��
		///mod140119 �t�r�_���ǉ������@�􂢉����͂���Ȃ�
		if ((o_ptr->curse_flags & TRC_PERMA_CURSE) || (p_ptr->pclass != CLASS_TSUKUMO_MASTER && p_ptr->prace != RACE_TSUKUMO && p_ptr->pclass != CLASS_FLAN))
		{
			/* Oops */
#ifdef JP
			msg_print("�Ӂ[�ށA�ǂ�������Ă���悤���B");
#else
			msg_print("Hmmm, it seems to be cursed.");
#endif

			/* Nope */
			return;
		}
		/*:::����m�͗͂Â��ő������͂����@����m(�ƕt�r�_)�ȊO�͂��ł�return���Ă���*/
		if (((o_ptr->curse_flags & TRC_HEAVY_CURSE) && one_in_(7)) || one_in_(4))
		{
#ifdef JP

			if(p_ptr->pclass == CLASS_FLAN ) msg_print("���ꂽ������͂Â��Ŕ��������I");
			else msg_print("���ꂽ�i���Ȃ��߂ĉ��Ƃ��O��Ă�������B");
#else
			msg_print("You teared a cursed equipment off by sheer strength!");
#endif

			/* Hack -- Assume felt */
			o_ptr->ident |= (IDENT_SENSE);

			if(p_ptr->pclass == CLASS_FLAN )
			{
				o_ptr->curse_flags = 0L;
				/* Take note */
				o_ptr->feeling = FEEL_NONE;
#ifdef JP
				msg_print("�􂢂�ł��j�����B");
#else
				msg_print("You break the curse.");
#endif
			}
			/* Recalculate the bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Window stuff */
			p_ptr->window |= (PW_EQUIP);


		}
		else
		{
#ifdef JP
			msg_print("�������O���Ȃ������B");
#else
			msg_print("You couldn't remove the equipment.");
#endif
			energy_use = 50;
			return;
		}
#endif

	}


	//��\��o�����t���f
	stop_tsukumo_music();

	/* Take a partial turn */
	if(flag_quickdraw)
	{
		msg_print("���Ȃ��͑f�����e��[�߂��B");
		energy_use = 25;
	}
	else
		energy_use = 50;

	/* Take off the item */
	(void)inven_takeoff(item, 255);

	/*:::���莝���Ȃǂ̍Ĕ���H*/
	kamaenaoshi(item);

	calc_android_exp();

	p_ptr->redraw |= (PR_EQUIPPY);
	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

}


/*
 * Drop an item
 */
/*:::�A�C�e���𗎂Ƃ�*/
void do_cmd_drop(void)
{
	int item, amt = 1;

	object_type *o_ptr;

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���𗎂Ƃ��܂���? ";
	s = "���Ƃ���A�C�e���������Ă��Ȃ��B";
#else
	q = "Drop which item? ";
	s = "You have nothing to drop.";
#endif

	///mod160224 ���ŋ����A�C�e�����ɏE����o�O�ւ̑Ή��̂��ߑS�A�C�e���\��ON
	//���������Ad�R�}���h���s�[�g�ŋ�A�C�e����������Ȃǃo�O�p�o����̂ł����߂�
	//item_tester_full = TRUE;
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN)))
	{
		//item_tester_full = FALSE;
		return;
	}
	//item_tester_full = FALSE;

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

	///mod140122 ��������
	if(!(wield_check(item,INVEN_PACK))) return;


	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_RARM) && object_is_cursed(o_ptr))
	{
		///sys ��������E�A�푰�����L�q�v3
		/* Oops */
#ifdef JP
		msg_print("�Ӂ[�ށA�ǂ�������Ă���悤���B");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif


		/* Nope */
		return;
	}


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Take a partial turn */
	energy_use = 50;

	/* Drop (some of) the item */
	inven_drop(item, amt);

	if (item >= INVEN_RARM)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}

	p_ptr->redraw |= (PR_EQUIPPY);
}

///sysdel ���@���j��Ōo���l�𓾂邽�߂̔���
static bool high_level_book(object_type *o_ptr)
{
	/*
	if ((o_ptr->tval == TV_LIFE_BOOK) ||
	    (o_ptr->tval == TV_SORCERY_BOOK) ||
	    (o_ptr->tval == TV_NATURE_BOOK) ||
	    (o_ptr->tval == TV_CHAOS_BOOK) ||
	    (o_ptr->tval == TV_DEATH_BOOK) ||
	    (o_ptr->tval == TV_TRUMP_BOOK) ||
	    (o_ptr->tval == TV_CRAFT_BOOK) ||
	    (o_ptr->tval == TV_DAEMON_BOOK) ||
	    (o_ptr->tval == TV_CRUSADE_BOOK) ||
	    (o_ptr->tval == TV_MUSIC_BOOK) ||
		(o_ptr->tval == TV_HEX_BOOK))
	{
		if (o_ptr->sval > 1)
			return TRUE;
		else
			return FALSE;
	}
	*/
	return FALSE;
}


/*
 * Destroy an item
 */
/*:::�A�C�e������*/
/*:::command_arg���ݒ肳��Ă���ꍇ�m�F���Ȃ��H*/
void do_cmd_destroy(void)
{
	int			item, amt = 1;
	int			old_number;

	bool		force = FALSE;

	object_type		*o_ptr;
	object_type             forge;
	object_type             *q_ptr = &forge;

	char		o_name[MAX_NLEN];

	char		out_val[MAX_NLEN+40];

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;


	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�����󂵂܂���? ";
	s = "�󂹂�A�C�e���������Ă��Ȃ��B";
#else
	q = "Destroy which item? ";
	s = "You have nothing to destroy.";
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

	/* Verify unless quantity given beforehand */
	/*:::command_arg��1�ȏ�ŃA�C�e���������l�łȂ����m�F�I�v�V������ON�̏ꍇ�m�F����*/
	if (!force && (confirm_destroy || (object_value(o_ptr) > 0)))
	{
		object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

		/* Make a verification */
		sprintf(out_val, 
#ifdef JP
			"�{����%s���󂵂܂���? [y/n/Auto]",
#else
			"Really destroy %s? [y/n/Auto]",
#endif
			o_name);

		msg_print(NULL);

		/* HACK : Add the line to message buffer */
		message_add(out_val);
		p_ptr->window |= (PW_MESSAGE);
		window_stuff();

		/* Get an acceptable answer */
		/*:::�A�C�e�����󂷂��ǂ����m�F���A�I���ɂ���Ă͎����j��̑Ώۂɒǉ�*/
		while (TRUE)
		{
			char i;

			/* Prompt */
			prt(out_val, 0, 0);

			i = inkey();

			/* Erase the prompt */
			prt("", 0, 0);


			if (i == 'y' || i == 'Y')
			{
				break;
			}
			if (i == ESCAPE || i == 'n' || i == 'N')
			{
				/* Cancel */
				return;
			}
			if (i == 'A')
			{
				/* Add an auto-destroy preference line */
				if (autopick_autoregister(o_ptr))
				{
					/* Auto-destroy it */
					autopick_alter_item(item, TRUE);
				}

				/* The object is already destroyed. */
				return;
			}
		} /* while (TRUE) */
	}

	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, 0);
	o_ptr->number = old_number;

	/* Take a turn */
	energy_use = 100;

	/* Artifacts cannot be destroyed */
	/*:::���ʐ��͉󂹂Ȃ��B���Ӓ�Ȃ�ȈՊӒ肷��*/
	if (!can_player_destroy_object(o_ptr))
	{
		energy_use = 0;

		/* Message */
#ifdef JP
		msg_format("%s�͔j��s�\���B", o_name);
#else
		msg_format("You cannot destroy %s.", o_name);
#endif

		/* Done */
		return;
	}

	object_copy(q_ptr, o_ptr);

	/* Message */
#ifdef JP
	msg_format("%s���󂵂��B", o_name);
#else
	msg_format("You destroy %s.", o_name);
#endif

	sound(SOUND_DESTITEM);

	/* Reduce the charges of rods/wands */
	/*:::���b�h�Ɩ��@�_�̏[�U������*/
	reduce_charges(o_ptr, amt);

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

	/*:::�ꕔ�E����ʖ��@�����󂷂ƌo���l�𓾂鏈��*/
	///sysdel class ��m��p���f�B���̖��@���j��o���l�l������
#if 0
	if (high_level_book(q_ptr))
	{
		bool gain_expr = FALSE;

		if (p_ptr->prace == RACE_ANDROID)
		{
		}
		else if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_BERSERKER))
		{
			gain_expr = TRUE;
		}
		else if (p_ptr->pclass == CLASS_PALADIN)
		{
			if (is_good_realm(p_ptr->realm1))
			{
				if (!is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
			else
			{
				if (is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
		}

		if (gain_expr && (p_ptr->exp < PY_MAX_EXP))
		{
			s32b tester_exp = p_ptr->max_exp / 20;
			if (tester_exp > 10000) tester_exp = 10000;
			if (q_ptr->sval < 3) tester_exp /= 4;
			if (tester_exp<1) tester_exp = 1;

#ifdef JP
msg_print("�X�Ɍo����ς񂾂悤�ȋC������B");
#else
			msg_print("You feel more experienced.");
#endif

			gain_exp(tester_exp * amt);
		}
		if (high_level_book(q_ptr) && q_ptr->tval == TV_LIFE_BOOK)
		{
			chg_virtue(V_UNLIFE, 1);
			chg_virtue(V_VITALITY, -1);
		}
		else if (high_level_book(q_ptr) && q_ptr->tval == TV_DEATH_BOOK)
		{
			chg_virtue(V_UNLIFE, -1);
			chg_virtue(V_VITALITY, 1);
		}
	
		if (q_ptr->to_a || q_ptr->to_h || q_ptr->to_d)
			chg_virtue(V_ENCHANT, -1);
	
		if (object_value_real(q_ptr) > 30000)
			chg_virtue(V_SACRIFICE, 2);
	
		else if (object_value_real(q_ptr) > 10000)
			chg_virtue(V_SACRIFICE, 1);
	}
	///sysdel virtue
	if (q_ptr->to_a != 0 || q_ptr->to_d != 0 || q_ptr->to_h != 0)
		chg_virtue(V_HARMONY, 1);
#endif
	if (item >= INVEN_RARM) calc_android_exp();
}


/*
 * Observe an item which has been *identify*-ed
 */
/*:::�A�C�e���̏ڍ׏��𒲂ׂ�*/
void do_cmd_observe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���𒲂ׂ܂���? ";
	s = "���ׂ���A�C�e�����Ȃ��B";
#else
	q = "Examine which item? ";
	s = "You have nothing to examine.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

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


	/* Require full knowledge */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
		///mod150121 �d�����򎞓��ꏈ��
		if(p_ptr->pclass == CLASS_KEINE && p_ptr->magic_num1[0] )
		{
			msg_print("���̃A�C�e���ɂ��Ă̒m�������ɗ��ꍞ��ł���I");
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->ident |= (IDENT_MENTAL);
			p_ptr->update |= (PU_BONUS);
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
		}
		else
		{
			msg_print("���̃A�C�e���ɂ��ē��ɒm���Ă��邱�Ƃ͂Ȃ��B");
			return;
		}
	}


	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
#ifdef JP
	msg_format("%s�𒲂ׂĂ���...", o_name);
#else
	msg_format("Examining %s...", o_name);
#endif

	/* Describe it fully */
#ifdef JP
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("���ɕς�����Ƃ���͂Ȃ��悤���B");
#else
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("You see nothing special.");
#endif

}



/*
 * Remove the inscription from an object
 * XXX Mention item (when done)?
 */
/*:::�A�C�e���̖�������*/
void do_cmd_uninscribe(void)
{
	int   item;

	object_type *o_ptr;

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���̖��������܂���? ";
	s = "����������A�C�e�����Ȃ��B";
#else
	q = "Un-inscribe which item? ";
	s = "You have nothing to un-inscribe.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

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

	/* Nothing to remove */
	if (!o_ptr->inscription)
	{
#ifdef JP
		msg_print("���̃A�C�e���ɂ͏����ׂ������Ȃ��B");
#else
		msg_print("That item had no inscription to remove.");
#endif

		return;
	}

	/* Message */
#ifdef JP
	msg_print("�����������B");
#else
	msg_print("Inscription removed.");
#endif


	/* Remove the incription */
	/*:::���ւ̃C���f�b�N�X���폜�@quark�Ɋi�[���ꂽ���{�̂ɂ͉������Ȃ��炵��*/
	o_ptr->inscription = 0;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* .��$�̊֌W��, �Čv�Z���K�v�Ȃ͂� -- henkma */
	p_ptr->update |= (PU_BONUS);

}


/*
 * Inscribe an object with a comment
 */
/*:::�A�C�e���ɖ�������*/
void do_cmd_inscribe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	char		out_val[80];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���ɖ������݂܂���? ";
	s = "�������߂�A�C�e�����Ȃ��B";
#else
	q = "Inscribe which item? ";
	s = "You have nothing to inscribe.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];

		if(check_invalidate_inventory(item))
		{
			msg_format("���܂��̃A�C�e���ɂ͐G��Ȃ��B");
			return;
		}
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Describe the activity */
	object_desc(o_name, o_ptr, OD_OMIT_INSCRIPTION);

	/* Message */
#ifdef JP
	msg_format("%s�ɖ������ށB", o_name);
#else
	msg_format("Inscribing %s.", o_name);
#endif

	msg_print(NULL);

	/* Start with nothing */
	strcpy(out_val, "");

	/* Use old inscription */
	if (o_ptr->inscription)
	{
		/* Start with the old inscription */
		strcpy(out_val, quark_str(o_ptr->inscription));
	}

	/* Get a new inscription (possibly empty) */
#ifdef JP
	if (get_string("��: ", out_val, 80))
#else
	if (get_string("Inscription: ", out_val, 80))
#endif
	{
		/* Save the inscription */
		o_ptr->inscription = quark_add(out_val);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* .��$�̊֌W��, �Čv�Z���K�v�Ȃ͂� -- henkma */
		p_ptr->update |= (PU_BONUS);
	}
}



/*
 * An "item_tester_hook" for refilling lanterns
 */
/*:::�����^���ɕ⋋�@���ق����̃����^��*/
///mod140819 TV_FLASK�ɑ��̃A�C�e�����₵���̂Ŗ��₾���Ώۂɂ���悤�C��
static bool item_tester_refill_lantern(object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval == TV_FLASK && o_ptr->sval == SV_FLASK_OIL) return (TRUE);

	/* Laterns are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refill the players lamp (from the pack or floor)
 */
/*:::�����^���ւ̔R���⋋*/
///item ego �����^���ւ̔R���⋋
static void do_cmd_refill_lamp(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;

	if(check_invalidate_inventory(INVEN_LITE))
	{
		msg_print("���܂͌����������Ȃ��B");
		return;
	}

	if (world_player || SAKUYA_WORLD)
	{
		msg_print("�����^���̓��͓������悤�ɂ��߂����~�߂Ă���E�E");
		return;
	}


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_lantern;


	/* Get an item */
#ifdef JP
	q = "�ǂ̖��ڂ��璍���܂���? ";
	s = "���ڂ��Ȃ��B";
#else
	q = "Refill with which flask? ";
	s = "You have no flasks of oil.";
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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the lantern */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4;

	/* Message */
#ifdef JP
	msg_print("�����v�ɖ��𒍂����B");
#else
	msg_print("You fuel your lamp.");
#endif

	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����v�������Ă��܂����I");
#else
		msg_print("Your lamp has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����������v�͑S������Ȃ��B");
#else
		msg_print("Curiously, your lamp doesn't light.");
#endif
	}
	else if (j_ptr->xtra4 >= FUEL_LAMP)
	{
		j_ptr->xtra4 = FUEL_LAMP;
#ifdef JP
		msg_print("�����v�̖��͈�t���B");
#else
		msg_print("Your lamp is full.");
#endif

	}

	if(object_is_artifact(o_ptr))
	{
		o_ptr->xtra4 = 0;
		p_ptr->update |= (PU_TORCH | PU_BONUS);
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		return;
	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}


/*
 * An "item_tester_hook" for refilling torches
 */
static bool item_tester_refill_torch(object_type *o_ptr)
{
	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH) && !object_is_artifact(o_ptr)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refuel the players torch (from the pack or floor)
 */
///item �����܂̎��������΂�
static void do_cmd_refill_torch(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;

	if(check_invalidate_inventory(INVEN_LITE))
	{
		msg_print("���܂͏����ɐG��Ȃ��B");
		return;
	}
	if (world_player || SAKUYA_WORLD)
	{
		msg_print("�����̓��͓������悤�ɂ��߂����~�߂Ă���E�E");
		return;
	}

	/* Restrict the choices */
	item_tester_hook = item_tester_refill_torch;

	/* Get an item */
#ifdef JP
	q = "�ǂ̏����Ŗ���������߂܂���? ";
	s = "���ɏ������Ȃ��B";
#else
	q = "Refuel with which torch? ";
	s = "You have no extra torches.";
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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the primary torch */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4 + 5;

	/* Message */
#ifdef JP
	msg_print("���������������B");
#else
	msg_print("You combine the torches.");
#endif


	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����������Ă��܂����I");
#else
		msg_print("Your torch has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����������͑S������Ȃ��B");
#else
		msg_print("Curiously, your torche don't light.");
#endif
	}
	/* Over-fuel message */
	else if (j_ptr->xtra4 >= FUEL_TORCH)
	{
		j_ptr->xtra4 = FUEL_TORCH;
#ifdef JP
		msg_print("�����̎����͏\�����B");
#else
		msg_print("Your torch is fully fueled.");
#endif

	}

	/* Refuel message */
	else
	{
#ifdef JP
		msg_print("�����͂����������邭�P�����B");
#else
		msg_print("Your torch glows more brightly.");
#endif

	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}


/*
 * Refill the players lamp, or restock his torches
 */
/*:::�R���⋋*/
///item TVAL �����܂������^���ւ̔R���⋋
void do_cmd_refill(void)
{
	object_type *o_ptr;

	/* Get the light */
	o_ptr = &inventory[INVEN_LITE];

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* It is nothing */
	if (o_ptr->tval != TV_LITE)
	{
#ifdef JP
		msg_print("�����𑕔����Ă��Ȃ��B");
#else
		msg_print("You are not wielding a light.");
#endif

	}

	/* It's a lamp */
	else if (o_ptr->sval == SV_LITE_LANTERN)
	{
		do_cmd_refill_lamp();
	}

	/* It's a torch */
	else if (o_ptr->sval == SV_LITE_TORCH && !object_is_artifact(o_ptr))
	{
		do_cmd_refill_torch();
	}

	/* No torch to refill */
	else
	{
#ifdef JP
		msg_print("���̌����͎��������΂��Ȃ��B");
#else
		msg_print("Your light cannot be refilled.");
#endif

	}
}


/*
 * Target command
 */
/*:::�^�[�Q�b�g�ݒ�@l�R�}���h�Ƃǂ��Ⴄ�̂��悭�����*/
void do_cmd_target(void)
{
	/* Target set */
	if (target_set(TARGET_KILL))
	{
#ifdef JP
		msg_print("�^�[�Q�b�g����B");
#else
		msg_print("Target Selected.");
#endif

	}

	/* Target aborted */
	else
	{
#ifdef JP
		msg_print("�^�[�Q�b�g�����B");
#else
		msg_print("Target Aborted.");
#endif

	}
}



/*
 * Look command
 */
void do_cmd_look(void)
{
	/*TEST*/
	p_ptr->window |= PW_MONSTER_LIST;
	p_ptr->window |= PW_FLOOR_ITEM_LIST;
	window_stuff();
	/*TEST*/

	/* Look around */
	if (target_set(TARGET_LOOK))
	{
#ifdef JP
		msg_print("�^�[�Q�b�g����B");
#else
		msg_print("Target Selected.");
#endif

	}
}



/*
 * Allow the player to examine other sectors on the map
 */
/*:::�}�b�v�����n���@�ڍז���*/
void do_cmd_locate(void)
{
	int		dir, y1, x1, y2, x2;

	char	tmp_val[80];

	char	out_val[160];

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);


	/* Start at current panel */
	y2 = y1 = panel_row_min;
	x2 = x1 = panel_col_min;

	/* Show panels until done */
	while (1)
	{
		/* Describe the location */
		if ((y2 == y1) && (x2 == x1))
		{
#ifdef JP
			strcpy(tmp_val, "�^��");
#else
			tmp_val[0] = '\0';
#endif

		}
		else
		{
#ifdef JP
			sprintf(tmp_val, "%s%s",
				((y2 < y1) ? "�k" : (y2 > y1) ? "��" : ""),
				((x2 < x1) ? "��" : (x2 > x1) ? "��" : ""));
#else
			sprintf(tmp_val, "%s%s of",
				((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
				((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));
#endif

		}

		/* Prepare to ask which way to look */
		sprintf(out_val,
#ifdef JP
			"�}�b�v�ʒu [%d(%02d),%d(%02d)] (�v���C���[��%s)  ����?",
#else
			"Map sector [%d(%02d),%d(%02d)], which is%s your sector.  Direction?",
#endif

			y2 / (hgt / 2), y2 % (hgt / 2),
			x2 / (wid / 2), x2 % (wid / 2), tmp_val);

		/* Assume no direction */
		dir = 0;

		/* Get a direction */
		while (!dir)
		{
			char command;

			/* Get a command (or Cancel) */
			if (!get_com(out_val, &command, TRUE)) break;

			/* Extract the action (if any) */
			dir = get_keymap_dir(command);

			/* Error */
			if (!dir) bell();
		}

		/* No direction */
		if (!dir) break;

		/* Apply the motion */
		if (change_panel(ddy[dir], ddx[dir]))
		{
			y2 = panel_row_min;
			x2 = panel_col_min;
		}
	}


	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();
}



/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
/*:::ang_sort()�Ɏg�����߂̐���֐��̂ЂƂH*/
bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;

	/* Sort by player kills */
	if (*why >= 4)
	{
		/* Extract player kills */
		z1 = r_info[w1].r_pkills;
		z2 = r_info[w2].r_pkills;

		/* Compare player kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = r_info[w1].r_tkills;
		z2 = r_info[w2].r_tkills;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = r_info[w1].level;
		z2 = r_info[w2].level;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = r_info[w1].mexp;
		z2 = r_info[w2].mexp;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
/*:::ang_sort()�̃N�C�b�N�\�[�g�p�̓���ւ����[�`��*/
void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Unused */
	(void)v;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}



/*
 * Identify a character, allow recall of monsters
 *
 * Several "special" responses recall "multiple" monsters:
 *   ^A (all monsters)
 *   ^U (all unique monsters)
 *   ^N (all non-unique monsters)
 *
 * The responses may be sorted in several ways, see below.
 *
 * Note that the player ghosts are ignored. XXX XXX XXX
 */
/*:::�����X�^�[�̎v���o������@�ڍז���*/
void do_cmd_query_symbol(void)
{
	int		i, n, r_idx;
	char	sym, query;
	char	buf[128];

	bool	all = FALSE;
	bool	uniq = FALSE;
	bool	norm = FALSE;
	bool	ride = FALSE;
	char    temp[80] = "";

	bool	recall = FALSE;

	u16b	why = 0;
	u16b	*who;

	/* Get a character, or abort */
#ifdef JP
	if (!get_com("�m�肽����������͂��ĉ�����(�L�� or ^A�S,^U��,^N��,^R��n,^M���O): ", &sym, FALSE)) return;
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE)) return;
#endif

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

	/* Describe */
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "�S�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Full monster list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�E�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Unique monster list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�O�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Non-unique monster list.");
#endif
	}
	else if (sym == KTRL('R'))
	{
		all = ride = TRUE;
#ifdef JP
		strcpy(buf, "��n�\�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Ridable monster list.");
#endif
	}
	/* XTRA HACK WHATSEARCH */
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("���O(�p��̏ꍇ�������ŉ�)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;
			return;
		}
#ifdef JP
		sprintf(buf, "���O:%s�Ƀ}�b�`",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
		sprintf(buf, "%c - %s", sym, "�����ȕ���");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif
	}

	/* Display the result */
	prt(buf, 0, 0);

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		///mod140712 �σp�����[�^�����X�^�[��/�R�}���h����Q�Ƃł��Ȃ��悤�ɂ���
		if(r_ptr->flags7 & RF7_VARIABLE) continue;

		/* Nothing to recall */
		if (!cheat_know && !r_ptr->r_sights) continue;

		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require ridable monsters if needed */
		if (ride && !(r_ptr->flags7 & (RF7_RIDING))) continue;

		/* XTRA HACK WHATSEARCH */
		if (temp[0])
		{
		  int xx;
		  char temp2[80];
  
		  for (xx=0; temp[xx] && xx<80; xx++)
		  {
#ifdef JP
		    if (iskanji( temp[xx])) { xx++; continue; }
#endif
		    if (isupper(temp[xx])) temp[xx]=tolower(temp[xx]);
		  }
  
#ifdef JP
		  strcpy(temp2, r_name+r_ptr->E_name);
#else
		  strcpy(temp2, r_name+r_ptr->name);
#endif
		  for (xx=0; temp2[xx] && xx<80; xx++)
		    if (isupper(temp2[xx])) temp2[xx]=tolower(temp2[xx]);
  
#ifdef JP
		  if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp) )
#else
		  if (my_strstr(temp2, temp))
#endif
			  who[n++]=i;
		}

		/* Collect "appropriate" monsters */
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		return;
	}


	/* Prompt XXX XXX XXX */
#ifdef JP
	put_str("�v���o�����܂���? (k:�œ|����/y/n): ", 0, 36);
#else
	put_str("Recall details? (k/y/n): ", 0, 40);
#endif


	/* Query */
	query = inkey();

	/* Restore */
	prt(buf, 0, 0);

	why = 2;

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array */
	ang_sort(who, &why, n);

	/* Sort by kills (and level) */
	if (query == 'k')
	{
		why = 4;
		query = 'y';
	}

	/* Catch "escape" */
	if (query != 'y')
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		return;
	}

	/* Sort if needed */
	if (why == 4)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	i = n - 1;

	/* Scan the monster memory */
	while (1)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Auto-recall */
		monster_race_track(r_idx);

		/* Hack -- Handle stuff */
		handle_stuff();

		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Save the screen */
				screen_save();

				/* Recall on screen */
				screen_roff(who[i], 0);
			}

			/* Hack -- Begin the prompt */
			roff_top(r_idx);

			/* Hack -- Complete the prompt */
#ifdef JP
			Term_addstr(-1, TERM_WHITE, " ['r'�v���o, ESC]");
#else
			Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");
#endif

			/* Command */
			query = inkey();

			/* Unrecall */
			if (recall)
			{
				/* Restore */
				screen_load();
			}

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, u16b);

	/* Re-display the identity */
	prt(buf, 0, 0);
}


/*:::���݂̎푰��E�ƂȂǂőI�������A�C�e���𑕔��i���邢�͑����O���j�\���`�F�b�N����B*/
/*:::slot�̓A�C�e���̑����ӏ���INVEN_???�ԍ� ������������Ă���ꍇ���ꂪ�O����邱�ƂɂȂ�B*/
/*:::item_new�͐V������������A�C�e����INVEN_???�ԍ��i�U�b�N�j���邢�͕��̏ꍇ�����o_list�Y����*/
/*:::�����O���A���Ƃ��A���p�ŐV���ɑ���������̂��Ȃ���item_new�ɂ͂��ӂ�̈��INVEN_PACK�����Ă��̊֐����ĂԂ��Ƃɂ����B*/
/*:::����Ŗ��Ȃ��Ƃ͎v������Ȃ��炩�Ȃ�Ђǂ��������B�|�C���^�����܂��ɂ悭�������Ă��Ȃ��B*/
///mod140914 ���j�[�N�N���X���Ή��ʂȂǂŎ푰�ϗe�������O���Ă͂����Ȃ����̂��O��Ă��܂��̂ŏ������������ύX
//slot==item_new�̂Ƃ���OK�ɂ��Ă���
bool wield_check( int slot, int item_new)
{
	object_type *o_ptr_old = &inventory[slot];
	object_type *o_ptr_new;

	if(slot < INVEN_RARM) return TRUE;

	if(item_new >= 0) o_ptr_new = &inventory[item_new];
	else			  o_ptr_new = &o_list[0 - item_new];


	//msg_format("slot:%d oldTV:%d newTV:%d",slot, o_ptr_old->tval, o_ptr_new->tval);

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_WIELD)
	{
		msg_print("���͑�����ύX�ł��Ȃ��B");
		return (FALSE);
	}

	if(check_invalidate_inventory(slot))
	{
		msg_print("�����̑�����ύX���邱�Ƃ͂ł��Ȃ��B");
		return (FALSE);
	}



	//v1.1.57 �ꎞ�A�C�e���u�������v�͊O���Ȃ�
	if (o_ptr_old->tval == TV_SWORD && o_ptr_old->sval == SV_WEAPON_SEVEN_STAR)
	{
		msg_print("���̑������O�����Ƃ͂ł��Ȃ��B");
		return (FALSE);

	}

	//�푰����
	/*:::�V��͏�ɓ��݂𑕔�*/
	//v1.1.80 ��p���i�̕��͕�
	if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU){
		if(slot == INVEN_HEAD && o_ptr_new->sval != SV_HEAD_TOKIN && !is_special_seikaku(SEIKAKU_SPECIAL_AYA))
		{
			msg_print("���Ȃ��͓��ɓ��݂𑕔����Ȃ��Ƃ����Ȃ��B");
			return (FALSE);
		}
	}

	//�E�ƌʐݒ�

	if(p_ptr->pclass == CLASS_KOKORO)
	{
		if(slot == INVEN_HEAD && o_ptr_new->tval == TV_HEAD)
		{
			msg_print("���Ȃ��͊���̖ʈȊO���肽���͂Ȃ��B");
			return (FALSE);
		}
	}
	//v2.0.12�d�m�q�͕���⏂�����ĂȂ� �O���}���ȂǕ��툵���łȂ�����A�C�e���͎��Ă�
	else if (p_ptr->pclass == CLASS_ENOKO)
	{
		if ((slot == INVEN_RARM || slot == INVEN_LARM) && (object_is_weapon(o_ptr_new) || object_is_shooting_weapon(o_ptr_new)|| o_ptr_new->tval == TV_SHIELD))
		{
			msg_print("�g���o�T�~�Ɉ����������Ď��ĂȂ��B");
			return (FALSE);
		}
	}

	else if(p_ptr->pclass == CLASS_ALICE)
	{
		if(slot == INVEN_HANDS)
		{
			msg_print("���������Ɛl�`�𑀂�Ȃ��B");
			return (FALSE);
		}
		if(object_is_weapon(o_ptr_new))
		{
			msg_print("���Ȃ��͎��畐���������͂Ȃ��B");
			return (FALSE);
		}
		else if(slot == INVEN_RARM)
		{
			msg_print("��������Ɛl�`�𑀂�Ȃ��B");
			return (FALSE);
		}
	}
	else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI)
	{
		if(slot == INVEN_HANDS)
		{
			msg_print("������܂͉��t�̎ז����B");
			return (FALSE);
		}
	}

	//�O����
	//v1.1.68 ��p���i�u�d����W���v�ł͐����Ȃ�
	else if(p_ptr->pclass == CLASS_3_FAIRIES && !is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES) &&
		slot >= INVEN_RIGHT && slot <= INVEN_FEET && slot != INVEN_LITE)
	{
		int i;
		int cnt = 0;
		for( i= INVEN_RIGHT;i<=INVEN_FEET;i++)
		{
			if(i == INVEN_LITE) continue;
			if(!inventory[i].k_idx) continue;
			if(i == slot) continue;

			if(inventory[i].tval == o_ptr_new->tval) cnt++;
			else if(wield_slot(&inventory[i]) == wield_slot(o_ptr_new)) cnt++;

		}
		if(cnt > 2)
		{
			msg_print("���łɎO�l�Ƃ����̕��ʂ̑��������Ă���B");
			return (FALSE);
		}
	}

	else if(p_ptr->pclass == CLASS_MAID || p_ptr->pclass == CLASS_SAKUYA)
	{
		if(slot == INVEN_BODY && p_ptr->psex == SEX_MALE && (o_ptr_new->tval != TV_CLOTHES || o_ptr_new->sval != SV_CLOTH_SUIT))
		{
			msg_print("���Ȃ��͏�ɃX�[�c�𒅗p���Ȃ��Ƃ����Ȃ��B");
			return (FALSE);
		}
		if(slot == INVEN_BODY && p_ptr->psex == SEX_FEMALE &&  (o_ptr_new->tval != TV_CLOTHES || o_ptr_new->sval != SV_CLOTH_MAID))
		{
			msg_print("���Ȃ��͏�Ƀ��C�h���𒅗p���Ȃ��Ƃ����Ȃ��B");
			return (FALSE);
		}
	}
	else if(p_ptr->pclass == CLASS_KISUME)
	{
		if(slot == INVEN_BODY && (o_ptr_new->tval != TV_ARMOR || o_ptr_new->sval != SV_ARMOR_TUB))
		{
			msg_print("���Ȃ��͉�����o���Ȃ��B");
			return (FALSE);
		}
	}

	//v1.1.37 ����ꎞ�ϐg
	if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI && o_ptr_old->name1 == ART_HYAKKI && slot != item_new)
	{
		msg_print("���͊G������������Ȃ��B");
		return (FALSE);
	}

	/*:::���P���ꏈ���@������O���Ȃ�*/
	///mod151101 ���_���̓��P�ƃx�[�X�A�C�e�������p������F�X�s��ł��̂Ōp���ڂ��B����͉������������ƕ����悤�B
	if(p_ptr->pclass == CLASS_KOGASA 
		&& o_ptr_old->tval == TV_STICK && o_ptr_old->sval == SV_WEAPON_KOGASA && o_ptr_old->weight == 30 && slot != item_new)
	{
		msg_print("�������������Ƃ͂ł��Ȃ��B�Ƃ���������͂��Ȃ��̖{�̂��B");
		return (FALSE);
	}

	if(p_ptr->pclass == CLASS_IKU && slot == INVEN_OUTER && o_ptr_old->name1 == ART_HEAVENLY_MAIDEN && slot != item_new)
	{
		msg_print("�厖�ȉH�߂��B������Ȃ��悤�ɂ��悤�B");
		return (FALSE);
	}
	if(p_ptr->pclass == CLASS_YUGI  && o_ptr_old->tval == TV_SHIELD && o_ptr_old->sval == SV_SAKAZUKI && slot != item_new)
	{
		msg_print("��������������͂Ȃ��B");
		return (FALSE);
	}
	if(p_ptr->pclass == CLASS_EIKI && 
		!ironman_no_fixed_art && 
		(object_is_melee_weapon(o_ptr_new) && o_ptr_new->name1 != ART_EIKI
		|| o_ptr_old->name1 == ART_EIKI && slot != item_new))
	{
		msg_print("���Ȃ��͉���̖_�ȊO�̂��̂�U�邤����͂Ȃ��B");
		return (FALSE);
	}

	/*:::�l���A�j���Â͌C�𗚂��Ȃ��@�O�����Ƃ͂ł��邪�����ɗ����Ɏ����I�ɊO���\��*/
	if(p_ptr->muta3 & MUT3_FISH_TAIL || p_ptr->prace == RACE_NINGYO  || p_ptr->pclass == CLASS_TOZIKO || p_ptr->pclass == CLASS_KISUME){
		if(slot == INVEN_FEET && o_ptr_new->tval != 0)
		{
			msg_print("���Ȃ��͌C�𗚂����Ƃ��ł��Ȃ��B");
			return (FALSE);
		}
	}
	/*:::�Ԕ؊�͎����������Ȃ�*/
	if(p_ptr->pclass == CLASS_BANKI ){
		if(slot == INVEN_NECK && o_ptr_new->tval != 0)
		{
			msg_print("���Ȃ��͎����������Ȃ��B");
			return (FALSE);
		}
	}
	//�����͎w�ւ��p�R�}���h���瑕������B
	//�܂���艿�i�ȉ��̂��̂𑕔��ł��Ȃ��B���{�c
	if (p_ptr->pclass == CLASS_JYOON) 
	{
		if (slot >= INVEN_RIGHT && slot <= INVEN_LEFT)
		{
			msg_print("�w�ւ̑����͐E�Ɠ��Z�R�}���h����s���B");
			return (FALSE);
		}
		/*
		if (o_ptr_new->k_idx && object_is_cheap_to_jyoon(o_ptr_new))
		{
			msg_print("���Ȃ��͂���Ȉ����𑕔��������Ȃ��B");
			return (FALSE);
		}

		if (o_ptr_old->k_idx && object_is_cheap_to_jyoon(o_ptr_old) && slot != item_new)
		{
			msg_print("���̂悤�Ȉ����͓�x�Ɛg�ɂ��邱�Ƃ͂Ȃ����낤�B");
			if (!get_check_strict("�{���ɍ��̑������O���܂����H", CHECK_OKAY_CANCEL))
			{
				return FALSE;
			}
		}
		*/

	}

	//v1.1.73
	if (p_ptr->pclass == CLASS_YACHIE)
	{
		if (o_ptr_new->tval == TV_ARMOR || o_ptr_new->tval == TV_DRAG_ARMOR)
		{
			msg_print("�w���̍b�����ז��ŊZ�𑕒��ł��Ȃ��B");
			return (FALSE);

		}
	}



	/*:::�p�������Ă�Ɗ������Ԃ�Ȃ�*/
	if(p_ptr->muta2 & (MUT2_BIGHORN) || p_ptr->pclass == CLASS_KASEN){
		if(slot == INVEN_HEAD && ( 
			o_ptr_new->sval == SV_HEAD_METALCAP 
		||  o_ptr_new->sval == SV_HEAD_STARHELM 
		||  o_ptr_new->sval == SV_HEAD_STEELHELM 
		||  o_ptr_new->sval == SV_HEAD_DRAGONHELM 
			))
		{
			if(p_ptr->pclass == CLASS_KASEN)
				msg_print("���Ȃ��͂���𑕔��ł��Ȃ��B");
			else
				msg_print("�p���ז��Ŋ������Ԃ�Ȃ��B");
			return (FALSE);
		}
	}

	if(p_ptr->pclass == CLASS_HECATIA)
	{
		if(slot == INVEN_HEAD && ( 
			o_ptr_new->sval == SV_HEAD_METALCAP 
		||  o_ptr_new->sval == SV_HEAD_STARHELM 
		||  o_ptr_new->sval == SV_HEAD_STEELHELM 
		||  o_ptr_new->sval == SV_HEAD_DRAGONHELM ))
		{
			msg_print("���Ȃ��͊��𑕔��ł��Ȃ��B");
			return (FALSE);
		}
	}


	if(p_ptr->muta3 & MUT3_PEGASUS){
		if(slot == INVEN_HEAD && o_ptr_new->tval != 0)
		{
			msg_print("���Ȃ��͍����ɉ��������ł��Ȃ��B");
			return (FALSE);
		}
	}


	if(p_ptr->prace == RACE_KOBITO || p_ptr->pclass == CLASS_NINJA)
	{
		if(((slot == INVEN_RARM || slot == INVEN_LARM) && o_ptr_new->weight > 60) 
			|| (slot == INVEN_BODY && o_ptr_new->weight > 100)
			|| (slot == INVEN_HEAD && o_ptr_new->weight > 50)
			|| (slot == INVEN_HANDS && o_ptr_new->weight > 30)
			|| (slot == INVEN_FEET && o_ptr_new->weight > 30))
		{
			msg_print("����𑕔�����Əd���Ă��܂������Ȃ��B");
			return (FALSE);

		}
	}

	if(p_ptr->pclass == CLASS_NINJA)
	{
		if(o_ptr_new->tval == TV_SHIELD) 
		{
			msg_print("���͓����̎ז����B");
			return (FALSE);

		}
	}

	return (TRUE);
}
