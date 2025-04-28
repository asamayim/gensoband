/* File: bldg.c */

/*
 * Purpose: Building commands
 * Created by Ken Wigle for Kangband - a variant of Angband 2.8.3
 * -KMW-
 *
 * Rewritten for Kangband 2.8.3i using Kamband's version of
 * bldg.c as written by Ivan Tkatchev
 *
 * Changed for ZAngband by Robert Ruehlmann
 */
/*:::���̌����֘A�@������\��*/


#include "angband.h"

/* hack as in leave_store in store.c */
static bool leave_bldg = FALSE;

/*:::�푰�A�E�ƁA�̈悪��v���Ă邩�ǂ�������H�ڍוs���@�폜�\��*/
static bool is_owner(building_type *bldg)
{

	//����S�Ẵv���C���[�������o�[�����Ƃ��Ă���
	return TRUE;
/*
	if (bldg->member_class[p_ptr->pclass] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if (bldg->member_race[p_ptr->prace] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if ((is_magic(p_ptr->realm1) && (bldg->member_realm[p_ptr->realm1] == BUILDING_OWNER)) ||
		(is_magic(p_ptr->realm2) && (bldg->member_realm[p_ptr->realm2] == BUILDING_OWNER)))
	{
		return (TRUE);
	}

	return (FALSE);
*/
}

/*:::�푰�A�E�ƁA�̈悪��v���Ă邩�ǂ�������H�@�폜�\��*/
/*:::�����H���ŐH���\���ǂ����������Ŕ��肳��Ă���炵��*/
static bool is_member(building_type *bldg)
{
	return (TRUE);
/*
	if (bldg->member_class[p_ptr->pclass])
	{
		return (TRUE);
	}

	if (bldg->member_race[p_ptr->prace])
	{
		return (TRUE);
	}

	if ((is_magic(p_ptr->realm1) && bldg->member_realm[p_ptr->realm1]) ||
	    (is_magic(p_ptr->realm2) && bldg->member_realm[p_ptr->realm2]))
	{
		return (TRUE);
	}


	if (p_ptr->pclass == CLASS_SORCERER)
	{
		int i;
		bool OK = FALSE;
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if (bldg->member_realm[i+1]) OK = TRUE;
		}
		return OK;
	}
	return (FALSE);
	*/
}


/*
 * Clear the building information
 */
 /*:::�{�݂̒��ɂ���Ƃ��̂��߂̎w��͈͉�ʃN���A min_row�`max_row�̑S�Ă̍s���s���܂ŋ󔒂ɂ���*/
void clear_bldg(int min_row, int max_row)
{
	int   i;

	for (i = min_row; i <= max_row; i++)
		prt("", i, 0);
}
/*:::�莝���̋��z��\���i��ʌ����p�j*/
///mod151114 bldg2.c�ł��g���悤��extern����
void building_prt_gold(void)
{
	char tmp_str[80];

#ifdef JP
prt("�莝���̂���: ", 23,53);
#else
	prt("Gold Remaining: ", 23, 53);
#endif


	sprintf(tmp_str, "%9ld", (long)p_ptr->au);
	prt(tmp_str, 23, 68);
}


/*
 * Display a building.
 */
/*:::�����őI�ׂ�s���ꗗ�\���B�푰�E�Ƃ��Ƃ̃R�X�g�̈Ⴂ�◘�p�ۂȂǋ��*/
///sys �����֌W
///mod160306 �O������Q�Ƃł���悤�ɂ���
void show_building(building_type* bldg)
{
	char buff[20];
	int i;
	byte action_color;
	char tmp_str[80];

	Term_clear();
	//sprintf(tmp_str, "%s (%s) %35s", bldg->owner_name, bldg->owner_race, bldg->name);
	///mod140117 �����\�L����I�[�i�[�푰���폜���I�[�i�[���ƌ��������t�ɂ���

	//v1.1.85 �[����Ǔے��ɓ�������X�����ς��
	if (p_ptr->town_num == TOWN_HITOZATO && f_info[cave[py][px].feat].subtype == 12 && is_midnight())
	{
		sprintf(tmp_str, "�y�\��z�Ǔے�  %35s", bldg->owner_name);
	}
	else
	{
		sprintf(tmp_str, "%s  %35s", bldg->name, bldg->owner_name);

	}
	prt(tmp_str, 2, 1);


	for (i = 0; i < 8; i++)
	{
		if (bldg->letters[i])
		{
			/*:::�N�ł����p�ł���ݔ�*/
			if (bldg->action_restr[i] == 0)
			{
				/*:::�����̂Ƃ�*/
				if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
					(!is_owner(bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				/*:::�L��/�����o�[*/
				else if (is_owner(bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->member_costs[i]);
#endif

				}
				/*:::�L��/�����o�[�ȊO*/
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->other_costs[i]);
#endif

				}
			}
			/*:::restr=1�̂Ƃ��A�����o�[�������p�ł��Ȃ��B�h�̐H���Ȃ�*/
			else if (bldg->action_restr[i] == 1)
			{
				if (!is_member(bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(�X)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
					(is_member(bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if (is_owner(bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->other_costs[i]);
#endif

				}
			}
			else
			{
				if (!is_owner(bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(�X)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if (bldg->member_costs[i] != 0)
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", (long int)bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", (long int)bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
			}

			sprintf(tmp_str," %c) %s %s", bldg->letters[i], bldg->act_names[i], buff);
			c_put_str(action_color, tmp_str, 19+(i/2), 35*(i%2));
		}
	}

#ifdef JP
prt(" ESC) �������o��", 23, 0);
#else
	prt(" ESC) Exit building", 23, 0);
#endif

}


/*
 * arena commands
 */
/*:::�A���[�i�����@*/
/*:::�A���[�i�ɓ���Ƃ��͂������̃t���O���Ă���ŊK����o�鑀��*/
//v1.1.51��nightmare_diary()�ɐV�A���[�i������B�����͂����g��Ȃ��̂ŃR�����g�A�E�g����B
#if 0
static void arena_comm(int cmd)
{
	monster_race    *r_ptr;
	cptr            name;


	switch (cmd)
	{
		case BACT_ARENA:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
			{
				clear_bldg(5, 19);
#ifdef JP
prt("�A���[�i�̗D���ҁI", 5, 0);
prt("���߂łƂ��I���Ȃ��͑S�Ă̓G��|���܂����B", 7, 0); 
prt("�܋��Ƃ��� $1,000,000 ���^�����܂��B", 8, 0);
#else
				prt("               Arena Victor!", 5, 0);
				prt("Congratulations!  You have defeated all before you.", 7, 0);
				prt("For that, receive the prize: 1,000,000 gold pieces", 8, 0);
#endif

				prt("", 10, 0);
				prt("", 11, 0);
				p_ptr->au += 1000000L;
#ifdef JP
msg_print("�X�y�[�X�L�[�ő��s");
#else
				msg_print("Press the space bar to continue");
#endif

				msg_print(NULL);
				p_ptr->arena_number++;
			}
			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
				if (p_ptr->arena_number < MAX_ARENA_MONS+2)
				{
#ifdef JP
msg_print("�N�̂��߂ɍŋ��̒���҂�p�ӂ��Ă������B");
#else
					msg_print("The strongest challenger is waiting for you.");
#endif

					msg_print(NULL);
#ifdef JP
					if (get_check("���킷�邩�ˁH"))
#else
					if (get_check("Do you fight? "))
#endif
					{	
#ifdef JP
                        msg_print("���ʂ��悢�B");
#else
					    msg_print("Die, maggots.");
#endif
						msg_print(NULL);
					
						p_ptr->exit_bldg = FALSE;
						reset_tim_flags();

						/* Save the surface floor as saved floor */
						prepare_change_floor_mode(CFM_SAVE_FLOORS);

						p_ptr->inside_arena = TRUE;
						p_ptr->leaving = TRUE;
						leave_bldg = TRUE;
					}
					else
					{
#ifdef JP
msg_print("�c�O���B");
#else
						msg_print("We are disappointed.");
#endif
					}
				}
				else
				{
#ifdef JP
msg_print("���Ȃ��̓A���[�i�ɓ���A���΂炭�̊ԉh���ɂЂ������B");
#else
					msg_print("You enter the arena briefly and bask in your glory.");
#endif

					msg_print(NULL);
				}
			}
			///class building ���b�g���ƋR���ȊO�͏�n�̂܂܃A���[�i�ɓ���Ȃ�
			else if (p_ptr->riding && (p_ptr->pclass != CLASS_BEASTMASTER) && (p_ptr->pclass != CLASS_CAVALRY))
			{
#ifdef JP
msg_print("�y�b�g�ɏ�����܂܂ł̓A���[�i�֓��ꂳ���Ă��炦�Ȃ������B");
#else
				msg_print("You don't have permission to enter with pet.");
#endif

				msg_print(NULL);
			}
			else
			{
				p_ptr->exit_bldg = FALSE;
				reset_tim_flags();

				/* Save the surface floor as saved floor */
				prepare_change_floor_mode(CFM_SAVE_FLOORS);

				p_ptr->inside_arena = TRUE;
				p_ptr->leaving = TRUE;
				leave_bldg = TRUE;
			}
			break;
		case BACT_POSTER:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
#ifdef JP
msg_print("���Ȃ��͏����҂��B �A���[�i�ł̃Z�����j�[�ɎQ�����Ȃ����B");
#else
				msg_print("You are victorious. Enter the arena for the ceremony.");
#endif

			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
#ifdef JP
msg_print("���Ȃ��͂��ׂĂ̓G�ɏ��������B");
#else
				msg_print("You have won against all foes.");
#endif
			}
			else
			{
				r_ptr = &r_info[arena_info[p_ptr->arena_number].r_idx];
				name = (r_name + r_ptr->name);
#ifdef JP
msg_format("%s �ɒ��킷����̂͂��Ȃ����H", name);
#else
				msg_format("Do I hear any challenges against: %s", name);
#endif
			}
			break;
///del131213 building �A���[�i�̃��[���\�� �g���ĂȂ��炵��
#if 0
		case BACT_ARENA_RULES:

			/* Save screen */
			screen_save();

			/* Peruse the arena help file */
#ifdef JP
(void)show_file(TRUE, "arena_j.txt", NULL, 0, 0);
#else
			(void)show_file(TRUE, "arena.txt", NULL, 0, 0);
#endif


			/* Load screen */
			screen_load();

			break;
#endif
	}
}
#endif


/*
 * display fruit for dice slots
 */
static void display_fruit(int row, int col, int fruit)
{
	switch (fruit)
	{
		case 0: /* lemon */
#ifdef JP
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " ������ ", row + 8, col);
#else
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " Lemon  ", row + 8, col);
#endif

			break;
		case 1: /* orange */
#ifdef JP
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   "�I�����W", row + 8, col);
#else
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   " Orange ", row + 8, col);
#endif

			break;
		case 2: /* sword */
#ifdef JP
			c_put_str(TERM_SLATE, "   ��   " , row, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 5, col);
			c_put_str(TERM_UMBER, " |=��=| " , row + 6, col);
			c_put_str(TERM_UMBER, "   ��   " , row + 7, col);
			prt(                  "   ��   " , row + 8, col);
#else
			c_put_str(TERM_SLATE, "   /\\   " , row, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 5, col);
			c_put_str(TERM_UMBER, " ###### " , row + 6, col);
			c_put_str(TERM_UMBER, "   ##   " , row + 7, col);
			prt(                  " Sword  " , row + 8, col);
#endif

			break;
		case 3: /* shield */
#ifdef JP
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  "   ��   ", row + 8, col);
#else
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  " Shield ", row + 8, col);
#endif

			break;
		case 4: /* plum */
#ifdef JP
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   " �v���� ", row + 8, col);
#else
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   "  Plum  ", row + 8, col);
#endif

			break;
		case 5: /* cherry */
#ifdef JP
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                "�`�F���[", row + 8, col);
#else
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                " Cherry ", row + 8, col);
#endif

			break;
	}
}

/*
 * kpoker no (tyuto-hannpa na)pakuri desu...
 * joker ha shineru node haitte masen.
 *
 * TODO: donataka! tsukutte!
 *  - agatta yaku no kiroku (like DQ).
 *  - kakkoii card no e.
 *  - sousa-sei no koujyo.
 *  - code wo wakariyasuku.
 *  - double up.
 *  - Joker... -- done.
 *
 * 9/13/2000 --Koka
 * 9/15/2000 joker wo jissou. soreto, code wo sukosi kakikae. --Habu
 */
#define SUIT_OF(card)  ((card) / 13)
#define NUM_OF(card)   ((card) % 13)
#define IS_JOKER(card) ((card) == 52)

static int cards[5]; /* tefuda no card */

static void reset_deck(int deck[])
{
	int i;
	for (i = 0; i < 53; i++) deck[i] = i;

	/* shuffle cards */
	for (i = 0; i < 53; i++){
		int tmp1 = randint0(53 - i) + i;
		int tmp2 = deck[i];
		deck[i] = deck[tmp1];
		deck[tmp1] = tmp2;
	}
}

static bool have_joker(void)
{
	int i;

	for (i = 0; i < 5; i++){
	  if(IS_JOKER(cards[i])) return TRUE;
	}
	return FALSE;
}

static bool find_card_num(int num)
{
	int i;
	for (i = 0; i < 5; i++)
		if (NUM_OF(cards[i]) == num && !IS_JOKER(cards[i])) return TRUE;
	return FALSE;
}

static bool yaku_check_flush(void)
{
	int i, suit;
	bool joker_is_used = FALSE;

	suit = IS_JOKER(cards[0]) ? SUIT_OF(cards[1]) : SUIT_OF(cards[0]);
	for (i = 0; i < 5; i++){
		if (SUIT_OF(cards[i]) != suit){
		  if(have_joker() && !joker_is_used)
		    joker_is_used = TRUE;
		  else
		    return FALSE;
		}
	}

	return TRUE;
}

static int yaku_check_straight(void)
{
	int i, lowest = 99;
	bool joker_is_used = FALSE;

	/* get lowest */
	for (i = 0; i < 5; i++)
	{
		if (NUM_OF(cards[i]) < lowest && !IS_JOKER(cards[i]))
			lowest = NUM_OF(cards[i]);
	}
	
	if (yaku_check_flush())
	{
	  if( lowest == 0 ){
		for (i = 0; i < 4; i++)
		{
			if (!find_card_num(9 + i)){
				if( have_joker() && !joker_is_used )
				  joker_is_used = TRUE;
				else
				  break;
			}
		}
		if (i == 4) return 3; /* Wow! Royal Flush!!! */
	  }
	  if( lowest == 9 ){
		for (i = 0; i < 3; i++)
		{
			if (!find_card_num(10 + i))
				break;
		}
		if (i == 3 && have_joker()) return 3; /* Wow! Royal Flush!!! */
	  }
	}

	joker_is_used = FALSE;
	for (i = 0; i < 5; i++)
	{
		if (!find_card_num(lowest + i)){
		  if( have_joker() && !joker_is_used )
		    joker_is_used = TRUE;
		  else
		    return 0;
		}
	}
	
	if (yaku_check_flush())
		return 2; /* Straight Flush */

	return 1;
}

/*
 * 0:nopair 1:1 pair 2:2 pair 3:3 cards 4:full house 6:4cards
 */
static int yaku_check_pair(void)
{
	int i, i2, matching = 0;

	for (i = 0; i < 5; i++)
	{
		for (i2 = i+1; i2 < 5; i2++)
		{
			if (IS_JOKER(cards[i]) || IS_JOKER(cards[i2])) continue;
			if (NUM_OF(cards[i]) == NUM_OF(cards[i2]))
				matching++;
		}
	}

	if(have_joker()){
	  switch(matching){
	  case 0:
	    matching = 1;
	    break;
	  case 1:
	    matching = 3;
	    break;
	  case 2:
	    matching = 4;
	    break;
	  case 3:
	    matching = 6;
	    break;
	  case 6:
	    matching = 7;
	    break;
	  default:
	    /* don't reach */
	    break;
	  }
	}

	return matching;
}

#define ODDS_5A 3000
#define ODDS_5C 400
#define ODDS_RF 200
#define ODDS_SF 80
#define ODDS_4C 16
#define ODDS_FH 12
#define ODDS_FL 8
#define ODDS_ST 4
#define ODDS_3C 1
#define ODDS_2P 1

static int yaku_check(void)
{
	prt("                            ", 4, 3);

	switch(yaku_check_straight()){
	case 3: /* RF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "���C�����X�g���[�g�t���b�V��",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Royal Flush",  4,  3);
#endif
		return ODDS_RF;
	case 2: /* SF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "�X�g���[�g�t���b�V��",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight Flush",  4,  3);
#endif
		return ODDS_SF;
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "�X�g���[�g",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight",  4,  3);
#endif
		return ODDS_ST;
	default:
		/* Not straight -- fall through */
		break;
	}

	if (yaku_check_flush())
	{

#ifdef JP
	c_put_str(TERM_YELLOW, "�t���b�V��",  4,  3);
#else
	c_put_str(TERM_YELLOW, "Flush",  4,  3);
#endif
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "�����y�A",  4,  3);
#else
		c_put_str(TERM_YELLOW, "One pair",  4,  3);
#endif
		return 0;
	case 2:
#ifdef JP
		c_put_str(TERM_YELLOW, "�c�[�y�A",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Two pair",  4,  3);
#endif
		return ODDS_2P;
	case 3:
#ifdef JP
		c_put_str(TERM_YELLOW, "�X���[�J�[�h",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Three of a kind",  4,  3);
#endif
		return ODDS_3C;
	case 4:
#ifdef JP
		c_put_str(TERM_YELLOW, "�t���n�E�X",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Full house",  4,  3);
#endif
		return ODDS_FH;
	case 6:
#ifdef JP
		c_put_str(TERM_YELLOW, "�t�H�[�J�[�h",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Four of a kind",  4,  3);
#endif
		return ODDS_4C;
	case 7:
		if (!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�t�@�C�u�G�[�X",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five ace",  4,  3);
#endif
			return ODDS_5A;
		}
		else
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�t�@�C�u�J�[�h",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five of a kind",  4,  3);
#endif
			return ODDS_5C;
		}
	default:
		break;
	}
	return 0;
}

static void display_kaeruka(int hoge, int kaeruka[])
{
	int i;
	char col = TERM_WHITE;
	for (i = 0; i < 5; i++)
	{
		if (i == hoge) col = TERM_YELLOW;
		else if(kaeruka[i]) col = TERM_WHITE;
		else col = TERM_L_BLUE;
#ifdef JP
		if(kaeruka[i])
			c_put_str(col, "������", 14,  5+i*16);
		else
			c_put_str(col, "�̂���", 14,  5+i*16);
#else
		if(kaeruka[i])
			c_put_str(col, "Change", 14,  5+i*16);
		else
			c_put_str(col, " Stay ", 14,  5+i*16);
#endif
	}
	if (hoge > 4) col = TERM_YELLOW;
	else col = TERM_WHITE;
#ifdef JP
	c_put_str(col, "����", 16,  38);
#else
	c_put_str(col, "Sure", 16,  38);
#endif

	/* Hilite current option */
	if (hoge < 5) move_cursor(14, 5+hoge*16);
	else move_cursor(16, 38);
}


static void display_cards(void)
{
	int i, j;
	char suitcolor[4] = {TERM_YELLOW, TERM_L_RED, TERM_L_BLUE, TERM_L_GREEN};
#ifdef JP
	cptr suit[4] = {"��", "��", "��", "��"};
	cptr card_grph[13][7] = {{"�`   %s     ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "     �{     ",
				  "     %s     ",
				  "          �`"},
				 {"�Q          ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          �Q"},
				 {"�R          ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "          �R"},
				 {"�S          ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "          �S"},
				 {"�T          ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "          �T"},
				 {"�U          ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          �U"},
				 {"�V          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          �V"},
				 {"�W          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "          �W"},
				 {"�X %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s �X"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"�i   ��     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=��=| %s",
				  "     ��   �i"},
				 {"�p ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##   �p"},
				 {"�j          ",
				  "%s �M�܁L   ",
				  "  ��������  ",
				  "  �� �� ��  ",
				  "   ��    �� ",
				  "    �� �m %s",
				  "          �j"}};
	cptr joker_grph[7] = {    "            ",
				  "     �i     ",
				  "     �n     ",
				  "     �j     ",
				  "     �d     ",
				  "     �q     ",
				  "            "};

#else

	cptr suit[4] = {"[]", "qp", "<>", "db"};
	cptr card_grph[13][7] = {{"A    %s     ",
				  "     He     ",
				  "     ng     ",
				  "     ba     ",
				  "     nd     ",
				  "     %s     ",
				  "           A"},
				 {"2           ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "           2"},
				 {"3           ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "           3"},
				 {"4           ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "           4"},
				 {"5           ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "           5"},
				 {"6           ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           6"},
				 {"7           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           7"},
				 {"8           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "           8"},
				 {"9  %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s  9"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"J    /\\     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=HH=| %s",
				  "     ][    J"},
				 {"Q  ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##    Q"},
				 {"K           ",
				  "%s _'~~`_   ",
				  "   jjjjj$&  ",
				  "   q q uu   ",
				  "   c    &   ",
				  "    v__/  %s",
				  "           K"}};
	cptr joker_grph[7] = {    "            ",
				  "     J      ",
				  "     O      ",
				  "     K      ",
				  "     E      ",
				  "     R      ",
				  "            "};
#endif

	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("����������������",  5,  i*16);
#else
		prt(" +------------+ ",  5,  i*16);
#endif
	}

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
#ifdef JP
			prt("��",  j+6,  i*16);
#else
			prt(" |",  j+6,  i*16);
#endif
			if(IS_JOKER(cards[i]))
				c_put_str(TERM_VIOLET, joker_grph[j],  j+6,  2+i*16);
			else
				c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]),  j+6,  2+i*16);
#ifdef JP
			prt("��",  j+6,  i*16+14);
#else
			prt("| ",  j+6,  i*16+14);
#endif
		}
	}
	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("����������������", 13,  i*16);
#else
		prt(" +------------+ ", 13,  i*16);
#endif
	}
}

static int do_poker(void)
{
	int i, k = 2;
	char cmd;
	int deck[53]; /* yamafuda : 0...52 */
	int deck_ptr = 0;
	int kaeruka[5]; /* 0:kaenai 1:kaeru */

	bool done = FALSE;
	bool kettei = TRUE;
	bool kakikae = TRUE;

	reset_deck(deck);

	for (i = 0; i < 5; i++)
	{
		cards[i] = deck[deck_ptr++];
		kaeruka[i] = 0; /* default:nokosu */
	}
	
#if 0
	/* debug:RF */
	cards[0] = 12;
	cards[1] = 0;
	cards[2] = 9;
	cards[3] = 11;
	cards[4] = 10;
#endif
#if 0
	/* debug:SF */
	cards[0] = 3;
	cards[1] = 2;
	cards[2] = 4;
	cards[3] = 6;
	cards[4] = 5;
#endif
#if 0
	/* debug:Four Cards */
	cards[0] = 0;
	cards[1] = 0 + 13 * 1;
	cards[2] = 0 + 13 * 2;
	cards[3] = 0 + 13 * 3;
	cards[4] = 51;
#endif
#if 0
	/* debug:Straight */
	cards[0] = 1;
	cards[1] = 0 + 13;
	cards[2] = 3;
	cards[3] = 2 + 26;
	cards[4] = 4;
#endif
#if 0
	/* debug */
	cards[0] = 52;
	cards[1] = 0;
	cards[2] = 1;
	cards[3] = 2;
	cards[4] = 3;
#endif

	/* suteruno wo kimeru */
#ifdef JP
	prt("�c���J�[�h�����߂ĉ�����(�����ňړ�, �X�y�[�X�őI��)�B", 0, 0);
#else
	prt("Stay witch? ", 0, 0);
#endif

	display_cards();
	yaku_check();

	while (!done)
	{
		if (kakikae) display_kaeruka(k+kettei*5, kaeruka);
		kakikae = FALSE;
		cmd = inkey();
		switch (cmd)
		{
		case '6': case 'l': case 'L': case KTRL('F'):
			if (!kettei) k = (k+1)%5;
			else {k = 0;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '4': case 'h': case 'H': case KTRL('B'):
			if (!kettei) k = (k+4)%5;
			else {k = 4;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '2': case 'j': case 'J': case KTRL('N'):
			if (!kettei) {kettei = TRUE;kakikae = TRUE;}
			break;
		case '8': case 'k': case 'K': case KTRL('P'):
			if (kettei) {kettei = FALSE;kakikae = TRUE;}
			break;
		case ' ': case '\r':
			if (kettei) done = TRUE;
			else {kaeruka[k] = !kaeruka[k];kakikae = TRUE;}
			break;
		default:
			break;
		}
	}
	
	prt("",0,0);

	for (i = 0; i < 5; i++)
		if (kaeruka[i] == 1) cards[i] = deck[deck_ptr++]; /* soshite toru */

	display_cards();
	
	return yaku_check();
}
#undef SUIT_OF
#undef NUM_OF
#undef IS_JOKER
/* end of poker codes --Koka */

/*
 * gamble_comm
 */
#if 0
static bool gamble_comm(int cmd)
{
	int i;
	int roll1, roll2, roll3, choice, odds, win;
	s32b wager;
	s32b maxbet;
	s32b oldgold;

	char out_val[160], tmp_str[80], again;
	cptr p;

	screen_save();

	if (cmd == BACT_GAMBLE_RULES)
	{
		/* Peruse the gambling help file */
#ifdef JP
(void)show_file(TRUE, "jgambling.txt", NULL, 0, 0);
#else
		(void)show_file(TRUE, "gambling.txt", NULL, 0, 0);
#endif

	}
	else
	{
		/* No money */
		if (p_ptr->au < 1)
		{
#ifdef JP
			msg_print("�����I���܂��ꕶ�Ȃ�����Ȃ����I��������o�Ă����I");
#else
			msg_print("Hey! You don't have gold - get out of here!");
#endif

			msg_print(NULL);
			screen_load();
			return FALSE;
		}

		clear_bldg(5, 23);

		maxbet = p_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, p_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"�q���� (1-%ld)�H", (long int)maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", (long int)maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > p_ptr->au)
			{
#ifdef JP
msg_print("�����I��������Ȃ�����Ȃ����I�o�Ă����I");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ld�S�[���h�����󂯂悤�B�c��͎���Ƃ��ȁB", (long int)maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", (long int)maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�n�j�A�P�S�[���h����͂��߂悤�B");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			win = FALSE;
			odds = 0;
			oldgold = p_ptr->au;

#ifdef JP
sprintf(tmp_str, "�Q�[���O�̏�����: %9ld", (long int)oldgold);
#else
			sprintf(tmp_str, "Gold before game: %9ld", (long int)oldgold);
#endif

			prt(tmp_str, 20, 2);

#ifdef JP
sprintf(tmp_str, "���݂̊|����:     %9ld", (long int)wager);
#else
			sprintf(tmp_str, "Current Wager:    %9ld", (long int)wager);
#endif

			prt(tmp_str, 21, 2);

			do
			{
				p_ptr->au -= wager;
				switch (cmd)
				{
				 case BACT_IN_BETWEEN: /* Game of In-Between */
#ifdef JP
c_put_str(TERM_GREEN, "�C���E�r�g�C�[��",5,2);
#else
					c_put_str(TERM_GREEN, "In Between", 5, 2);
#endif

					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
#ifdef JP
sprintf(tmp_str, "���_�C�X: %d        ���_�C�X: %d", roll1, roll2);
#else
					sprintf(tmp_str, "Black die: %d       Black Die: %d", roll1, roll2);
#endif

					prt(tmp_str, 8, 3);
#ifdef JP
sprintf(tmp_str, "�ԃ_�C�X: %d", choice);
#else
					sprintf(tmp_str, "Red die: %d", choice);
#endif

					prt(tmp_str, 11, 14);
					if (((choice > roll1) && (choice < roll2)) ||
						((choice < roll1) && (choice > roll2)))
						win = TRUE;
					break;
				case BACT_CRAPS:  /* Game of Craps */
#ifdef JP
c_put_str(TERM_GREEN, "�N���b�v�X", 5, 2);
#else
					c_put_str(TERM_GREEN, "Craps", 5, 2);
#endif

					win = 3;
					odds = 2;
					roll1 = randint1(6);
					roll2 = randint1(6);
					roll3 = roll1 +  roll2;
					choice = roll3;
#ifdef JP
sprintf(tmp_str, "�P�U���: %d %d      Total: %d", roll1, 
#else
					sprintf(tmp_str, "First roll: %d %d    Total: %d", roll1,
#endif

						 roll2, roll3);
					prt(tmp_str, 7, 5);
					if ((roll3 == 7) || (roll3 == 11))
						win = TRUE;
					else if ((roll3 == 2) || (roll3 == 3) || (roll3 == 12))
						win = FALSE;
					else
						do
						{
#ifdef JP
msg_print("�Ȃɂ��L�[�������Ƃ������U��܂��B");
#else
							msg_print("Hit any key to roll again");
#endif

							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;

#ifdef JP
sprintf(tmp_str, "�o��: %d %d          ���v:      %d",
#else
							sprintf(tmp_str, "Roll result: %d %d   Total:     %d",
#endif

								 roll1, roll2, roll3);
							prt(tmp_str, 8, 5);
							if (roll3 == choice)
								win = TRUE;
							else if (roll3 == 7)
								win = FALSE;
						} while ((win != TRUE) && (win != FALSE));
					break;

				case BACT_SPIN_WHEEL:  /* Spin the Wheel Game */
					win = FALSE;
					odds = 9;
#ifdef JP
c_put_str(TERM_GREEN, "���[���b�g", 5, 2);
#else
					c_put_str(TERM_GREEN, "Wheel", 5, 2);
#endif

					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
#ifdef JP
get_string("���ԁH (0-9): ", out_val, 32);
#else
					get_string("Pick a number (0-9): ", out_val, 32);
#endif
/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN	
					for (p = out_val; iswspace(*p); p++);
#else
					for (p = out_val; isspace(*p); p++);
#endif
					choice = atol(p);
					if (choice < 0)
					{
#ifdef JP
msg_print("0�Ԃɂ��Ƃ����B");
#else
						msg_print("I'll put you down for 0.");
#endif

						choice = 0;
					}
					else if (choice > 9)
					{
#ifdef JP
msg_print("�n�j�A9�Ԃɂ��Ƃ����B");
#else
						msg_print("Ok, I'll put you down for 9.");
#endif

						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
#ifdef JP
sprintf(tmp_str, "���[���b�g�͉��A�~�܂����B���҂� %d�Ԃ��B",
#else
					sprintf(tmp_str, "The wheel spins to a stop and the winner is %d",
#endif

						roll1);
					prt(tmp_str, 13, 3);
					prt("", 9, 0);
					prt("*", 9, (3 * roll1 + 5));
					if (roll1 == choice)
						win = TRUE;
					break;

				case BACT_DICE_SLOTS: /* The Dice Slots */
#ifdef JP
c_put_str(TERM_GREEN, "�_�C�X�E�X���b�g", 5, 2);
					c_put_str(TERM_YELLOW, "������   ������            2", 6, 37);
					c_put_str(TERM_YELLOW, "������   ������   ������   5", 7, 37);
					c_put_str(TERM_ORANGE, "�I�����W �I�����W �I�����W 10", 8, 37);
					c_put_str(TERM_UMBER, "��       ��       ��       20", 9, 37);
					c_put_str(TERM_SLATE, "��       ��       ��       50", 10, 37);
					c_put_str(TERM_VIOLET, "�v����   �v����   �v����   200", 11, 37);
					c_put_str(TERM_RED, "�`�F���[ �`�F���[ �`�F���[ 1000", 12, 37);
#else
					c_put_str(TERM_GREEN, "Dice Slots", 5, 2);
#endif

					win = FALSE;
					roll1 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll1-i) < 1)
						{
							roll1 = 7-i;
							break;
						}
						roll1 -= i;
					}
					roll2 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll2-i) < 1)
						{
							roll2 = 7-i;
							break;
						}
						roll2 -= i;
					}
					choice = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((choice-i) < 1)
						{
							choice = 7-i;
							break;
						}
						choice -= i;
					}
					put_str("/--------------------------\\", 7, 2);
					prt("\\--------------------------/", 17, 2);
					display_fruit(8,  3, roll1 - 1);
					display_fruit(8, 12, roll2 - 1);
					display_fruit(8, 21, choice - 1);
					if ((roll1 == roll2) && (roll2 == choice))
					{
						win = TRUE;
						switch(roll1)
						{
						case 1:
							odds = 5;break;
						case 2:
							odds = 10;break;
						case 3:
							odds = 20;break;
						case 4:
							odds = 50;break;
						case 5:
							odds = 200;break;
						case 6:
							odds = 1000;break;
						}
					}
					else if ((roll1 == 1) && (roll2 == 1))
					{
						win = TRUE;
						odds = 2;
					}
					break;
				case BACT_POKER:
					win = FALSE;
					odds = do_poker();
					if (odds) win = TRUE;
					break;
				}

				if (win)
				{
#ifdef JP
prt("���Ȃ��̏���", 16, 37);
#else
					prt("YOU WON", 16, 37);
#endif

					p_ptr->au += odds * wager;
#ifdef JP
sprintf(tmp_str, "�{��: %d", odds);
#else
					sprintf(tmp_str, "Payoff: %d", odds);
#endif

					prt(tmp_str, 17, 37);
				}
				else
				{
#ifdef JP
prt("���Ȃ��̕���", 16, 37);
#else
					prt("You Lost", 16, 37);
#endif

					prt("", 17, 37);
				}
#ifdef JP
sprintf(tmp_str, "���݂̏�����:     %9ld", (long int)p_ptr->au);
#else
				sprintf(tmp_str, "Current Gold:     %9ld", (long int)p_ptr->au);
#endif

				prt(tmp_str, 22, 2);
#ifdef JP
prt("������x(Y/N)�H", 18, 37);
#else
				prt("Again(Y/N)?", 18, 37);
#endif

				move_cursor(18, 52);
				again = inkey();
				prt("", 16, 37);
				prt("", 17, 37);
				prt("", 18, 37);
				if (wager > p_ptr->au)
				{
#ifdef JP
msg_print("�����I��������Ȃ�����Ȃ����I��������o�čs���I");
#else
					msg_print("Hey! You don't have the gold - get out of here!");
#endif

					msg_print(NULL);

					/* Get out here */
					break;
				}
			} while ((again == 'y') || (again == 'Y'));

			prt("", 18, 37);
			if (p_ptr->au >= oldgold)
			{
#ifdef JP
msg_print("�u����ׂ͖����ȁI�ł����͂������������Ă�邩��ȁA��΂ɁI�v");
#else
				msg_print("You came out a winner! We'll win next time, I'm sure.");
#endif
				chg_virtue(V_CHANCE, 3);
			}
			else
			{
#ifdef JP
msg_print("�u�����X�b�Ă��܂����ȁA��͂́I�����ɋA���������������B�v");
#else
				msg_print("You lost gold! Haha, better head home.");
#endif
				chg_virtue(V_CHANCE, -3);
			}
		}
		msg_print(NULL);
	}
	screen_load();
	return (TRUE);
}
#endif

///del ���Z��֘A
#if 0
static bool vault_aux_battle(int r_idx)
{
	int i;
	int dam = 0;

	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
/*	if (!mon_hook_dungeon(r_idx)) return FALSE; */

	/* Decline unique monsters */
/*	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE); */
/*	if (r_ptr->flags7 & (RF7_NAZGUL)) return (FALSE); */

	if (r_ptr->flags1 & (RF1_NEVER_MOVE)) return (FALSE);
	if (r_ptr->flags2 & (RF2_MULTIPLY)) return (FALSE);
	if (r_ptr->flags2 & (RF2_QUANTUM)) return (FALSE);
	if (r_ptr->flags7 & (RF7_AQUATIC)) return (FALSE);
	if (r_ptr->flags7 & (RF7_CHAMELEON)) return (FALSE);

	for (i = 0; i < 4; i++)
	{
		if (r_ptr->blow[i].method == RBM_EXPLODE) return (FALSE);
		if (r_ptr->blow[i].effect != RBE_DR_MANA) dam += r_ptr->blow[i].d_dice;
	}
	if (!dam && !(r_ptr->flags4 & (RF4_BOLT_MASK | RF4_BEAM_MASK | RF4_BALL_MASK | RF4_BREATH_MASK)) && !(r_ptr->flags5 & (RF5_BOLT_MASK | RF5_BEAM_MASK | RF5_BALL_MASK | RF5_BREATH_MASK)) && !(r_ptr->flags6 & (RF6_BOLT_MASK | RF6_BEAM_MASK | RF6_BALL_MASK | RF6_BREATH_MASK))) return (FALSE);

	/* Okay */
	return (TRUE);
}
#endif

///mod160305 ���l�`���l�̔��� 
static void buy_hinaningyou()
{
	char c;
	int price = 100;
	bool flag = FALSE;

	if(p_ptr->pclass == CLASS_HINA)
	{
		msg_print("���l�`�̔���s���͂ǂ����낤���H");
		return;
	}

	clear_bldg(4,18);

	prt(format("���l�`�̖��l�̔������B�e���Ȏ��l�`($%d)������c���Ă���B",price), 5,10);

	if (!get_check_strict("�����܂����H", CHECK_DEFAULT_Y))
	{
		clear_bldg(4,18);
		return;
	}
	clear_bldg(4,18);
	if(p_ptr->au < price)
	{
		msg_print("����������Ȃ��B");
		return;
	}

	msg_print("���l�`����Ɏ�����E�E");
	if(restore_level()) flag = TRUE;
	else if(remove_curse())
	{
		msg_print("�̂��y���Ȃ����C������B");
		flag = TRUE;
	}

	if(!flag) msg_print("���ɉ����N����Ȃ������B");
	msg_print("�l�`�͕���ď������B");

	p_ptr->au -= price;
	building_prt_gold();

}


//v2.0.3 �A�r���e�B�J�[�h���グ�L�^�ϐ��ɉ��Z
//���i�������܂�`�[�g�I�v�V������e��������Y��Ă����B���X�Ȃ���ǉ����邽�߂ɕʊ֐��ɕ����B
static void add_cardshop_profit(int price)
{

	if (p_ptr->noscore || p_ptr->pseikaku == SEIKAKU_MUNCHKIN) return;

	if (total_pay_cardshop < 100000000) total_pay_cardshop += price;


}


//v1.1.87 �R�@�̓q���10�A�K�`��
static void buy_gacha_box()
{
	int price;
	object_type forge;
	object_type *o_ptr = &forge;
	char o_name[MAX_NLEN];

	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		msg_print("�u��������ȁB���ɂ͉����Ȃ����B�v");

		return;
	}


	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		msg_print("�u�����ƁA���̉��̓J�[�h�̔��l��p���B�v");
		return;

	}

	//�U�b�N�ɋ󂫂��Ȃ��Ƃ�
	if (inventory[INVEN_PACK - 1].k_idx)
	{
		if(p_ptr->pclass == CLASS_SANNYO)
			msg_print("�U�b�N�ɋ󂫂��Ȃ��B");
		else
			msg_print("�u�ӂ�A�U�b�N�ɋ󂫂�����Ă���܂����ȁB�v");
		return;
	}

	//�K�`���̉��i�͍����x*1000 ���10��
	//�o�Ă���J�[�h�̊��Ғl�͍����x*1100�`1200���炢�Ȃ̂ő����͊��ɍ���
	price = p_ptr->abilitycard_price_rate * 1000;
	if (price > 100000) price = 100000;

	clear_bldg(4, 18);

	if (p_ptr->pclass == CLASS_SANNYO)
	{
		if (!buy_gacha_box_count)
		{
			price = 10;
			prt("�����ˁu���񂽂��A��܂ł͎��B�������ŏ����������Ă��炨���B", 5, 10);
			prt(format("�@�@�@���āA���Ȃ珉���T�[�r�X�ňꔠ$%d����I", price), 6, 10);
			prt("�@�@�@ ", 7, 10);
			prt("�@�@�@���H�������������Ɍ��܂��Ă邶��Ȃ����I�v", 8, 10);
		}
		else
		{
			prt("�����ˁu��������Ⴂ�I", 5, 10);
			prt(format("�@�@�@�������˂��B���̑���Ȃ�ꔠ$%d���ď����ȁB�v", price), 6, 10);
		}
	}
	else
	{
		prt("�R�@�u�悭�����ȁB���̓J�[�h���g�����ʔ����q�����n�߂��Ƃ��낾�B", 5, 10);
		prt("�@�@�@���̓��F�̔��ɂ̓A�r���e�B�J�[�h��10�������Ă���B", 6, 10);
		prt(format("�@�@�@�������ꔠ��%d�Ŕ����Ă�낤�B", price), 7, 10);
		prt("�@�@�@�^�ǂ����A�ȃJ�[�h��������Α�ׂ����Ă킯���B", 8, 10);
		prt("�@�@�@���_�A���̒��g�ɂ������������������Ȃǂ����Ⴂ�Ȃ��B�v", 9, 10);
		if (!buy_gacha_box_count)
		{
			price = 10;
			prt("�@�@�@ ", 10, 10);
			prt(format("�@�@�u�����Ă��O�͍K�^���B���񂾂��͑�T�[�r�X�́�%d�ɂ��Ă��B�v", price), 11, 10);
		}
	}



	if (!get_check_strict("�����܂����H", CHECK_DEFAULT_Y))
	{
		clear_bldg(4, 18);
		return;
	}
	clear_bldg(4, 18);
	if (p_ptr->au < price)
	{
		msg_print("����������Ȃ��B");
		return;
	}


	if (p_ptr->pclass == CLASS_SANNYO)
	{
		prt("���Ȃ��͔����w�������B", 5, 10);
	}
	else if (!buy_gacha_box_count)
	{
		prt("�R�@�u����Ƃ����ۛ��ɂȁB�ӂ��ӂ��Ӂc�c�v", 5, 10);
	}
	else
	{
		if (buy_gacha_box_count > 10 && one_in_(4))prt("�R�@�u�ڂ��������Ă���ȁB�ꕞ�ǂ����H�v", 5, 10);
		else if(one_in_(10))	prt("�R�@�u���̏����͎R�̏�̂ق��̘A�����ɂ܂�邩�������B���߂ɔ������ق����������H�v", 5, 10);
		else if(one_in_(7))		prt("�R�@�u�_���W�����̉��ŊJ�����烌�A�J�[�h���o�₷���H����Ȗ�Ȃ����낤���B�v", 5, 10);
		else if(one_in_(3))		prt("�R�@�u�����J�[�h�������Ă���Ƃ����ȁH�v", 5, 10);
		else 					prt("�R�@�u�悵�A���x����B�v", 5, 10);

	}
	buy_gacha_box_count++;
	//�J�[�h�̍��v�x���z�ɉ��Z
	//if(total_pay_cardshop < 100000000) total_pay_cardshop += price;
	add_cardshop_profit(price);

	object_prep(o_ptr, lookup_kind(TV_CHEST, SV_CHEST_10_GACHA));
	o_ptr->pval = -6; //(��{���̔� )	

	object_desc(o_name, o_ptr, 0);

	inven_carry(o_ptr);
	msg_format("%s���󂯎�����B", o_name);

	p_ptr->au -= price;
	building_prt_gold();

}


/*:::���J���@�X�|�C���g�����Ώەi�ڃe�[�u��*/
struct marisa_store_type marisa_wants_table[] =
{
	{TV_BOOK_ELEMENT,2,30},
	{TV_BOOK_ELEMENT,3,300},
	{TV_BOOK_FORESEE,2,30},
	{TV_BOOK_FORESEE,3,300},
	{TV_BOOK_ENCHANT,2,30},
	{TV_BOOK_ENCHANT,3,300},
	{TV_BOOK_SUMMONS,2,30},
	{TV_BOOK_SUMMONS,3,300},
	{TV_BOOK_MYSTIC,2,30},
	{TV_BOOK_MYSTIC,3,300},
	{TV_BOOK_LIFE,2,30},
	{TV_BOOK_LIFE,3,300},
	{TV_BOOK_PUNISH,2,30},
	{TV_BOOK_PUNISH,3,300},
	{TV_BOOK_NATURE,2,30},
	{TV_BOOK_NATURE,3,300},
	{TV_BOOK_TRANSFORM,2,30},
	{TV_BOOK_TRANSFORM,3,300},
	{TV_BOOK_DARKNESS,2,30},
	{TV_BOOK_DARKNESS,3,300},
	{TV_BOOK_NECROMANCY,2,30},
	{TV_BOOK_NECROMANCY,3,300},
	{TV_BOOK_CHAOS,2,30},
	{TV_BOOK_CHAOS,3,300},
	{TV_BOOK_OCCULT,2,10},
	{TV_BOOK_OCCULT,3,300},

	{TV_MATERIAL,SV_MATERIAL_HIKARIGOKE ,35 },
	{TV_MATERIAL,SV_MATERIAL_STONE  ,1 },
	{TV_MATERIAL,SV_MATERIAL_HEMATITE ,50 },
	{TV_MATERIAL,SV_MATERIAL_MAGNETITE ,60 },
	{TV_MATERIAL,SV_MATERIAL_ARSENIC ,45 },
	{TV_MATERIAL,SV_MATERIAL_MERCURY ,90 },
	{TV_MATERIAL,SV_MATERIAL_MITHRIL ,150 },
	{TV_MATERIAL,SV_MATERIAL_ADAMANTITE ,200 },
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL ,1000 },
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE ,300 },
	{TV_MATERIAL,SV_MATERIAL_GELSEMIUM ,50 },
	{TV_MATERIAL,SV_MATERIAL_GARNET,30 },
	{TV_MATERIAL,SV_MATERIAL_AMETHYST,40 },
	{TV_MATERIAL,SV_MATERIAL_AQUAMARINE ,35 },
	{TV_MATERIAL,SV_MATERIAL_DIAMOND ,1500 },
	{TV_MATERIAL,SV_MATERIAL_EMERALD ,600 },
	{TV_MATERIAL,SV_MATERIAL_MOONSTONE ,35 },
	{TV_MATERIAL,SV_MATERIAL_RUBY ,650 },
	{TV_MATERIAL,SV_MATERIAL_PERIDOT ,35 },
	{TV_MATERIAL,SV_MATERIAL_SAPPHIRE ,550 },
	{TV_MATERIAL,SV_MATERIAL_OPAL ,40 },
	{TV_MATERIAL,SV_MATERIAL_TOPAZ ,50 },
	{TV_MATERIAL,SV_MATERIAL_LAPISLAZULI ,35 },
	{TV_MATERIAL,SV_MATERIAL_METEORICIRON ,1200 },
	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE ,3000 },
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON ,5 },
	{TV_MATERIAL,SV_MATERIAL_ICESCALE ,300 },
	{TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT ,300 },
	{TV_MATERIAL,SV_MATERIAL_HOPE_FRAGMENT ,1200 },
	{TV_MATERIAL,SV_MATERIAL_MYSTERIUM ,2000 },
	{TV_MATERIAL,SV_MATERIAL_TAKAKUSAGORI ,400 },
	{TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT ,1250 },
	{TV_MATERIAL,SV_MATERIAL_ISHIZAKURA ,75 },
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU ,500 },

	{TV_MATERIAL,SV_MATERIAL_BROKEN_NEEDLE ,150 },
	{TV_MATERIAL,SV_MATERIAL_SKULL ,30 },	//v1.1.65

	{TV_MUSHROOM,SV_MUSHROOM_POISON ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_BLINDNESS  ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_PARANOIA ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_CONFUSION ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_HALLUCINATION  ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_PARALYSIS ,3 },
	{TV_MUSHROOM,SV_MUSHROOM_WEAKNESS ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_SICKNESS ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_STUPIDITY ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_NAIVETY  ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_UNHEALTH ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_DISEASE  ,6 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_POISON ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_BLINDNESS ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_PARANOIA ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_CONFUSION ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_CURE_SERIOUS ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_RESTORE_STR ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_RESTORE_CON ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_RESTORING ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MANA ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_BERSERK ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_RED ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_WHITE ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_BLUE ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_BLACK ,10 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_L_BLUE ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_GREEN ,20 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_GAUDY ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_GRAY  ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_CLEAR ,30 },
	{TV_MUSHROOM,SV_MUSHROOM_MON_SUPER ,2000 },

	{ TV_MUSHROOM,SV_MUSHROOM_PUFFBALL ,30 },

	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER ,3000 },
	{ TV_SOUVENIR,SV_SOUVENIR_SHILVER_KEY ,5000 },

	{ TV_FOOD,SV_FOOD_DATURA,10 },
	{ TV_SOUVENIR,SV_SOUVENIR_MIRROR_OF_RUFFNOR ,2000 },

	{ TV_SOUVENIR,SV_SOUVENIR_ELDER_THINGS_CRYSTAL ,800 },

	{ TV_SOUVENIR,SV_SOUVENIR_KODOKUZARA ,1200 },

	{ TV_SOUVENIR,SV_SOUVENIR_MOON_ORB ,300 },

	{ TV_SWEETS,SV_SWEETS_HONEY ,30 },

	{ TV_ALCOHOL,SV_ALCOHOL_NERIZAKE ,300 },


	{0,0,0}//�I�[�p�_�~�[
};

/*:::���J���@�X�|�C���g�����i�i�e�[�u��*/
struct marisa_store_type marisa_present_table[] =
{
	{TV_CHEST,SV_CHEST_MARIPO_01 , 20},//�ω΂̖�
	{TV_CHEST,SV_CHEST_MARIPO_02 , 20},//�ϗ�̖�
	{TV_CHEST,SV_CHEST_MARIPO_03 , 30},//�X�s�[�h�̖�
	{TV_CHEST,SV_CHEST_MARIPO_04 , 40},//��Ηn�𖂖@�_
	{TV_CHEST,SV_CHEST_MARIPO_05 , 50},//�e���|�A�E�F�C���@�_
	{TV_CHEST,SV_CHEST_MARIPO_06 , 70},//�ϐ��̖�
	{TV_CHEST,SV_CHEST_MARIPO_07 , 100},//�̗͉񕜂̖�
	{TV_CHEST,SV_CHEST_MARIPO_08 , 150},//�[�ւ̖�

	{TV_CHEST,SV_CHEST_MARIPO_09 , 200},//�X�s�[�h�̏�
	{TV_CHEST,SV_CHEST_MARIPO_10 , 300},//�X�̎w��
	{TV_CHEST,SV_CHEST_MARIPO_11 , 500},//�h���S���E�u���X�̖��@�_
	{TV_CHEST,SV_CHEST_MARIPO_12 , 700},//�ϓł̎w��
	{TV_CHEST,SV_CHEST_MARIPO_13 , 800},//�Ӓ�̃��b�h
	{TV_CHEST,SV_CHEST_MARIPO_14 , 1000},//�j��̏�
	{TV_CHEST,SV_CHEST_MARIPO_15 , 1200},//�����̖I����
	{TV_CHEST,SV_CHEST_MARIPO_16 , 1500},//���G�̖�

	{TV_CHEST,SV_CHEST_MARIPO_17 , 2000},//���̗͂��̏�
	{TV_CHEST,SV_CHEST_MARIPO_18 , 2500},//�T�C�o�[�f�[�����̐l�`
	{TV_CHEST,SV_CHEST_MARIPO_19 , 4000},//�d���i75�K���������h���b�v)
	{TV_CHEST,SV_CHEST_MARIPO_20 , 5000},//�k���m�[����
	{TV_CHEST,SV_CHEST_MARIPO_21 , 6000},//�\�E���\�[�h
	{TV_CHEST,SV_CHEST_MARIPO_22 , 10000},//���[�G���O�����Z
	{TV_CHEST,SV_CHEST_MARIPO_23 , 15000},//���̉H��
	{TV_CHEST,SV_CHEST_MARIPO_24 , 30000},//�����I�[���i�v�t�^

	{0,0,0}//�I�[�p�_�~�[
};


/*:::���J���@�X�ɂĖ��������󂯎��A�C�e���𔻒�*/
///mod150811 �������E�Ƃ̎��̖��͒��ohook�ɂ��g��
bool marisa_will_buy(object_type *o_ptr)
{
	int i;
	if(p_ptr->pclass == CLASS_MARISA && o_ptr->tval < TV_BOOK_END) return FALSE;

	for(i=0;marisa_wants_table[i].tval;i++)
	{
		if(o_ptr->tval == marisa_wants_table[i].tval && o_ptr->sval == marisa_wants_table[i].sval) return TRUE;
	}

	//v2.0.13 �ɐ��Z�����z���悷�邱�Ƃɂ���B�E�Ɩ������̂Ƃ��̖��͒��o�̑Ώۂɂ͂Ȃ�Ȃ��B
	if (p_ptr->pclass != CLASS_MARISA && o_ptr->name1 == ART_IBUKI) return TRUE;

	return FALSE;

}
//EX�����p�������g���[�h�A�C�e��hook
bool marisa_will_trade(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ITEMCARD) return TRUE;
	if(marisa_will_buy(o_ptr)) return TRUE;
	return FALSE;
}



//EX�����p�ޖ��g���[�h�A�C�e��hook
bool yuma_will_trade(object_type *o_ptr)
{
	if (o_ptr->tval == TV_FOOD) return TRUE;

	if (o_ptr->tval == TV_SWEETS) return TRUE;

	if (o_ptr->tval == TV_ALCOHOL) return TRUE;

	if (o_ptr->tval == TV_MUSHROOM) return TRUE;

	if (o_ptr->tval == TV_FLASK) return TRUE;//���قƃG���W�j�A�p�G�l���M�[�p�b�N

	if (o_ptr->name1 == ART_IBUKI) return TRUE;//�ɐ��Z

	if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BIG_EGG) return TRUE; //����ȗ�

	if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU) return TRUE; //�����̚�

	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI) return TRUE; //�����S�̒|

	return FALSE;
}

static void building_prt_maripo(void)
{
	char tmp_str[80];
	prt("MARIPO: ", 22,53);

	sprintf(tmp_str, "%7ld", (long)p_ptr->maripo);
	prt(tmp_str, 22, 63);
}

/*:::�A�C�e����n���|�C���g���擾����*/
///mod150219 ����ɖ������p���͕ϐ��ɉ��Z
static void material_2_maripo(void)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type forge;
	object_type *q_ptr;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point=0, total_point;

	building_prt_maripo();

	q = "���������Ă����񂾁H";
	s = "���̃U�b�N�̒��Ɏ����~�������͂Ȃ����B";

	item_tester_hook = marisa_will_buy;
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	amt = 1;
	if (o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return;
	}
	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;

	object_desc(o_name, q_ptr, 0);

	//v2.0.13 �ɐ��Z��1500MARIPO�ƌ������Ă����悤�ɂ���
	if (o_ptr->name1 == ART_IBUKI)
	{
		base_point = 1500;
	}
	else
	{
		for (i = 0; marisa_wants_table[i].tval; i++)
			if (q_ptr->tval == marisa_wants_table[i].tval && q_ptr->sval == marisa_wants_table[i].sval) base_point = marisa_wants_table[i].maripo;
	}

	total_point = base_point * amt;

	if(total_point < 1) msg_format("ERROR:Maripo<1"); //paranoia

	if(base_point < 30) msg_format("�u���̒��x�̕����B�܂�%d�|�C���g���ď����ȁB�v",total_point);
	else if(base_point < 300) msg_format("�u�ق��A���X�̕��������Ă����ȁB%d�|�C���g�o�����B�v",total_point);
	else if(base_point < 1000) msg_format("�u���Ȃ�̒l�ł����̂���Ȃ����B%d�|�C���g�łǂ����H�v",total_point);
	else msg_format("�u�䂸���Ă��ꂽ�̂ށI%d�|�C���g�o���Ă��������I�v",total_point);

	if(!get_check(format("%s��n���܂����H",o_name))) return;

	p_ptr->maripo += total_point;
	if(p_ptr->maripo > 999999) p_ptr->maripo = 999999; 
	building_prt_maripo();
	msg_format("�u�悵�A���k���������I�v");
	msg_format("%d�|�C���g��MARIPO���l�������B",total_point);

	///mod150219 ���������͊l��
	//v2.0.13 �ɐ��Z��n�����Ƃ��ɂ͖��͂𓾂��Ȃ��͂�
	marisa_gain_power(q_ptr, 100);

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);		

}

/*:::�|�C���g������v���[���g���󂯎��*/
static void maripo_2_present(void)
{

	int cs=0,page=0,choice=-1;
	int i;
	char c;
	object_type forge;
	object_type *o_ptr = &forge;
	int slot_new;
	char o_name[MAX_NLEN];

	object_wipe(o_ptr);

	building_prt_maripo();
	if(inventory[INVEN_PACK-1].k_idx)
	{
		msg_print("�u���������A�ו�����t�����B�v");
		return;
	}

	while(1)
	{

		clear_bldg(5, 18);
		c_put_str(TERM_WHITE,"�������u�����̂����ڂɉ����āA�ǂ�ł��D���Ȃ��̂�I��ł���I�v",5 , 6);
		c_put_str(TERM_WHITE,"(a�`h:�v���[���g��I�� / n,�X�y�[�X:���̃y�[�W�� / ESC:�L�����Z��)",6 , 6);
		for(i=0;i<8;i++)
		{

			int color;
			int table_num = page * 8 + i;

			if((p_ptr->maripo < marisa_present_table[table_num].maripo) || ( (1L << table_num) & (p_ptr->maripo_presented_flag)) ) color = TERM_L_DARK;
			else if(cs==i) color = TERM_YELLOW;
			else color = TERM_WHITE;
			object_prep(o_ptr, lookup_kind(marisa_present_table[table_num].tval,marisa_present_table[table_num].sval) );
			object_desc(o_name, o_ptr, 0);
			if( ( (1L << table_num) & (p_ptr->maripo_presented_flag))) c_put_str(color,format("%c) [   �i�؂�   ] %s",'a'+i,o_name),8 + i, 10);
			else c_put_str(color,format("%c) [%6dMARIPO] %s",'a'+i,marisa_present_table[table_num].maripo,o_name),8 + i, 10);
		}
		prt("",8+i,10);

		c = inkey();
		if (c == ESCAPE)
		{
			clear_bldg(5, 18);
			return;
		}
		if (c == '2' && cs < 7) cs++;
		if (c == '8' && cs > 0) cs--;
		if (c == '\r' || c == '\n') c=cs+'a';
		if (c == ' ' || c == 'n') page++;
		if (page > 2) page = 0;

		if(c >= 'a' && c <= 'h')
		{
			choice = page*8 + c - 'a';

			if( (1L << choice) & (p_ptr->maripo_presented_flag))
			{
				msg_format("�u����͂����i�؂ꂾ���B�v");
				continue;
			}
			else if(p_ptr->maripo < marisa_present_table[choice].maripo)
			{
				msg_format("�u�����ȁB���ꂪ�~�����Ȃ�����ƃ|�C���g�𒙂߂Ă���B�v");
				continue;
			}

			if (get_check_strict("�u���ꂪ�~�����̂��H�v", CHECK_DEFAULT_Y))
			{
			 	break;
			}
		}		
	}

	p_ptr->maripo_presented_flag |= (1L << choice);
	p_ptr->maripo -= marisa_present_table[choice].maripo;
	building_prt_maripo();

	object_prep(o_ptr, lookup_kind(marisa_present_table[choice].tval,marisa_present_table[choice].sval) );
	if(o_ptr->tval == TV_CHEST) o_ptr->pval = -6; //(��{���̔� )	
	else apply_magic(o_ptr,50,AM_NO_FIXED_ART); //���̂Ƃ��딠�����v���[���g���Ȃ����O�̂���
	object_desc(o_name, o_ptr, 0);

	slot_new = inven_carry(o_ptr);
	msg_format("%s���󂯎�����B",o_name);
	autopick_alter_item(slot_new, FALSE);		
	handle_stuff();
	clear_bldg(5, 18);

}



/*:::�������֒��i�𔄂���邽�߂̕\�@���J���@�X�̃e�[�u���𗬗p TV,SV,������艿�i*/
struct marisa_store_type korin_wants_table[] =
{
	{TV_SOUVENIR,SV_SOUVENIR_SMARTPHONE,100000},
	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER,300000},
	{TV_SOUVENIR,SV_SOUVENIR_GAMEBOY,1000000},
	{TV_SOUVENIR,SV_SOUVENIR_ILMENITE,100000},
	{TV_SOUVENIR,SV_SOUVENIR_ULTRASONIC,5000},

	{TV_SOUVENIR,SV_SOUVENIR_KAPPA_5,300000},
	{TV_SOUVENIR,SV_SOUVENIR_PRISM,30000},
	{TV_SOUVENIR,SV_SOUVENIR_KAPPA_ARM,100000},
	{TV_SOUVENIR,SV_SOUVENIR_BIG_EGG,200000},
	{TV_SOUVENIR,SV_SOUVENIR_ASIA,700000},
	{TV_SOUVENIR,SV_SOUVENIR_STARS_AND_STRIPES,100000},
	{TV_SOUVENIR,SV_SOUVENIR_OUIJA_BOARD,20000},
	{TV_SOUVENIR,SV_SOUVENIR_KESERAN_PASARAN,250000},
	{TV_SOUVENIR,SV_SOUVENIR_MARISA_TUBO,50000},
	{TV_SOUVENIR,SV_SOUVENIR_DOUJU,120000},
	{ TV_SOUVENIR,SV_SOUVENIR_MORINJI,80000 },
	{TV_SOUVENIR,SV_SOUVENIR_KINKAKUJI,700000},
	{ TV_SOUVENIR,SV_SOUVENIR_SHILVER_KEY,1000000 },

	{ TV_SOUVENIR,SV_SOUVENIR_FOUR_MONKEYS,40000 },

	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE,600000},
	{TV_MATERIAL,SV_MATERIAL_METEORICIRON,300000},
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL,100000},
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE,20000},
	{TV_MATERIAL,SV_MATERIAL_ICESCALE,15000},
	{TV_MATERIAL,SV_MATERIAL_MYSTERIUM,140000},
	{TV_MATERIAL,SV_MATERIAL_TAKAKUSAGORI,16000},
	{TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT,120000},

	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU,50000 },

	//	{TV_SOFTDRINK,SV_SOFTDRINK_COLA,500},

	{ TV_SOUVENIR,SV_SOUVENIR_STONE_BABY,100 },
	{ TV_SOUVENIR,SV_SOUVENIR_MIRROR_OF_RUFFNOR ,200000 },
	{ TV_SOUVENIR,SV_SOUVENIR_TRUMPS_OF_DOOM ,300000 },
	{ TV_SOUVENIR,SV_SOUVENIR_SKIDBLADNIR ,500000 },
	{ TV_SOUVENIR,SV_SOUVENIR_NODENS_CHARIOT ,400000 },

	{ TV_SOUVENIR,SV_SOUVENIR_ELDER_THINGS_CRYSTAL ,30000 },

	{ TV_SOUVENIR,SV_SOUVENIR_KODOKUZARA ,66000 },
	{ TV_SOUVENIR,SV_SOUVENIR_MOON_ORB ,20000 },

	{0,0,0}//�I�[�p�_�~�[
};

/*:::�������ɂĒ��i�Ƃ��Ĉ��������A�C�e���𔻒�*/
static bool korin_will_buy(object_type *o_ptr)
{
	int i;
	for(i=0;korin_wants_table[i].tval;i++)
	{
		if(o_ptr->tval == korin_wants_table[i].tval && o_ptr->sval == korin_wants_table[i].sval) return TRUE;
	}
	return FALSE;

}

//v2.0.6
//�������ɒ��i�Ȃǂ𔄂����Ƃ��̉��i
//�ޖ����H�ׂ��Ƃ��̉h�{�v�Z�ɂ��g�������Ȃ����̂Ŋ֐��𕪂���
int calc_korin_sell_price(object_type *o_ptr)
{
	int base_point = 0;
	int i;

	for (i = 0; korin_wants_table[i].tval; i++)
	{
		if (o_ptr->tval == korin_wants_table[i].tval && o_ptr->sval == korin_wants_table[i].sval) base_point = korin_wants_table[i].maripo;
	}

	return base_point;

}


/*::�������ɒ��i�𔄂����*/
static void sell_curiosity(void)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type forge;
	object_type *q_ptr;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point=0, total_point;

	building_prt_gold();

	q = "���������[�����̂������Ă���悤���ˁH";
	s = "���ɋ����������悤�Ȃ��̂͂Ȃ��悤���ˁB";

	item_tester_hook = korin_will_buy;
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	amt = 1;
	if (o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return;
	}
	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;

	object_desc(o_name, q_ptr, 0);

	base_point = calc_korin_sell_price(q_ptr);

	total_point = base_point * amt;

	if(total_point < 1) msg_format("ERROR:Maripo<1"); //paranoia

	if(base_point < 10000) msg_format("�u����قǒ��������̂ł��Ȃ��ȁB�܂� $%d�Ƃ������Ƃ��납�B�v",total_point);
	else if(base_point < 100000) msg_format("�u�ӂށA�Ȃ��Ȃ��̕i���B$%d�o�����B�v",total_point);
	else if(base_point < 1000000) msg_format("�u�ق��A���ɋ����[����i���B$%d�ň�����点�Ăق����B�v",total_point);
	else 
	{
		msg_format("�u�n���ȁA����ȕ������E���E�E�I");
		msg_print(NULL);
		msg_format("���⎸�炵���B������ɂ�$%d���x�����p�ӂ�����B�v",total_point);
	}

	if(!get_check(format("%s��n���܂����H",o_name))) return;

	p_ptr->au += total_point;
	building_prt_gold();
	msg_format("�u�悵�A���k�������B�v");
	msg_format("$%d�Ŕ��蕥�����B",total_point);

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);		

}






/*:::���Z�ꏈ���B�����X�^�[��I�肵�Đ�킹��Ƃ���܂ŁH*/
///sysdel tougi inside_battle�͖ʓ|�Ȃ̂Ŏc�������̊֐��͍폜����\��
#if 0
void battle_monsters(void)
{
	int total, i;
	int max_dl = 0;
	int mon_level;
	int power[4];
	bool tekitou;
	bool old_inside_battle = p_ptr->inside_battle;

	for (i = 0; i < max_d_idx; i++)
		if (max_dl < max_dlv[i]) max_dl = max_dlv[i];

	mon_level = randint1(MIN(max_dl, 122))+5;
	if (randint0(100) < 60)
	{
		i = randint1(MIN(max_dl, 122))+5;
		mon_level = MAX(i, mon_level);
	}
	if (randint0(100) < 30)
	{
		i = randint1(MIN(max_dl, 122))+5;
		mon_level = MAX(i, mon_level);
	}

	while (1)
	{
		total = 0;
		tekitou = FALSE;
		for(i=0;i<4;i++)
		{
			int r_idx, j;
			while (1)
			{
				get_mon_num_prep(vault_aux_battle, NULL);
				p_ptr->inside_battle = TRUE;
				r_idx = get_mon_num(mon_level);
				p_ptr->inside_battle = old_inside_battle;
				if (!r_idx) continue;

				if ((r_info[r_idx].flags1 & RF1_UNIQUE) || (r_info[r_idx].flags7 & RF7_UNIQUE2))
				{
					if ((r_info[r_idx].level + 10) > mon_level) continue;
				}

				for (j = 0; j < i; j++)
					if(r_idx == battle_mon[j]) break;
				if (j<i) continue;

				break;
			}
			battle_mon[i] = r_idx;
			if (r_info[r_idx].level < 45) tekitou = TRUE;
		}

		for (i=0;i<4;i++)
		{
			monster_race *r_ptr = &r_info[battle_mon[i]];
			int num_taisei = count_bits(r_ptr->flagsr & (RFR_IM_ACID | RFR_IM_ELEC | RFR_IM_FIRE | RFR_IM_COLD | RFR_IM_POIS));

			if (r_ptr->flags1 & RF1_FORCE_MAXHP)
				power[i] = r_ptr->hdice * r_ptr->hside * 2;
			else
				power[i] = r_ptr->hdice * (r_ptr->hside + 1);
			power[i] = power[i] * (100 + r_ptr->level) / 100;
			if (r_ptr->speed > 110)
				power[i] = power[i] * (r_ptr->speed * 2 - 110) / 100;
			if (r_ptr->speed < 110)
				power[i] = power[i] * (r_ptr->speed - 20) / 100;
			if (num_taisei > 2)
				power[i] = power[i] * (num_taisei*2+5) / 10;
			else if (r_ptr->flags6 & RF6_INVULNER)
				power[i] = power[i] * 4 / 3;
			else if (r_ptr->flags6 & RF6_HEAL)
				power[i] = power[i] * 4 / 3;
			else if (r_ptr->flags5 & RF5_DRAIN_MANA)
				power[i] = power[i] * 11 / 10;
			if (r_ptr->flags1 & RF1_RAND_25)
				power[i] = power[i] * 9 / 10;
			if (r_ptr->flags1 & RF1_RAND_50)
				power[i] = power[i] * 9 / 10;

			switch (battle_mon[i])
			{
				case MON_GREEN_G:
				case MON_THAT_BAT:
				case MON_GHOST_Q:
					power[i] /= 4;
					break;
				case MON_LOST_SOUL:
				case MON_GHOST:
					power[i] /= 2;
					break;
				case MON_UND_BEHOLDER:
				case MON_SANTACLAUS:
				case MON_ULT_BEHOLDER:
				case MON_UNGOLIANT:
				case MON_ATLACH_NACHA:
				case MON_Y_GOLONAC:
					power[i] = power[i] * 3 / 5;
					break;
				case MON_ROBIN_HOOD:
				case MON_RICH:
				case MON_LICH:
				case MON_COLOSSUS:
				case MON_CRYPT_THING:
				case MON_MASTER_LICH:
				case MON_DREADMASTER:
				case MON_DEMILICH:
				case MON_SHADOWLORD:
				case MON_ARCHLICH:
				case MON_BLEYS:
				case MON_CAINE:
				case MON_JULIAN:
				case MON_VENOM_WYRM:
				case MON_MASTER_MYS:
				case MON_G_MASTER_MYS:
					power[i] = power[i] * 3 / 4;
					break;
				case MON_VORPAL_BUNNY:
				case MON_SHAGRAT:
				case MON_GORBAG:
				case MON_LOG_MASTER:
				case MON_JURT:
				case MON_GRAV_HOUND:
				case MON_SHIM_VOR:
				case MON_JUBJUB:
				case MON_CLUB_DEMON:
				case MON_LLOIGOR:
				case MON_NIGHTCRAWLER:
				case MON_NIGHTWALKER:
				case MON_RAPHAEL:
				case MON_SHAMBLER:
				case MON_SKY_DRAKE:
				case MON_GERARD:
				case MON_G_CTHULHU:
				case MON_SPECT_WYRM:
				case MON_BAZOOKER:
				//case MON_GCWADL:
				case MON_KIRIN:
				case MON_FENGHUANG:
					power[i] = power[i] * 4 / 3;
					break;
				case MON_UMBER_HULK:
				case MON_FIRE_VOR:
				case MON_WATER_VOR:
				case MON_COLD_VOR:
				case MON_ENERGY_VOR:
				case MON_GACHAPIN:
				case MON_REVENANT:
				//case MON_NEXUS_VOR:
				case MON_PLASMA_VOR:
				case MON_TIME_VOR:
				case MON_MANDOR:
				case MON_KAVLAX:
				//case MON_RINALDO:
				case MON_STORMBRINGER:
				case MON_TIME_HOUND:
				case MON_PLASMA_HOUND:
				case MON_TINDALOS:
				case MON_CHAOS_VOR:
				case MON_AETHER_VOR:
				case MON_AETHER_HOUND:
				case MON_CANTORAS:
				case MON_GODZILLA:
				case MON_TARRASQUE:
				case MON_DESTROYER:
				case MON_MORGOTH:
				case MON_SERPENT:
				case MON_OROCHI:
				case MON_D_ELF_SHADE:
				case MON_MANA_HOUND:
				case MON_SHARD_VOR:
				case MON_BANORLUPART:
				case MON_BOTEI:
				case MON_JAIAN:
				case MON_BAHAMUT:
				case MON_WAHHA:
					power[i] = power[i] * 3 / 2;
					break;
				case MON_ROLENTO:
				case MON_CYBER:
				case MON_CYBER_KING:
				case MON_UNICORN_ORD:
					power[i] = power[i] * 5 / 3;
					break;
				case MON_ARCH_VILE:
				case MON_PHANTOM_B:
				case MON_WYRM_POWER:
					power[i] *= 2;
					break;
				case MON_NODENS:
				case MON_CULVERIN:
					power[i] *= 3;
					break;
				case MON_ECHIZEN:
					power[i] *= 9;
					break;
				case MON_HAGURE:
					power[i] *= 100000;
					break;
				default:
					break;
			}
			total += power[i];
		}
		for (i=0;i<4;i++)
		{
			power[i] = total*60/power[i];
			if (tekitou && ((power[i] < 160) || power[i] > 1500)) break;
			if ((power[i] < 160) && randint0(20)) break;
			if (power[i] < 101) power[i] = 100 + randint1(5);
			mon_odds[i] = power[i];
		}
		if (i == 4) break;
	}
}
#endif

#if 0
static bool kakutoujou(void)
{
	s32b maxbet;
	s32b wager;
	char out_val[160], tmp_str[80];
	cptr p;

	if ((turn - old_battle) > TURNS_PER_TICK*250)
	{
		battle_monsters();
		old_battle = turn;
	}

	screen_save();

	/* No money */
	if (p_ptr->au < 1)
	{
#ifdef JP
		msg_print("�����I���܂��ꕶ�Ȃ�����Ȃ����I��������o�Ă����I");
#else
		msg_print("Hey! You don't have gold - get out of here!");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}
	else
	{
		int i;

		clear_bldg(4, 10);

#ifdef JP
		prt("�����X�^�[                                                     �{��", 4, 4);
#else
		prt("Monsters                                                       Odds", 4, 4);
#endif
		for (i=0;i<4;i++)
		{
			char buf[80];
			monster_race *r_ptr = &r_info[battle_mon[i]];

#ifdef JP
			sprintf(buf,"%d) %-58s  %4ld.%02ld�{", i+1, format("%s%s",r_name + r_ptr->name, (r_ptr->flags1 & RF1_UNIQUE) ? "���ǂ�" : "      "), (long int)mon_odds[i]/100, (long int)mon_odds[i]%100);
#else
			sprintf(buf,"%d) %-58s  %4ld.%02ld", i+1, format("%s%s", (r_ptr->flags1 & RF1_UNIQUE) ? "Fake " : "", r_name + r_ptr->name), (long int)mon_odds[i]/100, (long int)mon_odds[i]%100);
#endif
			prt(buf, 5+i, 1);
		}

#ifdef JP
		prt("�ǂ�ɓq���܂���:", 0, 0);
#else
		prt("Which monster: ", 0, 0);
#endif
		while(1)
		{
			i = inkey();

			if (i == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
			if (i >= '1' && i <= '4')
			{
				sel_monster = i-'1';
				battle_odds = mon_odds[sel_monster];
				break;
			}
			else bell();
		}

		clear_bldg(4,4);
		for (i=0;i<4;i++)
			if (i !=sel_monster) clear_bldg(i+5,i+5);

		maxbet = p_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, p_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"�q���� (1-%ld)�H", (long int)maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", (long int)maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > p_ptr->au)
			{
#ifdef JP
msg_print("�����I��������Ȃ�����Ȃ����I�o�Ă����I");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ld�S�[���h�����󂯂悤�B�c��͎���Ƃ��ȁB", (long int)maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.",(long int) maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�n�j�A�P�S�[���h�ł������B");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			battle_odds = MAX(wager+1, wager * battle_odds / 100);
			kakekin = wager;
			p_ptr->au -= wager;
			reset_tim_flags();

			/* Save the surface floor as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS);

			p_ptr->inside_battle = TRUE;
			p_ptr->leaving = TRUE;

			leave_bldg = TRUE;
			screen_load();

			return (TRUE);
		}
	}
	screen_load();

	return (FALSE);
}
#endif


/*:::�{���̏܋����\������B���肷��̂͂����ł͂Ȃ�*/
///del131221 �܋���폜
#if 0
static void today_target(void)
{
	char buf[160];
	monster_race *r_ptr = &r_info[today_mon];

	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "�{���̏܋���", 5, 10);
#else
	prt("Wanted monster that changes from day to day", 5, 10);
#endif
#ifdef JP
	sprintf(buf,"�^�[�Q�b�g�F %s",r_name + r_ptr->name);
#else
	sprintf(buf,"target: %s",r_name + r_ptr->name);
#endif
	c_put_str(TERM_YELLOW, buf, 6, 10);
#ifdef JP
	sprintf(buf,"���� ---- $%d",r_ptr->level * 50 + 100);
#else
	sprintf(buf,"corpse   ---- $%d",r_ptr->level * 50 + 100);
#endif
	prt(buf, 8, 10);
#ifdef JP
	sprintf(buf,"��   ---- $%d",r_ptr->level * 30 + 60);
#else
	sprintf(buf,"skeleton ---- $%d",r_ptr->level * 30 + 60);
#endif
	prt(buf, 9, 10);
	p_ptr->today_mon = today_mon;
}
#endif

///del131231 �܋���폜
#if 0
static void tsuchinoko(void)
{
	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "��l����̑�`�����X�I�I�I", 5, 10);
c_put_str(TERM_YELLOW, "�^�[�Q�b�g�F���̒��b�u�c�`�m�R�v", 6, 10);
c_put_str(TERM_WHITE, "�����߂� ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "����     ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "��       ----   $100,000", 10, 10);
#else
c_put_str(TERM_YELLOW, "Big chance to quick money!!!", 5, 10);
c_put_str(TERM_YELLOW, "target: the rarest animal 'Tsuchinoko'", 6, 10);
c_put_str(TERM_WHITE, "catch alive ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "corpse      ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "bones       ----   $100,000", 10, 10);
#endif
}
#endif

///del131231 �܋���폜
#if 0

static void shoukinkubi(void)
{
	int i;
	int y = 0;

	clear_bldg(4,18);

#ifdef JP
	prt("���̂������A��Ε�V�������グ�܂��B",4 ,10);
c_put_str(TERM_YELLOW, "���݂̏܋���", 6, 10);
#else
	prt("Offer a prize when you bring a wanted monster's corpse",4 ,10);
c_put_str(TERM_YELLOW, "Wanted monsters", 6, 10);
#endif

	for (i = 0; i < MAX_KUBI; i++)
	{
		byte color;
		cptr done_mark;
		/*:::���Ɋ����ς݂̏ꍇidx��+10000����Ă���炵��*/
		monster_race *r_ptr = &r_info[(kubi_r_idx[i] > 10000 ? kubi_r_idx[i] - 10000 : kubi_r_idx[i])];

		if (kubi_r_idx[i] > 10000)
		{
			color = TERM_RED;
#ifdef JP
			done_mark = "(��)";
#else
			done_mark = "(done)";
#endif
		}
		else
		{
			color = TERM_WHITE;
			done_mark = "";
		}

		c_prt(color, format("%s %s", r_name + r_ptr->name, done_mark), y+7, 10);

		y = (y+1) % 10;
		if (!y && (i < MAX_KUBI -1))
		{
#ifdef JP
			prt("�����L�[�������Ă�������", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			clear_bldg(7,18);
		}
	}
}
#endif

/* List of prize object */
static struct {
	s16b tval;
	s16b sval;
} prize_list[MAX_KUBI] = 
{
	{TV_POTION, SV_POTION_CURING},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_SPEED},
	{TV_POTION, SV_POTION_RESISTANCE},
	{TV_POTION, SV_POTION_ENLIGHTENMENT},

	{TV_POTION, SV_POTION_HEALING},
	{TV_POTION, SV_POTION_RESTORE_MANA},
	{TV_SCROLL, SV_SCROLL_STAR_DESTRUCTION},
	{TV_POTION, SV_POTION_STAR_ENLIGHTENMENT},
	{TV_SCROLL, SV_SCROLL_SUMMON_PET},

	{TV_SCROLL, SV_SCROLL_GENOCIDE},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_STAR_HEALING},
	{TV_POTION, SV_POTION_NEW_LIFE},
	{TV_SCROLL, SV_SCROLL_MASS_GENOCIDE},

	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_LIFE},
	{TV_POTION, SV_POTION_AUGMENTATION},
	{TV_POTION, SV_POTION_INVULNERABILITY},
	{TV_SCROLL, SV_SCROLL_ARTIFACT},
};


/* Get prize */
/*:::�܋��������A�C�e���Ɉ���������*/
///del131221 ���̍폜�ɔ������������폜
#if 0
static bool kankin(void)
{
	int i, j;
	bool change = FALSE;
	char o_name[MAX_NLEN];
	object_type *o_ptr;

	/* Loop for inventory and right/left arm */
	for (i = 0; i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];

		/* Living Tsuchinoko worthes $1000000 */
		if ((o_ptr->tval == TV_CAPTURE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)(1000000L * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)(1000000L * o_ptr->number));
#endif
				p_ptr->au += 1000000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Corpse of Tsuchinoko worthes $200000 */
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)(200000L * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)(200000L * o_ptr->number));
#endif
				p_ptr->au += 200000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Bones of Tsuchinoko worthes $100000 */
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)(100000L * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)(100000L * o_ptr->number));
#endif
				p_ptr->au += 100000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)((r_info[today_mon].level * 50 + 100) * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)((r_info[today_mon].level * 50 + 100) * o_ptr->number));
#endif
				p_ptr->au += (r_info[today_mon].level * 50 + 100) * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s ���������܂����H",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (long int)((r_info[today_mon].level * 30 + 60) * o_ptr->number));
#else
				msg_format("You get %ldgp.", (long int)((r_info[today_mon].level * 30 + 60) * o_ptr->number));
#endif
				p_ptr->au += (r_info[today_mon].level * 30 + 60) * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (j = 0; j < MAX_KUBI; j++)
	{
		/* Need reverse order --- Positions will be changed in the loop */
		for (i = INVEN_PACK-1; i >= 0; i--)
		{
			o_ptr = &inventory[i];
			if ((o_ptr->tval == TV_CORPSE) && (o_ptr->pval == kubi_r_idx[j]))
			{
				char buf[MAX_NLEN+20];
				int num, k, item_new;
				object_type forge;

				object_desc(o_name, o_ptr, 0);
#ifdef JP
				sprintf(buf, "%s��n���܂����H",o_name);
#else
				sprintf(buf, "Hand %s over? ",o_name);
#endif
				if (!get_check(buf)) continue;

#if 0 /* Obsoleted */
#ifdef JP
				msg_format("�܋� %ld������ɓ��ꂽ�B", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#else
				msg_format("You get %ldgp.", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#endif
				p_ptr->au += (r_info[kubi_r_idx[j]].level+1) * 300 * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
				chg_virtue(V_JUSTICE, 5);
				kubi_r_idx[j] += 10000;

				change = TRUE;
#endif /* Obsoleted */

				/* Hand it first */
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);

				chg_virtue(V_JUSTICE, 5);
				kubi_r_idx[j] += 10000;

				/* Count number of unique corpses already handed */
				for (num = 0, k = 0; k < MAX_KUBI; k++)
				{
					if (kubi_r_idx[k] >= 10000) num++;
				}

#ifdef JP
				msg_format("����ō��v %d �|�C���g�l�����܂����B", num);
#else
				msg_format("You earned %d point%s total.", num, (num > 1 ? "s" : ""));
#endif

				/* Prepare to make a prize */
				object_prep(&forge, lookup_kind(prize_list[num-1].tval, prize_list[num-1].sval));
				apply_magic(&forge, object_level, AM_NO_FIXED_ART);

				/* Identify it fully */
				object_aware(&forge);
				object_known(&forge);

				/*
				 * Hand it --- Assume there is an empty slot.
				 * Since a corpse is handed at first,
				 * there is at least one empty slot.
				 */
				item_new = inven_carry(&forge);

				/* Describe the object */
				object_desc(o_name, &forge, 0);
#ifdef JP
				msg_format("%s(%c)�������B", o_name, index_to_label(item_new));
#else
				msg_format("You get %s (%c). ", o_name, index_to_label(item_new));
#endif

				/* Auto-inscription */
				autopick_alter_item(item_new, FALSE);

				/* Handle stuff */
				handle_stuff();

				change = TRUE;
			}
		}
	}

	if (!change)
	{
#ifdef JP
		msg_print("�܋��𓾂�ꂻ���Ȃ��͎̂����Ă��Ȃ������B");
#else
		msg_print("You have nothing.");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}
#endif


/*:::����������Ƃ��ɏo�Ă��郂���X�^�[�@�G���h���b�`�z���[������I�o*/
bool get_nightmare(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Require eldritch horrors */
	if (!(r_ptr->flags2 & (RF2_ELDRITCH_HORROR))) return (FALSE);

	/* Require high level */
	if (r_ptr->level <= p_ptr->lev) return (FALSE);

	/* Accept this monster */
	return (TRUE);
}

/*:::�������Ƃ��Ɉ��������鏈���@�������[�h�ŏh���Ŏ~�܂����薰�炳�ꂽ�薃Ⴢ����Ƃ�*/
void have_nightmare(int r_idx)
{
	bool happened = FALSE;
	monster_race *r_ptr = &r_info[r_idx];
	int power = r_ptr->level + 10;
	char m_name[80];
	cptr desc = r_name + r_ptr->name;

	if(p_ptr->pclass == CLASS_DOREMY)
	{
		msg_print("���Ȃ��͈�������������������B");
		(void)set_food(PY_FOOD_MAX - 1);
		return;
	}

	if(r_idx == MON_CLOWNPIECE || weird_luck())
	{
		msg_print("�����u�M���̈����͎����������������܂����B�v");
		return;
	}

	/* Describe it */
#ifndef JP
	if (!(r_ptr->flags1 & RF1_UNIQUE))
		sprintf(m_name, "%s %s", (is_a_vowel(desc[0]) ? "an" : "a"), desc);
	else
#endif
		sprintf(m_name, "%s", desc);

	if (!(r_ptr->flags1 & RF1_UNIQUE))
	{
		if (r_ptr->flags1 & RF1_FRIENDS) power /= 2;
	}
	else power *= 2;

	if (saving_throw(p_ptr->skill_sav * 100 / power))
	{

		if(one_in_(7) || p_ptr->pclass == CLASS_REIMU)
			msg_format("���̒��ŏ����E���̒e���Ɉ͂܂ꂽ�B", m_name);
		else
			msg_format("���̒���%s�ɒǂ�������ꂽ�B", m_name);

		/* Safe */
		return;
	}

	if (p_ptr->image)
	{
		/* Something silly happens... */
#ifdef JP
		msg_format("%s%s�̊�����Ă��܂����I",
#else
		msg_format("You behold the %s visage of %s!",
#endif

					  funny_desc[randint0(MAX_SAN_FUNNY)], m_name);

		if (one_in_(3))
		{
			msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
			p_ptr->image = p_ptr->image + randint1(r_ptr->level);
		}

		/* Never mind; we can't see it clearly enough */
		return;
	}

	/* Something frightening happens... */
#ifdef JP
	msg_format("%s%s�̊�����Ă��܂����I",
#else
	msg_format("You behold the %s visage of %s!",
#endif

				  horror_desc[randint0(MAX_SAN_HORROR)], desc);

	r_ptr->r_flags2 |= RF2_ELDRITCH_HORROR;

	///mod140126 �������������C�Ɖu�Ƒϐ��܂߂ď����Ȃ�����
#if 0
	if (!p_ptr->mimic_form)
	{
		///race �����������Ƃ��̃G���h���b�`�z���[�ϐ�
		///sys �G���h���b�`�z���[�ϐ��@������ϐ��̈�Ƃ��Ĕ��肷��\��
		switch (p_ptr->prace)
		{
		/* Demons may make a saving throw */
		case RACE_IMP:
		//case RACE_DEMON:
			if (saving_throw(20 + p_ptr->lev)) return;
			break;
		/* Undead may make a saving throw */
		case RACE_SKELETON:
		case RACE_ZOMBIE:
		case RACE_SPECTRE:
		case RACE_VAMPIRE:
			if (saving_throw(10 + p_ptr->lev)) return;
			break;
		}
	}
	///mod131228 ���C�ϐ��ň����ɒ�R
	else if(p_ptr->resist_insanity && saving_throw(30 + p_ptr->lev)){
		return;
	}
	else
	{
		/* Demons may make a saving throw */
		if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)
		{
			if (saving_throw(20 + p_ptr->lev)) return;
		}
		/* Undead may make a saving throw */
		else if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_UNDEAD)
		{
			if (saving_throw(10 + p_ptr->lev)) return;
		}
	}
#endif
	if(immune_insanity()) return;
	else if(p_ptr->resist_insanity && saving_throw(15 + p_ptr->lev)) return;


	/* Mind blast */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->resist_chaos && one_in_(3))
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}

	/* Lose int & wis */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
		return;
	}

	/* Brain smash */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
		}
		while (!saving_throw(p_ptr->skill_sav))
		{
			(void)do_dec_stat(A_INT);
		}
		while (!saving_throw(p_ptr->skill_sav))
		{
			(void)do_dec_stat(A_WIS);
		}
		if (!p_ptr->resist_chaos)
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}

	///mutation �����������Ƃ��̈����ˑR�ψ�
	/* Amnesia */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (lose_all_info())
		{
#ifdef JP
msg_print("���܂�̋��|�ɑS�Ă̂��Ƃ�Y��Ă��܂����I");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		}
		return;
	}
	//�H���l�͕ψٖ���
	if(p_ptr->prace == RACE_HOURAI) return;

	get_bad_mental_mutation();

#if 0

	/* Else gain permanent insanity */
	if ((p_ptr->muta3 & MUT3_MORONIC) && (p_ptr->muta2 & MUT2_BERS_RAGE) &&
		((p_ptr->muta2 & MUT2_COWARDICE) || (p_ptr->resist_fear)) &&
		((p_ptr->muta2 & MUT2_HALLU) || (p_ptr->resist_chaos)))
	{
		/* The poor bastard already has all possible insanities! */
		return;
	}

	while (!happened)
	{
		switch (randint1(4))
		{
			case 1:
			{
				if (!(p_ptr->muta3 & MUT3_MORONIC))
				{
					if ((p_ptr->stat_use[A_INT] < 4) && (p_ptr->stat_use[A_WIS] < 4))
					{
#ifdef JP
msg_print("���Ȃ��͊����Ȕn���ɂȂ����悤�ȋC�������B����������͌��X�������B");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
msg_print("���Ȃ��͊����Ȕn���ɂȂ����I");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if (p_ptr->muta3 & MUT3_HYPER_INT)
					{
#ifdef JP
msg_print("���Ȃ��̔]�͐��̃R���s���[�^�ł͂Ȃ��Ȃ����B");
#else
						msg_print("Your brain is no longer a living computer.");
#endif

						p_ptr->muta3 &= ~(MUT3_HYPER_INT);
					}
					p_ptr->muta3 |= MUT3_MORONIC;
					happened = TRUE;
				}
				break;
			}
			case 2:
			{
				if (!(p_ptr->muta2 & MUT2_COWARDICE) && !p_ptr->resist_fear)
				{
#ifdef JP
msg_print("���Ȃ��͕����|�ǂɂȂ����I");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if (p_ptr->muta3 & MUT3_FEARLESS)
					{
#ifdef JP
msg_print("���Ȃ��͂�������m�炸�ł͂Ȃ��Ȃ����B");
#else
						msg_print("You are no longer fearless.");
#endif

						p_ptr->muta3 &= ~(MUT3_FEARLESS);
					}

					p_ptr->muta2 |= MUT2_COWARDICE;
					happened = TRUE;
				}
				break;
			}
			case 3:
			{
				if (!(p_ptr->muta2 & MUT2_HALLU) && !p_ptr->resist_chaos)
				{
#ifdef JP
msg_print("���o���Ђ��N�������_�����Ɋׂ����I");
#else
					msg_print("You are afflicted by a hallucinatory insanity!");
#endif

					p_ptr->muta2 |= MUT2_HALLU;
					happened = TRUE;
				}
				break;
			}
			default:
			{
				if (!(p_ptr->muta2 & MUT2_BERS_RAGE))
				{
#ifdef JP
msg_print("����Ȋ���̔���ɂ�������悤�ɂȂ����I");
#else
					msg_print("You become subject to fits of berserk rage!");
#endif

					p_ptr->muta2 |= MUT2_BERS_RAGE;
					happened = TRUE;
				}
				break;
			}
		}
	}
#endif
	p_ptr->update |= PU_BONUS;
	handle_stuff();
}


/*
 * inn commands
 * Note that resting for the night was a perfect way to avoid player
 * ghosts in the town *if* you could only make it to the inn in time (-:
 * Now that the ghosts are temporarily disabled in 2.8.X, this function
 * will not be that useful.  I will keep it in the hopes the player
 * ghost code does become a reality again. Does help to avoid filthy urchins.
 * Resting at night is also a quick way to restock stores -KMW-
 */
/*:::�h���R�}���h*/
///sys �h���@�H�����ɐH���s�́��������Ŕ��ʂ��ă��b�Z�[�W�o���悤�ɂ���\��
static bool inn_comm(int cmd)
{
	switch (cmd)
	{
		case BACT_FOOD: /* Buy food & drink */

			if(    p_ptr->prace == RACE_TSUKUMO
				|| p_ptr->prace == RACE_ANDROID
				|| p_ptr->prace == RACE_GOLEM
				|| p_ptr->prace == RACE_VAMPIRE
				|| p_ptr->pclass == CLASS_HINA	
				|| p_ptr->prace == RACE_MAGIC_JIZO
				|| p_ptr->prace == RACE_ANIMAL_GHOST//v1.1.85�@�Y��Ă��̂Œǉ�
				|| p_ptr->prace == RACE_HANIWA
				|| p_ptr->pclass == CLASS_CHIYARI
				)
			{
				msg_print("���Ȃ������̐H���͂����ɂ͂Ȃ��悤���B");
				return FALSE;
			}
			else if(p_ptr->pclass == CLASS_DOREMY)
			{
				msg_print("���ʂ̐H���ɂ͋������Ȃ��B�܂��h�ɔ��܂낤�c");
				return FALSE;
			}

			//Ex���[�h�̃~�X�e�B�A�̉���ɂ���Ƃ�
			if(EXTRA_MODE && dun_level && building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_MYSTIA)
			{
				if(p_ptr->food < PY_FOOD_FULL -1 || p_ptr->blind || p_ptr->pclass == CLASS_YUMA)
				{
					msg_print("�X��͔��ډV�̊��Ă���U�����Ă��ꂽ�B");
					(void)set_food(PY_FOOD_MAX - 1);
					set_blind(0);
					//v2.0.10�ǉ�
					set_tim_invis(5000, FALSE);

					break;
				}
				else
				{
					msg_print("�X��͈�Ȕ�I���Ă��ꂽ�B");
					if(!p_ptr->resist_blind) set_blind(p_ptr->blind + 50 + randint1(50));
					return FALSE;
				}

			}
			//v1.1.53 Ex���[�h�̃����o�̕����ɂ���Ƃ�
			if (EXTRA_MODE && dun_level && building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_LARVA)
			{
				if (p_ptr->food < PY_FOOD_FULL - 1 )
				{
					msg_print("���Ȃ��͂����Ƃ��َq���y���񂾁B");
					(void)set_food(PY_FOOD_MAX - 1);
					break;
				}

			}

			if (p_ptr->food >= PY_FOOD_FULL && p_ptr->pclass != CLASS_YUMA)
			{
				msg_print("���͖������B");

				return FALSE;
			}

			if(EXTRA_MODE)
				msg_print("���Ȃ��͎葁���H�����ς܂����B");
			else
				msg_print("�X��͗����ƈ��ݕ���U�����Ă��ꂽ�B");

			(void)set_food(PY_FOOD_MAX - 1);
			break;

		case BACT_REST: /* Rest for the night */
			if ((p_ptr->poisoned) || (p_ptr->cut))
			{
#ifdef JP
				msg_print("���Ȃ��ɕK�v�Ȃ͕̂����ł͂Ȃ��A���Î҂ł��B");
#else
				msg_print("You need a healer, not a room.");
#endif

				msg_print(NULL);
#ifdef JP
				msg_print("���݂܂���A�ł������ŒN���Ɏ��Ȃꂿ�።��܂���ŁB");
#else
				msg_print("Sorry, but don't want anyone dying in here.");
#endif
			}
			else
			{
				s32b oldturn = turn;
				int prev_day, prev_hour, prev_min;

				extract_day_hour_min(&prev_day, &prev_hour, &prev_min);
#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�h���ɔ��܂����B");
#else
				if ((prev_hour >= 6) && (prev_hour <= 17)) do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stay over daytime at the inn.");
				else do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stay over night at the inn.");
#endif
				turn = (turn / (TURNS_PER_TICK*TOWN_DAWN/2) + 1) * (TURNS_PER_TICK*TOWN_DAWN/2);

				if(p_ptr->muta3 & MUT3_BYDO) p_ptr->bydo += turn - oldturn;
				if(p_ptr->bydo > 100000 * 3) p_ptr->bydo = 100000 * 3+1;

				//v1.1.59 �T�j�[�͖���ƒ~�ς�������������
				sunny_charge_sunlight(-1*(SUNNY_CHARGE_SUNLIGHT_MAX));

				if (dungeon_turn < dungeon_turn_limit)
				{
					dungeon_turn += MIN(turn - oldturn, TURNS_PER_TICK * 250);
					if (dungeon_turn > dungeon_turn_limit) dungeon_turn = dungeon_turn_limit;
				}

				//v2.0.6 �ޖ������Ă���\�͂�ϐ����h�ɔ������Ƃ������鏈��
				if (p_ptr->pclass == CLASS_YUMA && (turn - oldturn) >= YUMA_FLAG_DELETE_TICK)
				{
					yuma_lose_extra_power((turn - oldturn) / YUMA_FLAG_DELETE_TICK);
				}

				prevent_turn_overflow();

				//�h�ɔ��܂��ē��t���ς��Ƃ�
				if ((prev_hour >= 18) && (prev_hour <= 23))
				{
					do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

					p_ptr->today_mon = 0; //�얲�̐肢���Z�b�g

					//v2.05 �͂��Ă̐l�T�����Z�b�g EXTRA�ł͓����o�߂łȂ��t���A�ʉ߂Ń��Z�b�g�����̂ł����ł͉������Ȃ�
					if (!EXTRA_MODE)
					{
						p_ptr->hatate_mon_search_ridx = 0;
						p_ptr->hatate_mon_search_dungeon = 0;

					}

				}

				p_ptr->chp = p_ptr->mhp;

				engineer_malfunction(0, 5000); //�G���W�j�A�̕s���̋@�B���񕜂���

				if(CHECK_FAIRY_TYPE == 12 && lose_all_info()) msg_print("�����X�b�L�������I�ł������Y��Ă���悤�ȁB");

				if (ironman_nightmare && !diehardmind() && p_ptr->pclass != CLASS_DOREMY)
				{
#ifdef JP
					msg_print("����ɏA���Ƌ��낵�����i���S���悬�����B");
#else
					msg_print("Horrible visions flit through your mind as you sleep.");
#endif

					/* Pick a nightmare */
					get_mon_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					while(1)
					{
						have_nightmare(get_mon_num(MAX_DEPTH));

						if (!one_in_(3)) break;
					}

					/* Remove the monster restriction */
					get_mon_num_prep(NULL, NULL);

#ifdef JP
					msg_print("���Ȃ��͐⋩���Ėڂ��o�܂����B");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����ɂ��Ȃ���Ă悭����Ȃ������B");
#else
					msg_print("You awake screaming.");
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "be troubled by a nightmare.");
#endif
				}
				else
				{
					///mod150920 �h���~�[���ꏈ��
					if(p_ptr->pclass == CLASS_DOREMY)
					{
						if(ironman_nightmare)
							msg_print("�h���q�݂̂閲�͂ǂ���ɏ�̈������I");
						else
							msg_print("���Ȃ��͑��̏h���q�̖�����������������B");
						(void)set_food(PY_FOOD_MAX - 1);

					}
					set_blind(0);
					set_confused(0);
					p_ptr->stun = 0;
					p_ptr->chp = p_ptr->mhp;
					p_ptr->csp = p_ptr->msp;
					if(p_ptr->pclass == CLASS_SEIJA || p_ptr->pclass == CLASS_MAGIC_EATER)
					{
						restore_seija_item();
					}

					//v1.1.86 �h���ɘA���Ŕ��܂�����process_world()��6:00/18:00�̏�����ʂ�Ȃ����Ƃ�����̂ł����ɂ������Ƃ�
					//v1.1.87�@�g���[�h�J�E���g���Z�b�g��6:00��18:00�̗����ōs�����Ƃɂ����̂ŕs�v�ɂȂ����B
					//�A���ŏh���ɔ��܂����獂���x�Z�b�g���h����o���Ƃ��̈�x�����ɂȂ��Ă��܂����܂����͂Ȃ����낤�B
					//�����s��g���[�h�J�E���g�����Z�b�g
					//ability_card_trade_count = 0;

					if ((prev_hour >= 6) && (prev_hour <= 17))
					{
						msg_print("���Ȃ��̓��t���b�V�����Ėڊo�߁A�[�����}�����B");
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�[�����}�����B");

					}
					else
					{
						msg_print("���Ȃ��̓��t���b�V�����Ėڊo�߁A�V���ȓ����}�����B");
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����������������}�����B");


					}

					set_asthma(p_ptr->asthma - 6000);
				}

				//v1.1.29 �h�ɔ��܂����琌�����o�߂邱�Ƃɂ���
				set_alcohol(0);
			}
			break;

		case BACT_RUMORS: /* Listen for rumors */
			{
				display_rumor(TRUE);
				break;
			}
	}

	return (TRUE);
}



//v1.1.85 �ꕔ�N�G�X�g�̐l�ԑ�����/�d�������͂̕���̂��߂ɒǉ��B�O�����l�ԑ����͂̂Ƃ�TRUE
//���̍��d���l�b�g���[�N��H�ߍ����N�G�̐l�ԑ�����Ƃ͈Ⴄ�̂Œ���
//�u�l�Ԃ��猩�Đl�ԑ��Ɍ�����v�Ɓu�d�����猩�Đl�ԑ��Ɍ�����v�͔����ɈႤ�̂ŔY�܂������A�Ƃ肠�����l���̈�ʐl���猩�Ē��Ԃ��ۂ����TRUE
//TODO:�u�l�ԑ��Ɍ�����v�Ɓu�d�����Ɍ�����v�𗼕��������푰��L���������邵���̂����d��������p�̊֐���ʂɍ��ׂ����������
bool	player_looks_human_side()
{

	//�ϐg��
	//�l�Ԃɉ����Ă���ꍇ�͒ʂ�A����ȊO�ɉ����Ă���Ƃ��ʂ�Ȃ����Ƃɂ��Ă݂�B
	if (p_ptr->tim_mimic)
	{
		if (p_ptr->mimic_form == MIMIC_MARISA) return TRUE;
		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL)
		{
			monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];

			if ((r_ptr->flags3 & (RF3_HUMAN)) && !(r_ptr->flags3 & (RF3_KWAI | RF3_DEMIHUMAN | RF3_DRAGON | RF3_DEMON | RF3_UNDEAD | RF3_ANIMAL | RF3_FAIRY | RF3_HANIWA))) return TRUE;

		}
		return FALSE;
	}

	//���̎푰����̗�O �l���Ŗ������Ƃ��Ė����ʂ��Ă����ȖʁX�Ɣ������Ă����ȖʁX

	if (p_ptr->pclass == CLASS_BYAKUREN) return TRUE;
	if (p_ptr->pclass == CLASS_SHOU) return TRUE;
	if (p_ptr->pclass == CLASS_KEINE) return TRUE;
	if (p_ptr->pclass == CLASS_AUNN) return TRUE;
	if (p_ptr->pclass == CLASS_UDONGE) return TRUE;
	if (p_ptr->pclass == CLASS_TEWI) return TRUE;

	if (p_ptr->pclass == CLASS_HINA) return FALSE;
	if (p_ptr->pclass == CLASS_SHION) return FALSE;

	//�푰�ɂ���{����
	switch (p_ptr->prace)
	{
	case RACE_HUMAN:
	case RACE_KOBITO:
	case RACE_GREATER_LILLIPUT:
	case RACE_HALF_YOUKAI:
	case RACE_SENNIN:
	case RACE_TENNIN:
	case RACE_DEATH:
	case RACE_STRAYGOD:
	case RACE_DEMIGOD:
	case RACE_HALF_GHOST:
	case RACE_DEITY:
	case RACE_JK:
	case RACE_HOURAI:
	case RACE_ZASHIKIWARASHI:
	case RACE_MAGIC_JIZO:
		return TRUE;

	}

	return FALSE;

}





/*
 * Display quest information
 */
/*:::�N�G�X�g�̕��͂�\������*/
static void get_questinfo(int questnum, bool do_init)
{
	int     i;
	int     old_quest;
	char    tmp_str[80];


	/* Clear the text */
	for (i = 0; i < 10; i++)
	{
		quest_text[i][0] = '\0';
	}

	quest_text_line = 0;

	/* Set the quest number temporary */
	old_quest = p_ptr->inside_quest;
	p_ptr->inside_quest = questnum;

	/* Get the quest text */
	init_flags = INIT_SHOW_TEXT;
	if (do_init) init_flags |= INIT_ASSIGN;

	/*:::�Y���N�G�X�g�̏�����́B�t���O��INIT_SHOW_TEXT�Ȃ̂�quest_text[][]�̂ݍX�V�����H*/
	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	///mod140512 ���ꃋ�[�`���ǉ��@��������̃��j�[�N�v���C���[�̂Ƃ��N�G�X�g���͂��s���R�ɂȂ�̂ŏ���������
	(void) check_quest_unique_text();

	/* Reset the old quest number */
	p_ptr->inside_quest = old_quest;

	/* Print the quest info */
#ifdef JP
sprintf(tmp_str, "�N�G�X�g��� (�댯�x: %d �K����)", quest[questnum].level);
#else
	sprintf(tmp_str, "Quest Information (Danger level: %d)", quest[questnum].level);
#endif

	prt(tmp_str, 5, 0);

	prt(quest[questnum].name, 7, 0);

	for (i = 0; i < 10; i++)
	{
		c_put_str(TERM_YELLOW, quest_text[i], i + 8, 0);
	}
}


/*:::���ۉ��O�w��@�N�G�X�g�p*/
static bool item_tester_hook_kyomaru(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval ==  SV_MATERIAL_HEMATITE) 
		return (TRUE);
	else 
		return (FALSE);
}
/*:::�~�X�����w��@�N�G�X�g�p*/
static bool item_tester_hook_mithril(object_type *o_ptr)
{
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval ==  SV_MATERIAL_MITHRIL) 
		return (TRUE);
	else 
		return (FALSE);
}

/*:::�Ă����V�@�N�G�X�g�p*/
static bool item_tester_hook_eel(object_type *o_ptr)
{
	if (o_ptr->tval == TV_FOOD && o_ptr->sval == SV_FOOD_EEL)
		return (TRUE);
	else
		return (FALSE);
}

/*:::���ʐ��̉H�߁@�N�G�X�g�p*/
static bool item_tester_hook_special_hagoromo(object_type *o_ptr)
{
	if (o_ptr->tval == TV_CLOAK && o_ptr->sval == SV_CLOAK_HAGOROMO && object_is_artifact(o_ptr) && !object_is_cursed(o_ptr))
		return (TRUE);
	else
		return (FALSE);
}

/*:::�������w��@�N�G�X�g�p*/
static bool item_tester_hook_alcohol(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ALCOHOL && o_ptr->sval == SV_ALCOHOL_MARISA) 
		return (FALSE);
	if (o_ptr->tval == TV_ALCOHOL && o_ptr->pval >  6000) 
		return (TRUE);
	else 
		return (FALSE);
}

/*:::�P��܂̓��@�N�G�X�g�p*/
///mod151219 �S���B���������Ƃɋ��t���ǉ�
static bool item_tester_hook_kaguya(object_type *o_ptr)
{
	if(kaguya_quest_level >= KAGUYA_QUEST_MAX)
	{
		if(o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_KINKAKUJI) return TRUE;
		return FALSE;
	}
	else
	{
		if(object_is_fixed_artifact(o_ptr) && o_ptr->name1 == kaguya_quest_art_list[kaguya_quest_level]) return (TRUE);
		return (FALSE);
	}
}

/*:::����������̎��@�N�G�X�g�p*/
static bool item_tester_hook_hangoku(object_type *o_ptr)
{
	if (o_ptr->name1 == ART_HANGOKU_SAKE)
		return (TRUE);
	else
		return (FALSE);
}

/*:::�A�C�e���T���N�G�X�g�̑ΏۃA�C�e���m�F���[�`���B
 *:::�ΏۃA�C�e���������̎��⁚�����Ȃǂ��l�����q_info.txt���g���͖̂ʓ|�Ȃ̂ł��̒��Ƀn�[�h�R�[�f�B���O���Ă��܂��B*/
bool check_quest_deliver_item(int q_index)
{
	cptr q, s;
	char o_name[MAX_NLEN];
	object_type *o_ptr;
	int item;

	if(q_index == QUEST_KYOMARU)
	{
		item_tester_hook = item_tester_hook_kyomaru;
	}
	else if(q_index == QUEST_MITHRIL)
	{
		item_tester_hook = item_tester_hook_mithril;
	}
	else if(q_index == QUEST_HANAMI)
	{
		item_tester_hook = item_tester_hook_alcohol;
	}	
	else if(q_index == QUEST_KAGUYA)
	{
		item_tester_hook = item_tester_hook_kaguya;
	}	
	else if (q_index == QUEST_DELIVER_EEL)
	{
		item_tester_hook = item_tester_hook_eel;
	}
	else if (q_index == QUEST_KONNKATSU)
	{
		item_tester_hook = item_tester_hook_special_hagoromo;
	}
	else if (q_index == QUEST_HANGOKU1)
	{
		item_tester_hook = item_tester_hook_hangoku;
	}

	else
	{
		msg_format("ERROR:check_quest_deliver_item()�ɃN�G�X�gNo.(%d)���w�肳��Ă��Ȃ�",q_index);
		return FALSE;
	}
	q = "�A�C�e����I�����Ă��������B";
	s = "���Ȃ��͋��߂�ꂽ���̂������ė��Ă��Ȃ��B";

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return FALSE;
	o_ptr = &inventory[item];
	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if(o_ptr->number > 1)
	{
		if(!get_check(format("%s�����n���܂����H",o_name))) return FALSE;
	}
	else
	{
		if(!get_check(format("%s��n���܂����H",o_name))) return FALSE;
	}

	inven_item_increase(item, -1);
	inven_item_describe(item);
	inven_item_optimize(item);	

	if(item >= INVEN_RARM)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}

	return TRUE;

}



//���N�U�푈2�N�G�X�g�Ń^�[�Q�b�g��I��
//�I�������^�[�Q�b�g��r_idx��Ԃ��B�L�����Z����0
//castle_quest()�ŃN�G�X�g��̕��͂��o�Ă���Ƃ��Ɏ��s����ǉ��̑I������\������
int	select_quest_yakuza2_target()
{
	char c;
	int i;
	int r_idx;

	bool flag_enoko = TRUE;
	bool flag_biten = TRUE;
	bool flag_chiyari = TRUE;

	//�I�������玩���Ɋ֌W���鐨�͂����O
	//�V����3�l
	if (p_ptr->pclass == CLASS_ENOKO) flag_enoko = FALSE;
	if (p_ptr->pclass == CLASS_BITEN) flag_biten = FALSE;
	if (p_ptr->pclass == CLASS_CHIYARI) flag_chiyari = FALSE;

	//������̏���
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA)  flag_enoko = FALSE;
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU)  flag_biten = FALSE;
	if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU)  flag_chiyari = FALSE;

	//������߈�
	if (p_ptr->muta4 & (MUT4_GHOST_WOLF)) flag_enoko = FALSE;
	if (p_ptr->muta4 & (MUT4_GHOST_OTTER)) flag_biten = FALSE;
	if (p_ptr->muta4 & (MUT4_GHOST_EAGLE)) flag_chiyari = FALSE;

	//���N�U�N�G�X�g1�̂Ƃ��̑I����
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA) flag_enoko = FALSE;
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU) flag_biten = FALSE;
	if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU) flag_chiyari = FALSE;

	//���łɓ|���Ă����珜�O(�ߋ��o�[�W�����œ|���Ă���o�[�W�����A�b�v�����Ƃ�)
	if(r_info[MON_ENOKO].r_akills) flag_enoko = FALSE;
	if (r_info[MON_BITEN].r_akills) flag_biten = FALSE;
	if (r_info[MON_CHIYARI].r_akills) flag_chiyari = FALSE;

	//�����N�G�Ώۂ������珜�O
	//v2.0.13�ȍ~�͂��̎O�l�̓����N�G�I�肳��Ȃ��B�ߋ��o�[�W�����Ń����N�G�I�肳��ē|�����Ƀo�[�W�����A�b�v�����Ƃ��̂��߂̏���
	if (r_info[MON_ENOKO].flags1 & RF1_QUESTOR) flag_enoko = FALSE;
	if (r_info[MON_BITEN].flags1 & RF1_QUESTOR) flag_biten = FALSE;
	if (r_info[MON_CHIYARI].flags1 & RF1_QUESTOR) flag_chiyari = FALSE;

	//�^�[�Q�b�g���c��Ȃ��ꍇ�L�����Z��
	if (!flag_enoko && !flag_biten && !flag_chiyari)
	{
		c_put_str(TERM_WHITE, "���������Ȃ��̎󂯂�ꂻ���Ȏ�z���͎c���Ă��Ȃ������B", 15, 0);
		c_put_str(TERM_WHITE, "���̃N�G�X�g����̂��邱�Ƃ͂ł��Ȃ��悤���B", 16, 0);
		return 0;
	}

	//�c�����^�[�Q�b�g���ꗗ�\��
	c_put_str(TERM_WHITE, "�N��_���܂����H(ESC:�L�����Z��)", 14, 2);
	if (flag_enoko)
		c_put_str(TERM_WHITE, "a) �w�O�� �d�m�q�x  ($50,000+�r�̖͂�)", 15, 5);
	if (flag_biten)
		c_put_str(TERM_WHITE, "b) �w�� ���V�x      ($60,000+�m�\�̖�)", 16, 5);
	if (flag_chiyari)
		c_put_str(TERM_WHITE, "c) �w�V�ΐl �����x($70,000+�ϋv�̖͂�)", 17, 5);

	//�^�[�Q�b�g��I��
	//�^�[�Q�b�g�������鐨�͂��甽���𔃂��t���O���L�^
	while (TRUE)
	{
		r_idx = 0;
		c = inkey();

		if (c == ESCAPE)
		{
			clear_bldg(4, 18);
			return 0;
		}

		if (c == 'a' && flag_enoko)
		{
			r_idx = MON_ENOKO;
			p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q2_BEAT_KEIGA;
		}
		if (c == 'b' && flag_biten)
		{
			r_idx = MON_BITEN;
			p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q2_BEAT_KIKETSU;
		}
		if (c == 'c' && flag_chiyari)
		{
			r_idx = MON_CHIYARI;
			p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_Q2_BEAT_GOUYOKU;
		}

		if (r_idx) break;
	}

	//�I�����������X�^�[�ɏ܋���t���O�𗧂āA���łɓ|���Ă��畜��������
	r_info[r_idx].flags3 |= RF3_WANTED;
	if (!r_info[r_idx].max_num)
	{
		if (cheat_hear) msg_print("�I�����������X�^�[���œ|�ς݂Ȃ̂ŕ���������");
		r_info[r_idx].max_num = 1;
	}

	clear_bldg(4, 18);
	c_put_str(TERM_WHITE, "���Ȃ��͎�z���̈ꖇ����Ɏ�����B", 5, 5);


	return r_idx;

}


/*
 * Request a quest from the Lord.
 */
/*:::�������ł̃N�G�X�g�֘A����*/
static void castle_quest(void)
{
	int             q_index = 0;
	monster_race    *r_ptr;
	quest_type      *q_ptr;
	cptr            name;


	clear_bldg(4, 18);

	/* Current quest of the building */
	/*:::�N�G�X�g�C���f�b�N�X�𓾂�B*/
	q_index = cave[py][px].special;

	//v2.0.13 �������o�Omemo
	//���N�U�푈2�N�G�X�g(QUEST_YAKUZA2)�őI�������܋��񂪃����N�G�Ώۂł��������ꍇ�A
	//�����N�G�t���A�ɍs���ĊY�������X�^�[��|���Ɨ����̃N�G�X�g��������ԂɂȂ�̂����A
	//���̌ツ�N�U�푈2�N�G�X�g�̌����ɗ���ƂȂ���q_index��0�ɂȂ��ĕ�V���������ɍs���Ȃ��B
	//�����N�G�t���A�ȊO�œ|�����炿���Ɗ�������B
	//�����ǂ��Ȃ�����t0000014.txt�ɒ��ł����ꂽ�����ɉe�����o��̂��S���s���B
	//���ɕs�C�������Ƃ肠���������N�G�I��ς݂̓G�͏܋���ɑI���ł��Ȃ��悤�ɂ��Ă���
	if(cheat_xtra)msg_format("idx:%d", q_index);

	//v1.1.85
	// -Mega Hack- �Ǔے��N�G�X�g(�l�ԗp)��[��Ɏ󂯂���Ǔے��N�G�X�g(�d���p)�ɂȂ鏈��
	if (q_index == QUEST_GEIDON_HUMAN && is_midnight()) q_index = QUEST_GEIDON_KWAI;

	/* Is there a quest available at the building? */
	if (!q_index)
	{
#ifdef JP
put_str("�������Ŏ󂯂���N�G�X�g�͂Ȃ��悤���B", 8, 0);
#else
		put_str("I don't have a quest for you at the moment.", 8, 0);
#endif

		return;
	}

	//v1.1.18�ȍ~�A����̃N�G�X�g�͓���푰��N���X�ȊO��e��
	switch (q_index)
	{
	case QUEST_TYPHOON:
	{
		bool flag_ng = TRUE;


		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL)
		{
			//v1.1.85 �V��ɕϐg���Ă���Ǝ󂯂���悤�ɂ��Ă݂�
			switch (p_ptr->metamor_r_idx)
			{
			case MON_KARASU_TENGU:
			case MON_HAKUROU_TENGU:
			case MON_HANA_TENGU:
			case MON_MOMIZI:
			case MON_AYA:
			case MON_TENGU_HAWK:
			case MON_G_TENGU:
				flag_ng = FALSE;

				break;
			default:
				break;

			}

		}
		else
		{
			if (p_ptr->prace == RACE_HAKUROU_TENGU) flag_ng = FALSE;
			if (p_ptr->prace == RACE_KARASU_TENGU) flag_ng = FALSE;
		}


		if (flag_ng)
		{
			put_str("���̃N�G�X�g�͓V�炵���󂯂��Ȃ��悤���B", 8, 0);
			return;
		}
	}
	break;

	case QUEST_MIYOI:
	{
		if (p_ptr->pclass == CLASS_BYAKUREN)
		{
			put_str("���Ȃ��͉����ɂ�肱�̃N�G�X�g�ɎQ���ł��Ȃ��B", 8, 0);
			return;
		}
	}
	break;


	//v1.1.32
	case QUEST_OKINA:
	{
		if (!r_info[MON_MAI].r_akills && !r_info[MON_SATONO].r_akills)
		{
			put_str("���̃N�G�X�g�́u���q�c ���T�v���u����c ���v��|���Ȃ��Ǝ󂯂��Ȃ��悤���B", 8, 0);
			return;
		}
	}
	break;
	//v1.1.65
	case QUEST_KONNKATSU:
	{
		bool flag_ng = TRUE;


		if ((RACE_TENTAN_EFFECT)) flag_ng = FALSE;
		if (p_ptr->mimic_form == MIMIC_MARISA) flag_ng = FALSE;

		//v1.1.85 �l�Ԃɕϐg���Ă���Ǝ󂯂���悤�ɂ��Ă݂�
		if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL && r_info[p_ptr->metamor_r_idx].flags3 & RF3_HUMAN) flag_ng = FALSE;

		if(flag_ng)
		{
			put_str("���̃N�G�X�g�͐l�Ԃ�����ɋ߂��푰�łȂ��Ǝ󂯂��Ȃ��悤���B", 8, 0);
			return;
		}
	}
	break;

	case QUEST_SATORI:
	case QUEST_SHOOTINGSTAR:
	case QUEST_CRYPT:
	{
		if (p_ptr->pclass != CLASS_SATORI && (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH))
		{
			put_str("���Ȃ��͂Ȃ����n��a�̎�Ɗ�����킹��C�ɂȂ�Ȃ��B", 8, 8);
			return;
		}

	}
	break;

	//v1.1.85 �Ǔے��N�G�X�g�͕Е���̂���Ƃ����Е�����̂��邱�Ƃ͂ł��Ȃ��Ȃ�
	case QUEST_GEIDON_HUMAN:
		if (p_ptr->pclass == CLASS_MIYOI)
		{
			put_str("�X��ɂ͉����Y�݂�����悤�����A���Ȃ��ɘb���Ă͂���Ȃ������B", 8, 0);
			put_str("�[��ɂ��q���񂩂畷���Ă݂悤�B", 9, 0);
			return;
		}
		if (quest[QUEST_GEIDON_KWAI].status != QUEST_STATUS_UNTAKEN)
		{
			put_str("�������̎��ԑт̃N�G�X�g���󂯂邱�Ƃ͂ł��Ȃ��B", 8, 0);
			return;
		}
		break;
	case QUEST_GEIDON_KWAI:
		if (quest[QUEST_GEIDON_HUMAN].status != QUEST_STATUS_UNTAKEN)
		{
			put_str("�������̎��ԑт̃N�G�X�g���󂯂邱�Ƃ͂ł��Ȃ��B", 8, 0);
			return;
		}
		break;

	case QUEST_REIMU_ATTACK:

		//�疒�̓A�r���e�B�J�[�h����V�̃N�G�X�g���󂯂��Ȃ��B
		//check_ignoring_quest()���g���ƃA�W�g�����󂵂ă_�[�N�G���t�N�G���󂯂��Ȃ��Ȃ�̂ł����ŏ���
		if (p_ptr->pclass == CLASS_CHIMATA)
		{
			put_str("�������Ŏ󂯂���N�G�X�g�͂Ȃ��悤���B", 8, 0);
			return;
		}
		break;



	}

	q_ptr = &quest[q_index];


	//v1.1.98 �A�����������U�̃N�G�X�g�����łɉ���ɜ߈˂��ꂽ��ԂŎ󂯂�Ɠ���t���O�𗧂Ă�
	if (q_index == QUEST_HANGOKU2 && q_ptr->status == QUEST_STATUS_UNTAKEN && p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
	{
		p_ptr->quest_special_flag |= QUEST_SP_FLAG_HANGOKU2_POSSESSED;
		if (cheat_xtra) msg_print("SPECIAL FLAG ON");

	}

	//151124 v1.0.82b�ł�������quest53�`.txt�����Y��ăA�b�v���[�h�B�N�G�X�g��̂Ɏ��s���Aquest[53]�̓��e����̂܂�QUEST_STATUS_TAKEN�ɂȂ����B
	//������C�����邽�߂ɊY���󋵂ŃN�G�X�g�p�����[�^��ǂݒ����B
	if(q_index == QUEST_KAGUYA && q_ptr->status == QUEST_STATUS_TAKEN && q_ptr->type != QUEST_TYPE_DELIVER_ITEM)
	{
		get_questinfo(q_index, TRUE);
		return;
	}

	/* Quest is completed */
	/*:::�N�G�X�g�I���Ȃ��V�󂯎��҂��ɂ���*/
	if (q_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		q_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index, FALSE);

		/*:::�r��Đ����t���O�H�@�N�G�X�g��V��REWARDED�̏�ԂŒ����Đ������ꂽ�Ƃ��u�����炵��*/
		reinit_wilderness = TRUE;
	}
	/* Failed quest */
	else if (q_ptr->status == QUEST_STATUS_FAILED)
	{
		get_questinfo(q_index, FALSE);

		/* Mark quest as done (but failed) */
		q_ptr->status = QUEST_STATUS_FAILED_DONE;
		if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 40) set_deity_bias(DBIAS_REPUTATION, -3);
		set_deity_bias(DBIAS_COSMOS, -1);
		reinit_wilderness = TRUE;
	}
	/* Quest is still unfinished */
	else if (q_ptr->status == QUEST_STATUS_TAKEN)
	{
		/*:::�A�C�e���T���N�G�X�g*/
		if(q_ptr->type == QUEST_TYPE_DELIVER_ITEM)
		{
			/*:::�A�C�e���������Ă��ēn���΂����ɓ���B�N�G�X�g���������B*/
			if(check_quest_deliver_item(q_index))
			{

				//v1.1.70 �N�G�X�g�B�������𐳊m�ɂ��邽�ߒǉ�
				update_playtime();

				//v1.1.25 �N�G�X�g�B�������ǉ�
				q_ptr->comptime = playtime;

				q_ptr->complev = (byte)p_ptr->lev;
				q_ptr->status = QUEST_STATUS_REWARDED;
				get_questinfo(q_index, FALSE);
				reinit_wilderness = TRUE;

				// Hack - �P��N�G�X�g�B���̏ꍇ�i�s�x��i�߂�(�������܁A�`�[�g�I�v�V��������)
				if(q_index == QUEST_KAGUYA && !(p_ptr->noscore) && p_ptr->pseikaku != SEIKAKU_MUNCHKIN && kaguya_quest_level < KAGUYA_QUEST_MAX) 
					kaguya_quest_level++;
			}
			/*:::�A�C�e�����Ȃ����n���̂�f�����B�N�G�X�g�����m�F�B�����̏ꍇ���s���b�Z�[�W�Ǝ��s�����B*/
			///mod151112 �P��N�G�X�g�̏ꍇ��������Ȃ�
			else if(q_index != QUEST_KAGUYA && get_check_strict("���̃N�G�X�g��������܂����H", CHECK_OKAY_CANCEL))
			{
				q_ptr->complev = (byte)p_ptr->lev;
				q_ptr->status = QUEST_STATUS_FAILED;
				get_questinfo(q_index, FALSE);
				if(p_ptr->prace == RACE_STRAYGOD && p_ptr->race_multipur_val[DBIAS_REPUTATION] > 40) set_deity_bias(DBIAS_REPUTATION, -3);
				set_deity_bias(DBIAS_COSMOS, -1);
				q_ptr->status = QUEST_STATUS_FAILED_DONE;
				reinit_wilderness = TRUE;
			}

		}

		else
		{
			put_str("�����Ŏ󂯂��N�G�X�g���܂��I�������Ă��Ȃ��B", 8, 0);
			put_str("(CTRL-Q���g���΃N�G�X�g�̏�Ԃ��`�F�b�N�ł��܂�)", 9, 0);
		}
	}

	/* No quest yet */
	else if (q_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		q_ptr->status = QUEST_STATUS_TAKEN;
		///sys ������FALSE�ɂ���΃N�G�X�g��̂̎��_�ł͊X�̍ĕ`�悪����Ȃ��Ȃ�Ǝv�����A�������o��̂��낤���B
		reinit_wilderness = TRUE;

		/* Assign a new quest */
		if (q_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if (q_ptr->r_idx == 0)
			{
				/* Random monster at least 5 - 10 levels out of deep */
				q_ptr->r_idx = get_mon_num(q_ptr->level + 4 + randint1(6));
			}

			r_ptr = &r_info[q_ptr->r_idx];

			while ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->rarity != 1))
			{
				q_ptr->r_idx = get_mon_num(q_ptr->level) + 4 + randint1(6);
				r_ptr = &r_info[q_ptr->r_idx];
			}

			if (q_ptr->max_num == 0)
			{
				/* Random monster number */
				if (randint1(10) > 7)
					q_ptr->max_num = 1;
				else
					q_ptr->max_num = randint1(3) + 1;
			}

			q_ptr->cur_num = 0;
			name = (r_name + r_ptr->name);
#ifdef JP
msg_format("�N�G�X�g: %s�� %d�̓|��", name,q_ptr->max_num);
#else
			msg_format("Your quest: kill %d %s", q_ptr->max_num, name);
#endif

		}
		//v2.0.13 ���N�U�푈2�N�G�X�g�̓��ꏈ��
		//�N�G�X�g���͂�\���������Ƃǂ̏܋����_�����I������\�����A�I�΂ꂽ�����X�^�[��quest[].r_idx�ɋL�^����B
		else if (q_index == QUEST_YAKUZA_2)
		{

			int tmp_r_idx;
			get_questinfo(q_index, TRUE);

			tmp_r_idx = select_quest_yakuza2_target();

			if (tmp_r_idx)
			{
				q_ptr->r_idx = tmp_r_idx;
			}
			else
			{
				//�L�����Z�������ꍇ�N�G�X�g�𖢎�̂ɖ߂��Ă܂��N�G�X�g���͂ƑI�������o��悤�ɂ���
				q_ptr->status = QUEST_STATUS_UNTAKEN;
			}
		}
		//�ʏ�̃N�G�X�g�@�N�G�X�g���͕\��
		else
		{
			get_questinfo(q_index, TRUE);
		}


	}
}


/*
 * Display town history
 */
/*:::���̊T�v��\���@�}����*/
static void town_history(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the building help file */
#ifdef JP
(void)show_file(TRUE, "tbldg.txt", NULL, 0, 0);
#else
	(void)show_file(TRUE, "bldg.txt", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}

///mod141116 s16b��s32b�֕ύX
///mod150624 ������������u32b�ɂȂ��Ăĕ��̂Ƃ��I�[�o�[�t���[���Ă��̂�s32b�ɒ�����
s32b calc_expect_crit(int weight, int plus, int dam, s16b meichuu, bool dokubari)
{
	s32b i,k, num;
	int count;
	
	if(dokubari) return dam;

	//v1.1.69 �������Z�ɂ��d�ʃ{�[�i�X
	if (p_ptr->pclass == CLASS_URUMI && p_ptr->tim_general[0])
	{
		int old_weight = weight;
		weight = weight * p_ptr->lev * 8 / 50;
	}

	//v1.1.94 ����N���e�B�J�����㏸�Əd�ʏ��2000�𔽉f
	weight = weight * 2 + p_ptr->lev * 2;
	if (weight > 2000) weight = 2000;


	
	i = weight + (meichuu * 3 + plus * 5) + (p_ptr->lev * 3);
	num = 0;

	//��S�̂Ƃ���1d650��1~650�܂őS���_���[�W�v�Z���đ������킹�čŌ��650�Ŋ��邱�Ƃŉ�S���σ_���[�W���v�Z���Ă���

	//v1.1.94 
	//k��randint1(weight)+randint1(650)�ɂ����̂œ����悤�Ȍv�Z���ł��Ȃ��Ȃ����B
	//randint1(weight)��1����ő�܂�10%�����₵�đS�����v���邩�B���S�Ȑ����ł͂Ȃ��Ȃ邪����قǊO��Ă͂��Ȃ��Ǝv��
	for (count = 0; count <= 10; count++)
	{
		k = (weight+9)/10*count;
		if (k < 400)						num += (2 * dam + 5) * (400 - k);
		if (k < 700)						num += (2 * dam + 10) * (MIN(700, k + 650) - MAX(400, k));
		if (k >(700 - 650) && k < 900)		num += (3 * dam + 15) * (MIN(900, k + 650) - MAX(700, k));
		if (k >(900 - 650) && k < 1300)		num += (3 * dam + 20) * (MIN(1300, k + 650) - MAX(900, k));
		if (k >(1300 - 650))					num += (7 * dam / 2 + 25) * MIN(650, k - (1300 - 650));
	}
	num /= 650*11;

	/*
	k = weight;
	if (k < 400)						num += (2 * dam + 5) * (400 - k);
	if (k < 700)						num += (2 * dam + 10) * (MIN(700, k + 650) - MAX(400, k));
	if (k > (700 - 650) && k < 900)		num += (3 * dam + 15) * (MIN(900, k + 650) - MAX(700, k));
	if (k > (900 - 650) && k < 1300)		num += (3 * dam + 20) * (MIN(1300, k + 650) - MAX(900, k));
	if (k > (1300 - 650))					num += (7 * dam / 2 + 25) * MIN(650, k - (1300 - 650));
	num /= 650;

	*/

	if(p_ptr->pclass == CLASS_NINJA)
	{
		num *= i;
		num += (4444 - i) * dam;
		num /= 4444;
	}
	else
	{
		num *= i;
		num += (5000 - i) * dam;
		num /= 5000;
	}
	
	return num;
}

/*:::���폠�̕����r�T�u���[�`��4�@�X���C�v�Z*/
static s16b calc_slaydam(int dam, int mult, int div, bool force)
{
	int tmp;
	if(force)
	{
		tmp = dam * 60;
		tmp *= mult * 3;
		tmp /= div * 2;
		tmp += dam * 60 * 2;
		tmp /= 60;
	}
	else
	{
		tmp = dam * 60;
		tmp *= mult; 
		tmp /= div;
		tmp /= 60;
	}
	return tmp;
}

/*
 * Display the damage figure of an object
 * (used by compare_weapon_aux1)
 *
 * Only accurate for the current weapon, because it includes
 * the current +dam of the player.
 */
/*:::���폠�̕����r�T�u���[�`��3 �_���[�W�\��*/
static void compare_weapon_aux2(int r, int c, int mindam, int maxdam, cptr attr, byte color)
{
	char tmp_str[80];


	/* Print the intro text */
	c_put_str(color, attr, r, c);

	/* Calculate the min and max damage figures */
#ifdef JP
	sprintf(tmp_str, "�P�^�[��: %d-%d �_���[�W",
#else
	sprintf(tmp_str, "Attack: %d-%d damage",
#endif
			mindam, maxdam);
	
	/* Print the damage */
	put_str(tmp_str, r, c + 8);
}


/*
 * Show the damage figures for the various monster types
 *
 * Only accurate for the current weapon, because it includes
 * the current number of blows for the player.
 */
/*:::���폠�̕����r�̃T�u���[�`��2 �X���C���ݍU���͕\��*/
static void compare_weapon_aux1(object_type *o_ptr, int col, int r)
{
	u32b flgs[TR_FLAG_SIZE];
	int blow = p_ptr->num_blow[0];
	bool force = FALSE;
	bool dokubari = FALSE;
	
	/* Effective dices */
	int eff_dd = o_ptr->dd + p_ptr->to_dd[0];
	int eff_ds = o_ptr->ds + p_ptr->to_ds[0];
	
	int mindice = eff_dd;
	int maxdice = eff_ds * eff_dd;
	int mindam = 0;
	int maxdam = 0;
	int slaydice_min = 0;
	int slaydice_max = 0;
	int critdice_min = 0;
	int critdice_max = 0;
	int vorpal_mult = 1;
	int vorpal_div = 1;

	

	/* Get the flags of the weapon */
	object_flags(o_ptr, flgs);
	
	if((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) dokubari = TRUE;
	
	
	/* Show Critical Damage*/
	critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, mindice, p_ptr->to_h[0], dokubari);
	critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, maxdice, p_ptr->to_h[0], dokubari);
	
	mindam = blow * (critdice_min+ o_ptr->to_d + p_ptr->to_d[0]);
	maxdam = blow * (critdice_max+ o_ptr->to_d + p_ptr->to_d[0]);
	if(mindam<0) mindam=0; 
	if(maxdam<0) maxdam=0;
	
#ifdef JP
	compare_weapon_aux2(r++, col, mindam, maxdam, "��S:", TERM_L_RED);
#else
	compare_weapon_aux2(r++, col, mindam, maxdam, "Critical:", TERM_L_RED);
#endif
	
	/* Vorpal Hit*/
	///mod151101 *�؂ꖡ*�̂݃t���O�����Ă����Ƃ�(�b��Ȃ�)�ɑΏۂƂȂ�悤�������ǉ�
	if ((have_flag(flgs, TR_VORPAL) || have_flag(flgs, TR_EX_VORPAL) || hex_spelling(HEX_RUNESWORD)))
	{
		if(have_flag(flgs, TR_EX_VORPAL))
		{
			vorpal_mult = 5;
			vorpal_div = 3;
		}
		else
		{
			vorpal_mult = 11;
			vorpal_div = 9;
		}
		
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, mindice, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, maxdice, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		
		#ifdef JP
			compare_weapon_aux2(r++, col, mindam, maxdam, "�؂ꖡ:", TERM_L_RED);
		#else
			compare_weapon_aux2(r++, col, mindam, maxdam, "Vorpal:", TERM_L_RED);
		#endif
	}	
	
	if ((p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && have_flag(flgs, TR_FORCE_WEAPON) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
	{
		force = TRUE;
		
		slaydice_min = calc_slaydam(mindice, 1, 1, force);
		slaydice_max = calc_slaydam(maxdice, 1, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		
		/* Print the relevant lines */
#ifdef JP
		compare_weapon_aux2(r++, col, mindam, maxdam, "����:", TERM_L_BLUE);
#else
		compare_weapon_aux2(r++, col, mindam, maxdam, "Force  :", TERM_L_BLUE);
#endif		
	}
		
	/* Print the relevant lines */
	if (have_flag(flgs, TR_KILL_EVIL))
	{	
		slaydice_min = calc_slaydam(mindice, 7, 2, force);
		slaydice_max = calc_slaydam(maxdice, 7, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "���א���:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_EVIL))
	{	
		slaydice_min = calc_slaydam(mindice, 2, 1, force);
		slaydice_max = calc_slaydam(maxdice, 2, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "���א���:", TERM_YELLOW);
	}

	if (have_flag(flgs, TR_KILL_GOOD))
	{
		slaydice_min = calc_slaydam(mindice, 7, 2, force);
		slaydice_max = calc_slaydam(maxdice, 7, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "��������:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_GOOD))
	{
		slaydice_min = calc_slaydam(mindice, 2, 1, force);
		slaydice_max = calc_slaydam(maxdice, 2, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "��������:", TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_HUMAN))
	{	
		slaydice_min = calc_slaydam(mindice, 4, 1, force);
		slaydice_max = calc_slaydam(maxdice, 4, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�l��:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_HUMAN))
	{	
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�l��:", TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_ANIMAL))
	{
		slaydice_min = calc_slaydam(mindice, 4, 1, force);
		slaydice_max = calc_slaydam(maxdice, 4, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "����:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_ANIMAL)) 
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "����:", TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_UNDEAD))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�s��:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_UNDEAD)) 
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�s��:", TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_DEMON))
	{	
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "����:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_DEMON))
	{	
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "����:", TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_DEITY))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�_�i:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_DEITY))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�_�i:", TERM_YELLOW);
	}
	if (have_flag(flgs, TR_KILL_KWAI))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�d��:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_KWAI))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�d��:", TERM_YELLOW);
	}

	if (have_flag(flgs, TR_KILL_DRAGON))
	{
		slaydice_min = calc_slaydam(mindice, 5, 1, force);
		slaydice_max = calc_slaydam(maxdice, 5, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "��:", TERM_YELLOW);
	}
	else if (have_flag(flgs, TR_SLAY_DRAGON))
	{
		slaydice_min = calc_slaydam(mindice, 3, 1, force);
		slaydice_max = calc_slaydam(maxdice, 3, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "��:", TERM_YELLOW);
	}
	if (o_ptr->name1 == ART_KRONOS)
	{
		slaydice_min = calc_slaydam(mindice, 4, 1, force);
		slaydice_max = calc_slaydam(maxdice, 4, 1, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�j:", TERM_YELLOW);
	}


	if (have_flag(flgs, TR_BRAND_ACID))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�_����:", TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_ELEC))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�d����:", TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_FIRE))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "������:", TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_COLD))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�⑮��:", TERM_RED);
	}
	if (have_flag(flgs, TR_BRAND_POIS))
	{
		slaydice_min = calc_slaydam(mindice, 5, 2, force);
		slaydice_max = calc_slaydam(maxdice, 5, 2, force);
		critdice_min = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_min, p_ptr->to_h[0], dokubari);
		critdice_max = calc_expect_crit(o_ptr->weight, o_ptr->to_h, slaydice_max, p_ptr->to_h[0], dokubari);
		mindam = blow * (calc_slaydam(critdice_min, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		maxdam = blow * (calc_slaydam(critdice_max, vorpal_mult, vorpal_div, FALSE) + o_ptr->to_d + p_ptr->to_d[0]);
		if(mindam<0) mindam=0; 
		if(maxdam<0) maxdam=0;
		compare_weapon_aux2(r++, col, mindam, maxdam, "�ő���:", TERM_RED);
	}

	
}

static int hit_chance(int to_h, int ac)
{
	int chance = 0;
	int meichuu = p_ptr->skill_thn + (p_ptr->to_h[0] + to_h) * BTH_PLUS_ADJ;

	if (meichuu <= 0) return 5;

	chance = 100 - ((ac * 75) / meichuu);

	if (chance > 95) chance = 95;
	if (chance < 5) chance = 5;
//	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)	chance = (chance*19+9)/20;
	return chance;
}



/*::: -Mega Hack- ��������̃N���X��푰�̎��A����̃N�G�X�g�̕��͂�s���R�ɂȂ�Ȃ��悤�ɕύX����B*/
bool check_quest_unique_text(void)
{
	int qnum = p_ptr->inside_quest;
	bool comp = FALSE;
	bool fail = FALSE;
	bool accept = FALSE;
	int pc = p_ptr->pclass;
	int pr = p_ptr->prace;
	int line = 0;

	if(quest[qnum].status == QUEST_STATUS_FAILED) fail = TRUE;
	else if(quest[qnum].status == QUEST_STATUS_REWARDED) comp = TRUE;
	else accept = TRUE;

	switch(qnum)
	{
		//�����̃A�W�g
	case QUEST_HOME1:
		if(pc == CLASS_BANKI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�����u����M���́E�E���x�ǂ��Ƃ���ɗ��Ă���܂����B");
				strcpy(quest_text[line++], "�O��̋󂫉Ƃ������m�ł���ˁH");
				strcpy(quest_text[line++], "�����������o���̂Ȃ��Ȃ炸�҂̗��܂��ɂȂ��Ă���̂ł��B");
				strcpy(quest_text[line++], "�ނ�͗��œ��݂𓭂��Ă���悤�ł��B�厖�ɂȂ�O�ɒǂ��o���ĉ������B");
				strcpy(quest_text[line++], "���ʂ̐l�Ԃ����ł��B�M���Ȃ���Ȃ��ł��傤�E�E�v");
			}

		}
		else if(pc == CLASS_SEIJA)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�����u�c�c�O��̋󂫉Ƃɑ�؂ȋq�l�����������Ă��܂��B");
				strcpy(quest_text[line++], "�����ł����A��΂Ɍ����点�����Ă͂��߂ł���H�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�����u���炠��܂��܂��A�Ȃ�ė��\�Ȃ��Ƃ��Ȃ���̂ł��傤�B");
				strcpy(quest_text[line++], "�ǂ�����x�Ƃ��̉Ƃɋߊ��Ȃ��ł��������B�v");
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�����u腖��l�I�H���̂��p�ł��傤���A�܂����E�E");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "���������A���͋x�ɂňٕϒ������Ȃ̂ł����B����͐��ɓV�̏����ł��B");
				strcpy(quest_text[line++], "���͊O�E�̂Ȃ炸�҂��O��̋󂫉Ƃ�����ɓ��݂𓭂��Ă���̂ł��B");
				strcpy(quest_text[line++], "�ǂ��������Ē����܂��񂩁H�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�����u�L��������܂����B");
				strcpy(quest_text[line++], "�������ŋߗ��̎��肪�����ŊĎ��̖ڂ�����Ă��Ȃ��̂ł��B");
				strcpy(quest_text[line++], "�ł���Έٕς̊Ԃ����ł����̋󂫉Ƃ��Ǘ����Ē����܂��񂩁H");
				strcpy(quest_text[line++], "���u�Ƃ��Ăł��g���Ă�������Ώ\���ł��̂ŁB�v");
			}
		}
		//v1.1.37
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�����u���珬�邶��Ȃ��B������ו������Ǒݖ{����̋A��Ȃ́H");
				strcpy(quest_text[line++], "�Ƃ���ŁA�O��̋󂫉ƂɂȂ炸�҂��Z�ݒ������ƕ񍐂���������B");
				strcpy(quest_text[line++], "�Ώ����I���܂Ő�΂ɋߕt���Ȃ��悤�ɂˁB�����H�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�����u���邪�ގ������c�ł����āH");
				strcpy(quest_text[line++], "�������������炢���悤�Ȃ��̂́A����Ȃɖ������Ȃ��Ă����̂ɁB");
				strcpy(quest_text[line++], "���̋󂫉ƁH�������肪���܂�܂őq�ɂɂł��g���Ă������B");
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�����u���炱��ȏ��ɗd�����B");
				strcpy(quest_text[line++], "�����q������O�ŗV��łȂ����B");
				strcpy(quest_text[line++], "�O��̋󂫉Ƃɂ͕|���l�����邩��ߊ������ʖڂ�B�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�����u���̂Ȃ炸�҂�ǂ��o�����ł����āH��������ˁB");
				strcpy(quest_text[line++], "���J���ɂ��̋󂫉Ƃ����΂炭�݂��Ă�����B");
				strcpy(quest_text[line++], "�閧��n�ɂł��g���Ȃ����B�v");
			}
		}

		break;
		//�I�[�N�̃L�����v
	case QUEST_ORC:
		if(pc == CLASS_KEINE)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "�I�[�N�ƌĂ΂�鋥�\�ȏb�l���������Đ��̓��A�ɏW�܂��Ă���B");
				strcpy(quest_text[line++], "���u���Ă����Ɨ�����������邩������Ȃ��B���̂����ɒ@���Ă������B");
				strcpy(quest_text[line++], "�I�[�N��̈�̂͑債�ċ����Ȃ��悤�������������A");
				strcpy(quest_text[line++], "����ɗ͋������[�_�[�i�̃I�[�N������B���f�͋֕����B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�I�[�N�B�͎U��U��ɓ��������Ă������B�����͋ߊ���Ă��Ȃ����낤�B");
				strcpy(quest_text[line++], "�B�c�Ƃ̎҂�����̕i��͂��Ă��ꂽ�B");
				strcpy(quest_text[line++], "������B�ꖪ���B�����Z���Γ��A�T�������邾�낤�B");
			}
			else
			{
				strcpy(quest_text[line++], "�I�[�N�B�͎v�����ȏ�ɋ����A�s�o������Ă��܂����B");
				strcpy(quest_text[line++], "�K���Ȏ��ɁA���ɗ��Ă������g���b�𕷂��ăI�[�N��S��������΂��Ă��ꂽ�B");
			}

		}
		else if(pc == CLASS_EIKI)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "�d���u���ƁA腖��l�ł͂Ȃ��ł����B����ٕ̈ς̒������Ȃ̂ł��ˁB");
				strcpy(quest_text[line++], "���͈يE�̋��\�ȏb�l�炵���A�����߂��̓��A�ɐ��ݒ����Ă���܂��B");
				strcpy(quest_text[line++], "��낵����Αގ��ɗ͂�݂��Ă��������܂��񂩁H�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�d���u���͓Y�����肪�Ƃ��������܂����B");
				strcpy(quest_text[line++], "���������̒����ɂ��𗧂Ă��������B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�d���u�E�E腖��l�ɗ��炸�Ɨ͂ŉ΂̕��𕥂��Ƃ������S�Ȃ̂ł��ˁB");
				strcpy(quest_text[line++], "�����蓢������g�D�������܂��B�v");
			}

		}
		else if(pc == CLASS_KOSUZU)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "�d���u���⏬�邩�B���X�����i�D�łǂ������H");
				strcpy(quest_text[line++], "�����A�����Ă������Ƃ��������B");
				strcpy(quest_text[line++], "���ܗ��̋߂��ɃI�[�N�Ƃ������\�Ȉ��l�����W�܂��Ă���񂾁B");
				strcpy(quest_text[line++], "�댯�������q�Ȃ��ł͗����痣��Ȃ��悤�ɂȁB�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�d���u���A�I�[�N��S�Ēǂ��U�炵�Ă������ƁH");
				strcpy(quest_text[line++], "�������ȁB���̊Ԃɂ����܂ł̎g����ɂȂ����̂��B");
				strcpy(quest_text[line++], "�Ƃ������Ƃ��Ă��̉B�ꖪ���󂯎���Ă���B");
			}
			else
			{
				strcpy(quest_text[line++], "�d���u�悵�悵�A����͂Ȃ����H");
				strcpy(quest_text[line++], "�|����Ȃ����ɖ��S�C�ȂƂ���͐̂���ς��Ȃ��ȁB�v");
			}

		}
		else if(pc == CLASS_MOKOU)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "���q���ɍs���ƌd���ɗ��ݎ������ꂽ�B");
				strcpy(quest_text[line++], "�I�[�N�Ƃ��������\�Ȉ��l���߂��̓��A�ɓԂ��Ă���炵���B");
				strcpy(quest_text[line++], "�ŋߑ̂��݂��Ă��邵�y���^��������̂��������낤�B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�I�[�N��ǂ������Ď��q���ɕ񍐂ɖ߂�ƁA�d������̕i�����ꂽ�B");
				strcpy(quest_text[line++], "�G����g���B���B�ꖪ�炵���B");
				strcpy(quest_text[line++], "�ʂɉB���K�v���Ȃ��̂����A�d���Ȃ�̗D�����Ȃ̂��낤���B");
			}
			else
			{
				strcpy(quest_text[line++], "�d���u�M�����������قǂ̓G�ɂ͂ƂĂ������܂���ł������B");
				strcpy(quest_text[line++], "�܂����̒��ł��������̂ł����H�v");
			}

		}
		else if(pc == CLASS_REIMU)
		{
			if(fail)
			{
				strcpy(quest_text[line++], "�d���u����̛ޏ������ĂȂ��悤�ȑ���ɂ͌����Ȃ��������B");
				strcpy(quest_text[line++], "���������Ă͈������A���������ł���̂ł͂Ȃ����H�v");
			}

		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�d���u�d�����B���̓��A�ɂ͋ߊ��Ȃ������������B");
				strcpy(quest_text[line++], "���\�Ȉ��l������ɂ��Ă��Ċ댯�Ȃ񂾁B�߁X��������g�D���悤�ƍl���Ă���B");
				strcpy(quest_text[line++], "����܂ł��̕ӂł͗V�ԂȂ�B�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�d���u���A�I�[�N��S�Ēǂ��U�炵�Ă������ƁH");
				strcpy(quest_text[line++], "�ŋ߂͗d���Ƃ����ǂ��n���ɂł��Ȃ��ȁB");
				strcpy(quest_text[line++], "����ɂ���������悤�B���ň��Y�Ɏg���񂶂�Ȃ����H�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�d���u���猾��Ȃ����Ƃ���Ȃ��B");
				strcpy(quest_text[line++], "����͂Ȃ����H�v");
			}
		}

		break;

		//���O���X�g��+���@���A��V��A���T�V��
	case QUEST_HOME2:
		if(pc == CLASS_MOMIZI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���u�ӂ��ށA��������ς܂��Ă��܂��܂������B");
				strcpy(quest_text[line++], "�@���͂�����Ɗ��E�E���������S�z���Ă܂������]�v�������悤�ł��ˁB");
				strcpy(quest_text[line++], "�@���̋󂫉Ƃ݂͂�ȖY��邱�Ƃɂ����悤�ł����A���u�ɂł��g���΂ǂ��ł��H");
				strcpy(quest_text[line++], "�@���̒��x�͖𓾂��Ă��̂ł���B");

			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���u���犑���̐�x�x�͂ǂ�������ł��H");
				strcpy(quest_text[line++], "�@�͂͂��A��̋󂫉Ƃ̗]���҂̌��ł��ˁH");
				strcpy(quest_text[line++], "�@�������񂾌����Ă����̌��͔��T�V��̊Ǌ��ł�����˂��B");
				strcpy(quest_text[line++], "�@�������Ǝn�����Ė�������}�낤�Ɩ������󂯂����ĂƂ���ł����B");
				strcpy(quest_text[line++], "�@�E�E�����ɂ܂Ȃ��ŉ�������B�����^�����F�肵�܂��B");
				strcpy(quest_text[line++], "�@���s���Ă��X�N�[�v�����肵�Ȃ���������Ȃ����炲���S���������ȁB�v");
			}
		}
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���u���₨�⏬�邳�񂶂�Ȃ��ł����B");
				strcpy(quest_text[line++], "����\���グ�܂������A�����ł͗�ވ��̏��邳��Ƃ��Ă͈����܂����B");
				strcpy(quest_text[line++], "������Ɨ��݂�����̂ł����A�O��̋󂫉Ƃ�K�˂Ă���܂��񂩁H");
				strcpy(quest_text[line++], "�܂��ʓ|�Ȏ葱���̈�Ƃ������A��X�ȊO�̎肪�K�v�Ȃ̂ł���B");
			}
			if(comp)
			{
				strcpy(quest_text[line++], "���u����͂�����܂����ˁB");
				strcpy(quest_text[line++], "�����ɂȂ����Ƃ���𓥂ݍ��ނ���ł������A");
				strcpy(quest_text[line++], "�܂����M�����S���|���Ă��܂��Ƃ́B");
				strcpy(quest_text[line++], "�����A���̉Ƃ͍����グ�܂��傤�B���R�Ɏg���Ă��������B");
				strcpy(quest_text[line++], "������l���𓝂ׂ邩������Ȃ��l�Ԃɂ͂��ꂭ�炢������O�ł���c�v");
			}
		}
		else if(pc == CLASS_SEIJA)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "����悭�Ƃ�D���Ă�����B�]���҂��V��̘A���������C�����B");

			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "�]���҂��󂫉Ƃɐ��ݒ����Ė��Ȃ�����݂Ŏ肪�o���Ȃ��炵���B");
				strcpy(quest_text[line++], "���������������̗]���҂ɂȂ�����Ă��̉Ƃ�D���Ă�낤�B");
			}
		}
		else if(pc == CLASS_YUGI || pc == CLASS_SUIKA)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�@�߂��Ă���ƕ��͏����Ă����B");
				strcpy(quest_text[line++], "�@�Ƃ肠�������̋󂫉Ƃ͋��_�Ƃ��Đڎ����邱�Ƃɂ����B");

			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���u����E�E�ǁA�ǂ����B");
				strcpy(quest_text[line++], "�@�������������H������������Ȗő����Ȃ��B");
				strcpy(quest_text[line++], "�@���R�͎����ĕ��a�ł������܂���B");
				strcpy(quest_text[line++], "�@�������]���҂��󂫉Ƃ�苒������A");
				strcpy(quest_text[line++], "�@�����炻�����̂��ŐӔC�����t����������Ȃ�āA");
				strcpy(quest_text[line++], "�@���[�񂺂񂵂Ă���܂���B�v");
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�@�߂��Ă���Ǝ˖��ۂ͏����Ă����B");
				strcpy(quest_text[line++], "�@�Ƃ肠�������̋󂫉Ƃ͋��_�Ƃ��Đڎ����邱�Ƃɂ����B");

			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���u����͂���́A腖��l�ł͂���܂��񂩁E�E");
				strcpy(quest_text[line++], "�@�������������H������������Ȗő����Ȃ��B");
				strcpy(quest_text[line++], "�@���R�͎����ĕ��a�ł������܂���B");
				strcpy(quest_text[line++], "�@�������]���҂���̋󂫉Ƃ�苒������A");
				strcpy(quest_text[line++], "�@�����炻�����̂��ŐӔC�����t����������Ȃ�āA");
				strcpy(quest_text[line++], "�@���[�񂺂񂵂Ă���܂���B�v");
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���u�Ȃ�ƁA�������낤�ɗd���ɏo���������Ƃ́B");
				strcpy(quest_text[line++], "����͂�V��̈Ќ����������������񂶂Ⴀ��܂���˂��B");
				strcpy(quest_text[line++], "���̉Ƃ͓����D���Ɏg���Ă����ł���B");
				strcpy(quest_text[line++], "�󂫉Ƃ̂܂܂ɂ��Ă������͊�炩�܂��ł��傤�B�v");

			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���u�����A�����͋M���̂悤�ȗd���ɂ͊댯�ł���B");
				strcpy(quest_text[line++], "���ɍ��͗��ɂ�����Ƃ����ʓ|�����N�����Ă���̂ł�����B");
				strcpy(quest_text[line++], "�߂܂��ĕ|���ڂɑ����O�ɉƂɋA��Ȃ����B�v");
			}
		}

		else if(p_ptr->prace == RACE_KARASU_TENGU || p_ptr->prace == RACE_HAKUROU_TENGU)
		{
			//v2.0.5 �����u�͂��ẲƁv��ǉ������̂ł͂��Ă̂Ƃ��Z���t��ς��Ă���
			if (p_ptr->pclass == CLASS_HATATE)
			{
				if (comp)
				{
					strcpy(quest_text[line++], "�N���҂ɓˌ���ނ����s���A���̉Ƃ����߂����B");
					strcpy(quest_text[line++], "���̉�������炪�ڂɕ����Ԃ悤���B");
				}
				else if (fail)
				{
					msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
				}
				else
				{
					strcpy(quest_text[line++], "���΂炭����ɂ��Ă��镶�̉Ƃɗ]���҂��Z�ݒ����Ă����B");
					strcpy(quest_text[line++], "�]���҂͖�Ȗ�ȉ������V�����J��Ԃ��A�ߏ��ɔ�Q���o�Ă���悤���B");
					strcpy(quest_text[line++], "����͖ʔ����Ȃ��Ă����B");
				}

			}
			else
			{
				if (comp)
				{
					strcpy(quest_text[line++], "����ǂ��N���҂�Еt���A�䂪�Ƃ����߂����B");
					strcpy(quest_text[line++], "�낤���S�V�b�v�̃l�^�ɂȂ�Ƃ��낾�������A�Ƃ肠�����ʎq�͕ۂ����悤���B");

				}
				else if (fail)
				{
					msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
				}
				else
				{
					strcpy(quest_text[line++], "���΂炭����ɂ��Ă��邤���ɉƂɗ]���҂��Z�ݒ����Ă����B");
					strcpy(quest_text[line++], "�]���҂͖�Ȗ�ȉ������V�����J��Ԃ��A�ߏ��ɔ�Q���o�Ă���悤���B");
					strcpy(quest_text[line++], "���₩�Ɉ�l�Ŏn�������Ȃ��ƓV��̌ւ�Ɋւ��B");
				}


			}



		}
		break;

	case QUEST_HOME3: //�H�쉮�~���@���Ƃ�
		if(pc == CLASS_SATORI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���o���̂Ȃ��A�����������A�Ƃ������S�Ẳ���𒦂炵�߂��B");
				strcpy(quest_text[line++], "�󂫉Ƃ̂܂܂ł͂܂����Ȃ̂��o�邩������Ȃ��B");//��s40���܂�
				strcpy(quest_text[line++], "���u�ɂł��g�킹�Ă��炨���B�\�����Ă͒N���������͏o�Ȃ������B");//��s40���܂�
			}
			else if(accept)
			{
				strcpy(quest_text[line++], "�Ƃׂ̗̋󂫉ƂɊ�ȉ��삪�Ԃ��Ă���炵���B");
				strcpy(quest_text[line++], "����̊Ǘ��͎����̖������B");
				strcpy(quest_text[line++], "�l�q�����ɍs���Ă݂悤�B");
			}

		}
		else if(pc == CLASS_YUGI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "����ł��Ėڏ�肾�����̂őS���@���ׂ����B");
				strcpy(quest_text[line++], "�ߏ��̎҂���󂫉Ƃ̊Ǘ��𗊂ݍ��܂ꂽ�B");
				strcpy(quest_text[line++], "���u����Ɏg���Ă�낤�B");
			}
			else if(accept)
			{
				strcpy(quest_text[line++], "�n��a�ׂ̗̋󂫉ƂɊ�ȉ��삪�Ԃ��Ă���炵���B");
				strcpy(quest_text[line++], "�܊p�����猩���ɍs���Ă݂悤�B");
			}

		}
		break;

	case QUEST_KAPPA:	//���ᏋC�@�d���E�H�X�q
		///mod151001 �N�G�X�g���e�ύX
		if(pc == CLASS_YOUMU)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�����͉͓������̑��A�ɂȂ��Ă����炵���B�Ƃ肠�����S�ē|�����B");
				strcpy(quest_text[line++], "�ꑧ�����Ƃ��A���ʘO���o��ۂɎ傪���ȓ����a���Ă��ꂽ���Ƃ��v���o�����B");//��s40���܂�
				strcpy(quest_text[line++], "�ǂ���炱����g���Ύ��͂�S�Ēm�o�ł���炵���B");//��s40���܂�
				strcpy(quest_text[line++], "��̐S�����𖳑ʂɂ��Ă��܂����悤���B���߂č���̒T���ɖ𗧂Ă悤�B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�p���ɐ���ł����ҒB�́A���Ȃ��̎�ɂ͕����Ȃ������B");//��s40���܂�
				strcpy(quest_text[line++], "�傩��a������������̂��Ƃ��v���o�������A");//��s40���܂�
				strcpy(quest_text[line++], "�ǂ����ǂ������ɖ������Ă��܂����炵���E�E�B");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "���J���@�X�ɗH�삪���ӂ�Ă���Ɩ�����������������B");
				strcpy(quest_text[line++], "�H�삽���ɘb�𕷂��Ă݂�ƁA�ߏ��̔p���u��ؑ��v�ɏZ��ł����炵���B");
				strcpy(quest_text[line++], "���҂�����U�����󂯂ē����o���Ă����������B");
				strcpy(quest_text[line++], "�l�q�����ɍs���Ă݂悤�B");
			}


		}
		else if(pc == CLASS_YUYUKO)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�d���u�����Ȃ�����݂ł����B�Ƃ��������͉��̂��߂ɁE�E");
				strcpy(quest_text[line++], "���A���ꂨ�Ԃ����܂��ˁB�v");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�d���u�����A�ɂ������̂ł��ˁB");//��s40���܂�
				strcpy(quest_text[line++], "����ł͎����s���Ă��܂��B�v");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "�d���u���A�H�X�q�l�B�����A��ʂ�ł����B");
				strcpy(quest_text[line++], "���̔p���ɉ������ȕ����o�����ėH�삽����ǂ��o���Ă��܂��������ł��B");
				strcpy(quest_text[line++], "���H�H�X�q�l�����ɍs���Ă݂�̂ł����H�v");
			}


		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�d���u���ƁA�����͉͓������̑q�ɂ������̂ł����B");
				strcpy(quest_text[line++], "����Ȏ��ׂ̈ɐS���b���Ă����̂ɁA�܂��܂��C�s������܂���c");
				strcpy(quest_text[line++], "���A���������ΗH�X�q�l��������������Ă���Ă�����ł��B");
				strcpy(quest_text[line++], "�ǂ��������Ȃ̂��悭������Ȃ������̂ł����A�����腖��l�ɍ����グ�܂��ˁB�v");
			}
			else if(accept)
			{
				strcpy(quest_text[line++], "�d���u���̋߂��̋󂫉ƂɏZ��ł���H�삽�����A");
				strcpy(quest_text[line++], "���Ȃ��̂ɋ�������Ă���炵���̂ł��B");
				strcpy(quest_text[line++], "�����������s���Ă݂���ł����ʂɉ������Ȃ��݂�����");
				strcpy(quest_text[line++], "�ł��m���ɖ��ȋC�z�͂����ł��B腖��l�͉����킩��܂����H�v");
			}


		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�d���u�����̔p���ɋߊ���Ă͑ʖڂł���B���Ȃ��̂�����ł���悤�Ȃ̂ł��B�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�d���u���A�͓����Ԃ��Ă����́H�Ƃ��������Ȃ����S���|����������́H");
				strcpy(quest_text[line++], "�������ȁB����͂���ŗH�X�q�l�ɉ��ƕ񍐂��Ă������̂��E�E");
				strcpy(quest_text[line++], "�Ƃ肠��������������܂��ˁB�v");
			}
		}
		break;
		//�~�~�b�N�N�G
	case QUEST_MIMIC:

		break;

		//�f�X�\�[�h
	case QUEST_DEATHSWORD:
		break;

		//������ 
	case QUEST_SEWER:
		if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�����u�ŋ߉������̗l�q�����������́B");
				strcpy(quest_text[line++], "�Ђ���Ƃ���Ɨd�������������݂��Ă���̂�������Ȃ���B");
				strcpy(quest_text[line++], "�O�݂����ɖ�����������ʖڂ�B�{���ɂǂ��Ȃ��Ă��m��Ȃ�����ˁB�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�����u�����猾�����ł��傤�B");
				strcpy(quest_text[line++], "����ɒ��肽�班���͂��ƂȂ������Ă��邱�ƂˁB");
				strcpy(quest_text[line++], "�厖�Ɏ���Ȃ��Ă悩������B");
			}
			else
			{
				strcpy(quest_text[line++], "�����u���҂��̑像�j���̂�|���Ă����c�ł����āH");
				strcpy(quest_text[line++], "�M���A�{���ɐl�̓��𓥂݊O�������Ă�񂶂�Ȃ��́H�v");
			}
		}
		else if (pr == RACE_FAIRY)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�����u�����̌��H����͉������ɒʂ���l�E��B");
				strcpy(quest_text[line++], "�ŋ߉��������ݒ����Ă�݂����ŋ߁X��������\���B");
				strcpy(quest_text[line++], "��Ȃ��������Ȃ��悤�ɂˁB�v");
			}
			else if (fail)
			{
				strcpy(quest_text[line++], "�����u�����A�����܂łЂǂ����ƂɂȂ��Ă����̂ˁB");
				strcpy(quest_text[line++], "�ŋߖ{���ɕ����Ȃ񂾂���C�����Ȃ����B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�����u���҂��̑像�j���̂�|���Ă����c�ł����āH");
				strcpy(quest_text[line++], "�M���A�d������d���ɂȂ肩���ĂȂ��H�v");
			}
		}
		break;

		//�j��1�@�`���m
	case QUEST_DOOM1: 
		if(pc == CLASS_CIRNO)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�������v���Ă����z�ƈ�������A�Ƃ������S���|�����B");
				strcpy(quest_text[line++], "���ꂢ�Ȏw�ւ������Ă���z�������̂Ő험�i�Ƃ��Ē������Ă�����B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "������Ɨn�������ɂȂ����̂ňꎞ�P�ނ��邱�Ƃɂ����B");//��s40���܂�
				strcpy(quest_text[line++], "�Ƃ���������͂ǂ����Ă��d������Ȃ��B�����N�����Ă���񂾂낤�H");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "�Ƃ̋߂�����ł���Ƃ����Ȃ�΂̋ʂ��Ԃ���ꂽ�B");
				strcpy(quest_text[line++], "�΂̋ʂ��������z�͋߂��̓��A�ɓ������񂾂悤���B");
				strcpy(quest_text[line++], "�܂����̎O�l�̎d�ƂɈႢ�Ȃ��B");
				strcpy(quest_text[line++], "���x�����܂Ƃ߂ē��点�Ă�낤�B");
			}
		}
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			if (comp)
			{
				strcpy(quest_text[line++], "�`���m�u�ււ�A�d�����r�߂邩�炱���Ȃ�񂾁I");
				strcpy(quest_text[line++], "�C�����������炱������O�����ɂ�邼�I������ƑO�ɏE�������󂾁I�v");
			}
			else if (fail)
			{
				strcpy(quest_text[line++], "�`���m�u�����[�I���̐��ł������Ă����ĂȂ��Ȃ�ā[�I");
				strcpy(quest_text[line++], "�������͂܂Ƃ߂ē��点�Ă��I�v");
			}
			else
			{
				strcpy(quest_text[line++], "�`���m�u���������Εςȓz�ɉ΂̋ʂ��Ԃ���ꂽ���Ƃ�����񂾁B");
				strcpy(quest_text[line++], "�������������̕����I�݂�Ȃŕ񕜂��ɍs�����I�v");
			}

		}
		else if(pc == CLASS_SUNNY || pc == CLASS_LUNAR || pc == CLASS_STAR || pc == CLASS_3_FAIRIES || pc == CLASS_DAIYOUSEI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�`���m�u�Ӂ[��A�ςȓz�炪��������ˁB");
				strcpy(quest_text[line++], "�G��ߒ����Ĉ���������ˁI������������I�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�`���m�u���[��A���������񂾂�H");
				strcpy(quest_text[line++], "���∫���������Ă΁I����Ȃ��ɂ܂Ȃ��ł�I�v");
			}
			else
			{
				strcpy(quest_text[line++], "�`���m�u�������͂悭������Ă��ꂽ��ˁI");
				strcpy(quest_text[line++], "�����ŉ�������S�N�ځI�����q��Ɏ��˂��I");
				strcpy(quest_text[line++], "�c���H���̂��Ƃ����킩��Ȃ����āH");
				strcpy(quest_text[line++], "�������̓��A�̋߂��ł������ɉ΂̋ʂԂ����̂͂��񂽂���Ȃ��́H�v");
			}
		}

		break;

	case QUEST_TUPAI: //�c�p�C�@�p�`�����[�A�t����
		if(pc == CLASS_PATCHOULI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "����ǂ��y�b�g��߂܂����B");
				strcpy(quest_text[line++], "���~�B�����𑗂��Ă悱�����B���ɂ������܂����^��������B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�s�o�ɂ���蓦�����Ă��܂����B");//��s40���܂�
				strcpy(quest_text[line++], "���������d���͋�肾�B");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "���~�B�̃y�b�g�̖��Ȑ������������o�����炵���B");
				strcpy(quest_text[line++], "�܊p�Ȃ̂ŒT���̂���`���Ă����邱�Ƃɂ����B");
			}
		}
		if(pc == CLASS_MEIRIN)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���u�����l�B���ꂨ��l���炲�J���ł����āB");
				strcpy(quest_text[line++], "�����n�߂��̂��m��Ȃ����ǁA�����I��点�ċA���Ă��Ȃ����B�v");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���u�����̘H�n�Ŏ�蓦�������ł����āH�v");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "(���̎p�������Đ��b��Ăяo������)");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "���u��̂̏ꏊ�����������̂ō��߂܂��Ă���l�ɂ��͂����Ă�����B");
				strcpy(quest_text[line++], "�M�����������Ǝ�����ɖ߂�����H�v");
				
			}
			else
			{
				strcpy(quest_text[line++], "���u�������A����ȏ��ŉ������Ă���́H");
				strcpy(quest_text[line++], "�܂�������B");
				strcpy(quest_text[line++], "�c�p�C�������o���Ă��܂����́B�M�����T���̂���`���Ȃ����B�v");
			}
		}
		if(pc == CLASS_SAKUYA)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�����y�b�g���߂肨��l�͖������ꂽ�悤���B");
				strcpy(quest_text[line++], "�J���Ƃ��čŋߋC�ɓ����Ă��郊�{���𒸂����B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "��x�͌��������s�o�ɂ���蓦�����Ă��܂����B");
				strcpy(quest_text[line++], "����܂������Ԃ��B");
			
			}
			else
			{
				strcpy(quest_text[line++], "����ɂ��Ă��邤���ɂ���l�̃y�b�g�������o���Ă��܂����B");
				strcpy(quest_text[line++], "�����ɕ߂܂��Ȃ��Ƃ���l���߂���ł��܂��B");
			}
		}


		if(pc == CLASS_FLAN)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�ǂ��ɂ��y�b�g���󂳂��߂܂����B");
				strcpy(quest_text[line++], "���o�l�����𑗂��Ă悱�����B���ɂ������܂����^��������B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�s�o�ɂ���蓦�����Ă��܂����B");//��s40���܂�
				strcpy(quest_text[line++], "���������d���͋�肾�B");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "���o�l�̃y�b�g�̖��Ȑ������������o�����炵���B");
				strcpy(quest_text[line++], "�܊p�Ȃ̂ŒT���̂���`���Ă����邱�Ƃɂ����B");
			}


		}
		break;

		//�N���[���n��
	case QUEST_CLONE:
		if(pc == CLASS_KANAKO)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���V���u���X�܂������Ƃ��N����܂����B");
				strcpy(quest_text[line++], "��̃N���[��������ŃN���[���B���\�����Ă��܂��܂��ĂˁB�v");
				strcpy(quest_text[line++], "�������܂������Ƃ��n�������肢�������B�v");
			}
		}
		break;
		//�h���S���ގ��@�d���A�O�����A�A���X
	case QUEST_CHAOS_WYRM:
		if(pc == CLASS_SUNNY || pc == CLASS_LUNAR || pc == CLASS_STAR || pc == CLASS_3_FAIRIES )
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�{���Ƀh���S����|���Ă��܂����B");
				strcpy(quest_text[line++], "�Ƃ̋߂��𒲂ׂĂ�����Y��Ȏw�ւ��������B�������̕󕨂ɂ��悤�B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�낤�����x�݂ɂȂ�Ƃ��낾�����B");//��s40���܂�
				strcpy(quest_text[line++], "�f���ɗ얲����ɗ��݂ɍs�����E�E");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "���΂炭����ɂ��Ă���ƉƂ̋߂��Ɍ������Ƃ��Ȃ��h���S����������Ă����B");
				if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
					strcpy(quest_text[line++], "�ł����Ȃ炱��Ȃɒ��Ԃ�����I�h���S���ɂł����Ă����ȋC������I");
				else
					strcpy(quest_text[line++], "�ł����������͌��C��t���I�h���S���ɂł����Ă����ȋC������I");
			}
		}
		else if(pr == RACE_FAIRY )
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�T�j�[�~���N�u�����[���I�d�������̖閾�����߂���I�v");
				strcpy(quest_text[line++], "���i�`���C���h�u�Ƃ������M���{���ɗd���Ȃ́H�v");
				strcpy(quest_text[line++], "�X�^�[�T�t�@�C�A�u����ɂ���������グ�܂���B�������̕󕨂�B�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�T�j�[�~���N�u�����H���������ł���B�v");
				strcpy(quest_text[line++], "���i�`���C���h�u���[���A�܂������z�������B�v");//��s40���܂�
				strcpy(quest_text[line++], "�X�^�[�T�t�@�C�A�u���x�̖~�͂͏o�����ǂ������̂Ɏc�O�˂��B�v");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "�T�j�[�~���N�u�҂��Ȃ����A�X�̕��ɍs���Ɗ�Ȃ���I�v");
				strcpy(quest_text[line++], "���i�`���C���h�u�������ȃh���S���Ɉ��x�݂ɂ��ꂿ�Ⴄ��B�v");
				strcpy(quest_text[line++], "�X�^�[�T�t�@�C�A�u����̛ޏ������Q�΂�����B���������̂˂��B�v");
			}
		}

		break;

		//��������
	case QUEST_OLD_WILLOW:
		break;

		//�Η������@������,���ӁA���Ƃ�
	case QUEST_SHOOTINGSTAR:
		if(pc == CLASS_YUGI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���Ƃ�u�M���ł����E�E���x�����Ƃ���ɗ��Ă���܂����B");
				strcpy(quest_text[line++], "���͋��ܔM�n���̍Ő[���ɋ��\�ȉΗ������ݒ����āA");
				strcpy(quest_text[line++], "���Ȃ����Ă��Ă���̂ł��B");
				strcpy(quest_text[line++], "�ς݂܂��񂪁A���Ƃ������|���Ē����܂��񂩁H�v");
			}
		}
		else if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���ɕ������������ł��Ă邪�Ȃ����������v���o���Ȃ��B");
				strcpy(quest_text[line++], "���̊Ԃɂ���ȏ����Ɏ����Ă����B");//��s40���܂�
			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
				strcpy(quest_text[line++], "���Ӂu����H���N�������ɋ��Ȃ����������H�v");
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ�u�����ς킹�Đ\���󂠂�܂���B");
				strcpy(quest_text[line++], "�d�˂ċ��k�Ȃ̂ł����A�����������ŊǗ����Ē����܂��񂩁H");//��s40���܂�
				strcpy(quest_text[line++], "�댯�Ȗ�����Ȃ̂ŁA���܂�n��ɂ����Ă��������͂Ȃ��̂ł��B�v");
			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���Ƃ�u�M���́E�E�������Ă��ƁB");
				strcpy(quest_text[line++], "���̊��ɋy��ŉB���Ă��d������܂���ˁB");
				strcpy(quest_text[line++], "���ܔM�n���̍Ő[���ɈيE�̋��\�ȉΗ������ݕt���܂��āA");
				strcpy(quest_text[line++], "���̃y�b�g�B������Ă��Ă���̂ł��B");
				strcpy(quest_text[line++], "�Ǘ��ӔC�ɂ��Ă͏d�X���m���Ă���܂����A");
				strcpy(quest_text[line++], "���X��`���Ē����킯�ɂ͎Q��܂��񂩁H�v");
			}
		}
		else if(pc == CLASS_SATORI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ����͂ȗ���|�����Ƃɐ��������B");
				strcpy(quest_text[line++], "���͂Ȃ��Ȃ��ʔ������ȏ�������Ă����B");//��s40���܂�
			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���ܔM�n���ɈيE�̉Η������ݒ������ƃy�b�g����񍐂��������B");
				strcpy(quest_text[line++], "�����Ă����̂��댯�Ȃ̂œ����Ɍ�����Ȃ��Ƃ����Ȃ��B");
			}

		}
		else if(pc == CLASS_ORIN || pc == CLASS_UTSUHO)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�ǂ��ɂ����͂ȗ���|�����B");
				strcpy(quest_text[line++], "�n��̒��ɏ񂪕����Ă���B�������̂ŏE���Ă����B");//��s40���܂�
			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
			}
		}
		else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			if (comp)
			{
				strcpy(quest_text[line++], "���Ƃ�u���Ȃ����������̉Η���|�����ł����āH");
				strcpy(quest_text[line++], "�@�d�����Q���Ɣn���ɂȂ�Ȃ���ˁB");
				strcpy(quest_text[line++], "�Ƃ����ꍢ���Ă����̂�B���������B");
				strcpy(quest_text[line++], "��V�ɂ���������܂��傤�B�ł����p����Ɛg��łڂ����H�v");

			}
			else if (fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
				strcpy(quest_text[line++], "���Ӂu...���ĉ��������O���񂽂��̓]���]���ƁB");
				strcpy(quest_text[line++], "�@�����Ă�����H����̓�������璆�ɓ���񂶂�Ȃ���H�v");
			}

		}
		else if(pr == RACE_FAIRY)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ�u���Ȃ������̉Η���|�����ł����āH");
				strcpy(quest_text[line++], "�E�E�R�ł��ϑz�ł��Ȃ��悤�ˁB");
				strcpy(quest_text[line++], "�₩�ɂ͐M�������ǁA��������܂��B");
				strcpy(quest_text[line++], "��V�ɂ���������܂��傤�B�ł����p����Ɛg��łڂ����H�v");

			}
			else if(fail)
			{
				msg_print("ERROR:�����ɂ͗��Ȃ��͂�");
			}
			else
			{
				strcpy(quest_text[line++], "���Ӂu���ܔM�n���ɋ����������ݒ������񂾂��Ă��B");
				strcpy(quest_text[line++], "�@���Ƃ�l�������Ă邩�炠�����B�ŉ��Ƃ����Ȃ��ƁB�v");
				strcpy(quest_text[line++], "����u�΂�f���h���S�������B�v���Ԃ�ɖ\��b�オ���肻���ˁB�v");
				strcpy(quest_text[line++], "���Ӂu����H���N�������ɋ��Ȃ����������H�v");
			}
		}

		break;

		//���ۉ��O�@���~���A
	case QUEST_KYOMARU:
		if(pc == CLASS_REMY)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�p�`�����[�u����́E�E���ۉ��O�H");
				strcpy(quest_text[line++], "���̂��߂ɂ���Ȑ����炯�̂Ƃ���Ɏ��ɍs���Ă��ꂽ�́H");
				strcpy(quest_text[line++], "���肪�Ƃ��B����ɂ�����������B�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�p�`�����[�u���̂������ɗ���Ŏ����Ă��Ă��炨��������B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�p�`�����[�u���̖{�ɍڂ��Ă���Ԃ��C�ɂȂ�́H");
				strcpy(quest_text[line++], "����͋��ۉ��O�B�����̑�ɍ炭�������Ԃ�������B�v");
			}
		}
		else if(pc == CLASS_MARISA)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�p�`�����[�u�O�b���Ă������ۉ��O�A���ɂ�������Ă���Ȃ��H");
				strcpy(quest_text[line++], "��������ɖ{�������čs���Ă�񂾂���A���܂ɂ͉������ɗ����Ȃ����B�v");
			}

		}
		else if(pc == CLASS_SAKUYA)
		{
			if(fail)
			{
				strcpy(quest_text[line++], "�p�`�����[�u���Ȃ��̔L�x��6�_�B���������f���ɂȂ��Ă��܂���B�v");
			}

		}

		break;



		//��l�Ȃ�Ă��킭�Ȃ��@�e�T
	case QUEST_SENNIN:
		if(pc == CLASS_KAGEROU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���l�u�L�����@�_�q�v�̕����ȍ~�A��l�Ƃ��ďC�Ƃ��n�߂�҂������Ă���B");
				strcpy(quest_text[line++], "��l�͗d���̑�G���B���Ɏ��������̂悤�ȗ͂̎ア�d�������ɂƂ��ẮB");//��s40���܂�
				strcpy(quest_text[line++], "����ȏ�꒣������������O�ɒ@���Ă����ׂ���������Ȃ��B");
				strcpy(quest_text[line++], "���Ȃ��͂����l���A�_�Ђ̎Q�������l�̏Z���֒P�g�������邱�Ƃɂ����B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "���Ȃ��͍����Ȑ��l��ł��|���A��l�W�c�ɒɌ���^���邱�Ƃɐ��������B");
				strcpy(quest_text[line++], "���Ԃ��������Ȃ��Ɋ��ӂ̑��蕨��p�ӂ��Ă��ꂽ�B");
				strcpy(quest_text[line++], "����̐��Ƌ��ɂ��肪�����󂯎�����B");
			}
			else
			{
				strcpy(quest_text[line++], "���̐��l�̗͂͗\�z��y���ɒ����Ă����B���Ȃ��͖����炪�瓦���o�����B");
				strcpy(quest_text[line++], "�Â��˂Ŏ����ƒ��ԒB�̑O�r���Ă����Ɍ����Ĉꐺ���̂ł������B");
			}
		}
		break;

		//��n�x�����@���@�������o�[
	case QUEST_GRAVEDANCER:
		if(pc == CLASS_KYOUKO ||pc == CLASS_NAZRIN || pc == CLASS_ICHIRIN ||pc == CLASS_MURASA || pc == CLASS_SHOU )
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���@�u�������Ŗ����ɑ��V���I���邱�Ƃ��ł��܂����B�{���ɂ��肪�Ƃ��B�v");
				strcpy(quest_text[line++], "�Ƃ���ŁA���΂炭�O�������Ă������̂����������́B");
				strcpy(quest_text[line++], "���ꂩ��̋M���̏����ɂȂ�Ǝv����B�悯��Ύg���Ă݂ĂˁB�v");
			}
		}
		break;

	//��T�ގ��@�킩�����P	
	case QUEST_OOGAME:
		if(pc == CLASS_WAKASAGI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ȃ��͌�����C�̑�T�𒦂炵�߂��B");
				strcpy(quest_text[line++], "��T���Y��Ȏw�ւ����Ȃ��ɂ��ꂽ�B���Ȃ��̂��Ƃ��΂̎傾�Ǝv���Ă���炵���B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���Ȃ��͖����炪��΂��瓦���������B");
				strcpy(quest_text[line++], "�����͐_�Ђ̗��̒r�ɂł����ɂȂ邵���Ȃ����낤�E�E");
			}
			else
			{
				strcpy(quest_text[line++], "�΂��j���ł���Ƌ����ȑ�T�̃����X�^�[�ɑ��������B");
				strcpy(quest_text[line++], "���̂܂܂ł͖��������������Ȃ��B���Ƃ����Ȃ��ẮB");
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�킩�����P�u���΂ŗV�񂶂�_����I");
				strcpy(quest_text[line++], "�΂�f�����낵���������\��Ă���񂾂���I�v");
			}
			else if(comp)
			{
				if (pc == CLASS_3_FAIRIES)
				{
					strcpy(quest_text[line++], "�킩�����P�u���Ȃ����������̉�����|�����́H");
					strcpy(quest_text[line++], "�d���̒[����Ƃ��ė��ꂪ�Ȃ���E�E");
					strcpy(quest_text[line++], "����ɂ�����������ˁB�v");

				}
				else
				{
					strcpy(quest_text[line++], "�킩�����P�u�R�ł��傤�A���Ȃ������̉�����|�����́H");
					strcpy(quest_text[line++], "�d���̒[����Ƃ��ė��ꂪ�Ȃ���E�E");
					strcpy(quest_text[line++], "����ɂ�����������ˁB�v");

				}
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�킩�����P�u���΂炭�_�Ђ̗��̒r�ɂł������z����������E�E�v");
			}
		}

		break;

	//�Â���@���A�͂���
	case QUEST_OLDCASTLE:
		if(pc == CLASS_REIMU)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�͂��āu���ˁ[�B���΂͔���̛ޏ����ĂƂ��H");
				strcpy(quest_text[line++], "�ޏ��Ɏ؂�������ςȂ����Ă̂������������A");
				strcpy(quest_text[line++], "��V�ɂ��ꎝ���čs���Ă�B�v");
			}
		}
		else if(pc == CLASS_MOMIZI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�͂��āu���A���B��̏�̑|�����̐땺��\���t����ꂽ�񂾂��āH");
				strcpy(quest_text[line++], "���l�Ƃ����Ƃ������ȘA���΂��肢��̂ɑ�V��l�����˂��B");
				strcpy(quest_text[line++], "�܁A���͏E���Ă����邩��撣���ĂˁB�v");
			}
		}
		else if(pr == RACE_KARASU_TENGU)//v2.0.3 ��V��S�ʓ������b�Z�[�W�ɂ���
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�]���҂�����P�R�r�ł��A�傢�ɖʖڂ��{�����B");
				strcpy(quest_text[line++], "����ɖJ�ܕi�Ƃ��ĈȑO����ڂ����Ă����󕨂�����󂯂邱�Ƃɐ��������B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�܂������Ԃ������Ă��܂����B");
				strcpy(quest_text[line++], "�����̊ԐV�����ɂ͊���o�������ɂȂ��c�c");
			}
			else
			{
				strcpy(quest_text[line++], "�쓌�̌Â���ǂɃ����X�^�[���������H���Ă���Ƃ������𓾂��B");
				strcpy(quest_text[line++], "�ȑO�̐N�������ƈႢ�A����̗]���҂͂��Ȃ若���炵���B");
				strcpy(quest_text[line++], "�F�ɐ�񂶂Ă�����������A�����ԏ�ƃX�N�[�v�l���̈ꋓ������_���̂������Ȃ��B");
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�͂��āu���΂�腖��l�ˁ[�B");
				strcpy(quest_text[line++], "�����V��l����A����̌��̊��ӂ̈�ɂ��[�߂����������āB");
				strcpy(quest_text[line++], "���ꂶ��A�Ȃ񂩑�V��l�ɌĂ΂�Ă邩��s���Ȃ���B");
				strcpy(quest_text[line++], "���������킠�肪�Ɓ[�B�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�͂��āu���[��A腖��l���Ď��l���肶��Ȃ��Ƃ���ȋ����Ȃ��̂ˁB");
				strcpy(quest_text[line++], "�܂��d���Ȃ����B�R�̖��͎R�ŉ��Ƃ����Ȃ��Ƃˁ[�B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�͂��āu����A�Ȃ��腖��l������ȂƂ��ɋ���́H");
				strcpy(quest_text[line++], "���������x�ɒ��Ȃ񂾁B");
				strcpy(quest_text[line++], "����Ȃ炿����Ɨ��݂��Ƃ��Ă������ȁ[�B");
				strcpy(quest_text[line++], "���̊O��̌Â���ǂ����ȉ����̑��ɂȂ�������Ă�́B");
				strcpy(quest_text[line++], "�܂��͏����̓V��B����U�߂���񂾂��ǁA");
				strcpy(quest_text[line++], "�܂��ǂ����Ă����Ă����ɂȂ����A�ł���Ύ�`���Ă����ė~�����ȁB�v");
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�͂��āu��������B���񂽂���̋󂫉Ƃ̗]���҂��̂����񂾂��āH");
				strcpy(quest_text[line++], "�d���̂����ɂ��̂ˁ[�B");
				strcpy(quest_text[line++], "���łɂ�����d�����܂�Ă݂Ȃ��H");
				strcpy(quest_text[line++], "���x�͓��̌Â���ǂ�B���̋󂫉ƂƂ͔�ו��ɂȂ�Ȃ����G����������B");
				strcpy(quest_text[line++], "�܂������ɂƂ͌���Ȃ����ǁA�r�Ɏ��M������Ȃ����Ă݂Ȃ�B�v");
			}
		}
		break;

	case QUEST_LOOKING_GLASS:
		if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				if(pc == CLASS_SUNNY ||pc == CLASS_LUNAR ||pc == CLASS_STAR || pc == CLASS_3_FAIRIES)
				{
					strcpy(quest_text[line++], "�A���X�u����A������̎O�l�g����Ȃ��́B");
					strcpy(quest_text[line++], "�܂��g����U�镑���Ă��������Ƃ��낾���ǁA��������Ɠs���������́B");
					strcpy(quest_text[line++], "��Ȃ����玄�̉Ƃɍs������ʖڂ�B�v");
				}
				else
				{
					strcpy(quest_text[line++], "�A���X�u�d���H�E�E�̊��ɖ��ɋ������ˁB");
					strcpy(quest_text[line++], "�˂��M���A�r�Ɏ��M�͂��邩����H");
					strcpy(quest_text[line++], "�������牜�ɍs�����Ƃ���ɂ���Ƃ̒��ɉ������������Ă���́B");
					strcpy(quest_text[line++], "�������ǂ��ɂ��ł��Ȃ��H�v");
				}
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�A���X�u�d���������|���Ȃ�āA���ꂪ������o�Ă���������������B");
				strcpy(quest_text[line++], "���z���Ɉ�̉����N�����Ă���̂�����B");
				strcpy(quest_text[line++], "�ł��܂��͂�������Ȃ��ƂˁB������������B�v");
			}
		}
		else if(pc == CLASS_ALICE )
		{
			if(comp)
			{
				strcpy(quest_text[line++], "��ȉ�����|�����B��̂ǂ���������Ă����̂��낤�H");
				strcpy(quest_text[line++], "�Ƃ𒲂ׂĂ���Ƌ���Ɍ��o���̂Ȃ������i�������Ă����B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�����͗\�z�ȏ�ɋ��͂Ŏ�ɕ����Ȃ������B");
				strcpy(quest_text[line++], "�������̉���𓾂ĉ��Ƃ��Ȃ������A");
				strcpy(quest_text[line++], "���̎؂�͂����ꑊ���������ɈႢ�Ȃ��E�E");
			}
			else
			{
				strcpy(quest_text[line++], "���΂炭����ɂ��Ă��邤���ɉƂɊ�ȉ��������ݕt���Ă����B");
				strcpy(quest_text[line++], "���Ȃ�̋��G�̂悤�����ꍏ�������ǂ�����Ȃ��Ƃ����Ȃ��B");
			}
		}
		else if(pc == CLASS_MARISA )
		{
			if(fail)
			{
				strcpy(quest_text[line++], "�A���X�u������قǂɂ��Ȃ��̂ˁB");
				strcpy(quest_text[line++], "����ɂ��Ă���������B���ɒN�������������𗊂߂����Ȑl�Ȃ�ċ���������E�E�v");
			}
		}

		break;

		//���l�@�p�`�����[
	case QUEST_FLAN:
		if(pc == CLASS_PATCHOULI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ����l���Ȃ��߂邱�Ƃɐ��������B");
				strcpy(quest_text[line++], "���������ŏ\�N�O���琸�����̚b�����Ö򂪂��߉ނɂȂ�Ƃ��낾�����B");
				strcpy(quest_text[line++], "�܂����S�Ȍ��ʂ͂Ȃ�����������ł��܂����B���X�N�}�l�W�����g�͑厖���B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�}���ق����󂵂Ă��܂����B�l�X�Ȍ�����������S�ăp�A���B");
				strcpy(quest_text[line++], "�����͐��U�ő�̖���ɈႢ�Ȃ��E�E");
			}
			else
			{
				strcpy(quest_text[line++], "���C�h�d���������p�j�b�N���N�����Ă���B");
				strcpy(quest_text[line++], "�b�𕷂��Ă��v�̂𓾂Ȃ����A���l���ᛂ��N�����Ċق̒��Ŗ\��Ă���炵���B");
				strcpy(quest_text[line++], "���~�B�ƍ��͐_�Ђ̉���œ����A���Ă��Ȃ��B");
				strcpy(quest_text[line++], "�}���قɔ�Q���o��O�ɉ��Ƃ��~�߂Ȃ��Ƃ����Ȃ��B");
			}
		}
		else if(pc == CLASS_REMY)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ɉꂵ����������ĕ����֋A�����B");
				strcpy(quest_text[line++], "�p�`�F������ɂƉ���������������ꂽ�B");
				strcpy(quest_text[line++], "���ނƌ��N�ɂȂ�炵���B���̏�k���낤�B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���͈�Ӓ��\��܂�������ƕ����֋A���čs�����B");
				strcpy(quest_text[line++], "����ɂ��Ă��h��ɂԂ��󂵂Ă��ꂽ���̂��B");
				strcpy(quest_text[line++], "���Ƀp�`�F�ɂ͋C�̓łȂ��ƂɂȂ����B");
			}
			else
			{
				strcpy(quest_text[line++], "�����Ԃ�ɊقɋA��ƁA���C�h�d���������p�j�b�N���N�����Ă���B");
				strcpy(quest_text[line++], "�b�𕷂��Ă��v�̂𓾂Ȃ����A�����ᛂ��N�����Ė\��Ă���炵���B");
				strcpy(quest_text[line++], "�o�Ƃ��ĐܟB������Ă��K�v������悤���B");
			}
		}
		else if(pc == CLASS_SAKUYA)
		{
			 if(fail)
			{
				strcpy(quest_text[line++], "���͂⎩���ł͎�ɕ��������ɂȂ��B");
				strcpy(quest_text[line++], "����l���ܟB�Ɍ��������̂ł��̂����Ђ͕t�����낤���A");
				strcpy(quest_text[line++], "�}���ق���ł�����ꂽ�p�`�����[�l�ɂ͍��킹��炪�Ȃ��B");
			}
		}
		break;

		//���g�ւ̎h�q�@�i�鏴�Ŗʎ�����A���S�ā@������̂�
		//�i�Ԃ�
	case QUEST_KILL_MOKO:
		if(pc == CLASS_YOUMU || pc == CLASS_YUYUKO || pc == CLASS_REIMU || pc == CLASS_REMY || pc == CLASS_MARISA || pc == CLASS_ALICE || pc == CLASS_SAKUYA || pc == CLASS_YUKARI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�P��u�˂��A������x�̎���������Ă݂�C�͂Ȃ��H");
				strcpy(quest_text[line++], "���x�͈�l�Ŗ��g�̂Ƃ���ɍs���ēK���ɂ̂��ė��Ȃ����B");
				strcpy(quest_text[line++], "��V�͂͂��ނ��H�v");
			}

		}
		else if(pc == CLASS_TEWI || pc == CLASS_UDONGE)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�P��u����C�i�o�A���x�����Ƃ���ɗ�����ˁB");
				strcpy(quest_text[line++], "���낻�떅�g�ɂ܂��h�q�ł����낤�Ǝv���́B�s���Ă�����Ⴂ�B�v");
			}

		}
		else if(pc == CLASS_EIRIN)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�P��u�˂��i�ԁA������Ɩ��g�̂Ƃ���ɂ�����čs���Ȃ��H�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�s�����̖����璿�����f�ނ��̎悵���B");
				strcpy(quest_text[line++], "�厖�ɕۊǂ��Ă������B");
			}
			else
			{
				strcpy(quest_text[line++], "�P��u�����A��l������ŕ��������������Ȃ��́I");
				strcpy(quest_text[line++], "����V��ł���̂�i�ԁI�v");
			}

		}
		else if(pc == CLASS_KAGUYA)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���������΍ŋߖ��g�Ƃ�荇���Ă��Ȃ��B");
				strcpy(quest_text[line++], "���낻�뒼�X�ɏo�����̂��������낤�B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "���g���璿�����f�ނ��̎悵���B");
				strcpy(quest_text[line++], "�V�������Ɏg���Ȃ����낤���B");
			}
			else
			{
				strcpy(quest_text[line++], "�P���Ɏ��s�����B���g���F�X���܂�Ęr���グ�Ă���悤���B");
			}

		}
		break;

	case QUEST_KILL_GUYA: //�P��ւ̎h�q�@���g
		if(pc == CLASS_MOKOU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���������΍ŋߋP��Ƃ�荇���Ă��Ȃ��B");
				strcpy(quest_text[line++], "���܂ɂ͂����炩��o�����̂��������낤�B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�i��������ł����Ă�����B");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�P�邪�Ō�Ɏg�����Ƃ����󕨂𐨂��ŒD���Ă��Ă��܂����B");
				strcpy(quest_text[line++], "�܂��ǂ������̂����D���Ԃ��ɗ��邾�낤�B");
			}
			else
			{
				strcpy(quest_text[line++], "�P���Ɏ��s�����B�ǂ�����Ď@�m�����̂��G�̔����͖��S�������B");
				strcpy(quest_text[line++], "�ǂ����ŋߕ���������ł���C������B");
			}
		}
		else if (pc == CLASS_UDONGE) //v1.1.44 ��p���i
		{
			if (accept)
			{
				strcpy(quest_text[line++], "���ɂ��t���l�𒴂��鎞�������B");
				strcpy(quest_text[line++], "���Ȃ��͒P�g�i�����ւƌ��������c");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "���Ȃ��͌���̖��ɂ��t���l�ƕP�l��œ|�����B");
				strcpy(quest_text[line++], "����Ō��̎������X�g���X����J������ċC�����悭�ڊo�߂��邾�낤�B");
				strcpy(quest_text[line++], "���Ȃ��͂ƂĂ��������ł������B");
			}
			else
			{
				strcpy(quest_text[line++], "���t���l�̗͂͂��Ȃ��̑z���ȏゾ�����B");
				strcpy(quest_text[line++], "�܂��\�ꑫ��Ȃ����J�����炵�͂����܂łɂ��Ă������B");
			}
		}


		break;
	case QUEST_MITHRIL: //�~�X�����T���@�i�Y�[����
		if(pc == CLASS_NAZRIN)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�����u�~�X�����v�𔭌������B");
				strcpy(quest_text[line++], "������ւɉ��H���Ă���������E�E");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "��͂�P�Ƃł̐[�w�T���͖���������悤���B");//��s40���܂�
				strcpy(quest_text[line++], "�����ꂲ��l�̗͂ŉ��Ƃ����Ă��炨���E�E");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "�S���ɂ͐_��̋����u�~�X�����v������炵���B");
				strcpy(quest_text[line++], "������g���ă��b�h�����Ε�T��������ɈႢ�Ȃ��B");
			}
		}
		break;
		//�j��2�@��
	case QUEST_DOOM2:
		if(pc == CLASS_BYAKUREN)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���S�����͑ގU���Ă������B�܂��͈���S���B");
				strcpy(quest_text[line++], "���Ȃ��̐g���Ă�����q�����������̗͂��������ɓ���Ă��ꂽ�B");
				strcpy(quest_text[line++], "���肪�����g�킹�Ă��炨���B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���S�����͗\�z�ȏ�ɋ��͂Ŏ�ɕ����Ȃ������B");
				strcpy(quest_text[line++], "���z���̑��̎��͎҂����𗊂��ĉ��Ƃ����������񂾂��A");
				strcpy(quest_text[line++], "���΂炭�͔ޏ������ɓ����オ�肻���ɂȂ��E�E");
			}
			else
			{
				strcpy(quest_text[line++], "�يE�̈��S�����@���ɐN�U����ĂĂ���B");
				strcpy(quest_text[line++], "�b�͒ʂ��Ȃ��B�키�����Ȃ��悤���B");
				strcpy(quest_text[line++], "���S�̐��ƂɈ�l�ŏ�荞�߂Δ�Q�͍ŏ����ōςނ͂����B");
			}
		}
		else if(pc == CLASS_KYOUKO ||pc == CLASS_NAZRIN || pc == CLASS_ICHIRIN ||pc == CLASS_MURASA || pc == CLASS_SHOU )
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���@�u�������ł��B�M���͂��΂炭���Ȃ������ɑ�ςȏC�s��ς�ł����̂ˁB");
				strcpy(quest_text[line++], "����͗��̕�����i���ĉ����������͂Ȏ����̏�ł��B");
				strcpy(quest_text[line++], "�����̓���i�ł����A���̋M���Ȃ炽�₷���g�����Ȃ���͂���B�v");
			}
		}
		break;

		//�Q�������C�u�@�d��
	case QUEST_LIVE:
		if(pc == CLASS_REIMU)
		{
			if(fail)
			{
				strcpy(quest_text[line++], "�d���u����̛ޏ��ł��G��Ȃ��Ƃ́B");
				strcpy(quest_text[line++], "����͗R�X������肾���B������͗��̑����ɂ�����邩�������E�E�v");
			}

		}
		else if(pc == CLASS_KEINE)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "���Ȃ��͖钆�ɑ������T���U�炷�Q�������C�u�ɂ����䖝���Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "���̎҂͕s���ɔY�܂���Ă��邵�A");
				strcpy(quest_text[line++], "���̂܂܂ł͂��Ȃ��̗��j�Ҏ[�ɂ�������肪�o�邩������Ȃ��B");
				strcpy(quest_text[line++], "���X���\�����A���荞�݂������Ďd�u��������Ă�邱�Ƃɂ����B");
				strcpy(quest_text[line++], "���������C�u�ɂ͍ŋ߂ٕ̈ςŗ͂�t�������͂ȕt�r�_���Q�����Ă���悤���B");
				strcpy(quest_text[line++], "����Ƀ��C�u�̊ϋq�������S�đ���ɂ��Ȃ��Ƃ����Ȃ��B");
				strcpy(quest_text[line++], "���O�ȏ��������Ă���������K�v�����邾�낤�B");

			}
			else if(comp)
			{
				strcpy(quest_text[line++], "���Ȃ��͑匃��̖��ɃQ�������C�u����ł������B");
				strcpy(quest_text[line++], "���q���ɖ߂�����B�c�Ƃ����i���͂��Ă����B");
				strcpy(quest_text[line++], "����͖��͂��񕜂����鋭�͂ȏ񂾁B");
				strcpy(quest_text[line++], "���������̏�Ɠ��l�A�ȒP�ɔR�������ꂽ�肷��͂����B");
				strcpy(quest_text[line++], "�厖�Ɏg�킹�Ă��炨���B");
			}
			else
			{
				strcpy(quest_text[line++], "�����������͂ȗd�������͂ƂĂ����Ȃ��̎�ɂ͕����Ȃ������B");
				strcpy(quest_text[line++], "��ނȂ�����_�Ђ֋}�g���o�����B");
				strcpy(quest_text[line++], "����얲�Ȃ牽�Ƃ����Ă���邱�Ƃ��낤�E�E");
			}

		}
		else if(pc == CLASS_KOSUZU)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "�d���u�S���A�Q�������C�u�Ƃ������A���ɂ����������̂��ȁB");
				strcpy(quest_text[line++], "�l���ɂ��z��̉B��t�@��������炵�����A���O�͑��v���낤�ȁH");
				strcpy(quest_text[line++], "����ȏ��ɔE�э��񂾂疽�����������Ă�����Ȃ����B�v");

			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�d���u���̑����̒��ő�\�ꂵ�ė͂Â��Ŗق点�Ă����Ƃ����̂��H");
				strcpy(quest_text[line++], "���͂�p�Y�̊킾�ȁB�㐢�؂�ׂ��Ƃ͂��̂��Ƃ��B");
				strcpy(quest_text[line++], "����������Ă����Ƃ����B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�d���u������d���͊댯���Ɖ��x�����΁c");
				strcpy(quest_text[line++], "�搶�͂��O�̏������S�z�����B�v");
			}

		}
		else if(pc == CLASS_MOKOU)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "�ŋߌd���͐����Q�s���̂悤���B�b�𕷂��Ă݂�ƁA");
				strcpy(quest_text[line++], "���̋߂��ŃQ�������C�u�Ȃ闐�s�C����������d���B������炵���B");
				strcpy(quest_text[line++], "����ŗ��̏Z�l���s���ɔY��ł��邻�����B");
				strcpy(quest_text[line++], "�����d���ގ��������󂯂Ă������B");
				strcpy(quest_text[line++], "���C�u�ɂ͋��͂ȗd�����������Ă���炵���B");
				strcpy(quest_text[line++], "�v���Ԃ�ɘr����B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�匃��̖��ɃQ�������C�u����ł������B");
				strcpy(quest_text[line++], "���q���ɖ߂�����B�c�Ƃ����i���͂��Ă����B");
				strcpy(quest_text[line++], "����͖��͂��񕜂����鋭�͂ȏ�炵���B");
				strcpy(quest_text[line++], "���������̏�Ɠ��l�A�ȒP�ɔR�������ꂽ�肷��͂����B");
				strcpy(quest_text[line++], "�g�����ɂ͋C�����Ȃ��ẮB");
			}
			else
			{
				strcpy(quest_text[line++], "�d�������͗\�z�ȏ�ɋ��͂ŁA�P�ނ�]�V�Ȃ����ꂽ�B");
				strcpy(quest_text[line++], "�d���ގ��͖O����قǂ�������A�܂��܂��C�s������Ȃ��悤���B");
			}

		}
		else if(pc == CLASS_EIKI)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "�d���u腖��l�A�\����Ȃ��̂ł����܂��͂�݂��Ē����܂��񂩁H");
				strcpy(quest_text[line++], "�钆�ɋ߂��̐X�ő呛�������鍢�����z�炪���܂��āA");
				strcpy(quest_text[line++], "���̎ҒB���s���ɔY��ł���܂��B");
				strcpy(quest_text[line++], "���ɐV��̕t�r�_�̘A�������Ȃ�̋����łǂ��ɂ�����o���Ȃ��ł���̂ł��B�v");

			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�d���u���̊�@���~���Ē����A���ӂ̌��t���������܂���B");
				strcpy(quest_text[line++], "����͖��͂��񕜂����鋭�͂Ȗ�����ł��B����̈�ɂ��󂯎�肭�������B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�d���u���ƁA�r�Ԃ�d���̗͂�����قǂ̂��̂Ƃ́B");
				strcpy(quest_text[line++], "�}���ޏ��Ɏg�����o���܂��傤�B���͂ł�����˂΂Ȃ�܂���ˁB�v");
			}

		}
		else if(pr == RACE_FAIRY)
		{

			if(accept)
			{
				strcpy(quest_text[line++], "�d���u�����ƍς܂Ȃ��A�������Q���Ă����悤���B");
				strcpy(quest_text[line++], "�ŋߖ�ɂȂ�Ƒ剹�ʂŃ��C�u���n�߂�z�炪���ĂȁB");
				strcpy(quest_text[line++], "�F�Q�s���łقƂقƍ����Ă���񂾂�B�v");
			}
			else if(comp)
			{
				if(pc == CLASS_3_FAIRIES)
					strcpy(quest_text[line++], "�d���u���C�u������ł������̂͂��O�B�������̂��B");
				else
					strcpy(quest_text[line++], "�d���u���C�u������ł������̂͂��O�������̂��B");
				strcpy(quest_text[line++], "���B������Ă��Ă����̂ɁA�d���Ƃ͎v���Ȃ��������ȁB");
				strcpy(quest_text[line++], "��̈�ɂ�����g���Ă���B���͂��񕜂��鋭�͂ȏ񂾁B");
				strcpy(quest_text[line++], "��{�ƂȂ��M�d�i������ȁB��������󂵂���R�₳�ꂽ�肵�Ȃ��ł����B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�d���u��͂肱��ȏ�z�����u�͂ł��Ȃ��ȁB");
				strcpy(quest_text[line++], "�}���ޏ��Ɏg�����o�����Ƃɂ��悤�B�v");
			}

		}

		break;


		//�_�[�N�G���t�̉��@�͓��A�R���A�V��
	case QUEST_DARKELF:
		if(pr == RACE_KAPPA || pr == RACE_YAMAWARO)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�R���̖��u�ށA���O���B");
				strcpy(quest_text[line++], "��̗]���ҘA���̃A�W�g�����̋߂��ɂ���炵���B");
				strcpy(quest_text[line++], "������������̐�͂͐S���Ȃ��B");
				strcpy(quest_text[line++], "�������ɂȂ��Ă邤���ɃA�W�g�ɐN�����Đe�ʂ�|���邩�H�v");
			}

		}
		else if(pr == RACE_KARASU_TENGU || pr == RACE_HAKUROU_TENGU)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�R���̖��u������܂����B��������[�߉������B�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�R���̖��u�V��l�E�E�^�ʖڂɂ���ĉ�������B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�R���̖��u�ށA�V��l�E�E");
				strcpy(quest_text[line++], "�E�E���͂��̋߂��ɎR���r�炷�]���҂̃A�W�g������̂ł��B");
				strcpy(quest_text[line++], "���Ƃ����ĉ������܂��񂩁B�v");
			}
		}
		else if(pr == RACE_ONI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�R���̖��u������܂����B��������[�߉������B�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�R���̖��u�^�ʖڂɂ���Ē����܂��񂩁E�E�v");
			}
			else
			{
				strcpy(quest_text[line++], "�R���̖��u���A�M���́E�E");
				strcpy(quest_text[line++], "���͂��̋߂��ɎR���r�炷�]���҂̃A�W�g������̂ł��B");
				strcpy(quest_text[line++], "���Ƃ����ĉ������܂��񂩁B�v");
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�u���A�S���|�������ƁH");
				strcpy(quest_text[line++], "�d���ɂ����Ƃ́A���F�z����������|���������Ƃ������Ƃ��B");
				strcpy(quest_text[line++], "����J�������B�J���ɂ������낤�B�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�R���̖��u�G�ɂ�����Ȃ�̔����͂���悤���ȁB");
				strcpy(quest_text[line++], "�����s���Ă������B�v");
			}
			else
			{
				strcpy(quest_text[line++], "�R���̖��u�Ȃ񂾗d�����B���x�����B");
				strcpy(quest_text[line++], "���ɓ�����������ȁH���̒��ɔE�э���ŁA");
				strcpy(quest_text[line++], "���ɂǂ�ȓz�����邩��@���Ă��Ă���B�v");
			}
		}
		break;

	//���Ƃ̕�	������,����,���Ƃ�
	case QUEST_CRYPT:
		if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ȃ���ꀏL���揊�ɋ������Ȃ������B");
				strcpy(quest_text[line++], "�ӂƉ�������ƊZ�̂悤�Ȃ��̂������Ă���B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�������ʂ̈��삪��ꂾ�����n���̊X�͑呛�����B");
				strcpy(quest_text[line++], "���Ȃ��͂��΂炭���Ă������₪�ċ������Ȃ����Ă��̏����ɂ����B");
			}
			else
			{
				strcpy(quest_text[line++], "�o�̃y�b�g�B���n���Ɍ����Đ[�������@���Ă���̂����������B");
				strcpy(quest_text[line++], "���Ȃ��͉��ƂȂ��ꏏ�ɓ����Ă݂邱�Ƃɂ����B");
			}
		}
		else if(pc == CLASS_ORIN || pc == CLASS_UTSUHO)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�ǂ��ɂ����͂Ȏr�p�t��|�����B");
				strcpy(quest_text[line++], "�傩��Վ��{�[�i�X�Ƃ��ĕs�v�c�ȊZ����������B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�������ʂ̈��삪��ꂾ�����n���̊X�͑呛�����B");
				strcpy(quest_text[line++], "�����͎�Ƃ��ǂ��j�̂ނ���ɍ��邱�ƂɂȂ肻�����E�E");
			}
			else
			{
				strcpy(quest_text[line++], "�ŋߖ����������Ƃ�����l�̖��Œn���ւ̌����@���Ă����B");
				strcpy(quest_text[line++], "��l�̍l�����Ƃ���A�n���ɗ�_�������蒆�ɂ͉��O���Q�����Ă���B");
				strcpy(quest_text[line++], "�y�b�g�̒��ł��킢�̓��ӂȎ��������邱�ƂɂȂ����B");
				strcpy(quest_text[line++], "�w�A�[�`���b�`�x�Ƃ����p�t���ܑ̓|���Ăق����������B");
			}
		}
		else if(pc == CLASS_SATORI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�ǂ��ɂ����͂Ȏr�p�t��|�����B");
				strcpy(quest_text[line++], "�����ɖ߂�ƌ��o���̂Ȃ��Z�������Ă����B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�������ʂ̈��삪��ꂾ�����n���̊X�͑呛�����B");
				strcpy(quest_text[line++], "�����͐j�̂ނ���ɍ��邱�ƂɂȂ肻�����E�E");
			}
			else
			{
				strcpy(quest_text[line++], "�n���ɗ�_��������A���ɋ��͂ȉ��O���Q�����Ă���B");
				strcpy(quest_text[line++], "���̂܂܂ł͖����������B���Ƃ��|���Ă��܂������B");
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(fail)
			{
				strcpy(quest_text[line++], "���Ƃ�u腖��l�Ƃ͂����يE�̎��҂͊Ǌ��O�ł������B");
				strcpy(quest_text[line++], "���̌��̎n���͐���Ȓ����Ƃ̋����Ƃ������Ƃł��肢���܂��ˁB�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ�u���x�����ς킹�čς݂܂���B");
				strcpy(quest_text[line++], "�揊�̕󕨂͑S�Ĕފ݂̉^�c��ɂ��[�߂��������B");
				strcpy(quest_text[line++], "���ꂩ�炱����������グ�܂��B");
				strcpy(quest_text[line++], "�����炱���ɂ������̂��N�ɂ�������Ȃ��̂ł����A�����Ȃ��̂ł͂���悤�ł��B�v");
			}
		}

		break;
		//�_�ւ̒���@�얲�E���c
	case QUEST_KANASUWA:
		if(pc == CLASS_REIMU)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���c�u���ς�炸�̋����ł��ˁB�������ł����������ł��I");
				strcpy(quest_text[line++], "����ł͂����炪�ܕi�ɂȂ�܂��B�ǂ���������Ƃ����ł��ˁB�v");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���c�u���炢���̍Ⴆ�͂ǂ�������ł��H");
				strcpy(quest_text[line++], "����Ȃ��Ƃ��Ⴂ���ꎄ�B�����z���̐M�S���������Ⴂ�܂���H�v");
			}
		}
		else if(pc == CLASS_SAGUME)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���c�u�ނށc�{�C�o�����狭������Ȃ��ł����B");
				strcpy(quest_text[line++], "����ς�܂��n��N������ĂĂ�񂶂�Ȃ��ł����H�v");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���c�u���炠����v�ł����H");
				strcpy(quest_text[line++], "�ǂ��ł��A�����̐_�l�͂�������ł���I�v");
			}
		}
		else if(pc == CLASS_SANAE)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�����C�s�����������B���J���Ƃ��č��������Ȋ������������B");
				strcpy(quest_text[line++], "��͂��̊��̏ܕi�̂悤�����A�M�S�𔭊����Ėق��Ă��邱�Ƃɂ����B");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�_�X�̗͈͂��|�I�ł������B�܂��܂���y�҂Ǝ�������߂��B");
			}
			else
			{
				strcpy(quest_text[line++], "�_�ގq�l����u���܂ɂ͒��X�ɏC�s�����Ă�낤�v�Ƃ̗L������t�𒸂����B");
				strcpy(quest_text[line++], "�����Ɨ�̕�W�ւ̒���҂����N���[������������ދ����Ă���������̂��낤�B");
				strcpy(quest_text[line++], "�z�K�q�l���ŋߑދ������Ȃ̂łЂ���Ƃ����痐�����ė����邩������Ȃ��B");
				strcpy(quest_text[line++], "�������菀�����ς܂��Ă���A�낤�B");
			}
		}
		break;
		//�����
	case QUEST_HANAMI:
		if(pc == CLASS_MARISA)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�얲�u���疂�����B���̉���̊����͂��񂽂�B�Y��ĂȂ��ł��傤�ˁH");
				strcpy(quest_text[line++], "�ł���Δ�ѐ؂苭�������������Ă��Ă�B");
				strcpy(quest_text[line++], "������Ɛ����ׂ��Ă�肽���V�炪����́B�v");
			}
		}
		else if(pc == CLASS_YUKARI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�얲�u���̉���̊����͂��񂽂�B�Y��ĂȂ��ł��傤�ˁH");
				strcpy(quest_text[line++], "�ł���Δ�ѐ؂苭�������������Ă��Ă�B");
				strcpy(quest_text[line++], "������Ɛ����ׂ��Ă�肽���V�炪����́B�v");
			}
		}
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�얲�u���珬�邿��񂶂�Ȃ��B���̉���ɎQ�����Ă݂Ȃ��H");
				strcpy(quest_text[line++], "�ł���Ώ��邿���ɂ���������������ė~������ˁB");
				strcpy(quest_text[line++], "�Ȃ�ׂ������̂���{���肢�����B�v");
			}
			if(fail)
			{
				strcpy(quest_text[line++], "�얲�u�����H�܂����������͂��Ȃ���B");
				strcpy(quest_text[line++], "���c���畷�������ǁA�w����͂�x�͗ǂ��Ȃ��炵�����B�v");
			}
		}

		break;

	case QUEST_KORI: //�ϒK�푈�@�������Z���t������������
			if(comp)
			{
				if((quest[31].flags & QUEST_FLAG_TEAM_A) && p_ptr->pclass != CLASS_RAN)
				{
					if(p_ptr->pclass == CLASS_CHEN)
					{
						strcpy(quest_text[line++], "���u�悭������ȁB���O�̐����Ɏ����@���������B");
						strcpy(quest_text[line++], "���J���ɂ���������悤�B�厖�Ɏg���Ă����B�v");//��s40���܂�
					}
					else if(p_ptr->pclass == CLASS_YUKARI)
					{
						strcpy(quest_text[line++], "���u��A���l�I�H");
						strcpy(quest_text[line++], "�������̌����͎~�ނɎ~�܂�ʎ�������Ă̂��ƂŁA");//��s40���܂�
						strcpy(quest_text[line++], "�����Ė��߂ɔw������́E�E�v");//��s40���܂�
					}
					else if(p_ptr->pclass == CLASS_YUYUKO)
					{
						strcpy(quest_text[line++], "���u���͓Y���L��������܂����B");
						strcpy(quest_text[line++], "����̈�Ƃ��Ă�������[�߉������B");//��s40���܂�
					}
					else
					{
						strcpy(quest_text[line++], "���u�ӂ�A����ŒK�����g�̒����v���m�������낤�B");
						strcpy(quest_text[line++], "���O���Ȃ��Ȃ��ǂ��������������B����͖J�����B�v");//��s40���܂�
					}
				}
				else if((quest[31].flags & QUEST_FLAG_TEAM_B) && p_ptr->pclass != CLASS_MAMIZOU)
				{
					if(pc == CLASS_NUE)
					{
						strcpy(quest_text[line++], "�}�~�]�E�u�ǂ�����ϋ��߁A�K�̗͂��������I");
						strcpy(quest_text[line++], "���������ӂ��邼���B����͗炶��B�v");//��s40���܂�
					}
					else if(pc == CLASS_YUKARI)
					{
						strcpy(quest_text[line++], "�}�~�]�E�u���̌ς͂��񂳂�̎艺����Ȃ������񂩁H");
						strcpy(quest_text[line++], "�܂��������ɕt����Ă�����͂���ŋ����߂��Ⴊ�E�E");//��s40���܂�
						strcpy(quest_text[line++], "����������čs���B����ő݂��؂�Ȃ�����B�v");//��s40���܂�
					}
					else
					{
						strcpy(quest_text[line++], "�}�~�]�E�u�ǂ�����ϋ��߁A�K�̗͂��������I");
						strcpy(quest_text[line++], "������ӊO�ɂ��̂��B���������Ă����B�v");//��s40���܂�
					}
				}
				else if(pc == CLASS_NITORI)
				{
					strcpy(quest_text[line++], "�ς��K�����قǂɂ��Ȃ��B���Ȃ��͑S�����̂��Ă��܂����B");
					strcpy(quest_text[line++], "���Ȃ��͔�ѓ���̑O�Ɂs���������ɂȂ�t�Ƃ����匊�ɓq���Ă����B");
					strcpy(quest_text[line++], "�q���͂��Ȃ��̈�l�������B���Ԃ���̉��l�̐����S�n�悢�B");
				}
				else
				{
					if(p_ptr->prace == RACE_HUMAN)
						strcpy(quest_text[line++], "�ɂƂ�u������ق��I�ō����斿�F�I");
					else
						strcpy(quest_text[line++], "�ɂƂ�u������ق��I�M���ō�����I");

					strcpy(quest_text[line++], "�������Ŏ��̈�l�����E�E���₢�₱�����̘b�B");//��s40���܂�
					strcpy(quest_text[line++], "���j���ɂ��ꂠ����B���M��̐˗͔������u���B");
					strcpy(quest_text[line++], "�Ȃ��ɋC�ɂ���Ȃ��āB���̋C��������B�v");
				}
			}
			if(pc == CLASS_NITORI)
			{
				if(fail)
				{
					strcpy(quest_text[line++], "���Ȃ��͌ςƒK�̑匈�킩�甇�������̑̂œ����o�����B");
					strcpy(quest_text[line++], "�������ʂ����Â������悤���B");
				}
				if(accept)
				{
					strcpy(quest_text[line++], "�v���Ԃ�ɃA�W�g�ɖ߂�Ɖ͓��̒��ԒB���q���Ő���オ���Ă���B");
					strcpy(quest_text[line++], "�b�𕷂��Ă݂�ƁA�ςƒK���[�̗тő�K�͂Ȍ���������炵���B");
					strcpy(quest_text[line++], "���܂����Έ�ׂ��ł��邩������Ȃ��B");
				}
			}
			else if(pc == CLASS_KOSUZU)
			{
				if(accept)
				{
					strcpy(quest_text[line++], "�ɂƂ�u����H���������񂽂͑ݖ{���Łc�����ƁB");
					strcpy(quest_text[line++], "�܂�������B���ꂩ��[�̗тŌςƒK�̌������n�܂�񂾁B");
					strcpy(quest_text[line++], "�ʔ������猩�Ă����Ƃ�����B");
					strcpy(quest_text[line++], "��ѓ��肵�����Ȃ�~�߂Ȃ����ǁA�����ɂ������~���Ƃ��񂾂ˁB�v");
				}
				if(fail)
				{
					strcpy(quest_text[line++], "�ɂƂ�u�܂����ق�Ƃɔ�ѓ��肷��Ƃ͎v��Ȃ������ˁB");
					strcpy(quest_text[line++], "���䂵�Ă�Ȃ���h���Ă�낤���H�T�����ł�����B�v");
				}

			}

		break;

	case QUEST_WANTED_SEIJA: //���q�ˎҁ@��
		if(pc == CLASS_YUKARI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "�܋���̎�z���𔍂������B");
				strcpy(quest_text[line++], "�܊p��J���ď����������؏ܕi�������Ŏg�����ƂɂȂ肻�����B");
			}
			else if(accept)
			{
				strcpy(quest_text[line++], "���̓V�׋S�͌��z�����̏Z�l��G�ɉ񂵂Ė��������������Ă���B");
				strcpy(quest_text[line++], "���̂܂ܕ��u���Ă͂����Ȃ��B���낻�뒼�X�Ɉ�����n���Ă���悤�B");
			}
		}

		break;

	case QUEST_SPAWNING_PIT: //���B�n���@�ɂƂ�
		if(pc == CLASS_NITORI || pr == RACE_KAPPA)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ȃ��͑匃��̖��ɑ��H���Ă���������S�ē|���A�͓��̗��̉p�Y�ƂȂ����B");
				strcpy(quest_text[line++], "�͓����Ԃ��痢�̕󕨂̋��͂ȊZ��a�������B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�����ނȂ����A�����͉�������Ă��܂����B");//��s40���܂�
				strcpy(quest_text[line++], "���Ȃ�������̐g�̐U������l���Ȃ��Ƃ����Ȃ����낤�E�E");//��s40���܂�
			}
			else
			{
				strcpy(quest_text[line++], "�厖�Ȑ����Ɉٕς��N�����Ă���B");
				strcpy(quest_text[line++], "��̓���̂悤�ȉ��������X�ƐV���ȉ����𐶂ݏo���Ă���̂��B");
				strcpy(quest_text[line++], "���̂܂܂ł͐������͂�Ă��܂��B���Ƃ��������򉻂��˂΂Ȃ�Ȃ��B");
			}
		}
		break;
	//�T�g���R���t���N�g	������,����
	case QUEST_SATORI:
		if(pc == CLASS_KOISHI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ�u�������A�M���������́E�E����������B");
				strcpy(quest_text[line++], "���A���̖{���~�����́H");
				strcpy(quest_text[line++], "�����A���̖{�Ȃ�D���Ɏ����čs���Ă�����B�v");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�������͑�R�̖{�������č��΂����Ȃ��瓦���Ă������B");
				strcpy(quest_text[line++], "���������̒��ɂ͎o�����d�ɕۊǂ��Ă���{���������B���x�����Ă��炨���B");
			}
			else
			{
				strcpy(quest_text[line++], "�������n��a�����X�����������B�������}��������炵���B");
				strcpy(quest_text[line++], "��������̂��낤���B���Ȃ����ꏏ�ɑ҂��Ă݂邱�Ƃɂ����B");
			}
		}
		else if(pc == CLASS_ORIN || pc == CLASS_UTSUHO)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ�u�܂�������ȕ��������o���Ă���Ƃ͂ˁE�E");
				strcpy(quest_text[line++], "�ł��ǂ��ɂ����ނł����悤�ˁB���肪�Ƃ��B");//��s40���܂�
				strcpy(quest_text[line++], "�M�����~�������Ă��{���������B�����Ă����Ȃ����B");//��s40���܂�
			}
			else if(accept)
			{
				strcpy(quest_text[line++], "���Ƃ�u���x�ǂ��Ƃ���ɋA���Ă�����ˁB");
				strcpy(quest_text[line++], "���̔������@�g�������ւ��P�����ɗ��邩������Ȃ��́B");
				strcpy(quest_text[line++], "�M�����h�q����`���Ă��傤�����B�v");
			}
		}
		else if(pc == CLASS_EIKI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���Ƃ�u�����b�ɂȂ�܂����B����ɂ��̖{�������グ�܂��B");
				strcpy(quest_text[line++], "���̖��@�g���̖ړ��Ă̕i�������悤�ł����A");//��s40���܂�
				strcpy(quest_text[line++], "����������Ă��܂��������悢���낤�Ǝv���܂��āB�v");//��s40���܂�
			}
			else if(accept)
			{
				strcpy(quest_text[line++], "���Ƃ�u����腖��l�B���x�ǂ��Ƃ���ɂ�������Ⴂ�܂����B");
				strcpy(quest_text[line++], "�n��a���D�_�E�E�Ƃ�����苭���̏P�����󂯂鋰�ꂪ����̂ł��B");
				strcpy(quest_text[line++], "��낵����Α��̕ߊl�ɗ͂�݂��Ă��������܂��񂩁H�v");
			}
		}
		else if(pc == CLASS_SATORI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "������ʃS�[�����͂��Ȃ�̋��G���������A�ǂ��ɂ����̌��ނɐ��������B");
				strcpy(quest_text[line++], "���̍��z�Ȗ��@�����ړ��Ă������悤�����A�������蕥���Ă��܂������H");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�{�D�_�͍��΂����Ȃ��瓦���Ă������B");
				strcpy(quest_text[line++], "��Q�𒲍������Ƃ���A�l�Ɍ������Ȃ��K������Ă�����");
				strcpy(quest_text[line++], "���t���̔��������o���̉����炢�̊Ԃɂ������Ă����B");
				strcpy(quest_text[line++], "���Ƃ������Ƃ��낤�B�������ƔR�₵�Ă��܂��ׂ��������̂��B");
			}
			else
			{
				strcpy(quest_text[line++], "���̔������@�g�������ւ��P�����ɗ��邩������Ȃ��B");
				strcpy(quest_text[line++], "�y�b�g�B���W�߂Ėh�q����n�߂邱�Ƃɂ����B");
			}
		}

		break;

	case QUEST_WANTED_SUMIREKO: //�悤�������z���ց@�[��^�̖ʁX �ʂ����ǉ�
		if(pc == CLASS_MAMIZOU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�ؐ�u�v��͂��܂��s������B");
				strcpy(quest_text[line++], "������̒��\�͎҂͊F�ɒǂ��񂳂�Ă��邾�낤�B");
				strcpy(quest_text[line++], "���Ȃ炨�O���Q������Ƃ����B�v");
			}
			if(comp)
			{
				strcpy(quest_text[line++], "�ؐ�u�܂����{�[���ɂ���Ȕ閧���������Ȃ�āB");
				strcpy(quest_text[line++], "���얲���O�ɍs���������ɂ���Ă͈�厖���B");
				strcpy(quest_text[line++], "���̂����ɂ��������ł����B�v");
			}
		}
		else if(pc == CLASS_KOSUZU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�}�~�]�E�u����A�G����݂��Ă����C�ɂȂ������H");
				strcpy(quest_text[line++], "�Ⴄ�̂��B�����c�O����B");
				strcpy(quest_text[line++], "����͂��Ă����A���O����͒��\�͂Ƃ������̂�m���Ă���ȁH");
				strcpy(quest_text[line++], "�����̒��\�͎҂�����������f�r���Ă���B");
				strcpy(quest_text[line++], "�o�������P���|�����Ă��邩������񂩂�C�����邱�Ƃ���B�v");
			}
		}
		else if(pc == CLASS_YUKARI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�}�~�]�E�u�Ȃ񂶂�A���O����܂ł��̏��w���Ɍ��z���Ă���̂��H");
				strcpy(quest_text[line++], "�܂��������E�j��Ƃ����Ă͎~�ނ𓾂񂩁B");
				strcpy(quest_text[line++], "�d�u���͒��X�ɂ��Ă��񂶂Ⴜ�B�v");
			}
		}
		else if(pc == CLASS_MARISA || pc == CLASS_KASEN || pc == CLASS_SHINMYOUMARU || pc == CLASS_SHINMYOU_2
			|| pc == CLASS_BYAKUREN || pc == CLASS_MIKO || pc == CLASS_NITORI || pc == CLASS_KOISHI 
			|| pc == CLASS_KOKORO || pc == CLASS_NUE || pc == CLASS_ICHIRIN || pc == CLASS_MOKOU
			|| pc == CLASS_FUTO || pc == CLASS_REIMU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�}�~�]�E�u�v��ʂ�A���̏��w���͂�����ɗ����悤����ȁB");
				strcpy(quest_text[line++], "���A���z���̋��낵���������Ղ苳���Ă�낤���B�v");
			}
			if(comp && pc == CLASS_REIMU)
			{
				strcpy(quest_text[line++], "�}�~�]�E�u�����Ō�ɑz��O�̂��Ƃ��N�������悤���Ⴊ�A");
				strcpy(quest_text[line++], "���܂�����Ă��ꂽ�炵���́B");
				strcpy(quest_text[line++], "����J��������B���������Ă����B�v");
			}
		}
		break;


		//�؂�D�͂������Ĉ���@������
	case QUEST_HECATIA:
		if(pc == CLASS_MARISA)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�T�O���u���̖������̐��E����߂��Ă��Ȃ��́B");
				strcpy(quest_text[line++], "�ǂ���珃�ς̒��Ԃ����̖��𖲂ɔ���t���Ă���炵����B");
				strcpy(quest_text[line++], "������x���̐��E�Ɍ������A�G��ł��|���Ȃ����B");
				strcpy(quest_text[line++], "�����܂ł��Ȃ����ǁA�������������ɂ͋��ۂ��Ă����ʂ�B�v");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "��J���ނ��ĉƂɖ߂�ƌ��ւɌ��o���̂Ȃ������u����Ă����B");
				strcpy(quest_text[line++], "�ǂ���炱�ꂪ��V�̂���炵���B");
				strcpy(quest_text[line++], "���ꂾ���猎�̘A���͒����D���Ȃ��񂾁B");			
			}
		}
		else if(pc == CLASS_SAGUME)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���̖������̐��E����߂��Ă��Ȃ��B�܂��n��̐l�Ԃ��g���Ď��Ԃ����E���悤�B");
				strcpy(quest_text[line++], "�����l�������Ȃ��͔����̖��@�g���̏��Ɍ��������B");
				strcpy(quest_text[line++], "�����������ɂ����X���̒����ɏ悹���Ă�������]�v�Ȉꌾ���������Ă��܂����B");
				strcpy(quest_text[line++], "���͂⌎�̖����~�����߂ɂ͎��������̐��E�ɍs�������Ȃ��B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "���̖��𖲂̐��E����~�����Ƃɐ��������B");
				strcpy(quest_text[line++], "���̓s�����i�������Ă����B");
			}
		}
		else if(pc == CLASS_TOYOHIME || pc == CLASS_YORIHIME)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���̖������̐��E����߂��Ă��Ȃ��B");
				strcpy(quest_text[line++], "�ǂ���珃�ς̈ꖡ����U�����󂯂Ă���悤���B");
				strcpy(quest_text[line++], "���n��Ŏ��R�ɓ������͂͂��Ȃ��������Ȃ��B");
				strcpy(quest_text[line++], "�ň��ʘH���疲�̐��E�ɍs���A���ς̈ꖡ��œ|���Ȃ���΂Ȃ�Ȃ��B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "���̖��𖲂̐��E����~�����Ƃɐ��������B");
				strcpy(quest_text[line++], "���̓s�����i�������Ă����B");
			}
		}
		else if(pc == CLASS_DOREMY)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���̖����܂����̐��E�ɋ������Ă���B");
				strcpy(quest_text[line++], "�ǂ���珃�ς̒��Ԃ��܂���������񂵂Ă���炵���B");
				strcpy(quest_text[line++], "���̐��E�̊Ď��҂Ƃ��Ď������ꔧ�E�����Ƃɂ����B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�ǂ��ɂ����G��|���A���̖��𖲂̐��E����ǂ��o�����B");
				strcpy(quest_text[line++], "�Ȃ�������������̕i����z���Ă����B");			
				strcpy(quest_text[line++], "���̌��l�����ł܂����������Ă����̂�������Ȃ��B");			
			}
		}
		break;

		//���œ|���C���N�G�X�g
	case QUEST_YUKARI:
		if (pc == CLASS_EIKI)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "���z���ɐ[���Ȉٕς��N�����Ă���B");
				strcpy(quest_text[line++], "�S���ƌĂ΂�鋐��ȃ_���W�������o�����يE�̉��������o�����̂��B");
				strcpy(quest_text[line++], "�S���̒��w�Ŕ��_�����Ή��ɒǂ��Ă���悤�����A");
				strcpy(quest_text[line++], "���낻�날�̎҂ɂ�����������������Ă��˂΂Ȃ�܂��B");
			}
		}

		break;

		//���ΐ��N�œ|���C���N�G�X�g
	case QUEST_TAISAI:
		if(pc == CLASS_SEIJA)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�S���̒ꂩ�狭�͂Ȑ_�������o�����Ƃ��Ă���炵���B");
				strcpy(quest_text[line++], "���Ȃ��͂���𕷂��A���Ƃ��Ă�����𖄂ߒ����Ă�낤�ƌ��ӂ����B");
			}
		}
		else if(pc == CLASS_YUKARI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�A���o�[�̉��̗͂Ō��z���̒n�����S���ƌq�����Ă��܂����B");
				strcpy(quest_text[line++], "���̉e���ł��낤���Ƃ��n���̑��ΐ��N���@��o����Ă��܂����B");
				strcpy(quest_text[line++], "��x��ɂȂ�O�ɖ��ߒ����Ȃ��Ƃ����Ȃ��B");
			}
		}
		else if (pc == CLASS_MEIRIN)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "���z�������ΐ��N�̋��ЂɎN����Ă���I");
				strcpy(quest_text[line++], "���̂Ƃ��̖��͂�͂肱�̗\���������I");
				strcpy(quest_text[line++], "���̎��������őO���Ő��Ȃ���΁I");
			}
		}

		break;

		//�T�[�y���g�œ|���C���N�G�X�g
	case QUEST_SERPENT:
		//v1.1.18 
		if(pc == CLASS_JUNKO || pc == CLASS_HECATIA || pc == CLASS_CLOWNPIECE || pc == CLASS_VFS_CLOWNPIECE)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "����ٕ̈ς̌����́u���ׂ̃T�[�y���g�v�Ƃ������̋���ȑ��݂������B");
				strcpy(quest_text[line++], "���̊C�ɃT�[�y���g�̐��ޗ̈悩��̖傪�J���A���܌��̓s���N�����󂯂Ă���B");
				strcpy(quest_text[line++], "���Ȃ��̓T�[�y���g��|���ɍs���Ă��������A���̓s���łԂ̂��������߂Ă��Ă������B");
			}
		}

		break;

		//�N�G�X�g40-48�̓����N�G
		//�N�G�X�g49�͋󂫁H

		//�N�G�X�g50,51�͖�������p

		//�p�m�ق̕󕨌�
	case QUEST_VAULT:
		if(CLASS_IS_PRISM_SISTERS)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���Ȃ��͊ق̑��ɌÂ���������Ă��邱�Ƃ��v���o�����B");
				strcpy(quest_text[line++], "����ٕ̈ς͎苭���`���ɂȂ肻�������A");
				strcpy(quest_text[line++], "���ꂪ����Ώ����ɂȂ邩������Ȃ��B�܂����ĉ�����ɍs�����B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�\�z�ȏ�̓�s���������A�Ƃ���������̉���ɐ��������B");
			}
			else
			{
				strcpy(quest_text[line++], "����̉���Ɏ��s�����B");
				strcpy(quest_text[line++], "�ٕς̉e���Ȃ̂����������̉e���Ȃ̂��킩��Ȃ����A");
				strcpy(quest_text[line++], "���΂炭���Ȃ������ɑ��͖����ɂȂ��Ă����B");
			}

		}
		break;

	case QUEST_KAGUYA: //�܂̓��@�i��
		if(pc == CLASS_EIRIN)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�P��u�˂��i�ԁA�_���W�����̉��ɂ͂��̓�肪�{���ɂ��邩������Ȃ���B");
				strcpy(quest_text[line++], "�ꏏ�ɒT���ɍs���܂��傤��B�v");
			}
		}

		break;
	case QUEST_SHELOB: //�V�F���u�ގ��@�S�A���}��
		if(pr == RACE_ONI)
		{
			if(comp)
			{
				strcpy(quest_text[line++], "���}���u���̘r���߁A���΂͋S���˂��B");
				strcpy(quest_text[line++], "�������Ă��B���ړ��Ă͂��ꂾ�낤�H�v");//��s40���܂�
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���}���u���₨��S�Ƃ����낤�҂��ǂ��������Ă̂��B");//��s40���܂�
				strcpy(quest_text[line++], "�������ł����񂾂̂����H");//��s40���܂�
			}
		}
		else if(pc == CLASS_YAMAME)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���Ȃ��͍ŋߋߏ��ɏZ�ݒ��������\�҂̒w偂ɉ䖝���Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "�V�F���u�ƌĂ΂�邻�̒w偂͌��̋C�ƐH���C�������ɂȂ��悤���B");
				strcpy(quest_text[line++], "�����҂��W�܂�n��Ƃ����ǒʂ��ׂ��؂͂���B�����̗��V�������Ă�낤�B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "�]���҂͔��X�̑̂łǂ����֓����Ă������B");
				strcpy(quest_text[line++], "�w偂̓Őj���n�ʂɗ����Ă���̂��������B");
				strcpy(quest_text[line++], "��������H����΋��͂ȕ��킪�o���邾�낤�B");
				strcpy(quest_text[line++], "���Ȃ��͑�����ƂɎ��|�������E�E");
			}
			else
			{
				strcpy(quest_text[line++], "�]���҂𑊎�ɕs�o������Ă��܂����B");
				strcpy(quest_text[line++], "�����͒p��E��Œn��̍r����҂����𗊂邵���Ȃ����낤�E�E");
			}

		}
		break;

		//�V���p�@�h���΍��C��
	case QUEST_TYPHOON:

		if (pc == CLASS_MEGUMU)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "��N�ɂȂ��\�����l���ɋ߂Â��Ă���B");
				strcpy(quest_text[line++], "����͑䕗�ƌĂ΂��O�E�̗d�����B");
				strcpy(quest_text[line++], "����Ɋ�Â������閧���Ɍ��ނ��Ȃ���΂Ȃ�Ȃ��B");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "�閧�C���͖��Ȃ����������B");
				strcpy(quest_text[line++], "�����ƍ����ȓ��݂��F�Ɏx�����Ă�邾���̗\�Z���m�ۂł����B");
			}
			else
			{
				strcpy(quest_text[line++], "�l���ɔ�Q���o�����Ԃ�Ƃ��Ă��܂����B");
				strcpy(quest_text[line++], "���̑�V��ɂ����͂��Ă��炢�ǂ��ɂ����Ȃ��𓾂����A");
				strcpy(quest_text[line++], "����S�N���炢�͉A��������ꂻ���ł���B");
			}

		}
		break;


		//�}������N�G�@���ʂ�\��o�����ꏈ��
	case QUEST_TORRENT:
		{
			if(comp)
			{
				int i;
				int j=0;
				cptr name_list[3];
				bool flag = FALSE;

				//���X�g�ɋL�^���ꂽ�^�[�����Ƌ����ď��ʂ����߂�
				for(i=0;i<3;i++)
				{
					//���Ɠ����l�̓��X�g�����΂�
					if(torrent_quest_score_list[j].class_id == p_ptr->pclass || torrent_quest_score_list[j].class_id2 == p_ptr->pclass )
						j++;
					//�����܂������N�C�����ĂȂ��ꍇ���X�g�̃^�[���Ɣ�r�A���������̖��O�����X�g�ɓ����
					if(!flag && (int)qrkdr_rec_turn < torrent_quest_score_list[j].turn)
					{
						name_list[i] = player_name;
						flag = TRUE;//���܂����t���O
						qrkdr_rec_turn = 1 + i;//�L�^�^�[���̕ϐ����u���ʂ��������v�ɕς���
					}
					else
					{
						name_list[i] = torrent_quest_score_list[j].name;
						j++;
					}
				}

				strcpy(quest_text[line++], "���̌��ʂ�����o����Ă���B");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], format("�D���@�F%s",name_list[0]));
				strcpy(quest_text[line++], format("���D���F%s",name_list[1]));
				strcpy(quest_text[line++], format("��O�ʁF%s",name_list[2]));

			}

		}
		break;

		//���@�q�_���_�ގ�
	case QUEST_MORIYA_1:
		if(pc == CLASS_KANAKO)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�R�̒����̑��寂̒r���q�_���_�ɏP���Ă���B");
				strcpy(quest_text[line++], "�����Ă����Ă����寂����Ƃ����邾�낤���A");
				strcpy(quest_text[line++], "�����͎��݂��ĉ��𔄂��Ă������B");
			}
			else if(comp)
			{
				strcpy(quest_text[line++], "���寂̒r�ɕ��Ђ�u�����Ƃɐ��������B");
				strcpy(quest_text[line++], "�͂������l������M���W�܂��Ă���̂�������B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "���̂悤�Ȋi�̒Ⴂ�_��ɕs�o�����Ƃ͏�Ȃ��B");
				strcpy(quest_text[line++], "�×����A���������Ă͐�͏o���Ȃ����̂��B");
			}
		}
		if(pc == CLASS_SANAE || pc == CLASS_SUWAKO)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�_�ގq�u�R�̒����̑��寂̒r���q�_���_�ɏP���Ă����ł����āB");
				strcpy(quest_text[line++], "�����͎��݂��Ă����āA����悭�΂���[�����Ђɂ��ė��Ă�B�v");
			}
		}
		break;

		//�̗p�����񎟕�W�@�B��ޑœ|�N�G
	case QUEST_OKINA:
		//�얲�A�������A���@�@�`���m�@�@�����@�@��ʓV��@�@�ŃZ���t�ς���K�v�����邩
		if(pc == CLASS_REIMU || pc == CLASS_MARISA || pc == CLASS_AYA)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�G�߈ٕς̎�ƂƂ̐킢�͓��˂ɏI����Ă��܂����B");
				strcpy(quest_text[line++], "���Ȃ��͏����𐮂��čĐ�ɒ��ނ��Ƃ����ӂ����B");
				strcpy(quest_text[line++], "�ǂ�����΋����r����h�����Ƃ��ł��邾�낤�B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�܂����s�o������Ă��܂����B");
				strcpy(quest_text[line++], "��Δ�_�̗͂͂��܂�ɂ�����ł������B");
			}
		}
		else if(pc == CLASS_CIRNO)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�֎q�ɍ������̂����ȓz�ɓ������Ă��܂����B");
				strcpy(quest_text[line++], "���������T���Ă���ƁA���o���̂���n�����������B");
				strcpy(quest_text[line++], "�����ɓˌ�����΂܂����̏ꏊ�ɍs���邩������Ȃ��B");
			}
			else if(fail)
			{
				strcpy(quest_text[line++], "�Ȃ������񂺂�G��Ȃ������B");
				strcpy(quest_text[line++], "����ς肠��͈֎q�ɍ����Ă����̂Ƃ͕ʂ̓z�ɈႢ�Ȃ��B");
			}
		}
		else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�`���m�u�����̔w���ɔ����J���Ă���ȁB");
				strcpy(quest_text[line++], "���ɓ���΂܂��w�������[�̂Ƃ���֍s���邼�B�v");
				strcpy(quest_text[line++], "�s�[�X�u����Ȃ炠�̔�_�ɂ�����Ɨp������񂾁B");
				strcpy(quest_text[line++], "�݂�ȕt�������Ă���Ȃ��H�v");
				strcpy(quest_text[line++], "�T�j�[�u��[���A�˓���I�v");
			}
			else if (fail)
			{
				strcpy(quest_text[line++], "���i�u���A���ʂ��Ǝv������c�c�v");
				strcpy(quest_text[line++], "�`���m�u���������ȁH�O�͉��Ƃ��Ȃ����̂ɁB�v");
				strcpy(quest_text[line++], "�s�[�X�u�����A�{�X�ɓ{��ꂿ�܂��c�c�v");
			}
			else
			{
				strcpy(quest_text[line++], "�����o�u������������Ȃɋ����Ȃ��Ă��Ȃ�Ė��݂����ˁB�v");
				strcpy(quest_text[line++], "�X�^�[�u���J��������������B�F�̕󕨂ɂ��܂��傤�I�v");
				strcpy(quest_text[line++], "�s�[�X�u���ł��V�тɗ������Č����Ă��ꂽ���A�C�����������I�v");



			}
		}
		else if(pr == RACE_HAKUROU_TENGU || pr == RACE_KARASU_TENGU)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���T�u������Ɓ[�A��������܂��V������������C�H�v");
				strcpy(quest_text[line++], "���u�ӂ��A�\�z�O�͐l���̍ō��̃X�p�C�X���I�v");
			}
		}
		else if(pr == RACE_FAIRY)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "���T�u������Ɓ[�A��������܂��d�������������C�H�v");
				strcpy(quest_text[line++], "���u�ӂ��A�\�z�O�͐l���̍ō��̃X�p�C�X���I�v");
			}
		}
		else if(pc == CLASS_NARUMI)
		{
			if(accept)
			{
				strcpy(quest_text[line++], "�v���Ԃ�ɐX�ɋA������A���o���̂Ȃ��������ȓ�l�g������B");
				strcpy(quest_text[line++], "��l�͂��Ȃ����X�J�E�g�������ƌ����o�����B");
				strcpy(quest_text[line++], "���Ȃ��͂��̓�l�̑f���ɐS�����肪�������E�E�E");
			}
		}		
		break;
		//v1.1.36 ���鎸�H�@�Ō�̉���ɏo���ʎq�̎��s�Z���t�ύX�@���ƈ�l���ꂩ�������l�����邪
	case QUEST_KOSUZU:
		if(pc == CLASS_REIMU || pc == CLASS_MARISA || pc == CLASS_AYA || pc == CLASS_MAMIZOU
			|| pc == CLASS_SANAE || pc == CLASS_YUYUKO || pc == CLASS_YOUMU || pc == CLASS_REMY
			 || pc == CLASS_SAKUYA	)
		{
			if(fail)
			{
				strcpy(quest_text[line++], "�����u����̉���ł͂����b�ɂȂ�܂����B");
				strcpy(quest_text[line++], "�ЂƂ܂��ۂ����܂��ĉ����Ƃ����Ƃ���ł��傤���B");
				strcpy(quest_text[line++], "����ɂ��Ă�����̐悪�v��������c�v");
			}
		}
		else if (pr == RACE_FAIRY)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�����u�˂��A�ݖ{���̏�����ǂ����Ō��Ȃ������H");
				strcpy(quest_text[line++], "�@���܍s���s���ɂȂ��ĂĊF�ŒT���Ă�́B");
				strcpy(quest_text[line++], "�@�ł���΂��F�B�ɂ������Ă݂Ăق�����B�v");
			}
			if (comp)
			{
				strcpy(quest_text[line++], "�����u�����A����Ȏ��ɂȂ��Ă����Ȃ��...");
				strcpy(quest_text[line++], "�@�d���̂��Ȃ����A��߂��Ă��ꂽ�͙̂F��Ƃ������̂ˁB");
				strcpy(quest_text[line++], "�@���̃����b�N�͂����邩��ł���΂��܂�\�ɂ��Ȃ��ł����āB�v");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�@�@�@�@�@�@�@�@(�d���̊댯�x������C�������ق�������������...)");

			}
			if (fail)
			{
				strcpy(quest_text[line++], "�����u�_�Ђŏ���ɏP��ꂽ�ł����āH");
				strcpy(quest_text[line++], "�@���肪�Ƃ��A�����ɕی삳�����B");
				strcpy(quest_text[line++], "�@���邪���f�����Ă��߂�Ȃ����ˁB");
				strcpy(quest_text[line++], "�@�ł����̂��Ƃ͂ł���Δ閧�ɂ��Ă����āH�v");

			}

		}



		break;

		//���̐��E�́��œ|�N�G�@v1.1.52 俎q���ꐫ�i�Z���t�ύX
	case QUEST_DREAMDWELLER:
		if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�B��ށu���ɂ��O�ɂƂ��čő�̈����Ƃ̑Ό��̎��������B");
				strcpy(quest_text[line++], "�����ł������Ă݂���I�������]�����Ă����Ȃ�I�v");
			}
			else if (comp)
			{

				strcpy(quest_text[line++], "�B��ށu�V����ł���I�悭���������g�ɑł��������I");
				strcpy(quest_text[line++], "���āA���̂܂ܖڊo�߂�Ƃ��O�ƌ��z���Ƃ̉��͐؂�Ă��܂����낤�B");
				strcpy(quest_text[line++], "�����Ŏ�����˂̗͂��g���Ă��O�ƌ��z���̌q������ێ����Ă�낤�B");
				strcpy(quest_text[line++], "����ł��O�͖������ɖ{�����I");
				strcpy(quest_text[line++], "");
				strcpy(quest_text[line++], "�Ƃ���ŁA�ǂ������玄�̏��œ�������͂Ȃ��H�v");

			}
			else if (fail)
			{
				strcpy(quest_text[line++], "�h���~�[�u���Ȃ��͂����������瓦����܂���B");
				strcpy(quest_text[line++], "���z�Ɂc�c���������āc�c�v");
			}

		}
		break;

		//�V�̑f�Ă��[���N�G
	case QUEST_DELIVER_EEL:
		break;


		//���ۍ��@���_��腖��̂Ƃ�������ƃZ���t�ς��� �v�̉̂̂Ƃ����ς���
	case QUEST_SHORELINE2: 

		if (pc == CLASS_KUTAKA)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�v���Ԃ�ɐE��ɖ߂��Ĉꑧ���Ă���ƁA��i����ً}�̌Ăяo�����󂯂��B");
				strcpy(quest_text[line++], "�p���͐��ۍ��̍Ĕ��߂ł������B");
				strcpy(quest_text[line++], "���̓����삪���x�͒n���̕ʂ̃G���A�ւ̐N�U����ĂĂ���炵���B");
				strcpy(quest_text[line++], "�n���̑�66�K�w�֌��������B�v");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "�����ȋ������ւ�w鋋� ���S�x��œ|�����B");
				strcpy(quest_text[line++], "���̌��тɂ��Վ��{�[�i�X���o�邪�A");
				strcpy(quest_text[line++], "�c�O�Ȃ���\�Z�̓s���Ō����x���ƂȂ����B");

			}
		}
		else if (pr == RACE_DEATH || pc == CLASS_EIKI)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�v�̉́u���̓����삪���x�͒n���̕ʂ̃G���A�ւ̐N�U����ĂĂ��܂��B");
				strcpy(quest_text[line++], "���Ԃ��d��������w���͐��ۍ��̍Ĕ��߂ɓ��ݐ؂�܂����B");
				strcpy(quest_text[line++], "��̋󂢂Ă�����͕������߂��邢�͓����Ɍ������悤�ɂƂ̂��Ƃł��B�v");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "�v�̉́u�ޏ����悤�₭���肽�悤�ŁA");
				strcpy(quest_text[line++], "�����͒{���E�̎x�z�ɒ��͂������̂悤�ł��B");
				strcpy(quest_text[line++], "�܂��{���E�����̂��ƂȂ炱����̊֒m���邱�Ƃł͂���܂����ˁB");
				strcpy(quest_text[line++], "�M���ւ̗Վ��{�[�i�X���o�Ă��܂��̂ł�����Ɏ�̈�����肢���܂��B�v");

			}
		}
		break;

		//�j�ŃN�G3 �ݕP�˗��ŃT�C�o�[�f�[�����ގ�
	case QUEST_DOOM3:
		if (pr == RACE_HANIWA) //v1.1.66
		{
			if (comp)
			{
				strcpy(quest_text[line++], "�ݕP�u�O�̐��E�ł͂���ȋ����ȕ��킪�g���Ă���̂ˁB");
				strcpy(quest_text[line++], "�@��������ł��|���Ă���Ƃ́A���Ȃ��̒������������v����B");
				strcpy(quest_text[line++], "�@���āA�����J������炳�Ȃ��ƂˁB");
				strcpy(quest_text[line++], "�@���̐V�삾�����񂾂��ǋM���ɂ������B�v");

			}
			else if (fail)
			{
				strcpy(quest_text[line++], "�ݕP�u�O�̐��E�ł͂���ȕ��킪�g���Ă���̂��B");
				strcpy(quest_text[line++], "�@���͂����ǔ������Ɍ����Ă����ˁB");
				strcpy(quest_text[line++], "�@�}���Ŗh���\���̊J����i�߂܂��傤�B");
				strcpy(quest_text[line++], "�@�M���͐w�`�̌����������Ȃ����B");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�@����ɂ��Ă����̒{���ǂ��A");
				strcpy(quest_text[line++], "�@����Ȃ��̂��Ăэ���Ŏ�j����������ł���̂�����H�v");

			}
		}

		break;

		//v1.1.65 �H�ߍ��� ����푰��p�@���j�[�N�N���X�̂Ƃ������䎌��ς��Ă���
	case QUEST_KONNKATSU: 
		if (cp_ptr->flag_only_unique)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�V�q�u���傤�ǂ����Ƃ���ɗ����ȁB������Ɨ��܂�Ē��ՁB");
				strcpy(quest_text[line++], "���͍����Ɏ��s���ĉH�߂𖳂������Ԕ����ȓV�������ĂˁB ");
				strcpy(quest_text[line++], "�����ɋ����t����ĕ����Ă����Ȃ��Ȃ����ߋ肪���ɗ���ł����񂾁B ");
				strcpy(quest_text[line++], "�ł��z�؂�ꖇ�̂��߂ɒn���n���T�����Ȃ�Ď��͂܂��҂炾����A ");
				strcpy(quest_text[line++], "�����݂̗L�肻���ȓz��T���Ă����Ă킯�B ");
				strcpy(quest_text[line++], "���������킯�ŉH�߂��ꖇ�T���Ă��Ȃ����B ");
				strcpy(quest_text[line++], "���R���񂶂傻����̉H�߂���ʖڂ�B ");
				strcpy(quest_text[line++], "�V�E�ɂӂ��킵���y���ʐ��z�̉H�߂���Ȃ��ƂˁB�v ");
			}
		}
		break;
	case QUEST_MIYOI: //v1.1.78 �h���[���C�[�^�[�̐��Ё@�h���~�[
		if (pc == CLASS_DOREMY)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�������������B�̎��L�����͐H�זO���܂����B");
				strcpy(quest_text[line++], "�����Ȃ��͐_�Ђ̉���ɎQ�����Ĉ��Y�҂𒦂炵�߂邵������܂���B");
				strcpy(quest_text[line++], "�S�̎��̂����炷���e�͋����ɂ܂�Ȃ��ł����A");
				strcpy(quest_text[line++], "�����������܂������߂Ώ����邩������܂���ˁB");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "�����Ǝv�����͂̎g����͂ǂ����ɏ����Ă��܂��܂����B");
				strcpy(quest_text[line++], "�ǂ����S�̕Z�\�̒��ɓ������񂾂悤�ł����A");
				strcpy(quest_text[line++], "�Ƃ肠���������a�����Ă����Έ����͂ł��Ȃ��ł��傤�B");
			}
			else if (fail)
			{

				strcpy(quest_text[line++], "�����̉����ɒx������Ƃ͖��̎x�z�҂̖��܂�ł��B");
				strcpy(quest_text[line++], "���̂܂܂ł͐����Ŋ̑�������Ă��܂��܂��c�c");

			}

		}
		break;
	case QUEST_HANGOKU1://v1.1.84 �A�����������T�@���E��
		if (pc == CLASS_CHEN || pc == CLASS_RAN)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�w�������x�ƌĂ΂��i�v�ߐl�̑剅��̍s���͝��Ƃ��Ēm��Ȃ��B");
				strcpy(quest_text[line++], "����������ƍĂі������𑀂��đ��̒N���Ɏ��߂��@����f���Ă���̂����B");
				strcpy(quest_text[line++], "�_���W�����Ŗ�����������������L���Ă��܂����B");
				strcpy(quest_text[line++], "�������������������Ă�����哖���肾�B");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "�ʖڂŌ��X���������A�������������������ē|����̎���D�����B");
				strcpy(quest_text[line++], "�n�ꂩ��̎g���Ɉ����n���ĕԗ���󂯎�����B");
				strcpy(quest_text[line++], "����ŕЂ����Ƃ����̂����B");
			}
			else if (fail)
			{

				strcpy(quest_text[line++], "���Ȃ��̓_���W�����ł̑{������߂��B");
				strcpy(quest_text[line++], "��͂肠�����ςȑ剅�삪����ȂɊȒP�Ɍ�����͂��͂Ȃ��B");

				//��������|���ăh���b�v�A�C�e���́�����������Ă���ꍇ�ǉ����b�Z�[�W
				if (a_info[ART_HANGOKU_SAKE].cur_num)
				{
					strcpy(quest_text[line++], "�@");
					strcpy(quest_text[line++], "����ɂ��Ă����������瓪���ɂ����������悤�ȋC������B");
					strcpy(quest_text[line++], "���̒��q�������̂��낤���H");

				}

			}

		}
		break;

	case QUEST_GEIDON_HUMAN:

		break;

	case QUEST_GEIDON_KWAI:
		if (pc == CLASS_SUIKA || pc == CLASS_MAMIZOU)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "���ɕ������q�K�������s���Ȓm�点���悱���Ă����B");
				strcpy(quest_text[line++], "�O�E���痈���Ǝv���鋥���ȗd�������ɐN�������炵���B");
				strcpy(quest_text[line++], "��Q�҂��o��O�ɑ��₩�Ɏn�����Ȃ��Ƃ����Ȃ��B");
				strcpy(quest_text[line++], "���̗��͉�X�̓꒣��Ȃ̂�����B");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "��Q���o��O�Ɍ��̘A����ގ����邱�Ƃɐ��������B");
				strcpy(quest_text[line++], "�ڌ��҂��o�Ă��܂����悤�����A�q�K�������g���Č떂�����Ă������B");
			}
			else if (fail)
			{

				strcpy(quest_text[line++], "��Ȃ����ƂɊO�G�ɒx�������Ă��܂����B");
				strcpy(quest_text[line++], "���܂��ܒʂ肪�������얲��������΂��Ă��ꂽ���A");
				strcpy(quest_text[line++], "�����͓����オ�肻���ɂȂ�...");


			}

		}
		
		break;

	case QUEST_MORIYA_2:
		if (pc == CLASS_SANAE)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�s��̐_�Ǝ��_�Ћ��ẪA�r���e�B�J�[�h������J�Â��ꂽ�B");
				strcpy(quest_text[line++], "�S���C���i�܂Ȃ����A�������_�Ђ̕��j�Ƃ��ĎQ�����Ȃ��킯�ɂ͂����Ȃ��B");
			}
			else if (fail)
			{
				strcpy(quest_text[line++], "�疒�u�ǂ��ɂ��Q���҂̏W�܂肪������ˁB����Ȃɔ����̋�ԂȂ̂ɁB�v");
				strcpy(quest_text[line++], "�_�ގq�u�ӂ��ށA�ۑ����Ƃ��͔����Ƃ����ɂ͏����l�דI�������̂����B");
				strcpy(quest_text[line++], "�@���͊Ԍ���n���Z���^�[�̗n��G���A�ŊJ�Â���Ƃ����̂͂ǂ�������H�v");
				strcpy(quest_text[line++], "�疒�u�����I����ς肠�Ȃ��͎s��𕪂����Ă����ˁI�v�v");
			}
		}
		else if (pc == CLASS_KANAKO || pc == CLASS_SUWAKO)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�s��̐_�Ǝ��_�Ћ��ẪA�r���e�B�J�[�h������J�Â��ꂽ�B");
				strcpy(quest_text[line++], "��͂�_�Ђ̍Ր_�Ƃ��Ď���������o���ׂ����낤�B");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "�Ñ�̍Ղ���v���o���Č��������A�Ȃ��Ȃ��y�����Â��ł������B");
				strcpy(quest_text[line++], "�������Ȃ������܂�M�������������͂��Ȃ��B");
			}
			else if (fail)
			{
				strcpy(quest_text[line++], "���Ȃ��͌������ɒH�蒅���Ȃ������B");
				strcpy(quest_text[line++], "����ł͎�Î҂Ƃ��ė��ꂪ�Ȃ��B�M���S�R�����Ȃ�����...");
			}
		}

		break;

	case QUEST_REIMU_ATTACK:

		if(pc == CLASS_KASEN)
		{
			if (fail)
			{
				strcpy(quest_text[line++], "�얲�͎R���̃A�W�g��j�󂵂ċ����Ă������B");
				strcpy(quest_text[line++], "���X�̌����������ɂ���ȏ�A�R������������Ă����킯�ɂ������Ȃ��B");
				strcpy(quest_text[line++], "�x����\���o���炢�������ӂ��ꂽ���A");
				strcpy(quest_text[line++], "�ޏ������̖ڂ�^���������邱�Ƃ��ł��Ȃ�����...");
			}
		}
		else if (pc == CLASS_SANNYO)
		{
			if (comp)
			{
				strcpy(quest_text[line++], "�����ˁu���₠����������B�A�W�g��䖳���ɂ����Ƃ��낾�����B");
				strcpy(quest_text[line++], "���Ⴀ��̒ʉ݂��g���ĕ�V��n�����B");
				strcpy(quest_text[line++], "���ꂩ�������������ŗ��ނ�B");
			}

		}
		else if (pc == CLASS_TAKANE)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "����̛ޏ����A");
				strcpy(quest_text[line++], "�u���̉R�f���ǂ���S�ł����Ȃ��ƋC���ς܂Ȃ���I�v��");
				strcpy(quest_text[line++], "�{�苶���ď�荞��ł����B");
				strcpy(quest_text[line++], "�����Ȃ񂾂�������Ȃ����A�܂��͌}�����Ȃ��Ƃ����Ȃ��I");
			}
			else if (comp)
			{
				strcpy(quest_text[line++], "�ޏ��͂��ꂱ�ꕶ��������Ȃ���A���Ă������B");
				strcpy(quest_text[line++], "�����炪�d����������āA�S�����f�Șb�ł���B");
			}
			else if (fail)
			{
				strcpy(quest_text[line++], "�ޏ��̓A�W�g�𔼉󂳂������ɂ悤�₭�A���Ă������B");
				strcpy(quest_text[line++], "��X����̉��������Ƃ����̂��H");
			}
		}
		else if (pr == RACE_FAIRY)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�����ˁu�Ȃ񂾗d�����B���⍡�͗d���̎���؂肽���B");
				strcpy(quest_text[line++], "�@������݂�ȂŔ���얲�ƒe�������������񂾁I");
				strcpy(quest_text[line++], "�@�p���[�A�b�v�A�C�e���͎��������H�悵�˂����߁I�v");
			}
		}

		break;


	case QUEST_YAKUZA_1:

		//���̃N�G�X�g�͏������͂���₱�����̂Ŏ�̎��E�������E���s���ł܂�������
		if (accept)
		{
			//������߈ˎ�
			if (p_ptr->muta4 & (MUT4_GHOST_WOLF | MUT4_GHOST_EAGLE | MUT4_GHOST_OTTER | MUT4_GHOST_HANIWA))
			{
				strcpy(quest_text[line++], "�{���E�̓����삪�����̒r�n���ōR�����n�߂邻�����B");
				strcpy(quest_text[line++], "�������ǂ��������Ƃ��낤�H");
				strcpy(quest_text[line++], "�Ȃ������̐��͂ɏ��������Ȃ���΂����Ȃ��C�����Ďd�����Ȃ��I");
			}
			//���S		
			else if (pc == CLASS_SAKI)
			{
				strcpy(quest_text[line++], "�ŋߌ������Ȃ��b�W�ޖ��̍s���Ɋւ�����񋟎҂����ꂽ�B");
				strcpy(quest_text[line++], "�ޖ��͋����̒r�n���Ɉ����U�����Ă������Ɨ͂�~���Ă���炵���B");
				strcpy(quest_text[line++], "���������R��|���ɍs�����B");
			}
			//����d
			else if (pc == CLASS_YACHIE)
			{
				strcpy(quest_text[line++], "�ŋߌ������Ȃ��b�W�ޖ��̍s���Ɋւ�����񋟎҂����ꂽ�B");
				strcpy(quest_text[line++], "�b�W�͋����̒r�n���Ɉ����U�����ĐΖ����Â��Ă���炵���B");
				strcpy(quest_text[line++], "�b�W�ɗ͂�~������͍̂D�܂����Ȃ�����������X����ɂ���ΗL���ɂȂ�B");
				strcpy(quest_text[line++], "���u�����͂Ȃ����낤�B");
				strcpy(quest_text[line++], "���������炭���̎҂͑��̑g�D�ɂ����l�ɏ���񋟂��Ă���B");
				strcpy(quest_text[line++], "�ň��l�b�̍R���ɂȂ邾�낤�B���O�ɏ��������ėՂ܂˂΂Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "�Ƃ��ɐΖ��͂悭�R����̂ŉΉ��΍􂪏d�v���B");
				strcpy(quest_text[line++], "�b���Ƃ��Čق������͂ȉΗ���A��čs�����B");
			}
			//�ޖ�
			else if (pc == CLASS_YUMA)
			{
				strcpy(quest_text[line++], "�悤�₭�������������̒r�n���ɒ{���E�̘A��������������������Ă����B");
				strcpy(quest_text[line++], "���͂ȉΗ��������A�ꂽ���͂܂ł���悤���B");
				strcpy(quest_text[line++], "���������������݂��݂��Ζ��𖾂��n���̂͂��܂�ɂ��ɂ����B");
				strcpy(quest_text[line++], "�s�{�ӂ����h�q������˂΂Ȃ�Ȃ��B");
			}
			//����
			else if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
			{
				strcpy(quest_text[line++], "�{���E�̓����삪�����̒r�n���ōR�����n�߂�Ƃ̏�񂪓͂����B");
				strcpy(quest_text[line++], "�����ɍs���ē������|�����Ȃ���΂Ȃ�Ȃ��B");
				strcpy(quest_text[line++], "���G�̏��֕��c�̗͂�������x�������Ă�낤�B");
			}
			//�ق���ʓ�����@�������͂ŕ���
			//v2.0.11 �V����3�l���������b�Z�[�W��\�����邽�ߕ����ς���
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
			{
				strcpy(quest_text[line++], "���S�u���x�����Ƃ���ɗ����ȁI������R���ɍs�����I");
				strcpy(quest_text[line++], "�s����͋����̒r�n�����B�ޖ��̓z�������U�����ĉ������Ă���炵���I");
				strcpy(quest_text[line++], "�������������o���ڋ��҂��R�|���Ă��`�����X���I�݂�Ȏ��ɑ����I�v");
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
			{
				strcpy(quest_text[line++], "����d�u�ً}�̔C����I");
				strcpy(quest_text[line++], "�����̒r�n���ŐΖ��Ƃ����L�p�Ȏ�������ʂɔ������ꂽ�B");
				strcpy(quest_text[line++], "���݂͍��~�����Ɏx�z����Ă��āA���u����킯�ɂ͂����Ȃ��B");
				strcpy(quest_text[line++], "���̒n��D�悷�ׂ��}���ŕ��͂��W�߂Ă��邯�ǁA");
				strcpy(quest_text[line++], "����g��쒷������X�Ɠ������Ƃ��l���Ă���݂����B");
				strcpy(quest_text[line++], "�����炭���R���ɂȂ邩�炠�Ȃ�������ɉ�����āB");
				strcpy(quest_text[line++], "���ƐΖ��͂悭�R���邩��Ζ��̏�ł͉��U���̈З͂�������B");
				strcpy(quest_text[line++], "���͂ȉΗ����ق��ĘA��čs�����犪�����܂�Ȃ��悤�ɗ�������ĂˁB�v");

			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
			{
				strcpy(quest_text[line++], "�n�ʂ��畷������ޖ��̐��u�������O�B�����ɒn���̋����̒r�n���܂ŗ����B");
				strcpy(quest_text[line++], "��X�̐Ζ���_���đ����̘͂A�����U�߂Ă����B");
				strcpy(quest_text[line++], "�ǂ��]�Ԃɂ��돭���͓G�ɏo���������Ȃ���Η����ɐH�����ނ��Ƃ��ł���B");
				strcpy(quest_text[line++], "���O��������ĂȂ�ׂ��G�̐������点�B");
				strcpy(quest_text[line++], "�Ƃ��ɓG�̉��R�̉Η������B�Ή��΍�����Ă����B");
				strcpy(quest_text[line++], "����ɂ��Ă��h�q��ȂǑS�����ɍ����B������������...�v");
			}
			else if (pr == RACE_ANIMAL_GHOST)
			{
				strcpy(quest_text[line++], "�{���E�̓����삪���̉��̋����̒r�n���ōR�����n�߂�悤���B");
				strcpy(quest_text[line++], "�����ɂ͊֌W�̂Ȃ��b�����A�ǂ����ɖ������ĉ��𔄂��Ă������B");
				strcpy(quest_text[line++], "���邢�͑S���|���Ă��܂��Ύ��ɒɉ����낤�B");
			}

			//�ق���ʏ���
			else if (pr == RACE_HANIWA)
			{
				strcpy(quest_text[line++], "���|�u������ǂ��������̒r�n���ōR�����n�߂�Ƃ̏�񂪓�������I");
				strcpy(quest_text[line++], "�l�ԗ���s����̂ɖO�����炸�{���E�̊O�ɂ܂Ő�Ђ��L���悤�Ƃ͋����������I");
				strcpy(quest_text[line++], "���B�������Ɍ������A�z����|�����܂��傤�I�v");
			}
			//����ȊO
			else
			{
				strcpy(quest_text[line++], "��u�Ȃ�œ�����Ƃ����Ƃ����ւ̌Q�ꂪ");
				strcpy(quest_text[line++], "���܂��Ȃ���ܔM�n�������낼��ʂ蔲���čs���́H");
				strcpy(quest_text[line++], "�����������Ƃ��̂ɔ�ꂽ��[�I�v");
				strcpy(quest_text[line++], "�@");
				strcpy(quest_text[line++], "�Ӂu�����̒r�n���Œ{���E�̘A�����R���������n�߂�݂������˂��B");
				strcpy(quest_text[line++], "�܂����������܂ŕ����Ă����Ȃ�B");
				strcpy(quest_text[line++], "�ł��r�Ɏ��M������Ȃ�ЂƖ\�ꂵ�ĉ������ɉ��𔄂�邩������Ȃ��ˁB");
				if (pc == CLASS_SATORI)
					strcpy(quest_text[line++], "���Ƃ�l�������ɍs���Ă݂܂����H�v");
				else if (pc == CLASS_KOISHI)
					strcpy(quest_text[line++], "...��H���܂����ɒN�������悤�ȁH�v");
				else if (pc == CLASS_ORIN)
					strcpy(quest_text[line++], "�����������傢�ƌ����ɍs�������ȁH�v");
				else if (pc == CLASS_UTSUHO)
					strcpy(quest_text[line++], "���Ȃ炿�傢�ƌ����ɍs���Ă݂邩���H�v");
				else if (p_ptr->psex == SEX_MALE)
					strcpy(quest_text[line++], "�����̂��Z������s���Ă݂邩���H�v");
				else
					strcpy(quest_text[line++], "�����̂��o������s���Ă݂邩���H�v");

			}
		}
		//������ �����������͂ŕ���
		else if (comp)
		{

			if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KEIGA)
			{
				if (pc == CLASS_SAKI)
				{
					strcpy(quest_text[line++], "�{���E�̑����͂�S�ďR�U�炵�A���ɋC���������I");
					strcpy(quest_text[line++], "����ɏ��񋟎҂���ʔ����i�����コ�ꂽ�B");
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
				{
					strcpy(quest_text[line++], "���S�u�������I��͂�Ō�ɏ��͖̂\�͂��I");
					strcpy(quest_text[line++], "���O�������I�ȋ������������I�J���ɂ������낤�I�v");
				}
				else
				{
					strcpy(quest_text[line++], "���S�u�������I��͂�Ō�ɏ��͖̂\�͂��I");
					strcpy(quest_text[line++], "���O�������I�ȋ������������I�J���ɂ������낤�I");
					strcpy(quest_text[line++], "�Ƃ���ř���g�ɓ���C�͂Ȃ����H�v");
				}
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_KIKETSU)
			{
				if (pc == CLASS_YACHIE)
				{
					strcpy(quest_text[line++], "�l�b�̍R���𐧂��邱�Ƃɐ��������B");
					strcpy(quest_text[line++], "�n��̐_�B���F�X�ƌ������������Ă����̂ł��܂����v�������o���Ă�낤�B");
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
				{
					strcpy(quest_text[line++], "����d�u�f���炵���B�v���Ă������y���Ɏg����悤�ˁB");
					strcpy(quest_text[line++], "���Ƃ͎��ɔC���Ȃ����B���Ȃ��ɂ͏[���ȕ�V��^���܂��傤�B�v");
				}
				else
				{
					strcpy(quest_text[line++], "����d�u�����̗͂̔��荞�ݕ����悭�S���Ă���悤�ł��ˁB");
					strcpy(quest_text[line++], "���Ȃ��Ƃ͂悢�֌W�ł��肽�����̂ł��B");
					strcpy(quest_text[line++], "����̕�V�Ƃ��Ă���������グ�܂��傤�B�v");
				}
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_GOUYOKU)
			{
				if (pc == CLASS_YUMA)
				{
					strcpy(quest_text[line++], "���~�������̖{�C�̗͂̑O�ɑ����͓͂��������Ă������B");
					strcpy(quest_text[line++], "���ɗ����Ȃ������҂̐_���獡�X�x���i�̒񋟂̐\���o�����������A");
					strcpy(quest_text[line++], "�����������ۂݍ���ł�낤�B");
				}
				else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
				{
					strcpy(quest_text[line++], "�ޖ��u�N�b�N�b�N...�V���ȋ�������Ȃ����B�C�ɓ��������B");
					strcpy(quest_text[line++], "�J���Ƃ��Ă��������Ă��B");
					strcpy(quest_text[line++], "�����H�����肾�������A���O�Ɏg�킹���ق������v�ɂȂ肻�����B�v");
				}
				else
				{
					strcpy(quest_text[line++], "�ޖ��u�͂āA�Ȃ����O����X�ɖ��������H�����ɎQ���������̂��H");
					strcpy(quest_text[line++], "�܂������A�������ނ��B���̂Ԃ��V�ɐF��t���Ă��B�v");
				}
			}
			else if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_ALIGN_HANIWA)
			{
				if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
				{
					strcpy(quest_text[line++], "���֕��c�̖����̗͂̑O�ɓ����삽���͎U��U��ɓ����Ă������B");
					strcpy(quest_text[line++], "���񋟎҂��狦�͂̎ӗ�Ƃ��Ē������i���͂���ꂽ�B");
				}
				else if (pr == RACE_HANIWA)
				{
					strcpy(quest_text[line++], "���|�u��X�̏�����I");
					strcpy(quest_text[line++], "�����ȓ����������M���ɂ͖J���̕i����������܂��I");
					strcpy(quest_text[line++], "����̕����Ɋ��҂��Ă����I�v");
				}
				else
				{
					strcpy(quest_text[line++], "���|�u������ǂ��̑|���ɂ����͊��Ӓv���܂��I");
					strcpy(quest_text[line++], "�����ȓ����������M���ɂ͖J���̕i����������܂��I");
					strcpy(quest_text[line++], "����ƁA��X�̂悤�Ȋ����ȑ̂����]�݂Ȃ炢�ł������Ă��������ˁI�v");
					//TODO:�ݕP�̓X�ŏ��ւɎ푰�ύX�ł���悤�ɂȂ�Ƃ����̂͂ǂ����낤�H
				}
			}
			else //�S�Ń��[�g
			{
				strcpy(quest_text[line++], "�S�Ă̐��͂�P�Ƃşr�ł��Ė߂�ƁA�w���ɖ��Ȃނ��y�����������B");
				strcpy(quest_text[line++], "�U������Ă݂邪�N�����炸�A���������̊Ԃɂ������Ȕ����u����Ă����B");
				strcpy(quest_text[line++], "�ǂ���炠�Ȃ��̍s���͂ǂ����̒N������ɖ����������炵���B");
			}
		}

		//���s��
		else if (fail)
		{

			if (pc == CLASS_SAKI)
			{
				strcpy(quest_text[line++], "�܂����G�ΐ��͂ɖ\�͂Ō�������Ă��܂����B");
				strcpy(quest_text[line++], "�����̎�O�C��ɐU�镑�����A�H�����A��ʂ�Ȃ�...");
			}
			else if (pc == CLASS_YACHIE)
			{
				strcpy(quest_text[line++], "�R���ɔs��Ă��܂����B");
				strcpy(quest_text[line++], "�Ζ������𓾂������͂𑊎�ɂǂ������Ԃ��������ɂ�...");
			}
			else if (pc == CLASS_YUMA)
			{
				strcpy(quest_text[line++], "�܊p�̐Ζ�������D���Ă��܂����B");
				strcpy(quest_text[line++], "���͑�l��������������݂̂��B");
				strcpy(quest_text[line++], "�����ꂱ������E�э���őS���܂ݐH�����Ă�낤�B");
			}
			else if (pc == CLASS_MAYUMI || pc == CLASS_KEIKI)
			{
				strcpy(quest_text[line++], "�g���B�̖{�C�̍R���͑z���ȏ�̌������������B");
				strcpy(quest_text[line++], "�s�o�ɂ��s���̗J���ڂɂ����Ă��܂����B");
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KEIGA || pc == CLASS_ENOKO)
			{
				strcpy(quest_text[line++], "���S�u�������A�Ȃ񂾂��̟T���������́I");
				strcpy(quest_text[line++], "�������Ȃ񂩖������I�ł����͐�΂ɏ�����ȁI�v");
			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_KIKETSU || pc == CLASS_BITEN)
			{
				strcpy(quest_text[line++], "����d�u�܂����̎�̑����͐퓬���I����Ă��炪�{�Ԃ��B");
				strcpy(quest_text[line++], "���Ƃ͎�������Ă����B���Ȃ��͌��̔C���ɖ߂�Ȃ����B�v");

			}
			else if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_GOUYOKU || pc == CLASS_CHIYARI)
			{
				strcpy(quest_text[line++], "�ޖ��u�܂������B���ꂪ�N�̂��̂ɂȂ낤������ɔE�э���ŐH���܂ł��B");
				strcpy(quest_text[line++], "�������Ȃ�����Ȃɑ����A���ɐΖ��̂��Ƃ��R�ꂽ�H");
				strcpy(quest_text[line++], "��͂肠�̌ӎU�L���z�̍��������H�v");
			}
			else if (pr == RACE_ANIMAL_GHOST)
			{
				strcpy(quest_text[line++], "���Ȃ��͒{���E�Z�l����������ȍR�����疽���炪�瓦���o�����B");
			}
			else if (pr == RACE_HANIWA)
			{
				strcpy(quest_text[line++], "���|�u������ɂ���قǂ̂��Ƃ��ł���Ȃ��...");
				strcpy(quest_text[line++], "�@���̂悤�Ȍ�������I�v");
			}
			else if (pc == CLASS_SATORI || pc == CLASS_KOISHI || pc == CLASS_ORIN || pc == CLASS_UTSUHO)
			{
				strcpy(quest_text[line++], "���Ȃ��͒{���E�Z�l����������ȍR�����疽���炪�瓦���o�����B");
			}
			else if (player_looks_human_side())//���X���\�����l�Ԑ��͂��ۂ��O�����ǂ����ŕ���
			{
				if (p_ptr->psex == SEX_MALE)
					strcpy(quest_text[line++], "�Ӂu���₨�₨�Z������v�����H");
				else
					strcpy(quest_text[line++], "�Ӂu���₨�₨�o������v�����H");

				strcpy(quest_text[line++], "�������v����Ȃ��Ă����S���Ă�B");
				strcpy(quest_text[line++], "�������������Ǝ��̖̂ʓ|�����Ă����邩��ˁB�v");

			}
			else
			{
				if (p_ptr->psex == SEX_MALE)
				{
					strcpy(quest_text[line++], "�Ӂu���₨�₨�Z������v�����H");
					strcpy(quest_text[line++], "�ł����Z����̎��͖̂ʔ����Ȃ��������ˁB");
				}
				else
				{
					strcpy(quest_text[line++], "�Ӂu���₨�₨�o������v�����H");
					strcpy(quest_text[line++], "�ł����o����̎��͖̂ʔ����Ȃ��������ˁB");
				}
				strcpy(quest_text[line++], "�܁A����ɐZ�������炳�����ƋA�����A�����B");

			}

		}
		break;


	case QUEST_HANGOKU2://v1.1.98 �A�����������U
		if (accept)
		{
			if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
			{
				strcpy(quest_text[line++], "�d����c�̉������삩���q���Ăق����Ɨ��܂ꂽ�B");
				strcpy(quest_text[line++], "�S�������Ȃ��Ƃ��B");
				strcpy(quest_text[line++], "���̉���͖ڂ̑O�ɂ���Ƃ����̂ɁB");

			}
			else if (pc == CLASS_3_FAIRIES || pc == CLASS_LARVA || pr == RACE_FAIRY)
			{
				strcpy(quest_text[line++], "���z�̔��Ƀ~�X�e���[�T�[�N�����o�������B");
				strcpy(quest_text[line++], "�������d�������̂̑��c���J�����Ƃ����B");
				strcpy(quest_text[line++], "�������~�X�e���[�T�[�N���̒��ɋ��͂ȉ��삪�������Ă���炵���B");
				strcpy(quest_text[line++], "�d���̖����̂��߂Ɏ������撣���Ēǂ�����Ȃ��ƁB");
			}
		}

		if (comp)
		{

			if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
			{
				strcpy(quest_text[line++], "���z�̔��ŗd���ɐ��̂����j���Đ킢�ɂȂ����B");
				strcpy(quest_text[line++], "��������͂�ŋ߂̗d�����D�����Ă���B");
				strcpy(quest_text[line++], "�d���������Ȃ��Ȃ��S�䂩�����������Ă����̂Œ��Ղ��Ă������B");

			}
			//���܂�Ȃ��Ǝv�����A��̎��ɂ͜߈˂���Ă��������̌�ǂ����ŐV���̖�����ނȂǂ��Ĝ߈ˉ��������Ƃ�
			else if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
			{
				strcpy(quest_text[line++], "�C�����Ƒ����ɑ�d�����|�ꕚ���Ă���A");
				strcpy(quest_text[line++], "�ӂ�ɂ͋����������d�������������B");
				strcpy(quest_text[line++], "�����N�����Ă���̂��낤�H�ŋ߂̋L�����Ȃ��B");
				strcpy(quest_text[line++], "�ł�����͂���Ƃ��Ă������͖̂���Ă������B");

			}
			else if (pc == CLASS_3_FAIRIES || pc == CLASS_LARVA || pr == RACE_FAIRY)
			{
				strcpy(quest_text[line++], "��X�̏������I");
				strcpy(quest_text[line++], "���Ԃ�������̎^�ƂƂ��ɏܕi�𑗂�ꂽ�B");
				strcpy(quest_text[line++], "���̑O�̏o�X�̔���c��̂悤�ȋC�����邪�A");
				strcpy(quest_text[line++], "����Ȃ��Ƃ͂��̒B�����̑O�ł͍��ׂȖ�肾�B");
			}

		}

		if (fail)
		{
			if (p_ptr->muta4 & MUT4_GHOST_HANGOKUOH)
			{

				//�N�G�X�g��̎����牅��ɜ߈˂���Ă���Ƃ�
				if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
				{
					strcpy(quest_text[line++], "���z�̔��ŗd���ɐ��̂����j���Đ킢�ɂȂ����B");
					strcpy(quest_text[line++], "�Ȃ��Ȃ��̋��G�œP�ނ�]�V�Ȃ����ꂽ�B");
					strcpy(quest_text[line++], "�ŋ߂̗d���������̂Ă����̂ł͂Ȃ��̂�������Ȃ��B");
				}
				//����̓���s���ŋ������s�ɂȂ����炱���ɗ���͂�
				else
				{
					strcpy(quest_text[line++], "�u���̍���Ȃ��Ɏ��̑O�ɗ��Ƃ͑S�������т�ꂽ���̂��ȁB");
					strcpy(quest_text[line++], "���āA���̑̂��ǂ��g���Ă���悤���H�v");

				}

			}
			//�N�G�X�g��̎��ɉ���߈˂���Ă������r���ŐV���̖�ȂǂŜ߈ˉ��������Ƃ�
			else if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
			{
				strcpy(quest_text[line++], "�C�����Ƒ��z�̔��ő�d���ɒǂ������񂳂�Ă����B");
				strcpy(quest_text[line++], "�����N�����Ă���̂��낤�H�ŋ߂̋L�����Ȃ��B");

			}

		}

		break;

		//v2.0.13 ���N�U�푈2�N�G�X�g
	case QUEST_YAKUZA_2:
		if (pc == CLASS_ENOKO)		
		{
			if (accept)
			{
				strcpy(quest_text[line++], "���S�u���悢��n�߂邼�B�����͂����ȁH");
				strcpy(quest_text[line++], "�܂��͂������̎�z������D���ȓz��I��Œ@���̂߂��ė����B");
				strcpy(quest_text[line++], "���O���n��i�o�̈�ԑ����B�C�������I�v");
			}
			if (comp)
			{
				strcpy(quest_text[line++], "���S�u�悭������I�܂��̓��C�o�����ЂƂR���Ƃ����ȁB");
				strcpy(quest_text[line++], "���������ł����Ƌ����Ȃ��B�v");
			}
		}
		if (pc == CLASS_BITEN)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "����d�u�@�͏n�����B���낻��n�߂悤�B");
				strcpy(quest_text[line++], "�܂��͓G���͂̋����Ƃ�ׂ��Ȃ��ƂˁB");
				strcpy(quest_text[line++], "�����̎�z������D���Ȃق���I�тȂ����B");
				strcpy(quest_text[line++], "�O���d�m�q�͖��@�̐X�Ő[���A");
				strcpy(quest_text[line++], "�V�ΐl�����͋��ܔM�n��������ɂ��Ă��邻����B�v");
			}
			if (comp)
			{
				strcpy(quest_text[line++], "����d�u�܂��͈�����[�h�ˁB");
				strcpy(quest_text[line++], "���̕�V�Ő�͂𐮂��Ȃ����B�v");
			}
		}
		if (pc == CLASS_CHIYARI)
		{
			if (accept)
			{
				strcpy(quest_text[line++], "�ޖ��u����Ɨ������B�����������̑g�̒n�㐨�͂�ǂ��������B");
				strcpy(quest_text[line++], "���傤�Ǔz��݂͌��ɏ܋������������Ă邩�炤���Œ����Ă��܂����B");
				strcpy(quest_text[line++], "����g�̌����͖��@�̐X�ł悭��������B");
				strcpy(quest_text[line++], "�S���g�̉����͂��������Ŋ������Ă���悤���B");
				strcpy(quest_text[line++], "�V��̗��ɂ͐l�T�������ӂȒ���������炵��������𔃂��̂������ȁB�v");
			}
			if (comp)
			{
				strcpy(quest_text[line++], "�u�X�b�L������������Ă���ȁB");
				strcpy(quest_text[line++], "��V���󂯎��̂�Y���Ȃ�B�v");
			}
		}





		break;


	default:
		break;
	}

	/*:::�N�G�X�g���͂������������Ă���ꍇ�z��̎c���S�ċ󔒂ɂ���*/
	if(line)
	{
		while(line < 10) quest_text[line++][0] = '\0';

		return TRUE;
	}
	else return FALSE;
}

/*:::Mega Hack - �ꕔ���j�[�N�N���X�͓���̃N�G�X�g���ŏ�����󂯂��Ȃ��B*/
/*:::���́��Ŏ󂯂��Ȃ��N�G�X�g�̂Ƃ�TRUE��Ԃ��B*/
/*:::�����Ƃ��ẮA�Y���N�G�X�g���Q�[���X�^�[�g���ɑS�Ď��s�ςɂ��A�_���v��X�e�[�^�X�ɕ\�����Ȃ��B*/
/*:::������~�X�ɋC�������B���ꂾ�Ƃ�����TRUE�ɂȂ����N�G�X�g�����s�I���̏�Ԃŏ��������������B�܂�Ⴆ�΃t�����͖��l�N�G�����łȂ����ۉ��O�N�G���󂯂��Ȃ��B*/
/*:::�ǂ��������̂��B�Ƃ肠�������u�B*/
/*:::��txxxxxxx.txt�̏�������ɍ׍H���ė��������Ȃ����B*/
//�����̑��ɂ�castle_quest()�̂Ƃ���Ŏ󒍂̉\�s�\�����߂Ă��鏈��������B
//�\���΍��H�ߍ����ȂǁB
//������ł͎푰�ϗe�Ƃ�������󂯂��邪�󂯂��Ȃ��Ƃ��ɂ��̌�̃N�G�X�g(���͂Ȃ��͂�)���󂯂��Ȃ��B
//������ł͂��̌�̃N�G�X�g���󂯂��邪�A�푰�ϗe�������Ǝ󂯂ɗ��Ă����łɓ����I�Ɏ��s�ς݁B

bool check_ignoring_quest(int questnum)
{

	int pc = p_ptr->pclass;
	int pr;

	//v1.1.80 �L����������͍��̎푰�łȂ��L�������C�N���̎푰���Q�Ƃ���B
	//�푰�ϗe�����Ƃ������I���s�̂͂��̃N�G�X�g�����x��0���s�Ƃ��ă_���v�Ƃ��ɏo�Ă��ĕςȂ̂�
	if (!character_generated) 
		pr = p_ptr->prace;
	else
		pr = previous_char.prace;

	switch(questnum)
	{
	case QUEST_SEWER:
	case QUEST_KORI:
		if(pc == CLASS_EIKI) return TRUE;
		break;

		///mod151001 ���ᏋC�N�G�X�g����
	case QUEST_KAPPA:
		if(pr == RACE_KAPPA) return TRUE;
		break;
		//�c�p�C�N�G�@���~���A
	case QUEST_TUPAI:
		if(pc == CLASS_REMY) return TRUE;
		break;

		//���������@�f�P ���g�@�i��
	case QUEST_OLD_WILLOW:
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_EIRIN) return TRUE;
		if(pc == CLASS_MOKOU) return TRUE;
		if(pc == CLASS_KAGUYA) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		break;
		
	//��n�x�����}��@���ӁA��
	case QUEST_GRAVEDANCER:
		if(pc == CLASS_ORIN) return TRUE;
		if(pc == CLASS_BYAKUREN) return TRUE;
		break;

	//���g�ւ̎h�q�@�d�� ���g
	case QUEST_KILL_MOKO:
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_KEINE) return TRUE;
		if(pc == CLASS_MOKOU) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		break;

	//�P��ւ̎h�q�@���ǂ񂰁A�Ă�A�d���A�i�ԁA�Ȍ��o��
	case QUEST_KILL_GUYA:
		if(pc == CLASS_EIRIN) return TRUE;
		if(pc == CLASS_KAGUYA) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		if(pc == CLASS_TEWI) return TRUE;
		if(pc == CLASS_UDONGE && !is_special_seikaku(SEIKAKU_SPECIAL_UDONGE)) return TRUE;
		if(pc == CLASS_KEINE) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		if(pc == CLASS_TOYOHIME) return TRUE;
		if(pc == CLASS_YORIHIME) return TRUE;
		break;

	case QUEST_MITHRIL: //�~�X�����T���@���A�f�P
		if(pc == CLASS_BYAKUREN) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		break;

		//���l���~�߂ā@�t����
	case QUEST_FLAN:
		if(pc == CLASS_FLAN) return TRUE;
		break;

		//���ۉ��O�@�p�`�����[
	case QUEST_KYOMARU:
		if(pc == CLASS_PATCHOULI) return TRUE;
		break;
		//��J�_�Ё@�_�ގq,�z�K�q,�f�P
	case QUEST_KANASUWA:
		if(pc == CLASS_SUWAKO) return TRUE;
		if(pc == CLASS_KANAKO) return TRUE;
		if(pc == CLASS_EIKI) return TRUE;
		break;
		//������@�얲
	case QUEST_HANAMI:
		if(pc == CLASS_REIMU) return TRUE;
		break;
		//���q�ˎҁ@����
	case QUEST_WANTED_SEIJA:
		if(pc == CLASS_SEIJA) return TRUE;
		break;
	case QUEST_WANTED_SUMIREKO: //�悤�������z���� �얲��俎q ���Ɛ�����
		if(pc == CLASS_REIMU || pc == CLASS_SUMIREKO || pc == CLASS_RINGO || pc == CLASS_SEIRAN) return TRUE;
		if(pc == CLASS_SAGUME) return TRUE;
		if(pc == CLASS_TOYOHIME) return TRUE;
		if(pc == CLASS_YORIHIME) return TRUE;

		break;
		//�w�J�[�e�B�A�œ|�@�N���E���s�[�X�A�w�J�[�e�B�A�A����
	case QUEST_HECATIA:
		if(pc == CLASS_CLOWNPIECE) return TRUE;
		if (pc == CLASS_VFS_CLOWNPIECE) return TRUE;
		if(pc == CLASS_HECATIA) return TRUE;
		if(pc == CLASS_JUNKO) return TRUE;
		break;
		//�P��N�G�X�g 5�Ƃ��B��������I���
		///mod151219 �I���Ȃ�����
		///mod151229 ���Ȃ��I�v�V�����̂Ƃ��ɂ͏o�Ȃ�����
		//�P�鏜�O�ǉ�
	case QUEST_KAGUYA:
		if(pc == CLASS_KAGUYA) return TRUE;
		if(ironman_no_fixed_art) return TRUE;
		break;

	case QUEST_OKINA:
		//v1.1.32 �Ƃ肠�������ƃI�L�i�͕s���֌W���ۂ��̂Ńp�X
		if(pc == CLASS_YUKARI) return TRUE;
		if(pc == CLASS_RAN) return TRUE;
		//v1.1.41 ���Ɨ��T���p�X�@
		if (pc == CLASS_MAI) return TRUE;
		if (pc == CLASS_SATONO) return TRUE;
		//v1.1.57 �B���
		if (pc == CLASS_OKINA) return TRUE;

		break;

	//v1.1.36
	case QUEST_KOSUZU:
		if(pc == CLASS_YUKARI) return TRUE;
		if(pc == CLASS_KOSUZU) return TRUE;
		break;


	//v1.1.42
	case QUEST_DREAMDWELLER:
		if (pc == CLASS_DOREMY) return TRUE;
		//v1.1.52 �������͜߈ˉ؂Łu�S���X�J�X�J�Ŗ������Ȃ��H�v�Ƃ������Ă��̂Ŗ����ɂ��Ă����镕�i�C�g���A�_�C�A���[�Ŗ��̐��E�̂��������o�Ă����̂ŗL���ɂ���B
		//�������v���C���ɃZ�[�u�f�[�^���A�b�v�f�[�g����ƃN�G�X�g���s�����ɂȂ��Ă���̂�rd_savefile_new_aux()�ɂ��׍H�����B
		//	if (pc == CLASS_KOISHI) return TRUE; 
		if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE)) return TRUE;
		//v1.1.57 �B��ނ����̒��ɕ��ʂɏo�ė��Ă����ۂ��̂Ńp�X
		if (pc == CLASS_OKINA) return TRUE;
		break;

		//���ۍ��Ĕ��߁@������͎󂯂��Ȃ�
	case QUEST_SHORELINE2:

		if (pr == RACE_ANIMAL_GHOST) return TRUE;
		if (pc == CLASS_SAKI) return TRUE;
		if (pc == CLASS_YACHIE) return TRUE;

		//v2.0.14 �Y��Ă��̂Œǉ�
		if (pc == CLASS_YUMA) return TRUE;
		if (pc == CLASS_ENOKO) return TRUE;
		if (pc == CLASS_BITEN) return TRUE;
		if (pc == CLASS_CHIYARI) return TRUE;

		break;

		//�j�ł̃N�G�X�g3�@������͎󂯂��Ȃ�
	case QUEST_DOOM3:

		if (pr == RACE_ANIMAL_GHOST) return TRUE;
		if (pc == CLASS_SAKI) return TRUE;
		if (pc == CLASS_YACHIE) return TRUE;
		//�ݕP���t���@���łɕ�V�A�C�e���������Ă���̂�
		if (pc == CLASS_KEIKI) return TRUE;

		break;

	case QUEST_MIYOI: //�h���[���C�[�^�[�̐��Ё@�����A���A�}�~�]�E�͎�̕s��

		if (pc == CLASS_MIYOI || pc == CLASS_SUIKA || pc == CLASS_MAMIZOU) return TRUE;
		//�N���s���Q�[���I�[�o�[�ɂȂ����Ƃ������ł��Ȃ��̂Œe���Ƃ�
		if (pc == CLASS_CLOWNPIECE) return TRUE;
		break;

	case QUEST_HANGOKU1: //v1.1.84 �A�����������T�@�������E���E�����o�Ȃ����[�h�̂Ƃ���̕s��
		if (pc == CLASS_MARISA || pc == CLASS_YUKARI) return TRUE;
		if (ironman_no_fixed_art) return TRUE;

		break;

	case QUEST_MORIYA_2:
		if (pc == CLASS_CHIMATA) return TRUE;//�疒�̓A�r���e�B�J�[�h����V�̃N�G�X�g���󂯂��Ȃ�
		break;

	case QUEST_REIMU_ATTACK:
		if (pc == CLASS_REIMU) return TRUE;

		break;

	case QUEST_YAKUZA_1:
		if (pc == CLASS_OKINA) return TRUE;
		break;

		//v2.0.13 �܋���N�G�X�g�@�e���͑g���Ƃǂ��ɂ������ꂵ�Ȃ��l�͎�̕s��
	case QUEST_YAKUZA_2:
		if (pc == CLASS_SAKI || pc == CLASS_YACHIE || pc == CLASS_YUMA || pc == CLASS_ZANMU || pc == CLASS_HISAMI || pc == CLASS_EIKI || pc == CLASS_REIMU)
			return TRUE;
		break;

	}

	

	return FALSE;
}

/*
 * Displays all info about a weapon
 *
 * Only accurate for the current weapon, because it includes
 * various info about the player's +to_dam and number of blows.
 */
/*:::���폠�̕����r�T�u���[�`���@���햼�A�U���񐔁A�������A�ʏ�U���͂�\��*/
static void list_weapon(object_type *o_ptr, int row, int col)
{
	char o_name[MAX_NLEN];
	char tmp_str[80];

	/* Effective dices */
	int eff_dd = o_ptr->dd + p_ptr->to_dd[0];
	int eff_ds = o_ptr->ds + p_ptr->to_ds[0];

	int mindam,maxdam;

	/* Print the weapon name */
	object_desc(o_name, o_ptr, OD_NAME_ONLY);
	c_put_str(TERM_YELLOW, o_name, row, col);

	/* Print the player's number of blows */
#ifdef JP
sprintf(tmp_str, "�U����: %d", p_ptr->num_blow[0]);
#else
	sprintf(tmp_str, "Number of Blows: %d", p_ptr->num_blow[0]);
#endif

	put_str(tmp_str, row+1, col);

	/* Print to_hit and to_dam of the weapon */
#ifdef JP
sprintf(tmp_str, "������:  0  50 100 150 200 (�G��AC)");
#else
sprintf(tmp_str, "To Hit:  0  50 100 150 200 (AC)");
#endif

	put_str(tmp_str, row+2, col);

	/* Print the weapons base damage dice */
#ifdef JP
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#else
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#endif

	put_str(tmp_str, row+3, col);

#ifdef JP
c_put_str(TERM_YELLOW, "�\�ȃ_���[�W:", row+5, col);
#else
	c_put_str(TERM_YELLOW, "Possible Damage:", row+5, col);
#endif


	mindam = MAX((eff_dd + o_ptr->to_d + p_ptr->to_d[0]),0);
	maxdam = MAX((eff_ds * eff_dd + o_ptr->to_d + p_ptr->to_d[0]),0);
	
	/* Damage for one blow (if it hits) */
sprintf(tmp_str, "�U�����ɂ� %d-%d", mindam, maxdam);
	put_str(tmp_str, row+6, col+1);

	/* Damage for the complete attack (if all blows hit) */
sprintf(tmp_str, "�P�^�[���ɂ� %d-%d", p_ptr->num_blow[0] * mindam, p_ptr->num_blow[0] * maxdam);
	put_str(tmp_str, row+7, col+1);
}


//���폠�ł̔�r�p object_is_melee_weapon����Őj������������
//v1.1.93 �S�ʏ������� TV_MAGICWEAPON��ǉ��Ŏw��ł���悤�ɂȂ���
bool item_tester_hook_compare_weapon(object_type *o_ptr)
{
//	if(item_tester_hook_melee_weapon(o_ptr)) return TRUE;
//	if(o_ptr->tval == TV_MAGICWEAPON) return TRUE;

	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI) return (FALSE);

	return (object_is_melee_weapon(o_ptr));

}


/*
 * Hook to specify "ammo"
 */
static bool item_tester_hook_ammo(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		//case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

static bool item_tester_hook_bow_crossbow(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_BOW:
		case TV_CROSSBOW:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}



/*
 * Compare weapons
 *
 * Copies the weapons to compare into the weapon-slot and
 * compares the values for both weapons.
 */
/*:::���폠�ɂ�镐���r*/

static int compare_weapons(int bcost)
{
	int i, n;
	int item, item2;
	object_type *o_ptr[2];
	object_type orig_weapon;
	object_type *i_ptr;
	cptr q, s;
	int row = 2;
	int wid = 38, mgn = 2;
	bool old_character_xtra = character_xtra;
	char ch;
	int total = 0;
	int cost = 0; /* First time no price */

	if(p_ptr->pclass == CLASS_ALICE)
	{
		msg_print("���Ȃ��̐퓬�X�^�C���͕��폠�̗����𒴂��Ă���炵���B");
		return 0;
	}

	/* Save the screen */
	screen_save();

	/* Clear the screen */
	clear_bldg(0, 22);

	/* Store copy of original wielded weapon */
	/*:::�ꎞ�I�ɕ����ޔ�*/
	i_ptr = &inventory[INVEN_RARM];
	object_copy(&orig_weapon, i_ptr);

	/* Only compare melee weapons */
	item_tester_no_ryoute = TRUE;
	item_tester_hook = item_tester_hook_compare_weapon;

	/* Get the first weapon */
	q = _("���̕���́H", "What is your first weapon? ");
	s = _("��ׂ���̂�����܂���B", "You have nothing to compare.");

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN)))
	{
		screen_load();
		return (0);
	}

	/* Get the item (in the pack) */
	o_ptr[0] = &inventory[item];
	n = 1;
	total = bcost;

	while (TRUE)
	{
		/* Clear the screen */
		clear_bldg(0, 22);

		/* Only compare melee weapons */
		item_tester_no_ryoute = TRUE;
		//v1.1.93 melee_weapon��compare_weapon�ɏC��
		item_tester_hook = item_tester_hook_compare_weapon;

		/* Hack -- prevent "icky" message */
		character_xtra = TRUE;

		/* Diaplay selected weapon's infomation */
		for (i = 0; i < n; i++)
		{
			int col = (wid * i + mgn);

			/* Copy i-th weapon into the weapon slot (if it's not already there) */
			if (o_ptr[i] != i_ptr) object_copy(i_ptr, o_ptr[i]);

			/* Get the new values */
			calc_bonuses();

			/* List the new values */
			list_weapon(o_ptr[i], row, col);
			compare_weapon_aux1(o_ptr[i], col, row + 8);

			/* Copy back the original weapon into the weapon slot */
			object_copy(i_ptr, &orig_weapon);
		}

		/* Reset the values for the old weapon */
		calc_bonuses();

		character_xtra = old_character_xtra;

#ifdef JP
		put_str(format("[ ��r�Ώ�: 's'�ŕύX ($%d) ]", cost), 1, (wid + mgn));
		put_str("(��ԍ����_���[�W���K�p����܂��B�����̔{�Ō��ʂ͑����Z����܂���B)", row + 4, 0);
		prt("���݂̋Z�ʂ��画�f����ƁA���Ȃ��̕���͈ȉ��̂悤�ȈЗ͂𔭊����܂�:", 0, 0);
#else
		put_str(format("[ 's' Select secondary weapon($%d) ]", cost), row + 1, (wid * i + mgn));
		put_str("(Only highest damage applies per monster. Special damage not cumulative.)", row + 4, 0);
		prt("Based on your current abilities, here is what your weapons will do", 0, 0);
#endif

		flush();
		ch = inkey();

		if (ch == 's')
		{
			if (total + cost > p_ptr->au)
			{
				msg_print(_("����������܂���I", "You don't have enough money!"));
				msg_print(NULL);
				continue;
			}

			q = _("���̕���́H", "What is your second weapon? ");
			s = _("��ׂ���̂�����܂���B", "You have nothing to compare.");

			/* Get the second weapon */
			if (!get_item(&item2, q, s, (USE_EQUIP | USE_INVEN))) continue;

			total += cost;
			cost = bcost / 2;

			/* Get the item (in the pack) */
			o_ptr[1] = &inventory[item2];
			n = 2;
		}
		else
		{
			///mod140430 hook���N���A
			item_tester_no_ryoute = FALSE;
			item_tester_hook = NULL;
			break;
		}
	}

	/* Restore the screen */
	screen_load();

	/* Done */
	return (total);
}

/*:::���폠�́u�h��ɂ��Ē��ׂ�v�R�}���h*/
/*
 * Evaluate AC
 *
 * AC�����𗦁A�_���[�W���������v�Z���\������
 * Calculate and display the dodge-rate and the protection-rate
 * based on AC
 */
static bool eval_ac(int iAC)
{
#ifdef JP
	const char memo[] =
		"�_���[�W�y�����Ƃ́A�G�̍U�����������������̃_���[�W��\n"
		"���p�[�Z���g�y�����邩�������܂��B\n"
		"�_���[�W�y���͒ʏ�̒��ڍU���ɑ΂��Ă̂݌��ʂ�����܂��B\n"
		"(��ނ��u�U������v�u����𕉂킹��v�u���ӂ���v�̕�)\n \n"
		"�G�̃��x���Ƃ́A���̓G���ʏ퉽�K�Ɍ���邩�������܂��B\n \n"
		"��𗦂͓G�̒��ڍU�������p�[�Z���g�̊m���Ŕ����邩�������A\n"
		"�G�̃��x���Ƃ��Ȃ���AC�ɂ���Č��肳��܂��B\n \n"
		"�_���[�W���Ғl�Ƃ́A�G�̂P�O�O�|�C���g�̒ʏ�U���ɑ΂��A\n"
		"��𗦂ƃ_���[�W�y�������l�������_���[�W�̊��Ғl�������܂��B\n";
#else
	const char memo[] =
		"'Protection Rate' means how much damage is reduced by your armor.\n"
		"Note that the Protection rate is effective only against normal "
		"'attack' and 'shatter' type melee attacks, "
		"and has no effect against any other types such as 'poison'.\n \n"
		"'Dodge Rate' indicates the success rate on dodging the "
		"monster's melee attacks.  "
		"It is depend on the level of the monster and your AC.\n \n"
		"'Average Damage' indicates the expected amount of damage "
		"when you are attacked by normal melee attacks with power=100.";
#endif

	int protection;
	int col, row = 2;
	int lvl;
	char buf[80*20], *t;

	/* AC lower than zero has no effect */
	if (iAC < 0) iAC = 0;

	/* �_���[�W�y�������v�Z */
	protection = 100 * MIN(iAC, 150) / 250;

	screen_save();
	clear_bldg(0, 22);

#ifdef JP
	put_str(format("���Ȃ��̌��݂�AC: %3d", iAC), row++, 0);
	put_str(format("�_���[�W�y����  : %3d%%", protection), row++, 0);
	row++;

	put_str("�G�̃��x��      :", row + 0, 0);
	put_str("���          :", row + 1, 0);
	put_str("�_���[�W���Ғl  :", row + 2, 0);
#else
	put_str(format("Your current AC : %3d", iAC), row++, 0);
	put_str(format("Protection rate : %3d%%", protection), row++, 0);
	row++;

	put_str("Level of Monster:", row + 0, 0);
	put_str("Dodge Rate      :", row + 1, 0);
	put_str("Average Damage  :", row + 2, 0);
#endif
    
	for (col = 17 + 1, lvl = 0; lvl <= 100; lvl += 10, col += 5)
	{
		int quality = 60 + lvl * 3; /* attack quality with power 60 */
		int dodge;   /* ���(%) */
		int average; /* �_���[�W���Ғl */

		put_str(format("%3d", lvl), row + 0, col);

		/* ��𗦂��v�Z */
		dodge = 5 + (MIN(100, 100 * (iAC * 3 / 4) / quality) * 9 + 5) / 10;
		put_str(format("%3d%%", dodge), row + 1, col);

		/* 100�_�̍U���ɑ΂��Ẵ_���[�W���Ғl���v�Z */
		average = (100 - dodge) * (100 - protection) / 100;
		put_str(format("%3d", average), row + 2, col);
	}

	/* Display note */
	roff_to_buf(memo, 70, buf, sizeof(buf));
	for (t = buf; t[0]; t += strlen(t) + 1)
		put_str(t, (row++) + 4, 4);

#ifdef JP
	prt("���݂̂��Ȃ��̑������炷��ƁA���Ȃ��̖h��͂�"
		   "���ꂭ�炢�ł�:", 0, 0);
#else
	prt("Defense abilities from your current Armor Class are evaluated below.", 0, 0);
#endif
  
	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}


/*
 * Hook to specify "broken weapon"
 */
///mod131224  ��ꂽ����
static bool item_tester_hook_broken_weapon(object_type *o_ptr)
{
 	if (o_ptr->tval == TV_SWORD && o_ptr->sval == SV_WEAPON_BROKEN_SWORD) return TRUE;
 	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_BROKEN_DAGGER) return TRUE;
 	if (o_ptr->tval == TV_KATANA && o_ptr->sval == SV_WEAPON_BROKEN_KATANA) return TRUE;
 	if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_BROKEN_STICK) return TRUE;


	return FALSE;
}

/*:::�܂ꂽ����C�� from_ptr����\�͂����to_ptr�փR�s�[*/
static void give_one_ability_of_object(object_type *to_ptr, object_type *from_ptr)
{
	int i, n = 0;
	int cand[TR_FLAG_MAX];
	u32b to_flgs[TR_FLAG_SIZE];
	u32b from_flgs[TR_FLAG_SIZE];

	object_flags(to_ptr, to_flgs);
	object_flags(from_ptr, from_flgs);

	for (i = 0; i < TR_FLAG_MAX; i++)
	{
		switch (i)
		{
		case TR_IGNORE_ACID:
		case TR_IGNORE_ELEC:
		case TR_IGNORE_FIRE:
		case TR_IGNORE_COLD:
		case TR_ACTIVATE:
		case TR_RIDING:
		case TR_SHOW_MODS:
		case TR_HIDE_TYPE:
		//case TR_ES_ATTACK:
		//case TR_ES_AC:
		case TR_FULL_NAME:
		case TR_FIXED_FLAVOR:
		case TR_TY_CURSE:
		case TR_AGGRAVATE:
		case TR_DRAIN_EXP:
		case TR_ADD_L_CURSE:
		case TR_ADD_H_CURSE:
		case TR_MATERIAL:
		case TR_USABLE:
		case TR_READABLE:

			break;
		default:

			//v1.1.73 �؂ꖡ��n�k�������p���̂ɕ���̎�ނ��l������悤�C��
			if (i == TR_IMPACT && object_has_a_blade(to_ptr)) break;
			if (i == TR_VORPAL && !object_has_a_blade(to_ptr)) break;
			if (i == TR_EX_VORPAL && !object_has_a_blade(to_ptr)) break;


			if (have_flag(from_flgs, i) && !have_flag(to_flgs, i))
			{
				if (!(is_pval_flag(i) && (from_ptr->pval < 1))) cand[n++] = i;
			}
		}
	}

	if (n > 0)
	{
		int bmax;
		int tr_idx = cand[randint0(n)];
		add_flag(to_ptr->art_flags, tr_idx);
		if (is_pval_flag(tr_idx)) to_ptr->pval = MAX(to_ptr->pval, 1);
		bmax = MIN(3, MAX(1, 40 / (to_ptr->dd * to_ptr->ds)));
		if (tr_idx == TR_BLOWS) to_ptr->pval = MIN(to_ptr->pval, bmax);
		if (tr_idx == TR_SPEED) to_ptr->pval = MIN(to_ptr->pval, 4);
	}

	return;
}

///mod140909 ����C���f��
bool item_tester_hook_repair_material(object_type *o_ptr)
{

	//v1.1.64 EXTRA�����̗쒷���ł��g�����A���̂Ƃ��͕����I���ł��Ȃ�
	if (object_is_melee_weapon_except_strange_kind(o_ptr))
	{
		if (building_ex_idx[f_info[cave[py][px].feat].subtype] == BLDG_EX_KEIKI)
			return FALSE;
		else
			return TRUE;
	}
	else if(o_ptr->tval == TV_MATERIAL)
	{
		switch(o_ptr->sval)
		{
		case SV_MATERIAL_HEMATITE:
		case SV_MATERIAL_MAGNETITE:
		case SV_MATERIAL_ARSENIC:
		case SV_MATERIAL_MERCURY:
		case SV_MATERIAL_MITHRIL:
		case SV_MATERIAL_ADAMANTITE:
		case SV_MATERIAL_DRAGONNAIL:
		case SV_MATERIAL_DRAGONSCALE:
		case SV_MATERIAL_METEORICIRON:
		case SV_MATERIAL_HIHIIROKANE:
		case SV_MATERIAL_SCRAP_IRON:
		case SV_MATERIAL_ICESCALE:
		case SV_MATERIAL_NIGHTMARE_FRAGMENT:
		case SV_MATERIAL_HOPE_FRAGMENT:
		case SV_MATERIAL_MYSTERIUM:
		case SV_MATERIAL_IZANAGIOBJECT:
		case SV_MATERIAL_ISHIZAKURA:
		case SV_MATERIAL_BROKEN_NEEDLE:
		case SV_MATERIAL_RYUUZYU:

			return TRUE;
		}
	}
	else if(o_ptr->tval == TV_SOUVENIR)
	{
		switch(o_ptr->sval)
		{
		case SV_SOUVENIR_ILMENITE:
		case SV_SOUVENIR_PHOENIX_FEATHER:
		case SV_SOUVENIR_ASIA:
		case SV_SOUVENIR_SHILVER_KEY:
		case SV_SOUVENIR_MOON_ORB:
			return TRUE;
		}
	}

	return FALSE;
}


/*:::����C���f�ޗp�e�[�u�� TV,SV,�f�ޗppower,���̃A�C�e���ɂȂ邽�߂̕K�vpower(-1�͍쐬�s��)*/
//�C�����ʌ��莞�͏����𖞂�����ԉ��̂��̂��I�肳���̂ŁA��tval�̕���͕K�vpower�l���Ⴂ���ɕ���ł��Ȃ��Ƃ����Ȃ�
const struct guild_arts_type repair_weapon_power_table[] =
{
	{TV_KNIFE,SV_WEAPON_BROKEN_DAGGER ,0,-1," "},
	{TV_KNIFE,SV_WEAPON_KNIFE ,0,0," "},
	{TV_KNIFE,SV_WEAPON_DAGGER ,1,2," "},
	{TV_KNIFE,SV_WEAPON_MAIN_GAUCHE ,2,4," "},
	{TV_KNIFE,SV_WEAPON_YAMAGATANA ,4,6," "},
	{TV_KNIFE,SV_WEAPON_TANTO ,6,8," "},
	{TV_KNIFE,SV_WEAPON_NINJA_SWORD ,7,10," "},
	{TV_KNIFE,SV_WEAPON_HAYABUSA ,15,17," "},
	{TV_KNIFE,SV_WEAPON_DRAGON_DAGGER ,16,-1," "},
	{TV_KNIFE,SV_WEAPON_VAJRA ,20,-1," "},//v1.1.31
	//�Őj�͑I��ΏۊO

	{TV_SWORD,SV_WEAPON_BROKEN_SWORD ,0,-1," "},
	{TV_SWORD,SV_WEAPON_RAPIER ,1,0," "},
	{TV_SWORD,SV_WEAPON_SHORT_SWORD ,2,2," "},
	{TV_SWORD,SV_WEAPON_SABRE ,3,3," "},
	{TV_SWORD,SV_WEAPON_SCIMITAR ,4,5," "},
	{TV_SWORD,SV_WEAPON_LONG_SWORD ,5,6," "},
	{TV_SWORD,SV_WEAPON_BASTARD_SWORD ,7,8," "},
	{TV_SWORD,SV_WEAPON_CLAYMORE ,7,10," "},
	{TV_SWORD,SV_WEAPON_GREAT_SWORD ,9,12," "},
	{TV_SWORD,SV_WEAPON_EXECUTIONERS_SWORD ,12,14," "},
	{TV_SWORD,SV_WEAPON_BLADE_OF_CHAOS ,16,18," "},
	{TV_SWORD,SV_WEAPON_HIHIIROKANE ,20,-1," "},

	{TV_KATANA,SV_WEAPON_BROKEN_KATANA ,0,-1," "},
	{TV_KATANA,SV_WEAPON_SHORT_KATANA ,3,0," "},
	{TV_KATANA,SV_WEAPON_KATANA ,6,5," "},
	{TV_KATANA,SV_WEAPON_GREAT_KATANA ,8,10," "},
	{TV_KATANA,SV_WEAPON_ANCIENT_KATANA ,12,15," "},
	{TV_KATANA,SV_WEAPON_EVIL_KATANA ,18,20," "},

	{TV_HAMMER,SV_WEAPON_SPANNER ,0,0," "},
	{TV_HAMMER,SV_WEAPON_MACE ,2,2," "},
	{TV_HAMMER,SV_WEAPON_KINE ,3,3," "},
	{TV_HAMMER,SV_WEAPON_MORNING_STAR ,4,6," "},
	{TV_HAMMER,SV_WEAPON_FLAIL ,5,8," "},
	{TV_HAMMER,SV_WEAPON_WAR_HAMMER ,7,9," "},
	{TV_HAMMER,SV_WEAPON_BALL_AND_CHAIN ,10,12," "},
	{TV_HAMMER,SV_WEAPON_KANASAIBOU ,16,18," "},

	{TV_STICK,SV_WEAPON_BROKEN_STICK ,0,-1," "},
	{TV_STICK,SV_WEAPON_FLOWER ,2,-1," "},
	{TV_STICK,SV_WEAPON_WIZSTAFF ,10,-1," "},
	{TV_STICK,SV_WEAPON_QUARTERSTAFF ,3,0," "},
	{TV_STICK,SV_WEAPON_SYAKUJYOU ,5,3," "},
	{TV_STICK,SV_WEAPON_SIXFEET ,7,5," "},
	{TV_STICK,SV_WEAPON_NUNCHAKU ,8,7," "},
	{TV_STICK,SV_WEAPON_IRONSTICK ,10,10," "},
	{TV_STICK,SV_WEAPON_OONUSA ,18,18," "},
	{TV_STICK,SV_WEAPON_GROND ,30,-1," "},
	{TV_STICK,SV_WEAPON_KOGASA ,20,-1," "},

	{TV_AXE,SV_WEAPON_PICK ,1,-1," "},
	{TV_AXE,SV_WEAPON_FARANCESCA ,1,0," "},
	{TV_AXE,SV_WEAPON_HATCHET ,2,1," "},
	{TV_AXE,SV_WEAPON_NATA ,3,3," "},
	{TV_AXE,SV_WEAPON_BROAD_AXE ,4,5," "},
	{TV_AXE,SV_WEAPON_BATTLE_AXE ,6,7," "},
	{TV_AXE,SV_WEAPON_LOCHABER_AXE ,9,9," "},
	{TV_AXE,SV_WEAPON_GREAT_AXE ,12,11," "},
	{TV_AXE,SV_WEAPON_MASAKARI ,18,19," "},

	{TV_SPEAR,SV_WEAPON_JAVELIN ,1,0," "},
	{TV_SPEAR,SV_WEAPON_SPEAR ,2,2," "},
	{TV_SPEAR,SV_WEAPON_AWL_PIKE ,3,4," "},
	{TV_SPEAR,SV_WEAPON_LONG_SPEAR ,6,6," "},
	{TV_SPEAR,SV_WEAPON_LANCE ,8,9," "},
	{TV_SPEAR,SV_WEAPON_HEAVY_LANCE ,10,12," "},
	{TV_SPEAR,SV_WEAPON_TRIFURCATE_SPEAR ,17,18," "},

	{TV_POLEARM,SV_WEAPON_GLAIVE ,3,0," "},
	{TV_POLEARM,SV_WEAPON_NAGINATA ,4,3," "},
	{TV_POLEARM,SV_WEAPON_LUCERNE_HAMMER ,7,5," "},
	{TV_POLEARM,SV_WEAPON_WAR_SCYTHE ,8,8," "},
	{TV_POLEARM,SV_WEAPON_HALBERD ,10,10," "},
	{TV_POLEARM,SV_WEAPON_SCYTHE_OF_SLICING ,15,16," "},


	{TV_OTHERWEAPON,SV_OTHERWEAPON_FISHING ,3,-1," "},
	{TV_OTHERWEAPON,SV_OTHERWEAPON_NEEDLE ,6,-1," "},

	{TV_MATERIAL,SV_MATERIAL_HEMATITE,3,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MAGNETITE,5,-1," "},
	{TV_MATERIAL,SV_MATERIAL_ARSENIC,6,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MERCURY,7,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MITHRIL,10,-1," "},
	{TV_MATERIAL,SV_MATERIAL_ADAMANTITE,12,-1," "},
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL,13,-1," "},
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE,10,-1," "},
	{TV_MATERIAL, SV_MATERIAL_METEORICIRON ,16,-1," "},
	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE,25,-1," "},
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON,0,-1," "}, //sv�l�������_����1-18
	{TV_MATERIAL,SV_MATERIAL_ICESCALE,10,-1," "},
	{TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT,10,-1," "},
	{TV_MATERIAL,SV_MATERIAL_HOPE_FRAGMENT,14,-1," "},
	{TV_MATERIAL,SV_MATERIAL_MYSTERIUM,20,-1," "},
	{TV_MATERIAL,SV_MATERIAL_IZANAGIOBJECT,17,-1," "},
	{TV_MATERIAL,SV_MATERIAL_ISHIZAKURA,5,-1," "},
	{TV_MATERIAL,SV_MATERIAL_BROKEN_NEEDLE,8,-1," "},
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU,12,-1," " },

	{TV_SOUVENIR,SV_SOUVENIR_ILMENITE,13,-1," "},
	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER,20,-1," "},
	{TV_SOUVENIR,SV_SOUVENIR_ASIA,25,-1," "},
	{ TV_SOUVENIR,SV_SOUVENIR_SHILVER_KEY,25,-1," " },
	{ TV_SOUVENIR,SV_SOUVENIR_MOON_ORB,10,-1," " },

	{0,0,0,0,"�I�[�p�_�~�["}
};





/*:::����ɃX���C�═��\�͂������_���ɕt�^����Bo_ptr�͋ߐڕ���ł��邱�ƁB*/
//������Ă悭�l������G�S����C�����Ă��X���C���p���[�A�b�v���Ȃ��B�܂��������B
void gain_random_slay_2(object_type *o_ptr)
{

	int tester;

	for(tester = 500; tester > 0; tester--)
	{
		switch(randint1(14))
		{
		case 1:
			if (have_flag(o_ptr->art_flags, TR_CHAOTIC)) break;
			add_flag(o_ptr->art_flags, TR_CHAOTIC);
			return;
		case 2:
			if (have_flag(o_ptr->art_flags, TR_VAMPIRIC)) break;
			add_flag(o_ptr->art_flags, TR_VAMPIRIC);
			return;
		case 3:
			if (have_flag(o_ptr->art_flags, TR_FORCE_WEAPON)) break;
			add_flag(o_ptr->art_flags, TR_FORCE_WEAPON);
			return;
		case 4:
			if(!object_has_a_blade(o_ptr)) break;
			if (have_flag(o_ptr->art_flags, TR_EX_VORPAL)) break;
			if (have_flag(o_ptr->art_flags, TR_VORPAL)) add_flag(o_ptr->art_flags, TR_EX_VORPAL);
			else add_flag(o_ptr->art_flags, TR_VORPAL);
			return;
		case 5:
			if(object_has_a_blade(o_ptr)) break;
			if (have_flag(o_ptr->art_flags, TR_IMPACT)) break;
			add_flag(o_ptr->art_flags, TR_IMPACT);
			return;
		case 6:
			if (have_flag(o_ptr->art_flags, TR_KILL_EVIL)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_EVIL)) add_flag(o_ptr->art_flags, TR_KILL_EVIL);
			else add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
			return;
		case 7:
			if (have_flag(o_ptr->art_flags, TR_KILL_GOOD)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_GOOD)) add_flag(o_ptr->art_flags, TR_KILL_GOOD);
			else add_flag(o_ptr->art_flags, TR_SLAY_GOOD);
			return;
		case 8:
			if (have_flag(o_ptr->art_flags, TR_KILL_ANIMAL)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_ANIMAL)) add_flag(o_ptr->art_flags, TR_KILL_ANIMAL);
			else add_flag(o_ptr->art_flags, TR_SLAY_ANIMAL);
			return;
		case 9:
			if (have_flag(o_ptr->art_flags, TR_KILL_HUMAN)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN)) add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
			else add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
			return;
		case 10:
			if (have_flag(o_ptr->art_flags, TR_KILL_UNDEAD)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_UNDEAD)) add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);
			else add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
			return;
		case 11:
			if (have_flag(o_ptr->art_flags, TR_KILL_DRAGON)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_DRAGON)) add_flag(o_ptr->art_flags, TR_KILL_DRAGON);
			else add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
			return;
		case 12:
			if (have_flag(o_ptr->art_flags, TR_KILL_DEITY)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_DEITY)) add_flag(o_ptr->art_flags, TR_KILL_DEITY);
			else add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
			return;
		case 13:
			if (have_flag(o_ptr->art_flags, TR_KILL_DEMON)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_DEMON)) add_flag(o_ptr->art_flags, TR_KILL_DEMON);
			else add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
			return;
		case 14:
			if (have_flag(o_ptr->art_flags, TR_KILL_KWAI)) break;
			if (have_flag(o_ptr->art_flags, TR_SLAY_KWAI)) add_flag(o_ptr->art_flags, TR_KILL_KWAI);
			else add_flag(o_ptr->art_flags, TR_SLAY_KWAI);
			return;
		}

	}
}


/*:::����C�����[�`����������*/
static int repair_broken_weapon_aux(int bcost)
{
	s32b cost;
	int item, mater;
	object_type *o_ptr, *mo_ptr; /* broken weapon and material weapon */
	object_kind *k_ptr;
	int i, k_idx, dd_bonus, ds_bonus;
	char basenm[MAX_NLEN];
	cptr q, s; /* For get_item prompt */
	int r_sv=0,r_tv=0,sv_power;
	int row = 7;
	char c;
	int old_weight;

	bool use_weapon;

	/* Clear screen */
	clear_bldg(0, 22);

	/* Notice */
	prt("����̏C���ɂ͍ޗ��Ƃ��čz�΂��ʂ̕�����g���B", row, 2);

	/* Get an item */
	q = "�ǂ̐܂ꂽ������C���������H";
	s = "�C���̕K�v�ȕ��������Ă��Ȃ��悤���ȁB";

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_broken_weapon;

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (0);

	if(!(wield_check(item,INVEN_PACK,0))) return (0);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* It is worthless */
	/* �z�΂��ޗ��ɂ���Ƃ��ɂ͖��ʂƂ������Ȃ��̂ŃG�S�ȏ�łȂ��Ă��g����
	if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr))
	{
		msg_format(_("����͒����Ă����傤���Ȃ����B", "It is worthless to repair."));
		return (0);
	}
	*/
	/* They are too many */
	if (o_ptr->number > 1)
	{
		msg_format("�����Ă���͈̂�x�Ɉ�ɂ��Ă��炨���B");
		return (0);
	}

	/* Display item name */
	object_desc(basenm, o_ptr, OD_NAME_ONLY);
	prt(format("�C�����镐��@�F %s", basenm), row+3, 2);

	/* Get an item */
	q = "�ǂ���C���̍ޗ��Ɏg���H";
	s = "�C���̍ޗ��������Ă��Ȃ��悤���ȁB";

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_repair_material;

	if (!get_item(&mater, q, s, (USE_INVEN | USE_EQUIP))) return (0);
	if (mater == item)
	{
		msg_print("���̘b�𕷂��Ă��Ȃ������̂��H");
		return (0);
	}
	if(!(wield_check(mater,INVEN_PACK,0))) return (0);

	/* Get the item (in the pack) */
	mo_ptr = &inventory[mater];

	/* Display item name */
	object_desc(basenm, mo_ptr, OD_NAME_ONLY);
	prt(format("�ޗ��Ƃ��镐��F %s", basenm), row+4, 2);

	/* Get the value of one of the items (except curses) */
	cost = bcost + (object_value_real(o_ptr) + object_value_real(mo_ptr)) ;

#ifdef JP
	if (!get_check(format("��%d�\���󂯂�B�悢���H ", cost))) return (0);
#else
	if (!get_check(format("Costs %d gold, okay? ", cost))) return (0);
#endif

	/* Check if the player has enough money */
	if (p_ptr->au < cost)
	{
		msg_format("�����Ă��Ȃ��ł͂Ȃ����B�o�����ė����B");
		return (0);
	}


	use_weapon = object_is_melee_weapon(mo_ptr);
	old_weight = o_ptr->weight;

	//��������Tval����
	switch(o_ptr->tval)
	{
	case TV_KNIFE:
		r_tv = TV_KNIFE;
		break;
	case TV_SWORD:
		r_tv = TV_SWORD;
		break;
	case TV_KATANA:
		r_tv = TV_KATANA;
		break;
	case TV_STICK:

		screen_save();
//		for(i=1;i<8;i++)Term_erase(12, i, 255);
		Term_clear();

		msg_print("���̕����ǂ�ȕ���ɂ��ė~�����̂��H");
		put_str("a) �݊킪����", 2 , 40);
		put_str("b) �_������", 3 , 40);
		put_str("c) ��������", 4 , 40);
		put_str("d) ��������", 5 , 40);
		put_str("e) �������킪����", 6 , 40);

		while(1)
		{
			c = inkey();
			if(c >= 'a' && c <= 'e') break;
		}
		screen_load();
		if(c == 'a') r_tv = TV_HAMMER;
		else if(c == 'b') r_tv = TV_STICK;
		else if(c == 'c') r_tv = TV_AXE;
		else if(c == 'd') r_tv = TV_SPEAR;
		else if(c == 'e') r_tv = TV_POLEARM;
		break;

	default:
		msg_format("ERROR:����C����Tval(%d)�̏��������o�^",o_ptr->tval);
		return (0);

	}
	if(!r_tv){ msg_format("ERROR:����C���ŏC����tval��������"); return (0);}


	//��������Sval����
	//���ꏈ���@�Z���C�����ɗ��̗؂��g���Ɨ��؂̒Z���ɂȂ�
	if(r_tv == TV_KNIFE && mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONSCALE)
	{
		 r_sv = SV_WEAPON_DRAGON_DAGGER;
	}
	//���ꏈ���@���C�����Ƀq�q�C���J�l���g���ƃq�q�C���J�l�̌��ɂȂ�
	else if(r_tv == TV_SWORD && mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HIHIIROKANE)
	{
		 r_sv = SV_WEAPON_HIHIIROKANE;
	}
	else if(r_tv == TV_SWORD && mo_ptr->tval == TV_SWORD && mo_ptr->sval == SV_WEAPON_HIHIIROKANE)
	{
		 r_sv = SV_WEAPON_HIHIIROKANE;
	}

	//�ʏ폈���@sv_power��f�ނ��猈��A�K���ɑ��������A�l�ɉ�����Sval����
	else
	{
		sv_power = -1;
		for(i=0; repair_weapon_power_table[i].tval ;i++)
			if(repair_weapon_power_table[i].tval == mo_ptr->tval && repair_weapon_power_table[i].sval == mo_ptr->sval ) 
				sv_power = repair_weapon_power_table[i].type;
		if(sv_power == -1){ msg_format("ERROR:����C���ł��̏C���f�ނ�sv_power���o�^����Ă��Ȃ�"); return (0);}

		//�S���̃p���[�̓����_��
		if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_SCRAP_IRON) sv_power = randint1(18);
		//�A�[�e�B�t�@�N�g�͗ǂ����ɂȂ�₷��
		else if(object_is_artifact(mo_ptr)) sv_power += 3 + randint0(3);

		//+3:42% +6:18% +10:6%
		///mod160206 4��6�ɕύX +3:57% +6:33% +10:15%
		while(!one_in_(6)) sv_power++;
		
		for(i=0; repair_weapon_power_table[i].tval ;i++)
			if(repair_weapon_power_table[i].tval == r_tv && sv_power >= repair_weapon_power_table[i].num &&  repair_weapon_power_table[i].num >= 0) 
				r_sv = repair_weapon_power_table[i].sval;

	}
	if(!r_sv){ msg_format("ERROR:����C���ŏC����sval��������"); return (0);}
	k_idx = lookup_kind(r_tv,r_sv);
	if(!k_idx){ msg_format("ERROR:����C�������ik_idx����������"); return (0);}


	/* Calculate dice bonuses */
	dd_bonus = o_ptr->dd - k_info[o_ptr->k_idx].dd;
	ds_bonus = o_ptr->ds - k_info[o_ptr->k_idx].ds;
	dd_bonus += mo_ptr->dd - k_info[mo_ptr->k_idx].dd;
	ds_bonus += mo_ptr->ds - k_info[mo_ptr->k_idx].ds;

	/* Change base object */
	k_ptr = &k_info[k_idx];
	o_ptr->k_idx = k_idx;
	o_ptr->weight = k_ptr->weight;
	o_ptr->tval = k_ptr->tval;
	o_ptr->sval = k_ptr->sval;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Copy base object's ability */
	for (i = 0; i < TR_FLAG_SIZE; i++) o_ptr->art_flags[i] |= k_ptr->flags[i];
	///mod150403 MAX()�̒���randint1�����Ă��̂ŏC��
	if (k_ptr->pval)
	{
		int tester = randint1(k_ptr->pval);
		if(o_ptr->pval < tester) o_ptr->pval = tester;
	}
	if (have_flag(k_ptr->flags, TR_ACTIVATE)) o_ptr->xtra2 = k_ptr->act_idx;

	add_art_flags(o_ptr); //���Ɋm���Ő؂ꖡ�t�^����Ȃǂ̏���


	//�C���f�ނ�����̂Ƃ��A�f�ޕ���̔\�͂��ꕔ�����p��
	if(object_is_melee_weapon(mo_ptr))
	{
		/* Add one random ability from material weapon */
		give_one_ability_of_object(o_ptr, mo_ptr);

		/* Add to-dam, to-hit and to-ac from material weapon */
		o_ptr->to_d += MAX(0, (mo_ptr->to_d / 3));
		o_ptr->to_h += MAX(0, (mo_ptr->to_h / 3));
		o_ptr->to_a += MAX(0, (mo_ptr->to_a));
	}
	//�C���f�ނ�����ȊO�̃A�C�e���̂Ƃ��A�Ǝ�����
	//���ۉ��O�@�����_���\�͕t�^�A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HEMATITE)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		one_ability(o_ptr);
	}
	//�}�O�l�^�C�g�@�����_�������A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MAGNETITE)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if(one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else add_flag(o_ptr->art_flags, TR_BRAND_COLD);

	}
	//��f�z�΁@�ŎE�A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ARSENIC)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		add_flag(o_ptr->art_flags, TR_BRAND_POIS);
	}
	//����z�΁@�����_���X���C�A�C���l��������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MERCURY)
	{
		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		gain_random_slay_2(o_ptr);
	}
	//�~�X�����@�C���l�������A�d��30%�����A�����_���ϐ�
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_MITHRIL)
	{
		o_ptr->to_d += 3 + randint1(5);
		o_ptr->to_h += 3 + randint1(5);
		o_ptr->weight -= o_ptr->weight * 3 / 10;
		one_resistance(o_ptr);
	}
	//�A�_�}���^�C�g�@�_���[�W�啝�����A���������A�d��50%�����A�_�C�X�{�[�i�X
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ADAMANTITE)
	{
		o_ptr->to_d += 10;
		o_ptr->to_h -= 10;
		o_ptr->weight += o_ptr->weight / 2;
		dd_bonus += 1;
		ds_bonus += 1;
	}
	//���̗� ��ʑϐ�+�C���l������
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONSCALE)
	{
		o_ptr->to_d += 3 + randint1(5);
		o_ptr->to_h += 3 + randint1(5);
		one_high_resistance(o_ptr);
	}
	//覓S�@�C���l�啝�����A�؂ꖡ�������͒n�k�A�����_�������ƃ����_���X���C
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_METEORICIRON)
	{
		if(object_has_a_blade(o_ptr))
		{
			if (have_flag(o_ptr->art_flags, TR_VORPAL)) add_flag(o_ptr->art_flags, TR_EX_VORPAL);
			else add_flag(o_ptr->art_flags, TR_VORPAL);
		}
		else add_flag(o_ptr->art_flags, TR_IMPACT);

		gain_random_slay_2(o_ptr);
		if(one_in_(5)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if(one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if(one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_COLD);	
		else add_flag(o_ptr->art_flags, TR_BRAND_POIS);	

		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);

	}
	//���̒܁@�C���l�啝�����A�_�C�X�u�[�X�g�A�����_���X���C�Apval�u�[�X�g
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_DRAGONNAIL)
	{
		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);
		gain_random_slay_2(o_ptr);
		dd_bonus++;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//�q�q�C���J�l�@���ŋ�����ȏ�ɂȂ�@���łȂ��Ƃ��_�C�X�u�[�X�g�Ɛ؂ꖡ�@�C���l�啝�����@�����_���X���C2�A�����_���ϐ��A�����_�������@
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HIHIIROKANE)
	{
		if(o_ptr->tval != TV_SWORD)
		{
			if(object_has_a_blade(o_ptr))
			{
				add_flag(o_ptr->art_flags, TR_VORPAL);
			}
			dd_bonus++;
		}
		gain_random_slay_2(o_ptr);
		gain_random_slay_2(o_ptr);
		one_high_resistance(o_ptr);
		if(one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if(one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if(one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else add_flag(o_ptr->art_flags, TR_BRAND_COLD);

		o_ptr->to_d += 10;
		o_ptr->to_h += 10;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//�X�̗� ��C�U��+��C�ϐ�+���ϐ�+�C���l�������@�G�S�łȂ��ꍇ�����G�S�ɂȂ�
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ICESCALE)
	{
		if(object_is_artifact(o_ptr) || object_is_ego(o_ptr))
		{
			o_ptr->to_d += 2 + randint1(3);
			o_ptr->to_h += 2 + randint1(3);
			add_flag(o_ptr->art_flags, TR_BRAND_COLD);	
			add_flag(o_ptr->art_flags, TR_RES_COLD);
			add_flag(o_ptr->art_flags, TR_RES_WATER);
		}
		else
		{
			o_ptr->name2 = EGO_WEAPON_SUI_RYU;
			o_ptr->pval = randint1(2);
			o_ptr->to_d += 10 + randint1(4);
			o_ptr->to_h += 10 + randint1(4);
			one_high_resistance(o_ptr);
		}

	}
	//�����̂�����@�C���l�����A�l�ԃX���C
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_NIGHTMARE_FRAGMENT)
	{
		o_ptr->to_d += 5 + randint1(3);
		o_ptr->to_h += 5 + randint1(3);
		if (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN)) add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
		else add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);

	}
	//�΍� �n���ϐ��A���܂ɋz����l�ԃX���C
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_ISHIZAKURA)
	{
		o_ptr->to_d += 2 + randint1(3);
		o_ptr->to_h += 2 + randint1(3);

		add_flag(o_ptr->art_flags, TR_RES_NETHER);
		if(one_in_(2))
		{
			if (have_flag(o_ptr->art_flags, TR_SLAY_HUMAN)) add_flag(o_ptr->art_flags, TR_KILL_HUMAN);
			else add_flag(o_ptr->art_flags, TR_SLAY_HUMAN);
		}
		if(one_in_(3)) add_flag(o_ptr->art_flags, TR_VAMPIRIC);

	}
	//�܂ꂽ�j�@�����A�����_���X���C2�A�C���l����
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_BROKEN_NEEDLE)
	{
		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_THROW);
		gain_random_slay_2(o_ptr);
		gain_random_slay_2(o_ptr);
	}
	//��]�̌����@�C���l�啝�����A�Ύ׃X���C�A�j���A����
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_HOPE_FRAGMENT)
	{
		o_ptr->to_d += 7 + randint1(3);
		o_ptr->to_h += 7 + randint1(3);
		if (have_flag(o_ptr->art_flags, TR_SLAY_EVIL)) add_flag(o_ptr->art_flags, TR_KILL_EVIL);
		else add_flag(o_ptr->art_flags, TR_SLAY_EVIL);
		add_flag(o_ptr->art_flags, TR_BLESSED);
		add_flag(o_ptr->art_flags, TR_LITE);

	}

	//�S�� �S�������_��
	//�~�X�e���E���@�S�������_��������A�d�ʓ�{
	else if(mo_ptr->tval == TV_MATERIAL && (mo_ptr->sval == SV_MATERIAL_SCRAP_IRON || mo_ptr->sval == SV_MATERIAL_MYSTERIUM))
	{
		do
		{
			if(one_in_(5))gain_random_slay_2(o_ptr);
			if(one_in_(4))one_resistance(o_ptr);
			if(one_in_(3))one_ability(o_ptr);
			if(one_in_(9)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
			else if(one_in_(8)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
			else if(one_in_(7)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
			else if(one_in_(6)) add_flag(o_ptr->art_flags, TR_BRAND_COLD);	
			else if(one_in_(5)) add_flag(o_ptr->art_flags, TR_BRAND_POIS);	

			if(one_in_(8)) dd_bonus++;
			if(one_in_(8)) ds_bonus++;
			if(one_in_(3)) o_ptr->to_d += randint1(5);
			if(one_in_(3)) o_ptr->to_h += randint1(5);
			if(one_in_(5)) o_ptr->to_a += randint1(7);
			if(o_ptr->pval && one_in_(7)) o_ptr->pval++;

		}while(weird_luck() || mo_ptr->sval == SV_MATERIAL_MYSTERIUM && one_in_(2));

		if(mo_ptr->sval == SV_MATERIAL_MYSTERIUM) o_ptr->weight *= 2;
	}
	//���̃C�����i�C�g�@�C���l�������A�_�C�X�u�[�X�g�Apval�u�[�X�g
	else if(mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_ILMENITE)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		dd_bonus += 2;
		ds_bonus += 2;
		if(o_ptr->pval) o_ptr->pval++;
	}
	//�s�����̉H�� |��*��r�M+�� �}�H
	else if(mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_PHOENIX_FEATHER)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_IM_FIRE);
		add_flag(o_ptr->art_flags, TR_RES_FIRE);
		add_flag(o_ptr->art_flags, TR_RES_LITE);
		add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		add_flag(o_ptr->art_flags, TR_CON);
		if(!o_ptr->pval) o_ptr->pval = 3 + randint0(2);

	}
	//�G�C�W���̐Ԑ� /X���� /�_ r���M pval�u�[�X�g
	else if(mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_ASIA)
	{

		o_ptr->to_d += 10;
		o_ptr->to_h += 10;
		add_flag(o_ptr->art_flags, TR_RES_HOLY);
		add_flag(o_ptr->art_flags, TR_RES_LITE);
		add_flag(o_ptr->art_flags, TR_KILL_UNDEAD);		
		add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);		
		add_flag(o_ptr->art_flags, TR_KILL_DEMON);		
		add_flag(o_ptr->art_flags, TR_SLAY_DEMON);		
		add_flag(o_ptr->art_flags, TR_SLAY_DEITY);		
		if(o_ptr->pval) o_ptr->pval++;
	}
	//�ɜQ������ /�_ r�� �� pval�u�[�X�g
	else if(mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_IZANAGIOBJECT)
	{

		o_ptr->to_d += 5+randint1(5);
		o_ptr->to_h += 5+randint1(5);
		add_flag(o_ptr->art_flags, TR_RES_TIME);
		add_flag(o_ptr->art_flags, TR_SEE_INVIS);
		add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		if(o_ptr->pval) o_ptr->pval++;
	}
	//���� �C���l�����A/�� ~�� �����_������ �_�C�X�u�[�X�g
	else if (mo_ptr->tval == TV_MATERIAL && mo_ptr->sval == SV_MATERIAL_RYUUZYU)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_SLAY_DRAGON);
		add_flag(o_ptr->art_flags, TR_ESP_DRAGON);

		if (one_in_(4)) add_flag(o_ptr->art_flags, TR_BRAND_ACID);
		else if (one_in_(3)) add_flag(o_ptr->art_flags, TR_BRAND_ELEC);
		else if (one_in_(2)) add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
		else add_flag(o_ptr->art_flags, TR_BRAND_COLD);

		dd_bonus++;

	}

	//��̌� /X�f�_ r�� ���@��x���� �_�C�X�u�[�X�g
	else if (mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_SHILVER_KEY)
	{

		o_ptr->to_d += 5 + randint1(5);
		o_ptr->to_h += 5 + randint1(5);
		add_flag(o_ptr->art_flags, TR_RES_TIME);
		add_flag(o_ptr->art_flags, TR_FREE_ACT);
		add_flag(o_ptr->art_flags, TR_SEE_INVIS);
		add_flag(o_ptr->art_flags, TR_KILL_DEMON);
		add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		add_flag(o_ptr->art_flags, TR_KILL_DEITY);
		add_flag(o_ptr->art_flags, TR_SLAY_DEITY);
		add_flag(o_ptr->art_flags, TR_EASY_SPELL);
		dd_bonus += 2;
		ds_bonus += 2;
	}
	//���̕�� /�����d
	else if (mo_ptr->tval == TV_SOUVENIR && mo_ptr->sval == SV_SOUVENIR_MOON_ORB)
	{

		o_ptr->to_d += randint1(5);
		o_ptr->to_h += randint1(5);
		add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD);
		add_flag(o_ptr->art_flags, TR_SLAY_DEMON);
		add_flag(o_ptr->art_flags, TR_SLAY_KWAI);
		add_flag(o_ptr->art_flags, TR_BLESSED);
	}

	else { msg_format("ERROR:����C���ł��̑f�ނ̏�����������Ă��Ȃ�"); return (0);}



	/* Dice up */
	if (dd_bonus > 0)
	{
		o_ptr->dd++;
		dd_bonus--;
		for (i = 0; i < dd_bonus; i++)
		{
			if (one_in_(o_ptr->dd + i)) o_ptr->dd++;
		}
	}
	if (ds_bonus > 0)
	{
		o_ptr->ds++;
		ds_bonus--;
		for (i = 0; i < ds_bonus; i++)
		{
			if (one_in_(o_ptr->ds + i)) o_ptr->ds++;
		}
	}

	//���_�C�X����̒ǉ��U����pval������
	//�b��t���ǉ��U���t�������Ƃ������ɓ���Ȃ��B���������悤�ɂ��Ƃ��ׂ����B
	if (have_flag(o_ptr->art_flags, TR_BLOWS)  || o_ptr->name1 == ART_NARSIL )
	{
		if(o_ptr->dd * o_ptr->ds > 30)
		{
			if(o_ptr->pval > 2) o_ptr->pval = 1 + randint1(10) / 7;
			else o_ptr->pval = 1;
		}
		else if(o_ptr->dd * o_ptr->ds > 20)
		{
			if(o_ptr->pval > 2) o_ptr->pval = 2;
			else o_ptr->pval = 1;
		}
		else
		{
			if(o_ptr->pval > 3) o_ptr->pval = 3;
		}
	}

	add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
	add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(o_ptr->art_flags, TR_IGNORE_COLD);

//	if ((o_ptr->name1 == ART_NARSIL) ||
	//v1.1.31b �x�b�s�������̕����ɓ��Ă͂܂�悤�ɂ���
	if ((object_is_fixed_artifact(o_ptr)) ||
		(object_is_random_artifact(o_ptr)) ||
		(object_is_ego(o_ptr) && one_in_(7)))
	{

		/* Add one random ability from material weapon */
		if(object_is_melee_weapon(mo_ptr)) give_one_ability_of_object(o_ptr, mo_ptr);
		else
		{
			o_ptr->to_d += randint1(5);
			o_ptr->to_h += randint1(5);
		}

		/* Add one random activation */
		if (!activation_index(o_ptr)) one_activation(o_ptr);

		/* Narsil */
		if (o_ptr->name1 == ART_NARSIL)
		{
			one_high_resistance(o_ptr);
			one_ability(o_ptr);
		}
		if (o_ptr->name1 == ART_ONIKIRIMARU)
			msg_print(_("�܂������̓����Ăь���������悤�Ƃ͂ȁB", "This blade seems to be exceptionally."));
		else
			msg_print(_("����͂��Ȃ�̋ƕ��������悤���B", "This blade seems to be exceptionally."));
	}

	//�߂蕐��ɂ͓����K�����t��
	if (have_flag(o_ptr->art_flags, TR_BOOMERANG)) add_flag(o_ptr->art_flags, TR_THROW);

	object_desc(basenm, o_ptr, OD_NAME_ONLY);
#ifdef JP
	msg_format("�悵�B�C�����ς񂾂��B", cost, basenm);
#else
	msg_format("Repaired into %s for %d gold.", basenm, cost);
#endif
	msg_print(NULL);
	//�b��A�C�e�������ɂ���
	//�b��ς݃A�C�e����t�r�_�g���̗d��(xtra3=SPECIAL_ESSENCE_OTHER)�̏ꍇxtra3���ێ������B
	if(!object_is_smith(o_ptr)) o_ptr->xtra3 = SPECIAL_ESSENCE_ONI;

	///mod150311 �C������G�b�Z���X�����o�O�ɑΉ�
	/*:::Hack - �b��t�ƕ���t�̂Ƃ��̂ݓ���t���OON ����C���ƃG�b�Z���X�t���𗼕��s�������ƁA�G�b�Z���X�����������Ƃ��ɂ�����C�����ꂽ�������킩��悤��*/
	if(p_ptr->pclass == CLASS_JEWELER || p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_MISUMARU) o_ptr->xtra1 = 1;

	/* Remove BROKEN flag */
	o_ptr->ident &= ~(IDENT_BROKEN);

	//v1.1.25 ���莞���x���ƃ^�[�����L�^
	o_ptr->create_turn = turn;
	o_ptr->create_lev = dun_level;

	/* Add repaired flag */
	o_ptr->discount = 99;

	///mod141129 �d�ʍČv�Z
	p_ptr->total_weight -= old_weight;
	p_ptr->total_weight += o_ptr->weight;

	/* Decrease material object */
	inven_item_increase(mater, -1);
	inven_item_optimize(mater);

	/* Copyback */
	p_ptr->update |= PU_BONUS;
	handle_stuff();

	/* Something happened */
	return (cost);
}


/*
 * Repair broken weapon
 */
///sys ����C���@���΂炭�͖���
#if 0
static int repair_broken_weapon_aux(int bcost)
{
	s32b cost;
	int item, mater;
	object_type *o_ptr, *mo_ptr; /* broken weapon and material weapon */
	object_kind *k_ptr;
	int i, k_idx, dd_bonus, ds_bonus;
	char basenm[MAX_NLEN];
	cptr q, s; /* For get_item prompt */
	int row = 7;

	/* Clear screen */
	clear_bldg(0, 22);

	/* Notice */
	prt(_("�C���ɂ͍ޗ��ƂȂ����1�̕��킪�K�v�ł��B", "Hand one material weapon to repair a broken weapon."), row, 2);
	prt(_("�ޗ��Ɏg�p��������͂Ȃ��Ȃ�܂��I", "The material weapon will disappear after repairing!!"), row+1, 2);

	/* Get an item */
	q = _("�ǂ̐܂ꂽ������C�����܂����H", "Repair which broken weapon? ");
	s = _("�C���ł���܂ꂽ���킪����܂���B", "You have no broken weapon to repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_broken_weapon;

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (0);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* It is worthless */
	if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr))
	{
		msg_format(_("����͒����Ă����傤���Ȃ����B", "It is worthless to repair."));
		return (0);
	}

	/* They are too many */
	if (o_ptr->number > 1)
	{
		msg_format(_("��x�ɕ������C�����邱�Ƃ͂ł��܂���I", "They are too many to repair at once!"));
		return (0);
	}

	/* Display item name */
	object_desc(basenm, o_ptr, OD_NAME_ONLY);
	prt(format(_("�C�����镐��@�F %s", "Repairing: %s"), basenm), row+3, 2);

	/* Get an item */
	q = _("�ޗ��ƂȂ镐��́H", "Which weapon for material? ");
	s = _("�ޗ��ƂȂ镐�킪����܂���B", "You have no material to repair.");

	/* Only forge broken weapons */
	item_tester_hook = item_tester_hook_melee_weapon;

	if (!get_item(&mater, q, s, (USE_INVEN | USE_EQUIP))) return (0);
	if (mater == item)
	{
		msg_print(_("�N���C���̒ق���Ȃ��I", "This is not a klein bottle!"));
		return (0);
	}

	/* Get the item (in the pack) */
	mo_ptr = &inventory[mater];

	/* Display item name */
	object_desc(basenm, mo_ptr, OD_NAME_ONLY);
	prt(format(_("�ޗ��Ƃ��镐��F %s", "Material : %s"), basenm), row+4, 2);

	/* Get the value of one of the items (except curses) */
	cost = bcost + object_value_real(o_ptr) * 2;

#ifdef JP
	if (!get_check(format("��%d������܂�����낵���ł����H ", cost))) return (0);
#else
	if (!get_check(format("Costs %d gold, okay? ", cost))) return (0);
#endif

	/* Check if the player has enough money */
	if (p_ptr->au < cost)
	{
		object_desc(basenm, o_ptr, OD_NAME_ONLY);
		msg_format(_("%s���C�����邾���̃S�[���h������܂���I",
			"You do not have the gold to repair %s!"), basenm);
		msg_print(NULL);
		return (0);
	}

	if (o_ptr->sval == SV_BROKEN_DAGGER)
	{
		int i, n = 1;

		/* Suppress compiler warning */
		k_idx = 0;

		for (i = 1; i < max_k_idx; i++)
		{
			object_kind *k_ptr = &k_info[i];

			if (k_ptr->tval != TV_SWORD) continue;
			if ((k_ptr->sval == SV_BROKEN_DAGGER) ||
				(k_ptr->sval == SV_BROKEN_SWORD) ||
				(k_ptr->sval == SV_DOKUBARI)) continue;
			if (k_ptr->weight > 99) continue;

			if (one_in_(n)) 
			{
				k_idx = i;
				n++;
			}
		}
	}
	else /* TV_BROKEN_SWORD */
	{
		/* Repair to a sword or sometimes material's type weapon */
		int tval = (one_in_(5) ? mo_ptr->tval : TV_SWORD);

		while(1)
		{
			object_kind *ck_ptr;

			k_idx = lookup_kind(tval, SV_ANY);
			ck_ptr = &k_info[k_idx];

			if (tval == TV_SWORD)
			{
				if ((ck_ptr->sval == SV_BROKEN_DAGGER) ||
					(ck_ptr->sval == SV_BROKEN_SWORD) ||
					(ck_ptr->sval == SV_DIAMOND_EDGE) ||
					(ck_ptr->sval == SV_DOKUBARI)) continue;
			}
			if (tval == TV_POLEARM)
			{
				if ((ck_ptr->sval == SV_DEATH_SCYTHE) ||
					(ck_ptr->sval == SV_TSURIZAO)) continue;
			}
			if (tval == TV_HAFTED)
			{
				if ((ck_ptr->sval == SV_GROND) ||
					(ck_ptr->sval == SV_WIZSTAFF) ||
					(ck_ptr->sval == SV_NAMAKE_HAMMER)) continue;
			}

			break;
		}
	}

	/* Calculate dice bonuses */
	dd_bonus = o_ptr->dd - k_info[o_ptr->k_idx].dd;
	ds_bonus = o_ptr->ds - k_info[o_ptr->k_idx].ds;
	dd_bonus += mo_ptr->dd - k_info[mo_ptr->k_idx].dd;
	ds_bonus += mo_ptr->ds - k_info[mo_ptr->k_idx].ds;

	/* Change base object */
	k_ptr = &k_info[k_idx];
	o_ptr->k_idx = k_idx;
	o_ptr->weight = k_ptr->weight;
	o_ptr->tval = k_ptr->tval;
	o_ptr->sval = k_ptr->sval;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Copy base object's ability */
	for (i = 0; i < TR_FLAG_SIZE; i++) o_ptr->art_flags[i] |= k_ptr->flags[i];
	if (k_ptr->pval) o_ptr->pval = MAX(o_ptr->pval, randint1(k_ptr->pval));
	if (have_flag(k_ptr->flags, TR_ACTIVATE)) o_ptr->xtra2 = k_ptr->act_idx;

	/* Dice up */
	if (dd_bonus > 0)
	{
		o_ptr->dd++;
		for (i = 0; i < dd_bonus; i++)
		{
			if (one_in_(o_ptr->dd + i)) o_ptr->dd++;
		}
	}
	if (ds_bonus > 0)
	{
		o_ptr->ds++;
		for (i = 0; i < ds_bonus; i++)
		{
			if (one_in_(o_ptr->ds + i)) o_ptr->ds++;
		}
	}

	/* */
	if (have_flag(k_ptr->flags, TR_BLOWS))
	{
		int bmax = MIN(3, MAX(1, 40 / (o_ptr->dd * o_ptr->ds)));
		o_ptr->pval = MIN(o_ptr->pval, bmax);
	}

	/* Add one random ability from material weapon */
	give_one_ability_of_object(o_ptr, mo_ptr);

	/* Add to-dam, to-hit and to-ac from material weapon */
	o_ptr->to_d += MAX(0, (mo_ptr->to_d / 3));
	o_ptr->to_h += MAX(0, (mo_ptr->to_h / 3));
	o_ptr->to_a += MAX(0, (mo_ptr->to_a));

	if ((o_ptr->name1 == ART_NARSIL) ||
		(object_is_random_artifact(o_ptr) && one_in_(1)) ||
		(object_is_ego(o_ptr) && one_in_(7)))
	{
		/* Forge it */
		if (object_is_ego(o_ptr))
		{
			add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
			add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
		}

		/* Add one random ability from material weapon */
		give_one_ability_of_object(o_ptr, mo_ptr);

		/* Add one random activation */
		if (!activation_index(o_ptr)) one_activation(o_ptr);

		/* Narsil */
		if (o_ptr->name1 == ART_NARSIL)
		{
			one_high_resistance(o_ptr);
			one_ability(o_ptr);
		}

		msg_print(_("����͂��Ȃ�̋ƕ��������悤���B", "This blade seems to be exceptionally."));
	}

	object_desc(basenm, o_ptr, OD_NAME_ONLY);
#ifdef JP
	msg_format("��%d��%s�ɏC�����܂����B", cost, basenm);
#else
	msg_format("Repaired into %s for %d gold.", basenm, cost);
#endif
	msg_print(NULL);

	/* Remove BROKEN flag */
	o_ptr->ident &= ~(IDENT_BROKEN);

	/* Add repaired flag */
	o_ptr->discount = 99;

	/* Decrease material object */
	inven_item_increase(mater, -1);
	inven_item_optimize(mater);

	/* Copyback */
	p_ptr->update |= PU_BONUS;
	handle_stuff();

	/* Something happened */
	return (cost);
}
#endif

static int repair_broken_weapon(int bcost)
{
	int cost;

	screen_save();
	cost = repair_broken_weapon_aux(bcost);
	screen_load();
	return cost;
}

/*
 * Enchant item
 */
/*:::�X�ɂ�镐��h��C���l�㏸(�|��܂�)
 *:::������ĂԑO��item_tester_hook��ݒ肵�Ă���
 */
static bool enchant_item(int cost, int to_hit, int to_dam, int to_ac)
{
	int         i, item;
	bool        okay = FALSE;
	object_type *o_ptr;
	cptr        q, s;
	int         maxenchant = (p_ptr->lev / 5);
	char        tmp_str[MAX_NLEN];

	clear_bldg(4, 18);
#ifdef JP
	prt(format("���݂̂��Ȃ��̋Z�ʂ��ƁA+%d �܂ŉ��ǂł��܂��B", maxenchant), 5, 0);
	prt(format(" ���ǂ̗����͈�ɂ���%d �ł��B", cost), 7, 0);
#else
	prt(format("  Based on your skill, we can improve up to +%d.", maxenchant), 5, 0);
	prt(format("  The price for the service is %d gold per item.", cost), 7, 0);
#endif

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�������ǂ��܂����H";
	s = "���ǂł�����̂�����܂���B";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif
	/*:::����h��̃C���x���g���ԍ��𓾂�Bitem_tester_hook�g�p*/
	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (FALSE);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* Check if the player has enough money */
	if (p_ptr->au < (cost * o_ptr->number))
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%s�����ǂ��邾���̃S�[���h������܂���I", tmp_str);
#else
		msg_format("You do not have the gold to improve %s!", tmp_str);
#endif

		return (FALSE);
	}

	/*:::�����Ŏw�肵���񐔕��A�C�e���̏C���l�𑝉�
�@  /*:::�i�����o�O�������Bbreak�̂�����to_???��2�ȏ゠���Ă�1�����オ��Ȃ��Ǝv�����j*/
	/* Enchant to hit */
	for (i = 0; i < to_hit; i++)
	{
		if (o_ptr->to_h < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOHIT | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to damage */
	for (i = 0; i < to_dam; i++)
	{
		if (o_ptr->to_d < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TODAM | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to AC */
	for (i = 0; i < to_ac; i++)
	{
		if (o_ptr->to_a < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOAC | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_print("���ǂɎ��s�����B");
#else
		msg_print("The improvement failed.");
#endif

		return (FALSE);
	}
	else
	{
		object_desc(tmp_str, o_ptr, OD_NAME_AND_ENCHANT);
#ifdef JP
		msg_format("��%d��%s�ɉ��ǂ��܂����B", cost * o_ptr->number, tmp_str);
#else
		msg_format("Improved into %s for %d gold.", tmp_str, cost * o_ptr->number);
#endif

		/* Charge the money */
		p_ptr->au -= (cost * o_ptr->number);

		if (item >= INVEN_RARM) calc_android_exp();

		/* Something happened */
		return (TRUE);
	}
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
/*:::�A�C�e���Ə[�U�����w��@���s���Ȃ��@*/
static void building_recharge(void)
{
	int         item;
	object_type *o_ptr;
	object_kind *k_ptr;
	cptr        q, s;
	int         price;
	int         charges;
	int         max_charges;
	char        tmp_str[MAX_NLEN];

	msg_flag = FALSE;

	/* Display some info */
	clear_bldg(4, 18);
#ifdef JP
prt("  �ď[�U�̔�p�̓A�C�e���̎�ނɂ��܂��B", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif


	/* Only accept legal items */
	//item_tester_hook = item_tester_hook_recharge;
	//v1.1.86 �A�r���e�B�J�[�h�[�U�p�̃��[�`����g�񂾂��A�[�U�L��ɂ���Ƌ�������̂ŋ֎~�ɂ���
	item_tester_hook = item_tester_hook_staff_rod_wand;

	/* Get an item */
#ifdef JP
q = "�ǂ̃A�C�e���ɖ��͂��[�U���܂���? ";
s = "���͂��[�U���ׂ��A�C�e�����Ȃ��B";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif
	/*:::�A�C�e�����w�肷��B ����̃A�C�e�����w��ł��邪����������Ȃ̂Ŏ��������@���������Ȃ��Ƃ܂������R�ł�����̂��H*/
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

	k_ptr = &k_info[o_ptr->k_idx];

	/*
	 * We don't want to give the player free info about
	 * the level of the item or the number of charges.
	 */
	/* The item must be "known" */
	/*�Ӓ肳��ĂȂ����50$�ŊӒ�@�S�[�U�̂Ƃ��͊Ӓ藿�������Ă�*/
	if (!object_is_known(o_ptr))
	{
#ifdef JP
msg_format("�[�U����O�ɊӒ肳��Ă���K�v������܂��I");
#else
		msg_format("The item must be identified first!");
#endif

		msg_print(NULL);

		if ((p_ptr->au >= 50) &&
#ifdef JP
get_check("��50�ŊӒ肵�܂����H "))
#else
			get_check("Identify for 50 gold? "))
#endif

		{
			/* Pay the price */
			p_ptr->au -= 50;

			/* Identify it */
			identify_item(o_ptr);

			/* Description */
			object_desc(tmp_str, o_ptr, 0);

#ifdef JP
msg_format("%s �ł��B", tmp_str);
#else
			msg_format("You have: %s.", tmp_str);
#endif

			/* Auto-inscription */
			autopick_alter_item(item, FALSE);

			/* Update the gold display */
			building_prt_gold();
		}
		else
		{
			return;
		}
	}


	/*:::�[�U���i�v�Z*/
	/* Price for a rod */
	if (o_ptr->tval == TV_ROD)
	{
		int lev = k_info[o_ptr->k_idx].level;
		if (o_ptr->timeout > 0)
		{
			/* Fully recharge */
			/*:::���b�h�̏[�U���i�v�Z�@���b�h��pval�͏[�U�^�[�����Atimeout�͎c��[�U�^�[�����Ǝv����*/
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
		}
		else
		{
			/* No recharge necessary */
			price = 0;
#ifdef JP
msg_format("����͍ď[�U����K�v�͂���܂���B");
#else
			msg_format("That doesn't need to be recharged.");
#endif

			return;
		}
	}

	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		int rank = ability_card_list[o_ptr->pval].rarity_rank;
		int base_charge = ability_card_list[o_ptr->pval].charge_turn;

		if (o_ptr->timeout > 0)
		{
			//���ʂ���Ȏ��N����񂪃Q�[���̃o�[�W�����A�b�v�Ń`���[�W���Ԑݒ��h��ɘM������N���邩�������
			if (base_charge < 1)
			{
				msg_format("ERROR:���̃A�C�e���̃^�C���A�E�g�l�ݒ肪��������");
				o_ptr->timeout = 0;
				return;
			}

			price = (rank * rank * 500 * o_ptr->timeout) / base_charge;
		}
		else
		{
			price = 0;
			msg_format("����͍ď[�U����K�v�͂���܂���B");
			return;
		}


	}

	else if (o_ptr->tval == TV_STAFF)
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (k_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}
	else
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (k_info[o_ptr->k_idx].cost / 10);

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}

	/* Limit the number of charges for wands and staffs */
	if (o_ptr->tval == TV_WAND
		&& (o_ptr->pval / o_ptr->number >= k_ptr->pval))
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("���̖��@�_�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("These wands are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("���̖��@�_�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("This wand is already fully charged.");
#endif
		}
		return;
	}
	else if (o_ptr->tval == TV_STAFF && o_ptr->pval >= k_ptr->pval)
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("���̏�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("These staffs are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("���̏�͂����[���ɏ[�U����Ă��܂��B");
#else
			msg_print("This staff is already fully charged.");
#endif
		}
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au < price)
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
msg_format("%s���ď[�U����ɂ́�%d �K�v�ł��I", tmp_str,price );
#else
		msg_format("You need %d gold to recharge %s!", price, tmp_str);
#endif

		return;
	}

	/*:::���b�h�Ȃ�[�U���邩�ǂ����m�F*/
	if (o_ptr->tval == TV_ROD || o_ptr->tval == TV_ABILITY_CARD)
	{
		if (get_check(format("��%d �ōď[�U���܂����H", price)))
		{
			/* Recharge fully */
			/*:::���b�h�̏[�U�����@�^�C���A�E�g0��*/
			o_ptr->timeout = 0;
		}
		else
		{
			return;
		}
	}
	/*:::��A�_�͉񐔂��m�F*/
	else
	{
		if (o_ptr->tval == TV_STAFF)
			max_charges = k_ptr->pval - o_ptr->pval;
		else
			max_charges = o_ptr->number * k_ptr->pval - o_ptr->pval;

		/* Get the quantity for staves and wands */
#ifdef JP
charges = get_quantity(format("��񕪁�%d �ŉ��񕪏[�U���܂����H",
#else
		charges = get_quantity(format("Add how many charges for %d gold? ",
#endif

			      price), MIN(p_ptr->au / price, max_charges));

		/* Do nothing */
		if (charges < 1) return;

		/* Get the new price */
		price *= charges;

		/* Recharge */
		/*:::��̏[�U�����Ɓi��j�̖����O��*/
		o_ptr->pval += charges;

		/* We no longer think the item is empty */
		o_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Give feedback */
	object_desc(tmp_str, o_ptr, 0);
#ifdef JP
msg_format("%s����%d �ōď[�U���܂����B", tmp_str, price);
#else
	msg_format("%^s %s recharged for %d gold.", tmp_str, ((o_ptr->number > 1) ? "were" : "was"), price);
#endif

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au -= price;

	/* Finished */
	return;
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
/*:::���ɂ��S�Ă̖�����[�U�@�C���x���g���S�Ă��ΏۂɂȂ�ȊO�͒P�̂̂Ƃ��Ɠ���*/
static void building_recharge_all(void)
{
	int         i;
	object_type *o_ptr;
	object_kind *k_ptr;
	int         price = 0;
	int         total_cost = 0;


	/* Display some info */
	msg_flag = FALSE;
	clear_bldg(4, 18);
#ifdef JP
	prt("  �ď[�U�̔�p�̓A�C�e���̎�ނɂ��܂��B", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Calculate cost */
	for ( i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
				
		/* skip non magic device */
		//v1.1.86 �A�r���e�B�J�[�h�����ŏ��������@����Ŏ����������炱�̔��莮�͍����ĂȂ���������̕���ŕςȂ̂͒e�����̂Ŗ��Ȃ������炵��
		//if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		//��͂�A�r���e�B�J�[�h�̏[�U�͒��~
		//if (!item_tester_hook_recharge(o_ptr)) continue;
		if (!item_tester_hook_staff_rod_wand(o_ptr)) continue;

		/* need identified */
		if (!object_is_known(o_ptr)) total_cost += 50;


		k_ptr = &k_info[o_ptr->k_idx];

		switch (o_ptr->tval)
		{
		case TV_ROD:
			{
				int lev = k_info[o_ptr->k_idx].level;
				price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
				break;
			}

		case TV_STAFF:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (k_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (k_ptr->pval - o_ptr->pval) * price;
			break;

		case TV_WAND:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (k_info[o_ptr->k_idx].cost / 10);

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (o_ptr->number * k_ptr->pval - o_ptr->pval) * price;
			break;

			//v1.1.86 �A�r���e�B�J�[�h�̏[�U...�������~����
			/*
		case TV_ABILITY_CARD:
			{

				int rank = ability_card_list[o_ptr->pval].rarity_rank;
				int base_charge = ability_card_list[o_ptr->pval].charge_turn;

				if (o_ptr->timeout > 0)
				{
					//���ʂ���Ȏ��N����񂪃Q�[���̃o�[�W�����A�b�v�Ŕ����^�A�C�e���������^�A�C�e���ɕς����炱���ɗ���
					if (base_charge < 1)
					{
						msg_format("ERROR:���̃A�C�e���̃^�C���A�E�g�l�ݒ肪��������");
						o_ptr->timeout = 0;
						return;
					}

					price = (rank * rank * 500 * o_ptr->timeout) / base_charge;
				}

			}
			break;
			*/
		}

		/* if price <= 0 then item have enough charge */
		if (price > 0) total_cost += price;
	}

	if (!total_cost)
	{
#ifdef JP
		msg_print("�[�U����K�v�͂���܂���B");
#else
		msg_print("No need to recharge.");
#endif

		msg_print(NULL);
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au < total_cost)
	{
#ifdef JP
		msg_format("���ׂẴA�C�e�����ď[�U����ɂ́�%d �K�v�ł��I", total_cost );
#else
		msg_format("You need %d gold to recharge all items!",total_cost);
#endif

		msg_print(NULL);
		return;
	}

#ifdef JP
	if (!get_check(format("���ׂẴA�C�e���� ��%d �ōď[�U���܂����H",  total_cost))) return;
#else
	if (!get_check(format("Recharge all items for %d gold? ", total_cost))) return;
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];

		/* skip non magic device */
		//if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;
		if (!item_tester_hook_recharge(o_ptr)) continue;

		/* Identify it */
		if (!object_is_known(o_ptr))
		{
			identify_item(o_ptr);

			/* Auto-inscription */
			autopick_alter_item(i, FALSE);
		}

		/* Recharge */
		switch (o_ptr->tval)
		{
		case TV_ROD:
		//case TV_ABILITY_CARD:
			o_ptr->timeout = 0;
			break;
		case TV_STAFF:
			if (o_ptr->pval < k_ptr->pval) o_ptr->pval = k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		case TV_WAND:
			if (o_ptr->pval < o_ptr->number * k_ptr->pval)
				o_ptr->pval = o_ptr->number * k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		}
	}

	/* Give feedback */
#ifdef JP
	msg_format("��%d �ōď[�U���܂����B", total_cost);
#else
	msg_format("You pay %d gold.", total_cost);
#endif

	msg_print(NULL);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au -= total_cost;

	/* Finished */
	return;
}

/*:::�X�ړ��@�h���Ɛ�p���@*/
///mod140403 �s���悤�܂��w��ł��Ȃ������̂ŏC���@���łɐ�p���@�̊X�ړ��ł͂ǂ̊X�ɂ��s����悤�ɂ����B
//v1.1.87 �_��_�o��������g���ꍇ�ǉ������𕥂��Ζ��o�^�̏ꏊ�ɍs���邱�Ƃɂ���
//is_magic:���@�̂Ƃ�TRUE,�����R�}���h�̂Ƃ�FALSE ���@�̂Ƃ��͍s�����ꏊ�̂���X�S�Ăɍs����
bool tele_town(bool is_magic)
{
	int i, x, y;
	int new_town_idx = 0;
	int num = 0;
	bool town_ok[TOWN_MAX];

	//v1.1.87 ���o�^�̊X�ɍs���Ƃ��̒ǉ��R�X�g 0�͕s��
	//�C���f�b�N�X��TOWN_***�Ɉ�v
	//v2.0.12 ���n���X���͖��@�̐X����s����悤�ɂȂ�����30000����10000�ɒl��������
	int town_cost[TOWN_MAX] = { 0,0,5000,10000,0,3000,0,0,1000,0,0,0,0,0,0 };

	for(i=0;i<TOWN_MAX;i++) town_ok[i] = FALSE;

	if (dun_level)
	{
#ifdef JP
		msg_print("���̖��@�͒n��ł����g���Ȃ��I");
#else
		msg_print("This spell can only be used on the surface!");
#endif
		return FALSE;
	}

	if(p_ptr->word_recall)
	{
		msg_print("���͋A�ґ҂������B");
		return FALSE;
	}

	if (p_ptr->inside_arena || p_ptr->inside_battle)
	{
#ifdef JP
		msg_print("���̖��@�͊O�ł����g���Ȃ��I");
#else
		msg_print("This spell can only be used outside!");
#endif
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	//�s����ꏊ�̔���ƊX�ꗗ�̕\��
	for (i=1;i<TOWN_MAX;i++)
	{
		char buf[80];
		///sys ���ړ��ňړ��ł��Ȃ��X�𔻒肵�Ă��镔��
		///mod140329 �n�[�h�R�[�f�B���O�ɂ����B����ɐ�p�̊X�ړ��ł͂ǂ̊X�ɂ��s����悤�ɁB
		//���̂���town_cost[]�Ƃ��Ɠ������ăt���O���܂Ƃ߂���
		//if ((i == NO_TOWN) || (i == SECRET_TOWN) || (i == p_ptr->town_num) || !(p_ptr->visit & (1L << (i-1)))) continue;
		if( (!is_magic && i != TOWN_HITOZATO && i != TOWN_TENGU && i != TOWN_CHITEI && i != TOWN_HAKUREI  && i != TOWN_KOURIN )
			&& !(p_ptr->pclass == CLASS_MARISA && i == TOWN_KIRISAME)
			&& !(CLASS_IS_PRISM_SISTERS && i == TOWN_UG)
			) continue;
		//�����̂���X�̓p�X
		if (i == p_ptr->town_num) continue;

		//v1.1.87 �s�������Ƃ̂Ȃ��ꏊ��e���B�������{�ݗ��p�̏ꍇ�ǉ������𕥂��΍s����ꏊ�̓��X�g�ɓ����
		if (!(p_ptr->visit & (1L << (i - 1))))
		{
			if (is_magic) continue;

			if (!town_cost[i]) continue;
			sprintf(buf, "%c) %-20s (�ǉ�����$%d)", I2A(i - 1), town[i].name,town_cost[i]);
		}
		else
		{
			sprintf(buf, "%c) %-20s", I2A(i - 1), town[i].name);
		}

		town_ok[i]= TRUE;
		prt(buf, 5+i, 5);
		num++;
	}

	if (!num)
	{
#ifdef JP
		msg_print("�܂��s����Ƃ��낪�Ȃ��B");
#else
		msg_print("You have not yet visited any town.");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}

	//�ړ��\�t���O�̗������X��I������
	while(1)
	{
		prt("�ǂ��ɍs���܂���:", 0, 0);

		i = inkey();

		if (i == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		new_town_idx = i - 'a' + 1;

		if (new_town_idx <= 0 || new_town_idx >= TOWN_MAX) continue;
		if(!town_ok[new_town_idx])continue;

		if (!(p_ptr->visit & (1L << (new_town_idx - 1))))
		{
			prt("���̊X�ɂ͍s�������Ƃ��Ȃ��B�ǉ������𕥂��ΘA��čs���Ă����炵���B", 0, 0);
			inkey();
			if (get_check_strict("�x�����܂����H", CHECK_DEFAULT_Y))
			{
				if (p_ptr->au < 300 + town_cost[new_town_idx])//�{���̗��p��$300���n�[�h�R�[�f�B���O
				{
					msg_print("����������Ȃ��B");
					continue;
				}
				p_ptr->au -= town_cost[new_town_idx];
			}
			else
			{
				continue;
			}
		}

		break;
	}


	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == new_town_idx)
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}

	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	screen_load();
	return TRUE;
}


//v1.1.32 �����ɏ��R�}���h �l���̎��_��
bool riding_ropeway(void)
{
	int i, x, y;
	int num = 0;
	int new_town;

	if(p_ptr->word_recall)
	{
		msg_print("���͋A�ґ҂������B");
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	if(p_ptr->town_num == TOWN_HITOZATO)
	{
		prt("�u���_�Ђ܂ł̗D��ȋ�̗��͔@���ł����H", 4, 0);
		prt("�@��q���t���Ĉ��S���S�I����͉����킸��$500�ł��I�v", 5, 0);
		new_town = TOWN_MORIYA;
	}
	else
	{
		prt("�u�l���ɂ��A��ł����H", 4, 0);
		prt("�@�A�H�͖����ƂȂ��Ă���܂��B�v", 5, 0);
		new_town = TOWN_HITOZATO;
	}

	if (!get_check_strict("���܂����H", CHECK_DEFAULT_Y))
	{
		screen_load();
		return FALSE;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == new_town)
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}
	clear_bldg(4, 20);

	//�t���[�o�[���b�Z�[�W
	switch(randint1(7))
	{
	case 1:
		if(p_ptr->lev > 30)
		{
			prt("��q�̔��T�V�炪�A", 10, 10);
			prt("�u���̐l��q�v��Ȃ���ˁH�v�ƌ��������ɂ��Ȃ������Ă���...", 11, 10);
			break;
		}
		//lev30�ȉ���2��
	case 2: case 3: 
		prt("���Ȃ��͑��̊Ԃ̋�̗����y���񂾁B", 10, 10);
		break;
	case 4:
		prt("�V�畗�������������N���������A", 10, 10);
		prt("���Ȃ��̃S���h���ɓ͂��O�ɑj�܂�ĎU�����B", 11, 10);
		break;
	default:
		prt("���Ȃ��̓S���h������]�ޔ������i�F���y���񂾁B", 10, 10);
		break;
	}


	prt("�����L�[�������Ă�������", 0, 0);
	(void)inkey();
	prt("", 0, 0);


	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	teleport_town_mode = TELE_TOWN_MODE_ROPEWAY;
	screen_load();
	return TRUE;
}



//�n�����
//v1.1.91 ���@�̐X�̋��n���s�X
bool pass_through_chirei_koudou(void)
{
	int i, x, y;
	int num = 0;
	int new_town;

	if (p_ptr->word_recall)
	{
		msg_print("���͋A�ґ҂������B");
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 20);

	if (p_ptr->town_num == TOWN_KIRISAME)
	{
		prt("�L�����A�����ւƑ����Ă���B", 4, 0);
		prt("�@������ʂ�Βn��ɍs����͂���...", 5, 0);
		new_town = TOWN_CHITEI;
	}
	else
	{
		prt("�n��̖��@�̐X�ւƒʂ��Ă��铴�A���B", 4, 0);
		new_town = TOWN_KIRISAME;
	}

	if (!get_check_strict("���A�ɓ���܂����H", CHECK_DEFAULT_Y))
	{
		screen_load();
		return FALSE;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if (wilderness[y][x].town == new_town)
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}
	clear_bldg(4, 20);

	//�t���[�o�[���b�Z�[�W
	switch (randint1(5))
	{
	case 1:
		if (p_ptr->pclass != CLASS_MARISA)
		{
			prt("������ו�������ē��S���̖��@�g���Ƃ�������..", 10, 10);
			break;
		}
		//go through
	case 2:
		if (p_ptr->pclass != CLASS_YAMAME)
		{
			prt("�y�w偂����Q�����Ă���...", 10, 10);
			break;
		}
		//go through
	default:
		prt("���Ȃ��͔��Â����A�̒�������n�߂�...", 10, 10);
		break;
	}


	prt("�����L�[�������Ă�������", 0, 0);
	(void)inkey();
	prt("", 0, 0);


	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	teleport_town_mode = TELE_TOWN_MODE_CHIREIKOUDOU;
	screen_load();
	return TRUE;
}

/*
 *  research_mon
 *  -KMW-
 */
/*:::�����X�^�[��l */
static bool research_mon(void)
{
	int i, n, r_idx;
	char sym, query;
	char buf[128];

	bool notpicked;

	bool recall = FALSE;

	u16b why = 0;

	u16b	*who;

	/* XTRA HACK WHATSEARCH */
	bool    all = FALSE;
	bool    uniq = FALSE;
	bool    norm = FALSE;
	char temp[80] = "";

	/* XTRA HACK REMEMBER_IDX */
	static int old_sym = '\0';
	static int old_i = 0;


	/* Save the screen */
	screen_save();

	/* Get a character, or abort */
	/*:::�ꕶ�����͂��󂯂�*/
#ifdef JP
if (!get_com("�����X�^�[�̕�������͂��ĉ�����(�L�� or ^A�S,^U��,^N��,^M���O):", &sym, FALSE)) 
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE))
#endif

	{
		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Find that character info, and describe it */
	/*:::����for����i���ő�ȍ~�܂ő�����Ƃ��ƂŃ}�b�`�Ȃ��Ƃ��Ēe�����B^�����͕ʁB*/
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

		/* XTRA HACK WHATSEARCH */
		/*:::����ȕ����w��̂Ƃ��̓t���O�𗧂Ă�*/
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
	/*:::���O����*/
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

			/* Restore */
			screen_load();

			return FALSE;
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
	prt(buf, 16, 10);


	/* Allocate the "who" array */
	/*:::*who�̗̈�������X�^�[�̐��ɂ��킹�Ċm�ہA���������Ă���炵���H*/
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	/*:::r_info[]����w�肵���t���O�������ɍ��������X�^�[��I�肵�Awho[]�Ƀ����X�^�[�C���f�b�N�X�����X�g�A�b�v���Ă���*/
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];


		///mod140712 �σp�����[�^�����X�^�[�̓����X�^�[��l����Q�Ƃł��Ȃ��悤�ɂ���
		if(r_ptr->flags7 & RF7_VARIABLE) continue;
		//�e�X�g�d���Ɠ���t�B�[���h�͏o���Ȃ�
		if(i == MON_TEST || i == MON_EXTRA_FIELD) continue;

		/* Empty monster */
		if (!r_ptr->name) continue;

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* ���O���� */
		if (temp[0])
		{
			int xx;
			char temp2[80];

			for (xx = 0; temp[xx] && xx < 80; xx++)
			{
#ifdef JP
				if (iskanji(temp[xx]))
				{
					xx++;
					continue;
				}
#endif
				if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
			}
  
#ifdef JP
			strcpy(temp2, r_name + r_ptr->E_name);
#else
			strcpy(temp2, r_name + r_ptr->name);
#endif
			for (xx = 0; temp2[xx] && xx < 80; xx++)
				if (isupper(temp2[xx])) temp2[xx] = tolower(temp2[xx]);

#ifdef JP
			if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp))
#else
			if (my_strstr(temp2, temp))
#endif
				who[n++] = i;
		}
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Sort by level */
	why = 2;
	query = 'y';

	/* Sort if needed */
	/*:::�����X�^�[�����x�����Ƀ\�[�g�H����if�ɂ͕K������Ǝv�������̈Ӗ�������̂�*/
	if (why)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	/* XTRA HACK REMEMBER_IDX */
	if (old_sym == sym && old_i < n) i = old_i;
	else i = n - 1;

	notpicked = TRUE;

	/* Scan the monster memory */
	/*:::�����X�^�[����\�����A-�ƃX�y�[�X�Ő؂�ւ��Ar���������炻�̃����X�^�[�̏���S�ĕ\�����L������B*/
	while (notpicked)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Begin the prompt */
		roff_top(r_idx);

		/* Hack -- Complete the prompt */
#ifdef JP
Term_addstr(-1, TERM_WHITE, " ['r'�v���o, ' '�ő��s, ESC]");
#else
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC, space to continue]");
#endif


		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/*** Recall on screen ***/

				/* Get maximal info about this monster */
				/*:::�w�肵�������X�^�[�̑S�Ă̏����L������*/
				lore_do_probe(r_idx);

				/* Save this monster ID */
				/*:::�v���o�E�B���h�E�ɕ\��*/
				monster_race_track(r_idx);

				/* Hack -- Handle stuff */
				handle_stuff();

				/* know every thing mode */
				/*:::���C���E�B���h�E�㕔�Ƀ����X�^�[�̎v���o��\������*/
				screen_roff(r_idx, 0x01);
				notpicked = FALSE;

				/* XTRA HACK REMEMBER_IDX */
				old_sym = sym;
				old_i = i;
			}

			/* Command */
			query = inkey();

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


	/* Re-display the identity */
	/* prt(buf, 5, 5);*/

	/* Free the "who" array */
	C_KILL(who, max_r_idx, u16b);

	/* Restore */
	screen_load();

	return (!notpicked);
}

///mod140112 �\�������Ȃ����̂ŕ����s�\���̂��߂ɍ���Ă݂��B����ݔ��̐����\���ɂ��g���\��B
/*:::rumors_new.txt�����s�ǂݍ����\�ŉ��s�������s�ɕ����ĕ\������B*/
void display_rumor_new(int town_num)
{
	cptr rumor_ptr;
	char buf[1024];
	//char buf2[1024];
	bool err;
	int i;

	//v1.1.92 ������ς���ex_rumor_new()�Ő��l�ݒ肷�邱�Ƃɂ���
		//bool extra_grassroot_rumor = FALSE;
		//v1.1.26 EXTRA�p�\�̔����character_icky��ǉ��B�ʓe�̃��C�V�����ł��\�𕷂���̖Y��Ă�
		//v1.1.27 icky>1�ɂ�����S�����ʓe�p�\�ɂȂ����̂ŏC���B
		//if(EXTRA_MODE && character_icky)
		//	extra_grassroot_rumor = TRUE;

	//v1.1.18 EXTRA�p�\
	if(town_num == 23 && ex_buildings_param[f_info[cave[py][px].feat].subtype]== 255)
	{
		msg_print("���̍��d���l�b�g���[�N�̎ҒB�͂������Ȃ��B");
		return;
	}

	//LUNATIC�ł͕ςȉ\�ɂȂ�
	if (difficulty == DIFFICULTY_LUNATIC && (town_num < 7)) town_num = 99;

	screen_save();


	//v1.1.18 EXTRA�\�ǉ�
	//v1.1.92 ������ς���ex_rumor_new()�Ő��l�ݒ肷�邱�Ƃɂ���
	//else if(extra_grassroot_rumor) town_num = 23;


	err = get_rnd_line("rumors_new.txt", town_num, buf);
	if (err) strcpy(buf, "ERROR:���̔ԍ��̉\�͒�`����Ă��Ȃ��B");

	//roff_to_buf(buf, 74, buf2, sizeof(buf2));
	//rumor_ptr = buf2;
	rumor_ptr = buf;

	/*:::Mega Hack ����x�����Ă݂�������:-) */
	/*:::���̕��͂�'\'�������Ă��'\0'�ɕϊ����A�s����\0\0�ɂ���B*/
	/*:::�蓮���s��roff_to_buf()�݂����Ȃ���B*/
	for(i=0;i < sizeof(buf)-2;i++)
	{
		if(buf[i] == '\0')
			break; 
		/*:::�ꕔ���������s��������Ă��܂��̂�}�~�B����Ő������̂��ǂ����܂��������M���Ȃ��B*/
		else if(iskanji(buf[i]))
		{
			i++;
			continue;
		}
		else if(buf[i] == '\\') buf[i] = '\0';
	}
	buf[i] = '\0';
	buf[i+1] = '\0';

	Term_erase(14, 3, 255);
	/*:::15�s�܂ŕ\������*/
	for (i = 0; i< 15; i++)
	{
		if(rumor_ptr[0] == 0)
			break; 
		else
		{
			if(town_num == 0) prt(rumor_ptr, 4+i, 15);
			else prt(rumor_ptr, 4+i, 5);
			Term_erase(14, 5+i, 255);
			rumor_ptr += strlen(rumor_ptr) + 1;
		}
	}

	prt("�����L�[�������Ă�������", 0, 0);
	(void)inkey();
	prt("", 0, 0);

	screen_load();	

}

/*:::�P��Ƙb������@������A�T�[�y���g�N�G��̐�p*/
static void kaguya_quest(void)
{
		int tmp,i;
		quest_type      *q_ptr;

		//v2.0.19 ��ՓxNORMAL�Ŏ��œ|��ɗ̈�ɍs���Ă��܂��̂Ńt���O������������
		bool flag_enter_serpen = FALSE;

		if (quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED)
			flag_enter_serpen = TRUE;

		q_ptr = &quest[QUEST_SERPENT];

		if(q_ptr->status > QUEST_STATUS_UNTAKEN) return;

		if(!flag_enter_serpen)
		{
			if(p_ptr->pclass == CLASS_EIRIN)
				msg_format("�P��u�y���݂ˉi�ԁI���������̂���R����ꂻ������B�v");
			else if(p_ptr->pclass == CLASS_KAGUYA)
				msg_format("�~�͂͂܂��������Ă��Ȃ��B");
			else
				msg_format("�P��u���l�����Ƃ����Ă���́B�����������ƂŖK�˂Ă��āB�v");
			return;
		}

		clear_bldg(4, 18);
		//�i�ԂƋP��̂Ƃ��Z���t��ς���
		if(p_ptr->pclass == CLASS_EIRIN)
			display_rumor_new(98);
		else if(p_ptr->pclass == CLASS_KAGUYA)
		{
			bool flag = FALSE;

			for(i=0;i<KAGUYA_QUEST_MAX;i++)
			{
				if(a_info[kaguya_quest_art_list[i]].cur_num) flag = TRUE;
			}
			if(flag)
				display_rumor_new(97);
			else
				display_rumor_new(96);
		}
		//v1.1.18 ���ϐ��͂̓Z���t�ύX
		else if(p_ptr->pclass == CLASS_CLOWNPIECE || p_ptr->pclass == CLASS_VFS_CLOWNPIECE || p_ptr->pclass == CLASS_JUNKO || p_ptr->pclass == CLASS_HECATIA)
			display_rumor_new(94);
		//v1.1.13 �푰�����̖��̂Ƃ��䎌�ύX
		else if(p_ptr->prace == RACE_LUNARIAN)
			display_rumor_new(95);
		else
			display_rumor_new(p_ptr->town_num);
		tmp = p_ptr->inside_quest;
		p_ptr->inside_quest = QUEST_SERPENT;
		init_flags |= INIT_ASSIGN;
		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		q_ptr->status = QUEST_STATUS_TAKEN;
		p_ptr->inside_quest = tmp;

}



/*:::�e�T�Ƃ̉�b�B@�����̍��l�b�g���[�N�ɉ����ł��邩�ǂ��������肷��*/
static void talk_kagerou()
{
	bool flag_ok;
	int count;

	/*
	if(p_ptr->pclass == CLASS_BANKI)
	{
		msg_print("���Ȃ��͎����̓��̕��g���o���Ęb�������Ă݂��B�������ڐV���������͕Ԃ��Ă��Ȃ������B");
		return;
	}
	*/
	if(EXTRA_MODE && ex_buildings_param[f_info[cave[py][px].feat].subtype]== 255)
	{
		msg_print("���̍��d���l�b�g���[�N�̎ҒB�͂������Ȃ��B");
		return;
	}

	/*:::�����ς݁@��x����������Q�����i�������Ă����������ςȂ��ɂ��Ă���*/
	if(p_ptr->grassroots)
	{
		if(p_ptr->pclass == CLASS_KAGEROU) 
			msg_print("�킩�����P�Ɛ��Ԙb�������B");
		else if(EXTRA_MODE)
			msg_print("�e�T�u�݂�ȂŊ撣���ĉ���ڎw���܂��傤�B������ł��Ȃ�ׂ��ǂ����o����B�v");
		else 
			msg_print("���Ȃ��Ɖe�T�͐_�ЂɎ����킵���Ȃ����Ԙb�������B");
		return;
	}

	/*:::�����\����*/

	//���̍��Ǝ��̒ǉ�����
	switch(p_ptr->prace)
	{
	/*:::�l�Ԃ͉����s�@�������ψق�10�ȏ゠���OK���ꂽ��*/
	//v1.1.85 �����������ύX�B�ψق̐��łȂ��Ǔے��Ǝ���������g�����Ƃɂ���
		/*
	case RACE_HUMAN:
	case RACE_KOBITO:
		if(count_mutations() > 9) flag_ok=TRUE;
		else flag_ok = FALSE;
		break;
	case RACE_HALF_YOUKAI:
	case RACE_HALF_GHOST:
		if(count_mutations() > 4) flag_ok=TRUE;
		else flag_ok = FALSE;
		break;
		*/

		//v1.1.70 ������́u�������v�̂ݎQ���\
	case RACE_ANIMAL_GHOST:
		if (CHECK_ANIMAL_GHOST_STRIFE == ANIMAL_GHOST_STRIFE_NONE)
			flag_ok = TRUE;
		else 
			flag_ok = FALSE;
		break;

	/*:::�l�O�͊�{������*/
	case RACE_YOUKAI:
	case RACE_FAIRY:
	case RACE_MAGICIAN:
	case RACE_WARBEAST:
	case RACE_TSUKUMO:
	case RACE_ANDROID:
	case RACE_YOUKO:
	case RACE_BAKEDANUKI:
	case RACE_NINGYO:
	case RACE_NYUDOU:
	case RACE_IMP:
	case RACE_GOLEM:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		flag_ok = TRUE;
		break;

	//�l�ԑ��͊�{�����s�����A�ϐg�Ƃ�����������ł���悤�ɂ���B
	case RACE_HUMAN:
	case RACE_KOBITO:
	case RACE_GREATER_LILLIPUT:
	case RACE_JK:
	case RACE_DEMIGOD:
	case RACE_HALF_YOUKAI:
	case RACE_HALF_GHOST:
	case RACE_ZASHIKIWARASHI://���~��炵�͗d�����͂���̃X�p�C������ʗd������͐l�ԑ��ƔF������Ă���͂��H
	case RACE_MAGIC_JIZO:
		if (!player_looks_human_side()) flag_ok = TRUE;
		else flag_ok = FALSE;

	break;

	//�ق��A����ȑ��݂Ƃ��Ė����m��Ă�푰�╁�ʗd�����ɂȂ肻���ɂȂ��푰��ʏ���ɉ��炩�̐��͂ɏ������Ă�푰�͉����s�B
	//TODO:���i�u�͂��ꕨ�v��ǉ����A���̂ւ�̐�������������Ă�������������Ȃ�
	default:
		flag_ok = FALSE;
		break;

	}
	//���j�[�N�N���X�͈ꕔ�����������s��
	if(!(cp_ptr->flag_join_grassroots)) flag_ok = FALSE;

	if(flag_ok)
	{
		msg_print("���Ȃ��ɂ͑��̍��d���l�b�g���[�N�ւ̎Q�����i������炵���B");
		if (get_check_strict("�Q�����܂����H", CHECK_DEFAULT_Y))
		{
			if(EXTRA_MODE)
			{
				int ex_bldg_num = f_info[cave[py][px].feat].subtype;
				msg_print("�e�T�u�݂�ȐV�������Ԃ�I���ꂩ�����낵���ˁB�v");

				//-Hack- �����ɉ\�R�}���h��ǉ����čĕ`�悷��
				sprintf(building[ex_bldg_num].act_names[6],"�\�𕷂�");
				building[ex_bldg_num].letters[6] = 'r';
				building[ex_bldg_num].actions[6] = BACT_RUMORS_NEW;

				show_building(&building[ex_bldg_num]);
			}
			else
			{
				msg_print("�e�T�u�F�ɂ͘b��ʂ��Ă�����B�܂����傭���傭����o���ĂˁB�v");
			}
			p_ptr->grassroots = 1;
			//�얲�̐肢���Z�b�g
			p_ptr->today_mon = 0;
		}
	}
	else 
	{
		if(EXTRA_MODE)
			msg_print("�e�T�u�v��Ȃ��A�C�e������������������Ă����Ȃ��H�v");
		else if(p_ptr->pclass == CLASS_EIKI)
			msg_print("���Ȃ��͉e�T�ɐ����������B");
		else
			msg_print("���Ȃ��Ɖe�T�͂ǂ�����X�������Ԙb�������B");
	}
	return;
}


/*:::��ǐ_�l�̐_���^�C�v���m�F����*/
static void reimu_deity_check()
{
	cptr comment_cosmos[7] = {"���ׂ̋ɒv","����","���e","����","����","�Ύ��I","�����̌���"};
	cptr comment_warlike[7] = {"�����̎���","�F��","����","����","���\","����","�j��̉��g"};
	cptr comment_noble[7] = {"���|�̌���","����","����","����","�D�]","�l�C��","��]�̏ے�"};

	if(p_ptr->prace != RACE_STRAYGOD)
	{
		msg_print("ERROR:��ǐ_�l�ȊO��deity_check()���Ă΂ꂽ");
		return;
	}
	msg_print("�얲�u�����ƁA�M���̕]���͂ˁE�E�v");

	c_put_str(TERM_WHITE,format("�����x�F%s",comment_cosmos[p_ptr->race_multipur_val[DBIAS_COSMOS] / 37]),8 , 6);
	c_put_str(TERM_WHITE,format("�D��x�F%s",comment_warlike[p_ptr->race_multipur_val[DBIAS_WARLIKE] / 37]),9 , 6);
	c_put_str(TERM_WHITE,format("�l�C�x�F%s",comment_noble[p_ptr->race_multipur_val[DBIAS_REPUTATION] / 37]),10 , 6);

	return;
}


/*:::�얲�̐肢�@���̍��l�b�g���[�N������͗��p�s��*/
/*�g�p�ϐ��F
 *p_ptr->today_mon �肢�̌��ʂ̔ԍ����i�[�@����0:00��0�Ƀ��Z�b�g
 *p_ptr->barter_value �^�[�Q�b�g�_���W�����A�����X�^�[�A�K���Ȃǂ��i�[�@����͈�x�������ꂽ��0�ɂȂ�
 */
static bool reimu_fortune_teller()
{
	/*:::���̍��l�b�g���[�N������͗��p�s��*/
	if(p_ptr->grassroots)
	{
		msg_print("�얲�͐���Ă���Ȃ������B�C�����Ȃ��炵���B");
		return FALSE;
	}
	/*:::�����x�̂�*/
	if(p_ptr->today_mon)
	{
		msg_print("�얲�u���������������Ȃ��B�����͂����X�d������B�v");
		return FALSE;
	}
	msg_print("�얲�̓P�[�v�Ɛ���������������o���Ă����B");

	if(one_in_(25)) c_put_str(TERM_WHITE,"�ʓ|�L����ˁE�E",8 , 6);
	else if(one_in_(4)) c_put_str(TERM_WHITE,"�ȂɂȂɁE�E",8 , 6);
	else if(one_in_(3)) c_put_str(TERM_WHITE,"�ق��ق��E�E",8 , 6);
	else if(one_in_(2)) c_put_str(TERM_WHITE,"���炠��E�E",8 , 6);
	else				c_put_str(TERM_WHITE,"�ӂނӂށE�E",8 , 6);

	(void)inkey();

	while(1)
	{
		bool fail = FALSE;
		int tester = randint1(10);

		if(tester < 4) p_ptr->today_mon = FORTUNETELLER_TREASURE;
		else if(tester < 7) p_ptr->today_mon = FORTUNETELLER_TROPHY;
		else if(tester < 10) p_ptr->today_mon = FORTUNETELLER_GOODLUCK;
		else  p_ptr->today_mon = FORTUNETELLER_BADLUCK;

		switch(p_ptr->today_mon)
		{
		//����_���W�����̓���K�ɂȂɂ��ǂ�������
		case FORTUNETELLER_TREASURE:
			{
				int target_dungeon;
				int target_level;
				int lev_check = p_ptr->lev + (p_ptr->lev * p_ptr->lev / 50);
				int count = 0;
				do
				{
					if(one_in_(3))
					{
						target_dungeon = DUNGEON_ANGBAND;
						if(max_dlv[target_dungeon])
						{
							target_level = max_dlv[target_dungeon] + randint1(16) - 10;
							if(target_level > d_info[target_dungeon].maxdepth) target_level = d_info[target_dungeon].maxdepth;
							if(target_level < 1) target_level = randint1(5);
						}
						else target_level = randint1(5);
					}
					else
					{
						target_dungeon = 1 + randint1((p_ptr->total_winner)?11:10);
						target_level = rand_range(d_info[target_dungeon].mindepth,d_info[target_dungeon].maxdepth);
					}
					count++;
					if(count>100)
					{
						fail = TRUE;
						break;
					}

					//v1.1.55 �^�[�Q�b�g���S���ȊO�̂Ƃ��A1/2�̊m���Ńt���A�����𖳎�����悤�ɂ���

				}while((target_dungeon == DUNGEON_ANGBAND || one_in_(2)) && (target_level > lev_check * 12 / 10 || target_level < lev_check * 4 / 5));
				if(!fail)
				{
					p_ptr->barter_value = target_dungeon * 1000 + target_level;
					c_put_str(TERM_WHITE,format("%s��%d�K�ɂ��󂠂�A�Əo����I",(d_name + d_info[target_dungeon].name),target_level),9 , 6);
				}
			}
			break;
		//���̓��̂����ɓ��胆�j�[�N��|���ƍ����i���h���b�v
		//m_idx��p_ptr->barter_value�֊i�[
		//���̃��x���`���x��*2�̖��œ|�œ���t���O�̂Ȃ����j�[�N�����X�^�[
		case FORTUNETELLER_TROPHY:
			{
				int i;
				int count = 0;
				int target_idx = 0;
				int lev_check = p_ptr->lev + (p_ptr->lev * p_ptr->lev / 50) + 5;
				for(i=1;i<max_r_idx;i++)
				{
					monster_race *r_ptr = &r_info[i];
					if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;
					if (r_ptr->r_akills) continue;
					if (r_ptr->flags1 & RF1_QUESTOR) continue;
					if (r_ptr->flags1 & RF1_FORCE_DEPTH) continue;
					if (r_ptr->level > lev_check) continue;
					if (r_ptr->level < p_ptr->lev) continue;
					if (r_ptr->rarity > 100) continue; //v2.0.11�ǉ� ���ꃂ���X�^�[��o��֎~�����X�^�[
					count++;
					if(one_in_(count)) target_idx = i;

				}
				if(!target_idx)
				{
					fail = TRUE;
				}
				else
				{
					char m_name[80];
					strcpy(m_name, (r_name + r_info[target_idx].name));
					p_ptr->barter_value = target_idx;
					c_put_str(TERM_WHITE,format("%s��|���ƍK�����K���A�Əo����I",m_name),9 , 6);
				}
			}
			break;
		//�����I�[����肿����Ɨǂ��A�C�e�������A���A�C�e���̋��z��������
		case FORTUNETELLER_GOODLUCK:
			c_put_str(TERM_WHITE,"�����̋M���̉^�C�͐�D����I",9 , 6);
			break;
		//�����I�[���Ɠ����̃A�C�e�������A�G�������x�����X�u�[�X�g�AHORDE�������ܔ{
		case FORTUNETELLER_BADLUCK:
			c_put_str(TERM_WHITE,"�E�E�����͉Ƃő�l�������Ă������ق������������ˁB",9 , 6);
			break;
		default:
			msg_print("ERROR:����`�̐肢����");
			return FALSE;
		}
		if(!fail) break;
	}
	return TRUE;
}



//v1.1.86
//�����R�}���h:�A�r���e�B�J�[�h�̌�����
//������grassroots_trading_cards()�Ɠ�������H
void trading_ability_cards(void)
{

	int ref_pval, ref_num, ref_cost, ref_totalcost; //�v���C���[���n�����J�[�h�̏��
	int cs;
	char c;
	cptr q, s;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	cptr msg_select; //�J�[�h��I�ԂƂ��̃��b�Z�[�W
	cptr msg_reject; //�񎦂��ꂽ�J�[�h�ɍ����J�[�h��������Ȃ������Ƃ��̃��b�Z�[�W
	cptr msg_finish; //�����I�������b�Z�[�W
	char o_name[MAX_NLEN];
	int item;

	int i, j, k;

	int trade_chance, trade_num; //�I�肳�������J�[�h��␔,���ۂɑI�肳�ꂽ�J�[�h��
	object_type barter_list[10];

	//v2.0.7 �疒�v���C���ɂ̓J�[�h�����N�����\������
	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		chimata_comment_card_rank();
		return;
	}

	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print("���͉ו�����t���B");
		return;
	}

	if (!(EXTRA_MODE) && is_daytime())
	{
		if (one_in_(3))
			c_put_str(TERM_WHITE, "�u�ِ��E����̐N�����Ȃ�āI�����s�ꂪ���Ӑ��������ςȂ���B�v", 10, 20);
		else if (one_in_(2))
			c_put_str(TERM_WHITE, "�u�U�V�I�̃C�x���g�ɂ͎Q�����Ă��ꂽ������H�v", 10, 20);
		else
			c_put_str(TERM_WHITE, "�u�s�ꂪ�J���͖̂�ɂȂ��Ă����B�v", 10, 20);

		return;
	}
	else if (ability_card_trade_count > 2 && !p_ptr->wizard)
	{
		if(EXTRA_MODE)
			c_put_str(TERM_WHITE, "�u����̌����͏I����B�܂��ʂ̃t���A�ŉ�܂��傤�B�v", 10, 20);
		else
			c_put_str(TERM_WHITE, "�u�����̎s��͏I����B�܂������̔ӂɗ��ĂˁB�v", 10, 20);
		return;

	}

	//�����ɏo���J�[�h��I�я��擾

	q = "�u�����A�M���������ɏo���J�[�h��I�Ԃ̂ł��B�v";
	s = "�u����A�A�r���e�B�J�[�h�������Ă��Ȃ��̂ˁB�v";
	msg_select = "�Q���҂����̓J�[�h��񎦂����B";
	msg_reject = "�u�c�O�����ǁA�N���M���̏o�����J�[�h�Ɍ������J�[�h�������Ă��Ȃ��悤�ˁB�v";

	if(EXTRA_MODE)
		msg_finish = "���F�ɓh�肽����ꂽ�����ŃJ�[�h�̌������s��ꂽ�B";
	else
		msg_finish = "�����̉��ŃJ�[�h�̌������s��ꂽ�B";


	item_tester_tval = TV_ABILITY_CARD;
	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];

	ref_num = 1; //�A�r���e�B�J�[�h�����͏�Ɉꖇ����
	ref_pval = o_ptr->pval;

	if (ref_pval < 0 || ref_pval >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:trading_ability_cards()�ɕs���ȃJ�[�hidx(%d)���������A�r���e�B�J�[�h���n���ꂽ", ref_pval);
		return;
	}

	for (i = 0; i<10; i++) object_wipe(&barter_list[i]);

	//�������̃J�[�h�𐶐�����
	//�������N�̃J�[�h�͌������肪���Ȃ�
	trade_chance = 5 + (p_ptr->lev / 2 + randint1(p_ptr->lev / 2)) / 9;
	if (ability_card_list[ref_pval].rarity_rank == 4) trade_chance = trade_chance * 3 / 4;
	else if (ability_card_list[ref_pval].rarity_rank >= 3) trade_chance = trade_chance * 4 / 5;


	trade_num = 0;

	//�J�[�h�������J��Ԃ�
	for (i = 0; i<trade_chance && i<10; i++)
	{
		int j;
		int cnt = 0;
		int tmp_pv;

		//�J�[�h���X�g��S���`�F�b�N
		for (j = 0; j<ABILITY_CARD_LIST_LEN; j++)
		{
			int k;
			bool flag_ok = TRUE;
			//�ʏ퐶������Ȃ��J�[�h�͒e��
			if (ability_card_list[j].rarity_rank<1 || ability_card_list[j].rarity_rank>4) continue;
			//�����񎦂����J�[�h�Ɠ����̂͒e��
			if (j == ref_pval) continue;
			//���łɑI�肳�ꂽ�J�[�h�Ɠ����̂͒e�� 
			for (k = 0; k<trade_num; k++) if (j == barter_list[k].pval) flag_ok = FALSE;
			if (!flag_ok) continue;

			//�o�����J�[�h�̔����ȉ��̌������l�̃J�[�h�͒e��
			if (ability_card_list[j].trade_value < ability_card_list[ref_pval].trade_value / 2) continue;
			//�o�����J�[�h����̌������l�̃J�[�h�͊m���Œe��
			if (randint1(ability_card_list[ref_pval].trade_value / 2) < (ability_card_list[j].trade_value - ability_card_list[ref_pval].trade_value)) continue;

			//�o�����J�[�h����̃����N�̃J�[�h��*����*�o�ɂ�������
			//�����N3��4��_���ꍇ�A�������l����Ŗ�80%,���̔����0.8%,�ʂ����J�[�h���烉���_���ɑI�ԂƂ����7%���炢�̊m����ʂ�K�v������͂��B6����₪����Ƃ��Ă��o����0.3%�B���3��g���[�h���Ă�1%��B
			if (ability_card_list[ref_pval].rarity_rank < ability_card_list[j].rarity_rank)
			{
				if (ability_card_list[ref_pval].rarity_rank == 3)
				{
					if(!one_in_(128)) continue;
				} 
				else if (ability_card_list[ref_pval].rarity_rank == 2)
				{
					if (!one_in_(64)) continue;
				}
				else
				{
					if (!one_in_(8)) continue;
				}
			}

			//�����܂Œʂ����J�[�h���烉���_���ɑI��
			cnt++;
			if (!one_in_(cnt)) continue;
			tmp_pv = j;
		}
		//�I�����ꂽ�J�[�h������ꍇbarter_list�ɓo�^
		if (cnt)
		{
			int tmp_num;
			object_type *tmp_o_ptr = &barter_list[trade_num];
			object_prep(tmp_o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
			tmp_o_ptr->pval = tmp_pv;

			//*�Ӓ�*�ςɂ���
			object_aware(tmp_o_ptr);
			object_known(tmp_o_ptr);
			tmp_o_ptr->ident |= (IDENT_MENTAL);


			trade_num++;
		}
	}

	//2�ȉ��̏ꍇ�ꍇ�I��
	if (trade_num < 2)
	{
		c_put_str(TERM_WHITE, msg_reject, 4, 6);
		inkey();
		ability_card_trade_count++;
		return;
	}

	//�J�[�h��\�����I��
	cs = 0;
	while (1)
	{
		c_put_str(TERM_WHITE, msg_select, 4, 6);
		c_put_str(TERM_WHITE, "(�������Ȃ��FESC)", 5, 6);
		for (i = 0; i<trade_num; i++)
		{
			int color = TERM_WHITE;
			object_type *tmp_o_ptr = &barter_list[i];
			if (cs == i) color = TERM_YELLOW;
			else if (ability_card_list[barter_list[i].pval].rarity_rank > ability_card_list[ref_pval].rarity_rank)
				color = TERM_VIOLET;
			else if (ability_card_list[barter_list[i].pval].rarity_rank < ability_card_list[ref_pval].rarity_rank)
				color = TERM_BLUE;

			object_aware(tmp_o_ptr);
			object_known(tmp_o_ptr);
			object_desc(o_name, tmp_o_ptr, 0);
			//msg_format("%d:%s",o_value,o_name);
			c_put_str(color, format("%c) %s", 'a' + i, o_name), 6 + i, 10);
		}
		c = inkey();
		if (c == ESCAPE)
		{
			//if (get_check_strict("���̃t���A�ł͂����g���[�h�ł��܂���B��낵���ł����H", CHECK_OKAY_CANCEL)) 
				break;
		}
		else if (c == '2' && cs < (trade_num - 1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c = cs + 'a';
		else if (c >= 'a' && c <= 'a' + trade_num - 1)
		{
			object_desc(o_name, &barter_list[c - 'a'], 0);
			
			if (ability_card_list[barter_list[c - 'a'].pval].rarity_rank < ability_card_list[ref_pval].rarity_rank)
				msg_format("%s�͂��Ȃ����o�����J�[�h��胉���N���Ⴂ�B",o_name);
			else
				msg_format("%s���󂯎��B", o_name);

			if (!get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) continue;

			//�I���������������
			inven_item_increase(item, -ref_num);
			inven_item_describe(item);
			inven_item_optimize(item);
			inven_carry(&barter_list[c - 'a']);
			msg_format("%s���󂯎�����B", o_name);

			break;
		}
	}

	clear_bldg(4, 18);
	c_put_str(TERM_WHITE, msg_finish, 10, 10);
	ability_card_trade_count++;
	inkey();
	clear_bldg(4, 18);


	//���s�ς݃t���O���Ă�
	//ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;

}



//v1.1.87
//�����R�}���h:�A�r���e�B�J�[�h�̔̔���
//examine:�J�[�h�̐�������ǂ�
void buy_ability_card(bool examine)
{
	int trade_num = 10;
	int cs, item_new;
	char c;
	object_type *o_ptr;
	cptr msg_select; //�J�[�h��I�ԂƂ��̃��b�Z�[�W
	char o_name[MAX_NLEN];
	bool flag_no_item = TRUE;

	bool browse_mode = FALSE;

	int i, j, k;

	object_type barter_list[10];

	if (p_ptr->pclass == CLASS_CHIMATA)
	{
		int card_rank = CHIMATA_CARD_RANK;

		if (card_rank < 3)
		{
			c_put_str(TERM_WHITE, "�u������͂��łɏ������[�ł���B", 10, 20);
			c_put_str(TERM_WHITE, "���i�̊m�ۂ͋M���Ɍ������Ă����ł�����撣���Ă��������ˁH�v", 11, 20);
		}
		else if (card_rank < 5)
		{
			c_put_str(TERM_WHITE, "�u����n�߂܂�����I", 10, 20);
			c_put_str(TERM_WHITE, "�ʔ����Ȃ��Ă�����[�I�v", 11, 20);
		}
		else if (card_rank < 8)
		{
			c_put_str(TERM_WHITE, "�u��ɐ��ł���I", 10, 20);
			c_put_str(TERM_WHITE, "��͂�эj�ۗl�̖ڂɋ����͂���܂���ł����ˁB�v", 11, 20);
		}
		else
		{
			c_put_str(TERM_WHITE, "�u���A�Z����...", 10, 20);
			c_put_str(TERM_WHITE, "���낻�뗘�v�m�肵�Ď�d�����ɂ��܂���...�H�v", 11, 20);
		}

		return;
	}

	if (p_ptr->pclass == CLASS_TSUKASA)
	{
		msg_print("��̓J�[�h�ŏ\���ɉ҂����B���������ɗp�͂Ȃ��B");
		return;
	}


	if (p_ptr->pclass == CLASS_MIKE)
	{
		msg_print("�J�[�h�̔����ɂ͏����L�������Q��Ă��ċߊ�肽���Ȃ��B");
		return;
	}

	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		c_put_str(TERM_WHITE, "�u�ς݂܂���˂��A�����̓J�[�h�̔��l��p�̎s��Ȃ�ł���B�v", 10, 20);
		return;
	}


	if (inventory[INVEN_PACK - 1].k_idx && !examine)
	{
		msg_print("���͉ו�����t���B");
		return;
	}

	if(examine)
		msg_select = "�u�ӂނӂށA�ǂ�����m�F�Ȃ����܂����H�v";
	else
		msg_select = "�u���Ă��āA�ǂ���������グ�ɂȂ�܂����H�v";

	//make_ability_card_store_list()�Ō��܂����J�[�hidx���X�g���g���č݌Ƀ��X�g�����
	for (i = 0; i<10; i++)
	{
		int card_idx = p_ptr->magic_num2[i+ ABLCARD_MAGICNUM_SHIFT];

		if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
		{
			msg_format("ERROR:buy_ability_cards()�ɓn���ꂽcard_idx(%d)���͈͊O", card_idx);
			return;
		}

		o_ptr = &barter_list[i];
		object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(o_ptr, card_idx, 0, 0);
		flag_no_item = FALSE;
	}

	//���Ԍo�߂ō݌ɂ̈ꕔ�𔄂�؂�ɂ���H

	//if (flag_no_item)
	//{
	//	c_put_str(TERM_WHITE, "�u���A�l�ō݌ɂ��S�ĎJ�����Ⴂ�܂����B�܂����x���Ă��������B�v", 10, 20);
	//	return;
	//}
	//���t���O�̏������~�X���ĂđS���݌ɔ�������Ă����̃��b�Z�[�W�͏o�Ȃ��B�܂��債�����ł��Ȃ��̂ŏ�����


	//�J�[�h��\�����I��
	cs = 0;
	while (1)
	{
		c_put_str(TERM_WHITE, msg_select, 4, 6);
		c_put_str(TERM_WHITE, "(�L�����Z���FESC)", 5, 6);
		for (i = 0; i<trade_num; i++)
		{
			int value;
			int color;
			cptr desc;
			int price = 0;
			o_ptr = &barter_list[i];


			//pval=0(�󔒂̃J�[�h)�͔���؂ꈵ��
			if (!o_ptr->pval)
			{
				color = TERM_L_DARK;
				desc = format("(����؂�)");

			}
			else
			{
				color = TERM_WHITE;
				price = calc_ability_card_price(o_ptr->pval);

				//�u���{��`�̃W�����}�v�̃J�[�h�ɂ�鉿�i������
				//p_ptr->magic_num2[10-19)��50�����l�Ƃ���1���Ƃ�2%�㉺����
				if (p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT])
				{
					int old_price = price;
					price += price * (p_ptr->magic_num2[10 + i + ABLCARD_MAGICNUM_SHIFT] - 50) * 2 / 100;
					if (cheat_peek) msg_format("price:%d -> %d", old_price, price);
				}

				object_desc(o_name, o_ptr, 0);
				desc = format("%c) %-50s  $%d", 'a' + i, o_name, price);

			}
			if (cs == i) color = TERM_YELLOW;

			c_put_str(color, desc, 6 + i, 10);
		}
		c = inkey();
		if (c == ESCAPE)
		{
			//if (get_check_strict("���̃t���A�ł͂����g���[�h�ł��܂���B��낵���ł����H", CHECK_OKAY_CANCEL)) 
			break;
		}
		else if (c == '2' && cs < (trade_num - 1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c = cs + 'a';
		else if (c >= 'a' && c <= 'a' + trade_num - 1)
		{
	
			int price;
			int pos = c - 'a';
			o_ptr = &barter_list[pos];

			if (!o_ptr->pval)
			{
				msg_print("����͔���؂ꂾ�B");
				continue;
			}

			if (examine)//����
			{

				hack_flag_store_examine = TRUE;
				screen_object(o_ptr, SCROBJ_FORCE_DETAIL);
				hack_flag_store_examine = FALSE;

			}
			else //�w��
			{
				price = calc_ability_card_price(o_ptr->pval);

				//�u���{��`�̃W�����}�v�̃J�[�h�ɂ�鉿�i������
				//p_ptr->magic_num2[10-19)��50�����l�Ƃ���1����邲�Ƃ�2%�㉺����
				if (p_ptr->magic_num2[10 + pos + ABLCARD_MAGICNUM_SHIFT])
				{
					price += price * (p_ptr->magic_num2[10 + pos + ABLCARD_MAGICNUM_SHIFT] - 50) * 2 / 100;
				}


				if (price > p_ptr->au)
				{
					msg_print("����������Ȃ��B");
					continue;
				}

				object_desc(o_name, o_ptr, 0);
				msg_format("%s���w������B", o_name);
				if (!get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) continue;

				inven_carry(o_ptr);
				msg_format("%s���w�������B", o_name);
				p_ptr->au -= price;
				building_prt_gold();

				//���v�x���z�ɉ��Z
				//if (total_pay_cardshop < 100000000) total_pay_cardshop += price;
				add_cardshop_profit(price);

				//�J�[�h�݌Ƀ��X�g�̊Y���ӏ����w���ς݂�0�ɂ���
				p_ptr->magic_num2[c - 'a' + ABLCARD_MAGICNUM_SHIFT] = 0;

				break;
			}

		}
	}

	clear_bldg(4, 18);



}

//v1.1.87
//�A�r���e�B�J�[�h�𔄋p����
void	sell_ability_card(void)
{
	int item;
	cptr q, s;
	object_type *o_ptr;
	int amt;
	int price = 0;




	q = "�u�A�r���e�B�J�[�h����������̂ˁH�v";
	s = "�A�r���e�B�J�[�h�������Ă��Ȃ��B";

	item_tester_tval = TV_ABILITY_CARD;
	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];

	if (o_ptr->number > 1)
	{
		amt = get_quantity("�����ł����H:", o_ptr->number);
		if (amt <= 0) return;
	}
	else amt = 1;

	//���i�v�Z�@�̔�����1/3
	price = (calc_ability_card_price(o_ptr->pval) + 2) / 3 * amt;


	msg_format("�u%s�v�̃J�[�h%d������%d�Ŕ�������Ă����炵���B", ability_card_list[o_ptr->pval].card_name, amt, price);

	if (!get_check_strict("����܂����H", CHECK_DEFAULT_Y))
	{
		return;
	}

	p_ptr->au += price;

	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);
	building_prt_gold();

}


/*:::�A�C�e��������*/
///mod160307 EXTRA���[�h�Ŏg����悤����������
void grassroots_barter()
{
	int cs, item_new;
	char c;
	bool flag_1=FALSE,flag_2 = FALSE, flag_3 = FALSE;//�܂��A�C�e����n���ĂȂ��t���O�A�܂����Ԃ������Ă��Ȃ��t���O�A�A�C�e���󂯎��J�n�t���O
	//���������@�����ɂ͗��Ȃ��͂�
	if(!p_ptr->grassroots && !EXTRA_MODE)
	{
		 msg_print("ERROR:grassroots��������barter()���Ă΂�Ă���");
		 return;
	}

	if(EXTRA_MODE)
	{
		if(ex_buildings_param[f_info[cave[py][px].feat].subtype]==255)
		{
			msg_print("���̍��d���l�b�g���[�N�̎ҒB�͂������Ȃ��B");
			return;
		}
		flag_1 = TRUE;
		flag_3 = TRUE;
	}
	else
	{
		if(p_ptr->grassroots == 1) flag_1 = TRUE;
		else if((s32b)(p_ptr->grassroots + 50000) > turn) flag_2 = TRUE; //50000:����
		else flag_3 = TRUE;
	}

	//�A�C�e�����n�����ɉו��������ς���������I��
	if(flag_3 && inventory[INVEN_PACK-1].k_idx)
	{
		if(p_ptr->pclass == CLASS_KAGEROU) msg_print("�킩�����P�u���́A�ו�����t�̂悤�ł����E�E�v");
		else msg_print("�e�T�u�M���ו�����t��B�v");
		return;
	}



	/*:::�܂��A�C�e����n���Ă��Ȃ��Ƃ�*/
	if(flag_1)
	{
		object_type forge;
		object_type *q_ptr = &forge;
		object_type *o_ptr;
		cptr q, s;
		char o_name[MAX_NLEN];
		int item, value;

		if(p_ptr->pclass == CLASS_KAGEROU)
		{
			q = "���������ɏo���܂����H";
			s = "�������������ł������Ȃ��̂����肢���܂��E�E";
		}
		else
		{
			q = "�ǂ̃A�C�e�����o���́H";
			s = "�M����ȕ������ĂȂ�����Ȃ��B";
		}
		item_tester_hook = object_is_not_worthless;//���i��1�ȏ�̃A�C�e���𔻒肷��hook�����

		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP)))
		{
			//flush();
			return;
		}
		if(!(wield_check(item,INVEN_PACK,0)))
		{
			//flush();
			return;
		}
		o_ptr = &inventory[item];

		if (object_is_cursed(o_ptr))//paranoia
		{
			if(p_ptr->pclass == CLASS_KAGEROU) msg_print("�킩�����P�u���́A�������Ă܂���E�E�v");
			else msg_print("�e�T�u������ƁB�������Ă��B�v");
			return;
		}

		object_copy(q_ptr, o_ptr);
		q_ptr->number = 1;
		q_ptr->inscription = 0;
		q_ptr->feeling = FEEL_NONE;
		object_desc(o_name, q_ptr, 0);

		msg_format("%s(%c)��I�������B", o_name, index_to_label(item));
		msg_print("");
		value = object_value_real(q_ptr);

		if(value < 100) 
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print("�킩�����P�u���܂�ǂ������͊��҂ł��Ȃ���������܂���ˁB�v");
			else msg_print("�e�T�u���̒��x�̂��̂��Ƃ��܂���҂ł��Ȃ����H�v");
		}
		else if(value < 1000) 
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print("�킩�����P�u����������ɏo���܂���H�v");
			else msg_print("�e�T�u����������ɏo���̂ˁH�v");
		}
		else if(value < 10000) 
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print("�킩�����P�u����͖ʔ����i�ł��ˁB�F���񒣂�؂肻���ł��B�v");
			else msg_print("�e�T�u���X�̕i�ˁB�F����؂��B�v");
		}
		else if(value < 100000) 
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print("�킩�����P�u�f���炵���i�ł��ˁB�����撣��܂��I�v");
			else msg_print("�e�T�u����͂��Ȃ�̈�i�ˁB�F����قǂ̕����o���邩����B�v");
		}
		else 
		{
			if(p_ptr->pclass == CLASS_KAGEROU)  msg_print("�킩�����P�u�F���񂱂�Ȃ������������Ă���Ȃ������E�E�v");
			else msg_print("�e�T�u�E�E�{���ɂ����́H�M���呹���邩������Ȃ����ǁB�v");
		}

		if (!get_check_strict("�o�i�����A�C�e���͖߂��Ă��܂���B��낵���ł����H", CHECK_OKAY_CANCEL))
		{
			return;
		}

		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);

		if(!EXTRA_MODE)
		{
			if(p_ptr->pclass == CLASS_KAGEROU) msg_print("�킩�����P�u�܂�������ɏW�܂�܂��傤�B�撣���Ă������T���ė��܂��I�v");
			else msg_print("�e�T�u���ꂶ��A�F�ɒm�点�Č�����邩�甼�����x�o������܂����ĂˁB�v");

		}
		msg_print(NULL);
		p_ptr->barter_value = value;
		if(item > INVEN_PACK) calc_android_exp();
		handle_stuff();

		//���݃^�[�����L�^
		//Ex���[�h�ł͑҂����Ԃ��Ȃ��l�b�g���[�N�������ł������ł���̂Ńt���O�ێ��̂��߂ɋL�^���Ȃ�
		if(!EXTRA_MODE) p_ptr->grassroots = turn;
	}

	/*:::�܂������o���ĂȂ��Ƃ�*/
	if(flag_2)
	{
		if(p_ptr->pclass == CLASS_KAGEROU) msg_print("�킩�����P�u���݂܂���B�܂��F���񂩂�̕i���������ĂȂ���ł��B�v");
		else msg_print("�e�T�u�܂��F����̕i�����o�����Ă��Ȃ��́B���������҂��ĂāB�v");
		return;
	}
	/*:::�A�C�e���𐶐����ꗗ�\�����Ăǂꂩ�𓾂�*/
	if(flag_3)
	{
		object_type barter_list[10];
		int i;
		char o_name[MAX_NLEN];

		for(i=0;i<10;i++)
		{
			int k_idx;
			int j;

			for(j=0;j<30;j++)
			{
				object_type forge;
				object_type *o_ptr = &forge;
				object_type *q_ptr = &barter_list[i];
				int o_value,q_value, p_value;
				int margin_o, margin_q;

				if(j==0) object_wipe(q_ptr);

				if(EXTRA_MODE)
					k_idx = get_obj_num(dun_level+randint1(20));
				else
					k_idx = get_obj_num(p_ptr->lev);

				//msg_format("%d]k_idx:%d",j,k_idx);
				if(!k_idx)
				{
					msg_print("ERROR:barter�A�C�e����������k_idx�������Ȓl");
					return;
				}
				object_prep(o_ptr,k_idx);
				if(EXTRA_MODE)
					apply_magic(o_ptr,dun_level+randint1(20),AM_NO_FIXED_ART);
				else
					apply_magic(o_ptr,p_ptr->lev * 3 / 2,AM_NO_FIXED_ART);

				if((o_ptr->tval == TV_BULLET || o_ptr->tval ==TV_ARROW || o_ptr->tval == TV_BOLT) && !o_ptr->name1) 
					o_ptr->number = (byte)damroll(6, 7);
				else if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE && !object_is_artifact(o_ptr))
				{
					o_ptr->number = 8 + (byte)damroll(2, 6);
				}

//object_desc(o_name, o_ptr, 0);

				o_value = object_value_real(o_ptr) * o_ptr->number;
				q_value = object_value_real(q_ptr) * q_ptr->number;
				/*:::��炵�זh�~��20%����*/
				//140812 20%��30%
				///mod141129 LUNATIC�ł͂����ƈ�������
				if(difficulty == DIFFICULTY_LUNATIC)
					p_value = p_ptr->barter_value * 5 / 10; 
				else if(EXTRA_MODE)
				{
					//�l�b�g���[�N�������ł͖��͂ŏo�����ς��悤�ɂ��Ă݂�
					if(!p_ptr->grassroots)
						p_value = p_ptr->barter_value * (adj_general[p_ptr->stat_ind[A_CHR]] + 40) / 100; 
					else
						p_value = p_ptr->barter_value * 8 / 10; 
				}
				else 
					p_value = p_ptr->barter_value * 7 / 10; 
				margin_o = (o_value > p_value) ? o_value - p_value : p_value - o_value;
				margin_q = (q_value > p_value) ? q_value - p_value : p_value - q_value;
//msg_format("new:%s (%d)",o_name,o_value);
//msg_format("value_list:%d mer:%d",q_value,margin_q);
//msg_format("value_new:%d mer:%d",o_value,margin_o);
				if(j==0 || margin_o < margin_q)
				{
					object_copy(q_ptr, o_ptr);
				}
			}
		}
		cs = 0;

		if(EXTRA_MODE && !p_ptr->grassroots)
			c_put_str(TERM_WHITE,"���̍��̗d����������̏o�����������悤���B�~�������̂�I�ڂ��B",4 , 6);
		else if(p_ptr->pclass == CLASS_KAGEROU) 
			c_put_str(TERM_WHITE,"�킩�����P�u����ł͌�����̊J�n�ł��I�܂��e�T���񂩂�I��ł��������ˁB�v",4 , 6);
		else 
			c_put_str(TERM_WHITE,"�e�T�u�F���҂����[�I�����M������I��ŁB�ǂꂪ�����H�v",4 , 6);
		while(1)
		{
			for(i=0;i<10;i++)
			{
				//int o_value = object_value_real(&barter_list[i]) * barter_list[i].number;
				int color = TERM_WHITE;
				object_type *o_ptr = &barter_list[i];
				if(cs==i) color = TERM_YELLOW;
				object_aware(o_ptr);
				object_known(o_ptr);
				object_desc(o_name, o_ptr, 0);
				//msg_format("%d:%s",o_value,o_name);
				c_put_str(color,format("%c) %s",'a'+i,o_name),6 + i, 10);
				
			}
			c = inkey();
			if (c == '2' && cs < 9) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == ' ' || c == '\r' || c == '\n') c=cs+'a';

			if(c >= 'a' && c <= 'j')
			{
				object_desc(o_name, &barter_list[c-'a'], 0);
				msg_format("%s���󂯎��B",o_name);
				if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
			}
		}
		clear_bldg(4,18);
		item_new = inven_carry(&barter_list[c-'a']);
		autopick_alter_item(item_new, FALSE);
		handle_stuff();
		if(EXTRA_MODE)
		{
			msg_format("������I�������B�d�������͉ו����܂Ƃ߂Ď��̃t���A�ւƌ��������B");
			//�����g�p�ς݃t���O
			ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;
			return;
		}
		else
			msg_format("�A�C�e���������I�������B�F�͌��������A�C�e���������ċA���Ă������B");

		/*:::grassroots���f�t�H��1�֖߂��B0���ƃl�b�g���[�N�������B*/
		p_ptr->grassroots = 1;

	}
	return;
	
}









/*:::�J�[�h������*/
void grassroots_trading_cards(void)
{
	int i;
	int ref_pval=0,ref_num,ref_cost,ref_totalcost; //�v���C���[���n�����J�[�h�̏��
	char c;
	int cs;
	cptr q, s;
	cptr msg_select; //�J�[�h��I�ԂƂ��̃��b�Z�[�W
	cptr msg_reject; //�񎦂��ꂽ�J�[�h�ɍ����J�[�h��������Ȃ������Ƃ��̃��b�Z�[�W
	cptr msg_finish; //�����I�������b�Z�[�W
	char o_name[MAX_NLEN];
	int item;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int mult; //�J�[�h�̉��l�I�莞�̕␳
	int trade_chance, trade_num; //�I�肳�������J�[�h��␔,���ۂɑI�肳�ꂽ�J�[�h��
	object_type barter_list[10];
	int base_rarity; //�J�[�h�I��Ń��A�J�[�h�̏o�₷��

	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];

//���s�ς݃t���O�����Ă�ꍇ�͏I��
	if(ex_buildings_param[ex_bldg_num] == 255)
	{
		msg_print("����͏I�������B");
		return;
	}

	if(inventory[INVEN_PACK-1].k_idx)
	{
		msg_print("���͉ו�����t���B");
		return;
	}
	
	//���̍��l�b�g���[�N�ł̌����̏�����
	if(ex_bldg_idx == BLDG_EX_GRASSROOTS)
	{
		if(p_ptr->grassroots)
		{
			mult = 120;
			base_rarity = 80;
			trade_chance = 6 + randint1(4);
		}
		else
		{
			mult = 80 + adj_general[p_ptr->stat_ind[A_CHR]]/2;
			base_rarity = 20 + adj_general[p_ptr->stat_ind[A_CHR]];
			trade_chance = 3 + randint1(5);
		}
		if(p_ptr->pclass == CLASS_KAGEROU)
		{
			q = "�u�ǂ�ȃJ�[�h���o���Ă�����ł����H�v";
			s = "�u����A�J�[�h�������Ă��܂���ˁB�v";
		}
		else
		{
			q = "�u�����A�M�����o���J�[�h�͉�������H�v";
			s = "�u������ƁA�M���J�[�h�����ĂȂ�����Ȃ��B�v";
		}
		msg_select = "���̍��d���l�b�g���[�N�̗d�������̓J�[�h��񎦂����B";
		msg_reject = "�N�����Ȃ��̏o�����J�[�h�Ɍ������J�[�h�������Ă��Ȃ������E�E";
		msg_finish = "�g���[�h�͏I�������B�d�������͉ו����܂Ƃ߂Ď��̃t���A�֌��������B";
	}
	else if(ex_bldg_idx == BLDG_EX_REIMU)
	{
		//�p�����[�^(0-100)
		int rank = ex_buildings_param[ex_bldg_num];
		//msg_format("rank:%d",rank);
		if(!rank)
		{
			msg_print("�u�g���[�h�H�������m��Ȃ�����Ǒf�G�Ȃ��ΑK���͂�����B�v");
			return;
		}

		mult = 70 + rank/2;
		base_rarity = 50 + rank;
		trade_chance = 3 + rank / 15;
		q = "�ǂ̃J�[�h���o���܂����H";
		s = "�J�[�h�������Ă��Ȃ��B";
		msg_select = "�u�J�[�h���Ă���̂��ƁH���������炵�߂��d���������Ă����񂾂��ǁv";
		msg_reject = "�u���[��A����Ȑ������Ȃ͎̂����ĂȂ���B�v";
		msg_finish = "�u����Ŏ���I���ˁB�Ƃ���őf�G�Ȃ��ΑK���͂�����B�v";

	}
	else if(ex_bldg_idx == BLDG_EX_MARISA)
	{

		mult = 100;
		base_rarity = 70;
		trade_chance = 5 ;
		q = "�u���̃J�[�h���o���񂾁H��΂▂�@���ł��������B�v";
		s = "�����ł�����̂������Ă��Ȃ��B";
		msg_select = "�������̓J�[�h��񎦂����B";
		msg_reject = "�u�����ȁB����Ɍ������J�[�h�������ĂȂ����B�v";
		msg_finish = "�u���āA�܂��ǂ����ŏ��i���d����Ă��Ȃ���ȁB�v";

	}
	else if (ex_bldg_idx == BLDG_EX_YUMA)
	{

		mult = 80;
		base_rarity = 60;
		trade_chance = 4;
		q = "�u�����H�������悱���B�v";
		s = "�����ł�����̂������Ă��Ȃ��B";
		//msg_select = ""; ���ƂŐݒ肷��
		msg_reject = "�u�����ȁB����Ɍ������J�[�h�������ĂȂ��B�v";
		msg_finish = "�u���͂����Ǝ|�����̂������Ă����B�v";

	}


	else
	{
		msg_print("ERROR:���̌����ŃJ�[�h��������Ƃ��̊�{�p�����[�^���ݒ肳��Ă��Ȃ�");
		return;
	}

	//�J�[�h�I��
	if(ex_bldg_idx == BLDG_EX_MARISA)
		item_tester_hook = marisa_will_trade;
	else if(ex_bldg_idx == BLDG_EX_YUMA)
		item_tester_hook = yuma_will_trade;
	else
		item_tester_tval = TV_ITEMCARD;

	if (!get_item(&item, q, s, (USE_INVEN))) return;

	o_ptr = &inventory[item];
	if (o_ptr->number > 1)
	{
		if(o_ptr->tval == TV_ITEMCARD) ref_num = get_quantity("�����ł����H:", o_ptr->number);
		else ref_num = get_quantity(NULL, o_ptr->number);
		if (ref_num <= 0) return;
	}
	else ref_num = 1;

	//�J�[�h�I��O�̏������Ȃ�
	if(o_ptr->tval == TV_ITEMCARD)
	{
		ref_pval = o_ptr->pval;
		ref_cost = support_item_list[ref_pval].cost; 

		ref_totalcost = ref_cost * ref_num * mult / 100;

	}
	else if (ex_bldg_idx == BLDG_EX_MARISA && o_ptr->name1 == ART_IBUKI) //v2.0.13 �ړ����@�X�ňɐ��Z��n�����Ƃ�
	{
		ref_cost = 300;
		ref_totalcost = 300;

	}
	else if (ex_bldg_idx == BLDG_EX_MARISA) //�ړ����@�X�ł���ȊO�̃A�C�e����n�����Ƃ�
	{
		ref_pval = -1;

		for(i=0;marisa_wants_table[i].tval;i++) 
		{
			if(o_ptr->tval == marisa_wants_table[i].tval && o_ptr->sval == marisa_wants_table[i].sval)
			{
				if(o_ptr->tval < TV_BOOK_END && o_ptr->sval == 2)
					ref_cost = 20;
				else if(o_ptr->tval < TV_BOOK_END && o_ptr->sval == 3)
				{
					ref_cost = 50;
					base_rarity += 20;
				}
				else
				{
					ref_cost = marisa_wants_table[i].maripo / 10;
					if(ref_cost > 50) base_rarity += 20;
				}

				if(ref_cost > 30)
				{

					//ref_cost = 30 + (ref_cost - 30) / 2;
					trade_chance += ref_cost / 30;
					if(trade_chance > 10) trade_chance = 10;
				}

				ref_totalcost = ref_cost * ref_num * mult / 100;

				break;
			}
		}


	}
	else if (ex_bldg_idx == BLDG_EX_YUMA)
	{

		//����ȗ�
		if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_BIG_EGG)
		{
			msg_select = "�N�b�N�b�N...�����͒��������̂������Ă����ȁB";
			ref_cost = 300;
		}
		//�ɐ��Z
		else if (o_ptr->name1 == ART_IBUKI)
		{
			msg_select = "�ق��A������ł������o��̂��H���̎��ɑł��ĕt������Ȃ����B";
			ref_cost = 250;
		}
		//�����̚�
		else if (o_ptr->tval == TV_SOUVENIR && o_ptr->sval == SV_SOUVENIR_DOUJU)
		{
			msg_select = "�ق��A������ł������o��̂��H���̎��ɑł��ĕt������Ȃ����B";
			ref_cost = 200;
		}
		else if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI)
		{
			msg_select = "����͗��̍ޗ����B�����Ȃ��B";
			ref_cost = 50;
		}
		else if (o_ptr->tval == TV_FOOD)
		{

			switch (o_ptr->sval)
			{
			case SV_FOOD_SLIMEMOLD:
			case SV_FOOD_SENTAN:
			case SV_FOOD_MAGIC_WATERMELON:
				msg_select = "�܂����̑����ɂ͂Ȃ邩�B";
				ref_cost = 5;
				break;

			case SV_FOOD_EEL:
			case SV_FOOD_TENTAN:
			case SV_FOOD_STRANGE_BEAN:
				msg_select = "���X���{�����肻������Ȃ����B";
				ref_cost = 20;
				break;

			default:
				msg_select = "�܂��ȁB���ɉ��������ĂȂ��̂��H";
				ref_cost = 1;
				break;
			}
		}
		else if (o_ptr->tval == TV_MUSHROOM)
		{

			switch (o_ptr->sval)
			{
			case SV_MUSHROOM_MANA:
			case SV_MUSHROOM_PUFFBALL:
			case SV_MUSHROOM_MON_L_BLUE:
				msg_select = "�܂����̑����ɂ͂Ȃ邩�B";
				ref_cost = 5;
				break;

				//����EXTRA�ł͎�ɓ���Ȃ������̂��������Ŏ�ɓ���悤�ɂȂ邩�������̂Őݒ肵�Ă���
			case SV_MUSHROOM_MON_SUPER:
				msg_select = "�����͖��@�̐X�̎傩�H�Ȃ�����ȂƂ���ɁH";
				ref_cost = 300;
				break;

			default:
				msg_select = "���̑����ɂ��Ȃ��ȁB";
				ref_cost = 1;
				break;
			}
		}
		else if (o_ptr->tval == TV_SWEETS)
		{
			switch (o_ptr->sval)
			{
			case SV_SWEET_POTATO:
				msg_select = "���������ȏĂ������ȁB�����悱���B";
				ref_cost = 10;
				break;

			case SV_SWEETS_MIRACLE_FRUIT:
				msg_select = "���X���{�����肻������Ȃ����B";
				ref_cost = 20;
				break;

			case SV_SWEETS_PEACH:
				msg_select = "����͐响����Ȃ����B���������ȁB";
				ref_cost = 50;
				break;

			default:
				msg_select = "�܂��ȁB���ɉ��������ĂȂ��̂��H";
				ref_cost = 2;
				break;
			}
		}
		else if (o_ptr->tval == TV_FLASK)
		{
			switch (o_ptr->sval)
			{
				//���P�b�g�R������ԃR�X�p���������g�����Əd�ʂ��l����Α�ʂɔ����Ď��������̂͌����I�ł͂Ȃ����낤
			case SV_FLASK_ROCKET:
			case SV_FLASK_ENERGY_RED:
			case SV_FLASK_ENERGY_BLUE:
			case SV_FLASK_ENERGY_WHITE:
			case SV_FLASK_ENERGY_BLACK:
				msg_select = "�܂����̑����ɂ͂Ȃ邩�B";
				ref_cost = 5;
				break;
			case SV_FLASK_ENERGY_PARTICLE:
				msg_select = "��������́H�������Ƃ��Ȃ��͂������邼�B";
				ref_cost = 50;
				break;
			case SV_FLASK_ENERGY_WAVEMOTION:
				msg_select = "��������́H�������Ƃ��Ȃ��͂������邼�B";
				ref_cost = 100;
				break;
			case SV_FLASK_ENERGY_HYPERSPACE:
				msg_select = "��������́H�������Ƃ��Ȃ��͂������邼�B";
				ref_cost = 200;
				break;

			default:
				msg_select = "�܂��ȁB���ɉ��������Ă��Ȃ��̂��H";
				ref_cost = 1;
				break;
			}

		}
		else if (o_ptr->tval == TV_ALCOHOL)
		{
			switch (o_ptr->sval)
			{



			case SV_ALCOHOL_ORC:
			case SV_ALCOHOL_REISYU:
				msg_select = "�Ȃ񂩖�L�������ȁB";
				ref_cost = 10;
				break;

			case SV_ALCOHOL_STRONGWINE:
			case SV_ALCOHOL_MAGGOT_BEER:
			case SV_ALCOHOL_1420:
			case SV_ALCOHOL_SCARLET:
			case SV_ALCOHOL_MAMUSHI:
				msg_select = "�Ȃ��Ȃ������Ȃ������B�����Ǝ����Ă����B";
				ref_cost = 30;
				break;

			case SV_ALCOHOL_MARISA:
				msg_select = "�����H����������炽���̈���������Ȏ��ꂽ�㕨�ɂȂ�񂾁H";
				ref_cost = 50;
				break;

			case SV_ALCOHOL_90:
			case SV_ALCOHOL_SYUTYUU:
			case SV_ALCOHOL_TRUE_ONIKOROSHI:
				msg_select = "�ق��A�����͔����������B";
				ref_cost = 50;
				break;

			case SV_ALCOHOL_KOKO:
			case SV_ALCOHOL_EIRIN:
				msg_select = "�����Â����������ɖ��C�Ȃ������ȁB";
				ref_cost = 50;
				break;

			case SV_ALCOHOL_KUSHINADA:
				msg_select = "�N�b�N�b�N...���̒��x�̓Ŏ��ł��̎����ׂ��Ǝv�������H";
				ref_cost = 100;
				break;

			case SV_ALCOHOL_GOLDEN_MEAD:
				msg_select = "���̒m��Ȃ��Â������͂�������ȁB�Ȃ��Ȃ������[�����B";
				ref_cost = 100;
				break;

			case SV_ALCOHOL_NECTAR:
				msg_select = "�ق��A�ٍ��̐_�����I�����͉h�{���_���I";
				ref_cost = 200;
				break;

			default:
				msg_select = "�����Ƌ������͂Ȃ��̂��H";
				ref_cost = 5;
				break;
			}
		}
		else
		{
			msg_print("ERROR:���̃A�C�e����ޖ��ɓn�����Ƃ��̕]���l���ݒ肳��Ă��Ȃ�");
			return;
		}

		if (ref_cost > 30)
		{
			//ref_cost = 30 + (ref_cost - 30) / 2;
			trade_chance += ref_cost / 30;
			if (trade_chance > 10) trade_chance = 10;
			base_rarity += ref_cost / 5;
		}

		//�H������99�Ƃ��ō����ȃJ�[�h����肷�邱�Ƃ�h�����߂ɃA�C�e���̐������������totalcost�����炵�Ă���

		if (ref_num == 1)
			ref_totalcost = ref_cost * mult / 100;
		else if (ref_num <= 10)
			ref_totalcost = ref_cost * (1 + (ref_num - 1) * 2 / 3) * mult / 100;
		else if (ref_num <= 30)
			ref_totalcost = ref_cost * (7 + (ref_num - 10) / 2) * mult / 100;
		else if (ref_num <= 70)
			ref_totalcost = ref_cost * (17 + (ref_num - 30) / 4) * mult / 100;
		else
			ref_totalcost = ref_cost * (27 + (ref_num - 70) / 8) * mult / 100;



	}
	else
	{
		msg_print("ERROR:���̌����ŃJ�[�h��������Ƃ��̃R�X�g�v�Z���ݒ肳��Ă��Ȃ�");
		return;
	}


	for(i=0;i<10;i++) object_wipe(&barter_list[i]);	
	trade_num = 0;

	//�J�[�h�������J��Ԃ�
	for(i=0;i<trade_chance && i<10;i++)
	{
		int j;
		int cnt = 0;
		int tmp_pv;

		//�J�[�h���X�g�S���`�F�b�N
		for(j=0;support_item_list[j].rarity;j++)
		{
			int k;
			bool flag_ok = TRUE;
			//�����񎦂����J�[�h�Ɠ����̂͒e��
			if(j == ref_pval) continue;
			//���łɑI�肳�ꂽ�J�[�h�Ɠ����̂͒e�� 
			for(k=0;k<trade_num;k++) if(j == barter_list[k].pval) flag_ok = FALSE;
			if(!flag_ok) continue;
			//�ꖇ�̃R�X�g�����̒񎦂����J�[�h�S���̃R�X�g��荂���̂͒e��(�R�X�g1�̃J�[�h�͒e���Ȃ�)
			if(support_item_list[j].cost > 1 && support_item_list[j].cost > ref_totalcost) continue;

			//50���o���Ă����̒񎦂����J�[�h�S���̃R�X�g�ɑ���Ȃ��̂͒e��
			if( support_item_list[j].cost * 50 < ref_totalcost) continue;

			//�Œᐶ���K��������K���[���J�[�h�͍��m���Œe��
			if(dun_level < support_item_list[j].min_lev && randint1(support_item_list[j].min_lev - dun_level) < 5 && one_in_(2)) continue;
			//�ō������K��������K���󂢃J�[�h�͒��m���Œe��
			if(dun_level > support_item_list[j].max_lev && randint1(dun_level - support_item_list[j].max_lev) < 10 ) continue;
			//�������J�[�h�͊m���Œe��
			if(randint1(support_item_list[j].rarity) > base_rarity) continue;

			//�����܂Œʂ����J�[�h���烉���_���ɑI��
			cnt++;
			if(!one_in_(cnt)) continue;
			tmp_pv = j;
		}
		//�I�����ꂽ�J�[�h������ꍇ���������߂�barter_list�ɓo�^
		if(cnt)
		{
			int tmp_num;
			object_type *tmp_o_ptr = &barter_list[trade_num];
			object_prep(tmp_o_ptr,lookup_kind(TV_ITEMCARD,SV_ITEMCARD));
			tmp_o_ptr->pval = tmp_pv;

			//��{�����@������Ɨ����K�p
			tmp_num = (ref_totalcost*2/3 + randint1(ref_totalcost*2/3) + 1) / support_item_list[tmp_pv].cost;
			//�ʏ�1�������o�Ȃ��J�[�h�͐��������ɂ���
			if(support_item_list[tmp_pv].prod_num == 1)
			{ 
				if(tmp_num>10) tmp_num -= (tmp_num-9) / 2;
				tmp_num /= 2;
			}
			//���܂��R�o��J�[�h�͌��炷
			if(tmp_num > 40) tmp_num = 40 + randint1(tmp_num-40);
			if(tmp_num > 30) tmp_num = 30 + randint1(tmp_num-30);
			if(tmp_num > 20) tmp_num = 20 + randint1(tmp_num-20);
			if(tmp_num > 10) tmp_num = 10 + randint1(tmp_num-10);

			if(tmp_num > 99) tmp_num = 99;
			else if(tmp_num < 1) tmp_num = 1;
			tmp_o_ptr->number = tmp_num;

			trade_num++;
		}
	}

	//�ЂƂ��I�肳��Ȃ������ꍇ�I��
	if(!trade_num)
	{
		c_put_str(TERM_WHITE,msg_reject,4 , 6);
		inkey();
		return;
	}

	//�J�[�h��\�����I��
	cs = 0;
	while(1)
	{
		c_put_str(TERM_WHITE,msg_select,4 , 6);
		c_put_str(TERM_WHITE,"(�g���[�h����߂�FESC)",5 , 6);
		for(i=0;i<trade_num;i++)
		{
			int color = TERM_WHITE;
			object_type *tmp_o_ptr = &barter_list[i];
			if(cs==i) color = TERM_YELLOW;
			object_aware(tmp_o_ptr);
			object_known(tmp_o_ptr);
			object_desc(o_name, tmp_o_ptr, 0);
			//msg_format("%d:%s",o_value,o_name);
			c_put_str(color,format("%c) %s",'a'+i,o_name),6 + i, 10);
		}
		c = inkey();
		if(c == ESCAPE)
		{
			if (get_check_strict("���̃t���A�ł͂����g���[�h�ł��܂���B��낵���ł����H", CHECK_OKAY_CANCEL)) break;
		}
		else if (c == '2' && cs < (trade_num-1)) cs++;
		else if (c == '8' && cs > 0) cs--;
		else if (c == ' ' || c == '\r' || c == '\n') c=cs+'a';
		else if(c >= 'a' && c <= 'a'+trade_num-1)
		{
			object_desc(o_name, &barter_list[c-'a'], 0);
			msg_format("%s���󂯎��B",o_name);
			if (!get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) continue;

			//�I���������������
			inven_item_increase(item, -ref_num);
			inven_item_describe(item);
			inven_item_optimize(item);
			inven_carry(&barter_list[c-'a']);
			msg_format("%s���󂯎�����B",o_name);

			break;
		}
	}

	clear_bldg(4,18);
	c_put_str(TERM_WHITE,msg_finish,4 , 6);
	inkey();
	clear_bldg(4,18);

	//���s�ς݃t���O���Ă�
	ex_buildings_param[f_info[cave[py][px].feat].subtype] = 255;

}




/*:::��ʃG�S�t���p�f�ރe�[�u��*/
const struct guild_arts_type archer_guild_table[] =
{
	{TV_MUSHROOM,SV_MUSHROOM_MON_RED,EGO_ARROW_FIRE,5,"�Ή��̖�"},
	{TV_MATERIAL,SV_MATERIAL_RUBY,EGO_ARROW_FIRE,99,"�Ή��̖�"},
	{TV_MATERIAL,SV_MATERIAL_GARNET ,EGO_ARROW_FIRE,30,"�Ή��̖�"},
	{TV_MATERIAL,SV_MATERIAL_HEMATITE,EGO_ARROW_FIRE,20,"�Ή��̖�"},

	{TV_MUSHROOM,SV_MUSHROOM_MON_BLUE,EGO_ARROW_ELEC,5,"�d���̖�"},
	{TV_MATERIAL,SV_MATERIAL_SAPPHIRE,EGO_ARROW_ELEC,99,"�d���̖�"},
	{TV_MATERIAL,SV_MATERIAL_AMETHYST ,EGO_ARROW_ELEC,30,"�d���̖�"},
	{TV_MATERIAL,SV_MATERIAL_LAPISLAZULI,EGO_ARROW_ELEC,20,"�d���̖�"},

	{TV_MUSHROOM,SV_MUSHROOM_MON_WHITE, EGO_ARROW_COLD,5,"��C�̖�"},
	{TV_MATERIAL,SV_MATERIAL_EMERALD , EGO_ARROW_COLD,99,"��C�̖�"},
	{TV_MATERIAL, SV_MATERIAL_OPAL , EGO_ARROW_COLD,30,"��C�̖�"},
	{TV_MATERIAL,SV_MATERIAL_AQUAMARINE, EGO_ARROW_COLD,20,"��C�̖�"},
	{TV_MATERIAL,SV_MATERIAL_ICESCALE, EGO_ARROW_COLD,50,"��C�̖�"},

	{TV_MUSHROOM,SV_MUSHROOM_MON_GREEN, EGO_ARROW_POIS,5,"�ł̖�"},
	{TV_MATERIAL,SV_MATERIAL_GELSEMIUM , EGO_ARROW_POIS,99,"�ł̖�"},
	{TV_MATERIAL,SV_MATERIAL_ARSENIC, EGO_ARROW_POIS,30,"�ł̖�"},
	{ TV_MATERIAL,SV_MATERIAL_MERCURY, EGO_ARROW_POIS,50,"�ł̖�" },


	{TV_SOUVENIR,SV_SOUVENIR_PHOENIX_FEATHER  , EGO_ARROW_HOLY,99,"���Ȃ��"},
	{TV_MATERIAL, SV_MATERIAL_DIAMOND, EGO_ARROW_HOLY,50,"���Ȃ��"},
	{TV_MATERIAL,SV_MATERIAL_MITHRIL , EGO_ARROW_HOLY,5,"���Ȃ��"},
	{TV_MATERIAL, SV_MATERIAL_HOPE_FRAGMENT, EGO_ARROW_HOLY,50,"���Ȃ��"},
	{TV_MATERIAL, SV_MATERIAL_BROKEN_NEEDLE, EGO_ARROW_HOLY,10,"���Ȃ��"},

	{TV_MATERIAL,SV_MATERIAL_TOPAZ , EGO_ARROW_HUNTER,30,"��l�̖�"},
	{TV_MATERIAL,SV_MATERIAL_PERIDOT , EGO_ARROW_HUNTER,30,"��l�̖�"},
	{TV_MATERIAL,SV_MATERIAL_MOONSTONE , EGO_ARROW_HUNTER,30,"��l�̖�"},
	{TV_MATERIAL,SV_MATERIAL_TAKAKUSAGORI , EGO_ARROW_HUNTER,50,"��l�̖�"},

	{TV_MATERIAL,SV_MATERIAL_HIKARIGOKE , EGO_ARROW_DRAGONSLAY,5,"���E���̖�"},
	{TV_MATERIAL,SV_MATERIAL_DRAGONSCALE , EGO_ARROW_DRAGONSLAY,20,"���E���̖�"},
	{ TV_MATERIAL,SV_MATERIAL_RYUUZYU , EGO_ARROW_DRAGONSLAY,30,"���E���̖�" },
	{TV_MATERIAL,SV_MATERIAL_DRAGONNAIL , EGO_ARROW_DRAGONSLAY,50,"���E���̖�"},

	//v1.1.56 �ǉ�
	{ TV_MUSHROOM,SV_MUSHROOM_MON_BLACK, EGO_ARROW_ACID,5,"���I�̖�" },
	{ TV_MATERIAL,SV_MATERIAL_NIGHTMARE_FRAGMENT , EGO_ARROW_ACID,50,"���I�̖�" },
	{ TV_MATERIAL,SV_MATERIAL_ISHIZAKURA, EGO_ARROW_ACID,30,"���I�̖�" },
	
	{ TV_MATERIAL, SV_MATERIAL_IZANAGIOBJECT, EGO_ARROW_SLAY_DEITY,50,"�V�T���̖�" },

	{ TV_MATERIAL, SV_MATERIAL_MYSTERIUM, EGO_ARROW_AMANOZYAKU,99,"�V�׋S�̖�" },


	{0,0,0,0,"�I�[�p�_�~�["}

};

/*:::��ʋ����p�f�ރe�[�u��*/
const struct guild_arts_type archer_guild_table2[] =
{
	{TV_MATERIAL,SV_MATERIAL_SCRAP_IRON ,3,99," "},
	{TV_MATERIAL,SV_MATERIAL_MAGNETITE,7,99," "},
	{TV_MATERIAL,SV_MATERIAL_ADAMANTITE,10,99," "},
	{TV_MATERIAL, SV_MATERIAL_METEORICIRON ,15,99," "},
	{TV_MATERIAL,SV_MATERIAL_HIHIIROKANE,25,99," "},
	{0,0,0,0,"�I�[�p�_�~�["}
};



/*:::�A�[�`���[�M���h�ɂĖ�ʋ����̑f�ރA�C�e���𔻒�*/
static bool archer_guild_hook2(object_type *o_ptr)
{
	int i;
	for(i=0;archer_guild_table2[i].tval;i++)
	{
		if(o_ptr->tval == archer_guild_table2[i].tval && o_ptr->sval == archer_guild_table2[i].sval) return TRUE;
	}
	return FALSE;
}

/*:::�A�[�`���[�M���h�ɂăG�S�t���̑f�ރA�C�e���𔻒�*/
static bool archer_guild_hook(object_type *o_ptr)
{
	int i;
	for(i=0;archer_guild_table[i].tval;i++)
	{
		if(o_ptr->tval == archer_guild_table[i].tval && o_ptr->sval == archer_guild_table[i].sval) return TRUE;
	}
	return FALSE;
}



/*:::�f�ނ��g���Ė����������*/
static void archer_guild_arts2(void)
{
	int i, item, item_m;
	int thtd=0;
	object_type *o_ptr;
	object_type *om_ptr;
	cptr        q, s;
	char o_name[MAX_NLEN];

	if(p_ptr->pclass != CLASS_ARCHER)
	{
		msg_print("���Ȃ��͍H�[�ɓ���Ă��炦�Ȃ������B");
		return;
	}

	item_tester_hook = item_tester_hook_ammo;
	q = "�ǂ̖���������܂����H";
	s = "���Ȃ��͋����ł����������Ă��Ȃ��B";
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	item_tester_hook = archer_guild_hook2;
	q = "�ǂ̑f�ނ��g���ċ������܂����H";
	s = "���������Ȃ��͋����Ɏg���f�ނ������Ă��Ȃ������B";
	if (!get_item(&item_m, q, s, (USE_INVEN))) return;
	om_ptr = &inventory[item_m];

	for(i=0;archer_guild_table2[i].tval;i++) 
		if(archer_guild_table2[i].tval == om_ptr->tval && archer_guild_table2[i].sval == om_ptr->sval)
		{ thtd=archer_guild_table2[i].type; break;}

	if(!thtd) {msg_format("ERROR:archer_guild_arts2()�őf�ނ��K�؂ɓo�^����Ă��Ȃ��H"); return;}

	object_desc(o_name, o_ptr, 0);

	if(o_ptr->to_d >= thtd && o_ptr->to_h >= thtd)
	{
		msg_format("���̑f�ނł�%s�������ł��Ȃ��B",o_name);
		return;
	}
		
	msg_format("%s��+%d�܂ŋ����ł���B",o_name,thtd);
	
	if(!get_check("�������܂����H")) return;

	if(o_ptr->to_d < thtd) o_ptr->to_d = thtd;
	if(o_ptr->to_h < thtd) o_ptr->to_h = thtd;
	msg_format("���������������B");

	if (object_is_cursed(o_ptr))
	{
		msg_format("��̎􂢂��������B");
		o_ptr->curse_flags = 0L;
	}

	inven_item_increase(item_m, -1);
	inven_item_describe(item_m);
	inven_item_optimize(item_m);

	p_ptr->window |= (PW_INVEN);
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	return;
}


static bool item_tester_hook_ammo_make_ego(object_type *o_ptr)
{
	if(o_ptr->tval == TV_ARROW || o_ptr->tval == TV_BOLT)
	{
		if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr)) return (TRUE);
	}

	return (FALSE);
}



/*:::�f�ނ��g���Ė�ɑ����t�^����*/
static void archer_guild_arts(void)
{
	int i, item, item_m,slot_new;
	object_type *o_ptr;
	object_type *om_ptr;
	object_type forge;
	object_type *new_o_ptr = &forge;

	cptr        q, s;
	char o_name[MAX_NLEN];
	int num;

	if(p_ptr->pclass != CLASS_ARCHER)
	{
		msg_print("���Ȃ��͍H�[�ɓ���Ă��炦�Ȃ������B");
		return;
	}

	item_tester_hook = item_tester_hook_ammo_make_ego;
	q = "�ǂ̖�ɑ����t�^���܂����H";
	s = "���Ȃ��͑����t�^�ł����������Ă��Ȃ��B";
	if (!get_item(&item, q, s, (USE_INVEN))) return;
	o_ptr = &inventory[item];

	item_tester_hook = archer_guild_hook;
	q = "�ǂ̑f�ނ��g���ċ������܂����H";
	s = "���������Ȃ��͋����Ɏg���f�ނ������Ă��Ȃ������B";
	if (!get_item(&item_m, q, s, (USE_INVEN))) return;
	om_ptr = &inventory[item_m];

	for(i=0;archer_guild_table[i].tval;i++)
	{ 	
		if(archer_guild_table[i].tval == om_ptr->tval && archer_guild_table[i].sval == om_ptr->sval) break;
	}
	if(!archer_guild_table[i].tval) {msg_format("ERROR:archer_guild_arts()�őf�ނ��K�؂ɓo�^����Ă��Ȃ��H"); return;}


	object_desc(o_name, o_ptr, 0);

	num = archer_guild_table[i].num;

	if(inventory[INVEN_PACK-1].k_idx && om_ptr->number != 1 && o_ptr->number > num)
	{
		msg_print("�����t�^����������Ă�]�T���Ȃ��悤���B");
		return;
	}


	if(num < o_ptr->number)
		msg_format("%s�̂���%d�{��%s�ɂ��邱�Ƃ��ł���B",o_name,num, archer_guild_table[i].name);
	else
		msg_format("%s��%s�ɂ��邱�Ƃ��ł���B",o_name, archer_guild_table[i].name);
	
	if(!get_check("�����t�^���܂����H")) return;

	object_prep(new_o_ptr,o_ptr->k_idx);

	new_o_ptr->number = MIN(num,o_ptr->number);
	new_o_ptr->to_d = (o_ptr->to_d<0)?0:o_ptr->to_d;
	new_o_ptr->to_h = (o_ptr->to_h<0)?0:o_ptr->to_h;
	new_o_ptr->name2 = archer_guild_table[i].type;
	object_aware(new_o_ptr);
	object_known(new_o_ptr);
	
	if (object_is_cursed(new_o_ptr))
	{
		msg_format("��̎􂢂��������B");
		new_o_ptr->curse_flags = 0L;
	}

	msg_format("�����t�^�����������B");

	/*:::�ŏ��̃A�C�e�����������ŃC���x���g��������Ď��̃A�C�e�����������Ώۂ��ς�鋰�ꂪ����̂ŋ���̍�ŕ���B�����Ƃ܂Ƃ��ȕ��@�����肻���Ȃ��̂����v�����Ȃ������B*/
	if(item_m > item)
	{
		inven_item_increase(item_m, -1);
		inven_item_describe(item_m);
		inven_item_optimize(item_m);
		inven_item_increase(item, -num);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		inven_item_increase(item, -num);
		inven_item_describe(item);
		inven_item_optimize(item);
		inven_item_increase(item_m, -1);
		inven_item_describe(item_m);
		inven_item_optimize(item_m);
	}
	slot_new = inven_carry(new_o_ptr);
	autopick_alter_item(slot_new, FALSE);	

	p_ptr->window |= (PW_INVEN);
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	return;
}






/*:::�A�[�`���[�M���h�̐����𕷂�*/
static void archer_guild_guide(void)
{
	if(p_ptr->pclass != CLASS_ARCHER)
	{
		msg_print("���Ȃ��͎ˎ�ł͂Ȃ��̂ł����łł��邱�Ƃ͂Ȃ��悤���B");
		return;
	}
	display_rumor_new(20);
}

/*:::�O���l��p�@����_�Ђ���O�E�֋A�҂���*/
static bool escape_from_gensoukyou(void)
{
	int i;
	if(p_ptr->pclass != CLASS_OUTSIDER)
	{
		msg_print("���Ȃ��͌��z���o�g���B");
		return FALSE;
	}

	if( !get_check_strict("�O�E�֋A��܂���? ", CHECK_NO_HISTORY)) return FALSE;
	if (!p_ptr->noscore)
	{
	prt("�m�F�̂��� '@' �������ĉ������B", 0, 0);

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return FALSE;

		if(p_ptr->total_winner) finish_the_game = TRUE;
	}

	/* Initialize "last message" buffer */
	if (p_ptr->last_message) string_free(p_ptr->last_message);
	p_ptr->last_message = NULL;

	/*:::last_words:�⌾�I�v�V����*/
	if (last_words)
	{
		char buf[1024] = "";
		cptr tmp_msg;

		if(p_ptr->total_winner) 
			tmp_msg = "*����*���b�Z�[�W: ";
		else 
			tmp_msg = "�����ꌾ: ";

		do
		{
			while (!get_string(tmp_msg, buf, sizeof buf)) ;
		}
		while (!get_check_strict("��낵���ł����H", CHECK_NO_HISTORY));

		if (buf[0])
		{
			p_ptr->last_message = string_make(buf);
			msg_print(p_ptr->last_message);
		}
	}
	p_ptr->playing = FALSE;
	p_ptr->is_dead = TRUE;
	p_ptr->leaving = TRUE;

	if(p_ptr->total_winner) strcpy(p_ptr->died_from, "triumph");
	else strcpy(p_ptr->died_from, "escape");

	return TRUE;
}


//v1.1.92 
//�񕜎{�݂ŋ߂��̔z�����񕜂��邱�Ƃɂ���
//ratio:HP�̉񕜗�
static void bldg_heal_all_pets(int ratio)
{
	int i;

	//�t���A�̃����X�^�[�����[�v
	for (i = 1; i < m_max; i++)
	{
		int heal;
		bool flag_done = FALSE;

		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr;
		char m_name[240];
		r_ptr = &r_info[m_ptr->r_idx];
		if (!m_ptr->r_idx) continue;
		if (!is_pet(m_ptr)) continue;
		if (m_ptr->cdis > 3) continue;

		monster_desc(m_name, m_ptr, 0);
		r_ptr = &r_info[m_ptr->r_idx];
		heal = m_ptr->max_maxhp * ratio / 100;
		if (heal < 300) heal = 300;

		if (m_ptr->hp < m_ptr->max_maxhp)
		{
			int tmp;
			flag_done = TRUE;

			tmp = m_ptr->hp + heal;
			if (tmp > m_ptr->max_maxhp) tmp = m_ptr->max_maxhp;
			if (tmp > m_ptr->maxhp) m_ptr->maxhp = tmp;
			m_ptr->hp = tmp;

		}
		if (MON_STUNNED(m_ptr))
		{
			flag_done = TRUE;
			set_monster_stunned(i, 0);
		}
		if (MON_CONFUSED(m_ptr))
		{
			flag_done = TRUE;
			set_monster_confused(i, 0);

		}
		if (MON_SLOW(m_ptr))
		{
			flag_done = TRUE;
			set_monster_slow(i, 0);

		}
		if (MON_MONFEAR(m_ptr))
		{
			flag_done = TRUE;
			set_monster_monfear(i, 0);

		}

		if(flag_done) msg_format("%s�͉񕜂����B", m_name);
	}



	return;

}





/*:::����_�Ё@����*/
/*:::*����*�Ƃ͈Ⴂ�i���̎􂢕t�������̉���͑S���s���Ȃ��������ʓ|�Ȃ̂ŕ��u*/
//v1.1.32 ���_�Ђ��ǉ������̂Ń��b�Z�[�W�ǉ�
bool bldg_remove_curse( void)
{
	int i;
	u32b flag_curse=0L;
	u32b flag_perma=0L;

	//1:�얲 2:��� 3:���~��炵
	int msg_mode = 0;
	
	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];

	if (p_ptr->town_num == TOWN_HAKUREI)
		msg_mode = 1;
	else if (p_ptr->town_num == TOWN_MORIYA)
		msg_mode = 2;
	else if(EXTRA_MODE && building_ex_idx[ex_bldg_num] == BLDG_EX_REIMU)
		msg_mode = 1;
	else if (EXTRA_MODE && building_ex_idx[ex_bldg_num] == BLDG_EX_MORIYA)
		msg_mode = 2;
	else if(EXTRA_MODE && building_ex_idx[ex_bldg_num] == BLDG_EX_ZASHIKI)
		msg_mode = 3;

	for(i=INVEN_RARM;i<INVEN_TOTAL;i++)
	{
		object_type *o_ptr = &inventory[i];
		if (!o_ptr->k_idx) continue;
		if (!object_is_cursed(o_ptr)) continue;
		flag_curse |= 1L<<(i-INVEN_RARM);
		if(o_ptr->curse_flags & TRC_PERMA_CURSE)
		{
			flag_perma  |= 1L<<(i-INVEN_RARM);
		}
	}
	if(!flag_curse)
	{
		msg_print("���Ȃ��͎���Ă��Ȃ��B");
		return FALSE;
	}
	else if(flag_curse & ~flag_perma)
	{
		switch (msg_mode)
		{
		case 1:
			if (p_ptr->pclass == CLASS_REIMU)
				msg_print("���Ђ̌˂��J���z�K�q���o�Ă��Ď􂢂��z������Ă��ꂽ�B");
			else
				msg_print("�얲�͂��Ȃ��̖ڂ̑O�Ɏ���������Ė��Ȏ���Ŏw�𓮂������B");
			break;
		case 2:
			if (p_ptr->pclass == CLASS_KANAKO)
				msg_print("�z�K�q���j���j�����Ȃ���o�Ă��Ď􂢂��z��������B");
			else
				msg_print("�_�ގq���o�Ă��āu�n�@�[�[�I�I�v�ƋC����������I");
			break;

		case 3:
			msg_print("���~��炵��������ۂ悭���􂵂Ă��ꂽ�B");
			break;

		default:
			msg_print("ERROR:���̌����̉����������b�Z�[�W���ݒ肳��Ă��Ȃ�");
		}

		if(remove_all_curse())
		{
			msg_print("�����i�̎􂢂��������I");

		}
		return TRUE;
	}
	else
	{

		switch (msg_mode)
		{
		case 1:
			if (p_ptr->pclass == CLASS_REIMU)
				msg_print("���Ђ̌˂��J���z�K�q���o�Ă��āA�������킸�Ɉ������񂾁B");
			else
				msg_print("�얲�u���߂�Ȃ����B���̎􂢂͎��̎�ɂ������Ȃ���B�v");
			break;
		case 2:
			if (p_ptr->pclass == CLASS_KANAKO)
				msg_print("�z�K�q�u�����܂��A�Ђǂ��M��Ɍ����߂�ꂽ���񂾂˂��B�܂����ǂ��t�������񂾂ˁB�v");
			else
				msg_print("�_�ގq�u�ς܂ʂ����̎􂢂͐_�̎�ɂ������ʁB�w�}�f�̊����x��T���B�v");
			break;
		case 3:
			msg_print("���~��炵�������q�\�q�\�Ƒ��k�������Ă���B�䂪�Ƃ���̈����z�����������Ă���悤��...");
			break;
		default:
			msg_print("ERROR:���̌����̉��􎸔s���b�Z�[�W���ݒ肳��Ă��Ȃ�");
		}

		return FALSE;
	}


}


/*�G���W�j�A�M���h�F�@�B�����/���Օi�𒲒B����*/
static void bact_engineer_guild(bool make_machine)
{

	int cs=0,page=0,choice=-1;
	int i;
	char c;
	object_type forge;
	object_type *q_ptr = &forge;
	int slot_new;
	char o_name[MAX_NLEN];
	int price;
	int supply_price_list[MAX_MACHINE_SUPPLY];

	int power = p_ptr->lev + p_ptr->stat_ind[A_INT] + p_ptr->stat_ind[A_DEX] + 6;

	object_wipe(q_ptr);
	building_prt_gold();

	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI)
	{
		msg_print("���Ȃ��̓G���W�j�A�ł͂Ȃ��̂ŉ��ɓ���Ă��炦�Ȃ������B");
		return;
	}

	if(make_machine && inventory[INVEN_PACK-1].k_idx)
	{
		msg_print("�ו�����t���B���炵�Ă��痈�悤�B");
		return;
	}
	//���i�ϓ����l���������Օi�l�i�e�[�u�������
	for(i=0;i<MAX_MACHINE_SUPPLY;i++)
	{
		object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[i][0], engineer_guild_supply_table[i][1]));
		supply_price_list[i] = k_info[q_ptr->k_idx].cost;
		if(supply_price_list[i] > 100)
		{
			int mod = p_ptr->magic_num1[i];
			if(p_ptr->town_num == TOWN_KAPPA) mod = MAX(-50,mod-10);
			supply_price_list[i] = supply_price_list[i] / 100 * (100+mod);
		}
	}

	while(1)
	{

		clear_bldg(5, 18);
		if(make_machine)
		{
			c_put_str(TERM_WHITE,"�u�������́H�v",5 , 6);
			c_put_str(TERM_WHITE,"(a�`j:�i�ڂ�I�� /x:���������� / n,�X�y�[�X:���̃y�[�W�� / ESC:�L�����Z��)",6 , 6);
		}
		else
		{
			 c_put_str(TERM_WHITE,"�u�ǂ���������H�v",5 , 6);
			c_put_str(TERM_WHITE,"(a�`j:�i�ڂ�I�� / n,�X�y�[�X:���̃y�[�W�� / ESC:�L�����Z��)",6 , 6);
		}
		for(i=0;i<10;i++)
		{

			int color;
			int table_num = page * 10 + i;
			if(make_machine)
			{
				object_prep(q_ptr, lookup_kind(TV_MACHINE, table_num));

				if(cs==i) color = TERM_YELLOW;
				else if(p_ptr->au < k_info[q_ptr->k_idx].cost) color = TERM_L_DARK;
				else color = TERM_WHITE;
				object_desc(o_name, q_ptr, 0);
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					c_put_str(color,format("%c) [ �ݔ��s�� ] ",'a'+i),7 + i, 10);
				}
				else if(machine_table[table_num].difficulty > power)
				{
					c_put_str(color,format("%c) [ �쐬�s�� ] ",'a'+i),7 + i, 10);
				}
				else 		
					c_put_str(color,format("%c) [$ %8d] %s",'a'+i, k_info[q_ptr->k_idx].cost,o_name),7 + i, 10);

			}
			else //���Օi���B�e�[�u�� �Ƃ肠�����S�Ă�\��
			{
				if(table_num >= MAX_MACHINE_SUPPLY) break;
				object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[table_num][0], engineer_guild_supply_table[table_num][1]));

				if(cs==i) color = TERM_YELLOW;
				else if(p_ptr->au < supply_price_list[table_num]) color = TERM_L_DARK;
				else color = TERM_WHITE;
				object_desc(o_name, q_ptr, 0);
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					c_put_str(color,format("%c) [ �݌ɖ��� ] ",'a'+i),7 + i, 10);
				}
				else 
					c_put_str(color,format("%c) [$ %8d] %s",'a'+i, supply_price_list[table_num],o_name),7 + i, 10);
			}
		}

		prt("",0,0);
		c = inkey();
		if (c == ESCAPE)
		{
			clear_bldg(5, 18);
			return;
		}
		if (c == '2' && cs < 9) cs++;
		if (c == '8' && cs > 0) cs--;
		if (c == '\r' || c == '\n') c=cs+'a';
		if (c == ' ' || c == 'n') page++;
		if (make_machine && page > 2) page = 0;
		else if (!make_machine && page > 1) page = 0;

		if (make_machine && c == 'x')
		{
			choice = page*10 + cs;
			if(p_ptr->town_num == TOWN_HITOZATO && page)
			{
				msg_print("�u�����̐ݔ����ᖳ�����ˁB�������ǎ������̃o�U�[�܂ŗ��Ă�B�v");
				msg_print(NULL);
				continue;
			}
			else if(machine_table[choice].difficulty > power)
			{
				msg_print("���̂��Ȃ��ɂ͂�������͓̂���悤���B");
				msg_print(NULL);
				continue;
			}
			object_prep(q_ptr, lookup_kind(TV_MACHINE, choice));
			screen_object(q_ptr, SCROBJ_FORCE_DETAIL);

		}

		if(c >= 'a' && c <= 'j')
		{
			int number;
			choice = page*10 + (c - 'a');
			if(!make_machine && choice >= MAX_MACHINE_SUPPLY) continue; //���Օi���B�ŋ󗓂�I�񂾏ꍇ

			if(make_machine)
			{
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					msg_print("�u�����̐ݔ����ᖳ�����ˁB�������ǎ������̃o�U�[�܂ŗ��Ă�B�v");
					msg_print(NULL);
					continue;
				}
				else if(machine_table[choice].difficulty > power)
				{
					msg_print("���̂��Ȃ��ɂ͂�������͓̂���悤���B");
					msg_print(NULL);
					continue;
				}

				object_prep(q_ptr, lookup_kind(TV_MACHINE, choice));
				number = 1;
				break;
			}
			else 
			{
				if(p_ptr->town_num == TOWN_HITOZATO && page)
				{
					msg_print("�u���̎x���ɂ͒u���ĂȂ���B�������ǎ������̃o�U�[�܂ŗ��Ă�B�v");
					msg_print(NULL);
					continue;
				}
				else
				{
					object_prep(q_ptr, lookup_kind(engineer_guild_supply_table[choice][0], engineer_guild_supply_table[choice][1]));
					number = get_quantity("�����������܂����H",99);
					if(!number) continue;
					q_ptr->number = number;
					if(!inven_carry_okay(q_ptr))
					{
						msg_print("����ȂɎ�������Ȃ��B");
						clear_bldg(5, 18);
						return;
					}
					break;
				}
			}
		}		
	}

	clear_bldg(5, 18);
	if(make_machine) price = k_info[q_ptr->k_idx].cost;
	else price = supply_price_list[choice] * q_ptr->number;
	object_desc(o_name, q_ptr, 0);
	if(p_ptr->au < price)
	{
		msg_format("����������Ȃ��B");
		return;
	}

	if(make_machine) msg_format("$%d�g����%s�����B",price,o_name);
	else	msg_format("$%d��%s�𒲒B����B",price,o_name);
	if (!get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) return;

	//���̂Ƃ������p
	apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
	//�A�C�e����*�Ӓ�*��Ԃɂ���
	identify_item(q_ptr);
	q_ptr->ident |= (IDENT_MENTAL);

	p_ptr->au -= price;
	building_prt_gold();

	if(make_machine)
	{
		msg_print("���Ȃ��͋@�B�̐���Ɏ��|�������E�E");
		msg_print(NULL);
		msg_format("%s�����������I",o_name);
	}
	else  msg_format("%s���󂯎�����B",o_name);

	slot_new = inven_carry(q_ptr);
	autopick_alter_item(slot_new, FALSE);		

	handle_stuff();
	clear_bldg(5, 18);

}


/*:::�G���W�j�A�M���h�̐����𕷂�*/
static void engineer_guild_guide(void)
{
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI)
	{
		msg_print("���Ȃ��̓G���W�j�A�ł͂Ȃ��̂ł����łł��邱�Ƃ͂Ȃ��悤���B");
		return;
	}
	display_rumor_new(21);
}


//v1.1.92 EX�_���W���������p�̉\�R�}���h ���������Ƃ�
static void ex_rumor_new(void)
{

	int ex_bldg_num = f_info[cave[py][px].feat].subtype;
	int ex_bldg_idx = building_ex_idx[ex_bldg_num];

	switch(ex_bldg_idx)
	{
	case BLDG_EX_GRASSROOTS:
		display_rumor_new(23);
		break;

	case BLDG_EX_YUMA:
		display_rumor_new(24);
		break;
	default:
		msg_print("ERROR:���̌�����ex_rumor_new()�������o�^����Ă��Ȃ�");


	}
}


//v1.1.92 �ޖ������ꂽ�A�C�e����H�ׂď��ł�����
static void yuma_eats_cursed_item(void)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point, total_point;

	q = "�u�ǂ������Ăق����H�v";
	s = "�u�􂢂̓����͂��Ȃ��ȁB�܂��B�v";

	item_tester_hook = item_tester_hook_cursed;
	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return;
	o_ptr = &inventory[item];

	if (!(wield_check(item, INVEN_PACK,0)))
	{
		return;
	}

	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if (!get_check(format("�u%s���򂤂��H�����񂾂ȁH�v", o_name))) return;

	if (o_ptr->curse_flags & TRC_PERMA_CURSE)
	{
		prt("�u�����͐�N���̂̎􂢂��ȁI�h�{���_���I�v",10,20);
	}
	else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
	{
		prt("�u�Ȃ��Ȃ������Ȃ����̎􂢂��ȁB�v", 10, 20);
	}
	else
	{
		prt("�u�n���Ȏ􂢂��ȁB�򂢑����B�v", 10, 20);
	}


	inven_item_increase(item, (0-o_ptr->number));
	inven_item_describe(item);
	inven_item_optimize(item);

}



///mod150811 ���������@�쐬���[����ǂ�
static void marisa_read_memo(void)
{
	if(p_ptr->pclass != CLASS_MARISA)
	{
		msg_print("ERROR:�������ȊO��marisa_read_memo()���Ă΂ꂽ");
		return;
	}

	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("���͈Ŏs��̒��������B�A�r���e�B�J�[�h��T���ɍs�����B");
		return;
	}

	display_rumor_new(22);
}


/*
 * Execute a building command
 */
/*:::�������ł̃R�}���h����*/
///sys �������R�}���h����
static void bldg_process_command(building_type *bldg, int i)
{
	int bact = bldg->actions[i];
	int bcost;
	bool paid = FALSE;
	int amt;

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);



	//v2.0.8 �����炩�m���EXTRA�����ŉ��i��member_costs�ɐݒ肵�Ă�̂ɂ�����other_costs�ŏ������Ă邩��S���̉��i��0�ɂȂ��Ă�
	//���̏����ɖ߂��Bis_owner()�͌�����TRUE�Ȃ̂�member_costs[]�݂̂��Q�Ƃ����
	if (is_owner(bldg))
		bcost = bldg->member_costs[i];
	else

		bcost = bldg->other_costs[i];

	if(cheat_room) msg_format("cost:%d", bcost);

	/* action restrictions */
	///building �����ύX���Ȃ��Ƃ����Ȃ�
	if (((bldg->action_restr[i] == 1) && !is_member(bldg)) ||
	    ((bldg->action_restr[i] == 2) && !is_owner(bldg)))
	{
#ifdef JP
msg_print("�����I�����錠���͂���܂���I");
#else
		msg_print("You have no right to choose that!");
#endif
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	/*:::����������Ȃ��ꍇreturn ������������[�U�͕ʏ���*/
	//v1.1.48 �������}�C�i�X���Ɩ����̍s�ׂ��ł��Ȃ������̂ŏ������ǉ�
	if ((bact != BACT_RECHARGE) &&
	    ((bldg->member_costs[i] > 0 && (bldg->member_costs[i] > p_ptr->au) && is_owner(bldg)) ||
	     (bldg->other_costs[i] > 0 && (bldg->other_costs[i] > p_ptr->au) && !is_owner(bldg))))
	{
#ifdef JP
msg_print("����������܂���I");
#else
		msg_print("You do not have the gold!");
#endif
		return;
	}

	clear_bldg(4, 18);

	///sys building �����̊e�R�}���h���� BACT_???�Ɉ�v
	switch (bact)
	{
	case BACT_NOTHING:
		/* Do nothing */
		break;
	case BACT_RESEARCH_ITEM:
		paid = identify_fully(FALSE);
		break;
		///sysdel building �폜�ݔ�
	case BACT_TOWN_HISTORY:
		town_history();
		break;
		///sysdel building �폜�ݔ�
	case BACT_RACE_LEGENDS:
		race_legends();
		break;
	case BACT_QUEST:
		castle_quest();
		break;
	case BACT_KING_LEGENDS:
	case BACT_ARENA_LEGENDS:
	case BACT_LEGENDS:
		show_highclass();
		break;

	case BACT_POSTER:
	///del131213
	case BACT_TALK_KAGUYA:
		kaguya_quest();
		break;
	case BACT_ARENA:
		msg_print("ERROR:arena_comm()�͂����R�����g�A�E�g����");
		//arena_comm(bact);
		break;
	///sysdel �J�W�m
	//case BACT_IN_BETWEEN:
	//case BACT_CRAPS:
	//case BACT_SPIN_WHEEL:
	//case BACT_DICE_SLOTS:
	//case BACT_GAMBLE_RULES:
	//case BACT_POKER:
	//	gamble_comm(bact);
	//	break;
	case BACT_REST:
	case BACT_RUMORS:
	case BACT_FOOD:
		paid = inn_comm(bact);
		break;
	case BACT_RESEARCH_MONSTER:
		paid = research_mon();
		break;
	case BACT_COMPARE_WEAPONS:
		paid = TRUE;
		bcost = compare_weapons(bcost);
		break;
	case BACT_ENCHANT_WEAPON:
		item_tester_hook = object_allow_enchant_melee_weapon;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_ARMOR:
		item_tester_hook = object_is_armour;
		enchant_item(bcost, 0, 0, 1);
		break;
	case BACT_RECHARGE:
		building_recharge();
		break;
	case BACT_RECHARGE_ALL:
		building_recharge_all();
		break;
	case BACT_IDENTS: /* needs work */
#ifdef JP
		if (!get_check("��������S�ĊӒ肵�Ă�낵���ł����H")) break;
		identify_pack();
		msg_print(" �������S�Ă��Ӓ肳��܂����B");
#else
		if (!get_check("Do you pay for identify all your possession? ")) break;
		identify_pack();
		msg_print("Your possessions have been identified.");
#endif

		paid = TRUE;
		break;
	case BACT_IDENT_ONE: /* needs work */
		paid = ident_spell(FALSE);
		break;
	//case BACT_LEARN:
		/*:::�������w�K����B�������݂��̐ݔ��������͂Ȃ��炵���B*/
	//	do_cmd_study();
	//	break;
	case BACT_HEALING: /* needs work */
		if (dun_level)
			msg_print("���Â��󂯂��B");
		else
			msg_print("���Ȃ��͉���ő̂���₵��...");

		//v2.0.3 ���ւ�l�`�Ȃǂ̔j�������邱�Ƃɂ���
		if (RACE_BREAKABLE)
		{
			set_broken(-(BROKEN_MAX));
			handle_stuff();
		}

		hp_player(200);
		set_poisoned(0);
		set_blind(0);
		set_confused(0);
		set_cut(0);
		set_stun(0);

		//v1.1.92 �z�����񕜂���
		bldg_heal_all_pets(30);

		paid = TRUE;
		break;

		///mod140113 �i�����p�ɋ�������
	case BACT_HEALING_PLUS: 
		msg_print("���Â��󂯂��B");

		//v2.0.3 ���ւ�l�`�Ȃǂ̔j�������邱�Ƃɂ���
		if (RACE_BREAKABLE)
		{
			set_broken(-(BROKEN_MAX));
			handle_stuff();
		}

		hp_player(5000);
		set_poisoned(0);
		set_blind(0);
		set_confused(0);
		set_cut(0);
		set_stun(0);
		set_image(0);
		set_asthma(0);
		restore_level();
		set_food(MAX(p_ptr->food, PY_FOOD_FULL - 1));

		//v1.1.92 �z�����񕜂���
		bldg_heal_all_pets(100);



		paid = TRUE;
		break;
	case BACT_RESTORE: /* needs work */
		if (do_res_stat(A_STR)) paid = TRUE;
		if (do_res_stat(A_INT)) paid = TRUE;
		if (do_res_stat(A_WIS)) paid = TRUE;
		if (do_res_stat(A_DEX)) paid = TRUE;
		if (do_res_stat(A_CON)) paid = TRUE;
		if (do_res_stat(A_CHR)) paid = TRUE;
		if(!paid) msg_print("�\�͉񕜂̕K�v�͂Ȃ��悤���B");
		break;
	case BACT_ENCHANT_ARROWS:
		item_tester_hook = item_tester_hook_ammo;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_BOW:
		//item_tester_tval = TV_BOW;
		item_tester_hook = item_tester_hook_bow_crossbow;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_RECALL:
		if (recall_player(1)) paid = TRUE;
		break;
	///sysdel �g�����v���̊K�w��A�� �Ƃ肠���������ɂ��Ă����\��
	case BACT_TELEPORT_LEVEL:
	{
		int select_dungeon;
		int max_depth;

		clear_bldg(4, 20);
#ifdef JP
		select_dungeon = choose_dungeon("�Ƀe���|�[�g", 4, 0);
#else
		select_dungeon = choose_dungeon("teleport", 4, 0);
#endif
		show_building(bldg);
		if (!select_dungeon) return;

		max_depth = d_info[select_dungeon].maxdepth;

		/* Limit depth in Angband */
		///quest �g�����v���̃e���|�ŃI�x�������΂��Ȃ��悤�ɂ��镔��
		if (select_dungeon == DUNGEON_ANGBAND)
		{
			///mod140118 �ŏI�{�X���I�x�����ɂ���
			if (quest[QUEST_YUKARI].status != QUEST_STATUS_FINISHED) max_depth = 50;
			//easy�ł͎���|���Ă�50�K�܂ł����s���Ȃ�
			if (difficulty == DIFFICULTY_EASY ) max_depth = 50;

			//if (quest[QUEST_TAISAI].status != QUEST_STATUS_FINISHED) max_depth = 98;
			//else if(quest[QUEST_SERPENT].status != QUEST_STATUS_FINISHED) max_depth = 99;
		}

#ifdef JP
		amt = get_quantity(format("%s�̉��K�Ƀe���|�[�g���܂����H", d_name + d_info[select_dungeon].name), max_depth);
#else
		amt = get_quantity(format("Teleport to which level of %s? ", d_name + d_info[select_dungeon].name), max_depth);
#endif

		if (amt > 0)
		{
			p_ptr->word_recall = 1;
			p_ptr->recall_dungeon = select_dungeon;
			max_dlv[p_ptr->recall_dungeon] = ((amt > d_info[select_dungeon].maxdepth) ? d_info[select_dungeon].maxdepth : ((amt < d_info[select_dungeon].mindepth) ? d_info[select_dungeon].mindepth : amt));
			if (record_maxdepth)
#ifdef JP
				do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "�g�����v�^���[��");
#else
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "at Trump Tower");
#endif
#ifdef JP
			msg_print("���̑�C������߂Ă���...");
#else
			msg_print("The air about you becomes charged...");
#endif

			paid = TRUE;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	}

	case BACT_LOSE_MUTATION:
	/*:::�ˑR�ψَ��Á@���b�L�[�}���̔��I�[������*/
		///mod140318 �i���^�ψفA�����^�ψق̔���������ď��������������B
		{
			///mod160225 �o�O�Ή��̂��߉i���ψق�t������
			p_ptr->muta1 |= p_ptr->muta1_perma;
			p_ptr->muta2 |= p_ptr->muta2_perma;
			p_ptr->muta3 |= p_ptr->muta3_perma;
			p_ptr->muta4 |= p_ptr->muta4_perma;

			if(muta_erasable_count())
			{
				/*:::�ˑR�ψق������_���Ɏ����B20�񎎍s���č������Ă�ψقɓ�����Ȃ���FALSE�ɂȂ邪�����ɓ�����܂ł����ŌJ��Ԃ��Ă���*/
				while(!lose_mutation(0));
				paid = TRUE;	
			}
			else
			{
#ifdef JP
				msg_print("�����ׂ��ˑR�ψق������B");
#else
				msg_print("You have no mutations.");
#endif
				msg_print(NULL);
			}
			p_ptr->update |= PU_BONUS;
		}
		break;
	///sysdel ���Z��
	case BACT_BATTLE:
		msg_print("���Z��͐����悭������Ȃ��B");
		//kakutoujou();
		break;
	//���_��������
	case BACT_RYUUJIN:

		if(difficulty == DIFFICULTY_LUNATIC)
		{
			msg_print("����ȑ����������Ă���B�^�R�Ɏ��������A�����̐G�r�A�ЁX������܁E�E�H");
			if(!p_ptr->resist_chaos && !p_ptr->resist_insanity)set_image(p_ptr->image + 100);
		}
		else if(difficulty == DIFFICULTY_HARD)
			msg_print("���_���̖ڂ͐Ԃ������Ă���E�E");
		else
			msg_print("���_���̖ڂ͐������Ă���B�����͂����V�C�炵���B");
		//tsuchinoko();
		break;
	//case BACT_KUBI:
		///del131221 ���̍폜
		//msg_print("�������͕X�����B");
		//shoukinkubi();
		break;
	//case BACT_TARGET:
		///del131221 ���̍폜
		msg_print("�������͕X�����B");
		//today_target();
		break;
	//case BACT_KANKIN:
		///del131221 ���̍폜
		//kankin();
		//break;
		///sysdel virtue
	case BACT_GACHA:
		buy_gacha_box();

		break;
	case BACT_GLASSROOTS_RUMOR:
		msg_print("���������B");
		//kankin();
		break;
		/*:::���t��*/
	case BACT_HEIKOUKA:
	msg_print("���Ȃǒm��ʁB");
#if 0
#ifdef JP
		msg_print("���t���̋V�����s�Ȃ����B");
#else
		msg_print("You received an equalization ritual.");
#endif
		set_virtue(V_COMPASSION, 0);
		set_virtue(V_HONOUR, 0);
		set_virtue(V_JUSTICE, 0);
		set_virtue(V_SACRIFICE, 0);
		set_virtue(V_KNOWLEDGE, 0);
		set_virtue(V_FAITH, 0);
		set_virtue(V_ENLIGHTEN, 0);
		set_virtue(V_ENCHANT, 0);
		set_virtue(V_CHANCE, 0);
		set_virtue(V_NATURE, 0);
		set_virtue(V_HARMONY, 0);
		set_virtue(V_VITALITY, 0);
		set_virtue(V_UNLIFE, 0);
		set_virtue(V_PATIENCE, 0);
		set_virtue(V_TEMPERANCE, 0);
		set_virtue(V_DILIGENCE, 0);
		set_virtue(V_VALOUR, 0);
		set_virtue(V_INDIVIDUALISM, 0);
		get_virtues();
		paid = TRUE;
#endif

		break;

		/*:::�X�ړ�*/
	case BACT_TELE_TOWN:
		paid = tele_town(FALSE);
		break;
		//v1.1.32
	case BACT_ROPE_WAY:
		paid = riding_ropeway();
		break;

		//v1.1.91
	case BACT_GO_UNDERGROUND:
		paid = pass_through_chirei_koudou();
		break;

	case BACT_EX_RUMOR_NEW:
		ex_rumor_new();
		break;


	/*:::�h��ɂ��Ē��ׂ�i���폠�j*/
	case BACT_EVAL_AC:
		paid = eval_ac(p_ptr->dis_ac + p_ptr->dis_to_a);
		break;
	/*:::�܂ꂽ����C��*/
	case BACT_BROKEN_WEAPON:
		paid = TRUE;
		bcost = repair_broken_weapon(bcost);
		break;
	///mod140112 �\�@�����s�ɏC��
	case BACT_RUMORS_NEW:
		display_rumor_new(p_ptr->town_num);
		paid = TRUE;
		break;
	///mod140113 ���J���@�X�@�ڍז�����
	case BACT_MARIPO_GAIN:
		material_2_maripo();
		break;
	case BACT_MARIPO_CHANGE:
		maripo_2_present();
		break;
	case BACT_KORIN_MAKEOVER:
		msg_print("�ς܂Ȃ����A�܂��������ł��Ă��Ȃ��񂾂�B");
		break;
	case BACT_TALK_KAGEROU:
		talk_kagerou();
		break;
	case BACT_BARTER:
		grassroots_barter();
		break;
	case BACT_TRADING_ABILITY_CARD:
		trading_ability_cards();
		break;
	case BACT_BUY_ABILITY_CARD:
		buy_ability_card(FALSE);
		break;
	case BACT_SELL_ABILITY_CARD:
		sell_ability_card();
		break;

	case BACT_CHECK_ABILITY_CARD:
		buy_ability_card(TRUE);
		break;

	case BACT_ARCHER_GUILD_GUIDE:
		archer_guild_guide();
		break;
	case BACT_ARCHER_GUILD_ARROW_ENC:
		archer_guild_arts2();
		break;
	case BACT_ARCHER_GUILD_ARROW_EGO:
		archer_guild_arts();
		break;
	case BACT_REMOVE_CURSE:
		paid = bldg_remove_curse();
		break;
	case BACT_ESCAPE_FROM_GENSOUKYOU:
		if(escape_from_gensoukyou()) return;
		break;
	case	BACT_SELL_CURIOSITY:
		sell_curiosity();
		break;
	case BACT_MAKE_MACHINE:
		bact_engineer_guild(TRUE);
		break;
	case BACT_BUY_MACHINE_SUPPLY:
		bact_engineer_guild(FALSE);
		break;

	case BACT_ENGINEER_GUILD_GUIDE:
		engineer_guild_guide();
		break;

	case BACT_SMITH_GUILD:
		do_cmd_kaji(FALSE);
		break;
	case BACT_SMITH_GUILD_GUIDE:
		do_cmd_kaji(TRUE);
		break;
	case BACT_REIMU_FORTUNETELLER:
		paid = reimu_fortune_teller();
		break;
	case	BACT_REIMU_DEITY_CHECK:
		reimu_deity_check();
		break;
	///mod150129 ���ΑK��
	case	BACT_THROW_OSAISEN:
		throw_osaisen();
		break;
	case	BACT_CHECK_OSAISEN:
		check_osaisen();
		break;
	case BACT_BATTLE_MON_GAMBLING:
		battle_mon_gambling();
		break;
	case BACT_MARISA_MAKE_MAGIC_RECIPE:
		marisa_make_magic_recipe();
		break;
	case BACT_MARISA_READ_MEMO:
		marisa_read_memo();
		break;
	case BACT_MARISA_EXTRACT_MATERIAL:
		marisa_extract_material(TRUE);
		break;
	case BACT_MARISA_CHECK_ESSENCE:
		marisa_check_essence();

		break;
	case BACT_MARISA_MAKE_MAGIC:
		make_marisa_magic();
		break;
	case BACT_MARISA_CARRY_MAGIC:
		carry_marisa_magic();
		break;
	case BACT_MARISA_CHECK_MAGIC:
		check_marisa_recipe();
		break;
	case BACT_PRISM_CLASS_CHANGE:
		prism_change_class(FALSE);
		break;
	case BACT_HINA_MAKE_MONEY:
		hina_at_work();
		break;
	case BACT_COMPOUND_DRUG:
		compound_drug();
		break;

	case BACT_CHECK_DRUG_RECIPE:
		check_drug_recipe();
		break;

	case BACT_KOGASA_SMITH:
		kogasa_smith();
		break;

	case BACT_EX_SEARCH_AROUND:
		exbldg_search_around();
		break;

	case BACT_DESTROY_ITEM:
		yuma_eats_cursed_item();
		break;


	case BACT_BUY_HINA_NINGYOU:
		buy_hinaningyou();
		break;
		//Ex���[�h����@�A�C�e��������ɉ^��ł��炤
	case BACT_ITEM_TRANSFER:
		{
			int ex_bldg_num = f_info[cave[py][px].feat].subtype;
			int ex_bldg_idx = building_ex_idx[ex_bldg_num];
			int time_div = 8;
			//Hack - Ex�_���W�����̂��ӂɃA�C�e�����^��ł��炤�����́A�A�C�e��������玝���Ă��Ă�����Ă���Œ��ɂ͖����B
			if(ex_bldg_idx == BLDG_EX_ORIN && 
				(town[TOWN_HAKUREI].store[STORE_GENERAL].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) > turn ) && !p_ptr->wizard)
			{
				msg_print("���͒N�����Ȃ��B");
				break;
			}

			paid = item_recall(4);
			break;

		}

	case BACT_ITEMCARD_TRADING:
		grassroots_trading_cards();
		break;
	case BACT_ORIN_DELIVER_ITEM:
		hack_flag_access_home_orin = TRUE;
		paid = orin_deliver_item();
		hack_flag_access_home_orin = FALSE;
		break;

	case BACT_HELLO_WORK:
		paid = straygod_job_adviser();
		break;
	case BACT_NIGHTMARE_DIARY:
		if(nightmare_diary()) 	leave_bldg = TRUE;
		break;
	case BACT_BUY_STRANGE_OBJ:
		bact_buy_strange_object();
		break;

	case BACT_MAKE_SPELLCARD:
		bact_marisa_make_spellcard();
		break;

	case BACT_HATATE_SEARCH_MON:
		paid = hatate_search_unique_monster();
		break;

	case BACT_GEIDONTEI_COOKING:
		geidontei_cooking(p_ptr->pclass == CLASS_MIYOI);//�����̂Ƃ��̂݁u�����ō��t���O�vON
		break;

	default:
		msg_format("ERROR:BACT(%d)������`",bact);

	}

	if (paid)
	{
		p_ptr->au -= bcost;
	}
}


/*
 * Enter quest level
 */
void do_cmd_quest(void)
{
	energy_use = 100;

	if (!cave_have_flag_bold(py, px, FF_QUEST_ENTER))
	{
#ifdef JP
msg_print("�����ɂ̓N�G�X�g�̓����͂Ȃ��B");
#else
		msg_print("You see no quest level here.");
#endif

		return;
	}
	else
	{
		int quest_num = cave[py][px].special;


#ifdef JP
		///v1.1.12 �N�G�X�g���\�L
		if(quest_num)
		{
			//�N�����_�ł�quest[]�ɖ��O�������Ă��Ȃ��̂�p_ptr->inside_quest���ꎞ�I�ɏ��������ăN�G�X�g���������s���K�v������
			int old_quest = p_ptr->inside_quest;
			p_ptr->inside_quest = quest_num;
			init_flags = INIT_NAME_ONLY;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			msg_format("�����ɂ̓N�G�X�g�u%s�v(���x��%d)�ւ̓���������܂��B",quest[quest_num].name,quest[quest_num].level);
			p_ptr->inside_quest = old_quest;
		}
		else
			msg_print("�����ɂ̓N�G�X�g�ւ̓���������܂��B");

		if (!get_check("�N�G�X�g�ɓ���܂����H")) return;
		///del131213  msg
		//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
		//	msg_print("�w�Ƃɂ��������Ă݂悤�����B�x");
#else
		msg_print("There is an entry of a quest.");
		if (!get_check("Do you enter? ")) return;
#endif

		/* Player enters a new quest */
		p_ptr->oldpy = 0;
		p_ptr->oldpx = 0;
		/*:::�N�G�X�g�̏I����Ԃ��`�F�b�N�H�Ȃ����H*/
		leave_quest_check();

		if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM) dun_level = 1;
		p_ptr->inside_quest = cave[py][px].special;

		p_ptr->leaving = TRUE;

		///mod140629 �ϒK�푈
		/*:::-Hack- �ϒK�푈�N�G�ɓ���Ƃ��A�ǂ���̐��͂ɂ����I�����ăt���O��ۑ�����*/
		if(cave[py][px].special == QUEST_KORI)
		{
			bool enable_a = TRUE;
			bool enable_b = TRUE;
			bool enable_c = TRUE;
			int i;
			char c;

			screen_save();
			//���͌ϐw�c�̂�
			if(p_ptr->pclass == CLASS_RAN) 
			{
				 enable_b = FALSE;
				 enable_c = FALSE;
			}
			//�}�~�]�E�͒K�w�c�̂�
			else if(p_ptr->pclass == CLASS_MAMIZOU) 
			{
				 enable_a = FALSE;
				 enable_c = FALSE;
			}
			else if(p_ptr->prace == RACE_YOUKO) enable_b = FALSE;
			else if(p_ptr->prace == RACE_BAKEDANUKI) enable_a = FALSE;
			///mod150110 ��͒K�w�c�ɕt���Ȃ�
			else if(p_ptr->pclass == CLASS_CHEN) enable_b = FALSE;
			///mod150328 �ʂ��͒K�w�c�̂�
			else if(p_ptr->pclass == CLASS_NUE) enable_a = FALSE;
			//�얲�Ɖf�P�͟r�ł̂�
			//v2.0.17 �c�����S�ł݂̂ɂ��Ƃ�
			else if(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_EIKI || p_ptr->pclass == CLASS_ZANMU)
			{
				 enable_a = FALSE;
				 enable_b = FALSE;
			}

			if(!enable_a && !enable_b && !enable_c)
			{
				msg_format("ERROR:�ϒK�푈�N�G�X�g�̑I�������S�Ė���");
				return;
			}

			msg_print("�ǂ���̐��͂ɂ��܂����H");
			for(i=1;i<7;i++)Term_erase(12, i, 255);
			if(enable_a) put_str("a) �ϐw�c�ɖ�������", 3 , 40);
			if(enable_b) put_str("b) �K�w�c�ɖ�������", 4 , 40);
			if(enable_c) put_str("c) �S���|��", 5 , 40);

			while(1)
			{
				c = inkey();
				if(c == 'a' && !enable_a) continue;
				if(c == 'b' && !enable_b) continue;
				if(c == 'c' && !enable_c) continue;
				if(c >= 'a' && c <= 'c') break;
			}
			screen_load();

			if(c == 'a') quest[QUEST_KORI].flags |= QUEST_FLAG_TEAM_A;
			else if(c == 'b') quest[QUEST_KORI].flags |= QUEST_FLAG_TEAM_B;

		}

		//v1.1.91 �R���N�G�X�g1
		//�R���N�G�ɓ���Ƃ��A�ǂ���̐��͂ɂ����I�����ăt���O��p_ptr->animal_ghost_align_flag�ɋL�^����
		else if (cave[py][px].special == QUEST_YAKUZA_1)
			{
				int i;
				char c;
				u32b selectable_flags = 0L;

				//�I���\�Ȑ��͂��t���O�Őݒ�
				//�v���e�E�X�����O�Ƃ��ŕϐg���̂Ƃ����l������H

				//������ɜ߈˂���Ă���ꍇ�ŗD��ł��̏����ɌŒ�
				if (p_ptr->muta4 & MUT4_GHOST_WOLF)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA);
				}
				else if (p_ptr->muta4 & MUT4_GHOST_EAGLE)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU);
				}
				else if (p_ptr->muta4 & MUT4_GHOST_OTTER)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KIKETSU);
				}
				else if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_HANIWA);
				}
			//���S
				else if (p_ptr->pclass == CLASS_SAKI || p_ptr->pclass == CLASS_ENOKO)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA);
				}
			//����d
				else if (p_ptr->pclass == CLASS_YACHIE || p_ptr->pclass == CLASS_BITEN)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KIKETSU);
				}
			//�ޖ�
				else if (p_ptr->pclass == CLASS_YUMA || p_ptr->pclass == CLASS_CHIYARI)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU);
				}
			//���ւ��ݕP
				else if (p_ptr->prace == RACE_HANIWA || p_ptr->pclass == CLASS_KEIKI)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_HANIWA);
				}
			//�ق��푰������͏������͂Ō��܂�B������(���邢�͍����������邩������Ȃ����̕s���̏W�c)�͏��ֈȊO�S�đI���\
				else if (p_ptr->prace == RACE_ANIMAL_GHOST)
				{
					switch (CHECK_ANIMAL_GHOST_STRIFE)
					{
					case ANIMAL_GHOST_STRIFE_KEIGA:
						selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA);
						break;

					case ANIMAL_GHOST_STRIFE_KIKETSU:
						selectable_flags = (ANIMAL_GHOST_ALIGN_KIKETSU);
						break;

					case ANIMAL_GHOST_STRIFE_GOUYOKU:
						selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU);
						break;
					default:
						selectable_flags = (ANIMAL_GHOST_ALIGN_KEIGA | ANIMAL_GHOST_ALIGN_KIKETSU | ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
						break;
					}
				}
			//�_�ގq�ƃt�����͍��~���S��
				else if (p_ptr->pclass == CLASS_KANAKO || p_ptr->pclass == CLASS_FLAN)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}
			//���Ƃ܂��́H���ւ��S�ŁH
			//�얲�E�f�P�E�v�̉́E�푰���_�͑S�ŃI�����[
				//v2.0.17 �c�����S�ŃI�����[
				else if (p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_EIKI || p_ptr->pclass == CLASS_KUTAKA || p_ptr->prace == RACE_DEATH || p_ptr->pclass == CLASS_ZANMU)
				{
					selectable_flags = (ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}
				else //��ɋ����������ɓ��Ă͂܂�Ȃ��Ƃ��S�đI���\
				{
					selectable_flags |= (ANIMAL_GHOST_ALIGN_KEIGA | ANIMAL_GHOST_ALIGN_KIKETSU | ANIMAL_GHOST_ALIGN_GOUYOKU | ANIMAL_GHOST_ALIGN_HANIWA | ANIMAL_GHOST_ALIGN_KILLTHEMALL);
				}

				//v2.0.13 ���N�U�N�G�X�g2�œ|�����܋���̑�����g�D�ɂ͖����ł��Ȃ��B���������ł���g�D�����Ȃ��Ȃ�����S���|���̑I���������o�Ȃ�
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_KEIGA) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_KEIGA);
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_KIKETSU) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_KIKETSU);
				if (p_ptr->animal_ghost_align_flag & ANIMAL_GHOST_Q2_BEAT_GOUYOKU) selectable_flags &= ~(ANIMAL_GHOST_ALIGN_GOUYOKU);
				if (!selectable_flags) selectable_flags = ANIMAL_GHOST_ALIGN_KILLTHEMALL;

				screen_save();
				for (i = 1; i<9; i++)Term_erase(12, i, 255);

				put_str("�ǂ̐��͂ɂ��܂����H", 1, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KEIGA) put_str("a) ����g", 3, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KIKETSU) put_str("b) �S���g", 4, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_GOUYOKU) put_str("c) ���~����", 5, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_HANIWA) put_str("d) ���֕��c", 6, 40);
				if (selectable_flags & ANIMAL_GHOST_ALIGN_KILLTHEMALL) put_str("e) �S���|��", 7, 40);

				while (1)
				{
					c = inkey();
					//v1.1.92 �ԈႦ��&&�łȂ��ł��̂ŏC��
					if (c < 'a' || c > 'e') continue;

					if (c == 'a' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KEIGA)) continue;
					if (c == 'b' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KIKETSU)) continue;
					if (c == 'c' && !(selectable_flags & ANIMAL_GHOST_ALIGN_GOUYOKU)) continue;
					if (c == 'd' && !(selectable_flags & ANIMAL_GHOST_ALIGN_HANIWA)) continue;
					if (c == 'e' && !(selectable_flags & ANIMAL_GHOST_ALIGN_KILLTHEMALL)) continue;

					break;
				}
				screen_load();

				//�t���O��p_ptr->animal_ghost_align_flag�ɋL�^
				if (c == 'a')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_KEIGA;
				else if (c == 'b')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_KIKETSU;
				else if (c == 'c')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_GOUYOKU;
				else if (c == 'd')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_HANIWA;
				else if (c == 'e')
					p_ptr->animal_ghost_align_flag |= ANIMAL_GHOST_ALIGN_KILLTHEMALL;
				else
					msg_print("ERROR:�s���ȑI�����s��ꂽ");

			}


		//v1.1.24 �N�G�X�g�u�}������v�̊J�n���^�[�����L�^
		if(QRKDR)
		{
			qrkdr_rec_turn = turn;

		}

	}
}


/*
 * Do building commands
 */
/*:::�X�łȂ������ł̏����Ȃ�*/
void do_cmd_bldg(void)
{
	int             i, which;
	char            command;
	bool            validcmd;
	building_type   *bldg;


	energy_use = 100;
#if 0
	//����ꎞ���� ���Z��Ńe���|�[�g���ăt���[�Y�����Z�[�u�f�[�^�~�o�p
	if (p_ptr->inside_battle)
	{
		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		p_ptr->leaving = TRUE;
		p_ptr->inside_battle = FALSE;

		/* Re-enter the monster arena */
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
#endif

	if (!cave_have_flag_bold(py, px, FF_BLDG))
	{
		msg_print("ERROR:�����ł͂Ȃ��ꏊ��do_cmd_bldg()���Ă΂�Ă���");

		return;
	}

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
	{
		msg_print("�����ɓ���Ȃ��B");
		return ;
	}

	if(SUPER_SHION)
	{
		msg_print("�������ɓ��������ς��B");
		return;
	}


	if(world_player || SAKUYA_WORLD)
	{
		msg_print("�������Ă��Ӗ����Ȃ��B");
		return ;
	}

	which = f_info[cave[py][px].feat].subtype;

	bldg = &building[which];

	//���͖��������ƌ����ɓ���Ȃ�(���l�̔����̂݉�)
	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > (HINA_YAKU_LIMIT2+5000) && (p_ptr->town_num != TOWN_HITOZATO || which != 9))
	{
		msg_print("���낤�Ƃ������ǂ������Ă��܂����c");
		return ;
	}



	//v1.1.85 �Ǔے�
	if (p_ptr->town_num == TOWN_HITOZATO && which == 12)
	{
		if (is_daytime()) //�����͕X
		{
			if(p_ptr->pclass == CLASS_MIYOI)
				msg_print("�܂��J�X�O���B");
			else
				msg_print("�u���߂�Ȃ����B�܂��J�X�O�Ȃ�ł���[�B�v");
			return;
		}
		else if (is_midnight() && p_ptr->pclass != CLASS_MIYOI) //�[��͗d����p
		{
			if (player_looks_human_side())
			{
				if (one_in_(2))msg_print("�X�͕܂��Ă���B������͓��₩�ȋC�z���`����Ă���̂���...");
				else msg_print("�X�͕܂��Ă���B�ŔɁu�\��v�Ə����ꂽ���ȎD���|�����Ă���...?");
				return;
			}
		}
		else //�[��ȊO�̖�Ԃ͐l�Ԑ�p
		{
			if (!player_looks_human_side() && p_ptr->pclass != CLASS_MIYOI)
			{
				if (one_in_(2))msg_print("�����u������I�M���͂܂��ʖڂł���I�v");
				else msg_print("�l�Ԃ������k�΂��Ă���B�����ē���̂͂�߂Ă������B");
				return;
			}
		}
	}





	/* Don't re-init the wilderness */
	reinit_wilderness = FALSE;
	///sys store �A���[�i��B:2�ŋL�q����Ă��邱��
#if 0 
	//�{�ƃA���[�i�Ŕs�k��ɃA���[�i���p�s�ɂȂ鏈��
	if ((which == 2) && (p_ptr->arena_number < 0))
	{
#ifdef JP
		msg_print("�u�s�҂ɗp�͂Ȃ��B�v");
#else
		msg_print("'There's no place here for a LOSER like you!'");
#endif
		return;
	}
	//�{�ƃA���[�i�Ń����X�^�[��|���܂ŃA���[�i����o���Ȃ��Ȃ鏈��
	else if ((which == 2) && p_ptr->inside_arena)
	{
		if (!p_ptr->exit_bldg && m_cnt > 0)
		{
#ifdef JP
			prt("�Q�[�g�͕܂��Ă���B�����X�^�[�����Ȃ���҂��Ă���I", 0, 0);
#else
			prt("The gates are closed.  The monster awaits!", 0, 0);
#endif
		}
		/*:::�A���[�i�Ń����X�^�[�ɏ����ďo���ɓ��B�����Ƃ�*/
		else
		{
			/* Don't save the arena as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

			p_ptr->inside_arena = FALSE;
			p_ptr->leaving = TRUE;

			/* Re-enter the arena */
			/*:::�A���[�i�̌�����ʂ֖߂�*/
			command_new = SPECIAL_KEY_BUILDING;

			/* No energy needed to re-enter the arena */
			energy_use = 0;
		}

		return;
	}
	else 
#endif

	//v1.1.51�@�V�A���[�i�Ń����X�^�[���œ|�ł��o����悤�ɂ���
	if ((which == 2) && p_ptr->inside_arena)
	{
		if (!p_ptr->exit_bldg && m_cnt > 0)
		{
			if (!get_check_strict("�ڂ��o�܂��܂����H", CHECK_OKAY_CANCEL)) 	return;
		}

		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		p_ptr->inside_arena = FALSE;
		p_ptr->leaving = TRUE;
		//v1.1.52 �o��Ƃ����t���O���Z�b�g����B�����Ŝ߈ː���E���Ă����̂͐F�X�܂���
		reset_tim_flags();

		//v1.1.54
		if (p_ptr->food < PY_FOOD_ALERT) p_ptr->food = PY_FOOD_ALERT - 1;

		//v1.1.55 �A���[�i����o��Ƃ��A�A���[�i�ɓ������Ƃ���HP�ɖ߂�
		p_ptr->chp = MIN(nightmare_record_hp, p_ptr->mhp);

		if (p_ptr->chp < 0) p_ptr->chp = 0; //paranoia

		/* Re-enter the arena */
		/*:::�A���[�i�̌�����ʂ֖߂�*/
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
	/*:::���Z��Ō������������炱���ɗ���*/
	else if (p_ptr->inside_battle)
	{
		/* Don't save the arena as saved floor */
		prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

		p_ptr->leaving = TRUE;
		p_ptr->inside_battle = FALSE;

		/* Re-enter the monster arena */
		command_new = SPECIAL_KEY_BUILDING;

		/* No energy needed to re-enter the arena */
		energy_use = 0;

		return;
	}
	/*:::��������o��Ƃ��̂��߂Ɍ��́��̈ʒu��ۑ����Ă���*/
	else
	{
		p_ptr->oldpy = py;
		p_ptr->oldpx = px;
	}

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();

	/* Hack -- Increase "icky" depth */
	character_icky++;

	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	show_building(bldg);
	leave_bldg = FALSE;

	//v1.1.59
	if (!EXTRA_MODE || play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_BUILD_EX))
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_BUILD);

	/*:::��������o��܂Ń��[�v*/
	while (!leave_bldg)
	{
		validcmd = FALSE;
		prt("", 1, 0);

		building_prt_gold();

		command = inkey();

		if (command == ESCAPE)
		{
			leave_bldg = TRUE;
			p_ptr->inside_arena = FALSE;
			p_ptr->inside_battle = FALSE;
			break;
		}

		for (i = 0; i < 8; i++)
		{
			if (bldg->letters[i])
			{
				if (bldg->letters[i] == command)
				{
					validcmd = TRUE;
					break;
				}
			}
		}

		if (validcmd)
			bldg_process_command(bldg, i);


		//EX���[�h�̌������œ���̍s�����Ƃ����������I�Ɍ�������o�����Bprocess_command()��ɓG����
		if(hack_ex_bldg_summon_idx || hack_ex_bldg_summon_type ) leave_bldg = TRUE;

		if(p_ptr->inside_battle && p_ptr->leaving) leave_bldg = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/*:::�O���l���ޗp*/
		if(p_ptr->is_dead) break;
	}

	select_floor_music(); //v1.1.58


	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	/*:::�N�G�X�g��V�����ȂǁA�X���ĕ`�悷�鏈���炵��*/
	if (reinit_wilderness)
	{
		p_ptr->leaving = TRUE;
	}

	/* Hack -- Decrease "icky" depth */
	character_icky--;

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	p_ptr->update |= (PU_VIEW | PU_MONSTERS | PU_BONUS | PU_LITE | PU_MON_LITE);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/* Array of places to find an inscription */
static cptr find_quest[] =
{
#ifdef JP
"���Ƀ��b�Z�[�W�����܂�Ă���:",
#else
	"You find the following inscription in the floor",
#endif

#ifdef JP
"�ǂɃ��b�Z�[�W�����܂�Ă���:",
#else
	"You see a message inscribed in the wall",
#endif

#ifdef JP
"���b�Z�[�W��������:",
#else
	"There is a sign saying",
#endif

#ifdef JP
"�������K�i�̏�ɏ����Ă���:",
#else
	"Something is written on the staircase",
#endif

#ifdef JP
"�������������B���b�Z�[�W�������Ă���:",
#else
	"You find a scroll with the following message",
#endif

};


/*
 * Discover quest
 */
/*:::�����N�G�K�ɍ~��Ă����Ƃ��̃��b�Z�[�W*/
void quest_discovery(int q_idx)
{
	quest_type      *q_ptr = &quest[q_idx];
	monster_race    *r_ptr = &r_info[q_ptr->r_idx];
	int             q_num = q_ptr->max_num;
	char            name[80];

	/* No quest index */
	if (!q_idx) return;

	///mod160223 Extra�p�N�G�X�g���ꏈ��
	if(EXTRA_MODE)
	{
//		msg_print("�G�L�X�g���N�G�X�g���B���Ȃ��͐���Ă������������Ă������B");
		return;
	}


	strcpy(name, (r_name + r_ptr->name));
	msg_print(find_quest[rand_range(0, 4)]);
	msg_print(NULL);

	if (q_num == 1)
	{
		/* Unique */

		/* Hack -- "unique" monsters must be "unique" */
		//v1.1.92 place_quest_monsters()�̎d�l�ύX�ɂ�肱���ɂ͗��Ȃ��Ȃ�͂�
		if ((r_ptr->flags1 & RF1_UNIQUE) &&
		    (0 == r_ptr->max_num))
		{
#ifdef JP
			msg_print("���̊K�͈ȑO�͒N���ɂ���Ď���Ă����悤���c�B");
#else
			msg_print("It seems that this level was protected by someone before...");
#endif
			/* The unique is already dead */
			quest[q_idx].status = QUEST_STATUS_FINISHED;
			if(p_ptr->pclass == CLASS_REIMU) gain_osaisen(QUEST_OSAISEN(quest[q_idx].level));
			set_deity_bias(DBIAS_REPUTATION, 2);
			set_deity_bias(DBIAS_COSMOS, 1);
		}
		else
		{
#ifdef JP
			msg_format("���ӂ���I���̊K��%s�ɂ���Ď���Ă���I", name);
#else
			msg_format("Beware, this level is protected by %s!", name);
#endif
		}
	}
	else
	{
		/* Normal monsters */
#ifdef JP
msg_format("���ӂ���I���̊K��%d�̂�%s�ɂ���Ď���Ă���I", q_num, name);
#else
		plural_aux(name);
		msg_format("Be warned, this level is guarded by %d %s!", q_num, name);
#endif

	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
/*:::���݃N�G�X�g�_���W�����ɋ��邩���݂̊K�ŃN�G�X�g���s���Ă���Ƃ��N�G�X�gID��Ԃ��B*/
///quest
int quest_number(int level)
{
	int i;

	/* Check quests */
	if (p_ptr->inside_quest)
		return (p_ptr->inside_quest);

	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status != QUEST_STATUS_TAKEN) continue;

		if ((quest[i].type == QUEST_TYPE_KILL_LEVEL) &&
			!(quest[i].flags & QUEST_FLAG_PRESET) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == dungeon_type))
			return (i);
	}

	/* Check for random quest */
	return (random_quest_number(level));
}


/*
 * Return the index of the random quest on this level
 * (or zero)
 */
///quest �����N�G
///mod160223 �������ǉ�
int random_quest_number(int level)
{
	int i;

	if (dungeon_type != DUNGEON_ANGBAND) return 0;

	for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
	{
		if ((quest[i].type == QUEST_TYPE_RANDOM || EXTRA_MODE) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == DUNGEON_ANGBAND))
		{
			return i;
		}
	}

	/* Nope */
	return 0;
}

