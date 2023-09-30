/* File: effects.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: effects of various "objects" */
/*:::�T���E�\���E�^�Ȃǂ̍s�����Z�b�g������O��*/
#include "angband.h"

void set_action(int typ)
{
	int prev_typ = p_ptr->action;

	if (typ == prev_typ)
	{
		return;
	}
	else
	{
		switch (prev_typ)
		{
			case ACTION_SEARCH:
			{
#ifdef JP
				msg_print("�T������߂��B");
#else
				msg_print("You no longer walk carefully.");
#endif
				p_ptr->redraw |= (PR_SPEED);
				break;
			}
			case ACTION_REST:
			{
				resting = 0;
				break;
			}
			case ACTION_LEARN:
			{
#ifdef JP
				msg_print("�w�K����߂��B");
#else
				msg_print("You stop Learning");
#endif
				new_mane = FALSE;
				break;
			}
			case ACTION_KAMAE:
			{
#ifdef JP
				msg_print("�\�����Ƃ����B");
#else
				msg_print("You stop assuming the posture.");
#endif
				p_ptr->special_defense &= ~(KAMAE_MASK);
				break;
			}
			case ACTION_KATA:
			{
#ifdef JP
				if(p_ptr->pclass == CLASS_ICHIRIN)
					msg_print("�\�����������B");
				else if (p_ptr->pclass == CLASS_YUYUKO)
					msg_print("���Ȃ��̓Z�����͋C�����i�ʂ�ɖ߂����B");
				else
					msg_print("�^��������B");
#else
				msg_print("You stop assuming the posture.");
#endif
				p_ptr->special_defense &= ~(KATA_MASK);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->redraw |= (PR_STATUS);
				break;
			}
			case ACTION_SING:
			{
#ifdef JP

				if(p_ptr->pclass == CLASS_KYOUKO)
					msg_print("���Ԃ̂���߂��B");
				else if (p_ptr->pclass == CLASS_SANNYO)
					msg_print("�������������B");
				else if(CLASS_IS_PRISM_SISTERS)
					msg_print("�����~�߂��B");
				else if(p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI || p_ptr->pclass == CLASS_RAIKO)
					msg_print("���t���~�߂��B");
				else
					msg_print("�̂��̂���߂��B");
#else
				msg_print("You stop singing.");
#endif
				break;
			}
			case ACTION_HAYAGAKE:
			{
#ifdef JP
				msg_print("�����d���Ȃ����B");
#else
				msg_print("You are no longer walking extremely fast.");
#endif
				energy_use = 100;
				break;
			}
			case ACTION_SPELL:
			{
#ifdef JP
				msg_print("�����̉r���𒆒f�����B");
#else
				msg_print("You stopped spelling all spells.");
#endif
				break;
			}

		}
	}

	p_ptr->action = typ;

	/* If we are requested other action, stop singing */
	if (prev_typ == ACTION_SING) 
	{
		stop_singing();
		stop_tsukumo_music();
		stop_raiko_music();
	}

	switch (p_ptr->action)
	{
		case ACTION_SEARCH:
		{
#ifdef JP
			msg_print("���Ӑ[�������n�߂��B");
#else
			msg_print("You begin to walk carefully.");
#endif
			p_ptr->redraw |= (PR_SPEED);
			break;
		}
		case ACTION_LEARN:
		{
#ifdef JP
			msg_print("�w�K���n�߂��B");
#else
			msg_print("You begin Learning");
#endif
			break;
		}
		case ACTION_FISH:
		{
#ifdef JP
			msg_print("���ʂɎ��𐂂炵���D�D�D");
#else
			msg_print("You begin fishing...");
#endif
			break;
		}
		case ACTION_HAYAGAKE:
		{
#ifdef JP
			msg_print("�����H�̂悤�Ɍy���Ȃ����B");
#else
			msg_print("You begin to walk extremely fast.");
#endif
			break;
		}
		default:
		{
			break;
		}
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);
	
}

/* reset timed flags */
/*:::�ꎞ�X�e�[�^�X�Ȃǂ�S�ĉ�������*/
///sys �ꎞ�X�e�[�^�X�����A�o�����O�Ή���d�ϐ��ȂǓ���Ȃ͕̂ێ�
void reset_tim_flags(void)
{
	int i;
	p_ptr->fast = 0;            /* Timed -- Fast */
	p_ptr->lightspeed = 0;
	p_ptr->slow = 0;            /* Timed -- Slow */
	p_ptr->blind = 0;           /* Timed -- Blindness */

	//hack - ���[�~�A�Ő������͖Ӗڕێ�
	if(p_ptr->pclass == CLASS_RUMIA && p_ptr->tim_general[0]) p_ptr->blind = 1;

	rokuro_head_search_item(0, TRUE);

	p_ptr->tim_hirari_nuno = 0;

	p_ptr->paralyzed = 0;       /* Timed -- Paralysis */
	p_ptr->confused = 0;        /* Timed -- Confusion */
	p_ptr->afraid = 0;          /* Timed -- Fear */
	p_ptr->image = 0;           /* Timed -- Hallucination */
	p_ptr->poisoned = 0;        /* Timed -- Poisoned */
	p_ptr->cut = 0;             /* Timed -- Cut */
	p_ptr->stun = 0;            /* Timed -- Stun */

	p_ptr->alcohol = 0;

	p_ptr->protevil = 0;        /* Timed -- Protection */
	p_ptr->invuln = 0;          /* Timed -- Invulnerable */
	p_ptr->ult_res = 0;
	p_ptr->hero = 0;            /* Timed -- Heroism */
	p_ptr->shero = 0;           /* Timed -- Super Heroism */
	p_ptr->shield = 0;          /* Timed -- Shield Spell */
	p_ptr->blessed = 0;         /* Timed -- Blessed */
	p_ptr->tim_invis = 0;       /* Timed -- Invisibility */
	p_ptr->tim_infra = 0;       /* Timed -- Infra Vision */
	p_ptr->tim_regen = 0;       /* Timed -- Regeneration */
	p_ptr->tim_stealth = 0;     /* Timed -- Stealth */
	p_ptr->tim_esp = 0;
	p_ptr->wraith_form = 0;     /* Timed -- Wraith Form */
	p_ptr->tim_levitation = 0;
	p_ptr->tim_sh_touki = 0;
	p_ptr->tim_sh_fire = 0;
	p_ptr->tim_sh_holy = 0;
	p_ptr->tim_eyeeye = 0;
	p_ptr->magicdef = 0;
	p_ptr->resist_magic = 0;
	p_ptr->tsuyoshi = 0;
	p_ptr->kabenuke = 0;
	p_ptr->tim_res_nether = 0;
	p_ptr->tim_res_water = 0;
	p_ptr->tim_res_chaos = 0;
	p_ptr->tim_superstealth = 0;
	p_ptr->radar_sense = 0;
	p_ptr->clawextend = 0;
	p_ptr->tim_speedster = 0;
	p_ptr->tim_res_insanity = 0;

	for(i=0;i<TIM_GENERAL_MAX;i++)	p_ptr->tim_general[i] = 0;
	for(i=0;i<6;i++)	p_ptr->tim_addstat_count[i] = 0;
	p_ptr->reactive_heal = 0;

	p_ptr->lucky = 0;
	p_ptr->foresight = 0;
	p_ptr->deportation = 0;

	p_ptr->tim_res_time = 0;
	p_ptr->tim_mimic = 0;
	p_ptr->mimic_form = 0;
	p_ptr->tim_reflect = 0;
	p_ptr->tim_sh_death = 0;
	p_ptr->multishadow = 0;
	p_ptr->dustrobe = 0;
	p_ptr->action = ACTION_NONE;
	p_ptr->nennbaku = 0;
	p_ptr->tim_res_dark = 0;
	p_ptr->tim_unite_darkness = 0;

	p_ptr->tim_spellcard_count = 0;
	p_ptr->spellcard_effect_idx = 0;

	p_ptr->oppose_acid = 0;     /* Timed -- oppose acid */
	p_ptr->oppose_elec = 0;     /* Timed -- oppose lightning */
	p_ptr->oppose_fire = 0;     /* Timed -- oppose heat */
	p_ptr->oppose_cold = 0;     /* Timed -- oppose cold */
	p_ptr->oppose_pois = 0;     /* Timed -- oppose poison */

	p_ptr->word_recall = 0;
	p_ptr->alter_reality = 0;
	p_ptr->sutemi = FALSE;
	p_ptr->counter = FALSE;
	p_ptr->ele_attack = 0;
	p_ptr->ele_immune = 0;
	p_ptr->special_attack = 0L;
	p_ptr->special_defense = 0L;
	//v1.1.13
	p_ptr->special_flags = 0L;

	p_ptr->tim_aggravation = 0L;

	p_ptr->tim_no_move = 0L;
	p_ptr->transportation_trap = 0L;

	while(p_ptr->energy_need < 0) p_ptr->energy_need += ENERGY_NEED();
	world_player = FALSE;
	///race clalss �ꎞ���ʉ����ł��ꕔ�̐E��푰�͈ꕔ���ʂ��p������
	//if (prace_is_(RACE_DEMON) && (p_ptr->lev > 44)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_NINJA) && (p_ptr->lev > 44)) p_ptr->oppose_pois = 1;

	if ((p_ptr->pclass == CLASS_IKU ||p_ptr->pclass == CLASS_TOZIKO ||p_ptr->pclass == CLASS_RAIKO) && (p_ptr->lev > 29)) p_ptr->oppose_elec = 1;
	if ((p_ptr->pclass == CLASS_LETTY) && (p_ptr->lev > 19)) p_ptr->oppose_cold = 1;
	if ((p_ptr->pclass == CLASS_NARUMI) && (p_ptr->lev > 29)) p_ptr->oppose_cold = 1;
	if ((p_ptr->pclass == CLASS_ORIN || p_ptr->pclass == CLASS_FLAN) && (p_ptr->lev > 34)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_MOKOU) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_FUTO) && (p_ptr->lev > 39)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_MAYUMI) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;

	if ((p_ptr->pclass == CLASS_KEIKI) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;
	if ((p_ptr->pclass == CLASS_MOMOYO) && (p_ptr->lev > 19)) p_ptr->oppose_pois = 1;

	if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && (p_ptr->lev > 29)) p_ptr->oppose_fire = 1;

	if ((p_ptr->pclass == CLASS_YUMA) && (p_ptr->lev > 19)) p_ptr->oppose_pois = 1;

	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) p_ptr->shero = 1;

	if (p_ptr->riding)
	{
		(void)set_monster_fast(p_ptr->riding, 0);
		(void)set_monster_slow(p_ptr->riding, 0);
		(void)set_monster_invulner(p_ptr->riding, 0, FALSE);
	}

//	if (p_ptr->pclass == CLASS_BARD)
	///mod140817 �̊֌W�ɋ�V���l�ȊO�̐E���֏�
	if(CHECK_MUSIC_CLASS)
	{
		p_ptr->magic_num1[0] = 0;
		p_ptr->magic_num2[0] = 0;
	}
}

/*:::���͏������󂯂��Ƃ�*/
///system �������@�������ʂ�ǉ������炱���ɉ����p�����Ȃ��Ƃ����Ȃ�
void dispel_player(void)
{
	int i;
	(void)set_fast(0, TRUE);
	(void)set_lightspeed(0, TRUE);
	(void)set_slow(0, TRUE);
	(void)set_shield(0, TRUE);
	(void)set_blessed(0, TRUE);
	(void)set_tsuyoshi(0, TRUE);
	(void)set_hero(0, TRUE);
	(void)set_shero(0, TRUE);
	(void)set_protevil(0, TRUE);
	(void)set_invuln(0, TRUE);
	(void)set_wraith_form(0, TRUE);
	(void)set_kabenuke(0, TRUE);
	(void)set_tim_res_nether(0, TRUE);
	(void)set_tim_res_time(0, TRUE);
	/* by henkma */
	(void)set_tim_reflect(0,TRUE);
	(void)set_multishadow(0,TRUE);
	(void)set_dustrobe(0,TRUE);
	for(i=0;i<6;i++)(void)set_tim_addstat(i,0,0,TRUE);

	(void)set_reactive_heal(0, TRUE);
	(void)set_clawextend(0, TRUE);

	(void)set_tim_invis(0, TRUE);
	(void)set_tim_infra(0, TRUE);
	(void)set_tim_esp(0, TRUE);
	(void)set_tim_regen(0, TRUE);
	(void)set_tim_stealth(0, TRUE);
	(void)set_tim_levitation(0, TRUE);
	(void)set_tim_sh_touki(0, TRUE);
	(void)set_tim_sh_fire(0, TRUE);
	(void)set_tim_sh_holy(0, TRUE);
	(void)set_tim_eyeeye(0, TRUE);
	(void)set_magicdef(0, TRUE);
	(void)set_resist_magic(0, TRUE);
	(void)set_oppose_acid(0, TRUE);
	(void)set_oppose_elec(0, TRUE);
	(void)set_oppose_fire(0, TRUE);
	(void)set_oppose_cold(0, TRUE);
	(void)set_oppose_pois(0, TRUE);
	(void)set_ultimate_res(0, TRUE);
	(void)set_mimic(0, 0, TRUE);
	(void)set_ele_attack(0, 0);
	(void)set_ele_immune(0, 0);
	(void)set_kamioroshi(0, 0);

	///mod140209 ���ꎞ�ϐ�
	(void)set_tim_speedster(0, TRUE);
	(void)set_tim_res_insanity(0, TRUE);
	(void)set_tim_res_water(0, TRUE);
	(void)set_tim_res_chaos(0, TRUE);
	(void)set_tim_superstealth(0, TRUE,0);
	(void)set_tim_sh_death(0, TRUE);
	(void)set_radar_sense(0, TRUE);
	(void)set_tim_res_dark(0, TRUE);
	(void)set_tim_unite_darkness(0, TRUE);

	(void)set_tim_lucky(0, TRUE);
	(void)set_foresight(0, TRUE);
	(void)set_deportation(0, TRUE);

	//���@�̐g�̋����͖��͏������󂯂�
	if(p_ptr->pclass == CLASS_BYAKUREN) set_tim_general(0,TRUE,0,0);

	//�w�J�[�e�B�A�̃A�|�����ˋ���
	if(p_ptr->pclass == CLASS_HECATIA) set_tim_general(0,TRUE,0,0);

	//�T�O���_�X�̒e����
	if(p_ptr->pclass == CLASS_SAGUME) set_tim_general(0,TRUE,0,0);

	//v1.1.27 ���E�K�[�h
	set_mana_shield(FALSE,FALSE);

	//v1.1.56
	set_tim_spellcard_effect(0, TRUE, 0);

	//v1.1.69
	set_nennbaku(0, TRUE);
	//v1.1.93
	(void)set_tim_aggravation(0, TRUE);

	set_tim_transportation_trap(0, TRUE);

	//v1.1.17 ���ψꎞ���ʂ�
	//v1.1.52 俎q�V���i�ǉ�
	if(USE_NAMELESS_ARTS)
	{
		for(i=0;i<TIM_GENERAL_MAX;i++) set_tim_general(0,TRUE,i,0);
	}

	//���̒c�q�����ƃC���t���[�G���X��
	if(p_ptr->pclass == CLASS_RINGO)
	{
		set_tim_general(0,TRUE,0,0);
		set_tim_general(0,TRUE,1,0);
	}
	//�������Z�u�N���~�i���T�����F�C�V�����v��
	if(p_ptr->special_defense & (SD_HELLFIRE_BARRIER))
	{
		msg_print("���Ȃ��͂����n���̋Ɖ΂���ی삳��Ă��Ȃ��B");
		p_ptr->special_defense &= ~(SD_HELLFIRE_BARRIER);
	}

	//v1.1.94 �Ɏq�̏�
	if (p_ptr->special_defense & SD_GLASS_SHIELD)
	{
		msg_print("�Ɏq�̏����������B");
		p_ptr->special_defense &= ~(SD_GLASS_SHIELD);

	}

	//v2.0.1 ���������̖�
	if (p_ptr->special_defense & SD_LIFE_EXPLODE)
	{
		msg_print("���������̌��ʂ��������B");
		p_ptr->special_defense &= ~(SD_LIFE_EXPLODE);

	}


	//���ǂ񂰁u�C�r���A���W�����[�V�����v����
	if (p_ptr->special_defense & EVIL_UNDULATION_MASK)
	{
		evil_undulation(FALSE, FALSE);
	}


	/* Cancel glowing hands */
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
		p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
		msg_print("��̋P�����Ȃ��Ȃ����B");
#else
		msg_print("Your hands stop glowing.");
#endif
	}

	if (music_singing_any() || hex_spelling_any())
	{
		cptr str;
		if (music_singing_any())
		{
			if (p_ptr->pclass == CLASS_SANNYO)
				msg_print("�����̖����������B");
			else
				msg_print("�̂��r�؂ꂽ�B");
		}
		else
		{
			msg_print("�r�����r�؂ꂽ�B");
		}

		p_ptr->magic_num1[1] = p_ptr->magic_num1[0];
		p_ptr->magic_num1[0] = 0;
		p_ptr->action = ACTION_NONE;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS | PU_HP);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP | PR_STATUS | PR_STATE);

		/* Update monsters */
		p_ptr->update |= (PU_MONSTERS);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		p_ptr->energy_need += ENERGY_NEED();
	}
}


/*
 * Set "p_ptr->tim_mimic", and "p_ptr->mimic_form",
 * notice observable changes
 */
/*:::�ϐg �ϐg�s�̂Ƃ�FALSE*/
bool set_mimic(int v, int p, bool do_dec)
{
	bool notice = FALSE;
	bool check_nue = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	//v1.1.85 �V���ɕϐg����Ƃ��A�����X�^�[�ϐg�p��idx���L�^���Ă������̂����Z�b�g
	if (!do_dec) p_ptr->metamor_r_idx = 0;


	if(p_ptr->riding && mimic_info[p].MIMIC_FLAGS & MIMIC_NO_RIDING && !do_dec)
	{
		if (CLASS_RIDING_BACKDANCE && p_ptr->pclass == CLASS_TSUKASA)
			msg_print("�񐶒��͕ϐg�ł��Ȃ��B");
		else if (CLASS_RIDING_BACKDANCE)
			msg_print("���͗x��̂ɖZ�����ϐg�ł��Ȃ��B");
		else
			msg_print("�܂��n����~��Ă���łȂ��Ƃ��̕ϐg�͂ł��Ȃ��B");
		return FALSE;
	}
	if(p_ptr->prace == RACE_HOURAI && !do_dec)
	{
		msg_print("�E�E���Ȃ��̓��͕̂ϗe���󂯕t���Ȃ������B");
		return FALSE;
	}

	// -Hack - �ʂ��͓���t���O�������Ă������ϐg�������Ȃ�
	if(p_ptr->mimic_form == MIMIC_NUE && (p_ptr->special_defense & NUE_UNDEFINED) && !v)
	{
		p_ptr->tim_mimic = 1;
		return FALSE;
	}
	//��Ɠ������h���~�[�ϐg���t���O�������Ă��������Ȃ�
	//v1.1.47 �}�~�]�E�ϐg�Ƌ�ʂ��邽�ߏ����������t���O�ɕύX�@���������������ŕϐg����
	if(IS_METAMORPHOSIS && (p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME) && !v)
	{
		p_ptr->tim_mimic = 1;
		return FALSE;
	}
	//�}�~�]�E�̕ω��͉�����B������O�Ƀt���O�𗎂Ƃ�
	else if(IS_METAMORPHOSIS && !v)
	{
		p_ptr->special_defense &= ~(SD_METAMORPHOSIS);
	}

	/* Open */
	if (v)
	{
		if (p_ptr->tim_mimic && (p_ptr->mimic_form == p) && !do_dec)
		{
			if (p_ptr->tim_mimic > v) return FALSE;
		}
		else if ((!p_ptr->tim_mimic) || (p_ptr->mimic_form != p))
		{
			set_deity_bias(DBIAS_COSMOS, -1);
#ifdef JP

			if( p == MIMIC_GIGANTIC)
				msg_print("���Ȃ��͎R�̂悤�ȋ��̂ɂȂ����I");
			else if( p == MIMIC_BEAST)
				msg_print("���Ȃ��͋���ȏb�ɕϐg�����I");
			else if( p == MIMIC_SLIME)
				msg_print("���Ȃ��̑̂͗n������čL�������I");
			else if( p == MIMIC_MARISA)
				msg_print("���Ȃ��͔����̖��@�g���ɕϐg�����I");
			else if( p == MIMIC_MIST)
				msg_print("���Ȃ��̗̑̂֊s���ڂ₯�n�߂��E�E");
			else if( p == MIMIC_DRAGON)
				msg_print("���Ȃ��͋���ȗ��ւƕϐg�����I");
			else if( p == MIMIC_CAT)
				msg_print("���Ȃ��͔L�̎p�ɂȂ����B");
			else if( p == MIMIC_NUE)
				msg_print("���Ȃ��͐��̕s���̉����ɕϐg�����I");
			else if( p == MIMIC_SHINMYOU)
				msg_print("�`���̈ꐡ�@�t�̂悤�ɑ̂��������Ȃ����I");
			else if( p == MIMIC_KOSUZU_GHOST)
				msg_print("������ǂ�ł���ƈӎ������̂��Ă�����...");
			else if( p == MIMIC_KOSUZU_HYAKKI)
				msg_print("�n�ʂ��獕���e�������o���A���Ȃ��Ɏ��߂����I");
			else if (p == MIMIC_GOD_OF_NEW_WORLD)
			{
				if(one_in_(2))
					msg_print("���Ȃ��͋��M�ɐg��C���ċ���삯���I");
				else
					msg_print("���Ȃ�V���E�̐_�ɂł��Ȃꂻ���ȋC�����I");
			}
			//v1.1.47 �����@
			else if (p >= MIMIC_METAMORPHOSE_NORMAL && p <= MIMIC_METAMORPHOSE_GIGANTIC &&
				(p_ptr->special_flags & SPF_IGNORE_METAMOR_TIME) && p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͊��S�߈˂����������哱����D�����I");
			}
			else
				msg_print("�����̑̂��ς���Ă䂭�̂��������B");
#else
			msg_print("You feel that your body changes.");
#endif
			p_ptr->mimic_form=p;
			notice = TRUE;

			// Hack - special_defence�t���O���g������ȕϐg������E���ϐg���ɕ��ʂ̕ϐg�������ꍇ�t���O����
			if(p_ptr->pclass == CLASS_NUE && p_ptr->mimic_form != MIMIC_NUE) p_ptr->special_defense &= ~(NUE_UNDEFINED);
			if(IS_METAMORPHOSIS && (p_ptr->mimic_form < MIMIC_METAMORPHOSE_NORMAL || p_ptr->mimic_form > MIMIC_METAMORPHOSE_GIGANTIC))p_ptr->special_defense &= ~(SD_METAMORPHOSIS);

			//��\��o�����ꕔ�ϐg�������ꍇ���t����
			if(p_ptr->pclass == CLASS_BENBEN && p_ptr->pclass == CLASS_YATSUHASHI)
			{
				if( mimic_info[p].MIMIC_FLAGS & (MIMIC_ONLY_MELEE | MIMIC_NO_CAST)) stop_tsukumo_music();
			}


			//v1.1.68b �X���C���▶�ɕω�������؂菝��j�������邱�Ƃɂ���
			if (p == MIMIC_MIST || p == MIMIC_SLIME)
			{
				if (RACE_BREAKABLE)
					set_broken(-(BROKEN_MAX));
				else
					set_cut(0);
			}

		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_mimic)
		{
#ifdef JP
			if (p_ptr->mimic_form == MIMIC_GIGANTIC)
				msg_print("�̂̑傫�������ɖ߂����B");
			else if (p_ptr->mimic_form == MIMIC_MIST)
				msg_print("���Ȃ��̗̑̂֊s�͍Ăт͂����肵���B");
			else if (p_ptr->mimic_form == MIMIC_CAT)
				msg_print("�u���Ⴖ��[��I�v");
			else if( p_ptr->mimic_form == MIMIC_NUE)
				msg_print("���Ȃ��̎p�͌��ɖ߂����B");
			else if( p == MIMIC_KOSUZU_GHOST)
			{
				msg_print("���Ȃ��͐��C�ɖ߂����B");
				//�ǔ�����ς��g��Ԃ������炻���������
				set_kabenuke(0,TRUE);
				p_ptr->special_defense &= ~(NINJA_KAWARIMI);

			}
			else if( p == MIMIC_KOSUZU_HYAKKI)
				msg_print("�S�̉e�͂��Ȃ����痣�ꂽ�B");
			//v1.1.48 �����@���̏��������ƃG�N�X�g�����[�h�ŒK�̗t���ςŕϐg�����Ƃ������̃��b�Z�[�W�ɂȂ��Ă��܂����d���Ȃ���
			else if (p_ptr->mimic_form >= MIMIC_METAMORPHOSE_NORMAL && p_ptr->mimic_form <= MIMIC_METAMORPHOSE_GIGANTIC && p_ptr->pclass == CLASS_SHION)
			{
				msg_print("���Ȃ��͜߈˂��������B");
			}
			else
				msg_print("�ϐg���������B");
#else
			msg_print("You are no longer transformed.");
#endif
			if(p_ptr->pclass == CLASS_NUE && p_ptr->mimic_form != MIMIC_NUE) check_nue = TRUE;
			if (p_ptr->mimic_form == MIMIC_DEMON) set_oppose_fire(0, TRUE);
			p_ptr->mimic_form=0;
			notice = TRUE;
			p = 0;
		}
	}


	/* Use the value */
	p_ptr->tim_mimic = v;

	/* Nothing to notice */
	if (!notice)
		return (FALSE);

	//v1.1.58
	flag_update_floor_music = TRUE;

	/* Disturb */
	if (disturb_state) disturb(0, 1);

	/* Redraw title */
	p_ptr->redraw |= (PR_BASIC | PR_STATUS | PR_MAP);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_TORCH);
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	p_ptr->window |= (PW_EQUIP | PW_SPELL | PW_PLAYER | PW_INVEN);

	handle_stuff();

	/*::: -Hack- �ʂ���[���̕s��]�ȊO����̕ϐg�����̂Ƃ��A[���̕s��]�ɕϐg�\���`�F�b�N���ĉ\�Ȃ�ϐg����
	 *::: check_nue_undefined()����Ă�set_mimic()���Ă΂��*/
	//v2.0.5 �����̏�������߂�B�ʂ��̎����ϐg�͍s���J�n���ɂ̂݊m�F����
	if(check_nue) flag_nue_check_undefined = TRUE;
	//if(check_nue) check_nue_undefined();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->blind", notice observable changes
 *
 * Note the use of "PU_UN_LITE" and "PU_UN_VIEW", which is needed to
 * memorize any terrain features which suddenly become "visible".
 * Note that blindness is currently the only thing which can affect
 * "player_can_see_bold()".
 */
bool set_blind(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->cut = 0;
		return FALSE;
	}

	if(p_ptr->pclass == CLASS_RUMIA && p_ptr->tim_general[0]) v = 1;

	/* Open */
	if (v)
	{
		if (!p_ptr->blind)
		{
			///msg131221 �Ӗڎ��A���h���C�h�ł����ʂ̃��b�Z�[�W
			msg_print("�ڂ������Ȃ��Ȃ��Ă��܂����I");
#if 0
			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
msg_print("�Z���T�[�����ꂽ�I");
#else
				msg_print("You are blind!");
#endif
			}
			else
			{
#ifdef JP
msg_print("�ڂ������Ȃ��Ȃ��Ă��܂����I");
#else
				msg_print("You are blind!");
#endif
			}
#endif
			notice = TRUE;
			///del131221 virtue
			//chg_virtue(V_ENLIGHTEN, -1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->blind)
		{
			///msg131221 �Ӗڎ��A���h���C�h�ł����ʂ̃��b�Z�[�W
			msg_print("����Ɩڂ�������悤�ɂȂ����B");
#if 0
			if (p_ptr->prace == RACE_ANDROID)
			{
#ifdef JP
msg_print("�Z���T�[�����������B");
#else
				msg_print("You can see again.");
#endif
			}
			else
			{
#ifdef JP
msg_print("����Ɩڂ�������悤�ɂȂ����B");
#else
				msg_print("You can see again.");
#endif
			}
#endif
			notice = TRUE;

		}
	}

	/* Use the value */
	p_ptr->blind = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Fully update the visuals */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE | PU_VIEW | PU_LITE | PU_MONSTERS | PU_MON_LITE);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->confused", notice observable changes
 */
bool set_confused(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!p_ptr->confused)
		{
#ifdef JP
msg_print("���Ȃ��͍��������I");
#else
			msg_print("You are confused!");
#endif

			if (p_ptr->action == ACTION_LEARN)
			{
#ifdef JP
				msg_print("�w�K���������Ȃ��I");
#else
				msg_print("You cannot continue Learning!");
#endif
				new_mane = FALSE;

				p_ptr->redraw |= (PR_STATE);
				p_ptr->action = ACTION_NONE;
			}
			if (p_ptr->action == ACTION_KAMAE)
			{
#ifdef JP
				msg_print("�\�����Ƃ����B");
#else
				msg_print("Your posture gets loose.");
#endif
				p_ptr->special_defense &= ~(KAMAE_MASK);
				p_ptr->update |= (PU_BONUS);
				p_ptr->redraw |= (PR_STATE);
				p_ptr->action = ACTION_NONE;
			}
			else if (p_ptr->action == ACTION_KATA)
			{
#ifdef JP
				if(p_ptr->pclass == CLASS_YUYUKO)
					msg_print("���Ȃ��͋Z�𒆒f�����B");
				else

				msg_print("�^�����ꂽ�B");
#else
				msg_print("Your posture gets loose.");
#endif
				p_ptr->special_defense &= ~(KATA_MASK);
				p_ptr->update |= (PU_BONUS);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->redraw |= (PR_STATE);
				p_ptr->redraw |= (PR_STATUS);
				p_ptr->action = ACTION_NONE;
			}

			//v1.1.27 ���E�K�[�h
			set_mana_shield(FALSE,FALSE);

			/* Sniper */
			//if (p_ptr->concent) reset_concentration(TRUE);
			if (CLASS_USE_CONCENT) reset_concentration(TRUE);

			/* Hex */
			if (hex_spelling_any()) stop_hex_spell_all();

			notice = TRUE;
			p_ptr->counter = FALSE;
			chg_virtue(V_HARMONY, -1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->confused)
		{
#ifdef JP
msg_print("����ƍ����������܂����B");
#else
			msg_print("You feel less confused now.");
#endif

			//p_ptr->special_attack &= ~(ATTACK_SUIKEN);
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->confused = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->poisoned", notice observable changes
 */
/*:::�łɖ`���ꂽ�莡�鏈��*/
bool set_poisoned(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!p_ptr->poisoned)
		{
#ifdef JP
msg_print("�łɐN����Ă��܂����I");
#else
			msg_print("You are poisoned!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->poisoned)
		{
#ifdef JP
msg_print("����Ɠł̒ɂ݂��Ȃ��Ȃ����B");
#else
			msg_print("You are no longer poisoned.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->poisoned = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->afraid", notice observable changes
 */
bool set_afraid(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (!p_ptr->afraid)
		{
#ifdef JP
msg_print("�������������Ȃ��Ă����I");
#else
			msg_print("You are terrified!");
#endif

			if (p_ptr->special_defense & KATA_MASK)
			{
#ifdef JP
				msg_print("�^�����ꂽ�B");
#else
				msg_print("Your posture gets loose.");
#endif
				p_ptr->special_defense &= ~(KATA_MASK);
				p_ptr->update |= (PU_BONUS);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->redraw |= (PR_STATE);
				p_ptr->redraw |= (PR_STATUS);
				p_ptr->action = ACTION_NONE;
			}

			notice = TRUE;
			p_ptr->counter = FALSE;
			chg_virtue(V_VALOUR, -1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->afraid)
		{
#ifdef JP
msg_print("����Ƌ��|��U�蕥�����B");
#else
			msg_print("You feel bolder now.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->afraid = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->paralyzed", notice observable changes
 */
bool set_paralyzed(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->paralyzed = 0;
		return FALSE;
	}

	/* Open */
	if (v)
	{
		if (!p_ptr->paralyzed)
		{
#ifdef JP
			///msg131221
msg_print("�����Ȃ��I");
#else
			msg_print("You are paralyzed!");
#endif

			/* Sniper */
			//if (p_ptr->concent) reset_concentration(TRUE);
			if(CLASS_USE_CONCENT) reset_concentration(TRUE);

			//v1.1.27 ���E�K�[�h
			set_mana_shield(FALSE,FALSE);

			/* Hex */
			if (hex_spelling_any()) stop_hex_spell_all();

			p_ptr->counter = FALSE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->paralyzed)
		{
#ifdef JP
msg_print("����Ɠ�����悤�ɂȂ����B");
#else
			msg_print("You can move again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->paralyzed = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->image", notice observable changes
 *
 * Note that we must redraw the map when hallucination changes.
 */
bool set_image(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;


	/* Open */
	if (v)
	{
		set_tsuyoshi(0, TRUE);
		if (!p_ptr->image)
		{
#ifdef JP
msg_print("���[�I�I�����������F�Ɍ�����I");
#else
			msg_print("Oh, wow! Everything looks so cosmic now!");
#endif

			/* Sniper */
			//if (p_ptr->concent) reset_concentration(TRUE);
			if(CLASS_USE_CONCENT ) reset_concentration(TRUE);
			p_ptr->counter = FALSE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->image)
		{
#ifdef JP
msg_print("����Ƃ͂�����ƕ���������悤�ɂȂ����B");
#else
			msg_print("You can see clearly again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->image = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 1);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	if(CHECK_FAIRY_TYPE == 46) p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->fast", notice observable changes
 */
bool set_fast(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->fast && !do_dec)
		{
			if (p_ptr->fast > v) return FALSE;
		}
		else if (!IS_FAST() && !p_ptr->lightspeed)
		{
#ifdef JP
msg_print("�f����������悤�ɂȂ����I");
#else
			msg_print("You feel yourself moving much faster!");
#endif

			notice = TRUE;
			///del131216 virtue
			//chg_virtue(V_PATIENCE, -1);
			//chg_virtue(V_DILIGENCE, 1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->fast && !p_ptr->lightspeed && !music_singing(MUSIC_SPEED) && !music_singing(MUSIC_SHERO))
		{
#ifdef JP
msg_print("�����̑f�������Ȃ��Ȃ����悤���B");
#else
			msg_print("You feel yourself slow down.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->fast = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->lightspeed", notice observable changes
 */
/*:::�B�C�p�t�̍����ړ��Bv�͎c�莞�Ԃ��H*/
/*:::��Ԃɉ����ω��������TRUE,�Ȃ����FALSE��Ԃ��炵��*/
bool set_lightspeed(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if (p_ptr->wild_mode) v = 0;

	/* Open */
	if (v)
	{
		if (p_ptr->lightspeed && !do_dec)
		{
			if (p_ptr->lightspeed > v) return FALSE;
		}
		else if (!p_ptr->lightspeed)
		{
#ifdef JP
msg_print("���ɑf����������悤�ɂȂ����I");
#else
			msg_print("You feel yourself moving extremely faster!");
#endif

			notice = TRUE;
			chg_virtue(V_PATIENCE, -1);
			chg_virtue(V_DILIGENCE, 1);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->lightspeed)
		{
#ifdef JP
msg_print("�����̑f�������Ȃ��Ȃ����悤���B");
#else
			msg_print("You feel yourself slow down.");
#endif
			notice = TRUE;


		}
	}

	/* Use the value */
	p_ptr->lightspeed = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	//v1.1.58
	flag_update_floor_music = TRUE;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->slow", notice observable changes
 */
bool set_slow(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_RES_INER) v = 0;

	//v1.1.71 ���S�͌������󂯂Ȃ�
	if (p_ptr->pclass == CLASS_SAKI) v = 0;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->slow = 0;
		return FALSE;
	}

	/* Open */
	if (v)
	{
		if (p_ptr->slow && !do_dec)
		{
			if (p_ptr->slow > v) return FALSE;
		}
		else if (!p_ptr->slow)
		{
#ifdef JP
msg_print("�̂̓������x���Ȃ��Ă��܂����I");
#else
			msg_print("You feel yourself moving slower!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->slow)
		{
#ifdef JP
msg_print("�����̒x�����Ȃ��Ȃ����悤���B");
#else
			msg_print("You feel yourself speed up.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->slow = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->shield", notice observable changes
 */
/*:::�Δ��@��������ʂ�����*/
/*:::v:�^�[���� 
 *:::do_dec:v��p_ptr->shield�l���Ⴂ�ꍇ�ɂ�v��K�p����t���O �^�C���A�E�g�Ȃǂ̏����@
 */
bool set_shield(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		/*:::do_dec��FALSE�̂Ƃ��́Av�̂ق����������ꍇ�������Ȃ�*/
		if (p_ptr->shield && !do_dec)
		{
			if (p_ptr->shield > v) return FALSE;
		}
		else if (!p_ptr->shield)
		{
#ifdef JP
			///msg131221
msg_print("�h��͂��オ�����B");
#else
			msg_print("Your skin turns to stone.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->shield)
		{
#ifdef JP
msg_print("�h��̖��@���������B");
#else
			msg_print("Your skin returns to normal.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->shield = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::�j�ח̈�̐_�~�낵 �r�������̂��߃��[�`���܂Ƃ߂�*/
//�J�E���g��p_ptr->kamioroshi_count�ōs��
bool set_kamioroshi(u32b kamioroshi_type, int v)
{
	bool flag_master = FALSE;

	//�_�~�낵���Q�����ɍ~�낹��t���O
	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_PUNISH && p_ptr->lev > 39
		|| p_ptr->pclass == CLASS_REIMU && osaisen_rank() == 9	
		|| p_ptr->pclass == CLASS_YORIHIME)
		flag_master = TRUE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	//v1.1.14 �V�F�󔄖��̐_�~�낵���ɓV�Ƒ��_���ĂԂƓV�F�󔄖����L�����Z�������悤�ɂ���
	if((kamioroshi_type & KAMIOROSHI_AMATERASU) && (p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME))
	{
		p_ptr->kamioroshi &= ~(KAMIOROSHI_AMENOUZUME);
		msg_print("�V�F�󔄖��͖�ڂ��ʂ����ď������B");
	}

	//v1.1.14 �˕P���_���l�̐_�~�낵�������Ƃ�concent����
	//reset_concertration()�ɂ���Ɋւ����O�����ǉ�
	if(p_ptr->pclass == CLASS_YORIHIME && ((kamioroshi_type | p_ptr->kamioroshi)& KAMIOROSHI_GION))
	{
		reset_concent = FALSE;
	}


	///mod141019 �j�׃n�C�̐_�~�낵�����@�Q�����ɍ~�낹��悤�ɂ���
	///mod150201 �ő僉���N�̗얲�������ɂ���
	if(flag_master && kamioroshi_type)
	{
		int count = 0;
		int i;

		for(i=0;i<32;i++) if((p_ptr->kamioroshi >> i)&1L) count++;
		//msg_format("count:%d",count);

		if(count>1) for(i=0;i<32;i++) 
		{
			if((p_ptr->kamioroshi >> i)%2 == 0 || kamioroshi_type == (1L << i)) continue;

			switch(1L << i)
			{
			case KAMIOROSHI_IZUNA:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_IZUNA);
				msg_print("�эj�����̗͂��������B");
				break;

			case KAMIOROSHI_ISHIKORIDOME:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_ISHIKORIDOME);
				msg_print("�΋ÉW���̗͂��������B");
				break;

			case KAMIOROSHI_SUMIYOSHI:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_SUMIYOSHI);
				msg_print("�Z�g�O�_�̗͂��������B");
				break;

			case KAMIOROSHI_ATAGO:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_ATAGO);
				msg_print("�����l�̗͂��������B");
				break;

			case KAMIOROSHI_AMENOUZUME:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_AMENOUZUME);
				msg_print("�V�F�󔄖��̗͂��������B");
				break;

			case KAMIOROSHI_GION:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_GION);
				if(p_ptr->paralyzed || (p_ptr->stun >= 100) || p_ptr->alcohol >= DRANK_4 && !(p_ptr->muta2 & MUT2_ALCOHOL))
					msg_print("�_���l�̌����n�ʂ��甲�����B");
				else
					msg_print("�_���l�̌���n�ʂ�������������B");

				break;

			case KAMIOROSHI_KANAYAMAHIKO:
				p_ptr->kamioroshi &= ~(KAMIOROSHI_KANAYAMAHIKO);
				msg_print("���R�F���̗͂��������B");
				break;


			}
			break; //������͍̂ŏ��̈����

		}
	}
	else
	{

		if ((p_ptr->kamioroshi & (KAMIOROSHI_IZUNA)) && (kamioroshi_type != KAMIOROSHI_IZUNA))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_IZUNA);
			msg_print("�эj�����̗͂��������B");
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_ISHIKORIDOME)) && (kamioroshi_type != KAMIOROSHI_ISHIKORIDOME))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_ISHIKORIDOME);
			msg_print("�΋ÉW���̗͂��������B");
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_SUMIYOSHI)) && (kamioroshi_type != KAMIOROSHI_SUMIYOSHI))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_SUMIYOSHI);
			msg_print("�Z�g�O�_�̗͂��������B");
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_ATAGO)) && (kamioroshi_type != KAMIOROSHI_ATAGO))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_ATAGO);
			msg_print("�����l�̗͂��������B");
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_AMENOUZUME)) && (kamioroshi_type != KAMIOROSHI_AMENOUZUME))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_AMENOUZUME);
			msg_print("�V�F�󔄖��̗͂��������B");
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_GION)) && (kamioroshi_type != KAMIOROSHI_GION))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_GION);
			msg_print("�_���l�̌���n�ʂ�������������B");
		}
		if ((p_ptr->kamioroshi & (KAMIOROSHI_KANAYAMAHIKO)) && (kamioroshi_type != KAMIOROSHI_KANAYAMAHIKO))
		{
			p_ptr->kamioroshi &= ~(KAMIOROSHI_KANAYAMAHIKO);
			msg_print("���R�F���̗͂��������B");
		}

	}

	//��u�����������ď풓���Ȃ��^�C�v�̐_�~�낵�̓t���O�l��0�ɂ���
	if(kamioroshi_type == KAMIOROSHI_AMATO || kamioroshi_type == KAMIOROSHI_IZUNOME || kamioroshi_type == KAMIOROSHI_AMATERASU 
		||kamioroshi_type == KAMIOROSHI_YAOYOROZU || kamioroshi_type == KAMIOROSHI_HONOIKAZUTI || kamioroshi_type == KAMIOROSHI_AMATSUMIKABOSHI) 
		kamioroshi_type = 0L;

	if ((v) && (kamioroshi_type))
	{
		p_ptr->kamioroshi |= (kamioroshi_type);
		p_ptr->kamioroshi_count = v;
	}
	//�����^�_�~�낵�̓J�E���g�ƃt���O�����Z�b�g�B�������_�~�낵�����\�ȐE�̓��Z�b�g���Ȃ��B
	//2�~�낵�Ă���Ȃ�Е��c�邵1�����~�낵�ĂȂ��Ȃ�g�ɗ]�T�����邽�߁B
	else if(!v && !kamioroshi_type && !flag_master)
	{
		p_ptr->kamioroshi_count = 0;
		p_ptr->kamioroshi = 0L;
	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	///v1.1.14 �˕P�̋_���l����������concent�l�������0�ɂ���B���͏�����^�C���A�E�g�Ȃ�
	if(p_ptr->pclass == CLASS_YORIHIME && !(p_ptr->kamioroshi & KAMIOROSHI_GION) && p_ptr->concent)
		p_ptr->concent = 0;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS | PR_STATS | PR_ARMOR);

	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}


/*:::�Í��̈�@�łƂ̗Z�a*/
bool set_tim_unite_darkness(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_unite_darkness && !do_dec)
		{
			if (p_ptr->tim_unite_darkness > v) return FALSE;
		}
		else if (!p_ptr->tim_unite_darkness)
		{
#ifdef JP
msg_print("���Ȃ��͈̑̂ł֗n�����B");
#else
			msg_print("Your skin turns to stone.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_unite_darkness)
		{
#ifdef JP
msg_print("���Ȃ��͈ł̒�����p���������B");
#else
			msg_print("Your skin returns to normal.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_unite_darkness = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_TORCH | PU_VIEW | PU_LITE | PU_MON_LITE | PU_MONSTERS );
	p_ptr->redraw |= PR_MAP;

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_superstealth(int v, bool do_dec, int stealth_type)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		//���B���̃^�C�v(���ʁA���w���ʁA�X�B��Ȃ�)���L�^�B�g�p���ɑ��̃^�C�v�̂��g�p�����炻����ɏ㏑��
		if (!do_dec)
		{
			p_ptr->superstealth_type = stealth_type;

		}

		/*:::do_dec��FALSE�̂Ƃ��́Av�̂ق����������ꍇ�������Ȃ� ������stealth�̃^�C�v�͐V�����ق��ɕς��*/
		if (p_ptr->tim_superstealth && !do_dec)
		{
			if (p_ptr->tim_superstealth > v) return FALSE;
		}
		else if (!p_ptr->tim_superstealth)
		{
			if (stealth_type == SUPERSTEALTH_TYPE_FOREST)
			{
				if (cave_have_flag_bold((py), (px), FF_TREE))
					msg_print("���Ȃ��̎p�͐X�̒��ɏ�����...");
				else
					msg_print("���Ȃ��͐X�̒��ɐg���B���ׂ��g�\�����B");

			}
			else if(p_ptr->pclass == CLASS_3_FAIRIES)
			///msg131221
				msg_print("���Ȃ������͓G�̖ڂɉf��Ȃ��Ȃ����B");
			else
				msg_print("���Ȃ��͓G�̖ڂɉf��Ȃ��Ȃ����B");

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		//���B���̃^�C�v��superstealth_type�ɋL�^����O�̃t���O
		//v1.1.88 superstealth_type�ɒu�����������A�A�b�v�f�[�g�̂Ƃ��Ɍ��w���ʒ��̃v���C���[�����邩�������̂ł��΂炭�t���O�𗎂Ƃ��������c���Ƃ�
		//v2.0.1 ���������Ă������낤
		//p_ptr->special_defense &= ~(SD_OPTICAL_STEALTH);


		if (p_ptr->tim_superstealth)
		{
			msg_print("�ĂѓG�̖ڂɉf��悤�ɂȂ����B");
			notice = TRUE;
		}

		p_ptr->superstealth_type = 0;
	}

	/* Use the value */
	p_ptr->tim_superstealth = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	//check
	if (cheat_xtra) msg_format("stealth type:%d", p_ptr->superstealth_type);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



//v1.1.93 �ꎞ��������
bool set_tim_aggravation(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_aggravation && !do_dec)
		{
			if (p_ptr->tim_aggravation > v) return FALSE;
		}
		else if (!p_ptr->tim_aggravation)
		{
			msg_print("�}�Ɍ����҂ɂȂ����C������...");

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_aggravation)
		{
			msg_print("�����Ă��銴�����Ȃ��Ȃ����B");

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_aggravation = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}




/*
 * Set "p_ptr->tsubureru", notice observable changes
 */
/*:::���ɐL�т遨����̃I�[���ɕύX*/
bool set_tim_sh_death(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_death && !do_dec)
		{
			if (p_ptr->tim_sh_death > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_death)
		{
#ifdef JP
msg_print("������グ�鎀�삽�������Ȃ��̎���ɉQ�������B");
#else
			msg_print("Your body expands horizontally.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_death)
		{
#ifdef JP
msg_print("���Ȃ��̎���̎��삽�����������B");
#else
			msg_print("Your body returns to normal.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_death = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->magicdef", notice observable changes
 */
/*:::���@�̊Z�@���@�h���AC�㏸�A�������̃X�e�[�^�X�ُ�ϐ��@���͏����̑Ώ�*/
bool set_magicdef(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->magicdef && !do_dec)
		{
			if (p_ptr->magicdef > v) return FALSE;
		}
		else if (!p_ptr->magicdef)
		{
#ifdef JP
			msg_print("���@�̖h��͂��������悤�ȋC������B");
#else
			msg_print("You feel more resistant to magic.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->magicdef)
		{
#ifdef JP
			msg_print("���@�̖h��͂����ɖ߂����B");
#else
			msg_print("You feel less resistant to magic.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->magicdef = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->blessed", notice observable changes
 */
bool set_blessed(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->blessed && !do_dec)
		{
			if (p_ptr->blessed > v) return FALSE;
		}
		else if (!IS_BLESSED())
		{
#ifdef JP
			///msg131221
			msg_print("�傢�Ȃ鉽���̏j�����󂯂�����������B");
#else
			msg_print("You feel righteous!");
#endif
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->blessed && !music_singing(MUSIC_BLESS))
		{
#ifdef JP
msg_print("�j�����ꂽ�������������B");
#else
			msg_print("The prayer has expired.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->blessed = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->hero", notice observable changes
 */
bool set_hero(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->hero && !do_dec)
		{
			if (p_ptr->hero > v) return FALSE;
		}
		else if (!IS_HERO())
		{
#ifdef JP
msg_print("�q�[���[�ɂȂ����C������I");
#else
			msg_print("You feel like a hero!");
#endif

			notice = TRUE;
		}

		//v1.1.38 �q�[���[������Ƃ����|�������鏈����t���Y��₷���̂ŁA�����������Ɏ����Ă��邱�Ƃɂ����B
		if(!do_dec)	set_afraid(0);


	}

	/* Shut */
	else
	{
		if (p_ptr->hero && !music_singing(MUSIC_HERO) && !music_singing(MUSIC_SHERO))
		{
#ifdef JP
msg_print("�q�[���[�̋C���������������B");
#else
			msg_print("The heroism wears off.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->hero = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	p_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->shero", notice observable changes
 */
bool set_shero(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if(p_ptr->pclass == CLASS_EIRIN && p_ptr->pseikaku != SEIKAKU_BERSERK && v && !do_dec)
	{
		msg_print("���Ȃ��͗�Â�������Ȃ������B");
		return FALSE;
	}

	///sys class ����m�͏�Ƀo�[�T�[�N�ɂȂ鏈��
	if ( p_ptr->pseikaku == SEIKAKU_BERSERK) v = 1;
	/* Open */
	if (v)
	{
		if (p_ptr->shero && !do_dec)
		{
			if (p_ptr->shero > v) return FALSE;
		}
		else if (!p_ptr->shero)
		{
#ifdef JP
msg_print("�E�C�}�V�[���ɂȂ����C������I");
#else
			msg_print("You feel like a killing machine!");
#endif
			if(one_in_(3)) set_deity_bias(DBIAS_COSMOS, -1);

			notice = TRUE;
		}

		//v1.1.38 �q�[���[������Ƃ����|�������鏈����t���Y��₷���̂ŁA�����������Ɏ����Ă��邱�Ƃɂ����B
		if(!do_dec)	set_afraid(0);


	}

	/* Shut */
	else
	{
		if (p_ptr->shero)
		{
#ifdef JP
msg_print("��؂ȋC�����������������B");
#else
			msg_print("You feel less Berserk.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->shero = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	p_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->protevil", notice observable changes
 */
bool set_protevil(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0])
	{
		msg_print("���̖��@�����ł��������I");
		hina_gain_yaku(-v);
		v=0;
	}


	/* Open */
	if (v)
	{
		if (p_ptr->protevil && !do_dec)
		{
			if (p_ptr->protevil > v) return FALSE;
		}
		else if (!p_ptr->protevil)
		{
#ifdef JP
msg_print("�׈��Ȃ鑶�݂������Ă���悤�Ȋ���������I");
#else
			msg_print("You feel safe from evil!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->protevil)
		{
#ifdef JP
msg_print("�׈��Ȃ鑶�݂������Ă��銴�����Ȃ��Ȃ����B");
#else
			msg_print("You no longer feel safe from evil.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->protevil = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->wraith_form", notice observable changes
 */
///mod140213 ���[�~�A�̃_�[�N�T�C�h�I�u�U���[����H�̉��Ƌ��L�����ɂ����B
bool set_wraith_form(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->wraith_form && !do_dec)
		{
			if (p_ptr->wraith_form > v) return FALSE;
		}
		else if (!p_ptr->wraith_form)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_RUMIA) msg_print("���Ȃ��͈̑̂łɗn�����B");
			else msg_print("�����E�𗣂�ėH�S�̂悤�ȑ��݂ɂȂ����I");
#else
			msg_print("You leave the physical world and turn into a wraith-being!");
#endif

			notice = TRUE;
			///del131214 virtue
			//chg_virtue(V_UNLIFE, 3);
			//chg_virtue(V_HONOUR, -2);
			//chg_virtue(V_SACRIFICE, -2);
			//chg_virtue(V_VALOUR, -5);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->wraith_form)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_RUMIA) msg_print("���Ȃ��͍̑̂Ăт͂����肵���`���Ƃ����B");
			else msg_print("�s�����ɂȂ�������������B");
#else
			msg_print("You feel opaque.");
#endif

			notice = TRUE;

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Use the value */
	p_ptr->wraith_form = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);

}


/*
 * Set "p_ptr->invuln", notice observable changes
 */
bool set_invuln(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->invuln && !do_dec)
		{
			if (p_ptr->invuln > v) return FALSE;
		}
		else if (!IS_INVULN())
		{
#ifdef JP
msg_print("���G���I");
#else
			msg_print("Invulnerability!");
#endif

			notice = TRUE;
			///del131221 virtue
			//chg_virtue(V_UNLIFE, -2);
			//chg_virtue(V_HONOUR, -2);
			//chg_virtue(V_SACRIFICE, -3);
			//chg_virtue(V_VALOUR, -5);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->invuln && !music_singing(MUSIC_INVULN))
		{
#ifdef JP
msg_print("���G�ł͂Ȃ��Ȃ����B");
#else
			msg_print("The invulnerability wears off.");
#endif

			notice = TRUE;

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

			p_ptr->energy_need += ENERGY_NEED();
		}
	}

	/* Use the value */
	p_ptr->invuln = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	//v1.1.58
	flag_update_floor_music = TRUE;

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_esp", notice observable changes
 */
/*:::�ꎞ�e���p�X�𓾂�*/
bool set_tim_esp(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_esp && !do_dec)
		{
			if (p_ptr->tim_esp > v) return FALSE;
		}
		else if (!IS_TIM_ESP())
		{
#ifdef JP
msg_print("�ӎ����L�������C������I");
#else
			msg_print("You feel your consciousness expand!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_esp && !music_singing(MUSIC_MIND))
		{
#ifdef JP
msg_print("�ӎ��͌��ɖ߂����B");
#else
			msg_print("Your consciousness contracts again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_esp = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_invis", notice observable changes
 */
bool set_tim_invis(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_invis && !do_dec)
		{
			if (p_ptr->tim_invis > v) return FALSE;
		}
		else if (!p_ptr->tim_invis)
		{
#ifdef JP
msg_print("�ڂ����ɕq���ɂȂ����C������I");
#else
			msg_print("Your eyes feel very sensitive!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_invis)
		{
#ifdef JP
msg_print("�ڂ̕q�������Ȃ��Ȃ����悤���B");
#else
			msg_print("Your eyes feel less sensitive.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_invis = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::�����̈�́u���ҏp�v�@��莞�ԓG�̏�����j�Q���� */
bool set_deportation(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->deportation && !do_dec)
		{
			if (p_ptr->deportation > v) return FALSE;
		}
		else if (!p_ptr->deportation)
		{
#ifdef JP
msg_print("���Ȃ��͎��͂̋�Ԃ֊����n�߂��E�E");
#else
			msg_print("Your eyes feel very sensitive!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->deportation)
		{
#ifdef JP
msg_print("���͂̋�Ԃւ̊����~�܂����B");
#else
			msg_print("Your eyes feel less sensitive.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->deportation = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_foresight(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->foresight && !do_dec)
		{
			if (p_ptr->foresight > v) return FALSE;
		}
		else if (!p_ptr->foresight)
		{
#ifdef JP
			msg_print("���݂̌i�F�ɖ����̌i�F���d�Ȃ��Č�����I");
#else
			msg_print("Your eyes feel very sensitive!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->foresight)
		{
#ifdef JP
msg_print("�����������Ȃ��Ȃ����B");
#else
			msg_print("Your eyes feel less sensitive.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->foresight = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_infra", notice observable changes
 */
bool set_tim_infra(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_infra && !do_dec)
		{
			if (p_ptr->tim_infra > v) return FALSE;
		}
		else if (!p_ptr->tim_infra)
		{
#ifdef JP
msg_print("�ڂ����������ƋP���n�߂��I");
#else
			msg_print("Your eyes begin to tingle!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_infra)
		{
#ifdef JP
msg_print("�ڂ̋P�����Ȃ��Ȃ����B");
#else
			msg_print("Your eyes stop tingling.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_infra = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_regen", notice observable changes
 */
///sys �}�񕜗ݐς������ꍇ�������C��
bool set_tim_regen(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_regen && !do_dec)
		{
			if (p_ptr->tim_regen > v) return FALSE;
		}
		else if (!p_ptr->tim_regen)
		{
#ifdef JP
msg_print("�񕜗͂��オ�����I");
#else
			msg_print("You feel yourself regenerating quickly!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_regen)
		{
#ifdef JP
msg_print("�f�����񕜂��銴�����Ȃ��Ȃ����B");
#else
			msg_print("You feel yourself regenerating slowly.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_regen = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_stealth", notice observable changes
 */
bool set_tim_stealth(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_stealth && !do_dec)
		{
			if (p_ptr->tim_stealth > v) return FALSE;
		}
		else if (!IS_TIM_STEALTH())
		{
#ifdef JP
			///msg131221
			msg_print("���Ȃ��͋C�z���������B");
#else
			msg_print("You begin to walk silently!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_stealth && !music_singing(MUSIC_STEALTH))
		{
#ifdef JP
			msg_print("�C�z�������Ȃ��Ȃ����B");
#else
			msg_print("You no longer walk silently.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_stealth = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

///class �E�҂̒��B��
bool set_superstealth(bool set)
{
	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (set)
	{
		if (!(p_ptr->special_defense & NINJA_S_STEALTH))
		{
			if (cave[py][px].info & CAVE_MNLT)
			{
#ifdef JP
				if(!p_ptr->tim_superstealth) msg_print("�G�̖ڂ��甖���e�̒��ɕ����B���ꂽ�B");
#else
				msg_print("You are mantled in weak shadow from ordinary eyes.");
#endif
				p_ptr->monlite = p_ptr->old_monlite = TRUE;
			}
			else
			{
#ifdef JP
				if(!p_ptr->tim_superstealth) msg_print("�G�̖ڂ���e�̒��ɕ����B���ꂽ�I");
#else
				msg_print("You are mantled in shadow from ordinary eyes!");
#endif
				p_ptr->monlite = p_ptr->old_monlite = FALSE;
			}

			notice = TRUE;

			/* Use the value */
			p_ptr->special_defense |= NINJA_S_STEALTH;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->special_defense & NINJA_S_STEALTH)
		{
#ifdef JP
			if(!p_ptr->tim_superstealth) msg_print("�ĂѓG�̖ڂɂ��炳���悤�ɂȂ����B");
#else
			msg_print("You are exposed to common sight once more.");
#endif

			notice = TRUE;

			/* Use the value */
			p_ptr->special_defense &= ~(NINJA_S_STEALTH);
		}
	}

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_levitation", notice observable changes
 */
/*:::�ꎞ���V�@��{�I�Ȉꎞ�X�e�[�^�X�֐�*/
bool set_tim_levitation(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_levitation && !do_dec)
		{
			if (p_ptr->tim_levitation > v) return FALSE;
		}
		else if (!p_ptr->tim_levitation)
		{
#ifdef JP
			///msg131221
			msg_print("���Ȃ��̑̂͒��ɕ����񂾁B");
#else
			msg_print("You begin to fly!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_levitation)
		{
#ifdef JP
msg_print("�������ɕ����ׂȂ��Ȃ����B");
#else
			msg_print("You stop flying.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_levitation = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_sh_touki", notice observable changes
 */
/*:::�Z���C�@��{�I�Ȉꎞ�X�e�[�^�X����*/
bool set_tim_sh_touki(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_touki && !do_dec)
		{
			if (p_ptr->tim_sh_touki > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_touki)
		{
#ifdef JP
msg_print("�̂����C�̃I�[���ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by the aura of the Force!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_touki)
		{
#ifdef JP
msg_print("���C���������B");
#else
			msg_print("Aura of the Force disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_touki = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_sh_fire", notice observable changes
 */
bool set_tim_sh_fire(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_fire && !do_dec)
		{
			if (p_ptr->tim_sh_fire > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_fire)
		{
#ifdef JP
msg_print("�̂����̃I�[���ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by fiery aura!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_fire)
		{
#ifdef JP
msg_print("���̃I�[�����������B");
#else
			msg_print("Fiery aura disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_fire = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_sh_holy", notice observable changes
 */
bool set_tim_sh_holy(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_sh_holy && !do_dec)
		{
			if (p_ptr->tim_sh_holy > v) return FALSE;
		}
		else if (!p_ptr->tim_sh_holy)
		{
#ifdef JP
msg_print("�̂����Ȃ�I�[���ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by holy aura!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_sh_holy)
		{
#ifdef JP
msg_print("���Ȃ�I�[�����������B");
#else
			msg_print("Holy aura disappeared.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_sh_holy = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->tim_eyeeye", notice observable changes
 */
bool set_tim_eyeeye(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_eyeeye && !do_dec)
		{
			if (p_ptr->tim_eyeeye > v) return FALSE;
		}
		else if (!p_ptr->tim_eyeeye)
		{
#ifdef JP
			///msg131221
			//msg_print("�@�̎���ɂȂ����C�������I");
			msg_print("���Ȃ��͍U����҂��\���Ă���B");

#else
			msg_print("You feel like a keeper of commandments!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_eyeeye)
		{
#ifdef JP
			msg_print("�U����҂��\����̂���߂��B");
			//msg_print("���������s���邱�Ƃ��ł��Ȃ��Ȃ����B");
#else
			msg_print("You no longer feel like a keeper.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_eyeeye = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->resist_magic", notice observable changes
 */
/*:::���@�h��㏸�@�B�C�p�Ƌ����@*/
bool set_resist_magic(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->resist_magic && !do_dec)
		{
			if (p_ptr->resist_magic > v) return FALSE;
		}
		else if (!p_ptr->resist_magic)
		{
#ifdef JP
msg_print("���@�ւ̑ϐ��������B");
#else
			msg_print("You have been protected from magic!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->resist_magic)
		{
#ifdef JP
msg_print("���@�Ɏキ�Ȃ����B");
#else
msg_print("You are no longer protected from magic.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->resist_magic = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_reflect", notice observable changes
 */
bool set_tim_reflect(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_reflect && !do_dec)
		{
			if (p_ptr->tim_reflect > v) return FALSE;
		}
		else if (!p_ptr->tim_reflect)
		{
#ifdef JP
			///msg131221
msg_print("���Ȃ玩����_������𒵂˕Ԃ���C������B");
#else
			msg_print("Your body becames smooth.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_reflect)
		{
#ifdef JP
msg_print("��ʂ���̖h�삪�������B");
#else
			msg_print("Your body is no longer smooth.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_reflect = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * ����̈�́u�O���v�p�̏���
 */
bool set_nennbaku(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->nennbaku && !do_dec)
		{
			if (p_ptr->nennbaku > v) return FALSE;
		}
		else if (!p_ptr->nennbaku)
		{
			if(p_ptr->pclass == CLASS_YUGI)
				msg_print("���ƞg���Ј��I�ɒ��𕑂����B");
			else if (p_ptr->pclass == CLASS_KUTAKA); //���b�Z�[�W�Ȃ�
			else
				msg_print("�n�ʂ��甼�����̖����̘r�������Ă����E�E");

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->nennbaku)
		{
			if(p_ptr->pclass == CLASS_YUGI)
				msg_print("���ƞg���������B");
			else if (p_ptr->pclass == CLASS_KUTAKA); //���b�Z�[�W�Ȃ�
			else
				msg_print("�n�ʂ���̘r���������B");

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->nennbaku = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->multishadow", notice observable changes
 */
/*:::���g����*/
bool set_multishadow(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->multishadow && !do_dec)
		{
			if (p_ptr->multishadow > v) return FALSE;
		}
		else if (!p_ptr->multishadow)
		{
			if(p_ptr->pclass == CLASS_NUE || p_ptr->pclass == CLASS_AUNN)
				msg_format("���Ȃ��ׂ̗ɕ��g���p���������B");
			else if (p_ptr->pclass == CLASS_SHINMYOUMARU)
				msg_format("���Ȃ��͎��l�ɕ��g�����I");
			else
				msg_print("���Ȃ��̎���Ɍ��e�����܂ꂽ�B");

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->multishadow)
		{
#ifdef JP
			if (p_ptr->pclass == CLASS_AUNN)
				msg_format("���Ȃ��̕��g�͐_�Ђɖ߂����B");
			else
				msg_print("���e���������B");
#else
			msg_print("Your Shadow disappears.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->multishadow = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->dustrobe", notice observable changes
 */
bool set_dustrobe(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->dustrobe && !do_dec)
		{
			if (p_ptr->dustrobe > v) return FALSE;
		}
		else if (!p_ptr->dustrobe)
		{
#ifdef JP

//msg_print("�̂����̃I�[���ŕ���ꂽ�B");
#else
			msg_print("You were enveloped by mirror shards.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->dustrobe)
		{
#ifdef JP
			if(p_ptr->pclass == CLASS_IKU) msg_print("�����������B");
			else if(p_ptr->pclass == CLASS_PATCHOULI) msg_print("��]����n���������B");
			else if(p_ptr->pclass == CLASS_SANAE) msg_print("���ɕ`�����㎚���������B");
			else if(p_ptr->pclass == CLASS_NITORI) msg_print("�M�A�̉�]���~�܂����B");
			else if(p_ptr->pclass == CLASS_BYAKUREN) msg_print("�아���������B");
			else msg_print("���̃I�[�����������B");
#else
			msg_print("The mirror shards disappear.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->dustrobe = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_regen", notice observable changes
 */
/*:::�ǔ�����Ԃ̊Ǘ�*/
bool set_kabenuke(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->kabenuke && !do_dec)
		{
			if (p_ptr->kabenuke > v) return FALSE;
		}
		else if (!p_ptr->kabenuke)
		{
#ifdef JP
msg_print("�̂��������̏�ԂɂȂ����B");
#else
			msg_print("You became ethereal form.");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->kabenuke)
		{
#ifdef JP
msg_print("�̂������������B");
#else
			msg_print("You are no longer in an ethereal form.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->kabenuke = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tsuyoshi(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tsuyoshi && !do_dec)
		{
			if (p_ptr->tsuyoshi > v) return FALSE;
		}
		else if (!p_ptr->tsuyoshi)
		{
#ifdef JP
msg_print("�u�I�N���Z����I�v");
#else
			msg_print("Brother OKURE!");
#endif

			notice = TRUE;
			///del131214 virtue
			//chg_virtue(V_VITALITY, 2);
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tsuyoshi)
		{
#ifdef JP
msg_print("���̂��}���ɂ��ڂ�ł������B");
#else
			msg_print("Your body had quickly shriveled.");
#endif

			(void)dec_stat(A_CON, 20, TRUE);
			(void)dec_stat(A_STR, 20, TRUE);

			notice = TRUE;
			///del131214 virtue
			//chg_virtue(V_VITALITY, -3);
		}
	}

	/* Use the value */
	p_ptr->tsuyoshi = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate hitpoints */
	p_ptr->update |= (PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set a temporary elemental brand.  Clear all other brands.  Print status 
 * messages. -LM-
 */
///mod140725 ���@���m�Ȃǖ��@���̎�ޑ��₵��
bool set_ele_attack(u32b attack_type, int v)
{
	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Clear all elemental attacks (only one is allowed at a time). */
	if ((p_ptr->special_attack & (ATTACK_ACID)) && (attack_type != ATTACK_ACID))
	{
		p_ptr->special_attack &= ~(ATTACK_ACID);
		msg_print("�_�ōU���ł��Ȃ��Ȃ����B");
	}
	if ((p_ptr->special_attack & (ATTACK_ELEC)) && (attack_type != ATTACK_ELEC))
	{
		p_ptr->special_attack &= ~(ATTACK_ELEC);
		msg_print("�d���ōU���ł��Ȃ��Ȃ����B");
	}
	if ((p_ptr->special_attack & (ATTACK_FIRE)) && (attack_type != ATTACK_FIRE))
	{
		p_ptr->special_attack &= ~(ATTACK_FIRE);
		msg_print("�Ή��ōU���ł��Ȃ��Ȃ����B");
	}
	if ((p_ptr->special_attack & (ATTACK_COLD)) && (attack_type != ATTACK_COLD))
	{
		p_ptr->special_attack &= ~(ATTACK_COLD);
		msg_print("��C�ōU���ł��Ȃ��Ȃ����B");
	}
	if ((p_ptr->special_attack & (ATTACK_POIS)) && (attack_type != ATTACK_POIS))
	{
		p_ptr->special_attack &= ~(ATTACK_POIS);
		msg_print("�łōU���ł��Ȃ��Ȃ����B");
	}
	if ((p_ptr->special_attack & (ATTACK_DARK)) && (attack_type != ATTACK_DARK))
	{
		p_ptr->special_attack &= ~(ATTACK_DARK);
		msg_print("�����̋P�����������B");
	}
	if ((p_ptr->special_attack & (ATTACK_FORCE)) && (attack_type != ATTACK_FORCE))
	{
		p_ptr->special_attack &= ~(ATTACK_FORCE);
		msg_print("���͂̋P�����������B");
	}
	if ((p_ptr->special_attack & (ATTACK_FORESIGHT)) && (attack_type != ATTACK_FORESIGHT))
	{
		p_ptr->special_attack &= ~(ATTACK_FORESIGHT);
		msg_print("���킪�����������Ȃ��Ȃ����B");
	}
	if ((p_ptr->special_attack & (ATTACK_UNSUMMON)) && (attack_type != ATTACK_UNSUMMON))
	{
		p_ptr->special_attack &= ~(ATTACK_UNSUMMON);
		msg_print("���킩��c�݂��������B");
	}
	if ((p_ptr->special_attack & (ATTACK_INC_DICES)) && (attack_type != ATTACK_INC_DICES))
	{
		p_ptr->special_attack &= ~(ATTACK_INC_DICES);
		msg_print("����̋������@�̌��͂��������B");
	}
	if ((p_ptr->special_attack & (ATTACK_CHAOS)) && (attack_type != ATTACK_CHAOS))
	{
		p_ptr->special_attack &= ~(ATTACK_CHAOS);
		msg_print("���킩�獬�ׂ̗͂��������B");
	}

	if ((p_ptr->special_attack & (ATTACK_VORPAL)) && (attack_type != ATTACK_VORPAL))
	{
		p_ptr->special_attack &= ~(ATTACK_VORPAL);
		msg_print("�n�̐؂ꂪ�݂����C������B");
	}

	if ((v) && (attack_type))
	{
		/* Set attack type. */
		p_ptr->special_attack |= (attack_type);

		/* Set duration. */
		p_ptr->ele_attack = v;

		/* Message. */

		if(p_ptr->pclass == CLASS_ALICE)
		{
			if(attack_type == ATTACK_DARK) msg_print("�l�`�����̕���͗d��������������B");
			else if(attack_type == ATTACK_ACID) msg_print("�l�`�����̕��킩�牌������������B");
			else if(attack_type == ATTACK_ELEC) msg_print("�l�`�����̕��킩��ΉԂ��U�����B");
			else if(attack_type == ATTACK_FIRE) msg_print("�l�`�����̕���͐ԔM�����B");
			else if(attack_type == ATTACK_COLD) msg_print("�l�`�����̕���͗�C��������B");
			else if(attack_type == ATTACK_POIS) msg_print("�l�`�����̕��킩��ŉt���H�����B");
			else if (attack_type == ATTACK_VORPAL) msg_print("�l�`�����̕���͉s���P�����B");
			else msg_print("WARNING:�A���Xset_ele_attack()�̃��b�Z�[�W����`");
		}
		else
		{
			if(attack_type == ATTACK_DARK) msg_print("���Ȃ��̕���͗d�����P�����E�E");
			else if(attack_type == ATTACK_FORCE) msg_print("����͂��Ȃ��̈ӎv�Ɍĉ�����悤�Ɍ������B");
			else if(attack_type == ATTACK_FORESIGHT) msg_print("���킪����ɓG�̓�������肷��C������I");
			else if(attack_type == ATTACK_UNSUMMON) msg_print("����͊�Șc�݂��܂Ƃ����E�E");
			else if(attack_type == ATTACK_INC_DICES) msg_print("����ɔj��I�Ȗ��͂��h�����I");
			else if(attack_type == ATTACK_CHAOS) msg_print("���킪��ɔP����Č�����E�E");
			else if (attack_type == ATTACK_VORPAL) msg_print("����̐n���M�����ƋP�����B");
			else
				msg_format("%s�ōU���ł���悤�ɂȂ����I",
			     ((attack_type == ATTACK_ACID) ? "�_" :
			      ((attack_type == ATTACK_ELEC) ? "�d��" :
			       ((attack_type == ATTACK_FIRE) ? "�Ή�" : 
				((attack_type == ATTACK_COLD) ? "��C" : 
				 ((attack_type == ATTACK_POIS) ? "��" : 
					"(�Ȃ�)"))))));

		}

	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	p_ptr->update |= (PU_BONUS);

	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}



/*
 * Set a temporary elemental brand.  Clear all other brands.  Print status 
 * messages. -LM-
 */
/*:::�ꎞ���f�Ɖu�@�������L�^����K�v������̂ŏ�����������₱����*/
bool set_ele_immune(u32b immune_type, int v)
{
	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Clear all elemental attacks (only one is allowed at a time). */
	if ((p_ptr->special_defense & (DEFENSE_ACID)) && (immune_type != DEFENSE_ACID))
	{
		p_ptr->special_defense &= ~(DEFENSE_ACID);
#ifdef JP
		msg_print("�_�̍U���ŏ�������悤�ɂȂ����B�B");
#else
		msg_print("You are no longer immune to acid.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_ELEC)) && (immune_type != DEFENSE_ELEC))
	{
		p_ptr->special_defense &= ~(DEFENSE_ELEC);
#ifdef JP
		msg_print("�d���̍U���ŏ�������悤�ɂȂ����B�B");
#else
		msg_print("You are no longer immune to electricity.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_FIRE)) && (immune_type != DEFENSE_FIRE))
	{
		p_ptr->special_defense &= ~(DEFENSE_FIRE);
#ifdef JP
		msg_print("�Ή��̍U���ŏ�������悤�ɂȂ����B�B");
#else
		msg_print("You are no longer immune to fire.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_COLD)) && (immune_type != DEFENSE_COLD))
	{
		p_ptr->special_defense &= ~(DEFENSE_COLD);
#ifdef JP
		msg_print("��C�̍U���ŏ�������悤�ɂȂ����B�B");
#else
		msg_print("You are no longer immune to cold.");
#endif
	}
	if ((p_ptr->special_defense & (DEFENSE_POIS)) && (immune_type != DEFENSE_POIS))
	{
		p_ptr->special_defense &= ~(DEFENSE_POIS);
#ifdef JP
		msg_print("�ł̍U���ŏ�������悤�ɂȂ����B�B");
#else
		msg_print("You are no longer immune to poison.");
#endif
	}

	if ((v) && (immune_type))
	{
		p_ptr->ele_immune = v;

		if(immune_type == DEFENSE_4ELEM)
		{
			p_ptr->special_defense |= (DEFENSE_ACID | DEFENSE_ELEC | DEFENSE_FIRE | DEFENSE_COLD);	
			msg_print("�l���f�̍U�����󂯕t���Ȃ��Ȃ����I");
		}
		else
		{
			/* Set attack type. */
			p_ptr->special_defense |= (immune_type);

		/* Set duration. */

		/* Message. */
#ifdef JP
			msg_format("%s�̍U�����󂯂��Ȃ��Ȃ����I",
			     ((immune_type == DEFENSE_ACID) ? "�_" :
			      ((immune_type == DEFENSE_ELEC) ? "�d��" :
			       ((immune_type == DEFENSE_FIRE) ? "�Ή�" : 
				((immune_type == DEFENSE_COLD) ? "��C" : 
				 ((immune_type == DEFENSE_POIS) ? "��" : 
					"(�Ȃ�)"))))));
#else
		msg_format("For a while, You are immune to %s",
			     ((immune_type == DEFENSE_ACID) ? "acid!" :
			      ((immune_type == DEFENSE_ELEC) ? "electricity!" :
			       ((immune_type == DEFENSE_FIRE) ? "fire!" : 
				((immune_type == DEFENSE_COLD) ? "cold!" : 
				 ((immune_type == DEFENSE_POIS) ? "poison!" : 
					"do nothing special."))))));
#endif
		}

	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	return (TRUE);
}


/*
 * Set "p_ptr->oppose_acid", notice observable changes
 */
bool set_oppose_acid(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_acid && !do_dec)
		{
			if (p_ptr->oppose_acid > v) return FALSE;
		}
		else if (!IS_OPPOSE_ACID())
		{
#ifdef JP
msg_print("�_�ւ̑ϐ��������C������I");
#else
			msg_print("You feel resistant to acid!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_acid && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("�_�ւ̑ϐ������ꂽ�C������B");
#else
			msg_print("You feel less resistant to acid.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_acid = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_elec", notice observable changes
 */
bool set_oppose_elec(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	///mod140216 �ߋ��Lv30�ŗ�����d�ϐ�
	if ((p_ptr->pclass == CLASS_IKU || p_ptr->pclass == CLASS_TOZIKO ||p_ptr->pclass == CLASS_RAIKO) && (p_ptr->lev > 29)) v=1;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_elec && !do_dec)
		{
			if (p_ptr->oppose_elec > v) return FALSE;
		}
		else if (!IS_OPPOSE_ELEC())
		{
#ifdef JP
msg_print("�d���ւ̑ϐ��������C������I");
#else
			msg_print("You feel resistant to electricity!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_elec && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("�d���ւ̑ϐ������ꂽ�C������B");
#else
			msg_print("You feel less resistant to electricity.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_elec = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_fire", notice observable changes
 */
/*:::�Ή��ꎞ�ϐ��@�����ƍ����x���̃o�����O�͂����̒l��0�ɂȂ�Ȃ��悤��������Ă���*/
bool set_oppose_fire(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;


	///mod140608 ���ӂ�Lv35�ŉΉ���d�ϐ�
	if ((p_ptr->pclass == CLASS_ORIN || p_ptr->pclass == CLASS_FLAN) && (p_ptr->lev > 34)) v=1;
	else if ((p_ptr->pclass == CLASS_MOKOU) && (p_ptr->lev > 29)) v=1;
	else if ((p_ptr->pclass == CLASS_FUTO) && (p_ptr->lev > 39)) v=1;
	else if ((p_ptr->pclass == CLASS_MAYUMI) && (p_ptr->lev > 29)) v = 1;
	else if ((p_ptr->pclass == CLASS_KEIKI) && (p_ptr->lev > 29)) v = 1;
	else if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && (p_ptr->lev > 29)) v = 1;

	if ( (p_ptr->mimic_form == MIMIC_DEMON)) v = 1;
	//if ((prace_is_(RACE_DEMON) && (p_ptr->lev > 44)) || (p_ptr->mimic_form == MIMIC_DEMON)) v = 1;
	/* Open */
	if (v)
	{
		if (p_ptr->oppose_fire && !do_dec)
		{
			if (p_ptr->oppose_fire > v) return FALSE;
		}
		else if (!IS_OPPOSE_FIRE())
		{
#ifdef JP
msg_print("�΂ւ̑ϐ��������C������I");
#else
			msg_print("You feel resistant to fire!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_fire && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("�΂ւ̑ϐ������ꂽ�C������B");
#else
			msg_print("You feel less resistant to fire.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_fire = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_cold", notice observable changes
 */
bool set_oppose_cold(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	///mod140729 ���e�B��Lv20�ŗ�C��d�ϐ�
	if ((p_ptr->pclass == CLASS_LETTY) && (p_ptr->lev > 19)) v=1;

	if ((p_ptr->pclass == CLASS_NARUMI) && (p_ptr->lev > 29)) v=1;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_cold && !do_dec)
		{
			if (p_ptr->oppose_cold > v) return FALSE;
		}
		else if (!IS_OPPOSE_COLD())
		{
#ifdef JP
msg_print("��C�ւ̑ϐ��������C������I");
#else
			msg_print("You feel resistant to cold!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_cold && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("��C�ւ̑ϐ������ꂽ�C������B");
#else
			msg_print("You feel less resistant to cold.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_cold = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->oppose_pois", notice observable changes
 */
bool set_oppose_pois(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if ((p_ptr->pclass == CLASS_NINJA) && (p_ptr->lev > 44)) v = 1;

	if ((p_ptr->pclass == CLASS_MOMOYO) && (p_ptr->lev > 19)) v = 1;
	if ((p_ptr->pclass == CLASS_YUMA) && (p_ptr->lev > 19)) v = 1;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->oppose_pois && !do_dec)
		{
			if (p_ptr->oppose_pois > v) return FALSE;
		}
		else if (!IS_OPPOSE_POIS())
		{
#ifdef JP
msg_print("�łւ̑ϐ��������C������I");
#else
			msg_print("You feel resistant to poison!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->oppose_pois && !music_singing(MUSIC_NEW_LYRICA_SOLO) && !music_singing(MUSIC_RESIST) && !(p_ptr->special_defense & KATA_MUSOU))
		{
#ifdef JP
msg_print("�łւ̑ϐ������ꂽ�C������B");
#else
			msg_print("You feel less resistant to poison.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->oppose_pois = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->stun", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_stun(int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;

	//v1.1.66 ���ւȂǂ͕ʊ֐��֔�΂��Ĕj������������
	if (RACE_BREAKABLE) return set_broken(v * 10);

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;
	///race class �N�O�ւ̑ϐ�
	if (prace_is_(RACE_GOLEM) || (( p_ptr->pseikaku == SEIKAKU_BERSERK) && (p_ptr->lev > 34))) v = 0;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags3 & RF3_NO_STUN) v=0;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->stun = 0;
		return FALSE;
	}


	/* Knocked out */
	if (p_ptr->stun > 100)
	{
		old_aux = 3;
	}

	/* Heavy stun */
	else if (p_ptr->stun > 50)
	{
		old_aux = 2;
	}

	/* Stun */
	else if (p_ptr->stun > 0)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Knocked out */
	if (v > 100)
	{
		new_aux = 3;
	}

	/* Heavy stun */
	else if (v > 50)
	{
		new_aux = 2;
	}

	/* Stun */
	else if (v > 0)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		//v1.1.27 ���E�K�[�h
		if(v > 50)
		{
			set_mana_shield(FALSE,FALSE);
		}

		/* Describe the state */
		switch (new_aux)
		{
			/* Stun */
			case 1:
#ifdef JP
msg_print("�ӎ��������낤�Ƃ��Ă����B");
#else
			msg_print("You have been stunned.");
#endif

			break;

			/* Heavy stun */
			case 2:
#ifdef JP
msg_print("�ӎ����Ђǂ������낤�Ƃ��Ă����B");
#else
			msg_print("You have been heavily stunned.");
#endif

			break;

			/* Knocked out */
			case 3:
#ifdef JP
msg_print("�����N���N�����Ĉӎ������̂��Ă����B");
#else
			msg_print("You have been knocked out.");
#endif

			break;
		}

		if (randint1(1000) < v || one_in_(16))
		{
#ifdef JP
msg_print("�����悤�ȓ��ɂ�����B");
#else
			msg_print("A vicious blow hits your head.");
#endif

			if (one_in_(3))
			{
				if (!p_ptr->sustain_int) (void)do_dec_stat(A_INT);
				if (!p_ptr->sustain_wis) (void)do_dec_stat(A_WIS);
			}
			else if (one_in_(2))
			{
				if (!p_ptr->sustain_int) (void)do_dec_stat(A_INT);
			}
			else
			{
				if (!p_ptr->sustain_wis) (void)do_dec_stat(A_WIS);
			}
		}
		if (p_ptr->special_defense & KATA_MASK)
		{
#ifdef JP
			msg_print("�^�����ꂽ�B");
#else
			msg_print("Your posture gets loose.");
#endif
			p_ptr->special_defense &= ~(KATA_MASK);
			p_ptr->update |= (PU_BONUS);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->redraw |= (PR_STATE);
			p_ptr->redraw |= (PR_STATUS);
			p_ptr->action = ACTION_NONE;
		}

		/* Sniper */
		//if (p_ptr->concent) reset_concentration(TRUE);
		if(CLASS_USE_CONCENT) reset_concentration(TRUE); 

		/* Hex */
		if (hex_spelling_any()) stop_hex_spell_all();

		/* Notice */
		notice = TRUE;
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
#ifdef JP
msg_print("����ƞN�O��Ԃ���񕜂����B");
#else
			msg_print("You are no longer stunned.");
#endif

			if (disturb_state) disturb(0, 0);
			break;
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->stun = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the "stun" */
	p_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->cut", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_cut(int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;

	//v1.1.66 ���ւȂǂ͕ʊ֐��֔�΂��Ĕj������������
	if (RACE_BREAKABLE) return set_broken((v+1)/2);

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;
	///race �؂菝�ւ̑ϐ�
	if ((RACE_NO_CUT) && !p_ptr->mimic_form)
		v = 0;
	///mod140530 �X���C���▶�̂Ƃ��͐؂菝���󂯂Ȃ�
	if(p_ptr->mimic_form == MIMIC_SLIME || p_ptr->mimic_form == MIMIC_MIST) v=0;

	//v1.1.55
	if (p_ptr->special_defense & MUSOU_TENSEI)
	{
		p_ptr->cut = 0;
		return FALSE;
	}

	/* Mortal wound */
	if (p_ptr->cut > 1000)
	{
		old_aux = 7;
	}

	/* Deep gash */
	else if (p_ptr->cut > 200)
	{
		old_aux = 6;
	}

	/* Severe cut */
	else if (p_ptr->cut > 100)
	{
		old_aux = 5;
	}

	/* Nasty cut */
	else if (p_ptr->cut > 50)
	{
		old_aux = 4;
	}

	/* Bad cut */
	else if (p_ptr->cut > 25)
	{
		old_aux = 3;
	}

	/* Light cut */
	else if (p_ptr->cut > 10)
	{
		old_aux = 2;
	}

	/* Graze */
	else if (p_ptr->cut > 0)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Mortal wound */
	if (v > 1000)
	{
		new_aux = 7;
	}

	/* Deep gash */
	else if (v > 200)
	{
		new_aux = 6;
	}

	/* Severe cut */
	else if (v > 100)
	{
		new_aux = 5;
	}

	/* Nasty cut */
	else if (v > 50)
	{
		new_aux = 4;
	}

	/* Bad cut */
	else if (v > 25)
	{
		new_aux = 3;
	}

	/* Light cut */
	else if (v > 10)
	{
		new_aux = 2;
	}

	/* Graze */
	else if (v > 0)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Graze */
			case 1:
#ifdef JP
msg_print("�����菝�𕉂��Ă��܂����B");
#else
			msg_print("You have been given a graze.");
#endif

			break;

			/* Light cut */
			case 2:
#ifdef JP
msg_print("�y�����𕉂��Ă��܂����B");
#else
			msg_print("You have been given a light cut.");
#endif

			break;

			/* Bad cut */
			case 3:
#ifdef JP
msg_print("�Ђǂ����𕉂��Ă��܂����B");
#else
			msg_print("You have been given a bad cut.");
#endif

			break;

			/* Nasty cut */
			case 4:
#ifdef JP
msg_print("��ςȏ��𕉂��Ă��܂����B");
#else
			msg_print("You have been given a nasty cut.");
#endif

			break;

			/* Severe cut */
			case 5:
#ifdef JP
msg_print("�d��ȏ��𕉂��Ă��܂����B");
#else
			msg_print("You have been given a severe cut.");
#endif

			break;

			/* Deep gash */
			case 6:
#ifdef JP
msg_print("�Ђǂ��[��𕉂��Ă��܂����B");
#else
			msg_print("You have been given a deep gash.");
#endif

			break;

			/* Mortal wound */
			case 7:
#ifdef JP
msg_print("�v���I�ȏ��𕉂��Ă��܂����B");
#else
			msg_print("You have been given a mortal wound.");
#endif

			break;
		}

		/* Notice */
		notice = TRUE;

		if (randint1(1000) < v || one_in_(16))
		{
			if (!p_ptr->sustain_chr)
			{
#ifdef JP
msg_print("�Ђǂ����Ղ��c���Ă��܂����B");
#else
				msg_print("You have been horribly scarred.");
#endif


				do_dec_stat(A_CHR);
			}
		}
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
#ifdef JP
//msg_format("�����%s�B", p_ptr->prace == RACE_ANDROID ? "���䂪������" : "�o�����~�܂���");
msg_format("����Ɖ��䂪�������B");

#else
			msg_print("You are no longer bleeding.");
#endif

			if (disturb_state) disturb(0, 0);
			break;
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->cut = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the "cut" */
	p_ptr->redraw |= (PR_CUT);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}





//v1.1.66�@��Ԉُ�u�j���v�̏����B
//�d���l�`�E�S�[�����E���ւ݂̂��̏�ԂɂȂ�B
//�^�C���J�E���g�Ƃ��Ă����̎푰��race_multipur_val[0]���g�p����B
//���̊֐���set_stun()��set_cut()����Ă΂��B
//���̊֐��Ƃ͈Ⴂ�Av�̒l�ɐݒ肳���̂ł͂Ȃ�v�̒l�ɂ���ĉ����Z�����B
bool set_broken(int v)
{
	int old_val, new_val;
	int old_rank, new_rank;
	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	if (!RACE_BREAKABLE)
	{
		return FALSE;
	}


	old_val = p_ptr->race_multipur_val[0];

	new_val = old_val + v;

	if (new_val < 0) new_val = 0;
	if (new_val > BROKEN_MAX) new_val = BROKEN_MAX;

	///mod140530 �X���C���▶�̂Ƃ��͐؂菝���󂯂Ȃ�
	if (p_ptr->mimic_form == MIMIC_SLIME || p_ptr->mimic_form == MIMIC_MIST) new_val = 0;


	if (!old_val) old_rank = 0;
	else if (old_val < BROKEN_1) old_rank = 1;
	else if (old_val < BROKEN_2) old_rank = 2;
	else if (old_val < BROKEN_3) old_rank = 3;
	else 			    old_rank = 4;

	if (!new_val) new_rank = 0;
	else if (new_val < BROKEN_1) new_rank = 1;
	else if (new_val < BROKEN_2) new_rank = 2;
	else if (new_val < BROKEN_3) new_rank = 3;
	else 			    new_rank = 4;


	if (old_rank != new_rank)
	{
		switch (new_rank)
		{
		case 0://���
			msg_print("�[�̏C�������������B");
			break;
		case 1://��
			if (old_rank < new_rank)
				msg_print("���Ȃ��̔��ɑ傫�ȏ��������B");
			else
				msg_print("�[�̏C�����i��ł���B���Ƃ͂����炩�̏����c���݂̂��B");
			break;

		case 2://�T��
			if (old_rank < new_rank)
				msg_print("���Ȃ����[�ɋT�􂪑������I");
			else
				msg_print("�[�̌������ǂ������B�������܂��T�􂾂炯���B");
			break;

		case 3://�j��
			if (old_rank < new_rank)
				msg_print("���Ȃ����[�̈ꕔ���ӂ��Ă��܂����I");
			else
				msg_print("���̂̌����ǂ������B�������܂��l�����܂Ƃ��ɓ����Ȃ��B");
			break;

		default://��j
			msg_print("���Ȃ��̓��̂ɑ匊���J�����I");
			break;
		}

		notice = TRUE;

	}

	if (old_rank < new_rank && new_rank >= 2)
	{
		if (p_ptr->special_defense & KATA_MASK)
		{
			msg_print("�^�����ꂽ�B");
			p_ptr->special_defense &= ~(KATA_MASK);
			p_ptr->update |= (PU_BONUS);
			p_ptr->update |= (PU_MONSTERS);
			p_ptr->redraw |= (PR_STATE);
			p_ptr->redraw |= (PR_STATUS);
			p_ptr->action = ACTION_NONE;
		}
		if (CLASS_USE_CONCENT) reset_concentration(TRUE);



	}


	/* Use the value */
	p_ptr->race_multipur_val[0] = new_val;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	//�N�O�X�e�[�^�X�Ɠ����ꏊ�ɕ\������
	p_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


//v1.1.66 �C��
//�u�j���v��ԂɂȂ��������񕜂��悤�Ƃ���B
bool	self_repair(void)
{
	int repair_val, con_adj;

	if (!RACE_BREAKABLE)
	{
		msg_print("ERROR:�z�肳��Ȃ��푰��self_repair()���Ă΂ꂽ");
		return FALSE;
	}
	if (!REF_BROKEN_VAL)
	{
		msg_print("�C���̕K�v�͂Ȃ��B");
		return FALSE;

	}
	msg_print("�[���C�����Ă���...");

	//�ϋv10:1 20:3 30:6 40:10 50:16
	con_adj = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

	repair_val = randint1(con_adj * 4) + con_adj * 3;

	set_broken(-(repair_val));

	return TRUE;

}



/*
 * Set "p_ptr->food", notice observable changes
 *
 * The "p_ptr->food" variable can get as large as 20000, allowing the
 * addition of the most "filling" item, Elvish Waybread, which adds
 * 7500 food units, without overflowing the 32767 maximum limit.
 *
 * Perhaps we should disturb the player with various messages,
 * especially messages about hunger status changes.  XXX XXX XXX
 *
 * Digestion of food is handled in "dungeon.c", in which, normally,
 * the player digests about 20 food units per 100 game turns, more
 * when "fast", more when "regenerating", less with "slow digestion",
 * but when the player is "gorged", he digests 100 food units per 10
 * game turns, or a full 1000 food units per 100 game turns.
 *
 * Note that the player's speed is reduced by 10 units while gorged,
 * so if the player eats a single food ration (5000 food units) when
 * full (15000 food units), he will be gorged for (5000/100)*10 = 500
 * game turns, or 500/(100/5) = 25 player turns (if nothing else is
 * affecting the player speed).
 */
/*:::p_ptr->food�̖����x��v�ɕύX���A�󕠏�Ԃ̑J�ڂŃ��b�Z�[�W�\���B
 *:::�ő�l��20000�ł���B10000�Ŗ����ƂȂ�15000�ȏ�͐H�ׂ����ɂȂ�B
 *:::2000�ȉ��͋󕠁A1000�ȉ��͐���A500�ȉ��͋Q��(�ԕ�������)
 *:::100�Q�[���^�[�����Ƃ�20�������B�H�ׂ����Ȃ�1000�������B
 *:::�H�׉߂��̂Ƃ�����-10*/
/*:::�ǂ̎푰���ǂ�ȐH���łǂꂭ�疞���x�������邩�A�Ȃǂ̏����͕ʉӏ�*/

bool set_food(int v)
{
	int old_aux, new_aux;

	bool notice = FALSE;

	//v2.0.6
	//�ޖ����ꏈ��
	//�H�׉߂��ɂȂ�Ȃ��B
	//�����x��13000*10�܂ŃX�g�b�N�ł���B
	//15000�𒴂�����X�g�b�N��1���₵��13000�}�C�i�X���A
	//2000�����������X�g�b�N��1���炵��13000�v���X����B
	//�X�g�b�N��p_ptr->magic_num2[200]�ɋL�^����B
	if ( p_ptr->pclass == CLASS_YUMA)
	{
		//int max_stock = 3 + p_ptr->lev / 7;
		int max_stock = 10;
		int stock = p_ptr->magic_num2[200];

		//�h�������ʔ����Ȃǂ́u�����ɂȂ�v����������ɓ����Ȃ��Ȃ�̂ŁA
		//���̒l���Z�b�g���ČĂ΂ꂽ�Ƃ��͌��ݖ����x+10000�ƌ��Ȃ��B
		if (v == PY_FOOD_MAX - 1)  v = p_ptr->food + 10000;

		if (stock == max_stock && v >= PY_FOOD_MAX)
		{
			v = PY_FOOD_MAX - 1;
		}
		else if (stock < max_stock && v >= PY_FOOD_MAX)
		{
			v -= 13000;
			if (v >= PY_FOOD_MAX) v = PY_FOOD_MAX - 1;
			stock++;
			notice = TRUE;
		}
		else if (stock > 0 && v < PY_FOOD_ALERT)
		{
			v += 13000;
			stock--;
			notice = TRUE;
		}
		else if (stock == 0 && v < 0)
		{
			v = 0;
		}

		p_ptr->magic_num2[200] = stock;
	}
	else
	{

		/* Hack -- Force good values */
		v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	}

	///mod140113 ���@�g���͋󕠂ɂȂ�Ȃ��悤�ɂ���
	if(prace_is_(RACE_MAGICIAN) && v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;
	if(prace_is_(RACE_ULTIMATE) && v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;
	if(prace_is_(RACE_MAGIC_JIZO) && v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;

	//v1.1.66 ���ւ͋󕠂ɂ������ɂ��Ȃ�Ȃ�
	if (prace_is_(RACE_HANIWA))
	{
		if (v < PY_FOOD_ALERT) v = PY_FOOD_ALERT;
		if (v >= PY_FOOD_FULL) v = PY_FOOD_FULL - 1;
	}

	//�H���l�͋󕠂ł��܂萊�サ�Ȃ�
	if(p_ptr->prace == RACE_HOURAI && v < PY_FOOD_FAINT) v = PY_FOOD_FAINT;

	//���P�ƃ��f�B�X���͐H�׉߂��ɂȂ�Ȃ�
	if(p_ptr->pclass == CLASS_KOGASA && v > PY_FOOD_MAX-1) v = PY_FOOD_MAX-1;
	if(p_ptr->pclass == CLASS_MEDICINE && v > PY_FOOD_MAX-1) v = PY_FOOD_MAX-1;
	//�H�X�q�͖����x3000�ȏ�ɂȂ�Ȃ�
	if(p_ptr->pclass == CLASS_YUYUKO && v > 3000) v = 3000;

	/* Fainting / Starving */
	if (p_ptr->food < PY_FOOD_FAINT)
	{
		old_aux = 0;
	}

	/* Weak */
	else if (p_ptr->food < PY_FOOD_WEAK)
	{
		old_aux = 1;
	}

	/* Hungry */
	else if (p_ptr->food < PY_FOOD_ALERT)
	{
		old_aux = 2;
	}

	/* Normal */
	//v2.0.6 �ޖ��͒ʏ�̖����ȏ�ɂȂ�Ȃ�
	else if (p_ptr->food < PY_FOOD_FULL || p_ptr->pclass == CLASS_YUMA)
	{
		old_aux = 3;
	}

	/* Full */
	else if (p_ptr->food < PY_FOOD_MAX)
	{
		old_aux = 4;
	}

	/* Gorged */
	else
	{
		old_aux = 5;
	}




	/* Fainting / Starving */
	if (v < PY_FOOD_FAINT)
	{
		new_aux = 0;
	}

	/* Weak */
	else if (v < PY_FOOD_WEAK)
	{
		new_aux = 1;
	}

	/* Hungry */
	else if (v < PY_FOOD_ALERT)
	{
		new_aux = 2;
	}

	/* Normal */
	//v2.0.6 �ޖ��͒ʏ�̖����ȏ�ɂȂ�Ȃ�
	else if (v < PY_FOOD_FULL || p_ptr->pclass == CLASS_YUMA)
	{
		new_aux = 3;
	}

	/* Full */
	else if (v < PY_FOOD_MAX)
	{
		new_aux = 4;
	}

	/* Gorged */
	else
	{
		new_aux = 5;
	}

	///del131221 virtue
	/*
	if (old_aux < 1 && new_aux > 0)
		chg_virtue(V_PATIENCE, 2);
	else if (old_aux < 3 && (old_aux != new_aux))
		chg_virtue(V_PATIENCE, 1);
	if (old_aux == 2)
		chg_virtue(V_TEMPERANCE, 1);
	if (old_aux == 0)
		chg_virtue(V_TEMPERANCE, -1);
	*/
	/* Food increase */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Weak */
			case 1:
#ifdef JP
msg_print("�܂��󕠂œ|�ꂻ�����B");
#else
			msg_print("You are still weak.");
#endif

			break;

			/* Hungry */
			case 2:
#ifdef JP
msg_print("�܂��󕠂��B");
#else
			msg_print("You are still hungry.");
#endif

			break;

			/* Normal */
			case 3:
#ifdef JP
msg_print("�󕠊��������܂����B");
#else
			msg_print("You are no longer hungry.");
#endif

			break;

			/* Full */
			case 4:
#ifdef JP
msg_print("�������I");
#else
			msg_print("You are full!");
#endif

			break;

			/* Bloated */
			case 5:
#ifdef JP
msg_print("�H�׉߂����I");
#else
			msg_print("You have gorged yourself!");
#endif
			///del131221 virtue
			//chg_virtue(V_HARMONY, -1);
			//chg_virtue(V_PATIENCE, -1);
			//chg_virtue(V_TEMPERANCE, -2);

			break;
		}

		/* Change */
		notice = TRUE;
	}

	/* Food decrease */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Fainting / Starving */
			case 0:
#ifdef JP
msg_print("���܂�ɂ��󕠂ŋC�������Ă��܂����I");
#else
			msg_print("You are getting faint from hunger!");
#endif

			break;

			/* Weak */
			case 1:
#ifdef JP
msg_print("�������󂢂ē|�ꂻ�����B");
#else
			msg_print("You are getting weak from hunger!");
#endif

			break;

			/* Hungry */
			case 2:
#ifdef JP
msg_print("�������󂢂Ă����B");
#else
			msg_print("You are getting hungry.");
#endif

			break;

			/* Normal */
			case 3:
#ifdef JP
msg_print("���������Ȃ��Ȃ����B");
#else
			msg_print("You are no longer full.");
#endif

			break;

			/* Full */
			case 4:
#ifdef JP
msg_print("����Ƃ����������Ȃ��Ȃ����B");
#else
			msg_print("You are no longer gorged.");
#endif

			break;
		}

		if (p_ptr->wild_mode && (new_aux < 2))
		{
			change_wild_mode();
		}

		/* Change */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->food = v;


	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw hunger */
	p_ptr->redraw |= (PR_HUNGER);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

///mod140326 �D���x
/*:::�D���x�̃Z�b�g*/
bool set_alcohol(int v)
{
	int old_aux, new_aux;

	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	//�E�V�͓��ʎ��ɋ���
	if(p_ptr->pclass == CLASS_YUGI && v >= DRANK_4) v = DRANK_4 - 1;
	//��_�����ɋ���
	if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[3] == 53 && v >= DRANK_4)		v = DRANK_4 - 1;

	//�i�Ԃ͂���Ɏ��ɋ���
	if(p_ptr->pclass == CLASS_EIRIN && v >= DRANK_2) v = DRANK_2 - 1;
	//�ޖ���
	if (p_ptr->pclass == CLASS_YUMA && v >= DRANK_2) v = DRANK_2 - 1;

	//���i���C��
	if(p_ptr->pseikaku == SEIKAKU_BERSERK && v >= DRANK_4) v = DRANK_4 - 1;

	if(CHECK_FAIRY_TYPE == 45 && v >= DRANK_4) v = DRANK_4 - 1;

	if (p_ptr->alcohol == DRANK_0) old_aux = 0;
	else if (p_ptr->alcohol < DRANK_1) old_aux = 1;
	else if (p_ptr->alcohol < DRANK_2) old_aux = 2;
	else if (p_ptr->alcohol < DRANK_3) old_aux = 3;
	else if (p_ptr->alcohol < DRANK_4) old_aux = 4;
	else 	old_aux = 5;

	if (v == DRANK_0) new_aux = 0;
	else if (v < DRANK_1) new_aux = 1;
	else if (v < DRANK_2) new_aux = 2;
	else if (v < DRANK_3) new_aux = 3;
	else if (v < DRANK_4) new_aux = 4;
	else 	new_aux = 5;


	/* Food increase */
	if (new_aux != old_aux)
	{
		if(p_ptr->muta2 & MUT2_ALCOHOL)
		{
			if(new_aux == 0) msg_print("�����؂�Ă��܂����B�̂��������Ƃ𕷂��Ȃ��E�E");
			else if(new_aux == 1) msg_print("�Ȃ񂾂���������Ȃ��E�E");
			else if(new_aux == 2) msg_print("�����C�������I");
			else if(new_aux == 3) msg_print("�ō��̋C�����I");
			else  msg_print("��ނȂ��ō��̋C�����I");
		}
		else 
		{
			if(new_aux == 0) msg_print("���������߂��B");
			else if(new_aux == 1) msg_print("�����C���������B");
			else if(new_aux == 2) msg_print("�����C�������I");
			else if(new_aux == 3) msg_print("�ō��̋C�����I");
			else if(new_aux == 4) msg_print("���݂����ċC�����������E�E");
			else
			{
				msg_print("���Ȃ��͂��̏�ɓ|�ꂽ�B");
				//v1.1.27 ���E�K�[�h
				set_mana_shield(FALSE,FALSE);
			}
		}
		/* Change */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->alcohol = v;

	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	//v1.1.78 �����t���O����
	check_suiken();

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Redraw hunger */
/*:::Hack ���݉߂��ē|�ꂽ�Ƃ��݈̂ӎ��s���Ă�K�p����*/
	p_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Increases a stat by one randomized level             -RAK-
 *
 * Note that this function (used by stat potions) now restores
 * the stat BEFORE increasing it.
 */
/*:::��{�p�����[�^�㏸*/
///mod140104 �p�����[�^����ƕ\�L��ύX
bool inc_stat(int stat)
{
	int value, gain;

	/* Then augment the current/max stat */
	value = p_ptr->stat_cur[stat];

	/* Cannot go above 18/100 */
	if (value < p_ptr->stat_max_max[stat])
	{

		if(value > 27) gain = 1;
		else 
		{
			if(weird_luck())gain = 3;
			else if(one_in_(4)) gain = 2;
			else gain = 1;
		}
		value += gain;
		if(value > p_ptr->stat_max_max[stat]) value = p_ptr->stat_max_max[stat];
	
#if 0
		/* Gain one (sometimes two) points */
		if (value < 18)
		{
			gain = ((randint0(100) < 75) ? 1 : 2);
			value += gain;
		}

		/* Gain 1/6 to 1/3 of distance to 18/100 */
		else if (value < (p_ptr->stat_max_max[stat]-2))
		{
			/* Approximate gain value */
			gain = (((p_ptr->stat_max_max[stat]) - value) / 2 + 3) / 2;

			/* Paranoia */
			if (gain < 1) gain = 1;

			/* Apply the bonus */
			value += randint1(gain) + gain / 2;

			/* Maximal value */
			if (value > (p_ptr->stat_max_max[stat]-1)) value = p_ptr->stat_max_max[stat]-1;
		}

		/* Gain one point at a time */
		else
		{
			value++;
		}
#endif
		/* Save the new value */
		p_ptr->stat_cur[stat] = value;

		/* Bring up the maximum too */
		if (value > p_ptr->stat_max[stat])
		{
			p_ptr->stat_max[stat] = value;
		}

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Success */
		return (TRUE);
	}

	/* Nothing to gain */
	return (FALSE);
}



/*
 * Decreases a stat by an amount indended to vary from 0 to 100 percent.
 *
 * Amount could be a little higher in extreme cases to mangle very high
 * stats from massive assaults.  -CWS
 *
 * Note that "permanent" means that the *given* amount is permanent,
 * not that the new value becomes permanent.  This may not work exactly
 * as expected, due to "weirdness" in the algorithm, but in general,
 * if your stat is already drained, the "max" value will not drop all
 * the way down to the "cur" value.
 */
/*:::�\�͒l��������Bamount�͓G�̔\�͌����Ō���10,�ق��l�X�Ȃ��Ƃ�11�`30���x�@�M�����������Ƃ�50*/
///mod140104 �p�����[�^����ƕ\�L��ύX�@18�ȏ�̂Ƃ�����������₷���Ȃ��Ă���
bool dec_stat(int stat, int amount, int permanent)
{
	int cur, max, same, res = FALSE;
	bool flag_dec = FALSE;
	/* Acquire current value */
	cur = p_ptr->stat_cur[stat];
	max = p_ptr->stat_max[stat];
	/* Note when the values are identical */
	same = (cur == max);

	if (cur > 3)
	{
		if (amount > 90) cur--;
		if (amount > 50) cur--;
		if (amount > 20) cur--;
		if (amount > 10) cur--;
		else if(one_in_(2))
		{
			cur--;
			flag_dec = TRUE;
		}

		if (cur < 3) cur = 3;
		if (cur != p_ptr->stat_cur[stat]) res = TRUE;
	}
	if (permanent && (max > 3))
	{
		if (amount > 90) max--;
		if (amount > 50) max--;
		if (amount > 20) max--;
		if (amount > 10) max--;
		else if(flag_dec) max--;

		if (same || (max < cur)) max = cur;
		if (max != p_ptr->stat_max[stat]) res = TRUE;
	}
#if 0
	/* Damage "current" value */
	if (cur > 3)
	{
		/* Handle "low" values */
		if (cur <= 18)
		{
			if (amount > 90) cur--;
			if (amount > 50) cur--;
			if (amount > 20) cur--;
			cur--;
		}

		/* Handle "high" values */
		else
		{
			/* Hack -- Decrement by a random amount between one-quarter */
			/* and one-half of the stat bonus times the percentage, with a */
			/* minimum damage of half the percentage. -CWS */
			loss = (((cur-18) / 2 + 1) / 2 + 1);

			/* Paranoia */
			if (loss < 1) loss = 1;

			/* Randomize the loss */
			loss = ((randint1(loss) + loss) * amount) / 100;

			/* Maximal loss */
			if (loss < amount/2) loss = amount/2;

			/* Lose some points */
			cur = cur - loss;

			/* Hack -- Only reduce stat to 17 sometimes */
			if (cur < 18) cur = (amount <= 20) ? 18 : 17;
		}

		/* Prevent illegal values */
		if (cur < 3) cur = 3;

		/* Something happened */
		if (cur != p_ptr->stat_cur[stat]) res = TRUE;
	}

	/* Damage "max" value */
	if (permanent && (max > 3))
	{
		chg_virtue(V_SACRIFICE, 1);
		if (stat == A_WIS || stat == A_INT)
			chg_virtue(V_ENLIGHTEN, -2);

		/* Handle "low" values */
		if (max <= 18)
		{
			if (amount > 90) max--;
			if (amount > 50) max--;
			if (amount > 20) max--;
			max--;
		}

		/* Handle "high" values */
		else
		{
			/* Hack -- Decrement by a random amount between one-quarter */
			/* and one-half of the stat bonus times the percentage, with a */
			/* minimum damage of half the percentage. -CWS */
			loss = (((max-18) / 2 + 1) / 2 + 1);
			loss = ((randint1(loss) + loss) * amount) / 100;
			if (loss < amount/2) loss = amount/2;

			/* Lose some points */
			max = max - loss;

			/* Hack -- Only reduce stat to 17 sometimes */
			if (max < 18) max = (amount <= 20) ? 18 : 17;
		}

		/* Hack -- keep it clean */
		if (same || (max < cur)) max = cur;

		/* Something happened */
		if (max != p_ptr->stat_max[stat]) res = TRUE;
	}
#endif
	/* Apply changes */
	if (res)
	{
		/* Actually set the stat to its new value. */
		p_ptr->stat_cur[stat] = cur;
		p_ptr->stat_max[stat] = max;

		/* Redisplay the stats later */
		p_ptr->redraw |= (PR_STATS);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);
	}

	/* Done */
	return (res);
}


/*
 * Restore a stat.  Return TRUE only if this actually makes a difference.
 */
bool res_stat(int stat)
{
	/* Restore if needed */
	if (p_ptr->stat_cur[stat] != p_ptr->stat_max[stat])
	{
		/* Restore */
		p_ptr->stat_cur[stat] = p_ptr->stat_max[stat];

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Redisplay the stats later */
		p_ptr->redraw |= (PR_STATS);

		/* Success */
		return (TRUE);
	}

	/* Nothing to restore */
	return (FALSE);
}


/*
 * Increase players hit points, notice effects
 */
/*:::HP��num�񕜂�����B�ő�͒����Ȃ��B*/
bool hp_player(int num)
{
	//int vir;
	/*:::�u���v�̓��ɂ���Ă�HP�񕜗ʂ��㉺����炵��*/
	/*
	vir = virtue_number(V_VITALITY);
	if (vir)
	{
		num = num * (p_ptr->virtues[vir - 1] + 1250) / 1250;
	}
	*/
	/* Healing needed */
	if (p_ptr->chp < p_ptr->mhp)
	{
		if ((num > 0) && (p_ptr->chp < (p_ptr->mhp/3)))
			chg_virtue(V_TEMPERANCE, 1);
		/* Gain hitpoints */
		p_ptr->chp += num;

		/* Enforce maximum */
		if (p_ptr->chp >= p_ptr->mhp)
		{
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
		}

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		//�p���X�B��HP�����{�[�i�X
		if(p_ptr->pclass == CLASS_PARSEE) p_ptr->update |= PU_BONUS;

		/* Heal 0-4 */
		if (num < 5)
		{
#ifdef JP
msg_print("�����C�����ǂ��Ȃ����B");
#else
			msg_print("You feel a little better.");
#endif

		}

		/* Heal 5-14 */
		else if (num < 15)
		{
#ifdef JP
msg_print("�C�����ǂ��Ȃ����B");
#else
			msg_print("You feel better.");
#endif

		}

		/* Heal 15-34 */
		else if (num < 35)
		{
#ifdef JP
msg_print("�ƂĂ��C�����ǂ��Ȃ����B");
#else
			msg_print("You feel much better.");
#endif

		}

		/* Heal 35+ */
		else
		{
#ifdef JP
msg_print("�Ђ��傤�ɋC�����ǂ��Ȃ����B");
#else
			msg_print("You feel very good.");
#endif

		}

		/* Notice */
		return (TRUE);
	}

	/* Ignore */
	return (FALSE);
}


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_pos[] =
{
#ifdef JP
"����",
#else
	"strong",
#endif

#ifdef JP
"�m�I��",
#else
	"smart",
#endif

#ifdef JP
"����",
#else
	"wise",
#endif

#ifdef JP
"��p��",
#else
	"dextrous",
#endif

#ifdef JP
"���N��",
#else
	"healthy",
#endif

#ifdef JP
"������"
#else
	"cute"
#endif

};


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_neg[] =
{
#ifdef JP
"�キ",
"���m��",
"������",
"�s��p��",
"�s���N��",
"�X��"
#else
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"ugly"
#endif

};


/*
 * Lose a "point"
 */
bool do_dec_stat(int stat)
{
	bool sust = FALSE;

	/* Access the "sustain" */
	switch (stat)
	{
		case A_STR: if (p_ptr->sustain_str) sust = TRUE; break;
		case A_INT: if (p_ptr->sustain_int) sust = TRUE; break;
		case A_WIS: if (p_ptr->sustain_wis) sust = TRUE; break;
		case A_DEX: if (p_ptr->sustain_dex) sust = TRUE; break;
		case A_CON: if (p_ptr->sustain_con) sust = TRUE; break;
		case A_CHR: if (p_ptr->sustain_chr) sust = TRUE; break;
	}

	/* Sustain */
	if (sust && (!ironman_nightmare || randint0(13)))
	{
		/* Message */
#ifdef JP
msg_format("%s�Ȃ����C���������A�����Ɍ��ɖ߂����B",
#else
		msg_format("You feel %s for a moment, but the feeling passes.",
#endif

				desc_stat_neg[stat]);

		/* Notice effect */
		return (TRUE);
	}

	/* Attempt to reduce the stat */
	if (dec_stat(stat, 10, (ironman_nightmare && !randint0(13))))
	{
		/* Message */
#ifdef JP
msg_format("�Ђǂ�%s�Ȃ����C������B", desc_stat_neg[stat]);
#else
		msg_format("You feel very %s.", desc_stat_neg[stat]);
#endif


		/* Notice effect */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restore lost "points" in a stat
 */
/*:::�p�����[�^���񕜂���*/
bool do_res_stat(int stat)
{
	/* Attempt to increase */
	if (res_stat(stat))
	{
		/* Message */
#ifdef JP
msg_format("���ʂ��%s�Ȃ����C������B", desc_stat_pos[stat]);
#else
		msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Gain a "point" in a stat
 */
/*:::�w�肵���p�����[�^���㏸����@���x���A�b�v���Ȃ�*/
bool do_inc_stat(int stat)
{
	bool res;

	/* Restore strength */
	res = res_stat(stat);

	/* Attempt to increase */
	if (inc_stat(stat))
	{
		///sysdel virtue
		if (stat == A_WIS)
		{
			chg_virtue(V_ENLIGHTEN, 1);
			chg_virtue(V_FAITH, 1);
		}
		else if (stat == A_INT)
		{
			chg_virtue(V_KNOWLEDGE, 1);
			chg_virtue(V_ENLIGHTEN, 1);
		}
		else if (stat == A_CON)
			chg_virtue(V_VITALITY, 1);

		/* Message */
#ifdef JP
msg_format("���[�I�I�ƂĂ�%s�Ȃ����I", desc_stat_pos[stat]);
#else
		msg_format("Wow!  You feel very %s!", desc_stat_pos[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Restoration worked */
	if (res)
	{
		/* Message */
#ifdef JP
msg_format("���ʂ��%s�Ȃ����C������B", desc_stat_pos[stat]);
#else
		msg_format("You feel less %s.", desc_stat_neg[stat]);
#endif


		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restores any drained experience
 */
bool restore_level(void)
{
	/* Restore experience */
	if (p_ptr->exp < p_ptr->max_exp)
	{
		/* Message */
#ifdef JP
msg_print("�����͂��߂��Ă����C������B");
#else
		msg_print("You feel your life energies returning.");
#endif


		/* Restore the experience */
		p_ptr->exp = p_ptr->max_exp;

		/* Check the experience */
		check_experience();

		/* Did something */
		return (TRUE);
	}

	/* No effect */
	return (FALSE);
}


/*
 * Forget everything
 */
/*:::�L���r���@�ڍז���*/
bool lose_all_info(void)
{
	int i;

	if(p_ptr->pclass == CLASS_KEINE)
	{
		msg_print("����̌��͋L���r���ɒ�R�����I");
		return FALSE;
	}

	//chg_virtue(V_KNOWLEDGE, -5);
	//chg_virtue(V_ENLIGHTEN, -5);

	/* Forget info about objects */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Allow "protection" by the MENTAL flag */
		if (o_ptr->ident & (IDENT_MENTAL)) continue;

		/* Remove "default inscriptions" */
		o_ptr->feeling = FEEL_NONE;

		/* Hack -- Clear the "empty" flag */
		o_ptr->ident &= ~(IDENT_EMPTY);

		/* Hack -- Clear the "known" flag */
		o_ptr->ident &= ~(IDENT_KNOWN);

		/* Hack -- Clear the "felt" flag */
		o_ptr->ident &= ~(IDENT_SENSE);
	}
	///mod141101 �L���������󂯂�Ƃ��Ƃ�Ƌ��q�͋Z��Y���
	if(p_ptr->pclass == CLASS_SATORI) make_magic_list_satori(TRUE);
	kyouko_echo(TRUE,0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Mega-Hack -- Forget the map */
	wiz_dark();

	/* It worked */
	return (TRUE);
}


void do_poly_wounds(void)
{
	/* Changed to always provide at least _some_ healing */
	s16b wounds = p_ptr->cut;
	s16b hit_p = (p_ptr->mhp - p_ptr->chp);
	s16b change = damroll(p_ptr->lev, 5);
	bool Nasty_effect = one_in_(5);

	if (!(wounds || hit_p || Nasty_effect)) return;

#ifdef JP
msg_print("�������y�����̂ɕω������B");
#else
	msg_print("Your wounds are polymorphed into less serious ones.");
#endif

	hp_player(change);
	if (Nasty_effect)
	{
#ifdef JP
msg_print("�V���ȏ����ł����I");
take_hit(DAMAGE_LOSELIFE, change / 2, "�ω�������", -1);
#else
		msg_print("A new wound was created!");
		take_hit(DAMAGE_LOSELIFE, change / 2, "a polymorphed wound", -1);
#endif

		set_cut(change);
	}
	else
	{
		set_cut(p_ptr->cut - (change / 2));
	}
}


/*
 * Change player race
 */
/*:::�푰�ϗe*/
void change_race(int new_race, cptr effect_msg)
{
	cptr title = race_info[new_race].title;
	int  old_race = p_ptr->prace;
	int i;

	if((RACE_NEVER_CHANGE) || new_race == RACE_ANDROID || new_race == RACE_STRAYGOD || new_race == RACE_DEITY)
	{
		msg_print("ERROR:�Ӑ}���Ȃ��푰��change_race���Ă΂ꂽ");
		return;
	}

	if(new_race == RACE_ZASHIKIWARASHI)
	{
		msg_print("ERROR:���푰�����~��炵�ɕω����邱�Ƃ͂ł��Ȃ�");
		return;
	}



	//v1.1.69 �푰�ɂ�����炸�N�O�؂菝�j�������邱�Ƃɂ����Bv1.1.66�ł̋L�q���C��
	if (RACE_BREAKABLE)
	{
		set_broken(-(BROKEN_MAX));
	}
	else
	{
		set_cut(0);
		set_stun(0);
	}


#ifdef JP
	if(new_race == RACE_ULTIMATE)
		msg_format("�w���ɂ̐�����%s�̒a�����b�[���I�x", player_name);
	else if(new_race == RACE_LICH)
		msg_format("���Ȃ��͖{�����ɂ��Ă��̐����畑���߂����I");
	else 
		msg_format("���Ȃ���%s%s�ɕω������I", effect_msg, title);
#else
	msg_format("You turn into %s %s%s!", (!effect_msg[0] && is_a_vowel(title[0]) ? "an" : "a"), effect_msg, title);
#endif

	///sysdel virtue
	//chg_virtue(V_CHANCE, 2);

	if (p_ptr->prace < 32)
	{
		p_ptr->old_race1 |= 1L << p_ptr->prace;
	}
	else
	{
		p_ptr->old_race2 |= 1L << (p_ptr->prace-32);
	}
	p_ptr->prace = new_race;
	rp_ptr = &race_info[p_ptr->prace];

	/* Experience factor */
	p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;

	if(EXTRA_MODE) p_ptr->expfact /= 2;

	/*
	 * The speed bonus of Klackons and Sprites are disabled
	 * and the experience penalty is decreased.
	 */
	///race class �푰�ϗe���̉����푰�̏C�s�mEXP�{�[�i�X�֘A
	//if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_NINJA)) && ((p_ptr->prace == RACE_KLACKON) || (p_ptr->prace == RACE_SPRITE)))
	//	p_ptr->expfact -= 15;

	/* Get character's height and weight */
	/*:::�g���̏d�ύX*/
	///sysdel �g���̏d
	get_height_weight();

	/* Hitdice */
	///sys class �푰�ϗe����HD�v�Z �X�y�}�X���ꏈ��
	//v1.1.32 �p�`�����[���ꏈ��
	if (p_ptr->pclass == CLASS_SORCERER || is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
		p_ptr->hitdie = rp_ptr->r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
	else
		p_ptr->hitdie = rp_ptr->r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;

	do_cmd_rerate(FALSE);
	//�����^�ψق��Ď擾�@���̕ψق͊�{���ׂď�����
	gain_perma_mutation();

	//v1.1.66 �S�[��������ւɂȂ����Ƃ��N�O�Ɛ؂菝�������B
	//�����̎푰�ł͂���set_cut��set_stun�o�R�ł͒l�ɃA�N�Z�X���Ȃ��Ȃ�̂Œ��ڒl��ҏW����B
	//v1.1.69 �@�L�q���ԈႦ�Ă����̂ŏC�����ď�Ɉړ�

	/*:::�푰�p����l���N���A*/
	for(i=0;i<5;i++) p_ptr->race_multipur_val[i] = 0;

	//�d���ɂȂ����Ƃ����Z�K���@�N�C�b�N�X�^�[�g�p�ۑ������͕s�v
	if(new_race == RACE_FAIRY) 
		get_fairy_type(FALSE);

	//v1.1.70 ������ɂȂ����Ƃ��������́u�Ȃ��v�ɐݒ�
	if (p_ptr->prace == RACE_ANIMAL_GHOST) 
		p_ptr->race_multipur_val[0] = ANIMAL_GHOST_STRIFE_NONE;

	/* The experience level may be modified */
	check_experience();

	///mod140619
	for(i=INVEN_RARM;i<INVEN_TOTAL;i++)
	{
		if(!inventory[i].k_idx) continue;
		if(!wield_check(i,i))
		{
			object_type forge, *q_ptr = &forge;
			char obj_name[160];
			object_desc(obj_name, &inventory[i], 0);
			object_copy(q_ptr, &inventory[i]);

			msg_format("%s���O��Ă��܂����I",obj_name);
			inven_item_increase(i, -1);
			inven_item_optimize(i);
			drop_near(q_ptr,0,py,px);
			kamaenaoshi(i);		
			calc_android_exp();
			p_ptr->redraw |= (PR_EQUIPPY);		
		}
	}


	p_ptr->redraw |= (PR_BASIC);

	p_ptr->update |= (PU_BONUS);

	handle_stuff();

	/* Load an autopick preference file */
	if (old_race != p_ptr->prace) autopick_load_pref(FALSE);

	/* Player's graphic tile may change */
	lite_spot(py, px);
}

///sys race muta ���ȕϗe�@���ʂƎ푰�ύX�Ɋւ��đ��삷��\��
///mod140224 ���ȕϗe�@�푰���ʕω��͂Ƃ肠����������
void do_poly_self(void)
{
	int power = p_ptr->lev;

#ifdef JP
msg_print("���Ȃ��͕ω��̖K���������...");
#else
	msg_print("You feel a change coming over you...");
#endif
	///del131214 virtue
	//chg_virtue(V_CHANCE, 1);

	///mod140224 ���j�[�N�v���[���[�͎푰�A���ʕϗe���Ȃ��悤�ɂ���
	if ((power > randint0(20)) && one_in_(3) && (p_ptr->prace != RACE_ANDROID) && !(cp_ptr->flag_only_unique))
	{
		char effect_msg[80] = "";
		int new_race;

		/* Some form of racial polymorph... */
		power -= 10;

		if ((power > randint0(5)) && one_in_(4))
		{
			/* sex change */
			power -= 2;

			if (p_ptr->psex == SEX_MALE)
			{
				p_ptr->psex = SEX_FEMALE;
				sp_ptr = &sex_info[p_ptr->psex];
#ifdef JP
sprintf(effect_msg, "������");
#else
				sprintf(effect_msg, "female ");
#endif

			}
			else
			{
				p_ptr->psex = SEX_MALE;
				sp_ptr = &sex_info[p_ptr->psex];
#ifdef JP
sprintf(effect_msg, "�j����");
#else
				sprintf(effect_msg, "male ");
#endif

			}
		}

		if ((power > randint0(30)) && one_in_(5))
		{
			int tmp = 0;

			/* Harmful deformity */
			power -= 15;

			while (tmp < 6)
			{
				if (one_in_(2))
				{
					(void)dec_stat(tmp, randint1(6) + 6, one_in_(3));
					power -= 1;
				}
				tmp++;
			}

			/* Deformities are discriminated against! */
			(void)dec_stat(A_CHR, randint1(6), TRUE);

			if (effect_msg[0])
			{
				char tmp_msg[10];
#ifdef JP
				sprintf(tmp_msg,"%s",effect_msg);
				sprintf(effect_msg,"��`��%s",tmp_msg);
#else
				sprintf(tmp_msg,"%s ",effect_msg);
				sprintf(effect_msg,"deformed %s ",tmp_msg);
#endif

			}
			else
			{
#ifdef JP
				sprintf(effect_msg,"��`��");
#else
				sprintf(effect_msg,"deformed ");
#endif

			}
		}

		while ((power > randint0(20)) && one_in_(10))
		{
			/* Polymorph into a less mutated form */
			power -= 10;

			if (!lose_mutation(0))
#ifdef JP
msg_print("��Ȃ��炢���ʂɂȂ����C������B");
#else
				msg_print("You feel oddly normal.");
#endif

		}

		//v1.1.79 �V�푰�u���~��炵�v�ɂȂ邱�Ƃ͂ł��Ȃ�
		do
		{
			new_race = randint0(MAX_RACES);
		}
		while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ZASHIKIWARASHI)
			|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special
			|| new_race == RACE_ULTIMATE && !(weird_luck()));

		change_race(new_race, effect_msg);
	}
	if ((power > randint0(30)) && one_in_(6))
	{
		int tmp = 0;

		/* Abomination! */
		power -= 20;

#ifdef JP
//msg_format("%s�̍\�����ω������I", p_ptr->prace == RACE_ANDROID ? "�@�B" : "����");
msg_format("�̓��̍\�����ω������I");
#else
		msg_print("Your internal organs are rearranged!");
#endif

		while (tmp < 6)
		{
			(void)dec_stat(tmp, randint1(6) + 6, one_in_(3));
			tmp++;
		}
		if (one_in_(6))
		{
#ifdef JP
			msg_print("���݂̎p�Ő����Ă����͍̂���Ȃ悤���I");
			take_hit(DAMAGE_LOSELIFE, damroll(randint1(10), p_ptr->lev), "���ɍ���Ȃ��ˑR�ψ�", -1);
#else
			msg_print("You find living difficult in your present form!");
			take_hit(DAMAGE_LOSELIFE, damroll(randint1(10), p_ptr->lev), "a lethal mutation", -1);
#endif

			power -= 10;
		}
	}

	if ((power > randint0(20)) && one_in_(4))
	{
		power -= 10;

		get_max_stats();
		do_cmd_rerate(FALSE);
	}

	while ((power > randint0(15)) && one_in_(3))
	{
		power -= 7;
		(void)gain_random_mutation(0);
	}

	if (power > randint0(5))
	{
		power -= 5;
		do_poly_wounds();
	}

	/* Note: earlier deductions may have left power < 0 already. */
	while (power > 0)
	{
		mutate_player();
		power--;
	}
}


/*
 * Decreases players hit points and sets death flag if necessary
 *
 * XXX XXX XXX Invulnerability needs to be changed into a "shield"
 *
 * XXX XXX XXX Hack -- this function allows the user to save (or quit)
 * the game when he dies, since the "You die." message is shown before
 * setting the player to "dead".
 */
/*:::����HP�����������A�}�C�i�X�ɂȂ����玀�S����*/
/*:::�����A�ϐ��A��_�ɂ��_���[�W�����͊��ɍς�ł���*/
/*:::�Ԃ�l�͖ڂɂ͖ڂȂǂ̖��@�◎�n�����Ɏg����炵��*/
//����cptr hit_from��format()�̌��ʂ̃A�h���X�����̂܂ܓn����take_hit()�̏�������format()�̒��g���ς���čň��Q�[�����N���b�V�����邻���ȁB�m��Ȃ������B
int take_hit(int damage_type, int damage, cptr hit_from, int monspell)
{
	int old_chp = p_ptr->chp;

	int kaguya_card_num = 0;

	char death_message[1024];
	//v1.1.33b 80��256
	char tmp[256];

	int warning = (p_ptr->mhp * hitpoint_warn / 10);

	/* Paranoia */
	if (p_ptr->is_dead) return 0;

	///mod150610 ���[�U�[�n�́��ɂ��ʂ�炵���̂Ŗ�������
	if(p_ptr->inside_battle) return 0;

	if (p_ptr->sutemi) damage *= 2;
	if (p_ptr->special_defense & KATA_IAI) damage += (damage + 4) / 5;
	//�z�K�q�~�����@�ʏ햳�G�����ꕔ�U���͒ʂ�
	if (p_ptr->pclass == CLASS_SUWAKO && p_ptr->tim_general[0])
	{
		if(damage_type == DAMAGE_ATTACK) return 0;
		else	set_tim_general(0,TRUE,0,1);
	}
	//��֓���
	else if(p_ptr->pclass == CLASS_ICHIRIN && damage_type == DAMAGE_ATTACK && unzan_guard()) return 0;



	/*:::�ȒP���[�h�H*/
	///sys easymode
	if (easy_band) damage = (damage+1)/2;

	/*:::USELIFE:���@�⃌�C�V������HP��������Ƃ�*/
	if (damage_type != DAMAGE_USELIFE)
	{
		/* Disturb */
		disturb(1, 1);
		if (auto_more)
		{
			now_damaged = TRUE;
		}
	}
	/*:::���[�j���O����*/
	if (monspell >= 0) learn_spell(monspell);

	/* Mega-Hack -- Apply "invulnerability" */
	if ((damage_type != DAMAGE_USELIFE) && (damage_type != DAMAGE_LOSELIFE))
	{
		//v1.1.44 ���ǂ񂰁u�C�r���A���W�����[�V�����v
		if (evil_undulation(FALSE, TRUE))
		{
			if (damage_type == DAMAGE_FORCE)
			{
				msg_print("��ǂ��؂�􂩂ꂽ�I");
			}
			else
				return 0;
		}
		//v2.0.1 ���������̖�
		else if (p_ptr->special_defense & SD_LIFE_EXPLODE)
		{
			if (damage_type == DAMAGE_FORCE)
			{
				msg_print("�����͂̕ǂ��ђʂ��ꂽ�I");
			}
			else
			{
				//�U�����󂯂����Ɣ������N�����t���O�@�����Ŏ��s����ƃo�O�̌����ɂȂ肻���Ȃ̂�process_world()�ōs��
				flag_life_explode = TRUE;

				msg_print("�����̗͂��U���𖳌��������I");
				p_ptr->special_defense &= ~(SD_LIFE_EXPLODE);
				p_ptr->redraw |= PR_STATUS;
				return 0;
			}
		}

		/*:::�ؕ��͖��G�ɗD��H�@���G�ђʏ���*/
		if (IS_INVULN() && (damage < 9000))
		{
			/*:::���̌��Ǝ��̑劙�H*/
			///mod150201 �얲�̖��z�V���͖{����*���G*�ɂ��Ă���
			if(p_ptr->special_defense & MUSOU_TENSEI)
			{
				return 0;
			}
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				//msg_print("�o���A���؂�􂩂ꂽ�I");

				if(YUMA_ULTIMATE_MODE)
					msg_print("�����̈ݑ܂�˂��j��ꂽ�I");
				else
					msg_print("���s�\�̍U�����󂯂��I");

#else
				msg_print("The attack cuts your shield of invulnerability open!");
#endif
			}

			else if (one_in_(PENETRATE_INVULNERABILITY))
			{
#ifdef JP
				if (YUMA_ULTIMATE_MODE)
					msg_print("�U���̋z���Ɏ��s�����I");
				else
					msg_print("�U���𖳌����ł��Ȃ������I");

#else
				msg_print("The attack penetrates your shield of invulnerability!");
#endif
			}
			/*:::�_���[�W���󂯂��ɏI��*/
			else
			{
				return 0;
			}
		}
		/*:::���g*/
		if (CHECK_MULTISHADOW())
		{
			if (p_ptr->multishadow)
			{

				if (damage_type == DAMAGE_FORCE)
				{
					msg_print("���e����Ƃ��̂��؂�􂩂ꂽ�I");
				}
				else if (damage_type == DAMAGE_ATTACK)
				{
					msg_print("�U���͌��e�ɖ������A���Ȃ��ɂ͓͂��Ȃ������B");

					return 0;
				}

			}
			//v1.1.92 ������p���i �m���ōU���������ɓ������ă_���[�W��� multishadow�̔��莮�Ɋ�
			//�󂯂��_���[�W��magic_num1[6]�ɋL�^���Aprocess_upkeep_with_speed()�Ŗ����T������
			//���X�o���L���锻�肪�s���Amagic_num1[6]��30000���i�[����đ�ʂɖ����T���B���̂��ƈꎞ�I�ɒl���}�C�i�X�ɂȂ��čU�����炩�΂��Ă���Ȃ��Ȃ�
			else if(is_special_seikaku(SEIKAKU_SPECIAL_JYOON))
			{
				if (damage_type == DAMAGE_FORCE)
				{
					msg_print("�o����Ƃ��̂��؂�􂩂ꂽ�I");
				}
				else if(damage_type == DAMAGE_ATTACK && p_ptr->magic_num1[6] >= 0 )
				{
					msg_print("�U���͎o�ɖ������A���Ȃ��ɂ͓͂��Ȃ������B");
					p_ptr->magic_num1[6] += damage;

					//���X�o���L���锻��
					if (randint1(SHION_OIL_BOMB_LIMIT) < p_ptr->magic_num1[6]) p_ptr->magic_num1[6] = 30000;
					return 0;

				}

			}
			else
			{

				msg_print("ERROR:�z��O�̏󋵂�CHECK_MULTISHADOW()���ʂ��Ă���");
			}

		}




		/*:::�H�̉�*/
		if (p_ptr->wraith_form)
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("�������̑̂��؂�􂩂ꂽ�I");
#else
				msg_print("The attack cuts through your ethereal body!");
#endif
			}
			else
			{
				damage /= 2;
				if ((damage == 0) && one_in_(2)) damage = 1;
			}
		}
		///mod140218 ���@�̊Z
		/*:::���@�̊Z�̌��ʕύX*/
		if (p_ptr->magicdef)
		{
			if (damage_type == DAMAGE_FORCE)
			{
#ifdef JP
				msg_print("���@�̊Z���т��ꂽ�I");
#else
				msg_print("The attack cuts through your ethereal body!");
#endif
			}
			else
			{
				damage  = damage * 2 / 3;
				if ((damage == 0) && one_in_(2)) damage = 1;
			}
		}

		//�T�O���_�X�̒e��
		if(p_ptr->pclass == CLASS_SAGUME && p_ptr->tim_general[0] && damage_type != DAMAGE_FORCE && damage_type != DAMAGE_NOESCAPE)
		{
			if(damage < p_ptr->csp)
			{
				p_ptr->csp -= damage;
				p_ptr->redraw |= PR_MANA;
				redraw_stuff();
				return 0;
			}
			else
			{
				damage -= p_ptr->csp;
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				set_tim_general(0,TRUE,0,0);
				p_ptr->redraw |= PR_MANA;
				redraw_stuff();

			}

		}


		if(p_ptr->pclass == CLASS_CLOWNPIECE && damage_type != DAMAGE_FORCE)
		{
			int mult = 100 - (p_ptr->stat_ind[A_CON]+3) + 10;

			if(mult < 100) damage = damage * mult / 100;

		}
		if (p_ptr->special_defense & KATA_MUSOU)
		{
			damage /= 2;
			if ((damage == 0) && one_in_(2)) damage = 1;
		}

		//v1.1.27 ���E�K�[�h
		check_mana_shield(&damage,damage_type);



	} /* not if LOSELIFE USELIFE */


	/*:::���ׂЂ��z*/
	if (p_ptr->pclass == CLASS_SEIJA && p_ptr->magic_num1[SEIJA_ITEM_HIRARI] && !one_in_(7) &&
		(damage_type == DAMAGE_ATTACK || damage_type == DAMAGE_FORCE ))
	{
		p_ptr->magic_num1[SEIJA_ITEM_HIRARI] -= 1;
		if (damage_type == DAMAGE_FORCE)
		{
			msg_print("���̌��͂Ђ��z���ђʂ����I");
		}
		else
		{
			msg_print("�Ђ��z�ōU�������킵���I");
			return 0;
		}
	}
	//�p���X�B�̃_���[�W�J�E���g �A�C�e���J�[�h�ɑΉ����邽�ߑS�N���X�Ώۂɂ���
	if((damage_type == DAMAGE_ATTACK || damage_type == DAMAGE_FORCE ))
	{
		 parsee_damage_count += damage;
		 if(parsee_damage_count > 9999) parsee_damage_count = 9999;
	}
	//�N���E���s�[�X�̃_���[�W�J�E���g
	if(p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		p_ptr->magic_num1[0] += damage;
		p_ptr->update |= PU_HP;
	}

	if(damage) break_eibon_wheel();

	/* Hurt the player */
	p_ptr->chp -= damage;
	/*:::���E�̂Ƃ���HP0�Ŏ~�܂�H*/
	if(damage_type == DAMAGE_GENO && p_ptr->chp < 0)
	{
		damage += p_ptr->chp;
		p_ptr->chp = 0;
	}

	//v1.1.86 �A�r���e�B�J�[�h�u������P�̉B�����v�ɂ��MP��
	kaguya_card_num = count_ability_card(ABL_CARD_KAGUYA);
	if (damage_type == DAMAGE_ATTACK && kaguya_card_num && p_ptr->chp>=0)
	{
		int percen = calc_ability_card_prob(ABL_CARD_KAGUYA, kaguya_card_num);
		player_gain_mana(damage * percen / 100);
	}

	/*:::�����̈�̃��A�N�e�B�u�q�[���A�ϗe�̈�̃X���C���ω�*/
	//v1.1.85 �ŏ��̒��ɂ��郁�f�B�X�����ǉ�
	if((damage_type == DAMAGE_ATTACK || damage_type == DAMAGE_FORCE || damage_type == DAMAGE_NOESCAPE)	&& damage > 0 && p_ptr->chp >= 0 && 
		((p_ptr->pclass == CLASS_MEDICINE && cave_have_flag_bold(py, px, FF_POISON_PUDDLE) && cave_have_flag_bold(py, px, FF_DEEP)) 
			|| p_ptr->reactive_heal 
//			|| ((CHECK_YUMA_FOOD_STOCK) >= 5)
			|| (p_ptr->mimic_form == MIMIC_SLIME) 
			|| (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_LIFE && p_ptr->lev > 39)))
	{
		bool regenerator = FALSE;
		int heal = damroll(3,MAX(p_ptr->lev / 4,1));
		if(p_ptr->reactive_heal && (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_LIFE && p_ptr->lev > 39)) regenerator = TRUE;
//		if ((CHECK_YUMA_FOOD_STOCK) >= 9) regenerator = TRUE;
		if(regenerator) heal = heal * 3;

		p_ptr->chp +=heal;
		if(regenerator) msg_format("�󂯂��΂���̏����݂�݂�ǂ����Ă����I");
		else msg_format("�󂯂��΂���̏��������������I");
		if(p_ptr->reactive_heal) set_reactive_heal(p_ptr->reactive_heal - randint1(2),TRUE);
		//�ޖ����A�N�e�B�u�q�[���͖����x����
//		if ((CHECK_YUMA_FOOD_STOCK) >= 5)
//		{
//			set_food(p_ptr->food - heal * 10);
//		}
		if(p_ptr->chp >= p_ptr->mhp)
		{
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;

		}



	}
	if(p_ptr->pclass == CLASS_SEIJA && p_ptr->magic_num1[SEIJA_ITEM_JIZO] && p_ptr->chp < p_ptr->mhp / 2)
	{
		msg_format("�g����n�����ӂ����I");
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
		p_ptr->magic_num1[SEIJA_ITEM_JIZO] -= 1;
	}

	/* Display the hitpoints */
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	handle_stuff();

	/*
	if (damage_type != DAMAGE_GENO && p_ptr->chp == 0)
	{
		chg_virtue(V_SACRIFICE, 1);
		chg_virtue(V_CHANCE, 2);
	}
	*/
	/* Dead player */
	if (p_ptr->chp < 0)
	{
		bool android = (p_ptr->prace == RACE_ANDROID ? TRUE : FALSE);

//		if(p_ptr->pclass == CLASS_MOKOU && !p_ptr->inside_arena)
		if (p_ptr->pclass == CLASS_MOKOU)
		{
			if(p_ptr->csp >= 25)
			{
				flag_mokou_resurrection = TRUE;
				return damage;
			}
			else
				msg_print("���Ă��ƁI�̗͂̌��E���I");
		}

		if(p_ptr->pclass == CLASS_CLOWNPIECE)
		{
			msg_print("�����͂��c���Ă��Ȃ��c");
		}


#ifdef JP       /* ���񂾎��ɋ����I�����Ď�������ł��Ȃ����Ă݂� by Habu */
		/*:::���S���^����ɋ����Z�[�u*/
		if (!cheat_save)
			if(!save_player()) msg_print("�Z�[�u���s�I");
#endif

		/* Sound */
		sound(SOUND_DEATH);

		chg_virtue(V_SACRIFICE, 10);

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Note death */
		p_ptr->is_dead = TRUE;

		//�A���[�i���œ|�ꂽ�Ƃ��̏����@����������t���O������dungeon()���ōs��
		if (p_ptr->inside_arena)
		{
			//v1.1.51 �V�A���[�i�����ɂ�菑������
			if (p_ptr->pclass == CLASS_DOREMY)
			{
				msg_print("���Ȃ��͊댯�Ȗ�����ً}�����B");
			}
			else if (one_in_(3))
			{
				if (p_ptr->pclass == CLASS_SAGUME)
					msg_print("�u.....!!�v");
				else
					msg_print("�͂��I����������");
			}
			else if (one_in_(2))
			{
				if (p_ptr->pclass == CLASS_SAGUME)
					msg_print("���Ȃ��͐⋩�������čQ�ĂČ���}���ڂ��o�܂����B");
				else
					msg_print("���Ȃ��͐⋩���Ėڂ��o�܂����B");
			}
			else
			{
				msg_print("�h���~�[�u�M���̈����͎����������������܂����B�v");
			}

			//v1.1.57
			if (record_arena)
			{
				do_cmd_write_nikki(NIKKI_ARENA,-1, hit_from);
			}

#if 0
			cptr m_name = r_name+r_info[arena_info[p_ptr->arena_number].r_idx].name;
#ifdef JP
			msg_format("���Ȃ���%s�̑O�ɔs�ꋎ�����B", m_name);
#else
			msg_format("You are beaten by %s.", m_name);
#endif
			msg_print(NULL);
			if (record_arena) do_cmd_write_nikki(NIKKI_ARENA, -1 - p_ptr->arena_number, m_name);
#endif
		}
		else if (p_ptr->inside_quest == QUEST_MIYOI) //�|��Ă��Q�[���I�[�o�[�ɂȂ�Ȃ�����N�G�X�g
		{

		}
		else
		{
			int q_idx = quest_number(dun_level);
			bool seppuku = streq(hit_from, "Seppuku");
			bool winning_seppuku = p_ptr->total_winner && seppuku;

			//v1.1.58
			play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_GAMEOVER);

#ifdef WORLD_SCORE
			/* Make screen dump */
			screen_dump = make_screen_dump();
#endif

			/* Note cause of death */
			if (seppuku)
			{
				strcpy(p_ptr->died_from, hit_from);
#ifdef JP
				if (!winning_seppuku) strcpy(p_ptr->died_from, "�ؕ�");
#endif
			}
			else
			{
				char dummy[1024];
#ifdef JP
				sprintf(dummy, "%s%s%s", !p_ptr->paralyzed ? "" : p_ptr->free_act ? "������Ԃ�" : "��჏�Ԃ�", p_ptr->image ? "���o�ɘc��" : "", hit_from);
#else
				sprintf(dummy, "%s%s", hit_from, !p_ptr->paralyzed ? "" : " while helpless");
#endif
				my_strcpy(p_ptr->died_from, dummy, sizeof p_ptr->died_from);
			}

			/* No longer a winner */
			p_ptr->total_winner = FALSE;

			if (winning_seppuku)
			{
#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����̌�ؕ������B");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "did Seppuku after the winning.");
#endif
			}
			else
			{
				char buf[20];

				if (p_ptr->inside_arena)
#ifdef JP
					strcpy(buf,"�A���[�i");
#else
					strcpy(buf,"in the Arena");
#endif
				else if (!dun_level)
#ifdef JP
					strcpy(buf,"�n��");
#else
					strcpy(buf,"on the surface");
#endif
				else if (q_idx && (is_fixed_quest_idx(q_idx) &&
				   //      !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_SERPENT))))
				         !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_YUKARI))))
#ifdef JP
					strcpy(buf,"�N�G�X�g");
#else
					strcpy(buf,"in a quest");
#endif
				else
#ifdef JP
					sprintf(buf,"%d�K", dun_level);
#else
					sprintf(buf,"level %d", dun_level);
#endif

#ifdef JP
				///sys131209 dead
				//sprintf(tmp, "%s��%s�ɎE���ꂽ�B", buf, p_ptr->died_from);
				sprintf(tmp, "%s��%s�ɓ|���ꂽ�B", buf, p_ptr->died_from);

#else
				sprintf(tmp, "killed by %s %s.", p_ptr->died_from, buf);
#endif
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
			}

#ifdef JP
			do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- �Q�[���I�[�o�[ --------");
#else
			do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");

			flush();

#ifdef JP
			if (get_check_strict("��ʂ�ۑ����܂����H", CHECK_NO_HISTORY))
#else
			if (get_check_strict("Dump the screen? ", CHECK_NO_HISTORY))
#endif
			{
				do_cmd_save_screen();
			}

			flush();

			/* Initialize "last message" buffer */
			if (p_ptr->last_message) string_free(p_ptr->last_message);
			p_ptr->last_message = NULL;

			/* Hack -- Note death */
			if (!last_words)
			{
#ifdef JP
				///msg131209 dead
				//msg_format("���Ȃ���%s�܂����B", android ? "���" : "����");
				msg_format("  ���g�n�w�I�@Game over");

#else
				msg_print(android ? "You are broken." : "You die.");
#endif

				msg_print(NULL);
			}
			else
			{
				//���S���Z���t�̃f�t�H���g���b�Z�[�W
				if (winning_seppuku)
				{
#ifdef JP
					get_rnd_line("seppuku_j.txt", 0, death_message);
#else
					get_rnd_line("seppuku.txt", 0, death_message);
#endif
				}
				else if(p_ptr->prace == RACE_NINJA) 
					sprintf(death_message,"�T���i���I");
				else
				{
#ifdef JP
					get_rnd_line("death_j.txt", 0, death_message);
#else
					get_rnd_line("death.txt", 0, death_message);
#endif
				}

				do
				{
#ifdef JP
					///sys131209 dead
					//while (!get_string(winning_seppuku ? "�����̋�: " : "�f�����̋���: ", death_message, 1024)) ;
					while (!get_string(winning_seppuku ? "�����̋�: " : "�����Z���t: ", death_message, 1024)) ;
#else
					while (!get_string("Last word: ", death_message, 1024)) ;
#endif
				}
#ifdef JP
				while (winning_seppuku && !get_check_strict("��낵���ł����H", CHECK_NO_HISTORY));
#else
				while (winning_seppuku && !get_check_strict("Are you sure? ", CHECK_NO_HISTORY));
#endif

				if (death_message[0] == '\0')
				{
#ifdef JP

					///msg131209 dead
					strcpy(death_message, format("  ���g�n�w�I�@Game over"));
					//strcpy(death_message, format("���Ȃ���%s�܂����B", android ? "���" : "����"));
#else
					strcpy(death_message, android ? "You are broken." : "You die.");
#endif
				}
				else p_ptr->last_message = string_make(death_message);

#ifdef JP
				if (winning_seppuku)
				{
					int i, len;
					int w = Term->wid;
					int h = Term->hgt;
					int msg_pos_x[9] = {  5,  7,  9, 12,  14,  17,  19,  21, 23};
					int msg_pos_y[9] = {  3,  4,  5,  4,   5,   4,   5,   6,  4};
					cptr str;
					char* str2;

					Term_clear();

					/* ���U�� */
					for (i = 0; i < 40; i++)
						Term_putstr(randint0(w / 2) * 2, randint0(h), 2, TERM_VIOLET, "��");

					str = death_message;
					if (strncmp(str, "�u", 2) == 0) str += 2;

					str2 = my_strstr(str, "�v");
					if (str2 != NULL) *str2 = '\0';

					i = 0;
					while (i < 9)
					{
						str2 = my_strstr(str, " ");
						if (str2 == NULL) len = strlen(str);
						else len = str2 - str;

						if (len != 0)
						{
							Term_putstr_v(w * 3 / 4 - 2 - msg_pos_x[i] * 2, msg_pos_y[i], len,
							TERM_WHITE, str);
							if (str2 == NULL) break;
							i++;
						}
						str = str2 + 1;
						if (*str == 0) break;
					}

					/* Hide cursor */
					Term_putstr(w-1, h-1, 1, TERM_WHITE, " ");

					flush();
#ifdef WORLD_SCORE
					/* Make screen dump */
					screen_dump = make_screen_dump();
#endif

					/* Wait a key press */
					(void)inkey();
				}
				else
#endif
					msg_print(death_message);
			}
		}

		/* Dead */
		return damage;
	}

	//�p���X�B��HP�����{�[�i�X
	if(p_ptr->pclass == CLASS_PARSEE) p_ptr->update |= PU_BONUS;


	//v1.1.25 �s���`(50%�ȉ��Œ�)�Ɋׂ����񐔂��J�E���g���Ă݂�
	if(old_chp > p_ptr->mhp / 2 && p_ptr->chp < p_ptr->mhp / 2)
	{
		score_count_pinch++;
	}
	//v1.1.25 ��s���`(10%�ȉ��Œ�)�Ɋׂ����񐔂��J�E���g���Ă݂�
	if(old_chp > p_ptr->mhp / 10 && p_ptr->chp < p_ptr->mhp / 10)
	{
		score_count_pinch2++;
	}

	/* Hitpoint warning */
	if (p_ptr->chp < warning)
	{
		/* Hack -- bell on first notice */
		if (old_chp > warning) bell();

		sound(SOUND_WARN);


		if (record_danger && (old_chp > warning))
		{
			if (p_ptr->image && damage_type == DAMAGE_ATTACK)
#ifdef JP
				hit_from = "����";
#else
				hit_from = "something";
#endif

#ifdef JP
			sprintf(tmp,"%s�ɂ���ăs���`�Ɋׂ����B",hit_from);
#else
			sprintf(tmp,"A critical situation because of %s.",hit_from);
#endif
			do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
		}

		if (auto_more)
		{
			/* stop auto_more even if DAMAGE_USELIFE */
			now_damaged = TRUE;
		}

		/* Message */
#ifdef JP
msg_print("*** �x��:��q�b�g�E�|�C���g�I ***");
#else
		msg_print("*** LOW HITPOINT WARNING! ***");
#endif

		msg_print(NULL);
		flush();
	}
	if (p_ptr->wild_mode && !p_ptr->leaving && (p_ptr->chp < MAX(warning, p_ptr->mhp/5)))
	{
		change_wild_mode();
	}
	return damage;
}


/*
 * Gain experience
 */
/*:::�o���l��������*/
void gain_exp_64(s32b amount, u32b amount_frac)
{
	if (p_ptr->is_dead) return;

	if (p_ptr->prace == RACE_ANDROID) return;

	//v1.1.51 �V�A���[�i�ł͌o���l�𓾂��Ȃ�
	if (p_ptr->inside_arena) return;

	/*:::���̖�Ƃ�*/
	if(p_ptr->pclass == CLASS_HINA)
		hina_yakuotoshi((int)amount);

	/* Gain some experience */
	s64b_add(&(p_ptr->exp), &(p_ptr->exp_frac), amount, amount_frac);

	/* Slowly recover from experience drainage */
	if (p_ptr->exp < p_ptr->max_exp)
	{
		/* Gain max experience (20%) (was 10%) */
		p_ptr->max_exp += amount / 5;
	}

	/* Check Experience */
	check_experience();
}


/*
 * Gain experience
 */
void gain_exp(s32b amount)
{
	gain_exp_64(amount, 0L);
}

/*:::�A���h���C�h�o���l�v�Z*/
///item
void calc_android_exp(void)
{
	int i;
	u32b total_exp = 0;
	if (p_ptr->is_dead) return;

	if (p_ptr->prace != RACE_ANDROID) return;

	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];
		object_type forge;
		object_type *q_ptr = &forge;
		u32b value, exp;
		int level = MAX(k_info[o_ptr->k_idx].level - 8, 1);

		if ((i == INVEN_RIGHT) || (i == INVEN_LEFT) || (i == INVEN_NECK) || (i == INVEN_LITE)) continue;
		if (!o_ptr->k_idx) continue;

		/* Wipe the object */
		object_wipe(q_ptr);

		object_copy(q_ptr, o_ptr);
		q_ptr->discount = 0;
		q_ptr->curse_flags = 0L;

		if (object_is_fixed_artifact(o_ptr))
		{
			level = (level + MAX(a_info[o_ptr->name1].level - 8, 5)) / 2;
			level += MIN(20, a_info[o_ptr->name1].rarity/(a_info[o_ptr->name1].gen_flags & TRG_INSTA_ART ? 10 : 3));
		}
		else if (object_is_ego(o_ptr))
		{
			level += MAX(3, (e_info[o_ptr->name2].rating - 5)/2);
		}
		else if (o_ptr->art_name)
		{
			s32b total_flags = flag_cost(o_ptr, o_ptr->pval,FALSE);
			int fake_level;

			if (!object_is_weapon_ammo(o_ptr))
			{
				/* For armors */
				if (total_flags < 15000) fake_level = 10;
				else if (total_flags < 35000) fake_level = 25;
				else fake_level = 40;
			}
			else
			{
				/* For weapons */
				if (total_flags < 20000) fake_level = 10;
				else if (total_flags < 45000) fake_level = 25;
				else fake_level = 40;
			}

			level = MAX(level, (level + MAX(fake_level - 8, 5)) / 2 + 3);
		}

		value = object_value_real(q_ptr);
		///mod131223 �A���h���o���l�v�Z�@�J�[�h�폜�Ȃ�
		if (value <= 0) continue;
		//if ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ABUNAI_MIZUGI) && (p_ptr->pseikaku != SEIKAKU_SEXY)) value /= 32;

		if (value > 5000000L) value = 5000000L;
		if (o_ptr->tval == TV_DRAG_ARMOR) level /= 2;
		///item ����̍����x�[�X�̓A���h���o���l�ɂ��܂荂����^���Ȃ��悤�ɂ���
		if (object_is_artifact(o_ptr) || object_is_ego(o_ptr) ||
		    (o_ptr->tval == TV_DRAG_ARMOR) ||
		    ((o_ptr->tval == TV_HEAD) && (o_ptr->sval == SV_HEAD_DRAGONHELM)) ||
		    ((o_ptr->tval == TV_SHIELD) && (o_ptr->sval == SV_DRAGON_SHIELD)) ||
		    ((o_ptr->tval == TV_GLOVES) && (o_ptr->sval == SV_HAND_DRAGONGLOVES)) ||
		    ((o_ptr->tval == TV_BOOTS) && (o_ptr->sval == SV_LEG_DRAGONBOOTS)) 
			|| ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_WEAPON_HIHIIROKANE)))
			
		{
			if (level > 65) level = 35 + (level - 65) / 5;
			else if (level > 35) level = 25 + (level - 35) / 3;
			else if (level > 15) level = 15 + (level - 15) / 2;
			exp = MIN(100000L, value) * level * level / 2;
			if (value > 100000L)
				exp += (value - 100000L) * level * level / 8;
		}
		else
		{
			exp = MIN(100000L, value) * level;
			if (value > 100000L)
				exp += (value - 100000L) * level / 4;
		}
		//buki_motteruka()���Ɠ���ϐg���ɔ���ʂ�Ȃ��Čo���l��/16�̂ق��ɂȂ�̂ŏC��
		//if ((((i == INVEN_RARM) || (i == INVEN_LARM)) && (buki_motteruka(i))) || (i == INVEN_RIBBON)) total_exp += exp / 48;
		if ((((i == INVEN_RARM) || (i == INVEN_LARM)) && (object_is_melee_weapon(o_ptr))) || (i == INVEN_RIBBON)) total_exp += exp / 48;
		else total_exp += exp / 16;
		if (i == INVEN_BODY) total_exp += exp / 32;
	}
	p_ptr->exp = p_ptr->max_exp = total_exp;

	/* Check Experience */
	check_experience();
}


/*
 * Lose experience
 */
void lose_exp(s32b amount)
{
	if (p_ptr->prace == RACE_ANDROID) return;

	/* Never drop below zero experience */
	if (amount > p_ptr->exp) amount = p_ptr->exp;

	/* Lose some experience */
	p_ptr->exp -= amount;

	/* Check Experience */
	check_experience();
}


/*
 * Drain experience
 * If resisted to draining, return FALSE
 */
/*:::�o���l�z�������@*/
///mod131228 res �o���l�z����n���ϐ��Ŕ��肷��悤�ɂ���
bool drain_exp(s32b drain, s32b slip, int hold_life_prob)
{
	/* Androids and their mimics are never drained */
	if (p_ptr->prace == RACE_ANDROID) return FALSE;

	if (p_ptr->resist_neth && (randint0(100) < hold_life_prob))
	//if (p_ptr->hold_life && (randint0(100) < hold_life_prob))
	{
		/* Hold experience */
#ifdef JP
		msg_print("���������Ȃ̐����͂���肫�����I");
#else
		msg_print("You keep hold of your life force!");
#endif
		return FALSE;
	}

	/* Hold experience failed */
	//if (p_ptr->hold_life)
	if (p_ptr->resist_neth)
	{
#ifdef JP
		msg_print("�����͂������z�����ꂽ�C������I");
#else
		msg_print("You feel your life slipping away!");
#endif
		lose_exp(slip);
	}
	else
	{
#ifdef JP
		msg_print("�����͂��̂���z�����ꂽ�C������I");
#else
		msg_print("You feel your life draining away!");
#endif
		lose_exp(drain);
	}

	return TRUE;
}


bool set_ultimate_res(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->ult_res && !do_dec)
		{
			if (p_ptr->ult_res > v) return FALSE;
		}
		else if (!p_ptr->ult_res)
		{
#ifdef JP
msg_print("�����邱�Ƃɑ΂��đϐ��������C������I");
#else
			msg_print("You feel resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->ult_res)
		{
#ifdef JP
msg_print("�����邱�Ƃɑ΂���ϐ������ꂽ�C������B");
#else
			msg_print("You feel less resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->ult_res = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



///mod140420 �ψÍ�
bool set_tim_res_dark(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_dark && !do_dec)
		{
			if (p_ptr->tim_res_dark > v) return FALSE;
		}
		else if (!p_ptr->tim_res_dark)
		{
#ifdef JP
msg_print("�Í��̗͂ɑ΂��đϐ��������C������I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_dark)
		{
#ifdef JP
msg_print("�Í��̗͂ɑ΂���ϐ������ꂽ�C������B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_dark = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_nether(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_nether && !do_dec)
		{
			if (p_ptr->tim_res_nether > v) return FALSE;
		}
		else if (!p_ptr->tim_res_nether)
		{
#ifdef JP
msg_print("�n���̗͂ɑ΂��đϐ��������C������I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_nether)
		{
#ifdef JP
msg_print("�n���̗͂ɑ΂���ϐ������ꂽ�C������B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_nether = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



bool set_tim_res_insanity(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_insanity && !do_dec)
		{
			if (p_ptr->tim_res_insanity > v) return FALSE;
		}
		else if (!p_ptr->tim_res_insanity)
		{
#ifdef JP
msg_print("�يE�̜��܂������|�ɑ΂��đϐ��������C������I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_insanity)
		{
#ifdef JP
msg_print("�يE�̜��܂������|�ɑ΂���ϐ������ꂽ�C������B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_insanity = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_chaos(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_chaos && !do_dec)
		{
			if (p_ptr->tim_res_chaos > v) return FALSE;
		}
		else if (!p_ptr->tim_res_chaos)
		{
#ifdef JP
msg_print("�J�I�X�̗͂ɑ΂��đϐ��������C������I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_chaos)
		{
#ifdef JP
msg_print("�J�I�X�̗͂ɑ΂���ϐ������ꂽ�C������B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_chaos = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::���[�_�[�Z���X*/
bool set_radar_sense(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->radar_sense && !do_dec)
		{
			if (p_ptr->radar_sense > v) return FALSE;
		}
		else if (!p_ptr->radar_sense)
		{
#ifdef JP
msg_print("���͂̑S�Ă�m�o�ł���悤�ɂȂ����I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->radar_sense)
		{
#ifdef JP
msg_print("���i�ʂ�̒m�o�͂ɖ߂����C������B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->radar_sense = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tim_res_time(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_time && !do_dec)
		{
			if (p_ptr->tim_res_time > v) return FALSE;
		}
		else if (!p_ptr->tim_res_time)
		{
#ifdef JP
			///msg131221
msg_print("����̗͂ɑ΂��đϐ��������C������I");
#else
			msg_print("You feel time resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_time)
		{
#ifdef JP
msg_print("����̗͂ɑ΂���ϐ������ꂽ�C������B");
#else
			msg_print("You feel less time resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_time = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_reactive_heal(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->reactive_heal && !do_dec)
		{
			if (p_ptr->reactive_heal > v) return FALSE;
		}
		else if (!p_ptr->reactive_heal)
		{
#ifdef JP
msg_print("���Ȃ��͓G����̍U���ɔ������B");
#else
			msg_print("You feel time resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->reactive_heal)
		{
#ifdef JP
msg_print("�����񕜂̔������������C������B");
#else
			msg_print("You feel less time resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->reactive_heal = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*:::�ϗe�̈�@�܋���*/
bool set_clawextend(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->clawextend && !do_dec)
		{
			if (p_ptr->clawextend > v) return FALSE;
		}
		else if (!p_ptr->clawextend)
		{
#ifdef JP
			msg_print("���Ȃ��̒܂͒����L�сA�s���������܂��ꂽ�B");
#else
			msg_print("You feel your consciousness expand!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->clawextend)
		{
#ifdef JP
			msg_print("�܂������������B");
#else
			msg_print("Your consciousness contracts again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->clawextend = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

bool set_tim_res_water(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_res_water && !do_dec)
		{
			if (p_ptr->tim_res_water > v) return FALSE;
		}
		else if (!p_ptr->tim_res_water)
		{
#ifdef JP
msg_print("���ɑ΂��đϐ��������C������I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res_water)
		{
#ifdef JP
msg_print("���ɑ΂���ϐ������ꂽ�C������B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_res_water = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tim_speedster(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_speedster && !do_dec)
		{
			if (p_ptr->tim_speedster > v) return FALSE;
		}
		else if (!p_ptr->tim_speedster)
		{
#ifdef JP
msg_print("�����ňړ��ł���悤�ɂȂ����I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_speedster)
		{
#ifdef JP
msg_print("�����x���Ȃ����C������B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_speedster = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


bool set_tim_lucky(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->lucky && !do_dec)
		{
			if (p_ptr->lucky > v) return FALSE;
		}
		else if (!p_ptr->lucky)
		{
#ifdef JP
			if(p_ptr->prace == RACE_STRAYGOD)
				msg_print("�K�^������Ă���C������I");
			else
				msg_print("�K�^�̎d�g�݂��킩�����C������I");
#else
			msg_print("You feel nether resistant!");
#endif

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->lucky)
		{
#ifdef JP
			if(p_ptr->prace == RACE_STRAYGOD)
				msg_print("�^�C�����i�ʂ�ɖ߂����B");
			else
				msg_print("�K�^�Ƃ͉��������̂��킩��Ȃ��Ȃ����B");
#else
			msg_print("You feel less nether resistant");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->lucky = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Choose a warrior-mage elemental attack. -LM-
 */
bool choose_ele_attack(void)
{
	int num;

	char choice;

	if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM) && !(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts))
	{
#ifdef JP
		msg_format("����������Ȃ��Ɩ��@���͎g���Ȃ��B");
#else
		msg_format("You cannot use temporary branding with no weapon.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

	num = (p_ptr->lev - 20) / 5;

#ifdef JP
		      c_prt(TERM_RED,    "        a) �Ċ�", 2, 14);
#else
		      c_prt(TERM_RED,    "        a) Fire Brand", 2, 14);
#endif

#ifdef JP
	if (num >= 2) c_prt(TERM_L_WHITE,"        b) ����", 3, 14);
#else
	if (num >= 2) c_prt(TERM_L_WHITE,"        b) Cold Brand", 3, 14);
#endif
	else prt("", 3, 14);

#ifdef JP
	if (num >= 3) c_prt(TERM_GREEN,  "        c) �ŎE", 4, 14);
#else
	if (num >= 3) c_prt(TERM_GREEN,  "        c) Poison Brand", 4, 14);
#endif
	else prt("", 4, 14);

#ifdef JP
	if (num >= 4) c_prt(TERM_L_DARK, "        d) �n��", 5, 14);
#else
	if (num >= 4) c_prt(TERM_L_DARK, "        d) Acid Brand", 5, 14);
#endif
	else prt("", 5, 14);

#ifdef JP
	if (num >= 5) c_prt(TERM_BLUE,   "        e) �d��", 6, 14);
#else
	if (num >= 5) c_prt(TERM_BLUE,   "        e) Elec Brand", 6, 14);
#endif
	else prt("", 6, 14);

	prt("", 7, 14);
	prt("", 8, 14);
	prt("", 9, 14);

	prt("", 1, 0);
#ifdef JP
	prt("        �ǂ̌��f�U�������܂����H", 1, 14);
#else
	prt("        Choose a temporary elemental brand ", 1, 14);
#endif

	choice = inkey();

	if ((choice == 'a') || (choice == 'A')) 
		set_ele_attack(ATTACK_FIRE, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'b') || (choice == 'B')) && (num >= 2))
		set_ele_attack(ATTACK_COLD, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'c') || (choice == 'C')) && (num >= 3))
		set_ele_attack(ATTACK_POIS, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'd') || (choice == 'D')) && (num >= 4))
		set_ele_attack(ATTACK_ACID, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else if (((choice == 'e') || (choice == 'E')) && (num >= 5))
		set_ele_attack(ATTACK_ELEC, p_ptr->lev/2 + randint1(p_ptr->lev/2));
	else
	{
#ifdef JP
		msg_print("���@�����g���̂���߂��B");
#else
		msg_print("You cancel the temporary branding.");
#endif
		screen_load();
		return FALSE;
	}
	/* Load screen */
	screen_load();
	return TRUE;
}


/*
 * Choose a elemental immune. -LM-
 */
bool choose_ele_immune(int v)
{
	char choice;

	/* Save screen */
	screen_save();

#ifdef JP
	c_prt(TERM_RED,    "        a) �Ή�", 2, 14);
#else
	c_prt(TERM_RED,    "        a) Immune Fire", 2, 14);
#endif

#ifdef JP
	c_prt(TERM_L_WHITE,"        b) ��C", 3, 14);
#else
	c_prt(TERM_L_WHITE,"        b) Immune Cold", 3, 14);
#endif

#ifdef JP
	c_prt(TERM_L_DARK, "        c) �_", 4, 14);
#else
	c_prt(TERM_L_DARK, "        c) Immune Acid", 4, 14);
#endif

#ifdef JP
	c_prt(TERM_BLUE,   "        d) �d��", 5, 14);
#else
	c_prt(TERM_BLUE,   "        d) Immune Elec", 5, 14);
#endif


	prt("", 6, 14);
	prt("", 7, 14);
	prt("", 8, 14);
	prt("", 9, 14);

	prt("", 1, 0);
#ifdef JP
	prt("        �ǂ̌��f�̖Ɖu�����܂����H", 1, 14);
#else
	prt("        Choose a temporary elemental immune ", 1, 14);
#endif

	choice = inkey();

	if ((choice == 'a') || (choice == 'A')) 
		set_ele_immune(DEFENSE_FIRE, v);
	else if ((choice == 'b') || (choice == 'B'))
		set_ele_immune(DEFENSE_COLD, v);
	else if ((choice == 'c') || (choice == 'C'))
		set_ele_immune(DEFENSE_ACID, v);
	else if ((choice == 'd') || (choice == 'D'))
		set_ele_immune(DEFENSE_ELEC, v);
	else
	{
#ifdef JP
		msg_print("�Ɖu��t����̂���߂��B");
#else
		msg_print("You cancel the temporary immune.");
#endif
		screen_load();
		return FALSE;
	}
	/* Load screen */
	screen_load();
	return TRUE;
}


/*:::�E�Ƃ��Ɣėp�J�E���g reset_tim_flag()�Ń��Z�b�g���邪���͏����ł͏����Ȃ�*/
/*:::ind:p_ptr->tim_general[]�̓Y���@num:����������n�������Ƃ��Ɏg��*/
/*:::tim_general[0]: �����̖��Ԃ̋��ԁA���C�h�̃e�B�[�^�C���A*/
bool set_tim_general(int v, bool do_dec, int ind, int num)
{
	bool notice = FALSE;

	if (cheat_xtra && !do_dec)
		msg_format("set_tim_general time:%d ind:%d num:%d", v, ind, num);

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;
	if(ind >= TIM_GENERAL_MAX || ind < 0)
	{
		msg_format("ERROR:tim_general()���͈͊O��index(%d)�ŌĂ΂ꂽ",ind);
		return (FALSE);
	}

	if (p_ptr->is_dead) return FALSE;

	/*:::����,�얲,���~���A,�����ꏈ���@���Ԃ̋��ԂɎ���ꂽ�����X�^�[�����񂾂���ʂ�������*/
	//v1.1.95 �ړ��֎~������MTIMED2_NO_MOVE�ɓ��������̂ŏ����폜
//	if(p_ptr->pclass == CLASS_KOMACHI && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;
//	if(p_ptr->pclass == CLASS_REIMU && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;
//	if(p_ptr->pclass == CLASS_REMY && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;
//	if(p_ptr->pclass == CLASS_HINA && ind == 0 && !m_list[p_ptr->magic_num1[2]].r_idx) v=0;

	//�}�~�]�E���ϐg�����Ă��郂���X�^�[�@�|��������ʂ�������
	if(p_ptr->pclass == CLASS_MAMIZOU && ind == 0 && !m_list[p_ptr->magic_num1[0]].r_idx) v=0;


	/* Open */
	if (v)
	{
		if (p_ptr->tim_general[ind] && !do_dec)
		{
			if (p_ptr->tim_general[ind] > v) return FALSE;
		}
		else if (!p_ptr->tim_general[ind])
		{
			if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES)) ;
			//if(p_ptr->pclass == CLASS_KOMACHI && ind == 0) ;
			//else if(p_ptr->pclass == CLASS_HINA && ind == 0) ;
			//else if(p_ptr->pclass == CLASS_REIMU && ind == 0) ;
			//else if(p_ptr->pclass == CLASS_REMY && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MAMIZOU && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MAID && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MARISA && ind == 0) ;
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST ) ;
			else if(p_ptr->pclass == CLASS_SHINMYOUMARU && ind == 0);
			else if(USE_NAMELESS_ARTS)
			{
				//v1.1.56
				msg_print(msg_tim_nameless_arts(num, 0, TRUE));
				if(num == JKF1_SELF_LIGHT) p_ptr->update |= PU_TORCH; //�\�́u�����v�p
			}
			else if(p_ptr->pclass == CLASS_BYAKUREN && ind == 0)
				msg_format("�̂ɗ͂����ӂ�Ă���I");
			else if(p_ptr->pclass == CLASS_YOUMU && ind == 0)
				msg_format("�킢�̉��g�ɂȂ����C������I");
			else if(p_ptr->pclass == CLASS_SUWAKO && ind == 0)
				msg_format("���Ȃ��͖X�q�����̏�Ɏc���Ēn�ʂɐ������E�E");
			else if(p_ptr->pclass == CLASS_RUMIA && ind == 0)
				msg_format("���Ȃ��͔Z���Ȉł𐶂ݏo���g�ɂ܂Ƃ����I");
			else if(p_ptr->pclass == CLASS_HECATIA && ind == 0)
				msg_format("���Ȃ��͏�ǂ�W�J�����B");
			else if (p_ptr->pclass == CLASS_IKU && ind == 0)
				msg_format("���Ȃ��͓G�̍U���ɑ΂��Đg�\�����B");
			else if (p_ptr->pclass == CLASS_KOKORO && ind == 0)
				msg_format("���낵���`���̖ʂ����Ȃ��̎��͂ɕ��V�����B");
			else if(p_ptr->pclass == CLASS_YUKARI)
			{
				; //���b�Z�[�W�Ȃ�
			}
			else if (p_ptr->pclass == CLASS_OKINA)
			{
				if(p_ptr->stat_ind[A_CHR] < 3)
					msg_print("���Ȃ��͖`���I�ɐg�����˂点�n�߂�...");
				else if (p_ptr->stat_ind[A_CHR] < 13)
					msg_print("���i��蓮�����݂��Ȃ����C������B");
				else if (p_ptr->stat_ind[A_CHR] < 23)
					msg_print("���Ȃ��͌����悭�L�r�L�r�����n�߂��B");
				else if (p_ptr->stat_ind[A_CHR] < 33)
					msg_print("���Ȃ��͕����悤�ɗ���ɓ����n�߂��B");
				else
					msg_print("���Ȃ��̏���͐������ɂ߂��I");
			}
			else if(p_ptr->pclass == CLASS_RINGO);
			else if(p_ptr->pclass == CLASS_SAGUME)
				msg_format("�����̋P�����̂����Ȃ������͂񂾁I");
			else if (p_ptr->pclass == CLASS_JYOON)
				msg_format("���Ȃ��͍��������̂̉^�C��D���n�߂�...");
			else if (p_ptr->pclass == CLASS_PARSEE)
				msg_format("���Ȃ��̎��͂����i�ƕs�a�̕��͋C�ɖ�����...");
			else if (p_ptr->pclass == CLASS_SHION)
			{
				switch (randint1(3))
				{
				case 1:
					msg_format("���Ȃ��͑S�l�ނ̕s�K�̑̌��Ɖ������I");
					break;
				case 2:
					msg_format("���s���̕��̃I�[�����_���W�����𕢂��s�������I");
					break;
				default:
					msg_format("���܂�ɗ��܂����s�^�p���[�����������I");
					break;
				}
				//�}�b�v���ĕ`�悷��
				p_ptr->redraw |= (PR_MAP);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
			else if (p_ptr->pclass == CLASS_URUMI)
			{
				msg_format("���Ȃ��͎茳�ɗ͂����߂��B��������Ƃ����d�݂�������...");
			}
			else if (p_ptr->pclass == CLASS_YUYUKO) //v1.1.76
			{
				msg_format("���͂̎҂�����̕��֍��������悤�ȗd�������������...");
			}
			else if (p_ptr->pclass == CLASS_TAKANE)
			{
				msg_format("���Ȃ��͐Â��ɓ����n�߂�...");
			}
			else if (p_ptr->pclass == CLASS_MOMOYO)
			{
				if (ind == 0) msg_print("���Ȃ��͖җ�ȑ��x�ŕǂ��@��n�߂��I");
				else if (ind == 1) msg_print("�ւ�H�ׂ����C�����I");
				else if (ind == 2) msg_print("����H�ׂ����C�����I");
				else msg_print("�S�Ă�H���s�����ė͂𓾂����C�����I");
			}
			else if (p_ptr->pclass == CLASS_MIYOI)
			{
				msg_format("���Ȃ��͂����Ƃ��̏ꂩ�狎�낤�Ǝ��݂�...");

			}
			else
			{
				msg_format("ERROR:tim_general[%d]����`����Ă��Ȃ��Ă΂���������A�������̓��b�Z�[�W������`",ind);
			}
			notice = TRUE;
		}
		//���̋��� �ݐς���
		if(p_ptr->pclass == CLASS_RINGO && !do_dec && ind == 0)
		{
			p_ptr->magic_num1[1]++;
		}
		if(p_ptr->pclass == CLASS_RINGO && !do_dec && ind == 1)
		{
			msg_format("%s�����U���ւ̖Ɖu���l�������I",gf_desc_name[num]);
			p_ptr->magic_num1[3] = num;
			p_ptr->redraw |= (PR_STATUS);

		}

	}

	/* Shut */
	else
	{
		if (p_ptr->tim_general[ind])
		{
			if (USE_NAMELESS_ARTS)
			{
				//v1.1.56
				msg_print(msg_tim_nameless_arts(0, ind, FALSE));

				p_ptr->update |= PU_TORCH; //�\�́u�����v�p
			}
			/*
			else if(p_ptr->pclass == CLASS_KOMACHI && ind == 0)
			{
				msg_format("���Ԃ��������B");
				p_ptr->magic_num1[0] = 0;
			}
			else if(p_ptr->pclass == CLASS_HINA && ind == 0)
			{
				msg_format("�G��߂炦�Ă�����������B");
				p_ptr->magic_num1[2] = 0;
			}
			else if(p_ptr->pclass == CLASS_REIMU && ind == 0)
			{
				msg_format("�w���������B");
				p_ptr->magic_num1[0] = 0;
			}
			else if(p_ptr->pclass == CLASS_REMY && ind == 0)
			{
			msg_format("�����������B");
			p_ptr->magic_num1[0] = 0;
			}
			*/
			else if(p_ptr->pclass == CLASS_RUMIA && ind == 0)
			{
				msg_format("�ł��������B");
			}
			else if(p_ptr->pclass == CLASS_SAGUME && ind == 0)
			{
				msg_format("�e�����������B");
			}
			else if(p_ptr->pclass == CLASS_YOUMU && ind == 0)
			{
				msg_format("�҂�S���Â܂����B");
			}
			else if(p_ptr->pclass == CLASS_HECATIA && ind == 0)
			{
				msg_format("��ǂ��������B");
			}
			else if(p_ptr->pclass == CLASS_MARISA && ind == 0)
			{
				msg_format("�}�W�b�N�A�u�\�[�o�[�̌��ʂ��������B");
			}
			else if(p_ptr->pclass == CLASS_MAMIZOU && ind == 0)
			{
				int xx = m_list[p_ptr->magic_num1[0]].fx;
				int yy = m_list[p_ptr->magic_num1[0]].fy;
				msg_format("�p�̌��ʂ��������B");
				p_ptr->magic_num1[0] = 0;
				if(cave[yy][xx].m_idx) lite_spot(yy,xx);
			}
			else if(p_ptr->pclass == CLASS_MAID && ind == 0)
				msg_format("�����A���������B");
			else if((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES))
				msg_format("���͂ɉ����߂����B");
			else if(p_ptr->pclass == CLASS_SHINMYOUMARU && ind == 0)
				msg_format("���Ƃ̏[�U�����������I");
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && ind == 0)
			{
				msg_format("�E�����̌��ʂ��؂ꂽ�B�Z�̔������̂��P�����I");

				set_stun(p_ptr->stun + 30 + randint1(30));
				set_slow(p_ptr->slow + 30 + randint1(30),FALSE);
			}
			else if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && ind == 1)
			{
				msg_format("�̂��E�����̔�������񕜂����悤���B");
			}
			else if(p_ptr->pclass == CLASS_BYAKUREN && ind == 0)
				msg_format("�������@�̌��ʂ��؂ꂽ�I");
			else if (p_ptr->pclass == CLASS_KOKORO && ind == 0)
				msg_format("���낵���`���̖ʂ͏������B");

			else if (p_ptr->pclass == CLASS_OKINA)
				msg_print("���Ȃ��͕��i�̓����ɖ߂����B");
			else if (p_ptr->pclass == CLASS_IKU && ind == 0)
				msg_format("���Ȃ��͍\�����������B");
			else if(p_ptr->pclass == CLASS_SUWAKO && ind == 0)
			{
				if(!num) 
					msg_format("���Ȃ��͖ڂ��o�܂����B");
				else 
					msg_format("���Ȃ��͓~������@���N�����ꂽ�I");

				p_ptr->energy_need += ENERGY_NEED();
			}
			else if(p_ptr->pclass == CLASS_YUKARI)
			{
				if(ind == 0) msg_format("���E�����ɖ߂����B�͏ꂪ�������B");
				if(ind == 1) msg_format("���E�����ɖ߂����B");
				if(ind == 2) msg_format("�t���A�̋C�z�����ɖ߂����B");
			}
			else if(p_ptr->pclass == CLASS_RINGO)
			{
				if(ind == 0)
				{
					p_ptr->magic_num1[1] = 0;
					msg_print("���Ȃ��̋����͌��ɖ߂����B");
				}
				if(ind == 1)
				{
					msg_format("�Ɖu���������B");
					p_ptr->magic_num1[3] = 0;
					p_ptr->redraw |= (PR_STATUS);
				}

			}
			else if (p_ptr->pclass == CLASS_JYOON)
				msg_format("�s�K�������炷�͂��~�߂��B");
			else if (p_ptr->pclass == CLASS_PARSEE)
				msg_format("���͂̕��͋C���ʏ�ɖ߂����B");
			else if (p_ptr->pclass == CLASS_SHION)
			{
				msg_format("���̃I�[���̕��o���~�܂����B");
				//�}�b�v���ĕ`�悷��
				p_ptr->redraw |= (PR_MAP);
				p_ptr->update |= (PU_MONSTERS);
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
			else if (p_ptr->pclass == CLASS_URUMI)
				msg_format("��ɓ`���d�݂����Ƃɖ߂����B");
			else if (p_ptr->pclass == CLASS_YUYUKO) //v1.1.76
			{
				msg_format("���̌��ʂ��������B");

			}
			else if (p_ptr->pclass == CLASS_TAKANE)
			{
				msg_format("���Ȃ��͕��i�̓����ɖ߂����B");
			}
			else if (p_ptr->pclass == CLASS_MOMOYO)
			{
				if (ind == 0) msg_print("���Ȃ��͕ǂ��@�����~�߂��B");
				else if (ind == 1) msg_print("�ւ̋C���ł͂Ȃ��Ȃ����B");
				else if (ind == 2) msg_print("���̋C���ł͂Ȃ��Ȃ����B");
				else msg_print("�͂ւ̊��]�����܂����B");

			}
			else if (p_ptr->pclass == CLASS_MIYOI)
			{
				msg_format("���Ȃ��͕��i�̓����ɖ߂����B");
			}
			else
			{
				msg_format("ERROR:tim_general[%d]����`����Ă��Ȃ��Ă΂���������A�������̓��b�Z�[�W������`",ind);
			}

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_general[ind] = v;
	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
	/* Nothing to notice */
	if (!notice) return (FALSE);

	//Hack - ���[�~�A�ŉ����ɂ��Ӗډ���
	if(p_ptr->pclass == CLASS_RUMIA && ind == 0)
	{
		if(p_ptr->tim_general[0])	set_blind(1);
		else	set_blind(0);
	}

	/* Disturb */
	if (disturb_state) disturb(0, 0);
	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	/* Handle stuff */
	handle_stuff();
	/* Result */
	return (TRUE);
}


///mod140804 �b���x�̃Z�b�g
bool set_asthma(int v)
{
	int old_aux, new_aux;
	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	/* Hack -- Force good values */
	v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	if (p_ptr->asthma < ASTHMA_1) old_aux = 1;
	else if (p_ptr->asthma < ASTHMA_2) old_aux = 2;
	else if (p_ptr->asthma < ASTHMA_3) old_aux = 3;
	else if (p_ptr->asthma < ASTHMA_4) old_aux = 4;
	else  old_aux = 5;

	if (v < ASTHMA_1) new_aux = 1;
	else if (v < ASTHMA_2) new_aux = 2;
	else if (v < ASTHMA_3) new_aux = 3;
	else if (v < ASTHMA_4) new_aux = 4;
	else new_aux=5;

	if (new_aux != old_aux)
	{
		if(old_aux < new_aux) msg_print("�b���̒��q�������Ȃ����B");
		else msg_print("�b���̒��q���ǂ��Ȃ����B");

		if(new_aux == 1) msg_print("�A����������u�����B");
		else if(new_aux == 2) msg_print("�����P�����ފ���������B");
		else if(new_aux == 3) msg_print("�P���~�܂炸�Â��ɍs���ł��Ȃ��B");
		else if(new_aux == 4) msg_print("�P���Ђǂ��Ă낭�ɒ���Ȃ��B");
		else msg_print("�ċz�����邽�тɍA�����ߕt������悤���I");
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->asthma = v;

	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*:::�ꎞ�I�\�͏㏸*/
/*:::stat:A_STR�`A_CHR amount:�㏸�l limitbreak:���E�l40�𒴂��邩�ǂ��� v:�J�E���g do_dec:���炷�Ƃ�TRUE*/
/*:::do_dec�̖��������̊֐��Ƃ��Ⴄ�B����̃J�E���g�͏��v�ŏ㏑������Ado_dec��TRUE�̂Ƃ�amount�̒l�͖��������B*/
//���Fdo_dec�ŌĂԂƂ���amount��0�ł��邱��
bool set_tim_addstat(int stat, int amount, int v, bool do_dec)
{
	bool notice = FALSE;
	bool do_powerup = FALSE;

	if(do_dec && amount)
	{
		msg_print("WARNING:set_tim_addstat()��do_dec��TRUE�Ȃ̂�amount���ݒ肳��Ă���");
		amount = 0;
	}

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		///mod150724 ������ނ̓��̋��������݂������Ƃ��A���͂̋������̂���ɗD�悳���悤�ɂ��Ă���
		//if (!p_ptr->tim_addstat_count[stat])
		if (p_ptr->tim_addstat_num[stat] <= amount)
		{
#ifdef JP
			//�㏸������\�͂ɉ����ă��b�Z�[�W�o�������Ǝv�������A���̊֐��Ăԏ��Ōʂɏ��������ق������R���낤
#endif
			do_powerup = TRUE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if(stat==A_STR && p_ptr->tim_addstat_count[stat]) msg_print("�͂̋������������C������B");
		if(stat==A_INT && p_ptr->tim_addstat_count[stat]) msg_print("���]�̍Ⴆ���݂����C������B");
		if(stat==A_WIS && p_ptr->tim_addstat_count[stat]) msg_print("���_�̓������݂����C������B");
		if(stat==A_DEX && p_ptr->tim_addstat_count[stat]) msg_print("�����̐؂ꂪ�݂����C������B");
		if(stat==A_CON && p_ptr->tim_addstat_count[stat]) msg_print("�̂̊挒�����������C������B");
		if(stat==A_CHR && p_ptr->tim_addstat_count[stat]) msg_print("�J���X�}���������C������B");
			notice = TRUE;
	}

	/* Use the value */
	///mod150724 ������ނ̓��̋��������݂������Ƃ��A���͂̋������̂���ɗD�悳���悤�ɂ��Ă���
	//p_ptr->tim_addstat_count[stat] = v;

	if(do_powerup || do_dec) p_ptr->tim_addstat_count[stat] = v;

	if(do_powerup) p_ptr->tim_addstat_num[stat] = amount;
	///mod150724 ��L�̕ύX�ɔ����A�������؂ꂽ�狭���l�i�[�ϐ������Z�b�g����K�v��������
	if(!p_ptr->tim_addstat_count[stat]) p_ptr->tim_addstat_num[stat] = 0;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
	/* Nothing to notice */
	if (!notice) return (FALSE);
	/* Disturb */
	if (disturb_state) disturb(0, 0);
	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	/* Handle stuff */
	handle_stuff();
	/* Result */
	return (TRUE);
}



//v2.0.11 �ړ��֎~�̃o�b�h�X�e�[�^�X
//�g���o�T�~��㩂�G�̓��ʍs���ɂ��S���Ȃ�
bool set_no_move(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;


	/* Open */
	if (v)
	{
		set_tsuyoshi(0, TRUE);
		if (!p_ptr->tim_no_move)
		{
#ifdef JP
			msg_print("���̏ꂩ��ړ��ł��Ȃ��Ȃ����I");
#else
			msg_print("Oh, wow! Everything looks so cosmic now!");
#endif
			/* Sniper */
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_no_move)
		{
#ifdef JP
			msg_print("�ړ��ł���悤�ɂȂ����B");
#else
			msg_print("You can see clearly again.");
#endif

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_no_move = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);
	//v1.1.58
	flag_update_floor_music = TRUE;
	/* Disturb */
	if (disturb_state) disturb(0, 1);

	/* Redraw map */
	//p_ptr->redraw |= (PR_MAP);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS | PU_BONUS);

	/* Window stuff */
	//p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



///mod140103 �_���ϐ��̂��߂ɒǉ�
/*:::�����j�׍U�����󂯂��Ƃ��̑ϐ��E��_�E�Ɖu�l���̂��߂Ɋ֐��쐬�B*/
/*:::�ϐ��l���O�_���[�W���󂯎���čl����_���[�W��Ԃ��B*/
int mod_holy_dam(int dam){
	int base = 100;

	base += is_hurt_holy();

	if(p_ptr->resist_holy) base /= 2;

	//�x���̎��ɂ����b�Z�[�W���o�Ă��܂��̂ňꎞ����
	/*
	if(base > 150) msg_print("���Ȃ��̓��̂͐��Ȃ�͂ɂ��R���オ�����I");
	else if(base > 100) msg_print("���Ȃ�͂����Ȃ��̓��̂��ł������I");
	else if(base == 0) msg_print("���Ȃ��̐���ȓ��̂͐��Ȃ�͂ŏ����Ȃ��B");
	*/

	dam = dam * base / 100;

	return (dam);
}

///mod140105 �_����_�̓x��������
/*:::�����_����_�Ȃ炻�̓x������%�ŕԂ��B������0�ŕ��Ȃ狭�����Ȃ�ア�B�����ϐ��Ƃ͕ʁB*/
/*:::�ϐ��ꗗ��ʂȂǂق��̔���Ŏg���₷���悤�ɏ�̊֐����番����*/
int is_hurt_holy(){
	int mod = 0;


	//�푰�ɂ��ݒ�
	if(p_ptr->prace == RACE_YOUKAI) mod = 33;
	else if(p_ptr->prace == RACE_KAPPA) mod = 33;
	else if(p_ptr->prace == RACE_KARASU_TENGU) mod = 33;
	else if(p_ptr->prace == RACE_HAKUROU_TENGU) mod = 33;
	else if(p_ptr->prace == RACE_ONI) mod = 33;
	else if(p_ptr->prace == RACE_WARBEAST) mod = 33;
	else if(p_ptr->prace == RACE_TSUKUMO) mod = 33;
	else if(p_ptr->prace == RACE_ANDROID) mod = 33;
	else if(p_ptr->prace == RACE_YAMAWARO) mod = 33;
	else if(p_ptr->prace == RACE_BAKEDANUKI) mod = 33;
	else if(p_ptr->prace == RACE_NINGYO) mod = 33;
	else if(p_ptr->prace == RACE_HOFGOBLIN) mod = 33;
	else if(p_ptr->prace == RACE_NYUDOU) mod = 33;
	else if(p_ptr->prace == RACE_ZOMBIE) mod = 66;
	else if(p_ptr->prace == RACE_IMP) mod = 66;
	else if(p_ptr->prace == RACE_VAMPIRE) mod = 66;
	else if(p_ptr->prace == RACE_SPECTRE) mod = 66;
	else if (p_ptr->prace == RACE_ANIMAL_GHOST) mod = 66;
	else if(p_ptr->prace == RACE_DAIYOUKAI) mod = 33;
	else if (p_ptr->prace == RACE_ZASHIKIWARASHI) mod = 33;
	else if (p_ptr->prace == RACE_HALF_GHOST)
	{
		//v1.1.65 ���l����̔j�׎�_��������̂͗d��Lv40�ȍ~�ɂȂ�悤�L�q�ύX
		if(p_ptr->pclass == CLASS_YOUMU && p_ptr->lev > 39) mod = 0;
		else mod = 33;
	}
	else if(p_ptr->prace == RACE_LICH) mod = 66;
	else if(p_ptr->prace == RACE_YOUKO)
	{
		if((p_ptr->realm1 < TV_BOOK_TRANSFORM  || p_ptr->realm1 > TV_BOOK_CHAOS) && p_ptr->lev > 29) mod = 0; 
		else mod = 33;
	}
	else if(prace_is_(RACE_STRAYGOD))
	{
		switch(p_ptr->race_multipur_val[3])
		{
			//���_�A���҂̐_�A�א_�A�O�Ȃ�_
			case 59: case 61: case 62: case 63:
			mod = 50;
			break;
		}
	}
	else if(p_ptr->prace == RACE_TENNIN) mod = -100;
	else if(p_ptr->prace == RACE_LUNARIAN) mod = -100;

	/*:::�ꕔ�E�ɂ��푰�̗�O����*/
	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->pseikaku == SEIKAKU_SHIAWASE && p_ptr->lev > 44) mod = 0;
	else if(p_ptr->pclass == CLASS_KASEN) mod=33;
	else if(p_ptr->pclass == CLASS_ORIN) mod=66;
	else if(p_ptr->pclass == CLASS_MURASA) mod=66;
	else if(p_ptr->pclass == CLASS_NAZRIN || p_ptr->pclass == CLASS_SHOU) mod=0;
	else if(p_ptr->pclass == CLASS_SHOU && p_ptr->lev > 44) mod=-100;
	else if(p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0]) mod = 0;
	else if(p_ptr->pclass == CLASS_UTSUHO && p_ptr->lev > 44) mod=0;
	else if(p_ptr->pclass == CLASS_MEIRIN && p_ptr->lev > 39) mod=0;
	else if(p_ptr->pclass == CLASS_HINA) mod=50;
	else if(p_ptr->pclass == CLASS_AUNN) mod=0;
	else if(p_ptr->pclass == CLASS_KEINE) mod=0;
	else if(p_ptr->pclass == CLASS_MIKO && p_ptr->lev > 29) mod = -100;
	else if(p_ptr->pclass == CLASS_YUYUKO) mod = 50; 
	else if(p_ptr->pclass == CLASS_REIMU && p_ptr->lev > 29 && osaisen_rank() > 4) mod = -100;
	else if(p_ptr->pclass == CLASS_EIKI) mod = -100;
	else if (p_ptr->pclass == CLASS_MIKE) mod = 0;


	//v1.1.37 �ꕔ�j�ׂɎア�푰�ɕϐg���͎푰�I�ȗ�O������㏑�����邱�Ƃɂ���
	if(p_ptr->mimic_form == MIMIC_DEMON || p_ptr->mimic_form == MIMIC_DEMON_LORD || p_ptr->mimic_form == MIMIC_VAMPIRE ) mod = 66;
	else if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_IS_UNDEAD | MIMIC_IS_DEMON)) mod = 66; //v1.1.37�ǉ��@���̏������Ƃ�����Ɣ�邪
	else if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_HOLY) mod = 50;
	//v1.1.84 �������ɏ������Ă�Ƃ����j�ׂɎキ����
	if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH) mod = 66;

	if (p_ptr->mimic_form == MIMIC_GOD_OF_NEW_WORLD)
		mod = -100;

	///mod140219 �Í��A����̈悾�Ɣj�ׂɎキ�Ȃ�
	///mod150822 ���������A�����J����
	if(p_ptr->pclass != CLASS_MERLIN && p_ptr->pclass != CLASS_LYRICA)
	{
		if(p_ptr->realm1 == TV_BOOK_DARKNESS || p_ptr->realm1 == TV_BOOK_NECROMANCY) mod += 33;
		else if(p_ptr->realm2 == TV_BOOK_DARKNESS || p_ptr->realm2 == TV_BOOK_NECROMANCY) mod += 25;
	}

	//���ꏈ��
	if(p_ptr->tim_unite_darkness) mod += 50;

	if(mod >= 99) mod = 100;

/*
	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:�_����_�̔���l����������");
	}
*/
	return (mod);
}

///mod140115 �򉻎�_�̓x�������ʁ@�_���Ɠ������[�`���@

int mod_disen_dam(int dam){
	int base = 100;

	base += is_hurt_disen();



	if(p_ptr->resist_disen)
	{
	base *= 6;
	base /= (randint1(4) + 7);
	}



	//if(base > 100) msg_print("�̂���͂�������C������I");

	dam = dam * base / 100;

	return (dam);
}

int is_hurt_disen(){
	int mod = 0;

	if(prace_is_(RACE_TSUKUMO)) mod = 33;
	if(prace_is_(RACE_ANDROID)) mod = 33;
	if(prace_is_(RACE_GOLEM)) mod = 33;
	if(prace_is_(RACE_MAGIC_JIZO)) mod = 33;
	if(p_ptr->pclass == CLASS_ALICE) mod = 33;
	if (p_ptr->pclass == CLASS_MISUMARU) mod = 33;

	if (p_ptr->pclass == CLASS_TSUKASA) mod = 25;


	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->lev > 39) mod = 0;
	if(p_ptr->mimic_form == MIMIC_MIST) mod = 33;



	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:�򉻎�_�̔���l����������");
	}

	return (mod);
}


///mod140122 �M����_�̓x�������ʁ@�@

int mod_lite_dam(int dam){
	int base = 100;

	base += is_hurt_lite();

	if(p_ptr->resist_lite)
	{
	base *= 4;
	base /= (randint1(4) + 7);
	}

	//if(base > 100) msg_print("�����������Ȃ��̓��̂��ł������I");

	dam = dam * base / 100;

	return (dam);
}

int is_hurt_lite(){
	int mod = 0;


	if(p_ptr->pclass ==  CLASS_RUMIA) mod = 33;
	if(p_ptr->pclass ==  CLASS_NUE) mod = 50;
	if(p_ptr->muta3 & MUT3_HURT_LITE) mod = 33;
	if(prace_is_(RACE_VAMPIRE)) mod = 50;
	if (prace_is_(RACE_ANIMAL_GHOST) && p_ptr->pclass != CLASS_SAKI) mod = 50;
	if(prace_is_(RACE_SPECTRE) && !CLASS_IS_PRISM_SISTERS) mod = 50;
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 44) mod = -100;//���z�_

	if (p_ptr->wraith_form) mod = 100;
	else if(p_ptr->mimic_form == MIMIC_MIST) mod = 33;
	else if(p_ptr->mimic_form == MIMIC_VAMPIRE) mod = 50;

	if(p_ptr->tim_unite_darkness) mod += 50;
	if(p_ptr->pclass == CLASS_YUYUKO) mod = 0; //���ꏈ��

	if(p_ptr->pclass == CLASS_UTSUHO && p_ptr->lev > 44) mod = -100; //���ꏈ��
	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_LITE) mod = 50;

	//v1.1.37
	if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI) mod = 50;

	/*
	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:�M����_�̔���l����������");
	}
	*/

	return (mod);
}

///mod140126 ����_�̓x�������ʂƑϐ�����
int mod_water_dam(int dam){
	int base = 100;

	base += is_hurt_water();

	if(p_ptr->resist_water)
	{
	base /= 2;
	}

	//if(base > 100) msg_print("���͋�肾�I");

	dam = dam * base / 100;

	//if(dam<1) dam=1;
	return (dam);
}

int is_hurt_water(){
	int mod = 0;

	if(p_ptr->mimic_form == MIMIC_VAMPIRE) mod = 50;
	if(prace_is_(RACE_VAMPIRE)) mod = 50;
	if(p_ptr->pclass == CLASS_CHEN) mod = 33;

	//���Ɖu
	if(p_ptr->pclass == CLASS_WAKASAGI && p_ptr->lev > 19) mod = -100;
	if(p_ptr->pclass == CLASS_MURASA) mod = -100;
	//�C�_
	if(prace_is_(RACE_STRAYGOD) &&  p_ptr->race_multipur_val[3] == 47) mod = -100;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_WATER) mod = 50;

	if(mod>100 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:����_�̔���l����������");
	}

	return (mod);
}



///mod140302 �Ή���_�̓x�������ʂƑϐ����� �Œ�1�͎󂯂�悤�ɂ��Ă����@�Ɖu�͕ʏ���
int mod_fire_dam(int dam){
	int base = 100;

	base += is_hurt_fire();
	if (p_ptr->resist_fire) base /= 3;
	if (IS_OPPOSE_FIRE()) base /= 3;
	//if(base > 100) msg_print("���Ȃ��͔̑̂R���オ�����I");
	dam = dam * base / 100;

	if(dam<1) dam=1;
	return (dam);
}

int is_hurt_fire(){
	int mod = 0;

	if(p_ptr->pclass == CLASS_KOGASA) mod = 33;
	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->pseikaku == SEIKAKU_GAMAN && p_ptr->lev > 34) mod = 0;
	if(p_ptr->pclass == CLASS_CIRNO)
	{
		//v1.1.32
		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO)) mod = 0;
		else if(p_ptr->lev > 29) mod = 25; 
		else mod = 50;
	}

	if(p_ptr->pclass == CLASS_LETTY) mod = 33;
	if(p_ptr->pclass == CLASS_KAGEROU) mod = 25;

	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;
	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_FIRE) mod = 50;

	//v1.1.37
	if(p_ptr->mimic_form == MIMIC_KOSUZU_HYAKKI) mod = 50;


	if(mod>200 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:�Ή���_�̔���l����������");
	}

	return (mod);
}

///mod140302 ��C��_�̓x�������ʂƑϐ����� �Œ�1�͎󂯂�悤�ɂ��Ă����@�Ɖu�͕ʏ���
int mod_cold_dam(int dam){
	int base = 100;

	base += is_hurt_cold();
	if (p_ptr->resist_cold) base /= 3;
	if (IS_OPPOSE_COLD()) base /= 3;
	//if(base > 100) msg_print("���Ȃ��݂̑̂͂�݂铀������I");
	dam = dam * base / 100;

	if(dam<1) dam=1;
	return (dam);
}

int is_hurt_cold(){
	int mod = 0;

	if (p_ptr->pclass == CLASS_WRIGGLE) mod += 33;
	if (p_ptr->pclass == CLASS_SHIZUHA) mod += 25;
	if (p_ptr->pclass == CLASS_MINORIKO) mod += 25;
	if (p_ptr->pclass == CLASS_LARVA) mod += 33;

	if (IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_COLD) mod += 50;
	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;

	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;


	if (mod > 200) mod = 200;
	if (mod < -100) mod = -100;

	return (mod);
}

///mod140302 �d����_�̓x�������ʂƑϐ����� �Œ�1�͎󂯂�悤�ɂ��Ă����@�Ɖu�͕ʏ���
int mod_elec_dam(int dam){
	int base = 100;

	base += is_hurt_elec();
	if (p_ptr->resist_elec) base /= 3;
	if (IS_OPPOSE_ELEC()) base /= 3;
	//if(base > 100) msg_print("���Ȃ��̑̂�d�����삯�������I");
	dam = dam * base / 100;
	if(dam<1) dam=1;

	return (dam);
}

int is_hurt_elec(){
	int mod = 0;

	if(prace_is_(RACE_KAPPA)) mod = 33;



	if(p_ptr->pclass == CLASS_KAGEROU) mod += 25;
	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;
	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flagsr & RFR_HURT_ELEC) mod = 50;

	if(mod>200 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:�d����_�̔���l����������");
	}

	return (mod);
}

///mod140302 �_��_�̓x�������ʂƑϐ����� �Œ�1�͎󂯂�悤�ɂ��Ă����@�Ɖu�͕ʏ���
int mod_acid_dam(int dam){
	int base = 100;

	base += is_hurt_acid();
	if (p_ptr->resist_acid) base /= 3;
	if (IS_OPPOSE_ACID()) base /= 3;
//	if(base > 100) msg_print("�_�����Ȃ��̑̂��ł������I");
	dam = dam * base / 100;

	if(dam<1) dam=1;
	return (dam);
}

int is_hurt_acid(){
	int mod = 0;

	if(p_ptr->pclass == CLASS_KOGASA && p_ptr->lev < 30) mod = 33;


	if (p_ptr->special_defense & KATA_KOUKIJIN) mod += 33;
	if (p_ptr->muta3 & MUT3_VULN_ELEM) mod += 50;
	if (p_ptr->mimic_form == MIMIC_MIST) mod += 50;


	if(mod>200 || mod < -100)
	{
		mod = 0;
		msg_print("ERROR:�_��_�̔���l����������");
	}

	return (mod);
}






/*:::���C�ɑ΂���Ɖu�𔻒�*/
bool immune_insanity(void)
{
	if( p_ptr->pseikaku == SEIKAKU_BERSERK) return (TRUE);
	if(prace_is_(RACE_IMP)) return (TRUE);
	if(prace_is_(RACE_LICH)) return (TRUE);
	if(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON) return (TRUE);

	if(p_ptr->pclass == CLASS_CLOWNPIECE) return (TRUE);

	if(diehardmind()) return (TRUE);
	//�O�Ȃ�_
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 63) return (TRUE);


	 return (FALSE);
}

//�u�G�C�{���̖��̎ԗցv�̌��ʂ��؂��
void break_eibon_wheel(void)
{

	//v2.0.1 �Ђ��z�̃J�[�h���g���Ă���ꍇ���������ŉ�������
	if (p_ptr->tim_hirari_nuno) set_hirarinuno_card(0, TRUE);

	if(!(p_ptr->special_defense & (SD_EIBON_WHEEL))) return;

	//v1.1.33 ������̐Α��������̃��[�`���ɓ�������
	if(p_ptr->special_defense & (SD_STATUE_FORM))
		msg_print("���Ȃ��͐Α�����l�^�ɕς�����B");
	else
		msg_print("�����̓��������Ă��܂����I");

	p_ptr->special_defense &= ~(SD_EIBON_WHEEL | SD_STATUE_FORM);
	p_ptr->redraw |= (PR_STATUS);
	redraw_stuff();
}

//v1.1.44 ���ǂ񂰁u�C�r���A���W�����[�V�����v�̏����Bspecial_defense��3�r�b�g�g���B
//activate
//		TRUE�̂Ƃ��r�b�g��S�ė��Ă�B
//check
//		TRUE�̂Ƃ��A�r�b�g����ł������Ă���Έ���Ƃ���TRUE��Ԃ��B����Ȃ��ꍇFALSE��Ԃ��B
//
//�ǂ����FALSE�̂Ƃ��r�b�g��S�ė��Ƃ��B
bool evil_undulation(bool activate, bool check)
{

	if (activate)
	{
		msg_print("�َ��Ȕg�������Ȃ����񂾁B");
		p_ptr->special_defense |= EVIL_UNDULATION_MASK;
	}
	else if (check)
	{
		if (p_ptr->special_defense & EVIL_UNDULATION_MASK)
		{
			msg_print("��ǔg�������Ȃ���������I");

			if (p_ptr->special_defense & SD_EVIL_UNDULATION_1)
				p_ptr->special_defense &= ~(SD_EVIL_UNDULATION_1);
			else if (p_ptr->special_defense & SD_EVIL_UNDULATION_2)
				p_ptr->special_defense &= ~(SD_EVIL_UNDULATION_2);
			else
			{
				msg_print("���Ȃ������g�����������B");
				p_ptr->special_defense &= ~(EVIL_UNDULATION_MASK);
			}

		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		msg_print("���Ȃ������g�����������B");
		p_ptr->special_defense &= ~(EVIL_UNDULATION_MASK);

	}
	p_ptr->redraw |= PR_STATUS;
	return TRUE;

}

//v1.1.56 �A�C�e���u�X�y���J�[�h�v�ł̎��������ʏ���
bool set_tim_spellcard_effect(int v, bool do_dec, int art_idx)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->spellcard_effect_idx == art_idx)
		{
			//�����J�[�h�̂Ƃ����ʎ��Ԃ��Z���Ȃ邱�Ƃ͂Ȃ�
			if (p_ptr->tim_spellcard_count >= v) return FALSE;
		}
		else if (!do_dec)
		{
			//�Â����ʂ�����ꍇ���ꂪ��������郁�b�Z�[�W
			if (p_ptr->tim_spellcard_count)
				msg_format("%s", msg_tim_nameless_arts(p_ptr->spellcard_effect_idx,0, FALSE));
			//�V�������ʂ̔������b�Z�[�W
			msg_format("%s", msg_tim_nameless_arts(art_idx, 0,TRUE));
			notice = TRUE;

			//���N�����Ă�����ʂ�idx
			p_ptr->spellcard_effect_idx = art_idx;
		}
		//do_dec�̂Ƃ��͉��̃��b�Z�[�W���o���J�E���g�l�����邾��
	}

	/* Shut */
	else
	{
		if (p_ptr->spellcard_effect_idx)
		{
			//�Â����ʂ���������郁�b�Z�[�W
			msg_format("%s", msg_tim_nameless_arts(p_ptr->spellcard_effect_idx,0, FALSE));
			p_ptr->spellcard_effect_idx = 0;
			notice = TRUE;
		}
	}


	//�J�E���^�l
	p_ptr->tim_spellcard_count = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	p_ptr->update |= (PU_TORCH);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

//v1.1.71	�A�C�e���u�`���E�Z���A�T�K�I�̗t�v�̎����ɍ��킹�Ĉ݂̑|�����[�`����Ɨ�
bool throw_up(void)
{

	int food = p_ptr->food;

	//�őϐ�������ƌ��ʂ��Ȃ����Ƃɂ���
	if (p_ptr->resist_pois || IS_OPPOSE_POIS()) return FALSE;
	//��ǐ_�l�͕]����������
	set_deity_bias(DBIAS_REPUTATION, -3);

	msg_format("���I�H");
	msg_format("");
	screen_save();
	Term_clear();
	put_str("���΂炭���҂��������B", 15, 30);
	(void)inkey();
	//Term_xtra(TERM_XTRA_DELAY, 2000);
	screen_load();
	msg_format("�݂̒�����ɂȂ����B");

	if (food >= PY_FOOD_ALERT) set_food(PY_FOOD_ALERT-2);//�t���ς��̂��̂Ŗ����x��1������̂�-2�ɂ��Ƃ�
	else set_food(food / 2);
	set_alcohol(p_ptr->alcohol - 2000);

	set_poisoned(0);

	return TRUE;

}

//v1.1.78 ��������
void	check_suiken(void)
{
	bool flag_ok;

	if (p_ptr->alcohol >= DRANK_2) flag_ok = TRUE;//�u�ō��̋C�����I�v�ȏ�̂Ƃ�
	else flag_ok = FALSE;

	//�i����Ԃ̂Ƃ��̂�
	if (!p_ptr->do_martialarts) flag_ok = FALSE;

	if (ref_skill_exp(SKILL_MARTIALARTS) < 3200) flag_ok = FALSE;

	//�����ăm�b�N�_�E�����͐�����~
	if (!(p_ptr->muta2 & MUT2_ALCOHOL) && p_ptr->alcohol >= DRANK_4) flag_ok = FALSE;

	//�g����悤�ɂȂ�����
	if (flag_ok && !(p_ptr->special_attack & ATTACK_SUIKEN))
	{
		if(p_ptr->pclass == CLASS_REIMU)
			msg_print("���Ȃ��͔��헬���z�����̍\�����Ƃ����I");
		else if(one_in_(2))msg_print("���Ȃ��͐����̍\�����Ƃ����I");
		else msg_print("���Ȃ��͖��K���ɑ̂�h�炵�n�߂�...");

		p_ptr->special_attack |= ATTACK_SUIKEN;
		p_ptr->redraw |= PR_STATUS;

	}
	//�g���Ȃ��Ȃ�����
	else if (!flag_ok && (p_ptr->special_attack & ATTACK_SUIKEN))
	{
		msg_print("���Ȃ��͐�������߂��B");
		p_ptr->special_attack &= ~(ATTACK_SUIKEN);
		p_ptr->redraw |= PR_STATUS;

	}

}






//v2.0.1 �A�r���e�B�J�[�h�̂Ђ��z(���ד��Z�̂Ђ��z�ł͂Ȃ�)
//�J�E���g�_�E���̂ق��G�C�{���̖��̎ԗ�(break_eibon_wheel())�̂悤�Ɉړ���U���Ȃǂŉ��������
//�_���[�W�ł͉�������Ȃ�
bool set_hirarinuno_card(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->tim_hirari_nuno && !do_dec)
		{
			if (p_ptr->tim_hirari_nuno > v) return FALSE;
		}
		else
		{
			msg_print("���Ȃ��͕z�����Ԃ��Ďp���B�����B");

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_hirari_nuno)
		{
			msg_print("���Ȃ��͕z����o�Ďp���������B");

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_hirari_nuno = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



//�ڑ����
//�����ڑ���㩏�ԂɂȂ�ƗאڍU�����Ă��������X�^�[�ɔ���Ŗ��E����
bool set_tim_transportation_trap(int v, bool do_dec)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	/* Open */
	if (v)
	{
		if (p_ptr->transportation_trap && !do_dec)
		{
			if (p_ptr->transportation_trap>v) return FALSE;
		}
		else if (!p_ptr->transportation_trap)
		{
			msg_print("�ڑ���㩂����������B");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->transportation_trap)
		{
			msg_print("�ڑ���㩂̔������~�܂����B");

			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->transportation_trap = v;

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

