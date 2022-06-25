/* File: init1.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Initialization (part 1) -BEN- */

#include "angband.h"


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
 * Note that if "ALLOW_TEMPLATES" is not defined, then a lot of the code
 * in this file is compiled out, and the game will not run unless valid
 * "binary template files" already exist in "lib/data".  Thus, one can
 * compile Angband with ALLOW_TEMPLATES defined, run once to create the
 * "*.raw" files in "lib/data", and then quit, and recompile without
 * defining ALLOW_TEMPLATES, which will both save 20K and prevent people
 * from changing the ascii template files in potentially dangerous ways.
 *
 * The code could actually be removed and placed into a "stand-alone"
 * program, but that feels a little silly, especially considering some
 * of the platforms that we currently support.
 */

#ifdef ALLOW_TEMPLATES

#include "init.h"


/*** Helper arrays for parsing ascii template files ***/

/*
 * Monster Blow Methods
 */
///system 
/*:::�����X�^�[�Ō����� r_info�ɒǉ������炱���ɓ����@���̔z��̓Y����RBM_???�̒�`�l�ƈ�v���邱��*/
static cptr r_info_blow_method[] =
{
	"",
	"HIT",
	"TOUCH",
	"PUNCH",
	"KICK",
	"CLAW",
	"BITE",
	"STING",
	"SLASH",
	"BUTT",
	"CRUSH",
	"ENGULF",
	"CHARGE",
	"CRAWL",
	"DROOL",
	"SPIT",
	"EXPLODE",
	"GAZE",
	"WAIL",
	"SPORE",
	"DANCE",//v1.1.32
	"BEG",
	"INSULT",
	"MOAN",
	"SHOW",
		///mod131230 �U�����@�ǉ�
	"SPEAR",
	"BOOK",
	"MOFUMOFU",
	"SQUEEZE",
	"DRAG",
	"INJECT",
	"SMILE",
	"HEADBUTT",
	"FLASH",
	"HOWL",
	"WHISPER",
	"PRESS",
	"POINT",
	"STRIKE",
	"PLAY",
	"SHOOT",
	//v1.1.86
	"SMOKE",
	NULL
};


/*
 * Monster Blow Effects
 */
///system 
/*:::�����X�^�[�Ō����� r_info�ɒǉ������炱���ɓ����@���̔z��̓Y����RBE_???�̒�`�l�ƈ�v���邱��*/
static cptr r_info_blow_effect[] =
{
	"",
	"HURT",
	"POISON",
	"UN_BONUS",
	"UN_POWER",
	"EAT_GOLD",
	"EAT_ITEM",
	"EAT_FOOD",
	"EAT_LITE",
	"ACID",
	"ELEC",
	"FIRE",
	"COLD",
	"BLIND",
	"CONFUSE",
	"TERRIFY",
	"PARALYZE",
	"LOSE_STR",
	"LOSE_INT",
	"LOSE_WIS",
	"LOSE_DEX",
	"LOSE_CON",
	"LOSE_CHR",
	"LOSE_ALL",
	"SHATTER",
	"EXP_10",
	"EXP_20",
	"EXP_40",
	"EXP_80",
	"DISEASE",
	"TIME",
	"EXP_VAMP",
	"DR_MANA",
	"SUPERHURT",
	"INERTIA",
	"STUN",
	///mod131230 �U�����ʒǉ�
	"CHAOS",
	"ELEMENT",
	"MUTATE",
	"SMITE",
	"DROWN",
	"KILL",
	"CURSE",
	"PHOTO",
	"MELT",
	"BLEED",
	"INSANITY",
	"HUNGER",
	NULL
};


/*
 * Feature info flags
 */
//���̔z��̓Y����FF_***�̒l�Ɉ�v���Ă��邱��
static cptr f_info_flags[] =
{
	"LOS",
	"PROJECT",
	"MOVE",
	"PLACE",
	"DROP",
	"SECRET",
	"NOTICE",
	"REMEMBER",
	"OPEN",
	"CLOSE",
	"BASH",
	"SPIKE",
	"DISARM",
	"STORE",
	"TUNNEL",
	"MAY_HAVE_GOLD",
	"HAS_GOLD",
	"HAS_ITEM",
	"DOOR",
	"TRAP",
	"STAIRS",
	"GLYPH",
	"LESS",
	"MORE",
	"AVOID_RUN",
	"FLOOR",
	"WALL",
	"PERMANENT",
	"BOMB", ///mod150801
	"ELDER_SIGN",///mod160703
	"NEEDLE",
	"HIT_TRAP",

	"ICE_WALL",//v1.1.68 �V�n�`�u�X��v
	"COLD_PUDDLE", //v1.1.85
	"ELEC_PUDDLE",
	"ACID_PUDDLE",
	"POISON_PUDDLE",
	"GLOW",
	"ENSECRET",
	"WATER",
	"LAVA",
	"SHALLOW",
	"DEEP",
	"MIASMA", //v1.1.85
	"HURT_ROCK",
	"HURT_FIRE",
	"HURT_COLD",
	"HURT_ACID",
	"ICE",
	"ACID",
	"OIL",
	"PLATE",
	"OIL_FIELD",//v1.1.91
	"CAN_FLY",
	"CAN_SWIM",
	"CAN_PASS",
	"CAN_OOZE",
	"CAN_DIG",
	"HIDE_ITEM",
	"HIDE_SNEAK",
	"HIDE_SWIM",
	"HIDE_DIG",
	"KILL_HUGE",
	"KILL_MOVE",

	"PICK_TRAP",
	"PICK_DOOR",
	"ALLOC",
	"CHEST",
	"DROP_1D2",
	"DROP_2D2",
	"DROP_GOOD",
	"DROP_GREAT",
	"HURT_POIS",
	"HURT_ELEC",
	"HURT_WATER",
	"HURT_BWATER",
	"USE_FEAT",
	"GET_FEAT",
	"GROUND",
	"OUTSIDE",
	"EASY_HIDE",
	"EASY_CLIMB",
	"MUST_CLIMB",
	"TREE",
	"NEED_TREE",
	"BLOOD",
	"DUST",
	"SLIME",
	"PLANT",
	"SPIDER_NEST_1",
	"TEWI_PIT",
	"HANAKOSAN",
	"PORTAL",
	"ERUPT",
	"STRIKE",
	"SPREAD",

	"SPECIAL",
	"HURT_DISI",
	"QUEST_ENTER",
	"QUEST_EXIT",
	"QUEST",
	"SHAFT",
	"MOUNTAIN",
	"BLDG",
	"MINOR_GLYPH",
	"PATTERN",
	"TOWN",
	"ENTRANCE",
	"MIRROR",
	"UNPERM",
	"TELEPORTABLE",
	"CONVERT",
	"GLASS",
};


/*
 * Monster race flags
 */
 /*:::�ȉ����΂炭�����X�^�[�̃t���O�l
  *:::�����ɂ��镶����Ɉ�v������̂�r_info�ɂ���΁A����ID�̃����X�^�[��flags��
  *:::�Y���ʒu�̃t���O��ON�ɂȂ�B�����̕������RF?_*�̃t���O�萔�l�Ɉ�v����Y�����̏ꏊ�ɂ��邱��*/
static cptr r_info_flags1[] =
{
	"UNIQUE",
	"QUESTOR",
	"MALE",
	"FEMALE",
	"CHAR_CLEAR",
	"SHAPECHANGER",
	"ATTR_CLEAR",
	"ATTR_MULTI",
	"FORCE_DEPTH",
	"FORCE_MAXHP",
	"FORCE_SLEEP",
	"FORCE_EXTRA",
	"ATTR_SEMIRAND",
	"FRIENDS",
	"ESCORT",
	"ESCORTS",
	"NEVER_BLOW",
	"NEVER_MOVE",
	"RAND_25",
	"RAND_50",
	"ONLY_GOLD",
	"ONLY_ITEM",
	"DROP_60",
	"DROP_90",
	"DROP_1D2",
	"DROP_2D2",
	"DROP_3D2",
	"DROP_4D2",
	"DROP_GOOD",
	"DROP_GREAT",
	"FLAG_50F",
	"AISATSU"
};

/*
 * Monster race flags
 */
static cptr r_info_flags2[] =
{
	"STUPID",
	"SMART",
	"CAN_SPEAK",
	"REFLECTING",
	"INVISIBLE",
	"COLD_BLOOD",
	"EMPTY_MIND",
	"WEIRD_MIND",
	"MULTIPLY",
	"REGENERATE",
	"CHAR_MULTI",
	"ATTR_ANY",
	"POWERFUL",
	"ELDRITCH_HORROR",
	"AURA_FIRE",
	"AURA_ELEC",
	"OPEN_DOOR",
	"BASH_DOOR",
	"PASS_WALL",
	"KILL_WALL",
	"MOVE_BODY",
	"KILL_BODY",
	"TAKE_ITEM",
	"KILL_ITEM",
	"AURA_COLD",
	"TROPHY",
	"SMART_EX",
	"TELE_AFTER",
	"GIGANTIC",
	"PHANTOM",
	"EPHEMERA",
	"QUANTUM"
};

/*
 * Monster race flags
 */
static cptr r_info_flags3[] =
{
	"KWAI",
	"DEITY",
	"DEMIHUMAN",
	"DRAGON",
	"DEMON",
	"UNDEAD",
	"NONLIVING",
	"ANIMAL",
	"HUMAN",
	"FAIRY",
	"HANIWA",
	"ANG_CHAOS",
	"ANG_COSMOS",
	"ANG_AMBER",
	"ANG_WARLIKE",
	"ANG_NEUTRAL",
	"ANG_FRIENDLY",
	"GEN_HUMAN",
	"GEN_KWAI",
	"GEN_NEUTRAL",
	"GEN_WARLIKE",
	"GEN_FRIENDLY",
	"GEN_MOON",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"WANTED",
	"NO_FEAR",
	"NO_STUN",
	"NO_CONF",
	"NO_SLEEP"
};

/*
 * Monster race flags
 */
static cptr r_info_flags4[] =
{
	"SHRIEK",
	"DANMAKU",
	"DISPEL",
	"ROCKET",
	"SHOOT",
	"BR_HOLY",
	"BR_HELL",
	"BR_AQUA",
	"BR_ACID",
	"BR_ELEC",
	"BR_FIRE",
	"BR_COLD",
	"BR_POIS",
	"BR_NETH",
	"BR_LITE",
	"BR_DARK",
	"WAVEMOTION",
	"BR_SOUN",
	"BR_CHAO",
	"BR_DISE",
	"BR_NEXU",
	"BR_TIME",
	"BR_INER",
	"BR_GRAV",
	"BR_SHAR",
	"BR_PLAS",
	"BA_FORCE",
	"BR_MANA",
	"SPECIAL2",
	"BR_NUKE",
	"BA_CHAO",
	"BR_DISI",
};

/*
 * Monster race flags
 */
static cptr r_info_flags5[] =
{
	"BA_ACID",
	"BA_ELEC",
	"BA_FIRE",
	"BA_COLD",
	"BA_POIS",
	"BA_NETH",
	"BA_WATE",
	"BA_MANA",
	"BA_DARK",
	"DRAIN_MANA",
	"MIND_BLAST",
	"BRAIN_SMASH",
	"CAUSE_1",
	"CAUSE_2",
	"CAUSE_3",
	"CAUSE_4",
	"BO_ACID",
	"BO_ELEC",
	"BO_FIRE",
	"BO_COLD",
	"BA_LITE",
	"BO_NETH",
	"BO_WATE",
	"BO_MANA",
	"BO_PLAS",
	"BO_ICEE",
	"MISSILE",
	"SCARE",
	"BLIND",
	"CONF",
	"SLOW",
	"HOLD"
};

/*
 * Monster race flags
 */
static cptr r_info_flags6[] =
{
	"HASTE",
	"HAND_DOOM",
	"HEAL",
	"INVULNER",
	"BLINK",
	"TPORT",
	"WORLD",
	"SPECIAL",
	"TELE_TO",
	"TELE_AWAY",
	"TELE_LEVEL",
	"PSY_SPEAR",
	"DARKNESS",
	"TRAPS",
	"FORGET",
	"COSMIC_HORROR",
	"S_KIN",
	"S_CYBER",
	"S_MONSTER",
	"S_MONSTERS",
	"S_ANT",
	"S_SPIDER",
	"S_HOUND",
	"S_HYDRA",
	"S_ANGEL",
	"S_DEMON",
	"S_UNDEAD",
	"S_DRAGON",
	"S_HI_UNDEAD",
	"S_HI_DRAGON",
	"S_AMBERITES",
	"S_UNIQUE"
};


/*
 * Monster race flags
 */
static cptr r_info_flags7[] =
{
	"AQUATIC",
	"CAN_SWIM",
	"CAN_FLY",
	"FRIENDLY",
	"NAZGUL",
	"UNIQUE2",
	"RIDING",
	"KAGE",
	"HAS_LITE_1",
	"SELF_LITE_1",
	"HAS_LITE_2",
	"SELF_LITE_2",
	"GUARDIAN",
	"CHAMELEON",
	"KILL_EXP",
	"TANUKI",
	"HAS_DARK_1",
	"SELF_DARK_1",
	"HAS_DARK_2",
	"SELF_DARK_2",
	"FULL_MOON",
	"FROM_HELL",
	"VARIABLE",
	"DOPPELGANGER",
	"ONLY_RIDING",
	"FORCE_GO_STRAIGHT",
	"XXX7X26",
	"XXX7X27",
	"XXX7X28",
	"XXX7X29",
	"XXX7X30",
	"XXX7X31",
};

/*
 * Monster race flags
 */
static cptr r_info_flags8[] =
{
	"WILD_ONLY",
	"WILD_TOWN",
	"XXX8X02",
	"WILD_SHORE",
	"WILD_OCEAN",
	"WILD_WASTE",
	"WILD_WOOD",
	"WILD_VOLCANO",
	"XXX8X08",
	"WILD_MOUNTAIN",
	"WILD_GRASS",
	"STAY_AT_HOME1",
	"STAY_AT_HOME2",
	"STAY_AT_HOME3",
	"XXX8X14",
	"XXX8X15",
	"XXX8X16",
	"XXX8X17",
	"XXX8X18",
	"XXX8X19",
	"XXX8X20",
	"XXX8X21",
	"XXX8X22",
	"XXX8X23",
	"XXX8X24",
	"XXX8X25",
	"XXX8X26",
	"XXX8X27",
	"XXX8X28",
	"XXX8X29",
	"WILD_SWAMP",	/* ToDo: Implement Swamp */
	"WILD_ALL",
};


/*
 * Monster race flags - Drops
 */
static cptr r_info_flags9[] =
{
	"FIRE_STORM",
	"ICE_STORM",
	"THUNDER_STORM",
	"ACID_STORM",
	"TOXIC_STORM",
	"BA_POLLUTE",
	"BA_DISI",
	"BA_HOLY",
	"BA_METEOR",
	"BA_DISTORTION",
	"PUNISH_1",
	"PUNISH_2",
	"PUNISH_3",
	"PUNISH_4",
	"BO_HOLY",
	"GIGANTIC_LASER",
	"BO_SOUND",
	"S_ANIMAL",
	"BEAM_LITE",
	"SONICWAVE",
	"HELLFIRE",
	"HOLYFIRE",
	"FINALSPARK",
	"TORNADO",
	"DESTRUCTION",
	"S_DEITY",
	"S_HI_DEMON",
	"S_KWAI",
	"TELE_APPROACH",
	"TELE_HI_APPROACH",
	"MAELSTROM",
	"ALARM",
};


/*
 * Monster race flags - Resistances
 */
static cptr r_info_flagsr[] =
{
	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"IM_POIS",
	"RES_LITE",
	"RES_DARK",
	"RES_NETH",
	"RES_WATE",
	"RES_PLAS",//���͎g���Ă��Ȃ��͂�
	"RES_SHAR",
	"RES_SOUN",
	"RES_CHAO",
	"RES_HOLY",
	"RES_DISE",
	"RES_WALL",//���͎g���Ă��Ȃ��͂�
	"RES_INER",
	"RES_TIME",
	"RES_GRAV",//���͎g���Ă��Ȃ��͂�
	"RES_ALL",
	"RES_TELE",
	"XXX",
	"XXX",
	"XXX",
	"HURT_HOLY",
	"HURT_HELL",
	"HURT_WATER",
	"HURT_ELEC",
	"HURT_COLD",
	"HURT_FIRE",
	"HURT_ROCK",
	"HURT_LITE",
};


/*
 * Object flags
 */
/*:::parse_k_info()����Ă΂��*/
/*:::k/e/a_info��"STR"�Ȃǂ̕�������A�C�e���t���O�lTR_STR�Ȃǂɕϊ����邽�߂̔z��*/
/*:::���̔z��̓Y�����ƕ������TR_???�̐��l�͈�v���Ă��邱��*/
///mod131228 item �A�C�e���t���O����ւ�
static cptr k_info_flags[] =
{
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHR",

	"MAGIC_MASTERY",
	"STEALTH",
	"SEARCH",
	"INFRA",
	"TUNNEL",
	"SPEED",
	"BLOWS",

	"CHAOTIC",
	"VAMPIRIC",
	"FORCE_WEAPON",
	"VORPAL",
	"EX_VORPAL",
	"IMPACT",
	"BRAND_POIS",
	"BRAND_ACID",
	"BRAND_ELEC",
	"BRAND_FIRE",
	"BRAND_COLD",

	"SLAY_EVIL",
	"SLAY_GOOD",
	"SLAY_ANIMAL",
	"SLAY_HUMAN",
	"SLAY_UNDEAD",
	"SLAY_DRAGON",
	"SLAY_DEITY",
	"SLAY_DEMON",
	"SLAY_KWAI",

	"KILL_EVIL",
	"KILL_GOOD",
	"KILL_ANIMAL",
	"KILL_HUMAN",
	"KILL_UNDEAD",
	"KILL_DRAGON",
	"KILL_DEITY",
	"KILL_DEMON",
	"KILL_KWAI",

	"ESP_EVIL",
	"ESP_GOOD",
	"ESP_ANIMAL",
	"ESP_HUMAN",
	"ESP_UNDEAD",
	"ESP_DRAGON",
	"ESP_DEITY",
	"ESP_DEMON",
	"ESP_KWAI",
	"ESP_NONLIVING",
	"ESP_UNIQUE",

	"SUST_STR",
	"SUST_INT",
	"SUST_WIS",
	"SUST_DEX",
	"SUST_CON",
	"SUST_CHR",

	"EASY_SPELL",
	"DEC_MANA",
	"XTRA_MIGHT",
	"XTRA_SHOTS",

	"THROW",
	"RIDING",
	"REFLECT",

	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"RES_ACID",
	"RES_ELEC",
	"RES_FIRE",
	"RES_COLD",
	"RES_POIS",

	"RES_LITE",
	"RES_DARK",
	"RES_SHARDS",
	"RES_SOUND",
	"RES_NETHER",
	"RES_WATER",
	"RES_CHAOS",
	"RES_DISEN",
	"RES_HOLY",
	"RES_TIME",

	"FREE_ACT",
	"RES_FEAR",
	"RES_BLIND",
	"RES_CONF",
	"RES_INSANITY",

	"LEVITATION",
	"SEE_INVIS",
	"SLOW_DIGEST",
	"REGEN",
	"LITE",
	"WARNING",
	"BLESSED",
	"TELEPORT",

	"GENZI",
	"SPEEDSTER",
	"TELEPATHY",

	"SH_FIRE",
	"SH_ELEC",
	"SH_COLD",
	"NO_TELE",
	"NO_MAGIC",

	"TY_CURSE",
	"AGGRAVATE",
	"DRAIN_EXP",
	"ADD_L_CURSE",
	"ADD_H_CURSE",

	"IGNORE_ACID",
	"IGNORE_ELEC",
	"IGNORE_FIRE",
	"IGNORE_COLD",

	"ACTIVATE",
	"MATERIAL",
	"USABLE",
	"BOOMERANG",
	"READABLE",

	"ES_ATTACK", //�b��p����t���O
	"ES_AC", //�b��p����t���O

	"SHOW_MODS",
	"HIDE_TYPE",
	"FULL_NAME",
	"FIXED_FLAVOR",
	"HOUSE", //v1.1.79
};


static cptr k_info_gen_flags[] =
{
	"INSTA_ART",
	"QUESTITEM",
	"XTRA_POWER",
	"ONE_SUSTAIN",
	"XTRA_RES_OR_POWER",
	"XTRA_H_RES",
	"XTRA_E_RES",
	"XTRA_L_RES",
	"XTRA_D_RES",
	"XTRA_RES",
	"CURSED",
	"HEAVY_CURSE",
	"PERMA_CURSE",
	"RANDOM_CURSE0",
	"RANDOM_CURSE1",
	"RANDOM_CURSE2",
	"XTRA_DICE",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
};

/*
 * Dungeon flags
 */
static cptr d_info_flags1[] =
{
	"WINNER",
	"MAZE",
	"SMALLEST",
	"BEGINNER",
	"BIG",
	"NO_DOORS",
	"WATER_RIVER",
	"LAVA_RIVER",
	"CURTAIN",
	"GLASS_DOOR",
	"CAVE",
	"CAVERN",
	"ARCADE",
	"DREAM_WORLD", ///mod150906
	"SMALL",
	"XXX",
	"FORGET",
	"LAKE_WATER",
	"LAKE_LAVA",
	"LAKE_RUBBLE",
	"LAKE_TREE",
	"NO_VAULT",
	"ARENA",
	"DESTROY",
	"GLASS_ROOM",
	"NO_CAVE",
	"NO_MAGIC",
	"NO_MELEE",
	"CHAMELEON",
	"DARKNESS",
	"NEW_RIVER", //v1.1.85 
	"NEW_LAKE"
};


/*
 * Add a text to the text-storage and store offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the text.
 */
/*:::offset�̈ʒu�ɑ΂�buf�̕�������R�s�[���Aoffset�����̕��i�߁Ahead->text_size�𕶎��������Z*/
static bool add_text(u32b *offset, header *head, cptr buf, bool normal_text)
{
	/* Hack -- Verify space */
	if (head->text_size + strlen(buf) + 8 > FAKE_TEXT_SIZE)
		return (FALSE);

	/* New text? */
	if (*offset == 0)
	{
		/* Advance and save the text index */
		*offset = ++head->text_size;
	}

	/* Additional text */
	else if (normal_text)
	{
		/*
		 * If neither the end of the last line nor
		 * the beginning of current line is not a space,
		 * fill up a space as a correct separator of two words.
		 */
		if (head->text_size > 0 &&
#ifdef JP
		    (*(head->text_ptr + head->text_size - 1) != ' ') &&
		    ((head->text_size == 1) || !iskanji(*(head->text_ptr + head->text_size - 2))) && 
		    (buf[0] != ' ') && !iskanji(buf[0])
#else
		    (*(head->text_ptr + head->text_size - 1) != ' ') &&
		    (buf[0] != ' ')
#endif
		    )
		{
			/* Append a space */
			*(head->text_ptr + head->text_size) = ' ';

			/* Advance the index */
			head->text_size++;
		}
	}

	/* Append chars to the text */
	strcpy(head->text_ptr + head->text_size, buf);

	/* Advance the index */
	head->text_size += strlen(buf);

	/* Success */
	return (TRUE);
}


/*
 * Add a name to the name-storage and return an offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the name.
 */
/*:::offset�̈ʒu�ɑ΂�buf�̕�������R�s�[���Aheader.name_size�𕶎��������Z����*/
static bool add_name(u32b *offset, header *head, cptr buf)
{
	/* Hack -- Verify space */

	if (head->name_size + strlen(buf) + 8 > FAKE_NAME_SIZE)
	{
		//msg_format("%d",(head->name_size + strlen(buf) + 8));
		return (FALSE);
	}

	/* New name? */
	if (*offset == 0)
	{
		/* Advance and save the name index */
		*offset = ++head->name_size;
	}

	/* Append chars to the names */
	strcpy(head->name_ptr + head->name_size, buf);

	/* Advance the index */
	head->name_size += strlen(buf);

	/* Success */
	return (TRUE);
}


/*
 * Add a tag to the tag-storage and return an offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the name.
 */
static bool add_tag(s16b *offset, header *head, cptr buf)
{
	u32b i;

	/* Search for an existing (fake) tag */
	for (i = 1; i < head->tag_size; i += strlen(&head->tag_ptr[i]) + 1)
	{
		/* Found it */
		if (streq(&head->tag_ptr[i], buf)) break;
	}

	/* There was no existing tag */
	if (i >= head->tag_size)
	{
		/* Hack -- Verify space */
		if (head->tag_size + strlen(buf) + 8 > FAKE_TAG_SIZE)
			return FALSE;

		/* Append chars to the tags */
		strcpy(head->tag_ptr + head->tag_size, buf);

		/* Point the new tag */
		i = head->tag_size;

		/* Advance the index */
		head->tag_size += strlen(buf) + 1;
	}

	/* Return offset of the tag */
	*offset = (s16b)i;

	/* Success */
	return TRUE;
}


/*
 * Convert a "color letter" into an "actual" color
 * The colors are: dwsorgbuDWvyRGBU, as shown below
 */
byte color_char_to_attr(char c)
{
	switch (c)
	{
		case 'd': return (TERM_DARK);
		case 'w': return (TERM_WHITE);
		case 's': return (TERM_SLATE);
		case 'o': return (TERM_ORANGE);
		case 'r': return (TERM_RED);
		case 'g': return (TERM_GREEN);
		case 'b': return (TERM_BLUE);
		case 'u': return (TERM_UMBER);

		case 'D': return (TERM_L_DARK);
		case 'W': return (TERM_L_WHITE);
		case 'v': return (TERM_VIOLET);
		case 'y': return (TERM_YELLOW);
		case 'R': return (TERM_L_RED);
		case 'G': return (TERM_L_GREEN);
		case 'B': return (TERM_L_BLUE);
		case 'U': return (TERM_L_UMBER);
	}

	return (255);
}



/*** Initialize from ascii template files ***/


/*
 * Initialize an "*_info" array, by parsing an ascii "template" file
 */
/*:::fp�ɊJ���Ă���e�L�X�g�t�@�C�����w�肵���֐��ň�s����������*/
errr init_info_txt(FILE *fp, char *buf, header *head,
		   parse_info_txt_func parse_info_txt_line)
{
	errr err;

	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = 0;


	/* Prepare the "fake" stuff */
	head->name_size = 0;
	head->text_size = 0;
	head->tag_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}

		/* Mega Hack -- Calculate Check Sum */
		if (buf[0] != 'N' && buf[0] != 'D')
		{
			int i;
			for (i = 0; buf[i]; i++)
			{
				head->v_extra += (byte)buf[i];
				head->v_extra ^= (1 << (i % 8));
			}
		}

		/* Parse the line */
		if ((err = (*parse_info_txt_line)(buf, head)) != 0)
			return (err);
	}


	/* Complete the "name" and "text" sizes */
	if (head->name_size) head->name_size++;
	if (head->text_size) head->text_size++;

	/* Success */
	return (0);
}


/*
 * Initialize the "v_info" array, by parsing an ascii "template" file
 */
errr parse_v_info(char *buf, header *head)
{
	int i;
	char *s;

	/* Current entry */
	static vault_type *v_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (1);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		v_ptr = &v_info[i];

		/* Store the name */
		if (!add_name(&v_ptr->name, head, s)) return (7);
	}

	/* There better be a current v_ptr */
	else if (!v_ptr) return (3);

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* Acquire the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&v_ptr->text, head, s, FALSE)) return (7);
	}

	/* Process 'X' for "Extra info" (one line only) */
	else if (buf[0] == 'X')
	{
		int typ, rat, hgt, wid;

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			&typ, &rat, &hgt, &wid)) return (1);

		/* Save the values */
		v_ptr->typ = typ;
		v_ptr->rat = rat;
		v_ptr->hgt = hgt;
		v_ptr->wid = wid;
	}

	/* Oops */
	else	return (6);

	/* Success */
	return (0);
}



/*
 * Initialize the "s_info" array, by parsing an ascii "template" file
 */
/*:::s_info.txt�̍s���Ɖ���*/
#if 0
errr parse_s_info(char *buf, header *head)
{
	int i;

	/* Current entry */
	static skill_table *s_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Get the index */
		i = atoi(buf+2);

			/* Verify information */
		if (i <= error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		s_ptr = &s_info[i];
	}

	/* There better be a current s_ptr */
	else if (!s_ptr) return (3);

	/* Process 'W' for "Weapon exp" */
	else if (buf[0] == 'W')
	{
		int tval, sval, start, max;
		const s16b exp_conv_table[] =
		{
			WEAPON_EXP_UNSKILLED, WEAPON_EXP_BEGINNER, WEAPON_EXP_SKILLED,
			WEAPON_EXP_EXPERT, WEAPON_EXP_MASTER
		};

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
				&tval, &sval, &start, &max)) return (1);

		if (start < EXP_LEVEL_UNSKILLED || start > EXP_LEVEL_MASTER
			|| max < EXP_LEVEL_UNSKILLED || max > EXP_LEVEL_MASTER) return (8);

		/* Save the values */
		s_ptr->w_start[tval][sval] = exp_conv_table[start];
		s_ptr->w_max[tval][sval] = exp_conv_table[max];
	}

	/* Process 'S' for "Skill exp" */
	else if (buf[0] == 'S')
	{
		int num, start, max;

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
				&num, &start, &max)) return (1);

		if (start < WEAPON_EXP_UNSKILLED || start > WEAPON_EXP_MASTER
			|| max < WEAPON_EXP_UNSKILLED || max > WEAPON_EXP_MASTER) return (8);

		/* Save the values */
		s_ptr->s_start[num] = start;
		s_ptr->s_max[num] = max;
	}


	/* Oops */
	else return (6);

	/* Success */
	return (0);
}
#endif

/*
 * Initialize the "m_info" array, by parsing an ascii "template" file
 */
/*:::m_info.txt�̍s����*/
////del140208 ���@�d�l�ύX�ɂ��폜
#if 0
errr parse_m_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static player_magic *m_ptr = NULL;

	/* ---Hack--- */
	static int realm, magic_idx = 0, readable = 0;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Get the index */
		i = atoi(buf+2);

			/* Verify information */
		if (i <= error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		m_ptr = &m_info[i];
	}

	/* There better be a current m_ptr */
	else if (!m_ptr) return (3);

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		char *book, *stat;
		int xtra, type, first, weight;

		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		book = buf+2;

		if (streq(book, "SORCERY")) m_ptr->spell_book = TV_SORCERY_BOOK;
		else if (streq(book, "LIFE")) m_ptr->spell_book = TV_LIFE_BOOK;
		else if (streq(book, "MUSIC")) m_ptr->spell_book = TV_MUSIC_BOOK;
		else if (streq(book, "HISSATSU")) m_ptr->spell_book = TV_HISSATSU_BOOK;
		else if (streq(book, "NONE")) m_ptr->spell_book = 0;
		else return (5);

		stat = s;

		/* Find the colon before the name */
		s = my_strchr(s, ':');

		/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		if (streq(stat, "STR")) m_ptr->spell_stat = A_STR;
		else if (streq(stat, "INT")) m_ptr->spell_stat = A_INT;
		else if (streq(stat, "WIS")) m_ptr->spell_stat = A_WIS;
		else if (streq(stat, "DEX")) m_ptr->spell_stat = A_DEX;
		else if (streq(stat, "CON")) m_ptr->spell_stat = A_CON;
		else if (streq(stat, "CHR")) m_ptr->spell_stat = A_CHR;
		else return (5);


		/* Scan for the values */
		if (4 != sscanf(s, "%x:%d:%d:%d",
				(uint *)&xtra, &type, &first, &weight))	return (1);

		m_ptr->spell_xtra = xtra;
		m_ptr->spell_type = type;
		m_ptr->spell_first = first;
		m_ptr->spell_weight = weight;
	}


	/* Process 'R' for "Realm" (one line only) */
	else if (buf[0] == 'R')
	{
		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
				&realm, &readable)) return (1);

		magic_idx = 0;
	}

	else if (buf[0] == 'T')
	{
		int level, mana, fail, exp;

		if (!readable) return (1);
		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
				&level, &mana, &fail, &exp)) return (1);

		m_ptr->info[realm][magic_idx].slevel = level;
		m_ptr->info[realm][magic_idx].smana = mana;
		m_ptr->info[realm][magic_idx].sfail = fail;
		m_ptr->info[realm][magic_idx].sexp = exp;
		magic_idx ++;
	}


	/* Oops */
	else return (6);

	/* Success */
	return (0);
}
#endif

/*
 * Grab one flag from a textual string
 */
/*:::������what���w�肳�ꂽnames[]����T���A
 *:::���������ꏊ��names[]�̓Y�����V�t�g����flags�̃r�b�g��1�ɂ���*/
static errr grab_one_flag(u32b *flags, cptr names[], cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, names[i]))
		{
			*flags |= (1L << i);
			return 0;
		}
	}

	return -1;
}


/*
 * Grab one flag in an feature_type from a textual string
 */
static errr grab_one_feat_flag(feature_type *f_ptr, cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < FF_FLAG_MAX; i++)
	{
		if (streq(what, f_info_flags[i]))
		{
			add_flag(f_ptr->flags, i);
			return 0;
		}
	}

	/* Oops */
#ifdef JP
	msg_format("���m�̒n�`�t���O '%s'�B", what);
#else
	msg_format("Unknown feature flag '%s'.", what);
#endif

	/* Error */
	return PARSE_ERROR_GENERIC;
}


/*
 * Grab an action in an feature_type from a textual string
 */
static errr grab_one_feat_action(feature_type *f_ptr, cptr what, int count)
{
	int i;

	/* Check flags */
	for (i = 0; i < FF_FLAG_MAX; i++)
	{
		if (streq(what, f_info_flags[i]))
		{
			f_ptr->state[count].action = i;
			return 0;
		}
	}

	/* Oops */
#ifdef JP
	msg_format("���m�̒n�`�A�N�V���� '%s'�B", what);
#else
	msg_format("Unknown feature action '%s'.", what);
#endif

	/* Error */
	return PARSE_ERROR_GENERIC;
}


/*
 * Initialize the "f_info" array, by parsing an ascii "template" file
 */
/*:::f_info.txt�̊e�s�̏����@�ڍז���*/
errr parse_f_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static feature_type *f_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		if (s)
		{
			/* Nuke the colon, advance to the name */
			*s++ = '\0';
		}

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		f_ptr = &f_info[i];

		/* Tag name is given */
		if (s)
		{
			/* Store the tag */
			if (!add_tag(&f_ptr->tag, head, s)) return (7);
		}

		/* Default "mimic" */
		f_ptr->mimic = i;

		/* Default "destroyed state" -- if not specified */
		f_ptr->destroyed = i;

		/* Default "states" */
		for (i = 0; i < MAX_FEAT_STATES; i++) f_ptr->state[i].action = FF_FLAG_MAX;
	}

	/* There better be a current f_ptr */
	else if (!f_ptr) return (3);

#ifdef JP
	else if (buf[0] == 'J')
	{
		/* Store the name */
		if (!add_name(&f_ptr->name, head, buf+2)) return (7);
	}

	else if (buf[0] == 'E')
	{
		/* Ignore english name */
	}
#else
	else if (buf[0] == 'J')
	{
		/* Ignore Japanese name */
	}

	else if (buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if (!add_name(&f_ptr->name, head, s)) return (7);
	}
#endif


	/* Process 'M' for "Mimic" (one line only) */
	else if (buf[0] == 'M')
	{
		s16b offset;

		if (!add_tag(&offset, head, buf + 2)) return PARSE_ERROR_OUT_OF_MEMORY;

		/* Record a fake tag index */
		f_ptr->mimic = -offset;
	}


	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		int j;
		byte s_attr;
		char char_tmp[F_LIT_MAX];

		/* Paranoia */
		if (buf[1] != ':') return (1);
		if (!buf[2]) return (1);
		if (buf[3] != ':') return (1);
		if (!buf[4]) return (1);

		/* Extract the char */
		char_tmp[F_LIT_STANDARD] = buf[2];

		/* Extract the color */
		s_attr = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (s_attr > 127) return (1);

		/* Save the standard values */
		f_ptr->d_attr[F_LIT_STANDARD] = s_attr;
		f_ptr->d_char[F_LIT_STANDARD] = char_tmp[F_LIT_STANDARD];

		/* Is this feature supports lighting? */
		if (buf[5] == ':')
		{
			/* G:c:a:LIT (default) */
			apply_default_feat_lighting(f_ptr->d_attr, f_ptr->d_char);

			/* G:c:a:lc:la:dc:da */
			if (!streq(buf + 6, "LIT"))
			{
				char attr_lite_tmp[F_LIT_MAX - F_LIT_NS_BEGIN];

				if ((F_LIT_MAX - F_LIT_NS_BEGIN) * 2 != sscanf(buf + 6, "%c:%c:%c:%c",
					&char_tmp[F_LIT_LITE], &attr_lite_tmp[F_LIT_LITE - F_LIT_NS_BEGIN],
					&char_tmp[F_LIT_DARK], &attr_lite_tmp[F_LIT_DARK - F_LIT_NS_BEGIN])) return 1;
				if (buf[F_LIT_MAX * 4 + 1]) return 1;

				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					switch (attr_lite_tmp[j - F_LIT_NS_BEGIN])
					{
					case '*':
						/* Use default lighting */
						break;
					case '-':
						/* No lighting support */
						f_ptr->d_attr[j] = f_ptr->d_attr[F_LIT_STANDARD];
						break;
					default:
						/* Extract the color */
						f_ptr->d_attr[j] = color_char_to_attr(attr_lite_tmp[j - F_LIT_NS_BEGIN]);
						if (f_ptr->d_attr[j] > 127) return 1;
						break;
					}
					f_ptr->d_char[j] = char_tmp[j];
				}
			}
		}
		else if (!buf[5])
		{
			for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
			{
				f_ptr->d_attr[j] = s_attr;
				f_ptr->d_char[j] = char_tmp[F_LIT_STANDARD];
			}
		}
		else return 1;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* XXX XXX XXX Hack -- Read feature subtype */
			if (1 == sscanf(s, "SUBTYPE_%d", &i))
			{
				/* Extract a "subtype" */
				f_ptr->subtype =  i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* XXX XXX XXX Hack -- Read feature power */
			if (1 == sscanf(s, "POWER_%d", &i))
			{
				/* Extract a "power" */
				f_ptr->power =  i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if (0 != grab_one_feat_flag(f_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int priority;

		/* Scan for the value */
		if (1 != sscanf(buf+2, "%d", &priority)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		f_ptr->priority = priority;
	}

	/* Process 'K' for "States" (up to four lines + default (which cannot be last)) */
	else if (buf[0] == 'K')
	{
		s16b offset;

		/* Find the next empty state slot (if any) */
		for (i = 0; i < MAX_FEAT_STATES; i++) if (f_ptr->state[i].action == FF_FLAG_MAX) break;

		/* Oops, no more slots */
		if (i == MAX_FEAT_STATES) return PARSE_ERROR_GENERIC;

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Is this default entry? */
		if (streq(s, "DESTROYED"))
		{
			if (!add_tag(&offset, head, t)) return PARSE_ERROR_OUT_OF_MEMORY;

			/* Record a fake tag index */
			f_ptr->destroyed = -offset;
		}
		else
		{
			/* Reset */
			f_ptr->state[i].action = 0;

			/* Parse this entry */
			if (0 != grab_one_feat_action(f_ptr, s, i)) return PARSE_ERROR_INVALID_FLAG;

			if (!add_tag(&offset, head, t)) return PARSE_ERROR_OUT_OF_MEMORY;

			/* Record a fake tag index */
			f_ptr->state[i].result = -offset;
		}
	}

	/* Oops */
	else	return (6);

	/* Success */
	return (0);
}


/*
 * Convert a fake tag to a real feat index
 */
/*:::�悭����Ȃ���f_info.txt�̃^�O����ID�ɕϊ����Ă�炵��*/
s16b f_tag_to_index(cptr str)
{
	u16b i;

	/* Search for real index corresponding to this fake tag */
	for (i = 0; i < f_head.info_num; i++)
	{
		if (streq(f_tag + f_info[i].tag, str))
		{
			/* Return the index */
			return (s16b)i;
		}
	}

	/* Not found */
	return -1;
}


/*
 * Search for real index corresponding to this fake tag
 */
static void search_real_feat(s16b *feat)
{
	int i;

	/* Don't convert non-fake tag */
	if (*feat >= 0) return;

	/* Search for real index corresponding to this fake tag */
	for (i = 0; i < f_head.info_num; i++)
	{
		if ((-(*feat)) == f_info[i].tag)
		{
			/* Record real index */
			*feat = (s16b)i;
			return;
		}
	}

	/* Undefined tag */
#ifdef JP
	msg_format("����`�̃^�O '%s'�B", f_tag + (-(*feat)));
#else
	msg_format("%s is undefined.", f_tag + (-(*feat)));
#endif
}


/*
 * Retouch fake tags of f_info
 */
void retouch_f_info(header *head)
{
	int i;

	/* Convert fake tags to real feat indices */
	for (i = 0; i < head->info_num; i++)
	{
		feature_type *f_ptr = &f_info[i];
		int j;

		search_real_feat(&f_ptr->mimic);

		search_real_feat(&f_ptr->destroyed);

		for (j = 0; j < MAX_FEAT_STATES; j++) search_real_feat(&f_ptr->state[j].result);
	}
}


/*
 * Grab one flag in an object_kind from a textual string
 */
/*:::���͂���������ɍ��v����A�C�e���̃t���O�𓾂�Bk_info.txt�̉�͎��ɂ̂݌Ă΂��*/
static errr grab_one_kind_flag(object_kind *k_ptr, cptr what)
{
	int i;

	/* Check flags */
	/*:::�܂�k_info_flags[]����T��*/
	for (i = 0; i < TR_FLAG_MAX; i++)
	{
		if (streq(what, k_info_flags[i]))
		{
			add_flag(k_ptr->flags, i);
			return (0);
		}
	}
	/*:::�������k_info_gen_flags[]����T��*/
	if (grab_one_flag(&k_ptr->gen_flags, k_info_gen_flags, what) == 0)
		return 0;

	/* Oops */
#ifdef JP
	msg_format("���m�̃A�C�e���E�t���O '%s'�B", what);
#else
	msg_format("Unknown object flag '%s'.", what);
#endif


	/* Error */
	return (1);
}

/*
 * Grab one activation index flag
 */
/*:::�����t���O�𓾂�Bk_info/a_info/e_info�\����������Ă΂��*/
static byte grab_one_activation_flag(cptr what)
{
	int i;

	for (i = 0; ; i++)
	{
		if (activation_info[i].flag == NULL) break;

		if (streq(what, activation_info[i].flag))
		{
			return activation_info[i].index;
		}
	}

	i = atoi(what);
	 if (i > 0)
	 {
		 return ((byte) i);
	 }

	/* Oops */
#ifdef JP
	msg_format("���m�̔����E�t���O '%s'�B", what);
#else
	msg_format("Unknown activation flag '%s'.", what);
#endif

	/* Error */
	return (0);
}


/*
 * Initialize the "k_info" array, by parsing an ascii "template" file
 */
/*:::k_info.txt�̊e�s����*/
errr parse_k_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static object_kind *k_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	/*:::N�s�@k_info[ID]�̃|�C���^�𓾂Ė��O�𓾂�*/
	if (buf[0] == 'N')
	{
#ifdef JP
		char *flavor;
#endif

		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		/*:::ID�͏�ɏ����ɏ��������炵��*/
		if (i <= error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		k_ptr = &k_info[i];

#ifdef JP
		/* Paranoia -- require a name */
		if (!*s) return (1);

		/* Find the colon before the flavor */
		flavor = my_strchr(s, ':');

		/* Verify that colon */
		/*:::�s�m�薼������ꍇ���������*/
		if (flavor)
		{
			/* Nuke the colon, advance to the flavor */
			*flavor++ = '\0';

			/* Store the flavor */
			if (!add_name(&k_ptr->flavor_name, head, flavor)) return (7);
		}

		/* Store the name */
		if (!add_name(&k_ptr->name, head, s)) return (7);
#endif
	}

	/* There better be a current k_ptr */
	else if (!k_ptr) return (3);


#ifdef JP
	/* �p�ꖼ��ǂރ��[�`����ǉ� */
	/* 'E' ����n�܂�s�͉p�ꖼ�Ƃ��Ă��� */
	/*:::�p�ꖼ�����@r_info�ƈ���ĉ������ĂȂ�*/
	else if (buf[0] == 'E')
	{
		/* nothing to do */
	}
#else
	else if (buf[0] == 'E')
	{
		char *flavor;

		/* Acquire the name */
		s = buf+2;

		/* Find the colon before the flavor */
		flavor = my_strchr(s, ':');

		/* Verify that colon */
		if (flavor)
		{
			/* Nuke the colon, advance to the flavor */
			*flavor++ = '\0';

			/* Store the flavor */
			if (!add_name(&k_ptr->flavor_name, head, flavor)) return (7);
		}

		/* Store the name */
		if (!add_name(&k_ptr->name, head, s)) return (7);
	}
#endif

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
#ifdef JP
		if (buf[2] == '$')
			return (0);
		/* Acquire the text */
		s = buf+2;
#else
		if (buf[2] != '$')
			return (0);
		/* Acquire the text */
		s = buf+3;
#endif

		/* Store the text */
		if (!add_text(&k_ptr->text, head, s, TRUE)) return (7);
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		char sym;
		byte tmp;

		/* Paranoia */
		if (buf[1] != ':') return (1);
		if (!buf[2]) return (1);
		if (buf[3] != ':') return (1);
		if (!buf[4]) return (1);

		/* Extract the char */
		sym = buf[2];

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp > 127) return (1);

		/* Save the values */
		k_ptr->d_attr = tmp;
		k_ptr->d_char = sym;
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int tval, sval, pval;

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
				&tval, &sval, &pval)) return (1);

		/* Save the values */
		k_ptr->tval = tval;
		k_ptr->sval = sval;
		k_ptr->pval = pval;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, extra, wgt;
		long cost;

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
				&level, &extra, &wgt, &cost)) return (1);

		/* Save the values */
		k_ptr->level = level;
		k_ptr->extra = extra;
		k_ptr->weight = wgt;
		k_ptr->cost = cost;
	}

	/* Process 'A' for "Allocation" (one line only) */
	/*:::�������x���A���A���e�B�ω��l�E�E�H*/
	else if (buf[0] == 'A')
	{

		/* XXX XXX XXX Simply read each number following a colon */
		for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i)
		{
				/* Default chance */
			k_ptr->chance[i] = 1;

				/* Store the attack damage index */
			k_ptr->locale[i] = atoi(s+1);

				/* Find the slash */
			t = my_strchr(s+1, '/');

				/* Find the next colon */
			s = my_strchr(s+1, ':');

				/* If the slash is "nearby", use it */
			if (t && (!s || t < s))
			{
				int chance = atoi(t+1);
				if (chance > 0) k_ptr->chance[i] = chance;
			}
		}
	}

	/* Hack -- Process 'P' for "power" and such */
	else if (buf[0] == 'P')
	{
		int ac, hd1, hd2, th, td, ta;

		/* Scan for the values */
		if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
				&ac, &hd1, &hd2, &th, &td, &ta)) return (1);

		k_ptr->ac = ac;
		k_ptr->dd = hd1;
		k_ptr->ds = hd2;
		k_ptr->to_h = th;
		k_ptr->to_d = td;
		k_ptr->to_a =  ta;
	}

	/* Hack -- Process 'U' for activation index */
	else if (buf[0] == 'U')
	{
		byte n;
		n = grab_one_activation_flag(buf + 2);
		if (n > 0)
		{
			k_ptr->act_idx = n;
		}
		else
		{
			return (5);
		}
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
				/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

				/* Parse this entry */
			if (0 != grab_one_kind_flag(k_ptr, s)) return (5);

				/* Start the next entry */
			s = t;
		}
	}


	/* Oops */
	else return (6);


	/* Success */
	return (0);
}


/*
 * Grab one flag in an artifact_type from a textual string
 */
/*:::�A�[�e�B�t�@�N�g�̃t���O�𓾂�@a_info.txt���߂���̂݌Ă΂��*/
/*:::�����grab_one_kind_flag()�ƑS������*/
static errr grab_one_artifact_flag(artifact_type *a_ptr, cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < TR_FLAG_MAX; i++)
	{
		if (streq(what, k_info_flags[i]))
		{
			add_flag(a_ptr->flags, i);
			return (0);
		}
	}

	if (grab_one_flag(&a_ptr->gen_flags, k_info_gen_flags, what) == 0)
		return 0;

	/* Oops */
#ifdef JP
	msg_format("���m�̓`���̃A�C�e���E�t���O '%s'�B", what);
#else
	msg_format("Unknown artifact flag '%s'.", what);
#endif


	/* Error */
	return (1);
}


/*
 * Initialize the "a_info" array, by parsing an ascii "template" file
 */
/*:::a_info.txt�̊e�s����*/
errr parse_a_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static artifact_type *a_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

			/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';
#ifdef JP
		/* Paranoia -- require a name */
		if (!*s) return (1);
#endif
		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i < error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		a_ptr = &a_info[i];

		/* Ignore everything */
		add_flag(a_ptr->flags, TR_IGNORE_ACID);
		add_flag(a_ptr->flags, TR_IGNORE_ELEC);
		add_flag(a_ptr->flags, TR_IGNORE_FIRE);
		add_flag(a_ptr->flags, TR_IGNORE_COLD);
#ifdef JP
		/* Store the name */
		if (!add_name(&a_ptr->name, head, s)) return (7);
#endif
	}

	/* There better be a current a_ptr */
	else if (!a_ptr) return (3);


#ifdef JP
	/* �p�ꖼ��ǂރ��[�`����ǉ� */
	/* 'E' ����n�܂�s�͉p�ꖼ�Ƃ��Ă��� */
	else if (buf[0] == 'E')
	{
		/* nothing to do */
	}
#else
	else if (buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if (!add_name(&a_ptr->name, head, s)) return (7);
	}
#endif

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
#ifdef JP
		if (buf[2] == '$')
			return (0);
		/* Acquire the text */
		s = buf+2;
#else
		if (buf[2] != '$')
			return (0);
		/* Acquire the text */
		s = buf+3;
#endif

		/* Store the text */
		if (!add_text(&a_ptr->text, head, s, TRUE)) return (7);
	}


	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int tval, sval, pval;

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
				&tval, &sval, &pval)) return (1);

		/* Save the values */
		a_ptr->tval = tval;
		a_ptr->sval = sval;
		a_ptr->pval = pval;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, rarity, wgt;
		long cost;

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
				&level, &rarity, &wgt, &cost)) return (1);

		/* Save the values */
		a_ptr->level = level;
		a_ptr->rarity = rarity;
		a_ptr->weight = wgt;
		a_ptr->cost = cost;
	}

	/* Hack -- Process 'P' for "power" and such */
	else if (buf[0] == 'P')
	{
		int ac, hd1, hd2, th, td, ta;

		/* Scan for the values */
		if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
				&ac, &hd1, &hd2, &th, &td, &ta)) return (1);

		a_ptr->ac = ac;
		a_ptr->dd = hd1;
		a_ptr->ds = hd2;
		a_ptr->to_h = th;
		a_ptr->to_d = td;
		a_ptr->to_a =  ta;
	}

	/* Hack -- Process 'U' for activation index */
	else if (buf[0] == 'U')
	{
		byte n;
		n = grab_one_activation_flag(buf + 2);
		if (n > 0)
		{
			a_ptr->act_idx = n;
		}
		else
		{
			return (5);
		}
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
				/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

				/* Parse this entry */
			if (0 != grab_one_artifact_flag(a_ptr, s)) return (5);

				/* Start the next entry */
			s = t;
		}
	}


	/* Oops */
	else return (6);


	/* Success */
	return (0);
}


/*
 * Grab one flag in a ego-item_type from a textual string
 */
static bool grab_one_ego_item_flag(ego_item_type *e_ptr, cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < TR_FLAG_MAX; i++)
	{
		if (streq(what, k_info_flags[i]))
		{
			add_flag(e_ptr->flags, i);
			return (0);
		}
	}

	if (grab_one_flag(&e_ptr->gen_flags, k_info_gen_flags, what) == 0)
		return 0;

	/* Oops */
#ifdef JP
	msg_format("���m�̖��̂���A�C�e���E�t���O '%s'�B", what);
#else
	msg_format("Unknown ego-item flag '%s'.", what);
#endif


	/* Error */
	return (1);
}




/*
 * Initialize the "e_info" array, by parsing an ascii "template" file
 */
/*:::e_info.txt�̍s���Ɖ���*/
errr parse_e_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static ego_item_type *e_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

			/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';
#ifdef JP
		/* Paranoia -- require a name */
		if (!*s) return (1);
#endif
		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i < error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		e_ptr = &e_info[i];
#ifdef JP
		/* Store the name */
		if (!add_name(&e_ptr->name, head, s)) return (7);
#endif
	}

	/* There better be a current e_ptr */
	else if (!e_ptr) return (3);


#ifdef JP
	/* �p�ꖼ��ǂރ��[�`����ǉ� */
	/* 'E' ����n�܂�s�͉p�ꖼ */
	else if (buf[0] == 'E')
	{
		/* nothing to do */
	}
#else
	else if (buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if (!add_name(&e_ptr->name, head, s)) return (7);
	}
#endif
#if 0

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* Acquire the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&e_ptr->text, head, s, TRUE)) return (7);
	}

#endif

	/* Process 'X' for "Xtra" (one line only) */
	else if (buf[0] == 'X')
	{
		int slot, rating;

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
				&slot, &rating)) return (1);

		/* Save the values */
		e_ptr->slot = slot;
		e_ptr->rating = rating;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, rarity, pad2;
		long cost;

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
				&level, &rarity, &pad2, &cost)) return (1);

		/* Save the values */
		e_ptr->level = level;
		e_ptr->rarity = rarity;
		/* e_ptr->weight = wgt; */
		e_ptr->cost = cost;
	}

	/* Hack -- Process 'C' for "creation" */
	else if (buf[0] == 'C')
	{
		int th, td, ta, pv;

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
				&th, &td, &ta, &pv)) return (1);

		e_ptr->max_to_h = th;
		e_ptr->max_to_d = td;
		e_ptr->max_to_a = ta;
		e_ptr->max_pval = pv;
	}

	/* Hack -- Process 'U' for activation index */
	else if (buf[0] == 'U')
	{
		byte n;
		n = grab_one_activation_flag(buf + 2);
		if (n > 0)
		{
			e_ptr->act_idx = n;
		}
		else
		{
			return (5);
		}
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
				/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

				/* Parse this entry */
			if (0 != grab_one_ego_item_flag(e_ptr, s)) return (5);

				/* Start the next entry */
			s = t;
		}
	}

	/* Oops */
	else return (6);

	/* Success */
	return (0);
}


/*
 * Grab one (basic) flag in a monster_race from a textual string
 */
/*:::�����X�^�[�t���Oflags1-3,7-9,flagsr�𓾂�*/
static errr grab_one_basic_flag(monster_race *r_ptr, cptr what)
{
	if (grab_one_flag(&r_ptr->flags1, r_info_flags1, what) == 0)
		return 0;

	if (grab_one_flag(&r_ptr->flags2, r_info_flags2, what) == 0)
		return 0;

	if (grab_one_flag(&r_ptr->flags3, r_info_flags3, what) == 0)
		return 0;

	if (grab_one_flag(&r_ptr->flags7, r_info_flags7, what) == 0)
		return 0;

	if (grab_one_flag(&r_ptr->flags8, r_info_flags8, what) == 0)
		return 0;

///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ����ړ�
//	if (grab_one_flag(&r_ptr->flags9, r_info_flags9, what) == 0)
//		return 0;

	if (grab_one_flag(&r_ptr->flagsr, r_info_flagsr, what) == 0)
		return 0;

	/* Oops */
#ifdef JP
	msg_format("���m�̃����X�^�[�E�t���O '%s'�B", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif


	/* Failure */
	return (1);
}


/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
/*:::�����X�^�[�t���Oflags4-6�𓾂�*/
///system �����X�^�[�X�y���𑝂₷�Ȃ炱����G��Ȃ��Ƃ����Ȃ�
static errr grab_one_spell_flag(monster_race *r_ptr, cptr what)
{
	if (grab_one_flag(&r_ptr->flags4, r_info_flags4, what) == 0)
		return 0;

	if (grab_one_flag(&r_ptr->flags5, r_info_flags5, what) == 0)
		return 0;

	if (grab_one_flag(&r_ptr->flags6, r_info_flags6, what) == 0)
		return 0;

	///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ���flags9�ǉ�
	if (grab_one_flag(&r_ptr->flags9, r_info_flags9, what) == 0)
		return 0;

	/* Oops */
#ifdef JP
	msg_format("���m�̃����X�^�[�E�t���O '%s'�B", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif


	/* Failure */
	return (1);
}




/*
 * Initialize the "r_info" array, by parsing an ascii "template" file
 */
/*:::r_info.txt�̈�s�̉��߂�����*/
errr parse_r_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	/*:::��x�֐����I����Ă�r_ptr�̈ʒu�͕ێ�����Ă���*/
	static monster_race *r_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	/*:::ID�ƃ����X�^�[���𒊏o���Ar_ptr��ID�̈ʒu�ɍ��킹�A�����X�^�[�����L�^����*/
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

			/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';
#ifdef JP
		/* Paranoia -- require a name */
		if (!*s) return (1);
#endif
		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i < error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		r_ptr = &r_info[i];
#ifdef JP
		/* Store the name */
		if (!add_name(&r_ptr->name, head, s))
		{
			return (7);
		}
#endif
	}

	/* There better be a current r_ptr */
	else if (!r_ptr) return (3);


#ifdef JP
	/* �p�ꖼ��ǂރ��[�`����ǉ� */
	/* 'E' ����n�܂�s�͉p�ꖼ */
	/*:::�p�ꖼ�@���{�ꃂ�[�h�ł�����Ƃ����ق��������炵��*/
	else if (buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if (!add_name(&r_ptr->E_name, head, s)) return (7);
	}
#else
	else if (buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if (!add_name(&r_ptr->name, head, s)) return (7);
	}
#endif
	/* Process 'D' for "Description" */
	/*:::�����X�^�[������r_ptr->text�Ɋi�[����*/
	else if (buf[0] == 'D')
	{
#ifdef JP
		if (buf[2] == '$')
			return (0);
		/* Acquire the text */
		s = buf+2;
#else
		if (buf[2] != '$')
			return (0);
		/* Acquire the text */
		s = buf+3;
#endif

		/* Store the text */
		if (!add_text(&r_ptr->text, head, s, TRUE)) return (7);
	}

	/* Process 'G' for "Graphics" (one line only) */
	/*:::�����ƐF��d_char��d_attr�Ɋi�[�@�F�͕������琔���ɕϊ������*/
	else if (buf[0] == 'G')
	{
		char sym;
		byte tmp;

		/* Paranoia */
		if (buf[1] != ':') return (1);
		if (!buf[2]) return (1);
		if (buf[3] != ':') return (1);
		if (!buf[4]) return (1);

		/* Extract the char */
		sym = buf[2];

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp > 127) return (1);

		/* Save the values */
		r_ptr->d_char = sym;
		r_ptr->d_attr = tmp;
	}

	/* Process 'I' for "Info" (one line only) */
	/*:::�����i�\�L+110) (speed)�A�_�C�X��(hdice)�A�_�C�X��(hside)�A���m�͈�(aaf)�AAC(ac)�A�x���x(sleep)�����ꂼ��i�[*/
	else if (buf[0] == 'I')
	{
		int spd, hp1, hp2, aaf, ac, slp;

		/* Scan for the other values */
		if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
				&spd, &hp1, &hp2, &aaf, &ac, &slp)) return (1);

		/* Save the values */
		r_ptr->speed = spd;
		r_ptr->hdice = MAX(hp1, 1);
		r_ptr->hside = MAX(hp2, 1);
		r_ptr->aaf = aaf;
		r_ptr->ac = ac;
		r_ptr->sleep = slp;
	}

	/* Process 'W' for "More Info" (one line only) */
	/*���x���A���A���e�B�A�o���l�A�i���܂ł̌o���l�A�i����ID���i�[*/
	else if (buf[0] == 'W')
	{
		int lev, rar, pad;
		long exp;
		long nextexp;
		int nextmon;

		/* Scan for the values */
		if (6 != sscanf(buf+2, "%d:%d:%d:%ld:%ld:%d",
				&lev, &rar, &pad, &exp, &nextexp, &nextmon)) return (1);

		/* Save the values */
		r_ptr->level = lev;
		r_ptr->rarity = rar;
		r_ptr->extra = pad;
		r_ptr->mexp = exp;
		r_ptr->next_exp = nextexp;
		r_ptr->next_r_idx = nextmon;
	}

	/* Process 'B' for "Blows" (up to four lines) */
	/*:::�ʏ�U����4�܂�blow[]�Ɋi�[*/
	else if (buf[0] == 'B')
	{
		int n1, n2;

		/* Find the next empty blow slot (if any) */
		for (i = 0; i < 4; i++) if (!r_ptr->blow[i].method) break;

		/* Oops, no more slots */
		if (i == 4) return (1);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze the method */
		for (n1 = 0; r_info_blow_method[n1]; n1++)
		{
			if (streq(s, r_info_blow_method[n1])) break;
		}

		/* Invalid method */
		if (!r_info_blow_method[n1]) return (1);

		/* Analyze the second field */
		for (s = t; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze effect */
		for (n2 = 0; r_info_blow_effect[n2]; n2++)
		{
			if (streq(s, r_info_blow_effect[n2])) break;
		}

		/* Invalid effect */
		if (!r_info_blow_effect[n2]) return (1);

		/* Analyze the third field */
		for (s = t; *t && (*t != 'd'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Save the method */
		r_ptr->blow[i].method = n1;

		/* Save the effect */
		r_ptr->blow[i].effect = n2;

		/* Extract the damage dice and sides */
		r_ptr->blow[i].d_dice = atoi(s);
		r_ptr->blow[i].d_side = atoi(t);
	}

	/* Process 'F' for "Basic Flags" (multiple lines) */
	/*:::F�s�̃t���O�����߂�flags1-9,flagsr�Ɋi�[*/
	else if (buf[0] == 'F')
	{
		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
				/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

				/* Parse this entry */
			if (0 != grab_one_basic_flag(r_ptr, s)) return (5);

				/* Start the next entry */
			s = t;
		}
	}

	/* Process 'S' for "Spell Flags" (multiple lines) */
	/*:::�����X�^�[�X�y���̃t���O������*/
	else if (buf[0] == 'S')
	{
		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
				/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

				/* XXX XXX XXX Hack -- Read spell frequency */
			if (1 == sscanf(s, "1_IN_%d", &i))
			{
				/* Extract a "frequency" */
				r_ptr->freq_spell = 100 / i;

					/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

				/* Parse this entry */
			if (0 != grab_one_spell_flag(r_ptr, s)) return (5);

				/* Start the next entry */
			s = t;
		}
	}

	/* Oops */
	else return (6);


	/* Success */
	return (0);
}


/*
 * Grab one flag for a dungeon type from a textual string
 */
static errr grab_one_dungeon_flag(dungeon_info_type *d_ptr, cptr what)
{
	if (grab_one_flag(&d_ptr->flags1, d_info_flags1, what) == 0)
		return 0;

	/* Oops */
#ifdef JP
	msg_format("���m�̃_���W�����E�t���O '%s'�B", what);
#else
	msg_format("Unknown dungeon type flag '%s'.", what);
#endif

	/* Failure */
	return (1);
}

/*
 * Grab one (basic) flag in a monster_race from a textual string
 */
static errr grab_one_basic_monster_flag(dungeon_info_type *d_ptr, cptr what)
{
	if (grab_one_flag(&d_ptr->mflags1, r_info_flags1, what) == 0)
		return 0;

	if (grab_one_flag(&d_ptr->mflags2, r_info_flags2, what) == 0)
		return 0;

	if (grab_one_flag(&d_ptr->mflags3, r_info_flags3, what) == 0)
		return 0;

	if (grab_one_flag(&d_ptr->mflags7, r_info_flags7, what) == 0)
		return 0;

	if (grab_one_flag(&d_ptr->mflags8, r_info_flags8, what) == 0)
		return 0;

///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ����ړ�
	//if (grab_one_flag(&d_ptr->mflags9, r_info_flags9, what) == 0)
	//	return 0;

	if (grab_one_flag(&d_ptr->mflagsr, r_info_flagsr, what) == 0)
		return 0;

	/* Oops */
#ifdef JP
	msg_format("���m�̃����X�^�[�E�t���O '%s'�B", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif
	/* Failure */
	return (1);
}


/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
static errr grab_one_spell_monster_flag(dungeon_info_type *d_ptr, cptr what)
{
	if (grab_one_flag(&d_ptr->mflags4, r_info_flags4, what) == 0)
		return 0;

	if (grab_one_flag(&d_ptr->mflags5, r_info_flags5, what) == 0)
		return 0;

	if (grab_one_flag(&d_ptr->mflags6, r_info_flags6, what) == 0)
		return 0;
///mod140102 �����X�^�[�V���@�t���O�ǉ��ɔ���flags9�ǉ�
	if (grab_one_flag(&d_ptr->mflags9, r_info_flags9, what) == 0)
		return 0;


	/* Oops */
#ifdef JP
	msg_format("���m�̃����X�^�[�E�t���O '%s'�B", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif

	/* Failure */
	return (1);
}

/*
 * Initialize the "d_info" array, by parsing an ascii "template" file
 */
/*:::d_info.txt�̍s���*/
errr parse_d_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static dungeon_info_type *d_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (1);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';
#ifdef JP
		/* Paranoia -- require a name */
		if (!*s) return (1);
#endif
		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i < error_idx) return (4);

		/* Verify information */
		if (i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		d_ptr = &d_info[i];
#ifdef JP
		/* Store the name */
		if (!add_name(&d_ptr->name, head, s)) return (7);
#endif
	}

#ifdef JP
	else if (buf[0] == 'E') return (0);
#else
	else if (buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if (!add_name(&d_ptr->name, head, s)) return (7);
	}
#endif

	/* Process 'D' for "Description */
	else if (buf[0] == 'D')
	{
#ifdef JP
		if (buf[2] == '$')
			return (0);
		/* Acquire the text */
		s = buf+2;
#else
		if (buf[2] != '$')
			return (0);
		/* Acquire the text */
		s = buf+3;
#endif

		/* Store the text */
		if (!add_text(&d_ptr->text, head, s, TRUE)) return (7);
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int min_lev, max_lev;
		int min_plev, mode;
		int min_alloc, max_chance;
		int obj_good, obj_great;
		int pit, nest;

		/* Scan for the values */
		if (10 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%x:%x",
				 &min_lev, &max_lev, &min_plev, &mode, &min_alloc, &max_chance, &obj_good, &obj_great, (unsigned int *)&pit, (unsigned int *)&nest)) return (1);

		/* Save the values */
		d_ptr->mindepth = min_lev;
		d_ptr->maxdepth = max_lev;
		d_ptr->min_plev = min_plev;
		d_ptr->mode = mode;
		d_ptr->min_m_alloc_level = min_alloc;
		d_ptr->max_m_alloc_chance = max_chance;
		d_ptr->obj_good = obj_good;
		d_ptr->obj_great = obj_great;
		d_ptr->pit = pit;
		d_ptr->nest = nest;
	}

	/* Process 'P' for "Place Info" */
	else if (buf[0] == 'P')
	{
		int dy, dx;

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &dy, &dx)) return (1);

		/* Save the values */
		d_ptr->dy = dy;
		d_ptr->dx = dx;
	}

	/* Process 'L' for "fLoor type" (one line only) */
	else if (buf[0] == 'L')
	{
		char *zz[16];

		/* Scan for the values */
		if (tokenize(buf+2, DUNGEON_FEAT_PROB_NUM * 2 + 1, zz, 0) != (DUNGEON_FEAT_PROB_NUM * 2 + 1)) return (1);

		/* Save the values */
		for (i = 0; i < DUNGEON_FEAT_PROB_NUM; i++)
		{
			d_ptr->floor[i].feat = f_tag_to_index(zz[i * 2]);
			if (d_ptr->floor[i].feat < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;

			d_ptr->floor[i].percent = atoi(zz[i * 2 + 1]);
		}
		d_ptr->tunnel_percent = atoi(zz[DUNGEON_FEAT_PROB_NUM * 2]);
	}

	/* Process 'A' for "wAll type" (one line only) */
	else if (buf[0] == 'A')
	{
		char *zz[16];

		/* Scan for the values */
		if (tokenize(buf+2, DUNGEON_FEAT_PROB_NUM * 2 + 4, zz, 0) != (DUNGEON_FEAT_PROB_NUM * 2 + 4)) return (1);

		/* Save the values */
		for (i = 0; i < DUNGEON_FEAT_PROB_NUM; i++)
		{
			d_ptr->fill[i].feat = f_tag_to_index(zz[i * 2]);
			if (d_ptr->fill[i].feat < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;

			d_ptr->fill[i].percent = atoi(zz[i * 2 + 1]);
		}

		d_ptr->outer_wall = f_tag_to_index(zz[DUNGEON_FEAT_PROB_NUM * 2]);
		if (d_ptr->outer_wall < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;

		d_ptr->inner_wall = f_tag_to_index(zz[DUNGEON_FEAT_PROB_NUM * 2 + 1]);
		if (d_ptr->inner_wall < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;

		d_ptr->stream1 = f_tag_to_index(zz[DUNGEON_FEAT_PROB_NUM * 2 + 2]);
		if (d_ptr->stream1 < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;

		d_ptr->stream2 = f_tag_to_index(zz[DUNGEON_FEAT_PROB_NUM * 2 + 3]);
		if (d_ptr->stream2 < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
	}

	/* Process 'F' for "Dungeon Flags" (multiple lines) */
	else if (buf[0] == 'F')
	{
		int artif = 0, monst = 0;

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* XXX XXX XXX Hack -- Read Final Artifact */
			if (1 == sscanf(s, "FINAL_ARTIFACT_%d", &artif))
			{
				/* Extract a "Final Artifact" */
				d_ptr->final_artifact = artif;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* XXX XXX XXX Hack -- Read Final Object */
			if (1 == sscanf(s, "FINAL_OBJECT_%d", &artif))
			{
				/* Extract a "Final Artifact" */
				d_ptr->final_object = artif;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* XXX XXX XXX Hack -- Read Artifact Guardian */
			if (1 == sscanf(s, "FINAL_GUARDIAN_%d", &monst))
			{
				/* Extract a "Artifact Guardian" */
				d_ptr->final_guardian = monst;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* XXX XXX XXX Hack -- Read Special Percentage */
			if (1 == sscanf(s, "MONSTER_DIV_%d", &monst))
			{
				/* Extract a "Special %" */
				d_ptr->special_div = monst;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if (0 != grab_one_dungeon_flag(d_ptr, s)) return (5);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'M' for "Basic Flags" (multiple lines) */
	else if (buf[0] == 'M')
	{
		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Hack -- Read monster symbols */
			if (!strncmp(s, "R_CHAR_", 7))
			{
				/* Skip "R_CHAR_" */
				s += 7;

				/* Read a string */
				strncpy(d_ptr->r_char, s, sizeof(d_ptr->r_char));

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if (0 != grab_one_basic_monster_flag(d_ptr, s)) return (5);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'S' for "Spell Flags" (multiple lines) */
	else if (buf[0] == 'S')
	{
		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
				/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

				/* XXX XXX XXX Hack -- Read spell frequency */
			if (1 == sscanf(s, "1_IN_%d", &i))
			{
				/* Start at next entry */
				s = t;

					/* Continue */
				continue;
			}

				/* Parse this entry */
			if (0 != grab_one_spell_monster_flag(d_ptr, s)) return (5);

				/* Start the next entry */
			s = t;
		}
	}

	/* Oops */
	else return (6);

	/* Success */
	return (0);
}


#else	/* ALLOW_TEMPLATES */

#ifdef MACINTOSH
static int i = 0;
#endif

#endif	/* ALLOW_TEMPLATES */


/* Random dungeon grid effects */
#define RANDOM_NONE         0x00000000
#define RANDOM_FEATURE      0x00000001
#define RANDOM_MONSTER      0x00000002
#define RANDOM_OBJECT       0x00000004
#define RANDOM_EGO          0x00000008
#define RANDOM_ARTIFACT     0x00000010
#define RANDOM_TRAP         0x00000020

///mod140405
#define MON_NO_SLEEP		0x00000040
///mod140827
#define MON_FORCE_FRIENDLY	0x00000080
///v1.1.24
#define MON_FORCE_PET		0x00000100


typedef struct dungeon_grid dungeon_grid;

struct dungeon_grid
{
	int		feature;		/* Terrain feature */
	int		monster;		/* Monster */
	int		object;			/* Object */
	int		ego;			/* Ego-Item */
	int		artifact;		/* Artifact */
	int		trap;			/* Trap */
	int		cave_info;		/* Flags for CAVE_MARK, CAVE_GLOW, CAVE_ICKY, CAVE_ROOM */
	int		special;		/* Reserved for special terrain info */
	int		random;			/* Number of the random effect */
};


static dungeon_grid letter[255];


/*
 * Process "F:<letter>:<terrain>:<cave_info>:<monster>:<object>:<ego>:<artifact>:<trap>:<special>" -- info for dungeon grid
 */
/*:::letter[]�Ɋi�[���ꂽ�f�[�^�����Ƃłǂ��Q�Ƃ����̂��킩��Ȃ��B*/
/*:::�E�E�Ǝv�������A<letter>�̕�����int�ɕϊ������letter[]�̓Y�����ɂȂ�炵���ƋC���t�����B*/
static errr parse_line_feature(char *buf)
{
	int num;
	char *zz[9];

	/*:::�������t���O���u�����������v�Ȃ�I���H*/
	if (init_flags & INIT_ONLY_BUILDINGS) return (0);

	/* Tokenize the line */
	/*:::buf�̕������:��/�ŕ�������zz�ɍő�9�i�[���������ꂽ����߂�l�Ƃ���num�Ɋi�[*/
	if ((num = tokenize(buf+2, 9, zz, 0)) > 1)
	{
		/* Letter to assign */
		int index = zz[0][0];

		/* Reset the info for the letter */
		/*:::static dungeon_grid letter[255]*/
		letter[index].feature = feat_none;
		letter[index].monster = 0;
		letter[index].object = 0;
		letter[index].ego = 0;
		letter[index].artifact = 0;
		letter[index].trap = feat_none;
		letter[index].cave_info = 0;
		letter[index].special = 0;
		letter[index].random = RANDOM_NONE;

		/*:::��������Ȃ��B�Ȃ�switch���������Ɏg���Ă���̂��B���ꂶ��g�[�N���̈�ԍŌサ�����߂��Ă���Ȃ��Ǝv�����B*/
		/*:::�Ǝv���Ă���switch����break���Ȃ�������case�Ɋ֌W�Ȃ����̏����ւ��̂܂ܐi�ނ񂾂����B��������Y��Ă��B*/
		switch (num)
		{
			/* Special */
			case 9:
				letter[index].special = atoi(zz[8]);
				/* Fall through */
			/* Trap */
			case 8:
				if ((zz[7][0] == '*') && !zz[7][1])
				{
					letter[index].random |= RANDOM_TRAP;
				}
				else
				{
					/*:::�n�`���^�O����C���f�b�N�X�ɕϊ����Ă���H*/
					letter[index].trap = f_tag_to_index(zz[7]);
					if (letter[index].trap < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
				}
				/* Fall through */
			/* Artifact */
			case 7:
				if (zz[6][0] == '*')
				{
					letter[index].random |= RANDOM_ARTIFACT;
					if (zz[6][1]) letter[index].artifact = atoi(zz[6] + 1);
				}
				else if (zz[6][0] == '!')
				{
					//Hack - Extra���[�h40�K�̕󕨌Ɂ@�o���ς݂������灙�����u���Ƃ�
					//v1.1.43 ������ƑO�ɓ��ꂽ���O�U�ǉ�
					if(EXTRA_MODE)
					{
						int a_idx_list[27] = {45,59,62,72,74,85,90,97,98,99,105,131,133,151,155,157,158,159,173,176,193,195,211,213,313,314,315};
						letter[index].artifact = a_idx_list[randint0(27)];

					}
					else if (p_ptr->inside_quest)
					{
						letter[index].artifact = quest[p_ptr->inside_quest].k_idx;
					}
				}
				else
				{
					letter[index].artifact = atoi(zz[6]);
				}
				/* Fall through */
			/* Ego-item */
			case 6:
				if (zz[5][0] == '*')
				{
					letter[index].random |= RANDOM_EGO;
					if (zz[5][1]) letter[index].ego = atoi(zz[5] + 1);
				}
				else
				{
					letter[index].ego = atoi(zz[5]);
				}
				/* Fall through */
			/* Object */
			case 5:
				if (zz[4][0] == '*')
				{
					letter[index].random |= RANDOM_OBJECT;
					if (zz[4][1]) letter[index].object = atoi(zz[4] + 1);
				}
				else if (zz[4][0] == '!')
				{
					if(EXTRA_MODE)
					{
						letter[index].random |= RANDOM_OBJECT;
						if (zz[4][1]) letter[index].object = atoi(zz[4] + 1);
					}
					else if (p_ptr->inside_quest)
					{
						//�󕨌ɂŁ����Ȃ������Ƃ��p
						if(quest[p_ptr->inside_quest].type == QUEST_TYPE_FIND_RANDOM_ARTIFACT)
						{
							letter[index].object = quest[p_ptr->inside_quest].k_idx;
						}
						else
						{
							int a_idx = quest[p_ptr->inside_quest].k_idx;
							if (a_idx)
							{
								artifact_type *a_ptr = &a_info[a_idx];
								if (!(a_ptr->gen_flags & TRG_INSTA_ART))
								{
									letter[index].object = lookup_kind(a_ptr->tval, a_ptr->sval);
								}
							}
						}

					}
				}
				else
				{
					letter[index].object = atoi(zz[4]);
				}
				/* Fall through */
			/* Monster */
			case 4:
				if (zz[3][0] == '*')
				{
					letter[index].random |= RANDOM_MONSTER;
					if (zz[3][1]) letter[index].monster = atoi(zz[3] + 1);
				}
				/*:::c???�̏ꍇ�C���f�b�N�X-???(���j�[�N�̃N���[��)���z�u�����*/
				else if (zz[3][0] == 'c')
				{
					if (!zz[3][1]) return PARSE_ERROR_GENERIC;
					letter[index].monster = - atoi(zz[3] + 1);
				}
				///mod140405 hack �����X�^�[�C���f�b�N�X�̓���w��t��������ŏ�����N���Ă���悤�ɂ���
				else if (zz[3][0] == 'w')
				{
					if (!zz[3][1]) return PARSE_ERROR_GENERIC;
					letter[index].random |= MON_NO_SLEEP;
					letter[index].monster = atoi(zz[3] + 1);
				}	
				//v1.1.24 p��������z���Ƃ��Đ���
				else if (zz[3][0] == 'p')
				{
					if (!zz[3][1]) return PARSE_ERROR_GENERIC;
					letter[index].random |= MON_NO_SLEEP | MON_FORCE_PET;
					letter[index].monster = atoi(zz[3] + 1);
				}	
				else if (zz[3][0] == 'f')
				{
					if (!zz[3][1]) return PARSE_ERROR_GENERIC;
					letter[index].random |= (MON_NO_SLEEP | MON_FORCE_FRIENDLY);
					letter[index].monster = atoi(zz[3] + 1);
				}	
				else
				{
					letter[index].monster = atoi(zz[3]);
				}
				/* Fall through */
			/* Cave info */
			case 3:
				letter[index].cave_info = atoi(zz[2]);
				/* Fall through */
			/* Feature */
			case 2:
				if ((zz[1][0] == '*') && !zz[1][1])
				{
					letter[index].random |= RANDOM_FEATURE;
				}
				else
				{
					letter[index].feature = f_tag_to_index(zz[1]);
					if (letter[index].feature < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
				}
				break;
		}

		return (0);
	}

	return (1);
}


/*
 * Process "B:<Index>:<Command>:..." -- Building definition
 */
///system �X��푰�Ƃ������o�[�Ƃ��폜����\��
static errr parse_line_building(char *buf)
{
	int i;
	char *zz[37];
	int index;
	char *s;

#ifdef JP
	if (buf[2] == '$')
		return 0;
	s = buf + 2;
#else
	if (buf[2] != '$')
		return 0;
	s = buf + 3;
#endif
	/* Get the building number */
	index = atoi(s);

	/* Find the colon after the building number */
	/*:::�s�̓r���Ɋ������܂܂��\��������ꍇ���̊֐����g����tokenize����H�E�E�����ł��Ȃ�����*/
	/*:::������̃|�C���^���w�蕶���܂Ői�߂�i�����Ή��j*/
	s = my_strchr(s, ':');

	/* Verify that colon */
	if (!s) return (1);

	/* Nuke the colon, advance to the sub-index */
	*s++ = '\0';

	/* Paranoia -- require a sub-index */
	if (!*s) return (1);

	/* Building definition sub-index */
	switch (s[0])
	{
		/* Building name, owner, race */
		/*:::B:<�Xidx>:N:<�X�̖��O>:<�X��̖��O>:<�X��̎푰>*/
		case 'N':
		{
			if (tokenize(s + 2, 3, zz, 0) == 3)
			{
				/* Name of the building */
				strcpy(building[index].name, zz[0]);

				/* Name of the owner */
				strcpy(building[index].owner_name, zz[1]);

				/* Race of the owner */
				strcpy(building[index].owner_race, zz[2]);

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Action */
		case 'A':
		{
			if (tokenize(s + 2, 8, zz, 0) >= 7)
			{
				/* Index of the action */
				int action_index = atoi(zz[0]);

				/* Name of the action */
				strcpy(building[index].act_names[action_index], zz[1]);

				/* Cost of the action for members */
				building[index].member_costs[action_index] = atoi(zz[2]);

				/* Cost of the action for non-members */
				building[index].other_costs[action_index] = atoi(zz[3]);

				/* Letter assigned to the action */
				building[index].letters[action_index] = zz[4][0];

				/* Action code */
				building[index].actions[action_index] = atoi(zz[5]);

				/* Action restriction */
				building[index].action_restr[action_index] = atoi(zz[6]);

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Classes */
		///class
		case 'C':
		{
			//���������o�[�N���X�폜
			return 0;
			/*
			if (tokenize(s + 2, MAX_CLASS, zz, 0) == MAX_CLASS)
			{
				for (i = 0; i < MAX_CLASS; i++)
				{
					building[index].member_class[i] = atoi(zz[i]);
				}

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
			*/
		}
		///race
		/* Building Races */
		case 'R':
		{
			return 0;
			/*
			if (tokenize(s+2, MAX_RACES, zz, 0) == MAX_RACES)
			{
				for (i = 0; i < MAX_RACES; i++)
				{
					building[index].member_race[i] = atoi(zz[i]);
				}

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
			*/
		}
		///realm
		/* Building Realms */
		case 'M':
		{
			return 0;
			/*
			if (tokenize(s+2, MAX_MAGIC, zz, 0) == MAX_MAGIC)
			{
				for (i = 0; i < MAX_MAGIC; i++)
				{
					building[index].member_realm[i+1] = atoi(zz[i]);
				}

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
			*/
		}

		case 'Z':
		{
			/* Ignore scripts */
			break;
		}

		default:
		{
			return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
		}
	}

	return (0);
}


/*
 * Place the object j_ptr to a grid
 */
/*:::place_object�̌㔼�����ł������悤�Ȃ��Ƃ����Ă���*/
/*:::������ɂ͍ĕ`�揈����A�C�e�����I�[�o�[�������Ȃ��̂��Ⴂ���H*/
///mod140105 ������g���̂�static�ɂ���̂��~�߂� 
void drop_here(object_type *j_ptr, int y, int x)
{
	cave_type *c_ptr = &cave[y][x];
	object_type *o_ptr;

	/* Get new object */
	/*:::o_list�p�C���f�b�N�X�擾*/
	s16b o_idx = o_pop();

	/* Access new object */
	/*:::�A�h���X�n��*/
	o_ptr = &o_list[o_idx];

	/* Structure copy */
	object_copy(o_ptr, j_ptr);


	/* Locate */
	o_ptr->iy = y;
	o_ptr->ix = x;

	/* No monster */
	o_ptr->held_m_idx = 0;

	/* Build a stack */
	o_ptr->next_o_idx = c_ptr->o_idx;

	/* Place the object */
	c_ptr->o_idx = o_idx;
}



//�󕨌ɃN�G�X�g�ŃA�[�e�B�t�@�N�g���Ȃ��Ƃ��̐���hook
static bool vault_create_hook_weapon(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	//�ߐڕ���̂�
	if(k_ptr->tval < TV_KNIFE || k_ptr->tval > TV_POLEARM) return FALSE;

	//����ڂ�����͒u����Ȃ�
	if((k_ptr->tval == TV_KNIFE) && (k_ptr->sval <= SV_WEAPON_DAGGER))	return (FALSE);
	if((k_ptr->tval == TV_SWORD) && (k_ptr->sval <= SV_WEAPON_SHORT_SWORD))return (FALSE);
	if((k_ptr->tval == TV_KATANA) && (k_ptr->sval <= SV_WEAPON_SHORT_KATANA))return (FALSE);
	if((k_ptr->tval == TV_HAMMER) && k_ptr->sval <= SV_WEAPON_MORNING_STAR ) return (FALSE);
	if((k_ptr->tval == TV_STICK) && (k_ptr->sval <= SV_WEAPON_QUARTERSTAFF))return (FALSE);
	if((k_ptr->tval == TV_AXE) && (k_ptr->sval <= SV_WEAPON_HATCHET))return (FALSE);
	if((k_ptr->tval == TV_SPEAR) && (k_ptr->sval == SV_WEAPON_AWL_PIKE))return (FALSE);
	if((k_ptr->tval == TV_HAMMER) && (k_ptr->sval == SV_WEAPON_KINE))	return (FALSE);

	if((k_ptr->tval == TV_KNIFE) && (k_ptr->sval <= SV_WEAPON_HAYABUSA))return (TRUE);
	if((k_ptr->tval == TV_SWORD) && (k_ptr->sval <= SV_WEAPON_BLADE_OF_CHAOS))return (TRUE);
	if((k_ptr->tval == TV_KATANA) && (k_ptr->sval <= SV_WEAPON_ANCIENT_KATANA))return (TRUE);
	if((k_ptr->tval == TV_HAMMER) && (k_ptr->sval <= SV_WEAPON_BALL_AND_CHAIN))return (TRUE);
	if((k_ptr->tval == TV_STICK) && (k_ptr->sval <= SV_WEAPON_IRONSTICK))return (TRUE);
	if((k_ptr->tval == TV_AXE) && (k_ptr->sval <= SV_WEAPON_GREAT_AXE))return (TRUE);
	if((k_ptr->tval == TV_SPEAR) && (k_ptr->sval <= SV_WEAPON_LANCE))return (TRUE);
	if((k_ptr->tval == TV_POLEARM) && (k_ptr->sval <= SV_WEAPON_HALBERD))return (TRUE);



	return (FALSE);
}

/*
 * Parse a sub-file of the "extra info"
 */
/*::: buf:info.txt�̏��������s x/y min/max : ���������}�b�v�̗̈�H x,y: �����ł�xmin��ymin�̒l���i�[����Ă���*/
static errr process_dungeon_file_aux(char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x)
{
	int i;

	char *zz[33];


	/* Skip "empty" lines */
	if (!buf[0]) return (0);

	/* Skip "blank" lines */
/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN
	if (iswspace(buf[0])) return (0);
#else
	if (isspace(buf[0])) return (0);
#endif
	/* Skip comments */
	if (buf[0] == '#') return (0);

	/* Require "?:*" format */
	if (buf[1] != ':') return (1);

	/* Process "%:<fname>" */
	if (buf[0] == '%')
	{
		/* Attempt to Process the given file */
		/*:::"%:�t�@�C����"�̍\���̉��� �w�肳�ꂽ�t�@�C��������ɓǂ݂ɍs���ē�������������*/
		return (process_dungeon_file(buf + 2, ymin, xmin, ymax, xmax));
	}

	/* Process "F:<letter>:<terrain>:<cave_info>:<monster>:<object>:<ego>:<artifact>:<trap>:<special>" -- info for dungeon grid */
	/*:::�t���A�̃O���b�h���Ƃ̒n�`����錾���邽�߂̕��H������letter[255]�̒���<letter>�̕���(int)��Y�����Ƃ��Ă��̒n�`�̏�񂪊i�[�����B*/
	if (buf[0] == 'F')
	{
		return parse_line_feature(buf);
	}

	/* Process "D:<dungeon>" -- info for the cave grids */
	/* D:xxxxxxxxxxxxxxx�̍s�̉���*/
	else if (buf[0] == 'D')
	{
		object_type object_type_body;

		/* Acquire the text */
		char *s = buf + 2;

		/* Length of the text */
		/*:::D�s���Z������Ƃ��ɒe��*/
		int len = strlen(s);

		/*:::�������̏󋵂ɂ���Ă͂����ɗ��Ȃ��H*/
		if (init_flags & INIT_ONLY_BUILDINGS) return (0);

		for (*x = xmin, i = 0; ((*x < xmax) && (i < len)); (*x)++, s++, i++)
		{
			/* Access the grid */
			
			cave_type *c_ptr = &cave[*y][*x];

			/*::: "D:"�̎��̃A�h���X*/
			int idx = s[0];

			/*:::letter��info.txt�ŋL�q����ĂȂ������ɂ��Ă͂ǂ��ŋL�q����Ă���H�H�H*/
			/*:::���ꕔ�̕ǂȂǂ�q_pref.txt�ɋL�q����Ă�init_dungeon_quests()�̂Ƃ��ǂݍ��܂�Ă���*/
			/*:::���Ƃ͎��f_info[]����*/
			int object_index = letter[idx].object;
			int monster_index = letter[idx].monster;
			int random = letter[idx].random;
			int artifact_index = letter[idx].artifact;

			/* Lay down a floor */
			/*:::�ǂ⏰�͂����Ŕz�u����Ă���?*/
			c_ptr->feat = conv_dungeon_feat(letter[idx].feature);

			/* Only the features */
			/*:::�Ȃ�����ȂƂ����continue����̂��킩��Ȃ��Bfor�̊J�n���炱���܂�init_flags���ς��l�q���Ȃ�����break����΂悳�����Ȃ��񂾂�*/
			if (init_flags & INIT_ONLY_FEATURES) continue;

			/* Cave info */
			c_ptr->info = letter[idx].cave_info;

			/* Create a monster */
			if (random & RANDOM_MONSTER)
			{
				/*::: <*10>�Ƃ��̏��� monster_level�̓O���[�o���ϐ�*/
				monster_level = base_level + monster_index;
				/*:::�����X�^�[��z�u�B��񂵁B*/
				place_monster(*y, *x, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP));

				monster_level = base_level;
			}
			else if (monster_index)
			{
				int old_cur_num, old_max_num;/*:::�N���[���֌W�̏����p?*/
				bool clone = FALSE;

				/*:::�N���[��*/
				if (monster_index < 0)
				{
					monster_index = -monster_index;
					clone = TRUE;
				}
				/*:::cur_num:���ݑ��ݐ� max_num:�ő呶�݉\��*/
				old_cur_num = r_info[monster_index].cur_num;
				old_max_num = r_info[monster_index].max_num;

				/* Make alive again */
				/*::: ���j�[�N�Ȃ炷�łɎ���łĂ��o���H�G���b�N�p��*/
				if (r_info[monster_index].flags1 & RF1_UNIQUE)
				{
					r_info[monster_index].cur_num = 0;
					r_info[monster_index].max_num = 1;
				}

				/* Make alive again */
				/* Hack -- Non-unique Nazguls are semi-unique */
				/*:::�i�Y�O���p�H�g���ĂȂ��C�����邪*/
				else if (r_info[monster_index].flags7 & RF7_NAZGUL)
				{
					if (r_info[monster_index].cur_num == r_info[monster_index].max_num)
					{
						r_info[monster_index].max_num++;
					}
				}

				/* Place it */
				/*:::place_monster()����������ɗ���*/
				///mod140405 hack- �N�G�X�ginfo.txt�̃����X�^�[�C���f�b�N�X�ԍ���w��t�����letter[idx].random�Ƀt���O�������ă����X�^�[���N���Ĕz�u�����悤�ɂ���
				if(random & MON_FORCE_FRIENDLY)
					place_monster_aux(0, *y, *x, monster_index, (PM_NO_KAGE | PM_FORCE_FRIENDLY));
				else if(random & MON_FORCE_PET)
					place_monster_aux(0, *y, *x, monster_index, (PM_NO_KAGE | PM_FORCE_PET));
				else if(random & MON_NO_SLEEP)
					place_monster_aux(0, *y, *x, monster_index, (PM_NO_KAGE));
				else
					place_monster_aux(0, *y, *x, monster_index, (PM_ALLOW_SLEEP | PM_NO_KAGE));

				if (clone)
				{
					/* clone */
					m_list[hack_m_idx_ii].smart |= SM_CLONED;

					/* Make alive again for real unique monster */
					/*:::�N���[�������񂾂Ƃ��{�����o�Ă��Ȃ��Ȃ�Ȃ��悤�ɂ��Ă�炵��*/
					r_info[monster_index].cur_num = old_cur_num;
					r_info[monster_index].max_num = old_max_num;
				}
			}

			/* Object (and possible trap) */
			/*:::�H�쉮�~�Ƃ�*/
			if ((random & RANDOM_OBJECT) && (random & RANDOM_TRAP))
			{
				object_level = base_level + object_index;

				/*
				 * Random trap and random treasure defined
				 * 25% chance for trap and 75% chance for object
				 */
				if (randint0(100) < 75)
				{
					place_object(*y, *x, 0L);
				}
				else
				{
					place_trap(*y, *x);
				}

				object_level = base_level;
			}
			else if (random & RANDOM_OBJECT)
			{
				object_level = base_level + object_index;

				/* Create an out of deep object */
				if (randint0(100) < 75)
					place_object(*y, *x, 0L);
				else if (randint0(100) < 80)
					place_object(*y, *x, AM_GOOD);
				else
					place_object(*y, *x, AM_GOOD | AM_GREAT);

				object_level = base_level;
			}
			/* Random trap */
			else if (random & RANDOM_TRAP)
			{
				place_trap(*y, *x);
			}
			/* Hidden trap (or door) */
			else if (letter[idx].trap)
			{
				c_ptr->mimic = c_ptr->feat;
				c_ptr->feat = conv_dungeon_feat(letter[idx].trap);
			}
			else if (object_index)
			{
				/* Get local object */
				object_type *o_ptr = &object_type_body;

				/* Create the item */
				object_prep(o_ptr, object_index);

				if (o_ptr->tval == TV_GOLD)
				{
					coin_type = object_index - OBJ_GOLD_LIST;
					/*:::���̒��ł�����xobject_prep���Ă΂�Ă���̂��悭�킩���*/
					/*:::�ʃ��[�`��������𐶐��������Ƃ��̂��߂̏����ō���͏璷�H*/
					make_gold(o_ptr);
					coin_type = 0;
				}

				/* Apply magic (no messages, no artifacts) */
				apply_magic(o_ptr, base_level, AM_NO_FIXED_ART | AM_GOOD);

				//���s���ۓW�̓���A�C�e��
				if(p_ptr->inside_quest == QUEST_MOON_VAULT)
					o_ptr->marked |= OM_SPECIAL_FLAG;
				/*:::�A�C�e�����w��}�X�ɔz�u�Bplace_object()�̌㔼�ł������������Ă���*/
				drop_here(o_ptr, *y, *x);
			}

			/* Artifact */
			if (artifact_index)
			{
				/*:::Mega Hack - �󕨌ɂŁ����Ȃ��i�S�ē���ς݁A���Ȃ��I�v�V�����j�̂Ƃ�quest[].k_idx��k_info[]�Y����idx�����肱���Ł���������B*/
				if(quest[p_ptr->inside_quest].type ==  QUEST_TYPE_FIND_RANDOM_ARTIFACT)
				{
					object_type forge;
					object_type *q_ptr = &forge;
					object_level = 50;
					do
					{
						object_prep(q_ptr, quest[p_ptr->inside_quest].k_idx);
						create_artifact(q_ptr, FALSE);
					}while(object_is_cursed(q_ptr));
					q_ptr->marked |= OM_SPECIAL_FLAG;
					drop_here(q_ptr, *y, *x);

				}
				else if (a_info[artifact_index].cur_num)
				{
					/*:::���łɊY���A�[�e�B�t�@�N�g���o���ς݂̏ꍇ�͊l���̊����𐶐�*/
					//���������ɂ���
					//�����ʂ̃N�G�X�g��V�ŃK���h���G�����������ɂȂ��Ă��܂����̂Œ����B�X�}�b�v�������Ő��������̂�Y��Ă����B
					int k_idx;
					object_type forge;
					object_type *q_ptr = &forge;

					//v1.1.13 �i�����̃N�G�X�g��V(�܂̓��)�̕�V�������Ă���ꍇ���������ɂ���
					if(EXTRA_MODE) k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);
					else if(p_ptr->town_num == TOWN_EIENTEI) k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ARTIFACT);
					else k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);

					object_prep(q_ptr, k_idx);
					//���s���ۓW�̓���A�C�e��
					if(p_ptr->inside_quest == QUEST_MOON_VAULT)
						q_ptr->marked |= OM_SPECIAL_FLAG;

					/* Drop it in the dungeon */
					drop_here(q_ptr, *y, *x);
				}
				else
				{
					/* Create the artifact */
					if (create_named_art(artifact_index, *y, *x))
						a_info[artifact_index].cur_num = 1;
				}
			}

			/* Terrain special */
			/*:::����n�`�t���O�������p���H*/
			c_ptr->special = letter[idx].special;
		}

		(*y)++;

		return (0);
	}
	/*:::buf[0] == 'D'��������*/

	/* Process "Q:<number>:<command>:... -- quest info */
	else if (buf[0] == 'Q')
	{
		int num;
		quest_type *q_ptr;

		/*:::�N�G�X�g���e�L�X�g�ł�$����{��Ɖp��̋�ʂɎg���Ă���*/
#ifdef JP
		if (buf[2] == '$')
			return 0;
		num = tokenize(buf + 2, 33, zz, 0);
#else
		if (buf[2] != '$')
			return 0;
		num = tokenize(buf + 3, 33, zz, 0);
#endif

		/* Have we enough parameters? */
		if (num < 3) return (PARSE_ERROR_TOO_FEW_ARGUMENTS);

		/* Get the quest */
		q_ptr = &(quest[atoi(zz[0])]);

		///quest
		/* Process "Q:<q_index>:Q:<type>:<num_mon>:<cur_num>:<max_num>:<level>:<r_idx>:<k_idx>:<flags>" -- quest info */
		/*:::quest_type q_ptr�Ƀp�����[�^���i�[*/
		if (zz[1][0] == 'Q')
		{
			/*:::���̏����܂����܂�c�����ĂȂ�*/
			if (init_flags & INIT_ASSIGN)
			{
				monster_race *r_ptr;
				artifact_type *a_ptr;

				if (num < 9) return (PARSE_ERROR_TOO_FEW_ARGUMENTS);

				q_ptr->type    = atoi(zz[2]);
				q_ptr->num_mon = atoi(zz[3]);
				q_ptr->cur_num = atoi(zz[4]);
				q_ptr->max_num = atoi(zz[5]);
				q_ptr->level   = atoi(zz[6]);
				q_ptr->r_idx   = atoi(zz[7]);
				q_ptr->k_idx   = atoi(zz[8]);
				q_ptr->dungeon = atoi(zz[9]);

				if (num > 10)
					q_ptr->flags  = atoi(zz[10]);

				r_ptr = &r_info[q_ptr->r_idx];
				//���q�ˎ҃N�G�X�g��WANTED�t���O�̃��j�[�N�����X�^�[���ꏏ�ɐݒ肵�Ȃ��Ƃ����Ȃ�
				if(q_ptr->type == QUEST_TYPE_BOUNTY_HUNT)
				{
					if(!q_ptr->r_idx || !(r_ptr->flags3 & RF3_WANTED) || !(r_ptr->flags1 & RF1_UNIQUE)) 
						return(PARSE_ERROR_WANTED_UNIQUE);
				}

				/*:::���q�ˎ҈ȊO�̑œ|�����X�^�[�����j�[�N�̏ꍇQUESTOR�t���O�𗧂Ă�*/
				else if (r_ptr->flags1 & RF1_UNIQUE)
				{
					r_ptr->flags1 |= RF1_QUESTOR;
				}
				a_ptr = &a_info[q_ptr->k_idx];
				a_ptr->gen_flags |= TRG_QUESTITEM;
			}
			return (0);
		}

		/*::: Q:n:R:n:n:n:n:n.....*/
		/*::: �󕨌ɗp�B�����_����V�̃C���f�b�N�X��n��30�܂œ���*/
		else if (zz[1][0] == 'R')
		{
			if (init_flags & INIT_ASSIGN)
			{
				int idx, count = 0;
				int reward_idx = 0;

				/*:::�C���f�b�N�X����̒����烉���_����reward_idx�Ɋi�[���Ă�Bn�ڂ̃A�C�e����1/(n)�̊m���Œu�������邱�ƂőS�ċϓ��m���Ŋ���U����悤��*/
				for (idx = 2; idx < num; idx++)
				{
					int a_idx = atoi(zz[idx]);
					if (a_idx < 1) continue;
					if (a_info[a_idx].cur_num > 0) continue;
					count++;
					if (one_in_(count)) reward_idx = a_idx;
				}

				if (reward_idx)
				{
					/* Set quest's rewarding artifact */
					q_ptr->k_idx = reward_idx;
					a_info[reward_idx].gen_flags |= TRG_QUESTITEM;
				}
				else
				{
					/* Change a quest type to KILL_ALL when all artifact of reward list are got */
					//q_ptr->type = QUEST_TYPE_KILL_ALL;
					///mod151028 ���ꏈ���@�󕨌Ɂ��Ȃ��̂Ƃ���k_idx��k_info��index�����A�����_���A�[�e�B�t�@�N�g�ɂ���
					q_ptr->type = QUEST_TYPE_FIND_RANDOM_ARTIFACT;
					get_obj_num_hook = vault_create_hook_weapon;
					get_obj_num_prep();
					q_ptr->k_idx = get_obj_num(50);

					//hook������
					get_obj_num_hook = NULL;
					get_obj_num_prep();
				}
			}

			return (0);
		}

		/* Process "Q:<q_index>:N:<name>" -- quest name */
		else if (zz[1][0] == 'N')
		{
			if (init_flags & (INIT_ASSIGN | INIT_SHOW_TEXT | INIT_NAME_ONLY))
			{
				strcpy(q_ptr->name, zz[2]);
			}

			return (0);
		}

		/* Process "Q:<q_index>:T:<text>" -- quest description line */
		else if (zz[1][0] == 'T')
		{
			if (init_flags & INIT_SHOW_TEXT)
			{
				strcpy(quest_text[quest_text_line], zz[2]);
				quest_text_line++;
			}

			return (0);
		}
	}

	/* Process "W:<command>: ..." -- info for the wilderness */
	else if (buf[0] == 'W')
	{
		return parse_line_wilderness(buf, ymin, xmin, ymax, xmax, y, x);
	}

	/* Process "P:<y>:<x>" -- player position */
	else if (buf[0] == 'P')
	{
		if (init_flags & INIT_CREATE_DUNGEON)
		{
			if (tokenize(buf + 2, 2, zz, 0) == 2)
			{
				int panels_x, panels_y;
				/*:::�ڍוs���B�E�B���h�E�̕\���ʒu�����̈ʒu�ɍ��킹��H*/
				/* Hack - Set the dungeon size */
				panels_y = (*y / SCREEN_HGT);
				if (*y % SCREEN_HGT) panels_y++;
				cur_hgt = panels_y * SCREEN_HGT;

				panels_x = (*x / SCREEN_WID);
				if (*x % SCREEN_WID) panels_x++;
				cur_wid = panels_x * SCREEN_WID;

				/* Assume illegal panel */
				panel_row_min = cur_hgt;
				panel_col_min = cur_wid;

				/* Place player in a quest level */
				if (p_ptr->inside_quest || EXTRA_QUEST_FLOOR)
				{
					int yy, xx;

					/* Delete the monster (if any) */
					delete_monster(py, px);
					yy = atoi(zz[0]);
					xx = atoi(zz[1]);

					py = yy;
					px = xx;
				}
				/* Place player in the town */
				else if (!p_ptr->oldpx && !p_ptr->oldpy)
				{
					p_ptr->oldpy = atoi(zz[0]);
					p_ptr->oldpx = atoi(zz[1]);
				}
			}
		}

		return (0);
	}

	/* Process "B:<Index>:<Command>:..." -- Building definition */
	else if (buf[0] == 'B')
	{
		return parse_line_building(buf);
	}

	/* Process "M:<type>:<maximum>" -- set maximum values */
	/*:::misc.txt�����߂��� �l�X�ȗv�f��MAX�l���O���[�o���ϐ��Ɋi�[*/
	else if (buf[0] == 'M')
	{
		if (tokenize(buf+2, 2, zz, 0) == 2)
		{
			/* Maximum towns */
			if (zz[0][0] == 'T')
			{
				max_towns = atoi(zz[1]);
			}

			/* Maximum quests */
			else if (zz[0][0] == 'Q')
			{
				max_quests = atoi(zz[1]);
			}

			/* Maximum r_idx */
			else if (zz[0][0] == 'R')
			{
				max_r_idx = atoi(zz[1]);
			}

			/* Maximum k_idx */
			else if (zz[0][0] == 'K')
			{
				max_k_idx = atoi(zz[1]);
			}

			/* Maximum v_idx */
			else if (zz[0][0] == 'V')
			{
				max_v_idx = atoi(zz[1]);
			}

			/* Maximum f_idx */
			else if (zz[0][0] == 'F')
			{
				max_f_idx = atoi(zz[1]);
			}

			/* Maximum a_idx */
			else if (zz[0][0] == 'A')
			{
				max_a_idx = atoi(zz[1]);
			}

			/* Maximum e_idx */
			else if (zz[0][0] == 'E')
			{
				max_e_idx = atoi(zz[1]);
			}

			/* Maximum d_idx */
			else if (zz[0][0] == 'D')
			{
				max_d_idx = atoi(zz[1]); 
			}

			/* Maximum o_idx */
			else if (zz[0][0] == 'O')
			{
				max_o_idx = atoi(zz[1]);
			}

			/* Maximum m_idx */
			else if (zz[0][0] == 'M')
			{
				max_m_idx = atoi(zz[1]);
			}

			/* Wilderness size */
			else if (zz[0][0] == 'W')
			{
				/* Maximum wild_x_size */
				if (zz[0][1] == 'X')
					max_wild_x = atoi(zz[1]);
				/* Maximum wild_y_size */
				if (zz[0][1] == 'Y')
					max_wild_y = atoi(zz[1]);
			}

			return (0);
		}
	}


	/* Failure */
	return (1);
}


static char tmp[8];
static cptr variant = "ZANGBAND";


/*
 * Helper function for "process_dungeon_file()"
 */
/*:::info.txt��"?:[�`]�̍\�������߁B�����炭�u�����𖞂������Ƃ��̂ݎ��̍s�����s����v���߂̏������菈���B*/
/*:::�ŏI�I��v��0�ŏI���Ƃ��Ainfo.txt�̎��̍s���X�L�b�v�����*/
static cptr process_dungeon_file_expr(char **sp, char *fp)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';

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
	/*:::*v�̃A�h���X������"?o?o?"���ĕ�����̃A�h���X�ɂ���*v��?o?o?����*/
	v = "?o?o?";

	/* Analyze */
	if (*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_dungeon_file_expr(&s, &f);

		/* Oops */
		if (!*t)
		{
			/* Nothing */
		}

		/* Function: IOR */
		/*::: OR?*/
		else if (streq(t, "IOR"))
		{
			v = "0";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		/*::: AND?*/
		else if (streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
				if (*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		/*::: NOT? ���������̏�����������ɑ��������ł�1�Ȃ猋�ʂ�0�ɂȂ�H*/
		else if (streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
				if (*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		/*::: ��{�������@���̕ϐ�����Ȃǂɑ����͂�*/
		else if (streq(t, "EQU"))
		{
			v = "0";
			if (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = process_dungeon_file_expr(&s, &f);
				if (streq(t, p)) v = "1";
			}
		}

		/* Function: LEQ */
		/*::: �ϐ���̂�����҂̂ق����傫�����1? ������Ȃ�ŏ��̈ꕶ���̂ݔ�r�H*/
		else if (streq(t, "LEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_dungeon_file_expr(&s, &f);
				if (*t && atoi(p) > atoi(t)) v = "0";
			}
		}

		/* Function: GEQ */
		/*::: �ϐ���̂����O�҂̂ق����傫�����1? ������Ȃ�ŏ��̈ꕶ���̂ݔ�r�H*/
		else if (streq(t, "GEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_dungeon_file_expr(&s, &f);

				/* Compare two numbers instead of string */
				if (*t && atoi(p) < atoi(t)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
		}

		/* Verify ending */
		/*::: ?o?o?�������������̕�����ɂ͉��̈Ӗ�������̂��H*/
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

			/* Graphics */
			else if (streq(b+1, "GRAF"))
			{
				v = ANGBAND_GRAF;
			}

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

			/* Player name */
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

			/* Town */
			else if (streq(b+1, "TOWN"))
			{
				sprintf(tmp, "%d", p_ptr->town_num);
				v = tmp;
			}

			/* Level */
			else if (streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%d", p_ptr->lev);
				v = tmp;
			}

			/* Current quest number */
			else if (streq(b+1, "QUEST_NUMBER"))
			{
				sprintf(tmp, "%d", p_ptr->inside_quest);
				v = tmp;
			}

			/* Number of last quest */
			else if (streq(b+1, "LEAVING_QUEST"))
			{
				sprintf(tmp, "%d", leaving_quest);
				v = tmp;
			}

			/* Quest type */
			else if (prefix(b+1, "QUEST_TYPE"))
			{
				/* "QUEST_TYPE" uses a special parameter to determine the type of the quest */
				sprintf(tmp, "%d", quest[atoi(b+11)].type);
				v = tmp;
			}

			/* Quest status */
			else if (prefix(b+1, "QUEST"))
			{
				/* "QUEST" uses a special parameter to determine the number of the quest */
				/*:::atoi�͎w�肵���A�h���X���琔���������Ă�����艽�����ł�int�ɕϊ�����炵��*/
				sprintf(tmp, "%d", quest[atoi(b+6)].status);
				v = tmp;
			}

			///mod140401 ���̍��d���l�b�g���[�N��������1
			else if (prefix(b+1, "GLASSROOTS"))
			{
				if(p_ptr->grassroots > 0) v="1";
				else v="0";
			}

			///mod160223 Extra���[�h�̂Ƃ�1
			else if (prefix(b+1, "EXTRA_MODE"))
			{
				if(EXTRA_MODE) v="1";
				else v="0";
			}


			///mod140113 �Ē��펞���ǂ����̔���ǉ� ?:[EQU $RETRY(�N�G�X�g�ԍ�) 1]�@�̂悤�Ɏg��
			else if (prefix(b+1, "RETRY"))
			{
				if(quest[atoi(b+6)].flags & QUEST_FLAG_RETRY) v="1";
				else v="0";
			}

			//v1.1.56�ǉ�
			else if (prefix(b + 1, "TRUE_WINNER"))
			{
				if (PLAYER_IS_TRUE_WINNER || p_ptr->wizard) v = "1";
				else v = "0";
			}

			/* Random */
			else if (prefix(b+1, "RANDOM"))
			{
				/* "RANDOM" uses a special parameter to determine the number of the quest */
				sprintf(tmp, "%d", (int)(seed_town%atoi(b+7)));
				v = tmp;
			}
			//v1.1.98 �������ɜ߈˂��ꂽ��ԂŁu�A�����������U�v�N�G�X�g��̂����Ƃ� ?:[EQU $POSSESSED 1]
			//��̂��Ă���N�G�X�g�_���W�����ɍs���܂łɜ߈ˉ������Ă��Ă����̔����ʂ�
			else if (streq(b + 1, "POSSESSED"))
			{
				if (p_ptr->quest_special_flag & QUEST_SP_FLAG_HANGOKU2_POSSESSED)
					v = "1";
				else
					v = "0";
			}

			/* Variant name */
			else if (streq(b+1, "VARIANT"))
			{
				v = variant;
			}

			/* Wilderness */
			else if (streq(b+1, "WILDERNESS"))
			{
				if (vanilla_town)
					sprintf(tmp, "NONE");
				else if (lite_town)
					sprintf(tmp, "LITE");
				else
					sprintf(tmp, "NORMAL");
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

/*:::�`_info.txt�����߂��A�N�G�X�g�_���W�����𐶐�������}�b�v�𐶐������蒬�̐ݔ��̏����O���[�o���ϐ��Ɋi�[����*/
/*:::x/y min/max �͐��������_���W�����A�}�b�v�̍��W�͈́H*/

errr process_dungeon_file(cptr name, int ymin, int xmin, int ymax, int xmax)
{
	FILE *fp;

	char buf[1024];

	int num = -1;

	errr err = 0;

	bool bypass = FALSE;

	int x = xmin, y = ymin;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_EDIT, name);

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* No such file */
	if (!fp) return (-1);


	/* Process the file */
	/*:::�����œǂ܂��s�̓}�b�v�����łȂ��_���W������N�G�X�g�̃p�����[�^�ނ��܂�*/
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Count lines */
		num++;

		/* Skip "empty" lines */
		if (!buf[0]) continue;


		/* Skip "blank" lines */
		/*:::�󔒂Ŏn�܂�s�͑S�ăX�L�b�v�H*/
	/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN
		if (iswspace(buf[0])) continue;
#else
		if (isspace(buf[0])) continue;
#endif
		/* Skip comments */
		if (buf[0] == '#') continue;

		/* Process "?:<expr>" */
		if ((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			/*:::*/
			cptr v;
			char *s;

			/* Start */
			/*:::s��buf�̎O�����ڂ̃A�h���X�����Ă���B�܂�?:�̌�̏ꏊ*/
			s = buf + 2;

			/* Parse the expr */
			/*:::info_txt��"?:[�`]�̍\�������߁B�u�����𖞂������Ƃ��̂ݎ��ȍ~�̍s�����s����v�����B*/
			v = process_dungeon_file_expr(&s, &f);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			/* Continue */
			/*:::���̍s��ǂ݂ɍs���Ă���Bbypass�ȊO�͎c��Ȃ�*/
			continue;
		}

		/* Apply conditionals */
		/*bypass��True(?:[�`]���O�̍s�ɂ������������𖞂����Ȃ�)�̂Ƃ����̎��ȍ~�̍s�̓p�X�����B�����?:�s��bypass��FALSE�ɂȂ�܂ő����炵��*/
		if (bypass) continue;

		/* Process the line */
		/*:::�e�s�����߂����s�B�n�`�⃂���X�^�[��A�C�e����z�u����*/

		err = process_dungeon_file_aux(buf, ymin, xmin, ymax, xmax, &y, &x);

		/* Oops */
		if (err) break;
	}

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < PARSE_ERROR_MAX)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d (%s) at line %d of '%s'.", err, oops, num, name);
#ifdef JP
msg_format("'%s'����͒��B", buf);
#else
		msg_format("Parsing '%s'.", buf);
#endif

		msg_print(NULL);
	}


	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (err);
}



#if 0
void write_r_info_txt(void)
{
	int i, j, z, fc, bc;
	int dlen;

	cptr flags[32 * 10];

	u32b f_ptr[10];
	cptr *n_ptr[10];

	monster_race *r_ptr;

	monster_blow *b_ptr;

	FILE *fff = fopen("output.txt", "wt");

	cptr desc;

	int mode = -1;

	if (!fff) return;

	fprintf(fff, "# File: r_info.txt (autogenerated)\n\n");

	fprintf(fff, "# Version stamp (required)\n\n");

	/* Write Version */
	fprintf(fff, "V:%d.%d.%d\n\n\n", r_head->v_major, r_head->v_minor, r_head->v_patch);

	/* Write a note */
	fprintf(fff, "##### The Player #####\n\n");

	for (z = -1; z < alloc_race_size; z++)
	{
		/* Output the monsters in order */
		i = (z >= 0) ? alloc_race_table[z].index : 0;

		/* Acquire the monster */
		r_ptr = &r_info[i];

		/* Ignore empty monsters */
		if (!strlen(r_name + r_ptr->name)) continue;

		/* Ignore useless monsters */
		if (i && !r_ptr->speed) continue;

		/* Write a note if necessary */
		if (i && (!r_ptr->level != !mode))
		{
			/* Note the town */
			if (!r_ptr->level)
			{
				fprintf(fff, "\n##### Town monsters #####\n\n");
			}
			/* Note the dungeon */
			else
			{
				fprintf(fff, "\n##### Normal monsters #####\n\n");
			}

			/* Record the change */
			mode = r_ptr->level;
		}

		/* Acquire the flags */
		f_ptr[0] = r_ptr->flags1; n_ptr[0] = r_info_flags1;
		f_ptr[1] = r_ptr->flags2; n_ptr[1] = r_info_flags2;
		f_ptr[2] = r_ptr->flags3; n_ptr[2] = r_info_flags3;
		f_ptr[3] = r_ptr->flags4; n_ptr[3] = r_info_flags4;
		f_ptr[4] = r_ptr->flags5; n_ptr[4] = r_info_flags5;
		f_ptr[5] = r_ptr->flags6; n_ptr[5] = r_info_flags6;
		f_ptr[6] = r_ptr->flags7; n_ptr[6] = r_info_flags7;
		f_ptr[7] = r_ptr->flags8; n_ptr[7] = r_info_flags8;
		f_ptr[8] = r_ptr->flags9; n_ptr[8] = r_info_flags9;
		f_ptr[9] = r_ptr->flagsr; n_ptr[9] = r_info_flagsr;

		/* Write New/Number/Name */
		fprintf(fff, "N:%d:%s\n", z + 1, r_name + r_ptr->name);

		/* Write Graphic */
		fprintf(fff, "G:%c:%c\n", r_ptr->d_char, color_char[r_ptr->d_attr]);

		/* Write Information */
		fprintf(fff, "I:%d:%dd%d:%d:%d:%d\n", r_ptr->speed, r_ptr->hdice, r_ptr->hside,
														  r_ptr->aaf, r_ptr->ac, r_ptr->sleep);

		/* Write more information */
		fprintf(fff, "W:%d:%d:%d:%ld\n", r_ptr->level, r_ptr->rarity, r_ptr->extra, r_ptr->mexp);

		/* Write Blows */
		for(j = 0; j < 4; j++)
		{
			b_ptr = &(r_ptr->blow[j]);

			/* Stop when done */
			if (!b_ptr->method) break;

			/* Write the blows */
			fprintf(fff, "B:%s:%s:%dd%d\n", r_info_blow_method[b_ptr->method],
													  r_info_blow_effect[b_ptr->effect],
													  b_ptr->d_dice, b_ptr->d_side);
		}

		/* Extract the flags */
		for (fc = 0, j = 0; j < 32 * 3; j++)
		{
			/* Check this flag */
			if (f_ptr[j / 32] & (1L << (j % 32))) flags[fc++] = n_ptr[j / 32][j % 32];
		}

		/* Extract the extra flags */
		for (j = 32 * 6; j < 32 * 10; j++)
		{
			/* Check this flag */
			if (f_ptr[j / 32] & (1L << (j % 32))) flags[fc++] = n_ptr[j / 32][j % 32];
		}

		/* Write the flags */
		for (j = 0; j < fc;)
		{
			char buf[160];

			/* Start the line */
			sprintf(buf, "F:");

			for (bc = 0; (bc < 60) && (j < fc); j++)
			{
				char t[80];

				/* Format the flag */
				sprintf(t, "%s%s", flags[j], (j < fc - 1) ? " | " : "");

				/* Add it to the buffer */
				strcat(buf, t);

				/* Note the length */
				bc += strlen(t);
			}

			/* Done with this line; write it */
			fprintf(fff, "%s\n", buf);
		}

		/* Write Spells if applicable */
		if (r_ptr->freq_spell)
		{
			/* Write the frequency */
			fprintf(fff, "S:1_IN_%d | \n", 100 / r_ptr->freq_spell);

			/* Extract the spell flags */
			for (fc = 0, j = 96; j < 192; j++)
			{
				/* Check this flag */
				if (f_ptr[j / 32] & (1L << (j % 32))) flags[fc++] = n_ptr[j / 32][j % 32];
			}

			/* Write the flags */
			for (j = 0; j < fc;)
			{
				char buf[160], *t;

				/* Start the line */
				sprintf(buf, "S:");

				for (bc = 0, t = buf + 2; (bc < 60) && (j < fc); j++)
				{
					int tlen;

					/* Format the flag */
					sprintf(t, "%s%s", flags[j], (j < fc - 1) ? " | " : "");

					tlen = strlen(t);

					/* Note the length */
					bc += tlen;

					/* Advance */
					t += tlen;
				}

				/* Done with this line; write it */
				fprintf(fff, "%s\n", buf);
			}
		}

		/* Acquire the description */
		desc = r_text + r_ptr->text;
		dlen = strlen(desc);

		/* Write Description */
		for (j = 0; j < dlen;)
		{
			char buf[160], *t;

			/* Start the line */
			sprintf(buf, "D:");

	/*:::///patch131222�{��rev3510�̏C����K�p�H*/
#ifdef PATCH_ORIGIN
			for (bc = 0, t = buf + 2; ((bc < 60) || !iswspace(desc[j])) && (j < dlen); j++, bc++, t++)
#else
			for (bc = 0, t = buf + 2; ((bc < 60) || !isspace(desc[j])) && (j < dlen); j++, bc++, t++)
#endif
			{
				*t = desc[j];
			}

			/* Terminate it */
			*t = '\0';

			/* Done with this line; write it */
			fprintf(fff, "%s\n", buf);
		}

		/* Space between entries */
		fprintf(fff, "\n");
	}

	/* Done */
	fclose(fff);
}

#endif
