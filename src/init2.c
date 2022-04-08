/* File: init2.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Initialization (part 2) -BEN- */

#include "angband.h"

#include "init.h"

#ifndef MACINTOSH
#ifdef CHECK_MODIFICATION_TIME
#include <sys/types.h>
#include <sys/stat.h>
#endif /* CHECK_MODIFICATION_TIME */
#endif

/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * The "init1.c" file is used only to parse the ascii template files,
 * to create the binary image files.  If you include the binary image
 * files instead of the ascii template files, then you can undefine
 * "ALLOW_TEMPLATES", saving about 20K by removing "init1.c".  Note
 * that the binary image files are extremely system dependant.
 */



/*
 * Find the default paths to all of our important sub-directories.
 *
 * The purpose of each sub-directory is described in "variable.c".
 *
 * All of the sub-directories should, by default, be located inside
 * the main "lib" directory, whose location is very system dependant.
 *
 * This function takes a writable buffer, initially containing the
 * "path" to the "lib" directory, for example, "/pkg/lib/angband/",
 * or a system dependant string, for example, ":lib:".  The buffer
 * must be large enough to contain at least 32 more characters.
 *
 * Various command line options may allow some of the important
 * directories to be changed to user-specified directories, most
 * importantly, the "info" and "user" and "save" directories,
 * but this is done after this function, see "main.c".
 *
 * In general, the initial path should end in the appropriate "PATH_SEP"
 * string.  All of the "sub-directory" paths (created below or supplied
 * by the user) will NOT end in the "PATH_SEP" string, see the special
 * "path_build()" function in "util.c" for more information.
 *
 * Mega-Hack -- support fat raw files under NEXTSTEP, using special
 * "suffixed" directories for the "ANGBAND_DIR_DATA" directory, but
 * requiring the directories to be created by hand by the user.
 *
 * Hack -- first we free all the strings, since this is known
 * to succeed even if the strings have not been allocated yet,
 * as long as the variables start out as "NULL".  This allows
 * this function to be called multiple times, for example, to
 * try several base "path" values until a good one is found.
 */
void init_file_paths(char *path)
{
	char *tail;

#ifdef PRIVATE_USER_PATH
	char buf[1024];
#endif /* PRIVATE_USER_PATH */

	/*** Free everything ***/

	/* Free the main path */
	string_free(ANGBAND_DIR);

	/* Free the sub-paths */
	string_free(ANGBAND_DIR_APEX);
	string_free(ANGBAND_DIR_BONE);
	string_free(ANGBAND_DIR_DATA);
	string_free(ANGBAND_DIR_EDIT);
	string_free(ANGBAND_DIR_SCRIPT);
	string_free(ANGBAND_DIR_FILE);
	string_free(ANGBAND_DIR_HELP);
	string_free(ANGBAND_DIR_INFO);
	string_free(ANGBAND_DIR_SAVE);
	string_free(ANGBAND_DIR_USER);
	string_free(ANGBAND_DIR_XTRA);


	/*** Prepare the "path" ***/

	/* Hack -- save the main directory */
	ANGBAND_DIR = string_make(path);

	/* Prepare to append to the Base Path */
	tail = path + strlen(path);


#ifdef VM

	/*** Use "flat" paths with VM/ESA ***/

	/* Use "blank" path names */
	ANGBAND_DIR_APEX = string_make("");
	ANGBAND_DIR_BONE = string_make("");
	ANGBAND_DIR_DATA = string_make("");
	ANGBAND_DIR_EDIT = string_make("");
	ANGBAND_DIR_SCRIPT = string_make("");
	ANGBAND_DIR_FILE = string_make("");
	ANGBAND_DIR_HELP = string_make("");
	ANGBAND_DIR_INFO = string_make("");
	ANGBAND_DIR_SAVE = string_make("");
	ANGBAND_DIR_USER = string_make("");
	ANGBAND_DIR_XTRA = string_make("");


#else /* VM */


	/*** Build the sub-directory names ***/

	/* Build a path name */
	strcpy(tail, "apex");
	ANGBAND_DIR_APEX = string_make(path);

	/* Build a path name */
	strcpy(tail, "bone");
	ANGBAND_DIR_BONE = string_make(path);

	/* Build a path name */
	strcpy(tail, "data");
	ANGBAND_DIR_DATA = string_make(path);

	/* Build a path name */
	strcpy(tail, "edit");
	ANGBAND_DIR_EDIT = string_make(path);

	/* Build a path name */
	strcpy(tail, "script");
	ANGBAND_DIR_SCRIPT = string_make(path);

	/* Build a path name */
	strcpy(tail, "file");
	ANGBAND_DIR_FILE = string_make(path);

	/* Build a path name */
	strcpy(tail, "help");
	ANGBAND_DIR_HELP = string_make(path);

	/* Build a path name */
	strcpy(tail, "info");
	ANGBAND_DIR_INFO = string_make(path);

	/* Build a path name */
	strcpy(tail, "pref");
	ANGBAND_DIR_PREF = string_make(path);

	/* Build a path name */
	strcpy(tail, "save");
	ANGBAND_DIR_SAVE = string_make(path);

#ifdef PRIVATE_USER_PATH

	/* Build the path to the user specific directory */
	path_build(buf, sizeof(buf), PRIVATE_USER_PATH, VERSION_NAME);

	/* Build a relative path name */
	ANGBAND_DIR_USER = string_make(buf);

#else /* PRIVATE_USER_PATH */

	/* Build a path name */
	strcpy(tail, "user");
	ANGBAND_DIR_USER = string_make(path);

#endif /* PRIVATE_USER_PATH */

	/* Build a path name */
	strcpy(tail, "xtra");
	ANGBAND_DIR_XTRA = string_make(path);

#endif /* VM */


#ifdef NeXT

	/* Allow "fat binary" usage with NeXT */
	if (TRUE)
	{
		cptr next = NULL;

# if defined(m68k)
		next = "m68k";
# endif

# if defined(i386)
		next = "i386";
# endif

# if defined(sparc)
		next = "sparc";
# endif

# if defined(hppa)
		next = "hppa";
# endif

		/* Use special directory */
		if (next)
		{
			/* Forget the old path name */
			string_free(ANGBAND_DIR_DATA);

			/* Build a new path name */
			sprintf(tail, "data-%s", next);
			ANGBAND_DIR_DATA = string_make(path);
		}
	}

#endif /* NeXT */

}



#ifdef ALLOW_TEMPLATES


/*
 * Hack -- help give useful error messages
 */
int error_idx;
int error_line;


/*
 * Standard error message text
 */
cptr err_str[PARSE_ERROR_MAX] =
{
	NULL,
#ifdef JP
	"���@�G���[",
	"�Â��t�@�C��",
	"�L�^�w�b�_���Ȃ�",
	"�s�A�����R�[�h",
	"�������ȃt���O����",
	"����`����",
	"�������s��",
	"���W�͈͊O",
	"�����s��",
	"����`�n�`�^�O",
#else
	"parse error",
	"obsolete file",
	"missing record header",
	"non-sequential records",
	"invalid flag specification",
	"undefined directive",
	"out of memory",
	"coordinates out of bounds",
	"too few arguments",
	"undefined terrain tag",
#endif

};


#endif /* ALLOW_TEMPLATES */


/*
 * File headers
 */
header v_head;
header f_head;
header k_head;
header a_head;
header e_head;
header r_head;
header d_head;
header s_head;
header m_head;

#ifdef CHECK_MODIFICATION_TIME

/*:::raw�t�@�C����txt�t�@�C�����Â��Ȃ����ǂ����`�F�b�N*/
static errr check_modification_date(int fd, cptr template_file)
{
	char buf[1024];

	struct stat txt_stat, raw_stat;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_EDIT, template_file);

	/* Access stats on text file */
	if (stat(buf, &txt_stat))
	{
		/* No text file - continue */
	}

	/* Access stats on raw file */
	else if (fstat(fd, &raw_stat))
	{
		/* Error */
		return (-1);
	}

	/* Ensure text file is not newer than raw file */
	else if (txt_stat.st_mtime > raw_stat.st_mtime)
	{
		/* Reprocess text file */
		return (-1);
	}

	return (0);
}

#endif /* CHECK_MODIFICATION_TIME */



/*** Initialize from binary image files ***/


/*
 * Initialize the "*_info" array, by parsing a binary "image" file
 */
/*:::raw�t�@�C��(fd�ɃI�[�v���ρj��*_info[]�ɓW�J����*/
static errr init_info_raw(int fd, header *head)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != head->v_major) ||
	    (test.v_minor != head->v_minor) ||
	    (test.v_patch != head->v_patch) ||
	    (test.info_num != head->info_num) ||
	    (test.info_len != head->info_len) ||
	    (test.head_size != head->head_size) ||
	    (test.info_size != head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	(*head) = test;


	/* Allocate the "*_info" array */
	C_MAKE(head->info_ptr, head->info_size, char);

	/* Read the "*_info" array */
	fd_read(fd, head->info_ptr, head->info_size);


	if (head->name_size)
	{
		/* Allocate the "*_name" array */
		C_MAKE(head->name_ptr, head->name_size, char);

		/* Read the "*_name" array */
		fd_read(fd, head->name_ptr, head->name_size);
	}


	if (head->text_size)
	{
		/* Allocate the "*_text" array */
		C_MAKE(head->text_ptr, head->text_size, char);

		/* Read the "*_text" array */
		fd_read(fd, head->text_ptr, head->text_size);
	}


	if (head->tag_size)
	{
		/* Allocate the "*_tag" array */
		C_MAKE(head->tag_ptr, head->tag_size, char);

		/* Read the "*_tag" array */
		fd_read(fd, head->tag_ptr, head->tag_size);
	}


	/* Success */
	return (0);
}



/*
 * Initialize the header of an *_info.raw file.
 */
/*:::raw�t�@�C���̃w�b�_�������*/
static void init_header(header *head, int num, int len)
{
	/* Save the "version" */
	head->v_major = H_VER_MAJOR;
	head->v_minor = H_VER_MINOR;
	head->v_patch = H_VER_PATCH;
	head->v_extra = 0;

	/* Save the "record" information */
	/*:::�i�[����f�[�^�A���C�̐��ƌ^�T�C�Y*/
	head->info_num = num;
	head->info_len = len;

	/* Save the size of "*_head" and "*_info" */
	head->head_size = sizeof(header);
	head->info_size = head->info_num * head->info_len;
}


/*
 * Initialize the "*_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
/*:::�w�肳�ꂽinfo�t�@�C����raw�t�@�C����ǂݍ���??_info[ ]�ɓW�J����
 *:::raw�t�@�C�����Ȃ������݃o�[�W�������Â����?_info.txt��ǂݍ��݉�͂���raw�t�@�C������*/
static errr init_info(cptr filename, header *head,
		      void **info, char **name, char **text, char **tag)
{
	int fd;

	int mode = 0644;

	errr err = 1;

	FILE *fp;

	/* General buffer */
	char buf[1024];


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
#ifdef JP
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format("%s_j.raw", filename));
#else
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format("%s.raw", filename));
#endif


	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, format("%s.txt", filename));

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_info_raw(fd, head);

		/* Close it */
		(void)fd_close(fd);
	}


	/* Do we have to parse the *.txt file? */
	if (err)
	{
		/*** Make the fake arrays ***/

		/* Allocate the "*_info" array */
		C_MAKE(head->info_ptr, head->info_size, char);

		/* Hack -- make "fake" arrays */
		if (name) C_MAKE(head->name_ptr, FAKE_NAME_SIZE, char);
		if (text) C_MAKE(head->text_ptr, FAKE_TEXT_SIZE, char);
		if (tag)  C_MAKE(head->tag_ptr, FAKE_TAG_SIZE, char);

		if (info) (*info) = head->info_ptr;
		if (name) (*name) = head->name_ptr;
		if (text) (*text) = head->text_ptr;
		if (tag)  (*tag)  = head->tag_ptr;

		/*** Load the ascii template file ***/

		/* Build the filename */

		path_build(buf, sizeof(buf), ANGBAND_DIR_EDIT, format("%s.txt", filename));

		/* Open the file */
		fp = my_fopen(buf, "r");

		/* Parse it */
#ifdef JP
		if (!fp) quit(format("'%s.txt'�t�@�C�����I�[�v���ł��܂���B", filename));
#else
		if (!fp) quit(format("Cannot open '%s.txt' file.", filename));
#endif


		/* Parse the file */
		err = init_info_txt(fp, buf, head, head->parse_info_txt);

		/* Close it */
		my_fclose(fp);

		/* Errors */
		if (err)
		{
			cptr oops;

#ifdef JP
			/* Error string */
			oops = (((err > 0) && (err < PARSE_ERROR_MAX)) ? err_str[err] : "���m��");

			/* Oops */
			msg_format("'%s.txt'�t�@�C���� %d �s�ڂɃG���[�B", filename, error_line);
			msg_format("���R�[�h %d �� '%s' �G���[������܂��B", error_idx, oops);
			msg_format("�\�� '%s'�B", buf);
			msg_print(NULL);

			/* Quit */
			quit(format("'%s.txt'�t�@�C���ɃG���[", filename));
#else
			/* Error string */
			oops = (((err > 0) && (err < PARSE_ERROR_MAX)) ? err_str[err] : "unknown");

			/* Oops */
			msg_format("Error %d at line %d of '%s.txt'.", err, error_line, filename);
			msg_format("Record %d contains a '%s' error.", error_idx, oops);
			msg_format("Parsing '%s'.", buf);
			msg_print(NULL);

			/* Quit */
			quit(format("Error in '%s.txt' file.", filename));
#endif

		}


		/*** Make final retouch on fake tags ***/

		if (head->retouch)
		{
			(*head->retouch)(head);
		}


		/*** Dump the binary image file ***/

		/* File type is "DATA" */
		FILE_TYPE(FILE_TYPE_DATA);

		/* Build the filename */
#ifdef JP
		path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format("%s_j.raw", filename));
#else
		path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format("%s.raw", filename));
#endif


		/* Grab permissions */
		safe_setuid_grab();

		/* Kill the old file */
		(void)fd_kill(buf);

		/* Attempt to create the raw file */
		fd = fd_make(buf, mode);

		/* Drop permissions */
		safe_setuid_drop();

		/* Dump to the file */
		if (fd >= 0)
		{
			/* Dump it */
			fd_write(fd, (cptr)(head), head->head_size);

			/* Dump the "*_info" array */
			fd_write(fd, head->info_ptr, head->info_size);

			/* Dump the "*_name" array */
			fd_write(fd, head->name_ptr, head->name_size);

			/* Dump the "*_text" array */
			fd_write(fd, head->text_ptr, head->text_size);

			/* Dump the "*_tag" array */
			fd_write(fd, head->tag_ptr, head->tag_size);

			/* Close */
			(void)fd_close(fd);
		}


		/*** Kill the fake arrays ***/

		/* Free the "*_info" array */
		C_KILL(head->info_ptr, head->info_size, char);

		/* Hack -- Free the "fake" arrays */
		if (name) C_KILL(head->name_ptr, FAKE_NAME_SIZE, char);
		if (text) C_KILL(head->text_ptr, FAKE_TEXT_SIZE, char);
		if (tag)  C_KILL(head->tag_ptr, FAKE_TAG_SIZE, char);

#endif	/* ALLOW_TEMPLATES */


		/*** Load the binary image file ***/

		/* Build the filename */
#ifdef JP
		path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format("%s_j.raw", filename));
#else
		path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format("%s.raw", filename));
#endif


		/* Attempt to open the "raw" file */
		fd = fd_open(buf, O_RDONLY);

		/* Process existing "raw" file */
#ifdef JP
		if (fd < 0) quit(format("'%s_j.raw'�t�@�C�������[�h�ł��܂���B", filename));
#else
		if (fd < 0) quit(format("Cannot load '%s.raw' file.", filename));
#endif


		/* Attempt to parse the "raw" file */
		err = init_info_raw(fd, head);

		/* Close it */
		(void)fd_close(fd);

		/* Error */
#ifdef JP
		if (err) quit(format("'%s_j.raw'�t�@�C������͂ł��܂���B", filename));
#else
		if (err) quit(format("Cannot parse '%s.raw' file.", filename));
#endif

#ifdef ALLOW_TEMPLATES
	}
#endif

	if (info) (*info) = head->info_ptr;
	if (name) (*name) = head->name_ptr;
	if (text) (*text) = head->text_ptr;
	if (tag)  (*tag)  = head->tag_ptr;

	/* Success */
	return (0);
}

/*:::f_info[]������*/
/*
 * Initialize the "f_info" array
 */
static errr init_f_info(void)
{
	/* Init the header */
	init_header(&f_head, max_f_idx, sizeof(feature_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	f_head.parse_info_txt = parse_f_info;

	/* Save a pointer to the retouch fake tags */
	f_head.retouch = retouch_f_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("f_info", &f_head,
			 (void*)&f_info, &f_name, NULL, &f_tag);
}


/*
 * Initialize the "k_info" array
 */
static errr init_k_info(void)
{
	/* Init the header */
	init_header(&k_head, max_k_idx, sizeof(object_kind));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	k_head.parse_info_txt = parse_k_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("k_info", &k_head,
			 (void*)&k_info, &k_name, &k_text, NULL);
}



/*
 * Initialize the "a_info" array
 */
static errr init_a_info(void)
{
	/* Init the header */
	init_header(&a_head, max_a_idx, sizeof(artifact_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	a_head.parse_info_txt = parse_a_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("a_info", &a_head,
			 (void*)&a_info, &a_name, &a_text, NULL);
}



/*
 * Initialize the "e_info" array
 */
static errr init_e_info(void)
{
	/* Init the header */
	init_header(&e_head, max_e_idx, sizeof(ego_item_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	e_head.parse_info_txt = parse_e_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("e_info", &e_head,
			 (void*)&e_info, &e_name, &e_text, NULL);
}



/*
 * Initialize the "r_info" array
 */
/*:::r_info[]�̏�����*/
static errr init_r_info(void)
{
	/* Init the header */
	init_header(&r_head, max_r_idx, sizeof(monster_race));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	r_head.parse_info_txt = parse_r_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("r_info", &r_head,
			 (void*)&r_info, &r_name, &r_text, NULL);
}



/*
 * Initialize the "d_info" array
 */
static errr init_d_info(void)
{
	/* Init the header */
	init_header(&d_head, max_d_idx, sizeof(dungeon_info_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	d_head.parse_info_txt = parse_d_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("d_info", &d_head,
			 (void*)&d_info, &d_name, &d_text, NULL);
}


/*
 * Initialize the "v_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
errr init_v_info(void)
{
	/* Init the header */
	init_header(&v_head, max_v_idx, sizeof(vault_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	v_head.parse_info_txt = parse_v_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("v_info", &v_head,
			 (void*)&v_info, &v_name, &v_text, NULL);
}


/*
 * Initialize the "s_info" array
 */
#if 0
static errr init_s_info(void)
{
	/* Init the header */
	init_header(&s_head, MAX_CLASS, sizeof(skill_table));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	s_head.parse_info_txt = parse_s_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("s_info", &s_head,
			 (void*)&s_info, NULL, NULL, NULL);
}
#endif

/*
 * Initialize the "m_info" array
 */
///del140208 ���@�d�l�ύX�ō폜
#if 0
static errr init_m_info(void)
{
	/* Init the header */
	init_header(&m_head, MAX_CLASS, sizeof(player_magic));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	m_head.parse_info_txt = parse_m_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("m_info", &m_head,
			 (void*)&m_info, NULL, NULL, NULL);
}
#endif


/*** Initialize others ***/

/*
 * Hack -- Objects sold in the stores -- by tval/sval pair.
 */
///item store BM�ȊO�̓X�ɕ��Ԋ�{�A�C�e���@�����̃G���g���������_���ɌĂ΂��̂Ń����_���͈̔͂��H�v����Β����Ƃɕi������ς�����B

///mod131223
static byte store_table[MAX_STORES][STORE_CHOICES][2] =
{
	{
		/* General Store */

		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },

		{ TV_FOOD, SV_FOOD_HARDBUSC },
		{ TV_FOOD, SV_FOOD_HARDBUSC },
		{ TV_FOOD, SV_FOOD_VENISON },
		{ TV_FOOD, SV_FOOD_VENISON },

		{ TV_SOFTDRINK, SV_DRINK_WATER },//v1.1.78 ����W���[�X�𑝂₵��
		{ TV_SOFTDRINK, SV_DRINK_WATER },
		{ TV_SOFTDRINK, SV_DRINK_WATER },
		{ TV_SOFTDRINK, SV_DRINK_WATER },

		{ TV_FOOD, SV_FOOD_SENTAN },
		{ TV_FOOD, SV_FOOD_SENTAN },
		{ TV_LITE, SV_LITE_LANTERN },
		{ TV_LITE, SV_LITE_LANTERN },

		{ TV_SOFTDRINK, SV_DRINK_WATER },
		{ TV_SOFTDRINK, SV_DRINK_WATER },
		{ TV_SOFTDRINK, SV_DRINK_APPLE_JUICE },
		{ TV_SOFTDRINK, SV_DRINK_APPLE_JUICE },

		{ TV_SOFTDRINK, SV_DRINK_APPLE_JUICE },
		{ TV_SOFTDRINK, SV_DRINK_APPLE_JUICE },
		{ TV_SOFTDRINK, SV_DRINK_APPLE_JUICE },
		{ TV_SOFTDRINK, SV_DRINK_APPLE_JUICE },

		{ TV_SWEETS, SV_SWEET_COOKIE },
		{ TV_CLOAK, SV_CLOAK_FUR },
		{ TV_CLOAK, SV_CLOAK },
		{ TV_CLOAK, SV_CLOAK },

		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_LANTERN },

		{ TV_LITE, SV_LITE_LANTERN },
		{ TV_LITE, SV_LITE_LANTERN },
		{ TV_FLASK, SV_FLASK_OIL },
		{ TV_FLASK, SV_FLASK_OIL },

		{ TV_FLASK, SV_FLASK_OIL },
		{ TV_OTHERWEAPON, SV_OTHERWEAPON_FISHING },
		{ TV_OTHERWEAPON, SV_OTHERWEAPON_FISHING },
		{ TV_AXE, SV_WEAPON_PICK },

		{ TV_AXE, SV_WEAPON_PICK },
		{ TV_BOLT, SV_AMMO_LIGHT },
		{ TV_BOLT, SV_AMMO_LIGHT },
		{ TV_ARROW, SV_AMMO_LIGHT },

		{ TV_ARROW, SV_AMMO_LIGHT },
		{ TV_ARMOR, SV_ARMOR_TUB },
		{ TV_ARMOR, SV_ARMOR_TUB },
		{ TV_ARMOR, SV_ARMOR_TUB }
	},

	{
		/* Armoury */

		{ TV_BOOTS, SV_LEG_SHOES },
		{ TV_BOOTS, SV_LEG_WARAZI },
		{ TV_BOOTS, SV_LEG_GETA },
		{ TV_BOOTS, SV_LEG_BOOTS },

		{ TV_BOOTS, SV_LEG_LEATHERBOOTS },
		{ TV_GLOVES, SV_HAND_GLOVES },
		{ TV_GLOVES, SV_HAND_GLOVES },
		{ TV_GLOVES, SV_HAND_LEATHERGLOVES },

		{ TV_GLOVES, SV_HAND_LEATHERGLOVES },
		{ TV_GLOVES, SV_HAND_ARMGUARDS },
		{ TV_HEAD, SV_HEAD_HAT },
		{ TV_HEAD, SV_HEAD_HAT },

		{ TV_HEAD, SV_HEAD_LEATHER },
		{ TV_HEAD, SV_HEAD_STARHELM },
		{ TV_HEAD, SV_HEAD_METALCAP },
		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD },

		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD },
		{ TV_SHIELD, SV_LARGE_LEATHER_SHIELD },
		{ TV_SHIELD, SV_LARGE_LEATHER_SHIELD },
		{ TV_SHIELD, SV_SMALL_METAL_SHIELD },

		{ TV_CLOTHES, SV_CLOTHES },
		{ TV_CLOTHES, SV_CLOTHES },
		{ TV_CLOTHES, SV_CLOTH_ROBE },
		{ TV_CLOTHES, SV_CLOTH_ROBE },

		{ TV_CLOTHES, SV_CLOTH_ROBE },
		{ TV_CLOTHES, SV_CLOTH_MAID },
		{ TV_CLOTHES, SV_CLOTH_DOUGI },
		{ TV_CLOTHES, SV_CLOTH_DOUGI },

		{ TV_CLOTHES, SV_CLOTH_EASTERN },
		{ TV_CLOTHES, SV_CLOTH_EASTERN },
		{ TV_CLOTHES, SV_CLOTH_DOUSHI },
		{ TV_CLOTHES, SV_CLOTH_SYUGEN },

		{ TV_CLOTHES, SV_CLOTH_SUIT },
		{ TV_CLOTHES, SV_CLOTH_PADDED },
		{ TV_CLOTHES, SV_CLOTH_PADDED },
		{ TV_CLOTHES, SV_CLOTH_LEATHER },

		{ TV_ARMOR, SV_ARMOR_LEATHER },
		{ TV_ARMOR, SV_ARMOR_LEATHER },
		{ TV_ARMOR, SV_ARMOR_LEATHER },
		{ TV_ARMOR, SV_ARMOR_SCALE },

		{ TV_ARMOR, SV_ARMOR_SCALE },
		{ TV_ARMOR, SV_ARMOR_CHAIN },
		{ TV_ARMOR, SV_ARMOR_CHAIN },
		{ TV_ARMOR, SV_ARMOR_BRIGANDINE },

		{ TV_ARMOR, SV_ARMOR_BRIGANDINE },
		{ TV_ARMOR, SV_ARMOR_PARTIAL_PLATE },
		{ TV_ARMOR, SV_ARMOR_PARTIAL_PLATE },
		{ TV_ARMOR, SV_ARMOR_OOYOROI }
	},

	{
		/* Weaponsmith */

		{ TV_KNIFE, SV_WEAPON_KNIFE },
		{ TV_KNIFE, SV_WEAPON_DAGGER },
		{ TV_KNIFE, SV_WEAPON_DAGGER },
		{ TV_KNIFE, SV_WEAPON_TANTO },

		{ TV_KNIFE, SV_WEAPON_YAMAGATANA },
		{ TV_SWORD, SV_WEAPON_RAPIER },
		{ TV_SWORD, SV_WEAPON_SHORT_SWORD },
		{ TV_SWORD, SV_WEAPON_SABRE },

		{ TV_SWORD, SV_WEAPON_SCIMITAR },
		{ TV_SWORD, SV_WEAPON_LONG_SWORD },
		{ TV_SWORD, SV_WEAPON_LONG_SWORD },
		{ TV_SWORD, SV_WEAPON_BASTARD_SWORD },

		{ TV_KATANA, SV_WEAPON_SHORT_KATANA },
		{ TV_KATANA, SV_WEAPON_SHORT_KATANA },
		{ TV_KATANA, SV_WEAPON_KATANA },
		{ TV_KATANA, SV_WEAPON_KATANA },

		{ TV_HAMMER, SV_WEAPON_MACE },
		{ TV_HAMMER, SV_WEAPON_MACE },
		{ TV_HAMMER, SV_WEAPON_MORNING_STAR },
		{ TV_HAMMER, SV_WEAPON_FLAIL },

		{ TV_STICK, SV_WEAPON_QUARTERSTAFF },
		{ TV_STICK, SV_WEAPON_QUARTERSTAFF },
		{ TV_STICK, SV_WEAPON_SIXFEET },
		{ TV_STICK, SV_WEAPON_SYAKUJYOU },

		{ TV_STICK, SV_WEAPON_NUNCHAKU },
		{ TV_AXE, SV_WEAPON_FARANCESCA },
		{ TV_AXE, SV_WEAPON_HATCHET },
		{ TV_AXE, SV_WEAPON_BROAD_AXE },

		{ TV_AXE, SV_WEAPON_BROAD_AXE },
		{ TV_AXE, SV_WEAPON_BATTLE_AXE },
		{ TV_SPEAR, SV_WEAPON_JAVELIN },
		{ TV_SPEAR, SV_WEAPON_SPEAR },

		{ TV_SPEAR, SV_WEAPON_SPEAR },
		{ TV_SPEAR, SV_WEAPON_AWL_PIKE },
		{ TV_SPEAR, SV_WEAPON_LONG_SPEAR },
		{ TV_POLEARM, SV_WEAPON_GLAIVE },

		{ TV_POLEARM, SV_WEAPON_GLAIVE },
		{ TV_POLEARM, SV_WEAPON_NAGINATA },
		{ TV_POLEARM, SV_WEAPON_NAGINATA },
		{ TV_POLEARM, SV_WEAPON_LUCERNE_HAMMER },

		{ TV_BOW, SV_AMMO_NORMAL },
		{ TV_CROSSBOW, SV_AMMO_NORMAL },
		{ TV_ARROW, SV_AMMO_LIGHT },
		{ TV_BOLT, SV_AMMO_LIGHT },

		{ TV_BOOK_HISSATSU, 0 },
		{ TV_BOOK_HISSATSU, 0 },
		{ TV_BOOK_HISSATSU, 1 },
		{ TV_BOOK_HISSATSU, 1 },
	},

	{
		/* Temple */
		/*:::��E��X*/
		{ TV_POTION, SV_POTION_CURE_LIGHT },
		{ TV_POTION, SV_POTION_CURE_LIGHT },
		{ TV_POTION, SV_POTION_CURE_LIGHT },
		{ TV_POTION, SV_POTION_CURE_LIGHT },

		{ TV_POTION, SV_POTION_CURE_SERIOUS },
		{ TV_POTION, SV_POTION_CURE_SERIOUS },
		{ TV_POTION, SV_POTION_CURE_SERIOUS },
		{ TV_POTION, SV_POTION_CURE_SERIOUS },

		{ TV_POTION, SV_POTION_CURE_CRITICAL },
		{ TV_POTION, SV_POTION_CURE_CRITICAL },
		{ TV_POTION, SV_POTION_CURE_CRITICAL },
		{ TV_POTION, SV_POTION_CURE_CRITICAL },

		{ TV_POTION, SV_POTION_CURING },
		{ TV_POTION, SV_POTION_CURING },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_POTION, SV_POTION_RESTORE_EXP },

		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_INT },
		{ TV_POTION, SV_POTION_RES_INT },

		{ TV_POTION, SV_POTION_RES_WIS },
		{ TV_POTION, SV_POTION_RES_WIS },
		{ TV_POTION, SV_POTION_RES_DEX },
		{ TV_POTION, SV_POTION_RES_DEX },

		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CHR },
		{ TV_POTION, SV_POTION_RES_CHR },

		{ TV_POTION, SV_POTION_BOLDNESS },
		{ TV_POTION, SV_POTION_BOLDNESS },
		{ TV_POTION, SV_POTION_HEROISM },
		{ TV_POTION, SV_POTION_HEROISM },

		{ TV_POTION, SV_POTION_CURE_POISON },
		{ TV_POTION, SV_POTION_CURE_POISON },
		{ TV_POTION, SV_POTION_INFRAVISION },
		{ TV_POTION, SV_POTION_INFRAVISION },

		{ TV_ALCOHOL, SV_ALCOHOL_BEER },
		{ TV_ALCOHOL, SV_ALCOHOL_BEER },
		{ TV_ALCOHOL, SV_ALCOHOL_BEER },
		{ TV_ALCOHOL, SV_ALCOHOL_BEER },

		{ TV_ALCOHOL, SV_ALCOHOL_GINZYOU },
		{ TV_ALCOHOL, SV_ALCOHOL_GINZYOU },
		{ TV_ALCOHOL, SV_ALCOHOL_GINZYOU },
		{ TV_ALCOHOL, SV_ALCOHOL_GINZYOU },
		///del131221		{ TV_STATUE, SV_ANY },

		{ TV_BOOK_MEDICINE, 0 },
		{ TV_BOOK_MEDICINE, 1 },
		{ TV_ALCOHOL, SV_ALCOHOL_ONIKOROSHI },
		{ TV_ALCOHOL, SV_ALCOHOL_ONIKOROSHI }
	},

	{
		/* Alchemy shop */
		/*:::�@�B�X�ɕύX�\��*/
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_HIT },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_DAM },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },

		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_LIGHT },

		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_SCROLL, SV_SCROLL_MONSTER_CONFUSION },

		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_SCROLL, SV_SCROLL_DETECT_GOLD },
		{ TV_SCROLL, SV_SCROLL_DETECT_ITEM },
		{ TV_SCROLL, SV_SCROLL_DETECT_TRAP },

		{ TV_SCROLL, SV_SCROLL_DETECT_INVIS },
		{ TV_SCROLL, SV_SCROLL_RECHARGING },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },

		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },

		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_INT },
		{ TV_POTION, SV_POTION_RES_WIS },

		{ TV_POTION, SV_POTION_RES_DEX },
		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CHR },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },

		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_STAR_IDENTIFY },  /* Yep, occasionally! */
		{ TV_SCROLL, SV_SCROLL_STAR_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_LIGHT },

		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_INT },
		{ TV_POTION, SV_POTION_RES_WIS },
		{ TV_POTION, SV_POTION_RES_DEX },

		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CHR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },

		{ TV_SCROLL, SV_SCROLL_RECHARGING },
		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_HIT },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_DAM },

	},

	{
		/* Magic-User store */

		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_LIGHT },
		{ TV_SCROLL, SV_SCROLL_LIGHT },

		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },

		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_SCROLL, SV_SCROLL_DETECT_GOLD },
		{ TV_SCROLL, SV_SCROLL_DETECT_ITEM },
		{ TV_SCROLL, SV_SCROLL_DETECT_TRAP },

		{ TV_SCROLL, SV_SCROLL_RECHARGING },
		{ TV_SCROLL, SV_SCROLL_MONSTER_CONFUSION },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },

		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_REMOVE_CURSE },
		{ TV_STAFF, SV_STAFF_REMOVE_CURSE },

		{ TV_STAFF, SV_STAFF_DETECT_DOOR },
		{ TV_STAFF, SV_STAFF_DETECT_DOOR },
		{ TV_STAFF, SV_STAFF_DETECT_ITEM },
		{ TV_STAFF, SV_STAFF_DETECT_ITEM },

		{ TV_STAFF, SV_STAFF_MAPPING },
		{ TV_STAFF, SV_STAFF_MAPPING },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },

		{ TV_STAFF, SV_STAFF_PROBING },
		{ TV_STAFF, SV_STAFF_PROBING },
		{ TV_STAFF, SV_STAFF_SLEEP_MONSTERS },
		{ TV_STAFF, SV_STAFF_SLEEP_MONSTERS },

		{ TV_STAFF, SV_STAFF_SLOW_MONSTERS },
		{ TV_STAFF, SV_STAFF_SLOW_MONSTERS },
		{ TV_STAFF, SV_STAFF_LITE },
		{ TV_STAFF, SV_STAFF_LITE },

		{ TV_WAND, SV_WAND_FEAR_MONSTER },
		{ TV_WAND, SV_WAND_FEAR_MONSTER },
		{ TV_WAND, SV_WAND_SLEEP_MONSTER },
		{ TV_WAND, SV_WAND_SLEEP_MONSTER },

		{ TV_WAND, SV_WAND_TRAP_DOOR_DEST },
		{ TV_WAND, SV_WAND_TRAP_DOOR_DEST },
		{ TV_FIGURINE, 0 },
		{ TV_FIGURINE, 0 },

		{ TV_RING, SV_RING_RESIST_FIRE },
		{ TV_AMULET, SV_AMULET_RESIST_ELEC },
		{ TV_AMULET, SV_AMULET_RESIST_ACID },
		{ TV_RING, SV_RING_RESIST_COLD },

	},

	{
		/* Black Market (unused) */
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 }
	},

	{
		/* Home (unused) */
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 }
	},

	{
		/* Bookstore */
		{ TV_BOOK_ELEMENT, 0 },
		{ TV_BOOK_ELEMENT, 0 },
		{ TV_BOOK_ELEMENT, 1 },
		{ TV_BOOK_ELEMENT, 1 },

		{ TV_BOOK_FORESEE, 0 },
		{ TV_BOOK_FORESEE, 0 },
		{ TV_BOOK_FORESEE, 1 },
		{ TV_BOOK_FORESEE, 1 },

		{ TV_BOOK_ENCHANT, 0 },
		{ TV_BOOK_ENCHANT, 0 },
		{ TV_BOOK_ENCHANT, 1 },
		{ TV_BOOK_ENCHANT, 1 },

		{ TV_BOOK_SUMMONS, 0 },
		{ TV_BOOK_SUMMONS, 0 },
		{ TV_BOOK_SUMMONS, 1 },
		{ TV_BOOK_SUMMONS, 1 },

		{ TV_BOOK_MYSTIC, 0 },		/* +16 */
		{ TV_BOOK_MYSTIC, 0 },
		{ TV_BOOK_MYSTIC, 1 },
		{ TV_BOOK_MYSTIC, 1 },

		{ TV_BOOK_LIFE, 0 },
		{ TV_BOOK_LIFE, 0 },
		{ TV_BOOK_LIFE, 1 },
		{ TV_BOOK_LIFE, 1 },

		{ TV_BOOK_PUNISH, 0 },
		{ TV_BOOK_PUNISH, 0 },
		{ TV_BOOK_PUNISH, 1 },
		{ TV_BOOK_PUNISH, 1 },

		{ TV_BOOK_NATURE, 0 },
		{ TV_BOOK_NATURE, 0 },
		{ TV_BOOK_NATURE, 1 },
		{ TV_BOOK_NATURE, 1 },

		{ TV_BOOK_TRANSFORM, 0 },
		{ TV_BOOK_TRANSFORM, 1 },
		{ TV_BOOK_DARKNESS, 0 },
		{ TV_BOOK_DARKNESS, 1 },

		{ TV_CAPTURE, 0 },
		{ TV_CAPTURE, 0 },
		{ TV_CAPTURE, 0 },
		{ TV_CAPTURE, 0 },

		{ TV_CAPTURE, 0 },
		{ TV_CAPTURE, 0 },
		{ TV_CAPTURE, 0 },
		{ TV_CAPTURE, 0 },

		{ TV_BOOK_NECROMANCY, 0 },
		{ TV_BOOK_NECROMANCY, 1 },
		{ TV_BOOK_CHAOS, 0 },
		{ TV_BOOK_CHAOS, 1 },
	},

	{
		/* Museum (unused) */
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 }
	}
};


/*
 * Initialize misc. values
 */
static errr init_misc(void)
{
	/* Initialize the values */
	process_dungeon_file("misc.txt", 0, 0, 0, 0);

	return 0;
}


/*
 * Initialize town array
 */
/*:::�X�̏�����*/
static errr init_towns(void)
{
	int i, j, k;

	/*** Prepare the Towns ***/

	/* Allocate the towns */
	C_MAKE(town, max_towns, town_type);

	for (i = 1; i < max_towns; i++)
	{
		/*** Prepare the Stores ***/

		/* Allocate the stores */
		C_MAKE(town[i].store, MAX_STORES, store_type);

		/* Fill in each store */
		/*:::�X�̏�����*/
		for (j = 0; j < MAX_STORES; j++)
		{
			/* Access the store */
			store_type *st_ptr = &town[i].store[j];

			if ((i > 1) && (j == STORE_MUSEUM || j == STORE_HOME)) continue;

			/* Assume full stock */

		/*
		 * �䂪�Ƃ� 20 �y�[�W�܂Ŏg����B���@�\�̂��߂̏����B
		 * �I�v�V�������L���ł������łȂ��Ă��ꉞ�X�y�[�X
		 * ������Ă����B
		 */
		if (j == STORE_HOME)
		{
			st_ptr->stock_size = (STORE_INVEN_MAX * 10);
		}
		else if (j == STORE_MUSEUM)
		{
			st_ptr->stock_size = (STORE_INVEN_MAX * STORE_MUSEUM_PAGES);
		}
		else
		{
			st_ptr->stock_size = STORE_INVEN_MAX;
		}


			/* Allocate the stock */
			C_MAKE(st_ptr->stock, st_ptr->stock_size, object_type);

			/* No table for the black market or home */
			if ((j == STORE_BLACK) || (j == STORE_HOME) || (j == STORE_MUSEUM)) continue;

			/* Assume full table */
			st_ptr->table_size = STORE_CHOICES;

			/* Allocate the stock */
			C_MAKE(st_ptr->table, st_ptr->table_size, s16b);

			/* Scan the choices */
			for (k = 0; k < STORE_CHOICES; k++)
			{
				int k_idx;

				/* Extract the tval/sval codes */
				int tv = store_table[j][k][0];
				int sv = store_table[j][k][1];

				/* Look for it */
				for (k_idx = 1; k_idx < max_k_idx; k_idx++)
				{
					object_kind *k_ptr = &k_info[k_idx];

					/* Found a match */
					if ((k_ptr->tval == tv) && (k_ptr->sval == sv)) break;
				}

				/* Catch errors */
				if (k_idx == max_k_idx) continue;

				/* Add that item index to the table */
				st_ptr->table[st_ptr->table_num++] = k_idx;
			}
		}
	}

	return 0;
}

/*
 * Initialize buildings
 */
/*:::�����������@�X�͕ʁ@�}�b�v���؂�ւ���ĊX�ɗ��邽�тɌĂ΂��*/
///system race class �X�̃I�[�i�[�Ƃ��̍폜�\��
errr init_buildings(void)
{
	int i, j;

	for (i = 0; i < MAX_BLDG; i++)
	{
		building[i].name[0] = '\0';
		building[i].owner_name[0] = '\0';
		building[i].owner_race[0] = '\0';

		for (j = 0; j < 8; j++)
		{
			building[i].act_names[j][0] = '\0';
			building[i].member_costs[j] = 0;
			building[i].other_costs[j] = 0;
			building[i].letters[j] = 0;
			building[i].actions[j] = 0;
			building[i].action_restr[j] = 0;
		}

		for (j = 0; j < MAX_CLASS; j++)
		{
			building[i].member_class[j] = 0;
		}

		for (j = 0; j < MAX_RACES; j++)
		{
			building[i].member_race[j] = 0;
		}

		for (j = 0; j < MAX_MAGIC+1; j++)
		{
			building[i].member_realm[j] = 0;
		}
	}

	return (0);
}


/*
 * Initialize quest array
 */
/*:::�N�G�X�g��ԏ�����*/
static errr init_quests(void)
{
	int i;

	/*** Prepare the quests ***/

	/* Allocate the quests */
	C_MAKE(quest, max_quests, quest_type);

	/* Set all quest to "untaken" */
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = QUEST_STATUS_UNTAKEN;
	}

	return 0;
}


static bool feat_tag_is_not_found = FALSE;


s16b f_tag_to_index_in_init(cptr str)
{
	s16b feat = f_tag_to_index(str);

	if (feat < 0) feat_tag_is_not_found = TRUE;

	return feat;
}


/*
 * Initialize feature variables
 */
/*:::�n�`���̏������̑����炵��*/
static errr init_feat_variables(void)
{
	int i;

	/* Nothing */
	feat_none = f_tag_to_index_in_init("NONE");

	/* Floor */
	feat_floor = f_tag_to_index_in_init("FLOOR");

	/* Objects */
	feat_glyph = f_tag_to_index_in_init("GLYPH");
	feat_explosive_rune = f_tag_to_index_in_init("EXPLOSIVE_RUNE");
	feat_mirror = f_tag_to_index_in_init("MIRROR");

	///mod150719 �z�s�p�ǉ�
	feat_plate = f_tag_to_index_in_init("PLATE");

	///mod150801 �X�^�[�T�t�@�C�A
	feat_bomb = f_tag_to_index_in_init("BOMB");
	feat_elder_sign = f_tag_to_index_in_init("ELDER_SIGN");

	/* Doors */
	feat_door[DOOR_DOOR].open = f_tag_to_index_in_init("OPEN_DOOR");
	feat_door[DOOR_DOOR].broken = f_tag_to_index_in_init("BROKEN_DOOR");
	feat_door[DOOR_DOOR].closed = f_tag_to_index_in_init("CLOSED_DOOR");

	/* Locked doors */
	for (i = 1; i < MAX_LJ_DOORS; i++)
	{
		s16b door = f_tag_to_index(format("LOCKED_DOOR_%d", i));
		if (door < 0) break;
		feat_door[DOOR_DOOR].locked[i - 1] = door;
	}
	if (i == 1) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
	feat_door[DOOR_DOOR].num_locked = i - 1;

	/* Jammed doors */
	for (i = 0; i < MAX_LJ_DOORS; i++)
	{
		s16b door = f_tag_to_index(format("JAMMED_DOOR_%d", i));
		if (door < 0) break;
		feat_door[DOOR_DOOR].jammed[i] = door;
	}
	if (!i) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
	feat_door[DOOR_DOOR].num_jammed = i;

	/* Glass doors */
	feat_door[DOOR_GLASS_DOOR].open = f_tag_to_index_in_init("OPEN_GLASS_DOOR");
	feat_door[DOOR_GLASS_DOOR].broken = f_tag_to_index_in_init("BROKEN_GLASS_DOOR");
	feat_door[DOOR_GLASS_DOOR].closed = f_tag_to_index_in_init("CLOSED_GLASS_DOOR");

	/* Locked glass doors */
	for (i = 1; i < MAX_LJ_DOORS; i++)
	{
		s16b door = f_tag_to_index(format("LOCKED_GLASS_DOOR_%d", i));
		if (door < 0) break;
		feat_door[DOOR_GLASS_DOOR].locked[i - 1] = door;
	}
	if (i == 1) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
	feat_door[DOOR_GLASS_DOOR].num_locked = i - 1;

	/* Jammed glass doors */
	for (i = 0; i < MAX_LJ_DOORS; i++)
	{
		s16b door = f_tag_to_index(format("JAMMED_GLASS_DOOR_%d", i));
		if (door < 0) break;
		feat_door[DOOR_GLASS_DOOR].jammed[i] = door;
	}
	if (!i) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
	feat_door[DOOR_GLASS_DOOR].num_jammed = i;

	/* Curtains */
	feat_door[DOOR_CURTAIN].open = f_tag_to_index_in_init("OPEN_CURTAIN");
	feat_door[DOOR_CURTAIN].broken = feat_door[DOOR_CURTAIN].open;
	feat_door[DOOR_CURTAIN].closed = f_tag_to_index_in_init("CLOSED_CURTAIN");
	feat_door[DOOR_CURTAIN].locked[0] = feat_door[DOOR_CURTAIN].closed;
	feat_door[DOOR_CURTAIN].num_locked = 1;
	feat_door[DOOR_CURTAIN].jammed[0] = feat_door[DOOR_CURTAIN].closed;
	feat_door[DOOR_CURTAIN].num_jammed = 1;

	/* Stairs */
	feat_up_stair = f_tag_to_index_in_init("UP_STAIR");
	feat_down_stair = f_tag_to_index_in_init("DOWN_STAIR");
	feat_entrance = f_tag_to_index_in_init("ENTRANCE");

	/* Normal traps */
	init_normal_traps();

	/* Special traps */
	feat_trap_open = f_tag_to_index_in_init("TRAP_OPEN");
	feat_trap_armageddon = f_tag_to_index_in_init("TRAP_ARMAGEDDON");
	feat_trap_piranha = f_tag_to_index_in_init("TRAP_PIRANHA");

	/* Rubble */
	feat_rubble = f_tag_to_index_in_init("RUBBLE");

	/* Seams */
	feat_magma_vein = f_tag_to_index_in_init("MAGMA_VEIN");
	feat_quartz_vein = f_tag_to_index_in_init("QUARTZ_VEIN");

	/* Walls */
	feat_granite = f_tag_to_index_in_init("GRANITE");
	feat_permanent = f_tag_to_index_in_init("PERMANENT");

	/* Glass floor */
	feat_glass_floor = f_tag_to_index_in_init("GLASS_FLOOR");

	/* Glass walls */
	feat_glass_wall = f_tag_to_index_in_init("GLASS_WALL");
	feat_permanent_glass_wall = f_tag_to_index_in_init("PERMANENT_GLASS_WALL");

	/* Pattern */
	feat_pattern_start = f_tag_to_index_in_init("PATTERN_START");
	feat_pattern_1 = f_tag_to_index_in_init("PATTERN_1");
	feat_pattern_2 = f_tag_to_index_in_init("PATTERN_2");
	feat_pattern_3 = f_tag_to_index_in_init("PATTERN_3");
	feat_pattern_4 = f_tag_to_index_in_init("PATTERN_4");
	feat_pattern_end = f_tag_to_index_in_init("PATTERN_END");
	feat_pattern_old = f_tag_to_index_in_init("PATTERN_OLD");
	feat_pattern_exit = f_tag_to_index_in_init("PATTERN_EXIT");
	feat_pattern_corrupted = f_tag_to_index_in_init("PATTERN_CORRUPTED");

	/* Various */
	feat_black_market = f_tag_to_index_in_init("BLACK_MARKET");
	feat_town = f_tag_to_index_in_init("TOWN");

	/* Terrains */
	feat_deep_water = f_tag_to_index_in_init("DEEP_WATER");
	feat_shallow_water = f_tag_to_index_in_init("SHALLOW_WATER");
	feat_deep_lava = f_tag_to_index_in_init("DEEP_LAVA");
	feat_shallow_lava = f_tag_to_index_in_init("SHALLOW_LAVA");
	feat_dirt = f_tag_to_index_in_init("DIRT");
	feat_grass = f_tag_to_index_in_init("GRASS");
	feat_flower = f_tag_to_index_in_init("FLOWER");
	feat_brake = f_tag_to_index_in_init("BRAKE");
	feat_tree = f_tag_to_index_in_init("TREE");
	feat_mountain = f_tag_to_index_in_init("MOUNTAIN");
	feat_swamp = f_tag_to_index_in_init("SWAMP");

	feat_dark_pit = f_tag_to_index_in_init("DARK_PIT");

	feat_needle_mat = f_tag_to_index_in_init("NEEDLE_MAT");
	feat_ice_wall = f_tag_to_index_in_init("ICE_WALL");

	//v1.1.85
	feat_heavy_cold_zone = f_tag_to_index_in_init("HEAVY_COLD_ZONE");
	feat_cold_zone = f_tag_to_index_in_init("COLD_ZONE");
	feat_heavy_electrical_zone = f_tag_to_index_in_init("HEAVY_ELECTRICAL_ZONE");
	feat_electrical_zone = f_tag_to_index_in_init("ELECTRICAL_ZONE");
	feat_deep_acid_puddle = f_tag_to_index_in_init("DEEP_ACID_PUDDLE");
	feat_acid_puddle = f_tag_to_index_in_init("SHALLOW_ACID_PUDDLE");
	feat_deep_poisonous_puddle = f_tag_to_index_in_init("DEEP_POISONOUS_PUDDLE");
	feat_poisonous_puddle = f_tag_to_index_in_init("SHALLOW_POISONOUS_PUDDLE");
	feat_deep_miasma = f_tag_to_index_in_init("DEEP_MIASMA");
	feat_thin_miasma = f_tag_to_index_in_init("THIN_MIASMA");
	//v1.1.91
	feat_oil_field = f_tag_to_index_in_init("OIL_FIELD");


	/* Unknown grid (not detected) */
	feat_undetected = f_tag_to_index_in_init("UNDETECTED");

	/* Wilderness terrains */
	init_wilderness_terrains();

	return feat_tag_is_not_found ? PARSE_ERROR_UNDEFINED_TERRAIN_TAG : 0;
}


/*
 * Initialize some other arrays
 */
/*:::�l�X�ȃO���[�o���ϐ��̗̈�m�ۂȂ�*/
static errr init_other(void)
{
	int i, n;


	/*** Prepare the "dungeon" information ***/

	/* Allocate and Wipe the object list */
	C_MAKE(o_list, max_o_idx, object_type);

	/* Allocate and Wipe the monster list */
	C_MAKE(m_list, max_m_idx, monster_type);

	/* Allocate and Wipe the monster process list */
	for (i = 0; i < MAX_MTIMED; i++)
	{
		C_MAKE(mproc_list[i], max_m_idx, s16b);
	}

	/* Allocate and Wipe the max dungeon level */
	C_MAKE(max_dlv, max_d_idx, s16b);
	//v1.1.25 �_���W�������e�t���O
	C_MAKE(flag_dungeon_complete, max_d_idx, byte);

	/* Allocate and wipe each line of the cave */
	for (i = 0; i < MAX_HGT; i++)
	{
		/* Allocate one row of the cave */
		C_MAKE(cave[i], MAX_WID, cave_type);
	}


	/*** Prepare the various "bizarre" arrays ***/

	/* Macro variables */
	C_MAKE(macro__pat, MACRO_MAX, cptr);
	C_MAKE(macro__act, MACRO_MAX, cptr);
	C_MAKE(macro__cmd, MACRO_MAX, bool);

	/* Macro action buffer */
	C_MAKE(macro__buf, 1024, char);

	/* Quark variables */
	quark_init();

	/* Message variables */
	C_MAKE(message__ptr, MESSAGE_MAX, u16b);
	C_MAKE(message__buf, MESSAGE_BUF, char);

	/* Hack -- No messages yet */
	message__tail = MESSAGE_BUF;


	/*** Prepare the Player inventory ***/

	/* Allocate it */
	C_MAKE(inventory, INVEN_TOTAL, object_type);

	///mod140608 �ǉ��C���x���g��������
	C_MAKE(inven_add, INVEN_ADD_MAX, object_type);
	///mod160206 �ǉ��C���x���g��������2
	C_MAKE(inven_special, INVEN_SPECIAL_MAX, object_type);

	/*** Prepare the options ***/

	/* Scan the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Accept */
			option_mask[os] |= (1L << ob);

			/* Set */
			if (option_info[i].o_norm)
			{
				/* Set */
				option_flag[os] |= (1L << ob);
			}

			/* Clear */
			else
			{
				/* Clear */
				option_flag[os] &= ~(1L << ob);
			}
		}
	}

	/* Analyze the windows */
	for (n = 0; n < 8; n++)
	{
		/* Analyze the options */
		for (i = 0; i < 32; i++)
		{
			/* Accept */
			if (window_flag_desc[i])
			{
				/* Accept */
				window_mask[n] |= (1L << i);
			}
		}
	}

	/*
	 *  Set the "default" window flags
	 *  Window 1 : Display messages
	 *  Window 2 : Display inven/equip
	 */
	window_flag[1] = 1L << 6;
	window_flag[2] = 1L << 0;


	/*** Pre-allocate space for the "format()" buffer ***/

	/* Hack -- Just call the "format()" function */
	(void)format("%s (%s).", "Mr.Hoge", MAINTAINER);


	/* Success */
	return (0);
}


/*
 * Initialize some other arrays
 */
static errr init_object_alloc(void)
{
	int i, j;
	object_kind *k_ptr;
	alloc_entry *table;
	s16b num[MAX_DEPTH];
	s16b aux[MAX_DEPTH];


	/*** Analyze object allocation info ***/

	/* Clear the "aux" array */
	(void)C_WIPE(&aux, MAX_DEPTH, s16b);

	/* Clear the "num" array */
	(void)C_WIPE(&num, MAX_DEPTH, s16b);

	/* Free the old "alloc_kind_table" (if it exists) */
	if (alloc_kind_table)
	{
		C_KILL(alloc_kind_table, alloc_kind_size, alloc_entry);
	}

	/* Size of "alloc_kind_table" */
	alloc_kind_size = 0;

	/* Scan the objects */
	for (i = 1; i < max_k_idx; i++)
	{
		k_ptr = &k_info[i];

		/* Scan allocation pairs */
		for (j = 0; j < 4; j++)
		{
			/* Count the "legal" entries */
			if (k_ptr->chance[j])
			{
				/* Count the entries */
				alloc_kind_size++;

				/* Group by level */
				num[k_ptr->locale[j]]++;
			}
		}
	}

	/* Collect the level indexes */
	for (i = 1; i < MAX_DEPTH; i++)
	{
		/* Group by level */
		num[i] += num[i-1];
	}

	/* Paranoia */
#ifdef JP
if (!num[0]) quit("���̃A�C�e�����Ȃ��I");
#else
	if (!num[0]) quit("No town objects!");
#endif



	/*** Initialize object allocation info ***/

	/* Allocate the alloc_kind_table */
	C_MAKE(alloc_kind_table, alloc_kind_size, alloc_entry);

	/* Access the table entry */
	table = alloc_kind_table;

	/* Scan the objects */
	for (i = 1; i < max_k_idx; i++)
	{
		k_ptr = &k_info[i];

		/* Scan allocation pairs */
		for (j = 0; j < 4; j++)
		{
			/* Count the "legal" entries */
			if (k_ptr->chance[j])
			{
				int p, x, y, z;

				/* Extract the base level */
				x = k_ptr->locale[j];

				/* Extract the base probability */
				p = (100 / k_ptr->chance[j]);

				/* Skip entries preceding our locale */
				y = (x > 0) ? num[x-1] : 0;

				/* Skip previous entries at this locale */
				z = y + aux[x];

				/* Load the entry */
				table[z].index = i;
				table[z].level = x;
				table[z].prob1 = p;
				table[z].prob2 = p;
				table[z].prob3 = p;

				/* Another entry complete for this locale */
				aux[x]++;
			}
		}
	}

	/* Success */
	return (0);
}


/*
 * Initialize some other arrays
 */
/*:::�l�X�ȏ�����*/
static errr init_alloc(void)
{
	int i;
	monster_race *r_ptr;

	///system �l���Ƀ����X�^�[���łȂ����邽�߂Ɉ�H�v����̂�������Ȃ�
#ifdef SORT_R_INFO

	tag_type *elements;

	/* Allocate the "r_info" array */
	C_MAKE(elements, max_r_idx, tag_type);

	/* Scan the monsters */
	/*:::r_info[]�����x�����ɐ���H*/
	for (i = 1; i < max_r_idx; i++)
	{
		elements[i].tag = r_info[i].level;
		elements[i].index = i;
	}

	tag_sort(elements, max_r_idx);

	/*** Initialize monster allocation info ***/

	/* Size of "alloc_race_table" */
	alloc_race_size = max_r_idx;

	/* Allocate the alloc_race_table */
	C_MAKE(alloc_race_table, alloc_race_size, alloc_entry);

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		/* Get the i'th race */
		r_ptr = &r_info[elements[i].index];

		/* Count valid pairs */
		if (r_ptr->rarity)
		{
			int p, x;

			/* Extract the base level */
			x = r_ptr->level;

			/* Extract the base probability */
			p = (100 / r_ptr->rarity);

			/* Load the entry */
			alloc_race_table[i].index = elements[i].index;
			alloc_race_table[i].level = x;
			alloc_race_table[i].prob1 = p;
			alloc_race_table[i].prob2 = p;
			alloc_race_table[i].prob3 = p;
		}
	}

	/* Free the "r_info" array */
	C_KILL(elements, max_r_idx, tag_type);

#else /* SORT_R_INFO */

	int j;
	alloc_entry *table;
	s16b num[MAX_DEPTH];
	s16b aux[MAX_DEPTH];

	/*** Analyze monster allocation info ***/

	/* Clear the "aux" array */
	C_WIPE(&aux, MAX_DEPTH, s16b);

	/* Clear the "num" array */
	C_WIPE(&num, MAX_DEPTH, s16b);

	/* Size of "alloc_race_table" */
	alloc_race_size = 0;

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		/* Get the i'th race */
		r_ptr = &r_info[i];

		/* Legal monsters */
		if (r_ptr->rarity)
		{
			/* Count the entries */
			alloc_race_size++;

			/* Group by level */
			num[r_ptr->level]++;
		}
	}

	/* Collect the level indexes */
	for (i = 1; i < MAX_DEPTH; i++)
	{
		/* Group by level */
		num[i] += num[i-1];
	}

	/* Paranoia */
#ifdef JP
	if (!num[0]) quit("���̃����X�^�[���Ȃ��I");
#else
	if (!num[0]) quit("No town monsters!");
#endif



	/*** Initialize monster allocation info ***/

	/* Allocate the alloc_race_table */
	C_MAKE(alloc_race_table, alloc_race_size, alloc_entry);

	/* Access the table entry */
	table = alloc_race_table;

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		/* Get the i'th race */
		r_ptr = &r_info[i];

		/* Count valid pairs */
		if (r_ptr->rarity)
		{
			int p, x, y, z;

			/* Extract the base level */
			x = r_ptr->level;

			/* Extract the base probability */
			p = (100 / r_ptr->rarity);

			/* Skip entries preceding our locale */
			y = (x > 0) ? num[x-1] : 0;

			/* Skip previous entries at this locale */
			z = y + aux[x];

			/* Load the entry */
			table[z].index = i;
			table[z].level = x;
			table[z].prob1 = p;
			table[z].prob2 = p;
			table[z].prob3 = p;

			/* Another entry complete for this locale */
			aux[x]++;
		}
	}

#endif /* SORT_R_INFO */

	/* Init the "alloc_kind_table" */
	(void)init_object_alloc();

	/* Success */
	return (0);
}



/*
 * Hack -- take notes on line 23
 */
/*:::��ʂ̐^�񒆂ɕ�����\��*/
static void note(cptr str)
{
	Term_erase(0, 23, 255);
	Term_putstr(20, 23, -1, TERM_WHITE, str);
	Term_fresh();
}



/*
 * Hack -- Explain a broken "lib" folder and quit (see below).
 *
 * XXX XXX XXX This function is "messy" because various things
 * may or may not be initialized, but the "plog()" and "quit()"
 * functions are "supposed" to work under any conditions.
 */
/*:::init_angband�ŃG���[���N�������炱���ɗ���*/
static void init_angband_aux(cptr why)
{
	/* Why */
	plog(why);

#ifdef JP
	/* Explain */
	plog("'lib'�f�B���N�g�������݂��Ȃ������Ă���悤�ł��B");

	/* More details */
	plog("�Ђ���Ƃ���ƃA�[�J�C�u���������𓀂���Ă��Ȃ��̂�������܂���B");

	/* Explain */
	plog("�Y������'README'�t�@�C����ǂ�Ŋm�F���Ă݂ĉ������B");

	/* Quit with error */
	quit("�v���I�ȃG���[�B");
#else
	/* Explain */
	plog("The 'lib' directory is probably missing or broken.");

	/* More details */
	plog("Perhaps the archive was not extracted correctly.");

	/* Explain */
	plog("See the 'README' file for more information.");

	/* Quit with error */
	quit("Fatal Error.");
#endif

}


/*
 * Hack -- main Angband initialization entry point
 *
 * Verify some files, display the "news.txt" file, create
 * the high score file, initialize all internal arrays, and
 * load the basic "user pref files".
 *:::�t�@�C���̌��؁@news.txt�\���@�n�C�X�R�A�t�@�C���쐬�����ϐ��������@�ݒ�t�@�C�����[�h
 *
 * Be very careful to keep track of the order in which things
 * are initialized, in particular, the only thing *known* to
 * be available when this function is called is the "z-term.c"
 * package, and that may not be fully initialized until the
 * end of this function, when the default "user pref files"
 * are loaded and "Term_xtra(TERM_XTRA_REACT,0)" is called.
 *:::�������̏��Ԃɂ͒���
 *
 * Note that this function attempts to verify the "news" file,
 * and the game aborts (cleanly) on failure, since without the
 * "news" file, it is likely that the "lib" folder has not been
 * correctly located.  Otherwise, the news file is displayed for
 * the user.
 *:::lib�t�H���_�������ƒu����ĂȂ��ƃQ�[�����n�܂�Ȃ�
 *
 * Note that this function attempts to verify (or create) the
 * "high score" file, and the game aborts (cleanly) on failure,
 * since one of the most common "extraction" failures involves
 * failing to extract all sub-directories (even empty ones), such
 * as by failing to use the "-d" option of "pkunzip", or failing
 * to use the "save empty directories" option with "Compact Pro".
 * This error will often be caught by the "high score" creation
 * code below, since the "lib/apex" directory, being empty in the
 * standard distributions, is most likely to be "lost", making it
 * impossible to create the high score file.
 *:::lib/apex�f�B���N�g�������݂��Ȃ��ƃn�C�X�R�A�t�@�C������ꂸ������H
 *
 * Note that various things are initialized by this function,
 * including everything that was once done by "init_some_arrays".
 *
 * This initialization involves the parsing of special files
 * in the "lib/data" and sometimes the "lib/edit" directories.
 *:::�f�[�^�̏�������lib/data��lib/edit�̃t�@�C�����Q�Ƃ���
 *
 * Note that the "template" files are initialized first, since they
 * often contain errors.  This means that macros and message recall
 * and things like that are not available until after they are done.
 *:::�e���v���[�g�t�@�C�����ŏ��ɏ����������H
 *
 * We load the default "user pref files" here in case any "color"
 * changes are needed before character creation.
 *:::pref�t�@�C���H
 *
 * Note that the "graf-xxx.prf" file must be loaded separately,
 * if needed, in the first (?) pass through "TERM_XTRA_REACT".
 */
void init_angband(void)
{
	int fd = -1;

	int mode = 0664;

	FILE *fp;

	char buf[1024];

/*:::news_j.txt��ǂݍ���ŕ\��*/
	/*** Verify the "news" file ***/

	/* Build the filename */
#ifdef JP
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "news_j.txt");
#else
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "news.txt");
#endif


	/* Attempt to open the file */
	fd = fd_open(buf, O_RDONLY);

	/* Failure */
	if (fd < 0)
	{
		char why[1024];

		/* Message */
#ifdef JP
	sprintf(why, "'%s'�t�@�C���ɃA�N�Z�X�ł��܂���!", buf);
#else
		sprintf(why, "Cannot access the '%s' file!", buf);
#endif


		/* Crash and burn */
		init_angband_aux(why);
	}

	/* Close it */
	(void)fd_close(fd);


	/*** Display the "news" file ***/

	/* Clear screen */
	Term_clear();

	/* Build the filename */
#ifdef JP
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "news_j.txt");
#else
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "news.txt");
#endif


	/* Open the News file */
	fp = my_fopen(buf, "r");

	/* Dump */
	if (fp)
	{
		int i = 0;

		/* Dump the file to the screen */
		while (0 == my_fgets(fp, buf, sizeof(buf)))
		{
			/* Display and advance */
			Term_putstr(0, i++, -1, TERM_WHITE, buf);
		}

		/* Close */
		my_fclose(fp);
	}

	/* Flush it */
	Term_flush();


	/*** Verify (or create) the "high score" file ***/

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	/* Attempt to open the high score file */
	fd = fd_open(buf, O_RDONLY);

	/* Failure */
	if (fd < 0)
	{
		/* File type is "DATA" */
		FILE_TYPE(FILE_TYPE_DATA);

		/* Grab permissions */
		safe_setuid_grab();

		/* Create a new high score file */
		fd = fd_make(buf, mode);

		/* Drop permissions */
		safe_setuid_drop();

		/* Failure */
		if (fd < 0)
		{
			char why[1024];

			/* Message */
#ifdef JP
			sprintf(why, "'%s'�t�@�C�����쐬�ł��܂���!", buf);
#else
			sprintf(why, "Cannot create the '%s' file!", buf);
#endif


			/* Crash and burn */
			init_angband_aux(why);
		}
	}

	/* Close it */
	(void)fd_close(fd);


	/*** Initialize some arrays ***/

	/* Initialize misc. values */
#ifdef JP
note("[�ϐ������������Ă��܂�...(���̑�)");
#else
	note("[Initializing values... (misc)]");
#endif

#ifdef JP
if (init_misc()) quit("���̑��̕ϐ����������ł��܂���");
#else
	if (init_misc()) quit("Cannot initialize misc. values");
#endif


	/* Initialize feature info */
#ifdef JP
	note("[�f�[�^�̏�������... (�n�`)]");
	if (init_f_info()) quit("�n�`�������s�\");
	if (init_feat_variables()) quit("�n�`�������s�\");
#else
	note("[Initializing arrays... (features)]");
	if (init_f_info()) quit("Cannot initialize features");
	if (init_feat_variables()) quit("Cannot initialize features");
#endif


	/* Initialize object info */
#ifdef JP
	note("[�f�[�^�̏�������... (�A�C�e��)]");
	if (init_k_info()) quit("�A�C�e���������s�\");
#else
	note("[Initializing arrays... (objects)]");
	if (init_k_info()) quit("Cannot initialize objects");
#endif


	/* Initialize artifact info */
#ifdef JP
	note("[�f�[�^�̏�������... (�`���̃A�C�e��)]");
	if (init_a_info()) quit("�`���̃A�C�e���������s�\");
#else
	note("[Initializing arrays... (artifacts)]");
	if (init_a_info()) quit("Cannot initialize artifacts");
#endif


	/* Initialize ego-item info */
#ifdef JP
	note("[�f�[�^�̏�������... (���̂���A�C�e��)]");
	if (init_e_info()) quit("���̂���A�C�e���������s�\");
#else
	note("[Initializing arrays... (ego-items)]");
	if (init_e_info()) quit("Cannot initialize ego-items");
#endif


	/* Initialize monster info */
#ifdef JP
	note("[�f�[�^�̏�������... (�����X�^�[)]");
	if (init_r_info()) quit("�����X�^�[�������s�\");
#else
	note("[Initializing arrays... (monsters)]");
	if (init_r_info()) quit("Cannot initialize monsters");
#endif


	/* Initialize dungeon info */
#ifdef JP
	note("[�f�[�^�̏�������... (�_���W����)]");
	if (init_d_info()) quit("�_���W�����������s�\");
#else
	note("[Initializing arrays... (dungeon)]");
	if (init_d_info()) quit("Cannot initialize dungeon");
#endif
	{
		int i;
		for (i = 1; i < max_d_idx; i++)
			if (d_info[i].final_guardian)
				r_info[d_info[i].final_guardian].flags7 |= RF7_GUARDIAN;
	}
#if 0
	/* Initialize magic info */
#ifdef JP
	note("[�f�[�^�̏�������... (���@)]");
	if (init_m_info()) quit("���@�������s�\");
#else
	note("[Initializing arrays... (magic)]");
	if (init_m_info()) quit("Cannot initialize magic");
#endif
#endif

	/* Initialize weapon_exp info */
	///sys skill ����E�Z�\�n���x�ݒ�t�@�C���@������N���X�ݒ�l�ɓ������č폜����\��
#ifdef JP
	//note("[�f�[�^�̏�������... (�n���x)]");
	//if (init_s_info()) quit("�n���x�������s�\");
#else
	note("[Initializing arrays... (skill)]");
	if (init_s_info()) quit("Cannot initialize skill");
#endif

	/* Initialize wilderness array */
#ifdef JP
note("[�z������������Ă��܂�... (�r��)]");
#else
	note("[Initializing arrays... (wilderness)]");
#endif

#ifdef JP
if (init_wilderness()) quit("�r����������ł��܂���");
#else
	if (init_wilderness()) quit("Cannot initialize wilderness");
#endif


	/* Initialize town array */
#ifdef JP
note("[�z������������Ă��܂�... (�X)]");
#else
	note("[Initializing arrays... (towns)]");
#endif

#ifdef JP
if (init_towns()) quit("�X���������ł��܂���");
#else
	if (init_towns()) quit("Cannot initialize towns");
#endif


	/* Initialize building array */
#ifdef JP
note("[�z������������Ă��܂�... (����)]");
#else
	note("[Initializing arrays... (buildings)]");
#endif

#ifdef JP
if (init_buildings()) quit("�������������ł��܂���");
#else
	if (init_buildings()) quit("Cannot initialize buildings");
#endif


	/* Initialize quest array */
#ifdef JP
note("[�z������������Ă��܂�... (�N�G�X�g)]");
#else
	note("[Initializing arrays... (quests)]");
#endif

#ifdef JP
if (init_quests()) quit("�N�G�X�g���������ł��܂���");
#else
	if (init_quests()) quit("Cannot initialize quests");
#endif


	/* Initialize vault info */
#ifdef JP
	if (init_v_info()) quit("vault �������s�\");
#else
	if (init_v_info()) quit("Cannot initialize vaults");
#endif


	/* Initialize some other arrays */
#ifdef JP
	note("[�f�[�^�̏�������... (���̑�)]");
	if (init_other()) quit("���̑��̃f�[�^�������s�\");
#else
	note("[Initializing arrays... (other)]");
	if (init_other()) quit("Cannot initialize other stuff");
#endif


	/* Initialize some other arrays */
#ifdef JP
	/* translation */
	note("[�f�[�^�̏�������... (�A���P�[�V����)]");
	if (init_alloc()) quit("�A���P�[�V�����E�X�^�b�t�������s�\");
#else
	note("[Initializing arrays... (alloc)]");
	if (init_alloc()) quit("Cannot initialize alloc stuff");
#endif



	/*** Load default user pref files ***/

	/* Initialize feature info */
#ifdef JP
note("[���[�U�[�ݒ�t�@�C�������������Ă��܂�...]");
#else
	note("[Initializing user pref files...]");
#endif


	/* Access the "basic" pref file */
	strcpy(buf, "pref.prf");

	/* Process that file */
	process_pref_file(buf);

	/* Access the "basic" system pref file */
	sprintf(buf, "pref-%s.prf", ANGBAND_SYS);

	/* Process that file */
	process_pref_file(buf);

	/* Done */
#ifdef JP
	note("[�������I��]");
#else
	note("[Initialization complete]");
#endif

}

/*
 *  Get check sum in string form
 */
cptr get_check_sum(void)
{
	return format("%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
		      f_head.v_extra, 
		      k_head.v_extra, 
		      a_head.v_extra, 
		      e_head.v_extra, 
		      r_head.v_extra, 
		      d_head.v_extra, 
		      m_head.v_extra, 
		      s_head.v_extra, 
		      v_head.v_extra);
}

