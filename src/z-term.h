/* File: z-term.h */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

#ifndef INCLUDED_Z_TERM_H
#define INCLUDED_Z_TERM_H

#include "h-basic.h"


/*
 * A term_win is a "window" for a Term
 *
 *	- Cursor Useless/Visible codes
 *	- Cursor Location (see "Useless")
 *
 *	- Array[h] -- Access to the attribute array
 *	- Array[h] -- Access to the character array
 *
 *	- Array[h*w] -- Attribute array
 *	- Array[h*w] -- Character array
 *
 * Note that the attr/char pair at (x,y) is a[y][x]/c[y][x]
 * and that the row of attr/chars at (0,y) is a[y]/c[y]
 */

typedef struct term_win term_win;

struct term_win
{
	bool cu, cv;
	byte cx, cy;

	byte **a;
	char **c;

	byte *va;
	char *vc;

	byte **ta;
	char **tc;

	byte *vta;
	char *vtc;
};



/*
 * An actual "term" structure
 *
 *	- Extra "user" info (used by application)
 *
 *	- Extra "data" info (used by implementation)
 *
 *
 *	- Flag "user_flag"
 *	  An extra "user" flag (used by application)
 *
 *
 *	- Flag "data_flag"
 *	  An extra "data" flag (used by implementation)
 *
 *
 *	- Flag "active_flag"
 *	  This "term" is "active"
 *
 *	- Flag "mapped_flag"
 *	  This "term" is "mapped"
 *
 *	- Flag "total_erase"
 *	  This "term" should be fully erased
 *
 *	- Flag "fixed_shape"
 *	  This "term" is not allowed to resize
 *
 *	- Flag "icky_corner"
 *	  This "term" has an "icky" corner grid
 *
 *	- Flag "soft_cursor"
 *	  This "term" uses a "software" cursor
 *
 *	- Flag "always_pict"
 *	  Use the "Term_pict()" routine for all text
 *
 *	- Flag "higher_pict"
 *	  Use the "Term_pict()" routine for special text
 *
 *	- Flag "always_text"
 *	  Use the "Term_text()" routine for invisible text
 *
 *	- Flag "unused_flag"
 *	  Reserved for future use
 *
 *	- Flag "never_bored"
 *	  Never call the "TERM_XTRA_BORED" action
 *
 *	- Flag "never_frosh"
 *	  Never call the "TERM_XTRA_FROSH" action
 *
 *
 *	- Value "attr_blank"
 *	  Use this "attr" value for "blank" grids
 *
 *	- Value "char_blank"
 *	  Use this "char" value for "blank" grids
 *
 *
 *	- Ignore this pointer
 *
 *	- Keypress Queue -- various data
 *
 *	- Keypress Queue -- pending keys
 *
 *
 *	- Window Width (max 255)
 *	- Window Height (max 255)
 *
 *	- Minimum modified row
 *	- Maximum modified row
 *
 *	- Minimum modified column (per row)
 *	- Maximum modified column (per row)
 *
 *
 *	- Displayed screen image
 *	- Requested screen image
 *
 *	- Temporary screen image
 *	- Memorized screen image
 *
 *
 *	- Hook for init-ing the term
 *	- Hook for nuke-ing the term
 *
 *	- Hook for user actions
 *
 *	- Hook for extra actions
 *
 *	- Hook for placing the cursor
 *
 *	- Hook for drawing some blank spaces
 *
 *	- Hook for drawing a string of chars using an attr
 *
 *	- Hook for drawing a sequence of special attr/char pairs
 */

typedef struct term term;

struct term
{
	/*:::システムが使うらしい*/
	vptr user;
	vptr data;
	bool user_flag;
	bool data_flag;
	/*:::このTermはアクティブ*/
	bool active_flag;
	/*:::マップされているフラグ・・？*/
	bool mapped_flag;
	/*:::「このTermは完全削除の最中」フラグ*/
	bool total_erase;
	/*:::ウィンドウサイズ変更不可フラグ？*/
	bool fixed_shape;
	/*:::詳細不明*/
	bool icky_corner;
	bool soft_cursor;
	/*:::Term_pict()を使うフラグ？*/
	bool always_pict;
	bool higher_pict;
	/*:::Term_text()を使うフラグ？*/
	bool always_text;
	/*:::予備フラグ*/
	bool unused_flag;
	/*:::TERM_XTRA_BOREDが使われなくなる？*/
	bool never_bored;
	/*:::TERM_XTRA_FROSHが使われなくなる？*/
	bool never_frosh;
	/*:::空のマスにここの数値の記号？を入れる*/
	byte attr_blank;
	/*:::空のマスにここの数値の文字？を入れる*/
	char char_blank;

	/*:::Keypress Queue？*/
	char *key_queue;
	u16b key_head;
	u16b key_tail;
	u16b key_xtra;
	u16b key_size;

	/*:::WindowSize max255*/
	byte wid;
	byte hgt;

	/*:::modified row? 1:min 2:max*/
	byte y1;
	byte y2;

	byte *x1;
	byte *x2;

	/*:::表示情報を保持？*/
	term_win *old;
	/*:::新しく描画する情報を保持？*/
	term_win *scr;

	term_win *tmp;
	term_win *mem;

	/*:::機種依存対応用の関数ポインタ？
	  :::main-win.cで関数指定されてずっと使われているようだ
	  :::*/

	void (*init_hook)(term *t);
	void (*nuke_hook)(term *t);

	errr (*user_hook)(int n);

	errr (*xtra_hook)(int n, int v);

	errr (*curs_hook)(int x, int y);

	errr (*bigcurs_hook)(int x, int y);

	errr (*wipe_hook)(int x, int y, int n);

	errr (*text_hook)(int x, int y, int n, byte a, cptr s);

	void (*resize_hook)(void);

	errr (*pict_hook)(int x, int y, int n, const byte *ap, const char *cp, const byte *tap, const char *tcp);
};







/**** Available Constants ****/


/*
 * Definitions for the "actions" of "Term_xtra()"
 *
 * These values may be used as the first parameter of "Term_xtra()",
 * with the second parameter depending on the "action" itself.  Many
 * of the actions shown below are optional on at least one platform.
 *
 * The "TERM_XTRA_EVENT" action uses "v" to "wait" for an event
 * The "TERM_XTRA_SHAPE" action uses "v" to "show" the cursor
 * The "TERM_XTRA_FROSH" action uses "v" for the index of the row
 * The "TERM_XTRA_SOUND" action uses "v" for the index of a sound
 * The "TERM_XTRA_ALIVE" action uses "v" to "activate" (or "close")
 * The "TERM_XTRA_LEVEL" action uses "v" to "resume" (or "suspend")
 * The "TERM_XTRA_DELAY" action uses "v" as a "millisecond" value
 *
 * The other actions do not need a "v" code, so "zero" is used.
 */
#define TERM_XTRA_EVENT	1	/* Process some pending events */
#define TERM_XTRA_FLUSH 2	/* Flush all pending events */
#define TERM_XTRA_CLEAR 3	/* Clear the entire window */
#define TERM_XTRA_SHAPE 4	/* Set cursor shape (optional) */
#define TERM_XTRA_FROSH 5	/* Flush one row (optional) */
#define TERM_XTRA_FRESH 6	/* Flush all rows (optional) */
#define TERM_XTRA_NOISE 7	/* Make a noise (optional) */
#define TERM_XTRA_SOUND 8	/* Make a sound (optional) */
#define TERM_XTRA_BORED 9	/* Handle stuff when bored (optional) */
#define TERM_XTRA_REACT 10	/* React to global changes (optional) */
#define TERM_XTRA_ALIVE 11	/* Change the "hard" level (optional) */
#define TERM_XTRA_LEVEL 12	/* Change the "soft" level (optional) */
#define TERM_XTRA_DELAY 13	/* Delay some milliseconds (optional) */
//v1.1.58 本家BGM機能を改変コピー
#define TERM_XTRA_MUSIC_BASIC 14   /* Play a music(basic)   (optional) */
#define TERM_XTRA_MUSIC_DUNGEON 15 /* Play a music(dungeon) (optional) */
#define TERM_XTRA_MUSIC_QUEST 16   /* Play a music(quest)   (optional) */
#define TERM_XTRA_MUSIC_TOWN 17    /* Play a music(floor)   (optional) */
#define TERM_XTRA_MUSIC_MON_L 18    /* Play a music(monster)   (optional) */
#define TERM_XTRA_MUSIC_MON_M 19    /* Play a music(monster)   (optional) */
#define TERM_XTRA_MUSIC_MON_H 20    /* Play a music(monster)   (optional) */


/**** Available Variables ****/

extern term *Term;


/**** Available Functions ****/

extern errr Term_user(int n);
extern errr Term_xtra(int n, int v);

extern void Term_queue_char(int x, int y, byte a, char c, byte ta, char tc);
extern void Term_queue_bigchar(int x, int y, byte a, char c, byte ta, char tc);

extern void Term_queue_line(int x, int y, int n, byte *a, char *c, byte *ta, char *tc);

extern void Term_queue_chars(int x, int y, int n, byte a, cptr s);

extern errr Term_fresh(void);
extern errr Term_set_cursor(int v);
extern errr Term_gotoxy(int x, int y);
extern errr Term_draw(int x, int y, byte a, char c);
extern errr Term_addch(byte a, char c);
extern errr Term_add_bigch(byte a, char c);
extern errr Term_addstr(int n, byte a, cptr s);
extern errr Term_putch(int x, int y, byte a, char c);
extern errr Term_putstr(int x, int y, int n, byte a, cptr s);
#ifdef JP
extern errr Term_putstr_v(int x, int y, int n, byte a, cptr s);
#endif
extern errr Term_erase(int x, int y, int n);
extern errr Term_clear(void);
extern errr Term_redraw(void);
extern errr Term_redraw_section(int x1, int y1, int x2, int y2);

extern errr Term_get_cursor(int *v);
extern errr Term_get_size(int *w, int *h);
extern errr Term_locate(int *x, int *y);
extern errr Term_what(int x, int y, byte *a, char *c);

extern errr Term_flush(void);
extern errr Term_keypress(int k);
extern errr Term_key_push(int k);
extern errr Term_inkey(char *ch, bool wait, bool take);

extern errr Term_save(void);
extern errr Term_load(void);

extern errr Term_exchange(void);

extern errr Term_resize(int w, int h);

extern errr Term_activate(term *t);

extern errr term_nuke(term *t);
extern errr term_init(term *t, int w, int h, int k);


#endif


