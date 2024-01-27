/* File: obj_kind.c */

#include "angband.h"



/*:::�򔻒� �Ȃ�ł���ȕςȔ��莮���g���Hoptr->tval����܂����̂��H�킩���*/
///mod151227 ������(�H���̖򏜂�)�ǉ�
bool object_is_potion(object_type *o_ptr)
{
	if(o_ptr->tval == TV_COMPOUND && o_ptr->sval != SV_COMPOUND_HOURAI) return TRUE;

	return (k_info[o_ptr->k_idx].tval == TV_POTION);
}


//v1.1.92 do_spell����ړ����Ă���static�O����
bool item_tester_hook_cursed(object_type *o_ptr)
{
	return (bool)(object_is_cursed(o_ptr));
}

///sysdel �܋���
///del131221 �܋��񔻕�
#if 0
bool object_is_shoukinkubi(object_type *o_ptr)
{
	int i;

	/* Require corpse or skeleton */
	if (o_ptr->tval != TV_CORPSE) return FALSE;

	/* No wanted monsters in vanilla town */
	if (vanilla_town) return FALSE;

	/* Today's wanted */
	if (p_ptr->today_mon > 0 && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name))) return TRUE;

	/* Tsuchinoko */
	if (o_ptr->pval == MON_TSUCHINOKO) return TRUE;

	/* Unique monster */
	for (i = 0; i < MAX_KUBI; i++)
		if (o_ptr->pval == kubi_r_idx[i]) break;
	if (i < MAX_KUBI) return TRUE;

	/* Not wanted */
	return FALSE;
}
#endif

/*
 * Favorite weapons
 */
///mod131223
bool object_is_favorite(object_type *o_ptr)
{
	/* Only melee weapons match */
	/*
	if (!(o_ptr->tval == TV_POLEARM ||
	      o_ptr->tval == TV_SWORD ||
	      o_ptr->tval == TV_DIGGING ||
	      o_ptr->tval == TV_HAFTED))
	{
		return FALSE;
	}
	*/
	if(!object_is_weapon(o_ptr)) return FALSE;

	/* Favorite weapons are varied depend on the class */
	switch (p_ptr->pclass)
	{
	case CLASS_PRIEST:
	{
		u32b flgs[TR_FLAG_SIZE];
		object_flags_known(o_ptr, flgs);

		if (!have_flag(flgs, TR_BLESSED) && 
		    !(o_ptr->tval == TV_HAMMER || o_ptr->tval == TV_STICK))
			return FALSE;
		break;
	}

	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		/* Icky to wield? */
		//if (!(s_info[p_ptr->pclass].w_max[o_ptr->tval-TV_WEAPON_BEGIN][o_ptr->sval]))
		if(o_ptr->tval != TV_STICK || o_ptr->sval != SV_WEAPON_SIXFEET && o_ptr->sval != SV_WEAPON_NUNCHAKU)
		return FALSE;
		break;

	case CLASS_MARTIAL_ARTIST:
		if(o_ptr->tval != TV_STICK) return FALSE;
		break;


	case CLASS_BEASTMASTER:
	case CLASS_CAVALRY:
	{
		u32b flgs[TR_FLAG_SIZE];
		object_flags_known(o_ptr, flgs);

		/* Is it known to be suitable to using while riding? */
		if (!(have_flag(flgs, TR_RIDING)))
			return FALSE;

		break;
	}

	case CLASS_NINJA:
		/* Icky to wield? */
		//if (s_info[p_ptr->pclass].w_max[o_ptr->tval-TV_WEAPON_BEGIN][o_ptr->sval] <= WEAPON_EXP_BEGINNER)
		if(o_ptr->tval != TV_KNIFE)
			return FALSE;
		break;

	default:
		/* All weapons are okay for non-special classes */
		return TRUE;
	}

	return TRUE;
}


/*
 * Rare weapons/aromors
 * including Blade of Chaos, Dragon armors, etc.
 */
/*:::�A�C�e�������A���ǂ�������*/
///item ���A�x�[�X����
///mod131223
//v1.1.49 �ꕔ���p�x�[�X�A�C�e���ǉ�
bool object_is_rare(object_type *o_ptr)
{
	switch(o_ptr->tval)
	{
	case TV_KNIFE:
		if (o_ptr->sval == SV_WEAPON_HAYABUSA ||
		    o_ptr->sval == SV_WEAPON_DRAGON_DAGGER ||
			o_ptr->sval == SV_WEAPON_VAJRA ||
			o_ptr->sval == SV_WEAPON_DOKUBARI) return TRUE;
		break;
	case TV_SWORD:
		if (o_ptr->sval == SV_WEAPON_BLADE_OF_CHAOS ||
		    o_ptr->sval == SV_WEAPON_HIHIIROKANE) return TRUE;
		break;
	case TV_KATANA:
		if (o_ptr->sval == SV_WEAPON_ANCIENT_KATANA ||
		    o_ptr->sval == SV_WEAPON_EVIL_KATANA) return TRUE;
		break;
	case TV_HAMMER:
		if (o_ptr->sval == SV_WEAPON_KANASAIBOU ) return TRUE;
		break;
	case TV_STICK:
		if (o_ptr->sval == SV_WEAPON_WIZSTAFF ||
			o_ptr->sval == SV_WEAPON_GROND ||
			o_ptr->sval == SV_WEAPON_KOGASA ||
			o_ptr->sval == SV_WEAPON_OONUSA
			) return TRUE;
		break;
	case TV_AXE:
		if (o_ptr->sval == SV_WEAPON_MASAKARI) return TRUE;
		break;

	case TV_POLEARM:
		if (o_ptr->sval == SV_WEAPON_SCYTHE_OF_SLICING) return TRUE;
		break;
	case TV_SPEAR:
		if (o_ptr->sval == SV_WEAPON_TRIFURCATE_SPEAR) return TRUE;
		break;

	case TV_GUN:
		if (o_ptr->sval == SV_FIRE_GUN_CRIMSON ||
			o_ptr->sval == SV_FIRE_GUN_MAGNUM ||
			o_ptr->sval == SV_FIRE_GUN_LASER ||
			o_ptr->sval == SV_FIRE_GUN_ROCKET

			) return TRUE;
		break;
	case TV_MAGICITEM:
	case TV_MAGICWEAPON:
		return TRUE;
		break;

	case TV_SHIELD:
		if (o_ptr->sval == SV_DRAGON_SHIELD ||
		    o_ptr->sval == SV_MIRROR_SHIELD ||
			o_ptr->sval == SV_SAKAZUKI ||
			o_ptr->sval == SV_HACHI

			) return TRUE;
		break;
	case TV_LITE:
		if (o_ptr->sval == SV_LITE_EDISON ||
			o_ptr->sval == SV_LITE_GALADRIEL ||
			o_ptr->sval == SV_LITE_ELENDIL ||
			o_ptr->sval == SV_LITE_JUDGE ||
			o_ptr->sval == SV_LITE_LORE ||
			o_ptr->sval == SV_LITE_PALANTIR ||
			o_ptr->sval == SV_LITE_FLY_STONE ||
			o_ptr->sval == SV_LITE_ANDON ||
			o_ptr->sval == SV_LITE_HOUTOU ||
			o_ptr->sval == SV_LITE_SANAGI ||
			o_ptr->sval == SV_LITE_TRAPEZOHEDRON ||
			o_ptr->sval == SV_LITE_DRAGON
			) return TRUE;
		break;


	case TV_HEAD:
		if (o_ptr->sval == SV_HEAD_DRAGONHELM	||
			o_ptr->sval == SV_HEAD_IRONCROWN
			) return TRUE;
		break;

	case TV_BOOTS:
		if (o_ptr->sval == SV_LEG_DRAGONBOOTS) return TRUE;
		break;

	case TV_CLOAK:
		if (o_ptr->sval == SV_CLOAK_STEALTH ||
		    o_ptr->sval == SV_CLOAK_SHADOW) return TRUE;
		break;

	case TV_GLOVES:
		if (o_ptr->sval == SV_HAND_DRAGONGLOVES ||
			o_ptr->sval == SV_HAND_WRISTUNIT) return TRUE;
		break;

	case TV_CLOTHES:
		if (o_ptr->sval == SV_CLOTH_KUROSHOUZOKU ||
		    o_ptr->sval == SV_CLOTH_MIKO
			//||	o_ptr->sval == SV_CLOTH_T_SHIRT
			) return TRUE;
		break;

	case TV_ARMOR:
		if ( o_ptr->sval == SV_ARMOR_MITHRIL_PLATE	||
			o_ptr->sval == SV_ARMOR_MITHRIL_CHAIN	||
			o_ptr->sval == SV_ARMOR_FINE_CERAMIC
			) return TRUE;
		break;

	case TV_DRAG_ARMOR:
		return TRUE;




		return TRUE;



	default:
		break;
	}

	/* Any others are not "rare" objects. */
	return FALSE;
}


/*
 * Check if an object is weapon (including bows and ammo)
 */
///item TVAL�֘A����
///mod131223
///mod150805 MAGICWEAPON���܂߂�
bool object_is_weapon(object_type *o_ptr)
{
	if (TV_WEAPON_BEGIN <= o_ptr->tval && o_ptr->tval <= TV_WEAPON_END) return TRUE;
	if (o_ptr->tval == TV_MAGICWEAPON) return TRUE;
	if (o_ptr->tval == TV_OTHERWEAPON) return TRUE;

	return FALSE;
}


/*
 * Check if an object is weapon (including bows and ammo)
 */
///item TVAL�֘A����
///mod131223
///mod150805 MAGICWEAPON���܂߂�
bool object_is_weapon_ammo(object_type *o_ptr)
{
	if (TV_MISSILE_BEGIN <= o_ptr->tval && o_ptr->tval <= TV_WEAPON_END) return TRUE;
	if (o_ptr->tval == TV_MAGICWEAPON) return TRUE;
	if (o_ptr->tval == TV_OTHERWEAPON) return TRUE;
	return FALSE;
}


/*
 * Check if an object is ammo
 */
///item TVAL�֘A����
///mod131223
///mod150805 MAGICWEAPON���܂߂�
bool object_is_ammo(object_type *o_ptr)
{
	if (TV_MISSILE_BEGIN <= o_ptr->tval && o_ptr->tval <= TV_MISSILE_END) return TRUE;

	return FALSE;
}


/*
 * Check if an object is armour
 */
/*:::�Z�����łȂ��h��S��*/
///item TVAL�֘A����
///mod131223
bool object_is_armour(object_type *o_ptr)
{
	if (	o_ptr->tval == TV_SHIELD 
		||	o_ptr->tval == TV_CLOTHES 
		||	o_ptr->tval == TV_ARMOR
		||	o_ptr->tval == TV_DRAG_ARMOR 
		||	o_ptr->tval == TV_CLOAK
		||	o_ptr->tval == TV_HEAD 
		||	o_ptr->tval == TV_GLOVES 
		||	o_ptr->tval == TV_BOOTS 
		) return TRUE;

	return FALSE;
}


/*
 * Check if an object is weapon, armour or ammo
 */
///item TVAL
bool object_is_weapon_armour_ammo(object_type *o_ptr)
{
	if (object_is_weapon_ammo(o_ptr) || object_is_armour(o_ptr)) return TRUE;

	return FALSE;
}


/*
 * Melee weapons
 */
///item ����TVAL�z��֘A
///mod131223 tval
bool object_is_melee_weapon(object_type *o_ptr)
{
	if ((TV_KNIFE <= o_ptr->tval && o_ptr->tval <= TV_POLEARM)
		 || o_ptr->tval == TV_OTHERWEAPON || o_ptr->tval == TV_MAGICWEAPON ) return TRUE;

	return FALSE;
}

/* �ߐڕ��킩�����A�C�e���̓Őj��TV_MAGICWEAPON������������*/
// ����C���f�ނƕt�r�_�ω��̃A�C�e���w��Ɏg��
// ���Ƃ���bldg.c�ɂ���item_tester_hook_melee_weapon�Ƃ����֐����������A
// object_is_melee_weapon()�ƕ���킵���̂�v1.1.93 �Ŗ��̕ύX��obj_kind.c�Ɉړ����Ă���
// cmd3.c�ɂ�item_tester_hook_melee_weapon�����������폜
bool object_is_melee_weapon_except_strange_kind(object_type *o_ptr)
{

	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI) return (FALSE);
	if (o_ptr->tval == TV_MAGICWEAPON) return (FALSE);

	return (object_is_melee_weapon(o_ptr));

}



///mod131229 �n�̂��镐��̂Ƃ�TRUE��Ԃ� (���F�����X�Ȃǐn���Ȃ����v���[�X�g�����ł͂Ȃ����������̂ł��̊֐������ł̓v���[�X�g��������𔻕ʂł��Ȃ��j
bool object_has_a_blade(object_type *o_ptr)
{
	if (!o_ptr->k_idx) return FALSE;
	if (o_ptr->tval == TV_KNIFE && o_ptr->sval != SV_WEAPON_DOKUBARI) return TRUE;
	if (o_ptr->tval == TV_SWORD || o_ptr->tval == TV_KATANA) return TRUE;
	if (o_ptr->tval == TV_SPEAR && ( o_ptr->sval == SV_WEAPON_SPEAR || o_ptr->sval == SV_WEAPON_LONG_SPEAR)) return TRUE;
	if (o_ptr->tval == TV_POLEARM && o_ptr->sval != SV_WEAPON_LUCERNE_HAMMER) return TRUE;
	if (o_ptr->tval == TV_AXE && o_ptr->sval != SV_WEAPON_PICK) return TRUE;

	return FALSE;
}





/*
 * Wearable including all weapon, all armour, bow, light source, amulet, and ring
 */
///item TVAL
/*:::�����\�A�C�e��(�J�����j�����i�ϐ��ꗗ�R�}���h���炵���Ă΂�ĂȂ�*/
///mod131223
bool object_is_wearable(object_type *o_ptr)
{
	if (TV_WEARABLE_BEGIN <= o_ptr->tval && o_ptr->tval <= TV_WEARABLE_END
		&& o_ptr->tval != TV_MAGICITEM && o_ptr->tval != TV_MAGICWEAPON
		) return TRUE;

	return FALSE;
}


/*
 * Equipment including all wearable objects and ammo
 */
///item TVAL
///mod131223
bool object_is_equipment(object_type *o_ptr)
{
	if (TV_EQUIP_BEGIN <= o_ptr->tval && o_ptr->tval <= TV_EQUIP_END
		&& o_ptr->tval != TV_MAGICITEM && o_ptr->tval != TV_MAGICWEAPON
		) return TRUE;
	return FALSE;
}


/*
 * Poison needle can not be enchanted
 */
///item TVAL�@�����s�\�Ȓ��ڕ���
///mod131223
bool object_refuse_enchant_weapon(object_type *o_ptr)
{
	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI
		|| o_ptr->tval == TV_MAGICITEM 
		|| o_ptr->tval == TV_MAGICWEAPON) return TRUE;

	return FALSE;
}


/*
 * Check if an object is weapon (including bows and ammo) and allows enchantment
 */

bool object_allow_enchant_weapon(object_type *o_ptr)
{
	if (object_is_weapon_ammo(o_ptr) && !object_refuse_enchant_weapon(o_ptr)) return TRUE;

	return FALSE;
}


/*
 * Check if an object is melee weapon and allows enchantment
 */
bool object_allow_enchant_melee_weapon(object_type *o_ptr)
{
	if (object_is_melee_weapon(o_ptr) && !object_refuse_enchant_weapon(o_ptr)) return TRUE;

	return FALSE;
}


/*
 * Check if an object is made by a smith's special ability
 */
bool object_is_smith(object_type *o_ptr)
{
	//������̖ʂ͂����ł͏������Ȃ�
	if(o_ptr->tval == TV_MASK) return FALSE;
	/*:::����h��ȊO��xtra3���g���邱�Ƃ�����炵���B�J���d���̐������x���Ƃ�*/
	if (object_is_weapon_armour_ammo(o_ptr) && o_ptr->xtra3) return TRUE;
	///mod141023 ����t�����̂��߁A�����i�ƌ������Ώۂɂ���
	if (o_ptr->tval == TV_RIBBON && o_ptr->xtra3) return TRUE;
	if (o_ptr->tval == TV_RING && o_ptr->xtra3) return TRUE;
	if (o_ptr->tval == TV_AMULET && o_ptr->xtra3) return TRUE;
	if (o_ptr->tval == TV_LITE && o_ptr->sval != SV_LITE_TORCH && o_ptr->xtra3) return TRUE;


	return FALSE;
}


/*
 * Check if an object is artifact
 */
///item �A�C�e�����A�[�e�B�t�@�N�g���ǂ����@PDSM�������ɓ����H
bool object_is_artifact(object_type *o_ptr)
{
	if (object_is_fixed_artifact(o_ptr) || o_ptr->art_name) return TRUE;

	return FALSE;
}


/*
 * Check if an object is random artifact
 */
bool object_is_random_artifact(object_type *o_ptr)
{
	if (object_is_artifact(o_ptr) && !object_is_fixed_artifact(o_ptr)) return TRUE;

	return FALSE;
}


/*
 * Check if an object is neither artifact, ego, nor 'smith' object
 */
bool object_is_nameless(object_type *o_ptr)
{
	if (!object_is_artifact(o_ptr) && !object_is_ego(o_ptr) && !object_is_smith(o_ptr))
		return TRUE;

	return FALSE;
}


bool object_is_shooting_weapon(object_type *o_ptr)
{
	if(o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW || o_ptr->tval == TV_GUN ) return (TRUE); 
	else return (FALSE);
}
/*
 * Check if an object is melee weapon and allows wielding with two-hands
 */
/*:::���킪���莝���\���ǂ�������@���݁A�d��100�ȏ�������͒�������ŉ\*/
///
///mod131223 item ���莝���\������TVAL�ŕ�����
///mod140309 �ˌ�������Ώۂɂ���
///mod141122 ���l�Ɨd���͒Z���𗼎�Ŏ��Ă�悤�ɂ��Ă݂�
///mod151205 �O�����͗��莝���ł��Ȃ�
bool object_allow_two_hands_wielding(object_type *o_ptr)
{

	if(p_ptr->pclass == CLASS_3_FAIRIES) return FALSE;

	//if (object_is_melee_weapon(o_ptr) && ((o_ptr->weight > 99) || (o_ptr->tval == TV_POLEARM))) return TRUE;
	if(		(o_ptr->tval == TV_POLEARM)
		||	(o_ptr->tval == TV_SWORD)
		||	(o_ptr->tval == TV_KATANA)
		||	(o_ptr->tval == TV_HAMMER)
		||	(o_ptr->tval == TV_STICK)
		||	(o_ptr->tval == TV_AXE)
		||	(o_ptr->tval == TV_SPEAR) )return TRUE;
	if(object_is_shooting_weapon(o_ptr)) return TRUE;

	if((p_ptr->prace == RACE_FAIRY || p_ptr->prace == RACE_KOBITO) && o_ptr->tval == TV_KNIFE) return TRUE;

	return FALSE;
}


///mod140308 �|�g�p�~�����֘A
/*:::���݂̑����Ŏˌ����ł��邩���肷��B*/
/*:::�߂�l�F�ˌ��s��-0 �ˌ�������E��ɑ���-1 �ˌ����������ɑ���-2*/
/*:::tester:�ڂ�����Ԃ𐔒l�ŕԂ��B �������K�؂łȂ��A�������d���A�ˌ��\�����y�i���e�B�L��A�Ȃ�*/
/*:::���̊֐����Ă΂��Ƃ�inventory[INVEN_PACK]����ł��邱�Ƃ�O��Ɏˌ��۔���Ƃ�����Ă邪���Ȃ��̂��낤���B���ɐS�z���B*/

int shootable(int *tester)
{
	object_type *rarm_o_ptr = &inventory[INVEN_RARM]; 
	object_type *larm_o_ptr = &inventory[INVEN_LARM]; 
	int hand=0;

	/*:::�b�ϐg�Ȃǂ̂Ƃ��ˌ��s��*/
	if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)	*tester = SHOOT_UNSUITABLE;
	//����Ƃ��ˌ�����A���邢�͗���Ƃ��ˌ�����łȂ��Ƃ��͎ˌ��s��
	else if(!object_is_shooting_weapon(rarm_o_ptr) && !object_is_shooting_weapon(larm_o_ptr)) *tester = SHOOT_NO_WEAPON;

	///mod160503 �e�d�l�ύX
	else if(rarm_o_ptr->tval == TV_GUN || larm_o_ptr->tval == TV_GUN)
	{
		int hold_wgt = adj_str_hold[p_ptr->stat_ind[A_STR]] * 5;//�Ў�ō\������d�ʁ@�r��30��8.5kg,�r��40��16kg�܂�

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_WEAPON_MASTERY))
			hold_wgt *= 2;

		//�񒚌��e
		if(rarm_o_ptr->tval == TV_GUN && larm_o_ptr->tval == TV_GUN) 
		{
			if(rarm_o_ptr->weight < hold_wgt && larm_o_ptr->weight < hold_wgt)
			{
				hand = 1;
				*tester = SHOOT_NO_PENALTY;
			}
			else
				*tester = SHOOT_HEAVY_WEAPON;
		}
		//�Е����e�ŕЕ����e�ȊO�̎ˌ�����@�e�����g���Ȃ�
		else if(object_is_shooting_weapon(rarm_o_ptr) && object_is_shooting_weapon(larm_o_ptr))
		{
			if(rarm_o_ptr->tval == TV_GUN)
			{
				if(rarm_o_ptr->weight < hold_wgt)
				{
					hand = 1;
					*tester = SHOOT_GUN_ONLY;
				}
				else
					*tester = SHOOT_HEAVY_WEAPON;
			}
			else
			{
				if(larm_o_ptr->weight < hold_wgt)
				{
					hand = 2;
					*tester = SHOOT_GUN_ONLY;
				}
				else
					*tester = SHOOT_HEAVY_WEAPON;
			}
		}
		//�e�̗��莝���@�d�ʃ{�[�i�X
		else if(!rarm_o_ptr->k_idx || !larm_o_ptr->k_idx)
		{
			int check_weight;
			hold_wgt *= 2;
			if(p_ptr->stat_ind[A_STR] > 36) hold_wgt = 7000; //�n���R���l��2�̂��߂ɖ����ɑ��₷

			if (rarm_o_ptr->tval == TV_GUN)
			{
				if (rarm_o_ptr->weight < hold_wgt)
				{
					hand = 1;
					*tester = SHOOT_NO_PENALTY;
				}
				else
					*tester = SHOOT_HEAVY_WEAPON;
			}
			//v1.1.32 ����Ɏ����Ă����e�����ꂽ���ƉE��̕����O���č���e�̗��莝���ɂȂ����Ƃ��̏�����ǉ�
			else
			{
				if (larm_o_ptr->weight < hold_wgt)
				{
					hand = 2;
					*tester = SHOOT_NO_PENALTY;
				}
				else
					*tester = SHOOT_HEAVY_WEAPON;
			}

		}
		//����A���A�d���{�Ȃǂ������Е��̎�Ɏ����Ă���
		else
		{
			if(rarm_o_ptr->tval == TV_GUN)
			{
				if(rarm_o_ptr->weight < hold_wgt)
				{
					hand = 1;
					*tester = SHOOT_HIGH_PENALTY;
				}
				else
					*tester = SHOOT_HEAVY_WEAPON;
			}
			else
			{
				if(rarm_o_ptr->weight < hold_wgt && larm_o_ptr->weight < hold_wgt)
				{
					hand = 2;
					*tester = SHOOT_HIGH_PENALTY;
				}
				else
					*tester = SHOOT_HEAVY_WEAPON;
			}
		}
	}
	else if(object_is_shooting_weapon(rarm_o_ptr) && object_is_shooting_weapon(larm_o_ptr))	*tester = SHOOT_UNSUITABLE;

	///mod151205 �O�������ꏈ���@�d�ʉߑ��łȂ���΃y�i���e�B�Ȃ�
	//�ˌ�����𗼎莝�����Ă�Ƃ��̓y�i���e�B�Ȃ��@�������ˌ����킪�d������Ǝˌ��s��
	else if(object_is_shooting_weapon(rarm_o_ptr) && ((empty_hands(TRUE) & EMPTY_HAND_LARM) || p_ptr->pclass == CLASS_3_FAIRIES)) 
	{ 
		if(rarm_o_ptr->weight / 20 > p_ptr->stat_use[A_STR])
			*tester = SHOOT_HEAVY_WEAPON;
		else 
		{
			hand = 1;
			*tester = SHOOT_NO_PENALTY;
		}
	}
	else if(object_is_shooting_weapon(larm_o_ptr) && ((empty_hands(TRUE) & EMPTY_HAND_RARM)|| p_ptr->pclass == CLASS_3_FAIRIES)) 
	{ 
		if(larm_o_ptr->weight / 16 > p_ptr->stat_use[A_STR])
			*tester = SHOOT_HEAVY_WEAPON;
		else 
		{
			hand = 2;
			*tester = SHOOT_NO_PENALTY;
		}
	}
	//�e�͗���Ŏ����Ȃ��Ə�Ɏˌ��s��?
	else if(rarm_o_ptr->tval == TV_GUN || larm_o_ptr->tval == TV_GUN) *tester = SHOOT_UNSUITABLE;

	//�Ў�̏ꍇ�ˌ�����̏d���Ŏˌ��s�ɂȂ�₷���悤�ɂ���@
	else if(object_is_shooting_weapon(rarm_o_ptr) && rarm_o_ptr->weight / 5 > p_ptr->stat_use[A_STR]) *tester = SHOOT_HEAVY_WEAPON;
	else if(object_is_shooting_weapon(larm_o_ptr) && larm_o_ptr->weight / 5 > p_ptr->stat_use[A_STR]) *tester = SHOOT_HEAVY_WEAPON;

	else
	{
		if(object_is_shooting_weapon(rarm_o_ptr)) hand = 1;
		if(object_is_shooting_weapon(larm_o_ptr)) hand = 2;

		//�ˌ�����i�e�ȊO�j�Ƌߐڕ���������Ă���ꍇ�y�i���e�B�d��
		if(object_is_melee_weapon(rarm_o_ptr) || object_is_melee_weapon(larm_o_ptr)) *tester = SHOOT_HIGH_PENALTY; 

		//����ȊO�̓y�i���e�B�y��
		else *tester = SHOOT_LOW_PENALTY;
	}

	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_WEAPON_MASTERY) && *tester == SHOOT_HIGH_PENALTY)
		*tester = SHOOT_LOW_PENALTY;


	return hand;

}








bool object_is_not_worthless(object_type *o_ptr)
{
	if (object_value(o_ptr) != 0) return TRUE;

	return FALSE;
}

//v1.1.46 �����ɂƂ��āu�����v�ł���A�C�e���@���̏����ɓK�������A�C�e���͑����ł��Ȃ�
/* ����σ{�c�@
bool object_is_cheap_to_jyoon(object_type *o_ptr)
{
	int plev = p_ptr->lev;
	int value;

	value = object_value_real(o_ptr);

	if (value <= 0) return TRUE;

	if (plev >= 45 && value < 60000) return TRUE;
	if (plev >= 40 && value < 40000) return TRUE;
	if (plev >= 35 && value < 20000) return TRUE;
	if (plev >= 30 && value < 10000) return TRUE;
	if (plev >= 25 && value < 5000) return TRUE;
	if (plev >= 20 && value < 1000) return TRUE;

	return FALSE;

}
*/







