/* File: scores.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Highscores handling */

#include "angband.h"
	/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN
   #define _MBCS
#endif
/*
 * Seek score 'i' in the highscore file
 */
static int highscore_seek(int i)
{
	/* Seek for the requested record */
	return (fd_seek(highscore_fd, (huge)(i) * sizeof(high_score)));
}


/*
 * Read one score from the highscore file
 */
static errr highscore_read(high_score *score)
{
	/* Read the record, note failure */
	return (fd_read(highscore_fd, (char*)(score), sizeof(high_score)));
}


/*
 * Write one score to the highscore file
 */
static int highscore_write(high_score *score)
{
	/* Write the record, note failure */
	return (fd_write(highscore_fd, (char*)(score), sizeof(high_score)));
}


/*
 * Just determine where a new score *would* be placed
 * Return the location (0 is best) or -1 on failure
 */
static int highscore_where(high_score *score)
{
	int			i;

	high_score		the_score;
	int my_score;

	my_score = atoi(score->pts);

	/* Paranoia -- it may not have opened */
	if (highscore_fd < 0) return (-1);

	/* Go to the start of the highscore file */
	if (highscore_seek(0)) return (-1);

	/* Read until we get to a higher score */
	for (i = 0; i < MAX_HISCORES; i++)
	{
		int old_score;
		if (highscore_read(&the_score)) return (i);
		old_score = atoi(the_score.pts);
/*		if (strcmp(the_score.pts, score->pts) < 0) return (i); */
		if (my_score > old_score) return (i);
	}

	/* The "last" entry is always usable */
	return (MAX_HISCORES - 1);
}


/*
 * Actually place an entry into the high score file
 * Return the location (0 is best) or -1 on "failure"
 */
/*:::�n�C�X�R�A�t�@�C����high_score�^�̒l���L�^*/
/*:::�n�C�X�R�A�t�@�C���iscores.raw�j�����ɊJ����Ă��邱��*/
static int highscore_add(high_score *score)
{
	int			i, slot;
	bool		done = FALSE;

	high_score		the_score, tmpscore;


	/* Paranoia -- it may not have opened */
	if (highscore_fd < 0) return (-1);

	/* Determine where the score should go */
	slot = highscore_where(score);

	/* Hack -- Not on the list */
	if (slot < 0) return (-1);

	/* Hack -- prepare to dump the new score */
	the_score = (*score);

	/* Slide all the scores down one */
	for (i = slot; !done && (i < MAX_HISCORES); i++)
	{
		/* Read the old guy, note errors */
		if (highscore_seek(i)) return (-1);
		if (highscore_read(&tmpscore)) done = TRUE;

		/* Back up and dump the score we were holding */
		if (highscore_seek(i)) return (-1);
		if (highscore_write(&the_score)) return (-1);

		/* Hack -- Save the old score, for the next pass */
		the_score = tmpscore;
	}

	/* Return location used */
	return (slot);
}



/*
 * Display the scores in a given range.
 * Assumes the high score list is already open.
 * Only five entries per line, too much info.
 *
 * Mega-Hack -- allow "fake" entry at the given position.
 */
/*:::�����ڂ��Ă邠����̃X�R�A�t�@�C����\������*/
///system
void display_scores_aux(int from, int to, int note, high_score *score)
{
	int		i, j, k, n, place;
	byte attr;

	high_score	the_score;

	char	out_val[256];
	char	tmp_val[160];

	int wid, hgt, per_screen;

	Term_get_size(&wid, &hgt);
	per_screen = (hgt - 4) / 4;

	/* Paranoia -- it may not have opened */
	if (highscore_fd < 0) return;


	/* Assume we will show the first 10 */
	if (from < 0) from = 0;
	if (to < 0) to = 10;
	if (to > MAX_HISCORES) to = MAX_HISCORES;


	/* Seek to the beginning */
	if (highscore_seek(0)) return;

	/* Hack -- Count the high scores */
	for (i = 0; i < MAX_HISCORES; i++)
	{
		if (highscore_read(&the_score)) break;
	}

	/* Hack -- allow "fake" entry to be last */
	if ((note == i) && score) i++;

	/* Forget about the last entries */
	if (i > to) i = to;


	/* Show per_screen per page, until "done" */
	for (k = from, place = k+1; k < i; k += per_screen)
	{
		/* Clear screen */
		Term_clear();

		/* Title */
#ifdef JP
		///msg131221
		put_str("      ���z�ؓ{: �v���C���[�X�R�A", 0, 0);
#else
		put_str("                Hengband Hall of Fame", 0, 0);
#endif


		/* Indicate non-top scores */
		if (k > 0)
		{
#ifdef JP
sprintf(tmp_val, "( %d �ʈȉ� )", k + 1);
#else
			sprintf(tmp_val, "(from position %d)", k + 1);
#endif

			put_str(tmp_val, 0, 40);
		}

		/* Dump per_screen entries */
		for (j = k, n = 0; j < i && n < per_screen; place++, j++, n++)
		{
			int pr, pc, pa, clev, mlev, cdun, mdun;
			char ps;
			const player_seikaku *tmp_ap_ptr;

			cptr user, gold, when, aged;


			/* Hack -- indicate death in yellow */
			attr = (j == note) ? TERM_YELLOW : TERM_WHITE;


			/* Mega-Hack -- insert a "fake" record */
			if ((note == j) && score)
			{
				the_score = (*score);
				attr = TERM_L_GREEN;
				score = NULL;
				note = -1;
				j--;
			}

			/* Read a normal record */
			else
			{
				/* Read the proper record */
				if (highscore_seek(j)) break;
				if (highscore_read(&the_score)) break;
			}

			/* Extract the race/class */
			pr = atoi(the_score.p_r);
			pc = atoi(the_score.p_c);
			pa = atoi(the_score.p_a);
			ps = the_score.sex[0];

			/*:::151019�C��
			 *�X�R�A�����L���O�\�Ńo�O�����B�N���X�ԍ���100(��������)����A�����i���u���܂�Â悢(10)�v���u��������(11)�v�̂Ƃ��A
			 *high_score::p_c[3]�ɃN���X�ԍ����i�[���錅�����肸��\0���������܂ꂸ�A�ϐ�pc�̎O�������i�ԍ��̓񌅂Ƃ������Č܌��ɂȂ��Ă��܂��N���X���Q�Ǝ��ɃG���[�ɂȂ�B
			 *(���i����L�ȊO�̎��́A�N���X�ԍ�����\0���������܂�Ȃ���pc��pa�̊Ԃɋ󔒂�����̂ł�����atoi()�̏��Ő���Ȑ����ɂȂ�)
			 *�Ƃ肠����pc��4���ȏ�ɂȂ��Ă�Ζ�������񌅐؂藎�Ƃ��悤�ɂ���Ύ��͍ςށB
			 *�������{�I�ȏC���͉\�Ȃ̂��H�Ⴆ�΂��̐��l�̊Y������3���ɂȂ�悤�ɏ������狌scores.raw�t�@�C���ƌ݊���������̂��H
			 *���Ȃ莩�M���Ȃ��̂łƂ肠�������u�B�܂����푰�␫�i�̎�ނ�100�𒴂�����͂��Ȃ����낤���B
 			 */
			//test msg_format("pr:%d(%s) pc:%d(%s) pa:%d(%s)",pr,the_score.p_r,pc,the_score.p_c,pa,the_score.p_a);
			if(pc > 999) pc = pc / 100;

			if(pc >= MAX_CLASS || pc < 0)
			{
				msg_format("display_scores_aux()��player_class�l����������(%d)",pc);
				return;
			}
			if(pr >= MAX_RACES || pr < 0)
			{
				msg_format("display_scores_aux()��player_race�l����������(%d)",pr);
				return;
			}
			if(pa > 255 || pa < 0)
			{
				msg_format("display_scores_aux()��player_seikaku�l����������(%d)",pa);
				return;
			}

			/* Extract the level info */
			clev = atoi(the_score.cur_lev);
			mlev = atoi(the_score.max_lev);
			cdun = atoi(the_score.cur_dun);
			mdun = atoi(the_score.max_dun);

			/* Hack -- extract the gold and such */
	/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN			
			for (user = the_score.uid; iswspace(*user); user++) /* loop */;
			for (when = the_score.day; iswspace(*when); when++) /* loop */;
			for (gold = the_score.gold; iswspace(*gold); gold++) /* loop */;
			for (aged = the_score.turns; iswspace(*aged); aged++) /* loop */;
#else

			for (user = the_score.uid; isspace(*user); user++) /* loop */;
			for (when = the_score.day; isspace(*when); when++) /* loop */;
			for (gold = the_score.gold; isspace(*gold); gold++) /* loop */;
			for (aged = the_score.turns; isspace(*aged); aged++) /* loop */;
#endif

			/* Clean up standard encoded form of "when" */
			if ((*when == '@') && strlen(when) == 9)
			{
				sprintf(tmp_val, "%.4s-%.2s-%.2s",
					when + 1, when + 5, when + 7);
				when = tmp_val;
			}

			/* Dump some info */
#ifdef JP
/*sprintf(out_val, "%3d.%9s  %s%s%s�Ƃ�������%s��%s (���x�� %d)", */

			tmp_ap_ptr = get_ap_ptr(pr,pc,pa);

			if(ps == 'm')
			{
				sprintf(out_val, "%3d.%9s  %s%s%s - %s%s (���x�� %d)",
				place, the_score.pts,
				//seikaku_info[pa].title, (seikaku_info[pa].no ? "��" : ""),
				tmp_ap_ptr->title, (tmp_ap_ptr->no ? "��" : ""),
				the_score.who,
				race_info[pr].title, class_info[pc].title,
				clev);
			}
			else
			{

				sprintf(out_val, "%3d.%9s  %s%s%s - %s%s (���x�� %d)",
				place, the_score.pts,
				//seikaku_info[pa].f_title, (seikaku_info[pa].no ? "��" : ""),
				tmp_ap_ptr->f_title, (tmp_ap_ptr->no ? "��" : ""),
				the_score.who,
				race_info[pr].title, class_info[pc].f_title,
				clev);
			}

#else
			sprintf(out_val, "%3d.%9s  %s %s the %s %s, Level %d",
				place, the_score.pts,
				seikaku_info[pa].title,
				the_score.who, race_info[pr].title, class_info[pc].title,
				clev);
#endif


			/* Append a "maximum level" */
#ifdef JP
if (mlev > clev) strcat(out_val, format(" (�ō�%d)", mlev));
#else
			if (mlev > clev) strcat(out_val, format(" (Max %d)", mlev));
#endif


			/* Dump the first line */
			c_put_str(attr, out_val, n*4 + 2, 0);

			/* Another line of info */
#ifdef JP
			if (mdun != 0)
				sprintf(out_val, "    �ō�%3d�K", mdun);
			else
				sprintf(out_val, "             ");


			/* ���S�������I���W�i�����ׂ����\�� */
			if (streq(the_score.how, "yet"))
			{
				///msg131221
				sprintf(out_val+13, "  �Q�[���v���C�� (%d%s)",
				       cdun, "�K");
			}
			else if (streq(the_score.how, "ripe") && pc == CLASS_OUTSIDER)
			{
				sprintf(out_val+13, "  ���z���ɉi�Z (%d%s)",
					cdun, "�K");
			}
			else if (streq(the_score.how, "ripe") || streq(the_score.how, "true_loser"))
			{
				sprintf(out_val+13, "  �Q�[���N���A (%d%s)",
					cdun, "�K");
			}
			else if (streq(the_score.how, "lost"))
			{
				sprintf(out_val+13, "  ����");
			}
			else if (streq(the_score.how, "hourai"))
			{
				sprintf(out_val+13, "  �i���̖��𓾂�");
			}
			else if (streq(the_score.how, "mission_failed"))
			{
				sprintf(out_val+13, "  �P�̌�q�Ɏ��s (%d%s)",
					cdun, "�K");
			}
			else if (streq(the_score.how, "Seppuku"))
			{
				sprintf(out_val+13, "  �����̌�ɐؕ� (%d%s)",
					cdun, "�K");
			}
			else if (streq(the_score.how, "escape"))
			{
				sprintf(out_val+13, "  ���z������E�o (%d%s)",
					cdun, "�K");
			}
			else if (streq(the_score.how, "triumph"))
			{
				sprintf(out_val+13, "  ���z���̉p�Y�Ƃ��ĊO�E�֋A�� (%d%s)",
					cdun, "�K");
			}

			else
			{
				codeconv(the_score.how);

				/* Some people die outside of the dungeon */
				if (!cdun)
					sprintf(out_val+13, "  �n���%s�ɓ|���ꂽ", the_score.how);
				else
					sprintf(out_val+13, "  %d�K��%s�ɓ|���ꂽ",
						cdun, the_score.how);
			}

#else
			/* Some people die outside of the dungeon */
			if (!cdun)
				sprintf(out_val, 
					"               Killed by %s on the surface",
					the_score.how);
			else
				sprintf(out_val, 
					"               Killed by %s on %s %d",
					the_score.how, "Dungeon Level", cdun);

			/* Append a "maximum level" */
			if (mdun > cdun) strcat(out_val, format(" (Max %d)", mdun));
#endif

			/* Dump the info */
			c_put_str(attr, out_val, n*4 + 3, 0);

			/* And still another line of info */
#ifdef JP
			{
				char buf[11];

				/* ���t�� 19yy/mm/dd �̌`���ɕύX���� */
				if (strlen(when) == 8 && when[2] == '/' && when[5] == '/') {
					sprintf(buf, "%d%s/%.5s", 19 + (when[6] < '8'), when + 6, when);
					when = buf;
				}
				sprintf(out_val,
						"        (���[�U�[:%s, ���t:%s, ������:%s, �^�[��:%s)",
						user, when, gold, aged);
			}

#else
			sprintf(out_val,
				"               (User %s, Date %s, Gold %s, Turn %s).",
				user, when, gold, aged);
#endif

			c_put_str(attr, out_val, n*4 + 4, 0);
		}


		/* Wait for response */
#ifdef JP
prt("[ ESC�Œ��f, ���̑��̃L�[�ő����܂� ]", hgt - 1, 21);
#else
		prt("[Press ESC to quit, any other key to continue.]", hgt - 1, 17);
#endif

		j = inkey();
		prt("", hgt - 1, 0);

		/* Hack -- notice Escape */
		if (j == ESCAPE) break;
	}
}


/*
 * Hack -- Display the scores in a given range and quit.
 *
 * This function is only called from "main.c" when the user asks
 * to see the "high scores".
 */
void display_scores(int from, int to)
{
	char buf[1024];

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	/* Open the binary high score file, for reading */
	highscore_fd = fd_open(buf, O_RDONLY);

	/* Paranoia -- No score file */
#ifdef JP
if (highscore_fd < 0) quit("�X�R�A�E�t�@�C�����g�p�ł��܂���B");
#else
	if (highscore_fd < 0) quit("Score file unavailable.");
#endif


	/* Clear screen */
	Term_clear();

	/* Display the scores */
	display_scores_aux(from, to, -1, NULL);

	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;

	/* Quit */
	quit(NULL);
}

/*:::�X�R�A�̑��M*/

bool send_world_score(bool do_send)
{
#ifdef WORLD_SCORE
	if(send_score && do_send)
	{
/*
		if(easy_band)
		{
#ifdef JP
			msg_print("���S�҃��[�h�ł̓��[���h�X�R�A�ɓo�^�ł��܂���B");
#else
			msg_print("Since you are in the Easy Mode, you cannot send score to world score server.");
#endif
		}

		else 
*/
#ifdef JP
//		if(get_check_strict("�X�R�A���X�R�A�E�T�[�o�ɓo�^���܂���? ", (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
		if(get_check_strict("�Q�[���f�[�^�����z�ؓ{�X�R�A�E�T�[�o�ɑ��M���܂���? ", (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
#else
		else if(get_check_strict("Do you send score to the world score sever? ", (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
#endif
		{
			errr err;

			//v1.1.25 �X�R�A�T�[�o�ɑ��M����v���C���[����ݒ肷��
			get_score_server_name();


			prt("",0,0);
#ifdef JP
			prt("���M���D�D",0,0);
#else
			prt("Sending...",0,0);
#endif
			Term_fresh();
			screen_save();
			//v1.1.25 �X�R�A���M����
			err = report_score();
			screen_load();
			if (err)
			{
				return FALSE;
			}
#ifdef JP
			prt("�����B�����L�[�������Ă��������B", 0, 0);
#else
			prt("Completed.  Hit any key.", 0, 0);
#endif
			(void)inkey();
		}
		else return FALSE;
	}
#endif
	return TRUE;
}

/*
 * Enters a players name on a hi-score table, if "legal", and in any
 * case, displays some relevant portion of the high score list.
 *
 * Assumes "signals_ignore_tstp()" has been called.
 */
/*:::���̏����n�C�X�R�A���X�g�ɒǉ�����*/
errr top_twenty(void)
{
	int          j;

	high_score   the_score;

	time_t ct = time((time_t*)0);

	errr err;

	u32b points;

	/* Clear the record */
	(void)WIPE(&the_score, high_score);

	/* Save the version */
	///\sys131117 FAKE_VER����H_VER��
	sprintf(the_score.what, "%u.%u.%u",
		H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH);

	/* Calculate and save the points */
	//sprintf(the_score.pts, "%9ld", (long)total_points());
	points = total_points_new(FALSE);
	if (points > 999999999) points = 999999999;
	sprintf(the_score.pts, "%9lu", points);
	the_score.pts[9] = '\0';

	/* Save the current gold */
	sprintf(the_score.gold, "%9lu", (long)p_ptr->au);
	the_score.gold[9] = '\0';

	/* Save the current turn */
	sprintf(the_score.turns, "%9lu", (long)turn_real(turn));
	the_score.turns[9] = '\0';

#ifdef HIGHSCORE_DATE_HACK
	/* Save the date in a hacked up form (9 chars) */
	(void)sprintf(the_score.day, "%-.6s %-.2s", ctime(&ct) + 4, ctime(&ct) + 22);
#else
	/* Save the date in standard form (8 chars) */
/*	(void)strftime(the_score.day, 9, "%m/%d/%y", localtime(&ct)); */
	/* Save the date in standard encoded form (9 chars) */
	strftime(the_score.day, 10, "@%Y%m%d", localtime(&ct));
#endif

	/* Save the player name (15 chars) */
	sprintf(the_score.who, "%-.15s", player_name);

	/* Save the player info XXX XXX XXX */
	sprintf(the_score.uid, "%7u", player_uid);
	sprintf(the_score.sex, "%c", (p_ptr->psex ? 'm' : 'f'));
	sprintf(the_score.p_r, "%2d", p_ptr->prace);
	sprintf(the_score.p_c, "%2d", p_ptr->pclass);
	sprintf(the_score.p_a, "%2d", p_ptr->pseikaku);

	/* Save the level and such */
	sprintf(the_score.cur_lev, "%3d", p_ptr->lev);
	sprintf(the_score.cur_dun, "%3d", dun_level);
	sprintf(the_score.max_lev, "%3d", p_ptr->max_plv);
	sprintf(the_score.max_dun, "%3d", max_dlv[dungeon_type]);

	/* Save the cause of death (31 chars) */
	if (strlen(p_ptr->died_from) >= sizeof(the_score.how))
	{
#ifdef JP
		my_strcpy(the_score.how, p_ptr->died_from, sizeof(the_score.how) - 2);
		strcat(the_score.how, "�c");
#else
		my_strcpy(the_score.how, p_ptr->died_from, sizeof(the_score.how) - 3);
		strcat(the_score.how, "...");
#endif
	}
	else
	{
		strcpy(the_score.how, p_ptr->died_from);
	}

	/* Grab permissions */
	safe_setuid_grab();

	/* Lock (for writing) the highscore file, or fail */
	err = fd_lock(highscore_fd, F_WRLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if (err) return (1);

	/* Add a new entry to the score list, see where it went */
	j = highscore_add(&the_score);

	/* Grab permissions */
	safe_setuid_grab();

	/* Unlock the highscore file, or fail */
	err = fd_lock(highscore_fd, F_UNLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if (err) return (1);


	/* Hack -- Display the top fifteen scores */
	if (j < 10)
	{
		display_scores_aux(0, 15, j, NULL);
	}

	/* Display the scores surrounding the player */
	else
	{
		display_scores_aux(0, 5, j, NULL);
		display_scores_aux(j - 2, j + 7, j, NULL);
	}


	/* Success */
	return (0);
}


/*
 * Predict the players location, and display it.
 */
errr predict_score(void)
{
	int          j;
	u32b score_point;

	high_score   the_score;


	/* No score file */
	if (highscore_fd < 0)
	{
#ifdef JP
msg_print("�X�R�A�E�t�@�C�����g�p�ł��܂���B");
#else
		msg_print("Score file unavailable.");
#endif

		msg_print(NULL);
		return (0);
	}


	/* Save the version */
	///\sys131117 FAKE_VER����H_VER��
	sprintf(the_score.what, "%u.%u.%u",
		H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH);

	/* Calculate and save the points */
	//v1.1.56 ���X�����A�񎀖S/���ނŃQ�[���I�����X�R�A�{�[�h��\�������Ƃ��̃X�R�A�v�Z���Â������̂ŏC��
	//sprintf(the_score.pts, "%9ld", (long)total_points());
	score_point = total_points_new(FALSE);
	if (score_point > 999999999) score_point = 999999999;
	sprintf(the_score.pts, "%9ld", score_point);

	/* Save the current gold */
	sprintf(the_score.gold, "%9lu", (long)p_ptr->au);

	/* Save the current turn */
	sprintf(the_score.turns, "%9lu", (long)turn_real(turn));

	/* Hack -- no time needed */
#ifdef JP
strcpy(the_score.day, "����");
#else
	strcpy(the_score.day, "TODAY");
#endif


	/* Save the player name (15 chars) */
	sprintf(the_score.who, "%-.15s", player_name);

	/* Save the player info XXX XXX XXX */
	sprintf(the_score.uid, "%7u", player_uid);
	sprintf(the_score.sex, "%c", (p_ptr->psex ? 'm' : 'f'));
	sprintf(the_score.p_r, "%2d", p_ptr->prace);
	sprintf(the_score.p_c, "%2d", p_ptr->pclass);
	sprintf(the_score.p_a, "%2d", p_ptr->pseikaku);

	/* Save the level and such */
	sprintf(the_score.cur_lev, "%3d", p_ptr->lev);
	sprintf(the_score.cur_dun, "%3d", dun_level);
	sprintf(the_score.max_lev, "%3d", p_ptr->max_plv);
	sprintf(the_score.max_dun, "%3d", max_dlv[dungeon_type]);

	/* Hack -- no cause of death */
#ifdef JP
	/* �܂�����ł��Ȃ��Ƃ��̎��ʕ��� */
	strcpy(the_score.how, "yet");
#else
	strcpy(the_score.how, "nobody (yet!)");
#endif



	/* See where the entry would be placed */
	j = highscore_where(&the_score);


	/* Hack -- Display the top fifteen scores */
	if (j < 10)
	{
		display_scores_aux(0, 15, j, &the_score);
	}

	/* Display some "useful" scores */
	else
	{
		display_scores_aux(0, 5, -1, NULL);
		display_scores_aux(j - 2, j + 7, j, &the_score);
	}


	/* Success */
	return (0);
}



/*
 * show_highclass - selectively list highscores based on class
 * -KMW-
 */
/*:::�N���X���̃n�C�X�R�A�炵���@�폜�\��*/
///sysdel �N���X���̃n�C�X�R�A
void show_highclass(void)
{

	register int i = 0, j, m = 0;
	int pr, clev/*, al*/;
	high_score the_score;
	char buf[1024], out_val[256];

	screen_save();

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	highscore_fd = fd_open(buf, O_RDONLY);

	if (highscore_fd < 0)
	{
#ifdef JP
msg_print("�X�R�A�E�t�@�C�����g�p�ł��܂���B");
#else
		msg_print("Score file unavailable.");
#endif

		msg_print(NULL);
		return;
	}

	if (highscore_seek(0)) return;

	for (i = 0; i < MAX_HISCORES; i++)
		if (highscore_read(&the_score)) break;

	m = 0;
	j = 0;
	clev = 0;

	while ((m < 9) && (j < MAX_HISCORES))
	{
		if (highscore_seek(j)) break;
		if (highscore_read(&the_score)) break;
		pr = atoi(the_score.p_r);
		clev = atoi(the_score.cur_lev);

#ifdef JP
		sprintf(out_val, "   %3d) %s��%s (���x�� %2d)",
		    (m + 1), race_info[pr].title,the_score.who, clev);
#else
		sprintf(out_val, "%3d) %s the %s (Level %2d)",
		    (m + 1), the_score.who, race_info[pr].title, clev);
#endif

		prt(out_val, (m + 7), 0);
		m++;
		j++;
	}

#ifdef JP
	sprintf(out_val, "���Ȃ�) %s��%s (���x�� %2d)",
	    race_info[p_ptr->prace].title,player_name, p_ptr->lev);
#else
	sprintf(out_val, "You) %s the %s (Level %2d)",
	    player_name, race_info[p_ptr->prace].title, p_ptr->lev);
#endif

	prt(out_val, (m + 8), 0);

	(void)fd_close(highscore_fd);
	highscore_fd = -1;
#ifdef JP
	prt("�����L�[�������ƃQ�[���ɖ߂�܂�",0,0);
#else
	prt("Hit any key to continue",0,0);
#endif

	(void)inkey();

	for (j = 5; j < 18; j++) prt("", j, 0);
	screen_load();
}


/*
 * Race Legends
 * -KMW-
 */
/*:::�@�푰���ƃX�R�A�H�ڍז���*/
///sysdel 
void race_score(int race_num)
{
	register int i = 0, j, m = 0;
	int pr, clev, lastlev;
	high_score the_score;
	char buf[1024], out_val[256], tmp_str[80];

	lastlev = 0;

	/* rr9: TODO - pluralize the race */
#ifdef JP
sprintf(tmp_str,"�ō���%s", race_info[race_num].title);
#else
	sprintf(tmp_str,"The Greatest of all the %s", race_info[race_num].title);
#endif

	prt(tmp_str, 5, 15);

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	highscore_fd = fd_open(buf, O_RDONLY);

	if (highscore_fd < 0)
	{
#ifdef JP
msg_print("�X�R�A�E�t�@�C�����g�p�ł��܂���B");
#else
		msg_print("Score file unavailable.");
#endif

		msg_print(NULL);
		return;
	}

	if (highscore_seek(0)) return;

	for (i = 0; i < MAX_HISCORES; i++)
	{
		if (highscore_read(&the_score)) break;
	}

	m = 0;
	j = 0;

	while ((m < 10) || (j < MAX_HISCORES))
	{
		if (highscore_seek(j)) break;
		if (highscore_read(&the_score)) break;
		pr = atoi(the_score.p_r);
		clev = atoi(the_score.cur_lev);

		if (pr == race_num)
		{
#ifdef JP
		sprintf(out_val, "   %3d) %s��%s (���x�� %2d)",
			    (m + 1), race_info[pr].title, 
				the_score.who,clev);
#else
			sprintf(out_val, "%3d) %s the %s (Level %3d)",
			    (m + 1), the_score.who,
			race_info[pr].title, clev);
#endif

			prt(out_val, (m + 7), 0);
			m++;
			lastlev = clev;
		}
		j++;
	}

	/* add player if qualified */
	if ((p_ptr->prace == race_num) && (p_ptr->lev >= lastlev))
	{
#ifdef JP
	sprintf(out_val, "���Ȃ�) %s��%s (���x�� %2d)",
		     race_info[p_ptr->prace].title,player_name, p_ptr->lev);
#else
		sprintf(out_val, "You) %s the %s (Level %3d)",
		    player_name, race_info[p_ptr->prace].title, p_ptr->lev);
#endif

		prt(out_val, (m + 8), 0);
	}

	(void)fd_close(highscore_fd);
	highscore_fd = -1;
}


/*
 * Race Legends
 * -KMW-
 */
void race_legends(void)
{
	int i, j;

	for (i = 0; i < MAX_RACES; i++)
	{
		race_score(i);
#ifdef JP
msg_print("�����L�[�������ƃQ�[���ɖ߂�܂�");
#else
		msg_print("Hit any key to continue");
#endif

		msg_print(NULL);
		for (j = 5; j < 19; j++)
			prt("", j, 0);
	}
}


/*
 * Change the player into a King!			-RAK-
 */
/*:::�������Ĉ��ނ���Ƃ��@������\��*/
void kingly(void)
{
	int wid, hgt;
	int cx, cy;
	bool seppuku = streq(p_ptr->died_from, "Seppuku");
	bool triumph = streq(p_ptr->died_from, "triumph");
	bool hourai = streq(p_ptr->died_from, "hourai");

	/* Hack -- retire in town */
	dun_level = 0;



	/* Fake death */
	if (!seppuku && !triumph && !hourai)
	{
		//v1.1.48
		if (IS_SHION_TRUE_LOSER)
		{
			(void)strcpy(p_ptr->died_from, "true_loser");
		}
		else
		{
			/* ���ނ����Ƃ��̎��ʕ��� */
			(void)strcpy(p_ptr->died_from, "ripe");
		}
	}

	/* Restore the experience */
	p_ptr->exp = p_ptr->max_exp;

	/* Restore the level */
	p_ptr->lev = p_ptr->max_plv;

	Term_get_size(&wid, &hgt);
	cy = hgt / 2;
	cx = wid / 2;

	/* Hack -- Instant Gold */
	//p_ptr->au += 10000000L;

	/* Clear screen */
	Term_clear();

	/* Display a crown */
	put_str("#", cy - 11, cx - 1);
	put_str("#####", cy - 10, cx - 3);
	put_str("#", cy - 9, cx - 1);
	put_str(",,,  $$$  ,,,", cy - 8, cx - 7);
	put_str(",,=$   \"$$$$$\"   $=,,", cy - 7, cx - 11);
	put_str(",$$        $$$        $$,", cy - 6, cx - 13);
	put_str("*>         <*>         <*", cy - 5, cx - 13);
	put_str("$$         $$$         $$", cy - 4, cx - 13);
	put_str("\"$$        $$$        $$\"", cy - 3, cx - 13);
	put_str("\"$$       $$$       $$\"", cy - 2, cx - 12);
	put_str("*#########*#########*", cy - 1, cx - 11);
	put_str("*#########*#########*", cy, cx - 11);

	/* Display a message */
#ifdef JP
	put_str("Veni, Vidi, Vici!", cy + 3, cx - 9);
	put_str("�����A�����A�������I", cy + 4, cx - 10);

	if(cp_ptr->flag_only_unique)
		put_str(format("���z�̏������΁I"), cy + 5, cx - 11);
	else
		put_str(format("�̑�Ȃ�%s���΁I", sp_ptr->winner), cy + 5, cx - 11);
#else
	put_str("Veni, Vidi, Vici!", cy + 3, cx - 9);
	put_str("I came, I saw, I conquered!", cy + 4, cx - 14);
	put_str(format("All Hail the Mighty %s!", sp_ptr->winner), cy + 5, cx - 13);
#endif

	/* If player did Seppuku, that is already written in playrecord */
	if (!seppuku)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�_���W�����̒T��������ނ����B");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- �Q�[���I�[�o�[ --------");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "retired exploring dungeons.");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");
	}

	/* Flush input */
	flush();

	/* Wait for response */
	pause_line(hgt - 1);
}
