/* File: monster1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: describe monsters (using monster memory) */

#include "angband.h"


/*
 * Pronoun arrays, by gender.
 */
static cptr wd_he[4] =
#ifdef JP
{ "����", "��", "�ޏ�","�ޏ�����" };
#else
{ "it", "he", "she" };
#endif

static cptr wd_his[4] =
#ifdef JP
{ "�����", "�ނ�", "�ޏ���", "�ޏ�������" };
#else
{ "its", "his", "her" };
#endif



/*
 * Pluralizer.  Args(count, singular, plural)
 */
#define plural(c,s,p) \
    (((c) == 1) ? (s) : (p))






/*
 * Determine if the "armor" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_armour(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b kills = r_ptr->r_tkills;

	if (cheat_know) return (TRUE);

	/* Normal monsters */
	if (kills > 304 / (4 + level)) return (TRUE);

	/* Skip non-uniques */
	if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

	/* Unique monsters */
	if (kills > 304 / (38 + (5 * level) / 4)) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Determine if the "damage" of the given attack is known
 * the higher the level of the monster, the fewer the attacks you need,
 * the more damage an attack does, the more attacks you need
 */
static bool know_damage(int r_idx, int i)
{
	monster_race *r_ptr = &r_info[r_idx];

	s32b level = r_ptr->level;

	s32b a = r_ptr->r_blows[i];

	s32b d1 = r_ptr->blow[i].d_dice;
	s32b d2 = r_ptr->blow[i].d_side;

	s32b d = d1 * d2;

	if (d >= ((4+level)*MAX_UCHAR)/80) d = ((4+level)*MAX_UCHAR-1)/80;

	/* Normal monsters */
	if ((4 + level) * a > 80 * d) return (TRUE);

	/* Skip non-uniques */
	if (!(r_ptr->flags1 & RF1_UNIQUE)) return (FALSE);

	/* Unique monsters */
	if ((4 + level) * (2 * a) > 80 * d) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Prepare hook for c_roff(). It will be changed for spoiler generation in wizard1.c.
 */
void (*hook_c_roff)(byte attr, cptr str) = c_roff;

static void hooked_roff(cptr str)
{
	/* Spawn */
	hook_c_roff(TERM_WHITE, str);
}


/*
 * Hack -- display monster information using "hooked_roff()"
 *
 * This function should only be called with the cursor placed at the
 * left edge of the screen, on a cleared line, in which the recall is
 * to take place.  One extra blank line is left after the recall.
 */
/*:::�����X�^�[�̎v���o��\������*/
/*:::mode:0x03�̂Ƃ��E�B�U�[�h�R�}���h����̃X�|�C���[�����@�S�Ẵt���O��\�����邪�����|��������|���ꂽ���Ȃǂ͏������o�͐�̓t�@�C��*/
///mon �v���o�\���@�t���O�����Ȃ�

static void roff_aux(int r_idx, int mode)
{
	monster_race    *r_ptr = &r_info[r_idx];

	bool            old = FALSE;

	int             m, n, r;

	cptr            p, q;

#ifdef JP
	char            jverb_buf[64];
#else
	bool            sin = FALSE;
#endif
	int             msex = 0;

	bool nightmare = ironman_nightmare && !(mode & 0x02);
	int speed = nightmare ? r_ptr->speed + 5 : r_ptr->speed;

	bool            breath = FALSE;
	bool            magic = FALSE;

	u32b		flags1;
	u32b		flags2;
	u32b		flags3;
	u32b		flags4;
	u32b		flags5;
	u32b		flags6;
	u32b		flags7;
	u32b		flags9;

	u32b		flagsr;

	byte drop_gold, drop_item;

	int		vn = 0;
	//byte		color[96];
	//cptr		vp[96];

	///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ���flags9�ǉ� ���@�� 96��128
	byte		color[128];
	cptr		vp[128];

	bool know_everything = FALSE;

	/* Obtain a copy of the "known" number of drops */
	drop_gold = r_ptr->r_drop_gold;
	drop_item = r_ptr->r_drop_item;

	/* Obtain a copy of the "known" flags */
	/*:::���́u�m���Ă���v�����X�^�[�t���O�𓾂�*/
	flags1 = (r_ptr->flags1 & r_ptr->r_flags1);
	flags2 = (r_ptr->flags2 & r_ptr->r_flags2);
	flags3 = (r_ptr->flags3 & r_ptr->r_flags3);
	flags4 = (r_ptr->flags4 & r_ptr->r_flags4);
	flags5 = (r_ptr->flags5 & r_ptr->r_flags5);
	flags6 = (r_ptr->flags6 & r_ptr->r_flags6);
	/*:::flags7�͋����ł���Ȃǁu����΂킩��v�t���O*/
	flags7 = (r_ptr->flags7 & r_ptr->flags7);

	///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ���flags9�ǉ�
	flags9 = (r_ptr->flags9 & r_ptr->r_flags9);
	flagsr = (r_ptr->flagsr & r_ptr->r_flagsr);

	/* cheat_know or research_mon() */
	if (cheat_know || (mode & 0x01))
		know_everything = TRUE;

	/* Cheat -- Know everything */
	if (know_everything)
	{
		/* Hack -- maximal drops */
		drop_gold = drop_item =
		(((r_ptr->flags1 & RF1_DROP_4D2) ? 8 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_3D2) ? 6 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_2D2) ? 4 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_1D2) ? 2 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_90)  ? 1 : 0) +
		 ((r_ptr->flags1 & RF1_DROP_60)  ? 1 : 0));

		/* Hack -- but only "valid" drops */
		if (r_ptr->flags1 & RF1_ONLY_GOLD) drop_item = 0;
		if (r_ptr->flags1 & RF1_ONLY_ITEM) drop_gold = 0;

		/* Hack -- know all the flags */
		flags1 = r_ptr->flags1;
		flags2 = r_ptr->flags2;
		flags3 = r_ptr->flags3;
		flags4 = r_ptr->flags4;
		flags5 = r_ptr->flags5;
		flags6 = r_ptr->flags6;
	///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ���flags9�ǉ�
		flags9 = r_ptr->flags9;

		flagsr = r_ptr->flagsr;
	}


	///mod150731 �ށA�ޏ��Ȃǂ̕����@�O�d���̂݁u�ޏ������v��3�ɂ���
	/* Extract a gender (if applicable) */
	if(r_idx == MON_3FAIRIES) msex = 3;
	else if (r_ptr->flags1 & RF1_FEMALE) msex = 2;
	else if (r_ptr->flags1 & RF1_MALE) msex = 1;

	/* Assume some "obvious" flags */
	if (r_ptr->flags1 & RF1_UNIQUE)  flags1 |= (RF1_UNIQUE);
	if (r_ptr->flags1 & RF1_QUESTOR) flags1 |= (RF1_QUESTOR);
	if (r_ptr->flags1 & RF1_MALE)    flags1 |= (RF1_MALE);
	if (r_ptr->flags1 & RF1_FEMALE)  flags1 |= (RF1_FEMALE);

	/* Assume some "creation" flags */
	if (r_ptr->flags1 & RF1_FRIENDS) flags1 |= (RF1_FRIENDS);
	if (r_ptr->flags1 & RF1_ESCORT)  flags1 |= (RF1_ESCORT);
	if (r_ptr->flags1 & RF1_ESCORTS) flags1 |= (RF1_ESCORTS);

	/* Killing a monster reveals some properties */
	if (r_ptr->r_tkills || know_everything)
	{
		/* Know "race" flags */
		///mon �t���O
		///mod131231 �����X�^�[�t���O�ύX
		if (r_ptr->flags3 & RF3_KWAI)      flags3 |= (RF3_KWAI);
		if (r_ptr->flags3 & RF3_DEITY)    flags3 |= (RF3_DEITY);
		if (r_ptr->flags3 & RF3_DEMIHUMAN)    flags3 |= (RF3_DEMIHUMAN);
		if (r_ptr->flags3 & RF3_DRAGON)   flags3 |= (RF3_DRAGON);
		if (r_ptr->flags3 & RF3_DEMON)    flags3 |= (RF3_DEMON);
		if (r_ptr->flags3 & RF3_UNDEAD)   flags3 |= (RF3_UNDEAD);
		if (r_ptr->flags3 & RF3_ANG_CHAOS)     flags3 |= (RF3_ANG_CHAOS);
		if (r_ptr->flags3 & RF3_ANG_COSMOS)     flags3 |= (RF3_ANG_COSMOS);
		if (r_ptr->flags3 & RF3_ANIMAL)   flags3 |= (RF3_ANIMAL);
		if (r_ptr->flags3 & RF3_ANG_AMBER) flags3 |= (RF3_ANG_AMBER);
//		if (r_ptr->flags2 & RF2_HUMAN)    flags2 |= (RF2_HUMAN);
		if (r_ptr->flags3 & RF3_HUMAN)    flags3 |= (RF3_HUMAN);

		/* Know 'quantum' flag */
		if (r_ptr->flags2 & RF2_QUANTUM)  flags2 |= (RF2_QUANTUM);

		/* Know "forced" flags */
		if (r_ptr->flags1 & RF1_FORCE_DEPTH) flags1 |= (RF1_FORCE_DEPTH);
		if (r_ptr->flags1 & RF1_FORCE_MAXHP) flags1 |= (RF1_FORCE_MAXHP);
	}

	/* For output_monster_spoiler() */
	if (mode & 0x02)
	{
		/* Nothing to do */
	}
	else if(r_idx == MON_EXTRA_FIELD)
	{
		hooked_roff("���Ȃ��͂��܂��̎p�ɂȂ��Ă���B");
		hooked_roff("\n");
	}

	/* Treat uniques differently */
	else if (flags1 & RF1_UNIQUE)
	{
		/* Hack -- Determine if the unique is "dead" */
		bool dead = (r_ptr->max_num == 0) ? TRUE : FALSE;

		///msg140101
		/* We've been killed... */
		if (r_ptr->r_deaths)
		{
			/* Killed ancestors */
#ifdef JP
			hooked_roff(format("%^s�͂��Ȃ����ߋ��� %d ��|���Ă���",
					   wd_he[msex], r_ptr->r_deaths));
#else
			hooked_roff(format("%^s has slain %d of your ancestors",
					   wd_he[msex], r_ptr->r_deaths));
#endif


			/* But we've also killed it */
			if (dead)
			{
#ifdef JP
				hooked_roff(format("���A���łɎd�Ԃ��͍ς܂��Ă���B"));
#else
				hooked_roff(format(", but you have avenged %s!  ",
					    plural(r_ptr->r_deaths, "him", "them")));
#endif

			}

			/* Unavenged (ever) */
			else
			{
#ifdef JP
				hooked_roff(format("�B�d�Ԃ��͂܂����B"));
#else
				hooked_roff(format(", who %s unavenged.  ",
					    plural(r_ptr->r_deaths, "remains", "remain")));
#endif

			}

			/* Start a new line */
			hooked_roff("\n");
		}

		/* Dead unique who never hurt us */
		else if (dead)
		{
#ifdef JP
			hooked_roff("���Ȃ��͂��̋w�G�����łɓ|���Ă���B");
#else
			hooked_roff("You have slain this foe.  ");
#endif

			/* Start a new line */
			hooked_roff("\n");
		}
	}

	/* Not unique, but killed us */
	else if (r_ptr->r_deaths)
	{
		/* Dead ancestors */
#ifdef JP
		hooked_roff(format("���̃����X�^�[�͂��Ȃ����ߋ��� %d ��|���Ă���",
			    r_ptr->r_deaths ));
#else
		hooked_roff(format("%d of your ancestors %s been killed by this creature, ",
			    r_ptr->r_deaths, plural(r_ptr->r_deaths, "has", "have")));
#endif


		/* Some kills this life */
		if (r_ptr->r_pkills)
		{
#ifdef JP
			hooked_roff(format("���A���Ȃ��͂��̃����X�^�[�����Ȃ��Ƃ� %d �͓̂|���Ă���B", r_ptr->r_pkills));
#else
			hooked_roff(format("and you have exterminated at least %d of the creatures.  ", r_ptr->r_pkills));
#endif

		}

		/* Some kills past lives */
		else if (r_ptr->r_tkills)
		{
#ifdef JP
			hooked_roff(format("���A���Ȃ��͉ߋ��ɂ��̃����X�^�[�����Ȃ��Ƃ� %d ��͓|���Ă���B", r_ptr->r_tkills));
#else
			hooked_roff(format("and %s have exterminated at least %d of the creatures.  ",
				    "your ancestors", r_ptr->r_tkills));
#endif

		}

		/* No kills */
		else
		{
#ifdef JP
			hooked_roff(format("���A�܂�%s��|�������Ƃ͂Ȃ��B", wd_he[msex]));
#else
			hooked_roff(format("and %s is not ever known to have been defeated.  ", wd_he[msex]));
#endif

		}

		/* Start a new line */
		hooked_roff("\n");
	}

	/* Normal monsters */
	else
	{
		/* Killed some this life */
		if (r_ptr->r_pkills)
		{
#ifdef JP
			hooked_roff(format("���Ȃ��͂��̃����X�^�[�����Ȃ��Ƃ� %d �͓̂|���Ă���B", r_ptr->r_pkills));
#else
			hooked_roff(format("You have killed at least %d of these creatures.  ", r_ptr->r_pkills));
#endif

		}

		/* Killed some last life */
		else if (r_ptr->r_tkills)
		{
#ifdef JP
			hooked_roff(format("���Ȃ��͉ߋ��ɂ��̃����X�^�[�����Ȃ��Ƃ� %d �͓̂|���Ă���B", r_ptr->r_tkills));
#else
			hooked_roff(format("Your ancestors have killed at least %d of these creatures.  ", r_ptr->r_tkills));
#endif

		}

		/* Killed none */
		else
		{
#ifdef JP
			hooked_roff("���̃����X�^�[��|�������Ƃ͂Ȃ��B");
#else
			hooked_roff("No battles to the death are recalled.  ");
#endif
		}

		/* Start a new line */
		hooked_roff("\n");
	}
	///tmp
	//return;
	/* Descriptions */
	{
		/*:::�����X�^�[�̉������\��*/
		cptr tmp = r_text + r_ptr->text;

		if (tmp[0])
		{
			/* Dump it */
			hooked_roff(tmp);

			/* Start a new line */
			hooked_roff("\n");
		}
	}

	if (r_idx == MON_KAGE)
	{
		/* All done */
		hooked_roff("\n");

		return;
	}

	/* Nothing yet */
	old = FALSE;

	/* Describe location */
	if (r_ptr->level == 0)
	{
#ifdef JP
		hooked_roff(format("%^s�͒��ɏZ��", wd_he[msex]));
#else
		hooked_roff(format("%^s lives in the town", wd_he[msex]));
#endif

		old = TRUE;
	}
	else if (r_ptr->r_tkills || know_everything)
	{
		if (depth_in_feet)
		{
#ifdef JP
			hooked_roff(format("%^s�͒ʏ�n�� %d �t�B�[�g�ŏo����",
#else
			hooked_roff(format("%^s is normally found at depths of %d feet",
#endif

				    wd_he[msex], r_ptr->level * 50));
		}
		else
		{
#ifdef JP
			hooked_roff(format("%^s�͒ʏ�n�� %d �K�ŏo����",
#else
			hooked_roff(format("%^s is normally found on dungeon level %d",
#endif

				    wd_he[msex], r_ptr->level));
		}
		old = TRUE;
	}


	/* Describe movement */
	///mon �J�����I���̃����X�^�[�v���o�\��
	if (r_idx == MON_CHAMELEON)
	{
#ifdef JP
		hooked_roff("�A���̃����X�^�[�ɉ�����B");
#else
		hooked_roff("and can take the shape of other monster.");
#endif
		return;
	}
	else
	{
		/* Introduction */
		if (old)
		{
#ifdef JP
			hooked_roff("�A");
#else
			hooked_roff(", and ");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^s��", wd_he[msex]));
#else
			hooked_roff(format("%^s ", wd_he[msex]));
#endif

			old = TRUE;
		}
#ifndef JP
		hooked_roff("moves");
#endif

		/* Random-ness */
		if ((flags1 & RF1_RAND_50) || (flags1 & RF1_RAND_25))
		{
			/* Adverb */
			if ((flags1 & RF1_RAND_50) && (flags1 & RF1_RAND_25))
			{
#ifdef JP
				hooked_roff("���Ȃ�");
#else
				hooked_roff(" extremely");
#endif

			}
			else if (flags1 & RF1_RAND_50)
			{
#ifdef JP
				hooked_roff("����");
#else
				hooked_roff(" somewhat");
#endif

			}
			else if (flags1 & RF1_RAND_25)
			{
#ifdef JP
				hooked_roff("���X");
#else
				hooked_roff(" a bit");
#endif

			}

			/* Adjective */
#ifdef JP
			hooked_roff("�s�K����");
#else
			hooked_roff(" erratically");
#endif


			/* Hack -- Occasional conjunction */
#ifdef JP
			if (speed != 110) hooked_roff("�A����");
#else
			if (speed != 110) hooked_roff(", and");
#endif

		}

		/* Speed */
		if (speed > 110)
		{
#ifdef JP
			if (speed > 139) hook_c_roff(TERM_RED, "�M����ق�");
			else if (speed > 134) hook_c_roff(TERM_ORANGE, "�җ��");
			else if (speed > 129) hook_c_roff(TERM_ORANGE, "����");
			else if (speed > 124) hook_c_roff(TERM_UMBER, "���Ȃ�");
			else if (speed < 120) hook_c_roff(TERM_L_UMBER, "���");
			hook_c_roff(TERM_L_RED, "�f����");
#else
			if (speed > 139) hook_c_roff(TERM_RED, " incredibly");
			else if (speed > 134) hook_c_roff(TERM_ORANGE, " extremely");
			else if (speed > 129) hook_c_roff(TERM_ORANGE, " very");
			else if (speed > 124) hook_c_roff(TERM_UMBER, " fairly");
			else if (speed < 120) hook_c_roff(TERM_L_UMBER, " somewhat");
			hook_c_roff(TERM_L_RED, " quickly");
#endif

		}
		else if (speed < 110)
		{
#ifdef JP
			if (speed < 90) hook_c_roff(TERM_L_GREEN, "�M����ق�");
			else if (speed < 95) hook_c_roff(TERM_BLUE, "����");
			else if (speed < 100) hook_c_roff(TERM_BLUE, "���Ȃ�");
			else if (speed > 104) hook_c_roff(TERM_GREEN, "���");
			hook_c_roff(TERM_L_BLUE, "��������");
#else
			if (speed < 90) hook_c_roff(TERM_L_GREEN, " incredibly");
			else if (speed < 95) hook_c_roff(TERM_BLUE, " very");
			else if (speed < 100) hook_c_roff(TERM_BLUE, " fairly");
			else if (speed > 104) hook_c_roff(TERM_GREEN, " somewhat");
			hook_c_roff(TERM_L_BLUE, " slowly");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff("���ʂ̑�����");
#else
			hooked_roff(" at normal speed");
#endif

		}
#ifdef JP
		hooked_roff("�����Ă���");
#endif
	}

	/* The code above includes "attack speed" */
	if (flags1 & RF1_NEVER_MOVE)
	{
		/* Introduce */
		if (old)
		{
#ifdef JP
			hooked_roff("�A������");
#else
			hooked_roff(", but ");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^s��", wd_he[msex]));
#else
			hooked_roff(format("%^s ", wd_he[msex]));
#endif

			old = TRUE;
		}

		/* Describe */
#ifdef JP
		hooked_roff("�N���҂�ǐՂ��Ȃ�");
#else
		hooked_roff("does not deign to chase intruders");
#endif

	}

	/* End this sentence */
	if (old)
	{
#ifdef JP
		hooked_roff("�B");
#else
		hooked_roff(".  ");
#endif

		old = FALSE;
	}


	/* Describe experience if known */
	///mod140719 �푰�Ȃǂ̏��͓|���Ȃ��Ă��\������悤�ɂ���
	//if (r_ptr->r_tkills || know_everything)
	{
		/* Introduction */
#ifdef JP
		hooked_roff("����");
#else
		if (flags1 & RF1_UNIQUE)
		{
			hooked_roff("Killing this");
		}
		else
		{
			hooked_roff("A kill of this");
		}
#endif


		/* Describe the "quality" */



#ifdef JP
if (flags3 & RF3_ANG_CHAOS)            hook_c_roff(TERM_L_DARK, "���ׂ̐��͂ɑ�����");
#else
		if (flags3 & RF3_EVIL)            hook_c_roff(TERM_L_DARK, " evil");
#endif

#ifdef JP
if (flags3 & RF3_ANG_COSMOS)            hook_c_roff(TERM_YELLOW, "�����̐��͂ɑ�����");
#else
		if (flags3 & RF3_GOOD)            hook_c_roff(TERM_YELLOW, " good");
#endif

/*:::�u���C��U���v�Ȃǂ̋L�q�͓|���Ȃ��Ă��\������悤�ɉ��Ɉڂ���*/


/*:::�������肵�Ȃ��̂ŉ��Ɉړ�����
#ifdef JP
if (flags3 & RF3_ANIMAL)          hook_c_roff(TERM_L_GREEN, "���R�E��");
#else
		if (flags3 & RF3_ANIMAL)          hook_c_roff(TERM_L_GREEN, " natural");
#endif
#ifdef JP
if (flags3 & RF3_UNDEAD)          hook_c_roff(TERM_VIOLET, "�A���f�b�h��");
#else
		if (flags3 & RF3_UNDEAD)          hook_c_roff(TERM_VIOLET, " undead");
#endif
*/
#ifdef JP
if (flags3 & RF3_ANG_AMBER)        hook_c_roff(TERM_VIOLET, "�A���o�[�̉�����");
#else
		if (flags3 & RF3_AMBERITE)        hook_c_roff(TERM_VIOLET, " Amberite");
#endif



	///mod131231 �����X�^�[�t���O�ύX
	if ((flags3 & (RF3_ANIMAL | RF3_DRAGON | RF3_DEMON | RF3_FAIRY | RF3_DEMIHUMAN | RF3_DEITY | RF3_KWAI | RF3_HUMAN | RF3_UNDEAD | RF3_HANIWA)) || (flags2 & (RF2_QUANTUM)))
	//if ((flags3 & (RF3_DRAGON | RF3_DEMON | RF3_GIANT | RF3_TROLL | RF3_ORC)) || (flags2 & (RF2_QUANTUM | RF2_HUMAN)))
	{
		bool flag_halfblood = FALSE;
	/* Describe the "race" */

     if (flags3 & RF3_DRAGON){
		 hook_c_roff(TERM_ORANGE, "�h���S��");
		flag_halfblood = TRUE;
	 }

if (flags3 & RF3_UNDEAD)
{
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_VIOLET, "�A���f�b�h");
}
if (flags3 & RF3_ANIMAL)
{
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_GREEN, "�b");
}

if (flags3 & RF3_DEMON)
{
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_VIOLET, "����");
}

#ifdef JP
if (flags3 & RF3_DEMIHUMAN)
{
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_UMBER, "�l�Ԍ^����");
}
#else
		if (flags3 & RF3_GIANT)    hook_c_roff(TERM_L_UMBER, " giant");
#endif

#ifdef JP
if (flags3 & RF3_DEITY)
{
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_BLUE, "�_�i");
}
#else
		if (flags3 & RF3_TROLL)    hook_c_roff(TERM_L_BLUE, " troll");
#endif

#ifdef JP
if (flags3 & RF3_KWAI){
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;	
	hook_c_roff(TERM_UMBER, "�d��");
}

#else
		if (flags3 & RF3_ORC)      hook_c_roff(TERM_UMBER, " orc");
#endif

#ifdef JP
if (flags3 & RF3_HUMAN)
{
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_L_WHITE, "�l��");
}
	
	//if (flags2 & RF2_HUMAN) hook_c_roff(TERM_L_WHITE, "�l��");
#else
		if (flags3 & RF3_HUMAN) hook_c_roff(TERM_L_WHITE, " human");
#endif

#ifdef JP
if (flags2 & RF2_QUANTUM){
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_VIOLET, "�ʎq����");
}
#else
		if (flags2 & RF2_QUANTUM)  hook_c_roff(TERM_VIOLET, " quantum creature");
#endif


#ifdef JP
if (flags3 & RF3_FAIRY){
	if(flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
	else 	flag_halfblood = TRUE;
	hook_c_roff(TERM_YELLOW, "�d��");
}
#else
		if (flags3 & RF3_FAIRY)    hook_c_roff(TERM_VIOLET, " fairy");
#endif

	if (flags3 & RF3_HANIWA) {
		if (flag_halfblood == TRUE) 	hooked_roff("�ɂ���");
		else 	flag_halfblood = TRUE;
		hook_c_roff(TERM_ORANGE, "����");
		}


	}
#ifdef JP
else                            hooked_roff("����");
#else
		else                            hooked_roff(" creature");
#endif


#ifdef JP
		hooked_roff("��|�����Ƃ�");
#endif
		/* Group some variables */
		{
			long i, j;

#ifdef JP
			i = p_ptr->lev;
			hooked_roff(format(" %lu ���x���̃L�����N�^�ɂƂ���", (long)i));

			i = (long)r_ptr->mexp * r_ptr->level / (p_ptr->max_plv+2);
			j = ((((long)r_ptr->mexp * r_ptr->level % (p_ptr->max_plv+2)) *
			       (long)1000 / (p_ptr->max_plv+2) + 5) / 10);

			hooked_roff(format(" ��%ld.%02ld �|�C���g�̌o���ƂȂ�B",
				(long)i, (long)j ));
#else
			/* calculate the integer exp part */
			i = (long)r_ptr->mexp * r_ptr->level / (p_ptr->max_plv+2);

			/* calculate the fractional exp part scaled by 100, */
			/* must use long arithmetic to avoid overflow  */
			j = ((((long)r_ptr->mexp * r_ptr->level % (p_ptr->max_plv+2)) *
			       (long)1000 / (p_ptr->max_plv+2) + 5) / 10);

			/* Mention the experience */
			hooked_roff(format(" is worth about %ld.%02ld point%s",
				    (long)i, (long)j,
				    (((i == 1) && (j == 0)) ? "" : "s")));

			/* Take account of annoying English */
			p = "th";
			i = p_ptr->lev % 10;
			if ((p_ptr->lev / 10) == 1) /* nothing */;
			else if (i == 1) p = "st";
			else if (i == 2) p = "nd";
			else if (i == 3) p = "rd";

			/* Take account of "leading vowels" in numbers */
			q = "";
			i = p_ptr->lev;
			if ((i == 8) || (i == 11) || (i == 18)) q = "n";

			/* Mention the dependance on the player's level */
			hooked_roff(format(" for a%s %lu%s level character.  ",
				    q, (long)i, p));
#endif

		}
	}
	///mod140608 ���Ă킩������͓|���Ȃ��Ă��\������悤�ɂ���
	if (flags7 & RF7_CAN_FLY || (r_ptr->flags2 & RF2_GIGANTIC) || (flags2 & RF2_POWERFUL) || (flags2 & RF2_ELDRITCH_HORROR))
	{
		/*:::�ŏ��Abool eld=(flags2 & RF2_ELDRITCH_HORROR)�݂����ɏ��������Ă�fly�ȊO����ɓ��삵�Ȃ������B*/
		/*:::�ǂ���bool�^�͓����I�ɂ�byte�^�ŁA���̂悤�ȏ�����������Ɩ�����8�o�C�g�������g���Ȃ��炵���B*/
		/*:::����܂łɍ�����R�[�h���S�z���B-140608-*/
		bool fly=FALSE,giga=FALSE,power=FALSE,eld=FALSE;
		if(flags7 & RF7_CAN_FLY) fly = TRUE;
		if(r_ptr->flags2 & RF2_GIGANTIC) giga = TRUE;
		if(flags2 & RF2_POWERFUL) power = TRUE;
		if(flags2 & RF2_ELDRITCH_HORROR) eld = TRUE;

		hook_c_roff(TERM_WHITE, format("%^s��", wd_he[msex]));
		if (giga)
		{
			if(!(fly || eld || power))hook_c_roff(TERM_ORANGE, "���傾�B");
			else hook_c_roff(TERM_ORANGE, "�����");

		}
		if (power)
		{
			if(!( fly || eld))hook_c_roff(TERM_RED, "�͋����B");
			else hook_c_roff(TERM_RED, "�͋���");
		}
		if (fly)
		{
			if(!eld)	hook_c_roff(TERM_L_BLUE,"�����ł���B");
			else	hook_c_roff(TERM_L_BLUE,"�����ł���");
		}
		if (eld)
		{
			hook_c_roff(TERM_VIOLET, "���C��U�����܂����p���B");
		}

	}



///mod131231 �����X�^�[�t���O�ύX
	if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC) && (flags2 & RF2_AURA_COLD))
	//if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC) && (flags3 & RF3_AURA_COLD))
	{
#ifdef JP
		hook_c_roff(TERM_VIOLET, format("%^s�͉��ƕX�ƃX�p�[�N�ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_VIOLET, format("%^s is surrounded by flames, ice and electricity.  ", wd_he[msex]));
#endif
	}
	else if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC))
	{
#ifdef JP
		hook_c_roff(TERM_L_RED, format("%^s�͉��ƃX�p�[�N�ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_L_RED, format("%^s is surrounded by flames and electricity.  ", wd_he[msex]));
#endif
	}
	else if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_COLD))
	//else if ((flags2 & RF2_AURA_FIRE) && (flags3 & RF3_AURA_COLD))
	{
#ifdef JP
		hook_c_roff(TERM_BLUE, format("%^s�͉��ƕX�ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_BLUE, format("%^s is surrounded by flames and ice.  ", wd_he[msex]));
#endif
	}
	//else if ((flags3 & RF3_AURA_COLD) && (flags2 & RF2_AURA_ELEC))
	else if ((flags2 & RF2_AURA_COLD) && (flags2 & RF2_AURA_ELEC))
	{
#ifdef JP
		hook_c_roff(TERM_L_GREEN, format("%^s�͕X�ƃX�p�[�N�ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_L_GREEN, format("%^s is surrounded by ice and electricity.  ", wd_he[msex]));
#endif
	}
	else if (flags2 & RF2_AURA_FIRE)
	{
#ifdef JP
		hook_c_roff(TERM_RED, format("%^s�͉��ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_RED, format("%^s is surrounded by flames.  ", wd_he[msex]));
#endif
	}
	//else if (flags3 & RF3_AURA_COLD)
	else if (flags2 & RF2_AURA_COLD)
	{
#ifdef JP
		hook_c_roff(TERM_BLUE, format("%^s�͕X�ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_BLUE, format("%^s is surrounded by ice.  ", wd_he[msex]));
#endif
	}
	else if (flags2 & RF2_AURA_ELEC)
	{
#ifdef JP
		hook_c_roff(TERM_L_BLUE, format("%^s�̓X�p�[�N�ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_L_BLUE, format("%^s is surrounded by electricity.  ", wd_he[msex]));
#endif
	}






	if (flags2 & RF2_REFLECTING)
	{
#ifdef JP
		hooked_roff(format("%^s�͖�̎����𒵂˕Ԃ��B", wd_he[msex]));
#else
		hooked_roff(format("%^s reflects bolt spells.  ", wd_he[msex]));
#endif

	}

	/* Describe escorts */
	if ((flags1 & RF1_ESCORT) || (flags1 & RF1_ESCORTS))
	{
#ifdef JP
		hooked_roff(format("%^s�͒ʏ��q�𔺂��Č����B",
#else
		hooked_roff(format("%^s usually appears with escorts.  ",
#endif

			    wd_he[msex]));
	}

	/* Describe friends */
	else if (flags1 & RF1_FRIENDS)
	{
#ifdef JP
		hooked_roff(format("%^s�͒ʏ�W�c�Ō����B",
#else
		hooked_roff(format("%^s usually appears in groups.  ",
#endif

			    wd_he[msex]));
	}


	/* Collect inate attacks */
	vn = 0;
#ifdef JP
	if (flags4 & RF4_SHRIEK)  {vp[vn] = "�ߖŏ��������߂�";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & RF4_SHRIEK)  {vp[vn] = "shriek for help";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flags4 & RF4_ROCKET)  {vp[vn] = "���P�b�g�𔭎˂���";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & RF4_ROCKET)  {vp[vn] = "shoot a rocket";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & RF4_WAVEMOTION)  {vp[vn] = "�g���C�𔭎˂���";color[vn++] = TERM_WHITE;}
#else
	if (flags4 & RF4_ROCKET)  {vp[vn] = "shoot a rocket";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & RF4_BA_FORCE) {vp[vn] = "�C������čU������";color[vn++] = TERM_YELLOW;}
#else
	if (flags4 & RF4_SHOOT) {vp[vn] = "fire an arrow";color[vn++] = TERM_UMBER;}
#endif
#ifdef JP
	if (flags4 & RF4_DANMAKU) {vp[vn] = "�e��������čU������";color[vn++] = TERM_RED;}
#else
	if (flags4 & RF4_SHOOT) {vp[vn] = "fire an arrow";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & RF4_SHOOT) {vp[vn] = "���u�U��������";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & RF4_SHOOT) {vp[vn] = "fire an arrow";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags6 & (RF6_SPECIAL) || flags4 & (RF4_SPECIAL2)) {vp[vn] = "���ʂȍs��������";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_SPECIAL)) {vp[vn] = "do something";color[vn++] = TERM_VIOLET;}
#endif

	/* Describe inate attacks */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^s��", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
#ifdef JP
			if (n != vn - 1)
			{
				jverb(vp[n], jverb_buf, JVERB_OR);
				hook_c_roff(color[n], jverb_buf);
				hook_c_roff(color[n], "��");
				hooked_roff("�A");
			}
			else hook_c_roff(color[n], vp[n]);
#else
			/* Intro */
			if (n == 0) hooked_roff(" may ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" or ");

			/* Dump */
			hook_c_roff(color[n], vp[n]);
#endif

		}

		/* End */
#ifdef JP
		hooked_roff("���Ƃ�����B");
#else
		hooked_roff(".  ");
#endif

	}


	/* Collect breaths */
	vn = 0;
#ifdef JP
	if (flags4 & (RF4_BR_ACID))		{vp[vn] = "�_";color[vn++] = TERM_GREEN;}
#else
	if (flags4 & (RF4_BR_ACID))		{vp[vn] = "acid";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_ELEC))		{vp[vn] = "���";color[vn++] = TERM_BLUE;}
#else
	if (flags4 & (RF4_BR_ELEC))		{vp[vn] = "lightning";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_FIRE))		{vp[vn] = "�Ή�";color[vn++] = TERM_RED;}
#else
	if (flags4 & (RF4_BR_FIRE))		{vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_COLD))		{vp[vn] = "��C";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & (RF4_BR_COLD))		{vp[vn] = "frost";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_POIS))		{vp[vn] = "��";color[vn++] = TERM_L_GREEN;}
#else
	if (flags4 & (RF4_BR_POIS))		{vp[vn] = "poison";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NETH))		{vp[vn] = "�n��";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_NETH))		{vp[vn] = "nether";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_LITE))		{vp[vn] = "�M��";color[vn++] = TERM_YELLOW;}
#else
	if (flags4 & (RF4_BR_LITE))		{vp[vn] = "light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DARK))		{vp[vn] = "�Í�";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_DARK))		{vp[vn] = "darkness";color[vn++] = TERM_L_DARK;}
#endif
#ifdef JP
	if (flags4 & (RF4_BR_HOLY))		{vp[vn] = "�j��";color[vn++] = TERM_WHITE;}
#else
	if (flags4 & (RF4_BR_CONF))		{vp[vn] = "confusion";color[vn++] = TERM_L_UMBER;}
#endif
#ifdef JP
	if (flags4 & (RF4_BR_HELL))		{vp[vn] = "�n���̋Ɖ�";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_CONF))		{vp[vn] = "confusion";color[vn++] = TERM_L_UMBER;}
#endif
#ifdef JP
	if (flags4 & (RF4_BR_SOUN))		{vp[vn] = "�Ռ��g";color[vn++] = TERM_ORANGE;}
#else
	if (flags4 & (RF4_BR_SOUN))		{vp[vn] = "sound";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_AQUA))		{vp[vn] = "��";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_SOUN))		{vp[vn] = "sound";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_CHAO))		{vp[vn] = "�J�I�X";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_CHAO))		{vp[vn] = "chaos";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DISE))		{vp[vn] = "��";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_DISE))		{vp[vn] = "disenchantment";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NEXU))		{vp[vn] = "���ʍ���";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_NEXU))		{vp[vn] = "nexus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_TIME))		{vp[vn] = "���ԋt�]";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_TIME))		{vp[vn] = "time";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_INER))		{vp[vn] = "�x��";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_INER))		{vp[vn] = "inertia";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_GRAV))		{vp[vn] = "�d��";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_GRAV))		{vp[vn] = "gravity";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_SHAR))		{vp[vn] = "�j��";color[vn++] = TERM_L_UMBER;}
#else
	if (flags4 & (RF4_BR_SHAR))		{vp[vn] = "shards";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_PLAS))		{vp[vn] = "�v���Y�}";color[vn++] = TERM_L_RED;}
#else
	if (flags4 & (RF4_BR_PLAS))		{vp[vn] = "plasma";color[vn++] = TERM_L_RED;}
#endif

	///del131231 �t�H�[�X�u���X�p�~
/*
#ifdef JP
	if (flags4 & (RF4_BR_WALL))		{vp[vn] = "�t�H�[�X";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & (RF4_BR_WALL))		{vp[vn] = "force";color[vn++] = TERM_UMBER;}
#endif
	*/
#ifdef JP
	if (flags4 & (RF4_BR_MANA))		{vp[vn] = "����";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_MANA))		{vp[vn] = "mana";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NUKE))		{vp[vn] = "�j�M";color[vn++] = TERM_L_RED;}
#else
	if (flags4 & (RF4_BR_NUKE))		{vp[vn] = "toxic waste";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DISI))		{vp[vn] = "����";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_DISI))		{vp[vn] = "disintegration";color[vn++] = TERM_SLATE;}
#endif


	/* Describe breaths */
	if (vn)
	{
		/* Note breath */
		breath = TRUE;

		/* Intro */
#ifdef JP
		hooked_roff(format("%^s��", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("��");
#else
			if (n == 0) hooked_roff(" may breathe ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}
#ifdef JP
		hooked_roff("�̃u���X��f�����Ƃ�����");
#endif
	}


	/* Collect spells */
	vn = 0;
#ifdef JP
if (flags5 & (RF5_BA_ACID))         {vp[vn] = "�A�V�b�h�E�{�[��";color[vn++] = TERM_GREEN;}
#else
	if (flags5 & (RF5_BA_ACID))         {vp[vn] = "produce acid balls";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_ELEC))         {vp[vn] = "�T���_�[�E�{�[��";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BA_ELEC))         {vp[vn] = "produce lightning balls";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_FIRE))         {vp[vn] = "�t�@�C�A�E�{�[��";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BA_FIRE))         {vp[vn] = "produce fire balls";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_COLD))         {vp[vn] = "�A�C�X�E�{�[��";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_BA_COLD))         {vp[vn] = "produce frost balls";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_POIS))         {vp[vn] = "���L�_";color[vn++] = TERM_L_GREEN;}
#else
	if (flags5 & (RF5_BA_POIS))         {vp[vn] = "produce poison balls";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_NETH))         {vp[vn] = "�n����";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BA_NETH))         {vp[vn] = "produce nether balls";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_WATE))         {vp[vn] = "�E�H�[�^�[�E�{�[��";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BA_WATE))         {vp[vn] = "produce water balls";color[vn++] = TERM_BLUE;}
#endif

///del ���˔\���p�~
/*
#ifdef JP
if (flags4 & (RF4_BA_NUKE))         {vp[vn] = "���˔\��";color[vn++] = TERM_L_GREEN;}
#else
	if (flags4 & (RF4_BA_NUKE))         {vp[vn] = "produce balls of radiation";color[vn++] = TERM_L_GREEN;}
#endif
*/
#ifdef JP
if (flags5 & (RF5_BA_MANA))         {vp[vn] = "���̗͂�";color[vn++] = TERM_L_BLUE;}
#else
	if (flags5 & (RF5_BA_MANA))         {vp[vn] = "invoke mana storms";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_DARK))         {vp[vn] = "�Í��̗�";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BA_DARK))         {vp[vn] = "invoke darkness storms";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_LITE))         {vp[vn] = "�M���̗�";color[vn++] = TERM_YELLOW;}
#else
	if (flags5 & (RF5_BA_LITE))         {vp[vn] = "invoke starburst";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags4 & (RF4_BA_CHAO))         {vp[vn] = "�����O���X";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BA_CHAO))         {vp[vn] = "invoke raw Logrus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_HAND_DOOM))       {vp[vn] = "�j�ł̎�";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_HAND_DOOM))       {vp[vn] = "invoke the Hand of Doom";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_PSY_SPEAR))            {vp[vn] = "���̌�";color[vn++] = TERM_YELLOW;}
#else
	if (flags6 & (RF6_PSY_SPEAR))            {vp[vn] = "psycho-spear";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags5 & (RF5_DRAIN_MANA))      {vp[vn] = "���͋z��";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_DRAIN_MANA))      {vp[vn] = "drain mana";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_MIND_BLAST))      {vp[vn] = "���_�U��";color[vn++] = TERM_L_RED;}
#else
	if (flags5 & (RF5_MIND_BLAST))      {vp[vn] = "cause mind blasting";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BRAIN_SMASH))     {vp[vn] = "�]�U��";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BRAIN_SMASH))     {vp[vn] = "cause brain smashing";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_1))         {vp[vn] = "�y���{��";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_1))         {vp[vn] = "cause light wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_2))         {vp[vn] = "�d���{��";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_2))         {vp[vn] = "cause serious wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_3))         {vp[vn] = "�v�����{��";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_3))         {vp[vn] = "cause critical wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_4))         {vp[vn] = "���̌���";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_4))         {vp[vn] = "cause mortal wounds";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ACID))         {vp[vn] = "�A�V�b�h�E�{���g";color[vn++] = TERM_GREEN;}
#else
	if (flags5 & (RF5_BO_ACID))         {vp[vn] = "produce acid bolts";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ELEC))         {vp[vn] = "�T���_�[�E�{���g";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BO_ELEC))         {vp[vn] = "produce lightning bolts";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_FIRE))         {vp[vn] = "�t�@�C�A�E�{���g";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BO_FIRE))         {vp[vn] = "produce fire bolts";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_COLD))         {vp[vn] = "�A�C�X�E�{���g";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_BO_COLD))         {vp[vn] = "produce frost bolts";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_NETH))         {vp[vn] = "�n���̖�";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BO_NETH))         {vp[vn] = "produce nether bolts";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_WATE))         {vp[vn] = "�E�H�[�^�[�E�{���g";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BO_WATE))         {vp[vn] = "produce water bolts";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_MANA))         {vp[vn] = "���̖͂�";color[vn++] = TERM_L_BLUE;}
#else
	if (flags5 & (RF5_BO_MANA))         {vp[vn] = "produce mana bolts";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_PLAS))         {vp[vn] = "�v���Y�}�E�{���g";color[vn++] = TERM_L_RED;}
#else
	if (flags5 & (RF5_BO_PLAS))         {vp[vn] = "produce plasma bolts";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ICEE))         {vp[vn] = "�Ɋ��̖�";color[vn++] = TERM_WHITE;}
#else
	if (flags5 & (RF5_BO_ICEE))         {vp[vn] = "produce ice bolts";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_MISSILE))         {vp[vn] = "�}�W�b�N�~�T�C��";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_MISSILE))         {vp[vn] = "produce magic missiles";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_SCARE))           {vp[vn] = "���|";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_SCARE))           {vp[vn] = "terrify";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_BLIND))           {vp[vn] = "�ڂ���܂�";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BLIND))           {vp[vn] = "blind";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_CONF))            {vp[vn] = "����";color[vn++] = TERM_L_UMBER;}
#else
	if (flags5 & (RF5_CONF))            {vp[vn] = "confuse";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags5 & (RF5_SLOW))            {vp[vn] = "����";color[vn++] = TERM_UMBER;}
#else
	if (flags5 & (RF5_SLOW))            {vp[vn] = "slow";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags5 & (RF5_HOLD))            {vp[vn] = "���";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_HOLD))            {vp[vn] = "paralyze";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_HASTE))           {vp[vn] = "����";color[vn++] = TERM_L_GREEN;}
#else
	if (flags6 & (RF6_HASTE))           {vp[vn] = "haste-self";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags6 & (RF6_HEAL))            {vp[vn] = "����";color[vn++] = TERM_WHITE;}
#else
	if (flags6 & (RF6_HEAL))            {vp[vn] = "heal-self";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
	if (flags6 & (RF6_INVULNER))        {vp[vn] = "���G��";color[vn++] = TERM_WHITE;}
#else
	if (flags6 & (RF6_INVULNER))        {vp[vn] = "make invulnerable";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
if (flags4 & RF4_DISPEL)    {vp[vn] = "���͏���";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & RF4_DISPEL)    {vp[vn] = "dispel-magic";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags6 & (RF6_BLINK))           {vp[vn] = "�V���[�g�e���|�[�g";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_BLINK))           {vp[vn] = "blink-self";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TPORT))           {vp[vn] = "�e���|�[�g";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_TPORT))           {vp[vn] = "teleport-self";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_WORLD))            {vp[vn] = "�����~�߂�";color[vn++] = TERM_L_BLUE;}
#else
	if (flags6 & (RF6_WORLD))            {vp[vn] = "stop the time";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_TO))         {vp[vn] = "�e���|�[�g�o�b�N";color[vn++] = TERM_L_UMBER;}
#else
	if (flags6 & (RF6_TELE_TO))         {vp[vn] = "teleport to";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_AWAY))       {vp[vn] = "�e���|�[�g�A�E�F�C";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_TELE_AWAY))       {vp[vn] = "teleport away";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_LEVEL))      {vp[vn] = "�e���|�[�g�E���x��";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_TELE_LEVEL))      {vp[vn] = "teleport level";color[vn++] = TERM_ORANGE;}
#endif

	if (flags6 & (RF6_DARKNESS))
	{
		if ((p_ptr->pclass != CLASS_NINJA) || (r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags7 & RF7_DARK_MASK) || (r_ptr->flagsr & RFR_HURT_LITE))
		{
#ifdef JP
			vp[vn] =  "�È�"; color[vn++] = TERM_L_DARK;
#else
			vp[vn] = "create darkness"; color[vn++] = TERM_L_DARK;
#endif
		}
		else
		{
#ifdef JP
			vp[vn] = "�M��"; color[vn++] = TERM_YELLOW;
#else
			vp[vn] = "create light"; color[vn++] = TERM_YELLOW;
#endif
		}
	}

#ifdef JP
if (flags6 & (RF6_TRAPS))           {vp[vn] = "�g���b�v";color[vn++] = TERM_BLUE;}
#else
	if (flags6 & (RF6_TRAPS))           {vp[vn] = "create traps";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags6 & (RF6_FORGET))          {vp[vn] = "�L������";color[vn++] = TERM_BLUE;}
#else
	if (flags6 & (RF6_FORGET))          {vp[vn] = "cause amnesia";color[vn++] = TERM_BLUE;}
#endif
///mod ���ҕ������R�Y�~�b�N�E�z���[

#ifdef JP
if (flags6 & (RF6_COSMIC_HORROR))      {vp[vn] = "�R�Y�~�b�N�E�z���[";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_RAISE_DEAD))      {vp[vn] = "raise dead";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_MONSTER))       {vp[vn] = "�����X�^�[��̏���";color[vn++] = TERM_SLATE;}
#else
	if (flags6 & (RF6_S_MONSTER))       {vp[vn] = "summon a monster";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_MONSTERS))      {vp[vn] = "�����X�^�[��������";color[vn++] = TERM_L_WHITE;}
#else
	if (flags6 & (RF6_S_MONSTERS))      {vp[vn] = "summon monsters";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_KIN))           {vp[vn] = "�~������";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_KIN))           {vp[vn] = "summon aid";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_ANT))           {vp[vn] = "�A������";color[vn++] = TERM_RED;}
#else
	if (flags6 & (RF6_S_ANT))           {vp[vn] = "summon ants";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_SPIDER))        {vp[vn] = "������";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_SPIDER))        {vp[vn] = "summon spiders";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HOUND))         {vp[vn] = "�n�E���h����";color[vn++] = TERM_L_UMBER;}
#else
	if (flags6 & (RF6_S_HOUND))         {vp[vn] = "summon hounds";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HYDRA))         {vp[vn] = "�q�h������";color[vn++] = TERM_L_GREEN;}
#else
	if (flags6 & (RF6_S_HYDRA))         {vp[vn] = "summon hydras";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags6 & (RF6_S_ANGEL))         {vp[vn] = "�V�g��̏���";color[vn++] = TERM_YELLOW;}
#else
	if (flags6 & (RF6_S_ANGEL))         {vp[vn] = "summon an angel";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags6 & (RF6_S_DEMON))         {vp[vn] = "��������";color[vn++] = TERM_L_RED;}
#else
	if (flags6 & (RF6_S_DEMON))         {vp[vn] = "summon a demon";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_UNDEAD))        {vp[vn] = "�A���f�b�h��̏���";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_UNDEAD))        {vp[vn] = "summon an undead";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_DRAGON))        {vp[vn] = "�h���S����̏���";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_DRAGON))        {vp[vn] = "summon a dragon";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HI_UNDEAD))     {vp[vn] = "���͂ȃA���f�b�h����";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_HI_UNDEAD))     {vp[vn] = "summon Greater Undead";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HI_DRAGON))     {vp[vn] = "�Ñ�h���S������";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_HI_DRAGON))     {vp[vn] = "summon Ancient Dragons";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_CYBER))         {vp[vn] = "�T�C�o�[�f�[��������";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_S_CYBER))         {vp[vn] = "summon Cyberdemons";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_S_AMBERITES))     {vp[vn] = "�A���o�[�̉�������";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_S_AMBERITES))     {vp[vn] = "summon Lords of Amber";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_S_UNIQUE))        {vp[vn] = "���j�[�N�E�����X�^�[����";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_S_UNIQUE))        {vp[vn] = "summon Unique Monsters";color[vn++] = TERM_VIOLET;}
#endif


#ifdef JP
if (flags9 & (RF9_ACID_STORM))        {vp[vn] = "�_�̗�";color[vn++] = TERM_GREEN;}
if (flags9 & (RF9_THUNDER_STORM))        {vp[vn] = "���̗�";color[vn++] = TERM_BLUE;}
if (flags9 & (RF9_FIRE_STORM))        {vp[vn] = "���̗�";color[vn++] = TERM_L_RED;}
if (flags9 & (RF9_ICE_STORM))        {vp[vn] = "�X�̗�";color[vn++] = TERM_L_WHITE;}
if (flags9 & (RF9_TOXIC_STORM))        {vp[vn] = "�őf�̗�";color[vn++] = TERM_L_GREEN;}
if (flags9 & (RF9_BA_POLLUTE))        {vp[vn] = "�����̋�";color[vn++] = TERM_VIOLET;}
if (flags9 & (RF9_BA_DISI))        {vp[vn] = "���q����";color[vn++] = TERM_SLATE;}
if (flags9 & (RF9_BA_HOLY))        {vp[vn] = "�j�ׂ̌���";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_BA_METEOR))        {vp[vn] = "���e�I�X�g���C�N";color[vn++] = TERM_L_DARK;}
if (flags9 & (RF9_BA_DISTORTION))        {vp[vn] = "��Ԙc�Ȃ̋�";color[vn++] = TERM_L_BLUE;}
if (flags9 & (RF9_PUNISH_1))        {vp[vn] = "�ޖ�1";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_2))        {vp[vn] = "�ޖ�2";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_3))        {vp[vn] = "�ޖ�3";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_PUNISH_4))        {vp[vn] = "�ޖ�4";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_BO_HOLY))        {vp[vn] = "���Ȃ��";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_GIGANTIC_LASER))        {vp[vn] = "���僌�[�U�[";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_BO_SOUND))        {vp[vn] = "��̃{���g";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_S_ANIMAL))        {vp[vn] = "��������";color[vn++] = TERM_L_UMBER;}
if (flags9 & (RF9_BEAM_LITE))        {vp[vn] = "���[�U�[";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_SONICWAVE))        {vp[vn] = "�����g";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_HELLFIRE))        {vp[vn] = "�w���t�@�C�A";color[vn++] = TERM_VIOLET;}
if (flags9 & (RF9_HOLYFIRE))        {vp[vn] = "�z�[���[�t�@�C�A";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_FINALSPARK))        {vp[vn] = "�t�@�C�i���X�p�[�N";color[vn++] = TERM_WHITE;}
if (flags9 & (RF9_TORNADO))        {vp[vn] = "����";color[vn++] = TERM_L_GREEN;}
if (flags9 & (RF9_DESTRUCTION))        {vp[vn] = "*�j��*";color[vn++] = TERM_RED;}
if (flags9 & (RF9_S_DEITY))        {vp[vn] = "�_�i����";color[vn++] = TERM_YELLOW;}
if (flags9 & (RF9_S_HI_DEMON))        {vp[vn] = "��ʈ�������";color[vn++] = TERM_L_RED;}
if (flags9 & (RF9_S_KWAI))        {vp[vn] = "�d���ďW";color[vn++] = TERM_GREEN;}
if (flags9 & (RF9_TELE_APPROACH))        {vp[vn] = "�אڃe���|�[�g";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_TELE_HI_APPROACH))        {vp[vn] = "���E�O�אڃe���|�[�g";color[vn++] = TERM_ORANGE;}
if (flags9 & (RF9_MAELSTROM))        {vp[vn] = "���C���V���g����";color[vn++] = TERM_BLUE;}
if (flags9 & (RF9_ALARM))        {vp[vn] = "�x��+����";color[vn++] = TERM_L_RED;}



#endif








	/* Describe spells */
	if (vn)
	{
		/* Note magic */
		magic = TRUE;

		/* Intro */
		if (breath)
		{
#ifdef JP
			hooked_roff("�A�Ȃ�����");
#else
			hooked_roff(", and is also");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^s��", wd_he[msex]));
#else
			hooked_roff(format("%^s is", wd_he[msex]));
#endif

		}

#ifdef JP
		/* Adverb */
		if (flags2 & (RF2_SMART_EX)) hook_c_roff(TERM_ORANGE, "�ɂ߂ēI�m��");
		else if (flags2 & (RF2_SMART)) hook_c_roff(TERM_YELLOW, "�I�m��");

		/* Verb Phrase */
		hooked_roff("���@���g�����Ƃ��ł��A");
#else
		/* Verb Phrase */
		hooked_roff(" magical, casting spells");

		/* Adverb */
		if (flags2 & RF2_SMART) hook_c_roff(TERM_YELLOW, " intelligently");
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("�A");
#else
			if (n == 0) hooked_roff(" which ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}
#ifdef JP
		hooked_roff("�̎����������邱�Ƃ�����");
#endif
	}


	/* End the sentence about inate/other spells */
	if (breath || magic)
	{
		/* Total casting */
		m = r_ptr->r_cast_spell;

		/* Average frequency */
		n = r_ptr->freq_spell;

		/* Describe the spell frequency */
		if (m > 100 || know_everything)
		{
#ifdef JP
			hooked_roff(format("(�m��:1/%d)", 100 / n));
#else
			hooked_roff(format("; 1 time in %d", 100 / n));
#endif

		}

		/* Guess at the frequency */
		else if (m)
		{
			n = ((n + 9) / 10) * 10;
#ifdef JP
			hooked_roff(format("(�m��:��1/%d)", 100 / n));
#else
			hooked_roff(format("; about 1 time in %d", 100 / n));
#endif

		}

		/* End this sentence */
#ifdef JP
		hooked_roff("�B");
#else
		hooked_roff(".  ");
#endif

	}

	/* Describe monster "toughness" */
	if (know_armour(r_idx))
	{
		/* Armor */
#ifdef JP
		hooked_roff(format("%^s�� AC%d �̖h��͂�",
#else
		hooked_roff(format("%^s has an armor rating of %d",
#endif

			    wd_he[msex], r_ptr->ac));

		/* Maximized hitpoints */
		if ((flags1 & RF1_FORCE_MAXHP) || (r_ptr->hside == 1))
		{
			u32b hp = r_ptr->hdice * (nightmare ? 2 : 1) * r_ptr->hside;

			if(difficulty == DIFFICULTY_EASY) hp = MAX(1,hp / 2);
			else if(difficulty == DIFFICULTY_NORMAL) hp = MAX(1,hp * 3 / 4);


#ifdef JP
			hooked_roff(format(" %d �̗̑͂�����B",
#else
			hooked_roff(format(" and a life rating of %d.  ",
#endif
				    (s16b)MIN(30000, hp)));
		}

		/* Variable hitpoints */
		else
		{
			int hd = r_ptr->hdice * (nightmare ? 2 : 1);
			if(difficulty == DIFFICULTY_EASY) hd = MAX(1,hd / 2);
			else if(difficulty == DIFFICULTY_NORMAL) hd = MAX(1,hd * 3 / 4);
#ifdef JP
			hooked_roff(format(" %dd%d �̗̑͂�����B",
#else
			hooked_roff(format(" and a life rating of %dd%d.  ",
#endif
				    hd , r_ptr->hside));
		}
	}



	/* Collect special abilities. */
	vn = 0;
#ifdef JP
	if (flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) { vp[vn] = "�_���W�������Ƃ炷";     color[vn++] = TERM_WHITE; }
#else
	if (flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) { vp[vn] = "illuminate the dungeon"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags7 & (RF7_HAS_DARK_1 | RF7_HAS_DARK_2)) { vp[vn] = "�_���W�������Â�����";   color[vn++] = TERM_L_DARK; }
#else
	if (flags7 & (RF7_HAS_DARK_1 | RF7_HAS_DARK_2)) { vp[vn] = "darken the dungeon";     color[vn++] = TERM_L_DARK; }
#endif

#ifdef JP
	if (flags2 & RF2_OPEN_DOOR) { vp[vn] = "�h�A���J����"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_OPEN_DOOR) { vp[vn] = "open doors"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_BASH_DOOR) { vp[vn] = "�h�A��ł��j��"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_BASH_DOOR) { vp[vn] = "bash down doors"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_PASS_WALL) { vp[vn] = "�ǂ����蔲����"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_PASS_WALL) { vp[vn] = "pass through walls"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_WALL) { vp[vn] = "�ǂ��@��i��"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_WALL) { vp[vn] = "bore through walls"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_MOVE_BODY) { vp[vn] = "�ア�����X�^�[�������̂���"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_MOVE_BODY) { vp[vn] = "push past weaker monsters"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_BODY) { vp[vn] = "�ア�����X�^�[��|��"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_BODY) { vp[vn] = "destroy weaker monsters"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_TAKE_ITEM) { vp[vn] = "�A�C�e�����E��"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_TAKE_ITEM) { vp[vn] = "pick up objects"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_ITEM) { vp[vn] = "�A�C�e������"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_ITEM) { vp[vn] = "destroy objects"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_TELE_AFTER) { vp[vn] = "�e���|�[�g��ǔ�����"; color[vn++] = TERM_ORANGE; }
#endif





	/* Describe special abilities. */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^s��", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if (n != vn - 1)
			{
				jverb(vp[n], jverb_buf, JVERB_AND);
				hook_c_roff(color[n], jverb_buf);
				hooked_roff("�A");
			}
			else hook_c_roff(color[n], vp[n]);
#else
			if (n == 0) hooked_roff(" can ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" and ");

			/* Dump */
			hook_c_roff(color[n], vp[n]);
#endif

		}

		/* End */
#ifdef JP
		hooked_roff("���Ƃ��ł���B");
#else
		hooked_roff(".  ");
#endif

	}


	/* Describe special abilities. */
	if (flags7 & (RF7_SELF_LITE_1 | RF7_SELF_LITE_2))
	{
#ifdef JP
		hooked_roff(format("%^s�͌����Ă���B", wd_he[msex]));
#else
		hooked_roff(format("%^s is shining.  ", wd_he[msex]));
#endif

	}
	if (flags7 & (RF7_SELF_DARK_1 | RF7_SELF_DARK_2))
	{
#ifdef JP
		hook_c_roff(TERM_L_DARK, format("%^s�͈Í��ɕ�܂�Ă���B", wd_he[msex]));
#else
		hook_c_roff(TERM_L_DARK, format("%^s is surrounded by darkness.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_INVISIBLE)
	{
#ifdef JP
		hooked_roff(format("%^s�͓����ŖڂɌ����Ȃ��B", wd_he[msex]));
#else
		hooked_roff(format("%^s is invisible.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_COLD_BLOOD)
	{
#ifdef JP
		hooked_roff(format("%^s�͐ԊO���ł͊��m�ł��Ȃ��B", wd_he[msex]));
#else
		hooked_roff(format("%^s is cold blooded.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_EMPTY_MIND)
	{
#ifdef JP
		hooked_roff(format("%^s�̓e���p�V�[�ł͊��m�ł��Ȃ��B", wd_he[msex]));
#else
		hooked_roff(format("%^s is not detected by telepathy.  ", wd_he[msex]));
#endif

	}
	else if (flags2 & RF2_WEIRD_MIND)
	{
#ifdef JP
		hooked_roff(format("%^s�͂܂�Ƀe���p�V�[�Ŋ��m�ł���B", wd_he[msex]));
#else
		hooked_roff(format("%^s is rarely detected by telepathy.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_MULTIPLY)
	{
#ifdef JP
		hook_c_roff(TERM_L_UMBER, format("%^s�͔����I�ɑ��B����B", wd_he[msex]));
#else
		hook_c_roff(TERM_L_UMBER, format("%^s breeds explosively.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_REGENERATE)
	{
#ifdef JP
		hook_c_roff(TERM_L_WHITE, format("%^s�͑f�����̗͂��񕜂���B", wd_he[msex]));
#else
		hook_c_roff(TERM_L_WHITE, format("%^s regenerates quickly.  ", wd_he[msex]));
#endif

	}
	if (flags7 & RF7_RIDING)
	{
#ifdef JP
		hook_c_roff(TERM_SLATE, format("%^s�ɏ�邱�Ƃ��ł���B", wd_he[msex]));
#else
		hook_c_roff(TERM_SLATE, format("%^s is suitable for riding.  ", wd_he[msex]));
#endif

	}


	/* Collect susceptibilities */
	vn = 0;
#ifdef JP
///mod131231 �����X�^�[�t���O�ύX ��Ηd����_RF3����RFR��
	if (flagsr & RFR_HURT_ROCK) {vp[vn] = "��������������";color[vn++] = TERM_UMBER;}
#else
	if (flags3 & RF3_HURT_ROCK) {vp[vn] = "rock remover";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
///mod131231 �����X�^�[�t���O�ύX
	if (flagsr & RFR_HURT_LITE) {vp[vn] = "���邢��";color[vn++] = TERM_YELLOW;}
	//if (flags3 & RF3_HURT_LITE) {vp[vn] = "���邢��";color[vn++] = TERM_YELLOW;}
#else
	if (flags3 & RF3_HURT_LITE) {vp[vn] = "bright light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
///mod131231 �����X�^�[�t���O�ύX �Ή���_RF3����RFR��
	if (flagsr & RFR_HURT_FIRE) {vp[vn] = "��";color[vn++] = TERM_RED;}
#else
	if (flags3 & RF3_HURT_FIRE) {vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

///mod131231 �����X�^�[�t���O�ύX ��C��_RF3����RFR��
#ifdef JP
	if (flagsr & RFR_HURT_COLD) {vp[vn] = "��C";color[vn++] = TERM_L_WHITE;}
#else
	if (flags3 & RF3_HURT_COLD) {vp[vn] = "cold";color[vn++] = TERM_L_WHITE;}
#endif
#ifdef JP
	if (flagsr & RFR_HURT_ELEC) {vp[vn] = "�d��";color[vn++] = TERM_BLUE;}
	if (flagsr & RFR_HURT_WATER) {vp[vn] = "��";color[vn++] = TERM_L_BLUE;}
	///pend �n���̋ƉΎ�_�͉B���H
	//if (flagsr & RFR_HURT_HELL) {vp[vn] = "�n���̋Ɖ�";color[vn++] = TERM_L_DARK;}
	if (flagsr & RFR_HURT_HOLY) {vp[vn] = "�j��";color[vn++] = TERM_YELLOW;}

	if (flags3 & RF3_HANIWA) { vp[vn] = "�Ռ�"; color[vn++] = TERM_WHITE; }

#else
	if (flags3 & RF3_HURT_COLD) {vp[vn] = "cold";color[vn++] = TERM_L_WHITE;}
#endif

	/* Describe susceptibilities */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^s�ɂ�", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("��");
#else
			if (n == 0) hooked_roff(" is hurt by ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" and ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("�Ń_���[�W��^������B");
#else
		hooked_roff(".  ");
#endif

	}


	/* Collect immunities */
	vn = 0;
#ifdef JP
	if (flagsr & RFR_IM_ACID) {vp[vn] = "�_";color[vn++] = TERM_GREEN;}
#else
	if (flagsr & RFR_IM_ACID) {vp[vn] = "acid";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_ELEC) {vp[vn] = "���";color[vn++] = TERM_BLUE;}
#else
	if (flagsr & RFR_IM_ELEC) {vp[vn] = "lightning";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_FIRE) {vp[vn] = "��";color[vn++] = TERM_RED;}
#else
	if (flagsr & RFR_IM_FIRE) {vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_COLD) {vp[vn] = "��C";color[vn++] = TERM_L_WHITE;}
#else
	if (flagsr & RFR_IM_COLD) {vp[vn] = "cold";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flagsr & RFR_IM_POIS) {vp[vn] = "��";color[vn++] = TERM_L_GREEN;}
#else
	if (flagsr & RFR_IM_POIS) {vp[vn] = "poison";color[vn++] = TERM_L_GREEN;}
#endif


	/* Collect resistances */
#ifdef JP
	if (flagsr & RFR_RES_LITE) {vp[vn] = "�M��";color[vn++] = TERM_YELLOW;}
#else
	if (flagsr & RFR_RES_LITE) {vp[vn] = "light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_DARK) {vp[vn] = "�Í�";color[vn++] = TERM_L_DARK;}
#else
	if (flagsr & RFR_RES_DARK) {vp[vn] = "dark";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_NETH) {vp[vn] = "�n��";color[vn++] = TERM_L_DARK;}
#else
	if (flagsr & RFR_RES_NETH) {vp[vn] = "nether";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_WATE) {vp[vn] = "��";color[vn++] = TERM_BLUE;}
#else
	if (flagsr & RFR_RES_WATE) {vp[vn] = "water";color[vn++] = TERM_BLUE;}
#endif

///mod131231 �v���Y�}�ϐ��ύX
/*
#ifdef JP
	if (flagsr & RFR_RES_PLAS) {vp[vn] = "�v���Y�}";color[vn++] = TERM_L_RED;}
#else
	if (flagsr & RFR_RES_PLAS) {vp[vn] = "plasma";color[vn++] = TERM_L_RED;}
#endif
*/

#ifdef JP
	if (flagsr & RFR_RES_SHAR) {vp[vn] = "�j��";color[vn++] = TERM_L_UMBER;}
#else
	if (flagsr & RFR_RES_SHAR) {vp[vn] = "shards";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_SOUN) {vp[vn] = "����";color[vn++] = TERM_ORANGE;}
#else
	if (flagsr & RFR_RES_SOUN) {vp[vn] = "sound";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_CHAO) {vp[vn] = "�J�I�X";color[vn++] = TERM_VIOLET;}
#else
	if (flagsr & RFR_RES_CHAO) {vp[vn] = "chaos";color[vn++] = TERM_VIOLET;}
#endif


#ifdef JP
	if (flagsr & RFR_RES_HOLY) {vp[vn] = "�j��";color[vn++] = TERM_L_WHITE;}
#else
	if (flagsr & RFR_RES_HOLY) {vp[vn] = "nexus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_DISE) {vp[vn] = "��";color[vn++] = TERM_VIOLET;}
#else
	if (flagsr & RFR_RES_DISE) {vp[vn] = "disenchantment";color[vn++] = TERM_VIOLET;}
#endif

///�t�H�[�X�ϐ��폜
/*
#ifdef JP
	if (flagsr & RFR_RES_WALL) {vp[vn] = "�t�H�[�X";color[vn++] = TERM_UMBER;}
#else
	if (flagsr & RFR_RES_WALL) {vp[vn] = "force";color[vn++] = TERM_UMBER;}
#endif
*/
#ifdef JP
	if (flagsr & RFR_RES_INER) {vp[vn] = "�x��";color[vn++] = TERM_SLATE;}
#else
	if (flagsr & RFR_RES_INER) {vp[vn] = "inertia";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flagsr & RFR_RES_TIME) {vp[vn] = "����U��";color[vn++] = TERM_L_BLUE;}
#else
	if (flagsr & RFR_RES_TIME) {vp[vn] = "time";color[vn++] = TERM_L_BLUE;}
#endif
///mod131231 �d�͑ϐ��폜
/*
#ifdef JP
	if (flagsr & RFR_RES_GRAV) {vp[vn] = "�d��";color[vn++] = TERM_SLATE;}
#else
	if (flagsr & RFR_RES_GRAV) {vp[vn] = "gravity";color[vn++] = TERM_SLATE;}
#endif
*/
#ifdef JP
	if (flagsr & RFR_RES_ALL) {vp[vn] = "������U��";color[vn++] = TERM_YELLOW;}
#else
	if (flagsr & RFR_RES_ALL) {vp[vn] = "all";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if ((flagsr & RFR_RES_TELE) && !(r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "�e���|�[�g";color[vn++] = TERM_ORANGE;}
#else
	if ((flagsr & RFR_RES_TELE) && !(r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "teleportation";color[vn++] = TERM_ORANGE;}
#endif


	/* Describe immunities and resistances */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^s��", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("��");
#else
			if (n == 0) hooked_roff(" resists ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" and ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("�̑ϐ��������Ă���B");
#else
		hooked_roff(".  ");
#endif

	}


	if ((r_ptr->r_xtra1 & MR1_SINKA) || know_everything)
	{
		if (r_ptr->next_r_idx)
		{
#ifdef JP
			hooked_roff(format("%^s�͌o����ςނƁA", wd_he[msex]));
#else
			hooked_roff(format("%^s will evolve into ", wd_he[msex]));
#endif
			hook_c_roff(TERM_YELLOW, format("%s", r_name+r_info[r_ptr->next_r_idx].name));
#ifdef JP
			hooked_roff(format("�ɐi������B"));
#else
			hooked_roff(format(" when %s gets enugh experience.  ", wd_he[msex]));
#endif
		}
		else if (!(r_ptr->flags1 & RF1_UNIQUE))
		{
#ifdef JP
			hooked_roff(format("%s�͐i�����Ȃ��B", wd_he[msex]));
#else
			hooked_roff(format("%s won't evolve.  ", wd_he[msex]));
#endif
		}
	}

	/* Collect non-effects */
	vn = 0;
#ifdef JP
	if (flags3 & RF3_NO_STUN)  {vp[vn] = "�N�O�Ƃ��Ȃ�";color[vn++] = TERM_ORANGE;}
#else
	if (flags3 & RF3_NO_STUN)  {vp[vn] = "stunned";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_FEAR)  {vp[vn] = "���|�������Ȃ�";color[vn++] = TERM_SLATE;}
#else
	if (flags3 & RF3_NO_FEAR)  {vp[vn] = "frightened";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_CONF)  {vp[vn] = "�������Ȃ�";color[vn++] = TERM_L_UMBER;}
#else
	if (flags3 & RF3_NO_CONF)  {vp[vn] = "confused";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_SLEEP) {vp[vn] = "���炳��Ȃ�";color[vn++] = TERM_BLUE;}
#else
	if (flags3 & RF3_NO_SLEEP) {vp[vn] = "slept";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if ((flagsr & RFR_RES_TELE) && (r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "�e���|�[�g����Ȃ�";color[vn++] = TERM_ORANGE;}
#else
	if ((flagsr & RFR_RES_TELE) && (r_ptr->flags1 & RF1_UNIQUE)) {vp[vn] = "teleported";color[vn++] = TERM_ORANGE;}
#endif

	/* Describe non-effects */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^s��", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("���A");
#else
			if (n == 0) hooked_roff(" cannot be ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("�B");
#else
		hooked_roff(".  ");
#endif

	}


	/* Do we know how aware it is? */
	if ((((int)r_ptr->r_wake * (int)r_ptr->r_wake) > r_ptr->sleep) ||
		  (r_ptr->r_ignore == MAX_UCHAR) ||
	    (r_ptr->sleep == 0 && r_ptr->r_tkills >= 10) || know_everything)
	{
		cptr act;

		if (r_ptr->sleep > 200)
		{
#ifdef JP
			act = "�𖳎��������ł��邪";
#else
			act = "prefers to ignore";
#endif

		}
		else if (r_ptr->sleep > 95)
		{
#ifdef JP
			act = "�ɑ΂��ĂقƂ�ǒ��ӂ𕥂�Ȃ���";
#else
			act = "pays very little attention to";
#endif

		}
		else if (r_ptr->sleep > 75)
		{
#ifdef JP
			act = "�ɑ΂��Ă��܂蒍�ӂ𕥂�Ȃ���";
#else
			act = "pays little attention to";
#endif

		}
		else if (r_ptr->sleep > 45)
		{
#ifdef JP
			act = "�����߂��������ł��邪";
#else
			act = "tends to overlook";
#endif

		}
		else if (r_ptr->sleep > 25)
		{
#ifdef JP
			act = "���ق�̏����͌��Ă���";
#else
			act = "takes quite a while to see";
#endif

		}
		else if (r_ptr->sleep > 10)
		{
#ifdef JP
			act = "�����΂炭�͌��Ă���";
#else
			act = "takes a while to see";
#endif

		}
		else if (r_ptr->sleep > 5)
		{
#ifdef JP
			act = "���������Ӑ[�����Ă���";
#else
			act = "is fairly observant of";
#endif

		}
		else if (r_ptr->sleep > 3)
		{
#ifdef JP
			act = "�𒍈Ӑ[�����Ă���";
#else
			act = "is observant of";
#endif

		}
		else if (r_ptr->sleep > 1)
		{
#ifdef JP
			act = "�����Ȃ蒍�Ӑ[�����Ă���";
#else
			act = "is very observant of";
#endif

		}
		else if (r_ptr->sleep > 0)
		{
#ifdef JP
			act = "���x�����Ă���";
#else
			act = "is vigilant for";
#endif

		}
		else
		{
#ifdef JP
			act = "�����Ȃ�x�����Ă���";
#else
			act = "is ever vigilant for";
#endif

		}

#ifdef JP
		hooked_roff(format("%^s�͐N����%s�A %d �t�B�[�g�悩��N���҂ɋC�t�����Ƃ�����B",
		     wd_he[msex], act, 10 * r_ptr->aaf));
#else
		hooked_roff(format("%^s %s intruders, which %s may notice from %d feet.  ",
			    wd_he[msex], act, wd_he[msex], 10 * r_ptr->aaf));
#endif

	}


	/* Drops gold and/or items */
	if (drop_gold || drop_item)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^s��", wd_he[msex]));
#else
		hooked_roff(format("%^s may carry", wd_he[msex]));

		/* No "n" needed */
		sin = FALSE;
#endif


		/* Count maximum drop */
		n = MAX(drop_gold, drop_item);

		/* One drop (may need an "n") */
		if (n == 1)
		{
#ifdef JP
			hooked_roff("���");
#else
			hooked_roff(" a");
			sin = TRUE;
#endif
		}

		/* Two drops */
		else if (n == 2)
		{
#ifdef JP
			hooked_roff("������");
#else
			hooked_roff(" one or two");
#endif

		}

		/* Many drops */
		else
		{
#ifdef JP
			hooked_roff(format(" %d �܂ł�", n));
#else
			hooked_roff(format(" up to %d", n));
#endif

		}


		/* Great */
		if (flags1 & RF1_DROP_GREAT)
		{
#ifdef JP
			p = "���ʂ�";
#else
			p = " exceptional";
#endif

		}

		/* Good (no "n" needed) */
		else if (flags1 & RF1_DROP_GOOD)
		{
#ifdef JP
			p = "�㎿��";
#else
			p = " good";
			sin = FALSE;
#endif
		}

		/* Okay */
		else
		{
			p = NULL;
		}


		/* Objects */
		if (drop_item)
		{
			/* Handle singular "an" */
#ifndef JP
			if (sin) hooked_roff("n");
			sin = FALSE;
#endif

			/* Dump "object(s)" */
			if (p) hooked_roff(p);
#ifdef JP
			hooked_roff("�A�C�e��");
#else
			hooked_roff(" object");
			if (n != 1) hooked_roff("s");
#endif


			/* Conjunction replaces variety, if needed for "gold" below */
#ifdef JP
			p = "��";
#else
			p = " or";
#endif

		}

		/* Treasures */
		if (drop_gold)
		{
#ifndef JP
			/* Cancel prefix */
			if (!p) sin = FALSE;

			/* Handle singular "an" */
			if (sin) hooked_roff("n");
			sin = FALSE;
#endif

			/* Dump "treasure(s)" */
			if (p) hooked_roff(p);
#ifdef JP
			hooked_roff("����");
#else
			hooked_roff(" treasure");
			if (n != 1) hooked_roff("s");
#endif

		}

		/* End this sentence */
#ifdef JP
		hooked_roff("�������Ă��邱�Ƃ�����B");
#else
		hooked_roff(".  ");
#endif

	}


	/* Count the number of "known" attacks */
	for (n = 0, m = 0; m < 4; m++)
	{
		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;
		if (r_ptr->blow[m].method == RBM_SHOOT) continue;

		/* Count known attacks */
		if (r_ptr->r_blows[m] || know_everything) n++;
	}

	/* Examine (and count) the actual attacks */
	for (r = 0, m = 0; m < 4; m++)
	{
		int method, effect, d1, d2;

		/* Skip non-attacks */
		if (!r_ptr->blow[m].method) continue;
		if (r_ptr->blow[m].method == RBM_SHOOT) continue;

		/* Skip unknown attacks */
		if (!r_ptr->r_blows[m] && !know_everything) continue;

		/* Extract the attack info */
		method = r_ptr->blow[m].method;
		effect = r_ptr->blow[m].effect;
		d1 = r_ptr->blow[m].d_dice;
		d2 = r_ptr->blow[m].d_side;

		/* No method yet */
		p = NULL;

		/* Acquire the method */
		switch (method)
		{
#ifdef JP
case RBM_HIT:		p = "�U������"; break;
#else
			case RBM_HIT:		p = "hit"; break;
#endif

#ifdef JP
case RBM_TOUCH:		p = "�G��"; break;
#else
			case RBM_TOUCH:		p = "touch"; break;
#endif

#ifdef JP
case RBM_PUNCH:		p = "����"; break;
#else
			case RBM_PUNCH:		p = "punch"; break;
#endif

#ifdef JP
case RBM_KICK:		p = "�R��"; break;
#else
			case RBM_KICK:		p = "kick"; break;
#endif

#ifdef JP
case RBM_CLAW:		p = "�Ђ�����"; break;
#else
			case RBM_CLAW:		p = "claw"; break;
#endif

#ifdef JP
case RBM_BITE:		p = "����"; break;
#else
			case RBM_BITE:		p = "bite"; break;
#endif

#ifdef JP
case RBM_STING:		p = "�h��"; break;
#else
			case RBM_STING:		p = "sting"; break;
#endif

#ifdef JP
case RBM_SLASH:		p = "�a��"; break;
#else
			case RBM_SLASH:		p = "slash"; break;
#endif

#ifdef JP
case RBM_BUTT:		p = "�p�œ˂�"; break;
#else
			case RBM_BUTT:		p = "butt"; break;
#endif

#ifdef JP
case RBM_CRUSH:		p = "�̓����肷��"; break;
#else
			case RBM_CRUSH:		p = "crush"; break;
#endif

#ifdef JP
case RBM_ENGULF:	p = "���ݍ���"; break;
#else
			case RBM_ENGULF:	p = "engulf"; break;
#endif

#ifdef JP
case RBM_CHARGE: 	p = "���������悱��"; break;
#else
			case RBM_CHARGE: 	p = "charge";   break;
#endif

#ifdef JP
case RBM_CRAWL:		p = "�̂̏�𔇂����"; break;
#else
			case RBM_CRAWL:		p = "crawl on you"; break;
#endif

#ifdef JP
case RBM_DROOL:		p = "�悾������炷"; break;
#else
			case RBM_DROOL:		p = "drool on you"; break;
#endif

#ifdef JP
case RBM_SPIT:		p = "�΂�f��"; break;
#else
			case RBM_SPIT:		p = "spit"; break;
#endif

#ifdef JP
case RBM_EXPLODE:	p = "��������"; break;
#else
			case RBM_EXPLODE:	p = "explode"; break;
#endif

#ifdef JP
case RBM_GAZE:		p = "�ɂ��"; break;
#else
			case RBM_GAZE:		p = "gaze"; break;
#endif

#ifdef JP
case RBM_WAIL:		p = "��������"; break;
#else
			case RBM_WAIL:		p = "wail"; break;
#endif

#ifdef JP
case RBM_SPORE:		p = "�E�q���΂�"; break;
#else
			case RBM_SPORE:		p = "release spores"; break;
#endif

			case RBM_DANCE:		p = "�x��"; break;
#ifdef JP
case RBM_BEG:		p = "����������"; break;
#else
			case RBM_BEG:		p = "beg"; break;
#endif

#ifdef JP
case RBM_INSULT:	p = "���J����"; break;
#else
			case RBM_INSULT:	p = "insult"; break;
#endif

#ifdef JP
case RBM_MOAN:		p = "���߂�"; break;
#else
			case RBM_MOAN:		p = "moan"; break;
#endif

#ifdef JP
case RBM_SHOW:  	p = "�̂�"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif

#ifdef JP
case RBM_SPEAR:  	p = "�˂�"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_BOOK:  	p = "�{�ŉ���"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_MOFUMOFU:  	p = "�K���Ń��t���t����"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_SQUEEZE:  	p = "���ߕt����"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_DRAG:  	p = "�������荞��"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif

#ifdef JP
case RBM_INJECT:  	p = "���˂���"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif

#ifdef JP
case RBM_SMILE:  	p = "���΂�"; break;
#else
			case RBM_SMILE:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_HEADBUTT:  	p = "���˂�����"; break;
#else
			case RBM_HEADBUTT:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_FLASH:  	p = "����"; break;
#else
			case RBM_FLASH:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_HOWL:  	p = "�i����"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_WHISPER:  	p = "�����₭"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_PRESS:  	p = "�����ׂ�"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_POINT:  	p = "�w����"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_STRIKE:  	p = "�ł�������"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif
#ifdef JP
case RBM_PLAY:  	p = "���t����"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif

#ifdef JP
			case RBM_SMOKE:  	p = "���𐁂�������"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif



		}


		/* Default effect */
		q = NULL;

		/* Acquire the effect */
		switch (effect)
		{
#ifdef JP
///msg131230 SUPERHURT�̋L�q�ύX
case RBE_SUPERHURT:	q = "����ȍU����������"; break;

case RBE_HURT:    	q = "�U������"; break;
#else
			case RBE_SUPERHURT:
			case RBE_HURT:    	q = "attack"; break;
#endif

#ifdef JP
case RBE_POISON:  	q = "�ł�����킷"; break;
#else
			case RBE_POISON:  	q = "poison"; break;
#endif

#ifdef JP
case RBE_UN_BONUS:	q = "�򉻂�����"; break;
#else
			case RBE_UN_BONUS:	q = "disenchant"; break;
#endif

#ifdef JP
case RBE_UN_POWER:	q = "�[�U���͂��z������"; break;
#else
			case RBE_UN_POWER:	q = "drain charges"; break;
#endif

#ifdef JP
case RBE_EAT_GOLD:	q = "���𓐂�"; break;
#else
			case RBE_EAT_GOLD:	q = "steal gold"; break;
#endif

#ifdef JP
case RBE_EAT_ITEM:	q = "�A�C�e���𓐂�"; break;
#else
			case RBE_EAT_ITEM:	q = "steal items"; break;
#endif

#ifdef JP
case RBE_EAT_FOOD:	q = "���Ȃ��̐H����H�ׂ�"; break;
#else
			case RBE_EAT_FOOD:	q = "eat your food"; break;
#endif

#ifdef JP
case RBE_EAT_LITE:	q = "��������z������"; break;
#else
			case RBE_EAT_LITE:	q = "absorb light"; break;
#endif

#ifdef JP
case RBE_ACID:    	q = "�_���΂�"; break;
#else
			case RBE_ACID:    	q = "shoot acid"; break;
#endif

#ifdef JP
case RBE_ELEC:    	q = "���d������"; break;
#else
			case RBE_ELEC:    	q = "electrocute"; break;
#endif

#ifdef JP
case RBE_FIRE:    	q = "�R�₷"; break;
#else
			case RBE_FIRE:    	q = "burn"; break;
#endif

#ifdef JP
case RBE_COLD:    	q = "���点��"; break;
#else
			case RBE_COLD:    	q = "freeze"; break;
#endif

#ifdef JP
case RBE_BLIND:   	q = "�Ӗڂɂ���"; break;
#else
			case RBE_BLIND:   	q = "blind"; break;
#endif

#ifdef JP
case RBE_CONFUSE: 	q = "����������"; break;
#else
			case RBE_CONFUSE: 	q = "confuse"; break;
#endif

#ifdef JP
case RBE_TERRIFY: 	q = "���|������"; break;
#else
			case RBE_TERRIFY: 	q = "terrify"; break;
#endif

#ifdef JP
case RBE_PARALYZE:	q = "��Ⴢ�����"; break;
#else
			case RBE_PARALYZE:	q = "paralyze"; break;
#endif

#ifdef JP
case RBE_LOSE_STR:	q = "�r�͂�����������"; break;
#else
			case RBE_LOSE_STR:	q = "reduce strength"; break;
#endif

#ifdef JP
case RBE_LOSE_INT:	q = "�m�\������������"; break;
#else
			case RBE_LOSE_INT:	q = "reduce intelligence"; break;
#endif

#ifdef JP
case RBE_LOSE_WIS:	q = "����������������"; break;
#else
			case RBE_LOSE_WIS:	q = "reduce wisdom"; break;
#endif

#ifdef JP
case RBE_LOSE_DEX:	q = "��p��������������"; break;
#else
			case RBE_LOSE_DEX:	q = "reduce dexterity"; break;
#endif

#ifdef JP
case RBE_LOSE_CON:	q = "�ϋv�͂�����������"; break;
#else
			case RBE_LOSE_CON:	q = "reduce constitution"; break;
#endif

#ifdef JP
case RBE_LOSE_CHR:	q = "���͂�����������"; break;
#else
			case RBE_LOSE_CHR:	q = "reduce charisma"; break;
#endif

#ifdef JP
case RBE_LOSE_ALL:	q = "�S�X�e�[�^�X������������"; break;
#else
			case RBE_LOSE_ALL:	q = "reduce all stats"; break;
#endif

#ifdef JP
case RBE_SHATTER:	q = "���ӂ���"; break;
#else
			case RBE_SHATTER:	q = "shatter"; break;
#endif

#ifdef JP
case RBE_EXP_10:	q = "�o���l������(10d6+)������"; break;
#else
			case RBE_EXP_10:	q = "lower experience (by 10d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_20:	q = "�o���l������(20d6+)������"; break;
#else
			case RBE_EXP_20:	q = "lower experience (by 20d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_40:	q = "�o���l������(40d6+)������"; break;
#else
			case RBE_EXP_40:	q = "lower experience (by 40d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_80:	q = "�o���l������(80d6+)������"; break;
#else
			case RBE_EXP_80:	q = "lower experience (by 80d6+)"; break;
#endif

#ifdef JP
case RBE_DISEASE:	q = "�a�C�ɂ���"; break;
#else
			case RBE_DISEASE:	q = "disease"; break;
#endif

#ifdef JP
case RBE_TIME:      q = "���Ԃ��t�߂肳����"; break;
#else
			case RBE_TIME:      q = "time"; break;
#endif

#ifdef JP
case RBE_EXP_VAMP:  q = "�����͂��z������"; break;
#else
			case RBE_EXP_VAMP:  q = "drain life force"; break;
#endif

#ifdef JP
case RBE_DR_MANA:  q = "���͂�D��"; break;
#else
			case RBE_DR_MANA:  q = "drain mana force"; break;
#endif

#ifdef JP
			case RBE_INERTIA:  q = "����������"; break;
			case RBE_STUN:     q = "�N�O�Ƃ�����"; break;
#else
			case RBE_INERTIA:  q = "slow"; break;
			case RBE_STUN:     q = "stun"; break;
#endif

///mod131230 �����X�^�[�U���G�t�F�N�g�ǉ�
#ifdef JP
			case RBE_CHAOS:  q = "�J�I�X�ōU������"; break;
			case RBE_ELEMENT:     q = "�G�������g�ōU������"; break;
			case RBE_MUTATE:     q = "�ˑR�ψق�U������"; break;
			case RBE_SMITE:     q = "�򉻂���"; break;
			case RBE_DROWN:     q = "�M�ꂳ����"; break;
			case RBE_KILL:     q = "���ɗU��"; break;
			case RBE_CURSE:     q = "��"; break;
			case RBE_PHOTO:     q = "�ʐ^�B�e����"; break;
			case RBE_MELT:     q = "��������"; break;
			case RBE_BLEED:     q = "����𕉂킹��"; break;
			case RBE_INSANITY:     q = "���C�������炷"; break;
			case RBE_HUNGER:     q = "�Q��������炷"; break;



#else
			default: q = "OTHER"; break;

#endif










		}


#ifdef JP
		if ( r == 0 ) hooked_roff( format("%^s��", wd_he[msex]) );

		/***�኱�\����ύX ita ***/

			/* Describe damage (if known) */
		if (d1 && d2 && (know_everything || know_damage(r_idx, m)))
		  {
		    
		    /* Display the damage */
		    hooked_roff(format(" %dd%d ", d1, d2));
		    hooked_roff("�̃_���[�W��");
		  }
		/* Hack -- force a method */
		if (!p) p = "������Ȃ��Ƃ�����";

		/* Describe the method */
		/* XX����YY��/XX����YY����/XX��/XX���� */

		///mod131231 �����X�^�[�U���̎v���o����u�����āv���폜���Ă݂�
		if(method != RBM_HIT)
		{

			if(q) jverb( p ,jverb_buf, JVERB_TO);
			else if(r!=n-1) jverb( p ,jverb_buf, JVERB_AND);
			else strcpy(jverb_buf, p);

			hooked_roff(jverb_buf);
		}
		/* Describe the effect (if any) */
		if (q)
		{
		  if(r!=n-1) jverb( q,jverb_buf, JVERB_AND);
		  else strcpy(jverb_buf,q); 
		  hooked_roff(jverb_buf);
		}
		if(r!=n-1) hooked_roff("�A");
#else
		/* Introduce the attack description */
		if (!r)
		{
			hooked_roff(format("%^s can ", wd_he[msex]));
		}
		else if (r < n-1)
		{
			hooked_roff(", ");
		}
		else
		{
			hooked_roff(", and ");
		}


		/* Hack -- force a method */
		if (!p) p = "do something weird";

		/* Describe the method */
		hooked_roff(p);


		/* Describe the effect (if any) */
		if (q)
		{
			/* Describe the attack type */
			hooked_roff(" to ");
			hooked_roff(q);

			/* Describe damage (if known) */
			if (d1 && d2 && (know_everything || know_damage(r_idx, m)))
			{
				/* Display the damage */
				hooked_roff(" with damage");
				hooked_roff(format(" %dd%d", d1, d2));
			}
		}
#endif



		/* Count the attacks as printed */
		r++;
	}

	/* Finish sentence above */
	if (r)
	{
#ifdef JP
		hooked_roff("�B");
#else
		hooked_roff(".  ");
#endif

	}

	/* Notice lack of attacks */
	else if (flags1 & RF1_NEVER_BLOW)
	{
#ifdef JP
		hooked_roff(format("%^s�͕����I�ȍU�����@�������Ȃ��B", wd_he[msex]));
#else
		hooked_roff(format("%^s has no physical attacks.  ", wd_he[msex]));
#endif

	}

	/* Or describe the lack of knowledge */
	else
	{
#ifdef JP
		hooked_roff(format("%s�U���ɂ��Ă͉����m��Ȃ��B", wd_his[msex]));
#else
		hooked_roff(format("Nothing is known about %s attack.  ", wd_his[msex]));
#endif

	}


	/*
	 * Notice "Quest" monsters, but only if you
	 * already encountered the monster.
	 */
	if ((flags1 & RF1_QUESTOR) && ((r_ptr->r_sights) && (r_ptr->max_num) && ((r_idx == MON_OBERON) || (r_idx == MON_SERPENT) ||(r_idx == MON_YUKARI) )))
	{
#ifdef JP
		hook_c_roff(TERM_VIOLET, "���̃����X�^�[�������|���ׂ��{�X�ł���炵���E�E");
#else
		hook_c_roff(TERM_VIOLET, "You feel an intense desire to kill this monster...  ");
#endif

	}

	else if (flags7 & RF7_GUARDIAN)
	{
#ifdef JP
		hook_c_roff(TERM_L_RED, "���̃����X�^�[�̓_���W�����̎�ł���B");
#else
		hook_c_roff(TERM_L_RED, "This monster is the master of a dungeon.");
#endif

	}


	/* All done */
	hooked_roff("\n");

}



/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
/*:::�����X�^�[�̎v���o�̈�s�ڂ��L�q����*/
void roff_top(int r_idx)
{
	monster_race	*r_ptr = &r_info[r_idx];

	byte		a1, a2;
	char		c1, c2;
	char        buf2[255];

	/* Access the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Access the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

#ifndef JP
	/* A title (use "The" for non-uniques) */
	if (!(r_ptr->flags1 & RF1_UNIQUE))
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}
#endif

	///mod140712 �t�r�_�̎v���o�\�L����
	/*:::�t�r�_�̎v���o�\���@r_idx������񂪂Ȃ��̂�m_list���đ������Ė��O�𓾂Ă���B
	/*:::'/'�R�}���h�⃂���X�^�[��l����͕t�r�_���Q�Ƃł��Ȃ��悤�ɂ��Ă���̂ł����ɗ���Ƃ��͕K���t���A�ɊY���t�r�_�����Č��A�C�e���������Ă���͂�*/
	/*:::..�Ǝv������T�u�E�B���h�E�Ɏv���o�\���ɂ��Ă����ĕt�r�_�Ƀ^�[�Q�b�g�����܂܃t���A�ړ������炻���Ȃ�Ȃ��B��ނȂ��K���ȃ��b�Z�[�W�����ɓ����*/
	if(r_idx >= MON_TSUKUMO_WEAPON1 && r_idx <= MON_TSUKUMO_WEAPON5 )
	{
		int i;
		monster_type *m_ptr;
		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];
			if (m_ptr->r_idx == r_idx ) break; 
		}
		if(i == m_max || !m_ptr->hold_o_idx)
		{
			if(p_ptr->pclass == CLASS_TSUKUMO_MASTER) sprintf(buf2,"(�ȑO�̕t�r�_)");
			else sprintf(buf2,"(�ȑO���͕t�^��������)");
		}
		else 
			object_desc(buf2,&o_list[m_ptr->hold_o_idx],OD_NAME_ONLY);
		Term_addstr(-1, TERM_WHITE, buf2);
	}
	///mod140719 �����_�����j�[�N
	else if(IS_RANDOM_UNIQUE_IDX(r_idx))
	{
		sprintf(buf2,"%s",random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]);
		Term_addstr(-1, TERM_WHITE, buf2);
	}
	///mod150928 �ϐg�������X�^�[
	else if(r_idx == MON_EXTRA_FIELD)
	{
		sprintf(buf2,"%s",extra_mon_name);
		Term_addstr(-1, TERM_WHITE, buf2);
	}

	/* Dump the name */
	else 
		Term_addstr(-1, TERM_WHITE, (r_name + r_ptr->name));

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_add_bigch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
	Term_add_bigch(a2, c2);
	Term_addstr(-1, TERM_WHITE, "'):");

	/* Wizards get extra info */
	/*:::�E�B�U�[�h���[�h���̓����X�^�[�C���f�b�N�X���\�������*/
	if (p_ptr->wizard)
	{
		char buf[6];

		sprintf(buf, "%d", r_idx);

		Term_addstr(-1, TERM_WHITE, " (");
		Term_addstr(-1, TERM_L_BLUE, buf);
		Term_addch(TERM_WHITE, ')');
	}
}



/*
 * Hack -- describe the given monster race at the top of the screen
 */
/*:::���C���E�B���h�E�㕔�Ƀ����X�^�[�̎v���o��\������*/
void screen_roff(int r_idx, int mode)
{
	/* Flush messages */
	msg_print(NULL);

	/* Begin recall */
	Term_erase(0, 1, 255);

	hook_c_roff = c_roff;

	/* Recall monster */
	roff_aux(r_idx, mode);

	/* Describe monster */
	roff_top(r_idx);
}




/*
 * Hack -- describe the given monster race in the current "term" window
 */
/*:::�����X�^�[�̎v���o��\������*/
void display_roff(int r_idx)
{
	int y;

	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* Begin recall */
	Term_gotoxy(0, 1);

	hook_c_roff = c_roff;

	/* Recall monster */
	roff_aux(r_idx, 0);

	/* Describe monster */
	roff_top(r_idx);
}



/*
 * Hack -- output description of the given monster race
 */
/*:::�����X�^�[�̊��S�Ȏv���o���X�|�C���[�Ƃ��ďo�͂���*/
void output_monster_spoiler(int r_idx, void (*roff_func)(byte attr, cptr str))
{
	hook_c_roff = roff_func;

	/* Recall monster */
	roff_aux(r_idx, 0x03);
}

/*:::�_���W�����p�����X�^�[hook WILD_ONLY�͏o�Ȃ�*/
bool mon_hook_dungeon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!(r_ptr->flags8 & RF8_WILD_ONLY))
		return TRUE;
	else
	{
		dungeon_info_type *d_ptr = &d_info[dungeon_type];
		if ((d_ptr->mflags8 & RF8_WILD_MOUNTAIN) &&
		    (r_ptr->flags8 & RF8_WILD_MOUNTAIN)) return TRUE;
		return FALSE;
	}
}

/*:::�r��i�C�j�p�����X�^�[hook*/
static bool mon_hook_ocean(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_OCEAN)
		return TRUE;
	else
		return FALSE;
}

/*:::�r��i�󂢐��j�p�����X�^�[hook*/

static bool mon_hook_shore(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_SHORE)
		return TRUE;
	else
		return FALSE;
}

/*:::�r��i�r�n�j�p�����X�^�[hook*/
static bool mon_hook_waste(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & (RF8_WILD_WASTE | RF8_WILD_ALL))
		return TRUE;
	else
		return FALSE;
}

/*:::���p�����X�^�[hook*/
static bool mon_hook_town(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//if (r_ptr->flags8 & (RF8_WILD_TOWN | RF8_WILD_ALL))
	///mod141230 �l����������Ƃ炵���Ȃ��̂ŊX�Z�݂̃����X�^�[�̂ݏo��
	if (r_ptr->flags8 & (RF8_WILD_TOWN))
		return TRUE;
	else
		return FALSE;
}

//v1.1.32�@����X�phook
//�_�Ђ�i�����ȂǓ���̃����X�^�[�����o�������Ȃ����������Ȃǈ�؂̃����X�^�[���o�������Ȃ��Ƃ��g���B
//������TRUE��Ԃ��������X�^�[�����o�����Ȃ��B
//�������Awilderness_gen()���Ń����X�^�[�𐶐��������Ƃ�dungeon()�ɓ���Ƃ�monster_level��0�Ƀ��Z�b�g�����̂ŁA
//�X������̎��Ԍo�߂ɂ�郂���X�^�[�����ł̓��x�������Ɉ����������Ăǂ݂̂��o��Ȃ��B
#define LEV_CHK_OK (r_ptr->level < MAX(10,max_dlv[DUNGEON_ANGBAND]))
static bool mon_hook_special_town(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if(r_ptr->flags1 & RF1_QUESTOR)
		return FALSE;

	switch(p_ptr->town_num)
	{
	case TOWN_EIENTEI:
		if(r_idx == MON_YOUKAI_RABBIT || r_idx == MON_YOUKAI_RABBIT2) return TRUE;
		if(r_idx == MON_BAMBOO_FAIRY) return TRUE;
		if((r_idx == MON_TEWI || r_idx == MON_UDONGE) && LEV_CHK_OK) return TRUE;
		break;
	case TOWN_KOUMA:
		if(r_idx == MON_FAIRY || r_idx == MON_FLOWER_FAIRY || r_idx == MON_D_FAIRY || r_idx == MON_CIRNO || r_idx == MON_FAIRY_MAID) return TRUE;
		if(r_idx == MON_HOHGOBLIN) return TRUE;
		if(r_idx == MON_WAKASAGI) return TRUE;
		if(r_idx == MON_MEIRIN && LEV_CHK_OK) return TRUE;
		if(r_idx == MON_KOAKUMA && LEV_CHK_OK) return TRUE;
		break;
	case TOWN_HAKUREI:
		if (r_ptr->flags8 & (RF8_WILD_TOWN))
			return TRUE;
		//������A�O�����A���ӂ͏o�₷��
		if((r_idx == MON_ORIN ||  r_idx == MON_SHINMYOUMARU ||r_idx == MON_3FAIRIES || r_idx == MON_AUNN) && LEV_CHK_OK)
			return TRUE;

		//v1.1.43 �N���E���s�[�X���o��悤�ɂ���
		if (r_idx == MON_CLOWNPIECE && LEV_CHK_OK)	return TRUE;

		//���z���j�[�N�S���o��悤�ɂ��悤���Ǝv��������߂�
		//if(is_gen_unique(r_idx) && one_in_(10) && LEV_CHK_OK) 
		//	return TRUE;
		break;
	case TOWN_MYOURENJI:
		if (r_ptr->flags8 & (RF8_WILD_TOWN))
			return TRUE;
		if(r_idx >= MON_NAZRIN && r_idx <= MON_KYOUKO && LEV_CHK_OK)
			return TRUE;
		if(r_idx == MON_AUNN && LEV_CHK_OK) 
			return TRUE;
		if(r_idx == MON_YOUKAI_MOUSE) 
			return TRUE;
		break;
	case TOWN_MORIYA:
		if (r_ptr->flags8 & (RF8_WILD_TOWN))
			return TRUE;
		if(r_idx == MON_KAPPA || r_idx == MON_AUNN) 
			return TRUE;
		if(r_idx >= MON_SHIZUHA && r_idx <= MON_SUWAKO && LEV_CHK_OK)
			return TRUE;
		break;

	case TOWN_HIGAN:
		if (r_idx == MON_EIKI || r_idx == MON_KUTAKA)
			return TRUE;
		break;

	case TOWN_KOURIN:
		if (r_idx == MON_LAIKA) return TRUE;
		break;

	default:
		break;

		//��:�l����mon_hook_town�ŏ�������AWILD_TOWN�����X�^�[�S�ďo��

	}

	return FALSE;
}
#undef LEV_CHK_OK


/*:::�r��i�X�j�p�����X�^�[hook*/
static bool mon_hook_wood(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & (RF8_WILD_WOOD | RF8_WILD_ALL))
		return TRUE;
	else
		return FALSE;
}

/*:::�r��i�n��j�p�����X�^�[hook*/
static bool mon_hook_volcano(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_VOLCANO)
		return TRUE;
	else
		return FALSE;
}

/*:::�r��i�R�j�p�����X�^�[hook*/
static bool mon_hook_mountain(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & RF8_WILD_MOUNTAIN)
		return TRUE;
	else
		return FALSE;
}

/*:::�r��i���n�j�p�����X�^�[hook*/

static bool mon_hook_grass(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags8 & (RF8_WILD_GRASS | RF8_WILD_ALL))
		return TRUE;
	else
		return FALSE;
}

/*:::���������X�^�[�phook*/
static bool mon_hook_deep_water(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	//v1.1.64 �n��̐[�����n�`��WILD_OCEAN����WILD_ONLY�̃����X�^�[���o�Ȃ��̂ŏ�����������
	//if (!mon_hook_dungeon(r_idx)) return FALSE;
	if (dun_level && !mon_hook_dungeon(r_idx)) return FALSE;

	if (r_ptr->flags7 & RF7_AQUATIC)
		return TRUE;
	else
		return FALSE;
}

/*:::�󂢐��p�����X�^�[�phook�@�r���p�ƉΉ��I�[�������ȊO�͒ʂ�*/

static bool mon_hook_shallow_water(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if (r_ptr->flags2 & RF2_AURA_FIRE)
		return FALSE;
	else
		return TRUE;
}

///mod131231 �����X�^�[�t���O�ύX
static bool mon_hook_lava(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if (((r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK) ||
	     (r_ptr->flags7 & RF7_CAN_FLY)) &&
	    !(r_ptr->flags2 & RF2_AURA_COLD))
	    //!(r_ptr->flags3 & RF3_AURA_COLD))
		return TRUE;
	else
		return FALSE;
}
static bool mon_hook_acid(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_ACID_MASK) || (r_ptr->flags7 & RF7_CAN_FLY))
		return TRUE;
	else
		return FALSE;
}

static bool mon_hook_pois(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_ACID_MASK) || (r_ptr->flags7 & RF7_CAN_FLY))
		return TRUE;
	else
		return FALSE;
}
static bool mon_hook_cold(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_COLD_MASK))
		return TRUE;
	else
		return FALSE;
}
static bool mon_hook_elec(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!mon_hook_dungeon(r_idx)) return FALSE;

	if ((r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK))
		return TRUE;
	else
		return FALSE;
}





static bool mon_hook_floor(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (!(r_ptr->flags7 & RF7_AQUATIC) ||
	    (r_ptr->flags7 & RF7_CAN_FLY))
		return TRUE;
	else
		return FALSE;
}

/*:::get_mon_num_prep()�p��hook��ݒ肷�邽�߂̊֐�*/
/*:::wilderness[][].terrain�ɉ����n�`���Ƃɏo���\�����X�^�[����֐��̃A�h���X��Ԃ�*/
///system �r��p�̃����X�^�[��ތ��胋�[�`���̊�����@�t���[�o�[�̂��߂ɕύX���邩��
monster_hook_type get_monster_hook(void)
{

	if (!dun_level && !p_ptr->inside_quest)
	{
		switch (wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain)
		{
		case TERRAIN_TOWN:
			return (monster_hook_type)mon_hook_town;
		/*:::w_info.txt�̊X��terrain�l��2�̏ꍇ�����ɗ���B�����I�ɓG���o�Ȃ��E�E�H*/
		//v1.1.32�@���̂����ł͖��Ίy��{�[�_�C���ȂǕςȂ̂��o��̂ŐV��������n�`�������
		case TERRAIN_DEEP_WATER:
			return (monster_hook_type)mon_hook_ocean;
		case TERRAIN_SHALLOW_WATER:
		case TERRAIN_SWAMP:
			return (monster_hook_type)mon_hook_shore;
		case TERRAIN_DIRT:
		case TERRAIN_DESERT:
			return (monster_hook_type)mon_hook_waste;
		case TERRAIN_GRASS:
			return (monster_hook_type)mon_hook_grass;
		case TERRAIN_TREES:
			return (monster_hook_type)mon_hook_wood;
		case TERRAIN_SHALLOW_LAVA:
		case TERRAIN_DEEP_LAVA:
			return (monster_hook_type)mon_hook_volcano;
		case TERRAIN_MOUNTAIN:
			return (monster_hook_type)mon_hook_mountain;
		case TERRAIN_SPECIAL_TOWN:
			return (monster_hook_type)mon_hook_special_town;

		default:
			return (monster_hook_type)mon_hook_dungeon;
		}
	}
	else
	{
		return (monster_hook_type)mon_hook_dungeon;
	}
}

/*:::get_mon_num_prep()�p��hook�֐��̃A�h���X��n�`�ɉ����ĕԂ��@�_���W��������p�H*/
///system �_���W�����phook�@���A�n��̏o���\����@�Ђ���Ƃ�����ύX���邱�Ƃ����邩��
monster_hook_type get_monster_hook2(int y, int x)
{
	feature_type *f_ptr = &f_info[cave[y][x].feat];

	/* Set the monster list */

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		/* Deep water */
		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			return (monster_hook_type)mon_hook_deep_water;
		}

		/* Shallow water */
		else
		{
			return (monster_hook_type)mon_hook_shallow_water;
		}
	}

	/* Lava */
	else if (have_flag(f_ptr->flags, FF_LAVA))
	{
		return (monster_hook_type)mon_hook_lava;
	}
	//v1.1.85
	else if (have_flag(f_ptr->flags, FF_POISON_PUDDLE))
	{
		return (monster_hook_type)mon_hook_pois;
	}
	else if (have_flag(f_ptr->flags, FF_ACID_PUDDLE))
	{
		return (monster_hook_type)mon_hook_acid;
	}
	else if (have_flag(f_ptr->flags, FF_COLD_PUDDLE))
	{
		return (monster_hook_type)mon_hook_cold;
	}
	else if (have_flag(f_ptr->flags, FF_ELEC_PUDDLE))
	{
		return (monster_hook_type)mon_hook_elec;
	}






	else return (monster_hook_type)mon_hook_floor;
}


void set_friendly(monster_type *m_ptr)
{
	m_ptr->smart |= SM_FRIENDLY;
}

void set_pet(monster_type *m_ptr)
{
	if (!is_pet(m_ptr)) check_pets_num_and_align(m_ptr, TRUE);

	/* Check for quest completion */
	check_quest_completion(m_ptr);

	m_ptr->smart |= SM_PET;
	if (!(r_info[m_ptr->r_idx].flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS)))
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
}

/*
 * Makes the monster hostile towards the player
 */
/*:::�����X�^�[��G�ΓI�ɂ���*/
void set_hostile(monster_type *m_ptr)
{
	int i;
	if (p_ptr->inside_battle) return;

	//�P��(��)�͓G�΂��Ȃ�
	if(m_ptr->r_idx == MON_MASTER_KAGUYA) return;

	if (is_pet(m_ptr)) check_pets_num_and_align(m_ptr, FALSE);
	///mod141129 �F�D�I�ȓG���G�΂��鎞�A�������Ă��鑼�̗F�D�I�ȓG���G�΂���
	if(is_friendly(m_ptr) )
	{
		int add = 0;
		m_ptr->smart &= ~SM_FRIENDLY;
		for (i = 1; i < m_max; i++)
		{
			monster_type *m2_ptr = &m_list[i];
			monster_race *r2_ptr;
			if(!m2_ptr->r_idx) continue;
			if(m2_ptr->r_idx == MON_MASTER_KAGUYA) continue;
			r2_ptr = &r_info[m2_ptr->r_idx];

			///mod150830
			if(r2_ptr->flags2 & RF2_EMPTY_MIND) continue;

			if(is_friendly(m2_ptr) && (m2_ptr->cdis <= MAX_SIGHT) && !MON_CONFUSED(m2_ptr) && !MON_CSLEEP(m2_ptr))
			{
				m2_ptr->smart &= ~SM_FRIENDLY;
				add++;
			}
			//�T�g���R���N���t�g�ƌϒK�푈�ł͗F�D�I�ȓG�ɍU������ƑS�Ă̗F�D�G���G�΂���
			//v1.1.91 ���N�U�푈��
			else if(is_friendly(m2_ptr) && (p_ptr->inside_quest == QUEST_SATORI || p_ptr->inside_quest == QUEST_KORI || p_ptr->inside_quest == QUEST_YAKUZA_1))
			{

				m2_ptr->smart &= ~SM_FRIENDLY;
				add++;
			}
		}
		if(add) msg_format("���̖͂���%s���G�ɉ�����C������E�E",(add > 1)?"�B":"");
	}

	m_ptr->smart &= ~SM_PET;
	m_ptr->smart &= ~SM_FRIENDLY;
}


/*
 * Anger the monster
 */
 /*:::�F�D�I�ȓG��{�点��B�_���[�W��^�����Ƃ������ɗ���*/
//�Q�ĂĂ��N�����Ȃ��炵��
void anger_monster(monster_type *m_ptr)
{
	if (p_ptr->inside_battle) return;

	if(m_ptr->r_idx == MON_MASTER_KAGUYA) return;

	if(is_pet(m_ptr))
	{
		set_deity_bias(DBIAS_COSMOS, -1);
		set_deity_bias(DBIAS_REPUTATION, -1);
	}

	else if (is_friendly(m_ptr))
	{
		char m_name[80];

		monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%^s�͓{�����I", m_name);
#else
		msg_format("%^s gets angry!", m_name);
#endif
		set_deity_bias(DBIAS_COSMOS, -1);
		set_deity_bias(DBIAS_WARLIKE, 1);
		set_deity_bias(DBIAS_REPUTATION, -1);

		set_hostile(m_ptr);

		///del131214 virtue
		//chg_virtue(V_INDIVIDUALISM, 1);
		//chg_virtue(V_HONOUR, -1);
		//chg_virtue(V_JUSTICE, -1);
		//chg_virtue(V_COMPASSION, -1);
	}
}


/*
 * Check if monster can cross terrain
 */
/*:::�����X�^�[���n���n�`���ǂ�������*/
bool monster_can_cross_terrain(s16b feat, monster_race *r_ptr, u16b mode)
{
	feature_type *f_ptr = &f_info[feat];

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_RIDING))
		{
			if (!(r_ptr->flags7 & RF7_CAN_FLY)) return FALSE;
		}
		else
		{
			if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
		}
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && (r_ptr->flags7 & RF7_CAN_FLY)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && (r_ptr->flags7 & RF7_CAN_SWIM)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS))
	{
		if ((r_ptr->flags2 & RF2_PASS_WALL) && (!(mode & CEM_RIDING) || p_ptr->pass_wall)) return TRUE;
	}

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	/* Some monsters can walk on mountains */
	if (have_flag(f_ptr->flags, FF_MOUNTAIN) && (r_ptr->flags8 & RF8_WILD_MOUNTAIN)) return TRUE;

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		if (!(r_ptr->flags7 & RF7_AQUATIC))
		{
			/* Deep water */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Shallow water */
			else if (r_ptr->flags2 & RF2_AURA_FIRE) return FALSE;
		}
	}

	/* Aquatic monster into non-water? */
	else if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	/* Lava */
	if (have_flag(f_ptr->flags, FF_LAVA))
	{
		if (!(r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)) return FALSE;
	}
	if (have_flag(f_ptr->flags, FF_POISON_PUDDLE))
	{
		if (!(r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)) return FALSE;
	}
	if (have_flag(f_ptr->flags, FF_ACID_PUDDLE))
	{
		if (!(r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)) return FALSE;
	}




	return TRUE;
}


/*
 * Strictly check if monster can enter the grid
 */
bool monster_can_enter(int y, int x, monster_race *r_ptr, u16b mode)
{
	cave_type *c_ptr = &cave[y][x];

	/* Player or other monster */
	if (player_bold(y, x)) return FALSE;
	if (c_ptr->m_idx) return FALSE;

	return monster_can_cross_terrain(c_ptr->feat, r_ptr, mode);
}


/*
 * Check if this monster has "hostile" alignment (aux)
 */
/*:::�������̓`�F�b�N�@�׈��ƑP�ǂ̌���*/

static bool check_hostile_align(byte sub_align1, byte sub_align2)
{

	//v1.1.91 �N�G�X�g�_���W�����ŕ������͂̃����X�^�[�����荇���p
	if ((sub_align1 & SUB_ALIGN_QUEST_MASK) && (sub_align1 & SUB_ALIGN_QUEST_MASK))
	{
		if (sub_align1 != sub_align2) return TRUE;
		else return FALSE;
	}

	if (sub_align1 != sub_align2)
	{
		if (((sub_align1 & SUB_ALIGN_EVIL) && (sub_align2 & SUB_ALIGN_GOOD)) ||
			((sub_align1 & SUB_ALIGN_GOOD) && (sub_align2 & SUB_ALIGN_EVIL)))
			return TRUE;
	}

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Check if two monsters are enemies
 */
/*:::�����X�^�[��̂��G�Ώ�Ԃ��ǂ�������*/
/*:::�ڍז���*/
///system �A���o�[���Q���J�������萨�͓���������Ȃ炱���𑀍삷�ׂ���
bool are_enemies(monster_type *m_ptr, monster_type *n_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_race *s_ptr = &r_info[n_ptr->r_idx];

	///mod150616 ���Z��ύX
	if (p_ptr->inside_battle)
	{
		if (is_pet(m_ptr) || is_pet(n_ptr)) return FALSE;
		//���`�[���̃t���O�������ĂȂ���ΓG��
		return (!(m_ptr->mflag & n_ptr->mflag & MFLAG_BATTLE_MON_MASK));
	}

	//�p���X�B�Ɏ��i�S�����ꂽ�����X�^�[�̏���
	if (p_ptr->pclass == CLASS_PARSEE)
	{
		if (p_ptr->tim_general[0])
		{

			if (m_ptr->cdis <= MAX_SIGHT && !(r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) return TRUE;
			if (n_ptr->cdis <= MAX_SIGHT && !(s_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND))) return TRUE;
		}
		if ((m_ptr->mflag & MFLAG_SPECIAL) && (r_ptr->level < s_ptr->level)) return TRUE;
		if ((n_ptr->mflag & MFLAG_SPECIAL) && (r_ptr->level > s_ptr->level)) return TRUE;

	}

	/*:::�X�ɏo�郂���X�^�[�͓G�΂��Ȃ�*/
	if ((r_ptr->flags8 & (RF8_WILD_TOWN | RF8_WILD_ALL))
	    && (s_ptr->flags8 & (RF8_WILD_TOWN | RF8_WILD_ALL)))
	{
		if (!is_pet(m_ptr) && !is_pet(n_ptr)) return FALSE;
	}

	//v1.1.15 ���@�̓��ӂȐE�̏ꍇ�A�z�����m�����܂��Ȃ��悤�ɂ���
	if(cp_ptr->magicmaster && is_pet(m_ptr) && is_pet(n_ptr))
		return FALSE;


	//�ʂ��̐��̕s���̎�
	if((m_ptr->mflag & MFLAG_NUE_UNKNOWN || n_ptr->mflag & MFLAG_NUE_UNKNOWN)) return TRUE;

	//v1.1.43 �N���E���s�[�X�̋��C�̏���
	if ((m_ptr->mflag & MFLAG_LUNATIC_TORCH || n_ptr->mflag & MFLAG_LUNATIC_TORCH)) return TRUE;

	//v1.1.73 �Ǘ����
	if (n_ptr->mflag & MFLAG_ISOLATION) return TRUE;


	//俎q�͌��z���j�[�N����ǂ������񂳂ꒆ
	if((m_ptr->r_idx == MON_SUMIREKO) && is_gen_unique(n_ptr->r_idx)
		||(n_ptr->r_idx == MON_SUMIREKO) && is_gen_unique(m_ptr->r_idx))
		return TRUE;

	/*:::���ׂ͖������ɂ����鑶�݂ɓG�΂���*/
	if((m_ptr->r_idx == MON_SEIJA || m_ptr->r_idx == MON_SEIJA_D) && (n_ptr->r_idx != MON_SEIJA && n_ptr->r_idx != MON_SEIJA_D))
		return TRUE;
	if((n_ptr->r_idx == MON_SEIJA || n_ptr->r_idx == MON_SEIJA_D) && (m_ptr->r_idx != MON_SEIJA && m_ptr->r_idx != MON_SEIJA_D))
		return TRUE;
	/*:::�Ԕ؊�̓��̓��ꏈ���@���ɓG�΂���Ȃ瓯���悤�ɓG��  ...���ʂ̔z�������Ŗ��Ȃ��C������*/
	if(m_ptr->r_idx == MON_BANKI_HEAD_1 || m_ptr->r_idx == MON_BANKI_HEAD_2 || n_ptr->r_idx == MON_BANKI_HEAD_1 || n_ptr->r_idx == MON_BANKI_HEAD_2)
	{
		if(is_hostile(m_ptr) || is_hostile(n_ptr)) return TRUE;
		else return FALSE;
	}
	/*:::���ꏈ���@�N�G�X�g���̗��͗d���ςɓG�΂��Ȃ�*/
	if(p_ptr->inside_quest == QUEST_KORI)
	{
		if(m_ptr->r_idx == MON_RAN && n_ptr->mflag2 & MFLAG2_CHAMELEON) return FALSE;
		if(n_ptr->r_idx == MON_RAN && m_ptr->mflag2 & MFLAG2_CHAMELEON) return FALSE;

	}

	/*:::�������̓`�F�b�N*/
	/* Friendly vs. opposite aligned normal or pet */
	if (check_hostile_align(m_ptr->sub_align, n_ptr->sub_align))
	{
		if (!(m_ptr->mflag2 & MFLAG2_CHAMELEON) || !(n_ptr->mflag2 & MFLAG2_CHAMELEON)) return TRUE;
	}

	//�F�D�I�łȂ����C�}���͂����鑶�݂ɓG�΂���
	//..�悤�ɂ��Ă������A�����������������̏������������X�^�[�ɍU������Ă��܂��̂Ŗ������̂ݒʏ폈���ɖ߂�
	if(m_ptr->r_idx == MON_REIMU && is_hostile(m_ptr) || n_ptr->r_idx == MON_REIMU && is_hostile(n_ptr))
		return TRUE;
/*
	if(m_ptr->r_idx == MON_MARISA && is_hostile(m_ptr) || n_ptr->r_idx == MON_MARISA && is_hostile(n_ptr))
		return TRUE;
*/




	/* Hostile vs. non-hostile */
	/*:::���̓G�Ύ҂ƗF�D�Ȏ҂͓G�Ί֌W*/
	if (is_hostile(m_ptr) != is_hostile(n_ptr))
	{
		return TRUE;
	}

	/* Default */
	return FALSE;
}





///mod140420 @�̐��͔���@���̂Ƃ��끗�̏�������CHAOS��COSMOS�ǂ��瑤���o�邩�̔���ɂ����g���ĂȂ�
bool you_are_human_align(void)
{

	//v1.1.47 �ؐ�͔j�׎�_���������Œ��������o��悤�ɂ��Ƃ�
	if (p_ptr->pclass == CLASS_KASEN && !is_special_seikaku(SEIKAKU_SPECIAL_KASEN)) return TRUE;

	//�b��@�Ƃ肠�����j�ׂɎア���͗d����
	if(is_hurt_holy() > 0) return FALSE;
	else return TRUE;

}

/*
 * Check if this monster race has "hostile" alignment
 * If user is player, m_ptr == NULL.
 */

/*:::�w�肵�������X�^�[�����������͕ʂ̎w�胂���X�^�[�ɓG�΂��ǂ������肷��*/
///sys �����X�^�[�G�Ώ����ݒ蕔
bool monster_has_hostile_align(monster_type *m_ptr, int pa_good, int pa_evil, monster_race *r_ptr)
{
	byte sub_align1 = SUB_ALIGN_NEUTRAL;
	byte sub_align2 = SUB_ALIGN_NEUTRAL;

	if (m_ptr) /* For a monster */
	{
		sub_align1 = m_ptr->sub_align;
	}
	else /* For player */
	{
		/*
		if (p_ptr->align >= pa_good) sub_align1 |= SUB_ALIGN_GOOD;
		if (p_ptr->align <= pa_evil) sub_align1 |= SUB_ALIGN_EVIL;
		*/
		///mod140420 ���̏������Ȃǂ̎b�萨�͔���
		if(you_are_human_align()) sub_align1 |= SUB_ALIGN_GOOD;
		else sub_align1 |= SUB_ALIGN_EVIL;
	}

	/* Racial alignment flags */
	if (r_ptr->flags3 & RF3_ANG_CHAOS) sub_align2 |= SUB_ALIGN_EVIL;
	if (r_ptr->flags3 & RF3_ANG_COSMOS) sub_align2 |= SUB_ALIGN_GOOD;

	if (check_hostile_align(sub_align1, sub_align2)) return TRUE;

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Is the monster "alive"?
 *
 * Used to determine the message to print for a killed monster.
 * ("dies", "destroyed")
 */
///mon flag �������t���O����
///mod140222 �l�ԁA�����Ȃǂ̃t���O�������Ȃ��_�i�͖����������ɂ���悤�ɂ���
bool monster_living(monster_race *r_ptr)
{
	/* Non-living, undead, or demon */
	if (r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING))
		return FALSE;
	else if(r_ptr->flags3 & (RF3_KWAI | RF3_DEMIHUMAN | RF3_DRAGON | RF3_ANIMAL | RF3_HUMAN))
		return TRUE;
	else if (r_ptr->flags3 & RF3_DEITY )
		return FALSE;
	else
		return TRUE;
}


/*
 * Is this monster declined to be questor or bounty?
 */
/*:::�����N�G�ɂ��܋���ɂ��Ȃ�Ȃ��z�𔻒�@���̂Ƃ���o�[�m�[�������p�[�g�̂�*/
bool no_questor_or_bounty_uniques(int r_idx)
{
	switch (r_idx)
	{
	/*
	 * Decline them to be questor or bounty because they use
	 * special motion "split and combine"
	 */
	case MON_BANORLUPART:
	case MON_BANOR:
	case MON_LUPART:
		return TRUE;
	default:
		return FALSE;
	}
}
