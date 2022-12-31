
#include "angband.h"

/*:::���ϊ֌W�̃R�[�h�@�c�ꂻ���Ȃ̂ŕʃt�@�C���ɂ���*/

//p_ptr->magic_num1[0-98]:������u�����Ȃ��Z�v�̃p�����[�^�i�[ 9*11
//p_ptr->magic_num1[99-107]:�\��
//p_ptr->magic_num2[0]:��
//p_ptr->magic_num2[1-107]:�Z�����d�ݕt���̂��߂̃t���O�J�E���g�̈�


//���ς����ʐ��̓t���O�̓G��|�����Ƃ�������Z���t
void junko_msg(void)
{

	if (p_ptr->pclass != CLASS_JUNKO) return;

	switch (randint1(7))
	{
	case 1:
	case 2:
	case 3:
	case 4:
		msg_print("�u�s��ՓV�̓G�A�b�M��B���Ă邩�I�H�v");
		break;
	case 5:
		msg_print("�u���̗͂̑O�ł͖��́I��ΓI���́I�v");
		break;
	case 6:
		msg_print("�u��_�s�G�ȍ􂾂������ǁA�l�߂��Â������l�ˁB�v");
		break;
	case 7:
		msg_print("�u���O���o�Ă���܂ŁA�����������Ԃ葱���悤�I�v");
		break;

	}


}



/*****************�������珃���֘A*********************************************/

///���ς��u�����v�\�ȃA�C�e���@
static bool item_tester_hook_purify_able(object_type *o_ptr)
{
	if(o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER) return FALSE; 

	if(object_is_melee_weapon(o_ptr)) return TRUE;
	if(object_is_armour(o_ptr)) return TRUE;

	if(o_ptr->tval == TV_RIBBON) return TRUE;
	if(o_ptr->tval == TV_RING) return TRUE;
	if(o_ptr->tval == TV_AMULET) return TRUE;
	if(o_ptr->tval == TV_LITE)
	{
		if(o_ptr->sval == SV_LITE_TORCH) return FALSE;
		else if(o_ptr->sval == SV_LITE_LANTERN) return FALSE;
		else return TRUE;
	}
	
	return FALSE;

}

//���ϓ��Z�u�����U�v�ɂ�鑕���i����
//�����i��}�f�����A�폜���ꂽ�t���O��AC���U���C�����p�����[�^�㏸�l�ɕϊ�����
bool purify_equipment(void)
{

	int item;
	cptr q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	int bonus = 0;
	int old_pv,old_to_h,old_to_d,old_to_a;

	item_tester_hook = item_tester_hook_purify_able;

	q = "�ǂ̃A�C�e�����������܂����H";
	s = "�����ł���A�C�e��������܂���B";

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	if (!get_check_strict(format("�������܂����H [%s]", o_name),CHECK_OKAY_CANCEL)) return FALSE;

	//�����i�̒ǉ��t���O���{�[�i�X�l�Ƃ��ē���B
	//�܂��ǉ��t���O����Z�t���O�Ƃ��ē��Ă���B
	bonus = flag_cost(o_ptr,o_ptr->pval,TRUE);
	if(bonus < 0) bonus = 0;

	//�Â��{�[�i�X�l��ێ�(�Œ�0)
	old_pv = MAX(0,o_ptr->pval);
	old_to_h = MAX(0,o_ptr->to_h);
	old_to_d = MAX(0,o_ptr->to_d);
	old_to_a = MAX(0,o_ptr->to_a);


	//mundane_spell()����R�s�[
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
	o_ptr->pval = old_pv;
	//o_ptr->to_h = old_to_h;		
	//o_ptr->to_d = old_to_d;		
	//o_ptr->to_a = old_to_a;		
	bonus += old_to_h * 1000;
	bonus += old_to_d * 1000;
	bonus += old_to_a * 1000;


	if(object_is_melee_weapon(o_ptr)
	 || (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_ACCURACY)
	 || (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_DAMAGE)
	 || (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_SLAYING))
	{
		if(bonus > 200000) bonus = 200000;
		o_ptr->to_d += bonus / (1500+randint1(500));
		o_ptr->to_h += bonus / (1500+randint1(500));
		if(o_ptr->to_d > 99) o_ptr->to_d = 99;
		if(o_ptr->to_h > 99) o_ptr->to_h = 99;

		if(!k_info[o_ptr->k_idx].pval) o_ptr->pval = 0;

	}
	else if(object_is_armour(o_ptr) || o_ptr->tval == TV_RIBBON)
	{
		if(bonus > 200000) bonus = 200000;
		o_ptr->to_a += bonus / (1000+randint1(500));
		if(o_ptr->to_a > 99) o_ptr->to_a = 99;

		if(!k_info[o_ptr->k_idx].pval) o_ptr->pval = 0;
	}
	else //�w�ւƃA�~���ƌ����������ɗ���͂�
	{
		object_type forge;
		object_type *tmp_o_ptr = &forge;

		//���Xpval�̂���i���ǂ������肷�邽�߁A����k_idx�̕i���m�[�}����������Bk_info[].pval�͎w�ւƃA�~���̏ꍇ�S��0�ł��蔻��Ɏg���Ȃ��B
		object_prep(tmp_o_ptr, o_ptr->k_idx);
		apply_magic(tmp_o_ptr,1,AM_FORCE_NORMAL);

		if(tmp_o_ptr->pval)
		{
			if(o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_SPEED)
			{
				o_ptr->pval += bonus / (5000);
				if(o_ptr->pval > 99) o_ptr->pval = 99;
			}
			else if(o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_MUSCLE)
			{
				o_ptr->pval += bonus / (8000);
				if(o_ptr->pval > 9) o_ptr->pval = 9;
			}
			else if(o_ptr->tval == TV_AMULET && o_ptr->sval == SV_AMULET_BRILLIANCE)
			{
				o_ptr->pval += bonus / (8000);
				if(o_ptr->pval > 9) o_ptr->pval = 9;
			}
			else if(o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_ATTACKS)
			{
				o_ptr->pval += bonus / (10000 + randint1(10000));
				if(o_ptr->pval > 5) o_ptr->pval = 5;
				if(o_ptr->pval == 5 && !weird_luck()) o_ptr->pval = 4;
				if(o_ptr->pval  > 3 && !weird_luck()) o_ptr->pval = 3;
			}
			else
			{
				o_ptr->pval += bonus / (3000);
				if(o_ptr->pval > 99) o_ptr->pval = 99;
			}

		}
		else
		{
			o_ptr->pval = 0;
			if(bonus > 200000) bonus = 200000;
			o_ptr->to_a += bonus / (1000);
			if(o_ptr->to_a > 99) o_ptr->to_a = 99;
		}

	}


	/*:::���f����ی�*/
	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
	add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(o_ptr->art_flags, TR_IGNORE_COLD);

	//�Ӓ�ς�
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	//�G�S�A�C�e���ɂ���
	o_ptr->name2 = EGO_JUNKO_PURIFIED;

	//�����σt���O�𗧂Ă�
	o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	return TRUE;

}


/*:::���ϗp�R�}���h�u�����T�v�@�A�C�e�������l/100��MP�ɕϊ�����*/
bool convert_item_to_mana(void)
{
	int item, amt = 1;
	int old_number;
	long price;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	char out_val[160];

	cptr q, s;

	q = "�ǂ̃A�C�e�����������܂����H";
	s = "MP�ɕς����镨������܂���B";

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);
	if (item >= 0)
		o_ptr = &inventory[item];
	else
		o_ptr = &o_list[0 - item];

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
	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	o_ptr->number = old_number;

	sprintf(out_val, "%s�͏��ł��܂��B��낵���ł����H", o_name);
	if (!get_check_strict(out_val, CHECK_OKAY_CANCEL)) return FALSE;

	price = object_value_real(o_ptr);


	//���ς̏ꍇ��������A�C�e���ɂ���ăt���O�l�𓾂�
	if (p_ptr->pclass == CLASS_JUNKO)
	{
		int flag_val = 10;
		int i;
		u32b flgs[TR_FLAG_SIZE];
		int tv = o_ptr->tval;
		int sv = o_ptr->sval;
		int pv = o_ptr->pval;

		object_flags(o_ptr, flgs);


		//���@���Ȃ�
		if (tv <= TV_BOOK_END)
		{
			if (sv == 0) flag_val = 10 * amt;
			if (sv == 1) flag_val = 30 * amt;
			if (sv == 2) flag_val = 80 * amt;
			if (sv == 3) flag_val = 200 * amt;

			switch (tv)
			{
			case TV_BOOK_ELEMENT:

				if (sv == 0) add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
				if (sv == 1) add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv == 2) add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_RES_ELEM, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				add_junko_flag_val(JKF2_ACID, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
				if (sv == 3)  add_junko_flag_val(JKF2_MAKE_WALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_DISPEL, flag_val);
				break;
			case TV_BOOK_FORESEE:
				add_junko_flag_val(JKF1_IDENTIFY, flag_val);
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_TRAPS, flag_val);
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_OBJ, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_MENTAL_UP, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ENLIGHT, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ALTER_REALITY, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_RESTORE_NEXUS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_LUCKY, flag_val);
				add_junko_flag_val(JKF2_TIME, flag_val);
				add_junko_flag_val(JKF1_DETECT_ALL, flag_val);
				break;
			case TV_BOOK_ENCHANT:
				add_junko_flag_val(JKF1_FOOD, flag_val);
				if (sv == 0) add_junko_flag_val(JKF1_SELF_LIGHT, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MAGICDEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MASTER_MAGITEM, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SHIELD, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ADD_ESSENCE2, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_MAKE_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_MANA, flag_val);
				break;
			case TV_BOOK_SUMMONS:
				add_junko_flag_val(JKF1_TELEPO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_D_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_VANISH_BEAM, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MASS_GENO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_TELE_AWAY_ALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_DISTURB_SUMMON, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_CALL_METEOR, flag_val);
				add_junko_flag_val(JKF2_DISTORTION, flag_val);
				add_junko_flag_val(JKF2_CHAOS, flag_val);
				break;

			case TV_BOOK_MYSTIC:
				if (sv == 0 || sv == 1) add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ENLIGHT, flag_val);
				if (sv <= 1) add_junko_flag_val(JKF1_TELEPO, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_D_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MENTAL_UP, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_INVULNER, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SPEED, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_IDENTIFY, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_TELE_AWAY_ALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_STASIS, flag_val);
				break;

			case TV_BOOK_LIFE:
				add_junko_flag_val(JKF1_FOOD, flag_val);
				add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_HEAL2, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_RESTORE, flag_val);
				add_junko_flag_val(JKF1_CURE_ALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ULTI_RES, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_HOLY, flag_val);
				break;
			case TV_BOOK_PUNISH:
				if (sv == 0) add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
				if (sv == 1) add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv == 2) add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
				if (sv == 1 || sv == 2) add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_HEAL2, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_WHIRL_WIND, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_CURE_ALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_BREATH_DEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_RES_MAGIC, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_LITE, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_HOLY, flag_val);

				break;
			case TV_BOOK_NATURE:
				add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				add_junko_flag_val(JKF1_FOOD, flag_val);
				add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 2)add_junko_flag_val(JKF1_HEAL2, flag_val);
				add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_RES_ELEM, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MOSES, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_EARTHQUAKES, flag_val);

				add_junko_flag_val(JKF2_ACID, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_WATER, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_WIND, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_STEAM, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_MAKE_TREE, flag_val);
				if (sv >= 2)  add_junko_flag_val(JKF2_MAKE_WALL, flag_val);
				break;
			case TV_BOOK_TRANSFORM:
				if (sv <= 1) add_junko_flag_val(JKF1_SELF_LIGHT, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_BREATH_DEF, flag_val);
				if (sv == 1 || sv == 2) add_junko_flag_val(JKF1_KILL_WALL, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_SPEED, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_SHIELD, flag_val);
				add_junko_flag_val(JKF2_ACID, flag_val);
				add_junko_flag_val(JKF2_ELEC, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_SHARDS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF2_MAKE_WALL, flag_val);
				break;
			case TV_BOOK_DARKNESS:
				if (sv <= 1) add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DUSTROBE, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MASS_GENO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_STASIS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_SILENT_FLOOR, flag_val);
				add_junko_flag_val(JKF2_FIRE, flag_val);
				add_junko_flag_val(JKF2_DARK, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_HELL, flag_val);
				break;
			case TV_BOOK_NECROMANCY:
				add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MENTAL_UP, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_RES_MAGIC, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SHIELD, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ADD_ESSENCE2, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DEATH_RAY, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_VANISH_BEAM, flag_val);
				add_junko_flag_val(JKF2_COLD, flag_val);
				add_junko_flag_val(JKF2_NETHER, flag_val);
				add_junko_flag_val(JKF1_DEBUFF, flag_val);
				break;
			case TV_BOOK_CHAOS:
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_ROCKET, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ATTACK_FLOOR, flag_val / 4);
				add_junko_flag_val(JKF1_TELEPO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DESTROY, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_CALL_CHAOS, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_TELE_AWAY_ALL, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_CALL_METEOR, flag_val);

				add_junko_flag_val(JKF2_CHAOS, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_MANA, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_DISENCHANT, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISTORTION, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
				break;
			case TV_BOOK_HISSATSU:
				add_junko_flag_val(JKF1_RUSH_ATTACK, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_WHIRL_WIND, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MASTER_MELEE, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_SHIELD, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MULTI_HIT, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_S_STEALTH, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_INVULNER, flag_val);
				break;
			case TV_BOOK_MEDICINE:
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_ROCKET, flag_val);
				add_junko_flag_val(JKF1_FOOD, flag_val);
				add_junko_flag_val(JKF1_HEAL1, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_HEAL2, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_RESTORE, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_RESTORE_NEXUS, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_SPEED, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_ULTI_RES, flag_val);
				add_junko_flag_val(JKF2_POIS, flag_val);

				break;
			case TV_BOOK_OCCULT:
				if (sv >= 1) add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_DUSTROBE, flag_val);
				add_junko_flag_val(JKF1_DETECT_MON, flag_val);
				add_junko_flag_val(JKF1_TELEPO, flag_val);
				add_junko_flag_val(JKF1_RUSH_ATTACK, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_DESTRUCT_DEF, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF1_PASS_WALL, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_IDENTIFY, flag_val);
				if (sv == 3) add_junko_flag_val(JKF1_MASS_GENO, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF2_WATER, flag_val);
				if (sv == 3) add_junko_flag_val(JKF2_MAKE_DOOR, flag_val);
				if (sv >= 2) add_junko_flag_val(JKF2_DISTORTION, flag_val);
				if (sv >= 1) add_junko_flag_val(JKF1_EARTHQUAKES, flag_val);

				break;
			}
		}

		//�����i(�J����)
		else if (tv >= TV_EQUIP_BEGIN && tv <= TV_WEARABLE_END)
		{
			flag_val = price * amt / 500;
			if (flag_val < amt) flag_val = amt * 5;

			if (tv == TV_ARROW || tv == TV_BOLT)
				add_junko_flag_val(JKF1_ATTACK_BOLT, flag_val);
			else if (tv == TV_KNIFE || tv == TV_SPEAR)
				add_junko_flag_val(JKF1_ATTACK_BEAM, flag_val);
			else if (tv == TV_SWORD || tv == TV_KATANA)
				add_junko_flag_val(JKF1_ATTACK_AROUND, flag_val);
			else if (tv == TV_STICK || tv == TV_HAMMER || tv == TV_AXE)
				add_junko_flag_val(JKF1_ATTACK_BALL, flag_val);
			else if (tv == TV_MAGICITEM || tv == TV_MAGICWEAPON)
				add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);
			else if (tv == TV_POLEARM || tv == TV_OTHERWEAPON)
				add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);
			else if (tv == TV_BOW || tv == TV_CROSSBOW)
				add_junko_flag_val(JKF1_ATTACK_ROCKET, flag_val);
			else if (tv == TV_GUN)
				add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
			else if (tv == TV_DRAG_ARMOR)
				add_junko_flag_val(JKF1_ATTACK_BREATH, flag_val);

			junko_gain_equipment_flags(o_ptr, amt, flgs);

		}
		else if (tv == TV_WHISTLE) //�J
		{
			flag_val = 100 * amt;
			add_junko_flag_val(JKF2_SOUND, flag_val);
		}
		else if (tv == TV_STAFF) //��
		{
			switch (sv)
			{
			case SV_STAFF_TELEPORTATION:
				add_junko_flag_val(JKF1_TELEPO, 100 * amt);
				break;
			case SV_STAFF_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 50 * amt);
				break;
			case SV_STAFF_REMOVE_CURSE:
				add_junko_flag_val(JKF1_REMOVE_CURSE, 50 * amt);
				break;
			case SV_STAFF_STARLITE:
				add_junko_flag_val(JKF1_ATTACK_BEAM, 30 * amt);
				add_junko_flag_val(JKF2_LITE, 30);
				break;
			case SV_STAFF_LITE:
				add_junko_flag_val(JKF1_SELF_LIGHT, 100 * amt);
				add_junko_flag_val(JKF2_LITE, 40 * amt);
				break;
			case SV_STAFF_MAPPING:
				add_junko_flag_val(JKF1_DETECT_ALL, 50 * amt);
				add_junko_flag_val(JKF1_ENLIGHT, 10 * amt);
				break;
			case SV_STAFF_DETECT_GOLD:
			case SV_STAFF_DETECT_ITEM:
				add_junko_flag_val(JKF1_DETECT_ALL, 50 * amt);
				add_junko_flag_val(JKF1_DETECT_OBJ, 100 * amt);
				break;
			case SV_STAFF_DETECT_TRAP:
			case SV_STAFF_DETECT_DOOR:
				add_junko_flag_val(JKF1_DETECT_ALL, 50 * amt);
				add_junko_flag_val(JKF1_DETECT_TRAPS, 100 * amt);
				break;
			case SV_STAFF_DETECT_INVIS:
			case SV_STAFF_DETECT_EVIL:
				add_junko_flag_val(JKF1_DETECT_ALL, 50 * amt);
				add_junko_flag_val(JKF1_DETECT_MON, 70 * amt);
				break;
			case SV_STAFF_CURE_LIGHT:
				add_junko_flag_val(JKF1_HEAL1, 30 * amt);
				break;
			case SV_STAFF_CURING:
				add_junko_flag_val(JKF1_CURE_ALL, 80 * amt);
				break;
			case SV_STAFF_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 200 * amt);
				break;
			case SV_STAFF_THE_MAGI:
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 1000 * amt);
				add_junko_flag_val(JKF2_MANA, 1000 * amt);
				break;
			case SV_STAFF_SPEED:
				add_junko_flag_val(JKF1_SPEED, 100 * amt);
				break;
			case SV_STAFF_PROBING:
				add_junko_flag_val(JKF1_IDENTIFY, 100 * amt);
				break;
			case SV_STAFF_DISPEL_EVIL:
				add_junko_flag_val(JKF2_HOLY, 40 * amt);
				break;
			case SV_STAFF_HOLINESS:
				add_junko_flag_val(JKF2_HOLY, 100 * amt);
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 100 * amt);
				break;
			case SV_STAFF_GENOCIDE:
				add_junko_flag_val(JKF1_MASS_GENO, 100 * amt);
				break;
			case SV_STAFF_EARTHQUAKES:
				add_junko_flag_val(JKF1_EARTHQUAKES, 100 * amt);
				break;
			case SV_STAFF_DESTRUCTION:
				add_junko_flag_val(JKF1_DESTROY, 100 * amt);
				break;
			case SV_STAFF_DARKNESS:
				add_junko_flag_val(JKF2_DARK, 50 * amt);
				break;
			case SV_STAFF_MSTORM:
				add_junko_flag_val(JKF2_MANA, 300 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 400 * amt);

				break;
			}

		}
		else if (tv == TV_FIGURINE || tv == TV_CAPTURE || tv == TV_BUNBUN || tv == TV_KAKASHI) //�l�`�Ɨd���{�ƐV�� �ϐ��ƃ��x���ɉ����đ����t���O�l�𓾂�
		{
			monster_race *r_ptr = &r_info[pv];
			flag_val = r_ptr->level * r_ptr->level / (tv == TV_FIGURINE ? 20 : 100);
			if (flag_val < 10) flag_val = 10;
			flag_val *= amt;

			if (r_ptr->flagsr & RFR_IM_ACID) add_junko_flag_val(JKF2_ACID, flag_val);
			if (r_ptr->flagsr & RFR_IM_FIRE) add_junko_flag_val(JKF2_FIRE, flag_val);
			if (r_ptr->flagsr & RFR_IM_ELEC) add_junko_flag_val(JKF2_ELEC, flag_val);
			if (r_ptr->flagsr & RFR_IM_COLD) add_junko_flag_val(JKF2_COLD, flag_val);
			if (r_ptr->flagsr & RFR_IM_POIS) add_junko_flag_val(JKF2_POIS, flag_val);
			if (r_ptr->flagsr & RFR_RES_LITE) add_junko_flag_val(JKF2_LITE, flag_val);
			if (r_ptr->flagsr & RFR_RES_DARK) add_junko_flag_val(JKF2_DARK, flag_val);
			if (r_ptr->flagsr & RFR_RES_NETH) add_junko_flag_val(JKF2_NETHER, flag_val);
			if (r_ptr->flagsr & RFR_RES_WATE) add_junko_flag_val(JKF2_WATER, flag_val);
			if (r_ptr->flagsr & RFR_RES_SHAR) add_junko_flag_val(JKF2_SHARDS, flag_val);
			if (r_ptr->flagsr & RFR_RES_SOUN) add_junko_flag_val(JKF2_SOUND, flag_val);
			if (r_ptr->flagsr & RFR_RES_CHAO) add_junko_flag_val(JKF2_CHAOS, flag_val);
			if (r_ptr->flagsr & RFR_RES_HOLY) add_junko_flag_val(JKF2_HOLY, flag_val);
			if (r_ptr->flagsr & RFR_RES_DISE) add_junko_flag_val(JKF2_DISENCHANT, flag_val);
			if (r_ptr->flagsr & RFR_RES_TIME) add_junko_flag_val(JKF2_TIME, flag_val);
		}
		else if (tv == TV_WAND)//���@�_
		{
			switch (sv)
			{
			case SV_WAND_TELEPORT_AWAY:
				add_junko_flag_val(JKF1_TELE_AWAY_ALL, 30 * amt);
				break;
			case SV_WAND_DISARMING:
			case SV_WAND_TRAP_DOOR_DEST:
				add_junko_flag_val(JKF1_DISARM, 50 * amt);
				break;
			case SV_WAND_STONE_TO_MUD:
				add_junko_flag_val(JKF2_DISINTEGRATE, 30 * amt);
				break;
			case SV_WAND_LITE:
				add_junko_flag_val(JKF2_LITE, 30 * amt);
				break;
			case SV_WAND_SLEEP_MONSTER:
			case SV_WAND_SLOW_MONSTER:
			case SV_WAND_CONFUSE_MONSTER:
			case SV_WAND_FEAR_MONSTER:
			case SV_WAND_CHARM_MONSTER:
				add_junko_flag_val(JKF1_DAZZLE, 10 * amt);
				add_junko_flag_val(JKF1_STASIS, 20 * amt);
				break;
			case SV_WAND_DRAIN_LIFE:
				add_junko_flag_val(JKF2_NETHER, 50 * amt);
				break;
			case SV_WAND_POLYMORPH:
				add_junko_flag_val(JKF2_CHAOS, 25 * amt);
				break;
			case SV_WAND_STINKING_CLOUD:
				add_junko_flag_val(JKF2_POIS, 20 * amt);
				break;
			case SV_WAND_MAGIC_MISSILE:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				break;
			case SV_WAND_ACID_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_ACID, 30 * amt);
				break;
			case SV_WAND_FIRE_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_FIRE, 30 * amt);
				break;
			case SV_WAND_COLD_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_COLD, 30 * amt);
				break;
			case SV_WAND_ACID_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_ACID, 50 * amt);
				break;
			case SV_WAND_ELEC_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_ELEC, 50 * amt);
				break;
			case SV_WAND_FIRE_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_FIRE, 50 * amt);
				break;
			case SV_WAND_COLD_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 40 * amt);
				add_junko_flag_val(JKF2_COLD, 50 * amt);
				break;
			case SV_WAND_DISINTEGRATE:
				add_junko_flag_val(JKF1_ATTACK_BALL, 100 * amt);
				add_junko_flag_val(JKF2_DISINTEGRATE, 100 * amt);
				break;
			case SV_WAND_DRAGON_FIRE:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 100 * amt);
				add_junko_flag_val(JKF2_FIRE, 200 * amt);
				break;
			case SV_WAND_DRAGON_COLD:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 100 * amt);
				add_junko_flag_val(JKF2_COLD, 200 * amt);
				break;
			case SV_WAND_DRAGON_BREATH:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 200 * amt);
				break;
			case SV_WAND_ROCKETS:
				add_junko_flag_val(JKF1_ATTACK_ROCKET, 500 * amt);
				add_junko_flag_val(JKF2_SHARDS, 500 * amt);
				break;
			case SV_WAND_STRIKING:
				add_junko_flag_val(JKF2_METEOR, 500 * amt);
				break;
			case SV_WAND_GENOCIDE:
				add_junko_flag_val(JKF1_MASS_GENO, 100 * amt);
				break;
			}
		}



		else if (tv == TV_ROD) //���b�h
		{
			switch (sv)
			{
			case SV_ROD_DETECT_TRAP:
			case SV_ROD_DETECT_DOOR:
				add_junko_flag_val(JKF1_DETECT_ALL, 50 * amt);
				add_junko_flag_val(JKF1_DETECT_TRAPS, 50 * amt);
				break;
			case SV_ROD_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 300 * amt);
				break;
			case SV_ROD_RECALL:
				add_junko_flag_val(JKF1_TELEPO, 100 * amt);
				break;
			case SV_ROD_ILLUMINATION:
				add_junko_flag_val(JKF1_SELF_LIGHT, 100 * amt);
				break;
			case SV_ROD_MAPPING:
				add_junko_flag_val(JKF1_DETECT_ALL, 50 * amt);
				add_junko_flag_val(JKF1_ENLIGHT, 50 * amt);
				break;
			case SV_ROD_DETECTION:
				add_junko_flag_val(JKF1_DETECT_ALL, 400 * amt);
				break;
			case SV_ROD_PROBING:
				add_junko_flag_val(JKF1_DETECT_MON, 200 * amt);
				break;
			case SV_ROD_CURING:
				add_junko_flag_val(JKF1_CURE_ALL, 200 * amt);
				break;
			case SV_ROD_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 500 * amt);
				break;
			case SV_ROD_RESTORATION:
				add_junko_flag_val(JKF1_RESTORE, 200 * amt);
				break;
			case SV_ROD_SPEED:
				add_junko_flag_val(JKF1_SPEED, 300 * amt);
				break;
			case SV_ROD_PESTICIDE:
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 20 * amt);
				break;
			case SV_ROD_TELEPORT_AWAY:
				add_junko_flag_val(JKF1_TELE_AWAY_ALL, 50 * amt);
				break;
			case SV_ROD_DISARMING:
				add_junko_flag_val(JKF1_DISARM, 100 * amt);
				break;
			case SV_ROD_LITE:
				add_junko_flag_val(JKF2_LITE, 30 * amt);
				break;
			case SV_ROD_SLEEP_MONSTER:
			case SV_ROD_SLOW_MONSTER:
				add_junko_flag_val(JKF1_DAZZLE, 10 * amt);
				add_junko_flag_val(JKF1_STASIS, 30 * amt);
				break;
			case SV_ROD_DRAIN_LIFE:
				add_junko_flag_val(JKF2_NETHER, 80 * amt);
				break;
			case SV_ROD_POLYMORPH:
				add_junko_flag_val(JKF1_DAZZLE, 10 * amt);
				add_junko_flag_val(JKF2_CHAOS, 20 * amt);
				break;
			case SV_ROD_ACID_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_ACID, 50 * amt);
				break;
			case SV_ROD_ELEC_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_ELEC, 50 * amt);
				break;
			case SV_ROD_FIRE_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_FIRE, 50 * amt);
				break;
			case SV_ROD_COLD_BOLT:
				add_junko_flag_val(JKF1_ATTACK_BOLT, 30 * amt);
				add_junko_flag_val(JKF2_COLD, 50 * amt);
				break;
			case SV_ROD_ACID_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_ACID, 100 * amt);
				break;
			case SV_ROD_ELEC_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_ELEC, 100 * amt);
				break;
			case SV_ROD_FIRE_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_FIRE, 100 * amt);
				break;
			case SV_ROD_COLD_BALL:
				add_junko_flag_val(JKF1_ATTACK_BALL, 60 * amt);
				add_junko_flag_val(JKF2_COLD, 100 * amt);
				break;
			case SV_ROD_HAVOC:
				add_junko_flag_val(JKF1_DAZZLE, 100 * amt);
				add_junko_flag_val(JKF1_CALL_CHAOS, 500 * amt);
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 500 * amt);
				break;
			case SV_ROD_STONE_TO_MUD:
				add_junko_flag_val(JKF1_KILL_WALL, 50 * amt);
				add_junko_flag_val(JKF2_DISINTEGRATE, 50 * amt);
				break;

			}
		}
		else if (tv == TV_SCROLL)//����
		{

			switch (sv)
			{
			case SV_SCROLL_DARKNESS:
				add_junko_flag_val(JKF2_DARK, 5 * amt);
				break;
			case SV_SCROLL_CURSE_ARMOR:
			case SV_SCROLL_CURSE_WEAPON:
				add_junko_flag_val(JKF2_DISENCHANT, 50 * amt);
				break;
			case SV_SCROLL_TRAP_CREATION:
				add_junko_flag_val(JKF2_MAKE_DOOR, 20 * amt);
				break;
			case SV_SCROLL_PHASE_DOOR:
				add_junko_flag_val(JKF1_TELEPO, 2 * amt);
				break;
			case SV_SCROLL_TELEPORT:
				add_junko_flag_val(JKF1_TELEPO, 5 * amt);
				break;
			case SV_SCROLL_TELEPORT_LEVEL:
			case SV_SCROLL_WORD_OF_RECALL:
				add_junko_flag_val(JKF1_TELEPO, 10 * amt);
				break;
			case SV_SCROLL_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 3 * amt);
				break;
			case SV_SCROLL_STAR_IDENTIFY:
				add_junko_flag_val(JKF1_IDENTIFY, 50 * amt);
				break;
			case SV_SCROLL_REMOVE_CURSE:
				add_junko_flag_val(JKF1_REMOVE_CURSE, 5 * amt);
				break;
			case SV_SCROLL_STAR_REMOVE_CURSE:
				add_junko_flag_val(JKF1_REMOVE_CURSE, 50 * amt);
				break;
			case SV_SCROLL_ENCHANT_ARMOR:
				add_junko_flag_val(JKF1_ADD_ESSENCE2, 10 * amt);
				break;
			case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
			case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
				add_junko_flag_val(JKF1_ADD_ESSENCE1, 10 * amt);
				break;
			case SV_SCROLL_STAR_ENCHANT_ARMOR:
				add_junko_flag_val(JKF1_ADD_ESSENCE2, 50 * amt);
				break;
			case SV_SCROLL_STAR_ENCHANT_WEAPON:
				add_junko_flag_val(JKF1_ADD_ESSENCE1, 50 * amt);
				break;
			case SV_SCROLL_RECHARGING:
				add_junko_flag_val(JKF1_MASTER_MAGITEM, 5 * amt);
				break;
			case SV_SCROLL_MUNDANITY:
				add_junko_flag_val(JKF1_SILENT_FLOOR, 20 * amt);
				break;
			case SV_SCROLL_LIGHT:
				add_junko_flag_val(JKF1_SELF_LIGHT, 10 * amt);
				break;
			case SV_SCROLL_MAPPING:
				add_junko_flag_val(JKF1_ENLIGHT, 2 * amt);
				break;
			case SV_SCROLL_DETECT_GOLD:
			case SV_SCROLL_DETECT_ITEM:
				add_junko_flag_val(JKF1_DETECT_OBJ, 5 * amt);
				break;
			case SV_SCROLL_DETECT_TRAP:
			case SV_SCROLL_DETECT_DOOR:
				add_junko_flag_val(JKF1_DETECT_TRAPS, 5 * amt);
				break;
			case SV_SCROLL_DETECT_INVIS:
				add_junko_flag_val(JKF1_DETECT_MON, 5 * amt);
				break;
			case SV_SCROLL_HOLY_CHANT:
				add_junko_flag_val(JKF1_RES_MAGIC, 3 * amt);
				break;
			case SV_SCROLL_PROTECTION_FROM_EVIL:
				add_junko_flag_val(JKF1_SHIELD, 20 * amt);
				break;
			case SV_SCROLL_RUNE_OF_PROTECTION:
				add_junko_flag_val(JKF1_DISTURB_SUMMON, 20 * amt);
				break;
			case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
				add_junko_flag_val(JKF1_DISARM, 5 * amt);
				break;
			case SV_SCROLL_STAR_DESTRUCTION:
				add_junko_flag_val(JKF1_DESTROY, 20 * amt);
				break;
			case SV_SCROLL_DISPEL_UNDEAD:
				add_junko_flag_val(JKF2_HOLY, 20 * amt);
				break;
			case SV_SCROLL_SPELL:
				add_junko_flag_val(JKF1_ABSORB_MAGIC, 50 * amt);
				break;
			case SV_SCROLL_GENOCIDE:
			case SV_SCROLL_MASS_GENOCIDE:
				add_junko_flag_val(JKF1_MASS_GENO, 30 * amt);
				break;
			case SV_SCROLL_ACQUIREMENT:
				add_junko_flag_val(JKF1_LUCKY, 100 * amt);
				break;
			case SV_SCROLL_STAR_ACQUIREMENT:
				add_junko_flag_val(JKF1_LUCKY, 1000 * amt);
				break;
			case SV_SCROLL_FIRE:
				add_junko_flag_val(JKF2_FIRE, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 1000 * amt);
				break;
			case SV_SCROLL_ICE:
				add_junko_flag_val(JKF2_COLD, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 1000 * amt);
				break;
			case SV_SCROLL_CHAOS:
				add_junko_flag_val(JKF2_CHAOS, 500 * amt);
				add_junko_flag_val(JKF1_ATTACK_AROUND, 1000 * amt);
				break;
			case SV_SCROLL_ARTIFACT:
				add_junko_flag_val(JKF2_METEOR, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 1000 * amt);

				break;
			}
		}
		else if (tv == TV_SOUVENIR) //���i
		{

			switch (sv)
			{
			case SV_SOUVENIR_GAMEBOY:
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 1000 * amt);
				break;
			case SV_SOUVENIR_ILMENITE:
				add_junko_flag_val(JKF1_MAGICDEF, 300 * amt);
				break;
			case SV_SOUVENIR_PHOENIX_FEATHER:
				add_junko_flag_val(JKF2_NUKE, 1000 * amt);
				add_junko_flag_val(JKF1_HEAL2, 1000 * amt);
				add_junko_flag_val(JKF1_EXPLOSION_DEF, 1000 * amt);
				add_junko_flag_val(JKF1_BREATH_DEF, 1000 * amt);
				break;
			case SV_SOUVENIR_SMARTPHONE:
				add_junko_flag_val(JKF1_D_DOOR, 300 * amt);
				add_junko_flag_val(JKF1_ALTER_REALITY, 300 * amt);
				break;
			case SV_SOUVENIR_KAPPA_5:
				add_junko_flag_val(JKF1_RES_ELEM, 1000 * amt);
				add_junko_flag_val(JKF1_LUCKY, 1000 * amt);
				break;
			case SV_SOUVENIR_PRISM:
				add_junko_flag_val(JKF2_LITE, 300 * amt);
				add_junko_flag_val(JKF2_FIRE, 300 * amt);
				add_junko_flag_val(JKF2_ELEC, 300 * amt);
				add_junko_flag_val(JKF2_COLD, 300 * amt);
				break;
			case SV_SOUVENIR_KAPPA_ARM:
				add_junko_flag_val(JKF1_MULTI_HIT, 300 * amt);
				add_junko_flag_val(JKF1_HEAL2, 300 * amt);
				break;
			case SV_SOUVENIR_BIG_EGG:
				add_junko_flag_val(JKF1_DESTRUCT_DEF, 1000 * amt);
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 1000 * amt);
				break;
			case SV_SOUVENIR_ASIA:
				add_junko_flag_val(JKF2_LITE, 1000 * amt);
				add_junko_flag_val(JKF2_HOLY, 1000 * amt);
				add_junko_flag_val(JKF2_METEOR, 1000 * amt);
				break;
			case SV_SOUVENIR_ULTRASONIC:
				add_junko_flag_val(JKF1_MASS_GENO, 300 * amt);
				break;
			case SV_SOUVENIR_STARS_AND_STRIPES:
				add_junko_flag_val(JKF1_MUSCLE_UP, 1000 * amt);
				add_junko_flag_val(JKF1_MENTAL_UP, 1000 * amt);
				add_junko_flag_val(JKF1_CALL_CHAOS, 1000 * amt);
				break;
			case SV_SOUVENIR_OUIJA_BOARD:
				add_junko_flag_val(JKF1_DETECT_MON, 500 * amt);
				add_junko_flag_val(JKF1_DETECT_OBJ, 500 * amt);
				add_junko_flag_val(JKF1_DETECT_TRAPS, 500 * amt);
				break;
			case SV_SOUVENIR_KINKAKUJI:
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 1000 * amt);
				add_junko_flag_val(JKF1_CALL_METEOR, 1000 * amt);
				break;
			case SV_SOUVENIR_KESERAN_PASARAN:
				add_junko_flag_val(JKF1_VANISH_BEAM, 1000 * amt);
				add_junko_flag_val(JKF1_LUCKY, 200 * amt);
				break;
			case SV_SOUVENIR_MARISA_TUBO:
				add_junko_flag_val(JKF1_SILENT_FLOOR, 300 * amt);
				break;
			case SV_SOUVENIR_DOUJU:
				add_junko_flag_val(JKF1_HEAL2, 300 * amt);
				add_junko_flag_val(JKF2_FIRE, 500 * amt);
				add_junko_flag_val(JKF2_WATER, 500 * amt);
				break;

			case SV_SOUVENIR_KODOKUZARA:
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 1000 * amt);
				add_junko_flag_val(JKF2_POIS, 1000 * amt);
				add_junko_flag_val(JKF2_HELL, 1000 * amt);
				break;

			}
		}
		else if (tv == TV_MATERIAL) //�f��
		{
			switch (sv)
			{
			case SV_MATERIAL_HIKARIGOKE:
				add_junko_flag_val(JKF1_SELF_LIGHT, 100 * amt);
				break;
			case SV_MATERIAL_HEMATITE:
			case SV_MATERIAL_MAGNETITE:
				add_junko_flag_val(JKF2_MANA, 10 * amt);
				break;
			case SV_MATERIAL_ARSENIC:
			case SV_MATERIAL_MERCURY:
				add_junko_flag_val(JKF1_DEBUFF, 50 * amt);
				add_junko_flag_val(JKF2_POIS, 50 * amt);
				break;
			case SV_MATERIAL_MITHRIL:
				add_junko_flag_val(JKF2_HOLY, 50 * amt);
				break;
			case SV_MATERIAL_ADAMANTITE:
				add_junko_flag_val(JKF1_SHIELD, 50 * amt);
				break;
			case SV_MATERIAL_DRAGONNAIL:
				add_junko_flag_val(JKF1_HEAL2, 200 * amt);
				add_junko_flag_val(JKF1_WHIRL_WIND, 200 * amt);
				add_junko_flag_val(JKF1_MULTI_HIT, 100 * amt);

				break;
			case SV_MATERIAL_DRAGONSCALE:
				add_junko_flag_val(JKF2_FIRE, 200 * amt);
				add_junko_flag_val(JKF2_ELEC, 200 * amt);
				add_junko_flag_val(JKF2_ACID, 200 * amt);
				add_junko_flag_val(JKF2_COLD, 200 * amt);
				add_junko_flag_val(JKF1_RES_MAGIC, 100 * amt);
				break;
			case SV_MATERIAL_GELSEMIUM:
				add_junko_flag_val(JKF2_POIS, 500 * amt);
				break;
			case SV_MATERIAL_DIAMOND:
				add_junko_flag_val(JKF1_DESTRUCT_DEF, 500 * amt);
				break;
			case SV_MATERIAL_EMERALD:
				add_junko_flag_val(JKF1_BREATH_DEF, 500 * amt);
				break;
			case SV_MATERIAL_RUBY:
				add_junko_flag_val(JKF1_EXPLOSION_DEF, 500 * amt);
				break;
			case SV_MATERIAL_SAPPHIRE:
				add_junko_flag_val(JKF1_VANISH_BEAM, 500 * amt);
				break;
			case SV_MATERIAL_METEORICIRON:
				add_junko_flag_val(JKF1_CALL_METEOR, 1000 * amt);
				add_junko_flag_val(JKF2_METEOR, 1000 * amt);
				break;
			case SV_MATERIAL_HIHIIROKANE:
				for (i = 1; i <= JKF1_MAX; i++) p_ptr->magic_num2[i] = 255;//�S�t���O�lMAX
				break;
			case SV_MATERIAL_ICESCALE:
				add_junko_flag_val(JKF2_COLD, 300 * amt);
				add_junko_flag_val(JKF2_SHARDS, 100 * amt);
				break;
			case SV_MATERIAL_NIGHTMARE_FRAGMENT:
				add_junko_flag_val(JKF1_DEBUFF, 50 * amt);
				add_junko_flag_val(JKF1_STASIS, 300 * amt);
				add_junko_flag_val(JKF1_DEATH_RAY, 300 * amt);
				break;
			case SV_MATERIAL_HOPE_FRAGMENT:
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 300 * amt);
				add_junko_flag_val(JKF1_MOSES, 300 * amt);
				break;
			case SV_MATERIAL_TAKAKUSAGORI:
				add_junko_flag_val(JKF1_HEAL2, 300 * amt);
				add_junko_flag_val(JKF1_CURE_ALL, 300 * amt);
				add_junko_flag_val(JKF1_RESTORE, 300 * amt);
				break;
			case SV_MATERIAL_IZANAGIOBJECT:
				add_junko_flag_val(JKF1_ALTER_REALITY, 500 * amt);
				add_junko_flag_val(JKF1_TELE_AWAY_ALL, 300 * amt);
				add_junko_flag_val(JKF2_TIME, 300 * amt);
				break;

			case SV_MATERIAL_RYUUZYU:
				add_junko_flag_val(JKF1_BREATH_DEF, 300 * amt);
				add_junko_flag_val(JKF2_MANA, 100 * amt);

				break;


			case SV_MATERIAL_ISHIZAKURA:
			case SV_MATERIAL_SKULL:
				add_junko_flag_val(JKF2_NETHER, 100 * amt);
				break;
			}
		}

		else if (tv == TV_MUSHROOM)
		{

			switch (sv)
			{
			case SV_MUSHROOM_RESTORING:
				add_junko_flag_val(JKF1_RESTORE, 50 * amt);
				break;
			case SV_MUSHROOM_MON_SUPER:
				add_junko_flag_val(JKF1_MUSCLE_UP, 1000 * amt);
				add_junko_flag_val(JKF1_ATTACK_SPARK, 1000 * amt);
				break;
			}
		}
		else if (tv == TV_POTION) //��
		{

			switch (sv)
			{
			case SV_POTION_POISON:
				add_junko_flag_val(JKF2_POIS, 10 * amt);
				break;
			case SV_POTION_SLOWNESS:
			case SV_POTION_BLINDNESS:
			case SV_POTION_SLEEP:
				add_junko_flag_val(JKF1_STASIS, 10 * amt);
				break;
			case SV_POTION_RUINATION:
				add_junko_flag_val(JKF1_DEBUFF, 300 * amt);
				add_junko_flag_val(JKF1_CALL_CHAOS, 50 * amt);
				break;
			case SV_POTION_DETONATIONS:
				add_junko_flag_val(JKF1_ATTACK_BALL, 300 * amt);
				add_junko_flag_val(JKF2_SHARDS, 300 * amt);
				break;
			case SV_POTION_DEATH:
				add_junko_flag_val(JKF1_DEATH_RAY, 300 * amt);
				break;
			case SV_POTION_SLOW_POISON:
			case SV_POTION_CURE_POISON:
			case SV_POTION_BOLDNESS:
				add_junko_flag_val(JKF1_CURE_ALL, 10 * amt);
				break;
			case SV_POTION_SPEED:
				add_junko_flag_val(JKF1_SPEED, 40 * amt);
				break;
			case SV_POTION_RESIST_HEAT:
			case SV_POTION_RESIST_COLD:
				add_junko_flag_val(JKF1_RES_ELEM, 10 * amt);
				break;
			case SV_POTION_CURE_CRITICAL:
				add_junko_flag_val(JKF1_HEAL1, 10 * amt);
				break;
			case SV_POTION_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 20 * amt);
				break;
			case SV_POTION_STAR_HEALING:
				add_junko_flag_val(JKF1_HEAL2, 50 * amt);
				break;
			case SV_POTION_LIFE:
				add_junko_flag_val(JKF1_HEAL2, 100 * amt);
				add_junko_flag_val(JKF1_CURE_ALL, 100 * amt);
				add_junko_flag_val(JKF1_RESTORE, 100 * amt);
				break;
			case SV_POTION_RESTORE_MANA:
				add_junko_flag_val(JKF1_ABSORB_MAGIC, 50 * amt);
				break;
			case SV_POTION_RESTORE_EXP:
			case SV_POTION_RES_STR:
			case SV_POTION_RES_INT:
			case SV_POTION_RES_WIS:
			case SV_POTION_RES_CON:
			case SV_POTION_RES_DEX:
			case SV_POTION_RES_CHR:
				add_junko_flag_val(JKF1_RESTORE, 10 * amt);
				break;
			case SV_POTION_INC_STR:
			case SV_POTION_INC_CON:
			case SV_POTION_INC_DEX:
				add_junko_flag_val(JKF1_MUSCLE_UP, 100 * amt);
				break;
			case SV_POTION_INC_INT:
			case SV_POTION_INC_WIS:
			case SV_POTION_INC_CHR:
				add_junko_flag_val(JKF1_MENTAL_UP, 100 * amt);
				break;
			case SV_POTION_AUGMENTATION:
				add_junko_flag_val(JKF1_MUSCLE_UP, 500 * amt);
				add_junko_flag_val(JKF1_MENTAL_UP, 500 * amt);
				break;
			case SV_POTION_ENLIGHTENMENT:
				add_junko_flag_val(JKF1_ENLIGHT, 50 * amt);
				break;
			case SV_POTION_STAR_ENLIGHTENMENT:
				add_junko_flag_val(JKF1_ENLIGHT, 500 * amt);
				break;
			case SV_POTION_SELF_KNOWLEDGE:
				add_junko_flag_val(JKF1_MASTER_MELEE, 50 * amt);
				break;
			case SV_POTION_EXPERIENCE:
				add_junko_flag_val(JKF1_MASTER_MELEE, 200 * amt);
				break;
			case SV_POTION_RESISTANCE:
				add_junko_flag_val(JKF1_RES_ELEM, 50 * amt);
				break;
			case SV_POTION_CURING:
				add_junko_flag_val(JKF1_CURE_ALL, 50 * amt);
				add_junko_flag_val(JKF1_HEAL1, 30 * amt);
				break;
			case SV_POTION_INVULNERABILITY:
				add_junko_flag_val(JKF1_INVULNER, 50 * amt);
				break;
			case SV_POTION_NEW_LIFE:
				add_junko_flag_val(JKF1_RESTORE_NEXUS, 100 * amt);
				break;
			case SV_POTION_POLYMORPH:
				add_junko_flag_val(JKF2_CHAOS, 100 * amt);
				break;
			case SV_POTION_NANIKASUZUSHIKUNARU:
				add_junko_flag_val(JKF2_COLD, 1000 * amt);
				break;
			case SV_POTION_POISON2:
				add_junko_flag_val(JKF2_POIS, 100 * amt);
				break;
			case SV_POTION_LOVE:
				add_junko_flag_val(JKF1_DESTRUCT_DEF, 100 * amt);
				break;

			}
		}

		else if (tv == TV_ALCOHOL)
		{

			switch (sv)
			{
			case SV_ALCOHOL_GOLDEN_MEAD:
				add_junko_flag_val(JKF1_MENTAL_UP, 200 * amt);
				add_junko_flag_val(JKF1_RES_ELEM, 200 * amt);
				break;
			case SV_ALCOHOL_MOON:
				add_junko_flag_val(JKF1_ALTER_REALITY, 100 * amt);
				break;
			case SV_ALCOHOL_NECTAR:
				add_junko_flag_val(JKF1_EXPLOSION_DEF, 100 * amt);
				add_junko_flag_val(JKF1_BREATH_DEF, 100 * amt);
				break;
			case SV_ALCOHOL_SYUTYUU:
				add_junko_flag_val(JKF1_MUSCLE_UP, 100 * amt);
				break;
			case SV_ALCOHOL_TRUE_ONIKOROSHI:
				add_junko_flag_val(JKF1_DESTROY, 300 * amt);
				break;
			case SV_ALCOHOL_KUSHINADA:
				add_junko_flag_val(JKF1_STASIS, 300 * amt);
				break;
			}
		}
		else if (tv == TV_COMPOUND)//������@EXTRA�ł��ǂ񂰂����ɓ��镪����
		{

			switch (sv)
			{
			case SV_COMPOUND_MUSCLE_DRUG:
				add_junko_flag_val(JKF1_MUSCLE_UP, 50 * amt);
				break;
			case SV_COMPOUND_HAPPY:
				add_junko_flag_val(JKF1_LUCKY, 10 * amt);
				break;
			case SV_COMPOUND_BREATH_OF_FIRE:
				add_junko_flag_val(JKF1_ATTACK_BREATH, 50 * amt);
				add_junko_flag_val(JKF2_FIRE, 50 * amt);
				break;
			case SV_COMPOUND_JEKYLL:
				add_junko_flag_val(JKF1_MASTER_MELEE, 50 * amt);
				break;
			case SV_COMPOUND_METAMOR_BEAST:
				add_junko_flag_val(JKF1_MUSCLE_UP, 100 * amt);
				add_junko_flag_val(JKF1_RUSH_ATTACK, 100 * amt);
				add_junko_flag_val(JKF2_CHAOS, 50 * amt);
				add_junko_flag_val(JKF1_MASTER_MELEE, 100 * amt);
				break;
			case SV_COMPOUND_STAR_DETORNATION:
				add_junko_flag_val(JKF1_ATTACK_SIGHT, 200 * amt);
				add_junko_flag_val(JKF1_ATTACK_FLOOR, 50 * amt);
				add_junko_flag_val(JKF2_NUKE, 500 * amt);
				break;
			}
		}

	}

	if (price < 100)
	{
		msg_format("%s�͏��ł������A��͂𓾂��Ȃ������B", o_name);

	}
	else
	{
		price /= 100;

		if (amt > 1) price *= amt;
		if (price > 999) price = 999;
		msg_format("%s��%d��MP�ƂȂ��ď��ł����B", o_name, price);
		player_gain_mana(price);
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

//���ς������i�t���O�ɂ���ē��Z�t���O�l�𓾂�B
//��̏����R�}���h����g���̂ŕ���..���������ǈ�ӏ����炵���g���ĂȂ�
void junko_gain_equipment_flags(object_type *o_ptr, int amt, u32b flgs[TR_FLAG_SIZE])
{
	int i;
	int flag_val;
	int price;
	int tv = o_ptr->tval;
	int sv = o_ptr->sval;
	int pv = o_ptr->pval;

	if (p_ptr->pclass != CLASS_JUNKO) { msg_print("ERROR:���ψȊO��junko_gain_equipment_flags()���Ă΂ꂽ"); return; }

	price = object_value_real(o_ptr);

	flag_val = price * amt / 500;
	if (flag_val < amt) flag_val = amt * 5;

	if (object_is_artifact(o_ptr))
	{
		if (object_is_shooting_weapon(o_ptr)) add_junko_flag_val(JKF1_ATTACK_FLOOR, flag_val);
		else if (object_is_melee_weapon(o_ptr))
		{
			if (tv == TV_SPEAR || tv == TV_ARROW || tv == TV_BOLT)
				add_junko_flag_val(JKF1_RUSH_ATTACK, flag_val);
			else if (tv == TV_KNIFE)
				add_junko_flag_val(JKF1_MULTI_HIT, flag_val);
			else
				add_junko_flag_val(JKF1_WHIRL_WIND, flag_val);

			add_junko_flag_val(JKF1_MASTER_MELEE, flag_val);

		}
		else if (tv == TV_CLOTHES || tv == TV_ARMOR || tv == TV_DRAG_ARMOR)
		{
			if (price > 80000) add_junko_flag_val(JKF1_MAGICDEF, flag_val);
			else if (price > 30000) add_junko_flag_val(JKF1_RES_MAGIC, flag_val);
			else if (price > 10000) add_junko_flag_val(JKF1_SHIELD, flag_val);

		}
		else if (tv == TV_RIBBON)
		{
			add_junko_flag_val(JKF1_ATTACK_SPARK, flag_val);
		}
		else add_junko_flag_val(JKF1_ATTACK_SIGHT, flag_val);

		if (have_flag(flgs, TR_REGEN)) add_junko_flag_val(JKF1_HEAL2, flag_val);
		if (have_flag(flgs, TR_TELEPORT)) add_junko_flag_val(JKF1_D_DOOR, flag_val);
		if (have_flag(flgs, TR_NO_MAGIC)) add_junko_flag_val(JKF1_EXPLOSION_DEF, flag_val);
		if (have_flag(flgs, TR_NO_TELE)) add_junko_flag_val(JKF1_DESTRUCT_DEF, flag_val);

		if (have_flag(flgs, TR_ACTIVATE) && price > 30000) add_junko_flag_val(JKF1_MASTER_MAGITEM, flag_val);


		if (object_is_weapon_ammo(o_ptr)) 
			add_junko_flag_val(JKF1_ADD_ESSENCE1, flag_val);
		else 
			add_junko_flag_val(JKF1_ADD_ESSENCE2, flag_val);

	}
	else
	{
		if (have_flag(flgs, TR_REGEN)) add_junko_flag_val(JKF1_HEAL1, flag_val);
		if (have_flag(flgs, TR_TELEPORT)) add_junko_flag_val(JKF1_TELEPO, flag_val);
		if (have_flag(flgs, TR_NO_MAGIC)) add_junko_flag_val(JKF1_ABSORB_MAGIC, flag_val);

	}

	if (have_flag(flgs, TR_SLOW_DIGEST)) add_junko_flag_val(JKF1_FOOD, flag_val);
	for (i = TR_ESP_EVIL; i <= TR_ESP_UNIQUE; i++) if (have_flag(flgs, i)) add_junko_flag_val(JKF1_DETECT_MON, flag_val / 3);
	if (have_flag(flgs, TR_WARNING)) add_junko_flag_val(JKF1_DETECT_TRAPS, flag_val);
	for (i = TR_SUST_STR; i <= TR_SUST_CHR; i++) if (have_flag(flgs, i)) add_junko_flag_val(JKF1_RESTORE, flag_val / 2);

	if (have_flag(flgs, TR_FREE_ACT) || have_flag(flgs, TR_RES_FEAR) || have_flag(flgs, TR_RES_BLIND) || have_flag(flgs, TR_RES_CONF))
		add_junko_flag_val(JKF1_CURE_ALL, flag_val);

	if (have_flag(flgs, TR_BLESSED)) add_junko_flag_val(JKF1_REMOVE_CURSE, flag_val);
	if (have_flag(flgs, TR_REFLECT)) add_junko_flag_val(JKF1_VANISH_BEAM, flag_val);
	if (have_flag(flgs, TR_IMPACT)) add_junko_flag_val(JKF1_EARTHQUAKES, flag_val);

	if (have_flag(flgs, TR_MAGIC_MASTERY)) add_junko_flag_val(JKF1_MASTER_MAGITEM, flag_val * pv / 4);
	if (have_flag(flgs, TR_STR)) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_CON)) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_DEX)) add_junko_flag_val(JKF1_MUSCLE_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_INT)) add_junko_flag_val(JKF1_MENTAL_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_WIS)) add_junko_flag_val(JKF1_MENTAL_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_CHR)) add_junko_flag_val(JKF1_MENTAL_UP, flag_val * pv / 10);
	if (have_flag(flgs, TR_SPEED)) add_junko_flag_val(JKF1_SPEED, flag_val * pv / 2);
	if (have_flag(flgs, TR_TUNNEL)) add_junko_flag_val(JKF1_KILL_WALL, flag_val * pv / 2);
	if (have_flag(flgs, TR_STEALTH)) add_junko_flag_val(JKF1_S_STEALTH, flag_val * pv / 5);
	if (have_flag(flgs, TR_SEARCH)) add_junko_flag_val(JKF1_DETECT_OBJ, flag_val * pv / 2);
	if (have_flag(flgs, TR_BLOWS)) add_junko_flag_val(JKF1_MULTI_HIT, flag_val * pv / 2);

	if (have_flag(flgs, TR_DISARM)) add_junko_flag_val(JKF1_DISARM, flag_val * pv / 2);
	if (have_flag(flgs, TR_SAVING)) add_junko_flag_val(JKF1_RES_MAGIC, flag_val * pv / 2);


	if (have_flag(flgs, TR_RES_ACID) && have_flag(flgs, TR_RES_ELEC) && have_flag(flgs, TR_RES_FIRE) && have_flag(flgs, TR_RES_COLD))
		add_junko_flag_val(JKF1_RES_ELEM, flag_val);

	if (have_flag(flgs, TR_IM_ACID)) add_junko_flag_val(JKF2_ACID, 5000);
	if (have_flag(flgs, TR_IM_FIRE)) add_junko_flag_val(JKF2_FIRE, 5000);
	if (have_flag(flgs, TR_IM_ELEC)) add_junko_flag_val(JKF2_ELEC, 5000);
	if (have_flag(flgs, TR_IM_COLD)) add_junko_flag_val(JKF2_COLD, 5000);

	if (have_flag(flgs, TR_RES_ACID)) add_junko_flag_val(JKF2_ACID, flag_val);
	if (have_flag(flgs, TR_BRAND_ACID)) add_junko_flag_val(JKF2_ACID, flag_val);
	if (have_flag(flgs, TR_RES_ELEC)) add_junko_flag_val(JKF2_ELEC, flag_val);
	if (have_flag(flgs, TR_BRAND_ELEC)) add_junko_flag_val(JKF2_ELEC, flag_val);
	if (have_flag(flgs, TR_RES_FIRE)) add_junko_flag_val(JKF2_FIRE, flag_val);
	if (have_flag(flgs, TR_BRAND_FIRE)) add_junko_flag_val(JKF2_FIRE, flag_val);
	if (have_flag(flgs, TR_RES_COLD)) add_junko_flag_val(JKF2_COLD, flag_val);
	if (have_flag(flgs, TR_BRAND_COLD)) add_junko_flag_val(JKF2_COLD, flag_val);
	if (have_flag(flgs, TR_RES_POIS)) add_junko_flag_val(JKF2_POIS, flag_val);
	if (have_flag(flgs, TR_BRAND_POIS)) add_junko_flag_val(JKF2_POIS, flag_val);

	if (have_flag(flgs, TR_RES_LITE)) add_junko_flag_val(JKF2_LITE, flag_val);
	if (have_flag(flgs, TR_RES_DARK)) add_junko_flag_val(JKF2_DARK, flag_val);
	if (have_flag(flgs, TR_RES_SHARDS)) add_junko_flag_val(JKF2_SHARDS, flag_val);
	if (have_flag(flgs, TR_RES_WATER)) add_junko_flag_val(JKF2_WATER, flag_val);
	if (have_flag(flgs, TR_RES_SOUND)) add_junko_flag_val(JKF2_SOUND, flag_val);
	if (have_flag(flgs, TR_XTRA_MIGHT)) add_junko_flag_val(JKF2_NUKE, flag_val);
	if (have_flag(flgs, TR_XTRA_SHOTS)) add_junko_flag_val(JKF2_NUKE, flag_val);

	if (have_flag(flgs, TR_FORCE_WEAPON)) add_junko_flag_val(JKF2_MANA, flag_val);

	if (have_flag(flgs, TR_IMPACT)) add_junko_flag_val(JKF2_METEOR, flag_val);

	if (have_flag(flgs, TR_RES_DISEN)) add_junko_flag_val(JKF2_DISENCHANT, flag_val);
	if (have_flag(flgs, TR_RES_TIME)) add_junko_flag_val(JKF2_TIME, flag_val);
	if (have_flag(flgs, TR_SPEEDSTER)) add_junko_flag_val(JKF2_DISTORTION, flag_val);

	if (have_flag(flgs, TR_RES_HOLY)) add_junko_flag_val(JKF2_HOLY, flag_val);
	if (have_flag(flgs, TR_SLAY_EVIL)) add_junko_flag_val(JKF2_HOLY, flag_val);
	if (have_flag(flgs, TR_KILL_EVIL)) add_junko_flag_val(JKF2_HOLY, flag_val);

	if (have_flag(flgs, TR_SLAY_GOOD)) add_junko_flag_val(JKF2_HELL, flag_val);
	if (have_flag(flgs, TR_KILL_GOOD)) add_junko_flag_val(JKF2_HELL, flag_val);

	if (have_flag(flgs, TR_VORPAL)) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
	if (have_flag(flgs, TR_EX_VORPAL)) add_junko_flag_val(JKF2_DISINTEGRATE, flag_val);
	if (have_flag(flgs, TR_LEVITATION)) add_junko_flag_val(JKF2_WIND, flag_val);

	//v1.1.36�ǉ�
	if (have_flag(flgs, TR_RES_CHAOS)) add_junko_flag_val(JKF2_CHAOS, flag_val);
	if (have_flag(flgs, TR_RES_NETHER)) add_junko_flag_val(JKF2_NETHER, flag_val);

}


/************���̉��u�����Ȃ��Z�v�֘A*********************************/

//���ς̋Z���������񎟌����X�g�̗v�f��
#define JUNKO_NAME_LIST_NUM1	81
#define JUNKO_NAME_LIST_NUM_SUB	10

//���ϋZ���������X�g1 �����̓Y�������Z��{���e�[�u������Q�Ƃ����
cptr	junko_arts_name_words_1[JUNKO_NAME_LIST_NUM1][JUNKO_NAME_LIST_NUM_SUB] = 
{
	{"�b�M","�b�M","�b�M","�b�M","�b�M","�b�M","�b�M","�b�M","�b�M","�b�M",},//�_�~�[���b�M
//1
	{"�G","�G","�l��","�l��","�W�I","�W�I","�l��","�l��","����","����"},//�U���̕W�I�Ȃ�1
	{"���l","����","�����n","������������","����U�錢","�����","�_��","�����̎�","�ւ��ɂ܂ꂽ�^","���H����"},//�U���̕W�I�Ȃ�2
	{"��","��ؒ�","�r�Ƃ̋�","�S�㒹","��̑O�̐�","���ł̌�","�΋T","�C�K","�߂�ʒK","�l��"},//�U���̕W�I�Ȃ�3
	{"�܂̑l","�e","�m�n","�Ē�","�S�̉�","���q�̌�","�s�H�̐l","�j���̗r","�����̋�","�׏�̌�"},//�U���̕W�I�Ȃ�4
	{"�Ă̒�","�G���̏O","�ʁX�̖I","�J����","�w偂̎q","�L�ۖ���","����","�Q�r","�e��","�{�Q"},//�U���̕W�I�Ȃ�5 ���͂⎋�E�U���p
//6
	{"�S��","�S��","�S��","���E","���E","�O�琢�E����","��n","�V�n","�V��V��","�Z���O��"},//�U���̕W�I�Ȃǁ@�t���A�U��
	{"","","���߂�","���߂�","���߂�","������","������","������","���߂�����","���߂�����"},//�u���߂́v�Ȃ�
	{"","������","������","�P����","�P����","���Ղ�","�����Ȃ�","������","������","����̂Ȃ�"},//�u�����ȁv�Ȃ�
	{"��a�炰��","��a�炰��","��h��","�ɑ΍R����","�֑΍R����","���ɘa����","���ɘa����","�ɒ�R����","�ɍR��","�ɍR��"},//�h��n(��)
	{"��e��","��e��","����������","����������","�𖳗͉�����","�𖳗͉�����","�������Ȃ��Ȃ�","��ł�����","��ł�����","�������Ȃ��Ȃ�"},//�h��n(��)
//11
	{"�e","�e","�e","�e","��","��","��","���e","���e","���e"},//�U���F�{���g
	{"�e��","�e��","�e��","�e��","����","����","����","��","��","��"},//�U���F�{�[���A���P�b�g
	{"��","��","��","�M��","�M��","�M��","���߂�","���߂�","����","����"},//�U���F�r�[��
	{"��","��","�Q","�Q","�g��","�g��","��","��","�e��","�e��"},//�U���F���́A���E��
	{"���t","���t","����","�Q��","�����₫","�����₫","�Q��","�Q��","����","����"},//�u���X
//16	
	{"��","��","��","���\","��","��","�ӎu","�ӎu","�肢","�肢"},//�U���F�t���A
	{"�䂪�g","�䂪�g","����","����","���̐g","���̐g","�����[","�����[","���g","���g"},//�����p���Z
	{"�Ɋ��͂������炷","�Ɋ��͂������炷","�Ɋ��͂������炷","�̔�����苎��","�̔�����苎��","�̔���@������","�̔���@������","�ɗ͂���荞��","���Ăѓ�����","���Ăѓ�����"},//�󕠏[��
	{"��","��","����","��p","��","�藧��","���@","��i","���r","����"},//�⏕���Z�Ȃ�
	{"�̏������","�̏������","�̏������","�̏����ǂ�","�̏����ǂ�","�̌����","�̌����","������","������","�����Â���"},//��
//21
	{"��������","��������","�������o��","�������o��","�����o��","�����Ԃ�o��","���Ƃ炵�o��","���������","�����Ԃ��o��","�����Ԃ��o��"},//���m
	{"�","�","�","�d�|��","�d�|��","�G�̍�","�G�̍�","����","����","����"},//�g���b�v
	{"����","����","���i","���i","����","����","�ߋ�","�ߋ�","�Ε�","����"},//�A�C�e��
	{"���E","���E","�V��","�X������","�V�n","�痢�̖�","�R�C","��","����","�O�E"},//�t���A�Ώ�(�U���ȊO)
	{"�ɖւ��[��","�ɖւ��[��","�ɖւ��[��","�ɖւ��[��","�Ɍ��������炷","�Ɍ��������炷","�Ɍ��������炷","�����܂˂����ʂ�","�����܂˂����ʂ�","�����܂˂����ʂ�"},//�[��
//26
	{"���q���@������","���q���@������","���q���@������","�ɗ͂����߂�","�ɗ͂����߂�","�ɗ͂����߂�","�̞g����������","�̞g����������","����Ԃɕ�����","�̋@�\���C������"},//�\�́A�o���l����
	{"�ޕ��ւƒ��􂷂�","�ޕ��ւƒ��􂷂�","�u���Ƌ��Ɉړ�����","�u���Ƌ��Ɉړ�����","��Ԃ�c�߂�","��Ԃ�c�߂�","�������������","�������������","���̏�𗣒E����","���̏�𗣒E����"},//�e���|
	{"���u���Ԃɔ��","���u���Ԃɔ��","���ꑫ�ɔ��","���ꑫ�ɔ��","���ꑧ�ɋ삯��","���ꑧ�ɋ삯��","�ɂ��܂˂���������","�ւƔ����J��","�ւ̖���J������","�֓����q��"},//�����̔�
	{"���a�蕥��","��ǂ��U�炷","���R�U�炷","���R�U�炷","�𕏂Ŏa��ɂ���","�𕏂Ŏa��ɂ���","�𓥂݂�����","�𓥂݂ɂ���","��؂�J��","���������Ă͓�����"},//���͍U��
	{"�֎a�肩����","�Ɏa�肩����","�֔�ъ|����","�ɒ��ъ|����","��ǂ�������","��ǂ�������","�֒ǂ�������","�Ƃ̊ԍ������l�߂�","�Ƃ̋���������","�֕s�ӑł���������"},//���g
//31
	{"���Ȃ܂��ɂ���","��@���̂߂�","��ő��ł��ɂ���","��ő��ł��ɂ���","��������","��������","�������Ԃ葱����","�������Ԃ葱����","�������Ԃ葱����","�������Ԃ葱����"},//�A���U��
	{"�����","�����","�����","�̕a���𕥂�","�̕a���𕥂�","�ُ̈������","�ُ̈������","�̔M���܂�","�̔M���܂�","�ɓZ�������o������"},//��Ԉُ펡��
	{"����������","����������","�̈��ʂ��C������","�̈��ʂ��C������","�𕜋�����","�𕜋�����","���Đ�����","���Đ�����","���č\������","���č\������"},//��Ԉُ펡���{�񕜁{�ψَ���
	{"��","��","��","��","��","��","�v��","�v��","���q","���q"},//�����ق�
	{"��������炷","��������炷","��������炷","�G�e��e��","�G�e��e��","�G�e��e��","�e���������","�e���������","���[�U�[����������","�r�[���𖳗͉�����"},//�r�[���E�{���g����
//36
	{"����","����","�h��","�h��","�΍�","���v","�[�u","���","����","�ی�"},//�����i�h��p�j
	{"�򗈕�","�򗈕�","����","����","����","����","�\��","��","�{��","�{��"},//�{�[���A���P�b�g
	{"���ˍU��","���ˍU��","����","����","�u���X","�u���X","�u���X","�u���X","���ˍU��","���ˍU��"},//�u���X
	{"�j��̗�","�j��̗�","�j��̌�","�j��̌�","�łт̌�","�łт̌�","�j��̌��t","�j��̌��t","*�j��*","*�j��*"},//*�j��*�j�~
	{"����","����","����","���@","���@","���","���","�p�@","����","�p"},//���@�Ȃ�
//41
	{"���z������","���z������","���z������","��H��","��H��","���䂪�͂Ƃ���","��ϊ�����","���z�����","��]������","��]�p����"},//�z��
	{"������","������","���Ȃ�����ʂ�","���Ȃ�����ʂ�","�����@��","�����@��","�ǂ���","�ǂ���","����ӂ�","����ӂ�"},//�ǌ@��ړ�
	{"�ߋ�","�ߋ�","���","���","����","����","����","��","��","��"},//������
	{"�̔�߂���͂��J������","�̔�߂���͂��J������","�̔�߂���͂��J������","�̗͂���������","�̗͂���������","�����Ɏg��","�����Ɏg��","�����Ɏg��","�̌��ʂ𑝂�","�̌��ʂ𑝂�"},//������͎g�p
	{"�����x�ɂ���","�����x�ɂ���","�ɍ��͂������炷","�ɍ��͂������炷","�����s���炵�߂�","�����s���炵�߂�","�����N�ɂ���","�����N�ɂ���","�ɕ��E�̗_��������炷","�ɕ��E�̗_��������炷"},//���̋���
//46
	{"���_","���_","���_","�S","�S","�S","�m��","�m��","�C��","�C��"},//���_
	{"���������܂���","���������܂���","���������܂���","�����߂�","�����߂�","�����߂�","����������","����������","�̓����������ɂ���","�̓����������ɂ���"},//���_����
	{"�S��","�S��","�S��","�S��","������U��","������U��","�Q��","�Q��","����","����"},//���G��
	{"�̗��q������","�̗��q������","�̗��q������","����������","����������","����������","�̓����𑬂߂�","�̓����𑬂߂�","�̓����𑬂߂�","�̓����𑬂߂�"},//����
	{"�O��","�O��","�O��","�r��","�r��","�T�S","�\��","�\��","����","����"},//�����h��㏸
//51
	{"����g���B��","����g���B��","����g���B��","�̉e�ɉB���","�̉e�ɉB���","�̉e�ɉB���","�̈ӎ����������","�̈ӎ����������","�̎��p�ɓ���","�̎��p�ɓ���"},//���B��
	{"��","��","��","��","��","��n","��n","���","���","�y��"},//�ǔ���1
	{"�𔲂���","�𔲂���","�����蔲����","�����蔲����","��ʂ蔲����","�Ɏp���B��","�Ɠ�������","�Ɠ�������","�ɐg���B��","�ɐg���B��"},//�ǔ���2
	{"�����Ύa��","�����Ύa��","�����Ύa��","�����Ύa��","��⿂��ɂ�����","�𙒒肷��","�𙒒肷��","���j��������","���Ԉ���","�����~���ɂ���"},//�j�ЃI�[��
	{"���f","���f","���f","���f�̗�","���f�̗�","�����","�����","�����","�n���Ε�","�܍s"},//�G�������g�ϐ�
//56
	{"�ɗ͂�t�^����","�ɗ͂�t�^����","�ɗ͂�t�^����","�ɗ͂�t�^����","�ɗ͂�t�^����","�����H����","�����H����","�����H����","�Ɉꖡ������","�Ɉꖡ������"},//�X���C�A�ϐ��t�^
	{"�̖{����I��ɂ���","���悭����","��ǂ�����","������߂�","������߂�","������߂�","��i��߂���","��i��߂���","��ڗ�������","��ڗ�������"},//�Ӓ�
	{"�𒾖قɕ���","�𒾖قɕ���","���琺��D��","���琺��D��","�ɐÎ�������炷","�ɐÎ�������炷","��ق点��","��ق点��","��Â��ɂ���","��Â��ɂ���"},//�Î�
	{"����ł�����","����ł�����","����ł�����","����ł�����","����ł�����","����ł�����","����ł�����","����ł�����","����ł�����","����ł�����"},//���׏���
	{"��ǂ�����","��ǂ�����","��ǂ�����","����������","����������","����������","�ɏ����Ă��炤","�ɏ����Ă��炤","�����E�������","�����E�������"},//���E���A�E�F�C�A���Ӗ��E
//61
	{"���ɂݎE��","���ɂݎE��","���ɂߎE��","���ɂߎE��","�̊̂��Ԃ�","�̊̂��Ԃ�","���˔���","���˔���","���h��","���h��"},//���̌���
	{"��","��","��","����","����","����","����","����","����","����"},//���̌���2
	{"��","��","�C","�C","��","��","���","���","��","��"},//�C��������1
	{"�🿂炷","�🿂炷","������","������","������","�����オ�点��","�����オ�点��","�����オ�点��","�̐��������グ��","�̐��������グ��"},//�C��������2
	{"����␂߂�","����␂߂�","����␂߂�","����␂߂�","��␂܂���","��␂܂���","��␂܂���","���Ăю~�߂�","���Ăю~�߂�","���Ăю~�߂�"},//������
//66
	{"�����ꂴ��q","�����ꂴ��q","�ז���","�ז���","�V���ȓG","�V���ȓG","�G�e","�G�e","�ז����Ă����","�ז����Ă����"},//�����j�Q
	{"�����ł͂Ȃ��������֍s��","�i��������o�߂�","���E��h��ւ���","�ʂ̐��E�𖲌���","�o�߂Ȃ���������","�����ł͂Ȃ��������֍s��","�i��������o�߂�","���E��h��ւ���","�ʂ̐��E�𖲌���","�o�߂Ȃ���������"},//�����ϗe
	{"���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�","���𗎂Ƃ�"},//�����Q
	{"�܂�Ȃ�","���B��","���C��","������","�{���I��","�{���I��","���n��","�ŏ���","�����Ȃ�","��������"},//����2
	{"����","����","�����","��̓�","��","����","�C��","����","���","�d�|��"},//�⏕���Z�Ȃ�2
//71
	{"���������","���������","���������","�������Z��","�������Z��","�������Z��","���Èł��Ƃ炷","���Èł��Ƃ炷","���Èł��Ƃ炷","���Èł��Ƃ炷"},
	{"�𖜕v�s���̖Ҏ҂Ƃ���","�����|�S�ʂɒʂ���","�����|�S�ʂɒʂ���","���B�l�̎���𓾂�","�ɒB�l�̎���������炷","�𖳑o�̋��n�Ɏ��炵�߂�","�𖜕v�s���̖Ҏ҂ɂ���","�����|�S�ʂɒʂ���","�ɒB�l�̎���������炷","�𖳑o�̋��n�Ɏ��炵�߂�"},
	{"��n��","��n��","��n��","��n��","�n��","�n��","�n��","�n�Ղ�","�n�Ղ�","�n�Ղ�"},
	{"�h�炷","�h�炷","�h�炷","�h�邪��","�h�邪��","�h�邪��","�h���Ԃ�","�h���Ԃ�","����","����"},
	{"�K�^�������񂹂�","�K�^�������񂹂�","�K�^��͂ݎ��","�^����P���Ȃ���","�^����P���Ȃ���","���^�������炷","�^�C�����߂�","�^�C�����߂�","�΂̖ڂ𑀂�","�΂̖ڂ𑀂�"},
//76
	{ "���̒n","���̒n","���̒n","���A","���A","���A","���A","���A","���J�̒n","���J�̒n", }, //�L�͈͂̒n�`
	{ "�����n��","�����n��","�����ʂ�","�����ʂ�","����������","����������","�𒲂׏グ��","�𒲂׏グ��","����������","����������" }, //�S���m�A���ӊ��m
	{ "��f�킷","��f�킷","�����Ɋ���","�����Ɋ���","�̊��܂点��","�̊��܂点��","�𓦂��f�킹��","�𓦂��f�킹��","�����͘T��������","�����͘T��������" }, //���f
	{ "�ɞg��������","�ɞg��������","�ɞg��������","�̉��݂点��","�̉��݂点��","�̉��݂点��","�𖳗͉�����","�𖳗͉�����","���Ȃ߂�","���Ȃ߂�" }, //�\�͒ቺ
	{ "�̋��ς𔍂����","�̋��ς𔍂����","�̋��ς𔍂����","�̖{�����N��","�̖{�����N��","�̖{�����N��","�̐��̂�\��","�̐��̂�\��","�̗͂�D��","�̗͂�D��" }, //���͏���


//	{"","","","","","","","","",""},//

};

//���ϋZ���������X�g2�@����(JKF2_*�̐�)�ɑΉ��@�U���p
cptr	junko_arts_name_words_2_atk[JKF2_END - JKF2_START + 1][JUNKO_NAME_LIST_NUM_SUB] = 
{
	{"��n����","��n����","��n����","��n����","��ࣂꂳ����","��ࣂꂳ����","��Z�I����","��Z�I����","����������","����������"},//�_
	{"��Ⴢꂳ����","��Ⴢꂳ����","��Ⴢꂳ����","��Ⴢꂳ����","��Ⴢꂳ����","�����d������","�����d������","�����d������","�����d������","�����d������",},//�d��
	{"��R�₷","��R�₷","��R�₷","���Ă�","���Ă�","���Ă�","���ł���","���ł���","���ł���","���ċp����",},//�Ή��A�n�␶��
	{"���₷","���₷","���₷","�𓀂点��","�𓀂点��","�𓀂点��","�𓀂�������","�𓀂�������","��Ⓚ����","��Ⓚ����"},//��C
	{"���ꂵ�߂�","���ꂵ�߂�","���ꂵ�߂�","��a�ɖ`��","��a�ɖ`��","�Ɉ�����������","�Ɉ�����������","�������Ԃ�","�������Ԃ�","�������Ԃ�"},//��
	{"�̖ڂ�ׂ�","�̖ڂ�ׂ�","�̖ڂ�ׂ�","�����P�ɝf��","�����P�ɝf��","�����ɕ��","�����ɕ��","�����ɕ��","���Ă�","�����M����",},//�M��
	{"���łɕ��","���łɕ��","���łɕ��","�̖ڂ𕢂�","�̖ڂ𕢂�","�̖ڂ𕢂�","��h��ׂ�","��h��ׂ�","���Èłŕ���","���Èłŕ���"},//�Í�
	{"��؂��","��؂��","��؂��","��؂��","��������","��������","�������炯�ɂ���","���A���z�ɂ���","���A���z�ɂ���","���A���z�ɂ���"},//�j��
	{"��M�ꂳ����","��M�ꂳ����","�𐅐ӂ߂ɂ���","�𐅐ӂ߂ɂ���","�𐅐ӂ߂ɂ���","�𒾂߂�","�𒾂߂�","�𒾂߂�","�����̉a�ɂ���","�����̉a�ɂ���",},//���A���n�`����
	{"�։̂�������","�։̂�������","�̎���ׂ�","�̎���ׂ�","��ł��̂߂�","��ł��̂߂�","�ɌĂт�����","�ɌĂт�����","�ɌĂт�����","�ɌĂ΂��",},//����
	{"���Ă�����","���Ă�����","���Ă�����","���j�̉��ŕ��","���j�̉��ŕ��","���j�̉��ŕ��","�������Y�ɂ���","�������Y�ɂ���","���Ă��s����","���Ă��s����"},//�j�M
	{"��������","��������","��������","��łڂ�","��łڂ�","��łڂ�","��j�󂷂�","��j�󂷂�","��j�󂷂�","����������"},//����
	{"�������ׂ�","�������ׂ�","�������ׂ�","�������ׂ�","��ł���","��ł���","��ł���","��ł���","��ł���","��ł���"},//覐�
	{"�������~����","�������~����","�ɍ��ׂ������炷","�ɍ��ׂ������炷","�ɍ��ׂ������炷","��ϗe������","��ϗe������","��ϗe������","��ψق�����","��ψق�����"},//�J�I�X
	{"���E��","���E��","���E��","���E��","���E��","���E��","���q��������炷","���q��������炷","���q��������炷","���q��������炷",},//�n��
	{"�̗͂��킮","�̗͂��킮","�̗͂��킮","�̗͂��킮","���Ⴆ�Ȃ�����","���Ⴆ�Ȃ�����","����͂�D��","����͂�D��","����͂�D��","����͂�D��",},//��
	{"����Ԃ点��","����Ԃ点��","�̈��ʂ���","�̈��ʂ���","�̈��ʂ���","�̈��ʂ���","�����̍��ɖ߂�","�����̍��ɖ߂�","�����̍��ɖ߂�","�����̍��ɖ߂�"},//���ԋt�]
	{"��c�߂�","��c�߂�","��c�߂�","��c�߂�","��c�߂�","��P���Ȃ���","��P���Ȃ���","��P���Ȃ���","��P���Ȃ���","��P���Ȃ���"},//��Ԙc��
	{"���򉻂���","���򉻂���","���򉻂���","���򉻂���","���򉻂���","���򉻂���","���򉻂���","���򉻂���","���򉻂���","���򉻂���",},//�j��
	{"��n���ɗ��Ƃ�","��n���ɗ��Ƃ�","��n���ɗ��Ƃ�","��n���ɗ��Ƃ�","��n���̊��ɕ��荞��","�ɒn���̔����J��","�ɒn���̔����J��","��n���̘F�ɂ��ׂ�","��n���̘F�ɂ��ׂ�","��n���̘F�ɂ��ׂ�",},//�n���̍���
	{"������","������","������","������","����������","����������","����������","�����ł�����","�����ł�����","�����ł�����",},//����
	{"�𐁂���΂�","�𐁂���΂�","�𐁂���΂�","�𐁂���΂�","�𐁂��U�炷","�𐁂��U�炷","�𐁂��U�炷","�𒈂ɕ��킹��","�𒈂ɕ��킹��","�𒈂ɕ��킹��",},//����
	{"������","������","������","������","������","�������Ă��ɂ���","�������Ă��ɂ���","�������Ă��ɂ���","�������Ă��ɂ���","�������Ă��ɂ���",},//���C
	{"�𖄂߂�","�𖄂߂�","�𖄂߂�","������߂�","������߂�","��߂܂���","��߂܂���","�𐶂����߂ɂ���","�𐶂����߂ɂ���","�𐶂����߂ɂ���",},//��ΐ���
	{"���h�A�ŋ���","���h�A�ŋ���","���h�A�ŋ���","���h�A�ŋ���","���h�A�ŋ���","��ǂ��l�߂�","��ǂ��l�߂�","��ǂ��l�߂�","������߂�","������߂�",},//�h�A����
	{"��Ή�����","��Ή�����","��Ή�����","����ň͂�","����ň͂�","�𔛂�t����","�𔛂�t����","�ɉԂ��炩����","�ɉԂ��炩����","�ɉԂ��炩����",},//�X����
};

/*:::���ϋZ�쐬�̂��߂̃t���O���Z����*/
void	add_junko_flag_val(int flag_num, int add_val)
{
	int i;
	
	int new_val;

	if(p_ptr->pclass != CLASS_JUNKO) return;

	if(flag_num < 0 || flag_num > 107)
	{
		msg_format("ERROR:add_junko_flag_val()�ɕs����num�l(%d)�����͂��ꂽ");
		return;
	}

	new_val = p_ptr->magic_num2[flag_num];

	//�l��������قǑ����ʂ����炵�Ă����B�����ƌ����ǂ��v���O���������肻���Ȃ��̂����B
	//780���炢�Ńt���O�l��255�ɂȂ�B
	for(i=new_val/50;i<6;i++)
	{
		int add = MIN(50,( add_val / (i+1)));

		new_val += add;
		add_val -= add * (i+1);
		if(add_val < i+2) break;
	}

	if(new_val > 255) new_val = 255;

	p_ptr->magic_num2[flag_num] = (byte)new_val;

}


/*:::���������Z�����p�����[�^�p�\����*/
//nameless_arts_generate_type�͓��Z�𐶐�����Ƃ��ɎQ�Ƃ����f�[�^�̌^�A
//������͓��Z�𔭓�����Ƃ��Ɏg���p�����[�^���ꎞ�Ǘ����邽�߂̌^�ł���
typedef struct nameless_arts_activate_param_type nameless_arts_activate_param_type;
struct nameless_arts_activate_param_type
{

	s32b	art_idx;	//nameless_arts_generate_table�̃C���f�b�N�X�l
	s32b	gf_type;	//GF_***�ɑΉ����鑮���l�@�Ƃ����GF�Ƃ͉��̗��Ȃ̂��낤�H�����ɒm��Ȃ��B
	s32b	cost;		//����MP
	s32b	rad;		//���a
	s32b	base;		//��{���ʒl
	s32b	dice;
	s32b	sides;
	s32b	counter_idx;	//tim_general[]�̂ǂ̃J�E���^���g�����̃C���f�b�N�X�l 0-4 �Ȃ��X�y���J�[�h�▣�{�ۂ̌��ʂ�tim_general[]���g��Ȃ�
	s32b	quark_idx;	//�Z�̖��O��ۑ����邽�߂�quark_str()�C���f�b�N�X
	s32b	xtra1;		//���̑��p�����[�^
	s32b	xtra2;

};

/*:::���ϗp�Z�����K�����X�g*/
//�Y����JKF1_***�ɑΉ�����
typedef struct nameless_arts_generate_type nameless_arts_generate_type;
struct nameless_arts_generate_type
{
	u16b	rarity;		//�����قǑI�肳��ɂ��� 1-100
	byte	min_lev; 	//�g�p�\���x��(weird_luck()��ʂ��10���x����܂ŏo��)
	s16b	cost;		//��b�R�X�g
	u16b	base; 		//��b�x�[�X�l(���x��1��z��)
	u16b	dice;		//��b�_�C�X��
	u16b	sides; 		//��b�_�C�X��
	u16b	lev_coef;	//50���x���ɂ����Ƃ��̌��ʏ㏸�W��(%) 0�ŕω��Ȃ�

	s16b	name_idx[5];//���O�C���f�b�N�X junko_arts_name_words_1[][]��1�߂̓Y���ɑ���

	bool	need_flag2;	//JKF2_**���Q�l�ɂ��đ����l��ݒ肷��K�v�����邩�ǂ���
	bool	attack;		//�U���p�̋Z�̂Ƃ�TRUE
	bool	use_counter;//tim_general[]�J�E���^���g�����ǂ���

	byte	arts_type; //v2.0.4 ���ʐ���p�@���Z�̍U���E�h��E���m�Ȃǂ̕��ޔԍ�

};

/*:::�u�����Ȃ��Z�v�����K�����X�g*/
//�Y����JKF1_***�ɑΉ�����
const nameless_arts_generate_type nameless_arts_generate_table[JKF1_MAX+1] =
{
	{255,0,0,0,0,0,0,{0,0,0,0,0},FALSE,FALSE,FALSE,0},//�n�[�_�~�[
//1
	{20, 1, 3,  8, 2,  6,200,{1,-1,7,8,11},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��1 �{���g 20-60dam
	{20,10, 8, 25, 3,  9,400,{1,-1,7,8,12},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��2 �{�[�� 50-250dam 
	{25,15, 8, 30, 6,  6,300,{1,-1,7,8,13},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��3 �r�[�� 50-200dam 
	{35,35,16, 40, 8,  8,300,{1,-1,7,8,12},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��4 ���P�b�g 80-350dam
	{40,25,30, 10, 5,  5,150,{1,-1,7,8,15},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��5 �u���X ����HP�̐����̈�
	{55,45,35, 70,10, 10,350,{1,-1,7,8,13},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��6 ��^���[�U�[ 100-450dam
	{30,20,10, 60, 3, 16,500,{1,-1,7,8,14},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��7 �������S�{�[�� �ő�80-500dam
	{70,40,25, 30, 4,  8,400,{1,-1,7,8,14},TRUE,TRUE,FALSE,JKF_TYPE_ATTACK },//�����U��8 ���E���U�� 70-300dam
	{80,57,80, 50, 1, 30,300,{6,-1,7,8,16},TRUE,TRUE,FALSE,JKF_TYPE_SPECIAL},//�����U��9 �t���A�U�� weird_luck�v�@300dam���炢
	{25, 1,10,  0, 0,  0,  0,{17,18,7,19,0},FALSE,FALSE,FALSE,JKF_TYPE_HEAL},//�󕠏[��
//11
	{25,10, 4, 20, 2,  6,300,{17,20,7,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_HEAL },//��(��)
	{45,35, 8,100, 0,  0,300,{17,20,7,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_HEAL},//��(��)
	{20, 1, 3, 20, 0,  0,200,{ 1,21,7,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_SENSE},//�����X�^�[���m
	{25, 5, 4, 20, 0,  0,200,{22,21,7,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_SENSE},//㩊��m
	{30,10, 5, 20, 0,  0,200,{23,21,7,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_SENSE},//�A�C�e�����m
	{60,45,60,  0, 0,  0,  0,{24,25,7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_SENSE},//�[��
	{35,25,30,  0, 0,  0,  0,{17,26,7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_HEAL},//�\�́A�o���l����
	{25, 5, 5, 50, 0,  0,200,{27, 7,19, 0, 0},FALSE,FALSE,FALSE,JKF_TYPE_TELEPO},//�e���|
	{80,45,15, 20, 0,  0,300,{24,28, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_TELEPO},//�����̔�
	{40,30,30,  0, 0,  0,  0,{ 5,29, 7,19, 0},FALSE,TRUE,FALSE,JKF_TYPE_MELEE},//�S���͍U��
//21
	{35,15, 6,  3, 0,  0,300,{ 1,30, 7,19, 0},FALSE,TRUE,FALSE,JKF_TYPE_MELEE },//���g
	{65,35,72,  0, 0,  0,  0,{ 1,31, 7, 8,19},FALSE,TRUE,FALSE,JKF_TYPE_MELEE },//�A���U�� xtra1�̍U���񐔕��U������
	{30,20,10,  0, 0,  0,  0,{17,32, 7,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_HEAL},//����
	{80,45,99,  0, 0,  0,  0,{17,33, 7, 8,34},FALSE,FALSE,FALSE,JKF_TYPE_HEAL},//���ʏC��
	{40,15,30,  0, 0,  0,  0,{23,26, 7,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_HEAL },//����
//26
	{60,20,20, 10, 1, 10,100,{35,7, 8,36, 0},FALSE,FALSE,TRUE,JKF_TYPE_DEFENSE },//�r�[���E�{���g������
	{70,40,25,  8, 1,  8,100,{37,9, 7, 8,36},FALSE,FALSE,TRUE,JKF_TYPE_DEFENSE },//�����ی�
	{70,30,25,  5, 1,  5,100,{38,9, 7, 8,36},FALSE,FALSE,TRUE,JKF_TYPE_DEFENSE },//�u���X�ی�
	{80,45,60,  4, 1,  4,  0,{39,9, 7, 8,36},FALSE,FALSE,TRUE,JKF_TYPE_DEFENSE },//*�j��*�h��
	{60,20,30, 15, 1, 15,100,{40,41,7, 8,36},FALSE,FALSE,TRUE,JKF_TYPE_SPECIAL },//���@�z��
//31
	{45,20,30, 20, 1, 20,  0,{42,7,19, 0, 0},FALSE,FALSE,TRUE,JKF_TYPE_OTHER },//�ǌ@��
	{80,40,40, 20, 0,  0,  0,{43,44,7,19, 0},FALSE,FALSE,TRUE,JKF_TYPE_OTHER },//������͎g�p
	{40,25,12,  7, 1,  7,200,{17,45, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_MELEE },//���̋��� �����ʂ�xtra1�ɋL�^
	{40,25,12,  7, 1,  7,200,{46,47, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_OTHER },//���_���� �����ʂ�xtra1�ɋL�^
	{90,45,80,  4, 1,  4,  0,{48,10, 7, 8,36},FALSE,FALSE,FALSE,JKF_TYPE_SPECIAL},//���G��
//36
	{30,30,15, 10, 1, 10,100,{17,49, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_OTHER },//����
	{40,10,20, 10, 1, 10,  0,{40, 9, 7, 8,36},FALSE,FALSE,FALSE,JKF_TYPE_DEFENSE },//���@�h��
	{30,15,25, 20, 1, 20,  0,{50, 9, 7, 8,36},FALSE,FALSE,FALSE,JKF_TYPE_DEFENSE},//AC+50
	{80,40,60, 10, 1, 10,  0,{48, 9, 7, 8,36},FALSE,FALSE,FALSE,JKF_TYPE_SPECIAL },//���@�̊Z
	{80,45,70, 25, 1, 25,  0,{ 1,51, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_MELEE },//���B��
//41
	{60,35,50, 20, 1, 20,  0,{52,53, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_SPECIAL },//�ǔ���
	{40,10,25, 20, 1, 20,  0,{ 5,54, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_MELEE },//�j�ЃI�[��
	{35,20,20, 15, 1, 15,100,{55, 9, 7, 8,36},FALSE,FALSE,FALSE,JKF_TYPE_DEFENSE },//�G�������g�ϐ�
	{80,50,75, 15, 1, 15,  0,{48, 9, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_DEFENSE },//���ɂ̑ϐ�
	{60,15,80,  0, 0,  0,  0,{23,56, 7, 8,19}, TRUE,FALSE,FALSE,0 },//����ւ̔\�͕t�^
//46
	{60,15,80,  0, 0,  0,  0,{23,56, 7, 8,19}, TRUE,FALSE,FALSE,0},//�����i�ւ̑ϐ��t�^
	{35,15,15,  0, 0,  0,100,{23,57, 0, 0, 0},FALSE,FALSE,FALSE,JKF_TYPE_SENSE },//�Ӓ�
	{70,25,25,  5, 1,  4,100,{ 8,39, 0, 0, 0},FALSE, TRUE,FALSE,JKF_TYPE_SPECIAL },//*�j��*
	{80,50,180, 0, 0,  0,  0,{24,58, 7, 8,16},FALSE,FALSE,FALSE,JKF_TYPE_DEBUFF},//�Î�
	{70,40,35,100, 0,  0,100,{ 5,60, 7, 8,16},FALSE, TRUE,FALSE,JKF_TYPE_TELEPO },//���Ӗ��E
//51
	{80,30,45,  0, 0,  0,  0,{ 5,59, 7, 8,19},FALSE, TRUE,FALSE,JKF_TYPE_ATTACK},//���׏���
	{40,25,20, 50, 1, 50,200,{ 5,60, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_TELEPO},//���E���A�E�F�C
	{75,40,35,  0, 0,  0,  0,{ 5,61,62, 0, 0},FALSE, TRUE,FALSE,JKF_TYPE_DEBUFF},//���̌���
	{80,30,40, 50, 1, 50,200,{63,64, 7, 8,14},FALSE, TRUE,FALSE,JKF_TYPE_ATTACK },//�C��������
	{30,20,10, 50, 1, 50,200,{ 5,65, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_DEBUFF },//������
//56
	{70,35,20, 10, 1, 10,100,{66,60, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_DEBUFF },//�����j�Q
	{55,35,50,  0, 0,  0,  0,{67, 7, 8,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_TELEPO },//�����ϗe
	{80,45,45, 50, 1, 50,100,{68, 7, 8,16, 0},FALSE, TRUE,FALSE,JKF_TYPE_ATTACK },//�����Q
	{30, 1, 3, 50, 1, 50,200,{17,71, 7, 8,19},FALSE,FALSE, TRUE,JKF_TYPE_OTHER },//����
	{80,20,25, 20, 0,  0,200,{17,72, 7, 8,19},FALSE,FALSE, TRUE,JKF_TYPE_MELEE },//�����\�͏㏸
//61
	{40,15,15,  3, 1,  3,100,{73,74, 7, 8,34},FALSE, TRUE,FALSE,JKF_TYPE_OTHER },//�n�k
	{30, 5,10,  5, 0,  0,100,{22,10, 7, 8,19},FALSE,FALSE,FALSE,JKF_TYPE_OTHER },//�g���b�v����
	{60,30,50, 30, 1, 30,  0,{75, 7, 8,19, 0},FALSE,FALSE,FALSE,JKF_TYPE_SPECIAL },//�K�^
//v2.0.4 64-67�ǉ�
	{30,25,10, 50, 1, 50,200,{ 1,79, 7, 8,19 },FALSE,FALSE,FALSE,JKF_TYPE_DEBUFF },//�\�͒ቺ
	{50, 5,20,  0, 0,  0,  0,{ 4,80, 7, 8,19 },FALSE,FALSE,FALSE,JKF_TYPE_DEBUFF },//���͏���
	{40,30,25, 50, 1, 50,200,{ 5,78, 7, 8,13 },FALSE,FALSE,FALSE,JKF_TYPE_DEBUFF },//���f
	{40,30,20, 15, 0,  0,100,{ 76,77,7, 8, 62 },FALSE,FALSE,FALSE,JKF_TYPE_SENSE },//�S���m+�t���A���m


//	{ 0, 0, 0,  0, 0,  0,  0,{ 0, 0,0, 0, 0},FALSE,FALSE,FALSE},//


};



//���ʐ���̂��߂̑f�ރe�[�u��
typedef struct magatama_material_type magatama_material_type;
struct magatama_material_type
{
	byte tval;
	byte sval;
	byte mlev;//�f�ރ��x���@���Z�I�莞�̃��x������Ɂ��̃��x���̑���ɂ��̑f�ރ��x�����g�p���� weird_luck()�ɒʂ�Ɓu��S�̏o���v�ɂȂ��ă��x��+10
	int power;//�����قǃp���[��З͂��オ�� ��1,���ʕ��30,��ʕ��50,�ɜQ������150���炢 ��S�̏o���̂Ƃ��������{�[�i�X
	int suitable_skill_type;//���̑f�ނ��g�����Ƃ��ɍ���₷���X�L������ JKF_TYPE_***�Ɉ�v
	int suitable_gf_type;//���̑f�ނ��g���čU�����@�����ꂽ�Ƃ��I�肳��₷������ JKF2_***�Ɉ�v
};


const magatama_material_type magatama_material_table[] =
{
	{ TV_MATERIAL,SV_MATERIAL_STONE			,0,5,JKF_TYPE_ATTACK,0 }, //�΁@���x��0 �U�� ��S�̏o���ɂȂ�Ȃ��ƍ쐬���s����
	{ TV_MATERIAL,SV_MATERIAL_MAGNETITE		,10,15,JKF_TYPE_ATTACK,0 }, //�}�O�l�^�C�g ���x��5 �U��
	{ TV_MATERIAL,SV_MATERIAL_GARNET		,10,20,JKF_TYPE_TELEPO,0 }, //�K�[�l�b�g lev10
	{ TV_MATERIAL,SV_MATERIAL_AMETHYST		,10,20,JKF_TYPE_SENSE,0 }, //�A���W�X�g lev10 
	{ TV_MATERIAL,SV_MATERIAL_AQUAMARINE	,10,20,JKF_TYPE_HEAL,0 }, //�A�N�A�}���� lev10 
	{ TV_MATERIAL,SV_MATERIAL_MOONSTONE		,10,20,JKF_TYPE_MELEE,0 }, //���[���X�g�[�� lev10 
	{ TV_MATERIAL,SV_MATERIAL_PERIDOT		,10,20,JKF_TYPE_OTHER,0 }, //�y���h�b�g lev10
	{ TV_MATERIAL,SV_MATERIAL_OPAL			,10,20,0,0 },				//�I�p�[�� lev10
	{ TV_MATERIAL,SV_MATERIAL_TOPAZ			,10,20,JKF_TYPE_DEBUFF,0 }, //�g�p�[�Y lev10
	{ TV_MATERIAL,SV_MATERIAL_LAPISLAZULI	,10,20,JKF_TYPE_DEFENSE,0 }, //���s�X���Y�� lev10
	{ TV_MATERIAL,SV_MATERIAL_HEMATITE		,15,25,JKF_TYPE_OTHER,0 },//���ۉ��O ���x��15 ���̑�
	{ TV_MATERIAL,SV_MATERIAL_ARSENIC		,20,30,JKF_TYPE_ATTACK,JKF2_POIS }, //��f�z�΁@���x��20 �ōU��
	{ TV_MATERIAL,SV_MATERIAL_SCRAP_IRON	,25,1 ,0,0 },				 //���S�@�F�X�o���邪�p���[�Ⴂ
	{ TV_MATERIAL,SV_MATERIAL_MERCURY		,25,30,JKF_TYPE_DEBUFF,0 }, //���� lev25 �W�Q
	{ TV_MATERIAL,SV_MATERIAL_MITHRIL		,25,30,JKF_TYPE_HEAL,JKF2_LITE },//�~�X���� lev25 ��
	{ TV_MATERIAL,SV_MATERIAL_BROKEN_NEEDLE	,25,30,JKF_TYPE_MELEE,JKF2_SHARDS },	 //�܂ꂽ�j�@lev25 ���e
	{ TV_MATERIAL,SV_MATERIAL_ISHIZAKURA	,25,35,JKF_TYPE_ATTACK,0 },		 //�΍��@lev25 �U��
	{ TV_MATERIAL,SV_MATERIAL_ADAMANTITE	,25,35,JKF_TYPE_DEFENSE,0 },//�A�_�}���^�C�g lev25 �h��
	{ TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT,30,40,JKF_TYPE_DEBUFF,0 },		 //�����̌��Ё@lev30 �W�Q�n
	{ TV_MATERIAL,SV_MATERIAL_DRAGONSCALE	,30,40,JKF_TYPE_DEFENSE,0 }, //���̗� lev30 �h��
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU		,35,70,JKF_TYPE_ATTACK,0 }, //���� lev35 �U�� �p���[����
	{ TV_MATERIAL,SV_MATERIAL_DRAGONNAIL	,40,60,JKF_TYPE_MELEE,0 }, //���̉� lev40 ����
	{ TV_MATERIAL,SV_MATERIAL_EMERALD		,40,55,JKF_TYPE_TELEPO,0 }, //�G�������h lev40 �e���|
	{ TV_MATERIAL,SV_MATERIAL_SAPPHIRE		,40,55,JKF_TYPE_SENSE,0 }, //�T�t�@�C�A lev40 ���m
	{ TV_MATERIAL,SV_MATERIAL_RUBY			,40,55,JKF_TYPE_OTHER,0 }, //���r�[ lev40 �ق�
	{ TV_MATERIAL,SV_MATERIAL_DIAMOND		,40,65,JKF_TYPE_DEFENSE,0 }, //�_�C�A�����h lev40 �h�� 
	{ TV_MATERIAL,SV_MATERIAL_HOPE_FRAGMENT	,40,60,JKF_TYPE_HEAL,JKF2_HOLY }, //��]�̌����@lev40
	{ TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT	,45,80,0,0 },			//�ɜQ������ lev45 ���ނȂ�
	{ TV_MATERIAL,SV_MATERIAL_METEORICIRON	,50,100,JKF_TYPE_ATTACK,JKF2_METEOR },	 //覓S�@lev50 覐΍U��
	{ TV_MATERIAL,SV_MATERIAL_MYSTERIUM		,50,150,0,0 },			 //�~�X�e���E���@lev50 ���ނȂ�
	{ TV_MATERIAL,SV_MATERIAL_HIHIIROKANE	,50,200,JKF_TYPE_SPECIAL,0 }, //�q�q�C���J�l lev50 ����

	{ TV_SOUVENIR,SV_SOUVENIR_ELDER_THINGS_CRYSTAL	,20,75,0,0 }, //�Â����̂̐���
	{ TV_SOUVENIR,SV_SOUVENIR_PRISM					,30,80,JKF_TYPE_SENSE,0 }, //�O�ŋ��@lev30 ���m
	{ TV_SOUVENIR,SV_SOUVENIR_KAPPA_5				,35,100,JKF_TYPE_SPECIAL,0 }, //�͓��̌ܐF�b�� lev35 ����
	{ TV_SOUVENIR,SV_SOUVENIR_ILMENITE				,40,120,JKF_TYPE_TELEPO,0 }, //���̃C�����i�C�g
	{ TV_SOUVENIR,SV_SOUVENIR_ASIA					,50,150,JKF_TYPE_ATTACK,JKF2_NUKE }, //�G�C�W���@lev50 �j�M
	{ TV_SOUVENIR,SV_SOUVENIR_MIRROR_OF_RUFFNOR		,50,150,JKF_TYPE_DEFENSE,0 }, //���t�m�[���̋��@lev50 �h��

	{ 0,0,0,0,0 },//�I�[�_�~�[ tval=0
};



//�u�����Ȃ��Z�v�̌��ʂ���ʂɕ\������B����ʂ̕ۑ���ĕ`�揈���͍s��Ȃ��B
void	display_nameless_art_effect(nameless_arts_activate_param_type *naap_ptr, int mode)
{
	int i;

	for (i = 15; i<23; i++) Term_erase(17, i, 255);

	prt(format("���Z�^�C�v:%s", nameless_skill_type_desc[naap_ptr->art_idx]), 16, 30);
	if (naap_ptr->gf_type) prt(format("����:%s", gf_desc_name[naap_ptr->gf_type]), 17, 30);
	prt(format("����MP:%d", naap_ptr->cost), 18, 30);
	if (naap_ptr->rad>0) prt(format("���a:%d", ABS(naap_ptr->rad)), 19, 30);

	//�u���X�n��xtra1��
	if (naap_ptr->art_idx == JKF1_ATTACK_BREATH)
	{
		prt(format("�З�/����:����HP��%d%%", naap_ptr->xtra1), 20, 30);
	}
	//2.0.4 �������S�{�[����/2�ɕ\�L
	else if (naap_ptr->art_idx == JKF1_ATTACK_AROUND)
	{
		if (naap_ptr->dice && naap_ptr->sides && naap_ptr->base)
			prt(format("�З�/����:�ő�(%dd%d+%d)/2", naap_ptr->dice, naap_ptr->sides, naap_ptr->base), 20, 30);
		else if (naap_ptr->dice && naap_ptr->sides)
			prt(format("�З�/����:�ő�(%dd%d)/2", naap_ptr->dice, naap_ptr->sides), 20, 30);
		else if (naap_ptr->base)
			prt(format("�З�/����:�ő�%d", naap_ptr->base/2), 20, 30);

	}
	else
	{
		if (naap_ptr->dice && naap_ptr->sides && naap_ptr->base)
			prt(format("�З�/����:%dd%d+%d", naap_ptr->dice, naap_ptr->sides, naap_ptr->base), 20, 30);
		else if (naap_ptr->dice && naap_ptr->sides)
			prt(format("�З�/����:%dd%d", naap_ptr->dice, naap_ptr->sides), 20, 30);
		else if (naap_ptr->base)
			prt(format("�З�/����:%d", naap_ptr->base), 20, 30);
	}




}


//�u�����Ȃ��Z�v�̃p�����[�^��p_ptr->magic1[]����擾����
bool	read_nameless_arts_param_from_player(int idx, nameless_arts_activate_param_type *naap_ptr)
{

	//p_ptr->magic_num1[]�𖼂��Ȃ��Z�Ɏg���N���X�݂̂��g��
	if (!USE_NAMELESS_ARTS)
	{
		msg_print("ERROR:read_nameless_arts_param_from_player()�����o�^�̃L�����N�^�[�ŌĂ΂ꂽ");
		return FALSE;
	}
	if (idx < 0 || idx >= JUNKO_ARTS_COUNT_MAX)
	{
		msg_format("ERROR:read_nameless_arts_param_from_player()�ɓn���ꂽidx����������",idx);
		return FALSE;
	}

	naap_ptr->art_idx = JUNKO_ARTS_PARAM(idx, JKAP_JKF1);
	naap_ptr->gf_type = JUNKO_ARTS_PARAM(idx, JKAP_GF);
	naap_ptr->cost = JUNKO_ARTS_PARAM(idx, JKAP_COST);
	naap_ptr->rad = JUNKO_ARTS_PARAM(idx, JKAP_RAD);
	naap_ptr->base = JUNKO_ARTS_PARAM(idx, JKAP_BASE);
	naap_ptr->dice = JUNKO_ARTS_PARAM(idx, JKAP_DICE);
	naap_ptr->sides = JUNKO_ARTS_PARAM(idx, JKAP_SIDES);
	naap_ptr->counter_idx = JUNKO_ARTS_PARAM(idx, JKAP_COUNT_NUM);
	naap_ptr->quark_idx = JUNKO_ARTS_PARAM(idx, JKAP_QUARK_IDX);
	naap_ptr->xtra1 = JUNKO_ARTS_PARAM(idx, JKAP_XTRA_VAL1);
	naap_ptr->xtra2 = JUNKO_ARTS_PARAM(idx, JKAP_XTRA_VAL2);

	return TRUE;

}




//���ς��X���C�Ȃǂ��G�b�Z���X�t�^�ł��镐��𔻒肷��hook
bool object_allow_add_essence_junko1(object_type *o_ptr)
{
	if (!object_is_melee_weapon(o_ptr)) return FALSE;
	if (object_refuse_enchant_weapon(o_ptr)) return FALSE;
	if (object_is_artifact(o_ptr)) return FALSE;
	if (object_is_smith(o_ptr)) return FALSE;

	return TRUE;
}

//���ς��ϐ��G�b�Z���X�t�^�ł��镐��h��𔻒肷��hook
bool object_allow_add_essence_junko2(object_type *o_ptr)
{
	if (object_is_artifact(o_ptr)) return FALSE;
	if (object_is_smith(o_ptr)) return FALSE;
	if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_TORCH) return FALSE;
	if (item_tester_hook_jeweler(o_ptr) || object_is_armour(o_ptr)) return TRUE;

	return FALSE;

}







/*:::
*�u���������Z�v�̃p�����[�^��n���A���ۂɌ��ʔ������镔���B
*���Xactivate_nameless_art()�Ɋ܂܂�Ă������A�ڐA���m�ۂ̂��߂ɕʊ֐��ɂ����B�ŏ��������Ƃ��Ƃ����b�ł���B
*�R�X�g�⎸�s��(�����)�̏����͊O�ōs���B
*�^�[�Q�b�g�I���ŃL�����Z������ȂǍs��������Ȃ��Ƃ�FALSE��Ԃ��B
*/
bool	activate_nameless_art_aux(nameless_arts_activate_param_type *naap_ptr, int mode)
{
	int dir, i;
	int art_idx, typ, rad, base, dice, sides, counter_idx, quark_idx, xtra1, xtra2;

	int dice_roll_result; //�_�C�X��U���ăx�[�X�𑫂����l�B�_���[�W����ʎ��ԂȂǁB���ʂɂ���Ă͕ʂ̊֐���base��dice��sides��n���̂ł��̒l���g��Ȃ�

	art_idx = naap_ptr->art_idx;
	typ = naap_ptr->gf_type;
	rad = naap_ptr->rad;
	base = naap_ptr->base;
	dice = naap_ptr->dice;
	sides = naap_ptr->sides;
	counter_idx = naap_ptr->counter_idx;
	xtra1 = naap_ptr->xtra1;
	xtra2 = naap_ptr->xtra2;

	dice_roll_result = ((dice>0 && sides>0) ? damroll(dice, sides) : 0) + base;

	switch (art_idx)
	{
		//�{���g
	case JKF1_ATTACK_BOLT:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_bolt(typ, dir, dice_roll_result);
		break;
	}
	//�{�[��
	case JKF1_ATTACK_BALL:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(typ, dir, dice_roll_result, rad);
		if (typ == GF_LAVA_FLOW)
			fire_ball_hide(GF_FIRE, dir, dice_roll_result, rad);
		if (typ == GF_WATER_FLOW)
			fire_ball_hide(GF_WATER, dir, dice_roll_result, rad);
		break;
	}
	//�r�[��
	case JKF1_ATTACK_BEAM:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_beam(typ, dir, dice_roll_result);
		break;
	}
	//���P�b�g
	case JKF1_ATTACK_ROCKET:
	{
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(typ, dir, dice_roll_result, rad);
		if (typ == GF_LAVA_FLOW)
			fire_ball(GF_FIRE, dir, dice_roll_result, rad);
		if (typ == GF_WATER_FLOW)
			fire_ball(GF_WATER, dir, dice_roll_result, rad);
		break;
	}
	//�u���X
	case JKF1_ATTACK_BREATH:
	{
		int dam;
		if (rad >= 0 || rad < -2)
		{
			msg_format("ERROR:���σu���X��rad�l����������(%d)", rad);
			return FALSE;
		}
		if (!get_aim_dir(&dir)) return FALSE;
		dam = p_ptr->chp * xtra1 / 100; //�u���X�̈З͂�xtra1�l�ŕω�
		if (dam < 1) dam = 1;
		fire_ball(typ, dir, dam, rad);
		if (typ == GF_LAVA_FLOW)
			fire_ball(GF_FIRE, dir, dam, rad);
		if (typ == GF_WATER_FLOW)
			fire_ball(GF_WATER, dir, dam, rad);
		break;
	}
	//��^���[�U�[
	case JKF1_ATTACK_SPARK:
	{
		if (rad>2 || rad < 1)
		{
			msg_format("ERROR:���σX�p�[�N��rad�l����������(%d)", rad);
			return FALSE;
		}
		if (!get_aim_dir(&dir)) return FALSE;
		fire_spark(typ, dir, dice_roll_result, rad);
		if (typ == GF_LAVA_FLOW)
			fire_spark(GF_FIRE, dir, dice_roll_result, rad);
		if (typ == GF_WATER_FLOW)
			fire_spark(GF_WATER, dir, dice_roll_result, rad);
		break;
	}
	//���̓{�[��
	case JKF1_ATTACK_AROUND:
	{
		project(0, rad, py, px, dice_roll_result, typ, (PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID), -1);
		if (typ == GF_LAVA_FLOW)
			project(0, rad, py, px, dice_roll_result, GF_FIRE, (PROJECT_HIDE | PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID), -1);
		if (typ == GF_WATER_FLOW)
			project(0, rad, py, px, dice_roll_result, GF_WATER, (PROJECT_HIDE | PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID), -1);
		break;
	}
	//���E���U��
	case JKF1_ATTACK_SIGHT:
	{
		project_hack2(typ, dice, sides, base);
		if (typ == GF_LAVA_FLOW)
			project_hack2(GF_FIRE, dice, sides, base);
		if (typ == GF_WATER_FLOW)
			project_hack2(GF_WATER, dice, sides, base);
		break;
	}
	//�t���A�U��
	case JKF1_ATTACK_FLOOR:
	{
		floor_attack(typ, dice, sides, base, 0);

		break;
	}
	//�󕠏[��
	case JKF1_FOOD:
	{
		set_food(PY_FOOD_MAX - 1);
		break;
	}
	//��
	case JKF1_HEAL1:
	case JKF1_HEAL2:
	{
		hp_player(dice_roll_result);
		set_cut(0);
		set_poisoned(0);

		break;
	}
	//���m�Ȃ�
	case JKF1_DETECT_MON:
	{
		detect_monsters_invis(base);
		detect_monsters_normal(base);
		break;
	}
	case JKF1_DETECT_TRAPS:
	{
		detect_traps(base, TRUE);
		break;
	}
	case JKF1_DETECT_OBJ:
	{
		detect_objects_normal(base);
		break;
	}
	case JKF1_ENLIGHT:
	{
		wiz_lite(FALSE);
		break;
	}
	//�o���l�A�p�����[�^����
	case JKF1_RESTORE:
	{
		bool flag_notice = FALSE;
		if (restore_level()) flag_notice = TRUE;
		for (i = 0; i<6; i++) if (do_res_stat(i)) flag_notice = TRUE;

		if (!flag_notice) msg_print("���Ȃ��̔\�͂␶���͂͑��Ȃ��Ă͂��Ȃ��B");

		break;
	}
	//�e���|
	case JKF1_TELEPO:
	{
		teleport_player(dice_roll_result, 0L);
		break;
	}
	//�����̔�
	case JKF1_D_DOOR:
	{
		dimension_door_distance = MAX(20, dice_roll_result);
		if (!dimension_door(D_DOOR_CHOSEN_DIST)) return FALSE;
		break;
	}
	//���͍U�� ACT_WHIRLWIND����R�s�[
	//v1.1.33 �ʊ֐��ɓƗ�
	case JKF1_WHIRL_WIND:
	{
		whirlwind_attack(0);
		break;
	}
	//���g
	case JKF1_RUSH_ATTACK:
	{
		int len = MAX(3, dice_roll_result);
		if (!rush_attack(NULL, len, 0)) return FALSE;
		break;
	}
	//�A���U��
	case JKF1_MULTI_HIT:
	{
		int mult = MAX(2, xtra1);
		monster_type    *m_ptr;
		int x, y;

		if (!get_rep_dir(&dir, FALSE)) return FALSE;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (cave[y][x].m_idx)
		{
			m_ptr = &m_list[cave[y][x].m_idx];
			for (i = 0; i<mult; i++)
			{
				py_attack(y, x, 0L);
				if (!m_ptr->r_idx) break;
				if (p_ptr->is_dead) break;

			}
		}
		else
		{
			msg_print("������ɂ͉������Ȃ��B");
			return FALSE;
		}


		break;
	}
	//��Ԉُ펡��
	case JKF1_CURE_ALL:
	{
		if (!p_ptr->stun && !p_ptr->cut && !p_ptr->poisoned && !p_ptr->image && !p_ptr->alcohol && !p_ptr->asthma && !p_ptr->afraid)
		{
			msg_print("���ɑ̒��͈����Ȃ��B");
			break;
		}
		set_stun(0);
		set_cut(0);
		set_poisoned(0);
		set_image(0);
		set_alcohol(0);
		set_asthma(0);
		set_afraid(0);

		break;
	}
	//���ʏC��
	case JKF1_RESTORE_NEXUS:
	{
		msg_print("���Ȃ��͎�����C�����n�߂�..");
		hp_player(5000);
		set_poisoned(0);
		set_stun(0);
		set_cut(0);
		set_image(0);
		do_res_stat(A_STR);
		do_res_stat(A_INT);
		do_res_stat(A_WIS);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);
		do_res_stat(A_CHR);
		set_alcohol(0);
		restore_level();
		dispel_player();
		set_asthma(0);

		if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
		{
			if (muta_erasable_count()) msg_print("�S�Ă̓ˑR�ψق��������B");
			p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
			p_ptr->muta1 = p_ptr->muta1_perma;
			p_ptr->muta2 = p_ptr->muta2_perma;
			p_ptr->muta3 = p_ptr->muta3_perma;
			p_ptr->muta4 = p_ptr->muta4_perma;
			p_ptr->update |= PU_BONUS;
			p_ptr->bydo = 0;
			handle_stuff();
			mutant_regenerate_mod = calc_mutant_regenerate_mod();
		}
		break;
	}
	//����
	case JKF1_REMOVE_CURSE:
	{
		if (remove_all_curse())
			msg_print("�����i�̎������������B");
		else
			msg_print("�g�ɂ������͎̂���Ă��Ȃ��B");

		break;
	}
	//����ꎞ���ʑS��
	case JKF1_VANISH_BEAM:
	case JKF1_EXPLOSION_DEF:
	case JKF1_BREATH_DEF:
	case JKF1_DESTRUCT_DEF:
	case JKF1_ABSORB_MAGIC:
	case JKF1_KILL_WALL:
	case JKF1_MASTER_MAGITEM:
	case JKF1_SELF_LIGHT:
	case JKF1_MASTER_MELEE:
	{

		if (mode == NAMELESS_ART_MODE_SPELLCARD || mode == NAMELESS_ART_MODE_MAGATAMA)
			set_tim_spellcard_effect(dice_roll_result, FALSE, art_idx);
		else
			set_tim_general(dice_roll_result, FALSE, counter_idx, art_idx);

		break;
	}

	case JKF1_MUSCLE_UP:
	{

		msg_print("�g�̔\�͂����������B");
		set_tim_addstat(A_STR, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_DEX, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_CON, xtra1, dice_roll_result, FALSE);

		break;
	}
	case JKF1_MENTAL_UP:
	{

		msg_print("���_�����������B");
		set_tim_addstat(A_INT, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_WIS, xtra1, dice_roll_result, FALSE);
		set_tim_addstat(A_CHR, xtra1, dice_roll_result, FALSE);

		break;
	}
	case JKF1_INVULNER:
	{
		set_invuln(dice_roll_result, FALSE);

		break;
	}
	case JKF1_SPEED:
	{
		set_fast(dice_roll_result, FALSE);

		break;
	}
	case JKF1_RES_MAGIC:
	{
		set_resist_magic(dice_roll_result, FALSE);

		break;
	}
	case JKF1_SHIELD:
	{
		set_shield(dice_roll_result, FALSE);

		break;
	}
	case JKF1_MAGICDEF:
	{
		set_magicdef(dice_roll_result, FALSE);

		break;
	}
	case JKF1_S_STEALTH:
	{
		set_tim_superstealth(dice_roll_result, FALSE, SUPERSTEALTH_TYPE_NORMAL);

		break;
	}
	case JKF1_PASS_WALL:
	{
		set_kabenuke(dice_roll_result, FALSE);

		break;
	}
	case JKF1_DUSTROBE:
	{

		msg_print("���Ȃ��͍U���I�ȃI�[�����܂Ƃ����I");
		set_dustrobe(dice_roll_result, FALSE);

		break;
	}
	case JKF1_RES_ELEM: //�t�^�����ꎞ�ϐ��̓r�b�g�t���O�Ō���ς�
	{
		if ((xtra1) % 2) set_oppose_acid(dice_roll_result, FALSE);
		if ((xtra1 >> 1) % 2) set_oppose_elec(dice_roll_result, FALSE);
		if ((xtra1 >> 2) % 2) set_oppose_fire(dice_roll_result, FALSE);
		if ((xtra1 >> 3) % 2) set_oppose_cold(dice_roll_result, FALSE);
		if ((xtra1 >> 4) % 2) set_oppose_pois(dice_roll_result, FALSE);
		break;
	}
	case JKF1_ULTI_RES:
	{
		set_oppose_acid(dice_roll_result, FALSE);
		set_oppose_elec(dice_roll_result, FALSE);
		set_oppose_fire(dice_roll_result, FALSE);
		set_oppose_cold(dice_roll_result, FALSE);
		set_oppose_pois(dice_roll_result, FALSE);
		set_ultimate_res(dice_roll_result, FALSE);

		break;
	}

	//�G�b�Z���X�t�^ xtra1�ɓ��ꂽTR_*�l��+1����xtra1�ɓ����
	case JKF1_ADD_ESSENCE1:
	case JKF1_ADD_ESSENCE2:
	{
		cptr q, s;
		int item;
		object_type *o_ptr;
		char o_name[MAX_NLEN];
		cptr essence_name = "";

		if (art_idx == JKF1_ADD_ESSENCE1)
			item_tester_hook = object_allow_add_essence_junko1;
		else
			item_tester_hook = object_allow_add_essence_junko2;

		item_tester_no_ryoute = TRUE;

		q = "�ǂ̃A�C�e���ɗ͂�t�^���܂����H";
		s = "���ǂł���A�C�e��������܂���B";

		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return FALSE;
		if (item >= 0) 	o_ptr = &inventory[item];
		else 		o_ptr = &o_list[0 - item];

		object_desc(o_name, o_ptr, OD_NAME_ONLY);

		//�\�͖���b��t�p�G�b�Z���X�z�񂩂�T��
		for (i = 0; essence_info[i].add >= 0; i++)
		{
			if (essence_info[i].add != xtra1) continue;
			essence_name = essence_info[i].add_name;
			break;
		}

		if (!get_check(format("%s�Ɂu%s�v�̔\�͂�t�^���܂��B��낵���ł����H", o_name, essence_name))) return FALSE;

		o_ptr->xtra3 = xtra1 + 1; //�G�b�Z���X�t�^ TR�t���O+1�̒l

		p_ptr->notice |= (PN_COMBINE | PN_REORDER);
		p_ptr->window |= (PW_INVEN);
		p_ptr->update |= (PU_BONUS);

		break;
	}
	case JKF1_IDENTIFY:
	{
		if (xtra1)
		{
			if (!identify_fully(FALSE)) return FALSE;
		}
		else
		{
			if (!ident_spell(FALSE)) return FALSE;
		}

		break;
	}
	case JKF1_DESTROY:
	{
		destroy_area(py, px, dice_roll_result, FALSE, FALSE, FALSE);

		break;
	}
	case JKF1_SILENT_FLOOR:
	{
		msg_print("���͂����˂ɐÂ܂�Ԃ���..");
		p_ptr->silent_floor = 1;

		break;
	}
	case JKF1_MASS_GENO:
	{
		mass_genocide(dice_roll_result, TRUE);
		break;
	}
	case JKF1_CALL_CHAOS:
	{
		call_chaos();
		break;
	}
	case JKF1_TELE_AWAY_ALL:
	{
		banish_monsters(dice_roll_result);
		break;
	}
	case JKF1_DEATH_RAY:
	{
		deathray_monsters();
		break;
	}
	case JKF1_MOSES:
	{
		project_hack2(GF_MOSES, dice, sides, base);
		break;
	}
	case JKF1_STASIS:
	{
		stasis_monsters(dice_roll_result);
		break;
	}
	case JKF1_DISTURB_SUMMON:
	{
		set_deportation(dice_roll_result, FALSE);
		break;
	}
	case JKF1_ALTER_REALITY:
	{
		alter_reality();
		break;
	}
	case JKF1_CALL_METEOR:
	{

		cast_meteor(dice_roll_result, rad, GF_METEOR);
		break;
	}
	case JKF1_EARTHQUAKES:
	{
		earthquake(py, px, dice_roll_result);
	}
	break;
	case JKF1_DISARM:
	{
		project(0, dice_roll_result, py, px, 0, GF_KILL_DOOR, PROJECT_HIDE | PROJECT_GRID, -1);
	}
	break;
	case JKF1_LUCKY:
	{
		set_tim_lucky(dice_roll_result, FALSE);
		break;
	}


	//���m�Ȃ�
	case JKF1_DETECT_ALL:
	{
		detect_all(base);
		if (xtra1)map_area(base);
		break;
	}

	//���f
	case JKF1_DAZZLE:
	{
		confuse_monsters(dice_roll_result);
		if ((xtra1) % 2) stun_monsters(dice_roll_result);
		if ((xtra1 >> 1) % 2) turn_monsters(dice_roll_result);
		break;
	}

	//�^�[�Q�b�g�\�͒ቺ ����ቺ�����邩xtra1�ɋL�^
	case JKF1_DEBUFF:
	{

		if (!get_aim_dir(&dir)) return FALSE;

		if (xtra1 == 1)
			fire_ball_hide(GF_DEC_ATK, dir, dice_roll_result, 0);
		else if (xtra1 == 2)
			fire_ball_hide(GF_DEC_DEF, dir, dice_roll_result, 0);
		else if (xtra1 == 3)
			fire_ball_hide(GF_DEC_MAG, dir, dice_roll_result, 0);
		else
			fire_ball_hide(GF_DEC_ALL, dir, dice_roll_result, 0);

		break;
	}

	//���͏���
	case JKF1_DISPEL:
	{
		int m_idx;

		if (!target_set(TARGET_KILL)) return FALSE;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) break;
		dispel_monster_status(m_idx);
		break;
	}



	default:
		msg_format("ERROR:activate_nameless_art_aux()�֕s���ȃt���O�l���n���ꂽ(%d)", art_idx);
		return FALSE;
	}

	return TRUE;
}


//�u�����Ȃ��Z�v�g�p���[�`��
//magic_num1[]�ɋZ�p�����[�^���ۑ�����Ă���E�����̃��[�`�����g���B���̏����ςƖ��̐��E��俎q���g�p����B
//�s���������Ƃ�TRUE��Ԃ�
//v1.1.56 ���s���𕪗����Z�̃p�����[�^���\���̂ŊǗ�����悤�ύX
bool	activate_nameless_art(bool only_info)
{

	int num, i;
	int list_num = JUNKO_ARTS_COUNT;//�ő�9
	int dir;
	bool flag_choose = FALSE;
	nameless_arts_activate_param_type naap;

	int mode;



	if (!USE_NAMELESS_ARTS)
	{
		msg_print("ERROR:activate_nameless_art()�����o�^�̃L�����N�^�[�ŌĂ΂ꂽ");
		return FALSE;
	}

	if (p_ptr->pclass == CLASS_JUNKO)
		mode = NAMELESS_ART_MODE_JUNKO;
	else
		mode = NAMELESS_ART_MODE_SUMIREKO;

	if (!JUNKO_ARTS_PARAM(0, JKAP_JKF1))
	{
		msg_print("�܂��Z���Ȃ��B");
		return FALSE;
	}


	//ALLOW_REPEAT���s�[�g����
	if (repeat_pull(&num)) flag_choose = TRUE;

	//�Z�I�����[�v
	screen_save();
	while (!flag_choose)
	{
		char c;
		num = -1;

		//��ʃN���A
		for (i = 1; i<16; i++) Term_erase(17, i, 255);

		if (only_info)
			c_prt(TERM_WHITE, "�ǂ̏����m�F���܂����H", 3, 20);
		else
			c_prt(TERM_WHITE, "�ǂ���g���܂����H", 3, 20);

		c_prt(TERM_WHITE, "                                            �g�pMP ����", 4, 20);
		//���X�g�\��
		for (i = 0; i<list_num; i++)
		{
			byte color;
			char tmp_desc[64];
			char tmp_desc2[64];

			read_nameless_arts_param_from_player(i, &naap);


			if (!naap.art_idx) break;//�����̋Z������Ă��Ȃ�

			if (p_ptr->csp < naap.cost) color = TERM_L_DARK;//MP�s��
			else  color = TERM_WHITE;

			if (naap.gf_type && naap.art_idx != JKF1_ADD_ESSENCE1 && naap.art_idx != JKF1_ADD_ESSENCE2)
				sprintf(tmp_desc2, "[%s]", gf_desc_name[naap.gf_type]);
			else
				sprintf(tmp_desc2, "");

			if (naap.gf_type == GF_MAKE_TREE || naap.gf_type == GF_STONE_WALL || naap.gf_type == GF_MAKE_DOOR)
				sprintf(tmp_desc, "");
			else if (naap.art_idx == JKF1_ATTACK_BREATH) //�u���X��xtra1�̒l�ňЗ͂��ς��
				sprintf(tmp_desc, "(%d)", p_ptr->chp * naap.xtra1 / 100);
			else if (naap.base < 1 && (naap.dice < 1 || naap.sides < 1))
				sprintf(tmp_desc, "");
			else if (naap.base < 1)
				sprintf(tmp_desc, "(%dd%d)", naap.dice, naap.sides);
			else if (naap.dice < 1 || naap.sides < 1)
				sprintf(tmp_desc, "(%d)", naap.base);
			else
				sprintf(tmp_desc, "(%d+%dd%d)", naap.base, naap.dice, naap.sides);

			c_prt(color, format("%c) %-45s %-4d %s%s%s", ('a' + i), quark_str(naap.quark_idx),
				naap.cost, tmp_desc2, tmp_desc, ((naap.art_idx == JKF1_ATTACK_AROUND) ? "/2" : "")), 5 + i, 17);

		}

		c = inkey();

		if (c == ESCAPE)
		{
			break;
		}

		if (c >= 'a' && c <= ('a' + list_num - 1))
		{
			//�Z���Ȃ��Ƃ��������I�������f���ă��[�v����o��
			num = c - 'a';
			if (!JUNKO_ARTS_PARAM(num, JKAP_JKF1)) continue;
			if (!only_info) flag_choose = TRUE;
			repeat_push(num);
		}

		//only_info�̂Ƃ���ʂɏ�񂾂��\�����ă��[�v���s ESC�ŏI��
		if (only_info && num >= 0)
		{
			read_nameless_arts_param_from_player(num, &naap);
			display_nameless_art_effect(&naap, mode);

			continue;
		}

	}
	screen_load();

	if (num < 0) return FALSE; //ESC�͏I��

	//�I�������Z�̃p�����[�^��p_ptr->magic_num1[]����擾
	read_nameless_arts_param_from_player(num, &naap);


	if (cheat_xtra)
	{
		msg_format("name:%s", quark_str(naap.quark_idx));
		msg_format("idx:%d GF:%d", naap.art_idx, naap.gf_type);
		msg_format("base:%d,dice:%d,sides:%d", naap.base, naap.dice, naap.sides);
		msg_format("counter_idx:%d", naap.counter_idx);
		msg_format("xtra:%d/%d", naap.xtra1, naap.xtra2);
	}


	if (p_ptr->csp < naap.cost)
	{
		msg_print("MP������Ȃ��B");
		return FALSE;
	}

	//���Z���s
	if (!activate_nameless_art_aux(&naap, mode)) return FALSE;

	//MP����
	p_ptr->csp -= naap.cost;

	return TRUE;

}


//�����JKF1_*�t���O�̈ꎞ���ʂ�tim_gen[]�̂ǂꂩ�ŗL���ɂȂ��Ă��邩�ǂ������`�F�b�N
bool check_activated_nameless_arts(int art_idx)
{
	int i;

	//�X�y�J�̔���
	if (p_ptr->tim_spellcard_count && art_idx == p_ptr->spellcard_effect_idx)
		return TRUE;

	if(!USE_NAMELESS_ARTS) return FALSE;

	for(i=0;i<JUNKO_ARTS_COUNT;i++)
	{
		int tmp_idx = JUNKO_ARTS_PARAM(i,JKAP_JKF1);
		int count_num = JUNKO_ARTS_PARAM(i,JKAP_COUNT_NUM);
		if(count_num < 0) continue;//�J�E���g���g��Ȃ����Z
		if(count_num >= TIM_GENERAL_MAX){msg_format("nameless arts count_num�ݒ�G���[(%d)",count_num); return FALSE;}//paranoia
		if( tmp_idx == art_idx && p_ptr->tim_general[count_num]) return TRUE;

	}

	return FALSE;
}


//�u���������Z�v�̎��Ԍ��ʂ�����/�I�����ꂽ�Ƃ��̃��b�Z�[�W��Ԃ��B
//���Zart_idx���w�肷��B�s���ȏꍇ��tim_general[]�̃C���f�b�N�X����T���B
cptr msg_tim_nameless_arts(int art_idx, int counter_idx, bool activate)
{
	int i;

	//tim_general[]�̃^�C���J�E���^�I������art_idx�l���ێ�����Ă��Ȃ��̂ŁA���ݎg������Z����Y��������̂�T��

	if ((USE_NAMELESS_ARTS) && !art_idx)
	{
		for (i = 0; i<JUNKO_ARTS_COUNT; i++)
		{
			if (!nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) continue;
			if (JUNKO_ARTS_PARAM(i, JKAP_COUNT_NUM) == counter_idx)
			{
				art_idx = JUNKO_ARTS_PARAM(i, JKAP_JKF1);
				if (cheat_xtra) msg_format("art_idx found:%d", art_idx);
				break;
			}
		}
	}

	switch (art_idx)
	{
	case JKF1_VANISH_BEAM:
		if (activate)
			return "���Ȃ��̓��[�U�[��{���g�ɑ΂��Đg�\�����B";
		else
			return "���[�U�[��{���g�ւ̖h�䂪�������B";

	case JKF1_EXPLOSION_DEF:
		if (activate)
			return "���Ȃ��͔����ɑ΂��Ĕ������B";
		else
			return "�����ւ̖h�䂪�������B";

	case JKF1_BREATH_DEF:
		if (activate)
			return "���Ȃ��̓u���X�U���ɑ΂��Ĕ������B";
		else
			return "�u���X�U���ւ̖h�䂪�������B";

	case JKF1_DESTRUCT_DEF:
		if (activate)
			return "���Ȃ���*�j��*�ɔ������B";
		else
			return "*�j��*�̗͂ւ̖h�䂪�������B";


	case JKF1_ABSORB_MAGIC:
		if (activate)
			return "���Ȃ��͓G�����@���g���̂�҂��\���Ă���...";
		else
			return "���@���z���ł��Ȃ��Ȃ����B";

	case JKF1_KILL_WALL:
		if (activate)
			return "���Ȃ��̕��݂͗͋����𑝂����B";
		else
			return "�G�邾���ŕǂ�j��ł��Ȃ��Ȃ����B";
	case JKF1_MASTER_MAGITEM:
		if (activate)
			return "���Ȃ疂����������Ə�肭������B";
		else
			return "���������肭�����Ȃ��Ȃ����B";

	case JKF1_SELF_LIGHT:
		if (activate)
			return "���Ȃ��͌���n�߂��B";
		else
			return "���Ȃ��͌���Ȃ��Ȃ����B";
	case JKF1_MASTER_MELEE:
		if (activate)
			return "���Ȃ��͕��p�̒B�l�ɂȂ����I";
		else
			return "�����̐؂ꂪ�����Ȃ����B";
	default:
		return "(ERROR:���̈ꎞ���ʂ̃��b�Z�[�W���o�^����Ă��Ȃ�)";


	}

}


//v1.1.56 �^���������t���[�o�[�A�C�e���u�X�y���J�[�h�v�֘A
//���J���@�X�ɒ~�ς��ꂽ���͒l����J�[�h�쐬�̂Ƃ��̃{�[�i�X�l���v�Z����
int calc_make_spellcard_bonus(void)
{
	int mag_type;
	int marisa_total_mag_power = 0;
	int flag_bonus;

	//�X�y���쐬�̂��߂̃t���O�{�[�i�X�l�𖂖@�X�ɔ��~���ꂽ���͒l����K���ɏd�݂Â����Čv�Z����B���ʂɔ��S����邾���Ȃ�1�����炢���܂��Ă邾�낤���B
	for (mag_type = 0; mag_type<8; mag_type++)
	{
		int power_mult;
		switch (mag_type)
		{
		case MARISA_POWER_SUPER:
			power_mult = 7; break;
		case MARISA_POWER_ILLU:
		case MARISA_POWER_STAR:
			power_mult = 5; break;
		case MARISA_POWER_LITE:
		case MARISA_POWER_DARK:
			power_mult = 3; break;
		default:
			power_mult = 1; break;

		}
		marisa_total_mag_power += marisa_magic_power[mag_type] * power_mult;
	}

	if (marisa_total_mag_power < 1000)
		flag_bonus = 10;
	else if (marisa_total_mag_power < 25000)
		flag_bonus = marisa_total_mag_power / 100;
	else
		flag_bonus = 250 + (marisa_total_mag_power - 25000) / 200;
	if (flag_bonus > 500) flag_bonus = 500;

	if (p_ptr->wizard) flag_bonus = 50 + randint1(450);

	return flag_bonus;

}


//�����Ȃ��Z�����T�u�֐�1/3
//���Zart_idx(JKF1_***�ɑΉ����鐔��junko_arts_table[]�̃C���f�b�N�X)�����肷��
//����������art_idx���A���s������0��Ԃ�
//mode:�������[�h�@������g���L�����ɂ���ĕς��
//material_idx;���ʐ���̂Ƃ�magatama_material_table[]�̃C���f�b�N�X
//boost:���ʐ���̂Ƃ��u��S�̏o���v���ǂ���
int	make_nameless_arts_sub1_choose_idx(int mode, int material_idx, bool boost)
{
	int art_idx;
	int loop_max = 100;
	int plev = p_ptr->lev;

	//100��I�������
	while (loop_max--)
	{
		int randomizer = 0;
		int i;
		bool flag_ok = TRUE;

		//�܂����A���e�B�ɏ������m����art_idx��I�肷��
		switch (mode)
		{
			//���ς�magic_num2[]�Ɋi�[���ꂽ�t���O�l�ɂ��I��m�����ω�����(�������܂�@�\���Ă��Ȃ��C������)
			case NAMELESS_ART_MODE_JUNKO:
			{	

				//���܂萶���������ăt���O�l���Ȃ��Ȃ鎖�Ԃ�h�����߁A���x��������Ă�t���O�l���������₷�B���A���e�B�̍����t���O�͑����Â炢
				for (i = 1; i <= JKF1_MAX; i++)
				{
					if (p_ptr->magic_num2[i] > 100) continue;
					if (nameless_arts_generate_table[i].min_lev > plev) continue;
					if (nameless_arts_generate_table[i].rarity > 30 && !one_in_(nameless_arts_generate_table[i].rarity / 3)) continue;
					p_ptr->magic_num2[i]++;
				}

				//�t���O�l�ƃ��A���e�B�ɏ������m���œ��ZJKF�l��I�肷��

				for (i = 1; i <= JKF1_MAX; i++)
				{
					randomizer += p_ptr->magic_num2[i] * 100 / (nameless_arts_generate_table[i].rarity);
				}
				randomizer = randint1(randomizer);
				for (i = 1; i <= JKF1_MAX; i++)
				{
					randomizer -= p_ptr->magic_num2[i] * 100 / (nameless_arts_generate_table[i].rarity);
					if (randomizer <= 0) break;
				}
				art_idx = i;

			}
			break;
			//���ʐ���͎g�p�����f�ނƑ����̗ǂ����Z���I�肳��₷������
			case NAMELESS_ART_MODE_MAGATAMA:
			{

				for (i = 1; i <= JKF1_MAX; i++)
				{
					int tmp_rarity = nameless_arts_generate_table[i].rarity;

					if (nameless_arts_generate_table[i].arts_type == magatama_material_table[material_idx].suitable_skill_type) tmp_rarity /= 8;
					if (tmp_rarity < 1) tmp_rarity = 1;
					randomizer += 1000 / tmp_rarity;
				}
				randomizer = randint1(randomizer);
				for (i = 1; i <= JKF1_MAX; i++)
				{
					int tmp_rarity = nameless_arts_generate_table[i].rarity;

					if (nameless_arts_generate_table[i].arts_type == magatama_material_table[material_idx].suitable_skill_type)	tmp_rarity /= 8;
					if (tmp_rarity < 1) tmp_rarity = 1;
					randomizer -= 1000 / tmp_rarity;

					if (randomizer <= 0) break;
				}
				art_idx = i;

			}
			break;

			//�ق��̓��A���e�B�݂̂Ō��߂�
			default:
			{	
				for (i = 1; i <= JKF1_MAX; i++)
				{
					randomizer += 1000 / nameless_arts_generate_table[i].rarity;
				}
				randomizer = randint1(randomizer);
				for (i = 1; i <= JKF1_MAX; i++)
				{
					randomizer -= 1000 / nameless_arts_generate_table[i].rarity;
					if (randomizer <= 0) break;
				}
				art_idx = i;

			}

		}

		if (art_idx <= 0 || art_idx > JKF1_MAX)
		{
			msg_format("ERROR:make_nameless_arts_sub1()��idx����������(%d)", art_idx);
			return 0;
		}

		//����mode�ɍ��킹�ē���̋Z�̑I���}�~����
		switch (mode)
		{
			//����
		case NAMELESS_ART_MODE_JUNKO:
			//�����Z����������蒼��(�����U���Z����)
			for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (JUNKO_ARTS_PARAM(i, JKAP_JKF1) == art_idx && art_idx >JKF1_ATTACK_FLOOR) { flag_ok = FALSE; break; }
			//���x�������B�ł����܂�15���x����܂ŏo��
			if (nameless_arts_generate_table[art_idx].min_lev >(plev + (weird_luck() ? 15 : 0))) { flag_ok = FALSE; break; }

			//tim_gen�J�E���^���g���Z�����ł�5����ꍇ�I�ђ���
			if (nameless_arts_generate_table[art_idx].use_counter)
			{
				int	tim_counter_num = 0;
				for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) tim_counter_num++;
				if (tim_counter_num >= 5) { flag_ok = FALSE; break; }
			}

			break;
			//俎q��p���i��
		case NAMELESS_ART_MODE_SUMIREKO:

			//���\�͂ɂ���Z�͏o�Ȃ����Ƃɂ���
			if (art_idx == JKF1_DETECT_MON || art_idx == JKF1_DETECT_TRAPS || art_idx == JKF1_DETECT_OBJ || art_idx == JKF1_ENLIGHT || art_idx == JKF1_TELEPO
				|| art_idx == JKF1_SPEED || art_idx == JKF1_SHIELD || art_idx == JKF1_RES_ELEM || art_idx == JKF1_IDENTIFY || art_idx == JKF1_DETECT_ALL)
			{
				flag_ok = FALSE; break;
			}

			//�G�b�Z���X�t�^����߂Ƃ�
			if (art_idx == JKF1_ADD_ESSENCE1 || art_idx == JKF1_ADD_ESSENCE2) { flag_ok = FALSE; break; }

			//tim_gen�J�E���^���g���Z�����ł�5����ꍇ�I�ђ���
			if (nameless_arts_generate_table[art_idx].use_counter)
			{
				int	tim_counter_num = 0;
				for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) tim_counter_num++;
				if (tim_counter_num >= 5) { flag_ok = FALSE; break; }
			}

			//���x�������B�ł����܂�15���x����܂ŏo��
			if (nameless_arts_generate_table[art_idx].min_lev >(plev + (weird_luck() ? 15 : 0))) { flag_ok = FALSE; break; }

			//�g�p�\���x�������̃��x�����20�ȏ�Ⴂ�Z�͏o�Ȃ����Ƃɂ���
			if (nameless_arts_generate_table[art_idx].min_lev < (plev - 20)) { flag_ok = FALSE; break; }

			//�����Z����������蒼��(�����U���Z����)
			for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (JUNKO_ARTS_PARAM(i, JKAP_JKF1) == art_idx && art_idx >JKF1_ATTACK_FLOOR) { flag_ok = FALSE; break; }

			break;

			//�^�E���������@���J���@�X�ŃX�y���J�[�h����
		case NAMELESS_ART_MODE_SPELLCARD:


			//�^�E������Ȃ̂Ŏア�Z��X����A�C�e���ő�ւł���Z�͏o���Ȃ�
			if (art_idx == JKF1_ATTACK_BOLT || art_idx == JKF1_FOOD || art_idx == JKF1_HEAL1 || art_idx == JKF1_DETECT_TRAPS || art_idx == JKF1_DETECT_OBJ) { flag_ok = FALSE; break; }
			if (art_idx == JKF1_REMOVE_CURSE || art_idx == JKF1_IDENTIFY || art_idx == JKF1_SELF_LIGHT) { flag_ok = FALSE; break; }
			//*�j��*�h��͂����p���Ȃ��͂��Ȃ̂ŏo���Ȃ�
			//v2.0.4 �^������ł��I�x�������œ|�����肦��悤�ɂȂ����̂ŗ}�~���~
			//if (art_idx == JKF1_DESTRUCT_DEF) { flag_ok = FALSE; break; }
			//�G�b�Z���X�t�^�̓t���O�Ǘ����ʓ|�������X�v���Ǝv���̂ł�߂Ƃ�
			if (art_idx == JKF1_ADD_ESSENCE1 || art_idx == JKF1_ADD_ESSENCE2) { flag_ok = FALSE; break; }
			//���x�������B�ł����܂�15���x����܂ŏo��
			//�������x��MAX���Ǝv���̂Ńt���A�U��(���x��57)���o�Â炢�����Ƃ��Ă̂݋@�\����͂�
			if (nameless_arts_generate_table[art_idx].min_lev >(plev + (weird_luck() ? 15 : 0))) { flag_ok = FALSE; break; }
			break;

			//���{�ۂ̌��ʐ���
		case NAMELESS_ART_MODE_MAGATAMA:
			{
				int material_level = magatama_material_table[material_idx].mlev;

				//v2.0.4b �u��S�̏o���v�ɂȂ����Ƃ��_�C�X�l��x�[�X�l���Ȃ��f�ރ��x���ȉ��̌��ʂ����m���Œe�����Ƃɂ���
				if (boost && !nameless_arts_generate_table[art_idx].base && nameless_arts_generate_table[art_idx].min_lev <= material_level && !one_in_(6))continue;

				if (boost) material_level += 10;


				//�f�ރp���[��0���Ɖ����Ȃ�(�Ή򂪍ޗ����Ɓu��S�̏o���v�ɂȂ�Ȃ����莸�s����悤�ɂ��邽�߂̏���)
				if (material_level<1) return 0;

				//���Z���ނȂ�(�G�b�Z���X�t�^�̂�)�͌��ʐ���̑ΏۊO
				if (!nameless_arts_generate_table[art_idx].arts_type) continue;

				//���x�������B���̃��x���łȂ��f�ރ��x�����Q�Ƃ���B��S�̏o������10���x����܂ŏo��
				if (nameless_arts_generate_table[art_idx].min_lev > material_level) { flag_ok = FALSE; break; }

				//�f�ރ��x�����11���x���ȏ�Ⴂ�Z��1/3�A21���x���ȏ�Ⴂ�Z��1/9�A30���x���ȏ�Ⴂ�Z�͑I�肳��Ȃ�
				if (nameless_arts_generate_table[art_idx].min_lev <= (material_level - 30) ) { flag_ok = FALSE; break; }
				if (nameless_arts_generate_table[art_idx].min_lev < (material_level - 10) && !one_in_(3)) { flag_ok = FALSE; break; }
				if (nameless_arts_generate_table[art_idx].min_lev < (material_level - 20) && !one_in_(3)) { flag_ok = FALSE; break; }


			}
			break;

		default:
			msg_format("ERROR:make_nameless_arts_sub1()�ɐ�����mode�l(%d)���ݒ肳��Ă��Ȃ�", mode);
			return 0;

		}

		if (!flag_ok) continue;

		break;

	}

	if (loop_max < 1)
	{
		msg_format("ERROR:make_nameless_arts_sub1()��100��J��Ԃ��Ă��Z�����܂�Ȃ�����");
		return 0;
	}

	//���ς͋Z�̐����Ɏg�����t���O�l��50���炷
	if (mode == NAMELESS_ART_MODE_JUNKO)
	{
		p_ptr->magic_num2[art_idx] -= MIN(50, p_ptr->magic_num2[art_idx]);
	}

	if (p_ptr->wizard) msg_format("magatama_art_idx:%d",art_idx);

	return art_idx;

}


//�����Ȃ��Z�����T�u�֐�2/3
//���Zart_idx��n���A�U���Z�̑����l�����߂�B
//���߂������l��naap_ptr���Ɋi�[����
//����������elem_idx(JKF2_***�ɑΉ�����l)���A���s���邩�s�v�ȏꍇ0��Ԃ�
//mode:�������[�h�@������g���L�����ɂ���ĕς��
//material_idx;���ʐ���̂Ƃ�magatama_material_table[]�̃C���f�b�N�X
//boost:���ʐ���̂Ƃ��u��S�̏o���v���ǂ���
int	make_nameless_arts_sub2_choose_gf(int mode, nameless_arts_activate_param_type *naap_ptr, int material_idx, bool boost)
{
	int gf_idx, elem_idx,art_idx;
	byte tmp_flags[MAGIC_NUM_SIZE];
	int loop_max = 100;
	int plev = p_ptr->lev;
	int i;

	art_idx = naap_ptr->art_idx;
	//�ŏ���0�����Ă���
	naap_ptr->gf_type = 0;

	//�����l�̕s�v�ȓ��Z�͉������Ȃ�
	if (!nameless_arts_generate_table[art_idx].need_flag2) return 0;

	//tmp_flags[]�ɁA�����l�̕t�^����₷���̏����ݒ���s���B
	//俎q��p���i�̏ꍇ�A�����l�̃��A���e�B����Ƀ����_���Ȓl�����Z����B���x���������قǏ�ʑ�����������₷�����Ƃɂ���B
	//�X�y���J�[�h����������ł������낤
	if (mode == NAMELESS_ART_MODE_SUMIREKO || mode == NAMELESS_ART_MODE_SPELLCARD)
	{
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			if (i <= JKF2_POIS) tmp_flags[i] = 200;
			else tmp_flags[i] = 50 + p_ptr->lev * 5;
		}
	}
	//���ʐ��쎞�ɂ͑f�ނƑ����̗ǂ��������I�肳��₷������
	else if (mode == NAMELESS_ART_MODE_MAGATAMA)
	{
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			if (i <= JKF2_POIS) tmp_flags[i] = 15;
			else tmp_flags[i] = magatama_material_table[material_idx].mlev / 5;

			if (magatama_material_table[material_idx].suitable_gf_type == i) tmp_flags[i] = 250;
		}
	}

	//���ς�magic_num2[]�ɋL�^���ꂽ�t���O�l�ɂ�葮���̕t�^����₷�����ω�����
	else if (mode == NAMELESS_ART_MODE_JUNKO)
	{
		//�t���O�l�Ƀ����_���Ȓl�����Z
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			tmp_flags[i] = p_ptr->magic_num2[i];

			if (i <= JKF2_POIS && tmp_flags[i] < 100)  tmp_flags[i] += randint1(10);
			else if (i <= JKF2_STEAM && tmp_flags[i] < 50)  tmp_flags[i] += randint1(5);
			else if (i <= JKF2_END && tmp_flags[i] < 30)  tmp_flags[i] += randint1(3);

		}

	}
	else
	{
		msg_format("ERROR:make_nameless_arts_sub2_choose_gf()�ɓK�؂ȓ��샂�[�h���ݒ肳��Ă��Ȃ�");
		return 0;
	}


	while (loop_max--)
	{
		int randomizer = 0;
		bool flag_ok = TRUE;

		//tmp_flags[]�ɐݒ肵���l�����Ƃɂ��đ����l�������_���Ɍ��߂�
		//(����:���̎��_�ł͂����܂ő����t���O���idx(JKF2_***)�ł���GF_XXX�̒l�ł͂Ȃ�)
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			randomizer += tmp_flags[i];
		}
		randomizer = randint1(randomizer);
		for (i = JKF2_START; i <= JKF2_END; i++)
		{
			randomizer -= tmp_flags[i];
			if (randomizer <= 0) break;
		}
		elem_idx = i;

		if (elem_idx < JKF2_START || elem_idx > JKF2_END)
		{
			msg_format("ERROR:make_nameless_arts_sub2_choose_gf()�Ō��肳�ꂽ�����l����������(%d)", elem_idx);
			return 0;
		}

		//�h�A�����Ȃǒn�`�ω��`�����̃{���g��t���A�U���͂�����ƕςȂ̂ŏ��O
		if ((art_idx == JKF1_ATTACK_BOLT || art_idx == JKF1_ATTACK_FLOOR) && (elem_idx == JKF2_MAKE_WALL || elem_idx == JKF2_MAKE_DOOR || elem_idx == JKF2_MAKE_TREE))
			flag_ok = FALSE;


		//�����t���O��idx��GF_XXX�̒l�ɕϊ�����B�Ƃ��ǂ��Ή����v���Y�}�ɂȂ�����ω�������

		switch (elem_idx)
		{
		case JKF2_ACID:
			gf_idx = GF_ACID;
			break;
		case JKF2_ELEC:
			if (one_in_(6)) gf_idx = GF_PLASMA;
			else gf_idx = GF_ELEC;
			break;
		case JKF2_FIRE:
			if (one_in_(9) && art_idx != JKF1_ATTACK_FLOOR && art_idx != JKF1_ATTACK_BOLT) gf_idx = GF_LAVA_FLOW;
			else if (one_in_(6)) gf_idx = GF_PLASMA;
			else gf_idx = GF_FIRE;
			break;
		case JKF2_COLD:
			if (one_in_(9)) gf_idx = GF_ICE;
			else gf_idx = GF_COLD;
			break;
		case JKF2_POIS:
			if (one_in_(7)) gf_idx = GF_POLLUTE;
			else gf_idx = GF_POIS;
			break;
		case JKF2_LITE:
			gf_idx = GF_LITE;
			break;
		case JKF2_DARK:
			gf_idx = GF_DARK;
			break;
		case JKF2_SHARDS:
			if (one_in_(6)) gf_idx = GF_WINDCUTTER;
			gf_idx = GF_SHARDS;
			break;
		case JKF2_WATER:
			if (one_in_(6) && art_idx != JKF1_ATTACK_FLOOR && art_idx != JKF1_ATTACK_BOLT) gf_idx = GF_WATER_FLOW;
			else gf_idx = GF_WATER;
			break;
		case JKF2_SOUND:
			gf_idx = GF_SOUND;
			break;
		case JKF2_NUKE:
			gf_idx = GF_NUKE;
			break;
		case JKF2_MANA:
			gf_idx = GF_MANA;
			break;
		case JKF2_METEOR:
			gf_idx = GF_METEOR;
			break;
		case JKF2_CHAOS:
			gf_idx = GF_CHAOS;
			break;
		case JKF2_NETHER:
			gf_idx = GF_NETHER;
			break;
		case JKF2_DISENCHANT:
			gf_idx = GF_DISENCHANT;
			break;
		case JKF2_TIME:
			gf_idx = GF_TIME;
			break;
		case JKF2_DISTORTION:
			if (one_in_(3)) gf_idx = GF_NEXUS;
			else gf_idx = GF_DISTORTION;
			break;
		case JKF2_HOLY:
			gf_idx = GF_HOLY_FIRE;
			break;
		case JKF2_HELL:
			gf_idx = GF_HELL_FIRE;
			break;
		case JKF2_DISINTEGRATE:
			gf_idx = GF_DISINTEGRATE;
			break;
		case JKF2_WIND:
			gf_idx = GF_TORNADO;
			break;
		case JKF2_STEAM:
			gf_idx = GF_STEAM;
			break;
		case JKF2_MAKE_WALL:
			gf_idx = GF_STONE_WALL;
			break;
		case JKF2_MAKE_DOOR:
			gf_idx = GF_MAKE_DOOR;
			break;
		case JKF2_MAKE_TREE:
			gf_idx = GF_MAKE_TREE;
			break;
		default:
			msg_format("ERROR:make_nameless_arts_sub2_choose_gf()�Ńt���O�l%d�ɑΉ�����GF�l�����܂��Ă��Ȃ�", elem_idx);
			return 0;
		}

		//���ς�俎q�́A���łɎ����Ă���Z�Ɠ�����ނ����������̋Z���o����I�ђ���
		if (mode == NAMELESS_ART_MODE_JUNKO || mode == NAMELESS_ART_MODE_SUMIREKO)
		{
			for (i = 0; i < JUNKO_ARTS_COUNT_MAX; i++)
			{
				if (JUNKO_ARTS_PARAM(i, JKAP_JKF1) == naap_ptr->art_idx && JUNKO_ARTS_PARAM(i, JKAP_GF) == gf_idx)
					flag_ok = FALSE;
			}
		}
		//���ʐ���̂Ƃ��ǁA�X�A�h�A�����͂ł��ɂ���
		else if (mode == NAMELESS_ART_MODE_MAGATAMA)
		{
			if ((elem_idx == JKF2_MAKE_WALL || elem_idx == JKF2_MAKE_DOOR || elem_idx == JKF2_MAKE_TREE) && !weird_luck()) flag_ok = FALSE;

		}

		if (!flag_ok) continue;

		break;

	}

	if (!loop_max)
	{
		msg_format("ERROR:make_nameless_arts_sub2()��100��J��Ԃ��Ă������l�����܂�Ȃ�����");
		return 0;
	}

	//���ς͋Z�̐����Ɏg�����t���O�l��50���炷
	if (mode == NAMELESS_ART_MODE_JUNKO)
	{
		p_ptr->magic_num2[elem_idx] -= MIN(50, p_ptr->magic_num2[elem_idx]);
	}


	naap_ptr->gf_type = gf_idx;

	return elem_idx;

}


//�����Ȃ��Z�����T�u�֐�3/3
//���Z�̃R�X�g��_�C�X�Ȃǂ܂����߂ĂȂ��p�����[�^�����肷��
//mode:�������[�h�@������g���L�����ɂ���ĕς��
//material_idx;���ʐ���̂Ƃ�magatama_material_table[]�̃C���f�b�N�X
//boost:���ʐ���̂Ƃ��u��S�̏o���v���ǂ���
void	make_nameless_arts_sub3_apply_other_params(nameless_arts_activate_param_type *naap_ptr, int mode, int material_idx, bool boost)
{

	int art_idx, gf_idx, cost, rad, base, dice, sides, counter_idx, xtra1, xtra2;
	int plev = p_ptr->lev;
	int flag_bonus;
	int mult;	//�З͂ɑ΂��郉���_���␳

	mult = randnor(100, 20);
	if (mult < 75) mult = 75;


	//�_�C�X�l�Ȃǂɑ΂���{�[�i�X������
	switch (mode)
	{
	//���ς͏����ɂ��G�b�Z���X�����܂��Ă���ꍇ�Z���p���[�A�b�v����
	case NAMELESS_ART_MODE_JUNKO:
		flag_bonus = 100 + p_ptr->magic_num2[naap_ptr->art_idx];
		if (p_ptr->shero) flag_bonus += 100;
		break;
	//�X�y�J���쎞�͖��J���@�X�ɒ��܂������͂������
	case NAMELESS_ART_MODE_SPELLCARD:
		flag_bonus = calc_make_spellcard_bonus();
		break;
	//���ʐ��쎞�͑f�ނɂ�萶���p���[���ς��B��ʂɍ��邵4�{�X�Ȃ̂Ńp���[���
	case NAMELESS_ART_MODE_MAGATAMA:
		flag_bonus = plev/2 + magatama_material_table[material_idx].power;
		//��S�̏o���̂Ƃ��t���O�l�{�[�i�X��mult�l�Œ�ۏ�
		if (boost)
		{
			flag_bonus += plev / 2 + randint1(plev);
			if (mult < 110) mult = 110;
		}

		break;

	default:
		flag_bonus = 100 + plev * 2;
		break;

	}

	if (cheat_xtra) msg_format("nameless art mult:%d", mult);

	art_idx = naap_ptr->art_idx;
	gf_idx = naap_ptr->gf_type;
	cost = 0;
	rad = 0;
	base = 0;
	dice = 0;
	sides = 0;
	counter_idx = 0;
	xtra1 = 0;
	xtra2 = 0;


	//�R�X�g���߂�
	cost = nameless_arts_generate_table[art_idx].cost;
	cost += cost * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;
	//v2.0.4b mult100�𒴂���Ƃ��R�X�g�𑝂₳�Ȃ�
	if(mult < 100) cost = cost * mult / 100;

	if (cost < 1) cost = 1;
	if (cost > 400) cost = 400;

	//�_�C�X���߂�
	base = nameless_arts_generate_table[art_idx].base;
	dice = nameless_arts_generate_table[art_idx].dice;
	sides = nameless_arts_generate_table[art_idx].sides;
	if (base || dice || sides)
	{
		if (base)
		{
			base += base * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;
			base = base * (mult + (flag_bonus - 100) / 2) / 100;
			if (base < 1) base = 1;
		}
		if (dice && sides && base)
		{

			//���f�n�̓_���[�W�傫�߂ɂ���
			//v2.0.4b �͈͎w����~�X���Ă����̂ɋC�Â��ďC��
			//if (gf_idx >= GF_ELEC && gf_idx <= JKF2_FIRE)

			if (gf_idx >= GF_ELEC && gf_idx <= GF_FIRE)
			{
				base += base / 2 + randint1(base / 2);
				dice += dice / 2 + randint1((dice + 1) / 2);
				sides += (sides + 1) / 2;
			}
			else
			{
				base += randint1(base);
				dice += dice / 2 + randint1((dice + 1) / 2);
			}

			if (one_in_(2))
				dice += dice * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;
			else
				sides += sides * nameless_arts_generate_table[art_idx].lev_coef * (plev * 2) / 10000;


			sides = sides * (mult + (flag_bonus - 100) / 2) / 100;
			//�U���̂݃_�C�X�{�[�i�X����
			while (nameless_arts_generate_table[art_idx].attack && randint1(100) < plev) dice++;


			if (dice <= 0 || sides <= 0)
			{
				dice = 0;
				sides = 0;
			}
			else if (sides == 1)
			{
				base += sides;
				dice = 0;
				sides = 0;
			}

			if (one_in_(32) && dice > 1 && sides > 1 && nameless_arts_generate_table[art_idx].attack)//��m���Ń_�C�X���s���艻�@�_�C�X�{�[�i�X
			{
				sides = dice * (sides + 1) * (150 + randint1(plev * 3)) / 100 + base * (300 + randint1(plev * 4)) / 100;
				dice = 1;
				base = 0;
			}
			else if (one_in_(8) && dice > 1 && sides > 1)//��m���Ń_�C�X�s���艻�@�_�C�X�{�[�i�X
			{
				sides = dice * (sides + 1) * (100 + randint1(plev * 2)) / 100;
				dice = 1;
			}
			else if (one_in_(4)) //���m���őS��base��
			{
				base += dice * (sides + 1) / 2;
				dice = 0;
				sides = 0;
			}
		}

	}


	//����Ȉꎟ���ʂ������炷���Z�ɑ΂��Atim_general[]�̂ǂ���g�����C���f�b�N�X0-4��ݒ肷��
	//���ς�俎q�͈ꎞ�J�E���^�[��5�܂Ŏg�p����
	if (mode == NAMELESS_ART_MODE_SUMIREKO || mode == NAMELESS_ART_MODE_JUNKO)
	{
		//�J�E���^���g���Ƃ�tim_general[]�̂ǂ���g�����o�^
		if (nameless_arts_generate_table[art_idx].use_counter)
		{
			int i;
			int	tim_counter_num = 0;
			for (i = 0; i<JUNKO_ARTS_COUNT_MAX; i++) if (nameless_arts_generate_table[JUNKO_ARTS_PARAM(i, JKAP_JKF1)].use_counter) tim_counter_num++;
			counter_idx = tim_counter_num;
		}
		else
			counter_idx = 0;

	}
	//�X�y���J�[�h����̏ꍇ�Atim_general�ł͂Ȃ���p�̃J�E���^�[���g�����Ƃɂ����̂�idx�ݒ�s�v	
	else if (mode == NAMELESS_ART_MODE_SPELLCARD)
	{
		counter_idx = 0;

	}

	//���̑��K�v�ȃp�����[�^�ݒ�
	switch (art_idx)
	{
	case JKF1_ATTACK_BALL:
	case JKF1_ATTACK_ROCKET:
		rad = 1 + plev / 24 + (flag_bonus - 100) / 60;
		break;
	case JKF1_ATTACK_AROUND:
		rad = 1 + plev / 15 + (flag_bonus - 100) / 20;
		if (rad > 8) rad = 8;
		if (rad < 1) rad = 1;
		break;
	case JKF1_ATTACK_SPARK:
		rad = 1;
		if (plev * 2 + flag_bonus > 250) rad++;
		break;

	case  JKF1_ATTACK_BREATH: //�u���X�́u����HP�̉�%�̈З͂̃u���X�_���[�W�ɂȂ邩�v�̒l��xtra1�ɕۑ�
	{
		int per;
		if (randint1(100) < (plev + flag_bonus - 100)) rad = -2;
		else rad = -1;

		//v2.0.4 �p���[���Ⴂ�Ƃ��キ�Ȃ肷����̂Ōv�Z���ύX
		//xtra1 = (flag_bonus - 100 + plev) / 2;
		xtra1 = plev / 3 + flag_bonus / 3;
		//v2.0.4 ���f�n�͋���
		if (naap_ptr->gf_type <= GF_FIRE || naap_ptr->gf_type == GF_PLASMA) xtra1 += 16;

		if (xtra1 > 100) xtra1 = 100;
		if (xtra1 < 30) xtra1 = 30;

	}
	break;
	case JKF1_RUSH_ATTACK:
		if (base < 2) base = 2;
		if (base > MAX_SIGHT) base = MAX_SIGHT;
		break;
	case JKF1_MULTI_HIT:
		xtra1 = 2 + (flag_bonus - 100 + plev) / 100;
		break;
	case JKF1_MUSCLE_UP:
	case JKF1_MENTAL_UP:
	{
		xtra1 = 1 + randint1(3) + plev / 8;

		if (flag_bonus > 100)
		{
			xtra1 += 100;
		}
	}
	break;
	case JKF1_RES_ELEM://�G�������g�ϐ� 
	{
		int chance = plev + flag_bonus / 4;
		int bitflag = 0;

		if (flag_bonus > 120) bitflag = 31;//�t���O120�ȏ�őS�ϐ�

										   //�ǂ̃G�������g�ϐ��𓾂邩�����_���Ɍ��߃r�b�g�t���O��xtra1�ɋL�^
		while (!bitflag)
		{
			if (randint0(100) < chance) bitflag += 1;
			if (randint0(100) < chance) bitflag += 2;
			if (randint0(100) < chance) bitflag += 4;
			if (randint0(100) < chance) bitflag += 8;
			if (randint0(100) < chance) bitflag += 16;
		}
		xtra1 = bitflag;
	}
	break;

	//����G�b�Z���X�t�^
	case JKF1_ADD_ESSENCE1:
		switch (naap_ptr->gf_type)
		{
		case GF_ACID: case GF_POLLUTE: xtra1 = TR_BRAND_ACID; break;
		case GF_ELEC: case GF_PLASMA: xtra1 = TR_BRAND_ELEC; break;
		case GF_FIRE: case GF_LAVA_FLOW: xtra1 = TR_BRAND_FIRE; break;
		case GF_COLD: case GF_ICE: xtra1 = TR_BRAND_COLD; break;
		case GF_POIS: xtra1 = TR_BRAND_POIS; break;
		case GF_LITE: xtra1 = TR_SLAY_EVIL; break;
		case GF_DARK: xtra1 = TR_SLAY_GOOD; break;
		case GF_SHARDS: case GF_WINDCUTTER: xtra1 = TR_VORPAL; break;
		case GF_WATER: case GF_WATER_FLOW: xtra1 = TR_SLAY_UNDEAD; break;
		case GF_SOUND:xtra1 = TR_SLAY_ANIMAL; break;
		case GF_NUKE:xtra1 = TR_SLAY_HUMAN; break;
		case GF_MANA:xtra1 = TR_FORCE_WEAPON; break;
		case GF_METEOR:xtra1 = TR_CHAOTIC; break;
		case GF_CHAOS: xtra1 = TR_CHAOTIC; break;
		case GF_NETHER: xtra1 = TR_VAMPIRIC; break;
		case GF_DISENCHANT:xtra1 = TR_VAMPIRIC; break;
		case GF_TIME:case GF_NEXUS: xtra1 = TR_SLAY_DEITY; break;
		case GF_DISTORTION:xtra1 = TR_SLAY_DEMON; break;
		case GF_HOLY_FIRE:xtra1 = TR_SLAY_EVIL; break;
		case GF_HELL_FIRE:xtra1 = TR_SLAY_GOOD; break;
		case GF_DISINTEGRATE:xtra1 = TR_VORPAL; break;
		case GF_TORNADO:xtra1 = TR_IMPACT; break;
		case GF_STEAM:xtra1 = TR_SLAY_KWAI; break;
		case GF_STONE_WALL:xtra1 = TR_SLAY_DRAGON; break;
		case GF_MAKE_DOOR:xtra1 = TR_FORCE_WEAPON; break;
		case GF_MAKE_TREE:xtra1 = TR_FORCE_WEAPON; break;

		default:xtra1 = TR_FORCE_WEAPON; break;

		}
		break;

	case JKF1_ADD_ESSENCE2:
		switch (naap_ptr->gf_type)
		{
		case GF_ACID:xtra1 = TR_RES_ACID; break;
		case GF_ELEC: case GF_PLASMA: xtra1 = TR_RES_ELEC; break;
		case GF_FIRE: case GF_LAVA_FLOW: xtra1 = TR_RES_FIRE; break;
		case GF_COLD: case GF_ICE: xtra1 = TR_RES_COLD; break;
		case GF_POIS: case GF_POLLUTE: xtra1 = TR_RES_POIS; break;
		case GF_LITE:xtra1 = TR_RES_LITE; break;
		case GF_DARK:xtra1 = TR_RES_DARK; break;
		case GF_SHARDS:case GF_WINDCUTTER: xtra1 = TR_RES_SHARDS; break;
		case GF_WATER:case GF_WATER_FLOW: xtra1 = TR_RES_WATER; break;
		case GF_SOUND:xtra1 = TR_RES_SOUND; break;
		case GF_NUKE:xtra1 = TR_RES_BLIND; break;
		case GF_MANA:xtra1 = TR_DEC_MANA; break;
		case GF_METEOR:xtra1 = TR_SPEEDSTER; break;
		case GF_CHAOS:xtra1 = TR_RES_CHAOS; break;
		case GF_NETHER:xtra1 = TR_RES_NETHER; break;
		case GF_DISENCHANT:xtra1 = TR_RES_DISEN; break;
		case GF_TIME: case GF_NEXUS: xtra1 = TR_RES_TIME; break;
		case GF_DISTORTION:xtra1 = TR_RES_TIME; break;
		case GF_HOLY_FIRE:xtra1 = TR_RES_HOLY; break;
		case GF_HELL_FIRE:xtra1 = TR_RES_INSANITY; break;
		case GF_DISINTEGRATE:xtra1 = TR_REFLECT; break;
		case GF_TORNADO:xtra1 = TR_REFLECT; break;
		case GF_STEAM:xtra1 = TR_RES_WATER; break;
		case GF_STONE_WALL:xtra1 = TR_TELEPORT; break;
		case GF_MAKE_DOOR:xtra1 = TR_NO_MAGIC; break;
		case GF_MAKE_TREE:xtra1 = TR_REGEN; break;

		default:xtra1 = TR_REGEN; break;
		}
		break;


	//�Ӓ�@*�Ӓ�*�̂Ƃ�xtra1=1
	case JKF1_IDENTIFY:
	{
		if (plev + p_ptr->stat_ind[A_INT] + flag_bonus > 200) xtra1 = 1;
	}
	break;

	case JKF1_CALL_METEOR:
	{
		rad = 1 + (plev + flag_bonus) / 80;
		if (rad > 4) rad = 4;
	}
	break;

	case JKF1_DAZZLE://���f 
	{
		int chance = plev + flag_bonus / 4;
		int bitflag = 0;

		//���confuse_monsters()���������邪�ǉ��ŞN�O�Ƌ��|��
		if (randint0(100) < chance) bitflag += 1;
		if (randint0(100) < chance) bitflag += 2;

		xtra1 = bitflag;
	}
	break;

	case JKF1_DEBUFF://�^�[�Q�b�g�\�͒ቺ 
	{
		//xtra1=0�Ȃ�S�\�͒ቺ�A1,2,3�Ȃ炻�ꂼ��U���A�h��A���@�͒ቺ
		if (flag_bonus > 100)
		{
			xtra1 = 0;
			//�S�\�͒ቺ�̂Ƃ��p���[25%�ቺ
			base = (base * 3 + 1) / 4;
			if (dice > 1) dice = (dice * 3 + 1) / 4;
			else if (sides > 1) sides = (sides * 3 + 1) / 4;
		}
		else
		{
			xtra1 = randint1(3);
		}


	}
	break;


	default:
		break;
	}


	naap_ptr->cost = cost;
	naap_ptr->rad = rad;
	naap_ptr->base = base;
	naap_ptr->dice = dice;
	naap_ptr->sides = sides;
	naap_ptr->counter_idx = counter_idx;
	naap_ptr->xtra1 = xtra1;
	naap_ptr->xtra2 = xtra2;

}



/*:::���ς������Ȃ��Z�����o��*/
/*:::�n��o�����Z�̃p�����[�^�Ȃǂ��i�[���邽�߂�p_ptr->magic_num1[0-98]���g�p����*/
//���݂̃��x���ŏK���ł���S�Ă̓��Z����x�Ƀ��Z�b�g���č�蒼��
//magic_num2[]�ɒ~�ς����t���O�l�ɂ���đI�肳��₷���ɉe�����o��
//�����͋Z�̎�ނƑ����ɂ���Ď����I�ɍs����
bool	junko_make_nameless_arts(void)
{
	int num = JUNKO_ARTS_COUNT;//�ő�9
	int i;
	int plev = p_ptr->lev;
	int elem_flag_idx;
	int flag_total_val = 0;

	if (p_ptr->wild_mode || p_ptr->inside_battle) return FALSE;

	//���L���ȃ^�C���J�E���g�\�͂�����
	for (i = 0; i<TIM_GENERAL_MAX; i++)set_tim_general(0, TRUE, i, 0);

	//���ׂĂ̋Z�����Z�b�g
	for (i = 0; i<(11 * JUNKO_ARTS_COUNT); i++) p_ptr->magic_num1[i] = 0;


	for (num -= 1; num >= 0; num--)//�ő�8��0
	{
		nameless_arts_activate_param_type naap;
		int flag_bonus;
		int name_num = 500;
		char art_desc[255];
		cptr name_str[5];

		//����IDX����
		naap.art_idx = make_nameless_arts_sub1_choose_idx(NAMELESS_ART_MODE_JUNKO,0,FALSE);
		if (!naap.art_idx) return FALSE;

		//��������
		elem_flag_idx = make_nameless_arts_sub2_choose_gf(NAMELESS_ART_MODE_JUNKO, &naap,0,FALSE);

		//�_�C�X�l�Ȃǂ��̑�����
		make_nameless_arts_sub3_apply_other_params(&naap, NAMELESS_ART_MODE_JUNKO,0,FALSE);


		//����
		while (name_num--)
		{
			for (i = 0; i<5; i++)
			{
				int n_idx = nameless_arts_generate_table[naap.art_idx].name_idx[i];
				//��
				if (!n_idx) name_str[i] = "";
				//�������X�g2(�U���p)���Q��
				//������JKF2�l���v��
				else if (n_idx == -1)
				{
					name_str[i] = junko_arts_name_words_2_atk[elem_flag_idx - JKF2_START][randint0(JUNKO_NAME_LIST_NUM_SUB)];
				}
				else if (n_idx > 0 && n_idx < JUNKO_NAME_LIST_NUM1)
				{
					//�u�G�v�u����v�Ȃǂ̖������u�܂̑l�v�ȂǕʂ̖����Q�ɕς���
					if (n_idx == 1)
					{
						if (p_ptr->shero) n_idx = 0; //�b�M
						else if (one_in_(9))     n_idx += 3;
						else if (one_in_(8)) n_idx += 2;
						else if (one_in_(7)) n_idx++;
					}
					//�悭�g����������������ǉ�
					if (n_idx == 8 && one_in_(3)) n_idx = 69;
					if (n_idx == 19 && one_in_(3)) n_idx = 70;

					if (p_ptr->shero && n_idx == 5) n_idx = 0; //�b�M

					name_str[i] = junko_arts_name_words_1[n_idx][randint0(JUNKO_NAME_LIST_NUM_SUB)];
				}
				else
				{
					msg_format("ERROR:nameless_arts_generate_table[]��n_idx�l����������(%d)", n_idx);
					return FALSE;
				}
			}
			sprintf(art_desc, "%s%s%s%s%s", name_str[0], name_str[1], name_str[2], name_str[3], name_str[4]);

			//���O����������ƕt������
			//v1.1.52 50��45
			if (strlen(art_desc) > 45) continue;
			break;

		}
		if (name_num<1)
		{
			msg_format("ERROR:�Z�̖��O�����܂�Ȃ� idx:%d gf_type:%d", naap.art_idx, naap.gf_type);
			return FALSE;

		}

		naap.quark_idx = quark_add(art_desc);

		JUNKO_ARTS_PARAM(num, JKAP_JKF1) = naap.art_idx;
		JUNKO_ARTS_PARAM(num, JKAP_GF) = naap.gf_type;
		JUNKO_ARTS_PARAM(num, JKAP_COST) = naap.cost;
		JUNKO_ARTS_PARAM(num, JKAP_RAD) = naap.rad;
		JUNKO_ARTS_PARAM(num, JKAP_BASE) = naap.base;
		JUNKO_ARTS_PARAM(num, JKAP_DICE) = naap.dice;
		JUNKO_ARTS_PARAM(num, JKAP_SIDES) = naap.sides;
		JUNKO_ARTS_PARAM(num, JKAP_COUNT_NUM) = naap.counter_idx;
		JUNKO_ARTS_PARAM(num, JKAP_QUARK_IDX) = naap.quark_idx;
		JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL1) = naap.xtra1;
		JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL2) = naap.xtra2;



	}

	//msg_format("flag_total_val:%d",flag_total_val);

	return TRUE;
}


/*�B��ނ��V���ȋZ�����o���B���̏����̐��E��俎q��p*/
//�K�������Z�͏��ϓ��l��magic_num1[]�Ƀp�����[�^��ۑ�����
//num(�����߂̋Z��)�̓��͂��󂯂�B0-8
//���ςƈႢ�A������Z�𐶐����Anum�Ԗڂ̋Z�̈�Ɋi�[���A�Z�̌��ʂ�\�����Ė��O�̓��͂��󂯕t����B��x�K�������Z�͕ω����Ȃ��B
//v1.1.56 �֐��Ƃ����낢�됮��
bool	okina_make_nameless_arts(int num)
{
	int i;
	int plev = p_ptr->lev;
	int flag_bonus;
	char art_desc[255] = "";
	nameless_arts_activate_param_type naap;

	//magic_num1[]�̃T�C�Y�̓s���ŋZ����9�܂�
	if (num < 0 || num >= JUNKO_ARTS_COUNT_MAX) { msg_format("ERROR:okina_make_nameless_arts()�ɕs����num�l(%d)���n���ꂽ", num); return FALSE; }


	//����IDX����
	naap.art_idx = make_nameless_arts_sub1_choose_idx(NAMELESS_ART_MODE_SUMIREKO,0,FALSE);
	if (!naap.art_idx) return FALSE;

	//��������
	(void)make_nameless_arts_sub2_choose_gf(NAMELESS_ART_MODE_SUMIREKO, &naap,0,FALSE);

	//�_�C�X�l�Ȃǂ��̑�����
	make_nameless_arts_sub3_apply_other_params(&naap, NAMELESS_ART_MODE_SUMIREKO,0,FALSE);

	screen_save();

	while (1)
	{

		for (i = 7; i<22; i++) Term_erase(17, i, 255);

		prt("�B��ށu���O�ɐV���ȗ͂�݂��Ă�낤�I�v", 12, 28);

		display_nameless_art_effect(&naap, NAMELESS_ART_MODE_SUMIREKO);

		if (!get_string("���̋Z�����Ɩ��Â��܂����H:", art_desc, 45)) continue;

		prt(format("����:%s", art_desc), 21, 30);

		if (!get_check_strict("��낵���ł����H�F", CHECK_OKAY_CANCEL))continue;

		break;
	}


	screen_load();
	naap.quark_idx = quark_add(art_desc);


	//�Z�p�����[�^��magic_num1�Ɋi�[

	//p_ptr->magic_num1[0-98]�̓Y��%11���A
	//0:JKF1�l
	//1:����(GF)
	//2:����MP
	//3:rad
	//4:base
	//5:dice
	//6:sides
	//7:tim_gen�J�E���^�̉��Ԃ��g����
	//8:���O�pquark�C���f�b�N�X
	//9:flg�A�������͂ق������ݒ�l
	//10:�\��
	//�̒l���i�[����

	JUNKO_ARTS_PARAM(num, JKAP_JKF1) = naap.art_idx;
	JUNKO_ARTS_PARAM(num, JKAP_GF) = naap.gf_type;
	JUNKO_ARTS_PARAM(num, JKAP_COST) = naap.cost;
	JUNKO_ARTS_PARAM(num, JKAP_RAD) = naap.rad;
	JUNKO_ARTS_PARAM(num, JKAP_BASE) = naap.base;
	JUNKO_ARTS_PARAM(num, JKAP_DICE) = naap.dice;
	JUNKO_ARTS_PARAM(num, JKAP_SIDES) = naap.sides;
	JUNKO_ARTS_PARAM(num, JKAP_COUNT_NUM) = naap.counter_idx;
	JUNKO_ARTS_PARAM(num, JKAP_QUARK_IDX) = naap.quark_idx;
	JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL1) = naap.xtra1;
	JUNKO_ARTS_PARAM(num, JKAP_XTRA_VAL2) = naap.xtra2;

	return TRUE;
}





//v1.1.56 �^���������t���[�o�[�A�C�e���u�X�y���J�[�h�v�֘A
//�J�[�h�̃p�����[�^���A�C�e���ɋL�^����
bool	record_spellcard_param_onto_card(object_type *o_ptr, nameless_arts_activate_param_type *naap_ptr)
{
	if (o_ptr->tval != TV_SPELLCARD)
	{
		msg_format("ERROR:record_spellcard_param_onto_card()��TV_SPELLCARD�ȊO���n���ꂽ(%d)", o_ptr->tval); return FALSE;
	}

	if (naap_ptr->art_idx <= 0 || naap_ptr->art_idx > JKF1_MAX)
	{
		msg_format("ERROR:record_spellcard_param_onto_card()�ɂ�naap_ptr->art_idx����������(%d)", naap_ptr->art_idx); return FALSE;
	}

	o_ptr->pval = (s16b)(naap_ptr->art_idx);
	o_ptr->xtra1 = (byte)(naap_ptr->gf_type);
	o_ptr->xtra2 = (byte)(naap_ptr->rad);
	o_ptr->xtra3 = (byte)(naap_ptr->counter_idx);//������0���������������邩�������̂ňꉞ�L�^��������Ƃ�

	o_ptr->ac = (s16b)(naap_ptr->base);//������l�����邩������Ȃ��̂�to_d��ds�ɂ͋L�^���Ȃ��ł����B俎q���Z�ŕ�������Ƃ������ꍇ��...��������ɂ���
	o_ptr->dd = (s16b)(naap_ptr->dice);
	o_ptr->to_a = (s16b)(naap_ptr->sides);
	o_ptr->to_h = (s16b)(naap_ptr->cost);

	o_ptr->xtra4 = (s16b)(naap_ptr->xtra1);
	o_ptr->xtra5 = (s16b)(naap_ptr->xtra2);

	o_ptr->art_name = (s16b)(naap_ptr->quark_idx);//���������X�y�J�����L�^����B����ɂ��X�y���J�[�h�������_���A�[�e�B�t�@�N�g�ɂȂ�B

	return TRUE;
}

//v1.1.56 �^���������t���[�o�[�A�C�e���u�X�y���J�[�h�v�֘A
//�J�[�h�̃p�����[�^���A�C�e������ǂݏo��
bool	read_spellcard_param_from_card(object_type *o_ptr, nameless_arts_activate_param_type *naap_ptr)
{
	int tim_rad;
	if (o_ptr->tval != TV_SPELLCARD)
	{
		msg_format("ERROR:read_spellcard_param_from_card()��TV_SPELLCARD�ȊO���n���ꂽ(%d)", o_ptr->tval); return FALSE;
	}

	if (o_ptr->pval <= 0 || o_ptr->pval > JKF1_MAX)
	{
		msg_format("ERROR:read_spellcard_param_from_card()�ɂ�art_idx����������(%d)", o_ptr->pval); return FALSE;
	}

	naap_ptr->art_idx = o_ptr->pval;
	naap_ptr->gf_type = o_ptr->xtra1;

	//�u���X�̂Ƃ�rad���}�C�i�X�ɂȂ�̂�Y��ĕ����Ȃ�8bit�ɕۑ������̂œǂݏo���Ƃ��ɕϊ���������
	if (o_ptr->xtra2 > 127)
		naap_ptr->rad = o_ptr->xtra2 - 256;
	else
		naap_ptr->rad = o_ptr->xtra2;

	naap_ptr->counter_idx = o_ptr->xtra3;

	naap_ptr->base = o_ptr->ac;
	naap_ptr->dice = o_ptr->dd;
	naap_ptr->sides = o_ptr->to_a;
	naap_ptr->cost = o_ptr->to_h;

	naap_ptr->xtra1 = o_ptr->xtra4;
	naap_ptr->xtra2 = o_ptr->xtra5;

	naap_ptr->quark_idx = o_ptr->art_name;

	return TRUE;

}


/*:::�^�E���������Ŗ����X�y�J���J�����錚���p�R�}���h�B
 *�܂��^�E�����ゾ���Q�[���o�����X�Ƃ��C�ɂ���K�v�͂Ȃ����낤�B
 *�X�y���J�[�h��u�R�}���h����g�p�ł��A�������͏��όn�́u���������Z�v���g�p����B
 *�J�[�h��pval��xtra�Ȃ�object_type���ɔ����ɕK�v�ȃp�����[�^���L�����Ă����B
 */
void	bact_marisa_make_spellcard(void)
{

	int marisa_total_mag_power = 0;
	int mag_type;
	int flag_bonus;
	object_type	forge;
	object_type 	*o_ptr = &forge;
	int price = 10000000;//�ꖇ$1000��
	int k_idx;
	nameless_arts_activate_param_type card_param;

	if (!PLAYER_IS_TRUE_WINNER && !p_ptr->wizard)
	{
		msg_print("ERROR:*�^�E����*���Ă��Ȃ��̂�marisa_make_spellcard()���Ă΂ꂽ");
		return;
	}

	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print("�ו�����t���B");
		clear_bldg(4, 22);
		return;
	}

	screen_save();

	clear_bldg(4, 22);

	flag_bonus = calc_make_spellcard_bonus();

	//��ʂɌ��݂̃p���[���܂��ɕ\�������s�m�F
	if (p_ptr->pclass == CLASS_MARISA)
	{
		prt("���Ȃ�ŋ��̃X�y���J�[�h������C������B", 10, 15);
		if (flag_bonus > 400)
			prt("���͂̔��~���X��قǂ���I", 11, 15);
		else if (flag_bonus > 250)
			prt("���͂̔��~���\�����낤�B", 11, 15);
		else if (flag_bonus > 100)
			prt("���͂̔��~����炩�͂���B", 11, 15);
		else
			prt("���͂̔��~�������S���Ȃ����c�c", 11, 15);
	}
	else
	{
		prt("�������u�ق��A���ɐV�����X�y���J�[�h������Ăق����̂��B", 10, 15);
		prt("�@�ʔ��������B�����󂯂Ă�邺�B", 11, 15);
		if (flag_bonus > 400)
			prt("�@���͂̔��~���X��قǂ��邺�I�v", 12, 15);
		else if (flag_bonus > 250)
			prt("�@���͂̔��~���\�����낤�B�v", 12, 15);
		else if (flag_bonus > 100)
			prt("�@���͂̔��~����炩�͂��邵�ȁB�v", 12, 15);
		else
			prt("�@�ł����͂̔��~�������S���Ȃ��ȁB�v", 12, 15);

	}

	if (!get_check_strict("�ꖇ$1000��������B���܂����H", CHECK_DEFAULT_Y))
	{
		screen_load();
		return;
	}

	if (p_ptr->au < price)
	{
		msg_print("����������Ȃ��B");
		screen_load();
		return;
	}

	k_idx = lookup_kind(TV_SPELLCARD, 0);
	if (!k_idx)
	{
		screen_load();
		return;
	}
	object_prep(o_ptr, k_idx);

	//����IDX����
	card_param.art_idx = make_nameless_arts_sub1_choose_idx(NAMELESS_ART_MODE_SPELLCARD,0,FALSE);
	//��������
	make_nameless_arts_sub2_choose_gf(NAMELESS_ART_MODE_SPELLCARD, &card_param,0,FALSE);
	//�_�C�X�l�Ȃǂ��̑�����
	make_nameless_arts_sub3_apply_other_params(&card_param, NAMELESS_ART_MODE_SPELLCARD,0,FALSE);

	//���\��\���������@���\�\�������J�[�h��ǂރ��[�`���Ƌ��ʉ�����
	while (TRUE)
	{
		char art_desc[255] = "";

		display_nameless_art_effect(&card_param, NAMELESS_ART_MODE_SPELLCARD);

		if (!get_string("���̃X�y�������Ɩ��Â��܂����H:", art_desc, 45)) continue;
		prt(format("����:%s", art_desc), 21, 30);
		if (!get_check_strict("��낵���ł����H�F", CHECK_OKAY_CANCEL))continue;
		card_param.quark_idx = quark_add(art_desc);
		break;
	}


	//�J�[�h��object_type�e�v�f�ɔ����p�����[�^���L�^������
	record_spellcard_param_onto_card(o_ptr, &card_param);
	o_ptr->ident |= (IDENT_MENTAL);

	//������ƃJ�[�h�����n��
	p_ptr->au -= price;
	building_prt_gold();
	inven_carry(o_ptr);

	//���@�X�ɔ��~���ꂽ���͒l���ꗥ20%�������̂Ƃ���
	for (mag_type = 0; mag_type<8; mag_type++)
		marisa_magic_power[mag_type] = marisa_magic_power[mag_type] * 4 / 5;

	clear_bldg(4, 22);
	prt(format("�y%s�z�����������I", quark_str(card_param.quark_idx)), 18, 30);

	inkey();
	screen_load();

}

//�X�y���J�[�h��I�R�}���h��r�R�}���h�œǂ񂾂Ƃ��Ɍ��ʂ�\��
void look_spellcard(object_type *o_ptr)
{
	nameless_arts_activate_param_type card_param;
	int k, wid, hgt;

	if (o_ptr->tval != TV_SPELLCARD) { msg_print("ERROR:look_spellcard()�ɃX�y�J�ȊO���n���ꂽ"); return; }

	if (!read_spellcard_param_from_card(o_ptr, &card_param)) return;

	screen_save();
	Term_get_size(&wid, &hgt);
	for (k = 1; k < hgt; k++) prt("", k, 13);

	display_nameless_art_effect(&card_param, NAMELESS_ART_MODE_SPELLCARD);
	inkey();
	screen_load();

}

//�X�y���J�[�h���g��
void use_spellcard(object_type *o_ptr)
{
	nameless_arts_activate_param_type card_param;
	int k, wid, hgt;

	int dec_mp = 0;
	int dec_hp = 0;

	if (o_ptr->tval != TV_SPELLCARD) { msg_print("ERROR:use_spellcard()�ɃX�y�J�ȊO���n���ꂽ"); return; }

	if(!read_spellcard_param_from_card(o_ptr, &card_param)) return;

	//�����̓s����L�����Z�����Ă��s��������Ă��܂��̂łȂ񂩍s�����Ă���ۂ����b�Z�[�W����Ƃ��B�R�X�g���K�������悤�ɂ���B
	msg_format("���Ȃ��́y%s�z�̃J�[�h���f�����I", quark_str(card_param.quark_idx));


	//�R�X�g����
	dec_mp = card_param.cost;
	if (dec_mp > p_ptr->csp)
	{
		dec_hp = dec_mp - p_ptr->csp;
		dec_mp = p_ptr->csp;
	}

	if (dec_hp > p_ptr->chp)
	{
		msg_print("�������J�[�h���g�������̗͂��Ȃ��B");
		return;
	}
	p_ptr->csp -= dec_mp;
	p_ptr->chp -= dec_hp;
	p_ptr->redraw |= (PR_HP | PR_MANA);

	//�J�[�h���s����
	activate_nameless_art_aux(&card_param, NAMELESS_ART_MODE_SPELLCARD);


}



//���{�ۂ����ʂ̑f�ނɂł���A�C�e���𔻒肷��
static bool item_tester_magatama_material(object_type *o_ptr)
{
	int i;
	for (i = 0; magatama_material_table[i].tval; i++)
	{
		if (o_ptr->tval == magatama_material_table[i].tval && o_ptr->sval == magatama_material_table[i].sval) return TRUE;
	}

	return FALSE;
}

//bact_marisa_make_spellcard()������
//���{�ۂ����Z�u���ʐ���v�Ō��ʂ����
//���ʂ́u�X�y���J�[�h�v�Ɠ��������όn���Z�̔����p�����[�^��ۑ����Ă�������A�C�e��
//�s�����������Ƃ�TRUE
bool make_magatama(void)
{
	int item;
	cptr q, s;
	object_type forge;
	object_type	*o_ptr = &forge;//���ʃA�C�e��
	object_type *mo_ptr;//�f�ރA�C�e��
	int k_idx;
	nameless_arts_activate_param_type card_param;
	int i;

	int material_idx = -1;
	bool boost = FALSE;

	if (p_ptr->pclass != CLASS_MISUMARU) { msg_print("ERROR:���{�ۈȊO��make_magatama()���Ă΂ꂽ"); return FALSE; }
	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print("�ו�����t���B");
		return FALSE;
	}

	//�f�ޑI��
	q = "�u�ǂ̍ޗ��Ō��ʂ����܂��傤�H�v";
	s = "���ʂ̑f�ނɂȂ�A�C�e��������܂���B";
	item_tester_hook = item_tester_magatama_material;
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0)
	{
		mo_ptr = &inventory[item];
	}
	else
	{
		mo_ptr = &o_list[0 - item];
	}

	//�f�ރe�[�u�����ł̑f�ރC���f�b�N�X�����
	for (i = 0; magatama_material_table[i].tval; i++)
	{
		if (mo_ptr->tval == magatama_material_table[i].tval && mo_ptr->sval == magatama_material_table[i].sval)
		{
			material_idx = i;
			break;
		}
	}
	if (material_idx < 0) { msg_print("ERROR:make_magatama()�̑f�ސ����e�[�u������������"); return FALSE; }//paranoia


	msg_print("���Ȃ��͌��ʂ̐���Ɏ��|������...");
	object_prep(o_ptr, lookup_kind(TV_SPELLCARD, SV_SPELLCARD_MAGATAMA));
	//�m���Łu��S�̏o���v�ɂȂ��ē��Z�I�背�x����p���[������
	if (weird_luck() || p_ptr->wizard)
	{
		boost = TRUE;
		msg_print("��S�̏o�����I");
	}

	//����IDX����
	card_param.art_idx = make_nameless_arts_sub1_choose_idx(NAMELESS_ART_MODE_MAGATAMA,material_idx,boost);

	if (!card_param.art_idx)
	{
		msg_print("����Ɏ��s����...");
		return TRUE;
	}

	//��������
	make_nameless_arts_sub2_choose_gf(NAMELESS_ART_MODE_MAGATAMA, &card_param,material_idx,boost);

	//�_�C�X�l�Ȃǂ��̑�����
	make_nameless_arts_sub3_apply_other_params(&card_param, NAMELESS_ART_MODE_MAGATAMA,material_idx,boost);

	//���ʂɍ�����Z�̃^�C�v��З͂Ȃǂ̃p�����[�^���L�^
	record_spellcard_param_onto_card(o_ptr, &card_param);
	o_ptr->ident |= (IDENT_MENTAL);

	//���ʂ̓X�y���J�[�h�ƈႢ�����������N���A���A����ɑf��k_idx���L�^
	o_ptr->art_name = 0;
	o_ptr->xtra6 = lookup_kind(mo_ptr->tval,mo_ptr->sval);

	//�f�ޏ���
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


	msg_print("���ʂ����������I");

	screen_save();

	display_nameless_art_effect(&card_param, NAMELESS_ART_MODE_MAGATAMA);

	inkey();

	screen_load();

	inven_carry(o_ptr);
	return TRUE;
}

//���ʂ̈�s�ȗ����ʕ\��
void magatama_effect_desc(char *desc, object_type *o_ptr)
{

	nameless_arts_activate_param_type card_param;
	cptr desc1, desc2;

	if (o_ptr->tval != TV_SPELLCARD || o_ptr->sval != SV_SPELLCARD_MAGATAMA)
	{
		sprintf(desc, "(ERROR)");
		return;
	}
	if (!o_ptr->pval)
	{
		sprintf(desc, "(���s��)");
		return;
	}

	read_spellcard_param_from_card(o_ptr, &card_param);

	desc1 = nameless_skill_type_desc[card_param.art_idx];
	if (card_param.gf_type)
	{
		desc2 = gf_desc_name[card_param.gf_type];
		sprintf(desc, "�y%s(%s)�z", desc1, desc2);
	}
	else
	{
		sprintf(desc, "�y%s�z", desc1);
	}


}

//v2.0.4
//���{�ۂ̌��ʓ��Z�g�p���[�`��
//���ς�俎q�ƈႢmagic_num1[]�łȂ�inven_add[]�ɑ����������ʂ���X�y���J�[�h�Ɠ��l�ɋZ�p�����[�^��ǂݎ���ă��X�g����B
//����ȊO��activate_nameless_arts()�Ƃقړ����B���X�g�������ƋZ�������ɂ��ꂼ��p�����[�^��ǂݎ�蒼���̂͏璷�ȋC������̂ŋZ�p�����[�^��z��ɂ���
//�s���������Ƃ�TRUE��Ԃ�
bool	activate_magatama(bool only_info)
{

	int num, i;
	int list_num = 0;
	int dir;
	bool flag_choose = FALSE;
	nameless_arts_activate_param_type naap_lis[INVEN2_MAGATAMA_NUM_MAX];
	int inven2_idx_lis[INVEN2_MAGATAMA_NUM_MAX];

	if (p_ptr->pclass != CLASS_MISUMARU)
	{
		msg_print("ERROR:activate_magatama()�����{�ۈȊO�ŌĂ΂ꂽ");
		return FALSE;
	}

	//�Z���X�g�̐��� �ǉ��C���x���g���̋󗓂͋l�߂� 
	for (i = 0; i<INVEN2_MAGATAMA_NUM_MAX; i++)
	{
		naap_lis[i].art_idx = 0;
	}
	for (i = 0; i<INVEN2_MAGATAMA_NUM_MAX; i++)
	{
		object_type *o_ptr = &inven_add[i];

		if (!o_ptr->k_idx || o_ptr->tval != TV_SPELLCARD || o_ptr->sval != SV_SPELLCARD_MAGATAMA) continue;
		if (!read_spellcard_param_from_card(o_ptr, &naap_lis[list_num])) return FALSE;
		inven2_idx_lis[list_num] = i;//object_desc�̂Ƃ�������xinven_add�ɃA�N�Z�X����̂ł��̋Z�̃A�C�e���̏ꏊ��ێ�
		list_num++;

	}
	if (!list_num)
	{
		msg_print("���ʂ𑕔����Ă��Ȃ��B");
		return FALSE;
	}

	//ALLOW_REPEAT���s�[�g����
	if (repeat_pull(&num)) flag_choose = TRUE;

	//�Z�I�����[�v
	screen_save();
	while (!flag_choose)
	{
		char c;
		num = -1;

		//��ʃN���A
		for (i = 1; i<16; i++) Term_erase(17, i, 255);

		if (only_info)
			c_prt(TERM_WHITE, "�ǂ̏����m�F���܂����H", 3, 20);
		else
			c_prt(TERM_WHITE, "�ǂ���g���܂����H", 3, 20);

		c_prt(TERM_WHITE, "                                            �g�pMP ����", 4, 20);
		//���X�g�\��
		for (i = 0; i<list_num; i++)
		{
			byte color;
			char tmp_desc[64];
			char tmp_desc2[64];
			char o_name[MAX_NLEN];

			if (!naap_lis[i].art_idx) break;//�����̋Z������Ă��Ȃ�

			object_desc(o_name, &inven_add[inven2_idx_lis[i]], 0);

			if (p_ptr->csp < naap_lis[i].cost) color = TERM_L_DARK;//MP�s��
			else  color = TERM_WHITE;

			if (naap_lis[i].gf_type && naap_lis[i].art_idx != JKF1_ADD_ESSENCE1 && naap_lis[i].art_idx != JKF1_ADD_ESSENCE2)
				sprintf(tmp_desc2, "[%s]", gf_desc_name[naap_lis[i].gf_type]);
			else
				sprintf(tmp_desc2, "");

			if (naap_lis[i].gf_type == GF_MAKE_TREE || naap_lis[i].gf_type == GF_STONE_WALL || naap_lis[i].gf_type == GF_MAKE_DOOR)
				sprintf(tmp_desc, "");
			else if (naap_lis[i].art_idx == JKF1_ATTACK_BREATH) //�u���X��xtra1�̒l�ňЗ͂��ς��
				sprintf(tmp_desc, "(%d)", p_ptr->chp * naap_lis[i].xtra1 / 100);
			else if (naap_lis[i].base < 1 && (naap_lis[i].dice < 1 || naap_lis[i].sides < 1))
				sprintf(tmp_desc, "");
			else if (naap_lis[i].base < 1)
				sprintf(tmp_desc, "(%dd%d)", naap_lis[i].dice, naap_lis[i].sides);
			else if (naap_lis[i].dice < 1 || naap_lis[i].sides < 1)
				sprintf(tmp_desc, "(%d)", naap_lis[i].base);
			else
				sprintf(tmp_desc, "(%d+%dd%d)", naap_lis[i].base, naap_lis[i].dice, naap_lis[i].sides);

			c_prt(color, format("%c) %-45s %-4d %s%s%s", ('a' + i), o_name,
				naap_lis[i].cost, tmp_desc2, tmp_desc, ((naap_lis[i].art_idx == JKF1_ATTACK_AROUND) ? "/2" : "")), 5 + i, 17);

		}

		c = inkey();

		if (c == ESCAPE)
		{
			break;
		}

		if (c >= 'a' && c <= ('a' + list_num - 1))
		{
			//�Z���Ȃ��Ƃ��������I�������f���ă��[�v����o��
			num = c - 'a';
			if (!only_info) flag_choose = TRUE;
			repeat_push(num);
		}

		//only_info�̂Ƃ���ʂɏ�񂾂��\�����ă��[�v���s ESC�ŏI��
		if (only_info && num >= 0)
		{
			display_nameless_art_effect(&naap_lis[num], NAMELESS_ART_MODE_SPELLCARD);
			continue;
		}

	}
	screen_load();

	if (num < 0) return FALSE; //ESC�͏I��


	if (cheat_xtra)
	{
		msg_format("name:%s", quark_str(naap_lis[num].quark_idx));
		msg_format("idx:%d GF:%d", naap_lis[num].art_idx, naap_lis[num].gf_type);
		msg_format("base:%d,dice:%d,sides:%d", naap_lis[num].base, naap_lis[num].dice, naap_lis[num].sides);
		msg_format("counter_idx:%d", naap_lis[num].counter_idx);
		msg_format("xtra:%d/%d", naap_lis[num].xtra1, naap_lis[num].xtra2);
	}


	if (p_ptr->csp < naap_lis[num].cost)
	{
		msg_print("MP������Ȃ��B");
		return FALSE;
	}

	//���Z���s
	if (!activate_nameless_art_aux(&naap_lis[num], NAMELESS_ART_MODE_MAGATAMA)) return FALSE;

	//MP����
	p_ptr->csp -= naap_lis[num].cost;
	p_ptr->redraw |= (PR_HP | PR_MANA);

	return TRUE;

}

