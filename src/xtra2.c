/* File: xtra2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: effects of various "objects" */

#include "angband.h"

#define REWARD_CHANCE 10


//11.1.91 �Ζ��n�`�ł̉Ή��_���[�W�㏸����
//�Ή��������ǂ����̔���͊O�ōs��
int oil_field_damage_mod(int src_dam, int y, int x)
{
	int mod_dam;

	//�Ζ��n�`�łȂ���΂��̂܂܋A��
	if (!cave_have_flag_bold(y, x, FF_OIL_FIELD)) return src_dam;

	//�_���[�W+33%
	mod_dam = src_dam * 4 / 3;

	msg_print("�Ζ����R���オ�����I");


	return mod_dam;

}


/*:::�����X�^�[�����̃L�����N�^�[���ǂ�������@*/
/*:::�����L�����N�^�[�̏ꍇ�����X�^�[���\����(2P)�Ƃ��Ă������߂̏���*/
//v1.1.51 monster_hook����g�����ƂɂȂ����̂Ō������̂��߂�switch���ŏ���������
bool monster_is_you(s16b r_idx)
{
	int p = p_ptr->pclass;

	if(!cp_ptr->flag_only_unique) return FALSE;

	switch(p_ptr->pclass)
	{
		case CLASS_RUMIA:
			if (r_idx == MON_RUMIA) return TRUE;
			else return FALSE;

		case CLASS_YOUMU:
			if (r_idx == MON_YOUMU) return TRUE;
			else return FALSE;
		case CLASS_TEWI:
			if (r_idx == MON_TEWI) return TRUE;
			else return FALSE;
		case CLASS_UDONGE:
			if (r_idx == MON_UDONGE) return TRUE;
			else return FALSE;
		case CLASS_IKU:
			if (r_idx == MON_IKU) return TRUE;
			else return FALSE;

		case CLASS_KOMACHI:
			if (r_idx == MON_KOMACHI) return TRUE;
			else return FALSE;
		case CLASS_KOGASA:
			if (r_idx == MON_KOGASA) return TRUE;
			else return FALSE;
		case CLASS_KASEN:
			if (r_idx == MON_KASEN) return TRUE;
			else return FALSE;
		case CLASS_SUIKA:
			if (r_idx == MON_SUIKA) return TRUE;
			else return FALSE;
		case CLASS_WAKASAGI:
			if (r_idx == MON_WAKASAGI) return TRUE;
			else return FALSE;

		case CLASS_KOISHI:
			if (r_idx == MON_KOISHI) return TRUE;
			else return FALSE;
		case CLASS_MOMIZI:
			if (r_idx == MON_MOMIZI) return TRUE;
			else return FALSE;
		case CLASS_SEIGA:
			if (r_idx == MON_SEIGA) return TRUE;
			else return FALSE;
		case CLASS_CIRNO:
			if (r_idx == MON_CIRNO) return TRUE;
			else return FALSE;
		case CLASS_ORIN:
			if (r_idx == MON_ORIN) return TRUE;
			else return FALSE;
		case CLASS_SHINMYOUMARU:
		case CLASS_SHINMYOU_2:
			if (r_idx == MON_SHINMYOUMARU) return TRUE;
			else return FALSE;
		case CLASS_NAZRIN:
			if (r_idx == MON_NAZRIN) return TRUE;
			else return FALSE;
		case CLASS_LETTY:
			if (r_idx == MON_LETTY) return TRUE;
			else return FALSE;
		case CLASS_YOSHIKA:
			if (r_idx == MON_YOSHIKA) return TRUE;
			else if (r_idx == MON_SEIGA) return TRUE; //�M�����O�����Ƃ͂����o�Ă�����Z������̂ŐM��2P�����ɂ��Ă���
			else return FALSE;
		case CLASS_PATCHOULI:
			if (r_idx == MON_PATCHOULI) return TRUE;
			else return FALSE;

		case CLASS_AYA:
			if (r_idx == MON_AYA) return TRUE;
			else return FALSE;
		case CLASS_BANKI:
			if (r_idx == MON_SEKIBANKI) return TRUE;
			else return FALSE;
		case CLASS_MYSTIA:
			if (r_idx == MON_MYSTIA) return TRUE;
			else return FALSE;
		case CLASS_FLAN:
			if (r_idx == MON_FLAN) return TRUE;
			else return FALSE;
		case CLASS_SHOU:
			if (r_idx == MON_SHOU) return TRUE;
			else return FALSE;

		case CLASS_MEDICINE:
			if (r_idx == MON_MEDICINE) return TRUE;
			else return FALSE;
		case CLASS_YUYUKO:
			if (r_idx == MON_YUYUKO) return TRUE;
			else return FALSE;
		case CLASS_SATORI:
			if (r_idx == MON_SATORI) return TRUE;
			else return FALSE;
		case CLASS_KYOUKO:
			if (r_idx == MON_KYOUKO) return TRUE;
			else return FALSE;
		case CLASS_TOZIKO:
			if (r_idx == MON_TOZIKO) return TRUE;
			else return FALSE;

		case CLASS_LILY_WHITE:
			if (r_idx == MON_LILY_WHITE) return TRUE;
			else return FALSE;
		case CLASS_KISUME:
			if (r_idx == MON_KISUME) return TRUE;
			else return FALSE;
		case CLASS_HATATE:
			if (r_idx == MON_HATATE) return TRUE;
			else return FALSE;
		case CLASS_MIKO:
			if (r_idx == MON_MIKO) return TRUE;
			else return FALSE;
		case CLASS_KOKORO:
			if (r_idx == MON_KOKORO) return TRUE;
			else return FALSE;

		case CLASS_WRIGGLE:
			if (r_idx == MON_WRIGGLE) return TRUE;
			else return FALSE;
		case CLASS_YUUKA:
			if (r_idx == MON_YUUKA) return TRUE;
			else return FALSE;
		case CLASS_CHEN:
			if (r_idx == MON_CHEN) return TRUE;
			else return FALSE;
		case CLASS_MURASA:
			if (r_idx == MON_MURASA) return TRUE;
			else return FALSE;
		case CLASS_KEINE:
			if (r_idx == MON_KEINE) return TRUE;
			else return FALSE;

		case CLASS_YUGI:
			if (r_idx == MON_YUGI) return TRUE;
			else return FALSE;
		case CLASS_REIMU:
			if (r_idx == MON_REIMU) return TRUE;
			else return FALSE;
		case CLASS_KAGEROU:
			if (r_idx == MON_KAGEROU) return TRUE;
			else return FALSE;
		case CLASS_SHIZUHA:
			if (r_idx == MON_SHIZUHA) return TRUE;
			else return FALSE;
		case CLASS_SANAE:
			if (r_idx == MON_SANAE) return TRUE;
			else return FALSE;

		case CLASS_MINORIKO:
			if (r_idx == MON_MINORIKO) return TRUE;
			else return FALSE;
		case CLASS_REMY:
			if (r_idx == MON_REMY) return TRUE;
			else return FALSE;
		case CLASS_NITORI:
			if (r_idx == MON_NITORI) return TRUE;
			else return FALSE;
		case CLASS_BYAKUREN:
			if (r_idx == MON_BYAKUREN) return TRUE;
			else return FALSE;
		case CLASS_NUE:
			if (r_idx == MON_NUE) return TRUE;
			else return FALSE;

		case CLASS_UTSUHO:
			if (r_idx == MON_UTSUHO) return TRUE;
			else return FALSE;
		case CLASS_YAMAME:
			if (r_idx == MON_YAMAME) return TRUE;
			else return FALSE;
		case CLASS_SUWAKO:
			if (r_idx == MON_SUWAKO) return TRUE;
			else return FALSE;
		case CLASS_RAN:
			if (r_idx == MON_RAN) return TRUE;
			else return FALSE;
		case CLASS_TENSHI:
			if (r_idx == MON_TENSHI) return TRUE;
			else return FALSE;

		case CLASS_MEIRIN:
			if (r_idx == MON_MEIRIN) return TRUE;
			else return FALSE;
		case CLASS_EIKI:
			if (r_idx == MON_EIKI) return TRUE;
			else return FALSE;
		case CLASS_PARSEE:
			if (r_idx == MON_PARSEE) return TRUE;
			else return FALSE;
		case CLASS_ICHIRIN:
			if (r_idx == MON_ICHIRIN) return TRUE;
			else return FALSE;

		case CLASS_KANAKO:
			if (r_idx == MON_KANAKO) return TRUE;
			else return FALSE;
		case CLASS_FUTO:
			if (r_idx == MON_FUTO) return TRUE;
			else return FALSE;
		case CLASS_SUNNY:
		case CLASS_LUNAR:
		case CLASS_STAR:
		case CLASS_3_FAIRIES:
			if (r_idx == MON_3FAIRIES) return TRUE;
			//v1.1.68 �O�d����p���i�̓`���m�A�����o�A�s�[�X����������
			else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES) && (r_idx == MON_CIRNO || r_idx == MON_CLOWNPIECE || r_idx == MON_LARVA))
				return TRUE;
			else return FALSE;

		case CLASS_ALICE:
			if (r_idx == MON_ALICE) return TRUE;
			else return FALSE;
		case CLASS_LUNASA:
		case CLASS_MERLIN:
		case CLASS_LYRICA:
			if (r_idx == MON_LUNASA || r_idx == MON_MERLIN || r_idx == MON_LYRICA) return TRUE;
			else return FALSE;

		case CLASS_MARISA:
			if (r_idx == MON_MARISA) return TRUE;
			else return FALSE;
		case CLASS_CLOWNPIECE:
		case CLASS_VFS_CLOWNPIECE:
			if (r_idx == MON_CLOWNPIECE) return TRUE;
			else return FALSE;
		case CLASS_DOREMY:
			if (r_idx == MON_DOREMY) return TRUE;
			else return FALSE;
		case CLASS_SAKUYA:
			if (r_idx == MON_SAKUYA) return TRUE;
			else return FALSE;
		case CLASS_HINA:
			if (r_idx == MON_HINA) return TRUE;
			else return FALSE;

		case CLASS_BENBEN:
		case CLASS_YATSUHASHI:
			if (r_idx == MON_BENBEN || r_idx == MON_YATSUHASHI) return TRUE;
			else return FALSE;

		case CLASS_RAIKO:
			if (r_idx == MON_RAIKO) return TRUE;
			else return FALSE;
		case CLASS_MAMIZOU:
			if (r_idx == MON_MAMIZOU) return TRUE;
			else return FALSE;
		case CLASS_YUKARI:
			if (r_idx == MON_YUKARI) return TRUE;
			else return FALSE;
		case CLASS_RINGO:
			if (r_idx == MON_RINGO) return TRUE;
			else return FALSE;
		case CLASS_SEIRAN:
			if (r_idx == MON_SEIRAN) return TRUE;
			else return FALSE;

		case CLASS_EIRIN:
		case CLASS_KAGUYA:
			if (r_idx == MON_EIRIN || r_idx == MON_KAGUYA) return TRUE;
			else return FALSE;

		case CLASS_SAGUME:
			if (r_idx == MON_SAGUME) return TRUE;
			else return FALSE;
		case CLASS_REISEN2:
			if (r_idx == MON_REISEN2) return TRUE;
			else return FALSE;
		case CLASS_TOYOHIME:
			if (r_idx == MON_TOYOHIME) return TRUE;
			else return FALSE;
		case CLASS_YORIHIME:
			if (r_idx == MON_YORIHIME) return TRUE;
			else return FALSE;
		case CLASS_JUNKO:
			if (r_idx == MON_JUNKO) return TRUE;
			else return FALSE;

		case CLASS_HECATIA:
			if (r_idx >= MON_HECATIA1 && r_idx <= MON_HECATIA3) return TRUE;
			else return FALSE;

		case CLASS_NEMUNO:
			if (r_idx == MON_NEMUNO) return TRUE;
			else return FALSE;
		case CLASS_AUNN:
			if (r_idx == MON_AUNN) return TRUE;
			else return FALSE;
		case CLASS_NARUMI:
			if (r_idx == MON_NARUMI) return TRUE;
			else return FALSE;
		case CLASS_LARVA:
			if (r_idx == MON_LARVA) return TRUE;
			else return FALSE;
		case CLASS_MAI:
			if (r_idx == MON_MAI) return TRUE;
			else return FALSE;

		case CLASS_SATONO:
			if (r_idx == MON_SATONO) return TRUE;
			else return FALSE;
		case CLASS_JYOON:
			if (r_idx == MON_JYOON) return TRUE;
			else return FALSE;
		case CLASS_SHION:
			if (r_idx == MON_SHION_1 || r_idx == MON_SHION_2) return TRUE;
			else return FALSE;

		case CLASS_OKINA:
			if (r_idx == MON_OKINA) return TRUE;
			else return FALSE;
		case CLASS_EIKA:
			if (r_idx == MON_EIKA) return TRUE;
			else return FALSE;
		case CLASS_MAYUMI:
			if (r_idx == MON_MAYUMI) return TRUE;
			else return FALSE;

		case CLASS_KUTAKA:
			if (r_idx == MON_KUTAKA) return TRUE;
			else return FALSE;

		case CLASS_URUMI:
			if (r_idx == MON_URUMI) return TRUE;
			else return FALSE;

		case CLASS_SAKI:
			if (r_idx == MON_SAKI) return TRUE;
			else return FALSE;

		case CLASS_KEIKI:
			if (r_idx == MON_KEIKI) return TRUE;
			else return FALSE;

		case CLASS_TAKANE:
			if (r_idx == MON_TAKANE) return TRUE;
			else return FALSE;

		case CLASS_MOMOYO:
			if (r_idx == MON_MOMOYO) return TRUE;
			else return FALSE;

		case CLASS_SANNYO:
			if (r_idx == MON_SANNYO) return TRUE;
			else return FALSE;

		case CLASS_MIKE:
			if (r_idx == MON_MIKE) return TRUE;
			else return FALSE;

		case CLASS_TSUKASA:
			if (r_idx == MON_TSUKASA) return TRUE;
			else return FALSE;

		case CLASS_MEGUMU:
			if (r_idx == MON_MEGUMU) return TRUE;
			else return FALSE;

		case CLASS_MISUMARU:
			if (r_idx == MON_MISUMARU) return TRUE;
			else return FALSE;

		case CLASS_CHIMATA:
			if (r_idx == MON_CHIMATA) return TRUE;
			else return FALSE;


	}


	return FALSE;
}




/*:::��؂̐��_�U���������Ȃ�*/
bool diehardmind()
{
	if(p_ptr->pclass == CLASS_KOISHI) return TRUE;
	if(p_ptr->pclass == CLASS_FLAN) return TRUE;
	if(p_ptr->pclass == CLASS_EIKI) return TRUE;
	if(p_ptr->pclass == CLASS_JUNKO) return TRUE;
	if(prace_is_(RACE_HANIWA))return TRUE;


	return FALSE;
}

//v1.1.15 �w�J�[�e�B�A�̒n���ƌ��̑̂ɐV����T�V���c�����
static void make_heca_t(int body)
{
	object_type forge;
	object_type *o_ptr = &forge;
	object_type *to_ptr;

	//�ς�T�V���c���X�g
	cptr engrish_list[] =
	{
		"All your base are belong to us",
		"Today is under construction",
		"Crap Your Hands",
		"flog",
		"HELS",
		"EXPREMAY DENGERLUS",
		"JUST IT DO",
		"GET KILLED, GET NOTICED",
		"THIS IS NOT HERE",
		"FIGHT PEACEFUL",
		"I rub you",
		"Your Next",
		"�n����",
		"ajides",
		"ocosite",
		"KIBIN",
		"GHB��|�������ǂ���Ȍ���T�V���c������ɓ���Ȃ�������!",

	};

	int old_object_level = object_level;

	//inven_add[]�̑O�������n���̑̂ƌ��߂Ă���B�L�������C�N���̃f�t�H�ݒ�
	if(body == HECATE_BODY_EARTH) 
		to_ptr = &inven_add[INVEN_BODY - INVEN_RARM];
	else 
		to_ptr = &inven_add[INVEN_BODY - INVEN_RARM + 12];

	if(to_ptr->k_idx)
	{
		msg_print("ERROR:make_heca_t�̓��삪��������");
		return;
	}
	object_level = 70;

	do
	{
		object_prep(o_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_T_SHIRT));
		create_artifact(o_ptr, FALSE);
	}while(object_is_cursed(o_ptr));

	remove_flag(o_ptr->art_flags, TR_NO_TELE);
	remove_flag(o_ptr->art_flags, TR_NO_MAGIC);
	o_ptr->to_a = 18 + randint0(6);
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	o_ptr->art_name = quark_add(format("�y%s�z�Ə����ꂽ",engrish_list[randint0(sizeof(engrish_list) / sizeof(cptr))]));

	object_copy(to_ptr,o_ptr);

	object_level = old_object_level;

}

/*
 * Advance experience levels and print experience
 */
/*:::���x���A�b�v�`�F�b�N�ƃ��x���_�E������*/
/*:::���x��10���Ƃ̑����\�͒l�I���A�b�l�̓ˑR�ψفA�J�I�X�p�g����������*/
///sys race ���x���A�b�v�����ƃJ�I�X�p�g��������
void check_experience(void)
{
	bool level_reward = FALSE;
	bool level_mutation = FALSE;
	bool level_inc_stat = FALSE;
	bool android = (p_ptr->prace == RACE_ANDROID ? TRUE : FALSE);
	int  old_lev = p_ptr->lev;


	/* Hack -- lower limit */
	if (p_ptr->exp < 0) p_ptr->exp = 0;
	if (p_ptr->max_exp < 0) p_ptr->max_exp = 0;
	if (p_ptr->max_max_exp < 0) p_ptr->max_max_exp = 0;

	/* Hack -- upper limit */
	if (p_ptr->exp > PY_MAX_EXP) p_ptr->exp = PY_MAX_EXP;
	if (p_ptr->max_exp > PY_MAX_EXP) p_ptr->max_exp = PY_MAX_EXP;
	if (p_ptr->max_max_exp > PY_MAX_EXP) p_ptr->max_max_exp = PY_MAX_EXP;

	/* Hack -- maintain "max" experience */
	if (p_ptr->exp > p_ptr->max_exp) p_ptr->max_exp = p_ptr->exp;

	/* Hack -- maintain "max max" experience */
	if (p_ptr->max_exp > p_ptr->max_max_exp) p_ptr->max_max_exp = p_ptr->max_exp;

	/* Redraw experience */
	p_ptr->redraw |= (PR_EXP);

	/* Handle stuff */
	handle_stuff();


	/* Lose levels while possible */
	while ((p_ptr->lev > 1) &&
	       (p_ptr->exp < ((android ? player_exp_a : player_exp)[p_ptr->lev - 2] * p_ptr->expfact / 100L)))
	{
		/* Lose a level */
		p_ptr->lev--;

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		/* Handle stuff */
		handle_stuff();
	}


	/* Gain levels while possible */
	while ((p_ptr->lev < PY_MAX_LEVEL) &&
	       (p_ptr->exp >= ((android ? player_exp_a : player_exp)[p_ptr->lev-1] * p_ptr->expfact / 100L)))
	{
		int new_deity_rank = 0;//��ǐ_�l�p
		/* Gain a level */
		p_ptr->lev++;

		/* Save the highest level */
		//�ő僌�x�����X�V�����Ƃ��̏���(�o���l�_�E���Ȃǂŉ����������x�����񕜂���Ƃ��ɂ͂����ɂ͗��Ȃ�)
		if (p_ptr->lev > p_ptr->max_plv)
		{
			p_ptr->max_plv = p_ptr->lev;

			/*:::Hack ���P�̕��킪�p���[�A�b�v����*/
			if(p_ptr->pclass == CLASS_KOGASA)
				apply_kogasa_magic(&inventory[INVEN_RARM],p_ptr->max_plv,FALSE);

			//v1.1.21 ���m�̋Z�\�K��
			if(p_ptr->pclass == CLASS_SOLDIER)
			{
				get_soldier_skillpoint();
			}

			//v1.1.52 俎q�V���i�ł̐V�Z�K�� 7���x������
			if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
			{
				if ((p_ptr->lev - 1) % 7 == 0)
				{
					okina_make_nameless_arts(p_ptr->lev / 7);//1-7 0�̓Q�[���J�n���ɏ����ς�
				}
			}

			///mod150103 ��ǐ_�l�����v���X�@���背�x���ŐV�����_�i�ɂȂ�
			set_deity_bias(DBIAS_REPUTATION, 1);
			if(p_ptr->prace == RACE_STRAYGOD && p_ptr->max_plv == 30) new_deity_rank = 2; 
			else if(p_ptr->prace == RACE_STRAYGOD && p_ptr->max_plv == 45) new_deity_rank = 3;

			//v1.1.15 �w�J�[�e�B�A�����x��20�ɂȂ�����n���ƌ��̑̂��g����BT�V���c����
			if(p_ptr->pclass == CLASS_HECATIA && p_ptr->max_plv == 20)
			{
				make_heca_t(HECATE_BODY_EARTH);
				make_heca_t(HECATE_BODY_MOON);
			}

			//if ((p_ptr->pclass == CLASS_CHAOS_WARRIOR) ||
			if ((p_ptr->pclass == CLASS_MAID) ||
			    (p_ptr->muta2 & MUT2_CHAOS_GIFT))
			{
				level_reward = TRUE;
			}
			/*
			if (p_ptr->prace == RACE_BEASTMAN)
			{
				if (one_in_(5)) level_mutation = TRUE;
			}
			*/
			if (p_ptr->pclass == CLASS_LARVA)
			{
				if (one_in_(4)) level_mutation = TRUE;
			}
			level_inc_stat = TRUE;

			do_cmd_write_nikki(NIKKI_LEVELUP, p_ptr->lev, NULL);
		}

		/* Sound */
		sound(SOUND_LEVEL);

		/* Message */
#ifdef JP
msg_format("���x�� %d �ɂ悤�����B", p_ptr->lev);
#else
		msg_format("Welcome to level %d.", p_ptr->lev);

#endif

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE | PR_EXP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER | PW_SPELL | PW_INVEN);

		/* HP��MP�̏㏸�ʂ�\�� */
		level_up = 1;

		/* Handle stuff */
		handle_stuff();

		level_up = 0;

		if (level_inc_stat)
		{
			if(!(p_ptr->max_plv % 10))
			{
				int choice;
				screen_save();
				while(1)
				{
					int n;
					char tmp[32];

#ifdef JP
					cnv_stat(p_ptr->stat_max[0], tmp);
					prt(format("        a) �r�� (���ݒl %s)", tmp), 2, 14);
					cnv_stat(p_ptr->stat_max[1], tmp);
					prt(format("        b) �m�\ (���ݒl %s)", tmp), 3, 14);
					cnv_stat(p_ptr->stat_max[2], tmp);
					prt(format("        c) ���� (���ݒl %s)", tmp), 4, 14);
					cnv_stat(p_ptr->stat_max[3], tmp);
					prt(format("        d) ��p (���ݒl %s)", tmp), 5, 14);
					cnv_stat(p_ptr->stat_max[4], tmp);
					prt(format("        e) �ϋv (���ݒl %s)", tmp), 6, 14);
					cnv_stat(p_ptr->stat_max[5], tmp);
					prt(format("        f) ���� (���ݒl %s)", tmp), 7, 14);
					prt("", 8, 14);
					prt("        �ǂ̔\�͒l���グ�܂����H", 1, 14);
#else
					cnv_stat(p_ptr->stat_max[0], tmp);
					prt(format("        a) Str (cur %s)", tmp), 2, 14);
					cnv_stat(p_ptr->stat_max[1], tmp);
					prt(format("        b) Int (cur %s)", tmp), 3, 14);
					cnv_stat(p_ptr->stat_max[2], tmp);
					prt(format("        c) Wis (cur %s)", tmp), 4, 14);
					cnv_stat(p_ptr->stat_max[3], tmp);
					prt(format("        d) Dex (cur %s)", tmp), 5, 14);
					cnv_stat(p_ptr->stat_max[4], tmp);
					prt(format("        e) Con (cur %s)", tmp), 6, 14);
					cnv_stat(p_ptr->stat_max[5], tmp);
					prt(format("        f) Chr (cur %s)", tmp), 7, 14);
					prt("", 8, 14);
					prt("        Which stat do you want to raise?", 1, 14);
#endif
					while(1)
					{
						choice = inkey();
						if ((choice >= 'a') && (choice <= 'f')) break;
					}
					for(n = 0; n < 6; n++)
						if (n != choice - 'a')
							prt("",n+2,14);
#ifdef JP
					if (get_check("��낵���ł����H")) break;
#else
					if (get_check("Are you sure? ")) break;
#endif
				}
				do_inc_stat(choice - 'a');
				screen_load();
			}
			else if(!(p_ptr->max_plv % 2))
				do_inc_stat(randint0(6));
		}

		if (level_mutation)
		{
#ifdef JP
msg_print("���Ȃ��͕ς�����C������...");
#else
			msg_print("You feel different...");
#endif

			(void)gain_random_mutation(0);
			level_mutation = FALSE;
		}
		//��ǐ_�l�̐_�i�ύX
		if(new_deity_rank)	change_deity_type(new_deity_rank);

		/*
		 * ��V�Ń��x�������ƍċA�I�� check_experience() ��
		 * �Ă΂��̂ŏ��Ԃ��Ō�ɂ���B
		 */
		/*:::�J�I�X�p�g������V*/
		if (level_reward)
		{
			if(p_ptr->muta2 & MUT2_CHAOS_GIFT) gain_level_reward(0);
			else gain_maid_reward();
			level_reward = FALSE;
		}

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER | PW_SPELL);

		/* Handle stuff */
		handle_stuff();
	}
	///mod140913 ���ԋt�]�H������Ƃ������x���܂ł̕K�v�o���l�\���̍ĕ`�悪�x���̂Œǉ�
	p_ptr->redraw |= (PR_EXP);
	redraw_stuff();

	/* Load an autopick preference file */
	if (old_lev != p_ptr->lev) autopick_load_pref(FALSE);
}


/*
 * Hack -- Return the "automatic coin type" of a monster race
 * Used to allocate proper treasure when "Creeping coins" die
 *
 * XXX XXX XXX Note the use of actual "monster names"
 */
/*:::�N���[�s���O�R�C��������̎�ނ̃R�C���𗎂Ƃ����߂̏���*/
///item �N���[�s���O�R�C���Ȃǂ����Ƃ��R�C�����w�肷��
static int get_coin_type(int r_idx)
{
	/* Analyze monsters */
	switch (r_idx)
	{
	case MON_COPPER_COINS: return 2;
	case MON_SILVER_COINS: return 5;
	case MON_GOLD_COINS: return 10;
//	case MON_MITHRIL_COINS:
//	case MON_MITHRIL_GOLEM: return 16;
//	case MON_ADAMANT_COINS: return 17;
	}
	/* Assume nothing */
	return 0;
}


/*
 * Hack -- determine if a template is Cloak
 */
///item �N���[�N�𗎂Ƃ�hook
static bool kind_is_cloak(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_CLOAK)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Polearm
 */

///mod131224 item TVAL �|�[���A�[���𗎂Ƃ�hook �������������̃����_���ɂȂ�悤�ɂ���
static bool kind_is_polearm(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_POLEARM || k_ptr->tval == TV_AXE || k_ptr->tval == TV_SPEAR  )
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Sword
 */
///item �f�X�\�[�h�ƃw���u���[�h�����𗎂Ƃ����߂�hook 
///mod �Ƃ肠�����܂ꂽ���ȊO�̌��ɐݒ�
static bool kind_is_sword(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval == TV_SWORD) && (k_ptr->sval > SV_WEAPON_BROKEN_SWORD))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Book
 */
///item realm ���@��
/*:::���A�������Ƃ��{�Ɏg����*/
static bool kind_is_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_BOOK_ELEMENT) && (k_ptr->tval <= TV_BOOK_CHAOS))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Good book
 */
/*:::�㎿�Ȗ{��I�肷��hook*/
/*:::���A�������Ƃ��{�Ɏg����*/
///item realm ���@��
static bool kind_is_good_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_BOOK_ELEMENT) && (k_ptr->tval <= TV_BOOK_CHAOS)  && (k_ptr->sval > 1))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Armor
 */
///item �Z�𗎂Ƃ�hook
///mod131223
static bool kind_is_armor(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_ARMOR)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is hafted weapon
 */
///item �݊�𗎂Ƃ�hook
///mod131223 �Ƃ肠�����n���}�[�ŌŒ�
static bool kind_is_hafted(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_HAMMER)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}

/*:::�N�G�X�g�B�����̃t���O�����ƃ��b�Z�[�W*/
void complete_quest(int quest_num)
{
	switch (quest[quest_num].type)
	{
	case QUEST_TYPE_RANDOM:
		if (record_rand_quest) do_cmd_write_nikki(NIKKI_RAND_QUEST_C, quest_num, NULL);
		break;
	default:
		if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, quest_num, NULL);
		break;
	}

	//v1.1.40 �N�G�X�g�B�������𐳊m�ɂ��邽�ߒǉ�
	update_playtime();

	//v1.1.25 �N�G�X�g�B�������ǉ�
	quest[quest_num].comptime = playtime;

	quest[quest_num].status = QUEST_STATUS_COMPLETED;
	quest[quest_num].complev = (byte)p_ptr->lev;

	///mod140610
	if (quest[quest_num].type == QUEST_TYPE_BOUNTY_HUNT)
	{
		if(quest_num == QUEST_WANTED_SEIJA) msg_print(_("���q�ˎ҂�߂܂����I", "You just completed your quest!"));
		else msg_print(_("�N�G�X�g��B�������I", "You just completed your quest!"));
		msg_print(NULL);
	}
	else if(quest[quest_num].type == QUEST_TYPE_FIND_EXIT)
	{
		msg_print("���Ȃ��͖ړI�n�_�ɒH�蒅�����I");
		if(QRKDR)
		{
			//�J�n�^�[�����o�߃^�[���ɐ؂�ւ��ĕۑ�
			qrkdr_rec_turn = turn - qrkdr_rec_turn;
			if(cheat_xtra) msg_format("take_turn:%d",qrkdr_rec_turn);
		}
	}
	else if (!(quest[quest_num].flags & QUEST_FLAG_SILENT))
	{
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_QUEST_CLEAR);//v1.1.58
		msg_print(_("�N�G�X�g��B�������I", "You just completed your quest!"));
		msg_print(NULL);
	}



	//v1.1.91 �R���N�G�X�g�ł͒ǉ��Ő��̓t���O���Ǘ�����
	if (quest_num == QUEST_YAKUZA_1)
	{
		int check_r_idx = 0;
		//�R���N�G�X�g1�̃N�G�X�g�����t���O
		p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q1_COMP;

		switch (p_ptr->pclass)
		{
			//�����̐E�Ƃ͎����I�Ɋ��S�B��
		case CLASS_SAKI:
		case CLASS_YACHIE:
		case CLASS_YUMA:
		case CLASS_MAYUMI:
		case CLASS_KEIKI:
			p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q1_ACHIEVE;
			break;

		default:
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA) check_r_idx = MON_SAKI;
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU) check_r_idx = MON_YACHIE;
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU) check_r_idx = MON_YUMA;
			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_HANIWA) check_r_idx = MON_MAYUMI;
			break;
		}

		if (check_r_idx)
		{
			int i;


			//�w�肵�������X�^�[���t���A�ɐ������Ă��邩�`�F�b�N
			for (i = m_max - 1; i >= 1; i--)
			{
				//�������Ă���΍R���N�G�X�g1�̊��S�B���t���O�𗧂Ă�
				if (m_list[i].r_idx == check_r_idx)
				{
					if (cheat_xtra) msg_format("idx%d:check ok",check_r_idx);
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q1_ACHIEVE;
					break;
				}
			}
		}

	}

}

static int count_all_hostile_monsters(void)
{
	int x, y;
	int number_mon = 0;

	for (x = 0; x < cur_wid; ++ x)
	{
		for (y = 0; y < cur_hgt; ++ y)
		{
			int m_idx = cave[y][x].m_idx;

			if (m_idx > 0 && is_hostile(&m_list[m_idx]))
			{
				++ number_mon;
			}
		}
	}

	return number_mon;
}

/*:::Ex�_���W������p�̃N�G�X�g��V����*/
void create_extra_quest_rewards(void)
{
	int k_idx, a_idx,i;
	object_type forge;
	object_type *q_ptr = &forge;
	int lev = dun_level;
	int x,y,tx, ty;
	bool msg_done_flag = FALSE;
	bool flag_ok = FALSE;
	int sdia;

	if(!EXTRA_QUEST_FLOOR)
	{
		msg_print("ERROR:EX�N�G�X�g�t���A�łȂ��K��gain_extra_quest_rewards���Ă΂ꂽ");
		return;
	}
	//�K�i�̏ꏊ�𓾂�
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(cave_have_flag_bold(y,x,FF_STAIRS)) break;
		}
		if(cave_have_flag_bold(y,x,FF_STAIRS)) break;
	}
	for(sdia=1;sdia<10;sdia++)
	{
		int k;
		for(k=sdia*10;k>0;k--)
		{
			//�K�i�̋߂��ŃA�C�e���𗎂Ƃ�����W�𓾂�@�ǂ�����ł�����
			ty = y-sdia+randint0(sdia*2+1);
			tx = x-sdia+randint0(sdia*2+1);

			if(!in_bounds(ty,tx)) continue;
			if(!cave_drop_bold(ty,tx)) continue;
			flag_ok = TRUE;
			break;
		}
		if(flag_ok) break;
	}
	if(!flag_ok)
	{
		msg_print("WARNING:�N�G�X�g�A�C�e���𐶐��\�Ȓn�`���K�i�̋߂��Ɍ�����Ȃ�����");
		return;
	}

	for(i=0;ex_quest_reward_table[i].level;i++)
	{
		if(ex_quest_reward_table[i].level != lev) continue;
		object_wipe(q_ptr);
		
		a_idx = ex_quest_reward_table[i].a_idx;

		if(a_idx)	//���̐���
		{
			//���o
			if(a_info[a_idx].cur_num)
			{
				if(dun_level > 40) k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);
				else	k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);
				object_prep(q_ptr, k_idx);
				//�����Ȃ̂�apply_magic�s�v�̂͂�
			}
			else
			{
				artifact_type *a_ptr = &a_info[a_idx];
				if (create_named_art(a_idx, ty, tx))
				{
					a_ptr->cur_num = 1;
					if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
				}
				else if (!preserve_mode) a_ptr->cur_num = 1;
				continue;
			}


		}
		else//���ȊO
		{
			k_idx = lookup_kind(ex_quest_reward_table[i].tval, ex_quest_reward_table[i].sval);
			object_prep(q_ptr, k_idx);
			apply_magic(q_ptr, lev, ex_quest_reward_table[i].am_mode);
			q_ptr->number = ex_quest_reward_table[i].num;
		}

		(void)drop_near(q_ptr, -1, ty, tx);
		if(!msg_done_flag)
		{
			msg_print("�K�i�̂ق��ŉ����S�g�S�g�Ɖ��������B");
			msg_done_flag = TRUE;
		}
	}

}





/*
 * Check for "Quest" completion when a quest monster is killed or charmed.
 */
///sys �����X�^�[���|�ꂽ�Ƃ���������Ƃ��A�N�G�X�g������Ԋm�F
void check_quest_completion(monster_type *m_ptr)
{
	int i, j, y, x, ny, nx;

	int quest_num;

	bool create_stairs = FALSE;
	bool reward = FALSE;
	bool extra_reward = FALSE;

	object_type forge;
	object_type *q_ptr;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	//�ꎞ�I���݂͊֌W�Ȃ�
	if(m_ptr->mflag & MFLAG_EPHEMERA) return;

	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Inside a quest */
	quest_num = p_ptr->inside_quest;

	///mod141231 �얲�肢�̓��ւ��܋��񏈗�
	if(p_ptr->today_mon == FORTUNETELLER_TROPHY && m_ptr->r_idx == p_ptr->barter_value && r_ptr->r_akills)
	{
		reward = TRUE;
	}

	///mod140610 ���q�ˎ҃N�G�X�g
	/*:::�܋���N�G�X�g����*/
	if(r_ptr->flags3 & RF3_WANTED)
	{
		int cnt;
		bool ok = FALSE;
		for (cnt = max_quests - 1; cnt > 0; cnt--)
		{
			if (quest[cnt].status != QUEST_STATUS_TAKEN || quest[cnt].type != QUEST_TYPE_BOUNTY_HUNT)continue;
			if(quest[cnt].r_idx != m_ptr->r_idx) continue;

			ok = TRUE;
			break;
		}
		if(!ok) msg_format("ERROR:�N�G�X�g����̂��Ă��Ȃ��܋��񂪓|���ꂽ");
		else complete_quest(cnt);
	}

	/* Search for an active quest on this dungeon level */
	if (!quest_num)
	{
		for (i = max_quests - 1; i > 0; i--)
		{
			/* Quest is not active */
			if (quest[i].status != QUEST_STATUS_TAKEN)
				continue;

			/* Quest is not a dungeon quest */
			if (quest[i].flags & QUEST_FLAG_PRESET)
				continue;

			/* Quest is not on this level */
			if ((quest[i].level != dun_level) &&
			    (quest[i].type != QUEST_TYPE_KILL_ANY_LEVEL))
				continue;

			/* Not a "kill monster" quest */
			if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) ||
				(quest[i].type == QUEST_TYPE_FIND_OBJECT) ||
				(quest[i].type == QUEST_TYPE_FIND_EXIT))
				continue;

			/* Interesting quest */
			if ((quest[i].type == QUEST_TYPE_KILL_NUMBER) ||
			    (quest[i].type == QUEST_TYPE_TOWER) ||
			    (quest[i].type == QUEST_TYPE_KILL_ALL))
				break;

			/* Interesting quest */
			if (((quest[i].type == QUEST_TYPE_KILL_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_KILL_ANY_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_RANDOM)) &&
			     (quest[i].r_idx == m_ptr->r_idx))
				break;
		}

		quest_num = i;
	}

	/* Handle the current quest */
	if (quest_num && (quest[quest_num].status == QUEST_STATUS_TAKEN))
	{
		/* Current quest */
		i = quest_num;

		switch (quest[i].type)
		{
			case QUEST_TYPE_KILL_NUMBER:
			{
				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].num_mon)
				{
					complete_quest(i);

					quest[i].cur_num = 0;
				}
				break;
			}
			case QUEST_TYPE_KILL_ALL:
			{
				if (!is_hostile(m_ptr)) break;

				//�u���ܓ|����Ă���r���Ȃ̂��Ō�̓G�΃����X�^�[�v�̂Ƃ�
				if (count_all_hostile_monsters() == 1)
				{
					if (quest[i].flags & QUEST_FLAG_SILENT)
					{
						quest[i].status = QUEST_STATUS_FINISHED;
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
						set_deity_bias(DBIAS_REPUTATION, 2);
						set_deity_bias(DBIAS_COSMOS, 1);
					}
					else
					{
						complete_quest(i);
					}

					//�G�N�X�g���N�G�X�g�B�����A�X�e�[�^�X��FINISHED�ɂ�inside_quest���N���A����V�t���O�ݒ�
					if(INSIDE_EXTRA_QUEST)
					{
						extra_reward = TRUE;
						p_ptr->inside_quest = 0;
						quest[i].status = QUEST_STATUS_FINISHED;
					}
				}
				break;
			}
			case QUEST_TYPE_KILL_LEVEL:
			case QUEST_TYPE_RANDOM:
			{
				/* Only count valid monsters */
				if (quest[i].r_idx != m_ptr->r_idx)
					break;

				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].max_num)
				{
					complete_quest(i);

					//�G�N�X�g���N�G�X�g�B�����A�X�e�[�^�X��FINISHED�ɂ�inside_quest���N���A����V�t���O�ݒ�
					if(INSIDE_EXTRA_QUEST)
					{
						create_stairs = FALSE;
						extra_reward = TRUE;
						p_ptr->inside_quest = 0;
						quest[i].status = QUEST_STATUS_FINISHED;
					}
					else if (!(quest[i].flags & QUEST_FLAG_PRESET))
					{
						create_stairs = TRUE;
						p_ptr->inside_quest = 0;
					}


					/* Finish the two main quests without rewarding */
				//	if ((i == QUEST_TAISAI) || (i == QUEST_SERPENT))
					///mod140305 ����|���Ă���V�͏o�Ȃ�
					if ((i == QUEST_YUKARI) )
					{
						quest[i].status = QUEST_STATUS_FINISHED;
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
						set_deity_bias(DBIAS_REPUTATION, 10);
						set_deity_bias(DBIAS_COSMOS, 3);
					}
					/*:::Hack Easy�ł͎���|���Ă�50�K���牺�ɂ͂����Ȃ�*/
					if ((i == QUEST_YUKARI) && difficulty == DIFFICULTY_EASY) create_stairs = FALSE;

					if ((i == QUEST_TAISAI) ||(i == QUEST_SERPENT) )
					{
						///mod140118 �I�x�����N�G��B�����Ă��K�i���o�Ȃ��悤�ɂ���
						create_stairs = FALSE;
						quest[i].status = QUEST_STATUS_FINISHED;
						set_deity_bias(DBIAS_REPUTATION, 10); //�ǂ������̎��ɂ̓��x��45�ȏ�ɂȂ��Ă�Ǝv�����ꉞ
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
					}

					//Hack - Extra���[�h�ł̓I�x������|�����Ƃ��K�i���o��(���ׂ̗̈�֔��)
					if(EXTRA_MODE && i == QUEST_TAISAI)
					{
						create_stairs = TRUE;
					}

						///mod140118 �Η������N�G��B�����Ă��K�i���o�Ȃ��悤�ɂ���
						///mod150919 �w�J�[�e�B�A�N�G���ǉ�
					if (i == QUEST_SHOOTINGSTAR || i == QUEST_HECATIA)
					{
						create_stairs = FALSE;
					}

					if (quest[i].type == QUEST_TYPE_RANDOM)
					{
						reward = TRUE;
						quest[i].status = QUEST_STATUS_FINISHED;
						if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[i].level));
						set_deity_bias(DBIAS_REPUTATION, 2);
						set_deity_bias(DBIAS_COSMOS, 1);	
					}
				}
				break;
			}
			case QUEST_TYPE_KILL_ANY_LEVEL:
			{
				quest[i].cur_num++;
				if (quest[i].cur_num >= quest[i].max_num)
				{
					complete_quest(i);
					quest[i].cur_num = 0;
				}
				break;
			}
			case QUEST_TYPE_TOWER:
			{
				if (!is_hostile(m_ptr)) break;

				if (count_all_hostile_monsters() == 1)
				{
					quest[i].status = QUEST_STATUS_STAGE_COMPLETED;

					if((quest[QUEST_TOWER1].status == QUEST_STATUS_STAGE_COMPLETED) &&
					   (quest[QUEST_TOWER2].status == QUEST_STATUS_STAGE_COMPLETED) &&
					   (quest[QUEST_TOWER3].status == QUEST_STATUS_STAGE_COMPLETED))
					{

						complete_quest(QUEST_TOWER1);
					}
				}
				break;
			}
		}
	}

	/* Create a magical staircase */
	if (create_stairs)
	{
		/* Stagger around */
		while (cave_perma_bold(y, x) || cave[y][x].o_idx || (cave[y][x].info & CAVE_OBJECT) )
		{
			/* Pick a location */
			scatter(&ny, &nx, y, x, 1, 0);

			/* Stagger */
			y = ny; x = nx;
		}

		if(EXTRA_MODE && dun_level == 100)
			msg_print("���ׂ̗̈�ւ̒ʘH���J����...");
		else
			msg_print("���@�̊K�i�����ꂽ...");


		/* Create stairs down */
		cave_set_feat(y, x, feat_down_stair);

		/* Remember to update everything */
		p_ptr->update |= (PU_FLOW);
	}

	/*
	 * Drop quest reward
	 */
	if (reward)
	{
		//v1.1.55 �����N�G�Ɨ얲�܋���̕�V����
//		for (j = 0; j < (dun_level / 15)+1; j++)
		for (j = 0; j < (dun_level / 10)+1; j++)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Make a great object */
			make_object(q_ptr, AM_GOOD | AM_GREAT);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
	}

	if(extra_reward)
	{
		create_extra_quest_rewards();

	}

}

/*:::�A�C�e������^�N�G�X�g�ŃA�C�e������肵�����ǂ����`�F�b�N*/

void check_find_art_quest_completion(object_type *o_ptr)
{
	int i;
	/* Check if completed a quest */
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (quest[i].k_idx == o_ptr->name1))
		{
			complete_quest(i);
		}
		///mod151029 �󕨌ɂ́��Ȃ����̓��ꏈ��
		else if ((quest[i].type == QUEST_TYPE_FIND_RANDOM_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (o_ptr->marked & OM_SPECIAL_FLAG))
		{
			complete_quest(i);
		}

		else if ((quest[i].type == QUEST_TYPE_FIND_OBJECT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (quest[i].k_idx == o_ptr->k_idx))
		{
			complete_quest(i);
		}


	}
}

/*
 * Return monster death string
 */
/*:::�f�t�H���g�̃����X�^�[���S���Z���t*/
///mon �f�t�H�̃����X�^�[���S���Z���t
cptr extract_note_dies(monster_race *r_ptr)
{
	/* Some monsters get "destroyed" */
	if (!monster_living(r_ptr))
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			if (r_ptr->blow[i].method == RBM_EXPLODE)
			{
#ifdef JP
//				return "�͔������ĕ��X�ɂȂ����B";
				return "�͑唚�������I";
#else
				return " explodes into tiny shreds.";
#endif
			}
		}

#ifdef JP
		return "��|�����B";
#else
		return " is destroyed.";
#endif
	}

	/* Assume a default death */
#ifdef JP
	///msg131214 death
	//return "�͎��񂾁B";
	return "��|�����B";

#else
	return " dies.";
#endif
}





//v1.1.51 �V�A���[�i�p���i����
//nightmare_treasure_table[]���獡��̃X�e�[�W���x���ɍ��킹����V�A�C�e�������A���e�B�l�����đI�ѐ������Ēn�ʂɗ��Ƃ�
static void make_nightmare_treasure(byte x, byte y)
{
	object_type forge;
	object_type *o_ptr = &forge;
	int i;
	int treasure_idx = -1;
	int stage_level;

	int chance_total;

	//�O���[�o���ϐ��ɕۑ����Ă����������X�^�[���x������X�e�[�W���x�����t�Z 0-7
	for (i = 0; i < NIGHTMARE_DIARY_STAGE_LEV_MAX; i++)
	{
		stage_level = i;
		if (nightmare_mon_base_level <= nightmare_stage_table[i].level) break;
	}


	//rarity�l��S�������A���v�l�ȓ��̃����_���Ȓl���擾���A�������瓯������rarity�l�������Ă����A�}�C�i�X�ɂȂ����Ƃ���Ŏ~�߂Ă����̃C���f�b�N�X������l�Ƃ���
	chance_total = 0;
	for (i = 0; nightmare_treasure_table[i].rarity; i++)//rarity=0���z��̏I�[�_�~�[
	{
		if (nightmare_treasure_table[i].nightmare_level != stage_level) continue;
		chance_total += nightmare_treasure_table[i].rarity;
	}
	chance_total = randint0(chance_total);
	for (i = 0; nightmare_treasure_table[i].rarity; i++)
	{
		if (nightmare_treasure_table[i].nightmare_level != stage_level) continue;
		chance_total -= nightmare_treasure_table[i].rarity;

		if (chance_total < 0) break;
	}
	if (!nightmare_treasure_table[i].rarity) { msg_format("ERROR:make_nightmare_treasure()�Ŏw�肳�ꂽ���x��(%d)�̕�V�A�C�e�����o�^����Ă��Ȃ�", stage_level); return; }
	treasure_idx = i;

	//���肵��idx�̃A�C�e���𐶐�
	object_prep(o_ptr, lookup_kind(nightmare_treasure_table[treasure_idx].tval, nightmare_treasure_table[treasure_idx].sval));
	o_ptr->number = nightmare_treasure_table[treasure_idx].num;
	apply_magic(o_ptr, nightmare_mon_base_level, AM_FORCE_NORMAL);

	(void)drop_near(o_ptr, 0, y, x);

}


//v1.1.51�@�V�A���[�i��������p���[�`��
//monster_death()���Ń����X�^�[���|�ꂽ�Ƃ��ɌĂ΂��B�S���̃����X�^�[��|�������ǂ����m�F���A�B�������烁�b�Z�[�W���o���ܕi�������[�`�����Ă�
static void	check_nightmare_diary_completion(monster_type *m_ptr)
{

	if (!p_ptr->inside_arena)	return;
	if (!m_ptr->r_idx) return;
	if (!is_hostile(m_ptr)) return;

	//�u���ܑœ|�����r���̃����X�^�[���Ō�̓G�΃����X�^�[�ł���v�Ƃ��B���Ɣ��肷��
	if (count_all_hostile_monsters() == 1)
	{
		p_ptr->exit_bldg = TRUE;

		//�ܕi���������ɍς�ł���ꍇ�I���@
		//�A���[�i���ł̓����X�^�[�����n���S�Ė���������Ă���͂��Ȃ̂œ����A���[�i�ł����ɓ�x���邱�Ƃ͂Ȃ����O�̈�
		if (!nightmare_mon_base_level) return;

		//�V�A���[�i�̒B���񐔂ƍ��v�|�C���g���ɉ��Z
		nightmare_diary_win++;
		nightmare_total_point += tmp_nightmare_point;

		if (nightmare_total_point > 999999999)
			nightmare_total_point = 999999999;

		if (p_ptr->pclass == CLASS_DOREMY)
		{
			msg_print("���Ȃ��͊F�̈�������������������B");
			(void)set_food(PY_FOOD_MAX - 1);

		}

		//����\�������x�����オ������񍐁H

		//�ʐ^����
		make_nightmare_diary_photo(m_ptr->fx, m_ptr->fy);
		//�ܕi����
		make_nightmare_treasure(m_ptr->fx, m_ptr->fy);

		//v1.1.57
		if (record_arena) do_cmd_write_nikki(NIKKI_ARENA, 1, 0);

		//�ܕi�������Z�񂾃t���O�Ƃ��āA�O���[�o���ϐ��̃A���[�i�����X�^�[���x����0�ɂ���
		nightmare_mon_base_level = 0;

	}



}


/*:::�����X�^�[����̃A�C�e���h���b�v������ꕔ�����X�^�[���|�ꂽ���̓��ꏈ���@�E�����A�C�e���ƃN�G�X�g�����X�^�[�A�A���[�i�l��*/
/*:::���j�[�N�ɑ΂��Ắ��݂̂����̊֐�����������*/
/*
 * Handle the "death" of a monster.
 *
 * Disperse treasures centered at the monster location based on the
 * various flags contained in the monster flags fields.
 *
 * Check for "Quest" completion when a quest monster is killed.
 *
 * Note that only the player can induce "monster_death()" on Uniques.
 * Thus (for now) all Quest monsters should be Uniques.
 *
 * Note that monsters can now carry objects, and when a monster dies,
 * it drops all of its objects, which may disappear in crowded rooms.
 */
///item mon �����X�^�[���|�ꂽ���̏����@�A�C�e���h���b�v�Ȃ�
///mod131224 TVAL
void monster_death(int m_idx, bool drop_item)
{
	int i, j, y, x;

	int dump_item = 0;
	int dump_gold = 0;

	int number = 0;

	int yachie_card_num = 0;

	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	bool visible = ((m_ptr->ml && !p_ptr->image) || (r_ptr->flags1 & RF1_UNIQUE));

	u32b mo_mode = 0L;

	bool do_gold = (!(r_ptr->flags1 & RF1_ONLY_ITEM));
	bool do_item = (!(r_ptr->flags1 & RF1_ONLY_GOLD));
	bool cloned = (m_ptr->smart & SM_CLONED) ? TRUE : FALSE;
	int force_coin = get_coin_type(m_ptr->r_idx);

	object_type forge;
	object_type *q_ptr;

	/*:::�����X�^�[���w�肵���A�C�e���𗎂Ƃ����Ƃ��\�ɂ���t���O*/
	bool drop_chosen_item = drop_item && !cloned && !p_ptr->inside_arena
		&& !p_ptr->inside_battle && !is_pet(m_ptr);

	//�疒�́u����ւ̋����v���󂯂��G�̓A�C�e���𗎂Ƃ��Ȃ�
	if (p_ptr->pclass == CLASS_CHIMATA && (m_ptr->mflag & MFLAG_SPECIAL))
	{
		drop_chosen_item = FALSE;
		drop_item = FALSE;

	}

	//�z����|�������@��ǐ_�l����
	if(drop_item && is_pet(m_ptr))
	{
		set_deity_bias(DBIAS_COSMOS, -1);
		set_deity_bias(DBIAS_WARLIKE, 1);
		set_deity_bias(DBIAS_REPUTATION, -1);
	}
	//�_�ގq�̐M�|�C���g�擾
	if(p_ptr->pclass == CLASS_KANAKO && drop_chosen_item ) kanako_get_point(m_ptr);

	//���̐l�`�̔�
	if(p_ptr->pclass == CLASS_HINA && drop_chosen_item )
	{
		int get_au;
		get_au = r_ptr->level * r_ptr->level * r_ptr->level / (200-p_ptr->lev-p_ptr->stat_ind[A_CHR]);
		if(r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & RF7_UNIQUE2)
			get_au = get_au * 5 + 100;
		p_ptr->magic_num1[1] += get_au;
		if(p_ptr->magic_num1[1] > 100000) p_ptr->magic_num1[1] = 100000;
	}
	///mod160325 UNIQUE2��|�������ɃX�R�A���Z�l���v�Z
	if(r_ptr->flags7 & RF7_UNIQUE2 && drop_chosen_item)
	{
		score_bonus_kill_unique2 += r_ptr->level * r_ptr->level;
		if(score_bonus_kill_unique2 > 1000000L) score_bonus_kill_unique2 = 1000000L;
	}

	//�ꎞ�����X�^�[�̂Ƃ�������ۂ��t���OON
	if(m_ptr->mflag & MFLAG_EPHEMERA)
	{
		cloned = TRUE;
		drop_chosen_item = FALSE;
		drop_item = FALSE;
	}
	/* The caster is dead? */
	/*:::���Ԓ�~���̃����X�^�[�����S�����ꍇ���Ԓ�~����*/
	if (world_monster && world_monster == m_idx) world_monster = 0;

	/* Notice changes in view */
	if (r_ptr->flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
	{
		/* Update some things */
		p_ptr->update |= (PU_MON_LITE);
	}

	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
	{
		char m_name[80];

		monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
		do_cmd_write_nikki(NIKKI_NAMED_PET, 3, m_name);
	}




	/* Let monsters explode! */
	/*:::���������X�^�[�̏ꍇ�������� */
	for (i = 0; i < 4; i++)
	{
		if (r_ptr->blow[i].method == RBM_EXPLODE)
		{
			int rad = 3;
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int typ = mbe_info[r_ptr->blow[i].effect].explode_type;
			int d_dice = r_ptr->blow[i].d_dice;
			int d_side = r_ptr->blow[i].d_side;
			int damage = damroll(d_dice, d_side);

			if(m_ptr->r_idx == MON_SAGUME_MINE) rad = 4;

			if(!world_monster) project(m_idx, 3, y, x, damage, typ, flg, -1);
			break;
		}
	}

	//��̋P�邪�|�ꂽ��Q�[���I�[�o�[
	//���v���Ǝv�������Z���A���[�i�ł͔������Ȃ��悤�ɂ��Ƃ�
	if(m_ptr->r_idx == MON_MASTER_KAGUYA && p_ptr->pclass == CLASS_EIRIN && !p_ptr->inside_battle && !p_ptr->inside_arena)
	{
		msg_print("���Ƃ������Ƃ��I�P�����肫��Ȃ������I");
		if (last_words)
		{
			char death_message[1024];
			get_rnd_line("death_j.txt", 0, death_message);
			while (!get_string("�����Z���t: ", death_message, 1024)) ;
			p_ptr->last_message = string_make(death_message);
			msg_print(death_message);
		}
		p_ptr->playing = FALSE;
		p_ptr->is_dead = TRUE;
		p_ptr->leaving = TRUE;
		strcpy(p_ptr->died_from, "mission_failed");
		return;
	}

	/*:::�J�����I���͐��̂������H*/
	if (m_ptr->mflag2 & MFLAG2_CHAMELEON)
	{
		choose_new_monster(m_idx, TRUE, MON_CHAMELEON);
		r_ptr = &r_info[m_ptr->r_idx];
	}

	/* Check for quest completion */
	/*:::check_quest_completion() �N�G�X�g�B���󋵊m�F�H*/
	check_quest_completion(m_ptr);

	/*:::�A���[�i�Ńy�b�g�ȊO�����񂾏ꍇ�i�A���[�i�����j*/
	/* Handle the possibility of player vanquishing arena combatant -KMW- */
	if (p_ptr->inside_arena && !is_pet(m_ptr))
	{
		check_nightmare_diary_completion(m_ptr);

#if 0
		p_ptr->exit_bldg = TRUE;

		if (p_ptr->arena_number > MAX_ARENA_MONS)
		{
#ifdef JP
msg_print("�f���炵���I�N�����^�̏����҂��B");
#else
			msg_print("You are a Genuine Champion!");
#endif
		}
		else
		{
#ifdef JP
msg_print("�����I�`�����s�I���ւ̓���i��ł���B");
#else
			msg_print("Victorious! You're on your way to becoming Champion.");
#endif
		}

	/*:::�w�肵���A�C�e�����h���b�v*/
		if (arena_info[p_ptr->arena_number].tval)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a prize */
			object_prep(q_ptr, lookup_kind(arena_info[p_ptr->arena_number].tval, arena_info[p_ptr->arena_number].sval));

			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);


			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}

		if (p_ptr->arena_number > MAX_ARENA_MONS) p_ptr->arena_number++;
		p_ptr->arena_number++;
		if (record_arena)
		{
			char m_name[80];
			
			/* Extract monster name */
			monster_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
			
			do_cmd_write_nikki(NIKKI_ARENA, p_ptr->arena_number, m_name);
		}

#endif
	}

/*:::��n���̃����X�^�[�����񂾂Ȃ痎�n���� */
	if (m_idx == p_ptr->riding)
	{
		if (rakuba(-1, FALSE))
		{
#ifdef JP
			msg_print("�n�ʂɗ��Ƃ��ꂽ�B");
#else
			msg_print("You have fallen from your riding pet.");
#endif
		}
	}

#if 0
	///del131221 �����X�^�[�̎��̃h���b�v�𖳂�����
	/*:::���̂����𗎂Ƃ����� */
	/* Drop a dead corpse? */
	if (one_in_(r_ptr->flags1 & RF1_UNIQUE ? 1 : 4) &&
	    (r_ptr->flags9 & (RF9_DROP_CORPSE | RF9_DROP_SKELETON)) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle || cloned || ((m_ptr->r_idx == today_mon) && is_pet(m_ptr))))
	{
		/* Assume skeleton */
		bool corpse = FALSE;

		/*
		 * We cannot drop a skeleton? Note, if we are in this check,
		 * we *know* we can drop at least a corpse or a skeleton
		 */

		if (!(r_ptr->flags9 & RF9_DROP_SKELETON))
			corpse = TRUE;
		else if ((r_ptr->flags9 & RF9_DROP_CORPSE) && (r_ptr->flags1 & RF1_UNIQUE))
			corpse = TRUE;

		/* Else, a corpse is more likely unless we did a "lot" of damage */
		else if (r_ptr->flags9 & RF9_DROP_CORPSE)
		{
			/*:::�I�[�o�[�L����������ƍ��������₷������ �i�������X�^�[��HP�̓}�C�i�X�ł���j*/
			/* Lots of damage in one blow */
			if ((0 - ((m_ptr->maxhp) / 4)) > m_ptr->hp)
			{
				if (one_in_(5)) corpse = TRUE;
			}
			else
			{
				if (!one_in_(5)) corpse = TRUE;
			}
		}

		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make an object */
		object_prep(q_ptr, lookup_kind(TV_CORPSE, (corpse ? SV_CORPSE : SV_SKELETON)));

		apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);
		/*:::���̂�pval�̓����X�^�[idx������*/
		q_ptr->pval = m_ptr->r_idx;

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}
#endif

	/*:::�t�r�_���|�����ƌ��A�C�e�����򉻂��邱�Ƃ�����*/
	if (m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5 ) 
	{
		object_type *o_ptr = &o_list[m_ptr->hold_o_idx];

		if(object_is_artifact(o_ptr))
		{
			if (o_ptr->to_h > -99 && one_in_(2)) o_ptr->to_h--;
			if (o_ptr->to_d > -99 && one_in_(2)) o_ptr->to_d--;
			if (o_ptr->to_a > 0 && one_in_(2)) o_ptr->to_a--;
			if ((o_ptr->pval > 1) && one_in_(13) ) o_ptr->pval--;
		}
		else
		{
			int tmp1 = o_ptr->to_h - randint1(2) - m_bonus(5,dun_level);
			int tmp2 = o_ptr->to_d - randint1(2) - m_bonus(5,dun_level);
			int tmp3 = o_ptr->to_a - randint1(3);
			o_ptr->to_h = MAX(-99,(tmp1));
			o_ptr->to_d = MAX(-99,(tmp2));
			o_ptr->to_a = MAX(0,tmp3);
			if ((o_ptr->pval > 1) && one_in_(6) ) o_ptr->pval--;
		}


	}

	//�G�œ|����HP�z���Ƃ�
	//v1.1.83 @�������X�^�[��|�����Ƃ��݂̂Ɍ���B
	if (drop_item && !p_ptr->is_dead)
	{

		///mod150502 �z�K�q���ꏈ���@�l�Ԃ�|���Γ|���قǍő�MP�㏸
		if (p_ptr->pclass == CLASS_SUWAKO && (r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)) && p_ptr->magic_num1[0] < SUWAKO_POINT_MAX)
		{
			int point = 0;
			int point_old = p_ptr->magic_num1[0] / 100;

			point = r_ptr->level + randint1(10);
			if (r_ptr->flags3 & (RF3_DEMIHUMAN)) point /= 5;
			if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || (r_ptr->flags7 & RF7_UNIQUE2)) point *= 10;

			if (point_old > 500) point /= 2;

			p_ptr->magic_num1[0] += point;
			if (p_ptr->magic_num1[0] > SUWAKO_POINT_MAX) p_ptr->magic_num1[0] = SUWAKO_POINT_MAX;
			if (p_ptr->magic_num1[0] / 100 > point_old) p_ptr->update |= PU_MANA;
		}
		//v1.1.21 ���m
		if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT, SS_C_SCAVENGER))
		{
			int get_mana = r_ptr->level;
			if (r_ptr->level > 50 || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) get_mana *= 2;
			if (get_mana <1) get_mana = 1;
			if (player_gain_mana(get_mana))
				msg_print("MP���񕜂����I");
		}
		//v1.1.30 �d���@�C���̌�������
		if (p_ptr->pclass == CLASS_YOUMU && p_ptr->tim_general[0])
		{
			int gain = MAX(1, r_ptr->level / 2);
			if (r_ptr->level > 50 || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) gain *= 2;
			player_gain_mana(gain);

		}
		//v1.1.89 �S�X�����ł̃O���� HP�Ɩ����x�啝�z��
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[3])
		{
			hp_player(m_ptr->maxhp);

			if (p_ptr->food < PY_FOOD_FULL)
			{
				set_food(MIN(p_ptr->food + m_ptr->maxhp, PY_FOOD_FULL));

			}

		}

		//v1.1.65 �ؐ���ꐫ�i
		if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
		{
			{
				int gain = MAX(1, r_ptr->level * 2);
				if (r_ptr->level > 50 || r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) gain *= 2;
				hp_player(gain);
			}
		}

		//v1.1.70 �푰������͐������A���f�b�h��|�����ƂŖ����x�㏸
		//v1.1.87 �v�Z���~�X���ĂĂ�������イ�H�ׂ����ɂȂ��Ă��̂ŏC��
		if (prace_is_(RACE_ANIMAL_GHOST) && (monster_living(r_ptr) || r_ptr->flags3 & RF3_UNDEAD))
		{
			int feed = 1000 + r_ptr->level * 50 + r_ptr->level * r_ptr->level;
			if(p_ptr->food < PY_FOOD_ALERT) msg_print("���Ȃ��͓G���犈�͂�D�����B");
			if (p_ptr->food < PY_FOOD_FULL)	set_food(MIN(PY_FOOD_FULL, (p_ptr->food + feed)));

		}

	}


	/* Drop objects being carried */
	/*:::�����X�^�[���E���Ă����A�C�e���𗎂Ƃ�*/ 
	monster_drop_carried_objects(m_ptr);

	if (r_ptr->flags1 & RF1_DROP_GOOD) mo_mode |= AM_GOOD;
	if (r_ptr->flags1 & RF1_DROP_GREAT) mo_mode |= AM_GREAT;

	/*:::�����X�^�[�푰�ŕ��򂵁A�����X�^�[���S���̓��ꏈ��*/
	///mon �ꕔ�����X�^�[���S���̓��ꏈ��
	switch (m_ptr->r_idx)
	{
	/*:::�s���N�z���[�̕��􏈗� summon_specific()���*/
	case MON_PINK_HORROR:
		/* Pink horrors are replaced with 2 Blue horrors */
		if (!(p_ptr->inside_arena || p_ptr->inside_battle))
		{
			bool notice = FALSE;

			for (i = 0; i < 2; i++)
			{
				int wy = y, wx = x;
				bool pet = is_pet(m_ptr);
				u32b mode = 0L;

				if (pet) mode |= PM_FORCE_PET;

				if (summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_BLUE_HORROR, mode))
				{
					if (player_can_see_bold(wy, wx))
						notice = TRUE;
				}
			}

			if (notice)
#ifdef JP
				msg_print("�s���N�E�z���[�͕��􂵂��I");
#else
				msg_print("The Pink horror divides!");
#endif
		}
		break;
	/*:::���C��15%�̊m���ŃJ�I�X�u���[�h�𗎂Ƃ�*/
	///item ���C�����h���b�v�@�J�I�u���łȂ����ו���ɕς��悤
	///mon ���C�ɕς���č��̌����J�I�u���𗎂Ƃ�
#if 0
	case MON_BLOODLETTER:
		/* Bloodletters of Khorne may drop a blade of chaos */
		if (drop_chosen_item && (randint1(100) < 15))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a Blade of Chaos */
			object_prep(q_ptr, lookup_kind(TV_SWORD, SV_BLADE_OF_CHAOS));

			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART | mo_mode);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
#endif
	case MON_RAAL:
		/*:::���A���͊K�w�ɉ������m���Ŗ{�𗎂Ƃ�*/
		if (drop_chosen_item && (dun_level > 9))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Activate restriction */
			if ((dun_level > 49) && one_in_(5))
				get_obj_num_hook = kind_is_good_book;
			else
				get_obj_num_hook = kind_is_book;

			/* Make a book */
			make_object(q_ptr, mo_mode);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

		

	/*:::�ł̐�m�̉��R����*/
	case MON_DAWN:
		/*
		 * Mega^3-hack: killing a 'Warrior of the Dawn' is likely to
		 * spawn another in the fallen one's place!
		 */
		if (!p_ptr->inside_arena && !p_ptr->inside_battle)
		{
			if (!one_in_(7))
			{
				int wy = y, wx = x;
				int attempts = 100;
				bool pet = is_pet(m_ptr);

				do
				{
					scatter(&wy, &wx, y, x, 20, 0);
				}
				while (!(in_bounds(wy, wx) && cave_empty_bold2(wy, wx)) && --attempts);

				if (attempts > 0)
				{
					u32b mode = 0L;
					if (pet) mode |= PM_FORCE_PET;

					if (summon_specific((pet ? -1 : m_idx), wy, wx, 100, SUMMON_DAWN, mode))
					{
						if (player_can_see_bold(wy, wx))
#ifdef JP
							msg_print("�V���Ȑ�m�����ꂽ�I");
#else
							msg_print("A new warrior steps forth!");
#endif

					}
				}
			}
		}
		break;
	/*:::���ׂ͋���ȃJ�I�X�̔������N����*/
	case MON_UNMAKER:
		/* One more ultra-hack: An Unmaker goes out with a big bang! */
		{
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			(void)project(m_idx, 6, y, x, 100, GF_CHAOS, flg, -1);
		}
		break;
///del131224 item �Ȃ܂��_��
#if 0
	/*:::�Ȃ܂��_�퐶������*/
	case MON_UNICORN_ORD:
	case MON_MORGOTH:
	case MON_ONE_RING:
		/* Reward for "lazy" player */
		if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		{
			int a_idx = 0;
			artifact_type *a_ptr = NULL;

			if (!drop_chosen_item) break;

			do
			{
				switch (randint0(3))
				{
				case 0:
					a_idx = ART_NAMAKE_HAMMER;
					break;
				case 1:
					a_idx = ART_NAMAKE_BOW;
					break;
				case 2:
					a_idx = ART_NAMAKE_ARMOR;
					break;
				}

				a_ptr = &a_info[a_idx];
			}
			while (a_ptr->cur_num);

			/* Create the artifact */
			if (create_named_art(a_idx, y, x))
			{
				a_ptr->cur_num = 1;

				/* Hack -- Memorize location of artifact in saved floors */
				if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
			}
			else if (!preserve_mode) a_ptr->cur_num = 1;
		}
		break;
#endif
	/*:::J�̓O�����h�ƓS������*/
	///item mon J�̃h���b�v�A�C�e��
		///mod140304 �O�����h�̓����S�X�����Ƃ��悤�ɂ���
	case MON_MORGOTH:
		if (!drop_chosen_item) break;

		/* Get local object */
		q_ptr = &forge;


		//v1.1.23 ���Ȃ��I�v�V�����̂Ƃ��̓O�����h�́��ɂ���悤�ɂ����B3d9����₵���̂ł�����Ƌ������Ƃ��B
		if(ironman_no_fixed_art)
		{
			do
			{
				object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_GROND));
				create_artifact(q_ptr, FALSE);
				q_ptr->to_d += 5+randint1(5);
				q_ptr->to_h += 5+randint1(5);
				q_ptr->dd += 3;

			}while(object_is_cursed(q_ptr));
			//v1.1.24 ���������␳
			if(q_ptr->to_d < 20) q_ptr->to_d = 20;

		}
		else
		{
			/* Mega-Hack -- Prepare to make "Grond" */
			object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_GROND));
			/* Mega-Hack -- Mark this item as "Grond" */
			q_ptr->name1 = ART_GROND;
			/* Mega-Hack -- Actually create "Grond" */
			apply_magic(q_ptr, -1, AM_GOOD | AM_GREAT);
		}


		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
		break;
	case MON_SERPENT:
		if (!drop_chosen_item) break;

		/* Get local object */
		q_ptr = &forge;

		/* Mega-Hack -- Prepare to make "Chaos" */
		object_prep(q_ptr, lookup_kind(TV_HEAD, SV_HEAD_IRONCROWN));

		/* Mega-Hack -- Mark this item as "Chaos" */
		q_ptr->name1 = ART_CHAOS;

		/* Mega-Hack -- Actually create "Chaos" */
		apply_magic(q_ptr, -1, AM_GOOD | AM_GREAT);

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
		break;
	/*:::�܂ꂽ�f�X�\�[�h�͐܂ꂽ�����܂ꂽ�_�K�[�𐶐�*/
	case MON_B_DEATH_SWORD:
		if (drop_chosen_item)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a broken sword */
			//v1.1.29 ���򂪂������������̂Œ����� 
			if(one_in_(2))
				object_prep(q_ptr, lookup_kind(TV_SWORD, SV_WEAPON_BROKEN_SWORD));
			else 
				object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_BROKEN_DAGGER));

			//v1.1.29
			//apply_magic()������ĂȂ��������{�Ƃ��炾�����B�X�J���h�~���낤���H
			//�ł����Ƃ����ق����ʔ������Ȃ̂Œǉ����Ă���
			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);
			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	/*:::����G���[���̊ʋl����*/
	case MON_A_GOLD:
	case MON_A_SILVER:
		if (drop_chosen_item && ((m_ptr->r_idx == MON_A_GOLD) ||
		     ((m_ptr->r_idx == MON_A_SILVER) && (r_ptr->r_akills % 5 == 0))))
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a Can of Toys */
			object_prep(q_ptr, lookup_kind(TV_CHEST, SV_CHEST_KANDUME));

			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART);

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	/*:::�������g�͉΂̔���*/
	///mon �������g���S����
	case MON_ROLENTO:
		{
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			(void)project(m_idx, 3, y, x, damroll(20, 10), GF_FIRE, flg, -1);
		}
		break;
		///mod140424

	/*:::Mushroom hunting */
	case MON_MAGIC_MUSHROOM_RED:	
	case MON_MAGIC_MUSHROOM_WHITE:	
	case MON_MAGIC_MUSHROOM_BLUE:	
	case MON_MAGIC_MUSHROOM_BLACK:	
	case MON_MAGIC_MUSHROOM_GREEN:	
	case MON_MAGIC_MUSHROOM_GAUDY:	
	case MON_MAGIC_MUSHROOM_GRAY:	
	case MON_MAGIC_MUSHROOM_CLEAR:	
	case MON_MAGIC_MUSHROOM:	
		if(drop_chosen_item && one_in_(2))
		{
			int sval;
			q_ptr = &forge;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_RED) sval = SV_MUSHROOM_MON_RED;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_WHITE) sval = SV_MUSHROOM_MON_WHITE;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_BLUE) sval = SV_MUSHROOM_MON_BLUE;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_BLACK) sval = SV_MUSHROOM_MON_BLACK;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_GREEN) sval = SV_MUSHROOM_MON_GREEN;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_GAUDY) sval = SV_MUSHROOM_MON_GAUDY;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_GRAY) sval = SV_MUSHROOM_MON_GRAY;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM_CLEAR) sval = SV_MUSHROOM_MON_CLEAR;
			if(m_ptr->r_idx == MON_MAGIC_MUSHROOM) sval = SV_MUSHROOM_MON_L_BLUE;
			object_prep(q_ptr, lookup_kind(TV_MUSHROOM, sval));
			apply_magic(q_ptr, object_level, AM_NO_FIXED_ART | mo_mode);
			(void)drop_near(q_ptr, -1, y, x);

		}
		break;
	case  MON_SANAE:
		/*:::�t���[�c*/	
		if(drop_chosen_item )
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_MIRACLE_FRUIT ));
			q_ptr->number = randint1(5);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.49 �~�X�e�B�A��|������V�𗎂Ƃ����Ƃɂ����B�V�Ⴂ����
	case MON_MYSTIA:
		if (drop_chosen_item)
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_EEL));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case  MON_EIRIN:
		/*:::���s���ۓW�œ|�����Ƃ��̂݋ʎ蔠�𗎂Ƃ�*/	
		if(drop_chosen_item && p_ptr->inside_quest == QUEST_MOON_VAULT )
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_CHEST, SV_CHEST_MARIPO_24 ));
			q_ptr->pval = -6;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case  MON_MITHRIL_GOLEM :
		/*:::�~�X�����S�[�����̓~�X�����z�΂�1�`3���Ƃ��悤�ɂ���*/	
		if(drop_chosen_item )
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_MITHRIL ));
			q_ptr->number = randint1(3);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_BLOODLETTER:
		/*:::�J�I�X�u���[�h�łȂ����ו���𗎂Ƃ��悤�ɕύX*/
		if (drop_chosen_item && (randint1(100) < 20))
		{
			/*::: Hack - �J�I�X�u���[�h�܂ł̑S���킪�����_���ŏo�� */
			int sval = randint1(SV_WEAPON_BLADE_OF_CHAOS); 
			q_ptr = &forge;

			object_prep(q_ptr, lookup_kind(TV_SWORD, sval));

			q_ptr->to_h = 3 + randint1(dun_level) % 10;
			q_ptr->to_d = 3 + randint1(dun_level) % 10;
			one_resistance(q_ptr);
			q_ptr->name2 = EGO_WEAPON_CHAOS;

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		/*:::���̌��̓J�I�X�u���[�h�𗎂Ƃ��B1/2�ō����i*/
	case MON_CHAOS_BLADE:
		if (drop_chosen_item )
		{
			int am_mode = 0L;
			if(one_in_(2)) am_mode |= (AM_GOOD | AM_GREAT);
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SWORD, SV_WEAPON_BLADE_OF_CHAOS));
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		/*:::�����͎��_�̑劙�𗎂Ƃ��B1/2�ō����i*/
	case MON_KOMACHI:
		if (drop_chosen_item)
		{
			int am_mode = AM_GOOD;
			if(one_in_(2)) am_mode |= AM_GREAT;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_POLEARM, SV_WEAPON_SCYTHE_OF_SLICING));
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_REIMU:
		///mod160206 �얲�͐܂ꂽ�j�𗎂Ƃ�
		if(drop_chosen_item && randint1(r_ptr->level) > 20)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_BROKEN_NEEDLE ));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

		/*:::�����͋n�𗎂Ƃ�*/
		//���i�e�B�b�N�K�������Ƃ��H
	case MON_SEIRAN:
		if (drop_chosen_item)
		{
			int am_mode = AM_GOOD;
			int k_idx;

			if(one_in_(3)) am_mode |= AM_GREAT;

			k_idx = lookup_kind(TV_HAMMER, SV_WEAPON_KINE);

			q_ptr = &forge;
			object_prep(q_ptr, k_idx);
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_ORIN:
		if (drop_chosen_item)
		{
			int num,wy,wx;
			q_ptr = &forge;
			for(num = 2 + randint1(3);num > 0; num--)
			{
				scatter(&wy, &wx, y, x, 3, 0);
				object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_ISHIZAKURA));
				(void)drop_near(q_ptr, -1, wy, wx);
			}
			//v1.1.98 ���W���ǉ�
			for (num = 1 + randint1(2); num > 0; num--)
			{
				scatter(&wy, &wx, y, x, 3, 0);
				object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_SKULL));
				(void)drop_near(q_ptr, -1, wy, wx);
			}

		}
		break;
	case MON_RINGO:
		if (drop_chosen_item)
		{
			int num,wy,wx;
			q_ptr = &forge;
			for(num = 20 + randint1(20);num > 0; num--)
			{
				scatter(&wy, &wx, y, x, 3, 0);
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
				(void)drop_near(q_ptr, -1, wy, wx);
			}
		}
		break;
		//�Ă�ɍ����S�̌���|�ǉ�
	case MON_TEWI:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_TAKAKUSAGORI));
			q_ptr->number = 1+randint1(2);
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_SUMIREKO:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.98
	case MON_MIZUCHI:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_KODOKUZARA));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	case MON_NIGHTMARE_F:
		if (drop_chosen_item && one_in_(3))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_NIGHTMARE_FRAGMENT));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;


	case MON_3FAIRIES:
		if (drop_chosen_item)
		{
			int power = randint1(100);
			q_ptr = &forge;

			if(power < 94) object_prep(q_ptr, lookup_kind(TV_ALCOHOL, SV_ALCOHOL_GINZYOU));
			else if(power < 97) object_prep(q_ptr, lookup_kind(TV_ALCOHOL, SV_ALCOHOL_SYUTYUU));
			else if(power < 99) object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STARS_AND_STRIPES));
			else if(power < 100) object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_BIG_EGG));
			else object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_METEORICIRON));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

		//�������̓X�}�[�g�t�H���i俎q�����Ƃ������́j�������Ă���B俎q�̂Ƃ��̂ݗ��Ƃ��B
	case MON_KOISHI:
		if (drop_chosen_item && (p_ptr->pclass == CLASS_SUMIREKO) && !is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	case MON_UDONGE:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_ULTRASONIC));
			(void)drop_near(q_ptr, -1, y, x);

		}
		break;

		//�������͒�m���ŐF�X���Ƃ�
	case MON_MARISA:
		if (drop_chosen_item && one_in_(2))
		{
			q_ptr = &forge;

			if(one_in_(3) && randint1(dun_level) > 50)
				object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_MARISA_TUBO));
			else if(one_in_(2) && randint1(dun_level) > 30)
				object_prep(q_ptr, lookup_kind(TV_ALCOHOL, SV_ALCOHOL_MAMUSHI));
			else 
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_STRANGE_BEAN));

			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case MON_KASEN:
		if (drop_chosen_item && one_in_(3))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_DOUJU));
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
	case  MON_URUMI:
		//v1.1.69 �����͐΂̐Ԏq�𗎂Ƃ�
		if (drop_chosen_item)
		{
			int sval;
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_STONE_BABY));
			q_ptr->number =1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 �j���ۃn�E�X
	case	MON_SHINMYOUMARU:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SHINMYOU_HOUSE));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 �z���S�n�E�X
	case	MON_REMY: 
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_VAMPIRE_COFFIN));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 �}�C�N�����t�g�E�{�u
	case	MON_BULLGATES:
		if (drop_chosen_item) 
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_BOB));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;
		//v1.1.79 ���b�N�@���t�m�[���̋�
	case	MON_LOCKE:
		if (drop_chosen_item) 
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_MIRROR_OF_RUFFNOR));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

		//v1.1.79 �v�̉́@���̑����G�N�X�e
	case	MON_KUTAKA:
		if (drop_chosen_item) 
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_KUTAKA));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;
	//v1.1.79 ���L�@�X�L�[�Y�u���Y�j��
	case	MON_LOKI:
		if (drop_chosen_item) 
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SKIDBLADNIR));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;

	//v1.1.79 �˕P�@�Ï��^�R���s���[�^
	case	MON_YORIHIME:
		if (drop_chosen_item) 
			{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_MOON_COMPUTER));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;
	//v1.1.79 �m�[�f���X�@�L�k�̐��
	case	MON_NODENS:
		if (drop_chosen_item) 
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_NODENS_CHARIOT));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;
	//v1.1.91�@��ʓV�g���j�[�N�@�m�b�̖؂̎�
	case MON_URIEL:
	case MON_AZRIEL:
	case MON_RAPHAEL:
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_FORBIDDEN_FRUIT));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	//v1.1.80 �Â̂��́@5%�Ő���
	case MON_ELDER_THING:
		if (drop_chosen_item && one_in_(20))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_ELDER_THINGS_CRYSTAL));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
	break;
	//v1.1.9 �~�~�b�N(��) 1/3�ŋ󂫕r
	case MON_MIMIC_ALCOHOL:
		if (drop_chosen_item && one_in_(3))
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_EMPTY_BOTTLE));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;


	case MON_SUIRYUU:
		///�����̗�
		if (drop_chosen_item)
		{
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_ICESCALE));
			q_ptr->number = 1;
			(void)drop_near(q_ptr, -1, y, x);
		}
		break;

	default:
		if (!drop_chosen_item) break;

		/*:::(, /, [, \, | �͂��ꂼ��ɉ������V���{���̃A�C�e���𗎂Ƃ�*/
		switch (r_ptr->d_char)
		{
		case '(':
			if (dun_level > 0)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_cloak;

				/* Make a cloak */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '/':
			if (dun_level > 4)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_polearm;

				/* Make a poleweapon */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '[':
			if (dun_level > 19)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_armor;

				/* Make a hard armor */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '\\':
			//v1.1.86 �ۑ���|���Ă��A�C�e���𗎂Ƃ��Ȃ��悤�ɂ���
			if (dun_level > 4 && m_ptr->r_idx != MON_MARUTA)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_hafted;

				/* Make a hafted weapon */
				make_object(q_ptr, mo_mode);
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

		case '|':
			///mon �X�g�[���u�����K�[���G�����b�N�ɂ���\��
			//if (m_ptr->r_idx != MON_STORMBRINGER)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Wipe the object */
				object_wipe(q_ptr);

				/* Activate restriction */
				get_obj_num_hook = kind_is_sword;

				/* Make a sword */
				make_object(q_ptr, mo_mode);

				///mod141031 �󂷂���K�Ńf�X�\�[�h�|������󗓃A�C�e����������̂��C��
				if(!q_ptr->k_idx) break;

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;
	///mod140424 �h���S���������
		case 'D':
			if (randint1(r_ptr->level) > 50 && weird_luck() )
			{
				q_ptr = &forge;
				object_wipe(q_ptr);
				object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_DRAGONNAIL));
				(void)drop_near(q_ptr, -1, y, x);
			}
			else if (randint1(r_ptr->level) > 30 && weird_luck() )
			{
				q_ptr = &forge;
				object_wipe(q_ptr);
				object_prep(q_ptr, lookup_kind(TV_MATERIAL, SV_MATERIAL_DRAGONSCALE));
				(void)drop_near(q_ptr, -1, y, x);
			}
			break;

			//v1.1.47 �~�X�e�B�A������l�V���{����|������H���𓾂�悤�ɂ��Ă݂�
		case 'l':
			if (p_ptr->pclass == CLASS_MYSTIA && (r_ptr->flags7 & RF7_AQUATIC) && !(r_ptr->flags1 & RF1_UNIQUE) )
			{
				q_ptr = &forge;
				object_wipe(q_ptr);
				if(m_ptr->r_idx == MON_EEL)
					object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_EEL));
				else
					object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
				(void)drop_near(q_ptr, -1, y, x);

			}

		}
		break;
	}

	//�G�N�X�g�����[�h�Ń��j�[�N��|���ƃA�C�e���J�[�h�𗎂Ƃ�
	///mod160719 �z����|���Ă��J�[�h���Ƃ����̂��C�� �N���[���ł������Ȃ��悤�ɂȂ���
	if(EXTRA_MODE && (r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) && drop_chosen_item)
	{
		int drop_chance;
		
		if(is_gen_unique(m_ptr->r_idx)) drop_chance = 100;
		else drop_chance = 50 + r_ptr->level / 2;
		if(randint0(100) < drop_chance)
		{

			int idx = m_ptr->r_idx;
			if(one_in_(4)) idx = 0;//3/4�̊m���ŗ��Ƃ����L�����N�^�[�ɂ��Ȃ񂾃A�C�e���J�[�h�ɂȂ�
			q_ptr = &forge;
			object_prep(q_ptr, lookup_kind(TV_ITEMCARD, SV_ITEMCARD));
			apply_magic_itemcard(q_ptr, (dun_level+r_ptr->level)/2, idx);
			(void)drop_near(q_ptr, -1, y, x);
		}
	}

	//v1.1.22 ����̓G��|�����Ƃ��e���h���b�v���邱�Ƃɂ���
	if (drop_chosen_item && !gun_free_world)
	{
		int am_mode = AM_GOOD;
		int sv=0;
		int temp;

		switch(m_ptr->r_idx)
		{
		case MON_SEIRAN:
		case MON_RINGO:
			am_mode |= AM_GREAT;
			sv = SV_FIRE_GUN_LUNATIC;
			break;

		case MON_UDONGE:
			am_mode |= AM_GREAT;
			sv = SV_FIRE_GUN_LUNATIC_2;
			break;


		case MON_EAGLE_RABBIT:
			if(one_in_(2))sv = SV_FIRE_GUN_LUNATIC;
			else sv = SV_FIRE_GUN_HANDGUN;
			break;

		case MON_HELL_KAPPA:
			if(one_in_(10)) sv = SV_FIRE_GUN_LASER;
			else if(one_in_(5)) sv = SV_FIRE_GUN_MAGNUM;
			else if(one_in_(3)) sv = SV_FIRE_GUN_SHOTGUN;
			else sv = SV_FIRE_GUN_MUSKET;
			break;
		case MON_GYOKUTO:
		case MON_REISEN2:
			if(one_in_(4)) sv = SV_FIRE_GUN_MUSKET;
			else  sv = SV_FIRE_GUN_INFANTRY;
			break;
		case MON_BAZOOKER:
			if(one_in_(4)) sv = SV_FIRE_GUN_ROCKET;
			else sv = SV_FIRE_GUN_HANDGUN;
			break;
		case MON_GUNNER_OF_CORWIN:
			if(one_in_(5)) sv = SV_FIRE_GUN_MUSKET;
			else if(one_in_(2)) sv = SV_FIRE_GUN_SHOTGUN;
			else sv = SV_FIRE_GUN_INFANTRY;
			break;

		case MON_CLEANING_BOT:
			sv = SV_FIRE_GUN_LASER;
			break;
		case MON_CYBER:
			sv = SV_FIRE_GUN_ROCKET;
			break;
		case MON_CYBER_KING:
			sv = SV_FIRE_GUN_ROCKET;
			am_mode |= AM_GREAT;
			break;

		case MON_ITEM_CARRIER:
			temp = randint1(dun_level) + randint1(30);
			if(weird_luck()) temp += 50;
			if(one_in_(3)) am_mode |= AM_GREAT;

			if(temp < 10) sv = SV_FIRE_GUN_HANDGUN;
			else if(temp < 25) sv = SV_FIRE_GUN_INFANTRY;
			else if(temp < 40) sv = SV_FIRE_GUN_SHOTGUN;
			else if(temp < 60) sv = SV_FIRE_GUN_MUSKET;
			else if(temp < 75) sv = SV_FIRE_GUN_MAGNUM;
			else if(temp < 90) sv = SV_FIRE_GUN_LASER;
			else sv = SV_FIRE_GUN_ROCKET;

		}

		if(sv)
		{
			int k_idx = lookup_kind(TV_GUN, sv);
			q_ptr = &forge;
			object_prep(q_ptr, k_idx);
			apply_magic(q_ptr, object_level, am_mode);
			(void)drop_near(q_ptr, -1, y, x);
		}

	}


	//�P��_��h���b�v
	if(p_ptr->pclass == CLASS_KAGUYA && drop_chosen_item && !ironman_no_fixed_art)
	{
		artifact_type *a_ptr;
		int a_idx;

		for(i=0;i<KAGUYA_QUEST_MAX;i++)
		{
			
			//p_ptr->magic_num[0-4]���V���b�t���ς݃����X�^�[���X�g�ɂȂ��Ă���A
			//���X�g�ɂ��郂���X�^�[��|�����ꍇkaguya_quest_art_list[]�̓����Y���̐_�󐶐�����
			if(m_ptr->r_idx != p_ptr->magic_num1[i]) continue;
			a_idx = kaguya_quest_art_list[i];
			a_ptr = &a_info[a_idx];

			if(a_ptr->cur_num) break;

			if (create_named_art(a_idx, y, x))
			{
				a_ptr->cur_num = 1;

				if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
			}
			else if (!preserve_mode) a_ptr->cur_num = 1;			

			break;
		}

	}



/*:::����ɕʂ̕���ŃA�C�e���h���b�v�����@*/
/*:::chance�l��a_idx�œ���̓G������m���œ���A�[�e�B�t�@�N�g�𗎂Ƃ�����*/
/*:::chance��rand(0-99)������ΊY���A�[�e�B�t�@�N�g�����Bwizard�͕K���`�F�b�N��ʂ�B*/
	///mon item ���m���̃A�[�e�B�t�@�N�g�h���b�v����
	/* Mega-Hack -- drop fixed items */
	if (drop_chosen_item)
	{
		int a_idx = 0;
		int chance = 0;
		int chance_bonus = 0;

		int kamiyama_card_num = count_ability_card(ABL_CARD_KAMIYAMA);

		switch (m_ptr->r_idx)
		{
		case MON_OBERON:
			//v2.0 �I�x������|������A���o�[�̉������R���̕�΂�100%���Ƃ����Ƃɂ���
			if (!a_info[ART_AMBER].cur_num)
			{
				a_idx = ART_AMBER;
				chance = 100;
			}
			else if(!a_info[ART_JUDGE].cur_num)
			{
				a_idx = ART_JUDGE;
				chance = 100;
			}
			break;
		///del131224 T�V���c�p�~
	/*
		case MON_GHB:
			a_idx = ART_GHB;
			chance = 100;
			break;
	*/
		///mod �X�g�u�����G�����b�N
		case MON_STORMBRINGER:
			a_idx = ART_STORMBRINGER;
			chance = 100;
			break;

		case MON_YORIHIME:
			a_idx = ART_YORIHIME;
			chance = 100;
			break;

		case MON_TOYOHIME:
			a_idx = ART_SHIOMITSUTAMA;
			chance = 66;
			break;

		case MON_NITORI:
			a_idx = ART_CROWBAR;
			chance = 80;
			break;

		case MON_YUUKA:
			a_idx = ART_YUUKA;
			chance = 75;
			break;

		case MON_YUGI:
			a_idx = ART_HOSHIGUMA;
			chance = 100;
			break;

		case MON_FLAN:
			a_idx = ART_FLAN;
			chance = 100;
			break;

		case MON_KASEN:
			a_idx = ART_ONIKIRIMARU;
			chance = 25;
			break;


		case MON_MAYUMI:
			a_idx = ART_MAYUMI;
			chance = 100;
			break;


		case MON_ECHIZEN:
			if(!gun_free_world)
			{
				a_idx = ART_CRIMSON;
				chance = 50;
			}
			break;

		case MON_NINJA_SLAYER:
			a_idx = ART_MENPO;
			chance = 10;
			break;

		case MON_GANDALF:
			a_idx = ART_ICANUS;
			chance = 20;
			break;

		///mon ����̌��͋��������̂�5%���x�ɉ�����@�V�̉H�X�؂𑕔����Ă��50%�ɂ��Ă݂�H
		///mod131224 ����̌��h���b�v�͂Ƃ肠����3%��
		case MON_OROCHI:
			a_idx = ART_KUSANAGI;
			chance = 3;
			break;
			///mod150308�@�𗈖�ǉ�
		case MON_G_MUKADE:
		case MON_MOMOYO:
			a_idx = ART_HIRAISHI;
			chance = 50;
			break;

		case MON_DWORKIN:
		case MON_ERIC:
			a_idx = ART_JUDGE;
			chance = 20;
			break;
		case MON_MARIO:
			a_idx = ART_MARIO;
			chance = 10;
			break;

		case MON_SAURON:
			if (one_in_(10))
			{
				a_idx = ART_POWER;
				chance = 100;
			}
			else
			{
				a_idx = ART_AHO;
				chance = 100;
			}
			break;

		case MON_BRAND:
			if (!one_in_(3))
			{
				a_idx = ART_BRAND;
				chance = 25;
			}
			else
			{
				a_idx = ART_WEREWINDLE;
				chance = 33;
			}
			break;


		case MON_CORWIN:

			if(one_in_(3))
			{
				a_idx = ART_JUDGE;
				chance = 20;

			}
			else if (one_in_(2))
			{
				a_idx = ART_GRAYSWANDIR;
				chance = 33;
			}
			else
			{
				a_idx = ART_CORWIN;
				chance = 33;
			}
			break;
		case MON_BENEDICT:
			a_idx = ART_METALHAND;
			chance = 25;
			break;


		case MON_SURTUR:
			a_idx = ART_TWILIGHT;
			chance = 66;
			break;

		case MON_KRONOS:
			a_idx = ART_KRONOS;
			chance = 33;
			break;

		case MON_SARUMAN:
			a_idx = ART_ELENDIL;
			chance = 33;
			break;

		case MON_FIONA:
			a_idx = ART_FIONA;
			chance = 50;
			break;

		case MON_JULIAN:
			a_idx = ART_JULIAN;
			chance = 45;
			break;

		case MON_KLING:
			a_idx = ART_DESTINY;
			chance = 40;
			break;
		///mod131224 �S�G�������a�S�����Ƃ��m����20%�ɂ���
		case MON_GOEMON:
			a_idx = ART_ZANTETSU;
			chance = 20;
			break;

		case MON_HAGEN:
			a_idx = ART_HAGEN;
			chance = 66;
			break;

		case MON_CAINE:
			a_idx = ART_CAINE;
			chance = 50;
			break;

		case MON_BULLGATES:
			a_idx = ART_WINBLOWS;
			chance = 66;
			break;

		case MON_LUNGORTHIN:
			a_idx = ART_CALRIS;
			chance = 50;
			break;

		case MON_JACK_SHADOWS:
			a_idx = ART_JACK;
			chance = 15;
			break;

		case MON_DIO:
			a_idx = ART_STONEMASK;
			chance = 20;
			break;

		case MON_BELD:
			a_idx = ART_SOULCRUSH;
			chance = 10;
			break;

		case MON_PIP:
			a_idx = ART_EXCALIBUR_J;
			chance = 50;
			break;

			///item mon �ύX�\��H
		///del131224 ��ۓ��q�͍폜���邩������Ȃ��̂Ńh���b�v�����������Ƃ�
/*
		case MON_SHUTEN:
			a_idx = ART_SHUTEN_DOJI;
			chance = 33;
			break;
*/
		case MON_GOTHMOG:
			a_idx = ART_GOTHMOG;
			chance = 33;
			break;

		case MON_FUNDIN:
			a_idx = ART_FUNDIN;
			chance = 5;
			break;

		case MON_ROBIN_HOOD:
			a_idx = ART_ROBIN_HOOD;
			chance = 20;///5��20
			break;

		///mod140424 �F�X�h���b�v�ǉ�
		case MON_YOUMU:
			if (one_in_(2))
			{
				a_idx = ART_ROUKANKEN;
				chance = 100;
			}
			else
			{
				a_idx = ART_HAKUROUKEN;
				chance = 100;
			}
			break;

		case MON_IKU:
			a_idx = ART_HEAVENLY_MAIDEN;
			chance = 20;
			break;

		case MON_TENSHI:
			a_idx = ART_HISOU;
			chance = 20;
			break;
		case MON_SUIKA:
			a_idx = ART_IBUKI;
			chance = 30;
			break;
		case MON_SHOU:
			a_idx = ART_HOUTOU;
			chance = 75;
			break;
		case MON_NAZRIN:
			a_idx = ART_NAZRIN;
			chance = 50;
			break;
		case MON_SHINMYOUMARU:
			a_idx = ART_SHINMYOUMARU;
			chance = 50;
			break;
		case  MON_G_CTHULHU:
			a_idx = ART_WRIST;
			chance = 100;
			break;
		case MON_CLOWNPIECE:
			a_idx = ART_CLOWNPIECE;
			chance = 80;
			break;
		case MON_HECATIA1:
			a_idx = ART_HECATIA;
			chance = 100;
			break;
		case MON_YUKARI:
			a_idx = ART_YUKARI;
			chance = 70;
			break;
		case MON_MARISA:
			if (EXTRA_MODE)
			{
				a_idx = ART_GRIMOIRE_OF_MARISA;
				chance = (r_ptr->level - 20);
				if (chance < 0) chance = 0;
			}
			else if (r_ptr->flags2 & RF2_TROPHY)
			{
				a_idx = ART_HANGOKU_SAKE;
				chance = 100;
			}
			break;
		case MON_SUMIREKO:
			if(!gun_free_world)
			{
				a_idx = ART_3DPRINTER_GUN;
				chance = 70;
			}
			break;
		case MON_3FAIRIES:
			a_idx = ART_KODUCHI_TENGU;
			chance = 20;
			break;

		case MON_KOSUZU:
			a_idx = ART_HYAKKI;
			chance = 100;
			break;

		//v1.1.48�ǉ� �t�@�t�i�[������Ă�������̂ЂƂ�
		case MON_FAFNER:
			a_idx = ART_ANDVARI;
			chance = 50;
			break;

		case MON_YUMA:
			a_idx = ART_TOUTETSU;
			chance = 66;
			break;




		}

		//v1.1.86 �u�_�R�ւ̋����v�J�[�h�ɂ��h���b�v���{�[�i�X EXTRA�̃{�[�i�X�Ɣ{�X�ɂ͂Ȃ�Ȃ��悤��������
		if (EXTRA_MODE) chance_bonus = chance;

		if (kamiyama_card_num)
		{
			int prob_add = calc_ability_card_prob(ABL_CARD_KAMIYAMA, kamiyama_card_num);
			chance_bonus += chance * prob_add / 100;
		}

		chance += chance_bonus;

		if (p_ptr->wizard) msg_format("chance:%d (bonus:%d)", chance, chance_bonus);


		if ((a_idx > 0) && ((randint0(100) < chance) || p_ptr->wizard))
		{
			artifact_type *a_ptr = &a_info[a_idx];

			if (!a_ptr->cur_num)
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
		}

		/*:::�_���W�����̎�ɂ��A�[�e�B�t�@�N�g�h���b�v*/
		if ((r_ptr->flags7 & RF7_GUARDIAN) && (d_info[dungeon_type].final_guardian == m_ptr->r_idx))
		{
			/*:::final_object�͐V���̖�Ȃǈ�ʃA�C�e����V�̃_���W�����@���ꂪ�Ȃ��Ƃ��́�����V�Ȃ̂Ł�����������Ȃ������Ƃ��p�Ɋl���̊����̐�������������*/
			int k_idx = d_info[dungeon_type].final_object ? d_info[dungeon_type].final_object
				: lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);

			if (d_info[dungeon_type].final_artifact)
			{
				int a_idx = d_info[dungeon_type].final_artifact;
				artifact_type *a_ptr = &a_info[a_idx];

				if (!a_ptr->cur_num)
				{
					/* Create the artifact */
					if (create_named_art(a_idx, y, x))
					{
						a_ptr->cur_num = 1;

						/* Hack -- Memorize location of artifact in saved floors */
						if (character_dungeon) a_ptr->floor_id = p_ptr->floor_id;
					}
					else if (!preserve_mode) a_ptr->cur_num = 1;

					/* Prevent rewarding both artifact and "default" object */
					/*:::�����������ꂽ�̂Ŋl���̊������h���b�v���Ȃ��悤�ɂ���*/
					if (!d_info[dungeon_type].final_object) k_idx = 0;
				}
			}
			/*:::��ʃA�C�e���̃_���W�������e��V�@�������͕�V�́������łɎ����Ă���Ƃ�*/
			if (k_idx)
			{
				/* Get local object */
				q_ptr = &forge;

				/* Prepare to make a reward */
				object_prep(q_ptr, k_idx);

				apply_magic(q_ptr, object_level, AM_NO_FIXED_ART | AM_GOOD);

				/* Drop it in the dungeon */
				(void)drop_near(q_ptr, -1, y, x);
			}
#ifdef JP
			msg_format("���Ȃ���%s�𐧔e�����I",d_name+d_info[dungeon_type].name);
#else
			msg_format("You have conquered %s!",d_name+d_info[dungeon_type].name);
#endif
			set_deity_bias(DBIAS_WARLIKE, 5);
			set_deity_bias(DBIAS_REPUTATION, 3);

			//v1.1.25 �t���O����
			flag_dungeon_complete[dungeon_type] = 1;

		}
	}

	//v1.1.86 �S���g���̋��d�J�[�h�̌��ʁ@60%�h���b�v��90%�h���b�v��K�����Ƃ����Ƃɂ��A����Ƀh���b�v�ʑ���
	yachie_card_num = count_ability_card(ABL_CARD_YACHIE);

	/*:::�ʏ�̓G�̃A�C�e���h���b�v*/
	/*:::�Ⴆ��20�܂ł̃A�C�e���𗎂Ƃ��G�Ȃ�ADROP_1D2�`DROP_4D2�̃t���O���S�ė����Ă���*/
	/* Determine how much we can drop */
	if ((r_ptr->flags1 & RF1_DROP_60) && (randint0(100) < 60 || yachie_card_num)) number++;
	if ((r_ptr->flags1 & RF1_DROP_90) && (randint0(100) < 90 || yachie_card_num)) number++;
	if  (r_ptr->flags1 & RF1_DROP_1D2) number += damroll(1, 2);
	if  (r_ptr->flags1 & RF1_DROP_2D2) number += damroll(2, 2);
	if  (r_ptr->flags1 & RF1_DROP_3D2) number += damroll(3, 2);
	if  (r_ptr->flags1 & RF1_DROP_4D2) number += damroll(4, 2);


	//v1.1.86 �u�S���g���̋��d�v�J�[�h�̌���
	if(number && yachie_card_num)
	{
		int add = 0;
		int mult = 10;

		if (yachie_card_num>1) mult += (yachie_card_num - 1) * 2;
		if (yachie_card_num >= 9) mult = 30;


		add += number * mult / 100;
		if (randint1(100) <= (number*mult % 100)) add++;

		if (p_ptr->wizard) msg_format("number:%d add:%d", number, add);

		number += add;

	}



	if (cloned && !(r_ptr->flags1 & RF1_UNIQUE))
		number = 0; /* Clones drop no stuff unless Cloning Pits */

	//v1.1.48 �X�[�p�[�n�R�_�̂Ƃ��̓����X�^�[���A�C�e���𗎂Ƃ��Ȃ�(���ꏈ���̃h���b�v�͂���)
	if (SUPER_SHION)
		number = 0;

	if (is_pet(m_ptr) || p_ptr->inside_battle || p_ptr->inside_arena)
		number = 0; /* Pets drop no stuff */
	if (!drop_item && (r_ptr->d_char != '$')) number = 0;
	
	if((r_ptr->flags2 & (RF2_MULTIPLY)) && (r_ptr->r_akills > 1024))
		number = 0; /* Limit of Multiply monster drop */

	//Extra���[�h�Ń_���W����������ɐ������ꂽ�񃆃j�[�N�̓h���b�v������
	if(EXTRA_MODE && number && (m_ptr->mflag & MFLAG_EX_RESTRICT))
	{
		number = randint1(number+1) / 2;
		if(r_ptr->r_akills > 32) number = number * 32 / (r_ptr->r_akills);
	}

	//�얲�͓G��|���Ƃ��ΑK�𓾂�
	if(p_ptr->pclass == CLASS_REIMU && !is_pet(m_ptr))
	{
		int amount = r_ptr->level * r_ptr->level * (50 + p_ptr->stat_ind[A_CHR]) / 200;
		if(!amount) amount = 1;
		if(r_ptr->flags1 & RF1_UNIQUE) amount *= 10;
		if(r_ptr->flags3 & (RF3_KWAI | RF3_UNDEAD | RF3_DEMON)) amount *= 3;
		else if(r_ptr->flags3 & RF3_ANG_CHAOS) amount *= 2;
		else amount /= 2;

		gain_osaisen(amount);
	}

	/* Hack -- handle creeping coins */
	/*:::�N���[�s���O�R�C���p�̍���̎�ގw��*/
	coin_type = force_coin;

	/* Average dungeon and monster levels */
	object_level = (dun_level + r_ptr->level) / 2;

	/* Drop some objects */
	for (j = 0; j < number; j++)
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Make Gold */
		if (do_gold && (!do_item || (randint0(100) < 50)))
		{
			/* Make some gold */
			if (!make_gold(q_ptr)) continue;

			/* XXX XXX XXX */
			dump_gold++;
		}

		/* Make Object */
		else
		{
			/* Make an object */
			if (!make_object(q_ptr, mo_mode)) continue;

			/* XXX XXX XXX */
			dump_item++;
		}

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	/* Reset the object level */
	object_level = base_level;

	/* Reset "coin" type */
	coin_type = 0;


	/* Take note of any dropped treasure */
	/*:::�����X�^�[�����Ƃ��A�C�e���ɂ��Ă̎v���o�X�V�H*/
	if (visible && (dump_item || dump_gold))
	{
		/* Take notes on treasure */
		lore_treasure(m_idx, dump_item, dump_gold);
	}

	//v1.1.17 ���ς����ʐ��͂�|���������b�Z�[�W
	if(p_ptr->pclass == CLASS_JUNKO && (r_ptr->flags3 & RF3_GEN_MOON) && !is_pet(m_ptr))
	{
		junko_msg();
	}


	/* Only process "Quest Monsters" */
	if (!(r_ptr->flags1 & RF1_QUESTOR)) return;
	if (p_ptr->inside_battle) return;
	if (m_ptr->mflag & MFLAG_EPHEMERA) return;


	if ((m_ptr->r_idx == MON_YUKARI)  && !cloned && !p_ptr->inside_arena)
	{
		if(difficulty == DIFFICULTY_EASY)
		{
			msg_print("�u���߂łƂ��B���S�҃��[�h���Ƃł���B�v");
			msg_print("�u���͂����ƍ�����Փx�ŉ�܂��傤�B�v");
			p_ptr->total_winner = TRUE;
		}
		else if(!EXTRA_MODE)
		{
			int tmp;
			if (difficulty >= DIFFICULTY_HARD && one_in_(2) || p_ptr->pclass == CLASS_SEIJA)
			{
				msg_print("�u���Ƃ͔C�����I���₷�݁[�I�v");

			}
			else
			{
				msg_print("�u���̊Ԃɂ���قǂ̗͂��c�v");
				msg_print("�u���傤���Ȃ���ˁB���̐�ɐi�݂�����Ώ���ɂȂ����B�v");
				msg_print("�u�o����Έ�ԉ��ɂ�����҂𖄂ߒ����Ă��ꂽ�珕�����B�v");

			}
			//�m�[�}���͎���|���Έ��މ\
			if(difficulty == DIFFICULTY_NORMAL) p_ptr->total_winner = TRUE;

			/*:::Mega Hack - ����|�������I�x�����N�G�X�g���*/
			tmp = p_ptr->inside_quest;
			p_ptr->inside_quest = QUEST_TAISAI;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
	
			quest[QUEST_TAISAI].status = QUEST_STATUS_TAKEN;
			p_ptr->inside_quest = tmp;
		}
		//Extra���[�h�ł͉������Ȃ�

		if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(500000L);

	}


	if ((m_ptr->r_idx == MON_SERPENT) && !cloned)
	{
		/* Redraw the "title" */
		p_ptr->redraw |= (PR_TITLE);

		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "���Ȃ��͌��z�ؓ{��*�^�̏�����*�ƂȂ����I");

		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_FINAL_QUEST_CLEAR);//v1.1.58

		msg_print("*** CONGRATULATIONS ***");
		msg_print("���Ȃ��̓Q�[�����R���v���[�g���܂����B");
		if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(3000000L);

		//v1.1.25 �t���O����
		flag_dungeon_complete[DUNGEON_CHAOS] = 1;

		//�������X�[�p�[�n�R�_�̏�Ԃ�J��|�����Ƃ��̃t���O����
		if (SUPER_SHION)
		{
			p_ptr->magic_num2[0]++;
		}

	}

	/* Winner? */
	///sys �T�[�y���g�|�����Ƃ��̃��b�Z�[�W�ƃt���O�����@�I�x�����⎇�ɕύX���悤
	if ((m_ptr->r_idx == MON_TAISAI) && !cloned)
	{
		/* Total winner */
		p_ptr->total_winner = TRUE;

		/* Redraw the "title" */
		p_ptr->redraw |= (PR_TITLE);

		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����Ɍ��z�ؓ{�̏����҂ƂȂ����I");

		if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(2000000L);
		if (p_ptr->pclass == CLASS_MAID)
			msg_print("�傩��̃��b�Z�[�W���͂����B���Ȃ��̂��߂ɏj�����J���Ă���邻�����B");

		if ( /* (p_ptr->pclass == CLASS_CHAOS_WARRIOR) || */  (p_ptr->muta2 & MUT2_CHAOS_GIFT))
		{
			msg_format("%s����̐����������B", chaos_patrons[p_ptr->chaos_patron]);
			msg_print("�w�悭������A�薽�̎҂�I�x");
		}

		/* Congratulations */
		msg_print("*** GAME CLEAR ***");

		///msg140304
		msg_print("Q�R�}���h�ŃQ�[�����I�����ăX�R�A���c�����Ƃ��ł��܂��B");


		//v2.0.7 �疒�����X�{�X��|������u100��ڂ̃u���b�N�}�[�P�b�g�v�̃J�[�h���l������
		if (p_ptr->pclass == CLASS_CHIMATA)
		{
			msg_print("���Ȃ��́u�S��ڂ̃u���b�N�}�[�P�b�g�v�̃J�[�h����肵���I");
			p_ptr->magic_num2[ABL_CARD_100TH_MARKET] = 1;
			p_ptr->update |= PU_BONUS;
		}

		//v1.1.25 �t���O����
		//v2.0 ���X�{�X�u���ΐ��N�v��|���Ă��_���W�������e�ɂ͂Ȃ�Ȃ�
		//flag_dungeon_complete[DUNGEON_ANGBAND] = 1;


	}
}

/*
 * Modify the physical damage done to the monster.
 * (for example when it's invulnerable or shielded)
 *
 * ToDo: Accept a damage-type to calculate the modified damage from
 * things like fire, frost, lightning, poison, ... attacks.
 *
 * "type" is not yet used and should be 0.
 */
/*:::�������Ƃ͂��ꃁ�^���̃_���[�W����*/
int mon_damage_mod(monster_type *m_ptr, int dam, bool is_psy_spear)
{
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	if ((r_ptr->flagsr & RFR_RES_ALL) && dam > 0)
	{
		dam /= 100;
		if ((dam == 0) && one_in_(3)) dam = 1;
	}
	
	if(m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D)
	{
		if (is_psy_spear)
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print("�P���n���Ђ��z��؂�􂢂��I");
		}
		//�����U�������̂��肾�����������悤���ђʂ���B�܂�������
		else if (p_ptr->pclass == CLASS_FLAN)
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print("�j��̗͂��Ђ��z���ђʂ����I");
		}
		else if (p_ptr->pclass == CLASS_YUGI)
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print("���Ȃ��̍��r�͂Ђ��z�𕨂Ƃ����Ȃ������I");
		}
		else if(!one_in_(7))
		{
			if (!p_ptr->blind && is_seen(m_ptr)) msg_print("���ȕz�ōU������炳�ꂽ�B");
			return (0);
		}

	}

	if (MON_INVULNER(m_ptr))
	{
		if (is_psy_spear || p_ptr->pclass == CLASS_FLAN || p_ptr->pclass == CLASS_YUGI)
		{
			if (!p_ptr->blind && is_seen(m_ptr))
			{
#ifdef JP

				if( p_ptr->pclass == CLASS_YUGI)
					msg_print("�o���A�������􂢂��I");
				else
					msg_print("�o���A��؂�􂢂��I");
#else
				msg_print("The barrier is penetrated!");
#endif
			}
		}
		else if (!one_in_(PENETRATE_INVULNERABILITY))
		{
			return (0);
		}
	}
	return (dam);
}


/*
 * Calculate experience point to be get
 *
 * Even the 64 bit operation is not big enough to avoid overflaw
 * unless we carefully choose orders of multiplication and division.
 *
 * Get the coefficient first, and multiply (potentially huge) base
 * experience point of a monster later.
 */
/*:::�_���[�W�ʂ���o���l���v�Z����B*/
/*:::�����̍��������X�^�[�͌o���l�������A���B�����X�^�[�͌o���l��������*/
static void get_exp_from_mon(int dam, monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	s32b new_exp;
	u32b new_exp_frac;
	s32b div_h;
	u32b div_l;

	if (!m_ptr->r_idx) return;
	if (is_pet(m_ptr) || p_ptr->inside_battle) return;

	/*
	 * - Ratio of monster's level to player's level effects
	 * - Varying speed effects
	 * - Get a fraction in proportion of damage point
	 */
	new_exp = r_ptr->level * SPEED_TO_ENERGY(m_ptr->mspeed) * dam;
	new_exp_frac = 0;
	div_h = 0L;
	div_l = (p_ptr->max_plv+2) * SPEED_TO_ENERGY(r_ptr->speed);

	//v1.1.89 �S�X�����ł̃O����
	if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[3])
		new_exp *= 2;

	/* Use (average maxhp * 2) as a denominator */
	if (!(r_ptr->flags1 & RF1_FORCE_MAXHP))
		s64b_mul(&div_h, &div_l, 0, r_ptr->hdice * (ironman_nightmare ? 2 : 1) * (r_ptr->hside + 1));
	else
		s64b_mul(&div_h, &div_l, 0, r_ptr->hdice * (ironman_nightmare ? 2 : 1) * r_ptr->hside * 2);

	//�n��ł͌o���l��1/5�ɂȂ�炵���B�X�J���h�~���H
	/* Special penalty in the wilderness */
	if (!dun_level && (!(r_ptr->flags8 & RF8_WILD_ONLY) || !(r_ptr->flags1 & RF1_UNIQUE)))
		s64b_mul(&div_h, &div_l, 0, 5);


	/* Do division first to prevent overflaw */
	s64b_div(&new_exp, &new_exp_frac, div_h, div_l);

	/* Special penalty for mutiply-monster */
	if ((r_ptr->flags2 & RF2_MULTIPLY) || (m_ptr->r_idx == MON_DAWN))
	{
		int monnum_penarty = r_ptr->r_akills / 400;
		if (monnum_penarty > 8) monnum_penarty = 8;

		while (monnum_penarty--)
		{
			/* Divide by 4 */
			s64b_RSHIFT(new_exp, new_exp_frac, 2);
		}
	}
	/*:::Extra���[�h�Ń_���W����������ɐ������ꂽ�񃆃j�[�N��|�������o���l1/4�`1/32*/
	if (m_ptr->mflag & MFLAG_EX_RESTRICT)
	{
		int dev = 2;
		if(r_ptr->r_akills > 32) dev++;
		if(r_ptr->r_akills > 64) dev++;
		if(r_ptr->r_akills > 128) dev++;
		s64b_RSHIFT(new_exp, new_exp_frac, dev);
	}

	/* Finally multiply base experience point of the monster */
	s64b_mul(&new_exp, &new_exp_frac, 0, r_ptr->mexp);

	/* Gain experience */
	gain_exp_64(new_exp, new_exp_frac);
}


/*
 * Decreases monsters hit points, handling monster death.
 *
 * We return TRUE if the monster has been killed (and deleted).
 *
 *:::�����X�^�[�ւ̃_���[�W�Ǝ��S�������s���B�����X�^�[�����񂾂�TRUE��Ԃ��B
 * We announce monster death (using an optional "death message"
 * if given, and a otherwise a generic killed/destroyed message).
 *:::�����ɂ�鎀�S���b�Z�[�W��note�Ŏw�肳���B
 *
 * Only "physical attacks" can induce the "You have slain" message.
 * Missile and Spell attacks will induce the "dies" message, or
 * various "specialized" messages.  Note that "You have destroyed"
 * and "is destroyed" are synonyms for "You have slain" and "dies".
 *
 * Hack -- unseen monsters yield "You have killed it." message.
 *
 * Added fear (DGK) and check whether to print fear messages -CWS
 *
 * Made name, sex, and capitalization generic -BEN-
 *
 * As always, the "ghost" processing is a total hack.
 *
 * Hack -- we "delay" fear messages by passing around a "fear" flag.
 *
 * XXX XXX XXX Consider decreasing monster experience over time, say,
 * by using "(m_exp * m_lev * (m_lev)) / (p_lev * (m_lev + n_killed))"
 * instead of simply "(m_exp * m_lev) / (p_lev)", to make the first
 * monster worth more than subsequent monsters.  This would also need
 * to induce changes in the monster recall code.
 */
///sys mon ���ɂ�郂���X�^�[�ւ̃_���[�W����
//�����X�^�[���烂���X�^�[�ւ̃_���[�W������mon_take_hit_mon()��project_m()���ōs����B
bool mon_take_hit(int m_idx, int dam, bool *fear, cptr note)
{
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	monster_type    exp_mon;

	/* Innocent until proven otherwise */
	bool        innocent = TRUE, thief = FALSE;
	//int         i;
	int         expdam;

	if(m_idx < 1 || m_idx >=m_max)
	{
		msg_format("ERROR:mon_take_hit()��m_idx����������(%d)",m_idx);
		return FALSE;
	}

	if(m_ptr->r_idx < 1 || m_ptr->r_idx >= max_r_idx)
	{
		msg_format("ERROR:mon_take_hit()�Ŏw��O���b�h�̃����X�^�[��r_idx����������(%d)",m_ptr->r_idx);
		delete_monster_idx(m_idx);
		return FALSE;
	}

	if(SAKUYA_WORLD)
	{
		msg_format("ERROR:�u���̐��E�v���Ƀ����X�^�[���_���[�W���󂯂�s����������");
		return FALSE;
	}


	(void)COPY(&exp_mon, m_ptr, monster_type);
/*:::RF7_KILL_EXP(���񂾂Ƃ�����EXP������Ȃ��G�j�łȂ��Ƃ��A�GHP�����ʂɂ��_���[�W�𓾂�*/
	if (!(r_ptr->flags7 & RF7_KILL_EXP))
	{
			/*:::expdam(�o���l�Z�o�Ɏg���_���[�W�l?)���v�Z ��{�̓_���[�W�l���̂܂܂Ń����X�^�[�����񂾂�HP0�܂ł̒l�@*/
		expdam = (m_ptr->hp > dam) ? dam : m_ptr->hp;
		if (r_ptr->flags6 & RF6_HEAL) expdam = (expdam+1) * 2 / 3;

		get_exp_from_mon(expdam, &exp_mon);

		/* Genocided by chaos patron */
		/*:::�����悭������Ȃ��B�J�I�X�p�g�����ɖ��E���ꂽ�Ƃ��̂��߂̏����炵�����Ȃ�����if���̒���?*/
		if (!m_ptr->r_idx) m_idx = 0;
	}

	/* Redraw (later) if needed */
	if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
	if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

	if(MON_CSLEEP(m_ptr))
	{
		if(one_in_(100)) set_deity_bias(DBIAS_REPUTATION, -1); 
		if(one_in_(100)) set_deity_bias(DBIAS_COSMOS, -1); 
	}

	/* Wake it up */
	(void)set_monster_csleep(m_idx, 0);

	///class �E�҃X�e���X
	/* Hack - Cancel any special player stealth magics. -LM- */
	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		set_superstealth(FALSE);
	}

	/* Genocided by chaos patron */
	if (!m_idx) return TRUE;

	/* Hurt it */
	/*:::���ۂɃ_���[�W*/
	m_ptr->hp -= dam;

	/* It is dead now */
	/*:::�����X�^�[�����񂾂�*/
	if (m_ptr->hp < 0)
	{
		char m_name[80];



		/* Extract monster name */
		monster_desc(m_name, m_ptr, MD_TRUE_NAME);

		//���g��6/7�ŕ�������
		///mod150502 �z�K�q���ꏈ�� �u�Ԍ��i�~�V���O�`�j���܁v��������world_monster=-1�ɂȂ��ēG�̎􂢂Ȃǂ��󂯂Ȃ�
		//v1.1.52 �V�A���[�i���ł�6/7����3/4�ց@���̐��ׂ͐V�A���[�i�ɏo�Ȃ��̂ŕς��Ȃ�
		if (m_ptr->r_idx == MON_MOKOU && !one_in_(7) && !world_monster)
		{
			int rez_chance = p_ptr->inside_arena ? 4 : 7;

			if (!one_in_(rez_chance))
			{
				project(m_idx, 7, m_ptr->fy, m_ptr->fx, 300, GF_NUKE, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
				m_ptr->hp = m_ptr->maxhp;
				msg_format("�w -���U���N�V����- �x");
				return FALSE;
			}
		}
		//���ׂ�6/7�ŕ�������
		if (m_ptr->r_idx == MON_SEIJA && !one_in_(7))
		{
			m_ptr->maxhp = m_ptr->max_maxhp;
			m_ptr->hp = m_ptr->maxhp;
			msg_format("%s��|�����B",m_name);
			msg_format("�E�E�Ǝv�������n�ʂɓ]�������̂͒K�̂悤�Ȓn���������B");
			return FALSE;
		}
		// �N�G�X�g�u���s���ۓW�v���͋P�邪���邩����i�Ԃ���������
		if(m_ptr->r_idx == MON_EIRIN && p_ptr->inside_quest == QUEST_MOON_VAULT)
		{
			bool flag_kaguya = FALSE;
			int i;
			for (i = 1; i < m_max; i++)
			{
				monster_type *m2_ptr = &m_list[i];
				if(m2_ptr->r_idx == MON_KAGUYA && !(m2_ptr->smart & SM_CLONED))
				{
					flag_kaguya = TRUE;
					monster_desc(m_name, m2_ptr, 0);
					break;
				}
			}
			if(flag_kaguya)
			{
				msg_format("�u����V��ł���̂�i�ԁI�v");
				msg_format("�P��͉i�Ԃ𕜊��������I");
				m_ptr->maxhp = m_ptr->max_maxhp;
				m_ptr->hp = m_ptr->maxhp;
				return FALSE;
			}

		}

		//���ׁi�U�j�͓|�����Ƃ����O���ς��
		if(m_ptr->r_idx == MON_SEIJA_D)
		{
			sprintf(m_name,"�f�R�C�l�`");
		}

		/*:::��ǐ_�l�̓����ύX*/
		if(p_ptr->prace == RACE_STRAYGOD)
		{
			if(r_ptr->flags1 & RF1_UNIQUE)
			{

				if(r_ptr->flags3 & (RF3_WANTED))//���q�ˎ�
				{
					set_deity_bias(DBIAS_COSMOS, 5);
					set_deity_bias(DBIAS_REPUTATION, 5);
				}
				else if(r_ptr->flags3 & (RF3_GEN_FRIENDLY | RF3_ANG_FRIENDLY))//�F�D���j�[�N
				{
					set_deity_bias(DBIAS_COSMOS, -3);
					set_deity_bias(DBIAS_WARLIKE, 2);
					set_deity_bias(DBIAS_REPUTATION, -2);
				}
				else if(r_ptr->flags3 & RF3_ANG_AMBER)
				{
					set_deity_bias(DBIAS_COSMOS, -5);//�A���o�[�̉���
					set_deity_bias(DBIAS_REPUTATION, 3);
				}
				else if(r_ptr->flags3 & RF3_ANG_CHAOS)
				{
					set_deity_bias(DBIAS_COSMOS, 1);//���א��̓��j�[�N
					if(r_ptr->level > randint1(100)) set_deity_bias(DBIAS_REPUTATION, 1);
				}
				else if(r_ptr->flags3 & RF3_ANG_COSMOS)
				{
					set_deity_bias(DBIAS_COSMOS, -3);//�������̓��j�[�N
				}
				else if(r_ptr->flags3 & RF3_GEN_HUMAN)
				{
					set_deity_bias(DBIAS_COSMOS, -1);//���z���l�Ԑ��̓��j�[�N
					set_deity_bias(DBIAS_WARLIKE, 1);
					set_deity_bias(DBIAS_REPUTATION, -1);
				}
				else if(r_ptr->flags3 & RF3_HUMAN)
				{
					set_deity_bias(DBIAS_COSMOS, -1);//�������l�ԃ��j�[�N
				}

				else if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_KWAI))
				{
					if(one_in_(2))set_deity_bias(DBIAS_COSMOS, 1);//�A���f�b�h�E�����E�d�����j�[�N
					if(r_ptr->level > randint1(50)) set_deity_bias(DBIAS_REPUTATION, 1);
				}

				if(r_ptr->level > p_ptr->lev * 3 / 2 )//�����x�����j�[�N
				{
					set_deity_bias(DBIAS_REPUTATION, 1);
					set_deity_bias(DBIAS_WARLIKE, 1);
				}
				else if(r_ptr->level > p_ptr->lev )
				{
					if((r_ptr->level - p_ptr->lev) > randint1(20)) set_deity_bias(DBIAS_REPUTATION, 1);
				}


			}
			else //�񃆃j�[�N
			{
				if(r_ptr->flags3 & RF3_ANG_CHAOS)
				{
					if(r_ptr->level > randint1(1000)) set_deity_bias(DBIAS_COSMOS, 1);//���א���
				}
				else if(r_ptr->flags3 & RF3_ANG_COSMOS)
				{
					if(r_ptr->level > randint1(200)) set_deity_bias(DBIAS_COSMOS, -1);//��������
				}
				else if(r_ptr->flags3 & RF3_GEN_HUMAN)//���z���l�Ԑ���
				{
					if(r_ptr->level > randint1(300)) set_deity_bias(DBIAS_COSMOS, -1);
					if(r_ptr->level > randint1(300)) set_deity_bias(DBIAS_WARLIKE, 1);
					if(r_ptr->level > randint1(300)) set_deity_bias(DBIAS_REPUTATION, -1);
				}
				else if(r_ptr->flags3 & (RF3_GEN_FRIENDLY | RF3_ANG_FRIENDLY))//�F�D�����X�^�[
				{
					set_deity_bias(DBIAS_COSMOS, -1);
					set_deity_bias(DBIAS_WARLIKE, 1);
					set_deity_bias(DBIAS_REPUTATION, -1);
				}
			}

			//�D�_�Ō������X�^�[��|�����Ƃ�(���􂷂�G�͏���)
			if (((r_ptr->blow[0].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[0].effect == RBE_EAT_GOLD)||
				(r_ptr->blow[1].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[1].effect == RBE_EAT_GOLD)||
				(r_ptr->blow[2].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[2].effect == RBE_EAT_GOLD)||
				(r_ptr->blow[3].effect == RBE_EAT_ITEM)	|| (r_ptr->blow[3].effect == RBE_EAT_GOLD))
				&& !(r_ptr->flags2 & RF2_MULTIPLY))
			{
					if(one_in_(5)) set_deity_bias(DBIAS_COSMOS, 1);
					if(one_in_(10)) set_deity_bias(DBIAS_REPUTATION, 1);
			}
			//�אڂ����G��|�����Ƃ�
			if(m_ptr->cdis == 1)
			{
				if(randint1(p_ptr->lev * 5) < randint1(r_ptr->level)) set_deity_bias(DBIAS_WARLIKE, 1);
			}
			//�����E�ӖځE����m��ԂœG��|������
			if(p_ptr->confused || p_ptr->blind || p_ptr->shero)
			{
				if(randint1(p_ptr->lev * 3) < randint1(r_ptr->level))set_deity_bias(DBIAS_WARLIKE, 1);
				if(randint1(p_ptr->lev * 2) < randint1(r_ptr->level))set_deity_bias(DBIAS_COSMOS, -1);
			}
			//HP�����ȉ��œG��|�����Ƃ�
			if(p_ptr->chp < p_ptr->mhp / 2)
			{
				if(randint1(p_ptr->lev) < randint1(r_ptr->level)) set_deity_bias(DBIAS_WARLIKE, 1);
			}
		}



		/*:::�T���^��|�������̓��ꃁ�b�Z�[�W*/
		if ((m_ptr->r_idx == MON_SANTACLAUS))
			msg_print("��N���X�}�X���~�̂��m�点��");

		/*:::�������|�������̓��ꃁ�b�Z�[�W*/
		//v1.1.48 ���Z��ł̓��b�Z�[�W�o�Ȃ�����B�����ʂ�Ȃ���������Ȃ���
		if ((m_ptr->r_idx == MON_KOKORO) && !p_ptr->inside_battle && !p_ptr->inside_arena)
			msg_format("�y���O�z%s���A�_���W���������ʂ̂��ʂ������A��",player_name);

		/*:::���ʂ��ƃJ�����I���͐��̂������@���ꂼ�ꏭ���������Ⴄ*/
		if (r_info[m_ptr->r_idx].flags7 & RF7_TANUKI)
		{
			/* You might have unmasked Tanuki first time */
			r_ptr = &r_info[m_ptr->r_idx];
			m_ptr->ap_r_idx = m_ptr->r_idx;
			if (r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;
		}

		if (m_ptr->mflag2 & MFLAG2_CHAMELEON)
		{
			/* You might have unmasked Chameleon first time */
			r_ptr = real_r_ptr(m_ptr);
			if (r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;
		}

		//v1.1.51 �V�A���[�i���ł̓����X�^�[�̑œ|���������Ȃ����Ƃɂ���
		if (!p_ptr->inside_arena)
		{
			if (!(m_ptr->smart & SM_CLONED) && !(m_ptr->mflag & MFLAG_EPHEMERA))
			{
				/* When the player kills a Unique, it stays dead */
				if (r_ptr->flags1 & RF1_UNIQUE)
				{
					r_ptr->max_num = 0;

					/* Mega-Hack -- Banor & Lupart */
					/*:::�o�[�m�[�������p�[�g�p�̏����@��l�|������c����V���ɏo�����Ȃ��悤�ɂ���*/
					if ((m_ptr->r_idx == MON_BANOR) || (m_ptr->r_idx == MON_LUPART))
					{
						r_info[MON_BANORLUPART].max_num = 0;
						r_info[MON_BANORLUPART].r_pkills++;
						r_info[MON_BANORLUPART].r_akills++;
						if (r_info[MON_BANORLUPART].r_tkills < MAX_SHORT) r_info[MON_BANORLUPART].r_tkills++;
					}
					else if (m_ptr->r_idx == MON_BANORLUPART)
					{
						r_info[MON_BANOR].max_num = 0;
						r_info[MON_BANOR].r_pkills++;
						r_info[MON_BANOR].r_akills++;
						if (r_info[MON_BANOR].r_tkills < MAX_SHORT) r_info[MON_BANOR].r_tkills++;
						r_info[MON_LUPART].max_num = 0;
						r_info[MON_LUPART].r_pkills++;
						r_info[MON_LUPART].r_akills++;
						if (r_info[MON_LUPART].r_tkills < MAX_SHORT) r_info[MON_LUPART].r_tkills++;
					}
				}

				/* When the player kills a Nazgul, it stays dead */
				/*:::�i�Y�O���̏ꍇ�o���\�������炷*/
				else if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num--;
			}

			/* Count all monsters killed */
			/*:::r_akills���Z�@���́��ɂ�邱�̎푰�̃����X�^�[�œ|���@�m�炸�ɓ|�����Ԃ�܂�*/
			if (r_ptr->r_akills < MAX_SHORT) r_ptr->r_akills++;

			/* Recall even invisible uniques or winners */
			if ((m_ptr->ml && !p_ptr->image) || (r_ptr->flags1 & RF1_UNIQUE))
			{
				/*::: Hack ���m�J�E���g��0�̏ꍇ�����Ƃ�*/
				if (!r_ptr->r_sights) r_ptr->r_sights++;



				/*:::r_pkills���Z ���́��ɂ�邱�̎푰�̃����X�^�[�œ|�� �s���̂Ƃ��񃆃j�[�N��|�����ꍇ���Z����Ȃ�*/
				/* Count kills this life */
				if ((m_ptr->mflag2 & MFLAG2_KAGE) && (r_info[MON_KAGE].r_pkills < MAX_SHORT)) r_info[MON_KAGE].r_pkills++;
				else if (r_ptr->r_pkills < MAX_SHORT) r_ptr->r_pkills++;

				/* Count kills in all lives */
				/*:::r_tkills���Z �S�Ắ��̂��̎푰�̃����X�^�[�œ|�� �s���̂Ƃ��񃆃j�[�N��|�����ꍇ���Z����Ȃ�*/
				if ((m_ptr->mflag2 & MFLAG2_KAGE) && (r_info[MON_KAGE].r_tkills < MAX_SHORT)) r_info[MON_KAGE].r_tkills++;
				else if (r_ptr->r_tkills < MAX_SHORT) r_ptr->r_tkills++;

				/* Hack -- Auto-recall */
				monster_race_track(m_ptr->ap_r_idx);
			}
		//���胂���X�^�[��œ|�����Ƃ��X�N���[���V���b�g�l���B�ۑ����Ă����ăX�R�A���M�̂Ƃ��Ɉꏏ�ɑ���B
		//v1.1.25 �X�R�A���M�����B�ꏊ�������ړ����A�I�x�����Ǝ��̂Ƃ����B�邱�Ƃɂ����B��̃{�X��|������Q�[���I�[�o�[�ɂȂ�΂��̓s�x�㏑�������͂�
#ifdef WORLD_SCORE
			if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_TAISAI || m_ptr->r_idx == MON_YUKARI && difficulty != DIFFICULTY_EXTRA)
			{
				/* Make screen dump */
				screen_dump = make_screen_dump();
			}
#endif

		}

		/* Don't kill Amberites */
		/*:::�A���o�[��������1/2�Ō��̎�*/
		///sys mon �A���o�[�̌��̎� �I�x������100%�ɂ��悤��
		///mod150502 �z�K�q���ꏈ�� �u�Ԍ��i�~�V���O�`�j���܁v��������world_monster=-1�ɂȂ��ēG�̎􂢂Ȃǂ��󂯂Ȃ�
		///mod150711 ���g�p�[�X�g�o�C�t�F�j�b�N�X�̂Ƃ������̎􂢂��󂯂Ȃ�
		if ((r_ptr->flags3 & RF3_ANG_AMBER) && one_in_(2) && !world_monster && !flag_mokou_possess && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int curses = 1 + randint1(3);
			bool stop_ty = FALSE;
			int count = 0;

#ifdef JP
msg_format("%^s�͋��낵�����̎􂢂����Ȃ��ɂ������I", m_name);
#else
			msg_format("%^s puts a terrible blood curse on you!", m_name);
#endif

			curse_equipment(100, 50);

			do
			{
				stop_ty = activate_ty_curse(stop_ty, &count);
			}
			while (--curses);
		}


		/*:::���ɃZ���t*/
		if ((r_ptr->flags2 & RF2_CAN_SPEAK) && 
			!(m_ptr->mflag & MFLAG_NO_SPELL) && 
			!((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] && m_ptr->cdis <= p_ptr->lev / 5 || p_ptr->tim_general[1])) &&
			!p_ptr->silent_floor 
			)
		{
			char line_got[1024];

			/* Dump a message */
#ifdef JP
			if (!get_rnd_line("mondeath_j.txt", m_ptr->r_idx, line_got))
#else
			if (!get_rnd_line("mondeath.txt", m_ptr->r_idx, line_got))
#endif

				msg_format("%^s %s", m_name, line_got);


		}
		///mod140505 ���z���j�[�N�œ|�Z���t�͕ʃt�@�C���@CAN_SPEAK�t���O�s�v
		//v1.1.84 TROPHY�t���O����(�������Ɏ��߂���Ă���)�͒���Ȃ�
		else if(is_gen_unique(m_ptr->ap_r_idx) && !(r_ptr->flags2 & RF2_TROPHY) &&
			!(m_ptr->mflag & MFLAG_NO_SPELL) && 
			!((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] && m_ptr->cdis <= p_ptr->lev / 5 || p_ptr->tim_general[1])) &&
			!p_ptr->silent_floor)
		{
			char line_got[1024];
			if (!get_rnd_line("mondeath_gen.txt", m_ptr->r_idx, line_got))
			msg_format("%^s %s", m_name, line_got);
		}


///del131221 virtue
#if 0
		///sysdel virtue mon �������炵�΂炭�������Ȃ�
		if (!(d_info[dungeon_type].flags1 & DF1_BEGINNER))
		{
			if (!dun_level && !ambush_flag && !p_ptr->inside_arena)
			{
				chg_virtue(V_VALOUR, -1);
			}
			else if (r_ptr->level > dun_level)
			{
				if (randint1(10) <= (r_ptr->level - dun_level))
					chg_virtue(V_VALOUR, 1);
			}
			if (r_ptr->level > 60)
			{
				chg_virtue(V_VALOUR, 1);
			}
			if (r_ptr->level >= 2 * (p_ptr->lev+1))
				chg_virtue(V_VALOUR, 2);
		}
		
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			if (r_ptr->flags3 & (RF3_EVIL | RF3_GOOD)) chg_virtue(V_HARMONY, 2);

			if (r_ptr->flags3 & RF3_GOOD)
			{
				chg_virtue(V_UNLIFE, 2);
				chg_virtue(V_VITALITY, -2);
			}

			if (one_in_(3)) chg_virtue(V_INDIVIDUALISM, -1);
		}

		if (m_ptr->r_idx == MON_BEGGAR || m_ptr->r_idx == MON_LEPER)
		{
			chg_virtue(V_COMPASSION, -1);
		}

		if ((r_ptr->flags3 & RF3_GOOD) &&
			((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100)))
			chg_virtue(V_UNLIFE, 1);

		if (r_ptr->d_char == 'A')
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_FAITH, -2);
			else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100))
			{
				if (r_ptr->flags3 & RF3_GOOD) chg_virtue(V_FAITH, -1);
				else chg_virtue(V_FAITH, 1);
			}
		}
		else if (r_ptr->flags3 & RF3_DEMON)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_FAITH, 2);
			else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint1(100))
				chg_virtue(V_FAITH, 1);
		}

		if ((r_ptr->flags3 & RF3_UNDEAD) && (r_ptr->flags1 & RF1_UNIQUE))
			chg_virtue(V_VITALITY, 2);

		if (r_ptr->r_deaths)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				chg_virtue(V_HONOUR, 10);
			}
			else if ((r_ptr->level) / 10 + (2 * dun_level) >= randint1(100))
			{
				chg_virtue(V_HONOUR, 1);
			}
		}
		if ((r_ptr->flags2 & RF2_MULTIPLY) && (r_ptr->r_akills > 1000) && one_in_(10))
		{
			chg_virtue(V_VALOUR, -1);
		}

		for (i = 0; i < 4; i++)
		{
			if (r_ptr->blow[i].d_dice != 0) innocent = FALSE; /* Murderer! */

			if ((r_ptr->blow[i].effect == RBE_EAT_ITEM)
				|| (r_ptr->blow[i].effect == RBE_EAT_GOLD))

				thief = TRUE; /* Thief! */
		}

		/* The new law says it is illegal to live in the dungeon */
		if (r_ptr->level != 0) innocent = FALSE;

		if (thief)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_JUSTICE, 3);
			else if (1+((r_ptr->level) / 10 + (2 * dun_level))
				>= randint1(100))
				chg_virtue(V_JUSTICE, 1);
		}
		else if (innocent)
		{
			chg_virtue (V_JUSTICE, -1);
		}

		if ((r_ptr->flags3 & RF3_ANIMAL) && !(r_ptr->flags3 & RF3_EVIL) && !(r_ptr->flags4 & ~(RF4_NOMAGIC_MASK))  && !(r_ptr->flags5 & ~(RF5_NOMAGIC_MASK)) && !(r_ptr->flags6 & ~(RF6_NOMAGIC_MASK)))
		{
			if (one_in_(4)) chg_virtue(V_NATURE, -1);
		}
		///virtue�����܂�
#endif
		//v1.1.51 �V�A���[�i���ł̓��j�[�N�œ|���L�^����Ȃ�
		if ((r_ptr->flags1 & RF1_UNIQUE) && record_destroy_uniq && !p_ptr->inside_arena)
		{
			char note_buf[160];
#ifdef JP
			sprintf(note_buf, "%s%s", r_name + r_ptr->name, (m_ptr->smart & SM_CLONED) ? "(�N���[��)" : "");
#else
			sprintf(note_buf, "%s%s", r_name + r_ptr->name, (m_ptr->smart & SM_CLONED) ? "(Clone)" : "");
#endif
			do_cmd_write_nikki(NIKKI_UNIQUE, 0, note_buf);
		}

		/* Make a sound */
		sound(SOUND_KILL);

		//v1.1.24
		if(m_ptr->r_idx == MON_YOUR_BOAT || m_ptr->r_idx == MON_ANCIENT_SHIP)
		{
			msg_format("%^s�͒��񂾁B", m_name);
		}

		/* Death by Missile/Spell attack */
		else if (note)
		{
			msg_format("%^s%s", m_name, note);
		}

		/* Death by physical attack -- invisible monster */
		///sysdel dead �G���E�����b�Z�[�W�@�u������|�����v�u�����͓|�ꂽ�v�����ɂ��Ă�����
		else if (!m_ptr->ml)
		{
#ifdef JP
			///msg131221
			//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//	msg_format("��������������%s���E�����B", m_name);
			//else
			msg_format("%s��|�����B", m_name);
#else
				msg_format("You have killed %s.", m_name);
#endif

		}

		/* Death by Physical attack -- non-living monster */
		else if (!monster_living(r_ptr))
		{
			int i;
			bool explode = FALSE;

			for (i = 0; i < 4; i++)
			{
				if (r_ptr->blow[i].method == RBM_EXPLODE) explode = TRUE;
			}

			/* Special note at death */
			if (explode)
#ifdef JP
				msg_format("%s�͔������ĕ��X�ɂȂ����B", m_name);
#else
				msg_format("%^s explodes into tiny shreds.", m_name);
#endif
			else
			{
#ifdef JP
			//	if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//		msg_format("��������������%s��|�����B", m_name);
			//	else
msg_format("%s��|�����B", m_name);
#else
				msg_format("You have destroyed %s.", m_name);
#endif
			}
		}

		/* Death by Physical attack -- living monster */
		else
		{
#ifdef JP
			///msg131221
			//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			//	msg_format("��������������%s�𑒂苎�����B", m_name);
			//else
			msg_format("%s��|�����B", m_name);
#else
				msg_format("You have slain %s.", m_name);
#endif

		}
///msg131221 del �܋���
#if 0
		if ((r_ptr->flags1 & RF1_UNIQUE) && !(m_ptr->smart & SM_CLONED) && !vanilla_town)
		{
			for (i = 0; i < MAX_KUBI; i++)
			{
				if ((kubi_r_idx[i] == m_ptr->r_idx) && !(m_ptr->mflag2 & MFLAG2_CHAMELEON))
				{
#ifdef JP
msg_format("%s�̎�ɂ͏܋����������Ă���B", m_name);
#else
					msg_format("There is a price on %s's head.", m_name);
#endif
					break;
				}
			}
		}
#endif


		///mod140118 �A���X���|�ꂽ��l�`���S�Ĕ�������
		if (m_ptr->r_idx == MON_ALICE && !world_monster)
		{
			int i;
			bool flag_msg = TRUE;
			monster_type *tmp_m_ptr;

			for (i = m_max - 1; i >= 1; i--)
			{
				/* Access the monster */
				tmp_m_ptr = &m_list[i];

				/* Ignore "dead" monsters */
				if (!tmp_m_ptr->r_idx) continue;
				if (tmp_m_ptr->r_idx != MON_ALICE_DOLL) continue;
				if (!is_hostile(tmp_m_ptr)) continue; //v1.1.51�@�������ǉ�
				if(tmp_m_ptr->ml && flag_msg){
					flag_msg = FALSE;
					msg_print("�A���X�̐l�`�����������I");
				}
				project(i, 3, tmp_m_ptr->fy, tmp_m_ptr->fx, 100, GF_FIRE, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
				delete_monster_idx(i);
				if(p_ptr->chp < 0 ) return (TRUE);
			}
		}

		/*:::�_�ގq��|���ƃN�G�X�g�N���A�O�ɐz�K�q���o�Ă���*/
		if ((m_ptr->r_idx == MON_KANAKO) &&
		    p_ptr->inside_quest == QUEST_KANASUWA)
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			if (summon_named_creature(0, dummy_y, dummy_x, MON_SUWAKO, PM_NO_ENERGY))
			{
				msg_print("�u����ˁI���͎��ƗV�тȂ����I�v");
			}
		}

		/* Generate treasure */
		/*:::�����X�^�[���S�����@�A�C�e���Ȃǂ����Ƃ�*/
		monster_death(m_idx, TRUE);

		/*:::�C�P�^����*/
		/* Mega hack : replace IKETA to BIKETAL */
		if ((m_ptr->r_idx == MON_IKETA) &&
		    !(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;

			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;

			/* Delete the monster */
			delete_monster_idx(m_idx);

			if (summon_named_creature(0, dummy_y, dummy_x, MON_BIKETAL, mode))
			{
#ifdef JP
				msg_print("�u�n�@�b�n�b�n�b�n�I�I�����o�C�P�^�����I�I�v");
#else
				msg_print("Uwa-hahaha!  *I* am Biketal!");
#endif
			}
		}
		//�w�J�[�e�B�A�̒n���������|���ꂽ�猳�̈يE�̑̂��o��
		else if ((m_ptr->r_idx == MON_HECATIA2 || m_ptr->r_idx == MON_HECATIA3) &&
		    !(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;

			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;

			/* Delete the monster */
			delete_monster_idx(m_idx);

			if (summon_named_creature(0, dummy_y, dummy_x, MON_HECATIA1, mode))
			{
				msg_format("%^s�͑̂���サ���I", m_name);
			}
		}
		/*:::���Y�O�X�l�ϐg�����ǉ�*/
		else if ((m_ptr->r_idx == MON_MOZGUS) &&
		    !(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;
			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;
			delete_monster_idx(m_idx);
			if (summon_named_creature(0, dummy_y, dummy_x, MON_MOZGUS2, mode))
			{
#ifdef JP
				msg_print("�u��@��Ղ�������I�I�v");
#endif
			}
		}
		//v1.1.42 �����p���[�A�b�v����
		else if ((m_ptr->r_idx == MON_SHION_1) &&
			!(p_ptr->inside_arena || p_ptr->inside_battle) && !(m_ptr->mflag & MFLAG_EPHEMERA))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			u32b mode = PM_NO_ENERGY;
			if (is_pet(m_ptr)) mode |= PM_FORCE_PET;
			delete_monster_idx(m_idx);
			if (summon_named_creature(0, dummy_y, dummy_x, MON_SHION_2, mode))
			{
				msg_format("%s�̓X�[�p�[�n�R�_�ւƕω������I",m_name);
			}
		}
		else
		{
			/* Delete the monster */
			delete_monster_idx(m_idx);
		}

		/* Prevent bug of chaos patron's reward */
		if (r_ptr->flags7 & RF7_KILL_EXP)
			get_exp_from_mon((long)exp_mon.max_maxhp*2, &exp_mon);
		else
			get_exp_from_mon(((long)exp_mon.max_maxhp+1L) * 9L / 10L, &exp_mon);

		//v1.1.48 �����̓t���A�Ń����X�^�[���|�ꂽ�Ƃ����̌o���l�����̒l���u�s�^�p���[�v�Ƃ��Ē~�ς���
		if (p_ptr->pclass == CLASS_SHION)
		{
			p_ptr->magic_num1[1] += r_info[m_ptr->r_idx].mexp;

		}

		/* Not afraid */
		(*fear) = FALSE;

		/* Monster is dead */
		return (TRUE);
	}

/*:::���������A�����X�^�[������łȂ��Ƃ�*/
#ifdef ALLOW_FEAR
/*:::�����X�^�[�����|���Ă���Ƃ��_���[�W��^����Ƌ��|�������邱�Ƃ�����*/
	/* Mega-Hack -- Pain cancels fear */
	if (MON_MONFEAR(m_ptr) && (dam > 0))
	{
		/* Cure fear */
		if (set_monster_monfear(m_idx, MON_MONFEAR(m_ptr) - randint1(dam)))
		{
			/* No more fear */
			(*fear) = FALSE;
		}
	}
/*:::�����X�^�[�̓_���[�W�ŋ��|���邱�Ƃ�����set_monster_monfear()*/
	/* Sometimes a monster gets scared by damage */
	if (!MON_MONFEAR(m_ptr) && !(r_ptr->flags3 & (RF3_NO_FEAR)))
	{
		/* Percentage of fully healthy */
		int percentage = (100L * m_ptr->hp) / m_ptr->maxhp;

		/*
		 * Run (sometimes) if at 10% or less of max hit points,
		 * or (usually) when hit for half its current hit points
		 */
		if ((randint1(10) >= percentage) ||
		    ((dam >= m_ptr->hp) && (randint0(100) < 80)))
		{
			/* Hack -- note fear */
			(*fear) = TRUE;

			/* XXX XXX XXX Hack -- Add some timed fear */
			(void)set_monster_monfear(m_idx, (randint1(10) +
					  (((dam >= m_ptr->hp) && (percentage > 7)) ?
					   20 : ((11 - percentage) * 5))));
		}
	}

#endif
/*:::���n����炵���������ł͎��s����Ȃ�*/
#if 0
	if (p_ptr->riding && (p_ptr->riding == m_idx) && (dam > 0))
	{
		char m_name[80];

		/* Extract monster name */
		monster_desc(m_name, m_ptr, 0);

		if (m_ptr->hp > m_ptr->maxhp/3) dam = (dam + 1) / 2;
		if (rakuba((dam > 200) ? 200 : dam, FALSE))
		{
#ifdef JP
msg_format("%^s�ɐU�藎�Ƃ��ꂽ�I", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
		}
	}
#endif

	/* Not dead yet */
	return (FALSE);
}


/*
 * Get term size and calculate screen size
 */
void get_screen_size(int *wid_p, int *hgt_p)
{
	Term_get_size(wid_p, hgt_p);
	*hgt_p -= ROW_MAP + 2;
	*wid_p -= COL_MAP + 2;
	if (use_bigtile) *wid_p /= 2;
}


/*
 * Calculates current boundaries
 * Called below and from "do_cmd_locate()".
 */
/*:::�E�B���h�E�̑傫�������o���A���̏��ڐ����v�Z����H*/
void panel_bounds_center(void)
{
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	panel_row_max = panel_row_min + hgt - 1;
	panel_row_prt = panel_row_min - 1;
	panel_col_max = panel_col_min + wid - 1;
	panel_col_prt = panel_col_min - 13;
}


/*
 * Map resizing whenever the main term changes size
 */
void resize_map(void)
{
	/* Only if the dungeon exists */
	if (!character_dungeon) return;
	
	/* Mega-Hack -- no panel yet */
	panel_row_max = 0;
	panel_col_max = 0;

	/* Reset the panels */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;
				
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_TORCH | PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Forget lite/view */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	/* Hack -- update */
	handle_stuff();
	
	/* Redraw */
	Term_redraw();

	/*
	 * Waiting command;
	 * Place the cursor on the player
	 */
	if (can_save) move_cursor_relative(py, px);

	/* Refresh */
	Term_fresh();
}

/*
 * Redraw a term when it is resized
 */
void redraw_window(void)
{
	/* Only if the dungeon exists */
	if (!character_dungeon) return;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Hack -- update */
	handle_stuff();

	/* Redraw */
	Term_redraw();
}


/*
 * Handle a request to change the current panel
 *
 * Return TRUE if the panel was changed.
 *
 * Also used in do_cmd_locate
 */
bool change_panel(int dy, int dx)
{
	int y, x;
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	/* Apply the motion */
	y = panel_row_min + dy * hgt / 2;
	x = panel_col_min + dx * wid / 2;

	/* Verify the row */
	if (y > cur_hgt - hgt) y = cur_hgt - hgt;
	if (y < 0) y = 0;

	/* Verify the col */
	if (x > cur_wid - wid) x = cur_wid - wid;
	if (x < 0) x = 0;

	/* Handle "changes" */
	if ((y != panel_row_min) || (x != panel_col_min))
	{
		/* Save the new panel info */
		panel_row_min = y;
		panel_col_min = x;

		/* Recalculate the boundaries */
		panel_bounds_center();

		/* Update stuff */
		p_ptr->update |= (PU_MONSTERS);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Handle stuff */
		handle_stuff();

		/* Success */
		return (TRUE);
	}

	/* No change */
	return (FALSE);
}

bool change_panel_xy(int y, int x)
{
	int dy = 0, dx = 0;
	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	if (y < panel_row_min) dy = -1;
	if (y > panel_row_max) dy = 1;
	if (x < panel_col_min) dx = -1;
	if (x > panel_col_max) dx = 1;

	if (!dy && !dx) return (FALSE);

	return change_panel(dy, dx);
}


/*
 * Given an row (y) and col (x), this routine detects when a move
 * off the screen has occurred and figures new borders. -RAK-
 *
 * "Update" forces a "full update" to take place.
 *
 * The map is reprinted if necessary, and "TRUE" is returned.
 */
/*:::������ʊO�ɏ����Ȃ��悤�ɂ��鏈���H*/
void verify_panel(void)
{
	int y = py;
	int x = px;
	int wid, hgt;

	int prow_min;
	int pcol_min;
	int max_prow_min;
	int max_pcol_min;

	/* Get size */
	get_screen_size(&wid, &hgt);

	max_prow_min = cur_hgt - hgt;
	max_pcol_min = cur_wid - wid;

	/* Bounds checking */
	if (max_prow_min < 0) max_prow_min = 0;
	if (max_pcol_min < 0) max_pcol_min = 0;

		/* Center on player */
	if (center_player && (center_running || !running))
	{
		/* Center vertically */
		prow_min = y - hgt / 2;
		if (prow_min < 0) prow_min = 0;
		else if (prow_min > max_prow_min) prow_min = max_prow_min;

		/* Center horizontally */
		pcol_min = x - wid / 2;
		if (pcol_min < 0) pcol_min = 0;
		else if (pcol_min > max_pcol_min) pcol_min = max_pcol_min;
	}
	else
	{
		prow_min = panel_row_min;
		pcol_min = panel_col_min;

		/* Scroll screen when 2 grids from top/bottom edge */
		if (y > panel_row_max - 2)
		{
			while (y > prow_min + hgt-1 - 2)
			{
				prow_min += (hgt / 2);
			}
		}

		if (y < panel_row_min + 2)
		{
			while (y < prow_min + 2)
			{
				prow_min -= (hgt / 2);
			}
		}

		if (prow_min > max_prow_min) prow_min = max_prow_min;
		if (prow_min < 0) prow_min = 0;

		/* Scroll screen when 4 grids from left/right edge */
		if (x > panel_col_max - 4)
		{
			while (x > pcol_min + wid-1 - 4)
			{
				pcol_min += (wid / 2);
			}
		}
		
		if (x < panel_col_min + 4)
		{
			while (x < pcol_min + 4)
			{
				pcol_min -= (wid / 2);
			}
		}

		if (pcol_min > max_pcol_min) pcol_min = max_pcol_min;
		if (pcol_min < 0) pcol_min = 0;
	}

	/* Check for "no change" */
	if ((prow_min == panel_row_min) && (pcol_min == panel_col_min)) return;

	/* Save the new panel info */
	panel_row_min = prow_min;
	panel_col_min = pcol_min;

	/* Hack -- optional disturb on "panel change" */
	if (disturb_panel && !center_player) disturb(0, 0);

	/* Recalculate the boundaries */
	panel_bounds_center();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/*
 * Monster health description
 */
cptr look_mon_desc(monster_type *m_ptr, u32b mode)
{
	monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];
	bool         living;
	int          perc;
	cptr desc;
	cptr attitude;
	cptr clone;

	/* Determine if the monster is "living" */
	living = monster_living(ap_r_ptr);

	/* Calculate a health "percentage" */
	perc = 100L * m_ptr->hp / m_ptr->maxhp;

	/* Healthy monsters */
	if (m_ptr->hp >= m_ptr->maxhp)
	{
		/* No damage */
#ifdef JP
		desc = living ? "����" : "���_���[�W";
#else
		desc = living ? "unhurt" : "undamaged";
#endif

	}

	else if (perc >= 60)
	{
#ifdef JP
		desc = living ? "�y��" : "���_���[�W";
#else
		desc = living ? "somewhat wounded" : "somewhat damaged";
#endif

	}

	else if (perc >= 25)
	{
#ifdef JP
		desc = living ? "����" : "���_���[�W";
#else
		desc = living ? "wounded" : "damaged";
#endif

	}

	else if (perc >= 10)
	{
#ifdef JP
	//	desc = living ? "�d��" : "��_���[�W";
		desc = "��_���[�W";

#else
		desc = living ? "badly wounded" : "badly damaged";
#endif

	}

	else 
	{
#ifdef JP
		//desc = living ? "��������" : "�|�ꂩ��";
		desc = "�|�ꂩ��";
#else
		desc = living ? "almost dead" : "almost destroyed";
#endif
	}


	/* Need attitude information? */
	if (!(mode & 0x01))
	{
		/* Full information is not needed */
		attitude = "";
	}
	else if(m_ptr->r_idx == MON_MASTER_KAGUYA)
	{
		attitude = ", ��l";
	}
	else if (is_pet(m_ptr))
	{
#ifdef JP
		attitude = ", �z��";
#else
		attitude = ", pet";
#endif
	}
	else if (is_friendly(m_ptr))
	{
#ifdef JP
		attitude = ", �F�D�I";
#else
		attitude = ", friendly";
#endif
	}
	else if (p_ptr->pclass == CLASS_PARSEE && (m_ptr->mflag & MFLAG_SPECIAL))
	{
		attitude = ", ���i";
	}
	else
	{
#ifdef JP
		attitude = "";
#else
		attitude = "";
#endif
	}


	/* Clone monster? */
	if (m_ptr->smart & SM_CLONED)
	{
		clone = ", clone";
	}
	else if(m_ptr->mflag & MFLAG_EPHEMERA)
	{
		if(m_ptr->r_idx == MON_YOUMU || m_ptr->r_idx == MON_FLAN_4 ) clone = "(���g)";
		else clone = "";
	}
	else if(monster_is_you(m_ptr->r_idx))
	{
		clone = "(2P Color)";
	}
	else
	{
		clone = "";
	}

	/* Display monster's level --- idea borrowed from ToME */
	if (ap_r_ptr->r_tkills && !(m_ptr->mflag2 & MFLAG2_KAGE))
	{
#ifdef JP
		return format("���x��%d, %s%s%s", ap_r_ptr->level, desc, attitude, clone);
#else
		return format("Level %d, %s%s%s", ap_r_ptr->level, desc, attitude, clone);
#endif
	}
	else 
	{
#ifdef JP
		return format("���x��???, %s%s%s", desc, attitude, clone);
#else
		return format("Level ???, %s%s%s", desc, attitude, clone);
#endif
	}
}



/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
/*:::�N�C�b�N�\�[�g�@���s�O��comp_hook��swap_hook���w�肷��@�ڍז���*/
void ang_sort_aux(vptr u, vptr v, int p, int q)
{
	int z, a, b;

	/* Done sort */
	if (p >= q) return;

	/* Pivot */
	z = p;

	/* Begin */
	a = p;
	b = q;

	/* Partition */
	while (TRUE)
	{
		/* Slide i2 */
		while (!(*ang_sort_comp)(u, v, b, z)) b--;

		/* Slide i1 */
		while (!(*ang_sort_comp)(u, v, z, a)) a++;

		/* Done partition */
		if (a >= b) break;

		/* Swap */
		(*ang_sort_swap)(u, v, a, b);

		/* Advance */
		a++, b--;
	}

	/* Recurse left side */
	ang_sort_aux(u, v, p, b);

	/* Recurse right side */
	ang_sort_aux(u, v, b+1, q);
}


/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
void ang_sort(vptr u, vptr v, int n)
{
	/* Sort the array */
	ang_sort_aux(u, v, 0, n-1);
}



/*** Targeting Code ***/


/*
 * Determine is a monster makes a reasonable target
 *
 * The concept of "targeting" was stolen from "Morgul" (?)
 *
 * The player can target any location, or any "target-able" monster.
 *
 * Currently, a monster is "target_able" if it is visible, and if
 * the player can hit it with a projection, and the player is not
 * hallucinating.  This allows use of "use closest target" macros.
 *
 * Future versions may restrict the ability to target "trappers"
 * and "mimics", but the semantics is a little bit weird.
 */
/*:::�w�肵�������X�^�[���^�[�Q�b�g���ēK�؂����肷��*/
bool target_able(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];

	/* Monster must be alive */
	if (!m_ptr->r_idx) return (FALSE);

	/* Hack -- no targeting hallucinations */
	if (p_ptr->image) return (FALSE);

	/* Monster must be visible */
	if (!m_ptr->ml) return (FALSE);

	if (p_ptr->riding && (p_ptr->riding == m_idx)) return (TRUE);

	/* Monster must be projectable */
	if (!projectable(py, px, m_ptr->fy, m_ptr->fx)) return (FALSE);

	/* XXX XXX XXX Hack -- Never target trappers */
	/* if (CLEAR_ATTR && (CLEAR_CHAR)) return (FALSE); */

	/* Assume okay */
	return (TRUE);
}




/*
 * Update (if necessary) and verify (if possible) the target.
 *
 * We return TRUE if the target is "okay" and FALSE otherwise.
 */
/*:::target_who���`�F�b�N���A���݂��^�[�Q�b�g�Ƃ��Ė��Ȃ���΂��̍��W��target_row/col�Ɋi�[����*/
bool target_okay(void)
{
	/* Accept stationary targets */
	if (target_who < 0) return (TRUE);

	/* Check moving targets */
	if (target_who > 0)
	{
		/* Accept reasonable targets */
		if (target_able(target_who))
		{
			monster_type *m_ptr = &m_list[target_who];

			/* Acquire monster location */
			target_row = m_ptr->fy;
			target_col = m_ptr->fx;

			/* Good target */
			return (TRUE);
		}
	}

	/* Assume no target */
	return (FALSE);
}


/*
 * Sorting hook -- comp function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by double-distance to the player.
 */
static bool ang_sort_comp_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	int da, db, kx, ky;

	/* Absolute distance components */
	kx = x[a]; kx -= px; kx = ABS(kx);
	ky = y[a]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	da = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Absolute distance components */
	kx = x[b]; kx -= px; kx = ABS(kx);
	ky = y[b]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	db = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Compare the distances */
	return (da <= db);
}


/*
 * Sorting hook -- comp function -- by importance level of grids
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by level of monster
 */
static bool ang_sort_comp_importance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);
	cave_type *ca_ptr = &cave[y[a]][x[a]];
	cave_type *cb_ptr = &cave[y[b]][x[b]];
	monster_type *ma_ptr = &m_list[ca_ptr->m_idx];
	monster_type *mb_ptr = &m_list[cb_ptr->m_idx];
	monster_race *ap_ra_ptr, *ap_rb_ptr;

	/* The player grid */
	if (y[a] == py && x[a] == px) return TRUE;
	if (y[b] == py && x[b] == px) return FALSE;

	/* Extract monster race */
	if (ca_ptr->m_idx && ma_ptr->ml) ap_ra_ptr = &r_info[ma_ptr->ap_r_idx];
	else ap_ra_ptr = NULL;
	if (cb_ptr->m_idx && mb_ptr->ml) ap_rb_ptr = &r_info[mb_ptr->ap_r_idx];
	else ap_rb_ptr = NULL;

	if (ap_ra_ptr && !ap_rb_ptr) return TRUE;
	if (!ap_ra_ptr && ap_rb_ptr) return FALSE;

	/* Compare two monsters */
	if (ap_ra_ptr && ap_rb_ptr)
	{
		/* Unique monsters first */
		//if ((ap_ra_ptr->flags1 & RF1_UNIQUE) && !(ap_rb_ptr->flags1 & RF1_UNIQUE)) return TRUE;
		//if (!(ap_ra_ptr->flags1 & RF1_UNIQUE) && (ap_rb_ptr->flags1 & RF1_UNIQUE)) return FALSE;

		/* �σp�����[�^���j�[�N�ɂ��Ή������� */
		if ((ap_ra_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) ) && !(ap_rb_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) )) return TRUE;
		if (!(ap_ra_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) ) && (ap_rb_ptr->flags1 & RF1_UNIQUE ||(ap_ra_ptr->flags7 & RF7_UNIQUE2 && ap_ra_ptr->flags7 & RF7_VARIABLE) )) return FALSE;

		/* Shadowers first (���₵���e) */
		if ((ma_ptr->mflag2 & MFLAG2_KAGE) && !(mb_ptr->mflag2 & MFLAG2_KAGE)) return TRUE;
		if (!(ma_ptr->mflag2 & MFLAG2_KAGE) && (mb_ptr->mflag2 & MFLAG2_KAGE)) return FALSE;

 		/* Unknown monsters first */
		if (!ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills) return TRUE;
		if (ap_ra_ptr->r_tkills && !ap_rb_ptr->r_tkills) return FALSE;

		/* Higher level monsters first (if known) */
		if (ap_ra_ptr->r_tkills && ap_rb_ptr->r_tkills)
		{
			if (ap_ra_ptr->level > ap_rb_ptr->level) return TRUE;
			if (ap_ra_ptr->level < ap_rb_ptr->level) return FALSE;
		}

		/* Sort by index if all conditions are same */
		if (ma_ptr->ap_r_idx > mb_ptr->ap_r_idx) return TRUE;
		if (ma_ptr->ap_r_idx < mb_ptr->ap_r_idx) return FALSE;
	}

	/* An object get higher priority */
	if (cave[y[a]][x[a]].o_idx && !cave[y[b]][x[b]].o_idx) return TRUE;
	if (!cave[y[a]][x[a]].o_idx && cave[y[b]][x[b]].o_idx) return FALSE;

	/* Priority from the terrain */
	if (f_info[ca_ptr->feat].priority > f_info[cb_ptr->feat].priority) return TRUE;
	if (f_info[ca_ptr->feat].priority < f_info[cb_ptr->feat].priority) return FALSE;

	/* If all conditions are same, compare distance */
	return ang_sort_comp_distance(u, v, a, b);
}


/*
 * Sorting hook -- swap function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by distance to the player.
 */
static void ang_sort_swap_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	byte temp;

	/* Swap "x" */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;

	/* Swap "y" */
	temp = y[a];
	y[a] = y[b];
	y[b] = temp;
}



/*
 * Hack -- help "select" a location (see below)
 */
static s16b target_pick(int y1, int x1, int dy, int dx)
{
	int i, v;

	int x2, y2, x3, y3, x4, y4;

	int b_i = -1, b_v = 9999;


	/* Scan the locations */
	for (i = 0; i < temp_n; i++)
	{
		/* Point 2 */
		x2 = temp_x[i];
		y2 = temp_y[i];

		/* Directed distance */
		x3 = (x2 - x1);
		y3 = (y2 - y1);

		/* Verify quadrant */
		if (dx && (x3 * dx <= 0)) continue;
		if (dy && (y3 * dy <= 0)) continue;

		/* Absolute distance */
		x4 = ABS(x3);
		y4 = ABS(y3);

		/* Verify quadrant */
		if (dy && !dx && (x4 > y4)) continue;
		if (dx && !dy && (y4 > x4)) continue;

		/* Approximate Double Distance */
		v = ((x4 > y4) ? (x4 + x4 + y4) : (y4 + y4 + x4));

		/* XXX XXX XXX Penalize location */

		/* Track best */
		if ((b_i >= 0) && (v >= b_v)) continue;

		/* Track best */
		b_i = i; b_v = v;
	}

	/* Result */
	return (b_i);
}


/*
 * Hack -- determine if a given location is "interesting"
 */
static bool target_set_accept(int y, int x)
{
	cave_type *c_ptr;

	s16b this_o_idx, next_o_idx = 0;

	/* Bounds */
	if (!(in_bounds(y, x))) return (FALSE);

	/* Player grid is always interesting */
	if (player_bold(y, x)) return (TRUE);


	/* Handle hallucination */
	if (p_ptr->image) return (FALSE);


	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Visible monsters */
	if (c_ptr->m_idx)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];

		/* Visible monsters */
		if (m_ptr->ml) return (TRUE);
	}

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized object */
		if (o_ptr->marked & OM_FOUND) return (TRUE);
	}

	/* Interesting memorized features */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Notice object features */
		if (c_ptr->info & CAVE_OBJECT) return (TRUE);

		/* Feature code (applying "mimic" field) */
		if (have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_NOTICE)) return TRUE;
	}

	/* Nope */
	return (FALSE);
}


/*
 * Prepare the "temp" array for "target_set"
 *
 * Return the number of target_able monsters in the set.
 */
/*:::mode�ɂ���ă����X�^�[��A�C�e���̂���}�X�̈ꗗ���쐬����炵��*/
/*:::TARGET_KILL:�˒��������X�^�[���������Ƀ\�[�g*/
/*:::TARGET_LOOK:���m�ς݃����X�^�[�����x�����Ƀ\�[�g�H*/
//�\�[�g���ꂽ���ʂ́Atemp_x[],temp_y[],temp_n(��)�Ɋi�[�����
static void target_set_prepare(int mode)
{
	int y, x;
	int min_hgt, max_hgt, min_wid, max_wid;

	if (mode & TARGET_KILL)
	{
		/* Inner range */
		min_hgt = MAX((py - MAX_RANGE), 0);
		max_hgt = MIN((py + MAX_RANGE), cur_hgt - 1);
		min_wid = MAX((px - MAX_RANGE), 0);
		max_wid = MIN((px + MAX_RANGE), cur_wid - 1);
	}
	else if(mode & TARGET_FULL_RANGE)
	{
		/* Inner panel */
		min_hgt = 1;
		max_hgt = cur_hgt - 1;
		min_wid = 1;
		max_wid = cur_wid - 1;
	}
	else /* not targetting */
	{
		/* Inner panel */
		min_hgt = panel_row_min;
		max_hgt = panel_row_max;
		min_wid = panel_col_min;
		max_wid = panel_col_max;
	}

	/* Reset "temp" array */
	temp_n = 0;

	/* Scan the current panel */
	for (y = min_hgt; y <= max_hgt; y++)
	{
		for (x = min_wid; x <= max_wid; x++)
		{
			cave_type *c_ptr;

			/* Require "interesting" contents */
			/*:::�G�A�A�C�e���A���ꕨ�̂Ȃǂ����TRUE*/
			if (!target_set_accept(y, x)) continue;

			c_ptr = &cave[y][x];

			/* Require target_able monsters for "TARGET_KILL" */
			if ((mode & (TARGET_KILL)) && !target_able(c_ptr->m_idx)) continue;

			if ((mode & (TARGET_KILL)) && !target_pet && is_pet(&m_list[c_ptr->m_idx])) continue;

			if ((mode & (TARGET_KILL)) && m_list[c_ptr->m_idx].r_idx == MON_MASTER_KAGUYA) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Set the sort hooks */
	/*:::mode�ɂ����ang_sort�ȉ��Ŏg���֐���ύX���Ă���炵��*/
	if (mode & (TARGET_KILL))
	{
		/* Target the nearest monster for shooting */
		/*:::�����Ń\�[�g*/
		ang_sort_comp = ang_sort_comp_distance;
		ang_sort_swap = ang_sort_swap_distance;
	}
	else
	{
		/*:::��ʁA���x���ʂŃ\�[�g�H*/
		/* Look important grids first in Look command */
		ang_sort_comp = ang_sort_comp_importance;
		ang_sort_swap = ang_sort_swap_distance;
	}

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n);

	if (p_ptr->riding && target_pet && (temp_n > 1) && (mode & (TARGET_KILL)))
	{
		byte tmp;

		tmp = temp_y[0];
		temp_y[0] = temp_y[1];
		temp_y[1] = tmp;
		tmp = temp_x[0];
		temp_x[0] = temp_x[1];
		temp_x[1] = tmp;
	}
}
void target_set_prepare_look(){
	target_set_prepare(TARGET_LOOK);
}
void target_set_prepare_full() {
	target_set_prepare(TARGET_FULL_RANGE);
}


/*
 * Evaluate number of kill needed to gain level
 */
static void evaluate_monster_exp(char *buf, monster_type *m_ptr)
{
	monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];
	u32b num;
	s32b exp_mon, exp_adv;
	u32b exp_mon_frac, exp_adv_frac;

	if ((p_ptr->lev >= PY_MAX_LEVEL) || (p_ptr->prace == RACE_ANDROID))
	{
		sprintf(buf,"**");
		return;
	}
	else if (!ap_r_ptr->r_tkills || (m_ptr->mflag2 & MFLAG2_KAGE))
	{
		if (!p_ptr->wizard)
		{
			sprintf(buf,"??");
			return;
		}
	}


	/* The monster's experience point (assuming average monster speed) */
	exp_mon = ap_r_ptr->mexp * ap_r_ptr->level;
	exp_mon_frac = 0;
	s64b_div(&exp_mon, &exp_mon_frac, 0, (p_ptr->max_plv + 2));


	/* Total experience value for next level */
	exp_adv = player_exp[p_ptr->lev -1] * p_ptr->expfact;
	exp_adv_frac = 0;
	s64b_div(&exp_adv, &exp_adv_frac, 0, 100);

	/* Experience value need to get */
	s64b_sub(&exp_adv, &exp_adv_frac, p_ptr->exp, p_ptr->exp_frac);


	/* You need to kill at least one monster to get any experience */
	s64b_add(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);
	s64b_sub(&exp_adv, &exp_adv_frac, 0, 1);

	/* Extract number of monsters needed */
	s64b_div(&exp_adv, &exp_adv_frac, exp_mon, exp_mon_frac);

	/* If 999 or more monsters needed, only display "999". */
	num = MIN(999, exp_adv_frac);

	/* Display the number */
	sprintf(buf,"%03ld", (long int)num);
}


bool show_gold_on_floor = FALSE;

/*
 * Examine a grid, return a keypress.
 *
 * The "mode" argument contains the "TARGET_LOOK" bit flag, which
 * indicates that the "space" key should scan through the contents
 * of the grid, instead of simply returning immediately.  This lets
 * the "look" command get complete information, without making the
 * "target" command annoying.
 *
 * The "info" argument contains the "commands" which should be shown
 * inside the "[xxx]" text.  This string must never be empty, or grids
 * containing monsters will be displayed with an extra comma.
 *
 * Note that if a monster is in the grid, we update both the monster
 * recall info and the health bar info to track that monster.
 *
 * Eventually, we may allow multiple objects per grid, or objects
 * and terrain features in the same grid. XXX XXX XXX
 *
 * This function must handle blindness/hallucination.
 */
static int target_set_aux(int y, int x, int mode, cptr info)
{
	cave_type *c_ptr = &cave[y][x];
	s16b this_o_idx, next_o_idx = 0;
	cptr s1 = "", s2 = "", s3 = "", x_info = "";
	bool boring = TRUE;
	s16b feat;
	feature_type *f_ptr;
	int query = '\001';
	char out_val[MAX_NLEN+80];

#ifdef ALLOW_EASY_FLOOR
	int floor_list[23], floor_num = 0;

	/* Scan all objects in the grid */
	if (easy_floor)
	{
		floor_num = scan_floor(floor_list, y, x, 0x02);

		if (floor_num)
		{
#ifdef JP
			x_info = "x�� ";
#else
			x_info = "x,";
#endif
		}
	}

#endif /* ALLOW_EASY_FLOOR */

	/* Hack -- under the player */
	if (player_bold(y, x))
	{
		/* Description */
#ifdef JP
		s1 = "���Ȃ���";
		s2 = "�̏�";
		s3 = "�ɂ���";
#else
		s1 = "You are ";

		/* Preposition */
		s2 = "on ";
#endif
	}
	else
	{
#ifdef JP
		s1 = "�^�[�Q�b�g:";
#else
		s1 = "Target:";
#endif
	}

	/* Hack -- hallucination */
	if (p_ptr->image)
	{
		cptr name;
		name = "������ȕ�";


		/* Display a message */
#ifdef JP
		sprintf(out_val, "%s%s%s%s [%s]", s1, name, s2, s3, info);
#else
		sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Stop on everything but "return" */
		if ((query != '\r') && (query != '\n')) return query;

		/* Repeat forever */
		return 0;
	}


	/* Actual monsters */
	if (c_ptr->m_idx && m_list[c_ptr->m_idx].ml)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];
		monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];
		char m_name[80];
		bool recall = FALSE;

		/* Not boring */
		boring = FALSE;

		/* Get the monster name ("a kobold") */
		monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);

		/* Hack -- track this monster race */
		monster_race_track(m_ptr->ap_r_idx);

		/* Hack -- health bar for this monster */
		health_track(c_ptr->m_idx);

		/* Hack -- handle stuff */
		handle_stuff();

		/* Interact */
		while (1)
		{
			char acount[10];

			/* Recall */
			if (recall)
			{
				/* Save */
				screen_save();

				/* Recall on screen */
				screen_roff(m_ptr->ap_r_idx, 0);

				/* Hack -- Complete the prompt (again) */
#ifdef JP
				Term_addstr(-1, TERM_WHITE, format("  [r�v %s%s]", x_info, info));
#else
				Term_addstr(-1, TERM_WHITE, format("  [r,%s%s]", x_info, info));
#endif

				/* Command */
				query = inkey();

				/* Restore */
				screen_load();

				/* Normal commands */
				if (query != 'r') break;

				/* Toggle recall */
				recall = FALSE;

				/* Cleare recall text and repeat */
				continue;
			}

			/*** Normal ***/

			/* Describe, and prompt for recall */
			evaluate_monster_exp(acount, m_ptr);

			//���Ɨ��T�̃��b�Z�[�W�ύX
			if (CLASS_RIDING_BACKDANCE && player_bold(y, x))
				s2 = "�̔w��";

#ifdef JP
			sprintf(out_val, "[%s]%s%s(%s)%s%s [r�v %s%s]", acount, s1, m_name, look_mon_desc(m_ptr, 0x01), s2, s3, x_info, info);
#else
			sprintf(out_val, "[%s]%s%s%s%s(%s) [r, %s%s]", acount, s1, s2, s3, m_name, look_mon_desc(m_ptr, 0x01), x_info, info);
#endif

			prt(out_val, 0, 0);

			/* Place cursor */
			move_cursor_relative(y, x);

			/* Command */
			query = inkey();

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = TRUE;
		}

		/* Always stop at "normal" keys */
		if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

		/* Sometimes stop at "space" key */
		if ((query == ' ') && !(mode & (TARGET_LOOK))) return query;

		/* Change the intro */
#ifdef JP
		s1 = "�����";
#else
		s1 = "It is ";
#endif


		/* Hack -- take account of gender */
#ifdef JP
		if (ap_r_ptr->flags1 & (RF1_FEMALE)) s1 = "�ޏ���";
#else
		if (ap_r_ptr->flags1 & (RF1_FEMALE)) s1 = "She is ";
#endif

#ifdef JP
		else if (ap_r_ptr->flags1 & (RF1_MALE)) s1 = "�ނ�";
#else
		else if (ap_r_ptr->flags1 & (RF1_MALE)) s1 = "He is ";
#endif


		/* Use a preposition */
#ifdef JP
		s2 = "��";
		s3 = "�����Ă���";
#else
		s2 = "carrying ";
#endif
		///mod140710 �t�r�_�g��
		if(m_ptr->r_idx >= MON_TSUKUMO_WEAPON1 && m_ptr->r_idx <= MON_TSUKUMO_WEAPON5)
		{
			if(p_ptr->pclass == CLASS_TSUKUMO_MASTER)
			{
				s2 = "��";
				s3 = "�t�r�_��";
			}
			else
			{
				s2 = "��";
				s3 = "���͂����߂����̂�";
			}
		}

		/* Scan all objects being carried */
		for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			char o_name[MAX_NLEN];

			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Obtain an object description */
			object_desc(o_name, o_ptr, 0);

			/* Describe the object */
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

			/* Sometimes stop at "space" key */
			if ((query == ' ') && !(mode & (TARGET_LOOK))) return query;

			/* Change the intro */
#ifdef JP
			s2 = "���܂�";
#else
			s2 = "also carrying ";
#endif
		}

		/* Use a preposition */
#ifdef JP
		s2 = "�̏�";
		s3 = "�ɂ���";
#else
		s2 = "on ";
#endif
	}


#ifdef ALLOW_EASY_FLOOR
	if (floor_num)
	{
		int min_width = 0;

		while (1)
		{
			if (floor_num == 1)
			{
				char o_name[MAX_NLEN];

				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[floor_list[0]];

				/* Describe the object */
				object_desc(o_name, o_ptr, 0);

				/* Message */
#ifdef JP
				sprintf(out_val, "%s%s%s%s[%s]",
					s1, o_name, s2, s3, info);
#else
				sprintf(out_val, "%s%s%s%s [%s]",
					s1, s2, s3, o_name, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);

				/* Command */
				query = inkey();

				/* End this grid */
				return query;
			}

			/* Provide one cushion before item listing  */
			if (boring)
			{
				/* Display rough information about items */
#ifdef JP
				sprintf(out_val, "%s %d�̃A�C�e��%s%s ['x'�ňꗗ, %s]",
					s1, floor_num, s2, s3, info);
#else
				sprintf(out_val, "%s%s%sa pile of %d items [x,%s]",
					s1, s2, s3, floor_num, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);

				/* Command */
				query = inkey();

				/* No request for listing */
				if (query != 'x' && query != ' ') return query;
			}


			/** Display list of items **/

			/* Continue scrolling list if requested */
			while (1)
			{
				int i, o_idx;

				/* Save screen */
				screen_save();

				/* Display */
				show_gold_on_floor = TRUE;
				(void)show_floor(0, y, x, &min_width);
				show_gold_on_floor = FALSE;

				/* Prompt */
#ifdef JP
				sprintf(out_val, "%s %d�̃A�C�e��%s%s [Enter�Ŏ���, %s]",
					s1, floor_num, s2, s3, info);
#else
				sprintf(out_val, "%s%s%sa pile of %d items [Enter,%s]",
					s1, s2, s3, floor_num, info);
#endif
				prt(out_val, 0, 0);


				/* Wait */
				query = inkey();

				/* Load screen */
				screen_load();

				/* Exit unless 'Enter' */
				if (query != '\n' && query != '\r')
				{
					return query;
				}

				/* Get the object being moved. */
				o_idx = c_ptr->o_idx;
 
				/* Only rotate a pile of two or more objects. */
				if (!(o_idx && o_list[o_idx].next_o_idx)) continue;

				/* Remove the first object from the list. */
				excise_object_idx(o_idx);

				/* Find end of the list. */
				i = c_ptr->o_idx;
				while (o_list[i].next_o_idx)
					i = o_list[i].next_o_idx;

				/* Add after the last object. */
				o_list[i].next_o_idx = o_idx;

				/* Loop and re-display the list */
			}
		}

		/* NOTREACHED */
	}
#endif /* ALLOW_EASY_FLOOR */


	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Describe it */
		if (o_ptr->marked & OM_FOUND)
		{
			char o_name[MAX_NLEN];

			/* Not boring */
			boring = FALSE;

			/* Obtain an object description */
			object_desc(o_name, o_ptr, 0);

			/* Describe the object */
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if ((query != '\r') && (query != '\n') && (query != ' ') && (query != 'x')) return query;

			/* Sometimes stop at "space" key */
			if ((query == ' ') && !(mode & TARGET_LOOK)) return query;

			/* Change the intro */
#ifdef JP
			s1 = "�����";
#else
			s1 = "It is ";
#endif


			/* Plurals */
#ifdef JP
			if (o_ptr->number != 1) s1 = "������";
#else
			if (o_ptr->number != 1) s1 = "They are ";
#endif


			/* Preposition */
#ifdef JP
			///mod150719
			if (is_plate_grid(&cave[o_ptr->iy][o_ptr->ix]))
				s2 = "�̉�";
			else
				s2 = "�̏�";

			s3 = "�Ɍ�����";
#else
			s2 = "on ";
#endif

		}
	}


	/* Feature code (applying "mimic" field) */
	feat = get_feat_mimic(c_ptr);

	/* Require knowledge about grid, or ability to see grid */
	if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(y, x))
	{
		/* Forget feature */
		feat = feat_none;
	}

	f_ptr = &f_info[feat];

	/* Terrain feature if needed */
	if (boring || have_flag(f_ptr->flags, FF_REMEMBER))
	{
		cptr name;

		/* Hack -- special handling for quest entrances */
		if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
		{
			/* Set the quest number temporary */
			int old_quest = p_ptr->inside_quest;
			int j;

			/* Clear the text */
			for (j = 0; j < 10; j++) quest_text[j][0] = '\0';
			quest_text_line = 0;

			p_ptr->inside_quest = c_ptr->special;

			/* Get the quest text */
			init_flags = INIT_NAME_ONLY;

			process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
			name = format("�N�G�X�g�u%s�v(%d�K����)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#else
			name = format("the entrance to the quest '%s'(level %d)", quest[c_ptr->special].name, quest[c_ptr->special].level);
#endif

			/* Reset the old quest number */
			p_ptr->inside_quest = old_quest;
		}

		/* Hack -- special handling for building doors */
		else if (have_flag(f_ptr->flags, FF_BLDG) && !p_ptr->inside_arena)
		{
			name = building[f_ptr->subtype].name;
		}
		else if (have_flag(f_ptr->flags, FF_ENTRANCE))
		{
#ifdef JP
			name = format("%s(%d�K����)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#else
			name = format("%s(level %d)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#endif
		}
		else if (have_flag(f_ptr->flags, FF_TOWN))
		{
			name = town[c_ptr->special].name;
		}
		else if (p_ptr->wild_mode && (feat == feat_floor))
		{
#ifdef JP
			name = "��";
#else
			name = "road";
#endif
		}
		else
		{
			name = f_name + f_ptr->name;
		}


		/* Pick a prefix */
		if (*s2 &&
		    ((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
		     (!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE)) ||
		     have_flag(f_ptr->flags, FF_TOWN)))
		{
#ifdef JP
			s2 = "�̒�";
#else
			s2 = "in ";
#endif
		}

		/* Hack -- special introduction for store & building doors -KMW- */
		if (have_flag(f_ptr->flags, FF_STORE) ||
		    have_flag(f_ptr->flags, FF_QUEST_ENTER) ||
		    (have_flag(f_ptr->flags, FF_BLDG) && !p_ptr->inside_arena) ||
		    have_flag(f_ptr->flags, FF_ENTRANCE))
		{
#ifdef JP
			s2 = "�̓���";
#else
			s3 = "";
#endif
		}




#ifndef JP
		else if (have_flag(f_ptr->flags, FF_FLOOR) ||
			 have_flag(f_ptr->flags, FF_TOWN) ||
			 have_flag(f_ptr->flags, FF_SHALLOW) ||
			 have_flag(f_ptr->flags, FF_DEEP))
		{
			s3 ="";
		}
		else
		{
			/* Pick proper indefinite article */
			s3 = (is_a_vowel(name[0])) ? "an " : "a ";
		}
#endif

		/* Display a message */
		if (p_ptr->wizard)
		{
			char f_idx_str[32];
			if (c_ptr->mimic) sprintf(f_idx_str, "%d/%d", c_ptr->feat, c_ptr->mimic);
			else sprintf(f_idx_str, "%d", c_ptr->feat);
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s] %x %s %d %d %d (%d,%d) %d", s1, name, s2, s3, info, c_ptr->info, f_idx_str, c_ptr->dist, c_ptr->cost, c_ptr->when, y, x, travel.cost[y][x]);
#else
			sprintf(out_val, "%s%s%s%s [%s] %x %s %d %d %d (%d,%d)", s1, s2, s3, name, info, c_ptr->info, f_idx_str, c_ptr->dist, c_ptr->cost, c_ptr->when, y, x);
#endif
		}
		else
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Always stop at "normal" keys */
		if ((query != '\r') && (query != '\n') && (query != ' ')) return query;
	}

	/* Stop on everything but "return" */
	if ((query != '\r') && (query != '\n')) return query;

	/* Repeat forever */
	return 0;
}


/*
 * Handle "target" and "look".
 *
 * Note that this code can be called from "get_aim_dir()".
 *
 * All locations must be on the current panel.  Consider the use of
 * "panel_bounds()" to allow "off-panel" targets, perhaps by using
 * some form of "scrolling" the map around the cursor.  XXX XXX XXX
 * That is, consider the possibility of "auto-scrolling" the screen
 * while the cursor moves around.  This may require changes in the
 * "update_mon()" code to allow "visibility" even if off panel, and
 * may require dynamic recalculation of the "temp" grid set.
 *
 * Hack -- targeting/observing an "outer border grid" may induce
 * problems, so this is not currently allowed.
 *
 * The player can use the direction keys to move among "interesting"
 * grids in a heuristic manner, or the "space", "+", and "-" keys to
 * move through the "interesting" grids in a sequential manner, or
 * can enter "location" mode, and use the direction keys to move one
 * grid at a time in any direction.  The "t" (set target) command will
 * only target a monster (as opposed to a location) if the monster is
 * target_able and the "interesting" mode is being used.
 *
 * The current grid is described using the "look" method above, and
 * a new command may be entered at any time, but note that if the
 * "TARGET_LOOK" bit flag is set (or if we are in "location" mode,
 * where "space" has no obvious meaning) then "space" will scan
 * through the description of the current grid until done, instead
 * of immediately jumping to the next "interesting" grid.  This
 * allows the "target" command to retain its old semantics.
 *
 * The "*", "+", and "-" keys may always be used to jump immediately
 * to the next (or previous) interesting grid, in the proper mode.
 *
 * The "return" key may always be used to scan through a complete
 * grid description (forever).
 *
 * This command will cancel any old target, even if used from
 * inside the "look" command.
 */
/*:::�}�b�v���烂���X�^�[��O���b�h��I����target_who/row/col�Ɋi�[�H�ڍז���*/
bool target_set(int mode)
{
	int		i, d, m, t, bd;
	int		y = py;
	int		x = px;

	bool	done = FALSE;

	bool	flag = TRUE;

	char	query;

	char	info[80];

	cave_type		*c_ptr;

	int wid, hgt;
	
	/* Get size */
	get_screen_size(&wid, &hgt);

	/* Cancel target */
	target_who = 0;


	/* Cancel tracking */
	/* health_track(0); */


	/* Prepare the "temp" array */
	target_set_prepare(mode);

	/* Start near the player */
	m = 0;

	/* Interact */
	while (!done)
	{


		/* Interesting grids */
		if (flag && temp_n)
		{
			y = temp_y[m];
			x = temp_x[m];

			/* Set forcus */
			change_panel_xy(y, x);

			if (!(mode & TARGET_LOOK)) prt_path(y, x);

			///mod140428
			fix_floor_item_list(y,x);

			/* Access */
			c_ptr = &cave[y][x];

			/* Allow target */
			if (target_able(c_ptr->m_idx))
			{
#ifdef JP
strcpy(info, "q�~ t�� p�� o�� +�� -�O");
#else
				strcpy(info, "q,t,p,o,+,-,<dir>");
#endif

			}

			/* Dis-allow target */
			else
			{
#ifdef JP
strcpy(info, "q�~ p�� o�� +�� -�O");
#else
				strcpy(info, "q,p,o,+,-,<dir>");
#endif

			}
			
			/* Describe and Prompt */
			while (TRUE){
				query = target_set_aux(y, x, mode, info);
				if(query)break;
			}

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no "direction" */
			d = 0;

			if (use_menu)
			{
				if (query == '\r') query = 't';
			}  

			/* Analyze */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case 't':
				case '.':
				case '5':
				case '0':
				{
					if (target_able(c_ptr->m_idx))
					{
						health_track(c_ptr->m_idx);
						target_who = c_ptr->m_idx;
						target_row = y;
						target_col = x;
						done = TRUE;
					}
					else
					{
						bell();
					}
					break;
				}

				case ' ':
				case '*':
				case '+':
				{
					if (++m == temp_n)
					{
						m = 0;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case '-':
				{
					if (m-- == 0)
					{
						m = temp_n - 1;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel();

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_prepare(mode);

					y = py;
					x = px;
				}

				case 'o':
				{
					flag = FALSE;
					break;
				}

				case 'm':
				{
					break;
				}

				default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					if (!d) bell();
					break;
				}
			}

			/* Hack -- move around */
			if (d)
			{
				/* Modified to scroll to monster */
				int y2 = panel_row_min;
				int x2 = panel_col_min;

				/* Find a new monster */
				i = target_pick(temp_y[m], temp_x[m], ddy[d], ddx[d]);

				/* Request to target past last interesting grid */
				while (flag && (i < 0))
				{
					/* Note the change */
					if (change_panel(ddy[d], ddx[d]))
					{
						int v = temp_y[m];
						int u = temp_x[m];

						/* Recalculate interesting grids */
						target_set_prepare(mode);

						/* Look at interesting grids */
						flag = TRUE;

						/* Find a new monster */
						i = target_pick(v, u, ddy[d], ddx[d]);

						/* Use that grid */
						if (i >= 0) m = i;
					}

					/* Nothing interesting */
					else
					{
						int dx = ddx[d];
						int dy = ddy[d];

						/* Restore previous position */
						panel_row_min = y2;
						panel_col_min = x2;
						panel_bounds_center();

						/* Update stuff */
						p_ptr->update |= (PU_MONSTERS);

						/* Redraw map */
						p_ptr->redraw |= (PR_MAP);

						/* Window stuff */
						p_ptr->window |= (PW_OVERHEAD);

						/* Handle stuff */
						handle_stuff();

						/* Recalculate interesting grids */
						target_set_prepare(mode);

						/* Look at boring grids */
						flag = FALSE;

						/* Move */
						x += dx;
						y += dy;

						/* Do not move horizontally if unnecessary */
						if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
							 ((x > panel_col_min + wid / 2) && (dx < 0)))
						{
							dx = 0;
						}

						/* Do not move vertically if unnecessary */
						if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
							 ((y > panel_row_min + hgt / 2) && (dy < 0)))
						{
							dy = 0;
						}

						/* Apply the motion */
						if ((y >= panel_row_min+hgt) || (y < panel_row_min) ||
						    (x >= panel_col_min+wid) || (x < panel_col_min))
						{
							if (change_panel(dy, dx)) target_set_prepare(mode);
						}

						/* Slide into legality */
						if (x >= cur_wid-1) x = cur_wid - 2;
						else if (x <= 0) x = 1;

						/* Slide into legality */
						if (y >= cur_hgt-1) y = cur_hgt- 2;
						else if (y <= 0) y = 1;
					}
				}

				/* Use that grid */
				m = i;
			}
		}

		/* Arbitrary grids */
		else
		{
			bool move_fast = FALSE;

			if (!(mode & TARGET_LOOK)) prt_path(y, x);

			/* Access */
			c_ptr = &cave[y][x];

			///mod140428
			fix_floor_item_list(y,x);

			/* Default prompt */
#ifdef JP
strcpy(info, "q�~ t�� p�� m�� +�� -�O");
#else
			strcpy(info, "q,t,p,m,+,-,<dir>");
#endif


			/* Describe and Prompt (enable "TARGET_LOOK") */
			while (!(query = target_set_aux(y, x, mode | TARGET_LOOK, info)));

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no direction */
			d = 0;

			if (use_menu)
			{
				if (query == '\r') query = 't';
			}  

			/* Analyze the keypress */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case 't':
				case '.':
				case '5':
				case '0':
				{
					target_who = -1;
					target_row = y;
					target_col = x;
					done = TRUE;
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel();

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_prepare(mode);

					y = py;
					x = px;
				}

				case 'o':
				{
					break;
				}

				case ' ':
				case '*':
				case '+':
				case '-':
				case 'm':
				{
					flag = TRUE;

					m = 0;
					bd = 999;

					/* Pick a nearby monster */
					for (i = 0; i < temp_n; i++)
					{
						t = distance(y, x, temp_y[i], temp_x[i]);

						/* Pick closest */
						if (t < bd)
						{
							m = i;
							bd = t;
						}
					}

					/* Nothing interesting */
					if (bd == 999) flag = FALSE;

					break;
				}

				default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					/* XTRA HACK MOVEFAST */
					if (isupper(query)) move_fast = TRUE;

					if (!d) bell();
					break;
				}
			}

			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if (move_fast)
				{
					int mag = MIN(wid / 2, hgt / 2);
					x += dx * mag;
					y += dy * mag;
				}
				else
				{
					x += dx;
					y += dy;
				}

				/* Do not move horizontally if unnecessary */
				if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
					 ((x > panel_col_min + wid / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
					 ((y > panel_row_min + hgt / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + hgt) || (y < panel_row_min) ||
					 (x >= panel_col_min + wid) || (x < panel_col_min))
				{
					if (change_panel(dy, dx)) target_set_prepare(mode);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;
			}





		}



	}

	/* Forget */
	temp_n = 0;

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Failure to set target */
	if (!target_who) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Get an "aiming direction" from the user.
 *
 * The "dir" is loaded with 1,2,3,4,6,7,8,9 for "actual direction", and
 * "0" for "current target", and "-1" for "entry aborted".
 *
 * Note that "Force Target", if set, will pre-empt user interaction,
 * if there is a usable target already set.
 *
 * Note that confusion over-rides any (explicit?) user choice.
 */
/*:::�^�[�Q�b�g���w�肷��H�L�����Z�����ꂽ��FALSE��Ԃ�*/
bool get_aim_dir(int *dp)
{
	int		dir;

	char	command;

	cptr	p;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

	/* Hack -- auto-target if requested */
	if (use_old_target && target_okay()) dir = 5;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		/* Confusion? */

		/* Verify */
		if (!(*dp == 5 && !target_okay()))
		{
/*			return (TRUE); */
			dir = *dp;
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "���� ('*'�Ń^�[�Q�b�g�I��, ESC�Œ��f)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "���� ('5'�Ń^�[�Q�b�g��, '*'�Ń^�[�Q�b�g�đI��, ESC�Œ��f)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
		}  

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
			case 'T':
			case 't':
			case '.':
			case '5':
			case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
			case '*':
			case ' ':
			case '\r':
			{
				if (target_set(TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Extract the action (if any) */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir)
	{
		project_length = 0; /* reset to default */
		return (FALSE);
	}

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[randint0(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* A "valid" direction was entered */
	return (TRUE);
}



/*
 * Request a "movement" direction (1,2,3,4,6,7,8,9) from the user,
 * and place it into "command_dir", unless we already have one.
 *
 * This function should be used for all "repeatable" commands, such as
 * run, walk, open, close, bash, disarm, spike, tunnel, etc, as well
 * as all commands which must reference a grid adjacent to the player,
 * and which may not reference the grid under the player.  Note that,
 * for example, it is no longer possible to "disarm" or "open" chests
 * in the same grid as the player.
 *
 * Direction "5" is illegal and will (cleanly) abort the command.
 *
 * This function tracks and uses the "global direction", and uses
 * that as the "desired direction", to which "confusion" is applied.
 */
/*:::�v�����v�g���o���ĕ����𕷂�
 *:::dp:����1-9��Ԃ��|�C���^
 *:::under:�������w��\�ɂ��邩�ǂ���
 *:::���������Ȃǂ��Ă���ƃ����_���ɕ�����ւ���
 */
bool get_rep_dir(int *dp, bool under)
{
	int dir;
	cptr prompt;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */
	/*:::���s�[�g���Ȃ炻��dir�l��Ԃ�*/
	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	if (under)
	{
		prompt = _("���� ('.'����, ESC�Œ��f)? ", "Direction ('.' at feet, Escape to cancel)? ");
	}
	else
	{
		prompt = _("���� (ESC�Œ��f)? ", "Direction (Escape to cancel)? ");
	}
	
	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
		/*:::prompt�̕������\�������͂��󂯂�*/
		if (!get_com(prompt, &ch, TRUE)) break;

		/* Look down */
		if ((under) && ((ch == '5') || (ch == '-') || (ch == '.')))
		{
			dir = 5;
		}
		else
		{
			/* Look up the direction */
			/*:::���̓L�[��dir�̐��l�ɕϊ��@���[�O���C�N�L�[�Ή�*/
			dir = get_keymap_dir(ch);

			/* Oops */
			if (!dir) bell();
		}
	}

	/* Prevent weirdness */
	if ((dir == 5) && (!under)) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	/*:::���������n�̍����̂Ƃ������������_���ɕύX*/
	if (p_ptr->confused)
	{
		/* Standard confusion */
		if (randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}
	else if (p_ptr->riding)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		if (MON_CONFUSED(m_ptr))
		{
			/* Standard confusion */
			if (randint0(100) < 75)
			{
				/* Random direction */
				dir = ddd[randint0(8)];
			}
		}
		else if ((r_ptr->flags1 & RF1_RAND_50) && (r_ptr->flags1 & RF1_RAND_25) && (randint0(100) < 50))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
		else if ((r_ptr->flags1 & RF1_RAND_50) && (randint0(100) < 25))
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		if (p_ptr->confused)
		{
			/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
			msg_print("You are confused.");
#endif
		}
		else
		{
			char m_name[80];
			monster_type *m_ptr = &m_list[p_ptr->riding];

			monster_desc(m_name, m_ptr, 0);
			if (MON_CONFUSED(m_ptr))
			{
#ifdef JP
msg_format("%s�͍������Ă���B", m_name);
#else
 msg_format("%^s is confusing.", m_name);

#endif
			}
			else
			{
#ifdef JP
msg_format("%s�͎v���ʂ�ɓ����Ă���Ȃ��B", m_name);
#else
msg_format("You cannot control %s.", m_name);
#endif
			}
		}
	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	/*:::���s�[�g�R�}���h�ɕ�����o�^�H*/
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


bool get_rep_dir2(int *dp)
{
	int dir;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
if (!get_com("���� (ESC�Œ��f)? ", &ch, TRUE)) break;
#else
		if (!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if (!dir) bell();
	}

	/* Prevent weirdness */
	if (dir == 5) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if (p_ptr->confused)
	{
		/* Standard confusion */
		if (randint0(100) < 75)
		{
			/* Random direction */
			dir = ddd[randint0(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}
/*:::�J�I�X�p�g��������̕�V�@�ڍז���*/
void gain_level_reward(int chosen_reward)
{
	object_type *q_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         nasty_chance = 6;
	int         dummy = 0, dummy2 = 0;
	int         type, effect;
	cptr        reward = NULL;
	char o_name[MAX_NLEN];

	int count = 0;

	if (!chosen_reward)
	{
		if (multi_rew) return;
		else multi_rew = TRUE;
	}


	if (p_ptr->lev == 13) nasty_chance = 2;
	else if (!(p_ptr->lev % 13)) nasty_chance = 3;
	else if (!(p_ptr->lev % 14)) nasty_chance = 12;

	if (one_in_(nasty_chance))
		type = randint1(20); /* Allow the 'nasty' effects */
	else
		type = randint1(15) + 5; /* Or disallow them */

	if (type < 1) type = 1;
	if (type > 20) type = 20;
	type--;


#ifdef JP
sprintf(wrath_reason, "%s�̓{��",
		chaos_patrons[p_ptr->chaos_patron]);
#else
	sprintf(wrath_reason, "the Wrath of %s",
		chaos_patrons[p_ptr->chaos_patron]);
#endif


	effect = chaos_rewards[p_ptr->chaos_patron][type];

	if (one_in_(6) && !chosen_reward)
	{
#ifdef JP
msg_format("%^s�͖J���Ƃ��Ă��Ȃ���ˑR�ψق������B",
			chaos_patrons[p_ptr->chaos_patron]);
#else
		msg_format("%^s rewards you with a mutation!",
			chaos_patrons[p_ptr->chaos_patron]);
#endif

		(void)gain_random_mutation(0);
#ifdef JP
		reward = "�ψق����B";
#else
		reward = "mutation";
#endif
	}
	else
	{
	switch (chosen_reward ? chosen_reward : effect)
	{
		///sysdel �J�I�X�p�g�������ȕϗe�@�폜�\��
		case REW_POLY_SLF:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���A�V���Ȃ�p��K�v�Ƃ���I�v");
#else
			msg_print("'Thou needst a new form, mortal!'");
#endif

			do_poly_self();
#ifdef JP
			reward = "�ψق����B";
#else
			reward = "polymorphing";
#endif
			break;
		case REW_GAIN_EXP:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���͗ǂ��s������I������I�v");
#else
			msg_print("'Well done, mortal! Lead on!'");
#endif

			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
				msg_print("�����������N����Ȃ������B");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
msg_print("�X�Ɍo����ς񂾂悤�ȋC������B");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(ee);
#ifdef JP
				reward = "�o���l�𓾂�";
#else
				reward = "experience";
#endif
			}
			break;
		case REW_LOSE_EXP:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���l��A������ɒl�����B�v");
#else
			msg_print("'Thou didst not deserve that, slave.'");
#endif

			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
				msg_print("�����������N����Ȃ������B");
#else
				msg_print("But, nothing happen.");
#endif
			}
			else
			{
				lose_exp(p_ptr->exp / 6);
#ifdef JP
				reward = "�o���l���������B";
#else
				reward = "losing experience";
#endif
			}
			break;
		case REW_GOOD_OBJ:
#ifdef JP
msg_format("%s�̐��������₢��:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s whispers:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u�䂪�^�������������Ɏg���ׂ��B�v");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(py, px, 1, FALSE, FALSE);
#ifdef JP
			reward = "�㎿�ȃA�C�e������ɓ��ꂽ�B";
#else
			reward = "a good item";
#endif
			break;
		case REW_GREA_OBJ:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u�䂪�^�������������Ɏg���ׂ��B�v");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(py, px, 1, TRUE, FALSE);
#ifdef JP
			reward = "�����i�̃A�C�e������ɓ��ꂽ�B";
#else
			reward = "an excellent item";
#endif
			break;
		case REW_CHAOS_WP:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���̍s���͋M�����ɒl����B�v");
#else
			msg_print("'Thy deed hath earned thee a worthy blade.'");
#endif

			/* Get local object */
			///item TVAL �J�I�X�p�g�����̍��ו��퐶��
			///pend ���ו��퐶���@�����O�\�[�h�ŌŒ肵��
			q_ptr = &forge;
			dummy = TV_SWORD;
			dummy2 = SV_WEAPON_LONG_SWORD;
			/*
			switch (randint1(p_ptr->lev))
			{
				case 0: case 1:
					dummy2 = SV_DAGGER;
					break;
				case 2: case 3:
					dummy2 = SV_MAIN_GAUCHE;
					break;
				case 4:
					dummy2 = SV_TANTO;
					break;
				case 5: case 6:
					dummy2 = SV_RAPIER;
					break;
				case 7: case 8:
					dummy2 = SV_SMALL_SWORD;
					break;
				case 9: case 10:
					dummy2 = SV_BASILLARD;
					break;
				case 11: case 12: case 13:
					dummy2 = SV_SHORT_SWORD;
					break;
				case 14: case 15:
					dummy2 = SV_SABRE;
					break;
				case 16: case 17:
					dummy2 = SV_CUTLASS;
					break;
				case 18:
					dummy2 = SV_WAKIZASHI;
					break;
				case 19:
					dummy2 = SV_KHOPESH;
					break;
				case 20:
					dummy2 = SV_TULWAR;
					break;
				case 21:
					dummy2 = SV_BROAD_SWORD;
					break;
				case 22: case 23:
					dummy2 = SV_LONG_SWORD;
					break;
				case 24: case 25:
					dummy2 = SV_SCIMITAR;
					break;
				case 26:
					dummy2 = SV_NINJATO;
					break;
				case 27:
					dummy2 = SV_KATANA;
					break;
				case 28: case 29:
					dummy2 = SV_BASTARD_SWORD;
					break;
				case 30:
					dummy2 = SV_GREAT_SCIMITAR;
					break;
				case 31:
					dummy2 = SV_CLAYMORE;
					break;
				case 32:
					dummy2 = SV_ESPADON;
					break;
				case 33:
					dummy2 = SV_TWO_HANDED_SWORD;
					break;
				case 34:
					dummy2 = SV_FLAMBERGE;
					break;
				case 35:
					dummy2 = SV_NO_DACHI;
					break;
				case 36:
					dummy2 = SV_EXECUTIONERS_SWORD;
					break;
				case 37:
					dummy2 = SV_ZWEIHANDER;
					break;
				case 38:
					dummy2 = SV_HAYABUSA;
					break;
				default:
					dummy2 = SV_BLADE_OF_CHAOS;
			}
			*/
			object_prep(q_ptr, lookup_kind(dummy, dummy2));
			q_ptr->to_h = 3 + randint1(dun_level) % 10;
			q_ptr->to_d = 3 + randint1(dun_level) % 10;
			one_resistance(q_ptr);
			q_ptr->name2 = EGO_WEAPON_CHAOS;

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, py, px);
#ifdef JP
			reward = "(����)�̕������ɓ��ꂽ�B";
#else
			reward = "chaos weapon";
#endif
			break;
		case REW_GOOD_OBS:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���̍s���͋M���񂢂ɒl����B�v");
#else
			msg_print("'Thy deed hath earned thee a worthy reward.'");
#endif

			acquirement(py, px, randint1(2) + 1, FALSE, FALSE);
#ifdef JP
			reward = "�㎿�ȃA�C�e������ɓ��ꂽ�B";
#else
			reward = "good items";
#endif
			break;
		case REW_GREA_OBS:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���l��A���̌��g�ւ̉䂪�ɂ��ݖ����񂢂����邪�悢�B�v");
#else
			msg_print("'Behold, mortal, how generously I reward thy loyalty.'");
#endif

			acquirement(py, px, randint1(2) + 1, TRUE, FALSE);
#ifdef JP
			reward = "�����i�̃A�C�e������ɓ��ꂽ�B";
#else
			reward = "excellent items";
#endif
			break;
		case REW_TY_CURSE:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���l��A������Ȃ�B�v");
#else
			msg_print("'Thou art growing arrogant, mortal.'");
#endif

			(void)activate_ty_curse(FALSE, &count);
#ifdef JP
			reward = "�ЁX�����􂢂�������ꂽ�B";
#else
			reward = "cursing";
#endif
			break;
		case REW_SUMMON_M:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u�䂪���l������A���̘����Ȃ�҂�|���ׂ��I�v");
#else
			msg_print("'My pets, destroy the arrogant mortal!'");
#endif

			for (dummy = 0; dummy < randint1(5) + 1; dummy++)
			{
				(void)summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}
#ifdef JP
			reward = "�����X�^�[���������ꂽ�B";
#else
			reward = "summoning hostile monsters";
#endif
			break;
		case REW_H_SUMMON:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���A��苭���G��K�v�Ƃ���I�v");
#else
			msg_print("'Thou needst worthier opponents!'");
#endif

			activate_hi_summon(py, px, FALSE);
#ifdef JP
			reward = "�����X�^�[���������ꂽ�B";
#else
			reward = "summoning many hostile monsters";
#endif
			break;
		case REW_DO_HAVOC:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���Ɣj�󂱂��䂪��тȂ�I�v");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			call_chaos();
#ifdef JP
			reward = "�J�I�X�̗͂��Q�������B";
#else
			reward = "calling chaos";
#endif
			break;
		case REW_GAIN_ABL:
#ifdef JP
msg_format("%s�̐����苿����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s rings out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���܂�̂��A���l��B�]�����̓��̂�b����B�v");
#else
			msg_print("'Stay, mortal, and let me mold thee.'");
#endif

			if (one_in_(3) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_inc_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				do_inc_stat(randint0(6));
#ifdef JP
			reward = "�\�͒l���オ�����B";
#else
			reward = "increasing a stat";
#endif
			break;
		case REW_LOSE_ABL:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���l��A�]�͓��ɖO�݂���B�v");
#else
			msg_print("'I grow tired of thee, mortal.'");
#endif

			if (one_in_(3) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_dec_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				(void)do_dec_stat(randint0(6));
#ifdef JP
			reward = "�\�͒l�����������B";
#else
			reward = "decreasing a stat";
#endif
			break;
		case REW_RUIN_ABL:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���A�������邱�Ƃ��w�Ԃׂ��I�v");
msg_print("���Ȃ��͈ȑO���キ�Ȃ����I");
#else
			msg_print("'Thou needst a lesson in humility, mortal!'");
			msg_print("You feel less powerful!");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), TRUE);
			}
#ifdef JP
			reward = "�S�\�͒l�����������B";
#else
			reward = "decreasing all stats";
#endif
			break;
		case REW_POLY_WND:
#ifdef JP
msg_format("%s�̗͂��G���̂��������B",
#else
			msg_format("You feel the power of %s touch you.",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
			do_poly_wounds();
#ifdef JP
			reward = "�����ω������B";
#else
			reward = "polymorphing wounds";
#endif
			break;
		case REW_AUGM_ABL:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u�䂪�����₩�Ȃ鎒�����󂯂Ƃ邪�悢�I�v");
#else
			msg_print("'Receive this modest gift from me!'");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_inc_stat(dummy);
			}
#ifdef JP
			reward = "�S�\�͒l���オ�����B";
#else
			reward = "increasing all stats";
#endif
			break;
		case REW_HURT_LOT:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u�ꂵ�ނ��悢�A���\�ȋ����҂�I�v");
#else
			msg_print("'Suffer, pathetic fool!'");
#endif

			fire_ball(GF_DISINTEGRATE, 0, p_ptr->lev * 4, 4);
			take_hit(DAMAGE_NOESCAPE, p_ptr->lev * 4, wrath_reason, -1);
#ifdef JP
			reward = "�����̋������������B";
#else
			reward = "generating disintegration ball";
#endif
			break;
	   case REW_HEAL_FUL:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u�S�邪�悢�A�䂪���l��I�v");
#else
			msg_print("'Rise, my servant!'");
#endif

			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			hp_player(5000);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_res_stat(dummy);
			}
#ifdef JP
			reward = "�̗͂��񕜂����B";
#else
			reward = "healing";
#endif
			break;
		case REW_CURSE_WP:
			if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���A����ɗ��邱�ƂȂ���B�v");
#else
			msg_print("'Thou reliest too much on thy weapon.'");
#endif

			dummy = INVEN_RARM;
			if (buki_motteruka(INVEN_LARM))
			{
				dummy = INVEN_LARM;
				if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
			}
			object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
			(void)curse_weapon(FALSE, dummy);
#ifdef JP
			reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_CURSE_AR:
			if (!inventory[INVEN_BODY].k_idx) break;
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���A�h��ɗ��邱�ƂȂ���B�v");
#else
			msg_print("'Thou reliest too much on thine equipment.'");
#endif

			object_desc(o_name, &inventory[INVEN_BODY], OD_NAME_ONLY);
			(void)curse_armor();
#ifdef JP
			reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_PISS_OFF:
#ifdef JP
msg_format("%s�̐��������₢��:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s whispers:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���{�肵�߂��߂������ׂ��B�v");
#else
			msg_print("'Now thou shalt pay for annoying me.'");
#endif

			switch (randint1(4))
			{
				case 1:
					(void)activate_ty_curse(FALSE, &count);
#ifdef JP
					reward = "�ЁX�����􂢂�������ꂽ�B";
#else
					reward = "cursing";
#endif
					break;
				case 2:
					activate_hi_summon(py, px, FALSE);
#ifdef JP
					reward = "�����X�^�[���������ꂽ�B";
#else
					reward = "summoning hostile monsters";
#endif
					break;
				case 3:
					if (one_in_(2))
					{
						if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
						dummy = INVEN_RARM;
						if (buki_motteruka(INVEN_LARM))
						{
							dummy = INVEN_LARM;
							if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
						}
						object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
						(void)curse_weapon(FALSE, dummy);
#ifdef JP
						reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
						reward = format("destroying %s", o_name);
#endif
					}
					else
					{
						if (!inventory[INVEN_BODY].k_idx) break;
						object_desc(o_name, &inventory[INVEN_BODY], OD_NAME_ONLY);
						(void)curse_armor();
#ifdef JP
						reward = format("%s���j�󂳂ꂽ�B", o_name);
#else
						reward = format("destroying %s", o_name);
#endif
					}
					break;
				default:
					for (dummy = 0; dummy < 6; dummy++)
					{
						(void)dec_stat(dummy, 10 + randint1(15), TRUE);
					}
#ifdef JP
					reward = "�S�\�͒l�����������B";
#else
					reward = "decreasing all stats";
#endif
					break;
			}
			break;
		case REW_WRATH:
#ifdef JP
msg_format("%s�̐��������n����:",
#else
			msg_format("The voice of %s thunders:",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
#ifdef JP
msg_print("�u���ʂ��悢�A���l��I�v");
#else
			msg_print("'Die, mortal!'");
#endif

			take_hit(DAMAGE_LOSELIFE, p_ptr->lev * 4, wrath_reason, -1);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint1(15), FALSE);
			}
			activate_hi_summon(py, px, FALSE);
			(void)activate_ty_curse(FALSE, &count);
			if (one_in_(2))
			{
				dummy = 0;

				if (buki_motteruka(INVEN_RARM))
				{
					dummy = INVEN_RARM;
					if (buki_motteruka(INVEN_LARM) && one_in_(2)) dummy = INVEN_LARM;
				}
				else if (buki_motteruka(INVEN_LARM)) dummy = INVEN_LARM;

				if (dummy) (void)curse_weapon(FALSE, dummy);
			}
			if (one_in_(2)) (void)curse_armor();
			break;
		case REW_DESTRUCT:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u���Ɣj�󂱂��䂪��тȂ�I�v");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			(void)destroy_area(py, px, 25, FALSE,FALSE,FALSE);
#ifdef JP
			reward = "�_���W������*�j��*���ꂽ�B";
#else
			reward = "*destruct*ing dungeon";
#endif
			break;
		case REW_GENOCIDE:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u��A���̓G�𖕎E����I�v");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)symbol_genocide(0, FALSE,0);
#ifdef JP
			reward = "�����X�^�[�����E���ꂽ�B";
#else
			reward = "genociding monsters";
#endif
			break;
		case REW_MASS_GEN:
#ifdef JP
msg_format("%s�̐��������n����:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�u��A���̓G�𖕎E����I�v");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)mass_genocide(0, FALSE);
#ifdef JP
			reward = "�����X�^�[�����E���ꂽ�B";
#else
			reward = "genociding nearby monsters";
#endif
			break;
		case REW_DISPEL_C:
#ifdef JP
msg_format("%s�̗͂��G���U������̂��������I",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("You can feel the power of %s assault your enemies!",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			(void)dispel_monsters(p_ptr->lev * 4);
			break;
		case REW_IGNORE:
#ifdef JP
msg_format("%s�͂��Ȃ��𖳎������B",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s ignores you.",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			break;
		case REW_SER_DEMO:
#ifdef JP
msg_format("%s�͖J���Ƃ��Ĉ����̎g�����悱�����I",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with a demonic servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, SUMMON_DEMON, PM_FORCE_PET))
#ifdef JP
msg_print("��������Ȃ�����...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "�������z���ɂȂ����B";
#else
				reward = "a demonic servant";
#endif

			break;
		case REW_SER_MONS:
#ifdef JP
msg_format("%s�͖J���Ƃ��Ďg�����悱�����I",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with a servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, 0, PM_FORCE_PET))
#ifdef JP
msg_print("��������Ȃ�����...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "�����X�^�[���z���ɂȂ����B";
#else
				reward = "a servant";
#endif

			break;
		case REW_SER_UNDE:
#ifdef JP
msg_format("%s�͖J���Ƃ��ăA���f�b�h�̎g�����悱�����B",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with an undead servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, SUMMON_UNDEAD, PM_FORCE_PET))
#ifdef JP
msg_print("��������Ȃ�����...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "�A���f�b�h���z���ɂȂ����B";
#else
				reward = "an undead servant";
#endif

			break;
		default:
#ifdef JP
msg_format("%s�̐����ǂ�����:",
#else
			msg_format("The voice of %s stammers:",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
#ifdef JP
msg_format("�u���[�A���[�A������ %d/%d�B����͉��H�v", type, effect);
#else
			msg_format("'Uh... uh... the answer's %d/%d, what's the question?'", type, effect);
#endif

	}
	}
	if (reward)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("�p�g�����̕�V��%s", reward));
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("The patron rewards you with %s.", reward));
#endif
	}
}

/*:::���C�h�p�@�傩��̕�V*/
void gain_maid_reward(void)
{
	object_type *q_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         type,i,dummy;
	cptr        reward = NULL;

#ifndef JP
	return;
#endif

	if (multi_rew) return;
	else multi_rew = TRUE;
	q_ptr = &forge;

	sprintf(wrath_reason, "�傩��̂��d�u��");

	type = randint0(100);
	msg_print("�傩��̕��������ł����B");
	msg_print("");


	if(type < 5)
	{	
		msg_print("�������J�����B");
		msg_print("��̔M��Ȍ��オ������Ă���B�͂��݂Ȃ���I");
		reward = "�S�\�͒l���オ�����B";
		for (dummy = 0; dummy < 6; dummy++)
		{
			(void)do_inc_stat(dummy);
		}
	}
	else if(type < 14)
	{
		bool flag = FALSE;
		int mcount = 0;
		int mcount2 = 0;
		msg_print("�������J�����B");
		
		for(i=0;i<6;i++) if(p_ptr->stat_cur[i] < p_ptr->stat_max[i] - 2)flag = TRUE;
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr;
			monster_race *r_ptr;
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			if (!m_ptr->ml) continue;
			if(projectable(py,px,m_ptr->fy,m_ptr->fx)) mcount++;
			if(m_ptr->cdis < MAX_SIGHT) mcount2++;
		}

		if(p_ptr->chp < p_ptr->mhp / 2 || p_ptr->lev < p_ptr->max_plv || flag)
		{
			msg_print("�s���`�̂��Ȃ����傪��܂��Ă��ꂽ�I");
			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			hp_player(5000);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_res_stat(dummy);
			}
			reward = "�񕜂����B";
		}
		else if(mcount > 4)
		{
			msg_print("��̑剹���������n�����I");
			(void)dispel_monsters(p_ptr->lev * 6);
			(void)turn_monsters(p_ptr->lev * 6);

		}
		else if(mcount2 > 9)
		{
			msg_print("��̑剹���������n�����I");
			(void)mass_genocide(0, FALSE);
		}
		else if(p_ptr->lev < 50)
		{
			msg_print("�傩��̉����ȐS�������󂯎�����B");
			if (p_ptr->prace == RACE_ANDROID) 
			{
				acquirement(py, px, 1+randint1(2), TRUE, FALSE);
				reward = "�����ȃA�C�e����^����ꂽ�B";
			}
			else 
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
				reward = "�o���l�𓾂��B";
			}
		}
		else
		{
			msg_print("�傩��̂��j�����󂯎�����B");
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT));
			(void)drop_near(q_ptr, -1, py, px);
				reward = "�A�[�e�B�t�@�N�g�����̊������󂯎�����B";
		}
	}	
	else if(type < 23)
	{	
		msg_print("�������J�����B");
		msg_print("�傩��J�܂̌��t���󂯎�����B����Ȃ�͂��N�����B");
		reward = "�\�͒l���オ�����B";
			(void)do_inc_stat(randint0(6));
	}
	else if(type < 31)
	{
		msg_print("�Վ��{�[�i�X���o���I");
		p_ptr->au += p_ptr->lev * p_ptr->lev * 100;
		reward = "�{�[�i�X���󂯎�����B";
	}
	else if(type < 36)
	{
		msg_print("��͂��Ȃ��̕�d�ɐ[���������Ă���B�J���̕i�������Ă����B");
		reward = "�����ȃA�C�e����^����ꂽ�B";
		if(p_ptr->lev > 24 || one_in_(3)) acquirement(py, px, 1+randint1(2), TRUE, FALSE);
		else acquirement(py, px, 1, TRUE, FALSE);
	}
	else if(type < 43)
	{
		msg_print("�V���������������Ă����I");
		reward = "�������x�����ꂽ�B";
		if(p_ptr->psex == SEX_MALE)	object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_SUIT));
		else						object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_MAID));
		if(p_ptr->lev > 24 || one_in_(3)) apply_magic(q_ptr, object_level, AM_GOOD | AM_GREAT);
		else  apply_magic(q_ptr, object_level, AM_GOOD);
		(void)drop_near(q_ptr, -1, py, px);
	}
	else if(type < 48)
	{
		msg_print("�V�����{�������Ă����I");
		reward = "���@�����x�����ꂽ�B";
		//sval0:����ڂ���sval3:�l���ڂ܂�
		object_prep(q_ptr, lookup_kind(p_ptr->realm1, p_ptr->lev / 14));
		(void)drop_near(q_ptr, -1, py, px);
	}
	else if(type < 55)
	{
		msg_print("�������J�����B");
		msg_print("��͂��Ȃ��̓����ɖ������Ă���B��������������Ă���B");
		reward = "�������x�����ꂽ�B";
		//SVAL11-18�̔�
		object_prep(q_ptr, lookup_kind(TV_CHEST, randint1(8)+10));
		q_ptr->pval = -6; //��{��
		(void)drop_near(q_ptr, -1, py, px);
	}

	else if(type < 62)
	{
		bool anger=FALSE;
	
		msg_print("�������J�����B");
		for(i=INVEN_OUTER;i<=INVEN_FEET;i++)
		{
			u32b flgs[TR_FLAG_SIZE];
			int anger_rate = 0;
			object_type *o_ptr;
			
			if(inventory[i].k_idx==0) continue;
			/*:::�V��̓��݂͔j�󂳂�Ȃ�*/
			if(i==INVEN_HEAD && (p_ptr->prace == RACE_KARASU_TENGU ||  p_ptr->prace == RACE_HAKUROU_TENGU )) continue;
			o_ptr= &inventory[i];
			object_flags(o_ptr, flgs);

			if(i==INVEN_HEAD && o_ptr->weight > 50 || i==INVEN_HANDS && o_ptr->weight > 25 || i==INVEN_FEET && o_ptr->weight > 30 ) anger_rate += 30;
			if(object_is_nameless(o_ptr))anger_rate += 20;
			if(object_is_cursed(o_ptr)) anger_rate += 50;
			if(have_flag(flgs, TR_AGGRAVATE)) anger_rate += 70;
			if(object_is_artifact(o_ptr)) anger_rate /= 4;
			if(o_ptr->pval > 0 && have_flag(flgs, TR_CHR) || have_flag(flgs,TR_SUST_CHR)) anger_rate = 0;
			if(randint1(100) < anger_rate)
			{
				anger = TRUE;
				break;
			}
		}

		if(anger)
		{
			object_type *o_ptr = &inventory[i];
			char		o_name[MAX_NLEN];
			object_desc(o_name, o_ptr, OD_OMIT_PREFIX);
			msg_print("�����ƃG���K���g�ȑ���������Ǝ��ӂ��ꂽ�I");
			msg_format("%s�͕��X�ɍӂ��ď������B",o_name);
			reward = format("%s��j�󂳂ꂽ�B",o_name);
			inven_item_increase(i, -1);
			inven_item_describe(i);
			inven_item_optimize(i);
			calc_android_exp();

		}
		else
		{
			if(one_in_(36))     object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_SPEED));
			else if(one_in_(36))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_ATTACKS));
			else if(one_in_(36))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_LORDLY));
			else if(one_in_(24))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_RES_HOLY));
			else if(one_in_(16))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_TELEPATHY));
			else if(one_in_(8))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_THE_MAGI));
			else if(one_in_(7))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RESIST_POIS));
			else if(one_in_(6))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RES_NETHER));
			else if(one_in_(5))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RES_DISENCHANT));
			else if(one_in_(4))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_RES_CHAOS));
			else if(one_in_(3))object_prep(q_ptr, lookup_kind(TV_RING, SV_RING_MUSCLE));
			else if(one_in_(2))object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_CHARISMA));
			else object_prep(q_ptr, lookup_kind(TV_AMULET, SV_AMULET_RESISTANCE));
			apply_magic(q_ptr,p_ptr->lev*2,AM_NO_FIXED_ART | AM_GOOD);
			(void)drop_near(q_ptr, -1, py, px);
			msg_print("�����̌��g�̖J���ɍ����ȑ����i���������I");
			reward = format("�����ȑ����i���󂯎�����B");
		}
	}
	else if(type < 67)
	{
		msg_print("�������率�̉��������o�����Ȃ����񂾁E�E");
		reward = "�ˑR�ψق�����ꂽ�B";
		gain_random_mutation(0);

	}
	else if(type < 70)
	{
		msg_print("�傪�N���Ɉ��Ă��炵����������z����Ă����B");
		msg_print("���Ȃ��͒����S�𔭊����Č��Ȃ��������Ƃɂ����B");
	}
	else if(type < 73)
	{
		msg_print("���������Ă���Ƃ����v���Ȃ��M�ՂőO�q�I�ȃ|�G����������Ă���B");
		msg_print("���Ȃ��͒����S�𔭊����Č��Ȃ��������Ƃɂ����B");
	}
	else if(type < 76)
	{
		int num;
		msg_print("�������J�����琅�������o�����I");
		fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);
		num = 1 + dun_level/20;
		for (i = 0; i < num; i++) (void)summon_specific(0, py, px, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
	}
	else if(type < 81)
	{
		msg_print("�������J������唚�������I");
		fire_ball(GF_DISINTEGRATE, 0, p_ptr->lev * 4, 4);
		take_hit(DAMAGE_NOESCAPE, p_ptr->lev * 4, wrath_reason, -1);
		reward = "�d�u���󂯂��B";
	}
	else if(type < 87)
	{
		msg_print("�������J�����B");
		msg_print("�o���̂Ȃ����x�i����̎��ӂ��󂯂��B�������獷�������ꂽ�E�E");
		p_ptr->au = p_ptr->au / 10 * (5 + randint0(5));
		reward = "�����������炳�ꂽ�B";
	}
	else if(type < 92)
	{
		msg_print("�����Ă��̂܂܂��Ȃ��̊z�Ɏh�������B");
		reward = "�d�u���󂯂��B";
		take_hit(DAMAGE_NOESCAPE, damroll(7,7), wrath_reason, -1);
		set_stun(105);
	}
	else if(type < 99)
	{
		int count = 0;	
		char		o_name[MAX_NLEN];

		msg_print("�������J�����B");
		msg_print("��͑����@���������炵���E�E");
		switch (randint1(4))
		{
			case 1:
				(void)activate_ty_curse(FALSE, &count);
				reward = "�ЁX�����􂢂�������ꂽ�B";
				break;
			case 2:
				activate_hi_summon(py, px, FALSE);
				reward = "�����X�^�[���������ꂽ�B";
				break;
			case 3:
				if (one_in_(2))
				{
					if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)) break;
					dummy = INVEN_RARM;
					if (buki_motteruka(INVEN_LARM))
					{
						dummy = INVEN_LARM;
						if (buki_motteruka(INVEN_RARM) && one_in_(2)) dummy = INVEN_RARM;
					}
					object_desc(o_name, &inventory[dummy], OD_NAME_ONLY);
					(void)curse_weapon(FALSE, dummy);
					reward = format("%s���j�󂳂ꂽ�B", o_name);
				}
				break;
			default:
				for (dummy = 0; dummy < 6; dummy++)
				{
					(void)dec_stat(dummy, 10 + randint1(15), TRUE);
				}
				reward = "�S�\�͒l��������ꂽ�B";
				break;
		}
	}

	else
	{
		int num;
		int count = 0;

		msg_print("�������J�����B");
		screen_save();
		Term_erase(12, 19, 255);
		Term_erase(12, 18, 255);
		Term_erase(12, 17, 255);
		Term_erase(12, 16, 255);
		Term_erase(12, 15, 255);
		put_str("���ʂ��悢�B", 17 , 40);
		(void)inkey();
		screen_load();
		take_hit(DAMAGE_LOSELIFE, p_ptr->lev * 4, wrath_reason, -1);
		for (dummy = 0; dummy < 6; dummy++)
		{
			if(one_in_(3)) (void)dec_stat(dummy, 10 + randint1(15), TRUE);
			else (void)dec_stat(dummy, 10 + randint1(15), FALSE);
		}
		activate_hi_summon(py, px, FALSE);
		(void)activate_ty_curse(FALSE, &count);
		if (one_in_(2))
		{
			dummy = 0;
			if (buki_motteruka(INVEN_RARM))
			{
				dummy = INVEN_RARM;
				if (buki_motteruka(INVEN_LARM) && one_in_(2)) dummy = INVEN_LARM;
			}
			else if (buki_motteruka(INVEN_LARM)) dummy = INVEN_LARM;

			if (dummy) (void)curse_weapon(FALSE, dummy);
		}
		if (one_in_(2)) (void)curse_armor();
		reward = "�N�r�ɂ��ꂩ�����B";
	}

	if (reward)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("�傩��%s", reward));
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("The patron rewards you with %s.", reward));
#endif
	}
}

/*
 * XAngband: determine if a given location is "interesting"
 * based on target_set_accept function.
 */
static bool tgt_pt_accept(int y, int x)
{
	cave_type *c_ptr;

	/* Bounds */
	if (!(in_bounds(y, x))) return (FALSE);

	/* Player grid is always interesting */
	if ((y == py) && (x == px)) return (TRUE);

	/* Handle hallucination */
	if (p_ptr->image) return (FALSE);

	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Interesting memorized features */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Notice stairs */
		if (cave_have_flag_grid(c_ptr, FF_LESS))
			return (TRUE);
		if (cave_have_flag_grid(c_ptr, FF_MORE))
		{
			//v1.1.70 mimic�t���O���̉���K�i(�������̗��Ƃ���)�̓^�[�Q�b�g�̑Ώۂɂ��Ȃ�
			if (c_ptr->mimic)
				return FALSE;
			else
				return (TRUE); 
		}

		/* Notice quest features */
		if (cave_have_flag_grid(c_ptr, FF_QUEST_ENTER)) return (TRUE);
		if (cave_have_flag_grid(c_ptr, FF_QUEST_EXIT)) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * XAngband: Prepare the "temp" array for "tget_pt"
 * based on target_set_prepare funciton.
 */
static void tgt_pt_prepare(void)
{
	int y, x;

	/* Reset "temp" array */
	temp_n = 0;

	if (!expand_list) return;

	/* Scan the current panel */
	for (y = 1; y < cur_hgt; y++)
	{
		for (x = 1; x < cur_wid; x++)
		{
			/* Require "interesting" contents */
			if (!tgt_pt_accept(y, x)) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Target the nearest monster for shooting */
	ang_sort_comp = ang_sort_comp_distance;
	ang_sort_swap = ang_sort_swap_distance;

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n);
}

/*
 * old -- from PsiAngband.
 */
/*:::�ꏊ��I��ŃX�y�[�X�L�[�Ō���@�ڍז���*/
bool tgt_pt(int *x_ptr, int *y_ptr)
{
	char ch = 0;
	int d, x, y, n;
	bool success = FALSE;

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);

	x = px;
	y = py;

	//v1.1.66 ��������̃O���b�h���J�E���g����temp_n,temp_x[],temp_y[]�����Z�b�g���Ă���
	temp_n = 0;

	if (expand_list) 
	{
		tgt_pt_prepare();
		n = 0;
	}

#ifdef JP
	msg_print("�ꏊ��I��ŃX�y�[�X�L�[�������ĉ������B");
#else
	msg_print("Select a point and press space.");
#endif
	msg_flag = FALSE; /* prevents "-more-" message. */

	while ((ch != ESCAPE) && !success)
	{
		bool move_fast = FALSE;

		move_cursor_relative(y, x);
		ch = inkey();
//		msg_format("(%c:", ch);//chk
		switch (ch)
		{
		case ESCAPE:
			break;
		case ' ':
		case 't':
		case '.':
		case '5':
		case '0':
			/* illegal place */
			if (player_bold(y, x)) ch = 0;

			/* okay place */
			else success = TRUE;

			break;

		/* XAngband: Move cursor to stairs */
		case '>':
		case '<':
			if (expand_list && temp_n)
			{
				int dx, dy;
				int cx = (panel_col_min + panel_col_max) / 2;
				int cy = (panel_row_min + panel_row_max) / 2;

				n++;

				while(n < temp_n)	/* Skip stairs which have defferent distance */
				{
					cave_type *c_ptr = &cave[temp_y[n]][temp_x[n]];

					if (ch == '>')
					{
						if (cave_have_flag_grid(c_ptr, FF_LESS) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_ENTER))
						{
							n++;
							//koko
						}
						else
							break;
					}
					else /* if (ch == '<') */
					{
						if (cave_have_flag_grid(c_ptr, FF_MORE) ||
							cave_have_flag_grid(c_ptr, FF_QUEST_EXIT))
							n++;
						else
							break;
					}
				}

				if (n == temp_n)	/* Loop out taget list */
				{
					n = 0;
					y = py;
					x = px;
					verify_panel();	/* Move cursor to player */

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();
				}
				else	/* move cursor to next stair and change panel */
				{
					y = temp_y[n];
					x = temp_x[n];

					dy = 2 * (y - cy) / hgt;
					dx = 2 * (x - cx) / wid;
					if (dy || dx) change_panel(dy, dx);
				}
			}
			break;

		default:
			/* Look up the direction */
			d = get_keymap_dir(ch);

			/* XTRA HACK MOVEFAST */
			if (isupper(ch)) move_fast = TRUE;
//	msg_format("%d)", d);//chk
			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* XTRA HACK MOVEFAST */
				if (move_fast)
				{
					int mag = MIN(wid / 2, hgt / 2);
					x += dx * mag;
					y += dy * mag;
				}
				else
				{
					x += dx;
					y += dy;
				}

				/* Do not move horizontally if unnecessary */
				if (((x < panel_col_min + wid / 2) && (dx > 0)) ||
					 ((x > panel_col_min + wid / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + hgt / 2) && (dy > 0)) ||
					 ((y > panel_row_min + hgt / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + hgt) || (y < panel_row_min) ||
					 (x >= panel_col_min + wid) || (x < panel_col_min))
				{
					/* if (change_panel(dy, dx)) target_set_prepare(mode); */
					change_panel(dy, dx);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;

			}
			break;
		}
	}

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	//v1.1.66 ��������̃O���b�h���J�E���g����temp_n,temp_x[],temp_y[]�����Z�b�g���Ă���
	temp_n = 0;


	*x_ptr = x;
	*y_ptr = y;
	return success;
}

/*:::���������肷��B�L�����Z���s�H*/
bool get_hack_dir(int *dp)
{
	int		dir;
	cptr    p;
	char    command;


	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = 0;

	/* (No auto-targeting) */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "���� ('*'�Ń^�[�Q�b�g�I��, ESC�Œ��f)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "���� ('5'�Ń^�[�Q�b�g��, '*'�Ń^�[�Q�b�g�đI��, ESC�Œ��f)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
		}  

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
			case 'T':
			case 't':
			case '.':
			case '5':
			case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
			case '*':
			case ' ':
			case '\r':
			{
				if (target_set(TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Look up the direction */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir) return (FALSE);

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[randint0(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���Ȃ��͍������Ă���B");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

	/* A "valid" direction was entered */
	return (TRUE);
}


/*
 * �G�l���M�[�̑�����10d5�𑬂��v�Z���邽�߂̊֐�
 //�g���ĂȂ��̂ŏ���

#define Go_no_JuuJou 5*5*5*5*5*5*5*5*5*5

s16b gain_energy(void)
{
	int i;
	s32b energy_result = 10;
	s32b tmp;

	tmp = randint0(Go_no_JuuJou);

	for (i = 0; i < 9; i ++){
		energy_result += tmp % 5;
		tmp /= 5;
	}

	return (s16b)(energy_result + tmp);
}
*/


/*
 * Return bow energy 
 */
/*:::�|�̊�{�ˌ����x���v�Z�@�Ȃ܂��|�̓\�[�X���x���ő���*/
///item sys sval�ɂ��ˌ����x�ݒ� tval���Q�Ƃ���悤�ɕς��Ȃ��Ƃ����Ȃ�
///mod131229 �|�̊�{�ˌ����x ������Œ�
s16b bow_energy(int tval, int sval)
{
	int energy = 100;
	
	if(tval == TV_BOW && sval == SV_FIRE_SHORT_BOW)		energy = 10000;
	else if(tval == TV_BOW && sval == SV_FIRE_LONG_BOW)	energy = 10000;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_LIGHT_CROSSBOW)	energy = 12000;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_HEAVY_CROSSBOW)	energy = 13333;

	else if(tval == TV_GUN)	energy = 10000;

	else msg_print("ERROR:�ݒ肳��Ă��Ȃ�tval��sval��bow_energy()���Ă΂ꂽ");

	return (energy);
}


/*
 * Return bow tmul
 */
/*:::�|�̊�{�{��*/
///item sys �|�̊�{�{���@�|�ƃN���X�{�E��TVAL�𕪂���̂ł����ύX����K�v����
///mod131229 �|�̊�{�{��
int bow_tmul(int tval, int sval)
{
	int tmul = 0;
	/* Analyze the launcher */

	if(tval == TV_BOW && sval == SV_FIRE_SHORT_BOW)		tmul = 2;
	else if(tval == TV_BOW && sval == SV_FIRE_LONG_BOW)	tmul = 3;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_LIGHT_CROSSBOW)	tmul = 3;
	else if(tval == TV_CROSSBOW && sval == SV_FIRE_HEAVY_CROSSBOW)	tmul = 4;
//	else if(tval == TV_GUN && sval == SV_FIRE_GUN_CRIMSON)	tmul = 0;
//	else if(tval == TV_GUN && sval == SV_FIRE_GUN_MUSKET)	tmul = 0;
	else msg_print("ERROR:�ݒ肳��Ă��Ȃ�tval��sval��bow_tmul()���Ă΂ꂽ");

	return (tmul);
}

/*
 * Return alignment title
 */
cptr your_alignment(void)
{
#ifdef JP
	if (p_ptr->align > 150) return "��P";
	else if (p_ptr->align > 50) return "���P";
	else if (p_ptr->align > 10) return "���P";
	else if (p_ptr->align > -11) return "����";
	else if (p_ptr->align > -51) return "����";
	else if (p_ptr->align > -151) return "����";
	else return "�别";
#else
	if (p_ptr->align > 150) return "Lawful";
	else if (p_ptr->align > 50) return "Good";
	else if (p_ptr->align > 10) return "Neutral Good";
	else if (p_ptr->align > -11) return "Neutral";
	else if (p_ptr->align > -51) return "Neutral Evil";
	else if (p_ptr->align > -151) return "Evil";
	else return "Chaotic";
#endif
}


/*
 * Return proficiency level of weapons and misc. skills (except riding)
 */
///skill ����n���x�֌W
int weapon_exp_level(int weapon_exp)
{
	if (weapon_exp < WEAPON_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (weapon_exp < WEAPON_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (weapon_exp < WEAPON_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (weapon_exp < WEAPON_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Return proficiency level of riding
 */
int riding_exp_level(int riding_exp)
{
	if (riding_exp < RIDING_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (riding_exp < RIDING_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (riding_exp < RIDING_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (riding_exp < RIDING_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Return proficiency level of spells
 */
/*:::���@�̋Z�\�l���n�����x���\�L�p�̒l�ɒ����Ă���炵��*/
int spell_exp_level(int spell_exp)
{
	if (spell_exp < SPELL_EXP_BEGINNER) return EXP_LEVEL_UNSKILLED;
	else if (spell_exp < SPELL_EXP_SKILLED) return EXP_LEVEL_BEGINNER;
	else if (spell_exp < SPELL_EXP_EXPERT) return EXP_LEVEL_SKILLED;
	else if (spell_exp < SPELL_EXP_MASTER) return EXP_LEVEL_EXPERT;
	else return EXP_LEVEL_MASTER;
}


/*
 * Display a rumor and apply its effects
 */

int rumor_num(char *zz, int max_idx)
{
	if (strcmp(zz, "*") == 0) return randint1(max_idx - 1);
	return atoi(zz);
}

cptr rumor_bind_name(char *base, cptr fullname)
{
	char *s, *v;

	s = strstr(base, "{Name}");
	if (s)
	{
		s[0] = '\0';
		v = format("%s%s%s", base, fullname, (s + 6));
	}
	else
	{
		v = base;
	}

	return v;
}

/*:::�\�̊����A�h�́u�\�𕷂��v�R�}���h*/
void display_rumor(bool ex)
{
	bool err;
	int section = 0;
	char Rumor[1024];

	///sys131216 �\�R�}���h�Œ��̏��𕷂��Ȃ������̂Ŋm����≺����
	if (ex)
	{
		//if (randint0(3) == 0) section = 1;
		if (randint0(7) == 0) section = 1;

	}

#ifdef JP
	err = get_rnd_line_jonly("rumors_j.txt", section, Rumor, 10);
	if (err) strcpy(Rumor, "�R�̉\������B");
#else
	err = get_rnd_line("rumors.txt", section, Rumor);
	if (err) strcpy(Rumor, "Some rumors are wrong.");
#endif

	err = TRUE;

	/*:::rumors_j.txt��N:1�̃G���g��(section 1)�ɑΉ��@���⒬�Ȃǂ̏��𓾂�*/
	///sys �Â��X�ɂ��Ă̋L�q�͍폜�\��
	if (strncmp(Rumor, "R:", 2) == 0)
	{
		char *zz[4];
		cptr rumor_msg = NULL;
		cptr rumor_eff_format = NULL;
		char fullname[1024] = "";

		if (tokenize(Rumor + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			if (strcmp(zz[0], "ARTIFACT") == 0)
			{
				int a_idx, k_idx;
				object_type forge;
				object_type *q_ptr = &forge;
				artifact_type *a_ptr;

				while (1)
				{
					a_idx = rumor_num(zz[1], max_a_idx);

					a_ptr = &a_info[a_idx];
					if (a_ptr->name) break;
				}

				k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
				object_prep(q_ptr, k_idx);
				q_ptr->name1 = a_idx;
				q_ptr->ident = IDENT_STORE;
				object_desc(fullname, q_ptr, OD_NAME_ONLY);
			}
			else if  (strcmp(zz[0], "MONSTER") == 0)
			{
				int r_idx;
				monster_race *r_ptr;

				while(1)
				{
					r_idx = rumor_num(zz[1], max_r_idx);
					r_ptr = &r_info[r_idx];
					if (r_ptr->name) break;
				}

				strcpy(fullname, r_name + r_ptr->name);

				/* Remember this monster */
				if (!r_ptr->r_sights)
				{
					r_ptr->r_sights++;
				}
			}
			///del131216 �_���W�����ƒ��̂��킳�𕷂��Ȃ�����
#if 0
			else if  (strcmp(zz[0], "DUNGEON") == 0)
			{
				int d_idx;
				dungeon_info_type *d_ptr;

				while (1)
				{
					d_idx = rumor_num(zz[1], max_d_idx);
					d_ptr = &d_info[d_idx];
					if (d_ptr->name) break;
				}

				strcpy(fullname, d_name + d_ptr->name);

				if (!max_dlv[d_idx])
				{
					max_dlv[d_idx] = d_ptr->mindepth;
					rumor_eff_format = _("%s�ɋA�҂ł���悤�ɂȂ����B", "You can recall to %s.");
				}
			}
			else if  (strcmp(zz[0], "TOWN") == 0)
			{
				int t_idx;
				s32b visit;

				while(1)
				{
					t_idx = rumor_num(zz[1], NO_TOWN);
					if (town[t_idx].name) break;
				}

				strcpy(fullname, town[t_idx].name);

				visit = (1L << (t_idx - 1));
				if ((t_idx != SECRET_TOWN) && !(p_ptr->visit & visit))
				{
					p_ptr->visit |= visit;
					rumor_eff_format = _("%s�ɍs�������Ƃ�����C������B", "You feel you have been to %s.");
				}
			}
#endif
			rumor_msg = rumor_bind_name(zz[2], fullname);
			msg_print(rumor_msg);
			if (rumor_eff_format)
			{
				msg_print(NULL);
				msg_format(rumor_eff_format, fullname);
			}
			err = FALSE;
		}
	/* error */
#ifdef JP
		if (err) msg_print("���̏��͊Ԉ���Ă���B");
#else
		if (err) msg_print("This information is wrong.");
#endif
	}
			else
	{
		msg_format("%s", Rumor);
	}
}



//�i�Ԃ̎�z�u
void	place_master_kaguya(void)
{
	int tx,ty;
	int dir;

	if(p_ptr->pclass != CLASS_EIRIN) return;
	if(p_ptr->wild_mode) return;
	if(p_ptr->inside_battle) return;
	if(p_ptr->inside_arena) return;
	
	//�i�Ԃׂ̗ɔz�u�������˂��ꍇ�A�t���A�̓K���ȂƂ���ɂ�����x�z�u�����݂�B����ł������Ȃ�o�Ȃ��܂܁B
	if(!summon_named_creature(0,py,px,MON_MASTER_KAGUYA,PM_FORCE_FRIENDLY))
	{
		int i;
		for(i=100;i>0;i--)
		{
			tx = randint1(cur_wid);
			ty = randint1(cur_hgt);

			if(cave_empty_bold(ty,tx))
			{
				summon_named_creature(0,ty,tx,MON_MASTER_KAGUYA,PM_FORCE_FRIENDLY);
				break;
			}
		}
		msg_print("�E�E�H�P��Ƃ͂���Ă��܂����悤���B");

	}

}

//v1.1.53 �N���E���s�[�X�͉΂�����Ɗy�����Ȃ�炵��
void clownpiece_likes_fire(int typ)
{
	if (p_ptr->pclass != CLASS_CLOWNPIECE && p_ptr->pclass != CLASS_VFS_CLOWNPIECE) return;

	if (typ != GF_FIRE && typ != GF_NUKE) return;

	if (p_ptr->hero || p_ptr->is_dead) return;

	if (one_in_(3))
		msg_print("���Ȃ��͉΂����ċC�������g�����I");
	else if (one_in_(2))
		msg_print("�u�R���Ă������[�I�v");
	else
		msg_print("�u���̉�...���b�N�����I�v");

	set_hero(5 + randint1(5), FALSE);

}

//v1.1.95 �������̃����X�^�[���猩���Ă��邩�`�F�b�N
//�����X�^�[���N���Ă��Ē��ڎ������ʂ��Ă��邩�ǂ���
//���Ă��郂���X�^�[��������m_idx���A���Ȃ����0��Ԃ�
int check_player_is_seen(void)
{
	int i;

	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if (!m_ptr->r_idx) continue;
		if (!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;
		if (r_info[m_ptr->r_idx].flags2 & RF2_EMPTY_MIND) continue;
		if (MON_CONFUSED(m_ptr)) continue;
		if (MON_CSLEEP(m_ptr)) continue;

		return i;
	}

	return 0;

}



