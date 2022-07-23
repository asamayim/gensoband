/* File: object1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Object code, part 1 */

#include "angband.h"

#if defined(MACINTOSH) || defined(MACH_O_CARBON)
#ifdef verify
#undef verify
#endif
#endif
/*
 * Reset the "visual" lists
 *
 * This involves resetting various things to their "default" state.
 *
 * If the "prefs" flag is TRUE, then we will also load the appropriate
 * "user pref file" based on the current setting of the "use_graphics"
 * flag.  This is useful for switching "graphics" on/off.
 *
 * The features, objects, and monsters, should all be encoded in the
 * relevant "font.pref" and/or "graf.prf" files.  XXX XXX XXX
 *
 * The "prefs" parameter is no longer meaningful.  XXX XXX XXX
 */
/*:::��ʂ̕\���ݒ�������l�ɖ߂��炵��*/
//�^�C���̎g�p��g�p��؂�ւ����Ƃ����Ă΂�Ax_attr/x_char���f�t�H���g�l����^�C��bmp�t�@�C�����ʒu�ɕς����肵�Ă�
void reset_visuals(void)
{
	int i, j;

	/* Extract some info about terrain features */
	for (i = 0; i < max_f_idx; i++)
	{
		feature_type *f_ptr = &f_info[i];

		/* Assume we will use the underlying values */
		for (j = 0; j < F_LIT_MAX; j++)
		{
			f_ptr->x_attr[j] = f_ptr->d_attr[j];
			f_ptr->x_char[j] = f_ptr->d_char[j];
		}
	}

	/* Extract default attr/char code for objects */
	for (i = 0; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Default attr/char */
		k_ptr->x_attr = k_ptr->d_attr;
		k_ptr->x_char = k_ptr->d_char;
	}

	/* Extract default attr/char code for monsters */
	for (i = 0; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Default attr/char */
		r_ptr->x_attr = r_ptr->d_attr;
		r_ptr->x_char = r_ptr->d_char;
	}

	if (use_graphics)
	{
		char buf[1024];

		/* Process "graf.prf" */
		process_pref_file("graf.prf");

		/* Access the "character" pref file */
		sprintf(buf, "graf-%s.prf", player_base);

		/* Process "graf-<playername>.prf" */
		process_pref_file(buf);
	}

	/* Normal symbols */
	else
	{
		char buf[1024];

		/* Process "font.prf" */
		process_pref_file("font.prf");

		/* Access the "character" pref file */
		sprintf(buf, "font-%s.prf", player_base);

		/* Process "font-<playername>.prf" */
		process_pref_file(buf);
	}
}


/*
 * Obtain the "flags" for an item
 */
/*:::�����i�̑ϐ��A�\�͂������t���O��k_info,a_info,e_info,art_flags����S�Ĉ�ӏ���flags�ɓ���*/
/*:::flags��u32b ??[4]�łȂ��Ƃ����Ȃ�*/
///item res ego
void object_flags(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE])
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];
	int i;

	/* Base object */
	/*:::k_info[]�̃t���O*/
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = k_ptr->flags[i];

	/* Artifact */
	/*:::���̃t���O�@�x�[�X�A�C�e���̃t���O�͖�������*/
	if (object_is_fixed_artifact(o_ptr))
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] = a_ptr->flags[i];
	}

	/* Ego-item */
	/*:::�G�S�A�C�e���̃t���O�@�x�[�X�A�C�e���̃t���O�ɒǉ�����*/
	/*:::�R���؂�̃G�S��������͌��ʂ����O����*/
	if (object_is_ego(o_ptr))
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= e_ptr->flags[i];

		if ((o_ptr->name2 == EGO_LITE_ILLUMINATION) && !o_ptr->xtra4 && (o_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_SH_FIRE);
			remove_flag(flgs, TR_RES_FIRE);
			remove_flag(flgs, TR_RES_LITE);

		}
		else if ((o_ptr->name2 == EGO_LITE_BEAST) && !o_ptr->xtra4 && (o_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_INFRA);
		}
		else if ((o_ptr->name2 == EGO_LITE_IMMORTAL) && !o_ptr->xtra4 && (o_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_RES_BLIND);
			remove_flag(flgs, TR_SEE_INVIS);
		}
	}

	/* Random artifact ! */
	/*:::�����_���A�[�e�B�t�@�N�g��A�[�e�B�t�@�N�g�E�G�S�A�C�e���ǉ��̃t���O�@*/
	/*:::������G�S�̒ǉ��\�͂��܂܂�Ă���̂Ł��̂Ƃ��������������悤���򂷂邱�Ƃ͂��Ȃ�*/
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] |= o_ptr->art_flags[i];

	/*:::�b��t�쐬�̃A�C�e���̏���*/
	if (object_is_smith(o_ptr))
	{
		int add = o_ptr->xtra3 - 1;

		//����C���i�A�d��A����Ƃ��i�Ȃǂ͓����xtra3�����邪�����ł͉�������Ȃ�
		if (add < TR_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
		else if (add == ESSENCE_TMP_RES_ACID)
		{
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_ACTIVATE);
		}
		else if (add == ESSENCE_TMP_RES_ELEC)
		{
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_ACTIVATE);
		}
		else if (add == ESSENCE_TMP_RES_FIRE)
		{
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_ACTIVATE);
		}
		else if (add == ESSENCE_TMP_RES_COLD)
		{
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_ACTIVATE);
		}
		else if (add == ESSENCE_TMP_RES_ALL) //v1.1.74
		{
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_ACTIVATE);
		}
		else if (add == ESSENCE_WHIRL_WIND
			|| add == ESSENCE_PSY_SPEAR
			|| add == ESSENCE_L_RANGE_ATTACK
			|| add == ESSENCE_HIT_AND_AWAY
			|| add == ESSENCE_HEAL_500
			|| add == ESSENCE_INVULN
			|| add == ESSENCE_SS_ARROW
			|| add == ESSENCE_SUPER_STEALTH
			|| add == ESSENCE_DIM_DOOR
			|| add == ESSENCE_DUAL_CAST
			|| add == ESSENCE_TIM_INC_DICE
			)
		{
			add_flag(flgs, TR_ACTIVATE);
		}
		else if (add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_SH_FIRE);
		}
		else if (add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_SH_ELEC);
		}
		else if (add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_SH_COLD);
		}
		else if (add == ESSENCE_RESISTANCE)
		{
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
		}
		/*:::�����n�k�H���ꂾ�����̃G�b�Z���X�ނƒ�`�ꏊ���Ⴄ�悤����*/
		else if (add == TR_IMPACT)
		{
			add_flag(flgs, TR_ACTIVATE);
		}
	}

	//���̖�Ƃ��ɂ��􂢖���������
	if(p_ptr->pclass == CLASS_HINA && o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER)
	{
		remove_flag(flgs, TR_NO_TELE);
		remove_flag(flgs, TR_NO_MAGIC);
		remove_flag(flgs, TR_TY_CURSE);
		remove_flag(flgs, TR_AGGRAVATE);
		remove_flag(flgs, TR_DRAIN_EXP);
		remove_flag(flgs, TR_ADD_L_CURSE);
		remove_flag(flgs, TR_ADD_H_CURSE);

	}


}



/*
 * Obtain the "flags" for an item which are known to the player
 */
void object_flags_known(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE])
{
	bool spoil = FALSE;
	int i;

	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0;

	if (!object_is_aware(o_ptr)) return;

	/* Base object */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = k_ptr->flags[i];

	/* Must be identified */
	if (!object_is_known(o_ptr)) return;

	/* Ego-item (known basic flags) */
	if (object_is_ego(o_ptr))
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= e_ptr->flags[i];

		if ((o_ptr->name2 == EGO_LITE_ILLUMINATION) && !o_ptr->xtra4 && (o_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_SH_FIRE);
		}
		else if ((o_ptr->name2 == EGO_LITE_BEAST) && !o_ptr->xtra4 && (o_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_INFRA);
		}
		else if ((o_ptr->name2 == EGO_LITE_IMMORTAL) && !o_ptr->xtra4 && (o_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_RES_BLIND);
			remove_flag(flgs, TR_SEE_INVIS);
		}
	}


#ifdef SPOIL_ARTIFACTS
	/* Full knowledge for some artifacts */
	if (object_is_artifact(o_ptr)) spoil = TRUE;
#endif /* SPOIL_ARTIFACTS */

#ifdef SPOIL_EGO_ITEMS
	/* Full knowledge for some ego-items */
	if (object_is_ego(o_ptr)) spoil = TRUE;
#endif /* SPOIL_EGO_ITEMS */

	/* Need full knowledge or spoilers */
	if (spoil || (o_ptr->ident & IDENT_MENTAL))
	{
		/* Artifact */
		if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			for (i = 0; i < TR_FLAG_SIZE; i++)
				flgs[i] = a_ptr->flags[i];

			//v1.1.34
			if(o_ptr->name1 == ART_KUKAI && check_equip_specific_fixed_art(ART_KUKAI_KASA,TRUE))
			{
				add_flag(flgs, TR_EASY_SPELL);
			}
		}

		/* Random artifact ! */
		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= o_ptr->art_flags[i];
	}

	if (object_is_smith(o_ptr))
	{
		int add = o_ptr->xtra3 - 1;

		if (add < TR_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
		else if (add == ESSENCE_TMP_RES_ACID)
		{
			add_flag(flgs, TR_RES_ACID);
		}
		else if (add == ESSENCE_TMP_RES_ELEC)
		{
			add_flag(flgs, TR_RES_ELEC);
		}
		else if (add == ESSENCE_TMP_RES_FIRE)
		{
			add_flag(flgs, TR_RES_FIRE);
		}
		else if (add == ESSENCE_TMP_RES_COLD)
		{
			add_flag(flgs, TR_RES_COLD);
		}
		else if (add == ESSENCE_TMP_RES_ALL) //v1.1.74
		{
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_POIS);
		}
		else if (add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_SH_FIRE);
		}
		else if (add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_SH_ELEC);
		}
		else if (add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_SH_COLD);
		}
		else if (add == ESSENCE_RESISTANCE)
		{
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
		}
	}

	//���̖�Ƃ��ɂ��􂢖���������
	if(p_ptr->pclass == CLASS_HINA && o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER)
	{
		remove_flag(flgs, TR_NO_TELE);
		remove_flag(flgs, TR_NO_MAGIC);
		remove_flag(flgs, TR_TY_CURSE);
		remove_flag(flgs, TR_AGGRAVATE);
		remove_flag(flgs, TR_DRAIN_EXP);
		remove_flag(flgs, TR_ADD_L_CURSE);
		remove_flag(flgs, TR_ADD_H_CURSE);

	}

}


static cptr item_activation_dragon_breath(object_type *o_ptr)
{
	static char desc[256];
	u32b flgs[TR_FLAG_SIZE]; /* for resistance flags */
	int i, n = 0;

	object_flags(o_ptr, flgs);
	strcpy(desc, _("", "breath "));

	for (i = 0; dragonbreath_info[i].flag != 0; i++)
	{
		if (have_flag(flgs, dragonbreath_info[i].flag))
		{
			if (n > 0) strcat(desc, _("�A", ", "));
			strcat(desc, dragonbreath_info[i].name);
			n++;
		}
	}

	strcat(desc, _("�̃u���X(250)", ""));

	return (desc);
}
/*:::�����i�̔������ʂ��L�q����*/
static cptr item_activation_aux(object_type *o_ptr)
{
	static char activation_detail[256];
	cptr desc;
	char timeout[32];
	int constant, dice;
	const activation_type* const act_ptr = find_activation_info(o_ptr);

	if (!act_ptr) return _("����`", "something undefined");

	desc = act_ptr->desc;

	///mod160503 �e����ˌ�
	if(act_ptr->index == ACT_GUN_VARIABLE)
	{
		cptr gf_desc;
		cptr project_desc;
		//v1.1.31 �o�O�Ή��@�}�b�h�T�C�G���e�B�X�g�G�S�ŕs���Ȓl�������Ă���\��������
		gf_desc = gf_desc_name[(o_ptr->xtra1 >= MAX_GF) ? 0: o_ptr->xtra1];

		switch(o_ptr->xtra4)
		{
		case GUN_FIRE_MODE_BOLT:
			project_desc = "�{���g";break;

		case GUN_FIRE_MODE_BALL:
			project_desc = "�{�[��";break;
		case GUN_FIRE_MODE_BEAM:
			project_desc = "�r�[��";break;
		case GUN_FIRE_MODE_ROCKET:
			project_desc = "���P�b�g";break;
		case GUN_FIRE_MODE_BREATH:
			project_desc = "���ˍU��";break;
		case GUN_FIRE_MODE_SPARK:
			project_desc = "��^�r�[��";break;
		default:
			project_desc = "�ˌ�";break;
		}

		sprintf(timeout, "%d�^�[����(�e�n���x�ő���)",gun_base_param_table[o_ptr->sval].charge_turn);

		if(o_ptr->xtra1 == GF_ARROW)
			sprintf(activation_detail, "(%dd%d%s%d)�_���[�W��%s : %s",o_ptr->dd,o_ptr->ds,((o_ptr->to_d < 0)? "":"+"),o_ptr->to_d, project_desc,timeout);
		else if(o_ptr->xtra1 == GF_ROCKET)
			sprintf(activation_detail, "(%dd%d%s%d)�_���[�W��%s : %s",o_ptr->dd,o_ptr->ds,((o_ptr->to_d < 0)? "":"+"),o_ptr->to_d, project_desc,timeout);
		else
			sprintf(activation_detail, "(%dd%d%s%d)�_���[�W��%s����%s : %s",o_ptr->dd,o_ptr->ds,((o_ptr->to_d < 0)? "":"+"),o_ptr->to_d, gf_desc, project_desc,timeout);
		return activation_detail;
	}

	/* Overwrite description if it is special */
	///item �Ή��̎w�ցA�h���S���A�[�}�[�A���q�؂Ȃǂ̔����L�q�̓��ꏈ��
	switch (act_ptr->index) {
	case ACT_BR_FIRE:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
			desc = _("�Ή��̃u���X (200) �Ɖ΂ւ̑ϐ�", "breath of fire (200) and resist fire");
		break;
	case ACT_BR_COLD:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
			desc = _("��C�̃u���X (200) �Ɨ�C�ւ̑ϐ�", "breath of cold (200) and resist cold");
		break;
	case ACT_BR_DRAGON:
		desc = item_activation_dragon_breath(o_ptr);
		break;
	case ACT_AGGRAVATE:
		if (o_ptr->name1 == ART_HYOUSIGI)
			desc = _("���q�؂�ł��Ȃ炷", "beat wooden clappers");
		break;
	case ACT_RESIST_ACID:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ACID))
			desc = _("�A�V�b�h�E�{�[�� (100) �Ǝ_�ւ̑ϐ�", "ball of acid (100) and resist acid");
		break;
	case ACT_RESIST_FIRE:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
			desc = _("�t�@�C�A�E�{�[�� (100) �Ɖ΂ւ̑ϐ�", "ball of fire (100) and resist fire");
		break;
	case ACT_RESIST_COLD:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
			desc = _("�A�C�X�E�{�[�� (100) �Ɨ�C�ւ̑ϐ�", "ball of cold (100) and resist cold");
		break;
	case ACT_RESIST_ELEC:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ELEC))
			desc = _("�T���_�[�E�{�[�� (100) �Ɠd���ւ̑ϐ�", "ball of elec (100) and resist elec");
		break;
	}

	/* Timeout description */
	/*:::�����\�Ԋu�Ɋւ���L�q*/
	///item �����\�Ԋu�̋L�q
	constant = act_ptr->timeout.constant;
	dice = act_ptr->timeout.dice;
	if (constant == 0 && dice == 0) {
		/* We can activate it every turn */
		strcpy(timeout, _("���ł�", "every turn"));
	} else if (constant < 0) {
		/* Activations that have special timeout */
		switch (act_ptr->index) {
		case ACT_BR_FIRE:
			sprintf(timeout, _("%d �^�[����", "every %d turns"),
				((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES)) ? 200 : 250);
			break;
		case ACT_BR_COLD:
			sprintf(timeout, _("%d �^�[����", "every %d turns"),
				((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE)) ? 200 : 250);
			break;
		case ACT_TERROR:
			strcpy(timeout, _("3*(���x��+10) �^�[����", "every 3 * (level+10) turns"));
			break;
		case ACT_MURAMASA:
			strcpy(timeout, _("�m��50%�ŉ���", "(destroyed 50%)"));
			break;
		default:
			strcpy(timeout, "undefined");
			break;
		}
	} else {
		/* Normal timeout activations */
		char constant_str[16], dice_str[16];
		sprintf(constant_str, "%d", constant);
		sprintf(dice_str, "1d%d", dice);
		sprintf(timeout, _("%s%s%s �^�[����", "every %s%s%s turns"),
			(constant > 0) ? constant_str : "",
			(constant > 0 && dice > 0) ? "+" : "",
			(dice > 0) ? dice_str : "");
	}

	/* Build detail activate description */
	sprintf(activation_detail, _("%s : %s", "%s %s"), desc, timeout);

	return activation_detail;
}

/*
 * Determine the "Activation" (if any) for an artifact
 * Return a string, or NULL for "no activation"
 */
cptr item_activation(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Require activation ability */
#ifdef JP
	if (!(have_flag(flgs, TR_ACTIVATE))) return ("�Ȃ�");
#else
	if (!(have_flag(flgs, TR_ACTIVATE))) return ("nothing");
#endif

	/* Get an explain of an activation */
	if (activation_index(o_ptr))
	{
		return item_activation_aux(o_ptr);
	}

	///item �J�A�����X�^�[�{�[���Ɋւ������L�q
	/* Special items */
	if (o_ptr->tval == TV_WHISTLE)
	{
#ifdef JP
return "�y�b�g�Ăъ� : 100+d100�^�[����";
#else
		return "call pet every 100+d100 turns";
#endif
	}

	if (o_ptr->tval == TV_CAPTURE)
	{
#ifdef JP
return "�����X�^�[��߂���A���͉������B";
#else
		return "captures or releases a monster.";
#endif
	}

	/* Oops */
#ifdef JP
	return "�����N���Ȃ�";
#else
	return "Nothing";
#endif
}


/*
 * Describe a "fully identified" item
 */
/*:::��ʂɎw�肵���A�C�e����*�Ӓ�*����\������*/
///item res �A�C�e���ڍ׏��\���@�d�v
///mod131223 TVAL�֘A
bool screen_object(object_type *o_ptr, u32b mode)
{
	int                     i = 0, j, k;

	u32b flgs[TR_FLAG_SIZE];

	char temp[70 * 20];
	char temp2[70 * 20];
	cptr            info[128];
	char o_name[MAX_NLEN];
	int wid, hgt;

	int trivial_info = 0;

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	//v1.1.56
	if (o_ptr->tval == TV_SPELLCARD)
	{
		look_spellcard(o_ptr);
		return TRUE;
	}
	//�V�� 
	///pend ������t���[�o�[�e�L�X�g�Ƃ��ǉ�������
	else if(o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI)
	{
		sprintf(temp,"�����%s�ɂ��Ă̋L����������Ă���B",r_name + r_info[o_ptr->pval].name);
		info[i++]=temp;

		//info[i++] = format("�����%s�ɂ��Ă̋L����������Ă���B",r_name + r_info[o_ptr->pval].name);
		//info[i++] = "�����%s�ɂ��Ă̋L����������Ă���B";
	}

	/* Extract the description */
	/*:::a_text�ɂ͑S�A�[�e�B�t�@�N�g����������ƕ���ł���A
	 *:::artifact_type.text�ɂ�a_text�̐擪����̃o�C�g�����i�[����Ă���B
	 *:::�������當����𔲂��o���Ă���֐��B
	 */
	else
	{
		roff_to_buf(o_ptr->name1 ? (a_text + a_info[o_ptr->name1].text) :
			    (k_text + k_info[o_ptr->k_idx].text),
			    77 - 15, temp, sizeof(temp));
		for (j = 0; temp[j]; j += 1 + strlen(&temp[j]))
		{ info[i] = &temp[j]; i++;}
	}

	///mod160218 �A�C�e���J�[�h��*�Ӓ�*����Ɛ��������o��B
	if(o_ptr->tval == TV_ITEMCARD)
	{

		roff_to_buf((support_item_list[o_ptr->pval].infotxt),  77 - 15, temp2, sizeof(temp2));
		for (j = 0; temp2[j]; j += 1 + strlen(&temp2[j]))
		{ info[i] = &temp2[j]; i++;}
	}
	//v1.1.86 �A�r���e�B�J�[�h
	if (o_ptr->tval == TV_ABILITY_CARD)
	{

		roff_to_buf(use_ability_card_aux(o_ptr,TRUE),77 - 15, temp2, sizeof(temp2));
		for (j = 0; temp2[j]; j += 1 + strlen(&temp2[j]))
		{
			info[i] = &temp2[j]; i++;
		}
	}



	else if (object_is_equipment(o_ptr))
	{
		/* Descriptions of a basic equipment is just a flavor */
		trivial_info = i;
	}

	//v1.1.60 TV_ANTIQUE�̃A�C�e���̃t���O�L�q
	if (o_ptr->tval == TV_ANTIQUE)
	{
		if (have_flag(flgs, TR_STR))info[i++] = "����͌Ñ�̔M�������v���N�������锗�͂�����B";
		if (have_flag(flgs, TR_INT))info[i++] = "����͕����̖G�������������������ꂽ���肾�B";
		if (have_flag(flgs, TR_WIS))info[i++] = "����͌�����̂̋����܂��𐳂����鑑���ȕ��͋C�������o���B";
		if (have_flag(flgs, TR_DEX))info[i++] = "����͌Ñ�̏��̋Z���Â΂��鐸�k�ȑ��肾�B";
		if (have_flag(flgs, TR_CON))info[i++] = "����͉i�������o�ĂȂ��S���F�򂹂Ă��Ȃ��B";
		if (have_flag(flgs, TR_CHR))info[i++] = "����͌���҂̍���D���悤�Ȕ��������B";

		if (have_flag(flgs, TR_MAGIC_MASTERY))info[i++] = "����ɏ[�삷�閂�͂��W���P��������Ă���B";
		if (have_flag(flgs, TR_SEARCH))info[i++] = "����͗��j�w�Ɉ�΂𓊂���悤�ȋ����ׂ��\�������Ă���B";
		if (have_flag(flgs, TR_SPEED))info[i++] = "����͊���N���̍Ό����{�������Ñ�̕i���B";

		if (have_flag(flgs, TR_ESP_EVIL))info[i++] = "����͉����ЁX�������݂𘺂߂����ӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_GOOD))info[i++] = "����͉����P���������݂��̗g����ӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_ANIMAL))info[i++] = "����͍r�X�����b���ۂ����ӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_HUMAN))info[i++] = "����͌Ñ�̉p�Y���`����Ă���B";
		if (have_flag(flgs, TR_ESP_UNDEAD))info[i++] = "����͎������̑�ȉ����`����Ă���B";
		if (have_flag(flgs, TR_ESP_DRAGON))info[i++] = "����͌����ȗ��̈ӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_DEITY))info[i++] = "����͌����̐_�X���ۂ����ӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_DEMON))info[i++] = "����ɂ͋��낵���א_�̈ӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_KWAI))info[i++] = "����͐l�X�����������ق��ۂ����ӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_NONLIVING))info[i++] = "����͉���璊�ۓI�Ȉӏ����{����Ă���B";
		if (have_flag(flgs, TR_ESP_UNIQUE))info[i++] = "����͌`�e�����������d�s�v�c�ȃf�U�C�����B";

		if (have_flag(flgs, TR_EASY_SPELL) || have_flag(flgs, TR_DEC_MANA))info[i++] = "����ɂ͓�̋P�����[�����т�����ƒ��荞�܂�Ă���B";

		if (have_flag(flgs, TR_REFLECT))info[i++] = "����͂܂�ŋ��̂悤�Ɋ��炩�Ȏ������B";
		if (have_flag(flgs, TR_IM_ACID))info[i++] = "����͈�؂̕��I���񂹕t���Ȃ���̕����ō���Ă���B";
		if (have_flag(flgs, TR_IM_FIRE))info[i++] = "����͔R����悤�ȋP��������Ă���B";
		if (have_flag(flgs, TR_IM_ELEC))info[i++] = "����͏�ɋ��͂ȓd���g������Ă���B";
		if (have_flag(flgs, TR_IM_COLD))info[i++] = "����͐G��Ƌ��낵���₽���B";

		if (have_flag(flgs, TR_LITE))info[i++] = "�����ῂ��΂���̋P��������Ă���B";
		if (have_flag(flgs, TR_BLESSED))info[i++] = "����͐_�ɏj������Ă���B";

		if (have_flag(flgs, TR_DISARM))info[i++] = "����͌�����̂������悤�ȕ��G�ȕ��l�����܂�Ă���B";
		if (have_flag(flgs, TR_SAVING))info[i++] = "���������ƂȂɂ��Ɏ���Ă���悤�ȋC�ɂȂ��Ă���B";


	}
	//TV_ANTIQUE�ȊO�̃A�C�e���̃t���O�L�q
	else
	{

		/* Mega-Hack -- describe activation */
		if (have_flag(flgs, TR_ACTIVATE))
		{
	#ifdef JP
	info[i++] = "�n�������Ƃ��̌���...";
	#else
			info[i++] = "It can be activated for...";
	#endif

			info[i++] = item_activation(o_ptr);
	#ifdef JP
	info[i++] = "...�������������Ă��Ȃ���΂Ȃ�Ȃ��B";
	#else
			info[i++] = "...if it is being worn.";
	#endif

		}
		/* Figurines, a hack */
		if (o_ptr->tval == TV_FIGURINE)
		{
	#ifdef JP
	info[i++] = "����͓��������z���ɂȂ�B";
	#else
			info[i++] = "It will transform into a pet when thrown.";
	#endif

		}

		/* Figurines, a hack */
		if (o_ptr->name1 == ART_STONEMASK)
		{
	#ifdef JP
	info[i++] = "����𑕔������҂͋z���S�ɂȂ�B";
	#else
			info[i++] = "It makes you turn into a vampire permanently.";
	#endif

		}

		if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI))
		{
	#ifdef JP
	info[i++] = "����͑�����ꌂ�œ|�����Ƃ�����B";
	#else
			info[i++] = "It will attempt to kill a monster instantly.";
	#endif

		}

		//v1.1.48 �n�R�_�G�S�̕�
		if (o_ptr->name2 == EGO_BODY_POVERTY)
		{
			info[i++] = "����͋��^��ቺ������B";

		}



	/*
		if ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE))
		{
	#ifdef JP
	info[i++] = "����͎������g�ɍU�����Ԃ��Ă��邱�Ƃ�����B";
	#else
			info[i++] = "It causes you to strike yourself sometimes.";
	#endif

	#ifdef JP
	info[i++] = "����͖��G�̃o���A��؂�􂭁B";
	#else
			info[i++] = "It always penetrates invulnerability barriers.";
	#endif
		}
	*/
		///item ego ������GENZI�G�S�ɓ�������\��
		//if (o_ptr->name2 == EGO_HANDS_MUSOU || have_flag(flgs, TR_GENZI))
		if (have_flag(flgs, TR_GENZI))
		{
	#ifdef JP
	info[i++] = "����͓񓁗��ł̖����������コ����B";
	#else
			info[i++] = "It affects your ability to hit when you are wielding two weapons.";
	#endif

		}

		if (have_flag(flgs, TR_EASY_SPELL))
		{
	#ifdef JP
	info[i++] = "����͖��@�̓�Փx��������B";
	#else
			info[i++] = "It affects your ability to cast spells.";
	#endif
		}
	/*
		if (o_ptr->name2 == EGO_AMU_FOOL)
		{
	#ifdef JP
	info[i++] = "����͖��@�̓�Փx���グ��B";
	#else
			info[i++] = "It interferes with casting spells.";
	#endif
		}
	*/
		//if (o_ptr->name2 == EGO_RING_THROW)
		if (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_MIGHTY_THROW)
		{
	#ifdef JP
	info[i++] = "����͕������������邱�Ƃ��\�ɂ���B";
	#else
			info[i++] = "It provides great strength when you throw an item.";
	#endif
		}
	/*
		if (o_ptr->name2 == EGO_AMU_NAIVETY)
		{
	#ifdef JP
	info[i++] = "����͖��@��R�͂�������B";
	#else
			info[i++] = "It decreases your magic resistance.";
	#endif
		}
	*/
	///del131221 ��
	#if 0
		if (o_ptr->tval == TV_STATUE)
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];

			if (o_ptr->pval == MON_BULLGATES)
	#ifdef JP
				info[i++] = "����͕����ɏ���ƒp���������B";
	#else
				info[i++] = "It is shameful.";
	#endif
			else if ( r_ptr->flags2 & (RF2_ELDRITCH_HORROR))
	#ifdef JP
				info[i++] = "����͕����ɏ���Ƌ����B";
	#else
			info[i++] = "It is fearful.";
	#endif
			else
	#ifdef JP
				info[i++] = "����͕����ɏ���Ɗy�����B";
	#else
			info[i++] = "It is cheerful.";
	#endif
		}
	#endif	

		/* Hack -- describe lite's */
		if (o_ptr->tval == TV_LITE)
		{
			if (o_ptr->name2 == EGO_LITE_DARKNESS)
			{
	#ifdef JP
				info[i++] = "����͑S������Ȃ��B";
	#else
				info[i++] = "It provides no light.";
	#endif

				if (o_ptr->sval == SV_LITE_FEANOR)
				{
	#ifdef JP
					info[i++] = "����͖�����̔��a�����߂�(���a��-3)�B";
	#else
					info[i++] = "It decreases radius of light source by 3.";
	#endif
				}
				else if (o_ptr->sval == SV_LITE_LANTERN)
				{
	#ifdef JP
					info[i++] = "����͖�����̔��a�����߂�(���a��-2)�B";
	#else
					info[i++] = "It decreases radius of light source by 2.";
	#endif
				}
				else
				{
	#ifdef JP
					info[i++] = "����͖�����̔��a�����߂�(���a��-1)�B";
	#else
					info[i++] = "It decreases radius of light source by 1.";
	#endif
				}
			}
			else if (o_ptr->name1 == ART_TRAPEZOHEDRON)
			{
				info[i++] = "����͖�����̔��a�����߂�(���a��-4)�B";

			}
			else if (object_is_fixed_artifact(o_ptr))
			{
	#ifdef JP
	info[i++] = "����͉i���Ȃ閾����(���a 3)��������B";
	#else
				info[i++] = "It provides light (radius 3) forever.";
	#endif

			}
			else if (o_ptr->name2 == EGO_LITE_BRIGHT)
			{
				if (o_ptr->sval == SV_LITE_FEANOR)
				{
	#ifdef JP
	info[i++] = "����͉i���Ȃ閾����(���a 3)��������B";
	#else
					info[i++] = "It provides light (radius 3) forever.";
	#endif

				}
				else if (o_ptr->sval == SV_LITE_LANTERN)
				{
	#ifdef JP
	info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 3)��������B";
	#else
					info[i++] = "It provides light (radius 3) when fueled.";
	#endif

				}
				else
				{
	#ifdef JP
	info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 2)��������B";
	#else
					info[i++] = "It provides light (radius 2) when fueled.";
	#endif

				}
			}
			else
			{
				if (o_ptr->sval == SV_LITE_FEANOR)
				{
	#ifdef JP
	info[i++] = "����͉i���Ȃ閾����(���a 2)��������B";
	#else
					info[i++] = "It provides light (radius 2) forever.";
	#endif

				}
				else if (o_ptr->sval == SV_LITE_LANTERN)
				{
	#ifdef JP
	info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 2)��������B";
	#else
					info[i++] = "It provides light (radius 2) when fueled.";
	#endif

				}
				else
				{
	#ifdef JP
	info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 1)��������B";
	#else
					info[i++] = "It provides light (radius 1) when fueled.";
	#endif

				}
			}
			if (o_ptr->name2 == EGO_LITE_LONG)
			{
	#ifdef JP
	info[i++] = "����͒����^�[���������������B";
	#else
				info[i++] = "It provides light for much longer time.";
	#endif
			}
		}


		/* And then describe it fully */

		if (have_flag(flgs, TR_RIDING))
		{
			if ((o_ptr->tval == TV_SPEAR) && ((o_ptr->sval == SV_WEAPON_LANCE) || (o_ptr->sval == SV_WEAPON_HEAVY_LANCE)))
	#ifdef JP
	info[i++] = "����͔��ɏ�n�Ɍ��������킾�B";
	#else
				info[i++] = "It is made for use while riding.";
	#endif
			else
			{
	#ifdef JP
				info[i++] = "����͏�n���ł��g���₷���B";
	#else
				info[i++] = "It is suitable for use while riding.";
	#endif
				/* This information is not important enough */
				trivial_info++;
			}
		}
		if (have_flag(flgs, TR_STR))
		{
	#ifdef JP
	info[i++] = "����͘r�͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your strength.";
	#endif

		}
		if (have_flag(flgs, TR_INT))
		{
	#ifdef JP
	info[i++] = "����͒m�\�ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your intelligence.";
	#endif

		}
		if (have_flag(flgs, TR_WIS))
		{
	#ifdef JP
	info[i++] = "����͌����ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your wisdom.";
	#endif

		}
		if (have_flag(flgs, TR_DEX))
		{
	#ifdef JP
	info[i++] = "����͊�p���ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your dexterity.";
	#endif

		}
		if (have_flag(flgs, TR_CON))
		{
	#ifdef JP
	info[i++] = "����͑ϋv�͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your constitution.";
	#endif

		}
		if (have_flag(flgs, TR_CHR))
		{
	#ifdef JP
	info[i++] = "����͖��͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your charisma.";
	#endif

		}

		if (have_flag(flgs, TR_MAGIC_MASTERY))
		{
	#ifdef JP
	info[i++] = "����͖��@����g�p�\�͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your ability to use magic devices.";
	#endif

		}
		if (have_flag(flgs, TR_STEALTH))
		{
	#ifdef JP
	info[i++] = "����͉B���s���\�͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your stealth.";
	#endif

		}
		if (have_flag(flgs, TR_SEARCH))
		{
	#ifdef JP
	info[i++] = "����͒T���\�͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your searching.";
	#endif

		}
		//v1.1.32 ��ƕ� 
		//v1.1.99 �t���O�Ƃ��Đ����ǉ�
		if(o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_WORKER || have_flag(flgs,TR_DISARM))
		{
			info[i++]="����͉����\�͂ɉe�����y�ڂ��B";
		}
		//v1.1.99 ���@�h��
		if (have_flag(flgs, TR_SAVING))
		{
			info[i++] = "����͖��@�h��\�͂ɉe�����y�ڂ��B";
		}


		if (have_flag(flgs, TR_INFRA))
		{
	#ifdef JP
	info[i++] = "����͐ԊO�����͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your infravision.";
	#endif

		}
		if (have_flag(flgs, TR_TUNNEL))
		{
	#ifdef JP
	info[i++] = "����͍̌@�\�͂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your ability to tunnel.";
	#endif

		}
		if (have_flag(flgs, TR_SPEED))
		{
	#ifdef JP
	info[i++] = "����̓X�s�[�h�ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your speed.";
	#endif

		}
		if (have_flag(flgs, TR_BLOWS))
		{
	#ifdef JP
	info[i++] = "����͑Ō��񐔂ɉe�����y�ڂ��B";
	#else
			info[i++] = "It affects your attack speed.";
	#endif

		}

		if(o_ptr->name1 == ART_GRIMOIRE_OF_MARISA)
		{
			info[i++] = "����͓�ՓxEXTRA�ɂ����ăA�C�e���J�[�h�X���b�g���𑝂₷�B";
		}


		if (have_flag(flgs, TR_BRAND_ACID))
		{
	#ifdef JP
	info[i++] = "����͎_�ɂ���đ傫�ȃ_���[�W��^����B";
	#else
			info[i++] = "It does extra damage from acid.";
	#endif

		}
		if (have_flag(flgs, TR_BRAND_ELEC))
		{
	#ifdef JP
	info[i++] = "����͓d���ɂ���đ傫�ȃ_���[�W��^����B";
	#else
			info[i++] = "It does extra damage from electricity.";
	#endif

		}
		if (have_flag(flgs, TR_BRAND_FIRE))
		{
	#ifdef JP
	info[i++] = "����͉Ή��ɂ���đ傫�ȃ_���[�W��^����B";
	#else
			info[i++] = "It does extra damage from fire.";
	#endif

		}
		if (have_flag(flgs, TR_BRAND_COLD))
		{
	#ifdef JP
	info[i++] = "����͗�C�ɂ���đ傫�ȃ_���[�W��^����B";
	#else
			info[i++] = "It does extra damage from frost.";
	#endif

		}

		if (have_flag(flgs, TR_BRAND_POIS))
		{
	#ifdef JP
	info[i++] = "����͓G��ł���B";
	#else
			info[i++] = "It poisons your foes.";
	#endif

		}

		if (have_flag(flgs, TR_CHAOTIC))
		{
	#ifdef JP
	info[i++] = "����̓J�I�X�I�Ȍ��ʂ��y�ڂ��B";
	#else
			info[i++] = "It produces chaotic effects.";
	#endif

		}

		if (have_flag(flgs, TR_VAMPIRIC))
		{
	#ifdef JP
	info[i++] = "����͓G����q�b�g�|�C���g���z������B";
	#else
			info[i++] = "It drains life from your foes.";
	#endif

		}

		if (have_flag(flgs, TR_IMPACT))
		{
	#ifdef JP
	info[i++] = "����͒n�k���N�������Ƃ��ł���B";
	#else
			info[i++] = "It can cause earthquakes.";
	#endif

		}
		if (have_flag(flgs, TR_EX_VORPAL))
		{
	#ifdef JP
	info[i++] = "����͐��܂����؂ꖡ�œG��؂荏�ށB";
	#else
			info[i++] = "It is very sharp and can cut your foes.";
	#endif

		}
		else if (have_flag(flgs, TR_VORPAL))
		{
	#ifdef JP
	info[i++] = "����͔��ɐ؂ꖡ���s���G��ؒf���邱�Ƃ��ł���B";
	#else
			info[i++] = "It is very sharp and can cut your foes.";
	#endif

		}



		//���ꏈ��
		if (o_ptr->name1 == ART_EIKI)
		{
			info[i++] = "����͕K����������B";
			info[i++] = "����͍U������Ƃ��_�C�X����(�G���x��/10)������B";
		}
		else if (o_ptr->name1 == ART_KRONOS)
		{
			info[i++] = "����͑S�Ă̒j�̓V�G�ł���B";
		}

		if (have_flag(flgs, TR_KILL_DRAGON))
		{
	#ifdef JP
	info[i++] = "����̓h���S���ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of dragons.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_DRAGON))
		{
	#ifdef JP
	info[i++] = "����̓h���S���ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
	#else
			info[i++] = "It is especially deadly against dragons.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_DEITY))
		{
	#ifdef JP
	info[i++] = "����͐_�i�ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of orcs.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_DEITY))
		{
	#ifdef JP
	info[i++] = "����͐_�i�ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
	#else
			info[i++] = "It is especially deadly against orcs.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_KWAI))
		{
	#ifdef JP
	info[i++] = "����͗d���ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of trolls.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_KWAI))
		{
	#ifdef JP
	info[i++] = "����͗d���ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
	#else
			info[i++] = "It is especially deadly against trolls.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_GOOD))
		{
	#ifdef JP
	info[i++] = "����͒����̐��͂ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of giants.";
	#endif
		}
		else if (have_flag(flgs, TR_SLAY_GOOD))
		{
	#ifdef JP
	info[i++] = "����͒����̐��͂ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
	#else
			info[i++] = "It is especially deadly against giants.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_DEMON))
		{
	#ifdef JP
	info[i++] = "����͈����ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of demons.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_DEMON))
		{
	#ifdef JP
	info[i++] = "����͈����ɑ΂��Đ��Ȃ�͂𔭊�����B";
	#else
			info[i++] = "It strikes at demons with holy wrath.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_UNDEAD))
		{
	#ifdef JP
	info[i++] = "����̓A���f�b�h�ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of undead.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_UNDEAD))
		{
	#ifdef JP
	info[i++] = "����̓A���f�b�h�ɑ΂��Đ��Ȃ�͂𔭊�����B";
	#else
			info[i++] = "It strikes at undead with holy wrath.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_EVIL))
		{
	#ifdef JP
	info[i++] = "����͍��ׂ̐��͂ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of evil monsters.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_EVIL))
		{
	#ifdef JP
	info[i++] = "����͍��ׂ̐��͂ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
	#else
			info[i++] = "It fights against evil with holy fury.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_ANIMAL))
		{
	#ifdef JP
	info[i++] = "����͎��R�E�̓����ɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of natural creatures.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_ANIMAL))
		{
	#ifdef JP
	info[i++] = "����͎��R�E�̓����ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
	#else
			info[i++] = "It is especially deadly against natural creatures.";
	#endif

		}

		if (have_flag(flgs, TR_KILL_HUMAN))
		{
	#ifdef JP
	info[i++] = "����͐l�ԂɂƂ��Ă̓V�G�ł���B";
	#else
			info[i++] = "It is a great bane of humans.";
	#endif

		}
		else if (have_flag(flgs, TR_SLAY_HUMAN))
		{
	#ifdef JP
	info[i++] = "����͐l�Ԃɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
	#else
			info[i++] = "It is especially deadly against humans.";
	#endif

		}

		if (have_flag(flgs, TR_FORCE_WEAPON))
		{
	#ifdef JP
	info[i++] = "����͎g�p�҂̖��͂��g���čU������B";
	#else
			info[i++] = "It powerfully strikes at a monster using your mana.";
	#endif

		}
		if (have_flag(flgs, TR_DEC_MANA))
		{
	#ifdef JP
	info[i++] = "����͖��͂̏������������B";
	#else
			info[i++] = "It decreases your mana consumption.";
	#endif

		}
		if (have_flag(flgs, TR_SUST_STR))
		{
	#ifdef JP
	info[i++] = "����͂��Ȃ��̘r�͂��ێ�����B";
	#else
			info[i++] = "It sustains your strength.";
	#endif

		}
		if (have_flag(flgs, TR_SUST_INT))
		{
	#ifdef JP
	info[i++] = "����͂��Ȃ��̒m�\���ێ�����B";
	#else
			info[i++] = "It sustains your intelligence.";
	#endif

		}
		if (have_flag(flgs, TR_SUST_WIS))
		{
	#ifdef JP
	info[i++] = "����͂��Ȃ��̌������ێ�����B";
	#else
			info[i++] = "It sustains your wisdom.";
	#endif

		}
		if (have_flag(flgs, TR_SUST_DEX))
		{
	#ifdef JP
	info[i++] = "����͂��Ȃ��̊�p�����ێ�����B";
	#else
			info[i++] = "It sustains your dexterity.";
	#endif

		}
		if (have_flag(flgs, TR_SUST_CON))
		{
	#ifdef JP
	info[i++] = "����͂��Ȃ��̑ϋv�͂��ێ�����B";
	#else
			info[i++] = "It sustains your constitution.";
	#endif

		}
		if (have_flag(flgs, TR_SUST_CHR))
		{
	#ifdef JP
	info[i++] = "����͂��Ȃ��̖��͂��ێ�����B";
	#else
			info[i++] = "It sustains your charisma.";
	#endif

		}

		if (have_flag(flgs, TR_IM_ACID))
		{
	#ifdef JP
	info[i++] = "����͎_�ɑ΂��銮�S�ȖƉu��������B";
	#else
			info[i++] = "It provides immunity to acid.";
	#endif

		}

		else if (have_flag(flgs, TR_RES_ACID))
		{
	#ifdef JP
	info[i++] = "����͎_�ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to acid.";
	#endif

		}

		if (have_flag(flgs, TR_IM_ELEC))
		{
	#ifdef JP
	info[i++] = "����͓d���ɑ΂��銮�S�ȖƉu��������B";
	#else
			info[i++] = "It provides immunity to electricity.";
	#endif

		}
		else if (have_flag(flgs, TR_RES_ELEC))
		{
	#ifdef JP
	info[i++] = "����͓d���ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to electricity.";
	#endif

		}

		if (have_flag(flgs, TR_IM_FIRE))
		{
	#ifdef JP
	info[i++] = "����͉΂ɑ΂��銮�S�ȖƉu��������B";
	#else
			info[i++] = "It provides immunity to fire.";
	#endif

		}

		else if (have_flag(flgs, TR_RES_FIRE))
		{
	#ifdef JP
	info[i++] = "����͉΂ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to fire.";
	#endif

		}

		if (have_flag(flgs, TR_IM_COLD))
		{
	#ifdef JP
	info[i++] = "����͊����ɑ΂��銮�S�ȖƉu��������B";
	#else
			info[i++] = "It provides immunity to cold.";
	#endif

		}

		else if (have_flag(flgs, TR_RES_COLD))
		{
	#ifdef JP
	info[i++] = "����͊����ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to cold.";
	#endif
		}

		if (have_flag(flgs, TR_THROW))
		{
	#ifdef JP
			info[i++] = "����͓G�ɓ����đ傫�ȃ_���[�W��^���邱�Ƃ��ł���B";
	#else
			info[i++] = "It is perfectly balanced for throwing.";
	#endif
		}
		if (have_flag(flgs, TR_BOOMERANG))
		{
	#ifdef JP
			info[i++] = "����͓����Ă��茳�ɕԂ��Ă���B";
	#else
			info[i++] = "It is very sharp and can cut your foes.";
	#endif

		}


		if (have_flag(flgs, TR_RES_POIS))
		{
	#ifdef JP
			info[i++] = "����͓łւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to poison.";
	#endif

		}

		if (have_flag(flgs, TR_RES_LITE))
		{
	#ifdef JP
			info[i++] = "����͑M���ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to light.";
	#endif

		}
		if (have_flag(flgs, TR_RES_DARK))
		{
	#ifdef JP
			info[i++] = "����͈Í��ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to dark.";
	#endif

		}

		if (have_flag(flgs, TR_RES_WATER))
		{
	#ifdef JP
			info[i++] = "����͐��ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to blindness.";
	#endif

		}
		if (have_flag(flgs, TR_RES_HOLY))
		{
	#ifdef JP
			info[i++] = "����͔j�ׂւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to confusion.";
	#endif

		}
		if (have_flag(flgs, TR_RES_SOUND))
		{
	#ifdef JP
			info[i++] = "����͍����ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to sound.";
	#endif

		}
		if (have_flag(flgs, TR_RES_SHARDS))
		{
	#ifdef JP
			info[i++] = "����͔j�Ђւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to shards.";
	#endif

		}

		if (have_flag(flgs, TR_RES_NETHER))
		{
	#ifdef JP
			info[i++] = "����͒n���ƃG�i�W�[�h���C���ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to nether.";
	#endif

		}
		if (have_flag(flgs, TR_RES_TIME))
		{
	#ifdef JP
			info[i++] = "����͎���U���ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to nexus.";
	#endif

		}
		if (have_flag(flgs, TR_RES_CHAOS))
		{
	#ifdef JP
			info[i++] = "����̓J�I�X�ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to chaos.";
	#endif

		}
		if (have_flag(flgs, TR_RES_DISEN))
		{
	#ifdef JP
			info[i++] = "����͗򉻂ւ̑ϐ���������B";
	#else
			info[i++] = "It provides resistance to disenchantment.";
	#endif

		}

		if (have_flag(flgs, TR_RES_FEAR))
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ������|����ی삷��B";
	#else
			info[i++] = "It makes you completely fearless.";
	#endif

		}

		if (have_flag(flgs, TR_FREE_ACT))
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ���Ⴢ�Ö�����ی삷��B";
	#else
			info[i++] = "It provides immunity to paralysis.";
	#endif

		}

		if (have_flag(flgs, TR_RES_CONF))
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ�����������ی삷��B";
	#else
			info[i++] = "It makes you completely fearless.";
	#endif

		}

		if (have_flag(flgs, TR_RES_BLIND))
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ���Ӗڂ���ی삷��B";
	#else
			info[i++] = "It makes you completely fearless.";
	#endif

		}

		if (have_flag(flgs, TR_RES_INSANITY))
		{
	#ifdef JP
			info[i++] = "����͈يE�̜��܂������C�ɑ΂���ϐ���������B";
	#else
			info[i++] = "It provides resistance to life draining.";
	#endif

		}

		if (have_flag(flgs, TR_SPEEDSTER))
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ��̈ړ����x�𑬂߂�B";
	#else
			info[i++] = "It allows you to levitate.";
	#endif

		}
		if (have_flag(flgs, TR_LEVITATION))
		{
	#ifdef JP
			info[i++] = "����͒��ɕ������Ƃ��\�ɂ���B";
	#else
			info[i++] = "It allows you to levitate.";
	#endif

		}
		if (have_flag(flgs, TR_LITE))
		{
			if ((o_ptr->name1 == ART_NIGHT))
	#ifdef JP
				info[i++] = "����͖�����̔��a�����߂�(���a��-1)�B";
	#else
				info[i++] = "It decreases radius of your light source by 1.";
	#endif
			else
	#ifdef JP
				info[i++] = "����͉i���̖������������(���a��+1)�B";
	#else
				info[i++] = "It provides permanent light. (radius +1)";
	#endif

		}
		if (have_flag(flgs, TR_SEE_INVIS))
		{
	#ifdef JP
			info[i++] = "����͓����ȃ����X�^�[�����邱�Ƃ��\�ɂ���B";
	#else
			info[i++] = "It allows you to see invisible monsters.";
	#endif

		}
		if (have_flag(flgs, TR_TELEPATHY))
		{
	#ifdef JP
			info[i++] = "����̓e���p�V�[�\�͂�������B";
	#else
			info[i++] = "It gives telepathic powers.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_ANIMAL))
		{
	#ifdef JP
			info[i++] = "����͎��R�E�̐��������m����B";
	#else
			info[i++] = "It senses natural creatures.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_UNDEAD))
		{
	#ifdef JP
			info[i++] = "����̓A���f�b�h�����m����B";
	#else
			info[i++] = "It senses undead.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_DEMON))
		{
	#ifdef JP
			info[i++] = "����͈��������m����B";
	#else
			info[i++] = "It senses demons.";
	#endif

		}

		if (have_flag(flgs, TR_ESP_DEITY))
		{
	#ifdef JP
			info[i++] = "����͐_�i�����m����B";
	#else
			info[i++] = "It senses orcs.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_KWAI))
		{
	#ifdef JP
			info[i++] = "����͗d�������m����B";
	#else
			info[i++] = "It senses trolls.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_DRAGON))
		{
	#ifdef JP
			info[i++] = "����̓h���S�������m����B";
	#else
			info[i++] = "It senses dragons.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_HUMAN))
		{
	#ifdef JP
			info[i++] = "����͐l�Ԃ����m����B";
	#else
			info[i++] = "It senses humans.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_EVIL))
		{
	#ifdef JP
			info[i++] = "����͍��ׂ̐��͂����m����B";
	#else
			info[i++] = "It senses evil creatures.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_GOOD))
		{
	#ifdef JP
			info[i++] = "����͒����̐��͂����m����B";
	#else
			info[i++] = "It senses good creatures.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_NONLIVING))
		{
	#ifdef JP
			info[i++] = "����͊������閳�����̂����m����B";
	#else
			info[i++] = "It senses non-living creatures.";
	#endif

		}
		if (have_flag(flgs, TR_ESP_UNIQUE))
		{
	#ifdef JP
			info[i++] = "����͓��ʂȋ��G�����m����B";
	#else
			info[i++] = "It senses unique monsters.";
	#endif

		}
		if (have_flag(flgs, TR_SLOW_DIGEST))
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ��̉h�{�����x������B";
	#else
			info[i++] = "It slows your metabolism.";
	#endif

		}
		if (have_flag(flgs, TR_REGEN))
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ��̗͉̑񕜗͂���������B";
	#else
			info[i++] = "It speeds your regenerative powers.";
	#endif

		}
		if (have_flag(flgs, TR_WARNING))
		{
	#ifdef JP
			info[i++] = "����͊댯�ɑ΂��Čx���𔭂���B";
	#else
			info[i++] = "It warns you of danger";
	#endif

		}
		if (have_flag(flgs, TR_REFLECT))
		{
	#ifdef JP
			info[i++] = "����͖��{���g�𔽎˂���B";
	#else
			info[i++] = "It reflects bolts and arrows.";
	#endif

		}
		if (have_flag(flgs, TR_SH_FIRE))
		{
	#ifdef JP
			info[i++] = "����͉��̃o���A�𒣂�B";
	#else
			info[i++] = "It produces a fiery sheath.";
	#endif

		}
		if (have_flag(flgs, TR_SH_ELEC))
		{
	#ifdef JP
			info[i++] = "����͓d�C�̃o���A�𒣂�B";
	#else
			info[i++] = "It produces an electric sheath.";
	#endif

		}
		if (have_flag(flgs, TR_SH_COLD))
		{
	#ifdef JP
			info[i++] = "����͗�C�̃o���A�𒣂�B";
	#else
			info[i++] = "It produces a sheath of coldness.";
	#endif

		}
		if (have_flag(flgs, TR_NO_MAGIC))
		{
	#ifdef JP
			info[i++] = "����͔����@�o���A�𒣂�B";
	#else
			info[i++] = "It produces an anti-magic shell.";
	#endif

		}
		if (have_flag(flgs, TR_NO_TELE))
		{
	#ifdef JP
			info[i++] = "����̓e���|�[�g���ז�����B";
	#else
			info[i++] = "It prevents teleportation.";
	#endif

		}
		if (have_flag(flgs, TR_XTRA_MIGHT))
		{
	#ifdef JP
			info[i++] = "����͖�^�{���g�^�e����苭�͂ɔ��˂��邱�Ƃ��ł���B";
	#else
			info[i++] = "It fires missiles with extra might.";
	#endif

		}
		if (have_flag(flgs, TR_XTRA_SHOTS))
		{
	#ifdef JP
			info[i++] = "����͖�^�{���g�^�e����ɑ������˂��邱�Ƃ��ł���B";
	#else
			info[i++] = "It fires missiles excessively fast.";
	#endif

		}

		if (have_flag(flgs, TR_BLESSED))
		{
	#ifdef JP
			info[i++] = "����͐_�ɏj������Ă���B";
	#else
			info[i++] = "It has been blessed by the gods.";
	#endif

		}

		if (object_is_cursed(o_ptr))
		{
			if (o_ptr->curse_flags & TRC_PERMA_CURSE)
			{
	#ifdef JP
				info[i++] = "����͉i���̎􂢂��������Ă���B";
	#else
				info[i++] = "It is permanently cursed.";
	#endif

			}
			else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
	#ifdef JP
				info[i++] = "����͋��͂Ȏ􂢂��������Ă���B";
	#else
				info[i++] = "It is heavily cursed.";
	#endif

			}
			else
			{
	#ifdef JP
				info[i++] = "����͎���Ă���B";
	#else
				info[i++] = "It is cursed.";
	#endif

				/*
				 * It's a trivial infomation since there is
				 * fake inscription {cursed}
				 */
				trivial_info++;
			}
		}

		if ((have_flag(flgs, TR_TY_CURSE)) || (o_ptr->curse_flags & TRC_TY_CURSE))
		{
	#ifdef JP
			info[i++] = "����͑��ẨЁX�������O���h���Ă���B";
	#else
			info[i++] = "It carries an ancient foul curse.";
	#endif

		}
		if ((have_flag(flgs, TR_AGGRAVATE)) || (o_ptr->curse_flags & TRC_AGGRAVATE))
		{
	#ifdef JP
			info[i++] = "����͕t�߂̃����X�^�[��{�点��B";
	#else
			info[i++] = "It aggravates nearby creatures.";
	#endif

		}
		if ((have_flag(flgs, TR_DRAIN_EXP)) || (o_ptr->curse_flags & TRC_DRAIN_EXP))
		{
	#ifdef JP
			info[i++] = "����͌o���l���z�����B";
	#else
			info[i++] = "It drains experience.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_SLOW_REGEN)
		{
	#ifdef JP
			info[i++] = "����͉񕜗͂���߂�B";
	#else
			info[i++] = "It slows your regenerative powers.";
	#endif

		}
		if ((o_ptr->curse_flags & TRC_ADD_L_CURSE) || have_flag(flgs, TR_ADD_L_CURSE))
		{
	#ifdef JP
			info[i++] = "����͎ア�􂢂𑝂₷�B";
	#else
			info[i++] = "It adds weak curses.";
	#endif

		}
		if ((o_ptr->curse_flags & TRC_ADD_H_CURSE) || have_flag(flgs, TR_ADD_H_CURSE))
		{
	#ifdef JP
			info[i++] = "����͋��͂Ȏ􂢂𑝂₷�B";
	#else
			info[i++] = "It adds heavy curses.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_CALL_ANIMAL)
		{
	#ifdef JP
			info[i++] = "����͓������Ăъ񂹂�B";
	#else
			info[i++] = "It attracts animals.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_CALL_DEMON)
		{
	#ifdef JP
			info[i++] = "����͈������Ăъ񂹂�B";
	#else
			info[i++] = "It attracts demons.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_CALL_DRAGON)
		{
	#ifdef JP
			info[i++] = "����̓h���S�����Ăъ񂹂�B";
	#else
			info[i++] = "It attracts dragons.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_COWARDICE)
		{
	#ifdef JP
			info[i++] = "����͋��|���������N�����B";
	#else
			info[i++] = "It makes you subject to cowardice.";
	#endif

		}
		if ((have_flag(flgs, TR_TELEPORT)) || (o_ptr->curse_flags & TRC_TELEPORT))
		{
	#ifdef JP
			info[i++] = "����̓����_���ȃe���|�[�g�������N�����B";
	#else
			info[i++] = "It induces random teleportation.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_LOW_MELEE)
		{
	#ifdef JP
			info[i++] = "����͍U�����O���₷���B";
	#else
			info[i++] = "It causes you to miss blows.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_LOW_AC)
		{
	#ifdef JP
			info[i++] = "����͍U�����󂯂₷���B";
	#else
			info[i++] = "It helps your enemies' blows.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_LOW_MAGIC)
		{
	#ifdef JP
			info[i++] = "����͖��@�������ɂ�������B";
	#else
			info[i++] = "It encumbers you while spellcasting.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_FAST_DIGEST)
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ��̉h�{����𑬂�����B";
	#else
			info[i++] = "It speeds your metabolism.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_DRAIN_HP)
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ��̗̑͂��z�����B";
	#else
			info[i++] = "It drains you.";
	#endif

		}
		if (o_ptr->curse_flags & TRC_DRAIN_MANA)
		{
	#ifdef JP
			info[i++] = "����͂��Ȃ��̖��͂��z�����B";
	#else
			info[i++] = "It drains your mana.";
	#endif

		}

		/* Describe about this kind of object instead of THIS fake object */
		if (mode & SCROBJ_FAKE_OBJECT)
		{
			switch (o_ptr->tval)
			{
			case TV_RING:
				switch (o_ptr->sval)
				{
				case SV_RING_LORDLY:
	#ifdef JP
					info[i++] = "����͊���̃����_���ȑϐ���������B";
	#else
					info[i++] = "It provides some random resistances.";
	#endif
					break;
				case SV_RING_WARNING:
	#ifdef JP
					info[i++] = "����͂ЂƂ̒ዉ��ESP�������鎖������B";
	#else
					info[i++] = "It may provide a low rank ESP.";
	#endif
					break;
				}
				break;

			case TV_AMULET:
				switch (o_ptr->sval)
				{
				case SV_AMULET_RESISTANCE:
	#ifdef JP
					info[i++] = "����͓łւ̑ϐ��������鎖������B";
	#else
					info[i++] = "It may provides resistance to poison.";
	#endif
	#ifdef JP
					info[i++] = "����̓����_���ȑϐ��������鎖������B";
	#else
					info[i++] = "It may provide a random resistances.";
	#endif
					break;
				case SV_AMULET_THE_MAGI:
	#ifdef JP
					info[i++] = "����͍ő�łR�܂ł̒ዉ��ESP��������B";
	#else
					info[i++] = "It provides up to three low rank ESPs.";
	#endif
					break;
				}
				break;
			}
		}

		//���ꏈ��
		if (o_ptr->name1 == ART_CHICKEN_KNIFE)
		{
			info[i++] = "����̓N�G�X�g���瓦�S����΂���قǋ����Ȃ�B";
		}


	} //TV_ANTIQUE�ȊO�̃A�C�e���̃t���O�L�q�I��



	if (have_flag(flgs, TR_IGNORE_ACID) &&
	    have_flag(flgs, TR_IGNORE_ELEC) &&
	    have_flag(flgs, TR_IGNORE_FIRE) &&
	    have_flag(flgs, TR_IGNORE_COLD))
	{
#ifdef JP
		info[i++] = "����͎_�E�d���E�Ή��E��C�ł͏����Ȃ��B";
#else
		info[i++] = "It cannot be harmed by the elements.";
#endif
	}
	else
	{
		if (have_flag(flgs, TR_IGNORE_ACID))
		{
#ifdef JP
			info[i++] = "����͎_�ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by acid.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_ELEC))
		{
#ifdef JP
			info[i++] = "����͓d���ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by electricity.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_FIRE))
		{
#ifdef JP
			info[i++] = "����͉Ή��ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by fire.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_COLD))
		{
#ifdef JP
			info[i++] = "����͗�C�ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by cold.";
#endif
		}
	}

	if (mode & SCROBJ_FORCE_DETAIL) trivial_info = 0;

	/* No relevant informations */
	if (i <= trivial_info) return (FALSE);

	/* Save the screen */
	screen_save();

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Display Item name */
	if (!(mode & SCROBJ_FAKE_OBJECT))
		object_desc(o_name, o_ptr, 0);
	else
		object_desc(o_name, o_ptr, (OD_NAME_ONLY | OD_STORE));

	prt(o_name, 0, 0);

	/* Erase the screen */
	for (k = 1; k < hgt; k++) prt("", k, 13);

	/* Label the information */
	///pend item �ʐ^�Ɋւ���L�q�@TVAL��ύX����K�v�����邽�ߍ��͖��������Ă���
#if 0
	if ((o_ptr->tval == TV_STATUE) && (o_ptr->sval == SV_PHOTO))
	{
		monster_race *r_ptr = &r_info[o_ptr->pval];
		int namelen = strlen(r_name + r_ptr->name);
		prt(format("%s: '", r_name + r_ptr->name), 1, 15);
		Term_queue_bigchar(18 + namelen, 1, r_ptr->x_attr, r_ptr->x_char, 0, 0);
		prt("'", 1, (use_bigtile ? 20 : 19) + namelen);
	}
	else
#endif
///mod131227 �A�C�e���ڍו\���̂Ƃ��Ή��n���x��\������悤�ɂ��Ă݂�
if(o_ptr->tval == TV_KNIFE)		prt(format("����:�Z��(�Z�\Lv:%d)", ref_skill_exp(TV_KNIFE)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_SWORD)		prt(format("����:����(�Z�\Lv:%d)", ref_skill_exp(TV_SWORD)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_KATANA)	prt(format("����:��(�Z�\Lv:%d)", ref_skill_exp(TV_KATANA)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_HAMMER)	prt(format("����:�݊�(�Z�\Lv:%d)", ref_skill_exp(TV_HAMMER)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_STICK)		prt(format("����:�_(�Z�\Lv:%d)", ref_skill_exp(TV_STICK)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_AXE)		prt(format("����:��(�Z�\Lv:%d)", ref_skill_exp(TV_AXE)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_SPEAR)		prt(format("����:��(�Z�\Lv:%d)", ref_skill_exp(TV_SPEAR)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_POLEARM)	prt(format("����:��������(�Z�\Lv:%d)", ref_skill_exp(TV_POLEARM)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_BOW)		prt(format("����:�|(�Z�\Lv:%d)", ref_skill_exp(TV_BOW)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_CROSSBOW)	prt(format("����:�@�B�|(�Z�\Lv:%d)", ref_skill_exp(TV_CROSSBOW)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_GUN)		prt(format("����:�e(�Z�\Lv:%d)", ref_skill_exp(TV_GUN)/SKILL_LEV_TICK), 1, 15);
if(o_ptr->tval == TV_MAGICITEM)			prt(format("����:�}�W�b�N�A�C�e��(�Z�\Lv:--)"), 1, 15);
if(o_ptr->tval == TV_MAGICWEAPON)		prt(format("����:���ꕐ��(�Z�\Lv:--)"), 1, 15);
if(o_ptr->tval == TV_OTHERWEAPON)		prt(format("����:���ꕐ��(�Z�\Lv:--)"), 1, 15);
if(o_ptr->tval == TV_SHIELD)	prt(format("����:��(�Z�\Lv:%d)", ref_skill_exp(SKILL_SHIELD)/SKILL_LEV_TICK), 1, 15);



#ifdef JP
prt("     �A�C�e���̔\��:", 2, 15);
//prt("     �A�C�e���̔\��:", 1, 15);
#else
	prt("     Item Attributes:", 1, 15);
#endif

	/* We will print on top of the map (column 13) */
	//for (k = 2, j = 0; j < i; j++)
	for (k = 3, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 15);

		/* Every 20 entries (lines 2 to 21), start over */
		if ((k == hgt - 2) && (j+1 < i))
		{
#ifdef JP
prt("-- ���� --", k, 15);
#else
			prt("-- more --", k, 15);
#endif
			inkey();
			for (; k > 2; k--) prt("", k, 15);
		}
	}

	/* Wait for it */
#ifdef JP
prt("[�����L�[�������ƃQ�[���ɖ߂�܂�]", k, 15);
#else
	prt("[Press any key to continue]", k, 15);
#endif

	inkey();

	/* Restore the screen */
	screen_load();

	/* Gave knowledge */
	return (TRUE);
}



/*
 * Convert an inventory index into a one character label
 * Note that the label does NOT distinguish inven/equip.
 */
/*:::�C���x���g���ԍ���a�`w�ɕϊ�*/
char index_to_label(int i)
{
	/* Indexes for "inven" are easy */
	if (i < INVEN_RARM) return (I2A(i));

	/* Indexes for "equip" are offset */
	return (I2A(i - INVEN_RARM));
}


/*
 * Convert a label into the index of an item in the "inven"
 * Return "-1" if the label does not indicate a real item
 */
s16b label_to_inven(int c)
{
	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1);

	/* Verify the index */
	if ((i < 0) || (i > INVEN_PACK)) return (-1);

	/* Empty slots can never be chosen */
	if (!inventory[i].k_idx) return (-1);

	/* Return the index */
	return (i);
}


/* See cmd5.c */
extern bool select_ring_slot;


static bool is_ring_slot(int i)
{
	/*::: Hack - �O�d����ring_slot�̓���ւ�����𑽂��̑����X���b�g�Ɏg��*/
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		//v1.1.68 ���ꐫ�i�̂Ƃ��̓��{���X���b�g(��)���Ώۂɂ���
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			return ((i == INVEN_RIBBON) || (i == INVEN_RIGHT) || (i == INVEN_LEFT) || (i == INVEN_NECK) ||
				(i == INVEN_BODY) || (i == INVEN_OUTER) || (i == INVEN_HEAD) || (i == INVEN_HANDS) || (i == INVEN_FEET));

		}
		else
		{
			return ((i == INVEN_RIGHT) || (i == INVEN_LEFT) || (i == INVEN_NECK) ||
				(i == INVEN_BODY) || (i == INVEN_OUTER) || (i == INVEN_HEAD) || (i == INVEN_HANDS) || (i == INVEN_FEET));

		}
	}

	return (i == INVEN_RIGHT) || (i == INVEN_LEFT);
}


/*
 * Convert a label into the index of a item in the "equip"
 * Return "-1" if the label does not indicate a real item
 */
s16b label_to_equip(int c)
{
	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1) + INVEN_RARM;

	/* Verify the index */
	if ((i < INVEN_RARM) || (i >= INVEN_TOTAL)) return (-1);

	if (select_ring_slot) return is_ring_slot(i) ? i : -1;

	/* Empty slots can never be chosen */
	if (!inventory[i].k_idx) return (-1);

	/* Return the index */
	return (i);
}



/*
 * Determine which equipment slot (if any) an item likes
 */
/*:::�ǂ��ɑ��������A�C�e���Ȃ̂����ʂ���B�����i�łȂ��Ƃ�-1���Ԃ邱�Ƃɒ���*/
///item TVAL ����������
///mod131223 tval
///mod140308 �|�g�p�~�����֘A
s16b wield_slot(object_type *o_ptr)
{

	/* Slot for equipment */
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
		case TV_MAGICWEAPON:
	
		{
			if (!inventory[INVEN_RARM].k_idx) return (INVEN_RARM);
			if (inventory[INVEN_LARM].k_idx) return (INVEN_RARM);
			return (INVEN_LARM);
		}

		case TV_CAPTURE:
		case TV_MAGICITEM:
		case TV_SHIELD:
		{
			if (!inventory[INVEN_LARM].k_idx) return (INVEN_LARM);
			if (inventory[INVEN_RARM].k_idx) return (INVEN_LARM);
			return (INVEN_RARM);
		}

		//�|�F�ŗD��ō��葕���ɂȂ�
		case TV_BOW:
		{
			return (INVEN_LARM);
		}

		//�N���X�{�E�E�e�F�E��ɂȂɂ������Ă�����Ƃ肠��������
		case TV_CROSSBOW:
		case TV_GUN:
			{
			if (!inventory[INVEN_RARM].k_idx) return (INVEN_RARM);
			return (INVEN_LARM);
		}
		/*
		case TV_BOW:
		case TV_CROSSBOW:
		case TV_GUN:		
		{
			return (INVEN_BOW);
		}
		*/
		case TV_RIBBON:		
		{
			return (INVEN_RIBBON);
		}
		case TV_RING:
		{
			/* Use the right hand first */
			if (!inventory[INVEN_RIGHT].k_idx) return (INVEN_RIGHT);

			/* Use the left hand for swapping (by default) */
			return (INVEN_LEFT);
		}

		case TV_AMULET:
		case TV_WHISTLE:
		{
			return (INVEN_NECK);
		}

		case TV_LITE:
		{
			return (INVEN_LITE);
		}

		case TV_DRAG_ARMOR:
		case TV_ARMOR:
		case TV_CLOTHES:
		{
			return (INVEN_BODY);
		}

		case TV_CLOAK:
		{
			return (INVEN_OUTER);
		}

		case TV_HEAD:
		case TV_MASK:
		{
			return (INVEN_HEAD);
		}

		case TV_GLOVES:
		{
			return (INVEN_HANDS);
		}

		case TV_BOOTS:
		{
			return (INVEN_FEET);
		}
	}

	/* No slot available */
	return (-1);
}

///mod151202�O�d���p��O�����ǉ�
s16b wield_slot_3_fairies(object_type *o_ptr)
{

	if(p_ptr->pclass != CLASS_3_FAIRIES) return -1;
		
	switch (o_ptr->tval)
	{
		case TV_SHIELD:
			if (!is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
				return (INVEN_RIBBON);

			//v1.1.68 ���i�u�d����W���v�̂Ƃ��͏��X���b�g�����̂܂܁��ɍs���ē�������������

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
		{
			//v1.1.68
			if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES)&& !inventory[INVEN_RIBBON].k_idx)		return (INVEN_RIBBON);

			if (!inventory[INVEN_RIGHT].k_idx) return (INVEN_RIGHT);
			if (!inventory[INVEN_LEFT].k_idx) return (INVEN_LEFT);
			if (!inventory[INVEN_NECK].k_idx) return (INVEN_NECK);
			if (!inventory[INVEN_RIGHT].k_idx) return (INVEN_RIGHT);
			if (!inventory[INVEN_BODY].k_idx) return (INVEN_BODY);
			if (!inventory[INVEN_OUTER].k_idx) return (INVEN_OUTER);
			if (!inventory[INVEN_HEAD].k_idx) return (INVEN_HEAD);
			if (!inventory[INVEN_HANDS].k_idx) return (INVEN_HANDS);
			return (INVEN_FEET);
		}

		default:
			return wield_slot(o_ptr);
	}
}


/*
 * Return a string mentioning how a given item is carried
 */
///mod151202 �O�d���p���ꏈ���ǉ�
cptr mention_use(int i)
{
	cptr p;

	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		switch (i)
		{
		case INVEN_RARM:  
			p = "����1";
			break;
		case INVEN_LARM:  
			p = "����2";
			break;
		case INVEN_RIBBON:   
			if (!is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			{
				p = "��";
				break;
			}
			//v1.1.68 ���i�u�d����W���v�̂Ƃ��͏��X���b�g�����̂܂܁��ɍs���ē�������������

		case INVEN_RIGHT: 
		case INVEN_LEFT: 
		case INVEN_NECK: 
		case INVEN_BODY: 
		case INVEN_OUTER: 
		case INVEN_HEAD: 
		case INVEN_HANDS: 
		case INVEN_FEET: 
			p = "������"; 			
			break;
		case INVEN_LITE:  
			p = " ����"; 
			break;
		default:
			p = "�U�b�N"; 
			break;

		}
		return p;
	}


	/* Examine the location */
	switch (i)
	{
#ifdef JP
		case INVEN_RARM:  
			p = p_ptr->heavy_wield[0] ? "�^����" : ((p_ptr->ryoute && p_ptr->migite) ? " ����" : (left_hander ? " ����" : " �E��")); 
			break;
#else
		case INVEN_RARM:  p = p_ptr->heavy_wield[0] ? "Just lifting" : (p_ptr->migite ? "Wielding" : "On arm"); break;
#endif

#ifdef JP
		case INVEN_LARM:  
			p = p_ptr->heavy_wield[1] ? "�^����" : ((p_ptr->ryoute && p_ptr->hidarite) ? " ����" : (left_hander ? " �E��" : " ����")); 
			break;
#else
		case INVEN_LARM:  p = p_ptr->heavy_wield[1] ? "Just lifting" : (p_ptr->hidarite ? "Wielding" : "On arm"); break;
#endif

#ifdef JP
		case INVEN_RIBBON:   
			p = "���{��"; 
			break;
#else
		case INVEN_BOW:   p = (adj_str_hold[p_ptr->stat_ind[A_STR]] < inventory[i].weight / 10) ? "Just holding" : "Shooting"; break;
#endif

#ifdef JP
		case INVEN_RIGHT: p = (left_hander ? "����w" : "�E��w"); break;
#else
		case INVEN_RIGHT: p = (left_hander ? "On left hand" : "On right hand"); break;
#endif

#ifdef JP
		case INVEN_LEFT:  p = (left_hander ? "�E��w" : "����w"); break;
#else
		case INVEN_LEFT:  p = (left_hander ? "On right hand" : "On left hand"); break;
#endif

#ifdef JP
		case INVEN_NECK:  p = "  ��"; break;
#else
		case INVEN_NECK:  p = "Around neck"; break;
#endif

#ifdef JP
		case INVEN_LITE:  p = " ����"; break;
#else
		case INVEN_LITE:  p = "Light source"; break;
#endif

#ifdef JP
		case INVEN_BODY:  p = "  ��"; break;
#else
		case INVEN_BODY:  p = "On body"; break;
#endif

#ifdef JP
		case INVEN_OUTER: p = "�̂̏�"; break;
#else
		case INVEN_OUTER: p = "About body"; break;
#endif

#ifdef JP
		case INVEN_HEAD:  p = "  ��"; break;
#else
		case INVEN_HEAD:  p = "On head"; break;
#endif

#ifdef JP
		case INVEN_HANDS: p = "  ��"; break;
#else
		case INVEN_HANDS: p = "On hands"; break;
#endif

#ifdef JP
		case INVEN_FEET:  p = "  ��"; break;
#else
		case INVEN_FEET:  p = "On feet"; break;
#endif

#ifdef JP
		default:          p = "�U�b�N"; break;
#else
		default:          p = "In pack"; break;
#endif
	}

	/* Return the result */
	return p;
}


/*
 * Return a string describing how a given item is being worn.
 * Currently, only used for items in the equipment, not inventory.
 */
cptr describe_use(int i)
{
	cptr p;
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		if(i<INVEN_RARM) return "�U�b�N�ɓ����Ă���";
		else if(i <= INVEN_LARM && p_ptr->heavy_wield[i-INVEN_RARM]) return "�^������";
		else return "��������";
	}

	switch (i)
	{
#ifdef JP
		case INVEN_RARM:  p = p_ptr->heavy_wield[0] ? "�^������" : ((p_ptr->ryoute && p_ptr->migite) ? "����ɑ������Ă���" : (left_hander ? "����ɑ������Ă���" : "�E��ɑ������Ă���")); break;
#else
		case INVEN_RARM:  p = p_ptr->heavy_wield[0] ? "just lifting" : (p_ptr->migite ? "attacking monsters with" : "wearing on your arm"); break;
#endif

#ifdef JP
		case INVEN_LARM:  p = p_ptr->heavy_wield[1] ? "�^������" : ((p_ptr->ryoute && p_ptr->hidarite) ? "����ɑ������Ă���" : (left_hander ? "�E��ɑ������Ă���" : "����ɑ������Ă���")); break;
#else
		case INVEN_LARM:  p = p_ptr->heavy_wield[1] ? "just lifting" : (p_ptr->hidarite ? "attacking monsters with" : "wearing on your arm"); break;
#endif

#ifdef JP
		case INVEN_RIBBON:   p = "�g�������Ă���"; break;
#else
		case INVEN_BOW:   p = (adj_str_hold[p_ptr->stat_ind[A_STR]] < inventory[i].weight / 10) ? "just holding" : "shooting missiles with"; break;
#endif

#ifdef JP
		case INVEN_RIGHT: p = (left_hander ? "����̎w�ɂ͂߂Ă���" : "�E��̎w�ɂ͂߂Ă���"); break;
#else
		case INVEN_RIGHT: p = (left_hander ? "wearing on your left hand" : "wearing on your right hand"); break;
#endif

#ifdef JP
		case INVEN_LEFT:  p = (left_hander ? "�E��̎w�ɂ͂߂Ă���" : "����̎w�ɂ͂߂Ă���"); break;
#else
		case INVEN_LEFT:  p = (left_hander ? "wearing on your right hand" : "wearing on your left hand"); break;
#endif

#ifdef JP
		case INVEN_NECK:  p = "��ɂ����Ă���"; break;
#else
		case INVEN_NECK:  p = "wearing around your neck"; break;
#endif

#ifdef JP
		case INVEN_LITE:  p = "�����ɂ��Ă���"; break;
#else
		case INVEN_LITE:  p = "using to light the way"; break;
#endif

#ifdef JP
		case INVEN_BODY:  p = "�̂ɒ��Ă���"; break;
#else
		case INVEN_BODY:  p = "wearing on your body"; break;
#endif

#ifdef JP
		case INVEN_OUTER: p = "�g�ɂ܂Ƃ��Ă���"; break;
#else
		case INVEN_OUTER: p = "wearing on your back"; break;
#endif

#ifdef JP
		case INVEN_HEAD:  p = "���ɂ��Ԃ��Ă���"; break;
#else
		case INVEN_HEAD:  p = "wearing on your head"; break;
#endif

#ifdef JP
		case INVEN_HANDS: p = "��ɂ��Ă���"; break;
#else
		case INVEN_HANDS: p = "wearing on your hands"; break;
#endif

#ifdef JP
		case INVEN_FEET:  p = "���ɂ͂��Ă���"; break;
#else
		case INVEN_FEET:  p = "wearing on your feet"; break;
#endif

#ifdef JP
		default:          p = "�U�b�N�ɓ����Ă���"; break;
#else
		default:          p = "carrying in your pack"; break;
#endif
	}

	/* Return the result */
	return p;
}


/* Hack: Check if a spellbook is one of the realms we can use. -- TY */
/*:::�����g�p�\�Ȗ��@�����ǂ�������i�w�K�\�ł��K�����Ă��Ȃ��̈�͊܂܂�Ȃ��j*/
///mod140207 ���@�ύX�ɔ�����������ς���
bool check_book_realm(const byte book_tval, const byte book_sval)
{
	if (book_tval > TV_BOOK_END) return FALSE;

	if((p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU) && book_tval == TV_BOOK_HISSATSU) return TRUE;
	if(book_tval == TV_BOOK_MEDICINE) return FALSE;
	/*:::���@�g�p�s�E�t���O�@���̍s�Ȃ��Ă��Ō��FALSE������͂������ꉞ*/
	if(cp_ptr->realm_aptitude[0] == 0) return FALSE;

	//v1.1.32 �p�`�����[��p���i�u���s�v�̗�O����
	if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && book_tval <= MAX_MAGIC)
		return TRUE;


	/*:::���@�K���ς݈����̓���E�t���O*/
	if(REALM_SPELLMASTER)
	{
		switch(cp_ptr->realm_aptitude[book_tval])
		{
		case 0:
			return FALSE;
		case 1:
			if(book_sval > 2) return FALSE;
			else return TRUE;
		default:
			return TRUE;
		}
	}
	/*:::����ȊO�̐E�̏ꍇ�A���̈悩���̈��TVAL����v�����TRUE*/
	return (p_ptr->realm1 == book_tval || p_ptr->realm2 == book_tval);
}


/*
 * Check an item against the item tester info
 */
/*:::�A�C�e�����⏰��A�C�e���̒��ő����ł��镐���g���閂�@���Ȃǂӂ��킵���A�C�e�����ǂ�������*/
/*:::���G�ȏ����w����������ꍇ�͂��̊֐����ĂԑO��item_tester_tval��item_tester_hook()�ɏ����������Ă���*/
///item get_item()�ł̃`�F�b�N
bool item_tester_okay(object_type *o_ptr)
{
	/* Hack -- allow listing empty slots */
	/*:::�u�S�ĕ\������t���O�v�Ǝv����*/
	if (item_tester_full) return (TRUE);

	/* Require an item */
	/*:::�A�C�e�����󗓂̂Ƃ�*/
	if (!o_ptr->k_idx) return (FALSE);

	/* Hack -- ignore "gold" */
	/*:::����̋��͖����@�������t���O�����Ă�ꍇ������*/
	if (o_ptr->tval == TV_GOLD)
	{
		/* See xtra2.c */
		extern bool show_gold_on_floor;

		if (!show_gold_on_floor) return (FALSE);
	}

	/* Check the tval */
	/*:::���b�h�▂�@�ȂǓ����ނ̃A�C�e���̂ݕ\���������ꍇ�A���̃��[�`�����ĂԑO��item_tester_tval�Ɏw�肵�Ă���*/
	if (item_tester_tval)
	{
		/* Is it a spellbook? If so, we need a hack -- TY */
		///realm�@item �����ǂ߂閂�@��
		/*:::Q017���@���Ȃ�̈悪�����Ă邩�`�F�b�N�A�炵�����ǂ�����DEATH�`LIFE�Ȃ̂��HID����������O���̈������̂�*/
		/*:::�����炭�A�̂͂��̌ܗ̈悾�����������c�B���݂����ɗ���Ƃ�item_tester_tval�͏K�������̈�Ɋւ�炸�E�Ƃ��Ƃ̌Œ�l�������Ă���B(m_info�Q��)*/
		/*:::�Ⴆ�ΐԖ��ň����̖��@���O���ڂ������Ă��Ԃ�m�R�}���h���������ꍇ�Aitem_tester_tval�ɂ�TV_SORCERY�������Ă���check_book_realm()�Ɉڂ肻�̒��Œe�����B*/
		///mod140207 ���@�ύX
		
		//if ((item_tester_tval <= TV_DEATH_BOOK) &&
		//	(item_tester_tval >= TV_LIFE_BOOK))
		//	return check_book_realm(o_ptr->tval, o_ptr->sval);
		//else
		
			if (item_tester_tval != o_ptr->tval) return (FALSE);
	}

	/* Check the hook */
	/*:::item_tester_hook�Ɋ֐��i����̃A�C�e���̎�ނ𔻒肵�ĕԂ��悤�Ȃ��́j�������Ă�΂�������肷��*/
	if (item_tester_hook)
	{
		if (!(*item_tester_hook)(o_ptr)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}




/*
 * Choice window "shadow" of the "show_inven()" function
 */
/*:::Term�ɑ�����ꂽ�A�h���X�ɑ΂��A�C�e���ꗗ��\��*/
void display_inven(void)
{
	register        int i, n, z = 0;
	object_type     *o_ptr;
	byte            attr = TERM_WHITE;
	char            tmp_val[80];/*:::�C���x���g���̈�s���̕�����ҏW�p*/
	char            o_name[MAX_NLEN];
	int             wid, hgt;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Find the "final" slot */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Track */
		z = i + 1;
	}

	/* Display the pack */
	for (i = 0; i < z; i++)
	{
		/* Examine the item */
		o_ptr = &inventory[i];

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Is this item "acceptable"? */
		/*:::�e�A�C�e���ɕ\�����Ă悢���t�B���^��������炵��*/
		/*:::����hook���w�肵�Ă��Ȃ����߁A���Ƌ�A�C�e���ȊO�̑S�Ă��ʂ�*/
		if (item_tester_okay(o_ptr))
		{
			/*:::������̍ŏ���a)�`w)�ɂ��Ă���*/
			/* Prepare an "index" */
			tmp_val[0] = index_to_label(i);

			/* Bracket the "index" --(-- */
			tmp_val[1] = ')';
		}

		/* Display the index (or blank space) */
		Term_putstr(0, i, 3, TERM_WHITE, tmp_val);

		/* Obtain an item description */
		object_desc(o_name, o_ptr, 0);

		/* Obtain the length of the description */
		n = strlen(o_name);

		/* Get a color */
		/*:::�A�C�e���啪�ނ��ƂɐF���w��*/
		attr = tval_to_attr[o_ptr->tval % 128];

		/* Grey out charging items */
		/*:::�[�U��*/
		if (o_ptr->timeout)
		{
			attr = TERM_L_DARK;
		}

		/* Display the entry itself */
		Term_putstr(3, i, n, attr, o_name);

		/* Erase the rest of the line */
		Term_erase(3+n, i, 255);

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt),lbtokg2(wgt) );
#else
			sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			prt(tmp_val, i, wid - 9);
		}
	}

	/* Erase the rest of the window */
	for (i = z; i < hgt; i++)
	{
		/* Erase the line */
		Term_erase(0, i, 255);
	}
}



/*
 * Choice window "shadow" of the "show_equip()" function
 */
/*:::�T�u�E�B���h�E�ɑ����i�ꗗ��\������*/
void display_equip(void)
{
	register        int i, n;
	object_type     *o_ptr;
	byte            attr = TERM_WHITE;
	char            tmp_val[80];
	char            o_name[MAX_NLEN];
	int             wid, hgt;
	bool			flag_reins = FALSE;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Display the equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Examine the item */
		o_ptr = &inventory[i];

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Is this item "acceptable"? */
		/*:::select_ring_slot�͎w�ւ̑����̂Ƃ��g�� ���łɑ����ς݂Ȃ̂ŋ�łȂ���Βʂ�*/
		if (select_ring_slot ? is_ring_slot(i) : item_tester_okay(o_ptr))
		{
			/* Prepare an "index" */
			tmp_val[0] = index_to_label(i);

			/* Bracket the "index" --(-- */
			tmp_val[1] = ')';
		}

		/* Display the index (or blank space) */
		Term_putstr(0, i - INVEN_RARM, 3, TERM_WHITE, tmp_val);

		///mod140508 �ϐg������������
		if(check_invalidate_inventory(i))
		{
			if(SHION_POSSESSING)
				strcpy(o_name, "(�߈˒��̂��ߖ�����)");
			else
				strcpy(o_name, "(�ϐg���̂��ߖ�����)");
			attr = TERM_L_DARK;
		}
		///mod150805 �A���X
		else if(p_ptr->pclass == CLASS_ALICE && (i==INVEN_RARM || i == INVEN_LARM) && !o_ptr->k_idx )
		{
			strcpy(o_name, "(�l�`�𑀂��Ă���)");
			attr = TERM_WHITE;
		}

		/* Obtain an item description */
			///mod140308 �|�g�p�~�����֘A
			/*:::Mega Hack �|�𗼎�Ŏ����Ă��鎞�A�E�葤�ɋ����Ɂi����𗼎莝���j�ƕ\������*/
		//if ((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute)
		else if (((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute
			||  (i == INVEN_RARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_RARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_LARM])
			||  (i == INVEN_LARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_LARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_RARM]))
			&& p_ptr->pclass != CLASS_3_FAIRIES)
		{
			strcpy(o_name, "(����𗼎莝��)");
			attr = TERM_WHITE;
		}
		else if((i==INVEN_RARM) && !(CAN_TWO_HANDS_WIELDING()) && !inventory[INVEN_RARM].k_idx )
		{
			strcpy(o_name, "(�R�惂���X�^�[�𑀂��Ă���)");
			attr = TERM_WHITE;
			flag_reins = TRUE;
		}
		else if((i==INVEN_LARM) && !(CAN_TWO_HANDS_WIELDING()) && !inventory[INVEN_LARM].k_idx && !flag_reins)
		{
			strcpy(o_name, "(�R�惂���X�^�[�𑀂��Ă���)");
			attr = TERM_WHITE;
		}
		else if(i == INVEN_NECK && p_ptr->pclass == CLASS_BANKI)
		{
			strcpy(o_name, "(�����s��)");
			attr = TERM_L_DARK;
		}
		///mod150805 �A���X
		else if(i == INVEN_HANDS && p_ptr->pclass == CLASS_ALICE)
		{
			strcpy(o_name, "(�����s��)");
			attr = TERM_L_DARK;
		}
		else if (i >= INVEN_RIGHT && i <= INVEN_LEFT && p_ptr->pclass == CLASS_JYOON)
		{
			strcpy(o_name, "(�����s��)");
			attr = TERM_L_DARK;
		}
		else if(i == INVEN_FEET && (p_ptr->muta3 & MUT3_FISH_TAIL || p_ptr->prace == RACE_NINGYO || p_ptr->pclass == CLASS_TOZIKO || p_ptr->pclass == CLASS_KISUME))
		{
			strcpy(o_name, "(�����s��)");
			attr = TERM_L_DARK;
		}
		else
		{
			u32b mode = 0L;
			if(i== INVEN_RARM) mode |= OD_WEAPON_RARM;
			else if(i== INVEN_LARM) mode |= OD_WEAPON_LARM;
			object_desc(o_name, o_ptr, mode);
			attr = tval_to_attr[o_ptr->tval % 128];
		}

		/* Obtain the length of the description */
		n = strlen(o_name);

		/* Grey out charging items */
		if (o_ptr->timeout)
		{
			attr = TERM_L_DARK;
		}

		/* Display the entry itself */
		Term_putstr(3, i - INVEN_RARM, n, attr, o_name);

		/* Erase the rest of the line */
		Term_erase(3+n, i - INVEN_RARM, 255);

		/* Display the weight (if needed) */
		if (show_weights)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt));
#else
			sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			//prt(tmp_val, i - INVEN_RARM, wid - (show_labels ? 28 : 9));
			prt(tmp_val, i - INVEN_RARM, wid - (show_labels ? 22 : 9));
		}

		/* Display the slot description (if needed) */
		if (show_labels)
		{
			//Term_putstr(wid - 20, i - INVEN_RARM, -1, TERM_WHITE, " <-- ");
			//prt(mention_use(i), i - INVEN_RARM, wid - 15);
			Term_putstr(wid - 14, i - INVEN_RARM, -1, TERM_WHITE, " <-- ");
			prt(mention_use(i), i - INVEN_RARM, wid - 9);
		}
	}

	/* Erase the rest of the window */
	for (i = INVEN_TOTAL - INVEN_RARM; i < hgt; i++)
	{
		/* Clear that line */
		Term_erase(0, i, 255);
	}
}


/*
 * Find the "first" inventory object with the given "tag".
 *
 * A "tag" is a numeral "n" appearing as "@n" anywhere in the
 * inscription of an object.  Alphabetical characters don't work as a
 * tag in this form.
 *
 * Also, the tag "@xn" will work as well, where "n" is a any tag-char,
 * and "x" is the "current" command_cmd code.
 */
static bool get_tag(int *cp, char tag, int mode)
{
	int i, start, end;
	cptr s;

	/* Extract index from mode */
	switch (mode)
	{
	case USE_EQUIP:
		start = INVEN_RARM;
		end = INVEN_TOTAL - 1;
		break;

	case USE_INVEN:
		start = 0;
		end = INVEN_PACK - 1;
		break;

	default:
		return FALSE;
	}

	/**** Find a tag in the form of {@x#} (allow alphabet tag) ***/

	/* Check every inventory object */
	for (i = start; i <= end; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Skip empty inscriptions */
		if (!o_ptr->inscription) continue;

		/* Skip non-choice */
		if (!item_tester_okay(o_ptr)) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(o_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if ((s[1] == command_cmd) && (s[2] == tag))
			{
				/* Save the actual inventory ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}


	/**** Find a tag in the form of {@#} (allows only numerals)  ***/

	/* Don't allow {@#} with '#' being alphabet */
	if (tag < '0' || '9' < tag)
	{
		/* No such tag */
		return FALSE;
	}

	/* Check every object */
	for (i = start; i <= end; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Skip empty inscriptions */
		if (!o_ptr->inscription) continue;

		/* Skip non-choice */
		if (!item_tester_okay(o_ptr)) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(o_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the normal tags */
			if (s[1] == tag)
			{
				/* Save the actual inventory ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}

	/* No such tag */
	return (FALSE);
}


/*
 * Find the "first" floor object with the given "tag".
 *
 * A "tag" is a numeral "n" appearing as "@n" anywhere in the
 * inscription of an object.  Alphabetical characters don't work as a
 * tag in this form.
 *
 * Also, the tag "@xn" will work as well, where "n" is a any tag-char,
 * and "x" is the "current" command_cmd code.
 */
/*:::����̃A�C�e���̃^�O�i���@u1�ȂǁH)��T���@�R�}���h�ƃ^�O����v�������̂������TRUE�炵��*/
static bool get_tag_floor(int *cp, char tag, int floor_list[], int floor_num)
{
	int i;
	cptr s;

	/**** Find a tag in the form of {@x#} (allow alphabet tag) ***/

	/* Check every object in the grid */
	for (i = 0; i < floor_num && i < 23; i++)
	{
		object_type *o_ptr = &o_list[floor_list[i]];

		/* Skip empty inscriptions */
		if (!o_ptr->inscription) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(o_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if ((s[1] == command_cmd) && (s[2] == tag))
			{
				/* Save the actual floor object ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}


	/**** Find a tag in the form of {@#} (allows only numerals)  ***/

	/* Don't allow {@#} with '#' being alphabet */
	if (tag < '0' || '9' < tag)
	{
		/* No such tag */
		return FALSE;
	}

	/* Check every object in the grid */
	for (i = 0; i < floor_num && i < 23; i++)
	{
		object_type *o_ptr = &o_list[floor_list[i]];

		/* Skip empty inscriptions */
		if (!o_ptr->inscription) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(o_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the normal tags */
			if (s[1] == tag)
			{
				/* Save the floor object ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}

	/* No such tag */
	return (FALSE);
}


/*
 * Move around label characters with correspond tags
 */
/*:::�����̗v�f�ɏ��ԂɃ��x����t���H*/
static void prepare_label_string(char *label, int mode)
{
	cptr alphabet_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int  offset = (mode == USE_EQUIP) ? INVEN_RARM : 0;
	int  i;

	/* Prepare normal labels */
	strcpy(label, alphabet_chars);

	/* Move each label */
	for (i = 0; i < 52; i++)
	{
		int index;
		char c = alphabet_chars[i];

		/* Find a tag with this label */
		if (get_tag(&index, c, mode))
		{
			/* Delete the overwritten label */
			if (label[i] == c) label[i] = ' ';

			/* Move the label to the place of corresponding tag */
			label[index - offset] = c;
		}
	}
}


/*
 * Move around label characters with correspond tags (floor version)
 */
static void prepare_label_string_floor(char *label, int floor_list[], int floor_num)
{
	cptr alphabet_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int  i;

	/* Prepare normal labels */
	strcpy(label, alphabet_chars);

	/* Move each label */
	for (i = 0; i < 52; i++)
	{
		int index;
		char c = alphabet_chars[i];

		/* Find a tag with this label */
		if (get_tag_floor(&index, c, floor_list, floor_num))
		{
			/* Delete the overwritten label */
			if (label[i] == c) label[i] = ' ';

			/* Move the label to the place of corresponding tag */
			label[index] = c;
		}
	}
}


/*
 * Display the inventory.
 *
 * Hack -- do not display "trailing" empty slots
 */
int show_inven(int target_item)
{
	int             i, j, k, l, z = 0;
	int             col, cur_col, len;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	char            tmp_val[80];
	int             out_index[23];
	byte            out_color[23];
	char            out_desc[23][MAX_NLEN];
	int             target_item_label = 0;
	int             wid, hgt;
	char            inven_label[52 + 1];

	/* Starting column */
	col = command_gap;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Default "max-length" */
	len = wid - col - 1;


	/* Find the "final" slot */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Track */
		z = i + 1;
	}

	prepare_label_string(inven_label, USE_INVEN);

	/* Display the inventory */
	for (k = 0, i = 0; i < z; i++)
	{
		o_ptr = &inventory[i];

		/* Is this item acceptable? */
		/*:::�󗓈ȊO�͒ʂ�͂�*/
		if (!item_tester_okay(o_ptr)) continue;

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);

		/* Save the object index, color, and description */
		out_index[k] = i;
		out_color[k] = tval_to_attr[o_ptr->tval % 128];

		/* Grey out charging items */
		if (o_ptr->timeout)
		{
			out_color[k] = TERM_L_DARK;
		}

		(void)strcpy(out_desc[k], o_name);

		/* Find the predicted "line length" */
		l = strlen(out_desc[k]) + 5;

		/* Be sure to account for the weight */
		if (show_weights) l += 9;

		/* Account for icon if displayed */
		if (show_item_graph)
		{
			l += 2;
			if (use_bigtile) l++;
		}

		/* Maintain the maximum length */
		if (l > len) len = l;

		/* Advance to next "line" */
		k++;
	}

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);

	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		/* Get the index */
		i = out_index[j];

		/* Get the item */
		o_ptr = &inventory[i];

		/* Clear the line */
		prt("", j + 1, col ? col - 2 : col);

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "�t");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "  ");
		}
		else if (i <= INVEN_PACK)
		{
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", inven_label[i]);
		}
		else
		{
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", index_to_label(i));
		}

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j + 1, col);

		cur_col = col + 3;

		/* Display graphics for object, if desired */
		if (show_item_graph)
		{
			byte  a = object_attr(o_ptr);
			char c = object_char(o_ptr);

//tmp if(o_ptr->name2 == 98) msg_format("attr:%d char:%c desc:%s",a,c,out_desc[j]);

#ifdef AMIGA
			if (a & 0x80) a |= 0x40;
#endif

			Term_queue_bigchar(cur_col, j + 1, a, c, 0, 0);
			if (use_bigtile) cur_col++;

			cur_col += 2;
		}


		/* Display the entry itself */
		c_put_str(out_color[j], out_desc[j], j + 1, cur_col);

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			(void)sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			(void)sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			prt(tmp_val, j + 1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	/* Save the new column */
	command_gap = col;

	return target_item_label;
}



/*
 * Display the equipment.
 */
/*:::�����i�ꗗ��\������B*/
int show_equip(int target_item)
{
	int             i, j, k, l;
	int             col, cur_col, len;
	object_type     *o_ptr;
	char            tmp_val[80];
	char            o_name[MAX_NLEN];
	int             out_index[23];
	byte            out_color[23];
	char            out_desc[23][MAX_NLEN];
	int             target_item_label = 0;
	int             wid, hgt;
	char            equip_label[52 + 1];
	bool			flag_reins = FALSE;

	/* Starting column */
	col = command_gap;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Maximal length */
	len = wid - col - 1;


	/* Scan the equipment list */
	for (k = 0, i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		u32b od_mode = 0L;
		o_ptr = &inventory[i];

		/* Is this item acceptable? */
		if (!(select_ring_slot ? is_ring_slot(i) : item_tester_okay(o_ptr)) &&
		    (!((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute) ||
		     item_tester_no_ryoute)) continue;
			if(i==INVEN_RARM) od_mode |= OD_WEAPON_RARM;
			else if(i==INVEN_LARM) od_mode |= OD_WEAPON_LARM;

		/* Description */
		object_desc(o_name, o_ptr, od_mode);
			///mod140308 �|�g�p�~�����֘A
			/*:::Mega Hack �ˌ�����𗼎�Ŏ����Ă��鎞�A�����Ɂi����𗼎莝���j�ƕ\������*/
		//if ((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute)

		if(check_invalidate_inventory(i))
		{
			if (SHION_POSSESSING)
				strcpy(out_desc[k], "(�߈˒��̂��ߖ�����)");
			else
				strcpy(out_desc[k], "(�ϐg���̂��ߖ�����)");
			out_color[k] = TERM_L_DARK;
		}
		///mod150805 �A���X
		else if(p_ptr->pclass == CLASS_ALICE && (i==INVEN_RARM || i == INVEN_LARM) && !o_ptr->k_idx )
		{
			strcpy(out_desc[k], "(�l�`�𑀂��Ă���)");
			out_color[k] = TERM_WHITE;
		}
		else if (((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute
			||  (i == INVEN_RARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_RARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_LARM]) 
			||  (i == INVEN_LARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_LARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_RARM]) )
			&& p_ptr->pclass != CLASS_3_FAIRIES)
		{
#ifdef JP
			(void)strcpy(out_desc[k],"(����𗼎莝��)");
#else
			(void)strcpy(out_desc[k],"(wielding with two-hands)");
#endif
			out_color[k] = TERM_WHITE;
		}
		else if((i==INVEN_RARM) && !(CAN_TWO_HANDS_WIELDING()) && !inventory[INVEN_RARM].k_idx )
		{
			strcpy(out_desc[k], "(�R�惂���X�^�[�𑀂��Ă���)");
			out_color[k] = TERM_WHITE;
			flag_reins = TRUE;
		}
		else if((i==INVEN_LARM) && !(CAN_TWO_HANDS_WIELDING()) && !inventory[INVEN_LARM].k_idx && !flag_reins)
		{
			strcpy(out_desc[k], "(�R�惂���X�^�[�𑀂��Ă���)");
			out_color[k] = TERM_WHITE;		
		}
		else if(i == INVEN_NECK && p_ptr->pclass == CLASS_BANKI)
		{
			strcpy(out_desc[k], "(�����s��)");
			out_color[k] = TERM_L_DARK;
		}
		else if(i == INVEN_HANDS && p_ptr->pclass == CLASS_ALICE)
		{
			strcpy(out_desc[k], "(�����s��)");
			out_color[k] = TERM_L_DARK;
		}
		else if (i >= INVEN_RIGHT && i <= INVEN_LEFT && p_ptr->pclass == CLASS_JYOON)
		{
			strcpy(out_desc[k], "(�����s��)");
			out_color[k] = TERM_L_DARK;
		}
		else if(i == INVEN_FEET && (p_ptr->muta3 & MUT3_FISH_TAIL || p_ptr->prace == RACE_NINGYO || p_ptr->pclass == CLASS_TOZIKO || p_ptr->pclass == CLASS_KISUME))
		{
			strcpy(out_desc[k], "(�����s��)");
			out_color[k] = TERM_L_DARK;
		}
		else
		{
			(void)strcpy(out_desc[k], o_name);
			out_color[k] = tval_to_attr[o_ptr->tval % 128];
		}

		out_index[k] = i;
		/* Grey out charging items */
		if (o_ptr->timeout)
		{
			out_color[k] = TERM_L_DARK;
		}

		/* Extract the maximal length (see below) */
#ifdef JP
		l = strlen(out_desc[k]) + (2 + 1);
#else
		l = strlen(out_desc[k]) + (2 + 3);
#endif


		/* Increase length for labels (if needed) */
#ifdef JP
		if (show_labels) l += (7 + 2);
#else
		if (show_labels) l += (14 + 2);
#endif


		/* Increase length for weight (if needed) */
		if (show_weights) l += 9;

		if (show_item_graph) l += 2;

		/* Maintain the max-length */
		if (l > len) len = l;

		/* Advance the entry */
		k++;
	}

	/* Hack -- Find a column to start in */
#ifdef JP
	col = (len > wid - 6) ? 0 : (wid - len - 1);
#else
	col = (len > wid - 4) ? 0 : (wid - len - 1);
#endif

	prepare_label_string(equip_label, USE_EQUIP);

	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		/* Get the index */
		i = out_index[j];

		/* Get the item */
		o_ptr = &inventory[i];

		/* Clear the line */
		prt("", j + 1, col ? col - 2 : col);

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "�t");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "  ");
		}
		else if (i >= INVEN_RARM)
		{
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", equip_label[i - INVEN_RARM]);
		}
		else /* Paranoia */
		{
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", index_to_label(i));
		}

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j+1, col);

		cur_col = col + 3;

		/* Display graphics for object, if desired */
		if (show_item_graph)
		{
			byte a = object_attr(o_ptr);
			char c = object_char(o_ptr);

#ifdef AMIGA
			if (a & 0x80) a |= 0x40;
#endif

			Term_queue_bigchar(cur_col, j + 1, a, c, 0, 0);
			if (use_bigtile) cur_col++;

			cur_col += 2;
		}

		/* Use labels */
		if (show_labels)
		{
			/* Mention the use */
#ifdef JP
			(void)sprintf(tmp_val, "%-7s: ", mention_use(i));
#else
			(void)sprintf(tmp_val, "%-14s: ", mention_use(i));
#endif

			put_str(tmp_val, j+1, cur_col);

			/* Display the entry itself */
#ifdef JP
			c_put_str(out_color[j], out_desc[j], j+1, cur_col + 9);
#else
			c_put_str(out_color[j], out_desc[j], j+1, cur_col + 16);
#endif
		}

		/* No labels */
		else
		{
			/* Display the entry itself */
			c_put_str(out_color[j], out_desc[j], j+1, cur_col);
		}

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			(void)sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			(void)sprintf(tmp_val, "%3d.%d lb", wgt / 10, wgt % 10);
#endif

			prt(tmp_val, j + 1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	/* Save the new column */
	command_gap = col;

	return target_item_label;
}




/*
 * Flip "inven" and "equip" in any sub-windows
 */
/*:::�T�u�E�B���h�E�̑����ꗗ�ƃU�b�N�ꗗ��؂�ւ���*/
/*:::angband_term[0-7]�𑖍����A
 *:::window_flag[j]&PW_INVEN�Ȃ�PW_EQUIP�ɐ؂�ւ�
 *:::window_flag[j]&PW_EQUIP�Ȃ�PW_INVEN�ɐ؂�ւ�
 *:::p_ptr->window |= PW_EQUIP(��������PW_INVEN)���s���ĕ`��t���O�𗧂Ă�
 */
void toggle_inven_equip(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		/* Unused */
		if (!angband_term[j]) continue;

		/* Flip inven to equip */
		if (window_flag[j] & (PW_INVEN))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_INVEN);
			window_flag[j] |= (PW_EQUIP);

			/* Window stuff */
			p_ptr->window |= (PW_EQUIP);
		}

		/* Flip inven to equip */
		else if (window_flag[j] & (PW_EQUIP))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_EQUIP);
			window_flag[j] |= (PW_INVEN);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN);
		}
	}
}



/*
 * Verify the choice of an item.
 *
 * The item can be negative to mean "item on floor".
 */
static bool verify(cptr prompt, int item)
{
	char        o_name[MAX_NLEN];
	char        out_val[MAX_NLEN+20];
	object_type *o_ptr;


	/* Inventory */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Floor */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Describe */
	object_desc(o_name, o_ptr, 0);

	/* Prompt */
#ifdef JP
(void)sprintf(out_val, "%s%s�ł���? ", prompt, o_name);
#else
	(void)sprintf(out_val, "%s %s? ", prompt, o_name);
#endif


	/* Query */
	return (get_check(out_val));
}


/*
 * Hack -- allow user to "prevent" certain choices
 *
 * The item can be negative to mean "item on floor".
 */
//!q�Ȃǂ̖��ɂ��`�F�b�N�Bn�Ń��s�[�g�����Ƃ��͂�����ʂ�Ȃ��B

static bool get_item_allow(int item)
{
	cptr s;

	object_type *o_ptr;

	if (!command_cmd) return TRUE; /* command_cmd is no longer effective */

	/* Inventory */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Floor */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* No inscription */
	if (!o_ptr->inscription) return (TRUE);

	/* Find a '!' */
	s = my_strchr(quark_str(o_ptr->inscription), '!');

	/* Process preventions */
	while (s)
	{
		/* Check the "restriction" */
		if ((s[1] == command_cmd) || (s[1] == '*'))
		{
			/* Verify the choice */
#ifdef JP
if (!verify("�{����", item)) return (FALSE);
#else
			if (!verify("Really try", item)) return (FALSE);
#endif

		}

		/* Find another '!' */
		s = my_strchr(s + 1, '!');
	}

	/* Allow it */
	return (TRUE);
}



/*
 * Auxiliary function for "get_item()" -- test an index
 */
/*:::�����i�ƃU�b�N�ɑ΂���item_tester�̔�����s��*/
static bool get_item_okay(int i)
{
	/* Illegal items */
	if ((i < 0) || (i >= INVEN_TOTAL)) return (FALSE);

	///pend �����K���ɒe�������댯��������Ȃ�
	if(check_invalidate_inventory(i)) return (FALSE);

	/*:::���̕ϐ���TRUE�̂Ƃ��Ai�����w�̂Ƃ��̂ݖ�������TRUE tester���ʂ�Ȃ�*/
	if (select_ring_slot) return is_ring_slot(i);

	/* Verify the item */
	if (!item_tester_okay(&inventory[i])) return (FALSE);

	/* Assume okay */
	return (TRUE);
}



/*
 * Determine whether get_item() can get some item or not
 * assuming mode = (USE_EQUIP | USE_INVEN | USE_FLOOR).
 */
/*:::get_item()�ŃA�C�e���̌�₪���邩�ǂ�����ɔ��肷��*/
bool can_get_item(void)
{
	int j, floor_list[23], floor_num = 0;

	for (j = 0; j < INVEN_TOTAL; j++)
		if (item_tester_okay(&inventory[j]))
			return TRUE;

	floor_num = scan_floor(floor_list, py, px, 0x03);
	if (floor_num)
		return TRUE;

	return FALSE;
}

/*
 * Let the user select an item, save its "index"
 *:::���[�U�ɃA�C�e����I�������A���̃A�C�e���̃C���f�b�N�X��*cp�ɕۑ�
 *
 * Return TRUE only if an acceptable item was chosen by the user.
 *:::�K�؂ȃA�C�e�����I�����ꂽ�Ƃ��֐��̖߂�l��TRUE
 *
 * The selected item must satisfy the "item_tester_hook()" function,
 * if that hook is set, and the "item_tester_tval", if that value is set.
 *:::�I�����ꂽ�A�C�e����item_tester_hook()�̃`�F�b�N��TRUE�ɂȂ�K�v������
 *
 * All "item_tester" restrictions are cleared before this function returns.
 *:::item_tester�֌W�̕ϐ��͊֐����I������Ƃ������������
 *
 * The user is allowed to choose acceptable items from the equipment,
 * inventory, or floor, respectively, if the proper flag was given,
 * and there are any acceptable items in that location.
 *:::�t���O�̎w�肪����Ă��Ă��̏ꏊ�ɃA�C�e��������Ȃ�A�U�b�N�A�����A��������A�C�e����I���ł���
 *
 * The equipment or inventory are displayed (even if no acceptable
 * items are in that location) if the proper flag was given.
 *:::�����i�ƃU�b�N�́A�t���O���ݒ肳��Ă�ΑΏۃA�C�e�����Ȃ��Ă��\�������E�E�E�H
 *
 * If there are no acceptable items available anywhere, and "str" is
 * not NULL, then it will be used as the text of a warning message
 * before the function returns.
 *:::�ǂ��ɂ��K�؂ȃA�C�e�����Ȃ��Ƃ���*str�̕����񂪕\�������
 *
 * Note that the user must press "-" to specify the item on the floor,
 * and there is no way to "examine" the item on the floor, while the
 * use of "capital" letters will "examine" an inventory/equipment item,
 * and prompt for its use.
 *:::-����͂���Ώ�����Q�Ƃł���B���̊֐����s����I�R�}���h���g�p�s�ł���i�H�j
 *
 * If a legal item is selected from the inventory, we save it in "cp"
 * directly (0 to 35), and return TRUE.
 *:::�U�b�N�Ƒ�������A�C�e�����I�����ꂽ�Ƃ��A0-35�̃C���x���g���ԍ���cp�Ɋi�[�����
 *
 * If a legal item is selected from the floor, we save it in "cp" as
 * a negative (-1 to -511), and return TRUE.
 *:::���ォ��A�C�e�����I�����ꂽ�Ƃ��A����A�C�e����o_list[]�̔ԍ��H�����̒l��cp�Ɋi�[�����
 *
 * If no item is available, we do nothing to "cp", and we display a
 * warning message, using "str" if available, and return FALSE.
 *:::�L���ȃA�C�e�����Ȃ��ꍇ�Astr���\������cp�ɂ͂Ȃɂ�����FALSE���Ԃ�
 *
 * If no item is selected, we do nothing to "cp", and return FALSE.
 *:::�I�����L�����Z�����ꂽ�ꍇ�Acp�ɂ͉�������FALSE���Ԃ�
 *
 * Global "p_ptr->command_new" is used when viewing the inventory or equipment
 * to allow the user to enter a command while viewing those screens, and
 * also to induce "auto-enter" of stores, and other such stuff.
 *:::�R�}���h���s���ɕʂ̃R�}���h�Ɉڂ邱�Ƃ��\�@command_new���g����@�悭�����
 *
 * Global "p_ptr->command_see" may be set before calling this function to start
 * out in "browse" mode.  It is cleared before this function returns.
 *:::browse���[�h�̂Ƃ�command_see���L���ɂȂ��Ă邱�Ƃ�����A�֐��I�����ɏ����������H������悭�����
 *
 * Global "p_ptr->command_wrk" is used to choose between equip/inven listings.
 * If it is TRUE then we are viewing inventory, else equipment.
 *:::command_wrk�����ݕ\������Ă��郊�X�g�̏ꏊ�𔻕ʂ���̂Ɏg����
 *
 * We always erase the prompt when we are done, leaving a blank line,
 * or a warning message, if appropriate, if no items are available.
 *:::�������v�����v�g�͏��������
 */
/*:::�v�����v�g��\�����A�C�e����I��������@�\������A�C�e���̎�ނ�item_tester_hook�ȂǂŎw�肷��@�ڍז���*/
/*:::�֐��ɂ��Ă̏������݂�get_item_floor()��*/
bool get_item(int *cp, cptr pmt, cptr str, int mode)
{
	s16b this_o_idx, next_o_idx = 0;

	char which = ' ';

	int j, k, i1, i2, e1, e2;

	bool done, item;

	bool oops = FALSE;

	bool equip = FALSE;
	bool inven = FALSE;
	bool floor = FALSE;

	bool allow_floor = FALSE;

	bool toggle = FALSE;

	char tmp_val[160];
	char out_val[160];

	/* See cmd5.c */
	extern bool select_the_force;

	int menu_line = (use_menu ? 1 : 0);
	int max_inven = 0;
	int max_equip = 0;

	//v1.1.35
	static int prev_cp = 0;
	bool check_same_item = FALSE;

#ifdef ALLOW_REPEAT

	static char prev_tag = '\0';
	char cur_tag = '\0';

#endif /* ALLOW_REPEAT */

#ifdef ALLOW_EASY_FLOOR /* TNB */

	if (easy_floor || use_menu) return get_item_floor(cp, pmt, str, mode);

#endif /* ALLOW_EASY_FLOOR -- TNB */

	/* Extract args */
	if (mode & USE_EQUIP) equip = TRUE;
	if (mode & USE_INVEN) inven = TRUE;
	if (mode & USE_FLOOR) floor = TRUE;



#ifdef ALLOW_REPEAT

	/* Get the item index */
	if (repeat_pull(cp))
	{

		/* the_force */
		if (select_the_force && (*cp == INVEN_FORCE))
		{
			item_tester_tval = 0;
			item_tester_hook = NULL;
			command_cmd = 0; /* Hack -- command_cmd is no longer effective */
			return (TRUE);
		}

		/* Floor item? */
		else if (floor && (*cp < 0))
		{
			object_type *o_ptr;

			/* Special index */
			k = 0 - (*cp);

			/* Acquire object */
			o_ptr = &o_list[k];

			/* Validate the item */
			if (item_tester_okay(o_ptr))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				item_tester_hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}

		else if ((inven && (*cp >= 0) && (*cp < INVEN_PACK)) ||
		         (equip && (*cp >= INVEN_RARM) && (*cp < INVEN_TOTAL)))
		{



			if (prev_tag && command_cmd)
			{
				/* Look up the tag and validate the item */
				if (!get_tag(&k, prev_tag, (*cp >= INVEN_RARM) ? USE_EQUIP : USE_INVEN)) /* Reject */;
				else if ((k < INVEN_RARM) ? !inven : !equip) /* Reject */;
				else if (!get_item_okay(k)) /* Reject */;
				else
				{
					/* Accept that choice */
					(*cp) = k;

					/* Forget restrictions */
					item_tester_tval = 0;
					item_tester_hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					/* Success */
					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Verify the item */
			else if (get_item_okay(*cp))
			{
//���s�[�g���ɂ͂�����ʂ�B
				/* Forget restrictions */
				item_tester_tval = 0;
				item_tester_hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}
	}

#endif /* ALLOW_REPEAT */


	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Not done */
	done = FALSE;

	/* No item selected */
	item = FALSE;


	/* Full inventory */
	i1 = 0;
	i2 = INVEN_PACK - 1;

	/* Forbid inventory */
	if (!inven) i2 = -1;
	else if (use_menu)
	{
		for (j = 0; j < INVEN_PACK; j++)
			if (item_tester_okay(&inventory[j])) max_inven++;
	}

	/* Restrict inventory indexes */
	while ((i1 <= i2) && (!get_item_okay(i1))) i1++;
	while ((i1 <= i2) && (!get_item_okay(i2))) i2--;


	/* Full equipment */
	e1 = INVEN_RARM;
	e2 = INVEN_TOTAL - 1;

	/* Forbid equipment */
	if (!equip) e2 = -1;
	else if (use_menu)
	{
		for (j = INVEN_RARM; j < INVEN_TOTAL; j++)
			if (select_ring_slot ? is_ring_slot(j) : item_tester_okay(&inventory[j])) max_equip++;
		if (p_ptr->ryoute && !item_tester_no_ryoute) max_equip++;
	}

	/* Restrict equipment indexes */
	while ((e1 <= e2) && (!get_item_okay(e1))) e1++;
	while ((e1 <= e2) && (!get_item_okay(e2))) e2--;

	if (equip && p_ptr->ryoute && !item_tester_no_ryoute)
	{
		if (p_ptr->migite)
		{
			if (e2 < INVEN_LARM) e2 = INVEN_LARM;
		}
		else if (p_ptr->hidarite) e1 = INVEN_RARM;
	}


	/* Restrict floor usage */
	if (floor)
	{
		/* Scan all objects in the grid */
		for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Accept the item on the floor if legal */
			if (item_tester_okay(o_ptr) && (o_ptr->marked & OM_FOUND)) allow_floor = TRUE;
		}
	}

	/* Require at least one legal choice */
	if (!allow_floor && (i1 > i2) && (e1 > e2))
	{
		/* Cancel p_ptr->command_see */
		command_see = FALSE;

		/* Oops */
		oops = TRUE;

		/* Done */
		done = TRUE;

		if (select_the_force) {
		    *cp = INVEN_FORCE;
		    item = TRUE;
		}
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if (command_see && command_wrk && equip)
		{
			command_wrk = TRUE;
		}

		/* Use inventory if allowed */
		else if (inven)
		{
			command_wrk = FALSE;
		}

		/* Use equipment if allowed */
		else if (equip)
		{
			command_wrk = TRUE;
		}

		/* Use inventory for floor */
		else
		{
			command_wrk = FALSE;
		}
	}


	/*
	 * �ǉ��I�v�V����(always_show_list)���ݒ肳��Ă���ꍇ�͏�Ɉꗗ��\������
	 */
	if ((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if (command_see)
	{
		/* Save screen */
		screen_save();
	}


	/* Repeat until done */
	while (!done)
	{
		int get_item_label = 0;

		/* Show choices */
		int ni = 0;
		int ne = 0;

		/* Scan windows */
		for (j = 0; j < 8; j++)
		{
			/* Unused */
			if (!angband_term[j]) continue;

			/* Count windows displaying inven */
			if (window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if (window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if ((command_wrk && ni && !ne) ||
		    (!command_wrk && !ni && ne))
		{
			/* Toggle */
			toggle_inven_equip();

			/* Track toggles */
			toggle = !toggle;
		}

		/* Update */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Redraw windows */
		window_stuff();


		/* Inventory screen */
		if (!command_wrk)
		{
			/* Redraw if needed */
			if (command_see) get_item_label = show_inven(menu_line);
		}

		/* Equipment screen */
		else
		{
			/* Redraw if needed */
			if (command_see) get_item_label = show_equip(menu_line);
		}

		/* Viewing inventory */
		if (!command_wrk)
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "������:");
#else
			sprintf(out_val, "Inven:");
#endif

			/* Some legal items */
			if ((i1 <= i2) && !use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(i1), index_to_label(i2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
#ifdef JP
			if (equip) strcat(out_val, format(" %s �����i,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if (equip) strcat(out_val, format(" %s for Equip,", use_menu ? "4 or 6" : "/"));
#endif
		}

		/* Viewing equipment */
		else
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "�����i:");
#else
			sprintf(out_val, "Equip:");
#endif

			/* Some legal items */
			if ((e1 <= e2) && !use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(e1), index_to_label(e2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see) strcat(out_val, " * to see,");
#endif

			/* Append */
#ifdef JP
			if (inven) strcat(out_val, format(" %s ������,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if (inven) strcat(out_val, format(" %s for Inven,", use_menu ? "4 or 6" : "'/'"));
#endif
		}

		/* Indicate legality of the "floor" item */
		///class
#ifdef JP
		if (allow_floor) strcat(out_val, " '-'����,");
		if (select_the_force) strcat(out_val, " 'w'���C�p,");
#else
		if (allow_floor) strcat(out_val, " - for floor,");
		if (select_the_force) strcat(out_val, " w for the Force,");
#endif

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);

		/* Get a key */
		which = inkey();

		if (use_menu)
		{
		int max_line = (command_wrk ? max_equip : max_inven);
		switch (which)
		{
			case ESCAPE:
			case 'z':
			case 'Z':
			case '0':
			{
				done = TRUE;
				break;
			}

			case '8':
			case 'k':
			case 'K':
			{
				menu_line += (max_line - 1);
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
			case '6':
			case 'h':
			case 'H':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if (!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;
				max_line = (command_wrk ? max_equip : max_inven);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if (command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if (!get_item_okay(get_item_label))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if (!get_item_allow(get_item_label))
					{
						done = TRUE;
						break;
					}

					/* Accept that choice */
					(*cp) = get_item_label;
				}

				item = TRUE;
				done = TRUE;
				break;
			}
			case 'w':
			{
				if (select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}
			}
		}
		if (menu_line > max_line) menu_line -= max_line;
		}
		else
		{
		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				/* Hide the list */
				if (command_see)
				{
					/* Flip flag */
					command_see = FALSE;

					/* Load screen */
					screen_load();
				}

				/* Show the list */
				else
				{
					/* Save screen */
					screen_save();

					/* Flip flag */
					command_see = TRUE;
				}
				break;
			}

			case '/':
			{
				/* Verify legality */
				if (!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;

				/* Need to redraw */
				break;
			}

			case '-':
			{
				/* Use floor item */
				if (allow_floor)
				{
					/* Scan all objects in the grid */
					for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
					{
						object_type *o_ptr;

						/* Acquire object */
						o_ptr = &o_list[this_o_idx];

						/* Acquire next object */
						next_o_idx = o_ptr->next_o_idx;

						/* Validate the item */
						if (!item_tester_okay(o_ptr)) continue;

						/* Special index */
						k = 0 - this_o_idx;

						/* Verify the item (if required) */
#ifdef JP
if (other_query_flag && !verify("�{����", k)) continue;
#else
						if (other_query_flag && !verify("Try", k)) continue;
#endif


						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(k)) continue;

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						break;
					}

					/* Outer break */
					if (done) break;
				}

				/* Oops */
				bell();
				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				/* Look up the tag */
				if (!get_tag(&k, which, command_wrk ? USE_EQUIP : USE_INVEN))
				{
					bell();
					break;
				}

				/* Hack -- Validate the item */
				if ((k < INVEN_RARM) ? !inven : !equip)
				{
					bell();
					break;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
#ifdef ALLOW_REPEAT
				cur_tag = which;
#endif /* ALLOW_REPEAT */
				break;
			}

#if 0
			case '\n':
			case '\r':
			{
				/* Choose "default" inventory item */
				if (!command_wrk)
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
#endif

			case 'w':
			{
				if (select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}

				/* Fall through */
			}

			default:
			{
				int ver;
				bool not_found = FALSE;

				/* Look up the alphabetical tag */
				if (!get_tag(&k, which, command_wrk ? USE_EQUIP : USE_INVEN))
				{
					not_found = TRUE;
				}

				/* Hack -- Validate the item */
				else if ((k < INVEN_RARM) ? !inven : !equip)
				{
					not_found = TRUE;
				}

				/* Validate the item */
				else if (!get_item_okay(k))
				{
					not_found = TRUE;
				}

				if (!not_found)
				{
					/* Accept that choice */
					(*cp) = k;
					item = TRUE;
					done = TRUE;
#ifdef ALLOW_REPEAT
					cur_tag = which;
#endif /* ALLOW_REPEAT */
					break;
				}

				/* Extract "query" setting */
				ver = isupper(which);
				which = tolower(which);

				/* Convert letter to inventory index */
				if (!command_wrk)
				{
					if (which == '(') k = i1;
					else if (which == ')') k = i2;
					else k = label_to_inven(which);
				}

				/* Convert letter to equipment index */
				else
				{
					if (which == '(') k = e1;
					else if (which == ')') k = e2;
					else k = label_to_equip(which);
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
if (ver && !verify("�{����", k))
#else
				if (ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
		}
	}


	/* Fix the screen if necessary */
	if (command_see)
	{
		/* Load screen */
		screen_load();

		/* Hack -- Cancel "display" */
		command_see = FALSE;
	}


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	item_tester_no_ryoute = FALSE;

	/* Forget the item_tester_hook restriction */
	item_tester_hook = NULL;


	/* Clean up  'show choices' */
	/* Toggle again if needed */
	if (toggle) toggle_inven_equip();

	/* Update */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff();


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if (oops && str) msg_print(str);

	if (item)
	{
#ifdef ALLOW_REPEAT
		repeat_push(*cp);
		if (command_cmd) prev_tag = cur_tag;
#endif /* ALLOW_REPEAT */

		command_cmd = 0; /* Hack -- command_cmd is no longer effective */

		//v1.1.35
		prev_cp = *cp;
	}

	/* Result */
	return (item);
}


#ifdef ALLOW_EASY_FLOOR

/*
 * scan_floor --
 *
 * Return a list of o_list[] indexes of items at the given cave
 * location. Valid flags are:
 *
 *		mode & 0x01 -- Item tester
 *		mode & 0x02 -- Marked items only
 *		mode & 0x04 -- Stop after first
 */
/*:::�w��O���b�h��mode�̏����𖞂����A�C�e����T���A����o_list[]�ԍ���int�z��Ɋi�[*/
int scan_floor(int *items, int y, int x, int mode)
{
	int this_o_idx, next_o_idx;

	int num = 0;

	/* Sanity */
	if (!in_bounds(y, x)) return 0;

	/* Scan all objects in the grid */
	for (this_o_idx = cave[y][x].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Item tester */
		if ((mode & 0x01) && !item_tester_okay(o_ptr)) continue;

		/* Marked */
		if ((mode & 0x02) && !(o_ptr->marked & OM_FOUND)) continue;

		/* Accept this item */
		/* XXX Hack -- Enforce limit */
		/*:::����23�Ƃ��������͈�x�ɕ\���ł���������*/
		if (num < 23)
			items[num] = this_o_idx;

		num++;

		/* Only one */
		if (mode & 0x04) break;
	}

	/* Result */
	return num;
}


/*
 * Display a list of the items on the floor at the given location.
 */
int show_floor(int target_item, int y, int x, int *min_width)
{
	int i, j, k, l;
	int col, len;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char tmp_val[80];

	int out_index[23];
	byte out_color[23];
	char out_desc[23][MAX_NLEN];
	int target_item_label = 0;

	int floor_list[23], floor_num;
	int wid, hgt;
	char floor_label[52 + 1];

	bool dont_need_to_show_weights = TRUE;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Default length */
	len = MAX((*min_width), 20);


	/* Scan for objects in the grid, using item_tester_okay() */
	floor_num = scan_floor(floor_list, y, x, 0x03);

	/* Display the floor objects */
	for (k = 0, i = 0; i < floor_num && i < 23; i++)
	{
		o_ptr = &o_list[floor_list[i]];

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);

		/* Save the index */
		out_index[k] = i;

		/* Acquire inventory color */
		out_color[k] = tval_to_attr[o_ptr->tval & 0x7F];

		/* Save the object description */
		strcpy(out_desc[k], o_name);

		/* Find the predicted "line length" */
		l = strlen(out_desc[k]) + 5;

		/* Be sure to account for the weight */
		if (show_weights) l += 9;

		if (o_ptr->tval != TV_GOLD) dont_need_to_show_weights = FALSE;

		/* Maintain the maximum length */
		if (l > len) len = l;

		/* Advance to next "line" */
		k++;
	}

	if (show_weights && dont_need_to_show_weights) len -= 9;

	/* Save width */
	*min_width = len;

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);

	prepare_label_string_floor(floor_label, floor_list, floor_num);

	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		/* Get the index */
		i = floor_list[out_index[j]];

		/* Get the item */
		o_ptr = &o_list[i];

		/* Clear the line */
		prt("", j + 1, col ? col - 2 : col);

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "�t");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "   ");
		}
		else
		{
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", floor_label[j]);
		}

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j + 1, col);

		/* Display the entry itself */
		c_put_str(out_color[j], out_desc[j], j + 1, col + 3);

		/* Display the weight if needed */
		if (show_weights && (o_ptr->tval != TV_GOLD))
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			prt(tmp_val, j + 1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	return target_item_label;
}

/*
 * This version of get_item() is called by get_item() when
 * the easy_floor is on.
 */
bool get_item_floor(int *cp, cptr pmt, cptr str, int mode)
{
	char n1 = ' ', n2 = ' ', which = ' ';

	int j, k, i1, i2, e1, e2;

	bool done, item;

	bool oops = FALSE;

	/* Extract args */
	bool equip = (mode & USE_EQUIP) ? TRUE : FALSE;
	bool inven = (mode & USE_INVEN) ? TRUE : FALSE;
	bool floor = (mode & USE_FLOOR) ? TRUE : FALSE;

	bool allow_equip = FALSE;
	bool allow_inven = FALSE;
	bool allow_floor = FALSE;

	bool toggle = FALSE;

	char tmp_val[160];
	char out_val[160];

	int floor_num, floor_list[23], floor_top = 0;
	int min_width = 0;

	extern bool select_the_force;

	int menu_line = (use_menu ? 1 : 0);
	int max_inven = 0;/*:::�U�b�N�̒���tester��ʂ����A�C�e����*/
	int max_equip = 0;

#ifdef ALLOW_REPEAT

	static char prev_tag = '\0';
	char cur_tag = '\0';

	/* Get the item index */
	/*:::repeat__key[]�ɐ��l���o�^����Ă���Ƃ�*/
	if (repeat_pull(cp))
	{
		/* the_force */
		///class
		if (select_the_force && (*cp == INVEN_FORCE))
		{
			item_tester_tval = 0;
			item_tester_hook = NULL;
			command_cmd = 0; /* Hack -- command_cmd is no longer effective */
			return (TRUE);
		}

		/* Floor item? */
		else if (floor && (*cp < 0))
		{
			/*:::�^�O�̂���A�C�e�����g���R�}���h�����s�[�g�����ꍇ�E�E�H*/
			if (prev_tag && command_cmd)
			{
				/* Scan all objects in the grid */
				/*:::����̃A�C�e������item_teseter�Ɉ�v����A�C�e�������X�g������*/
				floor_num = scan_floor(floor_list, py, px, 0x03);

				/* Look up the tag */
				/*:::���ォ��^�O�̈�v����A�C�e����T���H*/
				if (get_tag_floor(&k, prev_tag, floor_list, floor_num))
				{
					/* Accept that choice */
					(*cp) = 0 - floor_list[k];

					/* Forget restrictions */
					item_tester_tval = 0;
					item_tester_hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					/* Success */
					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Validate the item */
			else if (item_tester_okay(&o_list[0 - (*cp)]))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				item_tester_hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}
		/*:::�U�b�N�Ƒ����i*/
		else if ((inven && (*cp >= 0) && (*cp < INVEN_PACK)) ||
		         (equip && (*cp >= INVEN_RARM) && (*cp < INVEN_TOTAL)))
		{
			if (prev_tag && command_cmd)
			{
				/* Look up the tag and validate the item */
				if (!get_tag(&k, prev_tag, (*cp >= INVEN_RARM) ? USE_EQUIP : USE_INVEN)) /* Reject */;
				else if ((k < INVEN_RARM) ? !inven : !equip) /* Reject */;
				else if (!get_item_okay(k)) /* Reject */;
				else
				{
					/* Accept that choice */
					(*cp) = k;

					/* Forget restrictions */
					item_tester_tval = 0;
					item_tester_hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					/* Success */
					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Verify the item */
			else if (get_item_okay(*cp))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				item_tester_hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}
	}

#endif /* ALLOW_REPEAT */


	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Not done */
	done = FALSE;

	/* No item selected */
	item = FALSE;


	/* Full inventory */
	i1 = 0;
	i2 = INVEN_PACK - 1;

	/* Forbid inventory */
	if (!inven) i2 = -1;
	else if (use_menu)
	{
		for (j = 0; j < INVEN_PACK; j++)
			if (item_tester_okay(&inventory[j])) max_inven++;
	}

	/* Restrict inventory indexes */
	while ((i1 <= i2) && (!get_item_okay(i1))) i1++;
	while ((i1 <= i2) && (!get_item_okay(i2))) i2--;


	/* Full equipment */
	e1 = INVEN_RARM;
	e2 = INVEN_TOTAL - 1;

	/* Forbid equipment */
	if (!equip) e2 = -1;
	else if (use_menu)
	{
		for (j = INVEN_RARM; j < INVEN_TOTAL; j++)
			if (select_ring_slot ? is_ring_slot(j) : item_tester_okay(&inventory[j])) max_equip++;
		if (p_ptr->ryoute && !item_tester_no_ryoute) max_equip++;
	}

	/* Restrict equipment indexes */
	/*:::����������tester�ɓK������A�C�e����T���Ă���H*/
	while ((e1 <= e2) && (!get_item_okay(e1))) e1++;
	while ((e1 <= e2) && (!get_item_okay(e2))) e2--;

	if (equip && p_ptr->ryoute && !item_tester_no_ryoute)
	{
		if (p_ptr->migite)
		{
			if (e2 < INVEN_LARM) e2 = INVEN_LARM;
		}
		else if (p_ptr->hidarite) e1 = INVEN_RARM;
	}


	/* Count "okay" floor items */
	floor_num = 0;

	/* Restrict floor usage */
	if (floor)
	{
		/* Scan all objects in the grid */
		floor_num = scan_floor(floor_list, py, px, 0x03);
	}

	/*:::tester�ɍ��v����A�C�e���������A�U�b�N�A���ɂ��ꂼ���ł����邩�̃t���O�炵��*/
	/* Accept inventory */
	if (i1 <= i2) allow_inven = TRUE;

	/* Accept equipment */
	if (e1 <= e2) allow_equip = TRUE;

	/* Accept floor */
	if (floor_num) allow_floor = TRUE;

	/* Require at least one legal choice */
	if (!allow_inven && !allow_equip && !allow_floor)
	{
		/* Cancel p_ptr->command_see */
		command_see = FALSE;

		/* Oops */
		oops = TRUE;

		/* Done */
		done = TRUE;
		///class
		if (select_the_force) {
		    *cp = INVEN_FORCE;
		    item = TRUE;
		}
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if (command_see && (command_wrk == (USE_EQUIP))
			&& allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use inventory if allowed */
		else if (allow_inven)
		{
			command_wrk = (USE_INVEN);
		}

		/* Use equipment if allowed */
		else if (allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use floor if allowed */
		else if (allow_floor)
		{
			command_wrk = (USE_FLOOR);
		}
	}
	/*:::********************************�����܂œǂ�*****/
	/*
	 * �ǉ��I�v�V����(always_show_list)���ݒ肳��Ă���ꍇ�͏�Ɉꗗ��\������
	 */
	if ((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if (command_see)
	{
		/* Save screen */
		screen_save();
	}

	/* Repeat until done */
	while (!done)
	{
		int get_item_label = 0;

		/* Show choices */
		int ni = 0;
		int ne = 0;

		/* Scan windows */
		for (j = 0; j < 8; j++)
		{
			/* Unused */
			if (!angband_term[j]) continue;

			/* Count windows displaying inven */
			if (window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if (window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if ((command_wrk == (USE_EQUIP) && ni && !ne) ||
		    (command_wrk == (USE_INVEN) && !ni && ne))
		{
			/* Toggle */
			toggle_inven_equip();

			/* Track toggles */
			toggle = !toggle;
		}

		/* Update */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Redraw windows */
		window_stuff();

		/* Inventory screen */
		if (command_wrk == (USE_INVEN))
		{
			/* Extract the legal requests */
			n1 = I2A(i1);
			n2 = I2A(i2);

			/* Redraw if needed */
			if (command_see) get_item_label = show_inven(menu_line);
		}

		/* Equipment screen */
		else if (command_wrk == (USE_EQUIP))
		{
			/* Extract the legal requests */
			n1 = I2A(e1 - INVEN_RARM);
			n2 = I2A(e2 - INVEN_RARM);

			/* Redraw if needed */
			if (command_see) get_item_label = show_equip(menu_line);
		}

		/* Floor screen */
		else if (command_wrk == (USE_FLOOR))
		{
			j = floor_top;
			k = MIN(floor_top + 23, floor_num) - 1;

			/* Extract the legal requests */
			n1 = I2A(j - floor_top);
			n2 = I2A(k - floor_top);

			/* Redraw if needed */
			if (command_see) get_item_label = show_floor(menu_line, py, px, &min_width);
		}

		/* Viewing inventory */
		if (command_wrk == (USE_INVEN))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "������:");
#else
			sprintf(out_val, "Inven:");
#endif

			if (!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(i1), index_to_label(i2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
			if (allow_equip)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '/' �����i,");
				else if (allow_floor)
					strcat(out_val, " '6' �����i,");
				else
					strcat(out_val, " '4'or'6' �����i,");
#else
				if (!use_menu)
					strcat(out_val, " / for Equip,");
				else if (allow_floor)
					strcat(out_val, " 6 for Equip,");
				else
					strcat(out_val, " 4 or 6 for Equip,");
#endif
			}

			/* Append */
			if (allow_floor)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '-'����,");
				else if (allow_equip)
					strcat(out_val, " '4' ����,");
				else
					strcat(out_val, " '4'or'6' ����,");
#else
				if (!use_menu)
					strcat(out_val, " - for floor,");
				else if (allow_equip)
					strcat(out_val, " 4 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}
		}

		/* Viewing equipment */
		else if (command_wrk == (USE_EQUIP))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "�����i:");
#else
			sprintf(out_val, "Equip:");
#endif

			if (!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(e1), index_to_label(e2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
			if (allow_inven)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '/' ������,");
				else if (allow_floor)
					strcat(out_val, " '4' ������,");
				else
					strcat(out_val, " '4'or'6' ������,");
#else
				if (!use_menu)
					strcat(out_val, " / for Inven,");
				else if (allow_floor)
					strcat(out_val, " 4 for Inven,");
				else
					strcat(out_val, " 4 or 6 for Inven,");
#endif
			}

			/* Append */
			if (allow_floor)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '-'����,");
				else if (allow_inven)
					strcat(out_val, " '6' ����,");
				else
					strcat(out_val, " '4'or'6' ����,");
#else
				if (!use_menu)
					strcat(out_val, " - for floor,");
				else if (allow_inven)
					strcat(out_val, " 6 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}
		}

		/* Viewing floor */
		else if (command_wrk == (USE_FLOOR))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "����:");
#else
			sprintf(out_val, "Floor:");
#endif

			if (!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',", n1, n2);
#else
				sprintf(tmp_val, " %c-%c,'(',')',", n1, n2);
#endif

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			if (use_menu)
			{
				if (allow_inven && allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4' �����i, '6' ������,");
#else
					strcat(out_val, " 4 for Equip, 6 for Inven,");
#endif
				}
				else if (allow_inven)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' ������,");
#else
					strcat(out_val, " 4 or 6 for Inven,");
#endif
				}
				else if (allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' �����i,");
#else
					strcat(out_val, " 4 or 6 for Equip,");
#endif
				}
			}
			/* Append */
			else if (allow_inven)
			{
#ifdef JP
				strcat(out_val, " '/' ������,");
#else
				strcat(out_val, " / for Inven,");
#endif
			}
			else if (allow_equip)
			{
#ifdef JP
				strcat(out_val, " '/'�����i,");
#else
				strcat(out_val, " / for Equip,");
#endif
			}

			/* Append */
			if (command_see && !use_menu)
			{
#ifdef JP
				strcat(out_val, " Enter ��,");
#else
				strcat(out_val, " Enter for scroll down,");
#endif
			}
		}

		/* Append */
#ifdef JP
		if (select_the_force) strcat(out_val, " 'w'���C�p,");
#else
		if (select_the_force) strcat(out_val, " w for the Force,");
#endif

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);

		/* Get a key */
		which = inkey();

		if (use_menu)
		{
		int max_line = 1;
		if (command_wrk == USE_INVEN) max_line = max_inven;
		else if (command_wrk == USE_EQUIP) max_line = max_equip;
		else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
		switch (which)
		{
			case ESCAPE:
			case 'z':
			case 'Z':
			case '0':
			{
				done = TRUE;
				break;
			}

			case '8':
			case 'k':
			case 'K':
			{
				menu_line += (max_line - 1);
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
				/* Verify legality */
				if (command_wrk == (USE_INVEN))
				{
					if (allow_floor) command_wrk = USE_FLOOR;
					else if (allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (allow_inven) command_wrk = USE_INVEN;
					else if (allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_equip) command_wrk = USE_EQUIP;
					else if (allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if (command_wrk == USE_INVEN) max_line = max_inven;
				else if (command_wrk == USE_EQUIP) max_line = max_equip;
				else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case '6':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if (command_wrk == (USE_INVEN))
				{
					if (allow_equip) command_wrk = USE_EQUIP;
					else if (allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (allow_floor) command_wrk = USE_FLOOR;
					else if (allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_inven) command_wrk = USE_INVEN;
					else if (allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if (command_wrk == USE_INVEN) max_line = max_inven;
				else if (command_wrk == USE_EQUIP) max_line = max_equip;
				else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if (command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if (!get_item_okay(get_item_label))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if (!get_item_allow(get_item_label))
					{
						done = TRUE;
						break;
					}

					/* Accept that choice */
					(*cp) = get_item_label;
				}

				item = TRUE;
				done = TRUE;
				break;
			}
			///class
			case 'w':
			{
				if (select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}
			}
		}
		if (menu_line > max_line) menu_line -= max_line;
		}
		else
		{
		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				/* Hide the list */
				if (command_see)
				{
					/* Flip flag */
					command_see = FALSE;

					/* Load screen */
					screen_load();
				}

				/* Show the list */
				else
				{
					/* Save screen */
					screen_save();

					/* Flip flag */
					command_see = TRUE;
				}
				break;
			}

			case '\n':
			case '\r':
			case '+':
			{
				int i, o_idx;
				cave_type *c_ptr = &cave[py][px];

				if (command_wrk != (USE_FLOOR)) break;

				/* Get the object being moved. */
				o_idx = c_ptr->o_idx;

				/* Only rotate a pile of two or more objects. */
				if (!(o_idx && o_list[o_idx].next_o_idx)) break;

				/* Remove the first object from the list. */
				excise_object_idx(o_idx);

				/* Find end of the list. */
				i = c_ptr->o_idx;
				while (o_list[i].next_o_idx)
					i = o_list[i].next_o_idx;

				/* Add after the last object. */
				o_list[i].next_o_idx = o_idx;

				/* Re-scan floor list */ 
				floor_num = scan_floor(floor_list, py, px, 0x03);

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				break;
			}

			case '/':
			{
				if (command_wrk == (USE_INVEN))
				{
					if (!allow_equip)
					{
						bell();
						break;
					}
					command_wrk = (USE_EQUIP);
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (!allow_inven)
					{
						bell();
						break;
					}
					command_wrk = (USE_INVEN);
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_inven)
					{
						command_wrk = (USE_INVEN);
					}
					else if (allow_equip)
					{
						command_wrk = (USE_EQUIP);
					}
					else
					{
						bell();
						break;
					}
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Need to redraw */
				break;
			}

			case '-':
			{
				if (!allow_floor)
				{
					bell();
					break;
				}

				/*
				 * If we are already examining the floor, and there
				 * is only one item, we will always select it.
				 * If we aren't examining the floor and there is only
				 * one item, we will select it if floor_query_flag
				 * is FALSE.
				 */
				if (floor_num == 1)
				{
					if ((command_wrk == (USE_FLOOR)) || (!carry_query_flag))
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;

						break;
					}
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				command_wrk = (USE_FLOOR);

				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				if (command_wrk != USE_FLOOR)
				{
					/* Look up the tag */
					if (!get_tag(&k, which, command_wrk))
					{
						bell();
						break;
					}

					/* Hack -- Validate the item */
					if ((k < INVEN_RARM) ? !inven : !equip)
					{
						bell();
						break;
					}

					/* Validate the item */
					if (!get_item_okay(k))
					{
						bell();
						break;
					}
				}
				else
				{
					/* Look up the alphabetical tag */
					if (get_tag_floor(&k, which, floor_list, floor_num))
					{
						/* Special index */
						k = 0 - floor_list[k];
					}
					else
					{
						bell();
						break;
					}
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
#ifdef ALLOW_REPEAT
				cur_tag = which;
#endif /* ALLOW_REPEAT */
				break;
			}

#if 0
			case '\n':
			case '\r':
			{
				/* Choose "default" inventory item */
				if (command_wrk == (USE_INVEN))
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else if (command_wrk == (USE_EQUIP))
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Choose "default" floor item */
				else if (command_wrk == (USE_FLOOR))
				{
					if (floor_num == 1)
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
					}
					break;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
#endif

			case 'w':
			{
				if (select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}

				/* Fall through */
			}

			default:
			{
				int ver;

				if (command_wrk != USE_FLOOR)
				{
					bool not_found = FALSE;

					/* Look up the alphabetical tag */
					if (!get_tag(&k, which, command_wrk))
					{
						not_found = TRUE;
					}

					/* Hack -- Validate the item */
					else if ((k < INVEN_RARM) ? !inven : !equip)
					{
						not_found = TRUE;
					}

					/* Validate the item */
					else if (!get_item_okay(k))
					{
						not_found = TRUE;
					}

					if (!not_found)
					{
						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
#ifdef ALLOW_REPEAT
						cur_tag = which;
#endif /* ALLOW_REPEAT */
						break;
					}
				}
				else
				{
					/* Look up the alphabetical tag */
					if (get_tag_floor(&k, which, floor_list, floor_num))
					{
						/* Special index */
						k = 0 - floor_list[k];

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
#ifdef ALLOW_REPEAT
						cur_tag = which;
#endif /* ALLOW_REPEAT */
						break;
					}
				}

				/* Extract "query" setting */
				ver = isupper(which);
				which = tolower(which);

				/* Convert letter to inventory index */
				if (command_wrk == (USE_INVEN))
				{
					if (which == '(') k = i1;
					else if (which == ')') k = i2;
					else k = label_to_inven(which);
				}

				/* Convert letter to equipment index */
				else if (command_wrk == (USE_EQUIP))
				{
					if (which == '(') k = e1;
					else if (which == ')') k = e2;
					else k = label_to_equip(which);
				}

				/* Convert letter to floor index */
				else if (command_wrk == USE_FLOOR)
				{
					if (which == '(') k = 0;
					else if (which == ')') k = floor_num - 1;
					else k = islower(which) ? A2I(which) : -1;
					if (k < 0 || k >= floor_num || k >= 23)
					{
						bell();
						break;
					}

					/* Special index */
					k = 0 - floor_list[k];
				}

				/* Validate the item */
				if ((command_wrk != USE_FLOOR) && !get_item_okay(k))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
if (ver && !verify("�{����", k))
#else
				if (ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
		}
	}

	/* Fix the screen if necessary */
	if (command_see)
	{
		/* Load screen */
		screen_load();

		/* Hack -- Cancel "display" */
		command_see = FALSE;
	}


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	/* Forget the item_tester_hook restriction */
	item_tester_hook = NULL;


	/* Clean up  'show choices' */
	/* Toggle again if needed */
	if (toggle) toggle_inven_equip();

	/* Update */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff();


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if (oops && str) msg_print(str);

	if (item)
	{
#ifdef ALLOW_REPEAT
		repeat_push(*cp);
		if (command_cmd) prev_tag = cur_tag;
#endif /* ALLOW_REPEAT */

		command_cmd = 0; /* Hack -- command_cmd is no longer effective */
	}

	/* Result */
	return (item);
}


static bool py_pickup_floor_aux(void)
{
	s16b this_o_idx;

	cptr q, s;

	int item;

	/* Restrict the choices */
	item_tester_hook = inven_carry_okay;

	/* Get an object */
#ifdef JP
	q = "�ǂ���E���܂����H";
	s = "�����U�b�N�ɂ͏��ɂ���ǂ̃A�C�e��������Ȃ��B";
#else
	q = "Get which item? ";
	s = "You no longer have any room for the objects on the floor.";
#endif

	if (get_item(&item, q, s, (USE_FLOOR)))
	{
		this_o_idx = 0 - item;
	}
	else
	{
		return (FALSE);
	}

	/* Pick up the object */
	py_pickup_aux(this_o_idx);

	return (TRUE);
}


/*
 * Make the player carry everything in a grid
 *
 * If "pickup" is FALSE then only gold will be picked up
 *
 * This is called by py_pickup() when easy_floor is TRUE.
 */
/*:::�����A�C�e����S�ďE���Bpickup��FALSE�Ȃ�������E���B�ڍז���*/
void py_pickup_floor(bool pickup)
{
	s16b this_o_idx, next_o_idx = 0;

	char o_name[MAX_NLEN];
	object_type *o_ptr;

	int floor_num = 0, floor_o_idx = 0;

	int can_pickup = 0;

	/* Scan the pile of objects */
	for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{

		/* Access the object */
		o_ptr = &o_list[this_o_idx];

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);

		/* Access the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			/* Message */
#ifdef JP
		msg_format(" $%ld �̉��l������%s���������B",
			   (long)o_ptr->pval, o_name);
#else
			msg_format("You have found %ld gold pieces worth of %s.",
				(long) o_ptr->pval, o_name);
#endif


			/* Collect the gold */
			p_ptr->au += o_ptr->pval;

			/* Redraw gold */
			p_ptr->redraw |= (PR_GOLD);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);

			/* Delete the gold */
			delete_object_idx(this_o_idx);

			/* Check the next object */
			continue;
		}
		else if (o_ptr->marked & OM_NOMSG)
		{
			/* If 0 or 1 non-NOMSG items are in the pile, the NOMSG ones are
			 * ignored. Otherwise, they are included in the prompt. */
			o_ptr->marked &= ~(OM_NOMSG);
			continue;
		}

		/* Count non-gold objects that can be picked up. */
		if (inven_carry_okay(o_ptr))
		{
			can_pickup++;
		}

		/* Count non-gold objects */
		floor_num++;

		/* Remember this index */
		floor_o_idx = this_o_idx;
	}

	/* There are no non-gold objects */
	if (!floor_num)
		return;

	/* Mention the number of objects */
	if (!pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Access the object */
			o_ptr = &o_list[floor_o_idx];

#ifdef ALLOW_EASY_SENSE

			/* Option: Make object sensing easy */
			if (easy_sense)
			{
				/* Sense the object */
				(void) sense_object(o_ptr);
			}

#endif /* ALLOW_EASY_SENSE */

			/* Describe the object */
			object_desc(o_name, o_ptr, 0);

			/* Message */
#ifdef JP
				msg_format("%s������B", o_name);
#else
			msg_format("You see %s.", o_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("%d �̃A�C�e���̎R������B", floor_num);
#else
			msg_format("You see a pile of %d items.", floor_num);
#endif

		}

		/* Done */
		return;
	}

	/* The player has no room for anything on the floor. */
	if (!can_pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Access the object */
			o_ptr = &o_list[floor_o_idx];

#ifdef ALLOW_EASY_SENSE

			/* Option: Make object sensing easy */
			if (easy_sense)
			{
				/* Sense the object */
				(void) sense_object(o_ptr);
			}

#endif /* ALLOW_EASY_SENSE */

			/* Describe the object */
			object_desc(o_name, o_ptr, 0);

			/* Message */
#ifdef JP
				msg_format("�U�b�N�ɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
#else
			msg_format("You have no room for %s.", o_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("�U�b�N�ɂ͏��ɂ���ǂ̃A�C�e��������Ȃ��B", o_name);
#else
			msg_print("You have no room for any of the objects on the floor.");
#endif

		}

		/* Done */
		return;
	}

	/* One object */
	if (floor_num == 1)
	{
		/* Hack -- query every object */
		if (carry_query_flag)
		{
			char out_val[MAX_NLEN+20];

			/* Access the object */
			o_ptr = &o_list[floor_o_idx];

#ifdef ALLOW_EASY_SENSE

			/* Option: Make object sensing easy */
			if (easy_sense)
			{
				/* Sense the object */
				(void) sense_object(o_ptr);
			}

#endif /* ALLOW_EASY_SENSE */

			/* Describe the object */
			object_desc(o_name, o_ptr, 0);

			/* Build a prompt */
#ifdef JP
			(void) sprintf(out_val, "%s���E���܂���? ", o_name);
#else
			(void) sprintf(out_val, "Pick up %s? ", o_name);
#endif


			/* Ask the user to confirm */
			if (!get_check(out_val))
			{
				/* Done */
				return;
			}
		}

		/* Access the object */
		o_ptr = &o_list[floor_o_idx];

#ifdef ALLOW_EASY_SENSE

		/* Option: Make object sensing easy */
		if (easy_sense)
		{
			/* Sense the object */
			(void) sense_object(o_ptr);
		}

#endif /* ALLOW_EASY_SENSE */

		/* Pick up the object */
		py_pickup_aux(floor_o_idx);
	}

	/* Allow the user to choose an object */
	else
	{
		while (can_pickup--)
		{
			if (!py_pickup_floor_aux()) break;
		}
	}
}

#endif /* ALLOW_EASY_FLOOR */

/*:::��̈�̖��@�������啪�ނɑ�����ꍇTRUE*/
bool same_category_realm(int realm1, int realm2)
{
	if(realm1 >= TV_BOOK_ELEMENT && realm1 <= TV_BOOK_SUMMONS && realm2 >= TV_BOOK_ELEMENT && realm2 <= TV_BOOK_SUMMONS )
		return (TRUE);
	if(realm1 >= TV_BOOK_MYSTIC && realm1 <= TV_BOOK_NATURE && realm2 >= TV_BOOK_MYSTIC && realm2 <= TV_BOOK_NATURE )
		return (TRUE);
	if(realm1 >= TV_BOOK_TRANSFORM && realm1 <= TV_BOOK_CHAOS && realm2 >= TV_BOOK_TRANSFORM && realm2 <= TV_BOOK_CHAOS )
		return (TRUE);

	if(realm1 == TV_BOOK_OCCULT && realm2 == TV_BOOK_OCCULT) return TRUE;

	return (FALSE);

}