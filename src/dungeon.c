/* File: dungeonc */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Angband game engine */

#include "angband.h"

#define TY_CURSE_CHANCE 200
#define CHAINSWORD_NOISE 100

static bool load = TRUE;

//�u�L��}�b�v�ł�dungeon_turn�l�������ɂ��������v��HPMP�����񕜏������s���Ă���Ԃ͓K�p���Ȃ��悤�ɂ��邽�߂̔���l�Ǝv����
static int wild_regen = 20;


/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
/*:::�ȈՊӒ�@�����ȈՊӒ�*/
///mod151121 ���Ŏg���̂�extern����

byte value_check_aux1(object_type *o_ptr)
{
	/* Artifacts */
	if (object_is_artifact(o_ptr))
	{
		/* Cursed/Broken */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Ego-Items */
	if (object_is_ego(o_ptr))
	{
		/* Cursed/Broken */
		if (object_is_cursed(o_ptr) || object_is_broken(o_ptr)) return FEEL_WORTHLESS;

		/* Normal */
		return FEEL_EXCELLENT;
	}

	/* Cursed items */
	if (object_is_cursed(o_ptr)) return FEEL_CURSED;

	/* Broken items */
	if (object_is_broken(o_ptr)) return FEEL_BROKEN;

	///mod151121�ǉ�..���悤�Ǝv��������߂��BIDENT_BROKEN���������ƈ��e�����o�邩������Ȃ�
	//if(o_ptr->pval < 0 || o_ptr->to_a < 0 || o_ptr->to_d < 0 || o_ptr->to_h < 0)
	//	 return FEEL_BROKEN;

	if ((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)) return FEEL_AVERAGE;

	/* Good "armor" bonus */
	if (o_ptr->to_a > 0) return FEEL_GOOD;

	/* Good "weapon" bonus */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_GOOD;

	/* Default to "average" */
	return FEEL_AVERAGE;
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
/*:::�ア�ȈՊӒ�*/
static byte value_check_aux2(object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (object_is_cursed(o_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if (object_is_broken(o_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if (object_is_artifact(o_ptr)) return FEEL_UNCURSED;

	/* Ego-Items -- except cursed/broken ones */
	if (object_is_ego(o_ptr)) return FEEL_UNCURSED;

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return FEEL_UNCURSED;

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_UNCURSED;

	/* No feeling */
	return FEEL_NONE;
}



static void sense_inventory_aux(int slot, bool heavy)
{
	byte        feel;
	object_type *o_ptr = &inventory[slot];
	char        o_name[MAX_NLEN];

	/* We know about it already, do not tell us again */
	if (o_ptr->ident & (IDENT_SENSE))return;

	if(check_invalidate_inventory(slot)) return;

	/* It is fully known, no information needed */
	if (object_is_known(o_ptr)) return;

	/* Check for a feeling */
	feel = (heavy ? value_check_aux1(o_ptr) : value_check_aux2(o_ptr));

	/* Skip non-feelings */
	if (!feel) return;

	/* Bad luck */
	if ((p_ptr->muta3 & MUT3_BAD_LUCK) && !randint0(13))
	{
		switch (feel)
		{
			case FEEL_TERRIBLE:
			{
				feel = FEEL_SPECIAL;
				break;
			}
			case FEEL_WORTHLESS:
			{
				feel = FEEL_EXCELLENT;
				break;
			}
			case FEEL_CURSED:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_GOOD : FEEL_AVERAGE;
				else
					feel = FEEL_UNCURSED;
				break;
			}
			case FEEL_AVERAGE:
			{
				feel = randint0(2) ? FEEL_CURSED : FEEL_GOOD;
				break;
			}
			case FEEL_GOOD:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_CURSED : FEEL_AVERAGE;
				else
					feel = FEEL_CURSED;
				break;
			}
			case FEEL_EXCELLENT:
			{
				feel = FEEL_WORTHLESS;
				break;
			}
			case FEEL_SPECIAL:
			{
				feel = FEEL_TERRIBLE;
				break;
			}
		}
	}

	/* Stop everything */
	if (disturb_minor) disturb(0, 0);

	/* Get an object description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	//�f�P���ꏈ��
	if(p_ptr->pclass == CLASS_EIKI)
	{
		identify_item(o_ptr);
		object_desc(o_name, o_ptr, 0L);
		msg_format("�����%s�ɈႢ�Ȃ��B",o_name);
	}
	else
	{
		/* Message (equipment) */
		if (slot >= INVEN_RARM)
		{
			msg_format("%s%s(%c)��%s�Ƃ�������������...",
				describe_use(slot),o_name, index_to_label(slot),game_inscriptions[feel]);
		}
		/* Message (inventory) */
		else
		{
			msg_format("�U�b�N�̒���%s(%c)��%s�Ƃ�������������...",
				o_name, index_to_label(slot),game_inscriptions[feel]);

		}
		/* We have "felt" it */
		o_ptr->ident |= (IDENT_SENSE);
		/* Set the "inscription" */
		o_ptr->feeling = feel;
	}


	/* Auto-inscription/destroy */
	autopick_alter_item(slot, destroy_feeling);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
}



/*
 * Sense the inventory
 *
 *   Class 0 = Warrior --> fast and heavy
 *   Class 1 = Mage    --> slow and light
 *   Class 2 = Priest  --> fast but light
 *   Class 3 = Rogue   --> okay and heavy
 *   Class 4 = Ranger  --> slow but heavy  (changed!)
 *   Class 5 = Paladin --> slow but heavy
 */
///class item TVAL �����A�C�e���̊ȈՊӒ�
static void sense_inventory1(void)
{
	int         i;
	int         plev = p_ptr->lev;
	bool        heavy = FALSE;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

	///mod140202 �ȈՊӒ�\�͂��N���X�Œ�̕ϐ��Q�Ƃɂ���
	switch (cp_ptr->sense_arms)
	{
		case 1:
			if (0 != randint0(240000L / (plev + 5))) return;
			break;
		case 2:
			if (0 != randint0(80000L / (plev * plev + 40))) return;
			break;
		case 3:
			if (0 != randint0(70000L / (plev * plev + 40))) return;
			heavy = TRUE;
			break;
		case 4:
			if (0 != randint0(20000L / (plev * plev + 40))) return;
			heavy = TRUE;
			break;
		case 5:
			if (0 != randint0(8000L / (plev * plev + 40))) return;
			heavy = TRUE;
			break;
		default:
			msg_print("ERROR:player_class.sense_arms�̐ݒ�l����������");
			return;

	}

#if 0
	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_SMITH:
		{
			/* Good sensing */
			if (0 != randint0(6000L / (plev * plev + 50))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		{
			/* Good (light) sensing */
			if (0 != randint0(10000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_ROGUE:
		case CLASS_NINJA:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_PALADIN:
		case CLASS_SNIPER:
		{
			/* Bad sensing */
			if (0 != randint0(77777L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		{
			/* Bad sensing */
			if (0 != randint0(75000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_IMITATOR:
		case CLASS_BLUE_MAGE:
		case CLASS_MIRROR_MASTER:
		{
			/* Bad sensing */
			if (0 != randint0(55000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_CHAOS_WARRIOR:
		{
			/* Bad sensing */
			if (0 != randint0(80000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_BEASTMASTER:
		{
			/* Bad sensing */
			if (0 != randint0(65000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}
		case CLASS_BERSERKER:
		{
			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}
	}
#endif
	///del131221  virtue
	//if (compare_virtue(V_KNOWLEDGE, 100, VIRTUE_LARGE)) heavy = TRUE;

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			///mod131223 �ȈՊӒ�\�ȕ���h��
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
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		/* Good luck */
		if ((p_ptr->muta3 & MUT3_GOOD_LUCK) && !randint0(13))
		{
			heavy = TRUE;
		}

		sense_inventory_aux(i, heavy);
	}
}

/*:::�ȈՊӒ菈��*/
///class item TVAL �ȈՊӒ�
static void sense_inventory2(void)
{
	int         i;
	int         plev = p_ptr->lev;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;



	///mod140202 �ȈՊӒ�\�͂��N���X�Œ�̕ϐ��Q�Ƃɂ���
	switch (cp_ptr->sense_mags)
	{
		case 1:
			return;
		case 2:
			if (0 != randint0(120000L / (plev * plev + 20))) return;
			break;
		case 3:
			if (0 != randint0(80000L / (plev * plev + 40))) return;
			break;
		case 4:
			if (0 != randint0(20000L / (plev * plev + 40))) return;
			break;
		case 5:
			if (0 != randint0(8000L / (plev * plev + 40))) return;
			break;
		default:
			msg_print("ERROR:player_class.sense_mags�̐ݒ�l����������");
			return;
	}
#if 0

	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		case CLASS_BERSERKER:
		case CLASS_SNIPER:
		{
			return;
		}

		case CLASS_SMITH:
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		case CLASS_IMITATOR:
		case CLASS_BEASTMASTER:
		case CLASS_NINJA:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		case CLASS_MONK:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_ROGUE:
		case CLASS_FORCETRAINER:
		case CLASS_MINDCRAFTER:
		{
			/* Good sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		case CLASS_MIRROR_MASTER:
		case CLASS_BLUE_MAGE:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Done */
			break;
		}
	}
#endif
	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		///mod131223�@�ȈՊӒ�\�Ȗ��@�̃A�C�e���Ƒ���
		switch (o_ptr->tval)
		{

			case TV_MAGICITEM:
			case TV_MAGICWEAPON:
			case TV_RIBBON:
			case TV_RING:
			case TV_AMULET:
			case TV_LITE:
			case TV_WHISTLE:
			case TV_FIGURINE:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		sense_inventory_aux(i, TRUE);
	}
}



/*
 * Go to any level (ripped off from wiz_jump)
 */
static void pattern_teleport(void)
{
	int min_level = 0;
	int max_level = 99;

	/* Ask for level */
#ifdef JP
	if (!EXTRA_MODE && get_check("���̊K�Ƀe���|�[�g���܂����H"))
#else
	if (get_check("Teleport level? "))
#endif

	{
		char	ppp[80];
		char	tmp_val[160];

		/* Only downward in ironman mode */
		if (ironman_downward)
			min_level = dun_level;

		/* Maximum level */
		if (dungeon_type == DUNGEON_ANGBAND)
		{
			if (dun_level > 100)
				max_level = MAX_DEPTH - 1;
			else if (dun_level == 100)
				max_level = 100;
		}
		else
		{
			max_level = d_info[dungeon_type].maxdepth;
			min_level = d_info[dungeon_type].mindepth;
		}

		/* Prompt */
#ifdef JP
		sprintf(ppp, "�e���|�[�g��:(%d-%d)", min_level, max_level);
#else
		sprintf(ppp, "Teleport to level (%d-%d): ", min_level, max_level);
#endif


		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);
	}
#ifdef JP
	//else if (get_check("�ʏ�e���|�[�g�H"))
	else if (get_check("���̃t���A�̖]�ޏꏊ�ɓ]�ڂ��܂����H"))
#else
	else if (get_check("Normal teleport? "))
#endif
	{
		dimension_door(D_DOOR_PATTERN);
		return;
	}
	else
	{
		return;
	}

	/* Paranoia */
	if (command_arg < min_level) command_arg = min_level;

	/* Paranoia */
	if (command_arg > max_level) command_arg = max_level;

	/* Accept request */
#ifdef JP
	msg_format("%d �K�Ƀe���|�[�g���܂����B", command_arg);
#else
	msg_format("You teleport to dungeon level %d.", command_arg);
#endif


	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	leave_quest_check();

	if (record_stair) do_cmd_write_nikki(NIKKI_PAT_TELE,0,NULL);

	p_ptr->inside_quest = 0;
	energy_use = 0;

	/*
	 * Clear all saved floors
	 * and create a first saved floor
	 */
	prepare_change_floor_mode(CFM_FIRST_FLOOR);

	/* Leaving */
	p_ptr->leaving = TRUE;
}


static void wreck_the_pattern(void)
{
	int to_ruin = 0, r_y, r_x;
	int pattern_type = f_info[cave[py][px].feat].subtype;

	if (pattern_type == PATTERN_TILE_WRECKED)
	{
		/* Ruined already */
		return;
	}

#ifdef JP
	msg_print("�p�^�[�������ŉ����Ă��܂����I");
	msg_print("�������낵�������N�������I");
#else
	msg_print("You bleed on the Pattern!");
	msg_print("Something terrible happens!");
#endif

	if (!IS_INVULN())
#ifdef JP
		take_hit(DAMAGE_NOESCAPE, damroll(10, 8), "�p�^�[������", -1);
#else
		take_hit(DAMAGE_NOESCAPE, damroll(10, 8), "corrupting the Pattern", -1);
#endif

	to_ruin = randint1(45) + 35;

	while (to_ruin--)
	{
		scatter(&r_y, &r_x, py, px, 4, 0);

		if (pattern_tile(r_y, r_x) &&
		    (f_info[cave[r_y][r_x].feat].subtype != PATTERN_TILE_WRECKED))
		{
			cave_set_feat(r_y, r_x, feat_pattern_corrupted);
		}
	}

	cave_set_feat(py, px, feat_pattern_corrupted);
}


/* Returns TRUE if we are on the Pattern... */
static bool pattern_effect(void)
{
	int pattern_type;

	if (!pattern_tile(py, px)) return FALSE;
/*
	if ((prace_is_(RACE_AMBERITE)) &&
	    (p_ptr->cut > 0) && one_in_(10))
	{
		wreck_the_pattern();
	}
*/
	pattern_type = f_info[cave[py][px].feat].subtype;

	switch (pattern_type)
	{
	case PATTERN_TILE_END:
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
		(void)hp_player(1000);

		cave_set_feat(py, px, feat_pattern_old);

#ifdef JP
		msg_print("�u�p�^�[���v�̂��̕����͑��̕�����苭�͂łȂ��悤���B");
#else
		msg_print("This section of the Pattern looks less powerful.");
#endif

		/*
		 * We could make the healing effect of the
		 * Pattern center one-time only to avoid various kinds
		 * of abuse, like luring the win monster into fighting you
		 * in the middle of the pattern...
		 */
		break;

	case PATTERN_TILE_OLD:
		/* No effect */
		break;

	case PATTERN_TILE_TELEPORT:
		pattern_teleport();
		break;

	case PATTERN_TILE_WRECKED:
		if (!IS_INVULN())
#ifdef JP
			take_hit(DAMAGE_NOESCAPE, 200, "��ꂽ�u�p�^�[���v��������_���[�W", -1);
#else
			take_hit(DAMAGE_NOESCAPE, 200, "walking the corrupted Pattern", -1);
#endif
		break;

	default:
		//if (prace_is_(RACE_AMBERITE) && !one_in_(2))
		//	return TRUE;
		//else 
			if (!IS_INVULN())
#ifdef JP
			take_hit(DAMAGE_NOESCAPE, damroll(1, 3), "�u�p�^�[���v��������_���[�W", -1);
#else
			take_hit(DAMAGE_NOESCAPE, damroll(1, 3), "walking the Pattern", -1);
#endif
		break;
	}

	return TRUE;
}





/*
 * Regenerate hit points				-RAK-
 */
/*:::����HP�����񕜁@�ڍז���*/
static void regenhp(int percent)
{
	s32b new_chp;
	u32b new_chp_frac;
	s32b old_chp;

	if (p_ptr->special_defense & KATA_KOUKIJIN) return;
	if (p_ptr->action == ACTION_HAYAGAKE) return;

	/* Save the old hitpoints */
	old_chp = p_ptr->chp;

	///mod140720 �F���Ɛ����n�C��*�}��*����
	if(p_ptr->pclass == CLASS_YOSHIKA ||  (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_LIFE && p_ptr->lev > 39))
	{
		percent = percent * p_ptr->lev / 10;
	}
	//�ޖ������x���ɂ����*�}��*
	else if (p_ptr->pclass == CLASS_YUMA)
	{
		percent = percent * (10 + p_ptr->lev) / 6;
	}
	else if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])//�z�K�q�~��
	{
		percent *= 10;
	}
	else if(p_ptr->pclass == CLASS_SUNNY && is_daytime()) percent *= 2;
	else if(p_ptr->pclass == CLASS_LUNAR && !is_daytime()) percent *= 2;
	else if(p_ptr->prace == RACE_HOURAI) percent = percent * 5 / 2;
	else //���̋����n�����L�� +50%�`+150%
	{
		int jizo_card_num = count_ability_card(ABL_CARD_JIZO);

		if (jizo_card_num)
		{
			int mult = calc_ability_card_prob(ABL_CARD_JIZO, jizo_card_num);

			percent += percent * mult / 100;

		}
	}


	/*
	 * Extract the new hitpoints
	 *
	 * 'percent' is the Regen factor in unit (1/2^16)
	 */
	new_chp = 0;
	new_chp_frac = (p_ptr->mhp * percent + PY_REGEN_HPBASE);



	/* Convert the unit (1/2^16) to (1/2^32) */
	s64b_LSHIFT(new_chp, new_chp_frac, 16);

	/* Regenerating */
	s64b_add(&(p_ptr->chp), &(p_ptr->chp_frac), new_chp, new_chp_frac);


	/* Fully healed */
	if (0 < s64b_cmp(p_ptr->chp, p_ptr->chp_frac, p_ptr->mhp, 0))
	{
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if (old_chp != p_ptr->chp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		wild_regen = 20;
	}
}


/*
 * Regenerate mana points
 */
/*:::MP�񕜁A�������͒��߃R�X�g�ɂ�錸�� upkeep_factor=�y�b�g�ێ��ȂǃR�X�g regen_amount=�񕜊�����{�l�@�ڍז���*/
static void regenmana(int upkeep_factor, int regen_amount)
{
	s32b old_csp = p_ptr->csp;
	s32b regen_rate = regen_amount * 100 - upkeep_factor * PY_REGEN_NORMAL;

	//v1.1.84 ��Ⴢ��Ă���Ƃ��͉񕜂��Ȃ�
	if (p_ptr->paralyzed) return;

	/*
	 * Excess mana will decay 32 times faster than normal
	 * regeneration rate.
	 */
	if (p_ptr->csp > p_ptr->msp)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		s32b decay = 0;
		u32b decay_frac = (p_ptr->msp * 32 * PY_REGEN_NORMAL + PY_REGEN_MNBASE);

		///mod150419 �i���Ƃ̒���MP�����ʂ͂����Ƒ�������
		if(p_ptr->pclass == CLASS_MARTIAL_ARTIST)
		{
			decay_frac *= 3;
			p_ptr->update |= (PU_BONUS);

		}
		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(decay, decay_frac, 16);

		/* Decay */
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), decay, decay_frac);

		/* Stop decaying */
		if (p_ptr->csp < p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
	}

	/* Regenerating mana (unless the player has excess mana) */
	else if (regen_rate > 0)
	{
		/* (percent/100) is the Regen factor in unit (1/2^16) */
		s32b new_mana = 0;
		u32b new_mana_frac = (p_ptr->msp * regen_rate / 100 + PY_REGEN_MNBASE);

		//�p�`�����[��MP�񕜑��x������ �ő�2.5�{
		if(p_ptr->pclass == CLASS_PATCHOULI)
		{
			new_mana_frac += new_mana_frac * p_ptr->lev / 20;
		}
		else if(p_ptr->prace == RACE_LUNARIAN)//���̖���MP�񕜑��x������ 2�{
		{
			new_mana_frac *= 2;
		}
		else if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0]) //�z�K�q�~������10�{
		{
			new_mana_frac *= 10;
		}
		else//v1.1.68 �A�r���e�B�J�[�h�u�]�΂ʐ�̃X�y���v���L�� 1.25�`2�{
		{
			int p_card_num = count_ability_card(ABL_CARD_PATHE_SPELL);
			if (p_card_num)
			{
				int percen = calc_ability_card_prob(ABL_CARD_PATHE_SPELL, p_card_num);
				new_mana_frac += new_mana_frac * percen / 100;

			}
		}

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(new_mana, new_mana_frac, 16);

		/* Regenerate */
		s64b_add(&(p_ptr->csp), &(p_ptr->csp_frac), new_mana, new_mana_frac);

		/* Must set frac to zero even if equal */
		if (p_ptr->csp >= p_ptr->msp)
		{
			p_ptr->csp = p_ptr->msp;
			p_ptr->csp_frac = 0;
		}
	}


	/* Reduce mana (even when the player has excess mana) */
	if (regen_rate < 0)
	{
		/* PY_REGEN_NORMAL is the Regen factor in unit (1/2^16) */
		//s32b reduce_mana = 0;
		//u32b reduce_mana_frac = (p_ptr->msp * (-1) * regen_rate / 100 + PY_REGEN_MNBASE);
		/* Convert the unit (1/2^16) to (1/2^32) */
		//s64b_LSHIFT(reduce_mana, reduce_mana_frac, 16);

		//�y�b�g���߃R�X�g�𗐖\�ɕύX
		///mod141101 ���߃R�X�g�ɘa
		//s32b reduce_mana = upkeep_factor / 10;
		s32b reduce_mana = 0 - regen_rate / PY_REGEN_NORMAL / 3;
		u32b reduce_mana_frac = 0;
		if(reduce_mana < 0) reduce_mana = 0;



		/* Reduce mana */
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), reduce_mana, reduce_mana_frac);

		/* Check overflow */
		if (p_ptr->csp < 0)
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
	}


	/* Redraw mana */
	if (old_csp != p_ptr->csp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);

		wild_regen = 20;
	}
}



/*
 * Regenerate magic
 * regen_amount: PY_REGEN_NORMAL * 2 (if resting) * 2 (if having regenarate)
 */
static void regenmagic(int regen_amount)
{
	s32b new_mana;
	int i;
	int dev = 30;
	int mult = (dev + adj_mag_mana[p_ptr->stat_ind[A_INT]]); /* x1 to x2 speed bonus for recharging */

	if(p_ptr->pclass != CLASS_SEIJA) //���ׂ̂Ƃ��͏�Ɩ��@�_���񕜂��Ȃ�
	{
		for (i = 0; i < EATER_EXT*2; i++)
		{
			if (!p_ptr->magic_num2[i]) continue;
			if (p_ptr->magic_num1[i] == ((long)p_ptr->magic_num2[i] << 16)) continue;

			/* Increase remaining charge number like float value */
			new_mana = (regen_amount * mult * ((long)p_ptr->magic_num2[i] + 13)) / (dev * 8);
			p_ptr->magic_num1[i] += new_mana;

			/* Check maximum charge */
			if (p_ptr->magic_num1[i] > (p_ptr->magic_num2[i] << 16))
			{
				p_ptr->magic_num1[i] = ((long)p_ptr->magic_num2[i] << 16);
			}
			wild_regen = 20;
		}
	}
	for (i = EATER_EXT*2; i < EATER_EXT*3; i++)
	{
		if (!p_ptr->magic_num1[i]) continue;
		if (!p_ptr->magic_num2[i]) continue;

		/* Decrease remaining period for charging */
		new_mana = (regen_amount * mult * ((long)p_ptr->magic_num2[i] + 10) * EATER_ROD_CHARGE) 
					/ (dev * 16 * PY_REGEN_NORMAL); 
		p_ptr->magic_num1[i] -= new_mana;

		/* Check minimum remaining period for charging */
		if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;
		wild_regen = 20;
	}
}






/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
/*:::�����X�^�[�S�Ă̎��R�� process_world()����Ă΂��*/
///system �����X�^�[���R�񕜁@�������[�h�͂����Ɖ񕜑��x�𑁂����Ă���������
static void regen_monsters(void)
{
	int i, frac;


	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];


		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->hp < m_ptr->maxhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->maxhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			//v1.1.41 ������R��@�񕜑��x����
			if (p_ptr->pclass == CLASS_MAI && i == p_ptr->riding)
			{
				int chr = p_ptr->stat_ind[A_CHR] + 3;
				frac += MAX(0, frac * (chr - 5) / 15);
			}

			/* Hack -- Regenerate */
			m_ptr->hp += frac;

			/* Do not over-regenerate */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			/* Redraw (later) if needed */
			if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);
		}
	}
}


/*
 * Regenerate the captured monsters (once per 30 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
/*:::�u���L���Ă���v�����X�^�[�{�[���̒��̃����X�^�[��HP���񕜂���B�Ƃ⏰�̃{�[���̒��̃����X�^�[�͉񕜂��Ȃ��B*/
static void regen_captured_monsters(void)
{
	int i, frac;
	bool heal = FALSE;

	/* Regenerate everyone */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		monster_race *r_ptr;
		object_type *o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;
		//v1.1.85 �d���{�ȊO�ɂ��ߊl�ł��鑕����ǉ��B�������xtra6�Ƀ����X�^�[r_idx���L�^����.xtra4��5�͓���
		if (o_ptr->tval == TV_CAPTURE)
		{
			if (!o_ptr->pval) continue;

			r_ptr = &r_info[o_ptr->pval];
		}
		else
		{
			if (activation_index(o_ptr) != ACT_ART_CAPTURE) continue;
			if (!o_ptr->xtra6) continue;
			r_ptr = &r_info[o_ptr->xtra6];
		}


		heal = TRUE;


		/* Allow regeneration (if needed) */
		if (o_ptr->xtra4 < o_ptr->xtra5)
		{
			/* Hack -- Base regeneration */
			frac = o_ptr->xtra5 / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			/* Hack -- Regenerate */
			o_ptr->xtra4 += frac;

			/* Do not over-regenerate */
			if (o_ptr->xtra4 > o_ptr->xtra5) o_ptr->xtra4 = o_ptr->xtra5;
		}
	}

	if (heal)
	{
		/* Combine pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
		p_ptr->window |= (PW_EQUIP);
		wild_regen = 20;
	}
}

/*:::�����肪����������������Ȏ��̃��b�Z�[�W*/
static void notice_lite_change(object_type *o_ptr)
{
	/* Hack -- notice interesting fuel steps */
	if ((o_ptr->xtra4 < 100) || (!(o_ptr->xtra4 % 100)))
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Hack -- Special treatment when blind */
	if (p_ptr->blind)
	{
		/* Hack -- save some light for later */
		if (o_ptr->xtra4 == 0) o_ptr->xtra4++;
	}

	/* The light is now out */
	else if (o_ptr->xtra4 == 0)
	{
		disturb(0, 1);
#ifdef JP
msg_print("�����肪�����Ă��܂����I");
#else
		msg_print("Your light has gone out!");
#endif

		/* Recalculate torch radius */
		p_ptr->update |= (PU_TORCH);

		/* Some ego light lose its effects without fuel */
		p_ptr->update |= (PU_BONUS);
	}

	/* The light is getting dim */
	else if (o_ptr->name2 == EGO_LITE_LONG)
	{
		if ((o_ptr->xtra4 < 50) && (!(o_ptr->xtra4 % 5))
		    && (turn % (TURNS_PER_TICK*2)))
		{
			if (disturb_minor) disturb(0, 1);
#ifdef JP
msg_print("�����肪�����ɂȂ��Ă��Ă���B");
#else
			msg_print("Your light is growing faint.");
#endif

		}
	}

	/* The light is getting dim */
	else if ((o_ptr->xtra4 < 100) && (!(o_ptr->xtra4 % 10)))
	{
		if (disturb_minor) disturb(0, 1);
#ifdef JP
msg_print("�����肪�����ɂȂ��Ă��Ă���B");
#else
		msg_print("Your light is growing faint.");
#endif

	}
}

/*:::�N�G�X�g�̏I����Ԃ��`�F�b�N�H�ڍז���*/
///quest
void leave_quest_check(void)
{
	/* Save quest number for dungeon pref file ($LEAVING_QUEST) */
	leaving_quest = p_ptr->inside_quest;

	/* Leaving an 'only once' quest marks it as failed */
	if (leaving_quest &&
	    ((quest[leaving_quest].flags & QUEST_FLAG_ONCE)  || (quest[leaving_quest].type == QUEST_TYPE_RANDOM)) &&
	    (quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
		quest[leaving_quest].complev = (byte)p_ptr->lev;

		/* Additional settings */
		switch (quest[leaving_quest].type)
		{
		case QUEST_TYPE_TOWER:
			quest[QUEST_TOWER1].status = QUEST_STATUS_FAILED;
			quest[QUEST_TOWER1].complev = (byte)p_ptr->lev;
			break;
		case QUEST_TYPE_FIND_ARTIFACT:
			a_info[quest[leaving_quest].k_idx].gen_flags &= ~(TRG_QUESTITEM);
			break;
		case QUEST_TYPE_RANDOM:
			r_info[quest[leaving_quest].r_idx].flags1 &= ~(RF1_QUESTOR);

			/* Floor of random quest will be blocked */
			prepare_change_floor_mode(CFM_NO_RETURN);
			break;
		}

		/* Record finishing a quest */
		if (quest[leaving_quest].type == QUEST_TYPE_RANDOM)
		{
			if (record_rand_quest) do_cmd_write_nikki(NIKKI_RAND_QUEST_F, leaving_quest, NULL);
		}
		else
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_F, leaving_quest, NULL);
		}
	}
	//���ꏈ���@�������̌��s���ۓW�ƃ}���R���N�G�͊K�i��o��ƒB�����I����
	else if(leaving_quest == QUEST_MOON_VAULT && quest[leaving_quest].status == QUEST_STATUS_COMPLETED)
	{
		quest[QUEST_MOON_VAULT].status = QUEST_STATUS_FINISHED;
	}
	else if(leaving_quest == QUEST_MARICON && quest[leaving_quest].status == QUEST_STATUS_COMPLETED)
	{
		quest[QUEST_MARICON].status = QUEST_STATUS_FINISHED;
	}
	else if(leaving_quest)
	{
		quest[leaving_quest].flags |= QUEST_FLAG_RETRY;
	}


}

/*:::������o��Ƃ��̏���*/
void leave_tower_check(void)
{
	leaving_quest = p_ptr->inside_quest;
	/* Check for Tower Quest */
	if (leaving_quest &&
		(quest[leaving_quest].type == QUEST_TYPE_TOWER) &&
		(quest[QUEST_TOWER1].status != QUEST_STATUS_COMPLETED))
	{
		if(quest[leaving_quest].type == QUEST_TYPE_TOWER)
		{
			quest[QUEST_TOWER1].status = QUEST_STATUS_FAILED;
			quest[QUEST_TOWER1].complev = (byte)p_ptr->lev;
		}
	}
}


/*
 * Forcibly pseudo-identify an object in the inventory
 * (or on the floor)
 *
 * note: currently this function allows pseudo-id of any object,
 * including silly ones like potions & scrolls, which always
 * get '{average}'. This should be changed, either to stop such
 * items from being pseudo-id'd, or to allow psychometry to
 * detect whether the unidentified potion/scroll/etc is
 * good (Cure Light Wounds, Restore Strength, etc) or
 * bad (Poison, Weakness etc) or 'useless' (Slime Mold Juice, etc).
 */
/*:::�T�C�R���g���[*/
bool psychometry(void)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	byte            feel;
	cptr            q, s;
	bool okay = FALSE;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e���𒲂ׂ܂����H";
s = "���ׂ�A�C�e��������܂���B";
#else
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* It is fully known, no information needed */
	if (object_is_known(o_ptr))
	{
#ifdef JP
msg_print("�����V�������Ƃ͔���Ȃ������B");
#else
		msg_print("You cannot find out anything more about that.");
#endif

		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Skip non-feelings */
	if (!feel)
	{
#ifdef JP
msg_format("%s����͓��ɕς�������͊����Ƃ�Ȃ������B", o_name);
#else
		msg_format("You do not perceive anything unusual about the %s.", o_name);
#endif

		return TRUE;
	}

#ifdef JP
msg_format("%s��%s�Ƃ�������������...",
    o_name,  game_inscriptions[feel]);
#else
	msg_format("You feel that the %s %s %s...",
			   o_name, ((o_ptr->number == 1) ? "is" : "are"),
			   game_inscriptions[feel]);
#endif


	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* "Inscribe" it */
	o_ptr->feeling = feel;

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Valid "tval" codes */
	///item TVAL �T�C�R���g���[�̂Ƃ��̎����j��\�t���O�H
	///mod131223 tval
	switch (o_ptr->tval)
	{
	case TV_BULLET:
	case TV_ARROW:
	case TV_BOLT:
	case TV_BOW:
	case TV_CROSSBOW:
	case TV_GUN:
	case TV_MAGICITEM:
	case TV_MAGICWEAPON:
	case TV_OTHERWEAPON:
	case TV_KNIFE:
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
	case TV_RING:
	case TV_RIBBON:
	case TV_AMULET:
	case TV_LITE:
	case TV_FIGURINE:
	case TV_WHISTLE:
		okay = TRUE;
		break;
	}

	/* Auto-inscription/destroy */
	autopick_alter_item(item, (bool)(okay && destroy_feeling));

	/* Something happened */
	return (TRUE);
}


/*
 * If player has inscribed the object with "!!", let him know when it's
 * recharged. -LM-
 */
/*:::��!!�ɂ��ď[�U�ʒm*/
static void recharged_notice(object_type *o_ptr)
{
	char o_name[MAX_NLEN];

	cptr s;

	if (o_ptr->name1 == ART_CLOWNPIECE)
	{
		msg_print("�����ɍĂщ΂��������B");
		p_ptr->update |= PU_TORCH;
		return;
	}

	/* No inscription */
	if (!o_ptr->inscription) return;

	/* Find a '!' */
	s = my_strchr(quark_str(o_ptr->inscription), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if (s[1] == '!')
		{
			if(o_ptr->tval == TV_ABILITY_CARD)
				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX));
			else
				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

			/* Notify the player */
			if(o_ptr->name1 == ART_YAKUZAGUN_1 || o_ptr->name1 == ART_YAKUZAGUN_2)
				msg_format("%s�u���U�����h�X�G�v", o_name);
			else if(o_ptr->tval == TV_GUN && o_ptr->sval != SV_FIRE_GUN_LASER && o_ptr->sval != SV_FIRE_GUN_LUNATIC)
				msg_format("%s�͍đ��U���ꂽ�B", o_name);
			else
				msg_format("%s�͍ď[�U���ꂽ�B", o_name);

			disturb(0, 0);

			/* Done. */
			return;
		}

		/* Keep looking for '!'s */
		s = my_strchr(s + 1, '!');
	}
}

/*:::process_upkeep_with_speed()����Ă΂��B�̂̌��ʔ���*/
///mod140817 �̊֌W�ɋ�V���l�ȊO�̐E���֏�
static void check_music(void)
{
	const magic_type *s_ptr;
	int spell;
	s32b need_mana;
	u32b need_mana_frac;

	/* Music singed by player */
	//if (p_ptr->pclass != CLASS_BARD) return;
	if (!(CHECK_MUSIC_CLASS)) return;

	if (!p_ptr->magic_num1[0] && !p_ptr->magic_num1[1]) return;
	if (p_ptr->anti_magic)
	{
		stop_singing();
		stop_tsukumo_music();
		return;
	}

	spell = p_ptr->magic_num2[0];
	if(p_ptr->pclass == CLASS_BARD)
	{
		msg_print("ERROR:��V���l�͖̉̂�������");
		return;
		//s_ptr = &technic_info[REALM_MUSIC - MIN_TECHNIC][spell];
		//need_mana = mod_need_mana(s_ptr->smana, spell, REALM_MUSIC);
	}
	else if(p_ptr->pclass == CLASS_MYSTIA)
	{
		switch(spell)
		{
		case MUSIC_NEW_MYSTIA_YAZYAKU: //�~�X�e�B�A�@�鐝�̉�
			need_mana = 8; break;
		case MUSIC_NEW_MYSTIA_MYSTERIOUS: //�~�X�e�B�A�@�~�X�e���A�X�\���O
			need_mana = 24; break;
		case MUSIC_NEW_MYSTIA_CHORUSMASTER: //�~�X�e�B�A�@�^�钆�̃R�[���X�}�X�^�[
			need_mana = 36; break;
		case MUSIC_NEW_MYSTIA_HUMAN_CAGE: //v1.1.47 �~�X�e�B�A�@�q���[�}���P�[�W
			need_mana = 30; break;

		default:
			msg_format("ERROR:���̐E�Ƃ̉��yidx(%d)�̌p���R�X�g��check_music()�ɓo�^����Ă��Ȃ�",spell);
			return;

		}
	}
	else if(p_ptr->pclass == CLASS_KYOUKO)
	{
		switch(spell)
		{
		case MUSIC_NEW_KYOUKO_NENBUTSU: //���q�@�����O��
			need_mana = 24; break;
		case MUSIC_NEW_KYOUKO_SCREAM: //���q�@�v���C�}���X�N���[��
			need_mana = 36; break;

		default:
			msg_format("ERROR:���̐E�Ƃ̉��yidx(%d)�̌p���R�X�g��check_music()�ɓo�^����Ă��Ȃ�",spell);
			return;

		}
	}
	//�O�o����1�ÂȂ̂ŉ���
	else if(p_ptr->pclass == CLASS_LUNASA) need_mana = 24;
	else if(p_ptr->pclass == CLASS_MERLIN) need_mana = 36;
	else if(p_ptr->pclass == CLASS_LYRICA) need_mana = 8;
	else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI)
	{
		switch(spell)
		{
		case MUSIC_NEW_TSUKUMO_HEIKE: //����u���Ƃ̑剅��v
			need_mana = 9; break;
		case MUSIC_NEW_TSUKUMO_STORM: //���̃A���T���u��
			need_mana = 24; break;
		case MUSIC_NEW_TSUKUMO_DOUBLESCORE: //�_�u���X�R�A
			need_mana = 48; break;
		case MUSIC_NEW_TSUKUMO_SCOREWEB: //�X�R�A�E�F�u
			need_mana = 36; break;
		case MUSIC_NEW_TSUKUMO_NORIGOTO: //�V�ً̏�
			need_mana = 12; break;
		case MUSIC_NEW_TSUKUMO_JINKIN: //�l�ՂƂ��ɖS��
			need_mana = 24; break;
		case MUSIC_NEW_TSUKUMO_ECHO: //�G�R�[�`�F���o�[
			need_mana = 32; break;
		case MUSIC_NEW_TSUKUMO_ECHO2: //�����̎c��
			need_mana = 8; break;

		default:
			msg_format("ERROR:���̐E�Ƃ̉��yidx(%d)�̌p���R�X�g��check_music()�ɓo�^����Ă��Ȃ�",spell);
			return;
		}
	}
	else if(p_ptr->pclass == CLASS_RAIKO)
	{
		switch(spell)
		{
		case MUSIC_NEW_RAIKO_AYANOTUDUMI: //�A���m�c�d�~
			need_mana = 6; break;
		case MUSIC_NEW_RAIKO_DENDEN: //�{��̃f���f������
			need_mana = 32; break;
		case MUSIC_NEW_RAIKO_PRISTINE: //�v���X�e�B���r�[�g
			need_mana = 48; break;

		default:
			msg_format("ERROR:���̐E�Ƃ̉��yidx(%d)�̌p���R�X�g��check_music()�ɓo�^����Ă��Ȃ�",spell);
			return;

		}
	}
	else if (p_ptr->pclass == CLASS_WAKASAGI)
	{
		need_mana = 10;
	}
	else if (p_ptr->pclass == CLASS_SANNYO)
	{
		switch (spell)
		{
		case MUSIC_NEW_SANNYO_SMOKE:
			need_mana = 0; break;
		case MUSIC_NEW_SANNYO_FEAR:
			need_mana = 6; break;
		case MUSIC_NEW_SANNYO_CONF:
			need_mana = 12; break;
		case MUSIC_NEW_SANNYO_STUN:
			need_mana = 20; break;
		case MUSIC_NEW_SANNYO_SLOW:
			need_mana = 30; break;
		case MUSIC_NEW_SANNYO_MINDBLAST:
			need_mana = 48; break;
		case MUSIC_NEW_SANNYO_BERSERK:
			need_mana = 80; break;

		default:
			msg_format("ERROR:���̐E�Ƃ̉��yidx(%d)�̌p���R�X�g��check_music()�ɓo�^����Ă��Ȃ�", spell);
			return;

		}


	}
	else
	{
		msg_print("ERROR:���̐E�Ƃ̉̌p��������check_music()�ɓo�^����Ă��Ȃ�");
		return;
	}

	need_mana_frac = 0;

	/* Divide by 2 */
	s64b_RSHIFT(need_mana, need_mana_frac, 1);

	if (s64b_cmp(p_ptr->csp, p_ptr->csp_frac, need_mana, need_mana_frac) < 0)
	{
		stop_singing();
		stop_tsukumo_music();
		stop_raiko_music();

		return;
	}
	else
	{
		s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), need_mana, need_mana_frac);

		p_ptr->redraw |= PR_MANA;
		if (p_ptr->magic_num1[1])
		{
			p_ptr->magic_num1[0] = p_ptr->magic_num1[1];
			p_ptr->magic_num1[1] = 0;

			if(p_ptr->pclass == CLASS_SANNYO)
				msg_print("�t���l�ߒ������B");
			else
				msg_print("�̂��ĊJ�����B");

			p_ptr->action = ACTION_SING;

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Redraw map and status bar */
			p_ptr->redraw |= (PR_MAP | PR_STATUS | PR_STATE);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}
	if(p_ptr->pclass == CLASS_BARD)
	{
		/*
		if (p_ptr->spell_exp[spell] < SPELL_EXP_BEGINNER) p_ptr->spell_exp[spell] += 5;
		else if(p_ptr->spell_exp[spell] < SPELL_EXP_SKILLED)	{ if (one_in_(2) && (dun_level > 4) && ((dun_level + 10) > p_ptr->lev)) p_ptr->spell_exp[spell] += 1; }
		else if(p_ptr->spell_exp[spell] < SPELL_EXP_EXPERT)	{ if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && ((dun_level + 5) > s_ptr->slevel)) p_ptr->spell_exp[spell] += 1; }
		else if(p_ptr->spell_exp[spell] < SPELL_EXP_MASTER)	{ if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && (dun_level > s_ptr->slevel)) p_ptr->spell_exp[spell] += 1; }
		*/
	}
	/* Do any effects of continual song */
	//do_spell(REALM_MUSIC, spell, SPELL_CONT);
	do_spell(TV_BOOK_MUSIC, spell, SPELL_CONT);

}


/* Choose one of items that have cursed flag */
static object_type *choose_cursed_obj_name(u32b flag)
{
	int i;
	int choices[INVEN_TOTAL-INVEN_RARM];
	int number = 0;

	/* Paranoia -- Player has no warning-item */
	if (!(p_ptr->cursed & flag)) return NULL;

	/* Search Inventry */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		if(check_invalidate_inventory(i)) continue;

		if (o_ptr->curse_flags & flag)
		{
			choices[number] = i;
			number++;
		}
		else if ((flag == TRC_ADD_L_CURSE) || (flag == TRC_ADD_H_CURSE))
		{
			u32b cf = (flag == TRC_ADD_L_CURSE) ? TR_ADD_L_CURSE : TR_ADD_H_CURSE;
			u32b flgs[TR_FLAG_SIZE];
			object_flags(o_ptr, flgs);
			if (have_flag(flgs, cf))
			{
				choices[number] = i;
				number++;
			}
		}
	}

	/* Choice one of them */
	return (&inventory[choices[randint0(number)]]);
}


//v1.1.86
//�V�_���W�����u�������v�Ŏ_�f���K�v�ȃv���C���[�̔���
static bool check_player_need_oxygen()
{
	bool need_oxygen;

	//���ϐ�������Όċz�ł��邱�Ƃɂ���
	if (p_ptr->resist_water) return FALSE;

	//�푰�Ŋ�{����
	need_oxygen = (RACE_NEED_OXYGEN);

	//�N���X�ŗ�O����
	switch (p_ptr->pclass)
	{
		//����傾����ċz���ĂȂ�����
	case CLASS_FUTO:
	case CLASS_MIKO:
		//�Ȃ����_�f����Ȃ���
	case CLASS_KASEN:
		need_oxygen = FALSE;
		break;

	}

	//�ꎞ�ϐg�ɂ���ĕω��B�ċz���Ȃ����̎푰���ċz���K�v�ɂȂ�����͂��Ȃ�
	switch (p_ptr->mimic_form)
	{
		//�f�[������A���f�b�h�△�����Ȃǂɕϐg������ċz�s�v
	case MIMIC_DEMON:
	case MIMIC_DEMON_LORD:
	case MIMIC_VAMPIRE:
	case MIMIC_SLIME:
	case MIMIC_MIST:
		need_oxygen = FALSE;
		break;
	case MIMIC_METAMORPHOSE_MONST:
	case MIMIC_METAMORPHOSE_GIGANTIC:
	case MIMIC_METAMORPHOSE_NORMAL:
		if (r_info[MON_EXTRA_FIELD].flags3 & (RF3_KWAI | RF3_DEITY | RF3_DEMON | RF3_NONLIVING | RF3_UNDEAD | RF3_HANIWA | RF3_FAIRY)) need_oxygen = FALSE;
		if (r_info[MON_EXTRA_FIELD].flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)) need_oxygen = TRUE;
		//�v���C���[�������Ă鐅�ϐ��ƈ���ă����X�^�[�������Ă鐅�ϐ����Ė��_�f�G���A�Ōċz�ł��銴�������Ȃ����܂��d�l�𑵂��Ă���
		if (r_info[MON_EXTRA_FIELD].flagsr & RFR_RES_WATE) need_oxygen = FALSE;
		break;

	}

	return need_oxygen;

}

//v1.1.85
//�n��Ȃǂ̒n�`�_���[�W��K�p����B�@�ŏ��Ƃ��F�X�ǉ������̂ŕʊ֐��ɕ�����B�Ƃ肠�����_�d�Η�ł݈̂ڂ����B
//feat_flag:FF_LAVA�ȂǑ����������n�`�t���O
//�_���[�W�Ȃ��̂Ƃ�FALSE��Ԃ�
static bool terrain_dam(int feat_flag)
{
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	int dam=0;
	int dam_x100=0;
	cptr name = f_name + f_info[get_feat_mimic(&cave[py][px])].name;
	cptr name_add = "";

	if (p_ptr->is_dead) return FALSE;

	//�n�`�_���[�W��ؖ����̖ʁX
	if (IS_INVULN()) return FALSE;
	if (p_ptr->pclass == CLASS_SUWAKO) return FALSE;
	if (p_ptr->pclass == CLASS_YUMA) return FALSE;
	if (p_ptr->pclass == CLASS_FUTO && p_ptr->lev > 29) return FALSE;
	if (SAKUYA_WORLD) return FALSE;
	if (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_NATURE) return FALSE;


 	switch (feat_flag)
	{
	case FF_LAVA:
		if (p_ptr->immune_fire) return FALSE;

		if (CHECK_FAIRY_TYPE == 5) return FALSE;//�n��Ō��C�ɂȂ�d��

		if (have_flag(f_ptr->flags, FF_DEEP))//�[���n��@���V��1/5�_��
		{
			dam_x100 = 6000 + randint0(4000);
			if (p_ptr->levitation)
			{
				name_add = "�̔M";
				dam_x100 /= 5;
			}
		}
		else  //�󂢗n��@���V�̓_���[�W�Ȃ�
		{
			dam_x100 = 3000 + randint0(2000);
			if (p_ptr->levitation) return FALSE;
		}

		dam_x100 = mod_fire_dam(dam_x100);

		break;
	case FF_ACID_PUDDLE:
		if (p_ptr->immune_acid) return FALSE;

		if (have_flag(f_ptr->flags, FF_DEEP))//�[���_�@���V��1/5�_��
		{
			dam_x100 = 6000 + randint0(4000);
			if (p_ptr->levitation)
			{
				name_add = "�̔�";
				dam_x100 /= 5;
			}
		}
		else  //�󂢎_�@���V�̓_���[�W�Ȃ�
		{
			dam_x100 = 3000 + randint0(2000);
			if (p_ptr->levitation) return FALSE;
		}

		dam_x100 = mod_acid_dam(dam_x100);

		break;

	case FF_COLD_PUDDLE:
		if (p_ptr->immune_cold) return FALSE;

		if (p_ptr->pclass == CLASS_LETTY) return FALSE;
		if (p_ptr->pclass == CLASS_CIRNO) return FALSE;

		//�_���[�W�͗n���_�̔����������V�Ōy������Ȃ�
		if (have_flag(f_ptr->flags, FF_DEEP))
			dam_x100 = 3000 + randint0(2000);
		else 
			dam_x100 = 1500 + randint0(1000);

		dam_x100 = mod_cold_dam(dam_x100);

		break;
	case FF_ELEC_PUDDLE:
		if (p_ptr->immune_elec) return FALSE;
		if (p_ptr->pclass == CLASS_IKU) return FALSE;
		if (p_ptr->pclass == CLASS_RAIKO) return FALSE;

		//�_���[�W�͗n���_�̔����������V�Ōy������Ȃ�
		if (have_flag(f_ptr->flags, FF_DEEP))
			dam_x100 = 3000 + randint0(2000);
		else
			dam_x100 = 1500 + randint0(1000);

		dam_x100 = mod_elec_dam(dam_x100);

		break;

	case FF_POISON_PUDDLE:
		if (p_ptr->pclass == CLASS_EIRIN) return FALSE;

		if (p_ptr->pclass == CLASS_MOMOYO) return FALSE;

		if (p_ptr->pclass == CLASS_MEDICINE)
		{
			set_food(MIN(PY_FOOD_MAX - 1, p_ptr->food + 100));
			return FALSE;
		}

		if (have_flag(f_ptr->flags, FF_DEEP))//�[���ŏ��@���V��1/5�_��
		{
			dam_x100 = 6000 + randint0(4000);
			if (p_ptr->levitation)
			{
				name_add = "�̉�";
				dam_x100 /= 5;
			}
		}
		else  //�󂢓Ł@���V�̓_���[�W�Ȃ�
		{
			dam_x100 = 3000 + randint0(2000);
			if (p_ptr->levitation) return FALSE;
		}

		if (p_ptr->resist_pois) dam_x100 = (dam_x100 + 2) / 3;
		if(IS_OPPOSE_POIS()) dam_x100 = (dam_x100 + 2) / 3;

		//�b������,��
		if ((p_ptr->muta2 & MUT2_ASTHMA))set_asthma(p_ptr->asthma + dam_x100 / 100);
		if(!p_ptr->resist_pois && !IS_OPPOSE_POIS()) set_poisoned(p_ptr->poisoned + randint1(dam_x100/100));

		break;

	default:
		msg_format("ERROR:��`����Ă��Ȃ��n�`(%d)��terrain_dam()���Ă΂ꂽ",feat_flag);
		return FALSE;
	}

	//100�{�_���[�W��ʏ�ɍČv�Z�@100�{�ɂ��Ă������̂͏����v�Z�̂��߂炵�����A��r���Z�q�̖߂�l�𐔒l�Ƃ��Ĉ����Ƃ��Ȃ񂩕|���̂ŏ�������
	//dam = dam_x100 / 100 + (randint0(100) < (dam_x100 % 100));
	dam = dam_x100 / 100;
	if (randint0(100) < (dam_x100 % 100)) dam++;

	if (!dam) return FALSE;

	//�_���[�W�����ƃ��b�Z�[�W�@
	msg_format("%s%s�Ń_���[�W���󂯂��I", name,name_add);
	take_hit(DAMAGE_NOESCAPE, dam, name, -1);


	if (p_ptr->wild_mode)
	{
		/*:::-Hack- �S�̃}�b�v�ړ�����energy_use�����{�ɂȂ��Ăėn��n�`�ɓ������u�ԉ��x���H����Ď��ɂ��˂��s���R�Ȃ̂Œ����Ă���*/
		energy_use = MIN(energy_use, 100);
		change_wild_mode();
	}

	return TRUE;

}

/*
 * Handle timed damage and regeneration every 10 game turns
 */
/*:::���ւ̃^�[���_���[�W�ƍĐ�*/
static void process_world_aux_hp_and_sp(void)
{
	feature_type *f_ptr = &f_info[cave[py][px].feat];
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;

	/* Default regeneration */
	int regen_amount = PY_REGEN_NORMAL;

	//v1.1.59 �T�j�[�~���N�������������ē�����̂ɒ~����
	if (!p_ptr->inside_arena && !p_ptr->inside_battle && (p_ptr->pclass == CLASS_SUNNY || p_ptr->pclass == CLASS_3_FAIRIES))
	{
		if (is_daytime() && !dun_level)
			sunny_charge_sunlight(1);
		else
			sunny_charge_sunlight(-1);

	}

	/*** Damage over Time ***/


	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > HINA_YAKU_LIMIT3 && one_in_(6) && !IS_INVULN())
	{
		take_hit(DAMAGE_NOESCAPE, 1, "��̕��S", -1);
	}


	/* Take damage from poison */
	if (p_ptr->poisoned && !IS_INVULN())
	{
		/* Take damage */
#ifdef JP
		take_hit(DAMAGE_NOESCAPE, 1, "��", -1);
#else
		take_hit(DAMAGE_NOESCAPE, 1, "poison", -1);
#endif

	}
	///sys ���̃_���[�W�Ń��b�Z�[�W�o���ɂ͂����ҏW
	/* Take damage from cuts */
	if (p_ptr->cut && !IS_INVULN())
	{
		int dam;

		/* Mortal wound or Deep Gash */
		if (p_ptr->cut > 1000)
		{
			dam = 200;
		}

		else if (p_ptr->cut > 200)
		{
			dam = 80;
		}

		/* Severe cut */
		else if (p_ptr->cut > 100)
		{
			dam = 32;
		}

		else if (p_ptr->cut > 50)
		{
			dam = 16;
		}

		else if (p_ptr->cut > 25)
		{
			dam = 7;
		}

		else if (p_ptr->cut > 10)
		{
			dam = 3;
		}

		/* Other cuts */
		else
		{
			dam = 1;
		}

		if(p_ptr->pclass == CLASS_HINA)
		{
			hina_gain_yaku(-dam);
		}
		else
		{
			/* Take damage */
			///msg131221
			take_hit(DAMAGE_NOESCAPE, dam, "����̒ɂ�", -1);
		}


	}


	/* (Vampires) Take damage from sunlight */
	///sysdel131221 �z���S�̗z���_���[�W�𖳂��ɂ���
#if 0
	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
	{
		if (!dun_level && !p_ptr->resist_lite && !IS_INVULN() && is_daytime())
		{
			if ((cave[py][px].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
			{
				/* Take damage */
#ifdef JP
msg_print("���������Ȃ��̃A���f�b�h�̓��̂��Ă��ł������I");
take_hit(DAMAGE_NOESCAPE, 1, "����", -1);
#else
				msg_print("The sun's rays scorch your undead flesh!");
				take_hit(DAMAGE_NOESCAPE, 1, "sunlight", -1);
#endif

				cave_no_regen = TRUE;
			}
		}

		if (inventory[INVEN_LITE].tval && (inventory[INVEN_LITE].name2 != EGO_LITE_DARKNESS) &&
		    !p_ptr->resist_lite)
		{
			object_type * o_ptr = &inventory[INVEN_LITE];
			char o_name [MAX_NLEN];
			char ouch [MAX_NLEN+40];

			/* Get an object description */
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
msg_format("%s�����Ȃ��̃A���f�b�h�̓��̂��Ă��ł������I", o_name);
#else
			msg_format("The %s scorches your undead flesh!", o_name);
#endif


			cave_no_regen = TRUE;

			/* Get an object description */
			object_desc(o_name, o_ptr, OD_NAME_ONLY);

#ifdef JP
sprintf(ouch, "%s�𑕔������_���[�W", o_name);
#else
			sprintf(ouch, "wielding %s", o_name);
#endif

			if (!IS_INVULN()) take_hit(DAMAGE_NOESCAPE, 1, ouch, -1);
		}
	}
#endif

	//v1.1.64 �V�n�`�u�j�̂ނ���v�@���V�����ɓ���ƃ_���[�W�ƕ���
	if (have_flag(f_ptr->flags, FF_NEEDLE) && !IS_INVULN() && !p_ptr->levitation)
	{
		int damage = damroll(4, 10);
		msg_print("�j���h�������I");
		take_hit(DAMAGE_NOESCAPE, damage, format("%s�̏�ɏ�����_���[�W", f_name + f_info[get_feat_mimic(&cave[py][px])].name), -1);
		set_cut(damage / 2);

	}


	//v1.1.85 �n�`�ǉ��B�_���[�W������ʊ֐��Ɉړ�����
	if (have_flag(f_ptr->flags, FF_LAVA) && terrain_dam(FF_LAVA)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_COLD_PUDDLE) && terrain_dam(FF_COLD_PUDDLE)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_ELEC_PUDDLE) && terrain_dam(FF_ELEC_PUDDLE)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_ACID_PUDDLE) && terrain_dam(FF_ACID_PUDDLE)) cave_no_regen = TRUE;
	if (have_flag(f_ptr->flags, FF_POISON_PUDDLE) && terrain_dam(FF_POISON_PUDDLE)) cave_no_regen = TRUE;
#if 0
	///sys �n��_���[�W�@���b�Z�[�W����悤���H
	if (have_flag(f_ptr->flags, FF_LAVA) && !IS_INVULN() && !p_ptr->immune_fire)
	{
		int damage = 0;

		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			damage = 6000 + randint0(4000);
		}
		else if (!p_ptr->levitation)
		{
			damage = 3000 + randint0(2000);
		}

		if(CHECK_FAIRY_TYPE == 5) damage = 0;
		else if(p_ptr->pclass == CLASS_SUWAKO) damage = 0;
		else if(p_ptr->pclass == CLASS_FUTO && p_ptr->lev > 29) damage = 0;
		else if(SAKUYA_WORLD) damage = 0;


		if (damage)
		{
			///race�@�n��_���[�W����
			/*
			if (prace_is_(RACE_ENT)) damage += damage / 3;
			if (p_ptr->resist_fire) damage = damage / 3;
			if (IS_OPPOSE_FIRE()) damage = damage / 3;
			*/
			///mod140302 ���f��_�A�ϐ���������
			damage = mod_fire_dam(damage);

			if (p_ptr->levitation) damage = damage / 5;

			damage = damage / 100 + (randint0(100) < (damage % 100));

			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("�M�ŉΏ������I");
				take_hit(DAMAGE_NOESCAPE, damage, format("%s�̏�ɕ��V�����_���[�W", f_name + f_info[get_feat_mimic(&cave[py][px])].name), -1);
#else
				msg_print("The heat burns you!");
				take_hit(DAMAGE_NOESCAPE, damage, format("flying over %s", f_name + f_info[get_feat_mimic(&cave[py][px])].name), -1);
#endif
			}
			else
			{
				cptr name = f_name + f_info[get_feat_mimic(&cave[py][px])].name;
#ifdef JP
				msg_format("%s�ŉΏ������I", name);
#else
				msg_format("The %s burns you!", name);
#endif
				take_hit(DAMAGE_NOESCAPE, damage, name, -1);
			}
			///mod140719 
			if (p_ptr->wild_mode)
			{
				/*:::-Hack- �S�̃}�b�v�ړ�����energy_use�����{�ɂȂ��Ăėn��n�`�ɓ������u�ԉ��x���H����Ď��ɂ��˂��s���R�Ȃ̂Œ����Ă���*/
				energy_use = MIN(energy_use, 100);
				change_wild_mode();
			}
			cave_no_regen = TRUE;
		}
	}
#endif

	if (p_ptr->drowning)
	{
		int dam;
		if (prace_is_(RACE_VAMPIRE)) dam = 30 + randint1(20);
		else dam = randint1(20);

		take_hit(DAMAGE_NOESCAPE, dam, "�M��", -1);
		cave_no_regen = TRUE;
	}
	//v1.1.86 �������[���ł͎_�f�̕K�v�ȃv���[���[�̓_���[�W
	else if (dungeon_type == DUNGEON_KOURYUU && check_player_need_oxygen())
	{
		if (dun_level <= 35)
		{
			if (one_in_(100)) msg_print("�̒���������Ȃ��B��C�������̂��낤���B");
			cave_no_regen = TRUE;

		}
		else if (dun_level > 35 && dun_level <= 45)
		{
			if (one_in_(20)) msg_print("�����������ꂵ��...");
			cave_no_regen = TRUE;
			take_hit(DAMAGE_NOESCAPE, 1, "�_��", -1);

		}
		else if (dun_level > 45 && dun_level <= 55)
		{
			if(one_in_(5) && PLAYER_CAN_COMMAND)msg_print("�܂Ƃ��Ɍċz���ł��Ȃ��B�����l�����Ȃ��Ȃ��Ă����B");

			cave_no_regen = TRUE;
			if(one_in_(4))set_stun(p_ptr->stun + 10 + randint1(10));
			if (one_in_(3)) do_dec_stat(A_INT);
			if (!p_ptr->resist_conf)set_confused(p_ptr->confused + randint1(5));

			take_hit(DAMAGE_NOESCAPE, 5 + randint1(5), "�_��", -1);

		}
		else
		{
			if (PLAYER_CAN_COMMAND)msg_print("�ӎ����r�؂ꂻ�����I�����͐l����������ׂ��ꏊ�ł͂Ȃ��I");
			cave_no_regen = TRUE;
			set_stun(p_ptr->stun + 20 + randint1(20));
			do_dec_stat(A_INT);
			if (!p_ptr->resist_conf)set_confused(p_ptr->confused + 10 + randint1(10));
			if (!p_ptr->resist_chaos)set_image(p_ptr->image + 10 + randint1(10));
			take_hit(DAMAGE_NOESCAPE, 10 + randint1(10), "�_��", -1);

		}

	}


#if 0
	if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
	    !p_ptr->levitation && !p_ptr->can_swim)
	{
		if (p_ptr->total_weight > weight_limit())
		{
			/* Take damage */
#ifdef JP

			//msg_print("�M��Ă���I");
			take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->lev), "�M��", -1);
#else
			msg_print("You are drowning!");
			take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->lev), "drowning", -1);
#endif

			cave_no_regen = TRUE;
		}
	}
#endif

	if (p_ptr->riding)
	{
		int damage;
		//v1.1.78 ��d�ϐ��̂Ƃ��I�[���_���[�W���󂯂Ȃ��悤�ύX
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_FIRE) && !p_ptr->immune_fire && !(IS_OPPOSE_FIRE()))
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			/*
			if (prace_is_(RACE_ENT)) damage += damage / 3;
			if (p_ptr->resist_fire) damage = damage / 3;
			if (IS_OPPOSE_FIRE()) damage = damage / 3;
			*/
			///mod140302 ���f��_�A�ϐ���������
			damage = mod_fire_dam(damage);

			if (CLASS_RIDING_BACKDANCE) damage /= 3;

	#ifdef JP
msg_print("�M���I");
take_hit(DAMAGE_NOESCAPE, damage, "���̃I�[��", -1);
#else
			msg_print("It's hot!");
			take_hit(DAMAGE_NOESCAPE, damage, "Fire aura", -1);
#endif
		}
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_ELEC) && !p_ptr->immune_elec && !(IS_OPPOSE_ELEC()))
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			/*
			if (prace_is_(RACE_KAPPA)) damage += damage / 3;
			if (p_ptr->resist_elec) damage = damage / 3;
			if (IS_OPPOSE_ELEC()) damage = damage / 3;
			*/
			///mod140302 ���f��_�A�ϐ���������
			damage = mod_elec_dam(damage);

			if (CLASS_RIDING_BACKDANCE) damage /= 3;

#ifdef JP
msg_print("�ɂ��I");
take_hit(DAMAGE_NOESCAPE, damage, "�d�C�̃I�[��", -1);
#else
			msg_print("It hurts!");
			take_hit(DAMAGE_NOESCAPE, damage, "Elec aura", -1);
#endif
		}

///mod131231 �����X�^�[�t���O�ύX
//		if ((r_info[m_list[p_ptr->riding].r_idx].flags3 & RF3_AURA_COLD) && !p_ptr->immune_cold)
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_COLD) && !p_ptr->immune_cold && !(IS_OPPOSE_COLD()))
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			/*
			if (p_ptr->resist_cold) damage = damage / 3;
			if (IS_OPPOSE_COLD()) damage = damage / 3;
			*/
			///mod140302 ���f��_�A�ϐ���������
			damage = mod_cold_dam(damage);

			if (CLASS_RIDING_BACKDANCE) damage /= 3;

#ifdef JP
msg_print("�₽���I");
take_hit(DAMAGE_NOESCAPE, damage, "��C�̃I�[��", -1);
#else
			msg_print("It's cold!");
			take_hit(DAMAGE_NOESCAPE, damage, "Cold aura", -1);
#endif
		}
	}

	/* Spectres -- take damage when moving through walls */
	/*
	 * Added: ANYBODY takes damage if inside through walls
	 * without wraith form -- NOTE: Spectres will never be
	 * reduced below 0 hp by being inside a stone wall; others
	 * WILL BE!
	 */
	if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
	{
		if (!IS_INVULN() && !p_ptr->wraith_form && !p_ptr->kabenuke 
			&& CHECK_FAIRY_TYPE != 41
			&& !(IS_METAMORPHOSIS && (r_info[MON_EXTRA_FIELD].flags2 & RF2_PASS_WALL))
			&& !((p_ptr->pclass == CLASS_SEIGA) && (p_ptr->lev > 39))
			&& !((p_ptr->pclass == CLASS_OKINA) && (have_flag(f_ptr->flags, FF_DOOR)))
			&& !((p_ptr->pclass == CLASS_YUKARI) && (p_ptr->lev > 39))
		    && ((p_ptr->chp > (p_ptr->lev / 5)) || !p_ptr->pass_wall))
		{
			cptr dam_desc;

			cave_no_regen = TRUE;


			if (p_ptr->pass_wall)
			{
#ifdef JP
				msg_print("�̂̕��q�����������C������I");
				dam_desc = "���x";
#else
				msg_print("Your molecules feel disrupted!");
				dam_desc = "density";
#endif
			}
			else
			{
#ifdef JP
				msg_print("���ꂽ��ɉ����ׂ��ꂽ�I");
				dam_desc = "�d����";
#else
				msg_print("You are being crushed!");
				dam_desc = "solid rock";
#endif
			}

			take_hit(DAMAGE_NOESCAPE, 1 + (p_ptr->lev / 5), dam_desc, -1);
		}
	}


	/*** handle regeneration ***/

	/* Getting Weak */
	if (p_ptr->food < PY_FOOD_WEAK)
	{
		/* Lower regeneration */
		if (p_ptr->food < PY_FOOD_STARVE)
		{
			regen_amount = 0;
		}
		else if (p_ptr->food < PY_FOOD_FAINT)
		{
			regen_amount = PY_REGEN_FAINT;
		}
		else
		{
			regen_amount = PY_REGEN_WEAK;
		}
	}

	//v1.1.41 ���Ɨ��T���u�������d���ďr�q�ɓ����̂�����v�Ƃ��ɗx���Ă���Ǝ��R�񕜑��x�啝�ቺ
	if (CLASS_RIDING_BACKDANCE && (p_ptr->pclass == CLASS_MAI || p_ptr->pclass == CLASS_SATONO) 
		&& heavy_armor() && p_ptr->riding)
	{
		regen_amount /= 3;
	}

	/* Are we walking the pattern? */
	if (pattern_effect())
	{
		cave_no_regen = TRUE;
	}
	else
	{
		/* Regeneration ability */
		if (p_ptr->regenerate)
		{
			regen_amount = regen_amount * 2;
		}
		if (p_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
		{
			regen_amount /= 2;
		}
		if (p_ptr->cursed & TRC_SLOW_REGEN)
		{
			regen_amount /= 5;
		}
	}


	/* Searching or Resting */
	if ((p_ptr->action == ACTION_SEARCH) || (p_ptr->action == ACTION_REST))
	{
		regen_amount = regen_amount * 2;
		//����͋x�e���̉񕜂�����
		if(p_ptr->pclass == CLASS_MEIRIN && (p_ptr->action == ACTION_REST)) regen_amount = regen_amount * 2;
	}

	upkeep_factor = calculate_upkeep();

	/* No regeneration while special action */
	if ((p_ptr->action == ACTION_LEARN) ||
	    (p_ptr->action == ACTION_HAYAGAKE) ||
	    (p_ptr->special_defense & KATA_KOUKIJIN))
	{
		upkeep_factor += 100;
	}

	/* Regenerate the mana */
	regenmana(upkeep_factor, regen_amount);

	//v1.1.15 �w�J�[�e�B�A�̑̂�HP��
	if(p_ptr->pclass == CLASS_HECATIA)
	{
		int i;
		for(i=0;i<6;i++)
		{
			p_ptr->magic_num1[i] += p_ptr->lev * 4;
			if(p_ptr->magic_num1[i] > 10000) p_ptr->magic_num1[i] = 10000;
		}
	}


	/* Recharge magic eater's power */
	///class�@������p�m�̖��@���R��
	if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
	{
		regenmagic(regen_amount);
	}

	if ((p_ptr->csp == 0) && (p_ptr->csp_frac == 0))
	{
		/*:::100�𒴂���Ƃ��͏�Ƀy�b�g������*/
		while (upkeep_factor > 100)
		{
#ifdef JP
			///msg131221
			msg_print("����Ȃɑ����̔z���𐧌�ł��Ȃ��I");
#else
			msg_print("Too many pets to control at once!");
#endif
			msg_print(NULL);
			do_cmd_pet_dismiss();

			upkeep_factor = calculate_upkeep();

#ifdef JP
			msg_format("�ێ��l�o�� %d%%", upkeep_factor);
#else
			msg_format("Upkeep: %d%% mana.", upkeep_factor);
#endif
			msg_print(NULL);
		}
	}

	/* Poisoned or cut yields no healing */
	if (p_ptr->poisoned) regen_amount = 0;
	if (p_ptr->cut) regen_amount = 0;

	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > HINA_YAKU_LIMIT3) regen_amount = 0;

	/* Special floor -- Pattern, in a wall -- yields no healing */
	if (cave_no_regen) regen_amount = 0;

	regen_amount = (regen_amount * mutant_regenerate_mod) / 100;

	/* Regenerate Hit Points if needed */
	if ((p_ptr->chp < p_ptr->mhp) && !cave_no_regen)
	{
		regenhp(regen_amount);
	}
}


/*
 * Handle timeout every 10 game turns
 */
/*:::�X�e�[�^�X�ُ��ꎞ���ʂȂǂ̗L�����ԃJ�E���g*/
static void process_world_aux_timeout(void)
{
	const int dec_count = (easy_band ? 2 : 1);
	int i;

	/*** Timeout Various Things ***/
	//�X�^�[�T�t�@�C�A�̔��e�N�� �A�C�e���J�[�h���g������EX���[�h�ł��Ώۂ�
	//v1.1.14 �T�O�����ǉ�
	//v1.1.21 �����ʓ|������S���J�E���g���Ă��܂��B����Ƃ��ɂ�����Əd�����������
	//if(EXTRA_MODE || p_ptr->pclass == CLASS_SAGUME || p_ptr->pclass == CLASS_STAR || p_ptr->pclass == CLASS_3_FAIRIES)
	{
		int x,y;
		for (y = 1; y < cur_hgt - 1; y++)
		{
			for (x = 1; x < cur_wid - 1; x++)
			{
				if(have_flag(f_info[cave[y][x].mimic].flags, FF_BOMB)) bomb_count(y,x,-1);

			}
		}
	}

	//v1.1.57 ���ꕐ��u�����̌��v�̃^�C���J�E���g�Ə��ŏ���
	//�J�E���g�̋L�^��xtra4���g�����Ƃɂ����B
	for (i = INVEN_RARM; i <= INVEN_LARM; i++)
	{
		object_type *o_ptr = &inventory[i];

		if (o_ptr->tval != TV_SWORD || o_ptr->sval != SV_WEAPON_SEVEN_STAR) continue;

		o_ptr->xtra4++;

		if (o_ptr->xtra4 > 25)
		{
			msg_print("�����̌����������B");
			inven_item_increase(i, -1);
			inven_item_optimize(i);
			kamaenaoshi(i);
			p_ptr->update |= (PU_BONUS | PU_TORCH);
			p_ptr->redraw |= (PR_EQUIPPY | PR_HP | PR_MANA);
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			update_stuff();
			disturb(1, 1);
		}

	}

	/* Mimic */
	if (p_ptr->tim_mimic)
	{
		(void)set_mimic(p_ptr->tim_mimic - 1, p_ptr->mimic_form, TRUE);
	}

	/* Hack -- Hallucinating */
	if (p_ptr->image)
	{
		(void)set_image(p_ptr->image - dec_count);
	}

	/* Blindness */
	if (p_ptr->blind)
	{
		(void)set_blind(p_ptr->blind - dec_count);
	}

	/* Times see-invisible */
	if (p_ptr->tim_invis)
	{
		(void)set_tim_invis(p_ptr->tim_invis - 1, TRUE);
	}

	/*:::�J�I�X�p�g�������炷�łɕ�V����������J�E���g�����ɖ߂�*/
	if (multi_rew)
	{
		multi_rew = FALSE;
	}

	/* Timed esp */
	if (p_ptr->tim_esp)
	{
		(void)set_tim_esp(p_ptr->tim_esp - 1, TRUE);
	}

	/* Timed temporary elemental brands. -LM- */
	if (p_ptr->ele_attack)
	{
		p_ptr->ele_attack--;

		/* Clear all temporary elemental brands. */
		if (!p_ptr->ele_attack) set_ele_attack(0, 0);
	}

	/* Timed temporary elemental immune. -LM- */
	if (p_ptr->ele_immune)
	{
		p_ptr->ele_immune--;

		/* Clear all temporary elemental brands. */
		if (!p_ptr->ele_immune) set_ele_immune(0, 0);
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra)
	{
		(void)set_tim_infra(p_ptr->tim_infra - 1, TRUE);
	}

	/* Timed stealth */
	if (p_ptr->tim_stealth)
	{
		(void)set_tim_stealth(p_ptr->tim_stealth - 1, TRUE);
	}

	/* Timed levitation */
	if (p_ptr->tim_levitation)
	{
		(void)set_tim_levitation(p_ptr->tim_levitation - 1, TRUE);
	}

	///mod140813 �����ړ��@�s���񐔃J�E���g�������̂�ʏ�̃^�C���A�b�v�����ɕύX����
	if (p_ptr->lightspeed)
	{
		(void)set_lightspeed(p_ptr->lightspeed - 1, TRUE);
	}

	/* Timed sh_touki */
	if (p_ptr->tim_sh_touki)
	{
		(void)set_tim_sh_touki(p_ptr->tim_sh_touki - 1, TRUE);
	}

	/* Timed sh_fire */
	if (p_ptr->tim_sh_fire)
	{
		(void)set_tim_sh_fire(p_ptr->tim_sh_fire - 1, TRUE);
	}

	/* Timed sh_holy */
	if (p_ptr->tim_sh_holy)
	{
		(void)set_tim_sh_holy(p_ptr->tim_sh_holy - 1, TRUE);
	}

	/* Timed eyeeye */
	if (p_ptr->tim_eyeeye)
	{
		(void)set_tim_eyeeye(p_ptr->tim_eyeeye - 1, TRUE);
	}

	if (p_ptr->tim_res_insanity)
	{
		(void)set_tim_res_insanity(p_ptr->tim_res_insanity - 1, TRUE);
	}

	/* Timed resist-magic */
	if (p_ptr->resist_magic)
	{
		(void)set_resist_magic(p_ptr->resist_magic - 1, TRUE);
	}

	/* Timed regeneration */
	if (p_ptr->tim_regen)
	{
		(void)set_tim_regen(p_ptr->tim_regen - 1, TRUE);
	}
	/*:::�_�~�낵�p�J�E���g�_�E��*/
	if (p_ptr->kamioroshi_count)
	{
		p_ptr->kamioroshi_count--;
		if (!p_ptr->kamioroshi_count) set_kamioroshi(0, 0);
	}
	/* Timed resist nether */
	if (p_ptr->tim_res_nether)
	{
		(void)set_tim_res_nether(p_ptr->tim_res_nether - 1, TRUE);
	}

	if (p_ptr->tim_res_dark)
	{
		(void)set_tim_res_dark(p_ptr->tim_res_dark - 1, TRUE);
	}
	for(i=0;i<TIM_GENERAL_MAX;i++)
	{
		if (p_ptr->tim_general[i]) (void)set_tim_general(p_ptr->tim_general[i] - 1, TRUE, i, 0);
	}

	///mod140325 �ꎞ�I�\�͏㏸
	for(i=0;i<6;i++)
	{
		if (p_ptr->tim_addstat_count[i]) (void)set_tim_addstat(i,0,p_ptr->tim_addstat_count[i] - 1, TRUE);
	}

	if (p_ptr->tim_rokuro_head_search)
	{
		rokuro_head_search_item(p_ptr->tim_rokuro_head_search - 1, FALSE);
	}
	if (p_ptr->tim_hirari_nuno)
	{
		set_hirarinuno_card(p_ptr->tim_hirari_nuno - 1, TRUE);

	}


	/*::: �K�^�̃��J�j�Y�� */
	if (p_ptr->lucky)
	{
		(void)set_tim_lucky(p_ptr->lucky - 1, TRUE);
	}
	/*::: �����\�m */
	if (p_ptr->foresight)
	{
		(void)set_foresight(p_ptr->foresight - 1, TRUE);
	}
	/*::: ���ҏp */
	if (p_ptr->deportation)
	{
		(void)set_deportation(p_ptr->deportation - 1, TRUE);
	}

	/*::: �܋��� */
	if (p_ptr->clawextend)
	{
		(void)set_clawextend(p_ptr->clawextend - 1, TRUE);
	}

	/*::: Timed speedster*/
	if (p_ptr->tim_speedster)
	{
		(void)set_tim_speedster(p_ptr->tim_speedster - 1, TRUE);
	}
	/*::: Timed resist water */
	if (p_ptr->tim_res_water)
	{
		(void)set_tim_res_water(p_ptr->tim_res_water - 1, TRUE);
	}
	/*::: Timed resist chaos */
	if (p_ptr->tim_res_chaos)
	{
		(void)set_tim_res_chaos(p_ptr->tim_res_chaos - 1, TRUE);
	}
	/*::: ���[�_�[�Z���X */
	if (p_ptr->radar_sense)
	{
		(void)set_radar_sense(p_ptr->radar_sense - 1, TRUE);
	}

	if (p_ptr->tim_unite_darkness)
	{
		(void)set_tim_unite_darkness(p_ptr->tim_unite_darkness - 1, TRUE);
	}

	/*::: �ꎞ���B�� */
	if (p_ptr->tim_superstealth)
	{
		(void)set_tim_superstealth(p_ptr->tim_superstealth - 1, TRUE, 0);
	}

	//v1.1.56
	if (p_ptr->tim_spellcard_count)
	{
		set_tim_spellcard_effect(p_ptr->tim_spellcard_count - 1, TRUE, 0);
	}

	/* Timed resist time */
	if (p_ptr->tim_res_time)
	{
		(void)set_tim_res_time(p_ptr->tim_res_time - 1, TRUE);
	}
	///mod140326
	/* Reactive Heal */
	if (p_ptr->reactive_heal)
	{
		(void)set_reactive_heal(p_ptr->reactive_heal - 1, TRUE);
	}
	/* Timed reflect */
	if (p_ptr->tim_reflect)
	{
		(void)set_tim_reflect(p_ptr->tim_reflect - 1, TRUE);
	}
	/* �O�� */
	if (p_ptr->nennbaku)
	{
		(void)set_nennbaku(p_ptr->nennbaku - 1, TRUE);
	}
	//v1.1.93 �ꎞ����
	if (p_ptr->tim_aggravation)
	{
		(void)set_tim_aggravation(p_ptr->tim_aggravation - 1, TRUE);
	}



	/* Multi-shadow */
	if (p_ptr->multishadow)
	{
		(void)set_multishadow(p_ptr->multishadow - 1, TRUE);
	}

	/* Timed Robe of dust */
	if (p_ptr->dustrobe)
	{
		(void)set_dustrobe(p_ptr->dustrobe - 1, TRUE);
	}

	/* Timed infra-vision */
	if (p_ptr->kabenuke)
	{
		(void)set_kabenuke(p_ptr->kabenuke - 1, TRUE);
	}

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		(void)set_paralyzed(p_ptr->paralyzed - dec_count);
	}

	/* Confusion */
	if (p_ptr->confused)
	{
		(void)set_confused(p_ptr->confused - dec_count);
	}

	/* Afraid */
	if (p_ptr->afraid)
	{
		(void)set_afraid(p_ptr->afraid - dec_count);
	}

	/* Fast */
	if (p_ptr->fast)
	{
		(void)set_fast(p_ptr->fast - 1, TRUE);
	}

	/* Slow */
	if (p_ptr->slow)
	{
		(void)set_slow(p_ptr->slow - dec_count, TRUE);
	}

	/* Protection from evil */
	if (p_ptr->protevil)
	{
		(void)set_protevil(p_ptr->protevil - 1, TRUE);
	}

	/* Invulnerability */
	if (p_ptr->invuln)
	{
		(void)set_invuln(p_ptr->invuln - 1, TRUE);
	}

	/* Wraith form */
	if (p_ptr->wraith_form)
	{
		(void)set_wraith_form(p_ptr->wraith_form - 1, TRUE);
	}

	/* Heroism */
	if (p_ptr->hero)
	{
		(void)set_hero(p_ptr->hero - 1, TRUE);
	}

	/* Super Heroism */
	if (p_ptr->shero)
	{
		(void)set_shero(p_ptr->shero - 1, TRUE);
	}

	/* Blessed */
	if (p_ptr->blessed)
	{
		(void)set_blessed(p_ptr->blessed - 1, TRUE);
	}

	/* Shield */
	if (p_ptr->shield)
	{
		(void)set_shield(p_ptr->shield - 1, TRUE);
	}

	/* Tsubureru */
	if (p_ptr->tim_sh_death)
	{
		(void)set_tim_sh_death(p_ptr->tim_sh_death - 1, TRUE);
	}

	/* Magicdef */
	if (p_ptr->magicdef)
	{
		(void)set_magicdef(p_ptr->magicdef - 1, TRUE);
	}

	/* Tsuyoshi */
	if (p_ptr->tsuyoshi)
	{
		(void)set_tsuyoshi(p_ptr->tsuyoshi - 1, TRUE);
	}

	/* Oppose Acid */
	if (p_ptr->oppose_acid)
	{
		(void)set_oppose_acid(p_ptr->oppose_acid - 1, TRUE);
	}

	/* Oppose Lightning */
	if (p_ptr->oppose_elec)
	{
		(void)set_oppose_elec(p_ptr->oppose_elec - 1, TRUE);
	}

	/* Oppose Fire */
	if (p_ptr->oppose_fire)
	{
		(void)set_oppose_fire(p_ptr->oppose_fire - 1, TRUE);
	}

	/* Oppose Cold */
	if (p_ptr->oppose_cold)
	{
		(void)set_oppose_cold(p_ptr->oppose_cold - 1, TRUE);
	}

	/* Oppose Poison */
	if (p_ptr->oppose_pois)
	{
		(void)set_oppose_pois(p_ptr->oppose_pois - 1, TRUE);
	}

	if (p_ptr->ult_res)
	{
		(void)set_ultimate_res(p_ptr->ult_res - 1, TRUE);
	}

	/*** Poison and Stun and Cut ***/


	/* Poison */
	if (p_ptr->poisoned)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_poisoned(p_ptr->poisoned - adjust);
	}

	//v1.1.66 �X�e�[�^�X�ُ�u�j���v
	if (REF_BROKEN_VAL)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;
		set_broken(-(adjust)); //set_stun()�ȂǂƈႢ�����l�𒼐ړn��

	}

	/* Stun */
	if (p_ptr->stun)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_stun(p_ptr->stun - adjust);
	}

	/* Cut */
	if (p_ptr->cut)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Hack -- Truly "mortal" wound */
		if (p_ptr->cut > 1000) adjust = 0;

		/* Apply some healing */
		(void)set_cut(p_ptr->cut - adjust);
	}

	///mod140326 �A���R�[�������l�Ɏ�������悤�ɂ���
	if (p_ptr->alcohol)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;
		///mod150131 �ӎ��s���Ď��A������Ɖ񕜑���
		//v1.1.12 �����Ƒ���
		//v1.1.78 �����5�{ *10��*50
		if(p_ptr->alcohol >= DRANK_4)
		{
			adjust += 5;
			adjust *= 50;
		}
		(void)set_alcohol(p_ptr->alcohol - adjust);
	}
	///mod140813 �b�������l�Ɏ�������悤�ɂ���
	if (p_ptr->asthma)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;
		(void)set_asthma(p_ptr->asthma - adjust);
	}


}


/*
 * Handle burning fuel every 10 game turns
 */
/*:::�����̔R����������*/
static void process_world_aux_light(void)
{
	/* Check for light being wielded */
	object_type *o_ptr = &inventory[INVEN_LITE];

	if(check_invalidate_inventory(INVEN_LITE)) return;

	/* Burn some fuel in the current lite */
	if (o_ptr->tval == TV_LITE)
	{
		/* Hack -- Use some fuel (except on artifacts) */
		if (!(object_is_fixed_artifact(o_ptr) || o_ptr->sval == SV_LITE_FEANOR) && (o_ptr->xtra4 > 0))
		{
			/* Decrease life-span */
			if (o_ptr->name2 == EGO_LITE_LONG)
			{
				if (turn % (TURNS_PER_TICK*2)) o_ptr->xtra4--;
			}
			else o_ptr->xtra4--;

			/* Notice interesting fuel steps */
			notice_lite_change(o_ptr);
		}
	}
}


/*
 * Handle mutation effects once every 10 game turns
 */
/*:::�ˑR�ψق̃����_������ muta2�̂�*/
static void process_world_aux_mutation(void)
{
	/* No mutation with effects */
	if (!p_ptr->muta2) return;

	/* No effect on monster arena */
	if (p_ptr->inside_battle) return;

	/* No effect on the global map */
	if (p_ptr->wild_mode) return;


	if ((p_ptr->muta2 & MUT2_BERS_RAGE) && one_in_(3000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("�ˑR���Ȃ��̌�����������I");
#else
		msg_print("RAAAAGHH!");
		msg_print("You feel a fit of rage coming over you!");
#endif

		(void)set_shero(10 + randint1(p_ptr->lev), FALSE);
		(void)set_afraid(0);
	}

	if ((p_ptr->muta2 & MUT2_COWARDICE) && (randint1(3000) == 13))
	{
		if (!p_ptr->resist_fear || IS_VULN_FEAR)
		{
			disturb(0, 1);
#ifdef JP
			msg_print("�ƂĂ��Â�... �ƂĂ������I");
#else
			msg_print("It's so dark... so scary!");
#endif

			set_afraid(p_ptr->afraid + 13 + randint1(26));
		}
	}

	if ((p_ptr->muta2 & MUT2_RTELEPORT) && (randint1(5000) == 88)  || p_ptr->pclass == CLASS_KOISHI && one_in_(2000))
	{
		///mod140121 �����_���e���|�ψف@�ϐ��h��s�� ��m���ŉ����E���Ă���

			disturb(1, 1);

			teleport_player(30 + randint0(170), TELEPORT_NONMAGICAL);
			msg_print("�c���̊Ԃɂ��m��Ȃ��ꏊ�ɂ����B");
			msg_print(NULL);
			msg_print(NULL);

			if( (p_ptr->lev + 40 < dun_level) && weird_luck())
			{
				msg_print("��������Ɏ����Ă����B");
				acquirement(py, px, 1, TRUE, FALSE);
			}

	}

	if ((p_ptr->muta2 & MUT2_HALLU) && (randint1(6400) == 42))
	{
		if (!p_ptr->resist_chaos)
		{
			disturb(0, 1);
			p_ptr->redraw |= PR_EXTRA;
			(void)set_image(p_ptr->image + randint0(50) + 20);
		}
	}

	if ((p_ptr->muta2 & MUT2_YAKU) && (randint1(3000) == 13))
	{
		disturb(0, 1);
		curse_equipment(100,(dun_level / 3 + 1));

	}

	if ((p_ptr->muta2 & MUT2_PROD_MANA) &&
	    !p_ptr->anti_magic && one_in_(9000))
	{
		int dire = 0;
		disturb(0, 1);
#ifdef JP
		msg_print("���@�̃G�l���M�[���ˑR���Ȃ��̒��ɗ��ꍞ��ł����I�G�l���M�[��������Ȃ���΂Ȃ�Ȃ��I");
#else
		msg_print("Magical energy flows through you! You must release it!");
#endif

		flush();
		msg_print(NULL);
		(void)get_hack_dir(&dire);
		fire_ball(GF_MANA, dire, p_ptr->lev * 2, 3);
	}

	if ((p_ptr->muta2 & MUT2_ATT_DEMON) &&
	    !p_ptr->anti_magic && (randint1(6666) == 666))
	{
		bool pet = one_in_(6);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px,
				    dun_level, SUMMON_DEMON, mode))
		{
#ifdef JP
			msg_print("���Ȃ��̓f�[�����������񂹂��I");
#else
			msg_print("You have attracted a demon!");
#endif

			disturb(0, 1);
		}
	}

	if ((p_ptr->muta2 & MUT2_SPEED_FLUX) && one_in_(6000))
	{
		disturb(0, 1);
		if (one_in_(2))
		{
#ifdef JP
			msg_print("���͓I�łȂ��Ȃ����C������B");
#else
			msg_print("You feel less energetic.");
#endif

			if (p_ptr->fast > 0)
			{
				set_fast(0, TRUE);
			}
			else
			{
				set_slow(randint1(30) + 10, FALSE);
			}
		}
		else
		{
#ifdef JP
			msg_print("���͓I�ɂȂ����C������B");
#else
			msg_print("You feel more energetic.");
#endif

			if (p_ptr->slow > 0)
			{
				set_slow(0, TRUE);
			}
			else
			{
				set_fast(randint1(30) + 10, FALSE);
			}
		}
		msg_print(NULL);
	}
	if ((p_ptr->muta2 & MUT2_BANISH_ALL) && one_in_(9000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("�ˑR�قƂ�ǌǓƂɂȂ����C������B");
#else
		msg_print("You suddenly feel almost lonely.");
#endif

		banish_monsters(100);
		///del131221 �X��͌�サ�Ȃ�
#if 0
		if (!dun_level && p_ptr->town_num)
		{
			int n;

			/* Pick a random shop (except home) */
			do
			{
				n = randint0(MAX_STORES);
			}
			while ((n == STORE_HOME) || (n == STORE_MUSEUM));

#ifdef JP
			msg_print("�X�̎�l���u�Ɍ������đ����Ă���I");
#else
			msg_print("You see one of the shopkeepers running for the hills!");
#endif

			store_shuffle(n);
		}
#endif
		msg_print(NULL);
	}

	if ((p_ptr->muta2 & MUT2_EAT_LIGHT) && one_in_(3000))
	{
		object_type *o_ptr;

#ifdef JP
		msg_print("�e�ɂ܂ꂽ�B");
#else
		msg_print("A shadow passes over you.");
#endif

		msg_print(NULL);

		/* Absorb light from the current possition */
		if ((cave[py][px].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
		{
			hp_player(10);
		}

		o_ptr = &inventory[INVEN_LITE];

		/* Absorb some fuel in the current lite */
		if (o_ptr->tval == TV_LITE)
		{
			/* Use some fuel (except on artifacts) */
			if (!object_is_fixed_artifact(o_ptr) && (o_ptr->xtra4 > 0))
			{
				/* Heal the player a bit */
				hp_player(o_ptr->xtra4 / 20);

				/* Decrease life-span of lite */
				o_ptr->xtra4 /= 2;

#ifdef JP
				msg_print("��������G�l���M�[���z�������I");
#else
				msg_print("You absorb energy from your light!");
#endif


				/* Notice interesting fuel steps */
				notice_lite_change(o_ptr);
			}
		}

		/*
		 * Unlite the area (radius 10) around player and
		 * do 50 points damage to every affected monster
		 */
		unlite_area(50, 10);
	}

	if ((p_ptr->muta2 & MUT2_ATT_ANIMAL) &&
	    !p_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_ANIMAL, mode))
		{
#ifdef JP
			msg_print("�����������񂹂��I");
#else
			msg_print("You have attracted an animal!");
#endif

			disturb(0, 1);
		}
	}

	if ((p_ptr->muta2 & MUT2_RAW_CHAOS) &&
	    !p_ptr->anti_magic && one_in_(8000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("����̋�Ԃ��c��ł���C������I");
#else
		msg_print("You feel the world warping around you!");
#endif

		msg_print(NULL);
		fire_ball(GF_CHAOS, 0, p_ptr->lev, 8);
	}
	if ((p_ptr->muta2 & MUT2_NORMALITY) && one_in_(5000))
	{
		if (!lose_mutation(0))
#ifdef JP
			msg_print("�ʏ�ɋ߂Â��Ă����C������B");
#else
		msg_print("You feel oddly normal.");
#endif

	}
	if ((p_ptr->muta2 & MUT2_WRAITH) && !p_ptr->anti_magic && one_in_(3000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("�񕨎��������I");
#else
		msg_print("You feel insubstantial!");
#endif

		msg_print(NULL);
		set_wraith_form(randint1(p_ptr->lev / 2) + (p_ptr->lev / 2), FALSE);
	}
	if ((p_ptr->muta2 & MUT2_ATT_GHOST) &&
	    !p_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_GHOST, mode))
		{
#ifdef JP
			msg_print("�H�삪����Ă����I");
#else
			msg_print("You have attracted an animal!");
#endif

			disturb(0, 1);
		}
	}
	if ((p_ptr->muta2 & MUT2_ATT_KWAI) &&
	    !p_ptr->anti_magic && one_in_(7000))
	{
		bool pet = one_in_(3);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_KWAI, mode))
		{
#ifdef JP
			msg_print("�d��������Ă����I");
#else
			msg_print("You have attracted an animal!");
#endif

			disturb(0, 1);
		}
	}
	if ((p_ptr->muta2 & MUT2_ATT_DRAGON) &&
	    !p_ptr->anti_magic && one_in_(3000))
	{
		bool pet = one_in_(5);
		u32b mode = PM_ALLOW_GROUP;

		if (pet) mode |= PM_FORCE_PET;
		else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

		if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_DRAGON, mode))
		{
#ifdef JP
			msg_print("�h���S���������񂹂��I");
#else
			msg_print("You have attracted a dragon!");
#endif

			disturb(0, 1);
		}
	}
	if ((p_ptr->muta2 & MUT2_XOM) && one_in_(3000))
	{
#ifdef JP
			msg_print("�]���u�c�O�Ȃ���܂����ꂵ�������ĂȂ��v");
#endif
		(void)set_shero(10 + randint1(p_ptr->lev), FALSE);
		(void)set_afraid(0);

	}
	if ((p_ptr->muta2 & MUT2_BIRDBRAIN) && one_in_(3000) || p_ptr->pclass == CLASS_KOISHI && one_in_(1000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("�c�H�@������Y��Ă���C������B");
#endif

		lose_all_info();
		if (music_singing_any()) stop_singing();
		if (hex_spelling_any()) stop_hex_spell_all();
		stop_tsukumo_music();
		stop_raiko_music();
	}

	if ((p_ptr->muta2 & MUT2_WALK_SHAD) &&
	    !p_ptr->anti_magic && one_in_(12000) && !p_ptr->inside_arena)
	{
		alter_reality();
	}

	if ((p_ptr->muta2 & MUT2_WARNING) && one_in_(1000))
	{
		int danger_amount = 0;
		int monster;

		for (monster = 0; monster < m_max; monster++)
		{
			monster_type    *m_ptr = &m_list[monster];
			monster_race    *r_ptr = &r_info[m_ptr->r_idx];

			/* Paranoia -- Skip dead monsters */
			if (!m_ptr->r_idx) continue;

			if (r_ptr->level >= p_ptr->lev)
			{
				danger_amount += r_ptr->level - p_ptr->lev + 1;
			}
		}

		if (danger_amount > 100)
#ifdef JP
			msg_print("���ɋ��낵���C������I");
#else
		msg_print("You feel utterly terrified!");
#endif

		else if (danger_amount > 50)
#ifdef JP
			msg_print("���낵���C������I");
#else
		msg_print("You feel terrified!");
#endif

		else if (danger_amount > 20)
#ifdef JP
			msg_print("���ɐS�z�ȋC������I");
#else
		msg_print("You feel very worried!");
#endif

		else if (danger_amount > 10)
#ifdef JP
			msg_print("�S�z�ȋC������I");
#else
		msg_print("You feel paranoid!");
#endif

		else if (danger_amount > 5)
#ifdef JP
			msg_print("�قƂ�ǈ��S�ȋC������B");
#else
		msg_print("You feel almost safe.");
#endif

		else
#ifdef JP
			msg_print("�₵���C������B");
#else
		msg_print("You feel lonely.");
#endif

	}
	if ((p_ptr->muta2 & MUT2_INVULN) && !p_ptr->anti_magic &&
	    one_in_(5000))
	{
		disturb(0, 1);
#ifdef JP
		msg_print("���G�ȋC������I");
#else
		msg_print("You feel invincible!");
#endif

		msg_print(NULL);
		(void)set_invuln(randint1(8) + 8, FALSE);
	}

	///mod140813 �b��
	if ((p_ptr->muta2 & MUT2_ASTHMA) && one_in_(1000))
	{
		msg_print("�b���̔��삾�I");
		(void)set_asthma(p_ptr->asthma + 1000 + randint0(2000));
	}

	if ((p_ptr->muta2 & MUT2_THE_WORLD) && !p_ptr->anti_magic &&
	    one_in_(4000))
	{
		disturb(0, 1);
		if (world_player)
		{
#ifdef JP
			msg_print("���Ɏ��͎~�܂��Ă���B");
#endif
		}
		else
		{
			world_player = TRUE;
			if(p_ptr->pclass == CLASS_SAKUYA) sakuya_time_stop(FALSE);

#ifdef JP
			msg_print("�˔@�A�ӂ�̑S�Ă��Î~�����B");
#endif
			msg_print(NULL);
			//v1.1.58
			flag_update_floor_music = TRUE;
			/* Hack */
			p_ptr->energy_need -= 1000 + (400)*TURNS_PER_TICK/10;
			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);
			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);
			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			handle_stuff();
		}
	}
	if ((p_ptr->muta2 & MUT2_DISARM) && one_in_(10000))
	{
		int slot = 0;
		object_type *o_ptr = NULL;

		disturb(0, 1);
#ifdef JP
		msg_print("��������ē]�񂾁I");
		take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->wt / 6), "�]�|", -1);
#else
		msg_print("You trip over your own feet!");
		take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->wt / 6), "tripping", -1);
#endif

		msg_print(NULL);
		if (buki_motteruka(INVEN_RARM))
		{
			slot = INVEN_RARM;
			o_ptr = &inventory[INVEN_RARM];

			if (buki_motteruka(INVEN_LARM) && one_in_(2))
			{
				o_ptr = &inventory[INVEN_LARM];
				slot = INVEN_LARM;
			}
		}
		else if (buki_motteruka(INVEN_LARM))
		{
			o_ptr = &inventory[INVEN_LARM];
			slot = INVEN_LARM;
		}

		///pend ������������藎�Ƃ������B�ꕔ�L�����N�^�[�͏��O�����v
		if (slot && !object_is_cursed(o_ptr) && p_ptr->pclass != CLASS_KOGASA)
		{
#ifdef JP
			msg_print("����𗎂Ƃ��Ă��܂����I");
#else
			msg_print("You drop your weapon!");
#endif
			inven_drop(slot, 1);
		}
	}
}

/*
 * Handle curse effects once every 10 game turns
 */
/*:::�􂢔��������@�g�����v����̃����_���e���|��R���̕�΂�HP�z����*/
static void process_world_aux_curse(void)
{

	if(p_ptr->inside_battle) return;

	//v1.1.48 ���X�����L��}�b�v�ł͔������Ȃ����Ƃɂ���
	if (p_ptr->wild_mode) return;

	//v1.1.48 �����̕s�^�p���[����������Ƃ��m���ŏ���ɃX�[�p�[�n�R�_�ɕϐg����
	if (p_ptr->pclass == CLASS_SHION && one_in_(99) && !p_ptr->tim_mimic)
	{
		int power = CALC_SUPER_SHION_TURNS;

		if (randint1(power) > 13)
		{

			msg_print("���ߍ��܂ꂽ�s�^�p���[���\�������I");
			disturb(1, 1);
			set_tim_general(power, FALSE, 0, 0);

			//�ϐg����magic_num1[0]�����݃^�[�����ɂ�[1]�����Z�b�g
			p_ptr->magic_num1[0] = turn;
			p_ptr->magic_num1[1] = 0;
			//HP�S��
			hp_player(9999);


		}
		else if (power >= 13)
		{
			msg_print("���ߍ��܂ꂽ�s�^�p���[���\���������ȋC������...");

		}
	}

	//v1.1.48 �������X�[�p�[�n�R�_�̂Ƃ��ɂ͐F�X�N����
	if (SUPER_SHION && one_in_(66) && !p_ptr->inside_battle && !p_ptr->wild_mode)
	{
		int mode = randint1(6);
		switch (mode)
		{

		case 1:
		case 2:
		{
			int y, x, feat;
			feature_type *f_ptr;

			if (mode == 1)msg_print("���Ȃ��͓S�C���ɏP��ꂽ�I");
			if (mode == 2)msg_print("���Ȃ��͗n�◬�ɏP��ꂽ�I");

			for (y = 1; y < cur_hgt - 1; y++)
			{
				for (x = 1; x < cur_wid - 1; x++)
				{
					cave_type *c_ptr = &cave[y][x];
					feat = get_feat_mimic(c_ptr);
					f_ptr = &f_info[feat];

					if (mode == 1 && have_flag(f_ptr->flags, FF_FLOOR))
						cave_set_feat(y, x, feat_deep_water);
					if (mode == 2 && have_flag(f_ptr->flags, FF_FLOOR))
						cave_set_feat(y, x, feat_deep_lava);
				}
			}

			p_ptr->redraw |= (PR_MAP);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			handle_stuff();
			break;
		}

		case 3:
			if (dun_level)
			{
				msg_print("���Ȃ��͗��ՂɏP��ꂽ�I");
				earthquake(py, px, 12);

			}
			else
			{
				msg_print("���Ȃ��͗����ɏP��ꂽ�I");
				project(0,9,py,px,666,GF_TORNADO,(PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
			}
			break;
		case 4:
			msg_print("���Ȃ��͗����J�ɏP��ꂽ�I");
			cast_meteor(300, 4, GF_METEOR);
			break;
		case 5:
			msg_print("���Ȃ��͓�̎���̘c�݂ɏP��ꂽ�I");
			project_hack2(GF_DISTORTION, 1, 1000, 0);
			break;
		case 6:
			msg_print("���s���̕��̃I�[���������ꂽ�I");
			floor_attack(GF_DISENCHANT, 0, 0, p_ptr->lev * 8, 0);
			break;
		}


	}

	//���̖�������̃y�i���e�B
	if(p_ptr->pclass == CLASS_HINA  && !p_ptr->inside_battle && !p_ptr->wild_mode)
	{
		int yaku = p_ptr->magic_num1[0];

		if(yaku > HINA_YAKU_LIMIT3)
		{
			if(one_in_(50))
			{
				int power = yaku / 50;
				switch(randint1(8))
				{
				case 1:
					msg_print("�˔@�A����ȗ��������������I");
					project(0,8,py,px,power+randint1(power),GF_TORNADO,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 2:
					if(dun_level && !p_ptr->inside_quest)
					{
						msg_print("�˔@�A�n�k�����������I");
						earthquake_aux(py,px,10,0);
					}
					else
						msg_print("�n���������������E�E");
					break;
				case 3:
					msg_print("�˔@�A�����J���~�蒍�����I");
					cast_meteor(power/2, 3,GF_METEOR);
					break;
				case 4:
					msg_print("�˔@�A�d�͏�ُ킪���������I");
					project(0,8,py,px,power+randint1(power),GF_GRAVITY,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 5:
					msg_print("�˔@�A�J�I�X�̉Q�����������I");
					project(0,8,py,px,power+randint1(power),GF_CHAOS,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 6:
					msg_print("�˔@�A�嗒�����������I");
					project_hack2(GF_TSUKUMO_STORM,1,power/2,power/2);
					break;
				case 7:
					msg_print("�˔@�A�v���Y�}�������������I");
					project(0,8,py,px,power+randint1(power),GF_PLASMA,(PROJECT_KILL|PROJECT_GRID|PROJECT_JUMP|PROJECT_ITEM),-1);
					break;
				case 8:
					mass_genocide(power,FALSE);
					msg_print("�ӂ肩�瑼�҂̋C�z���������C������B");
					break;

				}

			}
		}
		if(yaku > (HINA_YAKU_LIMIT3) && dun_level > 10 && !p_ptr->inside_quest && one_in_(200))
		{
			bool entry = FALSE;
			if(one_in_(2))
			{
				if(summon_named_creature(0,py,px,MON_REIMU,PM_NO_PET)) entry = TRUE;
			}
			else
			{
				if(summon_named_creature(0,py,px,MON_MARISA,PM_NO_PET)) entry = TRUE;
			}

			if(entry) 
			{
				msg_print("�ٕω����̐��Ƃ����Ȃ���ގ����ɂ���Ă����I");
				disturb(1, 1);
			}

		}
	}

	if ((p_ptr->cursed & TRC_P_FLAG_MASK) && !p_ptr->inside_battle && !p_ptr->wild_mode)
	{
		/*
		 * Hack: Uncursed teleporting items (e.g. Trump Weapons)
		 * can actually be useful!
		 */
		if ((p_ptr->cursed & TRC_TELEPORT_SELF) && one_in_(200))
		{
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			int i, i_keep = 0, count = 0;

			/* Scan the equipment with random teleport ability */
			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			{
				u32b flgs[TR_FLAG_SIZE];
				o_ptr = &inventory[i];

				/* Skip non-objects */
				if (!o_ptr->k_idx) continue;

				if(check_invalidate_inventory(i)) continue;

				/* Extract the item flags */
				object_flags(o_ptr, flgs);

				if (have_flag(flgs, TR_TELEPORT))
				{
					/* {.} will stop random teleportation. */
					if (!o_ptr->inscription || !my_strchr(quark_str(o_ptr->inscription), '.'))
					{
						count++;
						if (one_in_(count)) i_keep = i;
					}
				}
			}

			o_ptr = &inventory[i_keep];
			object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
			msg_format("%s���e���|�[�g�̔\�͂𔭓������悤�Ƃ��Ă���B", o_name);
#else
			msg_format("Your %s is activating teleportation.", o_name);
#endif

#ifdef JP
			if (get_check_strict("�e���|�[�g���܂����H", CHECK_OKAY_CANCEL))
#else
			if (get_check_strict("Teleport? ", CHECK_OKAY_CANCEL))
#endif
			{
				disturb(0, 1);
				teleport_player(50, 0L);
			}
			else
			{
#ifdef JP
				msg_format("%s��{.}(�s���I�h)�Ɩ������ނƔ�����}���ł��܂��B", o_name);
#else
				msg_format("You can inscribe {.} on your %s to disable random teleportation. ", o_name);
#endif
				disturb(1, 1);
			}
		}
		/* Make a chainsword noise */
		///item131221 �`�F�[���\�[�̃��b�Z�[�W���X�g�[���u�����K�[�ɍ����ւ��Ă݂�
		if ((p_ptr->cursed & TRC_STORMBRINGER) && one_in_(CHAINSWORD_NOISE))
		{
			char noise[1024];
#ifdef JP
			if (!get_rnd_line("storm_j.txt", 0, noise))
			//if (!get_rnd_line("chainswd_j.txt", 0, noise))

#else
			if (!get_rnd_line("chainswd.txt", 0, noise))
#endif
				msg_print(noise);
			disturb(FALSE, FALSE);
		}
		/* TY Curse */
		if ((p_ptr->cursed & TRC_TY_CURSE) && one_in_(TY_CURSE_CHANCE))
		{
			int count = 0;
			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 20)
			{
				msg_format("���Â̎􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			(void)activate_ty_curse(FALSE, &count);
		}
		/* Handle experience draining */
		if (p_ptr->prace != RACE_ANDROID && 
			((p_ptr->cursed & TRC_DRAIN_EXP) && one_in_(4)))
		{
			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 3) return;

			if(p_ptr->pclass == CLASS_HINA)
			{
				hina_gain_yaku(randint1(3));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				p_ptr->magic_num1[1] += 100 + randint1(100);
				return;
			}


			p_ptr->exp -= (p_ptr->lev+1)/2;
			if (p_ptr->exp < 0) p_ptr->exp = 0;
			p_ptr->max_exp -= (p_ptr->lev+1)/2;
			if (p_ptr->max_exp < 0) p_ptr->max_exp = 0;
			check_experience();
		}
		/* Add light curse (Later) */
		if ((p_ptr->cursed & TRC_ADD_L_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(50 + randint1(50));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 1000 + randint1(1000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 10)
			{
				msg_format("�􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			o_ptr = choose_cursed_obj_name(TRC_ADD_L_CURSE);

			new_curse = get_curse(0, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("���ӂɖ����������I�[����%s���Ƃ�܂���...", o_name);
#else
				msg_format("There is a malignant black aura surrounding your %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				p_ptr->update |= (PU_BONUS);
			}
		}
		/* Add heavy curse (Later) */
		if ((p_ptr->cursed & TRC_ADD_H_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̋��͂Ȏ􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(300 + randint1(300));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̋��͂Ȏ􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 5000 + randint1(5000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 15)
			{
				msg_format("���͂Ȏ􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			o_ptr = choose_cursed_obj_name(TRC_ADD_H_CURSE);

			new_curse = get_curse(1, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

				o_ptr->curse_flags |= new_curse;
#ifdef JP
				msg_format("���ӂɖ����������I�[����%s���Ƃ�܂���...", o_name);
#else
				msg_format("There is a malignant black aura surrounding your %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				p_ptr->update |= (PU_BONUS);
			}
		}
		/* Call animal */
		if ((p_ptr->cursed & TRC_CALL_ANIMAL) && one_in_(2500))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(50 + randint1(50));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 1000 + randint1(1000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 10)
			{
				msg_format("�􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			if (summon_specific(0, py, px, dun_level, SUMMON_ANIMAL,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_ANIMAL), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%s�������������񂹂��I", o_name);
#else
				msg_format("Your %s have attracted an animal!", o_name);
#endif

				disturb(0, 1);
			}
		}
		/* Call demon */
		if ((p_ptr->cursed & TRC_CALL_DEMON) && one_in_(1111))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̋��͂Ȏ􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(200 + randint1(200));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 3000 + randint1(3000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 15)
			{
				msg_format("���͂Ȏ􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			if (summon_specific(0, py, px, dun_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DEMON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%s�������������񂹂��I", o_name);
#else
				msg_format("Your %s have attracted a demon!", o_name);
#endif

				disturb(0, 1);
			}
		}
		/* Call dragon */
		if ((p_ptr->cursed & TRC_CALL_DRAGON) && one_in_(800))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̋��͂Ȏ􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(300 + randint1(300));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 5000 + randint1(5000);
				return;
			}
			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 15)
			{
				msg_format("���͂Ȏ􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			if (summon_specific(0, py, px, dun_level, SUMMON_DRAGON,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DRAGON), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%s���h���S���������񂹂��I", o_name);
#else
				msg_format("Your %s have attracted an animal!", o_name);
#endif

				disturb(0, 1);
			}
		}
		if ((p_ptr->cursed & TRC_COWARDICE) && one_in_(1500))
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(30 + randint1(10));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 1000 + randint1(1000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 5)
			{
				msg_format("�􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			if (!p_ptr->resist_fear || IS_VULN_FEAR)
			{
				disturb(0, 1);
#ifdef JP
				msg_print("�ƂĂ��Â�... �ƂĂ������I");
#else
				msg_print("It's so dark... so scary!");
#endif

				set_afraid(p_ptr->afraid + 13 + randint1(26));
			}
		}
		/* Teleport player */
		if ((p_ptr->cursed & TRC_TELEPORT) && one_in_(200) && !p_ptr->anti_tele)
		{
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̋��͂Ȏ􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(100 + randint1(200));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 2000 + randint1(2000);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 10)
			{
				msg_format("���͂Ȏ􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}
			disturb(0, 1);

			/* Teleport player */
			teleport_player(40, TELEPORT_PASSIVE);
		}
		/* Handle HP draining */
		if ((p_ptr->cursed & TRC_DRAIN_HP) && one_in_(666))
		{
			char o_name[MAX_NLEN];
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(20 + randint1(20));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 500 + randint1(500);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 5)
			{
				msg_format("�􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_HP), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%s�͂��Ȃ��̗̑͂��z�������I", o_name);
#else
			msg_format("Your %s drains HP from you!", o_name);
#endif
			take_hit(DAMAGE_LOSELIFE, MIN(p_ptr->lev*2, 100), o_name, -1);
		}
		/* Handle mana draining */
		if ((p_ptr->cursed & TRC_DRAIN_MANA) && p_ptr->csp && one_in_(666))
		{
			char o_name[MAX_NLEN];
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				hina_gain_yaku(25 + randint1(25));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�����i�̎􂢂������������A���Ȃ��͂�����z��������B");
				p_ptr->magic_num1[1] += 500 + randint1(500);
				return;
			}

			if(p_ptr->prace == RACE_TSUKUMO && randint1(p_ptr->lev) > 5)
			{
				msg_format("�􂢂����������������ǂ��ɂ��G�߂��B");
				return;
			}

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_MANA), (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
			msg_format("%s�͂��Ȃ��̖��͂��z�������I", o_name);
#else
			msg_format("Your %s drains mana from you!", o_name);
#endif
			p_ptr->csp -= MIN(p_ptr->lev, 50);
			if (p_ptr->csp < 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= PR_MANA;
		}
	}

	/* Rarely, take damage from the Jewel of Judgement */
	if (one_in_(999) && !p_ptr->anti_magic)
	{
		object_type *o_ptr;

		o_ptr = &inventory[INVEN_LITE];
		if (o_ptr->name1 == ART_JUDGE)
		{

			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�w�R���̕�΁x�����Ȃ��̗̑͂��z�����悤�Ƃ������A�t�ɖ���z��������B");
				hina_gain_yaku(200 + randint1(200));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("�w�R���̕�΁x�����Ȃ��̗̑͂��z�����悤�Ƃ������A�t�ɕs�^�p���[���z��������B");
				p_ptr->magic_num1[1] += 2000 + randint1(2000);
				return;
			}

			if (object_is_known(o_ptr))
				msg_print("�w�R���̕�΁x�͂��Ȃ��̗̑͂��z�������I");
			else
				msg_print("�Ȃɂ������Ȃ��̗̑͂��z�������I");
			take_hit(DAMAGE_LOSELIFE, MIN(p_ptr->lev, 50), "�R���̕��", -1);

		}

	}

	//v1.1.48 �n�R�_�G�S �O�d���͈Ⴄ�X���b�g�ɑ����ł���̂Ńy�i���e�B�Ȃ������܂����Ȃ��낤
	if (inventory[INVEN_BODY].name2 == EGO_BODY_POVERTY && one_in_(100))
	{
		int lose_money = p_ptr->au / 20 + randint0(p_ptr->au / 20) + 1;

		//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
		if (p_ptr->pclass == CLASS_SHION)
		{
			p_ptr->magic_num1[1] += 1000 + randint1(1000);
			return;
		}

		if (p_ptr->au < 1) return;

		//���z�������Ȃ��\�͎����̗d���͖h�䂷��
		if (CHECK_FAIRY_TYPE != 9)
		{
			p_ptr->au -= lose_money;
			p_ptr->redraw |= PR_GOLD;
			msg_print("���z���y���Ȃ����C������B");
			disturb(0, 1);
		}
	}

	/*:::���̉H�߂ɋL���r������*/
	if (one_in_(999) && !p_ptr->anti_magic)
	{
		if ( check_equip_specific_fixed_art(ART_HAGOROMO_MOON,TRUE) && !p_ptr->resist_insanity)
		{
			msg_print("���̉H�߂��������Ɍ������C������E�E");
			if (lose_all_info()) msg_print("�}�ɋL�����B���ɂȂ����B");
		}
	}
	//�i�����̋߂��ł͓��ɖ���
	if(!dun_level && !p_ptr->wild_mode && distance(p_ptr->wilderness_y,p_ptr->wilderness_x,55,46) < 6 && !p_ptr->town_num && one_in_(10) && !p_ptr->levitation && randint0(p_ptr->skill_srh) < 10)
	{
		teleport_player(100, TELEPORT_NONMAGICAL);
		msg_print("..���ɖ������C������B");

	}
	//���Q�[���I�[�o�[
	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] >= HINA_YAKU_MAX)
	{
		msg_format("���܂�ɗ��܂�����唚�����N�������I");
		project_hack2(GF_DISINTEGRATE,0,0,9999);
		take_hit(DAMAGE_NOESCAPE,9999,"��̑唚��",-1);
	}

}

///mod140330 �D���x�ɂ�鏈��
//v1.1.19 �D���x�}�W�b�N�i���o�[�r��
void process_world_aux_alcohol(void)
{
	int freq;
	int eccentricity;

	/*:::�A�����łȂ��Ƃ��D���x�ő�i�K�Ȃ牽�����Ȃ��i�ӎ��s���Ăœ|��Ă��邽�߁j*/
	if(!(p_ptr->muta2 & MUT2_ALCOHOL) && p_ptr->alcohol >= DRANK_4)return;
	//���[���h�}�b�v�ł͉����N����Ȃ�
	if(p_ptr->wild_mode) return;
	if (p_ptr->inside_battle) return;

	if(p_ptr->alcohol < DRANK_2) return;
	else if(p_ptr->alcohol < DRANK_3) freq = 100;
	else if(p_ptr->alcohol < DRANK_4) freq = 20;
	else freq = 10;

	if(!one_in_(freq)) return;
	/*:::����ȍ~�A�D���ɂ�鉽�炩�̌��ʔ���*/

	if(p_ptr->alcohol < DRANK_3) eccentricity = randint0(50);
	else eccentricity = randint0(90);

	if(eccentricity < 10)//�j��
	{
		msg_format("�������ƂĂ��K���ȋC�����I");
		set_blessed(randint1(25) + 25, FALSE);
	}
	else if(eccentricity < 15)//����
	{
		msg_format("�������̂��d���E�E");
		set_slow(randint1(25) + 25, FALSE);
	}
	else if(eccentricity < 20)//����
	{
		msg_format("�������E�E�������E�E���������H");
		if(!p_ptr->resist_conf) set_confused(randint1(25) + 25);
	}
	else if(eccentricity < 25)//����
	{
		msg_format("�������̂��y���I");
		set_fast(randint1(25) + 25, FALSE);
	}
	else if(eccentricity < 30)//����
	{
		msg_format("�������吺�ŉ̂��o�������Ȃ����I");
		aggravate_monsters(0,FALSE);
	}
	else if(eccentricity < 35)//�N�O
	{
		msg_format("�����������ڂ��肷��E�E");
		set_stun(p_ptr->stun + 10 + randint0(21));
	}
	else if(eccentricity < 40)//����m��
	{
		msg_format("�����������ɐl���a�肽���Ȃ����E�E");
			set_shero(randint1(25) + 25, FALSE);
			hp_player(30);
			set_afraid(0);
	}
	else if(eccentricity < 45)//�؂菝
	{
		msg_format("�������m��Ȃ������ɉ�������Ă���E�E");
		take_hit(DAMAGE_NOESCAPE,randint1(10),"���������ĕ���������",-1);
		set_cut(p_ptr->cut + randint1(10));
	}
	else if(eccentricity < 50)//�L���r�����e���|�[�g
	{
		if(lose_all_info())
		{
			msg_format("�������L�����B�����B");

			if(one_in_(3))
			{ 
				msg_print("�E�E�Ƃ����������͂ǂ����H");
				teleport_player(10 + randint1(200), TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
			}
		}
	}
	/*:::�ȉ��ADRANK_3�ȍ~�̂�*/
	else if(eccentricity < 55)//�݂̑|��
	{
		msg_format("�������E�E");
		if (!throw_up())
		{
			msg_format("�f�������ɂȂ������}�����B");

		}
	}
	else if(eccentricity < 60)//��
	{
		if(p_ptr->chp < p_ptr->mhp)
		{
			msg_format("����������̒ɂ݂������Ȃ��C������B");
			hp_player(p_ptr->lev * 4);
		}
	}

	else if(eccentricity < 65)//���o
	{
		if(!p_ptr->resist_pois && !p_ptr->resist_chaos)
		{
			msg_format("����������̌i�F���x���Č�����E�E");
			if(!p_ptr->resist_chaos) set_image(20 + randint0(81));
		}
	}

	else if(eccentricity < 70)//��������
	{
		int slot = INVEN_RIBBON + randint0(10);
		
		if(inventory[slot].k_idx )
		{
			if(!wield_check(slot,0)) msg_format("�������E�������Ȃ��������d�����B");
			else if( object_is_cursed(&inventory[slot]) || check_invalidate_inventory(slot) ) msg_format("�������E�������Ȃ������E���Ȃ������B");
			else
			{
				msg_format("�������E�������Ȃ����B");
				inven_takeoff(slot,1);
			}
		}
	}
	else if(eccentricity < 75)//�r�ό��E�˔j
	{
		int time = 25 + randint0(25);
		msg_format("���������̂̌��E���ǂ����ɒu���Y�ꂽ�C������I");
		set_tim_addstat(A_STR,100 + 10,time,FALSE);
		set_tim_addstat(A_CON,100 + 10,time,FALSE);
	}
	else if(eccentricity < 80)//����+����+��p���E�˔j
	{
		int time = 25 + randint0(25);
		msg_format("���������̂������̂��y���I");
		set_fast(time,FALSE);
		set_tim_levitation(time,FALSE);
		set_tim_addstat(A_DEX,100 + 10,time,FALSE);
		//set_tim_speedstar(time,FALSE);

	}
	else if(eccentricity < 85)//�����ϗe�i�����j
	{
		msg_format("�����������������Ă����C������B");
		if (p_ptr->inside_arena || ironman_downward)
			msg_print("����A�C�̂������H");
		else
		{
			/*:::0�ł͂܂����͂�*/
			p_ptr->alter_reality = 1;
			p_ptr->redraw |= (PR_STATUS);
		}
	}
	else //���G��
	{
		msg_format("���������Ȃ�Ȃ�ł��ł���C������I");
		(void)set_invuln(randint1(8) + 8, FALSE);
	}

}


/*
 * Handle recharging objects once every 10 game turns
 */
/*:::���b�h�Ȃǂ̎����[�U*/
static void process_world_aux_recharge(void)
{
	int i;
	bool changed;

	int hiziri_card_num;
	hiziri_card_num = count_ability_card(ABL_CARD_KYOUTEN);

	/* Process equipment */
	for (changed = FALSE, i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;
		///mod160508 �e�̏[�U�͂����ł͍s��Ȃ�
		if(o_ptr->tval == TV_GUN) continue; 
		if(check_invalidate_inventory(i)) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			//v1.1.86 �u�@�͌o�T�v�J�[�h�ɂ��[�U�u�[�X�g
			if (hiziri_card_num)
			{
				if (randint1(100) <= CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num)) o_ptr->timeout--;
			}
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}
		}
	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
		wild_regen = 20;
	}

	/*
	 * Recharge rods.  Rods now use timeout to control charging status,
	 * and each charging rod in a stack decreases the stack's timeout by
	 * one per turn. -LM-
	 */
	for (changed = FALSE, i = 0; i < INVEN_PACK; i++)
	{
		object_type *o_ptr = &inventory[i];
		object_kind *k_ptr = &k_info[o_ptr->k_idx];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Examine all charging rods or stacks of charging rods. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Determine how many rods are charging. */
			int temp = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
			if (temp > o_ptr->number) temp = o_ptr->number;


			//v1.1.86 �u�@�͌o�T�v�J�[�h�ɂ��[�U�u�[�X�g
			if (hiziri_card_num)
			{
				int prob = CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num);//������

				//��{�[�U��*�������̐��������͂��̂܂܉��Z
				int bonus = temp * prob / 100;
				//��{�[�U��*�������̗]�蕔���͂����%�l�Ƃ��Ĕ���ʂ�Ή��Z
				if (randint1(100) <= (temp*prob) % 100) bonus++;

				temp += bonus;
			}

			/* Decrease timeout by that number. */
			o_ptr->timeout -= temp;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if (!(o_ptr->timeout))
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}

			/* One of the stack of rod is charged */
			else if (o_ptr->timeout % k_ptr->pval)
			{
				changed = TRUE;
			}
		}

		//v1.1.86 �A�r���e�B�J�[�h�̏[�U
		if ((o_ptr->tval == TV_ABILITY_CARD) && (o_ptr->timeout))
		{
			int base_charge_time,temp;

			if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
			{
				msg_format("ERROR:�s���ȃJ�[�hidx(%d)���������A�r���e�B�J�[�h���[�U�������ꂽ", o_ptr->pval);
				continue;
			}
			base_charge_time = ability_card_list[o_ptr->pval].charge_turn;//�ꖇ������̊�{�`���[�W����

			//��{�`���[�W���Ԃ�0�������瑦���`���[�W���I�������Ď��ցB
			//���ʂ���Ȏ��͋N����Ȃ����A���������^�J�[�h�����L�^�J�[�h�Ɏd�l�ύX�����Ƃ��`���[�W���̃J�[�h�������Ă��炱���ɗ���
			if (!base_charge_time)
			{
				p_ptr->window |= (PW_INVEN);
				o_ptr->timeout = 0;
				continue;
			}

			//�Ⴆ��3�{��1�{�������[�U���̂Ƃ��A3�{�Ƃ��[�U���̂Ƃ��ɔ�ׂă^�C���A�E�g�l�̌������1/3�ɂȂ鏈�����B
			temp = (o_ptr->timeout + (base_charge_time - 1)) / base_charge_time;
			if (temp > o_ptr->number) temp = o_ptr->number;

			//�u�@�͌o�T�v�J�[�h�ɂ��[�U�u�[�X�g
			if (hiziri_card_num)
			{
				int prob = CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num);//������
				//��{�[�U��*�������̐��������͂��̂܂܉��Z
				int bonus = temp * prob / 100;
				//��{�[�U��*�������̗]�蕔���͂����%�l�Ƃ��Ĕ���ʂ�Ή��Z
				if (randint1(100) <= (temp*prob) % 100) bonus++;

				temp += bonus;
			}


			/* Decrease timeout by that number. */
			o_ptr->timeout -= temp;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if (!(o_ptr->timeout))
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}

			/* One of the stack of rod is charged */
			else if (o_ptr->timeout % base_charge_time)
			{
				changed = TRUE;
			}
		}


	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
		wild_regen = 20;
	}


	/* Process objects on floor */
	for (i = 1; i < o_max; i++)
	{
		/* Access object */
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge rods on the ground.  No messages. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Charge it */
			o_ptr->timeout -= o_ptr->number;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;
		}
	}

	//v1.1.86 ����̃A�r���e�B�J�[�h�͏[�U����Ȃ�


	//v1.1.46 ��������p�X���b�g�ɑ������Ă���w�ւ̏[�U�������s��
	if (p_ptr->pclass == CLASS_JYOON)	for (changed = FALSE, i = 0; i < INVEN_ADD_MAX; i++)
	{
		/* Get the object */
		object_type *o_ptr = &inven_add[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval == TV_GUN) continue;

		//�w�փX���b�g������������Ă���Ƃ��[�U���Ȃ��BEXTRA���[�h�̒K�̗t���ςŕϐg���Ă�Ƃ������֌W����͂��B
		//�{���E��w�ƍ���w�ŕ�����ׂ��Ȃ̂��낤������ȕϐg�Ȃ��������ꏏ�����ɂ��Ă����B
		if (check_invalidate_inventory(INVEN_RIGHT)) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			//v1.1.86 �u�@�͌o�T�v�J�[�h�ɂ��[�U�u�[�X�g
			if (hiziri_card_num)
			{
				if (randint1(100) <= CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num)) o_ptr->timeout--;
			}
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;


			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}
		}
	}
}

///mod160509
/*:::�e�̑��U recharge()�̏\�{�̕p�x�ŌĂяo�����B
 *timeout�l�ɂ͒ʏ�̐�{�̒l���i�[����Ă���A��{100����������B
 *�e�n���x�Ȃǂɉ����Č����l����������B*/
static void process_world_aux_recharge_gun(void)
{
	int i;
	bool changed = FALSE;

	int hiziri_card_num;
	hiziri_card_num = count_ability_card(ABL_CARD_KYOUTEN);


	//�s���s�\���ɂ̓`���[�W����Ȃ�
	if(p_ptr->paralyzed || p_ptr->confused || (p_ptr->stun >= 100) 
		|| p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL)) return;

	if (p_ptr->special_defense & KATA_MASK) return;

	for (i = 0; i <= INVEN_LARM; i++)
	{
		/* Get the object */
		object_type *o_ptr = &inventory[i];

		//�Z�\�u�����e�i���X�v���Ȃ����葕�����̏e�ɂ����`���[�W����Ȃ�
		if(!HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_MAINTENANCE) && i < INVEN_RARM) continue;


		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval != TV_GUN) continue;
		if(check_invalidate_inventory(i)) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			int ammo_old,ammo_new;
			int charge = 100;
			int timeout_base = calc_gun_timeout(o_ptr) * 1000;

			ammo_old = o_ptr->timeout / timeout_base;

			charge = charge * 100 / calc_gun_charge_mult(i);

			//�����e�i���X�������Ă��������łȂ��e�̃`���[�W���x�͔���
			if(i < INVEN_RARM) charge /= 2;

			//v1.1.86�u�@�͌o�T�v�J�[�h�ɂ��[�U�u�[�X�g
			if (hiziri_card_num)
			{
				int prob = CALC_ABL_KYOUTEN_RECHARGE_BONUS(hiziri_card_num);//������
				//��{�[�U��*�������̐��������͂��̂܂܉��Z
				int bonus = charge * prob / 100;
				//��{�[�U��*�������̗]�蕔���͂����%�l�Ƃ��Ĕ���ʂ�Ή��Z
				if (randint1(100) <= (charge*prob) % 100) bonus++;

				charge += bonus;
			}


			o_ptr->timeout -= charge;

			if(o_ptr->timeout < 0) o_ptr->timeout = 0;

			ammo_new = o_ptr->timeout / timeout_base;

			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				changed = TRUE;
			}
			else if(ammo_old != ammo_new)
			{
				changed = TRUE;
				//v1.1.21 �K���J�^�͎c�e����̂Ƃ��������Ȃ������̂ł����Ń`�F�b�N
				if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_LARM].tval == TV_GUN)
					p_ptr->update |= PU_BONUS;
			}
		}
	}

	/* Notice changes */
	if (changed)
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP | PW_INVEN);
		wild_regen = 20;
	}

}

/*
 * Handle involuntary movement once every 10 game turns
 */
/*:::���̑����Ԍ���*/
static void process_world_aux_movement(void)
{
	/* Delayed Word-of-Recall */
	if (p_ptr->word_recall)
	{

		//v1.1.68 ���ł�p_ptr->leaving��TRUE�̂Ƃ��A�����^�C�~���O�ŋA�҂��������Ȃ��悤�ɃJ�E���g���ЂƂx�点��B
		//�����ɂ�turn�l��10�̔{���̂Ƃ��ɂ������Ȃ��̂ŁAturn��10�̔{���̎��Ɂ��̍s���������ĊK�ړ������Ă��ꂪ�A�҂̔����Əd�Ȃ����Ƃ��ɂ̂݌��݉�����s��������炵���B
		if (p_ptr->leaving && p_ptr->word_recall == 1)
		{
			if (cheat_xtra) msg_format("recall:%d(+1)",p_ptr->word_recall);
			p_ptr->word_recall++;
		}

		/*
		 * HACK: Autosave BEFORE resetting the recall counter (rr9)
		 * The player is yanked up/down as soon as
		 * he loads the autosaved game.
		 */
		if (autosave_l && (p_ptr->word_recall == 1) && !p_ptr->inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards recall */
		p_ptr->word_recall--;

		p_ptr->redraw |= (PR_STATUS);

		/* Activate the recall */
		if (!p_ptr->word_recall)
		{
			/* Disturbing! */
			disturb(0, 1);


			//���s���ۓW�̃g���b�v����
			if(p_ptr->inside_quest == QUEST_MOON_VAULT && quest[QUEST_MOON_VAULT].flags & QUEST_FLAG_SPECIAL)
			{
				msg_print("�n��̖����̏p���A�҂̏p��ł��������I");
			}

			/* Determine the level */
			else if (dun_level || p_ptr->inside_quest || p_ptr->enter_dungeon)
			{
				msg_print(_("��Ɉ������肠�����銴��������I",
							"You feel yourself yanked upwards!"));

				if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);

				dun_level = 0;
				dungeon_type = 0;

				leave_quest_check();
				leave_tower_check();

				p_ptr->inside_quest = 0;

				p_ptr->leaving = TRUE;
			}
			else
			{
				msg_print(_("���Ɉ�������~�낳��銴��������I",
							"You feel yourself yanked downwards!"));

				dungeon_type = p_ptr->recall_dungeon;

				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);
				///system �����G��΍D���ȊK�w�ɋA�҂ł�����
				/* New depth */
				dun_level = max_dlv[dungeon_type];
				if (dun_level < 1) dun_level = 1;

				/* Nightmare mode makes recall more dangerous */
				if (ironman_nightmare && !randint0(666) && (dungeon_type == DUNGEON_ANGBAND))
				{
					if (dun_level < 50)
					{
						dun_level *= 2;
					}
					else if (dun_level < 99)
					{
						dun_level = (dun_level + 99) / 2;
					}
					else if (dun_level > 100)
					{
						dun_level = d_info[dungeon_type].maxdepth - 1;
					}
				}

				if (p_ptr->wild_mode)
				{
					p_ptr->wilderness_y = py;
					p_ptr->wilderness_x = px;
				}
				else
				{
					/* Save player position */
					p_ptr->oldpx = px;
					p_ptr->oldpy = py;
				}
				p_ptr->wild_mode = FALSE;

				/*
				 * Clear all saved floors
				 * and create a first saved floor
				 */
				prepare_change_floor_mode(CFM_FIRST_FLOOR);

				/* Leaving */
				p_ptr->leaving = TRUE;

				if (dungeon_type == DUNGEON_ANGBAND)
				{
					int i;

					for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
					{
						if ((quest[i].type == QUEST_TYPE_RANDOM) &&
						    (quest[i].status == QUEST_STATUS_TAKEN) &&
						    (quest[i].level < dun_level))
						{
							quest[i].status = QUEST_STATUS_FAILED;
							quest[i].complev = (byte)p_ptr->lev;
							r_info[quest[i].r_idx].flags1 &= ~(RF1_QUESTOR);
						}
					}
				}
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}


	/* Delayed Alter reality */
	if (p_ptr->alter_reality)
	{

		//v1.1.68 ���ł�p_ptr->leaving��TRUE�̂Ƃ��A�����^�C�~���O�Ō����ϗe���������Ȃ��悤�ɃJ�E���g���ЂƂx�点��B������̋A�҂̏����Ɠ��l
		if (p_ptr->leaving && p_ptr->alter_reality == 1)
		{
			if (cheat_xtra) msg_format("alter_reality:%d(+1)", p_ptr->alter_reality);
			p_ptr->alter_reality++;
		}

		if (autosave_l && (p_ptr->alter_reality == 1) && !p_ptr->inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards alter */
		p_ptr->alter_reality--;

		p_ptr->redraw |= (PR_STATUS);

		/* Activate the alter reality */
		if (!p_ptr->alter_reality)
		{
			/* Disturbing! */
			disturb(0, 1);

			/* Determine the level */
			if (!quest_number(dun_level) && dun_level)
			{
#ifdef JP
				msg_print("���E���ς�����I");
#else
				msg_print("The world changes!");
#endif

				/*
				 * Clear all saved floors
				 * and create a first saved floor
				 */
				prepare_change_floor_mode(CFM_FIRST_FLOOR);

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
			else
			{
#ifdef JP
				msg_print("���E�������̊ԕω������悤���B");
#else
				msg_print("The world seems to change for a moment!");
#endif
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}
}


/*
 * Count number of adjacent monsters
 */
static int get_monster_crowd_number(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	int my = m_ptr->fy;
	int mx = m_ptr->fx;
	int i;
	int count = 0;

	for (i = 0; i < 7; i++)
	{
		int ay = my + ddy_ddd[i];
		int ax = mx + ddx_ddd[i];

		if (!in_bounds(ay, ax)) continue;

		/* Count number of monsters */
		if (cave[ay][ax].m_idx > 0) count++;
 	}

	return count;
}



/*
 * Dungeon rating is no longer linear
 */
#define RATING_BOOST(delta) (delta * delta + 50 * delta)

/*
 * Examine all monsters and unidentified objects,
 * and get the feeling of current dungeon floor
 */
/*:::���͋C���v�Z����B���̃t���A�̃����X�^�[���ƐG���ĂȂ��A�C�e�����K���x���Ɣ�r*/
static byte get_dungeon_feeling(void)
{
	const int base = 10;
	int rating = 0;
	int i;

	/* Hack -- no feeling in the town */
	if (!dun_level) return 0;

	/* Examine each monster */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr;
		int delta = 0;

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Ignore pet */
		if (is_pet(m_ptr)) continue;

		if(m_ptr->r_idx == MON_MASTER_KAGUYA) continue;

		r_ptr = &r_info[m_ptr->r_idx];

		/* Unique monsters */
		///mod150224 UNIQUE2���܂ނ��Ƃɂ���
		if (r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & (RF7_UNIQUE2))
		{
			/* Nearly out-of-depth unique monsters */
			if (r_ptr->level + 10 > dun_level)
			{
				/* Boost rating by twice delta-depth */
				delta += (r_ptr->level + 10 - dun_level) * 2 * base;
			}
		}
		else
		{
			/* Out-of-depth monsters */
			if (r_ptr->level > dun_level)
			{
				/* Boost rating by delta-depth */
				delta += (r_ptr->level - dun_level) * base;
			}
		}

		/* Unusually crowded monsters get a little bit of rating boost */
		if (r_ptr->flags1 & RF1_FRIENDS)
		{
			if (5 <= get_monster_crowd_number(i)) delta += 1;
		}
		else
		{
			if (2 <= get_monster_crowd_number(i)) delta += 1;
		}


		rating += RATING_BOOST(delta);
	}

	/* Examine each unidentified object */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];
		object_kind *k_ptr = &k_info[o_ptr->k_idx];
		int delta = 0;

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip known objects */
		if (object_is_known(o_ptr))
		{
			/* Touched? */
			if (o_ptr->marked & OM_TOUCHED) continue;
		}

		/* Skip pseudo-known objects */
		if (o_ptr->ident & IDENT_SENSE) continue;

		///mod160314 �A�C�e���J�[�h�@�Œᐶ���K�ƃR�X�g�œK���Ɍv�Z
		if(o_ptr->tval == TV_ITEMCARD)
		{
			int fake_lev = (support_item_list[o_ptr->pval].min_lev + support_item_list[o_ptr->pval].cost * 4) / 2;

			if(fake_lev > dun_level) delta += (fake_lev - dun_level) * base;

		}

		/* Ego objects */
		if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			delta += e_ptr->rating * base;
		}

		/* Artifacts */
		if (object_is_artifact(o_ptr))
		{
			s32b cost = object_value_real(o_ptr);

			delta += 10 * base;
			if (cost > 10000L) delta += 10 * base;
			if (cost > 50000L) delta += 10 * base;
			if (cost > 100000L) delta += 10 * base;

			/* Special feeling */
			if (!preserve_mode) return 1;
		}
		///item ���͋C�v�Z
		///mod131223 ���͋C�v�Z tval
		if (o_ptr->tval == TV_DRAG_ARMOR) delta += 30 * base;
		if (o_ptr->tval == TV_DRAG_ARMOR && o_ptr->sval >=  SV_DRAGARMOR_MULTI) delta += 30 * base;

		if (o_ptr->tval == TV_SHIELD &&
		    o_ptr->sval == SV_DRAGON_SHIELD) delta += 5 * base;
		if (o_ptr->tval == TV_GLOVES &&
		    o_ptr->sval == SV_HAND_DRAGONGLOVES) delta += 5 * base;
		if (o_ptr->tval == TV_BOOTS &&
		    o_ptr->sval == SV_LEG_DRAGONBOOTS) delta += 5 * base;
		if (o_ptr->tval == TV_HEAD &&
		    o_ptr->sval == SV_HEAD_DRAGONHELM) delta += 5 * base;
		if (o_ptr->tval == TV_RING &&
		    o_ptr->sval == SV_RING_SPEED &&
		    !object_is_cursed(o_ptr)) delta += 25 * base;
		if (o_ptr->tval == TV_RING &&
		    o_ptr->sval == SV_RING_LORDLY &&
		    !object_is_cursed(o_ptr)) delta += 15 * base;
		if (o_ptr->tval == TV_AMULET &&
		    o_ptr->sval == SV_AMULET_THE_MAGI &&
		    !object_is_cursed(o_ptr)) delta += 15 * base;

		/* Out-of-depth objects */
		if (!object_is_cursed(o_ptr) && !object_is_broken(o_ptr) &&
		    k_ptr->level > dun_level)
		{
			/* Rating increase */
			delta += (k_ptr->level - dun_level) * base;
		}

		rating += RATING_BOOST(delta);
	}


	if (rating > RATING_BOOST(1000)) return 2;
	if (rating > RATING_BOOST(800)) return 3;
	if (rating > RATING_BOOST(600)) return 4;
	if (rating > RATING_BOOST(400)) return 5;
	if (rating > RATING_BOOST(300)) return 6;
	if (rating > RATING_BOOST(200)) return 7;
	if (rating > RATING_BOOST(100)) return 8;
	if (rating > RATING_BOOST(0)) return 9;

	return 10;
}


/*
 * Update dungeon feeling, and announce it if changed
 */
/*:::���͋C���m����Ə��� 1turn���ƂɌĂ΂��*/
//v1.1.59 static���O���A�������m�t���O�ǉ�
void update_dungeon_feeling(bool flag_feel_now)
{
	byte new_feeling;
	int quest_num;
	int delay;

	/* No feeling on the surface */
	if (!dun_level) return;

	/* No feeling in the arena */
	if (p_ptr->inside_battle) return;

	/* Extract delay time */
	delay = MAX(10, 150 - p_ptr->skill_fos) * (150 - dun_level) * TURNS_PER_TICK / 100;
	if(p_ptr->pclass == CLASS_IKU) delay = 10 * 30 * TURNS_PER_TICK / 100;//���ꏈ���ōő����m

 	/* Not yet felt anything */
	//if (turn < p_ptr->feeling_turn + delay && !cheat_xtra) return;
	///mod140216 �ߋ�͈�u�ŕ��͋C�����m����
	///mod140907 �E�E�\�肾�������A�ǂ������������������Ȃ�̂ň�u�łȂ��ő��ɂ���
	//if (turn < (p_ptr->feeling_turn + delay) && !cheat_xtra && p_ptr->pclass != CLASS_IKU) return;
	if (!flag_feel_now && turn < (p_ptr->feeling_turn + delay) && !cheat_xtra) return;

	/* Extract quest number (if any) */
	quest_num = quest_number(dun_level);
	///system �N�G�X�g�֘A
	/* No feeling in a quest */
	if (quest_num &&
	    (is_fixed_quest_idx(quest_num) &&
	  //   !((quest_num == QUEST_TAISAI) || (quest_num == QUEST_SERPENT) ||
	     !((quest_num == QUEST_TAISAI) || (quest_num == QUEST_YUKARI) ||
	       !(quest[quest_num].flags & QUEST_FLAG_PRESET)))) return;

	if(EXTRA_QUEST_FLOOR) return;

	/* Get new dungeon feeling */
	new_feeling = get_dungeon_feeling();

	/* Remember last time updated */
	p_ptr->feeling_turn = turn;

	/* No change */
	if (p_ptr->feeling == new_feeling) return;

	/* Dungeon feeling is changed */
	p_ptr->feeling = new_feeling;

	/* Announce feeling */
	do_cmd_feeling();

	select_floor_music(); //v1.1.58

	/* Update the level indicator */
	p_ptr->redraw |= (PR_DEPTH);

	/* Disturb */
	if (disturb_minor) disturb(0, 0);
}


/*
 * Handle certain things once every 10 game turns
 */
/*:::1�Q�[���^�[�����ƁA10�Q�[���^�[�����Ƃɍs���鏈��*/
static void process_world(void)
{
	int day, hour, min;

	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;
	s32b prev_turn_in_today = ((turn - TURNS_PER_TICK) % A_DAY + A_DAY / 4) % A_DAY;
	int prev_min = (1440 * prev_turn_in_today / A_DAY) % 60;
	
	extract_day_hour_min(&day, &hour, &min);

	/* Update dungeon feeling, and announce it if changed */
	update_dungeon_feeling(FALSE);

	///mod160508 �e�̏[�U����
	process_world_aux_recharge_gun();

	///mod160814 �T�O���u�_�X�̒e���v���Ŕ���
	if(p_ptr->pclass == CLASS_SAGUME && p_ptr->tim_general[0] && p_ptr->csp < 1)
	{
		set_tim_general(0,TRUE,0,0);
	}

	///mod150616 ���Z��ύX
	if (p_ptr->inside_battle && !p_ptr->leaving)
	{
		int i;
		int win_m_idx = 0;
		int number_mon[4] = {0,0,0,0};
		int number_team = 0;
		u32b bitflag_survive = 0L;

		/* Examine each monster */
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			if(!m_ptr->r_idx) continue;
			if(i == p_ptr->riding) continue;

			if(!(m_ptr->mflag & MFLAG_BATTLE_MON_MASK)) 
				msg_print("WARNING:���Z��ɂǂ̃`�[���ɂ���������n���ł��Ȃ������X�^�[������");
			else
				bitflag_survive |= (MFLAG_BATTLE_MON_MASK & m_ptr->mflag);

			win_m_idx = i;
		}


		if (!bitflag_survive)
		{
			p_ptr->au -= battle_mon_wager;
			gambling_gain -= battle_mon_wager;
			msg_print("�u���[�A����ᑊ�������ˁB�������Ǔ����̑���肳�B�v");
			msg_print(NULL);
			p_ptr->energy_need = 0;
			//battle_monsters();
		}
		else if (count_bits(bitflag_survive) == 1)
		{

			char m_name[80];
			monster_type *wm_ptr;

			p_ptr->au -= battle_mon_wager;
			gambling_gain -= battle_mon_wager;
	
			if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_A) i = 0;
			else if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_B) i = 1;
			else if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_C) i = 2;
			else if(bitflag_survive == MFLAG_BATTLE_MON_TEAM_D) i = 3;
			if(battle_mon_team_type[i] == 0)
			{
				wm_ptr = &m_list[win_m_idx];
				monster_desc(m_name, wm_ptr, 0);
				msg_format("%s�����������I", m_name);
			}
			else if(battle_mon_team_type[i] < 0)
			{
				msg_format("%s�����������I", battle_mon_special_team_list[0-battle_mon_team_type[i]].name);
			}
			else
			{
				msg_format("%s�����������I", battle_mon_random_team_name_list[battle_mon_team_type[i]]);
			}
			msg_print(NULL);


			if(battle_mon_team_bet & bitflag_survive )
			{
				int reward;

				reward = battle_mon_wager / 100 * battle_mon_odds;
				if(reward > 9999999) reward = 9999999;

				if(battle_mon_odds < 1000 && p_ptr->psex == SEX_MALE) msg_print("�u���߂łƂ��I�Z����̏������I�v");
				else if(battle_mon_odds < 1000 ) msg_print("�u���߂łƂ��I������̏������I�v");
				else if(reward < 1000000 )
				{
					//�匊�𓖂Ă�ƒ����x�啝�}�C�i�X�A�����啝�v���X
					msg_print("�u����������Ȃ����A�匊����I�v");
					set_deity_bias(DBIAS_REPUTATION, 3);
					set_deity_bias(DBIAS_COSMOS, -5);
				}
				else if(reward < 9999999)
				{
					msg_print("�u�ǂ����悤�A�R�̐_�l����a�������_���H����E�E�v");
					set_deity_bias(DBIAS_REPUTATION, 10);
					set_deity_bias(DBIAS_COSMOS, -30);
				}
				else
				{
					msg_print("�����͓̉��͖A�𐁂��ē|��A�ق��͓̉����Q�Ăĉ�����n�߂��B");
					set_deity_bias(DBIAS_REPUTATION, 100);
					set_deity_bias(DBIAS_COSMOS, -200);
				}
				msg_print(NULL);
				msg_format("%d�����󂯎�����B", reward);
				p_ptr->au += reward;

				//v1.1.25 �M�����u�����x���J�E���g���邱�Ƃɂ���
				gambling_gain += reward;
				if(gambling_gain > 99999999) gambling_gain = 99999999;

			}
			else
			{
				msg_print("�u�c�O�������ˁB�܂����肸�ɂ܂����Ă�B�v");
			}
			msg_print(NULL);
			p_ptr->energy_need = 0;
			//battle_monsters();
		}
		else if (turn - old_turn == 300 * TURNS_PER_TICK)
		{
			msg_print("�u�^�C���A�b�v���B�c�O�����Ǖԋ������B�v");
			msg_print(NULL);
			p_ptr->energy_need = 0;
		}
		//����ȊO�̂Ƃ��A�܂����������ĂȂ�

		//v1.1.96 ���Z��Łu�����Ȃ��v�ɂȂ�Ȃ��悤�ɒǉ��@���Ԃ񂱂̊֐��ł����͂�
		if (turn % 10 == 0) Term_flush();

	}

	/*** Check monster arena ***/

//���Z��̌��̃��[�`��
#if 0
	if (p_ptr->inside_battle && !p_ptr->leaving)
	{
		int i2, j2;
		int win_m_idx = 0;
		int number_mon = 0;

		/* Count all hostile monsters */
		for (i2 = 0; i2 < cur_wid; ++i2)
			for (j2 = 0; j2 < cur_hgt; j2++)
			{
				cave_type *c_ptr = &cave[j2][i2];

				if ((c_ptr->m_idx > 0) && (c_ptr->m_idx != p_ptr->riding))
				{
					number_mon++;
					win_m_idx = c_ptr->m_idx;
				}
			}

		if (number_mon == 0)
		{
#ifdef JP
			msg_print("���ł��ɏI���܂����B");
#else
			msg_print("They have kill each other at the same time.");
#endif
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else if ((number_mon-1) == 0)
		{
			char m_name[80];
			monster_type *wm_ptr;

			wm_ptr = &m_list[win_m_idx];

			monster_desc(m_name, wm_ptr, 0);
#ifdef JP
			msg_format("%s�����������I", m_name);
#else
			msg_format("%s is winner!", m_name);
#endif
			msg_print(NULL);

			if (win_m_idx == (sel_monster+1))
			{
#ifdef JP
				msg_print("���߂łƂ��������܂��B");
#else
				msg_print("Congratulations.");
#endif
#ifdef JP
				msg_format("%d�����󂯎�����B", battle_odds);
#else
				msg_format("You received %d gold.", battle_odds);
#endif
				p_ptr->au += battle_odds;
			}
			else
			{
#ifdef JP
				msg_print("�c�O�ł����B");
#else
				msg_print("You lost gold.");
#endif
			}
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else if (turn - old_turn == 150*TURNS_PER_TICK)
		{
#ifdef JP
			msg_print("�\����������܂��񂪁A���̏����͈��������Ƃ����Ă��������܂��B");
#else
			msg_format("This battle have ended in a draw.");
#endif
			p_ptr->au += kakekin;
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
	}
#endif



	//v2.0.1 �u���������̖�v�̃J�[�h�Ŕ������N����Ƃ�
	if (flag_life_explode && !p_ptr->is_dead)
	{
		flag_life_explode = FALSE;
		project(0, 5, py, px, p_ptr->csp, GF_FORCE, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);
	}




	/* Every 10 game turns */
	if (turn % TURNS_PER_TICK) return;
	/*:::���������A�����I10turn(�Q�[�����1�^�[��)���Ƃ̏���*/

	//v2.0.6 �ޖ����H���œ������Z��ϐ���������J�E���g�_�E��
	if (p_ptr->pclass == CLASS_YUMA && turn % YUMA_FLAG_DELETE_TICK == 0) yuma_lose_extra_power(1);

	/*** Check the Time and Load ***/
	///system �A���O�o���h�ւ̖�E�E�E�H
	///sysdel
	if (!(turn % (50*TURNS_PER_TICK)))
	{
		/* Check time and load */
		if ((0 != check_time()) || (0 != check_load()))
		{
			/* Warning */
			if (closing_flag <= 2)
			{
				/* Disturb */
				disturb(0, 1);

				/* Count warnings */
				closing_flag++;

				/* Message */
#ifdef JP
msg_print("�A���O�o���h�ւ̖傪���������Ă��܂�...");
msg_print("�Q�[�����I�����邩�Z�[�u���邩���ĉ������B");
#else
				msg_print("The gates to ANGBAND are closing...");
				msg_print("Please finish up and/or save your game.");
#endif

			}

			/* Slam the gate */
			else
			{
				/* Message */
#ifdef JP
msg_print("���A�A���O�o���h�ւ̖傪������܂����B");
#else
				msg_print("The gates to ANGBAND are now closed.");
#endif


				/* Stop playing */
				p_ptr->playing = FALSE;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}
	}

	/*** Attempt timed autosave ***/
	/*:::�I�[�g�Z�[�u*/
	if (autosave_t && autosave_freq && !p_ptr->inside_battle)
	{
		if (!(turn % ((s32b)autosave_freq * TURNS_PER_TICK)))
			do_cmd_save_game(TRUE);
	}

	if (mon_fight && !ignore_unview)
	{
#ifdef JP
		msg_print("���������������B");
#else
		msg_print("You hear noise.");
#endif
	}
	//�i�ԃv���C���ɋP�邪���E�O�Ń_���[�W�󂯂��Ƃ�kaguya_fight�Ɏc��HP�������L�^�����
	if(kaguya_fight && !ignore_unview && kaguya_fight < 100)
	{

		if(kaguya_fight > 80)
			msg_print("�P�邪��풆�̂悤���B");
		else if(kaguya_fight > 50)
			msg_print("�P��͋�킵�Ă���悤���B");
		else
			msg_print("�P�邪�댯���I");

		if(kaguya_fight <= 80)disturb(0,1);


		kaguya_fight = 100;

	}

	/*** Handle the wilderness/town (sunshine) ***/

	///mod140821 �G���W�j�A�M���h�̏��Օi�̒l�i���ς��
	if (!(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2))) engineer_guild_price_change();

	///mod160522 ���̎x�����������J�E���^�@���悻15�����Ƃ�
	if(p_ptr->pclass == CLASS_RINGO && !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 1000)))
	{
		p_ptr->magic_num1[4] += dun_level / 10 + 1;
	}

	/* While in town/wilderness */
	///system�@�����t���O�Ƃ���������Ȃ炱����
	if (!dun_level && !p_ptr->inside_quest && !p_ptr->inside_battle && !p_ptr->inside_arena)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if (!(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
		{
			bool dawn;

			//��̉��O�Ńp���[�A�b�v����d��
			if(CHECK_FAIRY_TYPE == 30)
			{
				p_ptr->redraw |= (PR_STATE);
				p_ptr->update |= (PU_BONUS);
				handle_stuff();
			}

			/* Check for dawn */
			dawn = (!(turn % (TURNS_PER_TICK * TOWN_DAWN)));

			/* Day breaks */
			/*:::�邪��������}�b�v�𖾂邭����*/
			if (dawn)
			{
				int y, x;

				msg_print("�邪�������B");

				if (!p_ptr->wild_mode)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < cur_hgt; y++)
					{
						for (x = 0; x < cur_wid; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &cave[y][x];

							/* Assume lit */
							c_ptr->info |= (CAVE_GLOW);

							/* Hack -- Memorize lit grids if allowed */
							if (view_perma_grids) c_ptr->info |= (CAVE_MARK);

							/* Hack -- Notice spot */
							note_spot(y, x);
						}
					}
				}
			}

			/*:::��ɂȂ�����}�b�v���Â�����*/
			/* Night falls */
			else
			{
				int y, x;

				/* Message */
#ifdef JP
				msg_print("�������񂾁B");
#else
				msg_print("The sun has fallen.");
#endif

				if (!p_ptr->wild_mode)
				{
					/* Hack -- Scan the town */
					for (y = 0; y < cur_hgt; y++)
					{
						for (x = 0; x < cur_wid; x++)
						{
							/* Get the cave grid */
							cave_type *c_ptr = &cave[y][x];

							/* Feature code (applying "mimic" field) */
							feature_type *f_ptr = &f_info[get_feat_mimic(c_ptr)];

							if (!is_mirror_grid(c_ptr) && !have_flag(f_ptr->flags, FF_QUEST_ENTER) &&
							    !have_flag(f_ptr->flags, FF_ENTRANCE))
							{
								/* Assume dark */
								c_ptr->info &= ~(CAVE_GLOW);

								if (!have_flag(f_ptr->flags, FF_REMEMBER))
								{
									/* Forget the normal floor grid */
									c_ptr->info &= ~(CAVE_MARK);

									/* Hack -- Notice spot */
									note_spot(y, x);
								}
							}
						}

						/* Glow deep lava and building entrances */
						glow_deep_lava_and_bldg();
					}
				}
			}

			/* Update the monsters */
			p_ptr->update |= (PU_MONSTERS | PU_MON_LITE);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			if (p_ptr->special_defense & NINJA_S_STEALTH)
			{
				if (cave[py][px].info & CAVE_GLOW) set_superstealth(FALSE);
			}
		}
	}

	//v1.1.86 �����Ɉ�x�A�����s��̃g���[�h�J�E���g�����Z�b�g
	//v1.1.87�A�r���e�B�J�[�h�̍����x��ݒ肵�ăJ�[�h�V���b�v�̍݌ɂ�ݒ�
	//EXTRA���[�h�ł͑��Ōv�Z����
	if (!(turn % (TURNS_PER_TICK * TOWN_DAWN / 2)) && !EXTRA_MODE)
	{
		ability_card_trade_count = 0;
		set_abilitycard_price_rate();
		make_ability_card_store_list();

		break_market();

	}

	//��ǐ_�l�͓����o�߂ōD��x�Ɩ����ω�
	if(p_ptr->prace == RACE_STRAYGOD && !(turn % (TURNS_PER_TICK * TOWN_DAWN)))
	{
		if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_WARLIKE] > 60) set_deity_bias(DBIAS_WARLIKE, -3);
		if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 80) set_deity_bias(DBIAS_REPUTATION, -1);
	}

	///mod151114 ���̐l�`�̔�
	if(p_ptr->pclass == CLASS_HINA && !(turn % (TURNS_PER_TICK * TOWN_DAWN / 24)))
	{
		p_ptr->magic_num1[1] += randint1((p_ptr->lev + p_ptr->stat_ind[A_CHR] + 3 + 20) * 5);
		if(p_ptr->magic_num1[1] > 100000) p_ptr->magic_num1[1] = 100000;
	}
	//���u�􂢂̐��l�`�v�̖����
	if (p_ptr->pclass == CLASS_HINA && (p_ptr->special_defense & SD_HINA_NINGYOU))
	{
		hina_gain_yaku(-(50+randint1(100)));
		if(!p_ptr->magic_num1[0])
		{
			msg_print("������c���Ă��Ȃ��B");
			p_ptr->special_defense &= ~(SD_HINA_NINGYOU);
			p_ptr->redraw |= (PR_STATUS);
		}
	}

	//�����̑����i�������������� ������100�J�E���g���Ėv��
	if (p_ptr->pclass == CLASS_SHION && !(turn % (TURNS_PER_TICK * TOWN_DAWN / 200)))
	{
		int i;
		for (i = INVEN_RARM; i <= INVEN_FEET; i++)
		{
			shion_seizure(i);

		}

	}


	///mod140412 ����� 
	///mod150121 �d������
	if((prace_is_(RACE_VAMPIRE) 
		|| (prace_is_(RACE_WARBEAST) && p_ptr->pclass != CLASS_KEINE) 
		||p_ptr->mimic_form == MIMIC_VAMPIRE)
		&& !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
	{
		bool dawn;
		bool beast = prace_is_(RACE_WARBEAST);


		dawn = (!(turn % (TURNS_PER_TICK * TOWN_DAWN)));
		if(dawn)
		{
			if(beast)
			{
				if(turn / A_DAY % 30 + 15 == 30 && (dun_level))
				{
					msg_print("�邪�������炵���B");
					disturb(0,1);
				}
			}
			else
			{
				if(dun_level) msg_print("�邪�������炵���B");
				msg_print("�̂����邢�E�E");
				disturb(0,1);
			}
		}
		else
		{
			if(beast)
			{
				if(turn / A_DAY % 30 + 16 == 30)
				{
					if(dun_level) msg_print("�������񂾂炵���B");
					msg_print("�����̔ӂ��I�͂��݂Ȃ���I");
					disturb(0,1);
				}
			}
			else
			{
				if(dun_level) msg_print("�������񂾂炵���B");
				if(turn / A_DAY % 30 + 16 == 30)
					msg_print("�����̔ӂ��I�͂��݂Ȃ���I");
				else 
					msg_print("�͂��N���Ă����B");
				disturb(0,1);
			}
		}
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();
	}
	else if(p_ptr->pclass == CLASS_SUNNY && !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)) )
	{
		if(!(turn % (TURNS_PER_TICK * TOWN_DAWN)))
		{
			if(dun_level) msg_format("�邪�������悤���B");
			msg_format("���C���o�Ă����I");
		}
		else
		{
			if(dun_level) msg_format("�������񂾂悤���B");
			if(dun_level) msg_format("���C���o�Ȃ��E�E");
		}
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

	}
	else if(p_ptr->pclass == CLASS_LUNAR && !(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)) )
	{
		if(!(turn % (TURNS_PER_TICK * TOWN_DAWN)))
		{
			if(dun_level) msg_format("�邪�������悤���B");
			msg_format("���C���o�Ȃ��E�E");
		}
		else
		{
			if(dun_level) msg_format("�������񂾂悤���B");
			msg_format("���C���o�Ă����B");
		}
			p_ptr->redraw |= (PR_STATE);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();

	}

	
	///mod150121 �d�����ꏈ��
	else if(p_ptr->pclass == CLASS_KEINE && !(p_ptr->inside_battle))
	{
		//�������ɒn��ɂ���Ɣ���
		if(FULL_MOON && !dun_level && !p_ptr->magic_num1[0])
		{
			//���[���h�}�b�v����
			if(p_ptr->wild_mode)
			{
				energy_use = MIN(energy_use,100);
				change_wild_mode();
			}
			//�ϐg����
			if(p_ptr->mimic_form) set_mimic(0,0,TRUE);
			p_ptr->muta2_perma &= ~(MUT2_HARDHEAD);
			p_ptr->muta2 &= ~(MUT2_HARDHEAD);
			p_ptr->muta2 |= (MUT2_BIGHORN);//�p�ψ�
			p_ptr->muta2_perma |= (MUT2_BIGHORN);
			msg_print("�������B���̐F���ς��A����̊p�����ꂽ�E�E");
			//�ꍇ�ɂ���Ă͑�������
			if(inventory[INVEN_HEAD].k_idx && 
			(	inventory[INVEN_HEAD].sval == SV_HEAD_METALCAP 
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STARHELM 
			||  inventory[INVEN_HEAD].sval == SV_HEAD_STEELHELM 
			||  inventory[INVEN_HEAD].sval == SV_HEAD_DRAGONHELM  ))
			{
				char obj_name[80];
				object_desc(obj_name, &inventory[INVEN_HEAD], 0);

				msg_format("%s���E���Ă��܂����I",obj_name);
				inven_takeoff(INVEN_HEAD,255);
				calc_android_exp();
				p_ptr->redraw |= (PR_EQUIPPY);
			}
			p_ptr->update |= (PU_BONUS);
			p_ptr->magic_num1[0] = 1;//�t���O
			handle_stuff();

		}
		//���򉻉���
		else if(!(FULL_MOON) && p_ptr->magic_num1[0])
		{
			if(p_ptr->wild_mode)
			{
				energy_use = MIN(energy_use,100);
				change_wild_mode();
			}
			//�ϐg����
			if(p_ptr->mimic_form) set_mimic(0,0,TRUE);
			msg_print("�閾�����B���̐F���߂�A����̊p���������B");
			p_ptr->muta2_perma &= ~(MUT2_BIGHORN);
			p_ptr->muta2 &= ~(MUT2_BIGHORN);
			p_ptr->muta2 |= (MUT2_HARDHEAD);//�Γ�
			p_ptr->muta2_perma |= (MUT2_HARDHEAD);
			p_ptr->update |= (PU_BONUS);
			p_ptr->magic_num1[0] = 0;//�t���O
			handle_stuff();

		}
	}


#if 0
	/* While in the dungeon (vanilla_town or lite_town mode only) */
	/*:::���K�͂ȊX�A���c�̒��̃_���W�������ł��܂ɓX������*/
	if ((vanilla_town || (lite_town && !p_ptr->inside_quest && !p_ptr->inside_battle && !p_ptr->inside_arena)) && dun_level)
	{
		/*** Shuffle the Storekeepers ***/

		/* Chance is only once a day (while in dungeon) */
		if (!(turn % (TURNS_PER_TICK * STORE_TICKS)))
		{
			/* Sometimes, shuffle the shop-keepers */
			if (one_in_(STORE_SHUFFLE))
			{
				int n, i;

				/* Pick a random shop (except home and museum) */
				do
				{
					n = randint0(MAX_STORES);
				}
				while ((n == STORE_HOME) || (n == STORE_MUSEUM));

				/* Check every feature */
				for (i = 1; i < max_f_idx; i++)
				{
					/* Access the index */
					feature_type *f_ptr = &f_info[i];

					/* Skip empty index */
					if (!f_ptr->name) continue;

					/* Skip non-store features */
					if (!have_flag(f_ptr->flags, FF_STORE)) continue;

					/* Verify store type */
					if (f_ptr->subtype == n)
					{
						/* Message */
#ifdef JP
						if (cheat_xtra) msg_format("%s�̓X����V���b�t�����܂��B", f_name + f_ptr->name);
#else
						if (cheat_xtra) msg_format("Shuffle a Shopkeeper of %s.", f_name + f_ptr->name);
#endif

						/* Shuffle it */
						//store_shuffle(n);

						break;
					}
				}
			}
		}
	}
#endif

	/*** Process the monsters ***/
	/*:::�_���W�����ɐV���ȃ����X�^�[���N������*/
	/* Check for creature generation. */
	if (one_in_(d_info[dungeon_type].max_m_alloc_chance) &&
	    !p_ptr->inside_arena && !p_ptr->inside_quest && !p_ptr->inside_battle && !(EXTRA_QUEST_FLOOR))
	{
		/* Make a new monster */
		(void)alloc_monster(MAX_SIGHT + 5, 0);
	}

	/* Hack -- Check for creature regeneration */
	/*:::�����X�^�[������*/
	if (!(turn % (TURNS_PER_TICK*10)) && !p_ptr->inside_battle) regen_monsters();
	if (!(turn % (TURNS_PER_TICK*3))) regen_captured_monsters();

	/*:::�t�r�_�g���̋A�Җ��ߏ���*/
	if (!(turn % (TURNS_PER_TICK*10)) && p_ptr->pclass == CLASS_TSUKUMO_MASTER && !(p_ptr->inside_battle)) tsukumo_recall();


	if (!p_ptr->leaving)
	{
		int i;

		/* Hack -- Process the counters of monsters if needed */
		/*:::MAX_MTIMED=16*/
		for (i = 0; i < MAX_MTIMED; i++)
		{
			if (mproc_max[i] > 0) process_monsters_mtimed(i);
		}
	}

	/*:::�o�C�h�זE�N�H�ψق��󂯂Ă���ƃJ�E���g�J�n�@3���ŃQ�[���I�[�o�[�@���Â��Ă��J�E���g�̓��Z�b�g����Ȃ�*/
	if(p_ptr->muta3 & MUT3_BYDO)
	{
		p_ptr->bydo += TURNS_PER_TICK;
		if((s32b)p_ptr->bydo > A_DAY * 3) p_ptr->bydo = A_DAY * 3+1;
		if((s32b)p_ptr->bydo > A_DAY * 3 && !(p_ptr->inside_battle)) take_hit(DAMAGE_LOSELIFE,9999,"�Ă̗[���@�₳�����}���Ă����̂́@�C���B�����Ȃ̂��H",0);
	}

	/*:::�����[�z���C�g�̃J�E���g�_�E��*/
	if(p_ptr->pclass == CLASS_LILY_WHITE)
	{
		if(turn > A_DAY * LILY_LIMIT && !(p_ptr->inside_battle)) take_hit(DAMAGE_LOSELIFE,9999,"�t�̏I���",0);
	}
	/*:::���̃J�E���g�_�E��*/
	if(p_ptr->pclass == CLASS_RAN && !p_ptr->total_winner)
	{
		if(turn > A_DAY * RAN_LIMIT && !(p_ptr->inside_battle)) take_hit(DAMAGE_LOSELIFE,9999,"�傩��̍ċ���",0);
	}
	//v1.1.92 ����(��p���i)�̃c�P�̎�藧��
	if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && p_ptr->magic_num1[4] && !CHECK_JYOON_BUY_ON_CREDIT)
	{
		msg_print("���ɓX�傽����Ⴢ��؂炵���I");
		msg_print(NULL);
		p_ptr->au -= p_ptr->magic_num1[4];
		p_ptr->magic_num1[4] = 0;
		p_ptr->redraw |= PR_GOLD;
		redraw_stuff();
		msg_print("�c�P����藧�Ă��Ă��܂���...");

	}

	/* Date changes */
	/*���t���ς�����Ƃ��̏����@���L�L���Ɩ{���̏܋���ύX*/
	///system
	if (!hour && !min)
	{
		if (min != prev_min)
		{
			p_ptr->today_mon = 0; //�얲�肢���Z�b�g


			do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

			//v2.05 �͂��Ă̐l�T�����Z�b�g EXTRA�ł͓����o�߂łȂ��Ώۃt���A�ʉ߂Ń��Z�b�g����̂ł����ł͉������Ȃ�
			if (!EXTRA_MODE)
			{
				p_ptr->hatate_mon_search_ridx = 0;
				p_ptr->hatate_mon_search_dungeon = 0;
			}


		//	determine_today_mon(FALSE);
		}
	}
	/*:::�������[�h�̑��Ẩ��O����*/
	/*
	 * Nightmare mode activates the TY_CURSE at midnight
	 *
	 * Require exact minute -- Don't activate multiple times in a minute
	 */
	if (ironman_nightmare && (min != prev_min))
	{
		/* Every 15 minutes after 11:00 pm */
		if ((hour == 23) && !(min % 15))
		{
			/* Disturbing */
			disturb(0, 1);

			switch (min / 15)
			{
			case 0:
#ifdef JP
				msg_print("�����ŕs�C���ȏ��̉��������B");
#else
				msg_print("You hear a distant bell toll ominously.");
#endif
				break;

			case 1:
#ifdef JP
				msg_print("�����ŏ����������B");
#else
				msg_print("A distant bell sounds twice.");
#endif
				break;

			case 2:
#ifdef JP
				msg_print("�����ŏ����O������B");
#else
				msg_print("A distant bell sounds three times.");
#endif
				break;

			case 3:
#ifdef JP
				msg_print("�����ŏ����l������B");
#else
				msg_print("A distant bell tolls four times.");
#endif
				break;
			}
		}

		/* TY_CURSE activates at midnight! */
		if (!hour && !min)
		{
			int count = 0;

			disturb(1, 1);
#ifdef JP
			msg_print("�����ŏ����������A���񂾂悤�ȐÂ����̒��֏����Ă������B");
#else
			msg_print("A distant bell tolls many times, fading into an deathly silence.");
#endif

			activate_ty_curse(FALSE, &count);
		}
	}


	/*** Check the Food, and Regenerate ***/
	/*:::���̖����x����*/
	///system ���@�g����d���l�`�͂����ŋ󕠂ɂȂ�Ȃ��悤�ɂ���΂����Hset_food���ł�����ق���������
	if (!p_ptr->inside_battle)
	{
		/* Digest quickly when gorged */
		if (p_ptr->food >= PY_FOOD_MAX)
		{
			/* Digest a lot of food */
			(void)set_food(p_ptr->food - 100);
		}

		/* Digest normally -- Every 50 game turns */
		///sys �}�񕜂�ݐς�����ɂ͂��̕ӂ�����K�v����
		else if (!(turn % (TURNS_PER_TICK*5)))
		{
			/* Basic digestion rate based on speed */
			//������10,����+10��20
			int digestion = SPEED_TO_ENERGY(p_ptr->pspeed);

			/* Regeneration takes more food */
			if (p_ptr->regenerate)
				digestion += 20;
			if (p_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
				digestion += 20;
			if (p_ptr->cursed & TRC_FAST_DIGEST)
				digestion += 30;

			if(p_ptr->pclass == CLASS_YOSHIKA)
				digestion += 20;


			/* Slow digestion takes less food */
			if (p_ptr->slow_digest)
				digestion -= 5;

			///mod140608 �푰���Ƃɖ���������������ƕ␳
			if(p_ptr->prace == RACE_SENNIN) digestion /= 3;
			if(p_ptr->prace == RACE_FAIRY) digestion /= 3;
			if(p_ptr->pclass == CLASS_HINA)	digestion /= 3;
			if(p_ptr->prace == RACE_ONI) digestion += 10;

			//v2.0.6 �ޖ������x������@���x���ɂ��ő�10�{
			if (p_ptr->pclass == CLASS_YUMA) digestion = digestion * (10 + p_ptr->lev) / 6;

			/* Minimal digestion */
			if (digestion < 1) digestion = 1;
			/* Maximal digestion */
			if (digestion > 1000) digestion = 1000;

			//�d��(������)
			if(CHECK_FAIRY_TYPE ==37 && !dun_level && turn % (TURNS_PER_TICK * TOWN_DAWN) < 50000)
			{
				set_food(MIN(p_ptr->food + digestion,PY_FOOD_MAX-1));
			}
			/* Digest some food */
			else 
				(void)set_food(p_ptr->food - digestion);
		}


		/* Getting Faint */
		if (p_ptr->food < PY_FOOD_FAINT)
		{
			/* Faint occasionally */
			//v2.0.6 �ޖ��͋󕠂Ŗ�Ⴢ��Ȃ�(process_player()�Ŗ\����ԂɂȂ�)
			if (!p_ptr->paralyzed && (randint0(100) < 10) && p_ptr->pclass != CLASS_YUMA)
			{
				/* Message */
#ifdef JP
				msg_print("���܂�ɂ��󕠂ŋC�₵�Ă��܂����B");
#else
				msg_print("You faint from the lack of food.");
#endif

				disturb(1, 1);

				/* Hack -- faint (bypass free action) */
				(void)set_paralyzed(p_ptr->paralyzed + 1 + randint0(5));
				if(one_in_(2)) set_deity_bias(DBIAS_REPUTATION, -1);
			}

			/* Starve to death (slowly) */
			if (p_ptr->food < PY_FOOD_STARVE)
			{
				/* Calculate damage */
				int dam = (PY_FOOD_STARVE - p_ptr->food) / 10;

				/* Take damage */
#ifdef JP
				//v2.0.6 �ޖ����Ƀ��[�h�̖��G���ł��󕠃_���[�W���󂯂�悤�ɂ����̖��G���������폜����
				//�܂��ޖ��ȊO�Ŗ��G�����̋󕠃_���[�W���C�ɂ���󋵂������Ȃ����낤
				//if (!IS_INVULN()) 
					take_hit(DAMAGE_LOSELIFE, dam, "��", -1);
#else
				if (!IS_INVULN()) take_hit(DAMAGE_LOSELIFE, dam, "starvation", -1);
#endif
			}
		}
	}


	//v1.1.86 �N�G�X�g�u���̍����U�v�̃_���W�������Ŋۑ����~���Ă��鏈��
	if (dun_level &&  p_ptr->inside_quest == QUEST_MORIYA_2)
	{
		int i;
		for (i = 0; i < 50; i++)
		{
			int x, y;
			x = randint1(220);
			y = 1;

			if (x > 190) x -= 30; //���X�g�X�p�[�g(X=160~190)�͊ۑ��̖��x��{

			//v2.0 �}�b�v�̍ŏ㕔���삯��������ۑ��ɓ�����Ȃ��̂̓N�G�X�g�̎�|�ɔ����邵���X�s���R�Ȃ̂Ń_���[�W���󂯂邱�Ƃɂ���
			if (player_bold(y, x))
			{
				msg_print("�����Ă����ۑ����]�V�ɒ��������I");
				disturb(1, 1);
				if (!IS_INVULN()) take_hit(DAMAGE_NOESCAPE, 50 + randint1(25), "�ۑ�", -1);
				set_stun(p_ptr->stun + randint1(50));
			}
			else if (in_bounds(y, x) && cave_empty_bold2(y, x))
			{
				summon_named_creature(0, y, x, MON_MARUTA, PM_NO_PET);
			}
		}
	}


	///mod140302 ��l(�ؐ�ӊO)�ƓV�l�����_�ɏP���鏈��
	//v1.1.42 EXTRA�N�G�X�g�t���A�ł͏o�Ȃ��悤�ɂ���
	if ((p_ptr->prace == RACE_SENNIN || p_ptr->prace == RACE_TENNIN) 
		&& (p_ptr->pclass != CLASS_KASEN) 
		&& p_ptr->lev > 19 
		&& !p_ptr->inside_arena 
		&& !(EXTRA_QUEST_FLOOR)
		&& !p_ptr->inside_battle 
		&& (!p_ptr->inside_quest || !is_fixed_quest_idx(p_ptr->inside_quest)))
	{
		int chance = 3000 - dun_level * 16;
		if( one_in_(chance))
		{
			//v1.2.00 ���x����dun_level����(plev+dun_level)/2�ɂ����B�w���t�@�C�AD���ŏI�Ղ܂ŏo�Ȃ��悤��
			if (summon_specific(0, py, px, (dun_level+p_ptr->lev)/2, SUMMON_DEATH,(PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				msg_format("�n������̎h�q�����ꂽ�I");
		}
	}


	if(p_ptr->special_defense & MUSOU_TENSEI)
	{
		if(p_ptr->csp < 50)
		{
			msg_print("�Ăїl�X�Ȃ��̂ɂƂ����悤�ɂȂ����B");
			p_ptr->special_defense &= ~(MUSOU_TENSEI);
			p_ptr->redraw |= (PR_MAP | PR_STATUS);
			p_ptr->update |= (PU_MONSTERS | PU_BONUS);
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
		else
		{
			p_ptr->csp -= 50;
			p_ptr->redraw |= PR_MANA;
		}
	}



	///mod140821 �G���W�j�A�̌̏ᒆ�̋@�B���C������鏈��
	engineer_malfunction(0, 1);

	/*:::�D���x�ɂ�鏈��*/
	process_world_aux_alcohol();

	/* Process timed damage and regeneration */
	process_world_aux_hp_and_sp();

	/* Process timeout */
	process_world_aux_timeout();

	/* Process light */
	process_world_aux_light();

	/* Process mutation effects */
	process_world_aux_mutation();

	/* Process curse effects */
	process_world_aux_curse();

	/* Process recharging */
	process_world_aux_recharge();

	/* Feel the inventory */
	sense_inventory1();
	sense_inventory2();

	/* Involuntary Movement */
	process_world_aux_movement();
}

///mod140225
/*:::b�R�}���h�ŕ\�����������I��*/
int choose_browse(void)
{
	int j, line;
	char temp[62*5];
	char com;

	screen_save();
	while(1)
	{
		Term_erase(12, 5, 255);
		Term_erase(12, 4, 255);
		Term_erase(12, 3, 255);
		Term_erase(12, 2, 255);
		Term_erase(12, 1, 255);

		put_str("a/u/U) �g�̓����E�ˑR�ψق̔���", 2, 16);
		put_str("b/m/M) �K���ς݂̖��@", 3, 16);
		put_str("c/j/J) �K���ς݂̐E�Ɠ��Z", 4, 16);

		prt("���ɂ��Ă̐�����ǂ݂܂����H",0,0);
		com = inkey();
		if (com == ESCAPE){ screen_load();return 0;}
		if (com=='a'||com=='u'||com=='U') {screen_load();return 1;}
		if (com=='b'||com=='m'||com=='M') {screen_load();return 2;}
		if (com=='c'||com=='j'||com=='J') {screen_load();return 3;}
	}
	
}



/*
 * Verify use of "wizard" mode
 */
static bool enter_wizard_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Wizard mode is not permitted */
		if (!allow_debug_opts || arg_wizard)
		{
#ifdef JP
			msg_print("�E�B�U�[�h���[�h�͋�����Ă��܂���B ");
#else
			msg_print("Wizard mode is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("�E�B�U�[�h���[�h�̓f�o�b�O�Ǝ����̂��߂̃��[�h�ł��B ");
		msg_print("��x�E�B�U�[�h���[�h�ɓ���ƃX�R�A�͋L�^����܂���B");
#else
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("�{���ɃE�B�U�[�h���[�h�ɓ��肽���̂ł���? "))
#else
		if (!get_check("Are you sure you want to enter wizard mode? "))
#endif
		{
			return (FALSE);
		}

		if (p_ptr->pseikaku == SEIKAKU_COLLECTOR)
		{
			msg_print("�E�B�U�[�h���[�h�ɓ���Ǝ��̏N�W�ƂɃG���g�����X�������p���Ȃ��Ȃ�܂��B");
			if(!get_check("***�{����***��낵���ł���? "))	return FALSE;
		}


#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�E�B�U�[�h���[�h�ɓ˓����ăX�R�A���c���Ȃ��Ȃ����B");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to enter wizard mode.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Debug mode is not permitted */
		if (!allow_debug_opts)
		{
#ifdef JP
			msg_print("�f�o�b�O�R�}���h�͋�����Ă��܂���B ");
#else
			msg_print("Use of debug command is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
		msg_print("�f�o�b�O�E�R�}���h�̓f�o�b�O�Ǝ����̂��߂̃R�}���h�ł��B ");
		msg_print("�f�o�b�O�E�R�}���h���g���ƃX�R�A�͋L�^����܂���B");
#else
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("�{���Ƀf�o�b�O�E�R�}���h���g���܂���? "))
#else
		if (!get_check("Are you sure you want to use debug commands? "))
#endif
		{
			return (FALSE);
		}

		if (p_ptr->pseikaku == SEIKAKU_COLLECTOR)
		{
			msg_print("�f�o�b�O�R�}���h���g���Ǝ��̏N�W�ƂɃG���g�����X�������p���Ȃ��Ȃ�܂��B");
			if(!get_check("***�{����***��낵���ł���? "))	return FALSE;
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�f�o�b�O���[�h�ɓ˓����ăX�R�A���c���Ȃ��Ȃ����B");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up sending score to use debug commands.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(void);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(void)
{
	/* Ask first time */
	if (!(p_ptr->noscore & 0x0010))
	{
		/* Mention effects */
#ifdef JP
		msg_print("�{�[�O�E�R�}���h�̓f�o�b�O�Ǝ����̂��߂̃R�}���h�ł��B ");
		msg_print("�{�[�O�E�R�}���h���g���ƃX�R�A�͋L�^����܂���B");
#else
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
		if (!get_check("�{���Ƀ{�[�O�E�R�}���h���g���܂���? "))
#else
		if (!get_check("Are you sure you want to use borg commands? "))
#endif
		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�{�[�O�E�R�}���h���g�p���ăX�R�A���c���Ȃ��Ȃ����B");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to use borg commands.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0010;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Ben Borg
 */
extern void do_cmd_borg(void);

#endif /* ALLOW_BORG */



/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 *
 * XXX XXX XXX Make some "blocks"
 */
static void process_command(void)
{
	int old_now_message = now_message;

#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	/*:::�R�}���h��'n'�̂Ƃ��A�O��̃R�}���h�������Ă���command_cmd�ɓ����B�����łȂ��Ƃ��R�}���h�����̃��s�[�g�p�ɓo�^����B*/
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	now_message = 0;

	/* Sniper */
	///class �X�i�C�p�[�W������
	//if ((p_ptr->pclass == CLASS_SNIPER) && (p_ptr->concent))
	///mod141116 �X�i�C�p�[�����ύX
	if ((CLASS_USE_CONCENT) && (p_ptr->concent))
		reset_concent = TRUE;

	break_eibon_flag = TRUE;

	/* Parse the command */
	switch (command_cmd)
	{
		/* Ignore */
		case ESCAPE:
		case ' ':
		{
			break;
		}

		/* Ignore return */
		case '\r':
		case '\n':
		{
			break;
		}

		/*** Wizard Commands ***/

		/* Toggle Wizard Mode */
		case KTRL('W'):
		{
			if (p_ptr->wizard)
			{
				p_ptr->wizard = FALSE;
#ifdef JP
msg_print("�E�B�U�[�h���[�h�����B");
#else
				msg_print("Wizard mode off.");
#endif

			}
			else if (enter_wizard_mode())
			{
				p_ptr->wizard = TRUE;
#ifdef JP
msg_print("�E�B�U�[�h���[�h�˓��B");
#else
				msg_print("Wizard mode on.");
#endif

			}

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			p_ptr->redraw |= (PR_TITLE);

			break;
		}


#ifdef ALLOW_WIZARD

		/* Special "debug" commands */
		case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode())
			{
				do_cmd_debug();
			}
			break;
		}

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

		/* Special "borg" commands */
		case KTRL('Z'):
		{
			/* Enter borg mode */
			if (enter_borg_mode())
			{
				if (!p_ptr->wild_mode) do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */



		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			if (!p_ptr->wild_mode) 
			{
				if(p_ptr->pclass == CLASS_3_FAIRIES)
					do_cmd_wield_3_fairies();
				else
					do_cmd_wield(FALSE);
			}
			break_eibon_flag = FALSE;
			break;
		}

		/* Take off equipment */
		case 't':
		{
			if (!p_ptr->wild_mode) do_cmd_takeoff();
			break_eibon_flag = FALSE;
			break;
		}

		/* Drop an item */
		case 'd':
		{
			if (!p_ptr->wild_mode) do_cmd_drop();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break_eibon_flag = FALSE;
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */

		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		/*:::�T�u�E�B���h�E�̑����ꗗ�ƃU�b�N�ꗗ��؂�ւ���*/
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		/* Alter a grid */
		/*:::�w������ɉ�������ėp�R�}���h*/
		case '+':
		{
			if (!p_ptr->wild_mode) do_cmd_alter();
			break;
		}

		/* Dig a tunnel */
		case 'T':
		{
			if (!p_ptr->wild_mode) do_cmd_tunnel();
			break;
		}

		/* Move (usually pick up things) */
		case ';':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}

		/* Move (usually do not pick up) */
		case '-':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(TRUE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(!always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}

		//v1.1.27 ���E�K�[�h
		case KTRL('g'):
		{
			if(set_mana_shield(TRUE,FALSE))
				energy_use = 100;
			break;
		}

		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
		case '.':
		{
			if (!p_ptr->wild_mode) do_cmd_run();
			break;
		}

		/* Stay still (usually pick things up) */
		case ',':
		{
			do_cmd_stay(always_pickup);
			break;
		}

		/* Stay still (usually do not pick up) */
		case 'g':
		{
			do_cmd_stay(!always_pickup);
			break;
		}

		/* Rest -- Arg is time */
		case 'R':
		{
			do_cmd_rest();
			break;
		}

		/* Search for traps/doors */
		case 's':
		{
			do_cmd_search();
			break;
		}

		/* Toggle search mode */
		case 'S':
		{
			if (p_ptr->action == ACTION_SEARCH) set_action(ACTION_NONE);
			else set_action(ACTION_SEARCH);
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
		case SPECIAL_KEY_STORE:
		{
			if (!p_ptr->wild_mode) do_cmd_store();
			break;
		}

		/* Enter building -KMW- */
		case SPECIAL_KEY_BUILDING:
		{
			if (!p_ptr->wild_mode) do_cmd_bldg();
			break;
		}

		/* Enter quest level -KMW- */
		case SPECIAL_KEY_QUEST:
		{
			if (!p_ptr->wild_mode) do_cmd_quest();
			break;
		}

		/* Go up staircase */
		case '<':
		{


			if (!p_ptr->wild_mode && !dun_level && !p_ptr->inside_arena && !p_ptr->inside_quest)
			{
				if (vanilla_town) break;

				if (ambush_flag && !SAKUYA_WORLD)
				{
#ifdef JP
					msg_print("�P�����瓦����ɂ̓}�b�v�̒[�܂ňړ����Ȃ���΂Ȃ�Ȃ��B");
#else
					msg_print("To flee the ambush you have to reach the edge of the map.");
#endif
					break;
				}

				if (p_ptr->food < PY_FOOD_WEAK)
				{
#ifdef JP
					msg_print("���̑O�ɐH�����Ƃ�Ȃ��ƁB");
#else
					msg_print("You must eat something here.");
#endif
					break;
				}

				change_wild_mode();
			}
			else
				do_cmd_go_up();
			break;
		}

		/* Go down staircase */
		case '>':
		{
			if(cave_have_flag_bold(py,px,FF_PORTAL))
				pass_through_portal();
			else if (p_ptr->wild_mode)
				change_wild_mode();
			else
				do_cmd_go_down();

			break;
		}

		/* Open a door or chest */
		case 'o':
		{
			if (!p_ptr->wild_mode) do_cmd_open();
			break;
		}

		/* Close a door */
		case 'c':
		{
			if (!p_ptr->wild_mode) do_cmd_close();
			break;
		}

		/* Jam a door with spikes */
		case 'j':
		{
			//����łR�}���h�폜
			//if (!p_ptr->wild_mode) do_cmd_spike();
			//�E�Ɣ\�͐��������Ă����B������b�R�}���h�ɓ����\��B
			///mod140906 ���[�O���C�N�R�}���h�̂Ƃ��Z���s�ɂ���悤�ɕύX
			if(rogue_like_commands)
			{
				 if(!p_ptr->wild_mode || p_ptr->pclass == CLASS_YUKARI) do_cmd_new_class_power(FALSE);
			}
			else do_cmd_new_class_power(TRUE);

			break;
		}
		case 'J':
		{
			///mod140202 �E�Ƃ��Ɠ���\��
			if (!(p_ptr->wild_mode && p_ptr->pclass != CLASS_YUKARI)) do_cmd_new_class_power(FALSE);
			break;
		}

		/* Bash a door */
		case 'B':
		{
			if (!p_ptr->wild_mode) do_cmd_bash();
			break;
		}

		/* Disarm a trap or chest */
		case 'D':
		{
			if (!p_ptr->wild_mode) do_cmd_disarm();
			break;
		}


		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
		case 'G':
		{
			///class realm
			///sys �����w�K�@������肩�����y�i���e�B�ɂ��邩�H
			//if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
			if (REALM_SPELLMASTER)
				msg_print("���Ȃ��͎������w�K����K�v�͂Ȃ��B");
			else if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
				do_cmd_gain_hissatsu();
			else if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
				gain_magic();
			else
				do_cmd_study();

			break_eibon_flag = FALSE;

			break;
		}

		/* Browse a book */
		case 'b':
		{
			int choose;
			if (!repeat_pull(&choose))
			{
				choose = choose_browse();
				if(choose) repeat_push(choose);
			}
			switch(choose)
			{
			case 0:
				break;
			case 1:
				do_cmd_racial_power(TRUE);
				break;
			case 2:
				if(p_ptr->pclass == CLASS_MINDCRAFTER || p_ptr->pclass == CLASS_SUMIREKO) do_cmd_mind_browse();
				else do_cmd_browse();
				break;
			case 3:
				do_cmd_new_class_power(TRUE);
				break;
			}
			break;

/*
			if ( (p_ptr->pclass == CLASS_MINDCRAFTER) ||
			     (p_ptr->pclass == CLASS_BERSERKER) ||
			     (p_ptr->pclass == CLASS_NINJA) ||
			     (p_ptr->pclass == CLASS_MIRROR_MASTER) 
			     ) do_cmd_mind_browse();
			else if (p_ptr->pclass == CLASS_SMITH)
				msg_print("�b��ꎞ��������");
				//do_cmd_kaji(TRUE);
			else if (p_ptr->pclass == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(TRUE, FALSE);
			else if (p_ptr->pclass == CLASS_SNIPER)
				do_cmd_snipe_browse();
			else do_cmd_browse();
			break;
*/
		}

		/* Cast a spell */
		///class m�R�}���h
		case 'm':
		{
			/* -KMW- */
			if (!p_ptr->wild_mode)
			{
				//if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_ARCHER) || (p_ptr->pclass == CLASS_CAVALRY))
				if(cp_ptr->realm_aptitude[0] == 0 && p_ptr->pclass != CLASS_SAMURAI 
					&& p_ptr->pclass != CLASS_YOUMU && p_ptr->pclass != CLASS_MINDCRAFTER && p_ptr->pclass != CLASS_SUMIREKO && p_ptr->pclass != CLASS_SEIJA 
					&& p_ptr->pclass != CLASS_MAGIC_EATER )
				{
#ifdef JP
					msg_print("���Ȃ��͎������g���Ȃ��B");
#else
					msg_print("You cannot cast spells!");
#endif
				}
				else if (dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC)/* && (p_ptr->pclass != CLASS_SMITH)*/)
				{
#ifdef JP
					msg_print("�_���W���������@���z�������I");
#else
					msg_print("The dungeon absorbs all attempted magic!");
#endif
					msg_print(NULL);
				}
				else if (p_ptr->anti_magic && (p_ptr->pclass != CLASS_BERSERKER)/* && (p_ptr->pclass != CLASS_SMITH)*/)
				{
#ifdef JP

					cptr which_power = "���@";
#else
					cptr which_power = "magic";
#endif
					if (p_ptr->pclass == CLASS_MINDCRAFTER || p_ptr->pclass == CLASS_SUMIREKO)
#ifdef JP
						which_power = "���\��";
#else
						which_power = "psionic powers";
#endif
					else if (p_ptr->pclass == CLASS_IMITATOR)
#ifdef JP
						which_power = "���̂܂�";
#else
						which_power = "imitation";
#endif
					else if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
#ifdef JP
						which_power = "�K�E��";
#else
						which_power = "hissatsu";
#endif
					else if (p_ptr->pclass == CLASS_MIRROR_MASTER)
#ifdef JP
						which_power = "�����@";
#else
						which_power = "mirror magic";
#endif
/*
					else if (p_ptr->pclass == CLASS_NINJA)
#ifdef JP
						which_power = "�E�p";
#else
						which_power = "ninjutsu";
#endif
*/
/*
					else if (mp_ptr->spell_book == TV_LIFE_BOOK)
#ifdef JP
						which_power = "�F��";
#else
						which_power = "prayer";
#endif
*/

#ifdef JP
					msg_format("�����@�o���A��%s���ז������I", which_power);
#else
					msg_format("An anti-magic shell disrupts your %s!", which_power);
#endif
					energy_use = 0;
				}

				else if (p_ptr->shero && p_ptr->pseikaku != SEIKAKU_BERSERK )
				{
#ifdef JP
					msg_format("����m�����Ă��ē������Ȃ��I");
#else
					msg_format("You cannot think directly!");
#endif
					energy_use = 0;
				}
				else
				{
					if ((p_ptr->pclass == CLASS_MINDCRAFTER) ||
					    (p_ptr->pclass == CLASS_SUMIREKO) ||
					   // (p_ptr->pclass == CLASS_BERSERKER) ||
					   // (p_ptr->pclass == CLASS_NINJA) ||
					    (p_ptr->pclass == CLASS_MIRROR_MASTER)
					    )
						do_cmd_mind();
					///sysdel ���̂܂ˁA���A�b��A�X�i�C�p�[�͂��΂炭���������Ă�����
					else if (p_ptr->pclass == CLASS_IMITATOR)
						do_cmd_mane(FALSE);
					else if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
						do_cmd_magic_eater(FALSE, FALSE);
					else if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
						do_cmd_hissatsu();
					else if (p_ptr->pclass == CLASS_BLUE_MAGE)
						do_cmd_cast_learned();
					//else if (p_ptr->pclass == CLASS_SMITH)
					//do_cmd_kaji(FALSE);
					else if (p_ptr->pclass == CLASS_SNIPER)
						do_cmd_snipe();
					else
						do_cmd_cast();
				}
			}
			break;
		}

		/* Issue a pet command */
		case 'p':
		{
			if (!p_ptr->wild_mode) do_cmd_pet();
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
		/*:::�A�C�e���ɖ���t����*/
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		/*:::�A�C�e���̖�������*/
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}

		/* Activate an artifact */
		case 'A':
		{
			if (!p_ptr->wild_mode)
			{
				do_cmd_activate();
				//v1.1.51 �A���[�i���ł�����ނ��g���邱�Ƃɂ���
#if 0
			if (!p_ptr->inside_arena)
				do_cmd_activate();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Eat some food */
		case 'E':
		{
			do_cmd_eat_food();
			break;
		}

		/* Fuel your lantern/torch */
		case 'F':
		{
			do_cmd_refill();
			break_eibon_flag = FALSE;
			break;
		}

		/* Fire an item */
		case 'f':
		{
			if (!p_ptr->wild_mode) (void)do_cmd_fire();
			break;
		}

		/* Throw an item */
		case 'v':
		{
			if (!p_ptr->wild_mode)
			{
				do_cmd_throw();
			}
			break;
		}

		/* Aim a wand */
		case 'a':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 �A���[�i���ł�����ނ��g���邱�Ƃɂ���
				do_cmd_aim_wand();
#if 0
				if (!p_ptr->inside_arena)
				do_cmd_aim_wand();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Zap a rod */
		case 'z':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 �A���[�i���ł�����ނ��g���邱�Ƃɂ���
				if (use_command && rogue_like_commands)
				{
					do_cmd_use();
				}
				else
				{
					do_cmd_zap_rod();
				}


#if 0
			if (p_ptr->inside_arena)
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && rogue_like_commands)
			{
				do_cmd_use();
			}
			else
			{
				do_cmd_zap_rod();
			}
#endif
			}
			break;
		}

		/* Quaff a potion */
		case 'q':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 �A���[�i���ł�����ނ��g���邱�Ƃɂ���
				do_cmd_quaff_potion();
#if 0
			if (!p_ptr->inside_arena)
				do_cmd_quaff_potion();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Read a scroll */
		case 'r':
		{
			if (!p_ptr->wild_mode)
			{
				//v1.1.51 �A���[�i���ł�����ނ��g���邱�Ƃɂ���
				do_cmd_read_scroll();
#if 0
				if (!p_ptr->inside_arena)
				do_cmd_read_scroll();
			else
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
#endif
			}
			break;
		}

		/* Use a staff */
		case 'u':
		{
			if (!p_ptr->wild_mode)
			{
			//v1.1.51 �A���[�i���ł�����ނ��g���邱�Ƃɂ���
			//if (p_ptr->inside_arena)
			if(FALSE)
			{
#ifdef JP
msg_print("�A���[�i�����@���z�������I");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && !rogue_like_commands)
			{
				do_cmd_use();
			}
			else
				do_cmd_use_staff();
			}
			break;
		}

		/* Use racial power */
		case 'U':
		{
			if (!p_ptr->wild_mode) do_cmd_racial_power(FALSE);
			break;
		}


		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
		case 'M':
		{
			do_cmd_view_map();
			break;
		}

		/* Locate player on map */
		case 'L':
		{
			do_cmd_locate();
			break;
		}

		/* Look around */
		case 'l':
		{
			do_cmd_look();
			break;
		}

		/* Target monster or location */
		case '*':
		{
			if (!p_ptr->wild_mode) do_cmd_target();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		case '$':
		{
			do_cmd_reload_autopick();
			break;
		}
		case '_':
		{
			do_cmd_edit_autopick();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			do_cmd_redraw();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			do_cmd_redraw();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			(void)combine_and_reorder_home(STORE_HOME);
			do_cmd_redraw();
			break;
		}

		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			if (!p_ptr->wild_mode) do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages(old_now_message);
			break;
		}

		/* Show quest status -KMW- */
		case KTRL('Q'):
		{
			do_cmd_checkquest();
			break;
		}

		/* Redraw the screen */
		case KTRL('R'):
		{
			now_message = old_now_message;
			do_cmd_redraw();
			break;
		}

#ifndef VERIFY_SAVEFILE

		/* Hack -- Save and don't quit */
		case KTRL('S'):
		{
			do_cmd_save_game(FALSE);
			break;
		}

#endif /* VERIFY_SAVEFILE */

		case KTRL('T'):
		{
			do_cmd_time();
			break;
		}

		/* Save and quit */
		case KTRL('X'):
		case SPECIAL_KEY_QUIT:
		{
			do_cmd_save_and_exit();
			break;
		}

		/* Quit (commit suicide) */
		case 'Q':
		{
			do_cmd_suicide();
			break;
		}

		case '|':
		{
			do_cmd_nikki();
			break;
		}

		/* Check artifacts, uniques, objects */
		case '~':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Record/stop "Movie" */
		///sysdel131221 ���[�r�[����
		///mod150224 ���[�r�[�����H
		case ']':
		{
			prepare_movie_hooks();
			break;
		}

		/* Make random artifact list */
		case KTRL('V'):
		{
			spoil_random_artifact("randifact.txt");
			break;
		}

#ifdef TRAVEL
		case '`':
		{
			if (!p_ptr->wild_mode) do_cmd_travel();
			if (p_ptr->special_defense & KATA_MUSOU)
			{
				set_action(ACTION_NONE);
			}
			break;
		}
#endif

		/* Hack -- Unknown command */
		default:
		{
			if (flush_failure) flush();
			if (one_in_(2))
			{
				char error_m[1024];
				sound(SOUND_ILLEGAL);
#ifdef JP
				if (!get_rnd_line("error_j.txt", 0, error_m))
#else
				if (!get_rnd_line("error.txt", 0, error_m))
#endif

					msg_print(error_m);
			}
			else
#ifdef JP
prt(" '?' �Ńw���v���\������܂��B", 0, 0);
#else
				prt("Type '?' for help.", 0, 0);
#endif

			break;
		}
	}
	if (!energy_use && !now_message)
		now_message = old_now_message;
}



/*:::�ނ�R�}���h�Œނ�郂���X�^�[����@������Jjlw�̔񃆃j�[�N�ł��邱��*/
///sys �ނ�@���j�[�N���ނ��悤�ɂ���ɂ͂����ݒ�v�@���������ł͑���Ȃ���
static bool monster_tsuri(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if ((r_ptr->flags7 & RF7_AQUATIC) && !(r_ptr->flags1 & RF1_UNIQUE) && my_strchr("Jjlw", r_ptr->d_char))
		return TRUE;
	else
		return FALSE;
}


/* Hack -- Pack Overflow */
/*:::�A�C�e�����ӂꏈ���Binventory[23]���g��*/
static void pack_overflow(void)
{
	if (inventory[INVEN_PACK].k_idx)
	{
		char o_name[MAX_NLEN];
		object_type *o_ptr;

		/* Is auto-destroy done? */
		notice_stuff();
		if (!inventory[INVEN_PACK].k_idx) return;

		/* Access the slot to be dropped */
		o_ptr = &inventory[INVEN_PACK];

		/* Disturbing */
		disturb(0, 1);

		/* Warning */
#ifdef JP
		msg_print("�U�b�N����A�C�e�������ӂꂽ�I");
#else
		msg_print("Your pack overflows!");
#endif

		/* Describe */
		object_desc(o_name, o_ptr, 0);

		/* Message */
#ifdef JP
		msg_format("%s(%c)�𗎂Ƃ����B", o_name, index_to_label(INVEN_PACK));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(INVEN_PACK));
#endif

		/* Drop it (carefully) near the player */
		/*:::�A�C�e���𗎂Ƃ��B���ł͂��Ȃ��B*/
		(void)drop_near(o_ptr, 0, py, px);

		/* Modify, Describe, Optimize */
		inven_item_increase(INVEN_PACK, -255);
		inven_item_describe(INVEN_PACK);
		inven_item_optimize(INVEN_PACK);

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();
	}
}

/*:::���̑��x�ŃG�l���M�[100���ɔ������鏈���@���̂Ƃ���̂Ǝ�p�̂�*/
/*:::���̍s���ɍ��킹��Ɖ̂��Ȃ���A�C�e�����Ƃ��ȂǂŃ^�[��������ŉ��x�����ʔ����ł��邩�珈���𕪂��Ă���̂��Ǝv����*/
/*
 * process the effects per 100 energy at player speed.
 */
static void process_upkeep_with_speed(void)
{

	int seiran_card_num = count_ability_card(ABL_CARD_SEIRAN);

	/* Give the player some energy */
	if (!load && p_ptr->enchant_energy_need > 0 && !p_ptr->leaving)
	{
		p_ptr->enchant_energy_need -= SPEED_TO_ENERGY(p_ptr->pspeed);
	}

	///mod151219 ��ჁA�ӎ��s���A������ԂɂȂ�Ɖ̂Ȃǂ���������悤�ɂ��Ă݂�
	///v1.1.12 �x�e���������ꂽ�̂ŋx�e�����͏��O
	if(p_ptr->action && p_ptr->action != ACTION_REST && (p_ptr->paralyzed || p_ptr->confused || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL)))
	{
		set_action(ACTION_NONE);
	}
	
	/* No turn yet */
	if (p_ptr->enchant_energy_need > 0) return;

	//v1.1.48 �ǉ�
	if (p_ptr->wild_mode) return;

	//v1.1.92 �ǉ�
	if (p_ptr->inside_battle) return;
	
	while (p_ptr->enchant_energy_need <= 0)
	{

		//v2.0.1 �A�r���e�B�J�[�h�u�����~�肽�e�v�̌���
		//target_who�̃����X�^�[�����E���ɂ���ƃ��P�b�g�ɂ��ǉ��U��
		if (seiran_card_num)
		{
			int prob = calc_ability_card_prob(ABL_CARD_SEIRAN, seiran_card_num);

			if (randint0(100) < prob && target_who > 0 && target_okay())
			{
				fire_rocket(GF_ROCKET, 5, p_ptr->lev * 2,1);

			}
		}

		///mod140329 ���[�_�[�Z���X�̏����������ɓ���Ă݂�
		if(!load && p_ptr->radar_sense)
		{
			int rad = p_ptr->lev / 2 + 5;

			map_area(rad);
		//	detect_all(rad);
			detect_traps(rad,TRUE);
			detect_objects_normal(rad);
			detect_objects_gold(rad);

		}
		///mod150116 ���т͐��n�`�����m����
		if(p_ptr->pclass == CLASS_MURASA && p_ptr->lev > 19
			|| CHECK_FAIRY_TYPE == 38)
		{
			murasa_detect_water = TRUE;
			map_area(15 + p_ptr->lev / 2);
			murasa_detect_water = FALSE;
		}
		//����c�l�����Ă��邼
		if(p_ptr->pclass == CLASS_KOISHI && !load && p_ptr->magic_num1[0] && one_in_(4))
		{
			int dam = p_ptr->lev + 30 + adj_general[p_ptr->stat_ind[A_CHR]] * 2;
			bool flag = FALSE;
			p_ptr->magic_num1[0] -= 1;
			msg_print("���͂̐l�e���������I");
			if(fire_random_target(GF_REDEYE, dam, 4, 0,0))flag = TRUE;
			if(fire_random_target(GF_REDEYE, dam, 4, 0,0))flag = TRUE;

			if(!flag) msg_print("�E�E�������G�����Ȃ������B");

		}
		//�C���y���V���u���V���[�e�B���O
		if(p_ptr->pclass == CLASS_MOKOU && !load && p_ptr->magic_num2[0])
		{
			int dam = p_ptr->lev * 5 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
			dam += randint1(dam);
			msg_print("�s�s�̒e���������ꂽ�I");
			project(0,5,py,px,dam,GF_MISSILE,PROJECT_KILL,-1);

			p_ptr->csp -= 30;
			if(p_ptr->csp < 1)
			{
				msg_print("�C�͂��s�����B");
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				p_ptr->magic_num2[0] = 0;
			}
			p_ptr->redraw |= (PR_MANA);
			redraw_stuff();

		}
		//v1.1.48 �X�[�p�[�n�R�_��Ԃ̎����@���E���򉻍U��
		if (SUPER_SHION && one_in_(6))
		{

			msg_print("���̃I�[�������͂�ۂݍ��񂾁I");
			project_hack2(GF_DISENCHANT,0,0, p_ptr->lev * 8);

		}

		//v1.1.76 �H�X�q���Z�u���s�����]���ρv
		if (p_ptr->pclass == CLASS_YUYUKO && p_ptr->special_defense & (KATA_MUSOU))
		{
			yuyuko_nehan();

		}


		//v1.1.92 ������p���i�@�_���[�W��H����������������T��
		//�H������_���[�W��magic_num1[6]�ɋL�^����Ă���
		if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && !p_ptr->inside_battle)
		{
			int plev = p_ptr->lev;
			//�o���L�������Ɣ�J���Ă���Ƃ�
			if (p_ptr->magic_num1[6] < 0)
			{
				p_ptr->magic_num1[6] += 1;
			}
			//�o���L����������ʂɂ΂�܂������J����
			else if (p_ptr->magic_num1[6] >= 30000)
			{
				int i;
				int dam = plev * 4;
				int rad = plev / 12 + 1;
				msg_print("�o���L�����I");
				disturb(0, 0);

				for (i = 1; i<10; i++)
				{
					int x, y;
					scatter(&y, &x, py, px, 4, 0);

					//�Ζ��n�`�����Ɛ��U��(�����Ȃ�)�̃����_������
					project(0, rad, y, x, dam, GF_DIG_OIL, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID, -1);
					project(0, rad, y, x, dam, GF_WATER, PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE, -1);
				}
				//���΂炭�l���}�C�i�X�ɂ���B���ꂪ0�ȏ�ɂȂ�܂ōU���������肵�Ă���Ȃ�(take_hit()�Q��)
				p_ptr->magic_num1[6] = -100 - randint1(100);

			}
			//����ȊO�̂Ƃ��A�m���ŐΖ����T���ď����J�E���g�����炷
			else if (p_ptr->magic_num1[6]>0)
			{
				int x, y;
				int rad = plev / 20;

				//���������������蒲���H�_���[�W�����܂��Ă���΂���قǔ������₷���A���܂薈�^�[���������Ȃ��悤��
				if (randint1(SHION_OIL_BOMB_LIMIT) < p_ptr->magic_num1[6] * 5)
				{
					scatter(&y, &x, py, px, 1, 0);
					msg_print("�o�̑̂���Ζ����H�藎�����B");
					project(0, rad, y, x, p_ptr->lev * 2, GF_DIG_OIL, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID, -1);

					p_ptr->magic_num1[6] -= 50;
					if (p_ptr->magic_num1[6] < 0) p_ptr->magic_num1[6] = 0;

				}

			}

		}

		/* Handle the player song */
		/*:::�p������̂�MP�v�Z����ʔ����Ȃ�*/
		if (!load) check_music();

		/* Hex - Handle the hex spells */
		if (!load) check_hex();
		if (!load) revenge_spell();
		
		/* There is some randomness of needed energy */
		p_ptr->enchant_energy_need += ENERGY_NEED();
	}
}


/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
static void process_player(void)
{
	int i;

	//v1.1.86 �O��s��������HP���ω��������ǂ����m�F���邽�߂̕ϐ��@�A�r���e�B�J�[�h�u�\�H�̃��J�f�v�Ɋ֌W
	static int prev_turn_hp = 0;

	//�Q�[���J�n���Ɉ�x�����s���鏈��������ꍇ�����ɋL�q�B
	//���̏����̌��ʂ̓N�C�b�N�X�^�[�g�ɂ͕ۑ�����Ȃ��B
	if (hack_startup)
	{
		//�G���W�j�A�M���h
		engineer_guild_price_change();

		//v1.1.21 ���m���ŏ��̃X�L���|�C���g�𓾂�
		if(p_ptr->pclass == CLASS_SOLDIER)
			get_soldier_skillpoint();

		//v1.1.52 俎q�V���i
		if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			okina_make_nameless_arts(0);
		}

		//v1.1.87 �A�r���e�B�J�[�h�����x�ƃJ�[�h�V���b�v�݌ɂ̐ݒ�
		set_abilitycard_price_rate();
		make_ability_card_store_list();

		//�푰��ǐ_�l �����_���Z�b�g
		if(p_ptr->prace == RACE_STRAYGOD)
		{
			p_ptr->race_multipur_val[3] = p_ptr->chaos_patron;
			p_ptr->race_multipur_val[DBIAS_COSMOS] = deity_table[p_ptr->race_multipur_val[3]].dbias_cosmos;
			p_ptr->race_multipur_val[DBIAS_WARLIKE] = deity_table[p_ptr->race_multipur_val[3]].dbias_warlike;
			p_ptr->race_multipur_val[DBIAS_REPUTATION] = deity_table[p_ptr->race_multipur_val[3]].dbias_reputation;
			p_ptr->redraw |= PR_MISC;
			redraw_stuff();
		}
		///mod160721 �P��J�n���ɂǂ̋��G���ǂ̐_��������Ă��邩���߂�
		if(p_ptr->pclass == CLASS_KAGUYA)
		{
			int tmp_list[KAGUYA_QUEST_MAX] = {MON_MORGOTH,MON_AZATHOTH,MON_ONE_RING,MON_UNICORN_ORD,MON_OMEGA};

			//�����X�^�[���X�g���V���b�t������B�����ƌ����悢�A���S���Y����������ł����肻���ȋC�����邪�B
			for(i=0;i<100;i++)
			{
				int n,m,tmp;

				n = randint0(KAGUYA_QUEST_MAX);
				m = randint0(KAGUYA_QUEST_MAX);

				if(n == m) continue;

				tmp = tmp_list[n];
				tmp_list[n] = tmp_list[m];
				tmp_list[m] = tmp;
			}
			//���X�g��ۑ�����Bkaguya_quest_art_list[]�̓Y�����ƈ�v
			for(i=0;i<KAGUYA_QUEST_MAX;i++)
			{
				p_ptr->magic_num1[i] = tmp_list[i];
			}
		}
		//v1.1.15
		//�w�J�[�e�B�A�J�n��
		if(p_ptr->pclass == CLASS_HECATIA)
		{
			//3�̑̂�HP/MP�̊����������ϐ���MAX�ɂ���(�����f�o�b�O���[�h�łȂ��Ɗ֌W�Ȃ���)
			for(i=0;i<6;i++) p_ptr->magic_num1[i] = 10000;
			//�n���ƌ��̑̂̐��i��ݒ肷��
			get_hecatia_seikaku();
			//�ǉ��C���x���g���̑O���ɓ����Ă���̂͂ǂ̑̂̑����i���t���O�@�Ƃ肠�����n��
			p_ptr->magic_num2[1] = HECATE_BODY_EARTH;

		}

		hack_startup = FALSE;
	}


	if(p_ptr->pclass == CLASS_MOKOU && flag_mokou_resurrection)
	{
		mokou_resurrection();
	}

	/*:::���Z��Ȃ�S�����X�^�[����ɔc������Ă����Ԃɂ���B���̂��Ɓ��͍s�����������̂܂܏I��*/
	if (p_ptr->inside_battle)
	{
		for(i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;

			/* Hack -- Detect monster */

			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);
		}
		prt_time();
	}
	else if (!(load && p_ptr->energy_need <= 0))
	{
		/* Give the player some energy */
		/*:::load��False�̂Ƃ��A��������energy_need��1�ȏ�̂Ƃ�ENERGY�𓾂�Bload�͉����낤�B�Q�[�����[�h����̂�true�H*/
		p_ptr->energy_need -= SPEED_TO_ENERGY(p_ptr->pspeed);
	}

	/* No turn yet */
	if (p_ptr->energy_need > 0) return;

	/*:::�R�}���h�����s�[�g���̂Ƃ��͎��Ԃ��X�V���Ȃ�*/
	if (!command_rep)
	{
			prt_time(); 
			///mod140322 �s���J�n���ɃX�R�A�\�����X�V����
			prt_score();

	}
	/*** Check for interupts ***/

	//v2.0.6 �ޖ��̋Q�샂�[�h
	if (p_ptr->pclass == CLASS_YUMA)
	{
		if (check_yuma_ultimate_mode()) return;
	}
	

	/*:::���S�x�e�����@-1��-2���Ɗ��S�񕜂܂ő�����炵��*/
	///sys ���������u�ő�l�𒴂���HP���񕜂���v��������������Ƃ��A�����C��t���Ȃ��ƃ��[�v����
	/* Complete resting */
	if (resting < 0)
	{
		/* Basic resting */
		if (resting == -1)
		{
			/* Stop resting */
			/*:::MP��������>=�Ȃ̂͌��p�Ƃ̂��߂̋L�q�Ǝv����*/

			if ((p_ptr->chp == p_ptr->mhp) && (p_ptr->csp >= p_ptr->msp))
			{
				set_action(ACTION_NONE);
			}
		}

		/* Complete resting */
		else if (resting == -2)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp >= p_ptr->msp) &&
			    !p_ptr->blind && !p_ptr->confused &&
			    !p_ptr->poisoned && !p_ptr->afraid &&
			    !p_ptr->stun && !p_ptr->cut &&
			    !p_ptr->slow && !p_ptr->paralyzed &&
			    !p_ptr->image && !p_ptr->word_recall &&
				!REF_BROKEN_VAL && //v1.1.66
			    !p_ptr->alter_reality)
			{
				set_action(ACTION_NONE);
			}
		}
	}
	/*:::�ނ�*/
	if (p_ptr->action == ACTION_FISH)
	{
		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, 10);
		if (one_in_(1000))
		{
			int r_idx;
			bool success = FALSE;
			get_mon_num_prep(monster_tsuri,NULL);
			r_idx = get_mon_num(dun_level ? dun_level : wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level);
			msg_print(NULL);
			if (r_idx && one_in_(2))
			{
				int y, x;
				y = py+ddy[tsuri_dir];
				x = px+ddx[tsuri_dir];
				if (place_monster_aux(0, y, x, r_idx, PM_NO_KAGE))
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
					msg_format("%s���ނꂽ�I", m_name);
#else
					msg_format("You have a good catch!", m_name);
#endif
					success = TRUE;
				}
			}
			if (!success)
			{
#ifdef JP
				msg_print("�a�����H���Ă��܂����I�������`�I");
#else
				msg_print("Damn!  The fish stole your bait!");
#endif
			}
			disturb(0, 1);
		}
	}

	/* Handle "abort" */
	/*:::�u�A���R�}���h�̓L�[���͂Œ��f�v�I�v�V������ON�̂Ƃ�*/
	if (check_abort)
	{
		/* Check for "player abort" (semi-efficiently for resting) */
		if (running || travel.run || command_rep || (p_ptr->action == ACTION_REST) || (p_ptr->action == ACTION_FISH))
		{
			/* Do not wait */
			inkey_scan = TRUE;

			/* Check for a key */
			/*:::�����L�[���͂�����Β��f����*/
			if (inkey())
			{
				/* Flush input */
				flush();

				/* Disturb */
				disturb(0, 1);

				/* Hack -- Show a Message */
#ifdef JP
msg_print("���f���܂����B");
#else
				msg_print("Canceled.");
#endif

			}
		}
	}

	/*:::Mega Hack �����Ԕ؊��d���̂Ƃ��A�s���J�n���ɕ��g�����Ă���n�`�ƃA�C�e�����L������*/
	update_minion_sight();

	/*:::�y�b�g��Ԉُ�񕜁@�N�O�E��������|�͋Z�\���ւ��*/
	if (p_ptr->riding && !p_ptr->confused && !p_ptr->blind)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		if (MON_CSLEEP(m_ptr))
		{
			char m_name[80];

			/* Recover fully */
			(void)set_monster_csleep(p_ptr->riding, 0);

			/* Acquire the monster name */
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%^s���N�������B", m_name);
#else
			msg_format("You have waked %s up.", m_name);
#endif
		}
		///sys ��n�����X�^�[��ԉ񕜁@�R��Z�\����

		if (MON_STUNNED(m_ptr))
		{
			/* Hack -- Recover from stun */
			///mod131226 skill �Z�\�ƕ���Z�\�̓���
			//if (set_monster_stunned(p_ptr->riding,(randint0(r_ptr->level) < p_ptr->skill_exp[GINOU_RIDING]) ? 0 : (MON_STUNNED(m_ptr) - 1)))
			if (set_monster_stunned(p_ptr->riding,(randint0(r_ptr->level) < ref_skill_exp(SKILL_RIDING) || p_ptr->pclass == CLASS_MAI) ? 0 : (MON_STUNNED(m_ptr) - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^s��N�O��Ԃ��痧�����点���B", m_name);
#else
				msg_format("%^s is no longer stunned.", m_name);
#endif
			}
		}

		if (MON_CONFUSED(m_ptr))
		{
			/* Hack -- Recover from confusion */
			if (set_monster_confused(p_ptr->riding,
				(randint0(r_ptr->level) < ref_skill_exp(SKILL_RIDING)) ? 0 : (MON_CONFUSED(m_ptr) - 1)))
///mod131226 skill �Z�\�ƕ���Z�\�̓���
//				(randint0(r_ptr->level) < p_ptr->skill_exp[GINOU_RIDING]) ? 0 : (MON_CONFUSED(m_ptr) - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^s��������Ԃ��痧�����点���B", m_name);
#else
				msg_format("%^s is no longer confused.", m_name);
#endif
			}
		}

		if (MON_MONFEAR(m_ptr))
		{
			/* Hack -- Recover from fear */
			///mod131226 skill �Z�\�ƕ���Z�\�̓���
			if (set_monster_monfear(p_ptr->riding,
				(randint0(r_ptr->level) < ref_skill_exp(SKILL_RIDING)) ? 0 : (MON_MONFEAR(m_ptr) - 1)))
			{
				char m_name[80];

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
				msg_format("%^s�����|���痧�����点���B", m_name);
#else
				msg_format("%^s is no longer fear.", m_name);
#endif
			}
		}

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();
	}
	
	load = FALSE;

	/* Fast */
	/*:::�B�C�p�t�̃t���O����*/
	///class ���C�p�m�̌����ړ��@�L�����Ԃ͍s���񐔂ŃJ�E���g
	///mod140813 �ʏ�̈ꎞ�����Ɠ����ꏊ�Ɉړ�
/*
	if (p_ptr->lightspeed)
	{
		(void)set_lightspeed(p_ptr->lightspeed - 1, TRUE);
	}
*/
	/*:::�B�C�p�t��magic_num1[0]�͗��܂����C�̗ʂ炵��*/
	///class ���C�p�@�C�̃^�[��������40
	if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
	{
		if (p_ptr->magic_num1[0] < 40)
		{
			p_ptr->magic_num1[0] = 0;
		}
		else p_ptr->magic_num1[0] -= 40;
		p_ptr->update |= (PU_BONUS);
	}
	/*:::���̊w�K��MP��������*/
	///class ��
	if (p_ptr->action == ACTION_LEARN)
	{
		s32b cost = 0L;
		u32b cost_frac = (p_ptr->msp + 30L) * 256L;

		/* Convert the unit (1/2^16) to (1/2^32) */
		s64b_LSHIFT(cost, cost_frac, 16);

 
		if (s64b_cmp(p_ptr->csp, p_ptr->csp_frac, cost, cost_frac) < 0)
		{
			/* Mana run out */
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
			set_action(ACTION_NONE);
		}
		else
		{
			/* Reduce mana */
			s64b_sub(&(p_ptr->csp), &(p_ptr->csp_frac), cost, cost_frac);
		}
		p_ptr->redraw |= PR_MANA;
	}

	/*:::���z�̌^��MP����*/
	///class ���p
	if (p_ptr->special_defense & KATA_MASK)
	{
		if (p_ptr->special_defense & KATA_MUSOU)
		{
			//v1.1.76 �H�X�q�ŏI���`�����ύX
			if (p_ptr->pclass == CLASS_YUYUKO)
			{
				if (p_ptr->csp < 50)
				{
					set_action(ACTION_NONE);
				}
				else
				{
					p_ptr->csp -= 50;
					p_ptr->redraw |= (PR_MANA);
				}


			}
			else //�ʏ�̖��z�̌^
			{
				if (p_ptr->csp < 3)
				{
					set_action(ACTION_NONE);
				}
				else
				{
					p_ptr->csp -= 2;
					p_ptr->redraw |= (PR_MANA);
				}
			}



		}
	}



	//v1.1.27 ���E�K�[�h�V�X�e��
	count_damage_guard_break = 0;


	//v2.0.5 �ʂ����ϐg�\���ǂ����`�F�b�N���A�\�Ȃ�ϐg����
	//�ϐg�\�`�F�b�N�͂�������ɂ���B���炭�����X�^�[�����ł����Ƃ��̕ϐg�\�`�F�b�N���z�肵�Ȃ��Ƃ��ɌĂ΂�Ă���̂��N���b�V���o�O�̌���
	if (p_ptr->pclass == CLASS_NUE && flag_nue_check_undefined)
	{

		check_nue_undefined();

		flag_nue_check_undefined = FALSE;

	}



	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	/*:::�R�}���h��t�E���s���[�v�@�����G�l���M�[���g�p����s��������܂ŌJ��Ԃ�*/
	while (p_ptr->energy_need <= 0)
	{
		p_ptr->window |= PW_PLAYER;
		p_ptr->sutemi = FALSE;
		now_damaged = FALSE;

		//v1.1.95 �J�E���^�[��Ԃ̉����@�ߋ�u�H�߂͎��̔@���v�g�p���͉�������Ȃ�
		//p_ptr->counter�̓Z�[�u/���[�h����Ȃ��̂ł����Ŗ�D�I��TRUE�ɂ�����
		if (p_ptr->pclass == CLASS_IKU && p_ptr->tim_general[0] && PLAYER_CAN_USE_ARTS)
		{
			p_ptr->counter = TRUE;
			p_ptr->redraw |= PR_STATUS;
		}
		else
		{
			p_ptr->counter = FALSE;
			p_ptr->redraw |= PR_STATUS;
		}

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Place the cursor on the player */
		move_cursor_relative(py, px);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		/*:::�A�C�e�����ӂꏈ��*/
		pack_overflow();


		/* Hack -- cancel "lurking browse mode" */
		/*:::get_item()��browse���[�h�œ��������߂̕ϐ��炵���@�ڍוs��*/
		if (!command_new) command_see = FALSE;


		/* Assume free turn */
		energy_use = 0;

		///mod150822 �v���Y�����o�[�O�o���͈ӎ��s���̂Ƃ����܂ɏ���������
		if(CLASS_IS_PRISM_SISTERS
			&& (p_ptr->paralyzed || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
			&& one_in_(7))
		{
			prism_change_class(TRUE);
			p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);
			redraw_stuff();

		}

		if (!PLAYER_CAN_COMMAND && p_ptr->au > 1000)
		{
			int enma_card_num = count_ability_card(ABL_CARD_EIKI);

			if (randint1(3) <= enma_card_num)
			{
				int percen = calc_ability_card_prob(ABL_CARD_EIKI,enma_card_num);
				int payment = p_ptr->au * percen / 100;

				screen_save();
				for (i = 9; i<16; i++) Term_erase(17, i, 255);

				put_str("�������琺����������...", 10, 30);
				if(percen >= 100)
					put_str("�u�L����S�Ăł��B�v", 12, 35);
				else
					put_str(format("�u��%d�ł��B�v",payment), 12, 35);

				if (get_check_strict("�x�����܂����H", CHECK_OKAY_CANCEL))
				{
					p_ptr->au -= payment;
					set_paralyzed(0);
					set_stun(0);
					set_alcohol(0);
					if (p_ptr->food < PY_FOOD_WEAK) set_food(PY_FOOD_ALERT);
					set_confused(0);
					set_blind(0);
					set_image(0);
					if (p_ptr->chp < p_ptr->mhp / 2) p_ptr->chp = p_ptr->mhp / 2;

					msg_print("���Ȃ��͕s�ӂɖڊo�߂��B���ɍ��z���y��...");
				}
				screen_load();
				do_cmd_redraw();

			}
		}


		///sysdel tougi
		if (p_ptr->inside_battle)
		{
			/* Place the cursor on the player */
			move_cursor_relative(py, px);

			command_cmd = SPECIAL_KEY_BUILDING;

			/* Process the command */
			process_command();
		}

		/* Paralyzed or Knocked Out */
		else if (p_ptr->paralyzed || (p_ptr->stun >= 100))
		{
			/* Take a turn */
			energy_use = 100;
			if(p_ptr->pclass == CLASS_SUMIREKO && !EXTRA_MODE && !p_ptr->inside_arena)
			{
				flag_sumireko_wakeup = TRUE;
				p_ptr->leaving = TRUE;
			}
		}
		///mod140330 �A�����łȂ��҂��D���x15000��������C��
		//v1.1.19 15000
		else if (p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
		{
			/* Take a turn */
			energy_use = 100;
			if(p_ptr->pclass == CLASS_SUMIREKO && !EXTRA_MODE && !p_ptr->inside_arena)
			{
				flag_sumireko_wakeup = TRUE;
				p_ptr->leaving = TRUE;
			}
		}
		/* Resting */
		else if (p_ptr->action == ACTION_REST)
		{
			/* Timed rest */
			/*:::�S�񕜂łȂ��^�[���w��x�e�̂Ƃ�*/
			if (resting > 0)
			{
				/* Reduce rest count */
				resting--;

				if (!resting) set_action(ACTION_NONE);

				/* Redraw the state */
				p_ptr->redraw |= (PR_STATE);
			}

			/* Take a turn */
			energy_use = 100;
		}
		//�z�K�q�~����
		else if(p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
		{
			energy_use = 100;
		}

		/* Fishing */
		else if (p_ptr->action == ACTION_FISH)
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Running */
		else if (running)
		{
			/* Take a step */
			run_step(0);
		}

#ifdef TRAVEL
		/* Traveling */
		else if (travel.run)
		{
			/* Take a step */
			travel_step();
		}
#endif

		/* Repeated command */
		else if (command_rep)
		{
			/* Count this execution */
			command_rep--;

			/* Redraw the state */
			p_ptr->redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff();

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command();
		}

		/* Normal command */
		else
		{

			//v1.1.86 �O��s��������HP���ς���Ă���cauc_bonuses()���Ď��s����B
			//�A�r���e�B�J�[�h�u�\�H�̃��J�f�v�Ɋ֌W
			//v2.0.1 �ǉ��J�[�h�u�s���̔]�؁v��
			if (prev_turn_hp != p_ptr->chp)
			{
				prev_turn_hp = p_ptr->chp;
				if (count_ability_card(ABL_CARD_MUKADE) || count_ability_card(ABL_CARD_SAKI_2))
				{
					p_ptr->update |= PU_BONUS;
					update_stuff();

				}
			}

			/* Place the cursor on the player */
			move_cursor_relative(py, px);
			///mod140917 �R�}���h�Ҏ󎞂Ƀ����X�^�[���X�g�X�V
			//v1.1.66 �펞ON�ɂ���
			//if(full_mon_list)
			{
				p_ptr->window |= PW_MONSTER_LIST;
				window_stuff();
			}

			//v1.1.58 �t���A��BGM�X�V�`�F�b�N
			if (flag_update_floor_music)
			{
				select_floor_music();
				flag_update_floor_music = FALSE;
			}

			can_save = TRUE;
			/* Get a command (normal) */
			request_command(FALSE);
			can_save = FALSE;

			/* Process the command */
			process_command();
		}
		/*:::�R�}���h�I���A���s���[�v�I��*/

		/* Hack -- Pack Overflow */
		pack_overflow();

		/*** Clean up ***/

		/* Significant */
		/*:::�����G�l���M�[�������s���������ꍇ*/
		if (energy_use)
		{
			///mod150114 �ړ���Ԃ���ʍ��ɕ\�����邱�Ƃɂ����̂ŁA�����s�����邽�тɕ\�����X�V���邱�Ƃɂ���
			p_ptr->redraw |= (PR_STATE);

			/* Use some energy */
			if (world_player || energy_use > 400)
			{
				/* The Randomness is irrelevant */
				p_ptr->energy_need += energy_use * TURNS_PER_TICK / 10;
			}
			else
			{
				/* There is some randomness of needed energy */
				p_ptr->energy_need += (s16b)((s32b)energy_use * ENERGY_NEED() / 100L);
			}

			/* Hack -- constant hallucination */
			if (p_ptr->image) p_ptr->redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			/*:::���F�����X�^�[�̍ĕ`�揈���H*/
			if (shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Skip unseen monsters */
					if (!m_ptr->ml) continue;

					/* Access the monster race */
					r_ptr = &r_info[m_ptr->ap_r_idx];

					/* Skip non-multi-hued monsters */
					if (!(r_ptr->flags1 & (RF1_ATTR_MULTI | RF1_SHAPECHANGER)))
						continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
			}


			/* Handle monster detection */
			/*:::�ڍוs���B�u���ƂŃ����X�^�[�����X�V����v�t���O�H*/
			if (repair_monsters)
			{
				/* Reset the flag */
				repair_monsters = FALSE;

				/* Rotate detection flags */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					//�����Ȃǂŏo�Ă��������X�^�[�����@���g���悤�ɂȂ�B
					//(�g���b�v�Ȃǁ��̍s���ŏ������ꂽ�����X�^�[�͂��̍s���I�����_�ł����ɓ���̂Ŏ������@������)
					/* Nice monsters get mean */
					if (m_ptr->mflag & MFLAG_NICE)
					{
						/* Nice monsters get mean */
						m_ptr->mflag &= ~(MFLAG_NICE);
					}

					/* Handle memorized monsters */
					if (m_ptr->mflag2 & MFLAG2_MARK)
					{
						/* Maintain detection */
						if (m_ptr->mflag2 & MFLAG2_SHOW)
						{
							/* Forget flag */
							m_ptr->mflag2 &= ~(MFLAG2_SHOW);

							/* Still need repairs */
							repair_monsters = TRUE;
						}

						/* Remove detection */
						else
						{
							/* Forget flag */
							m_ptr->mflag2 &= ~(MFLAG2_MARK);

							/* Assume invisible */
							m_ptr->ml = FALSE;

							/* Update the monster */
							update_mon(i, FALSE);

							if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
							if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);

							/* Redraw regardless */
							lite_spot(m_ptr->fy, m_ptr->fx);
						}
					}
				}
			}
			/*:::���̂܂ˎt�̂��̂܂ˏ��X�V*/
			///class ���̂܂˓��ꏈ��
			if (p_ptr->pclass == CLASS_IMITATOR)
			{
				if (p_ptr->mane_num > (p_ptr->lev > 44 ? 3 : p_ptr->lev > 29 ? 2 : 1))
				{
					p_ptr->mane_num--;
					for (i = 0; i < p_ptr->mane_num; i++)
					{
						p_ptr->mane_spell[i] = p_ptr->mane_spell[i+1];
						p_ptr->mane_dam[i] = p_ptr->mane_dam[i+1];
					}
				}
				new_mane = FALSE;
				p_ptr->redraw |= (PR_IMITATION);
			}
			if (p_ptr->action == ACTION_LEARN)
			{
				new_mane = FALSE;
				p_ptr->redraw |= (PR_STATE);
			}

			/*:::���Ԓ�~����energy_need��-1000��菬�����Ȃ��Ă���*/
			if (world_player && (p_ptr->energy_need > - 1000))
			{
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

#ifdef JP
				msg_print("�u���͓��������c�v");
#else
				msg_print("You feel time flowing around you once more.");
#endif
				msg_print(NULL);
				world_player = FALSE;
				p_ptr->energy_need = ENERGY_NEED();
				//v1.1.58
				flag_update_floor_music = TRUE;
				/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
				handle_stuff();
			}

			//v1.1.21 �e�֌W�̈ꎞ�l�����Z�b�g
			hack_gun_fire_critical_flag = 0L;
			special_shooting_mode = 0;

		}
		/*:::�����G�l���M�[�������s���������ꍇ�̏�������*/

		///mod160303 EX���[�h�̌������œ���̍s�����Ƃ����������̕ϐ����ݒ肳��G����
		if(hack_ex_bldg_summon_idx || hack_ex_bldg_summon_type )
		{
			if(hack_ex_bldg_summon_type)
				summon_specific(0,py,px,dun_level,hack_ex_bldg_summon_type,hack_ex_bldg_summon_mode);
			else if(hack_ex_bldg_summon_idx)
				summon_named_creature(0,py,px,hack_ex_bldg_summon_idx,hack_ex_bldg_summon_mode);
			hack_ex_bldg_summon_idx = 0;
			hack_ex_bldg_summon_type = 0;
			hack_ex_bldg_summon_mode = 0L;
		}

		/* Hack -- notice death */
		if (!p_ptr->playing || p_ptr->is_dead)
		{
			world_player = FALSE;
			break;
		}

		//�G�C�{���̖��̎ԗ֖���������
		if(energy_use && break_eibon_flag) break_eibon_wheel();

		/* Sniper */
		if (energy_use && reset_concent) reset_concentration(TRUE);

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/// -Mega Hack- ���̐��E���ꏈ��
		//energy_need�����0�ɏ��������A�^�[��������ōs����������B���[�v����
		if (SAKUYA_WORLD)
		{

			set_paralyzed(0);

			if(p_ptr->action == ACTION_REST && p_ptr->food < PY_FOOD_WEAK)
			{
				msg_print("�������󂢂ē|�ꂻ�����B�x�e�ǂ���ł͂Ȃ��B");
				p_ptr->action = ACTION_NONE;
				p_ptr->energy_need = 0;
			}

			if(p_ptr->energy_need > 0)//�󕠓x���� ���������ʏ�s���Ɠ����悤�ȏ����ɂ���
			{
				int digestion = 10;
				//v1.1.19 �D���x�}�W�b�N�i���o�[�r��
				if(p_ptr->alcohol >= DRANK_4) set_alcohol(DRANK_4-1);
				if(p_ptr->confused && one_in_(7)) set_confused(0);
				if(p_ptr->shero && one_in_(10)) set_shero(0,TRUE);

				if (p_ptr->regenerate) digestion += 20;
				if (p_ptr->cursed & TRC_FAST_DIGEST) digestion += 30;
				if (p_ptr->slow_digest)	digestion -= 5;


				digestion = digestion * p_ptr->energy_need / 500;

				if(p_ptr->action == ACTION_REST) 
					digestion = digestion * (1 + 50*(p_ptr->mhp - p_ptr->chp)/p_ptr->mhp);

				(void)set_food(p_ptr->food - digestion);
				if (p_ptr->wild_mode && p_ptr->food < PY_FOOD_WEAK)
				{
					change_wild_mode();
				}
				if(p_ptr->food < PY_FOOD_STARVE) take_hit(DAMAGE_LOSELIFE, 1, "��", -1);
			}

			if(p_ptr->action == ACTION_REST)
			{
				msg_print("���Ȃ��͋x�e�����B");
				p_ptr->chp = p_ptr->mhp;
				p_ptr->chp_frac = 0L;
				set_stun(0);
				set_cut(0);
				set_image(0);
				set_blind(0);
				set_confused(0);
				set_shero(0,TRUE);
			    if (p_ptr->food >= PY_FOOD_MAX) p_ptr->food = PY_FOOD_MAX -1;
				p_ptr->redraw |= PR_HP | PR_EXTRA | PR_STATE | PR_STATUS;
				p_ptr->action = ACTION_NONE;
			}
			else if(p_ptr->action == ACTION_FISH)//paranoia
			{
				msg_print("���ނ�Ȃǂ��Ă��Ӗ����Ȃ��B");
				p_ptr->action = ACTION_NONE;
			}

			
			resting = 0;
			p_ptr->energy_need = 0;
			handle_stuff();

		}

	}
	/*:::�R�}���h��t���s���[�v����*/

	//v1.1.84 ���Ƃ�̃g���b�v�������m�Ɏg��
	p_ptr->update |= (PU_VIEW);

	//v1.1.86�@�A�r���e�B�J�[�h��HP/MP���ϓ����邱�ƂɂȂ����̂ōs���ς܂��邽�тɌv�Z����
	p_ptr->update |= (PU_HP | PU_MANA);


	/* Update scent trail */
	update_smell();

}


/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
/*:::���݊K�ł̏������s���A�K�ړ��E�Q�[�����f�E�Q�[���I�[�o�[�ȊO�͂����̃��[�v���J��Ԃ�*/
static void dungeon(bool load_game)
{
	int quest_num = 0;
	int i;

	/* Set the base level */
	base_level = dun_level;

	/* Reset various flags */
	hack_mind = FALSE;

	/* Not leaving */
	p_ptr->leaving = FALSE;

	/* Reset the "command" vars */
	command_cmd = 0;

#if 0 /* Don't reset here --- It's used for Arena */
	command_new = 0;
#endif

	command_rep = 0;
	command_arg = 0;
	command_dir = 0;


	/* Cancel the target */
	target_who = 0;
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;
	ambush_flag = FALSE;

	/* Cancel the health bar */
	/*:::�^�[�Q�b�g�̗̑̓Q�[�W�������H*/
	health_track(0);

	/* Check visual effects */
	shimmer_monsters = TRUE;
	shimmer_objects = TRUE;
	repair_monsters = TRUE;
	repair_objects = TRUE;


	/* Disturb */
	disturb(1, 1);

	/* Get index of current quest (if any) */
	/*:::�N�G�X�g���Ȃ�N�G�X�gID���A�����łȂ��Ȃ�0��Ԃ�*/
	quest_num = quest_number(dun_level);

	/* Inside a quest? */
	if (quest_num)
	{
		/* Mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 |= RF1_QUESTOR;
	}

	/* Track maximum player level */
	if (p_ptr->max_plv < p_ptr->lev)
	{
		p_ptr->max_plv = p_ptr->lev;
	}

	/* Track maximum dungeon level (if not in quest -KMW-) */
	if ((max_dlv[dungeon_type] < dun_level) && !p_ptr->inside_quest)
	{
		max_dlv[dungeon_type] = dun_level;
		if (record_maxdepth) do_cmd_write_nikki(NIKKI_MAXDEAPTH, dun_level, NULL);

		///mod160522 ���̎x�����������J�E���^ �t���A���Z�b�g�ŃX�J���ł��Ȃ��悤magic_num2�ɓƎ��ōŐ[���B�K�����Ǘ����Ă���
		if(p_ptr->pclass == CLASS_RINGO && max_dlv[dungeon_type] > p_ptr->magic_num2[dungeon_type])
		{
			int tmp = (max_dlv[dungeon_type] - p_ptr->magic_num2[dungeon_type]) * (dun_level+100);
			if(tmp > 1000) tmp = 1000;
			p_ptr->magic_num1[4] += tmp;
			p_ptr->magic_num2[dungeon_type] = max_dlv[dungeon_type];
		}

	}
	//v1.1.25 �_���W�������e�t���O
	if(!p_ptr->inside_quest && dun_level && dun_level == d_info[dungeon_type].maxdepth && !flag_dungeon_complete[dungeon_type])
	{
		if(dungeon_type != DUNGEON_CHAOS && 
			(!d_info[dungeon_type].final_guardian || !r_info[d_info[dungeon_type].final_guardian].max_num))
		{
			msg_format("���Ȃ���%s�𐧔e�����I",d_name+d_info[dungeon_type].name);
			flag_dungeon_complete[dungeon_type] = 1;
		}

	}

	/*:::�y�b�g�̃R�X�g�v�Z�炵�����߂�l���󂯎���Ă��Ȃ��B����"p_ptr->update |= (PU_BONUS)"�̏����̂��߂Ɏ��s���Ă���̂��H*/
	(void)calculate_upkeep();

	/* Validate the panel */
	/*:::���݉�ʂ̏��ڐ��𓾂�H*/
	panel_bounds_center();

	/* Verify the panel */
	/*:::�`��֌W�̃`�F�b�N�⏉�����H*/
	verify_panel();

	/* Flush messages */
	msg_print(NULL);

	/* Enter "xtra" mode */
	/*:::����������Ԃ̃t���O�𗧂Ă�H*/
	character_xtra = TRUE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_MONSTER | PW_OVERHEAD | PW_DUNGEON);

	/* Redraw dungeon */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE | PU_TORCH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS | PU_DISTANCE | PU_FLOW);

	/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
	/*:::�X�e�[�^�X�E�p�����[�^�E�}�b�v�̍Čv�Z�E�ĕ`�揈��*/
	handle_stuff();
	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Handle "p_ptr->notice" */
	notice_stuff();

	/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
	handle_stuff();

	/*::: -Giga Hack- �얲�E�������Ȃǉσp�����[�^�����X�^�[������Ƃ��p�����[�^���Čv�Z����*/
	/*:::�Q�[���J�n�����r_info�̏����l�Ȃ̂Œ��ڏ㏑������*/
	for(i=1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		if(!(r_info[m_ptr->r_idx].flags7 & RF7_VARIABLE)) continue;

		if (m_ptr->r_idx == MON_REIMU ) apply_mon_race_reimu();
		else if (m_ptr->r_idx == MON_MARISA ) apply_mon_race_marisa();
		else if (m_ptr->r_idx == MON_MASTER_KAGUYA ) apply_mon_race_master_kaguya();
		else if(IS_RANDOM_UNIQUE_IDX(m_ptr->r_idx)); //�����_�����j�[�N�̓��[�h���Ƀp�����[�^��ǂݍ��ނ̂ł����ł͉������Ȃ�
		else if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
		{
			apply_mon_race_tsukumoweapon(&o_list[m_ptr->hold_o_idx],m_ptr->r_idx);
			lore_do_probe(m_ptr->r_idx);

		}
		else if(m_ptr->r_idx == MON_BANKI_HEAD_1 || m_ptr->r_idx == MON_BANKI_HEAD_2)
			(void) apply_mon_race_banki_head(m_ptr->r_idx);
		else if(m_ptr->r_idx == MON_GREEN_EYED)
			(void) apply_mon_race_green_eyed();

		else msg_format("ERROR:�σp�����[�^�����X�^�[(r_idx:%d)�̃��[�h���p�����[�^�Đݒ肪����Ă��Ȃ�",m_ptr->r_idx);
	}


	//�ʂ���p�����@�t���A�ɓ���������̕ϐg����
	//v2.0.5 ���̔����process_player()�Ɉڂ��B�����ł̓t���O�������Ă�
	//if(p_ptr->pclass == CLASS_NUE)
	//{
	//	check_nue_undefined();
	//}
	flag_nue_check_undefined = TRUE;

	/*:::Mega Hack - Extra���[�h�_���W�����������̖��O��ł��邱�Ƃ�ݒ�@�Z�[�u�����[�h�ɑΉ����邽�߃t���A�ɍ~�藧��������ɌĂ�*/
	if(EXTRA_MODE && dun_level)
	{
		init_extra_dungeon_buildings();
	}

	/* Refresh */
	Term_fresh();

	/*:::�N�G�X�g���Ń����N�G�ł��I�x�����ł�J�ł��Ȃ����C���_���W�����N�G�X�g�ł��Ȃ��ꍇ�A���͋C���m����*/
	/*:::�N�G�X�g�_���W�����ɓ������Ƃ��u�T�^�I�ȃN�G�X�g�̃_���W�����̂悤���v�ƕ\�������邽�߂̏����炵�����A*/
	/*:::����ɂ��Ă͏���������₱��������C������B���������Ƃ��Ă���̂��B*/
	if (quest_num && (is_fixed_quest_idx(quest_num) &&
	  //  !((quest_num == QUEST_TAISAI) || (quest_num == QUEST_SERPENT) ||
	    !((quest_num == QUEST_TAISAI) ||(quest_num == QUEST_YUKARI) ||
	    !(quest[quest_num].flags & QUEST_FLAG_PRESET)))) do_cmd_feeling();

	if (p_ptr->inside_battle)
	{
		if (load_game)
		{
			p_ptr->energy_need = 0;
			//battle_monsters();
		}
		else
		{
			msg_print("�����J�n�I");
			msg_print(NULL);
		}
	}

	if(	flag_sumireko_wakeup)
	{
		flag_sumireko_wakeup = FALSE;
		msg_print("���Ȃ��͊O�E�Ŗڊo�߁A���΂��̌��������̂��ƍĂі���A���z���֍~�藧�����B");

	}

	//v1.1.43 �o���l�e�[�u����ς����̂Ń_���W�����̃��C�����[�v�O�Ɍo���l�`�F�b�N�����Ă���
	check_experience();

	/*::: ��V���l�̗�I�m�o�Ɋւ��鏈���炵��*/
	//��I�m�o���̂��n�߂Ă���̃^�[������magic_num1[0]�����Z���邱�ƂŊǗ����悤�Ƃ��Ă���ς��߂��H�H�H
	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT))
		p_ptr->magic_num1[0] = MUSIC_DETECT;

	/* Hack -- notice death or departure */
	/*:::���S���Q�[���I���ł������o��*/
	if (!p_ptr->playing || p_ptr->is_dead) return;

	/* Print quest message if appropriate */
	///sys �����N�G�ƃ_���W�����̎�̏���
	if (!p_ptr->inside_quest && (dungeon_type == DUNGEON_ANGBAND))
	{
		quest_discovery(random_quest_number(dun_level));
		p_ptr->inside_quest = random_quest_number(dun_level);
	}
	if ((dun_level == d_info[dungeon_type].maxdepth) && d_info[dungeon_type].final_guardian && (CHECK_GUARDIAN_CAN_POP))
	{
		if (r_info[d_info[dungeon_type].final_guardian].max_num)
#ifdef JP
			msg_format("���̊K�ɂ�%s�̎�ł���%s������ł���B",
				   d_name+d_info[dungeon_type].name, 
				   r_name+r_info[d_info[dungeon_type].final_guardian].name);
#else
			msg_format("%^s lives in this level as the keeper of %s.",
					   r_name+r_info[d_info[dungeon_type].final_guardian].name, 
					   d_name+d_info[dungeon_type].name);
#endif
	}

	/*:::Hack - EX�_���W�����̒��{�X*/
	if (dungeon_type == DUNGEON_CHAOS)
	{
		monster_type *m_ptr_tmp;
		char temp_name[80];
		if(dun_level == 110 && r_info[MON_MORGOTH].max_num)
		{
			msg_format("���̊K�ɂ�%s������ł���B",
				   r_name+r_info[MON_MORGOTH].name);
		}
		if(dun_level == 120 && r_info[MON_AZATHOTH].max_num)
		{
			msg_format("���̊K�ɂ�%s������ł���B",
				   r_name+r_info[MON_AZATHOTH].name);
		}
	
	}
	//v1.1.27 ���E�K�[�h
	if(dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		set_mana_shield(FALSE,FALSE);
	}

	/*:::�K�ɍ~��Ă�������͒��B���ł͂Ȃ��H*/
	///class �E�ғ��ꏈ��
	if (!load_game && (p_ptr->special_defense & NINJA_S_STEALTH)) set_superstealth(FALSE);

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = base_level;

	/* Reset the object generation level */
	object_level = base_level;

	hack_mind = TRUE;

	/*:::�t���A��������A�n��ւ̋A�Ғ���A�A���[�i���ꎞ�Ɂ����ŏ��ɍs���ł���悤�ɂ��邽�߂̏���*/
	if (p_ptr->energy_need > 0 && !p_ptr->inside_battle &&
	    (dun_level || p_ptr->leaving_dungeon || p_ptr->inside_arena))
		p_ptr->energy_need = 0;

	///mod151104 �u���̐��E�v�����ꏈ��
	if (SAKUYA_WORLD)
	{
		p_ptr->energy_need = 0;
	}

	/*
	//�o�O�ŃN�G�X�g���s��������l�̋~�Ϗ���
	if (difficulty == DIFFICULTY_NORMAL && p_ptr->prace == RACE_STRAYGOD && p_ptr->pclass == CLASS_MAID 
		&& (quest[QUEST_OOGAME].status == QUEST_STATUS_FAILED || quest[QUEST_OOGAME].status == QUEST_STATUS_FAILED_DONE))
	{
		complete_quest(QUEST_OOGAME);

	}
	*/



	/* Not leaving dungeon */
	p_ptr->leaving_dungeon = FALSE;

	/* Initialize monster process */
	/*mproc_list[][]/mproc_max[]�Ƀt���A�̃����X�^�[�̈ꎞ�X�e�[�^�X�����i�[*/
	mproc_init();

	/* Main loop */
	while (TRUE)
	{
		/* Hack -- Compact the monster list occasionally */
		/*:::�t���A�̃����X�^�[���ő�ɋ߂��Ȃ����Ƃ������X�^�[���������k*/
		if ((m_cnt + 32 > max_m_idx) && !p_ptr->inside_battle) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		/*:::���S�ς݃����X�^�[���������Ƃ����X�g�𐮗�*/
		if ((m_cnt + 32 < m_max) && !p_ptr->inside_battle) compact_monsters(0);


		/* Hack -- Compact the object list occasionally */
		/*:::�t���A�̃A�C�e���������E�߂��ɂȂ����Ƃ��������k*/
		if (o_cnt + 32 > max_o_idx) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		/*:::�t���A�̖����ȃA�C�e�����������烊�X�g�𐮗�*/
		if (o_cnt + 32 < o_max) compact_objects(0);

		
		/* Process the player */
		process_player();
		
		process_upkeep_with_speed();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Process all of the monsters */
		process_monsters();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;


		/* Process the world */
		process_world();

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Count game turns */
		turn++;

		/*:::wild_regen�͏����l20 dungeon_turn_limit��birth���Ɍ��߂���B*/
		if (dungeon_turn < dungeon_turn_limit)
		{
			if (!p_ptr->wild_mode || wild_regen) dungeon_turn++;
			/*:::�S�̃}�b�v�ł�dungeon_turn�͑����ɂ����炵���B*/
			else if (p_ptr->wild_mode)
			{
				//v1.1.59 �T�j�[�ɓ����~�ς̓��Z�������̂œ����~�ώ���dungeon_turn�������悤�ɂ���
				if (p_ptr->pclass == CLASS_SUNNY && p_ptr->magic_num1[0] < SUNNY_CHARGE_SUNLIGHT_MAX && is_daytime())
					dungeon_turn++;
				if(!(turn % ((MAX_HGT + MAX_WID) / 2))) dungeon_turn++;
			}
		}

		prevent_turn_overflow();

		if (wild_regen) wild_regen--;
	}/*:::���C�����[�v�I���@���͎��ʂ��K�ړ����邩�Q�[���I������*/

	/* Inside a quest and non-unique questor? */
	/*:::����񃆃j�[�N�r�ŃN�G�X�g�������ꍇ�����X�^�[����t���O���O��*/
	if (quest_num && !(r_info[quest[quest_num].r_idx].flags1 & RF1_UNIQUE))
	{
		/* Un-mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 &= ~RF1_QUESTOR;
	}

	//�Z�[�u�I���ł����S�ł��Ȃ��Ƃ��B�ʏ�̊K�ړ�������O�̏���
	/* Not save-and-quit and not dead? */
	if (p_ptr->playing && !p_ptr->is_dead)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;

		//�w�J�[�e�B�A���ꏈ��
		for(i=0;i<3;i++) hecatia_hp[i] = 5600;

		//Ex���[�h�p�������J�E���g�����Z�b�g
		for(i=0;i<EX_BUILDINGS_PARAM_MAX;i++) ex_buildings_param[i] = 0;

		//�Ԕ؊���Z�̃A�C�e���T���𒆒f
		if (p_ptr->tim_rokuro_head_search)
		{
			rokuro_head_search_item(0, TRUE);
		}

		//���q�̋L�����Ă��閂�@���N���A�B�A�C�e���J�[�h����g����悤�Ɨ��ϐ��ɂ���
		kyouko_echo(TRUE,0);
		//�p���X�B�̃_���[�W�J�E���g���N���A
		parsee_damage_count = 0L;
		//�����X�^�[�u���ڂ��܁v�̃^�[�Q�b�g
		hassyakusama_target_idx = 0;

		//v1.1.58
		flag_update_floor_music = TRUE;

		//v1.1.87 EXTRA���[�h�̂Ƃ��A�t���A���ړ����邽�тɃA�r���e�B�J�[�h�̔̔����X�g�A�����񐔁A�����x���X�V
		if (EXTRA_MODE)
		{
			ability_card_trade_count = 0;
			set_abilitycard_price_rate();
			make_ability_card_store_list();

		}

		//v2.0.5 EXTRA�ł͂��Ẵ����X�^�[�{�������Ă���ꍇ���̃t���A��ʉ߂����烊�Z�b�g
		if (EXTRA_MODE && dun_level >= p_ptr->hatate_mon_search_dungeon % 1000)
		{
			if (p_ptr->wizard) msg_print("hatate reset");
			p_ptr->hatate_mon_search_dungeon = 0;
			p_ptr->hatate_mon_search_ridx = 0;
		}

		//v1.1.13 ���]������㩂̉���
		if(p_ptr->special_flags & SPF_ORBITAL_TRAP)
		{
			msg_print("���]������㩂����͂��������B");
			p_ptr->special_flags &= ~(SPF_ORBITAL_TRAP);
		}
		//v1.1.35 �l���m�꒣��N���A
		if(p_ptr->pclass == CLASS_NEMUNO)
		{
			erase_nemuno_sanctuary(FALSE,FALSE);
		}

		//v1.1.93 �~�P�́u���̃t���A�ł��łɋq���Ă񂾁v�t���O���N���A
		if (p_ptr->pclass == CLASS_MIKE) p_ptr->magic_num2[100] = 0;

		///mod140423
		if(p_ptr->silent_floor)
		{
			p_ptr->silent_floor=0;
			msg_print("�_���W��������َ��Ȗ��͂������������B");
		}
		///mod140502
		if(p_ptr->lucky)
		{
				set_tim_lucky(0,TRUE);
		}
		///mod150304 �����n���Z���N���A
		//v1.1.95 �ړ��֎~�n���Z��MTIMED2_NO_MOVE�ɓ��������̂ŏ����폜
//		if(p_ptr->pclass == CLASS_KOMACHI || p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_REMY || p_ptr->pclass == CLASS_MAMIZOU)

		//�}�~�]�E���ϐg�����Ă��郂���X�^�[�̉���
		if(p_ptr->pclass == CLASS_MAMIZOU)
		{
			if(p_ptr->magic_num1[0]) set_tim_general(0,TRUE,0,0);
		}
		//v1.1.17 ���ϋZ���Z�b�g�ƈꎞ�\�͉���
		if(p_ptr->pclass == CLASS_JUNKO)
		{
			junko_make_nameless_arts();
		}
		///mod141101 ���Ƃ�̖��@���X�g���N���A
		if(p_ptr->pclass == CLASS_SATORI)
		{
			make_magic_list_satori(TRUE);
		}
		//v1.1.95 �p���X�B�̘m�l�`�J�E���g���N���A
		if (p_ptr->pclass == CLASS_PARSEE)
		{
			p_ptr->magic_num1[4] = 0;
			p_ptr->magic_num2[4] = 0;
		}

		//v1.1.77 ����(��p���i)�̒ǐՑΏۃ����X�^�[���N���A
		if (is_special_seikaku(SEIKAKU_SPECIAL_ORIN) && p_ptr->magic_num1[0])
		{
			msg_print("�ǐՂ𒆒f�����B");
			p_ptr->magic_num1[0] = 0;
		}

		//���g�C���y���V���u���V���[�e�B���O������
		else if(p_ptr->pclass == CLASS_MOKOU && p_ptr->magic_num2[0])
		{
			p_ptr->magic_num2[0] = 0;
		}
		//���i�`���\�͉���
		else if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] || p_ptr->tim_general[1]))
		{
			set_tim_general(0,TRUE,0,0);
			set_tim_general(0,TRUE,1,0);
		}
		else if(p_ptr->pclass == CLASS_SANAE && p_ptr->magic_num1[1])
		{
			msg_print("�t���A����_�̋C�z���������B");
			p_ptr->magic_num1[1] = 0;
		}
		else if(p_ptr->pclass == CLASS_KOISHI && p_ptr->magic_num1[0])
		{
			msg_print("���͂̐l�e���������B");
			p_ptr->magic_num1[0] = 0;
		}
		///mod150516 �V�q�̗v�΂��N���A
		else if(p_ptr->pclass == CLASS_TENSHI)
		{
			p_ptr->magic_num1[0] = 0;
			p_ptr->magic_num1[1] = 0;
			p_ptr->magic_num1[2] = 0;
	
		}
		//�h���~�[�ϐg����
		else if(p_ptr->pclass == CLASS_DOREMY && IS_METAMORPHOSIS)
		{
			p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
			p_ptr->special_flags &= ~(SPF_IGNORE_METAMOR_TIME);
			set_mimic(0,0,TRUE);
		}
		//v1.1.48 �����߈ˉ���..�̓t���A���o�Ă�����Ȃ����Ƃɂ����B�����ł����Ȃ��Ǝシ����B
		/*
		else if (SHION_POSSESSING)
		{
			p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
			p_ptr->special_flags &= ~(SPF_IGNORE_METAMOR_TIME);
			set_mimic(0, 0, TRUE);
		}
		*/
		//���̎􂢒��f
		else if (p_ptr->pclass == CLASS_HINA)
		{
			if(p_ptr->magic_num1[0]) set_tim_general(0,TRUE,0,0);
			if(p_ptr->special_defense & SD_HINA_NINGYOU)
			{
				msg_print("����T���U�炷�̂��~�߂��B");
				p_ptr->special_defense &= ~(SD_HINA_NINGYOU);
				p_ptr->redraw |= (PR_STATUS);
			}
		}
		else if (p_ptr->pclass == CLASS_YACHIE)
		{
			if (p_ptr->magic_num1[0])
			{
				msg_print("�����H����~�߂��B");
				p_ptr->magic_num1[0] = 0;
				p_ptr->redraw |= PR_STATUS;
			}
		}


		///mod140517 �K�ړ������Ƃ��AEPHEMERA�t���O�����z���̏��ŏ��� �������Ă���A�C�e���͗��Ƃ�
		//�h�b�y���Q���K�[������
		//v1.1.78 RF2_PHANTOM�����̃����X�^�[������
		for (i = 1; i < m_max; i++)
		{
			char m_name[80];
			int dist_tmp;
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			r_ptr = &r_info[m_ptr->r_idx];

			if(!(m_ptr->mflag & MFLAG_EPHEMERA) && !(r_ptr->flags2 & RF2_PHANTOM)) continue;

			monster_desc(m_name, m_ptr, 0);
			if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 )
				msg_format("%s�͌��̕���ɖ߂����B", m_name);
			else if(m_ptr->r_idx == MON_YOUMU )
				msg_format("���Ȃ��̔���͌��̗�̂ɖ߂����B");
			else
				msg_format("%s�͏������B", m_name);
			monster_drop_carried_objects(m_ptr);
			delete_monster_idx(i);
		}
		//�P��(��)��������B
		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];
			if(m_ptr->r_idx != MON_MASTER_KAGUYA) continue;
			delete_monster_idx(i);
		}
		//�h�b�y���Q���K�[������
		for (i = 1; i < m_max; i++)
		{
			monster_race *r_ptr;
			char m_name[80];
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			r_ptr = &r_info[m_ptr->r_idx];
			if(!(r_ptr->flags7 & RF7_DOPPELGANGER)) continue;

			monster_desc(m_name, m_ptr, 0);
			msg_format("���Ȃ��̃h�b�y���Q���K�[�͏������B", m_name);

			monster_drop_carried_objects(m_ptr);
			delete_monster_idx(i);
		}

		/*
		 * Maintain Unique monsters and artifact, save current
		 * floor, then prepare next floor
		 */
		leave_floor();

		/* Forget the flag */
		reinit_wilderness = FALSE;
	}

	/* Write about current level on the play record once per level */
	write_level = TRUE;
}


/*
 * Load some "user pref files"
 *
 * Modified by Arcum Dagsson to support
 * separate macro files for different realms.
 */
/*:::user-win.prf,�}�N���ݒ�t�@�C���A�����E���t�@�C���ǂݍ���*/
static void load_all_pref_files(void)
{
	char buf[1024];


	/* Access the "user" pref file */
	sprintf(buf, "user.prf");

	/* Process that file */
	process_pref_file(buf);

	/*:::user-win.prf���J���Ă���B�F�ݒ�֘A�炵��*/
	/* Access the "user" system pref file */
	sprintf(buf, "user-%s.prf", ANGBAND_SYS);

	/* Process that file */
	process_pref_file(buf);

	/*:::�푰���Ƃ�prf�t�@�C���H����炵���t�@�C���͂Ȃ�*/
	/* Access the "race" pref file */
	sprintf(buf, "%s.prf", rp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/*:::�N���X���Ƃ�prf�t�@�C���H����炵���t�@�C���͂Ȃ�*/
	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", cp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/*:::@��.prf �}�N���ݒ�t�@�C���ǂݍ��݂�*/
	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "realm 1" pref file */
	if (p_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm1]);

		/* Process that file */
		process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if (p_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm2]);

		/* Process that file */
		process_pref_file(buf);
	}


	/* Load an autopick preference file */
	autopick_load_pref(FALSE);
}


/*
 * Extract option variables from bit sets
 */
/*:::���[�h����option_flag[]��option_info[]���Q�Ƃ��e�I�v�V�����ϐ��ɓW�J���Ă���*/
///mod140406 �����ݒ肩��O�����I�v�V������������difficulty�Q�Ƃ��ēK�p���������Ƃɂ����B
void extract_option_vars(void)
{
	int i;

	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Set */
			if (option_flag[os] & (1L << ob))
			{
				/* Set */
				(*option_info[i].o_var) = TRUE;
			}

			/* Clear */
			else
			{
				/* Clear */
				(*option_info[i].o_var) = FALSE;
			}
		}
	}


	///mod140307 ��Փx�ɂ�鎩���I�v�V�����I��
	///mod140406 ���[�h�����f����Ă��Ȃ������̂�player_birth()����ړ����Ă���
	switch(difficulty)
	{
	case DIFFICULTY_EASY:
		easy_band = TRUE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_NORMAL:
		easy_band = FALSE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_HARD:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_LUNATIC:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = TRUE;
		ironman_nightmare = TRUE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_EXTRA:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = TRUE;
		break;
	}




}


/*
 * Determine bounty uniques
 */
///del131221
#if 0
void determine_bounty_uniques(void)
{
	int          i, j, tmp;
	monster_race *r_ptr;

	get_mon_num_prep(NULL, NULL);
	for (i = 0; i < MAX_KUBI; i++)
	{
		while (1)
		{
			kubi_r_idx[i] = get_mon_num(MAX_DEPTH - 1);
			r_ptr = &r_info[kubi_r_idx[i]];

			if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

			if (!(r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON))) continue;

			if (r_ptr->rarity > 100) continue;

			if (no_questor_or_bounty_uniques(kubi_r_idx[i])) continue;

			for (j = 0; j < i; j++)
				if (kubi_r_idx[i] == kubi_r_idx[j]) break;

			if (j == i) break;
		}
	}

	/* Sort them */
	for (i = 0; i < MAX_KUBI - 1; i++)
	{
		for (j = i; j < MAX_KUBI; j++)
		{
			if (r_info[kubi_r_idx[i]].level > r_info[kubi_r_idx[j]].level)
			{
				tmp = kubi_r_idx[i];
				kubi_r_idx[i] = kubi_r_idx[j];
				kubi_r_idx[j] = tmp;
			}
		}
	}
}
#endif

/*
 * Determine today's bounty monster
 * Note: conv_old is used if loaded 0.0.3 or older save file
 */
/*:::�{���̏܋��񌈒�@�폜�\��*/
///sysdel
#if 0
void determine_today_mon(bool conv_old)
{
	int max_dl = 3, i;
	bool old_inside_battle = p_ptr->inside_battle;
	monster_race *r_ptr;

	if (!conv_old)
	{
		for (i = 0; i < max_d_idx; i++)
		{
			if (max_dlv[i] < d_info[i].mindepth) continue;
			if (max_dl < max_dlv[i]) max_dl = max_dlv[i];
		}
	}
	else max_dl = MAX(max_dlv[DUNGEON_ANGBAND], 3);

	p_ptr->inside_battle = TRUE;
	get_mon_num_prep(NULL, NULL);

	while (1)
	{
		today_mon = get_mon_num(max_dl);
		r_ptr = &r_info[today_mon];

		if (r_ptr->flags1 & RF1_UNIQUE) continue;
		if (r_ptr->flags7 & (RF7_NAZGUL | RF7_UNIQUE2)) continue;
		if (r_ptr->flags2 & RF2_MULTIPLY) continue;
		if ((r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) != (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) continue;
		if (r_ptr->level < MIN(max_dl / 2, 40)) continue;
		if (r_ptr->rarity > 10) continue;
		break;
	}

	p_ptr->today_mon = 0;
	p_ptr->inside_battle = old_inside_battle;
}
#endif

/*
 * Actually play a game
 *
 * If the "new_game" parameter is true, then, after loading the
 * savefile, we will commit suicide, if necessary, to allow the
 * player to start a new game.
 */
/*:::�Q�[�����[�`���̈�ԏ�@�V�K��TRUE,�t�@�C���J�����Ƃ�FALSE*/
void play_game(bool new_game)
{
	int i;
	bool load_game = TRUE;
	bool init_random_seed = FALSE;
	int tmp_data;

	//v1.1.25 �v���C���Ԃ��o�O��񍐂���B
	//�����s������start_time�����ݒ�̂Ƃ���update_playtime()���s���Ă���炵���B
	//�Ƃ肠�����ŏ��̍ŏ���start_time��ݒ肷��悤�ɂ��Ă���
	start_time = time(NULL) - 1;

/*:::�f�[�^���p�T�[�o�H�ǂ݂̂����͖����炵��*/
#ifdef CHUUKEI
	if (chuukei_client)
	{
		reset_visuals();
		browse_chuukei();
		return;
	}

	else if (chuukei_server)
	{
		prepare_chuukei_hooks();
	}
#endif

	///\131117 sysdel
	///mod150224 ���[�r�[�����H
	if (browsing_movie)
	{
		reset_visuals();
		browse_movie();
		return;
	}
	///race �b�l�J�n���ψٗp
	hack_startup = FALSE;

	/* Hack -- Character is "icky" */
	/*:::�L�����N�^�[�����������ŃQ�[���i�s���܂��ł��Ȃ��Ƃ�TRUE?*/
	character_icky = TRUE;

	/* Make sure main term is active */
	/*:::�d�v���[�`���炵���B���̂܂܎g���B*/
	Term_activate(angband_term[0]);

	/* Initialise the resize hooks */
	/*:::������悭������񂪂��̂܂܎g���B*/
	angband_term[0]->resize_hook = resize_map;

	for (i = 1; i < 8; i++)
	{
		/* Does the term exist? */
		if (angband_term[i])
		{
			/* Add the redraw on resize hook */
			angband_term[i]->resize_hook = redraw_window;
		}
	}

	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);


	/* Attempt to load */
	/*:::�Z�[�u�t�@�C����ǂ݂ɍs���Ă�B�V�K������x����H*/
	/*:::�f�[�^�����[�h���ꂽ��character_loaded=TRUE*/
	if (!load_player())
	{
		/* Oops */
#ifdef JP
quit("�Z�[�u�t�@�C�������Ă��܂�");
#else
		quit("broken savefile");
#endif

	}

	/* Extract the options */
	/*:::�I�v�V�����̐ݒ�����t���O�p�ϐ��ɓ���Ȃ����Ă���炵��*/
	extract_option_vars();

	/* Report waited score */
	/*:::�X�R�A���M��ҋ@���Ă����ꍇ������*/
	if ( p_ptr->wait_report_score)
	{
		char buf[1024];
		bool success;

#ifdef JP
		if (!get_check_strict("�ҋ@���Ă����X�R�A�o�^�����s�Ȃ��܂����H", CHECK_NO_HISTORY))
#else
		if (!get_check_strict("Do you register score now? ", CHECK_NO_HISTORY))
#endif
			quit(0);

		//start_time�����Z�b�g
		start_time = time(NULL);

		//v1.1.25 -hack- �X�R�A�v�Z�t���O���Đݒ�
		if(p_ptr->chp >= 0 && p_ptr->total_winner)
			finish_the_game = TRUE;

		/* Update stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Update stuff */
		update_stuff();

		p_ptr->is_dead = TRUE;

		//v1.1.25 �X�R�A�T�[�o�ɑ���f�[�^��is_dead��Ԃɂ��邽�߂ɒǉ�
		if (!save_player()) msg_print("�Z�[�u���s�I");


		/* No suspending now */
		signals_ignore_tstp();
		
		/* Hack -- Character is now "icky" */
		character_icky = TRUE;

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

		/* Open the high score file, for reading/writing */
		highscore_fd = fd_open(buf, O_RDWR);
		/* Handle score, show Top scores */
		success = send_world_score(TRUE);


#ifdef JP
		if (!success && !get_check_strict("�X�R�A�o�^����߂܂����H", CHECK_NO_HISTORY))
#else
		if (!success && !get_check_strict("Do you give up score registration? ", CHECK_NO_HISTORY))
#endif
		{
#ifdef JP
			//v1.1.25 �X�R�A�T�[�o�ɑ���f�[�^��is_dead��Ԃɂ��Ă��̂��܂��߂�
			p_ptr->is_dead = FALSE;
			if (!save_player()) msg_print("�Z�[�u���s�I");
			prt("���������ҋ@���܂��B", 0, 0);
#else
			prt("standing by for future registration...", 0, 0);
#endif

			(void)inkey();
		}
		else
		{
			p_ptr->wait_report_score = FALSE;
			top_twenty();
#ifdef JP
			if (!save_player()) msg_print("�Z�[�u���s�I");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}
		/* Shut the high score file */
		(void)fd_close(highscore_fd);

		/* Forget the high score fd */
		highscore_fd = -1;
		
		/* Allow suspending now */
		signals_handle_tstp();

		quit(0);
	}
	/*:::�V�K��TRUE*/
	creating_savefile = new_game;

	/* Nothing loaded */
	/*:::�V�K���A�܂��̓��[�h���s���ɂ����ɗ���*/
	if (!character_loaded)
	{
		/* Make new player */
		new_game = TRUE;

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Prepare to init the RNG */
		init_random_seed = TRUE;

		/* Initialize the saved floors data */
		init_saved_floors(FALSE);
	}

	/* Old game is loaded.  But new game is requested. */
	/*:::�����ɗ���̂͂ǂ������������낤�H*/
	else if (new_game)
	{
		/* Initialize the saved floors data */
		init_saved_floors(TRUE);
	}

	/* Process old character */
	/*:::���[�h�������������݂̋@��ɓK�����Ă��邩�`�F�b�N���Ă�炵��*/
	if (!new_game)
	{
		/* Process the player name */
		process_player_name(FALSE);
//testmsg(format("name:%s",player_name));
	}

	/* Init the RNG */
	/*:::�V�K���A�����_���l�̎���Z�b�g*/
	if (init_random_seed)
	{
		u32b seed;

		/* Basic seed */
		seed = (time(NULL));

#ifdef SET_UID

		/* Mutate the seed on Unix machines */
		seed = ((seed >> 3) * (getpid() << 1));

#endif

		/* Seed the RNG */
		Rand_state_init(seed);
	}

	/* Roll new character */
	if (new_game)
	{

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Start in town */
		dun_level = 0;
		p_ptr->inside_quest = 0;
		p_ptr->inside_arena = FALSE;
		p_ptr->inside_battle = FALSE;

		/*:::�悭�킩��񂪓��L�L�q�p�H*/
		write_level = TRUE;

		/* Hack -- seed for flavors */
		/*:::���������̃����_���v�f�Ɏg���炵���H*/
		seed_flavor = randint0(0x10000000);

		/* Hack -- seed for town layout */
		seed_town = randint0(0x10000000);

		/* Roll up a new character */
		/*:::�L�����N�^�[���C�N�@�ŏd�v*/
		///class race seikaku system
		player_birth();

		/*:::�v���C���ԂƃZ�[�u�񐔃J�E���g�̃��Z�b�g*/
		counts_write(2,0);
		p_ptr->count = 0;

		load = FALSE;
		/*:::�܋��񃊃X�g�̐����@�폜�\��*/
		///del131221�@�܋��񃊃X�g
		//determine_bounty_uniques();
		/*:::�{���̏܋���̐ݒ�@�폜�\��*/
		///sysdel �܋���
		//determine_today_mon(FALSE);

		/* Initialize object array */
		/*:::�����Ă�A�C�e����S�č폜*/
		wipe_o_list();


	}
	else
	{
		write_level = FALSE;

#ifdef JP
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ----�Q�[���ĊJ----");
#else
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ---- Restart Game ----");
#endif
#if 0
/*
 * 1.0.9 �ȑO�̓Z�[�u�O�� p_ptr->riding = -1 �Ƃ��Ă����̂ŁA�Đݒ肪�K�v�������B
 * �����s�v�����A�ȑO�̃Z�[�u�t�@�C���Ƃ̌݊��̂��߂Ɏc���Ă����B
 */
		///sysdel
		if (p_ptr->riding == -1)
		{
			p_ptr->riding = 0;
			for (i = m_max; i > 0; i--)
			{
				if (player_bold(m_list[i].fy, m_list[i].fx))
				{
					p_ptr->riding = i;
					break;
				}
			}
		}
#endif
		///mod150315 �d�����Z
		//v1.0.50���O�ŊJ�n���ėd���Ńv���C���Ƀo�[�W�����A�b�v�����ꍇ�����ɗ���͂�
		//�d�����Z���擾���A�N�C�b�N�X�^�[�g�p��previous_char.sc�ɕۑ�����
		if(p_ptr->prace == RACE_FAIRY && !CHECK_FAIRY_TYPE) get_fairy_type(TRUE);



		//v1.1.31c �d�����Z�s��Ή�
		//birth()�œ��Z�ݒ肵���Ƃ��̃~�X�œ��Zidx���N�C�b�N�X�^�[�g�p�ɕۑ�����p_ptr->sc��prev-sc�������l��50�̂܂܂ɂȂ��Ă���̂Őݒ肵����
		if(p_ptr->prace == RACE_FAIRY && previous_char.prace == RACE_FAIRY &&
			CHECK_FAIRY_TYPE && p_ptr->sc == 50)
		{
		
			p_ptr->sc = p_ptr->race_multipur_val[0];
			previous_char.sc = p_ptr->sc;
		}


	}
	/*:::���������͐V�K���A���[�h���Ƃ�����*/

	creating_savefile = FALSE;
	/*:::�l�X�ȃt���O�̃��Z�b�g*/
	p_ptr->teleport_town = FALSE;
	p_ptr->sutemi = FALSE;
	world_monster = FALSE;
	now_damaged = FALSE;
	now_message = 0;
	start_time = time(NULL) - 1;
	record_o_name[0] = '\0';

	/* Reset map panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Sexy gal gets bonus to maximum weapon skill of whip */
	/*:::�Ȃ��L�������C�N���ɏ����Ȃ��̂�����Ȃ��B�ǂ���ɂ��Ă��폜�\��*/
	///del131223 seikaku
	//if (p_ptr->pseikaku == SEIKAKU_SEXY)
	//	s_info[p_ptr->pclass].w_max[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_MASTER;

	///mod141005 ���j�[�N�N���X��2P�L�����̐F��ς��Ă݂�
	for (i = 1; i < max_r_idx; i++) if(monster_is_you(i)) r_info[i].d_attr = (r_info[i].d_attr)%15+1;

	/* Fill the arrays of floors and walls in the good proportions */
	/*:::�O���[�o���ϐ�floor_type��fill_type�Ƀ_���W�����̎�ނɂ��n�`�o�����ɉ����Ēn�`�f�[�^��100��i�[���Ă���B*/
	/*:::�V�K�̎��́u�r��v�Ƃ��Ď��s�����H�@�ǂ��������ł��̊֐����Ă΂�闝�R��������Ȃ��B���[�h���ɂ͂ǂ��Ȃ�̂��H*/
	set_floor_and_wall(dungeon_type);

	/* Flavor the objects */
	/*:::��Ȃǂ̖��Ӓ莞���������_���ɂ���̂��Ǝv��*/
	/*:::���[�h���������_���̃V�[�h���ێ�����Ă���̂œ������тɂȂ�*/
	flavor_init();

	//v1.1.85 �����o�Ȃ��I�v�V���������@�S��1�K�Ł������������Ă������B
	//����܂�change_floor()�̌�ɂ��̏��������s���Ă����AEXTRA���ƒn����K�����̂ق��������B�ӓ_�������B
	//�R�[�h���܂�ǂݒ������G�ɃR�s�[�B
	//TODO:���X���������̕��̓����������s�v���ǂ������ׂď����Ƃ�
	if (ironman_no_fixed_art)
	{
		int a_idx;
		for (a_idx = 1; a_idx < max_a_idx; a_idx++)	a_info[a_idx].cur_num = 1;
	}


	/* Flash a message */
#ifdef JP
prt("���҂�������...", 0, 0);
#else
	prt("Please wait...", 0, 0);
#endif


	/* Flush the message */
	Term_fresh();


	/* Hack -- Enter wizard mode */
	/*:::����̓E�B�U�[�h���[�h�˓��̂Ƃ�true�ɂȂ�̂��H�������Ă�True�ɂȂ鏈������������Ȃ���*/
	/*:::�ǂ��������̓R�}���h���C������f�o�b�O�I�v�V�����t����heng�𗧂��グ���Ƃ�����炵���H*/
	if (arg_wizard)
	{
		if (enter_wizard_mode())
		{
			p_ptr->wizard = TRUE;

			if (p_ptr->is_dead || !py || !px)
			{
				/* Initialize the saved floors data */
				init_saved_floors(TRUE);

				/* Avoid crash */
				p_ptr->inside_quest = 0;

				/* Avoid crash in update_view() */
				py = px = 10;
			}
		}
		else if (p_ptr->is_dead)
		{
			quit("Already dead.");
		}
	}

	/* Initialize the town-buildings if necessary */
	/*:::�r�삩���ɋ��ăN�G�X�g���łȂ��ꍇ�A*/
	if (!dun_level && !p_ptr->inside_quest)
	{
		/* Init the wilderness */
		/*:::���[���h�}�b�v�̓ǂݍ���*/
		process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);
		/* Init the town */
		init_flags = INIT_ONLY_BUILDINGS;
		/*:::���̓ǂݍ��� t_pref.txt��ǂ񂾌��p_ptr->town_num���Q�Ƃ����ꂼ��̃t�@�C����ǂ�*/
		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);

		select_floor_music(); //v1.1.58

	}
//testmsg(format("character_dungeon:%x",character_dungeon));
	/*:::�G�N�X�g�����[�h�͓S��1�K����J�n*/
	if(EXTRA_MODE && new_game)
	{
		dungeon_type = DUNGEON_ANGBAND;
		p_ptr->enter_dungeon = TRUE;
		dun_level = 1;
		prepare_change_floor_mode(CFM_RAND_PLACE | CFM_FIRST_FLOOR);
		change_floor();

	}

	/* Generate a dungeon level if needed */
	/*:::�t���A�𐶐��B�V�K���ɂ͂����ɗ��ĊX�����������͂������A���̏����ʒu�͂ǂ����܂�񂾂낤*/
	else if (!character_dungeon)
	{
		change_floor();
	}

	else
	{
		/* HACK -- Restore from panic-save */
		if (p_ptr->panic_save)
		{
//testmsg(format("py:%d px:%d cur_wid:%d cur_hgt:%d",py,px,cur_wid,cur_hgt));
			/* No player?  -- Try to regenerate floor */
			if (!py || !px)
			{
#ifdef JP
				msg_print("�v���C���[�̈ʒu�����������B�t���A���Đ������܂��B");
#else
				msg_print("What a strange player location.  Regenerate the dungeon floor.");
#endif
				change_floor();
			}

			/* Still no player?  -- Try to locate random place */
			if (!py || !px) py = px = 10;

			/* No longer in panic */
			p_ptr->panic_save = 0;
		}
	}

	/* Character is now "complete" */
	character_generated = TRUE;


	/* Hack -- Character is no longer "icky" */
	character_icky = FALSE;


	if (new_game)
	{
		char buf[80];

#ifdef JP
		sprintf(buf, "%s�ɍ~�藧�����B", map_name());
#else
		sprintf(buf, "You are standing in the %s.", map_name());
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, buf);
	}


	/* Start game */
	p_ptr->playing = TRUE;

	/* Reset the visual mappings */
	/*:::��ʂ̕\���ݒ�������l�ɖ߂��H*/
	reset_visuals();

	/* Load the "pref" files */
	load_all_pref_files();

	/*::: -Hack- �����o�Ȃ��I�v�V���������@�S�Ắ����o���ς݂ɂ��� �Q�[���J�n��Ɂ����ǉ����ꂽ�ꍇ�ɔ������[�h���ɂ����̏���������B*/
	if(ironman_no_fixed_art)
	{
		int a_idx;
		for (a_idx = 1; a_idx < max_a_idx; a_idx++)	a_info[a_idx].cur_num = 1;
	}


	/* Give startup outfit (after loading pref files) */
	/*:::�Q�[���J�n���A�����Ă���A�C�e�������߂鏈���Ȃ�*/
	if (new_game)
	{
		//�L�������C�N���̏��������A�C�e���𓾂�
		player_outfit();

		///mod150315 �d�����Z
		/*::: -Hack- �d���N�C�b�N�X�^�[�g */
		//�J�n���ɋZ�������Ă��Ȃ��ꍇ�A�N�C�b�N�X�^�[�g�Ȃ̂�sc�ɕۑ�����Ă���͂��̓��Z�l���R�s�[
		//sc��50�̏ꍇ�A���o�[�W��������̃N�C�b�N�X�^�[�g�ł܂����Z�t�^�������s���Ă��Ȃ��̂ōĕt�^����
		if(p_ptr->prace == RACE_FAIRY && !CHECK_FAIRY_TYPE && !(cp_ptr->flag_only_unique))
		{
			//if(p_ptr->sc == 50) v1.1.31c ���A���e�B100�̃_�~�[���I�������s����������̂ŏ������ύX
			if(p_ptr->sc <= FAIRY_TYPE_MAX && fairy_table[p_ptr->sc].rarity >= 100 )
			{
				get_fairy_type(TRUE);
			}
			else
			{
				if(p_ptr->sc > FAIRY_TYPE_MAX || p_ptr->sc < 1 )
				{
					msg_format("ERROR:p_ptr->sc�ɕۑ�����Ă���͂��̗d�����Z�ݒ�l����������(%d)",p_ptr->sc);
					get_fairy_type(TRUE);
				}
				else 
				{
					p_ptr->race_multipur_val[0] = p_ptr->sc;
				}
			}
		}
		//v1.1.70 ������N�C�b�N�X�^�[�g�̏������͌���
		if (p_ptr->prace == RACE_ANIMAL_GHOST && !CHECK_ANIMAL_GHOST_STRIFE)
		{
			get_animal_ghost_family(FALSE);
		}

	}

	/* React to changes */
	/*:::�ڍוs���B�E�B���h�E�֘A�H*/
	Term_xtra(TERM_XTRA_REACT, 0);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Window stuff */
	/*:::�T�u�E�B���h�E�Ɋւ���ݒ�H*/
	window_stuff();


	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Hack -- Enforce "delayed death" */
	/*:::���炩�̗��R�ŃZ�[�u���Ɏ��񂾂Ƃ��̏����H���񂾂Ƃ��ɕϋ�ؓ{�������I�������炽�Ԃ񂱂��ɗ���H*/
	if (p_ptr->chp < 0) p_ptr->is_dead = TRUE;

	if (p_ptr->prace == RACE_ANDROID) calc_android_exp();

	/*:::�R���E���b�g���j���[�Q�[�����̔n��������*/
	///class mon
	if (new_game && !EXTRA_MODE && ((p_ptr->pclass == CLASS_CAVALRY) || (p_ptr->pclass == CLASS_BEASTMASTER)))
	{
		monster_type *m_ptr;
		int pet_r_idx = ((p_ptr->pclass == CLASS_CAVALRY) ? MON_HORSE : MON_YASE_HORSE);
		monster_race *r_ptr = &r_info[pet_r_idx];
		place_monster_aux(0, py, px - 1, pet_r_idx,
				  (PM_FORCE_PET | PM_NO_KAGE));
		m_ptr = &m_list[hack_m_idx_ii];
		m_ptr->mspeed = r_ptr->speed;
		m_ptr->maxhp = r_ptr->hdice*(r_ptr->hside+1)/2;
		m_ptr->max_maxhp = m_ptr->maxhp;
		m_ptr->hp = r_ptr->hdice*(r_ptr->hside+1)/2;
		m_ptr->energy_need = ENERGY_NEED() + ENERGY_NEED();
	}

	/*:::Mega Hack - �P��N�G�X�g�i�s�x�ɍ��킹�A�Y���A�C�e����QUESTITEM�t���O���O���Ēʏ퐶�������悤�ɂ���*/
	///mod160721 �P��v���C���͑S��QUESTITEM�̂܂�
	if(kaguya_quest_level < KAGUYA_QUEST_MAX && quest[QUEST_KAGUYA].status < QUEST_STATUS_COMPLETED && p_ptr->pclass != CLASS_KAGUYA)
	{
		a_info[kaguya_quest_art_list[kaguya_quest_level]].gen_flags &= ~(TRG_QUESTITEM);
 	}
	//v1.1.26 �P��N�G�X�g������͓K���ɂǂꂩ��t���O�O���B�N���̂��тɕς��Ȃ��悤�ɂ���K�v������̂�seed_flavor���g���B
	//���ꂾ�ƋP��N�G�̍Ō�̃A�C�e���̔[�i���ς܂����L�����̎��ȍ~�̋N�����ɂ�����_�󂪐�������Ă��܂��\�������邪�A�܂��債�����ɂ͂Ȃ�Ȃ����B
	else if(kaguya_quest_level == KAGUYA_QUEST_MAX && p_ptr->pclass != CLASS_KAGUYA)
	{
		a_info[kaguya_quest_art_list[(seed_flavor % KAGUYA_QUEST_MAX)]].gen_flags &= ~(TRG_QUESTITEM);
 	}

	///mod160310 Extra���[�h�ł͈ꕔ�����������X�^�[��QUESTOR�t���O���O��
	if(EXTRA_MODE)
	{
		for (i = 1; i < max_r_idx; i++)
		{
			//v1.1.92 俎q(���Z���p)���o���Ȃ��悤�ǉ�

			if(i == MON_TAISAI || i == MON_OBERON || i== MON_SERPENT || i == MON_MORGOTH || i == MON_AZATHOTH || i == MON_FLAN || i == MON_SUMIREKO_2
			|| i == MON_GOLFIMBUL || i == MON_HECATIA1 || i == MON_HECATIA2 || i == MON_HECATIA3 || i == MON_MIKO || i == MON_ERIC)
			{
				r_info[i].flags1 |= (RF1_QUESTOR);
			}
			else
			{
				r_info[i].flags1 &= ~(RF1_QUESTOR);
			}
		}
	}

	//�f�o�b�O�A�f�[�^�~�o�p

	if(0)
	{
			Term_clear();
			put_str("***�����l���A��***", 17 , 40);
			(void)inkey();
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
			(void)set_food(PY_FOOD_MAX - 1);
			reset_tim_flags();
			dun_level = 0;
			p_ptr->inside_arena = FALSE;
			p_ptr->inside_battle = FALSE;
			leave_quest_check();
			leaving_quest = 0;
			p_ptr->inside_quest = 0;
			if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
			dungeon_type = 0;
			py = 33;
			px = 180;
			p_ptr->wilderness_y = 34;
			p_ptr->wilderness_x = 39;
			p_ptr->oldpy = 33;
			p_ptr->oldpx = 180;
			p_ptr->wild_mode = FALSE;
			wipe_m_list();
			leave_floor();
			change_floor();

	}

	/*:::�X��Ƃ̃A�C�e�����܂Ƃ߁A���i�Ǝ�ޏ��ɐ��񂷂�*/
	(void)combine_and_reorder_home(STORE_HOME);
	(void)combine_and_reorder_home(STORE_MUSEUM);

	select_floor_music(); //v1.1.58

	/* Process */
	/*:::���C�����[�`��*/
	while (TRUE)
	{

		/* Process the level */
		/*:::���̊K�ł̏����@�K�ړ��A���S�A�Q�[���I��������o��*/
		dungeon(load_game);

		/* Handle "p_ptr->notice" */
		notice_stuff();

		/* Hack -- prevent "icky" message */
		character_xtra = TRUE;

		/* Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window" */
		handle_stuff();

		character_xtra = FALSE;

		/* Cancel the target */
		target_who = 0;

		/* Cancel the health bar */
		health_track(0);


		/* Forget the lite */
		forget_lite();

		/* Forget the view */
		forget_view();

		/* Forget the view */
		clear_mon_lite();

		/* Handle "quit and save" */
		if (!p_ptr->playing && !p_ptr->is_dead) break;

		/* Erase the old cave */
		wipe_o_list();
		if (!p_ptr->is_dead) wipe_m_list();


		/* XXX XXX XXX */
		msg_print(NULL);

		load_game = FALSE;

		/* Accidental Death */
		/*:::�������ނ�ؕ��łȂ����񂾂Ƃ��@�����ł̓A���[�i�ƃE�B�U�[�h���[�h�̕�����������*/

		if (p_ptr->playing && p_ptr->is_dead)
		{
			//v1.1.51 �V�A���[�i�����ɔ�����������
#if 0
			if (p_ptr->inside_arena)
			{
				p_ptr->inside_arena = FALSE;
				if (p_ptr->arena_number > MAX_ARENA_MONS)
					p_ptr->arena_number++;
				else
					p_ptr->arena_number = -1 - p_ptr->arena_number;
				p_ptr->is_dead = FALSE;
				p_ptr->chp = 0;
				p_ptr->chp_frac = 0;
				p_ptr->exit_bldg = TRUE;
				reset_tim_flags();

				/* Leave through the exit */
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_RAND_CONNECT);

				/* prepare next floor */
				leave_floor();
			}
#endif
			if (p_ptr->inside_arena)
			{
				p_ptr->inside_arena = FALSE;

				p_ptr->is_dead = FALSE;
				p_ptr->exit_bldg = TRUE;
				reset_tim_flags();
				//v1.1.55 �s�k����HP��S���łȂ��A���[�i�ɓ������Ƃ��ɖ߂�
				p_ptr->chp = MIN(nightmare_record_hp,p_ptr->mhp);
				p_ptr->chp_frac = 0;

				//v1.1.54
				if (p_ptr->food < PY_FOOD_ALERT) p_ptr->food = PY_FOOD_ALERT - 1;

				/* Leave through the exit */

				//�s�k��ɍĒ��킵���烂���X�^�[�̔z�u�������ɂȂ��Ă�̂�CFM_NO_RETURN�ɕς���
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

				/* prepare next floor */
				leave_floor();
			}
			else if (p_ptr->inside_quest == QUEST_MIYOI)
			{
				p_ptr->is_dead = FALSE;
				(void)set_blind(0);
				(void)set_confused(0);
				(void)set_poisoned(0);
				(void)set_afraid(0);
				(void)set_paralyzed(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);
				set_alcohol(0);
				p_ptr->chp = p_ptr->mhp;
				p_ptr->word_recall = 0;
				p_ptr->redraw |= (PR_STATUS);
				leave_quest_check();
				p_ptr->inside_quest = 0;
				dun_level = 0;
				p_ptr->leaving = TRUE;
				if(p_ptr->pclass == CLASS_DOREMY)
					msg_print("���Ȃ��͈������痣�E�����B");
				else
					msg_print("�͂��I�����������B");
			}
			else
			{
				/* Mega-Hack -- Allow player to cheat death */
#ifdef JP
				if ((p_ptr->wizard || cheat_live || difficulty == DIFFICULTY_EASY) && get_check("�R���e�B�j���[���܂���? "))
#else
				if ((p_ptr->wizard || cheat_live) && !get_check("Die? "))
#endif
				{
					//v1.1.70 �R���e�B�j���[�����Ƃ���SC�ƔN������Z�b�g���鏈�����폜
					/* Mark social class, reset age, if needed */
					//if (p_ptr->sc) p_ptr->sc = p_ptr->age = 0;

					/* Increase age */
					p_ptr->age++;

					/* Mark savefile */
					if(difficulty != DIFFICULTY_EASY) p_ptr->noscore |= 0x0001;

					/* Message */
#ifdef JP
				//	msg_print("�E�B�U�[�h���[�h�ɔO�𑗂�A�����\�����B");
#else
					msg_print("You invoke wizard mode and cheat death.");
#endif
					msg_print(NULL);

					/* Restore hit points */
					p_ptr->chp = p_ptr->mhp;
					p_ptr->chp_frac = 0;

					if (p_ptr->pclass == CLASS_MAGIC_EATER)
					{
						for (i = 0; i < EATER_EXT*2; i++)
						{
							p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
						}
						for (; i < EATER_EXT*3; i++)
						{
							p_ptr->magic_num1[i] = 0;
						}
					}
					/* Restore spell points */
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;

					/* Hack -- cancel recall */
					if (p_ptr->word_recall)
					{
						/* Message */
#ifdef JP
						msg_print("����߂���C�����ꋎ����...");
#else
						msg_print("A tension leaves the air around you...");
#endif

						msg_print(NULL);

						/* Hack -- Prevent recall */
						p_ptr->word_recall = 0;
						p_ptr->redraw |= (PR_STATUS);
					}

					/* Hack -- cancel alter */
					if (p_ptr->alter_reality)
					{
						/* Hack -- Prevent alter */
						p_ptr->alter_reality = 0;
						p_ptr->redraw |= (PR_STATUS);
					}

					/* Note cause of death XXX XXX XXX */
#ifdef JP
					(void)strcpy(p_ptr->died_from, "�R���e�B�j���[");
#else
					(void)strcpy(p_ptr->died_from, "Cheating death");
#endif

					/* Do not die */
					p_ptr->is_dead = FALSE;

					/* Hack -- Healing */
					(void)set_blind(0);
					(void)set_confused(0);
					(void)set_poisoned(0);
					(void)set_afraid(0);
					(void)set_paralyzed(0);
					(void)set_image(0);
					(void)set_stun(0);
					(void)set_cut(0);
					set_alcohol(0);

					if(p_ptr->pclass == CLASS_HINA) hina_gain_yaku(-99999);

					/* Hack -- Prevent starvation */
					(void)set_food(PY_FOOD_MAX - 1);

					dun_level = 0;
					p_ptr->inside_arena = FALSE;
					p_ptr->inside_battle = FALSE;
					leaving_quest = 0;
					p_ptr->inside_quest = 0;
					if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
					dungeon_type = 0;
					if (lite_town || vanilla_town)
					{
						p_ptr->wilderness_y = 1;
						p_ptr->wilderness_x = 1;
						if (vanilla_town)
						{
							p_ptr->oldpy = 10;
							p_ptr->oldpx = 34;
						}
						else
						{
							p_ptr->oldpy = 33;
							p_ptr->oldpx = 180;
						}
					}
					else
					{
						p_ptr->wilderness_y = 34;
						p_ptr->wilderness_x = 39;
						p_ptr->oldpy = 33;
						p_ptr->oldpx = 180;
					}

					/* Leaving */
					p_ptr->wild_mode = FALSE;
					p_ptr->leaving = TRUE;

#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            �������A�R���e�B�j���[�����B");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            but revived.");
#endif

					/* Prepare next floor */
					leave_floor();
					wipe_m_list();
				}
			}
		}
		///mod150620 俎q���ꏈ���@��ჁA�ӎ��s���āA�}���A�����ŋ����I�ɐl�������֖߂�
		else if(flag_sumireko_wakeup)
		{

			Term_clear();
			put_str("���Ȃ��͈ӎ����������E�E", 17 , 40);
			(void)inkey();
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
			(void)set_food(PY_FOOD_MAX - 1);
			reset_tim_flags();
			dun_level = 0;
			p_ptr->inside_arena = FALSE;
			p_ptr->inside_battle = FALSE;
			leave_quest_check();
			leaving_quest = 0;
			p_ptr->inside_quest = 0;
			if (dungeon_type) p_ptr->recall_dungeon = dungeon_type;
			dungeon_type = 0;
			p_ptr->wilderness_y = 34;
			p_ptr->wilderness_x = 39;
			p_ptr->oldpy = 33;
			p_ptr->oldpx = 180;
			p_ptr->wild_mode = FALSE;
			leave_floor();
			wipe_m_list();
		}

		/* Handle "death" */
		if (p_ptr->is_dead) break;

		/* Make a new level */
		change_floor();
	}

	/* Close stuff */
	close_game();

	/* Quit */
	quit(NULL);
}

///mod140126 �A���f�b�h���邩��Q�[���J�n����̂���߂�
s32b turn_real(s32b hoge)
{
#if 0
	switch (p_ptr->start_race)
	{
	case RACE_VAMPIRE:
	case RACE_SKELETON:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		return hoge - (TURNS_PER_TICK * TOWN_DAWN * 3 / 4);
	default:
		return hoge;
	}
#endif
		return hoge;

}

/*
 * �^�[���̃I�[�o�[�t���[�ɑ΂���Ώ�
 * �^�[���y�у^�[�����L�^����ϐ����^�[���̌��E��1���O�܂Ŋ����߂�.
 */
void prevent_turn_overflow(void)
{
	int rollback_days, i, j;
	s32b rollback_turns;

	if (turn < turn_limit) return;

	rollback_days = 1 + (turn - turn_limit) / (TURNS_PER_TICK * TOWN_DAWN);
	rollback_turns = TURNS_PER_TICK * TOWN_DAWN * rollback_days;

	if (turn > rollback_turns) turn -= rollback_turns;
	else turn = 1; /* Paranoia */
	if (old_turn > rollback_turns) old_turn -= rollback_turns;
	else old_turn = 1;
	if (old_battle > rollback_turns) old_battle -= rollback_turns;
	else old_battle = 1;
	if (p_ptr->feeling_turn > rollback_turns) p_ptr->feeling_turn -= rollback_turns;
	else p_ptr->feeling_turn = 1;

	for (i = 1; i < max_towns; i++)
	{
		for (j = 0; j < MAX_STORES; j++)
		{
			store_type *st_ptr = &town[i].store[j];

			if (st_ptr->last_visit > -10L * TURNS_PER_TICK * STORE_TICKS)
			{
				st_ptr->last_visit -= rollback_turns;
				if (st_ptr->last_visit < -10L * TURNS_PER_TICK * STORE_TICKS) st_ptr->last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;
			}

			if (st_ptr->store_open)
			{
				st_ptr->store_open -= rollback_turns;
				if (st_ptr->store_open < 1) st_ptr->store_open = 1;
			}
		}
	}
}
