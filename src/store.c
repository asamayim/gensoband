/* File: store.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Store commands */

#include "angband.h"




#define MIN_STOCK 12

static int cur_store_num = 0;
static int store_top = 0;
static int store_bottom = 0;
static int xtra_stock = 0;
static store_type *st_ptr = NULL;
//static const owner_type *ot_ptr = NULL;
static s16b old_town_num = 0;
static s16b inner_town_num = 0;
#define RUMOR_CHANCE 8

#define MAX_COMMENT_1	4

///msg131222
/*
static cptr comment_1[MAX_COMMENT_1] =
{
#ifdef JP
	"���x���肪�Ƃ��������܂��I",
	"�������܂�܂����I",
	"����Ƃ����ۛ��ɁI",
#else
	"Okay.",
	"Accepted!",
	"Agreed!"
#endif

};
*/

#ifdef JP
/* �u���b�N�}�[�P�b�g�ǉ����b�Z�[�W�i�����j */
///msg �X�Ŕ����������Ƃ��̏����Z���t�@�����Ƃɕς���H
/*
static cptr comment_1_B[MAX_COMMENT_1] = {
	"�܂��A����ł�����B",
	"����������B",
	"����Ȃ��񂾂낤�B"
};

static cptr comment_1_C[MAX_COMMENT_1] = {
	"�ӂށB�ł͂���ŏ��k�������B",
	"�܂��̗��X�����҂����Ă����B",
	"�悵�B���x���肪�Ƃ��B"
};
*/

static cptr comment_basic_agree[MAX_COMMENT_1] =
{
	"�u���܂肾�I�v",
	"�u���x����I�v",
	"�u�悵�I�v",
	"�X��͗z�C�Ɏ��炵���B",
};
static cptr comment_bm_agree[MAX_COMMENT_1] =
{
	"�u�܂��A����ł�����B�v",
	"�u����������B�v",
	"�u����Ȃ��񂾂낤�B�v",
	"�X��͖ʓ|���������ɒ�������o�����B",
};
static cptr comment_suzuna_agree[MAX_COMMENT_1] =
{
	"�u���x���肪�Ƃ��������܂��I�v",
	"�u����Ƃ����ۛ��ɁI�v",
	"�u�������܂�܂����I�v",
	"�X��͔����Ƃ������ŗ���������B",
};
static cptr comment_korin_agree[MAX_COMMENT_1] =
{
	"�u�悵�A���k�������B�v",
	"�u�ӂށA���x���肪�Ƃ��B�v",
	"�u�܂����Ă��ꂽ�܂��B�v",
	"�X��͋C���悳�����D�~�b���n�߂��B",
};
static cptr comment_nazgul_agree[MAX_COMMENT_1] =
{
	"�X��̋���Ȗڂ����΂����Ȃ������߂��B",
	"�X��͑�������΂������������蓮�������B",
	"�X��͉����������Ȃ����t���Ԃ₢���B",
	"�X��͂������Ƃ��Ȃ������B",
};


static cptr comment_basic_profit1[MAX_COMMENT_1] =
{
	"�u���΂炵���I�v",
	"�u�N�����l�l�Ɍ������I�v",
	"�X�傪�N�X�N�X�΂��Ă���B",
	"�X�傪�吺�ŏ΂��Ă���B"
};
static cptr comment_suzuna_profit1[MAX_COMMENT_1] =
{
	"�u����A������ł����H�v",
	"�u��������B�ׂ�����������B�v",
	"�u���������ɂ����Ⴈ�����ȁE�E�v",
	"�u���킠�A���肪�Ƃ��������܂��I�v"
};
static cptr comment_korin_profit1[MAX_COMMENT_1] =
{
	"�u�܁A���������q�����邩��l�������ł���Ƃ������̂��B�v",
	"�u�ق��B���Ѝ���������ڊ肢�������̂��B�v",
	"�u������������B�����v��Ȃ��ł��ꂽ�܂��B�v",
	"�u���₨��A�����C�O�̂����q���ȁB�v"
};
static cptr comment_nazgul_profit1[MAX_COMMENT_1] =
{
	"�X��͌��������߂��B",
	"�X��͕��ꂽ�悤�Ɍy�����U�����B",
	"�X��͍A���犣�������𗧂Ă��B",
	"�X��͌��������߂��B",
};

static cptr comment_basic_profit2[MAX_COMMENT_1] =
{
	"�u����ق��I�v",
	"�u����Ȃ��������v����������A�^�ʖڂɓ����Ȃ��Ȃ�Ȃ��B�v",
	"�X��͊������Ē��ˉ���Ă���B",
	"�X��͖��ʂɏ΂݂��������Ă���B"
};

static cptr comment_suzuna_profit2[MAX_COMMENT_1] =
{
	"�u���H����Ȃ����b������Ȃ�āE�E�v",
	"�u���́E�E�����v��Ȃ��ł��������ˁH�v",
	"�u����ŗ~�����������̖{�𔃂��邩���E�E�v",
	"�u����Ȓl�ł���������ȂɈ��������Ă�����Ă����̂�����H�v"
};
static cptr comment_korin_profit2[MAX_COMMENT_1] =
{
	"�u�����A���������̖��i�������Ă����B�v",
	"�u���������Ȃ����ˁA������Ȃ�ł�����͕s�����Ƃ������̂���B�v",
	"�u���̉��l�̂킩��Ȃ����q���ȁB�v",
	"�u���Ԃ�ɂ��ނ̂����\�����A�����������Ƃ�����厖�ɂ����܂��B�v"
};
static cptr comment_nazgul_profit2[MAX_COMMENT_1] =
{
	"�X��͂��Ȃ��ɕ��̂̌��t�𓊂������B",
	"�X��͖��������[��h�炵���B",
	"�X��̃t�[�h�̉��̖ڂ��}�΂��悤�ɍׂ߂�ꂽ�B",
	"�X��̃t�[�h�̉��̖ڂ��}�΂��悤�ɍׂ߂�ꂽ�B",
};


static cptr comment_basic_loss1[MAX_COMMENT_1] =
{
	"�u�������I�v",
	"�u���̗d���߁I�v",
	"�X�傪���߂������Ɍ��Ă���B",
	"�X�傪�ɂ�ł���B"
};
static cptr comment_suzuna_loss1[MAX_COMMENT_1] =
{
	"�u������A�g�����ɂȂ�Ȃ���B�v",
	"�u����Ȃ��E�E�v",
	"�u���A�܂������������E�E�v",
	"�X����ɂ܂ꂽ�B"
};
static cptr comment_korin_loss1[MAX_COMMENT_1] =
{

	"�u���܂����I�l�Ƃ������Ƃ��B�v",
	"�u�܂����܂ɂ͂����������Ƃ����邩�B�v",
	"�u�����B�܂��܂��l���C�s�s�����ȁB�v",
	"�u�܂��A����͂���ŋ����[���i���B�v"
};
static cptr comment_nazgul_loss1[MAX_COMMENT_1] =
{
	"�X��̃t�[�h�̓��������Ɨh�ꂽ�B",
	"�X��͂��Ȃ��֎��f�̌��t�𓊂������B",
	"�X��͂��Ȃ���˂��h���悤�Ȗڂ��ɂ݂����B",
	"�X��͂��߂��悤�ȚX�萺���グ���B",
};

static cptr comment_basic_loss2[MAX_COMMENT_1] =
{

	"�u���킠�������I�v",
	"�u�Ȃ�Ă������I�v",
	"�N�����ނ��ы���������������...�B",
	"�X�傪�������ɂ�߂��Ă���I"
};

static cptr comment_suzuna_loss2[MAX_COMMENT_1] =
{

	"�u�R�A����Ȃ��Ƃ��āE�E�v",
	"�u���A���ꂳ�[��I�v",
	"�u�ǂ����悤�A�����X�Ԃ����Ă��炦�Ȃ������E�E�v",
	"�X��ɋ��������Ȋ���ɂ܂ꂽ�B"
};
static cptr comment_korin_loss2[MAX_COMMENT_1] =
{

	"�u�E�E���̎d�ł��͊o���Ă�����B�v",
	"�u�N���Ȃ��Ȃ��̈��}���ȁI�v",
	"�u���ʂ͏�����̂��B���̃c�P�͂�����N�ɕԂ��Ă����B�v",
	"�u����Ă��ꂽ���̂��E�E�v"
};
static cptr comment_nazgul_loss2[MAX_COMMENT_1] =
{
	"�u�E�E�M�l��^���Èł̔ޕ��̒Q���̉Ƃɉ^��ł���悤���B�v",
	"�X��͗�C��Z�����������Ȃ��ɓ˂������B",
	"�X�̉����狐��ȉe�̂悤�ȏb���o�Ă��Ă��Ȃ����ɂݕt�����B",
	"�X�̉����狐��ȉe�̂悤�ȏb���o�Ă��Ă��Ȃ����ɂݕt�����B",
};

#endif
#define MAX_COMMENT_2A	2

static cptr comment_2a[MAX_COMMENT_2A] =
{
#ifdef JP
	"���̔E�ϗ͂������Ă���̂����H $%s ���Ōゾ�B",
	"�䖝�ɂ����x�����邼�B $%s ���Ōゾ�B"
#else
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
#endif

};

#define MAX_COMMENT_2B	12

static cptr comment_2b[MAX_COMMENT_2B] =
{
#ifdef JP
	" $%s ���炢�͏o���Ȃ���_������B",
	" $%s �Ȃ�󂯎���Ă��������B",
	"�n�I $%s �ȉ��͂Ȃ��ˁB",
	"���ēz���I $%s �ȉ��͂��蓾�Ȃ����B",
	"���ꂶ�Ꮽ�Ȃ�����I $%s �͗~�����Ƃ��낾�B",
	"�o�J�ɂ��Ă���I $%s �͂����Ȃ��ƁB",
	"�R���낤�I $%s �łǂ������H",
	"���������I $%s ���l���Ă���Ȃ����H",
	"1000�C�̃I�[�N�̃m�~�ɋꂵ�߂��邪�����I $%s ���B",
	"���O�̑�؂Ȃ��̂ɍЂ�����I $%s �łǂ����B",
	"�����S�X�ɏܖ�����邪�����I�{���� $%s �Ȃ񂾂낤�H",
	"���O�̕�e�̓I�[�K���I $%s �͏o������Ȃ񂾂�H"
#else
	"I can take no less than %s gold pieces.",
	"I will accept no less than %s gold pieces.",
	"Ha!  No less than %s gold pieces.",
	"You knave!  No less than %s gold pieces.",
	"That's a pittance!  I want %s gold pieces.",
	"That's an insult!  I want %s gold pieces.",
	"As if!  How about %s gold pieces?",
	"My arse!  How about %s gold pieces?",
	"May the fleas of 1000 orcs molest you!  Try %s gold pieces.",
	"May your most favourite parts go moldy!  Try %s gold pieces.",
	"May Morgoth find you tasty!  Perhaps %s gold pieces?",
	"Your mother was an Ogre!  Perhaps %s gold pieces?"
#endif

};

#ifdef JP
/* �u���b�N�}�[�P�b�g�p�ǉ����b�Z�[�W�i����Ƃ��j */
static cptr comment_2b_B[MAX_COMMENT_2B] = {
	"�����牴�l�����l�D���Ƃ͂��� $%s �����E���ˁB���Ȃ�A��ȁB",
	"�����Ȃ��̂����A���񂽁H�܂��͉ƂɋA���� $%s �����Ă��ȁB",
	"���̉��l���������z���ȁB����� $%s �����ʂȂ񂾂�B",
	"���̕t�����l�i�ɕ��傪����̂��H $%s �����E���B",
	"�Ђ���Ƃ��ĐV��̏�k�����H $%s �����ĂȂ��Ȃ�A��ȁB",
	"�����͑��̓X�Ƃ͈Ⴄ�񂾂�B$%s ���炢�͏o���ȁB",
	"�����C���Ȃ��Ȃ�A��ȁB $%s ���ƌ����Ă���񂾁B",
	"�b�ɂȂ�Ȃ��ˁB $%s ���炢�����Ă���񂾂�H",
	"�́H�Ȃ񂾂����H $%s �̊ԈႢ���A�Ђ���Ƃ��āH",
	"�o���͂���������B����Ƃ� $%s �o����̂����A���񂽂ɁB",
	"���m�炸�ȓz���ȁB $%s �o���΍����̏��͊��ق��Ă���B",
	"�����̓X�͕n�R�l���f�肾�B $%s ���炢�o���Ȃ��̂����H"
};
#endif
#define MAX_COMMENT_3A	2

static cptr comment_3a[MAX_COMMENT_3A] =
{
#ifdef JP
	"���̔E�ϗ͂������Ă���̂����H $%s ���Ōゾ�B",
	"�䖝�ɂ����x�����邼�B $%s ���Ōゾ�B"
#else
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
#endif

};


#define MAX_COMMENT_3B	12

static cptr comment_3b[MAX_COMMENT_3B] =
{
#ifdef JP
	"�{���������� $%s �ł����񂾂�H",
	" $%s �łǂ������H",
	" $%s ���炢�Ȃ�o���Ă��������B",
	" $%s �ȏ㕥���Ȃ�čl�����Ȃ��ˁB",
	"�܂��������āB $%s �łǂ������H",
	"���̃K���N�^�Ȃ� $%s �ň�������B",
	"���ꂶ�ፂ������I $%s �������Ƃ�����B",
	"�ǂ�������Ȃ��񂾂�I $%s �ł�������H",
	"���߂��߁I $%s �������Ƃ�����������B",
	"�o�J�ɂ��Ă���I $%s �������������B",
	" $%s �Ȃ�������Ƃ��낾���Ȃ��B",
	" $%s �A����ȏ�̓r�^�ꕶ�o���Ȃ���I"
#else
	"Perhaps %s gold pieces?",
	"How about %s gold pieces?",
	"I will pay no more than %s gold pieces.",
	"I can afford no more than %s gold pieces.",
	"Be reasonable.  How about %s gold pieces?",
	"I'll buy it as scrap for %s gold pieces.",
	"That is too much!  How about %s gold pieces?",
	"That looks war surplus!  Say %s gold pieces?",
	"Never!  %s is more like it.",
	"That's an insult!  %s is more like it.",
	"%s gold pieces and be thankful for it!",
	"%s gold pieces and not a copper more!"
#endif

};

#ifdef JP
/* �u���b�N�}�[�P�b�g�p�ǉ����b�Z�[�W�i�������j */
static cptr comment_3b_B[MAX_COMMENT_3B] = {
	" $%s ���ĂƂ��낾�ˁB���̂ǂ����悤���Ȃ��K���N�^�́B",
	"���̉��� $%s ���Č����Ă���񂾂���A���̒ʂ�ɂ��������g�̂��߂����B",
	"���̗D�����ɊÂ���̂����������ɂ��Ă����B $%s ���B",
	"���̕i�Ȃ� $%s �Ŕ����Ă���Ă��邪�ˁA�펯����a�m�݂͂�ȁB",
	"�����܂��A���߂��z���ȁB�����牴�������Ƃ͂��� $%s �����E���B",
	" $%s ���B�ʂɉ��͂���ȃK���N�^�~�����͂Ȃ��񂾂���B",
	"���̊Ӓ�z���C�ɓ���Ȃ��̂��H $%s �A���Ȃ�A��ȁB",
	" $%s �ň�������Ă���B���Ŏ󂯎��ȁA�n�R�l�B",
	"���̉��l���������z�͎n���ɂ�����ȁB����� $%s �Ȃ񂾂�B",
	"����Ȃɋ����~�����̂��A���񂽁H $%s �Ŗ����ł���̂��H",
	"����X�ԈႦ�Ă񂶂�Ȃ��̂��H $%s �Ō��Ȃ瑼���������Ă���B",
	"���̌����l�ɃP�`������z������Ƃ́I ���̓x���ɖƂ��� $%s ���B"
};
#endif
#define MAX_COMMENT_4A	4

static cptr comment_4a[MAX_COMMENT_4A] =
{
#ifdef JP
	"�����������񂾁I���x�������킸��킹�Ȃ��ł���I",
	"�����[�I����̉䖝�̌��x�𒴂��Ă���I",
	"���������I���Ԃ̖��ʈȊO�̂Ȃɂ��̂ł��Ȃ��I",
	"��������Ă��Ȃ���I����������Ȃ��I"
#else
	"Enough!  You have abused me once too often!",
	"Arghhh!  I have had enough abuse for one day!",
	"That does it!  You shall waste my time no more!",
	"This is getting nowhere!  I'm going to Londis!"
#endif

};

#ifdef JP
/* �u���b�N�}�[�P�b�g�p�ǉ����b�Z�[�W�i�{��̒��_�j */
static cptr comment_4a_B[MAX_COMMENT_4A] = {
	"�Ȃ߂₪���āI�����ȉ��l�ł����E��������Ă��Ƃ�m��I",
	"���������܂œ{�点��...�������邾���ł����肪�����Ǝv���I",
	"�ӂ����Ă�̂��I��₩���Ȃ瑊������Ă���ɂ���I",
	"����������ɂ���I���x����Ȃ܂˂����炽�����Ⴈ���˂����I"
};
#endif
#define MAX_COMMENT_4B	4

static cptr comment_4b[MAX_COMMENT_4B] =
{
#ifdef JP
	"�X����o�čs���I",
	"���̑O�������������I",
	"�ǂ����ɍs�����܂��I",
	"�o��A�o��A�o�čs���I"
#else
	"Leave my store!",
	"Get out of my sight!",
	"Begone, you scoundrel!",
	"Out, out, out!"
#endif

};

#ifdef JP
/* �u���b�N�}�[�P�b�g�p�ǉ����b�Z�[�W�i�ǂ��o���j */
static cptr comment_4b_B[MAX_COMMENT_4B] = {
	"��x�Ƃ����ɗ���񂶂�˂��I�I",
	"�Ƃ��ƂƁA�ǂ����֎�����I�I",
	"����������������I�I",
	"�o�Ă����I�o�Ă����I�I"
};
#endif
#define MAX_COMMENT_5	8

static cptr comment_5[MAX_COMMENT_5] =
{
#ifdef JP
	"�l�������Ă���B",
	"����Ⴈ�������I",
	"�����Ɛ^�ʖڂɌ����Ă���I",
	"������C������̂����H",
	"��₩���ɗ����̂��I",
	"������k���I",
	"�䖝����ׂ����B",
	"�Ӂ[�ށA�ǂ��V�C���B"
#else
	"Try again.",
	"Ridiculous!",
	"You will have to do better than that!",
	"Do you wish to do business or not?",
	"You've got to be kidding!",
	"You'd better be kidding!",
	"You try my patience.",
	"Hmmm, nice weather we're having."
#endif

};

#ifdef JP
/* �u���b�N�}�[�P�b�g�p�ǉ����b�Z�[�W�i�{��j */
static cptr comment_5_B[MAX_COMMENT_5] = {
	"���Ԃ̖��ʂ��ȁA����́B",
	"���Ȃ��q�l���ȁI",
	"�b���ĕ����鑊�肶��Ȃ��������B",
	"�ɂ��ڂɂ��������炵���ȁI",
	"�Ȃ�ċ��~�ȓz���I",
	"�b�ɂȂ��y���I",
	"�ǂ����悤���Ȃ��n�R�l���I",
	"���܂𔄂��Ă���̂��H"
};
#endif
#define MAX_COMMENT_6	4

static cptr comment_6[MAX_COMMENT_6] =
{
#ifdef JP
	"�ǂ���畷���ԈႦ���炵���B",
	"����A�悭�������Ȃ�������B",
	"���܂Ȃ��A�������āH",
	"�����A������x�����Ă����H"
#else
	"I must have heard you wrong.",
	"I'm sorry, I missed that.",
	"I'm sorry, what was that?",
	"Sorry, what was that again?"
#endif

};

///bldg131222 �X��̋��~�l��ݒ肷��֐�������� max�̂ق��͎����g���Ȃ���
static byte max_inflate(void) 
{
	return 200;
}
static byte min_inflate(void) 
{
	return 140;
}


///mod140620 �X�ɂ�胁�b�Z�[�W�ύX�B
static void say_comment_1(void)
{
	/* �X�ɂ���ĕʂ̃��b�Z�[�W���o�� */
	if (p_ptr->town_num == TOWN_KOURIN) //������
		msg_print(comment_korin_agree[randint0(MAX_COMMENT_1)]);
	else if (p_ptr->town_num == TOWN_UG) //�n���X
		msg_print(comment_nazgul_agree[randint0(MAX_COMMENT_1)]);
	else if ( cur_store_num == STORE_BLACK ) //BM���
		msg_print(comment_bm_agree[randint0(MAX_COMMENT_1)]);
	else if ( cur_store_num == STORE_BOOK &&  p_ptr->town_num == TOWN_HITOZATO) //��ވ�
		msg_print(comment_suzuna_agree[randint0(MAX_COMMENT_1)]);
	else //�ق��̓X
		msg_print(comment_basic_agree[randint0(MAX_COMMENT_1)]);


	///dell131210 �������ŉ\�Ȃ�
/*
	if (one_in_(RUMOR_CHANCE))
	{
#ifdef JP
		msg_print("�X��͎���������:");
#else
		msg_print("The shopkeeper whispers something into your ear:");
#endif
		display_rumor(TRUE);
	}
*/
}


/*
 * Continue haggling (player is buying)
 */
static void say_comment_2(s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_2a[randint0(MAX_COMMENT_2A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
#ifdef JP
		/* �u���b�N�}�[�P�b�g�̎��͕ʂ̃��b�Z�[�W���o�� */
		if ( cur_store_num == STORE_BLACK ){
			msg_format(comment_2b_B[randint0(MAX_COMMENT_2B)], tmp_val);
		}
		else{
		msg_format(comment_2b[randint0(MAX_COMMENT_2B)], tmp_val);
	}
#else
		msg_format(comment_2b[randint0(MAX_COMMENT_2B)], tmp_val);
#endif

	}
}


/*
 * Continue haggling (player is selling)
 */
static void say_comment_3(s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_3a[randint0(MAX_COMMENT_3A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
#ifdef JP
		/* �u���b�N�}�[�P�b�g�̎��͕ʂ̃��b�Z�[�W���o�� */
		if ( cur_store_num == STORE_BLACK ){
			msg_format(comment_3b_B[randint0(MAX_COMMENT_3B)], tmp_val);
		}
		else{
		msg_format(comment_3b[randint0(MAX_COMMENT_3B)], tmp_val);
	}
#else
		msg_format(comment_3b[randint0(MAX_COMMENT_3B)], tmp_val);
#endif

	}
}


/*
 * Kick 'da bum out.					-RAK-
 */
static void say_comment_4(void)
{
#ifdef JP
	/* �u���b�N�}�[�P�b�g�̎��͕ʂ̃��b�Z�[�W���o�� */
	if ( cur_store_num == STORE_BLACK ){
		msg_print(comment_4a_B[randint0(MAX_COMMENT_4A)]);
		msg_print(comment_4b_B[randint0(MAX_COMMENT_4B)]);
	}
	else{
		msg_print(comment_4a[randint0(MAX_COMMENT_4A)]);
		msg_print(comment_4b[randint0(MAX_COMMENT_4B)]);
	}
#else
	msg_print(comment_4a[randint0(MAX_COMMENT_4A)]);
	msg_print(comment_4b[randint0(MAX_COMMENT_4B)]);
#endif

}


/*
 * You are insulting me
 */
static void say_comment_5(void)
{
#ifdef JP
	/* �u���b�N�}�[�P�b�g�̎��͕ʂ̃��b�Z�[�W���o�� */
	if ( cur_store_num == STORE_BLACK ){
		msg_print(comment_5_B[randint0(MAX_COMMENT_5)]);
	}
	else{
		msg_print(comment_5[randint0(MAX_COMMENT_5)]);
	}
#else
	msg_print(comment_5[randint0(MAX_COMMENT_5)]);
#endif

}


/*
 * That makes no sense.
 */
static void say_comment_6(void)
{
	msg_print(comment_6[randint0(MAX_COMMENT_6)]);
}



/*
 * Messages for reacting to purchase prices.
 */
/*
#define MAX_COMMENT_7A	4

static cptr comment_7a[MAX_COMMENT_7A] =
{
#ifdef JP
	"���킠�������I",
	"�Ȃ�Ă������I",
	"�N�����ނ��ы���������������...�B",
	"�X�傪�������ɂ�߂��Ă���I"
#else
	"Arrgghh!",
	"You bastard!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
#endif

};

#define MAX_COMMENT_7B	4

static cptr comment_7b[MAX_COMMENT_7B] =
{
#ifdef JP
	"�������I",
	"���̈����߁I",
	"�X�傪���߂������Ɍ��Ă���B",
	"�X�傪�ɂ�ł���B"
#else
	"Damn!",
	"You fiend!",
	"The shopkeeper curses at you.",
	"The shopkeeper glares at you."
#endif

};

#define MAX_COMMENT_7C	4

static cptr comment_7c[MAX_COMMENT_7C] =
{
#ifdef JP
	"���΂炵���I",
	"�N���V�g�Ɍ������I",
	"�X�傪�N�X�N�X�΂��Ă���B",
	"�X�傪�吺�ŏ΂��Ă���B"
#else
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
#endif

};

#define MAX_COMMENT_7D	4

static cptr comment_7d[MAX_COMMENT_7D] =
{
#ifdef JP
	"����ق��I",
	"����Ȃ��������v����������A�^�ʖڂɓ����Ȃ��Ȃ�Ȃ��B",
	"�X��͊������Ē��ˉ���Ă���B",
	"�X��͖��ʂɏ΂݂��������Ă���B"
#else
	"Yipee!",
	"I think I'll retire!",
	"The shopkeeper jumps for joy.",
	"The shopkeeper smiles gleefully."
#endif

};
*/


/*
 * Let a shop-keeper React to a purchase
 *
 * We paid "price", it was worth "value", and we thought it was worth "guess"
 */
/*:::���ۂ̉��i��荂��/�����������Ƃ��ɓX��̃��A�N�V����*/
///mod140620 �X�ɂ�胁�b�Z�[�W�ύX�B�呹�̒�`���u�����l�ȕ��𔃂�������v����u3000�ȏ�J����ꂽ�v�ɕς����B�u��ׂ��v�̒�`���u���ς�������ۉ��l��3000�ȏ㍂���v�ɕς����B
static void purchase_analyze(s32b price, s32b value, s32b guess)
{
	/* Item was worthless, but we bought it */
	//if ((value <= 0) && (price > value))
	if ((price - value) >= 3000)
	{
		/* Comment */
		//msg_print(comment_7a[randint0(MAX_COMMENT_7A)]);

		if (p_ptr->town_num == TOWN_KOURIN) //������
			msg_print(comment_korin_loss2[randint0(MAX_COMMENT_1)]);
		else if (p_ptr->town_num == TOWN_UG) //�n���X
			msg_print(comment_nazgul_loss2[randint0(MAX_COMMENT_1)]);
		else if ( cur_store_num == STORE_BOOK &&  p_ptr->town_num == TOWN_HITOZATO) //��ވ�
			msg_print(comment_suzuna_loss2[randint0(MAX_COMMENT_1)]);
		else //�ق��̓X
			msg_print(comment_basic_loss2[randint0(MAX_COMMENT_1)]);

		//chg_virtue(V_HONOUR, -1);
		//chg_virtue(V_JUSTICE, -1);

		/* Sound */
		sound(SOUND_STORE1);
		set_deity_bias(DBIAS_REPUTATION, -5);

	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if ((value < guess) && (price > value))
	{

		if (p_ptr->town_num == TOWN_KOURIN) //������
			msg_print(comment_korin_loss1[randint0(MAX_COMMENT_1)]);
		else if (p_ptr->town_num == TOWN_UG) //�n���X
			msg_print(comment_nazgul_loss1[randint0(MAX_COMMENT_1)]);
		else if ( cur_store_num == STORE_BOOK &&  p_ptr->town_num == TOWN_HITOZATO) //��ވ�
			msg_print(comment_suzuna_loss1[randint0(MAX_COMMENT_1)]);
		else //�ق��̓X
			msg_print(comment_basic_loss1[randint0(MAX_COMMENT_1)]);

		/* Comment */
		//msg_print(comment_7b[randint0(MAX_COMMENT_7B)]);

		//chg_virtue(V_JUSTICE, -1);
		//if (one_in_(4))
		//	chg_virtue(V_HONOUR, -1);

		if(one_in_(2))set_deity_bias(DBIAS_REPUTATION, -1);
		/* Sound */
		sound(SOUND_STORE2);
	}

	/* Item was a great bargain, and we got away with it */
	else if ((value - guess >= 3000) )
	{
		if (p_ptr->town_num == TOWN_KOURIN) //������
			msg_print(comment_korin_profit2[randint0(MAX_COMMENT_1)]);
		else if (p_ptr->town_num == TOWN_UG) //�n���X
			msg_print(comment_nazgul_profit2[randint0(MAX_COMMENT_1)]);
		else if ( cur_store_num == STORE_BOOK &&  p_ptr->town_num == TOWN_HITOZATO) //��ވ�
			msg_print(comment_suzuna_profit2[randint0(MAX_COMMENT_1)]);
		else //�ق��̓X
			msg_print(comment_basic_profit2[randint0(MAX_COMMENT_1)]);
		/* Comment */
		//msg_print(comment_7d[randint0(MAX_COMMENT_7D)]);

		//if (one_in_(2))
		//	chg_virtue(V_HONOUR, -1);
		//if (one_in_(4))
		//	chg_virtue(V_HONOUR, 1);

		//if (10 * price < value)
		//	chg_virtue(V_SACRIFICE, 1);

		/* Sound */
		sound(SOUND_STORE4);
	}

	/* Item was a good bargain, and we got away with it */
	//else if ((value > guess) && (value < (4 * guess)) && (price < value))
	else if ((value > guess) )
	{
		if (p_ptr->town_num == TOWN_KOURIN) //������
			msg_print(comment_korin_profit1[randint0(MAX_COMMENT_1)]);
		else if (p_ptr->town_num == TOWN_UG) //�n���X
			msg_print(comment_nazgul_profit1[randint0(MAX_COMMENT_1)]);
		else if ( cur_store_num == STORE_BOOK &&  p_ptr->town_num == TOWN_HITOZATO) //��ވ�
			msg_print(comment_suzuna_profit1[randint0(MAX_COMMENT_1)]);
		else //�ق��̓X
			msg_print(comment_basic_profit1[randint0(MAX_COMMENT_1)]);


		/* Comment */
		//msg_print(comment_7c[randint0(MAX_COMMENT_7C)]);

		//if (one_in_(4))
		//	chg_virtue(V_HONOUR, -1);
		//else if (one_in_(4))
		//	chg_virtue(V_HONOUR, 1);

		/* Sound */
		sound(SOUND_STORE3);
	}

}





//mod140314 �X�̃A�C�e���������p�e�[�u������get_obj_num_hook�ōs���悤�ɕύX�����B


//�G�ݓX�̃A�C�e������hook
static bool store_create_hook_general(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	//��
	if((k_ptr->tval == TV_ARMOR) && (k_ptr->sval == SV_ARMOR_TUB))	return (TRUE);
	//�i�C�t�ƎR��
	if((k_ptr->tval == TV_KNIFE) && (k_ptr->sval == SV_WEAPON_KNIFE || k_ptr->sval == SV_WEAPON_YAMAGATANA)) return (TRUE);
	//�ފ�
	if((k_ptr->tval == TV_OTHERWEAPON) && (k_ptr->sval == SV_OTHERWEAPON_FISHING))	return (TRUE);
	//�j
	if((k_ptr->tval == TV_OTHERWEAPON) && (k_ptr->sval == SV_OTHERWEAPON_NEEDLE))	return (TRUE);
	//��͂�
	if((k_ptr->tval == TV_AXE) && (k_ptr->sval == SV_WEAPON_PICK))	return (TRUE);
	///mod150911 �n��ǉ�
	if((k_ptr->tval == TV_HAMMER) && (k_ptr->sval == SV_WEAPON_KINE))	return (TRUE);
	//�����܂ƃ����^��
	if((k_ptr->tval == TV_LITE) && (k_ptr->sval == SV_LITE_TORCH || k_ptr->sval == SV_LITE_LANTERN)) return (TRUE);
	//����
	if((k_ptr->tval == TV_FLASK) && (k_ptr->sval == SV_FLASK_OIL))	return (TRUE);
	//�N���[�N�A�є�̃N���[�N
	if((k_ptr->tval == TV_CLOAK) && (k_ptr->sval == SV_CLOAK || k_ptr->sval == SV_CLOAK_FUR)) return (TRUE);
	//��
	if((k_ptr->tval == TV_SOFTDRINK) && (k_ptr->sval == SV_DRINK_WATER)) return (TRUE);
	//�N�b�L�[
	if((k_ptr->tval == TV_SWEETS) && (k_ptr->sval == SV_SWEET_COOKIE)) return (TRUE);
	//�H���A���p���A�������A��O
	if((k_ptr->tval == TV_FOOD) && (k_ptr->sval == SV_FOOD_RATION || k_ptr->sval == SV_FOOD_HARDBUSC || k_ptr->sval == SV_FOOD_VENISON  || k_ptr->sval == SV_FOOD_SENTAN)) return (TRUE);
	//���イ��i�͓��̗��̂݁j
	if((k_ptr->tval == TV_FOOD) && k_ptr->sval == SV_FOOD_CUCUMBER && p_ptr->town_num == TOWN_KAPPA) return (TRUE);
	//�X�p�i�i�͓��̗��̂݁j
	if((k_ptr->tval == TV_HAMMER) && k_ptr->sval == SV_WEAPON_SPANNER && p_ptr->town_num == TOWN_KAPPA) return (TRUE);
	//���g���i�l���̂݁j
	if((k_ptr->tval == TV_FOOD) && k_ptr->sval == SV_FOOD_ABURAAGE && p_ptr->town_num == TOWN_HITOZATO) return (TRUE);
	//v1.1.34 �}
	if((k_ptr->tval == TV_HEAD) && (k_ptr->sval == SV_HEAD_KASA))	return (TRUE);

	return (FALSE);
}

//�h��X�̃A�C�e������hook
static bool store_create_hook_armoury(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	//���͒u����Ȃ�
	if((k_ptr->tval == TV_ARMOR) && (k_ptr->sval == SV_ARMOR_TUB))	return (FALSE);

	//���{��
	if((k_ptr->tval == TV_RIBBON))	return (TRUE);
	//�������[�W�܂ł̏�
	if((k_ptr->tval == TV_SHIELD) && (k_ptr->sval <= SV_LARGE_METAL_SHIELD))return (TRUE);
	//�������ƛޏ����ȊO�̕�
	if((k_ptr->tval == TV_CLOTHES) && k_ptr->sval != SV_CLOTH_KUROSHOUZOKU && k_ptr->sval != SV_CLOTH_MIKO )return (TRUE);
	//�p�[�V�����v���[�g�܂ł̊Z
	if((k_ptr->tval == TV_ARMOR) && (k_ptr->sval <= SV_ARMOR_PARTIAL_PLATE))return (TRUE);
	//�N���[�N�A�}���g�A�є�N���[�N
	if((k_ptr->tval == TV_CLOAK) && (k_ptr->sval <= SV_CLOAK_FUR))return (TRUE);
	//�����X�q�܂ł̓�����
	if((k_ptr->tval == TV_HEAD) && (k_ptr->sval <= SV_HEAD_METALCAP))return (TRUE);
	//�Z�X�^�X�܂ł̘r����
	if((k_ptr->tval == TV_GLOVES) && (k_ptr->sval <= SV_HAND_CESTIS))return (TRUE);
	//�u�[�c�܂ł̑�����
	if((k_ptr->tval == TV_BOOTS) && (k_ptr->sval <= SV_LEG_LEATHERBOOTS))return (TRUE);

	//�n��ł͂������������Ȗh���
	///mod160229 �͓��̗���n���X���܂߂�
	if((k_ptr->tval == TV_ARMOR) && (k_ptr->sval <= SV_ARMOR_FULL_PLATE) && p_ptr->town_num >= TOWN_CHITEI) return (TRUE);
	if((k_ptr->tval == TV_HEAD) && (k_ptr->sval <= SV_HEAD_STEELHELM) && p_ptr->town_num >= TOWN_CHITEI)return (TRUE);
	if((k_ptr->tval == TV_GLOVES) && (k_ptr->sval <= SV_HAND_GAUNTLETS)&& p_ptr->town_num >= TOWN_CHITEI)return (TRUE);
	if((k_ptr->tval == TV_BOOTS) && (k_ptr->sval <= SV_LEG_SABATON) && p_ptr->town_num >= TOWN_CHITEI)return (TRUE);

	return (FALSE);
}

//����X�̃A�C�e������hook
static bool store_create_hook_weapon(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	//��͂��A�ԁA���p�t�̏�A�X�p�i�͒u����Ȃ�
	if((k_ptr->tval == TV_AXE) && (k_ptr->sval == SV_WEAPON_PICK))	return (FALSE);
	if((k_ptr->tval == TV_STICK) && (k_ptr->sval == SV_WEAPON_FLOWER))return (FALSE);
	if((k_ptr->tval == TV_STICK) && (k_ptr->sval == SV_WEAPON_WIZSTAFF))return (FALSE);
	if((k_ptr->tval == TV_HAMMER) && k_ptr->sval == SV_WEAPON_SPANNER ) return (FALSE);
	if((k_ptr->tval == TV_STICK) && (k_ptr->sval == SV_WEAPON_BROKEN_STICK))return (FALSE);
	if((k_ptr->tval == TV_KATANA) && (k_ptr->sval == SV_WEAPON_BROKEN_KATANA))return (FALSE);
	if((k_ptr->tval == TV_SWORD) && (k_ptr->sval == SV_WEAPON_BROKEN_SWORD))return (FALSE);
	if((k_ptr->tval == TV_KNIFE) && (k_ptr->sval == SV_WEAPON_BROKEN_DAGGER))return (FALSE);
	///mod150911 �n�͒u����Ȃ�
	if((k_ptr->tval == TV_HAMMER) && (k_ptr->sval == SV_WEAPON_KINE))	return (FALSE);

	//�Z���܂ł̒Z��
	if((k_ptr->tval == TV_KNIFE) && (k_ptr->sval <= SV_WEAPON_TANTO))return (TRUE);
	//�����O�\�[�h�܂ł̒���
	if((k_ptr->tval == TV_SWORD) && (k_ptr->sval <= SV_WEAPON_LONG_SWORD))return (TRUE);
	//�e���Ɠ�
	if((k_ptr->tval == TV_KATANA) && (k_ptr->sval <= SV_WEAPON_KATANA))return (TRUE);
	//�t���C���܂ł̓݊�
	if((k_ptr->tval == TV_HAMMER) && (k_ptr->sval <= SV_WEAPON_FLAIL))return (TRUE);
	//�k���`���N�܂ł̖_
	if((k_ptr->tval == TV_STICK) && (k_ptr->sval <= SV_WEAPON_NUNCHAKU))return (TRUE);
	//�o�g���A�b�N�X�܂ł̕�
	if((k_ptr->tval == TV_AXE) && (k_ptr->sval <= SV_WEAPON_BATTLE_AXE))return (TRUE);
	//�����O�X�s�A�܂ł̑�
	if((k_ptr->tval == TV_SPEAR) && (k_ptr->sval <= SV_WEAPON_LONG_SPEAR))return (TRUE);
	//���c�F�����n���}�[�܂ł̒�������
	if((k_ptr->tval == TV_POLEARM) && (k_ptr->sval <= SV_WEAPON_LUCERNE_HAMMER))return (TRUE);
	//�V���[�g�{�E�ƃ��C�g�N���X�{�E
	if((k_ptr->tval == TV_BOW) && (k_ptr->sval <= SV_FIRE_SHORT_BOW))return (TRUE);
	if((k_ptr->tval == TV_CROSSBOW) && (k_ptr->sval <= SV_FIRE_LIGHT_CROSSBOW))return (TRUE);
	//��ƃN���X�{�E�̖�
	if((k_ptr->tval == TV_ARROW || k_ptr->tval == TV_BOLT) && (k_ptr->sval == SV_AMMO_LIGHT))return (TRUE);

	//�n��ɂ͗ǂ��x�[�X���o��悤�ɂ���
	///mod160229 �͓��̗���n���X���܂߂�
	if(p_ptr->town_num >= TOWN_CHITEI)
	{
		if((k_ptr->tval == TV_KNIFE) && (k_ptr->sval <= SV_WEAPON_NINJA_SWORD))return (TRUE);
		if((k_ptr->tval == TV_SWORD) && (k_ptr->sval <= SV_WEAPON_GREAT_SWORD))return (TRUE);
		if((k_ptr->tval == TV_KATANA) && (k_ptr->sval <= SV_WEAPON_GREAT_KATANA))return (TRUE);
		if((k_ptr->tval == TV_HAMMER) && (k_ptr->sval <= SV_WEAPON_BALL_AND_CHAIN))return (TRUE);
		if((k_ptr->tval == TV_STICK) && (k_ptr->sval <= SV_WEAPON_IRONSTICK))return (TRUE);
		if((k_ptr->tval == TV_AXE) && (k_ptr->sval <= SV_WEAPON_GREAT_AXE))return (TRUE);
		if((k_ptr->tval == TV_SPEAR) && (k_ptr->sval <= SV_WEAPON_LANCE))return (TRUE);
		if((k_ptr->tval == TV_POLEARM) && (k_ptr->sval <= SV_WEAPON_HALBERD))return (TRUE);
		if((k_ptr->tval == TV_BOW) && (k_ptr->sval <= SV_FIRE_LONG_BOW))return (TRUE);
		if((k_ptr->tval == TV_CROSSBOW) && (k_ptr->sval <= SV_FIRE_HEAVY_CROSSBOW))return (TRUE);
		if((k_ptr->tval == TV_ARROW || k_ptr->tval == TV_BOLT) && (k_ptr->sval == SV_AMMO_NORMAL))return (TRUE);
		//v1.1.62
		if(k_ptr->tval == TV_MAGICITEM  && k_ptr->sval == SV_MAGICITEM_QUIVER)return (TRUE);

	}
	///mod160514 �͓��̗���n���X�ɂ͏e�����ԁ@�l���ɂ͗e�̂ݕ���
	if(k_ptr->tval == TV_GUN && p_ptr->town_num >= TOWN_KAPPA) return TRUE;
	if(k_ptr->tval == TV_GUN && k_ptr->sval == SV_FIRE_GUN_HUNTING && p_ptr->town_num == TOWN_HITOZATO) return TRUE;


	return (FALSE);
}

//���E��X�̃A�C�e������hook
static bool store_create_hook_liquor(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	if(k_ptr->tval == TV_POTION) switch(k_ptr->sval)
	{
	case SV_POTION_INFRAVISION:
	case SV_POTION_CURE_LIGHT:
	case SV_POTION_CURE_SERIOUS:
	case SV_POTION_CURE_CRITICAL:
	case SV_POTION_RESTORE_EXP:
	case SV_POTION_RES_STR:
	case SV_POTION_RES_INT:
	case SV_POTION_RES_WIS:
	case SV_POTION_RES_DEX:
	case SV_POTION_RES_CON:
	case SV_POTION_RES_CHR:
	case SV_POTION_CURING:
	//v1.1.83 �ωΑϗ�̖�ǉ�
	case SV_POTION_RESIST_HEAT:
	case SV_POTION_RESIST_COLD:


		return (TRUE);	
		break;
	default:
		return (FALSE);	
		break;
	}

	if((k_ptr->tval == TV_BOOK_MEDICINE) && k_ptr->sval < 2) return (TRUE);

	if(k_ptr->tval == TV_ALCOHOL)
	switch(k_ptr->sval)
	{
	case SV_ALCOHOL_BEER:
	case SV_ALCOHOL_GINZYOU:
	case SV_ALCOHOL_ONIKOROSHI:
	case SV_ALCOHOL_DOBUROKU:
	case SV_ALCOHOL_SYOUTYUU:
	case SV_ALCOHOL_PONSYU:

		return (TRUE);	
		break;
	default:
		return (FALSE);	
		break;
	}


	return (FALSE);
}

//�@�B�X�̃A�C�e������hook
/*
static bool store_create_hook_machine(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	//�X�p�i
	if((k_ptr->tval == TV_HAMMER) && k_ptr->sval == SV_WEAPON_SPANNER ) return (TRUE);
	//����
	if((k_ptr->tval == TV_FLASK) && (k_ptr->sval == SV_FLASK_OIL))	return (TRUE);
	//���S�z
	if((k_ptr->tval == TV_MATERIAL) && (k_ptr->sval == SV_MATERIAL_MAGNETITE))return (TRUE);

	//�@�B�@���݂͈����
	if((k_ptr->tval == TV_MACHINE)  ) return (TRUE);

	return (FALSE);
}
*/
//�����X�̃A�C�e������hook ���X�ɂ��A�҂ƃe���|�����͒u�������H
static bool store_create_hook_scroll(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	
	if(k_ptr->tval == TV_SCROLL) switch(k_ptr->sval)
	{
	case SV_SCROLL_PHASE_DOOR:
	case SV_SCROLL_TELEPORT:
	case SV_SCROLL_WORD_OF_RECALL:
	case SV_SCROLL_IDENTIFY:
	case SV_SCROLL_STAR_IDENTIFY:
	case SV_SCROLL_REMOVE_CURSE:
	case SV_SCROLL_STAR_REMOVE_CURSE:
	case SV_SCROLL_RECHARGING:
	case SV_SCROLL_LIGHT:
	case SV_SCROLL_MAPPING:
	case SV_SCROLL_DETECT_GOLD:
	case SV_SCROLL_DETECT_ITEM:
	case SV_SCROLL_MONSTER_CONFUSION:
	case SV_SCROLL_HOLY_CHANT:

	case SV_SCROLL_DETECT_TRAP:
	case SV_SCROLL_DETECT_DOOR:
	case SV_SCROLL_DETECT_INVIS:
	case SV_SCROLL_TRAP_DOOR_DESTRUCTION:

		return (TRUE);	
		break;
	default:
		return (FALSE);	
		break;
	}


	return (FALSE);
}

//���@�X�̃A�C�e������hook
static bool store_create_hook_magic(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	if(k_ptr->tval == TV_RING) switch(k_ptr->sval)
	{
	case SV_RING_LEVITATION_FALL:
	case SV_RING_RESIST_FIRE:
	case SV_RING_RESIST_COLD:
	case SV_RING_PROTECTION:
	case SV_RING_WARNING:
		return (TRUE);	
		break;
	default:
		return (FALSE);	
		break;
	}
	if(k_ptr->tval == TV_AMULET) switch(k_ptr->sval)
	{
	case SV_AMULET_RESIST_ELEC:
	case SV_AMULET_RESIST_ACID:

		return (TRUE);	
		break;
	//v1.1.12 ����A�~�����b�g�ǉ��B�l���ɔ�����
	case SV_AMULET_HAKUREI:
		if(p_ptr->town_num == TOWN_HITOZATO) return TRUE;
		else return FALSE;
		break;
	default:
		return (FALSE);	
		break;
	}

	if(k_ptr->tval == TV_STAFF) switch(k_ptr->sval)
	{
	case SV_STAFF_IDENTIFY:
	case SV_STAFF_LITE:
	case SV_STAFF_MAPPING:
	case SV_STAFF_DETECT_GOLD:
	case SV_STAFF_DETECT_ITEM:
	case SV_STAFF_DETECT_TRAP:
	case SV_STAFF_DETECT_DOOR:
	case SV_STAFF_DETECT_EVIL:
	case SV_STAFF_DETECT_INVIS:
	case SV_STAFF_PROBING:
		return (TRUE);	
		break;
	default:
		return (FALSE);	
		break;
	}
	if(k_ptr->tval == TV_WAND) switch(k_ptr->sval)
	{
	case SV_WAND_DISARMING:
	case SV_WAND_CONFUSE_MONSTER:
	case SV_WAND_STINKING_CLOUD:
	case SV_WAND_WONDER:
	case SV_WAND_MAGIC_MISSILE:
	case SV_WAND_COLD_BOLT:
	case SV_WAND_FIRE_BOLT:


		return (TRUE);	
		break;
	default:
		return (FALSE);	
		break;
	}



	return (FALSE);
}

//���X�̃A�C�e������hook
static bool store_create_hook_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	///mod141227 ���p��1,2���ڒǉ�
	switch(p_ptr->town_num)
	{
	//�l�� �d���{�A���p��p���@��2���ځA�l�`(�i���N�W�ɂ���)
	case TOWN_HITOZATO:
		if((k_ptr->tval == TV_CAPTURE)) return (TRUE);
		if(k_ptr->tval >= TV_BOOK_ELEMENT && k_ptr->tval <= TV_BOOK_NATURE && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_HISSATSU) && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_MEDICINE) && k_ptr->sval < 2) return (TRUE);
		if(k_ptr->tval == TV_BOOK_OCCULT) return(TRUE);
		if(k_ptr->tval == TV_FIGURINE) return(TRUE);
		break;

	//�V��A�n��@���p�d�p1,2����
	case TOWN_TENGU:
	case TOWN_CHITEI:
		if(k_ptr->tval >= TV_BOOK_ELEMENT && k_ptr->tval <= TV_BOOK_SUMMONS && k_ptr->sval < 2) return (TRUE);
		if(k_ptr->tval >= TV_BOOK_TRANSFORM && k_ptr->tval <= TV_BOOK_CHAOS && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_HISSATSU) && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_MEDICINE) && k_ptr->sval < 2) return (TRUE);
		break;
	//�n���_���W�����ł��d���{�����ԁB���ƂŒ��g����ɂ���B
	case TOWN_UG:
		if((k_ptr->tval == TV_CAPTURE)) return (TRUE);
		if(k_ptr->tval >= TV_BOOK_ELEMENT && k_ptr->tval <= TV_BOOK_CHAOS && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_HISSATSU) && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_MEDICINE) && k_ptr->sval < 2) return (TRUE);

	//v1.1.61 EXTRA�[�w�̒n���X�ɂ�3,4���ڂ����Ԃ悤�ɂ���
		if (EXTRA_MODE && k_ptr->tval >= TV_BOOK_ELEMENT && k_ptr->tval <= TV_BOOK_OCCULT && k_ptr->sval < (dun_level / 30 + 1)) return (TRUE);


		break;

	//�ق�(���̂Ƃ���g���ق̂�)�@�S�̈�1,2����
	default:
		if(k_ptr->tval >= TV_BOOK_ELEMENT && k_ptr->tval <= TV_BOOK_CHAOS && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_HISSATSU) && k_ptr->sval < 2) return (TRUE);
		if((k_ptr->tval == TV_BOOK_MEDICINE) && k_ptr->sval < 2) return (TRUE);
		break;
	}


/*	���������X�Ŕ���̒��~
	//�����@���@�X�Ŕ��镨�������̂ŏ��X�Ŕ����邱�Ƃɂ����B�������͂ǂ���ł��s���悤�ɂ���
	if(k_ptr->tval == TV_SCROLL) switch(k_ptr->sval)
	{
	case SV_SCROLL_PHASE_DOOR:
	case SV_SCROLL_TELEPORT:
	case SV_SCROLL_WORD_OF_RECALL:
	case SV_SCROLL_IDENTIFY:
	case SV_SCROLL_STAR_IDENTIFY:
	case SV_SCROLL_REMOVE_CURSE:
	case SV_SCROLL_STAR_REMOVE_CURSE:
	case SV_SCROLL_RECHARGING:
	case SV_SCROLL_LIGHT:
	case SV_SCROLL_MAPPING:
	case SV_SCROLL_DETECT_GOLD:
	case SV_SCROLL_DETECT_ITEM:
	case SV_SCROLL_MONSTER_CONFUSION:
	case SV_SCROLL_HOLY_CHANT:
		return (TRUE);	
		break;
	default:
		return (FALSE);	
		break;
	}
*/

	return (FALSE);
}




/*
 * We store the current "store feat" here so everyone can access it
 */
static int cur_store_feat;

///bldg131222 �X�̉��i�̎푰�␳�𖳌�
#if 0
/*
 * Buying and selling adjustments for race combinations.
 * Entry[owner][player] gives the basic "cost inflation".
 */
static byte rgold_adj[MAX_RACES][MAX_RACES] =
{
	/*Hum, HfE, Elf,  Hal, Gno, Dwa, HfO, HfT, Dun, HiE, Barbarian,
	 HfOg, HGn, HTn, Cyc, Yek, Klc, Kbd, Nbl, DkE, Drc, Mind Flayer,
	 Imp,  Glm, Skl, Zombie, Vampire, Spectre, Fairy, Beastman, Ent,
	 Angel, Demon, Kutar */

	/* Human */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 125, 125, 105, 120, 105,  95, 140,
	  100, 120, 110, 105 },

	/* Half-Elf */
	{ 110, 100, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  120, 115, 108, 115, 110, 110, 120, 120, 115, 115, 110,
	  120, 110, 110, 110, 120, 110, 100, 125, 100,  95, 140,
	  110, 115, 110, 110 },

	/* Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  120, 120, 105, 120, 110, 105, 125, 125, 110, 115, 108,
	  120, 115, 110, 110, 120, 110, 100, 125, 100,  95, 140,
	  110, 110, 105, 110 },

	/* Halfling */
	{ 115, 110, 105,  95, 105, 110, 115, 130, 115, 105, 115,
	  125, 120, 120, 125, 115, 110, 120, 120, 120, 115, 115,
	  120, 110, 120, 120, 130, 110, 110, 130, 110,  95, 140,
	  115, 120, 105, 115 },

	/* Gnome */
	{ 115, 115, 110, 105,  95, 110, 115, 130, 115, 110, 115,
	  120, 125, 110, 120, 110, 105, 120, 110, 110, 105, 110,
	  120, 101, 110, 110, 120, 120, 115, 130, 115,  95, 140,
	  115, 110, 110, 115 },

	/* Dwarf */
	{ 115, 120, 120, 110, 110,  95, 125, 135, 115, 120, 115,
	  125, 140, 130, 130, 120, 115, 115, 115, 135, 125, 120,
	  120, 105, 115, 115, 115, 115, 120, 130, 120,  95, 140,
	  115, 110, 115, 115 },

	/* Half-Orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 115, 120, 125, 115, 125,  95, 140,
	  115, 110, 115, 115 },

	/* Half-Troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 120, 110, 120,  95, 140,
	  110, 110, 115, 110 },

	/* Amberite */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  120, 120, 105, 120, 115, 105, 115, 120, 110, 105, 105,
	  120, 105, 120, 120, 125, 120, 105, 135, 105,  95, 140,
	  100, 110, 110, 100 },

	/* High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 125, 120, 100, 125, 100,  95, 140,
	  110, 110, 105, 110 },

	/* Human / Barbarian (copied from human) */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 130, 125, 115, 120, 115,  95, 140,
	  100, 120, 110, 100 },

	/* Half-Ogre: theoretical, copied from half-troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 120, 110, 120,  95, 140,
	  110, 110, 115, 110 },

	/* Half-Giant: theoretical, copied from half-troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 130, 120, 130,  95, 140,
	  110, 110, 115, 110 },

	/* Half-Titan: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110 },

	/* Cyclops: theoretical, copied from half-troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 130, 130, 130,  95, 140,
	  110, 110, 115, 110 },

	/* Yeek: theoretical, copied from Half-Orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115 },

	/* Klackon: theoretical, copied from Gnome */
	{ 115, 115, 110, 105,  95, 110, 115, 130, 115, 110, 115,
	  120, 125, 110, 120, 110, 105, 120, 110, 110, 105, 110,
	  120, 101, 110, 110, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115 },

	/* Kobold: theoretical, copied from Half-Orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115 },

	/* Nibelung: theoretical, copied from Dwarf */
	{ 115, 120, 120, 110, 110,  95, 125, 135, 115, 120, 115,
	  125, 140, 130, 130, 120, 115, 115, 115, 135, 125, 120,
	  120, 105, 115, 115, 120, 120, 130, 130, 130,  95, 140,
	  115, 135, 115, 115 },

	/* Dark Elf */
	{ 110, 110, 110, 115, 120, 130, 115, 115, 120, 110, 115,
	  115, 115, 116, 115, 120, 120, 115, 115, 101, 110, 110,
	  110, 110, 112, 122, 110, 110, 110, 115, 110, 120, 120,
	  110, 101, 115, 110 },

	/* Draconian: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110 },

	/* Mind Flayer: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110 },

	/* Imp: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  110, 110, 115, 110 },

	/* Golem: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110 },

	/* Skeleton: theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115 },

	/* Zombie: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115 },

	/* Vampire: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115 },

	/* Spectre: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115 },

	/* Sprite: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 105, 115 },

	/* Beastman: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115 },

	/* Ent */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  120, 120, 105, 120, 110, 105, 125, 125, 110, 115, 108,
	  120, 115, 110, 110, 120, 110, 100, 125, 100,  95, 140,
	  110, 110, 105, 110 },

	/* Angel */
	{  95,  95,  95,  95,  95,  95,  95,  95,  95,  95,  95,
	   95,  95,  95,  95,  95,  95,  95,  95,  95,  95,  95,
	   95,  95,  95,  95,  95,  95,  95,  95,  95,  95, 160,
	   95,  95,  95,  95 },

	/* Demon */
	{ 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
	  140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
	  140, 140, 140, 140, 140, 140, 140, 140, 140, 160, 120,
	  140, 140, 140, 140 },

	/* Dunadan */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 125, 125, 105, 120, 105,  95, 140,
	  100, 120, 110, 100 },

	/* Shadow Fairy */
	{ 110, 110, 110, 115, 120, 130, 115, 115, 120, 110, 115,
	  115, 115, 116, 115, 120, 120, 115, 115, 101, 110, 110,
	  110, 110, 112, 122, 110, 110, 110, 115, 110, 120, 120,
	  110, 101, 115, 110 },

	/* Kutar */
	{ 110, 110, 105, 105, 110, 115, 115, 115, 110, 105, 110,
	  115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
	  115, 115, 125, 125, 125, 125, 105, 115, 105,  95, 140,
	  110, 115, 100, 110 },

	/* Android */
	{ 105, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 125, 125, 105, 120, 105,  95, 140,
	  100, 120, 110, 100 },
};
#endif


//v1.1.64 ���i�ω��v�ffactor�l�����̃p�����[�^�ɂ���ĕς�邩���肵adj_chr_gold[]�p�z��Y��0-47��Ԃ�
static int select_price_factor_idx(object_type *o_ptr)
{
	int idx;

	//�i���Ə���͖{�̔��������ӁB���͂ƒm��+10�̍��������g����
	if ((p_ptr->pclass == CLASS_LIBRARIAN || p_ptr->pclass == CLASS_KOSUZU) && o_ptr->tval <= TV_BOOK_END)
		idx = MAX(p_ptr->stat_ind[A_INT] + 10, p_ptr->stat_ind[A_CHR]);
	//�Ó���͒m�͂Ɩ��͂̍��������g����
	else if (p_ptr->pclass == CLASS_SH_DEALER)
		idx = MAX(p_ptr->stat_ind[A_INT] , p_ptr->stat_ind[A_CHR]);
	//v1.1.46 �����̓��x���Ɩ��͂̂ǂ��炩���������g����
	else if (p_ptr->pclass == CLASS_JYOON)
		idx = MAX((p_ptr->lev - 3), p_ptr->stat_ind[A_CHR]);
	//����ȊO�͖��͂��g��
	else 
		idx = p_ptr->stat_ind[A_CHR];

	//idx�͈͏���
	if (idx < 0) idx = 0;
	if (idx > 47) idx = 47;

	return idx;
}

/*
 * Determine the price of an item (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * item, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
 /*:::�A�C�e���̒l�i�����肷��B��{���i�ɓX��Ƃ̑����⁗�̖��͂�����*/
 ///sys item �X�ł̃A�C�e���̒l�i�@�I�[�i�[�푰�ȂǍl��
 //flip:��������Ƃ�TRUE
static s32b price_item(object_type *o_ptr, int greed, bool flip)
{
	int 	factor;
	int 	adjust;
	s32b	price;


	/* Get the value of one of the items */
	price = object_value(o_ptr);

	/* Worthless items */
	if (price <= 0) return (0L);


	/* Compute the racial factor */
	/*:::�푰�����ɂ��l�i�C���l factor�͍����قǂڂ���������*/
	///bldg131222 �X�̉��i�̎푰�����폜�@������͓V��̒��Ől�Ԃ��J������Ƃ��������Ă�������
	//factor = rgold_adj[ot_ptr->owner_race][p_ptr->prace];
	/* Add in the charisma factor */

	//v1.1.64 factor�lidx�����ʊ֐��ɕ���
	factor = adj_chr_gold[select_price_factor_idx(o_ptr)] * 2;

	/* Shop is buying */
	if (flip)
	{
		/* Adjust for greed */
	//factor:260(����3)�`136(����50) greed:����140

		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (cur_store_num == STORE_BLACK)
			price = price / 2;

		/* Compute the final price (with rounding) */
		/* Hack -- prevent underflow */
		price = (price * adjust + 50L) / 100L;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (cur_store_num == STORE_BLACK)
			price = price * 2;


		/* Compute the final price (with rounding) */
		/* Hack -- prevent overflow */

		//v1.1.61 �I�[�o�[�t���[�}�~
		if(price > 10000000L)
			price = (s32b)((u32b)price  / 100UL * (u32b)adjust);
		else
			price = (s32b)(((u32b)price * (u32b)adjust + 50UL) / 100UL);
	}

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	//v1.1.61 �I�[�o�[�t���[�}�~
	if (price > 99999999) return (99999999L);

	/* Return the price */
	return (price);
}


/*
 * Certain "cheap" objects should be created in "piles"
 * Some objects can be sold at a "discount" (in small piles)
 */
///item TVAL �X�ɃA�C�e������ׂ�Ƃ��̌�����
///mod131223 TVAL
static void mass_produce(object_type *o_ptr)
{
	int size = 1;
	int discount = 0;
	int yamawaro_card_num;

	s32b cost = object_value(o_ptr);


	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Food, Flasks, and Lites */
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		case TV_SOFTDRINK: //v1.1.78�ǉ�
		{
			if (cost <= 5L) size += damroll(3, 5);
			if (cost <= 20L) size += damroll(3, 5);
			if (cost <= 50L) size += damroll(2, 2);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			if (cost <= 60L) size += damroll(3, 5);
			if (cost <= 240L) size += damroll(1, 5);
			if (o_ptr->sval == SV_SCROLL_STAR_IDENTIFY) size += damroll(3, 5);
			if (o_ptr->sval == SV_SCROLL_STAR_REMOVE_CURSE) size += damroll(1, 4);
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
		case TV_BOOK_OCCULT:

		{
			if (cost <= 50L) size += damroll(2, 3);
			if (cost <= 500L) size += damroll(1, 3);
			break;
		}

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

	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HEAD:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_CLOTHES:
	case TV_ARMOR:
		{
			if (object_is_artifact(o_ptr)) break;
			if (object_is_ego(o_ptr)) break;
			if (cost <= 10L) size += damroll(3, 5);
			if (cost <= 100L) size += damroll(3, 5);
			break;
		}

		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (cost <= 5L) size += damroll(5, 5);
			if (cost <= 50L) size += damroll(5, 5);
			if (cost <= 500L) size += damroll(5, 5);
			break;
		}

		case TV_FIGURINE:
		{
			if (cost <= 100L) size += damroll(2, 2);
			if (cost <= 1000L) size += damroll(2, 2);
			break;
		}

		case TV_CAPTURE:
		//case TV_STATUE:
		//case TV_CARD:
		{
			size = 1;
			break;
		}
		/*
		 * Because many rods (and a few wands and staffs) are useful mainly
		 * in quantity, the Black Market will occasionally have a bunch of
		 * one kind. -LM-
		 */
		case TV_ROD:
		case TV_WAND:
		case TV_STAFF:
		{
			if ((cur_store_num == STORE_BLACK) && one_in_(3))
			{
				if (cost < 1601L) size += damroll(1, 5);
				else if (cost < 3201L) size += damroll(1, 3);
			}
			break;
		}
	}

	//Extra���[�h�̃_���W�������X�͐����������Ȃ�
	if(EXTRA_MODE && !object_is_artifact(o_ptr))
	{
		if(o_ptr->tval == TV_ARROW || o_ptr->tval == TV_BOLT || o_ptr->tval == TV_OTHERWEAPON && o_ptr->sval == SV_OTHERWEAPON_NEEDLE)
			size = 30 + randint1(30);//v1.1.62 3�{�ɂ���
		else if(o_ptr->tval == TV_KNIFE && (o_ptr->sval == SV_WEAPON_DAGGER || o_ptr->sval == SV_WEAPON_KNIFE))
		{
			if(object_is_ego(o_ptr))
				size = randint1(dun_level/15+1);
			else
				size = 5 + randint1(5);
		}
		else
			size /= 7;

	}

	if(size < 1) size = 1;



	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (one_in_(25))
	{
		discount = 25;
	}
	else if (one_in_(150))
	{
		discount = 50;
	}
	else if (one_in_(300))
	{
		discount = 75;
	}
	else if (one_in_(500))
	{
		discount = 90;
	}

	//v1.1.86 �R���I�������p
	yamawaro_card_num = count_ability_card(ABL_CARD_YAMAWARO);
	if (yamawaro_card_num)
	{

		if (one_in_(12 - yamawaro_card_num) && discount < 50) discount = 50;


	}


	if (o_ptr->art_name)
	{
		if (cheat_peek && discount)
		{
#ifdef JP
msg_print("�����_���A�[�e�B�t�@�N�g�͒l�����Ȃ��B");
#else
			msg_print("No discount on random artifacts.");
#endif

		}
		discount = 0;
	}

	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);


	/* Ensure that mass-produced rods and wands get the correct pvals. */
	if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
	{
		o_ptr->pval *= o_ptr->number;
	}
}



/*
 * Determine if a store item can "absorb" another item
 *
 * See "object_similar()" for the same function for the "player"
 */
/*:::�A�C�e�����܂Ƃ߂��邩���肷��i�X�p�j*/
static bool store_object_similar(object_type *o_ptr, object_type *j_ptr)
{
	int i;

	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr) return (0);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);

	/* Different charges (etc) cannot be stacked, unless wands or rods. */
	if ((o_ptr->pval != j_ptr->pval) && (o_ptr->tval != TV_WAND) && (o_ptr->tval != TV_ROD)) return (0);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h) return (0);
	if (o_ptr->to_d != j_ptr->to_d) return (0);
	if (o_ptr->to_a != j_ptr->to_a) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2 != j_ptr->name2) return (0);

	/* Artifacts don't stack! */
	if (object_is_artifact(o_ptr) || object_is_artifact(j_ptr)) return (0);

	/* Hack -- Identical art_flags! */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		if (o_ptr->art_flags[i] != j_ptr->art_flags[i]) return (0);

	/* Hack -- Never stack "powerful" items */
	/*:::���̃t���O�͎g���Ă��Ȃ��炵��*/
	if (o_ptr->xtra1 || j_ptr->xtra1) return (0);

	/* Hack -- Never stack recharging items */
	if (o_ptr->timeout || j_ptr->timeout) return (0);

	/* Require many identical values */
	if (o_ptr->ac != j_ptr->ac)   return (0);
	if (o_ptr->dd != j_ptr->dd)   return (0);
	if (o_ptr->ds != j_ptr->ds)   return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (0);
	///del131210
	//if (o_ptr->tval == TV_STATUE) return (0);
	if (o_ptr->tval == TV_CAPTURE) return (0);
	if (o_ptr->tval == TV_MAGICITEM) return (0);

	/* Require matching discounts */
	if (o_ptr->discount != j_ptr->discount) return (0);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store item to absorb another item
 */
/*:::�A�C�e�����܂Ƃ߂�i�X�p�j*/
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int max_num = (o_ptr->tval == TV_ROD) ?
		MIN(99, MAX_SHORT / k_info[o_ptr->k_idx].pval) : 99;
	int total = o_ptr->number + j_ptr->number;
	int diff = (total > max_num) ? total - max_num : 0;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > max_num) ? max_num : total;

	//v1.1.91 �����܂𔄂����Ƃ��A�c�莞��(xtra4)���������ɑ�������
	if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_TORCH)
	{
		o_ptr->xtra4 = MAX(o_ptr->xtra4, j_ptr->xtra4);
	}

	/* Hack -- if rods are stacking, add the pvals (maximum timeouts) together. -LM- */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;
	}

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if (o_ptr->tval == TV_WAND)
	{
		o_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;
	}
}


/*
 * Check to see if the shop will be carrying too many objects	-RAK-
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.	Before, one could "nuke" potions this way.
 *
 * Return value is now int:
 *  0 : No space
 * -1 : Can be combined to existing slot.
 *  1 : Cannot be combined but there are empty spaces.
 */
/*:::�Ƃ�X�Ɏw�肵���A�C�e����������邩�`�F�b�N�@�ڍז���*/
static int store_check_num(object_type *o_ptr)
{
	int 	   i;
	object_type *j_ptr;


	/* The "home" acts like the player */
	if ((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM))
	{
		bool old_stack_force_notes = stack_force_notes;
		bool old_stack_force_costs = stack_force_costs;

		if (cur_store_num != STORE_HOME)
		{
			stack_force_notes = FALSE;
			stack_force_costs = FALSE;
		}

		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr))
			{
				if (cur_store_num != STORE_HOME)
				{
					stack_force_notes = old_stack_force_notes;
					stack_force_costs = old_stack_force_costs;
				}

				return -1;
			}
		}

		if (cur_store_num != STORE_HOME)
		{
			stack_force_notes = old_stack_force_notes;
			stack_force_costs = old_stack_force_costs;
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return -1;
		}
	}

	/* Free space is always usable */
	/*
	 * �I�v�V���� powerup_home ���ݒ肳��Ă����
	 * �䂪�Ƃ� 20 �y�[�W�܂Ŏg����
	 */
	if ((cur_store_num == STORE_HOME) && ( powerup_home == FALSE )) {
		if (st_ptr->stock_num < ((st_ptr->stock_size) / 10)) {
			return 1;
		}
	}
	else{
		if (st_ptr->stock_num < st_ptr->stock_size) {
			return 1;
		}
	}

	/* But there was no room at the inn... */
	return 0;
}


static bool is_blessed(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_BLESSED)) return (TRUE);
	else return (FALSE);
}

/*:::�G���W�j�A�p�̒e��A�G�l���M�[�p�b�N�ނ̂Ƃ�TRUE �G�݉��ȂǂŔ������s�ɂ��邽�߂̔���*/
static bool object_is_munition(object_type *o_ptr)
{
	if(o_ptr->tval == TV_MATERIAL && o_ptr->sval >= SV_MATERIAL_MOTOR_BULLET && o_ptr->sval <= SV_MATERIAL_CANNONBALL ) return TRUE;
	if(o_ptr->tval == TV_FLASK && o_ptr->sval >= SV_FLASK_BATTERY && o_ptr->sval <= SV_FLASK_ENERGY_HYPERSPACE ) return TRUE;	

	return FALSE;

}

/*
 * Determine if the current store will purchase the given item
 *
 * Note that a shop-keeper must refuse to buy "worthless" items
 */
/*:::������X���w�肵���A�C�e���𔃂���肽���邩����*/
/*:::�A�C�e���ύX�̂Ƃ��傫������������K�v�L��*/
///item TVAL �X��������肽����A�C�e��
///mod131223 TVAL�����
///mod140314 ���@�X�̕i�������������̂ŏ��X�Ŋ������������Ƃɂ���
static bool store_will_buy(object_type *o_ptr)
{
	/* Hack -- The Home is simple */
	if ((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM)) return (TRUE);



	/* Switch on the store */
	switch (cur_store_num)
	{
		/* General Store */
		case STORE_GENERAL:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SOFTDRINK:
				case TV_MUSHROOM:
				case TV_MATERIAL:
				case TV_WHISTLE:
				case TV_SWEETS:
				case TV_FOOD:
				case TV_LITE:
				case TV_BULLET:
				case TV_ARROW:
				case TV_BOLT:
				case TV_CLOAK:
				case TV_SOUVENIR:
				case TV_RIBBON:
				break;
				//�G�݉��͕���h���ފƂƉ��Ƃ�͂��̂ݗ~������
				case TV_ARMOR:
				{
					if(o_ptr->sval != SV_ARMOR_TUB) return (FALSE);
				}
				break;
				case TV_OTHERWEAPON:	
				{
					if(o_ptr->sval != SV_OTHERWEAPON_FISHING && o_ptr->sval != SV_OTHERWEAPON_NEEDLE) return (FALSE);
				}
				break;
				case TV_AXE:
				{
					if(o_ptr->sval != SV_WEAPON_PICK) return (FALSE);
				}
				break;
				///mod140819 TV_FLASK�ɑ��̃A�C�e�����₵���̂Ŗ��₾���Ώۂɂ���悤�C��
				case TV_FLASK:
				{
					if(o_ptr->sval != SV_FLASK_OIL) return (FALSE);
				}
				break;
				///mod150912 �G�݉��͓݊�J�e�S�����n�̂ݔ������
				case TV_HAMMER:
				{
					if(o_ptr->sval != SV_WEAPON_KINE) return (FALSE);
				}
				break;
				//v1.1.34 �G�݉��͊}���������
				case TV_HEAD:
				{
					if(o_ptr->sval != SV_HEAD_KASA) return (FALSE);
				}
				break;

				default:
				return (FALSE);
			}
			break;
		}

		/* Armoury */
		case STORE_ARMOURY:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{

			case TV_BOOTS:
			case TV_GLOVES:
			case TV_HEAD:
			case TV_SHIELD:
			case TV_CLOAK:
			case TV_CLOTHES:
			case TV_DRAG_ARMOR:
			case TV_RIBBON:
			break;
			case TV_ARMOR:
				{
					if(o_ptr->sval == SV_ARMOR_TUB) return (FALSE);
				}
			break;

				default:
				return (FALSE);
			}
			break;
		}

		/* Weapon Shop */
		case STORE_WEAPON:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_BULLET:
				case TV_ARROW:
				case TV_BOLT:

				case TV_BOW:
				case TV_CROSSBOW:
				case TV_GUN:

				case TV_KNIFE:
				case TV_SWORD:
				case TV_KATANA:
				case TV_HAMMER:

				case TV_AXE:
				case TV_SPEAR:
				case TV_POLEARM:		

		  		case TV_BOOK_HISSATSU:
				break;
				case TV_STICK:
				{
					if(o_ptr->sval == SV_WEAPON_WIZSTAFF) return (FALSE);
				}
				break;
				case TV_OTHERWEAPON:	
				{
					if(o_ptr->sval == SV_OTHERWEAPON_FISHING) return (FALSE);
				}
				break;
				case TV_MAGICITEM:
				{
					if (o_ptr->sval != SV_MAGICITEM_QUIVER) return (FALSE);
				}
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Temple */
		/*:::�򉮁E�����ɕύX*/
		case STORE_TEMPLE:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{

			  	case TV_BOOK_MEDICINE: 
				case TV_POTION:
				case TV_ALCOHOL:
				case TV_SOFTDRINK:
				case TV_MATERIAL:
				case TV_MUSHROOM:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Alchemist */
		/*:::�����X�ɕύX*/
		case STORE_ALCHEMIST:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SCROLL:
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
				  case TV_ABILITY_CARD:

					break;
				default:
					return (FALSE);
			}
			break;
		}

		/* Magic Shop */
		case STORE_MAGIC:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_AMULET:
				case TV_RING:
				case TV_STAFF:
				case TV_WAND:
				case TV_ROD:
				case TV_SCROLL:
				case TV_POTION:
				case TV_FIGURINE:
				case TV_MAGICITEM:
				case TV_MAGICWEAPON:
				case TV_MATERIAL:
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
				case TV_ABILITY_CARD:

				break;
				case TV_STICK:
				{
					if(o_ptr->sval == SV_WEAPON_WIZSTAFF) break;
					else return (FALSE);
				}
				default:
				return (FALSE);
			}
			break;
		}
		/* Bookstore Shop */

		case STORE_BOOK:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
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
  				case TV_CAPTURE:
		 		case TV_MAGICITEM:
			  	  case TV_BOOK_MEDICINE: 
				case TV_SCROLL:
				case TV_BOOK_OCCULT:
				case TV_ABILITY_CARD:
					break;
				default:
					return (FALSE);
			}
			break;
		}
	}

	/* XXX XXX XXX Ignore "worthless" items */
	if (object_value(o_ptr) <= 0) return (FALSE);

	///mod150124 �e��ނ͔��p�s�@�G���W�j�A�������B�j�~
	if(object_is_munition(o_ptr)) return (FALSE);

	//v1.1.86 �A�r���e�B�J�[�h�͔��p�s�ɂ��Ă���
	if (o_ptr->tval == TV_ABILITY_CARD) return FALSE;


	/* Assume okay */
	return (TRUE);
}


/*
 * Combine and reorder items in the home
 */
/*:::�X�i�����فA����܂ށj�A�C�e�����܂Ƃ߂Đ���*/
bool combine_and_reorder_home(int store_num)
{
	int         i, j, k;
	s32b        o_value;
	object_type forge, *o_ptr, *j_ptr;
	bool        flag = FALSE, combined;
	store_type  *old_st_ptr = st_ptr;
	bool        old_stack_force_notes = stack_force_notes;
	bool        old_stack_force_costs = stack_force_costs;

	st_ptr = &town[1].store[store_num];
	if (store_num != STORE_HOME)
	{
		stack_force_notes = FALSE;
		stack_force_costs = FALSE;
	}

	do
	{
		combined = FALSE;

		/* Combine the items in the home (backwards) */
		/*::: i:�X�̃A�C�e������납�烋�[�v*/
		for (i = st_ptr->stock_num - 1; i > 0; i--)
		{
			/* Get the item */
			o_ptr = &st_ptr->stock[i];

			/* Skip empty items */
			if (!o_ptr->k_idx) continue;

			/* Scan the items above that item */
			/*::: j:�X�̃A�C�e����O���烋�[�v*/
			for (j = 0; j < i; j++)
			{
				int max_num;

				/* Get the item */
				j_ptr = &st_ptr->stock[j];

				/* Skip empty items */
				if (!j_ptr->k_idx) continue;

				/*
				 * Get maximum number of the stack if these
				 * are similar, get zero otherwise.
				 */
				/*:::�A�C�e�����܂Ƃ߂��邩����*/
				max_num = object_similar_part(j_ptr, o_ptr);

				/* Can we (partialy) drop "o_ptr" onto "j_ptr"? */

				if (max_num && j_ptr->number < max_num)
				{
				//max_num(��{99�����ꕔ���b�h�̂�99���Ⴂ)�ȓ��Ȃ畁�ʂɂ܂Ƃ߂�
					if (o_ptr->number + j_ptr->number <= max_num)
					{
						/* Add together the item counts */
						/*:::�A�C�e�����܂Ƃ߂�*/
						object_absorb(j_ptr, o_ptr);

						/* One object is gone */
						st_ptr->stock_num--;

						/* Slide everything down */
						/*::: i�����ɂ���A�C�e���������炷(i�̃A�C�e���͏�����)?*/
						for (k = i; k < st_ptr->stock_num; k++)
						{
							/* Structure copy */
							st_ptr->stock[k] = st_ptr->stock[k + 1];
						}

						/* Erase the "final" slot */
						object_wipe(&st_ptr->stock[k]);
					}
					//max�𒴂���ꍇ������
					else
					{
						int old_num = o_ptr->number;
						int remain = j_ptr->number + o_ptr->number - max_num;

						/* Add together the item counts */
						object_absorb(j_ptr, o_ptr);

						o_ptr->number = remain;

						/* Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM- */
						if (o_ptr->tval == TV_ROD)
						{
							o_ptr->pval =  o_ptr->pval * remain / old_num;
							o_ptr->timeout = o_ptr->timeout * remain / old_num;
						}

						/* Hack -- if wands are stacking, combine the charges. -LM- */
						else if (o_ptr->tval == TV_WAND)
						{
							o_ptr->pval = o_ptr->pval * remain / old_num;
						}
					}

					/* Take note */
					combined = TRUE;

					/* Done */
					break;
				}
			}
		}

		flag |= combined;
	}
	while (combined);

	/* Re-order the items in the home (forwards) */
	/*:::�A�C�e�������l���ɐ���*/
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		/* Get the item */
		o_ptr = &st_ptr->stock[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Get the "value" of the item */
		o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			if (object_sort_comp(o_ptr, o_value, &st_ptr->stock[j])) break;
		}

		/* Never move down */
		if (j >= i) continue;

		/* Take note */
		flag = TRUE;

		/* Get local object */
		j_ptr = &forge;

		/* Save a copy of the moving item */
		object_copy(j_ptr, &st_ptr->stock[i]);

		/* Slide the objects */
		for (k = i; k > j; k--)
		{
			/* Slide the item */
			object_copy(&st_ptr->stock[k], &st_ptr->stock[k - 1]);
		}

		/* Insert the moving item */
		object_copy(&st_ptr->stock[j], j_ptr);
	}

	st_ptr = old_st_ptr;
	if (store_num != STORE_HOME)
	{
		stack_force_notes = old_stack_force_notes;
		stack_force_costs = old_stack_force_costs;
	}

	return flag;
}


/*
 * Add the item "o_ptr" to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" bacoming
 * known, the player may have to pick stuff up and drop it again.
 */
static int home_carry(object_type *o_ptr)
{
	int 				slot;
	s32b			   value;
	int 	i;
	object_type *j_ptr;
	bool old_stack_force_notes = stack_force_notes;
	bool old_stack_force_costs = stack_force_costs;

	if (cur_store_num != STORE_HOME)
	{
		stack_force_notes = FALSE;
		stack_force_costs = FALSE;
	}

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* The home acts just like the player */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, o_ptr);

			if (cur_store_num != STORE_HOME)
			{
				stack_force_notes = old_stack_force_notes;
				stack_force_costs = old_stack_force_costs;
			}

			/* All done */
			return (slot);
		}
	}

	if (cur_store_num != STORE_HOME)
	{
		stack_force_notes = old_stack_force_notes;
		stack_force_costs = old_stack_force_costs;
	}

	/* No space? */
	/*
	 * �B���@�\: �I�v�V���� powerup_home ���ݒ肳��Ă����
	 *           �䂪�Ƃ� 20 �y�[�W�܂Ŏg����
	 */
	/* No space? */
	if ((cur_store_num != STORE_HOME) || (powerup_home == TRUE)) {
		if (st_ptr->stock_num >= st_ptr->stock_size) {
			return (-1);
		}
	}
	else{
		if (st_ptr->stock_num >= ((st_ptr->stock_size) / 10)) {
			return (-1);
		}
	}


	/* Determine the "value" of the item */
	value = object_value(o_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		if (object_sort_comp(o_ptr, value, &st_ptr->stock[slot])) break;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i-1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	chg_virtue(V_SACRIFICE, -1);

	(void)combine_and_reorder_home(cur_store_num);

	/* Return the location */
	return (slot);
}


/*
 * Add the item "o_ptr" to a real stores inventory.
 *
 * If the item is "worthless", it is thrown away (except in the home).
 *
 * If the item cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.	Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an item to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
/*:::�w�肳�ꂽ�A�C�e����������X�̏��i�ɒǉ�����*/
/*:::�Ԃ�l��slot�ԍ��@���s������-1*/
static int store_carry(object_type *o_ptr)
{
	int 	i, slot;
	s32b	value, j_value;
	object_type *j_ptr;


	/* Evaluate the object */
	value = object_value(o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	/*:::�����l�ȃA�C�e���͏I���@�������Ӓ�̖����l�ȃA�C�e���𔄂�����Ƃ��p��*/
	if (value <= 0) return (-1);

	/* All store items are fully *identified* */
	o_ptr->ident |= IDENT_MENTAL;

	/* Erase the inscription */
	/*:::���͏�����*/
	o_ptr->inscription = 0;

	/* Erase the "feeling" */
	o_ptr->feeling = FEEL_NONE;

	/* Check each existing item (try to combine) */
	/*:::�A�C�e�����܂Ƃ߂���ꍇ�܂Ƃ߂�*/
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* Hack -- extra items disappear */
			store_object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size) return (-1);

	/*:::����*/
	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/*
		 * Hack:  otherwise identical rods sort by
		 * increasing recharge time --dsb
		 */
		if (o_ptr->tval == TV_ROD)
		{
			if (o_ptr->pval < j_ptr->pval) break;
			if (o_ptr->pval > j_ptr->pval) continue;
		}

		/* Evaluate that slot */
		j_value = object_value(j_ptr);

		/* Objects sort by decreasing value */
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i-1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	/* Return the location */
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.	This can result in zero items.
 */
static void store_item_increase(int item, int num)
{
	int 		cnt;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = o_ptr->number + num;
	if (cnt > 255) cnt = 255;
	else if (cnt < 0) cnt = 0;
	num = cnt - o_ptr->number;

	/* Save the new number */
	o_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(int item)
{
	int 		j;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Must exist */
	if (!o_ptr->k_idx) return;

	/* Must have no items */
	if (o_ptr->number) return;

	/* One less item */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
	{
		st_ptr->stock[j] = st_ptr->stock[j + 1];
	}

	/* Nuke the final slot */
	object_wipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any item that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
/*:::���̓X�Ŕ����Ă���̂Ɠ������̃A�C�e���𔻒�@�����Ń`�F�b�N�ɒʂ����A�C�e���̓u���}�̏��i���珜�O�����*/
static bool black_market_crap(object_type *o_ptr)
{
	int 	i, j;

	/* Ego items are never crap */
	if (object_is_ego(o_ptr)) return (FALSE);

	/* Good items are never crap */
	if (o_ptr->to_a > 0) return (FALSE);
	if (o_ptr->to_h > 0) return (FALSE);
	if (o_ptr->to_d > 0) return (FALSE);

	/* Check all stores */
	for (i = 0; i < MAX_STORES; i++)
	{
		if (i == STORE_HOME) continue;
		if (i == STORE_MUSEUM) continue;

		/* Check every item in the store */
		for (j = 0; j < town[p_ptr->town_num].store[i].stock_num; j++)
		{
			object_type *j_ptr = &town[p_ptr->town_num].store[i].stock[j];

			/* Duplicate item "type", assume crappy */
			if (o_ptr->k_idx == j_ptr->k_idx) return (TRUE);
		}
	}

	/* Assume okay */
	return (FALSE);
}


/*
 * Attempt to delete (some of) a random item from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
/*:::�X�̃A�C�e���������_���Ɉ�폜�@������ĂԑO�ɃO���[�o���ϐ�st_ptr�ɑΏۂ̓X�̏�񂪊i�[����Ă��邱��*/
static void store_delete(void)
{
	int what, num;

	/* Pick a random slot */
	what = randint0(st_ptr->stock_num);

	/* Determine how many items are here */
	num = st_ptr->stock[what].number;

	/* Hack -- sometimes, only destroy half the items */
	if (randint0(100) < 50) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single item */
	if (randint0(100) < 50) num = 1;

	/* Hack -- decrement the maximum timeouts and total charges of rods and wands. -LM- */
	if ((st_ptr->stock[what].tval == TV_ROD) || (st_ptr->stock[what].tval == TV_WAND))
	{
		st_ptr->stock[what].pval -= num * st_ptr->stock[what].pval / st_ptr->stock[what].number;
	}

	/* Actually destroy (part of) the item */
	store_item_increase(what, -num);
	store_item_optimize(what);
}



/*
 * Creates a random item and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 * Currently, "normal" stores use a pre-built array.
 *:::�X�ɃA�C�e����ǉ��B���̃A���S���Y���ɂ͖�肪�����H
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given item?
 */
/*:::st_ptr�Ŏ������X�ɃA�C�e������ǉ��@�ǉ�����Ȃ���������*/
///mod140313 �X�̂���X�ɂ���Đ������x����ς��A�x�[�X�A�C�e�������get_obj_num_hook�ōs�����Ƃɂ���
static void store_create(void)
{
	int i, tries, level;

	object_type forge;
	object_type *q_ptr;


	/* Paranoia -- no room left */
	/*:::�X�̃A�C�e������t�Ȃ�return ���������̊֐��͂����łȂ����Ƃ��m�F���Ă���Ă΂�Ă���*/
	if (st_ptr->stock_num >= st_ptr->stock_size) return;


	/* Hack -- consider up to four items */
	for (tries = 0; tries < 4; tries++)
	{
		int manekineko_card_num;
		int lev_bonus=0;

		i = 0;//�����A�C�e����k_idx

		//v1.1.86 �������ȏ����L
		manekineko_card_num = count_ability_card(ABL_CARD_MANEKINEKO);
		if (manekineko_card_num)
		{
			lev_bonus = manekineko_card_num * 3;
			if (manekineko_card_num == 9) lev_bonus += 3;

		}


		/* Black Market */
		if (cur_store_num == STORE_BLACK)
		{
			/* Pick a level for object/magic */
			//level = 25 + randint0(25);
			if(p_ptr->town_num==TOWN_HITOZATO) 	level = 30+randint0(30);//�l��
			else if(p_ptr->town_num==TOWN_TENGU) level = 35+randint0(30);//�V��̗�
			else if(p_ptr->town_num==TOWN_CHITEI) level = 40+randint0(30);//�n��
			else if(p_ptr->town_num==TOWN_KAPPA) level =  40+randint0(40);//�͓��̃o�U�[
			else if(p_ptr->town_num==TOWN_KOURIN) //������
			{
				level = 30+randint0(60);
				if(weird_luck()) level += randint0(50);
			}
			else
			{
				int tmp = dun_level + 20 + randint0(20);
				level = MIN(127,tmp); //�_���W������BM 
			}



			//v1.1.86 �������ȏ����L
			level += lev_bonus;
			if (level > 127) level = 127;

			//v1.1.60 *�^�E����*���BM�Ɏ��W�p���z�i������
			//v1.1.60b �I�v�V�����ł�ON/OFF�ɂ���
			if (bm_sells_antique)
			{
				bool flag_generate = TRUE;
				int j;

				//���ł�TV_ANTIQUE�̃A�C�e�����݌ɂɂ��邩�ǂ������肵�A�Ȃ���ΐ���
				for (j = 0; j < st_ptr->stock_num; j++)
				{
					if (st_ptr->stock[j].tval == TV_ANTIQUE) flag_generate = FALSE;
				}

				if (flag_generate)
				{
					if(!one_in_(3)) 
						i = lookup_kind(TV_ANTIQUE, 1);
					else if(!one_in_(3))
						i = lookup_kind(TV_ANTIQUE, 2);
					else if (!one_in_(3))
						i = lookup_kind(TV_ANTIQUE, 3);
					else if (!one_in_(3))
						i = lookup_kind(TV_ANTIQUE, 4);
					else
						i = lookup_kind(TV_ANTIQUE, 5);
				}
			}

			/* Random item (usually of given level) */
			if (!i) i = get_obj_num(level);

			/* Handle failure */
			if (!i) continue;
		}

		/* Normal Store */
		/*:::BM�ȊO�̓X�ł͗\�ߏ������ꂽ�f�[�^���珤�i�̃x�[�X�A�C�e�������߂Ă���炵��*/
		/*:::init2.c��store_table[]��idx�l�ɕϊ�����table[]����A�C�e����I��ł���B*/
		/*:::BM�ȊO�̓X���A�C�e��������get_obj_num_prep()�ɂ��ݒ肷�邱�Ƃɂ����B*/
		else
		{
			/* Hack -- Pick an item to sell */
			//i = st_ptr->table[randint0(st_ptr->table_num)];

			/* Hack -- fake level for apply_magic() */
			//level = rand_range(1, STORE_OBJ_LEVEL);
			if(p_ptr->town_num==TOWN_HITOZATO) level = 5;//�l��
			else if(p_ptr->town_num==TOWN_TENGU) level = 10;//�V��̗�
			else if(p_ptr->town_num==TOWN_CHITEI) level = 20;//�n��
			else if(p_ptr->town_num==TOWN_KOUMA) level = 20;//�g����
			else if(p_ptr->town_num==TOWN_HAKUREI) level = 15;//�_��
			else if(p_ptr->town_num==TOWN_KAPPA) level = 10 + randint1(20);//�͓��̃o�U�[
			else	level = dun_level; //�_���W�����A�[�P�[�h

			level += lev_bonus / 2;
			if (level > 127) level = 127;


			if(cur_store_num == STORE_GENERAL) get_obj_num_hook = store_create_hook_general;
			else if(cur_store_num == STORE_ARMOURY) get_obj_num_hook = store_create_hook_armoury;
			else if(cur_store_num == STORE_WEAPON) get_obj_num_hook = store_create_hook_weapon;
			else if(cur_store_num == STORE_TEMPLE) get_obj_num_hook = store_create_hook_liquor;
			else if(cur_store_num == STORE_ALCHEMIST) get_obj_num_hook = store_create_hook_scroll;
			else if(cur_store_num == STORE_MAGIC) get_obj_num_hook = store_create_hook_magic;
			else if(cur_store_num == STORE_BOOK) get_obj_num_hook = store_create_hook_book;
			else msg_format("store_create���s���Acur_store_num�ɖ���`�̒l(%d)�������Ă���",cur_store_num);


			get_obj_num_prep();
			//�X�݌ɗp�x�[�X�A�C�e������ �x�[�X�A�C�e������Ɍ��背�x�����u�[�X�g���Čv�Z����
			i = get_obj_num(level + 30);
			//hook�����������čēxprep()�����s���Ȃ��ƃA�C�e���e�[�u�����ς�����܂܂ɂȂ�
			get_obj_num_hook = NULL;
			get_obj_num_prep();

			if (!i) continue;
		}


		/* Get local object */
		q_ptr = &forge;

		/* Create a new object of the chosen kind */
		object_prep(q_ptr, i);

		/* Apply some "low-level" magic (no artifacts) */
		apply_magic(q_ptr, level, AM_NO_FIXED_ART);

		///mod140611 ���i�͓X�ɕ��΂Ȃ�
		if(q_ptr->tval == TV_SOUVENIR) continue;
		
		///mod150810 -hack- ��ވ��Ƀi���N�W�̐l�`������
		if(p_ptr->town_num==TOWN_HITOZATO && cur_store_num == STORE_BOOK && q_ptr->tval == TV_FIGURINE)
		{
			q_ptr->pval = MON_G_SLUG;
		}

		///mod160316 �n�����X�ł͒��g����d���{��������悤�ɂ��Ă݂�B�@�l�`�ɔ�ג჌�x���̃����X�^�[���o�ɂ����悤����
		if(p_ptr->town_num == TOWN_UG && cur_store_num == STORE_BOOK && q_ptr->tval == TV_CAPTURE)
		{
			int check;
			monster_race *r_ptr;
			while (1)
			{
				i = randint1(max_r_idx - 1);
				r_ptr = &r_info[i];

				if (!item_monster_okay(i)) continue;
				//if (i == MON_TSUCHINOKO) continue;v1.1.53 ���O��������
				if (!r_ptr->rarity) continue;
				if (r_ptr->rarity > 100) continue;

				if(dun_level < r_ptr->level) check = r_ptr->level - (dun_level);
				else if(dun_level > r_ptr->level*2) check = dun_level - (r_ptr->level *2);
				else check = 0;

				if (randint0(check)) continue;

				break;
			}
			q_ptr->pval = i;
			q_ptr->xtra3 = get_mspeed(r_ptr);
			if (r_ptr->flags1 & RF1_FORCE_MAXHP) q_ptr->xtra4 = maxroll(r_ptr->hdice, r_ptr->hside);
			else	q_ptr->xtra4 = damroll(r_ptr->hdice, r_ptr->hside);
			q_ptr->xtra5 = q_ptr->xtra4;
		}



		/* Require valid object */
		/*:::�X���~������Ȃ��A�C�e�����o���オ�������蒼��*/
		else if (!store_will_buy(q_ptr)) continue;
		/*:::�����^���Ə������[�U*/
		/* Hack -- Charge lite's */
		///item �X�̃����^���[�U
		if (q_ptr->tval == TV_LITE)
		{
			if (q_ptr->sval == SV_LITE_TORCH) q_ptr->xtra4 = FUEL_TORCH / 2;
			if (q_ptr->sval == SV_LITE_LANTERN) q_ptr->xtra4 = FUEL_LAMP / 2;
		}

		/*:::�A�C�e�����Ӓ�ς݂ɂ���*/
		/* The item is "known" */
		object_known(q_ptr);

		/* Mark it storebought */
		q_ptr->ident |= IDENT_STORE;

		/* Mega-Hack -- no chests in stores */
		if (q_ptr->tval == TV_CHEST) continue;

		/* Prune the black market */
		if (cur_store_num == STORE_BLACK)
		{
			/* Hack -- No "crappy" items */
			if (black_market_crap(q_ptr)) continue;

			/* Hack -- No "cheap" items */
			if (object_value(q_ptr) < 10) continue;

			/* No "worthless" items */
			/* if (object_value(q_ptr) <= 0) continue; */
		}

		/* Prune normal stores */
		else
		{
			/* No "worthless" items */
			if (object_value(q_ptr) <= 0) continue;
		}


		/* Mass produce and/or Apply discount */
		mass_produce(q_ptr);

		/* Attempt to carry the (known) item */
		(void)store_carry(q_ptr);

		/* Definitely done */
		break;
	}
}



/*
 * Eliminate need to bargain if player has haggled well in the past
 */
static bool noneedtobargain(s32b minprice)
{
	s32b good = st_ptr->good_buy;
	s32b bad = st_ptr->bad_buy;



	/* Cheap items are "boring" */
	if (minprice < 10L) return (TRUE);

	/* Perfect haggling */
	if (good == MAX_SHORT) return (TRUE);

	/* Reward good haggles, punish bad haggles, notice price */
	if (good > ((3 * bad) + (5 + (minprice/50)))) return (TRUE);

	/* Return the flag */
	return (FALSE);
}


/*
 * Update the bargain info
 */
static void updatebargain(s32b price, s32b minprice, int num)
{
	/* Hack -- auto-haggle */
	if (!manual_haggle) return;

	/* Cheap items are "boring" */
	if ((minprice/num) < 10L) return;

	/* Count the successful haggles */
	if (price == minprice)
	{
		/* Just count the good haggles */
		if (st_ptr->good_buy < MAX_SHORT)
		{
			st_ptr->good_buy++;
		}
	}

	/* Count the failed haggles */
	else
	{
		/* Just count the bad haggles */
		if (st_ptr->bad_buy < MAX_SHORT)
		{
			st_ptr->bad_buy++;
		}
	}
}



/*
 * Re-displays a single store entry
 */
/*:::�X�̃A�C�e������s�\��*/
static void display_entry(int pos)
{
	int 		i, cur_col;
	object_type 	*o_ptr;
	s32b		x;

	char		o_name[MAX_NLEN];
	char		out_val[160];


	int maxwid = 75;

	/* Get the item */
	o_ptr = &st_ptr->stock[pos];

	/* Get the "offset" */
	i = (pos % store_bottom);

	/* Label it, clear the line --(-- */
	(void)sprintf(out_val, "%c) ", ((i > 25) ? toupper(I2A(i - 26)) : I2A(i)));
	prt(out_val, i+6, 0);

	cur_col = 3;
	if (show_item_graph)
	{
		byte a = object_attr(o_ptr);
		char c = object_char(o_ptr);

#ifdef AMIGA
		if (a & 0x80)
			a |= 0x40;
#endif

		Term_queue_bigchar(cur_col, i + 6, a, c, 0, 0);
		if (use_bigtile) cur_col++;

		cur_col += 2;
	}

	/* Describe an item in the home */
	if ((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM))
	{
		maxwid = 75;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 10;

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);
		o_name[maxwid] = '\0';
		c_put_str(tval_to_attr[o_ptr->tval], o_name, i+6, cur_col);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
#ifdef JP
			sprintf(out_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
			put_str(out_val, i+6, 67);
#else
			(void)sprintf(out_val, "%3d.%d lb", wgt / 10, wgt % 10);
			put_str(out_val, i+6, 68);
#endif

		}
	}

	/* Describe an item (fully) in a store */
	else
	{
		/* Must leave room for the "price" */
		maxwid = 65;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 7;

		/* Describe the object (fully) */
		object_desc(o_name, o_ptr, 0);
		o_name[maxwid] = '\0';
		c_put_str(tval_to_attr[o_ptr->tval], o_name, i+6, cur_col);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
#ifdef JP
			sprintf(out_val, "%3d.%1d", lbtokg1(wgt) , lbtokg2(wgt) );
			put_str(out_val, i+6, 60);
#else
			(void)sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, i+6, 61);
#endif

		}

		/* Display a "fixed" cost */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Extract the "minimum" price */
			///bldg �I�[�i�[��min_inflate���Œ�
			//x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);
			x = price_item(o_ptr, min_inflate(), FALSE);

			/* Actually draw the price (not fixed) */
#ifdef JP
(void)sprintf(out_val, "%9ld��", (long)x);
#else
			(void)sprintf(out_val, "%9ld F", (long)x);
#endif

			put_str(out_val, i+6, 68);
		}

		/* Display a "taxed" cost */
		else if (!manual_haggle)
		{
			/* Extract the "minimum" price */
			///bldg131222
			//x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);
			x = price_item(o_ptr, min_inflate(), FALSE);

			/* Hack -- Apply Sales Tax if needed */
			if (!noneedtobargain(x)) x += x / 10;

			/* Actually draw the price (with tax) */
			(void)sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, i+6, 68);
		}

		/* Display a "haggle" cost */
		else
		{
			/* Extrect the "maximum" price */
			//x = price_item(o_ptr, ot_ptr->max_inflate, FALSE);
			x = price_item(o_ptr, max_inflate(), FALSE);

			/* Actually draw the price (not fixed) */
			(void)sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, i+6, 68);
		}
	}
}


/*
 * Displays a store's inventory 		-RAK-
 * All prices are listed as "per individual object".  -BEN-
 */
/*:::�X�̃A�C�e���ꗗ�̂������݂̃y�[�W��\������*/
/*:::���݂̃y�[�W��store_top�Ō��܂�H*/
static void display_inventory(void)
{
	int i, k;

	/* Display the next 12 items */
	for (k = 0; k < store_bottom; k++)
	{
		/* Do not display "dead" items */
		if (store_top + k >= st_ptr->stock_num) break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < store_bottom + 1; i++) prt("", i + 6, 0);

	/* Assume "no current page" */
#ifdef JP
	put_str("          ", 5, 20);
#else
	put_str("        ", 5, 20);
#endif


	/* Visual reminder of "more items" */
	if (st_ptr->stock_num > store_bottom)
	{
		/* Show "more" reminder (after the last item) */
#ifdef JP
		prt("-����-", k + 6, 3);
#else
		prt("-more-", k + 6, 3);
#endif


		/* Indicate the "current page" */
		/* Trailing spaces are to display (Page xx) and (Page x) */
#ifdef JP
		put_str(format("(%d�y�[�W)  ", store_top/store_bottom + 1), 5, 20);
#else
		put_str(format("(Page %d)  ", store_top/store_bottom + 1), 5, 20);
#endif

	}

	if (cur_store_num == STORE_HOME || cur_store_num == STORE_MUSEUM)
	{
		k = st_ptr->stock_size;

		if (cur_store_num == STORE_HOME && !powerup_home) k /= 10;
#ifdef JP
		put_str(format("�A�C�e����:  %4d/%4d", st_ptr->stock_num, k), 19 + xtra_stock, 27);
#else
		put_str(format("Objects:  %4d/%4d", st_ptr->stock_num, k), 19 + xtra_stock, 30);
#endif
	}
}


/*
 * Displays players gold					-RAK-
 */
/*:::�X�̉�ʂɏ�������\��*/
static void store_prt_gold(void)
{
	char out_val[64];

#ifdef JP
	prt("�莝���̂���: ", 19 + xtra_stock, 53);
#else
	prt("Gold Remaining: ", 19 + xtra_stock, 53);
#endif


	sprintf(out_val, "%9ld", (long)p_ptr->au);
	prt(out_val, 19 + xtra_stock, 68);
}


/*
 * Displays store (after clearing screen)		-RAK-
 */
/*:::�X�̉�ʁi�R�}���h�ꗗ�����j��\������*/
static void display_store(void)
{
	char buf[80];


	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (cur_store_num == STORE_HOME)
	{
		/* Put the owner name */
#ifdef JP
		put_str("�T�����_", 3, 31);
#else
		put_str("Your Home", 3, 30);
#endif


		/* Label the item descriptions */
#ifdef JP
		put_str("�A�C�e���̈ꗗ", 5, 4);
#else
		put_str("Item Description", 5, 3);
#endif


		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("�d��", 5, 72);
#else
			put_str("Weight", 5, 70);
#endif

		}
	}

	/* The "Home" is special */
	else if (cur_store_num == STORE_MUSEUM)
	{
		/* Put the owner name */
#ifdef JP
		///msg131210
		put_str("�Ƃ̃G���g�����X", 3, 31);
		///put_str("������", 3, 31);
#else
		put_str("Museum", 3, 30);
#endif


		/* Label the item descriptions */
#ifdef JP
		put_str("�A�C�e���̈ꗗ", 5, 4);
#else
		put_str("Item Description", 5, 3);
#endif


		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("�d��", 5, 72);
#else
			put_str("Weight", 5, 70);
#endif

		}
	}

	/* Normal stores */
	else
	{

		///bldg131222 �X���A�I�[�i�[���ȂǕύX
		//cptr store_name = (f_name + f_info[cur_store_feat].name);
		//cptr owner_name = (ot_ptr->owner_name);

		cptr store_name = new_store_name[p_ptr->town_num][cur_store_num];
		cptr owner_name = new_store_owner_name[p_ptr->town_num][cur_store_num];
		//cptr race_name = race_info[ot_ptr->owner_race].title;

		/* Put the owner name and race */
		//sprintf(buf, "%s (%s)", owner_name, race_name);
		sprintf(buf, "�i%s�j", owner_name);
		prt(buf, 3, 50);

		/* Show the max price in the store (above prices) */
		//sprintf(buf, "%s (%ld)", store_name, (long)(ot_ptr->max_cost));
		sprintf(buf, "%s", store_name);
		put_str(buf, 3, 10);


		/* Label the item descriptions */
#ifdef JP
		put_str("���i�̈ꗗ", 5, 7);
#else
		put_str("Item Description", 5, 3);
#endif


		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("�d��", 5, 62);
#else
			put_str("Weight", 5, 60);
#endif

		}

		/* Label the asking price (in stores) */
#ifdef JP
		put_str("���i", 5, 73);
#else
		put_str("Price", 5, 72);
#endif

	}


	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory();
}



/*
 * Get the ID of a store item and return its value	-RAK-
 */
/*:::�X�̏��i�����I�΂���@�ڍז���*/
static int get_stock(int *com_val, cptr pmt, int i, int j)
{
	char	command;
	char	out_val[160];
	char	lo, hi;

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the item index */
	if (repeat_pull(com_val))
	{
		/* Verify the item */
		if ((*com_val >= i) && (*com_val <= j))
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Assume failure */
	*com_val = (-1);

	/* Build the prompt */
	lo = I2A(i);
	hi = (j > 25) ? toupper(I2A(j - 26)) : I2A(j);
#ifdef JP
	(void)sprintf(out_val, "(%s:%c-%c, ESC�Œ��f) %s",
		(((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM)) ? "�A�C�e��" : "���i"), 
				  lo, hi, pmt);
#else
	(void)sprintf(out_val, "(Items %c-%c, ESC to exit) %s",
				  lo, hi, pmt);
#endif


	/* Ask until done */
	while (TRUE)
	{
		int k;

		/* Escape */
		if (!get_com(out_val, &command, FALSE)) break;

		/* Convert */
		if (islower(command))
			k = A2I(command);
		else if (isupper(command))
			k = A2I(tolower(command)) + 26;
		else
			k = -1;

		/* Legal responses */
		if ((k >= i) && (k <= j))
		{
			*com_val = k;
			break;
		}

		/* Oops */
		bell();
	}

	/* Clear the prompt */
	prt("", 0, 0);

	/* Cancel */
	if (command == ESCAPE) return (FALSE);

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*com_val);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


/*
 * Increase the insult counter and get angry if too many -RAK-
 */
///del140105 �X�匯���x�֘A�폜
static int increase_insults(void)
{
	return (FALSE);
#if 0
	/* Increase insults */
	st_ptr->insult_cur++;

	/* Become insulted */
	if (st_ptr->insult_cur > ot_ptr->insult_max)
	{
		/* Complain */
		say_comment_4();

		/* Reset insults */
		st_ptr->insult_cur = 0;
		st_ptr->good_buy = 0;
		st_ptr->bad_buy = 0;

		/* Open tomorrow */
		st_ptr->store_open = turn + TURNS_PER_TICK*TOWN_DAWN/8 + randint1(TURNS_PER_TICK*TOWN_DAWN/8);

		/* Closed */
		return (TRUE);
	}

	/* Not closed */
	return (FALSE);
#endif
}


/*
 * Decrease insults 				-RAK-
 */
static void decrease_insults(void)
{
	/* Decrease insults */
	if (st_ptr->insult_cur) st_ptr->insult_cur--;
}


/*
 * Have insulted while haggling 			-RAK-
 */
static int haggle_insults(void)
{
	/* Increase insults */
	if (increase_insults()) return (TRUE);

	/* Display and flush insult */
	say_comment_5();

	/* Still okay */
	return (FALSE);
}


/*
 * Mega-Hack -- Enable "increments"
 */
static bool allow_inc = FALSE;

/*
 * Mega-Hack -- Last "increment" during haggling
 */
static s32b last_inc = 0L;


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b *poffer, s32b price, int final)
{
	s32b		i;

	cptr		p;

	char				buf[128];
	char		out_val[160];


	/* Clear old increment if necessary */
	if (!allow_inc) last_inc = 0L;


	/* Final offer */
	if (final)
	{
#ifdef JP
		sprintf(buf, "%s [����] ", pmt);
#else
		sprintf(buf, "%s [accept] ", pmt);
#endif

	}

	/* Old (negative) increment, and not final */
	else if (last_inc < 0)
	{
#ifdef JP
		sprintf(buf, "%s [-$%ld] ", pmt, (long)(ABS(last_inc)));
#else
		sprintf(buf, "%s [-%ld] ", pmt, (long)(ABS(last_inc)));
#endif

	}

	/* Old (positive) increment, and not final */
	else if (last_inc > 0)
	{
#ifdef JP
		sprintf(buf, "%s [+$%ld] ", pmt, (long)(ABS(last_inc)));
#else
		sprintf(buf, "%s [+%ld] ", pmt, (long)(ABS(last_inc)));
#endif

	}

	/* Normal haggle */
	else
	{
		sprintf(buf, "%s ", pmt);
	}


	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Ask until done */
	while (TRUE)
	{
		bool res;

		/* Display prompt */
		prt(buf, 0, 0);

		/* Default */
		strcpy(out_val, "");

		/*
		 * Ask the user for a response.
		 * Don't allow to use numpad as cursor key.
		 */
		res = askfor_aux(out_val, 32, FALSE);

		/* Clear prompt */
		prt("", 0, 0);

		/* Cancelled */
		if (!res) return FALSE;

		/* Skip leading spaces */
		for (p = out_val; *p == ' '; p++) /* loop */;

		/* Empty response */
		if (*p == '\0')
		{
			/* Accept current price */
			if (final)
			{
				*poffer = price;
				last_inc = 0L;
				break;
			}

			/* Use previous increment */
			if (allow_inc && last_inc)
			{
				*poffer += last_inc;
				break;
			}
		}

		/* Normal response */
		else
		{
			/* Extract a number */
			i = atol(p);

			/* Handle "incremental" number */
			if ((*p == '+' || *p == '-'))
			{
				/* Allow increments */
				if (allow_inc)
				{
					/* Use the given "increment" */
					*poffer += i;
					last_inc = i;
					break;
				}
			}

			/* Handle normal number */
			else
			{
				/* Use the given "number" */
				*poffer = i;
				last_inc = 0L;
				break;
			}
		}

		/* Warning */
#ifdef JP
		msg_print("�l�����������ł��B");
#else
		msg_print("Invalid response.");
#endif

		msg_print(NULL);
	}

	/* Success */
	return (TRUE);
}


/*
 * Receive an offer (from the player)
 *
 * Return TRUE if offer is NOT okay
 */
static bool receive_offer(cptr pmt, s32b *poffer,
			  s32b last_offer, int factor,
			  s32b price, int final)
{
	/* Haggle till done */
	while (TRUE)
	{
		/* Get a haggle (or cancel) */
		if (!get_haggle(pmt, poffer, price, final)) return (TRUE);

		/* Acceptable offer */
		if (((*poffer) * factor) >= (last_offer * factor)) break;

		/* Insult, and check for kicked out */
		if (haggle_insults()) return (TRUE);

		/* Reject offer (correctly) */
		(*poffer) = last_offer;
	}

	/* Success */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
/*:::���i�̒l�؂����*/
///bldg �l�؂胋�[�`���폜

static bool purchase_haggle(object_type *o_ptr, s32b *price)
{
	s32b			   cur_ask, final_ask;
	s32b			   last_offer, offer;
	s32b			   x1, x2, x3;
	s32b			   min_per, max_per;
	int 			   flag, loop_flag, noneed;
	int 			   annoyed = 0, final = FALSE;

	bool		cancel = FALSE;

#ifdef JP
	cptr pmt = "�񎦉��i";
#else
	cptr		pmt = "Asking";
#endif


	char		out_val[160];


	*price = 0;


	/* Extract the starting offer and the final offer */
	///blgd131222
	//cur_ask = price_item(o_ptr, ot_ptr->max_inflate, FALSE);
	//final_ask = price_item(o_ptr, ot_ptr->min_inflate, FALSE);
	cur_ask = price_item(o_ptr, max_inflate(), FALSE);	
	final_ask = price_item(o_ptr, min_inflate(), FALSE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* No need to haggle */
	if (noneed || !manual_haggle)
	{
		/* No need to haggle */
		if (noneed)
		{
			/* Message summary */
#ifdef JP
			//v1.1.94 ���b�Z�[�W�폜
			//msg_print("���ǂ��̋��z�ɂ܂Ƃ܂����B");
#else
			msg_print("You eventually agree upon the price.");
#endif

			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
#ifdef JP
			//msg_print("����Ȃ�Ƃ��̋��z�ɂ܂Ƃ܂����B");
#else
			msg_print("You quickly agree upon the price.");
#endif

			//msg_print(NULL);
			if((p_ptr->pclass == CLASS_LIBRARIAN  || p_ptr->pclass == CLASS_KOSUZU)
				&& o_ptr->tval <= TV_BOOK_END)
			{
				msg_print("���Ȃ��͓I�m�ɖ{�̉��i���������B");
				msg_print(NULL);
			}
			else if(p_ptr->pclass == CLASS_SH_DEALER && p_ptr->stat_ind[A_CHR] <  p_ptr->stat_ind[A_INT]+5)
			{
				if(p_ptr->stat_ind[A_INT]+5 < 32) msg_print("���Ȃ��͌��t�I�݂Ɍ����ĉ��i��L���ɂ܂Ƃ߂��B");
				else msg_print("���Ȃ��̌��̎�ۂɒʍs�l���犅�т��������B");
				msg_print(NULL);
			}
			else if (p_ptr->pclass == CLASS_JYOON && p_ptr->stat_ind[A_CHR] < (p_ptr->lev - 3))
			{
				msg_print("���Ȃ��͔\�͂œX��ɗ������������B");
				msg_print(NULL);
			}


			/* Apply Sales Tax */
			final_ask += final_ask / 10;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Go to final offer */
#ifdef JP
		pmt = "���i";
//		pmt = "�ŏI�񎦉��i";
#else
		pmt = "Final Offer";
#endif

		final = TRUE;
	}


	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* Haggle parameters */
	//min_per = ot_ptr->haggle_per;
	min_per = 5;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	///bldg131222
	//last_offer = last_offer * (200 - (int)(ot_ptr->max_inflate)) / 100L;
	last_offer = last_offer * (200 - (int)max_inflate()) / 100L;

	if (last_offer <= 0) last_offer = 1;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;


	//v1.1.94 ���̂ւ�̂����폜�������̊m�F�ɂ���
	(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
	put_str(out_val, 1, 0);	
	*price = final_ask;
	cancel = !get_check_strict("�w�����܂����H", CHECK_DEFAULT_Y);
#if 0
	
	/* Haggle until done */
	for (flag = FALSE; !flag; )
	{
		loop_flag = TRUE;

		while (!flag && loop_flag)
		{
			(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer("�񎦂�����z? ",
#else
			cancel = receive_offer("What do you offer? ",
#endif

					       &offer, last_offer, 1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer > cur_ask)
			{
				say_comment_6();
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}
		}

		if (!flag)
		{
			x1 = 100 * (offer - last_offer) / (cur_ask - last_offer);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((cur_ask - offer) * x2 / 100L) + 1;
			/* don't let the price go up */
			if (x3 < 0) x3 = 0;
			cur_ask -= x3;

			/* Too little */
			if (cur_ask < final_ask)
			{
				final = TRUE;
				cur_ask = final_ask;
#ifdef JP
				pmt = "�ŏI�񎦉��i";
#else
				pmt = "Final Offer";
#endif

				annoyed++;
				if (annoyed > 3)
				{
					(void)(increase_insults());
					cancel = TRUE;
					flag = TRUE;
				}
			}
			else if (offer >= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
#ifdef JP
(void)sprintf(out_val, "�O��̒񎦋��z: $%ld",
#else
				(void)sprintf(out_val, "Your last offer: %ld",
#endif

							  (long)last_offer);
				put_str(out_val, 1, 39);
				say_comment_2(cur_ask, annoyed);
			}
		}
	}

#endif


	/* Cancel */
	if (cancel) return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask, o_ptr->number);

	/* Do not cancel */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
/*:::���p���l�i����*/
///bldg131222 ���p���l�i���폜
static bool sell_haggle(object_type *o_ptr, s32b *price)
{
	s32b    purse, cur_ask, final_ask;
	s32b    last_offer = 0, offer = 0;
	s32b    x1, x2, x3;
	s32b    min_per, max_per;
	int     flag, loop_flag, noneed;
	int     annoyed = 0, final = FALSE;
	bool    cancel = FALSE;
#ifdef JP
	cptr pmt = "�񎦋��z";
#else
	cptr    pmt = "Offer";
#endif

	char    out_val[160];


	*price = 0;


	/* Obtain the starting offer and the final offer */
	///bldg131222
	//cur_ask = price_item(o_ptr, ot_ptr->max_inflate, TRUE);
	//final_ask = price_item(o_ptr, ot_ptr->min_inflate, TRUE);
	cur_ask = price_item(o_ptr, max_inflate(), TRUE);
	final_ask = price_item(o_ptr, min_inflate(), TRUE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* Get the owner's payout limit */
	///bldg131222 ����������ύX
	//purse = (s32b)(ot_ptr->max_cost);
	purse = 99999;

	/* No need to haggle */
	if (noneed || !manual_haggle || (final_ask >= purse))
	{
		/* Apply Sales Tax (if needed) */
		if (!manual_haggle && !noneed)
		{
			final_ask -= final_ask / 10;
		}

		/* No reason to haggle */
		if (final_ask >= purse)
		{
			/* Message */
#ifdef JP

			msg_print("�����ɂ��̋��z�ɂ܂Ƃ܂����B");
#else
			msg_print("You instantly agree upon the price.");
#endif

			msg_print(NULL);

			/* Offer full purse */
			final_ask = purse;
		}

		/* No need to haggle */
		else if (noneed)
		{
			/* Message */
#ifdef JP
		//	msg_print("���ǂ��̋��z�ɂ܂Ƃ܂����B");
#else
			msg_print("You eventually agree upon the price.");
#endif

			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
#ifdef JP
			//msg_print("����Ȃ�Ƃ��̋��z�ɂ܂Ƃ܂����B");
#else
			msg_print("You quickly agree upon the price.");
#endif
			if((p_ptr->pclass == CLASS_LIBRARIAN  || p_ptr->pclass == CLASS_KOSUZU)
				&& o_ptr->tval <= TV_BOOK_END)
			{
				msg_print("���Ȃ��͓I�m�ɔ��l���������B");
				msg_print(NULL);
			}
			else if(p_ptr->pclass == CLASS_SH_DEALER && p_ptr->stat_ind[A_CHR] <  p_ptr->stat_ind[A_INT]+5)
			{
				if(p_ptr->stat_ind[A_INT]+5 < 32) msg_print("���Ȃ��͌��t�I�݂Ɍ����ĉ��i��L���ɂ܂Ƃ߂��B");
				else msg_print("���Ȃ��̌��̎�ۂɒʍs�l���犅�т��������B");
				msg_print(NULL);
			}
			else if (p_ptr->pclass == CLASS_JYOON && p_ptr->stat_ind[A_CHR] < (p_ptr->lev - 3))
			{
				msg_print("���Ȃ��͔\�͂œX��ɎU�����������B");
				msg_print(NULL);
			}


			//msg_print(NULL);
		}

		/* Final price */
		cur_ask = final_ask;

		/* Final offer */
		final = TRUE;
#ifdef JP
		pmt = "���l";
		//pmt = "�ŏI�񎦋��z";
#else
		pmt = "Final Offer";
#endif

	}

	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* XXX XXX XXX Display commands */

	/* Haggling parameters */
	//min_per = ot_ptr->haggle_per;
	min_per = 5;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	//last_offer = last_offer * ot_ptr->max_inflate / 100L;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;



	//v1.1.94 ���̂ւ�̂����폜�������̊m�F�ɂ���
	(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
	put_str(out_val, 1, 0);
	*price = final_ask;
	cancel = !get_check_strict("���p���܂����H", CHECK_DEFAULT_Y);
#if 0
	/* Haggle */
	for (flag = FALSE; !flag; )
	{
		while (1)
		{
			loop_flag = TRUE;

			(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer("�񎦂��鉿�i? ",
#else
			cancel = receive_offer("What price do you ask? ",
#endif

								   &offer, last_offer, -1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer < cur_ask)
			{
				say_comment_6();
				/* rejected, reset offer for incremental haggling */
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}

			/* Stop */
			if (flag || !loop_flag) break;
		}

		if (!flag)
		{
			x1 = 100 * (last_offer - offer) / (last_offer - cur_ask);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((offer - cur_ask) * x2 / 100L) + 1;
			/* don't let the price go down */
			if (x3 < 0) x3 = 0;
			cur_ask += x3;

			if (cur_ask > final_ask)
			{
				cur_ask = final_ask;
				final = TRUE;
#ifdef JP
				pmt = "�ŏI�񎦋��z";
#else
				pmt = "Final Offer";
#endif

				annoyed++;
				if (annoyed > 3)
				{
					flag = TRUE;
#ifdef JP
				/* �ǉ� $0 �Ŕ�������Ă��܂��̂�h�~ By FIRST*/
					cancel = TRUE;
#endif
					(void)(increase_insults());
				}
			}
			else if (offer <= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				(void)sprintf(out_val,
#ifdef JP
					      "�O��̒񎦉��i $%ld", (long)last_offer);
#else
							  "Your last bid %ld", (long)last_offer);
#endif

				put_str(out_val, 1, 39);
				say_comment_3(cur_ask, annoyed);
			}
		}
	}
#endif
	/* Cancel */
	if (cancel) return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask, o_ptr->number);

	/* Do not cancel */
	return (FALSE);
}

/*
 * Buy an item from a store 			-RAK-
 */
/*:::�X�ŃA�C�e���𔃂�����*/
static void store_purchase(void)
{
	int i, amt, choice;
	int item, item_new;

	s32b price, best;

	object_type forge;
	object_type *j_ptr;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char out_val[160];

	if (cur_store_num == STORE_MUSEUM)
	{
#ifdef JP
		///msg131210
		msg_print("�G���g�����X������o�����Ƃ͂ł��܂���B");
		///msg_print("�����ق�����o�����Ƃ͂ł��܂���B");

#else
		msg_print("Museum.");
#endif
		return;
	}

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == STORE_HOME)
#ifdef JP
			msg_print("�䂪�Ƃɂ͉����u���Ă���܂���B");
#else
			msg_print("Your home is empty.");
#endif

		else
#ifdef JP
			msg_print("���ݏ��i�̍݌ɂ�؂炵�Ă��܂��B");
#else
			msg_print("I am currently out of stock.");
#endif

		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > store_bottom) i = store_bottom;

	/* Prompt */
#ifdef JP
	/* �u���b�N�}�[�P�b�g�̎��͕ʂ̃��b�Z�[�W */
	///sys msg �X�̃Z���t

	switch( cur_store_num ) {
		case STORE_HOME:
			if(hack_flag_access_home_orin)
			{
				if(p_ptr->pclass == CLASS_SATORI || p_ptr->pclass == CLASS_KOISHI)
					sprintf(out_val, "�u�ǂ���Ƃ������ė��܂��傤���H �v");
				else
					sprintf(out_val, "�u��������ė���΂����񂾂��H �v");
			}
			else
				sprintf(out_val, "�ǂ̃A�C�e�������܂���? ");
			break;
		case 6:
			sprintf(out_val, "�ǂ�? ");
			break;
		default:
			//sprintf(out_val, "�ǂ̕i�����~�����񂾂�? ");
			sprintf(out_val, "�������T���ł���? ");

			break;
	}
#else
	if (cur_store_num == STORE_HOME)
	{
		sprintf(out_val, "Which item do you want to take? ");
	}
	else
	{
		sprintf(out_val, "Which item are you interested in? ");
	}
#endif


	/* Get the item number to be bought */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	//v1.1.62 ���@�̖������
	if (hack_flag_access_home_only_arrow)
	{
		if (o_ptr->tval != TV_ARROW && o_ptr->tval != TV_BOLT)
		{
			msg_print("���͖���������Ƃ��ł��Ȃ��B");
			return;
		}
	}

	/*:::���ӂ̔z���T�[�r�X�̂Ƃ������ŏ�����������B���݂̃U�b�N�̃X�y�[�X���킸�Ƃ��������C���x���g���Ɋi�[*/
	if(hack_flag_access_home_orin)
	{

		if(inven_special[INVEN_SPECIAL_SLOT_ORIN_DELIVER].k_idx)
		{
			msg_print("ERROR:���Ӕz���p����C���x���g���Ɋ��ɃA�C�e���������Ă���");
			return;
		}
		//������
		if(o_ptr->number > 1) amt = get_quantity(NULL, o_ptr->number);
		else amt = 1;
		if(amt <= 0) return;

		//���̗̈�փR�s�[
		j_ptr = &forge;
		object_copy(j_ptr, o_ptr);
		reduce_charges(j_ptr, o_ptr->number - amt); //���̊m�F�Łu���񕪁v�ƕ\�����邽�߂̈ꎞ�I�ȏ���
		j_ptr->number = amt;

		//�m�F
		object_desc(o_name, j_ptr, OD_NAME_AND_ENCHANT);
		if(!get_check_strict(format("%s�������Ă��Ă��炢�܂����H",o_name),CHECK_OKAY_CANCEL)) return;

		//�Ƃ̃A�C�e���̌��������A����C���x���g���ւ̊i�[
		distribute_charges(o_ptr, j_ptr, amt);
		store_item_increase(item, -amt);
		store_item_optimize(item);
		combine_and_reorder_home(STORE_HOME);

		object_copy(&inven_special[INVEN_SPECIAL_SLOT_ORIN_DELIVER],j_ptr);

		// -Hack- ����_�ЎG�݉��́u�Ō�ɖK�ꂽ�^�[���v�̒l������J�E���g�Ɏg��
		town[TOWN_HAKUREI].store[STORE_GENERAL].last_visit = turn;

		return;
	}



	/* Assume the player wants just one of them */
	amt = 1;

	/* Get local object */
	j_ptr = &forge;

	/* Get a copy of the object */
	object_copy(j_ptr, o_ptr);

	/*
	 * If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(j_ptr, o_ptr->number - amt);

	/* Modify quantity */
	j_ptr->number = amt;


	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
#ifdef JP
msg_print("����ȂɃA�C�e�������ĂȂ��B");
#else
		msg_print("You cannot carry that many different items.");
#endif

		return;
	}

	///bldg �I�[�i�[��min_inflate�l���Œ�
	/* Determine the "best" price (per item) */
	//best = price_item(j_ptr, ot_ptr->min_inflate, FALSE);
	best = price_item(j_ptr, min_inflate(), FALSE);

	/* Find out how many the player wants */
	if (o_ptr->number > 1)
	{
		/* Hack -- note cost of "fixed" items */
		if ((cur_store_num != STORE_HOME) &&
		    (o_ptr->ident & IDENT_FIXED))
		{
#ifdef JP
msg_format("��ɂ� $%ld�ł��B", (long)(best));
#else
			msg_format("That costs %ld gold per item.", (long)(best));
#endif

		}

		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Get local object */
	j_ptr = &forge;

	/* Get desired object */
	object_copy(j_ptr, o_ptr);

	/*
	 * If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(j_ptr, o_ptr->number - amt);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
#ifdef JP
		msg_print("�U�b�N�ɂ��̃A�C�e�������錄�Ԃ��Ȃ��B");
#else
		msg_print("You cannot carry that many items.");
#endif

		return;
	}

	/* Attempt to buy it */
	if (cur_store_num != STORE_HOME)
	{
		/* Fixed price, quick buy */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Assume accept */
			choice = 0;

			/* Go directly to the "best" deal */
			price = (best * j_ptr->number);
		}

		/* Haggle for it */
		else
		{
			/* Describe the object (fully) */
			object_desc(o_name, j_ptr, 0);

			/* Message */
#ifdef JP
msg_format("%s(%c)���w������B", o_name, I2A(item));
#else
			msg_format("Buying %s (%c).", o_name, I2A(item));
#endif

			msg_print(NULL);

			/* Haggle for a final price */
			/*:::choice�͌������̂Ƃ���FALSE������*/
			choice = purchase_haggle(j_ptr, &price);

			/* Hack -- Got kicked out */
			/*:::���܂�X���{�点��ƓX���܂��ďR��o�����*/
			if (st_ptr->store_open >= turn) return;
		}

		/* Player wants it */
		if (choice == 0)
		{
			/* Fix the item price (if "correctly" haggled) */
			//v1.1.94 ���i���֌W�̃t���O�폜
			//if (price == (best * j_ptr->number)) o_ptr->ident |= (IDENT_FIXED);


			//v1.1.95 �Ȃ񂩋������������̂ŏ���ݒ�
			if (CHECK_JYOON_BUY_ON_CREDIT && (p_ptr->magic_num1[4] + price) >= 100000000)
			{
				msg_print("�X��ɒǂ��o���ꂽ�B�c�P�Ŕ����������悤���B");
				return;
			}

			/* Player can afford it */
			//v1.1.92 �������ꐫ�i�̓c�P�����Ŕ�����
			if (p_ptr->au >= price || CHECK_JYOON_BUY_ON_CREDIT)
			{
				/* Say "okay" */
				say_comment_1();
				///del131210
				/*
				if (cur_store_num == STORE_BLACK) // The black market is illegal! 
					chg_virtue(V_JUSTICE, -1);
				if((o_ptr->tval == TV_BOTTLE) && (cur_store_num != STORE_HOME))
					chg_virtue(V_NATURE, -1);
				*/
				/* Make a sound */
				sound(SOUND_BUY);

				/* Be happy */
				decrease_insults();

				/* Spend the money */
				if (CHECK_JYOON_BUY_ON_CREDIT)
				{
					p_ptr->magic_num1[4] += price;

					//v1.1.95 �Ȃ񂩋������������̂ŏ���ݒ�
					if (p_ptr->magic_num1[4] >= 100000000) p_ptr->magic_num1[4] = 99999999;
				}
				else
				{
					p_ptr->au -= price;
				}

				//��ǐ_�l��BM�ȊO�ŕ��𔃂������A���i10000�ȏ�Ŏ��������������̂łȂ��Ȃ疼�����ő�5�A�b�v
				if(p_ptr->prace == RACE_STRAYGOD && cur_store_num != 6 && (price / j_ptr->number) >= 10000)
				{
					if(!(j_ptr->marked & OM_SPECIAL_FLAG)) set_deity_bias(DBIAS_REPUTATION, MIN(5,((price / j_ptr->number) / 10000)));
				}
				//BM�ŕ��𔃂��ƒ����l�▼���l�������邱�Ƃ�����
				else if(p_ptr->prace == RACE_STRAYGOD && cur_store_num == 6)
				{
					if(one_in_(2)) set_deity_bias(DBIAS_COSMOS, -1);
					if(one_in_(10)) set_deity_bias(DBIAS_REPUTATION, -1);
				}

				/* Update the display */
				store_prt_gold();

				/* Hack -- buying an item makes you aware of it */
				object_aware(j_ptr);

				/* Hack -- clear the "fixed" flag from the item */
				j_ptr->ident &= ~(IDENT_FIXED);

				/* Describe the transaction */
				object_desc(o_name, j_ptr, 0);

				/* Message */

				if (CHECK_JYOON_BUY_ON_CREDIT)
				{
					msg_format("%s���c�P�ōw�����܂����B", o_name);

				}
				else
				{
					msg_format("%s�� $%ld�ōw�����܂����B", o_name, (long)price);

				}


				strcpy(record_o_name, o_name);
				record_turn = turn;

				if (record_buy) do_cmd_write_nikki(NIKKI_BUY, 0, o_name);
				object_desc(o_name, o_ptr, OD_NAME_ONLY);
				if(record_rand_art && o_ptr->art_name)
					do_cmd_write_nikki(NIKKI_ART, 0, o_name);

				/* Erase the inscription */
				j_ptr->inscription = 0;

				/* Erase the "feeling" */
				j_ptr->feeling = FEEL_NONE;
				j_ptr->ident &= ~(IDENT_STORE);
				/* Give it to the player */
				item_new = inven_carry(j_ptr);

				/* Describe the final result */
				object_desc(o_name, &inventory[item_new], 0);

				/* Message */
#ifdef JP
		msg_format("%s(%c)����ɓ��ꂽ�B", o_name, index_to_label(item_new));
#else
				msg_format("You have %s (%c).",
						   o_name, index_to_label(item_new));
#endif

				/* Auto-inscription */
				/*:::�����E���ɂ����쐬�@�j��͂��Ȃ�*/
				autopick_alter_item(item_new, FALSE);

				/* Now, reduce the original stack's pval. */
				if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
				{
					o_ptr->pval -= j_ptr->pval;
				}

				/* Handle stuff */
				handle_stuff();

				/* Note how many slots the store used to have */
				i = st_ptr->stock_num;

				/* Remove the bought items from the store */
				store_item_increase(item, -amt);
				store_item_optimize(item);

				/* Store is empty */
				if (st_ptr->stock_num == 0)
				{
///del131210 �݌ɂ��Ȃ��Ȃ��Ă��X��͈��ނ��Ȃ�
#if 0
					/* Shuffle */
					if (one_in_(STORE_SHUFFLE))
					{
						char buf[80];
						/* Message */
#ifdef JP
						msg_print("�X��͈��ނ����B");
#else
						msg_print("The shopkeeper retires.");
#endif


						/* Shuffle the store */
						store_shuffle(cur_store_num);

						prt("",3,0);
						sprintf(buf, "%s (%s)",
							ot_ptr->owner_name, race_info[ot_ptr->owner_race].title);
						put_str(buf, 3, 10);
						sprintf(buf, "%s (%ld)",
							(f_name + f_info[cur_store_feat].name), (long)(ot_ptr->max_cost));
						prt(buf, 3, 50);
					}

					/* Maintain */
					else
#endif
					{
						/* Message */
#ifdef JP
						msg_print("�X��͐V���ȍ݌ɂ����o�����B");
#else
						msg_print("The shopkeeper brings out some new stock.");
#endif

					}

					/* New inventory */
					for (i = 0; i < 10; i++)
					{
						/* Maintain the store */
						store_maint(p_ptr->town_num, cur_store_num);
					}

					/* Start over */
					store_top = 0;

					/* Redraw everything */
					display_inventory();
				}

				/* The item is gone */
				else if (st_ptr->stock_num != i)
				{
					/* Pick the correct screen */
					if (store_top >= st_ptr->stock_num) store_top -= store_bottom;

					/* Redraw everything */
					display_inventory();
				}

				/* Item is still here */
				else
				{
					/* Redraw the item */
					display_entry(item);
				}
			}

			/* Player cannot afford it */
			else
			{
				/* Simple message (no insult) */
#ifdef JP
				///msg �X�ŋ�������Ȃ��Ƃ��̃��b�Z�[�W
				msg_print("����������܂���B");
#else
				msg_print("You do not have enough gold.");
#endif

			}
		}
	}

	/* Home is much easier */
	else
	{
		bool combined_or_reordered;

		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, j_ptr, amt);

		/* Give it to the player */
		item_new = inven_carry(j_ptr);

		/* Describe just the result */
		object_desc(o_name, &inventory[item_new], 0);

		/* Message */
#ifdef JP
				msg_format("%s(%c)��������B",
#else
		msg_format("You have %s (%c).",
#endif
 o_name, index_to_label(item_new));

		/* Handle stuff */
		handle_stuff();

		/* Take note if we take the last one */
		i = st_ptr->stock_num;

		/* Remove the items from the home */
		store_item_increase(item, -amt);
		store_item_optimize(item);

		combined_or_reordered = combine_and_reorder_home(STORE_HOME);

		/* Hack -- Item is still here */
		if (i == st_ptr->stock_num)
		{
			/* Redraw everything */
			if (combined_or_reordered) display_inventory();

			/* Redraw the item */
			else display_entry(item);
		}

		/* The item is gone */
		else
		{
			/* Nothing left */
			if (st_ptr->stock_num == 0) store_top = 0;

			/* Nothing left on that screen */
			else if (store_top >= st_ptr->stock_num) store_top -= store_bottom;

			/* Redraw everything */
			display_inventory();

			chg_virtue(V_SACRIFICE, 1);
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an item to the store (or home)
 */
static void store_sell(void)
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


	/* Prepare a prompt */
	if (cur_store_num == STORE_HOME)
#ifdef JP
	q = "�ǂ̃A�C�e����u���܂���? ";
#else
		q = "Drop which item? ";
#endif

	else if (cur_store_num == STORE_MUSEUM)
#ifdef JP
	///msg131210
	q = "�ǂ̃A�C�e��������܂���? ";
	//q = "�ǂ̃A�C�e�����񑡂��܂���? ";
#else
		q = "Give which item? ";
#endif

	else
#ifdef JP
		q = "�ǂ̃A�C�e���𔄂�܂���? ";
#else
		q = "Sell which item? ";
#endif


	item_tester_no_ryoute = TRUE;
	/* Only allow items the store will buy */
	item_tester_hook = store_will_buy;

	/* Get an item */
	/* �䂪�Ƃł������ȃ��b�Z�[�W���o��I���W�i���̃o�O���C�� */
	if (cur_store_num == STORE_HOME)
	{
#ifdef JP
		s = "�u����A�C�e���������Ă��܂���B";
#else
		s = "You don't have any item to drop.";
#endif
	}
	else if (cur_store_num == STORE_MUSEUM)
	{
#ifdef JP
		///msg131210
		s = "�����A�C�e���������Ă��܂���B";
		//s = "�񑡂ł���A�C�e���������Ă��܂���B";

#else
		s = "You don't have any item to give.";
#endif
	}
	else
	{
#ifdef JP
		///sys store ������肽�����̂��Ȃ����̓X�̃Z���t
		s = "�~���������Ȃ��ł��˂��B";
#else
		s = "You have nothing that I want.";
#endif
	}
	/*:::�C���x���g�����甄�肽���A�C�e����I�����Ă�炵�� USE_FLOOR�͕s�v�ȋC�����邪*/
	//v1.1.40 �����ւ����@�ő����A�C�e����u�����Ƃ��ăo�O�����̂�USE_FLOOR���폜
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return;

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


	///mod140122 ��������
	if(!(wield_check(item,INVEN_PACK))) return;

	//v1.1.62 ���@�̖������
	if (hack_flag_access_home_only_arrow)
	{
		if (o_ptr->tval != TV_ARROW && o_ptr->tval != TV_BOLT)
		{
			msg_print("���͖���������Ƃ��ł��Ȃ��B");
			return;
		}
	}

	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_RARM) && object_is_cursed(o_ptr))
	{
		/* Oops */
#ifdef JP
		msg_print("�Ӂ[�ށA�ǂ���炻��͎���Ă���悤���ˁB");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif


		/* Nope */
		return;
	}
	///sys item store ��������E�A�푰���������̃A�C�e����X�Ŕ���Ȃ��悤�Ɉ�H�v�v

	/* Assume one item */
	amt = 1;

	/* Find out how many the player wants (letter means "all") */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Get local object */
	q_ptr = &forge;

	/* Get a copy of the object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = amt;

	/*
	 * Hack -- If a rod or wand, allocate total maximum
	 * timeouts or charges to those being sold. -LM-
	 */
	if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
	{
		q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Get a full description */
	object_desc(o_name, q_ptr, 0);

	/* Remove any inscription, feeling for stores */
	if ((cur_store_num != STORE_HOME) && (cur_store_num != STORE_MUSEUM))
	{
		q_ptr->inscription = 0;
		q_ptr->feeling = FEEL_NONE;
	}

	/* Is there room in the store (or the home?) */
	if (!store_check_num(q_ptr))
	{
		if (cur_store_num == STORE_HOME)
#ifdef JP
			msg_print("�䂪�Ƃɂ͂����u���ꏊ���Ȃ��B");
#else
			msg_print("Your home is full.");
#endif

		else if (cur_store_num == STORE_MUSEUM)
#ifdef JP
			///msg131211
			msg_print("�G���g�����X�͂������t���B");
			///msg_print("�����ق͂������t���B");

#else
			msg_print("Museum is full.");
#endif

		else
#ifdef JP
			msg_print("�����܂��񂪁A�X�ɂ͂����u���ꏊ������܂���B");
#else
			msg_print("I have not the room in my store to keep it.");
#endif

		return;
	}


	/* Real store */
	if ((cur_store_num != STORE_HOME) && (cur_store_num != STORE_MUSEUM))
	{
		/* Describe the transaction */
#ifdef JP
		msg_format("%s(%c)�𔄋p����B", o_name, index_to_label(item));
#else
		msg_format("Selling %s (%c).", o_name, index_to_label(item));
#endif

		msg_print(NULL);

		/* Haggle for it */
		choice = sell_haggle(q_ptr, &price);

		/* Kicked out */
		if (st_ptr->store_open >= turn) return;

		/* Sold... */
		if (choice == 0)
		{
			/* Say "okay" */
			say_comment_1();

			/* Make a sound */
			sound(SOUND_SELL);

			///del131211
			/*
			if (cur_store_num == STORE_BLACK) //* The black market is illegal! *
				chg_virtue(V_JUSTICE, -1);

			if((o_ptr->tval == TV_BOTTLE) && (cur_store_num != STORE_HOME))
				chg_virtue(V_NATURE, 1);
			*/
			/* Be happy */
			decrease_insults();

			/* Get some money */
			p_ptr->au += price;

			/* Update the display */
			store_prt_gold();

			/* Get the "apparent" value */
			dummy = object_value(q_ptr) * q_ptr->number;

			/* Identify it */
			identify_item(o_ptr);

			/* Get local object */
			q_ptr = &forge;

			/* Get a copy of the object */
			object_copy(q_ptr, o_ptr);

			/* Modify quantity */
			q_ptr->number = amt;

			/* Make it look like to be known */
			q_ptr->ident |= IDENT_STORE;

			/*
			 * Hack -- If a rod or wand, let the shopkeeper know just
			 * how many charges he really paid for. -LM-
			 */
			if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
			{
				q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			}

			/* Get the "actual" value */
			value = object_value(q_ptr) * q_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, q_ptr, 0);

			/* Describe the result (in message buffer) */
#ifdef JP
msg_format("%s�� $%ld�Ŕ��p���܂����B", o_name, (long)price);
#else
			msg_format("You sold %s for %ld gold.", o_name, (long)price);
#endif

			if (record_sell) do_cmd_write_nikki(NIKKI_SELL, 0, o_name);

			if (!((o_ptr->tval == TV_FIGURINE) && (value > 0)))
			{
			 /* Analyze the prices (and comment verbally) unless a figurine*/
				/*:::���Ӓ�i�𔄂������̓X��̃Z���t*/
			purchase_analyze(price, value, dummy);
			}

			/*
			 * Hack -- Allocate charges between those wands or rods sold
			 * and retained, unless all are being sold. -LM-
			 */
			distribute_charges(o_ptr, q_ptr, amt);

			/* Reset timeouts of the sold items */
			q_ptr->timeout = 0;

			/* Take the item from the player, describe the result */
			inven_item_increase(item, -amt);
			inven_item_describe(item);

			/* If items remain, auto-inscribe before optimizing */
			if (o_ptr->number > 0)
				autopick_alter_item(item, FALSE);

			inven_item_optimize(item);

			/* Handle stuff */
			handle_stuff();

			//��ǐ_�l
			if(p_ptr->prace == RACE_STRAYGOD) q_ptr->marked |= OM_SPECIAL_FLAG;

			/* The store gets that (known) item */
			item_pos = store_carry(q_ptr);

			/* Re-display if item is now in store */
			if (item_pos >= 0)
			{
				store_top = (item_pos / store_bottom) * store_bottom;
				display_inventory();
			}
		}
	}

	/* Player is at museum */
	else if (cur_store_num == STORE_MUSEUM)
	{
		char o2_name[MAX_NLEN];
		object_desc(o2_name, q_ptr, OD_NAME_ONLY);

		if (-1 == store_check_num(q_ptr))
		{
#ifdef JP
			///msg131211
			msg_print("����Ɠ����i���͊��ɏ����Ă���B");
			///msg_print("����Ɠ����i���͊��ɔ����قɂ���悤�ł��B");
#else
			msg_print("The same object as it is already in the Museum.");
#endif
		}
		else
		{
#ifdef JP
			///msg131211
			msg_print("��x�G���g�����X�ɏ��������͉̂���ł��Ȃ��B");
			///msg_print("�����قɊ񑡂������͎̂��o�����Ƃ��ł��܂���I�I");
#else
			msg_print("You cannot take items which is given to the Museum back!!");
#endif
		}
#ifdef JP
		///msg131211
		if (!get_check(format("�{����%s���G���g�����X�ɏ���܂����H", o2_name))) return;
		///if (!get_check(format("�{����%s���񑡂��܂����H", o2_name))) return;
#else
		if (!get_check(format("Really give %s to the Museum? ", o2_name))) return;
#endif

		/* Identify it */
		identify_item(q_ptr);
		q_ptr->ident |= IDENT_MENTAL;

		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, q_ptr, amt);

		/* Describe */
#ifdef JP
		msg_format("%s��u�����B(%c)", o_name, index_to_label(item));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif
		//�����ق̃A�C�e�����X�R�A�v�Z�ɉ�����t���O
		hack_flag_calc_museum_score = TRUE;
		choice = 0;

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			store_top = (item_pos / store_bottom) * store_bottom;
			display_inventory();
		}
	}
	/* Player is at home */
	else
	{
		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, q_ptr, amt);

		/* Describe */
#ifdef JP
		msg_format("%s��u�����B(%c)", o_name, index_to_label(item));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif

		choice = 0;

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			store_top = (item_pos / store_bottom) * store_bottom;
			display_inventory();
		}
	}

	if ((choice == 0) && (item >= INVEN_RARM))
	{
		calc_android_exp();
		kamaenaoshi(item);
	}
}


/*
 * Examine an item in a store			   -JDL-
 */
static void store_examine(void)
{
	int         i;
	int         item;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	char        out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == STORE_HOME)
#ifdef JP
			msg_print("�䂪�Ƃɂ͉����u���Ă���܂���B");
#else
			msg_print("Your home is empty.");
#endif

		else if (cur_store_num == STORE_MUSEUM)
#ifdef JP
			///msg131211
			msg_print("�G���g�����X�ɂ͉��������Ă��܂���B");
			//msg_print("�����قɂ͉����u���Ă���܂���B");

#else
			msg_print("Museum is empty.");
#endif

		else
#ifdef JP
			msg_print("���ݏ��i�̍݌ɂ�؂炵�Ă��܂��B");
#else
			msg_print("I am currently out of stock.");
#endif

		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > store_bottom) i = store_bottom;

	/* Prompt */
#ifdef JP
sprintf(out_val, "�ǂ�𒲂ׂ܂����H");
#else
	sprintf(out_val, "Which item do you want to examine? ");
#endif


	/* Get the item number to be examined */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	//v1.1.62 ���@�̖������
	if (hack_flag_access_home_only_arrow)
	{
		if (o_ptr->tval != TV_ARROW && o_ptr->tval != TV_BOLT)
		{
			msg_print("���͖���������Ƃ��ł��Ȃ��B");
			return;
		}
	}

	/* Require full knowledge */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
		/* This can only happen in the home */
#ifdef JP
msg_print("���̃A�C�e���ɂ��ē��ɒm���Ă��邱�Ƃ͂Ȃ��B");
#else
		msg_print("You have no special knowledge about that item.");
#endif

		return;
	}

	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
#ifdef JP
msg_format("%s�𒲂ׂĂ���...", o_name);
#else
	msg_format("Examining %s...", o_name);
#endif


	/* Describe it fully */
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL))
#ifdef JP
msg_print("���ɕς�����Ƃ���͂Ȃ��悤���B");
#else
		msg_print("You see nothing special.");
#endif


	return;
}


/*
 * Remove an item from museum (Originally from TOband)
 */
static void museum_remove_object(void)
{
	int         i;
	int         item;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	char        out_val[160];

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
#ifdef JP
		msg_print("�G���g�����X�ɂ͉��������Ă��܂���B");
		//msg_print("�����قɂ͉����u���Ă���܂���B");
#else
		msg_print("Museum is empty.");
#endif

		return;
	}

	/* Find the number of objects on this and following pages */
	i = st_ptr->stock_num - store_top;

	/* And then restrict it to the current page */
	if (i > store_bottom) i = store_bottom;

	/* Prompt */


#ifdef JP
	sprintf(out_val, "�ǂ̃A�C�e����p�����܂����H");
#else
	sprintf(out_val, "Which item do you want to order to remove? ");
#endif

	/* Get the item number to be removed */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Description */
	object_desc(o_name, o_ptr, 0);

#ifdef JP
	msg_print("�p�������A�C�e���͓�x�ƌ��邱�Ƃ͂ł��܂���I");
	if (!get_check(format("�{����%s��p�����܂����H", o_name))) return;
#else
	msg_print("You cannot see items which is removed from the Museum!");
	if (!get_check(format("Really order to remove %s from the Museum? ", o_name))) return;
#endif

	/* Message */
#ifdef JP
	msg_format("%s�����������B", o_name);
#else
	msg_format("You ordered to remove %s.", o_name);
#endif

	//�����ق̃A�C�e�����X�R�A�v�Z�ɉ�����t���O
	hack_flag_calc_museum_score = TRUE;

	/* Remove the items from the home */
	store_item_increase(item, -o_ptr->number);
	store_item_optimize(item);

	(void)combine_and_reorder_home(STORE_MUSEUM);

	/* The item is gone */

	/* Nothing left */
	if (st_ptr->stock_num == 0) store_top = 0;

	/* Nothing left on that screen */
	else if (store_top >= st_ptr->stock_num) store_top -= store_bottom;

	/* Redraw everything */
	display_inventory();

	return;
}


/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands
 * in the stores which are not allowed in the dungeon, and we
 * must disable some commands which are allowed in the dungeon
 * but not in the stores, to prevent chaos.
 */
/*:::�X��p�̃R�}���h���߃��[�`��*/
static void store_process_command(void)
{
#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	if (rogue_like_commands && command_cmd == 'l')
	{
		command_cmd = 'x';	/* hack! */
	}

	/* Parse the command */
	switch (command_cmd)
	{
		/* Leave */
		case ESCAPE:
		{
			if(hack_flag_access_home && !get_check_strict("���ւ����I���܂����H", CHECK_OKAY_CANCEL)) break;

			if (hack_flag_access_home_only_arrow && !get_check_strict("��̎g�p���I���܂����H", CHECK_OKAY_CANCEL)) break;

			leave_store = TRUE;
			break;
		}

		/* ���{��Œǉ� */
		/* 1 �y�[�W�߂�R�}���h: �䂪�Ƃ̃y�[�W���������̂ŏd�󂷂�͂� By BUG */
		case '-':
		{
			if (st_ptr->stock_num <= store_bottom) {
#ifdef JP
				msg_print("����őS���ł��B");
#else
				msg_print("Entire inventory is shown.");
#endif
			}
			else{
				store_top -= store_bottom;
				if ( store_top < 0 )
					store_top = ((st_ptr->stock_num - 1 )/store_bottom) * store_bottom;
				if ( (cur_store_num == STORE_HOME) && (powerup_home == FALSE) )
					if ( store_top >= store_bottom ) store_top = store_bottom;
				display_inventory();
			}
			break;
		}

		/* Browse */
		case ' ':
		{
			if (st_ptr->stock_num <= store_bottom)
			{
#ifdef JP
				msg_print("����őS���ł��B");
#else
				msg_print("Entire inventory is shown.");
#endif

			}
			else
			{
				store_top += store_bottom;
				/*
				 * �B���I�v�V����(powerup_home)���Z�b�g����Ă��Ȃ��Ƃ���
				 * �䂪�Ƃł� 2 �y�[�W�܂ł����\�����Ȃ�
				 */
				if ((cur_store_num == STORE_HOME) && 
				    (powerup_home == FALSE) && 
					(st_ptr->stock_num >= STORE_INVEN_MAX))
				{
					if (store_top >= (STORE_INVEN_MAX - 1))
					{
						store_top = 0;
					}
				}
				else
				{
					if (store_top >= st_ptr->stock_num) store_top = 0;
				}

				display_inventory();
			}
			break;
		}

		/* Redraw */
		case KTRL('R'):
		{
			do_cmd_redraw();
			display_store();
			break;
		}

		/* Get (purchase) */
		case 'g':
		{
			store_purchase();
			break;
		}

		/* Drop (Sell) */
		case 'd':
		{
			if(hack_flag_access_home_orin) break;
			store_sell();
			break;
		}

		/* Examine */
		case 'x':
		{
			hack_flag_store_examine = TRUE;
			store_examine();
			hack_flag_store_examine = FALSE;
			break;
		}

		/* Ignore return */
		case '\r':
		{
			break;
		}

		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			if(p_ptr->pclass == CLASS_3_FAIRIES)
				do_cmd_wield_3_fairies();
			else
				do_cmd_wield(FALSE);

			break;
		}

		/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			if(hack_flag_access_home_orin) break;
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}



		/*** Use various objects ***/

		/* Browse a book */
		///del131222 �X����b�R�}���h�͎g���Ȃ�����
		///class �X��b�R�}���h���g���Ƃ�
		case 'b':
		{
				msg_print("���̃R�}���h�͓X�̒��ł͎g���Ȃ��Ȃ�܂����B");

#if 0

			if ( (p_ptr->pclass == CLASS_MINDCRAFTER) ||
			     (p_ptr->pclass == CLASS_BERSERKER) ||
			     (p_ptr->pclass == CLASS_NINJA) ||
			     (p_ptr->pclass == CLASS_MIRROR_MASTER) 
			     ) do_cmd_mind_browse();
			else if (p_ptr->pclass == CLASS_SMITH)
				do_cmd_kaji(TRUE);
			else if (p_ptr->pclass == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(TRUE, FALSE);
			else if (p_ptr->pclass == CLASS_SNIPER)
				do_cmd_snipe_browse();
			else do_cmd_browse();
#endif


			break;
		}
		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_change_name();
			p_ptr->town_num = inner_town_num;
			display_store();
			break;
		}


		/*** System Commands ***/
		case '!':
		case '"':
		case '@':
		case '%':
		case '&':
		case '=':
			{
			msg_print("���̃R�}���h�͓X�̒��ł͎g���Ȃ��Ȃ�܂����B");
			break;
			}

#if 0
		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_pref();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with macros */
		case '@':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_macros();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_visuals();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with colors */
		case '&':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_colors();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			(void)combine_and_reorder_home(STORE_HOME);
			do_cmd_redraw();
			display_store();
			break;
		}
#endif

		/*** Misc Commands ***/
		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages(0);
			break;
		}

		case '|':
		{
			do_cmd_nikki();
			break;
		}

		/* Check artifacts, uniques etc. */
		case '~':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Hack -- Unknown command */
		default:
		{
			if ((cur_store_num == STORE_MUSEUM) && (command_cmd == 'r'))
			{
				museum_remove_object();
			}
			else
			{
#ifdef JP
				msg_print("���̃R�}���h�͓X�̒��ł͎g���܂���B");
#else
				msg_print("That command does not work in stores.");
#endif
			}

			break;
		}
	}
}


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
/*:::�X�ɓ��蔃��������Brequest_command()�œ��͑҂����邪����̃R�}���h�͓X�p�̕ʂ̃R�}���h�ɕϊ������*/
/*:::store_process_command()�œX�p�R�}���h���s�@�X����o����ĕ`�揈��*/
void do_cmd_store(void)
{
	int         which;
	int         maintain_num;
	int         i;
	cave_type   *c_ptr;
	bool        need_redraw_store_inv; /* To redraw missiles damage and prices in store */
	int w, h;

	/* Get term size */
	Term_get_size(&w, &h);

	/* Calculate stocks per 1 page */
	/*:::�X�Ɉ�x�ɂ���������ׂ��邩�v�Z���Ă���炵��*/
	xtra_stock = MIN(14+26, ((h > 24) ? (h - 24) : 0));
	store_bottom = MIN_STOCK + xtra_stock;

	/* Access the player grid */
	c_ptr = &cave[py][px];
	/*:::���̌��݈ʒu�̒n�`��FF_STORE���Ȃ���ΏI���@(PARANOIA)*/
	/* Verify a store */
	///mod160303 �����������ւ����@�����ǉ�
	if (!hack_flag_access_home && !hack_flag_access_home_orin && !hack_flag_access_home_only_arrow &&
		!cave_have_flag_grid(c_ptr, FF_STORE))
	{
#ifdef JP
		msg_print("�����ɂ͓X������܂���B");
#else
		msg_print("You see no store here.");
#endif

		return;
	}


	/* Extract the store code */
	/*:::feature_type.subtype �X�̏ꍇSTORE_HOME/STORE_WEAPON�Ȃǂ̒l��defines.h�ɒ�`����Ă���*/
	/*:::�����ł܂��X�̎�ނ����܂�*/
	//���������Z�₨�ӑ�z�ւ͞�▂����������Ȃ�
	if(hack_flag_access_home) which = STORE_HOME;
	else if(hack_flag_access_home_orin) which = STORE_HOME;
	else if (hack_flag_access_home_only_arrow) which = STORE_HOME;
	else
	{
		which = f_info[c_ptr->feat].subtype;
		///mod150110 ��@�G��Ă��鎞�͗�������t�������Ă����
		if(p_ptr->pclass == CLASS_CHEN && p_ptr->magic_num1[0] && which == STORE_HOME)
		{
			msg_print("�傪����A����߂������Ă��ꂽ�B");
			p_ptr->magic_num1[0] = 0;
			p_ptr->update |= PU_BONUS;
		}
		///mod150401 ���ׁ@�A�C�e���[�U�񐔉�
		///mod150423 ������g�����ǉ�
		if((p_ptr->pclass == CLASS_SEIJA || p_ptr->pclass == CLASS_MAGIC_EATER ) && which == STORE_HOME)
		{
			restore_seija_item();
		}
	}

	old_town_num = p_ptr->town_num;
	/*:::�ƂƔ����ق��ǂ��̒�����ł������悤�Ɏg�����߁u�����ǂ̒��ɂ���̂��v���ꎞ�I�ɕӋ��ɃZ�b�g*/
	if ((which == STORE_HOME) || (which == STORE_MUSEUM)) p_ptr->town_num = TOWN_HITOZATO;
	//if (dun_level) p_ptr->town_num = NO_TOWN;
	else if (dun_level) p_ptr->town_num = TOWN_UG;
	inner_town_num = p_ptr->town_num;

	/* Hack -- Check the "locked doors" */
	/*:::���܂�X���{�点��ƈ��^�[����܂œX���܂�*/
	///bldg �����Ԃ����J���Ȃ��X�Ƃ������ɓ����H
	if ((town[p_ptr->town_num].store[which].store_open >= turn) ||
	    (ironman_shops))
	{
#ifdef JP
		msg_print("�h�A�Ɍ����������Ă���B");
#else
		msg_print("The doors are locked.");
#endif

		p_ptr->town_num = old_town_num;
		return;
	}

	//v1.1.40 �������ǉ�
	if (!hack_flag_access_home && mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_GIGANTIC)
	{
		msg_print("�����ɓ���Ȃ��B");
		p_ptr->town_num = old_town_num;
		return ;
	}

	//v1.1.36 ����N�G�X�g�󒍒�
	//v1.1.36b �X�̎�ނ̔����Y��đS�X�x�Ƃ��Ă��̂ŏC��
	if(p_ptr->town_num == TOWN_HITOZATO && which == STORE_BOOK && quest[QUEST_KOSUZU].status == QUEST_STATUS_TAKEN)
	{
		msg_print("��ވ��̔��ɂ́u�Վ��x�Ɓv�̎D���������Ă���...");
		p_ptr->town_num = old_town_num;
		return ;
	}
	//v1.1.37 ����v���C��
	if(p_ptr->town_num == TOWN_HITOZATO && which == STORE_BOOK && p_ptr->pclass == CLASS_KOSUZU)
	{
		msg_print("���̌��z���̐^����͂ނ܂ŉƂɖ߂�C�͂Ȃ��B");
		p_ptr->town_num = old_town_num;
		return ;
	}
	//v1.1.64 
	if (p_ptr->town_num == TOWN_HIGAN && which == STORE_GENERAL)
	{
		msg_print("���ɂ́u�������v�̎D���������Ă���B");
		p_ptr->town_num = old_town_num;
		return;
	}

	if(p_ptr->pclass == CLASS_HINA && p_ptr->magic_num1[0] > (HINA_YAKU_LIMIT2+5000) && which != STORE_HOME && which != STORE_MUSEUM)
	{
		msg_print("�X�̔��͌ł�������Ă���c");
		p_ptr->town_num = old_town_num;
		return ;
	}

	//v1.1.48 �X�[�p�[�n�R�_�̎����͓X�ɓ���Ȃ�
	if (SUPER_SHION && which != STORE_HOME && which != STORE_MUSEUM)
	{
		msg_print("�X�̔��Ɂu�Վ��x�Ɓv�̎D���������Ă���B");
		p_ptr->town_num = old_town_num;
		return;
	}

	if((world_player || SAKUYA_WORLD) && which != STORE_HOME && which != STORE_MUSEUM)
	{
		msg_print("�������Ă��Ӗ����Ȃ��B");
		p_ptr->town_num = old_town_num;
		return ;
	}

	///mod160228 Ex�_���W�������̓X��last_vist����(�K�ړ���������)�ȊO�͍݌ɍX�V����Ȃ����Ƃɂ���
	if(EXTRA_MODE)
	{
		if(town[p_ptr->town_num].store[which].last_visit < 0) 
			maintain_num = 5+randint0(dun_level / 20);
		else
			maintain_num = 0;
	}
	else
	{
		/* Calculate the number of store maintainances since the last visit */
		maintain_num = (turn - town[p_ptr->town_num].store[which].last_visit) / (TURNS_PER_TICK * STORE_TICKS);
	}


	/* Maintain the store max. 10 times */
	if (maintain_num > 10) maintain_num = 10;

	/*:::�Ō�ɓX��K�ꂽ�^�[������Amaintain_num(10000turn���ƂɃJ�E���g�j�̉񐔕��X�̕i�������X�V����*/
	if (maintain_num)
	{
		/* Maintain the store */
		for (i = 0; i < maintain_num; i++)
			store_maint(p_ptr->town_num, which);

		/* Save the visit */
		town[p_ptr->town_num].store[which].last_visit = turn;
	}

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();


	/* Hack -- Character is in "icky" mode */
	character_icky = TRUE;


	/* No command argument */
	command_arg = 0;

	/* No repeated command */
	command_rep = 0;

	/* No automatic command */
	command_new = 0;

	/* Do not expand macros */
	get_com_no_macros = TRUE;

	/* Save the store number */
	cur_store_num = which;

	/* Hack -- save the store feature */
	cur_store_feat = c_ptr->feat;

	/* Save the store and owner pointers */
	st_ptr = &town[p_ptr->town_num].store[cur_store_num];
	//ot_ptr = &owners[cur_store_num][st_ptr->owner];


	/* Start at the beginning */
	store_top = 0;

	//v1.1.59
	if(!EXTRA_MODE || play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_BUILD_EX))
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_BUILD); //v1.1.58

	/* Display the store */

	display_store();

	//test msg_format("store_num:%d",cur_store_num);


	/* Do not leave */
	leave_store = FALSE;

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Clear */
		clear_from(20 + xtra_stock);


		/* Basic commands */
#ifdef JP
		prt(" ESC) ��������o��", 21 + xtra_stock, 0);
#else
		prt(" ESC) Exit from Building.", 21 + xtra_stock, 0);
#endif


		/* Browse if necessary */
		if (st_ptr->stock_num > store_bottom)
		{
#ifdef JP
			prt(" -)�O�y�[�W", 22 + xtra_stock, 0);
			prt(" �X�y�[�X) ���y�[�W", 23 + xtra_stock, 0);
#else
			prt(" -) Previous page", 22 + xtra_stock, 0);
			prt(" SPACE) Next page", 23 + xtra_stock, 0);
#endif

		}

		if(hack_flag_access_home_orin)
		{
			prt("g) �A�C�e����I��", 21 + xtra_stock, 27);
			prt("x) �Ƃ̃A�C�e���𒲂ׂ�", 23 + xtra_stock, 27);
		}

		/* Home commands */
		else if (cur_store_num == STORE_HOME)
		{
#ifdef JP
			prt("g) �A�C�e�������", 21 + xtra_stock, 27);
			prt("d) �A�C�e����u��", 22 + xtra_stock, 27);
			prt("x) �Ƃ̃A�C�e���𒲂ׂ�", 23 + xtra_stock, 27);
#else
			prt("g) Get an item.", 21 + xtra_stock, 27);
			prt("d) Drop an item.", 22 + xtra_stock, 27);
			prt("x) eXamine an item in the home.", 23 + xtra_stock, 27);
#endif
		}

		/* Museum commands */
		else if (cur_store_num == STORE_MUSEUM)
		{
#ifdef JP
			prt("d) �A�C�e����u��", 21 + xtra_stock, 27);
			prt("r) �A�C�e���̓W������߂�", 22 + xtra_stock, 27);
			///msg131210
			prt("x) �G���g�����X�̃A�C�e���𒲂ׂ�", 23 + xtra_stock, 27);
			///prt("x) �����ق̃A�C�e���𒲂ׂ�", 23 + xtra_stock, 27);
#else
			prt("d) Drop an item.", 21 + xtra_stock, 27);
			prt("r) order to Remove an item.", 22 + xtra_stock, 27);
			prt("x) eXamine an item in the museum.", 23 + xtra_stock, 27);
#endif
		}

		/* Shop commands XXX XXX XXX */
		else
		{
#ifdef JP
			prt("p) ���i�𔃂�", 21 + xtra_stock, 30);
			prt("s) �A�C�e���𔄂�", 22 + xtra_stock, 30);
			prt("x) ���i�𒲂ׂ�", 23 + xtra_stock,30);
#else
			prt("p) Purchase an item.", 21 + xtra_stock, 30);
			prt("s) Sell an item.", 22 + xtra_stock, 30);
			prt("x) eXamine an item in the shop", 23 + xtra_stock,30);
#endif
		}

#ifdef JP
		/* ��{�I�ȃR�}���h�̒ǉ��\�� */

		prt("i/e) ������/�����̈ꗗ", 21 + xtra_stock, 56);

		if (rogue_like_commands)
		{
			prt("w/T) ��������/�͂���", 22 + xtra_stock, 56);
		}
		else
		{
			prt("w/t) ��������/�͂���", 22 + xtra_stock, 56);
		}
#else
		prt("i/e) Inventry/Equipment list", 21 + xtra_stock, 56);

		if (rogue_like_commands)
		{
			prt("w/T) Wear/Take off equipment", 22 + xtra_stock, 56);
		}
		else
		{
			prt("w/t) Wear/Take off equipment", 22 + xtra_stock, 56);
		}
#endif
		/* Prompt */
#ifdef JP
		prt("�R�}���h:", 20 + xtra_stock, 0);
#else
		prt("You may: ", 20 + xtra_stock, 0);
#endif


		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		store_process_command();

		/*
		 * Hack -- To redraw missiles damage and prices in store
		 * If player's charisma changes, or if player changes a bow, PU_BONUS is set
		 */
		need_redraw_store_inv = (p_ptr->update & PU_BONUS) ? TRUE : FALSE;

		/* Hack -- Character is still in "icky" mode */
		character_icky = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		///mod160312 ���ӃA�C�e���z���T�[�r�X�̃A�C�e�����I�΂ꂽ��X����o�鏈��
		if(hack_flag_access_home_orin && inven_special[INVEN_SPECIAL_SLOT_ORIN_DELIVER].k_idx)
		{
			leave_store = TRUE;
		}

		/* XXX XXX XXX Pack Overflow */
		/*:::�C���x���g���̂��ӂ�̈�ɃA�C�e������������X����o��@�Ƃŕ����܂��u����Ȃ�Ƃɒu��*/
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &inventory[item];

			/* Hack -- Flee from the store */
			if (cur_store_num != STORE_HOME)
			{
				/* Message */
#ifdef JP
				if (cur_store_num == STORE_MUSEUM)
					msg_print("�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂăG���g�����X����o��...");
					//msg_print("�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂĔ����ق���o��...");
				else
					msg_print("�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂēX����o��...");
#else
				if (cur_store_num == STORE_MUSEUM)
					msg_print("Your pack is so full that you flee the Museum...");
				else
					msg_print("Your pack is so full that you flee the store...");
#endif


				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
#ifdef JP
				msg_print("�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂĉƂ���o��...");
#else
				msg_print("Your pack is so full that you flee your home...");
#endif


				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type forge;
				object_type *q_ptr;

				char o_name[MAX_NLEN];


				/* Give a message */
#ifdef JP
				msg_print("�U�b�N����A�C�e�������ӂ�Ă��܂����I");
#else
				msg_print("Your pack overflows!");
#endif


				/* Get local object */
				q_ptr = &forge;

				/* Grab a copy of the item */
				object_copy(q_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, q_ptr, 0);

				/* Message */
#ifdef JP
				msg_format("%s���������B(%c)", o_name, index_to_label(item));
#else
				msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif


				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(q_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					store_top = (item_pos / store_bottom) * store_bottom;
					display_inventory();
				}
			}
		}

		/* Hack -- Redisplay store prices if charisma changes */
		/* Hack -- Redraw missiles damage if player changes bow */
		if (need_redraw_store_inv) display_inventory();

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn) leave_store = TRUE;
	}
	/*:::���ꂩ��X����o��*/

	/*:::�ƂƔ����ي֌W�̓��ꏈ����߂�*/
	p_ptr->town_num = old_town_num;

	select_floor_music(); //v1.1.58


	/* Free turn XXX XXX XXX */
	energy_use = 100;


	/* Hack -- Character is no longer in "icky" mode */
	character_icky = FALSE;


	/* Hack -- Cancel automatic command */
	command_new = 0;

	/* Hack -- Cancel "see" mode */
	command_see = FALSE;

	/* Allow expanding macros */
	get_com_no_macros = FALSE;

	/* Flush messages XXX XXX XXX */
	msg_print(NULL);


	/* Clear the screen */
	Term_clear();


	/* Update everything */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}



/*
 * Shuffle one of the stores.
 */
/*:::�X�僉���_�����@�ǓƕψفA���K�͂ȊX�Ȃǂ̓X����A����߂ɂ��X�����*/
#if 0
void store_shuffle(int which)
{
	int i, j;


	/* Ignore home */
	if (which == STORE_HOME) return;
	if (which == STORE_MUSEUM) return;


	/* Save the store index */
	cur_store_num = which;

	/* Activate that store */
	st_ptr = &town[p_ptr->town_num].store[cur_store_num];

	j = st_ptr->owner;
	/* Pick a new owner */
	while(1)
	{
		st_ptr->owner = (byte)randint0(MAX_OWNERS);
		if (j == st_ptr->owner) continue;
		for (i = 1;i < max_towns; i++)
		{
			if (i == p_ptr->town_num) continue;
			if (st_ptr->owner == town[i].store[cur_store_num].owner) break;
		}
		if (i == max_towns) break;
	}

	/* Activate the new owner */
	ot_ptr = &owners[cur_store_num][st_ptr->owner];


	/* Reset the owner data */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;


	/* Hack -- discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_type *o_ptr;

		/* Get the item */
		o_ptr = &st_ptr->stock[i];

		if (!object_is_artifact(o_ptr))
		{
			/* Hack -- Sell all non-artifact old items for "half price" */
			o_ptr->discount = 50;

			/* Hack -- Items are no longer "fixed price" */
			o_ptr->ident &= ~(IDENT_FIXED);

			/* Mega-Hack -- Note that the item is "on sale" */
#ifdef JP
			o_ptr->inscription = quark_add("���o��");
#else
			o_ptr->inscription = quark_add("on sale");
#endif
		}
	}
}
#endif

/*
 * Maintain the inventory at the stores.
 */
/*:::�X�̕i�������X�V�B�Ō�ɖK��Ă���10000turn���Ƃ�1��Ă΂��*/
void store_maint(int town_num, int store_num)
{
	int 		j;

	cur_store_num = store_num;

	/* Ignore home */
	if (store_num == STORE_HOME) return;
	if (store_num == STORE_MUSEUM) return;

	/* Activate that store */
	st_ptr = &town[town_num].store[store_num];

	/* Activate the owner */
	/*:::�X�̃I�[�i�[�@������폜���邪��񂵂Ŗ��Ȃ�����*/
	//ot_ptr = &owners[store_num][st_ptr->owner];

	/* Store keeper forgives the player */
	/*:::�X��̕s�@���x�����Z�b�g*/
	st_ptr->insult_cur = 0;

	/*�u���b�N�}�[�P�b�g�ő��̓X�Ŕ����Ă���̂Ɠ������̃A�C�e�������O*/
	/* Mega-Hack -- prune the black market */
	if (store_num == STORE_BLACK)
	{
		/* Destroy crappy black market items */
		for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *o_ptr = &st_ptr->stock[j];

			/* Destroy crappy items */
			if (black_market_crap(o_ptr))
			{
				/* Destroy the item */
				store_item_increase(j, 0 - o_ptr->number);
				store_item_optimize(j);
			}
		}
	}

	/*:::�X�ɃA�C�e���������c�������߂Ă���*/
	/* Choose the number of slots to keep */
	j = st_ptr->stock_num;

	/* Sell a few items */
	j = j - randint1(STORE_TURNOVER);

	j -= count_ability_card(ABL_CARD_MANEKINEKO);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "underflow" */
	if (j < 0) j = 0;

	/*:::�X�����A�C�e�������������𕡐���Ăяo���Ă���*/
	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j) store_delete();

	/*:::���l�ɓX�ɐV���ȏ��i��ǉ�*/
	/* Choose the number of slots to fill */
	j = st_ptr->stock_num;

	/* Buy some more items */
	j = j + randint1(STORE_TURNOVER);

	//v1.1.86 �������ȏ����L
	j += count_ability_card(ABL_CARD_MANEKINEKO) * 2;

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "overflow" */
	if (j >= st_ptr->stock_size) j = st_ptr->stock_size - 1;

	/* Acquire some new items */
	while (st_ptr->stock_num < j) store_create();
}


/*
 * Initialize the stores
 */
///sys store �I�[�i�[�ȂǏ���������
void store_init(int town_num, int store_num)
{
	int 		k;

	cur_store_num = store_num;

	/* Activate that store */
	/*:::�_���W�������u���}��A�[�P�[�h��town_num=NO_TOWN�ŌĂ΂��*/
	st_ptr = &town[town_num].store[store_num];


	/* Pick an owner */
	/*:::�����̒��ɂ��Ȃ��X���I�肵�Ă���*/
	while(1)
	{
		int i;

		st_ptr->owner = (byte)randint0(MAX_OWNERS);
		for (i = 1;i < max_towns; i++)
		{
			if (i == town_num) continue;
			if (st_ptr->owner == town[i].store[store_num].owner) break;
		}
		if (i == max_towns) break;
	}

	/* Activate the new owner */
	//ot_ptr = &owners[store_num][st_ptr->owner];


	/* Initialize the store */
	st_ptr->store_open = 0;
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/*
	 * MEGA-HACK - Last visit to store is
	 * BEFORE player birth to enable store restocking
	 */
	/*:::�ŏ��Ɂ����X�ɓ������Ƃ��݌ɍX�V���s����悤�ɍŏ��̃J�E���g��Ⴍ���Ă���*/
	st_ptr->last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;

	/* Clear any old items */
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		object_wipe(&st_ptr->stock[k]);
	}
}


void move_to_black_market(object_type *o_ptr)
{
	/* Not in town */
	if (!p_ptr->town_num) return;

	st_ptr = &town[p_ptr->town_num].store[STORE_BLACK];

	o_ptr->ident |= IDENT_STORE;

	(void)store_carry(o_ptr);

	object_wipe(o_ptr); /* Don't leave a bogus object behind... */
}


/*:::�A�C�e�����䂪�Ƃ܂œ`������*/
//mode0:���_���̎P�����@�����Ȃ�
//mode1:���R�̈��g���@�d��lev*2/50kg�܂�
//mode2:�����̈�]���w�@�����Ȃ�
//mode3:���ח̈拫�E�`���@��m���ŃA�C�e�����X�g
//mode4:Ex���[�h�_��_�o�����ł̃A�C�e���]���T�[�r�X�@�����Ȃ�
bool item_recall(int mode)
{
	object_type forge;
	object_type *q_ptr = &forge;

	char o_name[MAX_NLEN];
	int item, amt;
	object_type *o_ptr;
	cptr q, s;
	
	q = "�ǂ��T�����_�֑���܂���? ";
	s = "�A�C�e��������܂���B";

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0)	o_ptr = &inventory[item];
	else			o_ptr = &o_list[0 - item];

	amt = 1;
	if (o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return FALSE;
	}

	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;

	// Hack - �X����p����ϐ�������ɃZ�b�g
	cur_store_num = STORE_HOME;
	st_ptr = &town[TOWN_HITOZATO].store[cur_store_num];

	if (!store_check_num(q_ptr))
	{

		msg_print("���_�ɂ����u���ꏊ�͂����Ȃ��͂����B");
		return FALSE;
	}
	if(mode == 1 && (q_ptr->weight * q_ptr->number) > p_ptr->lev * 2)
	{
		msg_print("�ו����d���Ęh����ׂȂ��悤���B��߂Ă������B");
		return FALSE;
	}

	if (!get_check_strict("��낵���ł����H", CHECK_OKAY_CANCEL))
	{
		return FALSE;
	}

	distribute_charges(o_ptr, q_ptr, amt);

	object_desc(o_name, q_ptr, 0);

	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Handle stuff */
	handle_stuff();

	if(mode == 0)		
		msg_format("%s���X�L�}�ɕ��荞�񂾁B",o_name);
	else if(mode == 1)		
		msg_format("��h��%s�������Ĕ��ł������B",o_name);
	else if(mode == 2)		
		msg_format("%s�͖��@�w�̒��Ō�������ď������B",o_name);
	else if(mode == 3)
	{
		msg_format("%s���X�L�}�ɓ��ꂽ...",o_name);
		if(one_in_(13))
		{
			msg_format("���������Ɓ�",o_name);
			return TRUE;
		}
		else
		{
			msg_format("�]���ɐ��������悤���B",o_name);
		}
	}
	else if(mode == 4)
	{
		int ex_bldg_num = f_info[cave[py][px].feat].subtype;
		int ex_bldg_idx = building_ex_idx[ex_bldg_num];

		if(ex_bldg_idx == BLDG_EX_SHINREIBYOU)
		{
			if(p_ptr->pclass == CLASS_MIKO)
				msg_format("%s�����Ȃ��̐�E�ɉ^�񂾁B",o_name);
			else 
				msg_format("%s�����_�ւƉ^��ł�������B",o_name);
		}
		else if(ex_bldg_idx == BLDG_EX_ORIN)
		{
			if(p_ptr->pclass == CLASS_SATORI ||p_ptr->pclass == CLASS_KOISHI || p_ptr->pclass == CLASS_UTSUHO)
				msg_format("%s�����ӂɗa�����B",o_name);
			else if(one_in_(10))
				msg_format("�u���̂Ȃ���ł�����ł��^�Ԃ񂾂��ǂ˂��B�v");
			else
				msg_format("%s���Ή��L�ӂɗa�����B",o_name);
		}

	}

	/* Let the home carry it */
	home_carry(q_ptr);


	return TRUE;

}

//����p�@����̎w��A�C�e����1���炷
void yukari_dec_home_item(int item)
{
	st_ptr = &town[TOWN_HITOZATO].store[STORE_HOME];
	store_item_increase(item, -1);
	store_item_optimize(item);

}

//����p�@����փA�C�e����1����@��������ς��̎�FALSE
bool yukari_send_item_to_home(object_type *o_ptr)
{

	cur_store_num = STORE_HOME;
	st_ptr = &town[TOWN_HITOZATO].store[cur_store_num];

	if (!store_check_num(o_ptr))
	{
		return FALSE;
	}
	if(home_carry(o_ptr) < 0)
	{
		msg_print("ERROR:yukari_send_item_to_home()��home_carry�߂�l����������");
		return FALSE;
	}

	return TRUE;

}





