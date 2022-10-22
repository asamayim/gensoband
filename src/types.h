/* File: types.h */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: global type declarations */


/*
 * This file should ONLY be included by "angband.h"
 */

/*
 * Note that "char" may or may not be signed, and that "signed char"
 * may or may not work on all machines.  So always use "s16b" or "s32b"
 * for signed values.  Also, note that unsigned values cause math problems
 * in many cases, so try to only use "u16b" and "u32b" for "bit flags",
 * unless you really need the extra bit of information, or you really
 * need to restrict yourself to a single byte for storage reasons.
 *
 * Also, if possible, attempt to restrict yourself to sub-fields of
 * known size (use "s16b" or "s32b" instead of "int", and "byte" instead
 * of "bool"), and attempt to align all fields along four-byte words, to
 * optimize storage issues on 32-bit machines.  Also, avoid "bit flags"
 * since these increase the code size and slow down execution.  When
 * you need to store bit flags, use one byte per flag, or, where space
 * is an issue, use a "byte" or "u16b" or "u32b", and add special code
 * to access the various bit flags.
 *
 * Many of these structures were developed to reduce the number of global
 * variables, facilitate structured program design, allow the use of ascii
 * template files, simplify access to indexed data, or facilitate efficient
 * clearing of many variables at once.
 *
 * Certain data is saved in multiple places for efficient access, currently,
 * this includes the tval/sval/weight fields in "object_type", various fields
 * in "header_type", and the "m_idx" and "o_idx" fields in "cave_type".  All
 * of these could be removed, but this would, in general, slow down the game
 * and increase the complexity of the code.
 */





/*
 * Feature state structure
 *
 * - Action (FF_*)
 * - Result (f_info ID)
 */
typedef struct feature_state feature_state;

struct feature_state
{
	byte action;
	s16b result;
};


/*
 * Information about terrain "features"
 */

typedef struct feature_type feature_type;

struct feature_type
{
	u32b name;                /* Name (offset) */
	u32b text;                /* Text (offset) */
	s16b tag;                 /* Tag (offset) */

	/*:::*/
	s16b mimic;               /* Feature to mimic */

	/*:::defines.h��FF_????�Ƃ��Ē�`����Ă���t���O�������Ă�炵���B*/
	u32b flags[FF_FLAG_SIZE]; /* Flags */

	u16b priority;            /* Map priority */
	s16b destroyed;           /* Default destroyed state */

	feature_state state[MAX_FEAT_STATES];


	byte subtype;
	/*:::�h�A�Ɍ����|�����Ă���ꍇ��power�ɐ��l�������Ă���*/
	byte power;

	byte d_attr[F_LIT_MAX];   /* Default feature attribute */
	byte d_char[F_LIT_MAX];   /* Default feature character */

	byte x_attr[F_LIT_MAX];   /* Desired feature attribute */
	byte x_char[F_LIT_MAX];   /* Desired feature character */
};


/*
 * Information about object "kinds", including player knowledge.
 *
 * Only "aware" and "tried" are saved in the savefile
 */

typedef struct object_kind object_kind;

struct object_kind
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */
	/*:::�s�m�薼����΁@�Q�[���J�n���ɃV���b�t�������*/
	u32b flavor_name;		/* Flavor name (offset) */

	/*:::tval:�A�C�e���̕��ޔԍ� �Ⴆ�Ώd�Z�Ȃ�37 defines.h�ɒ�`����k_info.txt�ɋL�^����Ă���*/
	byte tval;			/* Object type */
	byte sval;			/* Object sub type */

	s16b pval;			/* Object extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage dice/sides */

	s16b weight;		/* Weight */

	s32b cost;			/* Object "base cost" */

	u32b flags[TR_FLAG_SIZE];	/* Flags */

	u32b gen_flags;		/* flags for generate */


	/*:::��{�������x���ƃ��A���e�B�ȊO�Ƀ��x���ƃ��A���e�B�̐ݒ���������Ƃ�4�܂œ��́E�E�炵�����A�g�������悭�킩��Ȃ��B*/
	/*:::init_object_alloc�ł����g���Ă���B*/
	byte locale[4];		/* Allocation level(s) */
	byte chance[4];		/* Allocation chance(s) */

	byte level;			/* Level */
	/*:::k_info.txt�̃��A���e�B�l�������Ă��邪�A�A�C�e���������ɂ̓��A���e�B�Ƃ��Ďg���Ă��Ȃ��悤��*/
	byte extra;			/* Something */


	byte d_attr;		/* Default object attribute */
	byte d_char;		/* Default object character */


	byte x_attr;		/* Desired object attribute */
	byte x_char;		/* Desired object character */


	s16b flavor;		/* Special object flavor (or zero) */

	bool easy_know;		/* This object is always known (if aware) */

	/*:::�Ӓ�ς�*/
	bool aware;			/* The player is "aware" of the item's effects */

	/*:::�g�������Ƃ�����*/
	bool tried;			/* The player has "tried" one of the items */

	byte act_idx;		/* Activative ability index */
};



/*
 * Information about "artifacts".
 *
 * Note that the save-file only writes "cur_num" to the savefile.
 *
 * Note that "max_num" is always "1" (if that artifact "exists")
 */

typedef struct artifact_type artifact_type;

struct artifact_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Artifact type */
	byte sval;			/* Artifact sub type */

	s16b pval;			/* Artifact extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage when hits */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */ /*:::�悭������񂪉��i�v�Z�p�H0�Ȃ疳���l�����H*/

	u32b flags[TR_FLAG_SIZE];       /* Artifact Flags */

	u32b gen_flags;		/* flags for generate */

	byte level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */

	byte cur_num;		/* Number created (0 or 1) */
	byte max_num;		/* Unused (should be "1") */

	s16b floor_id;          /* Leaved on this location last time */

	byte act_idx;		/* Activative ability index */
};


/*
 * Information about "ego-items".
 */

typedef struct ego_item_type ego_item_type;

struct ego_item_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte slot;			/* Standard slot value */
	byte rating;		/* Rating boost */ /*:::*/

	byte level;			/* Minimum level */
	byte rarity;		/* Object rarity */

	byte max_to_h;		/* Maximum to-hit bonus */
	byte max_to_d;		/* Maximum to-dam bonus */
	byte max_to_a;		/* Maximum to-ac bonus */

	byte max_pval;		/* Maximum pval */

	s32b cost;			/* Ego-item "cost" */

	u32b flags[TR_FLAG_SIZE];	/* Ego-Item Flags */

	u32b gen_flags;		/* flags for generate */

	byte act_idx;		/* Activative ability index */
};




/*
 * Monster blow structure
 *
 *	- Method (RBM_*)
 *	- Effect (RBE_*)
 *	- Damage Dice
 *	- Damage Sides
 */

typedef struct monster_blow monster_blow;

struct monster_blow
{
	/*:::u16b�ɕύX����H*/
	byte method;
	byte effect;
	byte d_dice;
	byte d_side;
};


typedef struct mbe_info_type mbe_info_type;

struct mbe_info_type
{
	int power;        /* The attack "power" */
	int explode_type; /* Explosion effect */
};


/*
 * Monster "race" information, including racial memories
 *
 * Note that "d_attr" and "d_char" are used for MORE than "visual" stuff.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 * Note that "cur_num" (and "max_num") represent the number of monsters
 * of the given race currently on (and allowed on) the current level.
 * This information yields the "dead" flag for Unique monsters.
 *
 * Note that "max_num" is reset when a new player is created.
 * Note that "cur_num" is reset when a new level is created.
 *
 * Note that several of these fields, related to "recall", can be
 * scrapped if space becomes an issue, resulting in less "complete"
 * monster recall (no knowledge of spells, etc).  All of the "recall"
 * fields have a special prefix to aid in searching for them.
 */


typedef struct monster_race monster_race;
/*:::�����X�^�[���@�푰����:::*/
struct monster_race
{
	u32b name;				/* Name (offset) */
#ifdef JP
	u32b E_name;                    /* �p�ꖼ (offset) *//*:::�X�|�C���[�t�@�C���o�͂ɂ͎g���Ă���悤��*/
#endif
	u32b text;				/* Text (offset) */

	byte hdice;				/* Creatures hit dice count */
	byte hside;				/* Creatures hit dice sides */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	byte aaf;				/* Area affect radius (1-100) */
	byte speed;				/* Speed (normally 110) */

	s32b mexp;				/* Exp value for kill */

	/*:::�g���Ă��Ȃ��Br_info.txt�ł͏��0�������Ă���*/
	/*:::�����̐��l���u�o���_���W�����v�Ƃ��Ďg�����Ƃɂ����BRF8_STAY_AT_HOME�ƍ��킹�Ďg���B*/
	s16b extra;				/* Unused (for now) */

	/*:::�����g�p�p�x�@%�Ŋi�[�����B1/2�Ȃ�50*/
	byte freq_spell;		/* Spell frequency */

	u32b flags1;			/* Flags 1 (general) */
	u32b flags2;			/* Flags 2 (abilities) */
	u32b flags3;			/* Flags 3 (race/resist) */
	u32b flags4;			/* Flags 4 (inate/breath) */
	u32b flags5;			/* Flags 5 (normal spells) */
	u32b flags6;			/* Flags 6 (special spells) */
	u32b flags7;			/* Flags 7 (movement related abilities) */
	u32b flags8;			/* Flags 8 (wilderness info) */
	/*:::flags9�ɂ͐V���ȃ����X�^�[���@����ꂽ*/
	u32b flags9;			/* Flags 9 (drops info) */
	u32b flagsr;			/* Flags R (resistances info) */

	monster_blow blow[4];	/* Up to four blows per round */

	s16b next_r_idx;
	u32b next_exp;

	byte level;				/* Level of creature */
	byte rarity;			/* Rarity of creature */


	byte d_attr;			/* Default monster attribute */
	byte d_char;			/* Default monster character */


	byte x_attr;			/* Desired monster attribute */
	byte x_char;			/* Desired monster character */


	byte max_num;			/* Maximum population allowed per level */

	
	byte cur_num;			/* Monster population on current level */

	/*:::���j�[�N�̌��ݒn�B�V�����������ꂽ�炻�̊K��ID�ɏ��������A�Â��}�b�v����͏�����*/
	s16b floor_id;                  /* Location of unique monster */

	/*:::���邽�тɃJ�E���g����Ă���B���ۂɂ́u��x�ł��������Ƃ�����΁`�v�I�ȏ������ɂ̂ݎg���Ă���悤��*/
	s16b r_sights;			/* Count sightings of this monster */
	s16b r_deaths;			/* Count deaths from this monster */

	s16b r_pkills;			/* Count visible monsters killed in this life */
	s16b r_akills;			/* Count all monsters killed in this life */
	s16b r_tkills;			/* Count monsters killed in all lives */

	byte r_wake;			/* Number of times woken up (?) */
	byte r_ignore;			/* Number of times ignored (?) */

	byte r_xtra1;			/* Something (unused) */
	byte r_xtra2;			/* Something (unused) */

	byte r_drop_gold;		/* Max number of gold dropped at once */
	byte r_drop_item;		/* Max number of item dropped at once */

	/*:::�����X�^�[��������������Ƃ���������񐔁H�@100���z����������g�p�m�����킩��H*/
	byte r_cast_spell;		/* Max number of other spells seen */

	byte r_blows[4];		/* Number of times each blow type was seen */

	/*:::�����m���Ă��郂���X�^�[�̓����@7�͍��̂Ƃ���g���ĂȂ�(����΂킩��A�������͒m��悤�̂Ȃ�����)*/
	u32b r_flags1;			/* Observed racial flags */
	u32b r_flags2;			/* Observed racial flags */
	u32b r_flags3;			/* Observed racial flags */
	u32b r_flags4;			/* Observed racial flags */
	u32b r_flags5;			/* Observed racial flags */
	u32b r_flags6;			/* Observed racial flags */
	/* u32b r_flags7; */			/* Observed racial flags */

	///mod140102 flags9�ǉ�
	u32b r_flags9;			/*:::�V���@�p�ɐV���ɍ����*/
	u32b r_flagsr;			/* Observed racial resistance flags */
};



/*
 * Information about "vault generation"
 */

typedef struct vault_type vault_type;

struct vault_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte typ;			/* Vault type */

	byte rat;			/* Vault rating */

	byte hgt;			/* Vault height */
	byte wid;			/* Vault width */
};


/*
 * Information about "skill"
 */

typedef struct skill_table skill_table;

/*:::����n���x�A�R��A�}�[�V�����A�[�c�A�񓁗��̃X�L���̏����l�A�ő�l*/
struct skill_table
{
	s16b w_start[5][64];	  /* start weapon exp */
	s16b w_max[5][64];        /* max weapon exp */
	s16b s_start[10];	  /* start skill */
	s16b s_max[10];           /* max skill */
};


/*
 * A single "grid" in a Cave
 *
 * Note that several aspects of the code restrict the actual cave
 * to a max size of 256 by 256.  In partcular, locations are often
 * saved as bytes, limiting each coordinate to the 0-255 range.
 *
 * The "o_idx" and "m_idx" fields are very interesting.  There are
 * many places in the code where we need quick access to the actual
 * monster or object(s) in a given cave grid.  The easiest way to
 * do this is to simply keep the index of the monster and object
 * (if any) with the grid, but this takes 198*66*4 bytes of memory.
 * Several other methods come to mind, which require only half this
 * amound of memory, but they all seem rather complicated, and would
 * probably add enough code that the savings would be lost.  So for
 * these reasons, we simply store an index into the "o_list" and
 * "m_list" arrays, using "zero" when no monster/object is present.
 *
 * Note that "o_idx" is the index of the top object in a stack of
 * objects, using the "next_o_idx" field of objects (see below) to
 * create the singly linked list of objects.  If "o_idx" is zero
 * then there are no objects in the grid.
 *
 * Note the special fields for the "MONSTER_FLOW" code.
 */

typedef struct cave_type cave_type;

struct cave_type
{
	u16b info;		/* Hack -- cave flags */

	/*:::�n�`���*/
	s16b feat;		/* Hack -- feature type */

	s16b o_idx;		/* Object in this grid */

	s16b m_idx;		/* Monster in this grid */

	/*:::���[���h�}�b�v�̊X�ԍ��A�_���W�����ւ̊K�i�̃_���W����ID�ȂǐF�X����*/
	s16b special;	/* Special cave info */

	/*:::�B���h�A�Ȃ�*/
	s16b mimic;		/* Feature to mimic */

	byte cost;		/* Hack -- cost of flowing */
	byte dist;		/* Hack -- distance from player */
	/*:::���̃O���b�h�̃R�X�g���v�Z���ꂽ�^�[���H*/
	byte when;		/* Hack -- when cost was computed */

	//v1.1.24 ��s����dist�l �ڍׂ�update_flying_dist()�Q��
	byte flying_dist;

	//v1.1.33 �_���W�����O���b�h�p�ϐ��ǉ� �Z�[�u�̂��߂�cave_template_type�֘A�ɂ��F�X�ǋL
	u32b cave_xtra_flag;
	s16b cave_xtra1;
	u16b cave_xtra2;

};



/*
 * Simple structure to hold a map location
 */
typedef struct coord coord;

struct coord
{
	byte y;
	byte x;
};



/*
 * Object information, for a specific object.
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Note that "object" records are "copied" on a fairly regular basis,
 * and care must be taken when handling such objects.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "o_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "hold_o_idx"
 * field (below).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a pile of objects held by the monster.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */

typedef struct object_type object_type;
/**::::�A�C�e�����:::*/
struct object_type
{
	/*:::k_info.txt�̔ԍ�?*/
	s16b k_idx;			/* Kind index (zero if "dead") */

	byte iy;			/* Y-position on map, or zero */
	byte ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	 /*:::�p�����[�^�㏸�l�����łȂ����̐����K�A������̎g�p�񐔁A����l�`�A���̂̃����X�^�[�C���f�b�N�X�������Ă���*/
	s16b pval;			/* Item extra-parameter */

	/*:::������ 0-99*/
	byte discount;		/* Discount (if any) */

	byte number;		/* Number of items */

	s16b weight;		/* Item weight */

	/*:::���̃A�[�e�B�t�@�N�gID������*/
	//mod140506 ���̐���255�𒴂����̂�name1��u16b�ɂ���
	u16b name1;			/* Artifact type, if any */
	byte name2;			/* Ego-Item type, if any */

	/*:::�g���Ă��Ȃ��炵�����R�[�h���c���Ă���B"powerful item"�炵���B*/
	/*:::�t�r�_�g���̗d�퉻�J�E���g�Ɏg�����Ƃɂ����B100,200�ɂȂ����Ƃ��\�͂��t�^����200�ŃJ�E���g���X�g�b�v����B*/
	//�b��t�ƕ���t�̂Ƃ��A�S�̒b�艮�ɂ�镐��C�����s��ꂽ�t���O�Ƃ��Ă��g�����Ƃɂ����B
	//����u�e�v�ɂ��g��
	byte xtra1;			/* Extra info type (now unused) */
	//���̔����C���f�b�N�X�ȂǁH
	byte xtra2;			/* Extra info activation index */
	/*:::�b��t�p�t���O�����A��������̃J���d���̐����K�Ƃ������X�^�[�{�[���̃p�����[�^�ȂǕʂ̂��Ƃɂ��g���Ă���*/
	/*:::�t�r�_�g���̕���̗d�퉻�ݒ�ɂ��g�����Ƃɂ����B�d�퉻�ς̂Ƃ�xtra3��255������B*/
	//���̖�Ƃ��ɂ��g��
	//v1.1.15 �������ꂽ����ɂ��g��
	byte xtra3;			/* Extra info for weaponsmith */
	s16b xtra4;			/* Extra info fuel or captured monster's current HP *//*:::���قƃ����^���̔R���ɂ��g���Ă���*/

	//�ϒK�푈�̕�V�Â�A�����됶���ʂł��g��
	//���̎􂢉����o���l(1/1000)�J�E���g�Ƃ��Ă��g�����Ƃɂ���
	//�V���̍����ɂ��g��
	//�����̍����������J�E���g�ɂ��g��
	s16b xtra5;			/* Extra info captured monster's max HP */

	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */
	s16b to_a;			/* Plusses to AC */

	s16b ac;			/* Normal AC */

	byte dd, ds;		/* Damage dice/sides */

	s16b timeout;		/* Timeout Counter */

	byte ident;			/* Special flags  */

	byte marked;		/* Object is marked *//*:::���������A�G�����Ȃǂ̃t���O*/

	u16b inscription;	/* Inscription index *//*:::���̃C���f�b�N�X�������ɓ����炵���B���̂̕������quark__str[]���ɂ���H*/
	u16b art_name;      /* Artifact name (random artifacts) */

	/*:::�ȈՊӒ肵���Ƃ����̎��ID������*/
	byte feeling;          /* Game generated inscription number (eg, pseudo-id) */

	/*:::pval�Ώےl�A�X���C�A�ϐ��A���̑��\�͂ȂǕВ[���炱���ɓ����ăr�b�g�����t�߂�*/
	u32b art_flags[TR_FLAG_SIZE];        /* Extra Flags for ego and artifacts */
	

	u32b curse_flags;        /* Flags for curse */

	s16b next_o_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Monster holding us (if any) */

	//v1.1.25 ���i(�X�R�A�T�[�o�p�@
	//���o�[�W��������̃f�[�^���[�h����0�����A
	//object_value_real��0�ȊO�ɂȂ����Ƃ��X�V����
	s32b score_value;
	//�������ꂽ�^�[���ƊK�w
	u32b create_turn;
	byte create_lev;

	//v1.1.81 �g���p�ϐ��ǉ�
	s16b xtra6;
	s16b xtra7;
	s16b xtra8;
	s16b xtra9;
	u32b new_obj_flags;
	s16b new_obj_param16[8];
	u32b new_obj_param32[4];


};



/*
 * Monster information, for a specific monster.
 *
 * Note: fy, fx constrain dungeon size to 256x256
 *
 * The "hold_o_idx" field points to the first object of a stack
 * of objects (if any) being carried by the monster (see above).
 */

typedef struct monster_type monster_type;
/*:::�����X�^�[���i�̂��Ɓj�ʒu�AHP�A�����Ȃ�  :::*/
struct monster_type
{
	s16b r_idx;		/* Monster race index */
	s16b ap_r_idx;		/* Monster race appearance index *//*:::���ʂ��ȊO��r_idx�ƈ�v�H*/
	byte sub_align;		/* Sub-alignment for a neutral monster */

	byte fy;		/* Y location on map */
	byte fx;		/* X location on map */

	s16b hp;		/* Current Hit points */
	s16b maxhp;		/* Max Hit points */
	s16b max_maxhp;		/* Max Max Hit points */

	s16b mtimed[MAX_MTIMED];	/* Timed status counter */

	byte mspeed;	        /* Monster "speed" */
	s16b energy_need;	/* Monster "energy" */

	byte cdis;		/* Current dis from player */

	//byte mflag;		/* Extra monster flags */
	u32b mflag;		/* Extra monster flags */
	byte mflag2;		/* Extra monster flags */

	bool ml;		/* Monster is "visible" */

	s16b hold_o_idx;	/* Object being held (if any) */

	s16b target_y;		/* Can attack !los player */
	s16b target_x;		/* Can attack !los player */

	/*:::�y�b�g�̖��O�@�����ɋL�^����̂�quark_str()�p�̃C���f�b�N�X*/
	u16b nickname;		/* Monster's Nickname */

	u32b exp;

	/*:::�����X�^�[���L�����Ă��遗�̑ϐ���񂪓���炵�� smart_cheat�̂Ƃ��͎g���Ȃ�*/
	u32b smart;			/* Field for "smart_learn" */

	/*:::���̃����X�^�[���������������X�^�[��idx*/
	s16b parent_m_idx;

	//v1.1.81 �g���p�ϐ��ǉ�
	s16b future_use[7];	

	u32b mflag3;
	u32b mflag4;
	s16b xtra_dir_param;
	s16b timed_shard_count;//�x�����̊ǌϒe

	s16b new_monster_param3;
	s16b new_monster_param4;
	s16b new_monster_param5;
	s16b new_monster_param6;
	s16b new_monster_param7;
	s16b new_monster_param8;

	u16b mon_random_number; //��������0-65535�̃����_���Ȑ��l������BWEIRD_MIND���m����Ȃǃ����_�������̈ˑ��Ȃ��ƂɎg���B


};




/*
 * An entry for the object/monster allocation functions
 *
 * Pass 1 is determined from allocation information
 * Pass 2 is determined from allocation restriction
 * Pass 3 is determined from allocation calculation
 */


/*:::get_obj_num_prep����g���Ă邪���̈Ӗ�������̂��悭������Ȃ�*/
/*:::�A�C�e���⃂���X�^�[�̊�{�K�w��S��ޕ��ŏ��ɕێ����A���̌�A�C�e���⃂���X�^�[�������ɂǂꂪ�I�΂�邩���߂�Ƃ��Ɏg���H*/
/*:::prob1-3�ɂǂ�Ȑ��l�������Ăǂ��g����̂��͂܂��͂����肵�Ȃ��@100�ӂ������ɂ��郌�A���e�B�l�H*/
/*:::get_mon_num_prep��alloc_race_table��������g���Ă���*/

typedef struct alloc_entry alloc_entry;

struct alloc_entry
{
	s16b index;		/* The actual index */

	byte level;		/* Base dungeon level */
	byte prob1;		/* Probability, pass 1 */
	byte prob2;		/* Probability, pass 2 */
	byte prob3;		/* Probability, pass 3 */

	u16b total;		/* Unused for now */
};



/*
 * Available "options"
 *
 *	- Address of actual option variable (or NULL)
 *
 *	- Normal Value (TRUE or FALSE)
 *
 *	- Option Page Number (or zero)
 *
 *	- Savefile Set (or zero)
 *	- Savefile Bit in that set
 *
 *	- Textual name (or NULL)
 *	- Textual description
 */

typedef struct option_type option_type;

struct option_type
{
	bool	*o_var;

	byte	o_norm;

	byte	o_page;

	byte	o_set;
	byte	o_bit;

	cptr	o_text;
	cptr	o_desc;
};


/*
 * Structure for the "quests"
 */
typedef struct quest_type quest_type;

struct quest_type
{
	s16b status;            /* Is the quest taken, completed, finished? */

	s16b type;              /* The quest type */

	char name[60];          /* Quest name */
	s16b level;             /* Dungeon level */

	/*:::���[�O�N�G�Ȃǎw�萔�̃����X�^�[��|���N�G�X�g�p*/
	s16b r_idx;             /* Monster race */
	s16b cur_num;           /* Number killed */
	s16b max_num;           /* Number required */

	/*:::�󕨌ɗp�H�A�[�e�B�t�@�N�g�C���f�b�N�X������炵��*/
	s16b k_idx;             /* object index */

	s16b num_mon;           /* number of monsters on level */

	byte flags;             /* quest flags */

							/*:::�ǂ��̃_���W�����ōs���邩�@�S����1 �C�[�N��2�@�I���W�i���}�b�v��0*/
	byte dungeon;           /* quest dungeon */

	byte complev;           /* player level (complete) */

	//v1.1.25 �B�����v���C���Ԃ��L�^����
	u32b comptime;


};

/*:::���͎����g���ĂȂ��͂�*/
/*
 * A store owner
 */
typedef struct owner_type owner_type;

struct owner_type
{
	cptr owner_name;	/* Name */

	s16b max_cost;		/* Purse limit */

	byte max_inflate;	/* Inflation (max) */
	byte min_inflate;	/* Inflation (min) */

	byte haggle_per;	/* Haggle unit */

	byte insult_max;	/* Insult limit */

	byte owner_race;	/* Owner race */
};




/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
typedef struct store_type store_type;

struct store_type
{
	byte type;				/* Store type */

	byte owner;				/* Owner index */
	byte extra;				/* Unused for now */

	s16b insult_cur;		/* Insult counter */

	s16b good_buy;			/* Number of "good" buys */
	s16b bad_buy;			/* Number of "bad" buys */

	s32b store_open;		/* Closed until this turn */

	/*:::�Ō�ɖK�ꂽ�Ƃ��̃Q�[���^�[��(s32b turn)*/
	s32b last_visit;		/* Last visited on this turn */

	/*:::BM�ȊO�̓X�ł͗\�ߏ������ꂽ�f�[�^���珤�i�̃x�[�X�A�C�e�������߂Ă���炵��*/
	s16b table_num;			/* Table -- Number of entries */
	s16b table_size;		/* Table -- Total Size of Array */
	s16b *table;			/* Table -- Legal item kinds */

	s16b stock_num;			/* Stock -- Number of entries */
	s16b stock_size;		/* Stock -- Total Size of Array */
	object_type *stock;		/* Stock -- Actual stock items */
};


/*
 * The "name" of spell 'N' is stored as spell_names[X][N],
 * where X is 0 for mage-spells and 1 for priest-spells.
 */
typedef struct magic_type magic_type;

///mod140315 byte����int�ɂ���
struct magic_type
{
	int slevel;		/* Required level (to learn) */ /*:::���̐E�Ŏg���Ȃ����@��100�ȏオ����炵��*/
	int smana;			/* Required mana (to cast) */
	int sfail;			/* Minimum chance of failure */
	int sexp;			/* Encoded experience bonus */
};

///mod140207 ���@�ύX
#if 0
/*
 * Information about the player's "magic"
 *
 * Note that a player with a "spell_book" of "zero" is illiterate.
 */

typedef struct player_magic player_magic;

struct player_magic
{
	/*:::������TV_LIFE_BOOK���Ɓu�F��v�Ŗ��@���g���E�Ɣ��肳���炵��*/
	/*:::Q021 spell_book�̖������悭����Ȃ��@���̈悪���̂܂ܓ����Ă�Ƃ��v���Ȃ���*/
	/*:::SORCERY,LIFE,MUSIC,HISSATSU,NONE�̂����ꂩ������B���@�K�����@�Ȃǂɉe������*/
	int spell_book;		/* Tval of spell books (if any) */
	int spell_xtra;		/* Something for later */

	int spell_stat;		/* Stat for spells (if any)  *//*:::�e���p�����[�^*/
	int spell_type;		/* Spell type (mage/priest) *//*:::�g���Ă��Ȃ��炵��*/

	int spell_first;		/* Level of first spell *//*:::���@�K������MP�ő�l�v�Z�Ɏg��*/
	int spell_weight;		/* Weight that hurts spells */

	/*:::���@10�̈敪�̊�{���@�Y������REALM_???����1�������l*/
	magic_type info[MAX_MAGIC][32];    /* The available spells */
};
#endif


/*
 * Player sex info
 */

typedef struct player_sex player_sex;

struct player_sex
{
	cptr title;			/* Type of sex */
	cptr winner;		/* Name of winner */
#ifdef JP
	cptr E_title;		/* �p�ꐫ�� */
	cptr E_winner;		/* �p�ꐫ�� */
#endif
};


/*
 * Player racial info
 */

typedef struct player_race player_race;

struct player_race
{
	cptr title;			/* Type of race */

#ifdef JP
	cptr E_title;		/* �p��푰 */
#endif
	s16b r_adj[6];		/* Racial stat bonuses */

	s16b r_dis;			/* disarming */
	s16b r_dev;			/* magic devices */
	s16b r_sav;			/* saving throw */
	s16b r_stl;			/* stealth */
	s16b r_srh;			/* search ability */

	///mod140105 �푰�ύX�i�����j
	s16b r_graze;		/*:::���m�Z�\��T���Ɠ������O���C�Y�Z�\�Ɠ���ւ���*/
//	s16b r_fos;			/* search frequency */
	s16b r_thn;			/* combat (normal) */
	s16b r_thb;			/* combat (shooting) */

	byte r_mhp;			/* Race hit-dice modifier */
	byte r_exp;			/* Race experience factor */
	///sysdel �s�v�Ȃ̂ŕʂ̒l�Ɏg����
	byte b_age;			/* base age */
	byte m_age;			/* mod age */

	byte m_b_ht;		/* base height (males) */
	byte m_m_ht;		/* mod height (males) */
	byte m_b_wt;		/* base weight (males) */
	byte m_m_wt;		/* mod weight (males) */

	byte f_b_ht;		/* base height (females) */
	byte f_m_ht;		/* mod height (females)	  */
	byte f_b_wt;		/* base weight (females) */
	byte f_m_wt;		/* mod weight (females) */

	byte infra;			/* Infra-vision	range */

	/*:::�����Ă�N���X�H�폜���Ƃ�����*/
	u32b choice;        /* Legal class choices */
/*    byte choice_xtra;   */

	byte realm_aptitude[16];/*:::�e���@�̈�ɑ΂���K���@�Y����TV_BOOK_???�ɑΉ� [0]�̓_�~�[�@0�͏K���s�A5�͕��ʁA9�͑哾��*/
	byte score_mult;		/*:::�X�R�A�{���@��{100*/
	bool flag_nofixed;		/*:::�܂���������ĂȂ��@�L�����I�����D�F�@���ȕϗe���s��*/
	bool flag_nobirth;		/*:::�����I��s�@�L�����I�����\������Ȃ� ���ȕϗe����*/
	bool flag_special;		/*:::���b�`�Ȃǁ@�L�����I�����\������Ȃ� ���ȕϗe���s�@�ꕔ����]�����ɉ�*/
	bool flag_only_unique;  /*:::���j�[�N�N���X��p�푰�̂Ƃ�TRUE�@�L�����I�����\�����ꂸ���ȕϗe�Ȃǂł����̎푰�ɂȂ�Ȃ�*/

	//v1.1.42 ���Ɏ����������_�����j�[�N����邽�߂̎푰���
	char random_unique_symbol;
	u32b random_unique_flags3;




};


/*
 * Player class info
 */

typedef struct player_class player_class;

struct player_class
{
	cptr title;			/* Type of class */
	cptr f_title; ///mod140129 �����p�E�Ɩ��ǉ�

#ifdef JP
	/*:::�����E����v���Z�X�t�@�C�����߂Ȃǂ̕����ŏ�������Ɏg���Ă�炵��*/
	cptr E_title;		/* �p��E�� */
#endif
	s16b c_adj[6];		/* Class stat modifier */

	s16b c_dis;			/* class disarming */
	s16b c_dev;			/* class magic devices */
	s16b c_sav;			/* class saving throws */
	s16b c_stl;			/* class stealth */
	s16b c_srh;			/* class searching ability */
	//�����O���C�Y�l�ɕύX�\��
	s16b c_fos;			/* class searching frequency */
	s16b c_thn;			/* class to hit (normal) */
	s16b c_thb;			/* class to hit (bows) */

	s16b x_dis;			/* extra disarming */
	s16b x_dev;			/* extra magic devices */
	s16b x_sav;			/* extra saving throws */
	s16b x_stl;			/* extra stealth */
	s16b x_srh;			/* extra searching ability */
	//�����O���C�Y�l�ɕύX�\��
	s16b x_fos;			/* extra searching frequency */
	s16b x_thn;			/* extra to hit (normal) */
	s16b x_thb;			/* extra to hit (bows) */

	s16b c_mhp;			/* Class hit-dice adjustment */
	s16b c_exp;			/* Class experience factor */

	byte pet_upkeep_div; /* Pet upkeep divider */

	///mod140128 �ǉ�����

	/*:::�e���@�̈�ւ̓K���@�K�����x�����x�ɉe�� 
	 *:::[0]�͖��@�K���t���O��0���Ɣ񖂖@�E�A1���ƈ�̈�g�p�A2���ƐԖ���X�y�}�X�ȂǓ���K�� 3���Ɠ�̈�K����
	 *:::[1�`15]�͖��@����TVAL�ɑΉ����A0:�g�p�s�� 1:�i���Ȃ�(����ڂ܂Ł@�x��) 2:��ʐl�Ȃǁi�ꕔ�̖��@�̂݁@�K�����Ȃ�x���j 3:�T���ƂȂǁi�ꕔ�g���Ȃ��@�K���x���j 4:���C�h�Ȃǁi�K�����x���j 5:���C�W�Ȃ� 6:�n�C���C�W�Ȃǁi���K��)*/
	/*:::  ���f�A�\���A�t�^�A�����A�_��A�����A�j�ׁA���R�A�ϗe�A�Í��A����A���ׁA�K�E���A�\���̈�1�A�\���̈�2�̏�*/
	byte realm_aptitude[16];

	/*:::�U���񐔌v�Z�l�@calc_bonuses()�̃n�[�h�R�[�f�B���O����ڂ�*/
	byte spell_stat; //���@���ǂ̔\�͒l�Ŏg�����@��{��A_INT
	byte weapon_num;
	byte weapon_mul;
	int weapon_div;
	/*:::�X�L�������l�Ɛ������ƌ��E�l�ɉe�� 1�`5*/
	/*:::0:�i�� 1:�� 2:��n 3:�� 4:���� 5-9:�_�~�[
	 *:::10:�Z���`20:�e*/
	byte skill_aptitude[SKILL_EXP_MAX];
	/*:::�X�R�A�{���@��{100*/
	int score_mult;
	/*:::�����d�ʂɂ�閂�@�ւ̈��e���̋N���ɂ��� 0(�e���Ȃ�or���@�s�g�p) 1(�y�i���e�B�d)-5(�y�i���e�B�y)*/
	byte heavy_cast_mult;
	/*:::����A���@�n�̊ȈՊӒ萸�x 1(�Ⴂ)�`5(����)*/
	byte sense_arms;
	byte sense_mags;

	/*:::�����@�E��TRUE ���͏[�U�≊�̊����Ȃǂ̏����Ɋ֌W �f���A��5%�������������*/
	bool magicmaster;
	/*:::�܂�������Ƃ��ς�łȂ����Ƃ������t���O�@�L�����I�����D�F�ɂȂ��đI�ׂȂ�*/
	bool flag_nofixed;		
	/*:::���j�[�N�N���X�t���O�@��ʑI������I�ׂȂ�*/
	bool flag_only_unique;

	//���̍��l�b�g���[�N�ɉ����ł���E��TRUE ��Ƀ��j�[�N�N���X�̉��������p
	bool flag_join_grassroots;

	/*:::MP�̑����₷�� 1�`5*/
	byte mana_rate;

	//v1.1.58 ���j�[�N�N���X�ɑΉ�����r_idx
	u16b unique_r_idx;

};


typedef struct player_seikaku player_seikaku;
struct player_seikaku
{
	/*:::*/
	cptr title;			/* Type of seikaku */
	cptr f_title;			/* Type of seikaku */ //�����p

#ifdef JP
	cptr E_title;		/* �p�ꐫ�i */
#endif

	s16b a_adj[6];		/* seikaku stat bonuses */

	s16b a_dis;			/* seikaku disarming */
	s16b a_dev;			/* seikaku magic devices */
	s16b a_sav;			/* seikaku saving throw */
	s16b a_stl;			/* seikaku stealth */
	s16b a_srh;			/* seikaku search ability */
	s16b a_fos;			/* seikaku search frequency */
	s16b a_thn;			/* seikaku combat (normal) */
	s16b a_thb;			/* seikaku combat (shooting) */

	s16b a_mhp;			/* Race hit-dice modifier */

	byte no;			/* �� */
	//v1.1.32 ���ʂɂ�鐫�i������������
	//byte sex;			/* seibetu seigen */

	bool flag_nofixed;
};


/*
 * Most of the "player" information goes here.
 *
 * This stucture gives us a large collection of player variables.
 *
 * This structure contains several "blocks" of information.
 *   (1) the "permanent" info
 *   (2) the "variable" info
 *   (3) the "transient" info
 *
 * All of the "permanent" info, and most of the "variable" info,
 * is saved in the savefile.  The "transient" info is recomputed
 * whenever anything important changes.
 */

typedef struct player_type player_type;

struct player_type
{
	s16b oldpy;		/* Previous player location -KMW- */
	s16b oldpx;		/* Previous player location -KMW- */

	byte psex;			/* Sex index */
	byte prace;			/* Race index */
	byte pclass;		/* Class index */
	byte pseikaku;		/* Seikaku index */
	byte realm1;        /* First magic realm */
	byte realm2;        /* Second magic realm */

	//v1.1.13
	//oops�Ƃ������g�p�ϐ����������A�ꕔ���Z��ON�ɂȂ����ȃt���O���i�[���邱�Ƃɂ����B
	//reset_tim_flags()�ȊO�ł͎�����������Ȃ�
	//�A�C�e���J�[�h����g�p����ȂǓ���̃N���X�Ɍ���Ȃ����Z�������������Ƃ���
	byte special_flags; 

	byte hitdie;		/* Hit dice (sides) */
	/*:::EXP�y�i���e�B*/
	u16b expfact;       /* Experience factor
			     * Note: was byte, causing overflow for Amberite
			     * characters (such as Amberite Paladins)
			     */

	s16b age;			/* Characters age */ //���푰�u������v�̏������͌���p�����Ƃ��Ďg���Ă���
	s16b ht;			/* Height */
	s16b wt;			/* Weight */ 
	s16b sc;			/* Social Class */ //���d���p�̓��Z�ۑ��̈�ɂ����B���푰�g�p�s��



	s32b au;			/* Current Gold *//*:::������*/

	s32b max_max_exp;	/* Max max experience (only to calculate score) */
	s32b max_exp;		/* Max experience */
	s32b exp;			/* Cur experience */
	u32b exp_frac;		/* Cur exp frac (times 2^16) */

	s16b lev;			/* Level */

	s16b town_num;			/* Current town number */
	/*:::�A���[�i�̂ǂ��܂ŏ����オ�������@�������畉�̒l������*/
	//s16b arena_number;		/* monster number in arena -KMW- */
	s16b kourindou_number;	//v1.1.56 �������ł̒��i�N�W�̂���arena_number������ɕς���
	bool inside_arena;		/* Is character inside arena? */
	s16b inside_quest;		/* Inside quest level */
	bool inside_battle;		/* Is character inside tougijou? */

	/*:::���[���h�}�b�v�ł̈ʒu*/
	s32b wilderness_x;	/* Coordinates in the wilderness */
	s32b wilderness_y;
	bool wild_mode;		/*:::���[���h�}�b�v����TRUE*/

	s32b mhp;			/* Max hit pts */
	s32b chp;			/* Cur hit pts */

	u32b chp_frac;		/* Cur hit frac (times 2^16) */	//HP�̒[���B���R�񕜗ʂ̌v�Z�Ɏg��

	s32b msp;			/* Max mana pts */
	s32b csp;			/* Cur mana pts */
	u32b csp_frac;		/* Cur mana frac (times 2^16) */

	s16b max_plv;		/* Max Player Level *//*:::���݃��x���_�E�����ĂĂ��l�����Ȃ��ō��n�_���x��*/

	s16b stat_max[6];	/* Current "maximal" stat values */
		/*:::��{�p�����[�^�ő�l 18/70�`18/130*/
	s16b stat_max_max[6];	/* Maximal "maximal" stat values */
	s16b stat_cur[6];	/* Current "natural" stat values */

	/*:::�K���ςݖ��@�̐��@�̈�ύX�Ō���Ȃ�*/
	s16b learned_spells;
	/*:::�ǉ����@�K���g�@�����̊����ő�����*/
	s16b add_spells;

	u32b count;

	s16b fast;		/* Timed -- Fast */
	s16b slow;		/* Timed -- Slow */
	s16b blind;		/* Timed -- Blindness */
	s16b paralyzed;		/* Timed -- Paralysis */
	s16b confused;		/* Timed -- Confusion */
	s16b afraid;		/* Timed -- Fear */
	s16b image;		/* Timed -- Hallucination */
	s16b poisoned;		/* Timed -- Poisoned */
	s16b cut;		/* Timed -- Cut *//*:::�؂菝�x*/
	s16b stun;		/* Timed -- Stun *//*:::�N�O�x 100�ňӎ��s���āA50�łЂǂ��N�O*/

	s16b protevil;		/* Timed -- Protection */
	s16b invuln;		/* Timed -- Invulnerable */
	s16b ult_res;		/* Timed -- Ultimate Resistance */
	s16b hero;		/* Timed -- Heroism */
	s16b shero;		/* Timed -- Super Heroism */
	s16b shield;		/* Timed -- Shield Spell */
	s16b blessed;		/* Timed -- Blessed */
	s16b tim_invis;		/* Timed -- See Invisible */
	s16b tim_infra;		/* Timed -- Infra Vision */
	s16b tsuyoshi;		/* Timed -- Tsuyoshi Special */
	s16b ele_attack;	/* Timed -- Elemental Attack */
	s16b ele_immune;	/* Timed -- Elemental Immune */

	s16b oppose_acid;	/* Timed -- oppose acid */
	s16b oppose_elec;	/* Timed -- oppose lightning */
	s16b oppose_fire;	/* Timed -- oppose heat */
	s16b oppose_cold;	/* Timed -- oppose cold */
	s16b oppose_pois;	/* Timed -- oppose poison */


	s16b tim_esp;       /* Timed ESP *//*:::�ꎞ�e���p�X*/
	s16b wraith_form;   /* Timed wraithform */

	s16b resist_magic;  /* Timed Resist Magic (later) */
	s16b tim_regen;
	s16b kabenuke;
	s16b tim_stealth;
	s16b tim_levitation;
	s16b tim_sh_touki;
	s16b lightspeed;
	s16b tim_sh_death;//����̃I�[��
	/*:::magicdef��resist_magic�̈Ⴂ�́H*/
	/*:::magicdef:���@�̊Z�@���@�h��㏸+AC50+�l�X�ȑϐ��@���͏����̑Ώ�*/
	/*:::resist_magic:���C�p�̖��@�h��㏸ ���@�h��㏸�����������͏����̑Ώۂɂ͂Ȃ�Ȃ�*/
	s16b magicdef;
	s16b tim_res_nether;	/* Timed -- Nether resistance */
	s16b tim_res_time;	/* Timed -- Time resistance */
	byte mimic_form;
	s16b tim_mimic;
	s16b tim_sh_fire;
	s16b tim_sh_holy;
	s16b tim_eyeeye;

	/* for mirror master */
	s16b tim_reflect;       /* Timed -- Reflect */
	s16b multishadow;       /* Timed -- Multi-shadow */
	s16b dustrobe;          /* Timed -- Robe of dust */

	//�J�I�X�p�g�����@�E�Ƃɂ�����炸�Q�[���J�n���Ƀ����_���Ɍ��܂�A�J�I�X�p�g�����ψق𓾂�ƕ\�������B
	///mod150103 �푰��ǐ_�l�̂Ƃ��A�N�C�b�N�X�^�[�g�̂��߂ɍŏ��̐_�i�^�C�v�������Ɋi�[���邱�Ƃɂ����B��ǐ_�l�̂Ƃ��̓J�I�X�p�g�����ψق𓾂��Ȃ��B
	s16b chaos_patron; 
	u32b muta1;
	u32b muta2;
	u32b muta3;

	///mod140316 �S�̊p��l���̐K���ȂǏ����Ȃ��ˑR�ψق������ɋL�^����悤�ɂ���
	u32b muta1_perma;
	u32b muta2_perma;
	u32b muta3_perma;

	//v1.1.64 �ǉ�
	u32b muta4;
	u32b muta4_perma;


	//���̕ϐ������g��񂩂�\���̈�ɂ��Ă��܂��������ǂǂ��ŉ��Ɏg���Ă邩������񂩂�g���Â炢
	s16b virtues[8];
	s16b vir_types[8];

	/*:::�A�҂̔����^�[����*/
	s16b word_recall;	  /* Word of recall counter */
	s16b alter_reality;	  /* Alter reality counter */
	/*:::�n�ォ��A�҂���Ƃ��̋A�Ґ�*/
	byte recall_dungeon;      /* Dungeon set to be recalled */

	/*:::���̍s���܂łɕK�v�ȃG�l���M�[�B���Ԓ�~����-1000�ȉ��ɂȂ�*/
	s16b energy_need;	  /* Energy needed for next move */
	/*:::process_upkeep_with_speed()�Ŏg����G�l���M�[*/
	s16b enchant_energy_need;	  /* Energy needed for next upkeep effect	 */

	s16b food;		  /* Current nutrition */
	
	///mod140326 �D���x
	s16b alcohol;

	u32b special_attack;	  /* Special attack capacity -LM- */
	/*:::���p�̌^�Ȃ�*/
	u32b special_defense;	  /* Special block capacity -LM- */
	byte action;		  /* Currently action */

	/*:::�K���������@�̃t���O*/
	u32b spell_learned1;	  /* bit mask of spells learned */
	u32b spell_learned2;	  /* bit mask of spells learned */
	/*:::��x�g�p�������@�̃t���O�@����g�p���̌o���l�l���̂��߂Ǝv����*/
	u32b spell_worked1;	  /* bit mask of spells tried and worked */
	u32b spell_worked2;	  /* bit mask of spells tried and worked */
	u32b spell_forgotten1;	  /* bit mask of spells learned but forgotten */
	u32b spell_forgotten2;	  /* bit mask of spells learned but forgotten */
	byte spell_order[64];	  /* order spells learned/remembered/forgotten */

	s16b spell_exp[64];       /* Proficiency of spells */
	///del131227�@����o���l��skill_exp�Ɠ����������ߍ폜
	//s16b weapon_exp[5][64];   /* Proficiency of weapons */
	///mod131226 skill skill_exp[]�𕐊�Z�\�l�Ɠ������邽��10��21�܂Ŋg��
	//v1.1.32 ��{�I��gain_skill_exp()�ł����ω����Ȃ����I�L�i�p���[�Ō��E�𒴂��郋�[�g������
	s16b skill_exp[SKILL_EXP_MAX];       /* Proficiency of misc. skill */
	//s16b skill_exp[10];       /* Proficiency of misc. skill */

	/*:::�E�Ƃ��Ƒ��ړI�ϐ��Ƃ��Ďg���Ă���*/
	//v1.1.94 magic_num�̃T�C�Y��108��256�ɕύX
	s32b magic_num1[MAGIC_NUM_SIZE];     /* Array for non-spellbook type magic */
	byte magic_num2[MAGIC_NUM_SIZE];     /* Flags for non-spellbook type magics */

	s16b mane_spell[MAX_MANE];
	s16b mane_dam[MAX_MANE];
	s16b mane_num;

	s16b concent;      /* Sniper's concentration level */

	s16b player_hp[PY_MAX_LEVEL];
	//v1.1.33b 80��256
	char died_from[256];   	  /* What killed the player */
	cptr last_message;        /* Last message on death or retirement */
	char history[4][60];  	  /* Textual "history" for the Player */

	u16b total_winner;	  /* Total winner */
	u16b panic_save;	  /* Panic save */

	/*:::�`�[�g�t���O�H*/
	u16b noscore;		  /* Cheating flags */

	bool wait_report_score;   /* Waiting to report score */
	bool is_dead;		  /* Player is dead */

	bool wizard;		  /* Player is in wizard mode */

	s16b riding;              /* Riding on a monster of this index */
	byte knowledge;           /* Knowledge about yourself */
	s32b visit;               /* Visited towns */

	byte start_race;          /* Race at birth */
	s32b old_race1;           /* Record of race changes */
	s32b old_race2;           /* Record of race changes */
	/*:::�ߋ��g�����̈�̃r�b�g���t���O�@�u���Ȃ��͂��ā`�̈���g�����v�̃_���v�̂��߂�*/
	s16b old_realm;           /* Record of realm changes */

	/*:::�y�b�g�ւ̖��߂ɂ�遗����̐ݒ苗��*/
	s16b pet_follow_distance; /* Length of the imaginary "leash" for pets */
	/*:::�y�b�g�ւ̖��� �r�b�g���Ƃ̃t���O�ŏ���*/
	s16b pet_extra_flags;     /* Various flags for controling pets */

	///mod141231 ���ւ��܋��񂾂������A�얲�̐肢���ʂƂ��čė��p���邱�Ƃɂ���
	s16b today_mon;           /* Wanted monster */

								/*:::�g���b�v���m�ς݃G���A�ɂ��邩�ǂ���*/
	bool dtrap;               /* Whether you are on trap-safe grids */ 
	s16b floor_id;            /* Current floor location */ 

	bool autopick_autoregister; /* auto register is in-use or not */

	byte feeling;		/* Most recent dungeon feeling *//*:::�K�̕��͋C*/
	s32b feeling_turn;	/* The turn of the last dungeon feeling *//*:::���͋C���m�p�ϐ��@���̊K�ɍ~��Ă���turn���A�O�񕵈͋C�����m����turn���������Ă���͂�*/


	/*** Temporary fields ***/

	bool playing;			/* True if player is playing */
	/*:::�}�b�v���o��Ƃ��H*/
	bool leaving;			/* True if player is leaving */

	//�A���[�i����O�ɏo�Ă����t���O�@��{FALSE�����A���[�i�ŏ�����������TRUE�ɂȂ�
	byte exit_bldg;			/* Goal obtained in arena? -KMW- */

	bool leaving_dungeon;	/* True if player is leaving the dungeon */
	/*:::�X�ړ��̂Ƃ�*/
	bool teleport_town;
	bool enter_dungeon;     /* Just enter the dungeon */

	/*:::�����^�[�Q�b�g�ɂ��Ă�HP�o�[���o�Ă�mon��ID?*/
	s16b health_who;	/* Health bar trackee */

	/*:::�����^�[�Q�b�g�ɂ��Ă�mon��ID health_who��蒷���ێ������炵��*/
	s16b monster_race_idx;	/* Monster race trackee */

	s16b object_kind_idx;	/* Object kind trackee */

	/*:::�K���\���@��*/
	s16b new_spells;	/* Number of spells available */
	s16b old_spells;

	s16b old_food_aux;	/* Old value of food */

	/*:::�Z�A�O���[�u��MP�������ǂ��ω����������肷�邽�߂̈ꎞ�l*/
	bool old_cumber_armor;
	bool old_cumber_glove;
	
	bool old_heavy_wield[2];
	//bool old_heavy_shoot;
	bool old_icky_wield[2];
	bool old_riding_wield[2];
	bool old_riding_ryoute;
	bool old_monlite;


	s16b old_lite;		/* Old radius of lite (if any) */

	int heavy_cast; //cumber_armor����ύX�@�R�X�g���㏸����悤�ɂ���
	//bool cumber_armor;	/* Mana draining armor */
	bool cumber_glove;	/* Mana draining gloves */
	bool heavy_wield[2];	/* Heavy weapon */
	//bool heavy_shoot;	/* Heavy shooter */
	byte shoot_penalty; //�ˌ��y�i���e�B
	byte old_shoot_penalty; //�ˌ��y�i���e�B
	bool icky_wield[2];	/* Icky weapon */
	bool riding_wield[2];	/* Riding weapon */
	bool riding_ryoute;	/* Riding weapon */
	/*:::�����X�^�[�ɂ���ďƂ炳��Ă����ԁ@�E�҂̒��B���Ɋ֌W*/
	bool monlite;

	s16b cur_lite;		/* Radius of lite (if any) */


	u32b notice;		/* Special Updates (bit flags) */
	u32b update;		/* Pending Updates (bit flags) */
	/*:::�ĕ`��̑Ώۂɂ�����́@�^�[�Q�b�gmon��HP�o�[�ȂǗl�X�ȃt���O������*/
	u32b redraw;		/* Normal Redraws (bit flags) */
	u32b window;		/* Window Redraws (bit flags) */

	s16b stat_use[6];	/* Current modified stats */
	s16b stat_top[6];	/* Maximal modified stats */

	///class ���p�Ɨp�ϐ�
	/*:::���p�Ɓ@process_player()�Ł��̔Ԃ�����Ă����烊�Z�b�g�����*/
	bool sutemi;
	bool counter;

	///sysdel �P�Ǔx�@�폜�\��
	s32b align;				/* Good/evil/neutral */
	s16b run_py;
	s16b run_px;


	/*** Extracted fields ***/

	u32b total_weight;	/* Total weight being carried */

	/*:::�푰�E�Ɛ��i�����Ȃǂɂ������Z�l*/
	s16b stat_add[6];	/* Modifiers to stat values */
	s16b stat_ind[6];	/* Indexes into stat tables */

	bool immune_acid;	/* Immunity to acid */
	bool immune_elec;	/* Immunity to lightning */
	bool immune_fire;	/* Immunity to fire */
	bool immune_cold;	/* Immunity to cold */

	bool resist_acid;	/* Resist acid */
	bool resist_elec;	/* Resist lightning */
	bool resist_fire;	/* Resist fire */
	bool resist_cold;	/* Resist cold */
	bool resist_pois;	/* Resist poison */

	bool resist_conf;	/* Resist confusion */
	bool resist_sound;	/* Resist sound */
	bool resist_lite;	/* Resist light */
	bool resist_dark;	/* Resist darkness */
	bool resist_chaos;	/* Resist chaos */
	bool resist_disen;	/* Resist disenchant */
	bool resist_shard;	/* Resist shards */
	///del131228 ���ʍ����ϐ��폜
	//bool resist_nexus;	/* Resist nexus */
	bool resist_blind;	/* Resist blindness */
	bool resist_neth;	/* Resist nether */
	bool resist_fear;	/* Resist fear */
	bool resist_time;	/* Resist time */

	bool reflect;       /* Reflect 'bolt' attacks */
	bool sh_fire;       /* Fiery 'immolation' effect */
	bool sh_elec;       /* Electric 'immolation' effect */
	bool sh_cold;       /* Cold 'immolation' effect */

	bool anti_magic;    /* Anti-magic */
	bool anti_tele;     /* Prevent teleportation */

	bool sustain_str;	/* Keep strength */
	bool sustain_int;	/* Keep intelligence */
	bool sustain_wis;	/* Keep wisdom */
	bool sustain_dex;	/* Keep dexterity */
	bool sustain_con;	/* Keep constitution */
	bool sustain_chr;	/* Keep charisma */

	u32b cursed;            /* Player is cursed */

	bool can_swim;		/* No damage falling */
	bool levitation;		/* No damage falling */
	bool lite;		/* Permanent light */
	bool free_act;		/* Never paralyzed */
	bool see_inv;		/* Can see invisible */
	bool regenerate;	/* Regenerate hit pts */
	//bool hold_life;		/* Resist life draining */

	bool telepathy;		/* Telepathy */
	bool esp_animal;
	bool esp_undead;
	bool esp_demon;
	//bool esp_orc;
	//bool esp_troll;
	//bool esp_giant;
	bool esp_dragon;
	bool esp_human;
	bool esp_evil;
	bool esp_good;
	bool esp_nonliving;
	bool esp_unique;

	bool slow_digest;	/* Slower digestion */
	bool bless_blade;	/* Blessed blade */
	bool xtra_might;	/* Extra might bow *//*:::���͎�*/
	bool impact[2];		/* Earthquake blows */
	bool pass_wall;     /* Permanent wraithform */
	bool kill_wall;
	bool dec_mana;
	bool easy_spell;
	bool heavy_spell;
	bool warning;
	bool mighty_throw;
	bool see_nocto;		/* Noctovision *//*:::�����Ȃ��Ń_���W���������ʂ�*/

	s16b to_dd[2]; /* Extra dice/sides */ /*:::���̂Ƃ���R���̃����X�n�̂݁H*/
	s16b to_ds[2];

	s16b dis_to_h[2];	/* Known bonus to hit (wield) */
	s16b dis_to_h_b;	/* Known bonus to hit (bow) */
	s16b dis_to_d[2];	/* Known bonus to dam (wield) */
	s16b dis_to_a;		/* Known bonus to ac */

	s16b dis_ac;		/* Known base ac */

	s16b to_h[2];			/* Bonus to hit (wield) */
	s16b to_h_b;			/* Bonus to hit (bow) */
	s16b to_h_m;			/* Bonus to hit (misc) */
	s16b to_d[2];			/* Bonus to dam (wield) */
	s16b to_d_m;			/* Bonus to dam (misc) */
	s16b to_a;			/* Bonus to ac */

	/*:::���@��Փx*/
	s16b to_m_chance;		/* Minusses to cast chance */

	/*:::��{�̕���𗼎莝�����Ă���Ƃ�TRUE�@���������肪��Ŋi������Ƃ�calc_bonuses()���ňꎞ�I��TRUE�ɂȂ�*/
	bool ryoute;
	/*:::�E��ōU������Ƃ�TRUE�@����A�f��܂�*/
	bool migite;
	/*:::����ōU������Ƃ�TRUE�@����A�f��܂�*/
	bool hidarite;
	bool no_flowed;/*:::�����X�^�[��������ɂ���ĒǐՂ���Ȃ��t���O�H*/

	s16b ac;			/* Base ac */

	s16b see_infra;		/* Infravision range */

	s16b skill_dis;		/* Skill: Disarming */
	s16b skill_dev;		/* Skill: Magic Devices *//*:::������g�p*/
	s16b skill_sav;		/* Skill: Saving throw */ /*:::���@�h�� ���ʂ�100�s���Ȃ����x*/
	s16b skill_stl;		/* Skill: Stealth factor */
	s16b skill_srh;		/* Skill: Searching ability */
	/*:::�m�o�\�́@�����ƈړ������serch()����������H*/
	s16b skill_fos;		/* Skill: Searching frequency */ //�����ꂱ�����O���C�Y�Z�\�ɕύX����\��
	s16b skill_thn;		/* Skill: To hit (normal) */
	s16b skill_thb;		/* Skill: To hit (shooting) */
	s16b skill_tht;		/* Skill: To hit (throwing) */ /*:::�����Z�\�@�قڎˌ��Ɠ����l*/
	s16b skill_dig;		/* Skill: Digging */

	s16b num_blow[2];	/* Number of blows */
	/*:::�ˌ���*100*/
	s16b num_fire;		/* Number of shots */

	byte tval_xtra;		/* Correct xtra tval */

	/*:::���ݑ������Ă���|�g�ɓK�������ʂ̎��*/
	byte tval_ammo;		/* Correct ammo tval */


	///mod131228 �V�����ϐ���ꎞ���ʂ�ǉ��@�Z�[�u/���[�h��wr/rd_extra��"Future use"�̋߂���
	/*:::�ꎞ���ʈȊO�̓Z�[�u�E���[�h����K�v���Ȃ��B���[�h����calc_bonuses()�Ŏ푰�E�Ƒ�������v�Z����邽�߁B*/
	bool resist_holy;
	bool resist_water;
	s16b tim_res_water;
	bool resist_insanity;
	bool speedster;
	bool esp_deity;
	bool esp_kwai;

	s16b nennbaku;//����̈�̔O��

	s16b tim_res_chaos;
	s16b tim_superstealth;
	s16b tim_res_dark;
	s16b tim_unite_darkness;

	///mod140222 �����ꎞ���ʂ���ꂽ���Ƃ��p�ɒǉ�
	/*:::�E�Ƃ��Ɣėp�J�E���g reset_tim_flags()�Ń��Z�b�g�@��{�I�ɖ��͏����ΏۂɂȂ�Ȃ�*/
	s16b tim_general[TIM_GENERAL_MAX];

	///mod140324 �ꎞ�\�͑����p�J�E���g�A�㏸�l (�㏸�l��100�𒴂����(�㏸�l-100)�ɂȂ邪40�̌��E�𒴂���悤�ɂ���)
	s16b tim_addstat_count[6];
	s16b tim_addstat_num[6];
	//�����̈�̔�����
	s16b reactive_heal;

	u32b bydo;//�o�C�h�זE�N�H MUT3_BYDO������ƃJ�E���g�������A�R����Q�[���I�[�o�[
	s16b radar_sense;//�ϗe�̈�@���[�_�[�Z���X

	u32b grassroots; //���̍��l�b�g���[�N�������� �� �A�C�e���������J�n����turn

	//���̍��l�b�g���[�N�Ō������̃A�C�e���̉��i
	///mod141231 �얲�̐肢�̓���ϐ��Ƃ��Ă��g�����Ƃɂ���
	s32b barter_value; 

	bool drowning; //�M��Ă�����

	///mod140406 �|�̗��葕���Ƃ��S�E�i�������Ƃ��Ŋi�����肪��₱�����Ȃ����̂Ńt���O�������B
	bool do_martialarts;

	s16b silent_floor;//�I�[�v���p���h���́u�G��ق点��v����
	s32b maripo;
	u32b maripo_presented_flag;

	///mod140502
	s16b lucky;//�\���̈�u�K�^�̃��J�j�Y���v
	s16b foresight;//�\���̈�u�����\�m�v
	s16b deportation;//�����̈�u���ҏp�v
	s16b tim_res_insanity;

	s16b tim_speedster;
	s16b clawextend;//�ϗe�̈�@�܋���

	/*:::�j�ח̈�̐_�~�낵�@�t���O�ƃJ�E���g*/
	u32b kamioroshi; 
	s16b kamioroshi_count;


	byte pspeed;		/* Current speed */

	//���ǉ��\��̓�����ʂ̕ϐ� save��load�ɂ͔��f�ς�

	s16b tim_res_holy;
	/*:::�ϗe�̈�̃h���S���ω��r�����̃��x��+�m�\+�����@�ő�150 �ϗe�n�C��+20 �h���S���̃p�����[�^�ɉe�� */
	s16b mimic_dragon_rank;

	/*:::�b���x*/
	s16b asthma;


	/*:::�푰�֌W�̑��ړI�ϐ� ���̒l�̓N�C�b�N�X�^�[�g�ɕۑ�����Ȃ��̂ŕۑ���������Α��ɋL�^����K�v������*/
	//��ǐ_�l�̑����l�A�d���̓��Z�A�S�[����/�d���l�`/���ւ̔j���^�C���J�E���g
	//������̏�������
	s16b race_multipur_val[5];

	s16b monkey_count ;//��p���@�u���̎�v�g�p�񐔃J�E���g

	//v1.1.56 �A�C�e���u�X�y���J�[�h�v�ɂ��ꎞ���ʂ��J�E���g����̈�
	s16b tim_spellcard_count;	//v1.1.56 �X�y���J�[�h�̃^�C���J�E���^
	s16b spellcard_effect_idx;	//v1.1.56 �X�y���J�[�h�̔���IDX

	//�����X�^�[�ϐg����r_idx
	s16b metamor_r_idx;

	//�A�r���e�B�J�[�h���i�̍����̓x���� ����10,�ŏI�I��300�`1000���炢�ŗ������H
	s16b abilitycard_price_rate;

	//v1.1.88 �������̈ꎞ���B���̃^�C�v(���ʁA���w���ʁA�X�B��Ȃ�)���L�^����
	s16b superstealth_type;

	//v1.1.93 �ꎞ����
	s16b tim_aggravation;

	//v2.0.1 �Ԕ؊�̃A�C�e���T�����Z(�A�r���e�B�J�[�h�ł��g��)
	s16b tim_rokuro_head_search;
	//v2.0.1 �A�r���e�B�J�[�h�̂Ђ��z�̗L������(���ׂ̓��Z�̂Ђ��z�ł͂Ȃ�)
	s16b tim_hirari_nuno;


	//v1.1.81 �����̊g���p�ɕϐ��𑝂₵�Ă���
	s16b future_use_counter7;
	s16b future_use_counter8;

	u32b animal_ghost_align_flag; //v1.1.91
	u32b quest_special_flag; //v1.1.98 �ꕔ�N�G�X�g���ǂ�ȏ�ԂŐi�s�������L�^����
	u32b ptype_new_flags3;
	u32b ptype_new_flags4;

	s16b future_use_s16b[16];
	s32b future_use_s32b[8];


};


/*
 * A structure to hold "rolled" information
 */
typedef struct birther birther;

/*:::�N�C�b�N�X�^�[�g�p�ɕێ������L�����N�^�[���C�L���O�f�[�^*/
struct birther
{
	byte psex;         /* Sex index */
	byte prace;        /* Race index */
	byte pclass;       /* Class index */
	byte pseikaku;     /* Seikaku index */
	byte realm1;       /* First magic realm */
	byte realm2;       /* Second magic realm */

	s16b age;
	s16b ht;
	s16b wt;

	/*:::�Љ�I�n�ʁ@�`�[�g�I�v�V������EASY�ŃR���e�B�j���[�����Ƃ�0�ɂȂ�*/
	///mod150315 �����̒l��d���̓���\�͗p�ɕێ����邱�Ƃɂ���
	s16b sc;

	s32b au;

	s16b stat_max[6];	/* Current "maximal" stat values */
	/*:::��{�p�����[�^�ő�l 18/70�`18/130*/
	s16b stat_max_max[6];	/* Maximal "maximal" stat values */
	s16b player_hp[PY_MAX_LEVEL];

	s16b chaos_patron;

	s16b vir_types[8];

	char history[4][60];

	///mod140307 ��Փx�ύX
	byte difficulty;

	bool quick_ok;
};


/* For Monk martial arts */
#if 0
typedef struct martial_arts martial_arts;

struct martial_arts
{
	cptr    desc;       /* A verbose attack description */
	int     min_level;  /* Minimum level to use */
	int     chance;     /* Chance of 'success' */
	int     dd;         /* Damage dice */
	int     ds;         /* Damage sides */
	int     effect;     /* Special effects */
};
#endif

typedef struct martial_arts_new martial_arts_new;

struct martial_arts_new
{
	cptr    desc;       /* A verbose attack description */
	int		method;		/*:::�U���̕��ށ@���O�Ɍ��߂�ꂽ���ނ̋Z�̒����炵���I�΂�Ȃ�*/
	int     min_level;  /* Minimum level to use */
	int     chance;     /* Chance of 'success' */
	int     dd;         /* Damage dice */
	int     ds;         /* Damage sides */
	int     effect;     /* Special effects */
	int		weight;     /*:::�d�� �����̒l��@�̃��x�����|�����l���N���e�B�J������Ɏg����@2�`20���x*/
};




typedef struct kamae kamae;

struct kamae
{
	cptr    desc;       /* A verbose kamae description */
	int     min_level;  /* Minimum level to use */
	cptr    info;
};

/* Mindcrafters */
typedef struct mind_type mind_type;
struct mind_type
{
	int     min_lev;
	int     mana_cost;
	int     fail;
	cptr    name;
};

typedef struct mind_power mind_power;
struct mind_power
{
	mind_type info[MAX_MIND_POWERS];
};

///mod140202 
/*:::����V�����ǉ�����E�Ƃ��Ƃ̋Z�̓��C�V�����܂߂đS�Ă�����g���\��B*/
/*:::�u�Z�v�͉p���power�Ȃ̂�skill�Ȃ̂�technique�Ȃ̂�arts�Ȃ̂��悭������Ȃ��B*/
typedef struct class_power_type class_power_type;
struct class_power_type
{
	int min_lev; //�g�p�\���x���@�z��̍Ō�ɂ͂�����99�ɂ�����G���g��������
	int cost;
	int fail;
	bool use_hp; //HP�������Ƃ�TRUE
	bool is_magic; //���@�̎�TRUE ���@��x�����Ȃǂ̌��ʂ��󂯂邪�����@�̂Ƃ��g���Ȃ��Ȃ�
	byte stat; //���s���̎Q�ƂɂȂ�p�����[�^ A_STR�`A_CHR
	byte heavy_armor_fail; //�������d�����̎��s�����Z
	byte lev_inc_cost; //���x���㏸���Ƃɑ�����R�X�g*10�@���x���㏸�ňЗ͂����ˏオ����Z�Ȃǂ�
	cptr name;
	cptr tips; //b�R�}���h�œǂ߂������
};




/* Imitator */

typedef struct monster_power monster_power;
struct monster_power
{
	int     level;
	int     smana;
	int     fail;
	int     manedam;
	int     manefail;
	int     use_stat;
	cptr    name;
};


/*
 * A structure to describe a building.
 * From Kamband
 */
 /*:::������\��*/
typedef struct building_type building_type;

struct building_type
{
	///mod140821 ������14�܂ł�����悤�ɂ���
	char name[30];                  /* proprietor name */
	char owner_name[30];            /* proprietor name */
	char owner_race[30];            /* proprietor race */

	char act_names[8][30];          /* action names */
	s32b member_costs[8];           /* Costs for class members of building */
	s32b other_costs[8];		    /* Costs for nonguild members */
	char letters[8];                /* action letters */
	s16b actions[8];                /* action codes */
	s16b action_restr[8];           /* action restrictions */

	s16b member_class[MAX_CLASS];   /* which classes are part of guild */
	s16b member_race[MAX_RACES];    /* which classes are part of guild */
	s16b member_realm[MAX_MAGIC+1]; /* which realms are part of guild */
};


/* Border */
typedef struct border_type border_type;
struct border_type
{
	s16b north[MAX_WID];
	s16b south[MAX_WID];
	s16b east[MAX_HGT];
	s16b west[MAX_HGT];
	s16b north_west;
	s16b north_east;
	s16b south_west;
	s16b south_east;
};


/*
 * A structure describing a wilderness area
 * with a terrain or a town
 */
typedef struct wilderness_type wilderness_type;
struct wilderness_type
{
	int         terrain;
	int         town;	/*:::�X�ԍ�*/
	int         road;
	u32b        seed;	/*:::�r��̒n�`�������_����������Ƃ��̂��߂̗����̎�H*/
	s16b        level;
	byte        entrance; /*:::�_���W�����ւ̊K�i�����邩�ǂ����B����Ȃ炻�̃_���W�����̃C���f�b�N�X���i�[*/
};


/*
 * A structure describing a town with
 * stores and buildings
 */
typedef struct town_type town_type;
struct town_type
{
	char        name[32];
	u32b        seed;      /* Seed for RNG */
	store_type	*store;    /* The stores [MAX_STORES] */
	byte        numstores;
};

/* Dungeons */
typedef struct dun_type dun_type;
struct dun_type
{
	byte min_level; /* Minimum level in the dungeon */
	byte max_level; /* Maximum dungeon level allowed */

	cptr name;      /* The name of the dungeon */
};

/*
 * Sort-array element
 */
typedef struct tag_type tag_type;

struct tag_type
{
	int     tag;
	int     index;
};

typedef bool (*monster_hook_type)(int r_idx);


/*
 * This seems like a pretty standard "typedef"
 */
typedef int (*inven_func)(object_type *);


/*
 * Semi-Portable High Score List Entry (128 bytes) -- BEN
 *
 * All fields listed below are null terminated ascii strings.
 *
 * In addition, the "number" fields are right justified, and
 * space padded, to the full available length (minus the "null").
 *
 * Note that "string comparisons" are thus valid on "pts".
 */

typedef struct high_score high_score;

struct high_score
{
	char what[8];		/* Version info (string) */

	char pts[10];		/* Total Score (number) */

	char gold[10];		/* Total Gold (number) */

	char turns[10];		/* Turns Taken (number) */

	char day[10];		/* Time stamp (string) */

	char who[16];		/* Player Name (string) */

	char uid[8];		/* Player UID (number) */

	char sex[2];		/* Player Sex (string) */
	char p_r[3];		/* Player Race (number) */
	char p_c[3];		/* Player Class (number) */
	char p_a[3];		/* Player Seikaku (number) */

	char cur_lev[4];		/* Current Player Level (number) */
	char cur_dun[4];		/* Current Dungeon Level (number) */
	char max_lev[4];		/* Max Player Level (number) */
	char max_dun[4];		/* Max Dungeon Level (number) */

	char how[40];		/* Method of death (string) */
};


typedef struct
{
	s16b feat;    /* Feature tile */
	byte percent; /* Chance of type */
}
feat_prob;


/* A structure for the != dungeon types */
typedef struct dungeon_info_type dungeon_info_type;
struct dungeon_info_type {
	u32b name;		/* Name */
	u32b text;		/* Description */

	byte dy;
	byte dx;

	/*:::�o�₷�����ƕǂ�3�܂ŏo�����ƈꏏ�ɓo�^���Ă�����炵��*/
	feat_prob floor[DUNGEON_FEAT_PROB_NUM]; /* Floor probability */
	feat_prob fill[DUNGEON_FEAT_PROB_NUM];  /* Cave wall probability */

	s16b outer_wall;                        /* Outer wall tile */
	s16b inner_wall;                        /* Inner wall tile */
	s16b stream1;                           /* stream tile */
	s16b stream2;                           /* stream tile */

	s16b mindepth;         /* Minimal depth */
	s16b maxdepth;         /* Maximal depth */
	byte min_plev;         /* Minimal plev needed to enter -- it's an anti-cheating mesure */
	s16b pit;
	s16b nest;
	byte mode;		/* Mode of combinaison of the monster flags */

	int min_m_alloc_level;	/* Minimal number of monsters per level */
	/*:::�_���W�������Ƃ̃����X�^�[pop��*/
	int max_m_alloc_chance;	/* There is a 1/max_m_alloc_chance chance per round of creating a new monster */

	/*:::�_���W�����t���O*/
	u32b flags1;		/* Flags 1 */

	/*:::�_���W�����ŏo�����郂���X�^�[�̃t���O ������ǂ̂悤�ɓK�p���邩��mode�Ō���@���ꂪ�K�p����邩�ǂ�����MONSTER_DIV_?�ɂ��*/
	u32b mflags1;		/* The monster flags that are allowed */
	u32b mflags2;
	u32b mflags3;
	u32b mflags4;
	u32b mflags5;
	u32b mflags6;
	u32b mflags7;
	u32b mflags8;
	u32b mflags9;
	u32b mflagsr;

	//v1.1.86 �z��T�C�Y��5��50��
	char r_char[DUNGEON_R_CHAR_MAX];		/* Monster race allowed */
	int final_object;	/* The object you'll find at the bottom */
	int final_artifact;	/* The artifact you'll find at the bottom */
	/*:::�_���W�����̎�̃����X�^�[ID�������Ă���炵��*/
	int final_guardian;	/* The artifact's guardian. If an artifact is specified, then it's NEEDED */

	/*:::���l�������قǏo�Ă���G�ɂ΂�����Ȃ��Hget_mon_num_prep()�Q�� 64�ȏゾ�ƃ����X�^�[�t���O�ɂ�鐧���������Ȃ��炵��*/
	byte special_div;	/* % of monsters affected by the flags/races allowed, to add some variety */
	/*:::�t���A�������ɕ������Ȃ��֐���build_tunnel()�łȂ�build_tunnel2()�ɂȂ�m���@�ڍוs��*/
	int tunnel_percent;
	/*:::apply_magic()�Ŏg����㎿�A�����i����l�̏��*/
	int obj_great;
	int obj_good;
};


/*
 *  A structure type for entry of auto-picker/destroyer
 */
typedef struct {
	/*:::�A�C�e���� ������v����*/
	cptr name;          /* Items which have 'name' as part of its name match */
	/*:::���������A�C�e���ɂǂ�Ȗ������ނ�*/
	cptr insc;          /* Items will be auto-inscribed as 'insc' */
	/*:::�������A���A�A�N���[�N�A����ڂȂǏ�������ɂ������t���O�S��*/
	u32b flag[2];       /* Misc. keyword to be matched */
	/*:::�E�����󂷂����u���邩�A�S�̃}�b�v�ɕ\�����邩�Ȃǂ̃t���O*/
	byte action;        /* Auto-pickup or Destroy or Leave items */
	/*:::����̃_�C�X�ڎw��@�f�t�H��0*/
	byte dice;          /* Weapons which have more than 'dice' dice match */
	/*:::����h��̃{�[�i�X�w��?*/
	byte bonus;         /* Items which have more than 'bonus' magical bonus match */
} autopick_type;


/*
 *  A structure type for the saved floor
 */
typedef struct 
{
	/*:::p_ptr->floor_id�ɂ͂����̒l������*/
	s16b floor_id;        /* No recycle until 65536 IDs are all used */

	byte savefile_id;     /* ID for savefile (from 0 to MAX_SAVED_FLOOR) */
	s16b dun_level;
	s32b last_visit;      /* Time count of last visit. 0 for new floor. */
	u32b visit_mark;      /* Older has always smaller mark. */
	s16b upper_floor_id;  /* a floor connected with level teleportation */
	s16b lower_floor_id;  /* a floor connected with level tel. and trap door */
} saved_floor_type;


/*
 *  A structure type for terrain template of saving dungeon floor
 */
typedef struct
{
	u16b info;
	s16b feat;
	s16b mimic;
	s16b special;
	u16b occurrence;
	//v1.1.33 ���A�p�t���O�ǉ�
	u32b cave_xtra_flag;
	s16b cave_xtra1;
	u16b cave_xtra2;

} cave_template_type;


/*
 * A structure type for arena entry
 */
typedef struct
{
	s16b r_idx; /* Monster (0 means victory prizing) */
	byte tval;  /* tval of prize (0 means no prize) */
	byte sval;  /* sval of prize */
} arena_type;


/*
 * A structure type for doors
 */
typedef struct
{
	s16b open;
	s16b broken;
	s16b closed;
	s16b locked[MAX_LJ_DOORS];
	s16b num_locked;
	s16b jammed[MAX_LJ_DOORS];
	s16b num_jammed;
} door_type;


#ifdef TRAVEL
/*
 *  A structure type for travel command
 */
typedef struct {
	int run; /* Remaining grid number */
	int cost[MAX_HGT][MAX_WID];
	int x; /* Target X */
	int y; /* Target Y */
	int dir; /* Running direction */
} travel_type;
#endif

typedef struct {
	cptr flag;
	byte index;
	byte level;
	s32b value;
	struct {
		int constant;
		int dice;
	} timeout;
	cptr desc;
} activation_type;

typedef struct {
	int flag;
	int type;
	cptr name;
} dragonbreath_type;


///mod140306 �푰�A�E�ƁA���ʂ��Ƃ̃Q�[���J�n�������A�C�e��
typedef struct outfit_type outfit_type;

struct outfit_type
{
	int type; //�푰�A�E��ID
	int psex; //���� 0:�������� 1:�j������ ����ȊO�F�j������
	int artifact_idx; //�Œ�A�[�e�B�t�@�N�g�������ĊJ�n����ꍇa_info��idx �ʏ�0 IDX������ꍇtval,sval,num�͖��������
	int tval; //�A�C�e����tval
	int sval; //�A�C�e����sval
	int num;  //�A�C�e���̌�
};


/*:::�_���W�������Y�i�e�[�u��*/
typedef struct souvenir_type souvenir_type;
struct souvenir_type
{
	int dun_num;//�����_���W����
	int tval;
	int sval;
	int min_lev;//�����Œ�K�w
	int max_lev;//�����ő�K�w
	int chance;//��{������ 0.01%�P��
	int max_num;//�������ꂽ�Ƃ��ꂩ���Ō�����ő吔
};


typedef struct marisa_store_type marisa_store_type;
struct marisa_store_type
{
	int tval;
	int sval;
	int maripo;
};

///mod140429 �A�[�`���[�M���h�p
typedef struct guild_arts_type guild_arts_type;
struct guild_arts_type
{
	int tval;
	int sval;
	int type;
	int num;
	cptr name;
};

///mod140819 �G���W�j�A�֌W
/*:::�@�B�d�l�e�[�u���@�Y������SVAL�Ɉ�v����*/
typedef struct machine_type machine_type;
struct machine_type
{
	int difficulty;//�쐬��x�i���x��+�m�\+��p)
	int tval; //�R��tval(0�͎g���̂�)
	int sval; //�R��sval
	int charge;//��{�g�p�񐔁@����pval�ɓ������g�p���邽�т�1����@�R���⋋����Ƃ��̒l�ɖ߂�
	int hate_water;//���U�����󂯂��Ƃ��g�p�s�\�ɂȂ�m�� 0�`100 �_���[�W�Ŋm������
	int hate_elec;//�d���U�����󂯂��Ƃ��g�p�s�\�ɂȂ�m�� 
};


/*:::���A���̂܂ˌn�p�����X�^�[���@�V���X�g�̂��߂̌^*/
/*:::�����̃��x�����Փx�Ȃǂ̊e���ڂ͐E�ɂ���Ă͖��������*/
typedef struct monspells2 monspells2;
struct monspells2
{
	int     level; //��{�g�p�\���x��
	int     smana; //��{����MP
	int     fail; //��{��Փx
	int     use_stat; //��Փx�Ɋ֌W����X�e�[�^�X
	bool	use_words; //���q�̃I�E���Ԃ����\���ǂ����@�����̂ق����т⍌���u���X���܂܂��
	bool	attack_spell; //�U�����@�@����p�t���O
	int		priority; //�����X�^�[�ϐg�̂Ƃ��Z�̑��������X�^�[�͑S�����X�g�ɍڂ����Ȃ��̂ŗv��Ȃ����Ȃ̂���邽�߂̗D�揇�ʁ@���Ȃ��قǏ�����ɂ�����0�͍ڂ�Ȃ�
	cptr    name; //������
	//v1.1.57�ǉ�
	byte monspell_type; //���@���{���g�E�{�[���E�u���X�Ȃǂ̕��ނ��ƂɑI�����邽�߂̕��ޒl NEW_MSPELL_TYPE_***�ɑΉ� �����ނ�0

};

/*:::��ǐ_�l�p*/
typedef struct deity_type deity_type;
struct deity_type
{
	s16b	rank;	//0:�I�[�p�_�~�[ 1:���� 2:Lev30 3:Lev45
	cptr	deity_name;	//�_�l�̎�ޖ�
	s16b	dbias_cosmos; //�����x�@0-255
	s16b	dbias_warlike; //�D��x
	s16b	dbias_reputation; //�����l�@�ŏ��݂͂ȒႢ
	s16b	stat_adj[6];	//�\�͏C���l
	s16b	r_dis;
	s16b	r_dev;
	s16b	r_sav;
	s16b	r_stl;
	s16b	r_srh;
	s16b	r_graze;
	s16b	r_thn;
	s16b	r_thb;

};

///mod140225 ��������cptr info�𑝂₵��
///mod150208 racial.c���炱���Ɏ����Ă���
typedef struct power_desc_type power_desc_type;

struct power_desc_type
{
	char name[40];
	int  level;
	int  cost;
	int  stat;
	int  fail;
	int  number;
	cptr info;
};


/*:::�d���p*/
typedef struct fairy_type fairy_type;
struct fairy_type
{
	byte	rarity;	//0-99 �d���^�C�v�I�莞�Arandint0(100)��rarity�����������Ē��I�����B�����炱�̐��l�����̂܂܊m���ɂȂ�킯�ł͂Ȃ��B
	cptr	fairy_desc;//����������x�̔\��
};

/*:::�V���Z���p����`�[�����X�g*/
typedef struct battle_mon_special_team_type battle_mon_special_team_type;
struct battle_mon_special_team_type
{
	int level;	 //�Œᐶ�����x��
	cptr 	name; //�`�[����
	int  r_idx_list[8];
};

/*:::�V���Z���p�����X�^�[���X�g*/
typedef struct battle_mon_type battle_mon_type;
struct battle_mon_type
{
	int     r_idx; //�����X�^�[�� 0�̓_�~�[
	u32b     team; //�`�[�� MFLAG_BATTLE_MON_TEAM_A�`D
};


///mod150811 ������
struct marisa_magic_type
{
	cptr	name; //���O
	cptr	info; //������
	s16b	use_magic_power[8]; //�����ɕK�v�Ȗ��͂̎�ނƗ�
	bool	is_drug; //�򒲍��̂Ƃ�TRUE ���b�Z�[�W�ɉe��	

};


/*:::���ܑf�ރe�[�u��*/
typedef struct drug_material_type drug_material_type;
struct drug_material_type
{
	byte	tval; //�f�ނ�tval��sval
	byte	sval;

	int	cost; //���܎����؂��̂ɕK�v�ȋ��z

	//�f�ނ̎������l
	int	elem[8];
};


/*:::���܃e�[�u��*/
typedef struct drug_compound_type drug_compound_type;
struct drug_compound_type
{
	byte	tval; //���������i��tval��sval
	byte	sval;

	//�����SVAL�̖{�����L���Ă���Ɠ�Փx��������H
	byte	difficulty;

	//���̖򂪐�������邽�߂̑f�ލ��v�K�v�����l�@
	//(���̂Ƃ��͐��l�ȉ��̒l�ɂȂ�K�v������B�����l�𖞂����҂���������ꍇ�͓���K�v�f�ނ̐ݒ肳��Ă�����́A�\�̉��ɂ��镨���D�悳���)
	int	elem[8];

	//�@�ޗ��R�X�g�����̃R�X�g��傫��������ƕ�����������邱�Ƃ�����B
	int		base_cost;
	//��̍ޗ��R�X�g�𒴉߂������̍ő吶����
	int		max_num;

	//����A�C�e���͂��̑f�ނ��܂܂�Ȃ��Ƃ����Ȃ�?
	byte	special_material_tval;
	byte	special_material_sval;
};

/*:::��`���e�[�u��*/
typedef struct drug_recipe_type drug_recipe_type;
struct drug_recipe_type
{
	byte	tval; //�����i��tval��sval
	byte	sval;
	
	//�f��1�`3�Ƃ��ĕ\�������tv,sv 
	byte	mat1_tv;
	byte	mat1_sv;
	byte	mat2_tv;
	byte	mat2_sv;
	byte	mat3_tv;
	byte	mat3_sv;

};



/*:::EXTRA���[�h�p�T�|�[�g�A�C�e�����X�g*/
typedef struct support_item_type support_item_type;
struct support_item_type
{
	byte	rarity;		//���Ȃ��قǏo�₷�� 0�̂Ƃ��I�[�_�~�[
	byte	min_lev; 	//��{�����J�n�K�@�������O���Ɛ�������������
	byte	max_lev; 	//��{�����I���K�@�������ゾ�Ɛ�������������
	byte	prod_num;//��b���������@1�̂��̂͏��1
	byte	cost; //�A�C�e�������̂Ƃ��̉��l

	s16b	r_idx;	 	//����idx�̓G��|�����Ƃ����肵�₷�� 0�̂Ƃ�����
	class_power_type *func_table; //���̃A�C�e�����Q�Ƃ��郆�j�[�N�N���X���Z�e�[�u���̃A�h���X�@�s�v��������񂯂ǈꉞ����Ƃ�
	cptr	(*func_aux)(int,bool); //���̃A�C�e���Ŏg�p���郆�j�[�N�N���X���Z���s�֐��ւ̃|�C���^�@���@����ō����Ă�̂����M�Ȃ�
	byte	func_num;	//func_table�̗v�f�ԍ��@���j�[�N�N���X�̓��Z�ύX���ɏ��Ԃ��ւ�����炱���̔ԍ���ς��Y��Ȃ��悤���ӂ��Ȃ��ƍň��o�O��B
	cptr	name; 		//�A�C�e�����@object_desc����Q�Ƃ���B
	cptr	infotxt;//������
};

/*:::�G�N�X�g���N�G�X�g��V�e�[�u��*/
typedef struct ex_quest_rew_type ex_quest_rew_type;
struct ex_quest_rew_type
{
	byte	level; //�N�G�X�g�t���A�K 0�͏I�[�_�~�[
	byte	a_idx; //���̂Ƃ��̃C���f�b�N�X
	byte	tval; //��V��tv,sv
	byte	sval;
	u32b	am_mode; //apply_magic()������Ƃ��̃��[�h
	byte 	num;	//��


};

/*:::�e��b�p�����[�^�e�[�u��*/
//�z��Y�����e��sval�ɑΉ�
//ACT_GUN_VARIABLE�ɂ��ˌ��ɂ̂ݎg���B
typedef struct gun_base_param_type gun_base_param_type;
struct gun_base_param_type
{

	byte	gf_type;//�ˌ������Ƃ��̊�{���� object_prep()��ʂ�����xtra1�Ɋi�[�����

	//�ˌ������Ƃ��̍U������ GUN_FIRE_MODE_*�ɑΉ� xtra4�Ɋi�[
	byte	project_type;

	//��b�[�U�^�[����
	s16b	charge_turn;

	//���e��
	byte bullets;

};


/*:::��p�u�يE�Ƃ̐ڐG�v�̃����X�^�[�e�[�u��*/
typedef struct occult_contact_type occult_contact_type;
struct occult_contact_type
{
	byte	featflag1; //�n�`FF_*�Ɉ�v
	byte	featflag2; //�n�`FF_*�Ɉ�v
	byte	featflag3; //�n�`FF_*�Ɉ�v���Ȃ�
	s16b	r_idx;	//���������G
	cptr	feat_desc; //�n�`�\�L

};

//v1.1.17 ���ς�essence_info���g�����߂�object2.c����ړ�
/*
 *  A structure for smithing
 */
typedef struct essence_type essence_type;
struct essence_type
{
	int add;       /* TR flag number or special essence id */
	cptr add_name; /* Name of this ability */
	int type;      /* Menu number */
	int essence;   /* Index for carrying essences */
	int value;     /* Needed value to add this ability */
};

/*:::���m�p���Z���X�g*/
typedef struct soldier_skill_type soldier_skill_type;
struct soldier_skill_type
{
	byte	lev;	//�K���\���x���H�s�v����
	cptr	name;	//���Z��
	cptr	info;	//�������@\�ŉ��s

};


//v1.1.22
//���m�̒e�ۊ����e�[�u���p
//�z��Y����p_ptr->magic_num1[]�̓Y����SSM_***�Ɉ�v���A�c�e���̊Ǘ��Ɏg����B
typedef struct soldier_bullet_type soldier_bullet_type;
struct soldier_bullet_type
{
	int 	cost;		//���̒e���ˌ������Ƃ��̏���MP
	cptr	name;		//�e�e��
	cptr	info; 		//������ \\�ŉ��s
	byte	magic_bullet_lev;	//�u�e�e�ύX�v�Ŏg���Ƃ��A�ǂ̋Z�\���x������g���邩

};

//v1.1.22
//���m�̒e�ۉ��H�p
typedef struct soldier_bullet_making_type soldier_bullet_making_type;
struct soldier_bullet_making_type
{
	byte	shooting_mode;	//SSM_***�Ɉ�v 0�͏I�[�_�~�[
	byte	mat_tval; 	//���̒e�����ޗ���tv,sv
	byte	mat_sval;
	byte	prod_num; 	//�f�ވ������̊�{���Y��

};


//v1.1.24
//�}������N�G�X�g���ʌ���p�̃X�R�A���X�g
typedef struct torrent_quest_score_type torrent_quest_score_type;
struct torrent_quest_score_type
{
	int		turn;
	s16b	class_id;
	s16b	class_id2;
	cptr	name;

};


//v1.1.51	�u���̐��E�ɍs��(��)�v�R�}���h�Ō��閲�̑I����
typedef struct nightmare_stage_type nightmare_stage_type;
struct nightmare_stage_type
{
	cptr desc;			//�X�e�[�W��
	u32b need_points;	//���̃X�e�[�W���I�����ɕ\������邽�߂ɕK�v�ȍ��v�|�C���g��
	byte level;			//�o�������X�^�[�̊�{���x��
	u32b min_points;	//���̃X�e�[�W�œ�����|�C���g��min/max����O�ꂽ�烂���X�^�[�I���蒼��
	u32b max_points;

};

//v1.1.51	�u���̐��E�ɍs��(��)�v�R�}���h�ō����X�y�J���Ɏg���镶����
typedef struct nightmare_spellname_type nightmare_spellname_type;
struct nightmare_spellname_type
{
	int r_idx;		//�o�^���ꂽr_idx������ƈ�v���镶���񂪑I�΂��B0���������I�[�_�~�[���K�v
	int class_idx;	//�������j�[�N�N���X�Ŗ��̐��E�́�(�����_�����j�[�N)��|�����Ƃ�����idx�ɍ������̂��I�΂��
	cptr conj1;	//�O�ɂ��ڑ��� ��Ȃ�NULL
	cptr phrase;	//������{��
	cptr conj2;	//��ɂ��ڑ����@��Ȃ�NULL
	byte position;	//���̐������傫���قǑO�ɗ���B���������X�y�J���͂��̌W����10�����̕���������Ȃ��Ƃ���܂܂Ȃ��Ƃ����Ȃ�

};



//v1.1.51	�V�A���[�i�̒n�`�����Ɏg���X�t�@�C�����Ɨ��p�J�n�ʒuxy���W
typedef struct nightmare_gen_map_type nightmare_gen_map_type;
struct nightmare_gen_map_type
{
	cptr	townfilename; //�X�t�@�C���� t0000001.txt�@�Ȃ�
	byte	x;	//���p�J�n����|�C���g(����)
	byte	y;

};

//v1.1.51 �V�A���[�i�p�ܕi�e�[�u��
typedef struct
{
	byte nightmare_level; //0�`7
	byte rarity;//���̒l�ŏo��������z������B�傫���قǏo�₷���B�z��̍Ō��rarity=0�̏I�[�_�~�[������
	byte tval;
	byte sval;
	byte num; //��{������
} nightmare_treasure_type;


//v1.1.57	�B��ށu��_�̈Ö��e���v�̍U���^�C�v
typedef struct 
{
	byte lev;	//�I���\���x���@���̍\���̂̓��x�����ɕ���ł��邱��
	cptr desc;	//�U���Ώە��ޖ�
	u32b rf3_flag;  //�U���Ώۏ���(r_ptr->flags3�ƏƉ��)
}pattern_attack_type;


//v1.1.86 �A�r���e�B�J�[�h���e�[�u��
typedef struct
{
	cptr	card_name; 	//�J�[�h�̖��O
	u16b	rarity_rank;	//�o�₷��0�`4�@0�͒ʏ�o�����Ȃ��B�����_���������ɂ�(1:80% 2:16% 3: 3.2% 4:0.8% weird_luck()�Ń����N�A�b�v)�őI�肳��� 
	u16b	trade_value;	//�������l�@�g���[�h�̂Ƃ��ɂ͑�̂��̃J�[�h��肿����ƒႢ���̂��o��BEXTRA�ł̓A�C�e���J�[�h�Ƃ̃g���[�h�ɂ��g��?
	bool	activate; 	//�����\�ȃJ�[�h�̂Ƃ�TRUE
	byte	difficulty;	//������Ƃ��Ă̔�����Փx
	u16b	charge_turn;	//�`���[�W���� �ő�300�܂�(99���X�^�b�N�����Ƃ���object_type.timeout�l��32767�ȓ��łȂ��ƃI�[�o�[�t���[����)
	u16b	r_idx;		//���̃J�[�h�������Ă��郂���X�^�[

} ability_card_type;






