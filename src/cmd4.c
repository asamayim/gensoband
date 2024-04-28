/* File: cmd4.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Interface commands */

#include "angband.h"



/*
 * A set of functions to maintain automatic dumps of various kinds.
 * -Mogami-
 *
 * remove_auto_dump(orig_file, mark)
 *     Remove the old automatic dump of type "mark".
 * auto_dump_printf(fmt, ...)
 *     Dump a formatted string using fprintf().
 * open_auto_dump(buf, mark)
 *     Open a file, remove old dump, and add new header.
 * close_auto_dump(void)
 *     Add a footer, and close the file.
 *
 *    The dump commands of original Angband simply add new lines to
 * existing files; these files will become bigger and bigger unless
 * an user deletes some or all of these files by hand at some
 * point.
 *
 *     These three functions automatically delete old dumped lines 
 * before adding new ones.  Since there are various kinds of automatic 
 * dumps in a single file, we add a header and a footer with a type 
 * name for every automatic dump, and kill old lines only when the 
 * lines have the correct type of header and footer.
 *
 *     We need to be quite paranoid about correctness; the user might 
 * (mistakenly) edit the file by hand, and see all their work come
 * to nothing on the next auto dump otherwise.  The current code only 
 * detects changes by noting inconsistencies between the actual number 
 * of lines and the number written in the footer.  Note that this will 
 * not catch single-line edits.
 */

/*
 *  Mark strings for auto dump
 */
static char auto_dump_header[] = "# vvvvvvv== %s ==vvvvvvv";
static char auto_dump_footer[] = "# ^^^^^^^== %s ==^^^^^^^";

/*
 * Variables for auto dump
 */
static FILE *auto_dump_stream;
static cptr auto_dump_mark;
static int auto_dump_line_num;

/*
 * Remove old lines automatically generated before.
 */
static void remove_auto_dump(cptr orig_file)
{
	FILE *tmp_fff, *orig_fff;

	char tmp_file[1024];
	char buf[1024];
	bool between_mark = FALSE;
	bool changed = FALSE;
	int line_num = 0;
	long header_location = 0;
	char header_mark_str[80];
	char footer_mark_str[80];
	size_t mark_len;

	/* Prepare a header/footer mark string */
	sprintf(header_mark_str, auto_dump_header, auto_dump_mark);
	sprintf(footer_mark_str, auto_dump_footer, auto_dump_mark);

	mark_len = strlen(footer_mark_str);

	/* Open an old dump file in read-only mode */
	orig_fff = my_fopen(orig_file, "r");

	/* If original file does not exist, nothing to do */
	if (!orig_fff) return;

	/* Open a new (temporary) file */
	tmp_fff = my_fopen_temp(tmp_file, 1024);

	if (!tmp_fff)
	{
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", tmp_file);
#else
	    msg_format("Failed to create temporary file %s.", tmp_file);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Loop for every line */
	while (TRUE)
	{
		/* Read a line */
		if (my_fgets(orig_fff, buf, sizeof(buf)))
		{
			/* Read error: Assume End of File */

			/*
			 * Was looking for the footer, but not found.
			 *
			 * Since automatic dump might be edited by hand,
			 * it's dangerous to kill these lines.
			 * Seek back to the next line of the (pseudo) header,
			 * and read again.
			 */
			if (between_mark)
			{
				fseek(orig_fff, header_location, SEEK_SET);
				between_mark = FALSE;
				continue;
			}

			/* Success -- End the loop */
			else
			{
				break;
			}
		}

		/* We are looking for the header mark of automatic dump */
		if (!between_mark)
		{
			/* Is this line a header? */
			if (!strcmp(buf, header_mark_str))
			{
				/* Memorise seek point of this line */
				header_location = ftell(orig_fff);

				/* Initialize counter for number of lines */
				line_num = 0;

				/* Look for the footer from now */
				between_mark = TRUE;

				/* There are some changes */
				changed = TRUE;
			}

			/* Not a header */
			else
			{
				/* Copy orginally lines */
				fprintf(tmp_fff, "%s\n", buf);
			}
		}

		/* We are looking for the footer mark of automatic dump */
		else
		{
			/* Is this line a footer? */
			if (!strncmp(buf, footer_mark_str, mark_len))
			{
				int tmp;

				/*
				 * Compare the number of lines
				 *
				 * If there is an inconsistency between
				 * actual number of lines and the
				 * number here, the automatic dump
				 * might be edited by hand.  So it's
				 * dangerous to kill these lines.
				 * Seek back to the next line of the
				 * (pseudo) header, and read again.
				 */
				if (!sscanf(buf + mark_len, " (%d)", &tmp)
				    || tmp != line_num)
				{
					fseek(orig_fff, header_location, SEEK_SET);
				}

				/* Look for another header */
				between_mark = FALSE;
			}

			/* Not a footer */
			else
			{
				/* Ignore old line, and count number of lines */
				line_num++;
			}
		}
	}

	/* Close files */
	my_fclose(orig_fff);
	my_fclose(tmp_fff);

	/* If there are some changes, overwrite the original file with new one */
	if (changed)
	{
		/* Copy contents of temporary file */

		tmp_fff = my_fopen(tmp_file, "r");
		orig_fff = my_fopen(orig_file, "w");

		while (!my_fgets(tmp_fff, buf, sizeof(buf)))
			fprintf(orig_fff, "%s\n", buf);

		my_fclose(orig_fff);
		my_fclose(tmp_fff);
	}

	/* Kill the temporary file */
	fd_kill(tmp_file);

	return;
}


/*
 * Dump a formatted line, using "vstrnfmt()".
 */
static void auto_dump_printf(cptr fmt, ...)
{
	cptr p;
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args, save the length */
	(void)vstrnfmt(buf, sizeof(buf), fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Count number of lines */
	for (p = buf; *p; p++)
	{
		if (*p == '\n') auto_dump_line_num++;
	}

	/* Dump it */
	fprintf(auto_dump_stream, "%s", buf);
}


/*
 *  Open file to append auto dump.
 */
static bool open_auto_dump(cptr buf, cptr mark)
{

	char header_mark_str[80];

	/* Save the mark string */
	auto_dump_mark = mark;

	/* Prepare a header mark string */
	sprintf(header_mark_str, auto_dump_header, auto_dump_mark);

	/* Remove old macro dumps */
	remove_auto_dump(buf);

	/* Append to the file */
	auto_dump_stream = my_fopen(buf, "a");

	/* Failure */
	if (!auto_dump_stream) {
#ifdef JP
		msg_format("%s ���J�����Ƃ��ł��܂���ł����B", buf);
#else
		msg_format("Failed to open %s.", buf);
#endif
		msg_print(NULL);

		/* Failed */
		return FALSE;
	}

	/* Start dumping */
	fprintf(auto_dump_stream, "%s\n", header_mark_str);

	/* Initialize counter */
	auto_dump_line_num = 0;

#ifdef JP
	auto_dump_printf("# *�x��!!* �ȍ~�̍s�͎����������ꂽ���̂ł��B\n");
	auto_dump_printf("# *�x��!!* ��Ŏ����I�ɍ폜�����̂ŕҏW���Ȃ��ł��������B\n");
#else
	auto_dump_printf("# *Warning!*  The lines below are an automatic dump.\n");
	auto_dump_printf("# Don't edit them; changes will be deleted and replaced automatically.\n");
#endif

	/* Success */
	return TRUE;
}

/*
 *  Append foot part and close auto dump.
 */
static void close_auto_dump(void)
{
	char footer_mark_str[80];

	/* Prepare a footer mark string */
	sprintf(footer_mark_str, auto_dump_footer, auto_dump_mark);

#ifdef JP
	auto_dump_printf("# *�x��!!* �ȏ�̍s�͎����������ꂽ���̂ł��B\n");
	auto_dump_printf("# *�x��!!* ��Ŏ����I�ɍ폜�����̂ŕҏW���Ȃ��ł��������B\n");
#else
	auto_dump_printf("# *Warning!*  The lines above are an automatic dump.\n");
	auto_dump_printf("# Don't edit them; changes will be deleted and replaced automatically.\n");
#endif

	/* End of dump */
	fprintf(auto_dump_stream, "%s (%d)\n", footer_mark_str, auto_dump_line_num);

	/* Close */
	my_fclose(auto_dump_stream);

	return;
}


#ifndef JP
/*
 * Return suffix of ordinal number
 */
cptr get_ordinal_number_suffix(int num)
{
	num = ABS(num) % 100;
	switch (num % 10)
	{
	case 1:
		return (num == 11) ? "th" : "st";
	case 2:
		return (num == 12) ? "th" : "nd";
	case 3:
		return (num == 13) ? "th" : "rd";
	default:
		return "th";
	}
}
#endif


/*
 *   Take note to the diary.
 */
/*:::playrecord-xxx.txt�ɓ��L������*/
///sys ���L�@���΂炭�͋����I�ɉ�������return�ɂ��Ă�������
errr do_cmd_write_nikki(int type, int num, cptr note)
{
	int day, hour, min;
	FILE *fff = NULL;
	char file_name[80];
	char buf[1024];
	cptr note_level = "";
	bool do_level = TRUE;
	char note_level_buf[40];
	int q_idx;

	static bool disable_nikki = FALSE;
//testmsg(format("NIKKI:%s",note));
	extract_day_hour_min(&day, &hour, &min);

	if (disable_nikki) return(-1);

	if (type == NIKKI_FIX_QUEST_C ||
	    type == NIKKI_FIX_QUEST_F ||
	    type == NIKKI_RAND_QUEST_C ||
	    type == NIKKI_RAND_QUEST_F ||
	    type == NIKKI_TO_QUEST)
	{
		int old_quest;

		old_quest = p_ptr->inside_quest;
		p_ptr->inside_quest = (quest[num].type == QUEST_TYPE_RANDOM) ? 0 : num;

		/* Get the quest text */
		init_flags = INIT_NAME_ONLY;

		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		/* Reset the old quest number */
		p_ptr->inside_quest = old_quest;
	}

#ifdef JP
	sprintf(file_name,"playrecord-%s.txt",savefile_base);
#else
	/* different filne name to avoid mixing */
	sprintf(file_name,"playrec-%s.txt",savefile_base);
#endif

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, file_name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff)
	{
#ifdef JP
		msg_format("%s ���J�����Ƃ��ł��܂���ł����B�v���C�L�^���ꎞ��~���܂��B", buf);
#else
		msg_format("Failed to open %s. Play-Record is disabled temporally.", buf);
#endif
		msg_format(NULL);
		disable_nikki=TRUE;
		return (-1);
	}

	q_idx = quest_number(dun_level);

	if (write_level)
	{
		if (p_ptr->inside_arena)
#ifdef JP
//			note_level = "�A���[�i:";v1.1.51�ύX
			note_level = "����:";
#else
			note_level = "Arane:";
#endif
		else if (!dun_level)
#ifdef JP
			note_level = "�n��:";
#else
			note_level = "Surface:";
#endif
		///sys ���L�@�I�x�����A�T�[�y���g�N�G
		else if (q_idx && (is_fixed_quest_idx(q_idx)
//		         && !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_SERPENT))))
		         && !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_YUKARI))))
#ifdef JP
			note_level = "�N�G�X�g:";
#else
			note_level = "Quest:";
#endif

		else if (INSIDE_EXTRA_QUEST)
		{
			sprintf(note_level_buf, "�G�N�X�g���N�G�X�g(%d�K):", dun_level);
			note_level = note_level_buf;
		}

		else
		{
#ifdef JP
			sprintf(note_level_buf, "%d�K(%s):", dun_level, d_name+d_info[dungeon_type].name);
#else
			sprintf(note_level_buf, "%s L%d:", d_name+d_info[dungeon_type].name, dun_level);
#endif
			note_level = note_level_buf;
		}
	}

	switch(type)
	{
		case NIKKI_HIGAWARI:
		{
#ifdef JP
			if (day < MAX_DAYS) fprintf(fff, "%d����\n", day);
			else fputs("*****����\n", fff);
#else
			if (day < MAX_DAYS) fprintf(fff, "Day %d\n", day);
			else fputs("Day *****\n", fff);
#endif
			do_level = FALSE;
			break;
		}
		case NIKKI_BUNSHOU:
		{
			if (num)
			{
				fprintf(fff, "%s\n",note);
				do_level = FALSE;
			}
			else
				fprintf(fff, " %2d:%02d %20s %s\n",hour, min, note_level, note);
			break;
		}
		case NIKKI_ART:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�𔭌������B\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s discovered %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_UNIQUE:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s��|�����B\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s defeated %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_FIX_QUEST_C:
		{
			if (quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �N�G�X�g�u%s�v��B�������B\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s completed quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case NIKKI_FIX_QUEST_F:
		{
			if (quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �N�G�X�g�u%s�v���疽���炪�瓦���A�����B\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s run away from quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case NIKKI_RAND_QUEST_C:
		{
			char name[80];
			strcpy(name, r_name+r_info[quest[num].r_idx].name);
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �����_���N�G�X�g(%s)��B�������B\n", hour, min, note_level, name);
#else
			fprintf(fff, " %2d:%02d %20s completed random quest '%s'\n", hour, min, note_level, name);
#endif
			break;
		}
		case NIKKI_RAND_QUEST_F:
		{
			char name[80];
			strcpy(name, r_name+r_info[quest[num].r_idx].name);
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �����_���N�G�X�g(%s)���瓦���o�����B\n", hour, min, note_level, name);
#else
			fprintf(fff, " %2d:%02d %20s ran away from quest '%s'.\n", hour, min, note_level, name);
#endif
			break;
		}
		case NIKKI_MAXDEAPTH:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�̍Ő[�K%d�K�ɓ��B�����B\n", hour, min, note_level, d_name+d_info[dungeon_type].name, num);
#else
			fprintf(fff, " %2d:%02d %20s reached level %d of %s for the first time.\n", hour, min, note_level, num, d_name+d_info[dungeon_type].name);
#endif
			break;
		}
		case NIKKI_TRUMP:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s%s�̍Ő[�K��%d�K�ɃZ�b�g�����B\n", hour, min, note_level, note, d_name + d_info[num].name, max_dlv[num]);
#else
			fprintf(fff, " %2d:%02d %20s reset recall level of %s to %d %s.\n", hour, min, note_level, d_name + d_info[num].name, max_dlv[num], note);
#endif
			break;
		}
		case NIKKI_STAIR:
		{
			cptr to;
			if (q_idx && (is_fixed_quest_idx(q_idx)
			   //  && !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_SERPENT))))
			       && !((q_idx == QUEST_TAISAI) || (q_idx == QUEST_YUKARI))))
			{
#ifdef JP
				to = "�n��";
#else
				to = "the surface";
#endif
			}
			else
			{
#ifdef JP
				if (!(dun_level+num)) to = "�n��";
				else to = format("%d�K", dun_level+num);
#else
				if (!(dun_level+num)) to = "the surface";
				else to = format("level %d", dun_level+num);
#endif
			}

#ifdef JP 
			fprintf(fff, " %2d:%02d %20s %s��%s�B\n", hour, min, note_level, to, note);
#else
			fprintf(fff, " %2d:%02d %20s %s %s.\n", hour, min, note_level, note, to);
#endif
			break;
		}
		case NIKKI_RECALL:
		{
			if (!num)
#ifdef JP
				fprintf(fff, " %2d:%02d %20s �A�҂��g����%s��%d�K�։��肽�B\n", hour, min, note_level, d_name+d_info[dungeon_type].name, max_dlv[dungeon_type]);
#else
				fprintf(fff, " %2d:%02d %20s recalled to dungeon level %d of %s.\n", hour, min, note_level, max_dlv[dungeon_type], d_name+d_info[dungeon_type].name);
#endif
			else
#ifdef JP
				fprintf(fff, " %2d:%02d %20s �A�҂��g���Ēn��ւƖ߂����B\n", hour, min, note_level);
#else
				fprintf(fff, " %2d:%02d %20s recalled from dungeon to surface.\n", hour, min, note_level);
#endif
			break;
		}
		case NIKKI_TO_QUEST:
		{
			if (quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �N�G�X�g�u%s�v�ւƓ˓������B\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s entered the quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case NIKKI_TELE_LEV:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s ���x���E�e���|�[�g�ŒE�o�����B\n", hour, min, note_level);
#else
			fprintf(fff, " %2d:%02d %20s Got out using teleport level.\n", hour, min, note_level);
#endif
			break;
		}
		case NIKKI_BUY:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s���w�������B\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s bought %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_SELL:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�𔄋p�����B\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s sold %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_ARENA:
		{
			int nightmare_rank = 0;

			while (nightmare_rank < NIGHTMARE_DIARY_STAGE_LEV_MAX-1)
			{
				if (nightmare_mon_base_level <= nightmare_stage_table[nightmare_rank].level) break;
				nightmare_rank++;
			}
			
			//�s�k
			if (num < 0)
			{
				fprintf(fff, " %2d:%02d %20s �u%s�v�̖��̏Z�l�����ɔs�ꋎ�����B\n", hour, min, note_level, nightmare_stage_table[nightmare_rank].desc);
			}
			//����
			else
			{
				fprintf(fff, " %2d:%02d %20s �u%s�v�̖��̏Z�l�����ɏ��������B\n", hour, min, note_level, nightmare_stage_table[nightmare_rank].desc);			
			}

#if 0
			if (num < 0)
			{
#ifdef JP
				fprintf(fff, " %2d:%02d %20s ���Z���%d���ŁA%s�̑O�ɔs�ꋎ�����B\n", hour, min, note_level, -num, note);
#else
				int n = -num;
				fprintf(fff, " %2d:%02d %20s beaten by %s in the %d%s fight.\n", hour, min, note_level, note, n, get_ordinal_number_suffix(n));
#endif
				break;
			}
#ifdef JP
			fprintf(fff, " %2d:%02d %20s ���Z���%d���(%s)�ɏ��������B\n", hour, min, note_level, num, note);
#else
			fprintf(fff, " %2d:%02d %20s won the %d%s fight (%s).\n", hour, min, note_level, num, get_ordinal_number_suffix(num), note);
#endif
			if (num == MAX_ARENA_MONS)
			{
#ifdef JP
				fprintf(fff, "                 ���Z��̂��ׂĂ̓G�ɏ������A�`�����s�I���ƂȂ����B\n");
#else
				fprintf(fff, "                 won all fight to become a Chanpion.\n");
#endif
				do_level = FALSE;
			}
#endif
			break;
		}
		case NIKKI_HANMEI:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�����ʂ����B\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s identified %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_WIZ_TELE:
		{
			cptr to;
			if (!dun_level)
#ifdef JP
				to = "�n��";
#else
				to = "the surface";
#endif
			else
#ifdef JP
				to = format("%d�K(%s)", dun_level, d_name+d_info[dungeon_type].name);
#else
				to = format("level %d of %s", dun_level, d_name+d_info[dungeon_type].name);
#endif

#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�ւƃE�B�U�[�h�E�e���|�[�g�ňړ������B\n", hour, min, note_level, to);
#else
			fprintf(fff, " %2d:%02d %20s wizard-teleport to %s.\n", hour, min, note_level, to);
#endif
			break;
		}
		case NIKKI_PAT_TELE:
		{
			cptr to;
			if (!dun_level)
#ifdef JP
				to = "�n��";
#else
				to = "the surface";
#endif
			else
#ifdef JP
				to = format("%d�K(%s)", dun_level, d_name+d_info[dungeon_type].name);
#else
				to = format("level %d of %s", dun_level, d_name+d_info[dungeon_type].name);
#endif

#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�ւƃp�^�[���̗͂ňړ������B\n", hour, min, note_level, to);
#else
			fprintf(fff, " %2d:%02d %20s used Pattern to teleport to %s.\n", hour, min, note_level, to);
#endif
			break;
		}
		case NIKKI_LEVELUP:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s ���x����%d�ɏオ�����B\n", hour, min, note_level, num);
#else
			fprintf(fff, " %2d:%02d %20s reached player level %d.\n", hour, min, note_level, num);
#endif
			break;
		}
		case NIKKI_GAMESTART:
		{
			time_t ct = time((time_t*)0);
			do_level = FALSE;
			if (num)
			{
				fprintf(fff, "%s %s",note, ctime(&ct));
			}
			else
				fprintf(fff, " %2d:%02d %20s %s %s",hour, min, note_level, note, ctime(&ct));
			break;
		}
		case NIKKI_NAMED_PET:
		{
			fprintf(fff, " %2d:%02d %20s ", hour, min, note_level);
			switch (num)
			{
				case RECORD_NAMED_PET_NAME:
#ifdef JP
					fprintf(fff, "%s�𗷂̗F�ɂ��邱�ƂɌ��߂��B\n", note);
#else
					fprintf(fff, "decided to travel together with %s.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_UNNAME:
#ifdef JP
					fprintf(fff, "%s�̖��O���������B\n", note);
#else
					fprintf(fff, "unnamed %s.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_DISMISS:
#ifdef JP
					fprintf(fff, "%s����������B\n", note);
#else
					fprintf(fff, "dismissed %s.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_DEATH:
#ifdef JP
					fprintf(fff, "%s���|�ꂽ�B\n", note);
#else
					fprintf(fff, "%s died.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_MOVED:
#ifdef JP
					fprintf(fff, "%s�������ĕʂ̃}�b�v�ֈړ������B\n", note);
#else
					fprintf(fff, "moved to another map leaving %s behind.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_LOST_SIGHT:
#ifdef JP
					fprintf(fff, "%s�Ƃ͂���Ă��܂����B\n", note);
#else
					fprintf(fff, "lost sight of %s.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_DESTROY:
#ifdef JP
					fprintf(fff, "%s��*�j��*�ɂ���ď����������B\n", note);
#else
					fprintf(fff, "%s was made disappeared by *destruction*.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_EARTHQUAKE:
#ifdef JP
					fprintf(fff, "%s���n�ʂɖ�����Ă͂��ꂽ�B\n", note);
#else
					fprintf(fff, "%s was crushed by falling rocks.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_GENOCIDE:
#ifdef JP
					fprintf(fff, "%s�����E�ɂ���ď����������B\n", note);
#else
					fprintf(fff, "%s was made disappeared by genocide.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_WIZ_ZAP:
#ifdef JP
					fprintf(fff, "%s���f�o�b�O�R�}���h�ɂ���ď����������B\n", note);
#else
					fprintf(fff, "%s was removed by debug command.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_TELE_LEVEL:
#ifdef JP
					fprintf(fff, "%s���e���|�[�g�E���x���ɂ���ď����������B\n", note);
#else
					fprintf(fff, "%s was made disappeared by teleport level.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_BLAST:
#ifdef JP
					fprintf(fff, "%s�𔚔j�����B\n", note);
#else
					fprintf(fff, "blasted %s.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_HEAL_LEPER:
#ifdef JP
					fprintf(fff, "%s�̕a�C�����藷����O�ꂽ�B\n", note);
#else
					fprintf(fff, "%s was healed and left.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_COMPACT:
#ifdef JP
					fprintf(fff, "%s�������X�^�[��񈳏k�ɂ���ď����������B\n", note);
#else
					fprintf(fff, "%s was made disappeared by compacting monsters.\n", note);
#endif
					break;
				case RECORD_NAMED_PET_LOSE_PARENT:
#ifdef JP
					fprintf(fff, "%s�̏����҂����ɂ��Ȃ����ߏ����������B\n", note);
#else
					fprintf(fff, "%s disappeared because there does not exist summoner.\n", note);
#endif
					break;
				default:
					fprintf(fff, "\n");
					break;
			}
			break;
		}
		default:
			break;
	}

	my_fclose(fff);

	if (do_level) write_level = FALSE;

	return (0);
}


#define MAX_SUBTITLE (sizeof(subtitle)/sizeof(subtitle[0]))

/*:::���L��\������B�K���ȃT�u�^�C�g��������*/
static void do_cmd_disp_nikki(void)
{
	char nikki_title[256];
	char file_name[80];
	char buf[1024];
	char tmp[80];
#ifdef JP
	static const char subtitle[][30] = {"�ŋ��̓��̂����߂�",
					   "�l������͂͂��Ȃ�",
					   "�����Ɍ�������",
					   "�I����ڂ�����",
					   "���Ƃ̍Ղ�",
					   "����͂����l����",
					   "���Ƃł�����",
					   "�e�ɂ��p�ɂ�",
					   "�E�\������",
					   "���͂₱��܂�",
					   "�Ȃ�ł����Ȃ��",
					   "����͖�����",
					   "�|���ׂ��G�̓Q���c",
					   "��`�H��������Ȃ�",
					   "�I���̖��������Ă݂�",
					   "�����ςɂȂ��������",
					   "�݊����܂���",
					   "��������������",
					   "�܂��܂��Â���",
					   "�ނ����ނ�������",
					   "����Ȃ��񂶂�Ȃ�",
					   "���߂������",
					   "�������Ă݂悤",
					   "������Ƃ�����",
					   "�������`����",
					   "��]�̉ʂ�",
					   "�����n��",
					   "�_�Ɍ��܂𔄂��",
					   "���m�̐��E��",
					   "�ō��̓��]�����߂�"};
#else
	static const char subtitle[][51] ={"Quest of The World's Toughest Body",
					   "Attack is the best form of defence.",
					   "Might is right.",
					   "An unexpected windfall",
					   "A drowning man will catch at a straw",
					   "Don't count your chickens before they are hatched.",
					   "It is no use crying over spilt milk.",
					   "Seeing is believing.",
					   "Strike the iron while it is hot.",
					   "I don't care what follows.",
					   "To dig a well to put out a house on fire.",
					   "Tomorrow is another day.",
					   "Easy come, easy go.",
					   "The more haste, the less speed.",
					   "Where there is life, there is hope.",
					   "There is no royal road to *WINNER*.",
					   "Danger past, God forgotten.",
					   "The best thing to do now is to run away.",
					   "Life is but an empty dream.",
					   "Dead men tell no tales.",
					   "A book that remains shut is but a block.",
					   "Misfortunes never come singly.",
					   "A little knowledge is a dangerous thing.",
					   "History repeats itself.",
					   "*WINNER* was not built in a day.",
					   "Ignorance is bliss.",
					   "To lose is to win?",
					   "No medicine can cure folly.",
					   "All good things come to an end.",
					   "M$ Empire strikes back.",
					   "To see is to believe",
					   "Time is money.",
					   "Quest of The World's Greatest Brain"};
#endif
#ifdef JP
	sprintf(file_name,"playrecord-%s.txt",savefile_base);
#else
	sprintf(file_name,"playrec-%s.txt",savefile_base);
#endif

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, file_name);

	if (p_ptr->pclass == CLASS_WARRIOR || p_ptr->pclass == CLASS_MONK || p_ptr->pclass == CLASS_SAMURAI)
		strcpy(tmp,subtitle[randint0(MAX_SUBTITLE-1)]);
	else if (p_ptr->pclass == CLASS_MAGE || p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER)
		strcpy(tmp,subtitle[randint0(MAX_SUBTITLE-1)+1]);
	else strcpy(tmp,subtitle[randint0(MAX_SUBTITLE-2)+1]);

#ifdef JP
	if(p_ptr->psex == SEX_MALE)
		sprintf(nikki_title, "�u%s%s%s�̓`�� -%s-�v",ap_ptr->title, ap_ptr->no ? "��" : "", player_name, tmp);
	else
		sprintf(nikki_title, "�u%s%s%s�̓`�� -%s-�v",ap_ptr->f_title, ap_ptr->no ? "��" : "", player_name, tmp);
#else
	sprintf(nikki_title, "Legend of %s %s '%s'",
		ap_ptr->title, player_name, tmp);
#endif

	/* Display the file contents */
	show_file(FALSE, buf, nikki_title, -1, 0);
}

/*:::���͂���͂����L�ɏ�������*/
static void do_cmd_bunshou(void)
{
	char tmp[80] = "\0";
	char bunshou[80] = "\0";

#ifdef JP
	if (get_string("���e: ", tmp, 79))
#else
	if (get_string("diary note: ", tmp, 79))
#endif
	{
		strcpy(bunshou, tmp);

		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, bunshou);
	}
}

/*:::���O�ɓ���A�Ӓ肵�����̂��L�^����*/
static void do_cmd_last_get(void)
{
	char buf[256];
	s32b turn_tmp;

	if (record_o_name[0] == '\0') return;

#ifdef JP
	sprintf(buf,"%s�̓�����L�^���܂��B",record_o_name);
#else
	sprintf(buf,"Do you really want to record getting %s? ",record_o_name);
#endif
	if (!get_check(buf)) return;

	turn_tmp = turn;
	turn = record_turn;
#ifdef JP
	sprintf(buf,"%s����ɓ��ꂽ�B", record_o_name);
#else
	sprintf(buf,"descover %s.", record_o_name);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 0, buf);
	turn = turn_tmp;
}

/*:::���L������*/
static void do_cmd_erase_nikki(void)
{
	char file_name[80];
	char buf[256];
	FILE *fff = NULL;

#ifdef JP
	if (!get_check("�{���ɋL�^���������܂����H")) return;
#else
	if (!get_check("Do you really want to delete all your record? ")) return;
#endif

#ifdef JP
	sprintf(file_name,"playrecord-%s.txt",savefile_base);
#else
	sprintf(file_name,"playrec-%s.txt",savefile_base);
#endif

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, file_name);

	/* Remove the file */
	fd_kill(buf);

	fff = my_fopen(buf, "w");
	if(fff){
		my_fclose(fff);
#ifdef JP
		msg_format("�L�^���������܂����B");
#else
		msg_format("deleted record.");
#endif
	}else{
#ifdef JP
		msg_format("%s �̏����Ɏ��s���܂����B", buf);
#else
		msg_format("failed to delete %s.", buf);
#endif
	}
	msg_print(NULL);
}

/*:::���L�֌W�̏���*/
///system �v���C����֌W���������Ă�����
void do_cmd_nikki(void)
{
	int i;

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Save the screen */
	screen_save();

	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("[ �L�^�̐ݒ� ]", 2, 0);
#else
		prt("[ Play Record ]", 2, 0);
#endif


		/* Give some choices */
#ifdef JP
		prt("(1) �L�^������", 4, 5);
		prt("(2) ���͂��L�^����", 5, 5);
		prt("(3) ���O�ɓ��薔�͊Ӓ肵�����̂��L�^����", 6, 5);
		prt("(4) �L�^����������", 7, 5);
///sysdel131221 �v���C���斳��
		//prt("(R) �v���C������L�^����/���~����", 9, 5);
#else
		prt("(1) Display your record", 4, 5);
		prt("(2) Add record", 5, 5);
		prt("(3) Record item you last get/identify", 6, 5);
		prt("(4) Delete your record", 7, 5);

		prt("(R) Record playing movie / or stop it", 9, 5);
#endif


		/* Prompt */
#ifdef JP
		prt("�R�}���h:", 18, 0);
#else
		prt("Command: ", 18, 0);
#endif


		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		switch (i)
		{
		case '1':
			do_cmd_disp_nikki();
			break;
		case '2':
			do_cmd_bunshou();
			break;
		case '3':
			do_cmd_last_get();
			break;
		case '4':
			do_cmd_erase_nikki();
			break;
///sysdel131221 �v���C���斳��
/*
		case 'r': case 'R':
			screen_load();
			prepare_movie_hooks();
			return;
*/
			default: /* Unknown option */
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	screen_load();
}

/*
 * Hack -- redraw the screen
 *
 * This command performs various low level updates, clears all the "extra"
 * windows, does a total redraw of the main window, and requests all of the
 * interesting updates and redraws that I can think of.
 *
 * This command is also used to "instantiate" the results of the user
 * selecting various things, such as graphics mode, so it must call
 * the "TERM_XTRA_REACT" hook before redrawing the windows.
 */
/*:::�S�̍ĕ`��炵���@�ڍז���*/
void do_cmd_redraw(void)
{
	int j;

	term *old = Term;


	/* Hack -- react to changes */
	Term_xtra(TERM_XTRA_REACT, 0);


	/* Combine and Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);


	/* Update torch */
	p_ptr->update |= (PU_TORCH);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Forget lite/view */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	update_playtime();

	/* Hack -- update */
	handle_stuff();

	if (p_ptr->prace == RACE_ANDROID) calc_android_exp();


	/* Redraw every window */
	for (j = 0; j < 8; j++)
	{
		/* Dead window */
		if (!angband_term[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw */
		Term_redraw();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- change name
 */
/*:::���̏���\������B�_���v�����o���A���O�ύX���s��*/

void do_cmd_change_name(void)
{
	char	c;

	int		mode = 0;

	char	tmp[160];


	/* Save the screen */
	screen_save();

	/* Forever */
	while (1)
	{
		update_playtime();

		/* Display the player */
		/*:::�ˑR�ψق�C�R�}���h�ŕ\������I�v�V������ON�̂Ƃ��A��������mode=4�œ���*/
		display_player(mode);

		if (mode == 4)
		{
			mode = 0;
			display_player(mode);
		}

		/* Prompt */
#ifdef JP
		if(cp_ptr->flag_only_unique) 
			Term_putstr(2, 23, -1, TERM_WHITE, "['c'�ŐE�Ɩ��ύX, 'f'�Ńt�@�C���֏��o, 'h'�Ń��[�h�ύX, ESC�ŏI��]");
		else
			Term_putstr(2, 23, -1, TERM_WHITE, "['c'�Ŗ��O�ύX, 'f'�Ńt�@�C���֏��o, 'h'�Ń��[�h�ύX, ESC�ŏI��]");
#else
		Term_putstr(2, 23, -1, TERM_WHITE,
			"['c' to change name, 'f' to file, 'h' to change mode, or ESC]");
#endif


		/* Query */
		c = inkey();

		/* Exit */
		if (c == ESCAPE) break;

		/* Change name */
		if (c == 'c')
		{
			///system ���j�[�N�v���C���[�Ȃ�E�Ɩ���ύX����悤�ɂ��Ă���
			if(cp_ptr->flag_only_unique) get_class_name();

			else
			{
				get_name();
				/* Process the player name */
				process_player_name(FALSE);
			}
		}

		/* File dump */
		else if (c == 'f')
		{
			sprintf(tmp, "%s.txt", player_base);
#ifdef JP
			if (get_string("�t�@�C����: ", tmp, 80))
#else
			if (get_string("File name: ", tmp, 80))
#endif

			{
				if (tmp[0] && (tmp[0] != ' '))
				{
					file_character(tmp);
				}
			}
		}

		/* Toggle mode */
		else if (c == 'h')
		{
			mode++;
		}

		/* Oops */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	screen_load();

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	handle_stuff();
}


/*
 * Recall the most recent message
 */
/*:::�ЂƂO�̃��b�Z�[�W�\��*/
void do_cmd_message_one(void)
{
	/* Recall one message XXX XXX XXX */
	prt(format("> %s", message_str(0)), 0, 0);
}


/*
 * Show previous messages to the user	-BEN-
 *
 * The screen format uses line 0 and 23 for headers and prompts,
 * skips line 1 and 22, and uses line 2 thru 21 for old messages.
 *
 * This command shows you which commands you are viewing, and allows
 * you to "search" for strings in the recall.
 *
 * Note that messages may be longer than 80 characters, but they are
 * displayed using "infinite" length, with a special sub-command to
 * "slide" the virtual display to the left or right.
 *
 * Attempt to only hilite the matching portions of the string.
 */
/*:::���b�Z�[�W������\������@�ڍז��ǂ������Ԃ񂱂̂܂܎g���邾�낤*/
void do_cmd_messages(int num_now)
{
	int i, n;

	char shower_str[81];
	char finder_str[81];
	char back_str[81];
	cptr shower = NULL;
	int wid, hgt;
	int num_lines;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Number of message lines in a screen */
	num_lines = hgt - 4;

	/* Wipe finder */
	strcpy(finder_str, "");

	/* Wipe shower */
	strcpy(shower_str, "");

	/* Total messages */
	n = message_num();

	/* Start on first message */
	i = 0;

	/* Save the screen */
	screen_save();

	/* Clear screen */
	Term_clear();

	/* Process requests until done */
	while (1)
	{
		int j;
		int skey;

		/* Dump up to 20 lines of messages */
		for (j = 0; (j < num_lines) && (i + j < n); j++)
		{
			cptr msg = message_str(i+j);

			/* Dump the messages, bottom to top */
			c_prt((i + j < num_now ? TERM_WHITE : TERM_SLATE), msg, num_lines + 1 - j, 0);

			/* Hilite "shower" */
			if (shower && shower[0])
			{
				cptr str = msg;

				/* Display matches */
				while ((str = my_strstr(str, shower)) != NULL)
				{
					int len = strlen(shower);

					/* Display the match */
					Term_putstr(str-msg, num_lines + 1 - j, len, TERM_YELLOW, shower);

					/* Advance */
					str += len;
				}
			}
		}

		/* Erase remaining lines */
		for (; j < num_lines; j++)
		{
			Term_erase(0, num_lines + 1 - j, 255);
		}

		/* Display header XXX XXX XXX */
#ifdef JP
		/* translation */
		prt(format("�ȑO�̃��b�Z�[�W %d-%d �S����(%d)",
			   i, i + j - 1, n), 0, 0);
#else
		prt(format("Message Recall (%d-%d of %d)",
			   i, i + j - 1, n), 0, 0);
#endif

		/* Display prompt (not very informative) */
#ifdef JP
		prt("[ 'p' �ōX�ɌÂ�����, 'n' �ōX�ɐV��������, '/' �Ō���, ESC �Œ��f ]", hgt - 1, 0);
#else
		prt("[Press 'p' for older, 'n' for newer, ..., or ESCAPE]", hgt - 1, 0);
#endif

		/* Get a command */
		skey = inkey_special(TRUE);

		/* Exit on Escape */
		if (skey == ESCAPE) break;

		/* Hack -- Save the old index */
		j = i;

		switch (skey)
		{
		/* Hack -- handle show */
		case '=':
			/* Prompt */
#ifdef JP
			prt("����: ", hgt - 1, 0);
#else
			prt("Show: ", hgt - 1, 0);
#endif

			/* Get a "shower" string, or continue */
			strcpy(back_str, shower_str);
			if (askfor(shower_str, 80))
			{
				/* Show it */
				shower = shower_str[0] ? shower_str : NULL;
			}
			else strcpy(shower_str, back_str);

			/* Okay */
			continue;

		/* Hack -- handle find */
		case '/':
		case KTRL('s'):
			{
				int z;

				/* Prompt */
#ifdef JP
				prt("����: ", hgt - 1, 0);
#else
				prt("Find: ", hgt - 1, 0);
#endif

				/* Get a "finder" string, or continue */
				strcpy(back_str, finder_str);
				if (!askfor(finder_str, 80))
				{
					strcpy(finder_str, back_str);
					continue;
				}
				else if (!finder_str[0])
				{
					shower = NULL; /* Stop showing */
					continue;
				}

				/* Show it */
				shower = finder_str;

				/* Scan messages */
				for (z = i + 1; z < n; z++)
				{
					cptr msg = message_str(z);

					/* Search for it */
					if (my_strstr(msg, finder_str))
					{
						/* New location */
						i = z;

						/* Done */
						break;
					}
				}
			}
			break;

		/* Recall 1 older message */
		case SKEY_TOP:
			/* Go to the oldest line */
			i = n - num_lines;
			break;

		/* Recall 1 newer message */
		case SKEY_BOTTOM:
			/* Go to the newest line */
			i = 0;
			break;

		/* Recall 1 older message */
		case '8':
		case SKEY_UP:
		case '\n':
		case '\r':
			/* Go older if legal */
			i = MIN(i + 1, n - num_lines);
			break;

		/* Recall 10 older messages */
		case '+':
			/* Go older if legal */
			i = MIN(i + 10, n - num_lines);
			break;

		/* Recall 20 older messages */
		case 'p':
		case KTRL('P'):
		case ' ':
		case SKEY_PGUP:
			/* Go older if legal */
			i = MIN(i + num_lines, n - num_lines);
			break;

		/* Recall 20 newer messages */
		case 'n':
		case KTRL('N'):
		case SKEY_PGDOWN:
			/* Go newer (if able) */
			i = MAX(0, i - num_lines);
			break;

		/* Recall 10 newer messages */
		case '-':
			/* Go newer (if able) */
			i = MAX(0, i - 10);
			break;

		/* Recall 1 newer messages */
		case '2':
		case SKEY_DOWN:
			/* Go newer (if able) */
			i = MAX(0, i - 1);
			break;
		}

		/* Hack -- Error of some kind */
		if (i == j) bell();
	}

	/* Restore the screen */
	screen_load();
}



/*
 * Number of cheating options
 */
#define CHEAT_MAX 7

/*
 * Cheating options
 */
static option_type cheat_info[CHEAT_MAX] =
{
	{ &cheat_peek,		FALSE,	255,	0x01, 0x00,
#ifdef JP
	"cheat_peek",		"�A�C�e���̐������̂�������"
#else
	"cheat_peek",		"Peek into object creation"
#endif
	},

	{ &cheat_hear,		FALSE,	255,	0x02, 0x00,
#ifdef JP
	"cheat_hear",		"�����X�^�[�̐������̂�������"
#else
	"cheat_hear",		"Peek into monster creation"
#endif
	},

	{ &cheat_room,		FALSE,	255,	0x04, 0x00,
#ifdef JP
	"cheat_room",		"�_���W�����̐������̂�������"
#else
	"cheat_room",		"Peek into dungeon creation"
#endif
	},

	{ &cheat_xtra,		FALSE,	255,	0x08, 0x00,
#ifdef JP
	"cheat_xtra",		"���̑��̎����̂�������"
#else
	"cheat_xtra",		"Peek into something else"
#endif
	},

	{ &cheat_know,		FALSE,	255,	0x10, 0x00,
#ifdef JP
	"cheat_know",		"���S�ȃ����X�^�[�̎v���o��m��"
#else
	"cheat_know",		"Know complete monster info"
#endif
	},

	{ &cheat_live,		FALSE,	255,	0x20, 0x00,
#ifdef JP
	"cheat_live",		"����������邱�Ƃ��\�ɂ���"
#else
	"cheat_live",		"Allow player to avoid death"
#endif
	},

	{ &cheat_save,		FALSE,	255,	0x40, 0x00,
#ifdef JP
	"cheat_save",		"���񂾎��Z�[�u���邩�m�F����"
#else
	"cheat_save",		"Ask for saving death"
#endif
	}
};

/*
 * Interact with some options for cheating
 */
static void do_cmd_options_cheat(cptr info)
{
	char	ch;

	int		i, k = 0, n = CHEAT_MAX;

	char	buf[80];


	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		int dir;

		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s ( ���^�[���Ŏ���, y/n �ŃZ�b�g, ESC �Ō��� )", info);
#else
		sprintf(buf, "%s (RET to advance, y/n to set, ESC to accept) ", info);
#endif

		prt(buf, 0, 0);

#ifdef JP
		if(p_ptr->pseikaku == SEIKAKU_COLLECTOR)
		{

			prt("                     <<  ����  >>", 11, 0);
			prt("      ���\�I�v�V��������x�ł��ݒ肷��ƁA", 12, 0);
			prt("      ���̏N�W�ƂɃG���g�����X�������p���Ȃ��Ȃ�܂��B", 13, 0);

		}
		else
		{
			/* ���\�I�v�V�������������肢�����Ă��܂��l������悤�Ȃ̂Œ��� */
			prt("                                 <<  ����  >>", 11, 0);
			prt("      ���\�I�v�V��������x�ł��ݒ肷��ƁA�X�R�A�L�^���c��Ȃ��Ȃ�܂��I", 12, 0);
			prt("      ��ɉ������Ă��_���ł��̂ŁA�����҂�ڎw�����͂����̃I�v�V�����͂�", 13, 0);
			prt("      ����Ȃ��悤�ɂ��ĉ������B", 14, 0);

		}
#endif
		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%s)",
			    cheat_info[i].o_desc,
#ifdef JP
			    (*cheat_info[i].o_var ? "�͂�  " : "������"),
#else
			    (*cheat_info[i].o_var ? "yes" : "no "),
#endif

			    cheat_info[i].o_text);
			c_prt(a, buf, i + 2, 0);
		}

		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/*
		 * HACK - Try to translate the key into a direction
		 * to allow using the roguelike keys for navigation.
		 */
		dir = get_keymap_dir(ch);
		if ((dir == 2) || (dir == 4) || (dir == 6) || (dir == 8))
			ch = I2D(dir);

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{
				if(!p_ptr->noscore)
#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "���\�I�v�V������ON�ɂ��āA�X�R�A���c���Ȃ��Ȃ����B");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up sending score to use cheating options.");
#endif
				p_ptr->noscore |= (cheat_info[k].o_set * 256 + cheat_info[k].o_bit);
				(*cheat_info[k].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				(*cheat_info[k].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case '?':
			{
#ifdef JP
				strnfmt(buf, sizeof(buf), "joption.txt#%s", cheat_info[k].o_text);
#else
				strnfmt(buf, sizeof(buf), "option.txt#%s", cheat_info[k].o_text);
#endif
				/* Peruse the help file */
				(void)show_file(TRUE, buf, NULL, 0, 0);

				Term_clear(); 
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


static option_type autosave_info[2] =
{
	{ &autosave_l,      FALSE, 255, 0x01, 0x00,
#ifdef JP
	    "autosave_l",    "�V�����K�ɓ���x�Ɏ����Z�[�u����" },
#else
	    "autosave_l",    "Autosave when entering new levels" },
#endif


	{ &autosave_t,      FALSE, 255, 0x02, 0x00,
#ifdef JP
	    "autosave_t",   "���^�[�����Ɏ����Z�[�u����" },
#else
	    "autosave_t",   "Timed autosave" },
#endif

};


static s16b toggle_frequency(s16b current)
{
	switch (current)
	{
	case 0: return 50;
	case 50: return 100;
	case 100: return 250;
	case 250: return 500;
	case 500: return 1000;
	case 1000: return 2500;
	case 2500: return 5000;
	case 5000: return 10000;
	case 10000: return 25000;
	default: return 0;
	}
}


/*
 * Interact with some options for cheating
 */
static void do_cmd_options_autosave(cptr info)
{
	char	ch;

	int     i, k = 0, n = 2;

	char	buf[80];


	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s ( ���^�[���Ŏ���, y/n �ŃZ�b�g, F �ŕp�x�����, ESC �Ō��� ) ", info);
#else
		sprintf(buf, "%s (RET to advance, y/n to set, 'F' for frequency, ESC to accept) ", info);
#endif

		prt(buf, 0, 0);

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%s)",
			    autosave_info[i].o_desc,
#ifdef JP
			    (*autosave_info[i].o_var ? "�͂�  " : "������"),
#else
			    (*autosave_info[i].o_var ? "yes" : "no "),
#endif

			    autosave_info[i].o_text);
			c_prt(a, buf, i + 2, 0);
		}

#ifdef JP
		prt(format("�����Z�[�u�̕p�x�F %d �^�[����",  autosave_freq), 5, 0);
#else
		prt(format("Timed autosave frequency: every %d turns",  autosave_freq), 5, 0);
#endif



		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{

				(*autosave_info[k].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				(*autosave_info[k].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case 'f':
			case 'F':
			{
				autosave_freq = toggle_frequency(autosave_freq);
#ifdef JP
				prt(format("�����Z�[�u�̕p�x�F %d �^�[����", 
					   autosave_freq), 5, 0);
#else
				prt(format("Timed autosave frequency: every %d turns",
					   autosave_freq), 5, 0);
#endif
				break;
			}

			case '?':
			{
#ifdef JP
				(void)show_file(TRUE, "toption.txt#Autosave", NULL, 0, 0);
#else
				(void)show_file(TRUE, "option.txt#Autosave", NULL, 0, 0);
#endif


				Term_clear(); 
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


/*
 * Interact with some options
 */
void do_cmd_options_aux(int page, cptr info)
{
	char    ch;
	int     i, k = 0, n = 0, l;
	int     opt[24];
	char    buf[100];
	bool    browse_only = (page == OPT_PAGE_BIRTH) && character_generated &&
	                      (!p_ptr->wizard || !allow_debug_opts);


	/* Lookup the options */
	for (i = 0; i < 24; i++) opt[i] = 0;

	/* Scan the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		/* Notice options on this "page" */
		if (option_info[i].o_page == page) opt[n++] = i;
	}


	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		int dir;

		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s (���^�[��:��, %sESC:�I��, ?:�w���v) ", info, browse_only ? "" : "y/n:�ύX, ");
#else
		sprintf(buf, "%s (RET:next, %s, ?:help) ", info, browse_only ? "ESC:exit" : "y/n:change, ESC:accept");
#endif

		prt(buf, 0, 0);


		/* HACK -- description for easy-auto-destroy options */
#ifdef JP
		if (page == OPT_PAGE_AUTODESTROY) c_prt(TERM_YELLOW, "�ȉ��̃I�v�V�����́A�ȈՎ����j����g�p����Ƃ��̂ݗL��", 6, 6);
#else
		if (page == OPT_PAGE_AUTODESTROY) c_prt(TERM_YELLOW, "Following options will protect items from easy auto-destroyer.", 6, 3);
#endif

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%.24s)",
				option_info[opt[i]].o_desc,
#ifdef JP
				(*option_info[opt[i]].o_var ? "�͂�  " : "������"),
#else
				(*option_info[opt[i]].o_var ? "yes" : "no "),
#endif

				option_info[opt[i]].o_text);
			if ((page == OPT_PAGE_AUTODESTROY) && i > 2) c_prt(a, buf, i + 5, 0);
			else c_prt(a, buf, i + 2, 0);
		}

		if ((page == OPT_PAGE_AUTODESTROY) && (k > 2)) l = 3;
		else l = 0;

		/* Hilite current option */
		move_cursor(k + 2 + l, 50);

		/* Get a key */
		ch = inkey();

		/*
		 * HACK - Try to translate the key into a direction
		 * to allow using the roguelike keys for navigation.
		 */
		dir = get_keymap_dir(ch);
		if ((dir == 2) || (dir == 4) || (dir == 6) || (dir == 8))
			ch = I2D(dir);

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{
				if (browse_only) break;
				(*option_info[opt[k]].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				if (browse_only) break;
				(*option_info[opt[k]].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case 't':
			case 'T':
			{
				if (!browse_only) (*option_info[opt[k]].o_var) = !(*option_info[opt[k]].o_var);
				break;
			}

			case '?':
			{
#ifdef JP
				strnfmt(buf, sizeof(buf), "toption.txt#%s", option_info[opt[k]].o_text);
#else
				strnfmt(buf, sizeof(buf), "option.txt#%s", option_info[opt[k]].o_text);
#endif
				/* Peruse the help file */
				(void)show_file(TRUE, buf, NULL, 0, 0);

				Term_clear();
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


/*
 * Modify the "window" options
 */
static void do_cmd_options_win(void)
{
	int i, j, d;

	int y = 0;
	int x = 0;

	char ch;

	bool go = TRUE;

	u32b old_flag[8];


	/* Memorize old flags */
	for (j = 0; j < 8; j++)
	{
		/* Acquire current flags */
		old_flag[j] = window_flag[j];
	}


	/* Clear screen */
	Term_clear();

	/* Interact */
	while (go)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		prt("�E�B���h�E�E�t���O (<����>�ňړ�, t�Ń`�F���W, y/n �ŃZ�b�g, ESC)", 0, 0);
#else
		prt("Window Flags (<dir>, t, y, n, ESC) ", 0, 0);
#endif


		/* Display the windows */
		for (j = 0; j < 8; j++)
		{
			byte a = TERM_WHITE;

			cptr s = angband_term_name[j];

			/* Use color */
			if (j == x) a = TERM_L_BLUE;

			/* Window name, staggered, centered */
			Term_putstr(35 + j * 5 - strlen(s) / 2, 2 + j % 2, -1, a, s);
		}

		/* Display the options */
		for (i = 0; i < 16; i++)
		{
			byte a = TERM_WHITE;

			cptr str = window_flag_desc[i];

			/* Use color */
			if (i == y) a = TERM_L_BLUE;

			/* Unused option */
#ifdef JP
			if (!str) str = "(���g�p)";
#else
			if (!str) str = "(Unused option)";
#endif


			/* Flag name */
			Term_putstr(0, i + 5, -1, a, str);

			/* Display the windows */
			for (j = 0; j < 8; j++)
			{
				a = TERM_WHITE;

				char c = '.';

				/* Use color */
				if ((i == y) && (j == x)) a = TERM_L_BLUE;

				/* Active flag */
				if (window_flag[j] & (1L << i)) c = 'X';

				/* Flag value */
				Term_putch(35 + j * 5, i + 5, a, c);
			}
		}

		/* Place Cursor */
		Term_gotoxy(35 + x * 5, y + 5);

		/* Get key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				go = FALSE;
				break;
			}

			case 'T':
			case 't':
			{
				/* Clear windows */
				for (j = 0; j < 8; j++)
				{
					window_flag[j] &= ~(1L << y);
				}

				/* Clear flags */
				for (i = 0; i < 16; i++)
				{
					window_flag[x] &= ~(1L << i);
				}

				/* Fall through */
			}

			case 'y':
			case 'Y':
			{
				/* Ignore screen */
				if (x == 0) break;

				/* Set flag */
				window_flag[x] |= (1L << y);
				break;
			}

			case 'n':
			case 'N':
			{
				/* Clear flag */
				window_flag[x] &= ~(1L << y);
				break;
			}

			case '?':
			{
#ifdef JP
				(void)show_file(TRUE, "toption.txt#Window", NULL, 0, 0);
#else
				(void)show_file(TRUE, "option.txt#Window", NULL, 0, 0);
#endif


				Term_clear(); 
				break;
			}

			default:
			{
				d = get_keymap_dir(ch);

				x = (x + ddx[d] + 8) % 8;
				y = (y + ddy[d] + 16) % 16;

				if (!d) bell();
			}
		}
	}

	/* Notice changes */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* Dead window */
		if (!angband_term[j]) continue;

		/* Ignore non-changes */
		if (window_flag[j] == old_flag[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Erase */
		Term_clear();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



#define OPT_NUM 15

static struct opts
{
	char key;
	cptr name;
	int row;
}
option_fields[OPT_NUM] =
{
#ifdef JP
	{ '1', "    �L�[����     �I�v�V����", 3 },
	{ '2', "   �}�b�v���    �I�v�V����", 4 },
	{ '3', "  �e�L�X�g�\��   �I�v�V����", 5 },
	{ '4', "  �Q�[���v���C   �I�v�V����", 6 },
	{ '5', "  �s�����~�֌W   �I�v�V����", 7 },
	{ '6', "  �ȈՎ����j��   �I�v�V����", 8 },
	{ 'r', "   �v���C�L�^    �I�v�V����", 9 },

	{ 'p', "�����E���G�f�B�^", 11 },
	{ 'd', " ��{�E�F�C�g�� ", 12 },
	{ 'h', "��q�b�g�|�C���g", 13 },
	{ 'm', "  �ᖂ�͐F臒l  ", 14 },
	{ 'a', "   �����Z�[�u    �I�v�V����", 15 },
	{ 'w', "�E�C���h�E�t���O", 16 },

	{ 'b', "      ����       �I�v�V���� (�Q�Ƃ̂�)", 18 },
	{ 'c', "      ���\       �I�v�V����", 19 },
#else
	{ '1', "Input Options", 3 },
	{ '2', "Map Screen Options", 4 },
	{ '3', "Text Display Options", 5 },
	{ '4', "Game-Play Options", 6 },
	{ '5', "Disturbance Options", 7 },
	{ '6', "Easy Auto-Destroyer Options", 8 },
	{ 'r', "Play record Options", 9 },

	{ 'p', "Auto-picker/destroyer editor", 11 },
	{ 'd', "Base Delay Factor", 12 },
	{ 'h', "Hitpoint Warning", 13 },
	{ 'm', "Mana Color Threshold", 14 },
	{ 'a', "Autosave Options", 15 },
	{ 'w', "Window Flags", 16 },

	{ 'b', "Birth Options (Browse Only)", 18 },
	{ 'c', "Cheat Options", 19 },
#endif
};


/*
 * Set or unset various options.
 *
 * The user must use the "Ctrl-R" command to "adapt" to changes
 * in any options which control "visual" aspects of the game.
 */
/*:::�I�v�V�����ݒ�@�ڍז���*/
///system�@�I�v�V�����ݒ�
void do_cmd_options(void)
{
	char k;
	int i, d, skey;
	int y = 0;

	/* Save the screen */
	screen_save();

	/* Interact */
	while (1)
	{
		int n = OPT_NUM;

		/* Does not list cheat option when cheat option is off */
		if (!p_ptr->noscore && !allow_debug_opts) n--;

		/* Clear screen */
		Term_clear();

		/* Why are we here */
#ifdef JP
		prt("[ �I�v�V�����̐ݒ� ]", 1, 0);
#else
		prt("TinyAngband options", 1, 0);
#endif

		while(1)
		{
			/* Give some choices */
			/*:::option_fields[][]���Q�Ƃ��ăI�v�V�������j���[��\���@�J�[�\���̂���s�͐F��ς���*/
			for (i = 0; i < n; i++)
			{
				byte a = TERM_WHITE;
				if (i == y) a = TERM_L_BLUE;
				Term_putstr(5, option_fields[i].row, -1, a, 
					format("(%c) %s", toupper(option_fields[i].key), option_fields[i].name));
			}

#ifdef JP
			prt("<����>�ňړ�, Enter�Ō���, ESC�ŃL�����Z��, ?�Ńw���v: ", 21, 0);
#else
			prt("Move to <dir>, Select to Enter, Cancel to ESC, ? to help: ", 21, 0);
#endif

			/* Get command */
			skey = inkey_special(TRUE);
			if (!(skey & SKEY_MASK)) k = (char)skey;
			else k = 0;

			/* Exit */
			if (k == ESCAPE) break;

			if (my_strchr("\n\r ", k))
			{
				k = option_fields[y].key;
				break;
			}

			for (i = 0; i < n; i++)
			{
				if (tolower(k) == option_fields[i].key) break;
			}

			/* Command is found */
			if (i < n) break;

			/* Hack -- browse help */
			if (k == '?') break;

			/* Move cursor */
			d = 0;
			if (skey == SKEY_UP) d = 8;
			if (skey == SKEY_DOWN) d = 2;
			y = (y + ddy[d] + n) % n;
			if (!d) bell();
		}

		/* Exit */
		if (k == ESCAPE) break;

		/* Analyze */
		switch (k)
		{
			case '1':
			{
				/* Process the general options */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_INPUT, "�L�[���̓I�v�V����");
#else
				do_cmd_options_aux(OPT_PAGE_INPUT, "Input Options");
#endif
				break;
			}

			case '2':
			{
				/* Process the general options */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_MAPSCREEN, "�}�b�v��ʃI�v�V����");
#else
				do_cmd_options_aux(OPT_PAGE_MAPSCREEN, "Map Screen Options");
#endif
				break;
			}

			case '3':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_TEXT, "�e�L�X�g�\���I�v�V����");
#else
				do_cmd_options_aux(OPT_PAGE_TEXT, "Text Display Options");
#endif
				break;
			}

			case '4':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_GAMEPLAY, "�Q�[���v���C�E�I�v�V����");
#else
				do_cmd_options_aux(OPT_PAGE_GAMEPLAY, "Game-Play Options");
#endif
				break;
			}

			case '5':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_DISTURBANCE, "�s�����~�֌W�̃I�v�V����");
#else
				do_cmd_options_aux(OPT_PAGE_DISTURBANCE, "Disturbance Options");
#endif
				break;
			}

			case '6':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_AUTODESTROY, "�ȈՎ����j��I�v�V����");
#else
				do_cmd_options_aux(OPT_PAGE_AUTODESTROY, "Easy Auto-Destroyer Options");
#endif
				break;
			}

			/* Play-record Options */
			case 'R':
			case 'r':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_PLAYRECORD, "�v���C�L�^�I�v�V����");
#else
				do_cmd_options_aux(OPT_PAGE_PLAYRECORD, "Play-record Options");
#endif
				break;
			}

			/* Birth Options */
			case 'B':
			case 'b':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_BIRTH, (!p_ptr->wizard || !allow_debug_opts) ? "�����I�v�V����(�Q�Ƃ̂�)" : "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
				do_cmd_options_aux(OPT_PAGE_BIRTH, (!p_ptr->wizard || !allow_debug_opts) ? "Birth Options(browse only)" : "Birth Options((*)s effect score)");
#endif
				break;
			}

			/* Cheating Options */
			case 'C':
			{
				if (!p_ptr->noscore && !allow_debug_opts)
				{
					/* Cheat options are not permitted */
					bell();
					break;
				}

				/* Spawn */
#ifdef JP
				do_cmd_options_cheat("���\�t�͌����ď����ł��Ȃ��I");
#else
				do_cmd_options_cheat("Cheaters never win");
#endif
				break;
			}

			case 'a':
			case 'A':
			{
#ifdef JP
				do_cmd_options_autosave("�����Z�[�u");
#else
				do_cmd_options_autosave("Autosave");
#endif
				break;
			}

			/* Window flags */
			case 'W':
			case 'w':
			{
				/* Spawn */
				do_cmd_options_win();
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL |
						  PW_PLAYER | PW_MESSAGE | PW_OVERHEAD |
							PW_MONSTER | PW_OBJECT | PW_SNAPSHOT |
							PW_BORG_1 | PW_BORG_2 | PW_DUNGEON   |
							PW_MONSTER_LIST | PW_FLOOR_ITEM_LIST);
				break;
			}

			/* Auto-picker/destroyer editor */
			case 'P':
			case 'p':
			{
				do_cmd_edit_autopick();
				break;
			}

			/* Hack -- Delay Speed */
			case 'D':
			case 'd':
			{
				/* Prompt */
				clear_from(18);
#ifdef JP
				prt("�R�}���h: ��{�E�F�C�g��", 19, 0);
#else
				prt("Command: Base Delay Factor", 19, 0);
#endif

				/* Get a new value */
				while (1)
				{
					int msec = delay_factor * delay_factor * delay_factor;
#ifdef JP
					prt(format("���݂̃E�F�C�g: %d (%d�~���b)",
						   delay_factor, msec), 22, 0);
#else
					prt(format("Current base delay factor: %d (%d msec)",
						   delay_factor, msec), 22, 0);
#endif

#ifdef JP
					prt("�E�F�C�g (0-9) ESC�Ō���: ", 20, 0);
#else
					prt("Delay Factor (0-9 or ESC to accept): ", 20, 0);
#endif

					k = inkey();
					if (k == ESCAPE) break;
					else if (k == '?')
					{
#ifdef JP
						(void)show_file(TRUE, "toption.txt#BaseDelay", NULL, 0, 0);
#else
						(void)show_file(TRUE, "option.txt#BaseDelay", NULL, 0, 0);
#endif
						Term_clear(); 
					}
					else if (isdigit(k)) delay_factor = D2I(k);
					else bell();
				}

				break;
			}

			/* Hack -- hitpoint warning factor */
			case 'H':
			case 'h':
			{
				/* Prompt */
				clear_from(18);
#ifdef JP
				prt("�R�}���h: ��q�b�g�|�C���g�x��", 19, 0);
#else
				prt("Command: Hitpoint Warning", 19, 0);
#endif

				/* Get a new value */
				while (1)
				{
#ifdef JP
					prt(format("���݂̒�q�b�g�|�C���g�x��: %d0%%",
						   hitpoint_warn), 22, 0);
#else
					prt(format("Current hitpoint warning: %d0%%",
						   hitpoint_warn), 22, 0);
#endif

#ifdef JP
					prt("��q�b�g�|�C���g�x�� (0-9) ESC�Ō���: ", 20, 0);
#else
					prt("Hitpoint Warning (0-9 or ESC to accept): ", 20, 0);
#endif

					k = inkey();
					if (k == ESCAPE) break;
					else if (k == '?')
					{
#ifdef JP
						(void)show_file(TRUE, "toption.txt#Hitpoint", NULL, 0, 0);
#else
						(void)show_file(TRUE, "option.txt#Hitpoint", NULL, 0, 0);
#endif
						Term_clear(); 
					}
					else if (isdigit(k)) hitpoint_warn = D2I(k);
					else bell();
				}

				break;
			}

			/* Hack -- mana color factor */
			case 'M':
			case 'm':
			{
				/* Prompt */
				clear_from(18);
#ifdef JP
				prt("�R�}���h: �ᖂ�͐F臒l", 19, 0);
#else
				prt("Command: Mana Color Threshold", 19, 0);
#endif

				/* Get a new value */
				while (1)
				{
#ifdef JP
					prt(format("���݂̒ᖂ�͐F臒l: %d0%%",
						   mana_warn), 22, 0);
#else
					prt(format("Current mana color threshold: %d0%%",
						   mana_warn), 22, 0);
#endif

#ifdef JP
					prt("�ᖂ��臒l (0-9) ESC�Ō���: ", 20, 0);
#else
					prt("Mana color Threshold (0-9 or ESC to accept): ", 20, 0);
#endif

					k = inkey();
					if (k == ESCAPE) break;
					else if (k == '?')
					{
#ifdef JP
						(void)show_file(TRUE, "toption.txt#Manapoint", NULL, 0, 0);
#else
						(void)show_file(TRUE, "option.txt#Manapoint", NULL, 0, 0);
#endif
						Term_clear(); 
					}
					else if (isdigit(k)) mana_warn = D2I(k);
					else bell();
				}

				break;
			}

			case '?':
#ifdef JP
				(void)show_file(TRUE, "toption.txt", NULL, 0, 0);
#else
				(void)show_file(TRUE, "option.txt", NULL, 0, 0);
#endif
				Term_clear(); 
				break;

			/* Unknown option */
			default:
			{
				/* Oops */
				bell();
				break;
			}
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	screen_load();

	/* Hack - Redraw equippy chars */
	p_ptr->redraw |= (PR_EQUIPPY);
}



/*
 * Ask for a "user pref line" and process it
 *
 * XXX XXX XXX Allow absolute file names?
 */
/*:::�ݒ�ύX�R�}���h�̓��͂��󂯂�*/
///sysdel �ݒ�ύX�R�}���h�@�����ɂ��Ă�����
void do_cmd_pref(void)
{
	char buf[80];

	/* Default */
	strcpy(buf, "");

	/* Ask for a "user pref command" */
#ifdef JP
	if (!get_string("�ݒ�ύX�R�}���h: ", buf, 80)) return;
#else
	if (!get_string("Pref: ", buf, 80)) return;
#endif


	/* Process that pref command */
	(void)process_pref_file_command(buf);
}

/*:::�����E���ݒ�t�@�C���̃��[�h*/
void do_cmd_reload_autopick(void)
{
#ifdef JP
	if (!get_check("�����E���ݒ�t�@�C�������[�h���܂���? ")) return;
#else
	if (!get_check("Reload auto-pick preference file? ")) return;
#endif

	/* Load the file with messages */
	autopick_load_pref(TRUE);
}

#ifdef ALLOW_MACROS

/*
 * Hack -- append all current macros to the given file
 */
static errr macro_dump(cptr fname)
{
	static cptr mark = "Macro Dump";

	int i;

	char buf[1024];

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	if (!open_auto_dump(buf, mark)) return (-1);

	/* Start dumping */
#ifdef JP
	auto_dump_printf("\n# �����}�N���Z�[�u\n\n");
#else
	auto_dump_printf("\n# Automatic macro dump\n\n");
#endif

	/* Dump them */
	for (i = 0; i < macro__num; i++)
	{
		/* Extract the action */
		ascii_to_text(buf, macro__act[i]);

		/* Dump the macro */
		auto_dump_printf("A:%s\n", buf);

		/* Extract the action */
		ascii_to_text(buf, macro__pat[i]);

		/* Dump normal macros */
		auto_dump_printf("P:%s\n", buf);

		/* End the macro */
		auto_dump_printf("\n");
	}

	/* Close */
	close_auto_dump();

	/* Success */
	return (0);
}


/*
 * Hack -- ask for a "trigger" (see below)
 *
 * Note the complex use of the "inkey()" function from "util.c".
 *
 * Note that both "flush()" calls are extremely important.
 */
static void do_cmd_macro_aux(char *buf)
{
	int i, n = 0;

	char tmp[1024];


	/* Flush */
	flush();

	/* Do not process macros */
	inkey_base = TRUE;

	/* First key */
	i = inkey();

	/* Read the pattern */
	while (i)
	{
		/* Save the key */
		buf[n++] = i;

		/* Do not process macros */
		inkey_base = TRUE;

		/* Do not wait for keys */
		inkey_scan = TRUE;

		/* Attempt to read a key */
		i = inkey();
	}

	/* Terminate */
	buf[n] = '\0';

	/* Flush */
	flush();


	/* Convert the trigger */
	ascii_to_text(tmp, buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);
}

#endif


/*
 * Hack -- ask for a keymap "trigger" (see below)
 *
 * Note that both "flush()" calls are extremely important.  This may
 * no longer be true, since "util.c" is much simpler now.  XXX XXX XXX
 */
static void do_cmd_macro_aux_keymap(char *buf)
{
	char tmp[1024];


	/* Flush */
	flush();


	/* Get a key */
	buf[0] = inkey();
	buf[1] = '\0';


	/* Convert to ascii */
	ascii_to_text(tmp, buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);


	/* Flush */
	flush();
}


/*
 * Hack -- append all keymaps to the given file
 */
static errr keymap_dump(cptr fname)
{
	static cptr mark = "Keymap Dump";
	int i;

	char key[1024];
	char buf[1024];

	int mode;

	/* Roguelike */
	if (rogue_like_commands)
	{
		mode = KEYMAP_MODE_ROGUE;
	}

	/* Original */
	else
	{
		mode = KEYMAP_MODE_ORIG;
	}


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	if (!open_auto_dump(buf, mark)) return -1;

	/* Start dumping */
#ifdef JP
	auto_dump_printf("\n# �����L�[�z�u�Z�[�u\n\n");
#else
	auto_dump_printf("\n# Automatic keymap dump\n\n");
#endif

	/* Dump them */
	for (i = 0; i < 256; i++)
	{
		cptr act;

		/* Loop up the keymap */
		act = keymap_act[mode][i];

		/* Skip empty keymaps */
		if (!act) continue;

		/* Encode the key */
		buf[0] = i;
		buf[1] = '\0';
		ascii_to_text(key, buf);

		/* Encode the action */
		ascii_to_text(buf, act);

		/* Dump the macro */
		auto_dump_printf("A:%s\n", buf);
		auto_dump_printf("C:%d:%s\n", mode, key);
	}

	/* Close */
	close_auto_dump();

	/* Success */
	return (0);
}



/*
 * Interact with "macros"
 *
 * Note that the macro "action" must be defined before the trigger.
 *
 * Could use some helpful instructions on this page.  XXX XXX XXX
 */
/*:::�}�N���ݒ�*/
void do_cmd_macros(void)
{
	int i;

	char tmp[1024];

	char buf[1024];

	int mode;


	/* Roguelike */
	if (rogue_like_commands)
	{
		mode = KEYMAP_MODE_ROGUE;
	}

	/* Original */
	else
	{
		mode = KEYMAP_MODE_ORIG;
	}

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save screen */
	screen_save();


	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Describe */
#ifdef JP
		prt("[ �}�N���̐ݒ� ]", 2, 0);
#else
		prt("Interact with Macros", 2, 0);
#endif



		/* Describe that action */
#ifdef JP
		prt("�}�N���s����(���������)���ɕ\������܂�:", 20, 0);
#else
		prt("Current action (if any) shown below:", 20, 0);
#endif


		/* Analyze the current action */
		ascii_to_text(buf, macro__buf);

		/* Display the current action */
		prt(buf, 22, 0);


		/* Selections */
#ifdef JP
		prt("(1) ���[�U�[�ݒ�t�@�C���̃��[�h", 4, 5);
#else
		prt("(1) Load a user pref file", 4, 5);
#endif

#ifdef ALLOW_MACROS
#ifdef JP
		prt("(2) �t�@�C���Ƀ}�N����ǉ�", 5, 5);
		prt("(3) �}�N���̊m�F", 6, 5);
		prt("(4) �}�N���̍쐬", 7, 5);
		prt("(5) �}�N���̍폜", 8, 5);
		prt("(6) �t�@�C���ɃL�[�z�u��ǉ�", 9, 5);
		prt("(7) �L�[�z�u�̊m�F", 10, 5);
		prt("(8) �L�[�z�u�̍쐬", 11, 5);
		prt("(9) �L�[�z�u�̍폜", 12, 5);
		prt("(0) �}�N���s���̓���", 13, 5);
#else
		prt("(2) Append macros to a file", 5, 5);
		prt("(3) Query a macro", 6, 5);
		prt("(4) Create a macro", 7, 5);
		prt("(5) Remove a macro", 8, 5);
		prt("(6) Append keymaps to a file", 9, 5);
		prt("(7) Query a keymap", 10, 5);
		prt("(8) Create a keymap", 11, 5);
		prt("(9) Remove a keymap", 12, 5);
		prt("(0) Enter a new action", 13, 5);
#endif

#endif /* ALLOW_MACROS */

		/* Prompt */
#ifdef JP
		prt("�R�}���h: ", 16, 0);
#else
		prt("Command: ", 16, 0);
#endif


		/* Get a command */
		i = inkey();

		/* Leave */
		if (i == ESCAPE) break;

		/* Load a 'macro' file */
		else if (i == '1')
		{
			errr err;

			/* Prompt */
#ifdef JP
			prt("�R�}���h: ���[�U�[�ݒ�t�@�C���̃��[�h", 16, 0);
#else
			prt("Command: Load a user pref file", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 18, 0);
#else
			prt("File: ", 18, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Ask for a file */
			if (!askfor(tmp, 80)) continue;

			/* Process the given filename */
			err = process_pref_file(tmp);
			if (-2 == err)
			{
#ifdef JP
				msg_format("�W���̐ݒ�t�@�C��'%s'��ǂݍ��݂܂����B", tmp);
#else
				msg_format("Loaded default '%s'.", tmp);
#endif
			}
			else if (err)
			{
				/* Prompt */
#ifdef JP
				msg_format("'%s'�̓ǂݍ��݂Ɏ��s���܂����I", tmp);
#else
				msg_format("Failed to load '%s'!");
#endif
			}
			else
			{
#ifdef JP
				msg_format("'%s'��ǂݍ��݂܂����B", tmp);
#else
				msg_format("Loaded '%s'.", tmp);
#endif
			}
		}

#ifdef ALLOW_MACROS

		/* Save macros */
		else if (i == '2')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: �}�N�����t�@�C���ɒǉ�����", 16, 0);
#else
			prt("Command: Append macros to a file", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 18, 0);
#else
			prt("File: ", 18, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Ask for a file */
			if (!askfor(tmp, 80)) continue;

			/* Dump the macros */
			(void)macro_dump(tmp);

			/* Prompt */
#ifdef JP
			msg_print("�}�N����ǉ����܂����B");
#else
			msg_print("Appended macros.");
#endif

		}

		/* Query a macro */
		else if (i == '3')
		{
			int k;

			/* Prompt */
#ifdef JP
			prt("�R�}���h: �}�N���̊m�F", 16, 0);
#else
			prt("Command: Query a macro", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�g���K�[�L�[: ", 18, 0);
#else
			prt("Trigger: ", 18, 0);
#endif


			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Acquire action */
			k = macro_find_exact(buf);

			/* Nothing found */
			if (k < 0)
			{
				/* Prompt */
#ifdef JP
				msg_print("���̃L�[�ɂ̓}�N���͒�`����Ă��܂���B");
#else
				msg_print("Found no macro.");
#endif

			}

			/* Found one */
			else
			{
				/* Obtain the action */
				strcpy(macro__buf, macro__act[k]);

				/* Analyze the current action */
				ascii_to_text(buf, macro__buf);

				/* Display the current action */
				prt(buf, 22, 0);

				/* Prompt */
#ifdef JP
				msg_print("�}�N�����m�F���܂����B");
#else
				msg_print("Found a macro.");
#endif

			}
		}

		/* Create a macro */
		else if (i == '4')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: �}�N���̍쐬", 16, 0);
#else
			prt("Command: Create a macro", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�g���K�[�L�[: ", 18, 0);
#else
			prt("Trigger: ", 18, 0);
#endif


			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Clear */
			clear_from(20);

			/* Help message */
#ifdef JP
			c_prt(TERM_L_RED, "�J�[�\���L�[�̍��E�ŃJ�[�\���ʒu���ړ��BBackspace��Delete�ňꕶ���폜�B", 22, 0);
#else
			c_prt(TERM_L_RED, "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char.", 22, 0);
#endif

			/* Prompt */
#ifdef JP
			prt("�}�N���s��: ", 20, 0);
#else
			prt("Action: ", 20, 0);
#endif


			/* Convert to text */
			ascii_to_text(tmp, macro__buf);

			/* Get an encoded action */
			if (askfor(tmp, 80))
			{
				/* Convert to ascii */
				text_to_ascii(macro__buf, tmp);

				/* Link the macro */
				macro_add(buf, macro__buf);

				/* Prompt */
#ifdef JP
				msg_print("�}�N����ǉ����܂����B");
#else
				msg_print("Added a macro.");
#endif

			}
		}

		/* Remove a macro */
		else if (i == '5')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: �}�N���̍폜", 16, 0);
#else
			prt("Command: Remove a macro", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�g���K�[�L�[: ", 18, 0);
#else
			prt("Trigger: ", 18, 0);
#endif


			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Link the macro */
			macro_add(buf, buf);

			/* Prompt */
#ifdef JP
			msg_print("�}�N�����폜���܂����B");
#else
			msg_print("Removed a macro.");
#endif

		}

		/* Save keymaps */
		else if (i == '6')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: �L�[�z�u���t�@�C���ɒǉ�����", 16, 0);
#else
			prt("Command: Append keymaps to a file", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 18, 0);
#else
			prt("File: ", 18, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Ask for a file */
			if (!askfor(tmp, 80)) continue;

			/* Dump the macros */
			(void)keymap_dump(tmp);

			/* Prompt */
#ifdef JP
			msg_print("�L�[�z�u��ǉ����܂����B");
#else
			msg_print("Appended keymaps.");
#endif

		}

		/* Query a keymap */
		else if (i == '7')
		{
			cptr act;

			/* Prompt */
#ifdef JP
			prt("�R�}���h: �L�[�z�u�̊m�F", 16, 0);
#else
			prt("Command: Query a keymap", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�����L�[: ", 18, 0);
#else
			prt("Keypress: ", 18, 0);
#endif


			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Look up the keymap */
			act = keymap_act[mode][(byte)(buf[0])];

			/* Nothing found */
			if (!act)
			{
				/* Prompt */
#ifdef JP
				msg_print("�L�[�z�u�͒�`����Ă��܂���B");
#else
				msg_print("Found no keymap.");
#endif

			}

			/* Found one */
			else
			{
				/* Obtain the action */
				strcpy(macro__buf, act);

				/* Analyze the current action */
				ascii_to_text(buf, macro__buf);

				/* Display the current action */
				prt(buf, 22, 0);

				/* Prompt */
#ifdef JP
				msg_print("�L�[�z�u���m�F���܂����B");
#else
				msg_print("Found a keymap.");
#endif

			}
		}

		/* Create a keymap */
		else if (i == '8')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: �L�[�z�u�̍쐬", 16, 0);
#else
			prt("Command: Create a keymap", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�����L�[: ", 18, 0);
#else
			prt("Keypress: ", 18, 0);
#endif


			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Clear */
			clear_from(20);

			/* Help message */
#ifdef JP
			c_prt(TERM_L_RED, "�J�[�\���L�[�̍��E�ŃJ�[�\���ʒu���ړ��BBackspace��Delete�ňꕶ���폜�B", 22, 0);
#else
			c_prt(TERM_L_RED, "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char.", 22, 0);
#endif

			/* Prompt */
#ifdef JP
			prt("�s��: ", 20, 0);
#else
			prt("Action: ", 20, 0);
#endif


			/* Convert to text */
			ascii_to_text(tmp, macro__buf);

			/* Get an encoded action */
			if (askfor(tmp, 80))
			{
				/* Convert to ascii */
				text_to_ascii(macro__buf, tmp);

				/* Free old keymap */
				string_free(keymap_act[mode][(byte)(buf[0])]);

				/* Make new keymap */
				keymap_act[mode][(byte)(buf[0])] = string_make(macro__buf);

				/* Prompt */
#ifdef JP
				msg_print("�L�[�z�u��ǉ����܂����B");
#else
				msg_print("Added a keymap.");
#endif

			}
		}

		/* Remove a keymap */
		else if (i == '9')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: �L�[�z�u�̍폜", 16, 0);
#else
			prt("Command: Remove a keymap", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�����L�[: ", 18, 0);
#else
			prt("Keypress: ", 18, 0);
#endif


			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Free old keymap */
			string_free(keymap_act[mode][(byte)(buf[0])]);

			/* Make new keymap */
			keymap_act[mode][(byte)(buf[0])] = NULL;

			/* Prompt */
#ifdef JP
			msg_print("�L�[�z�u���폜���܂����B");
#else
			msg_print("Removed a keymap.");
#endif

		}

		/* Enter a new action */
		else if (i == '0')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: �}�N���s���̓���", 16, 0);
#else
			prt("Command: Enter a new action", 16, 0);
#endif

			/* Clear */
			clear_from(20);

			/* Help message */
#ifdef JP
			c_prt(TERM_L_RED, "�J�[�\���L�[�̍��E�ŃJ�[�\���ʒu���ړ��BBackspace��Delete�ňꕶ���폜�B", 22, 0);
#else
			c_prt(TERM_L_RED, "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char.", 22, 0);
#endif

			/* Prompt */
#ifdef JP
			prt("�}�N���s��: ", 20, 0);
#else
			prt("Action: ", 20, 0);
#endif

			/* Hack -- limit the value */
			tmp[80] = '\0';

			/* Get an encoded action */
			if (!askfor(buf, 80)) continue;

			/* Extract an action */
			text_to_ascii(macro__buf, buf);
		}

#endif /* ALLOW_MACROS */

		/* Oops */
		else
		{
			/* Oops */
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Load screen */
	screen_load();
}


static cptr lighting_level_str[F_LIT_MAX] =
{
#ifdef JP
	"�W���F",
	"���F",
	"�ÐF",
#else
	"standard",
	"brightly lit",
	"darkened",
#endif
};


static bool cmd_visuals_aux(int i, int *num, int max)
{
	if (iscntrl(i))
	{
		char str[10] = "";
		int tmp;

		sprintf(str, "%d", *num);

		if (!get_string(format("Input new number(0-%d): ", max-1), str, 4))
			return FALSE;

		tmp = strtol(str, NULL, 0);
		if (tmp >= 0 && tmp < max)
			*num = tmp;
	}
	else if (isupper(i))
		*num = (*num + max - 1) % max;
	else
		*num = (*num + 1) % max;

	return TRUE;
}

static void print_visuals_menu(cptr choice_msg)
{
#ifdef JP
	prt("[ ��ʕ\���̐ݒ� ]", 1, 0);
#else
	prt("Interact with Visuals", 1, 0);
#endif

	/* Give some choices */
#ifdef JP
	prt("(0) ���[�U�[�ݒ�t�@�C���̃��[�h", 3, 5);
#else
	prt("(0) Load a user pref file", 3, 5);
#endif

#ifdef ALLOW_VISUALS
#ifdef JP
	prt("(1) �����X�^�[�� �F/���� ���t�@�C���ɏ����o��", 4, 5);
	prt("(2) �A�C�e����   �F/���� ���t�@�C���ɏ����o��", 5, 5);
	prt("(3) �n�`��       �F/���� ���t�@�C���ɏ����o��", 6, 5);
	prt("(4) �����X�^�[�� �F/���� ��ύX���� (���l����)", 7, 5);
	prt("(5) �A�C�e����   �F/���� ��ύX���� (���l����)", 8, 5);
	prt("(6) �n�`��       �F/���� ��ύX���� (���l����)", 9, 5);
	prt("(7) �����X�^�[�� �F/���� ��ύX���� (�V���{���G�f�B�^)", 10, 5);
	prt("(8) �A�C�e����   �F/���� ��ύX���� (�V���{���G�f�B�^)", 11, 5);
	prt("(9) �n�`��       �F/���� ��ύX���� (�V���{���G�f�B�^)", 12, 5);
#else
	prt("(1) Dump monster attr/chars", 4, 5);
	prt("(2) Dump object attr/chars", 5, 5);
	prt("(3) Dump feature attr/chars", 6, 5);
	prt("(4) Change monster attr/chars (numeric operation)", 7, 5);
	prt("(5) Change object attr/chars (numeric operation)", 8, 5);
	prt("(6) Change feature attr/chars (numeric operation)", 9, 5);
	prt("(7) Change monster attr/chars (visual mode)", 10, 5);
	prt("(8) Change object attr/chars (visual mode)", 11, 5);
	prt("(9) Change feature attr/chars (visual mode)", 12, 5);
#endif

#endif /* ALLOW_VISUALS */

#ifdef JP
	prt("(R) ��ʕ\�����@�̏�����", 13, 5);
#else
	prt("(R) Reset visuals", 13, 5);
#endif

	/* Prompt */
#ifdef JP
	prt(format("�R�}���h: %s", choice_msg ? choice_msg : ""), 15, 0);
#else
	prt(format("Command: %s", choice_msg ? choice_msg : ""), 15, 0);
#endif
}

static void do_cmd_knowledge_monsters(bool *need_redraw, bool visual_only, int direct_r_idx);
static void do_cmd_knowledge_objects(bool *need_redraw, bool visual_only, int direct_k_idx);
static void do_cmd_knowledge_features(bool *need_redraw, bool visual_only, int direct_f_idx, int *lighting_level);

/*
 * Interact with "visuals"
 */
/*:::�`��ݒ�@�ڍז���*/
///sysdel �`��ݒ�@�Ƃ肠���������ɂ��Ă����\��
void do_cmd_visuals(void)
{
	int i;
	char tmp[160];
	char buf[1024];
	bool need_redraw = FALSE;
	const char *empty_symbol = "<< ? >>";

	if (use_bigtile) empty_symbol = "<< ?? >>";

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Save the screen */
	screen_save();

	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
		print_visuals_menu(NULL);

		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		switch (i)
		{
		/* Load a 'pref' file */
		case '0':
			/* Prompt */
#ifdef JP
			prt("�R�}���h: ���[�U�[�ݒ�t�@�C���̃��[�h", 15, 0);
#else
			prt("Command: Load a user pref file", 15, 0);
#endif

			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 17, 0);
#else
			prt("File: ", 17, 0);
#endif

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Query */
			if (!askfor(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);

			need_redraw = TRUE;
			break;

#ifdef ALLOW_VISUALS

		/* Dump monster attr/chars */
		case '1':
		{
			static cptr mark = "Monster attr/chars";

			/* Prompt */
#ifdef JP
			prt("�R�}���h: �����X�^�[��[�F/����]���t�@�C���ɏ����o���܂�", 15, 0);
#else
			prt("Command: Dump monster attr/chars", 15, 0);
#endif

			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 17, 0);
#else
			prt("File: ", 17, 0);
#endif

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if (!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if (!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# �����X�^�[��[�F/����]�̐ݒ�\n\n");
#else
			auto_dump_printf("\n# Monster attr/char definitions\n\n");
#endif

			/* Dump monsters */
			for (i = 0; i < max_r_idx; i++)
			{
				monster_race *r_ptr = &r_info[i];

				/* Skip non-entries */
				if (!r_ptr->name) continue;

				/* Dump a comment */
				auto_dump_printf("# %s\n", (r_name + r_ptr->name));

				/* Dump the monster attr/char info */
				auto_dump_printf("R:%d:0x%02X/0x%02X\n\n", i,
					(byte)(r_ptr->x_attr), (byte)(r_ptr->x_char));
			}

			/* Close */
			close_auto_dump();

			/* Message */
#ifdef JP
			msg_print("�����X�^�[��[�F/����]���t�@�C���ɏ����o���܂����B");
#else
			msg_print("Dumped monster attr/chars.");
#endif

			break;
		}

		/* Dump object attr/chars */
		case '2':
		{
			static cptr mark = "Object attr/chars";

			/* Prompt */
#ifdef JP
			prt("�R�}���h: �A�C�e����[�F/����]���t�@�C���ɏ����o���܂�", 15, 0);
#else
			prt("Command: Dump object attr/chars", 15, 0);
#endif

			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 17, 0);
#else
			prt("File: ", 17, 0);
#endif

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if (!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if (!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# �A�C�e����[�F/����]�̐ݒ�\n\n");
#else
			auto_dump_printf("\n# Object attr/char definitions\n\n");
#endif

			/* Dump objects */
			for (i = 0; i < max_k_idx; i++)
			{
				char o_name[MAX_NLEN];
				object_kind *k_ptr = &k_info[i];

				/* Skip non-entries */
				if (!k_ptr->name) continue;

				if (!k_ptr->flavor)
				{
					/* Tidy name */
					strip_name(o_name, i);
				}
				else
				{
					object_type forge;

					/* Prepare dummy object */
					object_prep(&forge, i);

					/* Get un-shuffled flavor name */
					object_desc(o_name, &forge, OD_FORCE_FLAVOR);
				}

				/* Dump a comment */
				auto_dump_printf("# %s\n", o_name);

				/* Dump the object attr/char info */
				auto_dump_printf("K:%d:0x%02X/0x%02X\n\n", i,
					(byte)(k_ptr->x_attr), (byte)(k_ptr->x_char));
			}

			/* Close */
			close_auto_dump();

			/* Message */
#ifdef JP
			msg_print("�A�C�e����[�F/����]���t�@�C���ɏ����o���܂����B");
#else
			msg_print("Dumped object attr/chars.");
#endif

			break;
		}

		/* Dump feature attr/chars */
		case '3':
		{
			static cptr mark = "Feature attr/chars";

			/* Prompt */
#ifdef JP
			prt("�R�}���h: �n�`��[�F/����]���t�@�C���ɏ����o���܂�", 15, 0);
#else
			prt("Command: Dump feature attr/chars", 15, 0);
#endif

			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 17, 0);
#else
			prt("File: ", 17, 0);
#endif

			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if (!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if (!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# �n�`��[�F/����]�̐ݒ�\n\n");
#else
			auto_dump_printf("\n# Feature attr/char definitions\n\n");
#endif

			/* Dump features */
			for (i = 0; i < max_f_idx; i++)
			{
				feature_type *f_ptr = &f_info[i];

				/* Skip non-entries */
				if (!f_ptr->name) continue;

				/* Skip mimiccing features */
				if (f_ptr->mimic != i) continue;

				/* Dump a comment */
				auto_dump_printf("# %s\n", (f_name + f_ptr->name));

				/* Dump the feature attr/char info */
				auto_dump_printf("F:%d:0x%02X/0x%02X:0x%02X/0x%02X:0x%02X/0x%02X\n\n", i,
					(byte)(f_ptr->x_attr[F_LIT_STANDARD]), (byte)(f_ptr->x_char[F_LIT_STANDARD]),
					(byte)(f_ptr->x_attr[F_LIT_LITE]), (byte)(f_ptr->x_char[F_LIT_LITE]),
					(byte)(f_ptr->x_attr[F_LIT_DARK]), (byte)(f_ptr->x_char[F_LIT_DARK]));
			}

			/* Close */
			close_auto_dump();

			/* Message */
#ifdef JP
			msg_print("�n�`��[�F/����]���t�@�C���ɏ����o���܂����B");
#else
			msg_print("Dumped feature attr/chars.");
#endif

			break;
		}

		/* Modify monster attr/chars (numeric operation) */
		case '4':
		{
#ifdef JP
			static cptr choice_msg = "�����X�^�[��[�F/����]��ύX���܂�";
#else
			static cptr choice_msg = "Change monster attr/chars";
#endif
			static int r = 0;

#ifdef JP
			prt(format("�R�}���h: %s", choice_msg), 15, 0);
#else
			prt(format("Command: %s", choice_msg), 15, 0);
#endif

			/* Hack -- query until done */
			while (1)
			{
				monster_race *r_ptr = &r_info[r];
				char c;
				int t;

				byte da = r_ptr->d_attr;
				byte dc = r_ptr->d_char;
				byte ca = r_ptr->x_attr;
				byte cc = r_ptr->x_char;

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
					    format("�����X�^�[ = %d, ���O = %-40.40s",
						   r, (r_name + r_ptr->name)));
#else
				Term_putstr(5, 17, -1, TERM_WHITE,
					    format("Monster = %d, Name = %-40.40s",
						   r, (r_name + r_ptr->name)));
#endif

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("�����l  �F / ���� = %3u / %3u", da, dc));
#else
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("Default attr/char = %3u / %3u", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 19, da, dc, 0, 0);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("���ݒl  �F / ���� = %3u / %3u", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("Current attr/char = %3u / %3u", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 20, ca, cc, 0, 0);

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
					    "�R�}���h (n/N/^N/a/A/^A/c/C/^C/v/V/^V): ");
#else
				Term_putstr(0, 22, -1, TERM_WHITE,
					    "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V): ");
#endif

				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				if (iscntrl(i)) c = 'a' + i - KTRL('A');
				else if (isupper(i)) c = 'a' + i - 'A';
				else c = i;

				switch (c)
				{
				case 'n':
					{
						int prev_r = r;
						do
						{
							if (!cmd_visuals_aux(i, &r, max_r_idx))
							{
								r = prev_r;
								break;
							}
						}
						while (!r_info[r].name);
					}
					break;
				case 'a':
					t = (int)r_ptr->x_attr;
					(void)cmd_visuals_aux(i, &t, 256);
					r_ptr->x_attr = (byte)t;
					need_redraw = TRUE;
					break;
				case 'c':
					t = (int)r_ptr->x_char;
					(void)cmd_visuals_aux(i, &t, 256);
					r_ptr->x_char = (byte)t;
					need_redraw = TRUE;
					break;
				case 'v':
					do_cmd_knowledge_monsters(&need_redraw, TRUE, r);

					/* Clear screen */
					Term_clear();
					print_visuals_menu(choice_msg);
					break;
				}
			}

			break;
		}

		/* Modify object attr/chars (numeric operation) */
		case '5':
		{
#ifdef JP
			static cptr choice_msg = "�A�C�e����[�F/����]��ύX���܂�";
#else
			static cptr choice_msg = "Change object attr/chars";
#endif
			static int k = 0;

#ifdef JP
			prt(format("�R�}���h: %s", choice_msg), 15, 0);
#else
			prt(format("Command: %s", choice_msg), 15, 0);
#endif

			/* Hack -- query until done */
			while (1)
			{
				object_kind *k_ptr = &k_info[k];
				char c;
				int t;

				byte da = k_ptr->d_attr;
				byte dc = k_ptr->d_char;
				byte ca = k_ptr->x_attr;
				byte cc = k_ptr->x_char;

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
					    format("�A�C�e�� = %d, ���O = %-40.40s",
						   k, k_name + (!k_ptr->flavor ? k_ptr->name : k_ptr->flavor_name)));
#else
				Term_putstr(5, 17, -1, TERM_WHITE,
					    format("Object = %d, Name = %-40.40s",
						   k, k_name + (!k_ptr->flavor ? k_ptr->name : k_ptr->flavor_name)));
#endif

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("�����l  �F / ���� = %3d / %3d", da, dc));
#else
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("Default attr/char = %3d / %3d", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 19, da, dc, 0, 0);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("���ݒl  �F / ���� = %3d / %3d", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("Current attr/char = %3d / %3d", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 20, ca, cc, 0, 0);

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
					    "�R�}���h (n/N/^N/a/A/^A/c/C/^C/v/V/^V): ");
#else
				Term_putstr(0, 22, -1, TERM_WHITE,
					    "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V): ");
#endif

				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				if (iscntrl(i)) c = 'a' + i - KTRL('A');
				else if (isupper(i)) c = 'a' + i - 'A';
				else c = i;

				switch (c)
				{
				case 'n':
					{
						int prev_k = k;
						do
						{
							if (!cmd_visuals_aux(i, &k, max_k_idx))
							{
								k = prev_k;
								break;
							}
						}
						while (!k_info[k].name);
					}
					break;
				case 'a':
					t = (int)k_ptr->x_attr;
					(void)cmd_visuals_aux(i, &t, 256);
					k_ptr->x_attr = (byte)t;
					need_redraw = TRUE;
					break;
				case 'c':
					t = (int)k_ptr->x_char;
					(void)cmd_visuals_aux(i, &t, 256);
					k_ptr->x_char = (byte)t;
					need_redraw = TRUE;
					break;
				case 'v':
					do_cmd_knowledge_objects(&need_redraw, TRUE, k);

					/* Clear screen */
					Term_clear();
					print_visuals_menu(choice_msg);
					break;
				}
			}

			break;
		}

		/* Modify feature attr/chars (numeric operation) */
		case '6':
		{
#ifdef JP
			static cptr choice_msg = "�n�`��[�F/����]��ύX���܂�";
#else
			static cptr choice_msg = "Change feature attr/chars";
#endif
			static int f = 0;
			static int lighting_level = F_LIT_STANDARD;

#ifdef JP
			prt(format("�R�}���h: %s", choice_msg), 15, 0);
#else
			prt(format("Command: %s", choice_msg), 15, 0);
#endif

			/* Hack -- query until done */
			while (1)
			{
				feature_type *f_ptr = &f_info[f];
				char c;
				int t;

				byte da = f_ptr->d_attr[lighting_level];
				byte dc = f_ptr->d_char[lighting_level];
				byte ca = f_ptr->x_attr[lighting_level];
				byte cc = f_ptr->x_char[lighting_level];

				/* Label the object */
				prt("", 17, 5);
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
					    format("�n�` = %d, ���O = %s, ���x = %s",
						   f, (f_name + f_ptr->name), lighting_level_str[lighting_level]));
#else
				Term_putstr(5, 17, -1, TERM_WHITE,
					    format("Terrain = %d, Name = %s, Lighting = %s",
						   f, (f_name + f_ptr->name), lighting_level_str[lighting_level]));
#endif

				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("�����l  �F / ���� = %3d / %3d", da, dc));
#else
				Term_putstr(10, 19, -1, TERM_WHITE,
					    format("Default attr/char = %3d / %3d", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, empty_symbol);

				Term_queue_bigchar(43, 19, da, dc, 0, 0);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("���ݒl  �F / ���� = %3d / %3d", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
					    format("Current attr/char = %3d / %3d", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, empty_symbol);
				Term_queue_bigchar(43, 20, ca, cc, 0, 0);

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
					    "�R�}���h (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): ");
#else
				Term_putstr(0, 22, -1, TERM_WHITE,
					    "Command (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): ");
#endif

				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				if (iscntrl(i)) c = 'a' + i - KTRL('A');
				else if (isupper(i)) c = 'a' + i - 'A';
				else c = i;

				switch (c)
				{
				case 'n':
					{
						int prev_f = f;
						do
						{
							if (!cmd_visuals_aux(i, &f, max_f_idx))
							{
								f = prev_f;
								break;
							}
						}
						while (!f_info[f].name || (f_info[f].mimic != f));
					}
					break;
				case 'a':
					t = (int)f_ptr->x_attr[lighting_level];
					(void)cmd_visuals_aux(i, &t, 256);
					f_ptr->x_attr[lighting_level] = (byte)t;
					need_redraw = TRUE;
					break;
				case 'c':
					t = (int)f_ptr->x_char[lighting_level];
					(void)cmd_visuals_aux(i, &t, 256);
					f_ptr->x_char[lighting_level] = (byte)t;
					need_redraw = TRUE;
					break;
				case 'l':
					(void)cmd_visuals_aux(i, &lighting_level, F_LIT_MAX);
					break;
				case 'd':
					apply_default_feat_lighting(f_ptr->x_attr, f_ptr->x_char);
					need_redraw = TRUE;
					break;
				case 'v':
					do_cmd_knowledge_features(&need_redraw, TRUE, f, &lighting_level);

					/* Clear screen */
					Term_clear();
					print_visuals_menu(choice_msg);
					break;
				}
			}

			break;
		}

		/* Modify monster attr/chars (visual mode) */
		case '7':
			do_cmd_knowledge_monsters(&need_redraw, TRUE, -1);
			break;

		/* Modify object attr/chars (visual mode) */
		case '8':
			do_cmd_knowledge_objects(&need_redraw, TRUE, -1);
			break;

		/* Modify feature attr/chars (visual mode) */
		case '9':
		{
			int lighting_level = F_LIT_STANDARD;
			do_cmd_knowledge_features(&need_redraw, TRUE, -1, &lighting_level);
			break;
		}

#endif /* ALLOW_VISUALS */

		/* Reset visuals */
		case 'R':
		case 'r':
			/* Reset */
			reset_visuals();

			/* Message */
#ifdef JP
			msg_print("��ʏ��[�F/����]�������l�Ƀ��Z�b�g���܂����B");
#else
			msg_print("Visual attr/char tables reset.");
#endif

			need_redraw = TRUE;
			break;

		/* Unknown option */
		default:
			bell();
			break;
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	screen_load();

	if (need_redraw) do_cmd_redraw();
}


/*
 * Interact with "colors"
 */
/*:::�F�ݒ�*/
///sys �F�ݒ�R�}���h�@�����ɂ��Ă����\��
void do_cmd_colors(void)
{
	int i;

	char tmp[160];

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save the screen */
	screen_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("[ �J���[�̐ݒ� ]", 2, 0);
#else
		prt("Interact with Colors", 2, 0);
#endif


		/* Give some choices */
#ifdef JP
		prt("(1) ���[�U�[�ݒ�t�@�C���̃��[�h", 4, 5);
#else
		prt("(1) Load a user pref file", 4, 5);
#endif

#ifdef ALLOW_COLORS
#ifdef JP
		prt("(2) �J���[�̐ݒ���t�@�C���ɏ����o��", 5, 5);
		prt("(3) �J���[�̐ݒ��ύX����", 6, 5);
#else
		prt("(2) Dump colors", 5, 5);
		prt("(3) Modify colors", 6, 5);
#endif

#endif

		/* Prompt */
#ifdef JP
		prt("�R�}���h: ", 8, 0);
#else
		prt("Command: ", 8, 0);
#endif


		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		/* Load a 'pref' file */
		if (i == '1')
		{
			/* Prompt */
#ifdef JP
			prt("�R�}���h: ���[�U�[�ݒ�t�@�C�������[�h���܂�", 8, 0);
#else
			prt("Command: Load a user pref file", 8, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 10, 0);
#else
			prt("File: ", 10, 0);
#endif


			/* Default file */
			sprintf(tmp, "%s.prf", player_base);

			/* Query */
			if (!askfor(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);

			/* Mega-Hack -- react to changes */
			Term_xtra(TERM_XTRA_REACT, 0);

			/* Mega-Hack -- redraw */
			Term_redraw();
		}

#ifdef ALLOW_COLORS

		/* Dump colors */
		else if (i == '2')
		{
			static cptr mark = "Colors";

			/* Prompt */
#ifdef JP
			prt("�R�}���h: �J���[�̐ݒ���t�@�C���ɏ����o���܂�", 8, 0);
#else
			prt("Command: Dump colors", 8, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�t�@�C��: ", 10, 0);
#else
			prt("File: ", 10, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_base);

			/* Get a filename */
			if (!askfor(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

			/* Append to the file */
			if (!open_auto_dump(buf, mark)) continue;

			/* Start dumping */
#ifdef JP
			auto_dump_printf("\n# �J���[�̐ݒ�\n\n");
#else
			auto_dump_printf("\n# Color redefinitions\n\n");
#endif

			/* Dump colors */
			for (i = 0; i < 256; i++)
			{
				int kv = angband_color_table[i][0];
				int rv = angband_color_table[i][1];
				int gv = angband_color_table[i][2];
				int bv = angband_color_table[i][3];

#ifdef JP
				cptr name = "���m";
#else
				cptr name = "unknown";
#endif


				/* Skip non-entries */
				if (!kv && !rv && !gv && !bv) continue;

				/* Extract the color name */
				if (i < 16) name = color_names[i];

				/* Dump a comment */
#ifdef JP
				auto_dump_printf("# �J���[ '%s'\n", name);
#else
				auto_dump_printf("# Color '%s'\n", name);
#endif

				/* Dump the monster attr/char info */
				auto_dump_printf("V:%d:0x%02X:0x%02X:0x%02X:0x%02X\n\n",
					i, kv, rv, gv, bv);
			}

			/* Close */
			close_auto_dump();

			/* Message */
#ifdef JP
			msg_print("�J���[�̐ݒ���t�@�C���ɏ����o���܂����B");
#else
			msg_print("Dumped color redefinitions.");
#endif

		}

		/* Edit colors */
		else if (i == '3')
		{
			static byte a = 0;

			/* Prompt */
#ifdef JP
			prt("�R�}���h: �J���[�̐ݒ��ύX���܂�", 8, 0);
#else
			prt("Command: Modify colors", 8, 0);
#endif


			/* Hack -- query until done */
			while (1)
			{
				cptr name;
				byte j;

				/* Clear */
				clear_from(10);

				/* Exhibit the normal colors */
				for (j = 0; j < 16; j++)
				{
					/* Exhibit this color */
					Term_putstr(j*4, 20, -1, a, "###");

					/* Exhibit all colors */
					Term_putstr(j*4, 22, -1, j, format("%3d", j));
				}

				/* Describe the color */
#ifdef JP
				name = ((a < 16) ? color_names[a] : "����`");
#else
				name = ((a < 16) ? color_names[a] : "undefined");
#endif


				/* Describe the color */
#ifdef JP
				Term_putstr(5, 10, -1, TERM_WHITE,
					    format("�J���[ = %d, ���O = %s", a, name));
#else
				Term_putstr(5, 10, -1, TERM_WHITE,
					    format("Color = %d, Name = %s", a, name));
#endif


				/* Label the Current values */
				Term_putstr(5, 12, -1, TERM_WHITE,
					    format("K = 0x%02x / R,G,B = 0x%02x,0x%02x,0x%02x",
						   angband_color_table[a][0],
						   angband_color_table[a][1],
						   angband_color_table[a][2],
						   angband_color_table[a][3]));

				/* Prompt */
#ifdef JP
				Term_putstr(0, 14, -1, TERM_WHITE,
					    "�R�}���h (n/N/k/K/r/R/g/G/b/B): ");
#else
				Term_putstr(0, 14, -1, TERM_WHITE,
					    "Command (n/N/k/K/r/R/g/G/b/B): ");
#endif


				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') a = (byte)(a + 1);
				if (i == 'N') a = (byte)(a - 1);
				if (i == 'k') angband_color_table[a][0] = (byte)(angband_color_table[a][0] + 1);
				if (i == 'K') angband_color_table[a][0] = (byte)(angband_color_table[a][0] - 1);
				if (i == 'r') angband_color_table[a][1] = (byte)(angband_color_table[a][1] + 1);
				if (i == 'R') angband_color_table[a][1] = (byte)(angband_color_table[a][1] - 1);
				if (i == 'g') angband_color_table[a][2] = (byte)(angband_color_table[a][2] + 1);
				if (i == 'G') angband_color_table[a][2] = (byte)(angband_color_table[a][2] - 1);
				if (i == 'b') angband_color_table[a][3] = (byte)(angband_color_table[a][3] + 1);
				if (i == 'B') angband_color_table[a][3] = (byte)(angband_color_table[a][3] - 1);

				/* Hack -- react to changes */
				Term_xtra(TERM_XTRA_REACT, 0);

				/* Hack -- redraw */
				Term_redraw();
			}
		}

#endif

		/* Unknown option */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	screen_load();
}


/*
 * Note something in the message recall
 */
void do_cmd_note(void)
{
	char buf[80];

	/* Default */
	strcpy(buf, "");

	/* Input */
#ifdef JP
	if (!get_string("����: ", buf, 60)) return;
#else
	if (!get_string("Note: ", buf, 60)) return;
#endif


	/* Ignore empty notes */
	if (!buf[0] || (buf[0] == ' ')) return;

	/* Add the note to the message recall */
#ifdef JP
	msg_format("����: %s", buf);
#else
	msg_format("Note: %s", buf);
#endif

}


/*
 * Mention the current version
 */
///\131117 system ver�ύX
void do_cmd_version(void)
{

	/* Silly message */
#ifdef JP
	msg_format("���z�ؓ{ %d.%d.%dT",
		    H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH);
#else
	msg_format("You are playing Hengband %d.%d.%d.",
		    FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#endif
}



/*
 * Array of feeling strings
 */
static cptr do_cmd_feeling_text[11] =
{
#ifdef JP
	"���̊K�̕��͋C�������Ƃ�Ȃ�����...",
#else
	"Looks like any other level.",
#endif

#ifdef JP
	"���̊K�ɂ͉������ʂȂ��̂�����悤�ȋC������B",
#else
	"You feel there is something special about this level.",
#endif

#ifdef JP
	"���̊K�ŉ����Ƃ�ł��Ȃ������N�����Ă���I",
	///msg131221
	//"���낵�����̌����ڂɕ����сA�C�₵�����ɂȂ����I",

#else
	"You nearly faint as horrible visions of death fill your mind!",
#endif

#ifdef JP
	"���̊K�͂ƂĂ��댯�Ȃ悤���B",
#else
	"This level looks very dangerous.",
#endif

#ifdef JP
	"�ƂĂ������\��������...",
#else
	"You have a very bad feeling...",
#endif

#ifdef JP
	"�����\��������...",
#else
	"You have a bad feeling...",
#endif

#ifdef JP
	"�����ْ�����B",
#else
	"You feel nervous.",
#endif

#ifdef JP
	"�����s�^�ȋC������...",
#else
	"You feel your luck is turning...",
#endif

#ifdef JP
	"���̏ꏊ�͍D���ɂȂ�Ȃ��B",
#else
	"You don't like the look of this place.",
#endif

#ifdef JP
	"���̊K�͂���Ȃ�Ɉ��S�Ȃ悤���B",
#else
	"This level looks reasonably safe.",
#endif

#ifdef JP
	"�Ȃ�đދ��ȂƂ��낾..."
#else
	"What a boring place..."
#endif

};

static cptr do_cmd_feeling_text_combat[11] =
{
#ifdef JP
	"���̊K�̕��͋C�������Ƃ�Ȃ�����...",
#else
	"Looks like any other level.",
#endif

#ifdef JP
	"���̊K�ɂ͉������ʂȂ��̂�����悤�ȋC������B",
#else
	"You feel there is something special about this level.",
#endif

#ifdef JP
	"������܂��A�N�������𗎂Ƃ�...",
#else
	"You nearly faint as horrible visions of death fill your mind!",
#endif

#ifdef JP
	"���̊K�͂ƂĂ��댯�Ȃ悤���B",
#else
	"This level looks very dangerous.",
#endif

#ifdef JP
	"�ƂĂ������\��������...",
#else
	"You have a very bad feeling...",
#endif

#ifdef JP
	"�����\��������...",
#else
	"You have a bad feeling...",
#endif

#ifdef JP
	"�����ْ�����B",
#else
	"You feel nervous.",
#endif

#ifdef JP
	"�����s�^�ȋC������...",
#else
	"You feel your luck is turning...",
#endif

#ifdef JP
	"���̏ꏊ�͍D���ɂȂ�Ȃ��B",
#else
	"You don't like the look of this place.",
#endif

#ifdef JP
	"���̊K�͂���Ȃ�Ɉ��S�Ȃ悤���B",
#else
	"This level looks reasonably safe.",
#endif

#ifdef JP
	"�Ȃ�đދ��ȂƂ��낾..."
#else
	"What a boring place..."
#endif

};

static cptr do_cmd_feeling_text_lucky[11] =
{
#ifdef JP
	"���̊K�̕��͋C�������Ƃ�Ȃ�����...",
	"���̊K�ɂ͉������ʂȂ��̂�����悤�ȋC������B",
	"���̊K�͂��̏�Ȃ��f���炵������������B",
	"�f���炵������������...",
	"�ƂĂ��ǂ�����������...",
	"�ǂ�����������...",
	"������ƍK�^�Ȋ���������...",
	"�����͉^�������Ă�����...",
	"�������������͂Ȃ�...",
	"�S�R�ʖڂƂ������Ƃ͂Ȃ���...",
	"�Ȃ�đދ��ȂƂ��낾..."
#else
	"Looks like any other level.",
	"You feel there is something special about this level.",
	"You have a superb feeling about this level.",
	"You have an excellent feeling...",
	"You have a very good feeling...",
	"You have a good feeling...",
	"You feel strangely lucky...",
	"You feel your luck is turning...",
	"You like the look of this place...",
	"This level can't be all bad...",
	"What a boring place..."
#endif
};


/*
 * Note that "feeling" is set to zero unless some time has passed.
 * Note that this is done when the level is GENERATED, not entered.
 */
/*:::���͋C�\������*/
///seikaku item �R���o�b�g���͋C�֘A��O�����L
void do_cmd_feeling(void)
{
	int j;
	/* No useful feeling in quests */
	if (p_ptr->inside_quest && !random_quest_number(dun_level))
	{
#ifdef JP
		///msg131220
		msg_print("�T�^�I�ȃN�G�X�g�̃_���W�����̂悤���B");
		//msg_print("�T�^�I�ȃN�G�X�g�̃_���W�����̂悤���B");
#else
		msg_print("Looks like a typical quest level.");
#endif

		return;
	}
	else if(EXTRA_QUEST_FLOOR)
	{
		msg_print("�T�^�I�ȃN�G�X�g�̃_���W�����̂悤���B");
	}

	/* No useful feeling in town */
	else if (p_ptr->town_num && !dun_level)
	{
#ifdef JP
		/*:::�̂̃N�G�X�g�p�̓��꒬�H*/
		if (!strcmp(town[p_ptr->town_num].name, "�r��"))
#else
		if (!strcmp(town[p_ptr->town_num].name, "wilderness"))
#endif
		{
#ifdef JP
			msg_print("�������肻���ȍr��̂悤���B");
#else
			msg_print("Looks like a strange wilderness.");
#endif

			return;
		}
		else
		{
#ifdef JP
			///sys �X�̕��͋C�@�푰�⁗�ɂ��u�F�D�I/�r���I�v�̃��b�Z�[�W���o���Ă���������
			msg_print("�T�^�I�Ȓ��̂悤���B");
#else
			msg_print("Looks like a typical town.");
#endif

			return;
		}
	}

	/* No useful feeling in the wilderness */
	else if (!dun_level)
	{
#ifdef JP
		msg_print("�T�^�I�ȍr��̂悤���B");
#else
		msg_print("Looks like a typical wilderness.");
#endif

		return;
	}
	///system�@muta ���I�[�����̊K�̕��͋C�@���g�����ς��悤
	/* Display the feeling */
	if (p_ptr->muta3 & MUT3_GOOD_LUCK)
		msg_print(do_cmd_feeling_text_lucky[p_ptr->feeling]);
///del131211 �R���o�b�gmsg
	//else if (p_ptr->pseikaku == SEIKAKU_COMBAT ||
	//	 inventory[INVEN_BOW].name1 == ART_CRIMSON)
	//	msg_print(do_cmd_feeling_text_combat[p_ptr->feeling]);
	else
		msg_print(do_cmd_feeling_text[p_ptr->feeling]);
	///mod140611 �܋���p
	for (j = 1; j < m_max; j++) 
	{
		monster_race *r_ptr = &r_info[m_list[j].r_idx];
		//v1.1.48 �������Ɏ��߂��ꂽ������������Ƃ����ʒm���邱�Ƃɂ���
		if(r_ptr->flags3 & RF3_WANTED || r_ptr->flags2 & RF2_TROPHY) msg_print("���̊K�ɂ��q�ˎ҂�����悤�ȋC������...");
	}

}



/*
 * Description of each monster group.
 */
/*:::�O���[�v������郂���X�^�[�V���{���̕\�L�@monster_group_char[]�ƈ�v���邱��*/
///mon sys �����X�^�[�V���{���E�O���[�v
///mod131231 �V���{���ύX�ɔ���
static cptr monster_group_text[] = 
{
#ifdef JP
	"���j�[�N",	/* "Uniques" */
	"��n�\�ȃ����X�^�[",	/* "Riding" */
	///del131221 �܋���
	//"�܋���", /* "Wanted */
	"�A���o�[�̉���", /* "Ambertite" */
	"�A��",
	"�R�E����",
	"�N���E���J�f��",
	"�h���S��",
	"�ڋ�",
	"�L�E�L�^�b�l",
	"�S�[�����E�@�B",
	"�W���l�Ԍ^����",
	"�x�g�x�g",
	"�[���[",
	"���^�b�l",
	"��������",
	"�����h",
	"�i�[�K",
	"�I�[�N",
	"�l��",
	"�l���b",
	"�l�Y�~",
	"�X�P���g��",
	"�f�[����",
	"�{���e�b�N�X",
	"�C�����V/��Q",
	/* "unused", */
	"���^�d��",
	"�]���r/�~�C��",
	"�V�g",
	"��",
	"��",
	/* "�Ñ�h���S��/���C�A�[��", */
	"�G�������^��",
	"�d��",
	"�S�[�X�g",
	"�G��",
	"����",
	"�w�r",
	"�͓��E�V��",
	"���b�`",
	"�����঒���",
	"��̐���",
	"�I�[�K�E�S",
	"����l�Ԍ^����",
	"�N�C���X���O",
	"঒���/������",
	"�_�i",
	"�g����",
	/* "�㋉�f�[����", */
	"�z���S",
	"���C�g/���C�X/��",
	"�]�[��/�U����/��",
	"��^�d���E�l�^�d��",
	"�[�t�@�[�n�E���h",
	"�~�~�b�N",
	"��/�A��/�C��",
	"���΂��L�m�R",
	"����",
	"�v���C���[",
#else
	"Uniques",
	"Ridable monsters",
	"Wanted monsters",
	"Ambertite",
	"Ant",
	"Bat",
	"Centipede",
	"Dragon",
	"Floating Eye",
	"Feline",
	"Golem",
	"Hobbit/Elf/Dwarf",
	"Icky Thing",
	"Jelly",
	"Kobold",
	"Aquatic monster",
	"Mold",
	"Naga",
	"Orc",
	"Person/Human",
	"Quadruped",
	"Rodent",
	"Skeleton",
	"Demon",
	"Vortex",
	"Worm/Worm-Mass",
	/* "unused", */
	"Yeek",
	"Zombie/Mummy",
	"Angel",
	"Bird",
	"Canine",
	/* "Ancient Dragon/Wyrm", */
	"Elemental",
	"Dragon Fly",
	"Ghost",
	"Hybrid",
	"Insect",
	"Snake",
	"Killer Beetle",
	"Lich",
	"Multi-Headed Reptile",
	"Mystery Living",
	"Ogre",
	"Giant Humanoid",
	"Quylthulg",
	"Reptile/Amphibian",
	"Spider/Scorpion/Tick",
	"Troll",
	/* "Major Demon", */
	"Vampire",
	"Wight/Wraith/etc",
	"Xorn/Xaren/etc",
	"Yeti",
	"Zephyr Hound",
	"Mimic",
	"Wall/Plant/Gas",
	"Mushroom patch",
	"Ball",
	"Player",
#endif
	NULL
};


/*
 * Symbols of monsters in each group. Note the "Uniques" group
 * is handled differently.
 */
static cptr monster_group_char[] =
{
	(char *) -1L,
	(char *) -2L,
	///del131221 �܋���
	//	(char *) -3L,
	(char *) -4L,
	"a",
	"b",
	"c",
	"dD",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"uU",
	"v",
	"w",
	/* "x", */
	"y",
	"z",
	"A",
	"B",
	"C",
	/* "D", */
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"Kt",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	/* "U", */
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"!$&()+./=>?[\\]`{|~",
	"#%",
	",",
	"*",
	"@",
	NULL
};


/*
 * hook function to sort monsters by level
 */
static bool ang_sort_comp_monster_level(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	monster_race *r_ptr1 = &r_info[w1];
	monster_race *r_ptr2 = &r_info[w2];

	/* Unused */
	(void)v;

	if (r_ptr2->level > r_ptr1->level) return TRUE;
	if (r_ptr1->level > r_ptr2->level) return FALSE;

	if ((r_ptr2->flags1 & RF1_UNIQUE) && !(r_ptr1->flags1 & RF1_UNIQUE)) return TRUE;
	if ((r_ptr1->flags1 & RF1_UNIQUE) && !(r_ptr2->flags1 & RF1_UNIQUE)) return FALSE;
	return w1 <= w2;
}

/*
 * Build a list of monster indexes in the given group. Return the number
 * of monsters in the group.
 *
 * mode & 0x01 : check for non-empty group
 * mode & 0x02 : visual operation only
 */
static int collect_monsters(int grp_cur, s16b mon_idx[], byte mode)
{
	int i, mon_cnt = 0;
	int dummy_why;

	/* Get a list of x_char in this group */
	cptr group_char = monster_group_char[grp_cur];

	/* XXX Hack -- Check if this is the "Uniques" group */
	bool grp_unique = (monster_group_char[grp_cur] == (char *) -1L);

	/* XXX Hack -- Check if this is the "Riding" group */
	bool grp_riding = (monster_group_char[grp_cur] == (char *) -2L);

	///sysdel �܋���������X�^�[�ꗗ�ŕ\�������邽�߂̏����֘A
	/* XXX Hack -- Check if this is the "Wanted" group */
	///del131221 �܋���
	//bool grp_wanted = (monster_group_char[grp_cur] == (char *) -3L);

	/* XXX Hack -- Check if this is the "Amberite" group */
	bool grp_amberite = (monster_group_char[grp_cur] == (char *) -4L);


	/* Check every race */
	for (i = 0; i < max_r_idx; i++)
	{
		/* Access the race */
		monster_race *r_ptr = &r_info[i];

		/* Skip empty race */
		if (!r_ptr->name) continue ;

		///mod151105 ���ꃂ���X�^�[�ނ͔�Ώ�
		if(r_ptr->flags7 & RF7_VARIABLE) continue;
		if(i == MON_EXTRA_FIELD) continue;

		//v1.1.92 俎q(���Z���p���ꃂ���X�^�[)�����O
		if (i == MON_SUMIREKO_2) continue;


		/* Require known monsters */
		if (!(mode & 0x02) && !cheat_know && !r_ptr->r_sights) continue;

		if (grp_unique)
		{
			if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;
		}

		else if (grp_riding)
		{
			if (!(r_ptr->flags7 & RF7_RIDING)) continue;
		}
		///del131221 �܋���
/*
		else if (grp_wanted)
		{
			bool wanted = FALSE;
			int j;
			for (j = 0; j < MAX_KUBI; j++)
			{
				if (kubi_r_idx[j] == i || kubi_r_idx[j] - 10000 == i ||
					(p_ptr->today_mon && p_ptr->today_mon == i))
				{
					wanted = TRUE;
					break;
				}
			}
			if (!wanted) continue;
		}
*/		
		else if (grp_amberite)
		{
			if (!(r_ptr->flags3 & RF3_ANG_AMBER)) continue;
		}

		else
		{
			/* Check for race in the group */
			if (!my_strchr(group_char, r_ptr->d_char)) continue;
		}

		/* Add the race */
		mon_idx[mon_cnt++] = i;

		/* XXX Hack -- Just checking for non-empty group */
		if (mode & 0x01) break;
	}

	/* Terminate the list */
	mon_idx[mon_cnt] = -1;

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_monster_level;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort by monster level */
	ang_sort(mon_idx, &dummy_why, mon_cnt);

	/* Return the number of races */
	return mon_cnt;
}


/*
 * Description of each monster group.
 */
///mod131223
///item TVAL �A�C�e����TVAL���Ƃ̊ȈՕ\�L�@����object_group_tval[]�Ə��Ԃ���v���Ă���K�v������͂�
static cptr object_group_text[] = 
{
#ifdef JP
	"�L�m�R",	/* "Mushrooms" */
	"�H��",
	"�َq",
	"���ݕ�",
	"��",
	"�f��",
	"���i�E�L�O�i",
	"��",		/* "Potions" */
	"����",	/* "Flasks" */
	"����",		/* "Scrolls" */
	"�w��",		/* "Rings" */
	"�A�~�����b�g",	/* "Amulets" */
	"�J",		/* "Whistle" */
	"����",		/* "Lanterns" */
	"���@�_",	/* "Wands" */
	"��",		/* "Staffs" */
	"���b�h",	/* "Rods" */
	"�@�B",
	"�d���{",
	"�ʐ^",
	"��",
	"�l�`",
	"�Z��",
	"����",
	"��",
	"�݊�",
	"�_",
	"��",
	"��",
	"��������",
	"�|",	/* "Bows" */
	"�N���X�{�E",
	"������",
	"������",
	"���̑�����",
	"�e",
	"�e��",
	"��",
	"�{���g",
	"��",	/* "Soft Armor" */
	"�Z",	/* "Hard Armor" */
	"�h���S���Z",	/* "Dragon Armor" */
	"��",	/* "Shields" */
	"���{��",
	"�N���[�N",	/* "Cloaks" */
	"�Ď�",	/* "Gloves" */
	"�X�q�E��",
	"�u�[�c",	/* "Boots" */
	"���@��",
	"����",
	"����",
#else
	"Mushrooms",
	"Potions",
	"Flasks",
	"Scrolls",
	"Rings",
	"Amulets",
	"Whistle",
	"Lanterns",
	"Wands",
	"Staves",
	"Rods",
	"Cards",
	"Capture Balls",
	"Parchments",
	"Spikes",
	"Boxs",
	//"Figurines",
	"Statues",
	"Junks",
	"Bottles",
	//"Skeletons",
	//"Corpses",
	"Swords",
	"Blunt Weapons",
	"Polearms",
	"Diggers",
	"Bows",
	"Shots",
	"Arrows",
	"Bolts",
	"Soft Armor",
	"Hard Armor",
	"Dragon Armor",
	"Shields",
	"Cloaks",
	"Gloves",
	"Helms",
	"Crowns",
	"Boots",
	"Spellbooks",
	"Treasure",
	"Something",
#endif
	NULL
};


/*
 * TVALs of items in each group
 */
static byte object_group_tval[] = 
{
	TV_MUSHROOM,
	TV_FOOD,
	TV_SWEETS,
	TV_SOFTDRINK,
	TV_ALCOHOL,
	TV_MATERIAL,
	TV_SOUVENIR,
	TV_POTION,
	TV_FLASK,
	TV_SCROLL,
	TV_RING,
	TV_AMULET,
	TV_WHISTLE,
	TV_LITE,
	TV_WAND,
	TV_STAFF,
	TV_ROD,
	TV_MACHINE,
	TV_CAPTURE,
	TV_PHOTO,
	TV_CHEST,
	TV_FIGURINE,
	TV_KNIFE,
	TV_SWORD,
	TV_KATANA,
	TV_HAMMER,
	TV_STICK,
	TV_AXE,
	TV_SPEAR,
	TV_POLEARM,
	TV_BOW,
	TV_CROSSBOW,
	TV_MAGICITEM,
	TV_MAGICWEAPON,
	TV_OTHERWEAPON,
	TV_GUN,
	TV_BULLET,
	TV_ARROW,
	TV_BOLT,
	TV_CLOTHES,
	TV_ARMOR,
	TV_DRAG_ARMOR,
	TV_SHIELD,
	TV_RIBBON,
	TV_CLOAK,
	TV_GLOVES,
	TV_HEAD,
	TV_BOOTS,
	//TV_LIFE_BOOK, /* Hack -- all spellbooks */
	TV_BOOK_ELEMENT, /* Hack -- all spellbooks */
	TV_GOLD,
	0,
	0,
};


/*
 * Build a list of object indexes in the given group. Return the number
 * of objects in the group.
 *
 * mode & 0x01 : check for non-empty group
 * mode & 0x02 : visual operation only
 */
static int collect_objects(int grp_cur, int object_idx[], byte mode)
{
	int i, j, k, object_cnt = 0;

	/* Get a list of x_char in this group */
	byte group_tval = object_group_tval[grp_cur];

	/* Check every object */
	for (i = 0; i < max_k_idx; i++)
	{
		/* Access the object */
		object_kind *k_ptr = &k_info[i];

		/* Skip empty objects */
		if (!k_ptr->name) continue;

		if (mode & 0x02)
		{
			/* Any objects will be displayed */
		}
		else
		{
			if (!p_ptr->wizard)
			{
				/* Skip non-flavoured objects */
				if (!k_ptr->flavor) continue;

				/* Require objects ever seen */
				if (!k_ptr->aware) continue;
			}

			/* Skip items with no distribution (special artifacts) */
			for (j = 0, k = 0; j < 4; j++) k += k_ptr->chance[j];
			if (!k) continue;
		}

		/* Check for objects in the group */
		if (TV_BOOK_ELEMENT == group_tval)
		{
			/* Hack -- All spell books */
		//	if (TV_LIFE_BOOK <= k_ptr->tval && k_ptr->tval <= TV_HEX_BOOK)
			///mod160619 �s�����t�������̂Œ��������ǂ��ς�邾�낤
			if ( k_ptr->tval <= TV_BOOK_END)
			{
				/* Add the object */
				object_idx[object_cnt++] = i;
			}
			else continue;
		}
		else if (k_ptr->tval == group_tval)
		{
			/* Add the object */
			object_idx[object_cnt++] = i;
		}
		else continue;

		/* XXX Hack -- Just checking for non-empty group */
		if (mode & 0x01) break;
	}

	/* Terminate the list */
	object_idx[object_cnt] = -1;

	/* Return the number of objects */
	return object_cnt;
}


/*
 * Description of each feature group.
 */
static cptr feature_group_text[] = 
{
	"terrains",
	NULL
};


/*
 * Build a list of feature indexes in the given group. Return the number
 * of features in the group.
 *
 * mode & 0x01 : check for non-empty group
 */
static int collect_features(int grp_cur, int *feat_idx, byte mode)
{
	int i, feat_cnt = 0;

	/* Unused;  There is a single group. */
	(void)grp_cur;

	/* Check every feature */
	for (i = 0; i < max_f_idx; i++)
	{
		/* Access the index */
		feature_type *f_ptr = &f_info[i];

		/* Skip empty index */
		if (!f_ptr->name) continue;

		/* Skip mimiccing features */
		if (f_ptr->mimic != i) continue;

		/* Add the index */
		feat_idx[feat_cnt++] = i;

		/* XXX Hack -- Just checking for non-empty group */
		if (mode & 0x01) break;
	}

	/* Terminate the list */
	feat_idx[feat_cnt] = -1;

	/* Return the number of races */
	return feat_cnt;
}


#if 0
/*
 * Build a list of monster indexes in the given group. Return the number
 * of monsters in the group.
 */
static int collect_artifacts(int grp_cur, int object_idx[])
{
	int i, object_cnt = 0;

	/* Get a list of x_char in this group */
	byte group_tval = object_group_tval[grp_cur];

	/* Check every object */
	for (i = 0; i < max_a_idx; i++)
	{
		/* Access the artifact */
		artifact_type *a_ptr = &a_info[i];

		/* Skip empty artifacts */
		if (!a_ptr->name) continue;

		/* Skip "uncreated" artifacts */
		if (!a_ptr->cur_num) continue;

		/* Check for race in the group */
		if (a_ptr->tval == group_tval)
		{
			/* Add the race */
			object_idx[object_cnt++] = i;
		}
	}

	/* Terminate the list */
	object_idx[object_cnt] = 0;

	/* Return the number of races */
	return object_cnt;
}
#endif /* 0 */


/*
 * Encode the screen colors
 */
static char hack[17] = "dwsorgbuDWvyRGBU";


/*
 * Hack -- load a screen dump from a file
 */
/*:::�X�N���[���_���v��\������*/
void do_cmd_load_screen(void)
{
	int i, y, x;

	byte a = 0;
	char c = ' ';

	bool okay = TRUE;

	FILE *fff;

	char buf[1024];

	int wid, hgt;

	Term_get_size(&wid, &hgt);

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, "dump.txt");

	/* Append to the file */
	fff = my_fopen(buf, "r");

	/* Oops */
	if (!fff) {
#ifdef JP
		msg_format("%s ���J�����Ƃ��ł��܂���ł����B", buf);
#else
		msg_format("Failed to open %s.", buf);
#endif
		msg_print(NULL);
		return;
	}


	/* Save the screen */
	screen_save();

	/* Clear the screen */
	Term_clear();


	/* Load the screen */
	for (y = 0; okay; y++)
	{
		/* Get a line of data including control code */
		if (!fgets(buf, 1024, fff)) okay = FALSE;

		/* Get the blank line */
		if (buf[0] == '\n' || buf[0] == '\0') break;

		/* Ignore too large screen image */
		if (y >= hgt) continue;

		/* Show each row */
		for (x = 0; x < wid - 1; x++)
		{
			/* End of line */
			if (buf[x] == '\n' || buf[x] == '\0') break;

			/* Put the attr/char */
			Term_draw(x, y, TERM_WHITE, buf[x]);
		}
	}

	/* Dump the screen */
	for (y = 0; okay; y++)
	{
		/* Get a line of data including control code */
		if (!fgets(buf, 1024, fff)) okay = FALSE;

		/* Get the blank line */
		if (buf[0] == '\n' || buf[0] == '\0') break;

		/* Ignore too large screen image */
		if (y >= hgt) continue;

		/* Dump each row */
		for (x = 0; x < wid - 1; x++)
		{
			/* End of line */
			if (buf[x] == '\n' || buf[x] == '\0') break;

			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Look up the attr */
			for (i = 0; i < 16; i++)
			{
				/* Use attr matches */
				if (hack[i] == buf[x]) a = i;
			}

			/* Put the attr/char */
			Term_draw(x, y, a, c);
		}
	}


	/* Close it */
	my_fclose(fff);


	/* Message */
#ifdef JP
	prt("�t�@�C���ɏ����o���ꂽ���(�L�O�B�e)�����[�h���܂����B", 0, 0);
#else
	msg_print("Screen dump loaded.");
#endif

	flush();
	inkey();


	/* Restore the screen */
	screen_load();
}



///sys res ~9�R�}���h�ł̑ϐ��ꗗ
cptr inven_res_label = 
#ifdef JP
 "                               �_�d�Η�őM�Ôj�����������ח� �������Ӌ� �������r�x��";
#else
 "                               AcElFiCoPoLiDkShSoNtNxCaDi BlFeCfFaSeHlEpSdRgLv";
#endif


#ifdef JP
#define IM_FLAG_STR  "��"
#define HAS_FLAG_STR "�{"
#define NO_FLAG_STR  "�E"
#else
#define IM_FLAG_STR  "* "
#define HAS_FLAG_STR "+ "
#define NO_FLAG_STR  ". "
#endif

#define print_im_or_res_flag(IM, RES) \
{ \
	fputs(have_flag(flgs, (IM)) ? IM_FLAG_STR : \
	      (have_flag(flgs, (RES)) ? HAS_FLAG_STR : NO_FLAG_STR), fff); \
}

#define print_flag(TR) \
{ \
	fputs(have_flag(flgs, (TR)) ? HAS_FLAG_STR : NO_FLAG_STR, fff); \
}


/* XTRA HACK RESLIST */
///item res ~9�R�}���h�@�ۗL�A�C�e���̑ϐ��ꗗ
///mod131229 ~9�R�}���h�@�A�C�e���ϐ��t���O�ύX
static void do_cmd_knowledge_inven_aux(FILE *fff, object_type *o_ptr, int *j, byte tval, char *where)
{
	char o_name[MAX_NLEN];
	u32b flgs[TR_FLAG_SIZE];

	if (!o_ptr->k_idx) return;
	if (o_ptr->tval != tval) return;

	/* Identified items only */
	if (!object_is_known(o_ptr)) return;

	/*
	 * HACK:Ring of Lordly protection and Dragon equipment
	 * have random resistances.
	 */
	///item TVAl SVAL �ϐ��ꗗ��ʂɕ\������A�C�e���̎�ނ��w��@�h���S���Z���ǉ����悤
	///mod131223 tval
	if ((object_is_wearable(o_ptr) && object_is_ego(o_ptr))
	    || ((tval == TV_AMULET) && (o_ptr->sval == SV_AMULET_RESISTANCE))
	    || ((tval == TV_RING) && (o_ptr->sval == SV_RING_LORDLY))
	    || ((tval == TV_SHIELD) && (o_ptr->sval == SV_DRAGON_SHIELD))
	    || ((tval == TV_HEAD) && (o_ptr->sval == SV_HEAD_DRAGONHELM))
	    || ((tval == TV_GLOVES) && (o_ptr->sval == SV_HAND_DRAGONGLOVES))
	    || ((tval == TV_BOOTS) && (o_ptr->sval == SV_LEG_DRAGONBOOTS))
	    || ((tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DRAGON_DAGGER))
		|| (tval == TV_DRAG_ARMOR)

		|| object_is_artifact(o_ptr))
	{
		int i = 0;
		object_desc(o_name, o_ptr, OD_NAME_ONLY);

		while (o_name[i] && (i < 26))
		{
#ifdef JP
			if (iskanji(o_name[i])) i++;
#endif
			i++;
		}

		if (i < 28)
		{
			while (i < 28)
			{
				o_name[i] = ' '; i++;
			}
		}
		o_name[i] = '\0';

		fprintf(fff, "%s %s", where, o_name);

		if (!(o_ptr->ident & (IDENT_MENTAL)))
		{
#ifdef JP
			fputs("-------�s��------------------- ---�s��--- ----�s��----\n", fff);
#else
			fputs("-------unknown------------ -------unknown------\n", fff);
#endif
		}
		else
		{
			object_flags_known(o_ptr, flgs);

			print_im_or_res_flag(TR_IM_ACID, TR_RES_ACID);
			print_im_or_res_flag(TR_IM_ELEC, TR_RES_ELEC);
			print_im_or_res_flag(TR_IM_FIRE, TR_RES_FIRE);
			print_im_or_res_flag(TR_IM_COLD, TR_RES_COLD);
			print_flag(TR_RES_POIS);
			print_flag(TR_RES_LITE);
			print_flag(TR_RES_DARK);
			print_flag(TR_RES_SHARDS);
			print_flag(TR_RES_WATER);
			print_flag(TR_RES_HOLY);
			print_flag(TR_RES_SOUND);
			print_flag(TR_RES_NETHER);
			print_flag(TR_RES_TIME);
			print_flag(TR_RES_CHAOS);
			print_flag(TR_RES_DISEN);
			fputs(" ", fff);
			print_flag(TR_RES_FEAR);
			print_flag(TR_FREE_ACT);
			print_flag(TR_RES_CONF);
			print_flag(TR_RES_BLIND);
			print_flag(TR_RES_INSANITY);
			fputs(" ", fff);
			print_flag(TR_SEE_INVIS);
			print_flag(TR_TELEPATHY);
			print_flag(TR_LEVITATION);
			print_flag(TR_SPEEDSTER);
			print_flag(TR_WARNING);
			print_flag(TR_REFLECT);

			fputc('\n', fff);
		}
		(*j)++;
		if (*j == 9)
		{
			*j = 0;
			fprintf(fff, "%s\n", inven_res_label);
		}
	}
}

/*
 * Display *ID* ed weapons/armors's resistances
 */
///mod131229 ~9�R�}���h�@�ϐ��ύX�ɂ��C��
static void do_cmd_knowledge_inven(void)
{
	FILE *fff;

	char file_name[1024];

	store_type  *st_ptr;

	byte tval;
	int i = 0;
	int j = 0;

	char  where[32];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff)
	{
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	fprintf(fff, "%s\n", inven_res_label);

	for (tval = TV_WEARABLE_BEGIN; tval <= TV_WEARABLE_END; tval++)
	{
		/*:::10�s���Ƃɑ����������x����}��*/
		if (j != 0)
		{
			for (; j < 9; j++) fputc('\n', fff);
			j = 0;
			fprintf(fff, "%s\n", inven_res_label);
		}

#ifdef JP
		strcpy(where, "��");
#else
		strcpy(where, "E ");
#endif
		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{
			do_cmd_knowledge_inven_aux(fff, &inventory[i], &j, tval, where);
		}

#ifdef JP
		strcpy(where, "��");
#else
		strcpy(where, "I ");
#endif
		for (i = 0; i < INVEN_PACK; i++)
		{
			do_cmd_knowledge_inven_aux(fff, &inventory[i], &j, tval, where);
		}

		st_ptr = &town[1].store[STORE_HOME];
#ifdef JP
		strcpy(where, "��");
#else
		strcpy(where, "H ");
#endif

		for (i = 0; i < st_ptr->stock_num; i++)
		{
			do_cmd_knowledge_inven_aux(fff, &st_ptr->stock[i], &j, tval, where);
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "*�Ӓ�*�ςݕ���/�h��̑ϐ����X�g", 0, 0);
#else
	show_file(TRUE, file_name, "Resistances of *identified* equipment", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);
}

void do_cmd_save_screen_html_aux(char *filename, int message)
{
	int y, x, i;

	byte a = 0, old_a = 0;
	char c = ' ';

	FILE *fff, *tmpfff;
	char buf[2048];

	int yomikomu = 0;
	cptr tags[4] = {
		"HEADER_START:",
		"HEADER_END:",
		"FOOTER_START:",
		"FOOTER_END:",
	};

	cptr html_head[] = {
		"<html>\n<body text=\"#ffffff\" bgcolor=\"#000000\">\n",
		"<pre>",
		0,
	};
	cptr html_foot[] = {
		"</pre>\n",
		"</body>\n</html>\n",
		0,
	};

	int wid, hgt;

	Term_get_size(&wid, &hgt);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	fff = my_fopen(filename, "w");

	/* Oops */
	if (!fff) {
		if (message) {
#ifdef JP
		    msg_format("�t�@�C�� %s ���J���܂���ł����B", filename);
#else
		    msg_format("Failed to open file %s.", filename);
#endif
		    msg_print(NULL);
		}
		
		return;
	}

	/* Save the screen */
	if (message)
		screen_save();

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, "htmldump.prf");
	tmpfff = my_fopen(buf, "r");
	if (!tmpfff) {
		for (i = 0; html_head[i]; i++)
			fputs(html_head[i], fff);
	}
	else {
		yomikomu = 0;
		while (!my_fgets(tmpfff, buf, sizeof(buf))) {
			if (!yomikomu) {
				if (strncmp(buf, tags[0], strlen(tags[0])) == 0)
					yomikomu = 1;
			}
			else {
				if (strncmp(buf, tags[1], strlen(tags[1])) == 0)
					break;
				fprintf(fff, "%s\n", buf);
			}
		}
	}

	/* Dump the screen */
	for (y = 0; y < hgt; y++)
	{
		/* Start the row */
		if (y != 0)
			fprintf(fff, "\n");

		/* Dump each row */
		for (x = 0; x < wid - 1; x++)
		{
			int rv, gv, bv;
			cptr cc = NULL;
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			switch (c)
			{
			case '&': cc = "&amp;"; break;
			case '<': cc = "&lt;"; break;
			case '>': cc = "&gt;"; break;
#ifdef WINDOWS
			case 0x1f: c = '.'; break;
			case 0x7f: c = (a == 0x09) ? '%' : '#'; break;
#endif
			}

			a = a & 0x0F;
			if ((y == 0 && x == 0) || a != old_a) {
				rv = angband_color_table[a][1];
				gv = angband_color_table[a][2];
				bv = angband_color_table[a][3];
				fprintf(fff, "%s<font color=\"#%02x%02x%02x\">", 
					((y == 0 && x == 0) ? "" : "</font>"), rv, gv, bv);
				old_a = a;
			}
			if (cc)
				fprintf(fff, "%s", cc);
			else
				fprintf(fff, "%c", c);
		}
	}
	fprintf(fff, "</font>");

	if (!tmpfff) {
		for (i = 0; html_foot[i]; i++)
			fputs(html_foot[i], fff);
	}
	else {
		rewind(tmpfff);
		yomikomu = 0;
		while (!my_fgets(tmpfff, buf, sizeof(buf))) {
			if (!yomikomu) {
				if (strncmp(buf, tags[2], strlen(tags[2])) == 0)
					yomikomu = 1;
			}
			else {
				if (strncmp(buf, tags[3], strlen(tags[3])) == 0)
					break;
				fprintf(fff, "%s\n", buf);
			}
		}
		my_fclose(tmpfff);
	}

	/* Skip a line */
	fprintf(fff, "\n");

	/* Close it */
	my_fclose(fff);

	/* Message */
	if (message) {
#ifdef JP
	msg_print("���(�L�O�B�e)���t�@�C���ɏ����o���܂����B");
#else
		msg_print("Screen dump saved.");
#endif
		msg_print(NULL);
	}

	/* Restore the screen */
	if (message)
		screen_load();
}

/*
 * Hack -- save a screen dump to a file
 */
static void do_cmd_save_screen_html(void)
{
	char buf[1024], tmp[256] = "screen.html";

#ifdef JP
	if (!get_string("�t�@�C����: ", tmp, 80))
#else
	if (!get_string("File name: ", tmp, 80))
#endif
		return;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, tmp);

	msg_print(NULL);

	do_cmd_save_screen_html_aux(buf, 1);
}


/*
 * Redefinable "save_screen" action
 */
void (*screendump_aux)(void) = NULL;


/*
 * Hack -- save a screen dump to a file
 */
/*:::�X�N���[���V���b�g���t�@�C���œ���@�ڍז���*/
void do_cmd_save_screen(void)
{
	bool old_use_graphics = use_graphics;
	bool html_dump = FALSE;

	int wid, hgt;

#ifdef JP
	prt("�L�O�B�e���܂����H [(y)es/(h)tml/(n)o] ", 0, 0);
#else
	prt("Save screen dump? [(y)es/(h)tml/(n)o] ", 0, 0);
#endif
	while(TRUE)
	{
		char c = inkey();
		if (c == 'Y' || c == 'y')
			break;
		else if (c == 'H' || c == 'h')
		{
			html_dump = TRUE;
			break;
		}
		else
		{
			prt("", 0, 0);
			return;
		}
	}

	Term_get_size(&wid, &hgt);

	if (old_use_graphics)
	{
		use_graphics = FALSE;
		reset_visuals();

		/* Redraw everything */
		p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

		/* Hack -- update */
		handle_stuff();
	}

	if (html_dump)
	{
		do_cmd_save_screen_html();
		do_cmd_redraw();
	}

	/* Do we use a special screendump function ? */
	else if (screendump_aux)
	{
		/* Dump the screen to a graphics file */
		(*screendump_aux)();
	}
	else /* Dump the screen as text */
	{
		int y, x;

		byte a = 0;
		char c = ' ';

		FILE *fff;

		char buf[1024];

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_USER, "dump.txt");

		/* File type is "TEXT" */
		FILE_TYPE(FILE_TYPE_TEXT);

		/* Append to the file */
		fff = my_fopen(buf, "w");

		/* Oops */
		if (!fff)
		{
#ifdef JP
			msg_format("�t�@�C�� %s ���J���܂���ł����B", buf);
#else
			msg_format("Failed to open file %s.", buf);
#endif
			msg_print(NULL);
			return;
		}


		/* Save the screen */
		screen_save();


		/* Dump the screen */
		for (y = 0; y < hgt; y++)
		{
			/* Dump each row */
			for (x = 0; x < wid - 1; x++)
			{
				/* Get the attr/char */
				(void)(Term_what(x, y, &a, &c));

				/* Dump it */
				buf[x] = c;
			}

			/* Terminate */
			buf[x] = '\0';

			/* End the row */
			fprintf(fff, "%s\n", buf);
		}

		/* Skip a line */
		fprintf(fff, "\n");


		/* Dump the screen */
		for (y = 0; y < hgt; y++)
		{
			/* Dump each row */
			for (x = 0; x < wid - 1; x++)
			{
				/* Get the attr/char */
				(void)(Term_what(x, y, &a, &c));

				/* Dump it */
				buf[x] = hack[a&0x0F];
			}

			/* Terminate */
			buf[x] = '\0';

			/* End the row */
			fprintf(fff, "%s\n", buf);
		}

		/* Skip a line */
		fprintf(fff, "\n");


		/* Close it */
		my_fclose(fff);

		/* Message */
#ifdef JP
	msg_print("���(�L�O�B�e)���t�@�C���ɏ����o���܂����B");
#else
		msg_print("Screen dump saved.");
#endif

		msg_print(NULL);


		/* Restore the screen */
		screen_load();
	}

	if (old_use_graphics)
	{
		use_graphics = TRUE;
		reset_visuals();

		/* Redraw everything */
		p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

		/* Hack -- update */
		handle_stuff();
	}
}


/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
static bool ang_sort_art_comp(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;

	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = a_info[w1].tval;
		z2 = a_info[w2].tval;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = a_info[w1].sval;
		z2 = a_info[w2].sval;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = a_info[w1].level;
		z2 = a_info[w2].level;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
static void ang_sort_art_swap(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Unused */
	(void)v;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}


/*
 * Check the status of "artifacts"
 */
static void do_cmd_knowledge_artifacts(void)
{
	int i, k, z, x, y, n = 0;
	u16b why = 3;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	char base_name[MAX_NLEN];

	bool *okay;

	if(ironman_no_fixed_art)
	{
	    msg_format("����Ȃ��̂͂Ȃ��B");
		return;
	}

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_a_idx, s16b);

	/* Allocate the "okay" array */
	C_MAKE(okay, max_a_idx, bool);

	/* Scan the artifacts */
	for (k = 0; k < max_a_idx; k++)
	{
		artifact_type *a_ptr = &a_info[k];

		/* Default */
		okay[k] = FALSE;

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Skip "uncreated" artifacts */
		if (!a_ptr->cur_num) continue;

		/* Assume okay */
		okay[k] = TRUE;
	}

	/* Check the dungeon */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			s16b this_o_idx, next_o_idx = 0;

			/* Scan all objects in the grid */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Ignore non-artifacts */
				if (!object_is_fixed_artifact(o_ptr)) continue;

				/* Ignore known items */
				if (object_is_known(o_ptr)) continue;

				/* Note the artifact */
				okay[o_ptr->name1] = FALSE;
			}
		}
	}

	/* Check the inventory and equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Ignore non-objects */
		if (!o_ptr->k_idx) continue;

		/* Ignore non-artifacts */
		if (!object_is_fixed_artifact(o_ptr)) continue;

		/* Ignore known items */
		if (object_is_known(o_ptr)) continue;

		/* Note the artifact */
		okay[o_ptr->name1] = FALSE;
	}

	for (k = 0; k < max_a_idx; k++)
	{
		if (okay[k]) who[n++] = k;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_art_comp;
	ang_sort_swap = ang_sort_art_swap;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, n);

	/* Scan the artifacts */
	for (k = 0; k < n; k++)
	{
		artifact_type *a_ptr = &a_info[who[k]];

		/* Paranoia */
#ifdef JP
		strcpy(base_name, "���m�̓`���̃A�C�e��");
#else
		strcpy(base_name, "Unknown Artifact");
#endif


		/* Obtain the base object type */
		z = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* Real object */
		if (z)
		{
			object_type forge;
			object_type *q_ptr;

			/* Get local object */
			q_ptr = &forge;

			/* Create fake object */
			object_prep(q_ptr, z);

			/* Make it an artifact */
			//q_ptr->name1 = (byte)who[k];
			///mod141214 ���m�����X�g���������������̂��C��
			q_ptr->name1 = (s16b)who[k];

			/* Display as if known */
			q_ptr->ident |= IDENT_STORE;

			/* Describe the artifact */
			object_desc(base_name, q_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		}

		/* Hack -- Build the artifact name */
#ifdef JP
		fprintf(fff, "     %s\n", base_name);
#else
		fprintf(fff, "     The %s\n", base_name);
#endif

	}

	/* Free the "who" array */
	C_KILL(who, max_a_idx, s16b);

	/* Free the "okay" array */
	C_KILL(okay, max_a_idx, bool);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���m�̓`���̃A�C�e��", 0, 0);
#else
	show_file(TRUE, file_name, "Artifacts Seen", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known uniques
 * With "XTRA HACK UNIQHIST" (Originally from XAngband)
 */
static void do_cmd_knowledge_uniques(void)
{
	int i, k, n = 0;
	u16b why = 2;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	int n_alive[10];
	int n_alive_surface = 0;
	int n_alive_over100 = 0;
	int n_alive_total = 0;
	int max_lev = -1;

	for (i = 0; i < 10; i++) n_alive[i] = 0;

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if (!fff)
	{
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];
		int          lev;

		if (!r_ptr->name) continue;

		//v1.1.48
		if (i == MON_EXTRA_FIELD) continue;
		if (r_ptr->flags7 & RF7_VARIABLE) continue;

		//v1.1.92 俎q(���Z���p���ꃂ���X�^�[)�����O
		if (i == MON_SUMIREKO_2) continue;

		/* Require unique monsters */
		if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

		/* Only display "known" uniques */
		if (!cheat_know && !r_ptr->r_sights) continue;

		/* Only print rarity <= 100 uniques */
		if (!r_ptr->rarity || ((r_ptr->rarity > 100) && !(r_ptr->flags1 & RF1_QUESTOR))) continue;

		/* Only "alive" uniques */
		if (r_ptr->max_num == 0) continue;

		if (r_ptr->level)
		{
			lev = (r_ptr->level - 1) / 10;
			if (lev < 10)
			{
				n_alive[lev]++;
				if (max_lev < lev) max_lev = lev;
			}
			else n_alive_over100++;
		}
		else n_alive_surface++;

		/* Collect "appropriate" monsters */
		who[n++] = i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, n);

	if (n_alive_surface)
	{
#ifdef JP
		///msg131221
		fprintf(fff, "     �n��  ���œ|: %3d��\n", n_alive_surface);
#else
		fprintf(fff, "      Surface  alive: %3d\n", n_alive_surface);
#endif
		n_alive_total += n_alive_surface;
	}
	for (i = 0; i <= max_lev; i++)
	{
#ifdef JP
		fprintf(fff, "%3d-%3d�K  ���œ|: %3d��\n", 1 + i * 10, 10 + i * 10, n_alive[i]);
#else
		fprintf(fff, "Level %3d-%3d  alive: %3d\n", 1 + i * 10, 10 + i * 10, n_alive[i]);
#endif
		n_alive_total += n_alive[i];
	}
	if (n_alive_over100)
	{
#ifdef JP
		fprintf(fff, "101-   �K  ���œ|: %3d��\n", n_alive_over100);
#else
		fprintf(fff, "Level 101-     alive: %3d\n", n_alive_over100);
#endif
		n_alive_total += n_alive_over100;
	}

	if (n_alive_total)
	{
#ifdef JP
		fputs("---------  -----------\n", fff);
		fprintf(fff, "     ���v  ���œ|: %3d��\n\n", n_alive_total);
#else
		fputs("-------------  ----------\n", fff);
		fprintf(fff, "        Total  alive: %3d\n\n", n_alive_total);
#endif
	}
	else
	{
#ifdef JP
		fputs("���݂͊��m�̖��œ|���j�[�N�͂��܂���B\n", fff);
#else
		fputs("No known uniques alive.\n", fff);
#endif
	}

	/* Scan the monster races */
	for (k = 0; k < n; k++)
	{
		monster_race *r_ptr = &r_info[who[k]];

		/* Print a message */
#ifdef JP
		fprintf(fff, "     %s (���x��%d)\n", r_name + r_ptr->name, r_ptr->level);
#else
		fprintf(fff, "     %s (level %d)\n", r_name + r_ptr->name, r_ptr->level);
#endif
	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, s16b);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�܂��|���Ă��Ȃ����j�[�N�E�����X�^�[", 0, 0);
#else
	show_file(TRUE, file_name, "Alive Uniques", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display weapon-exp
 */
///sys item ����o���l�ꗗ��\��
///del131227 ����o���l��skill_exp�Ɠ�������
#if 0
static void do_cmd_knowledge_weapon_exp(void)
{
	int i, j, num, weapon_exp;

	FILE *fff;

	char file_name[1024];
	char tmp[30];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	for (i = 0; i < 5; i++)
	{
		for (num = 0; num < 64; num++)
		{
			for (j = 0; j < max_k_idx; j++)
			{
				object_kind *k_ptr = &k_info[j];

				if ((k_ptr->tval == TV_SWORD - i) && (k_ptr->sval == num))
				{
					if ((k_ptr->tval == TV_BOW) && (k_ptr->sval == SV_CRIMSON)) continue;

					weapon_exp = p_ptr->weapon_exp[4 - i][num];
					strip_name(tmp, j);
					fprintf(fff, "%-25s ", tmp);
					if (weapon_exp >= s_info[p_ptr->pclass].w_max[4 - i][num]) fprintf(fff, "!");
					else fprintf(fff, " ");
					fprintf(fff, "%s", exp_level_str[weapon_exp_level(weapon_exp)]);
					if (cheat_xtra) fprintf(fff, " %d", weapon_exp);
					fprintf(fff, "\n");
					break;
				}
			}
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "����̌o���l", 0, 0);
#else
	show_file(TRUE, file_name, "Weapon Proficiency", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}
#endif

/*
 * Display spell-exp
 */
static void do_cmd_knowledge_spell_exp(void)
{
	int i = 0, spell_exp, exp_level;

	FILE *fff;
	magic_type forge, *s_ptr = &forge;

	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	if(cp_ptr->realm_aptitude[0] == 0)
	{
	    msg_format("���Ȃ��͖��@�����g���Ȃ��B");
	    return;
	}
	if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
	{
	    msg_format("���Ȃ��͖��@�����g���Ȃ��B");
	    return;
	}


	if (p_ptr->realm1 != REALM_NONE)
	{
#ifdef JP
		fprintf(fff, "%s�̖��@��\n", realm_names[p_ptr->realm1]);
#else
		fprintf(fff, "%s Spellbook\n", realm_names[p_ptr->realm1]);
#endif
		for (i = 0; i < 32; i++)
		{
			//if (!is_magic(p_ptr->realm1))
			if (p_ptr->realm1 == TV_BOOK_HISSATSU)
			{
				s_ptr = &hissatsu_info[i];
			}
			else
			{
				//s_ptr = &mp_ptr->info[p_ptr->realm1 - 1][i];
				calc_spell_info(s_ptr,p_ptr->realm1,i);
			}
			if (s_ptr->slevel >= 99) continue;
			spell_exp = p_ptr->spell_exp[i];
			exp_level = spell_exp_level(spell_exp);
			fprintf(fff, "%-25s ", do_spell(p_ptr->realm1, i, SPELL_NAME));
			if (p_ptr->realm1 == TV_BOOK_HISSATSU)
				fprintf(fff, "[--]");
			else
			{
				if (exp_level >= EXP_LEVEL_MASTER) fprintf(fff, "!");
				else fprintf(fff, " ");
				fprintf(fff, "%s", exp_level_str[exp_level]);
			}
			if (cheat_xtra) fprintf(fff, " %d", spell_exp);
			fprintf(fff, "\n");
		}
	}

	if (p_ptr->realm2 != REALM_NONE)
	{
#ifdef JP
		fprintf(fff, "%s�̖��@��\n", realm_names[p_ptr->realm2]);
#else
		fprintf(fff, "\n%s Spellbook\n", realm_names[p_ptr->realm2]);
#endif
		for (i = 0; i < 32; i++)
		{
			/*
			if (!is_magic(p_ptr->realm1))
			{
				s_ptr = &technic_info[p_ptr->realm2 - MIN_TECHNIC][i];
			}
			else
			*/
			{
				//s_ptr = &mp_ptr->info[p_ptr->realm2 - 1][i];
				calc_spell_info(s_ptr,p_ptr->realm2,i);
			}
			if (s_ptr->slevel >= 99) continue;

			spell_exp = p_ptr->spell_exp[i + 32];
			exp_level = spell_exp_level(spell_exp);
			fprintf(fff, "%-25s ", do_spell(p_ptr->realm2, i, SPELL_NAME));
			if (exp_level >= EXP_LEVEL_EXPERT) fprintf(fff, "!");
			else fprintf(fff, " ");
			fprintf(fff, "%s", exp_level_str[exp_level]);
			if (cheat_xtra) fprintf(fff, " %d", spell_exp);
			fprintf(fff, "\n");
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���@�̌o���l", 0, 0);
#else
	show_file(TRUE, file_name, "Spell Proficiency", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display skill-exp
 */
///sys �Z�\�o���l�@����o���l�̃y�[�W�Ɠ������Ă��܂����B
///mod131226 skill �Z�\�ƕ���Z�\�̓����ɂ��t�H�[�}�b�g�ύX
/*:::memo:������ύX�����Ƃ�dump_aux_skills()�����l�ɕύX*/
static void do_cmd_knowledge_skill_exp(void)
{
	int i = 0, skill_exp;
	int tmp_tval;

	FILE *fff;

	char file_name[1024];
#ifdef JP
	//char skill_name[3][17]={"�}�[�V�����A�[�c", "�񓁗�          ", "��n            "};

	char skill_name[SKILL_EXP_MAX][16] ={"�i��","��","��n","�񓁗�","����","","","","","","�Z��","����","��","�݊�","�_","��","��","��������","�|","�N���X�{�E","�e"};
#else
	char skill_name[3][20]={"Martial Arts    ", "Dual Wielding   ", "Riding          "};
#endif

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	fprintf(fff, "�Z�\�̎��    �K��    �Z�\���x��\n");
	for (i = 0; i < SKILL_EXP_MAX; i++)
	{
		if(i==5)
		{ 
			/*::: hack: p_ptr->skill_exp[5~9]�͖��g�p�Ȃ̂ŃX�L�b�v*/
			i=9;
			fprintf(fff, "\n");
			continue;
		}
		skill_exp = ref_skill_exp(i);
		//skill_exp = p_ptr->skill_exp[i];
		fprintf(fff, "%-12s ", skill_name[i]);
		//if (skill_exp >= s_info[p_ptr->pclass].s_max[i]) fprintf(fff, "!");

		if(cp_ptr->skill_aptitude[i] == 0) fprintf(fff, "(�F��)");
		if(cp_ptr->skill_aptitude[i] == 1) fprintf(fff, "( E )");
		if(cp_ptr->skill_aptitude[i] == 2) fprintf(fff, "( D )");
		if(cp_ptr->skill_aptitude[i] == 3) fprintf(fff, "( C )");
		if(cp_ptr->skill_aptitude[i] == 4) fprintf(fff, "( B )");
		if(cp_ptr->skill_aptitude[i] == 5) fprintf(fff, "( A )");


		fprintf(fff, "     %d",skill_exp / SKILL_LEV_TICK);
		if (skill_exp > cp_ptr->skill_aptitude[i] * SKILL_LEV_TICK * 10) fprintf(fff, "(��������˔j)");
		else if (skill_exp == cp_ptr->skill_aptitude[i] * SKILL_LEV_TICK * 10) fprintf(fff, "(�������)");
		else fprintf(fff, " ");

		//v1.1.62 ���̋Z�\���g�p���̂Ƃ��}�[�N������
		switch (i)
		{
		case SKILL_MARTIALARTS:
			if(p_ptr->do_martialarts) 	fprintf(fff, "*");
			break;
		case SKILL_SHIELD:
			if(inventory[INVEN_RARM].tval == TV_SHIELD || inventory[INVEN_LARM].tval == TV_SHIELD) 
				fprintf(fff, "*");
			break;
		case SKILL_RIDING:
			if (!CLASS_RIDING_BACKDANCE && p_ptr->riding)
				 fprintf(fff, "*");
			break;
		case SKILL_2WEAPONS:
			if (object_is_melee_weapon(&inventory[INVEN_RARM]) && object_is_melee_weapon(&inventory[INVEN_LARM]))
				fprintf(fff, "*");
			break;
		default:
			tmp_tval = i + SKILL_WEAPON_SHIFT;
			if (tmp_tval >= TV_KNIFE)
			{
				if (inventory[INVEN_RARM].tval == tmp_tval || inventory[INVEN_LARM].tval == tmp_tval)
					fprintf(fff, "*");
			}
			break;
		}

		//fprintf(fff, "%s", exp_level_str[(i == GINOU_RIDING) ? riding_exp_level(skill_exp) : weapon_exp_level(skill_exp)]);
		//if (cheat_xtra) fprintf(fff, " %d", skill_exp);
		fprintf(fff, "\n");
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�Z�\�̌o���l", 0, 0);
#else
	show_file(TRUE, file_name, "Miscellaneous Proficiency", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);


}


/*
 * Pluralize a monster name
 */
void plural_aux(char *Name)
{
	int NameLen = strlen(Name);

	if (my_strstr(Name, "Disembodied hand"))
	{
		strcpy(Name, "Disembodied hands that strangled people");
	}
	else if (my_strstr(Name, "Colour out of space"))
	{
		strcpy(Name, "Colours out of space");
	}
	else if (my_strstr(Name, "stairway to hell"))
	{
		strcpy(Name, "stairways to hell");
	}
	else if (my_strstr(Name, "Dweller on the threshold"))
	{
		strcpy(Name, "Dwellers on the threshold");
	}
	else if (my_strstr(Name, " of "))
	{
		cptr aider = my_strstr(Name, " of ");
		char dummy[80];
		int i = 0;
		cptr ctr = Name;

		while (ctr < aider)
		{
			dummy[i] = *ctr;
			ctr++; i++;
		}

		if (dummy[i-1] == 's')
		{
			strcpy(&(dummy[i]), "es");
			i++;
		}
		else
		{
			strcpy(&(dummy[i]), "s");
		}

		strcpy(&(dummy[i+1]), aider);
		strcpy(Name, dummy);
	}
	else if (my_strstr(Name, "coins"))
	{
		char dummy[80];
		strcpy(dummy, "piles of ");
		strcat(dummy, Name);
		strcpy(Name, dummy);
		return;
	}
	else if (my_strstr(Name, "Manes"))
	{
		return;
	}
	else if (streq(&(Name[NameLen - 2]), "ey"))
	{
		strcpy(&(Name[NameLen - 2]), "eys");
	}
	else if (Name[NameLen - 1] == 'y')
	{
		strcpy(&(Name[NameLen - 1]), "ies");
	}
	else if (streq(&(Name[NameLen - 4]), "ouse"))
	{
		strcpy(&(Name[NameLen - 4]), "ice");
	}
	else if (streq(&(Name[NameLen - 2]), "us"))
	{
		strcpy(&(Name[NameLen - 2]), "i");
	}
	else if (streq(&(Name[NameLen - 6]), "kelman"))
	{
		strcpy(&(Name[NameLen - 6]), "kelmen");
	}
	else if (streq(&(Name[NameLen - 8]), "wordsman"))
	{
		strcpy(&(Name[NameLen - 8]), "wordsmen");
	}
	else if (streq(&(Name[NameLen - 7]), "oodsman"))
	{
		strcpy(&(Name[NameLen - 7]), "oodsmen");
	}
	else if (streq(&(Name[NameLen - 7]), "eastman"))
	{
		strcpy(&(Name[NameLen - 7]), "eastmen");
	}
	else if (streq(&(Name[NameLen - 8]), "izardman"))
	{
		strcpy(&(Name[NameLen - 8]), "izardmen");
	}
	else if (streq(&(Name[NameLen - 5]), "geist"))
	{
		strcpy(&(Name[NameLen - 5]), "geister");
	}
	else if (streq(&(Name[NameLen - 2]), "ex"))
	{
		strcpy(&(Name[NameLen - 2]), "ices");
	}
	else if (streq(&(Name[NameLen - 2]), "lf"))
	{
		strcpy(&(Name[NameLen - 2]), "lves");
	}
	else if (suffix(Name, "ch") ||
		 suffix(Name, "sh") ||
			 suffix(Name, "nx") ||
			 suffix(Name, "s") ||
			 suffix(Name, "o"))
	{
		strcpy(&(Name[NameLen]), "es");
	}
	else
	{
		strcpy(&(Name[NameLen]), "s");
	}
}

/*
 * Display current pets
 */
static void do_cmd_knowledge_pets(void)
{
	int             i;
	FILE            *fff;
	monster_type    *m_ptr;
	char            pet_name[80];
	int             t_friends = 0;
	int             show_upkeep = 0;
	char            file_name[1024];


	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Process the monsters (backwards) */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Access the monster */
		m_ptr = &m_list[i];

		/* Ignore "dead" monsters */
		if (!m_ptr->r_idx) continue;

		/* Calculate "upkeep" for pets */
		if (is_pet(m_ptr))
		{
			t_friends++;
			monster_desc(pet_name, m_ptr, MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
			fprintf(fff, "%s (%s)\n", pet_name, look_mon_desc(m_ptr, 0x00));
		}
	}

	show_upkeep = calculate_upkeep();

	fprintf(fff, "----------------------------------------------\n");
#ifdef JP
	///msg131221
	fprintf(fff, "    ���v: %d �̂̔z��\n", t_friends);
	fprintf(fff, " �ێ��R�X�g: %d%% MP\n", show_upkeep);
#else
	fprintf(fff, "   Total: %d pet%s.\n",
		t_friends, (t_friends == 1 ? "" : "s"));
	fprintf(fff, "   Upkeep: %d%% mana.\n", show_upkeep);
#endif



	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���݂̔z��", 0, 0);
#else
	show_file(TRUE, file_name, "Current Pets", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


//v1.1.55 ���݂̌������p�󋵂�\������
static void do_cmd_knowledge_buildings(void)
{
	int             i;
	FILE            *fff;
	monster_type    *m_ptr;
	char            file_name[1024];

	bool			flag_notice_sth = FALSE;
	char	o_name[MAX_NLEN];
	char	m_name[MAX_NLEN];

	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;/*:::10*10000*/

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) 
	{
		msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
		msg_print(NULL);
		return;
	}

	//���P�ɃA�C�e����a���Ă���ꍇ�@�ڍׂ�kogasa_smith()�Q��
	if (inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH].k_idx)
	{
		int time_div;
		if (EXTRA_MODE) time_div = 8;
		else time_div = 1;
		int need_turn;

		need_turn = town[TOWN_HAKUREI].store[STORE_WEAPON].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) - turn;

		object_desc(o_name, &inven_special[INVEN_SPECIAL_SLOT_KOGASA_SMITH], OD_NAME_ONLY);

		if (need_turn > 0)
		{
			int h = (24 * need_turn / A_DAY) % 24;
			int m = (1440 * need_turn / A_DAY) % 60;

			fprintf(fff, "���P��%s��a���Ă���B\n�񑩂̍����܂ł���%02d:%02d���B\n\n",o_name,h,m);

		}
		else
		{
			fprintf(fff, "���P��%s��a���Ă���B������Ƃ��ς�ł���͂����B\n\n", o_name);
		}

		flag_notice_sth = TRUE;

	}

	//���̍��l�b�g���[�N�ŃA�C�e��������̏ꍇ�@�ڍׂ�grassroots_barter()�Q��
	if (p_ptr->grassroots > 1)
	{
		int need_turn;

		need_turn = (p_ptr->grassroots + 50000) - turn; //�A�C�e����n�����^�[�����甼��(50000�^�[��)��

		if (need_turn > 0)
		{
			int h = (24 * need_turn / A_DAY) % 24;
			int m = (1440 * need_turn / A_DAY) % 60;

			fprintf(fff, "���̍��d���l�b�g���[�N�ɃA�C�e����a���Ă���B\n�A�C�e��������̊J�n�܂ł���%02d:%02d���B\n\n", h, m);
		}
		else
		{
			fprintf(fff, "���̍��d���l�b�g���[�N�ŃA�C�e��������n�܂��Ă��鍠���B�_�Ђɋ}�����B\n\n");
		}
		flag_notice_sth = TRUE;
	}
	//�얲�̐肢�@�ڍׂ�reimu_fortune_teller()�Q��
	else if (p_ptr->today_mon)
	{
		int target_dungeon;
		int target_level;
		int target_r_idx;

		switch (p_ptr->today_mon)
		{

		case FORTUNETELLER_TREASURE:
			if (!p_ptr->barter_value) break;
			target_dungeon = p_ptr->barter_value / 1000;
			target_level = p_ptr->barter_value % 1000;
			fprintf(fff, "�얲�̐肢�ɂ��ƁA����%s��%d�K�ɍs���Ƃ��󂪂���炵���B\n\n", (d_name + d_info[target_dungeon].name),target_level);
			flag_notice_sth = TRUE;
			break;

		case FORTUNETELLER_TROPHY:
			if (!p_ptr->barter_value) break;
			target_r_idx = p_ptr->barter_value;
			if (r_info[target_r_idx].r_akills) break;
			fprintf(fff, "�얲�̐肢�ɂ��ƁA��������%s��|���ƍK�����K���炵���B\n\n", (r_name + r_info[target_r_idx].name));
			flag_notice_sth = TRUE;
			break;
		case FORTUNETELLER_GOODLUCK:
			fprintf(fff, "�얲�̐肢�ɂ��ƁA�����̎����̉^�C�͐�D���炵���I\n\n");
			flag_notice_sth = TRUE;
			break;
		case FORTUNETELLER_BADLUCK:
			fprintf(fff, "�얲�̐肢�ɂ��ƁA�����̎����̉^�C�͐�s���炵���c�c\n\n");
			flag_notice_sth = TRUE;
			break;
		default:
			fprintf(fff, "(ERROR:���̐肢���ʂ̃��b�Z�[�W����`����Ă��Ȃ�)\n\n");
			flag_notice_sth = TRUE;
		}


	}

	//�͂��ĂɃ����X�^�[��T���Ă�����Ă���Ƃ�
	if (p_ptr->hatate_mon_search_ridx)
	{
		int target_r_idx = p_ptr->hatate_mon_search_ridx;
		int target_dungeon = p_ptr->hatate_mon_search_dungeon / 1000;
		int target_level = p_ptr->hatate_mon_search_dungeon % 1000;

		if (!r_info[target_r_idx].r_akills)
		{
			fprintf(fff, "�͂��Ă̒����ɂ��ƁA%s��%d�K�ɁA\n", (d_name + d_info[target_dungeon].name), target_level);
			fprintf(fff, "�@�y%s�z������炵���B\n\n",  (r_name + r_info[target_r_idx].name));
			flag_notice_sth = TRUE;
		}

	}

	//EXTRA���[�h�ł��ӂɃA�C�e����z�B���Ă�����Ă���ꍇ�@�ڍׂ�orin_deliver_item()�Q��
	if (inven_special[INVEN_SPECIAL_SLOT_ORIN_DELIVER].k_idx)
	{
		int time_div = 24;
		int need_turn;

		need_turn = town[TOWN_HAKUREI].store[STORE_GENERAL].last_visit + (TURNS_PER_TICK * TOWN_DAWN / time_div) - turn;

		object_desc(o_name, &inven_special[INVEN_SPECIAL_SLOT_ORIN_DELIVER], OD_NAME_ONLY);

		if (need_turn > 0)
		{
			int h = (24 * need_turn / A_DAY) % 24;
			int m = (1440 * need_turn / A_DAY) % 60;

			fprintf(fff, "�Ή��L�ӂ�%s��z�B���Ă�����Ă���B\n����%02d:%02d���炢�Œ����͂����B\n\n", o_name, h, m);

		}
		else
		{
			fprintf(fff, "�Ή��L�ӂ�%s��z�B���Ă�����Ă���B�����������Ă���͂����B\n\n", o_name);
		}

		flag_notice_sth = TRUE;



	}

	if(!flag_notice_sth)
		fprintf(fff, "���ɐS�ɗ��߂Ă������Ƃ͂Ȃ��B\n\n");

	my_fclose(fff);
	show_file(TRUE, file_name, "���݂̎{�ݗ��p��", 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}

/*
 * Total kill count
 *
 * Note that the player ghosts are ignored.  XXX XXX XXX
 */
static void do_cmd_knowledge_kill_count(void)
{
	int i, k, n = 0;
	u16b why = 2;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	s32b Total = 0;


	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	{
		/* Monsters slain */
		int kk;

		for (kk = 1; kk < max_r_idx; kk++)
		{
			monster_race *r_ptr = &r_info[kk];

			if (r_ptr->flags1 & (RF1_UNIQUE))
			{
				bool dead = (r_ptr->max_num == 0);

				if (dead)
				{
					Total++;
				}
			}
			else
			{
				s16b This = r_ptr->r_pkills;

				if (This > 0)
				{
					Total += This;
				}
			}
		}

		if (Total < 1)
#ifdef JP
			fprintf(fff,"���Ȃ��͂܂��G��|���Ă��Ȃ��B\n\n");
#else
			fprintf(fff,"You have defeated no enemies yet.\n\n");
#endif
		else
#ifdef JP
			fprintf(fff,"���Ȃ���%ld�̂̓G��|���Ă���B\n\n", (long int)Total);
#else
			fprintf(fff,"You have defeated %ld %s.\n\n", (long int)Total, (Total == 1) ? "enemy" : "enemies");
#endif
	}

	Total = 0;

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[n++] = i;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array by dungeon depth of monsters */
	ang_sort(who, &why, n);

	/* Scan the monster races */
	for (k = 0; k < n; k++)
	{
		monster_race *r_ptr = &r_info[who[k]];

		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			bool dead = (r_ptr->max_num == 0);

			if (dead)
			{
				/* Print a message */
				fprintf(fff, "     %s\n",
				    (r_name + r_ptr->name));
				Total++;
			}
		}
		else
		{
			s16b This = r_ptr->r_pkills;

			if (This > 0)
			{
#ifdef JP
				/* p,t�͐l�Ɛ����� by ita */
				if (my_strchr("pt", r_ptr->d_char))
					fprintf(fff, "     %3d �l�� %s\n", This, r_name + r_ptr->name);
				else
					fprintf(fff, "     %3d �̂� %s\n", This, r_name + r_ptr->name);
#else
				if (This < 2)
				{
					if (my_strstr(r_name + r_ptr->name, "coins"))
					{
						fprintf(fff, "     1 pile of %s\n", (r_name + r_ptr->name));
					}
					else
					{
						fprintf(fff, "     1 %s\n", (r_name + r_ptr->name));
					}
				}
				else
				{
					char ToPlural[80];
					strcpy(ToPlural, (r_name + r_ptr->name));
					plural_aux(ToPlural);
					fprintf(fff, "     %d %s\n", This, ToPlural);
				}
#endif


				Total += This;
			}
		}
	}

	fprintf(fff,"----------------------------------------------\n");
#ifdef JP
	fprintf(fff,"    ���v: %lu �̂�|�����B\n", (unsigned long int)Total);
#else
	fprintf(fff,"   Total: %lu creature%s killed.\n",
		(unsigned long int)Total, (Total == 1 ? "" : "s"));
#endif


	/* Free the "who" array */
	C_KILL(who, max_r_idx, s16b);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�|�����G�̐�", 0, 0);
#else
	show_file(TRUE, file_name, "Kill Count", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display the object groups.
 */
static void display_group_list(int col, int row, int wid, int per_page,
	int grp_idx[], cptr group_text[], int grp_cur, int grp_top)
{
	int i;

	/* Display lines until done */
	for (i = 0; i < per_page && (grp_idx[i] >= 0); i++)
	{
		/* Get the group index */
		int grp = grp_idx[grp_top + i];

		/* Choose a color */
		byte attr = (grp_top + i == grp_cur) ? TERM_L_BLUE : TERM_WHITE;

		/* Erase the entire line */
		Term_erase(col, row + i, wid);

		/* Display the group label */
		c_put_str(attr, group_text[grp], row + i, col);
	}
}


/* 
 * Move the cursor in a browser window 
 */
static void browser_cursor(char ch, int *column, int *grp_cur, int grp_cnt, 
						   int *list_cur, int list_cnt)
{
	int d;
	int col = *column;
	int grp = *grp_cur;
	int list = *list_cur;

	/* Extract direction */
	if (ch == ' ')
	{
		/* Hack -- scroll up full screen */
		d = 3;
	}
	else if (ch == '-')
	{
		/* Hack -- scroll down full screen */
		d = 9;
	}
	else
	{
		d = get_keymap_dir(ch);
	}

	if (!d) return;

	/* Diagonals - hack */
	if ((ddx[d] > 0) && ddy[d])
	{
		int browser_rows;
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		browser_rows = hgt - 8;

		/* Browse group list */
		if (!col)
		{
			int old_grp = grp;

			/* Move up or down */
			grp += ddy[d] * (browser_rows - 1);

			/* Verify */
			if (grp >= grp_cnt)	grp = grp_cnt - 1;
			if (grp < 0) grp = 0;
			if (grp != old_grp)	list = 0;
		}

		/* Browse sub-list list */
		else
		{
			/* Move up or down */
			list += ddy[d] * browser_rows;

			/* Verify */
			if (list >= list_cnt) list = list_cnt - 1;
			if (list < 0) list = 0;
		}

		(*grp_cur) = grp;
		(*list_cur) = list;

		return;
	}

	if (ddx[d])
	{
		col += ddx[d];
		if (col < 0) col = 0;
		if (col > 1) col = 1;

		(*column) = col;

		return;
	}

	/* Browse group list */
	if (!col)
	{
		int old_grp = grp;

		/* Move up or down */
		grp += ddy[d];

		/* Verify */
		if (grp >= grp_cnt)	grp = grp_cnt - 1;
		if (grp < 0) grp = 0;
		if (grp != old_grp)	list = 0;
	}

	/* Browse sub-list list */
	else
	{
		/* Move up or down */
		list += ddy[d];

		/* Verify */
		if (list >= list_cnt) list = list_cnt - 1;
		if (list < 0) list = 0;
	}

	(*grp_cur) = grp;
	(*list_cur) = list;
}


/*
 * Display visuals.
 */
static void display_visual_list(int col, int row, int height, int width, byte attr_top, byte char_left)
{
	int i, j;

	/* Clear the display lines */
	for (i = 0; i < height; i++)
	{
		Term_erase(col, row + i, width);
	}

	/* Bigtile mode uses double width */
	if (use_bigtile) width /= 2;

	/* Display lines until done */
	for (i = 0; i < height; i++)
	{
		/* Display columns until done */
		for (j = 0; j < width; j++)
		{
			byte a;
			char c;
			int x = col + j;
			int y = row + i;
			int ia, ic;

			/* Bigtile mode uses double width */
			if (use_bigtile) x += j;

			ia = attr_top + i;
			ic = char_left + j;

			/* Ignore illegal characters */
			if (ia > 0x7f || ic > 0xff || ic < ' ' ||
			    (!use_graphics && ic > 0x7f))
				continue;

			a = (byte)ia;
			c = (char)ic;

			/* Force correct code for both ASCII character and tile */
			if (c & 0x80) a |= 0x80;

			/* Display symbol */
			Term_queue_bigchar(x, y, a, c, 0, 0);
		}
	}
}


/*
 * Place the cursor at the collect position for visual mode
 */
static void place_visual_list_cursor(int col, int row, byte a, byte c, byte attr_top, byte char_left)
{
	int i = (a & 0x7f) - attr_top;
	int j = c - char_left;

	int x = col + j;
	int y = row + i;

	/* Bigtile mode uses double width */
	if (use_bigtile) x += j;

	/* Place the cursor */
	Term_gotoxy(x, y);
}


/*
 *  Clipboard variables for copy&paste in visual mode
 */
static byte attr_idx = 0;
static byte char_idx = 0;

/* Hack -- for feature lighting */
static byte attr_idx_feat[F_LIT_MAX];
static byte char_idx_feat[F_LIT_MAX];

/*
 *  Do visual mode command -- Change symbols
 */
static bool visual_mode_command(char ch, bool *visual_list_ptr,
				int height, int width,
				byte *attr_top_ptr, byte *char_left_ptr,
				byte *cur_attr_ptr, byte *cur_char_ptr, bool *need_redraw)
{
	static byte attr_old = 0, char_old = 0;

	switch (ch)
	{
	case ESCAPE:
		if (*visual_list_ptr)
		{
			/* Cancel change */
			*cur_attr_ptr = attr_old;
			*cur_char_ptr = char_old;
			*visual_list_ptr = FALSE;

			return TRUE;
		}
		break;

	case '\n':
	case '\r':
		if (*visual_list_ptr)
		{
			/* Accept change */
			*visual_list_ptr = FALSE;
			*need_redraw = TRUE;

			return TRUE;
		}
		break;

	case 'V':
	case 'v':
		if (!*visual_list_ptr)
		{
			*visual_list_ptr = TRUE;

			*attr_top_ptr = MAX(0, (*cur_attr_ptr & 0x7f) - 5);
			*char_left_ptr = MAX(0, *cur_char_ptr - 10);

			attr_old = *cur_attr_ptr;
			char_old = *cur_char_ptr;

			return TRUE;
		}
		break;

	case 'C':
	case 'c':
		{
			int i;

			/* Set the visual */
			attr_idx = *cur_attr_ptr;
			char_idx = *cur_char_ptr;

			/* Hack -- for feature lighting */
			for (i = 0; i < F_LIT_MAX; i++)
			{
				attr_idx_feat[i] = 0;
				char_idx_feat[i] = 0;
			}
		}
		return TRUE;

	case 'P':
	case 'p':
		if (attr_idx || (!(char_idx & 0x80) && char_idx)) /* Allow TERM_DARK text */
		{
			/* Set the char */
			*cur_attr_ptr = attr_idx;
			*attr_top_ptr = MAX(0, (*cur_attr_ptr & 0x7f) - 5);
			if (!*visual_list_ptr) *need_redraw = TRUE;
		}

		if (char_idx)
		{
			/* Set the char */
			*cur_char_ptr = char_idx;
			*char_left_ptr = MAX(0, *cur_char_ptr - 10);
			if (!*visual_list_ptr) *need_redraw = TRUE;
		}

		return TRUE;

	default:
		if (*visual_list_ptr)
		{
			int eff_width;
			int d = get_keymap_dir(ch);
			byte a = (*cur_attr_ptr & 0x7f);
			byte c = *cur_char_ptr;

			if (use_bigtile) eff_width = width / 2;
			else eff_width = width;

			/* Restrict direction */
			if ((a == 0) && (ddy[d] < 0)) d = 0;
			if ((c == 0) && (ddx[d] < 0)) d = 0;
			if ((a == 0x7f) && (ddy[d] > 0)) d = 0;
			if ((c == 0xff) && (ddx[d] > 0)) d = 0;

			a += ddy[d];
			c += ddx[d];

			/* Force correct code for both ASCII character and tile */
			if (c & 0x80) a |= 0x80;

			/* Set the visual */
			*cur_attr_ptr = a;
			*cur_char_ptr = c;


			/* Move the frame */
			if ((ddx[d] < 0) && *char_left_ptr > MAX(0, (int)c - 10)) (*char_left_ptr)--;
			if ((ddx[d] > 0) && *char_left_ptr + eff_width < MIN(0xff, (int)c + 10)) (*char_left_ptr)++;
			if ((ddy[d] < 0) && *attr_top_ptr > MAX(0, (int)(a & 0x7f) - 4)) (*attr_top_ptr)--;
			if ((ddy[d] > 0) && *attr_top_ptr + height < MIN(0x7f, (a & 0x7f) + 4)) (*attr_top_ptr)++;
			return TRUE;
		}
		break;
	}

	/* Visual mode command is not used */
	return FALSE;
}


/*
 * Display the monsters in a group.
 */
static void display_monster_list(int col, int row, int per_page, s16b mon_idx[],
	int mon_cur, int mon_top, bool visual_only)
{
	int i;

	/* Display lines until done */
	for (i = 0; i < per_page && (mon_idx[mon_top + i] >= 0); i++)
	{
		byte attr;

		/* Get the race index */
		int r_idx = mon_idx[mon_top + i] ;

		/* Access the race */
		monster_race *r_ptr = &r_info[r_idx];

		/* Choose a color */
		attr = ((i + mon_top == mon_cur) ? TERM_L_BLUE : TERM_WHITE);

		/* Display the name */
		c_prt(attr, (r_name + r_ptr->name), row + i, col);

		/* Hack -- visual_list mode */
		if (per_page == 1)
		{
			c_prt(attr, format("%02x/%02x", r_ptr->x_attr, r_ptr->x_char), row + i, (p_ptr->wizard || visual_only) ? 56 : 61);
		}
		if (p_ptr->wizard || visual_only)
		{
			c_prt(attr, format("%d", r_idx), row + i, 62);
		}

		/* Erase chars before overwritten by the race letter */
		Term_erase(69, row + i, 255);

		/* Display symbol */
		Term_queue_bigchar(use_bigtile ? 69 : 70, row + i, r_ptr->x_attr, r_ptr->x_char, 0, 0);

		if (!visual_only)
		{
			/* Display kills */
			if (!(r_ptr->flags1 & RF1_UNIQUE)) put_str(format("%5d", r_ptr->r_pkills), row + i, 73);
#ifdef JP
			///msg131221
			else c_put_str((r_ptr->max_num == 0 ? TERM_L_DARK : TERM_WHITE), (r_ptr->max_num == 0 ? "�œ|��" : "���œ|"), row + i, 74);
#else
			else c_put_str((r_ptr->max_num == 0 ? TERM_L_DARK : TERM_WHITE), (r_ptr->max_num == 0 ? " dead" : "alive"), row + i, 73);
#endif
		}
	}

	/* Clear remaining lines */
	for (; i < per_page; i++)
	{
		Term_erase(col, row + i, 255);
	}
}


/*
 * Display known monsters.
 */
/*:::�����X�^�[�ꗗ��\������*/
///sys mon �����X�^�[�ꗗ
static void do_cmd_knowledge_monsters(bool *need_redraw, bool visual_only, int direct_r_idx)
{
	int i, len, max;
	int grp_cur, grp_top, old_grp_cur;
	int mon_cur, mon_top;
	int grp_cnt, grp_idx[100];
	int mon_cnt;
	s16b *mon_idx;

	int column = 0;
	bool flag;
	bool redraw;

	bool visual_list = FALSE;
	byte attr_top = 0, char_left = 0;

	int browser_rows;
	int wid, hgt;

	byte mode;

	/* Get size */
	Term_get_size(&wid, &hgt);

	browser_rows = hgt - 8;

	/* Allocate the "mon_idx" array */
	C_MAKE(mon_idx, max_r_idx, s16b);

	max = 0;
	grp_cnt = 0;

	if (direct_r_idx < 0)
	{
		mode = visual_only ? 0x03 : 0x01;

		/* Check every group */
		for (i = 0; monster_group_text[i] != NULL; i++)
		{
			/* Measure the label */
			len = strlen(monster_group_text[i]);

			/* Save the maximum length */
			if (len > max) max = len;

			/* See if any monsters are known */
			if ((monster_group_char[i] == ((char *) -1L)) || collect_monsters(i, mon_idx, mode))
			{
				/* Build a list of groups with known monsters */
				grp_idx[grp_cnt++] = i;
			}
		}

		mon_cnt = 0;
	}
	else
	{
		mon_idx[0] = direct_r_idx;
		mon_cnt = 1;

		/* Terminate the list */
		mon_idx[1] = -1;

		(void)visual_mode_command('v', &visual_list, browser_rows - 1, wid - (max + 3),
			&attr_top, &char_left, &r_info[direct_r_idx].x_attr, &r_info[direct_r_idx].x_char, need_redraw);
	}

	/* Terminate the list */
	grp_idx[grp_cnt] = -1;

	old_grp_cur = -1;
	grp_cur = grp_top = 0;
	mon_cur = mon_top = 0;

	flag = FALSE;
	redraw = TRUE;

	mode = visual_only ? 0x02 : 0x00;

	while (!flag)
	{
		char ch;
		monster_race *r_ptr;

		if (redraw)
		{
			clear_from(0);

#ifdef JP
			prt(format("%s - �����X�^�[", !visual_only ? "�m��" : "�\��"), 2, 0);
			if (direct_r_idx < 0) prt("�O���[�v", 4, 0);
			prt("���O", 4, max + 3);
			if (p_ptr->wizard || visual_only) prt("Idx", 4, 62);
			prt("����", 4, 67);
			///msg131221
			if (!visual_only) prt("�œ|��", 4, 72);
#else
			prt(format("%s - monsters", !visual_only ? "Knowledge" : "Visuals"), 2, 0);
			if (direct_r_idx < 0) prt("Group", 4, 0);
			prt("Name", 4, max + 3);
			if (p_ptr->wizard || visual_only) prt("Idx", 4, 62);
			prt("Sym", 4, 68);
			if (!visual_only) prt("Kills", 4, 73);
#endif

			for (i = 0; i < 78; i++)
			{
				Term_putch(i, 5, TERM_WHITE, '=');
			}

			if (direct_r_idx < 0)
			{
				for (i = 0; i < browser_rows; i++)
				{
					Term_putch(max + 1, 6 + i, TERM_WHITE, '|');
				}
			}

			redraw = FALSE;
		}

		if (direct_r_idx < 0)
		{
			/* Scroll group list */
			if (grp_cur < grp_top) grp_top = grp_cur;
			if (grp_cur >= grp_top + browser_rows) grp_top = grp_cur - browser_rows + 1;

			/* Display a list of monster groups */
			display_group_list(0, 6, max, browser_rows, grp_idx, monster_group_text, grp_cur, grp_top);

			if (old_grp_cur != grp_cur)
			{
				old_grp_cur = grp_cur;

				/* Get a list of monsters in the current group */
				mon_cnt = collect_monsters(grp_idx[grp_cur], mon_idx, mode);
			}

			/* Scroll monster list */
			while (mon_cur < mon_top)
				mon_top = MAX(0, mon_top - browser_rows/2);
			while (mon_cur >= mon_top + browser_rows)
				mon_top = MIN(mon_cnt - browser_rows, mon_top + browser_rows/2);
		}

		if (!visual_list)
		{
			/* Display a list of monsters in the current group */
			display_monster_list(max + 3, 6, browser_rows, mon_idx, mon_cur, mon_top, visual_only);
		}
		else
		{
			mon_top = mon_cur;

			/* Display a monster name */
			display_monster_list(max + 3, 6, 1, mon_idx, mon_cur, mon_top, visual_only);

			/* Display visual list below first monster */
			display_visual_list(max + 3, 7, browser_rows-1, wid - (max + 3), attr_top, char_left);
		}

		/* Prompt */
#ifdef JP
		prt(format("<����>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r'�Ŏv���o������" : "",
			visual_list ? ", ENTER�Ō���" : ", 'v'�ŃV���{���ύX",
			(attr_idx || char_idx) ? ", 'c', 'p'�Ńy�[�X�g" : ", 'c'�ŃR�s�["),
			hgt - 1, 0);
#else
		prt(format("<dir>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r' to recall" : "",
			visual_list ? ", ENTER to accept" : ", 'v' for visuals",
			(attr_idx || char_idx) ? ", 'c', 'p' to paste" : ", 'c' to copy"),
			hgt - 1, 0);
#endif

		/* Get the current monster */
		r_ptr = &r_info[mon_idx[mon_cur]];

		if (!visual_only)
		{
			/* Mega Hack -- track this monster race */
			if (mon_cnt) monster_race_track(mon_idx[mon_cur]);

			/* Hack -- handle stuff */
			handle_stuff();
		}

		if (visual_list)
		{
			place_visual_list_cursor(max + 3, 7, r_ptr->x_attr, r_ptr->x_char, attr_top, char_left);
		}
		else if (!column)
		{
			Term_gotoxy(0, 6 + (grp_cur - grp_top));
		}
		else
		{
			Term_gotoxy(max + 3, 6 + (mon_cur - mon_top));
		}

		ch = inkey();

		/* Do visual mode command if needed */
		if (visual_mode_command(ch, &visual_list, browser_rows-1, wid - (max + 3), &attr_top, &char_left, &r_ptr->x_attr, &r_ptr->x_char, need_redraw))
		{
			if (direct_r_idx >= 0)
			{
				switch (ch)
				{
				case '\n':
				case '\r':
				case ESCAPE:
					flag = TRUE;
					break;
				}
			}
			continue;
		}

		switch (ch)
		{
			case ESCAPE:
			{
				flag = TRUE;
				break;
			}

			case 'R':
			case 'r':
			{
				/* Recall on screen */
				if (!visual_list && !visual_only && (mon_idx[mon_cur] > 0))
				{
					screen_roff(mon_idx[mon_cur], 0);

					(void)inkey();

					redraw = TRUE;
				}
				break;
			}

			default:
			{
				/* Move the cursor */
				browser_cursor(ch, &column, &grp_cur, grp_cnt, &mon_cur, mon_cnt);

				break;
			}
		}
	}

	/* Free the "mon_idx" array */
	C_KILL(mon_idx, max_r_idx, s16b);
}


/*
 * Display the objects in a group.
 */
static void display_object_list(int col, int row, int per_page, int object_idx[],
	int object_cur, int object_top, bool visual_only)
{
	int i;

	/* Display lines until done */
	for (i = 0; i < per_page && (object_idx[object_top + i] >= 0); i++)
	{
		char o_name[MAX_NLEN];
		byte a, c;
		object_kind *flavor_k_ptr;

		/* Get the object index */
		int k_idx = object_idx[object_top + i];

		/* Access the object */
		object_kind *k_ptr = &k_info[k_idx];

		/* Choose a color */
		byte attr = ((k_ptr->aware || visual_only) ? TERM_WHITE : TERM_SLATE);
		byte cursor = ((k_ptr->aware || visual_only) ? TERM_L_BLUE : TERM_BLUE);


		if (!visual_only && k_ptr->flavor)
		{
			/* Appearance of this object is shuffled */
			flavor_k_ptr = &k_info[k_ptr->flavor];
		}
		else
		{
			/* Appearance of this object is very normal */
			flavor_k_ptr = k_ptr;
		}



		attr = ((i + object_top == object_cur) ? cursor : attr);

		if (!k_ptr->flavor || (!visual_only && k_ptr->aware))
		{
			/* Tidy name */
			strip_name(o_name, k_idx);
		}
		else
		{
			/* Flavor name */
			strcpy(o_name, k_name + flavor_k_ptr->flavor_name);
		}

		/* Display the name */
		c_prt(attr, o_name, row + i, col);

		/* Hack -- visual_list mode */
		if (per_page == 1)
		{
			c_prt(attr, format("%02x/%02x", flavor_k_ptr->x_attr, flavor_k_ptr->x_char), row + i, (p_ptr->wizard || visual_only) ? 64 : 68);
		}
		if (p_ptr->wizard || visual_only)
		{
			c_prt(attr, format("%d", k_idx), row + i, 70);
		}

		a = flavor_k_ptr->x_attr;
		c = flavor_k_ptr->x_char;

		/* Display symbol */
		Term_queue_bigchar(use_bigtile ? 76 : 77, row + i, a, c, 0, 0);
	}

	/* Clear remaining lines */
	for (; i < per_page; i++)
	{
		Term_erase(col, row + i, 255);
	}
}

/*
 * Describe fake object
 */
static void desc_obj_fake(int k_idx)
{
	object_type *o_ptr;
	object_type object_type_body;

	/* Get local object */
	o_ptr = &object_type_body;

	/* Wipe the object */
	object_wipe(o_ptr);

	/* Create the artifact */
	object_prep(o_ptr, k_idx);

	/* It's fully know */
	o_ptr->ident |= IDENT_KNOWN;

	/* Track the object */
	/* object_actual_track(o_ptr); */

	/* Hack - mark as fake */
	/* term_obj_real = FALSE; */

	/* Hack -- Handle stuff */
	handle_stuff();

	if (!screen_object(o_ptr, SCROBJ_FAKE_OBJECT | SCROBJ_FORCE_DETAIL))
	{
#ifdef JP
		msg_print("���ɕς�����Ƃ���͂Ȃ��悤���B");
#else
		msg_print("You see nothing special.");
#endif
		msg_print(NULL);
	}
}



/*
 * Display known objects
 */
/*:::����́��Ŕ��ʍς݂̃A�C�e�����ꗗ�\������*/
///pend ���ʍς݃A�C�e���ꗗ�@���܂��\������Ȃ�
static void do_cmd_knowledge_objects(bool *need_redraw, bool visual_only, int direct_k_idx)
{
	int i, len, max;
	int grp_cur, grp_top, old_grp_cur;
	int object_old, object_cur, object_top;
	int grp_cnt, grp_idx[100];
	int object_cnt;
	int *object_idx;

	int column = 0;
	bool flag;
	bool redraw;

	bool visual_list = FALSE;
	byte attr_top = 0, char_left = 0;

	int browser_rows;
	int wid, hgt;

	byte mode;

	/* Get size */
	Term_get_size(&wid, &hgt);

	browser_rows = hgt - 8;

	/* Allocate the "object_idx" array */
	C_MAKE(object_idx, max_k_idx, int);

	max = 0;
	grp_cnt = 0;

	if (direct_k_idx < 0)
	{
		mode = visual_only ? 0x03 : 0x01;

		/* Check every group */
		for (i = 0; object_group_text[i] != NULL; i++)
		{
			/* Measure the label */
			len = strlen(object_group_text[i]);

			/* Save the maximum length */
			if (len > max) max = len;

			/* See if any monsters are known */
			if (collect_objects(i, object_idx, mode))
			{
				/* Build a list of groups with known monsters */
				grp_idx[grp_cnt++] = i;
			}
		}

		object_old = -1;
		object_cnt = 0;
	}
	else
	{
		object_kind *k_ptr = &k_info[direct_k_idx];
		object_kind *flavor_k_ptr;

		if (!visual_only && k_ptr->flavor)
		{
			/* Appearance of this object is shuffled */
			flavor_k_ptr = &k_info[k_ptr->flavor];
		}
		else
		{
			/* Appearance of this object is very normal */
			flavor_k_ptr = k_ptr;
		}

		object_idx[0] = direct_k_idx;
		object_old = direct_k_idx;
		object_cnt = 1;

		/* Terminate the list */
		object_idx[1] = -1;

		(void)visual_mode_command('v', &visual_list, browser_rows - 1, wid - (max + 3),
			&attr_top, &char_left, &flavor_k_ptr->x_attr, &flavor_k_ptr->x_char, need_redraw);
	}

	/* Terminate the list */
	grp_idx[grp_cnt] = -1;

	old_grp_cur = -1;
	grp_cur = grp_top = 0;
	object_cur = object_top = 0;

	flag = FALSE;
	redraw = TRUE;

	mode = visual_only ? 0x02 : 0x00;

	while (!flag)
	{
		char ch;
		object_kind *k_ptr, *flavor_k_ptr;

		if (redraw)
		{
			clear_from(0);

#ifdef JP
			prt(format("%s - �A�C�e��", !visual_only ? "�m��" : "�\��"), 2, 0);
			if (direct_k_idx < 0) prt("�O���[�v", 4, 0);
			prt("���O", 4, max + 3);
			if (p_ptr->wizard || visual_only) prt("Idx", 4, 70);
			prt("����", 4, 74);
#else
			prt(format("%s - objects", !visual_only ? "Knowledge" : "Visuals"), 2, 0);
			if (direct_k_idx < 0) prt("Group", 4, 0);
			prt("Name", 4, max + 3);
			if (p_ptr->wizard || visual_only) prt("Idx", 4, 70);
			prt("Sym", 4, 75);
#endif

			for (i = 0; i < 78; i++)
			{
				Term_putch(i, 5, TERM_WHITE, '=');
			}

			if (direct_k_idx < 0)
			{
				for (i = 0; i < browser_rows; i++)
				{
					Term_putch(max + 1, 6 + i, TERM_WHITE, '|');
				}
			}

			redraw = FALSE;
		}

		if (direct_k_idx < 0)
		{
			/* Scroll group list */
			if (grp_cur < grp_top) grp_top = grp_cur;
			if (grp_cur >= grp_top + browser_rows) grp_top = grp_cur - browser_rows + 1;

			/* Display a list of object groups */
			display_group_list(0, 6, max, browser_rows, grp_idx, object_group_text, grp_cur, grp_top);

			if (old_grp_cur != grp_cur)
			{
				old_grp_cur = grp_cur;

				/* Get a list of objects in the current group */
				object_cnt = collect_objects(grp_idx[grp_cur], object_idx, mode);
			}

			/* Scroll object list */
			while (object_cur < object_top)
				object_top = MAX(0, object_top - browser_rows/2);
			while (object_cur >= object_top + browser_rows)
				object_top = MIN(object_cnt - browser_rows, object_top + browser_rows/2);
		}

		if (!visual_list)
		{
			/* Display a list of objects in the current group */
			display_object_list(max + 3, 6, browser_rows, object_idx, object_cur, object_top, visual_only);
		}
		else
		{
			object_top = object_cur;

			/* Display a list of objects in the current group */
			display_object_list(max + 3, 6, 1, object_idx, object_cur, object_top, visual_only);

			/* Display visual list below first object */
			display_visual_list(max + 3, 7, browser_rows-1, wid - (max + 3), attr_top, char_left);
		}

		/* Get the current object */
		k_ptr = &k_info[object_idx[object_cur]];

		if (!visual_only && k_ptr->flavor)
		{
			/* Appearance of this object is shuffled */
			flavor_k_ptr = &k_info[k_ptr->flavor];
		}
		else
		{
			/* Appearance of this object is very normal */
			flavor_k_ptr = k_ptr;
		}

		/* Prompt */
#ifdef JP
		prt(format("<����>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r'�ŏڍׂ�����" : "",
			visual_list ? ", ENTER�Ō���" : ", 'v'�ŃV���{���ύX",
			(attr_idx || char_idx) ? ", 'c', 'p'�Ńy�[�X�g" : ", 'c'�ŃR�s�["),
			hgt - 1, 0);
#else
		prt(format("<dir>%s%s%s, ESC",
			(!visual_list && !visual_only) ? ", 'r' to recall" : "",
			visual_list ? ", ENTER to accept" : ", 'v' for visuals",
			(attr_idx || char_idx) ? ", 'c', 'p' to paste" : ", 'c' to copy"),
			hgt - 1, 0);
#endif

		if (!visual_only)
		{
			/* Mega Hack -- track this object */
			if (object_cnt) object_kind_track(object_idx[object_cur]);

			/* The "current" object changed */
			if (object_old != object_idx[object_cur])
			{
				/* Hack -- handle stuff */
				handle_stuff();

				/* Remember the "current" object */
				object_old = object_idx[object_cur];
			}
		}

		if (visual_list)
		{
			place_visual_list_cursor(max + 3, 7, flavor_k_ptr->x_attr, flavor_k_ptr->x_char, attr_top, char_left);
		}
		else if (!column)
		{
			Term_gotoxy(0, 6 + (grp_cur - grp_top));
		}
		else
		{
			Term_gotoxy(max + 3, 6 + (object_cur - object_top));
		}

		ch = inkey();

		/* Do visual mode command if needed */
		if (visual_mode_command(ch, &visual_list, browser_rows-1, wid - (max + 3), &attr_top, &char_left, &flavor_k_ptr->x_attr, &flavor_k_ptr->x_char, need_redraw))
		{
			if (direct_k_idx >= 0)
			{
				switch (ch)
				{
				case '\n':
				case '\r':
				case ESCAPE:
					flag = TRUE;
					break;
				}
			}
			continue;
		}

		switch (ch)
		{
			case ESCAPE:
			{
				flag = TRUE;
				break;
			}

			case 'R':
			case 'r':
			{
				/* Recall on screen */
				if (!visual_list && !visual_only && (grp_cnt > 0))
				{
					desc_obj_fake(object_idx[object_cur]);
					redraw = TRUE;
				}
				break;
			}

			default:
			{
				/* Move the cursor */
				browser_cursor(ch, &column, &grp_cur, grp_cnt, &object_cur, object_cnt);
				break;
			}
		}
	}

	/* Free the "object_idx" array */
	C_KILL(object_idx, max_k_idx, int);
}


/*
 * Display the features in a group.
 */
static void display_feature_list(int col, int row, int per_page, int *feat_idx,
	int feat_cur, int feat_top, bool visual_only, int lighting_level)
{
	int lit_col[F_LIT_MAX], i, j;
	int f_idx_col = use_bigtile ? 62 : 64;

	/* Correct columns 1 and 4 */
	lit_col[F_LIT_STANDARD] = use_bigtile ? (71 - F_LIT_MAX) : 71;
	for (i = F_LIT_NS_BEGIN; i < F_LIT_MAX; i++)
		lit_col[i] = lit_col[F_LIT_STANDARD] + 2 + (i - F_LIT_NS_BEGIN) * 2 + (use_bigtile ? i : 0);

	/* Display lines until done */
	for (i = 0; i < per_page && (feat_idx[feat_top + i] >= 0); i++)
	{
		byte attr;

		/* Get the index */
		int f_idx = feat_idx[feat_top + i];

		/* Access the index */
		feature_type *f_ptr = &f_info[f_idx];

		int row_i = row + i;

		/* Choose a color */
		attr = ((i + feat_top == feat_cur) ? TERM_L_BLUE : TERM_WHITE);

		/* Display the name */
		c_prt(attr, f_name + f_ptr->name, row_i, col);

		/* Hack -- visual_list mode */
		if (per_page == 1)
		{
			/* Display lighting level */
			c_prt(attr, format("(%s)", lighting_level_str[lighting_level]), row_i, col + 1 + strlen(f_name + f_ptr->name));

			c_prt(attr, format("%02x/%02x", f_ptr->x_attr[lighting_level], f_ptr->x_char[lighting_level]), row_i, f_idx_col - ((p_ptr->wizard || visual_only) ? 6 : 2));
		}
		if (p_ptr->wizard || visual_only)
		{
			c_prt(attr, format("%d", f_idx), row_i, f_idx_col);
		}

		/* Display symbol */
		Term_queue_bigchar(lit_col[F_LIT_STANDARD], row_i, f_ptr->x_attr[F_LIT_STANDARD], f_ptr->x_char[F_LIT_STANDARD], 0, 0);

		Term_putch(lit_col[F_LIT_NS_BEGIN], row_i, TERM_SLATE, '(');
		for (j = F_LIT_NS_BEGIN + 1; j < F_LIT_MAX; j++)
		{
			Term_putch(lit_col[j], row_i, TERM_SLATE, '/');
		}
		Term_putch(lit_col[F_LIT_MAX - 1] + (use_bigtile ? 3 : 2), row_i, TERM_SLATE, ')');

		/* Mega-hack -- Use non-standard colour */
		for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
		{
			Term_queue_bigchar(lit_col[j] + 1, row_i, f_ptr->x_attr[j], f_ptr->x_char[j], 0, 0);
		}
	}

	/* Clear remaining lines */
	for (; i < per_page; i++)
	{
		Term_erase(col, row + i, 255);
	}
}


/*
 * Interact with feature visuals.
 */
/*:::�n�`�ꗗ�\��*/
static void do_cmd_knowledge_features(bool *need_redraw, bool visual_only, int direct_f_idx, int *lighting_level)
{
	int i, len, max;
	int grp_cur, grp_top, old_grp_cur;
	int feat_cur, feat_top;
	int grp_cnt, grp_idx[100];
	int feat_cnt;
	int *feat_idx;

	int column = 0;
	bool flag;
	bool redraw;

	bool visual_list = FALSE;
	byte attr_top = 0, char_left = 0;

	int browser_rows;
	int wid, hgt;

	byte attr_old[F_LIT_MAX];
	byte char_old[F_LIT_MAX];
	byte *cur_attr_ptr, *cur_char_ptr;

	(void)C_WIPE(attr_old, F_LIT_MAX, byte);
	(void)C_WIPE(char_old, F_LIT_MAX, byte);

	/* Get size */
	Term_get_size(&wid, &hgt);

	browser_rows = hgt - 8;

	/* Allocate the "feat_idx" array */
	C_MAKE(feat_idx, max_f_idx, int);

	max = 0;
	grp_cnt = 0;

	if (direct_f_idx < 0)
	{
		/* Check every group */
		for (i = 0; feature_group_text[i] != NULL; i++)
		{
			/* Measure the label */
			len = strlen(feature_group_text[i]);

			/* Save the maximum length */
			if (len > max) max = len;

			/* See if any features are known */
			if (collect_features(i, feat_idx, 0x01))
			{
				/* Build a list of groups with known features */
				grp_idx[grp_cnt++] = i;
			}
		}

		feat_cnt = 0;
	}
	else
	{
		feature_type *f_ptr = &f_info[direct_f_idx];

		feat_idx[0] = direct_f_idx;
		feat_cnt = 1;

		/* Terminate the list */
		feat_idx[1] = -1;

		(void)visual_mode_command('v', &visual_list, browser_rows - 1, wid - (max + 3),
			&attr_top, &char_left, &f_ptr->x_attr[*lighting_level], &f_ptr->x_char[*lighting_level], need_redraw);

		for (i = 0; i < F_LIT_MAX; i++)
		{
			attr_old[i] = f_ptr->x_attr[i];
			char_old[i] = f_ptr->x_char[i];
		}
	}

	/* Terminate the list */
	grp_idx[grp_cnt] = -1;

	old_grp_cur = -1;
	grp_cur = grp_top = 0;
	feat_cur = feat_top = 0;

	flag = FALSE;
	redraw = TRUE;

	while (!flag)
	{
		char ch;
		feature_type *f_ptr;

		if (redraw)
		{
			clear_from(0);

#ifdef JP
			prt("�\�� - �n�`", 2, 0);
			if (direct_f_idx < 0) prt("�O���[�v", 4, 0);
			prt("���O", 4, max + 3);
			if (use_bigtile)
			{
				if (p_ptr->wizard || visual_only) prt("Idx", 4, 62);
				prt("���� ( l/ d)", 4, 66);
			}
			else
			{
				if (p_ptr->wizard || visual_only) prt("Idx", 4, 64);
				prt("���� (l/d)", 4, 68);
			}
#else
			prt("Visuals - features", 2, 0);
			if (direct_f_idx < 0) prt("Group", 4, 0);
			prt("Name", 4, max + 3);
			if (use_bigtile)
			{
				if (p_ptr->wizard || visual_only) prt("Idx", 4, 62);
				prt("Sym ( l/ d)", 4, 67);
			}
			else
			{
				if (p_ptr->wizard || visual_only) prt("Idx", 4, 64);
				prt("Sym (l/d)", 4, 69);
			}
#endif

			for (i = 0; i < 78; i++)
			{
				Term_putch(i, 5, TERM_WHITE, '=');
			}

			if (direct_f_idx < 0)
			{
				for (i = 0; i < browser_rows; i++)
				{
					Term_putch(max + 1, 6 + i, TERM_WHITE, '|');
				}
			}

			redraw = FALSE;
		}

		if (direct_f_idx < 0)
		{
			/* Scroll group list */
			if (grp_cur < grp_top) grp_top = grp_cur;
			if (grp_cur >= grp_top + browser_rows) grp_top = grp_cur - browser_rows + 1;

			/* Display a list of feature groups */
			display_group_list(0, 6, max, browser_rows, grp_idx, feature_group_text, grp_cur, grp_top);

			if (old_grp_cur != grp_cur)
			{
				old_grp_cur = grp_cur;

				/* Get a list of features in the current group */
				feat_cnt = collect_features(grp_idx[grp_cur], feat_idx, 0x00);
			}

			/* Scroll feature list */
			while (feat_cur < feat_top)
				feat_top = MAX(0, feat_top - browser_rows/2);
			while (feat_cur >= feat_top + browser_rows)
				feat_top = MIN(feat_cnt - browser_rows, feat_top + browser_rows/2);
		}

		if (!visual_list)
		{
			/* Display a list of features in the current group */
			display_feature_list(max + 3, 6, browser_rows, feat_idx, feat_cur, feat_top, visual_only, F_LIT_STANDARD);
		}
		else
		{
			feat_top = feat_cur;

			/* Display a list of features in the current group */
			display_feature_list(max + 3, 6, 1, feat_idx, feat_cur, feat_top, visual_only, *lighting_level);

			/* Display visual list below first object */
			display_visual_list(max + 3, 7, browser_rows-1, wid - (max + 3), attr_top, char_left);
		}

		/* Prompt */
#ifdef JP
		prt(format("<����>%s, 'd'�ŕW����������%s, ESC",
			visual_list ? ", ENTER�Ō���, 'a'�őΏۖ��x�ύX" : ", 'v'�ŃV���{���ύX",
			(attr_idx || char_idx) ? ", 'c', 'p'�Ńy�[�X�g" : ", 'c'�ŃR�s�["),
			hgt - 1, 0);
#else
		prt(format("<dir>%s, 'd' for default lighting%s, ESC",
			visual_list ? ", ENTER to accept, 'a' for lighting level" : ", 'v' for visuals",
			(attr_idx || char_idx) ? ", 'c', 'p' to paste" : ", 'c' to copy"),
			hgt - 1, 0);
#endif

		/* Get the current feature */
		f_ptr = &f_info[feat_idx[feat_cur]];
		cur_attr_ptr = &f_ptr->x_attr[*lighting_level];
		cur_char_ptr = &f_ptr->x_char[*lighting_level];

		if (visual_list)
		{
			place_visual_list_cursor(max + 3, 7, *cur_attr_ptr, *cur_char_ptr, attr_top, char_left);
		}
		else if (!column)
		{
			Term_gotoxy(0, 6 + (grp_cur - grp_top));
		}
		else
		{
			Term_gotoxy(max + 3, 6 + (feat_cur - feat_top));
		}

		ch = inkey();

		if (visual_list && ((ch == 'A') || (ch == 'a')))
		{
			int prev_lighting_level = *lighting_level;

			if (ch == 'A')
			{
				if (*lighting_level <= 0) *lighting_level = F_LIT_MAX - 1;
				else (*lighting_level)--;
			}
			else
			{
				if (*lighting_level >= F_LIT_MAX - 1) *lighting_level = 0;
				else (*lighting_level)++;
			}

			if (f_ptr->x_attr[prev_lighting_level] != f_ptr->x_attr[*lighting_level])
				attr_top = MAX(0, (f_ptr->x_attr[*lighting_level] & 0x7f) - 5);

			if (f_ptr->x_char[prev_lighting_level] != f_ptr->x_char[*lighting_level])
				char_left = MAX(0, f_ptr->x_char[*lighting_level] - 10);

			continue;
		}

		else if ((ch == 'D') || (ch == 'd'))
		{
			byte prev_x_attr = f_ptr->x_attr[*lighting_level];
			byte prev_x_char = f_ptr->x_char[*lighting_level];

			apply_default_feat_lighting(f_ptr->x_attr, f_ptr->x_char);

			if (visual_list)
			{
				if (prev_x_attr != f_ptr->x_attr[*lighting_level])
					 attr_top = MAX(0, (f_ptr->x_attr[*lighting_level] & 0x7f) - 5);

				if (prev_x_char != f_ptr->x_char[*lighting_level])
					char_left = MAX(0, f_ptr->x_char[*lighting_level] - 10);
			}
			else *need_redraw = TRUE;

			continue;
		}

		/* Do visual mode command if needed */
		else if (visual_mode_command(ch, &visual_list, browser_rows-1, wid - (max + 3), &attr_top, &char_left, cur_attr_ptr, cur_char_ptr, need_redraw))
		{
			switch (ch)
			{
			/* Restore previous visual settings */
			case ESCAPE:
				for (i = 0; i < F_LIT_MAX; i++)
				{
					f_ptr->x_attr[i] = attr_old[i];
					f_ptr->x_char[i] = char_old[i];
				}

				/* Fall through */

			case '\n':
			case '\r':
				if (direct_f_idx >= 0) flag = TRUE;
				else *lighting_level = F_LIT_STANDARD;
				break;

			/* Preserve current visual settings */
			case 'V':
			case 'v':
				for (i = 0; i < F_LIT_MAX; i++)
				{
					attr_old[i] = f_ptr->x_attr[i];
					char_old[i] = f_ptr->x_char[i];
				}
				*lighting_level = F_LIT_STANDARD;
				break;

			case 'C':
			case 'c':
				if (!visual_list)
				{
					for (i = 0; i < F_LIT_MAX; i++)
					{
						attr_idx_feat[i] = f_ptr->x_attr[i];
						char_idx_feat[i] = f_ptr->x_char[i];
					}
				}
				break;

			case 'P':
			case 'p':
				if (!visual_list)
				{
					/* Allow TERM_DARK text */
					for (i = F_LIT_NS_BEGIN; i < F_LIT_MAX; i++)
					{
						if (attr_idx_feat[i] || (!(char_idx_feat[i] & 0x80) && char_idx_feat[i])) f_ptr->x_attr[i] = attr_idx_feat[i];
						if (char_idx_feat[i]) f_ptr->x_char[i] = char_idx_feat[i];
					}
				}
				break;
			}
			continue;
		}

		switch (ch)
		{
			case ESCAPE:
			{
				flag = TRUE;
				break;
			}

			default:
			{
				/* Move the cursor */
				browser_cursor(ch, &column, &grp_cur, grp_cnt, &feat_cur, feat_cnt);
				break;
			}
		}
	}

	/* Free the "feat_idx" array */
	C_KILL(feat_idx, max_f_idx, int);
}


/*
 * List wanted monsters
 */
///del131221 �܋���ꗗ
#if 0
static void do_cmd_knowledge_kubi(void)
{
	int i;
	FILE *fff;
	
	char file_name[1024];
	
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
		bool listed = FALSE;

#ifdef JP
		fprintf(fff, "�����̃^�[�Q�b�g : %s\n", (p_ptr->today_mon ? r_name + r_info[p_ptr->today_mon].name : "�s��"));
		fprintf(fff, "\n");
		fprintf(fff, "�܋��񃊃X�g\n");
#else
		fprintf(fff, "Today target : %s\n", (p_ptr->today_mon ? r_name + r_info[p_ptr->today_mon].name : "unknown"));
		fprintf(fff, "\n");
		fprintf(fff, "List of wanted monsters\n");
#endif
		fprintf(fff, "----------------------------------------------\n");

		for (i = 0; i < MAX_KUBI; i++)
		{
			if (kubi_r_idx[i] <= 10000)
			{
				fprintf(fff,"%s\n", r_name + r_info[kubi_r_idx[i]].name);

				listed = TRUE;
			}
		}

		if (!listed)
		{
#ifdef JP
			fprintf(fff,"\n%s\n", "�܋���͂����c���Ă��܂���B");
#else
			fprintf(fff,"\n%s\n", "There is no more wanted monster.");
#endif
		}
	}
	
	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�܋���̈ꗗ", 0, 0);
#else
	show_file(TRUE, file_name, "Wanted monsters", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}
#endif

#if 0
/*
 * List virtues & status
 */
///sysdel ���ꗗ
static void do_cmd_knowledge_virtues(void)
{
	FILE *fff;
	
	char file_name[1024];
	
	
	///sys131221 ���\���y�[�W������
	msg_format("�u���v�̃V�X�e���͔p�~���܂����B");
	msg_print(NULL);
	return;



	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
#ifdef JP
		fprintf(fff, "���݂̑��� : %s\n\n", your_alignment());
#else
		fprintf(fff, "Your alighnment : %s\n\n", your_alignment());
#endif
		dump_virtues(fff);
	}
	
	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���̓�", 0, 0);
#else
	show_file(TRUE, file_name, "Virtues", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}
#endif
/*
* Dungeon
*
*/
static void do_cmd_knowledge_dungeon(void)
{
	FILE *fff;
	
	char file_name[1024];
	int i;
	
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
		for (i = 1; i < max_d_idx; i++)
		{
			bool seiha = FALSE;

			//v1.1.25 �_���W�������e�t���O
			if(flag_dungeon_complete[i]) seiha = TRUE;
#if 0
			if (!d_info[i].maxdepth) continue;
			if (!max_dlv[i]) continue;
			if (d_info[i].final_guardian)
			{
				if (!r_info[d_info[i].final_guardian].max_num) seiha = TRUE;
			}
			else if(i == DUNGEON_ANGBAND && quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED) seiha = TRUE;
			else if(i == DUNGEON_CHAOS && quest[QUEST_SERPENT].status == QUEST_STATUS_FINISHED) seiha = TRUE;
			else if (max_dlv[i] == d_info[i].maxdepth) seiha = TRUE;
#endif
#ifdef JP
			fprintf(fff,"%c%-20s :  %3d �K\n", seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#else
			fprintf(fff,"%c%-16s :  level %3d\n", seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#endif
		}
	}
	
	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���܂łɓ������_���W����", 0, 0);
#else
	show_file(TRUE, file_name, "Dungeon", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
* List virtues & status
*
*/
///sys �̗̓����N�A�p�����[�^MAX�̕\��
static void do_cmd_knowledge_stat(void)
{
	FILE *fff;
	
	char file_name[1024];
	int percent, v_nr;
	int i,mult_race = race_info[p_ptr->prace].score_mult;
	int mult_race_class;
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
		percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
			(2 * p_ptr->hitdie +
			((PY_MAX_LEVEL - 1 + 3) * (p_ptr->hitdie + 1))));
		///mod140104 �p�����[�^����ƕ\�L��ύX�@�\�͍ő�l�̕\�L���Ƃ肠����������
#ifdef JP
		if (p_ptr->knowledge & KNOW_HPRATE) fprintf(fff, "���݂̗̑̓����N : %d/100\n", percent);
		else fprintf(fff, "���݂̗̑̓����N : ???\n");
		//		fprintf(fff, "�\�͂̍ő�l\n\n");

				//v1.1.42 �X�R�A�v�Z�Ɋւ���ꕔ������ʊ֐��ɕ���
		mult_race_class = calc_player_score_mult();

		/*
		for (i = 0; i < MAX_RACES; i++)
		{
			if (i < 32)
			{
				if (!(p_ptr->old_race1 & 1L << i) || mult_race < race_info[i].score_mult) continue;
			}
			else
			{
				if (!(p_ptr->old_race2 & 1L << (i-32)) || mult_race < race_info[i].score_mult) continue;
			}
			mult_race = race_info[i].score_mult;
		}
		mult_race_class = mult_race * class_info[p_ptr->pclass].score_mult / 100;

		//���g�X�R�A����
		if(p_ptr->pclass == CLASS_MOKOU && p_ptr->magic_num1[0])
			mult_race_class /= (1 + MIN(99,p_ptr->magic_num1[0]));

		if(p_ptr->pclass == CLASS_KAGUYA && kaguya_quest_level && !ironman_no_fixed_art)
		{
			//�P��͏��������_��̐��ŃX�R�A����
			for(i=0;i<kaguya_quest_level;i++) mult_race_class /= 2;
		}

		if(p_ptr->pseikaku == SEIKAKU_BERSERK) mult_race_class /= 2;

		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO))
			mult_race_class /= 2;

		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			mult_race_class = mult_race_class * 3 / 4;
		*/

		extra_mode_score_mult(&mult_race_class, p_ptr->pclass);
		fprintf(fff, "���Ȃ��̃X�R�A�{�� : %d%%\n", mult_race_class);

		//v1.1.79 ���ݎg�p���̈ړ����~��\������
		if (p_ptr->prace == RACE_ZASHIKIWARASHI)
		{
			char o_name[MAX_NLEN];
			object_type *o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

			object_desc(o_name, o_ptr, OD_NAME_AND_ENCHANT);
			fprintf(fff, "���Ȃ��̈ړ����~ : %s\n",o_name);

		}




		fprintf(fff, "\n");

#else
		if (p_ptr->knowledge & KNOW_HPRATE) fprintf(fff, "Your current Life Rating is %d/100.\n\n", percent);
		else fprintf(fff, "Your current Life Rating is ???.\n\n");
		fprintf(fff, "Limits of maximum stats\n\n");
#endif


//		for (v_nr = 0; v_nr < 6; v_nr++)
//		{
//			if ((p_ptr->knowledge & KNOW_STAT) || p_ptr->stat_max[v_nr] == p_ptr->stat_max_max[v_nr]) fprintf(fff, "%s 18/%d\n", stat_names[v_nr], p_ptr->stat_max_max[v_nr]-18);
//			else fprintf(fff, "%s ???\n", stat_names[v_nr]);
//		}
	}

	dump_yourself(fff);

	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�����Ɋւ�����", 0, 0);
#else
	show_file(TRUE, file_name, "HP-rate & Max stat", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Print all active quests
 */
static void do_cmd_knowledge_quests_current(FILE *fff)
{
	char tmp_str[120];
	char rand_tmp_str[120] = "\0";
	char name[80];
	monster_race *r_ptr;
	int i;
	int rand_level = 100;
	int total = 0;

#ifdef JP
	fprintf(fff, "�s���s���̃N�G�X�g�t\n");
#else
	fprintf(fff, "< Current Quest >\n");
#endif

	for (i = 1; i < max_quests; i++)
	{
		if ((quest[i].status == QUEST_STATUS_TAKEN) ||
			((quest[i].status == QUEST_STATUS_STAGE_COMPLETED) && (quest[i].type == QUEST_TYPE_TOWER)) ||
			(quest[i].status == QUEST_STATUS_COMPLETED))
		{
			/* Set the quest number temporary */
			int old_quest = p_ptr->inside_quest;
			int j;

			/* Clear the text */
			for (j = 0; j < 10; j++) quest_text[j][0] = '\0';
			quest_text_line = 0;

			p_ptr->inside_quest = i;

			/* Get the quest text */
			init_flags = INIT_SHOW_TEXT;

			if(EXTRA_MODE && !is_fixed_quest_idx(i))
				process_dungeon_file("q_random.txt", 0, 0, 0, 0);
			else
				process_dungeon_file("q_info.txt", 0, 0, 0, 0);

			///mod140512 ���ꃋ�[�`���ǉ��@��������̃��j�[�N�v���C���[�̂Ƃ��N�G�X�g���͂��s���R�ɂȂ�̂ŏ���������
			(void)check_quest_unique_text();

			/* Reset the old quest number */
			p_ptr->inside_quest = old_quest;

			/* No info from "silent" quests */
			if (quest[i].flags & QUEST_FLAG_SILENT) continue;

			//�܂��K�ɓ��B���Ă��Ȃ��G�N�X�g���N�G�X�g�͕\�����Ȃ�
			if(EXTRA_MODE && !is_fixed_quest_idx(i) && dun_level < quest[i].level) continue;


			total++;

			if (quest[i].type != QUEST_TYPE_RANDOM)
			{
				char note[80] = "\0";

				if (quest[i].status == QUEST_STATUS_TAKEN || quest[i].status == QUEST_STATUS_STAGE_COMPLETED)
				{
					switch (quest[i].type)
					{
					case QUEST_TYPE_KILL_LEVEL:
					case QUEST_TYPE_KILL_ANY_LEVEL:
						r_ptr = &r_info[quest[i].r_idx];
						strcpy(name, r_name + r_ptr->name);
						if (quest[i].max_num > 1)
						{
#ifdef JP
							sprintf(note," - %d �̂�%s��|���B(���� %d ��)",
								quest[i].max_num, name, quest[i].max_num - quest[i].cur_num);
#else
							plural_aux(name);
							sprintf(note," - kill %d %s, have killed %d.",
								quest[i].max_num, name, quest[i].cur_num);
#endif
						}
						else if (i == QUEST_DREAMDWELLER)
						{
							if(is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
								sprintf(note, " - ���Ȃ��̈�����|���B");
							else
								sprintf(note, " - ���̐��E�̂��Ȃ���|���B");
						}
						else
#ifdef JP
							sprintf(note," - %s��|���B",name);
#else
							sprintf(note," - kill %s.",name);
#endif
						break;
						//�ʏ�A�C�e�����W�@���̂Ƃ��떂������p
					case QUEST_TYPE_FIND_OBJECT:
						sprintf(note," - �ʔ������Ȗ{�������o���B");
						break;

					case QUEST_TYPE_FIND_ARTIFACT:
						if (quest[i].k_idx)
						{
							artifact_type *a_ptr = &a_info[quest[i].k_idx];
							object_type forge;
							object_type *q_ptr = &forge;
							int k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
							object_prep(q_ptr, k_idx);
							q_ptr->name1 = quest[i].k_idx;
							q_ptr->ident = IDENT_STORE;
							object_desc(name, q_ptr, OD_NAME_ONLY);
						}
						sprintf(note," - %s�������o���B", name);
						break;
						//�󕨌ɂ����Ȃ��̂Ƃ�
					case QUEST_TYPE_FIND_RANDOM_ARTIFACT:
						if (quest[i].k_idx)
						{
							sprintf(note," - �`����%s�������o���B", (k_name + k_info[quest[i].k_idx].name));
						}
						break;

					case QUEST_TYPE_FIND_EXIT:
#ifdef JP
						sprintf(note," - �o���ɓ��B����B");
#else
						sprintf(note," - Reach to Exit.");
#endif
						break;
					case QUEST_TYPE_DELIVER_ITEM:

						if(i == QUEST_KAGUYA)
						{
							///mod151219 �S�B����ɋ��t���ǉ�
							if(kaguya_quest_level < KAGUYA_QUEST_MAX)
								sprintf(note,format(" - %s��T���o���B",a_name+a_info[kaguya_quest_art_list[kaguya_quest_level]].name));
							else
								sprintf(note,format(" - %s��T���o���B",k_name+k_info[lookup_kind(TV_SOUVENIR,SV_SOUVENIR_KINKAKUJI)].name));
						}
						else
							sprintf(note," - �w�肳�ꂽ�A�C�e����͂���B");

						break;
					case QUEST_TYPE_BOUNTY_HUNT:
						r_ptr = &r_info[quest[i].r_idx];
						strcpy(name, r_name + r_ptr->name);
						sprintf(note," - �_���W������%s��|���B",name);
						break;

					case QUEST_TYPE_KILL_NUMBER:
#ifdef JP
						sprintf(note," - %d �̂̃����X�^�[��|���B(���� %d ��)",
							quest[i].max_num, quest[i].max_num - quest[i].cur_num);
#else
						sprintf(note," - Kill %d monsters, have killed %d.",
							quest[i].max_num, quest[i].cur_num);
#endif
						break;

					case QUEST_TYPE_KILL_ALL:
					case QUEST_TYPE_TOWER:
#ifdef JP
						sprintf(note," - �S�Ẵ����X�^�[��|���B");
#else
						sprintf(note," - Kill all monsters.");
#endif
						break;
					}
				}

				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %s (�댯�x:%d�K����)%s\n",
					quest[i].name, quest[i].level, note);
#else
				sprintf(tmp_str, "  %s (Danger level: %d)%s\n",
					quest[i].name, quest[i].level, note);
#endif

				fputs(tmp_str, fff);

				if (quest[i].status == QUEST_STATUS_COMPLETED)
				{

					if(i == QUEST_MOON_VAULT || i == QUEST_MARICON)
						sprintf(tmp_str, "    �N�G�X�g�B�� - �܂��E�o���Ă��Ȃ��B\n");
					else
						sprintf(tmp_str, "    �N�G�X�g�B�� - �܂���V���󂯂Ƃ��ĂȂ��B\n");

					fputs(tmp_str, fff);
				}
				else
				{
					j = 0;

					while (quest_text[j][0] && j < 10)
					{
						fprintf(fff, "    %s\n", quest_text[j]);
						j++;
					}
				}
			}
			else if (quest[i].level < rand_level) /* QUEST_TYPE_RANDOM */
			{
				/* New random */
				rand_level = quest[i].level;

				if (max_dlv[DUNGEON_ANGBAND] >= rand_level)
				{
					/* Print the quest info */
					r_ptr = &r_info[quest[i].r_idx];
					strcpy(name, r_name + r_ptr->name);

					if (quest[i].max_num > 1)
					{
#ifdef JP
						sprintf(rand_tmp_str,"  %s (%d �K) - %d �̂�%s��|���B(���� %d ��)\n",
							quest[i].name, quest[i].level,
							quest[i].max_num, name, quest[i].max_num - quest[i].cur_num);
#else
						plural_aux(name);

						sprintf(rand_tmp_str,"  %s (Dungeon level: %d)\n  Kill %d %s, have killed %d.\n",
							quest[i].name, quest[i].level,
							quest[i].max_num, name, quest[i].cur_num);
#endif
					}
					else
					{
#ifdef JP
						sprintf(rand_tmp_str,"  %s (%d �K) - %s��|���B\n",
							quest[i].name, quest[i].level, name);
#else
						sprintf(rand_tmp_str,"  %s (Dungeon level: %d)\n  Kill %s.\n",
							quest[i].name, quest[i].level, name);
#endif
					}
				}
			}
		}
	}

	/* Print the current random quest  */
	if (rand_tmp_str[0]) fputs(rand_tmp_str, fff);

#ifdef JP
	if (!total) fprintf(fff, "  �Ȃ�\n");
#else
	if (!total) fprintf(fff, "  Nothing.\n");
#endif
}


/*
 * Print all finished quests
 */
void do_cmd_knowledge_quests_completed(FILE *fff, int quest_num[])
{
	char tmp_str[160];
	char str_time[20];
	int i;
	int total = 0;

#ifdef JP
	fprintf(fff, "�s�B�������N�G�X�g�t\n");
#else
	fprintf(fff, "< Completed Quest >\n");
#endif
	for (i = 1; i < max_quests; i++)
	{
		int q_idx = quest_num[i];

		///mod150504 Hack - ���͎��N�G�X�g��B���ς݂����X�R�A�Ȃǂɂ͎Q�����Ȃ�
		if((p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI) && q_idx == QUEST_YUKARI)	continue;

		//v1.1.25 �N�G�X�g�B�����ԏ��ǉ�
		if(quest[q_idx].comptime)
		{
			sprintf(str_time,"(%.2lu:%.2lu:%.2lu)", quest[q_idx].comptime/(60*60), (quest[q_idx].comptime/60)%60, quest[q_idx].comptime%60);
		}
		else
		{
			sprintf(str_time,"");
		}


		if (quest[q_idx].status == QUEST_STATUS_FINISHED)
		{
			if (is_fixed_quest_idx(q_idx))
			{
				/* Set the quest number temporary */
				int old_quest = p_ptr->inside_quest;

				p_ptr->inside_quest = q_idx;

				/* Get the quest */
				init_flags = INIT_NAME_ONLY;

				process_dungeon_file("q_info.txt", 0, 0, 0, 0);

				/* Reset the old quest number */
				p_ptr->inside_quest = old_quest;

				/* No info from "silent" quests */
				if (quest[q_idx].flags & QUEST_FLAG_SILENT) continue;
			}
			else if(EXTRA_MODE && !is_fixed_quest_idx(q_idx)) //�G�N�X�g���N�G�X�g
			{
				int old_quest = p_ptr->inside_quest;
				p_ptr->inside_quest = q_idx;
				init_flags = INIT_NAME_ONLY;
				process_dungeon_file("q_random.txt", 0, 0, 0, 0);
				p_ptr->inside_quest = old_quest;
			}

			total++;

			if (!is_fixed_quest_idx(q_idx) && quest[q_idx].r_idx && !EXTRA_MODE)
			{
				/* Print the quest info */

				if (quest[q_idx].complev == 0)
				{
					sprintf(tmp_str,
#ifdef JP
						"  %-40s (%3d�K)            -   �s�폟\n",
#else
						"  %-40s (Dungeon level: %3d) - (Cancelled)\n",
#endif
						r_name+r_info[quest[q_idx].r_idx].name,
						quest[q_idx].level);
				}
				else
				{
					sprintf(tmp_str,
#ifdef JP
						"  %-40s (%3d�K)            - ���x��%2d%s\n",
#else
						"  %-40s (Dungeon level: %3d) - level %2d\n",
#endif
						r_name+r_info[quest[q_idx].r_idx].name,
						quest[q_idx].level,
						quest[q_idx].complev,str_time);
				}
			}
			else
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %-40s (�댯�x:%3d�K����) - ���x��%2d%s\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev,str_time);
#else
				sprintf(tmp_str, "  %-40s (Danger  level: %3d) - level %2d\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev);
#endif
			}

			fputs(tmp_str, fff);
		}
	}
#ifdef JP
	if (!total) fprintf(fff, "  �Ȃ�\n");
#else
	if (!total) fprintf(fff, "  Nothing.\n");
#endif
}


/*
 * Print all failed quests
 */
void do_cmd_knowledge_quests_failed(FILE *fff, int quest_num[])
{
	char tmp_str[120];
	int i;
	int total = 0;

#ifdef JP
	fprintf(fff, "�s���s�����N�G�X�g�t\n");
#else
	fprintf(fff, "< Failed Quest >\n");
#endif
	for (i = 1; i < max_quests; i++)
	{
		int q_idx = quest_num[i];

		if ((quest[q_idx].status == QUEST_STATUS_FAILED_DONE) || (quest[q_idx].status == QUEST_STATUS_FAILED))
		{
			//�N�G�X�g���������Ŏ��s�����ɂ����N�G�X�g�͕\�L���Ȃ�
			if(check_ignoring_quest(q_idx)) continue;

			if (is_fixed_quest_idx(q_idx))
			{
				/* Set the quest number temporary */
				int old_quest = p_ptr->inside_quest;

				p_ptr->inside_quest = q_idx;

				/* Get the quest text */
				init_flags = INIT_NAME_ONLY;

				process_dungeon_file("q_info.txt", 0, 0, 0, 0);

				/* Reset the old quest number */
				p_ptr->inside_quest = old_quest;

				/* No info from "silent" quests */
				if (quest[q_idx].flags & QUEST_FLAG_SILENT) continue;
			}
			else if(EXTRA_MODE && !is_fixed_quest_idx(q_idx)) //�G�N�X�g���N�G�X�g
			{
				int old_quest = p_ptr->inside_quest;
				p_ptr->inside_quest = q_idx;
				init_flags = INIT_NAME_ONLY;
				process_dungeon_file("q_random.txt", 0, 0, 0, 0);
				p_ptr->inside_quest = old_quest;
			}

			total++;

			if (!is_fixed_quest_idx(q_idx) && quest[q_idx].r_idx && !EXTRA_MODE)
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %-40s (%3d�K)            - ���x��%2d\n",
					r_name+r_info[quest[q_idx].r_idx].name, quest[q_idx].level, quest[q_idx].complev);
#else
				sprintf(tmp_str, "  %-40s (Dungeon level: %3d) - level %2d\n",
					r_name+r_info[quest[q_idx].r_idx].name, quest[q_idx].level, quest[q_idx].complev);
#endif
			}
			else
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "  %-40s (�댯�x:%3d�K����) - ���x��%2d\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev);
#else
				sprintf(tmp_str, "  %-40s (Danger  level: %3d) - level %2d\n",
					quest[q_idx].name, quest[q_idx].level, quest[q_idx].complev);
#endif
			}
			fputs(tmp_str, fff);
		}
	}
#ifdef JP
	if (!total) fprintf(fff, "  �Ȃ�\n");
#else
	if (!total) fprintf(fff, "  Nothing.\n");
#endif
}


/*
 * Print all random quests
 */
static void do_cmd_knowledge_quests_wiz_random(FILE *fff)
{
	char tmp_str[120];
	int i;
	int total = 0;

#ifdef JP
	fprintf(fff, "�s�c��̃����_���N�G�X�g�t\n");
#else
	fprintf(fff, "< Remaining Random Quest >\n");
#endif
	for (i = 1; i < max_quests; i++)
	{
		/* No info from "silent" quests */
		if (quest[i].flags & QUEST_FLAG_SILENT) continue;

		if ((quest[i].type == QUEST_TYPE_RANDOM) && (quest[i].status == QUEST_STATUS_TAKEN))
		{
			total++;

			/* Print the quest info */
#ifdef JP
			sprintf(tmp_str, "  %s (%d�K, %s)\n",
				quest[i].name, quest[i].level, r_name+r_info[quest[i].r_idx].name);
#else
			sprintf(tmp_str, "  %s (%d, %s)\n",
				quest[i].name, quest[i].level, r_name+r_info[quest[i].r_idx].name);
#endif
			fputs(tmp_str, fff);
		}
	}
#ifdef JP
	if (!total) fprintf(fff, "  �Ȃ�\n");
#else
	if (!total) fprintf(fff, "  Nothing.\n");
#endif
}


bool ang_sort_comp_quest_num(vptr u, vptr v, int a, int b)
{
	int *q_num = (int *)u;
	quest_type *qa = &quest[q_num[a]];
	quest_type *qb = &quest[q_num[b]];

	/* Unused */
	(void)v;

	//v1.1.69 �����x���Ȃ�B�������������ق�����ɕ\�������悤�ɂ���(time�l���L�^����ĂȂ��Ƃ�����)
	if (qa->comptime && qb->comptime)
	{
		if (qa->comptime <= qb->comptime) return TRUE;

	}
	else
	{
		//v1.1.70 �����x���łȂ��Ă��B�������������ق�����ɕ\�������悤�ɂ����B
		//�B���������L�^����Ă��Ȃ��ꍇ�͒჌�x���Ȃ��̂����ɕ\������
		if (qa->complev < qb->complev) return TRUE;
		if (qa->complev > qb->complev) return FALSE;

		if (qa->level <= qb->level) return TRUE;
	}
	return FALSE;
}

void ang_sort_swap_quest_num(vptr u, vptr v, int a, int b)
{
	int *q_num = (int *)u;
	int tmp;

	/* Unused */
	(void)v;

	tmp = q_num[a];
	q_num[a] = q_num[b];
	q_num[b] = tmp;
}


/*
 * Print quest status of all active quests
 */
/*:::�N�G�X�g����B�����x�����Ƀ\�[�g���ĕ\�����Ă�炵���B*/
static void do_cmd_knowledge_quests(void)
{
	FILE *fff;
	char file_name[1024];
	int *quest_num, dummy, i;

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff)
	{
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Allocate Memory */
	C_MAKE(quest_num, max_quests, int);

	/* Sort by compete level */
	for (i = 1; i < max_quests; i++) quest_num[i] = i;
	ang_sort_comp = ang_sort_comp_quest_num;
	ang_sort_swap = ang_sort_swap_quest_num;
	ang_sort(quest_num, &dummy, max_quests);

	/* Dump Quest Information */
	do_cmd_knowledge_quests_current(fff);
	fputc('\n', fff);
	do_cmd_knowledge_quests_completed(fff, quest_num);
	fputc('\n', fff);
	do_cmd_knowledge_quests_failed(fff, quest_num);
	if (p_ptr->wizard)
	{
		fputc('\n', fff);
		do_cmd_knowledge_quests_wiz_random(fff);
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�N�G�X�g�B����", 0, 0);
#else
	show_file(TRUE, file_name, "Quest status", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);

	/* Free Memory */
	C_KILL(quest_num, max_quests, int);
}


/*
 * List my home
 */
static void do_cmd_knowledge_home(void)
{
	FILE *fff;

	int i;
	char file_name[1024];
	store_type  *st_ptr;
	char o_name[MAX_NLEN];
	cptr		paren = ")";

	process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
		msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
		msg_format("Failed to create temporary file %s.", file_name);
#endif
		msg_print(NULL);
		return;
	}

	if (fff)
	{
		/* Print all homes in the different towns */
		st_ptr = &town[1].store[STORE_HOME];

		/* Home -- if anything there */
		if (st_ptr->stock_num)
		{
#ifdef JP
			int x = 1;
#endif
			/* Header with name of the town */
#ifdef JP
			fprintf(fff, "  [ �䂪�Ƃ̃A�C�e�� ]\n");
#else
			fprintf(fff, "  [Home Inventory]\n");
#endif

			/* Dump all available items */
			for (i = 0; i < st_ptr->stock_num; i++)
			{
#ifdef JP
				if ((i % 12) == 0) fprintf(fff, "\n ( %d �y�[�W )\n", x++);
				object_desc(o_name, &st_ptr->stock[i], 0);
				if (strlen(o_name) <= 80-3)
				{
					fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
				}
				else
				{
					int n;
					char *t;
					for (n = 0, t = o_name; n < 80-3; n++, t++)
						if(iskanji(*t)) {t++; n++;}
					if (n == 81-3) n = 79-3; /* �Ōオ�������� */

					fprintf(fff, "%c%s %.*s\n", I2A(i%12), paren, n, o_name);
					fprintf(fff, "   %.77s\n", o_name+n);
				}
#else
				object_desc(o_name, &st_ptr->stock[i], 0);
				fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
#endif

			}

			/* Add an empty line */
			fprintf(fff, "\n\n");
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�䂪�Ƃ̃A�C�e��", 0, 0);
#else
	show_file(TRUE, file_name, "Home Inventory", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Check the status of "autopick"
 */
static void do_cmd_knowledge_autopick(void)
{
	int k;
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	if (!fff)
	{
#ifdef JP
	    msg_format("�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	if (!max_autopick)
	{
#ifdef JP
	    fprintf(fff, "�����j��/�E���ɂ͉����o�^����Ă��܂���B");
#else
	    fprintf(fff, "No preference for auto picker/destroyer.");
#endif
	}
	else
	{
#ifdef JP
	    fprintf(fff, "   �����E��/�j��ɂ͌��� %d�s�o�^����Ă��܂��B\n\n", max_autopick);
#else
	    fprintf(fff, "   There are %d registered lines for auto picker/destroyer.\n\n", max_autopick);
#endif
	}

	for (k = 0; k < max_autopick; k++)
	{
		cptr tmp;
		byte act = autopick_list[k].action;
		if (act & DONT_AUTOPICK)
		{
#ifdef JP
			tmp = "���u";
#else
			tmp = "Leave";
#endif
		}
		else if (act & DO_AUTODESTROY)
		{
#ifdef JP
			tmp = "�j��";
#else
			tmp = "Destroy";
#endif
		}
		else if (act & DO_AUTOPICK)
		{
#ifdef JP
			tmp = "�E��";
#else
			tmp = "Pickup";
#endif
		}
		else /* if (act & DO_QUERY_AUTOPICK) */ /* Obvious */
		{
#ifdef JP
			tmp = "�m�F";
#else
			tmp = "Query";
#endif
		}

		if (act & DO_DISPLAY)
			fprintf(fff, "%11s", format("[%s]", tmp));
		else
			fprintf(fff, "%11s", format("(%s)", tmp));

		tmp = autopick_line_from_entry(&autopick_list[k]);
		fprintf(fff, " %s", tmp);
		string_free(tmp);
		fprintf(fff, "\n");
	}
	/* Close the file */
	my_fclose(fff);
	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�����E��/�j�� �ݒ胊�X�g", 0, 0);
#else
	show_file(TRUE, file_name, "Auto-picker/Destroyer", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Interact with "knowledge"
 */
/*:::���m�̃A�C�e���⃆�j�[�N�Ȃǂ̌��ݏ�Ԃ�\��*/
///sys item mon ~�R�}���h
void do_cmd_knowledge(void)
{
	int i, p = 0;
	bool need_redraw = FALSE;

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Save the screen */
	screen_save();

	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt(format("%d/2 �y�[�W", (p+1)), 2, 65);
		prt("���݂̒m�����m�F����", 3, 0);
#else
		prt(format("page %d/2", (p+1)), 2, 65);
		prt("Display current knowledge", 3, 0);
#endif

		/* Give some choices */
#ifdef JP
		if (p == 0)
		{
			prt("(1) ���m�̓`���̃A�C�e��                 �̈ꗗ", 6, 5);
			prt("(2) ���m�̃A�C�e��                       �̈ꗗ", 7, 5);
			prt("(3) ���m�̖��œ|���j�[�N�E�����X�^�[     �̈ꗗ", 8, 5);
			prt("(4) ���m�̃����X�^�[                     �̈ꗗ", 9, 5);
			prt("(5) �|�����G�̐�                         �̈ꗗ", 10, 5);
			///del131221 �܋���ꗗ
			//if (!vanilla_town) prt("(6) �܋���                               �̈ꗗ", 11, 5);
			//v1.1.55 ���Y�^�R�}���h�ǉ�
			prt("(6) ���݂̎{�ݗ��p��                   �̈ꗗ", 11, 5);
			prt("(7) ���݂̔z��                           �̈ꗗ", 12, 5);
			prt("(8) �䂪�Ƃ̃A�C�e��                     �̈ꗗ", 13, 5);
			prt("(9) *�Ӓ�*�ςݑ����̑ϐ�                 �̈ꗗ", 14, 5);
			prt("(0) �n�`�̕\������/�^�C��                �̈ꗗ", 15, 5);
		}
		else
		{
			prt("(a) �����Ɋւ�����                     �̈ꗗ", 6, 5);
			prt("(b) �g�̓����E�ˑR�ψ�                   �̈ꗗ", 7, 5);
			//prt("(c) ����̌o���l                         �̈ꗗ", 8, 5);
			prt("(d) ���@�̌o���l                         �̈ꗗ", 9, 5);
			prt("(e) �Z�\�̌o���l                         �̈ꗗ", 10, 5);
			//prt("(f) �v���C���[�̓�                       �̈ꗗ", 11, 5);
			prt("(g) �������_���W����                     �̈ꗗ", 12, 5);
			prt("(h) ���s���̃N�G�X�g                     �̈ꗗ", 13, 5);
			prt("(i) ���݂̎����E��/�j��ݒ�              �̈ꗗ", 14, 5);
		}
#else
		if (p == 0)
		{
			prt("(1) Display known artifacts", 6, 5);
			prt("(2) Display known objects", 7, 5);
			prt("(3) Display remaining uniques", 8, 5);
			prt("(4) Display known monster", 9, 5);
			prt("(5) Display kill count", 10, 5);
			if (!vanilla_town) prt("(6) Display wanted monsters", 11, 5);
			prt("(7) Display current pets", 12, 5);
			prt("(8) Display home inventory", 13, 5);
			prt("(9) Display *identified* equip.", 14, 5);
			prt("(0) Display terrain symbols.", 15, 5);
		}
		else
		{
			prt("(a) Display about yourself", 6, 5);
			prt("(b) Display mutations", 7, 5);
			prt("(c) Display weapon proficiency", 8, 5);
			prt("(d) Display spell proficiency", 9, 5);
			prt("(e) Display misc. proficiency", 10, 5);
			prt("(f) Display virtues", 11, 5);
			prt("(g) Display dungeons", 12, 5);
			prt("(h) Display current quests", 13, 5);
			prt("(i) Display auto pick/destroy", 14, 5);
		}
#endif
		/* Prompt */
#ifdef JP
		prt("-����-", 17, 8);
		prt("ESC) ������", 21, 1);
		prt("SPACE) ���y�[�W", 21, 30);
		/*prt("-) �O�y�[�W", 21, 60);*/
		prt("�R�}���h:", 20, 0);
#else
		prt("-more-", 17, 8);
		prt("ESC) Exit menu", 21, 1);
		prt("SPACE) Next page", 21, 30);
		/*prt("-) Previous page", 21, 60);*/
		prt("Command: ", 20, 0);
#endif

		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;
		switch (i)
		{
		case ' ': /* Page change */
		case '-':
			p = 1 - p;
			break;
		case '1': /* Artifacts */
			do_cmd_knowledge_artifacts();
			break;
		case '2': /* Objects */
			do_cmd_knowledge_objects(&need_redraw, FALSE, -1);
			break;
		case '3': /* Uniques */
			do_cmd_knowledge_uniques();
			break;
		case '4': /* Monsters */
			do_cmd_knowledge_monsters(&need_redraw, FALSE, -1);
			break;
		case '5': /* Kill count  */
			do_cmd_knowledge_kill_count();
			break;
		case '6': /* wanted */
			///del131221 �܋���ꗗ
			//if (!vanilla_town) do_cmd_knowledge_kubi();
			//v1.1.55
			do_cmd_knowledge_buildings();
			break;
		case '7': /* Pets */
			do_cmd_knowledge_pets();
			break;
		case '8': /* Home */
			do_cmd_knowledge_home();
			break;
		case '9': /* Resist list */
			do_cmd_knowledge_inven();
			break;
		case '0': /* Feature list */
			{
				int lighting_level = F_LIT_STANDARD;
				do_cmd_knowledge_features(&need_redraw, FALSE, -1, &lighting_level);
			}
			break;
		/* Next page */
		case 'a': /* Max stat */
			do_cmd_knowledge_stat();
			break;
		case 'b': /* Mutations */
			do_cmd_knowledge_mutations();
			break;
			//����Z�\�͕ʂ̏��ŕ\��
		//case 'c': /* weapon-exp */
			//do_cmd_knowledge_weapon_exp();
			break;
		case 'd': /* spell-exp */
			do_cmd_knowledge_spell_exp();
			break;
		case 'e': /* skill-exp */
			do_cmd_knowledge_skill_exp();
			break;
		//case 'f': /* Virtues */
		//	do_cmd_knowledge_virtues();
		//	break;
		case 'g': /* Dungeon */
			do_cmd_knowledge_dungeon();
			break;
		case 'h': /* Quests */
			do_cmd_knowledge_quests();
			break;
		case 'i': /* Autopick */
			do_cmd_knowledge_autopick();
			break;
		default: /* Unknown option */
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	screen_load();

	if (need_redraw) do_cmd_redraw();
}


/*
 * Check on the status of an active quest
 */
/*:::^Q�R�}���h�@�N�G�X�g�i�s�󋵕\��*/
void do_cmd_checkquest(void)
{
	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Save the screen */
	screen_save();

	/* Quest info */
	do_cmd_knowledge_quests();

	/* Restore the screen */
	screen_load();
}


/*
 * Display the time and date
 */
/*:::������\��*/
void do_cmd_time(void)
{
	int day, hour, min, full, start, end, num;
	char desc[1024];

	char buf[1024];
	char day_buf[10];

	FILE *fff;

	extract_day_hour_min(&day, &hour, &min);

	full = hour * 100 + min;

	start = 9999;
	end = -9999;

	num = 0;

#ifdef JP
	strcpy(desc, "�ςȎ������B");
#else
	strcpy(desc, "It is a strange time.");
#endif


	if (day < MAX_DAYS) sprintf(day_buf, "%d", day);
	else strcpy(day_buf, "*****");

	/* Message */
#ifdef JP
	msg_format("%s����, ������%d:%02d %s�ł��B",
		   day_buf, (hour % 12 == 0) ? 12 : (hour % 12),
		   min, (hour < 12) ? "AM" : "PM");
#else
	msg_format("This is day %s. The time is %d:%02d %s.",
		   day_buf, (hour % 12 == 0) ? 12 : (hour % 12),
		   min, (hour < 12) ? "AM" : "PM");
#endif

	//v1.1.18 timenorm��timefun������������
#if 0
	/* Find the path */
	if (!randint0(10) || p_ptr->image)
	{
#ifdef JP
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "timefun_j.txt");
#else
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "timefun.txt");
#endif

	}
	else
	{
#ifdef JP
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "timenorm_j.txt");
#else
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "timenorm.txt");
#endif

	}
#endif
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "timefun_gen.txt");

	/* Open this file */
	fff = my_fopen(buf, "rt");

	/* Oops */
	if (!fff) return;

	/* Find this time */
	while (!my_fgets(fff, buf, sizeof(buf)))
	{
		/* Ignore comments */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Ignore invalid lines */
		if (buf[1] != ':') continue;

		/* Process 'Start' */
		if (buf[0] == 'S')
		{
			/* Extract the starting time */
			start = atoi(buf + 2);

			/* Assume valid for an hour */
			end = start + 59;

			/* Next... */
			continue;
		}

		/* Process 'End' */
		if (buf[0] == 'E')
		{
			/* Extract the ending time */
			end = atoi(buf + 2);

			/* Next... */
			continue;
		}

		/* Ignore incorrect range */
		if ((start > full) || (full > end)) continue;

		/* Process 'Description' */
		if (buf[0] == 'D')
		{
			num++;

			/* Apply the randomizer */
			if (!randint0(num)) strcpy(desc, buf + 2);

			/* Next... */
			continue;
		}
	}

	/* Message */
	msg_print(desc);

	/* Close the file */
	my_fclose(fff);
}
