/* Purpose: Object flavor code */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 */
/*:::���@�������ȂǁA�ŏ�����Ӓ�ς݂̃A�C�e���̔���*/
static bool object_easy_know(int i)
{
	object_kind *k_ptr = &k_info[i];

	/* Analyze the "tval" */
	switch (k_ptr->tval)
	{
		/* Spellbooks */
		/*
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		case TV_CRAFT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		case TV_HEX_BOOK:
		*/
		case TV_BOOK_ELEMENT:
		case TV_BOOK_FORESEE:
		case TV_BOOK_ENCHANT:
		case TV_BOOK_SUMMONS:
		case TV_BOOK_MYSTIC:
		case TV_BOOK_LIFE:
		case TV_BOOK_PUNISH:
		case TV_BOOK_NATURE:
		case TV_BOOK_TRANSFORM:
		case TV_BOOK_DARKNESS:
		case TV_BOOK_NECROMANCY:
		case TV_BOOK_CHAOS:
  		case TV_BOOK_HISSATSU:
  		case TV_BOOK_MEDICINE:
		case TV_BOOK_OCCULT:
		{
			return (TRUE);
		}

		/* Simple items */
		case TV_FLASK:
		case TV_WHISTLE:
		case TV_MATERIAL:
		case TV_SOUVENIR:
		case TV_COMPOUND:
		case TV_ALCOHOL:
		case TV_SWEETS:
		case TV_SOFTDRINK:
		case TV_MACHINE:
		case TV_MUSHROOM:
		case TV_POTION:
		case TV_SCROLL:
		case TV_ROD:
		case TV_FOOD:
		case TV_BUNBUN:
		case TV_KAKASHI:
		case TV_ITEMCARD:
		case TV_STRANGE_OBJ:
		case TV_SPELLCARD:
		case TV_ABILITY_CARD:
		{
			return (TRUE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Create a name from random parts.
 */
void get_table_name_aux(char *out_string)
{
#ifdef JP
	char Syllable[80];
	get_rnd_line("aname_j.txt", 1, Syllable);
	strcpy(out_string, Syllable);
	get_rnd_line("aname_j.txt", 2, Syllable);
	strcat(out_string, Syllable);
#else
#define MAX_SYLLABLES 164       /* Used with scrolls (see below) */

	static cptr syllables[MAX_SYLLABLES] = {
		"a", "ab", "ag", "aks", "ala", "an", "ankh", "app",
		"arg", "arze", "ash", "aus", "ban", "bar", "bat", "bek",
		"bie", "bin", "bit", "bjor", "blu", "bot", "bu",
		"byt", "comp", "con", "cos", "cre", "dalf", "dan",
		"den", "der", "doe", "dok", "eep", "el", "eng", "er", "ere", "erk",
		"esh", "evs", "fa", "fid", "flit", "for", "fri", "fu", "gan",
		"gar", "glen", "gop", "gre", "ha", "he", "hyd", "i",
		"ing", "ion", "ip", "ish", "it", "ite", "iv", "jo",
		"kho", "kli", "klis", "la", "lech", "man", "mar",
		"me", "mi", "mic", "mik", "mon", "mung", "mur", "nag", "nej",
		"nelg", "nep", "ner", "nes", "nis", "nih", "nin", "o",
		"od", "ood", "org", "orn", "ox", "oxy", "pay", "pet",
		"ple", "plu", "po", "pot", "prok", "re", "rea", "rhov",
		"ri", "ro", "rog", "rok", "rol", "sa", "san", "sat",
		"see", "sef", "seh", "shu", "ski", "sna", "sne", "snik",
		"sno", "so", "sol", "sri", "sta", "sun", "ta", "tab",
		"tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
		"ulk", "um", "un", "uni", "ur", "val", "viv", "vly",
		"vom", "wah", "wed", "werg", "wex", "whon", "wun", "x",
		"yerg", "yp", "zun", "tri", "blaa", "jah", "bul", "on",
		"foo", "ju", "xuxu"
	};

	int testcounter = randint1(3) + 1;

	strcpy(out_string, "");

	if (randint1(3) == 2)
	{
		while (testcounter--)
			strcat(out_string, syllables[randint0(MAX_SYLLABLES)]);
	}
	else
	{
		char Syllable[80];
		testcounter = randint1(2) + 1;
		while (testcounter--)
		{
			(void)get_rnd_line("elvish.txt", 0, Syllable);
			strcat(out_string, Syllable);
		}
	}

	out_string[0] = toupper(out_string[1]);

	out_string[16] = '\0';
#endif
}


/*
 * Create a name from random parts with quotes.
 */
void get_table_name(char *out_string)
{
	char buff[80];
	get_table_name_aux(buff);

#ifdef JP
	sprintf(out_string, "�w%s�x", buff);
#else
	sprintf(out_string, "'%s'", buff);
#endif
}


/*
 * Make random Sindarin name
 */
/*:::�A�[�e�B�t�@�N�g�p�ɃV���_������̖��O�����߂�*/
void get_table_sindarin_aux(char *out_string)
{
	char Syllable[80];
#ifdef JP
	char tmp[80];
#endif

	get_rnd_line("sname.txt", 1, Syllable);
#ifdef JP
	strcpy(tmp, Syllable);
#else
	strcpy(out_string, Syllable);
#endif

	get_rnd_line("sname.txt", 2, Syllable);
#ifdef JP
	strcat(tmp, Syllable);
	sindarin_to_kana(out_string, tmp);
#else
	strcat(out_string, Syllable);
#endif
}


/*
 * Make random Sindarin name with quotes
 */
///mod140719 �����_�����j�[�N�����p�ɁA�w�x�������ɂł���悤�ɂ���
void get_table_sindarin(char *out_string ,bool use_brackets)
{
	char buff[80];
	get_table_sindarin_aux(buff);

#ifdef JP
	if(use_brackets) sprintf(out_string, "�w%s�x", buff);
	else sprintf(out_string, "%s", buff);
#else
	sprintf(out_string, "'%s'", buff);
#endif
}


/*
 * Shuffle flavor indices of a group of objects with given tval
 */
static void shuffle_flavors(byte tval)
{
	s16b *k_idx_list;
	int k_idx_list_num = 0;
	int i;

	/* Allocate an array for a list of k_idx */
	C_MAKE(k_idx_list, max_k_idx, s16b);

	/* Search objects with given tval for shuffle */
	for (i = 0; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip non-Rings */
		if (k_ptr->tval != tval) continue;

		/* Paranoia -- Skip objects without flavor */
		if (!k_ptr->flavor) continue;

		/* Skip objects with a fixed flavor name */
		if (have_flag(k_ptr->flags, TR_FIXED_FLAVOR)) continue;

		/* Remember k_idx */
		k_idx_list[k_idx_list_num] = i;

		/* Increase number of remembered indices */
		k_idx_list_num++;
	}

	/* Shuffle flavors */
	for (i = 0; i < k_idx_list_num; i++)
	{
		object_kind *k1_ptr = &k_info[k_idx_list[i]];
		object_kind *k2_ptr = &k_info[k_idx_list[randint0(k_idx_list_num)]];

		/* Swap flavors of this pair */
		s16b tmp = k1_ptr->flavor;
		k1_ptr->flavor = k2_ptr->flavor;
		k2_ptr->flavor = tmp;
	}

	/* Free an array for a list of k_idx */
	C_KILL(k_idx_list, max_k_idx, s16b);
}

/*
 * Prepare the "variable" part of the "k_info" array.
 *
 * The "color"/"metal"/"type" of an item is its "flavor".
 * For the most part, flavors are assigned randomly each game.
 *
 * Initialize descriptions for the "colored" objects, including:
 * Rings, Amulets, Staffs, Wands, Rods, Food, Potions, Scrolls.
 *
 * The first 4 entries for potions are fixed (Water, Apple Juice,
 * Slime Mold Juice, Unused Potion).
 *
 * Scroll titles are always between 6 and 14 letters long.  This is
 * ensured because every title is composed of whole words, where every
 * word is from 1 to 8 letters long (one or two syllables of 1 to 4
 * letters each), and that no scroll is finished until it attempts to
 * grow beyond 15 letters.  The first time this can happen is when the
 * current title has 6 letters and the new word has 8 letters, which
 * would result in a 6 letter scroll title.
 *
 * Duplicate titles are avoided by requiring that no two scrolls share
 * the same first four letters (not the most efficient method, and not
 * the least efficient method, but it will always work).
 *
 * Hack -- make sure everything stays the same for each saved game
 * This is accomplished by the use of a saved "random seed", as in
 * "town_gen()".  Since no other functions are called while the special
 * seed is in effect, so this function is pretty "safe".
 *
 * Note that the "hacked seed" may provide an RNG with alternating parity!
 */
/*:::�A�C�e���̕s�m�薼���V���b�t�����Ă���*/
///item �s�m�薼�̂��銪�����Ȃǂ�ǉ������Ƃ�
void flavor_init(void)
{
	int i;
	u32b state_backup[4];

	/* Hack -- Backup the RNG state */
	Rand_state_backup(state_backup);

	/* Hack -- Induce consistant flavors */
	Rand_state_init(seed_flavor);


	/* Initialize flavor index of each object by itself */
	for (i = 0; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip objects without flavor name */
		if (!k_ptr->flavor_name) continue;

		/*
		 * Initialize flavor index to itself
		 *  -> Shuffle it later
		 */
		k_ptr->flavor = i;
	}

	/* Shuffle Rings */
	shuffle_flavors(TV_RING);

	/* Shuffle Amulets */
	shuffle_flavors(TV_AMULET);

	/* Shuffle Staves */
	shuffle_flavors(TV_STAFF);

	/* Shuffle Wands */
	shuffle_flavors(TV_WAND);

	/* Shuffle Rods */
	shuffle_flavors(TV_ROD);

	/* Shuffle Mushrooms */
	///mod140622 �L�m�R�̃t���[�o�[�u������
	shuffle_flavors(TV_MUSHROOM);

	/* Shuffle Potions */
	shuffle_flavors(TV_POTION);

	/* Shuffle Scrolls */
	shuffle_flavors(TV_SCROLL);


	/* Hack -- Restore the RNG state */
	Rand_state_restore(state_backup);

	/* Analyze every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip "empty" objects */
		if (!k_ptr->name) continue;

		/* No flavor yields aware */
		if (!k_ptr->flavor) k_ptr->aware = TRUE;

		/* Check for "easily known" */
		k_ptr->easy_know = object_easy_know(i);
	}
}


/*
 * Print a char "c" into a string "t", as if by sprintf(t, "%c", c),
 * and return a pointer to the terminator (t + 1).
 */
static char *object_desc_chr(char *t, char c)
{
	/* Copy the char */
	*t++ = c;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Print a string "s" into a string "t", as if by strcpy(t, s),
 * and return a pointer to the terminator.
 */
static char *object_desc_str(char *t, cptr s)
{
	/* Copy the string */
	while (*s) *t++ = *s++;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}



/*
 * Print an unsigned number "n" into a string "t", as if by
 * sprintf(t, "%u", n), and return a pointer to the terminator.
 */
static char *object_desc_num(char *t, uint n)
{
	uint p;

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}




#ifdef JP
/*
 * ���{��̌��\�����[�`��
 *�icmd1.c �ŗ��p���邽�߂� object_desc_japanese ����ړ������B�j
 */
///item �A�C�e���̌��𐔂���Ƃ��̒P��
///mod131223 �A�C�e���̌�
char *object_desc_kosuu(char *t, object_type *o_ptr)
{
    t = object_desc_num(t, o_ptr->number);

    switch (o_ptr->tval)
    {
      case TV_BOLT:
      case TV_ARROW:
      case TV_POLEARM:
      case TV_HAMMER:
      case TV_STICK:
      case TV_SPEAR:
  	  case TV_OTHERWEAPON:
      case TV_STAFF:
      case TV_WAND:
      case TV_ROD:
      case TV_RIBBON:
      case TV_SOFTDRINK:
      case TV_ALCOHOL:

	  {
	  t = object_desc_str(t, "�{");
	  break;
      }
	  case TV_PHOTO:
	  case TV_ITEMCARD:
	  case TV_ABILITY_CARD:
      {
	  t = object_desc_str(t, "��");
	  break;
      }

	  case TV_SCROLL:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_POTION:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
	  case TV_BOOK_ELEMENT:
	  case TV_BOOK_FORESEE:
	  case TV_BOOK_ENCHANT:
	  case TV_BOOK_SUMMONS:
	  case TV_BOOK_MYSTIC:
	  case TV_BOOK_LIFE:
	  case TV_BOOK_PUNISH:
	  case TV_BOOK_NATURE:
	  case TV_BOOK_TRANSFORM:
	  case TV_BOOK_DARKNESS:
	  case TV_BOOK_NECROMANCY:
	  case TV_BOOK_CHAOS:
  	  case TV_BOOK_HISSATSU:
  	  case TV_BOOK_MEDICINE:
  	  case TV_MAGICITEM:
  	  case TV_CAPTURE:
	  case TV_BOOK_OCCULT:
/*
	  case  TV_LIFE_BOOK:
      case  TV_SORCERY_BOOK:
      case  TV_NATURE_BOOK:
      case  TV_CHAOS_BOOK:
      case  TV_DEATH_BOOK:
      case  TV_TRUMP_BOOK:
      case  TV_ARCANE_BOOK:
      case  TV_CRAFT_BOOK:
      case  TV_DAEMON_BOOK:
      case  TV_CRUSADE_BOOK:
      case  TV_MUSIC_BOOK:
      case  TV_HISSATSU_BOOK:
	  case TV_HEX_BOOK:
*/
	  {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_CLOTHES:
      case TV_ARMOR:
      case TV_DRAG_ARMOR:
      case TV_CLOAK:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_KNIFE:
	  case TV_SWORD:
      case TV_KATANA:
      case TV_BOW:
      case TV_AXE:
	  {
	  t = object_desc_str(t, "�U");
	  break;
      }
      case TV_BOOTS:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_GUN:
      {
		  t = object_desc_str(t, "��");
		  break;
      }
      case TV_BULLET:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
	    /* �H�ׂ��� by ita */
      case TV_FOOD:
      case TV_SWEETS:
	  {
	      t = object_desc_str(t, "�H");
	      break;
      }
      default:
      {
	  if (o_ptr->number < 10)
	  {
	      t = object_desc_str(t, "��");
	  }
	  else
	  {
	      t = object_desc_str(t, "��");
	  }
	  break;
      }
  }
  return (t);		       
}
#endif

/*
 * Print an signed number "v" into a string "t", as if by
 * sprintf(t, "%+d", n), and return a pointer to the terminator.
 * Note that we always print a sign, either "+" or "-".
 */
static char *object_desc_int(char *t, sint v)
{
	uint p, n;

	/* Negative */
	if (v < 0)
	{
		/* Take the absolute value */
		n = 0 - v;

		/* Use a "minus" sign */
		*t++ = '-';
	}

	/* Positive (or zero) */
	else
	{
		/* Use the actual number */
		n = v;

		/* Use a "plus" sign */
		*t++ = '+';
	}

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Structs and tables for Auto Inscription for flags
 */

typedef struct flag_insc_table
{
#ifdef JP
	cptr japanese;
#endif
	cptr english;
	int flag;
	int except_flag;
} flag_insc_table;

#ifdef JP
static flag_insc_table flag_insc_plus[] =
{
	{ "�U", "At", TR_BLOWS, -1 },
	{ "��", "Sp", TR_SPEED, -1 },
	{ "�r", "St", TR_STR, -1 },
	{ "�m", "In", TR_INT, -1 },
	{ "��", "Wi", TR_WIS, -1 },
	{ "��", "Dx", TR_DEX, -1 },
	{ "��", "Cn", TR_CON, -1 },
	{ "��", "Ch", TR_CHR, -1 },
	{ "��", "Md", TR_MAGIC_MASTERY, -1 },
	{ "�B", "Sl", TR_STEALTH, -1 },
	{ "�T", "Sr", TR_SEARCH, -1 },
	{ "��", "If", TR_INFRA, -1 },
	{ "�@", "Dg", TR_TUNNEL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "�_", "Ac", TR_IM_ACID, -1 },
	{ "�d", "El", TR_IM_ELEC, -1 },
	{ "��", "Fi", TR_IM_FIRE, -1 },
	{ "��", "Co", TR_IM_COLD, -1 },
	{ NULL, NULL, 0, -1 }
};

///mod131228 TR�t���O�ύX
static flag_insc_table flag_insc_resistance[] =
{
	{ "�_", "Ac", TR_RES_ACID, TR_IM_ACID },
	{ "�d", "El", TR_RES_ELEC, TR_IM_ELEC },
	{ "��", "Fi", TR_RES_FIRE, TR_IM_FIRE },
	{ "��", "Co", TR_RES_COLD, TR_IM_COLD },
	{ "��", "Po", TR_RES_POIS, -1 },
	{ "�M", "Li", TR_RES_LITE, -1 },
	{ "��", "Dk", TR_RES_DARK, -1 },
	{ "�j", "Sh", TR_RES_SHARDS, -1 },
	{ "��", "Wa", TR_RES_WATER, -1 },
	{ "��", "Ho", TR_RES_HOLY, -1 },
	{ "��", "Ti", TR_RES_TIME, -1 },
	{ "��", "So", TR_RES_SOUND, -1 },
	{ "��", "Nt", TR_RES_NETHER, -1 },
	{ "��", "Ca", TR_RES_CHAOS, -1 },
	{ "��", "Di", TR_RES_DISEN, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
	{ "��", "Es", TR_EASY_SPELL, -1 },
	{ "��", "Dm", TR_DEC_MANA, -1 },
	{ "��", "Th", TR_THROW, -1 },
	{ "�o", "To", TR_GENZI, -1 },
	{ "��", "Rf", TR_REFLECT, -1 },
	{ "��", "Si", TR_SEE_INVIS, -1 },
	{ "��", "Fa", TR_FREE_ACT, -1 },
	{ "��", "Fe", TR_RES_FEAR, -1 },
	{ "��", "Co", TR_RES_CONF, -1 },
	{ "��", "Bl", TR_RES_BLIND, -1 },
	{ "��", "In", TR_RES_INSANITY, -1 },
	{ "�H", "Sd", TR_SLOW_DIGEST, -1 },
	{ "��", "Rg", TR_REGEN, -1 },
	{ "��", "Lv", TR_LEVITATION, -1 },
	{ "��", "Lu", TR_LITE, -1 },
	{ "�x", "Wr", TR_WARNING, -1 },
	{ "�r", "Sp", TR_SPEEDSTER, -1 },
	{ "�{", "Xm", TR_XTRA_MIGHT, -1 },
	{ "��", "Xs", TR_XTRA_SHOTS, -1 },
	{ "�u", "Te", TR_TELEPORT, -1 },
	{ "�{", "Ag", TR_AGGRAVATE, -1 },
	{ "�j", "Bs", TR_BLESSED, -1 },
	{ "��", "Ty", TR_TY_CURSE, -1 },
	{ "��", "C-", TR_ADD_L_CURSE, -1 },
	{ "�f", "C+", TR_ADD_H_CURSE, -1 },
	{ "��", "C+", TR_BOOMERANG, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "��", "F", TR_SH_FIRE, -1 },
	{ "�d", "E", TR_SH_ELEC, -1 },
	{ "��", "C", TR_SH_COLD, -1 },
	{ "��", "M", TR_NO_MAGIC, -1 },
	{ "�u", "T", TR_NO_TELE, -1 },
	{ NULL, NULL, 0, -1 }
};



static flag_insc_table flag_insc_brand[] =
{
	{ "�_", "A", TR_BRAND_ACID, -1 },
	{ "�d", "E", TR_BRAND_ELEC, -1 },
	{ "��", "F", TR_BRAND_FIRE, -1 },
	{ "��", "Co", TR_BRAND_COLD, -1 },
	{ "��", "P", TR_BRAND_POIS, -1 },
	{ "��", "Ca", TR_CHAOTIC, -1 },
	{ "�z", "V", TR_VAMPIRIC, -1 },
	{ "�k", "Q", TR_IMPACT, -1 },
//	{ "��", "S", TR_VORPAL, -1 },
	{ "��", "S", TR_VORPAL, TR_EX_VORPAL },
	{ "��", "M", TR_FORCE_WEAPON, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "��", "*", TR_KILL_EVIL, -1 },
	{ "�l", "p", TR_KILL_HUMAN, -1 },
	{ "��", "D", TR_KILL_DRAGON, -1 },
	{ "�_", "S", TR_KILL_DEITY, -1 },
	{ "�d", "K", TR_KILL_KWAI, -1 },
	{ "��", "C", TR_KILL_GOOD, -1 },
	{ "��", "U", TR_KILL_DEMON, -1 },
	{ "��", "L", TR_KILL_UNDEAD, -1 },
	{ "��", "Z", TR_KILL_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "��", "*", TR_SLAY_EVIL, TR_KILL_EVIL },
	{ "�l", "p", TR_SLAY_HUMAN, TR_KILL_HUMAN },
	{ "��", "D", TR_SLAY_DRAGON, TR_KILL_DRAGON },
	{ "�_", "S", TR_SLAY_DEITY, TR_KILL_DEITY },
	{ "�d", "K", TR_SLAY_KWAI, TR_KILL_KWAI },
	{ "��", "C", TR_SLAY_GOOD, TR_KILL_GOOD },
	{ "��", "U", TR_SLAY_DEMON, TR_KILL_DEMON },
	{ "��", "L", TR_SLAY_UNDEAD, TR_KILL_UNDEAD },
	{ "��", "Z", TR_SLAY_ANIMAL, TR_KILL_ANIMAL },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "��", "Tele", TR_TELEPATHY, -1 },
	{ "��", "Evil", TR_ESP_EVIL, -1 },
	{ "��", "Good", TR_ESP_GOOD, -1 },
	{ "��", "Nolv", TR_ESP_NONLIVING, -1 },
	{ "��", "Uniq", TR_ESP_UNIQUE, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "�l", "p", TR_ESP_HUMAN, -1 },
	{ "��", "D", TR_ESP_DRAGON, -1 },
	{ "�_", "o", TR_ESP_DEITY, -1 },
	{ "�d", "T", TR_ESP_KWAI, -1 },
	{ "��", "U", TR_ESP_DEMON, -1 },
	{ "��", "L", TR_ESP_UNDEAD, -1 },
	{ "��", "Z", TR_ESP_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
	{ "�r", "St", TR_SUST_STR, -1 },
	{ "�m", "In", TR_SUST_INT, -1 },
	{ "��", "Wi", TR_SUST_WIS, -1 },
	{ "��", "Dx", TR_SUST_DEX, -1 },
	{ "��", "Cn", TR_SUST_CON, -1 },
	{ "��", "Ch", TR_SUST_CHR, -1 },
	{ NULL, NULL, 0, -1 }
};

#else
static flag_insc_table flag_insc_plus[] =
{
	{ "At", TR_BLOWS, -1 },
	{ "Sp", TR_SPEED, -1 },
	{ "St", TR_STR, -1 },
	{ "In", TR_INT, -1 },
	{ "Wi", TR_WIS, -1 },
	{ "Dx", TR_DEX, -1 },
	{ "Cn", TR_CON, -1 },
	{ "Ch", TR_CHR, -1 },
	{ "Md", TR_MAGIC_MASTERY, -1 },
	{ "Sl", TR_STEALTH, -1 },
	{ "Sr", TR_SEARCH, -1 },
	{ "If", TR_INFRA, -1 },
	{ "Dg", TR_TUNNEL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "Ac", TR_IM_ACID, -1 },
	{ "El", TR_IM_ELEC, -1 },
	{ "Fi", TR_IM_FIRE, -1 },
	{ "Co", TR_IM_COLD, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_resistance[] =
{
	{ "Ac", TR_RES_ACID, TR_IM_ACID },
	{ "El", TR_RES_ELEC, TR_IM_ELEC },
	{ "Fi", TR_RES_FIRE, TR_IM_FIRE },
	{ "Co", TR_RES_COLD, TR_IM_COLD },
	{ "Po", TR_RES_POIS, -1 },
	{ "Li", TR_RES_LITE, -1 },
	{ "Dk", TR_RES_DARK, -1 },
	{ "Sh", TR_RES_SHARDS, -1 },
	{ "Bl", TR_RES_BLIND, -1 },
	{ "Cf", TR_RES_CONF, -1 },
	{ "So", TR_RES_SOUND, -1 },
	{ "Nt", TR_RES_NETHER, -1 },
	{ "Nx", TR_RES_NEXUS, -1 },
	{ "Ca", TR_RES_CHAOS, -1 },
	{ "Di", TR_RES_DISEN, -1 },
	{ "Fe", TR_RES_FEAR, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
	{ "Es", TR_EASY_SPELL, -1 },
	{ "Dm", TR_DEC_MANA, -1 },
	{ "Th", TR_THROW, -1 },
	{ "Rf", TR_REFLECT, -1 },
	{ "Fa", TR_FREE_ACT, -1 },
	{ "Si", TR_SEE_INVIS, -1 },
	{ "Hl", TR_HOLD_LIFE, -1 },
	{ "Sd", TR_SLOW_DIGEST, -1 },
	{ "Rg", TR_REGEN, -1 },
	{ "Lv", TR_LEVITATION, -1 },
	{ "Lu", TR_LITE, -1 },
	{ "Wr", TR_WARNING, -1 },
	{ "Xm", TR_XTRA_MIGHT, -1 },
	{ "Xs", TR_XTRA_SHOTS, -1 },
	{ "Te", TR_TELEPORT, -1 },
	{ "Ag", TR_AGGRAVATE, -1 },
	{ "Bs", TR_BLESSED, -1 },
	{ "Ty", TR_TY_CURSE, -1 },
	{ "C-", TR_ADD_L_CURSE, -1 },
	{ "C+", TR_ADD_H_CURSE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "F", TR_SH_FIRE, -1 },
	{ "E", TR_SH_ELEC, -1 },
	{ "C", TR_SH_COLD, -1 },
	{ "M", TR_NO_MAGIC, -1 },
	{ "T", TR_NO_TELE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_brand[] =
{
	{ "A", TR_BRAND_ACID, -1 },
	{ "E", TR_BRAND_ELEC, -1 },
	{ "F", TR_BRAND_FIRE, -1 },
	{ "Co", TR_BRAND_COLD, -1 },
	{ "P", TR_BRAND_POIS, -1 },
	{ "Ca", TR_CHAOTIC, -1 },
	{ "V", TR_VAMPIRIC, -1 },
	{ "Q", TR_IMPACT, -1 },
	{ "S", TR_VORPAL, -1 },
	{ "M", TR_FORCE_WEAPON, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "*", TR_KILL_EVIL, -1 },
	{ "p", TR_KILL_HUMAN, -1 },
	{ "D", TR_KILL_DRAGON, -1 },
	{ "o", TR_KILL_ORC, -1 },
	{ "T", TR_KILL_TROLL, -1 },
	{ "P", TR_KILL_GIANT, -1 },
	{ "U", TR_KILL_DEMON, -1 },
	{ "L", TR_KILL_UNDEAD, -1 },
	{ "Z", TR_KILL_ANIMAL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "*", TR_SLAY_EVIL, TR_KILL_EVIL },
	{ "p", TR_SLAY_HUMAN, TR_KILL_HUMAN },
	{ "D", TR_SLAY_DRAGON, TR_KILL_DRAGON },
	{ "o", TR_SLAY_ORC, TR_KILL_ORC },
	{ "T", TR_SLAY_TROLL, TR_KILL_TROLL },
	{ "P", TR_SLAY_GIANT, TR_KILL_GIANT },
	{ "U", TR_SLAY_DEMON, TR_KILL_DEMON },
	{ "L", TR_SLAY_UNDEAD, TR_KILL_UNDEAD },
	{ "Z", TR_SLAY_ANIMAL, TR_KILL_ANIMAL },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "Tele", TR_TELEPATHY, -1 },
	{ "Evil", TR_ESP_EVIL, -1 },
	{ "Good", TR_ESP_GOOD, -1 },
	{ "Nolv", TR_ESP_NONLIVING, -1 },
	{ "Uniq", TR_ESP_UNIQUE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "p", TR_ESP_HUMAN, -1 },
	{ "D", TR_ESP_DRAGON, -1 },
	{ "o", TR_ESP_ORC, -1 },
	{ "T", TR_ESP_TROLL, -1 },
	{ "P", TR_ESP_GIANT, -1 },
	{ "U", TR_ESP_DEMON, -1 },
	{ "L", TR_ESP_UNDEAD, -1 },
	{ "Z", TR_ESP_ANIMAL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
	{ "St", TR_SUST_STR, -1 },
	{ "In", TR_SUST_INT, -1 },
	{ "Wi", TR_SUST_WIS, -1 },
	{ "Dx", TR_SUST_DEX, -1 },
	{ "Cn", TR_SUST_CON, -1 },
	{ "Ch", TR_SUST_CHR, -1 },
	{ NULL, 0, -1 }
};
#endif

/* Simple macro for get_inscription() */
#define ADD_INSC(STR) (void)(ptr = object_desc_str(ptr, (STR)))

/*
 *  Helper function for get_inscription()
 */
static char *inscribe_flags_aux(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE], bool kanji, char *ptr)
{
#ifndef JP
	(void)kanji;
#endif

	while (fi_ptr->english)
	{
		if (have_flag(flgs, fi_ptr->flag) &&
		    (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
#ifdef JP
			ADD_INSC(kanji ? fi_ptr->japanese : fi_ptr->english);
#else
			ADD_INSC(fi_ptr->english);
#endif
		fi_ptr++;
	}

	return ptr;
}


/*
 *  Special variation of have_flag for auto-inscription
 */
static bool have_flag_of(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE])
{
	while (fi_ptr->english)
	{
		if (have_flag(flgs, fi_ptr->flag) &&
		   (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
			return (TRUE);
		fi_ptr++;
	}

	return (FALSE);
}

///item sys flag �A�C�e���̔\�͂�r�΂݂����ɕ\�L���镔���炵��
static char *get_ability_abbreviation(char *ptr, object_type *o_ptr, bool kanji, bool all)
{
	char *prev_ptr = ptr;
	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);


	/* Remove obvious flags */
	if (!all)
	{
		object_kind *k_ptr = &k_info[o_ptr->k_idx];
		int j;
				
		/* Base object */
		for (j = 0; j < TR_FLAG_SIZE; j++)
			flgs[j] &= ~k_ptr->flags[j];

		if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];
					
			for (j = 0; j < TR_FLAG_SIZE; j++)
				flgs[j] &= ~a_ptr->flags[j];
		}

		if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];
					
			for (j = 0; j < TR_FLAG_SIZE; j++)
				flgs[j] &= ~e_ptr->flags[j];
		}
	}


	/* Plusses */
	if (have_flag_of(flag_insc_plus, flgs))
	{
		if (kanji)
			ADD_INSC("+");
	}
	ptr = inscribe_flags_aux(flag_insc_plus, flgs, kanji, ptr);


	/* Immunity */
	if (have_flag_of(flag_insc_immune, flgs))
	{
		if (!kanji && ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
		ADD_INSC("*");
	}
	ptr = inscribe_flags_aux(flag_insc_immune, flgs, kanji, ptr);

	//Hack - �O���}���ɂ�+3���ƋL�q
	if(o_ptr->name1 == ART_GRIMOIRE_OF_MARISA) ADD_INSC("��");
	//v1.1.32 ��ƕ��ɉ��ƋL�q
	if(o_ptr->tval == TV_CLOTHES && o_ptr->sval == SV_CLOTH_WORKER) ADD_INSC("��");

	/* Resistance */
	if (have_flag_of(flag_insc_resistance, flgs))
	{
		if (kanji)
			ADD_INSC("r");
		else if (ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_resistance, flgs, kanji, ptr);

	/* Misc Ability */
	if (have_flag_of(flag_insc_misc, flgs))
	{
		if (ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_misc, flgs, kanji, ptr);

	if (o_ptr->name2 == EGO_BODY_POVERTY) ADD_INSC(";�n");

	/* Aura */
	if (have_flag_of(flag_insc_aura, flgs))
	{
		ADD_INSC("[");
	}
	ptr = inscribe_flags_aux(flag_insc_aura, flgs, kanji, ptr);

	/*:::Hack *�؂ꖡ*�̂Ƃ���|�؂ƕ\���ł���悤�ɂ��Ă݂�*/
	if(have_flag(flgs, TR_EX_VORPAL)) ADD_INSC("|X��");

	/* Brand Weapon */
	if (have_flag_of(flag_insc_brand, flgs))
		ADD_INSC("|");
	ptr = inscribe_flags_aux(flag_insc_brand, flgs, kanji, ptr);

	/* Kill Weapon */
	if (have_flag_of(flag_insc_kill, flgs))
		ADD_INSC("/X");
	ptr = inscribe_flags_aux(flag_insc_kill, flgs, kanji, ptr);

	/* Slay Weapon */
	if (have_flag_of(flag_insc_slay, flgs))
		ADD_INSC("/");
	ptr = inscribe_flags_aux(flag_insc_slay, flgs, kanji, ptr);

	/* Esp */
	if (kanji)
	{
		if (have_flag_of(flag_insc_esp1, flgs) ||
		    have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}
	else
	{
		if (have_flag_of(flag_insc_esp1, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		if (have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}

	/* sustain */
	if (have_flag_of(flag_insc_sust, flgs))
	{
		ADD_INSC("(");
	}
	ptr = inscribe_flags_aux(flag_insc_sust, flgs, kanji, ptr);

	*ptr = '\0';

	return ptr;
}


/*
 *  Get object inscription with auto inscription of object flags.
 */
static void get_inscription(char *buff, object_type *o_ptr)
{
	cptr insc = quark_str(o_ptr->inscription);
	char *ptr = buff;

	/* Not fully identified */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
		/* Copy until end of line or '#' */
		while (*insc)
		{
			if (*insc == '#') break;
#ifdef JP
			if (iskanji(*insc)) *buff++ = *insc++;
#endif
			*buff++ = *insc++;
		}

		*buff = '\0';
		return;
	}

	*buff = '\0';
	for (; *insc; insc++)
	{
		/* Ignore fake artifact inscription */
		if (*insc == '#') break;

		/* {%} will be automatically converted */
		else if ('%' == *insc)
		{
			bool kanji = FALSE;
			bool all;
			cptr start = ptr;

			/* check for too long inscription */
			if (ptr >= buff + MAX_NLEN) continue;

#ifdef JP
			if ('%' == insc[1])
			{
				insc++;
				kanji = FALSE;
			}
			else
			{
				kanji = TRUE;
			}
#endif
			if ('a' == insc[1] && 'l' == insc[2] && 'l' == insc[3])
			{
				all = TRUE;
				insc += 3;
			}
			else
			{
				all = FALSE;
			}

			ptr = get_ability_abbreviation(ptr, o_ptr, kanji, all);

			if (ptr == start)
				ADD_INSC(" ");
		}
		else
		{
			*ptr++ = *insc;
		}
	}
	*ptr = '\0';
}

///mod151031 �󕨌ɗp�ɕ���̃_�C�X���\���ɂ��郋�[�`�����������A
//�󕨌ɍĎ����̂��ߏ���������
bool object_is_quest_target(object_type *o_ptr)
{

	if(quest[p_ptr->inside_quest].type != QUEST_TYPE_FIND_ARTIFACT
		&& quest[p_ptr->inside_quest].type != QUEST_TYPE_FIND_RANDOM_ARTIFACT)
	return FALSE;
	//�����^�[�Q�b�g�̂Ƃ����Ɠ�����ނ̕i��TRUE
	if(quest[p_ptr->inside_quest].type == QUEST_TYPE_FIND_ARTIFACT)
	{
		int a_idx = quest[p_ptr->inside_quest].k_idx;
		if (a_idx)
		{
			artifact_type *a_ptr = &a_info[a_idx];
			if (!(a_ptr->gen_flags & TRG_INSTA_ART))
			{
				if((o_ptr->tval == a_ptr->tval) && (o_ptr->sval == a_ptr->sval))
				{
					return TRUE;
				}
			}
		}
	}
	//�����^�[�Q�b�g�̂Ƃ�
	else
	{
		int k_idx = quest[p_ptr->inside_quest].k_idx;
		if (k_idx)
		{
			if(k_info[k_idx].tval == o_ptr->tval && k_info[k_idx].sval == o_ptr->sval)
				return TRUE;
		}
	}
/*
	if (p_ptr->inside_quest)
	{
		int a_idx = quest[p_ptr->inside_quest].k_idx;
		if (a_idx)
		{
			artifact_type *a_ptr = &a_info[a_idx];
			if (!(a_ptr->gen_flags & TRG_INSTA_ART))
			{
				if((o_ptr->tval == a_ptr->tval) && (o_ptr->sval == a_ptr->sval))
				{
					return TRUE;
				}
			}
		}
	}
*/
	return FALSE;
}

/*:::����̓����_���[�W�𕶎���ɂ��ĕԂ�*/
///mod140930 ���퓊���_���[�W
cptr print_throw_dam(object_type *o_ptr)
{
	//�����܂��炢�͊܂ށH
	int mult = 1; //�ʏ퓊����z��
	int use = 100; //energy_use
	bool suitable_item = FALSE;
	u32b flgs[TR_FLAG_SIZE];
	int dam;
	int dd,ds;
	int turn_dam;

	//�I�v�V����OFF�̂Ƃ� 
	if(!show_throwing_dam) return "";
	//����݂̂��Ώ�
	if(!object_is_melee_weapon(o_ptr)) return "";
	//����ϐg���͓����s��
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE) return " (0/0)";
	//���Ӓ肾�ƈЗ͕s�� (�������Ӓ�ς݂łȂ��Ƃ��̃��[�`�����Ă΂�Ȃ�)
	if(!object_is_known(o_ptr)) return " (?/?)";

	object_flags(o_ptr, flgs);
	if(have_flag(flgs, TR_THROW)) suitable_item = TRUE;

	if (p_ptr->mighty_throw) mult += 2;

	if(ref_skill_exp(SKILL_THROWING) > 1600)
	{
		if(suitable_item)	use -= (ref_skill_exp(SKILL_THROWING) - 1600) / 100;
		else use -= (ref_skill_exp(SKILL_THROWING) -1600) / 128;
		if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE && p_ptr->pclass == CLASS_REIMU)
			use -= osaisen_rank() * 2;
	}
	if(p_ptr->pseikaku == SEIKAKU_TRIGGER) use -= use / 5;

	if(use < 20) use = 20;

	dd = o_ptr->dd;
	ds = o_ptr->ds;

	if(o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE 
		&&(p_ptr->pclass == CLASS_REIMU || p_ptr->pclass == CLASS_PRIEST || p_ptr->pclass == CLASS_SANAE) )
	{
		dd += 3;
	}
	//v1.1.21 ���m�Z���Z�\
	else if ((o_ptr->tval == TV_KNIFE) && HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_KNIFE_MASTERY))
	{
		dd += 2;
	}


	if(suitable_item && object_is_artifact(o_ptr)) dd *= 2;
	dam = dd * (ds + 1) / 2; //�_���[�W�_�C�X���Ғl
	if ((p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
		dam = dam * 7 / 2; //����

	if (o_ptr->to_d > 0) dam += o_ptr->to_d;
	else	dam += -o_ptr->to_d;

	if (suitable_item)
	{
		dam *= (2+mult);
		dam += p_ptr->to_d_m;
	}
	else
	{
		dam *= mult;
	}

	if(dam<0) dam = 0; //Paranoia

	turn_dam = dam * 100 / use;

	return format(" (%d/%d)",dam,turn_dam);

}


/*
 * Creates a description of the item "o_ptr", and stores it in "out_val".
 *
 * One can choose the "verbosity" of the description, including whether
 * or not the "number" of items should be described, and how much detail
 * should be used when describing the item.
 *
 * The given "buf" must be MAX_NLEN chars long to hold the longest possible
 * description, which can get pretty long, including incriptions, such as:
 * "no more Maces of Disruption (Defender) (+10,+10) [+5] (+3 to stealth)".
 * Note that the inscription will be clipped to keep the total description
 * under MAX_NLEN-1 chars (plus a terminator).
 *
 * Note the use of "object_desc_num()" and "object_desc_int()" as hyper-efficient,
 * portable, versions of some common "sprintf()" commands.
 *
 * Note that all ego-items (when known) append an "Ego-Item Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that all artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known", since they
 * have special "descriptions", such as "The Necklace of the Dwarves".
 *
 * Special Lite's use the "k_info" base-name (Phial, Star, or Arkenstone),
 * plus the artifact name, just like any other artifact, if known.
 *
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "k_info" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "k_info" name.  But,
 * they will append the artifact name, just like any artifact, if known.
 *
 * Hack -- Display "The One Ring" as "a Plain Gold Ring" until aware.
 *
 * Mode:
 *   OD_NAME_ONLY        : The Cloak of Death
 *   OD_NAME_AND_ENCHANT : The Cloak of Death [1,+3]
 *   OD_OMIT_INSCRIPTION : The Cloak of Death [1,+3] (+2 to Stealth)
 *   0                   : The Cloak of Death [1,+3] (+2 to Stealth) {nifty}
 *
 *   OD_OMIT_PREFIX      : Forbidden numeric prefix
 *   OD_NO_PLURAL        : Forbidden use of plural 
 *   OD_STORE            : Assume to be aware and known
 *   OD_NO_FLAVOR        : Allow to hidden flavor
 *   OD_FORCE_FLAVOR     : Get un-shuffled flavor name
 */
/*:::�A�C�e���ɂ��ċL�q���� �p�����[�^�A���A���Ӓ薼�ȂǊ֘A*/
///item ego �A�C�e���ɂ��Ă̋L�q
///mod131223 TVAL/SVAL
void object_desc(char *buf, object_type *o_ptr, u32b mode)
{
	/* Extract object kind name */
	cptr            kindname = k_name + k_info[o_ptr->k_idx].name;

	/* Extract default "base" string */
	cptr            basenm = kindname;

	/* Assume no "modifier" string */
	cptr            modstr = "";

	int             power;

	bool            aware = FALSE;
	bool            known = FALSE;
	bool            flavor = TRUE;

	bool            show_weapon = FALSE;
	bool            show_armour = FALSE;

	cptr            s, s0;
	char            *t;

	char            p1 = '(', p2 = ')';
	char            b1 = '[', b2 = ']';
	char            c1 = '{', c2 = '}';

	char            tmp_val[MAX_NLEN+160];
	char            tmp_val2[MAX_NLEN+10];
	char            fake_insc_buf[30];

	u32b flgs[TR_FLAG_SIZE];

	object_type *bow_ptr;

	object_kind *k_ptr = &k_info[o_ptr->k_idx];
	object_kind *flavor_k_ptr = &k_info[k_ptr->flavor];

	int dummy;

	/* Extract some flags */
	object_flags(o_ptr, flgs);

	/* See if the object is "aware" */
	if (object_is_aware(o_ptr)) aware = TRUE;

	/* See if the object is "known" */
	if (object_is_known(o_ptr)) known = TRUE;

	/* Allow flavors to be hidden when aware */
	if (aware && ((mode & OD_NO_FLAVOR) || plain_descriptions)) flavor = FALSE;

	/* Object is in the inventory of a store or spoiler */
	if ((mode & OD_STORE) || (o_ptr->ident & IDENT_STORE))
	{
		/* Don't show flavors */
		flavor = FALSE;

		/* Pretend known and aware */
		aware = TRUE;
		known = TRUE;
	}

	/* Force to be flavor name only */
	if (mode & OD_FORCE_FLAVOR)
	{
		aware = FALSE;
		flavor = TRUE;
		known = FALSE;

		/* Cancel shuffling */
		flavor_k_ptr = k_ptr;
	}

	/* Analyze the object */
	///item res TVAL 
	switch (o_ptr->tval)
	{
		/* Some objects are easy to describe */

		case TV_FLASK:
		case TV_CHEST:
		case TV_WHISTLE:
		case TV_BUNBUN:
		case TV_KAKASHI:
		case TV_SOUVENIR:
		case TV_COMPOUND:
		case TV_MATERIAL:
		case TV_SOFTDRINK:
		case TV_ALCOHOL:
		case TV_SWEETS:
		case TV_FOOD:
		case TV_BOOK_ELEMENT:
		case TV_BOOK_FORESEE:
		case TV_BOOK_ENCHANT:
		case TV_BOOK_SUMMONS:
		case TV_BOOK_MYSTIC:
		case TV_BOOK_LIFE:
		case TV_BOOK_PUNISH:
		case TV_BOOK_NATURE:
		case TV_BOOK_TRANSFORM:
		case TV_BOOK_DARKNESS:
		case TV_BOOK_NECROMANCY:
		case TV_BOOK_CHAOS:
  		case TV_BOOK_HISSATSU:
		case TV_BOOK_OCCULT:
	  	case TV_BOOK_MEDICINE:
			///item ������K�؂ȏꏊ�ɓ������\��
		case TV_MAGICITEM:
		case TV_MAGICWEAPON:
		case TV_MACHINE:
		case TV_RIBBON:
		case TV_ITEMCARD:
		case TV_STRANGE_OBJ:
		case TV_SPELLCARD:
		case TV_ABILITY_CARD:

		{
			break;
		}
		case TV_CAPTURE:
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];

			if (known)
			{
				if (!o_ptr->pval)
				{
#ifdef JP
					modstr = " (��)";
#else
					modstr = " (empty)";
#endif
				}
				//v1.1.16 �d���{�o�O�ɂ��Z�[�u�t�@�C���ǂݍ��ݕs�\�ɑΉ�
				else if(o_ptr->pval < 0 || o_ptr->pval >= max_r_idx)
				{
					sprintf(tmp_val2, " (ERROR:�d���{��r_idx�l���s��(%d))",o_ptr->pval);
					modstr = tmp_val2;

				}
				else
				{
#ifdef JP
					sprintf(tmp_val2, " (%s)",r_name + r_ptr->name);
					modstr = tmp_val2;
#else
					cptr t = r_name + r_ptr->name;

					if (!(r_ptr->flags1 & RF1_UNIQUE))
					{
						sprintf(tmp_val2, " (%s%s)", (is_a_vowel(*t) ? "an " : "a "), t);

						modstr = tmp_val2;
					}
					else
					{
						sprintf(tmp_val2, "(%s)", t);

						modstr = t;
					}
#endif
				}
			}
			break;
		}
		//v1.1.60
		case TV_ANTIQUE:

			break;

		/* Figurines/Statues */
		case TV_FIGURINE:
		case TV_PHOTO:

			///del131221 ��
		//case TV_STATUE:
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];

#ifdef JP
			modstr = r_name + r_ptr->name;
#else
			cptr t = r_name + r_ptr->name;

			if (!(r_ptr->flags1 & RF1_UNIQUE))
			{
				sprintf(tmp_val2, "%s%s", (is_a_vowel(*t) ? "an " : "a "), t);

				modstr = tmp_val2;
			}
			else
			{
				modstr = t;
			}
#endif


			break;
		}


///del131221 ����

#if 0
		/* Corpses */
		case TV_CORPSE:
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];

			modstr = r_name + r_ptr->name;

#ifdef JP
			basenm = "#%";
#else
			if (r_ptr->flags1 & RF1_UNIQUE)
				basenm = "& % of #";
			else
				basenm = "& # %";
#endif

			break;
		}
#endif

		/* Missiles/ Bows/ Weapons */
		case TV_BULLET:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_CROSSBOW:
		case TV_GUN:
		case TV_KNIFE:
		case TV_SWORD:
		case TV_KATANA:
		case TV_HAMMER:
		case TV_STICK:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:		
		case TV_OTHERWEAPON:		
			{
			show_weapon = TRUE;
			break;
		}

		/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_HEAD:
		case TV_SHIELD:
		case TV_CLOTHES:
		case TV_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_MASK:
		{
			show_armour = TRUE;
			break;
		}

		/* Lites (including a few "Specials") */
		case TV_LITE:
		{
			break;
		}

		/* Amulets (including a few "Specials") */
		///item ���Ӓ莞�ƊӒ��ŕ\�L�̑S���Ⴄ�A�C�e���͂���Ȋ���
		case TV_AMULET:
		{
			/* Known artifacts */
			if (aware)
			{
				if (object_is_fixed_artifact(o_ptr)) break;
				if (k_ptr->gen_flags & TRG_INSTA_ART) break;
			}

			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;


			//v1.1.12 ����A�~�����b�g�ǉ��B�x�[�X���Ɂu�́v���t���Ȃ�
			if(o_ptr->sval == SV_AMULET_HAKUREI)
			{
				if (!flavor)    basenm = "%�A�~�����b�g";
				else if (aware) basenm = "%#�A�~�����b�g";
				else	basenm = "#�A�~�����b�g"; 
			}
			else
			{
				if (!flavor)    basenm = "%�̃A�~�����b�g";
				else if (aware) basenm = "%��#�A�~�����b�g";
				else            basenm = "#�A�~�����b�g";
			}


			break;
		}

		/* Rings (including a few "Specials") */
		case TV_RING:
		{
			/* Known artifacts */
			if (aware)
			{
				if (object_is_fixed_artifact(o_ptr)) break;
				if (k_ptr->gen_flags & TRG_INSTA_ART) break;
			}

			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%�̎w��";
			else if (aware) basenm = "%��#�w��";
			else            basenm = "#�w��";
#else
			if (!flavor)    basenm = "& Ring~ of %";
			else if (aware) basenm = "& # Ring~ of %";
			else            basenm = "& # Ring~";
#endif


			if (!k_ptr->to_h && !k_ptr->to_d && (o_ptr->to_h || o_ptr->to_d)) show_weapon = TRUE;



			break;
		}

		case TV_STAFF:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%�̏�";
			else if (aware) basenm = "%��#��";
			else            basenm = "#��";
#else
			if (!flavor)    basenm = "& Staff~ of %";
			else if (aware) basenm = "& # Staff~ of %";
			else            basenm = "& # Staff~";
#endif

			break;
		}

		case TV_WAND:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%�̖��@�_";
			else if (aware) basenm = "%��#���@�_";
			else            basenm = "#���@�_";
#else
			if (!flavor)    basenm = "& Wand~ of %";
			else if (aware) basenm = "& # Wand~ of %";
			else            basenm = "& # Wand~";
#endif

			break;
		}

		case TV_ROD:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%�̃��b�h";
			else if (aware) basenm = "%��#���b�h";
			else            basenm = "#���b�h";
#else
			if (!flavor)    basenm = "& Rod~ of %";
			else if (aware) basenm = "& # Rod~ of %";
			else            basenm = "& # Rod~";
#endif

			break;
		}

		case TV_SCROLL:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%�̊���";
			else if (aware) basenm = "�u#�v�Ə����ꂽ%�̊���";
			else            basenm = "�u#�v�Ə����ꂽ����";
#else
			if (!flavor)    basenm = "& Scroll~ of %";
			else if (aware) basenm = "& Scroll~ titled \"#\" of %";
			else            basenm = "& Scroll~ titled \"#\"";
#endif

			break;
		}

		case TV_POTION:
		{
			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%�̖�";
			else if (aware) basenm = "%��#��";
			else            basenm = "#��";

			//�Ȃɂ��������Ȃ��́u�́v������
			//������
			//�q�[���[�̖򁨍R�T��
			if(o_ptr->sval == SV_POTION_NANIKASUZUSHIKUNARU || o_ptr->sval == SV_POTION_LOVE || o_ptr->sval == SV_POTION_HEROISM)
			{
				if (!flavor)    basenm = "%��";
				else if (aware) basenm = "%#��";
				else	basenm = "#��"; 
			}
#else
			if (!flavor)    basenm = "& Potion~ of %";
			else if (aware) basenm = "& # Potion~ of %";
			else            basenm = "& # Potion~";
#endif

			break;
		}

		case TV_MUSHROOM:
		{
			/* Ordinary food is "boring" */
			if (!k_ptr->flavor_name) break;

			/* Color the object */
			modstr = k_name + flavor_k_ptr->flavor_name;

#ifdef JP
			if (!flavor)    basenm = "%�̃L�m�R";
			else if (aware) basenm = "%��#�L�m�R";
			else            basenm = "#�L�m�R";
#else
			if (!flavor)    basenm = "& Mushroom~ of %";
			else if (aware) basenm = "& # Mushroom~ of %";
			else            basenm = "& # Mushroom~";
#endif

			break;
		}
#if 0
		/* Magic Books */
		case TV_LIFE_BOOK:
		{
#ifdef JP
			basenm = "�����̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Life Magic %";
			else
				basenm = "& Life Spellbook~ %";
#endif

			break;
		}

		case TV_SORCERY_BOOK:
		{
#ifdef JP
			basenm = "��p�̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Sorcery %";
			else
				basenm = "& Sorcery Spellbook~ %";
#endif

			break;
		}

		case TV_NATURE_BOOK:
		{
#ifdef JP
			basenm = "���R�̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Nature Magic %";
			else
				basenm = "& Nature Spellbook~ %";
#endif

			break;
		}

		case TV_CHAOS_BOOK:
		{
#ifdef JP
			basenm = "�J�I�X�̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Chaos Magic %";
			else
				basenm = "& Chaos Spellbook~ %";
#endif

			break;
		}

		case TV_DEATH_BOOK:
		{
#ifdef JP
			basenm = "�Í��̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Death Magic %";
			else
				basenm = "& Death Spellbook~ %";
#endif

			break;
		}

		case TV_TRUMP_BOOK:
		{
#ifdef JP
			basenm = "�g�����v�̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Trump Magic %";
			else
				basenm = "& Trump Spellbook~ %";
#endif

			break;
		}

		case TV_ARCANE_BOOK:
		{
#ifdef JP
			basenm = "��p�̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Arcane Magic %";
			else
				basenm = "& Arcane Spellbook~ %";
#endif

			break;
		}

		case TV_CRAFT_BOOK:
		{
#ifdef JP
			basenm = "���̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Craft Magic %";
			else
				basenm = "& Craft Spellbook~ %";
#endif

			break;
		}

		case TV_DAEMON_BOOK:
		{
#ifdef JP
			basenm = "�����̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Daemon Magic %";
			else
				basenm = "& Daemon Spellbook~ %";
#endif

			break;
		}

		case TV_CRUSADE_BOOK:
		{
#ifdef JP
			basenm = "�j�ׂ̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Crusade Magic %";
			else
				basenm = "& Crusade Spellbook~ %";
#endif

			break;
		}

		case TV_MUSIC_BOOK:
		{
#ifdef JP
			basenm = "�̏W%";
#else
			basenm = "& Song Book~ %";
#endif

			break;
		}

		case TV_HISSATSU_BOOK:
		{
#ifdef JP
			basenm = "& ���|�̏�%";
#else
			basenm = "Book~ of Kendo %";
#endif

			break;
		}

		case TV_HEX_BOOK:
		{
#ifdef JP
			basenm = "��p�̖��@��%";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Crusade Magic %";
			else
				basenm = "& Crusade Spellbook~ %";
#endif

			break;
		}
#endif
		/* Hack -- Gold/Gems */
		case TV_GOLD:
		{
			strcpy(buf, basenm);
			return;
		}

		/* Used in the "inventory" routine */
		default:
		{
#ifdef JP
			strcpy(buf, "(�Ȃ�)");
#else
			strcpy(buf, "(nothing)");
#endif

			return;
		}
	}

	/* Use full name from k_info or a_info */
	if (aware && have_flag(flgs, TR_FULL_NAME))
	{
		if (known && o_ptr->name1) basenm = a_name + a_info[o_ptr->name1].name;
		else basenm = kindname;
	}

	/* Start dumping the result */
	t = tmp_val;

#ifdef JP
	if (basenm[0] == '&')
		s = basenm + 2;
	else
		s = basenm;

	/* No prefix */
	if (mode & OD_OMIT_PREFIX)
	{
		/* Nothing */
	}
	else if (o_ptr->number > 1)
	{
		t = object_desc_kosuu(t, o_ptr);
		t = object_desc_str(t, "�� ");
	}

	/* �p��̏ꍇ�A�[�e�B�t�@�N�g�� The ���t���̂ŕ����邪
	 * ���{��ł͕�����Ȃ��̂Ń}�[�N������ 
	 */
	 //v1.1.48 �����̍�����������ꂽ�A�C�e��
	if (p_ptr->pclass == CLASS_SHION && o_ptr->tval >= TV_EQUIP_BEGIN && o_ptr->tval <= TV_EQUIP_END && o_ptr->xtra5)
	{
		t = object_desc_str(t, "��������ꂽ");

	}
	/*:::Hack-���P����\�L*/
	if (known)
	{
		if (object_is_fixed_artifact(o_ptr)) t = object_desc_str(t, "��");
		//���_���̓��P�ƃx�[�X�A�C�e�������p������s��o���̂ŏd�ʂŋ��
		else if (o_ptr->tval == TV_STICK && o_ptr->sval == SV_WEAPON_KOGASA && o_ptr->weight == 30) t = object_desc_str(t, "��");
		else if (o_ptr->art_name)
		{
			if(o_ptr->tval == TV_MASK) 
				t = object_desc_str(t, "��");
			else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE); //�e�����̎ʐ^�ɂ́������Ȃ�
			else if (o_ptr->tval == TV_ABILITY_CARD); //v1.1.86 �A�r���e�B�J�[�h�ɂ��������Ȃ�
			else
				t = object_desc_str(t, "��");
		}
	}

#else

	/* The object "expects" a "number" */
	if (basenm[0] == '&')
	{
		/* Skip the ampersand (and space) */
		s = basenm + 2;

		/* No prefix */
		if (mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- None left */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Extract the number */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && object_is_artifact(o_ptr))

		//else if ((known && object_is_artifact(o_ptr)) ||
		        // ((o_ptr->tval == TV_CORPSE) &&
		        //  (r_info[o_ptr->pval].flags1 & RF1_UNIQUE)))

		{
			t = object_desc_str(t, "The ");
		}

		/* A single one */
		else
		{
			bool vowel;

			switch (*s)
			{
			case '#': vowel = is_a_vowel(modstr[0]); break;
			case '%': vowel = is_a_vowel(*kindname); break;
			default:  vowel = is_a_vowel(*s); break;
			}

			if (vowel)
			{
				/* A single one, with a vowel */
				t = object_desc_str(t, "an ");
			}
			else
			{
				/* A single one, without a vowel */
				t = object_desc_str(t, "a ");
			}
		}
	}

	/* Hack -- objects that "never" take an article */
	else
	{
		/* No ampersand */
		s = basenm;

		/* No pref */
		if (mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- all gone */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Prefix a number if required */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && object_is_artifact(o_ptr))
		{
			t = object_desc_str(t, "The ");
		}

		/* Hack -- single items get no prefix */
		else
		{
			/* Nothing */
		}
	}
#endif

	/* Paranoia -- skip illegal tildes */
	/* while (*s == '~') s++; */

#ifdef JP
	if (object_is_smith(o_ptr))
	{
		if(o_ptr->xtra3 == SPECIAL_ESSENCE_ONI)//����C���i
			t = object_desc_str(t, "��");
		else if(p_ptr->pclass == CLASS_TSUKUMO_MASTER)//�t�r�_�g���̗d��
		{
			if(o_ptr->xtra1 >= 100)	t = object_desc_str(t, "(*�d��*)");
			else	t = object_desc_str(t, "(�d��)");
		}
		else //�b��i
		//	t = object_desc_str(t, format("�b��t%s��", player_name));
			t = object_desc_str(t, format("��"));

	}

	/* �`���̃A�C�e���A���̂���A�C�e���̖��O��t������ */
	if (known)
	{
		/* �����_���E�A�[�e�B�t�@�N�g */
		if (o_ptr->art_name)
		{
			cptr temp = quark_str(o_ptr->art_name);

			/* '�w' ����n�܂�Ȃ��`���̃A�C�e���̖��O�͍ŏ��ɕt������ */
			/* �p��ł̃Z�[�u�t�@�C�����痈�� 'of XXX' ��,�uXXX�́v�ƕ\������ */
			///mod141206 ������̖ʂ͓���
			if ( o_ptr->tval == TV_MASK)
			{
				t = object_desc_str(t, temp);
			}
			else if (o_ptr->tval == TV_SPELLCARD)
			{
				;
			}
			else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE)//v1.1.51 �V�A���[�i�ʐ^
			{
				t = object_desc_str(t, "�e����");
				t = object_desc_str(t, temp);
				t = object_desc_str(t, "��");
			}
			else if (strncmp(temp, "of ", 3) == 0)
			{
				t = object_desc_str(t, &temp[3]);
				t = object_desc_str(t, "��");
			}
			else if ((strncmp(temp, "�w", 2) != 0) &&
				 (strncmp(temp, "�s", 2) != 0) &&
				(temp[0] != '\''))
				t = object_desc_str(t, temp);
		}
		/* �`���̃A�C�e�� */
		else if (o_ptr->name1 && !have_flag(flgs, TR_FULL_NAME))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];
			/* '�w' ����n�܂�Ȃ��`���̃A�C�e���̖��O�͍ŏ��ɕt������ */
			if (strncmp(a_name + a_ptr->name, "�w", 2) != 0)
			{
				t = object_desc_str(t, a_name + a_ptr->name);
			}
		}
		/* ���̂���A�C�e�� */
		else if (object_is_ego(o_ptr))
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];
			t = object_desc_str(t, e_name + e_ptr->name);
		}
	}
#endif

	/* Copy the string */
	for (s0 = NULL; *s || s0; )
	{
		/* The end of the flavour/kind string. */
		if (!*s)
		{
			s = s0 + 1;
			s0 = NULL;
		}

		/* Begin to append the modifier (flavor) */
		else if ((*s == '#') && !s0)
		{
			s0 = s;
			s = modstr;

			/* Paranoia -- Never append multiple modstrs */
			modstr = "";
		}

		/* Begin to append the kind name */
		else if ((*s == '%') && !s0)
		{
			s0 = s;
			s = kindname;

			/* Paranoia -- Never append multiple kindnames */
			kindname = "";
		}

#ifndef JP
		/* Pluralizer */
		else if (*s == '~')
		{
			/* Add a plural if needed */
			if (!(mode & OD_NO_PLURAL) && (o_ptr->number != 1))
			{
				char k = t[-1];

				/* XXX XXX XXX Mega-Hack */

				/* Hack -- "Cutlass-es" and "Torch-es" */
				if ((k == 's') || (k == 'h')) *t++ = 'e';

				/* Add an 's' */
				*t++ = 's';
			}
			s++;
		}
#endif

		/* Normal */
		else
		{
			/* Copy */
			*t++ = *s++;
		}
	}

	/* Terminate */
	*t = '\0';


#ifdef JP
	/* '�w'����n�܂�`���̃A�C�e���̖��O�͍Ō�ɕt������ */
	if (known)
	{
		/* �����_���A�[�e�B�t�@�N�g�̖��O�̓Z�[�u�t�@�C���ɋL�^
		   �����̂ŁA�p��ł̖��O������炵���ϊ����� */
		if (o_ptr->art_name)
		{
			char temp[256];
			int itemp;
			strcpy(temp, quark_str(o_ptr->art_name));


			if ( o_ptr->tval == TV_MASK)
			{
				//t = object_desc_str(t, temp);
			}
			else if (o_ptr->tval == TV_SPELLCARD)
			{
				t = object_desc_str(t, "�y");
				t = object_desc_str(t, temp);
				t = object_desc_str(t, "�z");

			}
			/* MEGA HACK by ita */
			else if (strncmp(temp, "�w", 2) == 0 ||
			    strncmp(temp, "�s", 2) == 0)
				t = object_desc_str(t, temp);
			else if (temp[0] == '\'')
			{
				itemp = strlen(temp);
				temp[itemp - 1] = 0;
				t = object_desc_str(t, "�w");
				t = object_desc_str(t, &temp[1]);
				t = object_desc_str(t, "�x");
			}
		}
		else if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];
			if (strncmp(a_name + a_ptr->name, "�w", 2) == 0)
			{
				t = object_desc_str(t, a_name + a_ptr->name);
			}
		}
		else if (o_ptr->inscription)
		{
			cptr str = quark_str(o_ptr->inscription);

			while(*str)
			{
				if (iskanji(*str))
				{
					str += 2;
					continue;
				}
				if (*str == '#') break;
				str++;
			}
			if (*str)
			{
				/* Find the '#' */
				cptr str = my_strchr(quark_str(o_ptr->inscription), '#');

				/* Add the false name */
				t = object_desc_str(t,"�w");
				t = object_desc_str(t, &str[1]);
				t = object_desc_str(t,"�x");
			}
		}
	}




	//v1.1.85 �d���{�ȊO�̃����X�^�[�ߊl�A�C�e��
	if (activation_index(o_ptr) == ACT_ART_CAPTURE)
	{
		monster_race *r_ptr = &r_info[o_ptr->xtra6];
		if (known)
		{
			if (!o_ptr->xtra6)
			{
				t = object_desc_str(t, "(��)");
			}
			//v1.1.16 �d���{�o�O�ɂ��Z�[�u�t�@�C���ǂݍ��ݕs�\�ɑΉ�
			else if (o_ptr->xtra6 < 0 || o_ptr->xtra6 >= max_r_idx)
			{
				sprintf(tmp_val2, " (ERROR:�A�C�e������r_idx�l���s��(%d))", o_ptr->xtra6);
				t = object_desc_str(t, tmp_val2);

			}
			else
			{
				sprintf(tmp_val2, " (%s)", r_name + r_ptr->name);
				t = object_desc_str(t, tmp_val2);

			}
		}
	}


#else
	if (object_is_smith(o_ptr))
	{
		t = object_desc_str(t,format(" of %s the Smith",player_name));
	}

	/* Hack -- Append "Artifact" or "Special" names */
	if (known && !have_flag(flgs, TR_FULL_NAME))
	{
		/* Is it a new random artifact ? */
		if (o_ptr->art_name)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, quark_str(o_ptr->art_name));
		}

		/* Grab any artifact name */
		else if (object_is_fixed_artifact(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, a_name + a_ptr->name);
		}

		/* Grab any ego-item name */
		else
		{
			if (object_is_ego(o_ptr))
			{
				ego_item_type *e_ptr = &e_info[o_ptr->name2];

				t = object_desc_chr(t, ' ');
				t = object_desc_str(t, e_name + e_ptr->name);
			}

			if (o_ptr->inscription && my_strchr(quark_str(o_ptr->inscription), '#'))
			{
				/* Find the '#' */
				cptr str = my_strchr(quark_str(o_ptr->inscription), '#');

				/* Add the false name */
				t = object_desc_chr(t, ' ');
				t = object_desc_str(t, &str[1]);
			}
		}
	}
#endif


	/* No more details wanted */
	if (mode & OD_NAME_ONLY) goto object_desc_done;
	//�X�y�J��pval�Ƃ��E�C�C���͕\�����Ȃ�
	if(o_ptr->tval == TV_SPELLCARD) 
		goto object_desc_done;

	//������W�A�C�e����pval�Ƃ��E�C�C���͕\�����Ȃ�.�Ō�ɉ��i�����\������
	if (o_ptr->tval == TV_ANTIQUE)
	{

		sprintf(tmp_val2, " ($%d)", o_ptr->score_value);

		t = object_desc_str(t, tmp_val2);
		goto object_desc_done;
	}

	///mod151031 �󕨌ɂ̃^�[�Q�b�g�A�C�e���͖��O�̂ݕ\���ɂ���
	if(object_is_quest_target(o_ptr) && !known)
	{
		goto object_desc_done;
	}


	/* Hack -- Chests must be described in detail */
	if (o_ptr->tval == TV_CHEST)
	{
		/* Not searched yet */
		if (!known)
		{
			/* Nothing */
		}

		/* May be "empty" */
		else if (!o_ptr->pval)
		{
#ifdef JP
			t = object_desc_str(t, "(��)");
#else
			t = object_desc_str(t, " (empty)");
#endif
		}

		/* May be "disarmed" */
		else if (o_ptr->pval < 0)
		{
			if (chest_traps[0 - o_ptr->pval])
			{
#ifdef JP
				t = object_desc_str(t, "(������)");
#else
				t = object_desc_str(t, " (disarmed)");
#endif
			}
			else
			{
#ifdef JP
				t = object_desc_str(t, "(��{��)");
#else
				t = object_desc_str(t, " (unlocked)");
#endif
			}
		}
		else if (o_ptr->pval >= CHEST_TRAP_LIST_LENGTH)
		{
			t = object_desc_str(t, "(ERROR)");
		}
		/* Describe the traps, if any */
		else
		{
			/* Describe the traps */
			//v1.1.97 ���g���b�v���e����V

//			switch (chest_traps[o_ptr->pval])
			switch (chest_new_traps[o_ptr->pval])
			{


				case CHEST_TRAP_NOTHING:
				{
#ifdef JP
					t = object_desc_str(t, "(�{��)");
#else
					t = object_desc_str(t, " (Locked)");
#endif
					break;
				}
				case CHEST_TRAP_LOSE_STR:
					t = object_desc_str(t, "(�r�͒ቺ�_�[�c)");
					break;
				case CHEST_TRAP_LOSE_CON:
					t = object_desc_str(t, "(�ϋv�ቺ�_�[�c)");
					break;
				case CHEST_TRAP_LOSE_MAG:
					t = object_desc_str(t, "(�m�\�ቺ�_�[�c)");
					break;


				case CHEST_TRAP_BA_POIS:
					t = object_desc_str(t, "(�ŃK�X)");
					break;
				case CHEST_TRAP_BA_SLEEP:
					t = object_desc_str(t, "(�����K�X)");
					break;
				case CHEST_TRAP_BA_CONF:
					t = object_desc_str(t, "(�����K�X)");
					break;
				case CHEST_TRAP_EXPLODE:
					t = object_desc_str(t, "(���e)");
					break;
				case CHEST_TRAP_ALARM:
					t = object_desc_str(t, "(�A���[��)");
					break;
				case CHEST_TRAP_SUMMON:
					t = object_desc_str(t, "(�����̃��[��)");
					break;
				case CHEST_TRAP_S_BIRD:
					t = object_desc_str(t, "(�������̃��[��)");
					break;

				case CHEST_TRAP_S_ELEMENTAL:
					t = object_desc_str(t, "(���쏢���̃��[��)");
					break;
				case CHEST_TRAP_S_DEMON:
					t = object_desc_str(t, "(���������̃��[��)");
					break;

				case CHEST_TRAP_S_DRAGON:
					t = object_desc_str(t, "(�������̃��[��)");
					break;
				case CHEST_TRAP_S_CHIMERA:
					t = object_desc_str(t, "(���b�����̃��[��)");
					break;
				case CHEST_TRAP_S_VORTEX:
					t = object_desc_str(t, "(���Q�����̃��[��)");
					break;
				case CHEST_TRAP_S_KWAI:
					t = object_desc_str(t, "(�d�������̃��[��)");
					break;

				case CHEST_TRAP_SUIKI:
					t = object_desc_str(t, "(���S�S�_�����)");
					break;

				case CHEST_TRAP_RUIN:
					t = object_desc_str(t, "(�j�ł̃g���b�v)");
					break;
				case CHEST_TRAP_SLINGSHOT:
					t = object_desc_str(t, "(�΂Ԃ�)");
					break;
				case CHEST_TRAP_ARROW:
					t = object_desc_str(t, "(��)");
					break;
				case CHEST_TRAP_STEEL_ARROW:
					t = object_desc_str(t, "(�|�S�̖�)");
					break;
				case CHEST_TRAP_TELEPORTER:
					t = object_desc_str(t, "(�e���|�[�^�[)");
					break;
				case CHEST_TRAP_PUNCH:
					t = object_desc_str(t, "(�p���`)");
					break;
				case CHEST_TRAP_BR_FIRE:
					t = object_desc_str(t, "(�Ή�����)");
					break;
				case CHEST_TRAP_BR_ACID:
					t = object_desc_str(t, "(�_�̕���)");
					break;
				case CHEST_TRAP_BA_TIME:
					t = object_desc_str(t, "(�t�ʎ蔠)");
					break;
				case CHEST_TRAP_MIMIC:
					t = object_desc_str(t, "(�~�~�b�N)");
					break;
				case CHEST_TRAP_MAGIC_DRAIN:
					t = object_desc_str(t, "(���͋z��)");
					break;
				case CHEST_TRAP_BA_BERSERK:
					t = object_desc_str(t, "(�����K�X)");
					break;
				case CHEST_TRAP_FUSION:
					t = object_desc_str(t, "(�t���[�W����)");
					break;

				default:
				{
					t = object_desc_str(t, "(ERROR)");
					break;
				}

#if 0

				case CHEST_LOSE_STR:
				{
#ifdef JP
					t = object_desc_str(t, "(�Őj)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_LOSE_CON:
				{
#ifdef JP
					t = object_desc_str(t, "(�Őj)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_POISON:
				{
#ifdef JP
					t = object_desc_str(t, "(�K�X�E�g���b�v)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_PARALYZE:
				{
#ifdef JP
					t = object_desc_str(t, "(�K�X�E�g���b�v)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_EXPLODE:
				{
#ifdef JP
					t = object_desc_str(t, "(�������u)");
#else
					t = object_desc_str(t, " (Explosion Device)");
#endif
					break;
				}
				case CHEST_SUMMON:
				case CHEST_BIRD_STORM:
				case CHEST_E_SUMMON:
				case CHEST_H_SUMMON:
				{
#ifdef JP
					t = object_desc_str(t, "(�����̃��[��)");
#else
					t = object_desc_str(t, " (Summoning Runes)");
#endif
					break;
				}
				case CHEST_RUNES_OF_EVIL:
				{
#ifdef JP
					t = object_desc_str(t, "(�׈��ȃ��[��)");
#else
					t = object_desc_str(t, " (Gleaming Black Runes)");
#endif
					break;
				}
				case CHEST_ALARM:
				{
#ifdef JP
					t = object_desc_str(t, "(�x�񑕒u)");
#else
					t = object_desc_str(t, " (Alarm)");
#endif
					break;
				}
				default:
				{
#ifdef JP
					t = object_desc_str(t, "(�}���`�E�g���b�v)");
#else
					t = object_desc_str(t, " (Multiple Traps)");
#endif
					break;
				}
#endif

			}
		}
	}
	else if(o_ptr->tval == TV_BUNBUN)
	{
		if(show_special_info && (o_ptr->ident & IDENT_MENTAL))
			t = object_desc_str(t, format("�E���O�s����%04d�E%s�t",o_ptr->xtra5,r_name+r_info[o_ptr->pval].name));
		else
			t = object_desc_str(t, format("�E���O�s����%04d�t",o_ptr->xtra5));
	}
	else if(o_ptr->tval == TV_KAKASHI)
	{
		if(show_special_info && (o_ptr->ident & IDENT_MENTAL))
			t = object_desc_str(t, format("�E���ʓ��W��%04d�E%s",o_ptr->xtra5,r_name+r_info[o_ptr->pval].name));
		else
			t = object_desc_str(t, format("�E���ʓ��W��%04d",o_ptr->xtra5));
	}
	else if(o_ptr->tval == TV_ITEMCARD)
	{
		t = object_desc_str(t, format("�w%s�x",support_item_list[o_ptr->pval].name));
	}
	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
		{
			t = object_desc_str(t, format("(ERROR:idx%d)", o_ptr->pval));
		}
		else
		{
			t = object_desc_str(t, format("�w%s�x", ability_card_list[o_ptr->pval].card_name));
		}
	}

	else if (o_ptr->tval == TV_PHOTO && o_ptr->sval == SV_PHOTO_NIGHTMARE)//v1.1.51 �V�A���[�i�ʐ^ �X�R�A����
	{
		if (known)
		{
			char tmp_buf[32];

			int points = o_ptr->xtra4 * 10000 + o_ptr->xtra5;
			sprintf(tmp_buf, "(%d points)", points);
			t = object_desc_str(t, tmp_buf);
		}
	}


	/* Display the item like a weapon */
	if (have_flag(flgs, TR_SHOW_MODS)) show_weapon = TRUE;

	/* Display the item like a weapon */
	if (object_is_smith(o_ptr) && (o_ptr->xtra3 == 1 + ESSENCE_SLAY_GLOVE))
		show_weapon = TRUE;

	/* Display the item like a weapon */
	if (o_ptr->to_h && o_ptr->to_d) show_weapon = TRUE;

	/* Display the item like armour */
	if (o_ptr->ac) show_armour = TRUE;


	/* Dump base weapon info */
	switch (o_ptr->tval)
	{
		/* Missiles and Weapons */
		case TV_BULLET:
		case TV_BOLT:
		case TV_ARROW:
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
		case TV_GUN:

		/* In Vault Quest, hide the dice of target weapon. */
			/*:::�󕨌ɂł̓_�C�X��\�����Ȃ�*/
		///mod151031 �󕨌ɍĎ����@���O�����\�����Ȃ��悤�ɂ��邽�ߕʂ̏ꏊ��
		/*
		if(object_is_quest_target(o_ptr) && !known)
		{
			break;
		}
		*/
		///mod160505 ���Ӓ�̏e�̓_�C�X���\������Ȃ�
		//if(o_ptr->tval == TV_GUN && !known) break;

		/* Append a "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);

		if(mode & OD_WEAPON_RARM) t = object_desc_num(t, o_ptr->dd + p_ptr->to_dd[0]);
		else if(mode & OD_WEAPON_LARM) t = object_desc_num(t, o_ptr->dd + p_ptr->to_dd[1]);
		else 
			t = object_desc_num(t, o_ptr->dd);
		t = object_desc_chr(t, 'd');
		if(mode & OD_WEAPON_RARM) t = object_desc_num(t, o_ptr->ds + p_ptr->to_ds[0]);
		else if(mode & OD_WEAPON_LARM) t = object_desc_num(t, o_ptr->ds + p_ptr->to_ds[1]);
		else 
			t = object_desc_num(t, o_ptr->ds);
		t = object_desc_chr(t, p2);

		/* All done */
		break;


		/* Bows get a special "damage string" */
		case TV_BOW:
		case TV_CROSSBOW:
		//case TV_GUN:

		/* Mega-Hack -- Extract the "base power" */
		power = bow_tmul(o_ptr->tval, o_ptr->sval);

		/* Apply the "Extra Might" flag */
		if (have_flag(flgs, TR_XTRA_MIGHT)) power++;

		/* Append a special "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_chr(t, 'x');
		t = object_desc_num(t, power);
		t = object_desc_chr(t, p2);

		/* All done */
		break;
	}


	/* Add the weapon bonuses */
	if (known)
	{
		/* Show the tohit/todam on request */
		if (show_weapon)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}

		/* Show the tohit if needed */
		else if (o_ptr->to_h)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, p2);
		}

		/* Show the todam if needed */
		else if (o_ptr->to_d)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}
	}

	//bow_ptr = &inventory[INVEN_BOW];
	bow_ptr = &inventory[INVEN_PACK + shootable(&dummy)];

	/* If have a firing weapon + ammo matches bow */
	/*:::��ʂ̊��҃_���[�W*/
	//if (bow_ptr->k_idx && (o_ptr->tval == p_ptr->tval_ammo))
	if (dummy > SHOOT_UNSUITABLE && bow_ptr->k_idx && (o_ptr->tval == p_ptr->tval_ammo))
	{
		int avgdam = o_ptr->dd * (o_ptr->ds + 1) * 10 / 2;
		int tmul = bow_tmul(bow_ptr->tval, bow_ptr->sval);
		s16b energy_fire = bow_energy(bow_ptr->tval, bow_ptr->sval);

		/* See if the bow is "known" - then set damage bonus */
		if (object_is_known(bow_ptr)) avgdam += (bow_ptr->to_d * 10);

		/* Effect of ammo */
		if (known) avgdam += (o_ptr->to_d * 10);

		///mod141018 ��ɗ��͂��t���Ă���Ƃ��̃_���[�W��������
		if (have_flag(flgs, TR_FORCE_WEAPON) && (p_ptr->csp >=10))
			avgdam = avgdam * 5 / 2;

		/* Get extra "power" from "extra might" */
		if (p_ptr->xtra_might) tmul++;


		tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);

		/* Launcher multiplier */
		avgdam *= tmul;
		avgdam /= (100 * 10);

		/* Get extra damage from concentration */
		if ((p_ptr->pclass == CLASS_SNIPER) && p_ptr->concent) avgdam = boost_concentration_damage(avgdam);

		if (avgdam < 0) avgdam = 0;

		/* Display (shot damage/ avg damage) */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		if (p_ptr->num_fire == 0)
		{
			t = object_desc_chr(t, '0');
		}
		else
		{
			/* Calc effects of energy */
			avgdam *= (p_ptr->num_fire * 100);
			avgdam /= energy_fire;
			t = object_desc_num(t, avgdam);
		}

		t = object_desc_chr(t, p2);
	}

#if 0
	/*:::�j���W���̂����ѓ����_���[�W*/
	///sysdel item
	else if ((p_ptr->pclass == CLASS_NINJA) && (o_ptr->tval == TV_SPIKE))
	{
		int avgdam = p_ptr->mighty_throw ? (1 + 3) : 1;
		s16b energy_fire = 100 - p_ptr->lev;

		avgdam += ((p_ptr->lev + 30) * (p_ptr->lev + 30) - 900) / 55;

		/* Display (shot damage/ avg damage) */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		/* Calc effects of energy */
		avgdam = 100 * avgdam / energy_fire;

		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, p2);
	}
#endif
	/* Add the armor bonuses */
	if (known)
	{
		/* Show the armor class info */
		if (show_armour)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_num(t, o_ptr->ac);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}

		/* No base armor, but does increase armor */
		else if (o_ptr->to_a)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}
	}

	/* Hack -- always show base armor */
	else if (show_armour)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, b1);
		t = object_desc_num(t, o_ptr->ac);
		t = object_desc_chr(t, b2);
	}


	/* No more details wanted */
	if (mode & OD_NAME_AND_ENCHANT) goto object_desc_done;


	if (known) /* Known item only */
	{
		/*
		 * Hack -- Wands and Staffs have charges.  Make certain how many charges
		 * a stack of staffs really has is clear. -LM-
		 */
		/*:::��Ɩ��@�_�̉�*/
		if (((o_ptr->tval == TV_STAFF) || (o_ptr->tval == TV_WAND)))
		{
			/* Dump " (N charges)" */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Clear explaination for staffs. */
			if ((o_ptr->tval == TV_STAFF) && (o_ptr->number > 1))
			{
				t = object_desc_num(t, o_ptr->number);
				t = object_desc_str(t, "x ");
			}
			t = object_desc_num(t, o_ptr->pval);
#ifdef JP
			t = object_desc_str(t, "��");
#else
			t = object_desc_str(t, " charge");
			if (o_ptr->pval != 1) t = object_desc_chr(t, 's');
#endif

			t = object_desc_chr(t, p2);
		}
		/* Hack -- Rods have a "charging" indicator.  Now that stacks of rods may
		 * be in any state of charge or discharge, this now includes a number. -LM-
		 */
		/*:::���b�h�̏[�U*/
		else if (o_ptr->tval == TV_ROD)
		{
			/* Hack -- Dump " (# charging)" if relevant */
			if (o_ptr->timeout)
			{
				/* Stacks of rods display an exact count of charging rods. */
				if (o_ptr->number > 1)
				{
					/* Paranoia. */
					if (k_ptr->pval == 0) k_ptr->pval = 1;

					/* Find out how many rods are charging, by dividing
					 * current timeout by each rod's maximum timeout.
					 * Ensure that any remainder is rounded up.  Display
					 * very discharged stacks as merely fully discharged.
					 */
					power = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
					if (power > o_ptr->number) power = o_ptr->number;

					/* Display prettily. */
					t = object_desc_str(t, " (");
					t = object_desc_num(t, power);
#ifdef JP
					t = object_desc_str(t, "�{ �[�U��)");
#else
					t = object_desc_str(t, " charging)");
#endif
				}

				/* "one Rod of Perception (1 charging)" would look tacky. */
				else
				{
#ifdef JP
					t = object_desc_str(t, "(�[�U��)");
#else
					t = object_desc_str(t, " (charging)");
#endif
				}
			}
		}
		//v1.1.86
		else if (o_ptr->tval == TV_ABILITY_CARD)
		{
			if (o_ptr->timeout)
			{
				if (o_ptr->number > 1)
				{
					int base_charge_turn = ability_card_list[o_ptr->pval].charge_turn;

					power = (o_ptr->timeout + (base_charge_turn - 1)) / base_charge_turn;
					if (power > o_ptr->number) power = o_ptr->number;

					/* Display prettily. */
					t = object_desc_str(t, " (");
					t = object_desc_num(t, power);
					t = object_desc_str(t, "�� �[�U��)");
				}

				else
				{
					t = object_desc_str(t, "(�[�U��)");
				}
			}
		}

		/*:::pval�\���@�����Ȃǂ�*/
		/* Dump "pval" flags for wearable items */
		if (have_pval_flags(flgs))
		{
			/* Start the display */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Dump the "pval" itself */
			t = object_desc_int(t, o_ptr->pval);

			/* Do not display the "pval" flags */
			if (have_flag(flgs, TR_HIDE_TYPE))
			{
				/* Nothing */
			}

			/* Speed */
			else if (have_flag(flgs, TR_SPEED))
			{
				/* Dump " to speed" */
#ifdef JP
				t = object_desc_str(t, "����");
#else
				t = object_desc_str(t, " to speed");
#endif
			}

			/* Attack speed */
			else if (have_flag(flgs, TR_BLOWS))
			{
				/* Add " attack" */
#ifdef JP
				t = object_desc_str(t, "�U��");
#else
				t = object_desc_str(t, " attack");

				/* Add "attacks" */
				if (ABS(o_ptr->pval) != 1) t = object_desc_chr(t, 's');
#endif
			}

			/* Stealth */
			else if (have_flag(flgs, TR_STEALTH))
			{
				/* Dump " to stealth" */
#ifdef JP
				t = object_desc_str(t, "�B��");
#else
				t = object_desc_str(t, " to stealth");
#endif
			}

			/* Search */
			else if (have_flag(flgs, TR_SEARCH))
			{
				/* Dump " to searching" */
#ifdef JP
				t = object_desc_str(t, "�T��");
#else
				t = object_desc_str(t, " to searching");
#endif
			}

			/* Infravision */
			///item �ԊO�����͔p�~����Ȃ�
			else if (have_flag(flgs, TR_INFRA))
			{
				/* Dump " to infravision" */
#ifdef JP
				t = object_desc_str(t, "�ԊO������");
#else
				t = object_desc_str(t, " to infravision");
#endif
			}

			/* Finish the display */
			t = object_desc_chr(t, p2);
		}

		///mod140930 �������_���[�W�\�L�ǉ�
		t = object_desc_str(t, print_throw_dam(o_ptr));

		/* Hack -- Process Lanterns/Torches */
		//if ((o_ptr->tval == TV_LITE) && (!(object_is_fixed_artifact(o_ptr) || (o_ptr->sval == SV_LITE_FEANOR))))
		//v1.1.15�@�����O��
		if ((o_ptr->tval == TV_LITE) && (!(object_is_artifact(o_ptr) || (o_ptr->sval == SV_LITE_FEANOR))))
		{
			/* Hack -- Turns of light for normal lites */
#ifdef JP
			t = object_desc_chr(t, '(');
#else
			t = object_desc_str(t, " (with ");
#endif

			if (o_ptr->name2 == EGO_LITE_LONG) t = object_desc_num(t, o_ptr->xtra4 * 2);
			else t = object_desc_num(t, o_ptr->xtra4);
#ifdef JP
			t = object_desc_str(t, "�^�[���̎���)");
#else
			t = object_desc_str(t, " turns of light)");
#endif
		}

		if(o_ptr->tval == TV_GUN)
		{
			int timeout_base = calc_gun_timeout(o_ptr) * 1000;
			int bullets = calc_gun_load_num(o_ptr);

			t = object_desc_str(t, format("(%d/%d)",(timeout_base * bullets - o_ptr->timeout) / timeout_base,bullets));
		}
		/* Indicate charging objects, but not rods. */
		else if (o_ptr->timeout && (o_ptr->tval != TV_ROD && o_ptr->tval != TV_ABILITY_CARD))
		{
			t = object_desc_str(t, "(�[�U��)");
		}
	}


	/* No more details wanted */
	/*:::���̊֐��̍Ōォ���s�ڂ܂ōs���Ă���*/
	if (mode & OD_OMIT_INSCRIPTION) goto object_desc_done;


	/* Prepare real inscriptions in a buffer */
	tmp_val2[0] = '\0';

	/* Auto abbreviation inscribe */
	if ((abbrev_extra || abbrev_all) && (o_ptr->ident & IDENT_MENTAL))
	{
		if (!o_ptr->inscription || !my_strchr(quark_str(o_ptr->inscription), '%'))
		{
			bool kanji, all;

#ifdef JP
			kanji = TRUE;
#else
			kanji = FALSE;
#endif
			all = abbrev_all;

			get_ability_abbreviation(tmp_val2, o_ptr, kanji, all);
		}
	}

	/* Use the standard inscription if available */
	if (o_ptr->inscription)
	{
		char buff[1024];

		if (tmp_val2[0]) strcat(tmp_val2, ", ");

		/* Get inscription and convert {%} */
		get_inscription(buff, o_ptr);

		/* strcat with correct treating of kanji */
		my_strcat(tmp_val2, buff, sizeof(tmp_val2));
	}


	/* No fake inscription yet */
	fake_insc_buf[0] = '\0';

	/* Use the game-generated "feeling" otherwise, if available */
	if (o_ptr->feeling)
	{
		strcpy(fake_insc_buf, game_inscriptions[o_ptr->feeling]);
	}

	/* Note "cursed" if the item is known to be cursed */
	else if (object_is_cursed(o_ptr) && (known || (o_ptr->ident & IDENT_SENSE)))
	{
#ifdef JP
		strcpy(fake_insc_buf, "����Ă���");
#else
		strcpy(fake_insc_buf, "cursed");
#endif
	}

	/* Note "unidentified" if the item is unidentified */
	else if (((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)
		   || (o_ptr->tval == TV_LITE) || (o_ptr->tval == TV_FIGURINE))
		 && aware && !known
		 && !(o_ptr->ident & IDENT_SENSE))
	{
#ifdef JP
		strcpy(fake_insc_buf, "���Ӓ�");
#else
		strcpy(fake_insc_buf, "unidentified");
#endif
	}

	/* Mega-Hack -- note empty wands/staffs */
	else if (!known && (o_ptr->ident & IDENT_EMPTY))
	{
#ifdef JP
		strcpy(fake_insc_buf, "��");
#else
		strcpy(fake_insc_buf, "empty");
#endif
	}

	/* Note "tried" if the object has been tested unsuccessfully */
	else if (!aware && object_is_tried(o_ptr))
	{
#ifdef JP
		strcpy(fake_insc_buf, "������");
#else
		strcpy(fake_insc_buf, "tried");
#endif
	}

	/* Note the discount, if any */
	//������̖ʂ͓G���x����discount�Ɋi�[���Ă���̂Ŋ�������\�������Ȃ�
	if (o_ptr->discount && o_ptr->tval != TV_MASK)
	{
		/* Hidden by real inscription unless in a store */
		if (!tmp_val2[0] || (o_ptr->ident & IDENT_STORE))
		{
			char discount_num_buf[4];

			/* Append to other fake inscriptions if any */
			if (fake_insc_buf[0]) strcat(fake_insc_buf, ", ");

			(void)object_desc_num(discount_num_buf, o_ptr->discount);
			strcat(fake_insc_buf, discount_num_buf);
#ifdef JP
			strcat(fake_insc_buf, "%����");
#else
			strcat(fake_insc_buf, "% off");
#endif
		}
	}


	/* Append the inscription, if any */
	if (fake_insc_buf[0] || tmp_val2[0])
	{
		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);

		/* Append fake inscriptions */
		if (fake_insc_buf[0])
		{
			t = object_desc_str(t, fake_insc_buf);
		}

		/* Append a separater */
		if (fake_insc_buf[0] && tmp_val2[0])
		{
			t = object_desc_chr(t, ',');
			t = object_desc_chr(t, ' ');
		}

		/* Append real inscriptions */
		if (tmp_val2[0])
		{
			t = object_desc_str(t, tmp_val2);
		}

		t = object_desc_chr(t, c2);
	}

object_desc_done:
	my_strcpy(buf, tmp_val, MAX_NLEN);
}



// v1.1.51 �V�A���[�i
//�B����A�|���������X�^�[�����̃X�y�J�����q�����t���[�o�[�A�C�e���u�e�����y�����z�̎ʐ^�v�𐶐�����
void	make_nightmare_diary_photo(byte x, byte y)
{
	object_type forge;
	object_type *q_ptr = &forge;
	char name_buf[255];
	char *t = name_buf;

	int insc_idx_table[NIGHTMARE_DIARY_MONSTER_MAX];//nightmare_spellname_table[]�̓Y����5�܂őI��
	int insc_idx_num;//���ۂɁ��ɋL�^���ꂽ��

	int i,j;
	int try_count;

	//�K�؂Ȗ��O�����܂�܂ōő�100��̎��s
	for (try_count = 100; try_count; try_count--)
	{
		int cur_length = 0;
		bool flag_last_word = FALSE; //�X�y�J���̖����Ɏg���镶���񂪊��ɑI�΂ꂽ�t���O
		bool flag_pos0_word = FALSE; //�X�y�J���̖����Ɏg��Ȃ���΂����Ȃ������񂪊��ɑI�΂ꂽ�t���O

		//�O�̃��[�v�̒l���������߂̏�����
		insc_idx_num = 0;
		for (i = 0; i < NIGHTMARE_DIARY_MONSTER_MAX; i++) insc_idx_table[i] = 0;

		//����������X�^�[�����[�v
		for (i = 0; i < NIGHTMARE_DIARY_MONSTER_MAX; i++)
		{
			int tmp_insc_idx = 0;
			int r_idx = nightmare_mon_r_idx[i];//�V�A���[�i�������ɋL�^�����A���胂���X�^�[��r_idx
			int candidate_count = 0;

			if (!r_idx) break;

			//��������e�[�u�����I�[�_�~�[�܂Ń��[�v
			for (j = 0; nightmare_spellname_table[j].r_idx; j++)
			{
				bool flag_ok = FALSE;

				//�����X�^�[�������_�����j�[�N1(�����ł͖��̐��E�́�)�̏ꍇ�A���������class_idx�����̐E�Ƃƈ�v���Ă���Έ�v�Ƃ݂Ȃ�
				//�������j�[�N�N���X�łȂ��Ƃ��͕�����I��̑ΏۂƂ��Ȃ�
				if (r_idx == MON_RANDOM_UNIQUE_1)
				{
					if (cp_ptr->flag_only_unique)
					{
						if (p_ptr->pclass == nightmare_spellname_table[j].class_idx) flag_ok = TRUE;
					}
					else break;
				}
				//����ȊO�̂Ƃ���r_idx���m�̈�v�Ŕ���
				else
				{
					if (r_idx == nightmare_spellname_table[j].r_idx) flag_ok = TRUE;
				}

				//���łɁu�����Ɏg��Ȃ���΂����Ȃ������v���I�΂�Ă���ꍇ���l�̕�����I�Ԃ��Ƃ͂ł��Ȃ�
				if (flag_pos0_word && !nightmare_spellname_table[j].position) flag_ok = FALSE;

				//�����Ɉ�v�����������idx���ꎞ�I�ɋL��
				if (flag_ok)
				{
					candidate_count++;

					if (one_in_(candidate_count)) tmp_insc_idx = j;

				}
			}
			if (!candidate_count)
			{
				if (r_idx != MON_RANDOM_UNIQUE_1 || cp_ptr->flag_only_unique)
				{
					msg_format("WARNING:idx(%d)�̒e���ʐ^�����ݒ肳��Ă��Ȃ�", r_idx);
				}

				continue;
			}
			//���̃����X�^�[�ɑΉ����镶���񂪌��܂����̂ňꎞ�I�̈悩�琶���p�z��Ɉڂ�
			insc_idx_table[insc_idx_num] = tmp_insc_idx;
			insc_idx_num++;

			cur_length += strlen(nightmare_spellname_table[tmp_insc_idx].phrase);

			if (nightmare_spellname_table[tmp_insc_idx].position < 10) flag_last_word = TRUE;
			if (nightmare_spellname_table[tmp_insc_idx].position == 0) flag_pos0_word = TRUE;


		}
		if (!i)
		{
			msg_print("ERROR:nightmare_mon_r_idx[]����ł���");
			return;
		}
		if (insc_idx_num < 2)
		{
			msg_format("ERROR:nightmare_mon_r_idx[%d,%d,%d,%d,%d]�ɑΉ����镶����f�[�^���s�����Ă���", nightmare_mon_r_idx[0], nightmare_mon_r_idx[1], nightmare_mon_r_idx[2], nightmare_mon_r_idx[3], nightmare_mon_r_idx[4]);
			return;
		}

		//�����X�y�J�����ڑ��������őS�p25�������������蒼��
		if (cur_length > 50) continue;

		//�����X�y�J���̖����Ɏg���镶���񂪂ЂƂ��I�肳��Ȃ��������蒼��
		if (!flag_last_word) continue;


		break;
	}

	if (!try_count)
	{
		msg_format("ERROR:idx[%d,%d,%d,%d,%d]��100��J��Ԃ��Ă��X�y�J�������܂�Ȃ�����", nightmare_mon_r_idx[0], nightmare_mon_r_idx[1], nightmare_mon_r_idx[2], nightmare_mon_r_idx[3], nightmare_mon_r_idx[4]);
		return;

	}

	if (cheat_xtra)
	{
		msg_format("r_idx[%d,%d,%d,%d,%d]", nightmare_mon_r_idx[0], nightmare_mon_r_idx[1], nightmare_mon_r_idx[2], nightmare_mon_r_idx[3], nightmare_mon_r_idx[4]);
		msg_format("insc_idx_num:%d", insc_idx_num);
		msg_format("insc_idx[%d,%d,%d,%d,%d]", insc_idx_table[0], insc_idx_table[1], insc_idx_table[2], insc_idx_table[3], insc_idx_table[4]);
	}

	//�I�΂ꂽ�X�y�J�����iidx��D��x�ɏ]�����בւ��B�K���ɂ��������������Z���N�V�����\�[�g�ɂȂ������ǂ����v�f��������5�Ȃ̂Ŗ��Ȃ�
	for (i = 0; i < insc_idx_num-1; i++)
	{
		for (j = i+1; j < insc_idx_num; j++)
		{
			if (nightmare_spellname_table[insc_idx_table[i]].position < nightmare_spellname_table[insc_idx_table[j]].position)
			{
				int swap;
				swap = insc_idx_table[i];
				insc_idx_table[i] = insc_idx_table[j];
				insc_idx_table[j] = swap;
			}
		}
	}

	//���בւ���������idx�ɏ]���A�������ڑ����ƈꏏ�ɘA���B�A���̂��߂�object_desc_str()���g�p����
	t = object_desc_str(t, "�y");
	for (i = 0; i < insc_idx_num; i++)
	{
		//�{�̑O�ɐڑ���������Ƃ��A������S�̂̍ŏ��łȂ���Βu��
		if(i != 0 && nightmare_spellname_table[insc_idx_table[i]].conj1)
			t = object_desc_str(t, nightmare_spellname_table[insc_idx_table[i]].conj1);

		//�{�̂�u��
		t = object_desc_str(t, nightmare_spellname_table[insc_idx_table[i]].phrase);

		//�{�̌��ɐڑ���������Ƃ��A������S�̂̍Ō�łȂ��A�ЂƂ��̌�ɖ{�̑O�ڑ������Ȃ���Βu��
		//�ŏ���i<insc_idx_num-1�������ɂ��Ă���̂�i+1���z��̊O�ɂ͂ݏo�����Ƃ͂Ȃ��͂�
		if (i < insc_idx_num-1 && nightmare_spellname_table[insc_idx_table[i]].conj2 && !nightmare_spellname_table[insc_idx_table[i+1]].conj1)
			t = object_desc_str(t, nightmare_spellname_table[insc_idx_table[i]].conj2);
	}
	t = object_desc_str(t, "�z");

	//�A�C�e�������J�n
	object_prep(q_ptr, lookup_kind(TV_PHOTO, SV_PHOTO_NIGHTMARE));

	//�����I�Ƀ����_���A�[�e�B�t�@�N�g�����ɂ���
	q_ptr->art_name = quark_add(name_buf);
	//*�Ӓ�*�ς݈����ɂ���
	object_aware(q_ptr);
	object_known(q_ptr);
	q_ptr->ident |= (IDENT_MENTAL);
	//����̃o�g���ł̊l���|�C���g��xtra4��5�ɕ����ċL�^����
	q_ptr->xtra4 = (s16b)(tmp_nightmare_point / 10000);
	q_ptr->xtra5 = (s16b)(tmp_nightmare_point % 10000);

	drop_near(q_ptr, 0,y, x);


}



