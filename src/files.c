/* File: files.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: code dealing with files (and death) */

#include "angband.h"


/*
 * You may or may not want to use the following "#undef".
 */
/* #undef _POSIX_SAVED_IDS */


/*
 * Hack -- drop permissions
 */
void safe_setuid_drop(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(getuid()) != 0)
	{
#ifdef JP
quit("setuid(): �������������܂���I");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(getgid()) != 0)
	{
#ifdef JP
quit("setgid(): �������������܂���I");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
quit("setreuid(): �������������܂���I");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
quit("setregid(): �������������܂���I");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif

# endif

#endif

}


/*
 * Hack -- grab permissions
 */
/*:::���Ԃ�Unix��Linux�p�̌����m�F�@*/
void safe_setuid_grab(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(player_euid) != 0)
	{
#ifdef JP
quit("setuid(): �������������܂���I");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(player_egid) != 0)
	{
#ifdef JP
quit("setgid(): �������������܂���I");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
quit("setreuid(): �������������܂���I");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
quit("setregid(): �������������܂���I");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif /* SAFE_SETUID_POSIX */

# endif /* SAFE_SETUID */

#endif /* SET_UID */

}


/*
 * Extract the first few "tokens" from a buffer
 *
 * This function uses "colon" and "slash" as the delimeter characters.
 *
 * We never extract more than "num" tokens.  The "last" token may include
 * "delimeter" characters, allowing the buffer to include a "string" token.
 *
 * We save pointers to the tokens in "tokens", and return the number found.
 *
 * Hack -- Attempt to handle the 'c' character formalism
 *
 * Hack -- An empty buffer, or a final delimeter, yields an "empty" token.
 *
 * Hack -- We will always extract at least one token
 */
/*::: ":"��"/"�ŋ�؂�ꂽ������𕪊�����**token�Ɋi�[���A���̐���Ԃ� num�ȏ�̕����͖��������@*/
s16b tokenize(char *buf, s16b num, char **tokens, int mode)
{
	int i = 0;

	char *s = buf;


	/* Process */
	while (i < num - 1)
	{
		char *t;

		/* Scan the string */
		for (t = s; *t; t++)
		{
			/* Found a delimiter */
			if ((*t == ':') || (*t == '/')) break;

			/* Handle single quotes */
			/*:::TOKENIZE_CHECKQUOTE=0x01*/
			/*:::mode�ɂ��̐��l�������Ă���Ƃ���'��\�ɉ��炩�̏���������炵���H�ǂ�Ȃӂ��Ɏg���̂��ǂ����������*/
			if ((mode & TOKENIZE_CHECKQUOTE) && (*t == '\''))
			{
				/* Advance */
				t++;

				/* Handle backslash */
				if (*t == '\\') t++;

				/* Require a character */
				if (!*t) break;

				/* Advance */
				t++;

				/* Hack -- Require a close quote */
				if (*t != '\'') *t = '\'';
			}

			/* Handle back-slash */
			if (*t == '\\') t++;
		}

		/* Nothing left */
		if (!*t) break;

		/* Nuke and advance */
		/*:::������s��\0��������tokens�������̕�����Ƃ��ēƗ������H*/
		*t++ = '\0';

		/* Save the token */
		tokens[i++] = s;

		/* Advance */
		s = t;
	}

	/* Save the token */
	tokens[i++] = s;

	/* Number found */
	return (i);
}


/* A number with a name */
typedef struct named_num named_num;

struct named_num
{
	cptr name;		/* The name of this thing */
	int num;			/* A number associated with it */
};

///mod131229 GF_xxx�̒ǉ�
//spell.xx.prf�̋L�q�Ɠ����I��GF_XXX�̒l��Ή��t���邽�߂̕\
//GF_XXX�̏��Ԃǂ���łȂ��Ă��������Ȃ�ׂ������Ƃ�����
/* Index of spell type names */
static named_num gf_desc[] =
{
	{"GF_ELEC", 				GF_ELEC				},
	{"GF_POIS", 				GF_POIS				},
	{"GF_ACID", 				GF_ACID				},
	{"GF_COLD", 				GF_COLD				},
	{"GF_FIRE",		 			GF_FIRE				},
	{"GF_PSY_SPEAR",			GF_PSY_SPEAR			},
	{"GF_MISSILE",				GF_MISSILE			},
	{"GF_ARROW",				GF_ARROW				},
	{"GF_PLASMA",				GF_PLASMA			},
	{"GF_WATER",				GF_WATER				},
	{"GF_LITE",					GF_LITE				},
	{"GF_DARK",					GF_DARK				},
	{"GF_LITE_WEAK",			GF_LITE_WEAK		},
	{"GF_DARK_WEAK",			GF_DARK_WEAK		},
	{"GF_SHARDS",				GF_SHARDS			},
	{"GF_SOUND",				GF_SOUND				},
	{"GF_CONFUSION",			GF_CONFUSION		},
	{"GF_FORCE",				GF_FORCE				},
	{"GF_INACT",				GF_INACT			},
	{"GF_MANA",					GF_MANA				},
	{"GF_METEOR",				GF_METEOR			},
	{"GF_ICE",					GF_ICE				},
	{"GF_CHAOS",				GF_CHAOS				},
	//30
	{"GF_NETHER",				GF_NETHER			},
	{"GF_DISENCHANT",			GF_DISENCHANT		},
	{"GF_NEXUS",				GF_NEXUS				},
	{"GF_TIME",					GF_TIME				},
	{"GF_GRAVITY",				GF_GRAVITY			},
	{"GF_DEC_ATK",				GF_DEC_ATK },
	{"GF_DEC_DEF",				GF_DEC_DEF },
	{"GF_DEC_MAG",				GF_DEC_MAG },
	{"GF_DEC_ALL",				GF_DEC_ALL },
	{"GF_KILL_WALL",			GF_KILL_WALL		},
	{"GF_KILL_DOOR",			GF_KILL_DOOR		},
	{"GF_KILL_TRAP",			GF_KILL_TRAP		},
	{ "GF_NO_MOVE",				GF_NO_MOVE },
	{ "GF_BERSERK",				GF_BERSERK },
	{ "GF_SUPER_EGO",			GF_SUPER_EGO },

	{"GF_MAKE_DOOR",			GF_MAKE_DOOR		},
	{"GF_MAKE_TRAP",			GF_MAKE_TRAP		},
	{"GF_MAKE_TREE",			GF_MAKE_TREE		},
	{ "GF_DIG_OIL",			GF_DIG_OIL },
	{ "GF_ALCOHOL",			GF_ALCOHOL },

	{"GF_OLD_CLONE",			GF_OLD_CLONE		},
	{"GF_OLD_POLY",			GF_OLD_POLY			},
	{"GF_OLD_HEAL",			GF_OLD_HEAL			},
	{"GF_OLD_SPEED",			GF_OLD_SPEED		},
	{"GF_OLD_SLOW",			GF_OLD_SLOW			},
	{"GF_OLD_CONF",			GF_OLD_CONF			},
	{"GF_OLD_SLEEP",			GF_OLD_SLEEP		},
	{"GF_OLD_DRAIN",			GF_OLD_DRAIN		},
	{ "GF_PIT_FALL",			GF_PIT_FALL },
	{ "GF_ACTIV_TRAP",			GF_ACTIV_TRAP },
	{"GF_AWAY_UNDEAD",		GF_AWAY_UNDEAD		},
	{"GF_AWAY_EVIL",			GF_AWAY_EVIL		},
	{"GF_AWAY_ALL",			GF_AWAY_ALL			},
	{"GF_TURN_UNDEAD",		GF_TURN_UNDEAD		},
	{"GF_TURN_EVIL",			GF_TURN_EVIL		},
	{"GF_TURN_ALL",			GF_TURN_ALL			},
	{"GF_DISP_UNDEAD",		GF_DISP_UNDEAD		},
	{"GF_DISP_EVIL",			GF_DISP_EVIL		},
	{"GF_DISP_ALL",			GF_DISP_ALL			},
	{"GF_DISP_DEMON",			GF_DISP_DEMON		},
	{"GF_DISP_LIVING",		GF_DISP_LIVING		},
	{"GF_ROCKET",				GF_ROCKET			},
	{"GF_NUKE",					GF_NUKE				},
	{"GF_MAKE_GLYPH",			GF_MAKE_GLYPH		},
	{"GF_STASIS",				GF_STASIS			},
	{"GF_STONE_WALL",			GF_STONE_WALL		},
	{"GF_DEATH_RAY",			GF_DEATH_RAY		},
	{"GF_STUN",					GF_STUN				},
	{"GF_HOLY_FIRE",			GF_HOLY_FIRE		},
	{"GF_HELL_FIRE",			GF_HELL_FIRE		},
	{"GF_DISINTEGRATE",		GF_DISINTEGRATE	},
	{"GF_CHARM",				GF_CHARM				},
	{"GF_CONTROL_UNDEAD",	GF_CONTROL_UNDEAD	},
	{"GF_CONTROL_ANIMAL",	GF_CONTROL_ANIMAL	},
	{"GF_PSI",					GF_PSI				},
	{"GF_PSI_DRAIN",			GF_PSI_DRAIN		},
	{"GF_TELEKINESIS",		GF_TELEKINESIS		},
	{"GF_JAM_DOOR",			GF_JAM_DOOR			},
	{"GF_DOMINATION",			GF_DOMINATION		},
	{"GF_DISP_GOOD",			GF_DISP_GOOD		},
	{"GF_DRAIN_MANA",			GF_DRAIN_MANA		},
	{"GF_MIND_BLAST",			GF_MIND_BLAST		},
	{"GF_BRAIN_SMASH",			GF_BRAIN_SMASH		},
	{"GF_CAUSE_1",			GF_CAUSE_1		},
	{"GF_CAUSE_2",			GF_CAUSE_2		},
	{"GF_CAUSE_3",			GF_CAUSE_3		},
	{"GF_CAUSE_4",			GF_CAUSE_4		},
	{"GF_HAND_DOOM",			GF_HAND_DOOM		},
	{"GF_CAPTURE",			GF_CAPTURE		},
	{"GF_ANIM_DEAD",			GF_ANIM_DEAD		},
	{"GF_CONTROL_LIVING",		GF_CONTROL_LIVING	},
	{"GF_IDENTIFY",			GF_IDENTIFY	},
	{"GF_ATTACK",			GF_ATTACK	},
	{"GF_ENGETSU",			GF_ENGETSU	},
	{"GF_GENOCIDE",			GF_GENOCIDE	},
	{"GF_PHOTO",			GF_PHOTO	},
	{"GF_CONTROL_DEMON",	GF_CONTROL_DEMON	},
	{"GF_LAVA_FLOW",	GF_LAVA_FLOW	},
	{"GF_BLOOD_CURSE",	GF_BLOOD_CURSE	},
	{"GF_SEEKER",			GF_SEEKER			},
	{"GF_SUPER_RAY",		GF_SUPER_RAY			},
	{"GF_STAR_HEAL",		GF_STAR_HEAL			},
	{"GF_WATER_FLOW",		GF_WATER_FLOW			},
	{"GF_CRUSADE",		GF_CRUSADE			},
	{"GF_STASIS_EVIL",			GF_STASIS_EVIL		},
	{"GF_WOUNDS",			GF_WOUNDS		},
	{"GF_DISTORTION",		GF_DISTORTION	},
	{"GF_COSMIC_HORROR",			GF_COSMIC_HORROR		},
	{"GF_PUNISH_1",			GF_PUNISH_1		},
	{"GF_PUNISH_2",			GF_PUNISH_2		},
	{"GF_PUNISH_3",			GF_PUNISH_3		},
	{"GF_PUNISH_4",			GF_PUNISH_4		},
	{"GF_POLLUTE",			GF_POLLUTE		},
	{"GF_TORNADO",			GF_TORNADO		},
	{"GF_KYUTTOSHITEDOKA_N",GF_KYUTTOSHITEDOKA_N},
	{"GF_WINDCUTTER",			GF_WINDCUTTER		},
	{"GF_REDEYE",			GF_REDEYE		},
	{"GF_RYUURI",			GF_RYUURI		},
	{"GF_POSSESSION",			GF_POSSESSION		},
	{"GF_SOULSTEAL",			GF_SOULSTEAL		},
	{"GF_NIGHTMARE",			GF_NIGHTMARE		},
	{"GF_TRAIN",			GF_TRAIN		},
	{"GF_DISP_KWAI",			GF_DISP_KWAI		},
	{"GF_NORTHERN",			GF_NORTHERN		},
	{"GF_BANKI_BEAM",			GF_BANKI_BEAM		},
	{"GF_N_E_P",			GF_N_E_P		},
	{"GF_HOUTOU",			GF_HOUTOU		},
	{"GF_BANKI_BEAM2",			GF_BANKI_BEAM2		},
	{"GF_CHARM_PALADIN",				GF_CHARM_PALADIN	},
	{"GF_SATORI",				GF_SATORI	},
	{"GF_MAKE_FLOWER",				GF_MAKE_FLOWER	},
	{"GF_KOKORO",				GF_KOKORO	},
	{"GF_MOSES",				GF_MOSES	},
	{"GF_REDMAGIC",				GF_REDMAGIC	},
	{"GF_GUNGNIR",				GF_GUNGNIR	},
	{"GF_WALL_TO_DOOR",				GF_WALL_TO_DOOR	},
	{"GF_STASIS_DRAGON",			GF_STASIS_DRAGON			},
	{"GF_CHARM_C",			GF_CHARM_C			},
	{"GF_CHARM_J",			GF_CHARM_J			},
	{"GF_CHARM_B",			GF_CHARM_B			},
	{"GF_MAKE_SPIDER_NEST",	GF_MAKE_SPIDER_NEST},
	{"GF_THROW_PLATE",	GF_THROW_PLATE},
	{"GF_BLAZINGSTAR",	GF_BLAZINGSTAR},
	{"GF_SOULSCULPTURE",	GF_SOULSCULPTURE},
	{"GF_YAKU",	GF_YAKU},
	{"GF_STEAM",	GF_STEAM},
	{"GF_SEIRAN_BEAM",	GF_SEIRAN_BEAM},
	{"GF_PURIFY",	GF_PURIFY},
	{"GF_SPECIAL_SHOT",	GF_SPECIAL_SHOT},
	{"GF_YOUMU",	GF_YOUMU},
	{"GF_STASIS_LIVING",			GF_STASIS_LIVING },
	{ "GF_LUNATIC_TORCH",			GF_LUNATIC_TORCH },
	{ "GF_BRAIN_FINGER_PRINT",			GF_BRAIN_FINGER_PRINT },
	{ "GF_RAINBOW",			GF_RAINBOW },
	{ "GF_CONTROL_FISH",			GF_CONTROL_FISH },
	{ "GF_MAKE_BLIZZARD",			GF_MAKE_BLIZZARD },
	{ "GF_MAKE_POISON_PUDDLE",			GF_MAKE_POISON_PUDDLE },
	{ "GF_MAKE_ACID_PUDDLE",			GF_MAKE_ACID_PUDDLE },
	{ "GF_MAKE_STORM",			GF_MAKE_STORM },
	{ "GF_KANAMEISHI",			GF_KANAMEISHI },
	{ "GF_HOLY_WATER",			GF_HOLY_WATER },
	{ "GF_TIMED_SHARD",			GF_TIMED_SHARD },


	{NULL, 						0						}
};


/*
 * Parse a sub-file of the "extra info" (format shown below)
 *
 * Each "action" line has an "action symbol" in the first column,
 * followed by a colon, followed by some command specific info,
 * usually in the form of "tokens" separated by colons or slashes.
 *
 * Blank lines, lines starting with white space, and lines starting
 * with pound signs ("#") are ignored (as comments).
 *
 * Note the use of "tokenize()" to allow the use of both colons and
 * slashes as delimeters, while still allowing final tokens which
 * may contain any characters including "delimiters".
 *
 * Note the use of "strtol()" to allow all "integers" to be encoded
 * in decimal, hexidecimal, or octal form.
 *
 * Note that "monster zero" is used for the "player" attr/char, "object
 * zero" will be used for the "stack" attr/char, and "feature zero" is
 * used for the "nothing" attr/char.
 *
 * Parse another file recursively, see below for details
 *   %:<filename>
 *
 * Specify the attr/char values for "monsters" by race index
 *   R:<num>:<a>:<c>
 *
 * Specify the attr/char values for "objects" by kind index
 *   K:<num>:<a>:<c>
 *
 * Specify the attr/char values for "features" by feature index
 *   F:<num>:<a>:<c>
 *
 * Specify the attr/char values for unaware "objects" by kind tval
 *   U:<tv>:<a>:<c>
 *
 * Specify the attr/char values for inventory "objects" by kind tval
 *   E:<tv>:<a>:<c>
 *
 * Define a macro action, given an encoded macro action
 *   A:<str>
 *
 * Create a normal macro, given an encoded macro trigger
 *   P:<str>
 *
 * Create a command macro, given an encoded macro trigger
 *   C:<str>
 *
 * Create a keyset mapping
 *   S:<key>:<key>:<dir>
 *
 * Turn an option off, given its name
 *   X:<str>
 *
 * Turn an option on, given its name
 *   Y:<str>
 *
 * Specify visual information, given an index, and some data
 *   V:<num>:<kv>:<rv>:<gv>:<bv>
 *
 * Specify the set of colors to use when drawing a zapped spell
 *   Z:<type>:<str>
 *
 * Specify a macro trigger template and macro trigger names.
 *   T:<template>:<modifier chr>:<modifier name1>:<modifier name2>:...
 *   T:<trigger>:<keycode>:<shift-keycode>
 *
 */

 /*:::�ݒ�ύX�R�}���h�̉��ߕ�*/
errr process_pref_file_command(char *buf)
{
	int i, j, n1, n2;

	char *zz[16];


	/* Require "?:*" format */
	if (buf[1] != ':') return 1;


	switch (buf[0])
	{
	/* Mega-Hack -- read external player's history file */
	/* Process "H:<history>" */
	case 'H':
		add_history_from_pref_line(buf + 2);
		return 0;

	/* Process "R:<num>:<a>/<c>" -- attr/char for monster races */
	case 'R':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			monster_race *r_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_r_idx) return 1;
			r_ptr = &r_info[i];
			if (n1 || (!(n2 & 0x80) && n2)) r_ptr->x_attr = n1; /* Allow TERM_DARK text */
			if (n2) r_ptr->x_char = n2;
			return 0;
		}
		break;

	/* Process "K:<num>:<a>/<c>"  -- attr/char for object kinds */
	case 'K':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			object_kind *k_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_k_idx) return 1;
			k_ptr = &k_info[i];
			if (n1 || (!(n2 & 0x80) && n2)) k_ptr->x_attr = n1; /* Allow TERM_DARK text */
			if (n2) k_ptr->x_char = n2;
			return 0;
		}
		break;

	/* Process "F:<num>:<a>/<c>" -- attr/char for terrain features */
	/* "F:<num>:<a>/<c>" */
	/* "F:<num>:<a>/<c>:LIT" */
	/* "F:<num>:<a>/<c>:<la>/<lc>:<da>/<dc>" */
	case 'F':
		{
			feature_type *f_ptr;
			int num = tokenize(buf + 2, F_LIT_MAX * 2 + 1, zz, TOKENIZE_CHECKQUOTE);

			if ((num != 3) && (num != 4) && (num != F_LIT_MAX * 2 + 1)) return 1;
			else if ((num == 4) && !streq(zz[3], "LIT")) return 1;

			i = (huge)strtol(zz[0], NULL, 0);
			if (i >= max_f_idx) return 1;
			f_ptr = &f_info[i];

			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (n1 || (!(n2 & 0x80) && n2)) f_ptr->x_attr[F_LIT_STANDARD] = n1; /* Allow TERM_DARK text */
			if (n2) f_ptr->x_char[F_LIT_STANDARD] = n2;

			/* Mega-hack -- feat supports lighting */
			switch (num)
			{
			/* No lighting support */
			case 3:
				n1 = f_ptr->x_attr[F_LIT_STANDARD];
				n2 = f_ptr->x_char[F_LIT_STANDARD];
				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					f_ptr->x_attr[j] = n1;
					f_ptr->x_char[j] = n2;
				}
				break;

			/* Use default lighting */
			case 4:
				apply_default_feat_lighting(f_ptr->x_attr, f_ptr->x_char);
				break;

			/* Use desired lighting */
			case F_LIT_MAX * 2 + 1:
				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					n1 = strtol(zz[j * 2 + 1], NULL, 0);
					n2 = strtol(zz[j * 2 + 2], NULL, 0);
					if (n1 || (!(n2 & 0x80) && n2)) f_ptr->x_attr[j] = n1; /* Allow TERM_DARK text */
					if (n2) f_ptr->x_char[j] = n2;
				}
				break;
			}
		}
		return 0;

	/* Process "S:<num>:<a>/<c>" -- attr/char for special things */
	case 'S':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (byte)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			misc_to_attr[j] = n1;
			misc_to_char[j] = n2;
			return 0;
		}
		break;

	/* Process "U:<tv>:<a>/<c>" -- attr/char for unaware items */
	case 'U':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			for (i = 1; i < max_k_idx; i++)
			{
				object_kind *k_ptr = &k_info[i];
				if (k_ptr->tval == j)
				{
					if (n1) k_ptr->d_attr = n1;
					if (n2) k_ptr->d_char = n2;
				}
			}
			return 0;
		}
		break;

	/* Process "E:<tv>:<a>" -- attribute for inventory objects */
	case 'E':
		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) == 2)
		{
			j = (byte)strtol(zz[0], NULL, 0) % 128;
			n1 = strtol(zz[1], NULL, 0);
			if (n1) tval_to_attr[j] = n1;
			return 0;
		}
		break;

	/* Process "A:<str>" -- save an "action" for later */
	case 'A':
		text_to_ascii(macro__buf, buf+2);
		return 0;

	/* Process "P:<str>" -- normal macro */
	case 'P':
	{
		char tmp[1024];

		text_to_ascii(tmp, buf+2);
		macro_add(tmp, macro__buf);
		return 0;
	}

	/* Process "C:<str>" -- create keymap */
	case 'C':
	{
		int mode;
		char tmp[1024];

		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) != 2) return 1;

		mode = strtol(zz[0], NULL, 0);
		if ((mode < 0) || (mode >= KEYMAP_MODES)) return 1;

		text_to_ascii(tmp, zz[1]);
		if (!tmp[0] || tmp[1]) return 1;
		i = (byte)(tmp[0]);

		string_free(keymap_act[mode][i]);

		keymap_act[mode][i] = string_make(macro__buf);

		return 0;
	}

	/* Process "V:<num>:<kv>:<rv>:<gv>:<bv>" -- visual info */
	case 'V':
		if (tokenize(buf+2, 5, zz, TOKENIZE_CHECKQUOTE) == 5)
		{
			i = (byte)strtol(zz[0], NULL, 0);
			angband_color_table[i][0] = (byte)strtol(zz[1], NULL, 0);
			angband_color_table[i][1] = (byte)strtol(zz[2], NULL, 0);
			angband_color_table[i][2] = (byte)strtol(zz[3], NULL, 0);
			angband_color_table[i][3] = (byte)strtol(zz[4], NULL, 0);
			return 0;
		}
		break;

	/* Process "X:<str>" -- turn option off */
	/* Process "Y:<str>" -- turn option on */
	case 'X':
	case 'Y':
		for (i = 0; option_info[i].o_desc; i++)
		{
			if (option_info[i].o_var &&
			    option_info[i].o_text &&
			    streq(option_info[i].o_text, buf + 2))
			{
				int os = option_info[i].o_set;
				int ob = option_info[i].o_bit;

				if ((p_ptr->playing || character_xtra) &&
					(OPT_PAGE_BIRTH == option_info[i].o_page) && !p_ptr->wizard)
				{
#ifdef JP
					msg_format("�����I�v�V�����͕ύX�ł��܂���! '%s'", buf);
#else
					msg_format("Birth options can not changed! '%s'", buf);
#endif
					msg_print(NULL);
					return 0;
				}

				if (buf[0] == 'X')
				{
					/* Clear */
					option_flag[os] &= ~(1L << ob);
					(*option_info[i].o_var) = FALSE;
				}
				else
				{
					/* Set */
					option_flag[os] |= (1L << ob);
					(*option_info[i].o_var) = TRUE;
				}
				return 0;
			}
		}

		/* don't know that option. ignore it.*/
#ifdef JP
		msg_format("�I�v�V�����̖��O������������܂���F %s", buf);
#else
		msg_format("Ignored invalid option: %s", buf);
#endif
		msg_print(NULL);
		return 0;

	/* Process "Z:<type>:<str>" -- set spell color */
		/*:::���@�Ȃǂ̑������Ƃ̐F�ݒ蕔�� spell-xx.prf*/
	case 'Z':
	{
		/* Find the colon */
		char *t = my_strchr(buf + 2, ':');

		/* Oops */
		if (!t) return 1;

		/* Nuke the colon */
		*(t++) = '\0';

		for (i = 0; gf_desc[i].name; i++)
		{
			/* Match this type */
			if (streq(gf_desc[i].name, buf + 2))
			{
				/* Remember this color set */
				gf_color[gf_desc[i].num] = quark_add(t);

				/* Success */
				return 0;
			}
		}

		break;
	}

	/* Initialize macro trigger names and a template */
	/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
	/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
	case 'T':
	{
		int tok = tokenize(buf+2, 2+MAX_MACRO_MOD, zz, 0);

		/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
		if (tok >= 4)
		{
			int num;

			if (macro_template != NULL)
			{
				num = strlen(macro_modifier_chr);

				/* Kill the template string */
				string_free(macro_template);
				macro_template = NULL;

				/* Kill flag characters of modifier keys */
				string_free(macro_modifier_chr);

				/* Kill corresponding modifier names */
				for (i = 0; i < num; i++)
				{
					string_free(macro_modifier_name[i]);
				}

				/* Kill trigger name strings */
				for (i = 0; i < max_macrotrigger; i++)
				{
					string_free(macro_trigger_name[i]);
					string_free(macro_trigger_keycode[0][i]);
					string_free(macro_trigger_keycode[1][i]);
				}

				max_macrotrigger = 0;
			}

			if (*zz[0] == '\0') return 0; /* clear template */

			/* Number of modifier flags */
			num = strlen(zz[1]);

			/* Limit the number */
			num = MIN(MAX_MACRO_MOD, num);

			/* Stop if number of modifier is not correct */
			if (2 + num != tok) return 1;

			/* Get a template string */
			macro_template = string_make(zz[0]);

			/* Get flag characters of modifier keys */
			macro_modifier_chr = string_make(zz[1]);

			/* Get corresponding modifier names */
			for (i = 0; i < num; i++)
			{
				macro_modifier_name[i] = string_make(zz[2+i]);
			}
		}

		/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
		else if (tok >= 2)
		{
			char buf2[1024];
			int m;
			char *t, *s;
			if (max_macrotrigger >= MAX_MACRO_TRIG)
			{
#ifdef JP
				msg_print("�}�N���g���K�[�̐ݒ肪�������܂�!");
#else
				msg_print("Too many macro triggers!");
#endif
				return 1;
			}
			m = max_macrotrigger;
			max_macrotrigger++;

			/* Take into account the escape character  */
			t = buf2;
			s = zz[0];
			while (*s)
			{
				if ('\\' == *s) s++;
				*t++ = *s++;
			}
			*t = '\0';

			/* Get a trigger name */
			macro_trigger_name[m] = string_make(buf2);

			/* Get the corresponding key code */
			macro_trigger_keycode[0][m] = string_make(zz[1]);

			if (tok == 3)
			{
				/* Key code of a combination of it with the shift key */
				macro_trigger_keycode[1][m] = string_make(zz[2]);
			}
			else
			{
				macro_trigger_keycode[1][m] = string_make(zz[1]);
			}
		}

		/* No error */
		return 0;
	}
	}

	/* Failure */
	return 1;
}


/*
 * Helper function for "process_pref_file()"
 *
 * Input:
 *   v: output buffer array
 *   f: final character
 *
 * Output:
 *   result
 */
/*:::�ݒ�t�@�C���̍s��?:[������]�̉��߂�����@��������v������'0'�ȊO���Ԃ�͂�*/
cptr process_pref_file_expr(char **sp, char *fp)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';
	static char tmp[10];

	/* Initial */
	s = (*sp);

	/* Skip spaces */
	/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN	
	while (iswspace(*s)) s++;
#else
	while (isspace(*s)) s++;
#endif
	/* Save start */
	b = s;

	/* Default */
	v = "?o?o?";

	/* Analyze */
	if (*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_pref_file_expr(&s, &f);

		/* Oops */
		if (!*t)
		{
			/* Nothing */
		}

		/* Function: IOR */
		else if (streq(t, "IOR"))
		{
			v = "0";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if (streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if (streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		else if (streq(t, "EQU"))
		{
			v = "0";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = process_pref_file_expr(&s, &f);
				if (streq(t, p)) v = "1";
			}
		}

		/* Function: LEQ */
		else if (streq(t, "LEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && atoi(p) > atoi(t)) v = "0";
			}
		}

		/* Function: GEQ */
		else if (streq(t, "GEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);

				/* Compare two numbers instead of string */
				if (*t && atoi(p) < atoi(t)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
		}

		/* Verify ending */
		if (f != b2) v = "?x?x?";

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';
	}

	/* Other */
	else
	{
		/* Accept all printables except spaces and brackets */
#ifdef JP
		while (iskanji(*s) || (isprint(*s) && !my_strchr(" []", *s)))
		{
			if (iskanji(*s)) s++;
			s++;
		}
#else
		while (isprint(*s) && !my_strchr(" []", *s)) ++s;
#endif

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';

		/* Variable */
		if (*b == '$')
		{
			/* System */
			if (streq(b+1, "SYS"))
			{
				v = ANGBAND_SYS;
			}

			else if (streq(b+1, "KEYBOARD"))
			{
				v = ANGBAND_KEYBOARD;
			}

			/* Graphics */
			else if (streq(b+1, "GRAF"))
			{
				v = ANGBAND_GRAF;
			}

			/* Monochrome mode */
			else if (streq(b+1, "MONOCHROME"))
			{
				if (arg_monochrome)
					v = "ON";
				else
					v = "OFF";
			}

			/* Race */
			else if (streq(b+1, "RACE"))
			{
#ifdef JP
				v = rp_ptr->E_title;
#else
				v = rp_ptr->title;
#endif
			}

			/* Class */
			else if (streq(b+1, "CLASS"))
			{
#ifdef JP
				v = cp_ptr->E_title;
#else
				v = cp_ptr->title;
#endif
			}

			//v1.1.70 ���i�ǉ�
			else if (streq(b + 1, "PERSONALITY"))
			{
				v = ap_ptr->E_title;
			}

			/* Player */
			else if (streq(b+1, "PLAYER"))
			{
				static char tmp_player_name[32];
				char *pn, *tpn;
				for (pn = player_name, tpn = tmp_player_name; *pn; pn++, tpn++)
				{
#ifdef JP
					if (iskanji(*pn))
					{
						*(tpn++) = *(pn++);
						*tpn = *pn;
						continue;
					}
#endif
					*tpn = my_strchr(" []", *pn) ? '_' : *pn;
				}
				*tpn = '\0';
				v = tmp_player_name;
			}

			/* First realm */
			else if (streq(b+1, "REALM1"))
			{
#ifdef JP
				v = E_realm_names[p_ptr->realm1];
#else
				v = realm_names[p_ptr->realm1];
#endif
			}

			/* Second realm */
			else if (streq(b+1, "REALM2"))
			{
#ifdef JP
				v = E_realm_names[p_ptr->realm2];
#else
				v = realm_names[p_ptr->realm2];
#endif
			}

			/* Level */
			else if (streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%02d", p_ptr->lev);
				v = tmp;
			}

			/* Autopick auto-register is in-use or not? */
			else if (streq(b+1, "AUTOREGISTER"))
			{
				if (p_ptr->autopick_autoregister)
					v = "1";
				else
					v = "0";
			}
			else if (streq(b+1, "EXTRA_MODE"))
			{
				if (EXTRA_MODE)
					v = "ON";
				else
					v = "OFF";
			}
			else if (streq(b+1, "BERSERKER"))
			{
				if (p_ptr->pseikaku == SEIKAKU_BERSERK)
					v = "ON";
				else
					v = "OFF";
			}

			/* Money */
			else if (streq(b+1, "MONEY"))
			{
				sprintf(tmp, "%09ld", (long int)p_ptr->au);
				v = tmp;
			}
		}

		/* Constant */
		else
		{
			v = b;
		}
	}

	/* Save */
	(*fp) = f;

	/* Save */
	(*sp) = s;

	/* Result */
	return (v);
}


#define PREF_TYPE_NORMAL   0
#define PREF_TYPE_AUTOPICK 1
#define PREF_TYPE_HISTPREF 2

/*
 * Open the "user pref file" and parse it.
 */
/*:::�ݒ�t�@�C���̓ǂݍ��݂Ɖ���*/
static errr process_pref_file_aux(cptr name, int preftype)
{
	FILE *fp;

	char buf[1024];

	char old[1024];

	int line = -1;

	errr err = 0;

	bool bypass = FALSE;


	/* Open the file */
	fp = my_fopen(name, "r");

	/* No such file */
	if (!fp) return (-1);

	/* Process the file */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Count lines */
		line++;

		/* Skip "empty" lines */
		if (!buf[0]) continue;

		/* Skip "blank" lines */
#ifdef JP
		if (!iskanji(buf[0]))
#endif
	/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN	
		if (iswspace(buf[0])) continue;
#else
		if (isspace(buf[0])) continue;
#endif
		/* Skip comments */
		if (buf[0] == '#') continue;


		/* Save a copy */
		strcpy(old, buf);


		/* Process "?:<expr>" */
		if ((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_pref_file_expr(&s, &f);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			/* Continue */
			continue;
		}

		/* Apply conditionals */
		if (bypass) continue;


		/* Process "%:<file>" */
		if (buf[0] == '%')
		{
			static int depth_count = 0;

			/* Ignore if deeper than 20 level */
			if (depth_count > 20) continue;

			/* Count depth level */
			depth_count++;

  			/* Process that file if allowed */
			switch (preftype)
			{
			case PREF_TYPE_AUTOPICK:
				(void)process_autopick_file(buf + 2);
				break;
			case PREF_TYPE_HISTPREF:
				(void)process_histpref_file(buf + 2);
				break;
			default:
				(void)process_pref_file(buf + 2);
				break;
			}

			/* Set back depth level */
			depth_count--;

			/* Continue */
			continue;
		}


		/* Process the line */
		err = process_pref_file_command(buf);

		/* This is not original pref line... */
		if (err)
		{
			if (preftype != PREF_TYPE_AUTOPICK)
  				break;
			err = process_autopick_file_command(buf);
		}
	}


	/* Error */
	if (err)
	{
		/* Print error message */
		/* ToDo: Add better error messages */
#ifdef JP
	      msg_format("�t�@�C��'%s'��%d�s�ŃG���[�ԍ�%d�̃G���[�B", name, line, err);
	      msg_format("('%s'����͒�)", old);
#else
		msg_format("Error %d in line %d of file '%s'.", err, line, name);
		msg_format("Parsing '%s'", old);
#endif
		msg_print(NULL);
	}

	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (err);
}



/*
 * Process the "user pref file" with the given name
 *
 * See the functions above for a list of legal "commands".
 *
 * We also accept the special "?" and "%" directives, which
 * allow conditional evaluation and filename inclusion.
 */
/*:::*.prf�̐ݒ�t�@�C����ǂݍ���*/
errr process_pref_file(cptr name)
{
	char buf[1024];

	errr err1, err2;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_PREF, name);

	/* Process the system pref file */
	err1 = process_pref_file_aux(buf, PREF_TYPE_NORMAL);

	/* Stop at parser errors, but not at non-existing file */
	if (err1 > 0) return err1;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);
	
	/* Process the user pref file */
	err2 = process_pref_file_aux(buf, PREF_TYPE_NORMAL);


	/* User file does not exist, but read system pref file */
	if (err2 < 0 && !err1)
		return -2;

	/* Result of user file processing */
	return err2;
}



#ifdef CHECK_TIME

/*
 * Operating hours for ANGBAND (defaults to non-work hours)
 */
static char days[7][29] =
{
	"SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
	"MON:XXXXXXXX.........XXXXXXX",
	"TUE:XXXXXXXX.........XXXXXXX",
	"WED:XXXXXXXX.........XXXXXXX",
	"THU:XXXXXXXX.........XXXXXXX",
	"FRI:XXXXXXXX.........XXXXXXX",
	"SAT:XXXXXXXXXXXXXXXXXXXXXXXX"
};

/*
 * Restict usage (defaults to no restrictions)
 */
static bool check_time_flag = FALSE;

#endif


/*
 * Handle CHECK_TIME
 */
///sysdel
/*:::�A���O�o���h�̖又���Ŏg���Ă���炵���@�@�Ǝv������main.c������Ă΂�Ă�H�ǂ���ɂ��Ă��g���Ă��Ȃ���*/
errr check_time(void)
{

#ifdef CHECK_TIME

	time_t      c;
	struct tm   *tp;

	/* No restrictions */
	if (!check_time_flag) return (0);

	/* Check for time violation */
	c = time((time_t *)0);
	tp = localtime(&c);

	/* Violation */
	if (days[tp->tm_wday][tp->tm_hour + 4] != 'X') return (1);

#endif

	/* Success */
	return (0);
}



/*
 * Initialize CHECK_TIME
 */
errr check_time_init(void)
{

#ifdef CHECK_TIME

	FILE        *fp;

	char	buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "time.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Assume restrictions */
	check_time_flag = TRUE;

	/* Parse the file */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Chop the buffer */
		buf[29] = '\0';

		/* Extract the info */
		if (prefix(buf, "SUN:")) strcpy(days[0], buf);
		if (prefix(buf, "MON:")) strcpy(days[1], buf);
		if (prefix(buf, "TUE:")) strcpy(days[2], buf);
		if (prefix(buf, "WED:")) strcpy(days[3], buf);
		if (prefix(buf, "THU:")) strcpy(days[4], buf);
		if (prefix(buf, "FRI:")) strcpy(days[5], buf);
		if (prefix(buf, "SAT:")) strcpy(days[6], buf);
	}

	/* Close it */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}



#ifdef CHECK_LOAD

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif

typedef struct statstime statstime;

struct statstime
{
	int                 cp_time[4];
	int                 dk_xfer[4];
	unsigned int        v_pgpgin;
	unsigned int        v_pgpgout;
	unsigned int        v_pswpin;
	unsigned int        v_pswpout;
	unsigned int        v_intr;
	int                 if_ipackets;
	int                 if_ierrors;
	int                 if_opackets;
	int                 if_oerrors;
	int                 if_collisions;
	unsigned int        v_swtch;
	long                avenrun[3];
	struct timeval      boottime;
	struct timeval      curtime;
};

/*
 * Maximal load (if any).
 */
static int check_load_value = 0;

#endif


/*
 * Handle CHECK_LOAD
 */
/*:::�A���O�o���h�ւ̖又���ł̂ݎg���Ă���@�����Ă��܂����E�E�Ǝv������main.c()������g���Ă�H�ǂ���ɂ��Ă��g���Ă��Ȃ���*/
///sysdel
errr check_load(void)
{

#ifdef CHECK_LOAD

	struct statstime    st;

	/* Success if not checking */
	if (!check_load_value) return (0);

	/* Check the load */
	if (0 == rstat("localhost", &st))
	{
		long val1 = (long)(st.avenrun[2]);
		long val2 = (long)(check_load_value) * FSCALE;

		/* Check for violation */
		if (val1 >= val2) return (1);
	}

#endif

	/* Success */
	return (0);
}


/*
 * Initialize CHECK_LOAD
 */
errr check_load_init(void)
{

#ifdef CHECK_LOAD

	FILE        *fp;

	char	buf[1024];

	char	temphost[MAXHOSTNAMELEN+1];
	char	thishost[MAXHOSTNAMELEN+1];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "load.txt");

	/* Open the "load" file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Default load */
	check_load_value = 100;

	/* Get the host name */
	(void)gethostname(thishost, (sizeof thishost) - 1);

	/* Parse it */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		int value;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Parse, or ignore */
		if (sscanf(buf, "%s%d", temphost, &value) != 2) continue;

		/* Skip other hosts */
		if (!streq(temphost, thishost) &&
		    !streq(temphost, "localhost")) continue;

		/* Use that value */
		check_load_value = value;

		/* Done */
		break;
	}

	/* Close the file */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}


#define ENTRY_BARE_HAND 0
#define ENTRY_TWO_HANDS 1
#define ENTRY_RIGHT_HAND1 2
#define ENTRY_LEFT_HAND1 3
#define ENTRY_LEFT_HAND2 4
#define ENTRY_RIGHT_HAND2 5
#define ENTRY_POSTURE 6
#define ENTRY_SHOOT_HIT_DAM 7
#define ENTRY_SHOOT_POWER 8
#define ENTRY_SPEED 9
#define ENTRY_BASE_AC 10
#define ENTRY_LEVEL 11
#define ENTRY_CUR_EXP 12
#define ENTRY_MAX_EXP 13
#define ENTRY_EXP_TO_ADV 14
#define ENTRY_GOLD 15
#define ENTRY_DAY 16
#define ENTRY_HP 17
#define ENTRY_SP 18
#define ENTRY_PLAY_TIME 19
#define ENTRY_SKILL_FIGHT 20
#define ENTRY_SKILL_SHOOT 21
#define ENTRY_SKILL_SAVING 22
#define ENTRY_SKILL_STEALTH 23
#define ENTRY_SKILL_PERCEP 24
#define ENTRY_SKILL_SEARCH 25
#define ENTRY_SKILL_DISARM 26
#define ENTRY_SKILL_DEVICE 27
#define ENTRY_BLOWS 28
#define ENTRY_SHOTS 29
#define ENTRY_AVG_DMG 30
#define ENTRY_INFRA 31

#define ENTRY_NAME 32
#define ENTRY_SEX 33
#define ENTRY_RACE 34
#define ENTRY_CLASS 35
#define ENTRY_REALM 36
#define ENTRY_PATRON 37
#define ENTRY_AGE 38
#define ENTRY_HEIGHT 39
#define ENTRY_WEIGHT 40
#define ENTRY_SOCIAL 41
#define ENTRY_ALIGN 42

#define ENTRY_EXP_ANDR 43
#define ENTRY_EXP_TO_ADV_ANDR 44
#define ENTRY_ALICE_DOLL_ATTACK 45
#define ENTRY_METAMORPHOSIS		46
#define ENTRY_MONSTER_ATTACK	47
#define ENTRY_GUN_CHANCE_RIGHT	48
#define ENTRY_GUN_CHANCE_BOTH	49
#define ENTRY_GUN_CHARGE_RIGHT	50
#define ENTRY_GUN_CHANCE_LEFT	51
#define ENTRY_GUN_CHARGE_LEFT	52
#define ENTRY_HECATIA_BODY		53
#define ENTRY_ANIMAL_GHOST_STRIFE	54


///sys �X�e�[�^�X��ʂ̍��ڂƂ��ꂼ��̕\���ʒu
static struct
{
	int col;
	int row;
	int len;
	///mod150208 �����x���o�Ă�̂�20��24�ɕς���
	char header[24];
} disp_player_line[]
#ifdef JP
= {
	{ 1, 10, 25, "�Ō��C��(�i��)"},
	{ 1, 10, 25, "�Ō��C��(����)"},
	{ 1, 10, 25, "�Ō��C��(�E��)"},
	{ 1, 10, 25, "�Ō��C��(����)"},
	{ 1, 11, 25, "�Ō��C��(����)"},
	{ 1, 11, 25, "�Ō��C��(�E��)"},
	{ 1, 11, 25, ""},
	{ 1, 15, 25, "�ˌ��U���C��"},
	{ 1, 16, 25, "�ˌ�����{��"},
	{ 1, 20, 25, "����"},
	{ 1, 19, 25, "�`�b"},
	{29, 13, 21, "���x��"},
	{29, 14, 21, "�o���l"},
	{29, 15, 21, "�ő�o��"},
	{29, 16, 21, "�����x��"},
	{29, 17, 21, "������"},
	{29, 19, 21, "���t"},
	{29, 10, 21, "�g�o"},
	{29, 11, 21, "�l�o"},
	{29, 20, 21, "�v���C����"},
	{53, 10, -1, "�Ō��U��  :"},
	{53, 11, -1, "�ˌ��U��  :"},
	{53, 12, -1, "���@�h��  :"},
	{53, 13, -1, "�B���s��  :"},
	{53, 15, -1, "�m�o      :"},
	{53, 16, -1, "�T��      :"},
	{53, 17, -1, "����      :"},
	{53, 18, -1, "���@����  :"},
	{ 1, 12, 25, "�Ō���"},
	{ 1, 17, 25, "�ˌ���"},
	{ 1, 13, 25, "���σ_���[�W"},
	{53, 20, -1, "�ԊO������:"},
//	{26,  1, -1, "���O  : "},
	{16,  1, -1, "���O  : "},
	{ 1,  3, -1, "����     : "},
	{ 1,  4, -1, "�푰     : "},
	{ 1,  5, -1, "�E��     : "},
	{ 1,  6, -1, "���@     : "},
	{ 1,  7, -1, "��얂�_ : "},
	{29,  3, 21, "�N��"},
	{29,  4, 21, "�g��"},
	{29,  5, 21, "�̏d"},
	//{29,  6, 21, "�Љ�I�n��"},
	//{29,  7, 21, "����"},
	{29,  6, 21, "��Փx"},
	{29,  7, 21, "�X�R�A"},
	{29, 14, 21, "�����x"},
	{29, 16, 21, "�����x��"},
	{ 1, 10, 25, "�Ō��C��(�l�`)"},
	{ 1,  8, -1, "�ϐg�@�@ : "},
	{ 1, 10, 25, "�Ō��C��(����)"},
	{ 1, 15, 25, "�e(�E��)������"},
	{ 1, 15, 25, "�e(����)������"},
	{ 1, 16, 25, "���U����"},
	{ 1, 17, 25, "�e(����)������"},
	{ 1, 18, 25, "���U����"},
	{ 1,  7, -1, "�́@�@�@ : "},
	{ 1,  7, -1, "�������� : " },
};
#else
= {
	{ 1, 10, 25, "Bare hand"},
	{ 1, 10, 25, "Two hands"},
	{ 1, 10, 25, "Right hand"},
	{ 1, 10, 25, "Left hand"},
	{ 1, 11, 25, "Left hand"},
	{ 1, 11, 25, "Right hand"},
	{ 1, 11, 25, "Posture"},
	{ 1, 15, 25, "Shooting"},
	{ 1, 16, 25, "Multiplier"},
	{ 1, 20, 25, "Speed"},
	{ 1, 19, 25, "AC"},
	{29, 13, 21, "Level"},
	{29, 14, 21, "Experience"},
	{29, 15, 21, "Max Exp"},
	{29, 16, 21, "Exp to Adv"},
	{29, 17, 21, "Gold"},
	{29, 19, 21, "Time"},
	{29, 10, 21, "Hit point"},
	{29, 11, 21, "SP (Mana)"},
	{29, 20, 21, "Play time"},
	{53, 10, -1, "Fighting   : "},
	{53, 11, -1, "Bows/Throw : "},
	{53, 12, -1, "SavingThrow: "},
	{53, 13, -1, "Stealth    : "},
	{53, 15, -1, "Perception : "},
	{53, 16, -1, "Searching  : "},
	{53, 17, -1, "Disarming  : "},
	{53, 18, -1, "MagicDevice: "},
	{ 1, 12, 25, "Blows/Round"},
	{ 1, 17, 25, "Shots/Round"},
	{ 1, 13, 25, "AverageDmg/Rnd"},
	{53, 20, -1, "Infra-Vision: "},
	{26,  1, -1, "Name  : "},
	{ 1,  3, -1, "Sex      : "},
	{ 1,  4, -1, "Race     : "},
	{ 1,  5, -1, "Class    : "},
	{ 1,  6, -1, "Magic    : "},
	{ 1,  7, -1, "Patron   : "},
	{29,  3, 21, "Age"},
	{29,  4, 21, "Height"},
	{29,  5, 21, "Weight"},
	{29,  6, 21, "Social Class"},
	{29,  7, 21, "Align"},
	{29, 14, 21, "Construction"},
	{29, 16, 21, "Const to Adv"},
};
#endif

static void display_player_one_line(int entry, cptr val, byte attr)
{
	char buf[40];

	int row = disp_player_line[entry].row;
	int col = disp_player_line[entry].col;
	int len = disp_player_line[entry].len;
	cptr head = disp_player_line[entry].header;

	int head_len = strlen(head);

	Term_putstr(col, row, -1, TERM_WHITE, head);

	if (!val)
		return;

	if (len > 0)
	{
		int val_len = len - head_len;
		sprintf(buf, "%*.*s", val_len, val_len, val);
		Term_putstr(col + head_len, row, -1, attr, buf);
	}
	else
	{
		Term_putstr(col + head_len, row, -1, attr, val);
	}

	return;
}

/*:::�U���͕\��*/
static void display_player_melee_bonus(int hand, int hand_entry)
{
	char buf[160];
	//int show_tohit = p_ptr->dis_to_h[hand];
	//v1.1.71 �U���������\���ɋߐڋZ�\�̒l�𔽉f
	int show_tohit = p_ptr->dis_to_h[hand] + p_ptr->skill_thn / BTH_PLUS_ADJ;
	int show_todam = p_ptr->dis_to_d[hand];
	object_type *o_ptr = &inventory[INVEN_RARM + hand];


	//�A���X�̕���n���x��K���Ɍv�Z���ĉ��Z
	if (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts)
	{
		int i,attack_doll_num = 0, skill_sum=0,to_h_sum=0,to_d_sum=0;
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			int blow_num_temp;
			o_ptr = &inven_add[i];
			if(!o_ptr->k_idx) continue;
			if(!object_is_melee_weapon(o_ptr)) continue;
			blow_num_temp = calc_doll_blow_num(i);
			attack_doll_num+= blow_num_temp;

			skill_sum += ref_skill_exp(o_ptr->tval) * blow_num_temp;

			if (object_is_known(o_ptr))
			{
				to_h_sum += o_ptr->to_h * blow_num_temp;
				to_d_sum += o_ptr->to_d * blow_num_temp;
			}

		}
		if(!attack_doll_num){msg_print("ERROR:�A���X�U���񐔌v�Z����������"); return;}
		show_tohit += (skill_sum/attack_doll_num - WEAPON_EXP_BEGINNER)/200 + to_h_sum / attack_doll_num;
		show_todam += to_d_sum / attack_doll_num;
	}
	else if(!p_ptr->do_martialarts)
	{
		/* Hack -- add in weapon info if known */
		if (object_is_known(o_ptr) && !check_invalidate_inventory(INVEN_RARM + hand)) show_tohit += o_ptr->to_h;
		if (object_is_known(o_ptr) && !check_invalidate_inventory(INVEN_RARM + hand)) show_todam += o_ptr->to_d;
	}

	/* Melee attacks */
	sprintf(buf, "(%+d,%+d)", show_tohit, show_todam);

	/* Dump the bonuses to hit/dam */
	//�A���X�̕���n���x��K���Ɍv�Z���ĉ��Z
	if(IS_METAMORPHOSIS)
		display_player_one_line(ENTRY_MONSTER_ATTACK, "(---,---)", TERM_L_BLUE);
	else if (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts)
		display_player_one_line(ENTRY_ALICE_DOLL_ATTACK, buf, TERM_L_BLUE);
	else if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
		display_player_one_line(ENTRY_BARE_HAND, buf, TERM_L_BLUE);
	else if (p_ptr->ryoute)
		display_player_one_line(ENTRY_TWO_HANDS, buf, TERM_L_BLUE);
	else
		display_player_one_line(hand_entry, buf, TERM_L_BLUE);
}


/*
 * Prints the following information on the screen.
 */
///sys C�R�}���h�ɂ�遗�̏��@1�y�[�W�ڒ��@�X�L����U���͂Ȃ�
///mod140308 �|�g�p�~�����֘A
///mod160508 �e�d�l�ύX�ɔ����ˌ��֌W�̕\�L�ύX
static void display_player_middle(void)
{
	char buf[160];

	/* Base skill */
	int show_tohit = p_ptr->dis_to_h_b;
	int show_todam = 0;
	bool bow1=FALSE,bow2=FALSE;
	/* Range weapon */
	//object_type *o_ptr = &inventory[INVEN_BOW];
	object_type *o_ptr;

	int tmul = 0;
	int e;
	int tester;
	bool flag_use_gun = FALSE;

	//v1.1.71 �ˌ��Z�\�𔽉f...���悤�Ƃ�������������Ă��B
	/*
	if (object_is_shooting_weapon(&inventory[INVEN_RARM]))
	{
		bow1 = TRUE;
		o_ptr = &inventory[INVEN_RARM];
	}
	if (object_is_shooting_weapon(&inventory[INVEN_LARM]))
	{
		bow2 = TRUE;
		o_ptr = &inventory[INVEN_LARM];
	}

	show_tohit += p_ptr->skill_thb / BTH_PLUS_ADJ;
	//�ˌ�����n���x�̒l�����f�B�ǂ��炩�̎�Ɏˌ�����������Ă���Ƃ��̂݁B
	//do_cmd_fire_aux()�Ɠ����v�Z�����L�q�B�ˌ��Z�\�l�͓����Z�\�l�Ƌ����Ȃ̂ŋZ�\�l�ɂ܂Ƃ߂邱�Ƃ͂ł��Ȃ��B
	if (bow1 && !bow2 || !bow1 && bow2)
	{
		if (o_ptr->tval == TV_CROSSBOW)
			show_tohit += ref_skill_exp(TV_CROSSBOW) / 400;
		else if (o_ptr->tval == TV_BOW)
			show_tohit += (ref_skill_exp(TV_BOW) - (WEAPON_EXP_MASTER / 2)) / 200;

	}
	*/

	if(CHECK_USE_GUN) flag_use_gun = TRUE;

	o_ptr =  &inventory[INVEN_PACK + shootable(&tester)];

	if (p_ptr->migite)
	{
		display_player_melee_bonus(0, left_hander ? ENTRY_LEFT_HAND1 : ENTRY_RIGHT_HAND1);
	}

	if (p_ptr->hidarite)
	{
		display_player_melee_bonus(1, left_hander ? ENTRY_RIGHT_HAND2: ENTRY_LEFT_HAND2);
	}
	///sysdel  �C�s�m�̍\���\��
	else if ((p_ptr->pclass == CLASS_MONK) && (empty_hands(TRUE) & EMPTY_HAND_RARM))
	{
		int i;
		if (p_ptr->special_defense & KAMAE_MASK)
		{
			for (i = 0; i < MAX_KAMAE; i++)
			{
				if ((p_ptr->special_defense >> i) & KAMAE_GENBU) break;
			}
			if (i < MAX_KAMAE)
#ifdef JP
				display_player_one_line(ENTRY_POSTURE, format("%s�̍\��", kamae_shurui[i].desc), TERM_YELLOW);
#else
				display_player_one_line(ENTRY_POSTURE, format("%s form", kamae_shurui[i].desc), TERM_YELLOW);
#endif
		}
		else
#ifdef JP
				display_player_one_line(ENTRY_POSTURE, "�\���Ȃ�", TERM_YELLOW);
#else
				display_player_one_line(ENTRY_POSTURE, "none", TERM_YELLOW);
#endif
	}

	/* Apply weapon bonuses */
	if (object_is_known(o_ptr)) show_tohit += o_ptr->to_h;
	if (object_is_known(o_ptr)) show_todam += o_ptr->to_d;

	if(flag_use_gun)
	{
		int chance;
		int timeout;

		if(inventory[INVEN_RARM].tval == TV_GUN)
		{
			int kyouten_card_num = 0;
			chance = calc_gun_fire_chance(INVEN_RARM);
			timeout = calc_gun_timeout(&inventory[INVEN_RARM]) * calc_gun_charge_mult(INVEN_RARM);

			//v1.1.87d �@�͌o�T�ɂ��[�U�u�[�X�g���f
			kyouten_card_num = count_ability_card(ABL_CARD_KYOUTEN);
			if (kyouten_card_num)
			{
				timeout = timeout * 100 / (100+CALC_ABL_KYOUTEN_RECHARGE_BONUS(kyouten_card_num));
			}

			//�e���莝��
			if(!inventory[INVEN_LARM].k_idx)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_BOTH,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_RIGHT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else if(left_hander)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_LEFT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_LEFT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else
			{
				display_player_one_line(ENTRY_GUN_CHANCE_RIGHT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_RIGHT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}

		}
		if(inventory[INVEN_LARM].tval == TV_GUN)
		{
			chance = calc_gun_fire_chance(INVEN_LARM);
			timeout = calc_gun_timeout(&inventory[INVEN_LARM]) * calc_gun_charge_mult(INVEN_LARM);
			//�e���莝��(�E��ɕ��퍶��ɏe���e����遨����O���A�ō��藼�莝���ɂ��Ȃ肤��͂��H)
			if(!inventory[INVEN_RARM].k_idx)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_BOTH,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_LEFT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else if(left_hander)
			{
				display_player_one_line(ENTRY_GUN_CHANCE_RIGHT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_RIGHT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
			else
			{
				display_player_one_line(ENTRY_GUN_CHANCE_LEFT,  format("(%d%%)", chance), TERM_L_BLUE);
				display_player_one_line(ENTRY_GUN_CHARGE_LEFT,  format("%d.%02dturn", timeout/100, timeout%100), TERM_L_BLUE);
			}
		}
	}
	else
	{
		///item �|�̖������ƃN���X�{�E�̖����{�[�i�X�@�X�e�[�^�X��ʗp
		///mod131227 skill tval �ˌ�������
		///item �����e�����������炱���Ŗ������v�Z�v
		if (o_ptr->tval == TV_CROSSBOW)
			show_tohit += ref_skill_exp(TV_CROSSBOW) / 400;
		else
			show_tohit += (ref_skill_exp(TV_BOW) - (WEAPON_EXP_MASTER / 2)) / 200;
		/* Range attacks */
		display_player_one_line(ENTRY_SHOOT_HIT_DAM, format("(%+d,%+d)", show_tohit, show_todam), TERM_L_BLUE);

		if(o_ptr->tval == TV_BOW || o_ptr->tval == TV_CROSSBOW)
		{
			tmul = bow_tmul(o_ptr->tval, o_ptr->sval);
			if (p_ptr->xtra_might) tmul++;
			tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		}

		/* shoot power */
		display_player_one_line(ENTRY_SHOOT_POWER, format("x%d.%02d", tmul/100, tmul%100), TERM_L_BLUE);

	}


	/* Dump the armor class */
	display_player_one_line(ENTRY_BASE_AC, format("[%d,%+d]", p_ptr->dis_ac, p_ptr->dis_to_a), TERM_L_BLUE);

	/* Dump speed */
	{
		int tmp_speed = 0;
		byte attr;
		int i;

		i = p_ptr->pspeed-110;

		/* Hack -- Visually "undo" the Search Mode Slowdown */
		if (p_ptr->action == ACTION_SEARCH) i += 10;

		if (i > 0)
		{
			if (!p_ptr->riding)
				attr = TERM_L_GREEN;
			else
				attr = TERM_GREEN;
		}
		else if (i == 0)
		{
			if (!p_ptr->riding)
				attr = TERM_L_BLUE;
			else
				attr = TERM_GREEN;
		}
		else
		{
			if (!p_ptr->riding)
				attr = TERM_L_UMBER;
			else
				attr = TERM_RED;
		}

		if (!p_ptr->riding || CLASS_RIDING_BACKDANCE)
		{
			if(p_ptr->pclass == CLASS_MARTIAL_ARTIST && p_ptr->tim_general[0]) tmp_speed += 20;
			else if (IS_FAST()) tmp_speed += 10;
			if (p_ptr->slow) tmp_speed -= 10;
			if (p_ptr->lightspeed) tmp_speed = 99;
		}
		else
		{
			if (MON_FAST(&m_list[p_ptr->riding])) tmp_speed += 10;
			if (MON_SLOW(&m_list[p_ptr->riding])) tmp_speed -= 10;
		}

		if (tmp_speed)
		{
			if (!p_ptr->riding || CLASS_RIDING_BACKDANCE)
				sprintf(buf, "(%+d%+d)", i-tmp_speed, tmp_speed);
			else
#ifdef JP
				sprintf(buf, "��n�� (%+d%+d)", i-tmp_speed, tmp_speed);
#else
				sprintf(buf, "Riding (%+d%+d)", i-tmp_speed, tmp_speed);
#endif

			if (tmp_speed > 0)
				attr = TERM_YELLOW;
			else
				attr = TERM_VIOLET;
		}
		else
		{
			if (!p_ptr->riding || CLASS_RIDING_BACKDANCE)
				sprintf(buf, "(%+d)", i);
			else
#ifdef JP
				sprintf(buf, "��n�� (%+d)", i);
#else
				sprintf(buf, "Riding (%+d)", i);
#endif
		}
	
		display_player_one_line(ENTRY_SPEED, buf, attr);
	}

	/* Dump character level */
	display_player_one_line(ENTRY_LEVEL, format("%d", p_ptr->lev), TERM_L_GREEN);

	/* Dump experience */
	if (p_ptr->prace == RACE_ANDROID) e = ENTRY_EXP_ANDR;
	else e = ENTRY_CUR_EXP;

	if (p_ptr->exp >= p_ptr->max_exp)
		display_player_one_line(e, format("%ld", p_ptr->exp), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", p_ptr->exp), TERM_YELLOW);

	/* Dump max experience */
	if (p_ptr->prace == RACE_ANDROID)
		/* Nothing */;
	else
		display_player_one_line(ENTRY_MAX_EXP, format("%ld", p_ptr->max_exp), TERM_L_GREEN);

	/* Dump exp to advance */
	if (p_ptr->prace == RACE_ANDROID) e = ENTRY_EXP_TO_ADV_ANDR;
	else e = ENTRY_EXP_TO_ADV;

	if (p_ptr->lev >= PY_MAX_LEVEL)
		display_player_one_line(e, "*****", TERM_L_GREEN);
	else if (p_ptr->prace == RACE_ANDROID)
		display_player_one_line(e, format("%ld", (s32b)(player_exp_a[p_ptr->lev - 1] * p_ptr->expfact / 100L)), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", (s32b)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L)), TERM_L_GREEN);

	/* Dump gold */
	display_player_one_line(ENTRY_GOLD, format("%ld", p_ptr->au), TERM_L_GREEN);

	/* Dump Day */
	{
		int day, hour, min;
		extract_day_hour_min(&day, &hour, &min);

#ifdef JP
		if (day < MAX_DAYS) sprintf(buf, "%d���� %2d:%02d", day, hour, min);
		else sprintf(buf, "*****���� %2d:%02d", hour, min);
#else
		if (day < MAX_DAYS) sprintf(buf, "Day %d %2d:%02d", day, hour, min);
		else sprintf(buf, "Day ***** %2d:%02d", hour, min);
#endif
	}
	display_player_one_line(ENTRY_DAY, buf, TERM_L_GREEN);

	if(p_ptr->pclass == CLASS_CLOWNPIECE)
	{
		if (p_ptr->magic_num1[0] < 10000) 
			display_player_one_line(ENTRY_HP, format("%5d", p_ptr->mhp), TERM_L_GREEN);
		else if (p_ptr->magic_num1[0] < 20000) 
			display_player_one_line(ENTRY_HP, format("%5d", p_ptr->mhp), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_HP, format("%5d", p_ptr->mhp), TERM_RED);
	}
	else
	{
		/* Dump hit point */
		if (p_ptr->chp >= p_ptr->mhp) 
			display_player_one_line(ENTRY_HP, format("%4d/%4d", p_ptr->chp , p_ptr->mhp), TERM_L_GREEN);
		else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10) 
			display_player_one_line(ENTRY_HP, format("%4d/%4d", p_ptr->chp , p_ptr->mhp), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_HP, format("%4d/%4d", p_ptr->chp , p_ptr->mhp), TERM_RED);
	}

	/* Dump mana power */
	if (p_ptr->csp >= p_ptr->msp) 
		display_player_one_line(ENTRY_SP, format("%4d/%4d", p_ptr->csp , p_ptr->msp), TERM_L_GREEN);
	else if (p_ptr->csp > (p_ptr->msp * mana_warn) / 10) 
		display_player_one_line(ENTRY_SP, format("%4d/%4d", p_ptr->csp , p_ptr->msp), TERM_YELLOW);
	else
		display_player_one_line(ENTRY_SP, format("%4d/%4d", p_ptr->csp , p_ptr->msp), TERM_RED);

	/* Dump play time */
	display_player_one_line(ENTRY_PLAY_TIME, format("%.2lu:%.2lu:%.2lu", playtime/(60*60), (playtime/60)%60, playtime%60), TERM_L_GREEN);
}


/*
 * Hack -- pass color info around this file
 */
static byte likert_color = TERM_WHITE;


/*
 * Returns a "rating" of x depending on y
 */
//v1.1.81 �{�Ƃ́u�Z�\�l�Ɏ��l��\������v�I�v�V������ǉ�
static cptr likert(int x, int y)
{
	static char dummy[40] = "";

	/* Paranoia */
	if (y <= 0) y = 1;

	/* Negative value */
	if (x < 0)
	{
		likert_color = TERM_L_DARK;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-�Œ�", x);
		else
			sprintf(dummy, "�Œ�");
		return dummy;
#else
		return "Very Bad";
#endif
	}

	/* Analyze the value */
	switch ((x / y))
	{
	case 0:
	case 1:
		likert_color = TERM_RED;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-����", x);
		else
			sprintf(dummy, "����");
		return dummy;
#else
		return "Bad";
#endif

	case 2:
		likert_color = TERM_L_RED;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-���", x);
		else
			sprintf(dummy, "���");
		return dummy;
#else
		return "Poor";
#endif

	case 3:
	case 4:
		likert_color = TERM_ORANGE;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-����", x);
		else
			sprintf(dummy, "����");
		return dummy;
#else
		return "Fair";
#endif

	case 5:
		likert_color = TERM_YELLOW;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-�ǂ�", x);
		else
			sprintf(dummy, "�ǂ�");
		return dummy;
#else
		return "Good";
#endif

	case 6:
		likert_color = TERM_YELLOW;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-��ϗǂ�", x);
		else
			sprintf(dummy, "��ϗǂ�");
		return dummy;
#else
		return "Very Good";
#endif

	case 7:
	case 8:
		likert_color = TERM_L_GREEN;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-��z", x);
		else
			sprintf(dummy, "��z");
		return dummy;
#else
		return "Excellent";
#endif

	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		likert_color = TERM_GREEN;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-���z", x);
		else
			sprintf(dummy, "���z");
		return dummy;
#else
		return "Superb";
#endif

	case 14:
	case 15:
	case 16:
	case 17:
		likert_color = TERM_BLUE;
#ifdef JP
		if (show_actual_value)
			sprintf(dummy, "%3d-�p�Y�I", x);
		else
			sprintf(dummy, "�p�Y�I");
		return dummy;
#else
		return "Heroic";
#endif

	default:
		likert_color = TERM_VIOLET;
#ifdef JP
		if(show_actual_value)
			sprintf(dummy, "%3d-�`���I[%d]",x, (int)((((x / y) - 17) * 5) / 2));
		else
			sprintf(dummy, "�`���I[%d]", (int)((((x / y) - 17) * 5) / 2));
#else
		sprintf(dummy, "Legendary[%d]", (int)((((x / y) - 17) * 5) / 2));
#endif
		return dummy;
	}
}

///mod140223 �i���U���̈З�(100�������v)���T�Z����B��s�����Ƃ̍U���񐔂͊|�����Ă��Ȃ��B
//�{�Ƃł͐E��\�����Ƃ̐�p�e�[�u�����g���Ă����E�ƍU����i�̑����ŕs�\�ɂȂ�������py_attack_aux()�̌v�Z������100�񎎍s���č��v���邱�Ƃɂ����B
//C�R�}���h�����s���邽�тɒl������邪�d���Ȃ��B
//v1.1.65 display_player_various()���ɂ��������ʊ֐��ɕ���
int calc_martialarts_dam_x100(hand_idx)
{
	int sum=0;
	int num = 100;
	int count;

	if (!p_ptr->do_martialarts)
	{
		msg_print("ERROR:�i���łȂ��Ƃ���calc_martialarts_dam()���Ă΂ꂽ");
		return 0;
	}

	if (hand_idx == 0 && !p_ptr->migite || hand_idx == 1 && !p_ptr->hidarite)
		return 0;

	for (count = 0; count<num; count++)
	{
		const martial_arts_new *ma_ptr = &ma_blows_new[0], *old_ptr = &ma_blows_new[0];
		int times;
		int martial_arts_method = find_martial_arts_method(0);//�����ł�mode�͏��0
		int max_times = ref_skill_exp(SKILL_MARTIALARTS) / 2000 + 1; //�i���n���x�ɂ��A�U����ޑI�莎�s���B�U���񐔂ł͂Ȃ�
		for (times = 0; times < max_times; times++)
		{
			int min_level = 1;
			do
			{
				int mode_pick[50];
				int mode_cnt = 0;
				int j;
				for (j = 0; (ma_blows_new[j].method != 0 && mode_cnt < 50); j++)
					if (ma_blows_new[j].method == martial_arts_method)mode_pick[mode_cnt++] = j;

				if (!mode_cnt) return 0;//v1.1.92 ��p�i���U����ݒ肵�Y��ăL�������C�N��ʂŃN���b�V�������̂Œǉ�

				ma_ptr = &ma_blows_new[mode_pick[randint0(mode_cnt)]];
				min_level = ma_ptr->min_level;
			} while ((min_level > p_ptr->lev) || (randint1(p_ptr->lev) < ma_ptr->chance));

			if ((ma_ptr->min_level >= old_ptr->min_level))	old_ptr = ma_ptr;
			else ma_ptr = old_ptr;
		}
		if (p_ptr->pclass == CLASS_KISUME) sum += calc_kisume_bonus();
		sum += ((ma_ptr->dd + p_ptr->to_dd[hand_idx]) * (ma_ptr->ds + p_ptr->to_ds[hand_idx] + 1)) / 2;
	}

	sum += p_ptr->dis_to_d[hand_idx] * num;

	return sum;

}


/*
 * Prints ratings on certain abilities
 *
 * This code is "imitated" elsewhere to "dump" a character sheet.
 */
/*:::�X�e�[�^�X��ʁ@���̃X�L����ψٍU���Ȃ�*/
///sys ���̃X�L����ψٍU���ȂǕ\��
///mod140308 �|�g�p�~�����֘A
///mod140323 �ψيi���ȂǋL�q�ύX
static void display_player_various(void)
{
	int         tmp, damage[2], to_h[2], blows1, blows2, i, basedam;
	int			xthn, xthb, xfos, xsrh;
	int			xdis, xdev, xsav, xstl;
	cptr		desc;
	///mod140323 �ψيi���̍U���񐔂��������ǉ��i���S�ʂ̉񐔂Ƃ��Ďg�����Ƃɂ���
	int         muta_att = 0;
	u32b flgs[TR_FLAG_SIZE];
	int		shots, shot_frac,tester;
	bool dokubari;
	bool alice = (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts);
	bool flag_use_gun = FALSE;

	object_type		*o_ptr;

	if(CHECK_USE_GUN) flag_use_gun = TRUE;

	if((!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].tval == TV_SHIELD || !check_invalidate_inventory(INVEN_LARM) && inventory[INVEN_LARM].tval == TV_SHIELD) && ref_skill_exp(SKILL_SHIELD) > 3200) muta_att++;
	if (p_ptr->muta2 & MUT2_HARDHEAD || (buki_motteruka(INVEN_RARM) && ref_skill_exp(SKILL_MARTIALARTS) > 3200)) muta_att++;
	if (p_ptr->muta2 & MUT2_HORNS)     muta_att++;
	if (p_ptr->muta2 & MUT2_BIGHORN)      muta_att++;
	if (p_ptr->muta2 & MUT2_BIGTAIL) muta_att++;
	if (p_ptr->muta3 & MUT3_FISH_TAIL) muta_att++;
	if(inventory[INVEN_RARM].k_idx && inventory[INVEN_RARM].tval == TV_STICK && p_ptr->ryoute)
	{
		int num_xtra = 0;
		if(p_ptr->pclass == CLASS_MARTIAL_ARTIST) num_xtra++;
		if(ref_skill_exp(SKILL_MARTIALARTS) > 3000) num_xtra++;
		if(ref_skill_exp(SKILL_MARTIALARTS) > 4500) num_xtra++;
		if(ref_skill_exp(SKILL_MARTIALARTS) > 6000) num_xtra++;
		if(heavy_armor()) num_xtra /= 2;
		if(num_xtra) muta_att++;
	}
	xthn = p_ptr->skill_thn + (p_ptr->to_h_m * BTH_PLUS_ADJ);
	xthb = p_ptr->skill_thb + (p_ptr->to_h_b * BTH_PLUS_ADJ);

	///mod160508 �e�g�p���ɂ͎ˌ����x�p�����[�^��\�����Ȃ��Bdisplay_player_middle()�Ŕ�����������[�U���x�Ȃǂ�\������
	if(!flag_use_gun)
	{
		/* Shooting Skill (with current bow and normal missile) */
		//o_ptr = &inventory[INVEN_BOW];
		o_ptr = &inventory[INVEN_PACK + shootable(&tester)];
		///160508 ���̍��܂Œm��Ȃ������񂾂��ˌ��̋Z�\�l�\���ɂ͕���̖��������܂܂��炵���B�e�̕\�L�Ɛ����������ɂ������K���Ɍv�Z���悤
		xthb +=  (o_ptr->to_h * BTH_PLUS_ADJ);

		/* If the player is wielding one? */
		if (tester > SHOOT_UNSUITABLE && o_ptr->k_idx)
		{
			s16b energy_fire = bow_energy(o_ptr->tval, o_ptr->sval);

			/* Calculate shots per round */
			shots = p_ptr->num_fire * 100;
			shot_frac = (shots * 100 / energy_fire) % 100;
			shots = shots / energy_fire;
#if 0 
			//�A�[�`���[�̃N�����]���A�ˏ�����p�~����
			if (o_ptr->name1 == ART_CRIMSON)
			{
				shots = 1;
				shot_frac = 0;
				if (p_ptr->pclass == CLASS_ARCHER)
				{
					/* Extra shot at level 10 */
					if (p_ptr->lev >= 10) shots++;

					/* Extra shot at level 30 */
					if (p_ptr->lev >= 30) shots++;

					/* Extra shot at level 45 */
					if (p_ptr->lev >= 45) shots++;
				}
			}
#endif
		}
		else
		{
			shots = 0;
			shot_frac = 0;
		}
	}

	if(alice)//�A���X�אڍU���̓��ꏈ��
	{
		blows1 = 0;
		damage[0] = 0;
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			int tmp_blows = calc_doll_blow_num(i);
			if(!tmp_blows) continue;
			blows1 += tmp_blows;
			dokubari = FALSE;

			o_ptr = &inven_add[i];
			if((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) dokubari = TRUE;

			if(dokubari)
				damage[0] += 100; //���Ƃ�100�Ŋ���
			else
			{
				basedam = ((int)(o_ptr->dd) * (int)(o_ptr->ds + 1)) * 50;
				object_flags_known(o_ptr, flgs);

				if (have_flag(flgs, TR_EX_VORPAL))
					basedam = basedam * 5 / 3;
				else if (have_flag(flgs, TR_VORPAL))
					basedam = basedam * 11 / 9;
				basedam = calc_expect_crit(o_ptr->weight, o_ptr->to_h, basedam, p_ptr->dis_to_h[0], dokubari);

				damage[0] += basedam  * tmp_blows;
				damage[0] += p_ptr->dis_to_d[0] * tmp_blows * 100;

				if (object_is_known(o_ptr))
					damage[0] += o_ptr->to_d * tmp_blows * 100;
			}

		}


	}
	else if(IS_METAMORPHOSIS) //�����X�^�[�ϐg��
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];
		damage[0] = 0;
		blows1 = 0;
		for(i=0;i<4;i++)
		{
			if(!r_ptr->blow[i].method) continue;
			if(r_ptr->blow[i].method == RBM_SHOOT) continue;
			if(r_ptr->blow[i].effect == RBE_DR_MANA) continue;
			damage[0] += r_ptr->blow[i].d_dice * (r_ptr->blow[i].d_side+1) / 2;
			blows1++;
		}
	}
	else if (p_ptr->do_martialarts)//���ʂ̊i�� v1.1.65�ŕʊ֐��ɕ�������
	{
		for (i = 0; i < 2; i++)
		{
			damage[i] = calc_martialarts_dam_x100(i);
		}
		blows1 = p_ptr->migite ? p_ptr->num_blow[0] : 0;
		blows2 = p_ptr->hidarite ? p_ptr->num_blow[1] : 0;

	}
	else//����U��
	{
		for(i = 0; i < 2; i++)
		{
			damage[i] = p_ptr->dis_to_d[i] * 100;
			o_ptr = &inventory[INVEN_RARM + i];

			/* Average damage per round */

			if (o_ptr->k_idx)
			{
				to_h[i] = 0;
				dokubari = FALSE;
				int vorpal = 0; //v1.1.45 �l���m�̕�����ňꎞ�؂ꖡ�t�������Ƃ��X�e�[�^�X��ʂɔ��f
				///item �Őj
				///mod131223
				if((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) dokubari = TRUE;
				if (object_is_known(o_ptr))
				{ 
					damage[i] += o_ptr->to_d * 100;
					to_h[i] += o_ptr->to_h;
				}
				//basedam = ((o_ptr->dd + p_ptr->to_dd[i]) * (o_ptr->ds + p_ptr->to_ds[i] + 1)) * 50;
				basedam = ((int)(o_ptr->dd + p_ptr->to_dd[i]) * (int)(o_ptr->ds + p_ptr->to_ds[i] + 1)) * 50;
				object_flags_known(o_ptr, flgs);
								
				basedam = calc_expect_crit(o_ptr->weight, to_h[i], basedam, p_ptr->dis_to_h[i], dokubari);
				//if ((o_ptr->ident & IDENT_MENTAL) && ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD)))
				if (have_flag(flgs, TR_EX_VORPAL)) vorpal = 2;
				else if (have_flag(flgs, TR_VORPAL)) vorpal = 1;

				if (object_has_a_blade(o_ptr) && p_ptr->special_attack & ATTACK_VORPAL) vorpal++;

				if(vorpal >= 2)
				{
					/* vorpal blade */
					basedam *= 5;
					basedam /= 3;
				}
				else if(vorpal == 1)
				{
					/* vorpal flag only */
					basedam *= 11;
					basedam /= 9;
				}



				if ((p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && (have_flag(flgs, TR_FORCE_WEAPON) || p_ptr->special_attack & ATTACK_FORCE )&& (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
					basedam = basedam * 7 / 2;
			}
			else basedam = 0;

			damage[i] += basedam;

			if (!p_ptr->do_martialarts && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) damage[i] = 1;
			if (damage[i] < 0) damage[i] = 0;
		}
		blows1 = p_ptr->migite ? p_ptr->num_blow[0]: 0;
		blows2 = p_ptr->hidarite ? p_ptr->num_blow[1] : 0;
	}
	/* Basic abilities */

	xdis = p_ptr->skill_dis;
	xdev = p_ptr->skill_dev;
	xsav = p_ptr->skill_sav;
	xstl = p_ptr->skill_stl;
	xsrh = p_ptr->skill_srh;
	xfos = p_ptr->skill_fos;

	desc = likert(xthn, 12);
	display_player_one_line(ENTRY_SKILL_FIGHT, desc, likert_color);

	desc = likert(xthb, 12);
	display_player_one_line(ENTRY_SKILL_SHOOT, desc, likert_color);

	desc = likert(xsav, 7);
	display_player_one_line(ENTRY_SKILL_SAVING, desc, likert_color);

	/* Hack -- 0 is "minimum stealth value", so print "Very Bad" */
	desc = likert((xstl > 0) ? xstl : -1, 1);
	display_player_one_line(ENTRY_SKILL_STEALTH, desc, likert_color);

	/*:::*/
	desc = likert(xfos, 6);
	//display_player_one_line(ENTRY_SKILL_PERCEP, desc, likert_color);

	desc = likert(xsrh, 6);
	display_player_one_line(ENTRY_SKILL_SEARCH, desc, likert_color);

	desc = likert(xdis, 8);
	display_player_one_line(ENTRY_SKILL_DISARM, desc, likert_color);

	desc = likert(xdev, 6);
	display_player_one_line(ENTRY_SKILL_DEVICE, desc, likert_color);

	if(!flag_use_gun) 
		display_player_one_line(ENTRY_SHOTS, format("%d.%02d", shots, shot_frac), TERM_L_BLUE);

	if(alice)
	{
		if (!muta_att)
			display_player_one_line(ENTRY_BLOWS, format("%d", blows1), TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_BLOWS, format("%d+��", blows1), TERM_L_BLUE);

		if(!muta_att)
			desc = format("%d", damage[0] / 100);//�A���X��damage[0]�͍U���񐔕����v�ς�
		else 
			desc = format("%d+��", damage[0] / 100);

	}
	else if(IS_METAMORPHOSIS)
	{
		display_player_one_line(ENTRY_BLOWS, format("%d", blows1), TERM_L_BLUE);
		desc = format("%d", damage[0]);

	}
	else
	{
		//py_attack()�̒ǉ��U���s�����ƈ�v���邱��
		if((mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_EXATACK) 
		|| p_ptr->pclass == CLASS_3_FAIRIES ) muta_att = 0;

		if (!muta_att)
			display_player_one_line(ENTRY_BLOWS, format("%d+%d", blows1, blows2), TERM_L_BLUE);
		else	
			display_player_one_line(ENTRY_BLOWS, format("%d+%d+��", blows1, blows2), TERM_L_BLUE);

		if ((damage[0]+damage[1]) == 0)
			desc = "(�U���s��)";
		else if(!muta_att)
			desc = format("%d+%d", blows1 * damage[0] / 100, blows2 * damage[1] / 100);
		///mod140323 �ψٕ��ʂ�ǉ��i���̃_���[�W�͋ߎ��l�v�Z����̂��ʓ|�Ȃ̂�+���ł��܂������Ƃɂ���
		else 
			desc = format("%d+%d+��", blows1 * damage[0] / 100, blows2 * damage[1] / 100);
	}


	display_player_one_line(ENTRY_AVG_DMG, desc, TERM_L_BLUE);

	if(p_ptr->do_martialarts && p_ptr->pclass == CLASS_MEIRIN) 
	{
		int num = 100 / p_ptr->magic_num1[0];
		int num_flac = 10000 / p_ptr->magic_num1[0] % 100;
		Term_putstr(1, 11, -1, TERM_WHITE, "�U�����x");
		Term_putstr(21, 11, -1, TERM_L_BLUE, format("x%d.%02d",num,num_flac));
	}
	else if(p_ptr->pclass == CLASS_RAIKO && music_singing(MUSIC_NEW_RAIKO_PRISTINE))
	{
		int need = MAX((75 - p_ptr->concent * 5),25);
		int num = 100 / need;
		int num_flac = 10000 / need % 100;
		Term_putstr(1, 14, -1, TERM_WHITE, "�U�����x");
		Term_putstr(21, 14, -1, TERM_L_BLUE, format("x%d.%02d",num,num_flac));
	}

	display_player_one_line(ENTRY_INFRA, format("%d feet", p_ptr->see_infra * 10), TERM_WHITE);
}



/*
 * Obtain the "flags" for the player as if he was an item
 */
/*:::���̔\�͂��t���O�Ƃ��Ă܂Ƃ߂�@�X�e�[�^�X��ʂł̈ꗗ�\���p*/
static void player_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;
	int plev = p_ptr->lev;

	//�A���X�ƍ��~��炵���g��
	u32b doll_flgs[TR_FLAG_MAX];
	object_type *o_ptr;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	/* Classes */
	switch (p_ptr->pclass)
	{
	case CLASS_WARRIOR:
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 44) add_flag(flgs, TR_REGEN);
		break;

	case CLASS_PRIEST:
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 39)
		{
			if(is_good_realm(p_ptr->realm1)) add_flag(flgs, TR_RES_HOLY);
			else  add_flag(flgs, TR_RES_NETHER);
		}
		break;

	case CLASS_HIGH_MAGE:
		if(p_ptr->lev > 19 && p_ptr->realm1 == TV_BOOK_FORESEE) add_flag(flgs, TR_WARNING);
		if(p_ptr->lev > 19 && (p_ptr->realm1 == TV_BOOK_TRANSFORM || p_ptr->realm1 == TV_BOOK_LIFE)) add_flag(flgs, TR_REGEN);

		if(p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_OCCULT) add_flag(flgs, TR_RES_INSANITY);
		break;



	case CLASS_ARCHER:
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_BLIND);
		break;

	case CLASS_SAMURAI:
		if (p_ptr->lev > 29)
			add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_PALADIN:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_MAID:
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_DEX);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_CON);
		if (p_ptr->lev > 39)add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_ROGUE:
		if (p_ptr->lev > 29) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 39) add_flag(flgs, TR_THROW);
		break;
	case CLASS_RANGER:
		if (p_ptr->lev > 19) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 29) add_flag(flgs, TR_ESP_ANIMAL);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_POIS);
		break;
		/*
	case CLASS_CHAOS_WARRIOR:
		if (p_ptr->lev > 29)
			add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 39)
			add_flag(flgs, TR_RES_FEAR);
		break;
		*/
	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		if ((p_ptr->lev > 9) && !heavy_armor())
			add_flag(flgs, TR_SPEED);
		if ((p_ptr->lev>24) && !heavy_armor())
			add_flag(flgs, TR_FREE_ACT);
		break;
	case CLASS_NINJA:
		if (p_ptr->lev > 4) add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 14) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_DEX);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 34) add_flag(flgs, TR_THROW);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_DARK);
		break;
	case CLASS_MINDCRAFTER:
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SUST_WIS);
		if (p_ptr->lev > 24) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 39) add_flag(flgs, TR_TELEPATHY);
		break;
	case CLASS_BARD:
		add_flag(flgs, TR_RES_SOUND);
		break;
		/*
	case CLASS_BERSERKER:
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 39) add_flag(flgs, TR_REFLECT);
		break;
		*/
	case CLASS_MIRROR_MASTER:
		if(p_ptr->lev > 39)add_flag(flgs, TR_REFLECT);
		break;

	case CLASS_TSUKUMO_MASTER:
		if(p_ptr->lev > 9)add_flag(flgs, TR_SEE_INVIS);
		break;

	case CLASS_SYUGEN:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 44 && is_good_realm(p_ptr->realm1)) add_flag(flgs, TR_RES_HOLY);
		break;

	case CLASS_MAGIC_KNIGHT:
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_REGEN);
		break;

	case CLASS_RESEARCHER:
		if (p_ptr->lev > 14) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_BLIND);

		break;

	case CLASS_ENGINEER:
	case CLASS_NITORI:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_BLIND);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_SHARDS);
		break;
	case CLASS_JEWELER:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_BLIND);
		break;


	case CLASS_RUMIA:
		add_flag(flgs, TR_RES_DARK);
		break;
	case CLASS_YOUMU:
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
		break;
	case CLASS_TEWI:
		if (p_ptr->lev >  9) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_UDONGE:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev >  9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SOUND);
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 39) add_flag(flgs, TR_TELEPATHY);
		break;
	case CLASS_IKU:
		add_flag(flgs, TR_RES_ELEC);
		break;

	case CLASS_KOMACHI:
		if (p_ptr->lev > 19) 	add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 29) 	add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 39) 	add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_KOGASA:
		add_flag(flgs, TR_RES_WATER);
		break;


	case CLASS_KASEN:
		if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
		{
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_SEE_INVIS);
		}
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 19)add_flag(flgs, TR_SUST_STR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SUST_CON);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_SUIKA:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 19) 	add_flag(flgs, TR_RES_SOUND);
		if (p_ptr->lev > 39) 	add_flag(flgs, TR_RES_SHARDS);
		break;
	case CLASS_KOISHI:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_FREE_ACT);
		if(p_ptr->lev > 29) add_flag(flgs, TR_RES_CHAOS);
		break;
	case CLASS_MOMIZI:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_BLIND);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_SEIGA:
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_NETHER);
		break;
	case CLASS_CIRNO:
		add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);
		//v1.1.32
		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO))
		{
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_FEAR);
		}

		break;
	case CLASS_ORIN:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev >  9) add_flag(flgs, TR_ESP_UNDEAD);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_SHINMYOUMARU:
		add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
		break;
	case CLASS_NAZRIN:
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_LETTY:
		add_flag(flgs, TR_RES_COLD);
		break;
	case CLASS_PATCHOULI:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ACID);
		break;
	case CLASS_AYA:
		add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 9)  add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_BANKI:
		 add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_MYSTIA:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_FLAN:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_SHOU:
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_HOLY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_NETHER);
		break;

	case CLASS_YUYUKO:
		add_flag(flgs, TR_ESP_UNDEAD);
		break;
	case CLASS_SATORI:
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 24) add_flag(flgs, TR_TELEPATHY);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KYOUKO:
		add_flag(flgs, TR_RES_SOUND);
		if (p_ptr->lev > 39) add_flag(flgs, TR_REFLECT);
		break;

	case CLASS_TOZIKO:
		add_flag(flgs, TR_RES_ELEC);
		break;
	case CLASS_LILY_WHITE:
		add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KISUME:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_NETHER);
		break;
	case CLASS_HATATE:
		 add_flag(flgs, TR_SEE_INVIS);
		break;
	case CLASS_MIKO:
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 4) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 9) add_flag(flgs, TR_ESP_HUMAN);
		if (p_ptr->lev > 14) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 24) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 29) add_flag(flgs, TR_TELEPATHY);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_KOKORO:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_WRIGGLE:
		add_flag(flgs, TR_LITE);
		if (p_ptr->lev > 24) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_YUUKA:
		add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_CHEN:
		if(!p_ptr->magic_num1[0])
		{
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
		}
		break;
	case CLASS_MURASA:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		if(p_ptr->lev > 19) add_flag(flgs, TR_FREE_ACT);
		if(p_ptr->lev > 29) add_flag(flgs, TR_RES_POIS);
		if(p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KEINE:
		if(p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if(p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if(p_ptr->lev > 34) add_flag(flgs, TR_SUST_INT);
		if(p_ptr->lev > 34) add_flag(flgs, TR_SUST_WIS);
		break;
	case CLASS_YUGI:
		if(p_ptr->lev > 9)   add_flag(flgs, TR_RES_FIRE);
		if(p_ptr->lev > 14)  add_flag(flgs, TR_RES_COLD);
		if(p_ptr->lev > 19)  add_flag(flgs, TR_SUST_STR);
		if(p_ptr->lev > 29)  add_flag(flgs, TR_RES_LITE);

		break;
	case CLASS_REIMU:
		{
			int rank = osaisen_rank();
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_HOLY);
			if(rank > 2) add_flag(flgs, TR_SEE_INVIS);
			if(rank > 7) add_flag(flgs, TR_DEC_MANA);
			if(rank > 8)
			{
				add_flag(flgs, TR_RES_ACID);
				add_flag(flgs, TR_RES_FIRE); 
				add_flag(flgs, TR_RES_COLD); 
				add_flag(flgs, TR_RES_ELEC); 
				add_flag(flgs, TR_RES_POIS);
 			}
			if(plev > 4 && rank > 3) add_flag(flgs, TR_SLOW_DIGEST);
			if(plev > 4 && rank > 6) add_flag(flgs, TR_REGEN);
			if(plev > 9 && rank > 4) add_flag(flgs, TR_FREE_ACT);
			if(plev > 14 && rank > 3) add_flag(flgs, TR_RES_FEAR);
			if(plev > 19) add_flag(flgs, TR_ESP_KWAI);
			if(plev > 19 && rank > 4) add_flag(flgs, TR_RES_CONF);
			if(plev > 24 && rank > 1) add_flag(flgs, TR_ESP_UNDEAD);
			if(plev > 24 && rank > 5) add_flag(flgs, TR_RES_NETHER);
			if(plev > 29 && rank > 2) add_flag(flgs, TR_ESP_DEMON);
			if(plev > 34 && rank > 5) add_flag(flgs, TR_ESP_EVIL);
			if(plev > 34 && rank > 4) add_flag(flgs, TR_RES_LITE);
			if(plev > 39 && rank > 8) add_flag(flgs, TR_RES_TIME);
			if(plev > 39 && rank > 6) add_flag(flgs, TR_RES_CHAOS);
			if(plev > 44 && rank > 7) add_flag(flgs, TR_RES_INSANITY);
		}
		break;
	case CLASS_KAGEROU:
		if(p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if(p_ptr->lev > 39) add_flag(flgs, TR_SPEEDSTER);
		break;
	case CLASS_SANAE:
		if(p_ptr->lev > 9) add_flag(flgs, TR_ESP_DEITY);
		if(p_ptr->lev > 19) add_flag(flgs, TR_LEVITATION);
		if(p_ptr->lev > 39) add_flag(flgs, TR_RES_HOLY);
		break;
	case CLASS_REMY:
		add_flag(flgs, TR_LEVITATION);
		if(p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		break;
	case CLASS_BYAKUREN:
		add_flag(flgs, TR_RES_HOLY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if(p_ptr->tim_general[0]) //���@������
		{
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_LITE);
			if(p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
			if(p_ptr->lev > 4) add_flag(flgs, TR_RES_COLD);
			if(p_ptr->lev > 9) add_flag(flgs, TR_RES_ELEC);
			if(p_ptr->lev > 9) add_flag(flgs, TR_RES_ACID);
			if(p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
			if(p_ptr->lev > 24) add_flag(flgs, TR_SUST_STR);
			if(p_ptr->lev > 24) add_flag(flgs, TR_SUST_CON);
			if(p_ptr->lev > 24) add_flag(flgs, TR_SUST_DEX);
			if(p_ptr->lev > 29) add_flag(flgs, TR_RES_LITE);
			if(p_ptr->lev > 34) add_flag(flgs, TR_THROW);
			if(p_ptr->lev > 34) add_flag(flgs, TR_SPEEDSTER);
			if(p_ptr->lev > 39) add_flag(flgs, TR_RES_CHAOS);
			if(p_ptr->lev > 39) add_flag(flgs, TR_RES_DISEN);
			if(p_ptr->lev > 44) add_flag(flgs, TR_IM_FIRE);
		} 
		break;
	case CLASS_SEIJA:
		if(plev > 39) add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_UTSUHO:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_LITE);
			if(plev > 19) add_flag(flgs, TR_SH_FIRE);
			if(plev > 24) add_flag(flgs, TR_RES_BLIND);
			if(plev > 29) add_flag(flgs, TR_RES_DARK);
			if(plev > 34) add_flag(flgs, TR_IM_FIRE);
		break;

	case CLASS_YAMAME:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_TUNNEL);

		if(plev > 9) add_flag(flgs, TR_FREE_ACT);
		if(plev > 24) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 39) add_flag(flgs, TR_RES_DARK);
		break;

	case CLASS_MARTIAL_ARTIST:
		if(!heavy_armor() && plev > 9) 
			add_flag(flgs, TR_SPEED);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 29) add_flag(flgs, TR_SUST_STR);
		if(plev > 39) add_flag(flgs, TR_LEVITATION);
		break;

	case CLASS_SUWAKO:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_SEE_INVIS);
		if(plev > 9) add_flag(flgs, TR_RES_NETHER);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 29) add_flag(flgs, TR_RES_DARK);
		if(plev > 39) add_flag(flgs, TR_RES_CHAOS);
		break;
	case CLASS_RAN:
		if(plev > 4) add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_POIS);
		break;
	case CLASS_EIKI:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_ESP_UNIQUE);
		if(plev > 29) add_flag(flgs, TR_RES_CHAOS);
		if(plev > 39) add_flag(flgs, TR_TELEPATHY);
		break;

	case CLASS_MEIRIN:
		add_flag(flgs, TR_REGEN);
		if(plev > 4)add_flag(flgs, TR_RES_FIRE);
		if(plev > 14)add_flag(flgs, TR_SEE_INVIS);
		if(plev > 24)add_flag(flgs, TR_FREE_ACT);
		if(plev > 34)add_flag(flgs, TR_RES_LITE);

		break;
	case CLASS_PARSEE:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_ACID);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 34) add_flag(flgs, TR_RES_NETHER);
		break;
	case CLASS_SHINMYOU_2:
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_LEVITATION);
		if(prace_is_(RACE_KOBITO)) add_flag(flgs, TR_SPEEDSTER);
		if(plev > 9) add_flag(flgs, TR_SPEED);
		break;
	case CLASS_SUMIREKO:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 39) add_flag(flgs, TR_RES_TIME);
		break;
	case CLASS_ICHIRIN:
		add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_MOKOU:
		add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev >  9) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_LITE);
		break;
	case CLASS_KANAKO:
		{
			int rank = kanako_rank();
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_SEE_INVIS);
			if (p_ptr->lev >  9) add_flag(flgs, TR_RES_ELEC);
			if (p_ptr->lev > 14) add_flag(flgs, TR_FREE_ACT);
			if (p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
			if (rank > 2) add_flag(flgs, TR_RES_FEAR);
			if (rank > 4) add_flag(flgs, TR_REGEN);
			if (rank > 4) add_flag(flgs, TR_SLOW_DIGEST);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_HIZIRIN) add_flag(flgs, TR_RES_HOLY);;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_MIMIMI) add_flag(flgs, TR_RES_LITE);;
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_REIMU) add_flag(flgs, TR_RES_TIME);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_CTHULHU) add_flag(flgs, TR_RES_WATER);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_OBERON) add_flag(flgs, TR_RES_DISEN);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_AZAT) add_flag(flgs, TR_RES_INSANITY);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_MORGOTH) add_flag(flgs, TR_RES_DARK);
			if(p_ptr->magic_num2[0] & KANAKO_BEAT_0_J) add_flag(flgs, TR_RES_CHAOS);

			break;
		}
	case CLASS_FUTO:
		if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 24) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 34) add_flag(flgs, TR_WARNING);
		break;
	case CLASS_SUNNY:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->magic_num1[0])add_flag(flgs, TR_LITE);//v1.1.59
		if (p_ptr->magic_num1[0] >= SUNNY_CHARGE_SUNLIGHT_3) add_flag(flgs, TR_SH_FIRE);
		break;
	case CLASS_LUNAR:
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_STAR:
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		break;

	case CLASS_3_FAIRIES:
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_SOUND);
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
		{
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
		}
		if (p_ptr->magic_num1[0])add_flag(flgs, TR_LITE);//v1.1.59

		break;

	case CLASS_ALICE:
		//�A���X�̐l�`���������Ă��鏂
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			if(!inven_add[i].k_idx) continue;
			o_ptr = &inven_add[i];
			if(object_is_melee_weapon(o_ptr)) continue;
			object_flags(o_ptr, doll_flgs);
	
			if (have_flag(doll_flgs, TR_RES_ACID)) add_flag(flgs, TR_RES_ACID);
			if (have_flag(doll_flgs, TR_RES_ELEC)) add_flag(flgs, TR_RES_ELEC);
			if (have_flag(doll_flgs, TR_RES_FIRE)) add_flag(flgs, TR_RES_FIRE);
			if (have_flag(doll_flgs, TR_RES_COLD)) add_flag(flgs, TR_RES_COLD);
			if (have_flag(doll_flgs, TR_RES_POIS)) add_flag(flgs, TR_RES_POIS);
			if (have_flag(doll_flgs, TR_RES_LITE)) add_flag(flgs, TR_RES_LITE);
			if (have_flag(doll_flgs, TR_RES_DARK)) add_flag(flgs, TR_RES_DARK);
			if (have_flag(doll_flgs, TR_RES_SHARDS)) add_flag(flgs, TR_RES_SHARDS);
			if (have_flag(doll_flgs, TR_RES_WATER)) add_flag(flgs, TR_RES_WATER);
			if (have_flag(doll_flgs, TR_RES_HOLY)) add_flag(flgs, TR_RES_HOLY);
			if (have_flag(doll_flgs, TR_RES_SOUND)) add_flag(flgs, TR_RES_SOUND);
			if (have_flag(doll_flgs, TR_RES_NETHER)) add_flag(flgs, TR_RES_NETHER);
			if (have_flag(doll_flgs, TR_RES_CHAOS)) add_flag(flgs, TR_RES_CHAOS);
			if (have_flag(doll_flgs, TR_RES_DISEN)) add_flag(flgs, TR_RES_DISEN);
			if (have_flag(doll_flgs, TR_RES_TIME)) add_flag(flgs, TR_RES_TIME);

			if (have_flag(doll_flgs, TR_FREE_ACT)) add_flag(flgs, TR_FREE_ACT);
			if (have_flag(doll_flgs, TR_RES_BLIND)) add_flag(flgs, TR_RES_BLIND);
			if (have_flag(doll_flgs, TR_RES_FEAR)) add_flag(flgs, TR_RES_FEAR);
			if (have_flag(doll_flgs, TR_RES_CONF)) add_flag(flgs, TR_RES_CONF);
			if (have_flag(doll_flgs, TR_RES_INSANITY)) add_flag(flgs, TR_RES_INSANITY);

			if (have_flag(doll_flgs, TR_REFLECT)) add_flag(flgs, TR_REFLECT);
		}
		break;
	case CLASS_MARISA:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_POIS);
		if(plev > 6) add_flag(flgs, TR_SPEED);
		if(plev > 9) add_flag(flgs, TR_RES_FIRE);
		if(plev > 19) add_flag(flgs, TR_FREE_ACT);
		if(plev > 24) add_flag(flgs, TR_SPEEDSTER);
		if(plev > 29) add_flag(flgs, TR_RES_LITE);
		break;

	case CLASS_LUNASA:
		add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		if(plev > 34) add_flag(flgs, TR_WARNING);

		break;

	case CLASS_MERLIN:
		add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_FEAR);
		if(plev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;

	case CLASS_LYRICA:
		add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_BLIND);
		break;

	case CLASS_CLOWNPIECE:
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_NETHER);
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		if(plev > 8) add_flag(flgs, TR_SPEED);
		break;
	case CLASS_DOREMY:
		add_flag(flgs, TR_FREE_ACT);
		if(plev > 14) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 24) add_flag(flgs, TR_RES_CHAOS);
		if(plev > 39) add_flag(flgs, TR_RES_INSANITY);
		if(IN_DREAM_WORLD) add_flag(flgs, TR_TELEPATHY);
		break;

	case CLASS_YATSUHASHI:
	case CLASS_BENBEN:
		if(plev > 19) add_flag(flgs, TR_RES_SOUND);
		break;
	case CLASS_HINA:
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		if(plev > 24) add_flag(flgs, TR_RES_POIS);
		if(p_ptr->magic_num1[0] > 14999) add_flag(flgs, TR_RES_INSANITY);
		if(p_ptr->magic_num1[0] > 19999) add_flag(flgs, TR_REFLECT);
		break;

	case CLASS_SAKUYA:
		add_flag(flgs, TR_FREE_ACT);
		if(plev > 9) add_flag(flgs, TR_SLOW_DIGEST);
		if(plev > 14) add_flag(flgs, TR_RES_TIME);
		if(plev > 24) add_flag(flgs, TR_RES_FEAR);
		if(plev > 39) add_flag(flgs, TR_SPEEDSTER);
		if(plev > 7) add_flag(flgs, TR_SPEED);
		break;
	case CLASS_RAIKO:
		add_flag(flgs, TR_RES_ELEC);
		if(plev > 9) add_flag(flgs, TR_RES_SOUND);
		if(plev > 19) add_flag(flgs, TR_RES_CONF);
		if(plev > 34) add_flag(flgs, TR_LEVITATION);

		break;
	case CLASS_CHEMIST:
		if(plev > 14) add_flag(flgs, TR_FREE_ACT);
		if(plev > 29) add_flag(flgs, TR_RES_POIS);
		break;

	case CLASS_MAMIZOU:
		add_flag(flgs, TR_SEE_INVIS);
		if(plev > 14) add_flag(flgs, TR_FREE_ACT);
		if(plev > 24) add_flag(flgs, TR_RES_CONF);
		if(plev > 34) add_flag(flgs, TR_RES_CHAOS);
		if(plev > 39) add_flag(flgs, TR_TELEPATHY);
		break;

	case CLASS_YUKARI:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 29)  add_flag(flgs, TR_REFLECT);
		break;
	case CLASS_RINGO:
		if(plev > 9) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 19) add_flag(flgs, TR_WARNING);
		if(plev > 29) add_flag(flgs, TR_RES_CONF);
		if(plev > 39) add_flag(flgs, TR_SUST_WIS);
		if(plev > 39) add_flag(flgs, TR_SUST_INT);
		break;

	case CLASS_SEIRAN:
		if(plev > 14) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 44) add_flag(flgs, TR_RES_TIME);
		break;

	case CLASS_EIRIN:
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		if(plev > 14) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;
	case CLASS_KAGUYA:
		add_flag(flgs, TR_SUST_CHR);
		if(plev >  9) add_flag(flgs, TR_LITE);
		if(plev > 19) add_flag(flgs, TR_RES_DISEN);
		if(plev > 29) add_flag(flgs, TR_RES_TIME);
		if(plev > 39) add_flag(flgs, TR_RES_INSANITY);
		break;

	case CLASS_TOYOHIME:
		add_flag(flgs, TR_RES_WATER);
		if(plev > 19) add_flag(flgs, TR_RES_TIME);
		if(plev > 34) add_flag(flgs, TR_ESP_EVIL);
		break;

	case CLASS_YORIHIME:
		add_flag(flgs, TR_RES_WATER);
		if(plev > 9) add_flag(flgs, TR_RES_LITE);
		if(plev > 19) add_flag(flgs, TR_RES_DARK);
		if(plev > 29) add_flag(flgs, TR_RES_SHARDS);
		break;
	case CLASS_HECATIA:
		{
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_LITE);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_RES_FIRE);
			if(plev > 19) add_flag(flgs, TR_RES_CONF);
			if(plev > 29) add_flag(flgs, TR_RES_INSANITY);
			if(plev > 39) add_flag(flgs, TR_EASY_SPELL);

			if(hecatia_body_is_(HECATE_BODY_OTHER))
			{
				if(plev > 24) add_flag(flgs, TR_RES_CHAOS);
				if(plev > 44) add_flag(flgs, TR_RES_TIME);
			}
			if(hecatia_body_is_(HECATE_BODY_MOON))
			{
				if(plev > 24) add_flag(flgs, TR_RES_DARK);
				if(plev > 34) add_flag(flgs, TR_RES_LITE);
			}
			if(hecatia_body_is_(HECATE_BODY_EARTH))
			{
				if(plev > 24) add_flag(flgs, TR_RES_ACID);
				if(plev > 24) add_flag(flgs, TR_RES_COLD);
				if(plev > 24) add_flag(flgs, TR_RES_ELEC);
				if(plev > 34) add_flag(flgs, TR_RES_POIS);
			}
		}
		break;
	case CLASS_JUNKO:
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_FREE_ACT);
		break;

	case CLASS_SOLDIER:
		if(plev > 19) add_flag(flgs, TR_RES_FIRE);
		if(plev > 29) add_flag(flgs, TR_RES_FEAR);
		if(plev > 39) add_flag(flgs, TR_WARNING);

		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_HAWK_EYE))
			add_flag(flgs, TR_SEE_INVIS);
		break;

	case CLASS_NEMUNO:
		if(IS_NEMUNO_IN_SANCTUARY)
		{
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SPEED);

		}

		if(plev > 24) add_flag(flgs, TR_RES_FEAR);
		if(plev > 34) add_flag(flgs, TR_RES_WATER);
		break;
	case CLASS_AUNN:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_ESP_DEITY);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_HOLY);
		if(plev > 14) add_flag(flgs, TR_ESP_KWAI);
		if(plev > 14) add_flag(flgs, TR_ESP_DEMON);
		if(plev > 19) add_flag(flgs, TR_SEE_INVIS);
		if(plev > 24) add_flag(flgs, TR_RES_POIS);
		if(plev > 29) add_flag(flgs, TR_ESP_EVIL);
		if(plev > 34) add_flag(flgs, TR_RES_FEAR);

		break;
	case CLASS_NARUMI:
		add_flag(flgs, TR_RES_HOLY);
		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))	add_flag(flgs, TR_REGEN);
		break;

	case CLASS_LARVA:
		if (plev > 29) add_flag(flgs, TR_SUST_CON);
		if (plev > 39) add_flag(flgs, TR_RES_POIS);

		break;

	case CLASS_MAI:
	case CLASS_SATONO:
		if (plev > 9)	add_flag(flgs, TR_SUST_CHR);
		if (plev > 19)	add_flag(flgs, TR_RES_CONF);
		if (plev > 29)	add_flag(flgs, TR_RES_CHAOS);
		if (plev > 39)	add_flag(flgs, TR_RES_INSANITY);

		break;

	case CLASS_VFS_CLOWNPIECE:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_NETHER);
		if (plev > 19)	add_flag(flgs, TR_RES_INSANITY);
		break;

	case CLASS_JYOON:
		if (plev >  14) add_flag(flgs, TR_LITE);
		if (plev >  19) add_flag(flgs, TR_FREE_ACT);
		if (plev >  34) add_flag(flgs, TR_RES_LITE);
		break;

	case CLASS_SHION:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_LEVITATION);
		if (plev >  9) add_flag(flgs, TR_SEE_INVIS);
		if (plev >  19) add_flag(flgs, TR_RES_CONF);
		if (plev >  29) add_flag(flgs, TR_RES_CHAOS);
		break;

	case CLASS_OKINA:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		if (plev >  9) add_flag(flgs, TR_RES_CONF);
		if (plev >  19) add_flag(flgs, TR_RES_DARK);
		if (plev >  24) add_flag(flgs, TR_RES_ACID);
		if (plev >  24) add_flag(flgs, TR_RES_ELEC);
		if (plev >  24) add_flag(flgs, TR_RES_FIRE);
		if (plev >  24) add_flag(flgs, TR_RES_COLD);
		if (plev >  29) add_flag(flgs, TR_RES_CHAOS);
		if (plev >  39) add_flag(flgs, TR_RES_TIME);
		break;

	case CLASS_MAYUMI:
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_RES_FEAR);
		if (plev > 19) add_flag(flgs, TR_RES_CONF);
		if (plev > 29) add_flag(flgs, TR_RES_COLD);

		break;

	case CLASS_KUTAKA:
		add_flag(flgs, TR_WARNING);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_POIS);
		if (plev > 19)	add_flag(flgs, TR_RES_WATER);
		break;

	case CLASS_URUMI:
		add_flag(flgs, TR_RES_WATER);
		if (plev > 19)	add_flag(flgs, TR_RES_FEAR);
		break;

	case CLASS_SAKI:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
		if (plev>9) add_flag(flgs, TR_RES_ELEC);
		if (plev>19) add_flag(flgs, TR_ESP_UNDEAD);
		if (plev>19) add_flag(flgs, TR_SUST_STR);
		break;

	case CLASS_YACHIE:
		add_flag(flgs, TR_RES_WATER);
		if (plev  >14)add_flag(flgs, TR_RES_FIRE);
		if (plev  >19)add_flag(flgs, TR_RES_ELEC);
		if (plev  >24)add_flag(flgs, TR_RES_ACID);

		if (plev  >19)add_flag(flgs, TR_ESP_UNDEAD);
		if (plev  >29)add_flag(flgs, TR_RES_CONF);
		break;

	case CLASS_KEIKI:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_SUST_DEX);
		if (plev > 24)		add_flag(flgs, TR_ESP_NONLIVING);
		break;

	case CLASS_MIKE:
		if(plev > 29) add_flag(flgs, TR_RES_HOLY);

		break;
	case CLASS_TAKANE:

		if (plev > 24) add_flag(flgs, TR_RES_CONF);
		if (plev > 44) add_flag(flgs, TR_ESP_UNIQUE);
		break;

	case CLASS_SANNYO:

		if (plev > 19) add_flag(flgs, TR_RES_FEAR);
		if (plev > 29) add_flag(flgs, TR_RES_CONF);

		break;

	case CLASS_MOMOYO:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_TUNNEL);

		if(plev > 29)add_flag(flgs, TR_RES_NETHER);
		if (plev > 34)add_flag(flgs, TR_SUST_STR);
		if (plev > 34)add_flag(flgs, TR_SUST_DEX);
		if (plev > 34)add_flag(flgs, TR_SUST_CON);

		if (plev > 34)add_flag(flgs, TR_BRAND_POIS);

		if (plev > 23) add_flag(flgs, TR_BLOWS);

		break;
	case CLASS_TSUKASA:

		if (plev > 19) add_flag(flgs, TR_FREE_ACT);
		if (plev > 39) add_flag(flgs, TR_RES_FEAR);
		break;

	case CLASS_MEGUMU:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_FIRE);
		if (plev >  4) add_flag(flgs, TR_SPEED);
		if (plev >  9) add_flag(flgs, TR_SEE_INVIS);
		if (plev > 19) add_flag(flgs, TR_RES_CONF);
		if (plev > 29) add_flag(flgs, TR_RES_LITE);

		break;



	default:
		break; /* Do nothing */
	}





	/* Races */
	if (p_ptr->mimic_form && p_ptr->mimic_form != MIMIC_GIGANTIC)
	{
		switch(p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
			//add_flag(flgs, TR_HOLD_LIFE);
			add_flag(flgs, TR_RES_CHAOS);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			break;
		case MIMIC_DEMON_LORD:
			//add_flag(flgs, TR_HOLD_LIFE);
			add_flag(flgs, TR_RES_CHAOS);
			add_flag(flgs, TR_RES_NETHER);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_CONF);
			add_flag(flgs, TR_RES_DISEN);
			//add_flag(flgs, TR_RES_NEXUS);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_IM_FIRE);
			add_flag(flgs, TR_SH_FIRE);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_TELEPATHY);
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_SPEED);
			break;
		case MIMIC_VAMPIRE:
			//add_flag(flgs, TR_HOLD_LIFE);
			add_flag(flgs, TR_RES_DARK);
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_NETHER);
			if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			break;
		case MIMIC_BEAST:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_CONF);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
			add_flag(flgs, TR_WARNING);
			break;
		case MIMIC_SLIME:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_RES_CONF);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_RES_INSANITY);
			add_flag(flgs, TR_RES_BLIND);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_RES_SHARDS);
			add_flag(flgs, TR_RES_SOUND);
			add_flag(flgs, TR_RES_WATER);
			break;
		case MIMIC_MARISA:
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
			break;
		case MIMIC_MIST:
		{
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_RES_BLIND);
			add_flag(flgs, TR_RES_SHARDS);
			add_flag(flgs, TR_RES_SOUND);
			break;
		}
		case MIMIC_DRAGON:
		{
			int rank =p_ptr->mimic_dragon_rank;
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_SPEED);

			if(rank > 99) add_flag(flgs, TR_RES_POIS);
			if(rank > 99) add_flag(flgs, TR_LEVITATION);
			if(rank > 99) add_flag(flgs, TR_SEE_INVIS);

			if(rank > 102) add_flag(flgs, TR_RES_FEAR);
			if(rank > 104) add_flag(flgs, TR_REGEN);
			if(rank > 106) add_flag(flgs, TR_RES_CONF);

			if(rank > 110) add_flag(flgs, TR_RES_CHAOS);
			if(rank > 112) add_flag(flgs, TR_RES_WATER);
			if(rank > 114) add_flag(flgs, TR_RES_BLIND);

			if(rank > 116) add_flag(flgs, TR_SUST_STR);
			if(rank > 117) add_flag(flgs, TR_SUST_CON);
			if(rank > 118) add_flag(flgs, TR_SUST_DEX);
			if(rank > 119) add_flag(flgs, TR_RES_LITE);
			if(rank > 120) add_flag(flgs, TR_RES_DARK);

			if(rank > 124) add_flag(flgs, TR_TELEPATHY);
			if(rank > 126) add_flag(flgs, TR_RES_SHARDS);
			
			if(rank > 127) add_flag(flgs, TR_RES_SOUND);
			if(rank > 128) add_flag(flgs, TR_RES_NETHER);
	
			/*:::�ꎞ���������ł�rank�̌��E��130�܂�*/
			if(rank > 134) add_flag(flgs, TR_RES_DISEN);
			if(rank > 134) add_flag(flgs, TR_SUST_INT);
			if(rank > 134) add_flag(flgs, TR_SUST_WIS);
			if(rank > 134) add_flag(flgs, TR_SUST_CHR);

			if(rank >139) add_flag(flgs, TR_RES_TIME);
			if(rank >144) add_flag(flgs, TR_RES_HOLY);
			if(rank >144) add_flag(flgs, TR_SPEEDSTER);

			if(rank > 149) add_flag(flgs, TR_RES_INSANITY);
			break;
		}
		case MIMIC_CAT:
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_COLD);
			add_flag(flgs, TR_RES_ELEC);
			add_flag(flgs, TR_RES_ACID);
			add_flag(flgs, TR_RES_POIS);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_SPEEDSTER);
			add_flag(flgs, TR_WARNING);
			break;
		case MIMIC_NUE:
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_RES_DARK);
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_SEE_INVIS);
			add_flag(flgs, TR_RES_POIS);
			if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
			if (p_ptr->lev > 9) add_flag(flgs, TR_RES_COLD);
			if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ELEC);
			if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ACID);
			if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CHAOS);
			if (p_ptr->lev > 24) add_flag(flgs, TR_RES_NETHER);
			if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
			if (p_ptr->lev > 34) add_flag(flgs, TR_RES_FEAR);
			if (p_ptr->lev > 39) add_flag(flgs, TR_REGEN);
			if (p_ptr->lev > 39) add_flag(flgs, TR_SLOW_DIGEST);
			if (p_ptr->lev > 39) add_flag(flgs, TR_RES_INSANITY);
			break;


		case MIMIC_METAMORPHOSE_NORMAL:
		case MIMIC_METAMORPHOSE_MONST:
		case MIMIC_METAMORPHOSE_GIGANTIC:
			{
				monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];

				if(r_ptr->flags2 & RF2_REFLECTING) add_flag(flgs, TR_REFLECT);
				if(r_ptr->flags2 & RF2_REGENERATE) add_flag(flgs, TR_REGEN);
				if(r_ptr->flags2 & RF2_ELDRITCH_HORROR || r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON)) add_flag(flgs, TR_RES_INSANITY);
				if(r_ptr->flags2 & RF2_AURA_FIRE) add_flag(flgs, TR_LITE);

				if(r_ptr->flags3 & RF3_NO_FEAR) add_flag(flgs, TR_RES_FEAR);
				if(r_ptr->flags3 & RF3_NO_CONF) add_flag(flgs, TR_RES_CONF);

				if(r_ptr->flags7 & RF7_CAN_FLY) add_flag(flgs, TR_LEVITATION);

				if(r_ptr->flagsr & RFR_IM_ACID) add_flag(flgs, TR_RES_ACID);
				if(r_ptr->flagsr & RFR_IM_FIRE) add_flag(flgs, TR_RES_FIRE);
				if(r_ptr->flagsr & RFR_IM_COLD) add_flag(flgs, TR_RES_COLD);
				if(r_ptr->flagsr & RFR_IM_ELEC) add_flag(flgs, TR_RES_ELEC);
				if(r_ptr->flagsr & RFR_IM_POIS) add_flag(flgs, TR_RES_POIS);

				if(r_ptr->flagsr & RFR_RES_LITE) add_flag(flgs, TR_RES_LITE);
				if(r_ptr->flagsr & RFR_RES_DARK) add_flag(flgs, TR_RES_DARK);
				if(r_ptr->flagsr & RFR_RES_NETH) add_flag(flgs, TR_RES_NETHER);
				if(r_ptr->flagsr & RFR_RES_WATE) add_flag(flgs, TR_RES_WATER);
				if(r_ptr->flagsr & RFR_RES_SHAR) add_flag(flgs, TR_RES_SHARDS);
				if(r_ptr->flagsr & RFR_RES_SOUN) add_flag(flgs, TR_RES_SOUND);
				if(r_ptr->flagsr & RFR_RES_CHAO) add_flag(flgs, TR_RES_CHAOS);
				if(r_ptr->flagsr & RFR_RES_HOLY) add_flag(flgs, TR_RES_HOLY);
				if(r_ptr->flagsr & RFR_RES_DISE) add_flag(flgs, TR_RES_DISEN);
				if(r_ptr->flagsr & RFR_RES_TIME) add_flag(flgs, TR_RES_TIME);

				if(r_ptr->speed > 110) add_flag(flgs,TR_SPEED);
			}
			break;

		case MIMIC_KOSUZU_GHOST:
			{
				add_flag(flgs, TR_SEE_INVIS);
				add_flag(flgs, TR_RES_COLD);
				add_flag(flgs, TR_RES_NETHER);
				add_flag(flgs, TR_LEVITATION);
				add_flag(flgs, TR_RES_FEAR);
				add_flag(flgs, TR_ESP_HUMAN);
			}
			break;
		case MIMIC_KOSUZU_HYAKKI:
			{
				add_flag(flgs, TR_SEE_INVIS);
				add_flag(flgs, TR_RES_SHARDS);
				add_flag(flgs, TR_RES_DARK);
				add_flag(flgs, TR_RES_NETHER);
				add_flag(flgs, TR_RES_FEAR);
				add_flag(flgs, TR_RES_CONF);
				add_flag(flgs, TR_RES_BLIND);
				add_flag(flgs, TR_FREE_ACT);
				add_flag(flgs, TR_SPEED);
				if(plev > 20)
					add_flag(flgs, TR_BLOWS);
			}
			break;

		case MIMIC_GOD_OF_NEW_WORLD:
			add_flag(flgs, TR_LEVITATION);
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_SLOW_DIGEST);
			add_flag(flgs, TR_RES_HOLY);
			add_flag(flgs, TR_RES_FIRE);
			add_flag(flgs, TR_RES_LITE);
			add_flag(flgs, TR_RES_DARK);
			break;

			break;


/* GIGANTIC�͒ʏ�̎푰�ɏ����邱�Ƃɂ���
		case MIMIC_GIGANTIC:
			add_flag(flgs, TR_FREE_ACT);
			add_flag(flgs, TR_RES_FEAR);
			add_flag(flgs, TR_REGEN);
		//if (p_ptr->lev > 9) add_flag(flgs, TR_RES_DARK);
			add_flag(flgs, TR_SUST_STR);
			add_flag(flgs, TR_SUST_CON);
			add_flag(flgs, TR_RES_INSANITY);
			break;
*/

		}
	}
	else
	{
	switch (p_ptr->prace)
	{
	case RACE_HALF_YOUKAI:
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_POIS);
		break;

	case RACE_SENNIN:
		if(p_ptr->pclass != CLASS_KASEN) add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19 && p_ptr->pclass != CLASS_KASEN)
		{
			if( p_ptr->realm1>TV_BOOK_CHAOS || p_ptr->realm1 < TV_BOOK_TRANSFORM )  add_flag(flgs, TR_RES_HOLY);
			else if ( p_ptr->realm1==TV_BOOK_CHAOS) add_flag(flgs, TR_RES_CHAOS);
			else if ( p_ptr->realm1==TV_BOOK_TRANSFORM) add_flag(flgs, TR_RES_POIS);
			else if ( p_ptr->realm1==TV_BOOK_NECROMANCY) add_flag(flgs, TR_RES_NETHER);
			else if ( p_ptr->realm1==TV_BOOK_DARKNESS) add_flag(flgs, TR_RES_DARK);
		}

		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_WIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 39) add_flag(flgs, TR_LEVITATION);
		break;
	case RACE_TENNIN:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_ESP_UNDEAD);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 14)  add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19)  add_flag(flgs, TR_RES_LITE);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_WIS);
		break;

	case RACE_FAIRY:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_HOLY);
		break;
	case RACE_KAPPA:
		add_flag(flgs, TR_RES_WATER);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ACID);
		break;
	case RACE_MAGICIAN:
		if (p_ptr->lev > 9) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 15) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SUST_INT);
		break;

	case RACE_KARASU_TENGU:
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 6)	add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SPEEDSTER);
		break;
	case RACE_HAKUROU_TENGU:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_POIS);
		break;


	case RACE_ONI:
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 19)add_flag(flgs, TR_SUST_STR);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SUST_CON);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_INSANITY);
		break;
	case RACE_DEATH:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_NETHER);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 24) add_flag(flgs, TR_ESP_UNDEAD);
		if (p_ptr->lev > 24) add_flag(flgs, TR_ESP_HUMAN);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 44) add_flag(flgs, TR_RES_INSANITY);

		break;
	case RACE_WARBEAST:
		add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 29) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 14 && p_ptr->pclass != CLASS_AUNN) 
			add_flag(flgs, TR_SPEED);
		break;

	case RACE_TSUKUMO:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_BLIND);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		break;
	case RACE_ANDROID:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 9) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_FEAR);
		if (p_ptr->lev > 39)  add_flag(flgs, TR_RES_INSANITY);
		break;

	case RACE_GYOKUTO:
		if(p_ptr->pclass != CLASS_UDONGE) add_flag(flgs, TR_RES_HOLY);
		break;
	case RACE_YOUKO:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19) add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 39) add_flag(flgs, TR_TELEPATHY);
		if((p_ptr->realm1 < TV_BOOK_TRANSFORM  || p_ptr->realm1 > TV_BOOK_CHAOS) && p_ptr->lev > 44)add_flag(flgs, TR_RES_HOLY);
		break;
	case RACE_YAMAWARO:
		add_flag(flgs, TR_RES_WATER);
		break;

	case RACE_BAKEDANUKI:
		if (p_ptr->lev > 14) add_flag(flgs, TR_SEE_INVIS);
		break;
	case RACE_NINGYO:
		add_flag(flgs, TR_RES_WATER);
		break;

	case RACE_HOFGOBLIN:
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_POIS);
		break;
	case RACE_NYUDOU:
		if (p_ptr->lev > 9) add_flag(flgs, TR_FREE_ACT);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_SHARDS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_SOUND);
		break;
	case RACE_IMP:
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_NETHER);
		break;

	case RACE_GOLEM:
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 19) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 29)  add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 29)  add_flag(flgs, TR_RES_NETHER);
	break;




	case RACE_ZOMBIE:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 14) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_DARK);
		if (p_ptr->lev > 29) add_flag(flgs, TR_LEVITATION);
		break;

	case RACE_VAMPIRE:
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 34) add_flag(flgs, TR_LEVITATION);
		if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
		if (p_ptr->lev > 10) add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 39) add_flag(flgs, TR_SPEEDSTER);

		break;
	case RACE_SPECTRE:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		if (p_ptr->lev > 34)  add_flag(flgs, TR_TELEPATHY);
		break;
	case RACE_DAIYOUKAI:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 4) add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->lev > 9) add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 14) add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->lev > 19) add_flag(flgs, TR_RES_ACID);
		if (p_ptr->lev > 24) add_flag(flgs, TR_RES_POIS);
		if (p_ptr->lev > 29) add_flag(flgs, TR_RES_CHAOS);
		if (p_ptr->lev > 34) add_flag(flgs, TR_RES_FEAR);;
		if (p_ptr->lev > 39) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 39) add_flag(flgs, TR_SLOW_DIGEST);
		break;
	case RACE_STRAYGOD:
		{
			apply_deity_ability_flags(p_ptr->race_multipur_val[3],flgs);
			apply_deity_ability_flags(p_ptr->race_multipur_val[4]/128,flgs);
			apply_deity_ability_flags(p_ptr->race_multipur_val[4]%128,flgs);
		}
		break;
	case RACE_DEMIGOD:
		add_flag(flgs, TR_SEE_INVIS);
		if (p_ptr->lev > 24) add_flag(flgs, TR_REGEN);
		if (p_ptr->lev > 24) add_flag(flgs, TR_SLOW_DIGEST);
		break;

	case RACE_NINJA:
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_SPEEDSTER);
		if (p_ptr->lev > 6)	add_flag(flgs, TR_SPEED);
		break;

	case RACE_ULTIMATE:
		if (p_ptr->lev > 6)	add_flag(flgs, TR_SPEED);
		break;
	case RACE_LICH:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_TELEPATHY);
		break;

	case RACE_HOURAI:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SLOW_DIGEST);

		break;
	case RACE_LUNARIAN:
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_SUST_INT);
		if (p_ptr->lev > 9)	add_flag(flgs, TR_RES_CONF);
		if (p_ptr->lev > 19)	add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 29)	add_flag(flgs, TR_EASY_SPELL);
		if (p_ptr->lev > 39)	add_flag(flgs, TR_RES_TIME);
		break;
	case RACE_MAGIC_JIZO:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_HOLY);
		if(plev > 9) add_flag(flgs, TR_RES_FIRE);
		if(plev > 19) add_flag(flgs, TR_SUST_WIS);
		if(plev > 29) add_flag(flgs, TR_RES_CONF);
		if(plev > 39) add_flag(flgs, TR_RES_NETHER);
		break;

	case RACE_HALF_GHOST:
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SLOW_DIGEST);
		if (p_ptr->lev >  9)add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);
		break;

	case RACE_HANIWA:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_RES_INSANITY);
		if (p_ptr->lev > 19) add_flag(flgs, TR_SEE_INVIS);

		break;
	case RACE_ANIMAL_GHOST:
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_COLD);
		if (p_ptr->lev > 29)
			add_flag(flgs, TR_WARNING);
		if (p_ptr->lev > 14) add_flag(flgs, TR_SPEED);
		break;

	case RACE_ZASHIKIWARASHI:
		{
			add_flag(flgs, TR_WARNING);
			if (p_ptr->lev > 29) add_flag(flgs, TR_ESP_HUMAN);

			//�ړ����~�Ƃ��đ������Ă���i�̃p�����[�^�����̑ϐ��\�͂Ƃ��Ĕ��f����
			//����flgs�Ȃ̂ł��̂܂܃R�s�[����΂Ǝv�������ꕔ�g���Ȃ��p�����[�^������̂ł�߂Ƃ�
			//flgs�͂��łɎg���Ă�̂ŃA���X�̂Ƃ��Ɏg����doll_flgs�𗬗p

			o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

			if (object_is_artifact(o_ptr))
			{
				object_flags(o_ptr, doll_flgs);

				if (have_flag(doll_flgs, TR_SPEED)) add_flag(flgs, TR_SPEED);

				if (have_flag(doll_flgs, TR_ESP_EVIL)) add_flag(flgs, TR_ESP_EVIL);
				if (have_flag(doll_flgs, TR_ESP_GOOD))  add_flag(flgs, TR_ESP_GOOD);
				if (have_flag(doll_flgs, TR_ESP_ANIMAL))  add_flag(flgs, TR_ESP_ANIMAL);
				if (have_flag(doll_flgs, TR_ESP_HUMAN))  add_flag(flgs, TR_ESP_HUMAN);
				if (have_flag(doll_flgs, TR_ESP_UNDEAD))  add_flag(flgs, TR_ESP_UNDEAD);
				if (have_flag(doll_flgs, TR_ESP_DRAGON))  add_flag(flgs, TR_ESP_DRAGON);
				if (have_flag(doll_flgs, TR_ESP_DEITY))  add_flag(flgs, TR_ESP_DEITY);
				if (have_flag(doll_flgs, TR_ESP_DEMON))  add_flag(flgs, TR_ESP_DEMON);
				if (have_flag(doll_flgs, TR_ESP_KWAI))  add_flag(flgs, TR_ESP_KWAI);
				if (have_flag(doll_flgs, TR_ESP_NONLIVING))  add_flag(flgs, TR_ESP_NONLIVING);
				if (have_flag(doll_flgs, TR_ESP_UNIQUE))  add_flag(flgs, TR_ESP_UNIQUE);

				if (have_flag(doll_flgs, TR_SUST_STR)) add_flag(flgs, TR_SUST_STR);
				if (have_flag(doll_flgs, TR_SUST_INT)) add_flag(flgs, TR_SUST_INT);
				if (have_flag(doll_flgs, TR_SUST_WIS)) add_flag(flgs, TR_SUST_WIS);
				if (have_flag(doll_flgs, TR_SUST_DEX)) add_flag(flgs, TR_SUST_DEX);
				if (have_flag(doll_flgs, TR_SUST_CON)) add_flag(flgs, TR_SUST_CON);
				if (have_flag(doll_flgs, TR_SUST_CHR)) add_flag(flgs, TR_SUST_CHR);

				if (have_flag(doll_flgs, TR_EASY_SPELL)) add_flag(flgs, TR_EASY_SPELL);
				if (have_flag(doll_flgs, TR_DEC_MANA)) add_flag(flgs, TR_DEC_MANA);
				if (have_flag(doll_flgs, TR_REFLECT)) add_flag(flgs, TR_REFLECT);

				if (have_flag(doll_flgs, TR_RES_ACID)) add_flag(flgs, TR_RES_ACID);
				if (have_flag(doll_flgs, TR_RES_ELEC)) add_flag(flgs, TR_RES_ELEC);
				if (have_flag(doll_flgs, TR_RES_FIRE)) add_flag(flgs, TR_RES_FIRE);
				if (have_flag(doll_flgs, TR_RES_COLD)) add_flag(flgs, TR_RES_COLD);
				if (have_flag(doll_flgs, TR_RES_POIS)) add_flag(flgs, TR_RES_POIS);

				if (have_flag(doll_flgs, TR_RES_LITE)) add_flag(flgs, TR_RES_LITE);
				if (have_flag(doll_flgs, TR_RES_DARK)) add_flag(flgs, TR_RES_DARK);
				if (have_flag(doll_flgs, TR_RES_SHARDS)) add_flag(flgs, TR_RES_SHARDS);
				if (have_flag(doll_flgs, TR_RES_WATER)) add_flag(flgs, TR_RES_WATER);
				if (have_flag(doll_flgs, TR_RES_HOLY)) add_flag(flgs, TR_RES_HOLY);
				if (have_flag(doll_flgs, TR_RES_SOUND)) add_flag(flgs, TR_RES_SOUND);
				if (have_flag(doll_flgs, TR_RES_NETHER)) add_flag(flgs, TR_RES_NETHER);
				if (have_flag(doll_flgs, TR_RES_CHAOS)) add_flag(flgs, TR_RES_CHAOS);
				if (have_flag(doll_flgs, TR_RES_DISEN)) add_flag(flgs, TR_RES_DISEN);
				if (have_flag(doll_flgs, TR_RES_TIME)) add_flag(flgs, TR_RES_TIME);

				if (have_flag(doll_flgs, TR_FREE_ACT)) add_flag(flgs, TR_FREE_ACT);
				if (have_flag(doll_flgs, TR_RES_BLIND)) add_flag(flgs, TR_RES_BLIND);
				if (have_flag(doll_flgs, TR_RES_FEAR)) add_flag(flgs, TR_RES_FEAR);
				if (have_flag(doll_flgs, TR_RES_CONF)) add_flag(flgs, TR_RES_CONF);
				if (have_flag(doll_flgs, TR_RES_INSANITY)) add_flag(flgs, TR_RES_INSANITY);

				if (have_flag(doll_flgs, TR_LEVITATION)) add_flag(flgs, TR_LEVITATION);
				if (have_flag(doll_flgs, TR_SEE_INVIS)) add_flag(flgs, TR_SEE_INVIS);
				if (have_flag(doll_flgs, TR_SLOW_DIGEST)) add_flag(flgs, TR_SLOW_DIGEST);
				if (have_flag(doll_flgs, TR_REGEN)) add_flag(flgs, TR_REGEN);
				if (have_flag(doll_flgs, TR_LITE)) add_flag(flgs, TR_LITE);
				if (have_flag(doll_flgs, TR_SPEEDSTER)) add_flag(flgs, TR_SPEEDSTER);
				if (have_flag(doll_flgs, TR_TELEPATHY)) add_flag(flgs, TR_TELEPATHY);

				if (have_flag(doll_flgs, TR_SH_FIRE)) add_flag(flgs, TR_SH_FIRE);
				if (have_flag(doll_flgs, TR_SH_ELEC)) add_flag(flgs, TR_SH_ELEC);
				if (have_flag(doll_flgs, TR_SH_COLD)) add_flag(flgs, TR_SH_COLD);

			}

			else if (o_ptr->tval == TV_SOUVENIR)
			{
				switch (o_ptr->sval)
				{
					//�����̚�:�Ή��u���X
					//�Ηю��̒���:���C�̃u���X

				case SV_SOUVENIR_SHINMYOU_HOUSE: //�j���ۂ̉Ɓ@���f�ϐ�
					add_flag(flgs, TR_RES_ACID);
					add_flag(flgs, TR_RES_ELEC); 
					add_flag(flgs, TR_RES_FIRE); 
					add_flag(flgs, TR_RES_COLD);

					break;
				case SV_SOUVENIR_VAMPIRE_COFFIN: //�z���S�̊����@�Í��n���ϐ�
					add_flag(flgs, TR_RES_DARK);
					add_flag(flgs, TR_RES_NETHER);
					break;

					//���t�m�[���̋��@���ˁAAC100
				case SV_SOUVENIR_MIRROR_OF_RUFFNOR:
					add_flag(flgs, TR_REFLECT);
					break;

					//���̑����G�N�X�e �őϐ�
				case SV_SOUVENIR_KUTAKA:
					add_flag(flgs, TR_RES_POIS);
					break;

					//�X�L�[�Y�u���Y�j���@���̔\�͏㏸�E�j�בϐ�
				case SV_SOUVENIR_SKIDBLADNIR:
					add_flag(flgs, TR_RES_HOLY);
					add_flag(flgs, TR_RES_WATER);
					break;

					//�Ï��^�R���s���[�^�@�m�I�\�͏㏸
				case SV_SOUVENIR_MOON_COMPUTER:
					add_flag(flgs, TR_SUST_INT);
					add_flag(flgs, TR_SUST_WIS);
					add_flag(flgs, TR_RES_CONF);
					add_flag(flgs, TR_EASY_SPELL);
					break;

					//�L�k�̐�ԁ@�����A�ǔ���
				case SV_SOUVENIR_NODENS_CHARIOT:
					add_flag(flgs, TR_SPEED);
					break;

					//�Â̂��̂̐����@��C�I�[��,MP+100
				case SV_SOUVENIR_ELDER_THINGS_CRYSTAL:
					add_flag(flgs, TR_SH_COLD);
					break;

					//�����̎��̋󂫕r�@�\�͂Ȃ��@���C�V�����I�[�N����



				default:
					break;





				}


			}


		}

		break;

	//case RACE_ELF:
	//	add_flag(flgs, TR_RES_LITE);
	//	break;
	//case RACE_HOBBIT:
		//add_flag(flgs, TR_HOLD_LIFE);
		//break;
	//case RACE_GNOME:
		//add_flag(flgs, TR_FREE_ACT);
		//break;

	//case RACE_DWARF:
		//add_flag(flgs, TR_RES_BLIND);
		//break;
	//case RACE_HALF_ORC:
		//add_flag(flgs, TR_RES_DARK);
		//break;
	//case RACE_HALF_TROLL:
		//add_flag(flgs, TR_SUST_STR);
		//if (p_ptr->lev > 14)
		//{
		//	add_flag(flgs, TR_REGEN);
			//if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_BERSERKER))
			//{
				//add_flag(flgs, TR_SLOW_DIGEST);
				/*
				 * Let's not make Regeneration a disadvantage
				 * for the poor warriors who can never learn
				 * a spell that satisfies hunger (actually
				 * neither can rogues, but half-trolls are not
				 * supposed to play rogues)
				 */
		//	}
		//}
		//break;
	//case RACE_AMBERITE:
	//	add_flag(flgs, TR_SUST_CON);
	//	add_flag(flgs, TR_REGEN); /* Amberites heal fast */
	//	break;
	//case RACE_HIGH_ELF:
	//	add_flag(flgs, TR_RES_LITE);
	//	add_flag(flgs, TR_SEE_INVIS);
	//	break;
	//case RACE_BARBARIAN:
	//	add_flag(flgs, TR_RES_FEAR);
	//	break;
	//case RACE_HALF_OGRE:
		//add_flag(flgs, TR_SUST_STR);
	//	add_flag(flgs, TR_RES_DARK);
	//	break;
	//case RACE_HALF_GIANT:
	//	add_flag(flgs, TR_RES_SHARDS);
	//	add_flag(flgs, TR_SUST_STR);
	//	break;
	//case RACE_HALF_TITAN:
		//add_flag(flgs, TR_RES_CHAOS);
	//	break;
	//case RACE_CYCLOPS:
	//	add_flag(flgs, TR_RES_SOUND);
	//	break;
	//case RACE_YEEK:
	//	add_flag(flgs, TR_RES_ACID);
	//	if (p_ptr->lev > 19)
	//		add_flag(flgs, TR_IM_ACID);
	//	break;
	//case RACE_KLACKON:
	//	add_flag(flgs, TR_RES_CONF);
	//	add_flag(flgs, TR_RES_ACID);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_SPEED);
	//	break;
	//case RACE_KOBOLD:
	//	add_flag(flgs, TR_RES_POIS);
	//	break;
	//case RACE_NIBELUNG:
	//	add_flag(flgs, TR_RES_DISEN);
	//	add_flag(flgs, TR_RES_DARK);
	//	break;
	//case RACE_DARK_ELF:
	//	add_flag(flgs, TR_RES_DARK);
	//	if (p_ptr->lev > 19)
	//		add_flag(flgs, TR_SEE_INVIS);
	//	break;
	//case RACE_DRACONIAN:
	//	add_flag(flgs, TR_LEVITATION);
	//	if (p_ptr->lev > 4)
	//		add_flag(flgs, TR_RES_FIRE);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_RES_COLD);
	//	if (p_ptr->lev > 14)
	//		add_flag(flgs, TR_RES_ACID);
	//	if (p_ptr->lev > 19)
	//		add_flag(flgs, TR_RES_ELEC);
	//	if (p_ptr->lev > 34)
	//		add_flag(flgs, TR_RES_POIS);
	//	break;
	//case RACE_MIND_FLAYER:
	//	add_flag(flgs, TR_SUST_INT);
	//	add_flag(flgs, TR_SUST_WIS);
	//	if (p_ptr->lev > 14)
	//		add_flag(flgs, TR_SEE_INVIS);
	//	if (p_ptr->lev > 29)
	//		add_flag(flgs, TR_TELEPATHY);
	//	break;
	//case RACE_IMP:
	//	add_flag(flgs, TR_RES_FIRE);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_SEE_INVIS);
	//	break;
	//case RACE_GOLEM:
	//	add_flag(flgs, TR_SEE_INVIS);
	//	add_flag(flgs, TR_FREE_ACT);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
		//if (p_ptr->lev > 34)
			//add_flag(flgs, TR_HOLD_LIFE);
	//	break;
	//case RACE_SKELETON:
	//	add_flag(flgs, TR_SEE_INVIS);
	//	add_flag(flgs, TR_RES_SHARDS);
		//add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_POIS);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_RES_COLD);
	//	break;
	//case RACE_ZOMBIE:
	//	add_flag(flgs, TR_SEE_INVIS);
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_NETHER);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
	//	if (p_ptr->lev > 4)
	//		add_flag(flgs, TR_RES_COLD);
	//	break;
	//case RACE_VAMPIRE:
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_DARK);
	//	add_flag(flgs, TR_RES_NETHER);
	//	if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_RES_COLD);
	//	break;
	//case RACE_SPECTRE:
	//	add_flag(flgs, TR_LEVITATION);
	//	add_flag(flgs, TR_FREE_ACT);
	//	add_flag(flgs, TR_RES_COLD);
	//	add_flag(flgs, TR_SEE_INVIS);
		//add_flag(flgs, TR_HOLD_LIFE);
	//	add_flag(flgs, TR_RES_NETHER);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
		/* XXX pass_wall */
	//	if (p_ptr->lev > 34)
	//		add_flag(flgs, TR_TELEPATHY);
	//	break;
	//case RACE_SPRITE:
	//	add_flag(flgs, TR_RES_LITE);
	//	add_flag(flgs, TR_LEVITATION);
	//	if (p_ptr->lev > 9)
	//		add_flag(flgs, TR_SPEED);
	//	break;
//	case RACE_BEASTMAN:
//		add_flag(flgs, TR_RES_SOUND);
//		add_flag(flgs, TR_RES_CONF);
//		break;
	case RACE_ANGEL:
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_SEE_INVIS);
		break;
//	case RACE_DEMON:
	//	add_flag(flgs, TR_RES_FIRE);
		//add_flag(flgs, TR_RES_NETHER);
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	if (p_ptr->lev > 9)
		//	add_flag(flgs, TR_SEE_INVIS);
		//break;
		/*
	case RACE_DUNADAN:
		add_flag(flgs, TR_SUST_CON);
		break;
	case RACE_S_FAIRY:
		add_flag(flgs, TR_LEVITATION);
		break;
	case RACE_KUTAR:
		add_flag(flgs, TR_RES_CONF);
		break;
		*/
	//case RACE_ANDROID:
	//	add_flag(flgs, TR_FREE_ACT);
	//	add_flag(flgs, TR_RES_POIS);
	//	add_flag(flgs, TR_SLOW_DIGEST);
	//	add_flag(flgs, TR_HOLD_LIFE);
	//	break;
	default:
		; /* Do nothing */
	}
	}


	if( p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
		if (p_ptr->lev > 39) add_flag(flgs, TR_SPEEDSTER);
	}

	/* Mutations */
	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
			add_flag(flgs, TR_SEE_INVIS);
		}
		///mod141218�@�ǉ�
		if (p_ptr->muta3 & MUT3_BYDO)
		{
			add_flag(flgs, TR_SPEED);
		}

		if (p_ptr->muta3 & MUT3_SPEEDSTER)
		{
			add_flag(flgs, TR_SPEEDSTER);
		}
		if (p_ptr->muta3 & MUT3_WARNING)
		{
			add_flag(flgs, TR_WARNING);
		}
		if (p_ptr->muta3 & MUT3_RES_WATER)
		{
			add_flag(flgs, TR_RES_WATER);
		}
		if (p_ptr->muta3 & MUT3_FISH_TAIL)
		{
			add_flag(flgs, TR_RES_WATER);
		}

		if (p_ptr->muta3  & MUT3_ACID_SCALES)	add_flag(flgs, TR_RES_ACID);
		if (p_ptr->muta3  & MUT3_ELEC_SCALES)	add_flag(flgs, TR_RES_ELEC);
		if (p_ptr->muta3  & MUT3_FIRE_SCALES)	add_flag(flgs, TR_RES_FIRE);
		if (p_ptr->muta3  & MUT3_COLD_SCALES)	add_flag(flgs, TR_RES_COLD);
		if (p_ptr->muta3  & MUT3_POIS_SCALES)	add_flag(flgs, TR_RES_POIS);


		if (p_ptr->muta3 & MUT3_WINGS)
		{
			add_flag(flgs, TR_LEVITATION);
		}

		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			add_flag(flgs, TR_RES_FEAR);
		}


		if (p_ptr->muta3 & MUT3_ESP)
		{
			add_flag(flgs, TR_TELEPATHY);
		}

		if (p_ptr->muta3 & MUT3_MOTION)
		{
			add_flag(flgs, TR_FREE_ACT);
		}
	}

	if (p_ptr->muta4)
	{
		if (p_ptr->muta4  & MUT4_GHOST_JELLYFISH)	add_flag(flgs, TR_RES_WATER);
		if (p_ptr->muta4  & MUT4_GHOST_JELLYFISH)	add_flag(flgs, TR_RES_POIS);
		if (p_ptr->muta4  & MUT4_GHOST_JELLYFISH)	add_flag(flgs, TR_FREE_ACT);

		if (p_ptr->muta4  & MUT4_GHOST_HANGOKUOH)	add_flag(flgs, TR_RES_NETHER);

	}

	if((prace_is_(RACE_VAMPIRE) || prace_is_(RACE_WARBEAST)	|| p_ptr->mimic_form == MIMIC_VAMPIRE)	)
	{
		if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000 && (turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30))
		{
			add_flag(flgs, TR_SPEED);
		}

	}

	if (p_ptr->muta3 & MUT3_BYDO) add_flag(flgs, TR_AGGRAVATE);

	if(CHECK_FAIRY_TYPE == 46 && p_ptr->image) add_flag(flgs, TR_SPEED);


	switch(CHECK_FAIRY_TYPE)
	{
	case 11:
		if(p_ptr->chp < p_ptr->mhp/2) add_flag(flgs, TR_SPEEDSTER);
		break;
	default:
		break;
	}

	if (p_ptr->pseikaku == SEIKAKU_SEXY) add_flag(flgs, TR_AGGRAVATE);

	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
	//	add_flag(flgs, TR_HOLD_LIFE);
		if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
		if (p_ptr->lev > 9)
			add_flag(flgs, TR_SPEED);
	}
	if (p_ptr->special_defense & KATA_FUUJIN)
		add_flag(flgs, TR_REFLECT);
	if (p_ptr->special_defense & KAMAE_GENBU)
		add_flag(flgs, TR_REFLECT);
	if (p_ptr->special_defense & KAMAE_SUZAKU)
		add_flag(flgs, TR_LEVITATION);
	if (p_ptr->special_defense & KAMAE_SEIRYU)
	{
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_SH_FIRE);
		add_flag(flgs, TR_SH_ELEC);
		add_flag(flgs, TR_SH_COLD);
	}
	if (p_ptr->special_defense & KATA_MUSOU || p_ptr->prace == RACE_ULTIMATE)
	{
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_DISEN);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SH_FIRE);
		add_flag(flgs, TR_SH_ELEC);
		add_flag(flgs, TR_SH_COLD);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_LITE);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_SUST_CHR);
	}

	//v1.1.24 �}������N�G�X�g���͕��V�֎~
	if(QRKDR) 
	{
		remove_flag(flgs, TR_LEVITATION);
	}

}

/*:::���̈ꎞ�X�e�[�^�X�ɂ��\�́A�t���O�Ȃǂ��܂Ƃ߂�*/
///sys magic race �ꎞ�X�e�[�^�X�t���O���܂Ƃ߂�
static void tim_player_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	//v1.1.51 ���̐��E�ƐV�A���[�i�ł͋����I�ɕ��V�𓾂邱�Ƃɂ����B���V�Ȃ��ł͐V�A���[�i�̃}�b�v�ɂ���Ă͋l�ނ��Ƃ����邽�߁B
	if (IN_DREAM_WORLD) add_flag(flgs, TR_LEVITATION);

	if (p_ptr->foresight)
		add_flag(flgs, TR_WARNING);

	if (IS_HERO() || p_ptr->shero)
		add_flag(flgs, TR_RES_FEAR);
	if (p_ptr->tim_invis)
		add_flag(flgs, TR_SEE_INVIS);
	if (p_ptr->tim_regen)
		add_flag(flgs, TR_REGEN);
	if (IS_TIM_ESP())
		add_flag(flgs, TR_TELEPATHY);
	if (IS_FAST() || p_ptr->slow)
		add_flag(flgs, TR_SPEED);
	/*:::�C�[�N�̎_�Ɖu������Ƃ��͎_�̓�d�ϐ���\�L���Ȃ�*/
	//if (IS_OPPOSE_ACID() && !(p_ptr->special_defense & DEFENSE_ACID) && !(prace_is_(RACE_YEEK) && (p_ptr->lev > 19)))
	if (IS_OPPOSE_ACID() && !(p_ptr->special_defense & DEFENSE_ACID) )
		add_flag(flgs, TR_RES_ACID);
	if (IS_OPPOSE_ELEC() && !(p_ptr->special_defense & DEFENSE_ELEC))
		add_flag(flgs, TR_RES_ELEC);
	if (IS_OPPOSE_FIRE() && !(p_ptr->special_defense & DEFENSE_FIRE))
		add_flag(flgs, TR_RES_FIRE);
	if (IS_OPPOSE_COLD() && !(p_ptr->special_defense & DEFENSE_COLD))
		add_flag(flgs, TR_RES_COLD);
	if (IS_OPPOSE_POIS())
		add_flag(flgs, TR_RES_POIS);

	if (p_ptr->special_attack & ATTACK_DARK)
		add_flag(flgs, TR_SLAY_GOOD);
	if (p_ptr->special_attack & ATTACK_DARK && p_ptr->lev > 44)
		add_flag(flgs, TR_VAMPIRIC);
	if (p_ptr->special_attack & ATTACK_FORCE)
		add_flag(flgs, TR_FORCE_WEAPON);

	if (p_ptr->special_attack & ATTACK_ACID)
		add_flag(flgs, TR_BRAND_ACID);
	if (p_ptr->special_attack & ATTACK_ELEC)
		add_flag(flgs, TR_BRAND_ELEC);
	if (p_ptr->special_attack & ATTACK_FIRE)
		add_flag(flgs, TR_BRAND_FIRE);
	if (p_ptr->special_attack & ATTACK_COLD)
		add_flag(flgs, TR_BRAND_COLD);
	if (p_ptr->special_attack & ATTACK_POIS)
		add_flag(flgs, TR_BRAND_POIS);
	if (p_ptr->special_defense & DEFENSE_ACID)
		add_flag(flgs, TR_IM_ACID);
	if (p_ptr->special_defense & DEFENSE_ELEC)
		add_flag(flgs, TR_IM_ELEC);
	if (p_ptr->special_defense & DEFENSE_FIRE)
		add_flag(flgs, TR_IM_FIRE);
	if (p_ptr->special_defense & DEFENSE_COLD)
		add_flag(flgs, TR_IM_COLD);
	if (p_ptr->wraith_form)
		add_flag(flgs, TR_REFLECT);
	/* by henkma */
	if (p_ptr->tim_reflect || (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO2)))
		add_flag(flgs, TR_REFLECT);

	if (check_activated_nameless_arts(JKF1_SELF_LIGHT))
		add_flag(flgs, TR_LITE);

	if(p_ptr->pclass == CLASS_RUMIA && p_ptr->tim_general[0])
		add_flag(flgs, TR_RES_LITE);

	if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
		add_flag(flgs, TR_REFLECT);

	if (p_ptr->kamioroshi & KAMIOROSHI_SUMIYOSHI)
	{
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_SPEEDSTER);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_WARNING);
	}
	if (p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME)
	{
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_DARK);
	}
	if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO)
	{
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_BRAND_FIRE);
	}
	if (p_ptr->kamioroshi & KAMIOROSHI_IZUNA)
		add_flag(flgs, TR_SLAY_EVIL);

	if(p_ptr->nennbaku)
		add_flag(flgs, TR_NO_TELE);

	//v1.1.89 �S�X���h���S���C�[�^�[
	if(p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		add_flag(flgs, TR_SLAY_DRAGON);


	//�d������
	if(p_ptr->pclass == CLASS_KEINE && p_ptr->magic_num1[0] && !p_ptr->mimic_form)
	{
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_EASY_SPELL);
		add_flag(flgs, TR_WARNING);
		add_flag(flgs, TR_ESP_KWAI);
		add_flag(flgs, TR_ESP_DEMON);
		add_flag(flgs, TR_ESP_UNDEAD);
		add_flag(flgs, TR_ESP_ANIMAL);
		add_flag(flgs, TR_ESP_DEITY);
		add_flag(flgs, TR_ESP_DRAGON);
		add_flag(flgs, TR_ESP_UNIQUE);
		add_flag(flgs, TR_SPEED);
	}

	/*:::�n�`�ɂ��t���O����*/
	{
		feature_type *f_ptr = &f_info[cave[py][px].feat];
		if(p_ptr->prace == RACE_KAPPA && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs, TR_REGEN);
		}
		else if (p_ptr->prace == RACE_YAMAWARO && have_flag(f_ptr->flags, FF_TREE))
		{
			add_flag(flgs, TR_REGEN);
		}
		else if((p_ptr->prace == RACE_NINGYO) && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs,TR_SPEEDSTER);
			add_flag(flgs, TR_SPEED);
		}

		else if(p_ptr->pclass == CLASS_WAKASAGI && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs, TR_REGEN);
			add_flag(flgs, TR_FREE_ACT);
		}
		else if(p_ptr->pclass == CLASS_MURASA && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs,TR_SPEEDSTER);
			add_flag(flgs, TR_SPEED);
			add_flag(flgs, TR_REGEN);
		}
		else if(CHECK_FAIRY_TYPE == 32 && have_flag(f_ptr->flags, FF_WATER))
		{
			add_flag(flgs,TR_SPEEDSTER);
		}
		else if (p_ptr->pclass == CLASS_YAMAME && have_flag(f_ptr->flags, FF_SPIDER_NEST_1))
		{
			if (p_ptr->lev > 29) add_flag(flgs, TR_SPEEDSTER);
		}


	}


/*
	if (p_ptr->magicdef)
	{
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_LEVITATION);
	}
*/
	if (p_ptr->tim_res_nether || p_ptr->tim_sh_death)
	{
		add_flag(flgs, TR_RES_NETHER);
	}
	if (p_ptr->tim_res_insanity)
	{
		add_flag(flgs, TR_RES_INSANITY);
	}
	if (p_ptr->tim_res_water)
	{
		add_flag(flgs, TR_RES_WATER);
	}
	if (p_ptr->tim_res_chaos)
	{
		add_flag(flgs, TR_RES_CHAOS);
	}
	if (p_ptr->tim_res_dark)
	{
		add_flag(flgs, TR_RES_DARK);
	}
	if (p_ptr->tim_speedster)
	{
		add_flag(flgs, TR_SPEEDSTER);
	}

	///mod141218 �ǉ�
	if (p_ptr->lightspeed)
	{
		add_flag(flgs, TR_SPEED);
	}
	///mod150712
	if (p_ptr->tim_levitation)
	{
		add_flag(flgs, TR_LEVITATION);
	}

	if (p_ptr->tim_sh_fire)
	{
		add_flag(flgs, TR_SH_FIRE);
	}
	if (p_ptr->ult_res || SUPER_SHION)
	{

		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_FEAR);

		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_DARK);
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_RES_WATER);
		add_flag(flgs, TR_RES_CHAOS);
		add_flag(flgs, TR_RES_DISEN);
		add_flag(flgs, TR_RES_HOLY);
		add_flag(flgs, TR_RES_TIME);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_RES_INSANITY);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SH_FIRE);
		add_flag(flgs, TR_SH_ELEC);
		add_flag(flgs, TR_SH_COLD);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_LITE);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_SLOW_DIGEST);
		add_flag(flgs, TR_REGEN);
		add_flag(flgs, TR_SUST_STR);
		add_flag(flgs, TR_SUST_INT);
		add_flag(flgs, TR_SUST_WIS);
		add_flag(flgs, TR_SUST_DEX);
		add_flag(flgs, TR_SUST_CON);
		add_flag(flgs, TR_SUST_CHR);
	}

	/* Hex bonuses */
	/*
	if (p_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling(HEX_DEMON_AURA))
		{
			add_flag(flgs, TR_SH_FIRE);
			add_flag(flgs, TR_REGEN);
		}
		if (hex_spelling(HEX_ICE_ARMOR)) add_flag(flgs, TR_SH_COLD);
		if (hex_spelling(HEX_SHOCK_CLOAK)) add_flag(flgs, TR_SH_ELEC);
	}
	*/

	//v1.1.24 �}������N�G�X�g���͕��V�ƃe���|�֎~
	if(QRKDR) 
	{
		add_flag(flgs, TR_NO_TELE);
		remove_flag(flgs, TR_LEVITATION);
	}



}


/* Mode flags for displaying player flags */
#define DP_CURSE   0x01
#define DP_IMM     0x02
#define DP_WP      0x08


/*
 * Equippy chars
 */
/*:::�������ƃp�����[�^�̊Ԃ̑����i�V���{���ꗗ��\���@C�R�}���h�̃X�e�[�^�X��ʂł��g����*/
static void display_player_equippy(int y, int x, u16b mode)
{
	int i, max_i;

	byte a;
	char c;

	object_type *o_ptr;

	/* Weapon flags need only two column */
	if (mode & DP_WP) max_i = INVEN_LARM + 1;
	else max_i = INVEN_TOTAL;

	/* Dump equippy chars */
	for (i = INVEN_RARM; i < max_i; i++)
	{
		/* Object */
		o_ptr = &inventory[i];

		a = object_attr(o_ptr);
		c = object_char(o_ptr);

		/* Clear the part of the screen */
		if (!equippy_chars || !o_ptr->k_idx || check_invalidate_inventory(i))
		{
			c = ' ';
			a = TERM_DARK;
		}

		/* Dump */
		Term_putch(x + i - INVEN_RARM, y, a, c);
	}
}


void print_equippy(void)
{
	display_player_equippy(ROW_EQUIPPY, COL_EQUIPPY, 0);
}

/*
 *
 */
/*:::�����i�ɂ�錳�f�Ɖu�H*/
static void known_obj_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	/* Check equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		u32b o_flgs[TR_FLAG_SIZE];

		object_type *o_ptr;

		/* Object */
		o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;
		if(check_invalidate_inventory(i)) continue;

		/* Known flags */
		object_flags_known(o_ptr, o_flgs);

		if (have_flag(o_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
		if (have_flag(o_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
		if (have_flag(o_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
		if (have_flag(o_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);
	}

	//v1.1.46 �����̎w�ւ̏���
	if (p_ptr->pclass == CLASS_JYOON)
	{
		for (i = 0; i < INVEN_ADD_MAX; i++)
		{
			u32b o_flgs[TR_FLAG_SIZE];
			object_type *o_ptr;
			o_ptr = &inven_add[i];

			if (!o_ptr->k_idx) continue;
			if (check_invalidate_inventory(INVEN_RIGHT) || check_invalidate_inventory(INVEN_LEFT)) continue;

			object_flags_known(o_ptr, o_flgs);

			if (have_flag(o_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
			if (have_flag(o_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
			if (have_flag(o_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
			if (have_flag(o_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);

		}

	}


}

/*:::���̖Ɖu��\���@�X�e�[�^�X��ʗp*/
///race realm class �Ɖu��\��
static void player_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (is_hurt_holy() == -100)	add_flag(flgs, TR_RES_HOLY);
	if(immune_insanity()) add_flag(flgs, TR_RES_INSANITY);
	//���҂̐_,�C�_,���z�_,���_
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 61) add_flag(flgs, TR_RES_NETHER);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 44) add_flag(flgs, TR_RES_LITE);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 47) add_flag(flgs, TR_RES_WATER);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 55) add_flag(flgs, TR_RES_ELEC);
	if(prace_is_(RACE_STRAYGOD) && p_ptr->race_multipur_val[3] == 74) add_flag(flgs, TR_TELEPATHY);

	if(p_ptr->pclass == CLASS_WAKASAGI && p_ptr->lev > 19) add_flag(flgs, TR_RES_WATER);
	if(p_ptr->pclass == CLASS_MURASA) add_flag(flgs, TR_RES_WATER);

	if (prace_is_(RACE_SPECTRE))
		add_flag(flgs, TR_RES_NETHER);
	if (prace_is_(RACE_HANIWA))
		add_flag(flgs, TR_RES_NETHER);
	if (prace_is_(RACE_ANIMAL_GHOST))
		add_flag(flgs, TR_RES_NETHER);
	if (p_ptr->pclass == CLASS_YOSHIKA)
		add_flag(flgs, TR_RES_NETHER);
	if (p_ptr->mimic_form == MIMIC_VAMPIRE || prace_is_(RACE_VAMPIRE) || (p_ptr->pclass == CLASS_RUMIA && p_ptr->lev > 19) || p_ptr->tim_unite_darkness)
		add_flag(flgs, TR_RES_DARK);
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD)
		add_flag(flgs, TR_RES_FIRE);
	if (p_ptr->mimic_form == MIMIC_SLIME)
		add_flag(flgs, TR_RES_ACID);
	if (p_ptr->mimic_form == MIMIC_NUE)
		add_flag(flgs, TR_RES_DARK);

	if (p_ptr->pclass == CLASS_HECATIA)
		add_flag(flgs, TR_RES_NETHER);

	//v1.1.21 ���m
	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_DETONATOR))
		add_flag(flgs, TR_RES_SHARDS);



	if(p_ptr->prace == RACE_MAGICIAN) add_flag(flgs, TR_SLOW_DIGEST);

	//�A���X�̐l�`���������Ă��鏂
	if(p_ptr->pclass == CLASS_ALICE)
	{
		u32b doll_flgs[TR_FLAG_MAX];
		object_type *o_ptr;
		for(i=0;i<INVEN_DOLL_NUM_MAX;i++)
		{
			if(!inven_add[i].k_idx) continue;
			o_ptr = &inven_add[i];
			if(object_is_melee_weapon(o_ptr)) continue;

			object_flags(o_ptr, doll_flgs);

			if (have_flag(doll_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
			if (have_flag(doll_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
			if (have_flag(doll_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);
			if (have_flag(doll_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
		}
	}

	//v1.1.79 �푰���~��炵���ړ����~�Ƃ��đ������Ă���A�C�e��
	if (prace_is_(RACE_ZASHIKIWARASHI))
	{
		u32b doll_flgs[TR_FLAG_MAX];
		object_type *o_ptr;
		o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

		object_flags(o_ptr, doll_flgs);

		if (have_flag(doll_flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
		if (have_flag(doll_flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
		if (have_flag(doll_flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);
		if (have_flag(doll_flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);



	}


	if(p_ptr->pclass == CLASS_BYAKUREN && p_ptr->lev > 44 && p_ptr->tim_general[0])
	{
		add_flag(flgs, TR_RES_FIRE);
	}
	if (p_ptr->mimic_form == MIMIC_DRAGON)
	{
			int rank =p_ptr->mimic_dragon_rank;

			if(rank > 129) add_flag(flgs, TR_RES_FIRE);
			if(rank > 144) add_flag(flgs, TR_RES_ELEC);
			if(rank > 159) add_flag(flgs, TR_RES_ACID);
			if(rank > 159) add_flag(flgs, TR_RES_COLD);
			if(rank > 169) add_flag(flgs, TR_TELEPATHY);
	}
	if(p_ptr->pclass == CLASS_MOMIZI && p_ptr->lev > 34) add_flag(flgs, TR_TELEPATHY);

	if(p_ptr->pclass == CLASS_STAR && p_ptr->lev > 29) add_flag(flgs, TR_TELEPATHY);
	if(p_ptr->pclass == CLASS_3_FAIRIES && p_ptr->lev > 29) add_flag(flgs, TR_TELEPATHY);
	if(p_ptr->pclass == CLASS_REIMU && p_ptr->lev > 44 && osaisen_rank() > 8) add_flag(flgs, TR_TELEPATHY);

	if(p_ptr->pclass == CLASS_KANAKO && kanako_rank() > 6) add_flag(flgs, TR_RES_ELEC);

	if(p_ptr->pclass == CLASS_CIRNO && p_ptr->lev > 24) add_flag(flgs, TR_RES_COLD);
	if(p_ptr->pclass == CLASS_LETTY && p_ptr->lev > 39) add_flag(flgs, TR_RES_COLD);
	if(p_ptr->pclass == CLASS_MEDICINE) add_flag(flgs, TR_RES_POIS);
	if(p_ptr->pclass == CLASS_KYOUKO && p_ptr->lev > 29) add_flag(flgs, TR_RES_SOUND);

	if(p_ptr->pclass == CLASS_UTSUHO)
	{
		if(p_ptr->lev > 34)  add_flag(flgs, TR_RES_FIRE);
		if(p_ptr->lev > 44)  add_flag(flgs, TR_RES_LITE);

	}
	if(p_ptr->pclass == CLASS_HINA)
	{
		if(p_ptr->magic_num1[0] > 9999) add_flag(flgs, TR_RES_DARK);
		if(p_ptr->magic_num1[0] > 9999) add_flag(flgs, TR_RES_NETHER);
	}
	if(p_ptr->pclass == CLASS_EIRIN) add_flag(flgs, TR_RES_POIS);

	//v1.1.75 �Î��\�͂����Ƃ�������*���\������邱�Ƃɂ���

	if (p_ptr->pclass == CLASS_MYSTIA && p_ptr->lev > 39)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_NINJA)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_MOMIZI)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_NUE)add_flag(flgs, TR_LITE);
	if (p_ptr->pclass == CLASS_OKINA)add_flag(flgs, TR_LITE);
	if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET, SS_M_NOCTO_VISION))add_flag(flgs, TR_LITE);


	//else if (prace_is_(RACE_YEEK) && p_ptr->lev > 19)
	//	add_flag(flgs, TR_RES_ACID);
}

/*:::���̈ꎞ�Ɖu*/
static void tim_player_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (p_ptr->special_defense & DEFENSE_ACID)
		add_flag(flgs, TR_RES_ACID);
	if (p_ptr->special_defense & DEFENSE_ELEC)
		add_flag(flgs, TR_RES_ELEC);
	if (p_ptr->special_defense & DEFENSE_FIRE)
		add_flag(flgs, TR_RES_FIRE);
	if (p_ptr->special_defense & DEFENSE_COLD)
		add_flag(flgs, TR_RES_COLD);
	if (p_ptr->wraith_form)
		add_flag(flgs, TR_RES_DARK);
	if (p_ptr->kamioroshi & KAMIOROSHI_ISHIKORIDOME)
		add_flag(flgs, TR_RES_LITE);
	if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO)
		add_flag(flgs, TR_RES_FIRE);



	//v1.1.75 �Î��\�͂����Ƃ�������*���\������邱�Ƃɂ���

	if (p_ptr->tim_unite_darkness)add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_BEAST) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_SLIME) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_DRAGON) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_CAT) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_NORMAL) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_MONST) add_flag(flgs, TR_LITE);
	if (p_ptr->mimic_form == MIMIC_METAMORPHOSE_GIGANTIC) add_flag(flgs, TR_LITE);


}

/*:::���̎�_����*/
///racel class ��_�����t���O�̌v�Z
static void player_vuln_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	///mod140105 ��_�\�L������őS�Ă܂Ƃ߂���͂�
	if(is_hurt_holy() > 0) add_flag(flgs,TR_RES_HOLY);
	if(is_hurt_disen() > 0) add_flag(flgs,TR_RES_DISEN);
	if(is_hurt_lite() > 0) add_flag(flgs,TR_RES_LITE);
	if(is_hurt_water() > 0) add_flag(flgs,TR_RES_WATER);

	if(is_hurt_fire() > 0) add_flag(flgs,TR_RES_FIRE);
	if(is_hurt_elec() > 0) add_flag(flgs,TR_RES_ELEC);
	if(is_hurt_cold() > 0) add_flag(flgs,TR_RES_COLD);
	if(is_hurt_acid() > 0) add_flag(flgs,TR_RES_ACID);

	if(prace_is_(RACE_LUNARIAN))
	{
		add_flag(flgs,TR_RES_NETHER);
		add_flag(flgs,TR_RES_DARK);
	}

	if (prace_is_(RACE_HANIWA))
	{
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_SOUND);
	}

	if ((p_ptr->muta3 & MUT3_VULN_ELEM) || (p_ptr->special_defense & KATA_KOUKIJIN))
	{
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
	}
	//v1.1.24 �}������N�G�X�g���͕��V�֎~
	if(QRKDR) 
	{
		add_flag(flgs, TR_LEVITATION);
	}

	if (p_ptr->muta4 & MUT4_GHOST_HANIWA)
	{
		add_flag(flgs, TR_RES_SOUND);
		add_flag(flgs, TR_RES_SHARDS);
	}

	if(IS_VULN_FEAR)
		add_flag(flgs, TR_RES_FEAR);


	//if (prace_is_(RACE_ANDROID))
		//add_flag(flgs, TR_RES_ELEC);
	//if (prace_is_(RACE_ENT))
	//	add_flag(flgs, TR_RES_FIRE);
	//if (prace_is_(RACE_VAMPIRE) || prace_is_(RACE_S_FAIRY) ||
	//    (p_ptr->mimic_form == MIMIC_VAMPIRE))
	//	add_flag(flgs, TR_RES_LITE);
}


/*
 * A struct for storing misc. flags
 */
typedef struct {
	u32b player_flags[TR_FLAG_SIZE];
	u32b tim_player_flags[TR_FLAG_SIZE];
	u32b player_imm[TR_FLAG_SIZE];
	u32b tim_player_imm[TR_FLAG_SIZE];
	u32b player_vuln[TR_FLAG_SIZE];
	u32b known_obj_imm[TR_FLAG_SIZE];
} all_player_flags;





//�g���C���x���g���X���b�g��[0-3]���E��w�ցA[4-7]������w�ւƂ��ăt���O����ɂ܂Ƃ߂�B
//������display_flag_aux()�Ŏw�ւ̑ϐ��\��������̂Ɏg���B
void	obj_flags_known_jyoon_ring(int slot, u32b flgs[TR_FLAG_SIZE])
{
	int i;

	if (p_ptr->pclass != CLASS_JYOON) { msg_print("ERROR:�����ȊO��obj_flags_known_jyoon_ring()���Ă΂ꂽ"); return; }
	if (slot != INVEN_RIGHT && slot != INVEN_LEFT) { msg_print("ERROR:obj_flags_known_jyoon_ring()���s���ȃX���b�g�ŌĂ΂ꂽ"); return; }

	for (i = 0; i < TR_FLAG_SIZE; i++) flgs[i] = 0L;

	for (i = 0; i<4; i++)
	{
		int j;
		u32b tmp_flgs[TR_FLAG_SIZE];
		object_type *o_ptr = &inven_add[(slot - INVEN_RIGHT) * 4 + i];
		if (!o_ptr->k_idx)continue;

		object_flags_known(o_ptr, tmp_flgs);

		for (j = 0; j < TR_FLAG_SIZE; j++)	flgs[j] |= tmp_flgs[j];
	}

	//�g���C���x���g���ɓ��ꂽ�w�ւ̎􂢂͔������Ȃ��̂Ńt���O�폜(��������)����
	remove_flag(flgs, TR_TY_CURSE);
	remove_flag(flgs, TR_DRAIN_EXP);
	remove_flag(flgs, TR_ADD_L_CURSE);
	remove_flag(flgs, TR_ADD_H_CURSE);

}




/*
 * Helper function, see below
 */
static void display_flag_aux(int row, int col, cptr header,
				    int flag1, all_player_flags *f, u16b mode)
{
	int     i;
	bool    vuln = FALSE;
	int max_i;

	bool	empty = FALSE;
	bool	imm		= FALSE;

	if (have_flag(f->player_vuln, flag1) &&
	    !(have_flag(f->known_obj_imm, flag1) ||
	      have_flag(f->player_imm, flag1) ||
	      have_flag(f->tim_player_imm, flag1)))
		vuln = TRUE;

	if (!have_flag(f->player_flags, flag1) && !have_flag(f->tim_player_flags, flag1) 
		&& !have_flag(f->player_imm, flag1) && !have_flag(f->tim_player_imm, flag1) 
		&& !have_flag(f->known_obj_imm, flag1))
		empty = TRUE;

	if (have_flag(f->player_imm, flag1) || have_flag(f->tim_player_imm, flag1) 
		|| have_flag(f->known_obj_imm, flag1))
		imm = TRUE;

	if (mode & DP_CURSE) empty = TRUE;

	/* Weapon flags need only two column */
	if (mode & DP_WP) max_i = INVEN_LARM + 1;
	else max_i = INVEN_TOTAL;
	///mod141122 �ϐ������܂��ĂȂ��ƈÂ��\�����邽�߂̃t���O����
	for (i = INVEN_RARM; i < max_i; i++)
	{
		u32b flgs[TR_FLAG_SIZE];
		object_type *o_ptr;
		if(check_invalidate_inventory(i))continue;

		o_ptr = &inventory[i];
		//v1.1.46 �����̎w�ւ͓���t���O����
		if (p_ptr->pclass == CLASS_JYOON && (i == INVEN_RIGHT || i == INVEN_LEFT))
		{
			obj_flags_known_jyoon_ring(i, flgs);
		}
		else
		{
			if (!o_ptr->k_idx) continue;
			object_flags_known(o_ptr, flgs);
		}

		if(flag1 == TR_LITE)
		{
			if(have_flag(flgs, TR_LITE)) empty = FALSE;
			else if(have_flag(flgs, TR_SH_FIRE)) empty = FALSE;
			else if(i == INVEN_LITE)
			{
				if(o_ptr->name2 == EGO_LITE_DARKNESS) continue;
				else if((o_ptr->sval == SV_LITE_TORCH || o_ptr->sval == SV_LITE_LANTERN) && !o_ptr->xtra4) continue;
				else empty = FALSE;
			}
		}

		if(flag1 == TR_SLAY_EVIL && have_flag(flgs,TR_KILL_EVIL)) empty = FALSE; 
		else if(flag1 == TR_SLAY_GOOD && have_flag(flgs,TR_KILL_GOOD)) empty = FALSE; 
		else if(flag1 == TR_SLAY_ANIMAL && have_flag(flgs,TR_KILL_ANIMAL)) empty = FALSE; 
		else if(flag1 == TR_SLAY_HUMAN && have_flag(flgs,TR_KILL_HUMAN)) empty = FALSE; 
		else if(flag1 == TR_SLAY_UNDEAD && have_flag(flgs,TR_KILL_UNDEAD)) empty = FALSE; 
		else if(flag1 == TR_SLAY_DRAGON && have_flag(flgs,TR_KILL_DRAGON)) empty = FALSE; 
		else if(flag1 == TR_SLAY_DEITY && have_flag(flgs,TR_KILL_DEITY)) empty = FALSE; 
		else if(flag1 == TR_SLAY_DEMON && have_flag(flgs,TR_KILL_DEMON)) empty = FALSE; 
		else if(flag1 == TR_SLAY_KWAI && have_flag(flgs,TR_KILL_KWAI)) empty = FALSE; 
		else if(flag1 == TR_VORPAL && have_flag(flgs,TR_EX_VORPAL)) empty = FALSE; 

		if (mode & DP_CURSE)
		{
			if(object_is_cursed(o_ptr))empty = FALSE;
		}
		else
		{
			if (have_flag(flgs, flag1)) empty = FALSE;
		}
	}

	/* Header */
	if (!(mode & DP_IMM))
	{
		if(imm) c_put_str(TERM_L_BLUE, header, row, col);
		else if(vuln && !empty) c_put_str(TERM_ORANGE, header, row, col);
		else if(vuln) c_put_str(TERM_RED, header, row, col);
		else if(empty) c_put_str(TERM_L_DARK, header, row, col);
		else c_put_str(TERM_WHITE, header, row, col);
	}
	/* Advance */
	col += strlen(header) + 1;



	/* Check equipment */
	for (i = INVEN_RARM; i < max_i; i++)
	{
		u32b flgs[TR_FLAG_SIZE];
		object_type *o_ptr;

		/* Object */
		o_ptr = &inventory[i];
		//v1.1.46 �����̎w�ւ͓���t���O����
		if (p_ptr->pclass == CLASS_JYOON && (i == INVEN_RIGHT || i == INVEN_LEFT))
		{
			obj_flags_known_jyoon_ring(i, flgs);
		}
		else
		{
			object_flags_known(o_ptr, flgs);
		}

		/*:::���������ꂽ�X���b�g*/
		if(check_invalidate_inventory(i))
			c_put_str(TERM_DARK, ".", row, col);

		else if(flag1 == TR_LITE)
		{
			bool lite;
			if(!o_ptr->k_idx) lite = FALSE;
			else if(have_flag(flgs, TR_LITE)) lite = TRUE;
			else if(have_flag(flgs, TR_SH_FIRE)) lite = TRUE;
			else if(i == INVEN_LITE)
			{
				if(o_ptr->name2 == EGO_LITE_DARKNESS)lite = FALSE;
				else if((o_ptr->sval == SV_LITE_TORCH || o_ptr->sval == SV_LITE_LANTERN) && !o_ptr->xtra4)lite = FALSE;
				else lite = TRUE;
			}
			else lite = FALSE;

			if(lite) c_put_str((byte)TERM_WHITE,"+", row, col);
			else if(empty) c_put_str((byte)TERM_L_DARK, ".", row, col);
			else c_put_str((byte)TERM_SLATE, ".", row, col);
		}
		/* Default */
		else if (!(mode & DP_IMM))
		{
			if(imm) c_put_str(TERM_L_BLUE, ".", row, col);
			else if(empty) c_put_str((byte)(vuln ? TERM_RED : TERM_L_DARK), ".", row, col);
			else c_put_str((byte)(vuln ? TERM_ORANGE : TERM_SLATE), ".", row, col);
		}

		/* Check flags */
		if(check_invalidate_inventory(i)) /*�������Ȃ�*/;
		else if (mode & DP_CURSE)
		{
			if ((mode & DP_CURSE) && (have_flag(flgs, TR_ADD_L_CURSE) || have_flag(flgs, TR_ADD_H_CURSE)))
				c_put_str(TERM_L_DARK, "+", row, col);
			if ((mode & DP_CURSE) && (o_ptr->curse_flags & (TRC_CURSED | TRC_HEAVY_CURSE)))
				c_put_str(TERM_WHITE, "+", row, col);
			if ((mode & DP_CURSE) && (o_ptr->curse_flags & TRC_PERMA_CURSE))
				c_put_str(TERM_RED, "*", row, col);
		}
		else
		{

			if (have_flag(flgs, flag1))
			{
				if(imm || (mode & DP_IMM)) c_put_str(TERM_L_BLUE,(mode & DP_IMM) ? "*" : "+", row, col);
				else if(!empty && vuln) c_put_str(TERM_ORANGE,(mode & DP_IMM) ? "*" : "+", row, col);
				else if(empty) c_put_str((byte)(vuln ? TERM_L_RED : TERM_L_DARK),(mode & DP_IMM) ? "*" : "+", row, col);
				else c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE),(mode & DP_IMM) ? "*" : "+", row, col);
			}
		}

		/* Advance */
		col++;
	}

	/* Assume that player flag is already written */
	if (mode & DP_IMM) return;

	/* Default */
	if(imm) c_put_str(TERM_L_BLUE, ".", row, col);
	else if(!empty && vuln) c_put_str(TERM_ORANGE, ".", row, col);
	else if(empty) c_put_str((byte)(vuln ? TERM_RED : TERM_L_DARK), ".", row, col);
	else c_put_str((byte)(vuln ? TERM_RED : TERM_SLATE), ".", row, col);

	/* Player flags */
	if (have_flag(f->player_flags, flag1))
		c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE), "+", row, col);

	/* Timed player flags */
	if (have_flag(f->tim_player_flags, flag1)) c_put_str((byte)(vuln ? TERM_ORANGE : TERM_YELLOW), "#", row, col);

	/* Immunity */
	if (have_flag(f->tim_player_imm, flag1)) c_put_str(TERM_YELLOW, "*", row, col);
	if (have_flag(f->player_imm, flag1)) c_put_str(TERM_L_BLUE, "*", row, col);

	/* Vulnerability */
	if (vuln)
	{
		if(empty) c_put_str(TERM_RED, "v", row, col + 1);
		else c_put_str(TERM_ORANGE, "v", row, col + 1);
	}
}



/*
 * Special display, part 1
 */
/*:::���݂̑ϐ���\������*/
///sys race class magic �d�v�@�X�e�[�^�X��ʂɑϐ���\��
///mod131228 TR�t���O����ւ��ɂ�葮���ύX
static void display_player_flag_info(void)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	player_flags(f.player_flags);
	tim_player_flags(f.tim_player_flags);
	player_immunity(f.player_imm);
	tim_player_immunity(f.tim_player_imm);
	known_obj_immunity(f.known_obj_imm);
	player_vuln_flags(f.player_vuln);

	/*** Set 1 ***/

	row = 12;
	col = 1;

	display_player_equippy(row-2, col+8, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
display_flag_aux(row+0, col, "�ώ_  :", TR_RES_ACID, &f, 0);
display_flag_aux(row+0, col, "�ώ_  :", TR_IM_ACID, &f, DP_IMM);
display_flag_aux(row+1, col, "�ϓd��:", TR_RES_ELEC, &f, 0);
display_flag_aux(row+1, col, "�ϓd��:", TR_IM_ELEC, &f, DP_IMM);
display_flag_aux(row+2, col, "�ωΉ�:", TR_RES_FIRE, &f, 0);
display_flag_aux(row+2, col, "�ωΉ�:", TR_IM_FIRE, &f, DP_IMM);
display_flag_aux(row+3, col, "�ϗ�C:", TR_RES_COLD, &f, 0);
display_flag_aux(row+3, col, "�ϗ�C:", TR_IM_COLD, &f, DP_IMM);
display_flag_aux(row+4, col, "�ϓ�  :", TR_RES_POIS, &f, 0);
display_flag_aux(row+5, col, "�ϑM��:", TR_RES_LITE, &f, 0);
display_flag_aux(row+6, col, "�ψÍ�:", TR_RES_DARK, &f, 0);
display_flag_aux(row+7, col, "�ϔj��:", TR_RES_SHARDS, &f, 0);
display_flag_aux(row+8, col, "�ϐ�  :", TR_RES_WATER, &f, 0);
display_flag_aux(row+9, col, "�ϔj��:", TR_RES_HOLY, &f, 0);
#else
	display_flag_aux(row+0, col, "Acid  :", TR_RES_ACID, &f, 0);
	display_flag_aux(row+0, col, "Acid  :", TR_IM_ACID, &f, DP_IMM);
	display_flag_aux(row+1, col, "Elec  :", TR_RES_ELEC, &f, 0);
	display_flag_aux(row+1, col, "Elec  :", TR_IM_ELEC, &f, DP_IMM);
	display_flag_aux(row+2, col, "Fire  :", TR_RES_FIRE, &f, 0);
	display_flag_aux(row+2, col, "Fire  :", TR_IM_FIRE, &f, DP_IMM);
	display_flag_aux(row+3, col, "Cold  :", TR_RES_COLD, &f, 0);
	display_flag_aux(row+3, col, "Cold  :", TR_IM_COLD, &f, DP_IMM);
	display_flag_aux(row+4, col, "Poison:", TR_RES_POIS, &f, 0);
	display_flag_aux(row+5, col, "Light :", TR_RES_LITE, &f, 0);
	display_flag_aux(row+6, col, "Dark  :", TR_RES_DARK, &f, 0);
	display_flag_aux(row+7, col, "Shard :", TR_RES_SHARDS, &f, 0);
	display_flag_aux(row+8, col, "Blind :", TR_RES_BLIND, &f, 0);
	display_flag_aux(row+9, col, "Conf  :", TR_RES_CONF, &f, 0);
#endif


	/*** Set 2 ***/

	row = 12;
	col = 26;

	display_player_equippy(row-2, col+8, 0);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
display_flag_aux(row+0, col, "�ύ���:", TR_RES_SOUND, &f, 0);
display_flag_aux(row+1, col, "�ϒn��:", TR_RES_NETHER, &f, 0);
display_flag_aux(row+2, col, "�ώ���:", TR_RES_TIME, &f, 0);
display_flag_aux(row+3, col, "�ύ���:", TR_RES_CHAOS, &f, 0);
display_flag_aux(row+4, col, "�ϗ�:", TR_RES_DISEN, &f, 0);
display_flag_aux(row+5, col, "�ϋ��|:", TR_RES_FEAR, &f, 0);
display_flag_aux(row+6, col, "�ϖ��:", TR_FREE_ACT, &f, 0);
display_flag_aux(row+7, col, "�ύ���:", TR_RES_CONF, &f, 0);
display_flag_aux(row+8, col, "�ϖӖ�:", TR_RES_BLIND, &f, 0);
display_flag_aux(row+9, col, "�ϋ��C:", TR_RES_INSANITY, &f, 0);
#else
	display_flag_aux(row+0, col, "Sound :", TR_RES_SOUND, &f, 0);
	display_flag_aux(row+1, col, "Nether:", TR_RES_NETHER, &f, 0);
	display_flag_aux(row+2, col, "Nexus :", TR_RES_NEXUS, &f, 0);
	display_flag_aux(row+3, col, "Chaos :", TR_RES_CHAOS, &f, 0);
	display_flag_aux(row+4, col, "Disnch:", TR_RES_DISEN, &f, 0);
	display_flag_aux(row+5, col, "Fear  :", TR_RES_FEAR, &f, 0);
	display_flag_aux(row+6, col, "Reflct:", TR_REFLECT, &f, 0);
	display_flag_aux(row+7, col, "AuFire:", TR_SH_FIRE, &f, 0);
	display_flag_aux(row+8, col, "AuElec:", TR_SH_ELEC, &f, 0);
	display_flag_aux(row+9, col, "AuCold:", TR_SH_COLD, &f, 0);
#endif


	/*** Set 3 ***/

	row = 12;
	col = 51;

	display_player_equippy(row-2, col+10, 0);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+10);

#ifdef JP
display_flag_aux(row+0, col, "�������F:", TR_SEE_INVIS, &f, 0);
display_flag_aux(row+1, col, "�x��    :", TR_WARNING, &f, 0);
display_flag_aux(row+2, col, "�h�{����:", TR_SLOW_DIGEST, &f, 0);
display_flag_aux(row+3, col, "�}����:", TR_REGEN, &f, 0);
display_flag_aux(row+4, col, "���V    :", TR_LEVITATION, &f, 0);
display_flag_aux(row+5, col, "�����ړ�:", TR_SPEEDSTER, &f, 0);
display_flag_aux(row+6, col, "���e���|:", TR_NO_TELE, &f, 0);
display_flag_aux(row+7, col, "�����@  :", TR_NO_MAGIC, &f, 0);
display_flag_aux(row+8, col, "����    :", TR_REFLECT, &f, 0);
display_flag_aux(row+9, col, "��    :", 0, &f, DP_CURSE);
#else
	display_flag_aux(row+0, col, "Speed     :", TR_SPEED, &f, 0);
	display_flag_aux(row+1, col, "FreeAction:", TR_FREE_ACT, &f, 0);
	display_flag_aux(row+2, col, "SeeInvisi.:", TR_SEE_INVIS, &f, 0);
	display_flag_aux(row+3, col, "Hold Life :", TR_HOLD_LIFE, &f, 0);
	display_flag_aux(row+4, col, "Warning   :", TR_WARNING, &f, 0);
	display_flag_aux(row+5, col, "SlowDigest:", TR_SLOW_DIGEST, &f, 0);
	display_flag_aux(row+6, col, "Regene.   :", TR_REGEN, &f, 0);
	display_flag_aux(row+7, col, "Levitation:", TR_LEVITATION, &f, 0);
	display_flag_aux(row+8, col, "Perm Lite :", TR_LITE, &f, 0);
	display_flag_aux(row+9, col, "Cursed    :", 0, &f, DP_CURSE);
#endif

}


/*
 * Special display, part 2
 */
/*:::�X���C��e���p�X�Ȃǂ̏�Ԃ�\������*/
///item res sys �X���C�E�e���p�X�Ȃǂ̃p�����[�^��ʕ\��
///mod131228 TR�t���O����ւ��Ȃ�
static void display_player_other_flag_info(void)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	player_flags(f.player_flags);
	tim_player_flags(f.tim_player_flags);
	player_immunity(f.player_imm);
	tim_player_immunity(f.tim_player_imm);
	known_obj_immunity(f.known_obj_imm);
	player_vuln_flags(f.player_vuln);

	/*** Set 1 ***/

	row = 3;
	col = 1;

	display_player_equippy(row-2, col+12, DP_WP);

	c_put_str(TERM_WHITE, "ab@", row-1, col+12);

#ifdef JP
	display_flag_aux(row+ 0, col, "���� �{�� :", TR_SLAY_EVIL, &f, DP_WP);
	display_flag_aux(row+ 0, col, "���� �{�� :", TR_KILL_EVIL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 1, col, "���� �{�� :", TR_SLAY_GOOD, &f, DP_WP);
	display_flag_aux(row+ 1, col, "���� �{�� :", TR_KILL_GOOD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 2, col, "�l�� �{�� :", TR_SLAY_HUMAN, &f, DP_WP);
	display_flag_aux(row+ 2, col, "�l�� �{�� :", TR_KILL_HUMAN, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 3, col, "���� �{�� :", TR_SLAY_ANIMAL, &f, DP_WP);
	display_flag_aux(row+ 3, col, "���� �{�� :", TR_KILL_ANIMAL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 4, col, "�s�� �{�� :", TR_SLAY_UNDEAD, &f, DP_WP);
	display_flag_aux(row+ 4, col, "�s�� �{�� :", TR_KILL_UNDEAD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 5, col, "��   �{�� :", TR_SLAY_DRAGON, &f, DP_WP);
	display_flag_aux(row+ 5, col, "��   �{�� :", TR_KILL_DRAGON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 6, col, "���� �{�� :", TR_SLAY_DEMON, &f, DP_WP);
	display_flag_aux(row+ 6, col, "���� �{�� :", TR_KILL_DEMON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 7, col, "�_�i �{�� :", TR_SLAY_DEITY, &f, DP_WP);
	display_flag_aux(row+ 7, col, "�_�i �{�� :", TR_KILL_DEITY, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 8, col, "�d�� �{�� :", TR_SLAY_KWAI, &f, DP_WP);
	display_flag_aux(row+ 8, col, "�d�� �{�� :", TR_KILL_KWAI, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 9, col, "�n��      :", TR_BRAND_ACID, &f, DP_WP);
	display_flag_aux(row+10, col, "�d��      :", TR_BRAND_ELEC, &f, DP_WP);
	display_flag_aux(row+11, col, "�Ċ�      :", TR_BRAND_FIRE, &f, DP_WP);
	display_flag_aux(row+12, col, "����      :", TR_BRAND_COLD, &f, DP_WP);
	display_flag_aux(row+13, col, "�ŎE      :", TR_BRAND_POIS, &f, DP_WP);
	display_flag_aux(row+14, col, "�؂ꖡ    :", TR_VORPAL, &f, DP_WP);
	display_flag_aux(row+14, col, "�؂ꖡ    :", TR_EX_VORPAL, &f, DP_WP|DP_IMM);
	display_flag_aux(row+15, col, "�n�k      :", TR_IMPACT, &f, DP_WP);
	display_flag_aux(row+16, col, "�z��      :", TR_VAMPIRIC, &f, DP_WP);
	display_flag_aux(row+17, col, "�J�I�X����:", TR_CHAOTIC, &f, DP_WP);
	display_flag_aux(row+18, col, "����      :", TR_FORCE_WEAPON, &f, DP_WP);
#else
	display_flag_aux(row+ 0, col, "Slay Evil :", TR_SLAY_EVIL, &f, DP_WP);
	display_flag_aux(row+ 0, col, "Slay Evil :", TR_KILL_EVIL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 1, col, "Slay Und. :", TR_SLAY_UNDEAD, &f, DP_WP);
	display_flag_aux(row+ 1, col, "Slay Und. :", TR_KILL_UNDEAD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 2, col, "Slay Demon:", TR_SLAY_DEMON, &f, DP_WP);
	display_flag_aux(row+ 2, col, "Slay Demon:", TR_KILL_DEMON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 3, col, "Slay Drag.:", TR_SLAY_DRAGON, &f, DP_WP);
	display_flag_aux(row+ 3, col, "Slay Drag.:", TR_KILL_DRAGON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 4, col, "Slay Human:", TR_SLAY_HUMAN, &f, DP_WP);
	display_flag_aux(row+ 4, col, "Slay Human:", TR_KILL_HUMAN, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 5, col, "Slay Anim.:", TR_SLAY_ANIMAL, &f, DP_WP);
	display_flag_aux(row+ 5, col, "Slay Anim.:", TR_KILL_ANIMAL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 6, col, "Slay Orc  :", TR_SLAY_ORC, &f, DP_WP);
	display_flag_aux(row+ 6, col, "Slay Orc  :", TR_KILL_ORC, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 7, col, "Slay Troll:", TR_SLAY_TROLL, &f, DP_WP);
	display_flag_aux(row+ 7, col, "Slay Troll:", TR_KILL_TROLL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 8, col, "Slay Giant:", TR_SLAY_GIANT, &f, DP_WP);
	display_flag_aux(row+ 8, col, "Slay Giant:", TR_KILL_GIANT, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 9, col, "Acid Brand:", TR_BRAND_ACID, &f, DP_WP);
	display_flag_aux(row+10, col, "Elec Brand:", TR_BRAND_ELEC, &f, DP_WP);
	display_flag_aux(row+11, col, "Fire Brand:", TR_BRAND_FIRE, &f, DP_WP);
	display_flag_aux(row+12, col, "Cold Brand:", TR_BRAND_COLD, &f, DP_WP);
	display_flag_aux(row+13, col, "Poison Brd:", TR_BRAND_POIS, &f, DP_WP);
	display_flag_aux(row+14, col, "Sharpness :", TR_VORPAL, &f, DP_WP);
	display_flag_aux(row+15, col, "Quake     :", TR_IMPACT, &f, DP_WP);
	display_flag_aux(row+16, col, "Vampiric  :", TR_VAMPIRIC, &f, DP_WP);
	display_flag_aux(row+17, col, "Chaotic   :", TR_CHAOTIC, &f, DP_WP);
	display_flag_aux(row+18, col, "Force Wep.:", TR_FORCE_WEAPON, &f, DP_WP);
#endif


	/*** Set 2 ***/

	row = 3;
	col = col + 12 + 7;

	display_player_equippy(row-2, col+13, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+13);

#ifdef JP
	display_flag_aux(row+ 0, col, "�e���p�V�[ :", TR_TELEPATHY, &f, 0);
	display_flag_aux(row+ 1, col, "���j�[�NESP:", TR_ESP_UNIQUE, &f, 0);
	display_flag_aux(row+ 2, col, "������ESP  :", TR_ESP_NONLIVING, &f, 0);
	display_flag_aux(row+ 3, col, "����ESP    :", TR_ESP_EVIL, &f, 0);
	display_flag_aux(row+ 4, col, "����ESP    :", TR_ESP_GOOD, &f, 0);
	display_flag_aux(row+ 5, col, "�l��ESP    :", TR_ESP_HUMAN, &f, 0);
	display_flag_aux(row+ 6, col, "����ESP    :", TR_ESP_ANIMAL, &f, 0);
	display_flag_aux(row+ 7, col, "�s��ESP    :", TR_ESP_UNDEAD, &f, 0);
	display_flag_aux(row+ 8, col, "��ESP      :", TR_ESP_DRAGON, &f, 0);
	display_flag_aux(row+ 9, col, "����ESP    :", TR_ESP_DEMON, &f, 0);
	display_flag_aux(row+10, col, "�_�iESP    :", TR_ESP_DEITY, &f, 0);
	display_flag_aux(row+11, col, "�d��ESP    :", TR_ESP_KWAI, &f, 0);
	//display_flag_aux(row+12, col, "           :",  , &f, 0);
	display_flag_aux(row+13, col, "�r�͈ێ�   :", TR_SUST_STR, &f, 0);
	display_flag_aux(row+14, col, "�m�͈ێ�   :", TR_SUST_INT, &f, 0);
	display_flag_aux(row+15, col, "�����ێ�   :", TR_SUST_WIS, &f, 0);
	display_flag_aux(row+16, col, "��p�ێ�   :", TR_SUST_DEX, &f, 0);
	display_flag_aux(row+17, col, "�ϋv�ێ�   :", TR_SUST_CON, &f, 0);
	display_flag_aux(row+18, col, "���͈ێ�   :", TR_SUST_CHR, &f, 0);
#else
	display_flag_aux(row+ 0, col, "Telepathy  :", TR_TELEPATHY, &f, 0);
	display_flag_aux(row+ 1, col, "ESP Evil   :", TR_ESP_EVIL, &f, 0);
	display_flag_aux(row+ 2, col, "ESP Noliv. :", TR_ESP_NONLIVING, &f, 0);
	display_flag_aux(row+ 3, col, "ESP Good   :", TR_ESP_GOOD, &f, 0);
	display_flag_aux(row+ 4, col, "ESP Undead :", TR_ESP_UNDEAD, &f, 0);
	display_flag_aux(row+ 5, col, "ESP Demon  :", TR_ESP_DEMON, &f, 0);
	display_flag_aux(row+ 6, col, "ESP Dragon :", TR_ESP_DRAGON, &f, 0);
	display_flag_aux(row+ 7, col, "ESP Human  :", TR_ESP_HUMAN, &f, 0);
	display_flag_aux(row+ 8, col, "ESP Animal :", TR_ESP_ANIMAL, &f, 0);
	display_flag_aux(row+ 9, col, "ESP Orc    :", TR_ESP_ORC, &f, 0);
	display_flag_aux(row+10, col, "ESP Troll  :", TR_ESP_TROLL, &f, 0);
	display_flag_aux(row+11, col, "ESP Giant  :", TR_ESP_GIANT, &f, 0);
	display_flag_aux(row+12, col, "ESP Unique :", TR_ESP_UNIQUE, &f, 0);
	display_flag_aux(row+13, col, "Sust Str   :", TR_SUST_STR, &f, 0);
	display_flag_aux(row+14, col, "Sust Int   :", TR_SUST_INT, &f, 0);
	display_flag_aux(row+15, col, "Sust Wis   :", TR_SUST_WIS, &f, 0);
	display_flag_aux(row+16, col, "Sust Dex   :", TR_SUST_DEX, &f, 0);
	display_flag_aux(row+17, col, "Sust Con   :", TR_SUST_CON, &f, 0);
	display_flag_aux(row+18, col, "Sust Chr   :", TR_SUST_CHR, &f, 0);
#endif


	/*** Set 3 ***/

	row = 3;
	col = col + 12 + 17;

	display_player_equippy(row-2, col+14, 0);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+14);

#ifdef JP
	display_flag_aux(row+ 0, col, "����        :", TR_SPEED, &f, 0);
	display_flag_aux(row+ 1, col, "�ǉ��U��    :", TR_BLOWS, &f, 0);
	display_flag_aux(row+ 2, col, "�ԊO������  :", TR_INFRA, &f, 0);
	display_flag_aux(row+ 3, col, "���@����x�z:", TR_MAGIC_MASTERY, &f, 0);
	display_flag_aux(row+ 4, col, "�B��        :", TR_STEALTH, &f, 0);
	display_flag_aux(row+ 5, col, "�T��        :", TR_SEARCH, &f, 0);
	display_flag_aux(row+ 6, col, "�̌@        :", TR_TUNNEL, &f, 0);

	//v1.1.99 �ǉ�
	display_flag_aux(row + 7, col, "㩉����@�@�@:", TR_DISARM, &f, 0);
	display_flag_aux(row + 8, col, "���@�h��㏸:", TR_SAVING, &f, 0);

	display_flag_aux(row+ 9, col, "���@��x����:", TR_EASY_SPELL, &f, 0);
	display_flag_aux(row+10, col, "����͌���:", TR_DEC_MANA, &f, 0);
	display_flag_aux(row+11, col, "�j��        :", TR_BLESSED, &f, 0);
	display_flag_aux(row+12, col, "����        :", TR_LITE, &f, 0);
	///mod141219 �����͒��ŗ�O�������邱�Ƃɂ���
	//display_flag_aux(row+11, col, "����        :", TR_SH_FIRE, &f, 0);
	display_flag_aux(row+13, col, "��n        :", TR_RIDING, &f, 0);
	display_flag_aux(row+14, col, "����        :", TR_THROW, &f, 0);//�v��񂩂�
	display_flag_aux(row+15, col, "�o���l�z��  :", TR_DRAIN_EXP, &f, 0);
	display_flag_aux(row+16, col, "���e���|�[�g:", TR_TELEPORT, &f, 0);
	display_flag_aux(row+17, col, "����        :", TR_AGGRAVATE, &f, 0);
	display_flag_aux(row+18, col, "���Ẩ��O  :", TR_TY_CURSE, &f, 0);
#else
	display_flag_aux(row+ 0, col, "Add Blows   :", TR_BLOWS, &f, 0);
	display_flag_aux(row+ 1, col, "Add Tunnel  :", TR_TUNNEL, &f, 0);
	display_flag_aux(row+ 2, col, "Add Infra   :", TR_INFRA, &f, 0);
	display_flag_aux(row+ 3, col, "Add Device  :", TR_MAGIC_MASTERY, &f, 0);
	display_flag_aux(row+ 4, col, "Add Stealth :", TR_STEALTH, &f, 0);
	display_flag_aux(row+ 5, col, "Add Search  :", TR_SEARCH, &f, 0);

	display_flag_aux(row+ 7, col, "Riding      :", TR_RIDING, &f, 0);
	display_flag_aux(row+ 8, col, "Throw       :", TR_THROW, &f, 0);
	display_flag_aux(row+ 9, col, "Blessed     :", TR_BLESSED, &f, 0);
	display_flag_aux(row+10, col, "No Teleport :", TR_NO_TELE, &f, 0);
	display_flag_aux(row+11, col, "Anti Magic  :", TR_NO_MAGIC, &f, 0);
	display_flag_aux(row+12, col, "Econom. Mana:", TR_DEC_MANA, &f, 0);

	display_flag_aux(row+14, col, "Drain Exp   :", TR_DRAIN_EXP, &f, 0);
	display_flag_aux(row+15, col, "Rnd.Teleport:", TR_TELEPORT, &f, 0);
	display_flag_aux(row+16, col, "Aggravate   :", TR_AGGRAVATE, &f, 0);
	display_flag_aux(row+17, col, "TY Curse    :", TR_TY_CURSE, &f, 0);
#endif

}


/*
 * Special display, part 2a
 */
/*:::���̏��\�����3�y�[�W�ڂ̈ꕔ*/
static void display_player_misc_info(void)
{
	char	buf[80];
	char	tmp[80];

	/* Display basics */
#ifdef JP
//put_str("���O  :", 1, 26);
put_str("���O:", 1, 16);
put_str("����:", 3, 1);
put_str("�푰:", 4, 1);
put_str("�E��:", 5, 1);
#else
	put_str("Name  :", 1, 26);
	put_str("Sex   :", 3, 1);
	put_str("Race  :", 4, 1);
	put_str("Class :", 5, 1);
#endif
	if(p_ptr->psex == SEX_MALE)
		strcpy(tmp,ap_ptr->title);
	else
		strcpy(tmp,ap_ptr->f_title);
#ifdef JP
	if(ap_ptr->no == 1)
		strcat(tmp,"��");
#else
		strcat(tmp," ");
#endif
	strcat(tmp,player_name);

	//c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, tmp, 1, 24);
	c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 7);
	///mod150103 �푰���L�q�@��ǐ_�l��p�����ǉ�
	if(prace_is_(RACE_STRAYGOD))
		c_put_str(TERM_L_BLUE, deity_table[p_ptr->race_multipur_val[3]].deity_name, 4, 7);
	else
		c_put_str(TERM_L_BLUE, (p_ptr->mimic_form ? mimic_info[p_ptr->mimic_form].title : rp_ptr->title), 4, 7);
	//c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 9);
	///mod140206 ���E�Ɩ��ύX�Ή�
	c_put_str(TERM_L_BLUE, player_class_name, 5, 7);

	/* Display extras */
#ifdef JP
put_str("�k�u:", 6, 1);
put_str("�g�o:", 7, 1);
put_str("�l�o:", 8, 1);
#else
	put_str("Level :", 6, 1);
	put_str("Hits  :", 7, 1);
	put_str("Mana  :", 8, 1);
#endif


	(void)sprintf(buf, "%d", (int)p_ptr->lev);
	c_put_str(TERM_L_BLUE, buf, 6, 7);

	if(p_ptr->pclass == CLASS_CLOWNPIECE)
		(void)sprintf(buf, "%d", (int)p_ptr->mhp);
	else
		(void)sprintf(buf, "%d/%d", (int)p_ptr->chp, (int)p_ptr->mhp);

	c_put_str(TERM_L_BLUE, buf, 7, 7);
	(void)sprintf(buf, "%d/%d", (int)p_ptr->csp, (int)p_ptr->msp);
	c_put_str(TERM_L_BLUE, buf, 8, 7);
}


/*
 * Special display, part 2b
 *
 * How to print out the modifications and sustains.
 * Positive mods with no sustain will be light green.
 * Positive mods with a sustain will be dark green.
 * Sustains (with no modification) will be a dark green 's'.
 * Negative mods (from a curse) will be red.
 * Huge mods (>9), like from MICoMorgoth, will be a '*'
 * No mod, no sustain, will be a slate '.'
 */
/*:::�X�e�[�^�X��ʂ́��̃p�����[�^*/
///sys ���̃p�����[�^�v�Z�i�X�e�[�^�X��ʗp�j

///mod140104 �p�����[�^����ƕ\�L��ύX
static void display_player_stat_info(void)
{
	int i, e_adj;
	int stat_col, stat;
	int row, col;

	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];

	byte a;
	char c;

	char buf[80];


	/* Column */
	stat_col = 22;

	/* Row */
	row = 3;

	/* Print out the labels for the columns */
#ifdef JP
c_put_str(TERM_WHITE, "�\��", row, stat_col+1);
c_put_str(TERM_BLUE, "  ��{", row, stat_col+7);
c_put_str(TERM_L_BLUE, " �� �E �� �� ", row, stat_col+13);
c_put_str(TERM_L_GREEN, "���v", row, stat_col+28);
c_put_str(TERM_YELLOW, "����", row, stat_col+35);
#else
	c_put_str(TERM_WHITE, "Stat", row, stat_col+1);
	c_put_str(TERM_BLUE, "  Base", row, stat_col+7);
	c_put_str(TERM_L_BLUE, "RacClaPerMod", row, stat_col+13);
	c_put_str(TERM_L_GREEN, "Actual", row, stat_col+26);
	c_put_str(TERM_YELLOW, "Current", row, stat_col+32);
#endif


	/* Display the stats */
	for (i = 0; i < 6; i++)
	{
		int r_adj,c_adj;
		int tmp;

		if (p_ptr->mimic_form) r_adj = mimic_info[p_ptr->mimic_form].r_adj[i];
		else r_adj = rp_ptr->r_adj[i];

		//�E�ƕ␳�l�@�얲�ɗ�O����
		c_adj = (int)cp_ptr->c_adj[i];
		if(p_ptr->pclass == CLASS_REIMU) c_adj += osaisen_rank();
		else if(p_ptr->pclass == CLASS_KANAKO) c_adj += kanako_rank();
		else if(p_ptr->pclass == CLASS_DOREMY && IN_DREAM_WORLD) c_adj += 2;
		else if(p_ptr->pclass == CLASS_SUNNY)
		{
			if(is_daytime()) c_adj += 1;
			else c_adj -= 1;
		}
		else if(p_ptr->pclass == CLASS_LUNAR)
		{
			if(is_daytime()) c_adj -= 1;
			else c_adj += 1;
		}
		else if (p_ptr->pclass == CLASS_LARVA)
		{
			c_adj += p_ptr->lev / 15;
		}

		//v1.1.21 ���m�p�����[�^�A�b�v�@�b�l�����Ɨݐς���悤��else�͂��Ȃ�
		if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_MUSCLE))
		{
			if(i == A_STR) c_adj += 2;
			if(i == A_DEX) c_adj += 2;
			if(i == A_CON) c_adj += 2;
		}
		if (HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_MAGITEM))
		{
			if(i == A_INT) c_adj += 2;
			if(i == A_WIS) c_adj += 2;
		}

		/* Calculate equipment adjustment */
		e_adj = 0;
		///sys 18/***�Ȃǂ̕\�L�Ɋւ��镔��
		/* Icky formula to deal with the 18 barrier */
		
		///mod140104 �p�����[�^����ƕ\�L��ύX
		e_adj = p_ptr->stat_top[i] - p_ptr->stat_max[i];

		/*
		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - p_ptr->stat_max[i]) / 10;
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - p_ptr->stat_max[i];
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - 18) / 10 - p_ptr->stat_max[i] + 18;

		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - (p_ptr->stat_max[i] - 19) / 10 - 19;
		*/
		///race �G���g�̃p�����[�^���ꏈ��
		if (prace_is_(RACE_ENT))
		{
			switch (i)
			{
				case A_STR:
				case A_CON:
					if (p_ptr->lev > 25) r_adj++;
					if (p_ptr->lev > 40) r_adj++;
					if (p_ptr->lev > 45) r_adj++;
					break;
				case A_DEX:
					if (p_ptr->lev > 25) r_adj--;
					if (p_ptr->lev > 40) r_adj--;
					if (p_ptr->lev > 45) r_adj--;
					break;
			}
		}
		if (prace_is_(RACE_STRAYGOD))
		{
			r_adj += deity_table[p_ptr->race_multipur_val[3]].stat_adj[i];
		}
		///mod140412 �z���S�p�����[�^����
		if((prace_is_(RACE_VAMPIRE) || p_ptr->mimic_form == MIMIC_VAMPIRE)	)
		{
			if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
			{
				if(turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30) r_adj += 3;
			}
			else
			{
				if(!p_ptr->resist_lite) r_adj -= 2;
				else r_adj -= 1;
			}
		}
		///mod140412 �b�l�p�����[�^����
		///mod150121 �d���͏���
		if(prace_is_(RACE_WARBEAST) && p_ptr->pclass != CLASS_KEINE)
		{
			if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
			{
				if(turn / (TURNS_PER_TICK * TOWN_DAWN) % 30 + 16 == 30) r_adj += 2;
			}
		}
		//�d�����򉻒�
		else if(p_ptr->pclass == CLASS_KEINE && !p_ptr->mimic_form && p_ptr->magic_num1[0])
		{
			if(i == A_STR) r_adj += 3;
			if(i == A_INT) r_adj += 5;
			if(i == A_WIS) r_adj += 4;
			if(i == A_DEX) r_adj += 3;
			if(i == A_CON) r_adj += 3;
			if(i == A_CHR) r_adj += 1;
		}


		e_adj -= r_adj;
		e_adj -= c_adj;
		e_adj -= ap_ptr->a_adj[i];

		if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
			/* Reduced name of stat */
			c_put_str(TERM_WHITE, stat_names_reduced[i], row + i+1, stat_col+1);
		else
			c_put_str(TERM_WHITE, stat_names[i], row + i+1, stat_col+1);


		/* Internal "natural" max value.  Maxes at 18/100 */
		/* This is useful to see if you are maxed out */
		cnv_stat(p_ptr->stat_max[i], buf);

		if (p_ptr->stat_max[i] == p_ptr->stat_max_max[i])
		{
#ifdef JP
			c_put_str(TERM_BLUE, "!", row + i+1, stat_col + 6);
		//	c_put_str(TERM_WHITE, "!", row + i+1, stat_col + 6);
#else
			c_put_str(TERM_WHITE, "!", row + i+1, stat_col + 4);
#endif
		}

		c_put_str(TERM_BLUE, buf, row + i+1, stat_col + 13 - strlen(buf));

		/* Race, class, and equipment modifiers */
		(void)sprintf(buf, "%3d", r_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 13);
		//�E�ƕ␳�l
		(void)sprintf(buf, "%3d", c_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 16);
		//���i�␳�l
		(void)sprintf(buf, "%3d", (int)ap_ptr->a_adj[i]);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 19);

		(void)sprintf(buf, "%3d", (int)e_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 22);

		///mod140326 �p�����[�^���E�����\�L�����̂��ߕύX
		/* Actual maximal modified value */
		//cnv_stat(p_ptr->stat_top[i], buf);
		tmp = p_ptr->stat_ind[i]+3 + (p_ptr->stat_top[i] - p_ptr->stat_use[i]);
		if(tmp == STAT_HYPERMAX)
		{
			(void)sprintf(buf, "   *%2d", tmp);
			c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 26);
		}
		else if(tmp > STAT_ADD_MAX)
		{
			(void)sprintf(buf, "   !%2d", tmp);
			c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 26);
		}
		else if(tmp == STAT_ADD_MAX)
		{
			(void)sprintf(buf, "   !%2d", tmp);
			c_put_str(TERM_L_GREEN, buf, row + i+1, stat_col + 26);
		}
		else
		{
			(void)sprintf(buf, "    %2d", tmp);
			c_put_str(TERM_L_GREEN, buf, row + i+1, stat_col + 26);
		}

		/* Only display stat_use if not maximal */
		if (p_ptr->stat_use[i] < p_ptr->stat_top[i])
		{
			(void)sprintf(buf, "    %2d", p_ptr->stat_ind[i]+3);
			c_put_str(TERM_YELLOW, buf, row + i+1, stat_col + 33);
		}
	}

	/* Column */
	col = stat_col + 41;

	/* Header and Footer */
	c_put_str(TERM_WHITE, "abcdefghijkl@", row, col);
#ifdef JP
c_put_str(TERM_L_GREEN, "�\�͏C��", row - 1, col);
#else
	c_put_str(TERM_L_GREEN, "Modification", row - 1, col);
#endif


	/* Process equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Access object */
		o_ptr = &inventory[i];

		/* Acquire "known" flags */
		object_flags_known(o_ptr, flgs);

		/* Initialize color based of sign of pval. */
		///sys �p�����[�^��10�ȏ�オ��悤�ȑ����͐F��ς��Ĉ�̌������o���悤�ɂ��Ă݂�H
		for (stat = 0; stat < 6; stat++)
		{
			/* Default */
			a = TERM_SLATE;
			c = '.';

			if(check_invalidate_inventory(i))
			{
				//���������ꂽ�����i�ɂ͉������Ȃ�
			}

			/* Boost */
			else if (have_flag(flgs, stat))
			{
				/* Default */
				c = '*';

				/* Good */
				if (o_ptr->pval > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (o_ptr->pval < 10) c = '0' + o_ptr->pval;
				}

				if (have_flag(flgs, stat + TR_SUST_STR))
				{
					/* Dark green for sustained stats */
					a = TERM_GREEN;
				}

				/* Bad */
				if (o_ptr->pval < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (o_ptr->pval > -10) c = '0' - o_ptr->pval;
				}
			}

			/* Sustain */
			else if (have_flag(flgs, stat + TR_SUST_STR))
			{
				/* Dark green "s" */
				a = TERM_GREEN;
				c = 's';
			}

			/* Dump proper character */
			Term_putch(col, row + stat+1, a, c);
		}

		/* Advance */
		col++;
	}

	//v1.1.46 �������g���X���b�g�ɑ��������w�ւ̃p�����[�^�\��
	if (p_ptr->pclass == CLASS_JYOON)
	{
		int tmp_col = stat_col + 44;//d�X���b�g����J�n
		int j;

		//�E��w�A����w���[�v
		for (i = INVEN_RIGHT; i <= INVEN_LEFT; i++)
		{
			int stat_sum[6] = { 0,0,0,0,0,0 };
			bool sust_stat[6] = { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE };
			if (check_invalidate_inventory(i)) continue;

			//�E��w�A����w���ꂼ��ɑΉ�����g���C���x���g���̃X���b�g�X�[�v 0-3���E���4-7������
			for (j = 0; j<4; j++)
			{
				int slot = (i - INVEN_RIGHT) * 4 + j;
				int pv;
				o_ptr = &inven_add[slot];
				if (!o_ptr->k_idx) continue;

				pv = o_ptr->pval / 2; //�w�ւ�pval�����Ƃ���
				object_flags_known(o_ptr, flgs);
				//�p�����[�^���[�v�@A_STR�`A_CHR��0�`6�ł��邱�Ƃ��O��ɂȂ��Ă���
				//�Ή��p�����[�^��pval���邢�͈ێ��t���O���W�v����
				for (stat = 0; stat < 6; stat++)
				{
					if (have_flag(flgs, stat))
						stat_sum[stat] += pv;

					if (have_flag(flgs, stat + TR_SUST_STR))
						sust_stat[stat] = TRUE;

				}
			}

			//�W�v���ς񂾃p�����[�^��\������B�p�����[�^�̐����ƈێ��\�̗͂L���ŐF��L����ς���B
			for (stat = 0; stat < 6; stat++)
			{
				a = TERM_SLATE;
				c = '.';
				if (stat_sum[stat] > 0)
				{
					c = '*';
					a = TERM_L_GREEN;

					if (stat_sum[stat] > 0 && stat_sum[stat] < 10)
					c = '0' + stat_sum[stat];
					if (sust_stat[stat])
						a = TERM_GREEN;
				}

				else if (stat_sum[stat] < 0)
				{
					a = TERM_RED;
					if (stat_sum[stat] > -10)
						c = '0' - stat_sum[stat];
				}
				else if (sust_stat[stat])
				{
					a = TERM_GREEN;
					c = 's';
				}
				Term_putch(tmp_col, row + stat + 1, a, c);
			}
			tmp_col++;

		}
	}




	/* Player flags */
	player_flags(flgs);

	/* Check stats */
	for (stat = 0; stat < 6; stat++)
	{
		/* Default */
		a = TERM_SLATE;
		c = '.';



		/* Mutations ... */
		///mod140316 �����������ł����̒��ɓ��邱�Ƃɂ���B�債���������ɂ��Ȃ�񂾂낤�B
	//	if (p_ptr->muta2 || p_ptr->muta3 || p_ptr->tsuyoshi || p_ptr->pclass == CLASS_KOGASA)
		{
			int dummy = 0;
			feature_type *f_ptr = &f_info[cave[py][px].feat];





			///mod140316 �n�`�ɂ��p�����[�^�ω�
			//�푰
			if((p_ptr->prace == RACE_KAPPA) && have_flag(f_ptr->flags, FF_WATER))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if ((p_ptr->prace == RACE_YAMAWARO) && have_flag(f_ptr->flags, FF_TREE))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if(CHECK_FAIRY_TYPE == 5 && have_flag(f_ptr->flags, FF_LAVA))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if(CHECK_FAIRY_TYPE == 27 && have_flag(f_ptr->flags, FF_PLANT))
			{
				if (stat == A_STR || stat == A_WIS || stat == A_CON || stat == A_DEX) dummy += 1;
			}
			else if(CHECK_FAIRY_TYPE == 30 && !dun_level && turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
			{
				dummy += 1;
			}
			else if(CHECK_FAIRY_TYPE == 39 && have_flag(f_ptr->flags, FF_WATER))
			{
				if (stat == A_STR || stat == A_CON) dummy += 2;
			}
			else if(CHECK_FAIRY_TYPE == 43 && have_flag(f_ptr->flags, FF_TREE))
			{
				if (stat == A_STR || stat == A_WIS || stat == A_CON) dummy += 1;
			}
			//�E��
			if(p_ptr->pclass == CLASS_WAKASAGI && have_flag(f_ptr->flags, FF_WATER)) dummy += 1 + p_ptr->lev / 11;
			else if(p_ptr->pclass == CLASS_MURASA && have_flag(f_ptr->flags, FF_WATER)) dummy += 2;

			if(CHECK_FAIRY_TYPE == 46 && p_ptr->image)
			{
				dummy += 3;
			}

			//�o�C�h�זE�ɐN�H�����ƑS�\��+5
			if(p_ptr->muta3 & MUT3_BYDO && stat != A_CHR) dummy += 5;
			if(p_ptr->muta3 & MUT3_BYDO && stat == A_CHR) dummy -= 50;

			if (p_ptr->muta4 & MUT4_GHOST_OTTER) dummy -= 1;

			//�A�������V���t�ɂȂ�Ɣ\�̓_�E��
			if(p_ptr->muta2 & MUT2_ALCOHOL && p_ptr->alcohol == DRANK_0) dummy -= 3;

			///mod150110 �򂪔G�ꂽ��p�����[�^�_�E��
			if(p_ptr->pclass == CLASS_CHEN && p_ptr->magic_num1[0]) dummy -= 3;

			/*:::Hack- ���P�̖{�̂����ꂽ��p�����[�^�_�E��*/
			if(p_ptr->pclass == CLASS_KOGASA) 
			{
				if( inventory[INVEN_RARM].curse_flags & TRC_HEAVY_CURSE) dummy -= 3;
				else if( inventory[INVEN_RARM].curse_flags & TRC_CURSED) dummy -= 1;
			}

			if (stat == A_STR)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;

				if (p_ptr->muta3 & MUT3_HYPER_STR) dummy += 4;
				if (p_ptr->muta3 & MUT3_PUNY) dummy -= 4;
				if (p_ptr->tsuyoshi) dummy += 4;

				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy += 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_JELLYFISH) dummy -= 4;
				if (p_ptr->muta4 & MUT4_GHOST_COW) dummy += 4;

			}
			else if (stat == A_INT)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;


				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_FEVER) dummy -= 2;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
				if (p_ptr->muta2 & MUT2_HARDHEAD) dummy -= 1;

				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy += 2;

			}
			else if (stat == A_WIS)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;

				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
				if (p_ptr->muta2 & MUT2_HARDHEAD) dummy += 1;

				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy += 2;
			}


			else if (stat == A_DEX)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;

				if (p_ptr->muta3 & MUT3_FEVER) dummy -= 2;
				//if (p_ptr->muta3 & MUT3_IRON_SKIN) dummy -= 1;
				if (p_ptr->muta3 & MUT3_LIMBER) dummy += 3;
				if (p_ptr->muta3 & MUT3_ARTHRITIS) dummy -= 3;
				if (p_ptr->muta2 & MUT2_TAIL) dummy += 4;
				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy += 2;

			}
			else if (stat == A_CON)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;

				if (p_ptr->muta3 & MUT3_RESILIENT) dummy += 4;
				if (p_ptr->muta3 & MUT3_ALBINO) dummy -= 4;
				if (p_ptr->tsuyoshi) dummy += 4;
				if(p_ptr->asthma >= ASTHMA_4) dummy -= 4;
				if (p_ptr->muta4 & MUT4_GHOST_WOLF) dummy += 2;
				if (p_ptr->muta4 & MUT4_GHOST_EAGLE) dummy -= 2;
				if (p_ptr->muta4 & MUT4_GHOST_COW) dummy += 4;

			}
			else if (stat == A_CHR)
			{
				if (REF_BROKEN_VAL >= BROKEN_3) dummy -= 7;
				else if (REF_BROKEN_VAL >= BROKEN_2) dummy -= 4;
				else if (REF_BROKEN_VAL >= BROKEN_1) dummy -= 3;
				else if (REF_BROKEN_VAL) dummy -= 2;

				if (p_ptr->muta3 & MUT3_BLANK_FAC) dummy -= 3;
			//	if (p_ptr->muta3 & MUT3_SCALES) dummy -= 1;
				//if (p_ptr->muta3 & MUT3_WART_SKIN) dummy -= 2;
				//if (p_ptr->muta3 & MUT3_ILL_NORM) dummy = 0;
				if (p_ptr->muta3 & (MUT3_ACID_SCALES | MUT3_ELEC_SCALES | MUT3_FIRE_SCALES | MUT3_COLD_SCALES | MUT3_POIS_SCALES)) dummy -= 2;

				if (p_ptr->muta4 & MUT4_GHOST_CHICKEN) dummy += 4;
			}




			/* Boost */
			if (dummy)
			{
				/* Default */
				c = '*';

				/* Good */
				if (dummy > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (dummy < 10) c = '0' + dummy;
				}

				/* Bad */
				if (dummy < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (dummy > -10) c = '0' - dummy;
				}
			}
		}


		/* Sustain */
		if (have_flag(flgs, stat + TR_SUST_STR))
		{
			/* Dark green "s" */
			a = TERM_GREEN;
			c = 's';
		}




		/* Dump */
		Term_putch(col, row + stat+1, a, c);
	}
}


/*
 * Display the character on the screen (various modes)
 *
 * The top one and bottom two lines are left blank.
 *
 * Mode 0 = standard display with skills
 * Mode 1 = standard display with history
 * Mode 2 = summary of various things
 * Mode 3 = summary of various things (part 2)
 * Mode 4 = mutations
 */
/*:::���̏���\������BC�R�}���h�E�E�B���h�E�I�v�V�����E�Q�[���������_���v�@�ڍז���*/
///sys ���̏��\���@�d�v
void display_player(int mode)
{
	int i;

	char	buf[80];
	char	tmp[64];


	/* XXX XXX XXX */
	/*:::C�R�}���h�œˑR�ψق�\������I�v�V������ON�̂Ƃ�?�@C�R�}���h�ł�h����������mode�����Z����邽�߂�������ė]����v�Z����炵��*/
	if ((p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4) && display_mutations)
		mode = (mode % 5);
	else
		mode = (mode % 4);

	/* Erase screen */
	clear_from(0);

	/* Standard */
	if ((mode == 0) || (mode == 1))
	{
		/* Name, Sex, Race, Class */
#ifdef JP
	if(p_ptr->psex == SEX_MALE)
		sprintf(tmp, "%s%s%s", ap_ptr->title, ap_ptr->no == 1 ? "��":"", player_name);
	else
		sprintf(tmp, "%s%s%s", ap_ptr->f_title, ap_ptr->no == 1 ? "��":"", player_name);
#else
		sprintf(tmp, "%s %s", ap_ptr->title, player_name);
#endif

		display_player_one_line(ENTRY_NAME, tmp, TERM_L_BLUE);
		display_player_one_line(ENTRY_SEX, sp_ptr->title, TERM_L_BLUE);
		///mod150103 �푰���L�q�@��ǐ_�l��p�����ǉ�
		if(prace_is_(RACE_STRAYGOD))
			display_player_one_line(ENTRY_RACE, deity_table[p_ptr->race_multipur_val[3]].deity_name, TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_RACE, (p_ptr->mimic_form ? mimic_info[p_ptr->mimic_form].title : rp_ptr->title), TERM_L_BLUE);
	//	display_player_one_line(ENTRY_CLASS, cp_ptr->title, TERM_L_BLUE);
		///mod140206 �E�Ɩ��ύX�Ή�
		display_player_one_line(ENTRY_CLASS, player_class_name, TERM_L_BLUE);

		if (p_ptr->realm1)
		{
			if (p_ptr->realm2)
				sprintf(tmp, "%s, %s", realm_names[p_ptr->realm1], realm_names[p_ptr->realm2]);
			else
				strcpy(tmp, realm_names[p_ptr->realm1]);

			if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
				display_player_one_line(ENTRY_REALM, tmp, TERM_L_DARK);
			else
				display_player_one_line(ENTRY_REALM, tmp, TERM_L_BLUE);
		}
		///sys muta class �J�I�X�p�g�����̋L�q
		//if ((p_ptr->pclass == CLASS_CHAOS_WARRIOR) || (p_ptr->muta2 & MUT2_CHAOS_GIFT))
		if(IS_METAMORPHOSIS)
			display_player_one_line(ENTRY_METAMORPHOSIS, extra_mon_name, TERM_L_BLUE);

		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
			display_player_one_line(ENTRY_PATRON, chaos_patrons[p_ptr->chaos_patron], TERM_L_BLUE);
		//v1.1.70 ������̏�������
		else if (p_ptr->prace == RACE_ANIMAL_GHOST)
		{
			display_player_one_line(ENTRY_ANIMAL_GHOST_STRIFE, animal_ghost_strife_names[p_ptr->race_multipur_val[0]], TERM_L_BLUE);
		}

		//v1.1.15 �w�J�[�e�B�A�̍��̑̂��ǂꂩ�\��
		if(p_ptr->pclass == CLASS_HECATIA)
		{
			cptr hecatia_body_desc[] = {"�يE","�n��","��"};
			display_player_one_line(ENTRY_HECATIA_BODY, hecatia_body_desc[p_ptr->magic_num2[0]], TERM_L_BLUE);

		}

		/* Age, Height, Weight, Social */
		/* �g���̓Z���`���[�g���ɁA�̏d�̓L���O�����ɕύX���Ă���܂� */
		///sysdel �N��g���̏d
#ifdef JP
		///del131221 �N��̊i�Љ�I�n�ʂ�\�����Ȃ�
		//display_player_one_line(ENTRY_AGE, format("%d��" ,(int)p_ptr->age), TERM_L_BLUE);
		//display_player_one_line(ENTRY_HEIGHT, format("%dcm" ,(int)((p_ptr->ht*254)/100)), TERM_L_BLUE);
		//display_player_one_line(ENTRY_WEIGHT, format("%dkg" ,(int)((p_ptr->wt*4536)/10000)), TERM_L_BLUE);
		//display_player_one_line(ENTRY_SOCIAL, format("%d  " ,(int)p_ptr->sc), TERM_L_BLUE);
#else
		display_player_one_line(ENTRY_AGE, format("%d" ,(int)p_ptr->age), TERM_L_BLUE);
		display_player_one_line(ENTRY_HEIGHT, format("%d" ,(int)p_ptr->ht), TERM_L_BLUE);
		display_player_one_line(ENTRY_WEIGHT, format("%d" ,(int)p_ptr->wt), TERM_L_BLUE);
		display_player_one_line(ENTRY_SOCIAL, format("%d" ,(int)p_ptr->sc), TERM_L_BLUE);
#endif
		///del131229 ������\�����Ȃ�
		//display_player_one_line(ENTRY_ALIGN, format("%s" ,your_alignment()), TERM_L_BLUE);

		
	if(difficulty == DIFFICULTY_EASY)	display_player_one_line(ENTRY_SOCIAL, format("%s" ,"EASY"), TERM_L_GREEN);
	if(difficulty == DIFFICULTY_NORMAL) display_player_one_line(ENTRY_SOCIAL, format("%s" ,"NORMAL"), TERM_L_WHITE);
	if(difficulty == DIFFICULTY_HARD)	display_player_one_line(ENTRY_SOCIAL, format("%s" ,"HARD"), TERM_L_BLUE);
	if(difficulty == DIFFICULTY_LUNATIC)display_player_one_line(ENTRY_SOCIAL, format("%s" ,"LUNATIC"), TERM_VIOLET);
	if(difficulty == DIFFICULTY_EXTRA)	display_player_one_line(ENTRY_SOCIAL, format("%s" ,"EXTRA"), TERM_L_RED);
	//�X�R�A�\���@�L�����N�^�[���C�N���ɂ͕\�����Ȃ�
	//v1.1.53b �X�R�A���40���Ɋg�� lu��unsigned long
	if(character_generated) display_player_one_line(ENTRY_ALIGN, format("%lu" ,total_points_new(FALSE)), TERM_L_WHITE);


		///mod140104 �p�����[�^����ƕ\�L��ύX
		///mod140326 �p�����[�^����˔j�����ɑΉ�
		/* Display the stats */
		for (i = 0; i < 6; i++)
		{
			/* Special treatment of "injured" stats */
			if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
			{
				int value;

				/* Use lowercase stat name */
				put_str(stat_names_reduced[i], 3 + i, 53);

				/* Get the current stat */
				//value = p_ptr->stat_use[i];
				value = p_ptr->stat_ind[i]+3;

				/* Obtain the current stat (modified) */
				//cnv_stat(value, buf);
				//�\�͒ቺ�������܂����E�˔j��50�̂Ƃ�
				if(value == STAT_HYPERMAX)
				{
					sprintf(buf,"   *%2d",value);
					/* Display the current stat (modified) */
					c_put_str(TERM_L_BLUE, buf, 3 + i, 60);
				}
				//�\�͒ቺ�������܂����E�˔j�̂Ƃ�
				else if(value > STAT_ADD_MAX)
				{
					sprintf(buf,"   !%2d",value);
					/* Display the current stat (modified) */
					c_put_str(TERM_L_BLUE, buf, 3 + i, 60);
					value = p_ptr->stat_ind[i]+3 + (p_ptr->stat_max[i] - p_ptr->stat_cur[i]);
					if(value == STAT_HYPERMAX) sprintf(buf,"   *%2d",value);
					else sprintf(buf,"   !%2d",value);
					c_put_str(TERM_L_BLUE, buf, 3 + i, 67);

				}
				//�ʏ�̔\�͒ቺ��
				else 
				{
					if(value == STAT_ADD_MAX) sprintf(buf,"   !%2d",value);
					else sprintf(buf,"    %2d",value);
					/* Display the current stat (modified) */
					c_put_str(TERM_YELLOW, buf, 3 + i, 60);
					value = p_ptr->stat_ind[i]+3 + (p_ptr->stat_max[i] - p_ptr->stat_cur[i]);
					if(value > STAT_ADD_MAX)
					{
						value = STAT_ADD_MAX; //pend ���E�˔j���Ō��ݒl40�����ɂȂ������̕\��������������������Ȃ������u
						sprintf(buf,"   !%2d",value);
						c_put_str(TERM_L_GREEN, buf, 3 + i, 67);
					}
					else
					{
						sprintf(buf,"    %2d",value);
						c_put_str(TERM_L_GREEN, buf, 3 + i, 67);
					}
				}
				/* Display the current stat (modified) */
				//c_put_str(TERM_YELLOW, buf, 3 + i, 60);

				/* Acquire the max stat */
				//value = p_ptr->stat_top[i];

				/* Obtain the maximum stat (modified) */
				//cnv_stat(value, buf);

				/* Display the maximum stat (modified) */
				//c_put_str(TERM_L_GREEN, buf, 3 + i, 67);
			}

			/* Normal treatment of "normal" stats */
			else
			{
				int value;
				value = p_ptr->stat_ind[i]+3;
				/* Assume uppercase stat name */
				put_str(stat_names[i], 3 + i, 53);
				/* Obtain the current stat (modified) */
				//cnv_stat(p_ptr->stat_use[i], buf);
				if(value == STAT_HYPERMAX) sprintf(buf,"   *%2d",value);
				else if(value >= STAT_ADD_MAX) sprintf(buf,"   !%2d",value);
				else sprintf(buf,"    %2d",value);

				/* Display the current stat (modified) */
				if(value > STAT_ADD_MAX) c_put_str(TERM_L_BLUE, buf, 3 + i, 60);
				else c_put_str(TERM_L_GREEN, buf, 3 + i, 60);
			}

			if (p_ptr->stat_max[i] == p_ptr->stat_max_max[i])
			{
#ifdef JP
				//c_put_str(TERM_WHITE, "!", 3 + i, 58);
#else
				c_put_str(TERM_WHITE, "!", 3 + i, 58-2);
#endif
			}
		}
		/* Display "history" info */
		if (mode == 1)
		{
			char statmsg[1000];

#ifdef JP
			put_str("(�L�����N�^�[�̐�������)", 11, 25);
#else
			put_str("(Character Background)", 11, 25);
#endif

			for (i = 0; i < 4; i++)
			{
				put_str(p_ptr->history[i], i + 12, 10);
			}

			*statmsg = '\0';

			if (p_ptr->is_dead)
			{
				if (p_ptr->total_winner)
				{
#ifdef JP
					if(streq(p_ptr->died_from, "hourai"))
					{
						sprintf(statmsg, "�c���Ȃ��͉i���𐶂���H���l�ƂȂ����B");
					}
					else if (streq(p_ptr->died_from, "true_loser"))
					{
						sprintf(statmsg, "�c���Ȃ��͒N��l���҂̂��Ȃ����E�������炵���B");
					}
					else if(p_ptr->pclass == CLASS_OUTSIDER)
					{
						if (streq(p_ptr->died_from, "triumph"))
							strcpy(statmsg, "�c���Ȃ��͌��z���̉p�Y�Ƃ��ĊO�E�֋A�҂����B");
						else
							sprintf(statmsg, "�c���Ȃ��͌��z���ɉi�Z�����B");
					}
					else
						sprintf(statmsg, "�c���Ȃ��͏����̌���ނ����B");

//					sprintf(statmsg, "�c���Ȃ��͏����̌�%s�����B", streq(p_ptr->died_from, "Seppuku") ? "�ؕ�" : "����");
#else
					sprintf(statmsg, "...You %s after the winning.", streq(p_ptr->died_from, "Seppuku") ? "did Seppuku" : "retired from the adventure");
#endif
				}
				else if (streq(p_ptr->died_from, "escape"))
				{
					strcpy(statmsg, "�c���Ȃ��͌��z������E�o�����B");
				}
				else if (streq(p_ptr->died_from, "lost"))
				{
					strcpy(statmsg, "�c���Ȃ��͍ŏ����瑶�݂��Ȃ������B");
				}
				else if (streq(p_ptr->died_from, "mission_failed"))//�i�ԂŋP�邪�|���ꂽ���͌������_���v�o��
				{
					
					char m_name[80];
					int r_idx = p_ptr->magic_num1[0];
					if(!r_idx) sprintf(m_name,"���Ȃ��ɓ|���ꂽ�B");
					else if(r_idx < 0) sprintf(m_name,"�s���̎��̂ɂ��|�ꂽ�B");
					else if(IS_RANDOM_UNIQUE_IDX(r_idx))
						sprintf(m_name,"%s�ɓ|���ꂽ�B", random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]);
					else sprintf(m_name,"%s�ɓ|���ꂽ�B",r_name + r_info[r_idx].name);

					sprintf(statmsg, "�c�P���%s��%s",map_name(),m_name);
				}
				else if (!dun_level)
				{
#ifdef JP
					///msg131219 dead
					//sprintf(statmsg, "�c���Ȃ���%s��%s�ɎE���ꂽ�B", map_name(), p_ptr->died_from);
					sprintf(statmsg, "�c���Ȃ���%s��%s�ɓ|���ꂽ�B", map_name(), p_ptr->died_from);

#else
					sprintf(statmsg, "...You were killed by %s in %s.", p_ptr->died_from, map_name());
#endif
				}
				else if (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest))
				{
					/* Get the quest text */
					/* Bewere that INIT_ASSIGN resets the cur_num. */
					init_flags = INIT_NAME_ONLY;

					process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
					///msg131219 dead
					//sprintf(statmsg, "�c���Ȃ��́A�N�G�X�g�u%s�v��%s�ɎE���ꂽ�B", quest[p_ptr->inside_quest].name, p_ptr->died_from);
					sprintf(statmsg, "�c���Ȃ��́A�N�G�X�g�u%s�v��%s�ɓ|���ꂽ�B", quest[p_ptr->inside_quest].name, p_ptr->died_from);

#else
					sprintf(statmsg, "...You were killed by %s in the quest '%s'.", p_ptr->died_from, quest[p_ptr->inside_quest].name);
#endif
				}
				else if(INSIDE_EXTRA_QUEST)
				{
					sprintf(statmsg, "�c���Ȃ��́A%d�K�̃G�N�X�g���N�G�X�g����%s�ɓ|���ꂽ�B",dun_level, p_ptr->died_from);
				}
				else
				{
#ifdef JP
					///msg131219 dead
					//sprintf(statmsg, "�c���Ȃ��́A%s��%d�K��%s�ɎE���ꂽ�B", map_name(), dun_level, p_ptr->died_from);
					sprintf(statmsg, "�c���Ȃ��́A%s��%d�K��%s�ɓ|���ꂽ�B", map_name(), dun_level, p_ptr->died_from);
#else
					sprintf(statmsg, "...You were killed by %s on level %d of %s.", p_ptr->died_from, dun_level, map_name());
#endif
				}
			}
			else if (character_dungeon)
			{
				if (!dun_level)
				{
#ifdef JP
					sprintf(statmsg, "�c���Ȃ��͌��݁A %s �ɂ���B", map_name());
#else
					sprintf(statmsg, "...Now, you are in %s.", map_name());
#endif
				}
				else if (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest))
				{
					/* Clear the text */
					/* Must be done before doing INIT_SHOW_TEXT */
					for (i = 0; i < 10; i++)
					{
						quest_text[i][0] = '\0';
					}
					quest_text_line = 0;

					/* Get the quest text */
					init_flags = INIT_NAME_ONLY;

					process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
					sprintf(statmsg, "�c���Ȃ��͌��݁A �N�G�X�g�u%s�v�𐋍s�����B", quest[p_ptr->inside_quest].name);
#else
					sprintf(statmsg, "...Now, you are in the quest '%s'.", quest[p_ptr->inside_quest].name);
#endif
				}
				else if(INSIDE_EXTRA_QUEST)
				{
					sprintf(statmsg, "�c���Ȃ��́A%d�K�̃G�N�X�g���N�G�X�g�𐋍s�����B",dun_level);
				}
				else
				{
#ifdef JP
					sprintf(statmsg, "�c���Ȃ��͌��݁A %s �� %d �K�ŒT�����Ă���B", map_name(), dun_level);
#else
					sprintf(statmsg, "...Now, you are exploring level %d of %s.", dun_level, map_name());
#endif
				}
			}

			if (*statmsg)
			{
				char temp[64*2], *t;
				roff_to_buf(statmsg, 60, temp, sizeof(temp));
				t = temp;
				for(i=0 ; i<2 ; i++)
				{
					if(t[0]==0)
						break; 
					else
					{
						put_str(t, i + 5 + 12, 10);
						t += strlen(t)+1;
					}
				}
			}

		}

		/* Display "various" info */
		else
		{
			display_player_middle();
			display_player_various();
		}
	}

	/* Special */
	else if (mode == 2)
	{
		/* See "http://www.cs.berkeley.edu/~davidb/angband.html" */

		/* Dump the info */
		display_player_misc_info();
		display_player_stat_info();
		display_player_flag_info();
	}

	/* Special */
	else if (mode == 3)
	{
		display_player_other_flag_info();
	}

	else if (mode == 4)
	{
		do_cmd_knowledge_mutations();
	}
}


/*
 *
 */
static void dump_aux_display_player(FILE *fff)
{
	int x, y;
	byte a;
	char c;
	char		buf[1024];

	/* Display player */
	display_player(0);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
#ifdef JP
		fprintf(fff, "%s\n", buf);
#else
		fprintf(fff, "%s\n", buf);
#endif

	}

	/* Display history */
	display_player(1);

	/* Dump part of the screen */
	for (y = 10; y < 19; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 1) */
	display_player(2);

	/* Dump part of the screen */
	for (y = 2; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if (a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 2) */
	display_player(3);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if (a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");
}


/*
 *
 */
static void dump_aux_pet(FILE *fff)
{
	int i;
	bool pet = FALSE;
	bool pet_settings = FALSE;
	char pet_name[80];

	for (i = m_max - 1; i >= 1; i--)
	{
		monster_type *m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (!is_pet(m_ptr)) continue;
		pet_settings = TRUE;
		if (!m_ptr->nickname && (p_ptr->riding != i)) continue;
		if (!pet)
		{
#ifdef JP
			fprintf(fff, "\n\n  [��Ȕz��]\n\n");
#else
			fprintf(fff, "\n\n  [Leading Pets]\n\n");
#endif
			pet = TRUE;
		}
		monster_desc(pet_name, m_ptr, MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
		fprintf(fff, "%s\n", pet_name);
	}

	if (pet_settings)
	{
#ifdef JP
		fprintf(fff, "\n\n  [�z���ւ̖���]\n");
#else
		fprintf(fff, "\n\n  [Command for Pets]\n");
#endif

#ifdef JP
		fprintf(fff, "\n �h�A���J����:                       %s", (p_ptr->pet_extra_flags & PF_OPEN_DOORS) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Pets open doors:                    %s", (p_ptr->pet_extra_flags & PF_OPEN_DOORS) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n �A�C�e�����E��:                     %s", (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Pets pick up items:                 %s", (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n �e���|�[�g�n���@���g��:             %s", (p_ptr->pet_extra_flags & PF_TELEPORT) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow teleport:                     %s", (p_ptr->pet_extra_flags & PF_TELEPORT) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n �U�����@���g��:                     %s", (p_ptr->pet_extra_flags & PF_ATTACK_SPELL) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow cast attack spell:            %s", (p_ptr->pet_extra_flags & PF_ATTACK_SPELL) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n �������@���g��:                     %s", (p_ptr->pet_extra_flags & PF_SUMMON_SPELL) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow cast summon spell:            %s", (p_ptr->pet_extra_flags & PF_SUMMON_SPELL) ? "ON" : "OFF");
#endif

#ifdef JP
		fprintf(fff, "\n �v���C���[���������ޔ͈͖��@���g��: %s", (p_ptr->pet_extra_flags & PF_BALL_SPELL) ? "ON" : "OFF");
#else
		fprintf(fff, "\n Allow involve player in area spell: %s", (p_ptr->pet_extra_flags & PF_BALL_SPELL) ? "ON" : "OFF");
#endif

		fputc('\n', fff);
	}
}


/*
 *
 */
static void dump_aux_class_special(FILE *fff)
{
	if (p_ptr->pclass == CLASS_BLUE_MAGE)
	{
		int i = 0;
		int j = 0;
		int l1 = 0;
		int l2 = 0;
		int num = 0;
		int spellnum[MAX_MONSPELLS];
		s32b f4 = 0, f5 = 0, f6 = 0;
		char p[60][80];
		int col = 0;
		bool pcol = FALSE;

		for (i=0;i<60;i++) { p[i][0] = '\0'; }

#ifdef JP
		strcat(p[col], "\n\n  [�w�K�ς݂̐��@]\n");
#else
		strcat(p[col], "\n\n  [Learned Blue Magic]\n");
#endif


		for (j=1;j<6;j++)
		{
			col++;
			set_rf_masks(&f4, &f5, &f6, j);
			switch(j)
			{
				case MONSPELL_TYPE_BOLT:
#ifdef JP
					strcat(p[col], "\n     [�{���g�^]\n");
#else
					strcat(p[col], "\n     [Bolt  Type]\n");
#endif
					break;

				case MONSPELL_TYPE_BALL:
#ifdef JP
					strcat(p[col], "\n     [�{�[���^]\n");
#else
					strcat(p[col], "\n     [Ball  Type]\n");
#endif
					break;

				case MONSPELL_TYPE_BREATH:
#ifdef JP
					strcat(p[col], "\n     [�u���X�^]\n");
#else
					strcat(p[col], "\n     [  Breath  ]\n");
#endif
					break;

				case MONSPELL_TYPE_SUMMON:
#ifdef JP
					strcat(p[col], "\n     [�������@]\n");
#else
					strcat(p[col], "\n     [Summonning]\n");
#endif
					break;

				case MONSPELL_TYPE_OTHER:
#ifdef JP
					strcat(p[col], "\n     [ ���̑� ]\n");
#else
					strcat(p[col], "\n     [Other Type]\n");
#endif
					break;
			}

			for (i = 0, num = 0; i < 32; i++)
			{
				if ((0x00000001 << i) & f4) spellnum[num++] = i;
			}
			for (; i < 64; i++)
			{
				if ((0x00000001 << (i - 32)) & f5) spellnum[num++] = i;
			}
			for (; i < 96; i++)
			{
				if ((0x00000001 << (i - 64)) & f6) spellnum[num++] = i;
			}

			col++;
			pcol = FALSE;
			strcat(p[col], "       ");

			for (i = 0; i < num; i++)
			{
				if (p_ptr->magic_num2[spellnum[i]])
				{
					pcol = TRUE;
					/* Dump blue magic */
					l1 = strlen(p[col]);
					l2 = strlen(monster_powers_short[spellnum[i]]);
					if ((l1 + l2) >= 75)
					{
						strcat(p[col], "\n");
						col++;
						strcat(p[col], "       ");
					}
					strcat(p[col], monster_powers_short[spellnum[i]]);
					strcat(p[col], ", ");
				}
			}
			
			if (!pcol)
			{
#ifdef JP
				strcat(p[col], "�Ȃ�");
#else
				strcat(p[col], "None");
#endif
			}
			else
			{
				if (p[col][strlen(p[col])-2] == ',')
				{
					p[col][strlen(p[col])-2] = '\0';
				}
				else
				{
					p[col][strlen(p[col])-10] = '\0';
				}
			}
			
			strcat(p[col], "\n");
		}

		for (i=0;i<=col;i++)
		{
			fputs(p[i], fff);
		}
	}
	else if (p_ptr->pclass == CLASS_MAGIC_EATER || p_ptr->pclass == CLASS_SEIJA)
	{
		char s[EATER_EXT][MAX_NLEN];
		int tval, ext, k_idx;
		int i, magic_num;

		if (p_ptr->pclass == CLASS_SEIJA)
			fprintf(fff, "\n\n  [�B�����������@����]\n");
		else
			fprintf(fff, "\n\n  [��荞�񂾖��@����]\n");

		for (ext = 0; ext < 3; ext++)
		{
			int eat_num = 0;

			/* Dump an extent name */
			switch (ext)
			{
			case 0:
				tval = TV_STAFF;
#ifdef JP
				fprintf(fff, "\n[��]\n");
#else
				fprintf(fff, "\n[Staffs]\n");
#endif
				break;
			case 1:
				tval = TV_WAND;
#ifdef JP
				fprintf(fff, "\n[���@�_]\n");
#else
				fprintf(fff, "\n[Wands]\n");
#endif
				break;
			case 2:
				tval = TV_ROD;
#ifdef JP
				fprintf(fff, "\n[���b�h]\n");
#else
				fprintf(fff, "\n[Rods]\n");
#endif
				break;
			}

			/* Get magic device names that were eaten */
			for (i = 0; i < EATER_EXT; i++)
			{
				int idx = EATER_EXT * ext + i;

				magic_num = p_ptr->magic_num2[idx];
				if (!magic_num) continue;

				k_idx = lookup_kind(tval, i);
				if (!k_idx) continue;
				sprintf(s[eat_num], "%23s (%2d)", (k_name + k_info[k_idx].name), magic_num);
				eat_num++;
			}

			/* Dump magic devices in this extent */
			if (eat_num > 0)
			{
				for (i = 0; i < eat_num; i++)
				{
					fputs(s[i], fff);
					if (i % 3 < 2) fputs("    ", fff);
					else fputs("\n", fff);
				}

				if (i % 3 > 0) fputs("\n", fff);
			}
			else /* Not found */
			{
#ifdef JP
				fputs("  (�Ȃ�)\n", fff);
#else
				fputs("  (none)\n", fff);
#endif
			}
		}
	}
	//v1.1.52 俎q��p���i�̓��Z�����X�g�A�b�v
	//v2.0.4 ���{�ۂ̌��ʂ��ǉ�
	else if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO) || p_ptr->pclass == CLASS_MISUMARU)
	{
		int i,imax;

		if (p_ptr->pclass == CLASS_MISUMARU)
		{
			fprintf(fff, "\n\n  [���ʂ̗�]\n");
			imax = INVEN2_MAGATAMA_NUM_MAX;
		}
		else
		{
			fprintf(fff, "\n\n  [��˂̗�]\n");
			imax = 8;
		}

		//���Z�ő吔���[�v
		for (i = 0; i < imax; i++)
		{
			char buf[256] = "";
			int quark_idx=0;
			int art_idx,typ,rad,base,dice,sides,cost,xtra1;

			//���{�ۂ͑������Ă�����ʂ���Z�̃p�����[�^���擾
			if (p_ptr->pclass == CLASS_MISUMARU)
			{
				char o_name[128];
				object_type *o_ptr = &inven_add[i];

				if (!o_ptr->k_idx || o_ptr->tval != TV_SPELLCARD) continue;

				art_idx = o_ptr->pval;
				typ = o_ptr->xtra1;
				rad = o_ptr->xtra2;
				base = o_ptr->ac;
				dice = o_ptr->dd;
				sides = o_ptr->to_a;
				cost = o_ptr->to_h;
				xtra1 = o_ptr->xtra4;
				object_desc(o_name, o_ptr, OD_NAME_ONLY);
				fprintf(fff, " %s(%s)\n", o_name, nameless_skill_type_desc[art_idx]); //���̂Ɠ��Z���

			}
			//�ق�(����俎q)��p_ptr->magic_num1[]����Z�̃p�����[�^���擾
			else
			{
				quark_idx = JUNKO_ARTS_PARAM(i, JKAP_QUARK_IDX);
				art_idx = JUNKO_ARTS_PARAM(i, JKAP_JKF1);
				typ = JUNKO_ARTS_PARAM(i, JKAP_GF);
				rad = JUNKO_ARTS_PARAM(i, JKAP_RAD);
				base = JUNKO_ARTS_PARAM(i, JKAP_BASE);
				dice = JUNKO_ARTS_PARAM(i, JKAP_DICE);
				sides = JUNKO_ARTS_PARAM(i, JKAP_SIDES);
				cost = JUNKO_ARTS_PARAM(i, JKAP_COST);
				xtra1 = JUNKO_ARTS_PARAM(i, JKAP_XTRA_VAL1);
				if (!art_idx) break;
				fprintf(fff, "    %s(%s)\n", quark_str(quark_idx), nameless_skill_type_desc[art_idx]); //���̂Ɠ��Z���
			}

			fprintf(fff, "      ����MP:%-3d", cost);
			if (typ && art_idx != JKF1_ADD_ESSENCE1 && art_idx != JKF1_ADD_ESSENCE2)
				fprintf(fff, "  ����:%-10s", gf_desc_name[typ]);
			if (rad>0)
				fprintf(fff,"  ���a:%-2d", ABS(rad));

			if (typ == GF_STONE_WALL || typ == GF_MAKE_DOOR || typ == GF_MAKE_TREE)
			{	} //�ǐ����Ȃǂ͓����I�Ƀ_���[�W�l���L�^����Ă��邪�g��Ȃ��̂ŕ\�����Ȃ�
			else if (art_idx == JKF1_ATTACK_BREATH)
				fprintf(fff, "  �З�:����HP��%d%%", xtra1);
			else if (dice && sides && base)
				fprintf(fff, "  �З�/����:%dd%d+%d", dice, sides, base);
			else if (dice && sides)
				fprintf(fff, "  �З�/����:%dd%d", dice, sides);
			else if (base)
				fprintf(fff, "  �З�/����:%d",base);

			fprintf(fff, "\n");

		}

	}
	//v1.1.81 �e��������
	else if (p_ptr->pclass == CLASS_RESEARCHER)
	{
		int i,j;
		int monspell_kind_type[6] = { NEW_MSPELL_TYPE_BOLT_BEAM_ROCKET ,NEW_MSPELL_TYPE_BALL ,NEW_MSPELL_TYPE_BREATH ,NEW_MSPELL_TYPE_OTHER_ATTACK ,NEW_MSPELL_TYPE_SUMMMON ,NEW_MSPELL_TYPE_OTHER };
		cptr monspell_kind_desc[6] = { "(�{���g/�r�[��/���P�b�g)","(�{�[��)","(�u���X)","(���̑��̍U��)","(����)","(���̑�)" };
		bool flag_exist;
		fprintf(fff, "\n\n  [�K���ς݂̓��Z]\n");

		for (i = 0; i < 6; i++)
		{
			flag_exist = FALSE;
			fprintf(fff, "    %s\n",monspell_kind_desc[i]);

			for (j = 1; j <= MAX_MONSPELLS2; j++)
			{
				int spell_exp;

				//�{���g,�{�[��,�u���X�Ȃǂ̕��ނ�����Ȃ���΃p�X
				if (monspell_list2[j].monspell_type != monspell_kind_type[i]) continue;
				//���K���Ȃ�p�X 
				if (!check_monspell_learned(j)) continue;

				flag_exist = TRUE;
				fprintf(fff, "      %s\n", monspell_list2[j].name);

			}
			if(!flag_exist)
				fprintf(fff, "      (�Ȃ�)\n");

		}

	}




}


/*
 *
 */
static void dump_aux_quest(FILE *fff)
{
	int i;
	int *quest_num;
	int dummy;


#ifdef JP
	fprintf(fff, "\n\n  [�N�G�X�g���]\n");
#else
	fprintf(fff, "\n\n  [Quest Information]\n");
#endif

	/* Allocate Memory */
	C_MAKE(quest_num, max_quests, int);

	/* Sort by compete level */
	for (i = 1; i < max_quests; i++) quest_num[i] = i;
	ang_sort_comp = ang_sort_comp_quest_num;
	ang_sort_swap = ang_sort_swap_quest_num;
	ang_sort(quest_num, &dummy, max_quests);

	/* Dump Quest Information */
	fputc('\n', fff);
	do_cmd_knowledge_quests_completed(fff, quest_num);
	fputc('\n', fff);
	do_cmd_knowledge_quests_failed(fff, quest_num);
	fputc('\n', fff);

	/* Free Memory */
	C_KILL(quest_num, max_quests, int);
}


/*
 *
 */
static void dump_aux_last_message(FILE *fff)
{
	if (p_ptr->is_dead)
	{
		if (!p_ptr->total_winner)
		{
			int i;

#ifdef JP
			fprintf(fff, "\n  [�|��钼�O�ɋN����������]\n\n");
#else
			fprintf(fff, "\n  [Last Messages]\n\n");
#endif
			for (i = MIN(message_num(), 30); i >= 0; i--)
			{
				fprintf(fff,"> %s\n",message_str((s16b)i));
			}
			fputc('\n', fff);
		}

		/* Hack -- *Winning* message */
		else if (p_ptr->last_message)
		{
#ifdef JP
			fprintf(fff, "\n  [*����*���b�Z�[�W]\n\n");
#else
			fprintf(fff, "\n  [*Winning* Message]\n\n");
#endif
			fprintf(fff,"  %s\n", p_ptr->last_message);
			fputc('\n', fff);
		}
	}
}


/*
 *
 */
static void dump_aux_recall(FILE *fff)
{
	int y;

#ifdef JP
	fprintf(fff, "\n  [�A�ҏꏊ]\n\n");
#else
	fprintf(fff, "\n  [Recall Depth]\n\n");
#endif

	for (y = 1; y < max_d_idx; y++)
	{
		bool seiha = FALSE;

		if(flag_dungeon_complete[y]) seiha = TRUE;

		if (!d_info[y].maxdepth) continue;
		if (!max_dlv[y]) continue;
#if 0
		if (d_info[y].final_guardian)
		{
			if (!r_info[d_info[y].final_guardian].max_num) seiha = TRUE;
		}
		/*:::�I�x�����ƃT�[�y���g�|���Ă��A�҂��Ő[�łȂ��Ɛ��e�}�[�N�����Ȃ������̂ŏC��*/
		else if(y == DUNGEON_ANGBAND && quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED) seiha = TRUE;
		else if(y == DUNGEON_CHAOS && quest[QUEST_SERPENT].status == QUEST_STATUS_FINISHED) seiha = TRUE;
		else if (max_dlv[y] == d_info[y].maxdepth) seiha = TRUE;
#endif

#ifdef JP
		fprintf(fff, "   %c%-20s: %3d �K\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#else
		fprintf(fff, "   %c%-16s: level %3d\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#endif
	}
}


/*
 *
 */
static void dump_aux_options(FILE *fff)
{
#ifdef JP
	fprintf(fff, "\n  [�I�v�V�����ݒ�]\n");
#else
	fprintf(fff, "\n  [Option Settings]\n");
#endif


	if (preserve_mode)
#ifdef JP
		fprintf(fff, "\n �ۑ����[�h:         ON");
#else
		fprintf(fff, "\n Preserve Mode:      ON");
#endif

	else
#ifdef JP
		fprintf(fff, "\n �ۑ����[�h:         OFF");
#else
		fprintf(fff, "\n Preserve Mode:      OFF");
#endif


	if (ironman_small_levels)
#ifdef JP
		fprintf(fff, "\n �������_���W����:   ALWAYS");
#else
		fprintf(fff, "\n Small Levels:       ALWAYS");
#endif

	else if (always_small_levels)
#ifdef JP
		fprintf(fff, "\n �������_���W����:   ON");
#else
		fprintf(fff, "\n Small Levels:       ON");
#endif

	else if (small_levels)
#ifdef JP
		fprintf(fff, "\n �������_���W����:   ENABLED");
#else
		fprintf(fff, "\n Small Levels:       ENABLED");
#endif

	else
#ifdef JP
		fprintf(fff, "\n �������_���W����:   OFF");
#else
		fprintf(fff, "\n Small Levels:       OFF");
#endif


	if (vanilla_town)
#ifdef JP
		fprintf(fff, "\n ���c�̒��̂�:       ON");
#else
		fprintf(fff, "\n Vanilla Town:       ON");
#endif

	else if (lite_town)
#ifdef JP
		fprintf(fff, "\n ���K�͂Ȓ�:         ON");
#else
		fprintf(fff, "\n Lite Town:          ON");
#endif


	if (ironman_shops)
#ifdef JP
		fprintf(fff, "\n �X�Ȃ�:             ON");
#else
		fprintf(fff, "\n No Shops:           ON");
#endif

	if (ironman_no_fixed_art)
		fprintf(fff, "\n �����o�����Ȃ�:     ON");

	if (no_capture_book)
		fprintf(fff, "\n �d���{���o�����Ȃ�: ON");


	if (ironman_downward)
#ifdef JP
		fprintf(fff, "\n �K�i���オ��Ȃ�:   ON");
#else
		fprintf(fff, "\n Diving Only:        ON");
#endif


	if (ironman_rooms)
#ifdef JP
		fprintf(fff, "\n ���ʂłȂ�����:     ON");
#else
		fprintf(fff, "\n Unusual Rooms:      ON");
#endif


	if (ironman_nightmare)
#ifdef JP
		fprintf(fff, "\n �������[�h:         ON");
#else
		fprintf(fff, "\n Nightmare Mode:     ON");
#endif

	if (ironman_no_bonus)
		fprintf(fff, "\n �{�[�i�X�|�C���g:   0");
	else if (birth_bonus_10)
		fprintf(fff, "\n �{�[�i�X�|�C���g:   10");
	else if (birth_bonus_20)
		fprintf(fff, "\n �{�[�i�X�|�C���g:   20");


	if (ironman_empty_levels)
#ifdef JP
		fprintf(fff, "\n �A���[�i:           ALWAYS");
#else
		fprintf(fff, "\n Arena Levels:       ALWAYS");
#endif

	else if (empty_levels)
#ifdef JP
		fprintf(fff, "\n �A���[�i:           ENABLED");
#else
		fprintf(fff, "\n Arena Levels:       ENABLED");
#endif

	else
#ifdef JP
		fprintf(fff, "\n �A���[�i:           OFF");
#else
		fprintf(fff, "\n Arena Levels:       OFF");
#endif


	fputc('\n', fff);

	if (p_ptr->noscore)
#ifdef JP
		fprintf(fff, "\n �����s���Ȃ��Ƃ����Ă��܂��Ă��܂��B\n");
#else
		fprintf(fff, "\n You have done something illegal.\n");
#endif

	fputc('\n', fff);
}


//v1.1.56 �A���[�i����𖳌�������B�@p_ptr->arena_number��kourindou_number�ɕς���̂Ɏז��Ȃ���
#if 0
static void dump_aux_arena(FILE *fff)
{
	if (lite_town || vanilla_town) return;

	if (p_ptr->arena_number < 0)
	{
		if (p_ptr->arena_number <= ARENA_DEFEATED_OLD_VER)
		{
#ifdef JP
			fprintf(fff, "\n ���Z��: �s�k\n");
#else
			fprintf(fff, "\n Arena: Defeated\n");
#endif
		}
		else
		{
#ifdef JP
			fprintf(fff, "\n ���Z��: %d����%s�̑O�ɔs�k\n", -p_ptr->arena_number,
				r_name + r_info[arena_info[-1 - p_ptr->arena_number].r_idx].name);
#else
			fprintf(fff, "\n Arena: Defeated by %s in the %d%s fight\n",
				r_name + r_info[arena_info[-1 - p_ptr->arena_number].r_idx].name,
				-p_ptr->arena_number, get_ordinal_number_suffix(-p_ptr->arena_number));
#endif
		}
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS + 2)
	{
#ifdef JP
		fprintf(fff, "\n ���Z��: �^�̃`�����s�I��\n");
#else
		fprintf(fff, "\n Arena: True Champion\n");
#endif
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS - 1)
	{
#ifdef JP
		fprintf(fff, "\n ���Z��: �`�����s�I��\n");
#else
		fprintf(fff, "\n Arena: Champion\n");
#endif
	}
	else
	{
#ifdef JP
		fprintf(fff, "\n ���Z��: %2d��\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number));
#else
		fprintf(fff, "\n Arena: %2d Victor%s\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number), (p_ptr->arena_number > 1) ? "ies" : "y");
#endif
	}

	fprintf(fff, "\n");
}
#endif

/*
 *
 */
static void dump_aux_monsters(FILE *fff)
{
	/* Monsters slain */

	int k;
	long uniq_total = 0;
	long norm_total = 0;
	s16b *who;

	/* Sort by monster level */
	u16b why = 2;

#ifdef JP
	fprintf(fff, "\n  [�|���������X�^�[]\n\n");
#else
	fprintf(fff, "\n  [Defeated Monsters]\n\n");
#endif

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Count monster kills */
	for (k = 1; k < max_r_idx; k++)
	{
		monster_race *r_ptr = &r_info[k];

		/* Ignore unused index */
 		if (!r_ptr->name) continue;

		/* Unique monsters */
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			bool dead = (r_ptr->max_num == 0);
			if (dead)
			{
				norm_total++;

				/* Add a unique monster to the list */
				who[uniq_total++] = k;
			}
		}

		/* Normal monsters */
		else
		{
			if (r_ptr->r_pkills > 0)
			{
				norm_total += r_ptr->r_pkills;
			}
		}
	}


	/* No monsters is defeated */
	if (norm_total < 1)
	{
#ifdef JP
		fprintf(fff,"�܂��G��|���Ă��܂���B\n");
#else
		fprintf(fff,"You have defeated no enemies yet.\n");
#endif
	}

	/* Defeated more than one normal monsters */
	else if (uniq_total == 0)
	{
#ifdef JP
		fprintf(fff,"%ld�̂̓G��|���Ă��܂��B\n", norm_total);
#else
		fprintf(fff,"You have defeated %ld %s.\n", norm_total, norm_total == 1 ? "enemy" : "enemies");
#endif
	}

	/* Defeated more than one unique monsters */
	else /* if (uniq_total > 0) */
	{
#ifdef JP
		fprintf(fff, "%ld�̂̃��j�[�N�E�����X�^�[���܂ށA���v%ld�̂̓G��|���Ă��܂��B\n", uniq_total, norm_total); 
#else
		fprintf(fff, "You have defeated %ld %s including %ld unique monster%s in total.\n", norm_total, norm_total == 1 ? "enemy" : "enemies", uniq_total, (uniq_total == 1 ? "" : "s"));
#endif


		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array by dungeon depth of monsters */
		ang_sort(who, &why, uniq_total);

#ifdef JP
		fprintf(fff, "\n�s���%ld�̂̃��j�[�N�E�����X�^�[�t\n", MIN(uniq_total, 10));
#else
		fprintf(fff, "\n< Unique monsters top %ld >\n", MIN(uniq_total, 10));
#endif

		/* Print top 10 */
		for (k = uniq_total - 1; k >= 0 && k >= uniq_total - 10; k--)
		{
			monster_race *r_ptr = &r_info[who[k]];

#ifdef JP
			fprintf(fff, "  %-50s (���x��%3d)\n", (r_name + r_ptr->name), r_ptr->level); 
#else
			fprintf(fff, "  %-40s (level %3d)\n", (r_name + r_ptr->name), r_ptr->level); 
#endif
		}

	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, s16b);
}


/*
 *
 */
static void dump_aux_race_history(FILE *fff)
{
	///mod150104 ��ǐ_�l�̗���
	if(p_ptr->prace == RACE_STRAYGOD)
	{
		int rank1=0,rank2=0,rank3=0;
		rank2 = p_ptr->race_multipur_val[4] / 128;

		fprintf(fff, "\n  [�_������]\n");
		if(rank2)
		{
			rank3 = p_ptr->race_multipur_val[3];
			rank1 = p_ptr->race_multipur_val[4] % 128;
			fprintf(fff, "\n ���Ȃ��͂���%s�������B", deity_table[rank1].deity_name);
			fprintf(fff, "\n �₪��%s�ƂȂ�A", deity_table[rank2].deity_name);
			fprintf(fff, "\n ������%s�ƂȂ����B\n", deity_table[rank3].deity_name);
		}
		else if(p_ptr->race_multipur_val[4])
		{
			rank1 = p_ptr->race_multipur_val[4];
			rank2 = p_ptr->race_multipur_val[3];
			fprintf(fff, "\n ���Ȃ��͂���%s�������B", deity_table[rank1].deity_name);
			fprintf(fff, "\n ���̌�%s�ƂȂ����B\n", deity_table[rank2].deity_name);
		}
		else
		{
			rank1 = p_ptr->race_multipur_val[3];
			fprintf(fff, "\n ���Ȃ���%s���B\n", deity_table[rank1].deity_name);
		}

	}

	else if (p_ptr->old_race1 || p_ptr->old_race2)
	{
		int i;

#ifdef JP
		fprintf(fff, "\n\n ���Ȃ���%s�Ƃ��Đ��܂ꂽ�B", race_info[p_ptr->start_race].title);
#else
		fprintf(fff, "\n\n You were born as %s.", race_info[p_ptr->start_race].title);
#endif
		for (i = 0; i < MAX_RACES; i++)
		{
			if (p_ptr->start_race == i) continue;
			if (i < 32)
			{
				if (!(p_ptr->old_race1 & 1L << i)) continue;
			}
			else
			{
				if (!(p_ptr->old_race2 & 1L << (i-32))) continue;
			}
#ifdef JP
			fprintf(fff, "\n ���Ȃ��͂���%s�������B", race_info[i].title);
#else
			fprintf(fff, "\n You were a %s before.", race_info[i].title);
#endif
		}

		fputc('\n', fff);
	}
	//v1.1.51
	if (nightmare_diary_count && nightmare_total_point)
	{
		int iissune;
		int mult;

		mult = calc_player_score_mult() ;
		//TODO:���̃X�R�A�{��*��Փx�␳�̌v�Z�����b�p�[�H�Ƃ������̂�����Ĉ�ӏ��ɂ܂Ƃ߂�ׂ�����
		if (difficulty == DIFFICULTY_EASY) mult /= 10;
		else if (difficulty == DIFFICULTY_HARD) mult = mult * 2;
		else if (difficulty == DIFFICULTY_LUNATIC) mult = mult * 8; //v1.1.53b 7��8
		else if (difficulty == DIFFICULTY_EXTRA) mult = mult * 4;

		if (mult < 1) mult = 1;

		//�C�C�����˃|�C���g:���ϊl���X�R�A*�X�R�A�{��
		iissune = nightmare_total_point / nightmare_diary_count * mult / 100;
		if (iissune < nightmare_diary_count) iissune = nightmare_diary_count;

		fprintf(fff, "\n  [����]\n");
		fprintf(fff, "  ���v���j��  %d\n", nightmare_diary_win);
		fprintf(fff, "  ���v�����  %d\n", nightmare_diary_count);
		fprintf(fff, "  ���v�]���_    %d\n", nightmare_total_point);
		fprintf(fff, "  �C�C�����ˁI  %d\n", iissune);

	}

	if(CHECK_FAIRY_TYPE)
	{
		fprintf(fff, "\n  [�\��]\n\n");
		fprintf(fff, "%s\n\n", fairy_table[CHECK_FAIRY_TYPE].fairy_desc);
	}
	if(p_ptr->pclass == CLASS_MOKOU)
	{
		fprintf(fff, "\n[���U���N�V������] %d��\n", p_ptr->magic_num1[0]);
	}
	if (p_ptr->pclass == CLASS_SHION)
	{
		if(finish_the_game && !p_ptr->magic_num1[2])
			fprintf(fff, "\n[�߈ˉ�]\n�@���Ȃ��͈�x���߈˂��g�킸�ɃQ�[���N���A�����I(�X�R�A3�{) \n");
		else
			fprintf(fff, "\n[�߈ˉ�] %d��\n", p_ptr->magic_num1[2]);
	}

	if(p_ptr->pclass == CLASS_SOLDIER)
	{
		int i;
		fprintf(fff, "\n[�K���Z�\] \n");
		for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++)
		{
			fprintf(fff, "  %s: %d \n",soldier_skill_table[i][0].name, p_ptr->magic_num2[i]);

		}
	}

	if(p_ptr->pclass == CLASS_KAGUYA)
	{
		fprintf(fff, "\n  [�_��]");
		if(!kaguya_quest_level || ironman_no_fixed_art)
			fprintf(fff, "\n���Ȃ��͐_���S���������ɖ`�����J�n�����B\n");
		else if(kaguya_quest_level >= 5)
			fprintf(fff, "\n���Ȃ��͑S�Ă̐_��������Ė`�����J�n�����B\n");
		else
			fprintf(fff, "\n���Ȃ���%d�̐_��������Ė`�����J�n�����B\n", kaguya_quest_level);
	}
	if(CLASS_IS_PRISM_SISTERS && !p_ptr->magic_num2[75])
	{
		fprintf(fff, "\n���Ȃ��͈�x���o���Ɩ`������サ�Ă��Ȃ��B \n");
	}

	if(p_ptr->pclass == CLASS_RINGO)
	{
		fprintf(fff, "\n  [�c�q���]");
		if(!p_ptr->magic_num1[0])
			fprintf(fff, "\n���Ȃ��͂��̖`���őS���c�q�����ɂ��Ȃ������B \n");
		else if(p_ptr->magic_num1[0] < 10000)
			fprintf(fff, "\n���Ȃ��͂��̖`����%d�̒c�q��H�ׂ��B \n",p_ptr->magic_num1[0]);
		else
			fprintf(fff, "\n���Ȃ��͂��̖`���ňꖜ�ȏ�̒c�q��H�ׂ��B \n");
	}

	if (p_ptr->pclass == CLASS_JYOON )
	{
		//��p���i�̂Ƃ��ɂ͖��̗�(�Ζ��n�`�ɕς����O���b�h��)��\������
		//����Ƀc�P���c�����܂܏������ނ����ꍇ��p���b�Z�[�W����悤
		if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON))
		{

			fprintf(fff, "\n  [���̗�]");
			if(p_ptr->magic_num1[1])
				fprintf(fff, "\n���Ȃ��͂��̖`����%d%06d ���b�g�����̖����T���U�炵���B \n", p_ptr->magic_num1[1], p_ptr->magic_num1[0]);
			else
				fprintf(fff, "\n���Ȃ��͂��̖`����%d ���b�g���̖����T���U�炵���B \n", p_ptr->magic_num1[0]);

			if (p_ptr->magic_num1[4]>0 && finish_the_game)
			{
				fprintf(fff, "\n  [�c�P]");
				fprintf(fff, "\n���Ȃ���$%d�̃c�P�̎x�������瓦���؂����I\n", p_ptr->magic_num1[4]);

			}


		}
		else
		{

			fprintf(fff, "\n  [�U��]");
			if (!p_ptr->magic_num1[0] && !p_ptr->magic_num1[1])
			{
				fprintf(fff, "\n���Ȃ��͂��̖`����$1����Ƃ����ʂɂ��Ȃ������B \n");
			}
			else if (p_ptr->magic_num1[1] > 1000)
			{
				fprintf(fff, "\n���Ȃ��͂��̖`����$%d,%03d,%03d,%03d���U�������B \n", p_ptr->magic_num1[1] / 1000, p_ptr->magic_num1[1] % 1000, p_ptr->magic_num1[0] / 1000, p_ptr->magic_num1[0] % 1000);

			}
			else if (p_ptr->magic_num1[1])
			{
				fprintf(fff, "\n���Ȃ��͂��̖`����$%d,%03d,%03d���U�������B \n", p_ptr->magic_num1[1] % 1000, p_ptr->magic_num1[0] / 1000, p_ptr->magic_num1[0] % 1000);

			}
			else if (p_ptr->magic_num1[0] > 1000)
			{
				fprintf(fff, "\n���Ȃ��͂��̖`����$%d,%03d���U�������B \n", p_ptr->magic_num1[0] / 1000, p_ptr->magic_num1[0] % 1000);

			}
			else
			{
				fprintf(fff, "\n���Ȃ��͂��̖`����$%d���U�������B \n", p_ptr->magic_num1[0] % 1000);

			}
		}



	}

}


static void dump_aux_skills(FILE *fff)
{
	int i,skill_exp;
	char skill_name[SKILL_EXP_MAX][12] ={"�i��","��","��n","�񓁗�","����","","","","","","�Z��","����","��","�݊�","�_","��","��","��������","�|","�N���X�{�E","�e"};

	fprintf(fff, "\n  [�Z�\�ꗗ]\n");
	fprintf(fff, "�Z�\�̎��    �K��    �Z�\���x��\n");
	for (i = 0; i < SKILL_EXP_MAX; i++)
	{
		if(i==5)
		{ 
			i=9;
			fprintf(fff, "\n");
			continue;
		}
		skill_exp = ref_skill_exp(i);
		fprintf(fff, "%-12s ", skill_name[i]);

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

		fprintf(fff, "\n");
	}

}


/*
 *
 */
static void dump_aux_realm_history(FILE *fff)
{
	if (p_ptr->old_realm)
	{
		int i;

		fputc('\n', fff);
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if (!(p_ptr->old_realm & 1L << i)) continue;
#ifdef JP
			if(p_ptr->pclass == CLASS_MERLIN || p_ptr->pclass == CLASS_LYRICA)
				fprintf(fff, "\n ���i�T�͂���%s���@���g�����B", realm_names[i+1]);
			else
				fprintf(fff, "\n ���Ȃ��͂���%s���@���g�����B", realm_names[i+1]);
#else
			fprintf(fff, "\n You were able to use %s magic before.", realm_names[i+1]);
#endif
		}

		if (p_ptr->old_realm & 1L << (TV_BOOK_OCCULT-1))
				fprintf(fff, "\n ���Ȃ��͂��Ĕ�p���g�����B");

		fputc('\n', fff);
	}
}


/*
 *
 */
#if 0
static void dump_aux_virtues(FILE *fff)
{
#ifdef JP
	fprintf(fff, "\n\n  [�v���C���[�̓�]\n\n");
#else
	fprintf(fff, "\n\n  [Virtues]\n\n");
#endif

#ifdef JP
	fprintf(fff, "���� : %s\n", your_alignment());
#else
	fprintf(fff, "Your alighnment : %s\n", your_alignment());
#endif

	fprintf(fff, "\n");
	dump_virtues(fff);
}
#endif

/*
 *
 */
static void dump_aux_mutations(FILE *fff)
{
	if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
	{
#ifdef JP
		fprintf(fff, "\n\n  [�ˑR�ψ�]\n\n");
#else
		fprintf(fff, "\n\n  [Mutations]\n\n");
#endif

		dump_mutations(fff);
	}
}


/*
 *
 */
static void dump_aux_equipment_inventory(FILE *fff)
{
	int i;
	char o_name[MAX_NLEN];

	/* Dump the equipment */
	if (equip_cnt)
	{
#ifdef JP
		fprintf(fff, "  [�L�����N�^�̑���]\n\n");
#else
		fprintf(fff, "  [Character Equipment]\n\n");
#endif

		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{

			///mod140308 �|�g�p�~�����֘A
			/*:::Mega Hack �|�𗼎�Ŏ����Ă��鎞�A�E�葤�ɋ����Ɂi����𗼎莝���j�ƕ\������*/
	//		if ((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute)
			if (((((i == INVEN_RARM) && p_ptr->hidarite) || ((i == INVEN_LARM) && p_ptr->migite)) && p_ptr->ryoute 
				|| (i == INVEN_RARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_RARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_LARM])
				|| (i == INVEN_LARM) && CAN_TWO_HANDS_WIELDING() && !inventory[INVEN_LARM].k_idx && object_is_shooting_weapon(&inventory[INVEN_RARM]))
				&& p_ptr->pclass != CLASS_3_FAIRIES)
				strcpy(o_name, "(����𗼎莝��)");
			else if(i == INVEN_RARM)
				object_desc(o_name, &inventory[i], OD_WEAPON_RARM);
			else if(i == INVEN_LARM)
				object_desc(o_name, &inventory[i], OD_WEAPON_LARM);
			else
				object_desc(o_name, &inventory[i], 0);

			fprintf(fff, "%c) %s\n",
				index_to_label(i), o_name);
		}
		fprintf(fff, "\n\n");
	}

	/* Dump the inventory */
#ifdef JP
	fprintf(fff, "  [�L�����N�^�̎�����]\n\n");
#else
	fprintf(fff, "  [Character Inventory]\n\n");
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Don't dump the empty slots */
		if (!inventory[i].k_idx) break;

		/* Dump the inventory slots */
		object_desc(o_name, &inventory[i], 0);
		fprintf(fff, "%c) %s\n", index_to_label(i), o_name);
	}

	/* Add an empty line */
	fprintf(fff, "\n\n");
}

///mod141010
/*:::�ǉ��C���x���g���̒��g���_���v�ɏo�͂���*/
static void dump_aux_inventory2(FILE *fff)
{
	int i;
	char o_name[MAX_NLEN];
	char out_desc[MAX_NLEN];
	int inven2_num = calc_inven2_num();
	object_type *o_ptr;
	int pc = p_ptr->pclass;

	///mod160311 Extra���[�h�ŃA�C�e���J�[�h�X���b�g��\��
	if(EXTRA_MODE)
	{
		int slot_max = calc_itemcard_slot_size();
		fprintf(fff, "  [�A�C�e���J�[�h�X���b�g]\n\n");
		for (i = INVEN_SPECIAL_ITEM_SLOT_START; i <= slot_max; i++)
		{
			o_ptr = &inven_special[i];
			if(!o_ptr->k_idx) strcpy(out_desc, "(��)");
			else sprintf(out_desc,"�w%s�x(%d)",support_item_list[o_ptr->pval].name,o_ptr->number);
			fprintf(fff, "%c) %s\n",index_to_label(i-1), out_desc);
		}
		fprintf(fff, "\n\n");
	}

	//v1.1.79 �V�푰�u���~��炵�v�̈ړ����~
	if (p_ptr->prace == RACE_ZASHIKIWARASHI)
	{
		o_ptr = &inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE];

		fprintf(fff, "  [�ړ������~]\n\n");
		object_desc(o_name, o_ptr, OD_NAME_AND_ENCHANT);
		fprintf(fff, "  %s\n\n\n", o_name);
	}

	//�ǉ��C���x���g���̂Ȃ��E�͏I��
	if(!inven2_num) return;

	//v2.0.4 ���{�ۂ͌��ʓ��Z���X�g�̂Ƃ��ɕ\������̂ł����ł͕s�v
	if (p_ptr->pclass == CLASS_MISUMARU)
		return;

	switch(pc)
	{
	case CLASS_ORIN:
		fprintf(fff, "  [�L�Ԃ̒��g]\n\n");break;
	case CLASS_ENGINEER:
		fprintf(fff, "  [�o�b�N�p�b�N�̕���]\n\n");break;
	case CLASS_NITORI:
		fprintf(fff, "  [�����b�N�̒�]\n\n");break;
	case CLASS_SH_DEALER:
		fprintf(fff, "  [����̒��g]\n\n");break;
	case CLASS_SUMIREKO:
		fprintf(fff, "  [���͂ɕ����Ă������]\n\n");break;
	case CLASS_ALICE:
		fprintf(fff, "  [�l�`�̑����i]\n\n");break;
	case CLASS_JYOON:
		fprintf(fff, "  [�w��]\n\n"); break;
	case CLASS_SAKUYA:
		fprintf(fff, "  [�i�C�t�z���_�[]\n\n");break;
	case CLASS_CHEMIST:
		fprintf(fff, "  [�򔠂̒�]\n\n");break;
	case CLASS_TAKANE:
	case CLASS_CARD_DEALER:
	case CLASS_SANNYO:
	case CLASS_MIKE:
		fprintf(fff, "  [�J�[�h�P�[�X�̒�]\n\n"); break;
	case CLASS_MARISA:
		fprintf(fff, "  [�B���|�P�b�g�̒�]\n\n"); break;

	default:
		fprintf(fff, "  [�ǉ��C���x���g��]\n\n");break;
	}

	/*:::�ǉ��C���x���g���\���@��ɂ��ׂĂ̗�*/
	for (i = 0; i < inven2_num; i++)
	{
		o_ptr = &inven_add[i];

		if(!o_ptr->k_idx) strcpy(out_desc, "(��)");
		else if(pc == CLASS_ENGINEER || pc == CLASS_NITORI)
		{
			object_desc(o_name, o_ptr, 0);
			if(o_ptr->xtra4) sprintf(out_desc,"%s(�̏ᒆ)",o_name);
			else	sprintf(out_desc,"%s(%d/%d)",o_name,o_ptr->pval,machine_table[o_ptr->sval].charge);
		}
		else if(pc == CLASS_ALICE)
		{
			object_desc(o_name, o_ptr, 0);
			sprintf(out_desc,"%15s:%s",alice_doll_name[i],o_name);
		}
		else if (pc == CLASS_JYOON)
		{
			object_desc(o_name, o_ptr, 0);
			sprintf(out_desc, "%15s:%s", jyoon_inven2_finger_name[i], o_name);
		}
		else object_desc(out_desc, o_ptr, 0);

		fprintf(fff, "%c) %s\n",index_to_label(i), out_desc);

	}

	/* Add an empty line */
	fprintf(fff, "\n\n");


}


/*
 *
 */
static void dump_aux_home_museum(FILE *fff)
{
	char o_name[MAX_NLEN];
	store_type  *st_ptr;

	/* Do we need it?? */
	/* process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x); */

	/* Print the home */
	st_ptr = &town[1].store[STORE_HOME];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		int i;
		int x = 1;

#ifdef JP
		fprintf(fff, "  [�䂪�Ƃ̃A�C�e��]\n");
#else
		fprintf(fff, "  [Home Inventory]\n");
#endif

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			if ((i % 12) == 0)
#ifdef JP
				fprintf(fff, "\n ( %d �y�[�W )\n", x++);
#else
				fprintf(fff, "\n ( page %d )\n", x++);
#endif
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), o_name);
		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}


	/* Print the home */
	st_ptr = &town[1].store[STORE_MUSEUM];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		int i;
		int x = 1;

#ifdef JP
		fprintf(fff, "  [�G���g�����X�ɏ������A�C�e��]\n");
		//fprintf(fff, "  [�����ق̃A�C�e��]\n");
#else
		fprintf(fff, "  [Museum]\n");
#endif

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
#ifdef JP
		if ((i % 12) == 0) fprintf(fff, "\n ( %d �y�[�W )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), o_name);
#else
		if ((i % 12) == 0) fprintf(fff, "\n ( page %d )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), o_name);
#endif

		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}
}


/*
 * Output the character dump to a file
 */
/*:::�_���v�t�@�C���o��*/
///system
errr make_character_dump(FILE *fff)
{
#ifdef JP
	fprintf(fff, "  [���z�ؓ{ %d.%d.%dT �L�����N�^���]\n\n",
		H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH);
#else
	fprintf(fff, "  [Hengband %d.%d.%d Character Dump]\n\n",
		FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#endif

	int test = 0; //�e�X�g����Ƃ�1�ɂ���

	update_playtime();

	dump_aux_display_player(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("display_player..");
	dump_aux_last_message(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("last_message..");
	dump_aux_options(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("options..");
	dump_aux_recall(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("recall..");
	dump_aux_quest(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("quest..");
	//dump_aux_arena(fff);
	dump_aux_monsters(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("monsters..");
	//dump_aux_virtues(fff);
	dump_aux_race_history(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("race his..");
	dump_aux_realm_history(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("realm his..");
	dump_aux_class_special(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("class_special..");
	dump_aux_mutations(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	dump_aux_skills(fff);
	if (test) { msg_format("%d", test++);	inkey(); }

	//msg_print("muta..");
	dump_aux_pet(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("pet..");
	fputs("\n\n", fff);
	dump_aux_equipment_inventory(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	dump_aux_inventory2(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("Inven..");
	dump_aux_home_museum(fff);
	if (test) { msg_format("%d", test++);	inkey(); }
	//msg_print("home..");

#ifdef JP
	fprintf(fff, "  [�`�F�b�N�T��: \"%s\"]\n\n", get_check_sum());
#else
	fprintf(fff, "  [Check Sum: \"%s\"]\n\n", get_check_sum());
#endif
	//msg_print("chesksum..");

	return 0;
}

/*
 * Hack -- Dump a character description file
 *
 * XXX XXX XXX Allow the "full" flag to dump additional info,
 * and trigger its usage from various places in the code.
 */
/*:::�w�肵���t�@�C�����Ń_���v���o�͂���*/
errr file_character(cptr name)
{
	int		fd = -1;
	FILE		*fff = NULL;
	char		buf[1024];

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		//v1.1.56 �O�̈�160��1280
		char out_val[1280];

		/* Close the file */
		(void)fd_close(fd);

		/* Build query */
#ifdef JP
		(void)sprintf(out_val, "��������t�@�C�� %s �ɏ㏑�����܂���? ", buf);
#else
		(void)sprintf(out_val, "Replace existing file %s? ", buf);
#endif


		/* Ask */
		if (get_check_strict(out_val, CHECK_NO_HISTORY)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* Invalid file */
	if (!fff)
	{
		/* Message */
#ifdef JP
		prt("�L�����N�^���̃t�@�C���ւ̏����o���Ɏ��s���܂����I", 0, 0);
#else
		prt("Character dump failed!", 0, 0);
#endif

		(void)inkey();

		/* Error */
		return (-1);
	}

	(void)make_character_dump(fff);

	/* Close it */
	my_fclose(fff);


	/* Message */
#ifdef JP
	//v1.1.28 ���S�ς݂Ȃ烁�b�Z�[�W�o���Ȃ�(�X�R�A�T�[�o�̃��O�ɏo�Ȃ��悤��)
	if(!p_ptr->is_dead)
		msg_print("�L�����N�^���̃t�@�C���ւ̏����o���ɐ������܂����B");
#else
	msg_print("Character dump successful.");
#endif

	msg_print(NULL);

	/* Success */
	return (0);
}


/*
 * Display single line of on-line help file
 *
 * You can insert some special color tag to change text color.
 * Such as...
 * WHITETEXT [[[[y|SOME TEXT WHICH IS DISPLAYED IN YELLOW| WHITETEXT
 *
 * A colored segment is between "[[[[y|" and the last "|".
 * You can use any single character in place of the "|".
 */
static void show_file_aux_line(cptr str, int cy, cptr shower)
{
	static const char tag_str[] = "[[[[";
	byte color = TERM_WHITE;
	char in_tag = '\0';
	int cx = 0;
	int i;
	char lcstr[1024];

	if (shower)
	{
		/* Make a lower case version of str for searching */
		strcpy(lcstr, str);
		str_tolower(lcstr);
	}

	/* Initial cursor position */
	Term_gotoxy(cx, cy);

	for (i = 0; str[i];)
	{
		int len = strlen(&str[i]);
		int showercol = len + 1;
		int bracketcol = len + 1;
		int endcol = len;
		cptr ptr;

		/* Search for a shower string in the line */
		if (shower)
		{
			ptr = my_strstr(&lcstr[i], shower);
			if (ptr) showercol = ptr - &lcstr[i];
		}

		/* Search for a color segment tag */
		ptr = in_tag ? my_strchr(&str[i], in_tag) : my_strstr(&str[i], tag_str);
		if (ptr) bracketcol = ptr - &str[i];

		/* A color tag is found */
		if (bracketcol < endcol) endcol = bracketcol;

		/* The shower string is found before the color tag */
		if (showercol < endcol) endcol = showercol;

		/* Print a segment of the line */
		Term_addstr(endcol, color, &str[i]);
		cx += endcol;
		i += endcol;

		/* Shower string? */
		if (endcol == showercol)
		{
			int showerlen = strlen(shower);

			/* Print the shower string in yellow */
			Term_addstr(showerlen, TERM_YELLOW, &str[i]);
			cx += showerlen;
			i += showerlen;
		}

		/* Colored segment? */
		else if (endcol == bracketcol)
		{
			if (in_tag)
			{
				/* Found the end of colored segment */
				i++;

				/* Now looking for an another tag_str */
				in_tag = '\0';

				/* Set back to the default color */
				color = TERM_WHITE;
			}
			else
			{
				/* Found a tag_str, and get a tag color */
				i += sizeof(tag_str)-1;

				/* Get tag color */
				color = color_char_to_attr(str[i]);

				/* Illegal color tag */
				if (color == 255 || str[i+1] == '\0')
				{
					/* Illegal color tag */
					color = TERM_WHITE;

					/* Print the broken tag as a string */
					Term_addstr(-1, TERM_WHITE, tag_str);
					cx += sizeof(tag_str)-1;
				}
				else
				{
					/* Skip the color tag */
					i++;

					/* Now looking for a close tag */
					in_tag = str[i];

					/* Skip the close-tag-indicator */
					i++;
				}
			}
		}

	} /* for (i = 0; str[i];) */

	/* Clear rest of line */
	Term_erase(cx, cy, 255);
}


/*
 * Recursive file perusal.
 *
 * Process various special text in the input file, including
 * the "menu" structures used by the "help file" system.
 *
 * Return FALSE on 'q' to exit from a deep, otherwise TRUE.
 */
/*:::�w�肵���t�@�C�����J���\������B�ڍז���*/
bool show_file(bool show_version, cptr name, cptr what, int line, int mode)
{
	int i, n, skey;

	/* Number of "real" lines passed by */
	int next = 0;

	/* Number of "real" lines in the file */
	int size = 0;

	/* Backup value for "line" */
	int back = 0;

	/* This screen has sub-screens */
	bool menu = FALSE;

	/* Current help file */
	FILE *fff = NULL;

	/* Find this string (if any) */
	cptr find = NULL;

	/* Jump to this tag */
	cptr tag = NULL;

	/* Hold strings to find/show */
	char finder_str[81];
	char shower_str[81];
	char back_str[81];

	/* String to show */
	cptr shower = NULL;

	/* Filename */
	char filename[1024];

	/* Describe this thing */
	char caption[128];

	/* Path buffer */
	char path[1024];

	/* General buffer */
	char buf[1024];

	/* Sub-menu information */
	char hook[68][32];

	bool reverse = (line < 0);

	int wid, hgt, rows;

	Term_get_size(&wid, &hgt);
	rows = hgt - 4;

	/* Wipe finder */
	strcpy(finder_str, "");

	/* Wipe shower */
	strcpy(shower_str, "");

	/* Wipe caption */
	strcpy(caption, "");

	/* Wipe the hooks */
	for (i = 0; i < 68; i++)
	{
		hook[i][0] = '\0';
	}

	/* Copy the filename */
	strcpy(filename, name);

	n = strlen(filename);

	/* Extract the tag from the filename */
	for (i = 0; i < n; i++)
	{
		if (filename[i] == '#')
		{
			filename[i] = '\0';
			tag = filename + i + 1;
			break;
		}
	}

	/* Redirect the name */
	name = filename;

	/* Hack XXX XXX XXX */
	if (what)
	{
		/* Caption */
		strcpy(caption, what);

		/* Access the "file" */
		strcpy(path, name);

		/* Open */
		fff = my_fopen(path, "r");
	}

	/* Look in "help" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
sprintf(caption, "�w���v�E�t�@�C��'%s'", name);
#else
		sprintf(caption, "Help file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR_HELP, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
sprintf(caption, "�X�|�C���[�E�t�@�C��'%s'", name);
#else
		sprintf(caption, "Info file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR_INFO, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR, name);

		for (i = 0; path[i]; i++)
			if ('\\' == path[i])
				path[i] = PATH_SEP[0];

		/* Caption */
#ifdef JP
sprintf(caption, "�X�|�C���[�E�t�@�C��'%s'", name);
#else
		sprintf(caption, "Info file '%s'", name);
#endif

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Oops */
	if (!fff)
	{
		/* Message */
#ifdef JP
msg_format("'%s'���I�[�v���ł��܂���B", name);
#else
		msg_format("Cannot open '%s'.", name);
#endif

		msg_print(NULL);

		/* Oops */
		return (TRUE);
	}


	/* Pre-Parse the file */
	while (TRUE)
	{
		char *str = buf;

		/* Read a line or stop */
		if (my_fgets(fff, buf, sizeof(buf))) break;

		/* XXX Parse "menu" items */
		if (prefix(str, "***** "))
		{
			/* Notice "menu" requests */
			if ((str[6] == '[') && isalpha(str[7]))
			{
				/* Extract the menu item */
				int k = str[7] - 'A';

				/* This is a menu file */
				menu = TRUE;

				if ((str[8] == ']') && (str[9] == ' '))
				{
					/* Extract the menu item */
					strncpy(hook[k], str + 10, 31);

					/* Make sure it's null-terminated */
					hook[k][31] = '\0';
				}
			}
			/* Notice "tag" requests */
			else if (str[6] == '<')
			{
				size_t len = strlen(str);

				if (str[len - 1] == '>')
				{
					str[len - 1] = '\0';
					if (tag && streq(str + 7, tag)) line = next;
				}
			}

			/* Skip this */
			continue;
		}

		/* Count the "real" lines */
		next++;
	}

	/* Save the number of "real" lines */
	size = next;

	/* start from bottom when reverse mode */
	if (line == -1) line = ((size-1)/rows)*rows;

	/* Clear screen */
	Term_clear();

	/* Display the file */
	while (TRUE)
	{
		/* Restart when necessary */
		if (line >= size - rows) line = size - rows;
		if (line < 0) line = 0;

		/* Re-open the file if needed */
		if (next > line)
		{
			/* Close it */
			my_fclose(fff);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			/* Oops */
			if (!fff) return (FALSE);

			/* File has been restarted */
			next = 0;
		}

		/* Goto the selected line */
		while (next < line)
		{
			/* Get a line */
			if (my_fgets(fff, buf, sizeof(buf))) break;

			/* Skip tags/links */
			if (prefix(buf, "***** ")) continue;

			/* Count the lines */
			next++;
		}

		/* Dump the next 20, or rows, lines of the file */
		for (i = 0; i < rows; )
		{
			cptr str = buf;

			/* Hack -- track the "first" line */
			if (!i) line = next;

			/* Get a line of the file or stop */
			if (my_fgets(fff, buf, sizeof(buf))) break;

			/* Hack -- skip "special" lines */
			if (prefix(buf, "***** ")) continue;

			/* Count the "real" lines */
			next++;

			/* Hack -- keep searching */
			if (find && !i)
			{
				char lc_buf[1024];

				/* Make a lower case version of str for searching */
				strcpy(lc_buf, str);
				str_tolower(lc_buf);

				if (!my_strstr(lc_buf, find)) continue;
			}

			/* Hack -- stop searching */
			find = NULL;

			/* Dump the line */
			show_file_aux_line(str, i + 2, shower);

			/* Count the printed lines */
			i++;
		}

		while (i < rows)
		{
			/* Clear rest of line */
			Term_erase(0, i + 2, 255);

			i++;
		}

		/* Hack -- failed search */
		if (find)
		{
			bell();
			line = back;
			find = NULL;
			continue;
		}


		/* Show a general "title" */
		if (show_version)
		{
			prt(format(
#ifdef JP
				"[���z�ؓ{ %d.%d.%dT, %s, %d/%d]",
#else
				"[Hengband %d.%d.%d, %s, Line %d/%d]",
#endif

			   H_VER_MAJOR, H_VER_MINOR, H_VER_PATCH,
			   caption, line, size), 0, 0);
		}
		else
		{
			prt(format(
#ifdef JP
				"[%s, %d/%d]",
#else
				"[%s, Line %d/%d]",
#endif
				caption, line, size), 0, 0);
		}

		/* Prompt -- small files */
		if (size <= rows)
		{
			/* Wait for it */
#ifdef JP
prt("[�L�[:(?)�w���v (ESC)�I��]", hgt - 1, 0);
#else
			prt("[Press ESC to exit.]", hgt - 1, 0);
#endif

		}

		/* Prompt -- large files */
		else
		{
#ifdef JP
			if(reverse)
				prt("[�L�[:(RET/�X�y�[�X)�� (-)�� (?)�w���v (ESC)�I��]", hgt - 1, 0);
			else
				prt("[�L�[:(RET/�X�y�[�X)�� (-)�� (?)�w���v (ESC)�I��]", hgt - 1, 0);
#else
			prt("[Press Return, Space, -, =, /, |, or ESC to exit.]", hgt - 1, 0);
#endif
		}

		/* Get a special key code */
		skey = inkey_special(TRUE);

		switch (skey)
		{
		/* Show the help for the help */
		case '?':
			/* Hack - prevent silly recursion */
#ifdef JP
			if (strcmp(name, "jhelpinfo.txt") != 0)
				show_file(TRUE, "jhelpinfo.txt", NULL, 0, mode);
#else
			if (strcmp(name, "helpinfo.txt") != 0)
				show_file(TRUE, "helpinfo.txt", NULL, 0, mode);
#endif
			break;

		/* Hack -- try showing */
		case '=':
			/* Get "shower" */
#ifdef JP
			prt("����: ", hgt - 1, 0);
#else
			prt("Show: ", hgt - 1, 0);
#endif

			strcpy(back_str, shower_str);
			if (askfor(shower_str, 80))
			{
				if (shower_str[0])
				{
					/* Make it lowercase */
					str_tolower(shower_str);

					/* Show it */
					shower = shower_str;
				}
				else shower = NULL; /* Stop showing */
			}
			else strcpy(shower_str, back_str);
			break;

		/* Hack -- try finding */
		case '/':
		case KTRL('s'):
			/* Get "finder" */
#ifdef JP
			prt("����: ", hgt - 1, 0);
#else
			prt("Find: ", hgt - 1, 0);
#endif

			strcpy(back_str, finder_str);
			if (askfor(finder_str, 80))
			{
				if (finder_str[0])
				{
					/* Find it */
					find = finder_str;
					back = line;
					line = line + 1;

					/* Make finder lowercase */
					str_tolower(finder_str);

					/* Show it */
					shower = finder_str;
				}
				else shower = NULL; /* Stop showing */
			}
			else strcpy(finder_str, back_str);
			break;

		/* Hack -- go to a specific line */
		case '#':
			{
				char tmp[81];
#ifdef JP
				prt("�s: ", hgt - 1, 0);
#else
				prt("Goto Line: ", hgt - 1, 0);
#endif

				strcpy(tmp, "0");

				if (askfor(tmp, 80)) line = atoi(tmp);
			}
			break;

		/* Hack -- go to the top line */
		case SKEY_TOP:
			line = 0;
			break;

		/* Hack -- go to the bottom line */
		case SKEY_BOTTOM:
			line = ((size - 1) / rows) * rows;
			break;

		/* Hack -- go to a specific file */
		case '%':
			{
				char tmp[81];
#ifdef JP
				prt("�t�@�C���E�l�[��: ", hgt - 1, 0);
				strcpy(tmp, "jhelp.hlp");
#else
				prt("Goto File: ", hgt - 1, 0);
				strcpy(tmp, "help.hlp");
#endif

				if (askfor(tmp, 80))
				{
					if (!show_file(TRUE, tmp, NULL, 0, mode)) skey = 'q';
				}
			}
			break;

		/* Allow backing up */
		case '-':
			line = line + (reverse ? rows : -rows);
			if (line < 0) line = 0;
			break;

		/* One page up */
		case SKEY_PGUP:
			line = line - rows;
			if (line < 0) line = 0;
			break;

		/* Advance a single line */
		case '\n':
		case '\r':
			line = line + (reverse ? -1 : 1);
			if (line < 0) line = 0;
			break;

		/* Move up / down */
		case '8':
		case SKEY_UP:
			line--;
			if (line < 0) line = 0;
			break;

		case '2':
		case SKEY_DOWN:
			line++;
			break;

		/* Advance one page */
		case ' ':
			line = line + (reverse ? -rows : rows);
			if (line < 0) line = 0;
			break;

		/* One page down */
		case SKEY_PGDOWN:
			line = line + rows;
			break;
		}

		/* Recurse on numbers */
		if (menu)
		{
			int key = -1;

			if (!(skey & SKEY_MASK) && isalpha(skey))
				key = skey - 'A';

			if ((key > -1) && hook[key][0])
			{
				/* Recurse on that file */
				if (!show_file(TRUE, hook[key], NULL, 0, mode))
					skey = 'q';
			}
		}

		/* Hack, dump to file */
		if (skey == '|')
		{
			FILE *ffp;
			char buff[1024];
			char xtmp[82];

			strcpy (xtmp, "");

#ifdef JP
			if (!get_string("�t�@�C����: ", xtmp, 80)) continue;
#else
			if (!get_string("File name: ", xtmp, 80)) continue;
#endif

			/* Close it */
			my_fclose(fff);

			/* Build the filename */
			path_build(buff, sizeof(buff), ANGBAND_DIR_USER, xtmp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			ffp = my_fopen(buff, "w");

			/* Oops */
			if (!(fff && ffp))
			{
#ifdef JP
				msg_print("�t�@�C�����J���܂���B");
#else
				msg_print("Failed to open file.");
#endif
				skey = ESCAPE;
				break;
			}

			sprintf(xtmp, "%s: %s", player_name, what ? what : caption);
			my_fputs(ffp, xtmp, 80);
			my_fputs(ffp, "\n", 80);

			while (!my_fgets(fff, buff, sizeof(buff)))
				my_fputs(ffp, buff, 80);

			/* Close it */
			my_fclose(fff);
			my_fclose(ffp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");
		}

		/* Return to last screen */
		if ((skey == ESCAPE) || (skey == '<')) break;

		/* Exit on the ^q */
		if (skey == KTRL('q')) skey = 'q';

		/* Exit on the q key */
		if (skey == 'q') break;
	}

	/* Close the file */
	my_fclose(fff);

	/* Escape */
	if (skey == 'q') return (FALSE);

	/* Normal return */
	return (TRUE);
}


/*
 * Peruse the On-Line-Help
 */
/*:::�w���v��ǂ�*/
void do_cmd_help(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the main help file */
#ifdef JP
	///mod140927 �w���v�t�@�C���ύX
	//(void)show_file(TRUE, "jhelp.hlp", NULL, 0, 0);
	(void)show_file(TRUE, "thelp.hlp", NULL, 0, 0);
#else
	(void)show_file(TRUE, "help.hlp", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}


/*
 * Process the player name.
 * Extract a clean "base name".
 * Build the savefile name if needed.
 */
/*:::���̖��O�Ɋւ���@�킲�Ƃ̉۔��菈���A�Z�[�u�t�@�C���x�[�X���Ȃǌ���*/
void process_player_name(bool sf)
{
	int i, k = 0;
	char old_player_base[32] = "";

	if (character_generated) strcpy(old_player_base, player_base);

	/* Cannot be too long */
#if defined(MACINTOSH) || defined(MSDOS) || defined(USE_EMX) || defined(AMIGA) || defined(ACORN) || defined(VM)
#ifdef MSDOS
	if (strlen(player_name) > 8)
#else
	if (strlen(player_name) > 15)
#endif
	{
		/* Name too long */
#ifdef JP
quit_fmt("'%s'�Ƃ������O�͒������܂��I", player_name);
#else
		quit_fmt("The name '%s' is too long!", player_name);
#endif

	}
#endif

	/* Cannot contain "icky" characters */
	for (i = 0; player_name[i]; i++)
	{
		/* No control characters */
#ifdef JP
		if (iskanji(player_name[i])){i++;continue;}
		if (iscntrl( (unsigned char)player_name[i]))
#else
		if (iscntrl(player_name[i]))
#endif

		{
			/* Illegal characters */
#ifdef JP
quit_fmt("'%s' �Ƃ������O�͕s���ȃR���g���[���R�[�h���܂�ł��܂��B", player_name);
#else
			quit_fmt("The name '%s' contains control chars!", player_name);
#endif

		}
	}


#ifdef MACINTOSH

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif


		/* Convert "dot" to "underscore" */
		if (c == '.') c = '_';

		/* Accept all the letters */
		player_base[k++] = c;
	}

#else

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif

		/* Accept some letters */
#ifdef JP
		if(iskanji(c)){
		  if(k + 2 >= sizeof(player_base) || !player_name[i+1]) break;
		  player_base[k++] = c;
		  i++;
		  player_base[k++] = player_name[i];
		}
#ifdef SJIS
		else if (iskana(c)) player_base[k++] = c;
#endif
		else
#endif
		/* Convert path separator to underscore */
		if (!strncmp(PATH_SEP, player_name+i, strlen(PATH_SEP))){
			player_base[k++] = '_';
			i += strlen(PATH_SEP);
		}
		/* Convert some characters to underscore */
#ifdef MSDOS
		else if (my_strchr(" \"*+,./:;<=>?[\\]|", c)) player_base[k++] = '_';
#elif defined(WINDOWS)
		else if (my_strchr("\"*,/:;<>?\\|", c)) player_base[k++] = '_';
#endif
		else if (isprint(c)) player_base[k++] = c;
	}

#endif


#if defined(MSDOS)

	/* Hack -- max length */
	if (k > 8) k = 8;

#endif

	/* Terminate */
	player_base[k] = '\0';

	/* Require a "base" name */
	if (!player_base[0]) strcpy(player_base, "PLAYER");


#ifdef SAVEFILE_MUTABLE

	/* Accept */
	sf = TRUE;

#endif
	if (!savefile_base[0] && savefile[0])
	{
		cptr s;
		s = savefile;
		while (1)
		{
			cptr t;
			t = my_strstr(s, PATH_SEP);
			if (!t)
				break;
			s = t+1;
		}
		strcpy(savefile_base, s);
	}

	if (!savefile_base[0] || !savefile[0])
		sf = TRUE;

	///mod140316 �Z�[�u�t�@�C��������������̎����ݒ�łȂ������Ŏw�肷��悤�ɂ����B���j�[�N�v���C���[�̎����œ����Z�[�u�t�@�C���㏑�����N����₷���Ȃ������߁B
	/*:::�Z�[�u�t�@�C�������w�肷��B���̊��ł͂����ɂ̓L�������C�N���ɂ�������Ȃ��͂��B*/
	if (sf)
	{
		bool flag_err;
		while(1)
		{
			FILE *fp;
			char temp[128];

			flag_err = FALSE;
			strcpy(temp, player_base);
			get_string("�Z�[�u�t�@�C��������͂��Ă�������: ", temp, 30);
			if(!temp[0]) strcpy(temp, player_base);
			/*:::���͂����Z�[�u�t�@�C����windows�Ŏg���Ȃ���������������_�ɕϊ�����B�����������M�Ȃ��B*/
			for (i = 0; temp[i]; i++)
			{
				unsigned char c = temp[i];
				if(iskanji(c))
				{
					i++;
					continue;
				}
				else if(iskana(c)) continue;
				else if (my_strchr("\"*,/:;<>?\\|", c))
				{
					flag_err = TRUE;
					temp[i] = '_';
				}
			}
			strcpy(savefile_base, temp);
			path_build(savefile, sizeof(savefile), ANGBAND_DIR_SAVE, temp);
			/*:::�Z�[�u�t�@�C�����J���Ă݂ĊJ�����瓯���t�@�C��������*/
			fp = my_fopen(savefile, "r");
			if(fp)
			{
				my_fclose(fp);
				msg_format("�Z�[�u�t�@�C�������łɑ��݂��܂��B");
				if (get_check_strict("�㏑�����܂����H�F", CHECK_OKAY_CANCEL))break;
				else continue;
			}
			break;
		}
		if(flag_err)
		{ 
			msg_format("Windows�̃t�@�C�����Ɏg���Ȃ�������'_'�ɕϊ�����܂����B");
			inkey();
		}
	}


#if 0
	/*:::��̕ύX���~�X�������Ƃ��̂��߂̃o�b�N�A�b�v:::*/
	/* Change the savefile name */
	if (sf)
	{
		char temp[128];

		strcpy(savefile_base, player_base);

#ifdef SAVEFILE_USE_UID
		/* Rename the savefile, using the player_uid and player_base */
		(void)sprintf(temp, "%d.%s", player_uid, player_base);
#else
		/* Rename the savefile, using the player_base */
		(void)sprintf(temp, "%s", player_base);
#endif

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		(void)sprintf(temp, "%s.sv", player_base);
#endif /* VM */

		/* Build the filename */
		path_build(savefile, sizeof(savefile), ANGBAND_DIR_SAVE, temp);
	}
	/*:::�o�b�N�A�b�v�����܂�:::*/
#endif 

	/* Load an autopick preference file */
	if (character_generated)
	{
		if (!streq(old_player_base, player_base)) autopick_load_pref(FALSE);
	}
}


/*
 * Gets a name for the character, reacting to name changes.
 *
 * Assumes that "display_player(0)" has just been called
 *
 * Perhaps we should NOT ask for a name (at "birth()") on
 * Unix machines?  XXX XXX
 *
 * What a horrible name for a global function.  XXX XXX XXX
 */
/*:::���̖��O��ύX����@C�R�}���h���L�����N�^�[���C�L���O���ɌĂ΂��*/
void get_name(void)
{
	char tmp[64];

	/* Save the player name */
	strcpy(tmp, player_name);

	/* Prompt for a new name */
#ifdef JP
	if (get_string("�L�����N�^�[�̖��O����͂��ĉ�����: ", tmp, 30))
//	if (get_string("�L�����N�^�[�̖��O����͂��ĉ�����: ", tmp, 15))
#else
	if (get_string("Enter a name for your character: ", tmp, 15))
#endif
	{
		/* Use the name */
		strcpy(player_name, tmp);
	}

	if (0 == strlen(player_name))
	{
		/* Use default name */
		strcpy(player_name, "PLAYER");
	}

	/*:::��ʏ㕔�́u���i+���O�v�̕���������������*/
	if(p_ptr->psex == SEX_MALE)
		strcpy(tmp,ap_ptr->title);
	else
		strcpy(tmp,ap_ptr->f_title);
#ifdef JP
	if(ap_ptr->no == 1)
		strcat(tmp,"��");
#else
	strcat(tmp, " ");
#endif
	strcat(tmp,player_name);

	/* Re-Draw the name (in light blue) */
	//Term_erase(34, 1, 255);
	Term_erase(14, 1, 255);
	//c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, tmp, 1, 24);

	/* Erase the prompt, etc */
	clear_from(22);
}

/*:::���̐E�Ɩ���ύX����@���j�[�N�v���C���[�̂Ƃ�C�R�}���h����g����B
 *:::���邢�͐E�ƕ��ʐl�̎��L�������C�N�Ŏg����B*/
void get_class_name(void)
{
	char tmp[64];

	/* Save the player name */
	strcpy(tmp, player_class_name);

	/* Prompt for a new name */
#ifdef JP
	if (get_string("�L�����N�^�[�̐E�Ɩ����Đݒ�ł��܂�: ", tmp, 17))
//	if (get_string("�L�����N�^�[�̖��O����͂��ĉ�����: ", tmp, 15))
#endif
	{
		/* Use the name */
		strcpy(player_class_name, tmp);
	}

	if (0 == strlen(player_class_name))
	{
		/* Use default name */
		if(p_ptr->psex == SEX_MALE ) strcpy(player_class_name, cp_ptr->title);
		else  strcpy(player_class_name, cp_ptr->f_title);
	}

}

/*
 * Hack -- commit suicide
 */
/*:::���ށE���E*/
///system
void do_cmd_suicide(void)
{
	int i;

	/* Flush input */
	flush();

	//v1.1.51
	if (p_ptr->inside_arena)
	{
		msg_print("���̒��ł��̂悤�Ȍ��f�������ׂ��ł͂Ȃ��B");
		return;
	}

	/* Verify Retirement */
	if (p_ptr->total_winner)
	{
		/* Verify */
#ifdef JP
	if (p_ptr->pclass == CLASS_OUTSIDER && !get_check_strict("���z���ɉi�Z���܂���? ", CHECK_NO_HISTORY)) return;
	if (p_ptr->pclass != CLASS_OUTSIDER && !get_check_strict("���̖`�����I���܂���? ", CHECK_NO_HISTORY)) return;
#else
		if (!get_check_strict("Do you want to retire? ", CHECK_NO_HISTORY)) return;
#endif

	}

	/* Verify Suicide */
	else
	{
		/* Verify */
#ifdef JP
		if (!get_check("���̖`����������܂����H")) return;
		///msg131221 dead
		//if (!get_check("�{���Ɏ��E���܂����H")) return;
#else
		if (!get_check("Do you really want to commit suicide? ")) return;
#endif
	}


	if (!p_ptr->noscore)
	{
		/* Special Verification for suicide */
#ifdef JP
prt("�m�F�̂��� '@' �������ĉ������B", 0, 0);
#else
		prt("Please verify SUICIDE by typing the '@' sign: ", 0, 0);
#endif

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;

		if(p_ptr->total_winner) finish_the_game = TRUE;

		else 		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_GAMEOVER);//v1.1.58

	}

	/* Initialize "last message" buffer */
	/*:::�f�����⏟�����b�Z�[�W����łȂ���Ώ������E�E�H*/
	if (p_ptr->last_message) string_free(p_ptr->last_message);
	p_ptr->last_message = NULL;

	/* Hack -- Note *winning* message */
	/*:::last_words:�⌾�I�v�V����*/
	if (p_ptr->total_winner && last_words)
	{
		char buf[1024] = "";

		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_WINNER); //v1.1.58

		do
		{
#ifdef JP
			while (!get_string("*����*���b�Z�[�W: ", buf, sizeof buf)) ;
#else
			while (!get_string("*Winning* message: ", buf, sizeof buf)) ;
#endif
		}
#ifdef JP
		while (!get_check_strict("��낵���ł����H", CHECK_NO_HISTORY));
#else
		while (!get_check_strict("Are you sure? ", CHECK_NO_HISTORY));
#endif

		if (buf[0])
		{
			p_ptr->last_message = string_make(buf);
			msg_print(p_ptr->last_message);
		}
	}

	/* Stop playing */
	p_ptr->playing = FALSE;

	/* Kill the player */
	p_ptr->is_dead = TRUE;

	/* Leaving */
	p_ptr->leaving = TRUE;

	if (!p_ptr->total_winner)
	{
#ifdef JP
		///msg131221 dead
		//do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�_���W�����̒T���ɐ�]���Ď��E�����B");
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "����̃Q�[������������B");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- �Q�[���I�[�o�[ --------");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up all hope to commit suicide.");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");
	}

	/* Cause of death */
#ifdef JP
(void)strcpy(p_ptr->died_from, "�r���I��");
#else
	(void)strcpy(p_ptr->died_from, "Quitting");
#endif

}



/*
 * Save the game
 */
/*:::�Z�[�u����*/
void do_cmd_save_game(int is_autosave)
{
	/* Autosaves do not disturb */
	if (is_autosave)
	{
#ifdef JP
msg_print("�����Z�[�u��");
#else
		msg_print("Autosaving the game...");
#endif

	}
	else
	{
		/* Disturb the player */
		disturb(1, 1);
	}

	/* Clear messages */
	msg_print(NULL);

	/* Handle stuff */
	handle_stuff();

	/* Message */
#ifdef JP
prt("�Q�[�����Z�[�u���Ă��܂�...", 0, 0);
#else
	prt("Saving game...", 0, 0);
#endif


	/* Refresh */
	Term_fresh();

	/* The player is not dead */
#ifdef JP
(void)strcpy(p_ptr->died_from, "(�Z�[�u)");
#else
	(void)strcpy(p_ptr->died_from, "(saved)");
#endif


	/* Forbid suspend */
	/*:::�������ĂȂ��H*/
	signals_ignore_tstp();

	if(p_ptr->wizard) msg_format("playing:%d is_dead:%d",p_ptr->playing,p_ptr->is_dead);

	/* Save the player */
	if (save_player())
	{
#ifdef JP
prt("�Q�[�����Z�[�u���Ă��܂�... �I��", 0, 0);
#else
		prt("Saving game... done.", 0, 0);
#endif

	}

	/* Save failed (oops) */
	else
	{
#ifdef JP
prt("�Q�[�����Z�[�u���Ă��܂�... ���s�I", 0, 0);
#else
		prt("Saving game... failed!", 0, 0);
#endif

		if (!p_ptr->playing && !p_ptr->is_dead)
		{
			Term_clear();
			prt("��ϐ\���󂠂�܂���B�Z�[�u�Ɏ��s���܂����B", 5, 5);
			prt("�����I�ɐl���ɖ߂��čăZ�[�u�����݂܂��B", 6, 5);
			prt("�O�̂��ߑ��s�O��save�t�H���_�̃A�N�Z�X���Ȃǂ��������ĉ������B", 7, 5);
			prt("user�t�H���_�ɋً}�_���v���o�͂���܂��B", 8, 5);
			prt("�o�O�����̂��߂ɍ�҂ɑ����Ă���������ƍK���ł��B", 9, 5);
			prt("�����L�[�������Ƒ��s���܂��B", 12, 5);
			(void)inkey();
			(void)file_character("heng_katte_panic_dump.txt");
			p_ptr->word_recall = 0;
			p_ptr->alter_reality = 0;
			dun_level = 0;
			p_ptr->inside_quest = 0;
			dungeon_type = 0;
			p_ptr->floor_id = 0;
			p_ptr->wilderness_y = 34;
			p_ptr->wilderness_x = 39;
			p_ptr->oldpy = 33;
			p_ptr->oldpx = 180;
			p_ptr->riding = 0;
			wipe_m_list();
			change_floor();
			if(save_player()) prt("�ăZ�[�u�ɐ����B", 14, 5);
			else prt("�ăZ�[�u���s..", 14, 5);
			(void)inkey();
		}
	}

	/* Allow suspend again */
	signals_handle_tstp();

	/* Refresh */
	Term_fresh();

	/* Note that the player is not dead */
#ifdef JP
(void)strcpy(p_ptr->died_from, "(���C�ɐ����Ă���)");
#else
	(void)strcpy(p_ptr->died_from, "(alive and well)");
#endif

	/* HACK -- don't get sanity blast on updating view */
	hack_mind = FALSE;

	/* Update stuff */
	update_stuff();

	/* Initialize monster process */
	mproc_init();

	/* HACK -- reset the hackish flag */
	hack_mind = TRUE;
}


/*
 * Save the game and exit
 */
/*:::�Z�[�u���ďI���@�����ł̓t���O�����邾��*/
void do_cmd_save_and_exit(void)
{
	p_ptr->playing = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "----�Q�[�����f----");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "---- Save and Exit Game ----");
#endif
}


/*:::�X�R�A�v�Z�@�V*/
u32b total_points_new(bool test)
{
	u32b point, point_h=0, point_l=0;
	s16b max_dl = 0;
	int i,j,mult;
	int mult_race, mult_days;
	int value_tmp;
	int value_items = 0, value_items_max = 0;
	int questpoint=0,monpoint=0,turnbonus=0,floorpoint=0;
	object_type		*o_ptr;
	store_type *st_ptr_tmp;

	//v1.1.53b �X�R�A�v�Z�Ɏg���^�[�������u�Q�[���^�[���v�łȂ��u�_���W�����^�[���v�ɂ����B
	//���̒l�͍L��}�b�v�ňړ������Ƃ��̈����1�^�[�������Ƃ��ăJ�E���g������́B
	//Extra���L��}�b�v�ɍs���K�v���Ȃ������{�[�i�X���҂��₷���̂Ō��������������ߕύX����B
	int score_turn = dungeon_turn;

	//���̒l�̓v���O���������グ�������ƕێ�����B�Z�[�u�͂��Ȃ��B
	static int value_items_museum=0, value_items_max_museum=0; 


	//�ŐV���B�K�w�ɂ��{�[�i�X
	for (i = 0; i < max_d_idx; i++)	if(max_dlv[i] > max_dl)	max_dl = max_dlv[i];

	if(difficulty == DIFFICULTY_EASY) floorpoint = max_dl * max_dl * 3;
	else if(difficulty == DIFFICULTY_NORMAL) floorpoint = max_dl * max_dl * 5;
	else if(difficulty == DIFFICULTY_HARD) floorpoint = max_dl * max_dl * 10;
	else if(difficulty == DIFFICULTY_EXTRA) floorpoint = max_dl * max_dl * 30;
	else if(difficulty == DIFFICULTY_LUNATIC) floorpoint = max_dl * max_dl * 50;

	point_l += floorpoint;
if(test) msg_format("FLOOR_BONUS:%d",floorpoint);


	//�o���l/10(���1000000)
	point_l += MIN(1000000,p_ptr->max_exp / 10);
if(test) msg_format("EXP(MAX1000k):%d",p_ptr->max_exp / 10);

	st_ptr_tmp = &town[1].store[STORE_HOME];
	for (j = 0; j < st_ptr_tmp->stock_num; j++)
	{
		o_ptr = &st_ptr_tmp->stock[j];
		if (!o_ptr->k_idx) continue;
		if(finish_the_game) value_tmp = object_value_real(o_ptr);
		else value_tmp = object_value(o_ptr);
		if(value_items<10000000) value_items += value_tmp * o_ptr->number;
		value_items_max = MAX(value_items_max, value_tmp);

	}

	//���i�u�N�W�Ɓv�͔����ق̃A�C�e�����X�R�A�Ɍv�Z���Ȃ�(�X�R�A�T�[�o�����Ń{�c�ɂȂ�����)
	if(p_ptr->pseikaku == SEIKAKU_COLLECTOR)
	{
		value_items_museum=0;
		value_items_max_museum=0;
		hack_flag_calc_museum_score = FALSE;
	}
	///mod160326 �����قɂ�������A�C�e��������ƌv�Z���d�����Ȃ̂œ���̃t���O�������Ă��鎞�����v�Z���Ȃ����Ƃɂ���
	//is_dead�̔���͂Ȃ��Ă������͂����������ςȃo�O�N�����čŏI�_���v�����������Ȃ�Ȃ��悤�Ɉꉞ���Ƃ�
	else if(p_ptr->is_dead || hack_flag_calc_museum_score)
	{
		value_items_museum=0;
		value_items_max_museum=0;
		st_ptr_tmp = &town[1].store[STORE_MUSEUM];
		for (j = 0; j < st_ptr_tmp->stock_num; j++)
		{
			o_ptr = &st_ptr_tmp->stock[j];
			if (!o_ptr->k_idx) continue;
			if(finish_the_game) value_tmp = object_value_real(o_ptr);
			else value_tmp = object_value(o_ptr);
			if((value_items_museum + value_items)<10000000) value_items_museum += value_tmp * o_ptr->number;
			value_items_max_museum = MAX(value_items_max_museum, value_tmp);
		}
		hack_flag_calc_museum_score = FALSE;
	}
	value_items += value_items_museum;
	value_items_max = MAX(value_items_max, value_items_max_museum);

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];
		if (!o_ptr->k_idx) continue;
		if(finish_the_game) value_tmp = object_value_real(o_ptr);
		else value_tmp = object_value(o_ptr);
		if(value_items<10000000) value_items += value_tmp * o_ptr->number;
		value_items_max = MAX(value_items_max, value_tmp);
	}
	//�ǉ��C���x���g���̒��g�̃X�R�A�v�Z
	for (i = 0; i < INVEN_ADD_MAX; i++)
	{
		o_ptr = &inven_add[i];
		if (!o_ptr->k_idx) continue;
		if(finish_the_game) value_tmp = object_value_real(o_ptr);
		else value_tmp = object_value(o_ptr);
		if(value_items<10000000) value_items += value_tmp * o_ptr->number;
		value_items_max = MAX(value_items_max, value_tmp);
	}

	if(value_items<10000000) value_items += p_ptr->au;

	//v1.1.92 ����(��p���i)���c�P�𖢐��Z�̏ꍇ�v�Z���獷������
	if (is_special_seikaku(SEIKAKU_SPECIAL_JYOON) && p_ptr->magic_num1[4])
	{
		value_items -= p_ptr->magic_num1[4];
	}

	//v1.1.47 ��؋������ăv���C�J�n���遗�̂��߂ɉ����ǉ�
	if (value_items < 0) value_items = 0;
	//������+�����i�̉��l/10(���1000000)
	point_l += MIN(1000000,value_items / 10);
	//�ł������ȏ����i�̉��l(���1000000)
	point_l += MIN(1000000,value_items_max);
if(test) msg_format("ITEMS(MAX1000k):%d",value_items / 10);
if(test) msg_format("SOUVENIR(MAX1000k):%d",value_items_max);

	//���������N�G�X�g Lev^2*10(��{)
	for (i = 1; i < max_quests; i++)
	{
		///mod150504 Hack - ���͎��N�G�X�g��B���ς݂����X�R�A�Ȃǂɂ͎Q�����Ȃ�
		if((p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI) && i == QUEST_YUKARI) continue;

		if(quest[i].status == QUEST_STATUS_COMPLETED || quest[i].status == QUEST_STATUS_FINISHED)
		{
			//Extra���[�h�ł̓N�G�X�g�����Ȃ��̂Ń{�[�i�X��{
			questpoint += quest[i].level * quest[i].level * (60 - quest[i].complev) * (EXTRA_MODE?2:1);
			//����N�G�X�g�Ƀ{�[�i�X
			if(i == QUEST_YUKARI)	questpoint += 100000;
			if(i == QUEST_TAISAI)	questpoint += 500000;
			if(i == QUEST_SERPENT)	questpoint += 1000000;

		}
	}

	point_l += questpoint;
if(test) msg_format("QUEST:%d",questpoint);

	//�œ|���j�[�N�����X�^�[ Lev^2 �A���o�[�̉�����+10000
	for(i=1;i<max_r_idx;i++)
	{
		monster_race *r_ptr = &r_info[i];

		if(!(r_ptr->flags1 & RF1_UNIQUE) || !r_ptr->r_akills) continue;

		monpoint += r_ptr->level * r_ptr->level;

		//�����X�{�X�̃I�x������|���ƃX�R�A���啝�ɏオ��BEXTRA�ł͍���Ȃ̂ł���Ƀ{�[�i�X
		if (i == MON_OBERON)
		{
			if(EXTRA_MODE) monpoint += 600000;
			else monpoint += 300000;
		}
		else if ((r_ptr->flags3 & RF3_ANG_AMBER) && r_ptr->r_akills) monpoint += 10000;
	}

	///mod160326 UNIQUE2�œ|�������j�[�N�Ɠ��l�ɃX�R�A�Z�o
	monpoint += score_bonus_kill_unique2;

if(test) msg_format("UNIQUE:%d",monpoint);
	point_l += monpoint;

	//�I�x�����|���Ĉ��ނ���Ƃ��^�[���ɉ����ă{�[�i�X(�L���m���{�E���[�̓{�[�i�X����)
	if(finish_the_game && (quest[QUEST_TAISAI].status == QUEST_STATUS_FINISHED))
	{
		if(score_turn < TURNS_PER_TICK * TOWN_DAWN * 30) turnbonus += (TURNS_PER_TICK * TOWN_DAWN * 30 - score_turn);
		//v1.1.53b ��ՓxLUNATIC�̂Ƃ��͂ǂ����������R�Ȃ̂ŉ��̐��i�ł����l�̃{�[�i�X�𓾂��邱�Ƃɂ���
		if(p_ptr->pseikaku == SEIKAKU_SEXY || (difficulty == DIFFICULTY_LUNATIC))
		{
			if(EXTRA_MODE) turnbonus *= 4;
			else turnbonus *= 2;
		}

		if(p_ptr->pclass == CLASS_SAKUYA) turnbonus /= 2;

if(test) msg_format("TURNBONUS:%d",turnbonus);
		point_l += turnbonus;
	}



	//�푰�E�Ɣ{���K�p
	//v1.1.42 �ꕔ��ʊ֐��ɕ���
	mult = calc_player_score_mult() * 100;

	//v1.1.48 ��������x���߈˂��g�킸�Q�[���N���A������X�R�A3�{�ɂ��Ă��܂�
	if (p_ptr->pclass == CLASS_SHION)
	{
		if (finish_the_game && !p_ptr->magic_num1[2]) mult *= 3;
	}


/*
	mult_race = race_info[p_ptr->prace].score_mult;
if(test) msg_format("mult_race_start:%d",mult_race);
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
if(test) msg_format("mult_race_total:%d",mult_race);

	mult = mult * mult_race / 100;
	mult = mult * class_info[p_ptr->pclass].score_mult / 100;
*/

	//��Փx�A�I�v�V�����Ȃ�
	if(difficulty == DIFFICULTY_EASY) mult /= 10;
	else if(difficulty == DIFFICULTY_HARD) mult = mult * 2;
	else if(difficulty == DIFFICULTY_LUNATIC) mult = mult * 8; //v1.1.53b 7��8
	else if(difficulty == DIFFICULTY_EXTRA) mult = mult * 4;

	if(EXTRA_MODE)
	{
		if(ironman_empty_levels) mult = mult * 6 / 5;
	 	if(ironman_small_levels) mult = mult * 4 / 3;
		if(ironman_rooms) mult = mult * 4 / 3;
		if(ironman_shops) mult = mult * 4 / 3;
		else if(!powerup_home) mult = mult * 12 / 10;

		extra_mode_score_mult(&mult,p_ptr->pclass);//EXTRA��p�̃N���X�ʕ␳
	}
	else
	{
		if(ironman_empty_levels) mult = mult * 11 / 10;
		if(ironman_small_levels) mult = mult * 11 / 10;
		if(ironman_rooms) mult = mult * 4 / 5;
		if(ironman_shops) mult = mult * 3 / 2;
		else if(!powerup_home) mult = mult * 12 / 10;
	}

	if (ironman_no_bonus)  mult = mult * 3 / 2;
	else if (birth_bonus_10)  mult = mult * 6 / 5;
	else if (birth_bonus_20) mult = mult * 21 / 20;


	if(ironman_no_fixed_art)
	{
		if(p_ptr->pclass == CLASS_SH_DEALER || p_ptr->pclass == CLASS_JEWELER) mult = mult * 11 / 10;
		else 	mult = mult * 13 / 10;
	}
	else if(!preserve_mode)
	{
		if(EXTRA_MODE)
			mult = mult * 11 / 10;
		else
			mult = mult * 12 / 10;
	}
	
	if(!no_capture_book) mult = mult * 21 / 20;


	//�����o�߂Ń{�[�i�X���y�i���e�B
	//30���ȓ��Ȃ���1%���{�[�i�X
	if(score_turn < TURNS_PER_TICK * TOWN_DAWN * 30)
	{
		mult_days = (TURNS_PER_TICK * TOWN_DAWN * 30 - score_turn) / (TURNS_PER_TICK * TOWN_DAWN) + 1;
		if(p_ptr->pclass == CLASS_SAKUYA) mult_days /= 2;
	}
	//60���ȓ��Ȃ�y�i���e�B�Ȃ�
	else if(score_turn < TURNS_PER_TICK * TOWN_DAWN * 60)
	{
		mult_days = 0;
	}
	//����ȍ~��6���o�ߖ���1%�y�i���e�B(360����-50%�ŉ����~�܂�)
	else
	{
		mult_days = -1 - (score_turn - TURNS_PER_TICK * TOWN_DAWN * 60) / (TURNS_PER_TICK * TOWN_DAWN) / 4;
		if(mult_days < -50) mult_days = -50;
	}
	mult = mult * (100 + mult_days) / 100;

	if(p_ptr->pseikaku == SEIKAKU_MUNCHKIN) mult /= 100;
	//else if(p_ptr->pseikaku == SEIKAKU_BERSERK) mult /= 2; v1.1.42�ʊ֐��ɕ���

	if(finish_the_game) mult *= 2;

	//v1.1.32 �N���X��p���� ������ƋL�@�ύX
	//v1.1.42 �ʊ֐��ɕ���
	/*
	switch(p_ptr->pclass)
	{
	case CLASS_MOKOU:
		//���g�̓��U���N�V�����񐔂ŃX�R�A����
		if( p_ptr->magic_num1[0] ) mult /= (1 + MIN(99,p_ptr->magic_num1[0]));
		break;
	case CLASS_KAGUYA:
		if(kaguya_quest_level && !ironman_no_fixed_art)
		{
			//�P��͏��������_��̐��ŃX�R�A����
			for(i=0;i<kaguya_quest_level;i++) mult /= 2;
		}
		break;
	case CLASS_CIRNO:
		//v1.1.32 �`���m�̐��i�u���Ă������v�̓X�R�A����
		if(is_special_seikaku(SEIKAKU_SPECIAL_CIRNO)) mult /= 2;
		break;
	case CLASS_LUNASA:
	case CLASS_MERLIN:
	case CLASS_LYRICA:
		//�v���Y�����o�[�o���͈�x����サ�Ȃ��ƃX�R�A30%�A�b�v
		if(!p_ptr->magic_num2[75]) mult = mult * 13 / 10; 
		break;
	case CLASS_NARUMI:
		if(is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			mult = mult * 3 / 4;
		break;
	}
	*/

	if(finish_the_game && streq(p_ptr->died_from, "hourai"))
	{
		mult = mult * 4 / 3; 
	}

	mult /= 100;

	if(mult < 1) mult = 1;
if(test) msg_format("mult:%d",mult);

	point_h = point_l / 0x10000L;
	point_l = point_l % 0x10000L;
	point_h *= mult;
	point_l *= mult;
	point_h += point_l / 0x10000L;
	point_l %= 0x10000L;

	point_l += ((point_h % 100) << 16);
	point_h /= 100;
	point_l /= 100;
if(test) msg_format("point_h:%d",point_h);

	//v1.1.53c �X�R�A�����32bit����l�Ɋg��
	//if(point_h > 15257) point = 999999999;//paranoir
	if(point_h > 65535) point = 0xFFFFFFFFL;
	else point = (point_h << 16) + (point_l);

if(test) msg_format("TOTAL_SCORE:%lu",point);

	return point;

}


/*
 * Hack -- Calculates the total number of points earned		-JWT-
 */
///sys �X�R�A���v�Z����@�d�v
//v1.1.56 ������
#if 0
long total_points(void)
{
	int i, mult = 100;
	s16b max_dl = 0;
	u32b point, point_h, point_l;
	int arena_win = MIN(p_ptr->arena_number, MAX_ARENA_MONS);

	if (!preserve_mode) mult += 10;
	if (!autoroller) mult += 10;
	if (!smart_learn) mult -= 20;
	if (smart_cheat) mult += 30;
	if (ironman_shops) mult += 50;
	if (ironman_small_levels) mult += 10;
	if (ironman_empty_levels) mult += 20;
	if (!powerup_home) mult += 50;
	if (ironman_rooms) mult += 100;
	if (ironman_nightmare) mult += 100;

	if (mult < 5) mult = 5;

	for (i = 0; i < max_d_idx; i++)
		if(max_dlv[i] > max_dl)
			max_dl = max_dlv[i];

	point_l = (p_ptr->max_max_exp + (100 * max_dl));
	point_h = point_l / 0x10000L;
	point_l = point_l % 0x10000L;
	point_h *= mult;
	point_l *= mult;
	point_h += point_l / 0x10000L;
	point_l %= 0x10000L;

	point_l += ((point_h % 100) << 16);
	point_h /= 100;
	point_l /= 100;

	point = (point_h << 16) + (point_l);
	if (p_ptr->arena_number >= 0)
		point += (arena_win * arena_win * (arena_win > 29 ? 1000 : 100));

	if (ironman_downward) point *= 2;
	/*
	if (p_ptr->pclass == CLASS_BERSERKER)
	{
		if ( p_ptr->prace == RACE_SPECTRE )
			point = point / 5;
	}
	*/

	if ((p_ptr->pseikaku == SEIKAKU_MUNCHKIN) && point)
	{
		point = 1;
		if (p_ptr->total_winner) point = 2;
	}
	if (easy_band) point = (0 - point);

	return point;
}
#endif

#define GRAVE_LINE_WIDTH 31

/*
 * Centers a string within a GRAVE_LINE_WIDTH character string		-JWT-
 */
static void center_string(char *buf, cptr str)
{
	int i, j;

	/* Total length */
	i = strlen(str);

	/* Necessary border */
	j = GRAVE_LINE_WIDTH / 2 - i / 2;

	/* Mega-Hack */
	(void)sprintf(buf, "%*s%s%*s", j, "", str, GRAVE_LINE_WIDTH - i - j, "");
}


#if 0
/*
 * Save a "bones" file for a dead character
 *
 * Note that we will not use these files until Angband 2.8.0, and
 * then we will only use the name and level on which death occured.
 *
 * Should probably attempt some form of locking...
 */
static void make_bones(void)
{
	FILE                *fp;

	char                str[1024];


	/* Ignore wizards and borgs */
	if (!(p_ptr->noscore & 0x00FF))
	{
		/* Ignore people who die in town */
		if (dun_level)
		{
			char tmp[128];

			/* XXX XXX XXX "Bones" name */
			sprintf(tmp, "bone.%03d", dun_level);

			/* Build the filename */
			path_build(str, sizeof(str), ANGBAND_DIR_BONE, tmp);

			/* Attempt to open the bones file */
			fp = my_fopen(str, "r");

			/* Close it right away */
			if (fp) my_fclose(fp);

			/* Do not over-write a previous ghost */
			if (fp) return;

			/* File type is "TEXT" */
			FILE_TYPE(FILE_TYPE_TEXT);

			/* Grab permissions */
			safe_setuid_grab();

			/* Try to write a new "Bones File" */
			fp = my_fopen(str, "w");

			/* Drop permissions */
			safe_setuid_drop();

			/* Not allowed to write it?  Weird. */
			if (!fp) return;

			/* Save the info */
			fprintf(fp, "%s\n", player_name);
			fprintf(fp, "%d\n", p_ptr->mhp);
			fprintf(fp, "%d\n", p_ptr->prace);
			fprintf(fp, "%d\n", p_ptr->pclass);

			/* Close and save the Bones file */
			my_fclose(fp);
		}
	}
}
#endif


/*
 * Redefinable "print_tombstone" action
 */
bool (*tombstone_aux)(void) = NULL;


/*
 * Display a "tomb-stone"
 */
/*:::��΁A�����Ȃǂ�\�����镔��*/
///msg131221 ���ɂ�����郁�b�Z�[�W������������
static void print_tomb(void)
{
	bool done = FALSE;

#if 0
	/* Do we use a special tombstone ? */
	if (tombstone_aux)
	{
		/* Use tombstone hook */
		done = (*tombstone_aux)();
	}
#endif
	/* Print the text-tombstone */
	if (!done)
	{
		cptr   p;
		char   tmp[160];
		char   buf[1024];
		char   dummy[80];
		char   *t;
	//	FILE   *fp;
		time_t ct = time((time_t)0);
#ifdef JP
		int    extra_line = 0;
#endif

		/* Clear screen */
		Term_clear();
///mod140118 ��΍폜
#if 0
		/* Build the filename */
#ifdef JP
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "dead_j.txt");
#else
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "dead.txt");
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
				put_str(buf, i++, 0);
			}

			/* Close */
			my_fclose(fp);
		}
#endif

		/* King or Queen */
		///sys �������b�Z�[�W�@��Փx���ƂɁu�n�[�h���[�O���C�J�[�v�Ƃ�����悤��
		if (p_ptr->total_winner || (p_ptr->lev > PY_MAX_LEVEL))
		{
#ifdef JP
			/* �p���؂�ւ� */
			//p= "�̑�Ȃ��";
			if(difficulty == DIFFICULTY_EASY) p= "�C�[�W�[���[�O���C�J�[";
			else if(difficulty == DIFFICULTY_NORMAL) p= "�m�[�}�����[�O���C�J�[";
			else if(difficulty == DIFFICULTY_HARD) p= "�n�[�h���[�O���C�J�[";
			else if(difficulty == DIFFICULTY_LUNATIC) p= "���i�e�B�b�N���[�O���C�J�[";
			else  p= "�G�N�X�g�����[�O���C�J�[";
		center_string(buf, p);
		put_str(buf, 8, 11);

#else
			p = "Magnificent";
#endif
		}

		/* Normal */
		/*
		else
		{
			p =  player_title[p_ptr->pclass][(p_ptr->lev - 1) / 5];
		}
		*/


		center_string(buf, format("SCORE:%lu",total_points_new(FALSE)));
		put_str(buf, 4, 11);

		center_string(buf, player_name);
		put_str(buf, 6, 11);


#ifndef JP
		center_string(buf, "the");
		put_str(buf, 7, 11);
#endif

		//center_string(buf, p);
		//put_str(buf, 8, 11);

		//center_string(buf, cp_ptr->title);
		///mod140206 �E�Ɩ��ύX�Ή�
		center_string(buf, player_class_name);
		put_str(buf, 10, 11);

#ifdef JP
		(void)sprintf(tmp, "���x��: %d", (int)p_ptr->lev);
#else
		(void)sprintf(tmp, "Level: %d", (int)p_ptr->lev);
#endif
		center_string(buf, tmp);
		put_str(buf, 11, 11);

#ifdef JP
		(void)sprintf(tmp, "�o���l: %ld", (long)p_ptr->exp);
#else
		(void)sprintf(tmp, "Exp: %ld", (long)p_ptr->exp);
#endif
		center_string(buf, tmp);
		put_str(buf, 12, 11);

#ifdef JP
		(void)sprintf(tmp, "������: %ld", (long)p_ptr->au);
#else
		(void)sprintf(tmp, "AU: %ld", (long)p_ptr->au);
#endif
		center_string(buf, tmp);
		put_str(buf, 13, 11);

#ifdef JP
		/* ��ɍ��ތ��t���I���W�i�����ׂ����\�� */
		if (streq(p_ptr->died_from, "�r���I��"))
		{
			///msg131221
			strcpy(tmp, "<����>");
		}
		else if (streq(p_ptr->died_from, "ripe") && p_ptr->pclass == CLASS_OUTSIDER)
		{
			strcpy(tmp, "���z���ɉi�Z�����B");
		}
		else if (streq(p_ptr->died_from, "triumph"))
		{
			strcpy(tmp, "���z���̉p�Y�ƂȂ����̂��A�O�E�ւƋA�҂����B");
		}
		else if(streq(p_ptr->died_from, "hourai"))
		{
			sprintf(tmp, "�i���̖�����ɂ����B");
		}
		else if (streq(p_ptr->died_from, "true_loser"))
		{
			sprintf(tmp, "�������܂߂đS�Ăɔs�k�������炵���B");
		}
		else if(streq(p_ptr->died_from, "mission_failed"))
		{
			sprintf(tmp, "�P����肫��Ȃ������B");
		}
		else if (streq(p_ptr->died_from, "escape"))
		{
			strcpy(tmp, "���z������E�o�����B");
		}
		else if (streq(p_ptr->died_from, "lost"))
		{
			strcpy(tmp, "<����>");
		}
		else if (streq(p_ptr->died_from, "ripe"))
		{
			strcpy(tmp, "�������Q�[���N���A������");
		}
		else if (streq(p_ptr->died_from, "Seppuku"))
		{
			strcpy(tmp, "�����̌�A�ؕ�");
		}
		else
		{
			roff_to_buf(p_ptr->died_from, GRAVE_LINE_WIDTH + 1, tmp, sizeof tmp);
			t = tmp + strlen(tmp) + 1;
			if (*t)
			{
				strcpy(dummy, t); /* 2nd line */
				if (*(t + strlen(t) + 1)) /* Does 3rd line exist? */
				{
					for (t = dummy + strlen(dummy) - 2; iskanji(*(t - 1)); t--) /* Loop */;
					strcpy(t, "�c");
				}
				else if (my_strstr(tmp, "�w") && suffix(dummy, "�x"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "�w");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if (strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				else if (my_strstr(tmp, "�u") && suffix(dummy, "�v"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "�u");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if (strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				center_string(buf, dummy);
				put_str(buf, 15, 11);
				extra_line = 1;
			}
		}
		center_string(buf, tmp);
		put_str(buf, 14, 11);

		if (!streq(p_ptr->died_from, "ripe") && !streq(p_ptr->died_from, "true_loser") && !streq(p_ptr->died_from, "Seppuku") && !streq(p_ptr->died_from, "escape") && !streq(p_ptr->died_from, "triumph") && !streq(p_ptr->died_from, "lost") && !streq(p_ptr->died_from, "hourai")&& !streq(p_ptr->died_from, "mission_failed") )
		{
			if (dun_level == 0)
			{
				cptr locate_desc = p_ptr->town_num ? "�X" : "�r��";
				if (streq(p_ptr->died_from, "�r���I��"))
				{
					sprintf(tmp, "%s�œ|�ꂽ", locate_desc);
				}
				else
				{
					sprintf(tmp, "��%s�œ|���ꂽ", locate_desc);
				}
			}
			else
			{
				if (streq(p_ptr->died_from, "�r���I��"))
				{
					sprintf(tmp, "�n�� %d �K�œ|�ꂽ", dun_level);
				}
				else
				{
					sprintf(tmp, "�ɒn�� %d �K�œ|���ꂽ", dun_level);
				}
			}
			center_string(buf, tmp);
			put_str(buf, 15 + extra_line, 11);
		}
#else
		(void)sprintf(tmp, "Killed on Level %d", dun_level);
		center_string(buf, tmp);
		put_str(buf, 14, 11);

		roff_to_buf(format("by %s.", p_ptr->died_from), GRAVE_LINE_WIDTH + 1, tmp, sizeof tmp);
		center_string(buf, tmp);
		put_str(buf, 15, 11);
		t = tmp + strlen(tmp) + 1;
		if (*t)
		{
			strcpy(dummy, t); /* 2nd line */
			if (*(t + strlen(t) + 1)) /* Does 3rd line exist? */
			{
				int dummy_len = strlen(dummy);
				strcpy(dummy + MIN(dummy_len, GRAVE_LINE_WIDTH - 3), "...");
			}
			center_string(buf, dummy);
			put_str(buf, 16, 11);
		}
#endif

		(void)sprintf(tmp, "%-.24s", ctime(&ct));
		center_string(buf, tmp);
		put_str(buf, 17, 11);

#ifdef JP
		msg_format("���悤�Ȃ�A%s!", player_name);
#else
		msg_format("Goodbye, %s!", player_name);
#endif
	}
}


/*
 * Display some character info
 */
/*:::�_���v��ۑ��A�܂��͉�ʂɕ\������*/
static void show_info(void)
{
	int             i, j, k, l;
	object_type		*o_ptr;
	store_type		*st_ptr;

	int save_check_num = 100;

	/* Hack -- Know everything in the inven/equip */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Aware and Known */
		object_aware(o_ptr);
		object_known(o_ptr);
	}

	for (i = 1; i < max_towns; i++)
	{
		st_ptr = &town[i].store[STORE_HOME];

		/* Hack -- Know everything in the home */
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			o_ptr = &st_ptr->stock[j];

			/* Skip non-objects */
			if (!o_ptr->k_idx) continue;

			/* Aware and Known */
			object_aware(o_ptr);
			object_known(o_ptr);
		}
	}

	/* Hack -- Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Flush all input keys */
	flush();

	/* Flush messages */
	msg_print(NULL);


	/* Describe options */
///mod150403 �t�@�C���_���v�͎����I�ɍs���邱�Ƃɂ����B
	prt("�L�����N�^�[�̋L�^���t�@�C���ɏ����o����܂��B", 20, 0);
//	prt("�i���F����łȂ̂ŃX�R�A���M�͍s���܂���B�j", 22, 0);

#if 0
	prt("�L�����N�^�[�̋L�^���t�@�C���ɏ����o�����Ƃ��ł��܂��B", 20, 0);
	prt("���^�[���L�[�ŃL�����N�^�[�����܂��BESC�Œ��f���܂��B", 21, 0);
	/* Dump character records as requested */
	while (TRUE)
	{
		char out_val[160];

		/* Prompt */
#ifdef JP
put_str("�t�@�C���l�[��: ", 23, 0);
#else
		put_str("Filename: ", 23, 0);
#endif


		/* Default */
		strcpy(out_val, "");

		/* Ask for filename (or abort) */
		if (!askfor(out_val, 60)) return;

		/* Return means "show on screen" */
		if (!out_val[0]) break;

		/* Save screen */
		screen_save();

		/* Dump a character file */
		(void)file_character(out_val);

		/* Load screen */
		screen_load();
	}
#endif

	//v1.1.56 �e�X�g�v���C���Ă����Ƃ��ɂȂ����ُ�I�������̂ŔO�̈׃��[�v�ɉ񐔐���
	while (save_check_num--)
	{
		char out_val[160];

		strcpy(out_val, "");
		put_str("�t�@�C����: ", 23, 0);
		askfor(out_val, 80);

		if(!strlen(out_val))
		{
			if(auto_dump) strcpy(out_val, format("autodump_%s.txt",player_name));
			else break;
		}

		screen_save();
		//file_character��0���Ԃ���������
		if(!file_character(out_val)) break;
		screen_load();



	}
	screen_load();

	if (!save_check_num)
	{
		prt("ERROR:�L�����N�^�[�_���v��100�񎸔s���܂���", 20, 20);
	}
	
	update_playtime();

	/* Display player */
	display_player(0);

	/* Prompt for inventory */
#ifdef JP
prt("�����L�[�������Ƃ���ɏ�񂪑����܂� (ESC�Œ��f): ", 23, 0);
#else
	prt("Hit any key to see more information (ESC to abort): ", 23, 0);
#endif


	/* Allow abort at this point */
	if (inkey() == ESCAPE) return;


	/* Show equipment and inventory */

	/* Equipment -- if any */
	if (equip_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_equip(0);
#ifdef JP
prt("�������Ă����A�C�e��: -����-", 0, 0);
#else
		prt("You are using: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Inventory -- if any */
	if (inven_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_inven(0);
#ifdef JP
prt("�����Ă����A�C�e��: -����-", 0, 0);
#else
		prt("You are carrying: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Homes in the different towns */
	for (l = 1; l < max_towns; l++)
	{
		st_ptr = &town[l].store[STORE_HOME];

		/* Home -- if anything there */
		if (st_ptr->stock_num)
		{
			/* Display contents of the home */
			for (k = 0, i = 0; i < st_ptr->stock_num; k++)
			{
				/* Clear screen */
				Term_clear();

				/* Show 12 items */
				for (j = 0; (j < 12) && (i < st_ptr->stock_num); j++, i++)
				{
					char o_name[MAX_NLEN];
					char tmp_val[80];

					/* Acquire item */
					o_ptr = &st_ptr->stock[i];

					/* Print header, clear line */
					sprintf(tmp_val, "%c) ", I2A(j));
					prt(tmp_val, j+2, 4);

					/* Display object description */
					object_desc(o_name, o_ptr, 0);
					c_put_str(tval_to_attr[o_ptr->tval], o_name, j+2, 7);
				}

				/* Caption */
#ifdef JP
prt(format("���_�ɒu���Ă������A�C�e�� ( %d �y�[�W): -����-", k+1), 0, 0);
#else
				prt(format("Your home contains (page %d): -more-", k+1), 0, 0);
#endif


				/* Wait for it */
				if (inkey() == ESCAPE) return;
			}
		}
	}
}

/*:::�X�R�A�̓o�^���ł��邩�`�F�b�N*/
static bool check_score(void)
{
	/* Clear screen */
	Term_clear();


#ifdef SCORE_SERVER_TEST
	msg_print("�e�X�g�T�[�o�ɃX�R�A�𑗂�܂��B");
	return TRUE;
#endif

	/* No score file */
	if (highscore_fd < 0)
	{
#ifdef JP
msg_print("�X�R�A�E�t�@�C�����g�p�ł��܂���B");
#else
		msg_print("Score file unavailable.");
#endif

		msg_print(NULL);
		return FALSE;
	}

#ifndef SCORE_WIZARDS
	/* Wizard-mode pre-empts scoring */
	if (p_ptr->noscore & 0x000F)
	{
#ifdef JP
msg_print("�E�B�U�[�h�E���[�h�ł̓X�R�A���L�^����܂���B");
#else
		msg_print("Score not registered for wizards.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_BORGS
	/* Borg-mode pre-empts scoring */
	if (p_ptr->noscore & 0x00F0)
	{
#ifdef JP
msg_print("�{�[�O�E���[�h�ł̓X�R�A���L�^����܂���B");
#else
		msg_print("Score not registered for borgs.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_CHEATERS
	/* Cheaters are not scored */
	if (p_ptr->noscore & 0xFF00)
	{
#ifdef JP
msg_print("�`�[�g�t���O�������Ă��邽�߃X�R�A���L�^����܂���B");
//msg_print("���\��������l�̓X�R�A���L�^����܂���B");
#else
		msg_print("Score not registered for cheaters.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

	/* Interupted */
#ifdef JP
if (!p_ptr->total_winner && streq(p_ptr->died_from, "�����I��"))
#else
	if (!p_ptr->total_winner && streq(p_ptr->died_from, "Interrupting"))
#endif

	{
#ifdef JP
msg_print("�����I���̂��߃X�R�A���L�^����܂���B");
#else
		msg_print("Score not registered due to interruption.");
#endif

		msg_print(NULL);
		return FALSE;
	}

	/* Quitter */
#ifdef JP
if (!p_ptr->total_winner && streq(p_ptr->died_from, "�r���I��"))
#else
	if (!p_ptr->total_winner && streq(p_ptr->died_from, "Quitting"))
#endif

	{
#ifdef JP
msg_print("�r���I���̂��߃X�R�A���L�^����܂���B");
#else
		msg_print("Score not registered due to quitting.");
#endif

		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

/*
 * Close up the current game (player may or may not be dead)
 *
 * This function is called only from "main.c" and "signals.c".
 */
/*:::�Q�[���̏I������*/
///\131117 �X�R�A���M�폜
void close_game(void)
{
	char buf[1024];
	bool do_send = TRUE;

/*	cptr p = "[i:�L�����N�^�̏��, f:�t�@�C�������o��, t:�X�R�A, x:*�Ӓ�*, ESC:�Q�[���I��]"; */

	/* Handle stuff */
	handle_stuff();

	/* Flush the messages */
	msg_print(NULL);

	/* Flush the input */
	flush();


	/* No suspending now */
	signals_ignore_tstp();


	/* Hack -- Character is now "icky" */
	character_icky = TRUE;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	/* Grab permissions */
	safe_setuid_grab();

	/* Open the high score file, for reading/writing */
	highscore_fd = fd_open(buf, O_RDWR);

	/* Drop permissions */
	safe_setuid_drop();

	/* Handle death */
	if (p_ptr->is_dead)
	{
		/* Handle retirement */
		/*:::�����\��*/
		if (p_ptr->total_winner) kingly();

		/* Save memories */
#ifdef JP
		if (!cheat_save || get_check("�Q�[���I�[�o�[�f�[�^���Z�[�u���܂����H "))
#else
		if (!cheat_save || get_check("Save death? "))
#endif
		{

#ifdef JP
if (!save_player()) msg_print("�Z�[�u���s�I");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}
		else do_send = FALSE;

		/* You are dead */
		/*:::��Ε\��*/
		print_tomb();

		flush();

		/* Show more info */
		/*:::�_���v�̕ۑ����\��*/
		show_info();

		/* Clear screen */
		Term_clear();

		/*:::�X�R�A���M�̎��i�����邩�`�F�b�N*/
		if (check_score())
		{
			/*:::�X�R�A���M�����݂�*/
			if ((!send_world_score(do_send)))
			{
#ifdef JP
				if (get_check_strict("��ŃX�R�A��o�^���邽�߂ɑҋ@���܂����H", (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
#else
				if (get_check_strict("Stand by for later score registration? ", (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
#endif
				{
					p_ptr->wait_report_score = TRUE;
					p_ptr->is_dead = FALSE;
#ifdef JP
					if (!save_player()) msg_print("�Z�[�u���s�I");
#else
					if (!save_player()) msg_print("death save failed!");
#endif
				}
			}
			if (!p_ptr->wait_report_score)
				(void)top_twenty();
		}
		else if (highscore_fd >= 0)
		{
			display_scores_aux(0, 10, -1, NULL);
		}
#if 0
		/* Dump bones file */
		make_bones();
#endif
	}

	/* Still alive */
	else
	{
		/* Save the game */
		do_cmd_save_game(FALSE);

		/* Prompt for scores XXX XXX XXX */
#ifdef JP
prt("���^�[���L�[�� ESC �L�[�������ĉ������B", 0, 40);
#else
		prt("Press Return (or Escape).", 0, 40);
#endif

		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_EXIT);//v1.1.58


		/* Predict score (or ESCAPE) */
		if (inkey() != ESCAPE) predict_score();
	}


	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;

	/* Kill all temporal files */
	clear_saved_floor_files();

	/* Allow suspending now */
	signals_handle_tstp();
}


/*
 * Handle abrupt death of the visual system
 *
 * This routine is called only in very rare situations, and only
 * by certain visual systems, when they experience fatal errors.
 *
 * XXX XXX Hack -- clear the death flag when creating a HANGUP
 * save file so that player can see tombstone when restart.
 */
void exit_game_panic(void)
{
	/* If nothing important has happened, just quit */
#ifdef JP
	if (!character_generated || character_saved) quit("�ً}����");
#else
	if (!character_generated || character_saved) quit("panic");
#endif

	/* Mega-Hack -- see "msg_print()" */
	msg_flag = FALSE;

	/* Clear the top line */
	prt("", 0, 0);

	/* Hack -- turn off some things */
	disturb(1, 1);

	/* Mega-Hack -- Delay death */
	if (p_ptr->chp < 0) p_ptr->is_dead = FALSE;

	/* Hardcode panic save */
	p_ptr->panic_save = 1;

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Indicate panic save */
#ifdef JP
	(void)strcpy(p_ptr->died_from, "(�ً}�Z�[�u)");
#else
	(void)strcpy(p_ptr->died_from, "(panic save)");
#endif


	/* Panic save, or get worried */
#ifdef JP
	if (!save_player()) quit("�ً}�Z�[�u���s�I");
#else
	if (!save_player()) quit("panic save failed!");
#endif


	/* Successful panic save */
#ifdef JP
	quit("�ً}�Z�[�u�����I");
#else
	quit("panic save succeeded!");
#endif
}


/*
 * Get a random line from a file
 * Based on the monster speech patch by Matt Graham,
 */
/*:::���������痈���Ƃ��Ffile_name: a_high_j.txt�Ȃǁ@entry:artifact_bias�l *output:������t�@�C���� */
/*:::file_name:�ǂ܂��t�@�C����
 *:::entry:�֘A����C���f�b�N�X��
 *:::�ǂݍ��񂾃t�@�C���ɁuN:�����v�̍s������Aentry�Ƃ��̐�������v����΂��̍s���玟�̋󔒍s�܂łɂ���s�̓��e���烉���_���Ɉ�s���I�΂��炵��*/
/*:::"N:*"�̍s�ȍ~�̓f�t�H���g��*/
/*:::�������A�߂�l��0�ɂȂ邱�Ƃɒ���*/
errr get_rnd_line(cptr file_name, int entry, char *output)
{
	FILE    *fp;
	char    buf[1024];
	int     counter, test;
	int     line_num = 0;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, file_name);

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Failed */
	if (!fp) return -1;

	/* Find the entry of the monster */
	while (TRUE)
	{
		/* Get a line from the file */
		if (my_fgets(fp, buf, sizeof(buf)) == 0)
		{
			/* Count the lines */
			line_num++;

			/* Look for lines starting with 'N:' */
			if ((buf[0] == 'N') && (buf[1] == ':'))
			{
				/* Allow default lines */
				/*:::N:*�̓f�t�H���g���*/
				if (buf[2] == '*')
				{
					/* Default lines */
					break;
				}
				/*:::�����X�^�[�֘A�@entry�̃����X�^�[�C���f�b�N�X�̓G��male�̂Ƃ�*/
				else if (buf[2] == 'M')
				{
					if (r_info[entry].flags1 & RF1_MALE) break;
				}
				/*:::�����X�^�[�֘A�@entry�̃����X�^�[�C���f�b�N�X�̓G��female�̂Ƃ�*/
				else if (buf[2] == 'F')
				{
					if (r_info[entry].flags1 & RF1_FEMALE) break;
				}
				/* Get the monster number */
				/*:::entry�l��N:??�̐�������v�����Ƃ�*/
				else if (sscanf(&(buf[2]), "%d", &test) != EOF)
				{
					/* Is it the right number? */
					if (test == entry) break;
				}
				else
				{
					/* Error while converting the number */
					msg_format("Error in line %d of %s!", line_num, file_name);
					my_fclose(fp);
					return -1;
				}
			}
		}
		else
		{
			/* Reached end of file */
			my_fclose(fp);
			return -1;
		}
	}
	/*:::���̎��_�ŁAentry�l�Ŏw�肳�ꂽ���̍s���L���ꂽ�����̐擪��fp�����Ă���͂�*/
	/*:::��s���ǂݍ��݁AN:��#�Ŏn�܂�s�͖������A�󔒍s���t�@�C���I�[�܂łɓ���ꂽ�s�̂ǂꂩ��output�֊i�[*/
	/* Get the random line */
	for (counter = 0; ; counter++)
	{
		while (TRUE)
		{
			test = my_fgets(fp, buf, sizeof(buf));

			/* Count the lines */
			/* line_num++; No more needed */

			//�ǂݍ��ݐ���(my_fgets�͐�����0���Ԃ�)
			if (!test)
			{
				/* Ignore lines starting with 'N:' */
				if ((buf[0] == 'N') && (buf[1] == ':')) continue;
				/*:::#�Ŏn�܂�s��break�����A�Ăу��[�v��my_fgets()��*/
				if (buf[0] != '#') break;
			}
			else break;
		}

		/* Abort */
		/*:::�󔒍s���t�@�C���I�[�ŏI���H*/
		if (!buf[0]) break;

		/* Copy the line */
		if (one_in_(counter + 1)) strcpy(output, buf);
	}

	/* Close the file */
	my_fclose(fp);

	/* Success */
	return counter ? 0 : -1;
}


#ifdef JP
errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count)
{
	int  i, j, kanji;
	errr result = 1;

	for (i = 0; i < count; i++)
	{
		result = get_rnd_line(file_name, entry, output);
		if (result) break;
		kanji = 0;
		for (j = 0; output[j]; j++) kanji |= iskanji(output[j]);
		if (kanji) break;
	}
	return result;
}
#endif

/*
 * Process file for auto picker/destroyer.
 */
/*:::�����E���t�@�C���̓ǂݍ��݂�����*/
errr process_autopick_file(cptr name)
{
	char buf[1024];

	errr err = 0;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	err = process_pref_file_aux(buf, PREF_TYPE_AUTOPICK);

	/* Result */
	return (err);
}


/*
 * Process file for player's history editor.
 */
errr process_histpref_file(cptr name)
{
	char buf[1024];
	errr err = 0;
	bool old_character_xtra = character_xtra;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	/* Hack -- prevent modification birth options in this file */
	character_xtra = TRUE;

	err = process_pref_file_aux(buf, PREF_TYPE_HISTPREF);

	character_xtra = old_character_xtra;

	/* Result */
	return (err);
}

/*:::�I�[�v���ς݂̃o�C�i���t�@�C���̃|�C���^���w��̔Ԓn�܂Ői�߂�H*/
///system�@���̔N��ݒ肪�g���Ă���B��������폜���Ȃ��悤�ɒ��ӁB
static errr counts_seek(int fd, u32b where, bool flag)
{
	huge seekpoint;
	char temp1[128], temp2[128];
	u32b zero_header[3] = {0L, 0L, 0L};
	int i;

#ifdef SAVEFILE_USE_UID
	(void)sprintf(temp1, "%d.%s.%d%d%d", player_uid, savefile_base, p_ptr->pclass, p_ptr->pseikaku, p_ptr->age);
#else
	(void)sprintf(temp1, "%s.%d%d%d", savefile_base, p_ptr->pclass, p_ptr->pseikaku, p_ptr->age);
#endif
	/*:::char�̔z��̒��g��0�ŏ����������炵���B*/
	/*:::temp1[i]��char�l�̃r�b�g���K���ɔ��]�����肵�Ȃ������肵�Ă�H�ȈՈÍ������H*/
	for (i = 0; temp1[i]; i++)
		temp1[i] ^= (i+1) * 63;
	/*:::�ʓ|�ɂȂ����̂ł����œǂނ̂�߂�Bp_ptr�Ȃǂ̃r�b�g���𑝂₵���ꍇ���ӂ�Ȃ����`�F�b�N���Ȃ��Ƃ����Ȃ�����*/
	seekpoint = 0;
	while (1)
	{
		if (fd_seek(fd, seekpoint + 3 * sizeof(u32b)))
			return 1;
		if (fd_read(fd, (char*)(temp2), sizeof(temp2)))
		{
			if (!flag)
				return 1;
			/* add new name */
			fd_seek(fd, seekpoint);
			fd_write(fd, (char*)zero_header, 3*sizeof(u32b));
			fd_write(fd, (char*)(temp1), sizeof(temp1));
			break;
		}

		if (strcmp(temp1, temp2) == 0)
			break;

		seekpoint += 128 + 3 * sizeof(u32b);
	}

	return fd_seek(fd, seekpoint + where * sizeof(u32b));
}

/*:::�ȈՈÍ����ς݂�z_info_j.raw����w�肵���Ԓn�̐��l(32bit)��ǂݍ���ł���炵��*/
u32b counts_read(int where)
{
	int fd;
	u32b count = 0;
	char buf[1024];

#ifdef JP
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info_j.raw");
#else
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info.raw");
#endif
	fd = fd_open(buf, O_RDONLY);

	if (counts_seek(fd, where, FALSE) ||
	    fd_read(fd, (char*)(&count), sizeof(u32b)))
		count = 0;

	(void)fd_close(fd);

	return count;
}


/*:::z_info_j.raw���J���A�Í����ς݂̎w��Ԓn��32bit�̐��l����������ł���炵���@���Ԃ�s���Ď��p*/
errr counts_write(int where, u32b count)
{
	int fd;
	char buf[1024];
	errr err;

#ifdef JP
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info_j.raw");
#else
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, "z_info.raw");
#endif

	/* Grab permissions */
	/*:::���Ԃ�Unix��Linux�p�̌����m�F�@���ł͉�������Ă��Ȃ�*/
	safe_setuid_grab();

	/*:::0_RDWR��ReadAndWrite�̃t���O fd�ɂ̓t�@�C���f�X�N���v�^?���i�[�����*/
	fd = fd_open(buf, O_RDWR);

	/* Drop permissions */
	/*:::��������Ԃ�Unix��Linux�p�̌����֌W�@���ł͉�������Ă��Ȃ�*/
	safe_setuid_drop();

	/*:::z_info_j.raw�����݂��Ȃ��Ƃ�*/
	if (fd < 0)
	{
		/* File type is "DATA" */
		/*:::win�ł͊֌W�Ȃ��H*/
		FILE_TYPE(FILE_TYPE_DATA);

		/* Grab permissions */
		safe_setuid_grab();

		/* Create a new high score file */
		/*:::�o�C�i���t�@�C��z_info_j.raw���쐬����B0644�͍쐬�ғǂݏ�������ȊO�ǂݍ��݂̂݉̃p�[�~�b�V����*/
		fd = fd_make(buf, 0644);

		/* Drop permissions */
		safe_setuid_drop();
	}

	/* Grab permissions */
	safe_setuid_grab();

	/*:::�����UNIX��Linux��p�Ǝv����*/
	err = fd_lock(fd, F_WRLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if (err) return 1;

	/*:::�����r���܂ł����ǂ�łȂ� ����where�Ƃ͉��Ȃ̂��@�Ԓn�݂����Ȃ��̂�*/
	counts_seek(fd, where, TRUE);
	fd_write(fd, (char*)(&count), sizeof(u32b));

	/* Grab permissions */
	safe_setuid_grab();

	err = fd_lock(fd, F_UNLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if (err) return 1;

	(void)fd_close(fd);

	return 0;
}


#ifdef HANDLE_SIGNALS


#include <signal.h>


/*
 * Handle signals -- suspend
 *
 * Actually suspend the game, and then resume cleanly
 */
static void handle_signal_suspend(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);

#ifdef SIGSTOP

	/* Flush output */
	Term_fresh();

	/* Suspend the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 0);

	/* Suspend ourself */
	(void)kill(0, SIGSTOP);

	/* Resume the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 1);

	/* Redraw the term */
	Term_redraw();

	/* Flush the term */
	Term_fresh();

#endif

	/* Restore handler */
	(void)signal(sig, handle_signal_suspend);
}


/*
 * Handle signals -- simple (interrupt and quit)
 *
 * This function was causing a *huge* number of problems, so it has
 * been simplified greatly.  We keep a global variable which counts
 * the number of times the user attempts to kill the process, and
 * we commit suicide if the user does this a certain number of times.
 *
 * We attempt to give "feedback" to the user as he approaches the
 * suicide thresh-hold, but without penalizing accidental keypresses.
 *
 * To prevent messy accidents, we should reset this global variable
 * whenever the user enters a keypress, or something like that.
 */
static void handle_signal_simple(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	/* Count the signals */
	signal_count++;


	/* Terminate dead characters */
	if (p_ptr->is_dead)
	{
		/* Mark the savefile */
#ifdef JP
(void)strcpy(p_ptr->died_from, "�����I��");
#else
		(void)strcpy(p_ptr->died_from, "Abortion");
#endif

		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
quit("�����I��");
#else
		quit("interrupt");
#endif

	}

	/* Allow suicide (after 5) */
	else if (signal_count >= 5)
	{
		/* Cause of "death" */
#ifdef JP
(void)strcpy(p_ptr->died_from, "�����I����");
#else
		(void)strcpy(p_ptr->died_from, "Interrupting");
#endif


		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Stop playing */
		p_ptr->playing = FALSE;

		/* Suicide */
		p_ptr->is_dead = TRUE;

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
quit("�����I��");
#else
		quit("interrupt");
#endif

	}

	/* Give warning (after 4) */
	else if (signal_count >= 4)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);

		/* Clear the top line */
		Term_erase(0, 0, 255);

		/* Display the cause */
#ifdef JP
Term_putstr(0, 0, -1, TERM_WHITE, "�n���̏�̎��E�I");
#else
		Term_putstr(0, 0, -1, TERM_WHITE, "Contemplating suicide!");
#endif


		/* Flush */
		Term_fresh();
	}

	/* Give warning (after 2) */
	else if (signal_count >= 2)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);
	}

	/* Restore handler */
	(void)signal(sig, handle_signal_simple);
}


/*
 * Handle signal -- abort, kill, etc
 */
static void handle_signal_abort(int sig)
{
	int wid, hgt;

	Term_get_size(&wid, &hgt);

	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	forget_lite();
	forget_view();
	clear_mon_lite();

	/* Clear the bottom line */
	Term_erase(0, hgt - 1, 255);

	/* Give a warning */
	Term_putstr(0, hgt - 1, -1, TERM_RED,
#ifdef JP
"���낵���\�t�g�̃o�O����т������Ă����I");
#else
		    "A gruesome software bug LEAPS out at you!");
#endif


	/* Message */
#ifdef JP
Term_putstr(45, hgt - 1, -1, TERM_RED, "�ً}�Z�[�u...");
#else
	Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save...");
#endif


#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "----�Q�[���ُ�I��----");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "---- Panic Save and Abort Game ----");
#endif

	/* Flush output */
	Term_fresh();

	/* Panic Save */
	p_ptr->panic_save = 1;

	/* Panic save */
#ifdef JP
(void)strcpy(p_ptr->died_from, "(�ً}�Z�[�u)");
#else
	(void)strcpy(p_ptr->died_from, "(panic save)");
#endif


	/* Forbid suspend */
	signals_ignore_tstp();

	/* Attempt to save */
	if (save_player())
	{
#ifdef JP
Term_putstr(45, hgt - 1, -1, TERM_RED, "�ً}�Z�[�u�����I");
#else
		Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save succeeded!");
#endif

	}

	/* Save failed */
	else
	{
#ifdef JP
Term_putstr(45, hgt - 1, -1, TERM_RED, "�ً}�Z�[�u���s�I");
#else
		Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save failed!");
#endif

	}

	/* Flush output */
	Term_fresh();

	/* Quit */
#ifdef JP
quit("�\�t�g�̃o�O");
#else
	quit("software bug");
#endif

}




/*
 * Ignore SIGTSTP signals (keyboard suspend)
 */
void signals_ignore_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, SIG_IGN);
#endif

}

/*
 * Handle SIGTSTP signals (keyboard suspend)
 */
void signals_handle_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

}


/*
 * Prepare to handle the relevant signals
 */
void signals_init(void)
{

#ifdef SIGHUP
	(void)signal(SIGHUP, SIG_IGN);
#endif


#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif


#ifdef SIGINT
	(void)signal(SIGINT, handle_signal_simple);
#endif

#ifdef SIGQUIT
	(void)signal(SIGQUIT, handle_signal_simple);
#endif


#ifdef SIGFPE
	(void)signal(SIGFPE, handle_signal_abort);
#endif

#ifdef SIGILL
	(void)signal(SIGILL, handle_signal_abort);
#endif

#ifdef SIGTRAP
	(void)signal(SIGTRAP, handle_signal_abort);
#endif

#ifdef SIGIOT
	(void)signal(SIGIOT, handle_signal_abort);
#endif

#ifdef SIGKILL
	(void)signal(SIGKILL, handle_signal_abort);
#endif

#ifdef SIGBUS
	(void)signal(SIGBUS, handle_signal_abort);
#endif

#ifdef SIGSEGV
	(void)signal(SIGSEGV, handle_signal_abort);
#endif

#ifdef SIGTERM
	(void)signal(SIGTERM, handle_signal_abort);
#endif

#ifdef SIGPIPE
	(void)signal(SIGPIPE, handle_signal_abort);
#endif

#ifdef SIGEMT
	(void)signal(SIGEMT, handle_signal_abort);
#endif

#ifdef SIGDANGER
	(void)signal(SIGDANGER, handle_signal_abort);
#endif

#ifdef SIGSYS
	(void)signal(SIGSYS, handle_signal_abort);
#endif

#ifdef SIGXCPU
	(void)signal(SIGXCPU, handle_signal_abort);
#endif

#ifdef SIGPWR
	(void)signal(SIGPWR, handle_signal_abort);
#endif

}


#else	/* HANDLE_SIGNALS */


/*
 * Do nothing
 */
void signals_ignore_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_handle_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_init(void)
{
}
#endif	/* HANDLE_SIGNALS */

//�X�R�A�T�[�o�ɑ���v���C���[����o�^����
void get_score_server_name(void)
{

	bool need_set = FALSE;
	char tmp_name[SCORE_SERVER_STR_LEN] = "";
	char tmp_pass[SCORE_SERVER_STR_LEN] = "";
	int i;

	if(!strlen(score_server_name)) 
		need_set = TRUE;
	else if(!get_check_strict(format("�v���C���[�u%s�v�Ƃ��ăX�R�A�𑗂�܂��B",score_server_name), CHECK_DEFAULT_Y | CHECK_NO_HISTORY))
		need_set = TRUE;

	if(!need_set) return;

	screen_save();
	Term_clear();

	while(1)
	{
		prt("�u�v���C���[���v��ݒ肵�Ă��������B", 5,25);
		prt("�u�v���C���[���v�́A�X�R�A�T�[�o�ɕ\������܂��B", 6,25);
		prt("�ݒ肹���ɃX�R�A�𑗂邱�Ƃ��ł��܂��B", 7,25);
		prt("�ꕔ�̋L���͎g�p�ł��܂���B", 8,25);

		if (get_string("�v���C���[��: ", tmp_name, SCORE_SERVER_STR_LEN-4))
		{

			for (i = 0; tmp_name[i]; i++)
			{
				unsigned char c = tmp_name[i];

				if(iskanji(c) || iskana(c))
				{
					i++;
					continue;
				}
				else if (my_strchr("\"*,/:;<>?\\| ", c)) tmp_name[i] = '_';
			}
		}
		else
			strcpy(tmp_name,"");

		Term_clear();

		if(!strlen(tmp_name))
			prt("�v���C���[���͐ݒ肳��܂���B", 5,25);
		else
			prt(format("�v���C���[��:%s",tmp_name), 5,25);

		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y | CHECK_NO_HISTORY)) break;

	}
	//���܋L�^����Ă�̂ƈ�v���Ȃ�(strcmp��!=0)�Ƃ��ۑ�
	if(strcmp(score_server_name,tmp_name))
	{
		strcpy(score_server_name,tmp_name);
		//�X�R�A���M���O�Ȃ̂ŃZ�[�u���K�v
		if (!save_player()) msg_print("�Z�[�u���s�I");
	}

	screen_load();

}


//v1.1.42 �v���C���[�X�R�A�{��(%�P��)���v�Z����B
//�푰�E�E�ƁE���i�Ɋւ���{�����v�Z���邪�A��Փx�E�����I�v�V�����E�����o�߃y�i���e�B�Ȃǂ̔{���͌v�Z���Ȃ��B
//�X�R�A�v�Z�A�����Ɋւ�����́u���Ȃ��̃X�R�A�{���v�l�A�N�G�X�g�̓��ꃉ���_�����j�[�N�����X�^�[�̃p���[����Ɏg���B
int	calc_player_score_mult(void)
{
	int i;
	int mult;

	//�܂��푰�ɂ��X�R�A���v�Z����B���̎푰�̔{�������A����܂ł̎푰�ϗe�����ɂ����ƃX�R�A�̒Ⴂ�̂�����Ώ㏑������B
	mult = race_info[p_ptr->prace].score_mult;

	//v1.1.53b �d����t���H���̖�����񂾂Ƃ��푰�H���l�̎푰�{�����K�p����Ă��܂��Ă����̂Ŗ�t�o�g�̖H���l�̃X�R�A�{�������߂Ƃ�
	if (p_ptr->prace == RACE_HOURAI && p_ptr->pclass == CLASS_CHEMIST) mult = 200;

	for (i = 0; i < MAX_RACES; i++)
	{
		if (i < 32)
		{
			if (!(p_ptr->old_race1 & 1L << i) || mult < race_info[i].score_mult) continue;
		}
		else
		{
			if (!(p_ptr->old_race2 & 1L << (i - 32)) || mult < race_info[i].score_mult) continue;
		}
		mult = race_info[i].score_mult;
	}

	//�E�Ƃɂ��X�R�A�{�����悸��B
	mult = mult * class_info[p_ptr->pclass].score_mult / 100;

	//�ꕔ�̐E�Ƃ␫�i�ɂ����ꏈ�����s���B

	switch (p_ptr->pclass)
	{
	case CLASS_MOKOU:
		//���g�̓��U���N�V�����񐔂ŃX�R�A����
		if (p_ptr->magic_num1[0]) mult /= (1 + MIN(99, p_ptr->magic_num1[0]));
		break;
	case CLASS_KAGUYA:
		//�P��͏��������_��̐��ŃX�R�A����
		if (kaguya_quest_level && !ironman_no_fixed_art)
		{
			for (i = 0; i<kaguya_quest_level; i++) mult /= 2;
		}
		break;
	case CLASS_CIRNO:
		//v1.1.32 �`���m�̐��i�u���Ă������v�̓X�R�A����
		if (is_special_seikaku(SEIKAKU_SPECIAL_CIRNO)) mult /= 2;
		break;
	case CLASS_LUNASA:
	case CLASS_MERLIN:
	case CLASS_LYRICA:
		//�v���Y�����o�[�o���͈�x����サ�Ȃ���΃X�R�A30%�A�b�v
		if (!p_ptr->magic_num2[75]) mult = mult * 13 / 10;
		break;
	case CLASS_NARUMI:
		//�D��I�Ȑ����̓X�R�A-25%
		if (is_special_seikaku(SEIKAKU_SPECIAL_NARUMI))
			mult = mult * 3 / 4;
		break;
	case CLASS_3_FAIRIES:
		//v1.1.68 �d����W���̓X�R�A1/3
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			mult /= 3;
		break;

	case CLASS_MEGUMU:
		//v2.0.3 ��p���i�̓X�R�A1/4
		if (is_special_seikaku(SEIKAKU_SPECIAL_MEGUMU))
			mult /= 4;
		break;


	}

	//���i�u���C�́v�̓X�R�A����
	if (p_ptr->pseikaku == SEIKAKU_BERSERK)
	{
		mult /= 2;

	}


	if (mult<1) mult = 1;
	return	mult;
}