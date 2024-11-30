/* Purpose: create a player character */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * How often the autoroller will update the display and pause
 * to check for user interuptions.
 * Bigger values will make the autoroller faster, but slower
 * system may have problems because the user can't stop the
 * autoroller for this number of rolls.
 */
#define AUTOROLLER_STEP 5431L

/*
 * Define this to cut down processor use while autorolling
 */
#if 0
#  define AUTOROLLER_DELAY
#endif

/*
 * Maximum number of tries for selection of a proper quest monster
 */
#define MAX_TRIES 100

//#define MAX_CLASS_CHOICE     MAX_CLASS
///mod140202 ���j�[�N�N���X�����̂��߂ɒʏ�I��E�Ɣԍ��ő��ʂɂ���
//v1.1.87 �J�[�h���l�ǉ��̂���27��28 ����ȏ��ʐE�𑝂₷�Ȃ炱�̕ӂ̃��[�`���̏����������K�v�B
//v2.0.19 �{�I�Ǝ����ɂ��E�ƑI�𕔕���ύX�B���̒萔��p�~
//#define MAX_CLASS_CHOICE     28


static bool old_data_cheat_flag = FALSE;
static bool old_collector = FALSE;

/*
 * Forward declare
 */
typedef struct hist_type hist_type;

/*
 * Player background information
 */
struct hist_type
{
	cptr info;			    /* Textual History */

	byte roll;			    /* Frequency of this entry */
	byte chart;			    /* Chart index */
	byte next;			    /* Next chart index */
	byte bonus;			    /* Social Class Bonus + 50 */
};

///del1401 ������bg[]�Ƃ��Đ��������̃e���v�����͂����������S��������




static cptr info_difficulty[5] ={
	"���K�p�̃��[�h�ł��B�v���C���[�̔\�͂��オ��A�Q�[���I�[�o�[�ɂȂ��Ă��R���e�B�j���[���\�ł��B���̃��[�h��50�K�ŏI�����܂��B�X�R�A���啝�ɒቺ���܂��B",
	"�ʏ�̃��[�h�ł��B�G�̗̑͂��Ⴍ�A�܂����܂�Ђǂ����Ƃ͋N����ɂ����悤�ɒ�������Ă��܂��B50�K�N���A���_�ŃQ�[���N���A�����ɂȂ�܂��B",
	"�G�����͋����Ȃ�A���΂��Ώ��Ղ��狭�G���o�����A�������̎����ɕs�K�������o�܂��B��{�I�ɕϋ�ؓ{�Ɠ��l�̐ݒ�ł����A�����L�����N�^�[�Ńv���C����Εϋ�ؓ{���ȒP�ł��B�X�R�A��NORMAL��2�{�ł��BHARD�ȍ~�łȂ��ƑI���ł��Ȃ��E�Ƃ␫�i�����݂��܂��B",
	"�G�����܂����͋����f�������Ӑ[���Ȃ�A�L�蓾�Ȃ��قǂ̋��G���p�ɂɏo�����A���s�s�Ȃ܂ł̗l�X�ȍ�����Ȃ��ɏP���|����܂��B�X�R�A��NORMAL��8�{�ł��B",
	"�ɂ߂ē���ȃ��[�h�ł��B�S��1�K����Q�[�����J�n���ꂻ������~���݂̂ł��B���x�����オ��₷���Ȃ�A�[�w�̃A�C�e�����o�����₷���Ȃ�A���Օi�������܂Ƃ߂Đ�������A�u�A�C�e���J�[�h�v���o�����܂��B�����_���N�G�X�g�͂Ȃ��Ȃ�A���K���Ƃɓ���̃N�G�X�g�_���W��������������܂��B�\�S�ȏ����𐮂����Ȃ����ōőP��s�������Ƃ����߂��܂��B�X�R�A��NORMAL��4�{�ł��B"
};

static cptr race_jouhou[MAX_RACES] =
{
#ifdef JP
"���Ȃ��͕��ʂ̐l�Ԃł��B�l�Ԃ͎�ɐl���ɏZ��ł��܂����A�O�E�����������ł��邱�Ƃ�����܂��B���z���̂ق��̏Z�l�B�ɔ�ׂ�Ǝ������Z�����̓I�ɂ��n��ł����A�����̑����ɂ͓��M���ׂ����̂�����܂��B�G�ł��_���Ȃ�����Ɏ�_�炵����_���Ȃ��A�ǂ̂悤�ȐE�Ƃł�����Ȃ�ɂ��Ȃ����Ƃ��\�ł��B",
  
"���Ȃ��͌��z���̂��������ɐ��ݒ����Ⴋ�d�������̈�l�ł��B�Ⴂ�ƌ����Ă��y���S�N�͐����Ă��邩������܂��񂵁A�Ђ���Ƃ���ƍ�����܂ꂽ�΂��肩������܂���B�d���͈�ʂɒʏ�̐l�Ԃ�傫������g�̔\�͂������܂����A��������Ƃ��l����̂͋��ŏp�t�ɂ͌����܂���B�����͔j�ׂ̍U���Ɏキ�A�j�ב����̍U���▂�@�ɂ��傫�ȃ_���[�W���󂯂Ă��܂��܂��B",
  
"���Ȃ��͐l�ԂƗd���̃n�[�t�ł��B�ʏ�̐l�Ԃ��͂���v�ȑ̂������A���x�����オ��Ɠłɑ΂���ϐ��𓾂܂��B�܂���r�I���Ȃ��H���Ŋ������邱�Ƃ��\�ł��B��_�炵����_�͎������A�ǂ̂悤�ȐE�Ƃł�����Ȃ�ɂ��Ȃ��܂��B�l�Ԃɔ�ׂ�ƒ��������������܂����A���̂Ԃ񐬒������x���Ȃ�܂��B",
  
"���Ȃ��͐�l�ł��B�����C�s�̉ʂĂɎ������������A����Ȃ鍂�݂����߂ďC�s�𑱂��Ă��܂��B�g�̔\�͂͏�l���݂ł������̐��_�͌������܂���Ă���A�Ƃ��ɐ�p���������ƂɊւ��Ă͍ō��N���X�̓K���������܂��B�܂������킸���ȐH�ƂŊ������邱�Ƃ��\�ł��B���������ėl�X�Ȃ��Ƃ��o�����Ă������߂�␬�����x���Ȃ�܂��B�{���̎����𖳗���ɐL�΂��Ă��邽�߂��΂��Βn������̎h�q�ɑ_���܂��B���x�����オ��ƍ����Ȃǂɑ΂���ϐ��𓾂܂��B",
  
"���Ȃ��͓V�E�̏Z�l�ł��B���X�̂��r�񂾂�x������Ǝ��R�C�܂܂ɕ�炵�Ă��܂����A�ʏ�̐l�Ԃ�ꡂ��ɗ��킷��퓬�\�͂������܂��B��p�ɑ΂��čō��N���X�̓K���������܂����d�p�͑S���ƌ����Ă����قǈ����܂���B�������𐶂��Ă���̂Ő������x�͊ɖ��ł��B��l�Ɠ��l�ɂ��΂��Βn������̎h�q�ɑ_���܂��B�V�l�̐���ȓ��͔̂j�ׂ̗͂ɂ��U����S���󂯕t���܂��񂪁A�V�l�̓��̂ɑ�Ō���^����U�������݂��܂��B",
  
"���Ȃ��͗d���ł��B�d���͎��R�̗͂̋�Ō��z���̎��鏊�ɏo�����܂��B���Y����D���ł��������Ȑ퓬�s�ׂ͐�]�I�ɕs����ł��B�g�̔\�́A�m���Ƃ��Ɍ���ׂ��Ƃ���͂قƂ�ǂ���܂���B���̏㎩���b����Ƃ������z���Ȃ����ߐ������x�����ɖ��ł��B�B��B��邱�Ƃ����͓��ӂł��B�����`����S���ł���Α傫�ȗ_��ƂȂ邱�Ƃł��傤�B",
  
"���Ȃ��͉͓��ł��B���ӂɏZ�݉j�����B�҂Ő�΂ɓM��邱�Ƃ�����܂���B�d���̂��ɂ͂���قǋ��x�ȓ��̂������Ă��܂��񂪁A���̒��ł͌��Ⴆ��悤�ȗ͋����𔭊����܂��B�r��L�΂��ĉ����̓G�ɍU������Ɠ��̓��Z�������܂��B���̑ϐ��̂ق��A��������Ύ_�ւ̑ϐ����l�����܂����d���͋��ł��B�����͓����G���W�j�A�ɂȂ����ꍇ�͐�����_�Ƃ��Ȃ����͂ȋ@�B�������܂��B",
  
"���Ȃ��͖��@�g���ł��B�����Ō������@�g���Ƃ͐E�Ƃł͂Ȃ��A���@�̒T���̉ʂĂɎ���̖��@�Ŏ��������������悤�ɂȂ����ҒB�̂��Ƃ������܂��B���͐l�Ԃł������҂������̂ł������܂���̖��@�g�������݂��܂��B�g�̔\�͕͂n��ł������̒m���Ɩ��@�S�ʂւ̓K���͐��ł��B���x�����オ��ɂꂻ�̐��_�͂���Ɍ��S���𑝂��A������m�\�ቺ�U���ւ̑ϐ����l�����܂��B�܂������ɐڐG���Ė��͂��z�����邱�Ƃ��ł���悤�ɂȂ�܂��B���@�g���͖��͂Ŋ������Ă��邽�߈�؂̐H�������Ȃ��Ă��|��܂���B",
  
"���Ȃ�����V��ł��B�V��͗d���̎R�ň�吨�͂�z���W�c�ŁA��V��͂��̒��ŕ񓹕����̖�����S���Ă��܂��B�ɂ߂č����q�������ւ�A�m������r�I�����d�p�⎩�R�̐�p�𓾈ӂƂ��Ă��܂��B����ȃJ�������g���Ă���G�̒e���U����{���g���@���������Ė��������邱�Ƃ�����܂��B�V��͓��Ɂu���݁v�𑕔����Ȃ���΂Ȃ�܂���B",
  
"���Ȃ��͔��T�V��ł��B�V��͗d���̎R�ň�吨�͂�z���W�c�ŁA���T�V��͂��̒��Ŏ��q�c�̖�����S���Ă��܂��B���������\�͂ƒT�m�\�͂��ւ�A�Ɨ͂ŉߍ��Ȋ��ɓK�����邱�Ƃ��ł��܂��B�V��͓��Ɂu���݁v�𑕔����Ȃ���΂Ȃ�܂���B",
  
"���Ȃ��͋S�ł��B�S�͌Â�������p����鋭�͂ȗd���ŁA�S�푰�ō��̍��r�Ƒ̗͂��ւ蔒����ł͂����炭�ŋ��ł��B����ŏ��׍H�ɗ��邱�Ƃ��D�܂����@��T���Ȃǂ͋��ł��B�S�͗��h�Ȋp�𓪂ɐ��₵�Ă���A����͊i�����ɋ��͂ȕ���Ƃ��ċ@�\���܂����ꕔ�̓��h������ł��Ȃ��Ȃ�f�����b�g������܂��B���Ȃ�̑�ѐH�炢�Ȃ̂ŐH�Ƃ͑��߂Ɏ����܂��傤�B",
  
"���Ȃ��͎��_�ł��B���_�͔ފ݂̏Z�l�ŁA�O�r�̐�̑D����腖��l�̍ٔ��̏��L�ȂǑ��ʂȖ�E�ɏ]�����Ă��܂��B�퓬�\�͍͂����A��Ⴢ⍬���ȂǑ����̍U���ɑϐ��������܂��B�劙�𑕔�����Ɖ��ƂȂ������t���ĕ���̈З͂��㏸���܂��B",
  
"���Ȃ��͏b�l�ł��B�b�l�͐l�Əb�̗����̓����������݂ŁA���܂���̏b�l�Əb�l�������l�ԂƐl�������b�ɕ�����܂��B�����̏b�l�͐l�Ԃ�荂���g�̔\�͂������A���̈���Œm���͒ቺ���Ă��܂��B�����̔ӂɂ͗͂������܂��B�b�l�͏b�̉s�q�Ȋ��o�������A�댯���@���邱�Ƃ����ӂł��B",

"���Ȃ��͕t�r�_�ł��B�t�r�_�͌Â������������������݂ŁA�_�̈��ł������Ԃ͂قڗd���ł��B�퓬�\�͂͂���߂ł����A����̐S���킩��̂�㩉����▂����g�p�����ӂł��B���x�����オ��Ǝ􂢂̂����������̂𑕔����Ă����̈��e�����ɘa���邱�Ƃ��\�ɂȂ�܂��B�ʏ�̐����̐g�̍\���������Ȃ����ߓł�ڒׂ��Ȃǂ̉e�������܂�󂯂܂���B�H���̑���ɖ�����̃G�l���M�[���z�����ĉh�{�𓾂܂��B�j�ׂƗ򉻍U������_�ł��B",

"���Ȃ��͗d���Ƃ��Ď���𓾂��Â��l�`�ŁA�t�r�_�̈��ł��B�G��|�����Ƃł͌o���l�𓾂邱�Ƃ��ł����A�g�ɂ�������̗͂������̗͂Ɠ��������ă��x���A�b�v���܂��B���͂ȕ�������Ɏ�ɓ���Έ�C�ɖ`�����y�ɂȂ邱�Ƃł��傤�B�ǂ��炩�Ƃ����Əp�m����m���̓����������܂��B�d���l�`�͖��@�̏�Ȃǂ��疂�͂��z������ĉh�{�ɂ��܂��B���͂Ŋ������Ă��邽�ߗ򉻍U������_�ł��B�l�`�̑̂̂��ߓł��C�������ɂ����ق��A��������Ƌ��|�⋶�C�ɑ΂���ϐ��𓾂܂��B",
  
"���Ȃ��͌����痈���d���e�ł��B���̖h�q���Ƃ��Đ퓬�P����ς�ł������ߗl�X�ȋZ�\�ɏG�ł܂����A�����ӂ��҂̂��ߐ����͂��x�߂ł��B�傫�Ȏ����g���Ăǂ��ɂ��Ă����̉\�𕷂����Ƃ��ł��܂��B�q��Ȃ����̏Z�l�̂��ߔj�׍U���ɑϐ��������܂����A����ꕔ�̍U���ɑ΂��Ă͑�Ō����󂯂Ă��܂��܂��B",
  
"���Ȃ��͐l�������d���ςł��B�d���̊��ɂ͔�͂ł����d���炵����ʍ����m���������p�m�Ɍ����Ă��܂��B�Ƃ��ɕϐg����p�𓾈ӂƂ��Ă��܂��B�E�ƂɊ֌W�Ȃ��G��f�킹��\�͂������Ă���A��������Ɛ��_���m�\�͂��l�����܂��B����ɏb�̉s�q�Ȋ��o������Ă��邽�ߐ����\�͂����߂ł��B�����x���ɂȂ�Ǝ኱�̐_�i�𓾂Ĕj�ׂ���_�Ƃ��Ȃ��Ȃ邱�Ƃ�����܂��B���͂Ȏ푰�ł��������ɂ͂�Ⓑ�����Ԃ�������܂��B",
  
"���Ȃ��͎R�ɏZ�܂��͓��Ɏ����d���ł��B�R��X�ł̊��������ӂŁA�X�n�`�̒��ł͕��i�ȏ�̗͂𔭊����܂��B�܂����V�\�͂��Ȃ��Ă��������R�ɓo�邱�Ƃ��ł��܂��B",
  
"���Ȃ��͐l�������d���K�ł��B�m�ɒ����Ă���ϐg�ɂ���Đl���x������������邱�Ƃ��D�݂܂����A���e������ł͂���܂���B��������Ɖ��̂悤�Ɏp�������ēG�̍U�����]�����Z���K�����܂��B",
  
"���Ȃ��͐l���ł��B�l�Ԃ̏㔼�g�Ƌ���ȋ��̉����g�������Ă��܂��B����Ȕ��̂��ߌC�𗚂����Ƃ��ł����A���V�����ɒn��ɂ���ꍇ�ړ����x���������܂��B�����������ł͋ɂ߂đf�����s���ł��A�ߐڐ퓬�̂Ƃ����傫�Ȕ��ɂ��Ō��ɂ͂��Ȃ�̈З͂����҂ł��܂��B���@�̓K�����Ⴍ�͂Ȃ���ɗd�p�����ӂł��B",
  
"���Ȃ��̓z�t�S�u�����ł��B�ƂɍK���������Ƃ�����~���̌㊘�Ƃ��Č��z���ɏ����ꂽ���̂̌����ڂ̂��܂�̕|���ɐl������ǂ��o����Ă��܂����C�̓łȗd���ł��B�c�����݂̑̊i�̂��ߓ��e��͂����ł����A�Z�\�␬�����Ɋւ��Ă͈ĊO�D�G�ł��B��������ƓłƖ�Ⴢɑ΂���ϐ��𓾂܂��B",
  
"���Ȃ��͓����ł��B���h�ȑ̊i�������܂����o�����������傫���Ȃ����菬�����Ȃ�����ƒ݂͂ǂ���̂Ȃ��d���ł��B���̎��̞̂B�����ɂ�蕨���h��͂������A��������Ɩ�ჁA�j�ЁA�����U���ɑ΂��ϐ��𓾂܂��B��m�����̔\�͂��������@�̏K���͋��ł����A�ϗe�̗d�p�����͓��ӂł��B",
  
"���Ȃ��͏������ł��B�������͂��܂�͂������Ȃ������̑��̂ł��B�m�\�����߂ł��p�m���̔\�͂������܂������e�������Ȃ�ɂ��Ȃ��܂��B���p�Ɨd�p�����ӂŐ�p�͂��Ȃ���ł��B�������͈Í��ƒn���U���ւ̑ϐ��������ِ��E�̜��܂������݂�ڂɂ��Ă����܂舫�e�����󂯂܂��񂪁A�j�׍U���ɂ�蕁�ʂ̗d���ȏ�̑Ō����󂯂Ă��܂��܂��B",
  
"���Ȃ��͖��@�ɂ���ĉ��̖��𐁂����܂ꂽ�Ή�ł��B�����r�͂Ƒϋv�͂������܂��������͋ɂ߂Ċɖ����ݏd�ł��B�m�����قƂ�ǂȂ����@�̏K���͋ɂ߂č���ł��B�΂̑̂̂��ߑ����̍U���ɑϐ��������܂����A��(DISENCHANT)�̍U���ɑ΂��Ă͑�Ō����󂯂Ă��܂��܂��B�H�Ƃ͊��z�ΗނŁA���A�ȍz�΂�H�ׂ�Ƒ̗͂��񕜂�����L���Ȍ��ʂ��o�邱�Ƃ�����܂��B���x�����オ��ƃ_���W�����̕ǂ����̂܂ܐH�ׂ邱�Ƃ��o����悤�ɂȂ�܂��B",
  
"���Ȃ��͏��l�ł��B���ʂ̐l�Ԃ̕G�����x�̐g�����������Ȃ��ꑰ�ŁA�̗͂͗d����H��ɂ����S�푰�Œ�ł��B���̏゠�܂�̑̂̏������̂��ߏd�����̂𑕔��ł��܂���B�������B���\�͂Ɖ��\�͂͌Q�𔲂��č����A��������̂��������Ȃ����������̑������S�푰�ň�Ԃł��B�T���▂����g�p�����ӂł��B",
  
"���Ȃ��͑嗤�̖��@�ɂ���đ���ꕅ�s��Ƃ�Ă��鎀�̂ł��B�̂��d���������Ⴍ�������������ł��܂��񂪎��Ԃ������Ēb����΂���Ȃ�ɏ_��ȓ������ł���悤�ɂȂ�܂��B�r�͂Ƒϋv�͍͂����ł�n����Í��ȂǗl�X�ȍU���ւ̑ϐ��������܂����j�׍U���ɑ΂��ɂ߂ĐƎ�ł��B",
  
"���Ȃ��͋��͂Ȗ���ő��̈���ł��B�l�Ԃ�ꡂ��ɒ�����g�̔\�͂ɉ����s���g�ƌ������قǂ̑ϋv�͂��ւ�܂��B�����̖�ɂȂ�Ƃ��̗͍͂X�ɑ����܂����A���z������Ǝ�̉����Ă��܂��܂��B�����̑����ɑ΂����R�͂��������_�������A�M���Ɛ��̍U���ő�_���[�W���󂯁A�j�׍U���ł͂���ɐr��ȃ_���[�W���󂯂Ă��܂��܂��B���e��Ɩ��@�̗��������ӂł�����p�͋��ł��B�z���S�͌������Ȃ��Ă��Â��_���W���������ʂ��\�͂������Ă��܂��B�ʏ�̐H���ł͋󕠂𖞂������Ƃ��ł����A�����Ɋ��݂��Č��t���z����邱�Ƃŉh�{�𓾂܂��B",
  
"���Ȃ��͎����ĂȂ������ɗ��܂鍰�ł��B�ǂ����蔲���邱�Ƃ��ł��A�_���W�����T���ɂ����Ĕ��ɗL���ł��B�������̂��󔖂Ȃ��ߔ�����\�͂Ƒϋv�͍͂Œ�N���X�ł��B�n���̗͂ɑ΂��Ċ��S�ȖƉu�������A�ł�j�ЂƂ������댯�ȍU���ɂ��ϐ��������܂����A���Ɣj�ׂ̍U���ɑ΂��Ă͔��ɐƎ�ł��B���x���A�b�v�ɂ��߂��̎v�O�����m����\�͂𓾂܂��B�Ȃ��A�H���͕��ʂɐۂ�܂��B",
  
"���Ȃ����J����_�Ђ������Ȃ��G���Ȑ_�l�����̂����̈ꒌ�ŁA�M���قڎ�����R�łЂ������炵�Ă��܂����B�`����ʂ��ėl�X�Ȍo����ςނ��ƂŐM���W�܂�A���x�������ɂȂ�ƐV���Ȑ_���𓾂Ď푰�I�ȓ������傫���ς��܂��B�ǂ̂悤�ɕω����邩�ɂ��Ă͗l�X�ȗv�f�ɉe������܂����A�Ⴆ�Ζ��̂��鋭�G��|������N�G�X�g�����Ȃ��Ė��������߂邱�Ƃ͑����̏ꍇ�ǂ����ʂ������炷�ł��傤�B�Ȃ��A���Ȃ����ŏ����̐_�l�Ȃ̂��͑S�������_���Ɍ��܂�܂��B",
  
"���Ȃ��͒��������ė͂�~������d���ł��B���ʂ̗d����薾�炩�ɋ����͂������܂����A���Ȃ��ɂƂ��Ă��̐��̂��ׂĂ͌����ꂽ���̂΂���Ȃ̂Ő����̑��x�͋ɂ߂Ċɖ��ł��B��d���Ƃ͂����ˑR�Ƃ��Ĕj�׍U���͂��Ȃ��̎�_�ł��B",
  
"���Ȃ��͂����炩�̐_�����������l�Ԃł��B�����Ȃ����̂�����\�͂������A�ʏ�̐l�Ԃ��S�̓I�Ȕ\�͂���⍂�߂ł��B���x�����オ��ƒx�����Ƌ}�񕜂̔\�͂𓾂܂��B�����͐l�Ԃ�菭���x���Ȃ�A�X�R�A�����ቺ���܂��B",

"���Ȃ��͂�񂲂ƂȂ�����ɂ��n��ɍ~��Ă����M�����̖��ł��B�d���Q�������̓s�ŉi�������߂����Ă������߁A�����m�͕��̒n��l�̋y�ԂƂ���ł͂���܂���B	���Ȃ��͎��̓����Ɛ����̑������u�q��v�Ƃ��ċ����������Ă��܂��B���̂��ߗd�p�ɕ��ނ���閂�@�����܂�g���������⎩�R�̖��@����̋��ł��B	���Ȃ��͔j�׍U���ɑ΂��銮�S�ȖƉu�������܂����A�q�ꂽ�͂ōU�������Ƒ�_���[�W���󂯂Ă��܂��܂��B�l�X�ȋZ�\�ɗD��܂������܂���e����D�݂܂���B",
  
"���Ȃ��͕�������̃j���W���\�E���̜߈˂ɂ���ăj���W���Ƃ��Đ��܂�ς��܂����B",
  
"���Ȃ��͋ɂ߂Ē������l�ԂƗH��̃n�[�t�ł��B�l�Ԃ̑̂Ɋ��Y���悤�ɐl�����ЂƂ�����ł���ǂ�������Ȃ��̑̂ł��B���Ȃ��͒ʏ�̐l�Ԃ�莀�ɋ߂��Ƃ���ɂ��邽�ߎ��Ɋ֌W���邱�Ƃɑ΂��Ă�����x�̓K���������܂��B�܂��C�z���������߉B���s�������ӂł��B",
  
"���Ȃ��͖{���͐l�Ԃ̕G���x�̔w�䂵���Ȃ����l�ł����A���͏��Ƃ̗͂Ől�ԑ�ɂȂ��Ă��܂��B",
  
"���Ȃ��͎���p�̉��`�ɂ�苭�͂ȃA���f�b�h�ɓ]�����܂����B���@�Ɋւ���\�͂�����I�ɏ㏸���A�������̎׈��ȓ��Z���K�����܂��B���e��ɂ͑S�������܂��񂪁A���Ȃ��͓��̂ɗ��炸�키���@�����ɏ[���ɒm���Ă��܂��B�ϋv�͂�ϐ����D�G�ł����A�j�׍U�����󂯂�Ɣ�Q�͋ɂ߂Đr��ł��B",
  
"���Ȃ��͋��ɂ̐����̂ł��B�����鐶���̎��\�͂��s�g�ł��܂��B",

"���Ȃ��́A�l�X�ȕ����ɏh��{�����_�b�`���ɂ��؂�����`�Â���ꂽ���݂ł��B�ʏ�̐����△�����Ƃ͑S���قȂ����݂�������Ă��邽�߁A������푰�ϗe���󂯕t���܂���B",

"���Ȃ��͏��q�����ł��B�O�E�ŗB�ꖳ��̍ŋ����G�̎푰�ł��B",

"���Ȃ��͋֒f�̔��ɂ���ĕs�V�s���ƂȂ������݂ł��B������푰�ϗe�A�ˑR�ψق��󂯕t���܂���B�܂��ϐg�n�̖��@�����ʂ𔭊����܂���B",

"���Ȃ��͂��Ă��n���l�ł������A���@�̗͂ɂ���Đ����𓾂ē����o���܂����B�S�[�����Ɩ��@�g���̒��Ԃ̂悤�ȑ��݂ŁA�H����K�v�Ƃ�����v�ȑ̂��������@�ւ̓K�������߂ł��B����������͂��Ȃ�ɖ��Œ��ӗ͂ɂ������Ă��܂��B",

"���Ȃ��͑��`�_�ɂ���ēy�Ɛ�����n��o����A�����ʂ�̐_�Ƃɂ�荰�𐁂����܂ꂽ�l�ԃT�C�Y�̏���(�n�j��)�ł��B���Ȃ��͍��������Ȃ������̂������Ȃ������̂��߁A��I�E���_�I�ȍU���ɑ΂��قڐ�ΓI�Ȓ�R�͂𔭊����܂��B�܂��ł▃ჂƂ������������Q����U���ɑ΂��Ă��ϐ�������܂��B���������Ȃ��̑̂͋����Ռ����󂯂�Ɗ���Ă��܂���������܂���B�j�Ђ⍌���̍U���ɂ͒��ӂ��܂��傤�B",

"���Ȃ��͎�����H�̒{���E�ɐ��ޓ�����ł��B������̒��ł��͂������Ă���A��ɐl�Ԃɋ߂��p�ɕω����Ă��܂��B�{���E�ł͑����̋���g�D���e���������đ����Ă���A�قƂ�ǂ̓�����͂ǂꂩ�̑g�D�ɑ����Ă��܂��B������g�D�ɂ���ă��C�V�����p���[���ω����܂��B���Ȃ��͋ߐڐ퓬�����ӂŗl�X�ȍU���ɑ΂���ϐ��������܂������Ɣj�ׂ̍U���͑�̋��ł��B�܂����Ȃ��͒ʏ�̐H���ł͂قƂ�ǉh�{�𓾂邱�Ƃ��ł����A�G��|���ė͂��z�����邱�ƂŊ��͂𓾂܂��B",

"���Ȃ��͐l���Ől�C�̍��~��炵�ł��B�{�����~��炵�͉��~����o�������Ƃ��ł��܂��񂪁A�R�������̂悤�ȕ��i�Ɏ��߂��Ĉړ������~�Ƃ��Ď���������@����������A���Ȃ����O��̗��ɏo����悤�ɂȂ�܂����B�`���̗��ŐV���ȉ��~�ɂȂ肤��i�𔭌��������芷���邱�Ƃ��ł��A���~�̓����ɂ��\�͂���Z�������ω����܂��B���Ȃ��͒T���E���m�n�̔\�͂ɗD��܂����A�q�����̂��̂̑̊i�Ȃ̂ōr���ɂ͌����Ȃ���������܂���B",

#else

"The human is the base character.  All other races are compared to them.  Humans can choose any class and are average at everything.  Humans tend to go up levels faster than most other races because of their shorter life spans.  No racial adjustments or intrinsics occur to characters choosing human.",

"Half-elves tend to be smarter and faster than humans, but not as strong.  Half-elves are slightly better at searching, disarming, saving throws, stealth, bows, and magic, but they are not as good at hand weapons.  Half-elves may choose any class and do not receive any intrinsic abilities.",

"Elves are better magicians then humans, but not as good at fighting.  They tend to be smarter and faster than either humans or half-elves and also have better wisdom.  Elves are better at searching, disarming, perception, stealth, bows, and magic, but they are not as good at hand weapons.  They resist light effects intrinsically.",

"Hobbits, or Halflings, are very good at bows, throwing, and have good saving throws.  They also are very good at searching, disarming, perception, and stealth; so they make excellent rogues, but prefer to be called burglars.  They are much weaker than humans, and no good at melee fighting.  Halflings have fair infravision, so they can detect warm creatures at a distance.  They have a strong hold on their life force, and are thus intrinsically resistant to life draining.",

"Gnomes are smaller than dwarves but larger than Halflings.  They, like the hobbits, live in the earth in burrow-like homes.  Gnomes make excellent mages, and have very good saving throws.  They are good at searching, disarming, perception, and stealth.  They have lower strength than humans so they are not very good at fighting with hand weapons.  Gnomes have fair infra-vision, so they can detect warm-blooded creatures at a distance.  Gnomes are intrinsically protected against paralysis.",

"Dwarves are the headstrong miners and fighters of legend.  Dwarves tend to be stronger and tougher but slower and less intelligent than humans.  Because they are so headstrong and are somewhat wise, they resist spells which are cast on them.  They are very good at searching, perception, fighting, and bows.  Dwarves  have a miserable stealth.  They can never be blinded.",

"Half-orcs make excellent warriors, but are terrible at magic.  They are as bad as dwarves at stealth, and horrible at searching, disarming, and perception.  Half-orcs are quite ugly, and tend to pay more for goods in town.  Because of their preference to living underground to on the surface, half-orcs resist darkness attacks.",

"Half-Trolls are incredibly strong, and have more hit points than most other races.  They are also very stupid and slow.  They are bad at searching, disarming, perception, and stealth.  They are so ugly that a Half-Orc grimaces in their presence.  They also happen to be fun to run...  Half-trolls always have their strength sustained.  At higher levels, Half-Trolls regenerate wounds automatically, and if he or her is warrior slowly.",

"The Amberites are a reputedly immortal race, who are endowed with numerous advantages in addition to their longevity.  They are very tough and their constitution cannot be reduced, and their ability to heal wounds far surpasses that of any other race.  Having seen virtually everything, very little is new to them, and they gain levels much slower than the other races.",

"High-elves are a race of immortal beings dating from the beginning of time.  They are masters of all skills, and are strong and intelligent, although their wisdom is sometimes suspect.  High-elves begin their lives able to see the unseen, and resist light effects just like regular elves.  However, there are few things that they have not seen already, and experience is very hard for them to gain.",

"Barbarians are hardy men of the north.  They are fierce in combat, and their wrath is feared throughout the world.  Combat is their life: they feel no fear, and they learn to enter battle frenzy at will even sooner than half-trolls.  Barbarians are, however, suspicious of magic, which makes magic devices fairly hard for them to use. ",

"Half-Ogres are like Half-Orcs, only more so.  They are big, bad, and stupid.  For warriors, they have all the necessary attributes, and they can even become wizards: after all, they are related to Ogre Magi, from whom they have learned the skill of setting trapped runes once their level is high enough.  Like Half-Orcs, they resist darkness, and like Half-Trolls, they have their strength sustained.",

"Half-Giants limited intelligence makes it difficult for them to become full spellcasters, but with their huge strength they make excellent warriors.  Their thick skin makes them resistant to shards, and like Half-Ogres and Half-Trolls, they have their strength sustained.",

"Half-mortal descendants of the mighty titans, these immensely powerful creatures put almost any other race to shame.  They may lack the fascinating special powers of certain other races, but their enhanced attributes more than make up for that.  They learn to estimate the strengths of their foes, and their love for law and order makes them resistant to the effects of Chaos.",

"With but one eye, a Cyclops can see more than many with two eyes.  They are headstrong, and loud noises bother them very little.  They are not quite qualified for the magic using professions, but as a certain Mr.  Ulysses can testify, their accuracy with thrown rocks can be deadly...",

"Yeeks are among the most pathetic creatures.  Fortunately, their horrible screams can scare away less confident foes, and their skin becomes more and more resistant to acid, as they gain experience.  But having said that, even a mediocre monster can wipe the proverbial floor with an unwary Yeek.",

"Klackons are bizarre semi-intelligent ant-like insectoid creatures.  They make great fighters, but their mental abilities are severely limited.  Obedient and well-ordered, they can never be confused.  They are also very nimble, and become faster as they advance levels.  They are also very acidic, inherently resisting acid, and capable of spitting acid at higher levels. ",

"Kobolds are a weak goblin race.  They love poisoned weapons, and can learn to throw poisoned darts (of which they carry an unlimited supply).  They are also inherently resistant to poison, although they are not one of the more powerful races.",

"The hated and persecuted race of nocturnal dwarves, these cave-dwellers are not much bothered by darkness.  Their natural inclination to magical items has made them immune to effects which could drain away magical energy.",

"Another dark, cave-dwelling race, likewise unhampered by darkness attacks, the Dark Elves have a long tradition and knowledge of magic.  They have an inherent magic missile attack available to them at a low level.  With their keen sight, they also learn to see invisible things as their relatives High-Elves do, but at a higher level.",

"A humanoid race with dragon-like attributes.  As they advance levels, they gain new elemental resistances (up to Poison Resistance), and they also have a breath weapon, which becomes more powerful with experience.  The exact type of the breath weapon depends on the Draconian's class and level.  With their wings, they can easily escape any pit trap unharmed.",

"A secretive and mysterious ancient race.  Their civilization may well be older than any other on our planet, and their intelligence and wisdom are naturally sustained, and are so great that they enable Mind Flayers to become more powerful spellcasters than any other race, even if their physical attributes are a good deal less admirable.  As they advance levels, they gain the powers of See Invisible and Telepathy.",

"A demon-creature from the nether-world, naturally resistant to fire attacks, and capable of learning fire bolt and fire ball attacks.  They are little loved by other races, but can perform fairly well in most professions.  As they advance levels, they gain the powers of See Invisible.",

"A Golem is an artificial creature, built from a lifeless raw material like clay, and awakened to life.  They are nearly mindless, making them useless for professions which rely on magic, but as warriors they are very tough.  They are resistant to poison, they can see invisible things, and move freely.  At higher levels, they also become resistant to attacks which threaten to drain away their life force.  Golems gain very little nutrition from ordinary food, but can absorb mana from staves and wands as their power source.  Golems also gain a natural armor class bonus from their tough body.",

"There are two types of skeletons: the ordinary, warrior-like skeletons, and the spell-using skeletons, which are also called liches.  As undead beings, skeletons need to worry very little about poison or attacks that can drain life.  They do not really use eyes for perceiving things, and are thus not fooled by invisibility.  Their bones are resistant to sharp shrapnel, and they will quickly become resistant to cold.  Although the magical effects of these will affect the skeleton even without entering the skeleton's (non-existent) belly, the potion or food itself will fall through the skeleton's jaws, giving no nutritional benefit.  They can absorb mana from staves and wands as their energy source.",

"Much like Skeletons, Zombies too are undead horrors: they are resistant to life-draining attacks, and can learn to restore their life-force.  Like skeletons, they become resistant to cold-based attacks (actually earlier than skeletons), resist poison and can see invisible.  While still vulnerable to cuts (unlike skeletons), Zombies are resistant to Nether.  Like Golems, they gain very little nutrition from the food of mortals, but can absorb mana from staves and wands as their energy source.",

"One of the mightier undead creatures, the Vampire is an awe-inspiring sight.  Yet this dread creature has a serious weakness: the bright rays of sun are its bane, and it will need to flee the surface to the deep recesses of earth until the sun finally sets.  Darkness, on the other hand, only makes the Vampire stronger.  As undead, the Vampire has a firm hold on its life force, and resists nether attacks.  The Vampire also resists cold and poison based attacks.  It is, however, susceptible to its perpetual hunger for fresh blood, which can only be satiated by sucking the blood from a nearby monster.",

"Another powerful undead creature: the Spectre is a ghastly apparition, surrounded by an unearthly green glow.  They exist only partially on our plane of existence: half-corporeal, they can pass through walls, although the density of the wall will hurt them in the process of doing this.  As undead, they have a firm hold on their life force, see invisible, and resist poison and cold.  They also resist nether.  At higher levels they develop telepathic abilities.  Spectres make superb spellcasters, but their physical form is very weak.  They gain very little nutrition from the food of mortals, but can absorb mana from staves and wands as their energy source.",

"One of the several fairy races, Sprites are very small.  They have tiny wings and can fly over traps that may open up beneath them.  They enjoy sunlight intensely, and need worry little about light based attacks.  Although physically among the weakest races, Sprites are very talented in magic, and can become highly skilled wizards.  Sprites have the special power of spraying Sleeping Dust, and at higher levels they learn to fly faster.",

 "This race is a blasphemous abomination produced by Chaos.  It is not an independent race but rather a humanoid creature, most often a human, twisted by the Chaos, or a nightmarish crossbreed of a human and a beast.  All Beastmen are accustomed to Chaos so much that they are untroubled by confusion and sound, although raw logrus can still have effects on them.  Beastmen revel in chaos, as it twists them more and more.  Beastmen are subject to mutations: when they have been created, they receive a random mutation.  After that, every time they advance a level they have a small chance of gaining yet another mutation.",

"The Ents are a powerful race dating from the beginning of the world, oldest of all animals or plants who inhabit Arda.  Spirits of the land, they were summoned to guard the forests of Middle-earth.  Being much like trees they are very clumsy but strong, and very susceptible to fire.  They gain very little nutrition from the food of mortals, but they can absorb water from potions as their nutrition.",

"Archons are a higher class of angels.  They are good at all skills, and are strong, wise, and are a favorite with any people.  They are able to see the unseen, and their wings allow them to safely fly over traps and other dangerous places.  However, belonging to a higher plane as they do, the experiences of this world do not leave a strong impression on them and they gain levels slowly.",

"Balrogs are a higher class of demons.  They are strong, intelligent and tough.  They do not believe in gods, and are not suitable for priest at all.  Balrog are resistant to fire and nether, and have a firm hold on their life force.  They also eventually learn to see invisible things.  They are good at almost all skills except stealth.  They gain very little nutrition from the food of mortals, and need human corpses as sacrifices to regain their vitality.",

"Dunedain are a race of hardy men from the West.  This elder race surpasses human abilities in every field, especially constitution.  However, being men of the world, very little is new to them, and levels are very hard for them to gain.  Their constitution cannot be reduced. ",

"Shadow Fairies are one of the several fairy races.  They have wings, and can fly over traps that may open up beneath them.  Shadow Fairies must beware of sunlight, as they are vulnerable to bright light.  They are physically weak, but have advantages in using magic and are amazingly stealthy.  Shadow Fairies have a wonderful advantage in that they never aggravate monsters (If their equipment normally aggravates monsters, they only suffer a penalty to stealth, but if they aggravate by their personality itself, the advantage will be lost).",

"A Kutar is an expressionless animal-like living creature.  The word 'kuta' means 'absentmindedly' or 'vacantly'.  Their absentmindedness hurts their searching and perception skills, but renders them incapable of being confused.  Their unearthly calmness and serenity make them among the most stealthy of any race.  Kutars, although expressionless, are beautiful and so have a high charisma.  Members of this race can learn to expand their body horizontally.  This increases armour class, but renders them vulnerable to magical attacks.",

"An android is a artificial creation with a body of machinery.  They are poor at spell casting, but they make excellent warriors.  They don't acquire experience like other races, but rather gain in power as they attach new equipment to their frame.  Rings, amulets, and lights do not influence growth.  Androids are resistant to poison, can move freely, and are immune to life-draining attacks.  Moreover, because of their hard metallic bodies, they get a bonus to AC.  Androids have electronic circuits throughout their body and must beware of electric shocks.  They gain very little nutrition from the food of mortals, but they can use flasks of oil as their energy source."

#endif
};

static cptr class_jouhou[MAX_CLASS] =
{
#ifdef JP
"���Ȃ��͕���̈����ɏn�B���A���|�I�ȍ��r�œ���؂�J�����Ƃ��ł��܂��B���ډ���a�邾�����\�ł͂Ȃ��|��Ⓤ���̋Z�p���K�����Ă��܂��B1��1�̐킢�œG�Ɍ�����鎖�͂قƂ�ǂ���܂��񂪁A�퓬�ȊO�̂��Ƃ͋ɂ߂ĕs����ł��B���Ոȍ~�̃_���W�����Ő������т邽�߂ɂ́A���Ȗ������B���\�͂Ȃǂ��ǂ��ɂ����ĕ₤�����K�v�ł��B",
  
"���Ȃ��͒m����͂Ƃ��铹��I�сA���@�̉��`�����߂邽�߂Ɍ����Ǝ��H����X�ςݏd�˂Ă��܂��B���Ղ͕n��Ȑg�̔\�͂̂��߂ɋ�킵�܂����A�I�Ղ͋��͂����l�Ȑ킢�����\�ɂȂ邱�Ƃł��傤�B�@�m�ɕx��ł���A�T���▂����̎g�p�����ӂł��B���x�����オ��Ɩ�����̖��͂��z�����邱�Ƃ��ł���悤�ɂȂ�܂��B�S12�̈�̖��@�̒�����2�̈�̖��@���K���ł��܂��B���̈�̕��ނɂ��A���p�t�A�d�p�t�A���t�ƌĂѕ����ς��܂��B���̈�͂��ł��ύX�\�ł����A���̈悪���̈�ƕʂ̕��ނ̏ꍇ��Փx���啝�ɏ㏸���Ă��܂��܂��B",
  
"���Ȃ��͐_�Ɏd���ċV����F�����s�����Ƃ𐶋ƂƂ��Ă��܂��B�_�̏����𓾂Đ�p�ɑ����閂�@��2�̈�s�g�\�ł��B�M�̎����ɂ�薂�@�h��͂ɗD��A���x�����オ��Ƌ��|�Ȃǂɑ΂���ϐ��𓾂܂��B���X�̏C�s�̈�Ƃ��Ă�����x�̕��|�����Ȃ��܂����A����قǂ̒B�l�ɂ͂Ȃ�܂���B�܂��A�݊�Ɩ_�ȊO�̏j������Ă��Ȃ�����𑕔�����Ɛ_�̕s���𔃂����@�̍s�g�Ȃǂɍ�����肪�o�܂��B�����͂��Ȃ�x�߂ł����A�_�ɃA�C�e��������ċF�邱�Ƃɂ��􋖂��̌o���l�𓾂邱�Ƃ��ł��܂��B�Ȃ��A�����������炠�Ȃ��͎א_���M��_�ƌĂ΂��_�Ɏd���Ă���̂�������܂���B���̏ꍇ�͐�p�łȂ��d�p�ɑ����閂�@���K�����A�j�����ꂽ����𑕔�����ƃy�i���e�B���󂯂܂��B",
  
"���Ȃ��͕󕨂����߂ĉ����̐��Ƃ��Ղ�T������A������g���W���[�n���^�[�ł��B�B��������Ⓤ���ȂǗl�X�ȋZ�\�ɗD��A�T���Ɍ������֗��ȓ��Z�������܂��B�E�ƕ����΂��Ή������Ղ̔Ԑl�Ɛ퓬�ɂȂ邽�ߔ�����������������Ȃ��܂����A����m�قǂ̘r���߂̋����͂���܂���B���@����̈�K���\�ł������̊w�K�͒x���A���܂苭�͂Ȗ��@�͎g���܂���B�Z���̈��������ӂŒZ���𑕔����Ă���ƍU���񐔂��㏸���܂��B�Z���̂ق��ɂ̓N���X�{�E�̈��������ӂł��B����ȊO�̑傫�ȕ���̈����͋��ł��B",
  
"���Ȃ��͐X�̉��[���ŏb�̂悤�Ɏ������ĕ�炵�Ă��܂��B�ߍ��Ȋ��ň�������Ȃ��͔�����Ǝˌ��̗����ɏG�łĂ��܂��B�܂���B����`����ꂽ���R�̗͂��g����p���C�߂Ă��莩�R���ϗe�̖��@���s�g�\�ł��B��������Ɩ�Ⴢ�ł̑ϐ��𓾂āA����Ƀ����X�^�[�̏ڍׂȏ��𒲍�������Z���g����悤�ɂȂ�܂��B�B���s���͓��ӂł��������̑@�ׂȋZ�p�ɂ��Ă͂��s����ł��B",
  
"���Ȃ��͐������邽�߂ɐ키�����Ȑ�m�ł��B���������\�͂������A���Ɍ��Ƒ��̈����𓾈ӂƂ��܂��B���@�͎g���܂��񂪊O�G����g����邽�߂̂������̋Z���K���ł��܂��B�j�����ꂽ����𑕔�����ƍU���͂��������܂����A���ꂽ����𑕔�����ƍU���͂��啝�ɒቺ���܂��B�R��Z�\�̓K��������܂������ׂ̐��̖͂��b�𑀂邱�Ƃ͑�̋��ł��B���x�����㏸����Ƌ��|�Ɣj�׍U���ɑ΂���ϐ��𓾂܂��B",
  
"���Ȃ��͗l�X�Ȏ����ɑ΂���[���m�����C�߁A�������i�ɓ`���邱�Ƃ��g���Ƃ��Ă��܂��B���@��2�̈�K���\�ŁA�݂̂Ȃ炸�����퓬�Z�p�ɂ��Ă����L���K���������\�E�ł��B���������̔\�͂ɂ����@�̈����ɂ��ˏo�����Ƃ��낪�Ȃ��A�Z�\�̐����ɂ����܂�L�т��낪�Ȃ���p�n�R�ł�����܂��B���Ոȍ~�̃_���W�����Ő����c�邽�߂ɂ͎�ɓ�����̂�S�Ďg�����Ȃ��_������߂��܂��B",
  
"���Ȃ��͎�Ɏd���ē��X�̎G�����s���A�N���҂�����Ό}�����ꖽ����ΓG�n�����P���闊���]�҂ł��B�l�X�ȕ��������p�Ɉ����T���Ȃǂ̋Z�\����⍂���A�������֗̕��ȏ��Z���K�����܂��B����ɖ��@�E�قǂł͂Ȃ����̖̂��@��1�̈�s�g�\�Ȗ��\�^�E�Ƃł��B���������C�h�i�����j������̏�Ƀ��C�h���i�X�[�c�j�𒅗p���Ȃ���΂Ȃ�܂���B�D�G�Ȗh��̊m�ۂɂ͋�J���邱�Ƃł��傤�B���x���A�b�v�̂Ƃ��傩��J�����󂯎��܂��B��������̋@���������Ɨ��s�s�Ȕ����󂯂邱�Ƃ�����܂��B",
  
"���Ȃ��͕���ɗ��炸�g��Ő킢�������Ƃ��D�݁A���X���̂ƋZ�̒b�B�ɗ��ł��܂��B���̒b�B�̐��ʂ͌����Ȃ��̂ŁA�f��ł���ɂ��ւ�炸�d�����̐�m�ɕC�G����U���͂�@���o���܂��B�܂��������C�s�ɂ��u�C�v���������@���K�����Ă���A�l�X�ȏ�l���ꂵ���Z���g�����Ƃ��ł��܂��B����������d���Z�𑕔�����Ɗi���ɍ�����肪�o�邽�߁A�����𐮂���̂ɂ͋�J���邱�Ƃł��傤�B㩉����▂����Ƃ������Z�p�͂��܂蓾�ӂł͂���܂���B",
  
"���Ȃ��͂����钴�\�͎҂ŁA���_�̗͂ŗl�X�Ȋ�ՓI�Ȃ��Ƃ𐬂������܂��B���̔\�͕͂��L���A�Ƃ��ɖh��Ɗ��m�Ɋւ��Ă͐����Ȗ��@�����|���܂��B���\�͂̏K����s�g�ɂ͒m�\��v�����A���x���ƌ����݂̂��v������܂�(���\�͖͂��@�Ɠ����R�}���h����g�p���܂�)�B�̂͂��ؚ��ł������e��������������Ȃ��A�����₷�����\�^�ƌ�����ł��傤�B���x�����オ��ƍ����ϐ���e���p�V�[�̔\�͂��l�����܂��B�����͑��߂ł����X�R�A�͒�߂ł��B",
  
"���Ȃ��͒m����͂Ƃ��铹��I�сA�����Ă�����̈�̖��@���ɂ߂邱�Ƃ����S���܂����B�K���ł��閂�@�͈�̈悾���ł�����̂́A���̂ǂ̐E�����I�݂ɂ��̗̈�̖��@�������܂��B�ʏ�̖��@�E�������Ȃ�MP�������A��背�x���ɒB����ƑI�������̈�ɉ������Ɠ��̔\�͂𓾂邱�Ƃ��ł��܂��B�n�C���C�W�̌ď̂��܂����C�W�̂悤�ɗ̈悲�Ƃɕω����܂��B",
  
"���Ȃ��͌��z���ɕ�炷���ʂ̏Z�l�ł��B�l���̓�������������܂��񂵁A�Ԕ��ŗV�ԗd����������܂���B���炩�̎���Ń_���W�����T���ɐg�𓊂��܂������퓬�Ɋւ��Ă͂قƂ�ǉ��̎�蕿������܂���B1�̈�̂ݖ��@�̏K�����\�ł������͂Ȗ��@�͂��Ȃ��̎�ɂ͗]��܂��B���Ȃ��̖`���͋��ɖ��������̂ɂȂ�ł��傤�B�Ȃ��A�Q�[���J�n���ɐE�Ɩ����D���Ȃ悤�ɍĐݒ�\�ł��B",
  
"���Ȃ��͎R���Ă���A���̂Ɛ��_�̒b�B�̂��߂ɏC�s�ɖ������Ă��܂��B���Ɩ_�̈����ɏG�ŁA�f��ł̊i�������ӂł��B�_�𑕔����Ă���Ƃ��ɂ͊i���̒ǉ��U�����o�₷���Ȃ�܂��B���X�̏C�s�̎����Ő�p�ɑ����閂�@����̈�s�g�\�ł����A�Ђ���Ƃ���ƊO�@�Ɏ���o���ĕϗe�⎀��̗̈���K�����Ă��邩������܂���B���x�����オ��Ƌ��|�⍬���ւ̑ϐ���g�ɂ��A����ɐ��_���W�����đf����MP���񕜂��邱�Ƃ��ł���悤�ɂȂ�܂��B",
  
"���Ȃ��͕��X�Ȃ�ʓw�͂ɂ���Č��Ɩ��@�̗������K��������m�ł��B�����\�͂͏���m�ɉ����y�΂����@�̏K�n�������@�E�ɐ�������܂����A���ɖ��@�̌��ʂ��悹�ē���ȍU����������Z�������܂��B���p�����邢�͈Í��E���ׂ̗d�p�����̈�̖��@���K���ł��A���@���̌��ʂ͏K���������@�̈�ŕς��܂��B���̈����ɂ͋ɂ߂ďG�ł܂����قƂ�ǌ����������܂���B���x�����オ��Ɠ������F�Ƌ}�񕜂̔\�͂��l�����܂��B�Ȃ��A�����͂��x�߂ł��B",
  
"���Ȃ��͌��z���̒e�������ɖ������A������e��������Č����悤�Ɠ��錤�����d�˂Ă���ς��҂̃��C�W�ł��B�����X�^�[�̓��Z���ώ@���A���̏K���x�ɒB�������Z���g�p���邱�Ƃ��ł��܂��B�K���̂��₷���͓��Z�̃��x���Ɠ�Փx�A�����̃��x���ƒm�\�A�����X�^�[�Ƃ̋����ŕς��܂��B�Ȃ��u���Z�̊ώ@�v�Ƃ͑��̏ꍇ���̍U��������󂯂邱�Ƃ��Ӗ����܂��B�ŏI�I�ɂ͐��܂��������ɂȂ��\��������܂����A�����܂ł̓��̂�͋�J�Ɗ댯�̘A���ɂȂ邩������܂���B",
  
"���Ȃ��͋|��N���X�{�E�̈����ɓ���������m�ł��B���̎ˌ��̎���͐��܂������̂ŁA��l�̐��{�̑��x�Ő��m����Ȏˌ����s�����Ƃ��ł��܂��B�������ڋߐ�ł̋����͑��̐�m�n�E�Ƃɑ傫�����܂��B���Ȃ��͎�悪��p�ł��̕ӂ�ɐ����Ă���؂�������邱�Ƃ��ł��A�A�[�`���[�M���h�ɖ�Ƒf�ނ���������ŉ��H���邱�Ƃł���ɋ��͂Ȗ����邱�Ƃ��ł��܂��B�B����T���Ȃǂ��̑��̋Z�\�͊T�˕��ϓI�ȃ��x���ł��B���x���A�b�v�ɂ�蓧�����F��Ӗڑϐ��̔\�͂𓾂܂��B",
  
"���Ȃ��͓���ȏp�t�ł��B���@����S���ǂނ��Ƃ��ł��܂��񂪁A���̑���Ƀ}�W�b�N�A�C�e���̈����ɂ����Ă͒N�����D��Ă��܂��B��A���@�_�A���b�h�Ƃ������������̂Ɏ�荞��Ŏg�p����\�͂������Ă���A��荞�񂾖�����͎��Ԍo�߂Ŏg�p�񐔂��񕜂��܂��B�܂��[�U�┭���Ɋւ��邢�����̓��Z���K�����܂��B���e��͂���قǓ��ӂł͂Ȃ����߁A���͂Ȗ������ɓ���܂ł͐������L�єY�ނ�������܂���B",
  
"���Ȃ��͋@�B�Z�t�ŁA���G����ȋ@�B�𑀂��Đ�ǂ�؂�J���܂��B�@�B�̍쐬��e��R���̒��B�̓G���W�j�A�M���h�ōs���܂��B���Ȃ��͔�s�\�ȃo�b�N�p�b�N�������Ă��肻�̒��ɋ@�B���i�[���Ďg�p���܂��B�@�B�̎�ނ͑������ߊi�[����@�B�͋ᖡ����K�v������܂��B�@�B�͋ɂ߂ċ��͂ł����A��������ɂ��Ƃɂ���������������Ƃ����傫�Ȍ��_������܂��B�@�B�̍쐬�ɂ͒m�\�Ɗ�p�����v������܂����A�����K�v�Ȃ̂͐험�i�����������ċ�������邽�߂̖��͂�������܂���B�����̋@�B�͐��Ɠd������G�ŁA�����̍U�����󂯂�ƈꎞ�I�Ɏg�p�s�\�ɂȂ邱�Ƃ�����܂��B���Ȃ����g�̐퓬�\�͕͂��}�ł��B㩉����ƃN���X�{�E�����ӂł���������̎g�p�͑�̋��ł��B",
  
"���Ȃ��͐}���قɋΖ����Ă��܂��B�}���ق̎�Ɏd���ĎG�������Ȃ��{�D�_���}������̂��d���ł��B�����ɂ��čL���m���������Ă���A�����ׂ����ƂɑS�Ă̗̈�̓���ڂ܂ł̖��@���ŏ�����L�����Ă��܂��B���x��������Ă��Ė{�������Ă���Ζ��@���g�p�\�ł��B���������@�̏K�n�x�͖{�i�I�Ȗ��@�E�ɂ͉����y�΂��A�O���ڈȍ~�̋��͂Ȗ��@���͈�؎g���܂���B���p�ɂ��Ă�����Ȃ�ɐS���͂���܂�����������{�E�ɂ͈�A�������A���܂�傫�ȕ����U��񂷂̂͋��ł��B�`���̏��Ղ͉��s���R�Ȃ����Ȃ��܂����A�I�Ղŋ��G�𑊎�ɂ���̂͋�J���邩������܂���B���x�������Ȃ�オ���MP�𑽂߂ɏ���Ď����������r��������Z��g�ɂ��܂��B",
  
"���Ȃ��͌��̓����ɂ߂邽�߂ɓ��X�Z�𖁂��Ă��܂��B���̈����ɂ����Ă͉E�ɏo����̂͂��܂���B���@�͎g���܂��񂪁A��l�����̎c������`����ǂނ��ƂŁu�K�E���v���K�����邱�Ƃ��\�ł��B�܂��u�^�v�Ƃ�������ȍ\������邱�Ƃɂ��ꕗ�ς�����킢�������邱�Ƃ��ł��܂��B�񓁗��ɂ������K���������܂����A�T���▂����Ȃǂ͋��ł��B",
  
"���Ȃ��͏e�Ί�ŕ���������m�ł��B	�e�ƃi�C�t�̈����ɗD��A�i���E�J���E�B���ɂ��K��������܂��B����Ŗ�����̈����Ɩ��@�h��͂��Ȃ���ł��B��背�x���ɂȂ邲�ƂɋZ�\�K���|�C���g�𓾂āA�|�C���g������Ċ���̃J�e�S������Z�\���w�Ԃ��Ƃ��ł��܂��B�|�C���g�̊���U��ɂ��_���W�����U���̐헪��������x�ς��܂��B���Ȃ��͌��z���̏Z�l���猩��Ɩ��炩�Ɉَ��ȑ��������Ă��邽�߁A�X�ł͌x������Ă��܂���������܂���B",
  
"���Ȃ��͖������Ĕ��邱�ƂŐ��v�𗧂ĂĂ��܂��B�_���W�����Ȃǂőf�ނƒ��܃��V�s����肵�A�X�̒��܎����؂�Ė�𒲍����܂��B�퓬�͂��܂蓾�ӂłȂ����߁A���G�Ɛ키�Ƃ��͔���⋭����Ȃǂ����O�ɒ������Ă����K�v������ł��傤�B���Ȃ��̓U�b�N�Ƃ͕ʂɑf�ނ�������򔠂���Ɏ��������Ă��܂��B�򔠂̒��̃A�C�e���͌��d�ɕی삳��Ă���G�̍U���Ŕj�󂳂�܂��񂪁A��x�򔠂���o���Ȃ��Ǝg�p�ł��܂���B���x�����オ��ƃU�b�N�̒��̖򂪗�C�U���ȂǂŔj�󂳂�ɂ����Ȃ�܂��B��̍����ɂ͒m�\���K�v�ŁA�ޗ��̍̎�̂��߂ɂ͉B���\�͂�T���\�́A������x�̐퓬�̏������K�v�ł��B",
  
"���Ȃ��͏�n�ɏn�B�����R�m�ł��B���̏�n�̋Z�p�͌����Ȃ��̂ŁA�n�ɗ��܂炸�����⋐�b�A�ʂĂ͗��܂ŏ�肱�Ȃ��܂��B���͂ȃ����X�^�[�ɏ���΋ɂ߂ĐS�����x���������邱�Ƃł��傤�B���Ⓑ������̈��������ӂŋ|�̈������I�݂ł��B�܂���n�Ɍ����Ȃ�����ł�����Ȃ�Ɉ����܂��B��������m�Ɠ��l�ɖ��@�͈�؎g����������̈��������ł��B",
  
"���Ȃ��͓���ƕt�r�_�̐��ƂŁA�����t�r�_�ɕω����������ȋZ�������Ă��܂��B�t�r�_�ƂȂ�������͒��ɕ����A���Ȃ��̂����ׂƂȂ��Đ킢�܂��B�ʏ�̔z���ƈႢ�A�t�r�_�̍U���ł��Ȃ����g���o���l�ƃA�C�e���𓾂邱�Ƃ��ł��܂��B���Ȃ��̑������Ă��镐��͗d�퉻���čő���V���Ȕ\�͂��l�����邱�Ƃ�����܂��B���Ȃ��͊Ӓ��㩉����▂����g�p�ɏG�łĂ���A���ꂽ�����i�����􂹂��ɊO���܂��B���������Ȃ����g�̐퓬�\�͂͂���قǍ�������܂���B�_���W�����̐[�w�ł͒n�`�⑊���I�ԐT�d�ȗ�����肪�v������܂��B",

"���Ȃ��͏E�������������������G���ȕ��𔄂��ĕ�炵�Ă��܂��B���̖ڗ����͊m���ŁA�Ӓ�Ɋւ��Ă͂ǂ̐E�Ƃ����D��Ă��܂��B�������T���̋Z�\�ɗD��A�d���ו����^�Ԃ��Ƃ��ł��A�X�ł̌������ӂł��B�܂����Ȃ��ɂ͖��@�̑����i�����ς���Z�p������A�M���h�őf�ނƂȂ�A�C�e������\�͂𒊏o���ʂ̕���ɕt�����邱�Ƃ��ł��܂��B���������Ȃ��̊�{�I�Ȑ퓬�\�͂͂���قǍ������̂ł͂Ȃ��A���@����؎g�p�ł��܂���B�_���W�����̉��ɒH�蒅���ɂ͍��C�����f�ނ��W�߂đ�����b���A��͂̒�グ��}��K�v������܂��B���Ȃ��͖`���p�̃U�b�N�ɉ����ďE�������̂��^�Ԃ��߂̓���������Ă��܂��B",

"���Ȃ��͖��@�̑����i��������΍׍H�t�ł��B��΂⑕���i���疂�͂𒊏o���A���̑����i�ɓ��ʂȕ�����{�����ƂŖ��͂�t�^����Z�p�������Ă��܂��B��Ƃ͊X�̕���M���h�ōs���܂��B���Ȃ��͎�悪��p�Ŗ��@�̓���̈����ɗD��܂����A�ߐڐ퓬�\�͈͂�ʐl�ƕς��܂���B�܂����Ȃ��͖��@�g���ł����薂�@����̈�K���\�ł��������ȃ��C�W�ɔ�ׂ�Ƃ��f���ɗ��܂��B�C�����閂�@�̈�͐T�d�ɑI������K�v������ł��傤�B���x�����オ��Ǝ莝���̕�΂��疂�͂��z������MP���񕜂�����Z���K�����܂��B",

"���Ȃ��͓���ȌP����ς񂾋���ׂ��ÎE�҂ł��B�Èł̒��ł����͂����ʂ����Ƃ��ł��A�܂��Èł̒��ł͓G�Ɍ����炸�ɍs���ł��܂��B�Z����f��̈ꌂ�œG�𖳗͉����A���������Z�\�������A����ɂ͔E�p�ƌĂ΂�����Z�\���g�����Ȃ��܂��B�U���ɂ����Ă͂��̋������⊶�Ȃ��������܂����A����Ŗh��ʂɂ͂��s��������܂��B�ϐ���AC�ɂ͗D��Ă�����̗̂̑͂Ɍ����A����d������h�����ؑ����ł��܂���B�E�p�ɂ��h���������O��̂��郊�X�L�[�Ȃ��̂ł��B�E�p�̎g�p�ɂ͎�Ɋ�p�����邢�͒m�\���e�����܂��B",

"���Ȃ��͎��̑��������l���邢�͓��@�Ƃł��B�A�r���e�B�J�[�h�����ď����̗��s���m�M���A�J�[�h�𔃂���߂Ĉ�ׂ����悤�Ɗ��ł��܂��B�������������Ƃ��l����҂����X�Ɍ���A���i�̍����͕K���ł��B�Ƃɂ����}���ł����𒙂߁A���_�Ђ̋߂��̃J�[�h�̔����A�U�V�I�̓q��A���邢�̓J�[�h�����l�d����J�[�h�𔃂�����܂��傤�B���Ȃ��ɂ͖{�E�ɉ����y�΂Ȃ��܂ł������̋ߐڐ�▂�@�̐S��������܂��B�܂��J�[�h�P�[�X�������Ă��肱�̒��ɃJ�[�h��8��ނ܂Ŋi�[���邱�Ƃ��ł��܂��B���Ȃ��ɕK�v�Ȕ\�͂̓J�[�h�𔭓����邽�߂̖�����Z�\�A�A�r���e�B�J�[�h�̈З͂����߂�������悭������s�����߂̖��͂ł��B",

"���Ȃ��͋C���t���Ɖ����Ƃ��m��ʑ����ɗ����Ă��܂����B�l�̎�̓����Ă��Ȃ����R�̕��i�ɐS�����̂����̊ԁA���̏ꏊ�͂���܂ł̐l���Ŕ|���Ă����펯���S���ʂ��Ȃ����ƁA������舕����Ă���댯�ɖ����Ă��邱�ƁA�����Ď������S�����͂ł��邱�Ƃ�m��܂��B���Ȃ��͍K�^�ɂ��l�̏Z�ޏꏊ�ɒH�蒅���܂����B�K�����t�͒ʂ���悤�ł��B���ł͂��Ȃ��̂悤�Ȑl�����邱�Ƃ͏��߂Ăł͂Ȃ��炵���A�e�؂Ȑl������_�Ђɍs���ΊO�֑���Ԃ��Ă����Ƌ����Ă���܂����B�������_�Ђւ̓����͗d���̑��A�������ł��B�����ٕ̈ς��N�����Ă���炵�����͂�����Ă��܂����A���Ȃ��ɂƂ��Ă͉����ٕςŉ������핗�i�Ȃ̂�����肩�ł͂���܂���E�E",
"","",//���j�[�N�N���X�̐�������unique_player_table[]
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","","","","","","",
"","","","","",//class155
"���Ȃ��͊댯�Ȗ��@�̐X�ɋ����\����ς��҂ł��B�ߏ��̖��@�g�����X�̖I����ĂĖI�����W�߂Ă���̂����Đ^�����Ă݂邱�Ƃɂ��܂����B�X�̖��͂ŕώ������I�����͂��Ȃ��Ƃ����炩�̈ӎv�a�ʂ��ł��A�I�����W�߂�ȊO�ɂ��F�X�Ə����ɂȂ��Ă���܂��B���Ȃ��ɂ͕���Ɩ��@�̐S������������܂����ǂ�����{�E�ɂ͉����y�т܂���B�I���������܂��g���Đ������т܂��傤�B�������I�ւ̖��߂Ɏ��s����ƖI�͓{���čU�����Ă��邩������܂���B���Ȃ��ɕK�v�Ȕ\�͂͒m�\�Ɩ��͂ł��B",//156:�{�I��
#else

"A Warrior is a hack-and-slash character, who solves most of his problems by cutting them to pieces, but will occasionally fall back on the help of a magical device.  Unfortunately, many high-level devices may be forever beyond their use.",

"A Mage is a spell caster that must live by his wits as he cannot hope to simply hack his way through the dungeon like a warrior.  In addition to his spellbooks, a mage should carry a range of magical devices to help him in his endeavors which he can master far more easily than anyone else.  A mage's prime statistic is Intelligence as this determines his spell casting ability. ",

"A Priest is a character devoted to serving a higher power.  They explore the dungeon in the service of their God.  Since Priests receive new prayers as gifts from their patron deity, they cannot choose which ones they will learn.  Priests are familiar with magical devices which they believe act as foci for divine intervention in the natural order of things.  A priest wielding an edged weapon will be so uncomfortable with it that his fighting ability.  A Priest's primary stat is Wisdom since this determine his success at praying to his deity. ",

"A Rogue is a character that prefers to live by his cunning, but is capable of fighting his way out of a tight spot.  Rogues are good at locating hidden traps and doors and are the masters of disarming traps and picking locks.  A rogue has a high stealth allowing him to sneak around many creatures without having to fight, or to get in a telling first blow.  A rogue may also backstab a fleeing monster.  Intelligence determines a Rogue's spell casting ability.",

"A Ranger is a combination of a warrior and a mage who has developed a special affinity for the natural world around him.  He is a good fighter and also good about a missile weapon such as a bow.  A ranger has a good stealth, good perception, good searching, a good saving throw and is good with magical devices.  Intelligence determines a Ranger's spell casting ability.",

"A Paladin is a combination of a warrior and a priest.  Paladins are very good fighters, but not very good at missile weapons.  A paladin lacks much in the way of abilities.  He is poor at stealth, perception, searching, and magical devices but has a decent saving throw due to his divine alliance.  Wisdom determines a Paladin's success at praying to his deity.",

"A Warrior-Mage is precisely what the name suggests: a cross between the warrior and mage classes.  While their brothers, the rangers, specialize in Nature magic and survival skills, true Warrior-Mages attempt to reach the best of both worlds.  As warriors they are much superior to the usual Mage class.  Intelligence determines a Warrior-Mage's spell casting ability.",

"Chaos Warriors are the feared servants of the terrible Demon Lords of Chaos.  Every Chaos Warrior has a Patron Demon and, when gaining a level, may receive a reward from his Patron.  He might be healed or polymorphed, his stats could be increased, or he might be rewarded with an awesome weapon.  On the other hand, the Patrons might surround him with monsters, drain his stats or wreck his equipment or they might simply ignore him.  The Demon Lords of Chaos are chaotic and unpredictable indeed.  The exact type of reward depends on both the Patron Demon (different Demons give different rewards) and chance.",

"The Monk character class is very different from all other classes.  Their training in martial arts makes them much more powerful with no armor or weapons.  To gain the resistances necessary for survival a monk may need to wear some kind of armor, but if the armor he wears is too heavy, it will severely disturb his martial arts maneuvers.  As the monk advances levels, new, powerful forms of attack become available.  Their defensive capabilities increase likewise, but if armour is being worn, this effect decreases.  Wisdom determines a Monk's spell casting ability.",

"The Mindcrafter is a unique class that uses the powers of the mind instead of magic.  These powers are unique to Mindcrafters, and vary from simple extrasensory powers to mental domination of others.  Since these powers are developed by the practice of certain disciplines, a Mindcrafter requires no spellbooks to use them.  The available powers are simply determined by the character's level.  Wisdom determines a Mindcrafter's ability to use mind powers.",

"High-mages are mages who specialize in one particular field of magic and learn it very well - much better than the ordinary mage.  For the price of giving up a second realm of magic, they gain substantial benefits in the mana costs, minimum levels, and failure rates in the spells of the realm of their specialty.  A high mage's prime statistic is intelligence as this determines his spell casting ability. ",

"Tourists have visited this world for the purpose of sightseeing.  Their fighting skills is bad, and they cannot cast powerful spells.  They are the most difficult class to win the game with.  Intelligence determines a tourist's spell casting ability.",

"Imitators have enough fighting skills to survive, but rely on their ability to imitate monster spells.  When monsters in line of sight use spells, they are added to a temporary spell list which the imitator can choose among.  Spells should be imitated quickly, because timing and situation are everything.  An imitator can only repeat a spell once each time he observes it.  Dexterity determines general imitation ability, but a stat related to the specific action is often also taken into account.",

"Beastmasters are in tune with the minds of the creatures of the world of Hengband.  They are very good at riding, and have enough fighting ability.  They use monsters which summoned or dominated by him as his hands and feet.  Beastmasters can cast trump magic, and very good at summoning spell, but they can not summon non-living creatures.  Charisma determines a Beastmaster's spell casting ability.",

"Sorcerers are the all-around best magicians, being able to cast any spell from most magic realms without having to learn it.  On the downside, they are the worst fighters in the dungeon, being unable to use any weapon but a Wizardstaff.",

"Archers are to bows what warriors are to melee.  They are the best class around with any bow, crossbow, or sling.  They need a lot of ammunition, but will learn how to make it from junk found in the dungeon.  An archer is better than a warrior at stealth, perception, searching and magical devices.",

"Magic-Eaters can absorb the energy of wands, staffs, and rods, and can then use these magics as if they were carrying all of these absorbed devices.  They are middling-poor at fighting.  A Magic-Eater's prime statistic is intelligence.",

"Bards are something like traditional musicians.  Their magical attacks are sound-based, and last as long as the Bard has mana.  Although a bard cannot sing two or more songs at the same time, he or she does have the advantage that many songs affect all areas in sight.  A bard's prime statistic is charisma.",

"Red-Mages can use almost all spells from lower rank spellbooks of most realms without having to learn it.  At higher level, they develop the powerful ability \"Double Magic\".  However, they have large penalties in the mana costs, minimum levels, and failure rates of spells, and they cannot use any spells from higher rank spellbooks.  They are not bad at using magical devices and magic resistance, and are decent fighter, but are bad at other skills.  A red-mage's prime statistic is intelligence.",

"Samurai, masters of the art of the blade, are the next strongest fighters after Warriors.  Their spellpoints do not depend on level, but depend solely on wisdom, and they can use the technique Concentration to temporarily increase SP beyond its usual maximum value.  Samurai are not good at most other skills, and many magical devices may be too difficult for them to use.  Wisdom determines a Samurai's ability to use the special combat techniques available to him.",

"A ForceTrainer is a master of the spiritual Force.  They prefer fighting with neither weapon nor armor.  They are not as good fighters as are Monks, but they can use both magic and the spiritual Force.  Wielding weapons or wearing heavy armor disturbs use of the Force.  Wisdom is a ForceTrainer's primary stat.",

"A Blue-Mage is a spell caster that must live by his wits, as he cannot hope to simply hack his way through the dungeon like a warrior.  A major difference between the Mage and the Blue-Mage is the method of learning spells: Blue-Mages may learn spells from monsters by activating his Learning ability.  A Blue-Mage's prime statistic is Intelligence as this determines his spell casting ability. ",

"Cavalry ride on horses into battle.  Although they cannot cast spells, they are proud of their overwhelming offensive strength on horseback.  They are good at shooting.  At high levels, they learn to forcibly saddle and tame wild monsters.  Since they take pride in the body and the soul, they don't use magical devices well.",

"A Berserker is a fearful fighter indeed, immune to fear and paralysis.  At high levels, Berserkers can reflect bolt spells with their tough flesh.  Furthermore, they can fight without weapons, can remove cursed equipment by force, and can even use their special combat techniques when surrounded by an anti-magic barrier.  Berserkers, however, cannot use any magical devices or read any scrolls, and are hopeless at all non-combat skills.  Since Berserker Spectres are quite easy to *win* with, their scores are lowered.",

"A Weaponsmith can improve weapons and armors for him or herself.  They can extract the essences of special effects from weapons or armors which have various special abilities, and can add these essences to another weapon or armor.  They are good at fighting, but cannot cast spells, and are poor at skills such as stealth or magic defense.",

"Mirror-Masters are spell casters; like other mages, they must live by their wits.  They can create magical mirrors, and employ them in the casting of Mirror-Magic spells.  A Mirror-Master standing on a mirror has greater ability and, for example, can perform quick teleports.  The maximum number of Magical Mirrors which can be controlled simultaneously depends on the level.  Intelligence determines a Mirror-Master's spell casting ability.",

"A Ninja is a fearful assassin lurking in darkness.  He or she can navigate effectively with no light source, catch enemies unawares, and kill with a single blow.  Ninjas can use Ninjutsu, and are good at locating hidden traps and doors, disarming traps and picking locks.  Since heavy armors, heavy weapons, or shields will restrict their motion greatly, they prefer light clothes, and become faster and more stealthy as they gain levels.  A Ninja knows no fear and, at high level, becomes almost immune to poison and able to see invisible things.  Dexterity determines a Ninja's ability to use Ninjutsu.",

"Snipers are good at shooting, and they can kill targets by a few shots. After they concentrate deeply, they can demonstrate their shooting talents. You can see incredibly firepower of their shots."
#endif
};


static cptr seikaku_jouhou[MAX_SEIKAKU] =
{
#ifdef JP
"���Ȃ��͓��ɓ��M����ׂ��������Ȃ����i�ł��B������Z�\�𕽋ϓI�ɂ��Ȃ��܂��B",

"���Ȃ��͍l�������Ɏ肪�o��^�C�v�ł��B���̓I�Ȕ\�͂�Z�\���㏸���܂��B�������A���@�Ɋ֌W����\�͂�Z�\�͗��A��m���̃X�e�[�^�X�������܂��B",

"���Ȃ��͓��̉�]�����������Ďv���[�����i�ł��B���̓I�Ȕ\�͉͂�����܂����A�m�\�▂�@�Ɋ֌W����Z�\�͏㏸���A���C�W���̃X�e�[�^�X�������܂��B",

"���Ȃ��͏����݂��Ƃ��낪����܂������X�̂��Ƃł͓����܂���B�ϋv�͂ɂ��D��܂����s��p�ł��B",

"���Ȃ��͂�◎������������܂��񂪖ڒ[�������^�C�v�ł��B�ǂ̃X�L������r�I���܂����Ȃ��܂����A���̓I�Ȕ\�͂͂��Ⴍ�Ȃ�܂��B",

"���Ȃ��͈�x����ƌ��߂�����l���܂���B�퓬�́A���@�\�̗͂������㏸���܂����A���@�h��A�g�o�Ƃ������\�͈͂����Ȃ�܂��B",

"���Ȃ��̓V���[�e�B���O�̐\���q�ł��B���@���낤�Ǝˌ����낤�ƂƂɂ����������Ԃ��������Ƃɂ����Ă͓V����i�ł��B�����������̐g����邱�ƂȂǑS���ᒆ�ɂ���܂���B�������o��͂ł��Ă��܂��B",

"���Ȃ��͂�������ƍC�������o�ŗ��������Ă��艽������ɂ��X�}�[�g�ł��B�������͎d����B���s���͂��܂�D�݂ł͂���܂���B",

"���z���ŃL���m���{�[�����J�Â���܂����B�S��100�K���ł������삯���������̂��D���ł��B���Ȃ��͍����\�͂ƋZ�\�������܂����A�_���W�����̑S�Ă̑��݂����Ȃ��̎ז������悤�Ɨ����ǂ���܂��B���̐��i��I��ŒZ���Ԃŏ�������ƃX�R�A���オ��܂��B",

"���Ȃ��͔\�͂͗���Ă��܂����Ȃ�����������Ă����ƂȂ����܂������Ă��܂��^�C�v�ł��B",

"���Ȃ��͂�������ƕ����ɂƂ肭�ސT�d�Ȑ��i�ł��B�����ϋv�͂������܂�����������s������̂͋��ŁA�����̋Z�\�͒Ⴍ�Ȃ��Ă��܂��܂��B",

"�������܂́A���S�҂̗��K�p�̐��i�ł��B������\�͂������Ȃ��Ă��܂��B���̐��i���g���Ώ����҂ɂȂ邱�Ƃ͗e�Ղł����A�������Ă��S�������ɂȂ�܂���B",

"(�N���X���ƂɌʐݒ肷����ꐫ�i�@���̃��b�Z�[�W�͕\������Ȃ�)",

"��߂Ƃ��ȁI�C���ӂ�邺�H",

#else

"\"Ordinary\" is a personality with no special skills or talents, with unmodified stats and skills.",

"\"Mighty\" raises your physical stats and skills, but reduces stats and skills which influence magic.  It makes your stats suitable for a warrior.  Also it directly influences your hit-points and spell fail rate.",

"\"Shrewd\" reduces your physical stats, and raises your intelligence and magical skills.  It makes your stats suitable for a mage.  Also it directly influences your hit-points and spell fail rate.",

"\"Pious\" deepens your faith in your God.  It makes your physical ability average, and your stats suitable for priest. ",

"\"Nimble\" renders you highly skilled comparatively well, but reduces your physical ability. ",

"\"Fearless\" raises both your melee and magical ability.  Stats such as magic defense and constitution are reduced.  Also it has a direct bad influence on your hit-points.",

"\"Combat\" gives you comparatively high melee and shooting abilities, and average constitution.  Other skills such as stealth, magic defence, and magical devices are weakened.  All \"Combat\" people have great respect for the legendary \"Combat Echizen\".\n\
(See \"Death Crimson\" / Ecole Software Corp.)",

"A \"Lazy\" person has no good stats and can do no action well.  Also it has a direct bad influence on your spell fail rate.",

"\"Sexy\" rises all of your abilities, but your haughty attitude will aggravate all monsters.  Only females can choose this personality.",

"A \"Lucky\" man has poor stats, equivalent to a \"Lazy\" person.  Mysteriously, however, he can do all things well.  Only males can choose this personality.",

"A \"Patient\" person does things carefully.  Patient people have high constitution, and high resilience, but poor abilities in most other skills.  Also it directly influences your hit-points.",

"\"munchkin\" is a personality for beginners.  It raises all your stats and skills.  With this personality, you can win the game easily, but gain little honor in doing so."
#endif
};

static cptr seikaku_special_jouhou[MAX_SEIKAKU_SPECIAL] =
{
	"���̐��i�͓���̃N���X�̐�p���ꐫ�i�ł��B���̃N���X�ł͂��̐��i�͑I���ł��܂���B",
	//�`���m�u���Ă������v
	"���Ȃ��͕X�̗d�����Ƃ����̂ɂȂ������񂪂���Ă����܂����B�Ή���_���������A�S�\�͂��啝�ɏ㏸���A����ɓ��Z����������A���Ȃ��͑S�\���ɖ������ӂ�Ă��܂��B���������Ȃ��̖`���̃X�R�A�̔����͂ǂ����ɏ����Ă��܂��܂��B���Ȃ��̔w���ɂ͂��̊Ԃɂ���̔����J���Ă��܂����A���Ȃ�������ɋC�t�����Ƃ͂���܂���B",
	//�p�`�����[�u���s�v
	"���Ȃ��͎蓖���莟��̖{����S�s���ɓǂ݋���A���ɖ��p�����łȂ���p�Ɨd�p�̖��@�܂ł��S�ďK�����܂����B�������������M�������Ȃ��͂��ɂ������Č��N�𑹂Ȃ��A���̐g�̔\�͎͂G���d���ɂ�����܂��B",
	//�����u���R�Ȃ��D��I�ȁv
	"���Ȃ��͐X�ň�l�Â��ɕ�炵�Ă��܂������A����Ƃ��ˑR�c��Ȗ��͂��N���オ��̂������܂����B�N�ł��������炱�̗͂����������Ďd��������܂���B���̂��Ȃ��͋ߐڍU���\�͂���������AMP�ő�l�������A���Z�����͂ȍU�����̂��̂ɕς���Ă��܂��B���������Ȃ��̖`���̃X�R�A��25%�͂ǂ����ɏ����Ă��܂��܂��B���Ȃ��̔w��ł͓�̏����������K�T�K�T�U��Ȃ���x���Ă��܂����A���Ȃ�������ɋC�t�����Ƃ͂���܂���B",
	//���ǂ񂰁u���̐��E�́v
	"���Ȃ��͖��̐��E�̗��E�D�܉؉@�E�C�i�o�ł��B	���S�߈ˈٕς̉e���Ŗ��̐��E����ǂ��o����A���̋@��ɂ��Ȃ��̖{�̂ɃX�g���X��������҂���l�c�炸�@���̂߂����Ɩژ_��ł��܂��B���̐��i��I������ƒʏ�̐��i������Z���ω����A���e�̈����ɒ��������̂ɂȂ�܂��B",
	//俎q�u�����ɔY�܂����v
	"���Ȃ��͂��̂Ƃ���A��̈����ɔY�܂���Ă��܂��B���i�͂���Ȃ�ɗF�D�I�Ȃ͂��̌��z���̏Z�l��������l�c�炸���ӂ������ďP���|�����Ă���̂ł��B	�����������A���Ȃ��̌��ɐ�Δ�_�𖼏��l��������ė͂�݂��Ă���܂����B�V���ȓ��Z���g���ďI���Ȃ�������ŊJ���܂��傤�B��_�����Ȃ��Ɏ�������Z�͂��̎����̎��Ń����_���Ɍ��܂�A���Ȃ����D���Ȃ悤�ɖ��O�����邱�Ƃ��ł��܂��B",
	//�j���ہu�ӊO�Ɋ炪�L���v
	"���Ȃ��͏����ȑ̂�ῂ��΂���̗��z�Ə�M���߂Ă���A���̋P���͑����̐l�d�̐S�𓮂����܂��B���̐��i�ŃQ�[�����J�n����ƁA�ꕔ�N�G�X�g���������z���̏Z�l�S�Ă��F�D�I�ɂȂ�܂��B�܂����̐��i��p�̓��Z�ŗF�D�I�Ȗ������ی��Ȃ��Ăяo�����Ƃ��ł��܂��B�F�R�̍U���Ɋ������܂�Ȃ��悤���܂���������ēG�Ɏ~�߂��h���܂��傤�B",
	//�Ԕ؊�u�X�}�z��v
	"���Ȃ��͂��̂Ƃ���������Ԃ��ɂ���œǏ��ɖv�����Ă���̂ł����A�Ȃ������̖͂ڂ��₽���C�����܂��B	���̐��i�ł͒ʏ�̐��i�ɔ�ׂċߐڐ퓬�\�͂��啝�ɒቺ���܂����A����ɖ��@�֌W�̔\�͂��㏸�����@����̈�K���\�ɂȂ�܂��B",
	//�ؐ�u�@�C�גq�̋S�@�v
	"���Ȃ��͐�N�ԕ��󂳂�Ă����S�̘r�Ƃ̗Z�����ʂ����܂����B	���̐��i�ł͎푰���u�S�v�ɕω����Đg�̔\�͂��啝�ɏ㏸���A�|�����G����̗͂��z������悤�ɂȂ�A����ɓ��Z�����ɍU���I�Ȃ��̂ɕύX����܂��B�܂��K���ł��閂�@�̈悪�d�p�ɕς��܂��B",
	//���̎O�d���u�d����W���́v
	"�`���m�A�N���E���s�[�X�A�G�^�j�e�B�����o�A�����l�̑�d�������Ȃ������ɓ��s���Ĉꏏ�ɐ키���ƂɂȂ�܂����B����ɂ��B���ȊO�̑S�p�����[�^���㏸���A�����̎��R�x������ɏオ��A�i�����̍U������������A�g������Z���啝�ɕω����܂��B�������X�R�A��1/3�ɂȂ�܂��B",
	//���Ӂu�T�㏕��v
	"���Ȃ��͎�̂��Ƃ�̖��߂ŒT�㏕��Ƃ��Ē����̔C���ɏo�����܂����B�m�\�A��p�A�B���\�́A�T���\�͂��啝�ɏ㏸�������W�Ɍ������������̓��Z���K�����܂��B���������p�̔L�Ԃ͒n��a�ɒu���ė����̂Ŏg�p�ł��܂���B",
	//���u�Љ�h���|���C�^�[�v
	"���Ȃ��͈ٕϒ����̂��߂ɂ��΂��f�����B���Ċ������邱�Ƃɂ��܂����B�ʏ�ƈႢ���Ɂu���݁v�𑕔�����K�v������܂���B�T���\�͂����㏸���܂������e��֌W�̔\�͂������ቺ���܂��B",
	//�����u�Ζ����v
	"���Ȃ��Ǝo�͂Ђ��Ȃ��Ƃ���Ζ����@�蓖�ĐΖ����ɂȂ�܂����B���i�̂悤�ɋ���D������U�������肷�����ɁA�Ζ����T���U�炵����T�����Ζ���R�₵����o�����ɂ����肵�Ȃ���킢�܂��B�����ɂ��x���炵�����͋C�������o�����Ȃ��́A�X�̓X�ł�����ł��c�P�����Ŕ����������邱�Ƃ��ł��܂��B���������z���ɐΖ������������i�͂Ȃ��A������X��B�͂��Ȃ�����c�P����藧�Ă邱�Ƃł��傤�B",
	//�������u�Ŏs�꒲�����v
	"���Ȃ��̓A�r���e�B�J�[�h�̈Ŏs��̒������n�߂܂����B���̐��i�ł͒ʏ�̓��Z�̑���ɐE�Ɓu�J�[�h���l�v�Ɠ������Z���g�p�ł��܂��B�X�J�[�g�̉B���|�P�b�g�ɂ͂����̎���̖��@�̑���ɃA�r���e�B�J�[�h��8��ނ܂Ŋi�[�ł��܂��B���@�����g�����ʏ�̖��@�͂���܂Œʂ�ɏK���ł��܂��B",
	//���u�ʉݔ��s�ҁv
	"���Ȃ��̓A�r���e�B�J�[�h�u�[���̎d�|���l�ŁA�J�[�h�����p�̒ʉ݂𔭍s���ċ����̕x����ɂ��܂����B���̃Z�[�u�f�[�^�ŉߋ��̃v���C���[���J�[�h�̔����Ɏx���������z��1/4�������ăQ�[�����X�^�[�g���܂��B���������̎����͓V��Љ�̈ێ����W�̂��߂Ɏg���Ȃ���΂Ȃ�܂���B���̂��ߊX�ňꕔ�̎{�݂��g�p�s�\�ɂȂ�܂��B�Q�[���̃X�R�A���ʏ��1/4�ɂȂ�܂��B",

};

//���ꐫ�i�ǉ�memo:
//#define SEIKAKU_SPECIAL_***��ǉ�
//get_special_seikaku_index()�ɏ����ǉ�
//seikaku_special_jouhou[]�ɐ������ǉ�
//table.c��seikaku_info_special[]�Ƀp�����[�^�ǉ�
//�X�R�A�T�[�o�X�N���v�g��htmlout.py�ɒǋL
//is_special_seikaku(SEIKAKU_SPECIAL_***)�Ŕ��肵�Ă��̐��i�Ɋւ�����ꏈ��

//�N���XID����,���̃N���X�����ꐫ�i�ɂȂ����Ƃ��̓��ꐫ�i�C���f�b�N�X��Ԃ��B
//race_id���g�������Ǝv�������푰�ϗe������G���[�ɂȂ�̂ɋC�Â��ă{�c�B
//���̊֐��͎��ۂ�p_ptr->pseikaku�Ɋւ�炸���ꐫ�i�C���f�b�N�X��Ԃ��B
static int get_special_seikaku_index(int class_idx)
{

	if (class_idx == CLASS_CIRNO)		return SEIKAKU_SPECIAL_CIRNO;	
	if (class_idx == CLASS_PATCHOULI)	return SEIKAKU_SPECIAL_PATCHOULI;
	if (class_idx == CLASS_NARUMI)		return SEIKAKU_SPECIAL_NARUMI;
	if (class_idx == CLASS_UDONGE)		return SEIKAKU_SPECIAL_UDONGE;
	if (class_idx == CLASS_SUMIREKO)	return SEIKAKU_SPECIAL_SUMIREKO;
	if (class_idx == CLASS_SHINMYOU_2)	return SEIKAKU_SPECIAL_SHINMYOUMARU;
	if (class_idx == CLASS_BANKI)		return SEIKAKU_SPECIAL_SEKIBANKI;
	if (class_idx == CLASS_KASEN)		return SEIKAKU_SPECIAL_KASEN;
	if (class_idx == CLASS_3_FAIRIES)	return SEIKAKU_SPECIAL_3_FAIRIES;
	if (class_idx == CLASS_ORIN)		return SEIKAKU_SPECIAL_ORIN;
	if (class_idx == CLASS_AYA)			return SEIKAKU_SPECIAL_AYA;
	if (class_idx == CLASS_JYOON)		return SEIKAKU_SPECIAL_JYOON;
	if (class_idx == CLASS_MARISA)		return SEIKAKU_SPECIAL_MARISA;
	if (class_idx == CLASS_MEGUMU)		return SEIKAKU_SPECIAL_MEGUMU;


	return SEIKAKU_SPECIAL_NONE;
}





/*:::���@�i�ƌ��p�j�̗̈�������@�Y����TV_BOOK_????�ɑΉ�*/
static cptr realm_jouhou[TV_BOOK_END + 1] = {
	"",//�_�~�[
	"�������\������n���Ε��Ȃǂ̌��f�𑀂�̈�ł��B�ł����ʂȍU�����@������A�����Ď኱�̖h��E�⏕���@���܂܂�܂��B�����̕΂����G�ɑ΂��ėL���ȑŌ���^�����܂��B",
	"�B���ꂽ���A�m�肦�Ȃ��͂��̎���m�o����̈�ł��B���m��i���قڊ����ɑ����A�G�̐��_�ɍU�����閂�@������܂��B���͂ȕ⏕��񕜂�����܂����A�ɂ߂č���x�ł��B",
	"���̂⎩���̑̂ɑ΂����͂����߂ē��ʂȌ��ʂ�^����̈�ł��B�h��ƕ⏕�ɋɂ߂ėD��A������x�̐퓬��i������܂��B�������̖��@��MP�̂ق��ɐG�}���K�v�ƂȂ�܂��B",
	"�����Ȃ��Ƃ���ɖ���J���l�X�Ȃ��̂��Ăяo���̈�ł��B�����ƃe���|�[�g�n�ɋɂ߂ėD��܂����A����ȊO�͕s����ł��B�����̎�ނɂ���Ă͂�MP�̂ق��ɑ㏞���K�v�ƂȂ邱�Ƃ�����܂��B�܂��A�������@�Ɏ��s�������͏��҂��悤�Ƃ������̂��P���������Ă��邱�Ƃ�����܂��B",
	"�����R�I�Ȋ�Ղ��N�����̈�ł��B���m�ƕ⏕���[�����Ă���l�X�ȕ֗��Ȗ��@�������܂����A�G�ɒ��ڍU�����閂�@�͂���܂���B",
	"�����̃G�l���M�[�𑀂�̈�ł��B�񕜂ɍł��D��A�h���⏕�����͂ł��B�U�����@������܂����A���f�b�h�ɂ̂ݗL���ł��B",
	"�׈��Ȃ���̂�ł��|���̈�ł��B�d���A�����A�A���f�b�h�ɑ΂��ėL���ȍU�����@�������A�⏕���@���[�����Ă��܂��B�ŏ�ʂ̖��@���ɂ͋��͂Ȑ_�~�낵�̖��@������܂����A�����͓�ȏ㓯���Ɍ��ʂ𔭊����邱�Ƃ��ł��܂���B",
	"���R�̗͂��؂��̈�ł��B���ʂȖ��@���o�����X�ǂ������A���Ɋ��m�Ɩh��ɗD��܂��B�������U�����@�̈З͂͂���߂ł��B",
	"����̑̂���͂̒n�`��ω�������̈�ł��B���͂����l�ȏp�������܂����A�푰���Ƃ̓K������Փx�ɑ傫���e�����܂�(�\��)�B�܂��ϐg���@�̑����͖h����ʂ𔭊����Ȃ��Ȃ����芪�����g���Ȃ��Ȃ�ȂǓƓ��̃��X�N�𔺂��܂��B",
	"�׈��ȗ͂𑀂�̈�ł��B�ł�Í������𒆐S�Ƃ����U�����@�A�ɂ߂čU���I�ȕ⏕���@�A�����Ĉ����̎g���∫���ւ̕ϐg�Ƃ���������������܂��B�������h��⊴�m�͂��܂蓾�ӂł͂���܂���B���̗̈�̖��@���K������Ɣj�׍U���Ɏキ�Ȃ�܂��B",
	"������g��������􂢂ōU������̈�ł��B�⏕�⏢���𒆐S�ɑ��ʂȎ����������܂����A�􂢂ɂ��U���͈�����A���f�b�h�△�����ɂ͂قږ��͂ł��B���̗̈�̖��@���K������Ɣj�׍U���Ɏキ�Ȃ�܂��B",
	"�����̖������ȃG�l���M�[���䂪���Ƃ���̈�ł��B�����I�΂Ȃ����͂ȍU�����@�������܂��B�������⏕���@������܂����A���������������j��I�Ȃ��̂ł��B���̗̈�̑S�Ă̖��@�̎g�p�ɂ̓��X�N�������܂��B",
	"�K�E���Ƃ͌��p�Ƃ����̊ԂŌ��p���ꖁ���ꂽ���̋Z�ł��B����ł̍U���ɓƓ��̌��ʂ���悹����悤�ȋZ���قƂ�ǂł��B�K�E���͈�x�K������Δ�`���������Ă��Ȃ��Ă��g�p�ł��܂��B�������ꕔ�̋Z�͓���̎�ނ̕���𑕔����Ă��Ȃ��Ǝg�p�ł��܂���B",
	"",
	"���z���ŗl�X�ȉ\��s�s�`�����������悤�ɂȂ�܂����B�����ɕq���ꕔ�̎ҒB�͊O�E���痬��Ă����������������A�����u�I�J���g�v�𖂖@�̂悤�ɏK�����Ďg�����Ȃ��܂��B�I�J���g�͍U���𒆐S�ɋ��͂ő��ʂȂ��̂������܂����A����̏����𖞂�������㏞���x����Ȃ��Ǝg���Ȃ��Ȃ̋������̂��قƂ�ǂł��B",
};



/*
static cptr realm_jouhou[VALID_REALM] =
{
#ifdef JP
"�����͉񕜔\�͂ɗD�ꂽ���@�ł��B���Â�h��A���m���@�������܂܂�Ă��܂����A�U���������킸���Ɏ����Ă��܂��B���ɍ����x���̎����ɂ̓A���f�b�h��o�ɋA���͂�����ƌ����Ă��܂��B",

"��p�́umeta�v�̈�ł���A���m��Ӓ�A����ɑދp�p�̎����⎩�g�̔\�͂����߂�����Ȃǂ֗̕��Ȏ������܂܂�Ă��܂��B�������A���ڍU���p�̎����͎����Ă��܂���B",

"���R�̖��@�͎g�p�҂����f�̃}�X�^�[�ɂ��܂��B����ɂ͖h��A�T�m�A���ÂƍU���������܂܂�Ă��܂��B�܂��A�����ȊO�̗̈�ōō��̎��Î��������̗̈�ɂ���܂��B",

"�J�I�X�̖��@�͐��䂪����ŁA�\���̂ł��Ȃ����@������܂��B�J�I�X�͔��ɔ񌳑f�I�ł���A�J�I�X�̎����͑z���ł���ł�����ׂ��j�󕺊�ł��B���̎�������������̂̓J�I�X�̐땺�ɑ΂��A�G�⎩�����g�������ψق�����悤�v�����܂��B",

"�����p�ł���Í��̖��@�قǎ׈��ȃJ�e�S���[�͂���܂���B�����̎����͔�r�I�w�Ԃ̂�����ł����A�����x���ɂȂ�Əp�҂ɐ����ƃA���f�b�h�����R�ɑ���\�͂�^���܂��B�c�O�Ȃ��ƂɁA�����Ƃ����͂Ȏ����͂��̐G�}�Ƃ��ďp�Ҏ��g�̌���K�v�Ƃ��A�r�����ɂ��΂��Ώp�҂������܂��B",

"�g�����v�̖��@�̓e���|�[�g�n�̎����Ő��I���ꂽ���̂������Ă���A���̏o������͑��̐������������邽�߂ɂ��g���邽�߁A������������I�肷����ꂽ���̂����l�Ɏ����Ă��܂��B�������A���̖��@�ɂ���đS�Ẳ������ʂ̏ꏊ�֌Ă΂��̂𗝉�����킯�ł͂Ȃ��A�������������Ɏ��s����Ƃ��̐����͓G�ƂȂ�܂��B",

"��p�̖��@�́A�S�Ă̗̈悩��L�p�Ȏ���������������悤�Ƃ������p�r�̈�ł��B�K�v�ȁu����v�I�����������Ă��Ă������x���̋��͂Ȏ����͎����Ă��܂���B���ʂƂ��āA�S�Ă̎������͊X�Ŕ������߂邱�Ƃ��ł��܂��B�܂��A���̗̈�ɑ��݂��铯�l�Ȏ����̕������჌�x���A��R�X�g�ŏ����邱�Ƃ��ł��܂��B",

"���̖��@�́A�����⓹����������邽�߂̖��@���܂܂�Ă��܂��B���@�ɂ���Ď������g�̐퓬�͂���ɍ��߂邱�Ƃ��ł��܂����A����𒼐ڍU������悤�Ȏ����͊܂܂�Ă��܂���B",

"�����̖��@�͈Í��Ɠ��l���Ɏ׈��ȃJ�e�S���[�ł��B�l�X�ȍU�����@�ɗD��A�܂������̂��Ƃ��m�o�\�͂𓾂邱�Ƃ��ł��܂��B�����x���̎����͈��������݂ɑ���A�������g�̓��̂��������������邱�Ƃ��ł��܂��B",

"�j�ׂ́u���`�v�̖��@�ł��B���ړG�������閂�@�������܂܂�A���Ɏ׈��ȓG�ɑ΂���͂͋���ׂ����̂�����܂��B�������A�P�ǂȓG�ɂ͂��܂���ʂ�����܂���B",

"�̏W�́A�̂ɂ���Č��ʂ𔭊����閂�@�ł��B���@�Ɠ��l�A�g�������Ɍ��ʂ̂�����̂ƁA�̂������邱�Ƃɂ���Ď������Č��ʂ𔭊�������̂�����܂��B��҂̏ꍇ�́AMP�̑���������ʂ𔭊����邱�Ƃ��ł��܂����A�����ɉ̂���̂�1�����Ƃ�������������܂��B",

"���|�̏��́A�l�X�Ȑ퓬�̋Z�ɂ��ď�����Ă��܂��B���̖{�͋Z���o����Ƃ��ɓǂޕK�v������܂����A��x�o�����Z�͎g���̂ɖ{�����K�v�͂���܂���B�Z���g���Ƃ��ɂ͕K������𑕔����Ă��Ȃ���΂����܂���B",

"��p�͊��ނׂ��̈�ł��B�����̎􂢂̌��t���̂̂悤�ɖa���Ȃ���r�����܂��B�����̎����͉r���������邱�Ƃɂ���Č��ʂ���������܂��B�����ɂ͑���̍s���𑩔�������́A�_���[�W��^������́A�U���ɑ΂��Ĕ���������̂���������܂��B"
#else

"Life magic is very good for healing; it relies mostly on healing, protection and detection spells.  Also life magic have a few attack spells as well.  It said that some high level spell of life magic can disintegrate Undead monsters into ash.",

"Sorcery is a `meta` realm, including enchantment and general spells.  It provides superb protection spells, spells to enhance your odds in combat and, most importantly, a vast selection of spells for gathering information.  However, Sorcery has one weakness: it has no spells to deal direct damage to your enemies.",

"Nature magic makes you master of elements; it provides protection, detection, curing and attack spells.  Nature also has a spell of Herbal Healing, which is the only powerful healing spell outside the realm of Life magic.",

"There are few types of magic more unpredictable and difficult to control than Chaos magic.  Chaos is the very element of unmaking, and the Chaos spells are the most terrible weapons of destruction imaginable.  The caster can also call on the primal forces of Chaos to induce mutations in his/her opponents and even him/herself.",

"There is no fouler nor more evil category of spells than the necromantic spells of Death Magic.  These spells are relatively hard to learn, but at higher levels the spells give the caster power over living and the (un)dead, but the most powerful spells need his / her own blood as the focus, often hurting the caster in the process of casting.",

"Trump magic has, indeed, an admirable selection of teleportation spells.  Since the Trump gateways can also be used to summon other creatures, Trump magic has an equally impressive selection of summoning spells.  However, not all monsters appreciate being drawn to another place by Trump user.",

"Arcane magic is a general purpose realm of magic.  It attempts to encompass all 'useful' spells from all realms.  This is the downside of Arcane magic: while Arcane does have all the necessary 'tool' spells for a dungeon delver, it has no ultra-powerful high level spells.  As a consequence, all Arcane spellbooks can be bought in town.  It should also be noted that the 'specialized' realms usually offer the same spell at a lower level and cost. ",

"Craft magic can strengthen the caster or the equipments.  These spells can greatly improve the caster's fighting ability.  Using them against opponents directly is not possible.",

"Demon is a very evil realm, same as Death.  It provides various attack spells and devilish detection spells.  at higher levels, Demon magic provides ability to dominate demons, and to polymorph yourself into a demon.",

"Crusade is a magic of 'Justice'.  It includes damage spells, which are greatly effective against foul and evil monsters, but have poor effects against good monsters.",

"Music magic shows various effects as sing song.  There is two type of song; the one which shows effects instantly and the other one shows effect continuously until SP runs out.  But the latter type has a limit; only one song can be sing at the same time.",

"The books of Kendo describe about various combat techniques.  When learning new techniques, you are required to carry the books, but once you memorizes them, you don't have to carry them.  When using a technique, wielding a weapon is required.",

"Hex is a very terrible realm. Spells gives continual effects when they are spelled continually like songs. Spells may obstract monsters' actions, may deal damages in sight, may revenge against enemies."
#endif
};
*/
/*:::���@�̈�I�����̕⏕�����@�폜����*/
/*
///realm
static cptr realm_subinfo[VALID_REALM] =
{
#ifdef JP
"���m�Ɩh��Ɖ񕜂ɗD��Ă��܂�",
"�U���͂ł��܂��񂪔��ɕ֗��ł�",
"���m�Ɩh��ɗD��Ă��܂�",
"�j��I�ȍU���ɗD��Ă��܂�",
"�����̂���G�ւ̍U���ɗD��Ă��܂�",
"�����ƃe���|�[�g�ɗD��Ă��܂�",
"���ア�Ȃ�������ɕ֗��ł�",
"���ڐ퓬�̕⏕�ɗD��Ă��܂�",
"�U���Ɩh��̗��ʂɗD��Ă��܂�",
"�׈��ȉ����ɑ΂���U���ɗD��Ă��܂�",
"�l�X�Ȗ��@���ʂ��������̂��̂��܂�",
"�Ō��U���ɓ���\�͂�t�����܂�",
"�G���ז������Q��_���܂�"
#else
"Good at detection and healing.",
"Utility and protective spells.",
"Good at detection and defence.",
"Offensive and destructive.",
"Ruins living creatures.",
"Good at summoning, teleportation.",
"Very useful but poor a bit.",
"Support for melee fighting.",
"Good at both offence and defence.",
"Destroys evil creatures.",
"Song with magical effects.",
"Special attacks on melee.",
"Good at obstacle and revenge."
#endif
};
*/

/*
 * Autoroll limit
 */
 /*:::�����ł����g���ĂȂ��B�I�[�g���[���[�̍Œ�l��?*/
static s16b stat_limit[6];

/*:::�̊i�I�[�g���[���[�p��*/
static struct {
	s16b agemin, agemax;
	s16b htmin, htmax;
	s16b wtmin, wtmax;
	s16b scmin, scmax;
} chara_limit;

/*
 * Autoroll matches
 */
static s32b stat_match[6];

/*
 * Autoroll round
 */
static s32b auto_round;


static void birth_quit(void)
{
	remove_loc();
	quit(NULL);
}


/*
 *  Show specific help file
 */
static void show_help(cptr helpfile)
{
	/* Save screen */
	screen_save();

	/* Peruse the help file */
	(void)show_file(TRUE, helpfile, NULL, 0, 0);

	/* Load screen */
	screen_load();
}

/*:::���@�̗̈於�A�K����\��*/
///mod160620 �I�J���g�ǉ�
void print_realm_aptitude(int realm, bool second)
{

	char buf[80];
	cptr category[3] ={"���p","��p","�d�p"};

	if(realm < MIN_MAGIC || realm > MAX_MAGIC && realm != TV_BOOK_OCCULT) return;

	put_str("                                   ", 3, 40);
	put_str("                                   ", 4, 40);
	put_str("                                   ", 5, 40);
	put_str("                                   ", 6, 40);
	if(realm == TV_BOOK_OCCULT)
		sprintf(buf, "��p");
	else
		sprintf(buf, "%s��%s", realm_names[realm], category[(realm-1)/4]);
	c_put_str(TERM_L_BLUE, buf, 3, 40);
	sprintf(buf, "�E�ƓK���F%c", 'G' - cp_ptr->realm_aptitude[realm]);
	c_put_str(TERM_WHITE, buf, 4, 40);
	c_put_str(TERM_WHITE, "�푰�K���F", 5, 40);
	switch(rp_ptr->realm_aptitude[realm])
	{
	case 0:
		c_put_str(TERM_L_DARK, "����", 5, 50);break;
	case 1:case 2:
		c_put_str(TERM_L_RED, "����", 5, 50);break;
	case 3:case 4:
		c_put_str(TERM_ORANGE, "���", 5, 50);break;
	case 5:
		c_put_str(TERM_WHITE, "����", 5, 50);break;
	case 6:case 7:
		c_put_str(TERM_YELLOW, "����", 5, 50);break;
	case 8:case 9:
		c_put_str(TERM_L_GREEN, "��z", 5, 50);break;
	default:
		c_put_str(TERM_L_RED, "ERROR:�w��O�̐��l", 5, 50);break;
	}
	if(second && !same_category_realm(p_ptr->realm1,realm))
		c_put_str(TERM_L_UMBER, "(���F���̈�Ƒ����������̈�)", 6, 40);

}


/*
 * Choose from one of the available magical realms
 */
static byte choose_realm(int *count)
{
	int picks[TV_BOOK_END] = {0};
	int k, i, cs, os;
	byte auto_select = REALM_NONE;
	int n = 0;
	char c;
	char sym[TV_BOOK_END];
	char p2 = ')';
	char buf[80], cur[80];

	if(p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
	{
		*count = 1;
		return TV_BOOK_HISSATSU;
	}
	//���@�K���s��/�s�v�ȐE�͏I��
	else if(cp_ptr->realm_aptitude[0] == 0 || REALM_SPELLMASTER)
	{
		*count = 0;
		return REALM_NONE;
	}
	//v1.1.60 �Ԕ؊�͐�p���i�u�X�}�z��v�ȊO���@�K���ł��Ȃ�
	//���̊֐��͕��ʐ��i�I��O�ɌĂ΂�邪�A�Ԕ؊���i�u�X�}�z��v�ɂ����Ƃ��̂Ƃ��݂̂�����x�Ă΂��
	else if (p_ptr->pclass == CLASS_BANKI && !is_special_seikaku(SEIKAKU_SPECIAL_SEKIBANKI))
	{
		*count = 0;
		return REALM_NONE;

	}

	for(i=1;i<=TV_BOOK_END;i++)
	{
		if(cp_ptr->realm_aptitude[i] > 1)
		{

			//v1.1.65 �ؐ�͓��ꐫ�i�ɂ���Ɨd�p�I���\�ɂȂ�(���i�I����ɍĂт��̊֐��ɖ߂��Ă���)
			if (p_ptr->pclass == CLASS_KASEN)
			{
				if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
				{
					if (i > TV_BOOK_SUMMONS && i < TV_BOOK_TRANSFORM) continue;
				}
				else
				{
					if (i >= TV_BOOK_TRANSFORM) continue;
				}
			}


			if(p_ptr->realm1 != REALM_NONE)
			{
				if(p_ptr->realm1 == i) continue;
				if(p_ptr->pclass == CLASS_PRIEST && !same_category_realm(p_ptr->realm1,i)) continue;
			}
			picks[(*count)] = i;
			(*count)++;
			auto_select = i;
		}
	}
#if 0
	/* Count the choices */
	if (choices & CH_LIFE)
	{
		(*count)++;
		auto_select = REALM_LIFE;
	}
	if (choices & CH_SORCERY)
	{
		(*count)++;
		auto_select = REALM_SORCERY;
	}
	if (choices & CH_NATURE)
	{
		(*count)++;
		auto_select = REALM_NATURE;
	}
	if (choices & CH_CHAOS)
	{
		(*count)++;
		auto_select = REALM_CHAOS;
	}
	if (choices & CH_DEATH)
	{
		(*count)++;
		auto_select = REALM_DEATH;
	}
	if (choices & CH_TRUMP)
	{
		(*count)++;
		auto_select = REALM_TRUMP;
	}
	if (choices & CH_ARCANE)
	{
		(*count)++;
		auto_select = REALM_ARCANE;
	}
	if (choices & CH_ENCHANT)
	{
		(*count)++;
		auto_select = REALM_CRAFT;
	}
	if (choices & CH_DAEMON)
	{
		(*count)++;
		auto_select = REALM_DAEMON;
	}
	if (choices & CH_CRUSADE)
	{
		(*count)++;
		auto_select = REALM_CRUSADE;
	}
	if (choices & CH_MUSIC)
	{
		(*count)++;
		auto_select = REALM_MUSIC;
	}
	if (choices & CH_HISSATSU)
	{
		(*count)++;
		auto_select = REALM_HISSATSU;
	}
	if (choices & CH_HEX)
	{
		(*count)++;
		auto_select = REALM_HEX;
	}
#endif
	clear_from(10);

	/* Auto-select the realm */
	/*:::�K���\�ȗ̈悪1���񖂖@�E�Ȃ炱���ŏI��*/
	if ((*count) < 2) return auto_select;

	///class
	/* Constraint to the 1st realm */
/*
	if (p_ptr->realm2 != 255)
	{
		if (p_ptr->pclass == CLASS_PRIEST)
		{
			if (is_good_realm(p_ptr->realm1))
			{
				choices &= ~(CH_DEATH | CH_DAEMON);
			}
			else
			{
				choices &= ~(CH_LIFE | CH_CRUSADE);
			}
		}
	}
*/
	/* Extra info */
#ifdef JP
	if(p_ptr->pclass == CLASS_LYRICA || p_ptr->pclass == CLASS_MERLIN)
		put_str ("(�����őI���������@�̓��i�T�ɂ����g���܂���)", 11, 5);

	put_str ("���ӁF���@�̗̈�̑I���ɂ�肠�Ȃ����K����������̃^�C�v�����܂�܂��B", 23, 5);
#else
	put_str ("Note: The realm of magic will determine which spells you can learn.", 23, 5);
#endif

	cs = 0;
	for(i=1;i<=(*count);i++)
	{
		sym[n] = I2A(n);
		sprintf(buf, "%c%c %s", sym[n], p2, realm_names[picks[n]]);
		put_str(buf, 12 + (n/4), 2 + 15 * (n%4));
		n++;
	}
#if 0
	cs = 0;
	for (i = 0; i<32; i++)
	{
		/* Analize realms */
		if (choices & (1L << i))
		{
			if (p_ptr->realm1 == i+1)
			{
				
				if (p_ptr->realm2 == 255)
					cs = n;
				/*:::���̈�I�����A���̈�Ɠ����̈悪�\������Ȃ��Ȃ鏈���炵��*/
				else
					continue;
			}
			if (p_ptr->realm2 == i+1)
				cs = n;

			sym[n] = I2A(n);

			sprintf(buf, "%c%c %s", sym[n], p2, realm_names[i+1]);
			put_str(buf, 12 + (n/5), 2 + 15 * (n%5));
			picks[n++] = i+1;
		}
	}
#endif
#ifdef JP
	sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a realm */
	k = -1;
	os = n;
	while (1)	
	{
		/* Move Cursol */
		if (cs != os)
		{
			c_put_str(TERM_WHITE, cur, 12 + (os/4), 2 + 15 * (os%4));
			put_str("                                   ", 3, 40);
			put_str("                                   ", 4, 40);
			put_str("                                   ", 5, 40);
			put_str("                                   ", 6, 40);

			if(cs == n)
			{
#ifdef JP
				sprintf(cur, "%c%c %s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
			}
			else
			{
				sprintf(cur, "%c%c %s", sym[cs], p2, realm_names[picks[cs]]);
				print_realm_aptitude(picks[cs],(p_ptr->realm1 != REALM_NONE));

			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/4), 2 + 15 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�̈��I��ŉ�����(%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[n-1]);
#else
		sprintf(buf, "Choose a realm (%c-%c) ('=' for options): ", sym[0], sym[n-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return 255;
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == n)
			{
				k = randint0(n);
				break;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(n);
			break;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < n) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= n) cs += 4;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < n))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < n))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("tmagic.txt#Realm");
#else
			show_help("magic.txt#MagicRealms");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Clean up */
	clear_from(10);

	return (picks[k]);
}

/*
 * Choose the magical realms
 */
static bool get_player_realms(void)
{
	int i, count;
	char buf[80];

	/* Clean up infomation of modifications */
	put_str("                                           ", 3, 40);
	put_str("                                           ", 4, 40);
	put_str("                                           ", 5, 40);
	put_str("                                           ", 6, 40);

	/* Select the first realm */
	while (1)
	{
		char temp[80*10];
		cptr t;
		count = 0;

		p_ptr->realm1 = REALM_NONE;
		p_ptr->realm2 = 255;
		p_ptr->realm1 = choose_realm(&count);

		if (255 == p_ptr->realm1) return FALSE;
		if (!p_ptr->realm1) break;

		/* Clean up*/
		clear_from(10);
		print_realm_aptitude(p_ptr->realm1,FALSE);

		roff_to_buf(realm_jouhou[p_ptr->realm1], 74, temp, sizeof(temp));
		t = temp;
		for (i = 0; i < 10; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

		if (count < 2)
		{
#ifdef JP
			prt("�����L�[�������Ă�������", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			break;
		}
		else if(rp_ptr->realm_aptitude[p_ptr->realm1] == 0)
		{
			msg_format("���Ȃ��ɂ͂��̗̈���C�����邱�Ƃ͂ł������ɂȂ��B");
		}
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
	}
	if(p_ptr->realm1 != REALM_NONE)
	{
		put_str("���@        :", 6, 1);
		c_put_str(TERM_L_BLUE, realm_names[p_ptr->realm1], 6, 15);
	}
	/* Select the second realm */
	p_ptr->realm2 = REALM_NONE;
	if (p_ptr->realm1 && cp_ptr->realm_aptitude[0] == 3)
	{

		/* Select the second realm */
		while (1)
		{
			char temp[80*8];
			cptr t;

			p_ptr->realm2 = REALM_NONE;
			count = 0;
			p_ptr->realm2 = choose_realm(&count);

			if (255 == p_ptr->realm2) return FALSE;
			if (!p_ptr->realm2) break;

			/* Clean up*/
			clear_from(10);
			print_realm_aptitude(p_ptr->realm2,TRUE);
			roff_to_buf(realm_jouhou[p_ptr->realm2], 74, temp, sizeof(temp));
			t = temp;
			for (i = 0; i< 6; i++)
			{
				if(t[0] == 0)
					break; 
				else
				{
					prt(t, 12+i, 3);
					t += strlen(t) + 1;
				}
			}

			if (count < 2)
			{
#ifdef JP
				prt("�����L�[�������Ă�������", 0, 0);
#else
				prt("Hit any key.", 0, 0);
#endif
				(void)inkey();
				prt("", 0, 0);
				break;
			}
#ifdef JP


			if(rp_ptr->realm_aptitude[p_ptr->realm2] == 0)
			{
				msg_format("���Ȃ��ɂ͂��̗̈���C�����邱�Ƃ͂ł������ɂȂ��B");
			}
			if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
#else
			else if (get_check_strict("Are you sure? ", CHECK_DEFAULT_Y)) break;
#endif

		}
		clear_from(10);

		if (p_ptr->realm2)
		{
			/* Print the realm */
			c_put_str(TERM_L_BLUE, format("%s, %s", realm_names[p_ptr->realm1], realm_names[p_ptr->realm2]), 6, 15);
		}
	}
	if(p_ptr->pclass == CLASS_MAGE)
	{
		if(p_ptr->realm1 <= TV_BOOK_SUMMONS) strcpy(player_class_name, "���p�t");
		else if(p_ptr->realm1 <= TV_BOOK_NATURE) strcpy(player_class_name, "���t");
		else if(p_ptr->realm1 <= TV_BOOK_CHAOS) strcpy(player_class_name, "�d�p�t");
		else  strcpy(player_class_name, "��p�t");

	}
	else if( p_ptr->pclass == CLASS_HIGH_MAGE)
	{
		if(p_ptr->realm1 == TV_BOOK_ELEMENT) strcpy(player_class_name, "�G�[�e���}�X�^�[");
		else if(p_ptr->realm1 == TV_BOOK_FORESEE) strcpy(player_class_name, "�v���t�F�b�g");
		else if(p_ptr->realm1 == TV_BOOK_ENCHANT) strcpy(player_class_name, "�G���`�����^�[");
		else if(p_ptr->realm1 == TV_BOOK_SUMMONS) strcpy(player_class_name, "�T�}�i�[");
		else if(p_ptr->realm1 == TV_BOOK_MYSTIC) strcpy(player_class_name, "�~�X�e�B�b�N");
		else if(p_ptr->realm1 == TV_BOOK_LIFE) strcpy(player_class_name, "���W�F�l���[�^�[");
		else if(p_ptr->realm1 == TV_BOOK_PUNISH) strcpy(player_class_name, "�V���[�}��");
		else if(p_ptr->realm1 == TV_BOOK_NATURE) strcpy(player_class_name, "�W�I�}���T�[");
		else if(p_ptr->realm1 == TV_BOOK_TRANSFORM) strcpy(player_class_name, "�V�F�C�v�V�t�^�[");
		else if(p_ptr->realm1 == TV_BOOK_DARKNESS) strcpy(player_class_name, "�f�r���T�}�i�[");
		else if(p_ptr->realm1 == TV_BOOK_NECROMANCY) strcpy(player_class_name, "�l�N���}���T�[");
		else if(p_ptr->realm1 == TV_BOOK_CHAOS) strcpy(player_class_name, "���O���X�}�X�^�[");
		else  strcpy(player_class_name, "�I�J���e�B�X�g");

	}
		c_put_str(TERM_WHITE, "                 ", 5, 15);
		c_put_str(TERM_L_BLUE, player_class_name, 5, 15);



	return (TRUE);
}

/*
 * Save the current data for later
 */
/*:::�L�������C�N���̋L�^*/
/*:::���j�[�N�v���C���[��������g���񂵂͉\���������Astat_MAX_MAX�͕ύX�\��*/
static void save_prev_data(birther *birther_ptr)
{
	int i;

	/* Save the data */
	birther_ptr->psex = p_ptr->psex;
	birther_ptr->prace = p_ptr->prace;
	birther_ptr->pclass = p_ptr->pclass;
	birther_ptr->pseikaku = p_ptr->pseikaku;
	birther_ptr->realm1 = p_ptr->realm1;
	birther_ptr->realm2 = p_ptr->realm2;
	birther_ptr->age = p_ptr->age;
	birther_ptr->ht = p_ptr->ht;
	birther_ptr->wt = p_ptr->wt;
	birther_ptr->sc = p_ptr->sc;
	birther_ptr->au = p_ptr->au;
	///mod140307 ��Փx�ύX
	birther_ptr->difficulty = difficulty;


	/* Save the stats */
	for (i = 0; i < 6; i++)
	{
		birther_ptr->stat_max[i] = p_ptr->stat_max[i];
		birther_ptr->stat_max_max[i] = p_ptr->stat_max_max[i];
	}

	/* Save the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		birther_ptr->player_hp[i] = p_ptr->player_hp[i];
	}

	birther_ptr->chaos_patron = p_ptr->chaos_patron;

	///sysdel
	/* Save the virtues */
	for (i = 0; i < 8; i++)
	{
		birther_ptr->vir_types[i] = p_ptr->vir_types[i];
	}

	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(birther_ptr->history[i], p_ptr->history[i]);
	}
}


/*
 * Load the previous data
 */
/*:::�N�C�b�N�X�^�[�g�p�ɈȑO�̃L�������C�N���𓾂�*/
///system �傪����ȃp�����[�^�ύX�������炱�����ύX�K�v��
static void load_prev_data(bool swap)
{
	int i;

	birther	temp;

	/*** Save the current data ***/
	if (swap) save_prev_data(&temp);


	/*** Load the previous data ***/

	/* Load the data */
	p_ptr->psex = previous_char.psex;
	p_ptr->prace = previous_char.prace;
	p_ptr->pclass = previous_char.pclass;
	p_ptr->pseikaku = previous_char.pseikaku;
	p_ptr->realm1 = previous_char.realm1;
	p_ptr->realm2 = previous_char.realm2;
	p_ptr->age = previous_char.age;
	p_ptr->ht = previous_char.ht;
	p_ptr->wt = previous_char.wt;
	p_ptr->sc = previous_char.sc;
	p_ptr->au = previous_char.au;

	///mod140307 ��Փx�ύX
	difficulty = previous_char.difficulty;

	/* Load the stats */
	for (i = 0; i < 6; i++)
	{
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = previous_char.stat_max[i];
		p_ptr->stat_max_max[i] = previous_char.stat_max_max[i];
	}

	/* Load the hp */
	for (i = 0; i < PY_MAX_LEVEL; i++)
	{
		p_ptr->player_hp[i] = previous_char.player_hp[i];
	}
	p_ptr->mhp = p_ptr->player_hp[0];
	p_ptr->chp = p_ptr->player_hp[0];

	p_ptr->chaos_patron = previous_char.chaos_patron;

	for (i = 0; i < 8; i++)
	{
		p_ptr->vir_types[i] = previous_char.vir_types[i];
	}

	/* Load the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(p_ptr->history[i], previous_char.history[i]);
	}


	/*** Save the previous data ***/
	if (swap)
	{
		(void)COPY(&previous_char, &temp, birther);
	}
}




/*
 * Returns adjusted stat -JK-  Algorithm by -JWT-
 */
///mod140104 �p�����[�^����ƕ\�L��ύX
static int adjust_stat(int value, int amount)
{
	int i;

	value += amount;
	if(value < 3) value = 3;
	return (value);




#if 0
	/* Negative amounts */
	if (amount < 0)
	{
		/* Apply penalty */
		for (i = 0; i < (0 - amount); i++)
		{
			if (value >= 18+10)
			{
				value -= 10;
			}
			else if (value > 18)
			{
				value = 18;
			}
			else if (value > 3)
			{
				value--;
			}
		}
	}

	/* Positive amounts */
	else if (amount > 0)
	{
		/* Apply reward */
		for (i = 0; i < amount; i++)
		{
			if (value < 18)
			{
				value++;
			}
			else
			{
				value += 10;
			}
		}
	}

	/* Return the result */
	return (value);
#endif
}




/*
 * Roll for a characters stats
 *
 * For efficiency, we include a chunk of "calc_bonuses()".
 */
/*:::�X�e�[�^�X�����[������p_ptr->stat_cur[],stat_max[]�Ɋi�[����*/
/*:::�X�e�[�^�X��8�`17 �p�����[�^���v��73�`86�͈͓̔�*/
static void get_stats(void)
{
	/* Roll and verify some stats */
	while (TRUE)
	{
		int i;
		int sum = 0;

		/* Roll some dice */
		for (i = 0; i < 2; i++)
		{
			s32b tmp = randint0(60*60*60);
			int val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i] = p_ptr->stat_max[3*i] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp % 5; tmp /= 5;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i+1] = p_ptr->stat_max[3*i+1] = val;

			/* Extract 5 + 1d3 + 1d4 + 1d5 */
			val = 5 + 3;
			val += tmp % 3; tmp /= 3;
			val += tmp % 4; tmp /= 4;
			val += tmp;

			/* Save that value */
			sum += val;
			p_ptr->stat_cur[3*i+2] = p_ptr->stat_max[3*i+2] = val;
		}

		/* Verify totals */
		if ((sum > 42+5*6) && (sum < 57+5*6)) break;
		/* 57 was 54... I hate 'magic numbers' :< TY */
	}
}

/*:::��{�X�e�[�^�X�㏸�ő�l(18/70-18/130)�𓾂�*/
/*:::�ő�l��p_ptr->stat_max_max[]�Ɋi�[����A���̃p�����[�^������ȏ�Ȃ猻�݃p�����[�^����������*/

///mod140104 �p�����[�^����ƕ\�L��ύX�@�ő�l�͏��30(��18/120)�ɂȂ�悤�ɂ���
void get_max_stats(void)
{
	int		i, j;

	int		dice[6];
#if 0
	/* Roll and verify some stats */
	/*:::1-7�𓾂�B���v��24�ɂȂ�悤�ɁB4�̂Ƃ��ő�l��18/100�ɂȂ�B*/
	while (TRUE)
	{
		/* Roll some dice */
		for (j = i = 0; i < 6; i++)
		{
			/* Roll the dice */
			dice[i] = randint1(7);

			/* Collect the maximum */
			j += dice[i];
		}

		/* Verify totals */
		if (j == 24) break;
	}

	/* Acquire the stats */
	for (i = 0; i < 6; i++)
	{
		j = 18 + 60 + dice[i]*10;

		/* Save that value */
		p_ptr->stat_max_max[i] = j;
		if (p_ptr->stat_max[i] > j)
			p_ptr->stat_max[i] = j;
		if (p_ptr->stat_cur[i] > j)
			p_ptr->stat_cur[i] = j;
	}
#endif
	/*:::�V�����ő�l��K�p�B�V���̂Ƃ�stat_max���V�����ő�l�𒴂��邱�Ƃ͂����Ȃ��͂�*/
	for(i=0;i<6;i++) p_ptr->stat_max_max[i] = STAT_MAX_MAX;

	p_ptr->knowledge &= ~(KNOW_STAT);

	/* Redisplay the stats later */
	p_ptr->redraw |= (PR_STATS);
}


/*
 * Roll for some info that the auto-roller ignores
 */
/*:::�L�������C�N�Ɋւ��邢�����̓��ꎖ�������s*/
///race class seikaku
///system �L�������C�N�ɖ��ڂɊ֌W
static void get_extra(bool roll_hitdie)
{
	int i, j;

	/* Experience factor */
	/*:::exp�y�i���e�B���v�Z*/
	if (p_ptr->prace == RACE_ANDROID) p_ptr->expfact = rp_ptr->r_exp;
	else p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;

	if(EXTRA_MODE && p_ptr->prace != RACE_ANDROID) p_ptr->expfact /= 2;

	//if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_NINJA)) && ((p_ptr->prace == RACE_KLACKON) || (p_ptr->prace == RACE_SPRITE)))
	//	p_ptr->expfact -= 15;

	/* Reset record of race/realm changes */
	/*:::�u�ȑO�́`�������v�I���̃N���A*/
	p_ptr->start_race = p_ptr->prace;
	p_ptr->old_race1 = 0L;
	p_ptr->old_race2 = 0L;
	p_ptr->old_realm = 0;

	/*:::�X�y�}�X�ƐԖ��̌o���l����*/
	for (i = 0; i < 64; i++)
	{
		if (p_ptr->pclass == CLASS_SORCERER) p_ptr->spell_exp[i] = SPELL_EXP_MASTER;
		else if (p_ptr->pclass == CLASS_RED_MAGE) p_ptr->spell_exp[i] = SPELL_EXP_SKILLED;
		else p_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}
	///skill ����Z�\
/*
	for (i = 0; i < 5; i++)
		for (j = 0; j < 64; j++)
			p_ptr->weapon_exp[i][j] = s_info[p_ptr->pclass].w_start[i][j];
	if ((p_ptr->pseikaku == SEIKAKU_SEXY) && (p_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] < WEAPON_EXP_BEGINNER))
	{
		p_ptr->weapon_exp[TV_HAFTED-TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_BEGINNER;
	}
*/
	///mod131226 skill �Z�\�ƕ���Z�\�̓���
	///mod140130 skill �Z�\�ƕ���Z�\�����l�ݒ�
	//for (i = 0; i < 10; i++)
		//p_ptr->skill_exp[i] = s_info[p_ptr->pclass].s_start[i];

	for (i = 0; i < SKILL_EXP_MAX; i++)
	{
		switch(cp_ptr->skill_aptitude[i])
		{
		case 0: case 1: case 2:
			p_ptr->skill_exp[i] =  0;
			break;
		case 3:
			p_ptr->skill_exp[i] =  800;
			break;
		case 4:
			p_ptr->skill_exp[i] =  1600;
			break;
		case 5:
			p_ptr->skill_exp[i] =  2400;
			break;
		default:
			msg_format("ERROR:skill_aptitude[]���s���Ȓl");
		}
		/*:::���t�͍ŏ�����Z�\������*/
		if(p_ptr->pclass == CLASS_TEACHER) p_ptr->skill_exp[i] =  2400;
		/*:::���͐킢�̐S�����Ȃ�*/
		if(p_ptr->pclass == CLASS_SHOU) p_ptr->skill_exp[i] =  0;
		//���g�͋Z�\������(�e����)
		if(p_ptr->pclass == CLASS_MOKOU && i != 20) p_ptr->skill_exp[i] =  3200;

	}

	///mod150824 �v���Y�����o�[�O�o�����ꏈ���@�n�[�h�R�[�f�B���O�����@prism_change_class()�Q��
	if(CLASS_IS_PRISM_SISTERS)
	{
		//����őҋ@���Ă�o���̕��̏����Z�\�l�ݒ�
		for(i=0;i<3;i++)
		{
			for (j = 0; j < SKILL_EXP_MAX; j++)
			{
				int init_skill = MAX(0,(class_info[CLASS_LUNASA+i].skill_aptitude[j]-2)) * 800;
				p_ptr->magic_num1[3 + j + SKILL_EXP_MAX * i] = init_skill;
			}
		}
		//���b�L�[�K�[���̂Ƃ��S���ɕψكt���O�t�^
		if(p_ptr->pseikaku == SEIKAKU_LUCKY)
		{
			p_ptr->magic_num2[14] = 0x80;
			p_ptr->magic_num2[26] = 0x80;
			p_ptr->magic_num2[38] = 0x80;
			p_ptr->magic_num2[50] = 0x80;
			//v1.1.15 66��62
			p_ptr->magic_num2[62] = 0x80;
			p_ptr->magic_num2[74] = 0x80;
		}
	}


	/* Hitdice */
	/*:::HD�v�Z*/
	//v1.1.32 �p�`�����[���ꐫ�i
	if (p_ptr->pclass == CLASS_SORCERER || is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI))
		p_ptr->hitdie = rp_ptr->r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
	else
		p_ptr->hitdie = rp_ptr->r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;

	/* Roll for hit point unless quick-start */
	/*:::HP�v�Z*/
	if (roll_hitdie) do_cmd_rerate_aux();

	/* Initial hitpoints */
	p_ptr->mhp = p_ptr->player_hp[0];
}


/*
 * Get the racial history, and social class, using the "history charts".
 */
///sysdel race
/*:::���������f�t�H���g�ƎЉ�I�n�ʂ𓾂�B*/
static void get_history(void)
{
	int i, n, chart, roll, social_class;

	char *s, *t;

	char buf[240];

	///tmp131221 �Љ�I�n�ʂƃf�t�H���g�����������͂��Œ�
	p_ptr->sc = 50;
	strcpy(p_ptr->history[0], "�L�����N�^�[�̐��������⌈�ӕ\���ȂǍD���ɏ����Ă�������");
	return;
///del140105 ���������Ɏ푰���Ƃ̐��������f�t�H���g���菈�������������푰�ύX���ʓ|�Ȃ̂őS��������


}


/*
 * Get character's height and weight
 */
void get_height_weight(void)
{
	int h_percent; /* �g�������ςɂ���ׂĂǂ̂��炢�Ⴄ��. */

	/* Calculate the height/weight for males */
	if (p_ptr->psex == SEX_MALE)
	{
		p_ptr->ht = randnor(rp_ptr->m_b_ht, rp_ptr->m_m_ht);
		h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->m_b_ht);
		p_ptr->wt = randnor((int)(rp_ptr->m_b_wt) * h_percent /100
				    , (int)(rp_ptr->m_m_wt) * h_percent / 300 );
	}
  
	/* Calculate the height/weight for females */
	else if (p_ptr->psex == SEX_FEMALE)
	{
		p_ptr->ht = randnor(rp_ptr->f_b_ht, rp_ptr->f_m_ht);
		h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->f_b_ht);
		p_ptr->wt = randnor((int)(rp_ptr->f_b_wt) * h_percent /100
				    , (int)(rp_ptr->f_m_wt) * h_percent / 300 );
	}
}


/*
 * Computes character's age, height, and weight
 * by henkma
 */
///sysdel
/*:::�N��A�g���A�̏d�𓾂�p_ptr->age,p_ptr->ht,p_ptr->wt�Ɋi�[����B���ʂƎ푰���֌W����B*/
static void get_ahw(void)
{
	/* Get character's age */
	//v1.1.70 ������̏������͌���p�����̑���Ƃ��Ă�����g���Ă���
	p_ptr->age = rp_ptr->b_age + randint1(rp_ptr->m_age);

	/* Get character's height and weight */
	get_height_weight();
}


/*
 * Get the player's starting money
 */
///class system seikaku
/*:::�����������@�X�e�[�^�X�������ƌ���炵��*/
static void get_money(void)
{
	int i, gold;

	/* Social Class determines starting gold */
	gold = 1000 + randint1(1000);
	if (p_ptr->pclass == CLASS_CIVILIAN)  gold += gold*2 + randint0(2);
	else if (p_ptr->pclass == CLASS_RAN)  gold += gold*3 + randint0(3);


	while(one_in_(7)) gold *= 2;

	if(p_ptr->pclass == CLASS_SHOU) gold *= 3;

	///mod140104 �p�����[�^�\�L�ύX�ɔ����A���p�����[�^�ŏ����������鏈�����Ƃ肠����������
#if 0
	/* Process the stats */
	for (i = 0; i < 6; i++)
	{
		/* Mega-Hack -- reduce gold for high stats */
		if (p_ptr->stat_max[i] >= 18 + 50) gold -= 300;
		else if (p_ptr->stat_max[i] >= 18 + 20) gold -= 200;
		else if (p_ptr->stat_max[i] > 18) gold -= 150;
		else gold -= (p_ptr->stat_max[i] - 8) * 10;
	}
#endif
	/* Minimum 100 gold */
	//if (gold < 100) gold = 100;

	if(p_ptr->pclass == CLASS_OUTSIDER) gold = 0;
/*
	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		gold /= 2;
	else 
*/		
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
		gold = 10000000;
	//if (p_ptr->prace == RACE_ANDROID) gold /= 5;


	if(p_ptr->pclass == CLASS_TEWI) gold = 1000000;
	else if (p_ptr->pclass == CLASS_SHION) gold = -99999999;
	else if (is_special_seikaku(SEIKAKU_SPECIAL_MEGUMU)) gold = MAX(gold,(int)(total_pay_cardshop / 4));


	/* Save the gold */
	p_ptr->au = gold;
}



/*
 * Display stat values, subset of "put_stats()"
 *
 * See 'display_player()' for screen layout constraints.
 */
/*:::�I�[�g���[���[���A����I�Ɍ��݂̃X�e�[�^�X����ʂ̌��܂����ʒu�ɕ\������*/
static void birth_put_stats(void)
{
	int i, j, m, p;
	int col;
	byte attr;
	char buf[80];


	if (autoroller)
	{
		col = 42;
		/* Put the stats (and percents) */
		for (i = 0; i < 6; i++)
		{
			/* Race/Class bonus */
			j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

			/* Obtain the current stat */
			m = adjust_stat(p_ptr->stat_max[i], j);

			/* Put the stat */
			cnv_stat(m, buf);
			c_put_str(TERM_L_GREEN, buf, 3+i, col+24);

			/* Put the percent */
			if (stat_match[i])
			{
				if (stat_match[i] > 1000000L)
				{
					/* Prevent overflow */
					p = stat_match[i] / (auto_round / 1000L);
				}
				else
				{
					p = 1000L * stat_match[i] / auto_round;
				}
			
				attr = (p < 100) ? TERM_YELLOW : TERM_L_GREEN;
				sprintf(buf, "%3d.%d%%", p/10, p%10);
				c_put_str(attr, buf, 3+i, col+13);
			}

			/* Never happened */
			else
			{
#ifdef JP
				c_put_str(TERM_RED, "(�Ȃ�)", 3+i, col+13);
#else
				c_put_str(TERM_RED, "(NONE)", 3+i, col+13);
#endif

			}
		}
	}
}


static void k_info_reset(void)
{
	int i;

	/* Reset the "objects" */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Reset "tried" */
		k_ptr->tried = FALSE;

		/* Reset "aware" */
		k_ptr->aware = FALSE;
	}
}


/*
 * Clear all the global "character" data
 */
/*:::���̑S��񏉊���*/
//�����Z�[�u�f�[�^�Ɏc��悤�ȕϐ��𑝂₵���炱���ŖY�ꂸ����������
static void player_wipe(void)
{
	int i;

	/*:::���b�Z�[�W�����N���A*/
	/* Hack -- free the "last message" string */
	if (p_ptr->last_message) string_free(p_ptr->last_message);

	/*:::p_ptr�̃N���A*/
	/* Hack -- zero the struct */
	(void)WIPE(p_ptr, player_type);

	/* Wipe the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(p_ptr->history[i], "");
	}

	/*:::�N�G�X�g��A�C�e���Ȃǁ��ɂ������O���[�o���ϐ��̏�����*/
	/* Wipe the quests */
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = QUEST_STATUS_UNTAKEN;

		quest[i].cur_num = 0;
		quest[i].max_num = 0;
		quest[i].type = 0;
		quest[i].level = 0;
		quest[i].r_idx = 0;
		quest[i].complev = 0;
		//v1.1.25
		quest[i].comptime = 0;
	}

	/* No weight */
	p_ptr->total_weight = 0;

	/* No items */
	inven_cnt = 0;
	equip_cnt = 0;

	/* Clear the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&inventory[i]);
	}

	/* Clear the inventory2 */
	for (i = 0; i < INVEN_ADD_MAX; i++)
	{
		object_wipe(&inven_add[i]);
	}
	/* Clear the inventory3 */
	///mod160212 ��������inven_add�����������Ă��B�C��
	for (i = 0; i < INVEN_SPECIAL_MAX; i++)
	{
		object_wipe(&inven_special[i]);
	}

	/* Start with no artifacts made yet */
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		a_ptr->cur_num = 0;
	}

	//EX�����p�����[�^
	for(i=0;i<EX_BUILDINGS_PARAM_MAX;i++) ex_buildings_param[i] = 0;
	for(i=0;i<EX_BUILDINGS_HISTORY_MAX;i++) ex_buildings_history[i] = 0;

	/* Reset the objects */
	k_info_reset();

	/* Reset the "monsters" */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Hack -- Reset the counter */
		r_ptr->cur_num = 0;

		/* Hack -- Reset the max counter */
		r_ptr->max_num = 100;

		/* Hack -- Reset the max counter */
		if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 1;

		/* Hack -- Non-unique Nazguls are semi-unique */
		else if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num = MAX_NAZGUL_NUM;

		/* Clear visible kills in this life */
		r_ptr->r_pkills = 0;

		/* Clear all kills in this life */
		r_ptr->r_akills = 0;

		//v1.1.30 �����_�����j�[�N������������B
		//���������Ȃ��Ă��������ɐݒ肳��邩����Ȃ����낤�Ǝv���Ă����˕P�̃����_���_�~�낵�őO��Q�[���̃����_�����j�[�N�̋Z���o�Ă���
		if(IS_RANDOM_UNIQUE_IDX(i))
		{
			int j;
			my_strcpy(random_unique_name[i - MON_RANDOM_UNIQUE_1],"wiped_random_unique",sizeof(random_unique_name[i - MON_RANDOM_UNIQUE_1])-2);
			r_ptr->hdice = 1;
			r_ptr->hside = 1;
			r_ptr->ac = 10;
			r_ptr->sleep = 10;
			r_ptr->aaf = 10;
			r_ptr->speed = 110;
			r_ptr->mexp = 1;
			r_ptr->extra = 0;
			r_ptr->freq_spell = 0;
			r_ptr->flags1 = (RF1_FORCE_MAXHP);
			r_ptr->flags2 = 0L;
			r_ptr->flags3 = 0L;
			r_ptr->flags4 = 0L;
			r_ptr->flags5 = 0L;
			r_ptr->flags6 = 0L;
			r_ptr->flags7 = (RF7_UNIQUE2 | RF7_VARIABLE);
			if(i == MON_RANDOM_UNIQUE_3) r_ptr->flags7 |= (RF7_AQUATIC);
			r_ptr->flags8 = 0L;
			r_ptr->flags9 = 0L;
			r_ptr->flagsr = 0L;

			for(j=0;j<3;j++)
			{
				r_ptr->blow[j].method = RBM_HIT;
				r_ptr->blow[j].effect = RBE_HURT;
				r_ptr->blow[j].d_dice = 1;
				r_ptr->blow[j].d_side = 1;
			}
			r_ptr->level = 1;
			r_ptr->rarity = 1;
			r_ptr->r_flags1 = 0L;
			r_ptr->r_flags2 = 0L;
			r_ptr->r_flags3 = 0L;
			r_ptr->r_flags4 = 0L;
			r_ptr->r_flags5 = 0L;
			r_ptr->r_flags6 = 0L;
			r_ptr->r_flags9 = 0L;
			r_ptr->r_flagsr = 0L;
		}

	}

	/* Hack -- Well fed player */
	p_ptr->food = PY_FOOD_FULL - 1;


	/* Wipe the spells */
	///class �X�y�}�X���ŏ�����S�����K���ς݂ɂȂ��Ă鏈��
	if (p_ptr->pclass == CLASS_SORCERER)
		///bug �����̍s�L���ɂ�����V�K�v���C���Ƀt���[�Y�@���̒i�K�ł�cp_ptr���L���łȂ������炵���B�ƂȂ�Ɠ��ꖂ�@�K���E�͂ǂ��Ŕ��肵�悤�B
	//if (REALM_SPELLMASTER)
	{
		p_ptr->spell_learned1 = p_ptr->spell_learned2 = 0xffffffffL;
		p_ptr->spell_worked1 = p_ptr->spell_worked2 = 0xffffffffL;
	}
	else
	{
		p_ptr->spell_learned1 = p_ptr->spell_learned2 = 0L;
		p_ptr->spell_worked1 = p_ptr->spell_worked2 = 0L;
	}
	p_ptr->spell_forgotten1 = p_ptr->spell_forgotten2 = 0L;
	for (i = 0; i < 64; i++) p_ptr->spell_order[i] = 99;
	p_ptr->learned_spells = 0;
	p_ptr->add_spells = 0;
	p_ptr->knowledge = 0;

	/* Clean the mutation count */
	mutant_regenerate_mod = 100;

	/* Clear "cheat" options */
	cheat_peek = FALSE;
	cheat_hear = FALSE;
	cheat_room = FALSE;
	cheat_xtra = FALSE;
	cheat_know = FALSE;
	cheat_live = FALSE;
	cheat_save = FALSE;

	/* Assume no winning game */
	p_ptr->total_winner = FALSE;

	world_player = FALSE;

	/* Assume no panic save */
	p_ptr->panic_save = 0;

	/* Assume no cheating */
	p_ptr->noscore = 0;
	p_ptr->wizard = FALSE;

	/* Not waiting to report score */
	p_ptr->wait_report_score = FALSE;

	/* Default pet command settings */
	p_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	p_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_SUMMON_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_d_idx; i++)
	{
		max_dlv[i] = 0;
		flag_dungeon_complete[i] = 0;
	}

	p_ptr->visit = 1;

	/* Reset wild_mode to FALSE */
	p_ptr->wild_mode = FALSE;

	//v1.1.94 magic_num�T�C�Y�ύX
	for (i = 0; i < MAGIC_NUM_SIZE; i++)
	{
		p_ptr->magic_num1[i] = 0;
		p_ptr->magic_num2[i] = 0;
	}

	/* Level one */
	p_ptr->max_plv = p_ptr->lev = 1;

	///mod140423
	p_ptr->silent_floor = 0;
	p_ptr->maripo = 0;
	p_ptr->maripo_presented_flag = 0L;

	///mod140502
	p_ptr->lucky = 0;

	///mod140813
	p_ptr->asthma = 0;
	for (i = 0; i < 5; i++)p_ptr->race_multipur_val[i] = 0;

	/* Initialize arena and rewards information -KMW- */
	p_ptr->kourindou_number = 0;
	p_ptr->inside_arena = FALSE;
	p_ptr->inside_quest = 0;
	///class ���̂܂ˎt�̏�����
	for (i = 0; i < MAX_MANE; i++)
	{
		p_ptr->mane_spell[i] = -1;
		p_ptr->mane_dam[i] = 0;
	}
	p_ptr->mane_num = 0;
	p_ptr->exit_bldg = TRUE; /* only used for arena now -KMW- */

	/* Bounty */
	p_ptr->today_mon = 0;

	/* Reset monster arena */
	///sysdel tougi
	//battle_monsters();

	/* Reset mutations */
	p_ptr->muta1 = 0;
	p_ptr->muta2 = 0;
	p_ptr->muta3 = 0;
	p_ptr->muta1_perma = 0;
	p_ptr->muta2_perma = 0;
	p_ptr->muta3_perma = 0;
	//v1.1.64
	p_ptr->muta4 = 0;
	p_ptr->muta4_perma = 0;

	/* Reset virtues*/
	for (i = 0; i < 8; i++)
	{
			p_ptr->virtues[i]=0;
			p_ptr->vir_types[i] = 0;
	}

	/* Set the recall dungeon accordingly */
	if (vanilla_town)
	{
		dungeon_type = 0;
		p_ptr->recall_dungeon = DUNGEON_ANGBAND;
	}
	else
	{
		dungeon_type = 0;
		p_ptr->recall_dungeon = DUNGEON_TAISHI;
	}

	//v1.1.19
	p_ptr->monkey_count = 0;

	//v1.1.25
	score_bonus_kill_unique2 = 0L;

	//v1.1.52 �������ɒǉ����Y��Ă�
	qrkdr_rec_turn = 0;
	gambling_gain = 0;
	score_count_pinch = 0;
	score_count_pinch2 = 0;
	nightmare_diary_count = 0; 
	nightmare_diary_win = 0;	
	nightmare_total_point = 0;

	//v1.1.56
	p_ptr->tim_spellcard_count = 0;
	p_ptr->spellcard_effect_idx = 0;

	//v1.1.87

	ability_card_trade_count = 0;
	buy_gacha_box_count = 0;


}

/*
 *  Hook function for quest monsters
 */
/*:::�����N�G�I��phook*/
static bool mon_hook_quest(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Random quests are in the dungeon */
	if (r_ptr->flags8 & RF8_WILD_ONLY) return FALSE;

	/* No random quests for aquatic monsters */
	if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	/* No random quests for multiplying monsters */
	if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;

	/* No quests to kill friendly monsters */
	if (r_ptr->flags7 & RF7_FRIENDLY) return FALSE;

	/*::: Hack ���Y�O�X�l�̓����N�G�ɏo�Ȃ�*/
	if(r_idx == MON_MOZGUS || r_idx == MON_MOZGUS2) return FALSE;

	//v1.1.42 �����̓����N�G�ɏo�Ȃ�
	if (r_idx == MON_SHION_1 || r_idx == MON_SHION_2) return FALSE;



	return TRUE;
}


/*
 * Determine the random quest uniques
 */
/*:::�����N�G�p�����X�^�[��I�肷��*/
///system �����N�G�G��Ȃ炱�̕�
void determine_random_questor(quest_type *q_ptr)
{
	int          r_idx;
	monster_race *r_ptr;

	/* Prepare allocation table */
	get_mon_num_prep(mon_hook_quest, NULL);

	while (1)
	{
		/*
		 * Random monster 5 - 10 levels out of depth
		 * (depending on level)
		 */
		r_idx = get_mon_num(q_ptr->level + 5 + randint1(q_ptr->level / 10));
		r_ptr = &r_info[r_idx];

		if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

		///mod140301 �����n����
		/*:::�����_���W�������w�肳��Ă郂���X�^�[�͏o�Ȃ����Ƃɂ���*/
		if(r_ptr->extra != 0) continue;
		///mod140610 ���q�ˎ҃N�G�X�g
		/*:::���q�ˎ҂������N�G�ΏۂɂȂ�Ȃ��悤�ɐݒ肷��*/
		if(r_ptr->flags3 & RF3_WANTED) continue;
		if (r_ptr->flags1 & RF1_QUESTOR) continue;

		//2.0.13 ���N�U�푈2�N�G�X�g�ŏ܋���ݒ肳��邩������Ȃ��O�l�̓����N�G�ɑI�΂�Ȃ�
		if (r_idx == MON_ENOKO) continue;
		if (r_idx == MON_BITEN) continue;
		if (r_idx == MON_CHIYARI) continue;

		//v1.1.59 �j���ېV���i�@�������j�[�N�������N�G�ɑI�΂�Ȃ�
		if (is_special_seikaku(SEIKAKU_SPECIAL_SHINMYOUMARU) && is_gen_unique(r_idx)) continue;

		///mod140721 ���Փx�ł͏��ՃN�G�X�g�ɋ��G���o�Ȃ�
		if(q_ptr->level < 45 && difficulty <= DIFFICULTY_NORMAL && (r_ptr->flags1 & RF1_FLAG_50F))	continue;

		if (r_ptr->rarity > 100) continue;

		if (r_ptr->flags7 & RF7_FRIENDLY) continue;
		if (r_ptr->flags3 & RF3_ANG_FRIENDLY) continue;
		if (r_ptr->flags3 & RF3_GEN_FRIENDLY) continue;

		if (r_ptr->flags7 & RF7_AQUATIC) continue;

		if (r_ptr->flags8 & RF8_WILD_ONLY) continue;

		if (no_questor_or_bounty_uniques(r_idx)) continue;



		/*
		 * Accept monsters that are 2 - 6 levels
		 * out of depth depending on the quest level
		 */
		if (r_ptr->level > (q_ptr->level + (q_ptr->level / 20))) break;
	}

	//test v2.0.13 �ڍׂ�castle_quest�̖������o�Omemo
	//if (q_ptr->level == 38) r_idx = MON_BITEN;

	q_ptr->r_idx = r_idx;
}


/*
 *  Initialize random quests and final quests
 */
/*:::�N�G�X�g�̏������@�����N�G�̃����X�^�[�����肵�A�I�x�����ƃT�[�y���g�̃N�G�X�g��i�s����Ԃɂ��Ă���*/
static void init_dungeon_quests(void)
{
	//int number_of_quests = MAX_RANDOM_QUEST - MIN_RANDOM_QUEST + 1;
	///mod140305 50F�����N�G�𖳂�����
	int number_of_quests = 9;
	int i;

	/* Init the random quests */
	init_flags = INIT_ASSIGN;
	p_ptr->inside_quest = MIN_RANDOM_QUEST;

	/*:::���̒���q_pref.txt���ǂݍ��܂��*/
	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	p_ptr->inside_quest = 0;

	/* Generate quests */
	//quest[40]-[48]�������N�G�Ɏg����
	for (i = MIN_RANDOM_QUEST + number_of_quests - 1; i >= MIN_RANDOM_QUEST; i--)
	{
		quest_type      *q_ptr = &quest[i];
		monster_race    *quest_r_ptr;

		q_ptr->status = QUEST_STATUS_TAKEN;

		if(EXTRA_MODE) continue; //Extra���[�h�ł̓����N�G���ʃN�G�X�g�ɂȂ�̂Ŏ�̂������ă^�[�Q�b�g�����X�^�[�͌��߂Ȃ�

		///quest ����������������K�v�A��
		determine_random_questor(q_ptr);

		/* Mark uniques */
		quest_r_ptr = &r_info[q_ptr->r_idx];
		quest_r_ptr->flags1 |= RF1_QUESTOR;

		q_ptr->max_num = 1;
	}

	if(EXTRA_MODE)//Extra���[�h�ł͍ŏ�����I�x�����N�G�X�g��̂��A���N�G�X�g�͖���́@�ق��N�G�X�g�������������Ȃ�
	{
		init_flags = INIT_ASSIGN;
		p_ptr->inside_quest = QUEST_TAISAI;
		process_dungeon_file("q_info.txt", 0, 0, 0, 0);
		quest[QUEST_TAISAI].status = QUEST_STATUS_TAKEN;
	}
	else
	{
		/*:::Mega Hack �ꕔ���j�[�N�N���X�͓���N�G�X�g���ŏ���������I�Ɏ��s�ɂȂ�*/
		for (i = 1; i < max_quests; i++)
		{
			if(check_ignoring_quest(i)) quest[i].status = QUEST_STATUS_FAILED_DONE;
		}
		/* Init the two main quests (Oberon + Serpent) */
		init_flags = INIT_ASSIGN;
		//p_ptr->inside_quest = QUEST_TAISAI;
		p_ptr->inside_quest = QUEST_YUKARI;

		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		//quest[QUEST_TAISAI].status = QUEST_STATUS_TAKEN;
		quest[QUEST_YUKARI].status = QUEST_STATUS_TAKEN;

		/*:::Hack - ���͎��N�G�X�g�������ς�(�X�R�A�ɂ͎Q�����Ȃ�)*/
		///mod160213 ���������ɂ���
		if(p_ptr->pclass == CLASS_RAN || p_ptr->pclass == CLASS_YUKARI)
		{
			quest[QUEST_YUKARI].status = QUEST_STATUS_FINISHED;
			p_ptr->inside_quest = QUEST_TAISAI;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			quest[QUEST_TAISAI].status = QUEST_STATUS_TAKEN;

		}
		/*:::Hack - ��������p�N�G�X�g*/
		else if(p_ptr->pclass == CLASS_MARISA)
		{
			p_ptr->inside_quest = QUEST_MOON_VAULT;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			quest[QUEST_MOON_VAULT].status = QUEST_STATUS_TAKEN;

			p_ptr->inside_quest = QUEST_MARICON;
			init_flags |= INIT_ASSIGN;
			process_dungeon_file("q_info.txt", 0, 0, 0, 0);
			quest[QUEST_MARICON].status = QUEST_STATUS_TAKEN;
		}
	}

	//p_ptr->inside_quest = QUEST_SERPENT;
	//process_dungeon_file("q_info.txt", 0, 0, 0, 0);
	//quest[QUEST_SERPENT].status = QUEST_STATUS_TAKEN;
	p_ptr->inside_quest = 0;
}

/*
 * Reset turn
 */
/*:::�^�[���͈���ڂ̒�6������J�n�@�A���f�b�h��0�����炾�������I�ɂ͑����^�[�����i��ŊJ�n�炵��*/
static void init_turn(void)
{
#if 0
	if ((p_ptr->prace == RACE_VAMPIRE) ||
	    (p_ptr->prace == RACE_SKELETON) ||
	    (p_ptr->prace == RACE_ZOMBIE) ||
	    (p_ptr->prace == RACE_SPECTRE))
	{
		/* Undead start just after midnight */
		/*:::�A���f�b�h��0������J�n�@�^�[����18���Ԑi�߂�@*/
		turn = (TURNS_PER_TICK*3 * TOWN_DAWN) / 4 + 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * MAX_DAYS + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
	else
	{
		turn = 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
#endif
	///mod1401 �A���f�b�h���邩��J�n����d�l�𖳂�����
	turn = 1;
	turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;

	dungeon_turn = 1;
	dungeon_turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
}


/* 
 * Try to wield everything wieldable in the inventory. 
 * Code taken from Angband 3.1.0 under Angband license
 */ 
/*:::�C���x���g���ɂ���A�C�e���S�Ă𒲂ׁA�����ł�����̂𑕔�����@�����܂����@player_outfit()����̂݌Ă΂��*/
static void wield_all(void) 
{ 
	object_type *o_ptr; 
	object_type *i_ptr; 
	object_type object_type_body; 
 
	int slot; 
	int item; 
 
	/* Scan through the slots backwards */ 
	for (item = INVEN_PACK - 1; item >= 0; item--) 
	{ 
		o_ptr = &inventory[item]; 
 
		/* Skip non-objects */ 
		if (!o_ptr->k_idx) continue; 
 
		/*:::�����i�łȂ����́A�����A���łɉ����������Ă���ꏊ�̓p�X*/
		/* Make sure we can wield it and that there's nothing else in that slot */ 
		if(p_ptr->pclass == CLASS_3_FAIRIES) slot = wield_slot_3_fairies(o_ptr); 
		else	slot = wield_slot(o_ptr); 

		if (slot < INVEN_RARM) continue; 
		if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_TORCH && !object_is_artifact(o_ptr) ) continue; /* Does not wield toaches because buys a lantern soon */
		if (inventory[slot].k_idx) continue; 
 

		/* Get local object */ 
		i_ptr = &object_type_body; 
		object_copy(i_ptr, o_ptr); 
 
		/* Modify quantity */ 
		i_ptr->number = 1; 
 
		/* Decrease the item (from the pack) */ 
		if (item >= 0) 
		{ 
			inven_item_increase(item, -1); 
			inven_item_optimize(item); 
		} 
 
		/* Decrease the item (from the floor) */ 
		else 
		{ 
			floor_item_increase(0 - item, -1); 
			floor_item_optimize(0 - item); 
		} 
 
		/* Get the wield slot */ 
		o_ptr = &inventory[slot]; 
 
		/* Wear the new stuff */ 
		object_copy(o_ptr, i_ptr); 
 
		/* Increase the weight */ 
		/*:::�Ȃ񂩂����ŏd�ʉ��Z����̂͂��������C������B�܂��ǂ����قǂȂ��Čv�Z�����Ǝv����*/
		p_ptr->total_weight += i_ptr->weight; 
 
		/* Increment the equip counter by hand */ 
		equip_cnt++;

 	} 
	return; 
} 


/*
 * Each player starts out with a few items, given as tval/sval pairs.
 * In addition, he always has some food and a few torches.
 */
/*:::�N���X���Ƃ̏��������A�C�e���ꗗ*/
///class item �N���X���Ƃ̏��������A�C�e���ꗗ
#if 0
static byte player_init[MAX_CLASS][3][2] =
{
	{
		/* Warrior */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Priest */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for Life / Death book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_HAFTED, SV_MACE }
	},

	{
		/* Rogue */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Ranger */
		{ TV_NATURE_BOOK, 0 },
		{ TV_DEATH_BOOK, 0 },		/* Hack: for realm2 book */
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Paladin */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SCROLL, SV_SCROLL_PROTECTION_FROM_EVIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Warrior-Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_DEATH_BOOK, 0 }, /* Hack: for realm2 book */
		{ TV_SWORD, SV_SHORT_SWORD }
	},

	{
		/* Chaos Warrior */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: For realm1 book */
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* Monk */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_HEROISM }
	},

	{
		/* Mindcrafter */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SMALL_SWORD }
	},

	{
		/* High Mage */
		{ TV_SORCERY_BOOK, 0 }, /* Hack: for realm1 book */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Tourist */
		{ TV_FOOD, SV_FOOD_JERKY},
		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_BOW, SV_SLING}
	},

	{
		/* Imitator */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SHORT_SWORD}
	},

	{
		/* Beastmaster */
		{ TV_TRUMP_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POLEARM, SV_SPEAR}
	},

	{
		/* Sorcerer */
		{ TV_HAFTED, SV_WIZSTAFF }, /* Hack: for realm1 book */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_WAND, SV_WAND_MAGIC_MISSILE }
	},

	{
		/* Archer */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Magic eater */
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Bard */
		{ TV_MUSIC_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Red Mage */
		{ TV_ARCANE_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
	},

	{
		/* Samurai */
		{ TV_HISSATSU_BOOK, 0 },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD }
	},

	{
		/* ForceTrainer */
		{ TV_SORCERY_BOOK, 0 },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_RESTORE_MANA }
	},

	{
		/* Blue Mage */
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SWORD, SV_DAGGER }
	},

	{
		/* Cavalry */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_POLEARM, SV_BROAD_SPEAR}
	},

	{
		/* Berserker */
		{ TV_POTION, SV_POTION_HEALING },
		{ TV_HARD_ARMOR, SV_AUGMENTED_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE }
	},

	{
		/* Weaponsmith */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE }
	},
	{
		/* Mirror-Master */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER }
	},
	{
		/* Ninja */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},
	{
		/* Sniper */
		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER }
	},
};
#endif

/*
 * Hook function for human corpses
 */
static bool monster_hook_human(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;

	if (my_strchr("pht", r_ptr->d_char)) return TRUE;

	return FALSE;
}


/*
 * Add an outfit object
 */
/*:::player_outfit����Ă΂��B�w��A�C�e�����Ӓ肵�U�b�N�ɒǉ����\�Ȃ瑕������*/
static void add_outfit(object_type *o_ptr)
{
	s16b slot;

	object_aware(o_ptr);
	object_known(o_ptr);
	///mod140821 ���������i�͑S��*�Ӓ�*�����悤�ɏC��
	o_ptr->ident |= (IDENT_MENTAL);
	slot = inven_carry(o_ptr);

	/* Auto-inscription */
	autopick_alter_item(slot, FALSE);

	/* Now try wielding everything */ 
	wield_all(); 
}


/*:::player_outfit()����g�����������A�C�e��*/
/*:::�푰���ƊJ�n�������i�@�H����V��̓��݂Ȃ�*/
outfit_type birth_outfit_race[] = {
	{RACE_KARASU_TENGU,2,0,TV_HEAD,SV_HEAD_TOKIN,1},
	{RACE_HAKUROU_TENGU,2,0,TV_HEAD,SV_HEAD_TOKIN,1},

	{RACE_HUMAN,2,0,TV_FOOD,SV_FOOD_RATION,5},
	{RACE_YOUKAI,2,0,TV_FOOD,SV_FOOD_RATION,7},
	{RACE_HALF_YOUKAI,2,0,TV_FOOD,SV_FOOD_RATION,2},
	{RACE_SENNIN,2,0,TV_FOOD,SV_FOOD_SENTAN,3},
	{RACE_TENNIN,2,0,TV_SWEETS,SV_SWEETS_PEACH,6},
	{RACE_FAIRY,2,0,TV_SWEETS,SV_SWEET_COOKIE,4},
	{RACE_KAPPA,2,0,TV_FOOD,SV_FOOD_CUCUMBER,5},
	{RACE_KARASU_TENGU,2,0,TV_FOOD,SV_FOOD_HARDBUSC,6},
	{RACE_HAKUROU_TENGU,2,0,TV_FOOD,SV_FOOD_RATION,6},
	{RACE_ONI,2,0,TV_ALCOHOL,SV_ALCOHOL_ONIKOROSHI,5},
	{RACE_DEATH,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_WARBEAST,2,0,TV_FOOD,SV_FOOD_VENISON,20},
	{RACE_TSUKUMO,2,0,TV_STAFF,SV_STAFF_NOTHING,1},//���P�͎����Ȃ��@���ꏈ��
	{RACE_ANDROID,2,0,TV_STAFF,SV_STAFF_NOTHING,1},//���f�B�X���͎����Ȃ�����\��
	{RACE_GYOKUTO,2,0,TV_SWEETS,SV_SWEETS_DANGO,10},
	{RACE_YOUKO,2,0,TV_FOOD,SV_FOOD_ABURAAGE,3},
	{RACE_YAMAWARO,2,0,TV_FOOD,SV_FOOD_RATION,5},
	{RACE_BAKEDANUKI,2,0,TV_FOOD,SV_FOOD_RATION,6},
	{RACE_NINGYO,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_HOFGOBLIN,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_NYUDOU,2,0,TV_FOOD,SV_FOOD_RATION,7},
	{RACE_IMP,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_GOLEM,2,0,TV_MATERIAL,SV_MATERIAL_STONE,4},
	{RACE_KOBITO,2,0,TV_SWEETS,SV_SWEET_COOKIE,4},
	{RACE_ZOMBIE,2,0,TV_FOOD,SV_FOOD_SENTAN,3},
	{RACE_SPECTRE,2,0,TV_FOOD,SV_FOOD_RATION,2},
	{RACE_STRAYGOD,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_HALF_GHOST,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_DAIYOUKAI,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_GREATER_LILLIPUT,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{RACE_DEMIGOD,2,0,TV_FOOD,SV_FOOD_RATION,3},
	{ RACE_DEITY,2,0,TV_SWEETS,SV_SWEETS_DANGO,10 },
	{RACE_LUNARIAN,2,0,TV_SWEETS,SV_SWEETS_PEACH,4},

	{-1,0,0,0,0,0} //�I�[dummy
};

/*:::�E�Ƃ��ƊJ�n�������i�@�����i�Ȃ�*/
outfit_type birth_outfit_class[] = {
	{CLASS_WARRIOR,2,0,TV_SWORD,SV_WEAPON_LONG_SWORD,1},
	{CLASS_WARRIOR,2,0,TV_ARMOR,SV_ARMOR_SCALE,1},
	{CLASS_MAGE,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_MAGE,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},
	{CLASS_HIGH_MAGE,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_HIGH_MAGE,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},
	{CLASS_RANGER,2,0,TV_ARMOR,SV_ARMOR_LEATHER,1},
	{CLASS_RANGER,2,0,TV_KNIFE,SV_WEAPON_YAMAGATANA,1},
	{CLASS_RANGER,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_RANGER,2,0,TV_ARROW,SV_AMMO_LIGHT,20},
	{CLASS_MAID,0,0,TV_CLOTHES,SV_CLOTH_MAID,1},
	{CLASS_MAID,1,0,TV_CLOTHES,SV_CLOTH_SUIT,1},
	{CLASS_MAID,2,0,TV_KNIFE,SV_WEAPON_DAGGER,1},
	{CLASS_CIVILIAN,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},

	{CLASS_CAVALRY,2,0,TV_POLEARM,SV_WEAPON_GLAIVE,1},
	{CLASS_CAVALRY,2,0,TV_ARMOR,SV_ARMOR_SCALE,1},

	{ CLASS_RESEARCHER,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1 },

	{CLASS_ROGUE,2,0,TV_KNIFE,SV_WEAPON_TANTO,1},
	{CLASS_ROGUE,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1},
	{CLASS_ROGUE,2,0,TV_SCROLL,SV_SCROLL_MAPPING,5},

	{CLASS_ARCHER,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_ARCHER,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1},
	{CLASS_ARCHER,2,0,TV_ARROW,SV_AMMO_LIGHT,30},
	{CLASS_TEACHER,2,0,TV_SWORD,SV_WEAPON_RAPIER,1},
	{CLASS_TEACHER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_PRIEST,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1},
	{CLASS_PRIEST,2,0,TV_HAMMER,SV_WEAPON_MACE,1},
	{CLASS_OUTSIDER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_OUTSIDER,2,0,TV_SOUVENIR,SV_SOUVENIR_SMARTPHONE,1},
	{CLASS_TSUKUMO_MASTER,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	{CLASS_TSUKUMO_MASTER,2,0,TV_AXE,SV_WEAPON_BROAD_AXE,1},
	{CLASS_SYUGEN,2,0,TV_CLOTHES, SV_CLOTH_SYUGEN,1},
	{CLASS_SYUGEN,2,0,TV_STICK, SV_WEAPON_SYAKUJYOU,1},
	{CLASS_MAGIC_KNIGHT,2,0,TV_ARMOR, SV_ARMOR_LEATHER,1},
	{CLASS_MAGIC_KNIGHT,2,0,TV_SWORD, SV_WEAPON_SABRE,1},
	{CLASS_LIBRARIAN,2,0,TV_SWORD,SV_WEAPON_RAPIER,1},
	{CLASS_LIBRARIAN,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1},
	{CLASS_ENGINEER,2,0,TV_CLOTHES, SV_CLOTH_WORKER,1},
	{CLASS_ENGINEER,2,0,TV_HAMMER,SV_WEAPON_SPANNER,1},
	{CLASS_ENGINEER,2,0,TV_MACHINE,SV_MACHINE_WATERGUN,1},
	{CLASS_ENGINEER,2,0,TV_MACHINE,SV_MACHINE_LIGHTBOMB,1},
	{CLASS_SAMURAI,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	{CLASS_SAMURAI,2,0,TV_KATANA,SV_WEAPON_KATANA,1},
	{CLASS_MINDCRAFTER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_PALADIN,2,0,TV_ARMOR, SV_ARMOR_CHAIN,1},
	{CLASS_PALADIN,2,0,TV_SWORD, SV_WEAPON_SHORT_SWORD,1},
	{CLASS_SH_DEALER,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_SH_DEALER,2,0,TV_HAMMER,SV_WEAPON_FLAIL,1},
	{CLASS_JEWELER,2,0,TV_KNIFE,SV_WEAPON_DAGGER,1},
	{CLASS_JEWELER,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},

	{CLASS_RUMIA,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YOUMU,0,ART_ROUKANKEN,0,0,0},
	{CLASS_YOUMU,0,ART_HAKUROUKEN,0,0,0},
	{CLASS_YOUMU,2,0,TV_CLOTHES,SV_CLOTHES,1},

	{CLASS_UDONGE,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_UDONGE,2,0,TV_GUN,SV_FIRE_GUN_LUNATIC,1},
	{ CLASS_UDONGE,2,0,TV_HAMMER,SV_WEAPON_KINE,1 },
	{CLASS_UDONGE,2,0,TV_POTION,SV_POTION_CURE_SERIOUS,10},

	{CLASS_TEWI,2,0,TV_HAMMER,SV_WEAPON_KINE,1},
	{CLASS_TEWI,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_IKU,0,ART_HEAVENLY_MAIDEN,0,0,0},
	{CLASS_IKU,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_KOGASA,2,0,TV_CLOTHES,SV_CLOTHES,1},

	{CLASS_KOMACHI,2,0,TV_POLEARM,SV_WEAPON_WAR_SCYTHE,1},
	{CLASS_KOMACHI,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},

	{CLASS_KASEN,2,0,TV_CLOTHES,SV_CLOTHES,1},

	{CLASS_SUIKA,0,ART_IBUKI,0,0,0},
	{CLASS_SUIKA,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_MARISA,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_MARISA,2,0,TV_HEAD,SV_HEAD_WITCH,1},
	{CLASS_MARISA,2,0,TV_POTION,SV_POTION_SPEED,3},

	//�������A�r���e�B�J�[�h�@��p���i�ȊO�ł̓L�����Z�������
	{ CLASS_MARISA,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MARISA,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MARISA,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },


	{CLASS_WAKASAGI,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	{CLASS_WAKASAGI,2,0,TV_MATERIAL,SV_MATERIAL_AQUAMARINE,1},

	{CLASS_KOISHI,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_KOISHI,2,0,TV_HEAD,SV_HEAD_HAT,1},
	{CLASS_SEIGA,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1},

	{CLASS_MOMIZI,2,0,TV_CLOTHES, SV_CLOTH_SYUGEN,1},
	{CLASS_MOMIZI,2,0,TV_SWORD,SV_WEAPON_SCIMITAR,1},
	{CLASS_MOMIZI,2,0,TV_SHIELD,SV_SMALL_LEATHER_SHIELD,1},
	{CLASS_CIRNO,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_ORIN,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},
	{CLASS_ORIN,2,0,TV_MATERIAL, SV_MATERIAL_SKULL,1 },//v1.1.98

	{CLASS_SHINMYOUMARU,2,0,TV_CLOTHES, SV_CLOTH_EASTERN,1},
	{CLASS_SHINMYOUMARU,2,ART_SHINMYOUMARU,0,0,0},
	{CLASS_NAZRIN,0,ART_NAZRIN,0,0,0},
	{CLASS_NAZRIN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_NAZRIN,2,0,TV_STICK, SV_WEAPON_IRONSTICK,1},

	{CLASS_LETTY,2,0,TV_CLOTHES, SV_CLOTHES,1}, 

	{CLASS_YOSHIKA,2,0,TV_CLOTHES, SV_CLOTHES,1}, 
	{CLASS_YOSHIKA,2,0,TV_FOOD, SV_FOOD_SENTAN,5}, 

	{CLASS_PATCHOULI,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1}, 
	{CLASS_AYA,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_BANKI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_BANKI,2,0,TV_SPEAR, SV_WEAPON_AWL_PIKE,1},
	{CLASS_MYSTIA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_FLAN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_FLAN,2,ART_FLAN,0, 0,1},

	{CLASS_SHOU,0,ART_HOUTOU,0,0,0},
	{CLASS_SHOU,2,0,TV_CLOTHES, SV_CLOTH_LEATHER	,1},
	{CLASS_SHOU,2,0,TV_SPEAR, SV_WEAPON_SPEAR,1},

	{CLASS_MEDICINE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MEDICINE,2,0,TV_POTION, SV_POTION_POISON,16},

	{CLASS_YUYUKO,2,0,TV_CLOTHES, SV_CLOTH_EASTERN,1},
	{CLASS_YUYUKO,2,0,TV_FOOD, SV_FOOD_RATION,20},

	{CLASS_SATORI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KYOUKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_TOZIKO,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_KISUME,2,0,TV_ARMOR, SV_ARMOR_TUB,1},
	{CLASS_LILY_WHITE,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_HATATE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_HATATE,2,0,TV_CROSSBOW,SV_FIRE_LIGHT_CROSSBOW,1},
	{CLASS_HATATE,2,0,TV_BOLT,SV_AMMO_LIGHT,20},

	{CLASS_MIKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KOKORO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_WRIGGLE,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_NINJA,2,0,TV_KNIFE,SV_WEAPON_TANTO,1},
	{CLASS_NINJA,2,0,TV_CLOTHES, SV_CLOTH_KUROSHOUZOKU,1},
	{CLASS_NINJA,2,0,TV_SCROLL,SV_SCROLL_DARKNESS,7},
	{CLASS_CHEN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_YUUKA,0,ART_YUUKA,0,0,0},
	{CLASS_YUUKA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MURASA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KEINE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KEINE,2,0,TV_SWORD,SV_WEAPON_LONG_SWORD,1},

	{CLASS_REIMU,2,0,TV_STICK,SV_WEAPON_OONUSA,1},
	{CLASS_REIMU,2,0,TV_CLOTHES,SV_CLOTH_MIKO,1},

	{CLASS_YUGI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_YUGI,0,ART_HOSHIGUMA,0,0,0},

	{CLASS_KAGEROU,2,0,TV_CLOTHES, SV_CLOTH_ROBE,1},
	{CLASS_SHIZUHA,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},

	{CLASS_SANAE,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_SANAE,2,0,TV_CLOTHES,SV_CLOTH_MIKO,1},

	{CLASS_MINORIKO,2,0,TV_STICK,SV_WEAPON_QUARTERSTAFF,1},
	{CLASS_MINORIKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_REMY,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},

	{CLASS_NITORI,2,0,TV_CLOTHES, SV_CLOTH_WORKER,1},
	{CLASS_NITORI,2,0,TV_HAMMER,SV_WEAPON_SPANNER,1},
	{CLASS_NITORI,2,0,TV_MACHINE,SV_MACHINE_WATERGUN,1},

	{CLASS_BYAKUREN,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},
	{CLASS_SUWAKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_NUE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_NUE,2,0,TV_SPEAR, SV_WEAPON_TRIFURCATE_SPEAR,1},

	{CLASS_SEIJA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_SEIJA,2,0,TV_WAND, SV_WAND_MAGIC_MISSILE,1},
	{CLASS_SEIJA,2,0,TV_WAND, SV_WAND_STINKING_CLOUD,1},
	{CLASS_SEIJA,2,0,TV_WAND, SV_WAND_SLEEP_MONSTER,1},

	{CLASS_UTSUHO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_YAMAME,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_MARTIAL_ARTIST,2,0,TV_CLOTHES, SV_CLOTH_DOUGI,1},

	{CLASS_MAGIC_EATER,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MAGIC_EATER,2,0,TV_WAND, SV_WAND_MAGIC_MISSILE,1},
	{CLASS_MAGIC_EATER,2,0,TV_WAND, SV_WAND_STINKING_CLOUD,1},

	{CLASS_RAN,2,0,TV_CLOTHES, SV_CLOTH_DOUSHI,1},
	{CLASS_RAN,2,0,TV_KNIFE,SV_WEAPON_TANTO,1},
	{CLASS_RAN,2,0,TV_SCROLL, SV_SCROLL_WORD_OF_RECALL,9},

	{CLASS_EIKI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_EIKI,0,ART_EIKI,0,0,0},

	{CLASS_TENSHI,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_TENSHI,0,ART_HISOU,0,0,0},

	{CLASS_MEIRIN,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_PARSEE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_PARSEE,2,0,TV_KATANA, SV_WEAPON_SHORT_KATANA,1},

	{CLASS_SHINMYOU_2,2,0,TV_CLOTHES, SV_CLOTH_EASTERN,1},
	{CLASS_SHINMYOU_2,2,ART_SHINMYOUMARU,0,0,0},

	{CLASS_SUMIREKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_SUMIREKO,2,ART_3DPRINTER_GUN,0, 0,0},

	{CLASS_ICHIRIN,2,0,TV_CLOTHES,SV_CLOTH_ROBE,1},
	{CLASS_MOKOU,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_KANAKO,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_KANAKO,2,0,TV_STICK,SV_WEAPON_SIXFEET,1},

	{CLASS_FUTO,2,0,TV_CLOTHES, SV_CLOTH_DOUSHI,1},
	{CLASS_FUTO,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_FUTO,2,0,TV_ARROW,SV_AMMO_LIGHT,20},

	{CLASS_SUNNY,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_LUNAR,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_STAR,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_ALICE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_LUNASA,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_MERLIN,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_LYRICA,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_CLOWNPIECE,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_CLOWNPIECE,2,ART_CLOWNPIECE,0,0,0},

	{CLASS_DOREMY,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_SAKUYA,0,0,TV_CLOTHES,SV_CLOTH_MAID,1},
	{CLASS_BENBEN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YATSUHASHI,2,0,TV_CLOTHES,SV_CLOTHES,1},
	//{CLASS_HINA,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},���ꏈ���ōŏ�����􂢂ɂ���
	{CLASS_3_FAIRIES,2,0,TV_CLOTHES, SV_CLOTHES,1},//3�񑕔��������s�����߂�3����
	{CLASS_3_FAIRIES,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{CLASS_3_FAIRIES,2,0,TV_CLOTHES, SV_CLOTHES,1},
	{ CLASS_3_FAIRIES,2,ART_CLOWNPIECE,0,0,0 },//��p���i�ȊO�ł͏��O��������
	{ CLASS_3_FAIRIES,2,0,TV_FOOD, SV_FOOD_MAGIC_WATERMELON,1 },
	{ CLASS_3_FAIRIES,2,0,TV_MUSHROOM, SV_MUSHROOM_PUFFBALL,1 },


	{CLASS_RAIKO,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1},

	{CLASS_CHEMIST,2,0,TV_KNIFE,SV_WEAPON_YAMAGATANA,1},
	{CLASS_CHEMIST,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1},
	{CLASS_CHEMIST,2,0,TV_BOOK_MEDICINE,0,1},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_RED,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_WHITE,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_GREEN,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_BLUE,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_MON_BLACK,5},
	{CLASS_CHEMIST,2,0,TV_MUSHROOM,SV_MUSHROOM_POISON,5},
	{CLASS_MAMIZOU,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YUKARI,0,ART_YUKARI,0,0,0},
	{CLASS_YUKARI,2,0,TV_CLOTHES,SV_CLOTH_DRESS,1},

	{CLASS_RINGO,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_RINGO,2,0,TV_GUN,SV_FIRE_GUN_LUNATIC,1},
	{CLASS_RINGO,2,0,TV_SWEETS,SV_SWEETS_DANGO,30},

	{CLASS_SEIRAN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_SEIRAN,2,0,TV_GUN,SV_FIRE_GUN_LUNATIC,1},
	{CLASS_SEIRAN,2,0,TV_HAMMER,SV_WEAPON_KINE,1},

	{CLASS_EIRIN,2,0,TV_BOW,SV_FIRE_SHORT_BOW,1},
	{CLASS_EIRIN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_EIRIN,2,0,TV_ARROW,SV_AMMO_LIGHT,30},

	{CLASS_KAGUYA,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1},
	//�P����@���ꏈ����ON/OFF����
	{CLASS_KAGUYA,0,ART_KAGUYA_DRAGON,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_HOURAI,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_HOTOKE,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_HINEZUMI,0,0,0},
	{CLASS_KAGUYA,0,ART_KAGUYA_TSUBAME,0,0,0},

	{CLASS_SAGUME,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1},

	{CLASS_REISEN2,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_REISEN2,2,0,TV_GUN,SV_FIRE_GUN_INFANTRY,1},

	{CLASS_TOYOHIME,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_TOYOHIME,2,ART_TOYOHIME,0,0,0},
	{CLASS_TOYOHIME,2,0,TV_SWEETS,SV_SWEETS_PEACH,8},

	{CLASS_YORIHIME,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_YORIHIME,2,ART_YORIHIME,0,0,0},

	{CLASS_JUNKO,2,0,TV_CLOTHES, SV_CLOTHES,1},

	{CLASS_HECATIA,2,ART_HECATIA,0,0,0},

	{CLASS_SOLDIER,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1},
	{CLASS_SOLDIER,2,0,TV_KNIFE,SV_WEAPON_KNIFE,1},
	{CLASS_SOLDIER,2,0,TV_GUN,SV_FIRE_GUN_HANDGUN,1},

	{CLASS_NEMUNO,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_NEMUNO,2,0,TV_AXE,SV_WEAPON_NATA,1},

	{CLASS_AUNN,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_NARUMI,2,0,TV_CLOTHES,SV_CLOTHES,1},

	//v1.1.37 ����͐F�X���������Ă�
	{CLASS_KOSUZU,2,0,TV_CLOTHES,SV_CLOTHES,1},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_RUNE_OF_PROTECTION,5},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_DISPEL_UNDEAD,5},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_STAR_DESTRUCTION,5},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_FIRE,3},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_ICE,3},
	{CLASS_KOSUZU,2,0,TV_SCROLL,SV_SCROLL_CHAOS,3},
	{CLASS_KOSUZU,2,0,TV_BOOK_OCCULT,3,1},
	{CLASS_KOSUZU,2,0,TV_SOUVENIR,SV_SOUVENIR_OUIJA_BOARD,1},
	{CLASS_KOSUZU,2,ART_HYAKKI,0,0,1},

	{ CLASS_MAI,2,0,TV_CLOTHES,SV_CLOTH_DRESS,1 },
	{ CLASS_MAI,2,0,TV_SPEAR,SV_WEAPON_SPEAR,1 },

	{ CLASS_SATONO,2,0,TV_CLOTHES,SV_CLOTH_DRESS,1 },

	{ CLASS_VFS_CLOWNPIECE,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_VFS_CLOWNPIECE,2,ART_CLOWNPIECE,0,0,0 },

	{ CLASS_JYOON,2,0,TV_CLOTHES, SV_CLOTH_DRESS,1 },
	{ CLASS_SHION,2,0,TV_CLOTHES, SV_CLOTHES,1 },

	{ CLASS_OKINA,2,0,TV_CLOTHES, SV_CLOTH_DOUSHI,1 },
	{ CLASS_OKINA,2,ART_OKINA,0, 0,1 },
	{ CLASS_EIKA,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_MAYUMI,2,0,TV_ARMOR,SV_ARMOR_FINE_CERAMIC,1 },
	{ CLASS_MAYUMI,2,ART_MAYUMI,0,0,1 },

	{ CLASS_KUTAKA,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_URUMI,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_URUMI,2,0,TV_OTHERWEAPON,SV_OTHERWEAPON_FISHING,1 },
	{ CLASS_URUMI,2,0,TV_SOUVENIR,SV_SOUVENIR_STONE_BABY,1 },

	{ CLASS_SAKI,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_SAKI,2,0,TV_GUN,SV_FIRE_GUN_HANDGUN,1 },

	{ CLASS_YACHIE,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_KEIKI,2,ART_KEIKI,0,0,1 },
	{ CLASS_KEIKI,2,0,TV_KNIFE,SV_WEAPON_KNIFE,2 },

	{ CLASS_TAKANE,2,0,TV_CLOTHES,SV_CLOTH_PADDED,1 },
	{ CLASS_TAKANE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_TAKANE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_TAKANE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },

	{ CLASS_MIKE,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_MIKE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MIKE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_MIKE,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },


	{ CLASS_CARD_DEALER,2,0,TV_CLOTHES,SV_CLOTH_SUIT,1 },
	{ CLASS_CARD_DEALER,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_CARD_DEALER,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },
	{ CLASS_CARD_DEALER,2,0,TV_ABILITY_CARD,SV_ABILITY_CARD,1 },

	{ CLASS_MOMOYO,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_MOMOYO,2,0,TV_AXE,SV_WEAPON_PICK,1 },
	{ CLASS_MOMOYO,2,0,TV_MATERIAL,SV_MATERIAL_STONE,3 },

	{ CLASS_SANNYO,2,0,TV_CLOTHES,SV_CLOTH_EASTERN,1 },

	{ CLASS_TSUKASA,2,0,TV_CLOTHES,SV_CLOTHES,1 },

	{ CLASS_MEGUMU,2,0,TV_CLOTHES, SV_CLOTH_SYUGEN,1 },
	{ CLASS_MEGUMU,2,ART_MEGUMU,0,0,1 },

	{ CLASS_MISUMARU,2,0,TV_CLOTHES,SV_CLOTHES,1 },
	{ CLASS_MISUMARU,2,0,TV_MATERIAL,SV_MATERIAL_HEMATITE,1 },
	{ CLASS_MISUMARU,2,0,TV_MATERIAL,SV_MATERIAL_MAGNETITE,1 },
	{ CLASS_MISUMARU,2,0,TV_MATERIAL,SV_MATERIAL_OPAL,1 },

	{ CLASS_YUMA,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_YUMA,2,ART_TOUTETSU,0,0,1 },
	{ CLASS_YUMA,2,0,TV_FLASK, SV_FLASK_OIL,20 },

	{ CLASS_CHIMATA,2,0,TV_CLOTHES, SV_CLOTHES,1 },

	{ CLASS_MIYOI,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_MIYOI,2,ART_IBUKI,0,0,1 },

	{ CLASS_BITEN,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_BITEN,2,ART_BITEN,0,0,1 },

	{ CLASS_ENOKO,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_ENOKO,2,ART_ENOKO,0,0,1 },

	{ CLASS_CHIYARI,2,0,TV_CLOTHES, SV_CLOTH_T_SHIRT,1 },

	{ CLASS_HISAMI,2,0,TV_CLOTHES, SV_CLOTHES,1 },
	{ CLASS_ZANMU,2,0,TV_CLOTHES, SV_CLOTHES,1 },

	{ CLASS_BEEKEEPER,2,0,TV_CLOTHES, SV_CLOTH_PADDED,1 },
	{ CLASS_BEEKEEPER,2,0,TV_AXE, SV_WEAPON_NATA,1 },
	{ CLASS_BEEKEEPER,2,0,TV_SWEETS, SV_SWEETS_HONEY,10 },
	{ CLASS_BEEKEEPER,2,0,TV_POTION, SV_POTION_CURE_POISON,3 },


	{-1,0,0,0,0,0} //�I�[dummy
};



/*
 * Init players with some belongings
 *
 * Having an item makes the player "aware" of its purpose.
 */
/*:::�E�ƁA�푰�A���i�ɂ�鏉�������A�C�e�������肷��*/
///race class realm item �����A�C�e��
void player_outfit(void)
{
	int i, tv, sv;

	object_type	forge;
	object_type	*q_ptr;

	/* Get local object */
	q_ptr = &forge;

	/*:::���P���ꏈ���@��p�σp�����[�^�A�[�e�B�t�@�N�g*/
	if(p_ptr->pclass == CLASS_KOGASA)
	{
		object_prep(q_ptr, lookup_kind(TV_STICK, SV_WEAPON_KOGASA));
		apply_kogasa_magic(q_ptr,1,TRUE);
		add_outfit(q_ptr);
	}
	/*:::�A���X���ꏈ���@��C�l�`�Ƀ��C�s�A����*/
	else if(p_ptr->pclass == CLASS_ALICE)
	{
		object_prep(q_ptr, lookup_kind(TV_SWORD, SV_WEAPON_RAPIER));
		apply_magic(q_ptr,1,AM_FORCE_NORMAL);
		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		object_copy(&inven_add[0], q_ptr); 

	}
	/*:::�����ꏈ��*/
	else if(p_ptr->pclass == CLASS_SAKUYA)
	{
		object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_KNIFE));
		q_ptr->name2 = EGO_WEAPON_DEMONSLAY;
		q_ptr->pval = 1;
		q_ptr->to_d = 10;
		q_ptr->to_h = 10;

		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		add_outfit(q_ptr);

	}
	//v1.1.68 �O������p���i�́A����ʏ��3���ɒǉ��������6��
	else if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
	{
		for (i = 0; i < 6; i++)
		{
			object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTHES));
			apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
			object_aware(q_ptr);
			object_known(q_ptr);
			q_ptr->ident |= (IDENT_MENTAL);
			add_outfit(q_ptr);

		}


	}
	/*:::�����ꏈ��*/
	else if(p_ptr->pclass == CLASS_HINA)
	{
		object_prep(q_ptr, lookup_kind(TV_CLOTHES, SV_CLOTH_DRESS));
		apply_magic(q_ptr,1,AM_FORCE_NORMAL);
		q_ptr->curse_flags = TRC_CURSED;
		q_ptr->to_a = 3;
		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		add_outfit(q_ptr);

	}
	//v1.1.88 �����ˏ��������J�[�h�Ɏ��O�̂��ꖇ�ǉ�
	else if (p_ptr->pclass == CLASS_TAKANE)
	{
		object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(q_ptr, ABL_CARD_YAMAWARO, 0, 0);
		add_outfit(q_ptr);

	}
	//v1.1.90 �R�@��
	else if (p_ptr->pclass == CLASS_SANNYO)
	{
		object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(q_ptr, ABL_CARD_KISERU, 0, 0);
		add_outfit(q_ptr);

	}
	//v1.1.93 �~�P��
	else if (p_ptr->pclass == CLASS_MIKE)
	{
		object_prep(q_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		apply_magic_abilitycard(q_ptr, ABL_CARD_MANEKINEKO, 0, 0);
		add_outfit(q_ptr);

	}



	//v1.1.87 �J�[�h���l�n�̐E�Ƃ�EXTRA���[�h�ŊJ�n�����Ƃ�10�A�{�b�N�X��ǉ��Ŏ����Ă���
	//v1.1.87c CLASS_CARD_DEALER��CHECK_ABLCARD_DEALER_CLASS���ԈႦ�đS�L�����Ŕ�����ɓ����Ă����B�C���B
	if (EXTRA_MODE && CHECK_ABLCARD_DEALER_CLASS)
	{
		object_prep(q_ptr, lookup_kind(TV_CHEST, SV_CHEST_10_GACHA));
		apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		add_outfit(q_ptr);

	}






	//v1.1.79 �V�푰�u���~��炵�v�̏����̈ړ����~
	//v2.0.9 �����͈ɐ��Z�Œ�B�����������������瑕������
	if (p_ptr->prace == RACE_ZASHIKIWARASHI && p_ptr->pclass != CLASS_MIYOI)
	{
		object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_INN));
		object_copy(&inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE], q_ptr);

	}

	//�푰�Œ�̏����A�C�e���𐶐�
	for(i=0;birth_outfit_race[i].type != -1 ;i++)
	{
		//�푰�Ɛ��ʂ�����Ȃ��s�̓p�X
		if(birth_outfit_race[i].type != p_ptr->prace) continue;
		if(birth_outfit_race[i].psex == SEX_MALE && p_ptr->psex == SEX_FEMALE || birth_outfit_race[i].psex == SEX_FEMALE && p_ptr->psex == SEX_MALE) continue;
		//-Hack-���P�͕t�r�_�������H�ׂ��Ȃ��̂Ŏ����Ă��Ȃ�
		if(birth_outfit_race[i].tval==TV_STAFF && birth_outfit_race[i].sval==SV_STAFF_NOTHING && p_ptr->pclass == CLASS_KOGASA) continue;
		//-Hack-���f�B�X�������H�ׂ��Ȃ��̂Ŏ����Ă��Ȃ�
		if(p_ptr->pclass == CLASS_MEDICINE) break;
		//�O���l�͐H�Ƃ������Ȃ�
		if(p_ptr->pclass == CLASS_OUTSIDER) break;
		//�h���~�[���H�Ƃ������Ȃ�
		if(p_ptr->pclass == CLASS_DOREMY) break;
		//v1.1.80 ����p���i�͓��݂������Ȃ�
		if (is_special_seikaku(SEIKAKU_SPECIAL_AYA)) break;

		//�푰�����ɃA�[�e�B�t�@�N�g�͖��Ή�
		if(birth_outfit_race[i].artifact_idx)
		{
			msg_format("ERROR:birth_outfit_race[%d]�ɃA�[�e�B�t�@�N�g���w�肳��Ă���(������)",i);
			continue;
		}
		//�O�̂���
		if(birth_outfit_race[i].num < 1 || birth_outfit_race[i].num > 99 )
		{
			msg_format("ERROR:birth_outfit_race[%d]�̃A�C�e��������������",i);
			continue;
		}
		object_prep(q_ptr,lookup_kind(birth_outfit_race[i].tval,birth_outfit_race[i].sval));
			//apply_magic�́�����Ə㎿������L�����Z�������݂̕i�𐶐��B�����i��apply_magic���Ȃ���Ώ���ɕ��ɂȂ邪�����Ƃ��̏����̂���
		apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
		q_ptr->number = birth_outfit_race[i].num;
		add_outfit(q_ptr);
	}

	//�E�Ɓi���j�[�N�N���X�܂ށj�Œ�̏����A�C�e��
	for(i=0;birth_outfit_class[i].type != -1 ;i++)
	{
		//�E�ƂƐ��ʂ�����Ȃ��s�̓p�X
		if(birth_outfit_class[i].type != p_ptr->pclass) continue;
		if(birth_outfit_class[i].psex == SEX_MALE && p_ptr->psex == SEX_FEMALE || birth_outfit_class[i].psex == SEX_FEMALE && p_ptr->psex == SEX_MALE) continue;


		//v2.0.1 �������͐�p���i�ȊO�ł̓A�r���e�B�J�[�h�������Ȃ�
		if (p_ptr->pclass == CLASS_MARISA && !is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
		{
			if (birth_outfit_class[i].tval == TV_ABILITY_CARD) continue;
		}

		//�A�[�e�B�t�@�N�g�������ĊJ�n����N���X�̏��� �啔����create_named_art()����R�s�[
		if(birth_outfit_class[i].artifact_idx)
		{
			int tmp_k_idx;
			artifact_type *a_ptr = &a_info[birth_outfit_class[i].artifact_idx];

			//�����o�Ȃ��I�v�V������ON�̂Ƃ��A
			//player_outfit()�̒��O�ɑS�Ắ���cur_num��1�ɂȂ��Đ�������Ȃ��Ȃ�
			//v2.0.9 ��O�Ƃ��Ĕ����̉Ƃ̂ݐ����\�ɂ���
			if(a_ptr->cur_num && p_ptr->pclass != CLASS_MIYOI) continue;

			if (!a_ptr->name)
			{
				msg_format("ERROR:birth_outfit_class[%d]�̃A�[�e�B�t�@�N�gID����������",i);
				continue;
			}
			tmp_k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);
			if (!tmp_k_idx)
			{
				msg_format("ERROR:a_info[%d]��tval��sval����������",i);
				continue;
			}

			object_prep(q_ptr, tmp_k_idx);
			q_ptr->name1 = birth_outfit_class[i].artifact_idx;

			// Hack - �P����ꏈ���@�܂��P��N�G�X�g�œ͂����Ă��Ȃ��A�C�e�������O
			if(p_ptr->pclass == CLASS_KAGUYA)
			{
				if(q_ptr->name1 == ART_KAGUYA_TSUBAME && kaguya_quest_level < 1) continue;
				if(q_ptr->name1 == ART_KAGUYA_HINEZUMI && kaguya_quest_level < 2) continue;
				if(q_ptr->name1 == ART_KAGUYA_HOTOKE && kaguya_quest_level < 3) continue;
				if(q_ptr->name1 == ART_KAGUYA_DRAGON && kaguya_quest_level < 4) continue;
				if(q_ptr->name1 == ART_KAGUYA_HOURAI && kaguya_quest_level < 5) continue;
			}

			//�O�����͐�p���i�ȊO�ł̓N���E���s�[�X�̏����������Ȃ�
			if (p_ptr->pclass == CLASS_3_FAIRIES)
			{
				if (!is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES)) continue;
			}

			/* Extract the fields */
			q_ptr->pval = a_ptr->pval;
			q_ptr->ac = a_ptr->ac;
			q_ptr->dd = a_ptr->dd;
			q_ptr->ds = a_ptr->ds;
			q_ptr->to_a = a_ptr->to_a;
			q_ptr->to_h = a_ptr->to_h;
			q_ptr->to_d = a_ptr->to_d;
			q_ptr->weight = a_ptr->weight;

			///mod150908 �N���E���s�[�X�̏����̓��ꏈ��
			if(q_ptr->name1 == ART_CLOWNPIECE) q_ptr->xtra4 = -1;

			/* Hack -- extract the "cursed" flag */
			if (a_ptr->gen_flags & TRG_CURSED) q_ptr->curse_flags |= (TRC_CURSED);
			if (a_ptr->gen_flags & TRG_HEAVY_CURSE) q_ptr->curse_flags |= (TRC_HEAVY_CURSE);
			if (a_ptr->gen_flags & TRG_PERMA_CURSE) q_ptr->curse_flags |= (TRC_PERMA_CURSE);
			if (a_ptr->gen_flags & (TRG_RANDOM_CURSE0)) q_ptr->curse_flags |= get_curse(0, q_ptr);
			if (a_ptr->gen_flags & (TRG_RANDOM_CURSE1)) q_ptr->curse_flags |= get_curse(1, q_ptr);
			if (a_ptr->gen_flags & (TRG_RANDOM_CURSE2)) q_ptr->curse_flags |= get_curse(2, q_ptr);
			random_artifact_resistance(q_ptr, a_ptr);

			//���̓�s�v��̂��v��Ȃ��̂������ɂ͌��ĂȂ����Ƃ肠�����ǉ�
			q_ptr->number = 1;
			a_info[birth_outfit_class[i].artifact_idx].cur_num+=1;

			q_ptr->ident |= (IDENT_MENTAL);

			//v2.0.9 �����́u�ɐ��Z�v���ꏈ��
			if (p_ptr->pclass == CLASS_MIYOI)
			{
				object_aware(q_ptr);
				object_known(q_ptr);
				q_ptr->ident |= (IDENT_MENTAL);
				object_copy(&inven_special[INVEN_SPECIAL_ZASHIKI_WARASHI_HOUSE], q_ptr);

			}
			else
			{
				add_outfit(q_ptr);
			}

		}	
		//�O�̂���
		else if(birth_outfit_class[i].num < 1 || birth_outfit_class[i].num > 99 )
		{
			msg_format("ERROR:birth_outfit_class[%d]�̃A�C�e��������������",i);
			continue;
		}		
		else
		{

			int tmp_num;
			object_prep(q_ptr,lookup_kind(birth_outfit_class[i].tval,birth_outfit_class[i].sval));
			//apply_magic�́�����Ə㎿������L�����Z�������݂̕i�𐶐��B�����i��apply_magic���Ȃ���Ώ���ɕ��ɂȂ邪�����Ƃ��̏����̂���
			apply_magic(q_ptr, 1, AM_FORCE_NORMAL);
			//������
			//v1.1.62 EXTRA�ł͖�ʗނ̏����������𑝂₷
			tmp_num = birth_outfit_class[i].num;
			if (EXTRA_MODE && (q_ptr->tval == TV_ARROW || q_ptr->tval == TV_BOLT))
				tmp_num = MIN(99, tmp_num * 3);
			q_ptr->number = tmp_num;
			//pval�̂��镐��h��͏��1
			if(object_is_weapon_armour_ammo(q_ptr) && q_ptr->pval) q_ptr->pval = 1;
			add_outfit(q_ptr);
		}
	}
	//�ǂ߂閂�@���̈���ڂ�����
	if(p_ptr->realm1)
	{
		object_prep(q_ptr,lookup_kind(p_ptr->realm1,0));
		q_ptr->number = 1;
		if(EXTRA_MODE) q_ptr->number += randint1(2);
		add_outfit(q_ptr);
	}
	if(p_ptr->realm2)
	{
		object_prep(q_ptr,lookup_kind(p_ptr->realm2,0));
		q_ptr->number = 1;
		if(EXTRA_MODE) q_ptr->number += randint1(2);
		add_outfit(q_ptr);
	}
	//v1.1.44 ���ǂ񂰓��ꐫ�i
	if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
	{
		object_prep(q_ptr, lookup_kind(TV_BOOK_MEDICINE, 0));
		q_ptr->number = 1;
		if (EXTRA_MODE) q_ptr->number += randint1(2);
		add_outfit(q_ptr);
	}
	//v1.1.52 俎q���ꐫ�i
	else if (is_special_seikaku(SEIKAKU_SPECIAL_SUMIREKO))
	{
		object_prep(q_ptr, lookup_kind(TV_SOUVENIR, SV_SOUVENIR_SMARTPHONE));
		q_ptr->number = 1;
		add_outfit(q_ptr);
	}


	if(REALM_SPELLMASTER)
	{
		for(i=1;i<=TV_BOOK_END;i++)
		{
			bool flag_ok = FALSE;
			//v1.1.32 �p�`�����[���ꐫ�i�u���s�v�ǉ�
			if (is_special_seikaku(SEIKAKU_SPECIAL_PATCHOULI) && i <= MAX_MAGIC)
				flag_ok = TRUE;
			if (cp_ptr->realm_aptitude[i] && rp_ptr->realm_aptitude[i])
				flag_ok = TRUE;

			if(flag_ok)
			{
				object_prep(q_ptr,lookup_kind(i,0));
				q_ptr->number = 1;
				if(EXTRA_MODE) q_ptr->number += randint1(2);
				add_outfit(q_ptr);
			}
		}
	}


	//�����܂�
	if (p_ptr->pclass != CLASS_NINJA && p_ptr->pclass != CLASS_OUTSIDER  && p_ptr->pclass != CLASS_NUE
		&& p_ptr->prace != RACE_VAMPIRE && p_ptr->pclass != CLASS_WRIGGLE && p_ptr->pclass != CLASS_UTSUHO)
	{
		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));


		//v1.1.93 �����܂̐��\�Ɛ��𑵂���BEXTRA�ň�����������Qy@����̂��܂��̂�
		//q_ptr->number = (byte)rand_range(3, 7);
		//if(EXTRA_MODE) q_ptr->number += 3;
		//q_ptr->xtra4 = rand_range(3, 7) * 500;
		q_ptr->number = (EXTRA_MODE) ? 9 : 6;
		q_ptr->xtra4 = 2500;
		add_outfit(q_ptr);
	}

	//Ex���[�h�ł̓c���n�V�������Ă���B���܂ɔ��̂Ȃ������ɏo�邽�߁B
	if(EXTRA_MODE)
	{
		object_prep(q_ptr, lookup_kind(TV_AXE, SV_WEAPON_PICK));
		apply_magic(q_ptr,0,AM_FORCE_NORMAL);

		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= (IDENT_MENTAL);
		inven_carry(q_ptr);

	}





	//v1.1.37 ����̓����_���ȗd���{�������Ă���
	if(p_ptr->pclass == CLASS_KOSUZU)
	{
		int j;
		for(j=0;j<3;j++)
		{
			monster_race *r_ptr;

			object_prep(q_ptr, lookup_kind(TV_CAPTURE, SV_CAPTURE_NONE));
			while (1)
			{
				i = randint1(max_r_idx - 1);
				r_ptr = &r_info[i];
				//���x��1�ȏ�20�ȉ�����A�l�ԕs�A���j�[�N�s��
				if(!r_ptr->level || r_ptr->level > 20) continue;
				if(r_ptr->flags3 & RF3_HUMAN) continue;

				if (!item_monster_okay(i)) continue;
				if (!r_ptr->rarity) continue;

				break;
			}
			q_ptr->pval = i;
			q_ptr->xtra3 = get_mspeed(r_ptr);
			if (r_ptr->flags1 & RF1_FORCE_MAXHP) q_ptr->xtra4 = maxroll(r_ptr->hdice, r_ptr->hside);
			else	q_ptr->xtra4 = damroll(r_ptr->hdice, r_ptr->hside);
			q_ptr->xtra5 = q_ptr->xtra4;
			object_aware(q_ptr);
			object_known(q_ptr);
			q_ptr->ident |= (IDENT_MENTAL);
			inven_carry(q_ptr);
		}
	}

#if 0
	/* Give the player some food */
	switch (p_ptr->prace)
	{
	case RACE_VAMPIRE:
		/* Nothing! */
		/* Vampires can drain blood of creatures */
		break;

	case RACE_DEMON:
		/* Demon can drain vitality from humanoid corpse */

		/* Prepare allocation table */
		get_mon_num_prep(monster_hook_human, NULL);

		///del131221 death
		/*
		for (i = rand_range(3,4); i > 0; i--)
		{
			object_prep(q_ptr, lookup_kind(TV_CORPSE, SV_CORPSE));
			q_ptr->pval = get_mon_num(2);
			q_ptr->number = 1;
			add_outfit(q_ptr);
		}
		*/
		break;

#if 0
	case RACE_SKELETON:
		/* Some Skeletons */
		object_prep(q_ptr, lookup_kind(TV_SKELETON, SV_ANY));
		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(q_ptr);
		break;
#endif
	case RACE_SKELETON:
	case RACE_GOLEM:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		/* Staff (of Nothing) */
		object_prep(q_ptr, lookup_kind(TV_STAFF, SV_STAFF_NOTHING));
		q_ptr->number = 1;

		add_outfit(q_ptr);
		break;

	case RACE_ENT:
		/* Potions of Water */
		object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
		q_ptr->number = (byte)rand_range(15, 23);
		add_outfit(q_ptr);

		break;

	case RACE_ANDROID:
		/* Flasks of oil */
		object_prep(q_ptr, lookup_kind(TV_FLASK, SV_ANY));

		/* Fuel with oil (move pval to xtra4) */
		apply_magic(q_ptr, 1, AM_NO_FIXED_ART);

		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(q_ptr);

		break;

	default:
		/* Food rations */
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
		q_ptr->number = (byte)rand_range(3, 7);

		add_outfit(q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((p_ptr->prace == RACE_VAMPIRE) && (p_ptr->pclass != CLASS_NINJA))
	{
		/* Hack -- Give the player scrolls of DARKNESS! */
		object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_DARKNESS));

		q_ptr->number = (byte)rand_range(2, 5);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass != CLASS_NINJA)
	{
		/* Hack -- Give the player some torches */
		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));
		q_ptr->number = (byte)rand_range(3, 7);
		q_ptr->xtra4 = rand_range(3, 7) * 500;

		add_outfit(q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	if ((p_ptr->pclass == CLASS_RANGER) || (p_ptr->pclass == CLASS_CAVALRY))
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	if (p_ptr->pclass == CLASS_RANGER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_BOW, SV_SHORT_BOW));

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_ARCHER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_HIGH_MAGE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_WAND, SV_WAND_MAGIC_MISSILE));
		q_ptr->number = 1;
		q_ptr->pval = (byte)rand_range(25, 30);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_SORCERER)
	{
		for (i = TV_LIFE_BOOK; i <= TV_LIFE_BOOK+MAX_MAGIC-1; i++)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(i, 0));
			q_ptr->number = 1;

			add_outfit(q_ptr);
		}
	}
	else if (p_ptr->pclass == CLASS_TOURIST)
	{
		if (p_ptr->pseikaku != SEIKAKU_SEXY)
		{
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(TV_SHOT, SV_AMMO_LIGHT));
			q_ptr->number = (byte)rand_range(15, 20);

			add_outfit(q_ptr);
		}

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_BISCUIT));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_WAYBREAD));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_JERKY));
		q_ptr->number = (byte)rand_range(1, 3);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_ALE));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);

		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_WINE));
		q_ptr->number = (byte)rand_range(2, 4);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_NINJA)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_SPIKE, 0));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	else if (p_ptr->pclass == CLASS_SNIPER)
	{
		/* Hack -- Give the player some bolts */
		object_prep(q_ptr, lookup_kind(TV_BOLT, SV_AMMO_NORMAL));
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(q_ptr);
	}
	///del131223
	/*
	if(p_ptr->pseikaku == SEIKAKU_SEXY)
	{
		player_init[p_ptr->pclass][2][0] = TV_HAFTED;
		player_init[p_ptr->pclass][2][1] = SV_WHIP;
	}
	*/
	/* Hack -- Give the player three useful objects */
	for (i = 0; i < 3; i++)
	{
		/* Look up standard equipment */
		tv = player_init[p_ptr->pclass][i][0];
		sv = player_init[p_ptr->pclass][i][1];

		if ((p_ptr->prace == RACE_ANDROID) && ((tv == TV_SOFT_ARMOR) || (tv == TV_HARD_ARMOR))) continue;
		/* Hack to initialize spellbooks */
		if (tv == TV_SORCERY_BOOK) tv = TV_LIFE_BOOK + p_ptr->realm1 - 1;
		else if (tv == TV_DEATH_BOOK) tv = TV_LIFE_BOOK + p_ptr->realm2 - 1;

		else if (tv == TV_RING && sv == SV_RING_RES_FEAR &&
		    p_ptr->prace == RACE_BARBARIAN)
			/* Barbarians do not need a ring of resist fear */
			sv = SV_RING_SUSTAIN_STR;

		else if (tv == TV_RING && sv == SV_RING_SUSTAIN_INT &&
		    p_ptr->prace == RACE_MIND_FLAYER)
		  {
			tv = TV_POTION;
			sv = SV_POTION_RESTORE_MANA;
		  }

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player an object */
		object_prep(q_ptr, lookup_kind(tv, sv));

		/* Assassins begin the game with a poisoned dagger */
		///item ���������A�C�e�����G�S�����Ă���
		if ((tv == TV_SWORD || tv == TV_HAFTED) && (p_ptr->pclass == CLASS_ROGUE &&
			p_ptr->realm1 == REALM_DEATH)) /* Only assassins get a poisoned weapon */
		{
			q_ptr->name2 = EGO_BRAND_POIS;
		}

		add_outfit(q_ptr);
	}

	/* Hack -- make aware of the water */
	k_info[lookup_kind(TV_POTION, SV_POTION_WATER)].aware = TRUE;
#endif
}

#if 0
//v1.1.65 �����������̂ŌÂ��̂�ޔ�

/*
 * Player race
 */
/*:::p_ptr->prace�����肷��*/
///mod140105 �V�푰�e�[�u���ɑΉ��B�������⏉���I��s�̃t���O�������Ă�푰�͕\�����Ȃ��B
static bool get_player_race(void)
{
	int     k, n, cs, os;
	cptr    str;
	char    c;
	char	sym[MAX_RACES];
	char    p2 = ')';
	char    buf[80], cur[80];

	//����MAX_BIRTH_RACES�𒴂��Ď푰�𑝂₷�Ƃ��ɂ́A�������\���̂Ȃǂɕς��Ȃ��ƃG���[�N����
	cptr help_index[MAX_BIRTH_RACES] =
	{"Human","Youkai","Half-Youkai","Sennin","Tennin","Fairy",
	"Kappa","Magician","Karasu-tengu","Hakurou-tengu","Oni","Death",
	"Warbeast","Tsukumo","Android","Gyokuto","Youko","Yamawaro",
	"Bake-danuki","Ningyo","Hofgoblin","Nyudo","Imp","Golem","Kobito","Zombie",
	"Vampire","Spectre","Stray-god","Daiyoukai","Demigod","Lunarian"};


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���ӁF�s�푰�t�ɂ���ăL�����N�^�[�̐�V�I�Ȏ�����{�[�i�X���ω����܂��B", 23, 5);
#else
	put_str("Note: Your 'race' determines various intrinsic factors and bonuses.", 23 ,5);
#endif

	/* Dump races */
//	for (n = 0; n < MAX_RACES; n++)
	for (n = 0; n < MAX_BIRTH_RACES; n++)
	{
		/* Analyze */
		rp_ptr = &race_info[n];
		str = rp_ptr->title;

		/*:::���̈ꕶ��MAX_RACE����MAX_BIRTH_RACE�ɂ��邱�ƂŖ��ɗ����Ȃ��Ȃ������O�̂��ߎc���Ƃ�*/
		if(rp_ptr->flag_nobirth || rp_ptr->flag_special || rp_ptr->flag_only_unique) continue;
		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);
#ifdef JP
		sprintf(buf, "%c%c%s", sym[n], p2, str);
#else
		sprintf(buf, "%c%c %s", sym[n], p2, str);
#endif
		if(rp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, buf, 12 + (n/5), 1 + 16 * (n%5));
		else put_str(buf, 12 + (n/5), 1 + 16 * (n%5));

	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Choose */
	k = -1;
	cs = p_ptr->prace;
//	os = MAX_RACES;
	os = MAX_BIRTH_RACES;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			rp_ptr = &race_info[os];

			if(os != MAX_BIRTH_RACES && rp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, cur, 12 + (os/5), 1 + 16 * (os%5));
			else c_put_str(TERM_WHITE, cur, 12 + (os/5), 1 + 16 * (os%5));
			put_str("                                   ", 3, 40);
//			if(cs == MAX_RACES)
			if(cs == MAX_BIRTH_RACES)
			{
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                               ", 4, 40);
				put_str("                                               ", 5, 40);
			}
			else
			{
				rp_ptr = &race_info[cs];
				str = rp_ptr->title;
#ifdef JP
				sprintf(cur, "%c%c%s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				///mod140105 �X�R�A�{���L�q
				//put_str("�̎푰�C��", 3, 40+strlen(rp_ptr->title));
				//put_str("�r�� �m�\ ���� ��p �ϋv ���� �o�� ", 4, 40);
				put_str("�̎푰�C��", 3, 40+strlen(rp_ptr->title));
				put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��  �X�R�A�{��", 4, 40);
#else
				sprintf(cur, "%c%c %s", sym[cs], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				put_str(": Race modification", 3, 40+strlen(rp_ptr->title));
				put_str("Str  Int  Wis  Dex  Con  Chr   EXP ", 4, 40);
#endif
				if(cs == RACE_ANDROID)
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d  **** %4d%% ",
					///mod140119 �A���h���C�h�͌o���l�␳��\�����Ȃ��悤�ɂ���
					rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
					rp_ptr->r_adj[4], rp_ptr->r_adj[5], rp_ptr->score_mult);

				else
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
					///mod140105 �Ō�ɃX�R�A�{���L�q��ǉ�����
					rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
					rp_ptr->r_adj[4], rp_ptr->r_adj[5], (rp_ptr->r_exp - 100), rp_ptr->score_mult);


				c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/5), 1 + 16 * (cs%5));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		//sprintf(buf, "�푰��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[MAX_RACES-1]);
		sprintf(buf, "�푰��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[MAX_BIRTH_RACES-1]);
#else
		sprintf(buf, "Choose a race (%c-%c) ('=' for options): ", sym[0], sym[MAX_RACES-1]);
#endif

		put_str(buf, 10, 10);
		put_str("�D���̎푰�͖������ł��B ", 11, 10);

		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
//			if(cs == MAX_RACES)
			if(cs == MAX_BIRTH_RACES)//�����_��
			{
//				k = randint0(MAX_RACES);
				k = randint0(MAX_BIRTH_RACES);
				cs = k;
				continue;
			}
			else
			{
				k = cs;
				break;
			}
		}
		if (c == '*')
		{
//			k = randint0(MAX_RACES);
			k = randint0(MAX_BIRTH_RACES);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 5) cs -= 5;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
//			if (cs < MAX_RACES) cs++;
			if (cs < MAX_BIRTH_RACES) cs++;
		}
		if (c == '2')
		{
//			if ((cs + 5) <= MAX_RACES) cs += 5;
			if ((cs + 5) <= MAX_BIRTH_RACES) cs += 5;
		}
		k = (islower(c) ? A2I(c) : -1);
//		if ((k >= 0) && (k < MAX_RACES))
		if ((k >= 0) && (k < MAX_BIRTH_RACES))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
//		if ((k >= 26) && (k < MAX_RACES))
		if ((k >= 26) && (k < MAX_BIRTH_RACES))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?' && cs < MAX_BIRTH_RACES)
		{
			char help_entry[64];
			sprintf(help_entry,"traceclas.txt#%s",help_index[cs]);
#ifdef JP
			show_help(help_entry);
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif
			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set race */
	p_ptr->prace = k;

	rp_ptr = &race_info[p_ptr->prace];

	/* Display */
	c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

	/* Success */
	return TRUE;
}
#endif


/*:::p_ptr->prace�����肷��*/

//v1.1.65 MAX_BIRTH_RACES(32)�𒴂���푰�C���f�b�N�X��������悤���C
//race_info[]�ɋL�^����Ă���N���X�̒����珉���I���\�ȃN���X�����X�g�\�����A�I�����󂯕t���Ă��̃C���f�b�N�X��p_ptr->prace�Ɋi�[���ďI��


static bool get_player_race(void)
{
	int     n;
	cptr    str;
	char    c;
	char	sym[MAX_RACES];
	char    p2 = ')';
	char    buf[80], cur[80];

	int	old_cur_pos = 0, new_cur_pos = 0;//�Â��J�[�\���ʒu�ƐV�����J�[�\���ʒu�@race_idx_list[]�̃C���f�b�N�X�ɑΉ�
	int	selecting_race_idx;


	int race_idx_list[MAX_RACES + 1], i;
	int valid_race_idx_count;//�����I���\�Ȏ푰��

					  //�푰idx���X�g�ɏ����I���\�Ȏ푰idx���i�[���Ă���
	valid_race_idx_count = 0;
	for (i = 0; i<MAX_RACES; i++)
	{

		//flag_nobirth(�����I��s��)�Aflag_special(����ϐg��p)�Aflag_only_unique(���j�[�N�N���X��p)��e��.
		//flag_nofixed(������)�͌�ŏ�������
		if (race_info[i].flag_nobirth || race_info[i].flag_special || race_info[i].flag_only_unique) continue;

		race_idx_list[valid_race_idx_count] = i;
		valid_race_idx_count++;

	}

	//���X�g�̍Ō�Ɂu�����_���I���v�������_�~�[�푰idx�Ƃ���-1�����Ă���
	race_idx_list[valid_race_idx_count] = -1;


	//�v���Ǝv�����O�̂���
	if (valid_race_idx_count > 52 || !valid_race_idx_count)
	{
		put_str("���x����a�`Z�܂łő���Ȃ����J�E���g�l����������", 23, 5);
		inkey();
		return FALSE;
	}

	/* Extra info */
	clear_from(10);
	put_str("���ӁF�s�푰�t�ɂ���ăL�����N�^�[�̐�V�I�Ȏ�����{�[�i�X���ω����܂��B", 23, 5);

	//�푰�������x���t���ŕ\���B�u�����_���v�͂��Ƃŕ`�悷��
	for (n = 0; n < valid_race_idx_count; n++)
	{
		/* Analyze */
		rp_ptr = &race_info[race_idx_list[n]];
		str = rp_ptr->title;

		//�u�V�����J�[�\���ʒu�v�ɁA���ݐݒ肳��Ă���prace�̕\���ʒu���Z�b�g����B
		//�푰�I����L�����Z�����Ă�����x���̊֐��ɗ����Ƃ��Aprace�ɂ͂��̂Ƃ��I�����ꂽ�푰idx�������Ă���B
		//�S���̍ŏ��̏ꍇplayer_wipe()�ɂ��0(�l��)�������Ă���͂��B
		if (p_ptr->prace == race_idx_list[n]) new_cur_pos = n;

		/* Display */
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);
		sprintf(buf, "%c%c%s", sym[n], p2, str);

		if (rp_ptr->flag_nofixed)
			c_put_str(TERM_L_DARK, buf, 12 + (n / 5), 1 + 16 * (n % 5));
		else
			put_str(buf, 12 + (n / 5), 1 + 16 * (n % 5));

	}

	selecting_race_idx = -1;

	//old_cur_pos�ɂ͍ŏ������_���I���̏ꏊ�����Ă����B
	//���̌�́u�Â��J�[�\���ʒu�̐F��߂����߂ɕ`�悵�����v�����ł��łɂ܂�������Ă��Ȃ������_���I������`�悷�邽�߁B
	old_cur_pos = valid_race_idx_count;
	sprintf(cur, "%c%c%s", '*', p2, "�����_��");

	//�L�[���͂��󂯕t���A�����L�[�����͂��ꂽ��J�[�\����`�悵�����̐F��ς��Č��̏ꏊ�̐F��߂�
	while (1)
	{

		//�Â�race_idx�ƐV����race_idx
		int os = race_idx_list[old_cur_pos];
		int cs = race_idx_list[new_cur_pos];

		/* Move Cursol */
		if (old_cur_pos != new_cur_pos)
		{

			//�O�ɃJ�[�\�����������ꏊ�̍ĕ`��B���������͈Â�����B�ŏ��ɂ��̃��[�v�ɓ������Ƃ��u�����_���v�̃G���g�������߂ĕ`�悳���
			if (os >= 0 && race_info[os].flag_nofixed)
				c_put_str(TERM_L_DARK, cur, 12 + (old_cur_pos / 5), 1 + 16 * (old_cur_pos % 5));
			else
				c_put_str(TERM_WHITE, cur, 12 + (old_cur_pos / 5), 1 + 16 * (old_cur_pos % 5));

			//��ʏ�̊e�푰�p�����[�^��񗓂ƌ��݃J�[�\�����������Ă���ꏊ�̍ĕ`��
			put_str("                                   ", 3, 40);
			if (cs < 0)
			{
				sprintf(cur, "%c%c%s", '*', p2, "�����_��");
				put_str("                                               ", 4, 40);
				put_str("                                               ", 5, 40);
			}
			else
			{
				rp_ptr = &race_info[cs];
				str = rp_ptr->title;
				sprintf(cur, "%c%c%s", sym[new_cur_pos], p2, str);
				c_put_str(TERM_L_BLUE, rp_ptr->title, 3, 40);
				///mod140105 �X�R�A�{���L�q
				put_str("�̎푰�C��", 3, 40 + strlen(rp_ptr->title));
				put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��  �X�R�A�{��", 4, 40);

				if (cs == RACE_ANDROID)
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d  **** %4d%% ",
						///mod140119 �A���h���C�h�͌o���l�␳��\�����Ȃ��悤�ɂ���
						rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
						rp_ptr->r_adj[4], rp_ptr->r_adj[5], rp_ptr->score_mult);

				else
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
						///mod140105 �Ō�ɃX�R�A�{���L�q��ǉ�����
						rp_ptr->r_adj[0], rp_ptr->r_adj[1], rp_ptr->r_adj[2], rp_ptr->r_adj[3],
						rp_ptr->r_adj[4], rp_ptr->r_adj[5], (rp_ptr->r_exp - 100), rp_ptr->score_mult);


				c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (new_cur_pos / 5), 1 + 16 * (new_cur_pos % 5));
			old_cur_pos = new_cur_pos;
		}

		if (selecting_race_idx >= 0) break;

		sprintf(buf, "�푰��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[valid_race_idx_count - 1]);

		put_str(buf, 10, 10);
		put_str("�D���̎푰�͖������ł��B ", 11, 10);

		//���͂��ꂽ�L�[�ɂ���Ď푰�I������������J�[�\�����������������蕪��
		c = inkey();

		if (c == 'Q')
		{
			birth_quit();
		}
		else if (c == 'S' || c == ESCAPE)
		{
			return (FALSE);
		}
		else if (c == ' ' || c == '\r' || c == '\n')
		{
			if (race_idx_list[new_cur_pos] < 0)//�����_���̂Ƃ��̓����_���Ȓl��I�����ꂽidx�Ƃ��A�I�����������łȂ����[�v���������ăJ�[�\���ĕ`�悵�Ă���I��
			{
				new_cur_pos = randint0(valid_race_idx_count);
				selecting_race_idx = race_idx_list[new_cur_pos];
			}
			else	//�J�[�\���ʒu�Œ��ڃ��^�[���������ꂽ�炻�̂܂ܑI���������I��
			{
				selecting_race_idx = race_idx_list[new_cur_pos];
				break;
			}
		}
		else if (c == '*')
		{
			new_cur_pos = randint0(valid_race_idx_count);
			selecting_race_idx = race_idx_list[new_cur_pos];
		}
		//�����L�[�ɂ��J�[�\���ʒu���ړ�
		else if (c == '8')
		{
			if (new_cur_pos >= 5) new_cur_pos -= 5;
		}
		else if (c == '4')
		{
			if (new_cur_pos > 0) new_cur_pos--;
		}
		else if (c == '6')
		{
			if (new_cur_pos < valid_race_idx_count) new_cur_pos++;
		}
		else if (c == '2')
		{
			if ((new_cur_pos + 5) <= valid_race_idx_count) new_cur_pos += 5;
		}
		//�A���t�@�x�b�g���͂ɂ��J�[�\���ʒu���ړ��ƑI������
		else if (islower(c) && A2I(c) < valid_race_idx_count)
		{
			new_cur_pos = A2I(c);
			selecting_race_idx = race_idx_list[new_cur_pos];
			continue;
		}
		else if (isupper(c) && (c - 'A' + 26) < valid_race_idx_count)
		{
			new_cur_pos = (c - 'A' + 26);
			selecting_race_idx = race_idx_list[new_cur_pos];
			continue;
		}
		//�J�[�\��������ʒu�̃w���v�t�@�C���\��
		else if (c == '?' && new_cur_pos < valid_race_idx_count )
		{
			char help_entry[64];

			rp_ptr = &race_info[cs];
			sprintf(help_entry, "traceclas.txt#%s", rp_ptr->E_title);
			show_help(help_entry);

		}
		else if (c == '=')
		{
			screen_save();
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
			screen_load();
		}
		else
		{
			bell();

		}

	}

	/* Set race */
	p_ptr->prace = selecting_race_idx;

	rp_ptr = &race_info[p_ptr->prace];

	/* Display */
	c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

	/* Success */
	return TRUE;
}






/*
 * Player class
 */
static bool get_player_class(void)
{
	int     k, n, cs, os,i;
	char    c;
	char	sym[MAX_CLASS];
	char    p2 = ')';
	char    buf[80], cur[80];
	cptr    str;

	int class_list_len=0;
	int class_list[MAX_CLASS];
	int class_idx;

	/* v2.0.19 class_info[]�̉p�E�Ɩ����g�����Ƃɂ���
	cptr help_index[MAX_CLASS_CHOICE+1] =
	{"Warrior","Mage","Priest","Explorer","Ranger","Paladin",
	"Teacher","Maid","Martial-Artist","Mindcrafter","High-Mage","Civilian",
	"Syugen","Magic-Knight","Bullet-Researcher","Archer","Magic-Eater","Engineer",
	"Librarian","Samurai","Soldier","Chemist","Cavalry","Tsukumo-Master","Secondhand-dealer","Jeweler",
	"Ninja", "Card-Dealer",""};
	*/

	//v2.0.19 ��ʐE���X�g���쐬����
	for (i = 0; i < MAX_CLASS; i++)
	{
		if (class_info[i].flag_only_unique) continue;
		if (i == CLASS_OUTSIDER) continue;

		class_list[class_list_len] = i;

		class_list_len++;
	}


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���ӁF�s�E�Ɓt�ɂ���ăL�����N�^�[�̐�V�I�Ȕ\�͂�{�[�i�X���ω����܂��B", 23, 5);
#else
	put_str("Note: Your 'class' determines various intrinsic abilities and bonuses.", 23, 5);
#endif

#ifdef JP
	//put_str("()�ň͂܂ꂽ�I�����͂��̎푰�ɂ͎�����Ȃ��E�Ƃł��B", 11, 10);
	put_str("�D���̐E�Ƃ͖������ł��B", 11, 10);
#else
	put_str("Any entries in parentheses should only be used by advanced players.", 11, 5);
#endif


	/* Dump classes */
	for (n = 0; n < class_list_len; n++)
	{
		class_idx = class_list[n];

		/* Analyze */
		cp_ptr = &class_info[class_idx];

		///mod140202 ���ʂ��ƂɐE�Ɩ��𕪂���
		if(p_ptr->psex == SEX_MALE)	str = cp_ptr->title;
		else						str = cp_ptr->f_title;

		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		/*:::�����ĂȂ��E�Ƃɂ�()������B���݂͎�����~��*/
		/* Display */
		/*
		if (!(rp_ptr->choice & (1L << n)))
#ifdef JP
			sprintf(buf, "%c%c(%s)", sym[n], p2, str);
#else
			sprintf(buf, "%c%c (%s)", sym[n], p2, str);
#endif
		else
#ifdef JP
			sprintf(buf, "%c%c%s", sym[n], p2, str);
#else
			sprintf(buf, "%c%c %s", sym[n], p2, str);
#endif
		*/

		sprintf(buf, "%c%c%s", sym[n], p2, str);

		if(cp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, buf, 13 + (n/4), 2 + 19 * (n%4));
		else put_str(buf, 13+ (n/4), 2 + 19 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a class */
	k = -1;
//	cs = p_ptr->pclass;
//	os = MAX_CLASS_CHOICE;
	cs = 0;
	os = class_list_len;
	while (1)
	{
		class_idx = class_list[cs];
		/* Move Cursol */
		if (cs != os)
		{
			if(os != class_list_len && cp_ptr->flag_nofixed) c_put_str(TERM_L_DARK, cur, 13 + (os/4), 2 + 19 * (os%4));
			else c_put_str(TERM_WHITE, cur, 13 + (os/4), 2 + 19 * (os%4));
			put_str("                                   ", 3, 40);
			if(cs == class_list_len)
			{
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                               ", 4, 40);
				put_str("                                               ", 5, 40);
			}
			else
			{
				int mul_score;
				cp_ptr = &class_info[class_idx];
				///mod140202 ���ʂ��ƂɐE�Ɩ��𕪂���
				if(p_ptr->psex == SEX_MALE)	str = cp_ptr->title;
				else						str = cp_ptr->f_title;

/*�푰�Ɍ����Ȃ��E�Ƃ�()������{�Ƃ̎d�l�𖳌�������
				if (!(rp_ptr->choice & (1L << cs)))
#ifdef JP
					sprintf(cur, "%c%c(%s)", sym[cs], p2, str);
#else
					sprintf(cur, "%c%c (%s)", sym[cs], p2, str);
#endif
				else
#ifdef JP
					sprintf(cur, "%c%c%s", sym[cs], p2, str);
#else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
#endif
*/
					sprintf(cur, "%c%c%s", sym[cs], p2, str);

#ifdef JP
					//c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					if(p_ptr->psex == SEX_MALE)	c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					else						c_put_str(TERM_L_BLUE, cp_ptr->f_title, 3, 40);

					if(p_ptr->psex == SEX_MALE)	put_str("�̐E�ƏC��", 3, 40+strlen(cp_ptr->title));
					else						put_str("�̐E�ƏC��", 3, 40+strlen(cp_ptr->f_title));
					//put_str("�̐E�ƏC��", 3, 40+strlen(cp_ptr->title));
					put_str("�r�� �m�\ ���� ��p �ϋv ���� �o�� �X�R�A", 4, 40);
#else
					c_put_str(TERM_L_BLUE, cp_ptr->title, 3, 40);
					put_str(": Class modification", 3, 40+strlen(cp_ptr->title));
					put_str("Str  Int  Wis  Dex  Con  Chr   EXP ", 4, 40);
#endif
					mul_score = cp_ptr->score_mult;
					extra_mode_score_mult(&mul_score,class_idx); 
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %3d%% ",
						cp_ptr->c_adj[0], cp_ptr->c_adj[1], cp_ptr->c_adj[2], cp_ptr->c_adj[3],
						cp_ptr->c_adj[4], cp_ptr->c_adj[5], cp_ptr->c_exp, mul_score);
					c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 13 + (cs/4), 2 + 19 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "�E�Ƃ�I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[class_list_len-1]);
#else
		sprintf(buf, "Choose a class (%c-%c) ('=' for options): ",  sym[0], sym[MAX_CLASS_CHOICE-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == class_list_len)
			{
				int tmp = randint0(class_list_len);
				k = class_list[tmp];
				cs = tmp;
				continue;
			}
			else
			{
				k = class_list[cs];
				break;
			}
		}
		if (c == '*')
		{
			int tmp = randint0(class_list_len);
			k = class_list[tmp];
			cs = tmp;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < class_list_len) cs++;
		}
		if (c == '2')
		{
			if ((cs + 4) <= class_list_len) cs += 4;
		}

		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < class_list_len))
		{
			cs = k;
			continue;
		}

		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < class_list_len))
		{
			cs = k;
			continue;
		}
		else k = -1;




		if (c == '?' && cs < class_list_len)
		{
			char help_entry[64];
//			sprintf(help_entry,"traceclas.txt#%s",help_index[cs]);
			sprintf(help_entry,"traceclas.txt#%s",class_info[class_idx].E_title);
#ifdef JP
			show_help(help_entry);
#else
			show_help("raceclas.txt#TheRaces");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set class */
	p_ptr->pclass = class_idx;
	cp_ptr = &class_info[p_ptr->pclass];
	//mp_ptr = &m_info[p_ptr->pclass];

	if(p_ptr->psex == SEX_MALE )strcpy(player_class_name, cp_ptr->title);
	else strcpy(player_class_name, cp_ptr->f_title);

	/* Display */
	//c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);
	if(p_ptr->psex == SEX_MALE)	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);
	else						c_put_str(TERM_L_BLUE, cp_ptr->f_title, 5, 15);



	return TRUE;
}


/*
 * Player seikaku
 */
static bool get_player_seikaku(void)
{
	int     k, n, os, cs, term;
	char    c;
	char	sym[MAX_SEIKAKU];
	char    p2 = ')';
	char    buf[80], cur[80];
	char    tmp[64];
	cptr    str;


	/* Extra info */
	clear_from(10);
#ifdef JP
	put_str("���ӁF�s���i�t�ɂ���ăL�����N�^�[�̔\�͂�{�[�i�X���ω����܂��B", 23, 5);
	put_str("�D�F�̐��i�͖������ł��B", 24, 6);
#else
	put_str("Note: Your personality determines various intrinsic abilities and bonuses.", 23, 5);
#endif

	/* Dump seikakus */
	for (n = 0; n < MAX_SEIKAKU; n++)
	{

		/* Analyze */
		//v1.1.32 ���ꐫ�i����
		//ap_ptr = &seikaku_info[n];
		ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,n);
		if(p_ptr->psex == SEX_MALE) str = ap_ptr->title;
		else  str = ap_ptr->f_title;
		if (n < 26)
			sym[n] = I2A(n);
		else
			sym[n] = ('A' + n - 26);

		/* Display */
		/* Display */
#ifdef JP
		sprintf(buf, "%c%c%s", I2A(n), p2, str);
#else
		sprintf(buf, "%c%c %s", I2A(n), p2, str);
#endif
		if(ap_ptr->flag_nofixed) term = TERM_L_DARK;
		else term = TERM_WHITE;
		c_put_str(term, buf, 12 + (n/4), 2 + 18 * (n%4));
	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Get a seikaku */
	k = -1;
	cs = p_ptr->pseikaku;
	os = MAX_SEIKAKU;
	while (1)
	{
		/* Move Cursol */
		if (cs != os)
		{
			if(os != MAX_SEIKAKU && ap_ptr->flag_nofixed) term = TERM_L_DARK;
			else term = TERM_WHITE;
			c_put_str(term, cur, 12 + (os/4), 2 + 18 * (os%4));
			//c_put_str(TERM_WHITE, cur, 12 + (os/4), 2 + 18 * (os%4));
			put_str("                                             ", 3, 40);
			if(cs == MAX_SEIKAKU)
			{
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
				put_str("                                           ", 4, 40);
				put_str("                                           ", 5, 40);
			}
			else
			{
				//v1.1.32 ���ꐫ�i����
				//ap_ptr = &seikaku_info[cs];
				ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,cs);
				if(p_ptr->psex == SEX_MALE) str = ap_ptr->title;
				else  str = ap_ptr->f_title;
#ifdef JP
					sprintf(cur, "%c%c%s", sym[cs], p2, str);
#else
					sprintf(cur, "%c%c %s", sym[cs], p2, str);
#endif
#ifdef JP
				if(p_ptr->psex == SEX_MALE)		
					c_put_str(TERM_L_BLUE, ap_ptr->title, 3, 40);
				else  			
					c_put_str(TERM_L_BLUE, ap_ptr->f_title, 3, 40);
				if(p_ptr->psex == SEX_MALE) 
					put_str("�̐��i�C��", 3, 40+strlen(ap_ptr->title));
				else  			
					put_str("�̐��i�C��", 3, 40+strlen(ap_ptr->f_title));

					put_str("�r�� �m�\ ���� ��p �ϋv ����      ", 4, 40);
#else
					c_put_str(TERM_L_BLUE, ap_ptr->title, 3, 40);
					put_str(": Personality modification", 3, 40+strlen(ap_ptr->title));
					put_str("Str  Int  Wis  Dex  Con  Chr       ", 4, 40);
#endif
					sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d       ",
						ap_ptr->a_adj[0], ap_ptr->a_adj[1], ap_ptr->a_adj[2], ap_ptr->a_adj[3],
						ap_ptr->a_adj[4], ap_ptr->a_adj[5]);
					c_put_str(TERM_L_BLUE, buf, 5, 40);
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/4), 2 + 18 * (cs%4));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "���i��I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", sym[0], sym[MAX_SEIKAKU-1]);
#else
		sprintf(buf, "Choose a personality (%c-%c) ('=' for options): ", sym[0], sym[MAX_SEIKAKU-1]);
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == MAX_SEIKAKU)
			{

				k = randint0(MAX_SEIKAKU);

				cs = k;
				continue;
			}
			else
			{

				k = cs;
				break;
			}
		}
		if (c == '*')
		{
			k = randint0(n);
			cs = k;
			continue;
		}
		if (c == '8')
		{
			if (cs >= 4) cs -= 4;

		}
		if (c == '4')
		{
			if (cs > 0) cs--;

		}
		if (c == '6')
		{
			if (cs < MAX_SEIKAKU) cs++;

		}
		if (c == '2')
		{
			if ((cs + 4) <= MAX_SEIKAKU) cs += 4;

		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_SEIKAKU))
		{
			cs = k;
			continue;
		}
		k = (isupper(c) ? (26 + c - 'A') : -1);
		if ((k >= 26) && (k < MAX_SEIKAKU))
		{
			cs = k;
			continue;
		}
		else k = -1;
		if (c == '?')
		{
#ifdef JP
			show_help("traceclas.txt#ThePersonalities");
#else
			show_help("raceclas.txt#ThePersonalities");
#endif
		}
		else if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if (c !='2' && c !='4' && c !='6' && c !='8') bell();
	}

	/* Set seikaku */
	//v1.1.32 ���ꐫ�i����
	//ap_ptr = &seikaku_info[p_ptr->pseikaku];
	ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,k);
	p_ptr->pseikaku = k;
#ifdef JP
	if(p_ptr->psex == SEX_MALE)		
		strcpy(tmp, ap_ptr->title);
	else
		strcpy(tmp, ap_ptr->f_title);
	if(ap_ptr->no == 1)
	strcat(tmp,"��");
#else
	strcpy(tmp, ap_ptr->title);
	strcat(tmp," ");
#endif
	strcat(tmp,player_name);


	/* Display */
	//c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, tmp, 1, 24);

	return TRUE;
}

/*:::�I�[�g���[���[�@�Œ���������l��stat_limit[]�ɐݒ肷��*/
#ifdef ALLOW_AUTOROLLER
static bool get_stat_limits(void)
{
	int i, j, m, cs, os;
	int cval[6];
	char c;
	char buf[80], cur[80];
	char inp[80];

	/* Clean up */
	clear_from(10);

	/* Extra infomation */
#ifdef JP
	put_str("�Œ���������\�͒l��ݒ肵�ĉ������B", 10, 10);
	put_str("2/8�ō��ڑI���A4/6�Œl�̑����AEnter�Ŏ���", 11, 10);
#else
	put_str("Set minimum stats.", 10, 10);
	put_str("2/8 for Select, 4/6 for Change value, Enter for Goto next", 11, 10);
#endif
	
#ifdef JP
	put_str("         ��{�l  �푰 �E�� ���i     ���v�l  �ő�l", 13, 10);
#else
	put_str("           Base   Rac  Cla  Per      Total  Maximum", 13, 10);
#endif

	/* Output the maximum stats */
	for (i = 0; i < 6; i++)
	{
		/* Reset the "success" counter */
		stat_match[i] = 0;
		cval[i] = 3;

		/* Race/Class bonus */
		j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

		/* Obtain the "maximal" stat */
		m = adjust_stat(17, j);
///mod140104 �p�����[�^����ƕ\�L��ύX�ɔ����\�L�ύX
#if 0
		/* Above 18 */
		if (m > 18)
		{
#ifdef JP
			sprintf(cur, "18/%02d", (m - 18));
#else
			sprintf(cur, "18/%02d", (m - 18));
#endif
		}
		
		/* From 3 to 18 */
		else
#endif
		{
#ifdef JP
			sprintf(cur, "%2d", m);
#else
			sprintf(cur, "%2d", m);
#endif
		}

		/* Obtain the current stat */
		m = adjust_stat(cval[i], j);
#if 0
		/* Above 18 */
		if (m > 18)
		{
#ifdef JP
			sprintf(inp, "18/%02d", (m - 18));
#else
			sprintf(inp, "18/%02d", (m - 18));
#endif
		}
		
		/* From 3 to 18 */
		else
#endif
		{
#ifdef JP
			sprintf(inp, "%2d", m);
#else
			sprintf(inp, "%2d", m);
#endif
		}

		/* Prepare a prompt */
		sprintf(buf, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s  %6s",
			stat_names[i], cval[i], rp_ptr->r_adj[i], cp_ptr->c_adj[i],
			ap_ptr->a_adj[i], inp, cur);
		
		/* Dump the prompt */
		put_str(buf, 14 + i, 10);
	}
	
	/* Get a minimum stat */
	cs = 0;
	os = 6;
	while (TRUE)
	{
		/* Move Cursol */
		if (cs != os)
		{
			if(os == 6)
			{
#ifdef JP
				c_put_str(TERM_WHITE, "���肷��", 21, 35);
#else
				c_put_str(TERM_WHITE, "Accept", 21, 35);
#endif
			}
			else if(os < 6)
				c_put_str(TERM_WHITE, cur, 14 + os, 10);
			
			if(cs == 6)
			{
#ifdef JP
				c_put_str(TERM_YELLOW, "���肷��", 21, 35);
#else
				c_put_str(TERM_YELLOW, "Accept", 21, 35);
#endif
			}
			else
			{
				/* Race/Class bonus */
				j = rp_ptr->r_adj[cs] + cp_ptr->c_adj[cs] + ap_ptr->a_adj[cs];

				/* Obtain the current stat */
				m = adjust_stat(cval[cs], j);
#if 0				
				/* Above 18 */
				if (m > 18)
				{
#ifdef JP
					sprintf(inp, "18/%02d", (m - 18));
#else
					sprintf(inp, "18/%02d", (m - 18));
#endif
				}
				
				/* From 3 to 18 */
				else
#endif
				{
#ifdef JP
					sprintf(inp, "%2d", m);
#else
					sprintf(inp, "%2d", m);
#endif
				}
				
				/* Prepare a prompt */
				sprintf(cur, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s",
					stat_names[cs], cval[cs], rp_ptr->r_adj[cs],
					cp_ptr->c_adj[cs], ap_ptr->a_adj[cs], inp);
				c_put_str(TERM_YELLOW, cur, 14 + cs, 10);
			}
			os = cs;
		}
		
		/* Prompt for the minimum stats */
		c = inkey();
		switch ( c ){
		case 'Q':
			birth_quit();
		case 'S':
			return FALSE;
		case ESCAPE:
			break;
		case ' ':
		case '\r':
		case '\n':
			if(cs == 6) break;
			cs++;
			c = '2';
			break;
		case '8':
		case 'k':
			if (cs > 0) cs--;
			break;
		case '2':
		case 'j':
			if (cs < 6) cs++;
			break;
		case '4':
		case 'h':
			if (cs != 6)
			{
				if (cval[cs] == 3)
				{
					cval[cs] = 17;
					os = 7;
				}
				else if (cval[cs] > 3)
				{
					cval[cs]--;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case '6':
		case 'l':
			if (cs != 6)
			{
				if (cval[cs] == 17)
				{
					cval[cs] = 3;
					os = 7;
				}
				else if (cval[cs] < 17)
				{
					cval[cs]++;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case 'm':
			if(cs != 6)
			{
				cval[cs] = 17;
				os = 7;
			}
			break;
		case 'n':
			if(cs != 6)
			{
				cval[cs] = 3;
				os = 7;
			}
			break;
		case '?':
#ifdef JP
			show_help("jbirth.txt#AutoRoller");
#else
			show_help("birth.txt#AutoRoller");
#endif
			break;
		case '=':
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
			break;
		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == 6))break;
	}
	
	for (i = 0; i < 6; i++)
	{
		/* Save the minimum stat */
		stat_limit[i] = cval[i];
	}

	return TRUE;
}
#endif


#define STAT_DEFAULT 10

static bool get_bonus_points(void)
{
	int i, j, m, cs, os;
	int bonus_points;
	int bonus_use_total=0;
	char c;
	char buf[80];
	int bonus_use[6],stat_sum[6];

	/* Clean up */
	clear_from(10);

	if(ironman_no_bonus) bonus_points = 0;
	else if(birth_bonus_10) bonus_points = 10;
	else if(birth_bonus_20) bonus_points = 20;
	else
	{
		bonus_points = damroll(3,6);
		while(1)
		{
			if(one_in_(3)) break;
			bonus_points++;
		}
		if(one_in_(64)) bonus_points += 10;
	}
	if(difficulty == DIFFICULTY_EASY)  bonus_points += 10;
	if(bonus_points > 99) bonus_points = 99;

	put_str("�\�͒l�Ƀ{�[�i�X�|�C���g������U���ĉ������B", 10, 10);
	put_str("2/8�ō��ڑI���A4/6�Œl�̑���", 11, 10);
	put_str("         ��{�l  �푰 �E�� ���i   �{�[�i�X  ���v�l", 13, 10);

	for (i = 0; i < 6; i++)
		bonus_use[i] = 0;

	cs = 0;
	while(1)
	{
		for (i = 0; i < 6; i++)
		{
			stat_sum[i] = STAT_DEFAULT + rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i] + bonus_use[i];

			sprintf(buf, "%6s       %2d   %+2d   %+2d   %+2d   + %3d   =   %3d",
				stat_names[i], STAT_DEFAULT, rp_ptr->r_adj[i], cp_ptr->c_adj[i],	ap_ptr->a_adj[i], bonus_use[i], stat_sum[i]);
			if(cs == i) c_put_str(TERM_YELLOW, buf, 14 + i, 10);
			else		c_put_str(TERM_WHITE , buf, 14 + i, 10);
		}
		if(cs == 6) c_put_str(TERM_YELLOW, "���肷��", 21, 35);
		else c_put_str(TERM_WHITE, "���肷��", 21, 35);
		sprintf(buf,"(�c��{�[�i�X�|�C���g�F%d)",bonus_points - bonus_use_total);
		c_put_str(TERM_WHITE, buf, 22, 35);

		c = inkey();
		switch ( c ){
		case 'Q':
			birth_quit();
		case 'S':
			return FALSE;
		case ESCAPE:
			break;
		case ' ':
		case '\r':
		case '\n':
			if(cs == 6) break;
			cs++;
			c = '2';
			break;
		case '8':
		case 'k':
			if (cs > 0) cs--;
			break;
		case '2':
		case 'j':
			if (cs < 6) cs++;
			break;
		case '4':
		case 'h':
			if (cs != 6 && bonus_use[cs] != 0)
			{
				bonus_use[cs]--;
				bonus_use_total--;
			}
			break;
		case '6':
		case 'l':
			if (cs != 6 && (bonus_use[cs] + STAT_DEFAULT < STAT_MAX_MAX) && bonus_use_total < bonus_points)
			{
				bonus_use[cs]++;
				bonus_use_total++;
			}
			break;
			///mod150103 �{�[�i�X�|�C���g�I�v�V���������̂��߃{�[�i�X�|�C���g�U�蕪����ʂ���I�v�V�����ݒ�ł��Ȃ�����
/*
		case '=':
			screen_save();
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
			screen_load();
			break;
*/
		case '?':
			show_help("tbirth.txt#BonusPoint");
			break;

		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == 6))break;
	}
	
	for (i = 0; i < 6; i++)
	{
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = STAT_DEFAULT + bonus_use[i];
	}
	return TRUE;
}


#ifdef ALLOW_AUTOROLLER
static bool get_chara_limits(void)
{
#define MAXITEMS 8

	int i, j, m, cs, os;
	int mval[MAXITEMS], cval[MAXITEMS];
	int max_percent, min_percent;
	char c;
	char buf[80], cur[80];
	cptr itemname[] = {
#ifdef JP
		"�N��",
		"�g��(�C���`)",
		"�̏d(�|���h)",
		"�Љ�I�n��"
#else
		"age",
		"height",
		"weight",
		"social class"
#endif
	};

	/* Clean up */
	clear_from(10);
	
	/* Prompt for the minimum stats */
#ifdef JP
	put_str("2/4/6/8�ō��ڑI���A+/-�Œl�̑����AEnter�Ŏ���", 11, 10);
	put_str("���ӁF�g���Ƒ̏d�̍ő�l/�ŏ��l���肬��̒l�͔��ɏo���m�����Ⴍ�Ȃ�܂��B", 23, 2);
#else
	put_str("2/4/6/8 for Select, +/- for Change value, Enter for Goto next", 11, 10);
	put_str("Caution: Values near minimum or maximum is extremery rare.", 23, 5);
#endif
	
	if (p_ptr->psex == SEX_MALE)
	{
		max_percent = (int)(rp_ptr->m_b_ht+rp_ptr->m_m_ht*4-1) * 100 / (int)(rp_ptr->m_b_ht);
		min_percent = (int)(rp_ptr->m_b_ht-rp_ptr->m_m_ht*4+1) * 100 / (int)(rp_ptr->m_b_ht);
	}
	else
	{
		max_percent = (int)(rp_ptr->f_b_ht+rp_ptr->f_m_ht*4-1) * 100 / (int)(rp_ptr->f_b_ht);
		min_percent = (int)(rp_ptr->f_b_ht-rp_ptr->f_m_ht*4+1) * 100 / (int)(rp_ptr->f_b_ht);
	}
	
#ifdef JP
	put_str("�̊i/�n�ʂ̍ŏ��l/�ő�l��ݒ肵�ĉ������B", 10, 10);
	put_str("  ��    ��                 �ŏ��l  �ő�l", 13,20);
#else
	put_str(" Parameter                    Min     Max", 13,20);
	put_str("Set minimum/maximum attribute.", 10, 10);
#endif

	/* Output the maximum stats */
	for (i = 0; i < MAXITEMS; i++)
	{
		/* Obtain the "maximal" stat */
		switch (i)
		{
		case 0:	/* Minimum age */
			m = rp_ptr->b_age + 1;
			break;
		case 1:	/* Maximum age */
			m = rp_ptr->b_age + rp_ptr->m_age;
			break;

		case 2:	/* Minimum height */
			if (p_ptr->psex == SEX_MALE) m = rp_ptr->m_b_ht-rp_ptr->m_m_ht*4+1;
			else m = rp_ptr->f_b_ht-rp_ptr->f_m_ht*4+1;
			break;
		case 3:	/* Maximum height */
			if (p_ptr->psex == SEX_MALE) m = rp_ptr->m_b_ht+rp_ptr->m_m_ht*4-1;
			else m = rp_ptr->f_b_ht+rp_ptr->f_m_ht*4-1;
			break;
		case 4:	/* Minimum weight */
			if (p_ptr->psex == SEX_MALE) m = (rp_ptr->m_b_wt * min_percent / 100) - (rp_ptr->m_m_wt * min_percent / 75) +1;
			else m = (rp_ptr->f_b_wt * min_percent / 100) - (rp_ptr->f_m_wt * min_percent / 75) +1;
			break;
		case 5:	/* Maximum weight */
			if (p_ptr->psex == SEX_MALE) m = (rp_ptr->m_b_wt * max_percent / 100) + (rp_ptr->m_m_wt * max_percent / 75) -1;
			else m = (rp_ptr->f_b_wt * max_percent / 100) + (rp_ptr->f_m_wt * max_percent / 75) -1;
			break;
		case 6:	/* Minimum social class */
			m = 1;
			break;
		case 7:	/* Maximum social class */
			m = 100;
			break;
		default:
			m = 1;
			break;
		}
		
		/* Save the maximum or minimum */
		mval[i] = m;
		cval[i] = m;
	}

	for (i = 0; i < 4; i++)
	{
		/* Prepare a prompt */
		sprintf(buf, "%-12s (%3d - %3d)", itemname[i], mval[i*2], mval[i*2+1]);

		/* Dump the prompt */
		put_str(buf, 14 + i, 20);

		for (j = 0; j < 2; j++)
		{
			sprintf(buf, "     %3d", cval[i*2+j]);
			put_str(buf, 14 + i, 45 + 8 * j);
		}
	}
	
	/* Get a minimum stat */
	cs = 0;
	os = MAXITEMS;
	while (TRUE)
	{
		/* Move Cursol */
		if (cs != os)
		{
#ifdef JP
			const char accept[] = "���肷��";
#else
			const char accept[] = "Accept";
#endif
			if(os == MAXITEMS)
			{
				c_put_str(TERM_WHITE, accept, 19, 35);
			}
			else
			{
				c_put_str(TERM_WHITE, cur, 14 + os/2, 45 + 8 * (os%2));
			}
			
			if(cs == MAXITEMS)
			{
				c_put_str(TERM_YELLOW, accept, 19, 35);
			}
			else
			{
				/* Prepare a prompt */
				sprintf(cur, "     %3d", cval[cs]);
				c_put_str(TERM_YELLOW, cur, 14 + cs/2, 45 + 8 * (cs%2));
			}
			os = cs;
		}
		
		/* Prompt for the minimum stats */
		c = inkey();
		switch (c){
		case 'Q':
			birth_quit();
		case 'S':
			return (FALSE);
		case ESCAPE:
			break; /*��ł������break�����*/
		case ' ':
		case '\r':
		case '\n':
			if(cs == MAXITEMS) break;
			cs++;
			c = '6';
			break;
		case '8':
		case 'k':
			if (cs-2 >= 0) cs -= 2;
			break;
		case '2':
		case 'j':
			if (cs < MAXITEMS) cs += 2;
			if (cs > MAXITEMS) cs = MAXITEMS;
			break;
		case '4':
		case 'h':
			if (cs > 0) cs--;
			break;
		case '6':
		case 'l':
			if (cs < MAXITEMS) cs++;
			break;
		case '-':
		case '<':
			if (cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] > cval[cs-1])
					{
						cval[cs]--;
						os = 127;
					}
				}
				else
				{
					if(cval[cs] > mval[cs])
					{
						cval[cs]--;
						os = 127;
					}
				}
			}
			break;
		case '+':
		case '>':
			if (cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] < mval[cs])
					{
						cval[cs]++;
						os = 127;
					}
				}
				else
				{
					if(cval[cs] < cval[cs+1])
					{
						cval[cs]++;
						os = 127;
					}
				}
			}
			break;
		case 'm':
			if(cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] < mval[cs])
					{
						cval[cs] = mval[cs];
						os = 127;
					}
				}
				else
				{
					if(cval[cs] < cval[cs+1])
					{
						cval[cs] = cval[cs+1];
						os = 127;
					}
				}
			}
			break;
		case 'n':
			if(cs != MAXITEMS)
			{
				if(cs%2)
				{
					if(cval[cs] > cval[cs-1])
					{
						cval[cs] = cval[cs-1];
						os = 255;
					}
				}
				else
				{
					if(cval[cs] > mval[cs])
					{
						cval[cs] = mval[cs];
						os = 255;
					}
				}
			}
			break;
		case '?':
#ifdef JP
			show_help("jbirth.txt#AutoRoller");
#else
			show_help("birth.txt#AutoRoller");
#endif
			break;
		case '=':
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
			break;
		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == MAXITEMS))break;
	}

	/* Input the minimum stats */
	chara_limit.agemin = cval[0];
	chara_limit.agemax = cval[1];
	chara_limit.htmin = cval[2];
	chara_limit.htmax = cval[3];
	chara_limit.wtmin = cval[4];
	chara_limit.wtmax = cval[5];
	chara_limit.scmin = cval[6];
	chara_limit.scmax = cval[7];

	return TRUE;
}
#endif

#define HISTPREF_LIMIT 1024
static char *histpref_buf = NULL;

/*
 * Hook function for reading the histpref.prf file.
 */
void add_history_from_pref_line(cptr t)
{
	/* Do nothing if the buffer is not ready */
	if (!histpref_buf) return;

	my_strcat(histpref_buf, t, HISTPREF_LIMIT);
}


static bool do_cmd_histpref(void)
{
	char buf[80];
	errr err;
	int i, j, n;
	char *s, *t;
	char temp[64 * 4];
	char histbuf[HISTPREF_LIMIT];

#ifdef JP
	if (!get_check("���������ݒ�t�@�C�������[�h���܂���? ")) return FALSE;
#else
	if (!get_check("Load background history preference file? ")) return FALSE;
#endif

	/* Prepare the buffer */
	histbuf[0] = '\0';
	histpref_buf = histbuf;

#ifdef JP
	sprintf(buf, "histedit-%s.prf", player_base);
#else
	sprintf(buf, "histpref-%s.prf", player_base);
#endif
	err = process_histpref_file(buf);

	/* Process 'hist????.prf' if 'hist????-<name>.prf' doesn't exist */
	if (0 > err)
	{
#ifdef JP
		strcpy(buf, "histedit.prf");
#else
		strcpy(buf, "histpref.prf");
#endif
		err = process_histpref_file(buf);
	}

	if (err)
	{
#ifdef JP
		msg_print("���������ݒ�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B");
#else
		msg_print("Failed to load background history preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}
	else if (!histpref_buf[0])
	{
#ifdef JP
		msg_print("�L���Ȑ��������ݒ�͂��̃t�@�C���ɂ���܂���B");
#else
		msg_print("There does not exist valid background history preference.");
#endif
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}

	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) p_ptr->history[i][0] = '\0';

	/* Skip leading spaces */
	for (s = histpref_buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */
	while ((n > 0) && (s[n - 1] == ' ')) s[--n] = '\0';

	roff_to_buf(s, 60, temp, sizeof(temp));
	t = temp;
	for (i = 0; i < 4; i++)
	{
		if (t[0] == 0) break;
		else
		{
			strcpy(p_ptr->history[i], t);
			t += strlen(t) + 1;
		}
	}

	/* Fill the remaining spaces */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; p_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) p_ptr->history[i][j] = ' ';
		p_ptr->history[i][59] = '\0';
	}

	/* Kill the buffer */
	histpref_buf = NULL;

	return TRUE;
}

/*
 *  Character background edit-mode
 */
/*:::�������� p_ptr->history[][]��ҏW����*/
static void edit_history(void)
{
	char old_history[4][60];
	int y = 0, x = 0;
	int i, j;

	/* Edit character background */
	for (i = 0; i < 4; i++)
	{
		sprintf(old_history[i], "%s", p_ptr->history[i]);
	}
	/* Turn 0 to space */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; p_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) p_ptr->history[i][j] = ' ';
		p_ptr->history[i][59] = '\0';
	}
	display_player(1);
#ifdef JP
	c_put_str(TERM_L_GREEN, "(�L�����N�^�[�̐������� - �ҏW���[�h)", 11, 20);
	put_str("[ �J�[�\���L�[�ňړ��AEnter�ŏI���ACtrl-A�Ńt�@�C���ǂݍ��� ]", 17, 10);
#else
	c_put_str(TERM_L_GREEN, "(Character Background - Edit Mode)", 11, 20);
	put_str("[ Cursor key for Move, Enter for End, Ctrl-A for Read pref ]", 17, 10);
#endif

	while (TRUE)
	{
		int skey;
		char c;

		for (i = 0; i < 4; i++)
		{
			put_str(p_ptr->history[i], i + 12, 10);
		}
#ifdef JP
		if (iskanji2(p_ptr->history[y], x))
			c_put_str(TERM_L_BLUE, format("%c%c", p_ptr->history[y][x],p_ptr->history[y][x+1]), y + 12, x + 10);
		else
#endif
		c_put_str(TERM_L_BLUE, format("%c", p_ptr->history[y][x]), y + 12, x + 10);

		/* Place cursor just after cost of current stat */
		Term_gotoxy(x + 10, y + 12);

		/* Get special key code */
		skey = inkey_special(TRUE);

		/* Get a character code */
		if (!(skey & SKEY_MASK)) c = (char)skey;
		else c = 0;

		if (skey == SKEY_UP || c == KTRL('p'))
		{
			y--;
			if (y < 0) y = 3;
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_DOWN || c == KTRL('n'))
		{
			y++;
			if (y > 3) y = 0;
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (skey == SKEY_RIGHT || c == KTRL('f'))
		{
#ifdef JP
			if (iskanji2(p_ptr->history[y], x)) x++;
#endif
			x++;
			if (x > 58)
			{
				x = 0;
				if (y < 3) y++;
			}
		}
		else if (skey == SKEY_LEFT || c == KTRL('b'))
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = 58;
				}
				else x = 0;
			}

#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '\r' || c == '\n')
		{
			Term_erase(0, 11, 255);
			Term_erase(0, 17, 255);
#ifdef JP
			put_str("(�L�����N�^�[�̐������� - �ҏW�ς�)", 11, 20);
#else
			put_str("(Character Background - Edited)", 11, 20);
#endif
			break;
		}
		else if (c == ESCAPE)
		{
			clear_from(11);
#ifdef JP
			put_str("(�L�����N�^�[�̐�������)", 11, 25);
#else
			put_str("(Character Background)", 11, 25);
#endif

			for (i = 0; i < 4; i++)
			{
				sprintf(p_ptr->history[i], "%s", old_history[i]);
				put_str(p_ptr->history[i], i + 12, 10);
			}
			break;
		}
		else if (c == KTRL('A'))
		{
			if (do_cmd_histpref())
			{
#ifdef JP
				if ((x > 0) && (iskanji2(p_ptr->history[y], x - 1))) x--;
#endif
			}
		}
		else if (c == '\010')
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = 58;
				}
				else x = 0;
			}

			p_ptr->history[y][x] = ' ';
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x - 1)))
			{
				x--;
				p_ptr->history[y][x] = ' ';
			}
#endif
		}
#ifdef JP
		else if (iskanji(c) || isprint(c))
#else
		else if (isprint(c)) /* BUGFIX */
#endif
		{
#ifdef JP
			if (iskanji2(p_ptr->history[y], x))
			{
				p_ptr->history[y][x+1] = ' ';
			}

			if (iskanji(c))
			{
				if (x > 57)
				{
					x = 0;
					y++;
					if (y > 3) y = 0;
				}

				if (iskanji2(p_ptr->history[y], x+1))
				{
					p_ptr->history[y][x+2] = ' ';
				}

				p_ptr->history[y][x++] = c;

				c = inkey();
			}
#endif
			p_ptr->history[y][x++] = c;
			if (x > 58)
			{
				x = 0;
				y++;
				if (y > 3) y = 0;
			}
		}
	} /* while (TRUE) */

}

//�o����i�@����select_unique_entry[]�̓Y���Ɉ�v����悤��
#define ENTRY_REIMARI	0
#define ENTRY_KOUMA		1
#define ENTRY_YOUMU		2
#define ENTRY_EIYA		3
#define ENTRY_KAEI		4
#define ENTRY_HUUZIN	5
#define ENTRY_TIREI		6
#define ENTRY_SEIREN	7
#define ENTRY_SHINREI	8
#define ENTRY_KISHIN	9
#define ENTRY_KANZYU	10
#define ENTRY_TENKUU	11
#define ENTRY_KIKEI		12
#define ENTRY_KOURYUU	13
#define ENTRY_JUUOUEN	14
#define ENTRY_TASOGARE	15
#define ENTRY_OTHER		16


#define ENTRY_MAX 17 //��+1


const	cptr select_unique_entry[ENTRY_MAX] = {
		"a)��Ԏ��@��l",
		"b)�g����",
		"c)�d�X��",
		"d)�i�鏴",
		"e)�ԉf��",
		"f)���_�^",
		"g)�n��a",
		"h)���@�D",
		"i)�_��_",
		"j)�P�j��",
		"k)����`",
		"l)�V����",
		"m)�S�`�b",
		"n)������",
		"o)�b����",
		"p)�����e���A�N�V����",
		"q)���̑��̍�i",
	};

typedef struct unique_player_type unique_player_type;
struct unique_player_type
{
	bool flag_fix;  //�܂���������ĂȂ���FALSE
	cptr name;		//���O
	int class_id;	//CLASS_???�̒l
	int race;		//RACE_???�̒l
	int entry;		//ENTRY_???�̒l�@�o����i
	int sex;		//���ʁ@���Ԃ񍡌���S���������ꉞ
	cptr info;	//�L�������C�N���ɕ\������������
};

#define UNIQUE_PLAYER_NUM 127
#define CLASS_DUMMY 255
#define RACE_DUMMY 255
static unique_player_type unique_player_table[UNIQUE_PLAYER_NUM] =
{
	{TRUE,"����@�얲",CLASS_REIMU,RACE_HUMAN,ENTRY_REIMARI,SEX_FEMALE,
	"���Ȃ��͌��z���̊����ٕ̈ς������ɓ���������̛ޏ����̐l�ł��B�ɂ߂č������ݔ\�͂������܂����ӂ��҂Ȃ̂ł��܂���C������܂���B�������_�Ђւ̐M��(��̓I�ɂ͂��ΑK)�����܂�΂��C���o�邩������܂���B���ΑK�͓G��|������N�G�X�g�����Ȃ��ƎQ�q�҂������Ă��񂾂񗭂܂��Ă����܂��B�܂������Z�[�u�t�@�C���̈ȑO�̖`���ŒN�������ꂽ���ΑK�͑S�Ĉ����p����܂��B�ŏI�I�ɂ͔������݂������ɂȂ邱�Ƃł��傤�B�Ȃ��X�R�A�͋ɂ߂ĒႭ�Ȃ�܂��B�ޏ����ǂ����Ƀt���t�����ōs���Ĉٕς̎�d�҂�@���̂߂��ċA���Ă��邱�Ƃ͌��z���ɂ����ē��풃�ю��ł���N�������Ȃ����߂ł��B"},
	{TRUE,"���J�@������",CLASS_MARISA,RACE_HUMAN,ENTRY_REIMARI,SEX_FEMALE,
	"���Ȃ��͕��ʂ̖��@�g�������̂��Ă��܂����A���Ȃ����܂������ٕ̈ς������ɓ��������z���̉p�Y�ł��B�g�̌y���Ǝ�Ȃ̈����A�����ċ��͂ȍU�����@���g��ł��B���Ȃ��̓Ǝ��̖��@�͎g�p���@��������ŁA�����Ȗ��@�����������Đ��@���K�����A�f�ނ��W�߂Ē��o�������͂��疂�@�����A��������@�����������ď����`�Ŏg�p���܂��B�f�ނ̎��W�͒n���ȍ�Ƃł����A���ꂽ���@�̈З͂͐؂�D�ƌĂԂɂӂ��킵�����̂ł��B�܂������Z�[�u�t�@�C���̈ȑO�̖`���ŒN�������J���@�X�ɓn�����f�ރA�C�e�������͂Ƃ��Ē~�ς���Ă��܂��B���@�����Ȃ��Ă����͂ȓ��Z���K�����܂��B�Ȃ��ʏ�̖��@���K���\�ŁA���̈�Ƒ��̈�����R�ɕύX���邱�Ƃ��ł��܂��B"},

	{TRUE,"���[�~�A",CLASS_RUMIA,RACE_YOUKAI,ENTRY_KOUMA,SEX_FEMALE,
	"���Ȃ��͌��z���̋�����Ă��Ȃ��Y���d���ł��B�ł𑀂�\�͂�����A�Í��U���Ɋւ���X�L���Ƒϐ��������܂��B�g�̔\�͂͒ʏ�̐l�Ԃ��͍��߂ł����d���Ƃ��Ă͂������}�ł��B����̈����͂����Ŗ��@�͎g���܂���B��������ƈÍ��U����S���󂯕t���Ȃ��Ȃ�܂��B"},
	{TRUE,"�`���m",CLASS_CIRNO,RACE_FAIRY,ENTRY_KOUMA,SEX_FEMALE,
	"���Ȃ��͕��i�͖��̌΂ŗV��ŕ�炵�Ă���X�̗d���ł��B�d���Ƃ��Ă͋K�i�O�Ƃ�����قǋ����͂������܂����A����͂����܂ł��d���̒��ł̘b�ł��B�X�̗d���Ȃ̂ŗ�C�U���ɋ����A��������Ɨ�C�U����S���󂯕t���Ȃ��Ȃ�܂��B�������Ή��͂��Ȃ��̑�G�ł��B��C�֌W�̓��Z���L�x�ł����A���@���Ȃǂ͂܂������ǂ߂�������̈��������ł��B�T��������ȂǍׂ�����Ƃ����ł��B"},
	{TRUE,"�g�@����",CLASS_MEIRIN,RACE_YOUKAI,ENTRY_KOUMA,SEX_FEMALE,
	"���Ȃ��͍g���قŖ�Ԃ𖱂߂�d���ŁA���p�̒B�l�ł��B���Ȃ��̊i�����̒ʏ�U���͈�s��������̍U���񐔂���߂ł����A����s���͂��ʏ�����Z���Ȃ�܂��B�i���U����A���ōs���΍s���قǏ���s���͂�����Ɍ�������I�ȍU���͂��㏸���܂��B�茘���������Ζh��ɒ����܂����A���X�N�����΂��Ȃ�̍U���͂𔭊����邱�Ƃ��\�ł��B�C�𑀂�\�͂������Ă���A���m��⏕���������Ȃ��܂��B�����x���ɂȂ�Ɣj�׍U������_�Ƃ��Ȃ��Ȃ�܂��B"},
	{TRUE,"�p�`�����[�E�m�[���b�W",CLASS_PATCHOULI,RACE_MAGICIAN,ENTRY_KOUMA,SEX_FEMALE,
	"���Ȃ��͐��܂���̖��@�g���ŁA����܂ł̐l���̂قƂ�ǂ�Ǐ��ɔ�₵�ė��܂����B���݂͍g���ق̑�}���قɏZ��ł��ĊO�o�͂����H�ł��B���Ȃ��͖c��Ȗ��@�̒m���������Ă���A�Q�[���J�n���_�Ŗ��p��4�̈�S�Ă��L�����Ă��܂��B���x��������Ă��Ė��@���������Ă���Ζ��@���s�g�\�ł��B����Ɏ��j�𑀂鋭�͂Ȗ��@�������A���Ԍo�߂ɂ�閂�͂̉񕜂��ɂ߂đ����Ƃ�������������܂��B�������g�̔\�͂͂��Ȃ�n��ŏd�����̂������������A�܂��Đڋߐ�Ȃǂ͑S���̖��d�ł��B�b���������Ă���A�b���̒��q�������Ȃ�Ɩ��@�̎��s�����オ��A�ꍇ�ɂ���Ă͊�����ǂނ��Ƃ��玸�s���邩������܂���B�����̖�␶���̖�ȂǂŚb���̒��q���ǂ��Ȃ�܂��B"},
	{TRUE,"�\�Z��@���",CLASS_SAKUYA,RACE_HUMAN,ENTRY_KOUMA,SEX_FEMALE,
	"���Ȃ��͍g���ق����ɐ؂萷�肷��l�Ԃ̃��C�h�ŁA��̓i�C�t�̏N�W�ł��B���Ȃ��͎��Ԃ𑀂�Ƃ����l�ԗ��ꂵ���\�͂������Ă���A�^�[�������Ȃ��Ŏ��Ԃ��~�߂邱�Ƃ��ł��܂��B���Ԓ�~���ɓG�ɒ��ڍU�����邱�Ƃ͂ł��܂��񂪁A���Ԃ��~�߂��܂܍D���Ȃ��������������x�e���邱�Ƃ��ł��邽�߁A���m��T���̊T�O���S����������ɂȂ�܂��B�ق��ɂ��i�C�t������e���|�[�g�ȂǂɊւ�����Z��L�x�ɏK�����܂��B���Ȃ��̓��C�h�Ȃ̂ŏ�Ƀ��C�h���𒅂Ȃ���΂����܂���B"},	
	{TRUE,"���~���A�E�X�J�[���b�g",CLASS_REMY,RACE_VAMPIRE,ENTRY_KOUMA,SEX_FEMALE,
	"���Ȃ��͍g���ق̎�ŁA�^���𑀂�͂����Ƃ������͂ȋz���S�ł��B������ɋɂ߂ċ����A�����̋Z�\�ɗD��A���l�ȍU���ւ̑ϐ��������A���@�̏K�����\�ŁA���̏㋭�͂ȃA�C�e�����悭��ɓ���܂��B�������z���S�Ȃ̂Ŕj�ׁE�M���E���̍U���ɑ΂��Ă͐Ǝ�ł��B���͂ȕ��X�R�A�͂��Ȃ�Ⴍ�Ȃ�܂��B"},
	{TRUE,"�t�����h�[���E�X�J�[���b�g",CLASS_FLAN,RACE_VAMPIRE,ENTRY_KOUMA,SEX_FEMALE,
	"���Ȃ��͍g���ق̎�̖��ŁA�o�Ɠ��l���͂ȋz���S�ł��B���S�N���n�����ɂ��ĊO�ɂȂǋ����������܂���ł����B�j��̔\�͂������Ă���A�_���W�����̃g���b�v��h�A�Ȃǂ������Q��В[����󂵂Đi�݂܂��B���������Ȃ��̔j��̗͂͂��΂��Ή󂷂ׂ��łȂ����̂܂ł�������󂵂܂��B�l�X�ȓ��Z�������܂��������̑����͂�͂�j��I���s����ł��B���Ȃ��̓��قȐ��_�͂ǂ̂悤�ȋ��낵�����̂����Ă����ނ��Ƃ͂���܂���B"},

	{TRUE,"���e�B�E�z���C�g���b�N",CLASS_LETTY,RACE_YOUKAI,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͊��C�𑀂�d���ł��B�����Ԃ͂ǂ����ɉB��Z��ł��ē~�ɂȂ�ƊO�ɏo�Ă��܂��B��C�E�����U�������ӂŁA���܂������ΓG�ɍs���������ɓ|�����Ƃ��\�ł��B�����ɑ΂��Ă͗L���ɐ킦�܂����A���f�b�h��S�[���������肾�ƂقƂ�ǖ��͂ł��B��C�ɋ����ϐ���������ŉ��͂��Ȃ��̑�G�ł��B�`���̏��Ղ͊y�ɂ��Ȃ��܂����I�Ղ͂��Ȃ�̋�J���������邱�Ƃł��傤�B���Ȃ��͎���C���ς���悤�ȗ͎͂����Ă��܂��񂪁A����̒��ł͍ő�̗͂𔭊����邱�Ƃ��ł��܂��B"},
	{TRUE,"��",CLASS_CHEN,RACE_WARBEAST,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͑�d�����_���̎��ł��锪�_���̂��̂܂����ł��B�c���͂܂�����܂���B�G�̕s�ӂ��Ղ��̂����܂��A���ꂽ�G�Ɉ�u�ŋ삯�������ꌂ�������ė��E����ȂǑf�����s���𓾈ӂƂ��܂��B���������U�����󂯂��蕂�V�����ɐ��ɓ���Ǝ���������Ď�̉����Ă��܂��܂��B���̜߂��Ă��Ȃ����Ȃ��͂������}�ȉ����L�ł��B����������Ă���Ƃ��ɒT�����_�ɍs���Ǝ�Ɏ���߂������Ă��炦�܂��B����▂����̈����͂����ł��B"},
	{TRUE,"�A���X�E�}�[�K�g���C�h",CLASS_ALICE,RACE_MAGICIAN,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͐l�`�����ł��B���S�Ɏ������ē����l�`����邱�Ƃ�ڎw�����X�����𑱂��Ă��܂��B���Ȃ��̐퓬�X�^�C���͓Ɠ��ŁA����͕�����������ɖ��@�̎��ő���l�`�����𕐑������Đ킢�܂��B�l�`�𑀂�T��Ŗ��@����̈�s�g�\�ł��B�l�`�̐��̓��x���A�b�v�ɂ�葝���A�l�`�̍U���͂͒m�\�A��p���A�l�`�̎�ށA�����Ď�����������Ō��܂�܂��B�l�`�̎���������ɂ��������ϐ���\�͏㏸���ʂȂǂ̂������͔�������܂���B�l�`�ɂ��U���͋��͂ł������Ȃ����g�͂��Ȃ�ł���ア���ߗǎ��Ȗh��̊m�ۂ��d�v�ł��B�������l�`�𑀂�ז��ɂȂ�̂ŃO���[�u�𑕔����邱�Ƃ͂ł��܂���B���Ȃ��͐����Ȗ��@����Ɏg�������Ă��邽�ߗ򉻍U������_�ł��B�R��͑S���ł��܂���B"},
	{TRUE,"�����[�z���C�g",CLASS_LILY_WHITE,RACE_FAIRY,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͏t�̓�����m�点��d���ŁA�t�����Ƃ��Ă΂�܂��B���͏t�����Č��C�����ς��̂��ߕ��ʂ̗d����薾�炩�ɋ����͂������܂��B������30���o�Ət�̏I��肪�߂Â��A���C���Ȃ����Ă��܂��܂��B����܂ł�*����*���Ȃ��ƃQ�[���I�[�o�[�ł��B���Ȃ��̐퓬�\�͍͂����Ƃ͌������A���M�ɒl����قǂ̒�������Z�������Ă��܂���B���Ȃ��̖`���͉ߍ��Ȃ��̂ƂȂ�ł��傤�B"},
	{TRUE,"���i�T�E�v���Y�����o�[",CLASS_LUNASA,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͑���̎o���̒����ł��B������Ò����Ő^�ʖڂł��B���@���K�����邱�Ƃ��ł��A���e��͂��܂�D�݂ł͂���܂���B���Ȃ��̑t�ł鉹�ɂ͕����҂̋C���𒾂߂�͂�����܂��B���Ȃ������͎���ł���X�̔p�m�قɍs���đ��̎o���Ɩ`������シ�邱�Ƃ��ł��܂��B�o���l�A��b�p�����[�^�A�����i�Ȃǂ͎o���ŋ��L����܂��B���Ȃ��͗�̂̂��߃_���W�����ŕǂ𔲂��邱�Ƃ��ł��܂��B�`���ɂ����Ĕ��ɗL���ł����X�R�A���啝�ɉ�����A�܂��j�׍U���͂��Ȃ��̑�G�ł��B�Ȃ��A��x�����̎o���ƌ�ス���ɏ���������X�R�A���オ��܂��B"},
	{TRUE,"���������E�v���Y�����o�[",CLASS_MERLIN,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͑���̎o���̎����ł��B�������邭�N�炩�ŁA���ɂ͈ӎv�̑a�ʂ�����Ȃقǃn�C�e���V�����ł��B���Ȃ��̑t�ł鉹�ɂ͕����҂̋C�������g������͂�����܂��B���Ȃ������͎���ł���X�̔p�m�قɍs���đ��̎o���Ɩ`������シ�邱�Ƃ��ł��܂��B�o���l�A��b�p�����[�^�A�����i�Ȃǂ͎o���ŋ��L����܂��B���Ȃ��͗�̂̂��߃_���W�����ŕǂ𔲂��邱�Ƃ��ł��܂��B�`���ɂ����Ĕ��ɗL���ł����X�R�A���啝�ɉ�����A�܂��j�׍U���͂��Ȃ��̑�G�ł��B�Ȃ��A��x�����̎o���ƌ�ス���ɏ���������X�R�A���オ��܂��B"},
	{TRUE,"�����J�E�v���Y�����o�[",CLASS_LYRICA,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͑���̎o���̎O���ł��B�����q�҂ł����v�Z�����ڒ[�������A���̂��Ƃ���p�ɂ��Ȃ��܂��B���Ȃ��̑t�ł鉹�͒ʏ�̐��E�ɂ͑��݂��Ȃ����ŁA�l�X�ȕs�v�c�Ȍ��ʂ��N�����܂��B���Ȃ������͎���ł���X�̔p�m�قɍs���đ��̎o���Ɩ`������シ�邱�Ƃ��ł��܂��B�o���l�A��b�p�����[�^�A�����i�Ȃǂ͎o���ŋ��L����܂��B���Ȃ��͗�̂̂��߃_���W�����ŕǂ𔲂��邱�Ƃ��ł��܂��B�`���ɂ����Ĕ��ɗL���ł����X�R�A���啝�ɉ�����A�܂��j�׍U���͂��Ȃ��̑�G�ł��B�Ȃ��A��x�����̎o���ƌ�ス���ɏ���������X�R�A���オ��܂��B"},



	{TRUE,"��鮁@�d��",CLASS_YOUMU,RACE_HALF_GHOST,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͔��l����̌��m�ł��B���E�Œ�t����q�Ƃ��Ď�Ɏd���Ă��܂��B�����Ȃ��炩�Ȃ�b�����Ă���A�񓁗��̒B�l�ł��B�u�O�ό��v�u���O���v�Ƃ����R�����铁���ŏ����珊�����Ă��܂��B���قȑ̎��ƋC�������������Ɠ��̌��Z�Ő킢�܂��B�܂����|�̔�`����ǂނ��Ƃŋ��͂ȋZ�̏K�����\�ł��B"},
	{TRUE,"���s���@�H�X�q",CLASS_YUYUKO,RACE_SPECTRE,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͔��ʘO�̎�𖱂߂鋭�͂ȖS��ł��B䩗m�Ƃ��������ڂɔ��������ߐڐ퓬�\�͂������܂��B�܂�����̈�̖��@�𓾈ӂƂ��A����Ɏ��̗͂𑀂�����_���W�����̓G������������Z�������܂��B�l�X�ȍU���ɑ΂���ϐ��������A�n���U�������S�ɖ������������őM������_�Ƃ����A����ɕǂ𔲂��Ĉړ����邱�Ƃ��ł��܂��B�_���W�����T���ɂ����Ċ����ɋ߂������͂̎�����ł����A�B��̎�_�Ƃ��Ă����ɂ������󂫂܂��B�H���͑�ʂɎ��������܂��傤�B�Ȃ����Ȃ��͂��َq��H�ׂ�MP���񕜂��邱�Ƃ��ł��܂��B"},
	{TRUE,"���_�@��",CLASS_RAN,RACE_YOUKO,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͑�d���_���̎��ł��B�傩��S���Ő[�w�ւ̎h�q�𖽂����܂����B���Ȃ����g����d���Ƃ��Ēm�������̌ςł���A�����Ď傩�狭�͂Ȏ���t�^����Ă��邽�߂�����ʂŋɂ߂č����\�͂������܂��B���������ߊ����܂ł̊����͋͂���T�Ԃł��B�e�ڂ��U�炸�Ƀ_���W�����֐��葱����K�v������ł��傤�B�Ȃ��A�S���̈ʒu�͍ŏ�����m�炳��Ă���A�҂̖��@�Ȃǂōs�����Ƃ��ł��܂��B"},
	{TRUE,"���_�@��",CLASS_YUKARI,RACE_DAIYOUKAI,ENTRY_YOUMU,SEX_FEMALE,
	"���Ȃ��͌��z���ŌÎQ�̑�d���ŁA���z�����ێ��Ǘ������ڂ𕉂��Ă��܂��B�u���E�v�𑀍삷��\�͂������A���͂�����ȓ��Z���K�����܂��B��b�I�Ȕ\�͒l���������ɒm�\�̍����͑S�E�ƒ��g�b�v�N���X�ł��B�������X�R�A�{���͋ɂ߂ĒႭ�Ȃ�A�܂����������Ȃ�x�߂ł��B"},

	{TRUE,"���O���E�i�C�g�o�O",CLASS_WRIGGLE,RACE_YOUKAI,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͌u�̗d���ł��B峂𑀂�\�͂������Ă���A峂������g�����Đ�킹������͂����m����Ȃǂ̓��Z���K�����܂��B���Ȃ��͌������Ȃ��Ă��_���W�������Ƃ炷���Ƃ��ł��A���x�����オ��ƑM���ւ̑ϐ��𓾂܂��B�����������͂��Ȃ��̑�G�ł��B���Ȃ����g�̐퓬�\�͂͂���قǍ����͂Ȃ����߁A���Ոȍ~�͋����������邩������܂���B"},
	{TRUE,"�~�X�e�B�A�E���[�����C",CLASS_MYSTIA,RACE_YOUKAI,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͉̐��Ől��f�킹��鐝�̗d���ł��B���͂��������̂��̂����ƂŒ����҂𒹖ڂɂ����荬�������邱�Ƃ��ł��܂��B�܂����n�̃����X�^�[���x�z�����菢��������Z�������܂��B�퓬�\�͕͂W���I�ł��q�����ɗD��܂��B"},
	{TRUE,"�㔒��@�d��",CLASS_KEINE,RACE_WARBEAST,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͔��b�ł����A�l�Ԃ���̐M���������l���̎��q���ŋ��t�����ĕ�炵�Ă��܂��B�퓬�ɂ����Ă͔�����A���@�Ȃǈ�ʂ�̂��Ƃ�����Ȃ�ɂ��Ȃ��܂��B���j��H�ׂ�\�͂������A�l�X�ȑ��݂��B��������Z���K�����܂��B�b�l�ɂ͒������ɂ߂č����m���������܂����A���Ȃ�̌����ŐΓ��ł��B���i�͏b�炵���Ƃ���͂قƂ�ǂ���܂��񂪁A����������Əb���ł��锒��(�n�N�^�N)�̗͂��F�Z���������I�Ƀp���[�A�b�v���܂��B����ɂȂ鎞�ɂ͑傫�Ȋp�������邽�߁A���̑����i�ɂ͋C�����Ă��������B"},
	{TRUE,"�����@�Ă�",CLASS_TEWI,RACE_WARBEAST,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͂��Ȃ�Ê��̗d���e�ŁA�i�����œe�B�𗦂��Ă��܂��B�m�ɒ����Ă���㩂�ꌂ���E����g���Đ킢�܂��B�܂��A�ǂ��ŉ҂����̂��Q�[���X�^�[�g���ɑ�ʂ̎����������Ă��܂��B��������b�I�Ȑ퓬�\�͂͂������ɂ������Ƃ͂������A�`���̏I�Ղł͋�킷�邱�Ƃł��傤�B"},
	{TRUE,"���E�D�܉؉@�E�C�i�o",CLASS_UDONGE,RACE_GYOKUTO,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͌����痈���d���e�ł��B�̂�т艮�̑����ʓe�̒��ł͑�z�����퓬�Z���X�������Ă��܂����B�g���𑀂�Ƃ����ɂ߂ĉ��p���ɕx�񂾔\�͂������A�U���h�䊴�m�Ƒ��ʂȓ��Z���K�����܂��B�n��ł̕�炵���������ߋʓe�̔j�׍U���ϐ��������Ă��܂��B"},
	{TRUE,"���Ӂ@�i��",CLASS_EIRIN,RACE_HOURAI,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͂��Č��̌��҂ƌĂ΂ꂽ���M�ȑ��݂ł������A���͎�̋P��Ƌ��ɒn��l�Ƃ��ĕ�炵�Ă��܂��B���Ȃ��͓V�˓I�ȓ��]�������A���x��������ĂĖ��@���������Ă���ΑS�Ă̖��@���s�g�\�ł��B����ɂ����钲�܂̃��V�s���L�����Ă���A�|�̈��������ӂŁA����������Ȃ��A���̏��؂̓ŕ����󂯕t���܂���B���Ȃ��͋P��̌������ɂ�苤�ɓS���ɕ����V�R�̗��ɏo�����܂����B�P��̔\�͂̓t���A���ړ����邽�тɂ��Ȃ��̃��x���ɍ��킹�ĕϓ����܂��B������Ȃ���`�����������܂��傤�B���Ȃ��ƋP��̂ǂ��炩�Е��ł��|���ƃQ�[���I�[�o�[�ł��B"},
	{TRUE,"�H���R�@�P��",CLASS_KAGUYA,RACE_HOURAI,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͂������Ɍ���邩����P���̐l�ł��B���z���ŉi�����̎�Ƃ��Ē����ԂЂ�����ƕ�炵�Ă��܂������A�ŋ߂��܂�ɉɂȂ��߁A�����̂ɒT�����߂��܂̐_������x�͎����ŒT���o�����ƈ�O���N���܂����B�w�����S�X�x�w�����̃��j�R�[���x�w�����C�̎w�ցx�w�I���K�x�w�A�U�g�[�g�x�Ƃ������G��������ۗL���Ă���悤�ł��B�撣���ē|���Ď�ɓ���܂��傤�B���������Z�[�u�f�[�^�̈ȑO�̖`���҂������̐_������Ȃ��ɓ͂��Ă����ꍇ�A���Ȃ��͂�����Q�[���J�n�����珊�L���Ă��܂��B�������J�n���Ɉ���L���Ă��邲�ƂɃX�R�A�������ɂȂ�܂��B���Ȃ��͕s���g�̖H���l�ł����A��x�|�ꂽ��`���ɖO���Ă��܂����߃Q�[���I�[�o�[�ł��B"},
	{TRUE,"�����@���g",CLASS_MOKOU,RACE_HOURAI,ENTRY_EIYA,SEX_FEMALE,
	"���Ȃ��͐�N�ȏ�O�ɖH���̖������ŕs�V�s���ƂȂ����l�Ԃł��BHP���s���Ă����̏�ŕ�������Ƃ����|�j��̔\�͂������Ă��܂��B��������Ƃ��ɂ�MP������AHP��MP���ǂ���������Ȃ�ƃQ�[���I�[�o�[�ł��B�퓬�ɂ����đ����ȏ�ɖ����������܂����AMP�̎c�ʂɂ͋C��z��܂��傤�B�s�V�s���ł��邱�Ƃ������΂��Ȃ��̐g�̔\�͂͂���قǍ�������܂��񂪁A���S�N�ɂ킽��킢�̌o����ς�ł��邽�ߗl�X�ȋZ�\�ɗD��܂��B���@����̈�K���\�ł����A���Ȃ��͈̑̂�؂̕ϗe���󂯕t���Ȃ����ߕϐg���@�̎g�p�͂ł��܂���B���Ȃ��͐琔�S�N�̐��ɂ��O���Ă���A���x���A�b�v�ɂ͑����̌o���l���K�v�ł��B�܂������̂��тɃX�R�A���啝�Ɍ������܂��B"},

	{TRUE,"�����@�H��",CLASS_YUUKA,RACE_DAIYOUKAI,ENTRY_KAEI,SEX_FEMALE,
	"���Ȃ��͋��͂ȗd���ł��B�Ԃ��炩����\�͂������܂����퓬�Ŕ\�͂��g�p���邱�Ƃ͂قƂ�ǂȂ��A�����ɍ����d�͂Ɛg�̔\�͂œG�����|���܂��B����ɍŏ����狭�͂ȕ���������Ă��邽�߁A�킢�Ō�����邱�Ƃ͂قƂ�ǂȂ��ł��傤�B�����������͋ɂ߂Ēx���A�X�R�A���啝�ɉ�����܂��B"},
	{TRUE,"���f�B�X���E�������R���[",CLASS_MEDICINE,RACE_ANDROID,ENTRY_KAEI,SEX_FEMALE,
	"���Ȃ��͖����̋u�Ɏ̂Ă��Ă����l�`�ł��B�܂�����𓾂Ă��炠�܂�o���Ă��炸�A�F�X�ȏ��ɍs���Č������L�߂悤�Ƃ��Ă��܂��B���Ȃ��͗l�X�ȓł𑀂�͂������Ă���A�i�����ɂ͑S�Ă̍U�������͂ȓő����������܂��B���Ȃ��̑̂͗闖�̓łŖ�������Ă���A�ł��z�����đ̗͂Ɩ����x���񕜂����邱�Ƃ��ł��܂����A�ʏ�̗d���l�`�ƈقȂ薂�����H���ɂ��邱�Ƃ��ł��܂���B���Ȃ��̏����Ȑg�̔\�͕͂��}�ł��B���ʂ̐����ɑ΂��Ă͗L���ɐ킦�܂����ł̌����Ȃ��G�ɂ͋�킷�邱�Ƃł��傤�B�o���l�𓾂邱�ƂłȂ����͂ȑ��������邱�Ƃɂ���ă��x���A�b�v���܂��B"},
	{TRUE,"����ˁ@����",CLASS_KOMACHI,RACE_DEATH,ENTRY_KAEI,SEX_FEMALE,
	"���Ȃ��͎O�r�̐�őD�������ĕ�炵�Ă��鎀�_�ł��B���΂��Ύd�������o���ėl�X�Ȃ��ƂɎ��˂����݂܂��B�����𑀂�Ƃ����֗��Ȕ\�͂������A�e���|�[�g�n�̓��Z���[�����Ă��܂��B�H����g�������萶�҂��ꌂ�œ|���Ȃǎ��_�炵���Z�������Ă��܂��B��i�ł���l�G�f�P�ɂ͓����オ��܂���B"},
	{TRUE,"�l�G�f�P�E���}�U�i�h�D",CLASS_EIKI,RACE_DEITY,ENTRY_KAEI,SEX_FEMALE,
	"���Ȃ��͌��z���S����腖��l�ł��B腖��ɂȂ�O�͂��n���l�ł����B�����͂��������Ƃ����\�͂������A���̔\�͂ɂ��Q�[�����ɂ����đS�ẴL�����N�^�[�̍s�����ɗ������ւ��Ȃ��Ȃ�܂��B�܂����Ȃ��͐l���ق����Ƃɓ��������Ǎ����َ��Ȑ��_�������Ă���A�����鐸�_�U���𖳌������܂��B���Ȃ��̕���́u����̖_�v�ƌĂ΂��ł��B�G�̃��x���������قǈЗ͂��オ��A���l����Ƃ����̈ꌂ���瓦��邱�Ƃ͂ł��܂���B���ɂ��l�X�ȋ��͂Ȕ\�͂�����������܂����A�����͒x���X�R�A���啝�ɉ�����܂��B"},

	{TRUE,"�H�@�×t",CLASS_SHIZUHA,RACE_STRAYGOD,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͍g�t�Ɨ��t�̐_�l�ł��B���������Ȃ���M�S�ɐM����҂͂قƂ�ǂ��܂���B�~�������ŁA��C�U�����󂯂�Ə����_���[�W���傫���Ȃ�܂��B�ؚ��ȊO���̊��ɂ��Ȃ��̐g�̔\�͍͂����A��Ɋi���Ő킢�܂��B�������ϋv�͂͂��Ȃ��߂ŁA���M����قǂ̓��Z�������܂���B���Ȃ��̑����I�Ȑ퓬�\�͂͂��̕ӂ̗d���ȉ��ł����A�`���𑱂��l�X�Ȍo����ς�ł���Ƃ�����͐V���ȐM���l�����đ啝�Ƀp���[�A�b�v���邩������܂���B"},
	{TRUE,"�H�@���q",CLASS_MINORIKO,RACE_STRAYGOD,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͎���̏H���i��_�l�ł��B�l�X�ɍD�����_�l�ł����A�L���_�ɂ͑��ɗL���Ȑ_�l�������̂ł��Ȃ����g�͂��܂�M�𓾂��Ă��܂���B�����I�ȋC���Ȃ��_�l�ōr���͋��ł��B����̈������s����ł������n���ے�����劙�̈����͂���Ȃ�ɍI�݂ł��B���R���@���K���ł��܂����A���R���@�͂��U���͂Ɍ�����̂ł�����x�̓��e����o�傷��K�v������܂��B�܂����Ȃ��͓~�����Ȃ��ߗ�C�U���ɂ���Ă��傫�ȃ_���[�W���󂯂܂��B�����I�Ȑ퓬�\�͂͒�߂ł����A�`����ʂ��ĐV���ȐM�𓾂�ƑS���ʂ̐_�i�ɕω����邩������܂���B"},
	{TRUE,"���R�@��",CLASS_HINA,RACE_DEITY,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͖�_�ł��B�_�ƌĂ΂�Ă͂��܂������Ԃ͗d���ɋ߂��悤�ŁA�j�׍U������_�ł��B����z�����Ă��̐g�ɗ��ߍ��ޗ͂������Ă��܂��B��͎��ꂽ�A�C�e������z���������G����׈��ȍU�����󂯂邱�Ƃŗ��܂�A���Z�ŏ������G�ɉ����t���邱�ƂŌ���܂��B���Ȃ��͖���܂�Η��܂�قǋ����Ȃ�܂����A���܂�߂���Ƌɂ߂Ċ댯�Ȏ��ɂȂ�̂Ŗ�̗ʂɂ͏�ɋC��z���ĉ������B���Ȃ��͐l����������Ă���X�ł̔����ɋ�J���܂����A�l���̐��l�`�̔����Ŋ􋖂��̎����𓾂邱�Ƃ��ł��܂��B�܂����Ȃ��̓}�C�i�X���ʂ��������i��g�ɂ��Đ킢�����邱�Ƃł��̑����i�̃}�C�i�X���ʂ�ł��������蔽�]�����邱�Ƃ��ł��܂��B����������͋ɂ߂č���ł��B"},
	{TRUE,"�͏�@�ɂƂ�",CLASS_NITORI,RACE_KAPPA,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͉͓��̃G���W�j�A�ŁA�ʏ�̃G���W�j�A�Ɠ��l�ɑ�����g���ċ@�B�̊J�����\�ł��B�J�������@�B���i�[���郊���b�N�̗e�ʂ͏��Ȃ߂ł����A���̑���ɓ��Z�Ƃ��ēƎ��̋@�B���g�p�\�ɂȂ�܂��B�Ǝ��̋@�B�͊J�������Ȃ��Ă����背�x���Ŏg�p�\�ɂȂ�A�G�l���M�[�͐��n�`��ŏ[�U�\�ł��B�ʏ�̃G���W�j�A�̋@�B�����U���͂Ɍ����܂����R�X�g�̖ʂł͗L���ł��B�܂������b�N�̒��ɂ͗l�X�ȋߐڐ퓬�p�M�~�b�N���d���܂�Ă���ʏ�̃G���W�j�A�ɔ�׊i���\�͂����߂ł��B���𑀂�\�͂������Ă��肢�����̐��U���̓��Z���K�����܂��B"},
	{TRUE,"�����@��",CLASS_MOMIZI,RACE_HAKUROU_TENGU,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͗d���̎R�ŕ�炷�����[�̏����V��ł��B�痢��̔\�͂������A���m��T���Ɋւ��Ă͔�є����ėD��Ă��܂��B�������Ȃ��Ă��Â��_���W���������ʂ��A���x�����㏸����ƖӖڂɑ΂���ϐ����l�����A����Ɏ��ӂ̂�����G���펞���m����悤�ɂȂ�܂��B�������퓬�\�͂͂������}�ł��B�Ȃ��A���Ȃ��͓��݈ȊO�̓��h��𑕔��ł��܂���B"},
	{TRUE,"�˖��ہ@��",CLASS_AYA,RACE_KARASU_TENGU,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͌��z���̏��������̐V�����������Ƃɏ�M��R�₷��V��ł��B�����ڂɔ�������V��̒��ł����Ȃ�Ê��ł��B�S�L�����N�^�[�ō��̉����ƕq�����������A�L��}�b�v��ł͈ړ����x������ɑ���������؂̏P�����󂯂܂���B�ڋߐ�ɋ����A�_�邩���R�̖��@����̈�K���\�ŁA����ɕ֗��ȓ��Z�𑽂������\�^�ł��B�����������͂��Ȃ�x�߂ŃX�R�A���啝�ɒቺ���܂��B"},
	{TRUE,"�����J�@���c",CLASS_SANAE,RACE_DEMIGOD,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͕��̐_�Ɏd����_�E�ŁA���j(�����͂ӂ�/�����ق���)�ƌĂ΂�܂��B�M����������O�̐��E����_�l�Ƌ��Ɍ��z���ւƈڂ�Z��ł��܂����B���Ȃ����g���_�̌��������Ă���A��X�`����p�ŋ��͂Ȋ�Ղ��N�����܂��B���������̂��߂ɂ͐��^�[������ꍇ�ɂ���Ă͐��\�^�[���̒����r�����K�v�ł��B�G�̓������ǂ݂��ĉr�����Ȃ���҂�������Ȃǐ헪�I�ȗ�����肪�v������邱�Ƃł��傤�B���Ȃ��̐g�̔\�͕͂��ʂ̐l�Ԃƕς�炸�A���e��͂���قǓ��ӂł͂���܂���B"},
	{TRUE,"����@�_�ގq",CLASS_KANAKO,RACE_DEITY,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͐M�̎�������O�̐��E����_�Ђ��ƌ��z���ֈڏZ���Ă����_�l�ŁA�V�V�n�ŐV���ȐM�𓾂悤�Ɛ��͓I�Ɋ������Ă��܂��B���Ȃ��̏��Ղ̐퓬�\�͕͂��}�ł����A���̂���G��ł��|�����тɐM���W�܂��Ă��Ȃ��͂ǂ�ǂ񋭂��Ȃ�܂��B���ɓ���̏@���̑�\�I�ȑ��݂�|���Ƒ啝�ɐM���W�܂�A�V���ȑϐ��𓾂邱�Ƃ�����܂��B�ŏI�I�ɂ͑�ςȋ����ɂȂ�܂����A�����͒x���X�R�A���啝�ɒቺ���܂��B"},
	{TRUE,"�k��@�z�K�q",CLASS_SUWAKO,RACE_DEITY,ENTRY_HUUZIN,SEX_FEMALE,
	"���Ȃ��͂��ēy���̐_�X�𑩂˂鋭��Ȑ_�i�ł������A���݂͐_�Ђ̌o�c��_�ގq�ɔC���ċC�y�Ȑ_�l�����𑗂��Ă��܂��B�z�����M��_�̗͂��؂�đ�n�𑀂���Z�������A�d�p��S�̈�s�g�ł��A����ɔ����������Ȃ�ɂ��Ȃ��܂��B���̗͐̂͂������Ă���MP���Ⴍ���@����Z�̎g�p�ɋ�J���܂����A�l�Ԍn�̓G��|�������ĊF���狰������MP�ő�l���㏸���܂��B�����I�ɂ͋ɂ߂ċ��͂ł��������͂��Ȃ�x���X�R�A���啝�ɒቺ���܂��B"},

	{TRUE,"�L�X��",CLASS_KISUME,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͒ޕr���Ƃ��̗d���ŁA�N���̓���ɗ������邱�Ƃ����Ȃ��̐����b��ł��B�������̒��ɓ����Ă���A���ɂ͉��ȊO�̕��𑕔��ł����A���̂������������s�\�ł��B���Ȃ��̊i���U���͏�󂩂�̗����ł��B�U���񐔂͏��1�ł����З͍͂����A�����Ă��鉱����v�Ȃ��̂��Ƃ���ɈЗ͂����ˏオ��܂��B������яオ�邱�Ƃɂ�藎���U���̈З͂�啝�ɑ�����������Z�������܂��B"},
	{TRUE,"���J�@���}��",CLASS_YAMAME,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͓y�w偂̗d���ł��B�a�C�𑀂�͂������A�őϐ��������Ȃ��G�ɍU������Ƒ傫�ȃ_���[�W��^���邱�Ƃ��ł��܂��B�܂������g���ēG�̓�����W�Q�����莩���������ňړ����邱�Ƃ��ł��܂��B���Ȃ��͎͗����Ō��@�肪���ӂł����A����̈����Ȃǂ͂��܂蓾�ӂł͂���܂���B"},
	{TRUE,"�����@�p���X�B",CLASS_PARSEE,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͎��i�[�����P�ł��B���i�������肳�ꂽ�肷�邱�Ƃ����Ȃ��̗͂ɂȂ�܂��B�G�̃��x����������荂���ƍU���͂��㏸���A���ɂ��̓G���������ƍU���͂�����ɒ��ˏオ��܂��B�܂��̗͂������Ă���قǍU���͂⑬�x���㏸���܂��B�ق��ɂ����������Z�������܂����A�قƂ�ǂ������̗̑͂������Ă�����G�������قǋ��͂ɂȂ�悤�ȃn�C���X�N�Ȃ��̂ł��B���Ȃ��̊�b�I�Ȑ퓬�\�͂͒Ⴂ���ߋ��G�𑊎�ɂ���Ƃ��͊댯��`���K�v������ł��傤�B"},
	{TRUE,"���F�@�E�V",CLASS_YUGI,RACE_ONI,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͋S�̎l�V���̈�l�ɐ������鋭��ȋS�ł��B���̔�ނȂ����r�ŕs�𗝂Ȃ܂ł̔j��������N�����܂��B���Ȃ��͍ō��̘r�͂������܂��������i���Ȃǂ̋Z�\�̒b�B�ɂ͂��܂苻�����Ȃ��A�����ȗ͂œG���˂������邱�Ƃ��D�݂܂��B���Ȃ��͏�ɕЎ�ɔu�������Ă��āA�u�̕s�v�c�ȗ͂ł���������������ނ��Ƃ��ł��܂��B"},
	{TRUE,"�Ö��n�@���Ƃ�",CLASS_SATORI,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͒n��a�̎�̊o(���Ƃ�)�d���ł��B�����҂��W�܂�n��ɂ����Ă���ی����Ă���A���i�͊O�ɏo�邱�Ƃ���Ȃ��y�b�g�B�Ɉ͂܂�ĕ�炵�Ă��܂��B���Ȃ��͋��̑�O�̖ڂő��҂̐S��ǂ݁A���҂̃g���E�}���h�����Đ��_���U�����A����ɂ͓ǂ񂾐S�ۂ������ɍČ�������Z�������܂��B�����������̓��Z�͐��_�������Ȃ�����ɑ΂��Ă͑S�����͂ł��B���Ȃ��̐g�̔\�͂͗d���Ƃ��Ă͂��Ȃ�Ⴂ�̂Ő키����Ɛ헪�͐T�d�ɑI�т܂��傤�B���x�����オ��ƕǂ̌������̓G�����m���A���Ɏc������v�O��ǂݎ��A����ɍ����E���|�E���C�ɑ΂���ϐ��𓾂܂��B"},
	{TRUE,"�Ή��L�@��",CLASS_ORIN,RACE_WARBEAST,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͉ΎԂƌĂ΂�鉻�L�ł��B���i�͒n��a�̋��ܔM�n���œ����Ă��܂��B�����ɐN�����Ď��̂���������̂���D���ŁA�l���ł͂��Ȃ茙���Ă��܂��B�������̂����邽�߂̔L�Ԃ������Ă���A���̒��ɂ̓A�C�e�������[�ł��܂��B���[�����A�C�e���͓G�̍U���Ŕj�󂳂�܂��񂪈�x�o���Ȃ��Ǝg���܂���B�B����T���ɗD��A���@���K�����A����ɔL�ԂœG��瀂��čU�����邱�Ƃ��ł��܂��B�Ή��ƒn���̍U���ɑϐ��������A��������ƉΉ��̓�d�ϐ��Ƌ��C�ϐ����l�����܂��B�������j�׍U���ɑ΂��Ă͋ɂ߂ĐƎ�ł��B"},
	{TRUE,"��G�H�@��",CLASS_UTSUHO,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͒n��a�Ńy�b�g�Ƃ��ĕ�炷���}�Ȓn���G�ł������A��������@�G�����̐g�Ɏ�荞��Ŕ���I�Ƀp���[�A�b�v���A���낤���Ƃ��j�Z���𑀂�͂���ɂ��܂����B���₠�Ȃ��͖̑̂��s���̃G�l���M�[�̉�ł��B���͂ȉ��u�U������������ł����A�قƂ�ǂ̍U�����Ή���M�������̂��ߑϐ������G�����肾�Ƃ��肱���邩������܂���B���Ȃ��̖������㩉����Ȃǂ̋Z�\�͉����ȂׂĒႭ�A�Ԃ������ďĂ������ȊO�̂��Ƃ͂قƂ�ǂł��܂���B�܂��j�Z���̐���̂��߂ɂ͕Иr���J���Ă����Ȃ���΂Ȃ炸�A�O���[�u�ƃu�[�c�͂��ꂼ��Е����������ł��Ȃ����ߐ��\���ቺ���܂��B"},
	{TRUE,"�Ö��n�@������",CLASS_KOISHI,RACE_YOUKAI,ENTRY_TIREI,SEX_FEMALE,
	"���Ȃ��͓��ƐS��������o�̗d���ł��B�قƂ�ǖ��ĂȈӎ����������Ɍ��z���̂���������p�j���Ă��܂��B�ǐS�̓����������ɖ��ӎ��𑀂�\�͂𓾂Ă���A�G�͂��΂��Ζڂ̑O�ɂ��邠�Ȃ����������܂��B�G�̕s�ӂ����ċ���Ȉꌂ�����������Ƃ��e�Ղł��傤�B�������ʏ�̐��_�������Ȃ�����ɑ΂��Ă͂��̔\�͂͌��ʂ��ቺ���܂��B���Ȃ��̐��_�͂����Ȃ���̂�����e�����󂯂܂���B"},

	{TRUE,"�i�Y�[����",CLASS_NAZRIN,RACE_WARBEAST,ENTRY_SEIREN,SEX_FEMALE,
	"���Ȃ��͔�����V�̎g���̗d���l�ŁA���݂͖��@���ɋ��͂������˂̋߂��ŕ�炵�Ă��܂��B�_�E�W���O�̖���ŕ�T���͂���̕��ł��B����ɑl������ꌂ���E�Ȃǂ̏��Z�������A�B���s���▂����g�p�����ӂł��B�܂����Ȃ��͔�����V�̎g���ł��邽�ߗd���ł���ɂ�������炸�j�׍U������_�Ƃ��܂���B���������Ȃ��̑̊i�͂��Ȃ�n��ŁA�퓬�\�͂͂������ɂ������Ƃ͌����܂���B���Ȃ�T�d�ȗ�����肪�v������܂�������ł��댯�͑傫���ł��傤�B"},
	{TRUE,"���X�ǁ@���P",CLASS_KOGASA,RACE_TSUKUMO,ENTRY_SEIREN,SEX_FEMALE,
	"���Ȃ��͓��P�������ƌĂ΂��P�̕t�r�_�ł��B��Ɏ������P�����Ȃ��̖{�̂ŁA��������Ƃ͂ł��܂���B���Ȃ��͂��̎P��U��񂵂Đ키���ƂɂȂ�܂��B�P�Ȃ̂Ő��ɂ͋����̂ł����_�ƉΉ��͋��ł��B�t�r�_�̎푰���������킹��ƁA�����ł͎_�ƉΉ��Ɣj�ׂƗ򉻂���_�Ƃ������ƂɂȂ�܂��B���x�����オ��ƎP���p���[�A�b�v���A�������͉��P����邩������܂���B�ʏ�̕t�r�_�ƈႢ������ŐH����ۂ邱�Ƃ��ł����A�N�����������邱�ƂŊ��͂𓾂܂��B�܂����Ȃ��͒b�肪���ӂŁA����̖ڗ�����������������̕�����������邱�Ƃ��ł��܂��B"},
	{TRUE,"�_���@���",CLASS_ICHIRIN,RACE_YOUKAI,ENTRY_SEIREN,SEX_FEMALE,
	"���Ȃ��͓����g���Ƃ����������d���ŁA�_�̂悤�Ȏp�́u�_�R�v�ƌĂ΂������Ƃ����ꏏ�ɂ��܂��B�i���U��������Ƃ��A���Ȃ��̑���ɉ_�R���U�����s���܂��B�_�R�̋���Ȍ��ɂ͂��Ȃ�̈З͂����҂ł��A�U���̈З͂͂قƂ�ǂ��Ȃ��̘r�͂Ɋ֌W�Ȃ����܂�܂��B�������d������ƍU���ɍ�����肪�o��̂͒ʏ�̊i���Ɠ����ł��B�_�R�͗��ꂽ�ꏊ�ɍU�������背�[�U�[����ȂǕ֗��ȓ��Z�������A����Ƀs���`�̂Ƃ��ɂ͂��΂��΂��Ȃ�������Ă���܂��B���������Ȃ��͉_�R�Ƃ̘A�g�̂��ߏ�ɗ�����J���Ă����K�v������A�肪�ǂ���ƍU���͂��啝�ɒቺ���܂��B"},
	{TRUE,"���с@����",CLASS_MURASA,RACE_YOUKAI,ENTRY_SEIREN,SEX_FEMALE,
	"���Ȃ��͏M�H��ŁA��͐l��M�ꂳ���邱�Ƃł��B���̂��鏊�ǂ��ɂł��o�����܂��B�H��Ƃ͂����Ă����̂����O����ŁA��r�I�d���ɋ߂��̂����Ă��܂��B���Ɋւ��鑽�ʂȓ��Z�������A�G�̐��U�������S�ɖ��������邱�Ƃ��ł��܂��B�܂����̒��ł͍����ňړ��ł��A�p�����[�^����������A����ɑf��U���ɂ��G���ꌂ�œ|�����Ƃ�����܂��B�������������������肾�ƂقƂ�ǂ̍U����i�͖��͂ł��B�H��̈��Ȃ̂Ŕj�׍U���͑�̋��ł��B"},
	{TRUE,"�Њہ@��",CLASS_SHOU, RACE_WARBEAST ,ENTRY_SEIREN,SEX_FEMALE,
	"���Ȃ��͐l�X�̋�z���琶�܂ꂽ�d�b�ł������A�F�X�����č��͖��@���Ŕ�����V�̑㗝�𖱂߂Ă��܂��B�����_�i�����Ă��邽�ߗd���ł���ɂ�������炸�j�׍U���ɑϐ��������܂��B�g�̔\�͂������A�b�̉s�q�Ȋ��o�������A������Ȃǂ̋Z�\�����������D�G�ł��B���󂪏W�܂�\�͂����肠�܂莑���ɋ�J���܂���B����ɍŏ����������V�̕󓃂��������Ă��肱����g���ċ��͂ȉ��u�U�����\�ł��B�S�̓I�Ɍ��̂Ȃ��\�͂̍����ł����A�E�܂��������ڂɔ����ĕ��p�̐S���͑S������܂���B�撣���Ēb���܂��傤�B�󓃂�������Ƒ����̓��Z���g���Ȃ��Ȃ邽�߁A�������薳�����Ȃ��悤�ɋC�����ĉ������B"},
	{TRUE,"���@���@",CLASS_BYAKUREN,RACE_MAGICIAN,ENTRY_SEIREN,SEX_FEMALE,
	"���Ȃ��͐�N�ɂ킽�镕�󂩂���������ꂽ���͂Ȗ��@�g���ł��B������V�ɋA�˂��铿�̍����m���ł�����A���݂͖��@���̎����I�Ȏw���҂Ƃ��ďC�s�ƕz�������Ȃ����炵�Ă��܂��B���Ȃ��̓��Z�͋��͂Ȑg�̋������@�ɂ����e��ŁA�����ǂ���G�����X�Ɖ���|�����Ƃ��ł���ł��傤�B�����������̖��@���؂��Ƒł���ア�̂Ŗ��f�͋֕��ł��B�����I�ɋɂ߂ċ��͂ł����A�X�R�A�͑啝�ɒቺ���܂��B"},
	{TRUE,"���b�@�ʂ�",CLASS_NUE,RACE_DAIYOUKAI,ENTRY_SEIREN,SEX_FEMALE,
	"���Ȃ��͂��ĕ����������|�Ɋׂꂽ�Ɖ\������d���ł��B�����ɑ΂���F����f�킹��\�͂������A�G�̐��_�֍U�����d�|����̂����ӂł��B�������Ȃ��Ă��_���W���������ʂ����Ƃ��ł��A�Â����ł͓G���甭������Â炭�Ȃ�܂��B�܂����Ȃ��͎����̐��̂��B���ĉ����ɕϐg���Ă��܂��B�ϐg�͋��͂ł����G�ɐ��̂����j���Ă��܂��Ƃ��̓G���t���A���炢�Ȃ��Ȃ�܂ŕϐg����������܂��B���邢�ꏊ�ɂ����茫���G�ł������肠�Ȃ��̋��ȑ����̍U�������ꂽ�Ƃ��͐��̂����j���₷���悤�ł��B�����I�ɋ����Ԃ񐬒��͒x���X�R�A�����Ȃ��߂ł��B"},

	{TRUE,"�H�J�@���q",CLASS_KYOUKO,RACE_YOUKAI,ENTRY_SHINREI,SEX_FEMALE,
	"���Ȃ��͗H�J���i���}�r�R�j�̗d���ŁA���̂悤�Ȏ��ƐK���������Ă��܂��B���������疽�@���Ő^�ʖڂɏC�s�ɗ�ވ���ŁA��̓p���N���b�N�ɌX�|����ӊO�Ȉ�ʂ������܂��B�吺�ƃI�E���Ԃ������Z�ŁA�����œG��N�O�Ƃ�������G�̏��������@��^�����邱�Ƃ��ł��܂��B���x�����オ��ƍ����U�������S�ɖ��������A�G�̃{���g�n���@�𔽎˂��邱�Ƃ��ł��܂��B���������Ȃ��̐g�̔\�͂͂���قǍ����Ȃ��A��{�I�ɍr���͋��ł��B�吺�Ő키���߉B���s������̋��ł��B"},
	{TRUE,"�{�Á@�F��",CLASS_YOSHIKA,RACE_ZOMBIE,ENTRY_SHINREI,SEX_FEMALE,
	"���Ȃ��͎א�跐M�̈��p�̃L�����V�[�ł��B�����̃P�A�͖��S�ł����]�͊��S�ɕ����Ă��܂��B��ɓŒ܂̂Ђ������U���Ő킢�܂��B��Ȃ��̗̑͂������A�̗͂̎��R�񕜂��ɂ߂đ����A����Ɏ��͂̏��_���H�ׂđ̗͂��񕜂��邱�Ƃ��ł��܂��B�����������▂����ȂǓ�������g����Ƃ͂����Ȃׂċ��ł��B���Ȃ�̐����ŕ�������̂ŐH���͑��߂Ɏ����܂��傤�B"},
	{TRUE,"跁@�M",CLASS_SEIGA,RACE_SENNIN,ENTRY_SHINREI,SEX_FEMALE,
	"���Ȃ��͓����O�ꂽ��l�ł��B��{�I�Ɏ����̗~�]�ƍD��S�𖞂����������l���Ă��܂���B������w�ŕǂɌ����J������Z�������܂��B���x�����オ��ƈړ����ɕǂ��@�蔲���A����Ƀ��x�����オ��ƕǂ��@�炸�ɂ��蔲����悤�ɂȂ�܂��B���@���̈�K���\�ŁA����p�Ȃǂ̉ЁX�����p�ɓ��ɍ����K���������܂��B�������𐶂�����l�̂��߁A���΂��Βn������̎h�q�ɑ_���܂��B"},
	{TRUE,"�h��@�j����",CLASS_TOZIKO,RACE_SPECTRE,ENTRY_SHINREI,SEX_FEMALE,
	"���Ȃ��͖L�����_�q�ɏ]�҂Ƃ��Ďd����S��ł��B���Ă͋��͂ȉ���ł������A���͍��݂��قƂ�Ǐ����C�܂܂ȖS�쐶���𑗂��Ă��܂��B��̂̂��ߕǂ𔲂��邱�Ƃ��ł��_���W�����T���ɗL���ł��B�d���U�������ӂŁA����̈����͋|�������s����ł��B�����g�̋�����s���S�Ȃ��ߌC�𗚂����Ƃ��ł��܂���B"},
	{TRUE,"�����@�z�s",CLASS_FUTO,RACE_SENNIN,ENTRY_SHINREI,SEX_FEMALE,
	"���Ȃ��͐�l�S�N�̖��肩��ڊo�߂������ŁA�L�����_�q�̌��œ��m�Ƃ��ďC�s�̓��X�𑗂��Ă��܂��B��l�̊��ɖ��@���̈����͂��܂蓾�ӂłȂ��A����ɕ����̔�p�Ɠ����̐�p�����킹���Ɠ��̋Z�Ő킢�܂��B���̋Z�͂��Ȃ�����܂������O�ɏ�������ΈЗ͂͐��ł��B���܂藐������ƎM�オ���ނ��ߏ��Ղ͂��܂�����J�肵�܂��傤�B���Ȃ��̐g�̔\�͂͂���قǍ����Ƃ͂������A�|�ȊO�̕���̈����͋��ł��B�܂����Ȃ��͔�p�ɂ��{���̎������떂�����Ă��邽�߁A�_���W�������Œn������̎h�q�ɏP���邱�Ƃ�����܂��B"},
	{TRUE,"�L�����@�_�q",CLASS_MIKO,RACE_SENNIN,ENTRY_SHINREI,SEX_FEMALE,
	"���Ȃ��͐�l�S�N�̎����o�ĕ����𐋂��_�i���������l�ł��B�u�l�Ԃ𒴂���ׂ����Ē������v�ƌ�����قǂ̈��|�I�ȍ˔\�������܂��B��p�̎l�̈�̖��@��S�ďK�����Ă���A���x��������Ă��Ė��@���������Ă���Ίw�K�����Ȃ��Ă����@���g���܂��B����ɂ�����Z�\�ɗD��A���͂ȓ��Z���g���A�D�G�ȑϐ��������A��������������ɂ��Ȃ��܂��B���S�����ɋ߂������ł����A�X�R�A�͋ɂ߂ĒႭ�Ȃ�܂��B"},
	{TRUE,"��b��@�}�~�]�E",CLASS_MAMIZOU,RACE_BAKEDANUKI,ENTRY_SHINREI,SEX_FEMALE,
	"���Ȃ��͉����K�̐e���ŁA�d���̐g�ł���Ȃ�����ŋ߂܂ŊO�E�ɏZ��ł��܂����B���z���ł����X�ɎЉ�ɗn�����݁A�l�X�Ȋ�݂��ƂɈꖇ����ł��܂��B���Ȃ��͉�����(�΂�������)�������ŁA�G�⎩�����g���D������ɕϐg�����ēG��|�M���܂��B�܂��A�K�̔z�����Ăяo���ēG���U�����邱�Ƃ��ł��܂��B�ϐg�n�̓��Z�͒m�\�ƌ����A�z�����g�����Z�͖��͂ɂ���Č��ʂ��傫���ς��܂��B"},

	{TRUE,"�킩�����P",CLASS_WAKASAGI,RACE_NINGYO,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͖��̌΂ŉ̂��̂�����΂��E���ĕ�炷�l���ł��B������l�������i�Ő퓬�\�͂͂��Ȃ��߂ł�������ɂ���Ɨ͂������܂��B�������̓��Z���������o���܂������@�͎g���܂���B���x�����オ��Ɛ���ł̔\�͏㏸��������ɏオ��A���U���ɑ΂��銮�S�ȖƉu�𓾂܂��B�Ȃ��A���Ȃ��͐l���Ȃ̂ŌC�𗚂����Ƃ��ł��܂���B"},
	{TRUE,"�Ԕ؊�",CLASS_BANKI,RACE_YOUKAI,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͂낭���̗d���ŁA���̂��B���Đl���ŕ�炵�Ă��܂��B�񂪐L�т邾���ł͂Ȃ����𓷑̂��痣��Ĕ�΂����蓪�𕪐g��������ڂ��烌�[�U�[���������Ǝ��ɑ��|�ł��B���̕��g�̐��Ɛ퓬�͂̓��x���A�b�v�ŏオ��A���̕��g���������͖̂{�̂��F�����邱�Ƃ��ł��܂��B�{�̂̐퓬�\�͂͒Ⴍ�͂���܂��񂪗d���Ƃ��Ă͂������}�ł��B�J�����ꏊ�ŕ��g�ɉ��삳�����ڍU������X�^�C������{�ɂȂ�܂��B�R��ɓK���������܂�������ȊO�̋Z�\�͒�߂ł��B���Ȃ��͎����𑕔��ł��܂���B"},
	{TRUE,"����@�e�T",CLASS_KAGEROU,RACE_WARBEAST,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͒������j�z���I�I�J�~���ŁA���i�͖����̒|�тɏZ��ł��܂��B��r�I�����g�̔\�͂������܂�������̈����͋��ŁA��{�I�Ɋi���Ő킢�܂��B�܂��g�̂��Ȃ��͏r�q�Ȃ��̂̎��͂��܂��p�Ȃق��ł͂���܂���B�ϗe�E�Í������ꂩ�̗̈�̖��@���K���ł��A������@�͎g���܂��񂪈ꕔ�̕ϐg���@�͑哾�ӂł��B���₩�ȃ����O�w�A�������ŁA�������މΉ��E�d���U���͌����ł��B�����̔ӂɂ͐g�̔\�͂�����ɏ㏸���A���͂ȘT�ɕϐg�\�ɂȂ�܂��B�ϐg���͑����i�̈ꕔ�������������̂őϐ��ɂ͋C��t���Ă��������B"},
	{TRUE,"��\��@�فX",CLASS_BENBEN,RACE_TSUKUMO,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͌Â����i�̕t�r�_�ł��B�����Ŏ������g�����t���邱�Ƃ��ł��A�l�X�Ȍ��ʂ������y��t�łĐ킢�܂��B���ɓG�𒼐ڍU������悤�ȉ��y���D�݁A������x�̋ߐڐ퓬�����Ȃ��܂��B�܂����@����̈�K�����邱�Ƃ��\�ł��B���Ȃ��͖��@���r�������������݂Ȃ���ł����t���ł��܂����A�אڍU���⑕���ύX�Ȃǒ����Ԏ���g�����Ƃ�����Ɖ��t���r�؂�܂��B�܂����t�̎ז��ɂȂ�O���[�u�ނ𑕔��ł��܂���B���Ȃ��̖{�͔̂��i�̂��ߒʏ�̐H���ł͕��𖞂������A���@�̓����[�U���͂��z�����ĕ��𖞂����܂��B�ł�ӖځA�����U���ɑϐ��������܂����j�ׂƗ򉻍U���͂��Ȃ��̑�G�ł��B"},
	{TRUE,"��\��@����",CLASS_YATSUHASHI,RACE_TSUKUMO,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͌Â��Ղ̕t�r�_�ł��B�����Ŏ������g�����t���邱�Ƃ��ł��A�l�X�Ȍ��ʂ������y��t�łĐ킢�܂��B���ɝ��ߎ�œG��f�킹��悤�ȉ��y���D�݁A���e��͋��ł������@���̈�K�����邱�Ƃ��\�ł��B���Ȃ��͖��@���r�������������݂Ȃ���ł����t���ł��܂����A�אڍU���⑕���ύX�Ȃǒ����Ԏ���g�����Ƃ�����Ɖ��t���r�؂�܂��B�܂����t�̎ז��ɂȂ�O���[�u�ނ𑕔��ł��܂���B���Ȃ��̖{�̂͋Ղ̂��ߒʏ�̐H���ł͕��𖞂������A���@�̓����[�U���͂��z�����ĕ��𖞂����܂��B�ł�ӖځA�����U���ɑϐ��������܂����j�ׂƗ򉻍U���͂��Ȃ��̑�G�ł��B"},
	{TRUE,"�S�l�@����",CLASS_SEIJA,RACE_YOUKAI,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͔��t�̍������V�׋S�ł��B����厖�����N�����Ă��q�ˎ҂ɂȂ�A���z���̏Z�l�S�Ă���ǂ��Ă��܂��B���Ȃ��̐g�̔\�͂͋ɂ߂ĕn��Ȃ��߁A�G�ɓ��e��𒧂ނ͖̂��d�ł��B�ǂ����ɉB�������Ă����ʂ̓������g���Đ킢�܂��B����ɂ́u������v�Ɓu�s�v�c����v������A�ǂ�����T�����_���h���𗘗p���邱�ƂŎg�p�񐔂��񕜂��܂��B������Ɍ����Ă̓_���W�������ŏ[�U���邱�Ƃ��ł��܂��B�p���퓬�\�͂Ɍ����邽�ߍ��̎莝������œ|����G���ÂɌ��ɂ߂�K�v������܂��B�܂����Ȃ��͋ɓx�̔P����҂̂��߁A�l�Ɋ��ӂ����Ɛ��_�ɑŌ����󂯂Ĕ\�͂��ቺ���܂��B"},
	{TRUE,"�����@�j����",CLASS_SHINMYOUMARU,RACE_GREATER_LILLIPUT,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͈ꐡ�@�t�̖���ł���A�`���̑ŏo�̏��Ƃ̐����Ȍp���҂ł��B�ŏo�̏��ƂɊ肢�������邱�Ƃŗl�X�ȗL���Ȃ��Ƃ��N�����܂����A���Ƃɉ������肤�Ǝ��ɉ������肦��悤�ɂȂ�܂ň�莞�Ԃ̏��Ƃ̏[�U���K�v�ł��B�܂����Ƃ̗͂��g���Ɨl�X�ȑ㏞���������邱�Ƃ�����A�傫�Ȋ肢�ł���قǋ���ȑ㏞�ɂȂ�܂��B���@�͎g���܂��񂪉B���▂����Ȃǂ̋Z�\�͍����o����ςނ̂����߂ł��B"},
	{TRUE,"�x��@����",CLASS_RAIKO,RACE_TSUKUMO,ENTRY_KISHIN,SEX_FEMALE,
	"���Ȃ��͑������Ƒ�_���𕹂������͂ȕt�r�_�ł��B��������������g�����ނ��Ƃŕ���̗͂������o���ăp���[�A�b�v�����A����ɕ���̔\�͂ɉ��������@��u���X�Ȃǂ𔭓����邱�Ƃ��ł���悤�ɂȂ�܂��B�������Œ�A�[�e�B�t�@�N�g�̕���͌����������邹�������̔\�͂̑ΏۂɂȂ�܂���B�܂����Ȃ��͊y��̕t�r�_�Ȃ̂ŁA��������t���ėl�X�Ȍ��ʂ������y��t�ł邱�Ƃ��ł��܂��B�肾���łȂ����ł����t�ł��邽�߁A�ʏ�U����A�C�e���̎g�p�Ȃǂŉ��y�����f����܂���B�����I�ɋɂ߂č����\�͂�������������r�I���߂ł����A�X�R�A�͑啝�ɉ�����܂��B"},

	{TRUE,"����",CLASS_SEIRAN,RACE_GYOKUTO,ENTRY_KANZYU,SEX_FEMALE,
		"���Ȃ��͌�����h������Ă����n�㒲�������u�C�[�O�������B�v�̈���ł��B�����{���̔C���ɂ������Ă���A�T���Əe�����ӂł��B���Ȃ��َ͈�������e�ۂ��΂����Ƃ��ł��A�e�Ɋւ�����Z���������K�����܂��B���Z�̎g�p�ɂ͎�ɒm�\����p��������܂��B�ڋߐ�\�́A�J���Ȃǂ̋Z�\�͂������}�ł��B���Ոȍ~��؂蔲����ɂ͋��͂ȏe����ɓ����K�v������ł��傤�B"},
	{TRUE,"���",CLASS_RINGO,RACE_GYOKUTO,ENTRY_KANZYU,SEX_FEMALE,
		"���Ȃ��͌�����h������Ă����n�㒲�������u�C�[�O�������B�v�̈���ł��B���Ǘ��̔C���ɂ������Ă���A�T���ƒ����\�͂ɗD��Ă��܂��B���Ȃ��̊�{�I�Ȑ퓬�\�͕͂��}�ł����A�c�q��H�ׂ�ΐH�ׂ�قǃp���[�A�b�v����Ƃ����s�v�c�Ȕ\�͂������Ă��܂��B�������Ƃ����Ƃ��ɂ͐ɂ��݂Ȃ��c�q��H�ׂĐ؂蔲���܂��傤�B�c�q��H�׉߂��Č������邱�Ƃ͂���܂���B�܂��_���W�����̒T�����ɂ͎��X���Ԃ��x�������Ƃ��Ēc�q��͂��Ă���܂��B"},
	{TRUE,"�h���~�[�E�X�C�[�g",CLASS_DOREMY,RACE_YOUKAI,ENTRY_KANZYU,SEX_FEMALE,
	"���Ȃ����т̗d���ŁA�l�̈�������D���ł��B�ʏ�̐H���ł͂قƂ�ǉh�{�𓾂�ꂸ�A�_���W������h���ȂǂŖ����Ă���҂̖���H�ׂĕ��𖞂����܂��B�G�𖰂点���萸�_���U��������Z���K�����A�܂����@����̈�K���\�ł��B���Ȃ��̐퓬�\�͍͂����Ƃ͌����܂��񂪁A���Ȃ��̖{�͖̂��̐��E�Ŕ�������܂��B�_���W�����u���̐��E�v�̒��ł͔\�͂��㏸���ē��Z���啝�ɋ�������A����ɓ���ȋZ�œG��|�����Ƃł��̓G�ɕϐg���邱�Ƃ��ł��܂��B"},
	{TRUE,"�H�_�@�T�O��",CLASS_SAGUME,RACE_LUNARIAN,ENTRY_KANZYU,SEX_FEMALE,
	"���Ȃ��͌��̓s�̏d���ŁA���z���ɃI�J���g��������������{�l�ł��B���Ȃ��������ɂ��Č��Ɩ]�ޖ]�܂���Ɋւ�炸���̎��Ԃ��t�]���܂��B������ǂނƏ�����Ă��邱�ƂƑS���Ⴄ���Ƃ��N����̂ŋC��t���Ă��������B���Ȃ��͈�؂��q������₷�錎�̖��Ȃ̂��q����܂ލU���͑�̋��ł��B"},
	{TRUE,"�N���E���s�[�X",CLASS_CLOWNPIECE,RACE_FAIRY,ENTRY_KANZYU,SEX_FEMALE,
	"���Ȃ��͒n���̏��_�Ɏd����d���ł������A��̗F�l�̗͂Łu�����v����Ċo�����A�����͂̉�ƂȂ�܂����B���̂��Ȃ��͊��S�ɗd���̘g����O�ꂽ����ȗ͂������Ă��܂����A���̗͂͂����܂ňꎞ�I�ɗ^����ꂽ���̂ł���A����ꂽ�̗͓͂�x�Ɖ񕜂��邱�Ƃ͂���܂���B���S�ɑ̗͂��Ȃ��Ȃ�ƋA���ĂӂĐQ���Ă��܂��Q�[���I�[�o�[�ł��B���Ȃ��͒n���̏������g���Ă���A������g�����ƂŎ����⒇�ԁA����ɂ͎��͂̓G�����킹���苭�����邱�Ƃ��ł��܂��B"},
	{TRUE,"����",CLASS_JUNKO,RACE_DEITY,ENTRY_KANZYU,SEX_FEMALE,
	"���Ȃ��͌Â����͂Ȑ��ł��B���������Ȃ��̊j���Ȃ��̂́A�����̂ɕ��Q�𐋂��č��Ȃ������Ȃ�����ȑ����ł��B���̏����̑����݂͌��̖��Ɍ������Ă���A�ނ����������ƒ@���ׂ����ɂ͂���܂���B���Ȃ��͖��������Ď����Ă����{���I�ȗ͂�ێ����ė��p����u�����v�Ƃ����\�͂������A���̔\�͂ɂ���ėl�X�Ȋ�z�V�O�Ȃ��Ƃ𐬂������邱�Ƃ��ł��܂��B"},
	{TRUE,"�w�J�[�e�B�A�E���s�X���Y��",CLASS_HECATIA,RACE_DEITY,ENTRY_KANZYU,SEX_FEMALE,
	"���Ȃ��͒n���ƌ��ƈيE�̒n���𓝂ׂ鏗�_�ł��B���ꂼ��̒n���ɂ��ꂼ��̂������Ă���A���`�����Ă���g�̂𑼂̐g�̂ƌ�コ���邱�Ƃ��ł��܂��B���i�A�����i�AHP�AMP�A�ˑR�ψفA�g���閂�@�̈�͑̂��ƂɕʁX�ɂȂ�܂��B���Ȃ��̓��̏�ɂ͑傫�ȋ��̂��ڂ��Ă���A�w�����b�g�𑕔��ł��܂���B"},

	{TRUE,"�G�^�j�e�B�����o",CLASS_LARVA,RACE_FAIRY,ENTRY_TENKUU,SEX_FEMALE,
		"���Ȃ��̓A�Q�n���̗d���ŁA�����ƍĐ��ƕϑ�(�����w�I��)�̋�ł�����܂��B�����̔\�͈͂���ȂقǂɒႢ�̂ł����������ƂĂ������A�����ɏ]���Ĕ\�͂��啝�ɏ㏸���l�X�ȓˑR�ψق��l�����܂��B���Ȃ��͂����ˑR�ψق������Ă��Đ��\�͂������Ȃ��̂ŁA���̕s����Ȑ��ݔ\�͂������Ɋ��p���邩������܂��B���Ȃ��͔�͂ȗd���̂��߁A���Ȃ��̖`���͂��Ȃ�̋�J���������邱�ƂɂȂ�ł��傤�B"},
	{TRUE,"��c�@�l���m",CLASS_NEMUNO,RACE_YOUKAI,ENTRY_TENKUU,SEX_FEMALE,
		"���Ȃ��͗d���̎R�œƎ��̐������c�ގR�W�ł��B�꒣����r�炷�҂ɂ͗e�͂����܂���B�_���W�����̒��ŒN�����Ȃ������𐹈�ɐݒ肵�A�����œG���}�����Ƃ��ɂ������Ȃ��̗͍͂ł���������܂��B���悩��o�Ă���Ƃ��̂��Ȃ��͂���قǋ����킯�ł͂Ȃ��̂ŁA�G�n�ɍU�ߍ��܂���𓾂Ȃ��󋵂ւ̑Ώ��͋��ł��B"},
	{TRUE,"����� ������",CLASS_AUNN,RACE_WARBEAST,ENTRY_TENKUU,SEX_FEMALE,
		"���Ȃ��͍����ł��B�_�Ђ₨���ɏ���ɏZ�ݒ����ĉA�Ȃ����삵�Ă��܂��B�����Ȃ̂ő��̏b�l�قǑf��������܂��񂪑̂���v�Ŕj�׍U������_�Ƃ��܂���B�܂���p�̐S���������͎����Ă��܂��B���Ȃ��̓��ɂ͑傫�Ȋp�������Ă���A������g���ċ��͂ȍU�������邱�Ƃ��ł��܂������̑���Ɋ������Ԃ邱�Ƃ��ł��܂���B���͂��Ȃ��͑̂������Ă���A�ꕔ�̓��Z���g���Ɠ�l������ӏ��Ɍ���Ĉꏏ�ɐ킢�܂��B"},
	{TRUE,"��c�� ����",CLASS_NARUMI,RACE_MAGIC_JIZO,ENTRY_TENKUU,SEX_FEMALE,
		"���Ȃ��͖��@�̐X�̖��͂Ő����𓾂ē����o�������n������ł��B�O���̋~�ς�����̎g���ƍl���Ă��܂����A�o�s���łقƂ�ǐX����o�悤�Ƃ��܂���B���Ȃ��͐�������̖��@���ł����ӂƂ��Ă���A���̗͂���ɂ��q���ł��B�l�����ꂽ�X�ŕ���ɑς��Ă������ߗ�C�U���ɋ����ϐ��������܂��B"},
	{TRUE,"���q�c ���T",CLASS_SATONO,RACE_DEMIGOD,ENTRY_TENKUU,SEX_FEMALE,
		"���Ȃ��͔�_�u������ �B��ށv�̔z���ŁA��Ȏd���͒N���̔w��ŗx�苶�����Ƃł��B���Ȃ��̓����X�^�[���X�J�E�g���Ĕw��ŗx�邱�Ƃł��̃����X�^�[�̐��_�͂��������邱�Ƃ��ł��܂��B�w��ŗx�邠�Ȃ��͂��̃����X�^�[�ɋR�悵�Ă���̂ɋ߂������ɂȂ�܂��B��n�\�ȃ����X�^�[�łȂ��Ă��w��ŗx�邱�Ƃ��ł��܂��B���͂ȃ����X�^�[���X�J�E�g�ł���Έ�C�ɐ�͂���������邱�Ƃł��傤�B���������Ȃ����g�̐퓬�\�͂͂���قǍ�������܂���B" },
	{TRUE,"����c ��",CLASS_MAI,RACE_DEMIGOD,ENTRY_TENKUU,SEX_FEMALE,
		"���Ȃ��͔�_�u������ �B��ށv�̔z���ŁA��Ȏd���͒N���̔w��ŗx�苶�����Ƃł��B���Ȃ��̓����X�^�[���X�J�E�g���Ĕw��ŗx�邱�Ƃł��̃����X�^�[�̐g�̔\�͂��������邱�Ƃ��ł��܂��B�w��ŗx�邠�Ȃ��͂��̃����X�^�[�ɋR�悵�Ă���̂ɋ߂������ɂȂ�܂��B��n�\�ȃ����X�^�[�łȂ��Ă��w��ŗx�邱�Ƃ��ł��܂��B���͂ȃ����X�^�[���X�J�E�g�ł���Έ�C�ɐ�͂���������邱�Ƃł��傤�B���������Ȃ����g�̐퓬�\�͂͂���قǍ�������܂���B"},
	{TRUE,"������ �B���",CLASS_OKINA,RACE_DEITY,ENTRY_TENKUU,SEX_FEMALE,
		"���Ȃ��͒����ɂ킽��ł̉��ɔ邳��Ă�����߂����_�l�ł��B�s�v�c�Ȍ�˂̗͂ł���܂Ō��z���𗠂���x���Ă��܂����B���Ȃ��͂��������̃����X�^�[�̔w���̔���ʂ��ė͂��z���������Ă���A���x���A�b�v�ɔ��������X�^�[�B�̓��Z���䂪���Ƃ��܂��B�܂����Ȃ��͌�˂̐_�l�̑��ɂ��l�X�Ȑ_�l�̊�������Ă��āA���G���Ȃ�������͂Ȃ������̓��Z���K�����܂��B���Ȃ��͈ł̉���^���̂悤�Ɍ��ʂ��A�������̂���n�`���܂�Ŕ��ȂǂȂ����̂悤�ɕ��ނ��Ƃ��ł��܂��B"},

	{ TRUE,"�^ ����",CLASS_EIKA,RACE_STRAYGOD,ENTRY_KIKEI,SEX_FEMALE,
		"���Ȃ��͐_�i�������鐅�q��ŁA�΂̉͌��Ŏq���B�̗�ƈꏏ�ɐ΂�ς�ŕ�炵�Ă��܂��B���Ȃ��̓��Z�͐ΐς݂ł��B���Ȃ����F������߂Đς񂾐΂͐_���ȗ͂𔭊����܂��B���������Ȃ����g�͐키�͂��قƂ�ǎ����Ă��܂���B���Ȃ��̖`���͂܂��ɐΐς݂̂悤�ȋ�s�ƂȂ邱�Ƃł��傤�B���Ȃ��͌o����ςނ��ƂŐ_�i�Ƃ��Đ��n���\�͂��傫���ς��܂��B" },
	{ TRUE,"���� ����",CLASS_URUMI,RACE_YOUKAI,ENTRY_KIKEI,SEX_FEMALE,
		"���Ȃ��͎O�r�̉͂ŌÑ㋛��߂��ĕ�炷���S�ł��B���Ȃ��ɂ͐g�߂ȕ��̏d����ς���\�͂�����A���ӋZ�͐΂̐Ԏq�������t���Ē��d�ʂɂ��邱�ƂŃ����X�^�[�𐅂ɒ��߂邱�Ƃł��B���Ȃ��͐��ӂłȂ��Ɩ{�̂𔭊����邱�Ƃ��ł��܂��񂪁A�ǂ̂悤�ȏꏊ�ł������g�̔\�͂��������Ă���Ȃ�ɐ키���Ƃ��ł���ł��傤�B" },
	{ TRUE,"��n �v�̉�",CLASS_KUTAKA,RACE_DEITY,ENTRY_KIKEI,SEX_FEMALE,
		"���Ȃ��̓j���^���_�Ƃ����{�̐_�l�ł��B���i�͔ފ݂̊֏��Ŗ�Ԃ��΂߂Ă��܂��B���Ȃ��ɂ͍A�̕a�C������\�͂�����A�łɑ΂���ϐ�������܂��B�܂����͂̂��̂����m�����菵���ꂴ��҂�ދ�������悤�ȓ��Z���K�����A���@����̈�w�K�\�ł��B���Ȃ��̓��̏�ɂ͉��炵���q���R���Z��ł���A�s���`�̂Ƃ��ɂ̓s���s�����Ċ�@�������Ă���܂��B" },
	{ TRUE,"�g�� ����d",CLASS_YACHIE,RACE_ANIMAL_GHOST,ENTRY_KIKEI,SEX_FEMALE,
		"���Ȃ��͒{���E�̋���g�D�ł���S���g�i�������݁j�̑g���ł��B�t�炤�C�͂����킹��\�͂������Ă���A���U�ɂ���ă��j�[�N�����X�^�[��z���ɂ��邱�Ƃ��ł��܂��B���ɂ��G�̓�����W�Q������z�����x��������Z���K�����܂��B���Ȃ��͖d�������点�邱�Ƃ��D�݂܂������e��������ċ��ł͂���܂���B���������h�Ȋp�ƐK���ƍb�����ז��ŊZ�⊕�Ȃǂ̏d���������邱�Ƃ��ł��܂���B" },
	{ TRUE,"�񓁋� ���|",CLASS_MAYUMI,RACE_HANIWA,ENTRY_KIKEI,SEX_FEMALE,
		"���Ȃ��͑��`�_�w�����_�@�ݕP�x���n��グ�����֕��c�̕����ł��B���ւ̕��m������������Z���K�����A�ႢMP�R�X�g�ő����̏��ւ̔z�����ێ��ł��܂��B���Ȃ����g�̐퓬�\�͂����ɍ����A�Ƃ��Ɍ��E�|�E�R��̋Z�\�ɍ����K���������܂��B�܂����Ȃ��ɂ͒����S��͂ɂ���\�͂�����A���̔\�͂ɂ��r�͂Ƒϋv�͂������̐��l���Ⴍ�Ȃ邱�Ƃ�����܂���B���Ȃ��̌����ڂ͂قƂ�ǐl�Ԃƕς��܂��񂪁A���Ȃ����[�͂����ǂ��̏��ւł��B���̂��ߗl�X�Ȏ�ނ̍U���ɑ΂��Ēʏ�̐����ɂ͗L�蓾�Ȃ��قǂ̒�R�͂������܂����A������j�Ђ̍U���͋��ł��B" },
	{ TRUE,"�����_ �ݕP",CLASS_KEIKI,RACE_DEITY,ENTRY_KIKEI,SEX_FEMALE,
		"���Ȃ��͒{���E�̋s����ꂵ�l�ԗ삽���̋F��ɉ����Č��ꂽ���`�_�ł��B���i�͓����⋾����ւ̐���ɋ΂���ł��܂����A���ɂ͎���O���ɏo�Đ키���Ƃ�����܂��B	���Ȃ��͕���h��⑕���i���炻�̔\��(�G�b�Z���X)�𒊏o���A������g���đ��̑����i������������ꕔ�̓��Z�𔭓����邱�Ƃ��ł��܂��B�܂����@����̈�I�݂Ɉ������Ƃ��ł��A�ߐڐ퓬�̔\�͂��Ⴍ�͂���܂���B������x�̏����𐮂���΂ǂ̂悤�ȓG������ł��L���ɐ킢��i�߂邱�Ƃ��ł���ł��傤�B" },
	{ TRUE,"鋋� ���S",CLASS_SAKI,RACE_ANIMAL_GHOST,ENTRY_KIKEI,SEX_FEMALE,
		"���Ȃ��͒{���E�̋���g�D�ł��除��g�i���������݁j�̑g���ł��B	���ꂱ��v�������点�邱�Ƃ͍D�܂��A�s����ɗ����ӂ�������̂͑S�ďR�U�炷�̂����Ȃ��̗��V�ł��B���Ȃ��͋��x�ȋr�͂ɂ���Ĕ��ɑf�����������Ƃ��ł��A�܂������X�^�[�̍U���ɂ���Č������󂯂邱�Ƃ�����܂���B�i���Əe�����ӂł���ɂ�����邢�����̓��Z���K�����܂��B������ł���ɂ��ւ�炸�M������_�Ƃ��܂��񂪁A�j�׍U���͂�͂���ł��B" },

	{ TRUE,"�������@�~�P",CLASS_MIKE,RACE_WARBEAST,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͎O�єL�̏����L�ł��B���L��`�̏����L�Љ��r�˂��ꂽ�ߋ��������A���͗d���̎R�ŋC�y�ɕ�炵�Ă��܂��B���Ȃ��͏����L�Ƃ��ċq������Ăъ񂹂�\�͂𓾂�͂��ł������A�\���ȏC�s���ł��Ȃ����������Ńf�����b�g�𔺂��Ȃ̂���\�͂ɂȂ��Ă��܂��܂����B�󋵂����ɂ߂Č����\�͂��g���K�v������܂��B���Ȃ��̓A�r���e�B�J�[�h�ɖڂ������s�ɏ���Ĉ�ׂ����悤�Ɩژ_��ł��܂��B�����������L�Љ�̖W�Q�ɂ��J�[�h�̔����ɂ͗������邱�Ƃ��ł��܂���B�J�[�h�������A�J�[�h�����l�d�Ƃ̌��A�U�V�I�̓q��ȂǑ��̎�i�ŃJ�[�h����ɓ����K�v������܂��B�O�r�͑���ł��������ɂ�蔲���܂��傤�B" },
	{ TRUE,"�R��@������",CLASS_TAKANE,RACE_YAMAWARO,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͌������哾�ӂȎR���ł��B�ŋߏo���n�߂��u�A�r���e�B�J�[�h�v�ɖڂ����J�[�h���l�Ƃ��Ĉ���g���邱�Ƃɂ��܂����B���������̗d����������𑈂��ăA�r���e�B�J�[�h�𔃂��W�ߎn�߂Ă��荂���͕K���ł��B�ꍏ�����������𒙂߂ăJ�[�h�𔃂�����܂��傤�B���Ȃ��͔w���ɑ�^�̃J�[�h�P�[�X��w�����Ă���A���̒��ɃJ�[�h��16��ނ܂Ŏ��[���邱�Ƃ��ł��܂��B����ɐX�̋C�𑀂�\�͂������A�X�Ɋ֌W�������̓��Z���K�����܂��B���������@���w�K���邱�Ƃ͂ł����g�̔\�͂����܂荂������܂���B" },
	{ TRUE,"��@�R�@",CLASS_SANNYO,RACE_YOUKAI,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͗d���̎R�œq����J������u�R���Y�v�Ƃ����d���ł��B���ʂȃ^�o�R���z�����ƂŎ��͂̃����X�^�[�ɞN�O�⍬���Ȃǂ̌��ʂ�^����\�͂������Ă��܂��B��x�^�o�R���z���n�߂�Ƃ��̌��ʂ͖��^�[���p���I�ɔ������A���̂��т�MP����������܂��B�܂��q��̎�Ȃ̂Řr���߂ɂ͑����̊o��������܂��B����g�����퓬�p������̕��ł��B���Ȃ��́u�A�r���e�B�J�[�h�v���g�����q���ň�ׂ����邽�߂ɃJ�[�h���d����悤�Ƃ��Ă��܂��B�������������肪����ɑ����n�߂Ă���A�J�[�h�̉��i�͍������n�߂Ă��܂��B�}���ł������҂��ŃJ�[�h�𔃂�������܂��傤�B" },
	{ TRUE,"�ʑ��@���{��",CLASS_MISUMARU,RACE_DEITY,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͌��ʐ���E�l�ł��B�z���Ȃǂ̑f�ނ�����ʂ𐧍삵�A������p�X���b�g�ɑ������Ĕ������邱�Ƃŗl�X�Ȕ\�͂𔭊����邱�Ƃ��ł��܂��B�g�p����f�ނɂ���Č��ʂɔ�������\�͂̎�ނ⋭�����ω����܂��B�܂����Ȃ��͊X�̒b�����M���h�ŕ�΂⑕���i����G�b�Z���X�𒊏o�������ʂ̑����i�ɕt�^���邱�Ƃ��ł��܂��B����ɖ��@����̈�K�����邱�Ƃ��ł��܂����A���e��͑S���̕s����ł��B�܂����ʂ̑@�ׂȗ͂��g�����ߗ򉻍U�������ł��B" },
	{ TRUE,"���q�@�T",CLASS_TSUKASA,RACE_YOUKO,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͑�V��Ɏd����ǌςł��B�l�ɚ��������č����j�łƍ����������炷���Ƃ�����Ȃ��D�݂܂��B���Ȃ��͔z�������X�^�[�̔w��ɉB��đ���u�񐶁v�Ƃ�������ȋR���ԂɂȂ邱�Ƃ��ł��܂��B�ʏ�R��\�ȃ����X�^�[�ȊO�ɂ��񐶂��ł��A���̃����X�^�[����̍U���͍��m���Ŕz�������X�^�[���󂯁A�܂��z�������X�^�[���키���Ƃœ����o���l��A�C�e���͑S�Ă��Ȃ�������肵�܂��B���Ȃ��̓��e��\�͂͑S���b�ɂȂ�܂��񂪑���ɖ��@����̈�K�����邱�Ƃ��ł��܂��B�z�������X�^�[�����ɂ���납�疂�@����Z�ōU������̂���{�I�Ȑ킢���ɂȂ�܂��B���Ȃ��͕��������̂������ŁA�򉻂≘���̍U���Œʏ��葽���̃_���[�W���󂯂Ă��܂��܂��B" },
	{ TRUE,"�эj�ہ@��",CLASS_MEGUMU,RACE_KARASU_TENGU,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͗d���̎R����V�炽���𑩂˂��V��ł��B���ɓ������f�����@�m�ɕx�ݕ��������̋����ł��B����Ɏ��E�����ׂĂ��U�����鋭�͂ȓ��Z�𕡐��K�����܂��B���������������Ă������߃��x���A�b�v�ɕK�v�Ȍo���l�͑����Ȃ�܂��B�܂����ɂ͓��݂��������ł��܂���B" },
	{ TRUE,"�V�|�@�疒",CLASS_CHIMATA,RACE_DEITY,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͎s��̐_�ł��B���������Ȃ����i������̏�Ƃ��Ă̎s��͂Ƃ��̐̂ɔp��A���Ȃ��͗͂������������肻���ɂȂ��Ă��܂��B����ȂƂ��ɂ����V�炪�N���񐶂̃r�W�l�X�X�L�[������������ł��܂����B���ꂪ��������΂��Ȃ��͍Ăт��Ă̗͂����߂����Ƃ��ł���ł��傤�B���̂��߂ɂ͌��z�����̐l�d�ɐڐG���Ĕ\�͂��R�s�[���A�A�r���e�B�J�[�h�Ƃ��ė��ʂ����Ȃ���΂Ȃ�܂���B�\�͂̃R�s�[�ɂ͒m�\�Ɗ�p�����K�v�ł��B�R�s�[����O�ɓ|���Ă��܂��Ƃ��̎҂̔\�͓͂�x�Ǝ�ɓ���Ȃ��̂ŋC�����܂��傤�B�ŏ����̂��Ȃ��͉B���\�͂����͍����̂ł������̔\�͂͒�߂ł��B�����̃J�[�h�����ʂ���Ƃ��Ȃ��̔\�͂Ɠ��Z�͋��͂ɂȂ�A����ɉB���\�͂͒ቺ���܂��B" },
	{ TRUE,"�P���@�S�X��",CLASS_MOMOYO,RACE_DAIYOUKAI,ENTRY_KOURYUU,SEX_FEMALE,
		"���Ȃ��͋����ȑ���n(�I�I���J�f)�̑�d���ł��B���ɍ������̔\�͂Ɛ퓬�n�Z�\�������A�l�X�ȍU����h��̓��Z���K�����A����Ƀ��x�����オ��ƍU���ɓő������ǉ�����܂��B�܂���͂��𑕔�����ƕ���̈З͂Ƀ{�[�i�X�����܂��B��Έ�̉��荇���Ȃ�قړG�Ȃ��ł��傤�B�������B����T���͂���������ł��B���Ոȍ~�͂Ȃ�ׂ����͂ɒ��ӂ𕥂��܂��傤�B���Ȃ��͐H���R�}���h�ōz�΂�H�ׂĈꎞ�I�ɓ��ʂȌ��ʂ𓾂邱�Ƃ��ł��܂��B" },

	{ TRUE,"���@���V",CLASS_BITEN,RACE_YOUKAI,ENTRY_JUUOUEN,SEX_FEMALE,
		"���Ȃ��͂ق�̏����O�܂ł����̎R���ł������A����S�ɂ����̂�����Đ��n�̗����������Ƃŗd�������܂����B���̌����]�Ȑ܂��o�Ē{���E�̓�����̖\�͑g�D�u�S���g�v�̗V�����ɏ������Ă��܂��B���Ƃ������������ĐX�ł̗������͂���̕��ł��B�퓬�ɂ����Ă͖_�p�Ɠ����𓾈ӂƂ��܂��B����ɐ�p����̈�K�����邱�Ƃ��ł��܂���������̓K���͂��܂荂������܂���B" },
	{ TRUE,"�O���@�d�m�q",CLASS_ENOKO,RACE_WARBEAST,ENTRY_JUUOUEN,SEX_FEMALE,
		"���Ȃ��͔j���m���������̓����c���̓���H����ĉi���̎􂢂��󂯂����}�C�k�ł��B���r�ɕ����ȃg���o�T�~�����t�����Ă���A����ɂ���ċ��͂Ȋi���U�����ł��܂�������ɕ���⏂�𑕔����邱�Ƃ��ł��Ȃ��Ȃ�܂��B��⊪���Ȃǂ̏��Օi�͖��Ȃ��g�p�ł��܂��B�܂����Ȃ���㩂̈����𓾈ӂƂ��Ă���A㩂�ݒu���ēG��㩂ɂ����邱�Ƃ��ł��܂��B�G�ƑΛ�����O�Ƀ_���[�W��^�������̉�������Ȃǂ̃g���b�L�[�Ȑ킢�����\�ł��B���Ȃ��͋S�̕�ʂ��񂩂炩���Ă��܂��B���̕�ʂ͂��Ȃ��ɑ傫�ȗ͂������炵�܂����A����𑕔����Ă���Ǝ���̐l�d�����_���ďP���������Ă��܂��B" },
	{ TRUE,"�V�ΐl�@�����",CLASS_CHIYARI,RACE_YOUKAI,ENTRY_JUUOUEN,SEX_FEMALE,
		"���Ȃ��́u�V�ΐl�v�ƌĂ΂��z���d���ł��B�����̒r�n���ɒ����Ԉ����Ă����Ă��܂������ŋߊO�ɏo�Ă��܂����B���Ȃ��͒ʏ�̐H�������ɍ��킸�A����ɓ��Z�ő��҂��猌��D���Ă�������ނ��Ƃŕ��𖞂����đ̗͂Ɩ��͂��񕜂��܂��B���Ȃ��͎��ꂽ�����D�݂̂��߃]���r��f�[�����̌��ł��C�ɂ��܂���B�܂����Ȃ��͎���̌����g���čU���͂�啝�ɋ���������Z�������Ă��܂��B�g�������ďo���_���[�W�œ|��Ȃ��悤�ɋC�����܂��傤�B" },
	{ TRUE,"����s�@������",CLASS_HISAMI,RACE_DEATH,ENTRY_JUUOUEN,SEX_FEMALE,
		"���Ȃ��͒n���̏Z�l�̉���X��(�����c�V�R��)�ł��B�����c���̕����Ƃ��Ēn���̈ē��l��������ق���҂�������ɒn���ɗU�����肵�ĕ�炵�Ă��܂��B�퓬�ɂ����Ă͖��@����̂ɐ킢�܂��B�܂��G���S�����������̓G���ǂ��܂ł��ǂ������񂷂悤�ȓ��Z���K�����܂��B���Ȃ��͊Ö���H�ׂ��MP���񕜂��邱�Ƃ��ł��܂��B" },
	{ TRUE,"�����@�c��",CLASS_ZANMU,RACE_ONI,ENTRY_JUUOUEN,SEX_FEMALE,
		"���Ȃ��͒n�����x�z������v�������炵���S�ł��B���Ă͐l�Ԃ̔j���m�ł����B���Ȃ��̍����m���͌��݉ߋ������̗l�X�Ȃ��̂����ʂ��܂��B�퓬�ɂ����Ă͓��e����͖��@�Ⓤ���Ȃǂ𓾈ӂƂ��܂��B���@�͂������̈悩���̈���K���\�ł��B" },

	{TRUE,"�ɐ��@��",CLASS_SUIKA,RACE_ONI,ENTRY_TASOGARE,SEX_FEMALE,
	"���Ȃ��͌��z���Œ��炭�Y����Ă�������ȋS�ł��B�����Ȃ���ō��N���X�̐g�̔\�͂������i���Ɠ݊�𓾈ӂƂ��܂��B���Ƒa�𑀂�\�͂������A���剻�����薶�ɂȂ�����A�C�e�����W�߂�ȂǑ��ʂȋZ���K�����܂��B���Ȃ��͂������������ł��|��܂��񂪁A���������߂�Ɣ\�͂��啝�ɒቺ���Ă��܂��܂��B"},
	{TRUE,"�i�]�@�ߋ�",CLASS_IKU,RACE_YOUKAI,ENTRY_TASOGARE,SEX_FEMALE,
	"���Ȃ��͗��{�̎g���ƌĂ΂��d���ł��B���_�l�Ɏd���A���i�͉_�C�̒���Y���ĕ�炵�Ă��܂��B��C��ǂޔ\�͂������A�_���W�����̊댯�x��A�C�e���̉��l��f�����@������G�̍U�����󂯗������Ƃ��ł��܂��B�܂����𑀂邱�Ƃ����ӂł��B����▂�@�͂���Ȃ�ɐF�X�g���܂����A�H�߂�p�����i�����ł����ӂƂ��܂��B�Ȃ��A�H�߂͓V��l����̎x���i�̂��߉�ꂽ���ςȂ��ƂɂȂ�܂��B"},
	{TRUE,"��ߖ����@�V�q",CLASS_TENSHI,RACE_TENNIN,ENTRY_TASOGARE,SEX_FEMALE,
	"���Ȃ��͂Ƃ��閼�Ƃ��V�l�ƂȂ����Ƃ����łɓV�l�ɂȂ������l�ԂŁA�f�s�������V�E�ŕs�ǓV�l�ƌĂ΂�Ă��܂��B���������Ȃ��̎��͖͂{���ŁA��n�ɓ���������l�X�ȋ��͂ȓ��Z���g�����Ȃ��܂��B���Ȃ��̈��p�̕���͓V�E�̕󌕂ł���u��z�̌��v�ł��B�ꕔ�̓��Z�͂��̌��𑕔����Ă��Ȃ��Ǝg���܂���B"},
	{TRUE,"�`�@������",CLASS_KOKORO,RACE_TSUKUMO,ENTRY_TASOGARE,SEX_FEMALE,
	"���Ȃ��͉��ʂ̕t�r�_�Ŗʗ�C�Ƃ��Ă΂�܂��B���Ȃ��̖{�̂͐�N�ȏ�O�ɍ����Ȑ��l�ɍ��ꂽ�Z�\�Z�̖ʂŁA���̂��߂��̕ӂ̕t�r�_�ɔ�ז��炩�ɋ����͂������܂��B���鎖�������������ɖL���Ȋ����(���\��̂܂�)�\���悤�ɂȂ�A���݂͊�������肳���邽�߂̏C�s�ƌ����̓��X���߂����Ă��܂��B�l�̊���𑀂�͂������A�G�����ꂳ�����荬�������邱�Ƃ����ӂł��B�G�����ȋZ�œ|�����Ƃł��̓G�̊����\���ʂ��쐬���A����𑕔����Đ키���Ƃł��̓G�̋Z���Č����邱�Ƃ��ł��܂��B���������Ȃ��͒ʏ�̓��h��𑕔��ł��܂���B"},
	{TRUE,"�����@�j����",CLASS_SHINMYOU_2,RACE_KOBITO,ENTRY_TASOGARE,SEX_FEMALE,
	"���Ȃ��͈ꐡ�@�t�̖���ł���A�`���̑ŏo�̏��Ƃ̐����Ȍp���҂ł��B����͋���ԕs�v�c�Ȃ��o�ɏ���Đ킢�܂��B���Ȃ��͐l�̕G���炢�̐g�������Ȃ����l�Ȃ̂ŁA���̔\�͂͋ɂ߂ĕn��ŏd�����̂𑕔��ł��܂���B�����Z�\�ƕq�����A�����ėl�X�ȃg���b�L�[�ȓ��Z���g�����Ȃ��Ċ��H���J���܂��傤�B"},
	{TRUE,"�F�����@俎q",CLASS_SUMIREKO,RACE_JK,ENTRY_TASOGARE,SEX_FEMALE,
	"���Ȃ��͊O�E�̏��q�����Œ��\�͎҂ł��B�D��S���猶�z���֌��E�j����d�|���悤�Ƃ������̂̌��z���̐l�d�ɑj�~����A���͊O�E�Ŗ����Ă���Ԃɖ��̒��Ō��z���֖K��Ă��܂��Bm�R�}���h����ʏ�E�Ƃ̒��\�͎҂Ɠ��l�̒��\�͂��g�p�ł��A�����J�R�}���h������Ǝ��̒��\�͂��g�p�ł��܂��B�Ǝ��̔\�͓͂����Ɋւ�����̂������U���Ɍ����Ă��܂��B����A���Ȃ��̐g�̔\�͕͂��ʂ̐l�Ԃ��̂��̂ŕ���̐S�����S������܂���B���̂��ߒʏ�E�Ƃ̒��\�͎҂����e�킪���ł��B"},

	{ TRUE,"�ː_�@����",CLASS_JYOON,RACE_DEITY,ENTRY_TASOGARE,SEX_FEMALE,
		"���Ȃ��͉u�a�_�ł��B�A�C�e����⏕���@�ȂǑ����̂��̂����G�͂��Ȃ��̉a�H�ƂȂ邱�Ƃł��傤�B������̂��Ȃ��͗���̎w��8���̎w�ւ𑕔����邱�Ƃ��ł��܂����A�w�ւ̃p�����[�^�͔������Ă��܂��܂��B���Ȃ��͊i���U��������Ƃ��ɑ��҂������D�悷�邱�Ƃ��ł��A�܂����ɂ͒D��������h��ɎU�����Ȃ���키���Ƃ��ł��܂��B" },
	{ TRUE,"�ː_�@����",CLASS_SHION,RACE_DEITY,ENTRY_TASOGARE,SEX_FEMALE,
		"���Ȃ��͒N�����������n�R�_�ł��B��ςȎ؋�������Ă���A�����I�ɓX��X�̎{�݂𗘗p���邱�Ƃ��ł��܂���B����Ɏ���Ă��Ȃ������i�͑����ɍ�����������ꔼ���قǂŖv������Ă��܂��܂��B���Ȃ����_���W�����ŕ��ʂɐ키���Ƃ͋ɂ߂č���ŁA�u�������S�߈ˁv�̓��Z���g���ă����X�^�[�̑̂��������Đ키���ƂɂȂ�܂��B���Ȃ��͎��͂ŋN�������s�K�����������̐g�ɒ~�ς��Ă����A���ߍ��񂾕s�K���J�����ĂƂĂ��Ȃ������ȁu�X�[�p�[�n�R�_�v�ɕϐg���邱�Ƃ��ł��܂��B" },
	{ TRUE,"�b�W�@�ޖ�",CLASS_YUMA,RACE_DEITY,ENTRY_TASOGARE,SEX_FEMALE,
		"���Ȃ��͒{���E�ō��~�����𗦂��铯�����ł��B�d���Ɠ��e��̗����ɏG�ŁA�Ƃ��ɑϋv�͂ɗD��܂��B�����镨��H�ׂ邱�Ƃ��ł��A�H�ׂ����̂ɂ���ĉh�{�����łȂ��ϐ��A�\�́A���Z�A�o���l�AMP�ȂǗl�X�Ȃ��̂��l�����܂��B���Ȃ��͒ʏ�̏\�{�̖����x���E�������܂����������鑬�x������ȏ�ɑ����A����ɗאڍU������Z�̎g�p�ɂ���Ă������x��啝�ɏ���܂��B�Ȃ�ׂ���������Ȃ��s����S�����܂��傤�B�������������ē|��Ă��܂��Ƒ���s�\�ɂȂ���͂̂�������̂𖳍��ʂɋz�����ݎn�߂܂��B�����z�����ނ��̂��Ȃ��ꍇ�Q�[���I�[�o�[�ɂȂ�܂Ŏ~�܂�܂���B" },

	{TRUE,"���̎O�d��",CLASS_3_FAIRIES,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ������͌��̎O�d���ƌĂ΂��O�l�g�̗d���ł��B	�w�T�j�[�~���N�x�͌������܂�����\�͂��A�w���i�`���C���h�x�͉��������\�͂��A�w�X�^�[�T�t�@�C�A�x�͎��͂̐�����T��\�͂������܂��B	�O�l�ꏏ�Ƀ_���W�����ɒ��݁A�O�l���킹�ĕ����2�A����1�A������1�A�h���A�N�Z�T����8�܂ő����\�ł��B�����������ʂ̑����i�͍ő�3�܂łƂȂ�܂��B	�񓁗���ߐځE�ˌ����퓯�������̃y�i���e�B�͔������܂���B"},
	{TRUE,"�T�j�[�~���N",CLASS_SUNNY,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ��́u���̎O�d���v�ƌĂ΂��d���O�l�g�̈�l�ŁA�z���̗d���ł��B�������C�ōs���͂�����A���̏�̎v�����ő��̓�l����������񂵂܂��B���Ȃ��͌������܂�����\�͂������Ă���A�G�����e�Řf�킹��������ōU�����邱�Ƃ��ł��܂��B�퓬�ɂ����Ă͔�r�I���e��Ɍ����Ă���A��������ѓ���̈����͋��ł��B���Ȃ��͔�͂ȗd���̂��߁A���Ȃ��̖`���͂��Ȃ�̋�J���������邱�ƂɂȂ�ł��傤�B"},
	{TRUE,"���i�`���C���h",CLASS_LUNAR,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ��́u���̎O�d���v�ƌĂ΂��d���O�l�g�̈�l�ŁA�����̗d���ł��B�d���ɂ͒������A�Â��ɖ{��ǂ�ŉ߂������Ƃ��D�݂܂��B���Ȃ��͎��͂̉��������\�͂������Ă���A���@�̉r����W�Q���邱�Ƃ��ł��܂��B�퓬�ɂ����Ă͓��e��͂��܂蓾�ӂł͂Ȃ��A���@����Z�Ȃǂ�D������Ă��܂����܂��키�K�v������܂��B�Ƃ��ǂ���������]��ŕ���𗎂Ƃ��̂ŋC��t���Ă��������B���Ȃ��͔�͂ȗd���̂��߁A���Ȃ��̖`���͂��Ȃ�̋�J���������邱�ƂɂȂ�ł��傤�B"},
	{TRUE,"�X�^�[�T�t�@�C�A",CLASS_STAR,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ��́u���̎O�d���v�ƌĂ΂��d���O�l�g�̈�l�ŁA���̌��̗d���ł��B���i�͂ق��̓�l�������̂��y�������Ɍ��Ă��܂�(�����Ċ댯������Ɛ^����ɓ����܂�)���A���ɂ͋����悤�ȍs���͂𔭊����邱�Ƃ�����܂��B���Ȃ��͋߂��̐����̏ꏊ��T��\�͂������Ă���A���x�����オ��Ɗ��m�ł�����̂̎�ނ�͈͂������܂��B�퓬�ɂ����Ă͓��e��͋��ŁA������≓�u�U���A㩂Ȃǂ���g���Đ키�K�v������܂��B���Ȃ��͔�͂ȗd���̂��߁A���Ȃ��̖`���͂��Ȃ�̋�J���������邱�ƂɂȂ�ł��傤�B"},
	{TRUE,"��؁@�ؐ�",CLASS_KASEN,RACE_SENNIN,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ��͓����B�𓱂����Ƃ��g���Ƃ���V�Ă̐�l�ł��B�������g������肱�Ȃ��\�͂ɒ����Ă��܂��B�V�Đ�l�̊��ɂȂ����ٗl�ɐg�̔\�͂������A�Ȃ����w�����b�g�𑕔��ł����A�Ȃ����j�׍U������_�ł��B��ɕ�т������Ă���E��͂������̔�߂�ꂽ�͂������Ă��܂��B"},
	{TRUE,"�P�C���@�͂���",CLASS_HATATE,RACE_KARASU_TENGU,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ�����V��̐V���L�҂ŁA�˖��ە��Ƃ̓��C�o���֌W�ł��B��V��̊��ɂ͂��q�����Ɍ����܂����O�ʂ̔\�͂������Ă��芴�m��T���ɗD��܂��B�ڋߐ���͎ˌ��ɓK���������A�����E�Í��E�\���̂����ꂩ�̗̈�̖��@��������x�K���\�ł��B"},
	{TRUE,"�Ȍ��@�L�P",CLASS_TOYOHIME,RACE_LUNARIAN,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ��͌��̖��Ƃ̕P�ŁA�˕P�̎o�ł��B�C�ƎR���q���͂������A��K�͂ȏ������ԑ�����D������ɍs���܂��B�r���͋��ł������̍ŐV��������L���Ă���A���݂̓G�Ȃ�܂Ƃ߂Đ�����΂����Ƃ��ł��܂��B	���Ȃ����q����������̖��ł���A�l�X�Ȏ׈��ȍU���ő�_���[�W���󂯂Ă��܂��܂��B"},
	{TRUE,"�Ȍ��@�˕P",CLASS_YORIHIME,RACE_LUNARIAN,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ��͌��̖��Ƃ̕P�ŁA�L�P�̖��ł��B���S���̐_�����̐g�ɍ~�낷�͂������A������_�l�̗͂�В[����s�g���܂��B���̖h�q���̎w�����������ł�����A�ڋߐ�̋Z�\����z���Ă��܂��B���Ȃ����q����������̖��ł���A�l�X�Ȏ׈��ȍU���ő�_���[�W���󂯂Ă��܂��܂��B"},
	{TRUE,"���C�Z��",CLASS_REISEN2,RACE_GYOKUTO,ENTRY_OTHER,SEX_FEMALE,
	"���Ȃ��͌��̓e�ł��B���������������݂𝑂����������ɂȂ��Ēn��ɓ����o���܂������A���̌�F�X�����Č��̖h�q���̈���ɔ[�܂�܂����B�퓬�Ɋւ���Z���X�͂��܂�ǂ��Ȃ��悤�ł����A���ɂ͖ڂɂ��~�܂�ʏr�q�ȓ��������܂��B"},
	{TRUE,"�{���@����",CLASS_KOSUZU,RACE_HUMAN,ENTRY_OTHER,SEX_FEMALE,
		"���Ȃ��͐l���̑ݖ{���̖��ŁA�����镶����ǂ݉����\�͂������Ă��܂��B���炪�M����ׂ��^�������߁A�鑠�̏��i�����ɋl�߂ēX���яo���܂����B���������Ȃ����g�̐퓬�\�͂͂ƂĂ����̊O�̋��Ђɑ΍R�ł���悤�Ȃ��̂ł͂���܂���B���Ȃ��̖`���͉ߍ��Ȃ��̂ƂȂ�ł��傤�B"},
	{TRUE,"�N���E���s�[�X",CLASS_VFS_CLOWNPIECE,RACE_FAIRY,ENTRY_OTHER,SEX_FEMALE,
		"���Ȃ��͒n���o�g�̗d���ł��B�����O�ٕ̈ςŏd�v�ȓ��������Ȃ��A���̌�͎傩��̖������󂯂Ēn��ŕ�炵�Ă��܂��B�d���ł���Ȃ���n���œ��p�������قǂ̗͂������Ă��܂����A���̒��g�͕��ʂ̗d�����݂ł��B���Ȃ��͎�Ɏ����������Ől�X�����킹�邱�Ƃ��ł��A�܂��n���o�g���������ĉΉ��ƒn���̑����U���ɑ΂���ϐ�������܂��B" },

	{ TRUE,"����c�@����",CLASS_MIYOI,RACE_ZASHIKIWARASHI,ENTRY_OTHER,SEX_FEMALE,
		"���Ȃ��͈ɐ����̎�Z�\�w�ɐ��Z�x�ɐ��ݒ������~��炵�ł��B�Z�\�̉e���ɂ����̂��l�𐌂��ׂ��Ė���L���Ɋ����鐌���̂悤�ȗ͂������Ă��܂��B���Ȃ��ɂ͐키�͂͂قƂ�ǂ���܂��񂪁A���z�悭���������߂ēG�̐S����J������قǂ̐ڋq�\�͂�����܂��B���������x�����܂��Ă��̂܂ܐ����ׂ��Ă��܂��Γ|�����̂Ɠ��������ɂȂ�܂��B���������ɂ͎������߂Ă������Ȃ������X�^�[�₢�����������ł�����Ȃ������X�^�[�����܂��B�����������G�ɏP���Ă��܂����炠�Ȃ��ɂł��邱�Ƃ͓��S����݂̂ł��B���Ȃ��ɕK�v�Ȕ\�͖͂��͂ł��B�ڋq�̐����������łȂ����܂�����̋����ɂ��e����^���܂��B" },

	{ FALSE,"�{�o���@����",CLASS_MIZUCHI,RACE_SPECTRE,ENTRY_OTHER,SEX_FEMALE,
		"������" },



};

//Ex���[�h�Ɍ����Ȃ������������Ă���E�Ƃ̃X�R�A�{���ɕ␳(%�P�ʂŃX�R�A�{���ɏ悸��)
void extra_mode_score_mult(int *mult, int class_idx)
{
	int mod;

	if(!EXTRA_MODE) return;

	switch(class_idx)
	{
	case CLASS_WARRIOR:
		mod = 90;
		break;
	case CLASS_MAGE:
	case CLASS_HIGH_MAGE:
		mod = 120;
		break;

	case CLASS_MINDCRAFTER:
		mod = 90;
		break;
		//v1.1.62 ���@�̖�����������̂Ŕp�~
//	case CLASS_ARCHER:
//		mod = 120;
//		break;
	case CLASS_MAGIC_EATER:
		mod = 80;
		break;
	case CLASS_ENGINEER:
		mod = 150;
		break;
	case CLASS_CHEMIST:
		mod = 150;
		break;
	case CLASS_CAVALRY:
		mod = 110;
		break;
	case CLASS_TSUKUMO_MASTER:
		mod=125;
		break;
	case CLASS_SH_DEALER:
		mod=125;
		break;
	case CLASS_JEWELER:
	case CLASS_MISUMARU:
		mod=150;
		break;
	case CLASS_NINJA:
		mod=90;
		break;
	case CLASS_TEWI:
		mod=120;
		break;
	case CLASS_KOISHI:
		mod=120;
		break;

	case CLASS_MOMIZI:
		mod=70;
		break;
	case CLASS_SEIGA:
		mod = 110;
		break;
	case CLASS_ORIN:
		mod=70;
		break;
	case CLASS_PATCHOULI:
		mod = 120;
		break;
	case CLASS_MEDICINE:
		mod = 120;
		break;
	case CLASS_YUYUKO:
		mod = 120;
		break;
	case CLASS_LILY_WHITE:
		mod=80;
		break;
	case CLASS_CHEN:
		mod=125;
		break;
	case CLASS_NITORI:
		mod=120;
		break;
	case CLASS_SUWAKO:
		mod=150;
		break;
	case CLASS_SEIJA:
		mod=140;
		break;
	case CLASS_RAN:
		mod=60;
		break;
	case CLASS_SUMIREKO:
		mod=75;
		break;
	case CLASS_KANAKO:
		mod=200;
		break;
	case CLASS_CLOWNPIECE:
		mod = 40;
		break;
	case CLASS_DOREMY:
		mod = 150;
		break;
	case CLASS_HINA:
		mod = 160;
		break;
	case CLASS_SAKUYA:
		mod = 60;
		break;
	case CLASS_3_FAIRIES:
		mod = 80;
		break;
	case CLASS_RAIKO:
		mod = 150;
		break;
	case CLASS_RINGO:
		mod = 80;
		break;
	case CLASS_SAGUME:
		mod = 200;
		break;
	case CLASS_SHION:
		mod = 50;
		break;

	case CLASS_CHIMATA:
		mod = 200;
		break;

	default:
		return; //�������Ȃ�
	}

	*mult = *mult * mod / 100;

}

static bool get_unique_player(void)
{
	int i, k, n, cs, os;

	int mode = 0;
	cptr str;
	char c;
	int entry;
	unique_player_type table[16];
	int cnt_table;
	char temp[80*9];
	cptr t;
	char    buf[80], cur[80];
	int mul_exp, mul_score;
	char old_name[32];


	while(1)
	{

		bool ok = TRUE;

		put_str("���Ȃ��̓o���i��I��ł��������B(*:�����_��)", 10, 10);

		cs = 0;
		while(1)
		{
			for(n=0;n<ENTRY_MAX;n++)
			{
				if(n == cs) c_put_str(TERM_YELLOW, select_unique_entry[n], 12 + n, 10);
				else put_str(select_unique_entry[n], 12 + n, 10);
			}
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);

			if (c == '*')
			{
				k = randint0(ENTRY_MAX);
				break;
			}

			if (c == ' ' || c == '\r' || c == '\n')
			{
				k = cs;
				break;
			}
			if (c == '2' && cs < ENTRY_MAX-1) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == '?')
			{
				show_help("tbirth.txt#Class");
			}

			if (c >= 'a' && c <= 'a'+ENTRY_MAX-1)
			{
				k = c - 'a';
				break;
			}
		}
		clear_from(10);
		entry = k;


		cnt_table = 0;
		for (i = 0; i < UNIQUE_PLAYER_NUM; i++)if (unique_player_table[i].entry == entry)
		{
			if (cnt_table == 16)
			{
				put_str("ERROR:get_unique_player()��cnt_table����t", 12, 10);
				break;
			}
			table[cnt_table++] = unique_player_table[i];
		}
		if (cnt_table == 0)		c_put_str(TERM_L_BLUE, format("��iID:%d�ɒN���o�^����Ă��Ȃ�", entry), 8, 10);

		put_str("���Ȃ��͒N�ł����H(*:�����_��)", 10, 10);
		put_str("(�D���͖����� ESC�ō�i�I���֖߂�)", 11, 10);

		cs = 0;
		while (1)
		{
			for (n = 0; n < cnt_table; n++)
			{
				if (n == cs)
					c_put_str(TERM_YELLOW, format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);
				//���E����HARD�ȏ����
				else if ((table[n].class_id == CLASS_RAN || table[n].class_id == CLASS_YUKARI) && difficulty < DIFFICULTY_HARD)
					c_put_str(TERM_L_DARK, format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);
				else if (!table[n].flag_fix)
					c_put_str(TERM_L_DARK, format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);
				else
					put_str(format("%c) %s", ('a' + n), table[n].name), 12 + n, 10);

			}
			Term_gotoxy(0, 0);
			c = inkey();
			if (c == ESCAPE)
			{
				ok = FALSE;
				break;
			}
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);

			//v1.1.97 �����_���I��
			if (c == '*')
			{
				int available_number_list[64];
				int tmp_list_len = 0;

				for (n = 0; n < cnt_table; n++)
				{
					if (!table[n].flag_fix)continue;
					//���E����HARD�ȏ����
					if (table[k].class_id == CLASS_RAN && difficulty < DIFFICULTY_HARD) continue;
					if (table[k].class_id == CLASS_YUKARI && difficulty < DIFFICULTY_HARD) continue;
					available_number_list[tmp_list_len++] = n;
				}

				if (tmp_list_len)
				{
					k = available_number_list[randint0(tmp_list_len)];
					break;
				}
				//�Y���y�[�W�̎����L�������܂���l�����Ȃ��ꍇtmp_list_len��0�ɂȂ�*�������Ă������N����Ȃ�
			}


			if (c == ' ' || c == '\r' || c == '\n')
			{
				k = cs;
				if (!table[k].flag_fix)continue;
				//���E����HARD�ȏ����
				if (table[k].class_id == CLASS_RAN && difficulty < DIFFICULTY_HARD) continue;
				if (table[k].class_id == CLASS_YUKARI && difficulty < DIFFICULTY_HARD) continue;
				break;
			}
			if (c == '2' && cs < cnt_table - 1) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == '?')
			{
				show_help("tbirth.txt#Class");
			}

			if (c >= 'a' && c <= 'a' + cnt_table - 1)
			{
				k = c - 'a';
				if (!table[k].flag_fix)continue;

				//����HARD�ȏ����
				if (table[k].class_id == CLASS_RAN && difficulty < DIFFICULTY_HARD) continue;
				if (table[k].class_id == CLASS_YUKARI && difficulty < DIFFICULTY_HARD) continue;

				break;
			}
		}
		clear_from(10);
		if (!ok)
		{
			continue; //��i�I���܂Ŗ߂�
		}



		roff_to_buf(table[k].info, 74, temp, sizeof(temp));
		t = temp;
		for (i = 0; i< 9; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

		if(table[k].race == 255 || table[k].class_id == 255)
		{
			c_put_str(TERM_RED, "ERROR:���̃v���[���[�̎푰���N���X���ݒ肳��Ă��Ȃ�", 10, 10);
			inkey();
			return (FALSE);
		}
		if(!class_info[table[k].class_id].flag_only_unique)
		{
			c_put_str(TERM_RED, "ERROR:���̃N���X�Ƀ��j�[�N�N���X�t���O���ݒ肳��Ă��Ȃ�", 10, 10);
			inkey();
			return (FALSE);
		}

		/*:::���O�ύX*/
		strcpy(old_name, player_name);
		strcpy(player_name, table[k].name);
		put_str("���O  :�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@", 1,16);
		c_put_str(TERM_L_BLUE, player_name, 1, 24);

		/*:::���ʊm��*/
		p_ptr->psex = table[k].sex;
		sp_ptr = &sex_info[p_ptr->psex];
		c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 15);

		/*:::�푰�m��*/
		p_ptr->prace = table[k].race;
		rp_ptr = &race_info[p_ptr->prace];
		c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

		/*:::�E�Ɗm��*/
		p_ptr->pclass = table[k].class_id;
		cp_ptr = &class_info[p_ptr->pclass];
		if(p_ptr->psex == SEX_MALE )strcpy(player_class_name, cp_ptr->title);
		else strcpy(player_class_name, cp_ptr->f_title);



		///del m_info�폜
		//mp_ptr = &m_info[0];
		if(p_ptr->psex == SEX_MALE)	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);
		else						c_put_str(TERM_L_BLUE, cp_ptr->f_title, 5, 15);



		mul_score = cp_ptr->score_mult * rp_ptr->score_mult / 100;

		extra_mode_score_mult(&mul_score,p_ptr->pclass); 

		mul_exp = cp_ptr->c_exp + rp_ptr->r_exp - 100;

				str = rp_ptr->title;
				c_put_str(TERM_L_BLUE, table[k].name, 3, 40);
				put_str("�̔\�͒l�C��", 3, 40+strlen( table[k].name));
				put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��  �X�R�A�{��", 4, 40);
				if(table[k].race == RACE_ANDROID)
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d  **** %4d%% ",
					/*::: �d���l�`�͌o���l�␳��\�����Ȃ�*/
					rp_ptr->r_adj[0]+cp_ptr->c_adj[0], 
					rp_ptr->r_adj[1]+cp_ptr->c_adj[1],
					rp_ptr->r_adj[2]+cp_ptr->c_adj[2],
					rp_ptr->r_adj[3]+cp_ptr->c_adj[3],
					rp_ptr->r_adj[4]+cp_ptr->c_adj[4],
					rp_ptr->r_adj[5]+cp_ptr->c_adj[5],
					mul_score);

				else
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
					rp_ptr->r_adj[0]+cp_ptr->c_adj[0], 
					rp_ptr->r_adj[1]+cp_ptr->c_adj[1],
					rp_ptr->r_adj[2]+cp_ptr->c_adj[2],
					rp_ptr->r_adj[3]+cp_ptr->c_adj[3],
					rp_ptr->r_adj[4]+cp_ptr->c_adj[4],
					rp_ptr->r_adj[5]+cp_ptr->c_adj[5],
					mul_exp, mul_score);
				c_put_str(TERM_L_BLUE, buf, 5, 40);

		if (!get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y))
		{
			Term_clear();
			put_str("�L�����N�^�[���쐬���܂��B('S'��蒼��, 'Q'�I��, '?'�w���v)", 8, 10);
			put_str("���O  :�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@", 1,16);
			put_str("����        :", 3, 1);
			put_str("�푰        :", 4, 1);
			put_str("�E��        :", 5, 1);
			strcpy(player_name, old_name);
			c_put_str(TERM_L_BLUE, player_name, 1, 24);
			continue; //��i�I���܂Ŗ߂�
		}
		break;
	}
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);
	return (TRUE);

}



/*:::�O���l�ݒ�p�@�푰�E�ƌŒ�*/
static bool get_outside_player(void)
{
	int i;

	cptr str;
	char c;

	char temp[80*16];
	cptr t;
	char    buf[80], cur[80];
	int mul_exp, mul_score;

	/*:::�푰�m��*/
	p_ptr->prace = RACE_HUMAN;
	rp_ptr = &race_info[p_ptr->prace];
	c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);

	/*:::�E�Ɗm��*/
	p_ptr->pclass = CLASS_OUTSIDER;
	cp_ptr = &class_info[p_ptr->pclass];
	/*:::�܂����ʂ�ݒ肵�ĂȂ����E�Ɩ��͒j������*/
	strcpy(player_class_name, cp_ptr->title); 
	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);

	clear_from(10);
	roff_to_buf(class_jouhou[p_ptr->pclass], 74, temp, sizeof(temp));
	t = temp;
	for (i = 0; i< 12; i++)
	{
		if(t[0] == 0)
			break; 
		else
		{
			prt(t, 12+i, 3);
			t += strlen(t) + 1;
		}
	}
	mul_score = cp_ptr->score_mult * rp_ptr->score_mult / 100;
	mul_exp = cp_ptr->c_exp + rp_ptr->r_exp - 100;

				str = cp_ptr->title;
				c_put_str(TERM_L_BLUE, str, 3, 40);
				put_str("�̔\�͒l�C��", 3, 40+strlen( str));
				put_str("�r�� �m�\ ���� ��p �ϋv ���� �o��  �X�R�A�{��", 4, 40);
				sprintf(buf, "%+3d  %+3d  %+3d  %+3d  %+3d  %+3d %+4d%% %4d%% ",
					rp_ptr->r_adj[0]+cp_ptr->c_adj[0], 
					rp_ptr->r_adj[1]+cp_ptr->c_adj[1],
					rp_ptr->r_adj[2]+cp_ptr->c_adj[2],
					rp_ptr->r_adj[3]+cp_ptr->c_adj[3],
					rp_ptr->r_adj[4]+cp_ptr->c_adj[4],
					rp_ptr->r_adj[5]+cp_ptr->c_adj[5],
					mul_exp, mul_score);
				c_put_str(TERM_L_BLUE, buf, 5, 40);

	if (!get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y))	return (FALSE);

	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);
	clear_from(10);

	return (TRUE);

}

/*
 * Helper function for 'player_birth()'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.  And it cuts down on player stupidity.
 */
/*:::�V�L�����N�^�[����*/
///system class race seikaku �ŏd�v
static bool player_birth_aux(void)
{
	int i, k, n, cs, os;

	int mode = 0;

	bool flag = FALSE;
	bool prev = FALSE;
	bool flag_unique = FALSE;
	bool flag_general = FALSE;
	bool flag_outsider = FALSE;


	cptr str;

	char c;

#if 0
	char p1 = '(';
#endif

	char p2 = ')';
	char b1 = '[';
	char b2 = ']';

	char buf[80], cur[80];


	/*** Intro ***/

	/* Clear screen */
	Term_clear();

	/* Title everything */
#ifdef JP
//	put_str("���O  :", 1,26);
	put_str("���O  :", 1,16);
#else
	put_str("Name  :", 1,26);
#endif

#ifdef JP
	put_str("����        :", 3, 1);
#else
	put_str("Sex         :", 3, 1);
#endif

#ifdef JP
	put_str("�푰        :", 4, 1);
#else
	put_str("Race        :", 4, 1);
#endif

#ifdef JP
	put_str("�E��        :", 5, 1);
#else
	put_str("Class       :", 5, 1);
#endif


	/* Dump the default name */
//	c_put_str(TERM_L_BLUE, player_name, 1, 34);
	c_put_str(TERM_L_BLUE, player_name, 1, 24);


	/*** Instructions ***/

	/* Display some helpful information */
#ifdef JP
	put_str("�L�����N�^�[���쐬���܂��B('S'��蒼��, 'Q'�I��, '?'�w���v)", 8, 10);
#else
	put_str("Make your charactor. ('S' Restart, 'Q' Quit, '?' Help)", 8, 10);
#endif

	

	while(1)
	{
		char temp[80*9];
		cptr t;
		cptr select_difficulty[5] = {
			"a)EASY",
			"b)NORMAL",
			"c)HARD",
			"d)LUNATIC",
			"e)EXTRA",
		};

		put_str("��Փx��I�����Ă��������F", 10, 10);
		cs = 0;
		while(1)
		{

		for(n=0;n<5;n++)
			{
				if(n == cs) c_put_str(TERM_YELLOW, select_difficulty[n], 12 + n, 10);
				//else if(n == 4 && n != cs) c_put_str(TERM_L_DARK, select_difficulty[n], 12 + n, 10);
				else put_str(select_difficulty[n], 12 + n, 10);
			}
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);
			if (c == ' ' || c == '\r' || c == '\n')
			{
				c = cs + 'a';
			}
			if (c == '2' && cs < 4) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == 'a')
			{
				difficulty = DIFFICULTY_EASY;	
				break;
			}
			if (c == 'b')
			{
				difficulty = DIFFICULTY_NORMAL;	
				break;
			}
			if (c == 'c')
			{
				difficulty = DIFFICULTY_HARD;	
				break;
			}
			if (c == 'd')
			{
				difficulty = DIFFICULTY_LUNATIC;	
				break;
			}
			if (c == 'e')
			{
				difficulty = DIFFICULTY_EXTRA;	
				break;
			}
			if (c == '?')
			{
				show_help("tbirth.txt#Difficulty");
			}

		}
		clear_from(10);
		roff_to_buf(info_difficulty[difficulty], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 9; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
		/*
		if( difficulty == DIFFICULTY_EXTRA)
		{
			char tester;
			prt("�����L�[�������Ă�������", 0, 0);
			tester = inkey();
			
			prt("", 0, 0);
			if(tester == '@') break;
		}
		else 
		*/
		if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
		clear_from(10);
	}
	clear_from(10);

	/*:::��Փx�ʂɃI�v�V������ݒ肷��B���̕ϐ��̓Z�[�u�����[�h����Ȃ��̂ŁAdungeon.c��extract_option_vars()�ɓ����L�q������K�v������*/
	switch(difficulty)
	{
	case DIFFICULTY_EASY:
		easy_band = TRUE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_NORMAL:
		easy_band = FALSE;
		smart_learn = FALSE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_HARD:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_LUNATIC:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = TRUE;
		ironman_nightmare = TRUE;
		ironman_downward = FALSE;
		break;
	case DIFFICULTY_EXTRA:
		easy_band = FALSE;
		smart_learn = TRUE;
		smart_cheat = FALSE;
		ironman_nightmare = FALSE;
		ironman_downward = TRUE;
		break;
	}

	{
		cptr select_mode[3] = {
			"a)���z���ŏ����͖��̒m��ꂽ�҂��B",
			"b)���z���̖����Ȃ��Z�l���B",
			"c)���z���H��������́H",
		};

		put_str("���Ȃ��́E�E�E", 10, 10);
		cs = 0;
		while(1)
		{
			for(n=0;n<3;n++)
			{
				if(n == cs) c_put_str(TERM_YELLOW, select_mode[n], 12 + n, 10);
				else put_str(select_mode[n], 12 + n, 10);
			}
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);
			if (c == ' ' || c == '\r' || c == '\n')
			{
				c=cs+'a';
			}
			if (c == '2' && cs < 2) cs++;
			if (c == '8' && cs > 0) cs--;
			if (c == 'a')
			{
				flag_unique = TRUE;
				break;
			}
			if (c == 'b')
			{
				flag_general = TRUE;
				break;
			}
			if (c == 'c')
			{
				flag_outsider = TRUE;
				break;
			}
			if (c == '?')
			{
				show_help("tbirth.txt#Class");
			}
		}
	}
	clear_from(10);


	//�O���l��p�i�푰�E�ƌŒ�j
	if ( flag_outsider && !get_outside_player()) return FALSE;

	/*:::���j�[�N�v���C���[�i���ʁA���O�A�푰�A����E�ƌŒ�Z�b�g)*/
	if (flag_unique && !get_unique_player()) return FALSE;

	//���ʐݒ�
if(flag_general || flag_outsider)
{

	/*** Player sex ***/

	/* Extra info */
#ifdef JP
//	put_str("���ӁF�s���ʁt�̈Ⴂ�̓Q�[����قƂ�ǉe�����y�ڂ��܂���B", 23, 5);
	put_str("���ӁF�s���ʁt�̈Ⴂ�̓Q�[����قƂ�ǉe�����y�ڂ��܂��񂪁A�����i�̑����͏����p�ł��B", 23, 5);
#else
	put_str("Note: Your 'sex' does not have any significant gameplay effects.", 23, 5);
#endif


	/* Prompt for "Sex" */
	for (n = 0; n < MAX_SEXES; n++)
	{
		/* Analyze */
		sp_ptr = &sex_info[n];

		/* Display */
#ifdef JP
		sprintf(buf, "%c%c%s", I2A(n), p2, sp_ptr->title);
#else
		sprintf(buf, "%c%c %s", I2A(n), p2, sp_ptr->title);
#endif
		put_str(buf, 12 + (n/5), 2 + 15 * (n%5));
	}

#ifdef JP
	sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
	sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif

	/* Choose */
	k = -1;
	cs = 0;
	os = MAX_SEXES;
	while (1)
	{
		if (cs != os)
		{
			put_str(cur, 12 + (os/5), 2 + 15 * (os%5));
			if(cs == MAX_SEXES)
#ifdef JP
				sprintf(cur, "%c%c%s", '*', p2, "�����_��");
#else
				sprintf(cur, "%c%c %s", '*', p2, "Random");
#endif
			else
			{
				sp_ptr = &sex_info[cs];
				str = sp_ptr->title;
#ifdef JP
				sprintf(cur, "%c%c%s", I2A(cs), p2, str);
#else
				sprintf(cur, "%c%c %s", I2A(cs), p2, str);
#endif
			}
			c_put_str(TERM_YELLOW, cur, 12 + (cs/5), 2 + 15 * (cs%5));
			os = cs;
		}

		if (k >= 0) break;

#ifdef JP
		sprintf(buf, "���ʂ�I��ŉ����� (%c-%c) ('='�����I�v�V�����ݒ�): ", I2A(0), I2A(n-1));
#else
		sprintf(buf, "Choose a sex (%c-%c) ('=' for options): ", I2A(0), I2A(n-1));
#endif

		put_str(buf, 10, 10);
		c = inkey();
		if (c == 'Q') birth_quit();
		if (c == 'S') return (FALSE);
		if (c == ' ' || c == '\r' || c == '\n')
		{
			if(cs == MAX_SEXES)
				k = randint0(MAX_SEXES);
			else
				k = cs;
			break;
		}
		if (c == '*')
		{
			k = randint0(MAX_SEXES);
			break;
		}
		if (c == '4')
		{
			if (cs > 0) cs--;
		}
		if (c == '6')
		{
			if (cs < MAX_SEXES) cs++;
		}
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < MAX_SEXES))
		{
			cs = k;
			continue;
		}
		else k = -1;
		//if (c == '?') do_cmd_help();
		//else 
		if (c == '=')
		{
			screen_save();
#ifdef JP
			do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
			do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

			screen_load();
		}
		else if(c != '4' && c != '6')bell();
	}

	/* Set sex */
	p_ptr->psex = k;
	sp_ptr = &sex_info[p_ptr->psex];

	/* Display */
	c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 15);

	/* Clean up */
	clear_from(10);
	}
	//�푰�E�Ɛݒ�
	if(flag_general)
	{
	/* Choose the players race */
	p_ptr->prace = 0;
	while(1)
	{
		char temp[80*10];
		cptr t;

		if (!get_player_race()) return FALSE;

		clear_from(10);

		roff_to_buf(race_jouhou[p_ptr->prace], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 10; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
		if(rp_ptr->flag_nofixed)
		{
			prt("�����L�[�������Ă�������", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;

		clear_from(10);
		c_put_str(TERM_WHITE, "                          ", 4, 15);
	}

	/* Clean up */
	clear_from(10);
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);




	/* Choose the players class */
	p_ptr->pclass = 0;
	while(1)
	{
		char temp[80*9];
		cptr t;

		if (!get_player_class()) return FALSE;

		clear_from(10);
		roff_to_buf(class_jouhou[p_ptr->pclass], 74, temp, sizeof(temp));
		t = temp;

		for (i = 0; i< 9; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}

		if(cp_ptr->flag_nofixed)
		{
			prt("�����L�[�������Ă�������", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;

		c_put_str(TERM_WHITE, "                 ", 5, 15);
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);


	}

	///mod140209 ���ʂ̏Z�l�͂����ŐE�Ɩ���ς�����悤�ɂ���
	if(p_ptr->pclass == CLASS_CIVILIAN)
		{
			get_class_name();
			c_put_str(TERM_WHITE, "                     ", 5, 15);
			c_put_str(TERM_L_BLUE, player_class_name, 5, 15);
		}

}//��ʐE�����I��


	put_str("                                           ", 3, 40);
	put_str("                                           ", 4, 40);
	put_str("                                           ", 5, 40);
	put_str("                                           ", 6, 40);


	/* Choose the magic realms */
	if (!get_player_realms()) return FALSE;

	/* Choose the players seikaku */
	p_ptr->pseikaku = 0;
	while(1)
	{
		char temp[80*8];
		cptr t;

		if (!get_player_seikaku()) return FALSE;

		clear_from(10);
		if(p_ptr->pseikaku == SEIKAKU_SPECIAL)
			roff_to_buf(seikaku_special_jouhou[get_special_seikaku_index(p_ptr->pclass)], 74, temp, sizeof(temp));
		else
			roff_to_buf(seikaku_jouhou[p_ptr->pseikaku], 74, temp, sizeof(temp));

		t = temp;

		for (i = 0; i< 6; i++)
		{
			if(t[0] == 0)
				break; 
			else
			{
				prt(t, 12+i, 3);
				t += strlen(t) + 1;
			}
		}
		if(ap_ptr->flag_nofixed)
		{
			prt("�����L�[�������Ă�������", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if(old_data_cheat_flag && p_ptr->pseikaku == SEIKAKU_COLLECTOR)
		{
			prt("�O��̃Z�[�u�f�[�^�Ƀ`�[�g�t���O�������Ă��邽�ߑI��s��:", 0, 0);
			(void)inkey();
			prt("", 0, 0);
		}
		else if(old_collector && p_ptr->pseikaku != SEIKAKU_COLLECTOR)
		{
			screen_save();
			Term_clear();
			prt("����I���������i�́u�N�W�Ɓv�ł͂Ȃ��̂ŁA", 10, 20);
			prt("�O��܂łɏW�߂��G���g�����X�̃A�C�e���̓��Z�b�g����܂��B", 11, 20);
			(void)inkey();
			screen_load();
			if (get_check_strict("��낵���ł����H", CHECK_OKAY_CANCEL)) break;

		}
		else if (get_check_strict("��낵���ł����H", CHECK_DEFAULT_Y)) break;
		put_str("                                                       ", 1, 24);
		c_put_str(TERM_L_BLUE, player_name, 1, 24);
		//c_put_str(TERM_L_BLUE, player_name, 1, 34);
		prt("", 1, 34+strlen(player_name));
	}

	//-Hack- �Ԕ؊��p���i�̂Ƃ����@�̈��I��������
	if (is_special_seikaku(SEIKAKU_SPECIAL_SEKIBANKI))
	{
		if (!get_player_realms()) return FALSE;

	}

	//-Hack- �ؐ��p���i�̂Ƃ��푰�ύX�����@�̈��I��������
	if (is_special_seikaku(SEIKAKU_SPECIAL_KASEN))
	{
		p_ptr->prace = RACE_ONI;
		rp_ptr = &race_info[p_ptr->prace];
		put_str("�푰        :�@�@�@�@�@�@�@", 4, 1);
		c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);
		if (!get_player_realms()) return FALSE;
	}



	/* Clean up */
	clear_from(10);
	put_str("                                                       ", 3, 40);
	put_str("                                                       ", 4, 40);
	put_str("                                                       ", 5, 40);

	screen_save();
#ifdef JP
	do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
	do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

	screen_load();


#if 0
#ifdef ALLOW_AUTOROLLER

	/*** Autoroll ***/

	if (autoroller || autochara)
	{
		/* Clear fields */
		auto_round = 0L;
	}

	/* Initialize */
	if (autoroller)
	{
		if (!get_stat_limits()) return FALSE;
	}

	if (autochara)
	{
		///tmp131221 �g���̏d�N��֘A�̏������\����
		//if (!get_chara_limits()) return FALSE;
	}

#endif /* ALLOW_AUTOROLLER */

	/* Clear */
	clear_from(10);

	/* Reset turn; before auto-roll and after choosing race */
	init_turn();

	/*** Generate ***/

	/* Roll */
	while (TRUE)
	{
		int col;

		col = 42;

		if (autoroller || autochara)
		{
			Term_clear();

			/* Label count */
#ifdef JP
			put_str("�� :", 10, col+13);
#else
			put_str("Round:", 10, col+13);
#endif


			/* Indicate the state */
#ifdef JP
			put_str("(ESC�Œ�~)", 12, col+13);
#else
			put_str("(Hit ESC to stop)", 12, col+13);
#endif
		}

		/* Otherwise just get a character */
		else
		{
			/* Get a new character */
			get_stats();

			/* Roll for age/height/weight */
			get_ahw();

			/* Roll for social class */
			get_history();
		}

		/* Feedback */
		if (autoroller)
		{
			/* Label */
#ifdef JP
			put_str("�ŏ��l", 2, col+5);
#else
			put_str(" Limit", 2, col+5);
#endif


			/* Label */
#ifdef JP
			put_str("������", 2, col+13);
#else
			put_str("  Freq", 2, col+13);
#endif


			/* Label */
#ifdef JP
			put_str("���ݒl", 2, col+24);
#else
			put_str("  Roll", 2, col+24);
#endif


			/* Put the minimal stats */
			for (i = 0; i < 6; i++)
			{
				int j, m;

				/* Label stats */
				put_str(stat_names[i], 3+i, col);

				/* Race/Class bonus */
				j = rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i];

				/* Obtain the current stat */
				m = adjust_stat(stat_limit[i], j);

				/* Put the stat */
				cnv_stat(m, buf);
				c_put_str(TERM_L_BLUE, buf, 3+i, col+5);
			}
		}

		/* Auto-roll */
		while (autoroller || autochara)
		{
			bool accept = TRUE;

			/* Get a new character */
			get_stats();

			/* Advance the round */
			auto_round++;

			/* Hack -- Prevent overflow */
			if (auto_round >= 1000000000L)
			{
				auto_round = 1;

				if (autoroller)
				{
					for (i = 0; i < 6; i++)
					{
						stat_match[i] = 0;
					}
				}
			}

			if (autoroller)
			{
				/* Check and count acceptable stats */
				for (i = 0; i < 6; i++)
				{
					/* This stat is okay */
					if (p_ptr->stat_max[i] >= stat_limit[i])
					{
						stat_match[i]++;
					}

					/* This stat is not okay */
					else
					{
						accept = FALSE;
					}
				}
			}

			/* Break if "happy" */
			if (accept)
			{
				/* Roll for age/height/weight */
				get_ahw();

				/* Roll for social class */
				get_history();
				///del131221 �̊i�I�[�g���[������
				/*
				if (autochara)
				{
					if ((p_ptr->age < chara_limit.agemin) || (p_ptr->age > chara_limit.agemax)) accept = FALSE;
					if ((p_ptr->ht < chara_limit.htmin) || (p_ptr->ht > chara_limit.htmax)) accept = FALSE;
					if ((p_ptr->wt < chara_limit.wtmin) || (p_ptr->wt > chara_limit.wtmax)) accept = FALSE;
					if ((p_ptr->sc < chara_limit.scmin) || (p_ptr->sc > chara_limit.scmax)) accept = FALSE;
				}
				*/
				if (accept) break;
			}

			/* Take note every x rolls */
			flag = (!(auto_round % AUTOROLLER_STEP));

			/* Update display occasionally */
			if (flag)
			{
				/* Dump data */
				birth_put_stats();

				/* Dump round */
				put_str(format("%10ld", auto_round), 10, col+20);

#ifdef AUTOROLLER_DELAY
				/* Delay 1/10 second */
				if (flag) Term_xtra(TERM_XTRA_DELAY, 10);
#endif

				/* Make sure they see everything */
				Term_fresh();

				/* Do not wait for a key */
				inkey_scan = TRUE;

				/* Check for a keypress */
				if (inkey())
				{
					/* Roll for age/height/weight */
					get_ahw();

					/* Roll for social class */
					///sysdel
					get_history();

					break;
				}
			}
		}

		if (autoroller || autochara) sound(SOUND_LEVEL);

		/* Flush input */
		flush();


		/*** Display ***/

		/* Mode */
		mode = 0;

		/* Roll for base hitpoints */
		get_extra(TRUE);

		/* Roll for gold */
		get_money();

		/* Hack -- get a chaos patron even if you are not a chaos warrior */
		p_ptr->chaos_patron = (s16b)randint0(MAX_PATRON);

		/* Input loop */
		while (TRUE)
		{
			/* Calculate the bonuses and hitpoints */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Update stuff */
			update_stuff();

			/* Fully healed */
			p_ptr->chp = p_ptr->mhp;

			/* Fully rested */
			p_ptr->csp = p_ptr->msp;

			/* Display the player */
			display_player(mode);

			/* Prepare a prompt (must squeeze everything in) */
			Term_gotoxy(2, 23);
			Term_addch(TERM_WHITE, b1);
#ifdef JP
			Term_addstr(-1, TERM_WHITE, "'r' ���̐��l");
#else
			Term_addstr(-1, TERM_WHITE, "'r'eroll");
#endif

#ifdef JP
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p' �O�̐��l");
#else
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p'previous");
#endif

#ifdef JP
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' ���̑��̏��");
#else
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' Misc.");
#endif

#ifdef JP
			else Term_addstr(-1, TERM_WHITE, ", 'h' ����������\��");
#else
			else Term_addstr(-1, TERM_WHITE, ", 'h'istory");
#endif

#ifdef JP
			Term_addstr(-1, TERM_WHITE, ", Enter ���̐��l�Ɍ���");
#else
			Term_addstr(-1, TERM_WHITE, ", or Enter to accept");
#endif

			Term_addch(TERM_WHITE, b2);

			/* Prompt and get a command */
			c = inkey();

			/* Quit */
			if (c == 'Q') birth_quit();

			/* Start over */
			if (c == 'S') return (FALSE);

			/* Escape accepts the roll */
			if (c == '\r' || c == '\n' || c == ESCAPE) break;

			/* Reroll this character */
			if ((c == ' ') || (c == 'r')) break;

			/* Previous character */
			if (prev && (c == 'p'))
			{
				load_prev_data(TRUE);
				continue;
			}

			/* Toggle the display */
			if ((c == 'H') || (c == 'h'))
			{
				mode = ((mode != 0) ? 0 : 1);
				continue;
			}

			/* Help */
			if (c == '?')
			{
#ifdef JP
				show_help("jbirth.txt#AutoRoller");
#else
				show_help("birth.txt#AutoRoller");
#endif
				continue;
			}
			else if (c == '=')
			{
				screen_save();
#ifdef JP
				do_cmd_options_aux(OPT_PAGE_BIRTH, "�����I�v�V����((*)�̓X�R�A�ɉe��)");
#else
				do_cmd_options_aux(OPT_PAGE_BIRTH, "Birth Option((*)s effect score)");
#endif

				screen_load();
				continue;
			}

			/* Warning */
#ifdef JP
			bell();
#else
			bell();
#endif

		}

		/* Are we done? */
		if (c == '\r' || c == '\n' || c == ESCAPE) break;

		/* Save this for the "previous" character */
		save_prev_data(&previous_char);
		previous_char.quick_ok = FALSE;

		/* Note that a previous roll exists */
		prev = TRUE;
	}
#endif
	///mod140510 �{�[�i�X�|�C���g���ɂ���
	while(1)
	{
		bool ok = FALSE;
		get_bonus_points();
		init_turn();
		get_ahw();
		get_history();
		get_extra(TRUE);
		get_money();
		//��ǐ_�l���ꏈ���@�ŏ��̐_�i�^�C�v�����肵�J�I�X�p�g�����ϐ��֊i�[���� deity_table[]��3�`14
		//�J�I�X�p�g�����ϐ����g���̂̓N�C�b�N�X�^�[�g���ɕێ����邽��
		if(p_ptr->prace == RACE_STRAYGOD)
		{
			if(p_ptr->pclass == CLASS_SHIZUHA) p_ptr->chaos_patron = 2; 
			else if(p_ptr->pclass == CLASS_MINORIKO) p_ptr->chaos_patron = 1;
			else if (p_ptr->pclass == CLASS_EIKA) p_ptr->chaos_patron = 79;
			else p_ptr->chaos_patron = 2 + (s16b)randint1(12);

			p_ptr->race_multipur_val[3] = p_ptr->chaos_patron;
		}
		else
			p_ptr->chaos_patron = (s16b)randint0(MAX_PATRON);

		//v1.1.70 �푰�u������v�̏������͂����߂�
		if (p_ptr->prace == RACE_ANIMAL_GHOST)
		{
			get_animal_ghost_family(TRUE);
		}


		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA);
		update_stuff();
		p_ptr->chp = p_ptr->mhp;
		p_ptr->csp = p_ptr->msp;
		display_player(0);
		Term_gotoxy(2, 23);
		Term_addstr(-1, TERM_WHITE, "'r' �{�[�i�X�|�C���g���ă��[��");
		Term_addstr(-1, TERM_WHITE, ", Enter ���̐��l�Ɍ���");
		while(1)
		{
			c = inkey();
			if (c == 'Q') birth_quit();
			if (c == 'S') return (FALSE);
			if (c == '\r' || c == '\n' || c == ESCAPE)
			{
				ok = TRUE;
				break;
			}
			if ((c == 'r')) break;
		}
		if(ok) break;
		put_str("                                                       ", 3, 30);
		put_str("                                                       ", 4, 30);
		put_str("                                                       ", 5, 30);
		put_str("                                                       ", 6, 30);
		put_str("                                                       ", 7, 30);
		put_str("                                                       ", 8, 30);

	}
	///mod150315 �d������\�͌���(�ŏ�����L�������C�N�������p)
	if(p_ptr->prace == RACE_FAIRY) get_fairy_type(TRUE);



	/* Clear prompt */
	clear_from(23);

	/* Get a name, recolor it, prepare savefile */

	if(!flag_unique) get_name();


	/* Process the player name */
	process_player_name(creating_savefile);

	/*** Edit character background ***/
	edit_history();

	/*** Finish up ***/

	get_max_stats();


	///del140105 virtue
	//get_virtues();

	/* Prompt for it */
#ifdef JP
	prt("[ 'Q' ���f, 'S' ���߂���, Enter �Q�[���J�n ]", 23, 14);
#else
	prt("['Q'uit, 'S'tart over, or Enter to continue]", 23, 10);
#endif


	/* Get a key */
	c = inkey();

	/* Quit */
	if (c == 'Q') birth_quit();

	/* Start over */
	if (c == 'S') return (FALSE);


	/* Initialize random quests */
	init_dungeon_quests();

	/* Save character data for quick start */
	save_prev_data(&previous_char);
	previous_char.quick_ok = TRUE;

	/* Accept */
	return (TRUE);
}


/*
 * Ask whether the player use Quick Start or not.
 */
/*:::�N�C�b�N�X�^�[�g�̊m�F�@���s�Ȃ�f�[�^���[�h�@�N�C�b�N�X�^�[�g���Ȃ��Ȃ�FALSE��Ԃ�*/
static bool ask_quick_start(void)
{
	/* Doesn't have previous data */
	if (!previous_char.quick_ok) return FALSE;


	/* Clear screen */
	Term_clear();

	/* Extra info */
#ifdef JP
	put_str("�N�C�b�N�E�X�^�[�g���g���ƈȑO�ƑS�������L�����N�^�[�Ŏn�߂��܂��B", 11, 5);
#else
	put_str("Do you want to use the quick start function(same character as your last one).", 11, 2);
#endif

	/* Choose */
	while (1)
	{
		char c;

#ifdef JP
		put_str("�N�C�b�N�E�X�^�[�g���g���܂����H[y/N]", 14, 10);
#else
		put_str("Use quick start? [y/N]", 14, 10);
#endif
		c = inkey();

		if (c == 'Q') quit(NULL);
		else if (c == 'S') return (FALSE);
		else if (c == '?')
		{
#ifdef JP
			show_help("tbirth.txt#QuickStart");
#else
			show_help("birth.txt#QuickStart");
#endif
		}
		else if ((c == 'y') || (c == 'Y'))
		{
			/* Yes */
			break;
		}
		else
		{
			/* No */
			return FALSE;
		}
	}

	/*:::�N�C�b�N�X�^�[�g�p�ɐF�X�ȃf�[�^��ǂݍ���*/
	load_prev_data(FALSE);

	//v1.1.30 init_dungeon_quests()��init_turn()���t�ɂ���B�������Ȃ��Ƒ啝�ɓ����o�߂����Z�[�u�f�[�^�ŃN�C�b�N�X�^�[�g�����Ƃ��Ƀ����N�G�����X�^�[�ɋ��G���o������炵��
	/*:::�푰���ƂɃ^�[���v�Z*/
	init_turn();
	/*:::�N�G�X�g������*/
	init_dungeon_quests();

	sp_ptr = &sex_info[p_ptr->psex];
	rp_ptr = &race_info[p_ptr->prace];
	cp_ptr = &class_info[p_ptr->pclass];
	//mp_ptr = &m_info[p_ptr->pclass];
	//v1.1.32 ���ꐫ�i����
	//ap_ptr = &seikaku_info[p_ptr->pseikaku];
	ap_ptr = get_ap_ptr(p_ptr->prace,p_ptr->pclass,p_ptr->pseikaku);

	/* Calc hitdie, but don't roll */
	get_extra(FALSE);

	/* Calculate the bonuses and hitpoints */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Update stuff */
	update_stuff();

	/* Fully healed */
	p_ptr->chp = p_ptr->mhp;

	/* Fully rested */
	p_ptr->csp = p_ptr->msp;

	/* Process the player name */
	process_player_name(FALSE);

	return TRUE;
}



/*:::�N���X�Œ�̉i�v�ψق𓾂鏈���B�Q�[���J�n���Ǝ푰�ϗe�̂Ƃ��g����B*/
// �V���ɓ��������ψق����ݎ����Ă���ψقƑ�������Ƃ����̕ψق������Ȃ��Ƃ����Ȃ��B�@
// �ʓ|������푰�ϗe�������x�S���̕ψق�������悤�ɂ��Ă��܂������H
// ���Ƃ肠�����푰�ψق̎��͕ψق��S�������邱�Ƃɂ����B�v���o���Ȃ��������ψق��d�����Ȃ����Ƃ�O��ɃR�[�h�������������ǂ����ɂ������C������B
void gain_perma_mutation(void)
{
	p_ptr->muta1_perma = 0;
	p_ptr->muta2_perma = 0;
	p_ptr->muta3_perma &= MUT3_GOOD_LUCK ; //Hack ���@�X��V�œ��������I�[���͓]���Ƃ����Ă������Ȃ�
	p_ptr->muta4_perma = 0;


	if(p_ptr->pseikaku == SEIKAKU_LUCKY) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if (p_ptr->prace == RACE_ONI)
	{
		if(p_ptr->pclass == CLASS_ZANMU)
			p_ptr->muta2_perma |= MUT2_HORNS;
		else
			p_ptr->muta2_perma |= MUT2_BIGHORN;
	}

	if(p_ptr->prace == RACE_NINGYO) p_ptr->muta3_perma |= MUT3_FISH_TAIL;
	if(p_ptr->pclass == CLASS_TEWI) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if(p_ptr->pclass == CLASS_SUIKA) p_ptr->muta2_perma |= MUT2_ALCOHOL;
	if(p_ptr->pclass == CLASS_KOISHI) p_ptr->muta2_perma |= (MUT2_RTELEPORT | MUT2_BIRDBRAIN);
	if(p_ptr->pclass == CLASS_YOSHIKA) p_ptr->muta3_perma |= MUT3_MORONIC;
	if(p_ptr->pclass == CLASS_PATCHOULI && p_ptr->magic_num2[0] != 1) p_ptr->muta2_perma |= MUT2_ASTHMA;
	if(p_ptr->pclass == CLASS_YUGI) p_ptr->muta3_perma |= MUT3_HYPER_STR;
	if (p_ptr->pclass == CLASS_EIKA) p_ptr->muta3_perma |= MUT3_PUNY;

	if(p_ptr->pclass == CLASS_KYOUKO) p_ptr->muta1_perma |= MUT1_BEAST_EAR;
	if(p_ptr->pclass == CLASS_KEINE) p_ptr->muta2_perma |= MUT2_HARDHEAD;
	if(p_ptr->pclass == CLASS_REMY) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if(p_ptr->pclass == CLASS_UTSUHO) p_ptr->muta2_perma |= (MUT2_BIRDBRAIN);
	if(p_ptr->pclass == CLASS_LUNAR) p_ptr->muta2_perma |= (MUT2_DISARM);
	if(p_ptr->pclass == CLASS_3_FAIRIES && !is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES)) p_ptr->muta2_perma |= (MUT2_DISARM);
	if(p_ptr->pclass == CLASS_YUKARI) p_ptr->muta3_perma |= MUT3_HYPER_INT;
	if(p_ptr->pclass == CLASS_EIRIN) p_ptr->muta3_perma |= MUT3_HYPER_INT;
	if(p_ptr->pclass == CLASS_TOYOHIME) p_ptr->muta3_perma |= MUT3_GOOD_LUCK;
	if(p_ptr->pclass == CLASS_AUNN) p_ptr->muta2_perma |= MUT2_BIGHORN;

	if (p_ptr->pclass == CLASS_SHION) p_ptr->muta3_perma |= MUT3_BAD_LUCK;

	if (p_ptr->pclass == CLASS_KUTAKA) p_ptr->muta2_perma |= (MUT2_BIRDBRAIN);

	if(p_ptr->pclass == CLASS_URUMI) p_ptr->muta2_perma |= MUT2_BIGHORN;
	if (p_ptr->pclass == CLASS_BITEN) p_ptr->muta2_perma |= MUT2_TAIL;

	if (p_ptr->pclass == CLASS_SAKI)p_ptr->muta3_perma |= MUT3_SPEEDSTER | MUT3_WINGS;

	if (p_ptr->pclass == CLASS_YACHIE)	p_ptr->muta2_perma |= MUT2_BIGHORN | MUT2_BIGTAIL;

	if (p_ptr->pclass == CLASS_MOMOYO) p_ptr->muta1_perma |= MUT1_EAT_ROCK;

	if (p_ptr->pclass == CLASS_CHIYARI)	p_ptr->muta2_perma |= MUT2_BIGHORN | MUT2_BIGTAIL;

	if (p_ptr->pclass == CLASS_YUMA)
	{
		p_ptr->muta1_perma |= (MUT1_EAT_ROCK | MUT1_EAT_MAGIC);
		p_ptr->muta2_perma |= MUT2_BIGHORN;
	}


	p_ptr->muta1 = p_ptr->muta1_perma;
	p_ptr->muta2 = p_ptr->muta2_perma;
	p_ptr->muta3 = p_ptr->muta3_perma;

	p_ptr->muta4 = p_ptr->muta4_perma;


}

/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
/*:::�L�����N�^�[���C�N*/
void player_birth(void)
{
	int i, j;
	char buf[80];

	playtime = 0;

	//���i�R���N�^�[�p�̃t���O�@����ȃt���O���Ȃ��Ă������̂ǂꂩ�̃t���O�łǂ��ɂ��Ȃ�̂�������Ȃ������ׂĂȂ�
	if(p_ptr->noscore) old_data_cheat_flag = TRUE;
	if(p_ptr->pseikaku == SEIKAKU_COLLECTOR) old_collector = TRUE;

	/* 
	 * Wipe monsters in old dungeon
	 * This wipe destroys value of m_list[].cur_num .
	 */
	wipe_m_list();

	/* Wipe the player */
	player_wipe();

	/* Create a new character */

	/* Quick start? */
	/*:::�N�C�b�N�X�^�[�g�̊m�F�BY�Ȃ�O��f�[�^�����[�h���A�����łȂ��Ȃ�player_birth_aux()�����[�v*/
	if (!ask_quick_start())
	{
		//v1.1.58
		init_music_hack();
		play_music(TERM_XTRA_MUSIC_BASIC, MUSIC_BASIC_NEW_GAME);

		/* No, normal start */
		while (1)
		{
			/* Roll up a new character */
			if (player_birth_aux()) break;

			/* Wipe the player */
			player_wipe();
		}
	}

	//�S�̊p�⃉�b�L�[�}���̃I�[���Ȃǐ����^�ψق𓾂�
	gain_perma_mutation();

	//v2.0.14 �����Ɉꕔ�̐E�Ƃ̏����ݒ菈������������process_player()��hack_startup�̂Ƃ���ɂ܂Ƃ߂�


	/* Note player birth in the message recall */
	message_add(" ");
	message_add("  ");
	message_add("====================");
	message_add(" ");
	message_add("  ");

	///system ���j�[�N�v���C���[�����������̋L�q�ς��悤
#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- �V�K�Q�[���J�n --------");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- Start New Game --------");
#endif
	do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

#ifdef JP
	sprintf(buf,"                            ���ʂ�%s��I�������B", sex_info[p_ptr->psex].title);
#else
	sprintf(buf,"                            choose %s personality.", sex_info[p_ptr->psex].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            �푰��%s��I�������B", race_info[p_ptr->prace].title);
#else
	sprintf(buf,"                            choose %s race.", race_info[p_ptr->prace].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            �E�Ƃ�%s��I�������B", class_info[p_ptr->pclass].title);
#else
	sprintf(buf,"                            choose %s class.", class_info[p_ptr->pclass].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	if (p_ptr->realm1)
	{
#ifdef JP
		sprintf(buf,"                            ���@�̗̈��%s%s��I�������B",realm_names[p_ptr->realm1], p_ptr->realm2 ? format("��%s",realm_names[p_ptr->realm2]) : "");
#else
		sprintf(buf,"                            choose %s%s realm.",realm_names[p_ptr->realm1], p_ptr->realm2 ? format(" realm and %s",realm_names[p_ptr->realm2]) : "");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);
	}

#ifdef JP
//	sprintf(buf,"                            ���i��%s��I�������B", seikaku_info[p_ptr->pseikaku].title);
	if(p_ptr->psex == SEX_MALE)
		sprintf(buf,"                            ���i��%s��I�������B", ap_ptr->title);
	else
		sprintf(buf,"                            ���i��%s��I�������B", ap_ptr->f_title);
#else
	sprintf(buf,"                            choose %s.", seikaku_info[p_ptr->pseikaku].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	/* Init the shops */
	for (i = 1; i < max_towns; i++)
	{
		for (j = 0; j < MAX_STORES; j++)
		{
			//���i�u�N�W�Ɓv�́A�O��̃Z�[�u�f�[�^�̔����ق��N���A���Ȃ�
			if(p_ptr->pseikaku == SEIKAKU_COLLECTOR && j == STORE_MUSEUM)
				continue;

			/* Initialize */
			store_init(i, j);
		}
	}

	/* Generate the random seeds for the wilderness */
	seed_wilderness();

	/* Give beastman a mutation at character birth */
	///sysdel race
	//if (p_ptr->prace == RACE_BEASTMAN) hack_startup = TRUE;
	//else 
	hack_startup = TRUE; //hack_mutation��S�Ắ��̊J�n�������t���O�Ƃ��č��Ȃ�����

	//v1.1.25 �J�n���o�[�W�������L�^����
	start_ver[0] = H_VER_MAJOR;
	start_ver[1] = H_VER_MINOR;
	start_ver[2] = H_VER_PATCH;
	start_ver[3] = H_VER_EXTRA;

	/* Set the message window flag as default */
	if (!window_flag[1])
		window_flag[1] |= PW_MESSAGE;

	/* Set the inv/equip window flag as default */
	if (!window_flag[2])
		window_flag[2] |= PW_INVEN;
}

/*:::�푰�A�E�ƁA���i�A�̈�̐��������t�@�C���ɃR�s�[����*/
void dump_yourself(FILE *fff)
{
	char temp[80*10];
	int i;
	cptr t;

	if (!fff) return;

	roff_to_buf(race_jouhou[p_ptr->prace], 78, temp, sizeof(temp));
	fprintf(fff, "\n\n");
#ifdef JP
	fprintf(fff, "�푰: %s(%s)\n", race_info[p_ptr->prace].title, race_info[p_ptr->prace].E_title);
#else
	fprintf(fff, "Race: %s\n", race_info[p_ptr->prace].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}

	///mod150315 �d�����Z
	if(p_ptr->prace == RACE_FAIRY && CHECK_FAIRY_TYPE)
		fprintf(fff, "(%s)\n",fairy_table[CHECK_FAIRY_TYPE].fairy_desc);


	///mod140206 ���j�[�N�v���[���[��class_jouhou[]�ɐ��������Ȃ��̂ŏ�����������
	if(cp_ptr->flag_only_unique)
	{
		for(i=0;i<UNIQUE_PLAYER_NUM;i++)
		{
			if(unique_player_table[i].class_id == p_ptr->pclass) break;
		}
		if(i==UNIQUE_PLAYER_NUM)
		{
			msg_print("ERROR:���Ȃ��̃N���XID��unique_player_table[]�ɓo�^����ĂȂ�");
			return;
		}
		roff_to_buf(unique_player_table[i].info, 78, temp, sizeof(temp));		
	}
	else 
		roff_to_buf(class_jouhou[p_ptr->pclass], 78, temp, sizeof(temp));

	fprintf(fff, "\n");
#ifdef JP
	if(p_ptr->psex == SEX_MALE) fprintf(fff, "�E��: %s(%s)\n", class_info[p_ptr->pclass].title,class_info[p_ptr->pclass].E_title);
	else fprintf(fff, "�E��: %s(%s)\n", class_info[p_ptr->pclass].f_title,class_info[p_ptr->pclass].E_title);
#else
	fprintf(fff, "Class: %s\n", class_info[p_ptr->pclass].title);
#endif
	t = temp;

	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	if(p_ptr->pseikaku == SEIKAKU_SPECIAL)
		roff_to_buf(seikaku_special_jouhou[get_special_seikaku_index(p_ptr->pclass)], 74, temp, sizeof(temp));
	else
		roff_to_buf(seikaku_jouhou[p_ptr->pseikaku], 78, temp, sizeof(temp));

	fprintf(fff, "\n");
#ifdef JP
		//if(p_ptr->psex == SEX_MALE) fprintf(fff, "���i: %s\n", seikaku_info[p_ptr->pseikaku].title);
		//else fprintf(fff, "���i: %s\n", seikaku_info[p_ptr->pseikaku].f_title);
		if(p_ptr->psex == SEX_MALE) fprintf(fff, "���i: %s\n", ap_ptr->title);
		else fprintf(fff, "���i: %s\n", ap_ptr->f_title);
#else
	fprintf(fff, "Pesonality: %s\n", seikaku_info[p_ptr->pseikaku].title);
#endif
	t = temp;
	for (i = 0; i < 6; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	fprintf(fff, "\n");
	if (p_ptr->realm1)
	{
		roff_to_buf(realm_jouhou[p_ptr->realm1], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "���@: %s\n", realm_names[p_ptr->realm1]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[p_ptr->realm1]);
#endif
		t = temp;
		for (i = 0; i < 6; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
	fprintf(fff, "\n");
	if (p_ptr->realm2)
	{
		roff_to_buf(realm_jouhou[p_ptr->realm2], 78, temp, sizeof(temp));
#ifdef JP
		fprintf(fff, "���@: %s\n", realm_names[p_ptr->realm2]);
#else
		fprintf(fff, "Realm: %s\n", realm_names[p_ptr->realm2]);
#endif
		t = temp;
		for (i = 0; i < 6; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
}


//v1.1.32 ���ꐫ�i�����֘A
//���i��SEIKAKU_SPECIAL�̂Ƃ��A�L�����ɉ��������iinfo��special_seikaku_info[]���玝���Ă���.
//����ȊO�̐��i�̂Ƃ��͕��ʂ�ap_ptr��Ԃ�
const player_seikaku * get_ap_ptr(int race_idx, int class_idx, int seikaku_idx)
{

	if(seikaku_idx == SEIKAKU_SPECIAL)
	{
		int idx2 = get_special_seikaku_index(class_idx);
		if(idx2 >= MAX_SEIKAKU_SPECIAL)
		{
			msg_format("ERROR:���ꐫ�iidx��MAX����");

		}
		else
			return (const player_seikaku *) &seikaku_info_special[idx2];
	}
	
	return (const player_seikaku *) &seikaku_info[seikaku_idx];
}


//���݂́������ꐫ�iidx�Ɉ�v���Ă��邩���肷��B
//�L�������C�N�⃍�[�h����pseikaku��pclass���m�肵�Ă��Ȃ���ԂŎg��Ȃ��悤����
bool is_special_seikaku(int idx)
{
	if (p_ptr->pseikaku != SEIKAKU_SPECIAL)
		return FALSE;

	if (get_special_seikaku_index(p_ptr->pclass) == idx)
		return TRUE;

	return FALSE;

}
