/* File: cmd6.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Object commands */

#include "angband.h"

static void do_cmd_quaff_potion_aux(int item, bool flag_ignore_warning);

/*
 * This file includes code for eating food, drinking potions,
 * reading scrolls, aiming wands, using staffs, zapping rods,
 * and activating artifacts.
 *
 * In all cases, if the player becomes "aware" of the item's use
 * by testing it, mark it as "aware" and reward some experience
 * based on the object's level, always rounding up.  If the player
 * remains "unaware", mark that object "kind" as "tried".
 *
 * This code now correctly handles the unstacking of wands, staffs,
 * and rods.  Note the overly paranoid warning about potential pack
 * overflow, which allows the player to use and drop a stacked item.
 *
 * In all "unstacking" scenarios, the "used" object is "carried" as if
 * the player had just picked it up.  In particular, this means that if
 * the use of an item induces pack overflow, that item will be dropped.
 *
 * For simplicity, these routines induce a full "pack reorganization"
 * which not only combines similar items, but also reorganizes various
 * items to obey the current "sorting" method.  This may require about
 * 400 item comparisons, but only occasionally.
 *
 * There may be a BIG problem with any "effect" that can cause "changes"
 * to the inventory.  For example, a "scroll of recharging" can cause
 * a wand/staff to "disappear", moving the inventory up.  Luckily, the
 * scrolls all appear BEFORE the staffs/wands, so this is not a problem.
 * But, for example, a "staff of recharging" could cause MAJOR problems.
 * In such a case, it will be best to either (1) "postpone" the effect
 * until the end of the function, or (2) "change" the effect, say, into
 * giving a staff "negative" charges, or "turning a staff into a stick".
 * It seems as though a "rod of recharging" might in fact cause problems.
 * The basic problem is that the act of recharging (and destroying) an
 * item causes the inducer of that action to "move", causing "o_ptr" to
 * no longer point at the correct item, with horrifying results.
 *
 * Note that food/potions/scrolls no longer use bit-flags for effects,
 * but instead use the "sval" (which is also used to sort the objects).
 */


 //v1.1.49 ���������������ݐH�������Ƃ��ɂׂ��J�߂��܂��� �N���X�������łȂ��Ƃ�FALSE��Ԃ�
 //high_quality:TRUE�̏ꍇ���ʂɔ���������
 //kind:�u�����m��Ȃ���������v�̕���
static bool shion_comment_food(bool high_quality, cptr kind)
{
	int num = 3;

	if (kind) num++;

	if (p_ptr->pclass != CLASS_SHION) return FALSE;

	if (high_quality)
	{
		switch (randint1(num))
		{
		case 1:
			msg_format("����͖{���ɔ��������I���Ȃ��͊��܂Ɉ�т��y�����Â����B");
			break;
		case 2:
			msg_format("����͖{���ɔ��������I���ꂱ���҂��ɂ߂��V�E�̗����ɈႢ�Ȃ��B");
			break;
		default:
			msg_format("����͖{���ɔ��������I���Ȃ��͉��Y��Ď����̂ЂƂƂ��ɐZ�����B");
			break;
		}

	}
	else
	{
		switch (randint1(num))
		{
		case 1:
			msg_format("�u�����[�I�Ȃ�Ĕ��������́I�v");
			break;
		case 2:
			msg_format("�u�����ĂĂ悩�����I�����Ă邱�ƂɊ��ӂ���[�I�v");
			break;
		case 3:
			msg_format("�u�����A�Ȃ�č��؂Ȃ́I�v");
			break;
		default:
			msg_format("�u���O���m��Ȃ�%s����A���̖����肪�Ɓ[�I�v", kind);
			break;
		}
	}
	return TRUE;
}

/*:::�H���ێ���ʁ@���́��ɐH�ׂ���A�C�e�������łɑI���ς�*/
///item race sys ������H�ׂ��Ƃ��̌���
///mod131223 �H�Ɛێ�@�S�ʕύX tval,sval
//v2.0.6 �ޖ����g���̂�static���O���Ăق�����Ăׂ�悤�ɁB�܂��H�ׂ��Ƃ���TRUE,�H�ׂ��Ȃ������Ƃ�FALSE��Ԃ�
bool do_cmd_eat_food_aux(int item)
{
	int ident, lev;
	object_type *o_ptr;
	int food_val;

	bool no_bad_effect = FALSE;

	if(p_ptr->pclass == CLASS_EIRIN) no_bad_effect = TRUE;
	if (p_ptr->pclass == CLASS_YUMA) no_bad_effect = TRUE;

	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any()) stop_hex_spell_all();

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

	//v2.0.6 �ޖ��̎����ɂ��pval�ʂ�ɖ����x����������Ƃ͌���Ȃ��Ȃ����̂ň�xfood_val�ɋL�^���K�v�ɉ����ď���������
	food_val = o_ptr->pval;

	if (cheat_xtra && p_ptr->pclass == CLASS_YUMA) 
		msg_format("eat_food_aux - item:%d tval:%d sval:%d num:%d", item, o_ptr->tval, o_ptr->sval, o_ptr->number);

	/* Sound */
	sound(SOUND_EAT);

	/* Take a turn */
	energy_use = 100;

	/* Identity not known yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	if (o_ptr->tval == TV_MUSHROOM)
	{
		shion_comment_food(FALSE, "�L�m�R");


		/* Analyze the food */
		switch (o_ptr->sval)
		{
			case SV_MUSHROOM_POISON:
			{
				if(no_bad_effect){ident=TRUE;break;}

				if(p_ptr->pclass == CLASS_MEDICINE)
				{
					int damage = damroll(8,8);
					ident = TRUE;
					hp_player(damage);
					set_food(p_ptr->food + 5000);
					break;
				}
				else if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))
				{
					if (set_poisoned(p_ptr->poisoned + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 5000);
				break;
			}

			case SV_MUSHROOM_BLINDNESS:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_blind)
				{
					if (set_blind(p_ptr->blind + randint0(200) + 200))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_PARANOIA:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_fear)
				{
					if (set_afraid(p_ptr->afraid + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_CONFUSION:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_conf)
				{
					if (set_confused(p_ptr->confused + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_HALLUCINATION:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->resist_chaos)
				{
					if (set_image(p_ptr->image + randint0(250) + 250))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_PARALYSIS:
			{
				if(no_bad_effect){ident=TRUE;break;}
				if (!p_ptr->free_act)
				{
					if (set_paralyzed(p_ptr->paralyzed + randint0(10) + 10))
					{
						ident = TRUE;
					}
				}
				break;
			}

			case SV_MUSHROOM_WEAKNESS:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "�œ���H��", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_STR);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_SICKNESS:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "�œ���H��", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(6, 6), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_CON);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_STUPIDITY:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "�œ���H��", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_INT);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_NAIVETY:
			{
				if(no_bad_effect){ident=TRUE;break;}
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "�œ���H��", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(8, 8), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_WIS);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_UNHEALTH:
			{
				if(no_bad_effect){ident=TRUE;break;}

#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "�œ���H��", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_CON);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_DISEASE:
			{
				if(no_bad_effect){ident=TRUE;break;}

#ifdef JP
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "�œ���H��", -1);
#else
				take_hit(DAMAGE_NOESCAPE, damroll(10, 10), "poisonous food", -1);
#endif

				(void)do_dec_stat(A_STR);
				ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_POISON:
			{
				if(no_bad_effect){ident=TRUE;}

				if(p_ptr->pclass == CLASS_YAMAME)
				{
					ident = TRUE;
					msg_print("����͓ŏ������I���Ȃ��͋ꂵ���Ȃ��Ă����B");
					take_hit(DAMAGE_NOESCAPE, 50 + randint1(50), "��ł̃L�m�R", -1);
				}
				else if(p_ptr->pclass == CLASS_MEDICINE)
				{
					ident = TRUE;
					msg_print("����͓ŏ������I�̂���͂�������I");
					(void)set_food(PY_FOOD_STARVE - 1);
				}
				else if (set_poisoned(0)) ident = TRUE;

				break;
			}

			case SV_MUSHROOM_CURE_BLINDNESS:
			{
				if(no_bad_effect){ident=TRUE;}
				if (set_blind(0)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_PARANOIA:
			{
				if(no_bad_effect){ident=TRUE;}
				if (set_afraid(0)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_CONFUSION:
			{
				if(no_bad_effect){ident=TRUE;}
				if (set_confused(0)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_CURE_SERIOUS:
			{
				if(no_bad_effect){ident=TRUE;}
				if (hp_player(damroll(4, 8))) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_RESTORE_STR:
			{
				if(no_bad_effect){ident=TRUE;}
				if (do_res_stat(A_STR)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_RESTORE_CON:
			{
				if(no_bad_effect){ident=TRUE;}
				if (do_res_stat(A_CON)) ident = TRUE;
				break;
			}

			case SV_MUSHROOM_RESTORING:
			{
				if(no_bad_effect){ident=TRUE;}
				if (do_res_stat(A_STR)) ident = TRUE;
				if (do_res_stat(A_INT)) ident = TRUE;
				if (do_res_stat(A_WIS)) ident = TRUE;
				if (do_res_stat(A_DEX)) ident = TRUE;
				if (do_res_stat(A_CON)) ident = TRUE;
				if (do_res_stat(A_CHR)) ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MANA:
			{
				if(no_bad_effect){ident=TRUE;}
				if(p_ptr->csp < p_ptr->msp)
				{
					ident = TRUE;
					msg_print("���������n�b�L�������B");
				}
				p_ptr->csp += damroll(8,8);
				if (p_ptr->csp >= p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
				}
				p_ptr->redraw |= (PR_MANA);
				break;

			}
			case SV_MUSHROOM_BERSERK:
			{
				if(no_bad_effect){ident=TRUE;break;}
				msg_print("�ށH���̃L�m�R�́E�E");
				set_shero(randint1(25) + 25, FALSE);
				hp_player(30);
				set_afraid(0);
				break;
			}
			case SV_MUSHROOM_MON_RED:
			{
				set_oppose_fire(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_WHITE:
			{
				set_oppose_cold(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_BLUE:
			{
				set_oppose_elec(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_BLACK:
			{
				set_oppose_acid(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_GREEN:
			{
				set_oppose_pois(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_GAUDY:
			{
				set_tim_res_chaos(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_GRAY:
			{
				set_tim_res_nether(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_CLEAR:
			{
				set_shield(10 + randint0(11), FALSE);
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_MON_SUPER:
			{
				int percentage;
			
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_mimic(20 + randint1(20), MIMIC_GIGANTIC, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
				ident = TRUE;
				break;
			}
			case SV_MUSHROOM_PUFFBALL:
			{
				if(p_ptr->pclass != CLASS_SHION)
					msg_print("���͂Ƃ������H�׉����͔��Q���B");
				ident = TRUE;

			}
			break;

		}
	}
	else if (o_ptr->tval == TV_FOOD)
	{



		/* Analyze the food */
		switch (o_ptr->sval)
		{

#ifdef JP
			case SV_FOOD_VENISON:
			{
				if (shion_comment_food(FALSE, "����")) break;

				if(p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print("���Ȃ��͉����ɂ�����H�ׂ��Ȃ��B");
					return FALSE;
				}
				if(p_ptr->prace == RACE_LUNARIAN)
				{
					msg_print("���̐H�����q��ɖ����Ă��ĐH�ׂ��Ȃ��B");
					return FALSE;
				}
				msg_print("���������������Ă��������B");
				ident = TRUE;
				break;
			}

			case SV_FOOD_SLIMEMOLD:
			{
				if (shion_comment_food(FALSE, "�S��")) break;

				if(p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print("����͓��Ȃ̂��A���Ȃ̂��H�O�̂��߂Ɏ~�߂Ă������B");
					return FALSE;
				}
				if(p_ptr->prace == RACE_LUNARIAN)
				{
					msg_print("����ȋC�����̈������̂�H�ׂ����Ȃ��B");
					return FALSE;
				}
				msg_print("����͂Ȃ�Ƃ��`�e�������������B");
				ident = TRUE;
				break;
			}
			case SV_FOOD_HARDBUSC:
			{
				if (shion_comment_food(FALSE, "����")) break;

				msg_print("���΂��������C�Ȃ��B");
				ident = TRUE;
				break;
			}
			case SV_FOOD_CUCUMBER:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				if(p_ptr->prace == RACE_KAPPA) msg_print("�ނށE�E���̂��イ��͍ō��ɔ��������I");
				else msg_print("�|���|�����č��΂������������C�Ȃ��B");
				ident = TRUE;
				break;
			}
			case SV_FOOD_RATION:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				msg_print("����͂��������B");
				ident = TRUE;
				break;
			}
			case SV_FOOD_ABURAAGE:
			{
				if (shion_comment_food(FALSE, NULL)) break;

				if(p_ptr->prace == RACE_YOUKO) msg_print("�ނށE�E���̖��g���͍ō��ɔ��������I");
				else msg_print("���΂���������������Ȃ��B");
				ident = TRUE;
				break;
			}
#else
			case SV_FOOD_RATION:
			case SV_FOOD_BISCUIT:
			case SV_FOOD_JERKY:
			case SV_FOOD_SLIME_MOLD:
			{
				msg_print("That tastes good.");
				ident = TRUE;
				break;
			}
#endif


			case SV_FOOD_SENTAN:
			{
#ifdef JP
				msg_print("�����Ȋۖ�����ݍ��񂾁B");
#else
				msg_print("That tastes good.");
#endif

				(void)set_poisoned(0);
				(void)hp_player(damroll(4, 8));
				ident = TRUE;
				break;
			}
			//v1.1.49 ���Ȃ���H�ׂ�ƕa�C�����邱�Ƃɂ��Ă݂�
			case SV_FOOD_EEL:
			{
				if (shion_comment_food(TRUE, NULL)) break;

				if (p_ptr->pclass == CLASS_BYAKUREN || p_ptr->pclass == CLASS_ICHIRIN)
				{
					msg_print("���Ȃ��͉����ɂ�����H�ׂ��Ȃ��B");
					return FALSE;
				}
				else
				{
					msg_print("��������Ă��Ď��ɔ������I");
					restore_level();
					(void)do_res_stat(A_STR);
					(void)do_res_stat(A_INT);
					(void)do_res_stat(A_WIS);
					(void)do_res_stat(A_CON);
					(void)do_res_stat(A_DEX);
					(void)do_res_stat(A_CHR);
					update_stuff();
				}
			}
			break;

			//�V�l�̒O�@��l�Ɏ푰�ύX
			case SV_FOOD_TENTAN:
			{
				if (RACE_TENTAN_EFFECT)
				{
					msg_print("���Ȃ��͓V�l�̗������ɉ^��...");
					change_race(RACE_SENNIN, "");
					update_stuff();
					if (p_ptr->pclass == CLASS_SHINMYOUMARU || p_ptr->pclass == CLASS_SHINMYOU_2)
					{
						msg_print("���ɂ��Ȃ��͐l�ԃT�C�Y�̑̂���ɓ��ꂽ�I");
					}

				}
				else
				{
					msg_print("...�����N����Ȃ��B�����̕s�������c�q���B");

				}
			}
			break;

			//v1.1.68 ���@�X�C�J
			case SV_FOOD_MAGIC_WATERMELON:
			{

				msg_print("����͂Ȃ�Ƃ��`�e�������������B");
				ident = TRUE;
				break;

			}

			case SV_FOOD_DATURA:
			{
				if (p_ptr->pclass == CLASS_MEDICINE)
				{
					msg_print("���̗t���ς͂ƂĂ����������I");

				}
				else if(!throw_up())
					msg_print("����͓ő��������Ȃ��݂͕̈��Ƃ����Ȃ������B");

			}
			break;
			//v1.1.91
			case SV_FOOD_FORBIDDEN_FRUIT:
			{
				msg_print("�Ô��Ȃ鎀�Ɣw���̖�������B");

				if (do_inc_stat(A_INT));
				set_tim_addstat(A_INT, 110, 100, FALSE);
				set_tim_addstat(A_WIS, 110, 100, FALSE);
				player_gain_mana(999);

				//��ǐ_�l���H�ׂ��猃��ɃJ�I�X���ɂȂ�
				set_deity_bias(DBIAS_COSMOS, -250);
				set_deity_bias(DBIAS_REPUTATION, 100);

			}
			break;

			case SV_FOOD_STRANGE_BEAN:
			{
				msg_print("�s�C���Ȗ��ƐH����...");

				if (no_bad_effect) break;

				if (one_in_(2) && !(p_ptr->resist_pois || IS_OPPOSE_POIS()))
				{
					msg_print("���Ȃ��͋ꂵ���Ȃ��Ă����B");
					take_hit(DAMAGE_NOESCAPE, 25 + randint1(50), "�H������", -1);
					set_poisoned(100);
				}
				else if (one_in_(2) && !p_ptr->sustain_con)
				{
					if (do_dec_stat(A_CON))
						msg_print("���Ȃ��͑̒���������B");
				}
				else if (one_in_(2) && !p_ptr->sustain_str)
				{
					if (do_dec_stat(A_STR))
						msg_print("���Ȃ��͑̒���������B");
				}
				else
				{
					//�ϗe���@�u���̕ϗe�v�Ɠ�������
					gain_physical_mutation();
				}
			}
			break;



		}
	}

	///mod140730 �S�[�����Ɗ�H���ψَ������z�΂�H�ׂ����̏�������
	else if (o_ptr->tval == TV_MATERIAL)
	{

		switch (o_ptr->sval)
		{
			int base;

			//�w�}�^�C�g�A�}�O�l�^�C�g�@�ꎞAC����
			case SV_MATERIAL_HEMATITE:
			case SV_MATERIAL_MAGNETITE:
			{
			
				msg_print("�������̂���v�ɂȂ����C������I");
				base = 50;
				set_shield(randint1(base) + base, FALSE);
			}
			break;
			//��f�z�΁A�ꎞ�őϐ�
			case SV_MATERIAL_ARSENIC:
			{
				msg_print("�ł��̂ɓ���񂾋C������B");
				base = 100;
				set_oppose_pois(randint1(base) + base, FALSE);
			}
			break;
			//�΍��A�ꎞ�J�I�X�ϐ�
			case SV_MATERIAL_ISHIZAKURA:
			{
				msg_print("���Ȃ��͎̑̂׈��ȐF�ʂ�тт��E�E");
				base = 100;
				set_tim_res_chaos(randint1(base) + base, FALSE);
			}
			break;
			//����z�΁@�ꎞ����
			case SV_MATERIAL_MERCURY:
			{
			
				msg_print("�������̂̓��������炩�ɂȂ����C������I");
				base = 50;
				set_fast(randint1(base) + base, FALSE);
			}
			break;
			//�~�X�����@���͕���
			//�X�̗؂�����
			case SV_MATERIAL_MITHRIL:
			case SV_MATERIAL_ICESCALE:
			{
				//v1.1.78 ��ʍX�V���Ȃ��̂ŏC��
				if (player_gain_mana(p_ptr->msp))
					msg_print("�̂ɖ��͂��������I");

			}
			break;
			//�A�_�}���^�C�g�@�ꎞ�S�ϐ�+�Δ�
			case SV_MATERIAL_ADAMANTITE:
			{
				msg_print("���̂������̂���v�ɂȂ����C������I");
				base = 50;
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
				set_shield(randint1(base) + base, FALSE);
			}
			break;
			//��Β�ʁ@�̗͉�+�o���l����

			case SV_MATERIAL_GARNET:
			case SV_MATERIAL_AMETHYST:
			case SV_MATERIAL_AQUAMARINE:
			case SV_MATERIAL_MOONSTONE:
			case SV_MATERIAL_PERIDOT:
			case SV_MATERIAL_OPAL:
			case SV_MATERIAL_TOPAZ:
			case SV_MATERIAL_LAPISLAZULI:
			{
				msg_print("����͎��ɊÂ��Ă��������I�̂Ɋ��͂��߂��Ă����B");
				restore_level();
				hp_player(p_ptr->mhp / 3);
			}
			break;

			//�_�C�������h�@���G
			case SV_MATERIAL_DIAMOND:
			case SV_MATERIAL_HOPE_FRAGMENT:
			{
				msg_print("�̂�ῂ��P�����I");
				base = 10;
				set_invuln(randint1(base) + base, FALSE);
			}
			break;
			//�G�������h�@��p���͏㏸
			case SV_MATERIAL_EMERALD:
			{
				msg_print("�̂���C�i���ɂ��ݏo��C������I");
				do_inc_stat(A_DEX);
				do_inc_stat(A_CHR);
			}
			break;
			//���r�[�@�r�͑ϋv�㏸

			case SV_MATERIAL_RUBY:
			{
				msg_print("�̂̉��ꂩ��͂��N���o�Ă����I");
				do_inc_stat(A_STR);
				do_inc_stat(A_CON);
			}
			break;
			//�T�t�@�C�A �m�\�����㏸
			case SV_MATERIAL_SAPPHIRE:
			{
				msg_print("�������ݓn��悤�ɂ������肵���I");
				do_inc_stat(A_INT);
				do_inc_stat(A_WIS);
			}
			break;

			//覓S�A�~�X�e���E���@�S�\�͈ꎞ�㏸
			case SV_MATERIAL_METEORICIRON:
			case SV_MATERIAL_MYSTERIUM:
			{
				int i;
				msg_print("�����F���I�ȃp���[��������I");
				for(i=0;i<6;i++)set_tim_addstat(i,110,30,FALSE);
			}
			break;
			//�q�q�C���J�l�@���剻+���ɂ̑ϐ�
			//�ɜQ��������
			case SV_MATERIAL_HIHIIROKANE:
			case SV_MATERIAL_IZANAGIOBJECT:
			{
				int v;
				v = randint1(25) + 25;
				msg_print("���Ȃ��͐_�̗͂����̐g�Ɏ�荞�񂾁I");
				set_fast(v, FALSE);
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
				set_mimic(v, MIMIC_GIGANTIC, FALSE);
			}
			break;
			//����@�o���l�㏸
			case SV_MATERIAL_RYUUZYU:
			{

				if (p_ptr->prace == RACE_ANDROID) break;
				chg_virtue(V_ENLIGHTEN, 1);
				if (p_ptr->exp < PY_MAX_EXP)
				{
					s32b ee = (p_ptr->exp / 10) + 10;
					if (ee > 30000L) ee = 30000L;
					msg_print("���Ȃ��͗���������ė͂𓾂��B");

					gain_exp(ee);
					ident = TRUE;
				}
				break;






			}
			case SV_MATERIAL_COAL:
				msg_print("����I");
				break;

			default:
			{
				if(one_in_(3)) msg_print("����͍��΂����Č���������B");
				else if(one_in_(2)) msg_print("�ނށE�E�r�^�[�X�C�[�g�ȑ�l�̖����B");
				else  msg_print("�I�v�𕨌��[�����킢���B");
			}
			break;

			
		}
	}

	else if (o_ptr->tval == TV_SWEETS)
	{
		//v1.1.76
		int yuyuko_get_mana = 0;
		/* Analyze the food */
		switch (o_ptr->sval)
		{
			case SV_SWEET_COOKIE:
			{
				if (shion_comment_food(TRUE, NULL)) break;
				yuyuko_get_mana = 10 + randint1(10);

				msg_print("�T�N�T�N���ĊÂ��Ă��������B");
				ident = TRUE;
				break;
			}
			case SV_SWEET_POTATO:
			{
				if (!shion_comment_food(TRUE, NULL))
					msg_print("����͂��������Ă������B�g���S���g�܂�悤���B");

				if(p_ptr->csp < p_ptr->msp)
				{
					p_ptr->csp += p_ptr->msp / 2;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= (PR_MANA);
				}
				yuyuko_get_mana = p_ptr->msp / 4;

				ident = TRUE;
				break;
			}
			case SV_SWEETS_MIRACLE_FRUIT:
			{
				shion_comment_food(FALSE, NULL);

				if(p_ptr->csp < p_ptr->msp)
				{
					msg_print("�~���N���Ȍ��ʂŖ��͂��񕜂����I");
					p_ptr->csp += p_ptr->msp / 3;
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
					p_ptr->redraw |= (PR_MANA);
				}
				yuyuko_get_mana = p_ptr->msp / 3;

				ident = TRUE;
				break;
			}
			case SV_SWEETS_DANGO:
			{
				//���p���[�A�b�v
				if(p_ptr->pclass == CLASS_RINGO)
				{
					int perc =  p_ptr->chp * 100 / p_ptr->mhp;
					msg_print("MGMG..");
					//�@�J�E���^�J�n
					set_tim_general(20,FALSE,0,0);
					//HP�Čv�Z
					p_ptr->update |= (PU_BONUS);
					handle_stuff();
					p_ptr->chp = p_ptr->mhp * perc / 100;
					p_ptr->redraw |= (PR_HP );
					redraw_stuff();
					//����܂ŐH�ׂ��c�q�̐��J�E���g����
					p_ptr->magic_num1[0] += 1;
					if(p_ptr->magic_num1[0] > 10000) p_ptr->magic_num1[0] = 10000;
				}
				else
				{

					switch(randint1(10))
					{
					case 1:
						msg_print("���΂������Ӗ��c�q���B");break;
					case 2:
						msg_print("�Á`���݂��炵�c�q���B");break;
					case 3:
						msg_print("���X��������̃����M�c�q���B");break;
					case 4:
						msg_print("�Ɠ��̃R�N�̂��邸�񂾒c�q���B");break;
					case 5:
						msg_print("�������̒c�q���B"); break;
					case 6:
						msg_print("䕖��̒c�q���B"); break;
					case 7:
						msg_print("���ȕ����܂Ԃ����c�q���B"); break;
					case 8:
						msg_print("�ċG����̗�₵�c�q���B"); break;
					default:
						msg_print("�Q�̓����������c�q���B");break;
					}
					shion_comment_food(TRUE, NULL);
				}
				yuyuko_get_mana = 50 + randint1(50);
				ident = TRUE;
				break;
			}
			case SV_SWEETS_PEACH:
				msg_print("�e�肽�铍�̍��肪�������ς��ɍL�������B");
				shion_comment_food(TRUE, NULL);
				restore_level();
				(void)set_poisoned(0);
				(void)set_blind(0);
				(void)set_stun(0);
				(void)set_cut(0);
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_DEX);
				set_alcohol(0);
				update_stuff();

				if(one_in_(3))
				{
					int time = 5 + randint1(5);
					set_shield(time,FALSE);
				}
				yuyuko_get_mana = p_ptr->msp;

				break;
			case SV_SWEETS_MANJYU:
			{
				msg_print("�j�����������ȂقǊÂ��Ă����������\�����B");
				shion_comment_food(TRUE, NULL);
				ident = TRUE;
				yuyuko_get_mana = 100 + randint1(100);
				break;
			}

		}

		if (p_ptr->pclass == CLASS_YUYUKO && yuyuko_get_mana > 0)
		{
			msg_print("���Ȃ��͊Ö������\���ď�@�����B");
			player_gain_mana(yuyuko_get_mana);

		}


	}
	//v2.0.6b ������H���n�E�ƂŃG���[�ɂȂ��Ă��܂����̂ŏ������ύX
	else if (p_ptr->pclass == CLASS_YUMA)
	{

		//���ݕ��n��r���ƐH�ׂ��Ƃ��͖򕞗p�����ɓn���B�����ŃA�C�e�����������▞�������Ȃǂ��s���̂ł����ł͏����I������B
		if (o_ptr->tval == TV_POTION || o_ptr->tval == TV_COMPOUND || o_ptr->tval == TV_ALCOHOL || o_ptr->tval == TV_SOFTDRINK)
		{
			do_cmd_quaff_potion_aux(item, TRUE);

			return TRUE;
		}

		//������food_val��0�̂Ƃ��͏�������`�A�C�e���B�H�ׂ��Ɏc��
		food_val = yuma_eat_other_things(o_ptr);
		if (!food_val) return FALSE;

	}
	//����ȊO�A�������H���n�̐E�ƂŖ������H�ׂ��Ƃ��Ȃǂ͂����ł͉������Ȃ�


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214 virtue
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_KNOWLEDGE, -1);
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
	}
	*/
	/* We have tried it */
	if (o_ptr->tval == TV_MUSHROOM) object_tried(o_ptr);

	/* The player is now aware of the object */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	///item race �e�푰�̐H�ƐH�ׂ��Ƃ��̏���
	/* Food can feed the player */
	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
	{
		/* Reduced nutritional benefit */
		(void)set_food(p_ptr->food + (food_val / 10));
#ifdef JP
msg_print("���Ȃ��̂悤�Ȏ҂ɂƂ��ĐH�ƂȂǋ͂��ȉh�{�ɂ����Ȃ�Ȃ��B");
#else
		msg_print("Mere victuals hold scant sustenance for a being such as yourself.");
#endif

		if (p_ptr->food < PY_FOOD_ALERT)   /* Hungry */
#ifdef JP
msg_print("���Ȃ��̋Q���͐V�N�Ȍ��ɂ���Ă̂ݖ��������I");
#else
			msg_print("Your hunger can only be satisfied with fresh blood!");
#endif

	}
	//��̐H�������@�񐔂�1�������炷�@�ޖ��͏񂲂ƐH�ׂ�̂ł����ł͏������Ȃ�
	else if (
		p_ptr->pclass != CLASS_YUMA && 
		(
		//prace_is_(RACE_SKELETON) ||
		  prace_is_(RACE_GOLEM) ||
		 prace_is_(RACE_ANDROID) ||
		 prace_is_(RACE_TSUKUMO) ||
		p_ptr->pclass == CLASS_YOSHIKA && p_ptr->lev > 29
		 )
		 && (o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND))
	{
		cptr staff;

		if (o_ptr->tval == TV_STAFF &&
		    (item < 0) && (o_ptr->number > 1))
		{
#ifdef JP
			msg_print("�܂��͏���E��Ȃ���΁B");
#else
			msg_print("You must first pick up the staffs.");
#endif
			return FALSE;
		}

#ifdef JP
		staff = (o_ptr->tval == TV_STAFF) ? "��" : "���@�_";
#else
		staff = (o_ptr->tval == TV_STAFF) ? "staff" : "wand";
#endif

		/* "Eat" charges */
		if (o_ptr->pval == 0)
		{
#ifdef JP
			msg_format("����%s�ɂ͂������͂��c���Ă��Ȃ��B", staff);
#else
			msg_format("The %s has no charges left.", staff);
#endif

			o_ptr->ident |= (IDENT_EMPTY);

			/* Combine / Reorder the pack (later) */
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			p_ptr->window |= (PW_INVEN);

			return FALSE;
		}

#ifdef JP
		msg_format("���Ȃ���%s�̖��͂��G�l���M�[���Ƃ��ċz�������B", staff);
#else
		msg_format("You absorb mana of the %s as your energy.", staff);
#endif

		/* Use a single charge */
		o_ptr->pval--;

		/* Eat a charge */
		set_food(p_ptr->food + 5000);

		/* XXX Hack -- unstack if necessary */
		if (o_ptr->tval == TV_STAFF &&
		    (item >= 0) && (o_ptr->number > 1))
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
			msg_format("����܂Ƃ߂Ȃ������B");
#else
			msg_print("You unstack your staff.");
#endif
		}

		/* Describe charges in the pack */
		if (item >= 0)
		{
			inven_item_charges(item);
		}

		/* Describe charges on the floor */
		else
		{
			floor_item_charges(0 - item);
		}

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Don't eat a staff/wand itself */
		return TRUE;
	}
	///del131221 item �o�����O�̎��̐H��
#if 0
	else if ((prace_is_(RACE_DEMON) ||
		 (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)) &&
		 (o_ptr->tval == TV_CORPSE && o_ptr->sval == SV_CORPSE &&
		  my_strchr("pht", r_info[o_ptr->pval].d_char)))
	{
		/* Drain vitality of humanoids */
		char o_name[MAX_NLEN];

		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
		msg_format("%s�͔R�����D�ɂȂ����B���͂��z�������C������B", o_name);
#else
		msg_format("%^s is burnt to ashes.  You absorb its vitality!", o_name);
#endif
		(void)set_food(PY_FOOD_MAX - 1);
	}
#endif

#if 0
	///sys item �[���͐H�ׂ����̂�������i�L�m�R�����j
	else if (prace_is_(RACE_SKELETON))
	{
#if 0
		if (o_ptr->tval == TV_SKELETON ||
		    (o_ptr->tval == TV_CORPSE && o_ptr->sval == SV_SKELETON))
		{
#ifdef JP
			msg_print("���Ȃ��͍��Ŏ����̑̂������B");
#else
			msg_print("Your body absorbs the bone.");
#endif
			set_food(p_ptr->food + 5000);
		}
		else 
#endif

		if (o_ptr->tval == TV_FOOD && o_ptr->sval != SV_FOOD_SENTAN)
		{
			object_type forge;
			object_type *q_ptr = &forge;

#ifdef JP
msg_print("�H�ו����A�S��f�ʂ肵�ė������I");
#else
			msg_print("The food falls through your jaws!");
#endif


			/* Create the item */
			object_prep(q_ptr, lookup_kind(o_ptr->tval, o_ptr->sval));

			/* Drop the object from heaven */
			(void)drop_near(q_ptr, -1, py, px);
		}
		else
		{
#ifdef JP
msg_print("�H�ו����A�S��f�ʂ肵�ė����A�������I");
#else
			msg_print("The food falls through your jaws and vanishes!");
#endif

		}
	}
#endif
	else if(o_ptr->tval == TV_MATERIAL)
	{
		(void)set_food(p_ptr->food + food_val);
	}

	else if(p_ptr->pclass == CLASS_KOGASA || p_ptr->pclass == CLASS_DOREMY || p_ptr->pclass == CLASS_HINA)
	{
		msg_print("����ȕ��ł͑S�R�����c��Ȃ��B");
		set_food(p_ptr->food + ((food_val) / 20));
	}
	else if(p_ptr->pclass == CLASS_MEDICINE)
	{
		//v1.1.71 ���f�B�X�����`���E�Z���A�T�K�I��H�ׂ��疞���ɂȂ�
		if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_DATURA)
		{
			/* Waybread is always fully satisfying. */
			set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
		}
		else if (o_ptr->tval != TV_MUSHROOM && o_ptr->sval != SV_MUSHROOM_POISON)
		{
			msg_print("����ȕ��ł͑S�R�����c��Ȃ��B");
			set_food(p_ptr->food + ((food_val) / 50));
		}
	}
	else if (prace_is_(RACE_HANIWA))
	{
		msg_print("���Ȃ��͉h�{��K�v�Ƃ��Ȃ��B");


	}
	else if (prace_is_(RACE_GOLEM) ||
		// prace_is_(RACE_ZOMBIE) ||
		 prace_is_(RACE_ENT) ||
		// prace_is_(RACE_DEMON) ||
		 prace_is_(RACE_ANDROID) ||
		 prace_is_(RACE_MAGIC_JIZO) ||
		 prace_is_(RACE_TSUKUMO) ||
		 prace_is_(RACE_ANIMAL_GHOST) ||
		 // prace_is_(RACE_SPECTRE) ||
		 (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING))
	{

		msg_print("���҂̐H���͂��Ȃ��ɂƂ��ĂقƂ�ǉh�{�ɂȂ�Ȃ��B");

		if(prace_is_(RACE_ANIMAL_GHOST) && p_ptr->food < PY_FOOD_ALERT) 
			msg_print("�G��|���ė͂�D��Ȃ���΁B");

		set_food(p_ptr->food + ((food_val) / 20));
	}


	/*:::�ʏ�̐H��*/
	///item �G���t�̌���
	else if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_SENTAN)
	{
		if (p_ptr->pclass == CLASS_YUMA)
		{
			msg_print("�����ڂ���͐M�����Ȃ��قǂ̉h�{�����I");
			(void)set_food(p_ptr->food + 10000);
		}
		else
		{
			/* Waybread is always fully satisfying. */
			set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
			msg_print("��u�Ŗ����ɂȂ���!");
		}

	}
	//v1.1.43 �I�j�t�X�x�ƃX�[�p�[�L�m�R�͖����ɂȂ�
	else if (o_ptr->tval == TV_MUSHROOM && (o_ptr->sval == SV_MUSHROOM_MON_SUPER || o_ptr->sval == SV_MUSHROOM_PUFFBALL))
	{
		if (p_ptr->pclass == CLASS_YUMA)
		{
			(void)set_food(p_ptr->food + 10000);
		}
		else
		{
			/* Waybread is always fully satisfying. */
			set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
		}
	}

	//v1.1.86 �R���̓L���E����H�ׂĂ������ɂȂ�Ȃ����Ƃɂ���
	else if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_CUCUMBER && (p_ptr->prace == RACE_KAPPA))
	{
		/*:::�͓������イ���H�ׂ�Ɩ����ɂȂ�*/
		set_food(MAX(p_ptr->food, PY_FOOD_MAX - 1));
	}
	else if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_ABURAAGE && (p_ptr->prace == RACE_YOUKO))
	{
		/*:::�d�ς����g����H�ׂ��MP��20%�񕜂���*/
		/*:::��10%�܂Ō��炵��*/
		///mod150504 ����20%
		if(p_ptr->csp < p_ptr->msp)
		{
			if(p_ptr->pclass == CLASS_RAN)
			{
				msg_print("�ނށE�E���̖��g���͍ō��ɔ��������I");
				p_ptr->csp += p_ptr->msp / 5;
			}
			else
			{
				msg_print("�����d�͂��񕜂����C������B");
				p_ptr->csp += p_ptr->msp / 10;
			}
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			/* Redraw mana */
			p_ptr->redraw |= (PR_MANA);
		}
		(void)set_food(p_ptr->food + food_val);
	}
	//��肪���񂲂�H�ׂĂ��H�׉߂��ɂ͂Ȃ�Ȃ�
	else if(p_ptr->pclass == CLASS_RINGO && o_ptr->tval == TV_SWEETS && o_ptr->sval == SV_SWEETS_DANGO)
	{
		int food = MIN(p_ptr->food + food_val,PY_FOOD_MAX - 1);
		if(p_ptr->food < PY_FOOD_MAX) set_food(food);
	}
	else
	{
		/* Food can feed the player */
		(void)set_food(p_ptr->food + food_val);
	}

	/* Destroy a food in the pack */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Destroy a food on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	//���H�́u�G�C�{���̖��̎ԗցv�̖W���ɂȂ�Ȃ�
	break_eibon_flag = FALSE;

	return TRUE;

}


/*
 * Hook to determine if an object is eatable
 */
/*:::�H�ׂ���A�C�e���𔻕ʂ���@�ʏ�̐H���A�ꕔ�푰�̏�A�o�����O�A�����ω��̎���*/
///race item �H�ׂ���A�C�e����hook
///mod131223 �H�ׂ���A�C�e��
static bool item_tester_hook_eatable(object_type *o_ptr)
{

	//v2.0.6 �ޖ��͂�������̂�H�p�\
	if (p_ptr->pclass == CLASS_YUMA) return TRUE;

	if (o_ptr->tval==TV_FOOD) return TRUE;
	if (o_ptr->tval==TV_MUSHROOM) return TRUE;
	if (o_ptr->tval==TV_SWEETS) return TRUE;
	
	/*:::�̂͊[��������H�ׂ邱�Ƃ��ł����炵��*/
#if 0
	if (prace_is_(RACE_SKELETON))
	{
		if (o_ptr->tval == TV_SKELETON ||
		    (o_ptr->tval == TV_CORPSE && o_ptr->sval == SV_SKELETON))
			return TRUE;
	}
	else 
#endif

		//�S�[�����͊��H�ׂ邱�Ƃɂ���
	if(p_ptr->pclass == CLASS_YOSHIKA && p_ptr->lev > 29
		|| prace_is_(RACE_GOLEM) || p_ptr->muta1 & MUT1_EAT_ROCK)
	{
		if(o_ptr->tval == TV_MATERIAL &&(
			o_ptr->sval == SV_MATERIAL_STONE
			|| o_ptr->sval == SV_MATERIAL_HEMATITE
			|| o_ptr->sval == SV_MATERIAL_MAGNETITE
			|| o_ptr->sval == SV_MATERIAL_ARSENIC
			|| o_ptr->sval == SV_MATERIAL_MERCURY
			|| o_ptr->sval == SV_MATERIAL_MITHRIL
			|| o_ptr->sval == SV_MATERIAL_ADAMANTITE
			|| o_ptr->sval == SV_MATERIAL_GARNET
			|| o_ptr->sval == SV_MATERIAL_AMETHYST
			|| o_ptr->sval == SV_MATERIAL_AQUAMARINE
			|| o_ptr->sval == SV_MATERIAL_DIAMOND
			|| o_ptr->sval == SV_MATERIAL_EMERALD
			|| o_ptr->sval == SV_MATERIAL_MOONSTONE
			|| o_ptr->sval == SV_MATERIAL_RUBY
			|| o_ptr->sval == SV_MATERIAL_PERIDOT
			|| o_ptr->sval == SV_MATERIAL_SAPPHIRE
			|| o_ptr->sval == SV_MATERIAL_OPAL
			|| o_ptr->sval == SV_MATERIAL_TOPAZ
			|| o_ptr->sval == SV_MATERIAL_LAPISLAZULI
			|| o_ptr->sval == SV_MATERIAL_METEORICIRON
			|| o_ptr->sval == SV_MATERIAL_HIHIIROKANE
			|| o_ptr->sval == SV_MATERIAL_SCRAP_IRON
			|| o_ptr->sval == SV_MATERIAL_ICESCALE
			|| o_ptr->sval == SV_MATERIAL_HOPE_FRAGMENT
			|| o_ptr->sval == SV_MATERIAL_COAL
			|| o_ptr->sval == SV_MATERIAL_MYSTERIUM
			|| o_ptr->sval == SV_MATERIAL_IZANAGIOBJECT
			|| o_ptr->sval == SV_MATERIAL_ISHIZAKURA
			|| o_ptr->sval == SV_MATERIAL_RYUUZYU
			)) return TRUE;

	}

	if (
		//prace_is_(RACE_SKELETON) ||
		p_ptr->pclass == CLASS_YOSHIKA && p_ptr->lev > 29 || 
		prace_is_(RACE_ANDROID) && p_ptr->pclass != CLASS_MEDICINE ||
	    prace_is_(RACE_TSUKUMO) && p_ptr->pclass != CLASS_KOGASA
	    //prace_is_(RACE_ZOMBIE) ||
	    //prace_is_(RACE_SPECTRE)
		)
	{
		if (o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND)
			return TRUE;
	}
	///del131221 ����
#if 0
	else if (prace_is_(RACE_DEMON) ||
		 (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON))
	{
		if (o_ptr->tval == TV_CORPSE &&
		    o_ptr->sval == SV_CORPSE &&
		    my_strchr("pht", r_info[o_ptr->pval].d_char))
			return TRUE;
	}
#endif

	/* Assume not */
	return (FALSE);
}


/*
 * Eat some food (from the pack or floor)
 */
///item E�R�}���h
void do_cmd_eat_food(void)
{
	int         item;
	cptr        q, s;
	byte mode;

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Restrict choices to food */
	item_tester_hook = item_tester_hook_eatable;

	/* Get an item */
#ifdef JP
	q = "�ǂ��H�ׂ܂���? ";
	s = "�H�ו����Ȃ��B";
#else
	q = "Eat which item? ";
	s = "You have nothing to eat.";
#endif

	mode = (USE_INVEN | USE_FLOOR);

	if (p_ptr->pclass == CLASS_YUMA) mode |= USE_EQUIP;

	if (!get_item(&item, q, s, mode)) return;

	if (item > INVEN_PACK && !get_check_strict("�{���ɑ������Ă�����̂�H�ׂ܂����H", CHECK_NO_ESCAPE)) return;

	/* Eat the object */
	do_cmd_eat_food_aux(item);
}


/*:::�ێ悵���A���R�[�����ǂꂭ�炢p_ptr->alcohol�ɔ��f�����邩�̌v�Z�� �}20%���x�������߂Ď푰�␳*/
///mod151108 ����check�ǉ��@�x���p�̃e�X�g�l�ŏ�ɍő傪�Ԃ�
///v1.1.19 �v�Z���ύX�@�v���C���[HD�ƃ��x���Ő����ɂ����Ȃ�悤�ɂ��A�D���x��l���������Ĉ��������Â炭����
int calc_alcohol_mod(int num, bool check)
{
	int mul = 100 - p_ptr->lev;

	mul -= (p_ptr->hitdie - 15) * 2;

	//�V��ƋS(�Ɖؐ�)���E���o�~�ɐݒ�
	if(p_ptr->prace == RACE_ONI) mul /= 4;
	if(p_ptr->pclass == CLASS_KASEN) mul /= 8; //v1.1.49 4��8
	if(p_ptr->prace == RACE_KARASU_TENGU) mul /= 2;
	if(p_ptr->prace == RACE_HAKUROU_TENGU) mul /= 2;

	if(mul < 1) mul = 1;
	if(mul > 100) mul = 100;

	if(check) 
	{
		return (num * 6 / 5 * mul / 100 );
	}
	else
	{
		return (num * (80 + randint0(41)) / 100 * mul / 100);
	}
}


/*
 * Quaff a potion (from the pack or the floor)
 */
/*:::������ޏ����@��I���ς�*/
///item �������
//v1.1.19 �����K���������ޏ����ɑΉ�
//v2.0.6 �댯�Ȗ�����ގ����m�F�����Ȃ��I�v�V������ǉ��@�ޖ����H���R�}���h�ŕr���ƐH�ׂ�Ƃ�
static void do_cmd_quaff_potion_aux(int item , bool flag_ignore_warning)
{
	int         ident, lev, power;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr;

	bool hoshiguma = FALSE;
	bool hyakuyakumasu = FALSE; //v1.1.49
	bool no_bad_effect = FALSE;

	if(p_ptr->pclass == CLASS_EIRIN) no_bad_effect = TRUE;

	//�ޖ���������ł��|��Ȃ�
	if(p_ptr->pclass == CLASS_YUMA) no_bad_effect = TRUE;

	if(inventory[INVEN_RARM].name1 == ART_HOSHIGUMA || inventory[INVEN_LARM].name1 == ART_HOSHIGUMA) hoshiguma = TRUE;


	/* Take a turn */
	energy_use = 100;
	if(p_ptr->pclass == CLASS_CHEMIST) energy_use -= (p_ptr->lev*2/3); //��t�͖�����ނ̂�����

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("�r����t�̂�����o�Ă��Ȃ��I");
#else
		msg_print("The potion doesn't flow out from a bottle.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
	if (hex_spelling_any())
	{
		if (!hex_spelling(HEX_INHAIL)) stop_hex_spell_all();
	}

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

	//v1.1.49 �ؐ����ۂނƂ��͕S��e���g�����Ƃɂ���(���F�u����������)
	if (p_ptr->pclass == CLASS_KASEN && o_ptr->tval == TV_ALCOHOL && !hoshiguma)
	{
		no_bad_effect = TRUE;
		hyakuyakumasu = TRUE;
	}



	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Single object */
	q_ptr->number = 1;

	if(p_ptr->pclass == CLASS_BYAKUREN && o_ptr->tval == TV_ALCOHOL)
	{
		msg_print("���Ȃ��͉����ɂ��������߂Ȃ��B");
		return;
	}

	if(o_ptr->tval == TV_ALCOHOL && o_ptr->sval == SV_ALCOHOL_MARISA && !hoshiguma && !flag_ignore_warning)
	{
		msg_print("�E�E���ȗ\��������B");
		if (!get_check("�{���Ɉ��݂܂����H")) return;
	}

	if(p_ptr->prace == RACE_LUNARIAN && o_ptr->tval == TV_POTION && o_ptr->sval == SV_POTION_LIFE && !flag_ignore_warning)
	{
		char i;
		msg_print("���̖�͂��Ȃ��ɂƂ��đς���q��ɖ����Ă���B");
		if (!get_check("�{���Ɉ��݂܂����H")) return;
		prt("�m�F�̂��� '@' �������ĉ������B", 0, 0);

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;
	}

	//v1.1.84 �������̓��������@���ꁚ
	//v1.1.98 �A�����������U��̌�͂��̎��̌��ʂ��Ȃ��Ȃ�
	if (o_ptr->name1 == ART_HANGOKU_SAKE && quest[QUEST_HANGOKU2].status == QUEST_STATUS_UNTAKEN)
	{
		if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
		{
			msg_print("���X����Ȃ��̂�����ł��d�����Ȃ��B");
			return;
		}

		else if (p_ptr->prace == RACE_LUNARIAN)
		{
			char i;
			msg_print("���̎��͂��Ȃ��ɂƂ��đς���q��ɖ����Ă���B");
			if (!get_check("�{���Ɉ��݂܂����H")) return;
			prt("�m�F�̂��� '@' �������ĉ������B", 0, 0);

			flush();
			i = inkey();
			prt("", 0, 0);
			if (i != '@') return;
		}
		else if (p_ptr->pclass == CLASS_REIMU)
		{
			msg_print("���̐������ȗ\��������B���̎�������ł͂����Ȃ��B");
			return;
		}
		else if(!flag_ignore_warning)
		{
			msg_print("...���ȗ\��������B");
			if (!get_check("�{���Ɉ��݂܂����H")) return;

		}

	}

	if(o_ptr->tval == TV_COMPOUND && (o_ptr->sval == SV_COMPOUND_YOKOSHIMA || o_ptr->sval == SV_COMPOUND_ICHIKORORI || o_ptr->sval == SV_COMPOUND_HOURAI) && !flag_ignore_warning)
	{
		int i;
		if( !get_check_strict("�{���Ɉ��݂܂���? ", CHECK_NO_HISTORY)) return;
		prt("�m�F�̂��� '@' �������ĉ������B", 0, 0);

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;

	}

	///mod151108 �x���ǉ�
	if((o_ptr->tval == TV_ALCOHOL || o_ptr->tval == TV_FLASK && o_ptr->sval == SV_FLASK_OIL) 
		&& !(p_ptr->muta2 & MUT2_ALCOHOL) && !flag_ignore_warning)
	{
		int tmp = q_ptr->pval;
		if(hoshiguma && tmp < 5000) tmp = 5000;
		tmp = calc_alcohol_mod(tmp,TRUE);

		if((p_ptr->alcohol + tmp) >= DRANK_4 && !process_warning2() ) return;

	}

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

	/* Sound */
	sound(SOUND_QUAFF);


	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[q_ptr->k_idx].level;

	///mod140326 �A���R�[�����ʎ���
	if (q_ptr->tval == TV_ALCOHOL)
	{
		int i;


		if (hoshiguma)
		{
			msg_print("���F�u�Ɏ��𒍂��ň��񂾁B");
		}
		else if (hyakuyakumasu)
		{
			msg_print("�w��؂̕S��e�x�Ɏ��𒍂����B");
		}

		//v1.1.84 ���ꁚ�̏����͕ʂɂ��
		if (q_ptr->name1 == ART_HANGOKU_SAKE&& quest[QUEST_HANGOKU2].status == QUEST_STATUS_UNTAKEN)
		{

			if (p_ptr->prace == RACE_LUNARIAN)
			{
				take_hit(DAMAGE_LOSELIFE, 5000, "������", -1);
			}
			else
			{
				msg_print("�ˑR�A��ʂ̖��͂�D��ꂽ�I");
				process_over_exert(999 - p_ptr->csp);

				gain_random_mutation(217);//�������Ɏ��߂����ψ�

			}
		}
		else 
			switch (q_ptr->sval)
		{
		case SV_ALCOHOL_SUZUME:
			shion_comment_food(TRUE, NULL);

			msg_print("�u����[��@������v");
			msg_print("�u��炽�����@����`��v");
			break;
		case SV_ALCOHOL_1420:
			shion_comment_food(TRUE, NULL);
			msg_print("�u�����I���ꂼ��l��Z���́A�܂������Ȃ��I�v");
			break;
		case SV_ALCOHOL_TUTINOKO:
			if(hoshiguma)
				msg_print("����͒����������I");
			else if(!shion_comment_food(FALSE, NULL))
				msg_print("�ނށA���̖��́I�E�E�������B");
 			break;
		case SV_ALCOHOL_MANZAIRAKU:
			if(hoshiguma)
				msg_print("�ǂ��ɂł�������������������B");
			else if (!shion_comment_food(FALSE, NULL))
				msg_print("�E�E�ǂ��ɂł�����������B");
			break;
		case SV_ALCOHOL_GOLDEN_MEAD:
			msg_print("���_���ǂ��܂ł��L�����Ă����C������I");
			msg_print(NULL);
			wiz_lite(FALSE);
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			(void)detect_treasure(DETECT_RAD_DEFAULT);
			(void)detect_objects_gold(DETECT_RAD_DEFAULT);
			(void)detect_objects_normal(DETECT_RAD_DEFAULT);
			identify_pack();
			set_tim_addstat(A_INT,110,100, FALSE);
			set_tim_addstat(A_WIS,110,100, FALSE);
			(void)set_oppose_acid(100, FALSE);
			(void)set_oppose_elec(100, FALSE);
			(void)set_oppose_fire(100, FALSE);
			(void)set_oppose_cold(100, FALSE);
			(void)set_oppose_pois(100, FALSE);

			ignore_summon_align = TRUE;
			summon_specific(0, py, px, 50, SUMMON_BYAKHEE, PM_FORCE_PET | PM_ALLOW_GROUP);
			ignore_summon_align = FALSE;
			break;
		case SV_ALCOHOL_ORC:
			shion_comment_food(FALSE, NULL);

			set_fast(30, FALSE);
			set_hero(30, FALSE);
			(void)set_afraid(0);

			if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))	(void)set_poisoned(p_ptr->poisoned + 30);
			break;

		case SV_ALCOHOL_REISYU:
			msg_print("��������񂾁B�S�����ݓn��悤���B");
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp += 100;
				if(p_ptr->csp >= p_ptr->msp)
				{
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;
				}

				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);
				ident = TRUE;
			}
			break;

		case SV_ALCOHOL_NECTAR:
			shion_comment_food(TRUE, NULL);
			msg_print("�̒��ɐ����͂��������ӂ�Ă����I");

			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_CHR);
			(void)set_shero(0,TRUE);
			set_alcohol(0);
			update_stuff();
			hp_player(5000);
			ident = TRUE;	
			break;
		case SV_ALCOHOL_MARISA:
			if(hoshiguma)
			{
				msg_print("�E�E�S�R�|���Ȃ��B�u�̗͂ŏ㎿�ɂȂ��Ă���͂��Ȃ̂����B");
			}
			else
			{
				if(no_bad_effect){ident=TRUE;break;}

				msg_print("�g���S������Ă��āA���C�������Ă����悤���B");
				take_hit(DAMAGE_LOSELIFE, damroll(10, 10), "���Ē�", -1);
				(void)dec_stat(A_DEX, 25, TRUE);
				(void)dec_stat(A_WIS, 25, TRUE);
				(void)dec_stat(A_CON, 25, TRUE);
				(void)dec_stat(A_STR, 25, TRUE);
				(void)dec_stat(A_CHR, 25, TRUE);
				(void)dec_stat(A_INT, 25, TRUE);
				ident = TRUE;
			}
				break;

		case SV_ALCOHOL_MOON:
			if(p_ptr->prace == RACE_LUNARIAN || p_ptr->pclass == CLASS_EIRIN || p_ptr->pclass == CLASS_KAGUYA)
			{
				msg_print("����͊ԈႢ�Ȃ����̓s�Ő�N�ȏ�n�������������Î����I");
			}
			else if (!shion_comment_food(TRUE, NULL))
			{
				msg_print("�������ꂷ���ĉ������₵�������B");
			}

			 break;

		case SV_ALCOHOL_MAMUSHI:
			if (!shion_comment_food(FALSE, NULL))
				msg_print("..���͂��Ă����A���C���o�Ă���C������I");
			break;

		default:
			if(hoshiguma)
				msg_print("����͎��Ɏ|�������I");
			else if (!shion_comment_food(FALSE, NULL))
				msg_print("���̎��͂��������B");

			break;
		}
		//�D���x�㏸�@�푰�Ȃǉe���������_������
		power = q_ptr->pval;
		if(hoshiguma && power < 5000) power = 5000; //���F�u�̗͂Ŏ����p���[�A�b�v

		//�S��e�@���₵�̖�Ɠ������ʂɂ���
		if (hyakuyakumasu)
		{
			set_asthma(p_ptr->asthma - 3000);
			hp_player(50);
			set_blind(0);
			set_poisoned(0);
			set_confused(0);
			set_stun(0);
			set_cut(0);
			set_image(0);
		}

		set_alcohol(p_ptr->alcohol + calc_alcohol_mod(power, FALSE)); 
		if(p_ptr->alcohol >= DRANK_1) set_afraid(0);
		if(cheat_xtra) msg_format("alcohol:%d",p_ptr->alcohol);
	}
	else if (q_ptr->tval == TV_FLASK)
	{
		switch (q_ptr->sval)
		{
		case SV_FLASK_OIL:
			if(p_ptr->alcohol >= DRANK_3)
				msg_print("����͎��ɃR�N�������Ĕ������I������ł��ۂ߂������I");
			else
				msg_print("����͖����I���͒K��D�������Ă��܂��Ƃ������Ƃ����Ȃ��͎v���o�����B");

			power = q_ptr->xtra4;
			set_alcohol(p_ptr->alcohol + calc_alcohol_mod(power, FALSE)); 
			if(p_ptr->alcohol >= DRANK_1) set_afraid(0);
			if(cheat_xtra) msg_format("alcohol:%d",p_ptr->alcohol);

			break;
		default:
			msg_print("ERROR:���̃A�C�e�������񂾂Ƃ��̌��ʂ��ݒ肳��Ă��Ȃ�");
			return;

		}
	}
	else if (q_ptr->tval == TV_COMPOUND)
	{
		int time;
		switch (q_ptr->sval)
		{
		case SV_COMPOUND_MUSCLE_DRUG:
			{
				time = 25 + randint1(25);
				msg_print("������ނƗ͂��N���o���Ă����I");
				set_tim_addstat(A_STR,5,time,FALSE);
				set_tim_addstat(A_DEX,5,time,FALSE);
				set_tim_addstat(A_CON,5,time,FALSE);
			}
			break;
		case SV_COMPOUND_BREATH_OF_FIRE:
			{
				int dir;
				msg_print("������ނƍA�̉�����M�����̂����ݏグ�Ă����B");
				(void)get_hack_dir(&dir);
				msg_print("���Ȃ��͉Ή��̃u���X��f�����I");
				fire_ball(GF_FIRE, dir, 250, -2);

			}
			break;
		case SV_COMPOUND_NINGYO:
			{
				if(p_ptr->muta3 & MUT3_FISH_TAIL)
				{
					msg_print("������񂾂��A���Ȃ��͂��łɐl���̑̂��B");
				}
				else
				{
					msg_print("������ނƉ����g�����Y���Y���n�߂��E�E");
					gain_random_mutation(157);//�l���ψ�
				}
			}
			break;
		case SV_COMPOUND_JEKYLL:
			{
				int percentage;
				time = 50 + randint1(50);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_mimic(time, MIMIC_DEMON, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_HAPPY:
			{
				msg_print("���Ȃ��͎���ɍK����𓊗^�����B");
				set_hero(100,FALSE);
				if(no_bad_effect){ident=TRUE;break;}

				if(!p_ptr->resist_chaos) 
					set_image(p_ptr->image + 50);
				if(!p_ptr->resist_conf) 
					set_confused(p_ptr->confused + 50);

			}
			break;

		case SV_COMPOUND_GELSEMIUM_TEA:
			{
				int dir;
				(void)get_hack_dir(&dir);
				msg_print("�u�ԁ[�I�ғŁI�v");
				fire_ball(GF_POIS, dir, 800, -2);
			}
			break;
		case SV_COMPOUND_NIGHTMARE:
			if(no_bad_effect){ident=TRUE;break;}
			if (!p_ptr->free_act)
			{
				msg_print("���Ȃ��͋��낵�������������E�E");
				get_mon_num_prep(get_nightmare, NULL);
				have_nightmare(get_mon_num(MAX_DEPTH));
				get_mon_num_prep(NULL, NULL);
				set_paralyzed(p_ptr->paralyzed + randint1(16) + 16);
				if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print("�����X�b�L�������I�ł������Y��Ă���悤�ȁB");
			}
			break;
		case SV_COMPOUND_NECOMIMI:
			{
				if(p_ptr->muta1 & MUT1_BEAST_EAR)
				{
					msg_print("������񂾂��A���Ȃ��͂��łɏb�����B");
				}
				else
				{
					msg_print("������ނƓ������Y���Y���n�߂��E�E");
					gain_random_mutation(24);//�b���ψ�
				}
			}
			break;
			//���m���o�̖�@���ǂ񂰓��Z���[�`������R�s�[
		case SV_COMPOUND_KOKUSHI:
			{
				time = 25 + randint1(25);

				msg_format("��𕞗p�����B");
				if(!p_ptr->hero)
				{
					set_afraid(0);
					set_hero(time, FALSE);
				}
				else if(!p_ptr->shield)
				{
					set_shield(time, FALSE);
				}
				else if(!p_ptr->tim_addstat_count[A_STR] || p_ptr->tim_addstat_num[A_STR] < 105 ||
					!p_ptr->tim_addstat_count[A_DEX] || p_ptr->tim_addstat_num[A_DEX] < 105 ||
					!p_ptr->tim_addstat_count[A_CON] || p_ptr->tim_addstat_num[A_CON] < 105)
				{
					int percentage = p_ptr->chp * 100 / p_ptr->mhp;
					msg_format("����ȗ͂��N���o���Ă����I");
					set_tim_addstat(A_STR,105,time,FALSE);
					set_tim_addstat(A_DEX,105,time,FALSE);
					set_tim_addstat(A_CON,105,time,FALSE);
					p_ptr->chp = p_ptr->mhp * percentage / 100;
					p_ptr->redraw |= (PR_HP );
					redraw_stuff();

				}
				else 
				{
					int dam = p_ptr->mhp / 2;
					if(dam<1) dam=1;

					if(no_bad_effect)
					{
						msg_print("����ȏ�͊댯���B����ł͂����Ȃ��B");
						return;
					}

					msg_format("���Ȃ��͑唚�������I");
					project(0, 7, py, px, dam * 5 + randint1(dam*5), GF_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
					take_hit(DAMAGE_LOSELIFE, dam, "���m���o�̖�̈��݉߂�", -1);
					set_hero(0,TRUE);
					set_shield(0,TRUE);
					set_tim_addstat(A_STR,0,0,TRUE);
					set_tim_addstat(A_DEX,0,0,TRUE);
					set_tim_addstat(A_CON,0,0,TRUE);
				}
			}
			break;
		case SV_COMPOUND_METAMOR_BEAST:
			{
				int percentage;
				time = 25 + randint1(25);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				if(percentage < 50) percentage = 50;
				dispel_player();
				set_mimic(time, MIMIC_BEAST, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_YOKOSHIMA:
			{
				if(p_ptr->resist_time || IS_INVULN() || no_bad_effect)
				{
					world_monster = -1;
					do_cmd_redraw();
					Term_xtra(TERM_XTRA_DELAY, 3000);
					world_monster = 0;
					p_ptr->redraw |= (PR_MAP);
					handle_stuff();

					msg_print("..���Ȃ��͉ߋ��ւ̗����疳���A�҂����B");
				}
				else
				{
					p_ptr->playing = FALSE;
					p_ptr->is_dead = TRUE;
					p_ptr->leaving = TRUE;
					strcpy(p_ptr->died_from, "lost");
				}
			}
			break;
		case SV_COMPOUND_TITAN:
			{
				int percentage;
				time = 50 + randint1(50);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				set_mimic(time, MIMIC_GIGANTIC, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_ICHIKORORI:
			if(no_bad_effect)
			{
				if (p_ptr->pclass == CLASS_YUMA)
				{
					msg_print("���Ȃ��͌��ł�����Ȃ����݊����ĉh�{�ɂ����I");
					set_tim_addstat(A_CON, 110, 20 + randint1(20), FALSE);
					ident = TRUE;
				}
				else
				{
					msg_print("���Ȃ��͂���ł̂Ƃ���Ŏ����������������Ƃ����̂��C�t���Ď̂Ă��B");
					ident = TRUE;
					break;
				}
			}

			else
			{
				msg_print("���Ȃ��͈���ł͂����Ȃ���������������ł��܂����I");
				if(!IS_INVULN())
				{
					take_hit(DAMAGE_LOSELIFE,9999,"���f�\�͂̒ቺ",-1);
				}
			}
			break;
		case SV_COMPOUND_SUPER_SPEED:
			{
				time = 5 + randint1(5);
				set_lightspeed(time,FALSE);
			}
			break;
		case SV_COMPOUND_METAMOR_DRAGON:
			{
				int percentage;
				time = 50 + randint1(50);
				p_ptr->mimic_dragon_rank = 120+randint1(30);

				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				if(percentage < 50) percentage = 50;
				dispel_player();
				set_mimic(time, MIMIC_DRAGON, FALSE);

				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
			break;
		case SV_COMPOUND_HOURAI:
			{
				//�����I�ȃA�C�e�����p���ǉ��̂��ߓ���Ă���
				if(p_ptr->prace == RACE_HOURAI)
				{
					msg_print("���Ȃ��͂��łɖH���l���B");
					return;
				}

				if (last_words)
				{
					char buf[1024] = "";

					do
					{
						while (!get_string("*����*���b�Z�[�W: ", buf, sizeof buf)) ;
					}
					while (!get_check_strict("��낵���ł����H", CHECK_NO_HISTORY));
					if (buf[0])
					{
						p_ptr->last_message = string_make(buf);
						msg_print(p_ptr->last_message);
					}
				}
				//�푰�ϗe�����@���ޒ��O�Ȃ̂ŃA���h���C�h���낤�Ɨ�O�Ȃ�
				if (p_ptr->prace < 32)
					p_ptr->old_race1 |= 1L << p_ptr->prace;
				else
					p_ptr->old_race2 |= 1L << (p_ptr->prace-32);
				p_ptr->prace = RACE_HOURAI;
				rp_ptr = &race_info[p_ptr->prace];
				gain_perma_mutation();

				p_ptr->redraw |= (PR_BASIC);
				p_ptr->update |= (PU_BONUS);
				handle_stuff();

				p_ptr->total_winner = TRUE;
				finish_the_game = TRUE;
				p_ptr->playing = FALSE;
				p_ptr->is_dead = TRUE;
				p_ptr->leaving = TRUE;
				strcpy(p_ptr->died_from, "hourai");
			}
			break;


		default:
			if (p_ptr->pclass == CLASS_YUMA)
			{
				msg_print("����͈��ނ��̂���Ȃ�...�����Ȃ��͋C�ɂ������񂾁B");
			}
			else
			{
				msg_print("����͈��ނ��̂���Ȃ��B");
				return;

			}
		}

	}
	else if (q_ptr->tval == TV_SOFTDRINK)
	{
		switch (q_ptr->sval)
		{
		case SV_DRINK_WATER:
			msg_print("�ꑧ�����B");
			//v1.1.78 �������񂾂�D���x�ቺ
			set_alcohol(p_ptr->alcohol - 500);
			break;
		case SV_DRINK_APPLE_JUICE:
		case SV_DRINK_GRAPE_JUICE:
			msg_print("�Â��Ă��������B");
			//v1.1.78
			set_alcohol(p_ptr->alcohol - 2000);
			break;
		case SV_DRINK_SLIME_MOLD:
			msg_print("�Ȃ�Ƃ��s�C���Ȗ����B");
			//v1.1.78
			set_alcohol(p_ptr->alcohol - 10000);
			break;
		case SV_DRINK_SALT_WATER:

#ifdef JP
			msg_print("�����I�v�킸�f���Ă��܂����B");
#else
			msg_print("The potion makes you vomit!");
#endif
			if(no_bad_effect){ident=TRUE;break;}

			///race item �����̖�
			if (!(prace_is_(RACE_GOLEM) ||
			      //prace_is_(RACE_ZOMBIE) ||
			    //  prace_is_(RACE_DEMON) ||
			      prace_is_(RACE_ANDROID) ||
			      prace_is_(RACE_MAGIC_JIZO) ||
			      prace_is_(RACE_TSUKUMO) ||
				prace_is_(RACE_HANIWA) ||
				prace_is_(RACE_ANIMAL_GHOST) ||
				//prace_is_(RACE_SPECTRE) ||
			      (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)))
				{
				/* Only living creatures get thirsty */
				(void)set_food(PY_FOOD_STARVE - 1);
				}

			(void)set_poisoned(0);
			(void)set_paralyzed(p_ptr->paralyzed + 4);
			ident = TRUE;
			break;

		}


	}

	/* Analyze the potion */
	else if (q_ptr->tval == TV_POTION)
	{
		if(no_bad_effect)ident=TRUE;

		switch (q_ptr->sval)
		{

		case SV_POTION_SLOWNESS:
			if(no_bad_effect){ident=TRUE;break;}
			if (set_slow(randint1(25) + 15, FALSE)) ident = TRUE;
			break;


		case SV_POTION_POISON:
			if(no_bad_effect){ident=TRUE;break;}
			if(p_ptr->pclass == CLASS_MEDICINE)
			{
				int damage = damroll(4,8);
				ident = TRUE;
				hp_player(damage);
				set_food(p_ptr->food + 5000);
				break;
			}
			else if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()))
			{
				if (set_poisoned(p_ptr->poisoned + randint0(15) + 10))
				{
					ident = TRUE;
				}
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 5000);
			}
			break;
			///mod140907 �ғł̖�@���f�B�X�������ނ�HP,MP,�����x��
		case SV_POTION_POISON2:
			{
				int damage = damroll(20,20);
				if(no_bad_effect){ident=TRUE;break;}

				if(p_ptr->pclass == CLASS_MEDICINE)
				{
					ident = TRUE;
					hp_player(damage);
					if (p_ptr->csp < p_ptr->msp)
					{
						p_ptr->csp += damage;
						if(p_ptr->csp >= p_ptr->msp)
						{
							p_ptr->csp = p_ptr->msp;
							p_ptr->csp_frac = 0;
						}
						msg_print("�����n�b�L���Ƃ����B");

						p_ptr->redraw |= (PR_MANA);
						p_ptr->window |= (PW_PLAYER);
						p_ptr->window |= (PW_SPELL);
					}
					set_food(PY_FOOD_MAX - 1);
					break;
				}
				else if (p_ptr->resist_pois && IS_OPPOSE_POIS())	damage /= 9;
				else if (p_ptr->resist_pois || IS_OPPOSE_POIS()) damage /= 3;
				else set_poisoned(p_ptr->poisoned + randint0(15) + 10);

				ident = TRUE;
				take_hit(DAMAGE_NOESCAPE, damage, "�ғł̖�", -1);
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 5000);
			}
			break;
		case SV_POTION_BLINDNESS:
			if(no_bad_effect){ident=TRUE;break;}
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + randint0(100) + 100))
				{
					ident = TRUE;
				}
			}
			break;
		case SV_POTION_LOVE: //�����
			{
				msg_print("���Ȃ��͖����̃J���X�}�ɖ�����ꂽ�I");
				set_tim_addstat(A_CHR,120,30,FALSE);
				charm_monsters(200);
			}
			break;

		case SV_POTION_SLEEP:
			if(no_bad_effect){ident=TRUE;break;}
			if (!p_ptr->free_act)
			{
#ifdef JP
		msg_print("���Ȃ��͖����Ă��܂����B");
#else
		msg_print("You fall asleep.");
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
				if (set_paralyzed(p_ptr->paralyzed + randint0(4) + 4))
				{
					ident = TRUE;
				}
			}
			break;

		case SV_POTION_LOSE_MEMORIES:
			///mod131228 �����͈ێ��폜
			//if (!p_ptr->hold_life && (p_ptr->exp > 0))
			if(no_bad_effect){ident=TRUE;break;}

			if (p_ptr->exp > 0)
			{
#ifdef JP
				msg_print("�ߋ��̋L��������Ă����C������B");
#else
				msg_print("You feel your memories fade.");
#endif
				chg_virtue(V_KNOWLEDGE, -5);

				lose_exp(p_ptr->exp / 4);
				ident = TRUE;
			}
			break;

		case SV_POTION_RUINATION:

			if (p_ptr->pclass == CLASS_YUMA)
			{
				msg_print("���Ȃ��͌��ł�����Ȃ����݊����ĉh�{�ɂ����I");
				set_tim_addstat(A_STR, 105, 20 + randint1(20), FALSE);
				set_tim_addstat(A_INT, 105, 20 + randint1(20), FALSE);
				set_tim_addstat(A_WIS, 105, 20 + randint1(20), FALSE);
				set_tim_addstat(A_CON, 105, 20 + randint1(20), FALSE);
				set_tim_addstat(A_DEX, 105, 20 + randint1(20), FALSE);
				set_tim_addstat(A_CHR, 105, 20 + randint1(20), FALSE);
				ident = TRUE;
				break;
			}

			if(no_bad_effect){ident=TRUE;break;}

#ifdef JP
			msg_print("�g���S������Ă��āA���C�������Ă����悤���B");
			take_hit(DAMAGE_LOSELIFE, damroll(10, 10), "�j�ł̖�", -1);
#else
			msg_print("Your nerves and muscles feel weak and lifeless!");
			take_hit(DAMAGE_LOSELIFE, damroll(10, 10), "a potion of Ruination", -1);
#endif

			(void)dec_stat(A_DEX, 25, TRUE);
			(void)dec_stat(A_WIS, 25, TRUE);
			(void)dec_stat(A_CON, 25, TRUE);
			(void)dec_stat(A_STR, 25, TRUE);
			(void)dec_stat(A_CHR, 25, TRUE);
			(void)dec_stat(A_INT, 25, TRUE);
			ident = TRUE;
			break;

		case SV_POTION_DEC_STR:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_STR)) ident = TRUE;
			break;

		case SV_POTION_DEC_INT:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_INT)) ident = TRUE;
			break;

		case SV_POTION_DEC_WIS:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_WIS)) ident = TRUE;
			break;

		case SV_POTION_DEC_DEX:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_DEX)) ident = TRUE;
			break;

		case SV_POTION_DEC_CON:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_CON)) ident = TRUE;
			break;

		case SV_POTION_DEC_CHR:
			if(no_bad_effect){ident=TRUE;break;}
			if (do_dec_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_DETONATIONS:
			if (p_ptr->pclass == CLASS_YUMA)
			{
				msg_print("���̒��ŉ����������������A���Ȃ��͔�������ۂݍ��񂾁I");
				set_tim_addstat(A_STR, 110, 20 + randint1(20), FALSE);
				set_tim_addstat(A_DEX, 110, 20 + randint1(20), FALSE);
				ident = TRUE;
				break;
			}

			if(no_bad_effect)
			{
				msg_print("�y���b�A���̖���..�j�g���O���Z�����I");
				ident=TRUE;
				break;
			}
			else if(check_activated_nameless_arts(JKF1_EXPLOSION_DEF))
			{
				msg_print("���̒��ŉ����������������A���Ȃ��͉���f���������ōς񂾁B");
				ident=TRUE;
				break;
			}
#ifdef JP
			msg_print("�̂̒��Ō������������N�����I");
			take_hit(DAMAGE_NOESCAPE, damroll(50, 20), "�����̖�", -1);
#else
			msg_print("Massive explosions rupture your body!");
			take_hit(DAMAGE_NOESCAPE, damroll(50, 20), "a potion of Detonation", -1);
#endif

			//v1.1.66�u�j���v��ԂɂȂ�푰�̓��b�Z�[�W����x�o�ĕςȂ̂ŞN�O���������Ȃ����Ƃɂ����B�؂菝5000�ɔ�ׂ�Ό덷�B
			if(!RACE_BREAKABLE)
				(void)set_stun(p_ptr->stun + 75);
			(void)set_cut(p_ptr->cut + 5000);
			ident = TRUE;
			break;

		case SV_POTION_DEATH:
			if (p_ptr->pclass == CLASS_YUMA)
			{
				msg_print("���Ȃ��͌��ł�����Ȃ����݊����ĉh�{�ɂ����I");
				hp_player(5000);
				set_ultimate_res(20 + randint1(20), FALSE);

				ident = TRUE;
				break;
			}

			if(no_bad_effect){ident=TRUE;break;}
			//chg_virtue(V_VITALITY, -1);
			//chg_virtue(V_UNLIFE, 5);
#ifdef JP
			///msg131215 death
			msg_print("���܂����������̒����삯�߂������B");
			//msg_print("���̗\�����̒����삯�߂������B");
			take_hit(DAMAGE_LOSELIFE, 5000, "���̖�", -1);
#else
			msg_print("A feeling of Death flows through your body.");
			take_hit(DAMAGE_LOSELIFE, 5000, "a potion of Death", -1);
#endif

			ident = TRUE;
			break;

		case SV_POTION_INFRAVISION:
			if (set_tim_infra(p_ptr->tim_infra + 100 + randint1(100), FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_DETECT_INVIS:
			if (set_tim_invis(p_ptr->tim_invis + 12 + randint1(12), FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_SLOW_POISON:
			if (set_poisoned(p_ptr->poisoned / 2)) ident = TRUE;
			break;

		case SV_POTION_CURE_POISON:
			if(p_ptr->pclass == CLASS_YAMAME)
			{
				ident = TRUE;
				msg_print("����͓ŏ������I���Ȃ��͋ꂵ���Ȃ��Ă����B");
				take_hit(DAMAGE_NOESCAPE, 50 + randint1(50), "��ł̖�", -1);
			}
			else if(p_ptr->pclass == CLASS_MEDICINE)
			{
				ident = TRUE;
				msg_print("����͓ŏ������I�̂���͂�������I");
				(void)set_food(PY_FOOD_STARVE - 1);
			}
			else if (set_poisoned(0)) ident = TRUE;
			break;

		case SV_POTION_BOLDNESS:
			if (set_afraid(0)) ident = TRUE;
			break;

		case SV_POTION_SPEED:
			if (!p_ptr->fast)
			{
				if (set_fast(randint1(25) + 15, FALSE)) ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5, FALSE);
			}
			break;

		case SV_POTION_RESIST_HEAT:
			if (set_oppose_fire(p_ptr->oppose_fire + randint1(10) + 10, FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_RESIST_COLD:
			if (set_oppose_cold(p_ptr->oppose_cold + randint1(10) + 10, FALSE))
			{
				ident = TRUE;
			}
			break;

		case SV_POTION_HEROISM:
			if (set_afraid(0)) ident = TRUE;
			if (set_hero(p_ptr->hero + randint1(25) + 25, FALSE)) ident = TRUE;
			if (hp_player(10)) ident = TRUE;
			break;

		case SV_POTION_BESERK_STRENGTH:
			if (set_afraid(0)) ident = TRUE;
			if (set_shero(p_ptr->shero + randint1(25) + 25, FALSE)) ident = TRUE;
			if (hp_player(30)) ident = TRUE;
			break;

		case SV_POTION_CURE_LIGHT:
			if (hp_player(damroll(2, 8))) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_cut(p_ptr->cut - 10)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_CURE_SERIOUS:
			if (hp_player(damroll(4, 8))) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_cut((p_ptr->cut / 2) - 50)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_CURE_CRITICAL:
			if (hp_player(damroll(6, 8))) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_HEALING:
			if (hp_player(300)) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_STAR_HEALING:
			if (hp_player(1200)) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_LIFE:
			if(p_ptr->prace == RACE_LUNARIAN)
			{
				msg_print("�̒��ɐ����͂��������ӂ�Ă����I");
				take_hit(DAMAGE_LOSELIFE, 5000, "�����̖�", -1);

			}
			else
			{
				msg_print("�̒��ɐ����͂��������ӂ�Ă����I");

				restore_level();
				(void)set_poisoned(0);
				(void)set_blind(0);
				(void)set_confused(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_DEX);
				(void)do_res_stat(A_WIS);
				(void)do_res_stat(A_INT);
				(void)do_res_stat(A_CHR);
				(void)set_shero(0,TRUE);
				set_alcohol(0);
				update_stuff();
				hp_player(5000);
				set_asthma(0);
				ident = TRUE;


			}
			break;





		case SV_POTION_RESTORE_MANA:
			///class�@������p�m�̖��͉�
			if (p_ptr->pclass == CLASS_MAGIC_EATER)
			{
				int i;
				for (i = 0; i < EATER_EXT*2; i++)
				{
					p_ptr->magic_num1[i] += (p_ptr->magic_num2[i] < 10) ? EATER_CHARGE * 3 : p_ptr->magic_num2[i]*EATER_CHARGE/3;
					if (p_ptr->magic_num1[i] > p_ptr->magic_num2[i]*EATER_CHARGE) p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
				}
				for (; i < EATER_EXT*3; i++)
				{
					int k_idx = lookup_kind(TV_ROD, i-EATER_EXT*2);
					p_ptr->magic_num1[i] -= ((p_ptr->magic_num2[i] < 10) ? EATER_ROD_CHARGE*3 : p_ptr->magic_num2[i]*EATER_ROD_CHARGE/3)*k_info[k_idx].pval;
					if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;
				}
#ifdef JP
				msg_print("�����n�b�L���Ƃ����B");
#else
				msg_print("You feel your head clear.");
#endif
				p_ptr->window |= (PW_PLAYER);
				ident = TRUE;
			}
			//v2.0.7 �疒��������p�t�̂悤��MP�łȂ��A�r���e�B�J�[�h���񕜂���
			else if (p_ptr->pclass == CLASS_CHIMATA)
			{
				int i;
				bool flag_recharge_sth = FALSE;
				int card_rank = (CHIMATA_CARD_RANK);//�J�[�h���ʃ����N�����������ɓ�����
				for (i = 0; i < ABILITY_CARD_LIST_LEN; i++)
				{
					int new_charge;
					if (!p_ptr->magic_num1[i]) continue;//magic_num1[ability_card_idx]�Ƀ`���[�W�l���L�^���Ă���

					new_charge = p_ptr->magic_num1[i] - (card_rank * ability_card_list[i].charge_turn + 2) / 3;
					if (new_charge < 0) new_charge = 0;

					p_ptr->magic_num1[i] = new_charge;
					flag_recharge_sth = TRUE;
				}
				ident = TRUE;
				if (flag_recharge_sth) msg_print("�A�r���e�B�J�[�h�̗͂�������x�[�U���ꂽ�B");

			}
			else if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
#ifdef JP
				msg_print("�����n�b�L���Ƃ����B");
#else
				msg_print("You feel your head clear.");
#endif

				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);
				ident = TRUE;
			}
			if (set_shero(0,TRUE)) ident = TRUE;
			break;

		case SV_POTION_RESTORE_EXP:
			if (restore_level()) ident = TRUE;
			break;

		case SV_POTION_RES_STR:
			if (do_res_stat(A_STR)) ident = TRUE;
			break;

		case SV_POTION_RES_INT:
			if (do_res_stat(A_INT)) ident = TRUE;
			break;

		case SV_POTION_RES_WIS:
			if (do_res_stat(A_WIS)) ident = TRUE;
			break;

		case SV_POTION_RES_DEX:
			if (do_res_stat(A_DEX)) ident = TRUE;
			break;

		case SV_POTION_RES_CON:
			if (do_res_stat(A_CON)) ident = TRUE;
			break;

		case SV_POTION_RES_CHR:
			if (do_res_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_INC_STR:
			if (do_inc_stat(A_STR)) ident = TRUE;
			break;

		case SV_POTION_INC_INT:
			if (do_inc_stat(A_INT)) ident = TRUE;
			break;

		case SV_POTION_INC_WIS:
			if (do_inc_stat(A_WIS)) ident = TRUE;
			break;

		case SV_POTION_INC_DEX:
			if (do_inc_stat(A_DEX)) ident = TRUE;
			break;

		case SV_POTION_INC_CON:
			if (do_inc_stat(A_CON)) ident = TRUE;
			break;

		case SV_POTION_INC_CHR:
			if (do_inc_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_AUGMENTATION:
			if (do_inc_stat(A_STR)) ident = TRUE;
			if (do_inc_stat(A_INT)) ident = TRUE;
			if (do_inc_stat(A_WIS)) ident = TRUE;
			if (do_inc_stat(A_DEX)) ident = TRUE;
			if (do_inc_stat(A_CON)) ident = TRUE;
			if (do_inc_stat(A_CHR)) ident = TRUE;
			break;

		case SV_POTION_ENLIGHTENMENT:
#ifdef JP
			msg_print("�����̒u����Ă���󋵂��]���ɕ�����ł���...");
#else
			msg_print("An image of your surroundings forms in your mind...");
#endif
			///del131214 virtue
			//chg_virtue(V_KNOWLEDGE, 1);
			//chg_virtue(V_ENLIGHTEN, 1);
			wiz_lite(FALSE);
			ident = TRUE;
			break;

		case SV_POTION_STAR_ENLIGHTENMENT:
#ifdef JP
			msg_print("�X�Ȃ�[�ւ�������...");
#else
			msg_print("You begin to feel more enlightened...");
#endif
			///del131214 virtue
			//chg_virtue(V_KNOWLEDGE, 1);
			//chg_virtue(V_ENLIGHTEN, 2);
			msg_print(NULL);
			wiz_lite(FALSE);
			(void)do_inc_stat(A_INT);
			(void)do_inc_stat(A_WIS);
			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			(void)detect_treasure(DETECT_RAD_DEFAULT);
			(void)detect_objects_gold(DETECT_RAD_DEFAULT);
			(void)detect_objects_normal(DETECT_RAD_DEFAULT);
			identify_pack();
			self_knowledge();
			ident = TRUE;
			break;

		case SV_POTION_SELF_KNOWLEDGE:
#ifdef JP
			msg_print("�������g�̂��Ƃ������͕��������C������...");
#else
			msg_print("You begin to know yourself a little better...");
#endif

			msg_print(NULL);
			self_knowledge();
			ident = TRUE;
			break;

		case SV_POTION_EXPERIENCE:
			if (p_ptr->prace == RACE_ANDROID) break;
			chg_virtue(V_ENLIGHTEN, 1);
			if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
				msg_print("�X�Ɍo����ς񂾂悤�ȋC������B");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(ee);
				ident = TRUE;
			}
			break;

		case SV_POTION_RESISTANCE:
			(void)set_oppose_acid(p_ptr->oppose_acid + randint1(20) + 20, FALSE);
			(void)set_oppose_elec(p_ptr->oppose_elec + randint1(20) + 20, FALSE);
			(void)set_oppose_fire(p_ptr->oppose_fire + randint1(20) + 20, FALSE);
			(void)set_oppose_cold(p_ptr->oppose_cold + randint1(20) + 20, FALSE);
			(void)set_oppose_pois(p_ptr->oppose_pois + randint1(20) + 20, FALSE);
			ident = TRUE;
			break;

		case SV_POTION_CURING:
			set_asthma(p_ptr->asthma - 3000);
			if (hp_player(50)) ident = TRUE;
			if (set_blind(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_image(0)) ident = TRUE;
			//v1.1.78
			set_alcohol(p_ptr->alcohol - 3000);
		
			break;

		case SV_POTION_INVULNERABILITY:
			msg_print("���Ȃ��͉��S�ʂ���̖�����̌������I");
			(void)set_invuln(p_ptr->invuln + randint1(4) + 4, FALSE);
			ident = TRUE;
			break;

			/*:::item �V���̖�*/
		case SV_POTION_NEW_LIFE:
			set_deity_bias(DBIAS_REPUTATION, 5);
			do_cmd_rerate(FALSE);
			get_max_stats();
			p_ptr->update |= PU_BONUS;
			p_ptr->bydo = 0;
			set_alcohol(0);
			if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
			{
				///del131214 virtue
				//chg_virtue(V_CHANCE, -5);
				if(muta_erasable_count()) msg_print("�S�Ă̓ˑR�ψق��������B");

				p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
				///mod140324 �V���̖�ł������^�ψق͏����Ȃ�����
				p_ptr->muta1 = p_ptr->muta1_perma;
				p_ptr->muta2 = p_ptr->muta2_perma;
				p_ptr->muta3 = p_ptr->muta3_perma;
				p_ptr->muta4 = p_ptr->muta4_perma;
				p_ptr->update |= PU_BONUS;
				handle_stuff();
				mutant_regenerate_mod = calc_mutant_regenerate_mod();
			}
			ident = TRUE;
			break;

		case SV_POTION_NEO_TSUYOSHI:
			if(no_bad_effect)
			{
				//msg_print("�F�X�Ƃ�����ۂ��̂������Ă���悤���B"); ������̓l�I�悵�łȂ������}�T���_�C�i�~�b�N������
				ident=TRUE;
				break;
			}
			set_deity_bias(DBIAS_COSMOS, -1);

			(void)set_image(0);
			(void)set_tsuyoshi(p_ptr->tsuyoshi + randint1(100) + 100, FALSE);
			ident = TRUE;
			break;
			///item �悵�X�y�V����
		case SV_POTION_TSUYOSHI:
			if(no_bad_effect){ident=TRUE;break;}
			set_deity_bias(DBIAS_COSMOS, -1);
#ifdef JP
msg_print("�u�I�N���Z����I�v");
#else
			msg_print("Brother OKURE!");
#endif
			msg_print(NULL);
			p_ptr->tsuyoshi = 1;
			(void)set_tsuyoshi(0, TRUE);
			if (!p_ptr->resist_chaos)
			{
				(void)set_image(50 + randint1(50));
			}
			ident = TRUE;
			break;

		case SV_POTION_NANIKASUZUSHIKUNARU:

			if (p_ptr->pclass == CLASS_YUMA)
			{
				msg_print("���Ȃ��͌��ł�����Ȃ����݊����ĉh�{�ɂ����I");
				set_tim_addstat(A_INT, 110, 20 + randint1(20), FALSE);
				set_tim_addstat(A_WIS, 110, 20 + randint1(20), FALSE);
				ident = TRUE;
				break;
			}


			if(no_bad_effect){ident=TRUE;break;}
			if(p_ptr->immune_cold) break;
			msg_print("�̂̒����畨�����₦���I");
			take_hit(DAMAGE_NOESCAPE, mod_cold_dam(1600), "�����������Ȃ��", -1);
			break;

		case SV_POTION_POLYMORPH:
			if ((p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4) && one_in_(23))
			{
				///del131214 virtue
				//chg_virtue(V_CHANCE, -5);
				if(muta_erasable_count()) msg_print("�S�Ă̓ˑR�ψق��������B");

				p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
				///mod141204 �i���ψٓK�p
				p_ptr->muta1 = p_ptr->muta1_perma;
				p_ptr->muta2 = p_ptr->muta2_perma;
				p_ptr->muta3 = p_ptr->muta3_perma;
				p_ptr->muta4 = p_ptr->muta4_perma;
				p_ptr->update |= PU_BONUS;
				handle_stuff();
			}
			else
			{
				do
				{
					if (one_in_(2))
					{
						if(gain_random_mutation(0)) ident = TRUE;
					}
					else if (lose_mutation(0)) ident = TRUE;
				} while(!ident || one_in_(2));
			}
			break;
		}
	}
#if 0
	if (prace_is_(RACE_SKELETON))
	{
#ifdef JP
msg_print("�t�̂̈ꕔ�͂��Ȃ��̃A�S��f�ʂ肵�ė������I");
#else
		msg_print("Some of the fluid falls through your jaws!");
#endif

		(void)potion_smash_effect(0, py, px, q_ptr->k_idx);
	}
#endif
	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214 virtue
	/*
	if (!(object_is_aware(q_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/
	/* The item has been tried */
	object_tried(q_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(q_ptr))
	{
		object_aware(q_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		set_deity_bias(DBIAS_COSMOS, -2);
	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Potions can feed the player */
	///race ������񂾎��̖����x��������
	///mod140326 �������ނƎ푰�ɂ�����炸pval/3�Ԃ�̐H��
	//v1.1.19 ���K�����ǉ�
	if(q_ptr->tval == TV_ALCOHOL) (void)set_food(p_ptr->food + q_ptr->pval / 3);
	else if(q_ptr->tval == TV_FLASK)
	{
		switch(q_ptr->sval)
		{
		case SV_FLASK_OIL:
			(void)set_food(p_ptr->food + 3000);
			break;
		default:
			msg_print("ERROR:���̃A�C�e�������񂾂Ƃ��̖����x������������Ă��Ȃ�");
			break;
		}
	}
	else 
	switch (p_ptr->mimic_form)
	{
	case MIMIC_NONE:
		switch (p_ptr->prace)
		{
			case RACE_VAMPIRE:
				(void)set_food(p_ptr->food + (q_ptr->pval / 10));
				break;
			//case RACE_SKELETON:
				/* Do nothing */
			//	break;
			case RACE_GOLEM:
			//case RACE_ZOMBIE:
			case RACE_ANDROID:
			case RACE_TSUKUMO:
			//case RACE_DEMON:
			//case RACE_SPECTRE:
				set_food(p_ptr->food + ((q_ptr->pval) / 20));
				break;
				/*
			case RACE_ANDROID:
				if (q_ptr->tval == TV_FLASK)
				{
#ifdef JP
					msg_print("�I�C����⋋�����B");
#else
					msg_print("You replenish yourself with the oil.");
#endif
					set_food(p_ptr->food + 5000);
				}
				else
				{
					set_food(p_ptr->food + ((q_ptr->pval) / 20));
				}
				*/
				break;
			case RACE_ENT:
#ifdef JP
				msg_print("��������荞�񂾁B");
#else
				msg_print("You are moistened.");
#endif
				set_food(MIN(p_ptr->food + q_ptr->pval + MAX(0, q_ptr->pval * 10) + 2000, PY_FOOD_MAX - 1));
				break;
			default:
				(void)set_food(p_ptr->food + q_ptr->pval);
				break;
		}
		break;
	case MIMIC_DEMON:
	case MIMIC_DEMON_LORD:
		set_food(p_ptr->food + ((q_ptr->pval) / 20));
		break;
	case MIMIC_VAMPIRE:
		(void)set_food(p_ptr->food + (q_ptr->pval / 10));
		break;
	default:
		(void)set_food(p_ptr->food + q_ptr->pval);
		break;
	}
}


/*
 * Hook to determine if an object can be quaffed
 */
/*:::q�R�}���h�ň��߂镨�̔���@�A���h���C�h�Ŗ��ڂ����߂邩�ǂ����ȊO�ӂ�*/
///item ���߂�A�C�e���̔���@DRINKABLE�t���O��ǉ��\��@�A���h���C�h�͍폜
///mod131223 q�R�}���h�@���ݕ��Ǝ�ǉ�
///mod151227 ��t�p������ǉ�
//v1.1.19 ���K�͖������߂�悤�ɂ���
static bool item_tester_hook_quaff(object_type *o_ptr)
{
	if (o_ptr->tval == TV_POTION) return TRUE;
	if (o_ptr->tval == TV_SOFTDRINK) return TRUE;
	if (o_ptr->tval == TV_ALCOHOL) return TRUE;
	//v1.1.19 ���K�͖������߂�悤�ɂ���
	if (prace_is_(RACE_BAKEDANUKI))
	{
		if (o_ptr->tval == TV_FLASK && o_ptr->sval == SV_FLASK_OIL)
			return TRUE;
	}
	if (o_ptr->tval == TV_COMPOUND)//quaff_potion�͌��ʔ����̑O�ɏ����̂ň��߂Ȃ���͐�ɒe���Ă����K�v������
	{
		switch(o_ptr->sval)
		{
		case SV_COMPOUND_MUSCLE_DRUG:
		case SV_COMPOUND_BREATH_OF_FIRE:
		case SV_COMPOUND_NINGYO:
		case SV_COMPOUND_JEKYLL:
		case SV_COMPOUND_HAPPY:
		case SV_COMPOUND_GELSEMIUM_TEA:
		case SV_COMPOUND_NIGHTMARE:
		case SV_COMPOUND_NECOMIMI:
		case SV_COMPOUND_KOKUSHI:
		case SV_COMPOUND_METAMOR_BEAST:
		case SV_COMPOUND_YOKOSHIMA:
		case SV_COMPOUND_TITAN:
		case SV_COMPOUND_ICHIKORORI:
		case SV_COMPOUND_SUPER_SPEED:
		case SV_COMPOUND_METAMOR_DRAGON:
		case SV_COMPOUND_HOURAI:
			return TRUE;
		default:
			return FALSE;
		}
	}

/*
	if (prace_is_(RACE_ANDROID))
	{
		if (o_ptr->tval == TV_FLASK)
			return TRUE;
	}
*/
	return FALSE;
}


/*
 * Quaff some potion (from the pack or floor)
 */
/*:::�������*/
void do_cmd_quaff_potion(void)
{
	int  item;
	cptr q, s;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_QUAFF)
	{
		msg_print("���̎p�ł͖�����߂Ȃ��B");
		return ;
	}


	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Restrict choices to potions */
	item_tester_hook = item_tester_hook_quaff;

	/* Get an item */
#ifdef JP
	///msg131215
	//q = "�ǂ̖�����݂܂���? ";
	//s = "���߂�򂪂Ȃ��B";
	q = "�������݂܂���? ";
	s = "���߂镨�������Ă��Ȃ��B";
#else
	q = "Quaff which potion? ";
	s = "You have no potions to quaff.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Quaff the potion */
	do_cmd_quaff_potion_aux(item,FALSE);

	//���H�́u�G�C�{���̖��̎ԗցv�̖W���ɂȂ�Ȃ�
	break_eibon_flag = FALSE;

}



/*:::�T�O���́u�ǂށv�R�}���h�̌��ʔ���*/
static void sagume_read_scroll_aux(int item, bool known)
{
	int         k, used_up, ident, lev;
	object_type *o_ptr;

	int scroll_k_idx,item2;

	/* Get the item (in the pack) */
	if (item >= 0)	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	energy_use = 100;

	if (world_player)
	{
		if (flush_failure) flush();
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");
		sound(SOUND_FAIL);
		return;
	}

	if (p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		msg_print("�����̕������x���Č�����B���܂��ǂ߂Ȃ��B");
		return;
	}

	if (p_ptr->asthma >= ASTHMA_3)
	{
		if(randint0(p_ptr->asthma) > 6600)
		{
			msg_print("���Ȃ��͊P������ŕ�����ǂނ̂Ɏ��s�����I");
			return;
		}
	}

	//���o_ptr��ݒ肵�������߂Ɏg��
	scroll_k_idx = o_ptr->k_idx;

	ident = FALSE;
	used_up = TRUE;
	lev = k_info[o_ptr->k_idx].level;


	if (o_ptr->tval == TV_SCROLL)
	{
	switch (o_ptr->sval)
	{
		//�ÈŁF���E����U�U���A���E�����f
		case SV_SCROLL_DARKNESS:
		{
			msg_format("�ˑR������ῂ��������I");
			project_hack2(GF_LITE,3,6, 0);
			confuse_monsters(200);
			ident = TRUE;
			break;
		}
		//��:���E���Í��A�ӖځA��������

		case SV_SCROLL_LIGHT:
		{
			object_type *tmp_o_ptr = &inventory[INVEN_LITE];

			msg_format("�ˑR�ӂ�͈łɕ�܂ꂽ�B");
			project_hack2(GF_DARK,3,6,0);

			//��������
			if (tmp_o_ptr->k_idx && (tmp_o_ptr->xtra4 > 0) && (!object_is_fixed_artifact(tmp_o_ptr)))
			{
				//Hack - ������0�ɂ���Ə�������₱�������Ȃ̂�1�ɂ��Ēʏ�̌�����������������0�ɂ���
				tmp_o_ptr->xtra4 = 1;
				p_ptr->window |= (PW_EQUIP);
			}
			ident = TRUE;
			break;
		}

		//�G�L�T�C�g�����X�^�[�F������
		case SV_SCROLL_AGGRAVATE_MONSTER:
		{
			msg_print("�ӂ�̋�C������悤�ɒ���߂��E�E");

			stasis_monsters(1000);
			ident = TRUE;
			break;
		}

		//�h������F*�h���*�Ɠ���ւ���
		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if (curse_armor()) ident = TRUE;
			break;
		}
		case SV_SCROLL_CURSE_ARMOR:
		{
			if (!enchant_spell(0, 0, randint1(10) + 10)) used_up = FALSE;
			ident = TRUE;
			break;
		}
		//��������F*���틭��*�Ɠ���ւ���
		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			k = 0;
			if (buki_motteruka(INVEN_RARM))
			{
				k = INVEN_RARM;
				if (buki_motteruka(INVEN_LARM) && one_in_(2)) k = INVEN_LARM;
			}
			else if (buki_motteruka(INVEN_LARM)) k = INVEN_LARM;
			if (k && curse_weapon(FALSE, k)) ident = TRUE;
			break;
		}
		case SV_SCROLL_CURSE_WEAPON:
		{
			if (!enchant_spell(randint1(10)+10, randint1(10)+10, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		//�T���������X�^�[�F���Ӗ��E�Ɠ���ւ���
		case SV_SCROLL_MASS_GENOCIDE:
		{
			int num = 3 + randint1(3);
			for (k = 0; k < num; k++)
			{
				if (summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}
		case SV_SCROLL_SUMMON_MONSTER:
		{
			(void)mass_genocide(300, TRUE);
			ident = TRUE;
			break;
		}

		//�g���b�v�j��F�g���b�v�n�����h�A����
		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
		{
			if(one_in_(2))
			{
				if (trap_creation(py, px)) ident = TRUE;
			}
			else
			{
				door_creation(1);
				ident = TRUE;
			}
			break;
		}

		//�g���b�v�n���F�L�͈͂̃g���b�v�j��
		case SV_SCROLL_TRAP_CREATION:
		{
			(void)project(0,7, py, px, 0, GF_KILL_TRAP, (PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE), -1);
			ident = TRUE;
			break;
		}

		//�T�����A���f�b�h�F�A���f�b�h�ގU�Ɠ���ւ���
		case SV_SCROLL_DISPEL_UNDEAD:
		{
			int num = 3 + randint1(3);
			for (k = 0; k < num; k++)
			{
				if (summon_specific(0, py, px, dun_level, SUMMON_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			if(ident) msg_print("�s��ȋC�z���ӂ�ɖ������E�E");
			break;
		}

		case SV_SCROLL_SUMMON_UNDEAD:
		{
			if (dispel_undead(300)) ident = TRUE;
			break;
		}

		//�����́@���Ẩ��O1
		case SV_SCROLL_HOLY_CHANT:
		{
			bool stop_ty = FALSE;
			int count = 0;

			msg_print("�������s�J�b�ƌ������I");

			do
			{
				stop_ty = activate_ty_curse(stop_ty, &count);
			}
			while (one_in_(6));
			ident = TRUE;
		}

		//�t���A���Z�b�g�F�V���h�E�V�t�g����
		case SV_SCROLL_RESET_RECALL:
		{
			if (p_ptr->inside_arena || ironman_downward)
			{
				msg_print("��u�ӂ�̌i�F���c�񂾋C�����邪�A�����N����Ȃ������B");
			}
			else
			{
				/*:::0�ł͂܂����͂�*/
				p_ptr->alter_reality = 1;
				p_ptr->redraw |= (PR_STATUS);
				ident = TRUE;
			}
			break;
		}
		//���@�̒n�}�F�n�k
		case SV_SCROLL_MAPPING:
		{
			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level)
			{
				msg_print("��u�n�ʂ��h�ꂽ���A�����N����Ȃ������B");
			}
			else
			{
				msg_print("�˔@�A�_���W�����̓V�䂪���������I");
				earthquake_aux(py,px,12,0);
			}
			ident = TRUE;
			break;
		}
		//�h����F�h��򉻂���
		case SV_SCROLL_ENCHANT_ARMOR:
		{
			int inven_list[6] = {INVEN_RIBBON,INVEN_BODY,INVEN_OUTER,INVEN_HEAD,INVEN_HANDS,INVEN_FEET};
			apply_disenchant(0L,inven_list[randint0(6)]); 
			ident = TRUE;
			break;
		}
		//���틭���F���킪�򉻂���
		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			apply_disenchant(0L,INVEN_RARM); 
			ident = TRUE;
			break;
		}

		//���͏[�U�F���͐H��
		case SV_SCROLL_RECHARGING:
		{
			if (!eat_magic(130)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		//�}�f���������ɂ��Ă݂�i�I�j
		case SV_SCROLL_MUNDANITY:
		{
			ident = TRUE;
			if (!artifact_scroll()) used_up = FALSE;
			break;
		}


		//�l���F���׏���
		case SV_SCROLL_ACQUIREMENT:
		{
			call_chaos();
			ident = TRUE;
			break;
		}
		//*�l��*�F��������
		case SV_SCROLL_STAR_ACQUIREMENT:
		{
			call_the_();
			ident = TRUE;
			break;
		}

		//�~�������F�n���}�Q�h���g���b�v
		case SV_SCROLL_SUMMON_KIN:
		{
			static int levs[10] = {1, 3, 5, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;
			bool flag = FALSE;

			int mon_lev;

			/* Summon Demons and Angels */
			for (mon_lev = MAX(30,dun_level); mon_lev >= 20; mon_lev -= 1 + mon_lev /16)
			{
				int i;
				int num = levs[MIN(mon_lev /10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(px, 7);
					int y1 = rand_spread(py, 5);

					/* Skip illegal grids */
					if (!in_bounds(y1, x1)) continue;

					/* Require line of projection */
					if (!projectable(py, px, y1, x1)) continue;

					if (summon_specific(0, y1, x1, mon_lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if (summon_specific(0, y1, x1, mon_lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
					{
						good_idx = hack_m_idx_ii;
					}

					/* Let them fight each other */
					if (evil_idx && good_idx)
					{
						monster_type *evil_ptr = &m_list[evil_idx];
						monster_type *good_ptr = &m_list[good_idx];
						evil_ptr->target_y = good_ptr->fy;
						evil_ptr->target_x = good_ptr->fx;
						good_ptr->target_y = evil_ptr->fy;
						good_ptr->target_x = evil_ptr->fx;
						flag = TRUE;
					}
				}
			}
			if(flag)
			{
				msg_print("�ˑR�V�E�̐푈�Ɋ������܂ꂽ�I");
				ident = TRUE;
			}
			else
			{
				msg_print("��u�������Ȃ�ʋC�z�����������A�����N����Ȃ������B");
			}
			break;
		}

		//���󊴒m�F��������
		case SV_SCROLL_DETECT_GOLD:
		{
			if(p_ptr->au > 4)
			{
				msg_print("...�����z���y���Ȃ����C������B");
				p_ptr->au -= p_ptr->au / 4;
				p_ptr->redraw |= (PR_GOLD);	
				ident = TRUE;
			}
			break;
		}
		//����@�����i�������
		case SV_SCROLL_REMOVE_CURSE:
		{
			curse_equipment(100, 10);
			ident = TRUE;
			break;
		}

		//*����*�@�S�\�͒l�ቺ
		case SV_SCROLL_STAR_REMOVE_CURSE:
		{
			int dummy;
			msg_print("���|�̈Í��I�[�������Ȃ����ݍ��񂾁E�E");
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), TRUE);
			}
			
			ident = TRUE;
		}

		//�p�j�b�N�����X�^�[�F�����ƌ��o
		case SV_SCROLL_MONSTER_CONFUSION:
		{
			msg_print("�肪�P���n�߂��B�����ċP�����̒��ɍL�����Ă����E�E");
			if(!p_ptr->resist_conf) (void)set_confused(p_ptr->confused + randint0(10) + 10);
			if(!p_ptr->resist_chaos) (void)set_image(p_ptr->image + randint0(100) + 100);
			
			break;
		}

		//�y�b�g�����F�S�Ă̔z�������ł���
		case SV_SCROLL_SUMMON_PET:
		{
			int i;
			bool flag = FALSE;

			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];	
				if (!m_ptr->r_idx || !is_pet(m_ptr)) continue;
				delete_monster_idx(i);
				flag = TRUE;

				if (record_named_pet && m_ptr->nickname)
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
					do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_DISMISS, m_name);
				}
			}
			if(flag)
			{
				msg_print("�z���̋C�z���������E�E");
				ident = TRUE;
			}
			break;
		}

		//�Ӓ�F�A�C�e�������Ӓ�ɖ߂�
		case SV_SCROLL_IDENTIFY:
		{
			if(!ident_spell_2(3)) used_up = FALSE;
			else ident = TRUE;

			break;
		}
		//*�Ӓ�*�@�}�f
		case SV_SCROLL_STAR_IDENTIFY:
		{			
			if (!mundane_spell(FALSE)) used_up = FALSE;
			break;
		}

		//�����̊����@�t���A�̓G��ق点��
		case SV_SCROLL_SPELL:
		{
			msg_print("�_���W�����Ɉَ��Ȗ��͂��������B");
			p_ptr->silent_floor = 1;
			ident = TRUE;
			break;
		}

		//�\�̊����F�������ǂނ킯�ł͂Ȃ��̂ŉ����N����Ȃ�
		case SV_SCROLL_RUMOR:
		{
			msg_print("�����ɂ̓��b�Z�[�W��������Ă���:");

			display_rumor(TRUE);
			msg_print("�����͉��𗧂Ăď����������I");

			ident = TRUE;
			break;
		}
		//*�j��*�@���͂̕ǂ��i�v�ǂɂ��{�X�ȊO�̃����X�^�[�𖰂点��
		case SV_SCROLL_STAR_DESTRUCTION:
		{
			int x, y;
			int r = 13 + randint0(5);

			if ((p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) || !dun_level || EXTRA_QUEST_FLOOR)
			{
				msg_print("�����N����Ȃ������B");
				break;
			}

			for (y = (py - r); y <= (py + r); y++)
			{
				for (x = (px - r); x <= (px + r); x++)
				{
					bool flag_mark = FALSE;
					if (!in_bounds(y, x)) continue;
					if(player_bold(y,x)) continue;
					if(distance(py, px, y, x) > r) continue;

					if(cave[y][x].m_idx)//�����X�^�[������΃{�X�ȊO����
					{
						if(!(r_info[m_list[cave[y][x].m_idx].r_idx].flags1 & RF1_QUESTOR))
							set_monster_csleep(cave[y][x].m_idx, 5000);
						continue;
					}

					if(!cave_have_flag_bold((y), (x), FF_WALL)) continue;
					if(cave_have_flag_bold((y), (x), FF_PERMANENT)) continue;

					if(cave[y][x].info & (CAVE_KNOWN| CAVE_MARK)) flag_mark = TRUE;

					cave_set_feat(y, x, feat_permanent);

					if(flag_mark)
					{
						cave[y][x].info |= (CAVE_KNOWN | CAVE_MARK);
//						lite_spot(y,x);
					}

				}
			}
			msg_print("������悤�ȑM�������������I");
			ident = TRUE;

			break;
		}
		//�A�҂̏فF�ꎞ�e���|�j�Q
		case SV_SCROLL_WORD_OF_RECALL:
		{
			int base = 15;
			msg_print("���̑�C���S���Ă����E�E");
			set_nennbaku(randint1(base) + base, FALSE);
			ident = TRUE;
			break;
		}

		//���̊����F���Ɉ͂܂��
		case SV_SCROLL_FIRE:
		{
			msg_format("�ӂ�͈�u�ɂ��Đ[�����̒�ɂȂ����B");
			project_hack2(GF_WATER_FLOW,0,0,2);
			project_hack2(GF_WATER,0,0,600);

			ident = TRUE;
			break;
		}

		//�X�̊����F�n��Ɉ͂܂��
		case SV_SCROLL_ICE:
		{
			msg_format("�}�ɕӂ肪�M�C�ɕ�܂ꂽ�E�E");
			project_hack2(GF_LAVA_FLOW,0,0,2);
			project_hack2(GF_FIRE,0,0,1200);

			ident = TRUE;
			break;
		}

		//���O���X�̊����F�ꕔ�����������X�^�[���t���A���������
		case SV_SCROLL_CHAOS:
		{
			int i;
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (!m_ptr->r_idx) continue;
				if (i == p_ptr->riding) continue;
				if (r_ptr->flags1 & RF1_QUESTOR)
				{
					if(distance(m_ptr->fy,m_ptr->fx,py,px) <= MAX_SIGHT) teleport_away(i,100,TELEPORT_PASSIVE);
					continue;
				}

				if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
					do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
				}
				/* Delete this monster */
				delete_monster_idx(i);
			}

			msg_print("�}�ɕӂ肪�Â��ɂȂ����B");
			ident = TRUE;
			break;
		}

		//���������t�@�C�i���X�g���C�N�Ƃ��ǂ����낤
		case SV_SCROLL_ARTIFACT:
		{
			if(final_strike(0,TRUE))ident = TRUE;
			else used_up = FALSE;
			break;
		}

		//�e���|���x���F�t���A�̕ǂ�������
		case SV_SCROLL_TELEPORT_LEVEL:
		{
			if(p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) break;
			msg_print("�_���W�����̕ǂ��������������ď��ɒ��ݍ���ł����I");
			vanish_dungeon();
			ident = TRUE;
			break;
		}
		//�e���|�[�g�F���E���̃����_���ȓG�Ƀe���|�[�g�A�E�F�C
		case SV_SCROLL_TELEPORT:
		{
			int i, cnt=0, idx = 0;
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!projectable(py, px, y, x)) continue;
				cnt++;
				if(one_in_(cnt)) idx = i;
			}

			if(idx)
			{
				project(0, 0, m_list[idx].fy, m_list[idx].fx, 300, GF_AWAY_ALL, (PROJECT_JUMP|PROJECT_KILL), -1);
				ident = TRUE;
			}
			break;
		}
		//�V���[�g�e���|�F����
		case SV_SCROLL_PHASE_DOOR:
		{
			msg_print("�}�ɑ̂��d���Ȃ����C������E�E");
			(void)set_slow(randint1(15) + 15, FALSE);		
			ident = TRUE;
			break;
		}

		//�g���b�v���m:�[��������
		case SV_SCROLL_DETECT_TRAP:
		{
			if(!cave_clean_bold(py,px))
			{
				msg_print("�����������h�ꂽ�C�������������N����Ȃ������B");
				break;
			}

			cave_set_feat(py, px, feat_dark_pit);
			msg_print("������ǂނƁA�����ɑ匊���󂢂��I");
			ident = TRUE;

			break;
		}

		//�h�A���m�F�������g���b�v�h�A�ɂȂ�
		//v1.1.14 �����N�G�K�Ō��ʂ��o�Ȃ��悤�ɂ���
		case SV_SCROLL_DETECT_DOOR:
		{
			int x,y,dir;
			if(!cave_clean_bold(py,px) || (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) 
				|| !dun_level || (dun_level >= d_info[dungeon_type].maxdepth)
				|| quest_number(dun_level))
			{
				msg_print("�����������h�ꂽ�C�������������N����Ȃ������B");
				break;
			}
			cave_set_feat(py, px, f_tag_to_index_in_init("TRAP_TRAPDOOR"));
			//if(!p_ptr->levitation) hit_trap(FALSE);
			if(!p_ptr->levitation) activate_floor_trap(py,px,0L);
			ident = TRUE;

			break;
		}

		//�A�C�e�����m�F�T�����}�e���A��
		case SV_SCROLL_DETECT_ITEM:
		{
			int i, cnt=0, idx = 0;
			for (i = 1; i < m_max; i++)
			{
				int x,y;
				monster_type *m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				y = m_ptr->fy;
				x = m_ptr->fx;
				if(player_bold(y,x)) continue;
				if (!projectable(py, px, y, x)) continue;
				cnt++;
				if(one_in_(cnt)) idx = i;
			}

			if(idx)
			{
				summon_material(idx);
				ident = TRUE;
			}
			break;
		}

		//�������F�@�ӖڂɂȂ�
		case SV_SCROLL_DETECT_INVIS:
		{
			if(!p_ptr->resist_blind)
			{
				set_blind(p_ptr->blind + 5 + randint1(5));
				ident = TRUE;
			}
			
			break;
		}

		//�Ύ׈����E �t���A�ɍ��ׂ��΂�܂����
		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			bool flag = FALSE;
			int num = 5 + randint1(5);
			for(;num>0;num--)
			{
				int xx = randint1(cur_wid-1);
				int yy = randint1(cur_hgt-1);

				if(!cave_naked_bold(yy,xx)) continue;

				if(summon_named_creature(-1,yy,xx,MON_UNMAKER,PM_NO_PET)) flag = TRUE;

			}
			if(flag)
			{
				msg_print("..�����s���ȋC�z������B");
				ident = TRUE;
			}

			break;
		}

		//���̃��[���@���e���΂�܂�
		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			int x, y;
			int r = 9;


			for (y = (py - r); y <= (py + r); y++)
			{
				for (x = (px - r); x <= (px + r); x++)
				{
					if (!in_bounds(y, x)) continue;
					if(!futo_can_place_plate(y,x) || cave[y][x].special) continue;
					if(distance(py, px, y, x) > r) continue;
					if(!projectable(py,px,y,x)) continue;

					cave[y][x].info |= CAVE_OBJECT;
					cave[y][x].mimic = feat_bomb;
					cave[y][x].special = 9;
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			msg_print("�ӂ肩���ĂɃp�`�p�`���������������n�߂�...");
			ident = TRUE;

			break;
		}

		//���E�̊����@�t���A�̐��������􂷂�
		case SV_SCROLL_GENOCIDE:
		{
			msg_print("�t���A�����̂�����悤�ȋC�z�ɖ������ꂽ..");
			raising_game();
			ident = TRUE;
			break;
		}

		default:
		msg_print("ERROR:���̊������T�O�����ǂ񂾎��̏�����������");
		return;
	}

	///item �����ȊO�̕���ǂ񂾎��̌���
	}
	else if (o_ptr->name1 == ART_POWER)
	{
		msg_print("�u��̎w�ւ͑S�Ă𓝂ׁA");
		msg_print(NULL);
		msg_print("��̎w�ւ͑S�Ă������A");
		msg_print(NULL);
		msg_print("��̎w�ւ͑S�Ă�߂炦��");
		msg_print(NULL);
		msg_print("�Èł̒��Ɍq���Ƃ߂�B�v");
		used_up = FALSE;

		{
			byte iy = o_ptr->iy;
			byte ix = o_ptr->ix;
			s16b next_o_idx = o_ptr->next_o_idx;
			byte marked = o_ptr->marked; 
			s16b weight = o_ptr->number * o_ptr->weight;
			u16b inscription = o_ptr->inscription;

			object_prep(o_ptr, o_ptr->k_idx);
			o_ptr->iy = iy;
			o_ptr->ix = ix;
			o_ptr->next_o_idx = next_o_idx;
			o_ptr->marked = marked;
			o_ptr->inscription = inscription;
			if (item >= 0) p_ptr->total_weight += (o_ptr->weight - weight);
			p_ptr->window |= (PW_INVEN | PW_EQUIP);
			window_stuff();
		}
		msg_print("���Ȃ������镶����ǂݏグ��ƁA��̎w�ւ�������������E�E");

	}
	//READABLE�t���O�����A�C�e���������ɗ���͂�
	else
	{
		///mod151213 �V���ǉ�
		if(o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI)
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];
			msg_print("�V����ǂ񂾁B");
			msg_format("%s�ɂ��Ă̋L�����E�E",r_name + r_ptr->name);
			if(lore_do_probe(o_ptr->pval))
				msg_format("%s�̏��𓾂��B",r_name + r_ptr->name);

			o_ptr->ident |= (IDENT_MENTAL);
			used_up = FALSE;
		}

		//v1.1.78 �ǉ�
		//v1.1.86 �A�r���e�B�J�[�h�ǉ�
		else if (o_ptr->tval == TV_ITEMCARD || o_ptr->tval == TV_SPELLCARD || o_ptr->tval == TV_ABILITY_CARD)
		{
			o_ptr->ident |= (IDENT_MENTAL);
			screen_object(o_ptr, SCROBJ_FORCE_DETAIL);

			used_up = FALSE;
		}
		else if (o_ptr->name1 == ART_HYAKKI)
		{
			used_up = FALSE;
			msg_print("���ǂ남�ǂ낵���G���`���ꂽ�G�����B������Ă��镶���͑S���ǂ߂Ȃ��B");

		}

		else
		{
			msg_print("ERROR:���̃A�C�e�����T�O�����ǂ񂾎��̏������ݒ肳��Ă��Ȃ�");
			return;
		}
	}

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* The item was tried */
	object_tried(o_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Hack -- allow certain scrolls to be "preserved" */
	if (!used_up)
	{
		return;
	}

	sound(SOUND_SCROLL);

	/*:::�g���������̐������炷�B*/
	// -Hack- �t�@�C�i���X�g���C�N�ȂǃA�C�e����������Ƃ��C���x���g���ԍ��������̂ł�����x�g����������T���Ă��猸�炷 
	if(item >= 0)
	{
		for(item2=0;item2<INVEN_PACK;item2++)
		{
			if(inventory[item2].k_idx == scroll_k_idx) break;
		}
		if(item2 < INVEN_PACK)
		{
			inven_item_increase(item2, -1);
			inven_item_describe(item2);
			inven_item_optimize(item2);
		}
	}
	else
	{
		s16b next_o_idx = 0;
				 
		for(item2 = cave[py][px].o_idx; item2; item2 = next_o_idx)
		{
			if(o_list[item2].k_idx == scroll_k_idx)break;
			next_o_idx = o_list[item2].next_o_idx;
		}
		if(item2)
		{
			floor_item_increase(item2, -1);
			floor_item_describe(item2);
			floor_item_optimize(item2);
		}
	}
}




/*
 * Read a scroll (from the pack or floor).
 *
 * Certain scrolls can be "aborted" without losing the scroll.  These
 * include scrolls with no effects but recharge or identify, which are
 * cancelled before use.  XXX Reading them still takes a turn, though.
 */
/*:::�u�ǂށv�R�}���h�̌��ʔ���*/
static void do_cmd_read_scroll_aux(int item, bool known)
{
	int         k, used_up, ident, lev;
	object_type *o_ptr;


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


	/* Take a turn */
	energy_use = 100;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");
#else
		msg_print("Nothing happen.");
#endif

		sound(SOUND_FAIL);
		return;
	}
	///class ����m�͊�����ǂ߂Ȃ�
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		if(one_in_(3))msg_print("����������ɗx��o���Ċ�����ǂ߂Ȃ��B");
		else if(one_in_(2))msg_print("���������ɓ����Ă��Ȃ��Ċ�����ǂ߂Ȃ��B");
		else msg_print("�����ɉ��������Ă��邩�킩��Ȃ��B");
		return;
	}

	if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0]))
	{
		msg_print("�Î�\�͂����������B");
		set_tim_general(0,TRUE,0,0);
		energy_use = 125;
	}


	if (p_ptr->asthma >= ASTHMA_3)
	{
		if(randint0(p_ptr->asthma) > 6600)
		{
			msg_print("���Ȃ��͊P������Ŋ�����ǂނ̂Ɏ��s�����I");
			return;
		}
	}

	if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();

	/* Hex */
	//if (hex_spelling_any() && ((p_ptr->lev < 35) || hex_spell_fully())) stop_hex_spell_all();

	/* Not identified yet */
	ident = FALSE;

	/* Object level */
	lev = k_info[o_ptr->k_idx].level;

	/* Assume the scroll will get used up */
	used_up = TRUE;

	if (o_ptr->tval == TV_SCROLL)
	{
	/* Analyze the scroll */
	switch (o_ptr->sval)
	{
		case SV_SCROLL_DARKNESS:
		{
			if (!(p_ptr->resist_blind) && !(p_ptr->resist_dark) && p_ptr->pclass != CLASS_NINJA)
			{
				(void)set_blind(p_ptr->blind + 3 + randint1(5));
			}
			if (unlite_area(10, 3)) ident = TRUE;
			break;
		}

		case SV_SCROLL_AGGRAVATE_MONSTER:
		{
#ifdef JP
			msg_print("�J���������Ȃ�l�ȉ����ӂ�𕢂����B");
#else
			msg_print("There is a high pitched humming noise.");
#endif

			aggravate_monsters(0,TRUE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_ARMOR:
		{
			if (curse_armor()) ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_WEAPON:
		{
			k = 0;
			if (buki_motteruka(INVEN_RARM))
			{
				k = INVEN_RARM;
				if (buki_motteruka(INVEN_LARM) && one_in_(2)) k = INVEN_LARM;
			}
			else if (buki_motteruka(INVEN_LARM)) k = INVEN_LARM;
			if (k && curse_weapon(FALSE, k)) ident = TRUE;
			break;
		}

		case SV_SCROLL_SUMMON_MONSTER:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if (summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_UNDEAD:
		{
			for (k = 0; k < randint1(3); k++)
			{
				if (summon_specific(0, py, px, dun_level, SUMMON_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_PET:
		{
			if (summon_specific(-1, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_FORCE_PET)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_SUMMON_KIN:
		{
			if (summon_kin_player(p_ptr->lev, py, px, (PM_FORCE_PET | PM_ALLOW_GROUP)))
			{
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_TRAP_CREATION:
		{
			if (trap_creation(py, px)) ident = TRUE;
			break;
		}

		case SV_SCROLL_PHASE_DOOR:
		{
			teleport_player(10, 0L);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT:
		{
			teleport_player(100, 0L);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT_LEVEL:
		{
			(void)teleport_level(0);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_WORD_OF_RECALL:
		{
			if (!word_of_recall()) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_IDENTIFY:
		{
			if (!ident_spell(FALSE)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_IDENTIFY:
		{
			if (!identify_fully(FALSE)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_REMOVE_CURSE:
		{
			if (remove_curse())
			{
#ifdef JP
				///msg131215
				//msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
				msg_print("�����i�̎������������B");

#else
				msg_print("You feel as if someone is watching over you.");
#endif

				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_STAR_REMOVE_CURSE:
		{
			if (remove_all_curse())
			{
#ifdef JP
				///msg131215
				//msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
				msg_print("�����i�̎������������B");
#else
				msg_print("You feel as if someone is watching over you.");
#endif
			}
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_ARMOR:
		{
			ident = TRUE;
			if (!enchant_spell(0, 0, 1)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			if (!enchant_spell(1, 0, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			if (!enchant_spell(0, 1, 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if (!enchant_spell(0, 0, randint1(3) + 2)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			if (!enchant_spell(randint1(3), randint1(3), 0)) used_up = FALSE;
			ident = TRUE;
			break;
		}

		case SV_SCROLL_RECHARGING:
		{
			if (!recharge(130)) used_up = FALSE;
			ident = TRUE;
			break;
		}
		/*:::�}�f�̊���*/
		case SV_SCROLL_MUNDANITY:
		{
			ident = TRUE;
			if (!mundane_spell(FALSE)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_LIGHT:
		{
			if (lite_area(damroll(2, 8), 2)) ident = TRUE;
			break;
		}

		case SV_SCROLL_MAPPING:
		{
			map_area(DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_GOLD:
		{
			if (detect_treasure(DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_objects_gold(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_ITEM:
		{
			if (detect_objects_normal(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_TRAP:
		{
			if (detect_traps(DETECT_RAD_DEFAULT, known)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_DOOR:
		{
			if (detect_doors(DETECT_RAD_DEFAULT)) ident = TRUE;
			if (detect_stairs(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_INVIS:
		{
			if (detect_monsters_invis(DETECT_RAD_DEFAULT)) ident = TRUE;
			break;
		}
/*
		case SV_SCROLL_SATISFY_HUNGER:
		{
			if (set_food(PY_FOOD_MAX - 1)) ident = TRUE;
			break;
		}
*/
/*
		case SV_SCROLL_BLESSING:
		{
			if (set_blessed(p_ptr->blessed + randint1(12) + 6, FALSE)) ident = TRUE;
			break;
		}
*/
		case SV_SCROLL_HOLY_CHANT:
		{
			if (set_blessed(p_ptr->blessed + randint1(24) + 12, FALSE)) ident = TRUE;
			break;
		}
/*
		case SV_SCROLL_HOLY_PRAYER:
		{
			if (set_blessed(p_ptr->blessed + randint1(48) + 24, FALSE)) ident = TRUE;
			break;
		}
*/
		case SV_SCROLL_MONSTER_CONFUSION:
		{
			if (!(p_ptr->special_attack & ATTACK_CONFUSE))
			{
#ifdef JP
				msg_print("�肪�P���n�߂��B");
#else
				msg_print("Your hands begin to glow.");
#endif

				p_ptr->special_attack |= ATTACK_CONFUSE;
				p_ptr->redraw |= (PR_STATUS);
				ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			k = 3 * p_ptr->lev;
			if (set_protevil(p_ptr->protevil + randint1(25) + k, FALSE)) ident = TRUE;
			break;
		}

		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			warding_glyph();
			ident = TRUE;
			break;
		}

		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
		{
			if (destroy_doors_touch()) ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_DESTRUCTION:
		{
			if (destroy_area(py, px, 13 + randint0(5), FALSE,FALSE,FALSE))
				ident = TRUE;
			else
#ifdef JP
msg_print("�_���W�������h�ꂽ...");
#else
				msg_print("The dungeon trembles...");
#endif


			break;
		}

		case SV_SCROLL_DISPEL_UNDEAD:
		{
			if (dispel_undead(80)) ident = TRUE;
			break;
		}
		///class item �����̊����@�p�~�\��
		case SV_SCROLL_SPELL:
		{
			/*
			if ((p_ptr->pclass == CLASS_WARRIOR) ||
				(p_ptr->pclass == CLASS_IMITATOR) ||
				(p_ptr->pclass == CLASS_MINDCRAFTER) ||
				(p_ptr->pclass == CLASS_SORCERER) ||
				(p_ptr->pclass == CLASS_ARCHER) ||
				(p_ptr->pclass == CLASS_MAGIC_EATER) ||
				(p_ptr->pclass == CLASS_RED_MAGE) ||
				(p_ptr->pclass == CLASS_SAMURAI) ||
				(p_ptr->pclass == CLASS_BLUE_MAGE) ||
				(p_ptr->pclass == CLASS_CAVALRY) ||
				(p_ptr->pclass == CLASS_BERSERKER) ||
				(p_ptr->pclass == CLASS_SMITH) ||
				(p_ptr->pclass == CLASS_MIRROR_MASTER) ||
				(p_ptr->pclass == CLASS_NINJA) ||
				(p_ptr->pclass == CLASS_SNIPER)) break;
				*/
			if(!p_ptr->realm1 || p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU) break;
			if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA) break;

			p_ptr->add_spells++;
			p_ptr->update |= (PU_SPELLS);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_GENOCIDE:
		{
			(void)symbol_genocide(300, TRUE,0);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_MASS_GENOCIDE:
		{
			(void)mass_genocide(300, TRUE);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_ACQUIREMENT:
		{
			acquirement(py, px, 1, TRUE, FALSE);
			set_deity_bias(DBIAS_REPUTATION, 1);
			ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ACQUIREMENT:
		{
			acquirement(py, px, randint1(2) + 1, TRUE, FALSE);
			set_deity_bias(DBIAS_REPUTATION, 3);
			ident = TRUE;
			break;
		}

		/* New Hengband scrolls */
		case SV_SCROLL_FIRE:
		{
			fire_ball(GF_FIRE, 0, 666, 4);
			/* Note: "Double" damage since it is centered on the player ... */
			if (!(IS_OPPOSE_FIRE() || p_ptr->resist_fire || p_ptr->immune_fire))
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 50+randint1(50), "���̊���", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 50 + randint1(50), "a Scroll of Fire", -1);
#endif

			ident = TRUE;
			break;
		}


		case SV_SCROLL_ICE:
		{
			fire_ball(GF_ICE, 0, 777, 4);
			if (!(IS_OPPOSE_COLD() || p_ptr->resist_cold || p_ptr->immune_cold))
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 100+randint1(100), "�X�̊���", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 100 + randint1(100), "a Scroll of Ice", -1);
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_CHAOS:
		{
			fire_ball(GF_CHAOS, 0, 1000, 4);
			if (!p_ptr->resist_chaos)
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 111+randint1(111), "���O���X�̊���", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 111 + randint1(111), "a Scroll of Logrus", -1);
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_RUMOR:
		{
#ifdef JP
			msg_print("�����ɂ̓��b�Z�[�W��������Ă���:");
#else
			msg_print("There is message on the scroll. It says:");
#endif

			msg_print(NULL);
			display_rumor(TRUE);
			msg_print(NULL);
#ifdef JP
			msg_print("�����͉��𗧂Ăď����������I");
#else
			msg_print("The scroll disappears in a puff of smoke!");
#endif

			ident = TRUE;
			break;
		}

		case SV_SCROLL_ARTIFACT:
		{
			ident = TRUE;
			if (!artifact_scroll()) used_up = FALSE;
			break;
		}

		case SV_SCROLL_RESET_RECALL:
		{
			ident = TRUE;
			if (!reset_recall()) used_up = FALSE;
			break;
		}
		///del131216 �N���̊����𖳌���
#if 0
		case SV_SCROLL_AMUSEMENT:
		{
			ident = TRUE;
			amusement(py, px, 1, FALSE);
			break;
		}

		case SV_SCROLL_STAR_AMUSEMENT:
		{
			ident = TRUE;
			amusement(py, px,  randint1(2) + 1, FALSE);
			break;
		}
#endif
	}

	///item �����ȊO�̕���ǂ񂾎��̌���
	}
///del131216 GHB�V���c
#if 0
	else if (o_ptr->name1 == ART_GHB)
	{
#ifdef JP
		msg_print("���͋�J���āw�O���[�^�[�E�w��=�r�[�X�g�x��|�����B");
		msg_print("��������ɓ������̂͂��̂����Ȃ��s�V���c�����������B");
#else
		msg_print("I had a very hard time to kill the Greater hell-beast, ");
		msg_print("but all I got was this lousy t-shirt!");
#endif
		used_up = FALSE;
	}
#endif
	else if (o_ptr->name1 == ART_POWER)
	{
#ifdef JP
		msg_print("�u��̎w�ւ͑S�Ă𓝂ׁA");
		msg_print(NULL);
		msg_print("��̎w�ւ͑S�Ă������A");
		msg_print(NULL);
		msg_print("��̎w�ւ͑S�Ă�߂炦��");
		msg_print(NULL);
		msg_print("�Èł̒��Ɍq���Ƃ߂�B�v");
#else
		msg_print("'One Ring to rule them all, ");
		msg_print(NULL);
		msg_print("One Ring to find them, ");
		msg_print(NULL);
		msg_print("One Ring to bring them all ");
		msg_print(NULL);
		msg_print("and in the darkness bind them.'");
#endif
		used_up = FALSE;
	}
	//READABLE�t���O�����A�C�e���������ɗ���͂�
	else
	{
		///mod151213 �V���ǉ�
		if(o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI)
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];
			msg_print("�V����ǂ񂾁B");
			msg_format("%s�ɂ��Ă̋L�����E�E",r_name + r_ptr->name);

			if(lore_do_probe(o_ptr->pval))
				msg_format("%s�̏��𓾂��B",r_name + r_ptr->name);


			o_ptr->ident |= (IDENT_MENTAL);
			used_up = FALSE;
		}
		//EXTRA���[�h�̃A�C�e���J�[�h
		//v1.1.56 �X�y�J�������悤�ɂ���
		//v1.1.86 �A�r���e�B�J�[�h���ǉ�
		else if(o_ptr->tval == TV_ITEMCARD || o_ptr->tval == TV_SPELLCARD || o_ptr->tval == TV_ABILITY_CARD)
		{
			o_ptr->ident |= (IDENT_MENTAL);
			screen_object(o_ptr,SCROBJ_FORCE_DETAIL);

			used_up = FALSE;
		}

		//v1.1.36 �S�S��s�G��
		else if(o_ptr->name1 == ART_HYAKKI)
		{
			bool can_read = FALSE;
			used_up = FALSE;

			//�ǂ߂����Ȏ푰�ƐE��(�K��)
			switch(p_ptr->pclass)
			{
			case CLASS_MAMIZOU:
			case CLASS_YUKARI:
			case CLASS_KASEN:
			case CLASS_RAN:
			case CLASS_AYA:
			case CLASS_SH_DEALER:
			case CLASS_BYAKUREN:
			case CLASS_KEINE:
			case CLASS_EIRIN:
				can_read = TRUE;
			}

			switch(p_ptr->prace)
			{
			case RACE_ONI:
			case RACE_DAIYOUKAI:
			case RACE_DEITY:
			case RACE_TSUKUMO:
				can_read = TRUE;
			}

			//������ꏈ���H
			if(p_ptr->pclass == CLASS_KOSUZU)
				msg_print("���Ȃ��͊G�����J���A�r���r���̗d�C�Ƀ]�N�]�N�����B");
			else if(can_read)
				msg_print("���̊G���ɂ͌Â��׋S�����󂳂�Ă���B�����ɓǂݏグ��Ɗ댯���B");
			else
				msg_print("���ǂ남�ǂ낵���G���`���ꂽ�G�����B������Ă��镶���͑S���ǂ߂Ȃ��B");

		}
		else
		{
			msg_print("ERROR:���̃A�C�e����ǂ񂾎��̏������ݒ肳��Ă��Ȃ�");
			return;
		}
	}


///del131216 �r�玆
#if 0
	else if (o_ptr->tval==TV_PARCHMENT)
	{
		cptr q;
		char o_name[MAX_NLEN];
		char buf[1024];

		/* Save screen */
		screen_save();

		q=format("book-%d_jp.txt",o_ptr->sval);

		/* Display object description */
		object_desc(o_name, o_ptr, OD_NAME_ONLY);

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, q);

		/* Peruse the help file */
		(void)show_file(TRUE, buf, o_name, 0, 0);

		/* Load screen */
		screen_load();

		used_up=FALSE;
	}
#endif

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	///del131216 virtue
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/
	/* The item was tried */
	object_tried(o_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Hack -- allow certain scrolls to be "preserved" */
	if (!used_up)
	{
		return;
	}

	sound(SOUND_SCROLL);

	/* Destroy a scroll in the pack */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Destroy a scroll on the floor */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}


/*
 * Hook to determine if an object is readable
 */
/*:::�ǂ߂�A�C�e���@�����AT�V���c�A��̎w�ցA�r�玆���܂܂��*/
///mod131223 �V���c�Ɨr�玆�폜
///mod151213 READABLE�t���O�����ǉ�
static bool item_tester_hook_readable(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	if ((o_ptr->tval==TV_SCROLL) || (o_ptr->name1 == ART_POWER)) return (TRUE);
	object_flags(o_ptr, flgs);

	if (have_flag(flgs, TR_READABLE)) return (TRUE);

	/* Assume not */
	return (FALSE);
}

/*:::�ǂ�*/
void do_cmd_read_scroll(void)
{
	object_type *o_ptr;
	int  item;
	cptr q, s;

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Check some conditions */
	if (p_ptr->blind)
	{
#ifdef JP
		msg_print("�ڂ������Ȃ��B");
#else
		msg_print("You can't see anything.");
#endif

		return;
	}
	if (no_lite())
	{
#ifdef JP
		msg_print("�����肪�Ȃ��̂ŁA�Â��ēǂ߂Ȃ��B");
#else
		msg_print("You have no light to read by.");
#endif

		return;
	}
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("�������Ă��ēǂ߂Ȃ��B");
#else
		msg_print("You are too confused!");
#endif

		return;
	}
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
#ifdef JP
		msg_print("���̑̂ł͊�����ǂ߂Ȃ��B");
#else
		msg_print("You are too confused!");
#endif
		return;
	}




	/* Restrict choices to scrolls */
	item_tester_hook = item_tester_hook_readable;

	/* Get an item */
#ifdef JP
	q = "����ǂ݂܂���? ";
	s = "�ǂ߂銪�����Ȃ��B";
#else
	q = "Read which scroll? ";
	s = "You have no scrolls to read.";
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
	if(p_ptr->pclass == CLASS_SAGUME)
		sagume_read_scroll_aux(item, object_is_aware(o_ptr));
	else
	/* Read the scroll */
		do_cmd_read_scroll_aux(item, object_is_aware(o_ptr));
}

/*:::��̌��ʔ����@��g�p�Ɩ�����p�t�̋Z*/
static int staff_effect(int sval, bool *use_charge, bool powerful, bool magic, bool known)
{
	int k;
	int ident = FALSE;
	int lev = powerful ? (p_ptr->lev * 2) : p_ptr->lev;
	int detect_rad = powerful ? DETECT_RAD_DEFAULT * 3 / 2 : DETECT_RAD_DEFAULT;

	/* Analyze the staff */
	switch (sval)
	{
		case SV_STAFF_DARKNESS:
		{
			if (!(p_ptr->resist_blind) && !(p_ptr->resist_dark) && p_ptr->pclass != CLASS_NINJA)
			{
				if (set_blind(p_ptr->blind + 3 + randint1(5))) ident = TRUE;
			}
			if (unlite_area(10, (powerful ? 6 : 3))) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOWNESS:
		{
			if (set_slow(p_ptr->slow + randint1(30) + 15, FALSE)) ident = TRUE;
			break;
		}

		case SV_STAFF_HASTE_MONSTERS:
		{
			if (speed_monsters()) ident = TRUE;
			break;
		}

		case SV_STAFF_SUMMONING:
		{
			const int times = randint1(powerful ? 8 : 4);
			for (k = 0; k < times; k++)
			{
				if (summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
				{
					ident = TRUE;
				}
			}
			break;
		}

		case SV_STAFF_TELEPORTATION:
		{
			teleport_player((powerful ? 150 : 100), 0L);
			ident = TRUE;
			break;
		}

		case SV_STAFF_IDENTIFY:
		{
			if (powerful) {
				if (!identify_fully(FALSE)) *use_charge = FALSE;
			} else {
				if (!ident_spell(FALSE)) *use_charge = FALSE;
			}
			ident = TRUE;
			break;
		}

		case SV_STAFF_REMOVE_CURSE:
		{
			bool result = powerful ? remove_all_curse() : remove_curse();
			if (result)
			{
				if (magic)
				{
#ifdef JP
					///msg131216
					//msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
					msg_print("�����i�̎������������B");

#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
				else if (!p_ptr->blind)
				{
#ifdef JP
					msg_print("��͈�u�u���[�ɋP����...");
#else
					msg_print("The staff glows blue for a moment...");
#endif

				}
				ident = TRUE;
			}
			break;
		}

		case SV_STAFF_STARLITE:
		{
			int num = damroll(5, 3);
			int y, x;
			int attempts;

			if (!p_ptr->blind && !magic)
			{
#ifdef JP
				msg_print("��̐悪���邭�P����...");
#else
				msg_print("The end of the staff glows brightly...");
#endif

			}
			for (k = 0; k < num; k++)
			{
				attempts = 1000;

				while (attempts--)
				{
					scatter(&y, &x, py, px, 4, 0);

					if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

					if (!player_bold(y, x)) break;
				}

				project(0, 0, y, x, damroll(6 + lev / 8, 10), GF_LITE_WEAK,
						  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
			}
			ident = TRUE;
			break;
		}

		case SV_STAFF_LITE:
		{
			if (lite_area(damroll(2, 8), (powerful ? 4 : 2))) ident = TRUE;
			break;
		}

		case SV_STAFF_MAPPING:
		{
			map_area(powerful ? DETECT_RAD_MAP * 3 / 2 : DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_GOLD:
		{
			if (detect_treasure(detect_rad)) ident = TRUE;
			if (detect_objects_gold(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_ITEM:
		{
			if (detect_objects_normal(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_TRAP:
		{
			if (detect_traps(detect_rad, known)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_DOOR:
		{
			if (detect_doors(detect_rad)) ident = TRUE;
			if (detect_stairs(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_INVIS:
		{
			if (detect_monsters_invis(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_EVIL:
		{
			if (detect_monsters_evil(detect_rad)) ident = TRUE;
			break;
		}

		case SV_STAFF_CURE_LIGHT:
		{
			if (hp_player(damroll((powerful ? 4 : 2), 8))) ident = TRUE;
			if (powerful) {
				if (set_blind(0)) ident = TRUE;
				if (set_poisoned(0)) ident = TRUE;
				if (set_cut(p_ptr->cut - 10)) ident = TRUE;
			}
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_CURING:
		{
			if (set_blind(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_image(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			set_asthma(p_ptr->asthma - 1500);

			break;
		}

		case SV_STAFF_HEALING:
		{
			if (hp_player(powerful ? 500 : 300)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_THE_MAGI:
		{
			if (do_res_stat(A_INT)) ident = TRUE;
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
				ident = TRUE;
#ifdef JP
				msg_print("�����n�b�L���Ƃ����B");
#else
				msg_print("You feel your head clear.");
#endif

				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);
			}
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLEEP_MONSTERS:
		{
			//v1.1.90 +10
			if (sleep_monsters(lev+10)) ident = TRUE;
			break;
		}

		case SV_STAFF_SLOW_MONSTERS:
		{
			//v1.1.90 +10
			if (slow_monsters(lev+10)) ident = TRUE;
			break;
		}

		case SV_STAFF_SPEED:
		{
			if (set_fast(randint1(30) + (powerful ? 30 : 15), FALSE)) ident = TRUE;
			break;
		}

		case SV_STAFF_PROBING:
		{
			probing();
			ident = TRUE;
			break;
		}

		case SV_STAFF_DISPEL_EVIL:
		{
			if (dispel_evil(powerful ? 120 : 80)) ident = TRUE;
			break;
		}

		case SV_STAFF_POWER:
		{
			if (dispel_monsters(powerful ? 225 : 150)) ident = TRUE;
			break;
		}

		case SV_STAFF_HOLINESS:
		{
			if (dispel_evil(powerful ? 225 : 150)) ident = TRUE;
			k = 3 * lev;
			if (set_protevil((magic ? 0 : p_ptr->protevil) + randint1(25) + k, FALSE)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_afraid(0)) ident = TRUE;
			if (hp_player(50)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			break;
		}

		case SV_STAFF_GENOCIDE:
		{
			(void)symbol_genocide((magic ? lev + 50 : 200), TRUE,0);
			ident = TRUE;
			break;
		}

		case SV_STAFF_EARTHQUAKES:
		{
			if (earthquake(py, px, (powerful ? 15 : 10)))
				ident = TRUE;
			else
#ifdef JP
msg_print("�_���W�������h�ꂽ�B");
#else
				msg_print("The dungeon trembles.");
#endif


			break;
		}

		case SV_STAFF_DESTRUCTION:
		{
			if (destroy_area(py, px, (powerful ? 18 : 13) + randint0(5), FALSE,FALSE,FALSE))
				ident = TRUE;

			break;
		}
		///del131223���l�Ԃ��̏�
		/*
		case SV_STAFF_ANIMATE_DEAD:
		{
			if (animate_dead(0, py, px))
				ident = TRUE;

			break;
		}
		*/
		case SV_STAFF_MSTORM:
		{
#ifdef JP
			msg_print("���͂Ȗ��͂��G�������􂢂��I");
#else
			msg_print("Mighty magics rend your enemies!");
#endif
			project(0, (powerful ? 7 : 5), py, px,
				(randint1(200) + (powerful ? 500 : 300)) * 2, GF_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
			//if ((p_ptr->pclass != CLASS_MAGE) && (p_ptr->pclass != CLASS_HIGH_MAGE) && (p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_MAGIC_EATER) && (p_ptr->pclass != CLASS_BLUE_MAGE))
			if(!(cp_ptr->magicmaster))
			{
#ifdef JP
				(void)take_hit(DAMAGE_NOESCAPE, 50, "�R���g���[��������͂Ȗ��͂̉��", -1);
#else
				(void)take_hit(DAMAGE_NOESCAPE, 50, "unleashing magics too mighty to control", -1);
#endif
			}
			ident = TRUE;

			break;
		}

		case SV_STAFF_NOTHING:
		{
#ifdef JP
			msg_print("�����N��Ȃ������B");
#else
			msg_print("Nothing happen.");
#endif
			if (prace_is_(RACE_GOLEM) || prace_is_(RACE_TSUKUMO) || prace_is_(RACE_ANDROID)) 
#ifdef JP
				msg_print("���������Ȃ����������悤�ȋC������B�H�ו��͑�؂ɂ��Ȃ��ẮB");
#else
				msg_print("What a waste.  It's your food!");
#endif
			break;
		}
	}
	return ident;
}

/*
 * Use a staff.			-RAK-
 *
 * One charge of one staff disappears.
 *
 * Hack -- staffs of identify can be "cancelled".
 */
static void do_cmd_use_staff_aux(int item)
{
	int         ident, chance, lev;
	object_type *o_ptr;
	bool success = TRUE;
	bool powerful = FALSE;


	/* Hack -- let staffs of identify get aborted */
	bool use_charge = TRUE;


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


	/* Mega-Hack -- refuse to use a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
#ifdef JP
		msg_print("�܂��͏���E��Ȃ���΁B");
#else
		msg_print("You must first pick up the staffs.");
#endif

		return;
	}


	/* Take a turn */
	energy_use = 100;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;
	if (lev > 50) lev = 50 + (lev - 50)/2;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");
#else
		msg_print("Nothing happen. Maybe this staff is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || ( p_ptr->pseikaku == SEIKAKU_BERSERK)) success = FALSE;
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	if(!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("������܂��g���Ȃ������B");
#else
		msg_print("You failed to use the staff properly.");
#endif

		///mod140829 �t�����͂�������F�X��
		if(p_ptr->pclass == CLASS_FLAN && one_in_(13))
		{
			msg_print("�񂪎�̒��Ŕ��������I");
			if(item >= 0)
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
		}



		sound(SOUND_FAIL);
		return;
	}

	/* Notice empty staffs */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���̏�ɂ͂������͂��c���Ă��Ȃ��B");
#else
		msg_print("The staff has no charges left.");
#endif

		o_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN);

		return;
	}


	/* Sound */
	sound(SOUND_ZAP);

	if(check_activated_nameless_arts(JKF1_MASTER_MAGITEM)) powerful = TRUE;

	ident = staff_effect(o_ptr->sval, &use_charge, powerful, FALSE, object_is_aware(o_ptr));
	///del131216 virtue
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/
	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Tried the item */
	object_tried(o_ptr);

	/* An identification was made */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Hack -- some uses are "free" */
	if (!use_charge) return;


	/* Use a single charge */
	o_ptr->pval--;

	/* XXX Hack -- unstack if necessary */
	if ((item >= 0) && (o_ptr->number > 1))
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

	/* Describe charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(item);
	}

	/* Describe charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}



/*::: USABLE�t���O�̂�������A�C�e�����g�������� */
//���̂Ƃ���g���Ă��A�C�e���͏����Ȃ�
static void do_cmd_use_special_item(int item)
{
	object_type *o_ptr;
	bool disposable = FALSE;

	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];


	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");
		sound(SOUND_FAIL);
		return;
	}
	//�E���g���\�j�b�N����L�@r�V���{���ւ̖��E
	if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_ULTRASONIC)
	{

		if(p_ptr->prace == RACE_WARBEAST || p_ptr->prace == RACE_GYOKUTO ||  p_ptr->prace == RACE_HAKUROU_TENGU
			|| p_ptr->prace == RACE_YOUKO || p_ptr->prace == RACE_BAKEDANUKI || p_ptr->muta1 & MUT1_BEAST_EAR
			|| p_ptr->pclass == CLASS_MIKO)
		{
			msg_print("�����񂴂��悤�ȍ������苿�����I");
		}
		else
		{
			msg_print("�����������ȉ������������C������E�E");
		}

		(void)symbol_genocide(300, FALSE,'r');

	}
	//�E�B�W���{�[�h�@�����_�����m��G�V���{���G�Ώ���
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_OUIJA_BOARD)
	{
		int tmp = randint0(100);
		msg_print("���Ȃ��̓v�����V�F�b�g�Ɏ��u���ĔO�����E�E");

		if(tmp < 80)
		{
			msg_print("�����������N����Ȃ������B");
		}
		else if(tmp < 95)
		{
			msg_print("�ᑭ�삪�߂��̏��������Ă��ꂽ�I");
			if(one_in_(3)) 
				detect_objects_normal(DETECT_RAD_DEFAULT);
			else if(one_in_(2))
			{
				detect_monsters_normal(DETECT_RAD_DEFAULT);
				detect_monsters_invis(DETECT_RAD_DEFAULT);
			}
			else
				detect_traps(DETECT_RAD_DEFAULT,TRUE);
		}
		else
		{
			msg_print("�G�ΓI�ȗ���Ăяo���Ă��܂����I");
			summon_specific(0,py,px,dun_level+20,SUMMON_GHOST,(PM_ALLOW_UNIQUE | PM_NO_PET));
		}
	}
	//�P�Z�����p�T�����@�_��_�E���G��Ɣ����I�[���l��
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_KESERAN_PASARAN)
	{
		if(p_ptr->pclass == CLASS_REIMU  || p_ptr->pclass == CLASS_PRIEST
			|| p_ptr->prace == RACE_DEITY || p_ptr->prace == RACE_STRAYGOD|| p_ptr->prace == RACE_DEMIGOD
			|| REALM_SPELLMASTER && cp_ptr->realm_aptitude[TV_BOOK_PUNISH] > 1
			|| p_ptr->realm1 == TV_BOOK_PUNISH || p_ptr->realm2 == TV_BOOK_PUNISH)
		{
			msg_print("�P�Z�����p�T�����͂��Ȃ��ɂȂ��Ă���B�K���ȋC�����ɂȂ����B");
			(void)gain_random_mutation(193);//�����I�[��
		}
		else
		{
			msg_print("�G�낤�Ƃ�����Ód�C�̂悤�Ȃ��̂Œe���ꂽ�B���܂�D����Ă��Ȃ��悤���E�E");
		}
	}
	//v1.1.18 �������������Ă���������@�t���A�̗d���K�Ƃ��₵���e�̕ϐg����������悤�ɂ��Ă݂�
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_MARISA_TUBO)
	{
		int i;
		bool flag = FALSE;

		for(i=1;i<m_max;i++)
		{
			monster_type *m_ptr = &m_list[i];	
			if (!is_original_ap(m_ptr))
			{
				if (m_ptr->mflag2 & MFLAG2_KAGE) m_ptr->mflag2 &= ~(MFLAG2_KAGE);

				m_ptr->ap_r_idx = m_ptr->r_idx;
				lite_spot(m_ptr->fy, m_ptr->fx);
				flag = TRUE;
			}		
		}
		if(flag)
		{
			if(one_in_(3)) 
				msg_print("��u�₪�͂��ɖc�ꂽ�C�������B");
			else if(one_in_(2)) 
				msg_print("��������ɋz�����܂ꂽ�悤�ȋC������B");
			else
				msg_print("�������Q�Ă�悤�ȋC�z���`����Ă����B");
		}
		else
		{
			if(one_in_(7)) 
				msg_print("���@���Ă݂��������N����Ȃ�����..");
			else if(one_in_(6)) 
				msg_print("�������Ă݂��������N����Ȃ�����..");
			else if(one_in_(5)) 
				msg_print("������Ă݂��������N����Ȃ�����..");
			else if(one_in_(4)) 
				msg_print("���`���Ă݂��������N����Ȃ�����..");
			else if(one_in_(3)) 
				msg_print("����f���Ă݂��������N����Ȃ�����..");
			else if(one_in_(2)) 
				msg_print("��𖁂��Ă݂��������N����Ȃ�����..");
			else
				msg_print("��𕏂łĂ݂��������N����Ȃ�����..");
		}
	}
	//v1.1.19 �����̚�@������
	else if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU)
	{
		object_type forge, *q_ptr = &forge;
		msg_print("�₩��������ݏo�����B");
		object_prep(q_ptr, lookup_kind(TV_FLASK, SV_FLASK_OIL));
		drop_near(q_ptr, -1, py, px);
	}
	//v1.1.42�@��̌�
	else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_SHILVER_KEY)
	{
		msg_print("����G���Ă���ƃJ�`���Ɖ����ɛƂ܂銴�G�������B");

		if (dun_level)
		{
			alter_reality();
		}
		else
		{
			tele_town(TRUE);
		}

	}
	//v1.1.43 ���X�����X�}�z
	else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_SMARTPHONE)
	{
		if(p_ptr->pclass == CLASS_OUTSIDER || p_ptr->pclass == CLASS_SUMIREKO)
			msg_print("�X�}�[�g�t�H���ŕӂ���Ƃ炵���B");
		else
			msg_print("�X�}�[�g�t�H����M���Ă���ƌ��肾�����I");
		lite_area(damroll(1, 2), 2);
	}
	//v2.0.11 ���̕��
	//�u�q����ق�̂���P���v�炵���̂ŉ�łƎ���􂾂�����Ă���
	//�{���ɂ��̒��x�̌��ʂȂ琴���͂ǂ�����ēe�̊y���ȂǍ����肾�����̂��Ƃ����^��͂��邪
	else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_MOON_ORB)
	{
		msg_print("�q�ꂪ�ق�̂���P����C������...");
		set_poisoned(0);
		remove_curse();

	}

	//v1.1.56 �X�y�J
	else if (o_ptr->tval == TV_SPELLCARD)
	{
		use_spellcard(o_ptr);
	}
	//v1.1.86 �A�r���e�B�J�[�h
	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		//�^�[�Q�b�g�L�����Z���Ȃǂ����ꍇFALSE���Ԃ�s��������Ȃ�
		if(!use_ability_card(o_ptr)) return;
	}
	else
	{
		msg_format("ERROR:���̃A�C�e���̓���g�p�������L�q����Ă��Ȃ�");
		return;
	}

	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Take a turn */
	energy_use = 100;


}



///mod141231 u�R�}���h�Ŏg�p�\�ȃA�C�e���iu�őS�����g���I�v�V�����ł͂Ȃ�)
/*:::��̂ق���USABLE�t���O�����A�C�e���𒊏o*/
static bool item_tester_hook_cmd_use(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

 	if (o_ptr->tval == TV_STAFF) return TRUE;

	object_flags(o_ptr, flgs);

	if (have_flag(flgs, TR_USABLE)) return TRUE;

	if (o_ptr->tval == TV_ABILITY_CARD)
	{
		if(o_ptr->pval >= 0 && o_ptr->pval < ABILITY_CARD_LIST_LEN && ability_card_list[o_ptr->pval].activate) return TRUE;
	}


	return FALSE;
}

/*:::����g��*/
void do_cmd_use_staff(void)
{
	int  item;
	cptr q, s;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
		msg_print("���܂��������ĂȂ��B");
		return ;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	///sys item u�R�}���h�@USABLE�t���O�̃A�C�e���̏�����ǉ�
	/* Restrict choices to wands */
	//item_tester_tval = TV_STAFF;

	item_tester_hook = item_tester_hook_cmd_use;


	/* Get an item */
#ifdef JP
	q = "�����g���܂���? ";
	s = "�g����A�C�e�����Ȃ��B";//v1.1.86 �񁨃A�C�e���@�A�r���e�B�J�[�h�Ƃ��g���悤�ɂȂ����̂�
#else
	q = "Use which staff? ";
	s = "You have no staff to use.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;


	///mod141231 �g�p�\����A�C�e������
	if (item >= 0 && inventory[item].tval == TV_STAFF || item < 0 && o_list[0 - item].tval == TV_STAFF)
	{
		do_cmd_use_staff_aux(item);
	}
	else
	{
		do_cmd_use_special_item(item);

	}




}

/*:::���@�_�̌��ʔ����@���@�_�g�p�Ɩ�����p�t�̏p�ŌĂ΂��*/
static int wand_effect(int sval, int dir, bool powerful, bool magic)
{
	int ident = FALSE;
	int lev = powerful ? (p_ptr->lev * 2) : p_ptr->lev;
	int rad = powerful ? 3 : 2;

	/* XXX Hack -- Wand of wonder can do anything before it */
	if (sval == SV_WAND_WONDER)
	{
		///del131214 virtue
		//int vir = virtue_number(V_CHANCE);
		sval = randint0(SV_WAND_WONDER);
		/*
		if (vir)
		{
			if (p_ptr->virtues[vir - 1] > 0)
			{
				while (randint1(300) < p_ptr->virtues[vir - 1]) sval++;
				if (sval > SV_WAND_COLD_BALL) sval = randint0(4) + SV_WAND_ACID_BALL;
			}
			else
			{
				while (randint1(300) < (0-p_ptr->virtues[vir - 1])) sval--;
				if (sval < SV_WAND_HEAL_MONSTER) sval = randint0(3) + SV_WAND_HEAL_MONSTER;
			}
		}
		if (sval < SV_WAND_TELEPORT_AWAY)
			chg_virtue(V_CHANCE, 1);
		*/
	}

	/* Analyze the wand */
	switch (sval)
	{
		case SV_WAND_HEAL_MONSTER:
		{
			int dam = damroll((powerful ? 20 : 10), 10);
			if (heal_monster(dir, dam)) ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if (speed_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if (clone_monster(dir)) ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			int distance = MAX_SIGHT * (powerful ? 8 : 5);
			if (teleport_monster(dir, distance)) ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if (disarm_trap(dir)) ident = TRUE;
			if (powerful && disarm_traps_touch()) ident = TRUE;
			break;
		}

		//v1.1.96 �g���b�v�E�h�A�j��̖��@�_���g���b�v�����̖��@�_�ɕύX����
		//�g���b�v�����̖��@�_�Ɩ������啔������Ă����̂Œ��x����
		case SV_WAND_TRAP_DOOR_DEST:
		{

			//if (destroy_door(dir)) ident = TRUE;
			//if (powerful && destroy_doors_touch()) ident = TRUE;

			int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
			return (project_hook(GF_ACTIV_TRAP, dir, 0, flg));

			break;
		}

		case SV_WAND_STONE_TO_MUD:
		{
			int dam = powerful ? 40 + randint1(60) : 20 + randint1(30);
			if (wall_to_mud(dir, dam)) ident = TRUE;
			break;
		}

		case SV_WAND_LITE:
		{
			int dam = damroll((powerful ? 12 : 6), 8);
#ifdef JP
			msg_print("���P�������������ꂽ�B");
#else
			msg_print("A line of blue shimmering light appears.");
#endif

			(void)lite_line(dir, dam);
			ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			//v1.1.90 +20
			if (sleep_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_SLOW_MONSTER:
		{
			//v1.1.90 +20
			if (slow_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			//v1.1.90 +20
			if (confuse_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			//v1.1.90 +20
			if (fear_monster(dir, lev+20)) ident = TRUE;
			break;
		}

		case SV_WAND_DRAIN_LIFE:
		{
			if (drain_life(dir, 80 + lev)) ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if (poly_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_WAND_STINKING_CLOUD:
		{
			fire_ball(GF_POIS, dir, 12 + lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_MAGIC_MISSILE:
		{
			fire_bolt_or_beam(20, GF_MISSILE, dir, damroll(2 + lev / 10, 6));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BOLT:
		{
			fire_bolt_or_beam(20, GF_ACID, dir, damroll(6 + lev / 7, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_CHARM_MONSTER:
		{
			if (charm_monster(dir, MAX(20, lev)))
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BOLT:
		{
			fire_bolt_or_beam(20, GF_FIRE, dir, damroll(7 + lev / 6, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BOLT:
		{
			fire_bolt_or_beam(20, GF_COLD, dir, damroll(5 + lev / 8, 8));
			ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 60 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 40 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 70 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 50 + 3 * lev / 4, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_WONDER:
		{
#ifdef JP
			msg_print("�����ƁA��̖��@�_���n���������B");
#else
			msg_print("Oops.  Wand of wonder activated.");
#endif

			break;
		}

		case SV_WAND_DRAGON_FIRE:
		{
			fire_ball(GF_FIRE, dir, (powerful ? 300 : 200), -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			fire_ball(GF_COLD, dir, (powerful ? 270 : 180), -3);
			ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			int dam;
			int typ;

			switch (randint1(5))
			{
				case 1:
					dam = 240;
					typ = GF_ACID;
					break;
				case 2:
					dam = 210;
					typ = GF_ELEC;
					break;
				case 3:
					dam = 240;
					typ = GF_FIRE;
					break;
				case 4:
					dam = 210;
					typ = GF_COLD;
					break;
				default:
					dam = 180;
					typ = GF_POIS;
					break;
			}

			if (powerful) dam = (dam * 3) / 2;

			fire_ball(typ, dir, dam, -3);

			ident = TRUE;
			break;
		}

		case SV_WAND_DISINTEGRATE:
		{
			fire_ball(GF_DISINTEGRATE, dir, 200 + randint1(lev * 2), rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_ROCKETS:
		{
#ifdef JP
msg_print("���P�b�g�𔭎˂����I");
#else
			msg_print("You launch a rocket!");
#endif

			fire_rocket(GF_ROCKET, dir, 250 + lev * 3, rad);
			ident = TRUE;
			break;
		}

		case SV_WAND_STRIKING:
		{
			fire_bolt(GF_METEOR, dir, damroll(15 + lev / 3, 13));
			ident = TRUE;
			break;
		}

		case SV_WAND_GENOCIDE:
		{
			fire_ball_hide(GF_GENOCIDE, dir, magic ? lev + 50 : 250, 0);
			ident = TRUE;
			break;
		}
	}
	return ident;
}


/*
 * Aim a wand (from the pack or floor).
 *
 * Use a single charge from a single item.
 * Handle "unstacking" in a logical manner.
 *
 * For simplicity, you cannot use a stack of items from the
 * ground.  This would require too much nasty code.
 *
 * There are no wands which can "destroy" themselves, in the inventory
 * or on the ground, so we can ignore this possibility.  Note that this
 * required giving "wand of wonder" the ability to ignore destruction
 * by electric balls.
 *
 * All wands can be "cancelled" at the "Direction?" prompt for free.
 *
 * Note that the basic "bolt" wands do slightly less damage than the
 * basic "bolt" rods, but the basic "ball" wands do the same damage
 * as the basic "ball" rods.
 */
/*:::���@�_�̎g�p�@���@�_�I���ς݁@��������͂܂�*/
static void do_cmd_aim_wand_aux(int item)
{
	int         lev, ident, chance, dir;
	object_type *o_ptr;
	bool old_target_pet = target_pet;
	bool success = TRUE;
	bool powerful = FALSE;

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

	/* Mega-Hack -- refuse to aim a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
#ifdef JP
		msg_print("�܂��͖��@�_���E��Ȃ���΁B");
#else
		msg_print("You must first pick up the wands.");
#endif

		return;
	}


	/* Allow direction to be cancelled for free */
	if (object_is_aware(o_ptr) && (o_ptr->sval == SV_WAND_HEAL_MONSTER
				      || o_ptr->sval == SV_WAND_HASTE_MONSTER))
			target_pet = TRUE;
	if (!get_aim_dir(&dir))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	/* Take a turn */
	energy_use = 100;

	/* Get the level */
	lev = k_info[o_ptr->k_idx].level;
	if (lev > 50) lev = 50 + (lev - 50)/2;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* Hight level objects are harder */
	chance = chance - lev;

	/*:::USE_DEVICE=3*/
	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");
#else
		msg_print("Nothing happen. Maybe this wand is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE) || ( p_ptr->pseikaku == SEIKAKU_BERSERK)) success = FALSE;
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	if(!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���@�_�����܂��g���Ȃ������B");
#else
		msg_print("You failed to use the wand properly.");
#endif
		///mod140829 �t�����͂�������F�X��
		if(p_ptr->pclass == CLASS_FLAN && one_in_(13))
		{
			object_type forge;
			object_type *q_ptr = &forge;
			object_copy(q_ptr, o_ptr);
			distribute_charges(o_ptr, q_ptr, 1);

			msg_print("���@�_����̒��Ŕ��������I");
			if(item >= 0)
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
		}
		sound(SOUND_FAIL);
		return;
	}

	/* The wand is already empty! */
	if (o_ptr->pval <= 0)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���̖��@�_�ɂ͂������͂��c���Ă��Ȃ��B");
#else
		msg_print("The wand has no charges left.");
#endif

		o_ptr->ident |= (IDENT_EMPTY);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN);

		return;
	}

	/* Sound */
	sound(SOUND_ZAP);

	if(check_activated_nameless_arts(JKF1_MASTER_MAGITEM)) powerful = TRUE;

	ident = wand_effect(o_ptr->sval, dir, powerful, FALSE);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214 
	/*
	if (!(object_is_aware(o_ptr)))
	{
		chg_virtue(V_PATIENCE, -1);
		chg_virtue(V_CHANCE, 1);
		chg_virtue(V_KNOWLEDGE, -1);
	}
	*/

	/* Mark it as tried */
	object_tried(o_ptr);

	/* Apply identification */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);


	/* Use a single charge */
	o_ptr->pval--;

	/* Describe the charges in the pack */
	if (item >= 0)
	{
		inven_item_charges(item);
	}

	/* Describe the charges on the floor */
	else
	{
		floor_item_charges(0 - item);
	}
}


void do_cmd_aim_wand(void)
{
	int     item;
	cptr    q, s;


	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
		msg_print("���܂����@�_�����ĂȂ��B");
		return ;
	}

	/* Restrict choices to wands */
	item_tester_tval = TV_WAND;

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Get an item */
#ifdef JP
	q = "�ǂ̖��@�_�ő_���܂���? ";
	s = "�g���閂�@�_���Ȃ��B";
#else
	q = "Aim which wand? ";
	s = "You have no wand to aim.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Aim the wand */
	do_cmd_aim_wand_aux(item);
}

/*:::���b�h�̌��ʔ����@*/
static int rod_effect(int sval, int dir, bool *use_charge, bool powerful, bool magic)
{
	int ident = FALSE;
	int lev = powerful ? (p_ptr->lev * 2) : p_ptr->lev;
	int detect_rad = powerful ? DETECT_RAD_DEFAULT * 3 / 2 : DETECT_RAD_DEFAULT;
	int rad = powerful ? 3 : 2;

	/* Unused */
	(void)magic;

	/* Analyze the rod */
	switch (sval)
	{
		case SV_ROD_DETECT_TRAP:
		{
			if (detect_traps(detect_rad, (bool)(dir ? FALSE : TRUE))) ident = TRUE;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			if (detect_doors(detect_rad)) ident = TRUE;
			if (detect_stairs(detect_rad)) ident = TRUE;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			if (powerful) {
				if (!identify_fully(FALSE)) *use_charge = FALSE;
			} else {
				if (!ident_spell(FALSE)) *use_charge = FALSE;
			}
			ident = TRUE;
			break;
		}

		case SV_ROD_RECALL:
		{
			if (!word_of_recall()) *use_charge = FALSE;
			ident = TRUE;
			break;
		}

		case SV_ROD_ILLUMINATION:
		{
			if (lite_area(damroll(2, 8), (powerful ? 4 : 2))) ident = TRUE;
			break;
		}

		case SV_ROD_MAPPING:
		{
			map_area(powerful ? DETECT_RAD_MAP * 3 / 2 : DETECT_RAD_MAP);
			ident = TRUE;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detect_all(detect_rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_PROBING:
		{
			probing();
			ident = TRUE;
			break;
		}

		case SV_ROD_CURING:
		{
			if (set_blind(0)) ident = TRUE;
			if (set_poisoned(0)) ident = TRUE;
			if (set_confused(0)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_image(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			set_asthma(p_ptr->asthma - 3000);
			break;
		}

		case SV_ROD_HEALING:
		{
			if (hp_player(powerful ? 750 : 500)) ident = TRUE;
			if (set_stun(0)) ident = TRUE;
			if (set_cut(0)) ident = TRUE;
			if (set_shero(0,TRUE)) ident = TRUE;
			break;
		}

		case SV_ROD_RESTORATION:
		{
			if (restore_level()) ident = TRUE;
			if (do_res_stat(A_STR)) ident = TRUE;
			if (do_res_stat(A_INT)) ident = TRUE;
			if (do_res_stat(A_WIS)) ident = TRUE;
			if (do_res_stat(A_DEX)) ident = TRUE;
			if (do_res_stat(A_CON)) ident = TRUE;
			if (do_res_stat(A_CHR)) ident = TRUE;
			break;
		}

		case SV_ROD_SPEED:
		{
			if (set_fast(randint1(30) + (powerful ? 30 : 15), FALSE)) ident = TRUE;
			break;
		}

		case SV_ROD_PESTICIDE:
		{
			if (dispel_monsters(powerful ? 8 : 4)) ident = TRUE;
			break;
		}

		case SV_ROD_TELEPORT_AWAY:
		{
			int distance = MAX_SIGHT * (powerful ? 8 : 5);
			if (teleport_monster(dir, distance)) ident = TRUE;
			break;
		}

		case SV_ROD_DISARMING:
		{
			if (disarm_trap(dir)) ident = TRUE;
			if (powerful && disarm_traps_touch()) ident = TRUE;
			break;
		}

		case SV_ROD_LITE:
		{
			int dam = damroll((powerful ? 12 : 6), 8);
#ifdef JP
			msg_print("���P�������������ꂽ�B");
#else
			msg_print("A line of blue shimmering light appears.");
#endif

			(void)lite_line(dir, dam);
			ident = TRUE;
			break;
		}

		case SV_ROD_SLEEP_MONSTER:
		{
			//v1.1.90 +30
			if (sleep_monster(dir, lev+30)) ident = TRUE;
			break;
		}

		case SV_ROD_SLOW_MONSTER:
		{
			//v1.1.90 +30
			if (slow_monster(dir, lev+30)) ident = TRUE;
			break;
		}

		case SV_ROD_DRAIN_LIFE:
		{
			if (drain_life(dir, 70 + 3 * lev / 2)) ident = TRUE;
			break;
		}

		case SV_ROD_POLYMORPH:
		{
			if (poly_monster(dir, lev)) ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BOLT:
		{
			fire_bolt_or_beam(10, GF_ACID, dir, damroll(6 + lev / 7, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BOLT:
		{
			fire_bolt_or_beam(10, GF_ELEC, dir, damroll(4 + lev / 9, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BOLT:
		{
			fire_bolt_or_beam(10, GF_FIRE, dir, damroll(7 + lev / 6, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BOLT:
		{
			fire_bolt_or_beam(10, GF_COLD, dir, damroll(5 + lev / 8, 8));
			ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 60 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 40 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 70 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 50 + lev, rad);
			ident = TRUE;
			break;
		}

		case SV_ROD_HAVOC:
		{
			call_chaos();
			ident = TRUE;
			break;
		}

		case SV_ROD_STONE_TO_MUD:
		{
			int dam = powerful ? 40 + randint1(60) : 20 + randint1(30);
			if (wall_to_mud(dir, dam)) ident = TRUE;
			break;
		}

		case SV_ROD_AGGRAVATE:
		{
			aggravate_monsters(0,TRUE);
			ident = TRUE;
			break;
		}
	}
	return ident;
}

/*
 * Activate (zap) a Rod
 *
 * Unstack fully charged rods as needed.
 *
 * Hack -- rods of perception/genocide can be "cancelled"
 * All rods can be cancelled at the "Direction?" prompt
 *
 * pvals are defined for each rod in k_info. -LM-
 */
/*:::���b�h��U��ڍ׏���*/
static void do_cmd_zap_rod_aux(int item)
{
	int ident, chance, lev, fail;
	int dir = 0;
	object_type *o_ptr;
	bool success;
	bool powerful = FALSE;

	/* Hack -- let perception get aborted */
	bool use_charge = TRUE;

	object_kind *k_ptr;

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


	/* Mega-Hack -- refuse to zap a pile from the ground */
	if ((item < 0) && (o_ptr->number > 1))
	{
#ifdef JP
		msg_print("�܂��̓��b�h���E��Ȃ���΁B");
#else
		msg_print("You must first pick up the rods.");
#endif

		return;
	}


	/* Get a direction (unless KNOWN not to need it) */
	if (((o_ptr->sval >= SV_ROD_MIN_DIRECTION) && (o_ptr->sval != SV_ROD_HAVOC) && (o_ptr->sval != SV_ROD_AGGRAVATE) && (o_ptr->sval != SV_ROD_PESTICIDE)) ||
	     !object_is_aware(o_ptr))
	{
		/* Get a direction, allow cancel */
		if (!get_aim_dir(&dir)) return;
	}


	/* Take a turn */
	energy_use = 100;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	fail = lev+5;
	if (chance > fail) fail -= (chance - fail)*2;
	else chance -= (fail - chance)*2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");
#else
		msg_print("Nothing happen. Maybe this rod is freezing too.");
#endif

		sound(SOUND_FAIL);
		return;
	}

	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	/* Roll for usage */
	if (!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���܂����b�h���g���Ȃ������B");
#else
		msg_print("You failed to use the rod properly.");
#endif
		///mod140829 �t�����͂�������F�X��
		if(p_ptr->pclass == CLASS_FLAN && one_in_(13))
		{
			object_type forge;
			object_type *q_ptr = &forge;
			object_copy(q_ptr, o_ptr);
			distribute_charges(o_ptr, q_ptr, 1);

			msg_print("���b�h����̒��Ŕ��������I");
			if(item >= 0)
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
		}
		sound(SOUND_FAIL);
		return;
	}

	k_ptr = &k_info[o_ptr->k_idx];

	/* A single rod is still charging */
	if ((o_ptr->number == 1) && (o_ptr->timeout))
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���̃��b�h�͂܂����͂��[�U���Ă���Œ����B");
#else
		msg_print("The rod is still charging.");
#endif

		return;
	}
	/* A stack of rods lacks enough energy. */
	else if ((o_ptr->number > 1) && (o_ptr->timeout > k_ptr->pval * (o_ptr->number - 1)))
	{
		if (flush_failure) flush();
#ifdef JP
msg_print("���̃��b�h�͂܂��[�U���ł��B");
#else
		msg_print("The rods are all still charging.");
#endif

		return;
	}

	/* Sound */
	sound(SOUND_ZAP);

	if(check_activated_nameless_arts(JKF1_MASTER_MAGITEM)) powerful = TRUE;

	ident = rod_effect(o_ptr->sval, dir, &use_charge, powerful, FALSE);

	/* Increase the timeout by the rod kind's pval. -LM- */
	if (use_charge) o_ptr->timeout += k_ptr->pval;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	///del131214 virtue
	//if (!(object_is_aware(o_ptr)))
	//{
	//	chg_virtue(V_PATIENCE, -1);
	//	chg_virtue(V_CHANCE, 1);
	//	chg_virtue(V_KNOWLEDGE, -1);
	//}

	/* Tried the object */
	object_tried(o_ptr);

	/* Successfully determined the object function */
	if (ident && !object_is_aware(o_ptr))
	{
		object_aware(o_ptr);
		gain_exp((lev + (p_ptr->lev >> 1)) / p_ptr->lev);
		if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);

	}

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
}

/*:::���b�h��U��*/
void do_cmd_zap_rod(void)
{
	int item;
	cptr q, s;

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_MAGICITEM))
	{
		msg_print("���܂����b�h�����ĂȂ��B");
		return ;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	/* Restrict choices to rods */
	item_tester_tval = TV_ROD;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃��b�h��U��܂���? ";
	s = "�g���郍�b�h���Ȃ��B";
#else
	q = "Zap which rod? ";
	s = "You have no rod to zap.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Zap the rod */
	do_cmd_zap_rod_aux(item);
}


/*
 * Hook to determine if an object is activatable
 */
/*:::�����\�ȃA�C�e���𔻒�*/
//v1.1.46 �����̎w�֔����Ŏg�����߂�static���O����extern����
bool item_tester_hook_activate(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	/* Not known */
	/*:::���Ӓ�A�C�e���͔����ł��Ȃ�*/
	if (!object_is_known(o_ptr)) return (FALSE);

	///mod160504 �e�͔����łȂ��ˌ����爵�����Ƃɂ���
	if (o_ptr->tval == TV_GUN) return (FALSE);

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Check activation flag */
	if (have_flag(flgs, TR_ACTIVATE)) return (TRUE);

	/* Assume not */
	return (FALSE);
}


/*
 * Hack -- activate the ring of power
 */
/*:::�M������������*/
void ring_of_power(int dir)
{
	/* Pick a random effect */
	switch (randint1(10))
	{
		case 1:
		case 2:
		{

			/* Message */
#ifdef JP
			msg_print("���Ȃ��͈����̃I�[���ɕ�ݍ��܂ꂽ�B");
#else
			msg_print("You are surrounded by a malignant aura.");
#endif
			if(p_ptr->pclass == CLASS_HINA)
			{
				msg_format("�������w�ւ̎􂢂��z��������I");
				hina_gain_yaku(500 + randint1(500));
				return;
			}
			//v1.1.48 �����͎􂢂�s�^�p���[�Ƃ��ċz������
			if (p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͋���Ȏ􂢂��z�������I");
				p_ptr->magic_num1[1] += 25000 + randint1(25000);
				return ;
			}


			sound(SOUND_EVIL);

			/* Decrease all stats (permanently) */
			(void)dec_stat(A_STR, 50, TRUE);
			(void)dec_stat(A_INT, 50, TRUE);
			(void)dec_stat(A_WIS, 50, TRUE);
			(void)dec_stat(A_DEX, 50, TRUE);
			(void)dec_stat(A_CON, 50, TRUE);
			(void)dec_stat(A_CHR, 50, TRUE);

			/* Lose some experience (permanently) */
			p_ptr->exp -= (p_ptr->exp / 4);
			p_ptr->max_exp -= (p_ptr->exp / 4);
			check_experience();

			break;
		}

		case 3:
		{
			/* Message */
#ifdef JP
			msg_print("���Ȃ��͋��͂ȃI�[���ɕ�ݍ��܂ꂽ�B");
#else
			msg_print("You are surrounded by a powerful aura.");
#endif


			/* Dispel monsters */
			dispel_monsters(1000);

			break;
		}

		case 4:
		case 5:
		case 6:
		{
			/* Mana Ball */
			fire_ball(GF_MANA, dir, 600, 3);

			break;
		}

		case 7:
		case 8:
		case 9:
		case 10:
		{
			/* Mana Bolt */
			fire_bolt(GF_MANA, dir, 500);

			break;
		}
	}
}


bool ang_sort_comp_pet(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	monster_type *m_ptr1 = &m_list[w1];
	monster_type *m_ptr2 = &m_list[w2];
	monster_race *r_ptr1 = &r_info[m_ptr1->r_idx];
	monster_race *r_ptr2 = &r_info[m_ptr2->r_idx];

	/* Unused */
	(void)v;

	if (m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if (m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if ((r_ptr1->flags1 & RF1_UNIQUE) && !(r_ptr2->flags1 & RF1_UNIQUE)) return TRUE;
	if ((r_ptr2->flags1 & RF1_UNIQUE) && !(r_ptr1->flags1 & RF1_UNIQUE)) return FALSE;

	if (r_ptr1->level > r_ptr2->level) return TRUE;
	if (r_ptr2->level > r_ptr1->level) return FALSE;

	if (m_ptr1->hp > m_ptr2->hp) return TRUE;
	if (m_ptr2->hp > m_ptr1->hp) return FALSE;
	
	return w1 <= w2;
}



/*
 * Activate a wielded object.  Wielded objects never stack.
 * And even if they did, activatable objects never stack.
 *:::�������̃A�C�e���𔭓�����B�����\�ȃA�C�e���͑������łȂ��Ă��܂Ƃ߂��Ȃ��B
 *
 * Currently, only (some) artifacts, and Dragon Scale Mail, can be activated.
 * But one could, for example, easily make an activatable "Ring of Plasma".
 *:::(�Â��L�q�炵���B����)
 *
 * Note that it always takes a turn to activate an artifact, even if
 * the user hits "escape" at the "direction" prompt.
 *:::������A�����w�莞�ɃL�����Z�������Ƃ��Ă��^�[���������B
 */
/*:::�A�C�e�������i�����\�ȃA�C�e����I����j*/
//v1.1.46 �����̎w�֔�������g�����߂�static�O����extern�����B
//item�����̏ꍇinven_add[]����A�C�e�������擾���Ĕ������邱�Ƃɂ���
void do_cmd_activate_aux(int item)
{
	int         dir, lev, chance, fail;
	object_type *o_ptr;
	bool success;

	/*:::����̃A�C�e�����������Ă邪�����i�ł��������ɂ͗��Ȃ�*/

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		//v1.1.46 HACK - item���}�C�i�X�̂Ƃ�����łȂ��ǉ��C���x���g��inven_add[]����A�C�e�������擾���邱�Ƃɂ����B
		o_ptr = &inven_add[-1 - item];
	}

	/* Take a turn */
	energy_use = 100;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].level;

	/* Hack -- use artifact level instead */
	if (object_is_fixed_artifact(o_ptr)) lev = a_info[o_ptr->name1].level;
	else if (object_is_random_artifact(o_ptr))
	{
		/*:::���X�����\�ȃA�C�e�������ɂȂ����ꍇ�A���i��k_info[]��level���g���邪�I���W�i���̔������t�����ꂽ�ꍇact_ptr��FALSE�łȂ��Ȃ肱���ŏ�񂪏㏑�������炵��*/
		const activation_type* const act_ptr = find_activation_info(o_ptr);
		if (act_ptr) {
			lev = act_ptr->level;
		}
	}
	else if (((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)) && o_ptr->name2) lev = e_info[o_ptr->name2].level;
	
	/*:::���������v�Z*/
	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	fail = lev+5;
	if (chance > fail) fail -= (chance - fail)*2;
	else chance -= (fail - chance)*2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("�~�܂������̒��ł͂��܂������Ȃ��悤���B");
#else
		msg_print("It shows no reaction.");
#endif
		sound(SOUND_FAIL);
		return;
	}
	if (cheat_xtra) msg_format("ACTIVATE: k_idx:%d lev:%d chance:%d fail:%d", o_ptr->k_idx,lev, chance, fail);
	///sys class ����m�͕K�����������s����
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance*2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail*2) < chance) success = TRUE;
		else success = FALSE;
	}

	///mod140821 ���ꏈ���@�G���W�j�A�͕K���N�����]���̔����ɐ�������
	///mod160503 �p�~
	//if(p_ptr->pclass == CLASS_ENGINEER && o_ptr->name1 == ART_CRIMSON) success = TRUE;

	//�����x���ȕt�^�n�C���C�W�͕K����������
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_ENCHANT && p_ptr->lev > 39) success = TRUE;

	///mod150910 �N���E���s�[�X�͕K�������̔����ɐ�������
	if(p_ptr->pclass == CLASS_CLOWNPIECE && o_ptr->name1 == ART_CLOWNPIECE) success = TRUE;
	if (p_ptr->pclass == CLASS_VFS_CLOWNPIECE && o_ptr->name1 == ART_CLOWNPIECE) success = TRUE;


	/* Roll for usage */
	if (!success)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_print("���܂��n�������邱�Ƃ��ł��Ȃ������B");
#else
		msg_print("You failed to activate it properly.");
#endif
		sound(SOUND_FAIL);
		return;
	}

	/* Check the recharge */
	if (o_ptr->timeout)
	{
#ifdef JP
		msg_print("����͔����ɉ��𗧂āA�P���A������...");
#else
		msg_print("It whines, glows and fades...");
#endif
		return;
	}

	/* Some lights need enough fuel for activation */
	/*:::�����܂⃉���^���̔����t�A�C�e���́A�������ɔR��������H
	/*:::�E�E�͂��Ȃ��B�R��0�ł������ł���Btval�̎w����ԈႦ�Ă�񂶂�Ȃ��낤���B�o�O�H*/
	///item TVAL �����^���Ȃǂ̔������R������H
	if (!o_ptr->xtra4 && (o_ptr->tval == TV_FLASK) &&
		((o_ptr->sval == SV_LITE_TORCH) || (o_ptr->sval == SV_LITE_LANTERN)))
	{
#ifdef JP
		msg_print("�R�����Ȃ��B");
#else
		msg_print("It has no fuel.");
#endif
		energy_use = 0;
		return;
	}

	/* Activate the artifact */
	msg_print("�n��������...");

	/* Sound */
	sound(SOUND_ZAP);

	/* Activate object */
	if (activation_index(o_ptr))
	{
		/*:::�A�C�e���������ʁ@����p�֐��Ƃ����킯�ł͂Ȃ�*/
		(void)activate_random_artifact(o_ptr, item);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return;
	}

	/* Special items */
	/*:::���@�̓J�ƃ����X�^�[�{�[���̋L�q�͂���*/
	else if (o_ptr->tval == TV_WHISTLE)
	{
		if (music_singing_any() && !CLASS_IS_PRISM_SISTERS) stop_singing();
		if (hex_spelling_any()) stop_hex_spell_all();

#if 0
		if (object_is_cursed(o_ptr))
		{
#ifdef JP
			msg_print("�J���������������n�����B");
#else
			msg_print("You produce a shrill whistling sound.");
#endif
			aggravate_monsters(0,TRUE);
		}
		else
#endif
		{
			int pet_ctr, i;
			u16b *who;
			int max_pet = 0;
			u16b dummy_why;

			/* Allocate the "who" array */
			C_MAKE(who, max_m_idx, u16b);

			/* Process the monsters (backwards) */
			for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				if (is_pet(&m_list[pet_ctr]) && (p_ptr->riding != pet_ctr))
				  who[max_pet++] = pet_ctr;
			}

			/* Select the sort method */
			ang_sort_comp = ang_sort_comp_pet;
			ang_sort_swap = ang_sort_swap_hook;

			ang_sort(who, &dummy_why, max_pet);

			/* Process the monsters (backwards) */
			for (i = 0; i < max_pet; i++)
			{
				pet_ctr = who[i];
				teleport_monster_to(pet_ctr, py, px, 100, TELEPORT_PASSIVE);
			}

			/* Free the "who" array */
			C_KILL(who, max_m_idx, u16b);
		}
		o_ptr->timeout = 100+randint1(100);
		return;
	}
	else if (o_ptr->tval == TV_CAPTURE)
	{
		/*:::�����X�^�[�{�[���i�ߊl���j*/
		if(!o_ptr->pval)
		{
			bool old_target_pet = target_pet;
			target_pet = TRUE;
			if (!get_aim_dir(&dir))
			{
				target_pet = old_target_pet;
				return;
			}
			target_pet = old_target_pet;

			if(fire_ball(GF_CAPTURE, dir, 0, 0))
			{
				/*:::�����X�^�[�̃p�����[�^�������X�^�[�{�[���Ɋi�[�@��p�O���[�o���ϐ��g�p*/
				o_ptr->pval = cap_mon;
				o_ptr->xtra3 = cap_mspeed;
				o_ptr->xtra4 = cap_hp;
				o_ptr->xtra5 = cap_maxhp;
				if (cap_nickname)
				{
					cptr t;
					char *s;
					char buf[160] = "";

					if (o_ptr->inscription)
						strcpy(buf, quark_str(o_ptr->inscription));
					s = buf;
					for (s = buf;*s && (*s != '#'); s++)
					{
#ifdef JP
						if (iskanji(*s)) s++;
#endif
					}
					*s = '#';
					s++;
#ifdef JP
 /*nothing*/
#else
					*s++ = '\'';
#endif
					t = quark_str(cap_nickname);
					while (*t)
					{
						*s = *t;
						s++;
						t++;
					}
#ifdef JP
 /*nothing*/
#else
					*s++ = '\'';
#endif
					*s = '\0';
					o_ptr->inscription = quark_add(buf);
				}
			}
		}
		/*:::�����X�^�[�{�[���i������j*/
		else
		{
			bool release_success = FALSE;
			//v1.1.16 �d���{�p�����[�^�s���ɑΉ�
			if(o_ptr->pval < 0 || o_ptr->pval >= max_r_idx)
			{
				msg_format("ERROR:�d���{��pval�l����������(%d)",o_ptr->pval);
				return;
			}

			if (!get_rep_dir2(&dir)) return;

			if (monster_can_enter(py + ddy[dir], px + ddx[dir], &r_info[o_ptr->pval], 0))
			{
				if (place_monster_aux(0, py + ddy[dir], px + ddx[dir], o_ptr->pval, (PM_FORCE_PET | PM_NO_KAGE)))
				{
					if (o_ptr->xtra3) m_list[hack_m_idx_ii].mspeed = o_ptr->xtra3;
					if (o_ptr->xtra5) m_list[hack_m_idx_ii].max_maxhp = o_ptr->xtra5;
					if (o_ptr->xtra4) m_list[hack_m_idx_ii].hp = o_ptr->xtra4;
					m_list[hack_m_idx_ii].maxhp = m_list[hack_m_idx_ii].max_maxhp;
					if (o_ptr->inscription)
					{
						char buf[160];
						cptr t;
#ifndef JP
						bool quote = FALSE;
#endif

						t = quark_str(o_ptr->inscription);
						for (t = quark_str(o_ptr->inscription);*t && (*t != '#'); t++)
						{
#ifdef JP
							if (iskanji(*t)) t++;
#endif
						}
						if (*t)
						{
							char *s = buf;
							t++;
#ifdef JP
							/* nothing */
#else
							if (*t =='\'')
							{
								t++;
								quote = TRUE;
							}
#endif
							while(*t)
							{
								*s = *t;
								t++;
								s++;
							}
#ifdef JP
							/* nothing */
#else
							if (quote && *(s-1) =='\'')
								s--;
#endif
							*s = '\0';
							m_list[hack_m_idx_ii].nickname = quark_add(buf);
							t = quark_str(o_ptr->inscription);
							s = buf;
							while(*t && (*t != '#'))
							{
								*s = *t;
								t++;
								s++;
							}
							*s = '\0';
							o_ptr->inscription = quark_add(buf);
						}
					}
					o_ptr->pval = 0;
					o_ptr->xtra3 = 0;
					o_ptr->xtra4 = 0;
					o_ptr->xtra5 = 0;
					release_success = TRUE;
				}
			}
			if (!release_success)
#ifdef JP
				msg_print("�����ƁA����Ɏ��s�����B");
#else
				msg_print("Oops.  You failed to release your pet.");
#endif
		}
		return;
	}

	/* Mistake */
#ifdef JP
	msg_print("�����ƁA���̃A�C�e���͎n���ł��Ȃ��B");
#else
	msg_print("Oops.  That object cannot be activated.");
#endif

}

/*:::�A�C�e���𔭓�*/
void do_cmd_activate(void)
{
	int     item;
	cptr    q, s;


	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		msg_print("���̎p�ł̓A�C�e���̔������ł��Ȃ��B");
		return ;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Prepare the hook */
	item_tester_hook = item_tester_hook_activate;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�����n�������܂���? ";
	s = "�n���ł���A�C�e���𑕔����Ă��Ȃ��B";
#else
	q = "Activate which item? ";
	s = "You have nothing to activate.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP))) return;

	/* Activate the item */
	do_cmd_activate_aux(item);
}


/*
 * Hook to determine if an object is useable
 */
static bool item_tester_hook_use(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	/* Ammo */
	//if (o_ptr->tval == p_ptr->tval_ammo)
	//	return (TRUE);



	//v1.1.38
	object_flags(o_ptr, flgs);
	if(have_flag(flgs,TR_USABLE))
		return TRUE;


	/* Useable object */
	switch (o_ptr->tval)
	{
		//case TV_SPIKE:
		case TV_STAFF:
		case TV_WAND:
		case TV_ROD:
		case TV_SCROLL:
		case TV_POTION:
		case TV_FOOD:
		{
			return (TRUE);
		}
		//v1.1.86
		case TV_ABILITY_CARD:
		{
			if (ability_card_list[o_ptr->pval].activate) return TRUE;
			else return FALSE;
		}

		default:
		{
			int i;

			/* Not known */
			if (!object_is_known(o_ptr)) return (FALSE);

			/* HACK - only items from the equipment can be activated */
			for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
			{
				if (&inventory[i] == o_ptr)
				{
					/* Extract the flags */
					//object_flags(o_ptr, flgs); //��ɃR�s�[����

					/* Check activation flag */
					//mod160504 �e�͏���
					if (o_ptr->tval != TV_GUN && have_flag(flgs, TR_ACTIVATE)) return (TRUE);
				}
			}
		}
	}

	/* Assume not */
	return (FALSE);
}


/*
 * Use an item
 * XXX - Add actions for other item types
 */
void do_cmd_use(void)
{
	int         item;
	object_type *o_ptr;
	cptr        q, s;
	u32b flgs[TR_FLAG_SIZE];

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Prepare the hook */
	item_tester_hook = item_tester_hook_use;

	/* Get an item */
#ifdef JP
q = "�ǂ���g���܂����H";
s = "�g������̂�����܂���B";
#else
	q = "Use which item? ";
	s = "You have nothing to use.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return;

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


	/* Ammo */
	//if (o_ptr->tval == p_ptr->tval_ammo)
	//	return (TRUE);

	//v1.1.38
	object_flags(o_ptr, flgs);
	if(have_flag(flgs,TR_USABLE))
	{

		do_cmd_use_special_item(item);
		return;
	}

	switch (o_ptr->tval)
	{
		/* Spike a door */
		//�����т�łR�}���h���폜
	/*		
		case TV_SPIKE:
		{

			do_cmd_spike();
			break;
		}
	*/
		/* Eat some food */
		case TV_FOOD:
		{
			do_cmd_eat_food_aux(item);
			break;
		}

		/* Aim a wand */
		case TV_WAND:
		{
			do_cmd_aim_wand_aux(item);
			break;
		}

		/* Use a staff */
		case TV_STAFF:
		{
			do_cmd_use_staff_aux(item);
			break;
		}

		/* Zap a rod */
		case TV_ROD:
		{
			do_cmd_zap_rod_aux(item);
			break;
		}

		case TV_ABILITY_CARD:
		{
			do_cmd_use_special_item(item);
			break;

		}

		/* Quaff a potion */
		case TV_POTION:
		{
			do_cmd_quaff_potion_aux(item,FALSE);
			break;
		}

		/* Read a scroll */
		case TV_SCROLL:
		{
			/* Check some conditions */
			if (p_ptr->blind)
			{
#ifdef JP
msg_print("�ڂ������Ȃ��B");
#else
				msg_print("You can't see anything.");
#endif

				return;
			}
			if (no_lite())
			{
#ifdef JP
msg_print("�����肪�Ȃ��̂ŁA�Â��ēǂ߂Ȃ��B");
#else
				msg_print("You have no light to read by.");
#endif

				return;
			}
			if (p_ptr->confused)
			{
#ifdef JP
msg_print("�������Ă��ēǂ߂Ȃ��I");
#else
				msg_print("You are too confused!");
#endif

				return;
			}
			if(p_ptr->pclass == CLASS_SAGUME)
				sagume_read_scroll_aux(item, object_is_aware(o_ptr));
			else
			  do_cmd_read_scroll_aux(item, TRUE);
		  break;
		}

		/* Fire ammo */
		///mod140308 u�R�}���h�ł̎ˌ��p�~
		/*
		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			do_cmd_fire_aux(item, &inventory[INVEN_BOW]);
			break;
		}
		*/
		/* Activate an artifact */
		default:
		{
			do_cmd_activate_aux(item);
			break;
		}
	}
}

/*:::������p�t�@�z�������A�C�e������I������*/
///mod150401 �[�U�t���O��t����extern
int select_magic_eater(bool only_browse, bool recharge)
{
	int ext=0;
	char choice;
	bool flag, request_list;
	int tval = 0;
	int             ask = TRUE, i = 0;
	char            out_val[160];

	int menu_line = (use_menu ? 1 : 0);

#ifdef ALLOW_REPEAT
	int sn;
	if (repeat_pull(&sn) )
	{
		if(recharge) return sn;//�ǉ�

		/* Verify the spell */
		if (sn >= EATER_EXT*2 && !(p_ptr->magic_num1[sn] > k_info[lookup_kind(TV_ROD, sn-EATER_EXT*2)].pval * (p_ptr->magic_num2[sn] - 1) * EATER_ROD_CHARGE))
			return sn;
		else if (sn < EATER_EXT*2 && !(p_ptr->magic_num1[sn] < EATER_CHARGE))
			return sn;
	}
	
#endif /* ALLOW_REPEAT */

	for (i = 0; i < (EATER_EXT * 3); i++)
	{
		if (p_ptr->magic_num2[i]) break;
	}
	if (i == (EATER_EXT * 3))
	{
#ifdef JP
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("��������B�������Ă��Ȃ��I");
		else
			msg_print("���@���o���Ă��Ȃ��I");
#else
		msg_print("You don't have any magic!");
#endif
		return -1;
	}

	if (use_menu)
	{
		screen_save();

		while(!tval)
		{
#ifdef JP
			prt(format(" %s ��", (menu_line == 1) ? "�t" : "  "), 2, 14);
			prt(format(" %s ���@�_", (menu_line == 2) ? "�t" : "  "), 3, 14);
			prt(format(" %s ���b�h", (menu_line == 3) ? "�t" : "  "), 4, 14);
#else
			prt(format(" %s staff", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s wand", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s rod", (menu_line == 3) ? "> " : "  "), 4, 14);
#endif

			if(p_ptr->pclass == CLASS_SEIJA)
			{
				if (only_browse) prt(_("�ǂ̎�ނ̖���������܂����H", "Which type of magic do you browse?"), 0, 0);
				else if (recharge) prt(_("�ǂ̎�ނ̖�������[�U���܂����H", "Which type of magic do you browse?"), 0, 0);
				else prt(_("�ǂ̎�ނ̖�������g���܂����H", "Which type of magic do you use?"), 0, 0);
			}
			else
			{
				if (only_browse) prt(_("�ǂ̎�ނ̖��@�����܂����H", "Which type of magic do you browse?"), 0, 0);
				else prt(_("�ǂ̎�ނ̖��@���g���܂����H", "Which type of magic do you use?"), 0, 0);
			}
			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return -1;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 2;
				break;
			case '\r':
			case 'x':
			case 'X':
				ext = (menu_line-1)*EATER_EXT;
				if (menu_line == 1) tval = TV_STAFF;
				else if (menu_line == 2) tval = TV_WAND;
				else tval = TV_ROD;
				break;
			}
			if (menu_line > 3) menu_line -= 3;
		}
		screen_load();
	}
	else
	{
	while (TRUE)
	{
#ifdef JP
		if (!get_com("[A] ��, [B] ���@�_, [C] ���b�h:", &choice, TRUE))
#else
		if (!get_com("[A] staff, [B] wand, [C] rod:", &choice, TRUE))
#endif
		{
			return -1;
		}
		if (choice == 'A' || choice == 'a')
		{
			ext = 0;
			tval = TV_STAFF;
			break;
		}
		if (choice == 'B' || choice == 'b')
		{
			ext = EATER_EXT;
			tval = TV_WAND;
			break;
		}
		if (choice == 'C' || choice == 'c')
		{
			ext = EATER_EXT*2;
			tval = TV_ROD;
			break;
		}
	}
	}
	for (i = ext; i < ext + EATER_EXT; i++)
	{
		if (p_ptr->magic_num2[i])
		{
			if (use_menu) menu_line = i-ext+1;
			break;
		}
	}
	if (i == ext+EATER_EXT)
	{
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("���̎�ނ̖�����������Ă��Ȃ��I");
		else
			msg_print("���̎�ނ̖��@�͊o���Ă��Ȃ��I");
		return -1;
	}

	/* Nothing chosen yet */
	flag = FALSE;

	if(p_ptr->pclass == CLASS_SEIJA)
	{
		if (only_browse) strnfmt(out_val, 78, _("('*'�ňꗗ, ESC�Œ��f) �ǂ̖���������܂����H",	"(*=List, ESC=exit) Browse which power? "));
		else if (recharge) strnfmt(out_val, 78, _("('*'�ňꗗ, ESC�Œ��f) �ǂ̖�������[�U���܂����H",	"(*=List, ESC=exit) Browse which power? "));
		else strnfmt(out_val, 78, _("('*'�ňꗗ, ESC�Œ��f) �ǂ̖�������g���܂����H","(*=List, ESC=exit) Use which power? "));

	}
	else
	{
		if (only_browse) strnfmt(out_val, 78, _("('*'�ňꗗ, ESC�Œ��f) �ǂ̖��͂����܂����H","(*=List, ESC=exit) Browse which power? "));
		else strnfmt(out_val, 78, _("('*'�ňꗗ, ESC�Œ��f) �ǂ̖��͂��g���܂����H","(*=List, ESC=exit) Use which power? "));
	}

	/* Build a prompt */
	
	/* Save the screen */
	screen_save();

	request_list = always_show_list;

	/* Get a spell from the user */
	while (!flag)
	{
		/* Show the list */
		if (request_list || use_menu)
		{
			byte y, x = 0;
			int ctr, chance;
			int k_idx;
			char dummy[80];
			int x1, y1, level;
			byte col;

			strcpy(dummy, "");

			for (y = 1; y < 20; y++)
				prt("", y, x);

			y = 1;

			/* Print header(s) */
#ifdef JP
			prt(format("                           %s ����                           %s ����", (tval == TV_ROD ? "  ���  " : "�g�p��"), (tval == TV_ROD ? "  ���  " : "�g�p��")), y++, x);
#else
			prt(format("                           %s Fail                           %s Fail", (tval == TV_ROD ? "  Stat  " : " Charges"), (tval == TV_ROD ? "  Stat  " : " Charges")), y++, x);
#endif

			/* Print list */
			for (ctr = 0; ctr < EATER_EXT; ctr++)
			{
				if (!p_ptr->magic_num2[ctr+ext]) continue;

				k_idx = lookup_kind(tval, ctr);

				if (use_menu)
				{
					if (ctr == (menu_line-1))
#ifdef JP
						strcpy(dummy, "�t");
#else
					strcpy(dummy, "> ");
#endif
					else strcpy(dummy, "  ");
						
				}
				/* letter/number for power selection */
				else
				{
					char letter;
					if (ctr < 26)
						letter = I2A(ctr);
					else
						letter = '0' + ctr - 26;
					sprintf(dummy, "%c)",letter);
				}
				x1 = ((ctr < EATER_EXT/2) ? x : x + 40);
				y1 = ((ctr < EATER_EXT/2) ? y + ctr : y + ctr - EATER_EXT/2);
				level = (tval == TV_ROD ? k_info[k_idx].level * 5 / 6 - 5 : k_info[k_idx].level);
				chance = level * 4 / 5 + 20;
				chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);
				level /= 2;
				if (p_ptr->lev > level)
				{
					chance -= 3 * (p_ptr->lev - level);
				}
				chance = mod_spell_chance_1(chance);
				chance = MAX(chance, adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]]);
				/* Stunning makes spells harder */
				if (p_ptr->stun > 50) chance += 25;
				else if (p_ptr->stun) chance += 15;

				if (chance > 95) chance = 95;

				chance = mod_spell_chance_2(chance);

				col = TERM_WHITE;

				if (k_idx)
				{
					if (tval == TV_ROD)
					{
						strcat(dummy, format(
#ifdef JP
							       " %-22.22s �[�U:%2d/%2d%3d%%",
#else
							       " %-22.22s   (%2d/%2d) %3d%%",
#endif
							       k_name + k_info[k_idx].name, 
							       p_ptr->magic_num1[ctr+ext] ? 
							       (p_ptr->magic_num1[ctr+ext] - 1) / (EATER_ROD_CHARGE * k_info[k_idx].pval) +1 : 0, 
							       p_ptr->magic_num2[ctr+ext], chance));
						if (p_ptr->magic_num1[ctr+ext] > k_info[k_idx].pval * (p_ptr->magic_num2[ctr+ext]-1) * EATER_ROD_CHARGE) col = TERM_RED;
					}
					else
					{
						strcat(dummy, format(" %-22.22s    %2d/%2d %3d%%", k_name + k_info[k_idx].name, (s16b)(p_ptr->magic_num1[ctr+ext]/EATER_CHARGE), p_ptr->magic_num2[ctr+ext], chance));
						if (p_ptr->magic_num1[ctr+ext] < EATER_CHARGE) col = TERM_RED;
					}
				}
				else
					strcpy(dummy, "");
				c_prt(col, dummy, y1, x1);
			}
		}

		if (!get_com(out_val, &choice, FALSE)) break;

		if (use_menu && choice != ' ')
		{
			switch (choice)
			{
				case '0':
				{
					screen_load();
					return 0;
				}

				case '8':
				case 'k':
				case 'K':
				{
					do
					{
						menu_line += EATER_EXT - 1;
						if (menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!p_ptr->magic_num2[menu_line+ext-1]);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					do
					{
						menu_line++;
						if (menu_line > EATER_EXT) menu_line -= EATER_EXT;
					} while(!p_ptr->magic_num2[menu_line+ext-1]);
					break;
				}

				case '4':
				case 'h':
				case 'H':
				case '6':
				case 'l':
				case 'L':
				{
					bool reverse = FALSE;
					if ((choice == '4') || (choice == 'h') || (choice == 'H')) reverse = TRUE;
					if (menu_line > EATER_EXT/2)
					{
						menu_line -= EATER_EXT/2;
						reverse = TRUE;
					}
					else menu_line+=EATER_EXT/2;
					while(!p_ptr->magic_num2[menu_line+ext-1])
					{
						if (reverse)
						{
							menu_line--;
							if (menu_line < 2) reverse = FALSE;
						}
						else
						{
							menu_line++;
							if (menu_line > EATER_EXT-1) reverse = TRUE;
						}
					}
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
		}

		/* Request redraw */
		if (use_menu && ask) continue;

		/* Request redraw */
		if (!use_menu && ((choice == ' ') || (choice == '*') || (choice == '?')))
		{
			/* Hide the list */
			if (request_list)
			{
				/* Hide list */
				request_list = FALSE;
				
				/* Restore the screen */
				screen_load();
				screen_save();
			}
			else
				request_list = TRUE;

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
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
		if ((i < 0) || (i > EATER_EXT) || !p_ptr->magic_num2[i+ext])
		{
			bell();
			continue;
		}

		if (!only_browse)
		{
			/* Verify it */
			if (ask)
			{
				char tmp_val[160];

				/* Prompt */
#ifdef JP
				(void) strnfmt(tmp_val, 78, "%s���g���܂����H ", k_name + k_info[lookup_kind(tval ,i)].name);
#else
				(void) strnfmt(tmp_val, 78, "Use %s?", k_name + k_info[lookup_kind(tval ,i)].name);
#endif

				/* Belay that order */
				if (!get_check(tmp_val)) continue;
			}
			if (tval == TV_ROD)
			{
				if (p_ptr->magic_num1[ext+i]  > k_info[lookup_kind(tval, i)].pval * (p_ptr->magic_num2[ext+i] - 1) * EATER_ROD_CHARGE)
				{
					if(!recharge)
					{

						if(p_ptr->pclass == CLASS_SEIJA)
							msg_print("���̖�����͂܂��g���Ȃ��B");
						else
							msg_print("���̖��@�͂܂��[�U���Ă���Œ����B");
						msg_print(NULL);
						if (use_menu) ask = TRUE;
						continue;
					}
				}
			}
			else
			{
				if (p_ptr->magic_num1[ext+i] < EATER_CHARGE)
				{

					if(!recharge)
					{

						if(p_ptr->pclass == CLASS_SEIJA)
							msg_print("���̖�����͎g�p�񐔂��؂�Ă���B");
						else
							msg_print("���̖��@�͎g�p�񐔂��؂�Ă���B");
						msg_print(NULL);
						if (use_menu) ask = TRUE;
						continue;
					}
				}
			}
		}

		/* Browse */
		else
		{
			int line, j;
			char temp[70 * 20];

			/* Clear lines, position cursor  (really should use strlen here) */
			Term_erase(7, 23, 255);
			Term_erase(7, 22, 255);
			Term_erase(7, 21, 255);
			Term_erase(7, 20, 255);

			roff_to_buf(k_text + k_info[lookup_kind(tval, i)].text, 62, temp, sizeof(temp));
			for (j = 0, line = 21; temp[j]; j += 1 + strlen(&temp[j]))
			{
				prt(&temp[j], line, 10);
				line++;
			}

			continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	screen_load();

	if (!flag) return -1;

#ifdef ALLOW_REPEAT
	repeat_push(ext+i);
#endif /* ALLOW_REPEAT */
	return ext+i;
}


/*
 *  Use eaten rod, wand or staff
 */
/*:::������p�m�̋Z*/
bool do_cmd_magic_eater(bool only_browse, bool powerful)
{
	int item, chance, level, k_idx, tval, sval;
	bool use_charge = TRUE;

	/* Not when confused */
	if (!only_browse && p_ptr->confused)
	{
#ifdef JP
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("�������Ă��Ė���������o���Ȃ��I");
		else
			msg_print("�������Ă��ď������Ȃ��I");
#else
		msg_print("You are too confused!");
#endif

		return FALSE;
	}

	item = select_magic_eater(only_browse, FALSE);
	if (item == -1)
	{
		energy_use = 0;
		return FALSE;
	}
	if (item >= EATER_EXT*2) {tval = TV_ROD;sval = item - EATER_EXT*2;}
	else if (item >= EATER_EXT) {tval = TV_WAND;sval = item - EATER_EXT;}
	else {tval = TV_STAFF;sval = item;}
	k_idx = lookup_kind(tval, sval);

	level = (tval == TV_ROD ? k_info[k_idx].level * 5 / 6 - 5 : k_info[k_idx].level);
	chance = level * 4 / 5 + 20;
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);
	level /= 2;
	if (p_ptr->lev > level)
	{
		chance -= 3 * (p_ptr->lev - level);
	}
	chance = mod_spell_chance_1(chance);
	chance = MAX(chance, adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]]);
	/* Stunning makes spells harder */
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;

	if (chance > 95) chance = 95;

	chance = mod_spell_chance_2(chance);

	if (randint0(100) < chance)
	{
		if (flush_failure) flush();

#ifdef JP
		if(p_ptr->pclass == CLASS_SEIJA)
			msg_print("�B������������������܂��g���Ȃ������I");
		else
			msg_print("���������܂��������Ȃ������I");
#else
		msg_format("You failed to get the magic off!");
#endif

		sound(SOUND_FAIL);
//		if (randint1(100) >= chance)
//			chg_virtue(V_CHANCE,-1);

		energy_use = 100;

		return TRUE;
	}
	else
	{
		int dir = 0;

		if (tval == TV_ROD)
		{
			if ((sval >= SV_ROD_MIN_DIRECTION) && (sval != SV_ROD_HAVOC) && (sval != SV_ROD_AGGRAVATE) && (sval != SV_ROD_PESTICIDE))
				if (!get_aim_dir(&dir)) return FALSE;
			rod_effect(sval, dir, &use_charge, powerful, TRUE);
			if (!use_charge) return FALSE;
		}
		else if (tval == TV_WAND)
		{
			if (!get_aim_dir(&dir)) return FALSE;
			wand_effect(sval, dir, powerful, TRUE);
		}
		else
		{
			staff_effect(sval, &use_charge, powerful, TRUE, TRUE);
			if (!use_charge) return FALSE;
		}
		if (randint1(100) < chance)
			chg_virtue(V_CHANCE,1);
	}
	energy_use = 100;
	if (tval == TV_ROD) p_ptr->magic_num1[item] += k_info[k_idx].pval * EATER_ROD_CHARGE;
	else p_ptr->magic_num1[item] -= EATER_CHARGE;

        return TRUE;
}





//�������������߂ă����X�^�[��F�D�I�ɂ��锻��
//�������܂���̂ɐ��������Ƃ�TRUE��Ԃ�
bool	miyoi_make_mon_friendly(monster_type *m_ptr, int power)
{

	int r_idx = m_ptr->r_idx;
	monster_race *r_ptr;

	char m_name[120];

	if (!r_idx)
	{
		msg_format("ERROR:miyoi_make_mon_friendly()");
		return FALSE;
	}
	r_ptr = &r_info[r_idx];

	if (is_pet(m_ptr) || is_friendly(m_ptr)) return TRUE;


	monster_desc(m_name, m_ptr, 0);

	//�l�ԂɌ����₷��
	if (r_ptr->flags3 & RF3_HUMAN)
	{
		power *= 2;
	}

	if (p_ptr->cursed & TRC_AGGRAVATE)
	{
		msg_format("���̂��Ȃ��̌��t�ɂ͒N������݂��Ȃ��I");
		return FALSE;
	}

	if (r_ptr->level > randint1(power))
	{
		msg_format("%s�͂��Ȃ��̗U���ɉ����Ȃ������I", m_name);
		return FALSE;
	}

	//�N�G�X�g�œ|�Ώۂ͗F�D�I�ɂȂ�Ȃ��B���x���ۂ܂���Ȃ炻�̓s�x�����ʂ��K�v������
	if (r_ptr->flags1 & RF1_QUESTOR)
	{
		msg_format("%s�Ƃ��Ȃ��͐킢�̑O�ɔu�����킵���B", m_name);
	}
	else
	{
		if (one_in_(3))
			msg_format("%s�͂��Ȃ��̈��z���J����x�����������B", m_name);
		else if (one_in_(2))
			msg_format("%s�ƈӋC���������I", m_name);
		else
			msg_format("%s�Ɗ��t�����I", m_name);

		set_friendly(m_ptr);

	}

	return TRUE;


}


//v2.0.9
//�����������X�^�[�ɂ�����񋟂���
//���Z�u�Ŕ��̂��ށv�u�鑠�̈�{�v������i������Ă΂��
//�����X�^�[��F�D�I�ɂ��鏈���ƗF�D�I�ȃ����X�^�[�Ɏ𑮐��U�����s������
//alcohol:�A���R�[���̋���
//charm_power:�F�D�I�ɂ��閣�͔���l
//�Ώۂ��������񂾂Ƃ�TRUE��Ԃ�
bool	miyoi_serve_alcohol(monster_type *m_ptr, int alcohol, int charm_power)
{
	int r_idx = m_ptr->r_idx;
	monster_race *r_ptr;

	char m_name[120];

	if (!r_idx)
	{
		msg_format("ERROR:miyoi_offer_alcohol()");
		return FALSE;
	}
	r_ptr = &r_info[r_idx];
	monster_desc(m_name, m_ptr, 0);

	//�N����
	set_monster_csleep(cave[m_ptr->fy][m_ptr->fx].m_idx, 0);

	//v2.0.11 �����L�ł͖���
	if (p_ptr->inside_arena)
	{
		msg_format("%s�͒e���Ō���������C���X���I", m_name);
		return FALSE;
	}

	//�U���ɉ����Ȃ��ʁX
	if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))
	{
		msg_format("%s�͖��������B", m_name);
		return FALSE;
	}
	if (r_idx == MON_BYAKUREN)
	{
		msg_format("%s�͉����ɐG��邩��Ǝ����Ŏ������B", m_name);
		return FALSE;
	}
	if (r_idx == MON_SEIJA || r_idx == MON_SEIJA_D)
	{
		msg_format("%s�͂ǂ������Ă����Ȃ���M�p���Ȃ��悤���I", m_name);
		return FALSE;
	}

	//俎q���Ď������񂾂����H
	if (r_idx == MON_SUMIREKO)
	{
		msg_format("%s�͖����N������Ǝ����Ŏ������B", m_name);
		return FALSE;
	}

	//�ꌂ�œ|�����ꏈ��
	if (charm_power < 0)
	{
		project(0, 0, m_ptr->fy, m_ptr->fx, -1, GF_ALCOHOL, (PROJECT_KILL | PROJECT_JUMP), FALSE);
		return TRUE;
	}

	//�F�D�I�łȂ������X�^�[�ɂ͂܂����͔����ʂ��Ȃ��Ǝ���ۂ܂����Ȃ�
	if (!is_friendly(m_ptr))
	{
		//����ɐ��������烂���X�^�[���F�D�I�ɂȂ�(QUESTOR����)
		if (!miyoi_make_mon_friendly(m_ptr, charm_power)) return FALSE;

	}

	msg_format("%s�ɂ��������܂����I", m_name);

	//�����X�^�[�̓D���x�㏸��GF_ALCOHOL�o�R�ōs���B���������ł��ׂ�Ȃ������X�^�[�͂�����ŏ���
	project(0, 0, m_ptr->fy, m_ptr->fx, alcohol, GF_ALCOHOL, (PROJECT_KILL | PROJECT_JUMP), FALSE);

	return TRUE;

}

