#include "angband.h"

/*
 *cmd_yuma.c
 *v2.0.6
 *����b�W�ޖ��֌W�̏���
 */


 //yuma_gain_itemflag()�̃t���O�L�^���������؂蕪����B����ȊO�̃A�C�e����H�ׂ��Ƃ��ɂ��ʂɎw�肷��΃X�L����ϐ��𓾂���悤��
 //s_idx��TR_***�̒l�ƈ�v
 //������V����������TRUE
bool yuma_gain_itemflag_aux(int s_idx)
{
	bool flag_new = FALSE;

	if (p_ptr->pclass != CLASS_YUMA || s_idx < 0 || s_idx >= TR_FLAG_MAX)
	{
		msg_format("ERROR:yuma_gain_itemflag_aux(%d)", s_idx);
		return FALSE;
	}

	//�X���C�═�푮���Ȃǂ͏��O
	if (IS_WEAPON_FLAG(s_idx)) return FALSE;
	//���͎ˌ��Ȃǂ͏��O
	if (IS_BOW_FLAG(s_idx)) return FALSE;
	//�􂢂┽�e���|�Ȃǂ͏��O
	if (IS_BAD_FLAG(s_idx)) return FALSE;
	//���f�ی��SHOW_MODS�ȂǓ���t���O�͏��O
	if (IS_SUB_FLAG(s_idx)) return FALSE;

	if (!p_ptr->magic_num2[s_idx])
	{
		flag_new = TRUE;
		msg_format("%s�̓����𓾂��I", equipment_tr_flag_names[s_idx]);
	}

	p_ptr->magic_num2[s_idx] = YUMA_FLAG_DELETE_TURN;

	return flag_new;

}


 //�ޖ����u�H�ׂ�v�R�}���h���u�z�����ށv���Z�ő����i��H�ׂ��Ƃ����̃A�C�e�������t���O�l���g����悤�ɂȂ�
 //p_ptr->magic_num2[idx](idx��TR_***�Ƌ���)��50��ݒ肷��B
 //�����1000turn����1���炵0�ɂȂ�����g���Ȃ��Ȃ�B�Q�[�����Ŗ񔼓��B
 //���炩�̃t���O��V���ɓ�����TRUE��Ԃ��B
bool	yuma_gain_itemflag(object_type *o_ptr)
{

	u32b flgs[TR_FLAG_SIZE];
	int i, j, idx;
	bool flag_gain = FALSE;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_gain_itemflag()"); return FALSE; }

	object_flags(o_ptr, flgs);

	//�A�C�e������TR_***�̃t���O�����邩�ǂ���������ׁA��������yuma_gain_itemflag_aux()�ɓn����magic_num2[]�ɋL�^���Ă���
	for (i = 0; i<TR_FLAG_SIZE; i++)
	{
		for (j = 0; j<32; j++)
		{
			int s_idx = 32 * i + j;

			if (s_idx >= TR_FLAG_MAX) break;

			if ((1L << j) & flgs[i])
			{
				if (yuma_gain_itemflag_aux(s_idx)) flag_gain = TRUE;
			}
		}
	}

	if (flag_gain) p_ptr->update |= PU_BONUS;

	return flag_gain;


}




 //yuma_gain_monspell()�̃t���O�L�^�������؂蕪����B
 //s_idx��monspell_list2[]�̃C���f�b�N�X�ƈ�v
 //������V����������TRUE
bool yuma_gain_monspell_aux(int s_idx)
{
	int i, j, idx;
	bool flag_gain = FALSE;

	if (p_ptr->pclass != CLASS_YUMA || s_idx <= 0 || s_idx > MAX_MONSPELLS2)
	{
		msg_print("ERROR:yuma_gain_monspell_aux()");
		return FALSE;
	}

	//�u���ꂽ���t�̒ǉ��v���K������܂œ��Z�𓾂��Ȃ�
	if (p_ptr->lev < 30) return FALSE;

	//���������Z�Ɠ��ʍs�������O
	if (!monspell_list2[s_idx].level) return FALSE;

	//���x���̑���Ȃ����Z�͏K���ł��Ȃ��H�ǂ����g���Ȃ��̂Ŗ��Ȃ���
	//if (monspell_list2[s_idx].level > p_ptr->lev) return;

	if (!p_ptr->magic_num1[s_idx])
	{
		flag_gain = TRUE;
		msg_format("%s�̔\�͂��z�������I", monspell_list2[s_idx].name);

	}

	p_ptr->magic_num1[s_idx] = YUMA_FLAG_DELETE_TURN;

	return flag_gain;

}


 //�ޖ����u�z�����ށv�R�}���h�Ń����X�^�[��|�����Ƃ����̃����X�^�[�������Z���g����悤�ɂȂ�
 //p_ptr->magic_num1[idx](idx��monspell_list2[]�Ƌ���)��50��ݒ肷��B
 //�����1000turn����1���炵0�ɂȂ�����g���Ȃ��Ȃ�B�Q�[�����Ŗ񔼓��B
 //���炩�̋Z��V���ɓ�����TRUE��Ԃ��B
bool	yuma_gain_monspell(monster_race *r_ptr)
{

	int i, j, s_idx;

	bool flag_gain = FALSE;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_gain_monspell()"); return FALSE; }

	for (i = 0; i<4; i++)
	{
		u32b flgs;

		//r_info�̏��L���Z�t���O��monspell_list2[]�C���f�b�N�X�ɕϊ�
		if (i == 0)
			flgs = r_ptr->flags4;
		else if (i == 1)
			flgs = r_ptr->flags5;
		else if (i == 2)
			flgs = r_ptr->flags6;
		else
			flgs = r_ptr->flags9;

		for (j = 0; j<32; j++)
		{

			s_idx = 1 + 32 * i + j;

			if (s_idx > MAX_MONSPELLS2) break;

			//���̓��Z�������Ă�����v���C���[�̎g�p�\���@���X�g(p_ptr->magic_num1[idx])�ɋL�^
			if ((1L << j) & flgs)
			{
				if (yuma_gain_monspell_aux(s_idx)) flag_gain = TRUE;

			}
		}
	}

	return flag_gain;

}



//�ޖ��������X�^�[�═��瓾���p���[�����Ԍo�߂ɂ�莸����
//1000turn���ƂɎ��s��1���󂯎���ē����J�E���g�l���猸�炷�B�h���ȂǂŎ��Ԍo�߂����ꍇ�͌o��turn/1000���󂯎��
void	yuma_lose_extra_power(int dec_val)
{

	int i;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_lose_extra_power()"); return; }

	if (cheat_xtra) msg_format("yuma_lose_%d", dec_val);

	//�����X�^�[���瓾���X�L��
	for (i = 0; i <= MAX_MONSPELLS2; i++)
	{
		if (p_ptr->magic_num1[i] > 0)
		{
			if (dec_val > p_ptr->magic_num1[i]) dec_val = p_ptr->magic_num1[i];
			p_ptr->magic_num1[i] -= dec_val;

			if (p_ptr->magic_num1[i] == 0)
			{
				msg_format("%s�̔\�͂�����ꂽ�B", monspell_list2[i].name);

			}
		}
	}
	//����瓾�������i�t���O
	for (i = 0; i<TR_FLAG_MAX; i++)
	{
		if (p_ptr->magic_num2[i] > 0)
		{
			if (dec_val > p_ptr->magic_num2[i]) dec_val = p_ptr->magic_num2[i];
			p_ptr->magic_num2[i] -= dec_val;

			if (p_ptr->magic_num2[i] == 0)
			{
				msg_format("%s�̔\�͂�����ꂽ�B", equipment_tr_flag_names[i]);
				p_ptr->update |= (PU_BONUS | PU_LITE);
			}
		}
	}

}


//�ޖ����ʏ�E�R�}���h�ŐH�ׂȂ��悤�ȕ���H�ׂ�Ƃ��̏��� do_cmd_eat_food_aux()����Ă΂��
//TV_FOOD,TV_MUSHROOM,TV_SWEETS,TV_MATERIAL��do_cmd_eat_food_aux()�ŏ������ς�ł���
//TV_POTION,TV_COMPOUND,TV_ALCOHOL,TV_SOFTDRINK�͂����łȂ�quaff_potion_aux()�ɓn�����
//���̐H���œ����閞���x��Ԃ�
//0���Ԃ��ꂽ�Ƃ��H�ׂ�̂Ɏ��s�����Ƃ݂Ȃ��O�ł��A�C�e�������炳�������I������
int yuma_eat_other_things(object_type *o_ptr)
{
	int food_val = 0;
	int price;

	if (p_ptr->pclass != CLASS_YUMA) { msg_print("ERROR:yuma_eat_other_thing()"); return 0; }

	//�ʏ�̂����܂⃉���^���͔R���̎c��(o_ptr->xtra4)���v�Z�ɓ����
	if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_TORCH && object_is_nameless(o_ptr))
	{
		msg_print("�΂̂����܂܂̏�����ۂݍ��񂾁I");
		food_val = o_ptr->xtra4 + 100;
	}
	else if (o_ptr->tval == TV_LITE && o_ptr->sval == SV_LITE_LANTERN && object_is_nameless(o_ptr))
	{
		msg_print("�����^�������ݍӂ��Ė������񂾁I");
		food_val = o_ptr->xtra4 + 1000;
	}
	//�����i�͉��i�����̂܂ܐH���ɂ��A����ɂ��ꂪ���ϐ��Ȃǂ��ꎞ�I�Ɋl������
	else if (object_is_equipment(o_ptr))
	{
		switch (o_ptr->tval)
		{
		case TV_KNIFE:
		case TV_SWORD:
		case TV_KATANA:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:
			msg_print("�댯�ȕ������i�̂悤�ɓۂݍ��񂾁I"); break;
		case TV_GUN:
			msg_print("�e����ւ��܂��Ĉ��݉������I���̒����甚����������I"); break;
		case TV_RIBBON:
			msg_print("����������z���ꑧ�ɋz�����񂾁I"); break;
		case TV_LITE:
			msg_print("ῂ����鉽�������ɋz�����܂ꂽ�I"); break;
		case TV_CLOTHES:
		case TV_CLOAK:
			msg_print("��v�ȕz�n���p�C�̂悤�ɐH�����������I"); break;
		case TV_ARMOR:
			msg_print("�傫�ȊZ����ۂ݂ɂ����I"); break;
		case TV_DRAG_ARMOR:
			msg_print("���̗؂͒��X�̒������I"); break;
		case TV_BOOTS:
			msg_print("�C�̖�������قǈ����Ȃ����̂��B"); break;
		case TV_RING:
		case TV_AMULET:
			msg_print("�Y��ȑ����i�������َq�̂悤�Ɋ��ݍӂ����I"); break;
		default:
			msg_print("������o���{���H�ׂ��I"); break;

		}

		food_val = object_value_real(o_ptr);
		//����Ă���Ɖh�{�𑝂₷
		if (!food_val)
		{
			food_val = 1;
			if (object_is_cursed(o_ptr))
			{
				msg_print("���̎􂢂͎��ɔ������I");
				food_val += 5000;
				if (!object_is_nameless(o_ptr)) food_val += 15000;
			}
		}
		yuma_gain_itemflag(o_ptr);
	}
	else if (o_ptr->tval == TV_MAGICITEM || o_ptr->tval == TV_MAGICWEAPON)
	{
		msg_print("���������@�����ɂ������Ȃ��H�ׂ��I");
		food_val = MAX(object_value_real(o_ptr), 5000);
		yuma_gain_itemflag(o_ptr);

	}

	//���@���Ɗ��� �Ƃ肠�������i�����̂܂ܐH���ɁB���ʖ��@���͂Ȃɂ����Z���l�����邩�H
	else if (o_ptr->tval <= TV_BOOK_END || o_ptr->tval == TV_SCROLL)
	{
		msg_print("���͂̐��݂������Â�H�����I");

		//���ʖ��@���͌o���l�𓾂��邱�Ƃɂ���
		if (o_ptr->sval > 1) gain_exp(object_value_real(o_ptr)/2);

		food_val = 100;
	}
	//���@�z�ʂƓ��z��H����
	else if (o_ptr->tval == TV_GOLD)
	{
		msg_print("������󂪂��Ȃ��̌��ɋz�����܂ꂽ�I");
		food_val = o_ptr->pval;

	}
	//��E���@�_�E���b�h MP����
	else if (o_ptr->tval == TV_STAFF || o_ptr->tval == TV_WAND || o_ptr->tval == TV_ROD)
	{

		msg_print("���@�̏�������َq�̂悤�Ɋ��ݍӂ����I");
		food_val = object_value_real(o_ptr);

		if (!food_val) food_val = 1;

		if (player_gain_mana(k_info[o_ptr->k_idx].level * (o_ptr->tval == TV_ROD ? 4 : o_ptr->pval / 2)))
			msg_print("������̖��͂��z�������B");

	}
	//�V���Ǝʐ^�@���̃����X�^�[�̏��𓾂�
	else if (o_ptr->tval == TV_PHOTO || o_ptr->tval == TV_BUNBUN || o_ptr->tval == TV_KAKASHI)
	{
		if (o_ptr->pval > 0 && o_ptr->pval < max_r_idx)
		{
			if (lore_do_probe(o_ptr->pval))
				msg_format("%s�̏��𓾂��B", r_name + r_info[o_ptr->pval].name);

		}

		//�h�{�͂Ȃ��B�G���[�����ɂȂ��悤1�����Ԃ�
		food_val = 1;
	}
	else if (o_ptr->tval == TV_CHEST)
	{
		if (!o_ptr->pval)
		{
			msg_print("�󂫔���H�ׂ��B");
			food_val = 100;
		}
		else
		{
			msg_print("�����̓����Ă��锠�𒆐g���ƐH�ׂ��I");
			food_val = 10000;

		}
	}
	//�d���{�Ɛl�`�@���̃����X�^�[�̓��Z���l������
	else if (o_ptr->tval == TV_FIGURINE || o_ptr->tval == TV_CAPTURE)
	{
		food_val = object_value_real(o_ptr);
		if (!food_val) food_val = 5000;
		if (o_ptr->pval > 0 && o_ptr->pval < max_r_idx)
		{
			yuma_gain_monspell(&r_info[o_ptr->pval]);
		}
	}
	else if (o_ptr->tval == TV_ITEMCARD)
	{
		msg_print("�J�[�h�̖��͂��z��������I");

		food_val = support_item_list[o_ptr->pval].cost * 300;
	}
	else if (o_ptr->tval == TV_ABILITY_CARD)
	{
		msg_print("�J�[�h���狭�����͂��������̂����A���񂾓r�[�ɏ����Ă��܂����B");
		food_val = 1;

	}
	//��A���͂����ɓ���O��quaff_potion_aux()�ɓn���B�����Ō��ʔ����ƃA�C�e�������������s����B
	/*
	else if (o_ptr->tval == TV_POTION || o_ptr->tval == TV_COMPOUND || o_ptr->tval == TV_ALCOHOL || o_ptr->tval == TV_SOFTDRINK)
	{
	}
	*/
	//�R���@�ޖ��v���C���Ɏ�ɓ���͖̂��ق݂̂̂͂�
	else if (o_ptr->tval == TV_FLASK)
	{
		msg_print("���ڂ�r���ƐH�ׂ��I");
		food_val = 5000;
	}
	//���i�@�Ƃ肠�����o���l�ł����Ă����B���邢�͉������Z�l���Ƃ����Ă�������������Ȃ�
	else if (o_ptr->tval == TV_SOUVENIR)
	{
		int exp = calc_korin_sell_price(o_ptr);
		int val = 1000 + object_value_real(o_ptr);

		gain_exp(MAX(exp, 1));
		food_val = val;

	}
	//���ł��H�ׂ�ݒ肾���A*�^�E����*��̃g���t�B�[�I�Ȃ��̂͐H�ׂȂ��Ă��悩�낤
	else if (o_ptr->tval == TV_SPELLCARD || o_ptr->tval == TV_ANTIQUE || o_ptr->tval == TV_ANTIQUE)
	{
		msg_print("�����H�ׂ�ܑ͖̂̂Ȃ��B");
		food_val = 0;
	}

	else
	{
		msg_format("ERROR:yuma_eat_other_things()�ɖ���`�̃A�C�e��(TV:%d SV:%d)���n���ꂽ", o_ptr->tval, o_ptr->sval);
		return 0;
	}

	return food_val;

}




//�ޖ����Z�u�z�����ށv�œ���O���b�h�̃A�C�e�����z������
//rad:������z�����߂�L������
//�����H�ׂ���TRUE
bool yuma_swallow_item(int y, int x, int rad)
{
	bool flag_eat_sth = FALSE;
	int tmp_o_idx;

	//�}�b�v�͈͊O	
	if (!in_bounds(y, x)) return FALSE;
	//�A�C�e�����Ȃ��O���b�h
	if (!cave[y][x].o_idx) return FALSE;
	//����
	if (distance(py, px, y, x) > rad) return FALSE;
	//�ː�
	if (!projectable(py, px, y, x)) return FALSE;

	tmp_o_idx = cave[y][x].o_idx;

	while (tmp_o_idx)
	{

		int item = 0 - tmp_o_idx;
		int next_o_idx = o_list[tmp_o_idx].next_o_idx;

		//eat_food_aux()��o_list[]�̃C���f�b�N�X���}�C�i�X�ɂ������̂�n��
		if (do_cmd_eat_food_aux(item))
		{
			flag_eat_sth = TRUE;
			//�H�ׂĂ��̘g����ɂȂ����ꍇ�X�^�b�N�̎��̃A�C�e����
			if (!o_list[tmp_o_idx].k_idx) tmp_o_idx = next_o_idx;
		}
		else //eat_food_aux()���H�p�s�Ɣ��f���p�X�����Ƃ��̓X�^�b�N�̎��̃A�C�e����
		{
			tmp_o_idx = next_o_idx;
		}

	}

	return flag_eat_sth;

}



 //�ޖ����Z�u�z�����ށv�œ���O���b�h�̃����X�^�[�������񂹂ă_���[�W��^���A�|������\�͂��z�����Ƃ��ǂ����i���ς��
 //rad:������z�����߂�L������
 //�����X�^�[����h�{�𓾂���TRUE��Ԃ�
bool yuma_swallow_mon(int y, int x, int rad)
{
	int m_idx,r_idx;
	int mon_resist;
	monster_type *m_ptr;
	monster_race *r_ptr;
	int dam;
	bool dummy = FALSE;
	char m_name[120];

	//�}�b�v�͈͊O	
	if (!in_bounds(y, x)) return FALSE;

	m_idx = cave[y][x].m_idx;

	//�����X�^�[�����Ȃ��O���b�h
	if (!m_idx) return FALSE;
	//����
	if (distance(py, px, y, x) > rad) return FALSE;
	//�ː�
	if (!projectable(py, px, y, x)) return FALSE;

	dam = p_ptr->lev * 5;
	if (YUMA_ULTIMATE_MODE) dam *= 2;

	m_ptr = &m_list[m_idx];
	r_idx = m_ptr->r_idx;
	r_ptr = &r_info[r_idx];

	//�͂���񖳌�
	if (r_ptr->flagsr & RFR_RES_ALL) return FALSE;

	//�R�惂���X�^�[�͑ΏۊO ���Ɠ����O���b�h�ł͌Ă΂Ȃ����ꉞ
	if (p_ptr->riding == m_idx) return FALSE;

	//�����X�^�[�����͂Ɉ����񂹂� �͋����G�Ƌ���ȓG�ɂ͌����Â炢 ���ł���G�ɂ͌����₷��
	mon_resist = r_ptr->level;
	if (r_ptr->flags2 & (RF2_POWERFUL | RF2_GIGANTIC))mon_resist *= 2;
	if (r_ptr->flags7 & RF7_CAN_FLY) mon_resist /= 2;

	monster_desc(m_name, m_ptr, 0);

	if (mon_resist / 2 + randint0(mon_resist / 2) < p_ptr->lev && m_ptr->cdis > 1)
	{
		msg_format("%s���z���񂹂��I", m_name);
		teleport_monster_to(m_idx, py, px, 100, TELEPORT_FORCE_NEXT);
	}


	//�����񂹌�ɗאڂ��Ă���Ƃ��̂݃_���[�W����
	if (m_ptr->cdis == 1)
	{
		msg_format("%s�̗͂��z�����Ă���I", m_name);
		//�_���[�W��^����B�|�����Ƃ�{}���ցB���̂Ƃ�m_ptr�͂����N���A����Ă���͂��Ȃ̂Œ���
		if (mon_take_hit(m_idx, dam, &dummy, extract_note_dies(real_r_ptr(m_ptr))))
		{

			//�����x�㏸�@���l�͎푰������Ɠ����v�Z�������G�푰����Ȃ�
			int feed = 1000 + r_ptr->level * 50 + r_ptr->level * r_ptr->level;
			set_food(p_ptr->food + feed);

			//���̃����X�^�[�̔\�͂𓾂�
			yuma_gain_monspell(r_ptr);

			//���j�[�N�����X�^�[��|�����Ƃ����܂ɐ��i�ύX
			if (!ap_ptr->flag_strange_select)
			{
				int new_seikaku = p_ptr->pseikaku;
				switch (r_idx)
				{
				case MON_REIMU:
					new_seikaku = SEIKAKU_SHIAWASE; break;//�얲�̂�
				case MON_MARISA:
					new_seikaku = SEIKAKU_FUTUU; break;//�������ɂ߂ĕ���
				case MON_SAKUYA:
					new_seikaku = SEIKAKU_ELEGANT; break;//����n��
				case MON_CIRNO:
					new_seikaku = SEIKAKU_INOCHI; break;//�`���m���Ă��
				case MON_RAIKO:
					new_seikaku = SEIKAKU_KIREMONO; break;//���ۑ���

					//TODO:�ق��N���������H

				default:

					if (!(r_ptr->flags1 & RF1_UNIQUE)) break;
					//�L���m���{�E���[�⃉�b�L�[�K�[���ȂǓ���ȏ������K�v�Ȑ��i�������ă����_���ɑI��
					do
					{
						new_seikaku = randint0(MAX_SEIKAKU);
					} while (seikaku_info[new_seikaku].flag_strange_select);
					break;

				}
				if (new_seikaku != p_ptr->pseikaku)
				{
					msg_print("���i���ς�����C������...");
					p_ptr->pseikaku = new_seikaku;
					ap_ptr = &seikaku_info[new_seikaku];
					p_ptr->update |= PU_BONUS;

				}
			}


		}
		//�|���Ȃ��Ă��h�{�𓾂�
		else
		{
			set_food(p_ptr->food + dam);
		}

		//�h�{�𓾂��Ƃ�TRUE
		return TRUE;

	}

	return FALSE;

}




//yuma_swallow()����Ă΂��
//����O���b�h�̒n�`�����ł����Ė����x�㏸
//rad:������z�����߂�L������
//�Ȃɂ��H�ׂ���TRUE��Ԃ�
bool yuma_swallow_terrain(int y, int x, int rad)
{
	int m_idx;
	feature_type *f_ptr;

	int prob = 0;//�z�����݂ɐ�������m��
	int food = 0;//�z�����񂾂Ƃ��ɓ��閞���x

	//�}�b�v�͈͊O	
	if (!in_bounds(y, x)) return FALSE;

	f_ptr = &f_info[cave[y][x].feat];

	//�i�v�n�`�͏��O
	if (cave_perma_bold(y, x)) return FALSE;

	//����
	if (distance(py, px, y, x) > rad) return FALSE;
	//�ː�
	if (!projectable(py, px, y, x)) return FALSE;

	if (cave_have_flag_bold(y, x, FF_DOOR))//�h�A�E�J�[�e��
	{
		prob = 75;
		food = 1000;
	}
	else if (cave_have_flag_bold(y, x, FF_TREE))//��
	{
		prob = 30;
		food = 3000;
	}
	else if (cave_have_flag_bold(y, x, FF_PLANT))//���n�E���ނ�E��
	{
		prob = 80;
		food = 1000;
	}
	else if (cave_have_flag_bold(y, x, FF_GLYPH))//���̃��[��
	{
		prob = 30;
		food = 5000;
	}
	else if (cave_have_flag_bold(y, x, FF_MINOR_GLYPH))//�������[��
	{
		prob = 70;
		food = 3000;
	}
	else if (cave_have_flag_bold(y, x, FF_LAVA))//�n��
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_fire(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_fire(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_ACID_PUDDLE))//�_
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_acid(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_acid(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_COLD_PUDDLE))//���@����
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_cold(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_cold(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_ELEC_PUDDLE))//���̗͂��_
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_elec(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_elec(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_POISON_PUDDLE))//�ŏ�
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 20;
			food = 5000;
			set_oppose_pois(20, FALSE);
		}
		else
		{
			prob = 5;
			food = 10000;
			set_oppose_pois(50, FALSE);

		}
	}
	else if (cave_have_flag_bold(y, x, FF_WATER))//���E��
	{
		if (cave_have_flag_bold(y, x, FF_SHALLOW))
		{
			prob = 40;
			food = 2000;
		}
		else
		{
			prob = 20;
			food = 4000;

		}
	}
	else if (cave_have_flag_bold(y, x, FF_OIL_FIELD))//�Ζ�
	{
		prob = 35;
		food = 5000;
	}
	else if (cave_have_flag_bold(y, x, FF_ICE_WALL))//�X��
	{
		prob = 50;
		food = 3000;
	}
	else if (cave_have_flag_bold(y, x, FF_NEEDLE))//�j
	{
		prob = 10;
		food = 2000;
	}
	else if (cave_have_flag_bold(y, x, FF_WALL))//��
	{
		prob = 0;//�Q�샂�[�h�ȊO�ł͋z�����܂Ȃ�
		food = 5000;
	}
	else if (cave_have_flag_bold(y, x, FF_PLATE)) //�M
	{
		prob = 90;
		food = 100;
	}
	else if (cave_have_flag_bold(y, x, FF_BOMB)) //���e
	{
		prob = 90;
		food = 5000;
	}

	else if (cave_have_flag_bold(y, x, FF_TRAP))//�g���b�v
	{
		//���Ƃ������z������ŏ��ɂȂ�̂��ςȘb���������������ʂ̃g���b�v������������
		prob = 75;
		food = 2000;
	}
	else
	{
		return FALSE;
	}

	if (YUMA_ULTIMATE_MODE) prob += 30;

	if (randint1(100) > prob) return FALSE;

	msg_format("%s���z�����񂾁I", f_name + f_info[get_feat_mimic(&cave[y][x])].name);

	//�n�`�ύX�ƍĕ`��
	cave_set_feat(y, x, feat_dirt);

	set_food(p_ptr->food + food);

	return TRUE;
}






 //�ޖ����Z�u���̐��ɑ��݂��Ă͂Ȃ�Ȃ��\�H�v
 //���̈ʒu�𒆐S�ɉQ�����1�O���b�h�����W�����߂Ă����̃A�C�e���E�����X�^�[�E�n�`��yuma_swallow_***()�ŏ���
 //������ł��z�����񂾂�TRUE��Ԃ�
bool yuma_swallow(int rad)
{

	bool flag_eat_sth = FALSE;

	int xx, yy, i, j;

	//@�̈ʒu �����X�^�[�͂���Ƃ��Ă��R�撆�Ȃ̂ŏ������Ȃ�
	if (yuma_swallow_item(py, px, rad)) flag_eat_sth = TRUE;
	if (yuma_swallow_terrain(py, px, rad)) flag_eat_sth = TRUE;


	//@�̎��͂��Q������ɏ������Ă���
	//�����X�^�[��|��������ɂ��̃����X�^�[�̃h���b�v�A�C�e�����z����Ǝg���Â炢�̂ŁA
	//�A�C�e���������X�^�[���n�`�̏��ɏ�������

	//�A�C�e��
	for (i = 1; i <= rad; i++)
	{
		//�E�����E�ォ��1�O���b�h���܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i;
			yy = py - i + j;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//�E�と���ォ��1�O���b�h�E�܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i - j;
			yy = py + i;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//���と��������1�O���b�h��܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i;
			yy = py + i - j;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//�������E������1�O���b�h���܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i + j;
			yy = py - i;
			if (yuma_swallow_item(yy, xx, rad)) flag_eat_sth = TRUE;
		}
	}


	//�����X�^�[
	for (i = 1; i <= rad; i++)
	{
		//�E�����E�ォ��1�O���b�h���܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i;
			yy = py - i + j;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//�E�と���ォ��1�O���b�h�E�܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i - j;
			yy = py + i;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//���と��������1�O���b�h��܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i;
			yy = py + i - j;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//�������E������1�O���b�h���܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i + j;
			yy = py - i;
			if (yuma_swallow_mon(yy, xx, rad)) flag_eat_sth = TRUE;

		}
	}

	//�n�`
	for (i = 1; i <= rad; i++)
	{
		//�E�����E�ォ��1�O���b�h���܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i;
			yy = py - i + j;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//�E�と���ォ��1�O���b�h�E�܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px + i - j;
			yy = py + i;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//���と��������1�O���b�h��܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i;
			yy = py + i - j;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;
		}
		//�������E������1�O���b�h���܂�
		for (j = 0; j<rad + 1; j++)
		{
			xx = px - i + j;
			yy = py - i;
			if (yuma_swallow_terrain(yy, xx, rad)) flag_eat_sth = TRUE;

		}
	}


	return flag_eat_sth;

}



//v2.0.6 �ޖ��̍ŏI���[�h����
//process_player()�̍s�������肩��R�}���h���͂̊Ԃɍs���B
//�����x���Q���ԂɂȂ����Ƃ��Ap_ptr->special_defense��SD_UNIQUE_CLASS_POWER���Z�b�g���ꑀ��s�\�ɂȂ�B
//���̊Ԃ͋z�����ݓ��Z������ɔ�����������B
//�z�����ݓ��Z����������Ƃ��s���������TRUE��Ԃ�
bool check_yuma_ultimate_mode(void)
{
	int rad;

	static int countdown = 0;

	if (p_ptr->pclass != CLASS_YUMA) return FALSE;

	if (p_ptr->is_dead) return FALSE;

	//�q�������̍Œ��͂Ȃɂ����Ȃ�
	if (p_ptr->inside_battle) return FALSE;

	//�Q���Ԃ̂Ƃ�����t���O�𗧂Ă�
	if (p_ptr->food < PY_FOOD_FAINT)
	{
		if (!(p_ptr->special_defense & SD_UNIQUE_CLASS_POWER))
		{
			msg_print("���Ȃ��͎��͂̂��̂𖳍��ʂɋz�����ݎn�߂��I");
			countdown = 100;
		}
		else
			msg_print("���͂̂��̂��z������ł���...");

		p_ptr->special_defense |= SD_UNIQUE_CLASS_POWER;
		p_ptr->redraw |= PR_MAP;
		redraw_stuff();
	}

	//�Q�삪��������Ă�����t���O�������Ă���Ԃ͐�֐i��
	if (!(p_ptr->special_defense & SD_UNIQUE_CLASS_POWER)) return FALSE;

	Term_flush();
	p_ptr->redraw |= PR_MAP;
	redraw_stuff();

	set_cut(0);
	set_stun(0);

	//�ʏ�̔{�͈̔�
	rad = (3 + p_ptr->lev / 7) * 2;

	//�ޖ��̋z�����ݓ��Z������ɔ�������B���̓��Z�ŋz�����ނ��̂��Ȃ��Ȃ�A�������x���ʏ�ȏ�ɂȂ����Ƃ��t���O���O���
	//�����x���񕜂��Ȃ������Ƃ��̓Q�[���I�[�o�[�܂ł��̂܂�
	//�����������������񂢂�Ɩ������[�v�ɂȂ�̂�100�s���Ŗ߂邱�Ƃɂ���
	if (!yuma_swallow(rad) && p_ptr->food >= PY_FOOD_FAINT || (--countdown) <= 0)
	{
		countdown = 0;
		p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
		p_ptr->redraw |= PR_MAP;
		redraw_stuff();
		msg_print("���Ȃ��͈ӎ������߂����B���͎͂S�邽��L�l��...");

		return FALSE;
	}

	p_ptr->energy_need = 100;
	return TRUE;

}






